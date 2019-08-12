/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample.onclicklistener;

import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Toast;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import com.qualcomm.qti.auth.fidocryptosample.FidoCryptoClientActivity;

public class VerifyUserButtonOnClickListener implements OnClickListener {
  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  private byte[] Sha256(byte[] message) {
    byte[] out = null;
    MessageDigest digest = null;
    try {
      digest = MessageDigest.getInstance("SHA-256");
      if (message != null) {
        digest.update(message);
      }
      out = digest.digest();
    } catch (NoSuchAlgorithmException e1) {
      Log.e(FidoCryptoClientActivity.LOG_TAG, "Error in Sha256");
    }
    return out;
  }

  public VerifyUserButtonOnClickListener(
      FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  @Override
  public void onClick(View v) {
    Log.d(FidoCryptoClientActivity.LOG_TAG,
        VerifyUserButtonOnClickListener.class.getSimpleName() + ".onClick()");
    int authIndex = fidoCryptoClientActivity.spinner.getSelectedItemPosition();

    switch(authIndex){
      case 0:
        if (fidoCryptoClientActivity.maConnection.iMockAuthServiceConnection == null) {
          Toast.makeText(v.getContext(), "MockAuthService not connected",
              Toast.LENGTH_SHORT).show();
          Log.d(FidoCryptoClientActivity.LOG_TAG, "service not connected");
          return;
        }
        testGiveAuthToken(v);
        testVerifyUser(v);
        break;
      case 1:
        if (fidoCryptoClientActivity.saConnection.iSampleAuthenticatorServiceConnection == null) {
          Toast.makeText(v.getContext(), "SampleAuthenticatorService not connected",
              Toast.LENGTH_SHORT).show();
          Log.d(FidoCryptoClientActivity.LOG_TAG, "service not connected");
          return;
        }
        testVerifyUserSA(v);
        break;
      case 2:
        if (fidoCryptoClientActivity.ssaConnection.iSecureSampleAuthServiceConnection == null) {
          Toast.makeText(v.getContext(), "SecureSampleAuthService not connected",
              Toast.LENGTH_SHORT).show();
          Log.d(FidoCryptoClientActivity.LOG_TAG, "service not connected");
          return;
        }
        testVerifyUserSSA(v);
        break;
    }
  }

  private void testVerifyUser(View v) {
    try {
      byte[] nonce = Sha256(FidoCryptoClientActivity.finalChallenge);
      String appId = "fidocrypto";
      Log.d(FidoCryptoClientActivity.LOG_TAG, "Sending verify user request to "
          + appId);
      fidoCryptoClientActivity.maConnection.iMockAuthServiceConnection
          .verifyUser(nonce, appId, fidoCryptoClientActivity.iMockAuthListener);
      Toast.makeText(v.getContext(), "Verifying User", Toast.LENGTH_SHORT)
          .show();
    } catch (RemoteException e) {
      e.printStackTrace();
      Toast.makeText(v.getContext(),
          e.getClass().getSimpleName() + ":" + e.getMessage(),
          Toast.LENGTH_SHORT).show();
    }
  }

  private void testGiveAuthToken(View v) {
    try {
      int mVersionMajor = 1;
      int mVersionMinor = 0;
      int mResult = 0;
      long mUserId = 12345;
      long mUptime = 0;
      byte[] mExtData = { 0x03, 0x01, 0x04, 0x00, 0x01, 0x02, 0x03, 0x04 };
      long mUserEntityId = 987654321;

      Log.d(FidoCryptoClientActivity.LOG_TAG, "Sample token to be set");
      fidoCryptoClientActivity.maConnection.iMockAuthServiceConnection
          .giveAuthToken(mVersionMajor, mVersionMinor, mResult, mUserId,
              mUptime, mExtData, mUserEntityId);
      Toast.makeText(v.getContext(), "token given", Toast.LENGTH_SHORT).show();
    } catch (RemoteException e) {
      e.printStackTrace();
      Toast.makeText(v.getContext(),
          e.getClass().getSimpleName() + ":" + e.getMessage(),
          Toast.LENGTH_SHORT).show();
    }
  }

  private void testVerifyUserSA(View v) {
    try {
      byte[] nonce = Sha256(FidoCryptoClientActivity.finalChallenge);
      String appId = "fidocrypto";
      Log.d(FidoCryptoClientActivity.LOG_TAG,
          "Sending verify user request to " + appId);
      fidoCryptoClientActivity.saConnection.iSampleAuthenticatorServiceConnection.verifyUser(
          nonce, appId, fidoCryptoClientActivity.iSampleAuthenticatorListener);
      Toast.makeText(v.getContext(), "Verifying User", Toast.LENGTH_SHORT).show();
    } catch (RemoteException e) {
      e.printStackTrace();
      Toast.makeText(v.getContext(),
          e.getClass().getSimpleName() + ":" + e.getMessage(),
          Toast.LENGTH_SHORT).show();
    }
  }

  private void testVerifyUserSSA(View v) {
    try {
      byte[] nonce = Sha256(FidoCryptoClientActivity.finalChallenge);
      String appId = "fidocrypto";
      Log.d(FidoCryptoClientActivity.LOG_TAG,
          "Sending verify user request to " + appId);
      fidoCryptoClientActivity.ssaConnection.iSecureSampleAuthServiceConnection.verifyUser(
          nonce, appId, fidoCryptoClientActivity.iSecureSampleAuthListener);
      Toast.makeText(v.getContext(), "Verifying User", Toast.LENGTH_SHORT).show();
    } catch (RemoteException e) {
      e.printStackTrace();
      Toast.makeText(v.getContext(),
          e.getClass().getSimpleName() + ":" + e.getMessage(),
          Toast.LENGTH_SHORT).show();
    }
  }
}
