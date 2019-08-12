/******************************************************************************
 * @file    CallBackBroadCastReceiver.java
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/
package com.samautotester.controller;

import java.util.concurrent.atomic.AtomicReference;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.widget.Toast;

public class CallBackBroadCastReceiver extends BroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
        Log.d("CallBackBroadCastReceiver",
                "Received intent ... " + intent.getAction());
        String result = intent.getExtras().getString("message");

        if (result.contains("streamApduResponse")) {
            TestFunc.INDIVIDUAL
                    .set_StreamApdu_callbackResult(new AtomicReference<String>(
                            result));
        } else if (result.contains("switchSlotResponse")) {
            TestFunc.INDIVIDUAL
                    .set_SwitchSlot_callbackResult(new AtomicReference<String>(
                            result));
        } else if (result.contains("cardPowerUpResponse")) {
            TestFunc.INDIVIDUAL
                    .set_CardPowerUp_callbackResult(new AtomicReference<String>(
                            result));
        } else if (result.contains("cardPowerDownResponse")) {
            TestFunc.INDIVIDUAL
                    .set_CardPowerDown_callbackResult(new AtomicReference<String>(
                            result));
        }

        Toast.makeText(context, result, Toast.LENGTH_SHORT).show();
    }
}
