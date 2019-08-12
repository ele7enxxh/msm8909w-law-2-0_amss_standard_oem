/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchmessenger;

import android.app.AlertDialog;
import android.app.LoaderManager;
import android.app.PendingIntent;
import android.content.ContentResolver;
import android.content.ContentUris;
import android.content.ContentValues;
import android.content.Context;
import android.content.CursorLoader;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.Loader;
import android.database.ContentObserver;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.telephony.SmsManager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.SimpleCursorAdapter;
import android.widget.TextView;
import android.provider.Telephony.Sms;

import java.util.ArrayList;

public class CursorAdapter extends SimpleCursorAdapter implements
    LoaderManager.LoaderCallbacks<Cursor>, AdapterView.OnItemClickListener,
    AdapterView.OnItemLongClickListener {
    private Context context;
    private ArrayList<SmsData> smsDataArrayList;
    private LoaderManager loaderManager;
    private static final int LOADER_ID = 1;
    private Uri content_uri=Sms.CONTENT_URI;
    private ContentResolver cr;
    private LoaderManager.LoaderCallbacks<Cursor> cb;
    private ContentObserver observer;
    private SmsManager smsManager;
    private PendingIntent msgSentPendingIntent;
    private PendingIntent msgDeliveredPendingIntent;
    ArrayList<PendingIntent> sentIntents = null;
    ArrayList<PendingIntent> deliveredIntents = null;

    public CursorAdapter(Context context, int layout, Cursor c, String[] from,
        int[] to, int flags, LoaderManager loaderManager,
        ContentResolver contentResolver) {
        super(context, layout, c, from, to, flags);
        this.context = context;
        this.loaderManager = loaderManager;
        this.cr = contentResolver;
        cb = this;
        smsDataArrayList = new ArrayList<SmsData>();

        loaderManager.initLoader(LOADER_ID, null, this);
    }

    public Loader<Cursor> onCreateLoader(int id, Bundle args) {
        String sortOrder = Sms.DATE + " ASC";
        observer = new contentObserver(new Handler());
        cr.registerContentObserver(content_uri, true, observer);
        return new CursorLoader(context, Sms.Inbox.CONTENT_URI, null, null, null, sortOrder);
    }

    public void onLoadFinished(Loader<Cursor> loader, Cursor data) {

        if (data != null && data.moveToFirst()) {

            smsDataArrayList.clear();
            notifyDataSetChanged();
            while (data.isAfterLast() == false) {
                int index = data.getInt(data.getColumnIndex(Sms._ID));
                String msg_from = data.getString(data.getColumnIndex(Sms.ADDRESS));
                String msg_text = data.getString(data.getColumnIndex(Sms.BODY));
                String ms_ts =  data.getString(data.getColumnIndex(Sms.DATE));
                String msg_status = data.getString(data.getColumnIndex(Sms.STATUS));
                int read = data.getInt(data.getColumnIndex(Sms.READ));
                int error = data.getInt(data.getColumnIndex(Sms.ERROR_CODE));

                SmsData smsData = new SmsData();
                smsData.setSmsRowIndex(index);
                smsData.setSmsFrom(msg_from);
                smsData.setSmsBody(msg_text);
                smsData.setSmsReadStatus(Utils.getMsgStatus(read,msg_status));
                smsData.setSmsDateTime(ms_ts);
                smsData.setSmsErrorCode(error);

                smsDataArrayList.add(0, smsData);
                notifyDataSetChanged();
                data.moveToNext();
            }
        } else if(data.moveToFirst() == false){
            smsDataArrayList.clear();
            notifyDataSetChanged();
        }
        data.close();
    }

    public void onLoaderReset(Loader<Cursor> loader) {
        swapCursor(null);
        cr.unregisterContentObserver(observer);
    }

    class contentObserver extends ContentObserver{

        public contentObserver(Handler handler) {
            super(handler);
        }

        public void onChange(boolean selfChange) {
            super.onChange(selfChange);
        }

        public void onChange(boolean selfChange, Uri uri) {
            super.onChange(selfChange, uri);
            loaderManager.getLoader(LOADER_ID).forceLoad();
        }
    }

    public void onItemClick(AdapterView<?> parent, View view, int position,
        long id) {
        SmsData smsData = smsDataArrayList.get(position);
        int rowIndex = smsData.getSmsRowIndex();
        String status = smsData.getSmsReadStatus();
        String msg = smsData.getSmsBody().toString();
        String sender = smsData.getSmsFrom();
        int error = smsData.getSmsErrorCode();

       if(error > 0) {
           retryMsg(smsData);
           return;
        }

       Intent intent = new Intent(context, SmsDisplayActivity.class);
       intent.putExtra("msg", msg);
       intent.putExtra(Sms.ADDRESS,sender);
       context.startActivity(intent);

       if(status.equals(Utils.UNREAD)) {
           changeMessageStatus(rowIndex);
       }
    }

    public void retryMsg(SmsData smsData) {
        String phoneNumber = smsData.getSmsFrom();
        String msgText = smsData.getSmsBody().toString();
        long rowIndex = Long.valueOf(smsData.getSmsRowIndex());
        ArrayList<String> messages = null;

        smsManager = SmsManager.getDefault();
        messages = smsManager.divideMessage(msgText);
        int messageCount = messages.size();

        if(messageCount == 1){
            Intent msgSentIntent = new Intent(SmsBroadcastEventHandler.MESSAGE_SENT_ACTION);
            msgSentIntent.putExtra("rowIndex", rowIndex);
            msgSentPendingIntent = PendingIntent.getBroadcast(context, 0, msgSentIntent, PendingIntent.FLAG_UPDATE_CURRENT);

            Intent msgDeliveredIntent = new Intent(SmsBroadcastEventHandler.MESSAGE_DELIVERED_ACTION);
            msgDeliveredIntent.putExtra("rowIndex", rowIndex);
            msgDeliveredPendingIntent = PendingIntent.getBroadcast(context, 0, msgDeliveredIntent, PendingIntent.FLAG_UPDATE_CURRENT);

            smsManager.sendTextMessage(phoneNumber, null, msgText,
            msgSentPendingIntent, msgDeliveredPendingIntent);
        } else {
            sentIntents = new ArrayList<PendingIntent>(messageCount);
            deliveredIntents = new ArrayList<PendingIntent>(messageCount);

            for(int i=0; i < messageCount; i++) {
                Intent msgSentIntent = new Intent(SmsBroadcastEventHandler.MESSAGE_SENT_ACTION);
                msgSentIntent.putExtra("rowIndex", rowIndex);
                msgSentPendingIntent = PendingIntent.getBroadcast(context, 0, msgSentIntent, PendingIntent.FLAG_UPDATE_CURRENT);
                sentIntents.add(msgSentPendingIntent);

                Intent msgDeliveredIntent = new Intent(SmsBroadcastEventHandler.MESSAGE_DELIVERED_ACTION);
                msgDeliveredIntent.putExtra("rowIndex", rowIndex);
                msgDeliveredPendingIntent = PendingIntent.getBroadcast(context, 0, msgDeliveredIntent, PendingIntent.FLAG_UPDATE_CURRENT);
                deliveredIntents.add(msgDeliveredPendingIntent);
            }

            smsManager.sendMultipartTextMessage(phoneNumber, null, messages, sentIntents, deliveredIntents);
        }
    }

    public boolean onItemLongClick(AdapterView<?> parent, View view,
        int position, long id) {
        SmsData smsData = smsDataArrayList.get(position);
        final int rowIndex = smsData.getSmsRowIndex();
        AlertDialog alertBox = new AlertDialog.Builder(context)
            .setMessage(R.string.delete_confirmation_msg)
            .setPositiveButton(R.string.delete_button_text, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    deleteMessage(rowIndex);
                    dialog.dismiss();
                }
             })
             .setNegativeButton(R.string.cancel_button_text, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                }
             }).show();
        return true;
    }

    private void changeMessageStatus(int rowIndex){
        ContentValues contentValues = new ContentValues();
        contentValues.put(Sms.READ, 1);
        String where_update = Sms._ID + " = ?";
        String[] selectionArgs_update = {String.valueOf(rowIndex)};
        Uri single_uri = ContentUris.withAppendedId(Sms.Inbox.CONTENT_URI,rowIndex);
        cr.update(single_uri, contentValues, where_update,
            selectionArgs_update);
    }

    public void updateMsgstatusToSending(int rowIndex) {
        ContentValues contentValues = new ContentValues();
        contentValues.put(Sms.STATUS, context.getString(R.string.status_sending));
        String where_update = Sms._ID + " = ?";
        String[] selectionArgs_update = {String.valueOf(rowIndex)};
        Uri single_uri = ContentUris.withAppendedId(Sms.CONTENT_URI,rowIndex);
        cr.update(single_uri, contentValues, where_update,
            selectionArgs_update);
    }

    private void deleteMessage(int rowIndex){
        String where = Sms._ID + " = ?";
        String[] selectionArgs = {String.valueOf(rowIndex)};

        cr.delete(Sms.CONTENT_URI, where, selectionArgs);
    }

    class SmsListItemViewHolder {
        TextView msgFrom;
        TextView msgDateTime;
        TextView msgText;
        TextView msgStatus;
    }

    public int getCount() {
        return smsDataArrayList.size();
    }

    public View getView(int position, View convertView, ViewGroup parent) {

        final SmsListItemViewHolder viewHolder;
        LayoutInflater li = (LayoutInflater) context.getSystemService(
                Context.LAYOUT_INFLATER_SERVICE);

        if (convertView == null) {
            convertView = li.inflate(R.layout.sms_list_item, null);

            viewHolder = new SmsListItemViewHolder();

            viewHolder.msgFrom = (TextView) convertView.findViewById(R.id
                    .msgFrom);
            viewHolder.msgDateTime = (TextView) convertView.findViewById(R.id
                    .msgDateTime);
            viewHolder.msgText = (TextView) convertView.findViewById(R.id
                    .msgText);
            viewHolder.msgStatus = (TextView) convertView.findViewById(R.id
                    .msgStatus);

            convertView.setTag(viewHolder);
        } else {
            viewHolder = (SmsListItemViewHolder) convertView.getTag();
        }

        if (!smsDataArrayList.isEmpty()) {
            SmsData smsData = smsDataArrayList.get(position);

            viewHolder.msgFrom.setText(smsData.getSmsFrom());
            viewHolder.msgDateTime.setText(smsData.getSmsDateTime());
            viewHolder.msgText.setText(smsData.getSmsBody());
            viewHolder.msgStatus.setText(smsData.getSmsReadStatus());
        }

        return convertView;
    }
}
