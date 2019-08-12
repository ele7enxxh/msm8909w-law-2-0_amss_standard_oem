/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.sdk;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.util.Log;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.graphics.PointF;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.view.ViewGroup;
import android.view.View;
import android.view.WindowManager;
import android.graphics.PixelFormat;

import com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintService;
import com.qualcomm.qti.biometrics.voiceprint.service.IVoicePrintServiceCallback;

import java.security.InvalidParameterException;
import java.util.HashMap;
import java.util.Map;



/**
 * The Class VoicePrintManager. The VoicePrintManager is responsible for
 * communicating with the VoicePrint feature on the device. If the VoicePrint
 * feature is not available on the device, the API calls will return
 * FEATURE_NOT_AVAILABLE.
 * <p/>
 * *
 * <p/>
 * The API will provide classes that expose the VoicePrint functionality. To use
 * this SDK,
 * <ul>
 * <li>The minimum API level supported is <b>18</b>.</li>
 * <li>The voice print feature must be enabled on the device.</li>
 * <li>All voice print components are loaded on the device.</li>
 * </ul>
 * <p/>
 * <p/>
 * <p/>
 * The Voice Print SDK is a set of API that includes support for
 * Voice Print recognition/verification. It is a set of user-friendly APIs for
 * enabling and leveraging the Voice Print technology. These features include
 * matching, enrolling, deleting, and updating voiceprint data.
 * <p/>
 * The SDK interacts and binds directly with the VoicePrintService. The SDK
 * makes direct calls to the VoicePrintService via the AIDL interface. Calls to
 * the service should not block. Non-system applications have permission to use
 * a subset of the API. If a non-system app tries to use a privileged command,
 * an error will be returned. The application may query the SDK to check the
 * command set available. System-privileged applications may use all the API.
 * <p/>
 * Applications are responsible for recording and forwarding PCM data to the
 * SDK. The application must provide the audio data including sample rate,
 * channel configuration, and audio format. Processing is done asynchronously.
 * The result of a request is posted through a result interface. Since a request
 * such as match() may require multiple samples to be processed, the final
 * result is posted separately. The app is responsible for maintaining its own
 * state data.
 * <p/>
 * When the SDK is first instantiated, it will check if the OEM has enabled the
 * voice print feature on the device and that framework components are loaded on
 * the device. If either check fails, any API calls will result in a
 * FEATURE_NOT_AVAILABLE response. To use the Voice Print SDK,
 * create a new instance by calling VoicePrintSDK.newInstance(context) or
 * VoicePrintSDK.newInstance(context, mode). The query will fail if the feature
 * is not enabled on the device, components are missing, or there are permission
 * errors. In the same note, newInstance will return null if the feature is not
 * enabled on the device, components are missing, there are permission errors.
 * Before running voiceprint, the user should query isVoicePrintAvailable().
 * <p/>
 * The application is responsible for recording resource management, including
 * if a recording session is already running. The application is responsible for
 * requesting correct permissions to record and handle phone state changes. It
 * is also responsible for handling phone call state changes. If there is an
 * incoming call, the application should tear down the current recording and
 * cancel the request.
 * <p/>
 * <pre class="language-java">
 * Insert sample code here...TODO
 * </pre>
 */
public class VoicePrintManager implements AutoCloseable {


    /**
     * The Constant Logging TAG.
     */
    private final static String TAG = "VoicePrintManager";
    private final static String TAG_AUDIO = "VoicePrintAudio";


    public static final int THRESHOLD_LOW = 3;
    public static final int THRESHOLD_MED = 2;
    public static final int THRESHOLD_HIGH = 1;

    public static String THRESHOLD = "THRESHOLD";

    /**
     * The application context.
     */
    private Context mContext;

    /**
     * The Constant RECORD_AUDIO_PERMISSION.
     */
    private final static String RECORD_AUDIO_PERMISSION = "android.permission.RECORD_AUDIO";

    /**
     * The service.
     */
    private IVoicePrintService mService = null;

    /**
     * The service is bound flag.
     */
    private boolean mIsBound;

    /**
     * The component.
     */
    private ComponentName mComponent;

    private static Map<Integer, Boolean> FeatureMap = new HashMap<Integer, Boolean>();

    private final static String VOP_PROPERTY = "persist.qvop";

