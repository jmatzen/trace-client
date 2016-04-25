#pragma once

#include <trace/trace.h>

struct FormatArg
{
  ayxia_trace_type type;

  FormatArg(ayxia_trace_type type_)
    : type(type_) {}

  virtual ~FormatArg() {}
};


template<typename T>
struct FormatArgTraits
{
};

template<typename T> struct FormatArgType;

template<> struct FormatArgType<int8_t> {
  static const ayxia_trace_type value = att_int8;
};

template<> struct FormatArgType<char> {
  static const ayxia_trace_type value = att_int8;
};


template<> struct FormatArgType<uint8_t> {
  static const ayxia_trace_type value = att_uint8;
};

template<> struct FormatArgType<int32_t> {
  static const ayxia_trace_type value = att_int32;
};


template<> struct FormatArgType<const char*> {
  static const ayxia_trace_type value = att_string;
};

template<int N> struct FormatArgType<char[N]> {
  static const ayxia_trace_type value = att_string;
};



template<typename T>
class FormatArgImpl
  : public FormatArg
{
public:
  FormatArgImpl(const T& val)
    : FormatArg(FormatArgType<T>::value)
    , m_val(val)  {  }

  auto get() const {
    return ayxia_trace_arg {
      &m_val,
      type
    };
  }

private:
  const T& m_val;
};

template<typename T>
class FormatArgImpl<T*>
  : public FormatArg
{
public:
  FormatArgImpl(T* val)
    : FormatArg(FormatArgType<T*>::value)
    , m_val(val)
    , m_len(length(val)) {}
  static size_t length(const T* p);

  auto get() const {
    return ayxia_trace_arg{
      m_val, type
    };
  }
private:
  const T* m_val;
  size_t m_len;
};

template<>
size_t FormatArgImpl<const char*>::length(const char* p)
{
  return strlen(p);
}

template<>
size_t FormatArgImpl<const wchar_t*>::length(const wchar_t* p)
{
  return wcslen(p);
}

std::string format_string(const char* format, const FormatArg* args, size_t argn);
