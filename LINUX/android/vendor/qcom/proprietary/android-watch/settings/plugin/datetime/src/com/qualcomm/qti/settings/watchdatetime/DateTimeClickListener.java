/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchdatetime;

import android.app.Activity;
import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.TextView;
import android.widget.Toast;


public class DateTimeClickListener  implements
             AdapterView.OnItemClickListener {
    private Context context;
    private final String[] datetimeOptions;
    private boolean bBlocked = false;
    private ArrayAdapter adapter = null;
    private DateTimeData dt = null;
    private final String TAG = "DateTime";

    public DateTimeClickListener(Context context,
        String[] datetimeOptions, ArrayAdapter adapter,
        DateTimeData dt) {
            this.context = context;
            this.datetimeOptions = datetimeOptions;
            this.adapter = adapter;
            this.dt = dt;
    }

    public void onItemClick(AdapterView<?> adapterView, View view,
        int position, long id) {
        TextView textView = (TextView) view.findViewWithTag(DateTimeArrayAdapter.DATE_STATE_TAG);
        int item = position;
        Activity activity = (DateTimeSettings) context;
        Intent intent;
	if (datetimeOptions[position].equals(context.getString(
                    R.string.auto_date))) {
            try{
                if (dt.getAutoDateTime(Settings.Global.AUTO_TIME, context)) {
                    dt.setAutoDateTime(0, context);
                }else{
                    dt.setAutoDateTime(1, context);
                }
                adapter.notifyDataSetChanged();
            }catch (Exception e) {
                Log.d("__", "Failed to enable auto date and time");
                    e.printStackTrace();
            }
        }
        if (datetimeOptions[position].equals(context.getString(
                    R.string.auto_timezone))) {
            try{
                if (dt.getAutoDateTime(Settings.Global.AUTO_TIME_ZONE, context)) {
                    dt.setAutoTimeZone(0,context);
                }else {
                    dt.setAutoTimeZone(1,context);
                }
                adapter.notifyDataSetChanged();
            }catch (Exception e) {
                Log.d("__", "Failed to set auto timezone");
                    e.printStackTrace();
            }
        }
        if (datetimeOptions[position].equals(context.getString(
                    R.string.set_date))) {
            if(!(dt.getAutoDateTime(Settings.Global.AUTO_TIME, context))){
                intent = new Intent(this.context, DatePickerActivity.class);
                context.startActivity(intent);
                activity.finish(); //Fixme: Finishing the activity
            }else{
                Toast.makeText(context,"disabled",Toast.LENGTH_SHORT).show();
            }
        }
        if (datetimeOptions[position].equals(context.getString(
                    R.string.set_time))) {
           if(!(dt.getAutoDateTime(Settings.Global.AUTO_TIME, context))){
               intent = new Intent(this.context, TimePickerActivity.class);
               context.startActivity(intent);
               activity.finish(); //Fixme: Finishing the activity
            }else{
              Toast.makeText(context,"disabled",Toast.LENGTH_SHORT).show();
            }
        }
        if (datetimeOptions[position].equals(context.getString(
                    R.string.set_timezone))) {
           if(!(dt.getAutoDateTime(Settings.Global.AUTO_TIME_ZONE, context))){
                intent = new Intent(this.context, TimeZoneSettings.class);
                context.startActivity(intent);
                activity.finish(); //Fixme: Finishing the activity
            }else{
              Toast.makeText(context,"disabled",Toast.LENGTH_SHORT).show();
            }
        }
    }
}
