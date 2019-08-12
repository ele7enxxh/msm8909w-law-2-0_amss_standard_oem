/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
 * Copyright (C) 2006 The Android Open Source Project
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

package com.qualcomm.qti.networksetting;

import com.android.ims.ImsManager;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.TelephonyProperties;
import com.android.internal.telephony.uicc.IccCardApplicationStatus.AppType;
import com.android.internal.telephony.uicc.UiccController;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import android.app.ActionBar;
import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager.NameNotFoundException;
import android.net.Uri;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.PersistableBundle;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.os.UserHandle;
import android.os.UserManager;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.preference.SwitchPreference;
import android.provider.Settings.SettingNotFoundException;
import android.provider.Settings;
import android.telephony.CarrierConfigManager;
import android.telephony.PhoneStateListener;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.TabHost.OnTabChangeListener;
import android.widget.TabHost.TabContentFactory;
import android.widget.TabHost.TabSpec;
import android.widget.TabHost;

import org.codeaurora.internal.IExtTelephony;

/**
 * "Mobile network settings" screen.  This preference screen lets you
 * enable/disable mobile data, and control data roaming and other
 * network-specific mobile data features.  It's used on non-voice-capable
 * tablets as well as regular phone devices.
 *
 * Note that this PreferenceActivity is part of the phone app, even though
 * you reach it from the "Wireless & Networks" section of the main
 * Settings app.  It's not part of the "Call settings" hierarchy that's
 * available from the Phone app (see CallFeaturesSetting for that.)
 */
