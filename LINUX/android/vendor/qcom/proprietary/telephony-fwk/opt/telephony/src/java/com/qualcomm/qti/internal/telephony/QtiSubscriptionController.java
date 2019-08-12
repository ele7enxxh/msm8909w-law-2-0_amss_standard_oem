/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution, Apache license notifications and license are retained
 * for attribution purposes only.
 *
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


package com.qualcomm.qti.internal.telephony;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Binder;
import android.os.SystemProperties;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.telecom.PhoneAccount;
import android.telecom.PhoneAccountHandle;
import android.telecom.TelecomManager;
import android.telephony.Rlog;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.SubscriptionController;

import java.util.Map.Entry;
import java.util.Iterator;
import java.util.List;

/*
 * Extending SubscriptionController here:
 * 1. To override few methods like getPhoneId()/getSlotId() to return
 *    DUMMY subId values in NO SIM cases.
 * 2. To implement fall back of sms/data user preferred subId value to next
 *    available subId when current preferred SIM deactivated or removed.
 */
public class QtiSubscriptionController extends SubscriptionController {
    static final String LOG_TAG = "QtiSubscriptionController";

    private static CommandsInterface[] sCi = null;
    private static int sNumPhones;

    // When no valid SIM cards present on device, framework returns DUMMY subIds
    // with range starting from DUMMY_SUB_ID_BASE.
    private static final int DUMMY_SUB_ID_BASE = SubscriptionManager.MAX_SUBSCRIPTION_ID_VALUE
        - PhoneConstants.MAX_PHONE_COUNT_TRI_SIM;
    private static final int PROVISIONED = 1;
    private static final int NOT_PROVISIONED = 0;

    private static final String APM_SIM_NOT_PWDN_PROPERTY = "persist.radio.apm_sim_not_pwdn";

    private TelecomManager mTelecomManager;
    private TelephonyManager mTelephonyManager;

    private static final String SETTING_USER_PREF_DATA_SUB = "user_preferred_data_sub";
    /**
     * This intent would be broadcasted when a subId/slotId pair added to the
     * sSlotIdxToSubId hashmap.
     */
    private static final String ACTION_SUBSCRIPTION_RECORD_ADDED =
            "org.codeaurora.intent.action.SUBSCRIPTION_INFO_RECORD_ADDED";

    public static QtiSubscriptionController init(Context c, CommandsInterface[] ci) {
        synchronized (QtiSubscriptionController.class) {
            if (sInstance == null) {
                sInstance = new QtiSubscriptionController(c);
                sCi = ci;
                sNumPhones = TelephonyManager.getDefault().getPhoneCount();
            } else {
                Log.wtf(LOG_TAG, "init() called multiple times!  sInstance = " + sInstance);
            }
            return (QtiSubscriptionController)sInstance;
        }
    }

    public static QtiSubscriptionController getInstance() {
        if (sInstance == null) {
           Log.wtf(LOG_TAG, "getInstance null");
        }

        return (QtiSubscriptionController)sInstance;
    }

    private QtiSubscriptionController(Context c) {
        super(c);
        if (DBG) logd(" init by Context");

        mDefaultPhoneId = 0;
        mDefaultFallbackSubId = DUMMY_SUB_ID_BASE;
        mTelecomManager = TelecomManager.from(mContext);
        mTelephonyManager = TelephonyManager.from(mContext);
    }

    private void clearVoiceSubId() {
        final List<SubscriptionInfo> records = getActiveSubscriptionInfoList(
                mContext.getOpPackageName());
        if (DBG) logdl("[clearVoiceSubId] records: " + records);
        if (shouldDefaultBeCleared(records, getDefaultVoiceSubId())) {
            if (DBG) logdl("[clearVoiceSubId] clear voice sub id");
            setDefaultVoiceSubId(DUMMY_SUB_ID_BASE);
        }
    }

