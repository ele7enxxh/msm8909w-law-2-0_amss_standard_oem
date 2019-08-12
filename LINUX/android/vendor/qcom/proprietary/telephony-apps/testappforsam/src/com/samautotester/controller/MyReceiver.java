/******************************************************************************
 * @file    MyReceiver.java
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/
package com.samautotester.controller;

import android.os.Bundle;
import android.os.Handler;
import android.os.ResultReceiver;

public class MyReceiver extends ResultReceiver {

    private Receiver mReceiver;

    public MyReceiver(Handler handler) {
        super(handler);
        // TODO Auto-generated constructor stub
    }

    public interface Receiver {
        public void onReceiveResult(int resultCode, Bundle resultData);
    }

    public void setReceiver(Receiver receiver) {
        mReceiver = receiver;
    }

    @Override
    protected void onReceiveResult(int resultCode, Bundle resultData) {
        mReceiver.onReceiveResult(resultCode, resultData);
    }
}
