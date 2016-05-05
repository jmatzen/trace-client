using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Ayxia.Trace
{

  public enum ArgType
  {
    Int8,
    UInt8,
    Int16,
    UInt16,
    Int32,
    UInt32,
    Int64,
    UInt64,
    Float32,
    Float64,
    String,
    WString
  }

  public enum Command
  {
    Initialize,
    InitChannel,
    Trace,
    EndFrame,
    ThreadName
  }

  [StructLayout(LayoutKind.Sequential)]
  public struct ayxia_trace_arg
  {
    [MarshalAs(UnmanagedType.LPStr)]
    public string value;

    [MarshalAs(UnmanagedType.I4)]
    public ArgType type;
  }

  [StructLayout(LayoutKind.Sequential)]
  public struct ayxia_trace_initialize
  {
    [MarshalAs(UnmanagedType.LPStr)]
    public string RemoteHost;
    [MarshalAs(UnmanagedType.LPStr)]
    public string ProcessName;
    public IntPtr MaxNetworkMemoryKB;
    public Int32 AllowDroppedFrames;
  }

  [StructLayout(LayoutKind.Sequential)]
  public struct ayxia_trace_channel
  {
    private UInt32 Bits;
    public UInt32 Cookie;
    [MarshalAs(UnmanagedType.LPStr)]
    public string Channel;
    [MarshalAs(UnmanagedType.LPStr)]
    public string File;
    [MarshalAs(UnmanagedType.LPStr)]
    public string Func;
    [MarshalAs(UnmanagedType.LPStr)]
    public string Format;

    public int Level
    {
      get { return (int)(Bits & 0xff); }
      set { Bits = (uint)(Bits & ~0xff) | ((uint)value & 0xff); }
    }

    public uint LineNo
    {
      get { return (Bits & 0xefffff00) >> 8; }
      set { Bits = (Bits & 0x800000ff) | (value << 8); }
    }

    public bool Disabled
    {
      get { return (Bits & (1 << 31)) != 0; }
    }
  }

  public class Trace
  {
    [DllImport("ayxiatrace.dll")]
    public static extern void ayxia_tc_initialize(
      [In] ref ayxia_trace_initialize init);

    [DllImport("ayxiatrace.dll")]
    public static extern void ayxia_tc_shutdown();

    [DllImport("ayxiatrace.dll")]
    public static extern void ayxia_tc_init_channel(
      [In,Out] ref ayxia_trace_channel channel);

    [DllImport("ayxiatrace.dll")]
    public static extern void ayxia_tc_type_trace(
      [MarshalAs(UnmanagedType.LPStr)]
      string type, 
      [MarshalAs(UnmanagedType.LPStr)]
      string message);

    [DllImport("ayxiatrace.dll")]
    public static extern void ayxia_tc_trace(
      [In] ref ayxia_trace_channel channel,
      [MarshalAs(UnmanagedType.LPArray)]
      [In] ayxia_trace_arg[] args,
      [MarshalAs(UnmanagedType.SysUInt)]
      [In] UIntPtr nargs);

    public static void Debug(Type obj, String format, params Object[] args) 
    {
      ayxia_tc_type_trace(obj.GetType().FullName, String.Format(format, args));
    }

    public static void Debug(object obj, String format, params Object[] args)
    {
      var type = obj.GetType();
      String.Format(format, args);

    }


    public static void Debug(ref ayxia_trace_channel channel, params Object[] args)
    {
      var traceargs = new ayxia_trace_arg[args.Length];
      int i = 0;
      foreach (var arg in args)
      {
        var s = arg.ToString();
        if (arg.GetType() == typeof(short))
        {
          short val = (short)arg;
        }
      }
    }
  }
}

