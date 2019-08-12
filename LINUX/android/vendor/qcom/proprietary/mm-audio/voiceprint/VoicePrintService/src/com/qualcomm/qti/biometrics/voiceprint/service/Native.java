/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.biometrics.voiceprint.service;

/*
 * The Class Native.
 */
/**
 * The Class Native.
 */
public class Native {

  /**
   * The Constant JNI_LIB.
   */
  private static final String JNI_LIB = "qvop-service";

  private static boolean mLibraryLoaded = true;

  static {
    try {
      System.loadLibrary(JNI_LIB);
    } catch (Exception e) {
      mLibraryLoaded = false;
    } catch ( Error e) {
      mLibraryLoaded = false;
    }
  }

  public static boolean isLibraryAvailable() {
    return mLibraryLoaded;
  }

  /**
   * Native open.
   *
   * @return the long
   */
  public static native long open();

  /**
   * Native close.
   *
   * @return the long
   */
  public static native long close();

  /**
   * Shutdown QVOP, called on powerdown.
   *
   * @param jniContext
   *            Context returned by open()
   * @return 0 if success
   */
  public native int shutdown(long jniContext);

  /**
   * Native enroll.
   *
   * @param jniContext
   *            the jni context
   * @param cb
   *            the cb
   * @param user
   *            the user
   * @param identifier
   *            the identifier
   * @param phrase
   *            the phrase
   * @return the int
   */
  public static native int enroll(long jniContext, IJniCallback cb,
                                  String user, String identifier, String phrase);

  public static native int setThreshold(long jniContext, IJniCallback cb,
                                        String user, String identifier, int threshold);

  /**
   * Enroll capture start.
   *
   * @param jniContext
   *            the jni context
   * @param cb
   *            the cb
   * @return the int
   */
  public static native int enrollCaptureStart(long jniContext, IJniCallback cb);

  /**
   * Enroll capture complete.
   *
   * @param jniContext
   *            the jni context
   * @param cb
   *            the cb
   * @return the int
   */
  public static native int enrollCaptureComplete(long jniContext,
      IJniCallback cb);

  /**
   * Enroll commit.
   *
   * @param jniContext
   *            the jni context
   * @param cb
   *            the cb
   * @return the int
   */
  public static native int enrollCommit(long jniContext, IJniCallback cb);

  /**
  * Get Match Result
  *
  * @param jniContext
  *            the jni context
  * @param cb
  *            the cb
  * @return the int
  */
  public static native int matchCaptureComplete(long jniContext, IJniCallback cb);

  /**
   * Native match.
   *
   * @param jniContext
   *            the jni context
   * @param cb
   *            the cb
   * @param user
   *            the user
   * @return the int
   */
  public static native int match(long jniContext, IJniCallback cb, String user);

  /**
   * Match with id.
   *
   * @param jniContext
   *            the jni context
   * @param cb
   *            the cb
   * @param user
   *            the user
   * @param identifier
   *            the identifier
   * @return the int
   */
  public static native int matchWithId(long jniContext, IJniCallback cb,
                                       String user, String identifier);

  /**
   * Native match secure.
   *
   * @param jniContext
   *            the jni context
   * @param cb
   *            the cb
   * @param user
   *            the user
   * @return the int
   */
  public static native int matchSecure(long jniContext, IJniCallback cb,
                                       String user);

  /**
   * Native match any.
   *
   * @param jniContext
   *            the jni context
   * @param cb
   *            the cb
   * @param user
   *            the user
   * @return the int
   */
  public static native int matchAny(long jniContext, IJniCallback cb,
                                    String user);

  /**
   * Match any key phrase.
   *
   * @param jniContext
   *            the jni context
   * @param cb
   *            the cb
   * @param user
   *            the user
   * @param phrase
   *            the phrase
   * @return the int
   */
  public static native int matchAnyKeyPhrase(long jniContext,
      IJniCallback cb, String user, String phrase);

  /**
   * Native match any secure.
   *
   * @param jniContext
   *            the jni context
   * @param cb
   *            the cb
   * @param user
   *            the user
   * @return the int
   */
  public static native int matchAnySecure(long jniContext, IJniCallback cb,
                                          String user);

  /**
   * Native cancel.
   *
   * @param jniContext
   *            the jni context
   * @return the int
   */
  public static native int cancel(long jniContext);

  /**
   * Native delete.
   *
   * @param jniContext
   *            the jni context
   * @param cb
   *            the cb
   * @param user
   *            the user
   * @return the int
   */
  public static native int delete(long jniContext, IJniCallback cb,
                                  String user);

