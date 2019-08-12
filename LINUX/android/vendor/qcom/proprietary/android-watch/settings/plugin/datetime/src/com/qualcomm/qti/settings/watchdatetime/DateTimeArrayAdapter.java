/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchdatetime;


import java.util.Calendar;
import java.util.TimeZone;

import android.content.Context;
import android.graphics.Color;
import android.provider.Settings;
import android.text.format.DateFormat;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;
import android.widget.RelativeLayout;

public class DateTimeArrayAdapter extends ArrayAdapter<String> {

    private final String TAG = "DateTime";
    private Context context;
    private String[] dateTimeOptions;
    public final static String DATE_STATE_TAG = "DATE_STATE";
    private DateTimeData dt ;
    final Calendar now = Calendar.getInstance();

    private class DateTimeListItemViewHolder {
        RelativeLayout parent;
        TextView text1;
        TextView text2;
    }

    public DateTimeArrayAdapter(Context context,
            String[] dateTimeOptions, DateTimeData dt) {
        super(context, R.layout.datetime_list_item, dateTimeOptions);
        this.context = context;
        this.dateTimeOptions = dateTimeOptions;
        this.dt = dt;
    }

    public int getCount() {
        return dateTimeOptions.length;
    }

    public View getView(int position, View convertView,
            ViewGroup parent) {
        DateTimeListItemViewHolder viewHolder;

        LayoutInflater li = (LayoutInflater) context.getSystemService(
                Context.LAYOUT_INFLATER_SERVICE);

        if (convertView == null) {
            convertView = li.inflate(R.layout.datetime_list_item, null);
            viewHolder = new DateTimeListItemViewHolder();

            viewHolder.text1 = (TextView) convertView.findViewById(R.id.text1);
            viewHolder.text2 = (TextView) convertView.findViewById(R.id.text2);
            viewHolder.text2.setTag(DATE_STATE_TAG);
            viewHolder.parent = (RelativeLayout)convertView.findViewById(R.id.datetime_parent);
            convertView.setTag(viewHolder);
        } else {
            viewHolder = (DateTimeListItemViewHolder)
            convertView.getTag();
        }
        viewHolder.text1.setText(dateTimeOptions[position]);
        if(dateTimeOptions[position].equalsIgnoreCase(context.getString(R.string.auto_date))) {
            if(dt.getAutoDateTime(Settings.Global.AUTO_TIME, context)) {
                viewHolder.text2.setText(R.string.enabled);
            }else {
                viewHolder.text2.setText(R.string.disabled);
            }
        }
        if(dateTimeOptions[position].equalsIgnoreCase(context.getString(R.string.auto_timezone))) {
            if(dt.getAutoDateTime(Settings.Global.AUTO_TIME_ZONE, context)) {
                viewHolder.text2.setText(R.string.enabled);
            }else {
                viewHolder.text2.setText(R.string.disabled);
            }
        }
        if(dateTimeOptions[position].equalsIgnoreCase(context.getString(R.string.set_date))){
                viewHolder.text2.setText(DateFormat.getLongDateFormat(context).format(now.getTime()));
        }
        if(dateTimeOptions[position].equalsIgnoreCase(context.getString(R.string.set_time))){
                viewHolder.text2.setText(DateFormat.getTimeFormat(context).format(now.getTime()));
        }
        if(dateTimeOptions[position].equalsIgnoreCase(context.getString(R.string.set_timezone))){
            if(dt.getAutoDateTime(Settings.Global.AUTO_TIME_ZONE, context)) {
                viewHolder.text2.setText(DateTimeData.getGmtOffset(TimeZone.getDefault().getID().toString()));
            }else{
                viewHolder.text2.setText(DateTimeData.getGmtOffset(TimeZone.getDefault().getID().toString()));
            }
        }
        return convertView;
    }
}
