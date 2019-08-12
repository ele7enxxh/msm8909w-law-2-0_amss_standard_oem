/******************************************************************************
 * @file    GetSlotMappingData.java
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/
package com.samautotester.model;

public class GetSlotMappingData {

    // input
    // public int[] mSlotMap;

    // output
    public int mReturnValue;
    public Object mResult;
    public int[] mSlotMap;

    // state
    public State state = State.STOPPED;
}
