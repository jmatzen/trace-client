#include <trace/trace.h>
#include <memory>
#include <mutex>
#include <cstdio>
#include "context.h"

namespace
{
  std::unique_ptr<ayxia::trace::Context> s_context;
  std::mutex s_mutex;
}

TRACE_CLIENT_EXPORT void ayxia_tc_initialize(const ayxia_trace_initialize* init)
{
  std::unique_lock<std::mutex> lk(s_mutex);
  if (!s_context) {
    s_context = std::unique_ptr<ayxia::trace::Context>(new ayxia::trace::Context(init?*init:ayxia_trace_initialize()));
    s_context->Initialize();
  }
}

TRACE_CLIENT_EXPORT void ayxia_tc_shutdown()
{
  s_context.reset();
}

TRACE_CLIENT_EXPORT void ayxia_tc_trace(ayxia_trace_channel* channel, const ayxia_trace_arg* args, size_t nargs)
{
  if (channel)
    s_context->SendTrace(*channel, args, nargs);
}

TRACE_CLIENT_EXPORT void ayxia_tc_trace_varargs(
  ayxia_trace_channel* channel, 
  const char* format, ...)
{
  va_list lst;
  va_start(lst, format);
  char buffer[1024];
  vsnprintf(buffer, 1024, format, lst);
  ayxia_trace_arg arg = 
  {
    buffer, att_string
  };
  ayxia_tc_trace(channel, &arg, 1);
}

TRACE_CLIENT_EXPORT void ayxia_tc_init_channel(ayxia_trace_channel* channel)
{
  s_context->InitChannel(channel);
}

TRACE_CLIENT_EXPORT void ayxia_tc_end_frame_marker()
{
  s_context->EndFrameMarker();
}

TRACE_CLIENT_EXPORT void ayxia_tc_thread_name(const char * name)
{
  s_context->SetThreadName(name);
}

TRACE_CLIENT_EXPORT void ayxia_tc_type_trace(const char* typestr, const char* message)
{
  s_context->TypeTrace(typestr, message);
}

