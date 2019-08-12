/* Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.ims.tests;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.test.AndroidTestCase;

import com.android.ims.ImsCallProfile;
import com.android.ims.ImsReasonInfo;
import com.android.ims.ImsServiceClass;
import com.android.ims.internal.IImsCallSessionListener;
import com.android.ims.internal.IImsRegistrationListener;
import com.qualcomm.ims.vt.ImsVideoCallProviderImpl;
import com.qualcomm.ims.vt.ImsVideoGlobals;
import com.qualcomm.ims.vt.LowBatteryHandler;

import org.codeaurora.ims.DriverCallIms;
import org.codeaurora.ims.ImsCallSessionImpl;
import org.codeaurora.ims.ImsSenderRxr;
import org.codeaurora.ims.ImsServiceClassTracker;
import org.codeaurora.ims.ImsServiceSub;


public class ImsCallSessionImplTest extends AndroidTestCase {
    private static ImsSenderRxr sSenderRxr;
    private static ImsServiceSub sServiceSub;
    private static ImsServiceClassTracker sServiceTacker;

    ImsCallProfile mCallProfile;
    DriverCallIms mDriverCallIms;

    protected void setUp() throws Exception {
        super.setUp();

        if (sServiceSub == null ) {
            staticInitialization(getContext());
        }

        mCallProfile = sServiceSub.createCallProfile(ImsServiceClass.MMTEL,
                ImsCallProfile.SERVICE_TYPE_NORMAL, ImsCallProfile.CALL_TYPE_VIDEO_N_VOICE);

        mDriverCallIms = new DriverCallIms();
        mDriverCallIms.callFailCause = new ImsReasonInfo();

    }

    // TODO Re-factor singletons to make it easier to unit-test.
    private static void staticInitialization(Context context) {

        sServiceSub = new ImsServiceSub(0, context);
        sSenderRxr = new ImsSenderRxr(context);
        ImsVideoGlobals.init(sServiceSub, context);

        Intent intent = new Intent("com.android.ims.IMS_INCOMING_CALL");
        intent.addFlags(Intent.FLAG_RECEIVER_FOREGROUND);
        PendingIntent pIntent = PendingIntent.getBroadcast(context, 0, intent,
                PendingIntent.FLAG_UPDATE_CURRENT);

        sServiceTacker = new ImsServiceClassTracker(ImsServiceClass.MMTEL, pIntent,
                sSenderRxr, context, sServiceSub);
    }

    protected void tearDown() throws Exception {
        super.tearDown();
        mCallProfile = null;
        mDriverCallIms = null;
    }

    public void testCreateVideoCallProvider_MO() {
        // VideoCallProvider shall be null since we are instantiating MO session while
        // video calling is disabled.
        ImsCallSessionImpl session = new ImsCallSessionImpl(mCallProfile,
                null, sSenderRxr, getContext(), sServiceTacker, false);
        assertTrue(session.getImsVideoCallProviderImpl() == null);
        assertTrue(session.getVideoCallProvider() == null);

        // This ensures we don't get NPE and/or other exceptions later.
        DriverCallIms activeDc = new DriverCallIms(mDriverCallIms);
        activeDc.state = DriverCallIms.State.ACTIVE;
        session.updateCall(activeDc);

        // VideoCallProvider shall still be null since updating voice capabilities should not
        // impact VideoCallProvider.
        session.updateFeatureCapabilities(false, true);
        assertTrue(session.getImsVideoCallProviderImpl() == null);
        assertTrue(session.getVideoCallProvider() == null);

        // Now UE is video capable hence video call provider shall not be null.
        session.updateFeatureCapabilities(true, true);
        assertFalse(session.getImsVideoCallProviderImpl() == null);
        assertFalse(session.getVideoCallProvider() == null);

        // Now UE is not video capable however we don't remove video call provider,
        // hence it should not be null.
        final ImsVideoCallProviderImpl tmp = session.getImsVideoCallProviderImpl();
        session.updateFeatureCapabilities(true, true);
        assertFalse(session.getImsVideoCallProviderImpl() == null);
        assertFalse(session.getVideoCallProvider() == null);
        assertEquals(tmp, session.getImsVideoCallProviderImpl());
    }

    public void testCreateVideoCallProvider_MT() {
        // VideoCallProvider shall be null since we are instantiating MO session while
        // video calling is disabled.
        ImsCallSessionImpl session = new ImsCallSessionImpl(mDriverCallIms,
                sSenderRxr, getContext(), sServiceTacker, false);
        assertTrue(session.getImsVideoCallProviderImpl() == null);
        assertTrue(session.getVideoCallProvider() == null);

        // This ensures we don't get NPE and/or other exceptions later.
        DriverCallIms activeDc = new DriverCallIms(mDriverCallIms);
        activeDc.state = DriverCallIms.State.ACTIVE;
        session.updateCall(activeDc);

        // VideoCallProvider shall still be null since updating voice capabilities should not
        // impact VideoCallProvider.
        session.updateFeatureCapabilities(false, true);
        assertTrue(session.getImsVideoCallProviderImpl() == null);
        assertTrue(session.getVideoCallProvider() == null);

        // Now UE is video capable hence video call provider shall not be null.
        session.updateFeatureCapabilities(true, true);
        assertFalse(session.getImsVideoCallProviderImpl() == null);
        assertFalse(session.getVideoCallProvider() == null);

        // Now UE is not video capable however we don't remove video call provider,
        // hence it should not be null.
        final ImsVideoCallProviderImpl tmp = session.getImsVideoCallProviderImpl();
        session.updateFeatureCapabilities(true, true);
        assertFalse(session.getImsVideoCallProviderImpl() == null);
        assertFalse(session.getVideoCallProvider() == null);
        assertEquals(tmp, session.getImsVideoCallProviderImpl());
    }

}

