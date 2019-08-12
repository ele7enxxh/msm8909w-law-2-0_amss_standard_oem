/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchsettings.nfc;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.os.SystemProperties;
import android.nfc.NfcAdapter;
import android.content.IntentFilter;
import android.widget.ListView;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import com.qualcomm.qti.watchsettings.R;

public class NfcSettingsActivity  extends Activity{

    public static final String APP_TAG = "WatchSettings";
    private NfcStatus nfcStatus;
    private NfcArrayAdapter adapter;
    private IntentFilter mIntentFilter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_nfc);
        nfcStatus = new NfcStatus(this);

        final String[] NfcOptions = {
                getString(R.string.nfc),
                getString(R.string.android_beam)
        };

        ListView listView = (ListView) findViewById(R.id.listViewNfc);

        adapter = new NfcArrayAdapter(this, NfcOptions,
                        nfcStatus);
        listView.setAdapter(adapter);
        listView.setOnItemClickListener(new NfcClickListener(
            this, NfcOptions, nfcStatus, adapter));

        mIntentFilter = new IntentFilter(NfcAdapter.ACTION_ADAPTER_STATE_CHANGED);
    }

    @Override
    protected void onResume() {

        super.onResume();
        getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN);
        registerReceiver(mReceiver, mIntentFilter);

    }

    @Override
    protected void onDestroy() {

        unregisterReceiver(mReceiver);
        super.onDestroy();
    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (NfcAdapter.ACTION_ADAPTER_STATE_CHANGED.equals(action)) {
                adapter.notifyDataSetInvalidated();
            }
        }
    };


}