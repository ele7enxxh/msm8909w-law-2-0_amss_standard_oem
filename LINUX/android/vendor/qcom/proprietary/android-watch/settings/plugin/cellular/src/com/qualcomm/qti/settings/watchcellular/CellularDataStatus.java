/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchcellular;

import android.app.Activity;
import android.content.Context;
import android.telephony.TelephonyManager;
import android.telephony.SubscriptionManager;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

public class CellularDataStatus {
    private static final String TAG = "CellularSettings";
    private boolean mCellularDataEnabled = false;
    private boolean mCellularDataRoamingEnabled = false;
    private TelephonyManager mTelephonyManager = null;
    private SubscriptionManager mSubscriptionManager = null;
    private Context mContext = null;
    private Phone mPhone = null;

    public CellularDataStatus(Context context, Phone phone) {
        this.mContext = context;
        mTelephonyManager = TelephonyManager.from(context);
        mSubscriptionManager = SubscriptionManager.from(context);
        mPhone = phone;
    }

    public boolean isCellularDataEnabled() {
        if(mSubscriptionManager != null && mTelephonyManager != null) {
            int dataSubId = mSubscriptionManager.getDefaultDataSubscriptionId();
            mCellularDataEnabled = mTelephonyManager.getDataEnabled(dataSubId);
        } else {
            Log.e(TAG, "Failed to get if Cellular Data is On/OFF");
        }
        Log.d(TAG, "Cellular Data "+ mCellularDataEnabled);
        return mCellularDataEnabled;
    }

    public boolean isCellularDataRoamingEnabled() {
        if(mPhone != null) {
            mCellularDataRoamingEnabled = mPhone.getDataRoamingEnabled();
        } else {
            Log.e(TAG, "Failed to get Cellular Data roaming is enabled");
        }
        Log.d(TAG, "Cellular Data Roaming "+ mCellularDataRoamingEnabled);
        return mCellularDataRoamingEnabled;
    }

    public int setCellulluladData(boolean enabled){
        if(mSubscriptionManager != null && mTelephonyManager != null) {
            int dataSubId = mSubscriptionManager.getDefaultDataSubscriptionId();
            Log.d(TAG, "Set Cellular Data "+ enabled);
            mTelephonyManager.setDataEnabled(dataSubId, enabled);
            mCellularDataEnabled = enabled;
        } else {
            Log.e(TAG, "Failed to set Cellular Data" + enabled);
        }
        return 0;
    }

    public int setCellulluladDataRoaming(boolean enabled){
        Log.d(TAG, "Set Cellular Data Roam "+ enabled);
        if(mPhone != null) {
            mPhone.setDataRoamingEnabled(enabled);
            mCellularDataRoamingEnabled = enabled;
        } else {
            Log.e(TAG, "Failed to set Cellular Data roaming " + enabled);
        }
        return 0;
    }

    public void setCellularDataStatusText(TextView view, boolean bStatus) {
        if(view == null) {
            return;
        }

        if(bStatus) {
            view.setText(mContext.getString(R.string.enabled));
        } else {
            view.setText(mContext.getString(R.string.disabled));
        }
    }
}
