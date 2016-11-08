package com.ayxia.trace;

/**
 * Created by jmatzen on 11/4/2016.
 */
public class JNITrace {
	public static void load() {
		System.loadLibrary("trace-java");
	}

	public static native void initialize(String remoteHost, String processName, int flags);
	public static native void simpleTrace(int level, String channel, String text, String filename, int lineno);
	public static native void startFrame();
	public static native void shutdown();

	public static final int Level_INFO = 0;
	public static final int Level_WARNING = 1;
	public static final int Level_ERROR = 2;
}
