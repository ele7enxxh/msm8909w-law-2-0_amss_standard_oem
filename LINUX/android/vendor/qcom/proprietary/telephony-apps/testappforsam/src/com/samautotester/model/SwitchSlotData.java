/******************************************************************************
 * @file    SwitchSlotData.java
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/
package com.samautotester.model;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicReference;

public class SwitchSlotData {
    // input
    public List<Integer> mSlotList = new ArrayList<Integer>();

    // output
    public int mReturnValue;
    public Object mResult;

    // state
    public State state = State.STOPPED;
    public AtomicReference<String> mCallbackResult;
}
