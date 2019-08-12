/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample.samples;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import com.qualcomm.qti.auth.fidocryptosample.FidoCryptoClientActivity;
import com.qualcomm.qti.auth.fidocryptosample.FidoCryptoResultCode;
import com.qualcomm.qti.auth.fidocryptosample.persistence.UnableToReadKeyHandleException;

public class SignSampleCase extends AbstractSampleCase {
  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public SignSampleCase(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  private byte[] getSignCommand(byte[] keyHandle, byte authIndex) {
    byte[] userVerifyToken = null;
    byte[] signCommandFirstPart = new byte[] {
    /* 0x00 */getByte(TAG_UAFV1_SIGN_CMD, 0),
    /* 0x01 */getByte(TAG_UAFV1_SIGN_CMD, 1),
    /* 0x02 */0x00,
    /* 0x03 */0x00,
    /* 0x04 */getByte(TAG_AUTHENTICATOR_INDEX, 0),
    /* 0x05 */getByte(TAG_AUTHENTICATOR_INDEX, 1),
    /* 0x06 */0x01,
    /* 0x07 */0x00,
    /* 0x08 */authIndex,
    /* 0x09 */getByte(TAG_APPID, 0),
    /* 0x0A */getByte(TAG_APPID, 1),
    /* 0x0B */0x04,
    /* 0x0C */0x00,
    /* 0x0D */0x61,
    /* 0x0E */0x70,
    /* 0x0F */0x70,
    /* 0x10 */0x30,
    /* 0x11 */getByte(TAG_FINAL_CHALLENGE, 0),
    /* 0x12 */getByte(TAG_FINAL_CHALLENGE, 1),
    /* 0x13 */0x03,
    /* 0x14 */0x00,
    /* 0x15 */FidoCryptoClientActivity.finalChallenge[0],
    /* 0x16 */FidoCryptoClientActivity.finalChallenge[1],
    /* 0x17 */FidoCryptoClientActivity.finalChallenge[2],
    /* 0x18 */getByte(TAG_TRANSACTION_CONTENT, 0),
    /* 0x19 */getByte(TAG_TRANSACTION_CONTENT, 1),
    /* 0x1A */0x01,
    /* 0x1B */0x00,
    /* 0x1C */0x1C,
    /* 0x1D */getByte(TAG_KEYHANDLE_ACCESS_TOKEN, 0),
    /* 0x1E */getByte(TAG_KEYHANDLE_ACCESS_TOKEN, 1),
    /* 0x1F */0x03,
    /* 0x20 */0x00,
    /* 0x21 */0x27,
    /* 0x22 */0x28,
    /* 0x23 */0x29 };

    if (fidoCryptoClientActivity.encapsulatedResult != null) {
      ByteBuffer byteBuffer = ByteBuffer.allocate(fidoCryptoClientActivity.encapsulatedResult.length + 4);
      byteBuffer.order(ByteOrder.LITTLE_ENDIAN);
      byteBuffer.putShort(TAG_USERVERIFY_TOKEN);
      byteBuffer.putShort((short) (fidoCryptoClientActivity.encapsulatedResult.length));
      byteBuffer.put(fidoCryptoClientActivity.encapsulatedResult);
      userVerifyToken = byteBuffer.array();
    } else {
      ByteBuffer byteBuffer = ByteBuffer.allocate(4);
      byteBuffer.order(ByteOrder.LITTLE_ENDIAN);
      byteBuffer.putShort(TAG_USERVERIFY_TOKEN);
      byteBuffer.putShort((short) 0);
      userVerifyToken = byteBuffer.array();
    }

    ByteBuffer byteBuffer = ByteBuffer.allocate(userVerifyToken.length + keyHandle.length + 4 + 15);
    byteBuffer.order(ByteOrder.LITTLE_ENDIAN);
    byteBuffer.put(userVerifyToken);

    byteBuffer.putShort(TAG_TRANSACTION_CONFIRMATION);
    byteBuffer.putShort((short) 0x0B);
    byteBuffer.putShort((short) 0x28F5);
    byteBuffer.putShort((short) 0x01);
    byteBuffer.put((byte) 0x00);
    byteBuffer.putShort((short) 0x28F6);
    byteBuffer.putShort((short) 0x02);
    byteBuffer.putShort((short) 0x00);

    byteBuffer.putShort(TAG_KEYHANDLE);
    byteBuffer.putShort((short) keyHandle.length);
    byteBuffer.put(keyHandle);

    byte[] signCommandLastPart = byteBuffer.array();

    byte[] signCommand = new byte[signCommandFirstPart.length
        + signCommandLastPart.length];
    System.arraycopy(signCommandFirstPart, 0, signCommand, 0,
        signCommandFirstPart.length);
    System.arraycopy(signCommandLastPart, 0, signCommand,
        signCommandFirstPart.length, signCommandLastPart.length);
    signCommand[2] = getByte(signCommand.length - 4, 0);
    signCommand[3] = getByte(signCommand.length - 4, 1);
    return signCommand;

  }

  public void test(View v, byte authIndex) throws RemoteException {
    try {
      byte[] keyHandle = fidoCryptoClientActivity.persistenceManager
          .readKeyHandleFromFile();
      byte[] signCommand = getSignCommand(keyHandle, authIndex);
      int res = fidoCryptoClientActivity.process(signCommand, good_cookie);
      String resName = FidoCryptoResultCode.values()[res].name();
      Log.d(FidoCryptoClientActivity.LOG_TAG, "res=" + resName);
      Toast.makeText(v.getContext(), resName, Toast.LENGTH_SHORT).show();
    } catch (UnableToReadKeyHandleException e) {
      e.printStackTrace();
      Toast.makeText(v.getContext(), "Unable to read key handle",
          Toast.LENGTH_SHORT).show();
    }

  }
}
