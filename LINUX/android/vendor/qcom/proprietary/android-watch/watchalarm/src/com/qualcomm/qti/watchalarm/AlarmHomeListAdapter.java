/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchalarm;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

public class AlarmHomeListAdapter extends ArrayAdapter<String> {

    private final Context context;
    private final String[] listItemNames;

    class AlarmHomeListItemViewHolder {
        TextView text;
    }

    public AlarmHomeListAdapter(Context context, String[] values) {
        super(context, R.layout.alarm_home_list_item, values);
        this.context = context;
        this.listItemNames = values;
    }

    @Override
    public int getCount() {

        return listItemNames.length;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {

        AlarmHomeListItemViewHolder viewHolder;
        LayoutInflater li = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        if (convertView == null) {
            convertView = li.inflate(R.layout.alarm_home_list_item, null);
            viewHolder = new AlarmHomeListItemViewHolder();
            viewHolder.text = (TextView) convertView.findViewById(R.id.textViewListItem);

            // to increase performance set this in the convertView so that can be reused
            convertView.setTag(viewHolder);
        } else {
            // it was already created, so re-use
            viewHolder = (AlarmHomeListItemViewHolder) convertView.getTag();
        }

        viewHolder.text.setText(listItemNames[position]);

        return convertView;
    }
}
