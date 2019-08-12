/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchhome.sensor;

import android.content.Context;
import android.hardware.display.DisplayManager;
import android.hardware.Sensor;
import android.util.Log;

public class SensorManager {
    private static final String APP_TAG = "WatchHome";
    private Context context;
    private android.hardware.SensorManager sensorManager;

    public SensorManager(Context context) {
        this.context = context;
    }

    public Sensor initializeSensor(int type) {

        sensorManager = (android.hardware.SensorManager)context.getSystemService(Context.SENSOR_SERVICE);

        if (sensorManager.getDefaultSensor(type)
            != null) {

            Log.d(APP_TAG, "Wrist tilt gesture sensor available");

            return sensorManager.getDefaultSensor(type);

        } else {
            Log.d(APP_TAG, "Wrist tilt gesture sensor not available");
            return null;
        }
    }

    public void registerListener(Sensor sensor,SensorListener sensorListener){

        if (sensor != null) {
            sensorManager.registerListener(sensorListener, sensor,
                android.hardware.SensorManager.SENSOR_DELAY_NORMAL);
            Log.d(APP_TAG, "Registering wake on sensor listener");
        }

    }

    public void removeListener(Sensor sensor,SensorListener sensorListener){

        if (sensor != null) {
            sensorManager.unregisterListener(sensorListener, sensor);
            Log.d(APP_TAG, "Un-registering wake on sensor listener");
        }

    }

}

