/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchhome.sensor;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import android.hardware.Sensor;

public class SensorService extends Service{

    private static final String APP_TAG = "WatchHome";
    private SensorManager sensorManager;
    private SensorListener sensorListener;
    private Sensor tiltSensor;

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        // started on boot from a BroadcastReceiver
        Log.d(APP_TAG, " SensorService onStartCommand ");
        sensorManager = new SensorManager(this.getApplicationContext());
        tiltSensor = sensorManager.initializeSensor(Sensor.TYPE_WRIST_TILT_GESTURE);
        sensorListener = new SensorListener(this);
        if(sensorManager != null)
            sensorManager.registerListener(tiltSensor, sensorListener);
        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        if(sensorManager != null)
            sensorManager.removeListener(tiltSensor, sensorListener);
        super.onDestroy();
    }

}

