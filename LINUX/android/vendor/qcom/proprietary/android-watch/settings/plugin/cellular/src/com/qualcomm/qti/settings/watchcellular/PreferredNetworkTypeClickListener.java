/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchcellular;

import android.content.Context;
import android.content.ContentResolver;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.Toast;

import java.util.Arrays;
import java.util.List;

import com.android.internal.telephony.Phone;

public class PreferredNetworkTypeClickListener
    implements AdapterView.OnItemClickListener {
        private static final String APP_TAG = "PreferredNetworkType";
        public static final int EVENT_QUERY_GET_PREF_NETWORK_DONE = 100;
        private static final int EVENT_SET_PREF_NETWORK_DONE = 101;
        private static final int PREF_NET_TYPE_LTE_ONLY = 11; // LTE only
        private static final int PREF_NET_TYPE_LTE_WCDMA = 12; // LTE/WCDMA
        private static final int PREF_NET_TYPE_LTE_GSM_WCDMA = 9; //LTE,GSM/WCDMA
        private static final int PREF_NET_TYPE_WCDMA = 2; // WCDMA
        private static final int PREF_NET_TYPE_GSM_WCDMA = 0; // GSM/WCDMA(pref)
        private static final int PREF_NET_TYPE_GSM_ONLY = 1; // GSM only
        private static final int PREF_NET_TYPE_CDMA_ONLY = 5; // CDMA only
        private static final int PREF_NET_TYPE_LTE_CDMA_EVDO = 8; // LTE, CDMA and EvDo
        private final Context context;
        private final ListView listView;
        private final Phone phone;
        private Message msg;
        private int pref_network_type;

        List<Integer> preferredNetworkLabels = Arrays.asList(
            PREF_NET_TYPE_LTE_ONLY,
            PREF_NET_TYPE_LTE_WCDMA,
            PREF_NET_TYPE_LTE_GSM_WCDMA,
            PREF_NET_TYPE_LTE_CDMA_EVDO,
            PREF_NET_TYPE_WCDMA,
            PREF_NET_TYPE_GSM_WCDMA,
            PREF_NET_TYPE_GSM_ONLY,
            PREF_NET_TYPE_CDMA_ONLY
        );

        public PreferredNetworkTypeClickListener (Context context,
            ListView listView, Phone phone) {
                this.context = context;
                this.listView = listView;
                this.phone = phone;
        }

        public void setPreferredNetworkMode(ContentResolver cr,
            int networkMode, Message msg) {
                final int phoneSubId = phone.getSubId();
                Log.v(APP_TAG, "setPreferredNetworkMode: " + networkMode +
                        "phoneSubId: " + phoneSubId);
                android.provider.Settings.Global.putInt(cr,
                    android.provider.Settings.Global.PREFERRED_NETWORK_MODE + phoneSubId,
                        networkMode);
                phone.setPreferredNetworkType(networkMode, msg);
        }

        public void onItemClick(AdapterView<?> adapterView, View view,
            int position, long id) {
            listView.setClickable(false);

            pref_network_type = preferredNetworkLabels.get(position);

            msg = handler.obtainMessage(EVENT_SET_PREF_NETWORK_DONE);
            setPreferredNetworkMode(phone.getContext().getContentResolver(),
                pref_network_type, msg);
        }

        public Handler handler = new Handler() {
            public void handleMessage(Message msg) {
                AsyncResult asyncRes;

                switch (msg.what) {
                    case EVENT_QUERY_GET_PREF_NETWORK_DONE:
                        asyncRes = (AsyncResult) msg.obj;

                        if (asyncRes.exception == null) {
                            int networkType = ((int[]) asyncRes.result)[0];

                            if (preferredNetworkLabels.contains(networkType)) {
                                int index = preferredNetworkLabels.indexOf(
                                    networkType);
                                listView.setSelection(index);
                                listView.setItemChecked(index, true);

                                Log.v(APP_TAG,
                                    "Network mode set in the system : "
                                        + networkType);
                            } else {
                                Toast.makeText(context,
                                    context.getString(R.string.unknown) +
                                        networkType, Toast.LENGTH_SHORT).show();
                                Log.v(APP_TAG,
                                    "Unknown network mode set in the system: " +
                                        networkType);
                            }
                        } else {
                            Toast.makeText(context,
                                context.getString(R.string.get_exception),
                                    Toast.LENGTH_SHORT).show();
                            Log.d(APP_TAG,
                                "Couldn't get network type set in the system");
                        }
                        break;
                    case EVENT_SET_PREF_NETWORK_DONE:
                        asyncRes = (AsyncResult) msg.obj;
                        if (asyncRes.exception == null) {
                            Log.d(APP_TAG,
                                "Successfully set preferred network type");
                        } else if (asyncRes.exception != null) {
                            phone.getPreferredNetworkType(obtainMessage(
                                EVENT_QUERY_GET_PREF_NETWORK_DONE));
                            Toast.makeText(context,
                                context.getString(R.string.set_exception),
                                    Toast.LENGTH_SHORT).show();
                            Log.d(APP_TAG,
                                "Couldn't set preferred network type");
                        }
                        listView.setClickable(true);
                        break;
                }
            }
        };
}