    public static final String DATA = "DATA";


    /**
     * Callback listeners.
     */
    private OnResultListener resultListener;
    private OnMatchListener matchListener;
    private OnEnrollListener enrollListener;
    private OnErrorListener errorListener;
    private OnVersionListener versionListener;

    // All features are disabled by default. Verify availability after
    // service has connected to verify that libraries were properly loaded.
    static {
        FeatureMap.put(VoicePrintFeature.VOICEPRINT, isVoicePrintEnabled());
        FeatureMap.put(VoicePrintFeature.MATCH, isVoicePrintEnabled());
        FeatureMap.put(VoicePrintFeature.MATCH_ANY, isSystemUid());
        FeatureMap.put(VoicePrintFeature.ENROLL, isSystemUid());
        FeatureMap.put(VoicePrintFeature.DELETE, isSystemUid());
        FeatureMap.put(VoicePrintFeature.DELETE_ALL, isSystemUid());
        FeatureMap.put(VoicePrintFeature.PHRASE_STRENGTH, isSystemUid());
    }

    /**
     * <p/>
     * Creates a new VoicePrintManager SDK instance. Returns null if feature is
     * not enabled or components are missing on the device. Will throw
     * InvalidParameterException if context is not valid.
     * <p/>
     * The VoicePrintManager is responsible for communicating with the
     * VoicePrint feature on the device. If the VoicePrint feature is not
     * available on the device, the API calls will return FEATURE_NOT_AVAILABLE.
     *
     * @param context the context
     * @return the new instance of VoicePrintManager
     */
    static public VoicePrintManager newInstance(Context context) {
        if (context == null) {
            Log.e(TAG, "Context cannot be null.");
            throw new InvalidParameterException("Context cannot be null.");
        }

        // Check components
        try {
            return new VoicePrintManager(context);
        } catch (RuntimeException e) {

            Log.d(TAG, "Error constructing VoicePrintManager");
        }

        return null;
    }

    /**
     * <p/>
     * The QVoicePrintManager is responsible for communicating with the
     * VoicePrint feature on the device. If the VoicePrint feature is not
     * available on the device, the API calls will return FEATURE_NOT_AVAILABLE.
     * <p/>
     * This constructor will throw an InvalidParameterException if the context
     * is not valid and a RuntimeException if the RECORD_AUDIO permission is not
     * set.
     *
     * @param context the context
     * @throws RuntimeException          the runtime exception
     * @throws InvalidParameterException the invalid parameter exception
     */
    private VoicePrintManager(Context context) throws RuntimeException,
        InvalidParameterException {

        if (context == null) {
            Log.e(TAG, "Context cannot be null.");
            throw new InvalidParameterException("Context cannot be null.");
        }


        if (isVoicePrintEnabled() == false) {
            Log.e(TAG, "VoicePrint is DISABLED on this device.");
            throw new RuntimeException("Permission failure: Voice Print is not enabled on this device.");
        }

        Log.d(TAG, "VoicePrint is ENABLED on this device.");

        //        if (!hasRecordPermission(context)) {
        //            Log.e(TAG, "Permission failure: android.permission.RECORD_AUDIO");
        //            throw new RuntimeException(
        //                    "Permission failure: android.permission.RECORD_AUDIO");
        //        }

        mContext = context;

        try {
            // Start service in case it has not started
            Log.d(TAG, "Trying to start " + IVoicePrintService.class.getName());
            Intent intent = new Intent(IVoicePrintService.class.getName());
            intent.setPackage(IVoicePrintService.class.getPackage().getName());
            mComponent = mContext.startService(intent);

            Log.d(TAG, "binding to the service...");

            // Bind to the service
            mIsBound = mContext.bindService(
                           intent, connection,
                           Context.BIND_AUTO_CREATE);
        } catch (Exception e) {
            Log.e(TAG, "Exception binding to service. " + e.getMessage());
            e.printStackTrace();
            mIsBound = false;
            FeatureMap.put(VoicePrintFeature.VOICEPRINT, false);
            FeatureMap.put(VoicePrintFeature.MATCH, false);
            throw new RuntimeException("VoicePrintServiceError: Cannot bind to the service.");
        } catch (Error e) {
            Log.e(TAG, "Error binding to service. " + e.getMessage());
            e.printStackTrace();
            mIsBound = false;
            FeatureMap.put(VoicePrintFeature.VOICEPRINT, false);
            FeatureMap.put(VoicePrintFeature.MATCH, false);
            throw new RuntimeException("VoicePrintServiceError: Cannot bind to the service.");
        }

        Log.d(TAG, "Bound to Service? " + mIsBound);
        Log.d(TAG, "hasSystemPrivilege? " + isSystemUid());

        if (mIsBound) {
            // Wait until connection is made to verify that features
            // are available on the device.
        } else {
            Log.d(TAG, "Cannot bind to service or insufficient privileges.");
            FeatureMap.put(VoicePrintFeature.VOICEPRINT, false);
            FeatureMap.put(VoicePrintFeature.MATCH, false);
            FeatureMap.put(VoicePrintFeature.MATCH_ANY, false);
            FeatureMap.put(VoicePrintFeature.ENROLL, false);
            FeatureMap.put(VoicePrintFeature.DELETE, false);
            FeatureMap.put(VoicePrintFeature.DELETE_ALL, false);
            FeatureMap.put(VoicePrintFeature.PHRASE_STRENGTH, false);
        }
    }

