/*
 *                     Location Alarm Service
 *
 * GENERAL DESCRIPTION
 *   This file is the Service for the Alarms
 *
 * Copyright (c) 2012-2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 */

package com.qualcomm.location.localarm;

import java.util.LinkedList;
import java.util.ListIterator;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.PowerManager;
import android.os.IBinder;
import android.util.Log;

public class LocationAlarmService extends Service implements Runnable{
    public static final String LOG_TAG = "LAlarm";

    private AlarmManager mAlarmManager;
    private Intent mIntent;
    private PendingIntent mPendingIntent;
    private String message;
    private IntentFilter mIntentFilter;
    private LocationAlarmReceiver mAlarmReceiver;
    private boolean mReg;
    private LinkedList<Long> mAlarmList;


    static {
        System.loadLibrary("alarmservice_jni");
      }

    public boolean register() {
        int i;
        for (i = 0; i < 20; i++) {
            // Try registration a few times.
            mReg = native_register();
            if (mReg == true) {
                break;
            }
            try {
                Thread.sleep(1000);  // 1 seconds.
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

      return mReg;
    }

    public void deregister() {
        // If registered, deregister.
        if (mReg) {
            native_deregister();
        }
    }

    public void addAlarm(int duration) {
        long startTime = System.currentTimeMillis();
        long alarmTime = startTime + duration;
        int i = 0;

        String logMsg = "Alarm Added at " + String.valueOf(startTime) + " for " +
        String.valueOf(duration) + " ms.";
        Log.d(LOG_TAG, logMsg);
        synchronized (this) {
            ListIterator <Long> itr = mAlarmList.listIterator();
            Long e = null;
            while (itr.hasNext()) {
                e = itr.next();
                i++;
                // Find location where timer in to be inserted.
                if (e.longValue() > alarmTime) {
                    e = itr.previous();
                    i--;
                    break;
                }
                e = null;
            }
            if ( (e == null) || (e.longValue() - alarmTime > 20)) {
                itr.add(new Long(alarmTime));
                // Only set alarm if this new alarm was inserted at head of list.
                if (i == 0) {
                    setAlarm(alarmTime);
                }
            }
        }
        Log.d(LOG_TAG, "Number of Alarms: " + mAlarmList.size() + ". Shortest: " +
                String.valueOf(mAlarmList.peek().longValue() - startTime));
    }
    public void setAlarm(long alarmAt) {
        long currentTime = System.currentTimeMillis();
        long alarmDurationMs = alarmAt-currentTime;

        String logMsg = "Alarm Set at " + String.valueOf(currentTime) + " for " +
        String.valueOf(alarmDurationMs) + " ms.";
        Log.d(LOG_TAG, logMsg);
        mAlarmManager.set(AlarmManager.RTC_WAKEUP, alarmAt, mPendingIntent);
        // If Alarm is for small duration, acquire wakelock until the
        // alarm expires + 10 ms
        if ((alarmDurationMs > 0) && (alarmDurationMs < 1500)) {
            PowerManager pm = (PowerManager) getSystemService(POWER_SERVICE);
            PowerManager.WakeLock wl = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK,
                    LocationAlarmReceiver.ACTION);
            wl.acquire(alarmDurationMs+10);
        }
    }

    public void setNextAlarm() {
        long currentTime = System.currentTimeMillis();
        synchronized (this) {
            ListIterator <Long> itr = mAlarmList.listIterator();
            while (itr.hasNext()) {
                Long e = itr.next();
                // Remove timers with expiration time < current time.
                if (e.longValue() <= currentTime) {
                    itr.remove();
                }
                else {
                    break;
                }
            }
            if (!mAlarmList.isEmpty()) {
                setAlarm(mAlarmList.peek().longValue());
            }
        }
    }
    @Override
    public void run() {
        Log.e(LOG_TAG, "Thread created.");
        if (register() == true)
        {
           while(true)
           {
              int duration = native_wait_for_alarm();
              if (duration > 0) {
                  addAlarm(duration);
              }
           }
        }
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.e(LOG_TAG, "Service created.");
        mAlarmManager = (AlarmManager) getSystemService(ALARM_SERVICE);
        mAlarmReceiver = new LocationAlarmReceiver();
        mIntentFilter = new IntentFilter(LocationAlarmReceiver.ACTION);
        this.registerReceiver(mAlarmReceiver, mIntentFilter);
        mIntent = new Intent(LocationAlarmReceiver.ACTION);
        mPendingIntent = PendingIntent.getBroadcast(this, 0, mIntent, 0);

        mAlarmList = new LinkedList<Long> ();
        // Process timer/alarm requests in a separate thread.
        new Thread(this).start();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        Log.e(LOG_TAG, "Service started flags " + flags + " startId " + startId);

        // We want this service to continue running until it is explicitly
        // stopped, so return sticky.
        return START_STICKY;
    }
    @Override
    public void onDestroy() {
           super.onDestroy();
           this.unregisterReceiver(mAlarmReceiver);
           deregister();
           Log.e(LOG_TAG, "Service destroyed.");
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.e(LOG_TAG, "Service bind.");
        return null;
    }

    private native boolean native_register();
    private native boolean native_deregister();
    private native int native_wait_for_alarm();
}
