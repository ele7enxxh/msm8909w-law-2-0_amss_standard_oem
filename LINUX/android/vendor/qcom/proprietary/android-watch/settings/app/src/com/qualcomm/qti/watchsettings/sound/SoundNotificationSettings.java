/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */

/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
package com.qualcomm.qti.watchsettings.sound;

import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.database.ContentObserver;
import android.database.Cursor;
import android.database.sqlite.SQLiteException;
import android.media.AudioManager;
import android.media.RingtoneManager;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.preference.PreferenceFragment;
import android.preference.Preference;
import android.preference.PreferenceScreen;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.TwoStatePreference;
import android.preference.PreferenceCategory;
import android.provider.MediaStore;
import android.preference.SeekBarVolumizer;
import android.provider.OpenableColumns;
import android.provider.SearchIndexableResource;
import android.provider.Settings;
import android.text.TextUtils;
import android.util.Log;

import com.qualcomm.qti.watchsettings.R;
import com.qualcomm.qti.watchsettings.sound.SoundRingtonePreference;
import com.qualcomm.qti.watchsettings.sound.search.SoundBaseSearchIndexProvider;
import com.qualcomm.qti.watchsettings.sound.search.SoundIndexable;
//import com.qualcomm.qti.watchsettings.sound.SoundRingtonePreference;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;


public class SoundNotificationSettings  extends PreferenceFragment implements SoundIndexable{
private static final String TAG = "WatchSoundNotificationSettings";

    private static final String KEY_SOUND = "sound";
    private static final String KEY_MEDIA_VOLUME = "media_volume";
    private static final String KEY_ALARM_VOLUME = "alarm_volume";
    private static final String KEY_RING_VOLUME = "ring_volume";
    private static final String KEY_NOTIFICATION_VOLUME = "notification_volume";
    private static final String KEY_PHONE_RINGTONE = "ringtone";
    private static final String KEY_VIBRATE_WHEN_RINGING = "vibrate_when_ringing";
    private static final String SELECTED_PREFERENCE_KEY = "selected_preference";
    private static final int REQUEST_CODE = 200;
    private static final int SAMPLE_CUTOFF = 2000;  // manually cap sample playback at 2 seconds

    private final SoundVolumePreferenceCallback mVolumeCallback = new SoundVolumePreferenceCallback();
    private final H mHandler = new H();
    private final ArrayList<SoundVolumeSeekBarPreference> mVolumePrefs = new ArrayList<>();

    private Context mContext;
    private boolean mVoiceCapable;
    private AudioManager mAudioManager;
    private SoundVolumeSeekBarPreference mRingOrNotificationPreference;
    private Preference mPhoneRingtonePreference;
    private TwoStatePreference mVibrateWhenRinging;
    private SoundRingtonePreference mRequestPreference;
    private int mRingerMode = -1;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mContext = getActivity();
        mVoiceCapable = SoundUtils.isVoiceCapable(mContext);

        mAudioManager = (AudioManager) mContext.getSystemService(Context.AUDIO_SERVICE);

        addPreferencesFromResource(R.xml.sound_activity);

