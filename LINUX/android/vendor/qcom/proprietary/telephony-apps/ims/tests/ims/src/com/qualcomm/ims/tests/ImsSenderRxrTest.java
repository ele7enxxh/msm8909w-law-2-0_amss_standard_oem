/* Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved. Qualcomm Technologies Confidential and Proprietary.
 */
package com.qualcomm.qti.ims.tests;

import android.content.Context;
import android.test.AndroidTestCase;
import android.util.Log;
import com.android.ims.ImsReasonInfo;
import java.lang.reflect.*;
import java.util.ArrayList;
import org.codeaurora.ims.ImsSenderRxr;
import org.codeaurora.ims.ImsQmiIF;
import org.codeaurora.ims.DriverCallIms;

public class ImsSenderRxrTest extends AndroidTestCase {
    private static ImsSenderRxr sSenderRxr;
    private static final String LOG_TAG = "ImsSenderRxrTest";
    private static final int SIP_ERROR_CODE_200 = 200;
    private static final String SIP_ERROR_STRING_NORMAL_END = "Normal End";
    private static final String NUMBER_11111 = "11111";
    private static final String DUMMY_EXTRA = "DummyKey=DummyVal";

    protected void setUp() throws Exception {
        super.setUp();
        sSenderRxr = new ImsSenderRxr(getContext());
    }

    protected void tearDown() throws Exception {
        super.tearDown();
    }

    private ImsQmiIF.SipErrorInfo createSipErrorInfo(int code, String info) {
       ImsQmiIF.SipErrorInfo errorInfo = new ImsQmiIF.SipErrorInfo();
       errorInfo.setSipErrorCode(code);
       errorInfo.setSipErrorString(info);
       return errorInfo;
    }

    private byte[] createCallList() {
       ImsQmiIF.CallList callList = new ImsQmiIF.CallList();
       ImsQmiIF.CallList.Call call = new ImsQmiIF.CallList.Call();
       ImsQmiIF.CallFailCauseResponse failCause = new ImsQmiIF.CallFailCauseResponse();
       failCause.setErrorDetails(ImsTestUtils.createSipErrorInfo(SIP_ERROR_CODE_200,
           SIP_ERROR_STRING_NORMAL_END));
       failCause.setFailcause(ImsQmiIF.CALL_FAIL_SIP_REDIRECTED);
       call.setFailCause(failCause);
       call.setState(ImsQmiIF.CALL_END);
       call.setIndex(1);
       call.setIsMpty(false);
       call.setIsMT(false);
       call.setNumber(NUMBER_11111);

       ImsQmiIF.CallDetails details = new ImsQmiIF.CallDetails();
       details.setCallType(ImsQmiIF.CALL_TYPE_VOICE);
       details.setCallDomain(ImsQmiIF.CALL_DOMAIN_PS);
       details.addExtras(DUMMY_EXTRA);
       call.setCallDetails(details);

       callList.addCallAttributes(call);

       return callList.toByteArray();
    }

    public void testResponseCallList() {
        try {
            //Input
            Method m = sSenderRxr.getClass().getDeclaredMethod("responseCallList", byte[].class);
            m.setAccessible(true);

            //Test
            ArrayList<DriverCallIms> dcList = (ArrayList)m.invoke(sSenderRxr, createCallList());

            //Expect
            assertTrue(dcList != null);
            assertTrue(dcList.size() == 1);
            assertTrue(dcList.get(0).index == 1);
            assertTrue(dcList.get(0).isMpty == false);
            assertTrue(dcList.get(0).number.equals(NUMBER_11111));
            assertTrue(dcList.get(0).callFailCause.mCode == ImsReasonInfo.CODE_SIP_REDIRECTED);
            assertTrue(dcList.get(0).callFailCause.mExtraCode == SIP_ERROR_CODE_200);
            assertTrue(dcList.get(0).callFailCause.mExtraMessage.equals(SIP_ERROR_STRING_NORMAL_END));
        }
        catch (NoSuchMethodException e) {
            fail("NoSuchMethodException");
        }
        catch (IllegalAccessException e) {
            fail("IllegalAccessException");
        }
        catch (InvocationTargetException e) {
            fail("InvocationTargetException");
        }
    }
}
