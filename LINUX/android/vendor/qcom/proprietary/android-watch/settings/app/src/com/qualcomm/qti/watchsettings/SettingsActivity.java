/**
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchsettings;

import android.app.Activity;
import android.os.Bundle;
import android.widget.ListView;
import android.provider.Settings;
import android.util.Log;
import android.os.SystemProperties;
/**
 * Main Entry class for settings. Lists the possible settings menu options
 */
public class SettingsActivity extends Activity {

    public static final String APP_TAG = "WatchSettings";
    SettingsListAdapter listAdapter; // for handling main activity list items
    private int accelerometerRotation;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        try {
            accelerometerRotation = Settings.System.getInt(getContentResolver(), Settings.System.ACCELEROMETER_ROTATION);
            if (accelerometerRotation != 0) {
               Settings.System.putInt(getContentResolver(), Settings.System.ACCELEROMETER_ROTATION, 0);
               Log.w(APP_TAG, "Default Value of ACCELEROMETER ROTATION is: " + accelerometerRotation + ", Setting it to 0");
            }
        } catch (Settings.SettingNotFoundException ex) {
            Log.w(APP_TAG, "Orientation setting not found");
        }

        // list of options in the main menu of settings application
        String[] listRowNames = {
            getString(R.string.main_menu_item_bt),
            getString(R.string.main_menu_item_wifi),
            getString(R.string.main_menu_item_nfc),
            getString(R.string.main_menu_item_airplane_mode),
            getString(R.string.main_menu_item_display),
            getString(R.string.main_menu_item_sound),
            getString(R.string.main_menu_item_location),
            getString(R.string.main_menu_item_cellular),
            getString(R.string.main_menu_item_date_time),
            getString(R.string.main_menu_item_factory_reset),
            getString(R.string.main_menu_item_reboot),
            getString(R.string.main_menu_item_power_off),
            getString(R.string.main_menu_item_about),
            getString(R.string.main_menu_item_developer_options)
        };

        listAdapter = new SettingsListAdapter(this, listRowNames);
        ListView listView = (ListView) findViewById(R.id.listView);
        // layout for circuler display
        if(Utils.IsDisplayTypeCircular()) {
            int paddingDp = getResources().getDimensionPixelOffset(R.dimen.circuler_padding);
            listView.setPadding(paddingDp, 0, 0, paddingDp);
        }

        listView.setAdapter(listAdapter);
        listView.setOnItemClickListener(new SettingsListClickListener(this, listRowNames));
    }
}
