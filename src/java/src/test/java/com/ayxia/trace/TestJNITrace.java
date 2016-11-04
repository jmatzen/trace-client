package com.ayxia.trace;

import org.junit.Test;

/**
 * Created by jmatzen on 11/4/2016.
 */
public class TestJNITrace {
	{
		new JNITrace();
		JNITrace.initialize("localhost", "JNITrace", 0);
	}

	@Test
	public void simpleTrace() {
		JNITrace.simpleTrace(0, getClass().getName(), "this is a test");
	}


	@Test
	public void startFrame() throws Exception{
		for (int i = 0; i < 100; ++i ) {
			JNITrace.startFrame();
			JNITrace.simpleTrace(0, getClass().getName(), "this is a test");
			Thread.sleep(10);
		}
	}
}
