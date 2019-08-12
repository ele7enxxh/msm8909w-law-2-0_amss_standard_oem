/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.biometrics.voiceprint.service;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

/**
 * The Class IntentBroadcastReceiver. This class handles intents received from
 * the system that this application is interested in.
 */
public class IntentBroadcastReceiver extends BroadcastReceiver {

/**
 * The Constant Logging TAG.
 */
private final static String TAG = "VoicePrintService";

/*
 * (non-Javadoc)
 *
 * @see android.content.BroadcastReceiver#onReceive(android.content.Context,
 * android.content.Intent)
 */
@Override
public void onReceive(Context context, Intent intent) {

Log.d(TAG, "IntentBroadcastReceiver::onReceive " + intent.getAction());

// Only handling bootup intent at this time.
if (VoicePrintServiceStub.isVoicePrintEnabled()) {
Log.e(TAG,
"VoicePrint is enabled on this device. Starting service.");
//context.startService(new Intent(context,
//		VoicePrintServiceStub.class));
} else {
Log.e(TAG,
"VoicePrint is not enabled on this device. Service not started.");
}
}

}