    @Override
    public int getSlotId(int subId) {
        if (VDBG) printStackTrace("[getSlotId] subId=" + subId);

        if (subId == SubscriptionManager.DEFAULT_SUBSCRIPTION_ID) {
            subId = getDefaultSubId();
        }
        if (!SubscriptionManager.isValidSubscriptionId(subId)) {
            if (DBG) logd("[getSlotId]- subId invalid");
            return SubscriptionManager.INVALID_SIM_SLOT_INDEX;
        }

        if (subId >= DUMMY_SUB_ID_BASE) {
            logd("getPhoneId, received dummy subId " + subId);
            return getPhoneIdFromDummySubId(subId);
        }
        return super.getSlotId(subId);
    }

    @Override
    public int getPhoneId(int subId) {
        if (VDBG) printStackTrace("[getPhoneId] subId=" + subId);
        int phoneId;

        if (subId == SubscriptionManager.DEFAULT_SUBSCRIPTION_ID) {
            subId = getDefaultSubId();
            if (DBG) logdl("[getPhoneId] asked for default subId=" + subId);
        }

        if (!SubscriptionManager.isValidSubscriptionId(subId)) {
            if (DBG) {
                logdl("[getPhoneId]- invalid subId return="
                        + SubscriptionManager.INVALID_PHONE_INDEX);
            }
            return SubscriptionManager.INVALID_PHONE_INDEX;
        }

        if (subId >= DUMMY_SUB_ID_BASE) {
            logd("getPhoneId, received dummy subId " + subId);
            return getPhoneIdFromDummySubId(subId);
        }

        return super.getPhoneId(subId);
    }

    private int getPhoneIdFromDummySubId(int subId) {
        return subId - DUMMY_SUB_ID_BASE;
    }

    @Override
    // Override this method to change dummy subId base value to DUMMY_SUB_ID_BASE
    protected int[] getDummySubIds(int slotIdx) {
        int numSubs = getActiveSubInfoCountMax();
        if (numSubs > 0) {
            int[] dummyValues = new int[numSubs];
            for (int i = 0; i < numSubs; i++) {
                dummyValues[i] = DUMMY_SUB_ID_BASE + slotIdx;
            }
            if (VDBG) {
                logd("getDummySubIds: slotIdx=" + slotIdx
                    + " return " + numSubs + " DummySubIds with each subId=" + dummyValues[0]);
            }
            return dummyValues;
        } else {
            return null;
        }
    }

    @Override
    public int addSubInfoRecord(String iccId, int slotId) {

        int retVal = super.addSubInfoRecord(iccId, slotId);

        int[] subId = getSubId(slotId);
        if (subId != null && (subId.length > 0)) {
            // When a new entry added in sSlotIdxToSubId for slotId, broadcast intent
            logd("addSubInfoRecord: broadcast intent subId[" + slotId + "] = " + subId[0]);
            Intent intent = new Intent(ACTION_SUBSCRIPTION_RECORD_ADDED);
            SubscriptionManager.putPhoneIdAndSubIdExtra(intent, slotId, subId[0]);
            mContext.sendBroadcast(intent);
        }
        return retVal;
    }

    @Override
    public void clearDefaultsForInactiveSubIds() {
        enforceModifyPhoneState("clearDefaultsForInactiveSubIds");

        // Now that all security checks passes, perform the operation as ourselves.
        final long identity = Binder.clearCallingIdentity();
        try {
            final List<SubscriptionInfo> records = getActiveSubscriptionInfoList(
                    mContext.getOpPackageName());
            if (DBG) logdl("[clearDefaultsForInactiveSubIds] records: " + records);
            if (shouldDefaultBeCleared(records, getDefaultDataSubId())) {
                if (DBG) logd("[clearDefaultsForInactiveSubIds] clearing default data sub id");
                setDefaultDataSubId(SubscriptionManager.INVALID_SUBSCRIPTION_ID);
            }
            if (shouldDefaultBeCleared(records, getDefaultSmsSubId())) {
                if (DBG) logdl("[clearDefaultsForInactiveSubIds] clearing default sms sub id");
                setDefaultSmsSubId(SubscriptionManager.INVALID_SUBSCRIPTION_ID);
            }
            if (shouldDefaultBeCleared(records, getDefaultVoiceSubId())) {
                if (DBG) logdl("[clearDefaultsForInactiveSubIds] clearing default voice sub id");
                setDefaultVoiceSubId(DUMMY_SUB_ID_BASE);
            }
        } finally {
            Binder.restoreCallingIdentity(identity);
        }
    }

