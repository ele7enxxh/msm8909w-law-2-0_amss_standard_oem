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

public class RegisterSampleCase extends AbstractSampleCase {
  private static final String LOG_TAG = RegisterSampleCase.class
      .getSimpleName();
  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public RegisterSampleCase(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  public void test(View v, byte authIndex) throws RemoteException {

    byte[] registerCommand = getRegisterCommand(authIndex);
    int res = fidoCryptoClientActivity.process(registerCommand, good_cookie);
    String resName = FidoCryptoResultCode.values()[res].name();
    Log.d(RegisterSampleCase.LOG_TAG, "res=" + resName);
    Toast.makeText(v.getContext(), resName, Toast.LENGTH_SHORT).show();
  }

  private byte[] getRegisterCommand(byte authIndex) {

    byte[] userVerifyToken = null;

    byte[] registerCommandFirstPart = new byte[] {
    /* 0x00 */getByte(TAG_UAFV1_REGISTER_CMD, 0),
    /* 0x01 */getByte(TAG_UAFV1_REGISTER_CMD, 1),
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
    /* 0x18 */getByte(TAG_USERNAME, 0),
    /* 0x19 */getByte(TAG_USERNAME, 1),
    /* 0x1A */0x01,
    /* 0x1B */0x00,
    /* 0x1C */0x1C,
    /* 0x1D */getByte(TAG_ATTESTATION_TYPE, 0),
    /* 0x1E */getByte(TAG_ATTESTATION_TYPE, 1),
    /* 0x1F */0x02,
    /* 0x20 */0x00,
    /* 0x21 */0x07,
    /* 0x22 */0x3E,
    /* 0x23 */getByte(TAG_KEYHANDLE_ACCESS_TOKEN, 0),
    /* 0x24 */getByte(TAG_KEYHANDLE_ACCESS_TOKEN, 1),
    /* 0x25 */0x03,
    /* 0x26 */0x00,
    /* 0x27 */0x27,
    /* 0x28 */0x28,
    /* 0x29 */0x29 };

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

    Log.d(
        FidoCryptoClientActivity.LOG_TAG,
        "user verify token "
            + String.format("%x %x %x %x", userVerifyToken[0],
                userVerifyToken[1], userVerifyToken[2], userVerifyToken[3]));
    Log.d(FidoCryptoClientActivity.LOG_TAG, "user verify token length "
        + String.format("%x", userVerifyToken.length));

    byte[] registerCommand = new byte[registerCommandFirstPart.length
        + userVerifyToken.length];
    System.arraycopy(registerCommandFirstPart, 0, registerCommand, 0,
        registerCommandFirstPart.length);
    System.arraycopy(userVerifyToken, 0, registerCommand,
        registerCommandFirstPart.length, userVerifyToken.length);
    registerCommand[2] = getByte(registerCommand.length - 4, 0);
    registerCommand[3] = getByte(registerCommand.length - 4, 1);

    Log.d(
        FidoCryptoClientActivity.LOG_TAG,
        "register command "
            + String.format("%x %x %x %x", registerCommand[0],
                registerCommand[1], registerCommand[2], registerCommand[3]));
    Log.d(
        FidoCryptoClientActivity.LOG_TAG,
        "register command length "
            + String.format("%x", registerCommand.length));

    return registerCommand;
  }
}
