/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchcellular;

import android.content.Context;
import android.telephony.PhoneNumberUtils;
import android.telephony.ServiceState;
import android.telephony.SignalStrength;
import android.telephony.SubscriptionInfo;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import java.util.Arrays;
import java.util.List;

import com.android.internal.telephony.DefaultPhoneNotifier;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;

public class SimStatusArrayAdapter extends ArrayAdapter<String> {
    private final Context context;
    private final List<String> simStatusRowTags;
    private List<String> simStatusRowValues;
    private TelephonyManager telephonyManager;
    private SubscriptionInfo subscriptionInfo;
    private Phone phone;
    private int index;

    public SimStatusArrayAdapter(Context context, List<String> simStatusRowTags,
        List<String> simStatusRowValues, TelephonyManager telephonyManager,
            SubscriptionInfo subscriptionInfo, Phone phone) {

        super(context, R.layout.sim_status_list_item, simStatusRowTags);

        this.context = context;
        this.simStatusRowTags = simStatusRowTags;
        this.simStatusRowValues = simStatusRowValues;
        this.telephonyManager = telephonyManager;
        this.subscriptionInfo = subscriptionInfo;
        this.phone = phone;
    }

    class SimStatusListItemViewHolder {
        TextView text1;
        TextView text2;
    }

    public int getCount() {
        return simStatusRowTags.size();
    }

    public View getView(int position, View convertView, ViewGroup parent) {
        SimStatusListItemViewHolder viewHolder;

        LayoutInflater li = (LayoutInflater) context.getSystemService(
                Context.LAYOUT_INFLATER_SERVICE);

        if (convertView == null) {
            convertView = li.inflate(R.layout.sim_status_list_item, null);

            viewHolder = new SimStatusListItemViewHolder();
            viewHolder.text1 = (TextView) convertView.findViewById(R.id.text1);
            viewHolder.text2 = (TextView) convertView.findViewById(R.id.text2);

            convertView.setTag(viewHolder);
        } else {
            viewHolder = (SimStatusListItemViewHolder)
                    convertView.getTag();
        }

        viewHolder.text1.setText(simStatusRowTags.get(position));
        viewHolder.text2.setText(simStatusRowValues.get(position));

        return convertView;
    }

    public void refreshSimStatus(Phone phone){
        updateSignalStrength(phone.getSignalStrength());
        updateServiceState(phone.getServiceState());
        updateDataState();
        updateNetworkType();
        updatePhoneNumberImei();
    }

    private void updateSimStatus(int index, String update) {
        simStatusRowValues.set(index, update);
        notifyDataSetChanged();
    }

    private void updateSignalValue(String signalValue) {
        index = simStatusRowTags.indexOf(
            context.getString(R.string.signal_strength));
        updateSimStatus(index, signalValue);
    }

    public void updatePhoneNumberImei() {
        if ((phone != null) && (telephonyManager != null)) {

            String rawPhoneNumber = telephonyManager.
                getLine1Number(
                    subscriptionInfo.getSubscriptionId());
            String formattedPhoneNumber = context.getString(
                R.string.sim_status_unknown);

            if (!TextUtils.isEmpty(rawPhoneNumber)) {
                formattedPhoneNumber = PhoneNumberUtils.formatNumber(
                    rawPhoneNumber);
                index = simStatusRowTags.indexOf(
                    context.getString(R.string.phone_num));
                updateSimStatus(index, formattedPhoneNumber);
            }

            index = simStatusRowTags.indexOf(
                context.getString(R.string.imei));
            updateSimStatus(index, phone.getImei());

            index = simStatusRowTags.indexOf(
                context.getString(R.string.imei_sv));
            updateSimStatus(index, phone.getDeviceSvn());
        }
    }

    public void updateDataState(){
        if (phone != null) {
            final int state = DefaultPhoneNotifier.convertDataState(
                phone.getDataConnectionState());
            String dataState = context.getString(R.string.sim_status_unknown);

            switch (state) {
                case TelephonyManager.DATA_CONNECTED:
                    dataState = context.getString(R.string.data_connected);
                    break;
                case TelephonyManager.DATA_SUSPENDED:
                    dataState = context.getString(R.string.data_suspended);
                    break;
                case TelephonyManager.DATA_CONNECTING:
                    dataState = context.getString(R.string.data_connecting);
                    break;
                case TelephonyManager.DATA_DISCONNECTED:
                    dataState = context.getString(R.string.data_disconnected);
                    break;
            }

            index = simStatusRowTags.indexOf(
                context.getString(R.string.cellular_network_state));
            updateSimStatus(index, dataState);
        }
    }

