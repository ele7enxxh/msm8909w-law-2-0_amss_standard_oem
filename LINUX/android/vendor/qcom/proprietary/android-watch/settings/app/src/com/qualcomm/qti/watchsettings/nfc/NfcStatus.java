/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchsettings.nfc;

import android.app.Activity;
import android.content.Context;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.nfc.NfcAdapter;
import com.qualcomm.qti.watchsettings.R;

public class NfcStatus {
    private static final String TAG = "NFC";
    private boolean mNfcEnabled = false;
    private boolean mAndroidBeamEnabled = false;
    private Context mContext = null;
    private NfcAdapter mNfcAdapter;

    public NfcStatus(Context context) {
        this.mContext = context;
        mNfcAdapter = NfcAdapter.getDefaultAdapter(context);

    }

    public boolean isNfcEnabled() {
        if(mNfcAdapter != null) {
            mNfcEnabled = mNfcAdapter.isEnabled();
        } else {
            Log.e(TAG, "Failed to get NFC ");
        }
        Log.d(TAG, "Nfc Enabled "+ mNfcEnabled);
        return mNfcEnabled;
    }

    public boolean isAndroidBeamEnabled() {
        if(mNfcAdapter != null) {
            mAndroidBeamEnabled = mNfcAdapter.isNdefPushEnabled();
        } else {
            Log.e(TAG, "Failed to get AndroidBeam ");
        }
        Log.d(TAG, "AndroidBeam Enabled "+ mAndroidBeamEnabled);
        return mAndroidBeamEnabled;
    }

    public void setNfcStatus(boolean enabled){
        if(mNfcAdapter != null) {
            boolean success = false;
            if(enabled)
                success = mNfcAdapter.enable();
            else
                success = mNfcAdapter.disable();
            Log.d(TAG, "Set Nfc "+ success);
            mNfcEnabled = success;
        } else {
            Log.e(TAG, "Failed to set Nfc" + enabled);
        }
    }

    public void setAndroidBeamStatus(boolean enabled){
        if(mNfcAdapter != null) {
            boolean success = false;
            if(enabled)
                success = mNfcAdapter.enableNdefPush();
            else
                success = mNfcAdapter.disableNdefPush();
            Log.d(TAG, "Set AndroidBeam "+ success);
            mNfcEnabled = enabled;
        } else {
            Log.e(TAG, "Failed to set AndroidBeam" + enabled);
        }
    }

    public void setNfcStatusText(TextView view, boolean bStatus) {
        if(view == null) {
            return;
        }

        if(bStatus) {
            view.setText(mContext.getString(R.string.enable));
        } else {
            view.setText(mContext.getString(R.string.disable));
        }
    }

    public void setAndroidBeamStatusText(TextView view, boolean bStatus) {
        if(view == null) {
            return;
        }

        if(!isNfcEnabled()){
            view.setText(mContext.getString(R.string.unavailable));
            return;
        }

        if(bStatus) {
            view.setText(mContext.getString(R.string.enable));
        } else {
            view.setText(mContext.getString(R.string.disable));
        }
    }
}
