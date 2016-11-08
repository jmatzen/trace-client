package com.ayxia.trace;

import org.slf4j.Logger;
import org.slf4j.Marker;
import org.slf4j.helpers.MessageFormatter;
import sun.misc.resources.Messages_fr;

import java.util.HashMap;

/**
 * Created by john on 11/4/2016.
 */
public class LoggerImpl implements Logger {
  private final String channel;

  public LoggerImpl(String channel) {
    this.channel = channel;
  }

  public String getName() {
    return null;
  }

  public boolean isTraceEnabled() {
    return false;
  }

  public void trace(String s) {
  }

  public void trace(String s, Object o) {

  }

  public void trace(String s, Object o, Object o1) {

  }

  public void trace(String s, Object... objects) {

  }

  public void trace(String s, Throwable throwable) {

  }

  public boolean isTraceEnabled(Marker marker) {
    return false;
  }

  public void trace(Marker marker, String s) {

  }

  public void trace(Marker marker, String s, Object o) {

  }

  public void trace(Marker marker, String s, Object o, Object o1) {

  }

  public void trace(Marker marker, String s, Object... objects) {

  }

  public void trace(Marker marker, String s, Throwable throwable) {

  }

  public boolean isDebugEnabled() {
    return false;
  }

  public void debug(String s) {

  }

  public void debug(String s, Object o) {

  }

  public void debug(String s, Object o, Object o1) {

  }

  public void debug(String s, Object... objects) {

  }

  public void debug(String s, Throwable throwable) {

  }

  public boolean isDebugEnabled(Marker marker) {
    return false;
  }

  public void debug(Marker marker, String s) {

  }

  public void debug(Marker marker, String s, Object o) {

  }

  public void debug(Marker marker, String s, Object o, Object o1) {

  }

  public void debug(Marker marker, String s, Object... objects) {

  }

  public void debug(Marker marker, String s, Throwable throwable) {

  }

  public boolean isInfoEnabled() {
    return false;
  }

  public void info(String s) {
    logger(JNITrace.Level_INFO, s, null);
  }

  public void info(String s, Object o) {
    logger(JNITrace.Level_INFO, s, o);
  }

  public void info(String s, Object o, Object o1) {
    logger(JNITrace.Level_INFO, s, o, o1);
  }

  public void info(String s, Object... objects) {
    logger(JNITrace.Level_INFO, s, objects);
  }

  public void info(String s, Throwable throwable) {
    logger(JNITrace.Level_INFO, s, throwable);
  }

  public boolean isInfoEnabled(Marker marker) {
    return false;
  }

  public void info(Marker marker, String s) {
    logger(JNITrace.Level_INFO, s);
  }

  public void info(Marker marker, String s, Object o) {
    logger(JNITrace.Level_INFO, s, o);

  }

  public void info(Marker marker, String s, Object o, Object o1) {
    logger(JNITrace.Level_INFO, s, o, o1);

  }

  public void info(Marker marker, String s, Object... objects) {
    logger(JNITrace.Level_INFO, s, objects);
  }

  public void info(Marker marker, String s, Throwable throwable) {

  }

  public boolean isWarnEnabled() {
    return false;
  }

  public void warn(String s) {
    logger(JNITrace.Level_WARNING, s);

  }

  public void warn(String s, Object o) {
    logger(JNITrace.Level_WARNING, s, o);
  }

  public void warn(String s, Object... objects) {
    logger(JNITrace.Level_WARNING, s, objects);

  }

  public void warn(String s, Object o, Object o1) {
    logger(JNITrace.Level_WARNING, s, o, o1);

  }

  public void warn(String s, Throwable throwable) {

  }

  public boolean isWarnEnabled(Marker marker) {
    return false;
  }

  public void warn(Marker marker, String s) {
    logger(JNITrace.Level_WARNING, s);

  }

  public void warn(Marker marker, String s, Object o) {
    logger(JNITrace.Level_WARNING, s, o);

  }

  public void warn(Marker marker, String s, Object o, Object o1) {
    logger(JNITrace.Level_WARNING, s, o, o1);

  }

  public void warn(Marker marker, String s, Object... objects) {
    logger(JNITrace.Level_WARNING, s, objects);

  }

  public void warn(Marker marker, String s, Throwable throwable) {
    logger(JNITrace.Level_WARNING, s, throwable);

  }

  public boolean isErrorEnabled() {
    return false;
  }

  public void error(String s) {
    logger(JNITrace.Level_ERROR, s);

  }

  public void error(String s, Object o) {
    logger(JNITrace.Level_ERROR, s, o);

  }

  public void error(String s, Object o, Object o1) {
    logger(JNITrace.Level_ERROR, s, o, o1);

  }

  public void error(String s, Object... objects) {
    logger(JNITrace.Level_ERROR, s, objects);

  }

  public void error(String s, Throwable throwable) {
    logger(JNITrace.Level_ERROR, s, throwable);

  }

  public boolean isErrorEnabled(Marker marker) {
    return false;
  }

  public void error(Marker marker, String s) {
    logger(JNITrace.Level_ERROR, s);

  }

  public void error(Marker marker, String s, Object o) {
    logger(JNITrace.Level_ERROR, s, o);

  }

  public void error(Marker marker, String s, Object o, Object o1) {
    logger(JNITrace.Level_ERROR, s, o, o1);

  }

  public void error(Marker marker, String s, Object... objects) {
    logger(JNITrace.Level_ERROR, s, objects);

  }

  public void error(Marker marker, String s, Throwable throwable) {
    logger(JNITrace.Level_ERROR, s, throwable);

  }

  private void logger(int level, String s, Object... objects) {
    StackTraceElement e = Thread.currentThread().getStackTrace()[3];
    JNITrace.simpleTrace(
        level,
        channel+"."+e.getMethodName(),
        MessageFormatter.format(s, objects).getMessage(),
        e.getFileName(),
        e.getLineNumber()
    );

  }
}
