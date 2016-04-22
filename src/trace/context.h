#pragma once

#include <trace/trace.h>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <vector>

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

    private:
      std::thread m_thread;
      std::mutex m_mutex;
      std::condition_variable m_condvar;
      std::vector<char> m_buffer;

    };
  }
}
