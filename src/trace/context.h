#pragma once

#include <trace/trace.h>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <uv.h>

namespace ayxia
{
  namespace trace
  {
    class Context
    {
    public:
      Context();

      ~Context();

      void SendTrace(ayxia_trace_channel& channel, const ayxia_trace_arg* args, size_t nargs);

      void InitChannel(const ayxia_trace_channel* channel);

      void Initialize();

    private:

      void ThreadEntryPoint();

      void SendToLogger(ayxia_trace_command command, const char* p, size_t len);

      void OnSignal();

      void OnTimer();

      void Flush();

      void EnableLogging(bool enable);

      void OnConnect(uv_connect_t* con, int status);

      void OnRead(uv_tcp_t* stream, ssize_t nread, const uv_buf_t* buf);

      void OnClose(uv_tcp_t* stream);

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
    };
  }
}
