#include <UnitTest++/UnitTest++.h>
#include <trace/format.h>
#include <array>
#include <memory>
#include <string>
#include <algorithm>
#include <iostream>


template<typename T> void make_format_args(T) {}

// specialization for string types
template<typename T, typename Arg, typename... Args>
void make_format_args(T it, const Arg* arg, const Args& ...args)
{
  std::basic_string<Arg> s(arg);
  auto p = (char*)malloc(s.length()*sizeof(Arg)+2);
  uint16_t len = static_cast<uint16_t>(s.length());
  memcpy(p, &len, sizeof(uint16_t));
  memcpy(p + 2, s.data(), len * sizeof(Arg));
  *it++ = ayxia_trace_arg{ p, FormatArgType<const Arg*>::value };
  make_format_args(it, args...);
}


template<typename T, typename Arg, typename... Args>
void make_format_args(T it, const Arg& arg, const Args& ...args)
{
  *it++ = ayxia_trace_arg{ &arg, FormatArgType<Arg>::value };
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
  CHECK_EQUAL("-1", format("{0}", int16_t(0xffff)));
  CHECK_EQUAL("-1", format("{0}", int32_t(0xffffffff)));
  CHECK_EQUAL("-95210079662522641", format("{0}", int64_t(0xfeadbeefdeadbeefLL)));
}

TEST(TestUInt)
{
  CHECK_EQUAL("65535", format("{0}", uint16_t(~0U)));
  CHECK_EQUAL(std::to_string(~0U), format("{0}", uint32_t(~0U)));
  CHECK_EQUAL(std::to_string(~0ULL), format("{0}", uint64_t(~0ULL)));
}

TEST(TestString)
{
  CHECK_EQUAL("this is not a test", format("this is {0} a test", "not"));
}

TEST(TestDouble)
{
  CHECK_EQUAL("3.14", format("{0}", 3.14));
}

TEST(TestFloat)
{
  CHECK_EQUAL("3.14159", format("{0}", 3.14159f));
}

TEST(TestHex)
{
  CHECK_EQUAL("0xffff", format("0x{0:x}", int16_t(0xffff)));
  CHECK_EQUAL("0xffffffff", format("0x{0:x}", int32_t(0xffffffff)));
  CHECK_EQUAL("0xfeadbeefdeadbeef", format("0x{0:x}", int64_t(0xfeadbeefdeadbeefLL)));
  CHECK_EQUAL("0x3.14159", format("0x{0:x}", 3.14159));
  CHECK_EQUAL("0x3.14159", format("0x{0:x}", 3.14159f));
  CHECK_EQUAL("0xhello", format("0x{0:x}", "hello"));
  CHECK_EQUAL("0xhello", format("0x{0:}", "hello"));
}

TEST(TestGarbage1)
{
  CHECK_EQUAL("hello", format("{0:}", "hello"));
  CHECK_EQUAL("hello", format("{0,}", "hello"));
  CHECK_EQUAL("hello", format("{0,:}", "hello"));
  CHECK_EQUAL("hello", format("{0:,}", "hello"));
  CHECK_EQUAL("test", format("test{0,", "hello"));
  CHECK_EQUAL("test",format("test{0,:asdf", "hello"));
}

TEST(TestAlignLeft)
{
  CHECK_EQUAL("1234    1234", format("{0,-8}{1}", 1234, 1234));
  CHECK_EQUAL("-3.14     ", format("{0,-10:3}", -3.14159));
}

TEST(TestAlignRight)
{
  CHECK_EQUAL("    1234    1234", format("{0,8}{1,8}", 1234, 1234));
  CHECK_EQUAL("    12341234", format("{0,8}{1}", 1234, 1234));
  CHECK_EQUAL("    12341234    ", format("{0,8}{1,-8}", 1234, 1234));
  CHECK_EQUAL("   3.14159", format("{0,10}", 3.14159));
  CHECK_EQUAL("     -3.14", format("{0,10:3}", -3.14159));
}

TEST(TestPrecision)
{
  CHECK_EQUAL("-3.14", format("{0:3}", -3.14159));
  CHECK_EQUAL("3.14", format("{0:3}", 3.14159));
}

TEST(TestAlignStrings)
{
  CHECK_EQUAL("    test", format("{0,8}", "test"));
  CHECK_EQUAL("test    ", format("{0,-8}", "test"));
}

TEST(TestWideStrings)
{
  CHECK_EQUAL("test", format("{0}", L"test"));
}

int main()
{
  UnitTest::RunAllTests();
}
