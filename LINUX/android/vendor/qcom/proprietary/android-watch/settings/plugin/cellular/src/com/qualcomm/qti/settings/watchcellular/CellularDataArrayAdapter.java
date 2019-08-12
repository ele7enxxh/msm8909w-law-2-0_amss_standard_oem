/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchcellular;

import android.content.Context;
import android.database.DataSetObserver;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ListAdapter;
import android.widget.TextView;

public class CellularDataArrayAdapter extends ArrayAdapter<String> {

    private Context context;
    private String[] cellularDataOptions;
    private CellularDataStatus cellularDataStatus;
    public final static String DATA_STATE_TAG = "DATA_STATE";

    private class CellularDataListItemViewHolder {
        TextView text1;
        TextView text2;
    }

    public CellularDataArrayAdapter(Context context,
            String[] cellularDataOptions, CellularDataStatus cellularDataStatus) {
        super(context, R.layout.cellular_data_list_item, cellularDataOptions);
        this.context = context;
        this.cellularDataOptions = cellularDataOptions;
        this.cellularDataStatus = cellularDataStatus;
    }

    public int getCount() {
        return cellularDataOptions.length;
    }

    public View getView(int position, View convertView,
            ViewGroup parent) {
        CellularDataListItemViewHolder viewHolder;

        LayoutInflater li = (LayoutInflater) context.getSystemService(
                Context.LAYOUT_INFLATER_SERVICE);

        if (convertView == null) {
            convertView = li.inflate(R.layout.cellular_data_list_item, null);

            viewHolder = new CellularDataListItemViewHolder();
            viewHolder.text1 = (TextView) convertView.findViewById(R.id.text1);
            viewHolder.text2 = (TextView) convertView.findViewById(R.id.text2);
            viewHolder.text2.setTag(DATA_STATE_TAG);

            convertView.setTag(viewHolder);
        } else {
            viewHolder = (CellularDataListItemViewHolder)
                    convertView.getTag();
        }

        viewHolder.text1.setText(cellularDataOptions[position]);

        if(cellularDataOptions[position].equals(
                context.getString(R.string.sim_data_enable))) {

            cellularDataStatus.setCellularDataStatusText(viewHolder.text2,
                    cellularDataStatus.isCellularDataEnabled());

        }else if(cellularDataOptions[position].equals(
                context.getString(R.string.sim_data_roaming))) {

            cellularDataStatus.setCellularDataStatusText(viewHolder.text2,
                    cellularDataStatus.isCellularDataRoamingEnabled());
        }

        return convertView;
    }
}
