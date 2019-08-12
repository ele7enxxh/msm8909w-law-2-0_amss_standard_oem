/******************************************************************************
  @file    RadioConfigSyncRequest.java

  ---------------------------------------------------------------------------
  Copyright (c) 2014,2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.qti.radioconfiginterface;

import android.util.Log;
import java.lang.InterruptedException;
//need to handle sync for setter and getter nmethods??!!

public class RadioConfigSyncRequest {

    private boolean responseReceived = false;
    private String LOG_TAG = "RadioConfigSyncRequest";

    //whether set/get req
    private int msgId;
    private int mToken;

    private byte[] reqBytes;

    private byte[] respBytes;

    public RadioConfigSyncRequest() {

    }

    public RadioConfigSyncRequest(int token, int msgId) {
        this.msgId = msgId;
        mToken = token;
        LOG_TAG = LOG_TAG + "t-" + token;
    }

    public synchronized int getMsgId() {
        Log.i(LOG_TAG, "getMsgId");
        return msgId;
    }

    public synchronized int getToken() {
        Log.i(LOG_TAG, "getToken");
        return mToken;
    }

    public synchronized void setReqBytes(byte[] source){
        Log.i(LOG_TAG, "setReqBytes");
        if(source == null) {
            Log.e(LOG_TAG, "nullbytes");
            return;
        }
        reqBytes = new byte[source.length];
        System.arraycopy(source, 0, reqBytes, 0, source.length);

    }
    public synchronized byte[] getReqBytes() {
        Log.i(LOG_TAG, "getReqBytes");
        if(reqBytes == null)
            return null;
        byte[] ret = new byte[reqBytes.length];
        System.arraycopy(reqBytes, 0, ret, 0, reqBytes.length);
        return ret;
    }

    public synchronized void setRespBytes(byte[] source){
        Log.i(LOG_TAG, "setRespBytes");
        if(source == null) {
            Log.e(LOG_TAG, "nullbytes");
            return;
        }
        respBytes = new byte[source.length];
        System.arraycopy(source, 0, respBytes, 0, source.length);

    }
    public synchronized byte[] getRespBytes() {
        Log.i(LOG_TAG, "getRespBytes");
        if(respBytes == null)
            return null;
        byte[] ret = new byte[respBytes.length];
        System.arraycopy(respBytes, 0, ret, 0, respBytes.length);
        return ret;
    }
    public synchronized void waitForResp() {
        Log.i(LOG_TAG, "waitforresp!!!");

        //responseReceived = false;
        while(!responseReceived) {
            try {
                wait(); //timeout???
            } catch(InterruptedException e) {
                //log
                Log.e(LOG_TAG, "WaitForResp - exception? ");
            }
        }
        Log.i(LOG_TAG, "finished wait for resp!!");
    }

    public synchronized void notifyResp() {
        Log.i(LOG_TAG, "notifyResp");
        responseReceived = true;
        notify();
        Log.i(LOG_TAG, "notified!!");
    }
}
