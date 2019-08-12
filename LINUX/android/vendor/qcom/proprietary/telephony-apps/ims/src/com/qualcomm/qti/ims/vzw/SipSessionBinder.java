/* Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.ims.vzw;

import android.view.Surface;
import android.os.Message;
import android.net.sip.SipProfile;

import java.lang.String;
import java.util.Map;

import verizon.net.sip.internal.ISipSessionListener;
import verizon.net.sip.internal.ICamera;
import verizon.net.sip.internal.ISipSession;

/**
 * {@hide}
 */
class SipSessionBinder extends ISipSession.Stub {

    @Override
    public void setListener(ISipSessionListener listener, boolean callbackImmediately) {
    }

    @Override
    public boolean makeCall(String peerUri, int timeout, Map extraHeaders, String fromUri) {
        return false;
    }

    @Override
    public void answerCall(int type, int timeout) {
    }

    @Override
    public void endCall(int reason, int timeout) {
    }

    @Override
    public void resumeCall(int timeout) {
    }

    @Override
    public void holdCall(int timeout) {
    }

    @Override
    public void holdVideo() {
    }

    @Override
    public void continueVideo() {
    }

    @Override
    public int getState() {
        return 0;
    }

    @Override
    public int getCallType() {
        return 0;
    }

    @Override
    public SipProfile[] getPeerProfile() {
        return null;
    }

    @Override
    public void sendSessionModifyRequest(int type, int timeout) {
    }

    @Override
    public void sendSessionModifyResponse(int type, int timeout) {
    }

    @Override
    public void startAudio() {
    }

    @Override
    public void startVideo() {
    }

    @Override
    public void startMedia() {
    }

    @Override
    public void setOrientation(int orientation) {
    }

    @Override
    public void setCamera(int id) {
    }

    @Override
    public void setCamera2(String id) {
    }

    @Override
    public void setDisplay(Surface surface) {
    }

    @Override
    public ICamera getCamera() {
        return null;
    }

    @Override
    public void sendDtmf(int code, Message result) {
    }

    @Override
    public void close() {
    }

    @Override
    public void removeLastParticipant(int timeout) {
    }

    @Override
    public void setListener2(Map filter, ISipSessionListener listener) {
    }

    @Override
    public String sendSIP(String sip, String imsServer, String sipMessage) {
        return null;
    }

    // This API is not supported for now.
    //@Override
    // public String sendSIP2(String requestUri, String imsServer, SipRequest sipMessage,
    //     String to,String from) {
    //}

    @Override
    public void setupMediaPath(String[] remoteIp) {
    }
}



