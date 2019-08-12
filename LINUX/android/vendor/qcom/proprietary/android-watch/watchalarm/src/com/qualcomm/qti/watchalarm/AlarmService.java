/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchalarm;

import android.app.AlarmManager;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.media.Ringtone;
import android.media.RingtoneManager;
import android.net.Uri;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Vibrator;
import android.provider.AlarmClock;
import android.util.Log;

public class AlarmService extends Service{

    private static final String TAG = "WatchAlarmService";

    public static final String ACTION_START = "com.qualcomm.qti.watchalarmservice.action.START";
    public static final String ACTION_STOP = "com.qualcomm.qti.watchalarmservice.action.STOP";
    private static final int ALARM_NOTIFICATION_ID = 2001;

    private Vibrator mVibrator = null;
    private Ringtone mAlarmRingTone = null;

    public void OnCreate() {
        super.onCreate();
    }

    @Override
    public void onDestroy() {
        dismissAlarm();
        super.onDestroy();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if (intent != null) {
            if (intent.getAction() != null) {
                String action = intent.getAction();
                if(action.equalsIgnoreCase(ACTION_STOP)){
                    dismissAlarm();
                    stopSelf();
                } else {
                    Bundle bundle = intent.getBundleExtra(WatchAlarm.ALARM_DATA);
                    boolean bVibrate = bundle.getBoolean(WatchAlarm.ALARM_VIBRATE);
                    String ringTone = bundle.getString(WatchAlarm.ALARM_RINGTONE);
                    String alarmTitle = bundle.getString(WatchAlarm.ALARM_TITLE);
                    String alarmName = bundle.getString(WatchAlarm.ALARM_NAME);
                    sendNotification(alarmTitle, alarmName, bVibrate, ringTone);
                }
            }
        }
        return START_NOT_STICKY;
    }

    private void dismissAlarm() {
        Context context = getApplicationContext();
        Intent alarmIntent = new Intent(context, AlarmReceiver.class);
        alarmIntent.setData(Uri.parse("watchalarm://" + AlarmActivity.ALARM_ID));
        alarmIntent.setAction(String.valueOf(AlarmActivity.ALARM_ID));
        PendingIntent pendingIntent = PendingIntent.getBroadcast(context, 0, alarmIntent, PendingIntent.FLAG_NO_CREATE);
        if(pendingIntent != null) {
            AlarmManager alarmManager = (AlarmManager) context.getSystemService(ALARM_SERVICE);
            alarmManager.cancel(pendingIntent);
            pendingIntent.cancel();
        }
        if(mAlarmRingTone != null){
            mAlarmRingTone.stop();
        }
        mVibrator = (Vibrator) getApplicationContext().getSystemService(VIBRATOR_SERVICE);
        mVibrator.cancel();

        NotificationManager alarmNotificationManager = (NotificationManager) this
                .getSystemService(Context.NOTIFICATION_SERVICE);
        alarmNotificationManager.cancel(ALARM_NOTIFICATION_ID);

        AlarmActivity alarmActivity = AlarmActivity.getAlarmActivity();
        if(alarmActivity != null) {
            alarmActivity.setAlarmState(false);
        }
    }

    private void sendNotification(String alarmTitle, String alarmName, boolean bVibrate, String ringTone) {
        Log.d("AlarmService", "Preparing to send notification...: " + alarmTitle);
        Uri alarmUri = null;
        //Play Alarm RingTone
        if(ringTone != null && ringTone.equalsIgnoreCase(AlarmClock.VALUE_RINGTONE_SILENT)) {
            Log.d(TAG, "Alarm Ringtone: SILENT");
        } else {
            if(ringTone != null) {
                alarmUri = Uri.parse(ringTone);
                Log.d(TAG, "Alarm Ringtone: " + ringTone);
            } else {
                alarmUri = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_ALARM);
                if (alarmUri == null) {
                    alarmUri = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_NOTIFICATION);
                }
            }
            mAlarmRingTone = RingtoneManager.getRingtone(getApplicationContext(), alarmUri);
            mAlarmRingTone.play();
        }
        //Vibrate
        if(bVibrate) {
            mVibrator = (Vibrator) getApplicationContext().getSystemService(VIBRATOR_SERVICE);
            if(mVibrator != null) {
                mVibrator.vibrate(new long[] { 500, 1000, 500 }, 0);
            }
        }

        //Add Notification
        NotificationManager alarmNotificationManager = (NotificationManager) this
                .getSystemService(Context.NOTIFICATION_SERVICE);

        PendingIntent contentIntent = PendingIntent.getActivity(this, 0,
                new Intent(this, AlarmActivity.class), 0);

        Notification.Builder alarmNotificationBuilder = new Notification.Builder(
                this).setContentTitle(alarmTitle).setSmallIcon(R.drawable.ic_launcher)
                .setStyle(new Notification.BigTextStyle().bigText(alarmTitle))
                .setContentText(alarmName);


        alarmNotificationBuilder.setContentIntent(contentIntent);

        Intent intent = new Intent(getApplicationContext(), AlarmNotificationDismissedReceiver.class);
        intent.putExtra("com.qualcomm.qti.watchalarm.notificationId", ALARM_NOTIFICATION_ID);

        PendingIntent alarmNotificationPendingIntent =  PendingIntent.getBroadcast(
                                                            getApplicationContext(),
                                                            ALARM_NOTIFICATION_ID, intent, 0);
        alarmNotificationBuilder.setDeleteIntent(alarmNotificationPendingIntent);

        alarmNotificationManager.notify(ALARM_NOTIFICATION_ID, alarmNotificationBuilder.build());
        Log.d(TAG, "Notification sent.");
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

}
