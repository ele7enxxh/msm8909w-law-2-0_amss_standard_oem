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

import com.qualcomm.qti.auth.fidocryptosample.FidoCryptoClientActivity;
import com.qualcomm.qti.auth.fidocryptosample.samples.DeregisterSampleCase;
import com.qualcomm.qti.auth.fidocryptosample.samples.GetInfoSampleCase;
import com.qualcomm.qti.auth.fidocryptosample.samples.MalformedSampleCase;
import com.qualcomm.qti.auth.fidocryptosample.samples.OpenSettingsSampleCase;
import com.qualcomm.qti.auth.fidocryptosample.samples.RegisterSampleCase;
import com.qualcomm.qti.auth.fidocryptosample.samples.SignSampleCase;

public class ProcessButtonOnClickListener implements OnClickListener {
  private final FidoCryptoClientActivity fidoCryptoClientActivity;

  public ProcessButtonOnClickListener(
      FidoCryptoClientActivity fidoCryptoClientActivity) {
    this.fidoCryptoClientActivity = fidoCryptoClientActivity;
  }

  @Override
  public void onClick(View v) {
    Log.d(FidoCryptoClientActivity.LOG_TAG,
        ProcessButtonOnClickListener.class.getSimpleName() + ".onClick()");
    if (!fidoCryptoClientActivity.fidoCryptoConnection.isBound()) {
      Toast.makeText(v.getContext(), "FidoCryptoService not connected",
          Toast.LENGTH_SHORT).show();
      Log.d(FidoCryptoClientActivity.LOG_TAG, "service not connected");
      return;
    }
    try {
      switch (fidoCryptoClientActivity.radioGroup.getCheckedRadioButtonId()) {
      case FidoCryptoClientActivity.GET_INFO_RADIO_BUTTON_ID: {
        new GetInfoSampleCase(fidoCryptoClientActivity).test(v);
        break;
      }
      case FidoCryptoClientActivity.REGISTER_RADIO_BUTTON_ID: {
        int authIndex = fidoCryptoClientActivity.spinner.getSelectedItemPosition();
        switch(authIndex){
          case 0:
            new RegisterSampleCase(fidoCryptoClientActivity).test(v, (byte)0x02);
            break;
          case 1:
            new RegisterSampleCase(fidoCryptoClientActivity).test(v, (byte)0x03);
            break;
          case 2:
            // Set the authIndex 0 to use the biolib API, to 4 to use IAuth
            //new RegisterSampleCase(fidoCryptoClientActivity).test(v, (byte)0x00);
            new RegisterSampleCase(fidoCryptoClientActivity).test(v, (byte)0x04);
            break;
        }
        break;
      }
      case FidoCryptoClientActivity.DEREGISTER_RADIO_BUTTON_ID: {
        new DeregisterSampleCase(fidoCryptoClientActivity).test(v);
        break;
      }
      case FidoCryptoClientActivity.OPEN_SETTINGS_RADIO_BUTTON_ID: {
        new OpenSettingsSampleCase(fidoCryptoClientActivity).test(v);
        break;
      }
      case FidoCryptoClientActivity.SIGN_RADIO_BUTTON_ID: {
        int authIndex = fidoCryptoClientActivity.spinner.getSelectedItemPosition();
        switch(authIndex){
          case 0:
            new SignSampleCase(fidoCryptoClientActivity).test(v, (byte)0x02);
            break;
          case 1:
            new SignSampleCase(fidoCryptoClientActivity).test(v, (byte)0x03);
            break;
          case 2:
            // Set the authIndex 0 to use the biolib API, to 4 to use IAuth
            //new SignSampleCase(fidoCryptoClientActivity).test(v, (byte)0x00);
            new SignSampleCase(fidoCryptoClientActivity).test(v, (byte)0x04);
            break;
        }
        break;
      }
      case FidoCryptoClientActivity.MALFORMED_REQUEST_RADIO_BUTTON_ID: {
        new MalformedSampleCase(fidoCryptoClientActivity).test(v);
        break;
      }
      case -1: {
        Toast.makeText(v.getContext(), "check a function", Toast.LENGTH_SHORT)
            .show();
      }
      default: {
        Toast
            .makeText(
                v.getContext(),
                "radio group error. id "
                    + fidoCryptoClientActivity.radioGroup
                        .getCheckedRadioButtonId(), Toast.LENGTH_SHORT).show();
      }
      }
    } catch (RemoteException e) {
      fidoCryptoClientActivity.fidoCryptoConnection.unbind();
      e.printStackTrace();
      Toast.makeText(v.getContext(),
          e.getClass().getSimpleName() + ":" + e.getMessage(),
          Toast.LENGTH_SHORT).show();
    }
  }

}
