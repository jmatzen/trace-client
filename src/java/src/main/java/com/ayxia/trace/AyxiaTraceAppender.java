package com.ayxia.trace;

import ch.qos.logback.classic.Level;
import ch.qos.logback.classic.spi.LoggingEvent;
import ch.qos.logback.core.UnsynchronizedAppenderBase;

/**
 * Created by jmatzen on 11/8/2016.
 */
public class AyxiaTraceAppender<E extends LoggingEvent>  extends UnsynchronizedAppenderBase<E> {
	static final int MAX_MESSAGE_LENGTH = 4096;

	protected void append(E e) {
		StackTraceElement caller = e.getCallerData()[0];
		int level = JNITrace.Level_INFO;
		switch (e.getLevel().toInt()) {
			case Level.ERROR_INT: level = JNITrace.Level_ERROR; break;
			case Level.WARN_INT: level = JNITrace.Level_WARNING; break;
		}
		final String message = e.getFormattedMessage();
		JNITrace.simpleTrace(
			level,
			caller.getClassName() + "." + caller.getMethodName(),
			message.length() > MAX_MESSAGE_LENGTH
				? message.substring(0, Math.min(MAX_MESSAGE_LENGTH, message.length()))
				: message,
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
