#pragma once

#include <stdint.h>
#include <stddef.h>

#if defined (ayxiatrace_EXPORTS) && defined(_WIN32)
#  define TRACE_CLIENT_EXPORT __declspec(dllexport)
#else
#  define TRACE_CLIENT_EXPORT
#endif


#define AYX_TRACE_UNIQ_(x,l) x ## l
#define AYX_TRACE_UNIQ(x,l) AYX_TRACE_UNIQ_(x,l)

#if defined (__cplusplus)
extern "C" {
#endif

  typedef struct ayxia_trace_channel_
  {
    uint32_t level : 8;
    uint32_t lineno : 23;
    uint32_t channel_disable : 1;
    const char* channel;
    const char* file;
    const char* func;
    const char* format;
  } ayxia_trace_channel;

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
  };

  enum ayxia_trace_command
  {
    atc_initialize,
    atc_init_channel,
    atc_trace,
    atc_end_frame,
    atc_thread_name,
  };

  typedef struct ayxia_trace_arg_
  {
    const void* parg;
    enum ayxia_trace_type type;
  } ayxia_trace_arg;

  typedef struct ayxia_trace_initialize_
  {
    const char* process_name;
    size_t max_network_memory_kb;
    int allow_dropped_frames;
  } ayxia_trace_initialize;

  TRACE_CLIENT_EXPORT void ayxia_tc_initialize(const ayxia_trace_initialize* init);

  TRACE_CLIENT_EXPORT void ayxia_tc_shutdown();


  TRACE_CLIENT_EXPORT void ayxia_tc_trace(
    ayxia_trace_channel* channel,
    const ayxia_trace_arg* args,
    size_t nargs);

  TRACE_CLIENT_EXPORT void ayxia_tc_trace_varargs(
    const ayxia_trace_channel* channel,
    ...);

  TRACE_CLIENT_EXPORT void ayxia_tc_init_channel(const ayxia_trace_channel* channel);

  TRACE_CLIENT_EXPORT void ayxia_tc_end_frame_marker();

  TRACE_CLIENT_EXPORT void ayxia_tc_thread_name(const char * name);

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
    AYX_ARGTYPE(float, att_float32);
    AYX_ARGTYPE(double, att_float64);
    AYX_ARGTYPE(const char*, att_string);
    AYX_ARGTYPE(const wchar_t*, att_wstring);
    template<typename T, int N>
    struct argtype<T[N]> {
      static const ayxia_trace_type value = argtype<const T*>::value;
    };

    class Trace
    {
    public:
      Trace(int level, const char* channel, const char* file, const char * func, int lineno, const char *format)
      {
        _channel.level = level;
        _channel.channel = channel;
        _channel.file = file;
        _channel.func = func;
        _channel.lineno = lineno;
        _channel.format = format;
        ayxia_tc_init_channel(&_channel);
      }



      template<typename T>
      ayxia_trace_arg mkarg(const T& arg) const
      {
        ayxia_trace_arg res;
        res.parg = &arg;
        res.type = argtype<T>::value;
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


#define TRACE_INFO(channel, format, ...)  { \
  static ayxia::trace::Trace AYX_TRACE_UNIQ(ayx_trace_, __LINE__)(0, channel, __FILE__, __FUNCTION__, __LINE__, format); \
  AYX_TRACE_UNIQ(ayx_trace_, __LINE__)(__VA_ARGS__); }

#else

#define TRACE_INFO(ch, f, ...)  { \
    static ayxia_trace_channel AYX_TRACE_UNIQ(ayx_trace_, __LINE__) = { \
      .level = 0,.channel = ch,.file = __FILE__,.func = __FUNCTION__,.lineno = __LINE__,.format = f }; \
    if (!AYX_TRACE_UNIQ(ayx_trace_, __LINE__).channel_disable) \
      ayxia_tc_trace_varargs(&AYX_TRACE_UNIQ(ayx_trace_, __LINE__), __VA_ARGS__); \
}

#endif // __cplusplus