/* Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package verizon.net.sip.internal;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;

import java.util.Map;

import verizon.net.sip.internal.ISipRegistrationListener;
import verizon.net.sip.internal.ISipSession;
import verizon.net.sip.internal.ISipSessionListener;

/**
 * {@hide}
 */
interface ISipService {

    void open( String packageName, in PendingIntent incomingIntent,
             in ISipRegistrationListener listener, int priority);

    void setRegistrationListener(in ISipRegistrationListener listener);

    void setCallingListener(in ISipSessionListener listener);

    void resetCallingListeneer(in ISipSessionListener listener);

    ISipSession createSipSession();

    ISipSession getSipSession(int id);

    ISipSession makeMediaCall(String peerUri, in ISipSessionListener listener,
         int type, int timeout);

    boolean isRegistered(String localProfileUri);

    ISipSession mergeMediaCalls( in ISipSession call1, in ISipSession call2, int timeout);

    ISipSession openSipDialog( String packageName, in PendingIntent incoming, in Map filter,
         in ISipRegistrationListener listener);

    String getRegisterResponse();

    void enableCallWaitingRules(boolean enableRule);
}
