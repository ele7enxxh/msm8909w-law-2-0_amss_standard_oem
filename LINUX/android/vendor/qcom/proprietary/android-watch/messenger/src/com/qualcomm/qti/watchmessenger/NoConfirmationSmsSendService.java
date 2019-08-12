package com.qualcomm.qti.watchmessenger;

/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */

/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import android.app.IntentService;
import android.app.PendingIntent;
import android.content.ContentUris;
import android.content.Context;
import android.content.Intent;
import android.content.ReceiverCallNotAllowedException;
import android.net.Uri;
import android.os.Bundle;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;
import android.telephony.SmsManager;
import android.app.RemoteInput;

/**
 * Respond to a special intent and send an SMS message without the user's intervention, unless
 * the intent extra "showUI" is true.
 */
public class NoConfirmationSmsSendService extends IntentService {
    private static final String TAG = "watch_sms_intent_service";

    private PendingIntent msgSentPendingIntent;
    private PendingIntent msgDeliveredPendingIntent;
    private SmsManager smsManager;

    public NoConfirmationSmsSendService() {
        // Class name will be the thread name.
        super(NoConfirmationSmsSendService.class.getName());

        smsManager = SmsManager.getDefault();
        // Intent should be redelivered if the process gets killed before completing the job.
        setIntentRedelivery(true);
    }

    @Override
    protected void onHandleIntent(final Intent intent) {
        Log.v(TAG, "NoConfirmationSmsSendService onHandleIntent");

        final String action = intent.getAction();
        if (!TelephonyManager.ACTION_RESPOND_VIA_MESSAGE.equals(action)) {
            Log.v(TAG, "NoConfirmationSmsSendService onHandleIntent wrong action: " +
                action);
            return;
        }
        final Bundle extras = intent.getExtras();
        if (extras == null) {
            Log.v(TAG, "Called to send SMS but no extras");
            return;
        }

        // Get all possible extras from intent
        final String msgText = getText(intent, Intent.EXTRA_TEXT);
        final String msgSubject = getText(intent, Intent.EXTRA_SUBJECT);

        final Uri intentUri = intent.getData();
        final String msgReceipents = intentUri != null ? Utils.getSmsRecipients(intentUri) : null;

        if (TextUtils.isEmpty(msgReceipents)) {
            Log.v(TAG, "Recipient(s) cannot be empty");
            return;
        }

        if (extras.getBoolean("showUI", false)) {
            startActivity(new Intent(this, SmsComposer.class));
        } else {
            if (TextUtils.isEmpty(msgText)) {
                Log.v(TAG, "Message Text cannot be empty");
                return;
            }
            Context context = getApplicationContext();

            Uri rowIdUri = Utils.updateProvider(context, msgReceipents, msgText);
            long rowIndex = Long.valueOf(ContentUris.parseId(rowIdUri));

            Intent msgSentIntent = new Intent(SmsBroadcastEventHandler.MESSAGE_SENT_ACTION);
            msgSentIntent.putExtra("rowIndex", rowIndex);
            msgSentPendingIntent = PendingIntent.getBroadcast(context, 0, msgSentIntent, PendingIntent.FLAG_UPDATE_CURRENT);

            Intent msgDeliveredIntent = new Intent(SmsBroadcastEventHandler.MESSAGE_DELIVERED_ACTION);
            msgDeliveredIntent.putExtra("rowIndex", rowIndex);
            msgDeliveredPendingIntent = PendingIntent.getBroadcast(context, 0, msgDeliveredIntent, PendingIntent.FLAG_UPDATE_CURRENT);

            smsManager.sendTextMessage(msgReceipents, null, msgText,
            msgSentPendingIntent, msgDeliveredPendingIntent);
        }
    }

    private String getText(final Intent intent, final String textType) {
        final String message = intent.getStringExtra(textType);
        if (message == null) {
            final Bundle remoteInput = RemoteInput.getResultsFromIntent(intent);
            if (remoteInput != null) {
                final CharSequence extra = remoteInput.getCharSequence(textType);
                if (extra != null) {
                    return extra.toString();
                }
            }
        }
        return message;
    }
}