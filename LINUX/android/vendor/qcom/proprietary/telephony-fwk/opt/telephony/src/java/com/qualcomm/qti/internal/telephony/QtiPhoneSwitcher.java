/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony;

import static android.telephony.SubscriptionManager.INVALID_PHONE_INDEX;
import static android.telephony.SubscriptionManager.INVALID_SUBSCRIPTION_ID;
import static com.android.internal.telephony.DctConstants.APN_IMS_ID;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.NetworkCapabilities;
import android.net.NetworkFactory;
import android.net.NetworkRequest;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.Registrant;
import android.os.RegistrantList;
import android.telephony.Rlog;
import android.telephony.SubscriptionManager;
import android.telephony.ServiceState;
import android.text.TextUtils;
import android.util.LocalLog;
import android.util.Pair;


import com.android.internal.telephony.CallManager;
import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.dataconnection.DcRequest;
import com.android.internal.telephony.ITelephonyRegistry;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneSwitcher;
import com.android.internal.telephony.SubscriptionController;

import com.qualcomm.qti.internal.telephony.QtiRilInterface;
import com.qualcomm.qti.internal.telephony.QtiUiccCardProvisioner;
import com.qualcomm.qcrilhook.QcRilHook;

import java.lang.Integer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;


public class QtiPhoneSwitcher extends PhoneSwitcher {

    private final int MAX_CONNECT_FAILURE_COUNT = 5;
    private final int ALLOW_DATA_RETRY_DELAY = 30 * 1000;
    private int[] mAllowDataFailure;
    private int mIwlanPhoneId = -1;
    private CallManager mCm;
    private boolean mManualDdsSwitch = false;
    private QtiRilInterface mQtiRilInterface;

    public QtiPhoneSwitcher(int maxActivePhones, int numPhones, Context context,
            SubscriptionController subscriptionController, Looper looper, ITelephonyRegistry tr,
            CommandsInterface[] cis, Phone[] phones) {
        super (maxActivePhones, numPhones, context, subscriptionController, looper,
                tr, cis, phones);
        mAllowDataFailure = new int[numPhones];
        mCm = CallManager.getInstance();
        mCm.registerForDisconnect(this, EVENT_VOICE_CALL_ENDED, null);
        mQtiRilInterface = QtiRilInterface.getInstance(context);
        if (mQtiRilInterface.isServiceReady()) {
            queryMaxDataAllowed();
        } else {
            mQtiRilInterface.registerForServiceReadyEvent(this, EVENT_OEM_HOOK_SERVICE_READY, null);
        }
        mQtiRilInterface.registerForUnsol(this, EVENT_UNSOL_MAX_DATA_ALLOWED_CHANGED, null);
        for (int i = 0; i < numPhones; i++) {
            if (mPhones[i] != null) {
                mPhones[i].getServiceStateTracker().registerForDataRegStateOrRatChanged(this,
                        EVENT_DATA_RAT_CHANGED, i);
            }
        }
    }

    private void queryMaxDataAllowed() {
        mMaxActivePhones = mQtiRilInterface.getMaxDataAllowed();
    }

    private void handleUnsolMaxDataAllowedChange(Message msg) {
        if (msg == null ||  msg.obj == null) {
            log("Null data received in handleUnsolMaxDataAllowedChange");
            return;
        }
        ByteBuffer payload = ByteBuffer.wrap((byte[])msg.obj);
        payload.order(ByteOrder.nativeOrder());
        int rspId = payload.getInt();
        if ((rspId == QcRilHook.QCRILHOOK_UNSOL_MAX_DATA_ALLOWED_CHANGED)) {
            int response_size = payload.getInt();
            if (response_size < 0 ) {
                log("Response size is Invalid " + response_size);
                return;
            }
            mMaxActivePhones = payload.get();
            log(" Unsol Max Data Changed to: " + mMaxActivePhones);
        }
    }

