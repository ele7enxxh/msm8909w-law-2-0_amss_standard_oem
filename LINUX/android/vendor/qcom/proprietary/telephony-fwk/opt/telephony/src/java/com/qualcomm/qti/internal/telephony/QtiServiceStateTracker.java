/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony;

import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Message;
import android.content.Context;
import android.net.ZeroBalanceHelper;
import android.telephony.ServiceState;

import android.util.Log;
import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.GsmCdmaPhone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.ServiceStateTracker;


public class QtiServiceStateTracker extends ServiceStateTracker {
    private static final String LOG_TAG = "QtiServiceStateTracker";
    private static final boolean DBG = true;
    private static final boolean VDBG = false;  // STOPSHIP if true
    private static final String ACTION_MANAGED_ROAMING_IND =
            "codeaurora.intent.action.ACTION_MANAGED_ROAMING_IND";
    private final String ACTION_RAC_CHANGED = "qualcomm.intent.action.ACTION_RAC_CHANGED";
    private final String mRatInfo = "rat";
    private final String mRacChange = "rac";
    private int mRac;
    private int mRat;
    private int mTac = -1;

    private BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getAction().equals(ACTION_RAC_CHANGED)) {
                Bundle bundle = intent.getExtras();
                if (bundle != null) {
                    mRac = bundle.getInt(mRacChange);
                    mRat = bundle.getInt(mRatInfo);
                    enableBackgroundData();
                }
            }
        }
    };

    public QtiServiceStateTracker(GsmCdmaPhone phone, CommandsInterface ci) {
        super(phone,ci);
        IntentFilter filter = new IntentFilter();
        filter.addAction(ACTION_RAC_CHANGED);
        phone.getContext().registerReceiver(mIntentReceiver, filter);
    }

    private void enableBackgroundData() {
        ZeroBalanceHelper helper = new ZeroBalanceHelper();
        if (helper.getFeatureConfigValue() &&
                helper.getBgDataProperty().equals("true")) {
            Log.i("zerobalance","Enabling the background data on LAU/RAU");
            helper.setBgDataProperty("false");
        }
    }

    @Override
    protected void handlePollStateResultMessage(int what, AsyncResult ar) {
        switch (what) {
            case EVENT_POLL_STATE_REGISTRATION: {
                super.handlePollStateResultMessage(what, ar);
                String states[];
                if (mPhone.isPhoneTypeGsm()) {
                    states = (String[]) ar.result;
                    int regState = ServiceState.RIL_REG_STATE_UNKNOWN;
                    if (states.length > 0) {
                        try {
                            regState = Integer.parseInt(states[0]);
                        } catch (NumberFormatException ex) {
                            loge("error parsing RegistrationState: " + ex);
                        }
                    }

                    if ((regState == ServiceState.RIL_REG_STATE_DENIED
                            || regState == ServiceState.RIL_REG_STATE_DENIED_EMERGENCY_CALL_ENABLED)
                            && (states.length >= 14)) {
                        try {
                            int rejCode = Integer.parseInt(states[13]);
                            // Check if rejCode is "Persistent location update reject",
                            if (rejCode == 10) {
                                log(" Posting Managed roaming intent sub = "
                                        + mPhone.getSubId());
                                Intent intent =
                                        new Intent(ACTION_MANAGED_ROAMING_IND);
                                intent.putExtra(PhoneConstants.SUBSCRIPTION_KEY,
                                        mPhone.getSubId());
                                mPhone.getContext().sendBroadcast(intent);
                            }
                        } catch (NumberFormatException ex) {
                            loge("error parsing regCode: " + ex);
                        }
                    }
                }
                break;
            }

            default:
                super.handlePollStateResultMessage(what, ar);
        }
    }
}
