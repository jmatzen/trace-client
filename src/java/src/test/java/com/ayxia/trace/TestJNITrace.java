package com.ayxia.trace;

import org.junit.AfterClass;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Created by jmatzen on 11/4/2016.
 */
public class TestJNITrace {

	Logger logger = LoggerFactory.getLogger(TestJNITrace.class);


	@Test
	public void simpleTrace() {
		logger.info("this is a test");
	}


	@Test
	public void startFrame() throws Exception{
		for (int i = 0; i < 100; ++i ) {
			JNITrace.startFrame();
			logger.info("line {}", i);
			x();
			Thread.sleep(10);
		}
	}

	@AfterClass
	public static void teardown() {
		JNITrace.shutdown();
	}

	private void x() {
		logger.error("some error");
	}
}
