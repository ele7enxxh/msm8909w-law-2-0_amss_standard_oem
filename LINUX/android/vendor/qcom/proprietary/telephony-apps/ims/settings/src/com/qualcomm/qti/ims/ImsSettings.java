/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (C) 2015, The Linux Foundation. All rights reserved.
 */

package com.qualcomm.qti.ims;

import android.content.Context;
import android.os.Bundle;
import android.preference.EditTextPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.preference.SwitchPreference;
import android.provider.Settings;
import android.telephony.SubscriptionManager;
import android.util.Log;
import android.view.MenuItem;
import android.widget.Toast;

import com.android.ims.ImsConfig;
import com.android.ims.ImsException;
import com.android.ims.ImsManager;

import org.codeaurora.ims.utils.QtiCallUtils;
import org.codeaurora.ims.QtiCallConstants;
import org.codeaurora.ims.QtiImsException;
import org.codeaurora.ims.QtiImsExtListenerBaseImpl;
import org.codeaurora.ims.QtiImsExtManager;
import org.codeaurora.ims.utils.QtiImsExtUtils;

/**
 * The activity class for editing a new or existing IMS profile.
 */
public class ImsSettings extends PreferenceActivity
        implements Preference.OnPreferenceChangeListener {

    private static final String TAG = ImsSettings.class.getSimpleName();
    public static final String SUB_ID_EXTRA =
            "com.android.phone.settings.SubscriptionInfoHelper.SubscriptionId";

    private ListPreference mVideoCallQuality;
    private ImsConfig mImsConfig;

    private PreferenceScreen mScreen = null;
    private Preference mPcPreference = null;
    private Preference mPerPreference = null;

    private EditTextPreference mDeflectNum = null;
    private SwitchPreference mButtonCsRetry = null;

    @Override
    public void onResume() {
        super.onResume();
        Log.d(TAG, "onResume");
        getVideoQuality();
        displayPcPerOptions();
        displayDeflectNumEditor();
        displayCsRetryOpions();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "onDestroy");

        mPcPreference = null;
        mPerPreference = null;
        mDeflectNum = null;
        mScreen = null;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "start ImsSettings");
        super.onCreate(savedInstanceState);

        addPreferencesFromResource(R.xml.ims_settings);
        mScreen = getPreferenceScreen();

        if (mScreen == null) {
            Log.e(TAG, "PreferenceScreen is invalid");
            return;
        }

        mVideoCallQuality = (ListPreference) mScreen
                .findPreference(getString(R.string.ims_vt_call_quality));
        mVideoCallQuality.setOnPreferenceChangeListener(this);

        mPcPreference = (Preference) mScreen.findPreference(getString(R.string.ims_pc_count));
        mPerPreference = (Preference) mScreen.findPreference(getString(R.string.ims_per_count));

        mButtonCsRetry = (SwitchPreference)mScreen.findPreference(
                getString(R.string.ims_to_cs_retry));

        try {
            int subId = getIntent().getIntExtra(SUB_ID_EXTRA,
                    SubscriptionManager.getDefaultVoiceSubscriptionId());
            Log.d(TAG, "subscription: " + subId);
            ImsManager imsManager = ImsManager.getInstance(getBaseContext(), subId);
            mImsConfig = imsManager.getConfigInterface();
        } catch (ImsException e) {
            mImsConfig = null;
            Log.e(TAG, "ImsService is not running");
        }

        mDeflectNum = (EditTextPreference) mScreen.findPreference(getString(
                                                   R.string.qti_ims_call_deflect));
        mDeflectNum.setOnPreferenceChangeListener(this);

        getActionBar().setDisplayHomeAsUpEnabled(true);
    }

    private void displayCsRetryOpions() {
        if (mScreen == null || mButtonCsRetry == null) {
            Log.e(TAG, "displayCsRetryOptions PreferenceScreen is invalid");
            return;
        }

        //remove the references by default and add them only when config is enabled
        mScreen.removePreference(mButtonCsRetry);
        mButtonCsRetry.setOnPreferenceChangeListener(null);

        //Enable CS Retry settings depending on CS Retry Config
        if (QtiImsExtUtils.isCsRetryConfigEnabled(getApplicationContext())) {
            mScreen.addPreference(mButtonCsRetry);
            mButtonCsRetry.setOnPreferenceChangeListener(this);
            mButtonCsRetry.setChecked(QtiCallUtils.isCsRetryEnabledByUser(
                    getApplicationContext()));
        }
    }

    private void displayPcPerOptions() {
        if (mScreen == null) {
            Log.e(TAG, "displayPcPerOptions PreferenceScreen is invalid");
            return;
        }

        //remove the references by default and add them only when config is enabled
        mScreen.removePreference(mPcPreference);
        mScreen.removePreference(mPerPreference);
        mPcPreference.setOnPreferenceClickListener(null);
        mPerPreference.setOnPreferenceClickListener(null);

        if (isRtpStatisticsQueryEnabled()) {
            //Show Packet Count/Packet Error Count UI only if the config is enabled
            mScreen.addPreference(mPcPreference);
            mScreen.addPreference(mPerPreference);
            mPcPreference.setOnPreferenceClickListener(prefClickListener);
            mPerPreference.setOnPreferenceClickListener(prefClickListener);
        }
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {  // See ActionBar#setDisplayHomeAsUpEnabled()
            onBackPressed();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    private void setCsRetry(boolean turnOn) {
        final int value = (turnOn) ? 1:0;
        android.provider.Settings.Global.putInt(
                  getApplicationContext().getContentResolver(),
                  QtiCallConstants.IMS_TO_CS_RETRY_ENABLED, value);
    }

    private void displayToast(String displayStr) {
        Toast.makeText(getApplicationContext(), displayStr, Toast.LENGTH_SHORT).show();
    }

    private QtiImsExtListenerBaseImpl QtiImsExtListener =
            new QtiImsExtListenerBaseImpl() {

        @Override
        public void onGetPacketCount(int status, long packetCount) {
            mPcPreference.setEnabled(true);
            String sPc = getApplicationContext().getResources().getString(
                    R.string.ims_get_packet_count_failed);
            if (hasRequestFailed(status)) {
                Log.e(TAG, "onGetPacketCount: get failed. errorCode = " + status);
            } else {
                Log.d(TAG, "onGetPacketCount: packetCount = " + packetCount);
                Long pc = new Long(packetCount);
                sPc = "packetCount = " + pc.toString();
            }
            displayToast(sPc);
        }

        @Override
        public void onGetPacketErrorCount(int status, long packetErrorCount) {
            mPerPreference.setEnabled(true);
            String sPec = getApplicationContext().getResources().getString(
                    R.string.ims_get_packet_error_count_failed);
            if (hasRequestFailed(status)) {
                Log.e(TAG, "onGetPacketErrorCount: get failed. errorCode = " + status);
            } else {
                Log.d(TAG, "onGetPacketErrorCount: packetErrorCount = " + packetErrorCount);
                Long pec = new Long(packetErrorCount);
                sPec = "packetErrorCount = " + pec.toString();
            }
            displayToast(sPec);
        }
    };

    private void onPcPrefClicked() {
        Log.d(TAG, "onPcPrefClicked");

        try {
            QtiImsExtManager.getInstance().getPacketCount(QtiImsExtListener);
            mPcPreference.setEnabled(false);
        } catch (QtiImsException e) {
            Log.e(TAG, "getPacketCount failed. Exception = " + e);
        }
    }

    private void onPerPrefClicked() {
        Log.d(TAG, "onPerPrefClicked");

        try {
            QtiImsExtManager.getInstance().getPacketErrorCount(QtiImsExtListener);
            mPerPreference.setEnabled(false);
        } catch (QtiImsException e) {
            Log.e(TAG, "getPacketErrorCount failed. Exception = " + e);
        }
    }

    Preference.OnPreferenceClickListener prefClickListener =
            new Preference.OnPreferenceClickListener() {
        @Override
        public boolean onPreferenceClick(Preference pref) {
            Log.d(TAG, "onPreferenceClick");
            if (pref.equals(mPcPreference)) {
                onPcPrefClicked();
            } else if (pref.equals(mPerPreference)) {
                onPerPrefClicked();
            }
            return true;
        }
    };

    private boolean isRtpStatisticsQueryEnabled() {
        return getApplicationContext().getResources().getBoolean(
                R.bool.config_ims_enable_rtp_statistics);
    }

    @Override
    public boolean onPreferenceChange(Preference pref, Object newValue) {
        if (pref.equals(mVideoCallQuality)) {
            if (newValue == null) {
                Log.e(TAG, "onPreferenceChange invalid value received");
            } else {
                final int quality = Integer.parseInt(newValue.toString());
                boolean result = setVideoQuality(quality);
                if (result) {
                    loadVideoCallQualityPrefs(quality);
                }
                return result;
            }
        } else if (pref.equals(mDeflectNum)) {
            if (newValue == null) {
                Log.e(TAG, "onPreferenceChange Deflect number invalid value received");
            } else {
                QtiImsExtUtils.setCallDeflectNumber(
                        getApplicationContext().getContentResolver(), newValue.toString());
                displayDeflectNumEditor();
            }
        } else if (pref.equals(mButtonCsRetry)) {
            SwitchPreference csRetryPref = (SwitchPreference)pref;
            csRetryPref.setChecked(!csRetryPref.isChecked());
            setCsRetry(csRetryPref.isChecked());
        }
        return true;
    }

    private void loadVideoCallQualityPrefs(int vqValue) {
        Log.d(TAG, "loadVideoCallQualityPrefs, vqValue = " + vqValue);
        final String videoQuality = videoQualityToString(vqValue);
        mVideoCallQuality.setValue(String.valueOf(vqValue));
        mVideoCallQuality.setSummary(videoQuality);
    }

    private void getVideoQuality() {
        int videoQuality = ImsConfig.OperationStatusConstants.UNKNOWN;
        try {
            if (mImsConfig != null) {
                videoQuality = mImsConfig.getProvisionedValue(
                        ImsConfig.ConfigConstants.VIDEO_QUALITY);
            }
        } catch (ImsException e) {
            Log.e(TAG, "getVideoQuality failed. Exception = " + e);
        }
        loadVideoCallQualityPrefs(videoQuality);
    }

    private boolean setVideoQuality(int quality) {
        int result = ImsConfig.OperationStatusConstants.FAILED;
        try {
            if (mImsConfig != null) {
                result = mImsConfig.setProvisionedValue(ImsConfig.ConfigConstants.VIDEO_QUALITY,
                        quality);
            }
        } catch (ImsException e) {
            Log.e(TAG, "setVideoQuality failed. Exception = " + e);
        }
        Log.d(TAG, "setVideoQuality, result = " + result);
        return !hasRequestFailed(result);
    }

    private static boolean hasRequestFailed(int result) {
        return (result != ImsConfig.OperationStatusConstants.SUCCESS);
    }

    private String videoQualityToString(int quality) {
        switch (quality) {
            case ImsConfig.VideoQualityFeatureValuesConstants.HIGH:
                return getString(R.string.ims_vt_call_quality_high);
            case ImsConfig.VideoQualityFeatureValuesConstants.LOW:
                return getString(R.string.ims_vt_call_quality_low);
            case ImsConfig.OperationStatusConstants.UNKNOWN:
            default:
                return getString(R.string.ims_vt_call_quality_unknown);
        }
    }

    private boolean isCallDeflectionOrTransferEnabled() {
        return getApplicationContext().getResources().getBoolean(
                R.bool.config_carrier_ims_call_deflect) ||
                QtiImsExtUtils.isCallTransferEnabled(getApplicationContext());
    }

    private void displayDeflectNumEditor() {
        if (mScreen == null || mDeflectNum == null) {
            Log.e(TAG, "displayDeflectNumEditor PreferenceScreen is invalid");
            return;
        }

        //remove the references by default and add them only when config is enabled
        mScreen.removePreference(mDeflectNum);
        mDeflectNum.setOnPreferenceChangeListener(null);

        //Enable Call deflection editor depending on platform support
        if (isCallDeflectionOrTransferEnabled()) {
            mScreen.addPreference(mDeflectNum);
            mDeflectNum.setOnPreferenceChangeListener(this);

            String number = QtiImsExtUtils.getCallDeflectNumber(
                    getApplicationContext().getContentResolver());
            if (number != null) {
                mDeflectNum.setText(number);
                mDeflectNum.setSummary(number);
            } else {
                mDeflectNum.setText("");
                mDeflectNum.setSummary(getString(R.string.qti_ims_number_not_set));
            }
        }
    }
}
