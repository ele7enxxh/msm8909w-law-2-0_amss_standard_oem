/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.ims.tests;

import android.app.PendingIntent;
import android.content.Context;
import android.net.Uri;
import android.os.AsyncResult;
import android.os.Handler;
import android.test.AndroidTestCase;
import android.util.Log;

import java.lang.reflect.*;
import java.util.concurrent.CountDownLatch;

import com.android.ims.ImsCallProfile;
import com.android.ims.ImsConfig;
import com.android.ims.ImsReasonInfo;
import com.android.ims.ImsServiceClass;
import com.android.ims.internal.IImsCallSessionListener;
import com.android.ims.internal.IImsRegistrationListener;
import com.android.ims.internal.IImsEcbm;
import com.android.ims.internal.IImsMultiEndpoint;
import com.android.ims.internal.IImsUtListener;
import com.android.ims.internal.ImsCallSession;
import com.android.internal.telephony.CallForwardInfo;
import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.DriverCall;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;

import org.codeaurora.ims.ImsSenderRxr;
import org.codeaurora.ims.ImsServiceSub;
import org.codeaurora.ims.ImsQmiIF;

public class ImsServiceSubTest extends AndroidTestCase {

    private ImsServiceSub mServiceSub;
    private Object mServiceSubHandler;
    private final String LOG_TAG = "ImsServiceSubTest";

    private final int DEFAULT_TEST_SUB = 0;

    protected void setUp() throws Exception {
        super.setUp();
        mServiceSub = new ImsServiceSub(DEFAULT_TEST_SUB, getContext());
        // Get and cache the ImsServiceSub handler reference, for relfection.
        Field mHandlerFieldRef = mServiceSub.getClass().getDeclaredField("mHandler");
        mHandlerFieldRef.setAccessible(true);
        mServiceSubHandler = mHandlerFieldRef.get(mServiceSub);
    }

    protected void tearDown() throws Exception {
        super.tearDown();
    }

    /**
     * Run all the test cases related to self-identity URI parsing and
     * propagation via callback.
     */
    public void testUriExtractionAndPropagationTests() {
        testUriExtraction();
        testUriExtractionWithSeparatorAtTheEnd();
        testUriExtractionWithEmptyInput();
        testUriPropagationInImsStateChangedHandler();
    }

    /**
     * Utility method to get ImsServiceSub Handler method with the specified
     * name and arguments.
     */
    private Method getHandlerMethod(String methodName, Class<?>... parameterTypes) {
        Method m = null;
        try {
            m = mServiceSubHandler.getClass().getDeclaredMethod(methodName, parameterTypes);
            m.setAccessible(true);
        } catch (Exception exc) {
            fail("Could not find handler method '" + methodName
                    + "' with given arguments.\n" + exc.toString());
        }
        return m;
    }

    /*
     * This test tests the extraction of multiple, '|'-separated URI strings,
     * that are passed as a single String from RIL.
     */
    private void testUriExtraction() {
        try {
            Method m = getHandlerMethod("extractUrisFromPipeSeparatedUriStrings", String.class);
            Object ret = m.invoke(mServiceSubHandler,
                    "sip:test123@mob-best-network.org|tel:hello@utf-mob.com|sip:11112@ATorT.com");
            Uri[] resUris = (Uri[])ret;
            assertEquals(3, resUris.length);
            if (!resUris[0].toString().equals("sip:test123@mob-best-network.org")
                    || !resUris[1].toString().equals("tel:hello@utf-mob.com")
                    || !resUris[2].toString().equals("sip:11112@ATorT.com")) {
                fail("URI extraction failure. Mismatch in expected-result URIs.");
            }
        } catch (Exception exc) {
            fail(exc.toString());
        }
    }

