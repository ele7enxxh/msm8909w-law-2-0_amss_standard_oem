/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qti.primarycardcontroller;

import java.util.ArrayList;
import java.util.List;

import android.content.Context;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.telephony.Rlog;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;

public class PrefNetworkRequest extends SyncQueue.SyncRequest {
    private static final String TAG = "PrefNetworkRequest";

    private static final SyncQueue sSyncQueue = new SyncQueue();

    private final Message mCallback;
    protected final List<PrefNetworkSetCommand> mCommands;
    private final Context mContext;
    private SubscriptionManager mSubscriptionManager;
    protected Phone[] mPhones = PhoneFactory.getPhones();

    private static final int EVENT_SET_PREF_NETWORK_DONE = 1;
    private static final int EVENT_GET_PREF_NETWORK_DONE = 2;
    private static final int EVENT_START_REQUEST = 3;

    private Handler mHandler = new Handler(Looper.getMainLooper()) {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case EVENT_SET_PREF_NETWORK_DONE:
                    handleSetPreferredNetwork(msg);
                    break;
                case EVENT_GET_PREF_NETWORK_DONE:
                    handleGetPreferredNetwork(msg);
                    break;
                case EVENT_START_REQUEST:
                    request((Integer) msg.obj);
                    break;
            }
        }
    };

    protected class PrefNetworkSetCommand {
        private final int mSlot;
        private final int mPrefNetwork;

        public PrefNetworkSetCommand(int slot, int prefNetwork) {
            mSlot = slot;
            mPrefNetwork = prefNetwork;
        }
    }

    private void request(final int index) {
        final PrefNetworkSetCommand command = mCommands.get(index);
        logd("save network mode " + command.mPrefNetwork + " for slot" + command.mSlot
                + " to DB first");
        savePrefNetworkInDb(command.mSlot, command.mPrefNetwork);

        logd("set " + command.mPrefNetwork + " for slot" + command.mSlot);
        mPhones[command.mSlot].setPreferredNetworkType(
                command.mPrefNetwork,
                mHandler.obtainMessage(EVENT_SET_PREF_NETWORK_DONE, index));
    }

    private void handleGetPreferredNetwork(Message msg) {
        int modemNetworkMode = -1;
        AsyncResult ar = (AsyncResult) msg.obj;
        int index = (Integer) ar.userObj;
        PrefNetworkSetCommand command = mCommands.get(index);
        if (ar.exception == null) {
            modemNetworkMode = ((int[]) ar.result)[0];
            savePrefNetworkInDb(command.mSlot, modemNetworkMode);
        }
        logd(" get perferred N/W mode = [" + command.mSlot + "] = "
                + modemNetworkMode + " done, " + ar.exception);
        if (++index < mCommands.size()) {
            request(index);
        } else {
            response(mCallback);
            end();
        }
    }

    private void handleSetPreferredNetwork(Message msg) {
        AsyncResult ar = (AsyncResult) msg.obj;
        int index = (Integer) ar.userObj;
        PrefNetworkSetCommand command = mCommands.get(index);
        logd("set " + command.mPrefNetwork + " for slot" + command.mSlot + " done, "
                + ar.exception);
        if (ar.exception != null) {
            mPhones[command.mSlot].getPreferredNetworkType(
                    mHandler.obtainMessage(EVENT_GET_PREF_NETWORK_DONE, index));
            return;
        }
        if (++index < mCommands.size()) {
            request(index);
        } else {
            response(mCallback);
            end();
        }
    }

    public PrefNetworkRequest(Context context, int slot, int networkMode, Message callback) {
        super(sSyncQueue);
        mContext = context;
        mCallback = callback;
        mCommands = new ArrayList<PrefNetworkSetCommand>();
        mSubscriptionManager = SubscriptionManager.from(mContext);
        prepare(slot, networkMode);
    }

    protected void prepare(int slot, int nwMode) {
        // This request comes here to set N/W mode on primary card(i.e CMCC).
        // Send  GSM N/W mode on all other phones so that framework can initiate
        // flex map to link CMCC to primary stack.
        if (nwMode != Phone.NT_MODE_GSM_ONLY) {
            for (int index = 0; index < PrimarySubSelectionController.PHONE_COUNT; index++) {
                PrimarySubSelectionController subController =
                         PrimarySubSelectionController.getInstance();
                if ((subController != null) && (!subController.isCtCardPresent())) {
                    if (index != slot) {
                        mCommands.add(new PrefNetworkSetCommand(index, Phone.NT_MODE_GSM_ONLY));
                    }
                }
            }
        }
        if (slot >= 0 && slot < PrimarySubSelectionController.PHONE_COUNT) {
            mCommands.add(new PrefNetworkSetCommand(slot, nwMode));
        }
    }

    protected void start() {
        if (mCommands.isEmpty()) {
            logd("no command sent");
            response(mCallback);
            end();
        } else {
            PrefNetworkSetCommand command = mCommands.get(mCommands.size() - 1);
            logd("try to set network=" + command.mPrefNetwork + " on slot" + command.mSlot);
            mHandler.obtainMessage(EVENT_START_REQUEST, 0).sendToTarget();
        }
    }

    protected void savePrefNetworkInDb(int slotId, int networkMode) {
        int[] subId = SubscriptionManager.getSubId(slotId);

        android.provider.Settings.Global.putInt(mContext.getContentResolver(),
                android.provider.Settings.Global.PREFERRED_NETWORK_MODE + subId[0], networkMode);
        TelephonyManager.putIntAtIndex(mContext.getContentResolver(),
                android.provider.Settings.Global.PREFERRED_NETWORK_MODE, slotId, networkMode);
    }

    private void response(Message callback) {
        if (callback == null) {
            return;
        }
        if (callback.getTarget() != null) {
            callback.sendToTarget();
        } else {
            Log.w(TAG, "can't response the result, replyTo and target are all null!");
        }
    }


    protected int getNetworkModeFromDb(int slotId) {
        int networkMode = Phone.PREFERRED_NT_MODE;
        int[] subId = mSubscriptionManager.getSubId(slotId);

        if ((subId != null) && (subId.length > 0) &&
                mSubscriptionManager.isActiveSubId(subId[0])) {
            networkMode = android.provider.Settings.Global.getInt(mContext.getContentResolver(),
                    android.provider.Settings.Global.PREFERRED_NETWORK_MODE + subId[0],
                    Phone.PREFERRED_NT_MODE);
        } else {
            try {
                networkMode = TelephonyManager.getIntAtIndex(mContext.getContentResolver(),
                        android.provider.Settings.Global.PREFERRED_NETWORK_MODE, slotId);
            } catch (SettingNotFoundException snfe) {
                networkMode = Phone.PREFERRED_NT_MODE;
            }
        }
        return networkMode;
    }

    private void logd(String msg) {
        Rlog.d(TAG , msg);
    }
}
