/**
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchsettings;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.os.SystemProperties;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.view.View.OnClickListener;


public class DisplayActivity  extends Activity{

    public static final String APP_TAG = "WatchSettings";

    private LinearLayout contextModeCheckBox;
    private static final String contextualModePropertyKey = "persist.sys.ctxtl_enabled";
    private boolean isChecked;
    private TextView contextModestatus;


    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.display_activity);

        // layout for circuler display
        if(Utils.IsDisplayTypeCircular()) {
            TextView textView = (TextView)findViewById(R.id.textView);
            int paddingDp = getResources().getDimensionPixelOffset(R.dimen.circuler_padding);
            textView.setPadding(0, paddingDp, 0, 0);
        }
    }

    @Override
    protected void onResume() {

        super.onResume();
        getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN);

        setupContextualModeCheckBox();
    }

    void setupContextualModeCheckBox() {

        contextModeCheckBox = (LinearLayout) findViewById(R.id.contextModeCheckBox);
        contextModestatus = (TextView) findViewById(R.id.contextModestatus);
        updateContextualModeCheckBox();
        contextModeCheckBox.setOnClickListener(new OnClickListener()
        {
            @Override
            public void onClick(View v) {

                updateContextualModeState(!isChecked);
                updateContextualModeCheckBox(); // this will handle use case of update failure
            }
        });
    }

    void updateContextualModeCheckBox() {

        boolean isContextualModeOn = SystemProperties.getBoolean(contextualModePropertyKey, true);
        Log.i(APP_TAG, "Get Contextual Mode: " + Boolean.toString(isContextualModeOn));
        isChecked = isContextualModeOn;
        //contextModeCheckBox.setChecked(isContextualModeOn);

        if (isChecked) {
            contextModestatus.setText(getString(R.string.display_contextual_mode_on));
        }
        else {
            contextModestatus.setText(getString(R.string.display_contextual_mode_off));
        }
    }

    void updateContextualModeState(boolean isOn) {

        Log.i(APP_TAG, "Set Contextual Mode: " + Boolean.toString(isOn));
        try {
            SystemProperties.set(contextualModePropertyKey, Boolean.toString(isOn));
        }
        catch(Exception e) {
            Log.e(APP_TAG, "Failed to set contextualModeProperty");
            e.printStackTrace();
        }
    }
}
