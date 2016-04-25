#include <UnitTest++/UnitTest++.h>
#include <trace/format.h>
#include <array>


template<typename T, typename Arg>
void make_format_arg(T it, const Arg& arg)
{
  *it = std::make_unique<FormatArgImpl<T>>(arg);
}

template<typename T> void make_format_args(T) {}

template<typename T, typename Arg, typename... Args>
void make_format_args(T it, const Arg& arg, const Args& ...args)
{
  make_format_arg(it++, arg);
  make_format_args(it, args...);
}

template<typename ...Args>
void format(const char* format, const Args& ...args)
{
  std::array<std::unique_ptr<FormatArg>, sizeof...(Args)> argsarr;
  make_format_args(argsarr.begin(), args...);
}

TEST(Test1)
{
  format("%d %d", 32, 64);
}

int main()
{
  UnitTest::RunAllTests();
}
