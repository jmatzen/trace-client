#include <trace/trace.h>
#include <string>
#include <array>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <cstdio>

template<typename T>
int format_arg(char* s, size_t slen, const std::string& fmt, const void* p)
{
  T val;
  memcpy(&val, p, sizeof(T));
  try
  {
    return snprintf(s, slen, fmt.c_str(), val);
  }
  catch (...)
  {
    return -1;
  }
}

inline int format_arg(char* s, size_t slen, const std::string& fmt, ayxia_trace_arg const& arg)
{
  switch (arg.type) {
  case att_int8:
    return format_arg<int8_t>(s, slen, fmt, arg.parg); break;
  case att_uint8:
    return format_arg<uint8_t>(s, slen, fmt, arg.parg); break;
  case att_int16:
    return format_arg<int16_t>(s, slen, fmt, arg.parg); break;
  case att_uint16:
    return format_arg<uint16_t>(s, slen, fmt, arg.parg); break;
  case att_int32:
    return format_arg<int32_t>(s, slen, fmt, arg.parg); break;
  case att_uint32:
    return format_arg<uint32_t>(s, slen, fmt, arg.parg); break;
  case att_int64:
    return format_arg<int64_t>(s, slen, fmt, arg.parg); break;
  case att_uint64:
    return format_arg<uint64_t>(s, slen, fmt, arg.parg); break;
  case att_float32:
    return format_arg<float>(s, slen, fmt, arg.parg); break;
  case att_float64:
    return format_arg<double>(s, slen, fmt, arg.parg); break;
  case att_string:
    return format_arg<const char*>(s, slen, fmt, &arg.parg); break;
  case att_wstring:
    return format_arg<const wchar_t*>(s, slen, fmt, &arg.parg); break;
  default:
    break;
  }
  return -1;
}

extern"C" void ayxia_tc_format(
  char* s,
  size_t slen,
  const char* const format_, 
  const ayxia_trace_arg* args, 
  size_t nargs)
{
  const char* format = format_;
  const char * const end = s + slen;
  char* ptr = s;

  enum State
  {
    s_default,
    s_formatspec,
    s_done,
  };

  std::string spec;

  State state = s_default;
  char ch = *format++;
  while (state != s_done && ptr < end)
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
      else {
        *ptr++ = ch;
      }
      ch = *format++;
      break;
    case s_formatspec:
      spec.push_back(ch);
      if (strchr("diuoxXfFeEgGaAcspn", ch)) {
        int bc = format_arg(ptr, end - ptr, spec, *args++);
        if (bc == -1)
          return;
        ptr += bc;
        state = s_default;
        spec.clear();
      }
      ch = *format++;
      break;
      case s_done:
        break;
    }
  }
  *ptr = '\0';
}


