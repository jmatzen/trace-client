#include <UnitTest++/UnitTest++.h>
#include <trace/trace.h>
#include <array>

template<typename T>
void make_format_arg(T& container) {}

template<typename T, typename Arg>
void make_format_arg(T& container, const Arg& arg)
{
}

template<typename T, typename Arg, typename... Args>
void make_format_args(T& container, const Arg& arg, const Args& ...args)
{
  make_format_arg(container, arg);
  make_format_args(container, args...);
}

template<typename ...Args>
void format(const char* format, const Args& ...args)
{
  std::array<ayxia_trace_arg, sizeof...(Args)> argsarr;
  make_format_args(argsarr, args...);
}

TEST(Test1)
{
  format("%d %d", 32, 64);
}

int main()
{
  UnitTest::RunAllTests();
}
