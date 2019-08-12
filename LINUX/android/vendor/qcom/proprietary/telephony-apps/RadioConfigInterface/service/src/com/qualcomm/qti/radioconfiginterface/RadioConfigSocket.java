/******************************************************************************
  @file    RadioConfigSocket.java

  ---------------------------------------------------------------------------
  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.qti.radioconfiginterface;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Arrays;
import java.lang.Byte;

import com.qualcomm.qti.radioconfiginterface.RadioConfigSyncRequest;
import com.qualcomm.qti.radioconfiginterface.RadioConfigService;

import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

public class RadioConfigSocket extends Handler implements Runnable {
    private final String LOG_TAG = "RadioConfigSocket";

    //to remove after MSIM arch is finalized
    private final String SOCKET_ADDRESS = "qmux_radio/qcril_radio_config0";
    private final String SOCKET_ADDRESS_1 = "qmux_radio/qcril_radio_config1";
    private final String SOCKET_ADDRESS_2 = "qmux_radio/qcril_radio_config2";
    private final int SOCKET_FAILED_RETRY_TIME = 20;
    private final int SOCKET_FAILED_SLEEP_TIME = 4000;
    private LocalSocket mSocket = null;
    private InputStream mIS = null;
    private OutputStream mOS = null;

    private final int BUFFER_SIZE = 1024;
    private byte[] mBuffer = new byte[BUFFER_SIZE];
    private int mBufferLength = 0;

    private Handler mRecvHdlr = null;

    private boolean mToDestroy = false;

    private Integer mInstanceId;

    public RadioConfigSocket(Handler recvHdlr, Integer instanceId){
        Log.i(LOG_TAG, "RadioConfigSocket(): instanceId = " + instanceId);
        mRecvHdlr = recvHdlr;
        mInstanceId = instanceId;
    }

    public void toDestroy() {
        Log.i(LOG_TAG, "toDestroy()");
        mToDestroy = true;
        resetSocket();
    }

    private boolean connectSocket() {
        Log.i(LOG_TAG, "connectSocket()");
        resetSocket();

        int counter = 0;

        while (counter < SOCKET_FAILED_RETRY_TIME) {
            try {
                String socketAddress = SOCKET_ADDRESS;

                if (mInstanceId == 2) {
                    socketAddress = SOCKET_ADDRESS_2;
                } else if (mInstanceId == 1) {
                    socketAddress = SOCKET_ADDRESS_1;
                } else {
                    socketAddress = SOCKET_ADDRESS;
                }

                Log.i(LOG_TAG, "Connecting to SUB_" + mInstanceId +
                        " socket address = " + socketAddress);

                LocalSocketAddress addr = new LocalSocketAddress(socketAddress, LocalSocketAddress.Namespace.RESERVED);
                mSocket = new LocalSocket();
                Log.i(LOG_TAG, "Connecting to socket " + addr.getName() + "...");
                mSocket.connect(addr);
                Log.d(LOG_TAG, "Connected to socket " + addr.getName());
                mOS = mSocket.getOutputStream();
                mIS = mSocket.getInputStream();
                return true;
            } catch (IOException e) {
                Log.e(LOG_TAG, "Socket connection failed");
                mSocket = null;
                e.printStackTrace();
                try {
                    Thread.sleep(SOCKET_FAILED_SLEEP_TIME);
                } catch (Exception sleepExp) {
                    Log.e(LOG_TAG, "thread sleep failed");
                }
            }
            counter++;
        }
        return false;
    }

    private void resetSocket() {
        Log.i(LOG_TAG, "resetSocket()");
        if (mSocket != null) {
            try {
                mSocket.shutdownInput();
                mSocket.shutdownOutput();
                mSocket.close();
            } catch (IOException e) {
                Log.e(LOG_TAG, "resetSocket() - failed!");
                e.printStackTrace();
            }
            mSocket = null;
            mIS = null;
            mOS = null;
        } else {
            Log.i(LOG_TAG, "resetSocket() - socket is not initialized");
        }
    }

    @Override
    public void handleMessage(Message msg) {
        RadioConfigSyncRequest reqMsgObj = (RadioConfigSyncRequest) msg.obj;

        Log.i(LOG_TAG, "handleMessage() - token: " + reqMsgObj.getToken());
        send(reqMsgObj);
    }

    private void send(RadioConfigSyncRequest reqMsgObj) {
        if (mSocket == null) {
            Log.e(LOG_TAG, "send() - mSocket is null!");
            reqMsgObj.notifyResp();
            return;
        }
        byte[] bytes = reqMsgObj.getReqBytes();
        Log.i(LOG_TAG, "handleMessage() - Message length: " + bytes.length);
        try {
            byte[] toSend = new byte[bytes.length + 4];

            // length in big endian
            toSend[0] = toSend[1] = 0;
            toSend[2] = (byte)((bytes.length >> 8) & 0xff);
            toSend[3] = (byte)(bytes.length & 0xff);

            System.arraycopy(bytes, 0, toSend, 4, bytes.length);

            mOS.write(toSend);
            Log.i(LOG_TAG, "Socket Send successful!!");
        } catch (IOException e) {
            Log.e(LOG_TAG, "send() - write failed !!!");
            reqMsgObj.notifyResp();
            e.printStackTrace();
        }
    }

    public void run(){
        Log.i(LOG_TAG, "run()");
        boolean toConnectSocket = true;
        while (!mToDestroy) {
            if (toConnectSocket && !connectSocket()) {
                Log.e(LOG_TAG, "run() - connect socket failed.");
                return;
            }

            toConnectSocket = false;

            int bytesRead = 0;
            int posOffset = 0;
            int bytesToRead = 4;

            try {
                do {
                    bytesRead = mIS.read(mBuffer, posOffset, bytesToRead);
                    if (bytesRead < 0) {
                        Log.e(LOG_TAG, "run() - bytesRead < 0 when reading message length");
                        break;
                    }
                    posOffset += bytesRead;
                    bytesToRead -= bytesRead;
                } while (bytesToRead > 0);
            } catch (IOException e) {
                Log.e(LOG_TAG, "Exception in reading socket");
                e.printStackTrace();
                break;
            }

            if (bytesRead < 0) {
                toConnectSocket = true;
                continue;
            }

            posOffset = 0;
            bytesToRead = ((mBuffer[0] & 0xff) << 24)
                          | ((mBuffer[1] & 0xff) << 16)
                          | ((mBuffer[2] & 0xff) << 8)
                          | (mBuffer[3] & 0xff);
            Log.i(LOG_TAG, "run() - Message size is: " + bytesToRead);
            mBufferLength = bytesToRead;

            try {
                do {
                    bytesRead = mIS.read(mBuffer, posOffset, bytesToRead);
                    if (bytesRead < 0) {
                        Log.e(LOG_TAG, "run() - bytesRead < 0 when reading message");
                        break;
                    }
                    posOffset += bytesRead;
                    bytesToRead -= bytesRead;
                } while (bytesToRead > 0);
            } catch (IOException e) {
                Log.e(LOG_TAG, "Exception in reading socket");
                e.printStackTrace();
                break;
            }

            if (bytesRead < 0) {
                toConnectSocket = true;
                continue;
            }

            handleRecvBytes();
        }
    }

    private void handleRecvBytes() {
        Log.i(LOG_TAG, "handleRecvBytes()");

        if (mRecvHdlr == null) {
            return;
        }

        Message msg;
        byte[] bytes = Arrays.copyOf(mBuffer, mBufferLength);

        msg = mRecvHdlr.obtainMessage(RadioConfigService.EVENT_RESP, bytes);
        msg.sendToTarget();
    }
}
