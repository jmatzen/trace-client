package org.slf4j.impl;

import com.ayxia.trace.JNITrace;
import com.ayxia.trace.LoggerImpl;
import org.slf4j.ILoggerFactory;
import org.slf4j.Logger;

/**
 * Created by john on 11/4/2016.
 */
public class StaticLoggerBinder {
  public static String REQUESTED_API_VERSION = "1.7.21";

  public ILoggerFactory getLoggerFactory() {
    return new ILoggerFactory() {
      public Logger getLogger(String s) {
        return new LoggerImpl(s);
      }
    };
  }

  public String getLoggerFactoryClassString() {
    return null;
  }

  public static StaticLoggerBinder getSingleton() {
    return loggerBinder;
  }


  static StaticLoggerBinder loggerBinder = new StaticLoggerBinder();
  static JNITrace jniTrace = new JNITrace();
}
