package com.qualcomm.qti.settings.watchcellular;

/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

import android.content.Context;
import android.content.Intent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.TextView;
import android.widget.AdapterView.OnItemClickListener;

public class CellularDataClickListener  implements
AdapterView.OnItemClickListener {
    private final Context context;
    private final String[] cellularDataOptions;
    private CellularDataStatus cellularDataStatus;
    private boolean bBlocked = false;
    private ArrayAdapter adapter = null;

    public CellularDataClickListener(Context context,
        String[] cellularDataOptions, CellularDataStatus cellularDataStatus,
        ArrayAdapter adapter) {
            this.context = context;
            this.cellularDataOptions = cellularDataOptions;
            this.cellularDataStatus = cellularDataStatus;
            this.adapter = adapter;
    }

    public void onItemClick(AdapterView<?> adapterView, View view,
        int position, long id) {

            Intent intent;

            if(bBlocked) {
                return;
            }

            bBlocked = true;
            TextView textView = (TextView) view.findViewWithTag(CellularDataArrayAdapter.DATA_STATE_TAG);
            if (cellularDataOptions[position].equals(context.getString(
                R.string.sim_data_enable))) {
                //toggle data enable/disable
                cellularDataStatus.setCellulluladData(!cellularDataStatus.isCellularDataEnabled());
                adapter.notifyDataSetChanged();
            } else if (cellularDataOptions[position].equals(context.getString(
                R.string.sim_data_roaming))) {
                //toggle data roaming enable/disable
                cellularDataStatus.setCellulluladDataRoaming(!cellularDataStatus.isCellularDataRoamingEnabled());
                adapter.notifyDataSetChanged();
            }
            bBlocked = false;
    }
}
