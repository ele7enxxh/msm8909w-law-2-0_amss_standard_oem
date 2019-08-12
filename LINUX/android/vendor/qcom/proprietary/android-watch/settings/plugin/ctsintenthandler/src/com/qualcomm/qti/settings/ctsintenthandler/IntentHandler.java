/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.settings.ctsintenthandler;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

public class IntentHandler extends Activity {
    private static final String APP_TAG = "UnsupportedFeatureIntentHandler";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_intent_handler);
        Log.i("APP_TAG", "Handling CTS intent");
    }
}
