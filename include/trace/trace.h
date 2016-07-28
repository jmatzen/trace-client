#pragma once

#include <stdint.h>
#include <stddef.h>

#if defined (ayxiatrace_EXPORTS) && defined(_WIN32)
#  define TRACE_CLIENT_EXPORT __declspec(dllexport)
#  define TRACE_CLIENT_IMPORT __declspec(dllexport)
#else
#  define TRACE_CLIENT_EXPORT
#  define TRACE_CLIENT_IMPORT __declspec(dllimport)
#endif

#if defined (__cplusplus)
extern "C" {
#endif

  enum ayxia_trace_level
  {
    // atl_debug = 0, // placeholder
    atl_info = 0,
    atl_warning = 1,
    atl_error = 2,
  };

  enum ayxia_trace_type
  {
    att_int8,
    att_uint8,
    att_int16,
    att_uint16,
    att_int32,
    att_uint32,
    att_int64,
    att_uint64,
    att_float32,
    att_float64,
    att_string,
    att_wstring,
    att_pointer = 0x80,
  };

  enum ayxia_trace_command
  {
    atc_initialize,
    atc_init_channel,
    atc_trace,
    atc_start_frame,
    atc_thread_name,
  };

  enum initialization_flags
  {
    aif_none = 0,
  };

  typedef struct ayxia_trace_channel_
  {
    uint32_t level : 8;
    uint32_t lineno : 23;
    uint32_t channel_disable : 1; // out parameter
    uint32_t cookie; // out parameter
    const char* channel;
    const char* file;
    const char* func;
    const char* format;
  } ayxia_trace_channel;

  typedef struct ayxia_trace_context_
  {
    const char* name;
  } ayxia_trace_context;

  typedef struct ayxia_trace_arg_
  {
    const void* parg;
    enum ayxia_trace_type type;
  } ayxia_trace_arg;

  typedef struct ayxia_trace_initialize_
  {
    const char* remote_host;
    const char* process_name;
    uint32_t max_network_memory_kb;
    int flags;
  } ayxia_trace_initialize;

#if defined (__cplusplus)
  class TraceInitialize : public ayxia_trace_initialize
  {
  public:
    TraceInitialize(const char *RemoteHost,
      const char* ProcessNme,
      uint32_t MaxNetworkMemoryKb)
    {
      remote_host = RemoteHost;
      process_name = ProcessNme;
      max_network_memory_kb = MaxNetworkMemoryKb;
      flags = aif_none;
    }
  };
#endif

  TRACE_CLIENT_EXPORT void ayxia_tc_initialize(const ayxia_trace_initialize* init);

  TRACE_CLIENT_EXPORT void ayxia_tc_shutdown();


  TRACE_CLIENT_EXPORT void ayxia_tc_trace(
    ayxia_trace_channel* channel,
    const ayxia_trace_arg* args,
    size_t nargs);

  TRACE_CLIENT_EXPORT void ayxia_tc_trace_varargs(
    ayxia_trace_channel* channel,
    const char* format,
    ...);

  TRACE_CLIENT_EXPORT void ayxia_tc_init_channel(ayxia_trace_channel* channel);

  TRACE_CLIENT_EXPORT void ayxia_tc_start_frame();

  TRACE_CLIENT_EXPORT void ayxia_tc_thread_name(const char * name);

  TRACE_CLIENT_EXPORT void ayxia_tc_simple_trace(
    enum ayxia_trace_level level, 
    const char* channel, 
    const char* message);

  TRACE_CLIENT_EXPORT void ayxia_tc_enter_context(const ayxia_trace_context* context);

  TRACE_CLIENT_EXPORT void ayxia_tc_leave_context();

  TRACE_CLIENT_IMPORT extern int ayxia_tc_enable;

#if defined(__cplusplus)
}

namespace ayxia
{
  namespace trace
  {
    template<typename t> struct argtype;
    
#define AYX_ARGTYPE(type,att)      template<> struct argtype<type> {static const ayxia_trace_type value = att;};
    AYX_ARGTYPE(int8_t, att_int8);
    AYX_ARGTYPE(uint8_t, att_int8);
    AYX_ARGTYPE(int16_t, att_int16);
    AYX_ARGTYPE(uint16_t, att_uint16);
    AYX_ARGTYPE(int32_t, att_int32);
    AYX_ARGTYPE(uint32_t, att_uint32);
#if defined _MSC_VER
    AYX_ARGTYPE(long, att_int32);
    AYX_ARGTYPE(unsigned long, att_uint32);
#endif