    /*
     * This test tests the extraction of multiple, '|'-separated URI strings,
     * that are passed as a single String from RIL. An invalid '|' character is present
     * at the end of the String; but the functionality should not be affected by this.
     */
    private void testUriExtractionWithSeparatorAtTheEnd() {
        try {
            Method m = getHandlerMethod("extractUrisFromPipeSeparatedUriStrings", String.class);
            Object ret = m.invoke(mServiceSubHandler,
                    "sip:test123@mob-best-network.org|tel:hello@utf-mob.com|sip:11112@ATorT.com|");
            Uri[] resUris = (Uri[])ret;
            assertEquals(3, resUris.length);
            if (!resUris[0].toString().equals("sip:test123@mob-best-network.org")
                    || !resUris[1].toString().equals("tel:hello@utf-mob.com")
                    || !resUris[2].toString().equals("sip:11112@ATorT.com")) {
                fail("URI extraction failure. Mismatch in expected-result URIs.");
            }
        } catch (Exception exc) {
            fail(exc.toString());
        }
    }

    /*
     * This test tests the handling of the URI extraction logic, when the (invalid) input
     * Null String is passed into it. The method is expected to return null.
     */
    private void testUriExtractionWithEmptyInput() {
        try {
            Method m = getHandlerMethod("extractUrisFromPipeSeparatedUriStrings", String.class);
            Object ret = m.invoke(mServiceSubHandler, "");
            assertEquals(null, ret);
        } catch (Exception exc) {
            fail(exc.toString());
        }
    }

    private class TestResult {
        public boolean result;

        public TestResult(boolean res) {
            result = res;
        }
    }

    private class TestRegistrationListener extends IImsRegistrationListener.Stub {
        public CountDownLatch latch;
        public TestResult result;

        public TestRegistrationListener(CountDownLatch l, TestResult r) {
            super();
            latch = l;
            result = r;
        }

        // Dummy implementations.
        @Deprecated
        public void registrationConnected() {}
        @Deprecated
        public void registrationProgressing() {}
        @Override
        public void registrationConnectedWithRadioTech(int imsRadioTech) {}
        @Override
        public void registrationProgressingWithRadioTech(int imsRadioTech) {}
        @Override
        public void registrationDisconnected(ImsReasonInfo imsReasonInfo) {}
        @Override
        public void registrationResumed() {}
        @Override
        public void registrationSuspended() {}
        @Override
        public void registrationServiceCapabilityChanged(int serviceClass, int event) {}
        @Override
        public void registrationFeatureCapabilityChanged(int serviceClass,
                int[] enabledFeatures, int[] disabledFeatures) {}
        @Override
        public void voiceMessageCountUpdate(int count) {}
        @Override
        public void registrationAssociatedUriChanged(Uri[] uris) {}
    }

    /*
     * This test tests the extraction and propagation of multiple URIs. The test
     * listener is expected to receive an array or URIs. Each URI is checked, and
     * has to match the expected URI string, for the test to pass.
     */
    private void testUriPropagationInImsStateChangedHandler() {
        try {
            Method m = getHandlerMethod("handleImsStateChanged", AsyncResult.class);

            final CountDownLatch latch = new CountDownLatch(1);
            final TestResult res = new TestResult(true);
            IImsRegistrationListener testRegListener = new TestRegistrationListener(latch, res) {
                public void registrationAssociatedUriChanged(final Uri[] uris) {
                    if (!uris[0].toString().equals("sip:test123@mob-best-network.org")
                            || !uris[1].toString().equals("tel:hello@utf-mob.com")
                            || !uris[2].toString().equals("sip:11112@ATorT.com")) {
                        res.result = false;
                    }
                    latch.countDown();
                }
            };

            // Initialize the ImsServiceClassTracker instance, that will call the
            // registration callback. The test listener is set here as well.
            mServiceSub.getServiceId(ImsServiceClass.MMTEL, null, testRegListener);

            ImsQmiIF.Registration reg = new ImsQmiIF.Registration();
            // NOTE: Registration status and RadioTech values are of no significance to this
            //       test case, and are dummy values, required by the method being tested.
            reg.setState(ImsQmiIF.Registration.NOT_REGISTERED);
            reg.setRadioTech(ImsQmiIF.RADIO_TECH_LTE);
            reg.setPAssociatedUris(
                    "sip:test123@mob-best-network.org|tel:hello@utf-mob.com|sip:11112@ATorT.com");

            AsyncResult ar = new AsyncResult(null, reg, null);

            m.invoke(mServiceSubHandler, ar);
            latch.await();

            assertTrue(res.result);

        } catch (Exception exc) {
            fail(exc.toString());
        }
    }

}

