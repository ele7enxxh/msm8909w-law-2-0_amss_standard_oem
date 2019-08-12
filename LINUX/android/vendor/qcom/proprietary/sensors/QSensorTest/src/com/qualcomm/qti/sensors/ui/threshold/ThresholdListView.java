/*============================================================================
@file ThresholdListView.java

@brief
View corresponding to the list of Sensors on the Threshold tab.

Copyright (c) 2012-2013, 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.ui.threshold;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.widget.LinearLayout;

import com.qualcomm.qti.sensors.core.stream.SensorAdapter;
import com.qualcomm.qti.sensors.core.threshold.ThresholdSensorAdapter;
import com.qualcomm.qti.sensors.ui.stream.SensorController;
import com.qualcomm.qti.sensors.ui.stream.SensorListView;
import com.qualcomm.qti.sensors.ui.stream.SensorView;
import com.qualcomm.qti.sensors.core.qsensortest.QSensorContext;
import com.qualcomm.qti.sensors.core.qsensortest.SettingsDatabase.SensorSetting;

import java.util.List;

public class ThresholdListView extends SensorListView {
  public ThresholdListView(Context context, SensorManager sensorManager) {
    super(context, sensorManager);
  }

  @Override
  protected List<? extends SensorAdapter> getSensorAdapterList() {
    return  QSensorContext.thresholdSensorAdapterList();
  }

  @Override
  protected SensorView createView(SensorSetting settings, SensorAdapter sensorAdapter) {
    ThresholdSensorView sensorView = null;

    if(settings.getEnableThresh()) {
      ThresholdSensorAdapter thresholdSensorAdapter = (ThresholdSensorAdapter) sensorAdapter;
      sensorView = new ThresholdSensorView(thresholdSensorAdapter, sensorManager);
    }

    return sensorView;
  }

  public class ThresholdSensorView extends SensorView {
    private ThresholdSensorAdapter sensorAdapter;
    public ThresholdSensorView(ThresholdSensorAdapter sensorAdapter, SensorManager sensorManager) {
      super(sensorAdapter, sensorManager);
      this.sensorAdapter = sensorAdapter;
      this.controller = initController(sensorManager);
      this.initView(sensorManager);

      this.reportRateView.setVisibility(LinearLayout.INVISIBLE);
    }

    @Override
    protected SensorController initController(SensorManager sensorManager) {
      if(null != this.sensorAdapter)
        return new ThresholdSensorController(sensorManager, this.sensorAdapter);
      else
        return null;
    }
  }
}