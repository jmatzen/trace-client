package com.ayxia.trace;

/**
 * Created by jmatzen on 11/4/2016.
 */
public class JNITrace {
	{
		System.loadLibrary("../../build/bin/debug/trace-java");
	}

	public static native void initialize(String remoteHost, String processName, int flags);
	public static native void simpleTrace(int level, String channel, String text);
	public static native void startFrame();
	public static native void shutdown();
}