    @Override
    protected boolean shouldDefaultBeCleared(List<SubscriptionInfo> records, int subId) {
        if (DBG) logdl("[shouldDefaultBeCleared: subId] " + subId);
        if (records == null) {
            if (DBG) logdl("[shouldDefaultBeCleared] return true no records subId=" + subId);
            return true;
        }
        if (!SubscriptionManager.isValidSubscriptionId(subId)) {
            // If the subId parameter is not valid its already cleared so return false.
            if (DBG) logdl("[shouldDefaultBeCleared] return false only one subId, subId=" + subId);
            return false;
        }
        for (SubscriptionInfo record : records) {
            int id = record.getSubscriptionId();
            if (DBG) logdl("[shouldDefaultBeCleared] Record.id: " + id);
            if (id == subId) {
                logdl("[shouldDefaultBeCleared] return false subId is active, subId=" + subId);
                return false;
            }
        }
        int slotId = getSlotId(subId);
        if (getUiccProvisionStatus(slotId) == PROVISIONED) {
            logdl("[shouldDefaultBeCleared] return false subId is provisioned, subId=" + subId);
            return false;
        }
        if (DBG) logdl("[shouldDefaultBeCleared] return true not active subId=" + subId);
        return true;
    }

    private boolean isRadioAvailableOnAllSubs() {
        for (int i = 0; i < sNumPhones; i++) {
            if (sCi != null && !sCi[i].getRadioState().isAvailable()) return false;
        }
        return true;
    }

    private boolean isShuttingDown() {
        for (int i = 0; i < sNumPhones; i++) {
            if (sPhones[i] != null && sPhones[i].isShuttingDown()) return true;
        }
        return false;
    }

    public boolean isRadioInValidState() {
        boolean isApmSimNotPwrDown = (SystemProperties.getInt(APM_SIM_NOT_PWDN_PROPERTY, 0) == 1);
        int isAPMOn = Settings.Global.getInt(mContext.getContentResolver(),
                Settings.Global.AIRPLANE_MODE_ON, 0);

        // Do not updateUserPrefs, SIM is pwdn due to APM, as apm_sim_not_pwdn flag is not set.
        if ((isAPMOn == 1) && (!isApmSimNotPwrDown)) {
            logd("isRadioInValidState, isApmSimNotPwrDown = " + isApmSimNotPwrDown
                    + ", isAPMOn:" + isAPMOn);
            return false;
        }

        // Radio Unavailable, do not updateUserPrefs. As this may happened due to SSR or RIL Crash.
        if (!isRadioAvailableOnAllSubs()) {
            logd(" isRadioInValidState, radio not available");
            return false;
        }

        //Do not updateUserPrefs when Shutdown is in progress
        if (isShuttingDown()) {
            logd(" isRadioInValidState: device shutdown in progress ");
            return false;
        }
        return true;
    }

