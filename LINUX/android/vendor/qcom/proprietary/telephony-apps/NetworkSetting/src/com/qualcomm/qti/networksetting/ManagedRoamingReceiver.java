/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.networksetting;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

import android.telephony.SubscriptionManager;
import com.android.internal.telephony.PhoneConstants;


public class ManagedRoamingReceiver extends BroadcastReceiver {

    private static final String TAG = "ManagedRoamingReceiver";
    private static final String ACTION_MANAGED_ROAMING_IND =
            "codeaurora.intent.action.ACTION_MANAGED_ROAMING_IND";
    @Override
    public void onReceive(Context context, Intent intent) {

        String action = intent.getAction();
        if (action.equals(ACTION_MANAGED_ROAMING_IND)) {
                int subscription = intent.getIntExtra(PhoneConstants.SUBSCRIPTION_KEY,
                        SubscriptionManager.getDefaultSubscriptionId());
                Intent createIntent = new Intent();
                createIntent.setClass(context, ManagedRoaming.class);
                createIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                createIntent.putExtra(PhoneConstants.SUBSCRIPTION_KEY, subscription);
                context.startActivity(createIntent);
            }
    }
}
