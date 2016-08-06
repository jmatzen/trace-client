#include <trace/trace.h>
#include <thread>
#include <chrono>

#if defined _WIN32

#include <crtdbg.h>

struct InitAllocHook
{
  InitAllocHook()
  {
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG| _CRTDBG_MODE_WNDW);
    //_CrtSetBreakAlloc(152);
  }
} static const $allocInit;

#endif

class TraceTest
{
public:
  static void info()
  {
    TRACE_INFO("StreamReader::Read", "Reading {0} bytes.", rand());
    if (rand() % 2)
      return;
    TRACE_INFO("StreamReader::Read::Process", "Processed {0} bytes.", rand());
  }

  static void warning()
  {
    if (rand() % 10)
      return;
    TRACE_WARNING("StreamReader::Read::failed", "Checksum failed address={0}", (void*)intptr_t(0xdeadbeef));
  }

  static void error()
  {
    if (rand() % 15)
      return;
    TRACE_ERROR("StreamReader::Write", "failed to write {0} bytes. {1}", rand(), rand()?true:false);

  }
};

int main()
{
  auto init = TraceInitialize("localhost", "sample_cpp", 1024 * 1024);
  ayxia_tc_initialize(&init);
  
  TRACE_INFO("boolcheck", "{0} {1}", true, false);
  for (int i = 0; i < 100; ++i)
  {
    ayxia_tc_start_frame();
    const char* str = "teest";
    const wchar_t* str2 = L"wteest";
    TRACE_INFO("a", "this is a test {0} {1}", true, false);
    TRACE_INFO("xx", "this is a test {0} {1}", str,str2);
    TRACE_WARNING("b", "this is a test {0} {1} {2,16:4} {3:4} {4} 0x{0:x}", i, "test", float(i), double(i));
    TRACE_ERROR("c", "this is a test {0} {1} {2,16:4} {3:4} {4} 0x{0:x}", i, "test", float(i), double(i));
  }

  for (;;)
  {
    ayxia_tc_start_frame();
    switch (rand() % 3) {
    case 0: TraceTest::info(); break;
    case 1: TraceTest::warning(); break;
    case 2: TraceTest::error(); break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(rand()%100?10:100));
  }
  ayxia_tc_shutdown();
}


