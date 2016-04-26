#include <trace/trace.h>
#include <string>
#include <array>
#include <cstdarg>
#include <cstdlib>
#include <sstream>
#include <cctype>
#include <iomanip>

struct FormatSpec
{
  std::string format;
  int align;
  int prec;
};

FormatSpec parse_format_specifier(const std::string& fmt)
{
  enum State {
    s_begin,
    s_index,
    s_align,
    s_fmt,
    s_prec,
    s_done
  } state = s_begin;

  auto fs = FormatSpec();
  bool neg = false;

  for (auto ch : fmt) {
    switch (state) {
    case s_begin:
      if (ch == '{') {
        state = s_index;
      } else {
        return FormatSpec();
      }
      break;
    case s_index:
      if (isdigit(ch)) {
      } else if (ch == ',') {
        state = s_align;
        neg = false;
      } else if (ch == ':') {
        state = s_fmt;
      }
      break;
    case s_align:
      if (ch == '-')
        neg = true;
      else if (isdigit(ch)) {
        fs.align = fs.align * 10 + (ch - '0');
      } else {
        if (neg) fs.align *= -1;
        if (ch == ':') {
          state = s_fmt;
        } else {
          return FormatSpec();
        }
      }
    case s_fmt:
      if (isalpha(ch)) {
        fs.format.push_back(ch);
      } else if (isdigit(ch)) {
        fs.prec = (ch - '0');
        state = s_prec;
      } else {
        return FormatSpec();
      }
    case s_prec:
      if (isdigit(ch)) {
        fs.prec = fs.prec * 10 + (ch - '0');
      } else {
        return FormatSpec();
      }
    }
  }
  return fs;
}

template<typename T>
inline void format_arg(std::ostream& s, const std::string& fmt, const void* p)
{
  auto fs = parse_format_specifier(fmt);

  s << std::setprecision(fs.prec);

  s << std::dec;

  if (!fs.format.empty()) {
    if (fs.format == "x" || fs.format == "X") {
      s << std::hex;
    }
  }

  T value;
  memcpy(&value, p, sizeof(T));
  try
  {
    s << value;
  }
  catch (...)
  {
    return;
  }
}

inline void format_arg(std::ostream& s, const std::string& fmt, const ayxia_trace_arg*  args, size_t nargs)
{
  size_t index = atoi(fmt.c_str() + 1);
  if (index >= nargs)
    return;

  auto& arg = args[index];
  
  switch (arg.type) {
  case att_int8:
    format_arg<int8_t>(s, fmt, arg.parg); break;
  case att_uint8:
    format_arg<uint8_t>(s, fmt, arg.parg); break;
  case att_int16:
    format_arg<int16_t>(s, fmt, arg.parg); break;
  case att_uint16:
    format_arg<uint16_t>(s, fmt, arg.parg); break;
  case att_int32:
    format_arg<int32_t>(s, fmt, arg.parg); break;
  case att_uint32:
    format_arg<uint32_t>(s, fmt, arg.parg); break;
  case att_int64:
    format_arg<int64_t>(s, fmt, arg.parg); break;
  case att_uint64:
    format_arg<uint64_t>(s, fmt, arg.parg); break;
  case att_float32:
    format_arg<float>(s, fmt, arg.parg); break;
  case att_float64:
    format_arg<double>(s, fmt, arg.parg); break;
  case att_string:
    format_arg<const char*>(s, fmt, &arg.parg); break;
  case att_wstring:
    format_arg<const wchar_t*>(s, fmt, &arg.parg); break;
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

  enum State
  {
    s_default,
    s_formatspec,
    s_done,
  };

  std::stringstream stream;

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
      if (ch == '{') {
        spec.push_back(ch);
        state = s_formatspec;
      }
      else {
        stream << ch;
      }
      ch = *format++;
      break;
    case s_formatspec:
      spec.push_back(ch);
      if (ch == '}') {
        format_arg(stream, spec, args, nargs);
        state = s_default;
        spec.clear();
      }
      ch = *format++;
      break;
      case s_done:
        break;
    }
  }
  stream << ch;
  strncpy(s, stream.str().c_str(), slen);
}


