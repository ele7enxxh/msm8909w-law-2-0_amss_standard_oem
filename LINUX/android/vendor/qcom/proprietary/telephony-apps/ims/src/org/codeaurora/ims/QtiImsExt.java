/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package org.codeaurora.ims;

import android.Manifest;
import android.content.Context;
import org.codeaurora.ims.internal.IQtiImsExt;
import org.codeaurora.ims.internal.IQtiImsExtListener;
import org.codeaurora.ims.QtiImsExtBase;

public class QtiImsExt extends QtiImsExtBase {
    private final String MODIFY_PHONE_STATE = Manifest.permission.MODIFY_PHONE_STATE;
    private final String READ_PHONE_STATE = Manifest.permission.READ_PHONE_STATE;

    private Context mContext;
    private ImsServiceSub mServiceSub;

    public QtiImsExt(Context context, ImsServiceSub serviceSub) {
        mContext = context;
        mServiceSub = serviceSub;
    }

    @Override
    protected void onSetCallForwardUncondTimer(int startHour, int startMinute, int endHour,
            int endMinute, int action, int condition, int serviceClass, String number,
            IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "setCallForwardUncondTimer");
        mServiceSub.setCallForwardUncondTimer(startHour, startMinute,
                endHour, endMinute, action, condition, serviceClass, number, listener);
    }

    @Override
    protected void onGetCallForwardUncondTimer(int reason, int serviceClass,
            IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(READ_PHONE_STATE, "getCallForwardUncondTimer");
        mServiceSub.getCallForwardUncondTimer(reason,
                serviceClass, listener);
    }

    @Override
    protected void onGetPacketCount(IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(READ_PHONE_STATE, "getPacketCount");
        mServiceSub.getPacketCount(listener);
    }

    @Override
    protected void onGetPacketErrorCount(IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(READ_PHONE_STATE, "getPacketErrorCount");
        mServiceSub.getPacketErrorCount(listener);
    }

    @Override
    protected void onSendCallDeflectRequest(int phoneId, String deflectNumber,
            IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "sendCallDeflectRequest");
        mServiceSub.sendCallDeflectRequest(deflectNumber, listener);
    }

    @Override
    protected void onResumePendingCall(int videoState) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "resumePendingCall");
        mServiceSub.resumePendingCall(videoState);
    }

    @Override
    protected void onSendCallTransferRequest(int phoneId, int type, String number,
            IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "sendCallTransferRequest");
        mServiceSub.sendCallTransferRequest(type, number, listener);
    }

    @Override
    protected void onQueryVopsStatus(IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(READ_PHONE_STATE, "queryVopsStatus");
        mServiceSub.queryVopsStatus(listener);
    }

    @Override
    protected void onQuerySsacStatus(IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(READ_PHONE_STATE, "querySsacStatus");
        mServiceSub.querySsacStatus(listener);
    }

    @Override
    protected int onGetImsPhoneId() {
        mContext.enforceCallingOrSelfPermission(READ_PHONE_STATE, "getImsPhoneId");
        return mServiceSub.getImsPhoneId();
    }

    @Override
    protected void onRegisterForViceRefreshInfo(IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "registerForViceRefreshInfo");
        mServiceSub.registerForViceRefreshInfo(listener);
    }

    @Override
    protected void onRegisterForParticipantStatusInfo(IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE,
                "registerForParticipantStatusInfo");
        mServiceSub.registerForParticipantStatusInfo(listener);
    }

    @Override
    protected void onUpdateVoltePreference(int phoneId, int preference,
            IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "updateVoltePreference");
        mServiceSub.updateVoltePreference(preference, listener);
    }

    @Override
    protected void onQueryVoltePreference(int phoneId, IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(READ_PHONE_STATE, "queryVoltePreference");
        mServiceSub.queryVoltePreference(listener);
    }

    @Override
    protected void onGetHandoverConfig(IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(READ_PHONE_STATE, "getHandoverConfig");
        mServiceSub.getHandoverConfig(listener);
    }

    @Override
    protected void onSetHandoverConfig(int hoConfig, IQtiImsExtListener listener) {
        mContext.enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "setHandoverConfig");
        mServiceSub.setHandoverConfig(hoConfig, listener);
    }
}
