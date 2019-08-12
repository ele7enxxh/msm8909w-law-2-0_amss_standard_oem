/******************************************************************************
 * @file    TempSAMResponseCallback.java
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

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.widget.Toast;

import com.qualcomm.qti.sam.manager.SAMResponseCallback;
import com.samautotester.controller.SAMExecutor;
import com.samautotester.controller.TestFunc;

public class TempSAMResponseCallback extends SAMResponseCallback {
    private Context mContext;

    /*
     * public TempSAMResponseCallback() { }
     */

    public TempSAMResponseCallback(Context context) {
        mContext = context;
    }

    private static final String LOG_TAG = TempSAMResponseCallback.class
            .getName();

    public void streamApduResponse(int slotId, int actionStatus,
            int statusWord1, int statusWord2, String dataRsp) {
        StringBuilder builder = new StringBuilder();
        builder.append("streamApduResponse : ");
        builder.append("slotId -> " + slotId);
        builder.append(" , ");
        builder.append("actionStatus -> " + actionStatus);
        builder.append(" , ");
        builder.append("statusWord1 -> " + statusWord1);
        builder.append(" , ");
        builder.append("statusWord2 -> " + statusWord2);
        builder.append(" , ");
        builder.append("dataRsp -> " + dataRsp);
        // Toast.makeText(mContext, "streamApduResponse :  dataRsp "+dataRsp,
        // Toast.LENGTH_SHORT).show();
        // Log.d(LOG_TAG,"SAMResponseCallback - streamApduResponse() "+"dataRsp "+dataRsp);
        // TestFunc.INDIVIDUAL.set_StreamApdu_callbackResult(new
        // AtomicReference<String>(builder.toString()));

        Log.d(LOG_TAG, "SAMResponseCallback - streamApduResponse() "
                + "dataRsp " + dataRsp);

        Intent intent = new Intent();
        intent.setAction("android.intent.action.SAM_CALLBACK");
        intent.putExtra("message", builder.toString());
        mContext.sendBroadcast(intent);

    }

    public void switchSlotResponse(int actionStatus) {
        // Toast.makeText(mContext,
        // "switchSlotResponse :  actionStatus "+actionStatus,
        // Toast.LENGTH_SHORT).show();
        Log.d(LOG_TAG, "SAMResponseCallback - switchSlotResponse() "
                + "actionStatus " + actionStatus);

        StringBuilder builder = new StringBuilder();
        builder.append("switchSlotResponse : ");
        builder.append("actionStatus -> " + actionStatus);

        Intent intent = new Intent();
        intent.setAction("android.intent.action.SAM_CALLBACK");
        intent.putExtra("message", builder.toString());
        mContext.sendBroadcast(intent);

    }

    public void cardPowerUpResponse(int slotId, int actionStatus) {
        // Toast.makeText(mContext,
        // "cardPowerUpResponse :  slotId : "+slotId+" , actionStatus : "+actionStatus,
        // Toast.LENGTH_SHORT).show();
        Log.d(LOG_TAG, "SAMResponseCallback - cardPowerUpResponse() "
                + "slotId : " + slotId + " , actionStatus : " + actionStatus);

        StringBuilder builder = new StringBuilder();
        builder.append("cardPowerUpResponse : ");
        builder.append("slotId -> " + slotId);
        builder.append(" , ");
        builder.append("actionStatus -> " + actionStatus);

        Intent intent = new Intent();
        intent.setAction("android.intent.action.SAM_CALLBACK");
        intent.putExtra("message", builder.toString());
        mContext.sendBroadcast(intent);

    }

    public void cardPowerDownResponse(int slotId, int actionStatus) {
        // Toast.makeText(mContext,
        // "cardPowerDownResponse :  slotId : "+slotId+" , actionStatus : "+actionStatus,
        // Toast.LENGTH_SHORT).show();
        Log.d(LOG_TAG, "SAMResponseCallback - cardPowerDownResponse() "
                + " slotId : " + slotId + " , actionStatus : " + actionStatus);

        StringBuilder builder = new StringBuilder();
        builder.append("cardPowerDownResponse : ");
        builder.append("slotId -> " + slotId);
        builder.append(" , ");
        builder.append("actionStatus -> " + actionStatus);

        Intent intent = new Intent();
        intent.putExtra("message", builder.toString());
        intent.setAction("android.intent.action.SAM_CALLBACK");
        mContext.sendBroadcast(intent);

    }
}
