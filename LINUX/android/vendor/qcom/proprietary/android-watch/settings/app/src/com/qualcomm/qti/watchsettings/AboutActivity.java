/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchsettings;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

public class AboutActivity extends Activity{

    private static final String APP_TAG = "WatchSettings";

    private String[] rowValues;
    private ArrayAdapter adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.about_activity);

        String[] rowTags = {getString(R.string.about_device_name),
            getString(R.string.about_bt_address),
            getString(R.string.about_sw_version)};

        String version = Build.VERSION.CODENAME + "-" + Build.VERSION.INCREMENTAL +
                "-" + Build.VERSION.RELEASE + "-" + Build.VERSION.SDK_INT;

        Log.d(APP_TAG, "VERSION = " + Build.VERSION.CODENAME + "-" + Build.VERSION.INCREMENTAL +
                "-" + Build.VERSION.RELEASE + "-" + Build.VERSION.SDK_INT);

        rowValues = new String[3];
        rowValues[0] = BluetoothAdapter.getDefaultAdapter().getName();
        rowValues[1] = BluetoothAdapter.getDefaultAdapter().getAddress();
        rowValues[2] = version;


        adapter = new ArrayAdapter(this, android.R.layout.simple_expandable_list_item_2,
                android.R.id.text1, rowTags) {
            @Override
            public View getView(int position, View convertView, ViewGroup parent) {

                View view = super.getView(position, convertView, parent);
                TextView value = (TextView) view.findViewById(android.R.id.text2);
                value.setText(rowValues[position]);
                return view;
            }
        };

        ListView listView = (ListView) findViewById(R.id.listViewAbout);
        listView.setAdapter(adapter);
    }
}