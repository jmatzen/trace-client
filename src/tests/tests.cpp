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

TEST(TestInt)
{
  CHECK_EQUAL(format("{0}", int16_t(0xffff)), "-1");
  CHECK_EQUAL(format("{0}", int32_t(0xffffffff)), "-1");
  CHECK_EQUAL(format("{0}", int64_t(0xfeadbeefdeadbeefLL)), "-95210079662522641");
}

TEST(TestUInt)
{
  CHECK_EQUAL(format("{0}", uint16_t(~0U)), "65535");
  CHECK_EQUAL(format("{0}", uint32_t(~0U)), std::to_string(~0U));
  CHECK_EQUAL(format("{0}", uint64_t(~0ULL)), std::to_string(~0ULL));
}

TEST(TestString)
{
  CHECK_EQUAL(format("this is {0} a test", "not"), "this is not a test");
}

TEST(TestDouble)
{
  CHECK_EQUAL(format("{0}", 3.14), "3.14");
}

TEST(TestFloat)
{
  CHECK_EQUAL(format("{0}", 3.14159f), "3.14159");
}

TEST(TestHex)
{
  CHECK_EQUAL("0xffff", format("0x{0:x}", int16_t(0xffff)));
  CHECK_EQUAL(format("0x{0:x}", int32_t(0xffffffff)), "0xffffffff");
  CHECK_EQUAL(format("0x{0:x}", int64_t(0xfeadbeefdeadbeefLL)), "0xfeadbeefdeadbeef");
  CHECK_EQUAL(format("0x{0:x}", 3.14159), "0x3.14159");
  CHECK_EQUAL(format("0x{0:x}", 3.14159f), "0x3.14159");
  CHECK_EQUAL(format("0x{0:x}", "hello"), "0xhello");
}

TEST(TestException)
{
  //CHECK(format("%s", 1) == "");
}

int main()
{
  UnitTest::RunAllTests();
}
