/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qti.primarycardcontroller;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.telephony.Rlog;
import android.telephony.TelephonyManager;
import android.os.SystemProperties;
import android.os.UserHandle;

import com.qti.primarycardcontroller.cttuneaway.CtPrimaryStackMappingController;

public class PrimaryCardReceiver extends BroadcastReceiver {
    private static final String LOG_TAG = "PrimaryCardReceiver";
    private static final String CARRIER_MODE_CT_CLASS_C = "ct_class_c";
    private final String mCarrierMode = SystemProperties.get("persist.radio.carrier_mode", "default");
    private final boolean mIsCtClassC = mCarrierMode.equals(CARRIER_MODE_CT_CLASS_C);
    private CtPrimaryStackMappingController mCtPrimaryStackMappingController;

    @Override
    public void onReceive(Context context, Intent intent) {

        if ((UserHandle.myUserId() == 0) &&
            (TelephonyManager.getDefault().getPhoneCount() > 1) &&
            Intent.ACTION_BOOT_COMPLETED.equals(intent.getAction())) {
            Rlog.d(LOG_TAG, " invoking PrimarySubSelectionController init");
            PrimarySubSelectionController.init(context);
            if (mIsCtClassC) {
                Rlog.d(LOG_TAG, " Runing up CtPrimaryStackMappingController");
                mCtPrimaryStackMappingController = new CtPrimaryStackMappingController(
                        context.getApplicationContext());
            }
        }
    }
}