    @Override
    public void handleMessage(Message msg) {
        final int ddsSubId = mSubscriptionController.getDefaultDataSubId();
        log("handle event - " + msg.what);
        AsyncResult ar = null;
        switch (msg.what) {
            case EVENT_DEFAULT_SUBSCRIPTION_CHANGED:
                if (ddsSubId != mDefaultDataSubscription) {
                    mManualDdsSwitch = true;
                    informDdsToRil(ddsSubId);
                }
                //Block dds switch request as it would fail in case of active call
                if (isAnyVoiceCallActiveOnDevice()) {
                    log("Voice call active. Waiting for call end");
                    return;
                }
                onEvaluate(REQUESTS_UNCHANGED, "defaultChanged");
                break;
            case EVENT_OEM_HOOK_SERVICE_READY:
                ar = (AsyncResult)msg.obj;
                if (ar.result != null) {
                    boolean isServiceReady = (boolean)ar.result;
                    if (isServiceReady) {
                        queryMaxDataAllowed();
                    }
                } else {
                    log("Error: empty result, EVENT_OEM_HOOK_SERVICE_READY");
                }
                break;
            case EVENT_UNSOL_MAX_DATA_ALLOWED_CHANGED:
                ar = (AsyncResult)msg.obj;
                if (ar.result != null) {
                    handleUnsolMaxDataAllowedChange((Message)ar.result);
                } else {
                    log("Error: empty result, EVENT_UNSOL_MAX_DATA_ALLOWED_CHANGED");
                }
                break;
            case EVENT_REQUEST_NETWORK:
                DcRequest dcRequest = new DcRequest(((NetworkRequest)msg.obj), mContext);
                if (mPrioritizedDcRequests.contains(dcRequest) == false) {
                    int phoneIdForRequest = phoneIdForRequest(dcRequest.networkRequest);
                    int subIdForRequest = mSubscriptionController.
                            getSubIdUsingPhoneId(phoneIdForRequest);
                    //Send dds info for request received on non dds sub
                    if (ddsSubId != subIdForRequest) {
                        informDdsToRil(ddsSubId);
                    }
                }
                super.handleMessage(msg);
                break;
            case EVENT_ALLOW_DATA_RESPONSE:
                onAllowDataResponse(msg.arg1, (AsyncResult)msg.obj);
                break;
            case EVENT_SUBSCRIPTION_CHANGED:
                if (ddsSubId != mDefaultDataSubscription) {
                    //dds switch must be done with sim removal
                    informDdsToRil(ddsSubId);
                }

                broadcastNetworkSpecifier();
                onEvaluate(REQUESTS_UNCHANGED, "subChanged");
                break;
            case EVENT_DATA_RAT_CHANGED:
                ar = (AsyncResult)msg.obj;
                Pair<Integer, Integer> drsRatPair = (Pair<Integer, Integer>)ar.result;
                int rilRat = drsRatPair.second;
                log("RIL RAT :"+rilRat);
                if (rilRat == ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN) {
                    mIwlanPhoneId = (Integer)ar.userObj;
                    activateIwlanSub();
                }
                break;
            case EVENT_VOICE_CALL_ENDED:
                log("EVENT_VOICE_CALL_ENDED");
                int ddsPhoneId = mSubscriptionController.getPhoneId(ddsSubId);
                if (SubscriptionManager.isValidPhoneId(ddsPhoneId) &&
                        !isAnyVoiceCallActiveOnDevice()) {
                    if (getConnectFailureCount(ddsPhoneId) > 0) {
                        resendDataAllowed(ddsPhoneId);
                    }
                }
                break;
            default:
                super.handleMessage(msg);
        }
    }

    /* Activate the IWLAN subscription so that TelePhonyNetworkFactory proceeds with
     * enabling the apn
     */
    private void activateIwlanSub() {
        PhoneState state = mPhoneStates[mIwlanPhoneId];
        if (state.active == true) return;
        state.active = true;
        log("activate IWLAN phone id : " + mIwlanPhoneId);
        mActivePhoneRegistrants[mIwlanPhoneId].notifyRegistrants();
    }

