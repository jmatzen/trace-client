#include <UnitTest++/UnitTest++.h>
#include <trace/format.h>
#include <array>
#include <memory>
#include <algorithm>



template<typename T> void make_format_args(T) {}

template<typename T, typename Arg, typename... Args>
void make_format_args(T it, const Arg& arg, const Args& ...args)
{
  FormatArgImpl<Arg> farg(arg);
  *it++ = farg.get();
  make_format_args(it, args...);
}

template<typename ...Args>
auto format(const char* format, const Args& ...args)
{
  std::array<ayxia_trace_arg, sizeof...(Args)> argsarr;
  make_format_args(argsarr.begin(), args...);
  std::array<char, 1024> buf;
  return ayxia_tc_format(
    buf.data(), 
    buf.size(), 
    format, 
    argsarr.data(), 
    argsarr.size());
}

TEST(Test1)
{
  const char* p = "tmp";
  format("%d %d", (uint8_t)4, 32, "this is a tesT", (char)3, p);
}

int main()
{
  UnitTest::RunAllTests();
}
