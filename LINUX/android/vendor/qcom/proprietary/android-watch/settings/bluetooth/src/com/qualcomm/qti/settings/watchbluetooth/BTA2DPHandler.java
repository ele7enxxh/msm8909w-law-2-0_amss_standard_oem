/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchbluetooth;

import android.bluetooth.BluetoothA2dp;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothProfile;
import android.util.Log;

import java.lang.reflect.Method;
import java.util.List;

public class BTA2DPHandler implements BTProfileHandler {

    // create a possibly unique log tag to help with adb debugging
    public static final String APP_TAG = "BTSettings";

    private final String AudioSyncUUID = "0000110B-0000-1000-8000-00805F9B34FB";

    private BluetoothAdapter btAdapter;
    private BTActivity btActivity;
    private BluetoothA2dp a2dpProfile;

    BTA2DPHandler(final BTActivity btActivity) {

        this.btActivity = btActivity;
        btAdapter = BluetoothAdapter.getDefaultAdapter();

        // access the default a2dp profile
        btAdapter.getProfileProxy(btActivity, new BluetoothProfile.ServiceListener() {
            @Override
            public void onServiceConnected(int profile, BluetoothProfile proxy) {
                a2dpProfile = (BluetoothA2dp) proxy;
                Log.v(APP_TAG, "A2DP Service Connected");
                btActivity.updateConnectedDevices();
            }

            @Override
            public void onServiceDisconnected(int profile) {

                Log.v(APP_TAG, "A2DP Service disconnected");
            }
        }, BluetoothProfile.A2DP);
    }

    @Override
    public boolean connect(BluetoothDevice device) {

        if (a2dpProfile == null) {
            Log.d(APP_TAG, "No A2DP Service");
            return false;
        }

        // disconnect any already connected a2dp devices
        List<BluetoothDevice> connectedDevices = a2dpProfile.getConnectedDevices();
        if (connectedDevices != null) {
            for (BluetoothDevice thisDevice : connectedDevices) {

                if (thisDevice.equals(device)) {
                    Log.v(APP_TAG, "Ignoring connect, a2dp already connected");
                    return true;
                }

                disconnect(thisDevice);
            }
        }

        // now try connecting to the device
        try {
            Log.d(APP_TAG, "Calling A2DP Connect to " + device.getName());
            Method method = a2dpProfile.getClass().getMethod("connect", BluetoothDevice.class);
            method.invoke(a2dpProfile, device);

        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    boolean isConnected(BluetoothDevice device) {

        if (a2dpProfile == null) {
            Log.v(APP_TAG, "a2dp profile is null, so not connected");
            return false;
        }

        Log.v(APP_TAG, "Connected State: " + a2dpProfile.getConnectionState(device));
        return (a2dpProfile.getConnectionState(device) == BluetoothProfile.STATE_CONNECTED);
    }

    @Override
    public boolean disconnect(BluetoothDevice device) {

        if (a2dpProfile == null) {
            return false;
        }

        // now try disconnecting from the device
        try {
            Log.d(APP_TAG, "Calling A2DP disconnect from " + device.getName());
            Method method = a2dpProfile.getClass().getMethod("disconnect", BluetoothDevice.class);
            method.invoke(a2dpProfile, device);

        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }

        return true;
    }

    public List<BluetoothDevice> getConnectedDevices() {

        if (a2dpProfile == null) {
            return null;
        }
        return a2dpProfile.getConnectedDevices();
    }

    @Override
    public void releaseResource() {

        finalize();
    }

    String getUuid() {

        return AudioSyncUUID;
    }

    protected void finalize() {

        Log.d(APP_TAG, "finalize()");
        if (a2dpProfile != null) {
            try {
                BluetoothAdapter.getDefaultAdapter().closeProfileProxy(BluetoothProfile.A2DP,
                        a2dpProfile);
                a2dpProfile = null;
                Log.v(APP_TAG, "Releasing a2dp profile proxy");
            } catch (Throwable t) {
                Log.w(APP_TAG, "Error cleaning up A2DP proxy", t);
            }
        }
    }
}