    public void updateNetworkType(){
        if ((phone != null) && (subscriptionInfo != null) &&
            (telephonyManager != null)) {

            String networkType = context.getString(R.string.sim_status_unknown);
            final int subId = subscriptionInfo.getSubscriptionId();

            final int setDataNetworkType = telephonyManager.
                getDataNetworkType(subId);
            final int setVoiceNetworkType = telephonyManager.
                getVoiceNetworkType(subId);

            if (TelephonyManager.NETWORK_TYPE_UNKNOWN !=
                setDataNetworkType) {

                networkType = telephonyManager.getNetworkTypeName(
                    setDataNetworkType);

            } else if (TelephonyManager.NETWORK_TYPE_UNKNOWN !=
                setVoiceNetworkType) {

                networkType = telephonyManager.getNetworkTypeName(
                    setVoiceNetworkType);

            }

            index = simStatusRowTags.indexOf(
                context.getString(R.string.cellular_network_type));
            updateSimStatus(index, networkType);
        }
    }

    public void updateSignalStrength(SignalStrength signalStrength){
        if (phone != null) {
            final int serviceState = phone.getServiceState().
                getState();
            final int dataRegState = phone.getServiceState().
                getDataRegState();
            String signalValue;

            if (((ServiceState.STATE_OUT_OF_SERVICE == serviceState) &&
                (ServiceState.STATE_OUT_OF_SERVICE == dataRegState)) ||
                (ServiceState.STATE_POWER_OFF == serviceState)) {

                updateSignalValue("0");
                return;

            }

            int signalDbm = signalStrength.getDbm();
            int signalAsu = signalStrength.getAsuLevel();

            if (-1 == signalDbm) {
                signalDbm = 0;
            }

            if (-1 == signalAsu) {
                signalAsu = 0;
            }

            signalValue = String.valueOf(signalDbm) +
                context.getString(R.string.dBm) + " " +
                    String.valueOf(signalAsu) +
                        context.getString(R.string.asu);
            updateSignalValue(signalValue);
        }
    }

    public void updateServiceState(ServiceState serviceState){
        if (phone != null) {
            final int state = serviceState.getState();
            final int dataRegState = phone.getServiceState().getDataRegState();
            String voiceState = context.getString(R.string.sim_status_unknown);
            String dataState = context.getString(R.string.sim_status_unknown);
            String serviceStateUpdate;

            switch (state) {
                case ServiceState.STATE_OUT_OF_SERVICE:
                    /** Set signal strength to 0 when service state is
                      * STATE_OUT_OF_SERVICE
                      */
                    if (ServiceState.STATE_OUT_OF_SERVICE == dataRegState) {
                        updateSignalValue("0");
                    }
                    voiceState = dataState = context.getString(
                        R.string.service_out);
                    break;

                case ServiceState.STATE_POWER_OFF:
                    /** Set signal strength to 0 when service state is
                      * STATE_POWER_OFF
                      */
                    updateSignalValue("0");
                    voiceState = dataState = context.getString(
                        R.string.service_off);
                    break;

                case ServiceState.STATE_EMERGENCY_ONLY:
                    voiceState = dataState = context.getString(
                        R.string.service_out);

                case ServiceState.STATE_IN_SERVICE:
                    voiceState = dataState = context.getString(
                        R.string.service_in);
            }

            serviceStateUpdate = "Voice : " + voiceState +
                "\nData  : " + dataState;
            index = simStatusRowTags.indexOf(
                context.getString(R.string.service_state));
            updateSimStatus(index, serviceStateUpdate);

            if (serviceState.getRoaming()) {
                index = simStatusRowTags.indexOf(
                    context.getString(R.string.roaming));
                updateSimStatus(index, context.getString(
                    R.string.roaming_state));
            } else {
                index = simStatusRowTags.indexOf(
                    context.getString(R.string.roaming));
                updateSimStatus(index, context.getString(
                    R.string.not_roaming_state));
            }
            if (serviceState.getOperatorAlphaLong() != null) {
                index = simStatusRowTags.indexOf(
                    context.getString(R.string.network));
                updateSimStatus(index, serviceState.getOperatorAlphaLong());
            }
        }
    }
}
