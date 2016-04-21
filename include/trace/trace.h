#pragma once

#include <stdint.h>

#if defined (ayxiatrace_EXPORTS) && defined(_WIN32)
#  define TRACE_CLIENT_EXPORT __declspec(dllexport)
#else
#  define TRACE_CLIENT_EXPORT
#endif

#if defined (__cplusplus)
extern "C" {
#endif

TRACE_CLIENT_EXPORT void ayxia_tc_initialize();
TRACE_CLIENT_EXPORT void ayxia_tc_shutdown();

typedef struct ayxia_trace_block_
{
  int level;
  const char* channel;
  const char* file;
  const char* func;
  int lineno;
  const char* format;
} ayxia_trace_block;

enum ayxia_trace_type
{
  att_int8,
  att_int16,
  att_int32,
  att_int64,
  att_float32,
  att_float64,
  att_string,
  att_wstring,
};

typedef struct ayxia_trace_arg_
{
  const void* parg;
  enum ayxia_trace_type type;
} ayxia_trace_arg;

TRACE_CLIENT_EXPORT void ayxia_tc_trace(
  const ayxia_trace_block* block, 
  const ayxia_trace_arg* args,
  size_t nargs);


#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

namespace ayxia
{
  namespace trace
  {
    class Trace
    {
    public:
      Trace(int level, const char* channel, const char* file, const char * func, int lineno, const char *format)
      {
        _blk.level = level;
        _blk.channel = channel;
        _blk.file = file;
        _blk.func = func;
        _blk.lineno = lineno;
        _blk.format = format;
      }

      void operator()() const
      {
        Send(0, 0);
      }

      template<typename t> struct argtype;

      template<>
      struct argtype<int> {
        static const ayxia_trace_type value = att_int32;
      };

      template<typename T>
      ayxia_trace_arg mkarg(const T& arg) const
      {
        ayxia_trace_arg res;
        res.parg = &arg;
        res.type = argtype<T>::value;
        return res;
      }

      template<typename A1> 
      void operator()(const A1& a1) const
      {
        ayxia_trace_arg args[] = { mkarg(a1) };
        Send(args, 2);
      }

      template<typename A1, typename A2>
      void operator()(const A1& a1, const A2& a2) const {
        ayxia_trace_arg args[] = {
          mkarg(a1), mkarg(a2)
        };
        Send(args, 2);
      }

      TRACE_CLIENT_EXPORT void Send(const ayxia_trace_arg* args, size_t nargs) const;

    private:
      ayxia_trace_block _blk;

      union {
        struct {
          int _global_disable : 1;
          int _trace_disable : 1;
        };
        int _disable;
      };
    };
  }
}

#define AYX_TRACE_UNIQ_(x,l) x ## l
#define AYX_TRACE_UNIQ(x,l) AYX_TRACE_UNIQ_(x,l)

#define TRACE_INFO(channel, format, ...) \
  static ayxia::trace::Trace AYX_TRACE_UNIQ(ayx_trace_,__LINE__)(0, channel, __FILE__, __FUNCTION__, __LINE__, format); \
  AYX_TRACE_UNIQ(ayx_trace_,__LINE__)(__VA_ARGS__);

  

#endif // __cplusplus