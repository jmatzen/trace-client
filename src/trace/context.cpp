#include "context.h"
#include <chrono>
#include <cassert>
#include <array>
#include <type_traits>
#include <atomic>
#include <mutex>
#include <condition_variable>

#if defined _DEBUG
#  include <iostream>
//#  define DEBUG_LOG(s) do { std::cerr << s << std::endl; } while(0);
#  define DEBUG_LOG(s)
#else
#  define DEBUG_LOG(s)
#endif

#if ! defined _WIN32
#  include <unistd.h>
#endif

namespace
{
  const size_t kBufferSize = 65536;

#if defined _WIN32
  thread_local int32_t s_threadid = -1;
#else
  __thread int32_t s_threadid = -1;
#endif

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

  void allocator(uv_handle_t* , size_t size, uv_buf_t* buf)
  {
    buf->base = new char[size];
    buf->len = unsigned(size);
  }
}


void ayxia::trace::Context::SendTrace(const ayxia_trace_channel& channel, const ayxia_trace_arg* args, size_t nargs)
{
  if (!m_loggingEnabled) {
    const_cast<ayxia_trace_channel&>(channel).channel_disable = 1;
    return;
  }
  
  if (s_threadid == -1) {
#if defined _WIN32
    s_threadid = GetCurrentThreadId();
#else
    s_threadid = getpid();
#endif
  }


  std::array<char, 4096> buf;
  auto ptr = buf.data();
  if (channel.cookie)
    ptr = write_buffer(ptr, uint64_t(channel.cookie));
  else
    ptr = write_buffer(ptr, uint64_t(&channel));
  ptr = write_buffer(ptr, GetTimestamp());
  ptr = write_buffer(ptr, s_threadid);
  ptr = write_buffer(ptr, uint8_t(nargs));

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

  SendToLogger(atc_trace, buf.data(), ptr - buf.data());
}

void ayxia::trace::Context::InitChannel(ayxia_trace_channel& channel)
{

  if (!m_loggingEnabled)
    return;

  // fix up the source file name
  const char* delim = strrchr(channel.file, '\\');
  if (!delim)
    delim = strrchr(channel.file, '/');
  if (delim)
    channel.file = ++delim;

  std::string chname = channel.channel;
  // fix up the channel name;
  while (!chname.empty() && chname.back() == '.') chname.pop_back();

  std::array<char, 4096> buf;
  auto p = buf.data();
  if (channel.cookie)
    p = write_buffer(p, uint64_t(channel.cookie));
  else
    p = write_buffer(p, uint64_t(&channel));
  p = write_buffer(p, uint8_t(channel.level));
  p = write_buffer(p, uint16_t(channel.lineno));
  p = write_buffer(p, chname.c_str());
  p = write_buffer(p, channel.file);
  p = write_buffer(p, channel.func);
  p = write_buffer(p, channel.format);
  SendToLogger(atc_init_channel, buf.data(), p - buf.data());
}

void ayxia::trace::Context::Initialize()
{
  {
    std::unique_lock<std::mutex> lk(m_mutex);
    m_thread = std::thread(std::bind(&Context::ThreadEntryPoint, this));
    DEBUG_LOG("waiting for thread creation to complete.");
    m_condvar.wait(lk);
  }
  std::array<char, 1024> buf;
  auto p = buf.data();
  p = write_buffer(p, m_processName.c_str());
  SendToLogger(atc_initialize, buf.data(), p - buf.data());
}

void ayxia::trace::Context::StartFrame()
{
  TimestampT ts = GetTimestamp();
  SendToLogger(atc_start_frame, (char*)&ts, sizeof(TimestampT));
}

void ayxia::trace::Context::SetThreadName(const char * name)
{
  auto p = (char*)alloca(strlen(name) + 2 + sizeof(s_threadid));
  auto q = p;
  q = write_buffer(q, s_threadid);
  q = write_buffer(p, name);
  SendToLogger(atc_thread_name, p, q - p);
}

void ayxia::trace::Context::SendTrace(
  ayxia_trace_level level,
  const char * channelName,
  const char * message)
{
  auto channel = ayxia_trace_channel();
  channel.channel = channelName;
  channel.level = level;
  channel.file = "";
  channel.func = "";
  channel.format = "{0}";
  channel.cookie = uint32_t(std::hash<const char*>()(channelName));
  channel.cookie ^= uint32_t(std::hash<int>()(level));
  {
      std::unique_lock<std::mutex> lk(m_channelSetMutex);
      if (m_channelSet.insert(channel.cookie).second)
      {
        InitChannel(channel);
      }
  }
  ayxia_trace_arg arg = {
    message, att_string
  };
  SendTrace(channel, &arg, 1);
}

void ayxia::trace::Context::ThreadEntryPoint()
{
  //initialize libuv
  uv_loop_init(&m_uvLoop);

  // initialize async event handler
  uv_async_init(&m_uvLoop, &m_uvSignal, [](uv_async_t* as)
  {
    ((Context*)as->data)->OnSignal();
  });
  m_uvSignal.data = this;

  uv_timer_init(&m_uvLoop, &m_uvTimer);
  m_uvTimer.data = this;


  // this magically remains on the stack
  auto getaddr_req = uv_getaddrinfo_t();
  getaddr_req.data = this;
  
  auto ai = addrinfo();
  
  uv_getaddrinfo(
                 &m_uvLoop,
                 &getaddr_req,
                 [](uv_getaddrinfo_t* req, int status, addrinfo* res)
                 {
                   ((Context*)req->data)->OnGetAddrInfo(status, res);
                   uv_freeaddrinfo(res);
                 },
                 m_remoteHost.c_str(),
                 0,
                 &ai);



  uv_run(&m_uvLoop, UV_RUN_DEFAULT);

  DEBUG_LOG("exiting network thread");
}