    /**
     * The connection.
     */
    private ServiceConnection connection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            Log.d(TAG, "Service Connected");
            mService = IVoicePrintService.Stub.asInterface(service);

            boolean hasSystemPrivilege = isSystemUid();

            try {
                // If libraries are not available on the device, disable all features.
                if (mService.isLibraryAvailable()) {
                    FeatureMap.put(VoicePrintFeature.VOICEPRINT, true);
                    FeatureMap.put(VoicePrintFeature.MATCH, true);
                    FeatureMap.put(VoicePrintFeature.MATCH_ANY, hasSystemPrivilege);
                    FeatureMap.put(VoicePrintFeature.ENROLL, hasSystemPrivilege);
                    FeatureMap.put(VoicePrintFeature.DELETE, hasSystemPrivilege);
                    FeatureMap.put(VoicePrintFeature.DELETE_ALL, hasSystemPrivilege);
                    // Not available yet
                    // FeatureMap.put(VoicePrintFeature.PHRASE_STRENGTH, hasSystemPrivilege);
                } else {
                    FeatureMap.put(VoicePrintFeature.VOICEPRINT, false);
                    FeatureMap.put(VoicePrintFeature.MATCH, false);
                    FeatureMap.put(VoicePrintFeature.MATCH_ANY, false);
                    FeatureMap.put(VoicePrintFeature.ENROLL, false);
                    FeatureMap.put(VoicePrintFeature.DELETE, false);
                    FeatureMap.put(VoicePrintFeature.DELETE_ALL, false);
                    FeatureMap.put(VoicePrintFeature.PHRASE_STRENGTH, false);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            Log.d(TAG, "Service Disconnected");
            mService = null;
        }
    };

    /**
     * Checks for record_audio permission.
     *
     * @param context the context
     * @return true, if successful
     */
    private boolean hasRecordPermission(Context context) {
        try {
            int res = context
                      .checkCallingOrSelfPermission(RECORD_AUDIO_PERMISSION);
            return (res == PackageManager.PERMISSION_GRANTED);
        } catch (Exception e) {
            e.printStackTrace();
        }

        return false;
    }

    /*
     * (non-Javadoc)
     *
     * @see java.lang.Object#finalize()
     */
    @Override
    public void finalize() {
        close();
    }

    /**
     * Check system properties if voice print feature is enabled on this device.
     *
     * @return
     */
    private static boolean isVoicePrintEnabled() {
        boolean enabled = SystemProperties.getBoolean(VOP_PROPERTY, false);
        return enabled;
    }

