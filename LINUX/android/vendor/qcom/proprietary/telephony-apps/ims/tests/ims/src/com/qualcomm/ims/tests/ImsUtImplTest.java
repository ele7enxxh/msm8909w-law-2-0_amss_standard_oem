/* Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.ims.tests;

import android.content.Context;
import android.os.AsyncResult;
import android.os.Message;
import android.test.AndroidTestCase;
import android.util.Log;
import com.android.ims.ImsReasonInfo;
import com.android.ims.internal.IImsUt;
import java.lang.reflect.*;
import org.codeaurora.ims.ImsQmiIF;
import org.codeaurora.ims.ImsSenderRxr;
import org.codeaurora.ims.ImsUtImpl;
import org.codeaurora.ims.ImsUtListenerProxy;

public class ImsUtImplTest extends AndroidTestCase {
    private static final String LOG_TAG = "ImsUtImplTest";
    private static final int SIP_ERROR_CODE_100 = 100;
    private static final String SIP_ERROR_STRING_NETWORK_ERROR = "Network Error";
    private static ImsSenderRxr sSenderRxr;
    private static ImsUtImpl sUtImpl;
    private static Class<?> sUtImplHandlerClass;
    private static Constructor<?> sUtImplHandlerConstructor;
    private static Object sUtImplHandler;
    private static Field sListenerProxy;

    // Supplementary Service Events
    // Copied from Ut Impl for unit testing
    private static final int EVENT_QUERY_CF    = 1;
    private static final int EVENT_UPDATE_CF   = 2;
    private static final int EVENT_QUERY_CW    = 3;
    private static final int EVENT_UPDATE_CW   = 4;
    private static final int EVENT_QUERY_CLIR  = 5;
    private static final int EVENT_UPDATE_CLIR = 6;
    private static final int EVENT_QUERY_CLIP  = 7;
    private static final int EVENT_UPDATE_CLIP = 8;
    private static final int EVENT_QUERY_COLR  = 9;
    private static final int EVENT_UPDATE_COLR = 10;
    private static final int EVENT_QUERY_COLP  = 11;
    private static final int EVENT_UPDATE_COLP = 12;
    private static final int EVENT_QUERY_CB    = 13;
    private static final int EVENT_UPDATE_CB   = 14;

    protected void setUp() throws Exception {
        super.setUp();
        sSenderRxr = new ImsSenderRxr(getContext());
        ImsUtImpl.createUtInterface(sSenderRxr, getContext());
        sUtImpl = ImsUtImpl.getUtInterface();
        sUtImplHandlerClass = Class.forName("org.codeaurora.ims.ImsUtImpl$ImsUtImplHandler");
        sUtImplHandlerConstructor = sUtImplHandlerClass.getDeclaredConstructor(ImsUtImpl.class);
        sUtImplHandlerConstructor.setAccessible(true);
        sUtImplHandler = sUtImplHandlerConstructor.newInstance(sUtImpl);
        sListenerProxy = sUtImpl.getClass().getDeclaredField("mListenerProxy");
        sListenerProxy.setAccessible(true);
    }

    protected void tearDown() throws Exception {
        super.tearDown();
    }

    public void testQueryCB() {
        //Inject
        Message msg = new Message();
        msg.what = EVENT_QUERY_CB;
        ImsQmiIF.SuppSvcResponse result = new ImsQmiIF.SuppSvcResponse();
        result.setErrorDetails(ImsTestUtils.createSipErrorInfo(SIP_ERROR_CODE_100,
            SIP_ERROR_STRING_NETWORK_ERROR));
        msg.obj = new AsyncResult(null, result, null);

        try {
            sListenerProxy.set(sUtImpl, new ImsUtListenerProxy() {
                public void utConfigurationQueryFailed(final IImsUt ut,
                    final int id, final ImsReasonInfo error) {
                     //Expect
                     assertTrue(error.mExtraCode == SIP_ERROR_CODE_100);
                     assertTrue(error.mExtraMessage.equals(SIP_ERROR_STRING_NETWORK_ERROR));
                }
            });

            Method m = sUtImplHandler.getClass().getDeclaredMethod("handleMessage", Message.class);
            m.setAccessible(true);
            //Test
            m.invoke(sUtImplHandler, msg);

        }
        catch (Exception e) {
            fail(e.toString());
        }
    }
}
