/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.auth.fidocryptosample.onclicklistener;

import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Toast;

import com.qualcomm.qti.auth.fidocryptosample.FidoCryptoClientActivity;

public class UnBindFidoCryptoButtonOnClickListener implements OnClickListener {
  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public UnBindFidoCryptoButtonOnClickListener(
      FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  @Override
  public void onClick(View v) {
    Log.d(FidoCryptoClientActivity.LOG_TAG,
        UnBindFidoCryptoButtonOnClickListener.class.getSimpleName()
            + ".onClick()");
    if (fidoCryptoClientActivity.fidoCryptoConnection.isBound()) {
      fidoCryptoClientActivity.unbindFidoCryptoService();
      Toast.makeText(v.getContext(), "unbinding fidocrypto service",
          Toast.LENGTH_SHORT).show();
    } else {
      Toast.makeText(v.getContext(), "fidocrypto service not bound",
          Toast.LENGTH_SHORT).show();
    }
  }

}
