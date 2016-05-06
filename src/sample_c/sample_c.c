#include <trace/trace.h>
#include <Windows.h>
int main()
{
  ayxia_trace_initialize init =
  {
    .remote_host = "localhost",
    .process_name = "sample_c",
    .max_network_memory_kb = 1024 * 1024,
    .flags = aif_none,
  };

  ayxia_tc_initialize(&init);
  TRACE_INFO("channel", "this is a test %d", 3);
  Sleep(1000);
  ayxia_tc_shutdown();
}
