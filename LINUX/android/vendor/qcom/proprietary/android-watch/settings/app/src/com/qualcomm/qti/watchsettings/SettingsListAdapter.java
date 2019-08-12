/**
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchsettings;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

public class SettingsListAdapter extends ArrayAdapter<String> {
    private final Context context;
    private final String[] listItemNames;

    class SettingsListItemViewHolder {
        TextView text;
    }

    public SettingsListAdapter(Context context, String[] values) {
        super(context, R.layout.settings_list_item, values);
        this.context = context;
        this.listItemNames = values;
    }

    @Override
    public int getCount() {

        return listItemNames.length;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {

        SettingsListItemViewHolder viewHolder;
        LayoutInflater li = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        if (convertView == null) {
            convertView = li.inflate(R.layout.settings_list_item, null);
            viewHolder = new SettingsListItemViewHolder();
            viewHolder.text = (TextView) convertView.findViewById(R.id.textViewListItem);

            // to increase performance set this in the convertView so that can be reused
            convertView.setTag(viewHolder);
        } else {
            // it was already created, so re-use
            viewHolder = (SettingsListItemViewHolder) convertView.getTag();
        }

        viewHolder.text.setText(listItemNames[position]);

        return convertView;
    }
}
