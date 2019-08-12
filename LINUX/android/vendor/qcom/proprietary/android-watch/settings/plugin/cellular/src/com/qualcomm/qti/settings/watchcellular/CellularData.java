/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchcellular;

import android.app.Activity;
import android.os.Bundle;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;

public class CellularData extends Activity{
    private static final String TAG = "CellularData";

    private Phone mPhone = null;
    private ArrayAdapter adapter = null;
    CellularDataStatus cellularDataStatus;
    CellularDataListener dataListener = null;
    private TelephonyManager mTelephonyManager = null;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cellular_data);

        try {
            mPhone = PhoneFactory.getDefaultPhone();
        }catch (Exception ex) {
            Log.e(TAG, "Exception: " + ex.getMessage());
        }
        //null check is performed for mPhone in CellularDataStatus
        //if phone is null, data roaming status will not be fetched.
        cellularDataStatus = new CellularDataStatus(this, mPhone);

        final String[] CellularDataOptions = {
                getString(R.string.sim_data_enable),
                getString(R.string.sim_data_roaming)
        };

        ListView listView = (ListView) findViewById(R.id.listViewCellularData);

        adapter = new CellularDataArrayAdapter(this, CellularDataOptions,
                        cellularDataStatus);
        listView.setAdapter(adapter);
        listView.setOnItemClickListener(new CellularDataClickListener(
            this, CellularDataOptions, cellularDataStatus, adapter));

        mTelephonyManager = TelephonyManager.from(this);
        if(mTelephonyManager != null) {
            dataListener = new CellularDataListener();

            mTelephonyManager.listen(dataListener,
                    PhoneStateListener.LISTEN_DATA_CONNECTION_STATE);
        }
    }

    @Override
    protected void onDestroy() {
        if(mTelephonyManager != null) {
            mTelephonyManager.listen(dataListener,
                    PhoneStateListener.LISTEN_NONE);
    }
        super.onDestroy();
    }

    public class CellularDataListener extends PhoneStateListener {
        @Override
        public void onDataConnectionStateChanged(int state, int networkType) {
            adapter.notifyDataSetInvalidated();
            super.onDataConnectionStateChanged(state, networkType);
        }
    };

}
