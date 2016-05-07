using System;
using System.Runtime.InteropServices;

namespace Ayxia.Trace
{

	public enum Level
	{
		Info,
		Warning,
		Error,
	}


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


	public class Trace
	{
		[DllImport("ayxiatrace.dll")]
		public static extern void ayxia_tc_initialize(
		[In] ref ayxia_trace_initialize init);

		[DllImport("ayxiatrace.dll")]
		public static extern void ayxia_tc_shutdown();

		[DllImport("ayxiatrace.dll")]
		public static extern void ayxia_tc_simple_trace(
		[MarshalAs(UnmanagedType.I4)]
[In] Level level,
		[MarshalAs(UnmanagedType.LPStr)]
[In] string channel,
		[MarshalAs(UnmanagedType.LPStr)]
[In] string message);


		[DllImport("ayxiatrace.dll")]
		public static extern void ayxia_tc_start_frame();

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

