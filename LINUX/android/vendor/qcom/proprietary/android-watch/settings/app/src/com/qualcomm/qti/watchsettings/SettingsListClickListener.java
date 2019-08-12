/**
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchsettings;

import android.content.Context;
import android.content.Intent;
import android.provider.Settings;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Toast;
import android.os.SystemProperties;

/**
 * This class handles single click on main menu and starts the appropriate activity
 */
class SettingsListClickListener implements AdapterView.OnItemClickListener {
    private final String[] listItemNames;
    private final Context context;

    private static final String debugShutdownEnabledProp = "debug.enableshutdown";
    private static final String APP_TAG = "WatchSettings";

    public SettingsListClickListener(Context context, String[] values) {

        this.context = context;
        this.listItemNames = values;
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {

        Intent intent;

        if (listItemNames[position].equals(context.getString(R.string.main_menu_item_bt))) {
            intent = new Intent(Settings.ACTION_BLUETOOTH_SETTINGS);
            context.startActivity(intent);

            return;
        }

        if (listItemNames[position].equals(context.getString(R.string.main_menu_item_wifi))) {
            intent = new Intent(Settings.ACTION_WIFI_SETTINGS);
            context.startActivity(intent);

            return;
        }

        if (listItemNames[position].equals(context.getString(R.string.main_menu_item_nfc))) {
            intent = new Intent(Settings.ACTION_NFC_SETTINGS);
            context.startActivity(intent);

            return;
        }

        if (listItemNames[position].equals(
            context.getString(R.string.main_menu_item_airplane_mode))) {
            intent = new Intent(Settings.ACTION_AIRPLANE_MODE_SETTINGS);
            context.startActivity(intent);

            return;
        }

        if (listItemNames[position].equals(
            context.getString(R.string.main_menu_item_display))) {
            intent = new Intent(Settings.ACTION_DISPLAY_SETTINGS);
            context.startActivity(intent);

            return;
        }

        if (listItemNames[position].equals(
            context.getString(R.string.main_menu_item_sound))) {
            intent = new Intent(Settings.ACTION_SOUND_SETTINGS);
            context.startActivity(intent);

            return;
        }

        if (listItemNames[position].equals(
            context.getString(R.string.main_menu_item_location))) {
            intent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
            context.startActivity(intent);

            return;
        }

        if (listItemNames[position].equals(
            context.getString(R.string.main_menu_item_cellular))) {
            intent = new Intent(Settings.ACTION_WIRELESS_SETTINGS);
            context.startActivity(intent);

            return;
        }

        if (listItemNames[position].equals(
            context.getString(R.string.main_menu_item_factory_reset))) {
            FactoryResetHandler handler = new FactoryResetHandler(context);
            handler.performReset();

            return;
        }

        if (listItemNames[position].equals(
            context.getString(R.string.main_menu_item_reboot))) {

            if (isShutdownEnabled(context)) {
                intent = new Intent(Intent.ACTION_REBOOT);
                intent.putExtra("android.intent.extra.KEY_CONFIRM", true);
                context.startActivity(intent);
            }
            return;
        }

        if (listItemNames[position].equals(
                context.getString(R.string.main_menu_item_power_off))) {

            if (isShutdownEnabled(context)) {
                intent = new Intent("android.intent.action.ACTION_REQUEST_SHUTDOWN");
                intent.putExtra("android.intent.extra.KEY_CONFIRM", true);
                context.startActivity(intent);
            }
            return;
        }

        if (listItemNames[position].equals(
            context.getString(R.string.main_menu_item_about))) {
            intent = new Intent(Settings.ACTION_DEVICE_INFO_SETTINGS);
            context.startActivity(intent);

            return;
        }

        if (listItemNames[position].equals(
            context.getString(R.string.main_menu_item_developer_options))) {
            intent = new Intent(Settings.ACTION_APPLICATION_DEVELOPMENT_SETTINGS);
            context.startActivity(intent);

            return;
        }

        if (listItemNames[position].equals(
            context.getString(R.string.main_menu_item_date_time))) {
            intent = new Intent(Settings.ACTION_DATE_SETTINGS);
            context.startActivity(intent);

            return;
        }

        Toast.makeText(context,
            context.getString(R.string.unsupported_feature), Toast.LENGTH_SHORT).show();

    }

    static boolean isShutdownEnabled(Context context) {

        boolean shutdownEnabledProperty = SystemProperties.getBoolean(debugShutdownEnabledProp, true);
        Log.v(APP_TAG, "shutdownEnabled is " + shutdownEnabledProperty);

        if (!shutdownEnabledProperty) {
            Toast.makeText(context,
                    context.getString(R.string.disabled_feature), Toast.LENGTH_SHORT).show();
        }

        return shutdownEnabledProperty;
    }
}
