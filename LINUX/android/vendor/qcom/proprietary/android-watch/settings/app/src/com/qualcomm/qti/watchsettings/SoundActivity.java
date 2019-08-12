/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.watchsettings;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import com.qualcomm.qti.watchsettings.sound.SoundNotificationSettings;

public class SoundActivity  extends Activity{

    public static final String APP_TAG = "WatchSettings";

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        getFragmentManager().beginTransaction().replace(android.R.id.content,
                new SoundNotificationSettings()).commit();

    }

    @Override
    protected void onResume() {
        super.onResume();
    }

}
