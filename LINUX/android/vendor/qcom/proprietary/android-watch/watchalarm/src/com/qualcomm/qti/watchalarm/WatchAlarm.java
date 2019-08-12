/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchalarm;

import java.util.ArrayList;

public class WatchAlarm{

    private int alarm_ID = 0;
    private boolean mAlarmON = false;
    private int mHour = 0;
    private int mMinutes = 0;
    private ArrayList<Integer> mDays;
    private boolean mVibrate = true;
    private String mAlarmName;
    private String mRingtone = null;

    public static final String ALARM_DATA = "AlarmData";
    public static final String ALARM_TITLE = "AlarmTitle";
    public static final String ALARM_NAME = "AlarmName";
    public static final String ALARM_VIBRATE = "AlarmVibrate";
    public static final String ALARM_RINGTONE = "AlarmRingtone";

    public WatchAlarm() {

    }

    public WatchAlarm(int hour,
                        int minutes,
                        ArrayList<Integer> days,
                        boolean vibrate,
                        String ringtone,
                        String name) {
        this.mHour = hour;
        this.mMinutes = minutes;
        this.mAlarmName = name;
        this.mDays = days;
        this.mVibrate = vibrate;
        this.mRingtone = ringtone;
    }

    public void setAlarmID(int id) {
        this.alarm_ID = id;
    }

    public int getAlarmID() {
        return alarm_ID;
    }

    public void turnAlarmON(boolean alarmON) {
        this.mAlarmON = alarmON;
    }

    public boolean isAlalrmON() {
        return mAlarmON;
    }

    public void setHour(int hour) {
        this.mHour = hour;
    }

    public void setMinutes(int minutes) {
        this.mMinutes = minutes;
    }

    public void setVibration(boolean vibrate) {
        this.mVibrate = vibrate;
    }

    public void setDays(ArrayList<Integer> days) {
        this.mDays = days;
    }

    public void setAlarmName(String name) {
        this.mAlarmName = name;
    }

    public int getHour() {
        return mHour;
    }

    public int getMinutes() {
        return mMinutes;
    }

    public boolean doVibration() {
        return mVibrate;
    }

    public ArrayList<Integer> getDays() {
        return mDays;
    }

    public String getAlarmName() {
        return mAlarmName;
    }

    public String getRingTone() {
        return mRingtone;
    }
}
