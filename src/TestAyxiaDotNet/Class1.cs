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
      init.RemoteHost = "localhost";
      init.ProcessName = "TestAyxiaDotNet";
      Trace.ayxia_tc_initialize(ref init);

      Trace.Debug(typeof(Class1), "format {0}", 3);

      Trace.ayxia_tc_shutdown();
    }
  }
}
