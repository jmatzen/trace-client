#include <trace/trace.h>
#include <Windows.h>
int main()
{
  ayxia_trace_initialize init =
  {
    .process_name = "sample_c",
    .max_network_memory_kb = 1024 * 1024,
    .flags = TRACE_INIT_FLAGS_ALLOW_DROPPED_FRAMES,
  };

  ayxia_tc_initialize(&init);
  TRACE_INFO("channel", "this is a test %d", 3);
  Sleep(1000);
  ayxia_tc_shutdown();
}
