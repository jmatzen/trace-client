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
  auto init = TraceInitialize("localhost", "", 1024 * 1024);
  ayxia_tc_initialize(&init);
  
  for (int i = 0; i < 100; ++i)
  {
    TRACE_INFO("this.is.a.test", "this is a test {0} {1} {2,16:4} {3:4} {4} 0x{0:x}", i, "test", float(i), double(i));
  }
  ayxia_tc_shutdown();
}


