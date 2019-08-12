/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.watchface;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.graphics.Color;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import java.text.SimpleDateFormat;
import java.util.Calendar;

import com.qualcomm.qti.contextualmodelib.ContextualModeHelper;
import com.qualcomm.qti.contextualmodelib.ContextualModeListener;

public class WatchFaceActivity extends Activity {

    private static final String APP_TAG = "WatchFaceActivity";
    private static final String LAUNCHER_TAG = "com.qualcomm.qti.launcher";
    private static final String TIME_FORMAT_12_HOUR = "hh:mm a";
    private TextView mClockTime;

    private ContextualModeHelper mContextualModeHelper =
        new ContextualModeHelper();

    private ContextualModeListener mContextualModeListener =
        new ContextualModeListener() {
            @Override
            public void contextualModeEntered() {
                setClockColor(mClockTime, getResources()
                    .getColor(R.color.white));
                Log.i(APP_TAG, "Entering contextual mode");
            }

            @Override
            public void contextualModeExited() {
                setClockColor(mClockTime, getResources()
                    .getColor(R.color.green));
                Log.i(APP_TAG, "Exiting contextual mode");
            }

            @Override
            public void contextualModeUpdate() {
                Log.i(APP_TAG,
                    "Placeholder to update clock in contextual mode");
            }
        };

    //Change digital clock color to white in contextual mode
    private void setClockColor(final TextView clock, final int color){
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                clock.setTextColor(color);
            }
        });
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d(APP_TAG, "Created and clock updated");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_watchface);

        //Get clock time and update it as soon as watch face launches
        mClockTime = (TextView) findViewById(R.id.watch_time);
        setClockColor(mClockTime, getResources().getColor(R.color.green));
        mClockTime.setText(new SimpleDateFormat(TIME_FORMAT_12_HOUR)
            .format(Calendar.getInstance().getTime()));

        //Register contextual mode listener
        mContextualModeHelper.registerContextualModeListener(this,
            mContextualModeListener);

        //Use long press/click to launch custom launcher from watchface
        View v = (View) findViewById(R.id.watch_face);
        v.setLongClickable(true);
        v.setOnLongClickListener(new View.OnLongClickListener() {
            @Override
            public boolean onLongClick(View v) {
                getLaunchIntentForCustomLauncher();
                return true;
            }
        });
    }

    @Override
    public void onDestroy() {
        super.onDestroy();

        mContextualModeHelper.unregisterContextualModeService(this);
    }

    @Override
    protected void onStart() {
        //Get clock time and update it as soon as watch face launches
        mClockTime.setText(new SimpleDateFormat(TIME_FORMAT_12_HOUR)
            .format(Calendar.getInstance().getTime()));

        super.onStart();

        Log.i(APP_TAG, "Started. Registering broadcast receiver");
        //use action time tick intent for updating clock every minute
        IntentFilter mClockTimeTick = new IntentFilter(Intent.ACTION_TIME_TICK);
        //register broadcast receiver to update clock time every minute
        registerReceiver(mClockTimeReceiver, mClockTimeTick);
    }

    //Unregister broadcast receiver while stopping activity
    @Override
    protected void onStop() {
        Log.i(APP_TAG, "Stopped. Unregistering broadcast receiver");
        super.onStop();
        unregisterReceiver(mClockTimeReceiver);
        /**
         * Clear clock time displayed so that stale time doesn't
         * show up in onStart
         */
        mClockTime.setText("");
    }

    //On long click, launch intent for custom launcher and launch it
    public void getLaunchIntentForCustomLauncher() {
        PackageManager pm = getPackageManager();
        Intent launchIntent = pm.getLaunchIntentForPackage(LAUNCHER_TAG);
        if (launchIntent == null) {
            Log.e(APP_TAG, "Launch intent for custom launcher not found");
        } else {
            startActivity(launchIntent);
        }
    }

    //Broadcast receiver to update clock time every minute
    private BroadcastReceiver mClockTimeReceiver = new BroadcastReceiver(){
        @Override
        public void onReceive(Context c, Intent i) {
            Log.i(APP_TAG, "Broadcast receiver. Received time tick ");
            mClockTime.setText(new SimpleDateFormat(TIME_FORMAT_12_HOUR)
                .format(Calendar.getInstance().getTime()));
        }
    };

    //Override back button for watchface as it's home screen
    @Override
    public void onBackPressed() {
        moveTaskToBack(true);
    }
}
