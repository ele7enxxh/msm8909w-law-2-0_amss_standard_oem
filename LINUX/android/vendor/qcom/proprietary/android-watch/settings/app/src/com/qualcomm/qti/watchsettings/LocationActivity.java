/**
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchsettings;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.location.LocationManager;
import android.os.Bundle;
import android.provider.Settings;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;
import android.os.SystemProperties;

public class LocationActivity extends Activity {

    private static final String APP_TAG = "WatchSettings";
    private static final String MODE_CHANGING_ACTION = "com.android.settings.location.MODE_CHANGING";
    private static final String CURRENT_MODE_KEY = "CURRENT_MODE";
    private static final String NEW_MODE_KEY = "NEW_MODE";

    private String[] rowValues;
    String[] rowTags;
    private ArrayAdapter adapter;
    private LocationIzatHandler izatHandler;

    private ModeChangeReceiver modeChangeReceiver;

    private int numVisibleRows; // number of rows in the list made visible

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.location_activity);
        // layout for circuler display
        if(Utils.IsDisplayTypeCircular()) {
            TextView textView = (TextView)findViewById(R.id.textView);
            int paddingDp = getResources().getDimensionPixelOffset(R.dimen.circuler_padding);
            textView.setPadding(0, paddingDp, 0, 0);
        }
        // setup the broadcast receiver to listen for mode change events
        modeChangeReceiver = new ModeChangeReceiver();

        izatHandler = new LocationIzatHandler(this);

        // setup the adapter and list view and click listeners
        setupListView();

    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.d(APP_TAG, "Resuming..");
        updateRowValue(); // update rowValues to reflect current state of location modes

        IntentFilter modeFilter = new IntentFilter(LocationManager.MODE_CHANGED_ACTION);
        this.registerReceiver(modeChangeReceiver, modeFilter);
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(modeChangeReceiver);
    }

    @Override
    protected void onStart() {
        super.onStart();
        izatHandler.bindIzatService();
    }

    @Override
    protected void onStop() {
        super.onStop();
        izatHandler.shutdownService();
    }

    void updateRowValue() {

        Log.e(APP_TAG, "Updating Row Values");
        int currentMode = getLocationMode();
        rowValues[0] = isLocationEnabledAsString(currentMode);
        rowValues[1] = locationModeAsString(currentMode);
        rowValues[2] = izatHandler.isIzatEnabled();

        updateNumVisibleRows(currentMode);

        adapter.notifyDataSetChanged(); // refresh the list display
    }

    // extract location mode state as a string representation
    private String locationModeAsString(int mode) {

        String locationMode = getString(R.string.location_mode_unknown);
        switch (mode) {
            case Settings.Secure.LOCATION_MODE_OFF:
                locationMode = getString(R.string.location_mode_disabled);
                break;
            case Settings.Secure.LOCATION_MODE_BATTERY_SAVING:
                locationMode = getString(R.string.location_mode_battery_saving);
                break;
            case Settings.Secure.LOCATION_MODE_HIGH_ACCURACY:
                locationMode = getString(R.string.location_mode_high_accuracy);
                break;
            case Settings.Secure.LOCATION_MODE_SENSORS_ONLY:
                locationMode = getString(R.string.location_mode_sensors_only);
                break;
        }

        Log.i(APP_TAG, "Location Mode: " + locationMode);
        return locationMode;
    }

    private int getLocationMode() {

        int locationMode = -1;
        try {
            locationMode = Settings.Secure.getInt(getContentResolver(), Settings.Secure.LOCATION_MODE);
            Log.v(APP_TAG, "Location Mode retrieved: " + locationMode);
        } catch (Settings.SettingNotFoundException e) {
            e.printStackTrace();
        }

        return locationMode;
    }

    private String isLocationEnabledAsString(int mode) {

        if (mode != Settings.Secure.LOCATION_MODE_OFF) {
            return getString(R.string.location_mode_enabled);
        }
        return getString(R.string.location_mode_disabled);
    }

    private void setLocationMode(int mode) {

        // send location Mode Changing broad cast
        int currentMode = getLocationMode();
        Intent intent = new Intent(MODE_CHANGING_ACTION);
        intent.putExtra(CURRENT_MODE_KEY, currentMode);
        intent.putExtra(NEW_MODE_KEY, mode);
        sendBroadcast(intent, android.Manifest.permission.WRITE_SECURE_SETTINGS);

        // now update the mode to the new mode
        Settings.Secure.putInt(getContentResolver(), Settings.Secure.LOCATION_MODE, mode);
        Log.d(APP_TAG, "Put Int Location Mode:" +  mode);
        updateRowValue();
    }

    private void setupListView() {

        Log.d(APP_TAG, "Setting up List");
        updateRowTags();

        rowValues = new String[3];
        numVisibleRows = rowValues.length;

        adapter = new ArrayAdapter(this, android.R.layout.simple_expandable_list_item_2,
                android.R.id.text1, rowTags) {

            @Override
            public int getCount() {
                return numVisibleRows;
            }

            @Override
            public View getView(int position, View convertView, ViewGroup parent) {

                View view = super.getView(position, convertView, parent);
                TextView value = (TextView) view.findViewById(android.R.id.text2);
                value.setText(rowValues[position]);
                return view;
            }
        };

        ListView listView = (ListView) findViewById(R.id.listViewLocation);
        // layout for circuler display
        if(Utils.IsDisplayTypeCircular()) {
            int paddingDp = getResources().getDimensionPixelOffset(R.dimen.circuler_padding);
            listView.setPadding(paddingDp, paddingDp, 0, paddingDp);
        }

        listView.setAdapter(adapter);

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {

                if (position == 0) { // means location state
                    toggleLocationState();
                }

                if (position == 1) { // means location mode
                    toggleLocationModes();
                }

                if (position == 2) { // means IZAT options
                    izatHandler.toggleIzat();
                }
            }
        });
    }

    private void updateNumVisibleRows(int mode) {

        if (mode == Settings.Secure.LOCATION_MODE_OFF) {
            numVisibleRows = 1;
        }
        else {
            numVisibleRows =  rowValues.length;
        }
    }

    private void updateRowTags() {

        rowTags = new String[3];
        rowTags[0] = getString(R.string.location_menu_location_state);
        rowTags[1] = getString(R.string.location_menu_location_mode);
        rowTags[2] = izatHandler.getTitle();
    }

    private void toggleLocationState() {

        if (getLocationMode() == 0) {
            setLocationMode(Settings.Secure.LOCATION_MODE_HIGH_ACCURACY);
        } else {
            setLocationMode(Settings.Secure.LOCATION_MODE_OFF);
        }
    }

    private void toggleLocationModes() {

        int currentMode = getLocationMode();
        if (currentMode == 0) {
            // do nothing as location is disabled
            Log.d(APP_TAG, "Cant toggle location Mode as location is disabled");
            return;
        }

        // toggle through the three location modes
        currentMode++;
        if (currentMode > Settings.Secure.LOCATION_MODE_HIGH_ACCURACY) {
            currentMode = Settings.Secure.LOCATION_MODE_SENSORS_ONLY;
        }
        setLocationMode(currentMode);
    }

    /**
     * This class handles Location Mode change actions
     */
    public class ModeChangeReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context c, Intent i) {
            Log.v(APP_TAG, "Received Mode Change Action");
            updateRowValue();
        }
    }
}
