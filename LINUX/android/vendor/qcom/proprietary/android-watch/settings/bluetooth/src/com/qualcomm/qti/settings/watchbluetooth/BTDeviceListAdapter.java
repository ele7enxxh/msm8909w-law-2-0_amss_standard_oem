/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchbluetooth;

import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

/**
 * This class is an adapter for the list view which shows paired, connected and scanned devices
 * This adapter uses the devices managed by BTCachedDeviceManager class to manage the list view
 */
public class BTDeviceListAdapter extends ArrayAdapter<String> {

    public static final String APP_TAG = "BTSettings";

    Context context;

    BTDeviceListAdapter(Context context) {

        super(context, android.R.layout.simple_list_item_1);
        this.context = context;
    }

    @Override
    public int getCount() {

        return BTCachedDeviceManager.getInstance().getCount();
    }

    @Override
    public long getItemId(int position) {

        return 0;
    }

    class DeviceListItemViewHolder {

        TextView text;
        TextView stateText;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {

        DeviceListItemViewHolder viewHolder;
        LayoutInflater li = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        if (convertView == null) {
            convertView = li.inflate(R.layout.scan_list_item, null);
            viewHolder = new DeviceListItemViewHolder();
            viewHolder.text = (TextView) convertView.findViewById(R.id.textViewScanListItem);
            viewHolder.stateText = (TextView) convertView.findViewById(R.id.textViewStateScanListItem);

            // to increase performance set this in the convertView so that can be reused
            convertView.setTag(viewHolder);
        } else {
            // it was already created, so re-use
            viewHolder = (DeviceListItemViewHolder) convertView.getTag();
        }

        BTCachedDevice thisCachedDevice = BTCachedDeviceManager.getInstance().getDevice(position);

        if (thisCachedDevice == null) {
            Log.e(APP_TAG, "Unexpectedly null device");
            return convertView;
        }

        BluetoothDevice thisDevice = thisCachedDevice.getDevice();
        if (thisDevice.getName() == null || thisDevice.getName().isEmpty()) {
            viewHolder.text.setText(thisDevice.getAddress());
        } else {
            viewHolder.text.setText(thisDevice.getName());
        }

        // if connected, show C in GREEN
        // if paired show P in BLUE
        // if neither paired nor connected, do not show state text
        viewHolder.stateText.setText(" ");
        int stateColor = context.getResources().getColor(R.color.FoundColor);

        if (thisCachedDevice.isConnected()) {
            stateColor = context.getResources().getColor(R.color.ConnectedColor);
            viewHolder.stateText.setText(context.getString(R.string.bt_device_state_connected));
            Log.v(APP_TAG, "Device is connected: " + thisDevice.getName());
        } else if (thisDevice.getBondState() == BluetoothDevice.BOND_BONDED) {
            stateColor = context.getResources().getColor(R.color.BondedColor);
            viewHolder.stateText.setText(context.getString(R.string.bt_device_state_paired));
            Log.v(APP_TAG, "Device is paired: " + thisDevice.getName());
        }

        Log.v(APP_TAG, "Set state text to: " + viewHolder.stateText.getText());
        viewHolder.stateText.setTextColor(stateColor);

        return convertView;
    }
}
