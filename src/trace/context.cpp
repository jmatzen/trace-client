#include "context.h"
#include <chrono>
#include <cassert>
#include <type_traits>

namespace
{
  const size_t kBufferSize = 65536;


  template<typename T>
  char* write_buffer(char* p, T arg) {
    memcpy(p, &arg, sizeof(T));
    return p + sizeof(T);
  }

  template<>
  char* write_buffer(char* p, const char* arg) {
    const uint16_t len = arg ? static_cast<uint16_t>(strlen(arg)) : 0xffff;
    p = write_buffer(p, len);
    if (arg) memcpy(p, arg, len);
    return p + len;
  }

  template<>
  char* write_buffer(char* p, const wchar_t* arg) {
    const uint16_t len = arg ? static_cast<uint16_t>(wcslen(arg)) : 0xffff;
    p = write_buffer(p, len);
    if (arg) memcpy(p, arg, len*sizeof(wchar_t));
    return p + len;
  }



  template<typename T>
  void serialize_arg(char*& p, const ayxia_trace_arg& arg) 
  {
    p = write_buffer(p, int8_t(arg.type));
    T value;
    memcpy(&value, arg.parg, sizeof(T));
    p = write_buffer<T>(p, value);
  }


  template<typename T>
  void serialize_arg_string(char*& p, const ayxia_trace_arg& arg)
  {
    p = write_buffer(p, int8_t(arg.type));
    p = write_buffer(p, (const T*)arg.parg);
  }
}


void ayxia::trace::Context::Send(const ayxia_trace_channel* channel, const ayxia_trace_arg* args, size_t nargs)
{
  size_t args_size = 0;

  for (auto it = args; it != args + nargs; ++it) {
    switch (it->type) {
    case att_uint8:
    case att_int8: args_size += 1; break;
    case att_uint16:
    case att_int16: args_size += 2; break;
    case att_uint32:
    case att_float32:
    case att_int32: args_size += 4; break;
    case att_uint64:
    case att_float64:
    case att_int64: args_size += 8; break;
    case att_string:
      args_size += it->parg ? strlen((char*)it->parg) + 2 : 0;
      break;
    case att_wstring:
      args_size += it->parg ? wcslen((wchar_t*)it->parg) + 2 : 0;
      break;
    default:
      abort();
    }
  }

  // TODO: use pool
  const size_t buflen = 1 + sizeof(uint64_t) + args_size + nargs;
  auto buf = (char*)alloca(buflen);
  auto ptr = buf;
  ptr = write_buffer(ptr, uint8_t(atc_trace));
  ptr = write_buffer(ptr, uint64_t(channel));

  for (auto it = args; it != args + nargs; ++it) {
    switch (it->type) {
    case att_int8: serialize_arg<int8_t>(ptr, *it); break;
    case att_uint8: serialize_arg<uint8_t>(ptr, *it); break;
    case att_uint16:serialize_arg<uint8_t>(ptr, *it); break;
    case att_int16:serialize_arg<int16_t>(ptr, *it); break;
    case att_uint32:serialize_arg<uint32_t>(ptr, *it); break;
    case att_float32:serialize_arg<float>(ptr, *it); break;
    case att_int32:serialize_arg<int32_t>(ptr, *it); break;
    case att_uint64:serialize_arg<uint64_t>(ptr, *it); break;
    case att_float64:serialize_arg<double>(ptr, *it); break;
    case att_int64:serialize_arg<int64_t>(ptr, *it); break;
    case att_string:serialize_arg_string<char>(ptr, *it); break;
    case att_wstring:serialize_arg_string<wchar_t>(ptr, *it); break;
    default: abort();
    }
  }

  assert(ptr == buf + buflen);
  SendToLogger(buf, buflen);
}

void ayxia::trace::Context::InitChannel(const ayxia_trace_channel * channel)
{
  const size_t bufsz =
    1
    + sizeof(uint64_t) // channel hash (address)
    + sizeof(uint8_t) // level
    + sizeof(uint16_t) // lineno
    + strlen(channel->channel) + 2
    + strlen(channel->file) + 2
    + strlen(channel->func) + 2
    + strlen(channel->format) + 2;
  auto buf = (char*)alloca(bufsz);
  auto p = buf;
  p = write_buffer(p, uint8_t(atc_init_channel));
  p = write_buffer(p, uint64_t(channel));
  p = write_buffer(p, uint8_t(channel->level));
  p = write_buffer(p, uint16_t(channel->lineno));
  p = write_buffer(p, channel->channel);
  p = write_buffer(p, channel->file);
  p = write_buffer(p, channel->func);
  p = write_buffer(p, channel->format);
  assert(p == buf + bufsz);
  SendToLogger(buf, bufsz);
}

void ayxia::trace::Context::Initialize()
{
  std::unique_lock<std::mutex> lk(m_mutex);
  m_thread = std::thread(std::bind(&Context::ThreadEntryPoint, this));
  m_condvar.wait(lk);
}

void ayxia::trace::Context::ThreadEntryPoint()
{
  {
    std::unique_lock<std::mutex> lk(m_mutex);
    m_condvar.notify_all();
  }

  std::vector<char> tmp;
  tmp.reserve(kBufferSize);

  while (m_thread.joinable())
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    {
      if (!m_buffer.empty()) {
        std::unique_lock<std::mutex> lk(m_mutex);
        std::swap(tmp, m_buffer);
        m_condvar.notify_all();
      }
    }
    tmp.clear();
  }
}

void ayxia::trace::Context::SendToLogger(const char * p, size_t len)
{
  std::unique_lock<std::mutex> lk(m_mutex);
  if (m_buffer.size() + len > kBufferSize) {
    m_condvar.wait(lk);
  }
  m_buffer.insert(m_buffer.end(), p, p + len);
}

ayxia::trace::Context::~Context()
{
  auto thread = std::move(m_thread);
  thread.join();
}

ayxia::trace::Context::Context()
{
  m_buffer.reserve(kBufferSize);
}
