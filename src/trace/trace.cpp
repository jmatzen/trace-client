#include <trace/trace.h>
#include <memory>
#include "context.h"

namespace
{
  std::unique_ptr<ayxia::trace::Context> s_context;
}

TRACE_CLIENT_EXPORT void ayxia_tc_initialize()
{
  s_context = std::unique_ptr<ayxia::trace::Context>(new ayxia::trace::Context());
  s_context->Initialize();
}

TRACE_CLIENT_EXPORT void ayxia_tc_shutdown()
{
  s_context.reset();
}

TRACE_CLIENT_EXPORT void ayxia_tc_trace(const ayxia_trace_channel* channel, const ayxia_trace_arg* args, size_t nargs)
{
  s_context->Send(channel, args, nargs);
}

TRACE_CLIENT_EXPORT void ayxia_tc_trace_varargs(const ayxia_trace_channel* channel, ...)
{

}

TRACE_CLIENT_EXPORT void ayxia_tc_init_channel(const ayxia_trace_channel* channel)
{
  s_context->InitChannel(channel);
}


