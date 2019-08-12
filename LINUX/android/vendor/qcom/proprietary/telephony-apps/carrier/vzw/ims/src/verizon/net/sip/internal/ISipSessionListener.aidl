/* Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package verizon.net.sip.internal;

import  android.net.sip.SipProfile;
import java.lang.String;
import java.util.Map;

import verizon.net.sip.internal.ISipSession;

/**
 * {@hide}
 */
oneway interface ISipSessionListener {

        void onCallBusy(in ISipSession iSession);

        void onCallEnded(in ISipSession iSession);

        void onCallEstablished(in ISipSession iSession);

        void onCallHeld(in ISipSession iSession);

        void onCalling(in ISipSession iSession);

        void onChanged(in ISipSession iSession);

        void onError(in ISipSession iSession, int errorCode, String errorMessage);

        void onReadyToCall(in ISipSession iSession);

        void onRinging(in ISipSession iSession, in SipProfile caller);

        void onRingingBack(in ISipSession iSession);

        void onChangeRequest(in ISipSession iSession, int type);

        void onVideoAvailable(in ISipSession iSession);

        void onCallResumed(in ISipSession iSession);

        void onVideoHeld(in ISipSession iSession);

        void onVideoResumed(in ISipSession iSession);

        void onConferenceEvent(in ISipSession iSession, String event, in SipProfile participant);

        void onOrientationChanged(in ISipSession iSession);

        void onSipReceived(in ISipSession iSession, String sipMessage);
    }