public class MobileNetworkSettings extends PreferenceActivity
        implements DialogInterface.OnClickListener,
        DialogInterface.OnDismissListener, Preference.OnPreferenceChangeListener{

    // debug data
    private static final String LOG_TAG = "MobileNetworkSettings";
    private static final boolean DBG = true;
    public static final int REQUEST_CODE_EXIT_ECM = 17;

    // Number of active Subscriptions to show tabs
    private static final int TAB_THRESHOLD = 2;

    //String keys for preference lookup
    private static final String BUTTON_PREFERED_NETWORK_MODE = "preferred_network_mode_key";
    private static final String BUTTON_PREFERED_NETWORK_CMCC_MODE =
            "preferred_network_mode_cmcc_key";
    private static final String BUTTON_ROAMING_KEY = "button_roaming_key";
    private static final String BUTTON_CDMA_LTE_DATA_SERVICE_KEY = "cdma_lte_data_service_key";
    private static final String BUTTON_UPLMN_KEY = "button_uplmn_key";
    private static final String BUTTON_ENABLED_NETWORKS_KEY = "enabled_networks_key";
    private static final String BUTTON_4G_LTE_KEY = "enhanced_4g_lte";
    private static final String BUTTON_ENABLE_4G_KEY = "enable_4g_settings";
    private static final String BUTTON_CELL_BROADCAST_SETTINGS = "cell_broadcast_settings";
    private static final String BUTTON_APN_EXPAND_KEY = "button_apn_key";
    private static final String BUTTON_OPERATOR_SELECTION_EXPAND_KEY = "button_carrier_sel_key";
    private static final String BUTTON_CARRIER_SETTINGS_KEY = "carrier_settings_key";
    private static final String BUTTON_CDMA_SYSTEM_SELECT_KEY = "cdma_system_select_key";
    private static final String KEY_HIDE_NETWORK_SETTINGS_BOOL = "hide_network_settings_bool";

    private static final String PRIMARY_4G_CARD_PROPERTY_NAME = "persist.radio.detect4gcard";
    private static final String PRIMARY_CARD_PROPERTY_NAME = "persist.radio.primarycard";
    private static final String CONFIG_CURRENT_PRIMARY_SUB = "config_current_primary_sub";

    //Carrier mode
    private static final String CARRIER_MODE_CT_CLASS_A = "ct_class_a";
    private static final String CARRIER_MODE_CT_CLASS_C = "ct_class_c";
    private static final String CARRIER_MODE_CMCC = "cmcc";
    private String mCarrierMode = SystemProperties.get("persist.radio.carrier_mode", "default");
    private boolean mIsCTClassC = mCarrierMode.equals(CARRIER_MODE_CT_CLASS_C);
    private boolean mIsCMCC = mCarrierMode.equals(CARRIER_MODE_CMCC);

    public static final String EXTRA_EXIT_ECM_RESULT = "exit_ecm_result";

    private static final boolean PRIMCARYCARD_L_W_ENABLED =
            SystemProperties.getBoolean("persist.radio.lw_enabled", false);
    private int preferredNetworkMode = Phone.PREFERRED_NT_MODE;

    //Information about logical "up" Activity
    private static final String UP_ACTIVITY_PACKAGE = "com.android.settings";
    private static final String UP_ACTIVITY_CLASS =
            "com.android.settings.Settings$WirelessSettingsActivity";
    private CarrierConfigManager mConfigManager;
    private SubscriptionManager mSubscriptionManager;
    private String tabDefaultLabel = "SIM slot ";

    //UI objects
    private ListPreference mButtonPreferredNetworkMode;
    private ListPreference mButtonEnabledNetworks;
    private ListPreference mButtonPreferredCMCCNetworkMode;
    private RestrictedSwitchPreference mButtonDataRoam;
    private SwitchPreference mButton4glte;
    private SwitchPreference mButtonEnable4g;
    private Preference mLteDataServicePref;
    private Preference mButtonUplmnPref;

    private static final String iface = "rmnet0"; //TODO: this will go away
    private List<SubscriptionInfo> mActiveSubInfos;

    private UserManager mUm;
    private Phone mPhone;
    private MyHandler mHandler;
    private boolean mOkClicked;

    // We assume the the value returned by mTabHost.getCurrentTab() == slotId
    private TabHost mTabHost;

    //GsmUmts options and Cdma options
    GsmUmtsOptions mGsmUmtsOptions;
    CdmaOptions mCdmaOptions;

    private Preference mClickedPreference;
    private boolean mShow4GForLTE;
    private boolean mIsGlobalCdma;
    private boolean mUnavailable;
    private IExtTelephony mExtTelephony = IExtTelephony.Stub.
            asInterface(ServiceManager.getService("extphone"));
    private static final int NOT_PROVISIONED = 0;

    int[] callState = new int[TelephonyManager.getDefault().getPhoneCount()];

    private void listenForCallState(int subId, int listenStatus) {
        TelephonyManager tm = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
        tm.listen(new PhoneStateListener(subId) {
            /*
             * Enable/disable the 'Enhanced 4G LTE Mode' when in/out of a call
             * and depending on TTY mode and TTY support over VoLTE.
             * @see android.telephony.PhoneStateListener#onCallStateChanged(int,
             * java.lang.String)
             */
            @Override
            public void onCallStateChanged(int state, String incomingNumber) {
                if (DBG) {
                    log("PhoneStateListener.onCallStateChanged: state=" + state +
                            " SubId: " + mSubId);
                }
                int phoneId = SubscriptionManager.getPhoneId(mSubId);
                boolean enabled = true;

                callState[phoneId] = state;
                for (int i = 0; i < TelephonyManager.getDefault().getPhoneCount(); i++) {
                    if (DBG) log("callstate: " + callState[i]);
                    enabled = (callState[i] == TelephonyManager.CALL_STATE_IDLE);
                    if (!enabled) break;
                }
                enabled = enabled &&
                        ImsManager.isNonTtyOrTtyOnVolteEnabled(getApplicationContext());
                Preference pref = getPreferenceScreen().findPreference(BUTTON_4G_LTE_KEY);
                if (pref != null) pref.setEnabled(enabled && hasActiveSubscriptions());
            }

        }, listenStatus);
    }

    private final BroadcastReceiver mPhoneChangeReceiver = new PhoneChangeReceiver();

    private class PhoneChangeReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action.equals(TelephonyIntents.ACTION_RADIO_TECHNOLOGY_CHANGED)) {
                int phoneId = intent.getIntExtra(PhoneConstants.PHONE_KEY,
                        SubscriptionManager.INVALID_PHONE_INDEX);
                if (DBG) log("onReceive: phoneId: " + phoneId);
                //Update UI if RAT change is on current slot/phone TAB.
                if (mPhone.getPhoneId() == phoneId) {
                    // When the radio changes (ex: CDMA->GSM), refresh all options.
                    mGsmUmtsOptions = null;
                    mCdmaOptions = null;
                    updateBody();
                }
            } else if (action.equals(Intent.ACTION_AIRPLANE_MODE_CHANGED) ||
                    action.equals(TelephonyIntents.ACTION_SIM_STATE_CHANGED)) {
                setScreenState();
            }
        }
    }

    private boolean isDetect4gCardEnabled() {
        return SystemProperties.getBoolean(PRIMARY_CARD_PROPERTY_NAME, false) &&
            SystemProperties.getBoolean(PRIMARY_4G_CARD_PROPERTY_NAME, false);
    }

    private void setScreenState() {
        if (mPhone != null) {
            int phoneId = mPhone.getPhoneId();
            int simState = TelephonyManager.getDefault().getSimState(phoneId);
            boolean screenState = simState != TelephonyManager.SIM_STATE_ABSENT;
            log("set sub screenState phoneId=" + phoneId + ", simState=" + simState);
            if (screenState && isDetect4gCardEnabled()) {
                //primary card feature is enabled
                int provStatus = NOT_PROVISIONED;
                try{
                    provStatus = mExtTelephony.
                        getCurrentUiccCardProvisioningStatus(phoneId);
                } catch (RemoteException ex){
                    loge("RemoteException @setScreenState=" + ex + ", phoneId=" + phoneId);
                } catch (NullPointerException ex) {
                    loge("NullPointerException @setScreenState=" + ex + ", phoneId=" + phoneId);
                }
                screenState = provStatus != NOT_PROVISIONED;
                log("set sub screenState provStatus=" + provStatus +
                    ", screenState=" + screenState);
            }
            getPreferenceScreen().setEnabled(screenState);
        }
    }

    //This is a method implemented for DialogInterface.OnClickListener.
    //  Used to dismiss the dialogs when they come up.
    public void onClick(DialogInterface dialog, int which) {
        if (which == DialogInterface.BUTTON_POSITIVE) {
            mPhone.setDataRoamingEnabled(true);
            mOkClicked = true;
        } else {
            // Reset the toggle
            mButtonDataRoam.setChecked(false);
        }
    }

    @Override
    public void onDismiss(DialogInterface dialog) {
        // Assuming that onClick gets called first
        mButtonDataRoam.setChecked(mOkClicked);
    }

    /**
     * Invoked on each preference click in this hierarchy, overrides
     * PreferenceActivity's implementation.  Used to make sure we track the
     * preference click events.
     */
    @Override
    public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        /** TODO: Refactor and get rid of the if's using subclasses */
        final int phoneSubId = mPhone.getSubId();
        if (preference.getKey().equals(BUTTON_4G_LTE_KEY)) {
            return true;
        } else if (preference.getKey().equals(BUTTON_ENABLE_4G_KEY)) {
            return true;
        } else if (mGsmUmtsOptions != null &&
                mGsmUmtsOptions.preferenceTreeClick(preference) == true) {
            return true;
        } else if (mCdmaOptions != null &&
                   mCdmaOptions.preferenceTreeClick(preference) == true) {
            if (Boolean.parseBoolean(
                    SystemProperties.get(TelephonyProperties.PROPERTY_INECM_MODE))) {

                mClickedPreference = preference;

                // In ECM mode launch ECM app dialog
                startActivityForResult(
                    new Intent(TelephonyIntents.ACTION_SHOW_NOTICE_ECM_BLOCK_OTHERS, null),
                    REQUEST_CODE_EXIT_ECM);
            }
            return true;
        } else if (preference == mButtonPreferredNetworkMode) {
            //displays the value taken from the Settings.System
            int settingsNetworkMode = getPreferredNetworkModeForSubId();
            mButtonPreferredNetworkMode.setValue(Integer.toString(settingsNetworkMode));
            return true;
        }  else if (preference == mButtonPreferredCMCCNetworkMode) {
            //displays the value taken from the Settings.System
            int settingsNetworkMode = getPreferredNetworkModeForSubId();
            mButtonPreferredCMCCNetworkMode.setValue(Integer.toString(settingsNetworkMode));
            return true;
        } else if (preference == mLteDataServicePref) {
            String tmpl = android.provider.Settings.Global.getString(getContentResolver(),
                        android.provider.Settings.Global.SETUP_PREPAID_DATA_SERVICE_URL);
            if (!TextUtils.isEmpty(tmpl)) {
                TelephonyManager tm = (TelephonyManager) getSystemService(
                        Context.TELEPHONY_SERVICE);
                String imsi = tm.getSubscriberId();
                if (imsi == null) {
                    imsi = "";
                }
                final String url = TextUtils.isEmpty(tmpl) ? null
                        : TextUtils.expandTemplate(tmpl, imsi).toString();
                Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
                startActivity(intent);
            } else {
                android.util.Log.e(LOG_TAG, "Missing SETUP_PREPAID_DATA_SERVICE_URL");
            }
            return true;
        }  else if (preference == mButtonEnabledNetworks) {
            int settingsNetworkMode = getPreferredNetworkModeForSubId();
            mButtonEnabledNetworks.setValue(Integer.toString(settingsNetworkMode));
            return true;
        } else if (preference == mButtonDataRoam) {
            // Do not disable the preference screen if the user clicks Data roaming.
            return true;
        } else {
            // if the button is anything but the simple toggle preference,
            // we'll need to disable all preferences to reject all click
            // events until the sub-activity's UI comes up.
            preferenceScreen.setEnabled(false);
            // Let the intents be launched by the Preference manager
            return false;
        }
    }

    private final SubscriptionManager.OnSubscriptionsChangedListener mOnSubscriptionsChangeListener
            = new SubscriptionManager.OnSubscriptionsChangedListener() {
        @Override
        public void onSubscriptionsChanged() {
            if (DBG) log("onSubscriptionsChanged:");
            List<SubscriptionInfo> newSil = mSubscriptionManager.getActiveSubscriptionInfoList();
            if (DBG) log("onSubscriptionsChanged: newSil: " + newSil +
                    " mActiveSubInfos: " + mActiveSubInfos);
            if (newSil == null) {
                return;
            }
            // Update UI when there is a change in number of active subscriptions or
            // there is a change in display name or subID
            if (mActiveSubInfos == null || mActiveSubInfos.size() != newSil.size()) {
                initializeSubscriptions();
            } else {
                boolean subsChanged = false;
                outer:
                for (SubscriptionInfo si : mActiveSubInfos) {
                    for(SubscriptionInfo newSi : newSil) {
                        //Compare SubscriptionInfo of same slot
                        if (si.getSimSlotIndex() == newSi.getSimSlotIndex()) {
                            if (DBG) log("onSubscriptionsChanged: Slot matched SimSlotIndex: "
                                    + si.getSimSlotIndex());
                            if (!newSi.getDisplayName().equals(si.getDisplayName()) ||
                                    newSi.getSubscriptionId() != si.getSubscriptionId()) {
                                if (DBG) log("onSubscriptionsChanged: subs changed ");
                                subsChanged = true;
                                break outer;
                            }
                        }
                    }
                }
                if (subsChanged) {
                    initializeSubscriptions();
                }
            }
        }
    };

    private void initializeSubscriptions() {
        if (isDestroyed()) { // Process preferences in activity only if its not destroyed
            return;
        }
        int currentTab = 0;
        if (DBG) log("initializeSubscriptions:+");

        // Before updating the the active subscription list check
        // if tab updating is needed as the list is changing.
        List<SubscriptionInfo> sil = mSubscriptionManager.getActiveSubscriptionInfoList();
        // Display all tabs according to configuration in Multi Sim mode
        TelephonyManager tm = (TelephonyManager) getSystemService(
                Context.TELEPHONY_SERVICE);
        int phoneCount = tm.getPhoneCount();
        TabState state = TabState.UPDATE;
        if (phoneCount < 2) {
            state = isUpdateTabsNeeded(sil);
        }

        // Update to the active subscription list
        mActiveSubInfos.clear();
        if (sil != null) {
            mActiveSubInfos.addAll(sil);
            // If there is only 1 sim then currenTab should represent slot no. of the sim.
            if (sil.size() == 1) {
                currentTab = sil.get(0).getSimSlotIndex();
            }
        }

        switch (state) {
            case UPDATE: {
                if (DBG) log("initializeSubscriptions: UPDATE");
                currentTab = mTabHost != null ? mTabHost.getCurrentTab() : 0;

                setContentView(com.android.internal.R.layout.common_tab_settings);

                mTabHost = (TabHost) findViewById(android.R.id.tabhost);
                mTabHost.setup();

                // Update the tabName. Get tab name from SubscriptionInfo,
                // if SubscriptionInfo not available get default tabName for that slot.
                for (int simSlotIndex = 0; simSlotIndex < phoneCount; simSlotIndex++) {
                    String tabName = null;
                    for(SubscriptionInfo si : mActiveSubInfos) {
                        if (DBG) log("initializeSubscriptions: si: " + si);
                        if (si != null && si.getSimSlotIndex() == simSlotIndex) {
                            // Slot is not empty and we match
                            tabName = String.valueOf(si.getDisplayName());
                            break;
                        }
                    }
                    if (tabName == null) {
                        try {
                            Context con = createPackageContext("com.android.settings", 0);
                            int id = con.getResources().getIdentifier("sim_editor_title",
                                    "string", "com.android.settings");
                            tabName = con.getResources().getString(id, simSlotIndex + 1);
                        } catch (NameNotFoundException e) {
                            loge("NameNotFoundException for sim_editor_title");
                            tabName = tabDefaultLabel + simSlotIndex;
                        }
                    }
                    if (DBG) {
                        log("initializeSubscriptions: tab=" + simSlotIndex + " name=" + tabName);
                    }

                    mTabHost.addTab(buildTabSpec(String.valueOf(simSlotIndex), tabName));
                }

                mTabHost.setOnTabChangedListener(mTabListener);
                mTabHost.setCurrentTab(currentTab);
                break;
            }
            case NO_TABS: {
                if (DBG) log("initializeSubscriptions: NO_TABS");

                if (mTabHost != null) {
                    mTabHost.clearAllTabs();
                    mTabHost = null;
                }
                setContentView(com.android.internal.R.layout.common_tab_settings);
                break;
            }
            case DO_NOTHING: {
                if (DBG) log("initializeSubscriptions: DO_NOTHING");
                if (mTabHost != null) {
                    currentTab = mTabHost.getCurrentTab();
                }
                break;
            }
        }
        updatePhone(currentTab);
        updateBody();
        if (DBG) log("initializeSubscriptions:-");
    }

    private enum TabState {
        NO_TABS, UPDATE, DO_NOTHING
    }
    private TabState isUpdateTabsNeeded(List<SubscriptionInfo> newSil) {
        TabState state = TabState.DO_NOTHING;
        if (newSil == null) {
            if (mActiveSubInfos.size() >= TAB_THRESHOLD) {
                if (DBG) log("isUpdateTabsNeeded: NO_TABS, size unknown and was tabbed");
                state = TabState.NO_TABS;
            }
        } else if (newSil.size() < TAB_THRESHOLD && mActiveSubInfos.size() >= TAB_THRESHOLD) {
            if (DBG) log("isUpdateTabsNeeded: NO_TABS, size went to small");
            state = TabState.NO_TABS;
        } else if (newSil.size() >= TAB_THRESHOLD && mActiveSubInfos.size() < TAB_THRESHOLD) {
            if (DBG) log("isUpdateTabsNeeded: UPDATE, size changed");
            state = TabState.UPDATE;
        } else if (newSil.size() >= TAB_THRESHOLD) {
            Iterator<SubscriptionInfo> siIterator = mActiveSubInfos.iterator();
            for(SubscriptionInfo newSi : newSil) {
                SubscriptionInfo curSi = siIterator.next();
                if (!newSi.getDisplayName().equals(curSi.getDisplayName())) {
                    if (DBG) log("isUpdateTabsNeeded: UPDATE, new name=" + newSi.getDisplayName());
                    state = TabState.UPDATE;
                    break;
                }
            }
        }
        if (DBG) {
            log("isUpdateTabsNeeded:- " + state
                + " newSil.size()=" + ((newSil != null) ? newSil.size() : 0)
                + " mActiveSubInfos.size()=" + mActiveSubInfos.size());
        }
        return state;
    }

    private OnTabChangeListener mTabListener = new OnTabChangeListener() {
        @Override
        public void onTabChanged(String tabId) {
            if (DBG) log("onTabChanged: " + tabId);
            // The User has changed tab; update the body.
            updatePhone(Integer.parseInt(tabId));
            updateBody();
        }
    };

    private void updatePhone(int slotId) {
        mPhone = PhoneFactory.getPhone(slotId);
        if (mPhone == null) {
            // Do the best we can
            mPhone = PhoneFactory.getDefaultPhone();
        }
        preferredNetworkMode = getPreferredNetworkModeForPhoneId();
        if (DBG) log("updatePhone:- slotId=" + slotId);
    }

    private TabContentFactory mEmptyTabContent = new TabContentFactory() {
        @Override
        public View createTabContent(String tag) {
            return new View(mTabHost.getContext());
        }
    };

    private TabSpec buildTabSpec(String tag, String title) {
        return mTabHost.newTabSpec(tag).setIndicator(title).setContent(
                mEmptyTabContent);
    }

    @Override
    protected void onCreate(Bundle icicle) {
        if (DBG) log("onCreate:+");
        setTheme(android.R.style.Theme_Material_Settings);
        super.onCreate(icicle);

        mHandler = new MyHandler();
        mUm = (UserManager) getSystemService(Context.USER_SERVICE);
        mSubscriptionManager = SubscriptionManager.from(this);
        mConfigManager = (CarrierConfigManager)
                getSystemService(Context.CARRIER_CONFIG_SERVICE);
        if (mUm.hasUserRestriction(UserManager.DISALLOW_CONFIG_MOBILE_NETWORKS)) {
            mUnavailable = true;
            setContentView(R.layout.telephony_disallowed_preference_screen);
            return;
        }

        addPreferencesFromResource(R.xml.network_setting);

        mButton4glte = (SwitchPreference)findPreference(BUTTON_4G_LTE_KEY);
        mButton4glte.setOnPreferenceChangeListener(this);
        mButtonEnable4g = (SwitchPreference)findPreference(BUTTON_ENABLE_4G_KEY);

        try {
            Context con = createPackageContext("com.android.systemui", 0);
            int id = con.getResources().getIdentifier("config_show4GForLTE",
                    "bool", "com.android.systemui");
            mShow4GForLTE = con.getResources().getBoolean(id);
        } catch (NameNotFoundException e) {
            loge("NameNotFoundException for show4GFotLTE");
            mShow4GForLTE = false;
        }

        //get UI object references
        PreferenceScreen prefSet = getPreferenceScreen();

        mButtonDataRoam = (RestrictedSwitchPreference) prefSet.findPreference(BUTTON_ROAMING_KEY);
        mButtonPreferredNetworkMode = (ListPreference) prefSet.findPreference(
                BUTTON_PREFERED_NETWORK_MODE);
        mButtonEnabledNetworks = (ListPreference) prefSet.findPreference(
                BUTTON_ENABLED_NETWORKS_KEY);
        mButtonPreferredCMCCNetworkMode = (ListPreference) prefSet.findPreference(
                BUTTON_PREFERED_NETWORK_CMCC_MODE);
        mButtonDataRoam.setOnPreferenceChangeListener(this);

        mLteDataServicePref = prefSet.findPreference(BUTTON_CDMA_LTE_DATA_SERVICE_KEY);

        mButtonUplmnPref = prefSet.findPreference(BUTTON_UPLMN_KEY);

        // Initialize mActiveSubInfo
        int max = mSubscriptionManager.getActiveSubscriptionInfoCountMax();
        mActiveSubInfos = new ArrayList<SubscriptionInfo>(max);

        initializeSubscriptions();

        if (mIsCTClassC) {
            mButtonEnable4g.setOnPreferenceChangeListener(this);
            updateButtonEnable4g();
        } else {
            prefSet.removePreference(mButtonEnable4g);
            mButtonEnable4g = null;
        }

        IntentFilter intentFilter = new IntentFilter(
                TelephonyIntents.ACTION_RADIO_TECHNOLOGY_CHANGED);
        intentFilter.addAction(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        intentFilter.addAction(Intent.ACTION_AIRPLANE_MODE_CHANGED);
        registerReceiver(mPhoneChangeReceiver, intentFilter);
        if (DBG) log("onCreate:-");
    }

    private boolean checkForCtCard(String iccId) {
        // iin length is 6 or 7
        if (iccId == null || iccId.length() < 6) {
            return false;
        }
        boolean isCtCard = false;
        String ctIccIdPrefix = iccId.substring(0, 6);
        String[] ctIccIdList = getResources().getStringArray(R.array.ct_iccid_prefix_list);
        if (ctIccIdList != null) {
            for (String iccidPrefix : ctIccIdList) {
                if (ctIccIdPrefix.equals(iccidPrefix)) {
                    isCtCard = true;
                    break;
                }
            }
        }
        return isCtCard;
    }

    private void handleEnable4gChange() {
        int networkType = Phone.NT_MODE_GSM_ONLY;
        boolean isCtCard = false;

        //Disable the option until response is received
        mButtonEnable4g.setEnabled(false);

        int subId = mPhone.getSubId();
        List<SubscriptionInfo> sirList =
                    SubscriptionManager.from(mPhone.getContext()).getActiveSubscriptionInfoList();
        if (sirList != null ) {
            for (SubscriptionInfo sir : sirList) {
                if (sir != null && sir.getSimSlotIndex() >= 0
                        && sir.getSubscriptionId() == subId) {
                    String iccId = sir.getIccId();
                    isCtCard = checkForCtCard(iccId);
                    break;
                }
            }
        }

        if (isCtCard) {
            networkType = mButtonEnable4g.isChecked() ? Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA
                    : Phone.NT_MODE_GLOBAL;
        } else {
            networkType = mButtonEnable4g.isChecked() ? Phone.NT_MODE_LTE_TDSCDMA_GSM_WCDMA
                    : Phone.NT_MODE_TDSCDMA_GSM_WCDMA;
        }
        log("Enable 4G option: isCtCard - " + isCtCard + ", set networkType - " + networkType);
        if (getPreferenceScreen().findPreference(BUTTON_PREFERED_NETWORK_MODE) != null)  {
            mButtonPreferredNetworkMode.setValue(Integer.toString(networkType));
        }
        if (getPreferenceScreen().findPreference(BUTTON_ENABLED_NETWORKS_KEY) != null)  {
            mButtonEnabledNetworks.setValue(Integer.toString(networkType));
        }
        mPhone.setPreferredNetworkType(networkType,
                mHandler.obtainMessage(MyHandler.MESSAGE_SET_PREFERRED_NETWORK_TYPE));
    }

    private boolean isNwModeLte() {
        //if present nwMode is LTE return true.
        int type = getPreferredNetworkModeForPhoneId();
        if (type == Phone.NT_MODE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA
                || type == Phone.NT_MODE_LTE_TDSCDMA_GSM_WCDMA
                || type == Phone.NT_MODE_LTE_TDSCDMA_WCDMA
                || type == Phone.NT_MODE_LTE_TDSCDMA_GSM
                || type == Phone.NT_MODE_LTE_TDSCDMA
                || type == Phone.NT_MODE_LTE_WCDMA
                || type == Phone.NT_MODE_LTE_ONLY
                || type == Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA
                || type == Phone.NT_MODE_LTE_GSM_WCDMA
                || type == Phone.NT_MODE_LTE_CDMA_AND_EVDO
                ) {
            return true;
        }
        return false;
    }

    private void updateButtonEnable4g() {
        if (mButtonEnable4g == null) {
            return;
        }
        boolean enabled = false;

        mButtonEnable4g.setChecked(isNwModeLte());

        int simState = TelephonyManager.getDefault().getSimState(mPhone.getPhoneId());
        enabled = (android.provider.Settings.System.getInt(mPhone.getContext().getContentResolver(),
                android.provider.Settings.System.AIRPLANE_MODE_ON, 0) == 0)
                && (simState == TelephonyManager.SIM_STATE_READY);

        //if there is no valid subid to get IccId info, disable the option.
        int subId = mPhone.getSubId();
        List<SubscriptionInfo> sirList =
                    SubscriptionManager.from(mPhone.getContext()).getActiveSubscriptionInfoList();
        if (enabled && sirList != null ) {
            for (SubscriptionInfo sir : sirList) {
                if (sir != null && sir.getSimSlotIndex() >= 0
                        && sir.getSubscriptionId() == subId) {
                    String iccId = sir.getIccId();
                    enabled = (iccId != null && iccId.length() >= 6) ? true : false;
                    break;
                }
            }
        }
        mButtonEnable4g.setEnabled(enabled);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(mPhoneChangeReceiver);
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (DBG) log("onResume:+");

        if (mUnavailable) {
            if (DBG) log("onResume:- ignore mUnavailable == false");
            return;
        }

        setScreenState();
        preferredNetworkMode = getPreferredNetworkModeForPhoneId();

        // Set UI state in onResume because a user could go home, launch some
        // app to change this setting's backend, and re-launch this settings app
        // and the UI state would be inconsistent with actual state
        mButtonDataRoam.setChecked(mPhone.getDataRoamingEnabled());
        updateButtonEnable4g();

        if (getPreferenceScreen().findPreference(BUTTON_PREFERED_NETWORK_MODE) != null
                || getPreferenceScreen().findPreference(BUTTON_ENABLED_NETWORKS_KEY) != null)  {
            updatePreferredNetworkUIFromDb();
        }

        if (ImsManager.isVolteEnabledByPlatform(this)
                && ImsManager.isVolteProvisionedOnDevice(this)) {
            Phone[] phones = PhoneFactory.getPhones();
            for (Phone phone : phones) {
                listenForCallState(phone.getSubId(), PhoneStateListener.LISTEN_CALL_STATE);
            }
        }

        // NOTE: Buttons will be enabled/disabled in mPhoneStateListener
        boolean enh4glteMode = ImsManager.isEnhanced4gLteModeSettingEnabledByUser(this)
                && ImsManager.isNonTtyOrTtyOnVolteEnabled(this);
        mButton4glte.setChecked(enh4glteMode);

        mSubscriptionManager.addOnSubscriptionsChangedListener(mOnSubscriptionsChangeListener);

        if (DBG) log("onResume:-");

    }

    private boolean hasActiveSubscriptions() {
        boolean isActive = false;
        int subId = mPhone.getSubId();
        for(SubscriptionInfo si : mActiveSubInfos) {
            if (si.getSubscriptionId() == subId) {
                isActive = true;
            }
        }
        return isActive;
    }

    private void updateBody() {
        final Context context = getApplicationContext();
        PreferenceScreen prefSet = getPreferenceScreen();
        boolean isLteOnCdma = mPhone.getLteOnCdmaMode() == PhoneConstants.LTE_ON_CDMA_TRUE;
        final int phoneSubId = mPhone.getSubId();
        PersistableBundle carrierConfig =
                mConfigManager.getConfigForSubId(phoneSubId);

        if (DBG) {
            log("updateBody: isLteOnCdma=" + isLteOnCdma + " phoneSubId=" + phoneSubId);
        }

        if (prefSet != null) {
            prefSet.removeAll();
            if (getResources().getBoolean(R.bool.config_uplmn_for_usim)) {
                mButtonUplmnPref.getIntent().putExtra(PhoneConstants.PHONE_KEY,
                            mPhone.getPhoneId());
                prefSet.addPreference(mButtonUplmnPref);
            }
            prefSet.addPreference(mButtonDataRoam);
            prefSet.addPreference(mButtonPreferredNetworkMode);
            prefSet.addPreference(mButtonEnabledNetworks);
            prefSet.addPreference(mButton4glte);

            /** Some carriers required that network mode UI need to be hidden in below conditions:
                    * 1. The absence of sim cards.
                    * 2. Non-USIM card is inserted.
                    * 3. Network type is GSM only or Global
                    */
            if (mIsCTClassC) {
                prefSet.addPreference(mButtonEnable4g);
            } else if (mIsCMCC && !SystemProperties.getBoolean
                    (PRIMARY_4G_CARD_PROPERTY_NAME, false)) {
                int phoneId = mPhone.getPhoneId();
                prefSet.addPreference(mButtonPreferredCMCCNetworkMode);
                prefSet.removePreference(mButtonPreferredNetworkMode);
                prefSet.removePreference(mButtonEnabledNetworks);
                log(" phoneId: " + phoneId + " and hide_network_settings_bool is: "
                        + carrierConfig.getBoolean(KEY_HIDE_NETWORK_SETTINGS_BOOL));
                if (UiccController.getInstance().getUiccCard(phoneId) == null ||
                    !carrierConfig.getBoolean(KEY_HIDE_NETWORK_SETTINGS_BOOL)) {
                    prefSet.removePreference(mButtonPreferredCMCCNetworkMode);
                }
            }
        }

        setScreenState();

        int settingsNetworkMode = getPreferredNetworkModeForSubId();
        mIsGlobalCdma = isLteOnCdma
                && carrierConfig.getBoolean(CarrierConfigManager.KEY_SHOW_CDMA_CHOICES_BOOL);
        if (carrierConfig.getBoolean(CarrierConfigManager.KEY_HIDE_CARRIER_NETWORK_SETTINGS_BOOL)) {
            prefSet.removePreference(mButtonPreferredNetworkMode);
            prefSet.removePreference(mButtonEnabledNetworks);
            prefSet.removePreference(mLteDataServicePref);
        } else if (carrierConfig.getBoolean(CarrierConfigManager
                    .KEY_HIDE_PREFERRED_NETWORK_TYPE_BOOL)
                && !mPhone.getServiceState().getRoaming()) {
            prefSet.removePreference(mButtonPreferredNetworkMode);
            prefSet.removePreference(mButtonEnabledNetworks);

            final int phoneType = mPhone.getPhoneType();
            if (phoneType == PhoneConstants.PHONE_TYPE_CDMA) {
                mCdmaOptions = new CdmaOptions(this, prefSet, mPhone);
                // In World mode force a refresh of GSM Options.
                if (isWorldMode()) {
                    mGsmUmtsOptions = null;
                }
            } else if (phoneType == PhoneConstants.PHONE_TYPE_GSM) {
                mGsmUmtsOptions = new GsmUmtsOptions(this, prefSet, phoneSubId);
            } else {
                throw new IllegalStateException("Unexpected phone type: " + phoneType);
            }
            // Since pref is being hidden from user, set network mode to default
            // in case it is currently something else. That is possible if user
            // changed the setting while roaming and is now back to home network.
            settingsNetworkMode = preferredNetworkMode;
        } else if (carrierConfig.getBoolean(CarrierConfigManager.KEY_WORLD_PHONE_BOOL) == true) {
            prefSet.removePreference(mButtonEnabledNetworks);
            // set the listener for the mButtonPreferredNetworkMode list preference so we can issue
            // change Preferred Network Mode.
            if (PRIMCARYCARD_L_W_ENABLED) {
                mButtonPreferredNetworkMode.setEntries(
                        R.array.preferred_network_mode_choices);
                mButtonPreferredNetworkMode.setEntryValues(
                        R.array.preferred_network_mode_values);
            }
            mButtonPreferredNetworkMode.setOnPreferenceChangeListener(this);

            mCdmaOptions = new CdmaOptions(this, prefSet, mPhone);
            mGsmUmtsOptions = new GsmUmtsOptions(this, prefSet, phoneSubId);
        } else {
            prefSet.removePreference(mButtonPreferredNetworkMode);
            final int phoneType = mPhone.getPhoneType();
            if (phoneType == PhoneConstants.PHONE_TYPE_CDMA) {
                int lteForced = android.provider.Settings.Global.getInt(
                        mPhone.getContext().getContentResolver(),
                        android.provider.Settings.Global.LTE_SERVICE_FORCED + mPhone.getSubId(),
                        0);

                if (isLteOnCdma) {
                    if (lteForced == 0) {
                        mButtonEnabledNetworks.setEntries(
                                R.array.enabled_networks_cdma_choices);
                        mButtonEnabledNetworks.setEntryValues(
                                R.array.enabled_networks_cdma_values);
                    } else {
                        switch (settingsNetworkMode) {
                            case Phone.NT_MODE_CDMA:
                            case Phone.NT_MODE_CDMA_NO_EVDO:
                            case Phone.NT_MODE_EVDO_NO_CDMA:
                                mButtonEnabledNetworks.setEntries(
                                        R.array.enabled_networks_cdma_no_lte_choices);
                                mButtonEnabledNetworks.setEntryValues(
                                        R.array.enabled_networks_cdma_no_lte_values);
                                break;
                            case Phone.NT_MODE_GLOBAL:
                            case Phone.NT_MODE_LTE_CDMA_AND_EVDO:
                            case Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
                            case Phone.NT_MODE_LTE_ONLY:
                                mButtonEnabledNetworks.setEntries(
                                        R.array.enabled_networks_cdma_only_lte_choices);
                                mButtonEnabledNetworks.setEntryValues(
                                        R.array.enabled_networks_cdma_only_lte_values);
                                break;
                            default:
                                mButtonEnabledNetworks.setEntries(
                                        R.array.enabled_networks_cdma_choices);
                                mButtonEnabledNetworks.setEntryValues(
                                        R.array.enabled_networks_cdma_values);
                                break;
                        }
                    }
                }
                mCdmaOptions = new CdmaOptions(this, prefSet, mPhone);

                // In World mode force a refresh of GSM Options.
                if (isWorldMode()) {
                    mGsmUmtsOptions = null;
                }
            } else if (phoneType == PhoneConstants.PHONE_TYPE_GSM) {
                if (isSupportTdscdma()) {
                    mButtonEnabledNetworks.setEntries(
                            R.array.enabled_networks_tdscdma_choices);
                    mButtonEnabledNetworks.setEntryValues(
                            R.array.enabled_networks_tdscdma_values);
                } else if (!carrierConfig.getBoolean(CarrierConfigManager.KEY_PREFER_2G_BOOL)
                        && !getResources().getBoolean(R.bool.config_enabled_lte)) {
                    mButtonEnabledNetworks.setEntries(
                            R.array.enabled_networks_except_gsm_lte_choices);
                    mButtonEnabledNetworks.setEntryValues(
                            R.array.enabled_networks_except_gsm_lte_values);
                } else if (!carrierConfig.getBoolean(CarrierConfigManager.KEY_PREFER_2G_BOOL)) {
                    int select = (mShow4GForLTE == true) ?
                            R.array.enabled_networks_except_gsm_4g_choices
                            : R.array.enabled_networks_except_gsm_choices;
                    mButtonEnabledNetworks.setEntries(select);
                    mButtonEnabledNetworks.setEntryValues(
                            R.array.enabled_networks_except_gsm_values);
                } else if (!getResources().getBoolean(R.bool.config_enabled_lte)) {
                    mButtonEnabledNetworks.setEntries(
                            R.array.enabled_networks_except_lte_choices);
                    mButtonEnabledNetworks.setEntryValues(
                            R.array.enabled_networks_except_lte_values);
                } else if (mIsGlobalCdma) {
                    mButtonEnabledNetworks.setEntries(
                            R.array.enabled_networks_cdma_choices);
                    mButtonEnabledNetworks.setEntryValues(
                            R.array.enabled_networks_cdma_values);
                } else {
                    int select = (mShow4GForLTE == true) ? R.array.enabled_networks_4g_choices
                            : R.array.enabled_networks_choices;
                    mButtonEnabledNetworks.setEntries(select);
                    mButtonEnabledNetworks.setEntryValues(
                            R.array.enabled_networks_values);
                }
                mGsmUmtsOptions = new GsmUmtsOptions(this, prefSet, phoneSubId);
            } else {
                throw new IllegalStateException("Unexpected phone type: " + phoneType);
            }
            if (isWorldMode()) {
                mButtonEnabledNetworks.setEntries(
                        R.array.preferred_network_mode_choices_world_mode);
                mButtonEnabledNetworks.setEntryValues(
                        R.array.preferred_network_mode_values_world_mode);
            }
            mButtonEnabledNetworks.setOnPreferenceChangeListener(this);
            if (DBG) log("settingsNetworkMode: " + settingsNetworkMode);
        }

        final boolean missingDataServiceUrl = TextUtils.isEmpty(
                android.provider.Settings.Global.getString(getContentResolver(),
                        android.provider.Settings.Global.SETUP_PREPAID_DATA_SERVICE_URL));
        if (!isLteOnCdma || missingDataServiceUrl) {
            prefSet.removePreference(mLteDataServicePref);
        } else {
            android.util.Log.d(LOG_TAG, "keep ltePref");
        }

        // Enable enhanced 4G LTE mode settings depending on whether exists on platform
        // or in multi-sim cases, depending on whether ImsPhone exists for that subscription
        if (!(ImsManager.isVolteEnabledByPlatform(this)
                && ImsManager.isVolteProvisionedOnDevice(this)) || (mPhone.getImsPhone() == null)) {
            Preference pref = prefSet.findPreference(BUTTON_4G_LTE_KEY);
            if (pref != null) {
                prefSet.removePreference(pref);
            }
        }

        ActionBar actionBar = getActionBar();
        if (actionBar != null) {
            // android.R.id.home will be triggered in onOptionsItemSelected()
            actionBar.setDisplayHomeAsUpEnabled(true);
        }

        // Enable link to CMAS app settings depending on the value in config.xml.
        final boolean isCellBroadcastAppLinkEnabled = this.getResources().getBoolean(
                com.android.internal.R.bool.config_cellBroadcastAppLinks);
        if (!mUm.isAdminUser() || !isCellBroadcastAppLinkEnabled
                || mUm.hasUserRestriction(UserManager.DISALLOW_CONFIG_CELL_BROADCASTS)) {
            PreferenceScreen root = getPreferenceScreen();
            Preference ps = findPreference(BUTTON_CELL_BROADCAST_SETTINGS);
            if (ps != null) {
                root.removePreference(ps);
            }
        }

        // Get the networkMode from Settings.System and displays it
        mButtonDataRoam.setChecked(mPhone.getDataRoamingEnabled());
        mButtonEnabledNetworks.setValue(Integer.toString(settingsNetworkMode));
        mButtonPreferredNetworkMode.setValue(Integer.toString(settingsNetworkMode));
        mButtonPreferredCMCCNetworkMode.setValue(Integer.toString(settingsNetworkMode));
        UpdatePreferredNetworkModeSummary(settingsNetworkMode);
        UpdateEnabledNetworksValueAndSummary(settingsNetworkMode);
        UpdatePreferredCMCCNetworkModeSummary(settingsNetworkMode);

        /**
         * Enable/disable depending upon if there are any active subscriptions.
         *
         * I've decided to put this enable/disable code at the bottom as the
         * code above works even when there are no active subscriptions, thus
         * putting it afterwards is a smaller change. This can be refined later,
         * but you do need to remember that this all needs to work when subscriptions
         * change dynamically such as when hot swapping sims.
         */
        boolean hasActiveSubscriptions = hasActiveSubscriptions();
        TelephonyManager tm = (TelephonyManager) getSystemService(
                Context.TELEPHONY_SERVICE);
        boolean canChange4glte = (tm.getCallState() == TelephonyManager.CALL_STATE_IDLE) &&
                ImsManager.isNonTtyOrTtyOnVolteEnabled(getApplicationContext()) &&
                carrierConfig.getBoolean(CarrierConfigManager.KEY_EDITABLE_ENHANCED_4G_LTE_BOOL);
        mButtonDataRoam.setDisabledByAdmin(false);
        mButtonDataRoam.setEnabled(hasActiveSubscriptions);
        if (mButtonDataRoam.isEnabled()) {
            if (RestrictedLockUtils.hasBaseUserRestriction(context,
                    UserManager.DISALLOW_DATA_ROAMING, UserHandle.myUserId())) {
                mButtonDataRoam.setEnabled(false);
            } else {
                mButtonDataRoam.checkRestrictionAndSetDisabled(UserManager.DISALLOW_DATA_ROAMING);
            }
        }
        mButtonPreferredNetworkMode.setEnabled(hasActiveSubscriptions);
        mButtonEnabledNetworks.setEnabled(hasActiveSubscriptions);
        mButton4glte.setEnabled(hasActiveSubscriptions && canChange4glte);
        mLteDataServicePref.setEnabled(hasActiveSubscriptions);
        Preference ps;
        PreferenceScreen root = getPreferenceScreen();
        ps = findPreference(BUTTON_CELL_BROADCAST_SETTINGS);
        if (ps != null) {
            ps.setEnabled(hasActiveSubscriptions);
        }
        ps = findPreference(BUTTON_APN_EXPAND_KEY);
        if (ps != null) {
            ps.setEnabled(hasActiveSubscriptions);
        }
        ps = findPreference(BUTTON_OPERATOR_SELECTION_EXPAND_KEY);
        if (ps != null && !carrierConfig.getBoolean(CarrierConfigManager.
                KEY_SHOW_APN_SETTING_CDMA_BOOL)) {
            ps.setEnabled(hasActiveSubscriptions && mPhone.getPhoneType()
                == PhoneConstants.PHONE_TYPE_GSM);
        }
        ps = findPreference(BUTTON_CARRIER_SETTINGS_KEY);
        if (ps != null) {
            ps.setEnabled(hasActiveSubscriptions);
        }
        ps = findPreference(BUTTON_CDMA_SYSTEM_SELECT_KEY);
        if (ps != null) {
            ps.setEnabled(hasActiveSubscriptions);
        }

        // Disable nwMode option in UI if current SIM is non-primary card with GSM only.
        if (SystemProperties.getBoolean(PRIMARY_4G_CARD_PROPERTY_NAME, false)) {
            int phoneId = mPhone.getPhoneId();
            int currentPrimarySlot = Settings.Global.getInt(context.getContentResolver(),
                     CONFIG_CURRENT_PRIMARY_SUB, SubscriptionManager.INVALID_SIM_SLOT_INDEX);

            if (SubscriptionManager.isValidSlotId(currentPrimarySlot)
                    && (phoneId != currentPrimarySlot)) {
                if (PRIMCARYCARD_L_W_ENABLED) {
                    mButtonPreferredNetworkMode.setEntries(
                            R.array.preferred_network_mode_gsm_wcdma_choices);
                    mButtonPreferredNetworkMode.setEntryValues(
                            R.array.preferred_network_mode_gsm_wcdma_values);
                    mButtonEnabledNetworks.setEntries(
                            R.array.preferred_network_mode_gsm_wcdma_choices);
                    mButtonEnabledNetworks.setEntryValues(
                            R.array.preferred_network_mode_gsm_wcdma_values);
                } else if (getPreferredNetworkModeForPhoneId() == Phone.NT_MODE_GSM_ONLY) {
                    mButtonPreferredNetworkMode.setEnabled(false);
                    mButtonEnabledNetworks.setEnabled(false);
                }
            }
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (DBG) log("onPause:+");

        if (ImsManager.isVolteEnabledByPlatform(this)
                && ImsManager.isVolteProvisionedOnDevice(this)) {
            Phone[] phones = PhoneFactory.getPhones();
            for (Phone phone : phones) {
                listenForCallState(phone.getSubId(), PhoneStateListener.LISTEN_NONE);
            }
        }

        mSubscriptionManager
            .removeOnSubscriptionsChangedListener(mOnSubscriptionsChangeListener);
        if (DBG) log("onPause:-");
    }

    /**
     * Implemented to support onPreferenceChangeListener to look for preference
     * changes specifically on CLIR.
     *
     * @param preference is the preference to be changed, should be mButtonCLIR.
     * @param objValue should be the value of the selection, NOT its localized
     * display value.
     */
    public boolean onPreferenceChange(Preference preference, Object objValue) {
         if (DBG) log("onPreferenceChange");
        final int phoneSubId = mPhone.getSubId();
        if (preference == mButtonPreferredNetworkMode) {
            //NOTE onPreferenceChange seems to be called even if there is no change
            //Check if the button value is changed from the System.Setting
            mButtonPreferredNetworkMode.setValue((String) objValue);
            int buttonNetworkMode;
            buttonNetworkMode = Integer.parseInt((String) objValue);
            int settingsNetworkMode = getPreferredNetworkModeForSubId();
             if (DBG) log("buttonNetworkMode: " + buttonNetworkMode +
                    " settingsNetworkMode: " + settingsNetworkMode);
            if (buttonNetworkMode != settingsNetworkMode) {
                int modemNetworkMode;
                // if new mode is invalid ignore it
                switch (buttonNetworkMode) {
                    case Phone.NT_MODE_WCDMA_PREF:
                    case Phone.NT_MODE_GSM_ONLY:
                    case Phone.NT_MODE_WCDMA_ONLY:
                    case Phone.NT_MODE_GSM_UMTS:
                    case Phone.NT_MODE_CDMA:
                    case Phone.NT_MODE_CDMA_NO_EVDO:
                    case Phone.NT_MODE_EVDO_NO_CDMA:
                    case Phone.NT_MODE_GLOBAL:
                    case Phone.NT_MODE_LTE_CDMA_AND_EVDO:
                    case Phone.NT_MODE_LTE_GSM_WCDMA:
                    case Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
                    case Phone.NT_MODE_LTE_ONLY:
                    case Phone.NT_MODE_LTE_WCDMA:
                    case Phone.NT_MODE_TDSCDMA_ONLY:
                    case Phone.NT_MODE_TDSCDMA_WCDMA:
                    case Phone.NT_MODE_LTE_TDSCDMA:
                    case Phone.NT_MODE_TDSCDMA_GSM:
                    case Phone.NT_MODE_LTE_TDSCDMA_GSM:
                    case Phone.NT_MODE_TDSCDMA_GSM_WCDMA:
                    case Phone.NT_MODE_LTE_TDSCDMA_WCDMA:
                    case Phone.NT_MODE_LTE_TDSCDMA_GSM_WCDMA:
                    case Phone.NT_MODE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
                    case Phone.NT_MODE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
                        // This is one of the modes we recognize
                        modemNetworkMode = buttonNetworkMode;
                        break;
                    default:
                        loge("Invalid Network Mode (" + buttonNetworkMode + ") chosen. Ignore.");
                        return true;
                }
                UpdatePreferredNetworkModeSummary(buttonNetworkMode);
                //Set the modem network mode
                mPhone.setPreferredNetworkType(modemNetworkMode, mHandler
                        .obtainMessage(MyHandler.MESSAGE_SET_PREFERRED_NETWORK_TYPE));
            }
        } else if (preference == mButtonEnabledNetworks) {
            mButtonEnabledNetworks.setValue((String) objValue);
            int buttonNetworkMode;
            buttonNetworkMode = Integer.parseInt((String) objValue);
            if (DBG) log("buttonNetworkMode: " + buttonNetworkMode);
            int settingsNetworkMode = getPreferredNetworkModeForSubId();
            if (buttonNetworkMode != settingsNetworkMode) {
                int modemNetworkMode;
                // if new mode is invalid ignore it
                switch (buttonNetworkMode) {
                    case Phone.NT_MODE_WCDMA_PREF:
                    case Phone.NT_MODE_GSM_ONLY:
                    case Phone.NT_MODE_LTE_GSM_WCDMA:
                    case Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
                    case Phone.NT_MODE_CDMA:
                    case Phone.NT_MODE_CDMA_NO_EVDO:
                    case Phone.NT_MODE_LTE_CDMA_AND_EVDO:
                    case Phone.NT_MODE_TDSCDMA_ONLY:
                    case Phone.NT_MODE_TDSCDMA_WCDMA:
                    case Phone.NT_MODE_LTE_TDSCDMA:
                    case Phone.NT_MODE_TDSCDMA_GSM:
                    case Phone.NT_MODE_LTE_TDSCDMA_GSM:
                    case Phone.NT_MODE_TDSCDMA_GSM_WCDMA:
                    case Phone.NT_MODE_LTE_TDSCDMA_WCDMA:
                    case Phone.NT_MODE_LTE_TDSCDMA_GSM_WCDMA:
                    case Phone.NT_MODE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
                    case Phone.NT_MODE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
                        // This is one of the modes we recognize
                        modemNetworkMode = buttonNetworkMode;
                        break;
                    default:
                        loge("Invalid Network Mode (" + buttonNetworkMode + ") chosen. Ignore.");
                        return true;
                }

                UpdateEnabledNetworksValueAndSummary(buttonNetworkMode);

                //Set the modem network mode
                mPhone.setPreferredNetworkType(modemNetworkMode, mHandler
                        .obtainMessage(MyHandler.MESSAGE_SET_PREFERRED_NETWORK_TYPE));
            }
        } else if (preference == mButton4glte) {
            SwitchPreference enhanced4gModePref = (SwitchPreference) preference;
            boolean enhanced4gMode = !enhanced4gModePref.isChecked();
            enhanced4gModePref.setChecked(enhanced4gMode);
            ImsManager.setEnhanced4gLteModeSetting(this, enhanced4gModePref.isChecked());
        } else if (mButtonEnable4g != null && preference == mButtonEnable4g) {
            SwitchPreference enable4g = (SwitchPreference)preference;
            enable4g.setChecked(!enable4g.isChecked());
            handleEnable4gChange();
        } else if (preference == mButtonDataRoam) {
            if (DBG) log("onPreferenceTreeClick: preference == mButtonDataRoam.");

            //normally called on the toggle click
            if (!mButtonDataRoam.isChecked()) {
                // First confirm with a warning dialog about charges
                mOkClicked = false;
                new AlertDialog.Builder(this).setMessage(
                        getResources().getString(R.string.roaming_warning))
                        .setTitle(R.string.roaming_alert_title)
                        .setIconAttribute(android.R.attr.alertDialogIcon)
                        .setPositiveButton(android.R.string.yes, this)
                        .setNegativeButton(android.R.string.no, this)
                        .show()
                        .setOnDismissListener(this);
            } else {
                mPhone.setDataRoamingEnabled(false);
            }
            return true;
        }

        // always let the preference setting proceed.
        return true;
    }

    // Update network mode in DB for both SubId  and phoneId
    private void setPreferredNetworkMode(int nwMode) {
        final int phoneSubId = mPhone.getSubId();
        final int phoneId = mPhone.getPhoneId();
        if (DBG) log("setPreferredNetworkMode: nwMode = " + nwMode +
                " phoneSubId = " + phoneSubId + " phoneId = " + phoneId);
        preferredNetworkMode = nwMode;
        android.provider.Settings.Global.putInt(mPhone.getContext().getContentResolver(),
                android.provider.Settings.Global.PREFERRED_NETWORK_MODE + phoneSubId,
                nwMode );
        TelephonyManager.putIntAtIndex(mPhone.getContext().getContentResolver(),
                android.provider.Settings.Global.PREFERRED_NETWORK_MODE, phoneId,
                nwMode );
    }

    //Get preferred network mode based on phoneId
    private int getPreferredNetworkModeForPhoneId() {
        final int phoneId = mPhone.getPhoneId();
        int phoneNwMode;

        try {
            phoneNwMode = android.telephony.TelephonyManager.getIntAtIndex(
                    mPhone.getContext().getContentResolver(),
                    android.provider.Settings.Global.PREFERRED_NETWORK_MODE, phoneId);
        } catch (SettingNotFoundException snfe) {
            log("getPreferredNetworkModeForPhoneId: Could not find PREFERRED_NETWORK_MODE");
            phoneNwMode = Phone.PREFERRED_NT_MODE;
        }
        if (DBG) log("getPreferredNetworkModeForPhoneId: phoneNwMode = " + phoneNwMode +
                " phoneId = " + phoneId);
        return phoneNwMode;
    }

    //Get preferred network mode based on subId
    private int getPreferredNetworkModeForSubId() {
        final int subId = mPhone.getSubId();
        int phoneNwMode;
        int nwMode;

        nwMode = android.provider.Settings.Global.getInt(
                mPhone.getContext().getContentResolver(),
                android.provider.Settings.Global.PREFERRED_NETWORK_MODE + subId,
                preferredNetworkMode);
        if (DBG) log("getPreferredNetworkModeForSubId: phoneNwMode = " + nwMode +
                " subId = "+ subId);
        return nwMode;
    }

    private class MyHandler extends Handler {

        static final int MESSAGE_SET_PREFERRED_NETWORK_TYPE = 0;

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MESSAGE_SET_PREFERRED_NETWORK_TYPE:
                    handleSetPreferredNetworkTypeResponse(msg);
                    break;
            }
        }

        private void handleSetPreferredNetworkTypeResponse(Message msg) {
            if (isDestroyed()) { // Access preferences of activity only if it is not destroyed
                return;
            }

            AsyncResult ar = (AsyncResult) msg.obj;

            if (ar.exception == null) {
            log("handleSetPreferredNetworkTypeResponse: Sucess");
                int networkMode;
                if (getPreferenceScreen().findPreference(BUTTON_PREFERED_NETWORK_MODE) != null)  {
                    networkMode =  Integer.parseInt(mButtonPreferredNetworkMode.getValue());
                    setPreferredNetworkMode(networkMode);
                }
                if (getPreferenceScreen().findPreference(BUTTON_ENABLED_NETWORKS_KEY) != null)  {
                    networkMode = Integer.parseInt(mButtonEnabledNetworks.getValue());
                    setPreferredNetworkMode(networkMode);
                }
                updateButtonEnable4g();
            } else {
                if (DBG) {
                    log("handleSetPreferredNetworkTypeResponse: exception in setting network mode.");
                }
                updatePreferredNetworkUIFromDb();
            }
        }
    }

    private void updatePreferredNetworkUIFromDb() {
        final int phoneSubId = mPhone.getSubId();

        int settingsNetworkMode = getPreferredNetworkModeForSubId();

        if (DBG) {
            log("updatePreferredNetworkUIFromDb: settingsNetworkMode = " +
                    settingsNetworkMode);
        }

        UpdatePreferredNetworkModeSummary(settingsNetworkMode);
        UpdateEnabledNetworksValueAndSummary(settingsNetworkMode);
        // changes the mButtonPreferredNetworkMode accordingly to settingsNetworkMode
        mButtonPreferredNetworkMode.setValue(Integer.toString(settingsNetworkMode));
    }

    private void UpdatePreferredCMCCNetworkModeSummary(int NetworkMode) {
        if (Phone.NT_MODE_LTE_TDSCDMA_GSM_WCDMA == NetworkMode)
            mButtonPreferredCMCCNetworkMode.setSummary
                (R.string.preferred_network_mode_cmcc_summary);
    }

    private void UpdatePreferredNetworkModeSummary(int NetworkMode) {
        switch(NetworkMode) {
            case Phone.NT_MODE_TDSCDMA_GSM_WCDMA:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_tdscdma_gsm_wcdma_summary);
                break;
            case Phone.NT_MODE_TDSCDMA_GSM:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_tdscdma_gsm_summary);
                break;
            case Phone.NT_MODE_WCDMA_PREF:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_wcdma_perf_summary);
                break;
            case Phone.NT_MODE_GSM_ONLY:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_gsm_only_summary);
                break;
            case Phone.NT_MODE_TDSCDMA_WCDMA:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_tdscdma_wcdma_summary);
                break;
            case Phone.NT_MODE_WCDMA_ONLY:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_wcdma_only_summary);
                break;
            case Phone.NT_MODE_GSM_UMTS:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_gsm_wcdma_summary);
                break;
            case Phone.NT_MODE_CDMA:
                switch (mPhone.getLteOnCdmaMode()) {
                    case PhoneConstants.LTE_ON_CDMA_TRUE:
                        mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_cdma_summary);
                    break;
                    case PhoneConstants.LTE_ON_CDMA_FALSE:
                    default:
                        mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_cdma_evdo_summary);
                        break;
                }
                break;
            case Phone.NT_MODE_CDMA_NO_EVDO:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_cdma_only_summary);
                break;
            case Phone.NT_MODE_EVDO_NO_CDMA:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_evdo_only_summary);
                break;
            case Phone.NT_MODE_LTE_TDSCDMA:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_lte_tdscdma_summary);
                break;
            case Phone.NT_MODE_LTE_ONLY:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_lte_summary);
                break;
            case Phone.NT_MODE_LTE_TDSCDMA_GSM:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_lte_tdscdma_gsm_summary);
                break;
            case Phone.NT_MODE_LTE_TDSCDMA_GSM_WCDMA:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_lte_tdscdma_gsm_wcdma_summary);
                break;
            case Phone.NT_MODE_LTE_GSM_WCDMA:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_lte_gsm_wcdma_summary);
                break;
            case Phone.NT_MODE_LTE_CDMA_AND_EVDO:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_lte_cdma_evdo_summary);
                break;
            case Phone.NT_MODE_TDSCDMA_ONLY:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_tdscdma_summary);
                break;
            case Phone.NT_MODE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_lte_tdscdma_cdma_evdo_gsm_wcdma_summary);
                break;
            case Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
                if (mPhone.getPhoneType() == PhoneConstants.PHONE_TYPE_CDMA ||
                        mIsGlobalCdma ||
                        isWorldMode()) {
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_global_summary);
                } else {
                    mButtonPreferredNetworkMode.setSummary(
                            R.string.preferred_network_mode_lte_summary);
                }
                break;
            case Phone.NT_MODE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_tdscdma_cdma_evdo_gsm_wcdma_summary);
                break;
            case Phone.NT_MODE_GLOBAL:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_cdma_evdo_gsm_wcdma_summary);
                break;
            case Phone.NT_MODE_LTE_TDSCDMA_WCDMA:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_lte_tdscdma_wcdma_summary);
                break;
            case Phone.NT_MODE_LTE_WCDMA:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_lte_wcdma_summary);
                break;
            default:
                mButtonPreferredNetworkMode.setSummary(
                        R.string.preferred_network_mode_global_summary);
        }
    }

    private void UpdateEnabledNetworksValueAndSummary(int NetworkMode) {
        switch (NetworkMode) {
            case Phone.NT_MODE_TDSCDMA_WCDMA:
            case Phone.NT_MODE_TDSCDMA_GSM_WCDMA:
            case Phone.NT_MODE_TDSCDMA_GSM:
                mButtonEnabledNetworks.setValue(
                        Integer.toString(Phone.NT_MODE_TDSCDMA_GSM_WCDMA));
                mButtonEnabledNetworks.setSummary(R.string.network_3G);
                break;
            case Phone.NT_MODE_WCDMA_ONLY:
            case Phone.NT_MODE_GSM_UMTS:
            case Phone.NT_MODE_WCDMA_PREF:
                if (!mIsGlobalCdma) {
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_WCDMA_PREF));
                    mButtonEnabledNetworks.setSummary(R.string.network_3G);
                } else {
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA));
                    mButtonEnabledNetworks.setSummary(R.string.network_global);
                }
                break;
            case Phone.NT_MODE_GSM_ONLY:
                if (!mIsGlobalCdma) {
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_GSM_ONLY));
                    mButtonEnabledNetworks.setSummary(R.string.network_2G);
                } else {
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA));
                    mButtonEnabledNetworks.setSummary(R.string.network_global);
                }
                break;
            case Phone.NT_MODE_LTE_GSM_WCDMA:
                if (isWorldMode()) {
                    mButtonEnabledNetworks.setSummary(
                            R.string.preferred_network_mode_lte_gsm_umts_summary);
                    controlCdmaOptions(false);
                    controlGsmOptions(true);
                    break;
                }
            case Phone.NT_MODE_LTE_ONLY:
            case Phone.NT_MODE_LTE_WCDMA:
                if (!mIsGlobalCdma) {
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_LTE_GSM_WCDMA));
                    mButtonEnabledNetworks.setSummary((mShow4GForLTE == true)
                            ? R.string.network_4G : R.string.network_lte);
                } else {
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA));
                    mButtonEnabledNetworks.setSummary(R.string.network_global);
                }
                break;
            case Phone.NT_MODE_LTE_CDMA_AND_EVDO:
                if (isWorldMode()) {
                    mButtonEnabledNetworks.setSummary(
                            R.string.preferred_network_mode_lte_cdma_summary);
                    controlCdmaOptions(true);
                    controlGsmOptions(false);
                } else {
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_LTE_CDMA_AND_EVDO));
                    mButtonEnabledNetworks.setSummary(R.string.network_lte);
                }
                break;
            case Phone.NT_MODE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
                mButtonEnabledNetworks.setValue(
                        Integer.toString(Phone.NT_MODE_TDSCDMA_CDMA_EVDO_GSM_WCDMA));
                mButtonEnabledNetworks.setSummary(R.string.network_3G);
                break;
            case Phone.NT_MODE_CDMA:
            case Phone.NT_MODE_EVDO_NO_CDMA:
            case Phone.NT_MODE_GLOBAL:
                mButtonEnabledNetworks.setValue(
                        Integer.toString(Phone.NT_MODE_CDMA));
                mButtonEnabledNetworks.setSummary(R.string.network_3G);
                break;
            case Phone.NT_MODE_CDMA_NO_EVDO:
                mButtonEnabledNetworks.setValue(
                        Integer.toString(Phone.NT_MODE_CDMA_NO_EVDO));
                mButtonEnabledNetworks.setSummary(R.string.network_1x);
                break;
            case Phone.NT_MODE_TDSCDMA_ONLY:
                mButtonEnabledNetworks.setValue(
                        Integer.toString(Phone.NT_MODE_TDSCDMA_ONLY));
                mButtonEnabledNetworks.setSummary(R.string.network_3G);
                break;
            case Phone.NT_MODE_LTE_TDSCDMA_GSM:
            case Phone.NT_MODE_LTE_TDSCDMA_GSM_WCDMA:
            case Phone.NT_MODE_LTE_TDSCDMA:
            case Phone.NT_MODE_LTE_TDSCDMA_WCDMA:
            case Phone.NT_MODE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
            case Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
                if (isSupportTdscdma()) {
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA));
                    mButtonEnabledNetworks.setSummary(R.string.network_lte);
                } else {
                    if (isWorldMode()) {
                        controlCdmaOptions(true);
                        controlGsmOptions(false);
                    }
                    mButtonEnabledNetworks.setValue(
                            Integer.toString(Phone.NT_MODE_LTE_CDMA_EVDO_GSM_WCDMA));
                    if (mPhone.getPhoneType() == PhoneConstants.PHONE_TYPE_CDMA ||
                            mIsGlobalCdma ||
                            isWorldMode()) {
                        mButtonEnabledNetworks.setSummary(R.string.network_global);
                    } else {
                        mButtonEnabledNetworks.setSummary((mShow4GForLTE == true)
                                ? R.string.network_4G : R.string.network_lte);
                    }
                }
                break;
            default:
                String errMsg = "Invalid Network Mode (" + NetworkMode + "). Ignore.";
                loge(errMsg);
                mButtonEnabledNetworks.setSummary(errMsg);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch(requestCode) {
        case REQUEST_CODE_EXIT_ECM:
            Boolean isChoiceYes =
                data.getBooleanExtra(EXTRA_EXIT_ECM_RESULT, false);
            if (isChoiceYes) {
                // If the phone exits from ECM mode, show the CDMA Options
                mCdmaOptions.showDialog(mClickedPreference);
            } else {
                // do nothing
            }
            break;

        default:
            break;
        }
    }

    private static void log(String msg) {
        Log.d(LOG_TAG, msg);
    }

    private static void loge(String msg) {
        Log.e(LOG_TAG, msg);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        final int itemId = item.getItemId();
        if (itemId == android.R.id.home) {  // See ActionBar#setDisplayHomeAsUpEnabled()
            // Commenting out "logical up" capability. This is a workaround for issue 5278083.
            //
            // Settings app may not launch this activity via UP_ACTIVITY_CLASS but the other
            // Activity that looks exactly same as UP_ACTIVITY_CLASS ("SubSettings" Activity).
            // At that moment, this Activity launches UP_ACTIVITY_CLASS on top of the Activity.
            // which confuses users.
            // TODO: introduce better mechanism for "up" capability here.
            /*Intent intent = new Intent(Intent.ACTION_MAIN);
            intent.setClassName(UP_ACTIVITY_PACKAGE, UP_ACTIVITY_CLASS);
            intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
            startActivity(intent);*/
            finish();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    private boolean isWorldMode() {
        boolean worldModeOn = false;
        final TelephonyManager tm = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
        final String configString = getResources().getString(R.string.config_world_mode);

        if (!TextUtils.isEmpty(configString)) {
            String[] configArray = configString.split(";");
            // Check if we have World mode configuration set to True only or config is set to True
            // and SIM GID value is also set and matches to the current SIM GID.
            if (configArray != null &&
                   ((configArray.length == 1 && configArray[0].equalsIgnoreCase("true")) ||
                       (configArray.length == 2 && !TextUtils.isEmpty(configArray[1]) &&
                           tm != null && configArray[1].equalsIgnoreCase(tm.getGroupIdLevel1())))) {
                               worldModeOn = true;
            }
        }

        if (DBG) {
            log("isWorldMode=" + worldModeOn);
        }

        return worldModeOn;
    }

    private void controlGsmOptions(boolean enable) {
        PreferenceScreen prefSet = getPreferenceScreen();
        if (prefSet == null) {
            return;
        }

        if (mGsmUmtsOptions == null) {
            mGsmUmtsOptions = new GsmUmtsOptions(this, prefSet, mPhone.getSubId());
        }
        PreferenceScreen apnExpand =
                (PreferenceScreen) prefSet.findPreference(BUTTON_APN_EXPAND_KEY);
        PreferenceScreen operatorSelectionExpand =
                (PreferenceScreen) prefSet.findPreference(BUTTON_OPERATOR_SELECTION_EXPAND_KEY);
        PreferenceScreen carrierSettings =
                (PreferenceScreen) prefSet.findPreference(BUTTON_CARRIER_SETTINGS_KEY);
        if (apnExpand != null) {
            apnExpand.setEnabled(isWorldMode() || enable);
        }
        if (operatorSelectionExpand != null) {
            if (enable) {
                operatorSelectionExpand.setEnabled(true);
            } else {
                prefSet.removePreference(operatorSelectionExpand);
            }
        }
        if (carrierSettings != null) {
            prefSet.removePreference(carrierSettings);
        }
    }

    private void controlCdmaOptions(boolean enable) {
        PreferenceScreen prefSet = getPreferenceScreen();
        if (prefSet == null) {
            return;
        }
        if (enable && mCdmaOptions == null) {
            mCdmaOptions = new CdmaOptions(this, prefSet, mPhone);
        }
        CdmaSystemSelectListPreference systemSelect =
                (CdmaSystemSelectListPreference)prefSet.findPreference
                        (BUTTON_CDMA_SYSTEM_SELECT_KEY);
        if (systemSelect != null) {
            systemSelect.setEnabled(enable);
        }
    }

    private boolean isSupportTdscdma() {
        final int subId = mPhone.getSubId();
        if (SubscriptionManager.getResourcesForSubId(this, subId)
                .getBoolean(R.bool.config_support_tdscdma)) {
            log("Supports Tdscdma, subId " + subId);
            return true;
        }

        String operatorNumeric = mPhone.getServiceState().getOperatorNumeric();
        String[] numericArray =
                SubscriptionManager.getResourcesForSubId(this, subId).getStringArray(
                R.array.config_support_tdscdma_roaming_on_networks);
        log("isSupportTdscdma, operatorNumeric " + operatorNumeric + " subId " + subId);
        if (numericArray.length == 0 || operatorNumeric == null) {
            return false;
        }
        for (String numeric : numericArray) {
            if (operatorNumeric.equals(numeric)) {
                return true;
            }
        }
        return false;
    }
}
