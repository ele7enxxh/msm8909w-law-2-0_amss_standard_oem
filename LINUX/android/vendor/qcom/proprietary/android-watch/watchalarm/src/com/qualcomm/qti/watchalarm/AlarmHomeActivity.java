/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchalarm;

import android.os.Bundle;
import android.app.Activity;
import android.widget.ListView;

public class AlarmHomeActivity extends Activity {

    public static final String APP_TAG = "WatchAlarm";
    AlarmHomeListAdapter listAdapter; // for handling main activity list items

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_alarm_home);

        // list of options in the main menu of settings application
        String[] listRowNames = {
            getString(R.string.main_menu_item_alarm),
            getString(R.string.main_menu_item_timer),
        };

        listAdapter = new AlarmHomeListAdapter(this, listRowNames);
        ListView listView = (ListView) findViewById(R.id.listView);
        listView.setAdapter(listAdapter);
        listView.setOnItemClickListener(new AlarmHomeListClickListener(this, listRowNames));
    }
}
