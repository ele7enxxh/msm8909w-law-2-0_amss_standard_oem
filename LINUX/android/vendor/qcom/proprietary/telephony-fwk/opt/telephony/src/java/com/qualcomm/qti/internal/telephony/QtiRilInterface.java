/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.internal.telephony;


import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.os.Registrant;
import android.os.RegistrantList;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.telephony.Rlog;
import android.util.Log;

import com.qualcomm.qti.internal.telephony.dataconnection.QtiApnProfileOmh;
import com.qualcomm.qti.internal.telephony.dataconnection.QtiApnSetting;
import com.qualcomm.qti.internal.telephony.QtiUiccCardProvisioner.UiccProvisionStatus;

import com.qualcomm.qcrilhook.OemHookCallback;
import com.qualcomm.qcrilhook.QcRilHook;
import com.qualcomm.qcrilhook.QcRilHookCallback;

import java.util.ArrayList;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import org.codeaurora.internal.IDepersoResCallback;

/**
 *  Its an interface to OemHook library from Telephony framework.
 *  This class holds the library functions to frame data as expected
 *  by OEM HOOK framework and calls QcRilHook methods to communicate
 *  with RIL over qcrilmsgtunnel socket.
 */
public class QtiRilInterface {
    private final static String LOG_TAG = "QtiRilInterface";

    private final static int INT_SIZE = 4;
    private final static int BYTE_SIZE = 1;

    private static final char NULL_TERMINATOR = '\0';

    private static final int NULL_TERMINATOR_LENGTH = BYTE_SIZE;

    private static QtiRilInterface sInstance = null;
    private QcRilHook mQcRilHook;
    private RegistrantList mServiceReadyRegistrantList;

    private static boolean mIsServiceReady = false;
    private String OMH_FAKE_QCRIL_HOOK_RESPONSE = "persist.test.omh.fakeprofile";

    /**
     * Called when connection to QcrilMsgTunnelService has been established.
     */
    private QcRilHookCallback mQcrilHookCb = new QcRilHookCallback() {
        public void onQcRilHookReady() {
            mIsServiceReady = true;
            logd("Service ready, notifying registrants");

            AsyncResult ar = new AsyncResult(null, mIsServiceReady, null);
            mServiceReadyRegistrantList.notifyRegistrants(ar);
        }

        public synchronized void onQcRilHookDisconnected() {
            mIsServiceReady = false;
            logd("Service disconnected, notifying registrants");

            AsyncResult ar = new AsyncResult(null, mIsServiceReady, null);
            mServiceReadyRegistrantList.notifyRegistrants(ar);
            sInstance = null;
        }
    };

    public static synchronized QtiRilInterface getInstance(Context context) {
        if (sInstance == null) {
            sInstance = new QtiRilInterface(context);
        } else {
            Log.wtf(LOG_TAG, "instance = " + sInstance);
        }

        return sInstance;
    }

    private QtiRilInterface(Context context) {
        logd(" in constructor ");

        mServiceReadyRegistrantList = new RegistrantList();
        mQcRilHook = new QcRilHook(context, mQcrilHookCb);
    }

    public UiccProvisionStatus getUiccProvisionPreference(int phoneId) {
        UiccProvisionStatus provStatus = new UiccProvisionStatus();
        byte[] requestData = new byte[0];

        AsyncResult ar = mQcRilHook.sendQcRilHookMsg(
                QcRilHook.QCRIL_EVT_HOOK_GET_UICC_PROVISION_PREFERENCE, requestData, phoneId);
        if ((ar.exception == null) && (ar.result != null)) {
            ByteBuffer byteBuf = ByteBuffer.wrap((byte[]) ar.result);
            byteBuf.order(ByteOrder.nativeOrder());

            logd("Data received: " + byteBuf.toString());

            provStatus.setUserPreference(byteBuf.getInt());
            provStatus.setCurrentState(byteBuf.getInt());
        }

        logi("get pref, phoneId " + phoneId + " " + provStatus + " exception " + ar.exception);
        return provStatus;
    }

    private ArrayList<QtiApnSetting> parseOmhProfiles(byte[] buffer) {
        ArrayList<QtiApnSetting> profilesList = new ArrayList<QtiApnSetting>();
        ByteBuffer byteBuf = ByteBuffer.wrap(buffer);
        if(byteBuf != null) {
            byteBuf.order(ByteOrder.nativeOrder());

            logi("Data received: " + byteBuf.toString());

            int nProfiles = byteBuf.getInt();

            int profileId = 0;
            int priority = 0;

            for (int i = 0; i < nProfiles; i++) {
                profileId = byteBuf.getInt();
                priority = byteBuf.getInt();
                QtiApnProfileOmh profile = new QtiApnProfileOmh(profileId, priority);
                logi("getOmhCallProfile " + profileId + ":" + priority);
                profilesList.add(profile);
            }
        }

        return profilesList;

    }

