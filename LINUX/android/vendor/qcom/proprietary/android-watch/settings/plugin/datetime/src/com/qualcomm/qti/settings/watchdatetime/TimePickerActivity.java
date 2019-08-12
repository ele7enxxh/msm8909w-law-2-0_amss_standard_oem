/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchdatetime;

import java.util.Calendar;

import android.app.Activity;
import android.app.AlarmManager;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TimePicker;

public class TimePickerActivity extends Activity{
    public static final String TIME_PICKER_HOUR = "hour";
    public static final String TIME_PICKER_MIN = "minutes";
    private TimePicker mTimePicker = null;
    private Context context = this;
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_timepicker);
        mTimePicker = (TimePicker)findViewById(R.id.watchTimePicker);
        mTimePicker.setIs24HourView(false);
    }
    public void onTimeSetClicked(View view) {
        Intent intent = new Intent();
        final Calendar calendar = Calendar.getInstance();
        int hour = mTimePicker.getCurrentHour();
        int min = mTimePicker.getCurrentMinute();
        setTime(context, hour, min);
        finish();
    }

    static void setTime(Context context, int hourOfDay, int minute) {
        Calendar c = Calendar.getInstance();
        Log.d("setTime ", "hourOfDay "+hourOfDay + "minute" + minute);
        c.set(Calendar.HOUR_OF_DAY, hourOfDay);
        c.set(Calendar.MINUTE, minute);
        c.set(Calendar.SECOND, 0);
        c.set(Calendar.MILLISECOND, 0);
        long when = c.getTimeInMillis();
           ((AlarmManager) context.getSystemService(Context.ALARM_SERVICE)).setTime(when);
   }
}

