#include <trace/trace.h>

int main()
{
  ayxia_tc_initialize();
  TRACE_INFO("channel", "this is a test %d", 3, "test", L"foo");
  ayxia_tc_shutdown();
}


