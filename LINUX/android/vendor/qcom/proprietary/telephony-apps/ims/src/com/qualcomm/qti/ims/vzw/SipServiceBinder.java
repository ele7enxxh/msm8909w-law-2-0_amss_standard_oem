/* Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.ims.vzw;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;

import java.util.Map;

import verizon.net.sip.internal.ISipRegistrationListener;
import verizon.net.sip.internal.ISipSession;
import verizon.net.sip.internal.ISipSessionListener;
import verizon.net.sip.internal.ISipService;


public class SipServiceBinder extends ISipService.Stub {

    @Override
    public void open(String packageName, PendingIntent incomingIntent,
            ISipRegistrationListener listener, int priority) {
    }

    @Override
    public void setRegistrationListener(ISipRegistrationListener listener) {

    }

    @Override
    public void setCallingListener(ISipSessionListener listener) {

    }

    @Override
    public void resetCallingListeneer(ISipSessionListener listener) {

    }

    @Override
    public ISipSession createSipSession() {
        return null;

    }

    @Override
    public ISipSession getSipSession(int id) {
        return null;
    }

    @Override
    public ISipSession makeMediaCall(String peerUri, ISipSessionListener listener,
         int type, int timeout) {
        return null;
    }

    @Override
    public boolean isRegistered(String localProfileUri) {
        return false;
    }

    @Override
    public ISipSession mergeMediaCalls(ISipSession call1, ISipSession call2, int timeout) {
        return null;
    }

    @Override
    public ISipSession openSipDialog(String packageName, PendingIntent incoming, Map filter,
            ISipRegistrationListener listener) {
        return null;
    }

    @Override
    public String getRegisterResponse() {
        return null;
    }

    @Override
    public void enableCallWaitingRules(boolean enableRule) {

    }
}
