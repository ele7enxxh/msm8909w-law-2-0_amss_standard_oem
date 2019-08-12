/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchbluetooth;

import android.bluetooth.BluetoothDevice;

public class BTCachedDevice {
    BluetoothDevice device;
    boolean connectionState;

    BTCachedDevice(BluetoothDevice device) {

        this.device = device;
        connectionState = false;
    }

    void setConnectionState(boolean connected) {

        connectionState = connected;
    }

    void setConnectionState(String connectionState) {
        if (connectionState.equals(BluetoothDevice.ACTION_ACL_CONNECTED)) {
            setConnectionState(true);
        }
        else {
            setConnectionState(false);
        }
    }
    boolean isConnected() {

        return (connectionState);
    }

    BluetoothDevice getDevice() {

        return  device;
    }
}