        final PreferenceCategory sound = (PreferenceCategory) findPreference(KEY_SOUND);
        initVolumePreference(KEY_MEDIA_VOLUME, AudioManager.STREAM_MUSIC,
                com.android.internal.R.drawable.ic_audio_vol_mute);
        initVolumePreference(KEY_ALARM_VOLUME, AudioManager.STREAM_ALARM,
                com.android.internal.R.drawable.ic_audio_alarm_mute);
        if (mVoiceCapable) {
            mRingOrNotificationPreference =
                    initVolumePreference(KEY_RING_VOLUME, AudioManager.STREAM_RING,
                            com.android.internal.R.drawable.ic_audio_ring_notif_mute);
            sound.removePreference(sound.findPreference(KEY_NOTIFICATION_VOLUME));
        } else {
            mRingOrNotificationPreference =
                    initVolumePreference(KEY_NOTIFICATION_VOLUME, AudioManager.STREAM_NOTIFICATION,
                            com.android.internal.R.drawable.ic_audio_ring_notif_mute);
            sound.removePreference(sound.findPreference(KEY_RING_VOLUME));
        }
	initRingtones();
        initVibrateWhenRinging();
        if (savedInstanceState != null) {
            String selectedPreference = savedInstanceState.getString(SELECTED_PREFERENCE_KEY, null);
            if (!TextUtils.isEmpty(selectedPreference)) {
                mRequestPreference = (SoundRingtonePreference) findPreference(selectedPreference);
            }
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        lookupRingtoneNames();
        for (SoundVolumeSeekBarPreference volumePref : mVolumePrefs) {
            volumePref.onActivityResume();
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        for (SoundVolumeSeekBarPreference volumePref : mVolumePrefs) {
            volumePref.onActivityPause();
        }
        mVolumeCallback.stopSample();
    }

    @Override
    public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        if (preference instanceof SoundRingtonePreference) {
            mRequestPreference = (SoundRingtonePreference) preference;
            mRequestPreference.onPrepareRingtonePickerIntent(mRequestPreference.getIntent());
            startActivityForResult(preference.getIntent(), REQUEST_CODE);
            return true;
        }

        return super.onPreferenceTreeClick(preferenceScreen, preference);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (mRequestPreference != null) {
            mRequestPreference.onActivityResult(requestCode, resultCode, data);
            mRequestPreference = null;
        }
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        if (mRequestPreference != null) {
            outState.putString(SELECTED_PREFERENCE_KEY, mRequestPreference.getKey());
        }
    }

    // === Volumes ===

    private SoundVolumeSeekBarPreference initVolumePreference(String key, int stream, int muteIcon) {
        final SoundVolumeSeekBarPreference volumePref = (SoundVolumeSeekBarPreference) findPreference(key);
        volumePref.setCallback(mVolumeCallback);
        volumePref.setStream(stream);
        mVolumePrefs.add(volumePref);
        volumePref.setMuteIcon(muteIcon);
        return volumePref;
    }


    private final class SoundVolumePreferenceCallback implements SoundVolumeSeekBarPreference.Callback {
        private SeekBarVolumizer mCurrent;

        @Override
        public void onSampleStarting(SeekBarVolumizer sbv) {
            if (mCurrent != null && mCurrent != sbv) {
                mCurrent.stopSample();
            }
            mCurrent = sbv;
            if (mCurrent != null) {
                mHandler.removeMessages(H.STOP_SAMPLE);
                mHandler.sendEmptyMessageDelayed(H.STOP_SAMPLE, SAMPLE_CUTOFF);
            }
        }

        @Override
        public void onStreamValueChanged(int stream, int progress) {
            // noop
        }

        public void stopSample() {
            if (mCurrent != null) {
                mCurrent.stopSample();
            }
        }
    };


    private final class H extends Handler {
        private static final int UPDATE_PHONE_RINGTONE = 1;
        private static final int STOP_SAMPLE = 3;