    /* Broadcast the subscription id to connectivityService which in turn
     * adds the relevant sub id to the network request
     */
    private void broadcastNetworkSpecifier() {
        ArrayList<Integer> subIdList = new ArrayList<Integer>();
        for (int i = 0; i < mNumPhones; ++i) {
            int[] subId = mSubscriptionController.getSubId(i);
            if (subId != null && subId.length > 0 &&
                    mSubscriptionController.isActiveSubId(subId[0]) &&
                    isUiccProvisioned(i)) {
                subIdList.add(subId[0]);
            }
        }

        if (subIdList.size() > 0) {
             Intent intent = new Intent(
                     "org.codeaurora.intent.action.ACTION_NETWORK_SPECIFIER_SET");
             intent.putIntegerArrayListExtra("SubIdList", subIdList);
             log("Broadcast network specifier set intent");
             mContext.sendBroadcast(intent);
        }
    }

    @Override
    protected void onEvaluate(boolean requestsChanged, String reason) {
        StringBuilder sb = new StringBuilder(reason);
        if (isEmergency()) {
            log("onEvalute aborted due to Emergency");
            return;
        }

        boolean diffDetected = requestsChanged;
        final int dataSub = mSubscriptionController.getDefaultDataSubId();
        if (dataSub != mDefaultDataSubscription) {
            sb.append(" default ").append(mDefaultDataSubscription).append("->").append(dataSub);
            mDefaultDataSubscription = dataSub;
            diffDetected = true;
        }

        for (int i = 0; i < mNumPhones; i++) {
            int sub = mSubscriptionController.getSubIdUsingPhoneId(i);
            if (sub != mPhoneSubscriptions[i]) {
                sb.append(" phone[").append(i).append("] ").append(mPhoneSubscriptions[i]);
                sb.append("->").append(sub);
                mPhoneSubscriptions[i] = sub;
                diffDetected = true;
            }
        }

        if (diffDetected) {
            log("evaluating due to " + sb.toString());

            List<Integer> newActivePhones = new ArrayList<Integer>();

            for (DcRequest dcRequest : mPrioritizedDcRequests) {
                int phoneIdForRequest = phoneIdForRequest(dcRequest.networkRequest,
                        dcRequest.apnId);
                if (phoneIdForRequest == INVALID_PHONE_INDEX) continue;
                if (newActivePhones.contains(phoneIdForRequest)) continue;
                newActivePhones.add(phoneIdForRequest);
                if (newActivePhones.size() >= mMaxActivePhones) break;
            }

            if (VDBG) {
                log("default subId = " + mDefaultDataSubscription);
                for (int i = 0; i < mNumPhones; i++) {
                    log(" phone[" + i + "] using sub[" + mPhoneSubscriptions[i] + "]");
                }
                log(" newActivePhones:");
                for (Integer i : newActivePhones) log("  " + i);
            }

            for (int phoneId = 0; (phoneId < mNumPhones) && (mMaxActivePhones != 2); phoneId++) {
                if (newActivePhones.contains(phoneId) == false) {
                    deactivate(phoneId);
                }
            }

            // only activate phones up to the limit
            for (int phoneId : newActivePhones) {
                activate(phoneId);
            }
        }
        mManualDdsSwitch = false;
    }

    /* Determine the phone id on which PS attach needs to be done
     */
    protected int phoneIdForRequest(NetworkRequest netRequest, int apnid) {
        String specifier = netRequest.networkCapabilities.getNetworkSpecifier();
        int subId;

        if (TextUtils.isEmpty(specifier)) {
            subId = mDefaultDataSubscription;
        } else {
            if ((APN_IMS_ID == apnid) && mManualDdsSwitch) {
                subId = mDefaultDataSubscription;
            } else {
                subId = Integer.parseInt(specifier);
            }
        }
        int phoneId = INVALID_PHONE_INDEX;
        if (subId == INVALID_SUBSCRIPTION_ID) return phoneId;

        for (int i = 0 ; i < mNumPhones; i++) {
            if (mPhoneSubscriptions[i] == subId) {
                phoneId = i;
                break;
            }
        }
        return phoneId;
    }

    private boolean isUiccProvisioned(int phoneId) {
        QtiUiccCardProvisioner uiccProvisioner = QtiUiccCardProvisioner.getInstance();
        boolean status = (uiccProvisioner.getCurrentUiccCardProvisioningStatus(phoneId) > 0)?
                true : false;
        log("isUiccProvisioned = " + status);

        return status;
    }

