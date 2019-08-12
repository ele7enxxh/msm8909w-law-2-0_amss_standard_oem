/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qti.primarycardcontroller;

import android.app.AlertDialog;
import android.content.ActivityNotFoundException;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.DialogInterface.OnDismissListener;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.preference.PreferenceManager;
import android.provider.Settings;
import android.telephony.RadioAccessFamily;
import android.telephony.Rlog;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.Html;
import android.text.TextUtils;
import android.view.WindowManager;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.IccCardConstants;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.PhoneProxy;
import com.android.internal.telephony.SubscriptionController;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.uicc.IccCardApplicationStatus.AppState;
import com.android.internal.telephony.uicc.IccCardApplicationStatus.AppType;
import com.android.internal.telephony.uicc.IccCardStatus.CardState;
import com.android.internal.telephony.uicc.UiccCard;
import com.android.internal.telephony.uicc.UiccCardApplication;
import com.android.internal.telephony.uicc.UiccController;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class PrimarySubSelectionController extends Handler implements OnClickListener,
        OnDismissListener {
    private static final boolean DEBUG = true;
    static final String TAG = "PrimarySubSelectionController";
    static final String PRIMARY_CARD_PROPERTY_NAME = "persist.radio.primarycard";
    static final int PHONE_COUNT = TelephonyManager.getDefault().getPhoneCount();

    private static PrimarySubSelectionController sInstance;
    Phone[] mPhones;
    CardStateMonitor mCardStateMonitor;
    private boolean mAllCardsAbsent = true;
    private boolean mCardChanged = false;
    private boolean mNeedHandleModemReadyEvent = false;
    private boolean mRestoreDdsToPrimarySub = false;
    private boolean mRadioCapabilityAvailable = false;
    private boolean[] mIccLoaded;
    private boolean mIsCtCardPresent = false;
    private boolean mDetect4gCardEnabled = false;
    private boolean mShowSimSelectionDialog = false;
    private int mNumActiveSubs = 0;

    public static final String CONFIG_LTE_SUB_SELECT_MODE = "config_lte_sub_select_mode";
    public static final String CONFIG_PRIMARY_SUB_SETABLE = "config_primary_sub_setable";
    public static final String CONFIG_CURRENT_PRIMARY_SUB = "config_current_primary_sub";
    public static final String CHINA_TELECOM_SPN = "China Telecom";
    public static final String CHINA_UNICOM_SPN = "China Unicom";
    public static final String CONFIG_DISABLE_DDS_PREFERENCE = "config_disable_dds_preference";
    private static final String SETTING_USER_PREF_PRIMARY_SUB = "user_preferred_primary_sub";

    private static final String SETTING_USER_PREF_DATA_SUB = "user_preferred_data_sub";
    private static final String ACTION_RADIO_CAPABILITY_UPDATED =
            "org.codeaurora.intent.action.ACTION_RADIO_CAPABILITY_UPDATED";

    private static final int MSG_ALL_CARDS_AVAILABLE = 1;
    private static final int MSG_CONFIG_LTE_DONE = 2;
    private static final int MSG_RADIO_CAPS_READY = 3;

    private final Context mContext;
    private AlertDialog mSIMChangedDialog = null;

    public static void init(Context context) {
        synchronized (PrimarySubSelectionController.class) {
            if (sInstance == null && isPrimaryCardFeatureEnabled()) {
                sInstance = new PrimarySubSelectionController(context);
            }
        }
    }

    public static PrimarySubSelectionController getInstance() {
        synchronized (PrimarySubSelectionController.class) {
            if (sInstance == null) {
                throw new RuntimeException("PrimarySubSelectionController was not initialize!");
            }
            return sInstance;
        }
    }

    private PrimarySubSelectionController(Context context) {
        mContext = context.getApplicationContext();
        mPhones = new PhoneProxy[PHONE_COUNT];
        mPhones = PhoneFactory.getPhones();

        Rlog.d(TAG, " in constructor, context =  " + mContext);

        mCardStateMonitor = new CardStateMonitor(mContext);
        mCardStateMonitor.registerAllCardsInfoAvailable(this,
                MSG_ALL_CARDS_AVAILABLE, null);
        mDetect4gCardEnabled = mCardStateMonitor.isDetect4gCardEnabled();

        mIccLoaded = new boolean[PHONE_COUNT];
        for (int i = 0; i < PHONE_COUNT; i++) {
            mIccLoaded[i] = false;
        }

        IntentFilter intentFilter = new IntentFilter(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        intentFilter.addAction(Intent.ACTION_LOCALE_CHANGED);
        intentFilter.addAction(ACTION_RADIO_CAPABILITY_UPDATED);
        mContext.registerReceiver(mReceiver, intentFilter);
    }

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            logd("Recieved intent " + action);
            if (TelephonyIntents.ACTION_SIM_STATE_CHANGED.equals(action)) {
                final int slot = intent.getIntExtra(PhoneConstants.SLOT_KEY, 0);
                final String stateExtra = intent
                        .getStringExtra(IccCardConstants.INTENT_KEY_ICC_STATE);
                mIccLoaded[slot] = false;
                if (IccCardConstants.INTENT_VALUE_ICC_LOADED.equals(stateExtra) ||
                        IccCardConstants.INTENT_VALUE_ICC_IMSI.equals(stateExtra)) {
                    mIccLoaded[slot] = true;
                    int primarySlot = getPrimarySlot();
                    int currentDds = SubscriptionManager.getDefaultDataSubscriptionId();
                    int currentDdsSlotId = SubscriptionManager.getSlotId(currentDds);
                    logd("ACTION_SIM_STATE_CHANGED current defalut dds :" + currentDds
                            + ", primary slot :" + primarySlot);
                    if (currentDdsSlotId == primarySlot) {
                        mRestoreDdsToPrimarySub = false;
                        return;
                    }

                    logd("ACTION_SIM_STATE_CHANGED mRestoreDdsToPrimarySub: "
                            + mRestoreDdsToPrimarySub);
                    if (mRestoreDdsToPrimarySub) {
                        if (slot == primarySlot) {
                            int subId = SubscriptionManager.getSubId(slot)[0];
                            SubscriptionManager.from(context).setDefaultDataSubId(subId);
                            setUserPrefDataSubIdInDB(subId);
                            mRestoreDdsToPrimarySub = false;
                            logd("restore dds to primary card, dds[" + slot +"] = " + subId);
                        }
                    }
                }
                logd("ACTION_SIM_STATE_CHANGED intent received SIM STATE is " + stateExtra
                        + ", mIccLoaded[" + slot + "] = " + mIccLoaded[slot]);
            } else if (Intent.ACTION_LOCALE_CHANGED.equals(action)) {
                logd("Recieved EVENT ACTION_LOCALE_CHANGED");
                if (mSIMChangedDialog != null && mSIMChangedDialog.isShowing()) {
                    logd("Update SIMChanged dialog");
                    mSIMChangedDialog.dismiss();
                    alertSIMChanged();
                }
            } else if (ACTION_RADIO_CAPABILITY_UPDATED.equals(action)) {
                sendMessage(obtainMessage(MSG_RADIO_CAPS_READY));
            }
        }
    };

    private int getUserPrefPrimarySlotFromDB() {
        if (isDetect4gCardEnabled()) {
            int userPrefsubIdFromDB = getUserPrefPrimarySubIdFromDB();
            SubscriptionInfo sir = SubscriptionManager.from(mContext).
                    getActiveSubscriptionInfo(userPrefsubIdFromDB);
            if ((sir != null) && mCardStateMonitor.getProvisionPref(sir.getSimSlotIndex())
                     == mCardStateMonitor.PROVISIONED) {
                return sir.getSimSlotIndex();
            }
        }
        return SubscriptionManager.INVALID_SIM_SLOT_INDEX;
    }

    private int getUserPrefPrimarySubIdFromDB() {
        int subId = SubscriptionManager.INVALID_SUBSCRIPTION_ID;
        subId = android.provider.Settings.Global.getInt(mContext.getContentResolver(),
                SETTING_USER_PREF_PRIMARY_SUB, subId);
        logd("getUserPrefPrimarySubIdFromDB: " + subId);
        return subId;
    }

    void setUserPrefPrimarySubIdInDB(int subId) {
        android.provider.Settings.Global.putInt(mContext.getContentResolver(),
                SETTING_USER_PREF_PRIMARY_SUB, subId);
        logd("updating preferred primary subId: " + subId + " in DB");
    }

    protected boolean isCardsInfoChanged(int phoneId) {
        String iccId = mCardStateMonitor.getIccId(phoneId);
        String iccIdInSP = PreferenceManager.getDefaultSharedPreferences(mContext).getString(
                PhoneConstants.SUBSCRIPTION_KEY + phoneId, null);
        logd(" phoneId " + phoneId + " icc id = " + iccId + ", icc id in sp=" + iccIdInSP);
        return !TextUtils.isEmpty(iccId) && !iccId.equals(iccIdInSP);
    }

    private void loadStates() {
        mCardChanged = false;
        for (int i = 0; i < PHONE_COUNT; i++) {
            if (isCardsInfoChanged(i)) {
                mCardChanged = true;
            }
        }
        if (mCardChanged) {
            mShowSimSelectionDialog = true;
            setUserPrefPrimarySubIdInDB(-1);
        }
        updateNumActiveSubs();
        mAllCardsAbsent = isAllCardsAbsent();
    }

    private void updateNumActiveSubs() {
        mNumActiveSubs = 0;
        for (int i = 0; i < PHONE_COUNT; i++) {
            if (isCardActivated(i)) {
                mNumActiveSubs++;
            }
        }
    }

    private boolean isCardActivated(int index) {
        UiccCard uiccCard = CardStateMonitor.getUiccCard(index);
        if (uiccCard != null && uiccCard.getCardState() != CardState.CARDSTATE_ABSENT) {
            UiccCardApplication app = uiccCard.getApplication(UiccController.APP_FAM_3GPP);
            if (app != null && app.getState() == AppState.APPSTATE_READY) {
                return true;
            }
        }
        return false;
    }

    private void saveSubscriptions() {
        for (int i = 0; i < PHONE_COUNT; i++) {
            String iccId = mCardStateMonitor.getIccId(i);
             if (isDetect4gCardEnabled() &&
                     (iccId == null || mCardStateMonitor.isCardDeactivated(i))) {
                 // IccId is null i.e. Card is removed OR Card is deactivated,
                 // so update SP with IccId as -1
                 logd("card removed or deactivated, save iccid -1 on slotId: " + i);
                  PreferenceManager.getDefaultSharedPreferences(mContext).edit()
                          .putString(PhoneConstants.SUBSCRIPTION_KEY + i, "-1").commit();
            } else if (iccId != null) {
                logd("save subscription on slotId " + i + " , iccId :" + iccId);
                PreferenceManager.getDefaultSharedPreferences(mContext).edit()
                        .putString(PhoneConstants.SUBSCRIPTION_KEY + i, iccId).commit();
            }
        }
    }

    public void onClick(DialogInterface dialog, int which) {
        switch (which) {
            case DialogInterface.BUTTON_NEGATIVE:
                // do nothing.
                break;
            case DialogInterface.BUTTON_POSITIVE:
                // call dual settings;
                Intent intent = new Intent("com.android.settings.sim.SIM_SUB_INFO_SETTINGS");
                intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK
                        | Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
                try {
                    mContext.startActivity(intent);
                } catch (ActivityNotFoundException e) {
                    logd("can not start activity " + intent);
                }
                break;
        }
    }

    @Override
    public void onDismiss(DialogInterface dialog) {
       if (mSIMChangedDialog == (AlertDialog) dialog) {
            mSIMChangedDialog = null;
        }
    }

    protected void alertSIMChanged() {
        AlertDialog.Builder builder = new AlertDialog.
            Builder(mContext, R.style.CustomAlertDialogBackground).setTitle(R.string.sim_info)
                .setMessage(Html.fromHtml(getSIMInfo())).setNegativeButton(R.string.close, this);
        if (PHONE_COUNT > 1) {
            builder.setPositiveButton(R.string.change, this);
        }
        mSIMChangedDialog = builder.create();
        mSIMChangedDialog.getWindow().setType(
                WindowManager.LayoutParams.TYPE_SYSTEM_DIALOG);
        mSIMChangedDialog.setOnDismissListener(this);
        mSIMChangedDialog.show();
    }

    private String getSIMInfo() {
        ConnectivityManager connService = (ConnectivityManager) mContext
                .getSystemService(Context.CONNECTIVITY_SERVICE);
        String mobileDataState = connService.getMobileDataEnabled() ? mContext
                .getString(R.string.mobile_data_on) : mContext.getString(R.string.mobile_data_off);
        String html = mContext.getString(R.string.new_sim_detected) + "<br>";

        // show SIM card info
        for (int index = 0; index < PHONE_COUNT; index++) {
            html += getSimName(index) + ":" + getSimCardInfo(index) + "<br>";
        }

        // show data status
        html += mContext.getString(R.string.default_sim_setting) + "<br>"
                + mContext.getString(R.string.mobile_data) + mobileDataState;

        return html;
    }

    public String getSimName(int slot) {
        SubscriptionInfo subInfo = SubscriptionManager.from(mContext).getActiveSubscriptionInfo(
                SubscriptionManager.getSubId(slot)[0]);
        return subInfo == null ? mContext.getResources().getString(R.string.sim_card_number_title,
                slot + 1) : subInfo.getDisplayName().toString();
    }

    public String getSimCardInfo(int slot) {
        UiccCard uiccCard = CardStateMonitor.getUiccCard(slot);
        if (uiccCard != null && uiccCard.getCardState() == CardState.CARDSTATE_ABSENT) {
            return mContext.getString(R.string.sim_absent);
        } else {
            String carrierName = TelephonyManager.getDefault().getSimOperatorName(
                    SubscriptionManager.getSubId(slot)[0]);
            carrierName = getLocalString(mContext, carrierName, mContext.getPackageName(),
                    R.array.original_carrier_names, R.array.locale_carrier_names);
            if (TextUtils.isEmpty(carrierName) || TextUtils.isDigitsOnly(carrierName)) {
                String iccId = mCardStateMonitor.getIccId(slot);
                String spn = IINList.getInstance(mContext).getSpn(iccId);
                if (spn != null) {
                    carrierName = getLocalString(mContext, spn, mContext.getPackageName(),
                            R.array.original_carrier_names, R.array.locale_carrier_names);
                } else {
                    if (isDetect4gCardEnabled()) {
                        carrierName = getSimName(slot);
                    } else {
                        carrierName = mContext.getString(R.string.sim_unknown);
                     }
                }
            }
            if (isAutoConfigMode() && slot == getPrimarySlot() && !isDetect4gCardEnabled()) {
                if (uiccCard.isApplicationOnIcc(AppType.APPTYPE_USIM)) {
                    return carrierName + "(4G)";
                } else {
                    return carrierName + "(3G)";
                }
            } else {
                return carrierName;
            }
        }
    }

    private void configPrimaryLteSub() {
        int slotIndex = SubscriptionManager.INVALID_SIM_SLOT_INDEX;
        if (!isPrimarySetable()) {
            logd("primary is not setable in any sub!");
            return;
        } else {
            int prefPrimarySlot = getPrefPrimarySlot();
            int primarySlot = getPrimarySlot();
            logd("pref primary slotIndex = " + prefPrimarySlot + " curr primary slot = " +
                     primarySlot + " mCardChanged = " + mCardChanged);
            if (prefPrimarySlot == SubscriptionManager.INVALID_SIM_SLOT_INDEX &&
                    (mCardChanged || primarySlot == SubscriptionManager.INVALID_SIM_SLOT_INDEX)) {
                slotIndex = 0;
            } else if (prefPrimarySlot != SubscriptionManager.INVALID_SIM_SLOT_INDEX &&
                    (mCardChanged || primarySlot != prefPrimarySlot)) {
                slotIndex = prefPrimarySlot;
            }
            if (slotIndex != SubscriptionManager.INVALID_SIM_SLOT_INDEX &&
                    primarySlot == slotIndex && !mCardChanged) {
                logd("primary sub and network mode are all correct, just notify");
                obtainMessage(MSG_CONFIG_LTE_DONE, primarySlot, -1).sendToTarget();
                return;
            } else if (slotIndex == SubscriptionManager.INVALID_SIM_SLOT_INDEX) {
                logd("card not changed and primary sub is correct, do nothing");
                boolean is7_5_modeEnabled = multipleLteNetworkSupported();
                if (!is7_5_modeEnabled && !isDetect4gCardEnabled()) {
                    trySetDdsToPrimarySub();
                }
                return;
            }
        }
        setPreferredNetwork(slotIndex, obtainMessage(MSG_CONFIG_LTE_DONE, slotIndex, -1));
    }

    private boolean trySetDdsToPrimarySub() {
        boolean set = false;
        int primarySlot = getPrimarySlot();

        if (primarySlot != SubscriptionManager.INVALID_SIM_SLOT_INDEX) {
            int currentDds = SubscriptionManager.getDefaultDataSubscriptionId();
            int currentDdsSlotId = SubscriptionManager.getSlotId(currentDds);
            String iccId = mCardStateMonitor.getIccId(primarySlot);

            logd("trySetDdsToPrimarySub primary Slot " + primarySlot + ", currentDds = " + currentDds
                    + ", mIccLoaded[" + primarySlot
                    + "] =" + mIccLoaded[primarySlot] + "Icc Id = " + iccId);
            if (mIccLoaded[primarySlot] || !TextUtils.isEmpty(iccId)) {
                int subId = SubscriptionManager.getSubId(primarySlot)[0];
                if (currentDdsSlotId != primarySlot) {
                    SubscriptionManager.from(mContext).setDefaultDataSubId(subId);
                    mRestoreDdsToPrimarySub = false;
                    set = true;
                }
                setUserPrefDataSubIdInDB(subId);
            }
        }
        return set;
    }

    protected void onConfigLteDone(Message msg) {
        boolean isManualConfigMode = isManualConfigMode();
        int slotId = msg.arg1;
        if (slotId != SubscriptionManager.INVALID_SIM_SLOT_INDEX) {
            logd("onConfigLteDone, n/w mode success, primary card = " + slotId);
            savePrimarySlot(slotId);
        }
        if (!trySetDdsToPrimarySub()) {
            mRestoreDdsToPrimarySub = true;
        }
        logd("onConfigLteDone isManualConfigMode " + isManualConfigMode);
        if (isAutoConfigMode() && !isDetect4gCardEnabled()) {
            alertSIMChanged();
        } else if (isManualConfigMode || showSelectionDialog()) {
            if (isDetect4gCardEnabled() && !mShowSimSelectionDialog) return;
            logd("Show Selection Dialog");
            mShowSimSelectionDialog = false;
            Intent intent = new Intent(mContext, PrimarySubSetting.class);
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK
                    | Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
            mContext.startActivity(intent);
        }
    }

    private boolean showSelectionDialog() {
        if (isDetect4gCardEnabled() && mShowSimSelectionDialog && (mNumActiveSubs > 1)) {
            for (int index = 0; index < PHONE_COUNT; index++) {
                if (!mCardStateMonitor.getCardInfo(index).
                        isCardStateEquals(CardState.CARDSTATE_PRESENT.toString())
                        || !(mCardStateMonitor.getCardInfo(index).isEfReadCompleted())) {
                    return false;
                }
            }
            return true;
        } else {
            return false;
        }
    }

    private void setPrimarySub() {
        if (!mRadioCapabilityAvailable) {
            logd("Radio capability not available, do not set primary sub.");
            mNeedHandleModemReadyEvent = true;
            return;
        }

        // reset states and load again by new card info
        loadStates();
        if (isPrimaryCardFeatureEnabled()) {
            logd("primary sub config feature is enabled!");
            configPrimaryLteSub();
        }
        saveSubscriptions();
        saveLteSubSelectMode();
        savePrimarySetable();
        configureCtIfRequired();
    }

    public void configureCtIfRequired() {
        int numCTSims = 0, numCUSims = 0;
        int numCardsPresent = 0;
        int ctIndex = SubscriptionManager.INVALID_SIM_SLOT_INDEX;
        int cuIndex = SubscriptionManager.INVALID_SIM_SLOT_INDEX;

        mIsCtCardPresent = false;
        boolean is7_5_modeEnabled = multipleLteNetworkSupported();
        if (!is7_5_modeEnabled) {
            logd("7+5 mode not enabled. No need to check for card info.");
            return;
        }

        for (int index = 0; index < PHONE_COUNT; index++) {
            String iccId = mCardStateMonitor.getIccId(index);
            String spnName = IINList.getInstance(mContext).getSpn(iccId);

            if (!TextUtils.isEmpty(spnName) && spnName.equals(CHINA_TELECOM_SPN)) {
                numCTSims++;
                ctIndex = index;
            } else if (!TextUtils.isEmpty(spnName) && spnName.equals(CHINA_UNICOM_SPN)) {
                numCUSims++;
                cuIndex = index;
            }
            if (mCardStateMonitor.getCardInfo(index).isCardPresent()) numCardsPresent++;
        }

        // If only one CT SIM is present
        // i) Set nwMode 1x/DO/G/W on that slot.
        if (numCTSims == 1 && ctIndex != SubscriptionManager.INVALID_SIM_SLOT_INDEX) {
            logd(" CT card, setting global N/W mode [" + ctIndex + "] = " + Phone.NT_MODE_GLOBAL);
            mIsCtCardPresent = true;
            new PrefNetworkRequest(mContext, ctIndex, Phone.NT_MODE_GLOBAL, null).loop();
        }

        //if 2 CT cards are present, set nwMode 1x/DO/G/W on sub 0 and GSM on sub 1
        //Also set DDS on sub 0
        if (numCTSims == 2) {
            mIsCtCardPresent = true;
            new PrefNetworkRequest(mContext, 1, Phone.NT_MODE_GSM_ONLY, null).loop();
            new PrefNetworkRequest(mContext, 0, Phone.NT_MODE_GLOBAL, null).loop();
            int subId = SubscriptionManager.getSubId(0)[0];
            SubscriptionManager.from(mContext).setDefaultDataSubId(subId);

            // update subid as user preferred DDS sub id
            setUserPrefDataSubIdInDB(subId);
        }

        // ii)save ct mode, so that Settings app can enable/disable the DDS option.
        Settings.Global.putInt(mContext.getContentResolver(),
                CONFIG_DISABLE_DDS_PREFERENCE, mIsCtCardPresent ? 1 : 0);

        // If only CT card is present set DDS to that sub.
        if (numCardsPresent == 1 && numCTSims == 1 &&
                ctIndex != SubscriptionManager.INVALID_SIM_SLOT_INDEX) {
            int subId = SubscriptionManager.getSubId(ctIndex)[0];
            logd("only one card present, setting dds to CT SIM[" + ctIndex + "] = " + subId);
            SubscriptionManager.from(mContext).setDefaultDataSubId(subId);
        }

        //In Case of CU+CT, set DDS and nwMode GSM on CU Card
        if (numCardsPresent == 2 && numCTSims == 1 &&
                ctIndex != SubscriptionManager.INVALID_SIM_SLOT_INDEX &&
                numCUSims == 1 && cuIndex != SubscriptionManager.INVALID_SIM_SLOT_INDEX) {
            int subId = SubscriptionManager.getSubId(cuIndex)[0];
            logd(" setting dds to CU SIM[" + cuIndex + "] = " + subId);
            SubscriptionManager.from(mContext).setDefaultDataSubId(subId);
            setUserPrefDataSubIdInDB(subId); // update fallback sub id
            new PrefNetworkRequest(mContext, cuIndex, Phone.NT_MODE_GSM_ONLY, null).loop();
        }
    }

    @Override
    public void handleMessage(Message msg) {
        switch (msg.what) {
            case MSG_RADIO_CAPS_READY:
                logd("on EVENT MSG_RADIO_CAPS_READY " + mNeedHandleModemReadyEvent);
                mRadioCapabilityAvailable = true;
                if (mNeedHandleModemReadyEvent) {
                    setPrimarySub();
                    mNeedHandleModemReadyEvent = false;
                } else if (isPrimaryCardFeatureEnabled() && !isDetect4gCardEnabled()) {
                    int primarySlot = getPrimarySlot();
                    int currentDds = SubscriptionManager.getDefaultDataSubscriptionId();
                    int currentDdsSlotId = SubscriptionManager.getSlotId(currentDds);
                    if ((primarySlot != SubscriptionManager.INVALID_SIM_SLOT_INDEX) &&
                            (mIccLoaded[primarySlot])) {
                        int subId = SubscriptionManager.getSubId(primarySlot)[0];
                        if (currentDdsSlotId != primarySlot) {
                            logd(" Radio caps ready, new dds[" + primarySlot + "] = " + subId);
                            SubscriptionManager.from(mContext).setDefaultDataSubId(subId);
                        }
                        setUserPrefDataSubIdInDB(subId);
                    }
                }
                break;
            case MSG_ALL_CARDS_AVAILABLE:
                logd("on EVENT MSG_ALL_CARDS_AVAILABLE");
                setPrimarySub();
                break;
            case MSG_CONFIG_LTE_DONE:
                logd("on EVENT MSG_CONFIG_LTE_DONE");
                onConfigLteDone(msg);
                break;
        }
    }

    public void setPreferredNetwork(int slotIndex, Message msg) {
        int network = -1;

        if (slotIndex >= 0 && slotIndex < PHONE_COUNT) {
            String iccId = mCardStateMonitor.getIccId(slotIndex);
            UiccCard uiccCard = CardStateMonitor.getUiccCard(slotIndex);

            network = IINList.getInstance(mContext).getIINPrefNetwork(iccId, uiccCard);
            if (network == -1) {
                logd("network mode is -1 , can not set primary card for " + slotIndex);
                msg.arg1 = -1;
                if (msg.getTarget() != null) {
                    msg.sendToTarget();
                }
                return;
            }
        }

        logd("set primary card for slot = " + slotIndex + ", network=" + network);
        new PrefNetworkRequest(mContext, slotIndex, network, msg).loop();
    }

    // Retrieve IIN priorities of current inserted SIM cards.
    private Map<Integer, Integer> retrieveCurrentCardsIINPriorities() {
        Map<Integer, Integer> priorities = new HashMap<Integer, Integer>();
        for (int index = 0; index < PHONE_COUNT; index++) {
            if (isDetect4gCardEnabled() && !isCardActivated(index)) continue;
            String iccId = mCardStateMonitor.getIccId(index);
            UiccCard uiccCard = CardStateMonitor.getUiccCard(index);
            priorities.put(index, IINList.getInstance(mContext).getIINPriority(iccId, uiccCard));
        }
        return priorities;
    }


    public boolean isDetect4gCardEnabled() {
        return mDetect4gCardEnabled;
    }

    private int getPriority(Map<Integer, Integer> priorities, Integer higherPriority) {
        int count = getCount(priorities, higherPriority);
        if (count == 1) {
            return getKey(priorities, higherPriority);
        } else if (count > 1) {
            //priority match, check for 4g card here
            return mCardStateMonitor.get4gCardSlot();
        } else if (higherPriority > 0) {
            return getPriority(priorities, --higherPriority);
        } else {
            return IINList.INVALID_IIN_PRIORITY;
        }
    }

    private int getCount(Map<Integer, Integer> priorities, int priority) {
        int count = 0;
        for (Integer key : priorities.keySet()) {
            if (priorities.get(key) == priority) {
                count++;
            }
        }
        return count;
    }

    private Integer getKey(Map<Integer, Integer> map, int priority) {
        for (Integer key : map.keySet()) {
            if (map.get(key) == priority) {
                return key;
            }
        }
        return null;
    }

    private boolean multipleLteNetworkSupported() {
        int raf;
        int numOfPhonesSupportLte = 0;

        for (Phone phone : mPhones) {
            if (phone != null) {
                raf = phone.getRadioAccessFamily();
                if ((raf & RadioAccessFamily.RAF_LTE) == RadioAccessFamily.RAF_LTE) {
                    numOfPhonesSupportLte++;
                }
                logd(" raf[" + phone.getPhoneId() + "] = " + raf +
                        " no of LTE phones " + numOfPhonesSupportLte);
            }
        }
        return (numOfPhonesSupportLte > 1);
    }

    public void setRestoreDdsToPrimarySub(boolean restoreDdsToPrimarySub) {
        mRestoreDdsToPrimarySub = restoreDdsToPrimarySub;
    }

    // This method return preferred primary slot as
    // 1. SlotId in which the highest IIN priority Card inserted.
    // 2. '-1' if two SIM cards from both slots has same priority.
    public int getPrefPrimarySlot() {
        int prefSlot = getUserPrefPrimarySlotFromDB();
        if (prefSlot == SubscriptionManager.INVALID_SIM_SLOT_INDEX) {
            prefSlot = getPriority(retrieveCurrentCardsIINPriorities(),
                 IINList.getInstance(mContext).getHighestPriority());
        }
        return prefSlot;
    }

    public boolean isPrimarySetable() {
        Map<Integer, Integer> priorities = retrieveCurrentCardsIINPriorities();
        int unsetableCount = getCount(priorities, IINList.INVALID_IIN_PRIORITY);
        return unsetableCount < priorities.size();
    }

    // Primary card feature would be enabled when system property
    // PRIMARY_CARD_PROPERTY_NAME set to true on MSIM devices.
    public static boolean isPrimaryCardFeatureEnabled() {
        return SystemProperties.getBoolean(PRIMARY_CARD_PROPERTY_NAME, false) && (PHONE_COUNT > 1);
    }

    public boolean isCtCardPresent() {
        return mIsCtCardPresent;
    }

    private boolean isAllCardsAbsent() {
        for (int i = 0; i < PHONE_COUNT; i++) {
            UiccCard uiccCard = CardStateMonitor.getUiccCard(i);
            if (uiccCard == null || uiccCard.getCardState() != CardState.CARDSTATE_ABSENT) {
                logd("card state on sub" + i + " not absent");
                return false;
            }
        }
        logd("all cards absent");
        return true;
    }

    public void setUserPrefDataSubIdInDB(int subId) {
        android.provider.Settings.Global.putInt(mContext.getContentResolver(),
                SETTING_USER_PREF_DATA_SUB, subId);
        logd("updating preferred data subId: " + subId + " in DB");
    }

    private void saveLteSubSelectMode() {
        Settings.Global.putInt(mContext.getContentResolver(),
                CONFIG_LTE_SUB_SELECT_MODE, (isManualConfigMode() ||
               (isDetect4gCardEnabled() && (mNumActiveSubs > 1))) ? 0 : 1);
    }

    private void savePrimarySetable() {
        Settings.Global.putInt(mContext.getContentResolver(),
                CONFIG_PRIMARY_SUB_SETABLE, isPrimarySetable() ? 1 : 0);
    }

    // This method returns current configured primary slot, with
    // condition that corresponding N/W mode should be set to 18/20.
    public int getPrimarySlot() {
        if (isDetect4gCardEnabled()) {
            return get4GPrimarySlot();
        }
        int slotId = Settings.Global.getInt(mContext.getContentResolver(),
                CONFIG_CURRENT_PRIMARY_SUB, SubscriptionManager.INVALID_SIM_SLOT_INDEX);

        if (SubscriptionManager.isValidSlotId(slotId)) {
            int nwMode = getPreferredNetworkFromDb(slotId);
            if (nwMode != Phone.NT_MODE_TD_SCDMA_GSM_WCDMA_LTE
                    && nwMode != Phone.NT_MODE_TD_SCDMA_GSM_WCDMA) {
                logd(" getPrimarySlot, N/W mode[" + slotId + "] = " + nwMode + " not correct");
                slotId = SubscriptionManager.INVALID_SIM_SLOT_INDEX;
            }
        }
        return slotId;
    }

    // When 4G primary card feature enabled, return slotId in below order
    // 1. User selected primary SIM slot if activated.
    // 2. Current configured primary slot if activated.
    // 3. SubscriptionManager.INVALID_SIM_SLOT_INDEX.
    int get4GPrimarySlot() {
        int slotId = getUserPrefPrimarySlotFromDB();
        if (!SubscriptionManager.isValidSlotId(slotId)) {
            slotId = Settings.Global.getInt(mContext.getContentResolver(),
                    CONFIG_CURRENT_PRIMARY_SUB, SubscriptionManager.INVALID_SIM_SLOT_INDEX);
            if (!SubscriptionManager.isValidSlotId(slotId) ||
                   (mCardStateMonitor.getProvisionPref(slotId) != mCardStateMonitor.PROVISIONED)) {
                logd(" get4GPrimarySlot not correct " + slotId);
                slotId = SubscriptionManager.INVALID_SIM_SLOT_INDEX;
            }
        }
        return slotId;
    }

    void savePrimarySlot(int slotId) {
        Settings.Global.putInt(mContext.getContentResolver(),
                CONFIG_CURRENT_PRIMARY_SUB, slotId);
    }

    private boolean isManualConfigMode() {
        return isPrimarySetable() && getPrefPrimarySlot() ==
                SubscriptionManager.INVALID_SIM_SLOT_INDEX;
    }

    private boolean isAutoConfigMode() {
        return isPrimarySetable() && getPrefPrimarySlot() !=
                SubscriptionManager.INVALID_SIM_SLOT_INDEX;
    }

    private int getPreferredNetworkFromDb(int slotId) {
        SubscriptionController subContrl = SubscriptionController.getInstance();
        int[] subId = subContrl.getSubId(slotId);
        int nwMode = -1;

        if (subContrl.isActiveSubId(subId[0])) {
            nwMode = Settings.Global.getInt(mContext.getContentResolver(),
                    Settings.Global.PREFERRED_NETWORK_MODE + subId[0], nwMode);
        }
        return nwMode;
    }

    /**
     * parse the string to current language.
     *
     * @param context base context of the application
     * @param originalString original string
     * @param defPackage the target package where the local language strings
     *            defined
     * @param originNamesId the id of the original string array.
     * @param localNamesId the id of the local string keys.
     * @return local language string
     */
    private final String getLocalString(Context context, String originalString,
            String defPackage, int originNamesId, int localNamesId) {
        String[] origNames = context.getResources().getStringArray(originNamesId);
        String[] localNames = context.getResources().getStringArray(localNamesId);
        for (int i = 0; i < origNames.length; i++) {
            if (origNames[i].equalsIgnoreCase(originalString)) {
                return localNames[i];
            }
        }
        return originalString;
    }

    private void logd(String message) {
        if (DEBUG) {
            Rlog.d(TAG, message);
        }
    }
}
