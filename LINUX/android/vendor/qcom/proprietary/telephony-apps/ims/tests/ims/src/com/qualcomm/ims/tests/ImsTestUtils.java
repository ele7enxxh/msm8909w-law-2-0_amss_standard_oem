/* Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved. Qualcomm Technologies Confidential and Proprietary.
 */
package com.qualcomm.qti.ims.tests;

import org.codeaurora.ims.ImsQmiIF;

public final class ImsTestUtils {

    private ImsTestUtils() {}

    protected static ImsQmiIF.SipErrorInfo createSipErrorInfo(int code, String info) {
       ImsQmiIF.SipErrorInfo errorInfo = new ImsQmiIF.SipErrorInfo();
       errorInfo.setSipErrorCode(code);
       errorInfo.setSipErrorString(info);
       return errorInfo;
    }
}
