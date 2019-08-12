/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchalarm;

import java.util.ArrayList;
import java.util.Calendar;

import android.net.Uri;
import android.os.Bundle;
import android.provider.AlarmClock;
import android.app.Activity;
import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.Intent;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

public class AlarmActivity extends Activity {

    public static final int ALARM_ID = 1001;
    private static final int ACTIVITY_TIME_PICKER = 1;
    private static final String TAG = "watchAlarm";
    private final String TIME_FORMAT = "%02d:%02d";
    private TextView mAlarmName = null;
    private Button mTimeSelectButton = null;
    private TextView mTimeView = null;
    private TextView mRepeatText = null;
    private ToggleButton mAlarmButton = null;
    private AlarmManager mAlarmManager = null;
    private PendingIntent mPendingIntent = null;
    private static AlarmActivity myInstance = null;
    private WatchAlarm mWatchAlarm = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_alarm);
        mTimeSelectButton = (Button)findViewById(R.id.timeSelect);
        mTimeSelectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showTimePicker();
            }
        });

        mTimeView= (TextView)findViewById(R.id.time);
        mAlarmButton = (ToggleButton)findViewById(R.id.alarmToggle);
        mAlarmName = (TextView)findViewById(R.id.alarmName);
        mAlarmManager = (AlarmManager) getSystemService(ALARM_SERVICE);
        mRepeatText = (TextView)findViewById(R.id.repeatText);
        String time = String.format(TIME_FORMAT, 0, 0);
        mTimeView.setText(time);

        mAlarmName.setText(R.string.alarm_title);
        mRepeatText.setText(R.string.alarm_repeat_default);
        mWatchAlarm = new WatchAlarm();

        Intent intent = getIntent();
        if(intent != null) {
            String action = intent.getAction();
            if(action != null) {
                if(action.equalsIgnoreCase(AlarmClock.ACTION_SET_ALARM)) {
                    Log.d(TAG, "Received ACTION_SET_ALARM");
                    handleSetAlarmAction(intent);
                } else if(action.equalsIgnoreCase(AlarmClock.ACTION_SHOW_ALARMS)) {
                    Log.d(TAG, "Received ACTION_SHOW_ALARMS");
                }
            }
        }
    }

    private void showTimePicker() {
        Intent intent = new Intent(this, WatchTimePicker.class);
        startActivityForResult(intent, ACTIVITY_TIME_PICKER);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == ACTIVITY_TIME_PICKER) {
             if(resultCode == RESULT_OK){
                 int hour = data.getIntExtra(WatchTimePicker.TIME_PICKER_HOUR, 0);
                 int min = data.getIntExtra(WatchTimePicker.TIME_PICKER_MIN, 0);
                 mWatchAlarm.setHour(hour);
                 mWatchAlarm.setMinutes(min);

                 String time= String.format(TIME_FORMAT, hour, min);
                 mTimeView.setText(time);
             }
        }
    }

    private void handleSetAlarmAction(Intent intent) {
        Bundle bundle = intent.getExtras();
        //get data from intent
        boolean skipUI = bundle.getBoolean(AlarmClock.EXTRA_SKIP_UI);
        String titleText = bundle.getString(AlarmClock.EXTRA_MESSAGE, " ");
        ArrayList<Integer> days= bundle.getIntegerArrayList(AlarmClock.EXTRA_DAYS);
        int hour = bundle.getInt(AlarmClock.EXTRA_HOUR);
        int minutes =  bundle.getInt(AlarmClock.EXTRA_MINUTES);
        boolean bVibrate = bundle.getBoolean(AlarmClock.EXTRA_VIBRATE);
        String ringtone = bundle.getString(AlarmClock.EXTRA_RINGTONE);

        String stringDay = "";
        if(days!= null) {
            for(int day: days) {
                switch(day) {
                    case 1: stringDay+=" " + getString(R.string.sunday);
                    break;
                    case 2: stringDay+=" " + getString(R.string.monday);
                    break;
                    case 3: stringDay+=" " + getString(R.string.tuesday);
                    break;
                    case 4: stringDay+=" " + getString(R.string.wednesday);
                    break;
                    case 5: stringDay+=" " + getString(R.string.thursday);
                    break;
                    case 6: stringDay+=" " + getString(R.string.friday);
                    break;
                    case 7: stringDay+=" " + getString(R.string.saturday);
                    break;
                }
            }
            mRepeatText.setText(getString(R.string.alarm_repeat) + stringDay);
        }
        //create alarm
        mWatchAlarm = new WatchAlarm(hour, minutes, days,
                bVibrate, ringtone, titleText);
        if(hour >0 || minutes >0) {
            //set alarm ON
            setAlarm(mWatchAlarm);
        }

        if(skipUI){
            Toast.makeText(this, "Alarm: " + titleText + " " + hour+":"+minutes, Toast.LENGTH_LONG).show();
            finish();
        }

        //update UI
        mAlarmName.setText(titleText);
        String time = String.format(TIME_FORMAT, hour, minutes);
        mTimeView.setText(time);
    }

    public static AlarmActivity getAlarmActivity() {
        return myInstance;
    }

    @Override
    public void onStart() {
        super.onStart();
        myInstance = this;
    }

    public void onAlarmToggleClicked(View view) {
        if (((ToggleButton) view).isChecked()) {
            Log.d(TAG, "Alarm On");
            mWatchAlarm.setAlarmName(getString(R.string.alarm_title));
            setAlarm(mWatchAlarm);
        } else {
            Log.d(TAG, "Alarm Off");
            mWatchAlarm.turnAlarmON(false);

            Intent serviceIntent = new Intent(AlarmActivity.this, AlarmService.class);
            serviceIntent.setAction(AlarmService.ACTION_STOP);
            this.startService(serviceIntent);
        }
    }

    private int setAlarm(WatchAlarm alarm) {

        alarm.setAlarmID(ALARM_ID);

        Calendar calendar = Calendar.getInstance();
        calendar.set(Calendar.HOUR_OF_DAY, alarm.getHour());
        calendar.set(Calendar.MINUTE, alarm.getMinutes());

        Intent intent = new Intent(AlarmActivity.this, AlarmReceiver.class);
        intent.setData(Uri.parse("watchalarm://" + alarm.getAlarmID()));
        intent.setAction(String.valueOf(alarm.getAlarmID()));
        Bundle alarmBundle = new Bundle();
        alarmBundle.putString(WatchAlarm.ALARM_TITLE, getString(R.string.alarm_title));
        alarmBundle.putString(WatchAlarm.ALARM_NAME, alarm.getAlarmName());
        alarmBundle.putBoolean(WatchAlarm.ALARM_VIBRATE, alarm.doVibration());
        alarmBundle.putString(WatchAlarm.ALARM_RINGTONE, alarm.getRingTone());
        intent.putExtra(WatchAlarm.ALARM_DATA, alarmBundle);

        mPendingIntent = PendingIntent.getBroadcast(AlarmActivity.this, 0, intent, PendingIntent.FLAG_CANCEL_CURRENT );

        long timeInMiliSec = calendar.getTimeInMillis();
        Log.d(TAG, "Alarm Set for " + timeInMiliSec +" msec");

        mAlarmManager.set(AlarmManager.RTC, timeInMiliSec, mPendingIntent);
        alarm.turnAlarmON(true);

        return 0;
    }

    @Override
    protected void onResume() {
       //TODO: get WatchAlarm

        Intent alarmIntent = new Intent(AlarmActivity.this, AlarmReceiver.class);
        alarmIntent.setData(Uri.parse("watchalarm://" + ALARM_ID));
        alarmIntent.setAction(String.valueOf(ALARM_ID));
        mPendingIntent = PendingIntent.getBroadcast(AlarmActivity.this, 0, alarmIntent, PendingIntent.FLAG_NO_CREATE);
        if(mPendingIntent == null) {
            setAlarmState(false);
        } else {
            setAlarmState(true);
        }


        super.onResume();
    }

    public void setAlarmState(boolean alarmON) {
        mWatchAlarm.turnAlarmON(alarmON);
        mAlarmButton.setChecked(mWatchAlarm.isAlalrmON());
    }

    public WatchAlarm getAlarm() {
        return mWatchAlarm;
    }
}
