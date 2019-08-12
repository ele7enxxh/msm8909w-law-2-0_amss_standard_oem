/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchcellular;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.telephony.PhoneStateListener;
import android.telephony.ServiceState;
import android.telephony.SignalStrength;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.widget.ListView;
import android.widget.TextView;

import java.util.Arrays;
import java.util.List;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;

public class SimStatus extends Activity {
    private static final String APP_TAG = "SimStatus";
    private TextView mTextView;
    private TelephonyManager telephonyManager;
    private SubscriptionManager subscriptionManager;
    private List<SubscriptionInfo> subscriptionInfoList;
    private SubscriptionInfo subscriptionInfo;
    private Phone phone = null;
    private PhoneStateListener phoneStateListener;
    private SimStatusArrayAdapter simStatusArrayAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sim_status);

        final List<String> simStatusRowTags = Arrays.asList(
                getString(R.string.network),
                getString(R.string.signal_strength),
                getString(R.string.cellular_network_type),
                getString(R.string.service_state),
                getString(R.string.roaming),
                getString(R.string.cellular_network_state),
                getString(R.string.phone_num),
                getString(R.string.imei),
                getString(R.string.imei_sv)
        );

        List<String> simStatusRowValues = Arrays.asList(
            getString(R.string.sim_status_unknown),
            getString(R.string.sim_status_unknown),
            getString(R.string.sim_status_unknown),
            getString(R.string.sim_status_unknown),
            getString(R.string.sim_status_unknown),
            getString(R.string.sim_status_unknown),
            getString(R.string.sim_status_unknown),
            getString(R.string.sim_status_unknown),
            getString(R.string.sim_status_unknown)
        );

        ListView listView = (ListView) findViewById(R.id.listViewSimStatus);

        telephonyManager = (TelephonyManager) getSystemService
            (Context.TELEPHONY_SERVICE);

        getSubscriptionInfo();

        setUpPhone();

        simStatusArrayAdapter = new SimStatusArrayAdapter(this,
            simStatusRowTags, simStatusRowValues, telephonyManager,
                subscriptionInfo, phone);
        listView.setAdapter(simStatusArrayAdapter);

        setUpPhoneListener();
    }

    private void getSubscriptionInfo(){
        subscriptionManager = SubscriptionManager.from(this);
        subscriptionInfoList = subscriptionManager.
            getActiveSubscriptionInfoList();

        if (subscriptionInfoList == null) {
            subscriptionInfo = null;
            Log.d(APP_TAG, "subscriptionInfo is null!");
        } else {
            subscriptionInfo = subscriptionInfoList.size() > 0 ?
                subscriptionInfoList.get(0) : null;
        }
    }

    private void setUpPhone() {
        if ( subscriptionInfo != null) {
            phone = PhoneFactory.getPhone(SubscriptionManager.getPhoneId(
                subscriptionInfo.getSubscriptionId()));
        }
    }

    private void setUpPhoneListener() {
        if ( subscriptionInfo != null) {
            phoneStateListener = new PhoneStateListener(){
                @Override
                public void onDataConnectionStateChanged(int state) {
                    simStatusArrayAdapter.updateDataState();
                    simStatusArrayAdapter.updateNetworkType();
                }

                @Override
                public void onSignalStrengthsChanged(
                    SignalStrength signalStrength) {
                    simStatusArrayAdapter.updateSignalStrength(signalStrength);
                }

                @Override
                public void onServiceStateChanged(
                    ServiceState serviceState) {
                    simStatusArrayAdapter.updateServiceState(serviceState);
                }
            };
        }
    }

    @Override
    protected void onResume() {
        super.onResume();

        if (phone != null) {
            telephonyManager.listen(phoneStateListener,
                PhoneStateListener.LISTEN_DATA_CONNECTION_STATE
                    | PhoneStateListener.LISTEN_SIGNAL_STRENGTHS
                    | PhoneStateListener.LISTEN_SERVICE_STATE);

            simStatusArrayAdapter.refreshSimStatus(phone);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();

        if (phone != null) {
            telephonyManager.listen(phoneStateListener,
                PhoneStateListener.LISTEN_NONE);
        }
    }
}
