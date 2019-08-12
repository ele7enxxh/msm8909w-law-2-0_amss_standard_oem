/*============================================================================
@file ThresholdDialog.java

@brief
Dialog pop-up window used to specify parameters and enable the threshold
algorithm.

Copyright (c) 2012-2013, 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.ui.threshold;

import android.app.Dialog;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.qualcomm.qti.sensors.qsensortest.R;
import com.qualcomm.qti.sensors.ui.qsensortest.TabControl;

public class ThresholdDialog extends Dialog {
  public ThresholdDialog(View.OnClickListener onClickListener) {
    super(TabControl.getContext());

    this.setContentView(R.layout.threshold_dialog);
    this.setTitle("Threshold Rate");

    String threshXValue = String.valueOf(32767);
    EditText threshXField = (EditText) this.findViewById(R.id.thresh_x_field);
    threshXField.setText(threshXValue);

    String threshYValue = String.valueOf(32767);
    EditText threshYField = (EditText) this.findViewById(R.id.thresh_y_field);
    threshYField.setText(threshYValue);

    String threshZValue = String.valueOf(32767);
    EditText threshZField = (EditText) this.findViewById(R.id.thresh_z_field);
    threshZField.setText(threshZValue);

    String currentRate = String.valueOf(10);
    EditText rateField = (EditText) this.findViewById(R.id.thresh_delay_field);
    rateField.setText(currentRate);

    Button dialogSubmitButton = (Button) this.findViewById(R.id.thresh_button_submit);
    dialogSubmitButton.setOnClickListener(onClickListener);

    Button dialogCancelButton = (Button) this.findViewById(R.id.thresh_button_cancel);
    dialogCancelButton.setOnClickListener(onClickListener);
  }
}