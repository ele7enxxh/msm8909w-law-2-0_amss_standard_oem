/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchdatetime;

import android.app.Activity;
import android.app.AlarmManager;
import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

import java.util.Calendar;
import java.util.TimeZone;

public class TimeZoneSettings extends Activity {
    private final String APP_TAG = "TimeZoneSettings";
    private ArrayAdapter adapter;
    private TextView mTimeZoneInfo;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_timezone);
        mTimeZoneInfo = (TextView) findViewById(R.id.timeZone);
        setupListView();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mTimeZoneInfo.setText("TimeZone Set: " +
            TimeZone.getDefault().getID().toString());
    }

    //Set up the list view and populate all timezone menu items
    private void setupListView() {
        final String[] rowTags = getResources().getStringArray(R.array.timezones);
        final int mTimeZoneCount = rowTags.length;
        final String[] rowValues = new String[mTimeZoneCount];

        //Polulate GMT offset for all timezones
        for(int i=0; i < mTimeZoneCount; i++) {
            rowValues[i] = getGmtOffset(rowTags[i]);
        }

        class ViewHolder {
            TextView text1;
            TextView text2;
        }

        adapter = new ArrayAdapter(this,
                android.R.layout.simple_expandable_list_item_2,
                android.R.id.text1, rowTags) {

            @Override
            public View getView(int position, View convertView,
                                ViewGroup parent) {
                ViewHolder viewHolder;
                LayoutInflater li = (LayoutInflater)
                    getSystemService(Context.LAYOUT_INFLATER_SERVICE);

                if (convertView == null) {
                    convertView = li.inflate(android.R.layout
                        .simple_expandable_list_item_2, null);
                    viewHolder = new ViewHolder();
                    viewHolder.text1 = (TextView) convertView.findViewById(
                        android.R.id.text1);
                    viewHolder.text2 = (TextView) convertView.findViewById(
                        android.R.id.text2);

                    /**
                      * To increase performance set this in the convertView
                      * so that can be reused
                      */
                    convertView.setTag(viewHolder);
                } else {
                    // View was already created, so re-use
                    viewHolder = (ViewHolder) convertView.getTag();
                }

                viewHolder.text1.setText(rowTags[position]);
                viewHolder.text2.setText(rowValues[position]);
                return convertView;
            }

            @Override
            public int getCount() {
                return rowTags.length;
            }
        };

        final ListView listView = (ListView) findViewById(
                R.id.listViewTimeZoneSettings);
        listView.setAdapter(adapter);

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                                    int position, long id) {
                //Set default timezone
                setTimeZone(rowTags[position]);
                finish();
            }
        });
    }

    //Calculate GMT offset for each timezone
    private String getGmtOffset(String mTimeZoneId) {
        TimeZone mTimeZone = TimeZone.getTimeZone(mTimeZoneId);
        Calendar calendar = Calendar.getInstance(mTimeZone);
        int mOffsetInMillis = mTimeZone.getOffset(calendar.getTimeInMillis());

        //Calculate offset in hh:min format
        String mOffset = String.format("%02d:%02d",
            Math.abs(mOffsetInMillis / 3600000),
            Math.abs((mOffsetInMillis / 60000) % 60));
        mOffset = "GMT" + (mOffsetInMillis >= 0 ? "+" : "-") + mOffset;

        return mOffset;
    }

    //Set user chosen timezone as the default timezone
    private void setTimeZone(String mTimeZoneId) {
        try {
            ((AlarmManager) getSystemService(Context.ALARM_SERVICE))
                .setTimeZone(mTimeZoneId);
            mTimeZoneInfo.setText("TimeZone Set: " + mTimeZoneId);
            Log.i(APP_TAG, "New timezone that's set : " + mTimeZoneId);
        } catch (SecurityException se) {
            se.printStackTrace();
            Log.i(APP_TAG,
                "Failed to set new timezone due to security reasons");
        }
    }
}