    private ArrayList<QtiApnSetting> getFakeOmhProfiles(int profileId) {

        int[] prioritySortedProfiles = {2, 32, 64, 1}; //MMS, SUPL, DUN, DEFAULT

        ArrayList<QtiApnSetting> profilesList = new ArrayList();
        for(int i = 0; i < prioritySortedProfiles.length; i++) {
            if (prioritySortedProfiles[i] == profileId) {
                QtiApnProfileOmh profile = new QtiApnProfileOmh(prioritySortedProfiles[i], i);
                logi("profile(id=" + profileId + ") =" + profile);
                profilesList.add(profile);
                break;
            }
        }
        return profilesList;
    }

    private class OmhCallProfileCallback extends OemHookCallback {
        Message mAppMessage;
        int mModemApnType;

        public OmhCallProfileCallback(int modemApnType, Message msg) {
            super(msg);
            mAppMessage = msg;
            mModemApnType = modemApnType;
        }

        @Override
        public void onOemHookResponse(byte[] response, int phoneId) throws RemoteException {
            ArrayList<QtiApnSetting> profiles = new ArrayList<QtiApnSetting>();
            boolean fakeResponse = SystemProperties.getBoolean(OMH_FAKE_QCRIL_HOOK_RESPONSE,
                    false);

            if (fakeResponse) {
                logi("Getting fake omh profiles");
                profiles = getFakeOmhProfiles(mModemApnType);
                AsyncResult.forMessage(mAppMessage, profiles, null);
            } else {
                if (response != null) {
                    Log.d(LOG_TAG, "getOmhCallProfile: onOemHookResponse = " + response.toString());
                    profiles = parseOmhProfiles(response);
                    AsyncResult.forMessage(mAppMessage, profiles, null);
                } else {
                    AsyncResult.forMessage(mAppMessage, profiles,
                            new Exception("QCRIL_EVT_HOOK_GET_OMH_CALL_PROFILE failed"));
                }
            }
            mAppMessage.sendToTarget();
        }
    }

    public void getOmhCallProfile(int modemApnType, Message callbackMsg, int phoneId) {
        logi("getOmhCallProfile()");
        byte[] requestData = new byte[INT_SIZE];
        ByteBuffer reqBuffer = mQcRilHook.createBufferWithNativeByteOrder(requestData);

        reqBuffer.putInt(modemApnType);
        OemHookCallback oemHookCb = new OmhCallProfileCallback(modemApnType, callbackMsg);

        mQcRilHook.sendQcRilHookMsgAsync(
                QcRilHook.QCRIL_EVT_HOOK_GET_OMH_CALL_PROFILE,
                requestData,
                oemHookCb,
                phoneId);
    }

    private class DepersoCallback extends OemHookCallback {
        IDepersoResCallback depersoCallBack;
        int SUCCESS = 0;
        int ERROR = 1;

        public DepersoCallback(IDepersoResCallback callback, Message msg) {
            super(msg);
            depersoCallBack = callback;
        }

        @Override
        public void onOemHookResponse(byte[] response, int phoneId) throws RemoteException {
            if (response != null) {
                logd("DepersoResult SUCCESS");
                depersoCallBack.onDepersoResult(SUCCESS, phoneId);
            } else {
                logd("DepersoResult ERROR");
                depersoCallBack.onDepersoResult(ERROR, phoneId);
            }
        }

        @Override
        public void onOemHookException(int phoneId) throws RemoteException  {
            logd("DepersoResult ERROR");
            depersoCallBack.onDepersoResult(ERROR, phoneId);
        }
    }

