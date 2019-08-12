/******************************************************************************
 * @file    TempSAMUnsolicitedCallback.java
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/
package com.samautotester.model;

import android.content.Context;
import android.util.Log;
import android.widget.Toast;

import com.qualcomm.qti.sam.manager.SAMUnsolicitedCallback;
import com.qualcomm.qti.sam.interfaces.CardState;

public class TempSAMUnsolicitedCallback extends SAMUnsolicitedCallback {
    private static final String LOG_TAG = TempSAMUnsolicitedCallback.class
            .getName();

    // private Context context;
    public TempSAMUnsolicitedCallback() {
        // this.context = context;
    }

    public void cardStateUpdateIndication(CardState cardState) {
        // Toast.makeText(context,
        // "CardState : SlotId - "+cardState.getSlotId()+" CardState - "+cardState.getCardState(),
        // Toast.LENGTH_SHORT).show();
        Log.d(LOG_TAG, "cardState - slotId - " + cardState.getSlotId());
        Log.d(LOG_TAG, "cardState - cardState - " + cardState.getCardState());
    }

}
