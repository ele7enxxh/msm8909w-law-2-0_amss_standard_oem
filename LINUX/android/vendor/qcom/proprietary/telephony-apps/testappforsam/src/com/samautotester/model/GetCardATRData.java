/******************************************************************************
 * @file    GetCardATRData.java
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/
package com.samautotester.model;

public class GetCardATRData {
    // input
    public String mSlotID = "";

    // output
    public int mReturnValue;
    public Object mResult;
    public Object mCardATRCallback;

    // state
    public State state = State.STOPPED;
}
