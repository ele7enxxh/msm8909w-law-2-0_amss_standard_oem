/*
 *                     Location Alarm Receiver
 *
 * GENERAL DESCRIPTION
 *   This file is the receiver for the Alarm set by LocationAlarmService
 *   Sets the wakelock so that CPU is left on for 10 ms for other
 *   processes to run.
 *
 * Copyright (c) 2012-2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.location.localarm;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class LocationAlarmReceiver extends BroadcastReceiver {
    static public String ACTION = "com.qualcomm.location.localarm.ALARM";

    @Override
    public void onReceive(Context ctxt, Intent intent) {
        long rtcTime = System.currentTimeMillis();
        ((LocationAlarmService) ctxt).setNextAlarm();
        String message = "Alarm worked at " + String.valueOf(rtcTime) + ".";
        Log.d(LocationAlarmService.LOG_TAG, message);
    }
}
