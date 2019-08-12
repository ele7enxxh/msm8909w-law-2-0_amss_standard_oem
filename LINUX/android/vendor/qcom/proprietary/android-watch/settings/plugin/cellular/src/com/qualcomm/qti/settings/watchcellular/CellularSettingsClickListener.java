/**
 * Copyright (c) 2015-16 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchcellular;

import android.content.Context;
import android.content.Intent;
import android.view.View;
import android.widget.AdapterView;

public class CellularSettingsClickListener implements
    AdapterView.OnItemClickListener {
        private final Context context;
        private final String[] cellularOptions;

        public CellularSettingsClickListener(Context context,
            String[] cellularOptions) {
                this.context = context;
                this.cellularOptions = cellularOptions;
        }

        public void onItemClick(AdapterView<?> adapterView, View view,
            int position, long id) {

                Intent intent;

                if (cellularOptions[position].equals(context.getString(
                    R.string.preferred_network_type_title))) {
                        intent = new Intent(this.context,
                            PreferredNetworkType.class);
                        context.startActivity(intent);
                        return;
                } else if (cellularOptions[position].equals(context.getString(
                    R.string.sim_status_activity_title))) {
                        intent = new Intent(this.context,
                            SimStatus.class);
                        context.startActivity(intent);
                        return;
                }else if(cellularOptions[position].equals(context.getString(
                    R.string.cellular_data))) {
                        intent = new Intent(this.context,
                            CellularData.class);
                        context.startActivity(intent);
                }
        }
}
