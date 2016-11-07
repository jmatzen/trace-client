#include <trace/trace.h>
#include <stdlib.h>

int main()
{
  ayxia_trace_initialize init =
  {
    .remote_host = "localhost",
    .process_name = "sample_c",
    .max_network_memory_kb = 1024 * 1024,
  };

  ayxia_tc_initialize(&init);
  for (int i = 0; i != 100; ++i)
  { 
    if (!(i%10))
      ayxia_tc_start_frame();
    if (rand() & 1)
    {
      TRACE_INFO(__FUNCTION__, "this is a test %d", i);
    }
    else
    {
      ayxia_tc_simple_trace(rand() % 3, "x.y.z", "this is a message", "", 0);
    }
  }
  ayxia_tc_shutdown();
}
