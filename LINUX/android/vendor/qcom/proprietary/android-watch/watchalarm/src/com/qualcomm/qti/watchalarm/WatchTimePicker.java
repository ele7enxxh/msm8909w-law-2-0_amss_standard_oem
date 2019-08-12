/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchalarm;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.TimePicker;

public class WatchTimePicker extends Activity{


    public static final String TIME_PICKER_HOUR = "hour";
    public static final String TIME_PICKER_MIN = "minutes";
    private TimePicker mTimePicker = null;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.watch_time_picker);

        mTimePicker = (TimePicker)findViewById(R.id.watchTimePicker);
        mTimePicker.setIs24HourView(true);
    }

    public void onTimeSetClicked(View view) {
        Intent intent = new Intent();
        int hour = mTimePicker.getCurrentHour();
        int min = mTimePicker.getCurrentMinute();

        intent.putExtra(TIME_PICKER_HOUR, hour);
        intent.putExtra(TIME_PICKER_MIN, min);
        setResult(RESULT_OK, intent);
        finish();
    }


}