    @Override
    protected void activate(int phoneId) {
        PhoneState state = mPhoneStates[phoneId];
        if ((state.active == true) && !mManualDdsSwitch) return;
        state.active = true;
        log("activate " + phoneId);
        state.lastRequested = System.currentTimeMillis();
        mCommandsInterfaces[phoneId].setDataAllowed(true,
                obtainMessage(EVENT_ALLOW_DATA_RESPONSE, phoneId, 0));
    }

    @Override
    protected void onResendDataAllowed(Message msg) {
        final int phoneId = msg.arg1;
        mCommandsInterfaces[phoneId].setDataAllowed(mPhoneStates[phoneId].active,
                obtainMessage(EVENT_ALLOW_DATA_RESPONSE, phoneId, 0));
    }

    private void resetConnectFailureCount(int phoneId) {
        mAllowDataFailure[phoneId] = 0;
    }

    private void incConnectFailureCount(int phoneId) {
        mAllowDataFailure[phoneId]++;
    }

    private int getConnectFailureCount(int phoneId) {
        return mAllowDataFailure[phoneId];
    }

    private void handleConnectMaxFailure(int phoneId) {
        resetConnectFailureCount(phoneId);
        int ddsSubId = mSubscriptionController.getDefaultDataSubId();
        int ddsPhoneId = mSubscriptionController.getPhoneId(ddsSubId);
        if ((ddsPhoneId > 0 && ddsPhoneId < mNumPhones) &&
                phoneId != ddsPhoneId) {
            log("ALLOW_DATA retries exhausted on phoneId = " + phoneId);
            enforceDds(ddsPhoneId);
        }
    }

    private void enforceDds(int phoneId) {
        int[] subId = mSubscriptionController.getSubId(phoneId);
        log("enforceDds: subId = " + subId[0]);
        mSubscriptionController.setDefaultDataSubId(subId[0]);
    }

    private boolean isAnyVoiceCallActiveOnDevice() {
        boolean ret = (mCm.getState() != PhoneConstants.State.IDLE);
        log("isAnyVoiceCallActiveOnDevice: " + ret);
        return ret;
    }

    private void onAllowDataResponse(int phoneId, AsyncResult ar) {
        if (ar.exception != null) {
            incConnectFailureCount(phoneId);
            log("Allow_data failed on phoneId = " + phoneId + ", failureCount = "
                    + getConnectFailureCount(phoneId));

            if (isAnyVoiceCallActiveOnDevice()) {
                /* Any DDS retry while voice call is active is in vain
                   Wait for call to get disconnected */
                log("Wait for call end indication");
                return;
            }

            if (getConnectFailureCount(phoneId) == MAX_CONNECT_FAILURE_COUNT) {
                handleConnectMaxFailure(phoneId);
            } else {
                log("Scheduling retry connect/allow_data");
                postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        int ddsSubId = mSubscriptionController.getDefaultDataSubId();
                        int ddsPhoneId = mSubscriptionController.getPhoneId(ddsSubId);
                        if (ddsPhoneId == phoneId) {
                            log("Running retry connect/allow_data");
                            resendDataAllowed(phoneId);
                        } else {
                            log("Dds sub changed");
                            resetConnectFailureCount(phoneId);
                        }
                    }}, ALLOW_DATA_RETRY_DELAY);
                }
        } else {
            log("Allow_data success on phoneId = " + phoneId);
            resetConnectFailureCount(phoneId);
            mActivePhoneRegistrants[phoneId].notifyRegistrants();
        }
    }

    private void informDdsToRil(int ddsSubId) {
        int ddsPhoneId = mSubscriptionController.getPhoneId(ddsSubId);

        QtiRilInterface qtiRilInterface = QtiRilInterface.getInstance(mContext);
        if (!qtiRilInterface.isServiceReady()) {
            log("Oem hook service is not ready yet");
            return;
        }

        for (int i = 0; i < mNumPhones; i++) {
            log("InformDdsToRil rild= " + i + ", DDS=" + ddsPhoneId);
            qtiRilInterface.qcRilSendDDSInfo(ddsPhoneId, i);
        }
    }

}