    // 1. Sets DUMMY subId value to voice/data/sms preferences when NO SIM
    //    present OR no SIM in provisioned state.
    // 2. If any of the voice/data/sms preference related SIM
    //    deactivated/re-activated this will update the preference
    //    with next available/activated SIM.
    synchronized void updateUserPreferences() {
        SubscriptionInfo mNextActivatedSub = null;
        int activeCount = 0;
        if (!isRadioInValidState()) {
            logd("Radio is in Invalid state, Ignore Updating User Preference!!!");
            return;
        }
        List<SubscriptionInfo> sil = getActiveSubscriptionInfoList(mContext.getOpPackageName());
        // If list of active subscriptions empty OR non of the SIM provisioned
        // clear defaults preference of voice/sms/data.
        if (sil == null || sil.size() < 1) {
            logi("updateUserPreferences: Subscription list is empty");
            // If no SIM cards are present on the device, set dummy subId
            // as the default sub id.
            clearVoiceSubId();
            setDefaultFallbackSubId(DUMMY_SUB_ID_BASE);
            return;
        }

        // Do not fallback to next available sub if AOSP feature
        // "User choice of selecting data/sms fallback preference" enabled.
        if (SystemProperties.getBoolean("persist.radio.aosp_usr_pref_sel", false)) {
            logi("updateUserPreferences: AOSP user preference option enabled ");
            return;
        }

        //Get num of activated Subs and next available activated sub info.
        for (SubscriptionInfo subInfo : sil) {
            if (getUiccProvisionStatus(subInfo.getSimSlotIndex()) == PROVISIONED) {
                activeCount++;
                if (mNextActivatedSub == null) mNextActivatedSub = subInfo;
            }
        }
        logd("updateUserPreferences:: active sub count = " + activeCount + " dds = "
                 + getDefaultDataSubId() + " voice = " + getDefaultVoiceSubId() +
                 " sms = " + getDefaultSmsSubId());

        // If active SUB count is 1, Always Ask Prompt to be disabled and
        // preference fallback to the next available SUB.
        if (activeCount == 1) {
            setSMSPromptEnabled(false);
        }

        // TODO Set all prompt options to false ?

        // in Single SIM case or if there are no activated subs available, no need to update. EXIT.
        if ((mNextActivatedSub == null) || (getActiveSubInfoCountMax() == 1)) return;

        handleDataPreference(mNextActivatedSub.getSubscriptionId());

        if (!isSubProvisioned(getDefaultSmsSubId())) {
            setDefaultSmsSubId(mNextActivatedSub.getSubscriptionId());
        }
        if (!isSubProvisioned(getDefaultVoiceSubId())) {
            setDefaultVoiceSubId(mNextActivatedSub.getSubscriptionId());
        }

        // voice preference is handled in such a way that
        // 1. Whenever current Sub is deactivated or removed It fall backs to
        //    next available Sub.
        // 2. When device is flashed for the first time, initial voice preference
        //    would be set to always ask.
        if (!isNonSimAccountFound() && activeCount == 1) {
            final int subId = mNextActivatedSub.getSubscriptionId();
            PhoneAccountHandle phoneAccountHandle = subscriptionIdToPhoneAccountHandle(subId);
            logi("set default phoneaccount to  " + subId);
            mTelecomManager.setUserSelectedOutgoingPhoneAccount(phoneAccountHandle);
        }
        if (!isSubProvisioned(mDefaultFallbackSubId)) {
            setDefaultFallbackSubId(mNextActivatedSub.getSubscriptionId());
        }

        notifySubscriptionInfoChanged();
        logd("updateUserPreferences: after currentDds = " + getDefaultDataSubId() + " voice = " +
                 getDefaultVoiceSubId() + " sms = " + getDefaultSmsSubId());
    }

    private void handleDataPreference(int nextActiveSubId) {
        int userPrefDataSubId = getUserPrefDataSubIdFromDB();
        int currentDataSubId = getDefaultDataSubId();

        List<SubscriptionInfo> subInfoList =
                getActiveSubscriptionInfoList(mContext.getOpPackageName());
        if (subInfoList == null) {
            return;
        }
        boolean userPrefSubValid = false;
        for (SubscriptionInfo subInfo : subInfoList) {
            if (subInfo.getSubscriptionId() == userPrefDataSubId) {
                 userPrefSubValid = true;
            }
        }
        logd("havePrefSub = " + userPrefSubValid + " user pref subId = "
                 + userPrefDataSubId + " current dds " + currentDataSubId
                 + " next active subId " + nextActiveSubId);

        // If earlier user selected DDS is now available, set that as DDS subId.
        if (userPrefSubValid && isSubProvisioned(userPrefDataSubId) &&
                 (currentDataSubId != userPrefDataSubId)) {
            setDefaultDataSubId(userPrefDataSubId);
        } else if (!isSubProvisioned(currentDataSubId)) {
            setDefaultDataSubId(nextActiveSubId);
        }

        // Check and set DDS after sub activation
        QtiRadioCapabilityController radioCapController =
                QtiRadioCapabilityController.getInstance();
        radioCapController.setDdsIfRequired(false);
    }

