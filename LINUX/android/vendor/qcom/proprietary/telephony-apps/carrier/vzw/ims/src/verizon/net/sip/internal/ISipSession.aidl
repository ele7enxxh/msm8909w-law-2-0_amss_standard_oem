/* Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package verizon.net.sip.internal;

import android.view.Surface;
import android.os.Message;
import android.net.sip.SipProfile;

import java.lang.String;
import java.util.Map;

import verizon.net.sip.internal.ISipSessionListener;
import verizon.net.sip.internal.ICamera;

/**
 * {@hide}
 */
interface ISipSession {

    void setListener(in ISipSessionListener listener, boolean callbackImmediately);

    boolean makeCall(String peerUri, int timeout, in Map extraHeaders, String fromUri);

    void answerCall(int type, int timeout);

    void endCall(int reason, int timeout);

    void resumeCall(int timeout);

    void holdCall(int timeout);

    void holdVideo();

    void continueVideo();

    int getState();

    int getCallType();

    SipProfile[] getPeerProfile();

    void sendSessionModifyRequest(int type, int timeout);

    void sendSessionModifyResponse(int type, int timeout);

    void startAudio();

    void startVideo();

    void startMedia();

    void setOrientation(int orientation);

    void setCamera(int id);

    void setCamera2(String id);

    void setDisplay(in Surface surface);

    ICamera getCamera();

    void sendDtmf(int code, in Message result);

    void close();

    void removeLastParticipant(int timeout);

    void setListener2(in Map filter, in ISipSessionListener listener);

    String sendSIP(String sip, in String imsServer, String sipMessage);

    // This API is not supported for now.
    // String sendSIP2(String requestUri, String imsServer, in SipRequest sipMessage,
    //     String to,String from);

    void setupMediaPath(in String[] remoteIp);
}
