/**
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchdeveloperoptions;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.BatteryManager;
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

public class DeveloperOptionsSettings extends Activity {
    private static final String APP_TAG = "DeveloperOptions";
    private final Context context = this;
    private String[] rowValues;
    private ArrayAdapter adapter;
    static final int TAPS_TO_BE_A_DEVELOPER = 7;
    int mDevHitCountdown;
    private static final String debugUsbDisconnectEnabledProp =
        "debug.enableusbdisconnect";
    private String PanelDisplay;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_developer_options);
        // setup the adapter, list view and click listeners
        setupListView();
    }

    @Override
    protected void onResume() {
        super.onResume();
        // update rowValues to reflect current state of developer options
        updateRowValue();

        if (Settings.Global.getInt(getContentResolver(),
            Settings.Global.DEVELOPMENT_SETTINGS_ENABLED, 0) == 0) {
            mDevHitCountdown = TAPS_TO_BE_A_DEVELOPER;
        }
    }

    //Set up the list view and populate all developer options menu items
    private void setupListView() {
        final String[] rowTags = {getString(R.string.app_name),
            getString(R.string.stay_awake_menu),
            getString(R.string.usb_debugging_menu),
            getString(R.string.mock_location_menu)};

        rowValues = new String[4];

        adapter = new ArrayAdapter(this,
            android.R.layout.simple_expandable_list_item_2,
                android.R.id.text1, rowTags) {

            @Override
            public View getView(int position, View convertView,
                ViewGroup parent) {
                View view = super.getView(position, convertView, parent);
                TextView value = (TextView) view.findViewById(
                    android.R.id.text2);
                value.setText(rowValues[position]);
                return view;
            }
            //Show all options only if developer options is turned ON
            @Override
            public int getCount() {
                if (Settings.Global.getInt(getContentResolver(),
                    Settings.Global.DEVELOPMENT_SETTINGS_ENABLED, 0) != 0) {
                    return rowTags.length;
                }
                return 1;
            }
        };

        final ListView listView = (ListView) findViewById(
            R.id.listViewDeveloperOptions);

        // layout for circuler display
        if(Utils.IsDisplayTypeCircular()) {
            int paddingDp = context.getResources().getDimensionPixelOffset(R.dimen.circuler_padding);
            TextView textView = (TextView)findViewById(R.id.appNameTextView);
            textView.setPadding(0, paddingDp, 0, 0);
            listView.setPadding(paddingDp, 0, 0, paddingDp);
        }

        listView.setAdapter(adapter);

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                int position, long id) {

                if (position == 0) {
                    mDevHitCountdown--;
                    if (mDevHitCountdown == 0) {
                        toggleDeveloperOptionsState();
                    } else if (mDevHitCountdown > 0
                        && mDevHitCountdown < (TAPS_TO_BE_A_DEVELOPER - 4)) {
                        Toast.makeText(getApplicationContext(), getResources()
                            .getQuantityString(R.plurals.show_dev_countdown,
                                mDevHitCountdown, mDevHitCountdown),
                                    Toast.LENGTH_SHORT).show();
                    } else if (mDevHitCountdown < 0) {
                     AlertDialog.Builder alertDialog =
                        new AlertDialog.Builder(DeveloperOptionsSettings.this);

                        alertDialog.setTitle(R.string.dev_options_turn_off_warning_title);
                        alertDialog.setMessage(R.string.dev_options_turn_off_warning_message);

                        //Set dialog message
                        alertDialog
                            .setCancelable(true)
                            .setPositiveButton(R.string.ok,
                            new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog,
                                    int id) {
                                    setDeveloperOptionsState(0);
                                    disableAllOptions();
                                    mDevHitCountdown = TAPS_TO_BE_A_DEVELOPER;
                                    Log.d(APP_TAG, "Disabling Dev Options now");
                                }
                            })
                            .setNegativeButton(R.string.cancel, null)
                            .create()
                            .show();
                    }
                }

                if (position == 1) {
                    toggleStayAwakeState();
                }

                if (position == 2) {
                    toggleUSBDebuggingState();
                }

                if (position == 3) {
                    toggleAllowMockLocationState();
                }
            }
        });
    }

    //Update Enable/Disable state for all developer options
    private void updateRowValue() {
        rowValues[0] = isDeveloperOptionsEnabled();
        rowValues[1] = isStayAwakeEnabled();
        rowValues[2] = isUSBDebuggingEnabled();
        rowValues[3] = isAllowMockLocationEnabled();
        adapter.notifyDataSetChanged(); // refresh the list displayed
    }

    //Disable all menu items as Developer options is turned OFF
    private void disableAllOptions() {
        setStayAwakeState(0);
        setUSBDebuggingState(0);
        setAllowMockLocationState(0);
        adapter.notifyDataSetChanged(); // refresh the list displayed
    }

    //Set Enabled/Disabled status based on the current state
    private String isDeveloperOptionsEnabled() {
        if (Settings.Global.getInt(getContentResolver(),
            Settings.Global.DEVELOPMENT_SETTINGS_ENABLED, 0) != 0) {
            return getString(R.string.on);
        }
        return getString(R.string.off);
    }

    //Get the current state of Developer Options
    private int getDeveloperOptionsState() {
        int developerOptionsState = 0;
        try {
            developerOptionsState = Settings.Global.getInt(
                getContentResolver(),
                    Settings.Global.DEVELOPMENT_SETTINGS_ENABLED, 0);
            Log.v(APP_TAG, "Developer Options current state : " +
                developerOptionsState);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return developerOptionsState;
    }

    //Set the new state of Developer Options
    private void setDeveloperOptionsState(int requiredState) {
        Settings.Global.putInt(getContentResolver(),
            Settings.Global.DEVELOPMENT_SETTINGS_ENABLED, requiredState);
        updateRowValue();
    }

    //Toggle Developer Options State
    private void toggleDeveloperOptionsState() {
        try {
            if (getDeveloperOptionsState() == 0) {

                AlertDialog.Builder alertDialog =
                    new AlertDialog.Builder(DeveloperOptionsSettings.this);

                alertDialog.setTitle(
                    R.string.dev_options_settings_warning_title);
                alertDialog.setMessage(
                    R.string.dev_options_settings_warning_message);

                alertDialog
                    .setCancelable(true)
                    .setPositiveButton(R.string.ok,
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog,
                                int id) {
                                setDeveloperOptionsState(1);
                                Toast.makeText(getApplicationContext(),
                                    "You are a Developer now!",
                                        Toast.LENGTH_SHORT).show();
                                Log.d(APP_TAG, "Enabling Developer Options");
                                }
                        })
                    .setNegativeButton(R.string.cancel,
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog,
                                int id) {
                                mDevHitCountdown = TAPS_TO_BE_A_DEVELOPER;
                                dialog.cancel();
                            }
                        })
                    .create()
                    .show();
            } else {
                setDeveloperOptionsState(0);
                disableAllOptions();

                Log.d(APP_TAG, "Disabling Developer Options");
            }
        } catch (Exception e) {
            Log.e(APP_TAG, "Failed to set Developer Options state");
            e.printStackTrace();
        }
    }

    //Check if Stay Awake option is enabled or disabled
    private String isStayAwakeEnabled() {
        if (Settings.Global.getInt(getContentResolver(),
            Settings.Global.STAY_ON_WHILE_PLUGGED_IN, 0) != 0) {
            return getString(R.string.enabled);
        }
        return getString(R.string.disabled);
    }

    //Get current state of Stay Awake option
    private int getStayAwakeState() {
        int stayAwakeState = 0;
        try {
            stayAwakeState = Settings.Global.getInt(getContentResolver(),
                Settings.Global.STAY_ON_WHILE_PLUGGED_IN, 0);
            Log.v(APP_TAG, "Stay Awake current state : " + stayAwakeState);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return stayAwakeState;
    }

    //Set the new state of Stay Awake option
    private void setStayAwakeState(int requiredState) {
        Settings.Global.putInt(getContentResolver(),
            Settings.Global.STAY_ON_WHILE_PLUGGED_IN, requiredState);
        updateRowValue();
    }

    //Toggle Stay Awake state
    private void toggleStayAwakeState() {
        try {
            if (getStayAwakeState() == 0) {
                setStayAwakeState((BatteryManager.BATTERY_PLUGGED_AC |
                    BatteryManager.BATTERY_PLUGGED_USB));
                Log.d(APP_TAG, "Enabling stay awake now");
            } else {
                setStayAwakeState(0);
                Log.d(APP_TAG, "Disabling stay awake now");
            }
        } catch (Exception e) {
            Log.e(APP_TAG, "Failed to set Stay Awake state");
            e.printStackTrace();
        }
    }

    //Check is USB Debugging option is enabled or disabled
    private String isUSBDebuggingEnabled() {
        if (Settings.Global.getInt(getContentResolver(),
            Settings.Global.ADB_ENABLED, 0) != 0) {
            return getString(R.string.enabled);
        }
        return getString(R.string.disabled);
    }

    //Get current state of USB DEbugging option
    private int getUSBDebuggingState() {
        int USBDebuggingState = 0;
        try {
            USBDebuggingState = Settings.Global.getInt(getContentResolver(),
                Settings.Global.ADB_ENABLED, 0);
            Log.v(APP_TAG, "USB Debugging current state : " + USBDebuggingState);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return USBDebuggingState;
    }

    //Set the new state of USB Debugging developer otpion
    private void setUSBDebuggingState(int requiredState) {
        if (isUsbDisconnectEnabled(context) ||
            ((isUsbDisconnectEnabled(context) == false) && (requiredState == 1))) {
            Settings.Global.putInt(getContentResolver(),
                Settings.Global.ADB_ENABLED, requiredState);
            updateRowValue();
        } else if ((isUsbDisconnectEnabled(context) == false) && (requiredState == 0)){
            Toast.makeText(context,
                context.getString(R.string.usb_disconnect_disabled),
                    Toast.LENGTH_SHORT).show();
        }
    }

    //Toggle USDB Debugging state
    private void toggleUSBDebuggingState() {
        try {
            if (getUSBDebuggingState() == 0) {

                AlertDialog.Builder alertDialog =
                    new AlertDialog.Builder(DeveloperOptionsSettings.this);

                alertDialog.setTitle(R.string.adb_dialog_warning_title);
                alertDialog.setMessage(R.string.adb_dialog_warning_message);

                alertDialog
                    .setCancelable(true)
                    .setPositiveButton(R.string.ok,
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog,
                                int id) {
                                setUSBDebuggingState(1);
                                Log.d(APP_TAG,"Enabling USB Debugging option");
                            }
                        })
                    .setNegativeButton(R.string.cancel, null)
                    .create()
                    .show();
            } else {
                setUSBDebuggingState(0);
                Log.d(APP_TAG, "Disabling USB Debugging option");
            }
        } catch (Exception e) {
            Log.e(APP_TAG, "Failed to set USB Debugging option state");
            e.printStackTrace();
        }
    }

    //Check if Mock Location is enabled or disabled
    private String isAllowMockLocationEnabled() {
        if (Settings.Secure.getInt(getContentResolver(),
            Settings.Secure.ALLOW_MOCK_LOCATION, 0) != 0) {
            return getString(R.string.enabled);
        }
        return getString(R.string.disabled);
    }

    //Get current state of Allow Mock Location developer option
    private int getAllowMockLocationState() {
        int allowMockLocationState = 0;
        try {
            allowMockLocationState = Settings.Secure.getInt(
                getContentResolver(), Settings.Secure.ALLOW_MOCK_LOCATION, 0);
            Log.v(APP_TAG, "Allow Mock Location current state : " +
                allowMockLocationState);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return allowMockLocationState;
    }

    //Set new state of Allow Mock Location developer option
    private void setAllowMockLocationState(int requiredState) {
        Settings.Secure.putInt(getContentResolver(),
            Settings.Secure.ALLOW_MOCK_LOCATION, requiredState);
        updateRowValue();
    }

    //Toggle Allow Mock Location state
    private void toggleAllowMockLocationState() {
        try {
            if (getAllowMockLocationState() == 0) {
                setAllowMockLocationState(1);
                Log.d(APP_TAG, "Enabling Allow Mock Location option");
            } else {
                setAllowMockLocationState(0);
                Log.d(APP_TAG, "Disabling Allow Mock Location option");
            }
        } catch (Exception e) {
            Log.e(APP_TAG, "Failed to set Allow Mock Location option state");
            e.printStackTrace();
        }
    }

    //Check if USB disconnection property is enabled or disabled
    static boolean isUsbDisconnectEnabled(Context context) {
        boolean usbdisconnectEnabledProperty = SystemProperties.getBoolean(
            debugUsbDisconnectEnabledProp, true);
        Log.v(APP_TAG, "usbdisconnectEnabledProperty is " +
            usbdisconnectEnabledProperty);
        return usbdisconnectEnabledProperty;
    }
}
