/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qti.qualcomm.datastatusnotification;

import java.lang.Exception;
import java.util.Arrays;
import java.util.List;
import java.util.HashMap;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.ContentObserver;
import android.database.Cursor;
import android.net.Uri;
import android.os.IBinder;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.provider.Telephony;
import android.telephony.TelephonyManager;
import android.telephony.SubscriptionManager;
import android.text.TextUtils;
import android.util.Log;
import com.android.internal.telephony.IccCardConstants;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.TelephonyProperties;
import com.qualcomm.qcrilhook.IQcRilHook;
import com.qualcomm.qcrilhook.QcRilHook;
import com.qualcomm.qcrilhook.QcRilHookCallback;

public class DataStatusNotificationService extends Service {
    private static final String TAG = "QcDataStatusNotification Service";
    private static final boolean DBG = true;
    private static final String MOBILE_DATA = Settings.Global.MOBILE_DATA;
    private static final String DATA_ROAMING = Settings.Global.DATA_ROAMING;
    private static final String CARRIERS = "carriers";
    private static final Uri CARRIERS_URI = Telephony.Carriers.CONTENT_URI;
    private QcRilHook mQcRilHook;
    private int mPhoneCount = TelephonyManager.getDefault().getPhoneCount();
    private DataSettingsObserver mDataSettingsObserver;
    // Array to store default and ims apns for each phone
    private String[] mDefaultApns;
    private String[] mImsApns;
    private HashMap<Integer, Boolean> mSubEnabledMap;
    private ContentResolver mResolver;
    boolean mIsQcRilHookReady = false;

    private class Apn {
        private String mType;
        private String mApn;

        public Apn(String type, String apn) {
            mType = type;
            mApn = apn;
        }

        public String getType() {
            return mType;
        }

        public String getApn() {
            return mApn;
        }

        public String toString() {
            StringBuilder sb = new StringBuilder();
            sb.append(mType);
            sb.append(" ");
            sb.append(mApn);
            return sb.toString();
        }
    }

    private QcRilHookCallback mQcRilHookCb = new QcRilHookCallback() {
        public void onQcRilHookReady() {
            mIsQcRilHookReady = true;
            Log.d(TAG, "mPhoneCount = " + mPhoneCount);
            /*
             * Store default apns for each phoneId as TelephonyProvider does not call notifyChange
             * for APN changes specific to a phoneId. It calls notifyChange() for any change in
             * carriers DB
             */
            mDefaultApns = new String[mPhoneCount];
            mImsApns = new String[mPhoneCount];
            mResolver = DataStatusNotificationService.this.getContentResolver();
            mDataSettingsObserver = new DataSettingsObserver();
            mSubEnabledMap = new HashMap<Integer, Boolean>();
            // Register content observer for carriers DB for changes in APN
            mResolver.registerContentObserver(CARRIERS_URI, false, mDataSettingsObserver);

            for (int i = 0; i < mPhoneCount; i++) {
                int[] subIds = SubscriptionManager.getSubId(i);
                if (subIds != null) {
                    if (SubscriptionManager.isUsableSubIdValue(subIds[0])) {
                        Uri dataUri = formUri(MOBILE_DATA, subIds[0]);
                        Uri roamingUri = formUri(DATA_ROAMING, subIds[0]);
                        if (DBG)
                            Log.d(TAG, "enableContentObservers for subId = " + subIds[0]);
                        enableContentObservers(dataUri, roamingUri);
                        // Explicitly trigger onChange() at start of service
                        // since modem needs to know the values at start of service
                        triggerOnChange(dataUri, roamingUri);
                        mSubEnabledMap.put(subIds[0], true);
                    } else {
                        Log.d(TAG, "Subscription id is not usable: " + subIds[0]);
                    }
                } else {
                    Log.d(TAG, "getSubId returned null array");
                }
            }
            registerSimStateReceiver();
        }

        public void onQcRilHookDisconnected() {
            mIsQcRilHookReady = false;
        }
    };

    /* Register sim state receiver for loaded event */
    private void registerSimStateReceiver() {
        Context context = getApplicationContext();
        context.registerReceiver(mSimStateReceiver, new IntentFilter(
                TelephonyIntents.ACTION_SIM_STATE_CHANGED));
    }

