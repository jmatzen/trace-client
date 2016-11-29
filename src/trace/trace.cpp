#include <trace/trace.h>
#include <memory>
#include <mutex>
#include <cstdio>
#include <utility>
#include <unordered_set>
#include "context.h"

namespace
{
  std::unique_ptr<ayxia::trace::Context> s_context;
  std::mutex s_mutex;
  std::unordered_set<size_t> s_channelSet;
}

TRACE_CLIENT_EXPORT int ayxia_tc_enable = 1;

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
  std::unique_lock<std::mutex> lk(s_mutex);
  s_channelSet.clear();
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
  if (!channel->cookie) {
    std::unique_lock<std::mutex> lk(s_mutex);
    if (!channel->cookie) {
      channel->cookie = uint32_t(std::hash<intptr_t>()(intptr_t(&channel)));
      ayxia_tc_init_channel(channel);
    }
  }
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
  s_context->InitChannel(*channel);
}

TRACE_CLIENT_EXPORT void ayxia_tc_start_frame()
{
  s_context->StartFrame();
}

TRACE_CLIENT_EXPORT void ayxia_tc_thread_name(const char * name)
{
  s_context->SetThreadName(name);
}

TRACE_CLIENT_EXPORT void ayxia_tc_simple_trace(
  ayxia_trace_level level, 
  const char* channel, 
  const char* message,
  const char* filename,
  int lineno)
{
  const size_t hash = std::hash<const char *>()(channel)
    ^ std::hash<const char *>()(filename)
    ^ std::hash<uint32_t>()(lineno)
    ^ std::hash<int>()((int)level);

  ayxia_trace_channel node{
    (uint32_t)level,
    (uint32_t)lineno,
    0,
    static_cast<uint32_t>(hash),
    channel,
    filename,
    "",
    "{0}"
  };

  {
    std::unique_lock<std::mutex> lk(s_mutex);
    if (s_channelSet.find(hash) == s_channelSet.end()) 
    {
      ayxia_tc_init_channel(&node);
      s_channelSet.emplace(hash);
    }
  }

  const ayxia_trace_arg arg = {
    message, att_string
  };

  ayxia_tc_trace(&node, &arg, 1);
}

