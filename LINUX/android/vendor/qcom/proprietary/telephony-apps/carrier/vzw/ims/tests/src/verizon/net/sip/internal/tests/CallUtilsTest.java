/* Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package verizon.net.sip.internal.tests;

import android.test.AndroidTestCase;

import verizon.net.sip.internal.SipCallType;


public class CallUtilsTest extends AndroidTestCase {

    protected void setUp() throws Exception {
        super.setUp();
    }

    protected void tearDown() throws Exception {
        super.tearDown();
    }

    public void testSample() {
        assertTrue(SipCallType.AUDIO_CALL != SipCallType.VIDEO_SHARE);
    }
}

