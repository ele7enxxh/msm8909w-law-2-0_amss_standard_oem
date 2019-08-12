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
import com.qualcomm.qti.auth.fidocryptosample.ServiceAlreadyBoundException;

public class BindAuthButtonOnClickListener implements OnClickListener {
  private final FidoCryptoClientActivity parent;

  public BindAuthButtonOnClickListener(FidoCryptoClientActivity parent) {
    this.parent = parent;
  }

  @Override
  public void onClick(View v) {
    Log.d(FidoCryptoClientActivity.LOG_TAG,
        BindAuthButtonOnClickListener.class.getSimpleName() + ".onClick()");
    int authIndex = parent.spinner.getSelectedItemPosition();
    switch(authIndex){
      case 0:
        try {
          parent.bindMAService();
          Toast.makeText(v.getContext(), "binding MA service ...",
              Toast.LENGTH_SHORT).show();
        } catch (ServiceAlreadyBoundException e) {
          Toast.makeText(v.getContext(), "MA service already bound",
              Toast.LENGTH_SHORT).show();
        }
        break;
      case 1:
        try {
          parent.bindSAService();
          Toast.makeText(v.getContext(), "binding SA service ...", Toast.LENGTH_SHORT)
              .show();
        } catch (ServiceAlreadyBoundException e) {
          Toast.makeText(v.getContext(), "SA service already bound",
              Toast.LENGTH_SHORT).show();
        }
        break;
      case 2:
        try {
          parent.bindSSAService();
          Toast.makeText(v.getContext(), "binding SSA service ...", Toast.LENGTH_SHORT)
              .show();
        } catch (ServiceAlreadyBoundException e) {
          Toast.makeText(v.getContext(), "SSA service already bound",
              Toast.LENGTH_SHORT).show();
        }
        break;
    }
  }

}