    private int getUiccProvisionStatus(int slotId) {
        QtiUiccCardProvisioner uiccCardProvisioner = QtiUiccCardProvisioner.getInstance();
        if (uiccCardProvisioner != null) {
            return uiccCardProvisioner.getCurrentUiccCardProvisioningStatus(slotId);
        } else {
            return NOT_PROVISIONED;
        }
    }

    // This method returns true if subId and corresponding slotId is in valid
    // range and the Uicc card corresponds to this slot is provisioned.
    private boolean isSubProvisioned(int subId) {
        boolean isSubIdUsable = SubscriptionManager.isUsableSubIdValue(subId);

        if (isSubIdUsable) {
            int slotId = getSlotId(subId);
            if (!SubscriptionManager.isValidSlotId(slotId) || (subId >= DUMMY_SUB_ID_BASE)) {
                loge(" Invalid slotId " + slotId + " or subId = " + subId);
                isSubIdUsable = false;
            } else {
                if (getUiccProvisionStatus(slotId) != PROVISIONED) {
                    isSubIdUsable = false;
                }
                loge("isSubProvisioned, state = " + isSubIdUsable + " subId = " + subId);
            }
        }
        return isSubIdUsable;
    }

    /* Returns User SMS Prompt property,  enabled or not */
    public boolean isSMSPromptEnabled() {
        boolean prompt = false;
        int value = 0;
        try {
            value = Settings.Global.getInt(mContext.getContentResolver(),
                    Settings.Global.MULTI_SIM_SMS_PROMPT);
        } catch (SettingNotFoundException snfe) {
            loge("Settings Exception Reading Dual Sim SMS Prompt Values");
        }
        prompt = (value == 0) ? false : true ;
        if (VDBG) logd("SMS Prompt option:" + prompt);

       return prompt;
    }

    /*Sets User SMS Prompt property,  enable or not */
    public void setSMSPromptEnabled(boolean enabled) {
        enforceModifyPhoneState("setSMSPromptEnabled");
        int value = (enabled == false) ? 0 : 1;
        Settings.Global.putInt(mContext.getContentResolver(),
                Settings.Global.MULTI_SIM_SMS_PROMPT, value);
        logi("setSMSPromptOption to " + enabled);
    }

    private boolean isNonSimAccountFound() {
        final Iterator<PhoneAccountHandle> phoneAccounts =
                mTelecomManager.getCallCapablePhoneAccounts().listIterator();

        while (phoneAccounts.hasNext()) {
            final PhoneAccountHandle phoneAccountHandle = phoneAccounts.next();
            final PhoneAccount phoneAccount = mTelecomManager.getPhoneAccount(phoneAccountHandle);
            if (mTelephonyManager.getSubIdForPhoneAccount(phoneAccount) ==
                            SubscriptionManager.INVALID_SUBSCRIPTION_ID) {
                logi("Other than SIM account found. ");
                return true;
            }
        }
        logi("Other than SIM account not found ");
        return false;
    }

    private PhoneAccountHandle subscriptionIdToPhoneAccountHandle(final int subId) {
        final Iterator<PhoneAccountHandle> phoneAccounts =
                mTelecomManager.getCallCapablePhoneAccounts().listIterator();

        while (phoneAccounts.hasNext()) {
            final PhoneAccountHandle phoneAccountHandle = phoneAccounts.next();
            final PhoneAccount phoneAccount = mTelecomManager.getPhoneAccount(phoneAccountHandle);
            if (subId == mTelephonyManager.getSubIdForPhoneAccount(phoneAccount)) {
                return phoneAccountHandle;
            }
        }

        return null;
    }

    private int getUserPrefDataSubIdFromDB() {
        return android.provider.Settings.Global.getInt(mContext.getContentResolver(),
                SETTING_USER_PREF_DATA_SUB, SubscriptionManager.INVALID_SUBSCRIPTION_ID);
    }

    private void logd(String string) {
        if (DBG) Rlog.d(LOG_TAG, string);
    }

    private void logi(String string) {
        Rlog.i(LOG_TAG, string);
    }

    private void loge(String string) {
        Rlog.e(LOG_TAG, string);
    }
}
