/**
 * Copyright (c) 2015-16 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchcellular;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.ListView;
import android.widget.TextView;
import android.os.SystemProperties;

public class CellularSettings extends Activity {
    private static final String TAG = "CellularSettings";

    CellularDataStatus cellularDataStatus;
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cellular);

        final String[] CellularOptions = {
                getString(R.string.preferred_network_type_title),
                getString(R.string.sim_status_activity_title),
                getString(R.string.cellular_data)
        };

        ListView listView = (ListView) findViewById(R.id.list_view);
        // layout for circuler display
        if(Utils.IsDisplayTypeCircular()) {
            TextView textView = (TextView)findViewById(R.id.textView);
            int paddingDp = getResources().getDimensionPixelOffset(R.dimen.circuler_padding);
            textView.setPadding(0, paddingDp, 0, 0);
        }

        listView.setAdapter(new CellularSettingsArrayAdapter(
            this, CellularOptions));
        listView.setOnItemClickListener(new CellularSettingsClickListener(
            this, CellularOptions));
    }
}
