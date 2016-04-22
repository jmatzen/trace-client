#include "context.h"
#include <chrono>
#include <cassert>
#include <type_traits>

#if defined _DEBUG
#  include <iostream>
#  define DEBUG_LOG(s) do { std::cerr << s << std::endl; } while(0);
#else
#  define DEBUG_LOG(s)
#endif

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
    return p + len*sizeof(wchar_t);
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

  void allocator(uv_handle_t* handle, size_t size, uv_buf_t* buf)
  {
    buf->base = new char[size];
    buf->len = size;
  }
}


void ayxia::trace::Context::SendTrace(const ayxia_trace_channel* channel, const ayxia_trace_arg* args, size_t nargs)
{
  if (!m_loggingEnabled)
    return;

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
      args_size += it->parg ? wcslen((wchar_t*)it->parg) * sizeof(wchar_t) + 2 : 0;
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
  if (!m_loggingEnabled)
    return;

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


  // initialize the connection to the server
  auto stream = new uv_tcp_t();
  uv_tcp_init(m_uvLoop, stream);
  stream->data = this;

  auto con = new uv_connect_t();
  con->data = stream;

  auto sin = sockaddr_in6();
  sin.sin6_family = AF_INET6;
  sin.sin6_port = htons(1333);
  inet_pton(AF_INET6, "::1", &sin.sin6_addr);

  uv_tcp_connect(con, stream, (sockaddr*)&sin, [](uv_connect_t* const con, int status)
  {
    auto stream = (uv_tcp_t*)con->data;
    auto context = (Context*)stream->data;
    context->OnConnect(con, status);
  });


  uv_run(m_uvLoop, UV_RUN_DEFAULT);
}

void ayxia::trace::Context::OnConnect(uv_connect_t* con, int status)
{
  EnableLogging(status == 0);
  auto stream = (uv_tcp_t*)con->data;
  if (status != 0)
  {
    delete stream;
  }
  else
  {
    m_uvStream.reset(stream);

    uv_read_start((uv_stream_t*)stream, allocator, [](uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
    {
      ((Context*)stream->data)->OnRead((uv_tcp_t*)stream, nread, buf);
    });
  }


  delete con;

  std::unique_lock<std::mutex> lk(m_mutex);
  m_condvar.notify_all();
}

void ayxia::trace::Context::OnRead(uv_tcp_t * stream, ssize_t nread, const uv_buf_t * buf)
{
  assert(stream == m_uvStream.get());
  if (nread <= 0)
  {
    uv_close((uv_handle_t*)stream, [](uv_handle_t* h)
    {
      ((Context*)h->data)->OnClose((uv_tcp_t*)h);
    });
  }
}

void ayxia::trace::Context::OnClose(uv_tcp_t* stream)
{
  assert(stream == m_uvStream.get());
  m_uvStream.reset();
}

void ayxia::trace::Context::SendToLogger(const char * p, size_t len)
{
  std::unique_lock<std::mutex> lk(m_mutex);
  if (m_buffer.size() + len + sizeof(uint16_t) > kBufferSize) {
    uv_async_send(&m_uvSignal);
    m_condvar.wait(lk);
  }
  uint16_t len_;
  m_buffer.insert(m_buffer.end(), (char*)&len_, (char*)&len_ + sizeof(len_));
  m_buffer.insert(m_buffer.end(), p, p + len);
}

void ayxia::trace::Context::OnSignal()
{
  DEBUG_LOG("OnSignal");
  Flush();

  // check thread joinable state and stop libuv if
  // we're tearing down
  if (!m_thread.joinable())
  {
    uv_stop(m_uvLoop);
  }
}

void ayxia::trace::Context::Flush()
{
  std::vector<char> tmp;
  tmp.reserve(kBufferSize);

  if (!m_buffer.empty()) 
  {
    {
      std::unique_lock<std::mutex> lk(m_mutex);
      std::swap(tmp, m_buffer);
      DEBUG_LOG("Flushed " << tmp.size() << " bytes");
      m_condvar.notify_all();
    }
    uv_buf_t buf;
    buf.len = tmp.size();
    buf.base = tmp.data();

    auto write_req = new uv_write_t();
    write_req->data = buf.base;

    //typedef void (*uv_write_cb)(uv_write_t* req, int status);

    uv_write(write_req, (uv_stream_t*)m_uvStream.get(), &buf, 1, [](uv_write_t* req, int status)
    {
      delete[] (char*)req->data;
      delete req;
    });
  }
  tmp.clear();
}

void ayxia::trace::Context::EnableLogging(bool enable)
{
  m_loggingEnabled = enable;

  if (enable)
  {
    // start libuv timer
    uv_timer_start(&m_uvTimer, [](uv_timer_t* const timer)
    {
      ((Context*)timer->data)->OnTimer();
    }, 0, 100);
  }
  else
  {
    uv_timer_stop(&m_uvTimer);
  }
}

void ayxia::trace::Context::OnTimer()
{
  DEBUG_LOG("OnTimer");
  Flush();
}

ayxia::trace::Context::~Context()
{
  // tear down thread and network library

  assert(m_thread.joinable());
  assert(m_uvLoop);

  if (m_thread.joinable())
  {
    // move the thread object
    auto thread = std::move(m_thread);

    // signal the network loop
    uv_async_send(&m_uvSignal);

    // wait for thread to terminate
    thread.join();

  }

  if (m_uvLoop)
  {
    uv_loop_close(m_uvLoop);
  }

}

ayxia::trace::Context::Context()
  : m_loggingEnabled(false)
  , m_uvLoop(0)
{
  //initialize libuv
  m_uvLoop = uv_loop_new();

  // initialize async event handler
  uv_async_init(m_uvLoop, &m_uvSignal, [](uv_async_t* as)
  {
    ((Context*)as->data)->OnSignal();
  });
  m_uvSignal.data = this;

  uv_timer_init(m_uvLoop, &m_uvTimer);
  m_uvTimer.data = this;

  m_buffer.reserve(kBufferSize);
}
