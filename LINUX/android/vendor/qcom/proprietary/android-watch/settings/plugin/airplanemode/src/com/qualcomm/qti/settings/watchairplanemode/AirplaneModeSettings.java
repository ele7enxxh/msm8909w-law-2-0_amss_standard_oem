/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.watchairplanemode;

import android.app.Activity;
import android.content.Intent;
import android.database.ContentObserver;
import android.os.Bundle;
import android.os.Handler;
import android.provider.Settings;
import android.util.Log;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.view.View.OnClickListener;
import android.view.View;
import android.os.SystemProperties;


public class AirplaneModeSettings extends Activity {

    private static final String APP_TAG = "AirplaneModeSettings";

    private LinearLayout airplaneModeSwitch;

    private ContentObserver airplaneModeObserver;

    private boolean isChecked;

    private TextView apmMode;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_airplane_mode);

        // create an observer for airplane mode changes that happen outside of this app
        airplaneModeObserver = new ContentObserver(new Handler()) {
            @Override
            public void onChange(boolean selfChange) {

                updateSwitchState();
            }
        };

        setupSwitch();
    }

    @Override
    protected void onResume() {

        super.onResume();
        updateSwitchState();

        // register for any change in the airplane mode content
        getContentResolver().registerContentObserver(
                Settings.Global.getUriFor(Settings.Global.AIRPLANE_MODE_ON), true,
                airplaneModeObserver);
    }

    @Override
    protected void onPause() {

        getContentResolver().unregisterContentObserver(airplaneModeObserver);
        super.onPause();
    }

    // set up the switch
    private void setupSwitch() {

        airplaneModeSwitch = (LinearLayout) findViewById(R.id.switch1);
        apmMode = (TextView) findViewById(R.id.apmMode);
        airplaneModeSwitch.setOnClickListener( new OnClickListener() {
            @Override
            public void onClick(View view) {

                // if the new switch state is different from the airplane mode, then update the
                // airplane mode according to the switch state

                if (isChecked == isAirplaneModeOn()) {
                    setAirplaneMode(!isChecked);
                }
                updateCheckedText();
            }
        });

        updateSwitchState();
    }

    // update the switch state to ON if airplane mode is set, off otherwise
    private void updateSwitchState() {
        isChecked = isAirplaneModeOn();
        //airplaneModeSwitch.setChecked(isAirplaneModeOn());
        updateCheckedText();
    }

    // set the text state of the switch based on its checked state
    private void updateCheckedText() {

        if (isChecked) {
            apmMode.setText(R.string.airplane_mode_on);
        } else {
            apmMode.setText(R.string.airplane_mode_off);
        }
    }

    // test whether airplane mode is on
    private boolean isAirplaneModeOn() {

        return (Settings.Global.getInt(getContentResolver(),
            Settings.Global.AIRPLANE_MODE_ON, 0) != 0);
    }

    // Set the airplane mode state and send a broadcast about this change
    private void setAirplaneMode(boolean isOn) {

        if (isOn) {
            Settings.Global.putInt(getContentResolver(), Settings.Global.AIRPLANE_MODE_ON, 1);
            Log.i(APP_TAG, "Airplane mode is set to ON");
        } else {
            Settings.Global.putInt(getContentResolver(), Settings.Global.AIRPLANE_MODE_ON, 0);
            Log.i(APP_TAG, "Airplane mode is set to OFF");
        }

        sendAirplaneModeChangeBroadcast(isOn);
    }

    // send broadcast event about airplane mode change
    private void sendAirplaneModeChangeBroadcast(boolean enabled) {

        Intent intent = new Intent(Intent.ACTION_AIRPLANE_MODE_CHANGED);
        intent.putExtra("state", enabled);

        Log.v(APP_TAG, "Sending airplane mode change broadcast");
        this.sendBroadcast(intent);
    }

    //This class handles user action to change Airplane Mode enable/disable switch state
 /*   class AirplaneModeSwitchStateOnClickChangeListener implements View.OnClickListener
    {

        @Override
        public void OnClick(View view) {

            // if the new switch state is different from the airplane mode, then update the
            // airplane mode according to the switch state
            if (isChecked != isAirplaneModeOn()) {
                setAirplaneMode(isChecked);
            }

            updateCheckedText();
        }
    }*/
}