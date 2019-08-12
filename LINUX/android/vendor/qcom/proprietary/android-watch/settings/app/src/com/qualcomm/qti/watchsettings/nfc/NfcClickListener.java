package com.qualcomm.qti.watchsettings.nfc;

/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

import android.content.Context;
import android.content.Intent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.TextView;
import android.widget.AdapterView.OnItemClickListener;
import com.qualcomm.qti.watchsettings.R;

public class NfcClickListener  implements
AdapterView.OnItemClickListener {
    private final Context context;
    private final String[] nfcOptions;
    private NfcStatus nfcStatus;
    private boolean bBlocked = false;
    private ArrayAdapter adapter = null;

    public NfcClickListener(Context context,
        String[] nfcOptions, NfcStatus nfcStatus,
        ArrayAdapter adapter) {
            this.context = context;
            this.nfcOptions = nfcOptions;
            this.nfcStatus = nfcStatus;
            this.adapter = adapter;
    }

    public void onItemClick(AdapterView<?> adapterView, View view,
        int position, long id) {

            Intent intent;

            if(bBlocked) {
                return;
            }

            bBlocked = true;
            TextView textView = (TextView) view.findViewWithTag(NfcArrayAdapter.DATA_STATE_TAG);
            if (nfcOptions[position].equals(context.getString(
                R.string.nfc))) {
                //toggle data enable/disable
                nfcStatus.setNfcStatus(!nfcStatus.isNfcEnabled());
                adapter.notifyDataSetChanged();
            }else if (nfcOptions[position].equals(context.getString(
                R.string.android_beam))) {
                //toggle data enable/disable
                if(nfcStatus.isNfcEnabled()){
                    nfcStatus.setAndroidBeamStatus(!nfcStatus.isAndroidBeamEnabled());
                    adapter.notifyDataSetChanged();
                }

            }
            bBlocked = false;
    }
}