    public void
    supplyIccDepersonalization(String netpin, String type, IDepersoResCallback callback,
            int phoneId) {
        logd("supplyDepersonalization: netpin = " + netpin + " type = " + type
                + "phoneId = " +phoneId);

        Message msg = Message.obtain();
        byte[] payload = null;
        // type + null character +
        // netpin + null character
        int payloadLength  = type.length() + NULL_TERMINATOR_LENGTH +
                (netpin == null ? NULL_TERMINATOR_LENGTH
                : netpin.length() + NULL_TERMINATOR_LENGTH);

        payload = new byte[payloadLength];
        ByteBuffer buf = mQcRilHook.createBufferWithNativeByteOrder(payload);
        // type
        buf.put(type.getBytes());
        buf.put((byte)NULL_TERMINATOR); // null character
        // pin
        if (netpin != null) buf.put(netpin.getBytes());
        buf.put((byte)NULL_TERMINATOR); // null character

        OemHookCallback oemHookCb = new DepersoCallback(callback, msg);
        mQcRilHook.sendQcRilHookMsgAsync(
                QcRilHook.QCRIL_EVT_HOOK_ENTER_DEPERSONALIZATION_CODE, payload, oemHookCb,
                phoneId);
    }

    public String getUiccIccId(int phoneId) {
        String iccId = null;
        byte[] requestData = new byte[INT_SIZE];
        ByteBuffer reqBuffer = mQcRilHook.createBufferWithNativeByteOrder(requestData);

        reqBuffer.putInt(phoneId);
        AsyncResult ar = mQcRilHook.sendQcRilHookMsg(
                QcRilHook.QCRIL_EVT_HOOK_GET_UICC_ICCID, requestData, phoneId);
        if ((ar.exception == null) && (ar.result != null)) {
            iccId = new String((byte[])ar.result);
        }

        logi("getUiccIccId iccId[" + phoneId + "] = " + iccId + " exception: " + ar.exception);
        return iccId;
    }

    public int getMaxDataAllowed() {
        int maxData = 0;
        AsyncResult ar = mQcRilHook.sendQcRilHookMsg(
                QcRilHook.QCRIL_EVT_HOOK_GET_MAX_DATA_ALLOWED);
        if ((ar.exception == null) && (ar.result != null)) {
            byte[] response = (byte[]) ar.result;
            maxData = ByteBuffer.wrap(response).get();
        }

        logi("getMaxDataAllowed maxData = " + maxData + " exception: " + ar.exception);
        return maxData;
    }

    public void sendPhoneStatus(int isReady, int phoneId) {
        String iccId = null;
        byte[] requestData = new byte[BYTE_SIZE];
        ByteBuffer reqBuffer = mQcRilHook.createBufferWithNativeByteOrder(requestData);

        reqBuffer.put((byte)isReady);
        AsyncResult ar = mQcRilHook.sendQcRilHookMsg(
                QcRilHook.QCRIL_EVT_HOOK_SET_ATEL_UI_STATUS, requestData, phoneId);
    }


    public boolean setUiccProvisionPreference(int userPref, int phoneId) {
        boolean retval = false;
        byte[] requestData = new byte[2*INT_SIZE];
        ByteBuffer reqBuffer = mQcRilHook.createBufferWithNativeByteOrder(requestData);

        reqBuffer.putInt(userPref);
        reqBuffer.putInt(phoneId);

        AsyncResult ar = mQcRilHook.sendQcRilHookMsg(
                QcRilHook.QCRIL_EVT_HOOK_SET_UICC_PROVISION_PREFERENCE, requestData, phoneId);
        if (ar.exception == null) {
            retval = true;
        }

        logi("set provision userPref " + userPref + " phoneId " + phoneId
                + " exception: " + ar.exception);
        return retval;
    }

    public boolean isServiceReady() {
        return mIsServiceReady;
    }

    public void registerForUnsol(Handler handler, int event, Object obj) {
        mQcRilHook.register(handler, event, obj);
    }

    public void unRegisterForUnsol(Handler handler) {
        mQcRilHook.unregister(handler);
    }

    public void registerForServiceReadyEvent(Handler h, int what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        mServiceReadyRegistrantList.add(r);
        if (isServiceReady()) {
            r.notifyRegistrant(new AsyncResult(null, mIsServiceReady, null));
        }
    }

    public void qcRilSendDDSInfo(int ddsPhoneId, int rilId) {
        mQcRilHook.qcRilSendDDSInfo(ddsPhoneId, rilId);
    }

    public void unRegisterForServiceReadyEvent(Handler h) {
        mServiceReadyRegistrantList.remove(h);
    }

    boolean setLocalCallHold(int phoneId, boolean enable) {
        return mQcRilHook.setLocalCallHold(phoneId, enable);
    }

    private void logd(String string) {
        Rlog.d(LOG_TAG, string);
    }

    private void logi(String string) {
        Rlog.i(LOG_TAG, string);
    }

    private void loge(String string) {
        Rlog.e(LOG_TAG, string);
    }
}
