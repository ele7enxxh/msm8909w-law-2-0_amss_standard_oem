/******************************************************************************
 * @file    AudioSphereFeatures.java
 * @brief   Provides the settings to enable/disable Audio Sphere related
 *          features
 *
 *
 * ---------------------------------------------------------------------------
 *  Copyright (c) 2015 Qualcomm Technologies, Inc.
 *  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
 *  ---------------------------------------------------------------------------
 *  ******************************************************************************/
package com.qualcomm.qualcommsettings;

import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.provider.Settings;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.CheckBoxPreference;
import android.preference.EditTextPreference;
import android.widget.EditText;
import android.widget.Toast;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.media.AudioManager;
import android.os.SystemProperties;
import android.util.Log;
import android.os.FileObserver;
import android.content.BroadcastReceiver;
import android.text.TextUtils;
import java.io.File;


public class AudioSphereFeatures extends PreferenceActivity implements
        OnSharedPreferenceChangeListener {

   private static final String TAG = "AudioSphereFeatures";
   private static final boolean DEBUG = false;
   private static final String ASPHERE_EVENT_NODE = "/data/misc/audio_pp/event_node";
   private static final String ASPHERE_SHARED_PREF_NAME = "com.qualcomm.qti.qualcommsettings.asphere_features";
   private static final String ASPHERE_PARAM_STATUS = "asphere_status";
   private static final String ASPHERE_PARAM_ENABLE = "asphere_enable";
   private static final String ASPHERE_PARAM_STRENGTH = "asphere_strength";
   private static final String ASPHERE_KEY_STATUS = "asphere_status";
   private static final String ASPHERE_KEY_ENABLE = "asphere_enable";
   private static final String ASPHERE_KEY_STRENGTH = "asphere_strength";
   private static final String ASPHERE_PARAM_VALUE_ENABLED = "1";
   private static final String ASPHERE_PARAM_VALUE_DISABLED = "0";;
   private static final int ASPHERE_STRENGTH_MIN = 0;
   private static final int ASPHERE_STRENGTH_MAX = 1000;
   private final int ASPHERE_ACTIVE = 0;

   private static Object mLock = new Object();

   private SharedPreferences mAsphereSharedPrefs = null;
   private CheckBoxPreference mAspherePrefEnable = null;
   private EditTextPreference mAspherePrefStrength = null;

   private Context mContext = null;
   private AudioManager mAudioManager = null;
   private FileObserver mAsphereFileEventObserver = null;

   /** Called when the activity is first created. */
   @Override
   public void onCreate(Bundle savedInstanceState) {
       super.onCreate(savedInstanceState);
       Log.v(TAG, "onCreate");
       mContext = getApplicationContext();
       mAudioManager = (AudioManager)getSystemService(Context.AUDIO_SERVICE);
       mAsphereSharedPrefs = getSharedPreferences(ASPHERE_SHARED_PREF_NAME, Context.MODE_PRIVATE);

       addPreferencesFromResource(R.xml.audio_sphere_features);
       mAspherePrefEnable = (CheckBoxPreference)findPreference(ASPHERE_KEY_ENABLE);
       mAspherePrefStrength = (EditTextPreference) findPreference(ASPHERE_KEY_STRENGTH);

       mAspherePrefEnable.setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {
           public boolean onPreferenceChange(Preference preference, Object newValue) {
               if (DEBUG) {
                   Log.v(TAG, "onPreferenceChange(Enable):" + newValue.toString());
               }
               boolean enable = newValue.toString().equals("true");
               int status = mAsphereSharedPrefs.getInt(ASPHERE_KEY_STATUS, ASPHERE_ACTIVE);
               if (status != ASPHERE_ACTIVE)
                   mAspherePrefEnable.setSummary((enable ? "Enabled" : "Disabled") + "(Suspended)");
               else
                   mAspherePrefEnable.setSummary(enable ? "Enabled" : "Disabled");

               synchronized(mLock) {
                   if (enable != mAsphereSharedPrefs.getBoolean(ASPHERE_KEY_ENABLE, false)) {
                       SharedPreferences.Editor editor = mAsphereSharedPrefs.edit();
                       editor.putBoolean(ASPHERE_KEY_ENABLE, enable);
                       editor.commit();
                   }
               }
               return true;
           }
       });

       mAspherePrefStrength.setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {
           public boolean onPreferenceChange(Preference preference, Object newValue) {
               if (DEBUG) {
                   Log.v(TAG, "onPreferenceChange(Strength): " + newValue.toString());
               }
               String newStrengthStr = newValue.toString();
               boolean ret = false;
               int newStrength = -1;
               if (newStrengthStr != null) {
                   try {
                       newStrength = Integer.parseInt(newStrengthStr);
                   } catch (NumberFormatException e) {
                       Log.e(TAG, "onPreferenceChange(Strength): error parsing input:" + newStrengthStr);
                   }
                   if (newStrength < ASPHERE_STRENGTH_MIN || newStrength > ASPHERE_STRENGTH_MAX) {
                       String msg = "\t\tError setting ASphere strength!!!\n" +
                            "Strength should be Integer within " +
                            ASPHERE_STRENGTH_MIN + " to " + ASPHERE_STRENGTH_MAX;
                       showMsg(msg);
                   } else {
                       ret = true;
                   }
               }
               synchronized(mLock) {
                   if (ret == true &&
                       newStrength != mAsphereSharedPrefs.getInt(ASPHERE_KEY_STRENGTH, ASPHERE_STRENGTH_MAX)) {
                       mAspherePrefStrength.setSummary("value set is: " + newStrengthStr +
                                  ", range is: " + ASPHERE_STRENGTH_MIN + " to " + ASPHERE_STRENGTH_MAX);
                       SharedPreferences.Editor editor = mAsphereSharedPrefs.edit();
                       editor.putInt(ASPHERE_KEY_STRENGTH, newStrength);
                       editor.commit();
                   }
               }
               return ret;
           }
       });

       if (DEBUG) {
           File f = new File(ASPHERE_EVENT_NODE);
           Log.v(TAG, "onCreate: " + ASPHERE_EVENT_NODE + " exists :" + f.exists() + ", can read: " + f.canRead());
       }
       mAsphereFileEventObserver = new FileObserver(ASPHERE_EVENT_NODE, FileObserver.OPEN) {
           @Override
           public void onEvent(int event, String file) {
                checkAsphereParamForUpdate();
           }
       };

       mAsphereSharedPrefs.registerOnSharedPreferenceChangeListener(this);
   }

   @Override
   protected void onResume() {
       super.onResume();
       Log.v(TAG, "onResume");
       checkAsphereParamForUpdate();
   }

   @Override
   protected void onStart() {
       super.onStart();
       if (DEBUG) {
           Log.v(TAG, "onStart");
       }
       if (mAsphereFileEventObserver != null)
           mAsphereFileEventObserver.startWatching();
   }

   @Override
   protected void onStop() {
       super.onStop();
       if (mAsphereFileEventObserver != null)
           mAsphereFileEventObserver.stopWatching();
   }

   public void checkAsphereParamForUpdate() {
       synchronized(mLock) {
           boolean enableObtained = false;
           int statusObtained = ASPHERE_ACTIVE, strengthObtained = 0;
           int statusStored = mAsphereSharedPrefs.getInt(ASPHERE_KEY_STATUS, ASPHERE_ACTIVE);
           boolean enableStored = mAsphereSharedPrefs.getBoolean(ASPHERE_KEY_ENABLE, false);;
           int strengthStored = mAsphereSharedPrefs.getInt(ASPHERE_KEY_STRENGTH, ASPHERE_STRENGTH_MAX);;

           // fetch values for all the params
           String paramResp = mAudioManager.getParameters(ASPHERE_PARAM_STATUS + ";" +
                              ASPHERE_PARAM_ENABLE + ";" + ASPHERE_PARAM_STRENGTH);
           if (paramResp != null) {
               Log.v(TAG, "checkAsphereParamForUpdate: " + paramResp);
               String[] splitParams = paramResp.split(";", 3);
               for (int i = 0; i < splitParams.length; i++) {
                    if (splitParams[i].startsWith(ASPHERE_PARAM_STATUS + "="))
                        statusObtained = Integer.parseInt(splitParams[i].substring(splitParams[i].indexOf("=") + 1));
                    else if (splitParams[i].startsWith(ASPHERE_PARAM_ENABLE + "="))
                        enableObtained = splitParams[i].substring(splitParams[i].indexOf("=") + 1).equals(ASPHERE_PARAM_VALUE_ENABLED);
                    else if(splitParams[i].startsWith(ASPHERE_PARAM_STRENGTH + "="))
                        strengthObtained = Integer.parseInt(splitParams[i].substring(splitParams[i].indexOf("=") + 1));
               }
           }

           // set params again, if value stored with preferences is not same
           String param = null;
           if (enableStored != enableObtained)
               param = new String(ASPHERE_PARAM_ENABLE + "=" +
                             (enableStored ? ASPHERE_PARAM_VALUE_ENABLED : ASPHERE_PARAM_VALUE_DISABLED));
           if (strengthStored != strengthObtained) {
               if (param != null)
                   param += ";";
               param += new String(ASPHERE_PARAM_STRENGTH + "=" + Integer.toString(strengthStored));
           }
           if (param != null)
               mAudioManager.setParameters(param);

           // update value of status stored with preferences if its not same
           if (statusStored != statusObtained) {
               SharedPreferences.Editor editor = mAsphereSharedPrefs.edit();
               editor.putInt(ASPHERE_KEY_STATUS, statusObtained);
               editor.commit();
           }
       }
   }

   private void onAsphereStatusChanged() {
       int status = mAsphereSharedPrefs.getInt(ASPHERE_KEY_STATUS, ASPHERE_ACTIVE);
       boolean enabled = mAsphereSharedPrefs.getBoolean(ASPHERE_KEY_ENABLE, false);
       if (status != ASPHERE_ACTIVE)
           mAspherePrefEnable.setSummary((enabled ? "Enabled" : "Disabled") + "(Suspended)");
        else
            mAspherePrefEnable.setSummary(enabled ? "Enabled" : "Disabled");
   }

   private void onAsphereEnableChanged() {
       boolean enable = mAsphereSharedPrefs.getBoolean(ASPHERE_KEY_ENABLE, false);
       mAudioManager.setParameters(ASPHERE_PARAM_ENABLE + "=" +
                          (enable ? ASPHERE_PARAM_VALUE_ENABLED : ASPHERE_PARAM_VALUE_DISABLED));
   }

   private void onAsphereStrengthChanged() {
       int strength = mAsphereSharedPrefs.getInt(ASPHERE_KEY_STRENGTH, ASPHERE_STRENGTH_MAX);
       mAudioManager.setParameters(ASPHERE_PARAM_STRENGTH + "=" + strength);
   }

   public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
       Log.v(TAG, "onSharedPreferenceChanged:" + key);
       if (key.equals(ASPHERE_KEY_ENABLE)) {
           onAsphereEnableChanged();
       } else if (key.equals(ASPHERE_KEY_STRENGTH)) {
           onAsphereStrengthChanged();
       } else if (key.equals(ASPHERE_KEY_STATUS)) {
           onAsphereStatusChanged();
       }
   }

   @Override
   protected void onDestroy() {
       super.onDestroy();
       mAsphereSharedPrefs.unregisterOnSharedPreferenceChangeListener(this);
   }

   private void showMsg(String msg) {
       final Toast toast = Toast.makeText(mContext, msg, Toast.LENGTH_LONG);
       toast.show();
   }

   public static class BootCompleteReceiver extends BroadcastReceiver {
       @Override
       public void onReceive(Context context, Intent intent) {
           if ("android.intent.action.BOOT_COMPLETED".equals(intent.getAction())) {
               Log.v(TAG, "Set AudioSphere params on Boot");
               synchronized(mLock) {
                   SharedPreferences asphereSharedPref =
                       context.getSharedPreferences(ASPHERE_SHARED_PREF_NAME, Context.MODE_PRIVATE);

                   int strength = asphereSharedPref.getInt(ASPHERE_KEY_STRENGTH, ASPHERE_STRENGTH_MAX);
                   boolean enabled = asphereSharedPref.getBoolean(ASPHERE_KEY_ENABLE, false);
                   String paramEnable = ASPHERE_PARAM_ENABLE + "=" +
                       (enabled ? ASPHERE_PARAM_VALUE_ENABLED : ASPHERE_PARAM_VALUE_DISABLED);
                   String paramStrength = ASPHERE_PARAM_STRENGTH + "=" + Integer.toString(strength);

                   AudioManager am = (AudioManager)context.getSystemService(Context.AUDIO_SERVICE);
                   am.setParameters(paramEnable + ";" + paramStrength);
               }
           } else {
               Log.e(TAG, "Received Unexpected Intent " + intent.toString());
           }
       }
   }
}

