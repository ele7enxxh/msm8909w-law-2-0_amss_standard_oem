/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchdialer;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class PhoneEventsHandler extends BroadcastReceiver {

    public static final String APP_TAG = "WatchDialer";

    @Override
    public void onReceive(Context context, Intent intent) {

        Log.v(APP_TAG, "Received Phone Event:");

    }
}
