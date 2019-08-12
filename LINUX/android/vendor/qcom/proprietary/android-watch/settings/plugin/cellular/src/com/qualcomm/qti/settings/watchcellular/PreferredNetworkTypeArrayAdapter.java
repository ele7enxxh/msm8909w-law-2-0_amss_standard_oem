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

public class PreferredNetworkTypeArrayAdapter extends ArrayAdapter<String> {
    private final Context context;
    private final String[] networkModes;

    public PreferredNetworkTypeArrayAdapter (Context context,
        String[] networkModes) {
            super(context, android.R.layout.simple_list_item_single_choice,
                android.R.id.text1, networkModes);
            this.context = context;
            this.networkModes = networkModes;
    }

    public int getCount() {
        return networkModes.length;
    }

    public View getView(int position, View convertView,
                        ViewGroup parent) {
        View view = super.getView(position, convertView, parent);
        TextView value = (TextView) view.findViewById(
                android.R.id.text1);
        value.setText(networkModes[position]);
        return view;
    }
}
