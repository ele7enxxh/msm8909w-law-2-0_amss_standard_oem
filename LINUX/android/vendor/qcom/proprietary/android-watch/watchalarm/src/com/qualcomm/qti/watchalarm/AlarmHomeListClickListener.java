/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchalarm;

import android.content.Context;
import android.content.Intent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;

public class AlarmHomeListClickListener implements OnItemClickListener {

    private Context context = null;
    private String[] listItemNames;

    public AlarmHomeListClickListener(Context context, String[] values) {

        this.context = context;
        this.listItemNames = values;
    }
    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        Intent intent;

        if (listItemNames[position].equals(context.getString(R.string.main_menu_item_alarm))) {
            intent = new Intent(context, AlarmActivity.class);
            context.startActivity(intent);

            return;
        }
        if (listItemNames[position].equals(context.getString(R.string.main_menu_item_timer))) {
            intent = new Intent(context, TimerActivity.class);
            context.startActivity(intent);

            return;
        }
    }

}
