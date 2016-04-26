#include <UnitTest++/UnitTest++.h>
#include <trace/format.h>
#include <array>
#include <memory>
#include <string>
#include <algorithm>
#include <iostream>


template<typename T> void make_format_args(T) {}

template<typename T, typename Arg, typename... Args>
void make_format_args(T it, const Arg& arg, const Args& ...args)
{
  FormatArgImpl<Arg> farg(arg);
  *it++ = farg.get();
  make_format_args(it, args...);
}

template<typename ...Args>
std::string format(const char* format, const Args& ...args)
{
  std::array<ayxia_trace_arg, sizeof...(Args)> argsarr;
  make_format_args(argsarr.begin(), args...);
  std::array<char, 1024> buf;
  ayxia_tc_format(buf.data(), buf.size()-1, format, argsarr.data(), argsarr.size());
  return buf.data();
}

TEST(Test1)
{
  CHECK(format("test %d test", int8_t(0xff)) == "test -1 test");
  CHECK(format("%d", int16_t(0xffff)) == "-1");
  CHECK(format("%d", int32_t(0xffffffff)) == "-1");
  CHECK(format("%lld", int64_t(0xfeadbeefdeadbeefLL)) == "-95210079662522641");
}

TEST(Test2)
{
  CHECK(format("%u", uint8_t(~0U)) == "255");
  CHECK(format("%u", uint16_t(~0U)) == "65535");
  CHECK(format("%u", uint32_t(~0U)) == std::to_string(~0U));
  CHECK(format("%llu", uint64_t(~0ULL)) == std::to_string(~0ULL));
}

TEST(TestString)
{
  CHECK(format("this is %s a test", "not") == "this is not a test");
}

TEST(TestDouble)
{
  CHECK(format("%f", 3.14) == "3.140000");
}

TEST(TestFloat)
{
  CHECK(format("%f", 3.14159f) == "3.141590");
}

TEST(TestException)
{
  //CHECK(format("%s", 1) == "");
}

int main()
{
  UnitTest::RunAllTests();
}
