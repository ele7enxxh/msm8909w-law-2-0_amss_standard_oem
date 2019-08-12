/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo;

import android.app.Application;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
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
import android.content.Context;
import android.os.Bundle;

import com.qualcomm.qti.biometrics.voiceprint.sdk.VoicePrintManager;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.audio.AudioRecordManager;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.KeyPhrase;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.KeyPhraseAction;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.VoicePrintDatabase;
import com.qualcomm.qti.biometrics.voiceprint.sdk.OnResultBase;


import java.util.List;

public class VoicePrintDemoApp extends Application {

    private final static String TAG = "VoicePrint";

    //    private KeyPhraseList mKeyPhraseList = null;
    private KeyPhrase mCurrentKeyPhrase = new KeyPhrase();

    private KeyPhraseAction[] mActions;

    private static VoicePrintDemoApp mInstance;

    private VoicePrintManager mManager;

    private DebugView debugView;

    private boolean debugMode = false;

    public VoicePrintDemoApp() {

    }

    @Override
    public void onCreate() {
        super.onCreate();
        VoicePrintDatabase.instantiate(this);
        AudioRecordManager.instantiate(this);
        createActions();
        mInstance = this;
        mManager = VoicePrintManager.newInstance(this);

        // Debug View
        debugView = new DebugView(this);

    }

    public static final String DATA = "DATA";

    String mDebugText = null;

    public void update(Bundle data) {
        try {
            if ( data == null ) {
                mDebugText = null;
            } else {
                mDebugText = data.getString(OnResultBase.DEBUG);
                if ( mDebugText != null ) {
                    Log.d(TAG, mDebugText);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        // Log.d(TAG, "Try draw");
        debugView.invalidate();
    }

    public void onLongBackPress() {

        // Long Press back will switch mode
        debugMode = !debugMode;

        WindowManager windowManager = (WindowManager) getSystemService(Context.WINDOW_SERVICE);

        try {
            if ( debugMode ) {

                windowManager.addView(
                    debugView,
                    new WindowManager.LayoutParams(
                        WindowManager.LayoutParams.FILL_PARENT,
                        WindowManager.LayoutParams.FILL_PARENT,
                        WindowManager.LayoutParams.TYPE_SYSTEM_OVERLAY,
                        WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN,
                        PixelFormat.TRANSLUCENT));
            } else {
                windowManager.removeView(debugView);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
    * The Class DebugView.
    */
    class DebugView extends View {


        /** The text paint. */
        private Paint mTextPaint;


        /**
         * Instantiates a new count down view.
         *
         * @param context
         *            the context
         */
        public DebugView(Context context) {
            super(context);

            init();

        }

        /**
         * Inits the.
         */
        private void init() {
            mTextPaint = new Paint();
            mTextPaint.setAntiAlias(true);
            mTextPaint.setTextSize(40);
            mTextPaint.setTypeface(Typeface.DEFAULT);
            mTextPaint.setTextAlign(Paint.Align.CENTER);
            mTextPaint.setColor(Color.BLACK);
            mTextPaint.setAlpha(0xff);

        }


        /*
        * (non-Javadoc)
        *
        * @see android.view.View#onDraw(android.graphics.Canvas)
        */
        @Override
        protected void onDraw(Canvas canvas) {
            try {
                //Log.d(TAG, "Draw");
                //canvas.drawText("HELLO!!!!", 100, 100, mTextPaint);

                if ( mDebugText != null ) {
                    canvas.drawText(mDebugText, 800, 300, mTextPaint);
                } else {
                    Log.d(TAG, "mDebugText is null");
                }

            } catch (Exception e) {
                e.printStackTrace();
                Log.d(TAG, "Excepting in drawing view " + e.getMessage());

            }


        }

    }

    public static VoicePrintDemoApp getInstance() {
        return mInstance;
    }

    public VoicePrintManager getManager() {
        return mManager;
    }

    public KeyPhraseAction getAction(int id) {
        if (id >= 0 && id < mActions.length) {
            return mActions[id];
        }
        return null;
    }


    public String getCurrentActionName() {
        KeyPhraseAction action = null;
        if (mCurrentKeyPhrase != null) {
            action = getAction(mCurrentKeyPhrase.getActionId());
        }
        return action == null ? "" : action.toString();
    }

    private void createActions() {
        mActions = new KeyPhraseAction[3];

        // Find by traversing list for email clients.
        // Use first one found.

        // Email
        Intent intent = new Intent(Intent.ACTION_MAIN);
        intent.addCategory(Intent.CATEGORY_APP_EMAIL);
        PackageManager pkgManager = getPackageManager();
        List<ResolveInfo> activities = pkgManager.queryIntentActivities(intent, 0);
        if (activities.size() > 0) {
            ResolveInfo resolveInfo = activities.get(0);
            mActions[KeyPhraseAction.EMAIL] = new KeyPhraseAction(KeyPhraseAction.EMAIL,
                    getString(R.string.email), resolveInfo);

        }

        // Browser
        intent = new Intent(Intent.ACTION_MAIN);
        intent.addCategory(Intent.CATEGORY_APP_BROWSER);
        activities = pkgManager.queryIntentActivities(intent, 0);
        if (activities.size() > 0) {
            ResolveInfo resolveInfo = activities.get(0);
            mActions[KeyPhraseAction.BROWSER] = new KeyPhraseAction(KeyPhraseAction.BROWSER,
                    getString(R.string.google), resolveInfo);
        }

        // SMS
        intent = new Intent(Intent.ACTION_MAIN);
        intent.addCategory(Intent.CATEGORY_APP_MESSAGING);
        activities = pkgManager.queryIntentActivities(intent, 0);
        if (activities.size() > 0) {
            ResolveInfo resolveInfo = activities.get(0);
            mActions[KeyPhraseAction.SMS] = new KeyPhraseAction(KeyPhraseAction.SMS,
                    getString(R.string.sms), resolveInfo);
        }
    }

    public void setKeyPhrase(KeyPhrase phrase) {

        mCurrentKeyPhrase = phrase;

        if ( mCurrentKeyPhrase != null ) {
            Log.d(TAG, "Updating current keyphrase");
            Log.d(TAG, mCurrentKeyPhrase.toString());
        }
    }

    public void setKeyPhraseName(String name) {
        mCurrentKeyPhrase.setName(name);
    }

    public void setKeyPhrase(String phrase) {
        mCurrentKeyPhrase.setPhrase(phrase);
    }

    public KeyPhrase getCurrentKeyPhrase() {
        return mCurrentKeyPhrase;
    }

    public void setKeyPhraseAction(int action) {
        mCurrentKeyPhrase.setActionId(action);
    }

    public void setKeyPhraseAction(KeyPhraseAction action) {
        mCurrentKeyPhrase.setActionId(action.getId());
    }
}
