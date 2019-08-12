/* Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved. Qualcomm Technologies Confidential and Proprietary.
 */

package com.qualcomm.qti.ims.tests;

import android.os.AsyncResult;
import android.test.AndroidTestCase;
import com.android.ims.ImsReasonInfo;
import org.codeaurora.ims.ImsCallUtils;
import org.codeaurora.ims.ImsQmiIF;
import org.codeaurora.ims.ImsRilException;
import com.qualcomm.qti.ims.vzw.SipServiceBinder;
import verizon.net.sip.internal.ISipRegistrationListener;
import verizon.net.sip.internal.ISipSession;
import verizon.net.sip.internal.ISipSessionListener;
import verizon.net.sip.internal.ISipService;

public class ImsCallUtilsTest extends AndroidTestCase {
    private static final int SIP_ERROR_CODE_100 = 100;
    private static final String SIP_ERROR_STRING_NORMAL_END = "Normal end";
    private static final String SIP_ERROR_STRING_NETWORK_ERROR = "Network error";

    protected void setUp() throws Exception {
        super.setUp();
    }

    protected void tearDown() throws Exception {
        super.tearDown();
    }

    public void testSipServiceBinder() {
        SipServiceBinder binder = new SipServiceBinder();
        assertTrue(binder.getSipSession(0) == null);
    }

    public void testGetImsReasonInfoAsyncResult() {
        //Input
        AsyncResult input = new AsyncResult(null,
            ImsTestUtils.createSipErrorInfo(SIP_ERROR_CODE_100,
            SIP_ERROR_STRING_NORMAL_END), null);

        //Test
        ImsReasonInfo output = ImsCallUtils.getImsReasonInfo(input);

        //Expect
        assertTrue(output.mCode == ImsReasonInfo.CODE_UNSPECIFIED);
        assertTrue(output.mExtraCode == SIP_ERROR_CODE_100);
        assertTrue(output.mExtraMessage.equals(SIP_ERROR_STRING_NORMAL_END));
    }

    public void testGetImsResonInfoAsyncResultException() {
       //Input
       ImsRilException ex = new ImsRilException(ImsQmiIF.E_REINVITE_COLLISION, null);
       AsyncResult input = new AsyncResult(null,
           ImsTestUtils.createSipErrorInfo(SIP_ERROR_CODE_100,
           SIP_ERROR_STRING_NETWORK_ERROR), ex);

       //Test
       ImsReasonInfo output = ImsCallUtils.getImsReasonInfo(input);

       //Expect
       assertTrue(output.mCode == ImsReasonInfo.CODE_SUPP_SVC_REINVITE_COLLISION);
       assertTrue(output.mExtraCode == SIP_ERROR_CODE_100);
       assertTrue(output.mExtraMessage.equals(SIP_ERROR_STRING_NETWORK_ERROR));
    }

    public void testGetImsReasonInfoNullError() {
       //Input
       int errorCode = 200;

       //Test
       ImsReasonInfo output = ImsCallUtils.getImsReasonInfo(null, errorCode);

       //Expect
       assertTrue(output.mCode == errorCode);
       assertTrue(output.mExtraCode == 0);
       assertTrue(output.mExtraMessage == null);
    }
}
