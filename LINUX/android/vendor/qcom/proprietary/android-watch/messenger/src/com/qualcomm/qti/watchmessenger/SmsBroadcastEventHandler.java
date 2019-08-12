/**
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchmessenger;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.ContentUris;
import android.content.ContentValues;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.telephony.SmsManager;
import android.widget.Toast;
import android.telephony.SmsMessage;
import android.os.Bundle;

import java.text.SimpleDateFormat;

import android.net.Uri;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.provider.Settings;
import android.provider.Telephony.Sms;
import android.util.Log;

public class SmsBroadcastEventHandler extends BroadcastReceiver {
    private static final String APP_TAG = "Messenger";
    public static final String MESSAGE_SENT_ACTION =
        "com.qualcomm.qti.watchmessenger.SmsBroadcastEventHandler.MESSAGE_SENT";
    public static final String MESSAGE_DELIVERED_ACTION =
        "com.qualcomm.qti.watchmessenger.SmsBroadcastEventHandler.MESSAGE_DELIVERED";
    private static final String SMS_RECEIVED_ACTION =
        "android.provider.Telephony.SMS_RECEIVED";
    private static final String SMS_DELIVER_ACTION =
        "android.provider.Telephony.SMS_DELIVER";
    private static final String SMS_SEND_ACTION =
        "android.provider.Telephony.SMS_SEND";

    private static final int RESULT_ERROR_GENERIC_FAILURE = 1;
    private static final int RESULT_ERROR_NO_SERVICE = 2;
    private static final int RESULT_ERROR_NULL_PDU = 3;
    private static final int RESULT_ERROR_RADIO_OFF = 4;
    private static final int RESULT_ERROR_OTHER_FAILURE = 5;
    private static final int DELIVERY_FAILURE = 6;


    private Context context;
    final String TIME_FORMAT_12_HOUR = "MMM dd  hh:mm a";

    public SmsBroadcastEventHandler(){
    }

    SmsBroadcastEventHandler(Context context) {
            this.context = context;
    }

    public void registerForEvents() {
        IntentFilter smsStatusIntentFilter = new IntentFilter();
        smsStatusIntentFilter.addAction(MESSAGE_SENT_ACTION);
        smsStatusIntentFilter.addAction(MESSAGE_DELIVERED_ACTION);
        smsStatusIntentFilter.addAction(SMS_DELIVER_ACTION);
        smsStatusIntentFilter.addAction(SMS_SEND_ACTION);
        context.registerReceiver(this, smsStatusIntentFilter);
    }

    public void unregisterForEvents() {
        context.unregisterReceiver(this);
    }

    public void onReceive(Context context, Intent intent) {
        String action = intent.getAction();
        if (action != null) {
            if (MESSAGE_SENT_ACTION.equals(action)) {
                getSmsSentStatus(intent);
            } else if (MESSAGE_DELIVERED_ACTION.equals(action)) {
                getSmsDeliveryStatus(intent);
            } else if (SMS_DELIVER_ACTION.equals(action)) {
                SmsReceived(context, intent);
            }
        }
    }

    public void SmsReceived(Context context, Intent intent) {
        SmsMessage[] smsMessage = null;
        Bundle bundle = intent.getExtras();

        if (bundle != null) {
            Object[] pdus = (Object[]) bundle.get("pdus");
            smsMessage = new SmsMessage[pdus.length];

            for (int i = 0; i < smsMessage.length; i++) {
                smsMessage[i] = SmsMessage.createFromPdu((byte[]) pdus[i]);
                handleNewMessage(context, smsMessage[i]);
            }
        }
    }

    public void handleNewMessage(Context ctx, SmsMessage sms) {
        Context context = ctx;
        SmsMessage smsMessage = sms;

        updateProvider(context, smsMessage);
        createNotification(context, smsMessage);
    }

    private void updateProvider(Context context, SmsMessage smsMessage){
        ContentValues contentValues = new ContentValues();
        contentValues.put(Sms.ADDRESS,smsMessage.getOriginatingAddress());
        contentValues.put(Sms.BODY,smsMessage.getMessageBody().toString());
        contentValues.put(Sms.DATE,new SimpleDateFormat(TIME_FORMAT_12_HOUR).format(
            smsMessage.getTimestampMillis()));
        contentValues.put(Sms.READ,0);

        Uri rowIdUri = context.getContentResolver().insert(
            Sms.CONTENT_URI, contentValues);
    }

    private void createNotification(Context context, SmsMessage smsMessage){
        final int NOTIFICATION_ID = 345;

        Intent clickIntent = new Intent(context, SmsActivity.class);

        clickIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK
                | Intent.FLAG_ACTIVITY_SINGLE_TOP
                | Intent.FLAG_ACTIVITY_CLEAR_TOP);

        PendingIntent pendingIntent = PendingIntent.getActivity(context, 0, clickIntent, PendingIntent.FLAG_UPDATE_CURRENT);

        Notification notification = new Notification.Builder(context)
                .setWhen(smsMessage.getTimestampMillis())
                .setSmallIcon(R.drawable.sms_notification_icon)
                .setContentTitle(smsMessage.getOriginatingAddress())
                .setContentText(smsMessage.getMessageBody().toString())
                .setContentIntent(pendingIntent)
                .setSound(Settings.System.DEFAULT_NOTIFICATION_URI)
                .setAutoCancel(true)
                .build();

        NotificationManager nm = (NotificationManager)
                context.getSystemService(Context.NOTIFICATION_SERVICE);
        nm.notify(NOTIFICATION_ID, notification);
    }

    private void getSmsSentStatus(Intent intent) {
        long rowIndex = intent.getLongExtra("rowIndex", -1);
        String status = context.getString(R.string.status_sending);
        int error =0;

        switch (getResultCode()) {
            case Activity.RESULT_OK:
                status = context.getString(R.string.status_sent);
                break;
            case SmsManager.RESULT_ERROR_GENERIC_FAILURE:
                Toast.makeText(context, context.getString(
                    R.string.sent_sms_error_generic_failure),
                        Toast.LENGTH_SHORT).show();
                status = context.getString(R.string.failed_msg_try_again);
                error = RESULT_ERROR_GENERIC_FAILURE;
                break;
            case SmsManager.RESULT_ERROR_NO_SERVICE:
                Toast.makeText(context, context.getString(
                    R.string.sent_sms_error_service_unavailable),
                        Toast.LENGTH_SHORT).show();
                status = context.getString(R.string.no_service_try_later);
                error = RESULT_ERROR_NO_SERVICE;
                break;
            case SmsManager.RESULT_ERROR_NULL_PDU:
                Toast.makeText(context, context.getString(
                    R.string.sent_sms_error_null_pdu),
                        Toast.LENGTH_SHORT).show();
                status = context.getString(R.string.failed_msg_try_again);
                error = RESULT_ERROR_NULL_PDU;
                break;
            case SmsManager.RESULT_ERROR_RADIO_OFF:
                Toast.makeText(context, context.getString(
                    R.string.sent_sms_error_radio_turned_off),
                        Toast.LENGTH_SHORT).show();
                status = context.getString(R.string.no_network_try_again);
                error = RESULT_ERROR_RADIO_OFF;
                break;
            default:
                Toast.makeText(context, context.getString(
                    R.string.sent_sms_error_unknown),
                        Toast.LENGTH_SHORT).show();
                status = context.getString(R.string.failed_msg_try_again);
                error = RESULT_ERROR_OTHER_FAILURE;
                break;
        }

        if(rowIndex != -1) {
            updateSmsStatus(rowIndex, status,error);
        }
    }

    private void getSmsDeliveryStatus(Intent intent) {
        long rowIndex = intent.getLongExtra("rowIndex", -1);
        String status = context.getString(R.string.status_sending);
        int error = 0;
        switch (getResultCode()) {
            case Activity.RESULT_OK:
                status = context.getString(R.string.status_delivered);
                break;
            case Activity.RESULT_CANCELED:
                Toast.makeText(context, context.getString(
                    R.string.sms_status_not_delivered),
                        Toast.LENGTH_SHORT).show();
                status = context.getString(R.string.sms_status_not_delivered);
                error = DELIVERY_FAILURE;
                break;
        }

        if(rowIndex != -1) {
            updateSmsStatus(rowIndex, status,error);
        }

    }

    private void updateSmsStatus(long rowIndex, String status,int error){
        ContentValues contentValues = new ContentValues();
        contentValues.put(Sms.STATUS, status);
        contentValues.put(Sms.ERROR_CODE, error);

        String where_update = Sms._ID + " = ?";
        String[] selectionArgs_update = {String.valueOf(rowIndex)};

        Uri single_uri = ContentUris.withAppendedId(Sms.CONTENT_URI,rowIndex);
        context.getContentResolver().update(single_uri, contentValues, where_update, selectionArgs_update);
    }

}
