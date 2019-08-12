/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchbluetooth;

import android.bluetooth.BluetoothDevice;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;

/**
 * This class is responsible for managing which profiles to connect to and initiating the
 * connection through appropriate handlers
 */
public class BTProfileManager {

    public static final String APP_TAG = "BTSettings";

    BTActivity btActivity;
    BTA2DPHandler a2dpHandler;
    BTHeadsetHandler headsetHandler;

    boolean enabled;

    BTProfileManager(BTActivity btActivity) {

        this.btActivity = btActivity;
        enabled = false;
    }

    void setEnabled(boolean enabled) {

        this.enabled = enabled;

        if (enabled) {
            createProfileHandlers();
        } else {
            releaseResource();
        }
    }

    void createProfileHandlers() {

        a2dpHandler = new BTA2DPHandler(btActivity);
        headsetHandler = new BTHeadsetHandler(btActivity);
    }

    boolean connect(BluetoothDevice device) {

        ArrayList<BTProfileHandler> handlers = getProfileHandlers(device);

        // iterate through the profile handlers found and try to connect to those
        for (int index = 0; index < handlers.size(); ++index) {
            BTProfileHandler handler = handlers.get(index);
            handler.connect(device);
        }

        //update the list of connected devices
        btActivity.updateConnectedDevices();

        return true;
    }

    boolean isConnected(BluetoothDevice device) {

        boolean isConnected = false;

        if (a2dpHandler != null) {
            isConnected |= a2dpHandler.isConnected(device);
        }

        if (headsetHandler != null) {
            isConnected |= headsetHandler.isConnected(device);
        }

        return isConnected;
    }

    List<BluetoothDevice> getConnectedDevices() {

        List<BluetoothDevice> connectedDevices = null;

        if (a2dpHandler != null) {
            connectedDevices = a2dpHandler.getConnectedDevices();
        }

        if (headsetHandler != null) {
            if (connectedDevices != null) {
                List<BluetoothDevice> headsetConnectedDevices = headsetHandler.getConnectedDevices();
                if (headsetConnectedDevices != null) {
                    connectedDevices.addAll(headsetConnectedDevices);
                }
            } else {
                connectedDevices = headsetHandler.getConnectedDevices();
            }
        }

        return connectedDevices;
    }

    void releaseResource() {

        if (a2dpHandler != null) {
            a2dpHandler.releaseResource();
        }

        if (headsetHandler != null) {
            headsetHandler.releaseResource();
        }
    }

    /**
     * Get the list of profile handlers to which connection should be attempted
     *
     * @param device the device specifying the supported profile uuids
     * @return a array list of profile handlers
     */
    private ArrayList<BTProfileHandler> getProfileHandlers(BluetoothDevice device) {

        ArrayList<BTProfileHandler> handlers = new ArrayList<>();

        if (device.getUuids() == null) {
            return handlers; // empty handler array as nothing is known about support UUIDs
        }

        for (int i = 0; i < device.getUuids().length; ++i) {
            String thisUuid = device.getUuids()[i].toString();
            Log.v(APP_TAG, "Supported UUids [" + i + "]:" + thisUuid);

            if (a2dpHandler.getUuid().equalsIgnoreCase(thisUuid)) {
                handlers.add(a2dpHandler);
                continue;
            }

            if (headsetHandler.getUuid().equalsIgnoreCase(thisUuid)) {
                handlers.add(headsetHandler);
                continue;
            }
        }

        if (handlers.size() == 0) {
            Log.d(APP_TAG, "NO Profile Handler matches this device");
        }

        return handlers;
    }
}
