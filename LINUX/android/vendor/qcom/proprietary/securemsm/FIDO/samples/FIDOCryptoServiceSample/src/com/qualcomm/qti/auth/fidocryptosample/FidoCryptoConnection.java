/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample;

import com.qualcomm.qti.auth.IFidoCrypto;
import com.qualcomm.qti.auth.IFidoCryptoListener;

import android.content.ComponentName;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import android.widget.Toast;

public class FidoCryptoConnection implements ServiceConnection {

  private final FidoCryptoClientActivity fidoCryptoClientActivity;
  private IFidoCrypto fidoCryptoService;

  public FidoCryptoConnection(FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  public boolean isBound() {
    return fidoCryptoService != null;
  }

  @Override
  public void onServiceConnected(ComponentName name, IBinder service) {
    Log.d(FidoCryptoClientActivity.LOG_TAG, FidoCryptoConnection.class.getSimpleName()
        + "onServiceConnected()" + name.toString());
    if (service == null) {
      Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
          "FidoCryptoService connection failed", Toast.LENGTH_SHORT).show();
      return;
    }
    fidoCryptoService = IFidoCrypto.Stub.asInterface(service);
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
        "FidoCryptoService connected", Toast.LENGTH_SHORT).show();
  }

  @Override
  public void onServiceDisconnected(ComponentName className) {
    Log.d(FidoCryptoClientActivity.LOG_TAG, FidoCryptoConnection.class.getSimpleName()
        + "onServiceDisconnected()" + className.toString());
    fidoCryptoService = null;
    Toast.makeText(fidoCryptoClientActivity.getBaseContext(),
        "FidoCryptoService disconnected", Toast.LENGTH_SHORT).show();
  }

  public void unbind() {
    fidoCryptoService = null;
  }

  public int process(byte[] command, String appId, byte[] cookie,
      IFidoCryptoListener iFidoCryptoListener) throws RemoteException {
    return fidoCryptoService.process(command, appId, cookie, iFidoCryptoListener);
  }

  public int setChallenge(byte[] finalChallenge, IFidoCryptoListener iFidoCryptoListener) throws RemoteException {
    return fidoCryptoService.setChallenge(finalChallenge, iFidoCryptoListener);
  }

}
