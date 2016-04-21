#include <trace/trace.h>
#include <memory>
#include "context.h"

namespace
{
  std::unique_ptr<ayxia::trace::Context> s_context;
}

TRACE_CLIENT_EXPORT void ayxia_tc_initialize()
{
  s_context = std::make_unique<ayxia::trace::Context>();
}

TRACE_CLIENT_EXPORT void ayxia_tc_shutdown()
{
  s_context.reset();
}

TRACE_CLIENT_EXPORT void ayxia_tc_trace(const ayxia_trace_block* block, const ayxia_trace_arg* args, size_t nargs)
{
  s_context->Send(block, args, nargs);
}

TRACE_CLIENT_EXPORT void ayxia::trace::Trace::Send(const ayxia_trace_arg* args, size_t nargs) const
{
  ayxia_tc_trace(&_blk, args, nargs);
}


