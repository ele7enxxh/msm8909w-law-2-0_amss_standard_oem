/*
 * Copyright (c) 2015-16 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchhome;

import android.app.Activity;
import android.app.WallpaperInfo;
import android.app.WallpaperManager;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.graphics.PixelFormat;
import android.os.Bundle;
import android.service.wallpaper.WallpaperService;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewConfiguration;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.AnalogClock;

import com.qualcomm.qti.contextualmodelib.ContextualModeHelper;
import com.qualcomm.qti.contextualmodelib.ContextualModeListener;

import org.xmlpull.v1.XmlPullParserException;

import java.io.IOException;
import java.util.List;

public class WatchHomeActivity extends Activity {
    private static final String APP_TAG = "WatchHome";
    private static final String REQUEST_STATE_ACTION =
        "com.google.android.wearable.watchfaces.action.REQUEST_STATE";
    private static final int REQUEST_CODE_SIM_STATE_PIN_VALIDATE = 107;
    private static final int REQUEST_CODE_SIM_STATE_PUK_VALIDATE = 108;
    private AnalogClock mAnalogClock;
    private WallpaperManager mWallpaperManager;

    private ContextualModeHelper mContextualModeHelper =
            new ContextualModeHelper();

    private boolean mInContextualMode = false;
    private boolean mPINRequired = false;
    private boolean mPUKRequired = false;

    private void updateWallpaperState(boolean shouldDoContextualUpdate) {
        Bundle bundle = new Bundle();

        if (mInContextualMode) {
            bundle.putBoolean("ambient_mode", true);
        } else {
            bundle.putBoolean("ambient_mode", false);
        }

        final View layout = (View) findViewById(
                com.qualcomm.qti.watchhome.R.id.watch_home);

        if(layout.getWindowToken() != null) {
            mWallpaperManager.sendWallpaperCommand(
                    layout.getWindowToken(),
                    "com.google.android.wearable.action.BACKGROUND_ACTION",
                    0, 0, 0, bundle);

            if (shouldDoContextualUpdate) {
                mWallpaperManager.sendWallpaperCommand(
                        layout.getWindowToken(),
                        "com.google.android.wearable.action.AMBIENT_UPDATE",
                        0, 0, 0, null);

                Log.i(APP_TAG,
                        "Sending ambient update command to wallpaper");
            } else {
                Log.i(APP_TAG,
                        "Sent ambient_mode enter command to wallpaper");
            }
        }
    }

    private ContextualModeListener mContextualModeListener =
            new ContextualModeListener() {
                @Override
                public void contextualModeEntered() {
                    mInContextualMode = true;
                    updateWallpaperState(false);
                }

                @Override
                public void contextualModeExited() {
                    mInContextualMode = false;
                    updateWallpaperState(false);
                }

                @Override
                public void contextualModeUpdate() {
                    updateWallpaperState(true);
                }
            };

    private BroadcastReceiver mWatchfaceBroadcastReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getAction().equals(REQUEST_STATE_ACTION)) {
                updateWallpaperState(false);
            }
            if (intent.getAction().equals("android.intent.action.SIM_STATE_CHANGED")) {
                handleSIMStateChanged();
            }
        }
    };

    //verify if SIM PIN is required and start PIN/PUK entry.
    private void handleSIMStateChanged() {
        TelephonyManager tm = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
        int simStatieNum = tm.getSimState();
        Log.i(APP_TAG, "SIM state " + simStatieNum);
        switch (simStatieNum) {
        case TelephonyManager.SIM_STATE_PIN_REQUIRED:
            Log.i(APP_TAG, "SIM PIN Required");
            mPINRequired = true;
            mPUKRequired = false;
            validateSIMCard();
            break;
        case TelephonyManager.SIM_STATE_PUK_REQUIRED:
            Log.i(APP_TAG, "SIM PUK Required");
            mPINRequired = false;
            mPUKRequired = true;
            validateSIMCard();
            break;
        default:
            break;
        }
     }

    private boolean validateSIMCard(){
        if(mPINRequired){
            Intent validatePINIntent = new Intent(this, ValidateSIMPIN.class);
            startActivityForResult(validatePINIntent, REQUEST_CODE_SIM_STATE_PIN_VALIDATE);
            return mPINRequired;
        }else if(mPUKRequired){
            Intent validatePUKIntent = new Intent(this, ValidateSIMPUK.class);
            startActivityForResult(validatePUKIntent, REQUEST_CODE_SIM_STATE_PUK_VALIDATE);
            return mPUKRequired;
        }else {
            return false;
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {

        if (requestCode == REQUEST_CODE_SIM_STATE_PIN_VALIDATE) {
            if(resultCode == Activity.RESULT_OK){
                mPINRequired = false;
                openLauncher();
            }
        }else if (requestCode == REQUEST_CODE_SIM_STATE_PUK_VALIDATE) {
            if(resultCode == Activity.RESULT_OK){
                mPUKRequired = false;
                openLauncher();
            }
        }
    }

    public void openLauncher() {
        Log.i(APP_TAG, "Launching the custom launcher");
        PackageManager pm = getPackageManager();
        final String LAUNCHER_TAG = getResources().getString(
                R.string.launcher_package_name);
        Intent launchIntent = pm.getLaunchIntentForPackage(
                LAUNCHER_TAG);
        if (launchIntent == null) {
            Log.e(APP_TAG,
                   "Launch intent for custom launcher not found");
        } else if(!validateSIMCard()){
            startActivity(launchIntent);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(APP_TAG, "OnCreate");

        setContentView(
            com.qualcomm.qti.watchhome.R.layout.activity_home);

        mAnalogClock = (AnalogClock) findViewById(R.id.analogClock);

        SharedPreferences wallpaper = this.getSharedPreferences(
                "wallpaper", MODE_PRIVATE);
        final SharedPreferences.Editor editor = wallpaper.edit();

        //Set wallpaper window parameters
        int wpFlags = WindowManager.LayoutParams.FLAG_SHOW_WALLPAPER;
        int currFlags = getWindow().getAttributes().flags &
                (WindowManager.LayoutParams.FLAG_SHOW_WALLPAPER);

        if (wpFlags != currFlags) {
            getWindow().setFlags(wpFlags,
                WindowManager.LayoutParams.FLAG_SHOW_WALLPAPER);
        }

        final WindowManager.LayoutParams params =
            new WindowManager.LayoutParams(
                WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.TYPE_SYSTEM_ALERT,
                WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN,
                PixelFormat.TRANSLUCENT);

        params.width = ViewGroup.LayoutParams.MATCH_PARENT;
        params.height = ViewGroup.LayoutParams.MATCH_PARENT;

        getWindow().setType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD);

        //Get WallpaperManager instance
        mWallpaperManager = WallpaperManager.getInstance(this);

        //Get WatchHome activity layout
        final View layout = (View) findViewById(
            com.qualcomm.qti.watchhome.R.id.watch_home);

        //Remember wallpaper selected for future boot ups
        boolean wallpaperSetState = wallpaper.getBoolean("key", false);

        if (!wallpaperSetState) {
            Log.i(APP_TAG, "Starting wallpaper service");

            final String WATCHFACE_PACKAGE = getResources()
                .getString(R.string.anadigclock_package_name);
            final String WATCHFACE_CLASS = getResources()
                .getString(R.string.anadigclock_class_name);
            final ComponentName cn =
                new ComponentName(WATCHFACE_PACKAGE, WATCHFACE_CLASS);

            try {
                mWallpaperManager.getIWallpaperManager().setWallpaperComponent(
                    cn);
                mWallpaperManager.setWallpaperOffsetSteps(0.5f, 0.0f);
                mWallpaperManager.setWallpaperOffsets(layout.getWindowToken(),
                    0.5f, 0.0f);
            } catch (Exception e) {
                e.printStackTrace();
            }

            //Once wallpaper is set, use the same for subsequent boot ups
            editor.putBoolean("key", true);
            editor.commit();
        }

        IntentFilter filter = new IntentFilter();
        filter.addAction(REQUEST_STATE_ACTION);
        filter.addAction("android.intent.action.SIM_STATE_CHANGED");
        registerReceiver(mWatchfaceBroadcastReceiver, filter);

    }

    @Override
    protected void onStart() {
        super.onStart();
        Log.i(APP_TAG, "Started");
        //Register contextual mode listener
        mContextualModeHelper.registerContextualModeListener(this,
                mContextualModeListener);
    }

    @Override
    protected void onResume() {
        super.onResume();
        final View layout = (View) findViewById(
            com.qualcomm.qti.watchhome.R.id.watch_home);
        //Set activity background to null
        layout.setBackground(null);

        //Get currently set wallpaper info
        WallpaperInfo wallpaperInfo = mWallpaperManager.getWallpaperInfo();
        if(wallpaperInfo != null) {
            String mSetWallpaper = wallpaperInfo.getServiceName().toString();

        /**
          * On device reboot, based on wallpaper set,
          * check if analog clock is to be shown or not
          */
            if (mSetWallpaper.equals(getResources()
                    .getString(R.string.anadigclock_class_name)) ||
                        mSetWallpaper.equals(getResources()
                            .getString(R.string.anaclock_class_name))) {
                mAnalogClock.setVisibility(View.VISIBLE);
            } else {
                mAnalogClock.setVisibility(View.GONE);
            }
        }
        Log.i(APP_TAG, "Resumed and background set to null");

        //Get layout width and height and use it for fling distance threshold
        final int width = layout.getWidth();
        final int height = layout.getHeight();

        final PackageManager mPackageManager =
            getApplicationContext().getPackageManager();

        //Populate list with all available live wallpaper services
        final List<ResolveInfo> mWallpaperList =
            mPackageManager.queryIntentServices(
                new Intent(WallpaperService.SERVICE_INTERFACE),
                    PackageManager.GET_META_DATA);

        //Get total number of wallpapers available
        final int mCount = mWallpaperList.size();

        //Arrays for storing live wallpaper info
        final String[] mPackageName = new String[mCount];
        final String[] mServiceName = new String[mCount];

        //Get Package and Service names of all available live wallpapers
        retrieveLiveWallpaperInfo(mWallpaperList, mCount,
            mPackageName, mServiceName);

        //Use gesture listener to launch wallpaper menu and launcher
        class OnFlingTouchListener implements View.OnTouchListener,
            OnFlingTouchDetector.FlingGestureListenerInterface {

            private final GestureDetector gestureDetector;

            //Constructor
            public OnFlingTouchListener(Context context) {
                gestureDetector = new GestureDetector(context,
                    new OnFlingTouchDetector(context,this,width,height));
            }

            public boolean onTouch(View v, MotionEvent event) {
                return gestureDetector.onTouchEvent(event);
            }

            //On left fling, set next live wallpaper as watchface
            //@Override
            public void goToWallPaperChooser() {
                //Get currently set wallpaper info
                WallpaperInfo wallpaperInfo = mWallpaperManager.getWallpaperInfo();
                if(wallpaperInfo != null) {
                    String mSetWallpaper = wallpaperInfo.getServiceName().toString();

                    int mIndex = 0;

                    //Get index of wallpaper to be set as new watchface
                    for(int i=0; i < mCount; i++) {
                        if(mServiceName[i].equals(mSetWallpaper)) {
                            if (i == (mCount - 1)) {
                                mIndex = 0;
                            } else {
                                mIndex = i+1;
                            }
                        //Set chosen wallpaper as new watchface
                        setWallpaper(layout, mIndex,
                        mPackageName[mIndex], mServiceName[mIndex]);
                        break;
                        }
                    }
                }
            }

            //On long press, go to launcher
            //@Override
            public void goToLauncher() {
                Log.i(APP_TAG, "Launching the custom launcher");
                PackageManager pm = getPackageManager();
                final String LAUNCHER_TAG = getResources().getString(
                    R.string.launcher_package_name);
                Intent launchIntent = pm.getLaunchIntentForPackage(
                    LAUNCHER_TAG);
                if (launchIntent == null) {
                    Log.e(APP_TAG,
                        "Launch intent for custom launcher not found");
                } else if(!validateSIMCard()){
                    startActivity(launchIntent);
                }
            }
        }

        layout.setOnTouchListener(
            new OnFlingTouchListener(getApplicationContext()) {
        });
    }

    //Populate package and service name of available live wallpapers
    private void retrieveLiveWallpaperInfo(List<ResolveInfo>mWallpaperList,
        int mCount, String[] mPackageName, String[] mServiceName) {
        //Populate package name and service name from wallpaper info
        for(int i=0; i < mCount; i++) {
            WallpaperInfo info = null;
            ResolveInfo resolveInfo = mWallpaperList.get(i);
            try {
                info = new WallpaperInfo(getApplicationContext(),
                    resolveInfo);
                mPackageName[i] = info.getPackageName().toString();
                mServiceName[i] = info.getServiceName().toString();
            } catch (XmlPullParserException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }


    //Set chosen wallpaper as new watchface
    private void setWallpaper(View layout, int index,
        String packageName, String serviceName) {
        //Check if analog clock is to be made visible or not
        if (serviceName.equals(getResources()
            .getString(R.string.anadigclock_class_name)) ||
                serviceName.equals(getResources()
                    .getString(R.string.anaclock_class_name))) {
            mAnalogClock.setVisibility(View.VISIBLE);
        } else {
            mAnalogClock.setVisibility(View.GONE);
        }

        Log.i(APP_TAG, "Set live wallpaper as new watchface.Package name : " +
            packageName);
        final String WATCHFACE_PACKAGE = packageName;
        final String WATCHFACE_CLASS = serviceName;
        final ComponentName cn =
            new ComponentName(WATCHFACE_PACKAGE, WATCHFACE_CLASS);
            try {
                mWallpaperManager.getIWallpaperManager().setWallpaperComponent(
                    cn);
                mWallpaperManager.setWallpaperOffsetSteps(0.5f, 0.0f);
                mWallpaperManager.setWallpaperOffsets(layout.getWindowToken(),
                    0.5f, 0.0f);
            } catch (Exception e) {
                e.printStackTrace();
            }
    }

    //Unregister broadcast receiver while stopping activity
    @Override
    protected void onStop() {
        super.onStop();
        Log.i(APP_TAG, "Stopped");
        //Unregister ContextualMode Service
        mContextualModeHelper.unregisterContextualModeService(this);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.i(APP_TAG, "onDestroy");
        unregisterReceiver(mWatchfaceBroadcastReceiver);
    }

    //Override back button for watchhome as it's home screen
    @Override
    public void onBackPressed() {
        moveTaskToBack(true);
    }
}
