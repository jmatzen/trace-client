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

      void Send(const ayxia_trace_channel* channel, const ayxia_trace_arg* args, size_t nargs);

      void InitChannel(const ayxia_trace_channel* channel);

      void Initialize();

    private:

      void ThreadEntryPoint();

      void SendToLogger(const char* p, size_t len);

      void OnSignal();

      void OnTimer();

    private:
      std::thread m_thread;
      std::mutex m_mutex;
      std::condition_variable m_condvar;
      std::vector<char> m_buffer;

      uv_loop_t* m_uvLoop;
      uv_async_t m_uvSignal;
      uv_timer_t m_uvTimer;
    };
  }
}
