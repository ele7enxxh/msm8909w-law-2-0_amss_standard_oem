/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.qti.settings.watchdatetime;


import java.sql.Date;
import java.util.Calendar;

import android.app.Activity;
import android.app.AlarmManager;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.text.format.DateFormat;
import android.util.Log;
import android.view.View;
import android.widget.DatePicker;
import android.widget.Toast;

public class DatePickerActivity extends Activity{
    public static final String TIME_PICKER_HOUR = "hour";
    public static final String TIME_PICKER_MIN = "minutes";
    private DatePicker mDatePicker = null;
    private Context context = this;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_datepicker);
        mDatePicker = (DatePicker)findViewById(R.id.datePicker);
    }

    public void onDateSetClicked(View view) {
        Intent intent = new Intent();
        int year = mDatePicker.getYear();
        int month = mDatePicker.getMonth();
        int day = mDatePicker.getDayOfMonth();
        if(year <= 1969){
            Toast.makeText(context,R.string.year_check,Toast.LENGTH_SHORT).show();
        }else{
            setDate(context, year, month, day);
            finish();
        }
    }
   static void setDate(Context context, int year, int month, int day) {
        Calendar c = Calendar.getInstance();
        c.set(Calendar.YEAR, year);
        c.set(Calendar.MONTH, month);
        c.set(Calendar.DAY_OF_MONTH, day);
        long when = c.getTimeInMillis();
            ((AlarmManager) context.getSystemService(Context.ALARM_SERVICE)).setTime(when);
    }
}
