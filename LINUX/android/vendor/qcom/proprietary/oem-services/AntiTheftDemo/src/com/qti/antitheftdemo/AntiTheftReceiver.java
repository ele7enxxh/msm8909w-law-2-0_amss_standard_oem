/******************************************************************************
 * @file    AntiTheftReceiver.java
 * ---------------------------------------------------------------------------
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *  ---------------------------------------------------------------------------
 ******************************************************************************/
package com.qti.antitheftdemo;

import android.Manifest;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.telephony.ServiceState;
import android.telephony.SmsMessage;
import android.util.Log;

import com.android.internal.telephony.TelephonyIntents;

public class AntiTheftReceiver extends BroadcastReceiver {

    private static final String[] RUNTIME_PERMISSIONS = { Manifest.permission.RECEIVE_SMS, Manifest.permission.SEND_SMS,
            Manifest.permission.READ_PHONE_STATE };
    @Override
    public void onReceive(Context context, Intent intent) {

        for (String runtimePermission : RUNTIME_PERMISSIONS) {
            if (context.checkSelfPermission(runtimePermission) != PackageManager.PERMISSION_GRANTED) {
                return;
            }
        }

        if ("android.provider.Telephony.SMS_RECEIVED".equals(intent.getAction())) {

            Bundle bundle = intent.getExtras();
            SmsMessage[] msgs = null;
            String messageReceived = "";
            if (bundle != null) {
                Object[] pdus = (Object[]) bundle.get("pdus");
                msgs = new SmsMessage[pdus.length];
                for (int i = 0; i < msgs.length; i++) {
                    msgs[i] = SmsMessage.createFromPdu((byte[]) pdus[i]);
                    messageReceived += msgs[i].getMessageBody().toString();
                    messageReceived += "\n";
                }

                String senderPhoneNumber = msgs[0].getOriginatingAddress();
                messageReceived = messageReceived.trim();
                CommandHandler.checkMessage(context, senderPhoneNumber, messageReceived);
            }
        } else if (TelephonyIntents.ACTION_SERVICE_STATE_CHANGED.equals(intent.getAction())) {
            ServiceState state = ServiceState.newFromBundle(intent.getExtras());
            if (Debug.DEBUG)
                Log.d(Debug.TAG, "Service state changed: " + state.getState());
            if (state.getState() == ServiceState.STATE_IN_SERVICE) {
                CommandHandler.checkSimChange(context);
            }
        }
    }
}
