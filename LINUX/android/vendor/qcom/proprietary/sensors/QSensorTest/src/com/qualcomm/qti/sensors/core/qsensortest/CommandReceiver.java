/*============================================================================
@file CommandReceiver.java

@brief
Receives and processes broadcast commands.

Copyright (c) 2012-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.core.qsensortest;

import java.util.List;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.Sensor;
import android.util.Log;
import android.widget.Toast;

import com.qualcomm.qti.sensors.core.stream.SensorAdapter;
import com.qualcomm.qti.sensors.core.threshold.ThresholdSensorAdapter;
import com.qualcomm.qti.sensors.core.sensortest.AR;

public class CommandReceiver extends BroadcastReceiver {
  public CommandReceiver() {}

  private static final String TAG = QSensorContext.TAG;

  @Override
  public void onReceive(Context context, Intent intent) {
    if(intent.getAction().contentEquals("com.qualcomm.qti.sensors.qsensortest.intent.STREAM")) {
      CharSequence sensorName = intent.getCharSequenceExtra("sensorname");
      int sensorType = intent.getIntExtra("sensortype", 0);
      int rate = intent.getIntExtra("rate", 60);
      int batchRate = intent.getIntExtra("batch", -1);
      boolean isSensorFound = false;

      Log.i(TAG, "received stream intent with parameters: (sensorname: " + sensorName + ", sensortype: " + sensorType + ", rate: " + rate + " batch: " + batchRate + ")");

      List<SensorAdapter> sensorAdapters = QSensorContext.sensorAdapterList();
      for(SensorAdapter adapter : sensorAdapters) {
        Sensor sensor = adapter.sensor();
        if(sensor.getName().contentEquals(sensorName) &&
           sensor.getType() == sensorType &&
           SettingsDatabase.getSettings().getSensorSetting(sensor).getEnableStreaming()) {
          isSensorFound = true;
          Log.d(QSensorContext.TAG, "CR setting stream rate for " + sensorName + ": " + rate + ", batch:" + batchRate);
          adapter.streamRateIs(rate, batchRate, true);
          break;
        }
      }
      if (isSensorFound == false) {
          Log.e(QSensorContext.TAG, "improper command for STREAM, Mismatch of sensorname or sensortype , please check name and type in available sensor list ");
      }
    }
    if(intent.getAction().contentEquals("com.qualcomm.qti.sensors.qsensortest.intent.THRESH")) {
      CharSequence sensorName = intent.getCharSequenceExtra("sensorname");
      int sensorType = intent.getIntExtra("sensortype", 0);
      int streamRate = intent.getIntExtra("rate", 0);
      int threshXValue = intent.getIntExtra("x", 0);
      int threshYValue = intent.getIntExtra("y", 0);
      int threshZValue = intent.getIntExtra("z", 0);

      Log.i(TAG, "received stream intent with parameters: (sensorname: " + sensorName + ", sensortype: " + sensorType + ", rate: " + streamRate + ", x: " + threshXValue + ", y: " + threshYValue + ", z: " + threshZValue + ")");

      List<ThresholdSensorAdapter> thresholdSensorAdapters = QSensorContext.thresholdSensorAdapterList();
      for(ThresholdSensorAdapter adapter : thresholdSensorAdapters) {
        Sensor sensor = adapter.sensor();
        if(sensor.getType() == sensorType) {
          Log.d(QSensorContext.TAG, "CR setting threshold for " + sensorType + ": " + streamRate +
                ", x:" + threshXValue + ", y:" + threshYValue + ", z:" + threshZValue);
          adapter.thresholdIs(streamRate, threshXValue, threshYValue, threshZValue);
          break;
        }
      }
    }
    if ( intent.getAction().contentEquals("com.qualcomm.qti.sensors.sensortest.intent.AR")) {
        AR ar = new AR();
        int  Enabled = intent.getIntExtra("Enabled", 0);
        Log.i(TAG,"Enabled = "+Enabled);
        if ( 1 == Enabled ) {
            Toast.makeText(context,"Enabled Successfully",Toast.LENGTH_SHORT).show();
            for (int i = 0; i < ar.MAX_NO_ACTIVITIES; i++) {
                ar.enableActivityEvent(i, ar.EVENT_TYPE_ENTER, 0);
                ar.enableActivityEvent(i, ar.EVENT_TYPE_EXIT, 0);
            }
        } else {
            Toast.makeText(context,"Disabled Successfully",Toast.LENGTH_SHORT).show();
            for (int i = 0; i < ar.MAX_NO_ACTIVITIES; i++) {
                ar.disableActivityEvent(i, ar.EVENT_TYPE_ENTER);
                ar.disableActivityEvent(i, ar.EVENT_TYPE_EXIT);
            }
        }
    }
    if (intent.getAction().contentEquals("com.qualcomm.qti.sensors.qsensortest.intent.SENSORLIST")) {
        List<SensorAdapter> sensorAdapters = QSensorContext.sensorAdapterList();
        for (SensorAdapter adapter : sensorAdapters) {
            Sensor sensor = adapter.sensor();
            Log.d(QSensorContext.TAG,"< "+sensor.getStringType()+" > " +" Sensor type:" + "\"" +sensor.getType() + "\"" + " SensorName:" + "\""+sensor.getName()+"\"" );
        }
    }
  }

  public IntentFilter getIntentFilter() {
    IntentFilter intentFilter = new IntentFilter();
    intentFilter.addAction("com.qualcomm.qti.sensors.qsensortest.intent.STREAM");
    Log.d(QSensorContext.TAG, "Available: com.qualcomm.qti.sensors.qsensortest.intent.STREAM");
    intentFilter.addAction("com.qualcomm.qti.sensors.qsensortest.intent.THRESH");
    Log.d(QSensorContext.TAG, "Available: com.qualcomm.qti.sensors.qsensortest.intent.THRESH");
    intentFilter.addAction("com.qualcomm.qti.sensors.sensortest.intent.AR");
    Log.d(QSensorContext.TAG, "Available: com.qualcomm.qti.sensors.sensortest.intent.AR");
    intentFilter.addAction("com.qualcomm.qti.sensors.qsensortest.intent.SENSORLIST");
    Log.d(QSensorContext.TAG,"Available: com.qualcomm.qti.sensors.qsensortest.intent.SENSORLIST");
    intentFilter.addCategory("com.qualcomm.qti.sensors.qsensortest.intent.category.DEFAULT");
    return intentFilter;
  }
}
