/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchbluetooth;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothHeadset;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothProfile;
import android.util.Log;

import java.lang.reflect.Method;
import java.util.List;

public class BTHeadsetHandler implements BTProfileHandler {

    public static final String APP_TAG = "BTSettings";

    private final String HeadsetUUID = "00001108-0000-1000-8000-00805F9B34FB";

    private BluetoothAdapter btAdapter;
    private BTActivity btActivity;
    private BluetoothHeadset headsetProfile;

    BTHeadsetHandler(final BTActivity btActivity) {

        this.btActivity = btActivity;
        btAdapter = BluetoothAdapter.getDefaultAdapter();

        // access the default headset profile
        btAdapter.getProfileProxy(btActivity, new BluetoothProfile.ServiceListener() {

            @Override
            public void onServiceConnected(int profile, BluetoothProfile proxy) {
                headsetProfile = (BluetoothHeadset) proxy;

                btActivity.updateConnectedDevices();
                Log.v(APP_TAG, "Headset Service Connected");

            }

            @Override
            public void onServiceDisconnected(int profile) {

                Log.v(APP_TAG, "HSP Service disconnected");
            }
        }, BluetoothProfile.HEADSET);
    }

    @Override
    public boolean connect(BluetoothDevice device) {

        if (headsetProfile == null) {
            Log.d(APP_TAG, "No HSP Service");
            return false;
        }

        // disconnect any already connected a2dp devices
        List<BluetoothDevice> connectedDevices = headsetProfile.getConnectedDevices();
        if (connectedDevices != null) {
            for (BluetoothDevice thisDevice : connectedDevices) {
                if (thisDevice.equals(device)) {
                    Log.v(APP_TAG, "Ignoring connect, already HSP connected");
                    return true;
                }

                disconnect(thisDevice);
            }
        }

        try {
            Log.d(APP_TAG, "Calling HSP Connect to " + device.getName());
            Method method = headsetProfile.getClass().getMethod("connect", BluetoothDevice.class);
            method.invoke(headsetProfile, device);

        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }

    @Override
    public boolean disconnect(BluetoothDevice device) {

        if (headsetProfile == null) {
            return false;
        }

        // now try dis-connecting from the device
        try {
            Log.d(APP_TAG, "Calling HSP dis-connect from " + device.getName());
            Method method = headsetProfile.getClass().getMethod("disconnect", BluetoothDevice.class);
            method.invoke(headsetProfile, device);

        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }

        return true;
    }

    public List<BluetoothDevice> getConnectedDevices() {

        if (headsetProfile == null) {
            return null;
        }
        return headsetProfile.getConnectedDevices();
    }

    boolean isConnected(BluetoothDevice device) {

        if (headsetProfile == null) {
            Log.v(APP_TAG, "a2dp profile is null, so not connected");
            return false;
        }

        Log.v(APP_TAG, "Connected State: " + headsetProfile.getConnectionState(device));
        return (headsetProfile.getConnectionState(device) == BluetoothProfile.STATE_CONNECTED);
    }

    String getUuid() {

        return HeadsetUUID;
    }

    @Override
    public void releaseResource() {

        finalize();
    }

    protected void finalize() {

        Log.d(APP_TAG, "finalize()");
        if (headsetProfile != null) {
            try {
                BluetoothAdapter.getDefaultAdapter().closeProfileProxy(BluetoothProfile.HEADSET,
                        headsetProfile);
                headsetProfile = null;
                Log.v(APP_TAG, "Releasing HSP proxy");
            } catch (Throwable t) {
                Log.w(APP_TAG, "Error cleaning up HSP proxy", t);
            }
        }
    }

}
