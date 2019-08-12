/******************************************************************************
 * @file    StreamApduData.java
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/
package com.samautotester.model;

import java.util.concurrent.atomic.AtomicReference;

public class StreamApduData {
    // input
    public String mSlotID = "";
    public String mClassType = "";
    public String mInstruction = "";
    public String mParam1 = "";
    public String mParam2 = "";
    public String mParam3 = "";
    public String mAPDUCmd = "";

    // output
    public int mReturnValue;
    public Object mResult;
    public AtomicReference<String> mCallbackResult;

    // state
    public State state = State.STOPPED;
}
