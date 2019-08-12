/******************************************************************************
 * @file    CardPowerUpData.java
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

public class CardPowerUpData {
    // input
    public String mSlotID = "";

    // output
    public int mReturnValue;
    public Object mResult;

    // state
    public State state = State.STOPPED;

    public AtomicReference<String> mCallbackResult;
}
