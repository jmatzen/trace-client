using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Ayxia.Trace;

namespace TestAyxiaDotNet
{
  public class Class1
  {
    public static void Main(string[] args)
    {
      var init = new ayxia_trace_initialize();
      init.ProcessName = "TestAyxiaDotNet";
      Trace.ayxia_tc_initialize(ref init);

      var channel = new ayxia_trace_channel();
      channel.Level = 0;
      channel.LineNo = 1;
      channel.File = "file";
      channel.Func = "Main";
      channel.Format = "this is a test";
      channel.Channel = "channel";
      Trace.ayxia_tc_init_channel(ref channel);

      Trace.ayxia_tc_trace(ref channel, null, new UIntPtr(0));

      Trace.Debug(ref channel, (short)3);

      Trace.ayxia_tc_shutdown();
    }
  }
}