    private BroadcastReceiver mSimStateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent)
        {
            String action = intent.getAction();
            if (action.equals(TelephonyIntents.ACTION_SIM_STATE_CHANGED)) {
                String stateExtra = intent.getStringExtra(IccCardConstants.INTENT_KEY_ICC_STATE);
                int subId = intent.getExtras().getInt(PhoneConstants.SUBSCRIPTION_KEY);
                if (DBG)
                    Log.d(TAG, "onReceive subId = " + subId);
                if (IccCardConstants.INTENT_VALUE_ICC_LOADED.equals(stateExtra)) {
                    Uri dataUri = formUri(MOBILE_DATA, subId);
                    Uri roamingUri = formUri(DATA_ROAMING, subId);
                    if (!mSubEnabledMap.containsKey(subId) || !mSubEnabledMap.get(subId)) {
                        mSubEnabledMap.put(subId, true);
                        Log.d(TAG, "enableContentObservers subId = " + subId);
                        enableContentObservers(dataUri, roamingUri);
                    }
                    triggerOnChange(dataUri, roamingUri);
                }
            }
        }
    };

    private class DataSettingsObserver extends ContentObserver {
        DataSettingsObserver() {
            super(null);
        }

        @Override
        public void onChange(boolean selfChange, Uri uri) {
            if (!mIsQcRilHookReady) {
                Log.e(TAG, "QcRilhook not read. Bail out");
                return;
            }

            if (uri != null) {
                String authority = uri.getAuthority();
                String uriLastSegment = uri.getLastPathSegment();
                int phoneId = 0;
                int subId = 0;
                if (authority.equals("settings")) { // authority for mobile_data and roaming
                    if (mPhoneCount > 1) {
                        /*
                         * For multi-sim, the URI last segment contains subId in the format
                         * mobile_data0
                         */
                        String[] lastSegmentParts = uriLastSegment.split("\\d");
                        int uriLength = uriLastSegment.length();
                        int keyLength = lastSegmentParts[0].length();
                        subId = Integer.parseInt(uriLastSegment.substring(keyLength, uriLength));
                        phoneId = SubscriptionManager.getPhoneId(subId);
                        uriLastSegment = uriLastSegment.substring(0, keyLength);
                        if (DBG)
                            Log.d(TAG, "MultiSim onChange(): subId = " + subId);
                    }
                }
                if (DBG)
                    Log.d(TAG, "onChange():uri=" + uri.toString() + " authority=" + authority
                            + " path=" + uri.getPath() + " segments=" + uri.getPathSegments()
                            + " uriLastSegment=" + uriLastSegment);
                switch (uriLastSegment) {
                    case MOBILE_DATA:
                        int mobile_data_status = 0;
                        try {
                            if (mPhoneCount > 1) {
                                mobile_data_status = Settings.Global.getInt(
                                        mResolver, MOBILE_DATA + subId);
                            } else {
                                mobile_data_status = Settings.Global.getInt(
                                        mResolver, MOBILE_DATA);
                            }
                        } catch (SettingNotFoundException ex) {
                            Log.e(TAG, ex.getMessage());
                        }
                        if (DBG)
                            Log.d(TAG, "handleMessage: Data Enable changed to "
                                    + mobile_data_status
                                    + " on phoneId = " + phoneId);
                        mQcRilHook.qcRilSendDataEnableStatus(mobile_data_status, phoneId);
                        break;
                    case DATA_ROAMING:
                        int data_roaming_status = 0;
                        try {
                            if (mPhoneCount > 1) {
                                data_roaming_status = Settings.Global.getInt(
                                        mResolver, DATA_ROAMING + subId);
                            } else {
                                data_roaming_status = Settings.Global.getInt(
                                        mResolver, DATA_ROAMING);
                            }
                        } catch (SettingNotFoundException ex) {
                            Log.e(TAG, ex.getMessage());
                        }
                        if (DBG)
                            Log.d(TAG, "handleMessage: Data Roaming changed to "
                                    + data_roaming_status
                                    + " on phoneId = " + phoneId);
                        mQcRilHook.qcRilSendDataRoamingEnableStatus(data_roaming_status, phoneId);
                        break;
                    case CARRIERS:
                        for (int i = 0; i < mPhoneCount; i++) {
                            onApnChanged(i);
                        }
                        break;
                    default:
                        Log.e(TAG, "Received unsupported uri");
                }
            } else
                Log.e(TAG, "Received uri is null");
        }
    }

    private void triggerOnChange(Uri dataUri, Uri roamingUri) {
        mDataSettingsObserver.onChange(false, dataUri);
        mDataSettingsObserver.onChange(false, roamingUri);
        mDataSettingsObserver.onChange(false, CARRIERS_URI);
    }

    private Uri formUri(String uriStr, int subId) {
        Uri result = null;
        if (mPhoneCount > 1) {
            result = Settings.Global.getUriFor(uriStr + subId);
        } else {
            result = Settings.Global.getUriFor(uriStr);
        }
        return result;
    }

    private void enableContentObservers(Uri dataUri, Uri roamingUri) {
        // Data Enable Content Observer
        mResolver.registerContentObserver(dataUri, false, mDataSettingsObserver);

        // Data Roaming Content Observer
        mResolver.registerContentObserver(roamingUri, false, mDataSettingsObserver);
    }

    private void onApnChanged(int phoneId) {
        Apn newApn = null;
        Apn newImsApn = null;
        // Get subIds for a particular phoneId. Only first subId is relevant for now
        int[] subIdList = SubscriptionManager.getSubId(phoneId);
        if (subIdList == null)
            return;
        int subId = subIdList[0];

        // Find operator of the subId where APN has changed
        String operator = TelephonyManager.getDefault().getSimOperatorNumeric(subId);
        if (DBG)
            Log.d(TAG, "onApnChanged: phoneId = " + phoneId + " subId = "
                    + subId + " operator = " + operator);

        // For operator, get cursor from DB
        if (operator != null && !operator.isEmpty()) {
            String selection = "numeric = '" + operator + "'";
            selection += " and carrier_enabled = 1";
            Cursor cursor = mResolver.query(getUri(Telephony.Carriers.CONTENT_URI, subId),
                    new String[] {
                            "apn", "numeric", "type", "carrier_enabled"
                    }, selection, null, null);
            if (cursor != null && cursor.getCount() > 0) {
                // Try to get preferredApn if preferred APN is set, can handle
                // default type and has correct operator
                // else get first APN that can handle default type
                if (DBG)
                    Log.d(TAG, "APN change URI is  "
                            + getUri(Telephony.Carriers.CONTENT_URI, subId).toString()
                            + " count = " + cursor.getCount());
                if ((newApn = getPreferredApn(subId, selection)) != null) {
                    if (DBG)
                        Log.d(TAG,
                                "Found preferred APN: " + newApn.toString());
                } else if ((newApn = getDefaultApn(cursor)) != null) {
                    if (DBG)
                        Log.d(TAG, "Found default APN: " + newApn.toString());
                } else {
                    Log.d(TAG,
                            "Could not find preferred or default APN");
                }
                // Call oemhook with newApn if it is not same as previously stored apn
                if (newApn != null && (!newApn.getApn().equals(mDefaultApns[phoneId]))) {
                    if (DBG)
                        Log.d(TAG, "Found new apn.Call oemhook on phoneId " + phoneId);
                    mDefaultApns[phoneId] = newApn.getApn();
                    mQcRilHook.qcRilSendApnInfo(newApn.getType(), newApn.getApn(), 1 /* enable */,
                            phoneId);
                }

                cursor.moveToFirst();
                newImsApn = getImsApn(cursor);
                if (newImsApn != null && (!newImsApn.getApn().equals(mImsApns[phoneId]))) {
                    if (DBG)
                        Log.d(TAG, "Found new IMS apn.Call oemhook on phoneId " + phoneId);
                    mImsApns[phoneId] = newImsApn.getApn();
                    mQcRilHook.qcRilSendApnInfo(newImsApn.getType(), newImsApn.getApn(),
                            1 /* enable */, phoneId);
                }

                cursor.close();
            } else {
                Log.d(TAG,
                        "No rows in Carriers DB for current operator");
            }
        } else {
            Log.d(TAG, "Could not get current operator");
        }

        // The case where previous apn that was enabled is now disabled
        if (mDefaultApns[phoneId] != null && newApn == null) {
            if (DBG)
                Log.d(TAG, "Previous default apn has been removed.Call oemhook on phoneId "
                        + phoneId);
            mQcRilHook.qcRilSendApnInfo(PhoneConstants.APN_TYPE_DEFAULT, mDefaultApns[phoneId],
                    0 /* disable */, phoneId);
            mDefaultApns[phoneId] = null;
        }

        // The case where previous IMS apn that was enabled is now disabled
        if (mImsApns[phoneId] != null && newImsApn == null) {
            if (DBG)
                Log.d(TAG, "Previous IMS apn has been removed.Call oemhook on phoneId "
                        + phoneId);
            mQcRilHook.qcRilSendApnInfo(PhoneConstants.APN_TYPE_IMS, mImsApns[phoneId],
                    0 /* disable */, phoneId);
            mImsApns[phoneId] = null;
        }
    }

    private Apn getApnFound(Cursor cursor, String type) {
        Apn apn = null;
        String[] typesSupported = parseTypes(cursor.getString(cursor
                .getColumnIndexOrThrow(Telephony.Carriers.TYPE)));
        if (DBG)
            Log.d(TAG, "getApnFound: typesSupported = "
                    + Arrays.toString(typesSupported) + " type requested=" + type);
        for (String t : typesSupported) {
            if (t.equalsIgnoreCase(PhoneConstants.APN_TYPE_ALL)
                    || t.equalsIgnoreCase(type)) {
                apn = new Apn(
                        type,
                        cursor.getString(cursor
                                .getColumnIndexOrThrow(Telephony.Carriers.APN)));
                if (DBG)
                    Log.d(TAG, "getApnFound: Apn = " + apn.toString());
                break;
            }
        }
        return apn;
    }

    private Apn getPreferredApn(int subId, String selection) {
        Uri PREFERAPN_URI = Uri.parse("content://telephony/carriers/preferapn");
        Apn preferredApn = null;
        Cursor cursor = mResolver.query(
                getUri(PREFERAPN_URI, subId), new String[] {
                        "apn", "numeric", "type", "carrier_enabled"
                }, selection, null, null);
        if (cursor != null && cursor.getCount() > 0) {
            cursor.moveToFirst();
            preferredApn = getApnFound(cursor, PhoneConstants.APN_TYPE_DEFAULT);
            cursor.close();
        }
        return preferredApn;
    }

    private Apn getDefaultApn(Cursor cursor) {
        Apn defaultApn = null;
        while (defaultApn == null && cursor.moveToNext()) {
            defaultApn = getApnFound(cursor, PhoneConstants.APN_TYPE_DEFAULT);
        }
        return defaultApn;
    }

    private Apn getImsApn(Cursor cursor) {
        Apn imsApn = null;
        int count = cursor.getCount();
        if (DBG)
            Log.d(TAG, "getImsApnCount = " + count);
        int i = 0;
        while (imsApn == null && i < count) {
            imsApn = getApnFound(cursor, PhoneConstants.APN_TYPE_IMS);
            cursor.moveToNext();
            i++;
        }
        return imsApn;
    }

    private String[] parseTypes(String types) {
        String[] result;
        // If unset, set to DEFAULT.
        if (TextUtils.isEmpty(types)) {
            result = new String[1];
            result[0] = PhoneConstants.APN_TYPE_ALL;
        } else {
            result = types.split(",");
        }
        return result;
    }

    private Uri getUri(Uri uri, int subId) {
        return Uri.withAppendedPath(uri, "subId/" + subId);
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if (DBG)
            Log.d(TAG, "onStartCommand");
        if (mQcRilHook == null) {
            mQcRilHook = new QcRilHook(this, mQcRilHookCb);
        }
        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        if (DBG)
            Log.d(TAG, "onDestroy");
        mResolver.unregisterContentObserver(mDataSettingsObserver);
        unregisterReceiver(mSimStateReceiver);
        mSubEnabledMap.clear();
    }
}
