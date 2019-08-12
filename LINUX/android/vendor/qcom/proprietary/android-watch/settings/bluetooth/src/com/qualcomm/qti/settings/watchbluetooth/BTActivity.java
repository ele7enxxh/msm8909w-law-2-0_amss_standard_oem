/**
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchbluetooth;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ListView;
import android.widget.TextView;
import android.os.SystemProperties;
import android.widget.Toast;
import android.widget.LinearLayout;

import java.lang.reflect.Method;
import java.util.List;
import java.util.Set;

/**
 * Bluetooth main activity
 */
public class BTActivity extends Activity {

    public static final String APP_TAG = "BTSettings";
    private static final String DISPLAY_TYPE = "ro.display.type";
    private static final int REQ_ID_DISCOVERY_ON = 1;
    private static final int VISIBILITY_DUR = 120; // make device visible for 120 seconds

    private BluetoothAdapter btAdapter;
    private BTBroadcastEventHandler btBroadcastEventHandler;

    private LinearLayout btSwitch; // enable or disable bt
    private Button visibilityButton;
    private Button scanButton;
    private TextView btStatus;
    private boolean isChecked;

    BTDeviceListAdapter scanListAdapter;
    BTProfileManager profileManager;

    BTCachedDeviceManager deviceManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bt);
        deviceManager = BTCachedDeviceManager.getInstance();

        // create the BT adapter
        setupBTAdapter();

        // initialize the profile manager
        profileManager = new BTProfileManager(this);

        // create broadcast event handler to receive BT events
        btBroadcastEventHandler = new BTBroadcastEventHandler(this, btAdapter);

        // setup the UI elements: buttons, switch, list
        setupScanList();
        setupBTStateSwitch();
        setupVisibilityButton();
        setupScanButton();

        Log.v(APP_TAG, "Main Activity Created");
    }

    @Override
    protected void onStart() {

        super.onStart();
        btBroadcastEventHandler.registerForEvents();
    }

    @Override
    protected void onStop() {

        btBroadcastEventHandler.unregisterForEvents();
        btAdapter.cancelDiscovery(); // in case scan is going on cancel it

        super.onStop();
    }

    @Override
    protected void onDestroy() {

        profileManager.releaseResource();
        super.onDestroy();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {

        if (requestCode == REQ_ID_DISCOVERY_ON) {
            Log.d(APP_TAG, "Discovery result:" + resultCode);
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    /**
     * A new device is found in scan, so refresh the device list view
     */
    void updateFoundDevice(BluetoothDevice device) {

        deviceManager.addDevice(new BTCachedDevice(device));
        scanListAdapter.notifyDataSetChanged();
    }

    /**
     * Get the list of bonded devices and update the scan list
     */
    void updateBondedDevices() {

        Set<BluetoothDevice> pairedDevices = btAdapter.getBondedDevices();
        Log.d(APP_TAG, "# bonded devices:" + pairedDevices.size());

        deviceManager.addDevices(pairedDevices);
        scanListAdapter.notifyDataSetChanged();
    }


    void updateConnectedDevices() {

        List<BluetoothDevice> connectedDevices = profileManager.getConnectedDevices();

        if (connectedDevices != null) {
            deviceManager.addConnectedDevices(connectedDevices);
            scanListAdapter.notifyDataSetChanged();
        }
    }

    /**
     * Device connection state is updated, so refresh the device list view
     */
    void updateConnectedDevice(BluetoothDevice device, String connectionState) {

        BTCachedDevice foundDevice = deviceManager.getDevice(device);
        if (foundDevice == null) {
            foundDevice = new BTCachedDevice(device);
            deviceManager.addDevice(foundDevice);
            scanListAdapter.notifyDataSetChanged();
        }

        foundDevice.setConnectionState(connectionState);
        scanListAdapter.notifyDataSetChanged();
    }

    /**
     * Set the state of the visibility button based on SCAN mode
     */
    void updateVisibilityState() {

        if (btAdapter.getScanMode() == BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE) {
            // set to green to indicate that this device is visible to other nearby devices
            visibilityButton.setTextColor(getResources().getColor(R.color.VisibleColor));
        } else {
            // set to red meaning this device is not visible to nearby devices
            visibilityButton.setTextColor(getResources().getColor(R.color.NotVisibleColor));
        }
    }

    private void setupVisibilityButton() {

        visibilityButton = (Button) findViewById(R.id.discovery_button);
        visibilityButton.setText(getString(R.string.bt_visibility_button_name));

        // set up single click listener
        visibilityButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // make the device visible. If visibility is ongoing, calling this extends the
                // visibility time
                makeDeviceVisible();
            }
        });
        updateVisibilityState();
    }

    private void makeDeviceVisible() {

        Intent discoverableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
        discoverableIntent.putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, VISIBILITY_DUR);
        startActivityForResult(discoverableIntent, REQ_ID_DISCOVERY_ON);
    }

    /**
     * Depending on the state of the bt adapter, update the state of the switch
     */
    void updateCheckedState() {

        // set the switch state appropriately
        isChecked = btAdapter.isEnabled();
        if(isChecked) {
            btStatus.setText(getString(R.string.bt_switch_state_on));
            profileManager.setEnabled(true);
            // show the bonded devices by default
            updateBondedDevices();
            updateConnectedDevices();
        } else {
            btStatus.setText(getString(R.string.bt_switch_state_off));
            deviceManager.clear(); // remove all scanned devices displayed currently
            scanListAdapter.notifyDataSetChanged();
            profileManager.setEnabled(false); // remove all registered profiles
        }
    }

    private void setupBTStateSwitch() {

        btSwitch = (LinearLayout) findViewById(R.id.bt_switch);
        btStatus = (TextView) findViewById(R.id.btStatus);
        btSwitch.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!isChecked) {
                    if (!btAdapter.enable()) {
                        Log.e(APP_TAG, "BT ON Error");
                    }
                } else {
                    btAdapter.disable();
                }
             }
        });
        updateCheckedState();
    }

    private void setupBTAdapter() {

        btAdapter = BluetoothAdapter.getDefaultAdapter();

        Log.i(APP_TAG, "BT Name: " + btAdapter.getName() + " Address: " + btAdapter.getAddress());
    }

    /**
     * Update the state of the scan button based on whether scan is ongoing
     */
    void updateScanState() {

        if (btAdapter.isDiscovering()) {
            scanButton.setTextColor(getResources().getColor(R.color.DiscoveryColor));
        } else {
            scanButton.setTextColor(getResources().getColor(R.color.NotDiscoveringColor));
        }
    }

    /**
     * Setup the scan button and its listeners
     * This button acts a toggle to start and stop scan. The text color indicates whether
     * a scan is ongoing. Green indicates an ongoing scan while black indicates that no scan is
     * in progress
     */
    private void setupScanButton() {

        scanButton = (Button) findViewById(R.id.scan_button);
        scanButton.setText(getString(R.string.bt_scan_button_name));

        scanButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                if (!btAdapter.isDiscovering()) {
                    deviceManager.clear();
                    scanListAdapter.notifyDataSetChanged();

                    if (!btAdapter.isEnabled()) {
                        Toast.makeText(getApplication(),
                                R.string.bt_scan_disabled_error, Toast.LENGTH_SHORT).show();
                        return;
                    }

                    if (!btAdapter.startDiscovery()) {
                        Log.e(APP_TAG, getString(R.string.bt_scan_error));
                        return;
                    }

                    // always show bonded and connected devices in the list of devices
                    updateBondedDevices();
                    updateConnectedDevices();

                    scanListAdapter.notifyDataSetChanged();
                } else {
                    btAdapter.cancelDiscovery();
                }
            }
        });

        updateScanState();
    }

    /**
     * Set up the list display for showing devices found by doing a scan
     */
    private void setupScanList() {

        scanListAdapter = new BTDeviceListAdapter(this);

        ListView listView = (ListView) findViewById(R.id.list_scan);
     // layout for circuler display
        if(Utils.IsDisplayTypeCircular()) {
            int paddingDp = getResources().getDimensionPixelOffset(R.dimen.circuler_padding);
            listView.setPadding(paddingDp, 0, 0, paddingDp);
        }

        listView.setAdapter(scanListAdapter);

        // single click on the device creates a bond if the device is not already bonded
        // single click on a bonded device creates a connection
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {

                // before bonding/connecting operation clear ongoing scan state
                btAdapter.cancelDiscovery();

                BTCachedDevice cachedDevice = deviceManager.getDevice(position);
                if (cachedDevice == null) {
                    return; // no item on the list
                }

                BluetoothDevice device = cachedDevice.getDevice();
                if (device == null) {
                    return; // no items on the list
                }

                if (device.getBondState() == BluetoothDevice.BOND_NONE) {
                    Log.v(APP_TAG, "Pairing with " + device.getName() + " " + device.getAddress());
                    device.createBond();
                } else if (device.getBondState() == BluetoothDevice.BOND_BONDED) {
                    Log.v(APP_TAG, "Connecting to " + device.getName() + " " + device.getAddress());
                    profileManager.connect(device);
                } else {
                    Log.v(APP_TAG, "Device " + device.getName()
                            + " already in bonding state");
                }
            }
        });

        // long click on the device removes a bond
        listView.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
            @Override
            public boolean onItemLongClick(AdapterView<?> parent, View view, int position, long id) {

                // before bonding operation clear ongoing scan state
                btAdapter.cancelDiscovery();

                BTCachedDevice cachedDevice = deviceManager.getDevice(position);
                if (cachedDevice == null) {
                    return false;
                }
                BluetoothDevice device = cachedDevice.getDevice();

                if (device == null) {
                    return false; // no items on the list
                }

                Log.v(APP_TAG, "Unpairing from " + device.getName() + " " + device.getAddress());

                callRemoveDevice(device);
                return true;
            }
        });

        View footer = getLayoutInflater().inflate(R.layout.scan_list_footer, null);
        listView.addFooterView(footer);
    }


    /**
     * Unbonds a device
     *
     * @param device the device to be unbonded
     */
    private void callRemoveDevice(BluetoothDevice device) {

        Toast.makeText(this, getString(R.string.bt_unbonding_in_progress), Toast.LENGTH_SHORT).show();

        if (device.getBondState() == BluetoothDevice.BOND_BONDING) {
            // device is in the process of bonding, so cancel bonding
            callCancelBondProcess(device);
            return;
        }

        // device is already bonded or not bonded at all, call remove on device
        try {
            Log.d(APP_TAG, "Unbonding " + device.getName());
            Method method = device.getClass().getMethod("removeBond", (Class[]) null);
            method.invoke(device, (Object[]) null);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Cancel an ongoing unbonding process
     *
     * @param device the device which is currently in the  bonding process
     */
    private void callCancelBondProcess(BluetoothDevice device) {

        try {
            Log.d(APP_TAG, "Cancelling Bond " + device.getName());
            Method method = device.getClass().getMethod("cancelBondProcess", (Class[]) null);
            method.invoke(device, (Object[]) null);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * * This class handles user action to change BT enable/disable state
     */
    class BTStateOnCheckedChangeListener implements CompoundButton.OnCheckedChangeListener {

        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            if (isChecked) {
                if (!btAdapter.enable()) {
                    Log.e(APP_TAG, "BT ON Error");
                }
            } else {
                btAdapter.disable();
            }
        }
    }

}
