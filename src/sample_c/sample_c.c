#include <trace/trace.h>

int main()
{
  ayxia_tc_initialize();

  ayxia_trace_block block =
  {
    .level = 0,
    .channel = "ayxia.tc.sample",
    .file = __FILE__,
    .func = __FUNCTION__,
    .lineno = __LINE__,
    .format = "some data \0 wtf \1"
  };

  int myval = 3;
  const char* mystr = "asdfasdf";

  ayxia_trace_arg args[] = 
  {
    {.parg = &myval,.type = att_int32 },
    {.parg = mystr,.type = att_string }
  };

  ayxia_tc_trace(&block, args, 2);

  ayxia_tc_shutdown();
}