        private H() {
            super(Looper.getMainLooper());
        }

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
		case UPDATE_PHONE_RINGTONE:
                    mPhoneRingtonePreference.setSummary((CharSequence) msg.obj);
                    break;
                case STOP_SAMPLE:
                    mVolumeCallback.stopSample();
                    break;
            }
        }
    }


    // === Indexing ===

    public static final SoundBaseSearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
            new SoundBaseSearchIndexProvider() {

        public List<SearchIndexableResource> getXmlResourcesToIndex(
                Context context, boolean enabled) {
            final SearchIndexableResource sir = new SearchIndexableResource(context);
            sir.xmlResId = R.xml.sound_activity;
            return Arrays.asList(sir);
        }

        public List<String> getNonIndexableKeys(Context context) {
            final ArrayList<String> rt = new ArrayList<String>();
            if (SoundUtils.isVoiceCapable(context)) {
                rt.add(KEY_NOTIFICATION_VOLUME);
            } else {
                rt.add(KEY_RING_VOLUME);
                rt.add(KEY_PHONE_RINGTONE);
            }
            return rt;
        }
    };

    // === Phone & notification ringtone ===

    private void initRingtones() {
        mPhoneRingtonePreference = getPreferenceScreen().findPreference(KEY_PHONE_RINGTONE);
        if (mPhoneRingtonePreference != null && !mVoiceCapable) {
            getPreferenceScreen().removePreference(mPhoneRingtonePreference);
            mPhoneRingtonePreference = null;
        }
    }

    private void lookupRingtoneNames() {
        AsyncTask.execute(mLookupRingtoneNames);
    }

    private final Runnable mLookupRingtoneNames = new Runnable() {
        @Override
        public void run() {
            if (mPhoneRingtonePreference != null) {
                final CharSequence summary = updateRingtoneName(
                        mContext, RingtoneManager.TYPE_RINGTONE);
                if (summary != null) {
                    mHandler.obtainMessage(H.UPDATE_PHONE_RINGTONE, summary).sendToTarget();
                }
            }
        }
    };

    private static CharSequence updateRingtoneName(Context context, int type) {
        if (context == null) {
            Log.e(TAG, "Unable to update ringtone name, no context provided");
            return null;
        }
        Uri ringtoneUri = RingtoneManager.getActualDefaultRingtoneUri(context, type);
        CharSequence summary = context.getString(com.android.internal.R.string.ringtone_unknown);
        // Is it a silent ringtone?
        if (ringtoneUri == null) {
            summary = context.getString(com.android.internal.R.string.ringtone_silent);
        } else {
            Cursor cursor = null;
            try {
                if (MediaStore.AUTHORITY.equals(ringtoneUri.getAuthority())) {
                    // Fetch the ringtone title from the media provider
                    cursor = context.getContentResolver().query(ringtoneUri,
                            new String[] { MediaStore.Audio.Media.TITLE }, null, null, null);
                } else if (ContentResolver.SCHEME_CONTENT.equals(ringtoneUri.getScheme())) {
                    cursor = context.getContentResolver().query(ringtoneUri,
                            new String[] { OpenableColumns.DISPLAY_NAME }, null, null, null);
                }
                if (cursor != null) {
                    if (cursor.moveToFirst()) {
                        summary = cursor.getString(0);
                    }
                }
            } catch (SQLiteException sqle) {
                // Unknown title for the ringtone
            } catch (IllegalArgumentException iae) {
                // Some other error retrieving the column from the provider
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }
        }
        return summary;
    }
    // === Vibrate when ringing ===

    private void initVibrateWhenRinging() {
        mVibrateWhenRinging =
                (TwoStatePreference) getPreferenceScreen().findPreference(KEY_VIBRATE_WHEN_RINGING);
        if (mVibrateWhenRinging == null) {
            Log.i(TAG, "Preference not found: " + KEY_VIBRATE_WHEN_RINGING);
            return;
        }
        if (!mVoiceCapable) {
            getPreferenceScreen().removePreference(mVibrateWhenRinging);
            mVibrateWhenRinging = null;
            return;
        }
        mVibrateWhenRinging.setPersistent(false);
        updateVibrateWhenRinging();
        mVibrateWhenRinging.setOnPreferenceChangeListener((preference, newValue) -> {
            final boolean val = (Boolean) newValue;
                return Settings.System.putInt(mContext.getContentResolver(),
                        Settings.System.VIBRATE_WHEN_RINGING,
                        val ? 1 : 0);
        });
    }


    private void updateVibrateWhenRinging() {
        if (mVibrateWhenRinging == null) return;
        mVibrateWhenRinging.setChecked(Settings.System.getInt(mContext.getContentResolver(),
                Settings.System.VIBRATE_WHEN_RINGING, 0) != 0);
    }
}

