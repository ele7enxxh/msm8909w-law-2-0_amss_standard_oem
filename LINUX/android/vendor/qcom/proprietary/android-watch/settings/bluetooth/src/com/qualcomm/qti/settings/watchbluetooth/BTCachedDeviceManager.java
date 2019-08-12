/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchbluetooth;

import android.bluetooth.BluetoothDevice;
import android.util.Log;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Set;

/**
 * This class manages the list of connected, paired and found devices
 */
public class BTCachedDeviceManager {

    public static final String APP_TAG = "BTSettings";

    private static BTCachedDeviceManager sInstance;

    private ArrayList<BTCachedDevice> devices;

    public static synchronized BTCachedDeviceManager getInstance() {

        if (sInstance == null) {
            sInstance = new BTCachedDeviceManager();
        }

        return sInstance;
    }

    private BTCachedDeviceManager() {

        devices = new ArrayList<>();
    }

    synchronized void addDevice(BTCachedDevice device) {

        // remove any device with the same device id
        removeDevice(device);

        devices.add(device);

        Collections.sort(devices, new BTDeviceDisplayOrderComparator());
    }

    synchronized void addDevices(Set<BluetoothDevice> newDevices) {

        if (newDevices.size() > 0) {
            // Loop through paired devices
            for (BluetoothDevice device : newDevices) {
                BTCachedDevice thisDevice = new BTCachedDevice(device);
                addDevice(thisDevice);
            }
        }
    }

    synchronized void addConnectedDevices(List<BluetoothDevice> newDevices) {

        if (!newDevices.isEmpty()) {
            // Loop through paired devices
            for (BluetoothDevice device : newDevices) {
                BTCachedDevice thisDevice = new BTCachedDevice(device);
                thisDevice.setConnectionState(true);
                addDevice(thisDevice);
            }
        }
    }

    synchronized void removeDevice(BTCachedDevice device) {

        int index = getPosition(device);
        if (index != -1) {
            devices.remove(index);
        }
    }

    synchronized int getCount() {

        return devices.size();
    }

    synchronized int getPosition(BTCachedDevice device) {

        int foundIndex = -1;

        for (int index = 0; index < devices.size(); ++index) {
            BTCachedDevice thisDevice = devices.get(index);
            if (thisDevice.device.getAddress().equals(device.device.getAddress())) {
                foundIndex = index;
                break;
            }
        }
        return foundIndex;
    }

    synchronized BTCachedDevice getDevice(BluetoothDevice device) {

        for (BTCachedDevice deviceIterator : devices) {
            if (deviceIterator.getDevice().getAddress().equals(device.getAddress())) {
                return deviceIterator;
            }
        }

        return null;
    }

    synchronized BTCachedDevice getDevice(int position) {

        BTCachedDevice thisDevice = null;

        if (getCount() > position)
            thisDevice = devices.get(position);

        return thisDevice;
    }

    synchronized void clear() {

        devices.clear();
    }

    /**
     * This class is used to compare two BTCachedDevice Objects
     */
    class BTDeviceDisplayOrderComparator implements Comparator<BTCachedDevice> {

        @Override
        public int compare(BTCachedDevice lhs, BTCachedDevice rhs) {
            if (rhs.getDevice().getName() == null || lhs.getDevice().getName() == null) {
                Log.d(APP_TAG, "No Label found for this application..");
                return 0;
            }
            return lhs.getDevice().getName().compareTo(rhs.getDevice().getName());
        }
    }
}