    AYX_ARGTYPE(bool, att_string);
    AYX_ARGTYPE(int64_t, att_int64);
    AYX_ARGTYPE(uint64_t, att_uint64);
    AYX_ARGTYPE(float, att_float32);
    AYX_ARGTYPE(double, att_float64);
    AYX_ARGTYPE(const char*, att_string);
    AYX_ARGTYPE(const wchar_t*, att_wstring);
    template<typename T, int N>
    struct argtype<T[N]> {
      static const ayxia_trace_type value = argtype<const T*>::value;
    };

    class TraceContext
    {
    public:
      TraceContext(const char* name)
      {
        _context.name = name;
        //ayxia_tc_init_context(&_context);
      }
    private:
      ayxia_trace_context _context;
    };

    class Trace
    {
    public:
      Trace(int level, const char* channel, const char* file, const char * func, int lineno, const char *format)
      {
        _channel.level = level;
        _channel.channel = channel;
        _channel.cookie = 0;
        _channel.file = file;
        _channel.func = func;
        _channel.lineno = lineno;
        _channel.format = format;
        ayxia_tc_init_channel(&_channel);
      }

      template<typename T>
      static ayxia_trace_arg mkarg(const T& arg) 
      {
        ayxia_trace_arg res;
        res.parg = &arg;
        res.type = argtype<T>::value;
        return res;
      }

      template<typename T>
      static ayxia_trace_arg mkarg(T* ptr)  {
        ayxia_trace_arg res;
        res.parg = ptr;
        res.type = static_cast<ayxia_trace_type>(
          argtype<uint64_t>::value | att_pointer);
        return res;
      }

      static ayxia_trace_arg mkarg(const char* ptr) {
        ayxia_trace_arg res;
        res.parg = ptr;
        res.type = argtype<const char * >::value;
        return res;
      }

      static ayxia_trace_arg mkarg(bool arg)
      {
        ayxia_trace_arg res;
        res.parg = arg ? "true" : "false";
        res.type = argtype<bool>::value;
        return res;
      }

      void operator()() 
      {
        if (_channel.channel_disable) return;
        ayxia_tc_trace(&_channel, nullptr, 0);
      }

#if __cplusplus >= 201402L || _MSC_VER >= 1900

      template<typename... Args>
      void operator()(const Args& ...args_)
      {
        if (_channel.channel_disable) return;
        const ayxia_trace_arg args[] = { mkarg(args_)... };
        ayxia_tc_trace(&_channel, args, sizeof...(args_));
      }
#else
      template<typename A1>
      void operator()(const A1& a1) 
      {
        if (_channel.channel_disable) return;
        ayxia_trace_arg args[] = { mkarg(a1) };
        ayxia_tc_trace(&_channel, args, 1);
      }

      template<typename A1, typename A2>
      void operator()(const A1& a1, const A2& a2)  {
        if (_channel.channel_disable) return;
        ayxia_trace_arg args[] = {
          mkarg(a1), mkarg(a2)
        };
        ayxia_tc_trace(&_channel, args, 2);
      }

      template<typename A1, typename A2, typename  A3>
      void operator()(const A1& a1, const A2& a2, const A3& a3)  {
        if (_channel.channel_disable) return;
        ayxia_trace_arg args[] = {
          mkarg(a1), mkarg(a2), mkarg(a3)
        };
        ayxia_tc_trace(&_channel, args, 3);
      }

#endif

      ayxia_trace_channel _channel;
    };
  }
}


#define TRACE_LINE_(level, channel, format, ...)  if (ayxia_tc_enable) { \
  static ayxia::trace::Trace channel$(level, channel, __FILE__, __FUNCTION__, __LINE__, format); \
  channel$(__VA_ARGS__); }

#define TRACE_INFO(channel,format,...) TRACE_LINE_(atl_info, "root" ## "." ## channel, format, __VA_ARGS__)
#define TRACE_ERROR(channel,format,...) TRACE_LINE_(atl_error, "root" ## "." ## channel, format, __VA_ARGS__)
#define TRACE_WARNING(channel,format,...) TRACE_LINE_(atl_warning, "root" ## "." ## channel, format, __VA_ARGS__)

#else

#define TRACE_INFO(ch, f, ...)  if (ayxia_tc_enable) { \
    static ayxia_trace_channel channel$ = { \
      .level = 0,.channel = ch,.file = __FILE__,.func = __FUNCTION__,.lineno = __LINE__,.format = "{0}" }; \
    if (!channel$.channel_disable) \
      ayxia_tc_trace_varargs(&channel$, f, __VA_ARGS__); \
}

#endif // __cplusplus