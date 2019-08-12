/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchalarm;

import android.app.Activity;
import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.provider.AlarmClock;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

public class TimerActivity extends Activity {

    public static final int TIMER_ID = 3001;
    private static final String TAG = "watchTimer";
    private ToggleButton mTimerButton = null;
    private AlarmManager mAlarmManager = null;
    private PendingIntent mPendingIntent = null;
    private TextView mTimerTextView = null;
    private Button mSetTimerButton = null;

    private int mHour = 0;
    private int mMinutes = 0;
    private int mSeconds = 0;

    private boolean mTimerRunning = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_timer);
        mTimerButton = (ToggleButton)findViewById(R.id.timerToggle);
        mSetTimerButton = (Button)findViewById(R.id.timerSet);
        mTimerTextView = (TextView)findViewById(R.id.timerText);
        mAlarmManager = (AlarmManager) getSystemService(ALARM_SERVICE);

        mSetTimerButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                openTimePicker();
            }
        });

        updateDisplayTime();

        Intent intent = getIntent();
        if(intent != null) {
            String action = intent.getAction();
            if(action != null) {
                if(action.equalsIgnoreCase(AlarmClock.ACTION_SET_TIMER)) {
                    Log.d(TAG, "Received ACTION_SET_TIMER");
                    handleSetTimerAction(intent);
                }
            }
        }
    }

    private void openTimePicker() {

        Intent intent = new Intent(this, WatchTimerPicker.class);
        startActivityForResult(intent, 1);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == 1) {
             if(resultCode == RESULT_OK){
                 mHour = data.getIntExtra(WatchTimerPicker.TIMER_PICKER_HOUR, 0);
                 mMinutes = data.getIntExtra(WatchTimerPicker.TIMER_PICKER_MIN, 0);
                 mSeconds = data.getIntExtra(WatchTimerPicker.TIMER_PICKER_SEC, 0);

                 updateDisplayTime();
             }
        }
    }

    private void handleSetTimerAction(Intent intent) {
        Bundle bundle = intent.getExtras();
        //get data from intent
        boolean skipUI = bundle.getBoolean(AlarmClock.EXTRA_SKIP_UI);
        String titleText = bundle.getString(AlarmClock.EXTRA_MESSAGE, " ");
        int timerLength = bundle.getInt(AlarmClock.EXTRA_LENGTH);

        if(timerLength >0) {
            mHour = timerLength/(60 * 60);
            mMinutes = (timerLength/60)%60;
            mSeconds = timerLength%60;

            startTimer(timerLength, titleText);

            //
            if(skipUI){
                Toast.makeText(this, "Timer: " + titleText + " time: " + timerLength, Toast.LENGTH_LONG).show();
                finish();
            }

            setTimerState(true);
        }
        //update UI
        mTimerTextView.setText(titleText);
        updateDisplayTime();
    }

    private void updateDisplayTime() {
        String time= String.format("%02d:%02d:%02d", mHour, mMinutes, mSeconds);
        TextView textView = (TextView)findViewById(R.id.timerValue);
        textView.setText(time);
    }

    public void onTimerToggleClicked(View view) {
        if (((ToggleButton) view).isChecked()) {
            Log.d(TAG, "Timer On");
            int timeInSec = mHour*3600 + mMinutes*60 + mSeconds;
            startTimer(timeInSec, "");
            mTimerRunning = true;
        } else {
            Log.d(TAG, "Timer Off");
            mTimerRunning = false;
            Intent serviceIntent = new Intent(TimerActivity.this, AlarmService.class);
            serviceIntent.setAction(AlarmService.ACTION_STOP);
            this.startService(serviceIntent);
            mPendingIntent.cancel();
        }
    }

    private void startTimer(int timeInSec, String message) {
        Intent intent = new Intent(TimerActivity.this, AlarmReceiver.class);
        intent.setData(Uri.parse("watchtimer://" + TIMER_ID));
        intent.setAction(String.valueOf(TIMER_ID));
        Bundle alarmBundle = new Bundle();
        alarmBundle.putString(WatchAlarm.ALARM_TITLE, getString(R.string.timer_title));
        alarmBundle.putString(WatchAlarm.ALARM_NAME, message);
        intent.putExtra(WatchAlarm.ALARM_DATA, alarmBundle);

        mPendingIntent = PendingIntent.getBroadcast(TimerActivity.this, 0, intent, PendingIntent.FLAG_CANCEL_CURRENT );

        Log.d(TAG, "Timer Set for " + timeInSec +" sec");
        Long timeInMiliSec = (long) (timeInSec * 1000);
        Log.d(TAG, "Timer Set for " + timeInMiliSec +" msec");
        mTimerRunning = true;
        mAlarmManager.set(AlarmManager.RTC, (System.currentTimeMillis() + timeInMiliSec), mPendingIntent);
    }

    @Override
    protected void onResume() {
       //TODO: get Data from pending intent

        Intent alarmIntent = new Intent(TimerActivity.this, AlarmReceiver.class);
        alarmIntent.setData(Uri.parse("watchtimer://" + TIMER_ID));
        alarmIntent.setAction(String.valueOf(TIMER_ID));
        mPendingIntent = PendingIntent.getBroadcast(TimerActivity.this, 0, alarmIntent, PendingIntent.FLAG_NO_CREATE);
        if(mPendingIntent == null) {
            setTimerState(false);
        } else {
            setTimerState(true);
        }
        super.onResume();
    }

    public void setTimerState(boolean timerRunning) {
        mTimerRunning = timerRunning;
        mTimerButton.setChecked(mTimerRunning);
    }
}
