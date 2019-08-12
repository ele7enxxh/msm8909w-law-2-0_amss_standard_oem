/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchhome.sensor;

import android.content.Context;
import android.hardware.display.DisplayManager;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.os.PowerManager;
import android.os.SystemClock;
import android.util.Log;
import android.view.Display;

public class SensorListener implements SensorEventListener {
    private static final String APP_TAG = "WatchHome";
    private Context context;
    private DisplayManager displayManager;
    private PowerManager powerManager;
    private PowerManager.WakeLock wakeLock;
    private long WAKE_LOCK_TIME_MS = 10;

    public SensorListener(Context context) {
        this.context = context;
        this.displayManager = (DisplayManager) context.getSystemService(
            context.DISPLAY_SERVICE);
        this.powerManager = (PowerManager) context.getSystemService(
            context.POWER_SERVICE);
        wakeLock = powerManager.newWakeLock(
            PowerManager.SCREEN_BRIGHT_WAKE_LOCK |
            PowerManager.ACQUIRE_CAUSES_WAKEUP, APP_TAG);
    }

    @Override
    public void onSensorChanged(SensorEvent event) {

        if(event.sensor.getType() == Sensor.TYPE_WRIST_TILT_GESTURE){
            Log.d(APP_TAG, "onSensorChanged. Received wake on tilt event");
            for (Display display : displayManager.getDisplays()) {
                int displayState = display.getState();
                Log.d(APP_TAG, "displayState: " + displayState);

                if (displayState != Display.STATE_ON) {
                    wakeLock.acquire(WAKE_LOCK_TIME_MS);
                }
            }
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        Log.d(APP_TAG, "onAccuracyChanged. accuracy: " + accuracy);
    }
}