  /**
   * Native delete.
   *
   * @param jniContext
   *            the jni context
   * @param cb
   *            the cb
   * @param user
   *            the user
   * @param identifier
   *            the identifier
   * @return the int
   */
  public static native int deleteWithId(long jniContext, IJniCallback cb,
                                        String user, String identifier);

  /**
   * Native delete all.
   *
   * @param jniContext
   *            the jni context
   * @param cb
   *            the cb
   * @param user
   *            the user
   * @return the int
   */
  public static native int deleteAll(long jniContext, IJniCallback cb,
                                     String user);

  /**
   * Process frame.
   *
   * @param jniContext
   *            the jni context
   * @param cb
   *            the cb
   * @param timeStamp
   *            the time stamp
   * @param len
   *            the len
   * @param frame
   *            the frame
   * @return the int
   */
  public static native final int processFrame(long jniContext,
      IJniCallback cb, long timeStamp, int len, short[] frame);

  /**
   * Process frame with id.
   *
   * @param jniContext
   *            the jni context
   * @param cb
   *            the cb
   * @param timeStamp
   *            the time stamp
   * @param id
   *            the id
   * @param len
   *            the len
   * @param frame
   *            the frame
   * @return the int
   */
  public static native final int processFrameWithId(long jniContext,
      IJniCallback cb, long timeStamp, long id, int len, short[] frame);

  /**
   * On start audio capture.
   *
   * @param jniContext
   *            the jni context
   * @param cb
   *            the cb
   * @param sampleRate
   *            the sample rate
   * @param numberOfChannels
   *            the number of channels
   * @param audioFormat
   *            the audio format
   * @return the int
   */
  public static native final int onStartAudioCapture(long jniContext,
      IJniCallback cb, int sampleRate, int numberOfChannels,
      int audioFormat);

  /**
   * On stop audio capture.
   *
   * @param jniContext
   *            the jni context
   * @param cb
   *            the cb
   * @return the int
   */
  public static native final int onStopAudioCapture(long jniContext,
      IJniCallback cb);

  /**
   * Rename.
   *
   * @param jniContext
   *            the jni context
   * @param cb
   *            the cb
   * @param oldId
   *            the old id
   * @param newId
   *            the new id
   * @return the int
   */
  public static native final int rename(long jniContext, IJniCallback cb,
                                        String user, String oldId, String newId);

// Must match the interface header file.

  public final static int QVOP_SUCCESS = 0;  // The call executed successfully
  public final static int QVOP_FAILURE = -1;  // There was an error executing the call
  public final static int QVOP_TIMEOUT = 1;  // A timeout was triggered during the call
  public final static int QVOP_CANCEL = 2;  // The call was canceled by a user application
  public final static int QVOP_STOPPED = 3;  // The call was forced to stop by the OS or framework
  public final static int QVOP_PCM_DATA_REQUEST = 4;  //requesting PCM
  public final static int QVOP_PCM_REWIND = 5;
  public final static int QVOP_ENROLLMENT_COMPLETE = 10;  // All individual enrollment scans have been successfully collected for the current finger
  public final static int QVOP_TZ_CANCEL_SUCCESS = 13;
  public final static int QVOP_ENROLLMENT_FAILED = -10;  // The system was unable to collect an adequate number of enrollment scans from the user
  public final static int QVOP_ENROLLMENT_NO_SPACE = -11;  // There is no more space to enroll
  public final static int QVOP_ENROLLEE_NOT_FOUND = -12; // Enrollee not found (for delete enrollee case)
  public final static int QVOP_MATCH_SUCCESS = 20;  // Inquiry was successfully matched to a fingerprint in the database
  public final static int QVOP_MATCH_COMPLETE = 21;
  public final static int QVOP_MATCH_FAILURE = -21;  // Inquiry was not matched to a voiceprint in the database

  public final static int QVOP_ERR_UNKNOWN_COMMAND = -110;
  public final static int QVOP_ERR_NOT_IMPLEMENTED = -120;
  public final static int QVOP_ERR_NOT_INITIALIZED = -130;
  public final static int QVOP_ERR_BAD_PARAM = -140;
  public final static int QVOP_ERR_MALLOC = -210;
  public final static int QVOP_ERR_FREE = -211;
  public final static int QVOP_ENROLLMENT_SECURE_UI_ERR = -312;

}
