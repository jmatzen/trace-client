#pragma once

#include <trace/trace.h>
#include <thread>
#include <condition_variable>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <string>
#include <uv.h>

namespace ayxia
{
  namespace trace
  {
    class Context
    {
    public:
      Context(const ayxia_trace_initialize& init);

      ~Context();

      void SendTrace(ayxia_trace_channel& channel, const ayxia_trace_arg* args, size_t nargs);

      void InitChannel(ayxia_trace_channel* channel);

      void Initialize();

      void EndFrameMarker();

      void SetThreadName(const char* name);

      void TypeTrace(const char* typestr, const char* message);

    private:

      typedef uint64_t TimestampT;

      void ThreadEntryPoint();

      void SendToLogger(ayxia_trace_command command, const char* p, size_t len);

      void OnSignal();

      void OnTimer();

      void Flush();

      void EnableLogging(bool enable);

      void OnConnect(uv_connect_t* con, int status);

      void OnRead(uv_tcp_t* stream, ssize_t nread, const uv_buf_t* buf);

      void OnClose(uv_tcp_t* stream);
      
      void OnGetAddrInfo(int status, const addrinfo* result);

      TimestampT GetTimestamp();

    private:
      std::thread m_thread;
      std::mutex m_mutex;
      std::condition_variable m_condvar;
      std::vector<char> m_buffer;
      bool m_loggingEnabled;

      uv_loop_t m_uvLoop;
      uv_async_t m_uvSignal;
      uv_timer_t m_uvTimer;
      std::unique_ptr<uv_tcp_t> m_uvStream;
      uint64_t m_timestampBaseTime;
      
      std::string m_remoteHost;

      std::unordered_map<size_t, uint32_t> m_typeToChannelMap;
    };
  }
}
