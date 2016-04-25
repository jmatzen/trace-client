#include <trace/trace.h>
#include <string>
#include <array>
#include <cstdarg>

#if 0
template<typename T>
inline void format_arg_push(va_list& lst, const ayxia_trace_arg& arg)
{
  T val;
  memcpy(&val, arg.parg, sizeof(T));
  (intptr_t&)va_arg(lst, T) = val;
}

template<>
inline void format_arg_push<int64_t>(va_list& lst, const ayxia_trace_arg& arg)
{
  int64_t val;
  memcpy(&val, arg.parg, sizeof(int64_t));
  va_arg(lst, int64_t) = val;
}

template<typename T>
inline void format_arg_push_str(va_list& lst, const ayxia_trace_arg& arg)
{
  const T*  val = (T*)arg.parg;
  va_arg(lst, const T* ) = val;
}


template<>
inline void format_arg_push<float>(va_list& lst, const ayxia_trace_arg& arg)
{
  float val;
  memcpy(&val, arg.parg, sizeof(float));
  va_arg(lst, double) = static_cast<double>(val);
}
#endif

template<typename T, int N>
inline void format_arg_push(va_list& lst, const ayxia_trace_arg& arg)
{
}

template<typename T>
inline void format_arg_push<T, 8>(va_list& lst, const ayxia_trace_arg& arg)
{

}

template<typename T>
inline void format_arg_push(va_list& lst, const ayxia_trace_arg& arg)
{
  format_arg_push<T, sizeof(T)>(lst, arg);
}

template<typename T>
inline void format_arg_push_str(va_list& lst, const ayxia_trace_arg& arg)
{
}

inline void format_arg_push(va_list& lst, const ayxia_trace_arg& arg)
{
  switch (arg.type) 
  {
  case att_uint8:
    format_arg_push<uint8_t>(lst, arg);
    break;
  case att_int8:
    format_arg_push<int8_t>(lst, arg);
    break;
  case att_uint16:
    format_arg_push<uint16_t>(lst, arg);
    break;
  case att_int16:
    format_arg_push<int16_t>(lst, arg);
    break;
  case att_uint32:
    format_arg_push<uint32_t>(lst, arg);
    break;
  case att_int32:
    format_arg_push<int32_t>(lst, arg);
    break;
  case att_uint64:
    format_arg_push<uint64_t>(lst, arg);
    break;
  case att_int64:
    format_arg_push<int64_t>(lst, arg);
    break;
  case att_float32:
    format_arg_push<float>(lst, arg);
    break;
  case att_float64:
    format_arg_push<double>(lst, arg);
    break;
  case att_string:
    format_arg_push_str<char>(lst, arg);
    break;
  case att_wstring:
    format_arg_push_str<wchar_t>(lst, arg);
    break;
  default:
    break;
  }
}

extern"C" void ayxia_tc_format(
  char* s,
  size_t slen,
  const char* const format_, 
  const ayxia_trace_arg* args, 
  size_t nargs)
{
  const char* format = format_;

  auto const valist = reinterpret_cast<char*>(alloca(nargs * sizeof(uint64_t)));
  memset(valist, 0, nargs * sizeof(uint64_t));
  auto vaitem = valist;

  enum State
  {
    s_default,
    s_formatspec,
    s_done,
  };

  std::string spec;

  State state = s_default;
  char ch = *format++;
  while (state != s_done)
  {
    if (ch == '\0')
      state = s_done;
    switch (state)
    {
    case s_default:
      if (ch == '%') {
        spec.push_back(ch);
        state = s_formatspec;
      }
      ch = *format++;
      break;
    case s_formatspec:
      spec.push_back(ch);
      if (isalpha(ch)) {
        format_arg_push(vaitem, *args++);
        state = s_default;
        spec.clear();
      }
      ch = *format++;
      break;
    }
  }

  vsnprintf(s, slen, format_, valist);
}


