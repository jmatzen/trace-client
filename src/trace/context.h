#pragma once

#include <trace/trace.h>

namespace ayxia
{
  namespace trace
  {
    class Context
    {
    public:
      void Send(const ayxia_trace_block* block, const ayxia_trace_arg* args, size_t nargs);
    private:
    };
  }
}
