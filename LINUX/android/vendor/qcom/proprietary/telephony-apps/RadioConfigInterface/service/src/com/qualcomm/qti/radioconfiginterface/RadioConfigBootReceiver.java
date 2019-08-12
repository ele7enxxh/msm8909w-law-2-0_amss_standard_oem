/******************************************************************************
 * @file    RadioConfigBootReceiver.java
 * @brief   Receive boot completed intent and start the RadioConfig
 *          service
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/


package com.qualcomm.qti.radioconfiginterface;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.os.SystemProperties;

public class RadioConfigBootReceiver extends BroadcastReceiver {
    private static final String LOG_TAG = "RadioConfigBootReceiver";

    @Override
    public void onReceive(Context context, Intent intent) {
        Log.d(LOG_TAG, "onReceive");
        boolean startRadioConfigOnBoot = SystemProperties.getBoolean("persist.lted.enabled",
                false);

        if (startRadioConfigOnBoot) {
            Log.i(LOG_TAG, "starting RadioConfigService on boot");
            context.startService(new Intent(context, RadioConfigService.class));
        } else {
            Log.i(LOG_TAG, "RadioConfig service is not started on boot");
        }
    }
}
