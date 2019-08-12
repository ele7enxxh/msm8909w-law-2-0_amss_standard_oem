/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.service;

import android.content.Context;
import android.os.RemoteException;
import android.util.Log;

import java.io.Closeable;
import java.io.IOException;
import java.lang.reflect.Method;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import android.app.ActivityManagerNative;
import android.os.SystemProperties;
import android.os.Bundle;
import android.os.Handler;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.AudioFormat;
import android.media.AudioManager;
import java.util.Arrays;
/**
 * The Class QVoicePrintServiceStub.
 */
public class VoicePrintServiceStub extends IVoicePrintService.Stub implements
    Closeable {

    /**
     * The Constant Logging TAG.
     */
    private final static String TAG = "VoicePrintService";
    private final static String TAG_AUDIO = "VoicePrintAudio";

    /**
     * The version major.
     */
    private final int VERSION_MAJOR = 1;

    /**
     * The version minor.
     */
    private final int VERSION_MINOR = 0;

    /**
     * The jni context.
     */
    private long mJniContext = 0l;

    /**
     * The jni callback.
     */
    private IJniCallback mJniCallback = null;

    /**
     * The context.
     */
    private Context mContext = null;

    /** The enrolling flag. */
    private static boolean mEnrolling = false;

    private static boolean mMatching = false;

    private final static int MAX_RECORDING = 10000; // 10 seconds

    /**
     * The _mutex.
     */
    private final Lock _mutex = new ReentrantLock(true);

    /**
     * The _version.
     */
    private final String _version = String.format("%d.%d", VERSION_MAJOR,
                                    VERSION_MINOR);

    /** The Constant VOP_PROPERTY. */
    private final static String VOP_PROPERTY = "persist.qvop";

    /**
     * The _callback.
     */
    private IVoicePrintServiceCallback _callback = null;

    public static final String DEBUG = "DEBUG";
    public static final String DATA = "DATA";

    private long mLastMatchCapture = 0;
    private long mLastEnrollCapture = 0;
    private long mLastEnrollCommit = 0;

    public static String THRESHOLD = "THRESHOLD";
    public static String BYTE_INDEX = "BYTE_INDEX";

    Handler timeoutHandler = new Handler();

    /**
     * Instantiates a new voice print service stub.
     *
     * @param context
     *            the context
     */
    public VoicePrintServiceStub(Context context) {
        try {
            mJniCallback = new JniCallback();
            Log.d(TAG, "VoicePrintServiceStub enter open");
            mJniContext = Native.open();

            if (mJniContext == 0) {
                Log.d(TAG, "mJniContext failed.");
                throw new RemoteException("Unable to connect to qvop service!");
            }
            Log.d(TAG, "VoicePrintServiceStub enter done");
            mContext = context;

            Log.d(TAG, "VoicePrintServiceStub constructed jniContext=" + mJniContext);
        } catch (Exception e) {

            e.printStackTrace();
        }
    }

    /**
     * The Class JniCallback.
     */
    public class JniCallback implements IJniCallback {

        /**
         * Instantiates a new jni callback.
         */
        public JniCallback() {
            Log.d(TAG, "JniCallback constructed.");
        }

        /*
         * (non-Javadoc)
         *
         * @see
         * com.qualcomm.qti.biometrics.voiceprint.service.IJniCallback#onResult
         * (int, android.os.Bundle)
         */
        @Override
        public void onResult(int code) {
            Log.d(TAG, "onResult code=" + code);
            releaseResource();
            if (_callback != null) {
                try {
                    _callback.onResult(code, null);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
            setState(VoicePrintServiceStates.READY);
        }

        /*
         * (non-Javadoc)
         *
         * @see
         * com.qualcomm.qti.biometrics.voiceprint.service.IJniCallback#onError
         * (int)
         */
        @Override
        public void onError(int error) {
            Log.d(TAG, "onError error=" + error);
            releaseResource();
            if (_callback != null) {
                try {

                    if ( mMatching ) {
                        _callback.onMatchStatus(error, null, null, null);
                    } else if ( mEnrolling ) {
                        _callback.onEnrollStatus(error);
                    } else {
                        _callback.onError(error);
                    }

                } catch (RemoteException e) {
                    e.printStackTrace();
                }

                // Invalidate the states an error has occurred
                setState(VoicePrintServiceStates.READY);
            }
            setState(VoicePrintServiceStates.READY);
        }

        /*
         * (non-Javadoc)
         *
         * @see
         * com.qualcomm.qti.biometrics.voiceprint.service.IJniCallback#onVersionInfo
         * (long, long)
         */
        @Override
        public void onVersionInfo(long major, long minor) {
            Log.d(TAG, "onVersionInfo version=" + major + "." + minor);
            releaseResource();
            if (_callback != null) {
                try {
                    _callback.onVersionInfo(major, minor);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
            setState(VoicePrintServiceStates.READY);
        }

        /*
         * (non-Javadoc)
         *
         * @see
         * com.qualcomm.qti.biometrics.voiceprint.service.IJniCallback#onMatchStatus
         * (int, java.lang.String, java.lang.String)
         */
        @Override
        public void onMatchStatus(int status, String user_id, String identifier, int index, double sentence_score, double user_score,
                                  float spoof_score) {
            Log.d(TAG, "onMatchStatus status=" + status + ", index=" + index + ", sentence_score=" + sentence_score +
                  ", user_score=" + user_score + ", spoof_score=" + spoof_score);
            releaseResource();
            if (_callback != null) {
                try {
                    Bundle data = new Bundle();
                    StringBuilder debug = new StringBuilder();
                    String newline = System.getProperty("line.separator");
                    debug.append("index=" + index );
                    debug.append(newline);
                    debug.append(String.format("sentence_score=%.3f", sentence_score));
                    debug.append(newline);
                    debug.append(String.format("user_score=%.3f", user_score));
                    debug.append(newline);
                    debug.append(String.format("spoof_score=%.3f", spoof_score));
                    data.putString(DEBUG, debug.toString());
                    data.putInt(BYTE_INDEX, index);

                    _callback.onMatchStatus(status, user_id, identifier, data);

                } catch (RemoteException e) {
                    e.printStackTrace();
                }


            }
            setState(VoicePrintServiceStates.READY);
        }

        /*
         * (non-Javadoc)
         *
         * @see com.qualcomm.qti.biometrics.voiceprint.service.IJniCallback#
         * onEnrollStatus(int)
         */
        @Override
        public void onEnrollStatus(int status) {
            Log.d(TAG, "onEnrollStatus status=" + status);
            // long latency = System.currentTimeMillis() - mLastEnrollCommit;
            //Log.d(TAG, "enroll_commit_latency=" + latency);
            releaseResource();
            if (_callback != null) {
                try {
                    _callback.onEnrollStatus(status);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }


            setState(VoicePrintServiceStates.READY);
        }

        /*
         * (non-Javadoc)
         *
         * @see com.qualcomm.qti.biometrics.voiceprint.service.IJniCallback#
         * onDeleteStatus(int, java.lang.String, java.lang.String)
         */
        @Override
        public void onDeleteStatus(int status, String user_id, String identifier) {
            Log.d(TAG, "onDeleteStatus status=" + status);
            releaseResource();
            if (_callback != null) {
                try {
                    _callback.onDeleteStatus(status, user_id, identifier);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
            setState(VoicePrintServiceStates.READY);
        }

        /*
         * (non-Javadoc)
         *
         * @see com.qualcomm.qti.biometrics.voiceprint.service.IJniCallback#
         * onConfigUpdateStatus(int)
         */
        @Override
        public void onConfigUpdateStatus(int status) {
            Log.d(TAG, "onConfigUpdateStatus status=" + status);
            releaseResource();
            if (_callback != null) {
                try {
                    _callback.onConfigUpdateStatus(status);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
            setState(VoicePrintServiceStates.READY);
        }

        /*
         * (non-Javadoc)
         *
         * @see com.qualcomm.qti.biometrics.voiceprint.service.IJniCallback#
         * onRenameStatus(int)
         */
        @Override
        public void onRenameStatus(int status) {
            Log.d(TAG, "onRenameStatus status=" + status);
            releaseResource();
            if (_callback != null) {
                try {
                    _callback.onRenameStatus(status);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
            setState(VoicePrintServiceStates.READY);
        }
    }


    @Override
    public boolean isLibraryAvailable() {
        return Native.isLibraryAvailable();
    }


    /*
     * (non-Javadoc)
     *
     * @see
     * com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService#match
     * (com
     * .qualcomm.qti.biometrics.voiceprint.service.IVoicePrintServiceCallback)
     */
    @Override
    public void match(IVoicePrintServiceCallback cb) throws RemoteException {

        Log.d(TAG, "match");
        if (lockResource(cb)) {
            _callback = cb;
            throwIfFail(Native.match(mJniContext,
                                     mJniCallback,
                                     getCurrentUser()));
        }

        setState(VoicePrintServiceStates.WAIT_FOR_AUDIO_MATCHING);
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService#matchWithId
     * (java.lang.String,
     * com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintServiceCallback
     * )
     */
    @Override
    public void matchWithId(String id, IVoicePrintServiceCallback cb)
    throws RemoteException {

        Log.d(TAG, "match id=" + id + ", jniContext=" + mJniContext);
        if (lockResource(cb)) {
            _callback = cb;
            throwIfFail(Native.matchWithId(mJniContext,
                                           mJniCallback,
                                           getCurrentUser(), id));
        }

        setState(VoicePrintServiceStates.WAIT_FOR_AUDIO_MATCHING);
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService#matchSecure
     * (
     * com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintServiceCallback
     * )
     */
    @Override
    public void matchSecure(IVoicePrintServiceCallback cb)
    throws RemoteException {
        Log.d(TAG, "matchSecure");

        if (lockResource(cb)) {
            _callback = cb;
            throwIfFail(Native.matchSecure(mJniContext,
                                           mJniCallback, getCurrentUser()));
        }

        setState(VoicePrintServiceStates.WAIT_FOR_AUDIO_MATCHING);
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService#matchAny
     * (
     * com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintServiceCallback
     * )
     */
    @Override
    public void matchAny(IVoicePrintServiceCallback cb) throws RemoteException {
        Log.d(TAG, "matchAny");
        if (isSystemUid() == false) {
            cb.onError(VoicePrintService.PERMISSION_DENIED);
            Log.w(TAG,
                  "matchAny: PERMISSION_DENIED. Insufficient app permission.");
            return;
        }

        if (lockResource(cb)) {
            _callback = cb;
            throwIfFail(Native.matchAny(mJniContext,
                                        mJniCallback, getCurrentUser()));
        }
        setState(VoicePrintServiceStates.WAIT_FOR_AUDIO_MATCHING);
    }

    /*
     * (non-Javadoc)
     *
     * @see com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService#
     * matchAnyKeyPhrase(java.lang.String,
     * com.qualcomm.qti.biometrics.voiceprint
     * .service.IVoicePrintServiceCallback)
     */
    @Override
    public void matchAnyKeyPhrase(String keyphrase,
                                  IVoicePrintServiceCallback cb) throws RemoteException {
        if (isSystemUid() == false) {
            cb.onError(VoicePrintService.PERMISSION_DENIED);
            Log.w(TAG,
                  "matchAnyKeyPhrase: PERMISSION_DENIED. Insufficient app permission.");
            return;
        }

        if (lockResource(cb)) {
            _callback = cb;
            throwIfFail(Native.matchAnyKeyPhrase(mJniContext,
                                                 mJniCallback, getCurrentUser(), keyphrase));
        }
        setState(VoicePrintServiceStates.WAIT_FOR_AUDIO_MATCHING);
    }

    /**
     * Checks if is system uid.
     *
     * @return true, if is system uid
     */
    private boolean isSystemUid() {
        return (android.os.Process.myUid() == android.os.Process.SYSTEM_UID);
    }

    /*
     * (non-Javadoc)
     *
     * @see com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService#
     * matchAnySecure
     * (com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintServiceCallback
     * )
     */
    @Override
    public void matchAnySecure(IVoicePrintServiceCallback cb)
    throws RemoteException {
        if (isSystemUid() == false) {
            cb.onError(VoicePrintService.PERMISSION_DENIED);
            Log.w(TAG,
                  "matchAnySecure: PERMISSION_DENIED. Insufficient app permission.");
            return;
        }
        //  mState.matchAnySecure(this, cb);
        if (lockResource(cb)) {
            _callback = cb;
            throwIfFail(Native.matchAnySecure(mJniContext,
                                              mJniCallback, getCurrentUser()));
        }
        setState(VoicePrintServiceStates.WAIT_FOR_AUDIO_MATCHING);
    }


    /**
     * @brief Enrollment API
     * @details First method to invoke when wanting to enroll a voice biometric template.
     *
     * @param id identifier
     * @param keyphrase text of the keyphrase
     * @param threshold threshold for match (1: HIGH, 2: MED: 3: LOW)
     * @param cb callback
     */
    @Override
    public void enroll(String id,
                       String keyphrase,
                       int threshold,
                       IVoicePrintServiceCallback cb) throws RemoteException {
        Log.d(TAG, "enroll request");

        if ( _callback == null ) {
            Log.d(TAG, "_callback is null.");
        }

        if (isSystemUid() == false) {
            cb.onError(VoicePrintService.PERMISSION_DENIED);
            Log.w(TAG,
                  "enroll: PERMISSION_DENIED. Insufficient app permission.");
            return;
        }

        if (lockResource(cb)) {
            _callback = cb;

            Log.d(TAG, "Have Callback=" + (_callback != null));
            throwIfFail(Native.enroll(mJniContext,
                                      mJniCallback, getCurrentUser(), id, keyphrase));
            setState(VoicePrintServiceStates.WAIT_FOR_AUDIO_ENROLLING);
        }
        // Stay in this state until enroll capture begins

    }

    /*
     * (non-Javadoc)
     *
     * @see com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService#
     * completeEnroll
     * (com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintServiceCallback
     * )
     */
    @Override
    public void completeEnroll(IVoicePrintServiceCallback cb)
    throws RemoteException {
        if (isSystemUid() == false) {
            cb.onError(VoicePrintService.PERMISSION_DENIED);
            Log.w(TAG,
                  "completeEnroll: PERMISSION_DENIED. Insufficient app permission.");
            return;
        }

        if ( mEnrolling ) {
            _callback = cb;
            mLastEnrollCommit = System.currentTimeMillis();
            throwIfFail(Native.enrollCommit(mJniContext,
                                            mJniCallback));
        }

        setState(VoicePrintServiceStates.WAIT_FOR_RESULT);
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService#delete
     * (com
     * .qualcomm.qti.biometrics.voiceprint.service.IVoicePrintServiceCallback)
     */
    @Override
    public void delete(IVoicePrintServiceCallback cb) throws RemoteException {
        if (isSystemUid() == false) {
            cb.onError(VoicePrintService.PERMISSION_DENIED);
            Log.w(TAG,
                  "delete: PERMISSION_DENIED. Insufficient app permission.");
            return;
        }

        if (lockResource(cb)) {
            _callback = cb;
            Log.d(TAG, "process delete request null id");
            throwIfFail(Native.delete(mJniContext,
                                      mJniCallback, getCurrentUser()));
        }
        setState(VoicePrintServiceStates.WAIT_FOR_RESULT);
    }

    /*
     * (non-Javadoc)
     *
     * @see com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService#
     * deleteWithId(java.lang.String,
     * com.qualcomm.qti.biometrics.voiceprint.service
     * .IVoicePrintServiceCallback)
     */
    @Override
    public void deleteWithId(String id, IVoicePrintServiceCallback cb)
    throws RemoteException {
        if (isSystemUid() == false) {
            cb.onError(VoicePrintService.PERMISSION_DENIED);
            Log.w(TAG,
                  "deleteWithId: PERMISSION_DENIED. Insufficient app permission.");
            return;
        }

        if (lockResource(cb)) {
            _callback = cb;
            Log.d(TAG, "deleteWithId called for " + id);
            throwIfFail(Native.deleteWithId(mJniContext,
                                            mJniCallback, getCurrentUser(), id));
        }

        setState(VoicePrintServiceStates.WAIT_FOR_RESULT);
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService#deleteAll
     * (
     * com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintServiceCallback
     * )
     */
    @Override
    public void deleteAll(IVoicePrintServiceCallback cb) throws RemoteException {
        if (isSystemUid() == false) {
            cb.onError(VoicePrintService.PERMISSION_DENIED);
            Log.w(TAG,
                  "deleteAll: PERMISSION_DENIED. Insufficient app permission.");
            return;
        }

        if (lockResource(cb)) {
            _callback = cb;
            throwIfFail(Native.deleteAll(mJniContext,
                                         mJniCallback, getCurrentUser()));
        }
        setState(VoicePrintServiceStates.WAIT_FOR_RESULT);
    }

    /*
     * (non-Javadoc)
     *
     * @see com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService#
     * onStartAudioCapture(int, int, int)
     */
    @Override
    public void onStartAudioCapture(int sampleRate, int numberOfChannels,
                                    int audioFormat, IVoicePrintServiceCallback cb) throws RemoteException {

        Log.d(TAG, "onStartAudioCapture sampleRate" + sampleRate +
              ", numberOfChannels=" + numberOfChannels +
              ", audioFormat=" + audioFormat);
        _callback = cb;
        throwIfFail(Native.onStartAudioCapture(mJniContext,
                                               mJniCallback, sampleRate, numberOfChannels,
                                               audioFormat));

        // Recording has started. Set timeout.
        //  timeoutHandler.postDelayed(timeoutRunnable, MAX_RECORDING);

        if (mEnrolling) {
            throwIfFail(Native.enrollCaptureStart(
                            mJniContext, mJniCallback));
            setState(VoicePrintServiceStates.WAIT_FOR_AUDIO_ENROLLING);
        }
    }


    final Runnable timeoutRunnable = new Runnable() {
        public void run() {
            Log.d(TAG, "Time's up! Should not be recording more than " + MAX_RECORDING + " ms.");
            mJniCallback.onError(VoicePrintService.TIMEOUT);
            try {
                throwIfFail(Native.cancel(mJniContext));
            } catch (Exception e) {
                e.printStackTrace();
            }
            setState(VoicePrintServiceStates.READY);
        }
    };

    /*
     * (non-Javadoc)
     *
     * @see com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService#
     * onStopAudioCapture()
     */
    @Override
    public void onStopAudioCapture(IVoicePrintServiceCallback cb) throws RemoteException {
        Log.d(TAG, "onStopAudioCapture");
        _callback = cb;
        //timeoutHandler.removeCallbacksAndMessages(null);
        throwIfFail(Native.onStopAudioCapture(mJniContext,
                                              mJniCallback));

        JniCallback stopAudioCaptureCallback = new JniCallback() {


            @Override
            public void onResult(int code) {
                mJniCallback.onResult(code);
            }
            @Override
            public void onError(int error) {
                mJniCallback.onError(error);
            }
            @Override
            public void onVersionInfo(long major, long minor) {
                // Ignore
            }
            @Override
            public void onMatchStatus(int status, String user_id, String identifier, int index,
                                      double sentence_score, double user_score, float spoofScore) {
                // long latency = System.currentTimeMillis() - mLastMatchCapture;
                // Log.d(TAG, "match_latency=" + latency);
                mJniCallback.onMatchStatus(status, user_id, identifier, index, sentence_score, user_score, spoofScore);
            }
            @Override
            public void onEnrollStatus(int status) {
                if ( mEnrolling ) {
                    //   long latency = System.currentTimeMillis() - mLastEnrollCapture;
                    //   Log.d(TAG, "enroll_train_latency=" + latency);

                    if (_callback != null) {
                        try {
                            // Training returns a result and not an
                            // full enrollment
                            _callback.onResult(status, null);

                        } catch (RemoteException e) {
                            e.printStackTrace();
                        }

                    }
                    // Stay in same state until commit is received unless

                }
            }
            @Override
            public void onDeleteStatus(int status, String user_id, String identifier) {
                // Ignore
            }
            @Override
            public void onConfigUpdateStatus(int status) {
                // Ignore
            }
            @Override
            public void onRenameStatus(int status) {
                // Ignore
            }
        };

        // Check current state.
        // Enrollment does not complete until ready to commit.
        // Match end of audio should check for match result.
        if (mEnrolling) {
            mLastEnrollCapture = System.currentTimeMillis();
            Log.d(TAG, "poll for enroll train result");
            throwIfFail(Native.enrollCaptureComplete(
                            mJniContext, stopAudioCaptureCallback));
            setState(VoicePrintServiceStates.WAIT_FOR_AUDIO_ENROLLING);
        } else if (mMatching ) {
            Log.d(TAG, "poll for match result");
            mLastMatchCapture = System.currentTimeMillis();
            throwIfFail(Native.matchCaptureComplete(
                            mJniContext, stopAudioCaptureCallback));
        }

    }


    @Override
    public void setConfig(String identifier, Bundle config, IVoicePrintServiceCallback cb) {
        if ( config == null ) {
            Log.d(TAG, "Config cannot be null.");
            return;
        }

        if ( cb != null ) {
            _callback = cb;
        }
        try {
            // Handle the various config
            for (String key : config.keySet()) {
                if ( key.equalsIgnoreCase(THRESHOLD)) {
                    Log.d(TAG, "Received threshold config");
                    int value = config.getInt(THRESHOLD);
                    throwIfFail(Native.setThreshold(mJniContext,
                                                    mJniCallback,
                                                    getCurrentUser(),
                                                    identifier,
                                                    value));
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService#addFrame
     * (byte[])
     */
    @Override
    public void addFrame(Bundle data) throws RemoteException {
        //mState.addFrame(this, buffer);
        //Log.d(TAG, "addFrame buffersize=" + frame.length);
        if ( data == null ) {
            Log.d(TAG, "Data cannot be null.");
            return;
        }

        short[] buffer = byteToShortArray(data.getByteArray(DATA));
        if ( buffer == null || buffer.length == 0 ) {
            Log.d(TAG, "Error with voice data.");
            return;
        }

        // Log.d(TAG_AUDIO, "frame[0]: " + buffer[0]);
        // Log.d(TAG_AUDIO, "frame[" + (buffer.length - 1) + "]: " + buffer[buffer.length - 1]);

        if ( mState == VoicePrintServiceStates.WAIT_FOR_AUDIO_MATCHING ||
                mState == VoicePrintServiceStates.WAIT_FOR_AUDIO_ENROLLING) {
            throwIfFail(Native.processFrame(mJniContext,
                                            mJniCallback, System.currentTimeMillis(),
                                            buffer.length, buffer));
        } else {
            invalidStateTransition("addFrameWithId", mState.toString());
        }

        // No State Change
    }

    /*
     * (non-Javadoc)
     *
     * @see com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService#
     * addFrameWithId(int, byte[])
     */
    @Override
    public void addFrameWithId(int id, Bundle data) throws RemoteException {
        //mState.addFrameWithId(this, id, buffer);
        //Log.d(TAG, "addFrameWithId buffersize=" + frame.length);

        if ( data == null ) {
            Log.d(TAG, "Data cannot be null.");
            return;
        }

        short[] buffer = byteToShortArray(data.getByteArray(DATA));
        if ( buffer == null || buffer.length == 0 ) {
            Log.d(TAG, "Error with voice data.");
            return;
        }

        // Log.d(TAG_AUDIO, "frame[0]: " + buffer[0]);
        // Log.d(TAG_AUDIO, "frame[" + (buffer.length - 1) + "]: " + buffer[buffer.length - 1]);

        if ( mState == VoicePrintServiceStates.WAIT_FOR_AUDIO_MATCHING ||
                mState == VoicePrintServiceStates.WAIT_FOR_AUDIO_ENROLLING) {

            throwIfFail(Native.processFrameWithId(mJniContext,
                                                  mJniCallback, System.currentTimeMillis(),
                                                  (long) id, buffer.length, buffer));
        } else {
            invalidStateTransition("addFrameWithId", mState.toString());
        }
        // Stay in this state.
    }

    /**
     * @brief Conversion method for byte[] to short[]
     * @details Conversion method for byte[] to short[].
     *
     * @param byteArray array to convert
     * @return the short[] conversion or null if data was invalid
     */
    public static short[] byteToShortArray(byte[] byteArray) {
        if ( byteArray == null || byteArray.length == 0 ) {
            return null;
        }
        short[] shortOut = new short[byteArray.length / 2];
        ByteBuffer byteBuffer = ByteBuffer.wrap(byteArray);
        byteBuffer.order(ByteOrder.LITTLE_ENDIAN).asShortBuffer().get(shortOut);
        return shortOut;
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService#rename
     * (java.lang.String, java.lang.String,
     * com.qualcomm.qti.biometrics.voiceprint
     * .service.IVoicePrintServiceCallback)
     */
    @Override
    public void rename(String oldId, String newId, IVoicePrintServiceCallback cb)
    throws RemoteException {

        if (lockResource(cb)) {
            _callback = cb;

            throwIfFail(Native.rename(mJniContext,
                                      mJniCallback, getCurrentUser(), oldId, newId));
        }
        setState(VoicePrintServiceStates.WAIT_FOR_RESULT);
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService#cancel
     * (com
     * .qualcomm.qti.biometrics.voiceprint.service.IVoicePrintServiceCallback)
     */
    @Override
    public void cancel(IVoicePrintServiceCallback cb) throws RemoteException {
        // Can cancel anytime
        throwIfFail(Native.cancel(mJniContext));
        setState(VoicePrintServiceStates.READY);
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService#update
     * (java.lang.String, java.lang.String,
     * com.qualcomm.qti.biometrics.voiceprint
     * .service.IVoicePrintServiceCallback)
     */
    @Override
    public void update(String id, String phrase, IVoicePrintServiceCallback cb)
    throws RemoteException {
        if (isSystemUid() == false) {
            cb.onError(VoicePrintService.PERMISSION_DENIED);
            Log.w(TAG,
                  "update: PERMISSION_DENIED. Insufficient app permission.");
            return;
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService#
     * completeUpdate
     * (com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintServiceCallback
     * )
     */
    @Override
    public void completeUpdate(IVoicePrintServiceCallback cb)
    throws RemoteException {
        if (isSystemUid() == false) {
            cb.onError(VoicePrintService.PERMISSION_DENIED);
            Log.w(TAG,
                  "completeUpdate: PERMISSION_DENIED. Insufficient app permission.");
            return;
        }
    }

    /**
     * Lock resource.
     *
     * @param cb
     *            the cb
     * @return true, if successful
     */
    private boolean lockResource(IVoicePrintServiceCallback cb) {
        try {
            return true;
            //if (_mutex.tryLock()) {
            //} else {
            //  Log.d(TAG, "Resource is locked.");
            // resource is not available
            //  if (cb != null) {
            //      cb.onError(VoicePrintService.RESOURCE_UNAVAILABLE);
            // PTON TODO FIX ME!!!!
            //  }
            //}
        } catch (Exception e) {
            e.printStackTrace();
        }

        return false;
    }

    /**
     * Throw if fail.
     *
     * @param rc
     *            the rc
     * @throws RemoteException
     *             the remote exception
     */
    private void throwIfFail(int rc) throws RemoteException {
        if (rc != 0) {
            _callback = null;
            Log.e(TAG, "The jniContext is null!!");
            throw new RemoteException("Error: " + rc);
        }
    }

    /*
     * (non-Javadoc)
     *
     * @see
     * com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService#version
     * ()
     */
    @Override
    public String version() throws RemoteException {
        return _version;
    }

    /**
     * Check system properties if voice print feature is enabled on this device.
     *
     * @return true, if is voice print enabled
     */
    protected static boolean isVoicePrintEnabled() {

        boolean enabled = SystemProperties.getBoolean(VOP_PROPERTY, false);
        /*
         * try {
         *
         * @SuppressWarnings("rawtypes") Class clazz = null; clazz =
         * Class.forName("android.os.SystemProperties");
         *
         * @SuppressWarnings("unchecked") Method method =
         * clazz.getDeclaredMethod("get", String.class);
         *
         * @SuppressWarnings("unused") String value = (String)
         * method.invoke(null, VOP_PROPERTY); enabled =
         * value.equalsIgnoreCase("true");
         *
         * } catch (Exception e) { e.printStackTrace(); }
         */
        return enabled;
    }

    /*
     * (non-Javadoc)
     *
     * @see java.io.Closeable#close()
     */
    @Override
    public void close() throws IOException {
        try {
            releaseResource();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void releaseResource() {
        try {
            Log.d(TAG, "Releasing resource...");
            //_mutex.unlock();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    /**
     * Gets the current user.
     *
     * @return the current user
     * @throws RemoteException
     *             the remote exception
     */
    private static String getCurrentUser() throws RemoteException {
        String userName = Integer.toString(ActivityManagerNative.getDefault()
                                           .getCurrentUser().id);
        return userName;
    }

    /** The state. */
    private VoicePrintServiceStates mState;

    /**
     * Sets the state.
     *
     * @param state
     *            the new state
     */
    void setState(VoicePrintServiceStates state) {
        mState = state;

        mMatching = state == VoicePrintServiceStates.WAIT_FOR_AUDIO_MATCHING;
        mEnrolling = state == VoicePrintServiceStates.WAIT_FOR_AUDIO_ENROLLING;
    }

    /**
     * Invalid state transition.
     *
     * @param fn
     *            the fn
     * @param state
     *            the state
     */
    private static void invalidStateTransition(String fn, String state) {
        Log.w(TAG, "Invalid function call " + fn + " in state " + state);
    }

    /**
     * The Enum VoicePrintServiceStates.
     */
    enum VoicePrintServiceStates  {

        READY,
        WAIT_FOR_AUDIO_MATCHING,
        WAIT_FOR_AUDIO_ENROLLING,
        WAIT_FOR_RESULT
    }


}
