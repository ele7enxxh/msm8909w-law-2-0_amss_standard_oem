/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qti.primarycardcontroller.cttuneaway;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Handler;
import android.os.Message;
import android.content.Intent;
import android.content.IntentFilter;
import android.preference.PreferenceManager;
import android.telephony.Rlog;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.RadioCapability;
import com.android.internal.telephony.TelephonyIntents;
import com.qti.primarycardcontroller.R;

import java.util.List;

public class CtPrimaryStackMappingController extends Handler {
    private final String TAG = CtPrimaryStackMappingController.class.getSimpleName();
    private final String ACTION_RADIO_CAPABILITY_UPDATED =
            "org.codeaurora.intent.action.ACTION_RADIO_CAPABILITY_UPDATED";
    private static final String SP_SUB = "spsub";
    private static final int EVT_REF_REQUEST_DONE = 1;
    private static final int EVT_SUB_CHANGED = 2;
    private static final int sPhoneCount = TelephonyManager.getDefault().getPhoneCount();
    private Context mContext;
    private RadioCapability[] mRadioCapability;
    private Phone[] mPhones;
    private SubscriptionManager mSubscriptionManager;
    private CtTuneAwayPrefRequest mCtTuneAwayPrefRequest;
    private SubscriptionInfo[] mSubInfo;
    private String[] mCtMccMncs;
    private boolean mBothCtCardPresent = false;
    private boolean mBootup = true;
    private boolean mHandling = false;

    public CtPrimaryStackMappingController(Context context) {
        mContext = context;
        mRadioCapability = new RadioCapability[sPhoneCount];
        mPhones = PhoneFactory.getPhones();
        mSubInfo = new SubscriptionInfo[sPhoneCount];
        mCtMccMncs = context.getResources().getStringArray(R.array.ct_mcc_mnc_list);
        mSubscriptionManager = SubscriptionManager.from(context);
        mSubscriptionManager.addOnSubscriptionsChangedListener(mSubChangedListener);
        IntentFilter intentFilter = new IntentFilter(ACTION_RADIO_CAPABILITY_UPDATED);
        intentFilter.addAction(TelephonyIntents.ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED);
        mContext.registerReceiver(mReceiver, intentFilter);
    }

    private final SubscriptionManager.OnSubscriptionsChangedListener mSubChangedListener =
            new SubscriptionManager.OnSubscriptionsChangedListener() {
                @Override
                public void onSubscriptionsChanged() {
                    obtainMessage(EVT_SUB_CHANGED).sendToTarget();
                }
            };

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action.equals(ACTION_RADIO_CAPABILITY_UPDATED)) {
                for (int i = 0; i < sPhoneCount; i++) {
                    mRadioCapability[i] = mPhones[i].getRadioCapability();
                }
                Rlog.d(TAG, "ACTION_RADIO_CAPABILITY_UPDATED");
                triggerPrimaryStackMapToDdsSubIfRequired();
            } else if (action.equals(TelephonyIntents.ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED)) {
                Rlog.d(TAG, "ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED");
                triggerPrimaryStackMapToDdsSubIfRequired();
            }
        }
    };

    private void triggerPrimaryStackMapToDdsSubIfRequired() {
        if (mBothCtCardPresent) {
            boolean needed = false;
            int sub = mSubscriptionManager.getDefaultDataSubId();
            int slotId = mSubscriptionManager.getSlotId(sub);
            Rlog.d(TAG, "slotId = " + slotId + "dds sub = " + sub);
            if (mRadioCapability[slotId] == null) {
                mRadioCapability[slotId] = mPhones[slotId].getRadioCapability();
            }
            if (mRadioCapability[slotId] != null) {
                String uuid = mRadioCapability[slotId].getLogicalModemUuid();
                Rlog.d(TAG, "uuid =" + uuid);
                if (Integer.parseInt(uuid) != 0) {
                    needed = true;
                }
            }
            Rlog.d(TAG, "needed =" + needed);
            if (needed) {
                if (!mHandling) {
                    new CtTuneAwayPrefRequest(mContext,
                        slotId, obtainMessage(EVT_REF_REQUEST_DONE)).loop();
                    mHandling = true;
                }
            }
        }
    }

    @Override
    public void handleMessage(Message msg) {
        switch (msg.what) {
            case EVT_SUB_CHANGED:
                List<SubscriptionInfo> subInfoList = mSubscriptionManager.
                        getActiveSubscriptionInfoList();
                if (subInfoList != null) {
                    updateSimInfo(subInfoList);
                }
                break;
            case EVT_REF_REQUEST_DONE:
                mHandling = false;
                Rlog.d(TAG, "EVT_REF_REQUEST_DONE");
                // After request, must be check again due to
                // user frequently switch DDS
                triggerPrimaryStackMapToDdsSubIfRequired();
                break;
        }
    }

    private void updateSimInfo(List<SubscriptionInfo> subInfoList) {
        SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(mContext);
        boolean changed = false;

        for (SubscriptionInfo subInfo : subInfoList) {
            if (SubscriptionManager.isValidSlotId(subInfo.getSimSlotIndex())) {
                mSubInfo[subInfo.getSimSlotIndex()] = subInfo;
            }
        }

        for (int i = 0; i < mSubInfo.length; i++) {
            if (mSubInfo[i] != null) {
                int sub = sp.getInt(SP_SUB + mSubInfo[i].getSimSlotIndex(),
                        SubscriptionManager.INVALID_SUBSCRIPTION_ID);
                if (sub != mSubInfo[i].getSubscriptionId()) {
                    changed = true;
                    sp.edit().putInt(SP_SUB + mSubInfo[i].getSimSlotIndex(),
                            mSubInfo[i].getSubscriptionId()).apply();
                }
            }
        }

        Rlog.d(TAG, "updateSimInfo changed = " + changed);
        if (mBootup || changed) {
            mBootup = false;
            if (areAllCtCard()) {
                mBothCtCardPresent = true;
                triggerPrimaryStackMapToDdsSubIfRequired();
            } else {
                mBothCtCardPresent = false;
            }
        }
    }

    private boolean isCtCard(int phoneId) {
        SubscriptionInfo subInfo = mSubInfo[phoneId];
        if (subInfo != null && mCtMccMncs != null) {
            String mccMnc = String.format("%03d%02d", subInfo.getMcc(), subInfo.getMnc());
            Rlog.d(TAG, "slot: " + phoneId + " mccMnc: " + mccMnc);
            for (String ctMccMnc : mCtMccMncs) {
                if (mccMnc.equals(ctMccMnc)) {
                    return true;
                }
            }
        }
        return false;
    }

    private boolean areAllCtCard() {
        boolean rlt = (sPhoneCount == 2);
        for (int i = 0; i < mSubInfo.length; i++) {
            rlt &= isCtCard(i);
        }
        return rlt;
    }
}
