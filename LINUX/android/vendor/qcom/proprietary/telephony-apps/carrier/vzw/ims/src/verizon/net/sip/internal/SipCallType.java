/* Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package verizon.net.sip.internal;

/**
 * {@hide}
 */
public class SipCallType {

    private SipCallType() {}

    /**
     * Call type of SIP media call
     */
    public static final int AUDIO_CALL = 0;
    public static final int VIDEO_SHARE = 1;
    public static final int VIDEO_CALL = 2;
}