/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchbluetooth;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;

public class BTBroadcastEventHandler extends BroadcastReceiver {

    private static final String APP_TAG = "BTSettings";

    private BluetoothAdapter btAdapter;
    private BTActivity btActivity;

    public BTBroadcastEventHandler(BTActivity btActivity, BluetoothAdapter btAdapter) {
        this.btAdapter = btAdapter;
        this.btActivity = btActivity;
    }

    @Override
    public void onReceive(Context context, Intent intent) {

        String action = intent.getAction();

        Log.v(APP_TAG, "Broadcast Event Received: " + action);

        // BT Enable Disable state change handling
        if (handleBtStateChange(action)) {
            return;
        }

        if (handleScanResult(action, intent)) {
            return;
        }

        if (handleScanModeChange(action)) {
            return;
        }

        if (handleDiscoveryModeChange(action)) {
            return;
        }

        if (handleBondStateChanged(action, intent)) {
            return;
        }

        if (handleConnectionStateChange(action, intent)) {
            return;
        }

        Log.d(APP_TAG, "Unexpected BT event received:" + action);
    }

    public void registerForEvents() {

        // Filter for watchbluetooth on/off state change
        IntentFilter btEventsFilter = new IntentFilter();

        btEventsFilter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);      // BT Enable/Disable
        btEventsFilter.addAction(BluetoothAdapter.ACTION_SCAN_MODE_CHANGED);  // Discoverable
        btEventsFilter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED);  // Scan Started
        btEventsFilter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED); // Scan finished

        btEventsFilter.addAction(BluetoothDevice.ACTION_FOUND);               // for scan results
        btEventsFilter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);  // device paired
        btEventsFilter.addAction(BluetoothDevice.ACTION_ACL_CONNECTED);       // device connected
        btEventsFilter.addAction(BluetoothDevice.ACTION_ACL_DISCONNECTED);    // device disconnected

        btActivity.registerReceiver(this, btEventsFilter);
    }

    public void unregisterForEvents() {
        btActivity.unregisterReceiver(this);
    }

    private boolean handleBtStateChange(String action) {

        if (BluetoothAdapter.ACTION_STATE_CHANGED.equals(action)) {
            Log.d(APP_TAG, "New State: " + btAdapter.getState());
            // update state if it is not one of the intermediate states
            if (btAdapter.getState() == BluetoothAdapter.STATE_ON ||
                    btAdapter.getState() == BluetoothAdapter.STATE_OFF) {
                btActivity.updateCheckedState();
            }
            return true;
        }
        return false;
    }

    private boolean handleScanResult(String action, Intent intent) {

        if (BluetoothDevice.ACTION_FOUND.equals(action)) {
            // Get the BluetoothDevice object from the Intent
            BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

            // Add the name and address to an array adapter to show in a ListView
            Log.v(APP_TAG, "Found device " + device.getName() + "\n" + device.getAddress());

            //AA btActivity.scanListAdapter.addDevice(device);
            btActivity.updateFoundDevice(device);

            return true;
        }

        // this is not the action the function is supposed to handle
        return false;
    }

    private boolean handleScanModeChange(String action) {

        if (BluetoothAdapter.ACTION_SCAN_MODE_CHANGED.equals(action)) {
            btActivity.updateVisibilityState();
            Log.d(APP_TAG, "Visible: " + btAdapter.isDiscovering());

            return true;
        }

        return false;
    }

    private boolean handleDiscoveryModeChange(String action) {

        if (BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action) ||
                BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {
            btActivity.updateScanState();
            return true;
        }

        return false;
    }

    private boolean handleBondStateChanged(String action, Intent intent) {

        if (BluetoothDevice.ACTION_BOND_STATE_CHANGED.equals(action)) {
            BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
            int bondState = intent.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE,
                    BluetoothDevice.BOND_NONE);

            Log.d(APP_TAG, "Device: " + device.getName() + "Bond State Updated: " + bondState);

            btActivity.updateBondedDevices();
            return true;
        }

        return false;
    }

    private boolean handleConnectionStateChange(String action, Intent intent) {

        if (BluetoothDevice.ACTION_ACL_CONNECTED.equals(action) ||
                BluetoothDevice.ACTION_ACL_DISCONNECTED.equals(action)) {
            BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

            Log.d(APP_TAG, "Device: " + device.getName() + " " + action);
            btActivity.updateConnectedDevice(device, action);
            return true;
        }

        return false;
    }
}

