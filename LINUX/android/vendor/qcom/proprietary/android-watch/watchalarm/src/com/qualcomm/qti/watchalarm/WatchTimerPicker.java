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
import android.widget.NumberPicker;
import android.widget.TextView;

public class WatchTimerPicker extends Activity{
    public static final String TIMER_PICKER_HOUR = "hour";
    public static final String TIMER_PICKER_MIN = "minutes";
    public static final String TIMER_PICKER_SEC = "seconds";
    private NumberPicker mHourPicker = null;
    private NumberPicker mMinPicker = null;
    private NumberPicker mSecPicker = null;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.watch_timer_picker);

        mHourPicker = (NumberPicker)findViewById(R.id.watchHourPicker);
        mHourPicker.setMinValue(0);
        mHourPicker.setMaxValue(23);
        mHourPicker.setDescendantFocusability(NumberPicker.FOCUS_BLOCK_DESCENDANTS);

        mMinPicker = (NumberPicker)findViewById(R.id.watchMinutesPicker);
        mMinPicker.setMinValue(0);
        mMinPicker.setMaxValue(59);
        mMinPicker.setDescendantFocusability(NumberPicker.FOCUS_BLOCK_DESCENDANTS);

        mSecPicker = (NumberPicker)findViewById(R.id.watchSecondsPicker);
        mSecPicker.setMinValue(0);
        mSecPicker.setMaxValue(59);
        mSecPicker.setDescendantFocusability(NumberPicker.FOCUS_BLOCK_DESCENDANTS);

        TextView tv = (TextView)findViewById(R.id.separator1);
        int height = mHourPicker.getHeight();
        tv.setHeight(height);

        tv = (TextView)findViewById(R.id.separator2);
        tv.setHeight(height);
    }

    public void onTimeSetClicked(View view) {
        Intent intent = new Intent();
        int hour = mHourPicker.getValue();
        int min = mMinPicker.getValue();
        int sec = mSecPicker.getValue();

        intent.putExtra(TIMER_PICKER_HOUR, hour);
        intent.putExtra(TIMER_PICKER_MIN, min);
        intent.putExtra(TIMER_PICKER_SEC, sec);
        setResult(RESULT_OK, intent);
        finish();
    }

}
