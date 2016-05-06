﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Ayxia.Trace
{

  public enum Level
  {
    Info,
    Warning,
    Error,
  }

  //public enum ArgType
  //{
  //  Int8,
  //  UInt8,
  //  Int16,
  //  UInt16,
  //  Int32,
  //  UInt32,
  //  Int64,
  //  UInt64,
  //  Float32,
  //  Float64,
  //  String,
  //  WString
  //}

  //public enum Command
  //{
  //  Initialize,
  //  InitChannel,
  //  Trace,
  //  EndFrame,
  //  ThreadName
  //}

  //[StructLayout(LayoutKind.Sequential)]
  //public struct ayxia_trace_arg
  //{
  //  [MarshalAs(UnmanagedType.LPStr)]
  //  public string value;

  //  [MarshalAs(UnmanagedType.I4)]
  //  public ArgType type;
  //}

	[Flags]
	public enum InitializationFlags
	{
		None = 0,
	}

  [StructLayout(LayoutKind.Sequential)]
  public struct ayxia_trace_initialize
  {
    [MarshalAs(UnmanagedType.LPStr)]
    public string RemoteHost;
    [MarshalAs(UnmanagedType.LPStr)]
    public string ProcessName;
    public Int32 MaxNetworkMemoryKB;
		[MarshalAs(UnmanagedType.I4)]
		public InitializationFlags Flags;
  }

  //[StructLayout(LayoutKind.Sequential)]
  //public struct ayxia_trace_channel
  //{
  //  private UInt32 Bits;
  //  public UInt32 Cookie;
  //  [MarshalAs(UnmanagedType.LPStr)]
  //  public string Channel;
  //  [MarshalAs(UnmanagedType.LPStr)]
  //  public string File;
  //  [MarshalAs(UnmanagedType.LPStr)]
  //  public string Func;
  //  [MarshalAs(UnmanagedType.LPStr)]
  //  public string Format;

  //  public int Level
  //  {
  //    get { return (int)(Bits & 0xff); }
  //    set { Bits = (uint)(Bits & ~0xff) | ((uint)value & 0xff); }
  //  }

  //  public uint LineNo
  //  {
  //    get { return (Bits & 0xefffff00) >> 8; }
  //    set { Bits = (Bits & 0x800000ff) | (value << 8); }
  //  }

  //  public bool Disabled
  //  {
  //    get { return (Bits & (1 << 31)) != 0; }
  //  }
  //}

  public class Trace
  {
    [DllImport("ayxiatrace.dll")]
    public static extern void ayxia_tc_initialize(
      [In] ref ayxia_trace_initialize init);

    [DllImport("ayxiatrace.dll")]
    public static extern void ayxia_tc_shutdown();

    //[DllImport("ayxiatrace.dll")]
    //public static extern void ayxia_tc_init_channel(
    //  [In,Out] ref ayxia_trace_channel channel);

    [DllImport("ayxiatrace.dll")]
    public static extern void ayxia_tc_simple_trace(
      [MarshalAs(UnmanagedType.I4)]
      [In] Level level,
      [MarshalAs(UnmanagedType.LPStr)]
      [In] string channel, 
      [MarshalAs(UnmanagedType.LPStr)]
			[In] string message);

    //[DllImport("ayxiatrace.dll")]
    //public static extern void ayxia_tc_trace(
    //  [In] ref ayxia_trace_channel channel,
    //  [MarshalAs(UnmanagedType.LPArray)]
    //  [In] ayxia_trace_arg[] args,
    //  [MarshalAs(UnmanagedType.SysUInt)]
    //  [In] UIntPtr nargs);

    public static void Message(Type obj, Level level, String format, params Object[] args) 
    {
      ayxia_tc_simple_trace(level, obj.FullName, String.Format(format, args));
    }

    public static void Message(object obj, Level level, String format, params Object[] args)
    {
      Message(obj.GetType(), level, format, args);
    }
  }
}

