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


template<typename T> struct FormatArgImpl
  : public FormatArg
  , public FormatArgTraits<T>
{
  T pval;
  FormatArgImpl(ayxia_trace_type type_, const uint8_t* p)
    : FormatArg(type_)
  {
    memcpy(&pval, p, sizeof(T));
  }
};

template<typename T> struct FormatArgImpl<T*>
  : public FormatArg
  , public FormatArgTraits<T*>
{
  const T* pval;
  size_t len;

  FormatArgImpl(ayxia_trace_type type_, const uint8_t* p, size_t len_)
    : FormatArg(type_)
    , len(len_)
  {
    pval = reinterpret_cast<const T*>(p);
  }

};