void ayxia::trace::Context::OnGetAddrInfo(int status,const addrinfo *addr)
{
  const addrinfo* theone = 0;

  for (; addr; addr = addr->ai_next) {
    if (addr->ai_family == AF_INET) {
      theone = addr;
    }/* else if (addr->ai_family == AF_INET6) {
      theone = addr;
      break;
    }*/
  }

  if (theone)
  {
    union 
    {
      sockaddr* saddr;
      sockaddr_in* sin4;
      sockaddr_in6* sin6;
    };

    saddr = (sockaddr*)alloca(theone->ai_addrlen);
    memcpy(saddr, theone->ai_addr, theone->ai_addrlen);

    if (theone->ai_family == AF_INET)
    {
      sin4->sin_port = htons(8372);
    }
    else
    {
      sin6->sin6_port = htons(8372);
    }

    // initialize the connection to the server
    auto stream = new uv_tcp_t();
    uv_tcp_init(&m_uvLoop, stream);
    stream->data = this;

    auto con = new uv_connect_t();
    con->data = stream;

    uv_tcp_connect(con, stream, saddr, [](uv_connect_t* const con, int status)
    {
      auto stream = (uv_tcp_t*)con->data;
      auto context = (Context*)stream->data;
      context->OnConnect(con, status);
    });
  }
}

void ayxia::trace::Context::OnConnect(uv_connect_t* con, int status)
{
  EnableLogging(status == 0);
  auto stream = (uv_tcp_t*)con->data;
  if (status != 0)
  {
    DEBUG_LOG(uv_strerror(status));
    uv_close((uv_handle_t*)stream, [](uv_handle_t* stream)
    {
      delete stream;
    });
  }
  else
  {
    DEBUG_LOG("Connected to log viewer.");
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

void ayxia::trace::Context::OnRead(uv_tcp_t * stream, ssize_t nread, const uv_buf_t * /*buf*/)
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

ayxia::trace::Context::TimestampT ayxia::trace::Context::GetTimestamp()
{
#if defined _WIN32
  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);
  uint64_t timestamp = counter.QuadPart - m_timestampBaseTime;
  timestamp = (timestamp * 1000000000) / m_highResTimerFrequency;
  return timestamp;
#else
  auto timespan(std::chrono::high_resolution_clock::now() - m_timestampBaseTime);
  auto nanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(timespan);
  return nanosecs.count();
#endif
  
}

void ayxia::trace::Context::SendToLogger(
  ayxia_trace_command command, const char * p, size_t len)
{
  std::unique_lock<std::mutex> lk(m_mutex);
  if (m_buffer.size() + len + sizeof(uint16_t) > kBufferSize) {
    uv_async_send(&m_uvSignal);
    m_condvar.wait(lk);
  }
  m_buffer.insert(m_buffer.end(), static_cast<char>(command));
  uint16_t len_ = static_cast<uint16_t>(len);
  m_buffer.insert(m_buffer.end(), (char*)&len_, (char*)&len_ + sizeof(len_));
  m_buffer.insert(m_buffer.end(), p, p + len);
}

void ayxia::trace::Context::OnSignal()
{
  DEBUG_LOG("OnSignal");
  Flush();

  if (!m_thread.joinable())
  {
    uv_close((uv_handle_t*)&m_uvSignal, nullptr);
    uv_close((uv_handle_t*)&m_uvTimer, nullptr);
    if (m_uvStream)
      uv_close((uv_handle_t*)m_uvStream.get(), nullptr);
  }
}

void ayxia::trace::Context::Flush()
{
  std::vector<char> tmp;
  tmp.reserve(kBufferSize);

  struct write_ctx
    : public uv_write_t
    , public uv_buf_t 
  {
  };

  if (!m_buffer.empty()) 
  {
    {
      std::unique_lock<std::mutex> lk(m_mutex);
      std::swap(tmp, m_buffer);
      DEBUG_LOG("Flushed " << tmp.size() << " bytes");
      m_condvar.notify_all();
    }
    auto ctx = new write_ctx();
    ctx->len = unsigned(tmp.size());
    ctx->base = new char[tmp.size()];
    memcpy(ctx->base, tmp.data(), tmp.size());

    uv_write(ctx,
      (uv_stream_t*)m_uvStream.get(), 
      static_cast<uv_buf_t*>(ctx), 
      1,
      [](uv_write_t* req, int status)
    {
      auto ctx = static_cast<write_ctx*>(req);
      DEBUG_LOG("[status="<<status<<"] deleting " << ctx->len << " bytes");
      delete[](char*)ctx->base;
      delete ctx;
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
  DEBUG_LOG("tearing down connection context");
  // tear down thread and network library


  assert(m_thread.joinable());

  if (m_thread.joinable())
  {
    // move the thread object
    auto thread = std::move(m_thread);

    // signal the network loop
    uv_async_send(&m_uvSignal);

    DEBUG_LOG("waiting for thread to terminate");
    // wait for thread to terminate
    thread.join();

  }

  uv_loop_close(&m_uvLoop);

  DEBUG_LOG("done.");
}

ayxia::trace::Context::Context(const ayxia_trace_initialize& init)
  : m_loggingEnabled(false)
  , m_uvLoop()
  , m_remoteHost(init.remote_host ? init.remote_host : "localhost")
  , m_processName(init.process_name ? init.process_name : "")
{
  m_buffer.reserve(kBufferSize);

#if defined _WIN32
  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);
  m_timestampBaseTime = counter.QuadPart;

  LARGE_INTEGER freq;
  QueryPerformanceFrequency(&freq);
  m_highResTimerFrequency = freq.QuadPart;
#else
  m_timestampBaseTime = std::chrono::high_resolution_clock::now();
#endif
}
