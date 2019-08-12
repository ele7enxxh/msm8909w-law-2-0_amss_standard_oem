/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data;


import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;

public class VoicePrintPreferences {

    private static final String PREFERENCE_NAME = "VoicePrintData";
    private static final String KEYPHRASES = "KEYPHRASES";
    private static final String LAST_KEYPHRASE_SELECTED = "LAST_KEYPHRASE_SELECTED";
    private static String DELIMITER = "=";
    private static final String TAG = "VoicePrintPreferences";


    public static String getLastSelectedKeyPhrase(Context context) {
        SharedPreferences settings = PreferenceManager
                .getDefaultSharedPreferences(context);
        return settings.getString(LAST_KEYPHRASE_SELECTED, null);
    }

    public static void saveLastSelectedKeyPhrase(Context context, String id) {
        SharedPreferences settings = PreferenceManager
                .getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = settings.edit();
        editor.putString(LAST_KEYPHRASE_SELECTED, id).apply();
    }


}
