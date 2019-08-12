/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchalarm;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class AlarmNotificationDismissedReceiver extends BroadcastReceiver{

    @Override
    public void onReceive(Context context, Intent intent) {
        int notificationId = intent.getExtras().getInt("com.qualcomm.qti.watchalarm.notificationId");
        Log.d("watchAlarmDismiss","Received dismiss for alarm " + notificationId);

        //Send ACTION STOP to stop the service and dismiss the alarm
        Intent serviceIntent = new Intent(context, AlarmService.class);
        serviceIntent.setAction(AlarmService.ACTION_STOP);
        context.startService(serviceIntent);
    }
}
