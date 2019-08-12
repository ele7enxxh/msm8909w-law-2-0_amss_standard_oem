
/******************************************************************************
  @file    RadioConfigMsgPacker.java

  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.qti.radioconfiginterface;

import android.util.Log;
import com.qualcomm.qti.radioconfiginterface.RadioConfigInterface;

public class RadioConfigMsgPacker {
    private static final String LOG_TAG = "RadioConfigMsgPacker";

    //Can have a helper function for packing the item. just a thought
    public static byte[] packMsg (int msgId, int msgType, Object msg) {
        byte[] bytes = null;
        Log.i(LOG_TAG, "packMsg() - msgId: " + msgId + ", msgType: " + msgType);

        try {
            if (msgType == RadioConfigInterface.RADIO_CONFIG_MSG_REQUEST) {
                //no need to differentiate. To clean up
                switch (msgId) {
                case RadioConfigInterface.RADIO_CONFIG_SET_CONFIG:
                    bytes = ((RadioConfigInterface.ConfigMsg)msg).toByteArray();
                    break;
                case RadioConfigInterface.RADIO_CONFIG_GET_CONFIG:
                    bytes = ((RadioConfigInterface.ConfigMsg)msg).toByteArray();
                    break;
                default:
                    Log.e(LOG_TAG, "unexpected msgId");
                };
            } else {
                Log.e(LOG_TAG, "unexpected msgType");
            }
        } catch (Exception e) {
            Log.e(LOG_TAG, "Exception in msg protobuf encoding");
        }

        return bytes;
    }

    public static Object unpackMsg(int msgId, int msgType, byte[] bytes) {
        Object msg = null;
        Log.i(LOG_TAG, "unpackMsg() - msgId: " + msgId + ", msgType: " + msgType);

        try {
            if (msgType == RadioConfigInterface.RADIO_CONFIG_MSG_RESPONSE) {
                switch (msgId) {
                case RadioConfigInterface.RADIO_CONFIG_SET_CONFIG:
                    msg = RadioConfigInterface.ConfigMsg.parseFrom(bytes);
                    break;
                case RadioConfigInterface.RADIO_CONFIG_GET_CONFIG:
                    msg = RadioConfigInterface.ConfigMsg.parseFrom(bytes);
                    break;
                default:
                    Log.e(LOG_TAG, "unexpected msgId");
                };
            } else if (msgType == RadioConfigInterface.RADIO_CONFIG_MSG_INDICATION) {
//              switch (msgId) {
//
//              }
                Log.i(LOG_TAG, "Indications are currently not supported.");

            } else {
                Log.e(LOG_TAG, "unexpected msgType");
            }
        } catch (Exception e) {
            Log.e(LOG_TAG, "Exception in msg protobuf decoding");
        }

        return msg;
    }

    public static byte[] packTag(RadioConfigInterface.MessageTag tag) {
        Log.i(LOG_TAG, "packTag()");
        byte[] bytes = null;
        try {
            bytes = tag.toByteArray();
        } catch (Exception e) {
            Log.e(LOG_TAG, "Exception in msg protobuf encoding");
        }
        return bytes;
    }

    public static RadioConfigInterface.MessageTag unpackTag(byte[] bytes) {
        Log.i(LOG_TAG, "unpackTag()");
        RadioConfigInterface.MessageTag tag = null;
        try {
            tag = RadioConfigInterface.MessageTag.parseFrom(bytes);
        } catch (Exception e) {
            Log.e(LOG_TAG, "Exception in tag protobuf decoding");
        }
        return tag;
    }
}
