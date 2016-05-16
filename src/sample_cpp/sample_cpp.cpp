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


int main()
{
  auto init = TraceInitialize("localhost", "sample_cpp", 1024 * 1024);
  ayxia_tc_initialize(&init);
  
  for (int i = 0; i < 100; ++i)
  {
    ayxia_tc_start_frame();
    TRACE_INFO("info", "this is a test {0} {1} {2,16:4} {3:4} {4} 0x{0:x}", i, "test", float(i), double(i));
    TRACE_WARNING("warning", "this is a test {0} {1} {2,16:4} {3:4} {4} 0x{0:x}", i, "test", float(i), double(i));
    TRACE_ERROR("error", "this is a test {0} {1} {2,16:4} {3:4} {4} 0x{0:x}", i, "test", float(i), double(i));
  }

  for (;;)
  {
    ayxia_tc_start_frame();
    TRACE_INFO("info", "this is a test");
    TRACE_WARNING("warning", "this is a test");
    TRACE_ERROR("error", "this is a test {0}");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  ayxia_tc_shutdown();
}