    /*
     * (non-Javadoc)
     *
     * @see java.lang.AutoCloseable#close()
     */
    @Override
    public synchronized void close() {
        if (mService != null) {
            try {
                mService.cancel(_callback);
                mContext.unbindService(connection);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    /**
     * Gets the service.
     *
     * @return the service
     * @throws Exception the exception
     */
    private IVoicePrintService getService() throws Exception {
        // Service should have started
        if (mComponent == null) {
            throw new Exception("Voice Print service never started correctly!");
        }

        // Should be connected to the service
        if (mIsBound == false) {
            throw new Exception("Failed to bind to Voice Print Service");
        }

        if (mService != null) {
            return mService;
        }

        throw new Exception("No Voice Print Service available");
    }


    /**
     * Checks if is feature available. Static method to query if a feature is
     * available. Some APIs require system privilege. For example, query if
     * UPDATE is available for the app.
     *
     * @param feature the feature
     * @return true, if is feature available
     */
    public boolean isFeatureAvailable(int feature) {
        return FeatureMap.get(feature);
    }

    /**
     * Checks if is voice print available. Checks if the feature is enabled and
     * all necessary components are on the device.
     *
     * @return true, if is voice print available
     */
    public boolean isVoicePrintAvailable() {
        return FeatureMap.get(VoicePrintFeature.VOICEPRINT);
    }

    /*
    public void setMatchListener(OnMatchListener listener) {
        matchListener = listener;
    }

    public void setEnrollListener(OnEnrollListener listener) {
        enrollListener = listener;
    }*/

    /**
     * Match. Check if the current user is a match. Result posted will be MATCH,
     * NO_MATCH, or an error. Additional data will be bundled with the result.
     *
     * @return the result constant
     */
    public int match() {
        Log.d(TAG, "match called phraseId=NULL");
        try {
            if (internalFeatureCheck(VoicePrintFeature.MATCH) == false) {
                Log.d(TAG, "feature is not available");
                return VoicePrintError.FEATURE_NOT_AVAILABLE;
            }

            getService().match(_callback);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Match. Check if the current user is a match. Result posted will be MATCH,
     * NO_MATCH, or an error. Additional data will be bundled with the result.
     *
     * @param callback the callback
     * @return the result constant
     */
    public int match(com.qualcomm.qti.biometrics.voiceprint.sdk.OnMatchListener callback) {
        Log.d(TAG, "match called phraseId=NULL");


        matchListener = callback;

        return match();
    }

    /**
     * Match. Check if the current user is a match to particular identifier.
     * Result posted will be MATCH, NO_MATCH, or an error. Additional data will
     * be bundled with the result.
     *
     * @param phraseId unique phrase identifier
     * @param callback the callback
     * @return the result constant
     */
    public int match(String phraseId, com.qualcomm.qti.biometrics.voiceprint.sdk.OnMatchListener callback) {
        Log.d(TAG, "match called phraseId=" + phraseId);


        matchListener = callback;


        try {
            if (internalFeatureCheck(VoicePrintFeature.MATCH) == false) {
                Log.d(TAG, "feature is not available");
                return VoicePrintError.FEATURE_NOT_AVAILABLE;
            }

            getService().matchWithId(phraseId, _callback);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Match any. Check if voice is a match to any user in the device database.
     * Result posted will be MATCH, NO_MATCH, or an error. If a match is a
     * found, additional data will be bundled with the result.
     *
     * @return the result constant
     */
    public int matchAny() {

        try {
            if (internalFeatureCheck(VoicePrintFeature.MATCH_ANY) == false) {
                return VoicePrintError.FEATURE_NOT_AVAILABLE;
            }

            getService().matchAny(_callback);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Match any. Check if voice is a match to any user in the device database.
     * Result posted will be MATCH, NO_MATCH, or an error. If a match is a
     * found, additional data will be bundled with the result.
     *
     * @param keyphrase the keyphrase to search for
     * @return the result constant
     */
    public int matchAny(String keyphrase) {
        try {
            if (internalFeatureCheck(VoicePrintFeature.MATCH_ANY) == false) {
                return VoicePrintError.FEATURE_NOT_AVAILABLE;
            }

            getService().matchAnyKeyPhrase(keyphrase, _callback);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Match any. Check if voice is a match to any user in the device database.
     * Result posted will be MATCH, NO_MATCH, or an error. If a match is a
     * found, additional data will be bundled with the result.
     *
     * @param keyphrase the keyphrase to search for
     * @param callback  the listener
     * @return the result constant
     */
    public int matchAny(String keyphrase, com.qualcomm.qti.biometrics.voiceprint.sdk.OnMatchListener callback) {

        matchListener = callback;

        return matchAny(keyphrase);
    }

    /**
     * Match any. Check if voice is a match to any user in the device database.
     * Result posted will be MATCH, NO_MATCH, or an error. If a match is a
     * found, additional data will be bundled with the result.
     *
     * @param callback the listener
     * @return the result constant
     */
    public int matchAny(com.qualcomm.qti.biometrics.voiceprint.sdk.OnMatchListener callback) {

        matchListener = callback;

        return matchAny();
    }

    /**
     * On start audio capture. PCM recording information.
     *
     * @param sampleRate       the sample rate expressed in Hz (16000)
     * @param numberOfChannels the configuration of the audio channels (1 for mono, 2 for
     *                         stereo)
     * @param audioFormat      the format in which the audio data is represented -
     *                         ENCODING_PCM_16BIT, ENCODING_PCM_8BIT. (8, 16)
     * @return the result constant
     */
    public int onStartAudioCapture(int sampleRate,
                                   int numberOfChannels,
                                   int audioFormat,
                                   com.qualcomm.qti.biometrics.voiceprint.sdk.OnErrorListener callback) {

        errorListener = callback;

        try {
            getService().onStartAudioCapture(sampleRate, numberOfChannels, audioFormat, _callback);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Adds the frame. Sends PCM data to the service. This may be continuous.
     * The service is responsible for buffering all the PCM data. Will return an
     * error if PCM information has not been provided. onStartAudioCapture must
     * have been called before addFrame is valid.
     *
     * @param buffer the array to which the recorded audio data is written
     * @return the result constant
     */
    public int addFrame(byte[] buffer) {
        try {
            Bundle data = new Bundle();
            Log.d(TAG_AUDIO, "frame[0]: " + buffer[0]);
            Log.d(TAG_AUDIO, "frame[" + (buffer.length - 1) + "]: " + buffer[buffer.length - 1]);
            data.putByteArray(DATA, buffer);
            getService().addFrame(data);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }


    /**
     * Adds the frame. Sends PCM data to the service. This may be continuous.
     * The service is responsible for buffering all the PCM data. Will return an
     * error if PCM information has not been provided. onStartAudioCapture must
     * have been called before addFrame is valid.
     *
     * @param frame_id id of the particular frame. This will be used to let the
     *                 application know which frame an EoP (end of phrase) has been
     *                 detected in.
     * @param buffer   the array to which the recorded audio data is written
     * @return the result constant
     */
    public int addFrame(int frame_id, byte[] buffer) {
        try {
            Bundle data = new Bundle();
            Log.d(TAG_AUDIO, "frame[0]: " + buffer[0]);
            Log.d(TAG_AUDIO, "frame[" + (buffer.length - 1) + "]: " + buffer[buffer.length - 1]);
            data.putByteArray(DATA, buffer);
            getService().addFrameWithId(frame_id, data);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * checkPhraseStrength. Check the relative strength of the phrase. Value of
     * phrase strength will be posted in result.
     *
     * @param phrase the phrase to check against
     * @return the result constant
     */
    public int checkPhraseStrength(String phrase) {
        if (internalFeatureCheck(VoicePrintFeature.PHRASE_STRENGTH) == false) {
            return VoicePrintError.FEATURE_NOT_AVAILABLE;
        }

        return 0;
    }

    /**
     * On stop audio capture. Called when the last sample has been sent. Result
     * will not be posted until this method has been called.
     *
     * @return the result constant
     */
    public int onStopAudioCapture(com.qualcomm.qti.biometrics.voiceprint.sdk.OnResultListener callback) {

        resultListener = callback;

        try {
            getService().onStopAudioCapture(_callback);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Enroll. Start an enrollment session. The enrolled user will be the
     * current user. Additional data will be bundled with the posted result.
     *
     * @param identifier unique phrase identifier
     * @param phrase     the phrase
     * @return the result constant
     */
    public int enroll(String identifier, String phrase, int threshold) {
        try {
            if (internalFeatureCheck(VoicePrintFeature.ENROLL) == false) {
                return VoicePrintError.FEATURE_NOT_AVAILABLE;
            }

            Log.d(TAG, "_callbackValid=" + (_callback != null));
            getService().enroll(identifier, phrase, threshold, _callback);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Enroll. Start an enrollment session. The enrolled user will be the
     * current user . Additional data will be bundled with the posted result.
     *
     * @param identifier unique phrase identifier
     * @param phrase     the phrase
     * @param callback   the listener
     * @return the result constant
     */
    public int enroll(String identifier, String phrase, int threshold,
                      OnEnrollListener callback) {

        enrollListener = callback;


        Log.d(TAG, "Have enroll listener? " + (enrollListener != null));

        return enroll(identifier, phrase, threshold);
    }

    public int setThreshold(String identifier, int threshold) {
        try {
            Log.d(TAG, "Setting threshold " + identifier + "=" + threshold);
            Bundle config = new Bundle();
            config.putInt(THRESHOLD, threshold);
            getService().setConfig(identifier, config, _callback);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return VoicePrintError.NO_ERROR;
    }

    /**
     * Complete enroll. End an enrollment session.The voice model will be saved
     * for the user.
     *
     * @return the result constant
     */
    public int completeEnroll(OnErrorListener callback) {
        try {
            // Do not overwrite the enroll listener
            errorListener = callback;


            getService().completeEnroll(_callback);
            Log.d(TAG, "Completing enrollment");
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Delete. Start an delete session. All of the current users enrolled data
     * will be deleted. Result will be posted.
     *
     * @return the result constant
     */
    public int delete() {
        try {
            if (internalFeatureCheck(VoicePrintFeature.DELETE) == false) {
                return VoicePrintError.FEATURE_NOT_AVAILABLE;
            }

            getService().delete(_callback);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Delete. Start an delete session. The current users particular voicepint
     * template with identifier will be deleted. Result will be posted.
     *
     * @param identifier unique phrase identifier
     * @return the result constant
     */
    public int delete(String identifier) {
        try {
            if (internalFeatureCheck(VoicePrintFeature.DELETE) == false) {
                return VoicePrintError.FEATURE_NOT_AVAILABLE;
            }

            Log.d(TAG, "Delete " + identifier);
            getService().deleteWithId(identifier, _callback);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Delete. Start an delete session. All of the current users enrolled data
     * will be deleted. Result will be posted.
     *
     * @param callback the listener
     * @return the result constant
     */
    public int delete(com.qualcomm.qti.biometrics.voiceprint.sdk.OnResultListener callback) {

        resultListener = callback;

        return delete();
    }

    /**
     * Delete. Start an delete session. The current users particular voicepint
     * template with identifier will be deleted. Result will be posted.
     *
     * @param identifier unique phrase identifier
     * @param callback   the listener
     * @return the result constant
     */
    public int delete(String identifier, com.qualcomm.qti.biometrics.voiceprint.sdk.OnResultListener callback) {

        resultListener = callback;

        return delete(identifier);
    }

    /**
     * Delete all. Start an delete session. All users will be deleted from the
     * database. Result will be posted.
     *
     * @return the result constant
     */
    public int deleteAll() {
        try {
            if (internalFeatureCheck(VoicePrintFeature.DELETE_ALL) == false) {
                return VoicePrintError.FEATURE_NOT_AVAILABLE;
            }

            getService().deleteAll(_callback);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Delete all. Start an delete session. All users will be deleted from the
     * database. Result will be posted.
     *
     * @param callback the listener
     * @return the result constant
     */
    public int deleteAll(OnResultListener callback) {

        resultListener = callback;

        return deleteAll();
    }

    /**
     * Update. Start an update session. The updated user will be the current
     * user. Result will be. If user/identifier is not in the database,
     * user/identifier will be added if update completes.
     *
     * @param identifier unique phrase identifier
     * @param phrase     the phrase
     * @return the result constant
     */
    public int update(String identifier, String phrase) {
        try {
            if (internalFeatureCheck(VoicePrintFeature.UPDATE) == false) {
                return VoicePrintError.FEATURE_NOT_AVAILABLE;
            }

            getService().update(identifier, phrase, _callback);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Complete update. End an update session. The old voice print model will be
     * deleted. The new voice model will be saved for the user.
     *
     * @return the result constant
     */
    public int completeUpdate() {
        try {
            getService().completeUpdate(_callback);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Cancel. Cancels current action. Action must be associated with app and
     * cannot cancel action request from another app.
     *
     * @return the result constant
     */
    public int cancel() {
        try {
            getService().cancel(_callback);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Rename a voiceprint template.
     *
     * @param oldKey
     * @param newKey
     * @return
     */
    public int rename(String oldKey, String newKey) {
        try {
            getService().rename(oldKey, newKey, _callback);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Rename a voiceprint template.
     *
     * @param oldKey
     * @param newKey
     * @param callback
     * @return
     */
    public int rename(String oldKey, String newKey, OnResultListener callback) {

        resultListener = callback;

        return rename(oldKey, newKey);
    }

    /**
     * Check is System UID.
     *
     * @return true if a system app
     */
    private static boolean isSystemUid() {
        return (android.os.Process.myUid() == android.os.Process.SYSTEM_UID);
    }

    /**
     * Check if feature is enabled.
     *
     * @param feature
     * @return true if available else false
     */
    private boolean internalFeatureCheck(int feature) {
        if (FeatureMap.get(feature)) {
            return true;
        }
        return false;
    }


    /**
     * Messages for AIDL callback.
     */
    private final int MSG_MATCH_RESULT = 1;
    private final int MSG_ERROR = 2;
    private final int MSG_ENROLL_RESULT = 3;
    private final int MSG_RESULT = 4;
    private final int MSG_VERSION = 5;

    /**
     * The handler.
     */
    Handler mHandler = new Handler(Looper.getMainLooper()) {
        @Override
        public void handleMessage(Message msg) {

            switch (msg.what) {
            case MSG_MATCH_RESULT:
                if (matchListener != null) {
                    Bundle bundle = null;
                    if (msg.obj instanceof Bundle) {
                        bundle = (Bundle) msg.obj;
                    }
                    matchListener.onMatchStatus((msg.arg1 == VoicePrintError.NO_ERROR), bundle);
                }
                matchListener = null;
                break;
            case MSG_ERROR:
                // Notify all listeners.
                if (errorListener != null) {
                    errorListener.onError(msg.arg1);
                }
                errorListener = null;

                if (matchListener != null) {
                    matchListener.onError(msg.arg1);
                }
                matchListener = null;

                if (enrollListener != null) {
                    enrollListener.onError(msg.arg1);
                }
                enrollListener = null;
                break;
            case MSG_ENROLL_RESULT:
                if (enrollListener != null) {
                    Bundle bundle = null;
                    if (msg.obj instanceof Bundle) {
                        bundle = (Bundle) msg.obj;
                    }
                    enrollListener.onEnrollComplete((msg.arg1 == VoicePrintError.NO_ERROR), bundle);
                }
                enrollListener = null;
                break;
            case MSG_RESULT:
                if (resultListener != null) {
                    Bundle bundle = null;
                    if (msg.obj instanceof Bundle) {
                        bundle = (Bundle) msg.obj;
                    }
                    resultListener.onResult(msg.arg1, bundle);
                }
                resultListener = null;
                break;
            case MSG_VERSION:
                if (versionListener != null) {
                    versionListener.onVersionInfo(msg.arg1, msg.arg2);
                }
                versionListener = null;
                break;
            default:
                Log.d(TAG, "Not handling msg " + msg.what);
                break;
            }
        }
    };

    /* *
    * This implementation is used to receive callbacks from the remote
    * service.
    */
    private IVoicePrintServiceCallback _callback = new IVoicePrintServiceCallback.Stub() {
        @Override
        public void onResult(int code, Bundle data) throws RemoteException {
            Log.d(TAG, "onResult code=" + code);
            Message msg = mHandler.obtainMessage(MSG_RESULT, code, 0, data);
            mHandler.sendMessage(msg);
        }

        @Override
        public void onError(int error) throws RemoteException {
            Log.d(TAG, "onError error=" + error);
            Message msg = mHandler.obtainMessage(MSG_ERROR, error, 0, null);
            mHandler.sendMessage(msg);
        }

        @Override
        public void onMatchStatus(int status, String user_id, String identifier,
                                  Bundle data) throws RemoteException {
            Log.d(TAG, "onMatchStatus status=" + status);

            boolean success = (status == VoicePrintError.NO_ERROR);

            // Send a response to the matchListener
            if (matchListener == null) {
                Log.d(TAG, "No match listener is set");
                return;
            }

            if ( data == null ) {
                data = new Bundle();
            } else if ( data.getString(OnResultBase.DEBUG) != null ) {
                Log.d(TAG, "DEBUG: " + data.getString(OnResultBase.DEBUG));
            }

            if (user_id != null) {
                data.putString(OnResultBase.DATA_USER_ID, user_id);
            }
            if (identifier != null) {
                data.putString(OnResultBase.DATA_KEYPHRASE_ID, identifier);
            }

            // Error Code
            data.putInt(OnResultBase.FAILURE_CODE, status);
            Message msg = mHandler.obtainMessage(MSG_MATCH_RESULT, status, 0, data);
            mHandler.sendMessage(msg);
        }

        @Override
        public void onEnrollStatus(int status) throws RemoteException {
            Log.d(TAG, "onEnrollStatus status=" + status);

            boolean success = (status == VoicePrintError.NO_ERROR);

            // Send a response to the matchListener
            if (enrollListener == null) {
                Log.d(TAG, "No enroll listener is set");
                return;
            }

            Bundle data = new Bundle();


            // TODO
            // TZ needs to send back user id and other information.
            /*
            if (user_id != null) {
                data.putString(OnResultBase.DATA_USER_ID, user_id);
            }
            if (identifier != null) {
                data.putString(OnResultBase.DATA_KEYPHRASE_ID, identifier);
            }*/


            data.putInt(OnResultBase.FAILURE_CODE, status);
            Message msg = mHandler.obtainMessage(MSG_ENROLL_RESULT, status, 0, data);
            mHandler.sendMessage(msg);
        }

        @Override
        public void onDeleteStatus(int status, String user_id, String identifier) throws RemoteException {
            Log.d(TAG, "onDeleteStatus status=" + status);
            if (resultListener == null) {
                return;
            }

            boolean success = (status == VoicePrintError.NO_ERROR);

            // Send a response to the matchListener
            if (resultListener == null) {
                Log.d(TAG, "No result listener is set");
                return;
            }

            Bundle data = new Bundle();


            if (user_id != null) {
                data.putString(OnResultBase.DATA_USER_ID, user_id);
            }
            if (identifier != null) {
                data.putString(OnResultBase.DATA_KEYPHRASE_ID, identifier);
            }


            data.putInt(OnResultBase.FAILURE_CODE, status);
            Message msg = mHandler.obtainMessage(MSG_RESULT, status, 0, data);
            mHandler.sendMessage(msg);
        }

        @Override
        public void onVersionInfo(long major, long minor) throws RemoteException {
            Log.d(TAG, "onVersionInfo version=" + major + "." + minor);
            if (versionListener == null) {
                return;
            }

            Bundle data = new Bundle();
            data.putLong(OnVersionListener.VERSION_MAJOR, major);
            data.putLong(OnVersionListener.VERSION_MINOR, minor);
            Message msg = mHandler.obtainMessage(MSG_RESULT, (int) major, (int) minor, data);
            mHandler.sendMessage(msg);

        }

        @Override
        public void onConfigUpdateStatus(int status) throws RemoteException {
            Log.d(TAG, "onConfigUpdateStatus status=" + status);
            if (resultListener == null) {
                return;
            }

            boolean success = (status == VoicePrintError.NO_ERROR);

            // Send a response to the matchListener
            if (resultListener == null) {
                Log.d(TAG, "No result listener is set");
                return;
            }

            Bundle data = new Bundle();
            data.putInt(OnResultBase.FAILURE_CODE, status);

            Message msg = mHandler.obtainMessage(MSG_RESULT, status, 0, data);
            mHandler.sendMessage(msg);
        }

        @Override
        public void onRenameStatus(int status) throws RemoteException {
            Log.d(TAG, "onRenameStatus status=" + status);
            if (resultListener == null) {
                return;
            }

            boolean success = (status == VoicePrintError.NO_ERROR);

            // Send a response to the matchListener
            if (resultListener == null) {
                Log.d(TAG, "No result listener is set");
                return;
            }

            Bundle data = new Bundle();
            data.putInt(OnResultBase.FAILURE_CODE, status);

            Message msg = mHandler.obtainMessage(MSG_RESULT, status, 0, data);
            mHandler.sendMessage(msg);
        }

        @Override
        public IBinder asBinder() {
            return super.asBinder();
        }

    };


}
