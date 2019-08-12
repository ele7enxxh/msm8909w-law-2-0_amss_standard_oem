/******************************************************************************
 * @file    SAMManagerWrapper.java
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/
package com.samautotester.controller;

import android.content.Context;
import android.os.Handler;
import android.util.Log;
import android.widget.Toast;

import com.qualcomm.qti.sam.manager.SAMManager;
import com.qualcomm.qti.sam.interfaces.CardState;
import com.qualcomm.qti.sam.interfaces.CardATR;
import com.qualcomm.qti.sam.interfaces.SlotMap;
import com.qualcomm.qti.sam.manager.SAMUnsolicitedCallback;
import com.qualcomm.qti.sam.manager.SAMResponseCallback;
import com.samautotester.model.TempSAMResponseCallback;
import com.samautotester.model.TempSAMUnsolicitedCallback;

public class SAMManagerWrapper {
    private static final String LOG_TAG = SAMManagerWrapper.class
            .getSimpleName();
    private SAMManager manager;
    private Context mContext;
    private SAMUnsolicitedCallback unsolicitedCallback = new TempSAMUnsolicitedCallback();
    private SAMResponseCallback responseCallback;

    public SAMManagerWrapper(Context context) {
        mContext = context;
        responseCallback = new TempSAMResponseCallback(mContext);
        manager = new SAMManager(context);
    }

    public boolean isServiceConnected() {
        return manager.isServiceConnected();
    }

    public boolean connectService(Handler callbackHandler,
            int serviceConnectionStatusId) {
        return manager.connectService(callbackHandler,
                serviceConnectionStatusId);
    }

    public void disconnectService() {
        manager.disconnectService();
    }

    public int registerCallback() {
        return manager.registerUnsolicitedResponseCallback(unsolicitedCallback);
    }

    public int deregisterCallback() {
        return manager
                .deregisterUnsolicitedResponseCallback(unsolicitedCallback);
    }

    public int getCardState(int slotId, TestFunc testfunc) {
        CardState cardState = new CardState();
        int returnValue = manager.getCardState(slotId, cardState);
        if (returnValue == 0) {
            switch (cardState.getCardState()) {
            case CardState.CARD_STATE_ABSENT:
                testfunc.set_CardState_callback("CARD_STATE_ABSENT");
                break;
            case CardState.CARD_STATE_PRESENT:
                testfunc.set_CardState_callback("CARD_STATE_PRESENT");
                break;
            case CardState.CARD_STATE_ERROR:
                testfunc.set_CardState_callback("CARD_STATE_ERROR");
                break;
            }
        } else
            testfunc.set_CardState_callback("");

        return returnValue;
    }

    public int getCardATR(int slotId, TestFunc testfunc) {
        CardATR cardATR = new CardATR();
        int returnValue = manager.getCardATR(slotId, cardATR);

        if (returnValue == 0) {
            testfunc.set_CardATR_callback(cardATR.getCardATR());
        } else {
            testfunc.set_CardATR_callback("");
        }
        return returnValue;
    }

    public int streamApdu(int slotId, int class_type, int instruction,
            int param1, int param2, int param3, String dataCmd) {
        return manager.streamApdu(slotId, class_type, instruction, param1,
                param2, param3, dataCmd, responseCallback);
    }

    public int switchSlot(int[] slotList) {
        SlotMap slotMap = new SlotMap();
        slotMap.setSlotMap(slotList);
        return manager.switchSlot(slotMap, responseCallback);
    }

    public int getSlotMapping(TestFunc testfunc) {
        int returnValue = -1;
        try {
            SlotMap slotMap = new SlotMap();
            returnValue = manager.getSlotMapping(slotMap);

            testfunc.set_GetSlotMapping_slotMap(slotMap.getSlotMap());
        } catch (Exception exception) {
            Log.d(LOG_TAG, exception.getLocalizedMessage());
        }

        return returnValue;
    }

    public int cardPowerUp(int slotId) {
        return manager.cardPowerUp(slotId, responseCallback);
    }

    public int cardPowerDown(int slotId) {
        return manager.cardPowerDown(slotId, responseCallback);
    }
}
