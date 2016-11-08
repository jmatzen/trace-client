package com.ayxia.trace;

import ch.qos.logback.classic.Level;
import ch.qos.logback.classic.spi.LoggingEvent;
import ch.qos.logback.core.UnsynchronizedAppenderBase;

/**
 * Created by jmatzen on 11/8/2016.
 */
public class AyxiaTraceAppender<E extends LoggingEvent>  extends UnsynchronizedAppenderBase<E> {
	protected void append(E e) {
		StackTraceElement caller = e.getCallerData()[0];
		int level = JNITrace.Level_INFO;
		switch (e.getLevel().toInt()) {
			case Level.ERROR_INT: level = JNITrace.Level_ERROR; break;
			case Level.WARN_INT: level = JNITrace.Level_WARNING; break;
		}
		JNITrace.simpleTrace(
			level,
			caller.getClassName() + "." + caller.getMethodName(),
			e.getFormattedMessage(),
			caller.getFileName(),
			caller.getLineNumber()
		);
	}

	public synchronized void start() {
		JNITrace.load();
		JNITrace.initialize("localhost", "JNITrace", 0);
		super.start();
	}

	public void stop() {
		JNITrace.shutdown();
	}
}
