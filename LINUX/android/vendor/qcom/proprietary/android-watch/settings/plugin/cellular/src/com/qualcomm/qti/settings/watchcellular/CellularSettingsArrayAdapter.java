/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchcellular;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

public class CellularSettingsArrayAdapter extends ArrayAdapter<String> {
    private final Context context;
    private final String[] cellularOptions;

    class CellularSettingsListItemViewHolder {
        TextView text;
    }

    public CellularSettingsArrayAdapter(Context context,
        String[] cellularOptions) {
            super(context, R.layout.cellular_settings_list_item, cellularOptions);
            this.context = context;
            this.cellularOptions = cellularOptions;
    }

    public int getCount() {
        return cellularOptions.length;
    }

    public View getView(int position, View convertView, ViewGroup parent) {
        CellularSettingsListItemViewHolder viewHolder;
        LayoutInflater li = (LayoutInflater) context.getSystemService(
            Context.LAYOUT_INFLATER_SERVICE);

        if (convertView == null) {
            convertView = li.inflate(R.layout.cellular_settings_list_item, null);
            viewHolder = new CellularSettingsListItemViewHolder();
            viewHolder.text = (TextView) convertView.findViewById(
                R.id.textViewListItem);

            /* to increase performance set this in the convertView
             * so that it can be reused
             */
            convertView.setTag(viewHolder);
        } else {
            // view was already created, so re-use
            viewHolder = (CellularSettingsListItemViewHolder)
                convertView.getTag();
        }

        viewHolder.text.setText(cellularOptions[position]);
        return convertView;
    }
}
