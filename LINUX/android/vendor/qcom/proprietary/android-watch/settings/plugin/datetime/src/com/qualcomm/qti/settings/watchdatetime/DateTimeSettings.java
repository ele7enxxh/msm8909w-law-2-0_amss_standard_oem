/**
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchdatetime;

import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.text.format.DateFormat;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.util.Log;

public class DateTimeSettings extends Activity {
    private static DateTimeArrayAdapter adapter = null;
    private String[] rowValues;
    private Context context = null;
    private DateTimeData dateTimeData = null;
    DateTimeClickListener mDateTimeClickListener;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_datetime);
        dateTimeData = new DateTimeData();
        final String[] dateTimeOptions = {
            getString(R.string.auto_date),
            getString(R.string.auto_timezone),
            getString(R.string.set_date),
            getString(R.string.set_time),
            getString(R.string.set_timezone)
        };
        ListView listView = (ListView) findViewById(R.id.listViewDateTime);
        listView = (ListView) findViewById(R.id.listViewDateTime);
        adapter = new DateTimeArrayAdapter(this, dateTimeOptions, dateTimeData);
        mDateTimeClickListener =  new DateTimeClickListener(
            this, dateTimeOptions, adapter, dateTimeData);
        listView.setAdapter(adapter);
        listView.setOnItemClickListener(mDateTimeClickListener);
    }
}
