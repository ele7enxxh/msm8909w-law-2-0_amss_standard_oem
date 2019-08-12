/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchbluetooth;

import android.bluetooth.BluetoothDevice;

import java.util.List;

/**
 * Interface for profile handlers
 */
public abstract interface BTProfileHandler {

    // connect to this device
    abstract boolean connect(BluetoothDevice device);

    //disconnect from this device
    abstract boolean disconnect(BluetoothDevice device);

    abstract List<BluetoothDevice> getConnectedDevices();

    // release any resources allocated by this profile
    abstract void releaseResource();
}
