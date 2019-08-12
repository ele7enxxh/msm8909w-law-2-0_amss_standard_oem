/*============================================================================
@file QArSensor.java

@brief
Manages all sensor data and current listener parameters.

Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.ui.artest;

import android.app.Activity;
import android.os.Bundle;
import android.content.Intent;
import android.widget.TextView;
import android.widget.Button;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.EditText;
import android.widget.Toast;
import android.content.Context;
import android.util.Log;
import android.text.method.ScrollingMovementMethod;

import com.qualcomm.qti.sensors.ui.artest.R;

public class QArSensor extends Activity {
    public static final String TAG = "ArActivity";
    public static final int EVENT_TYPE_FLUSH_COMPLETE = 0;
    public static final int EVENT_TYPE_ENTER = 1;
    public static final int EVENT_TYPE_EXIT = 2;
    public static int MAX_NO_ACTIVITIES = 10;

    // for UI alignment spaces are adjusted
    public String []activityNames = { "IN_VEHICLE",
                                      "ON_BICYCLE",
                                      "WALKING    ",
                                      "RUNNING    ",
                                      "STILL           ",
                                      "HIKING      ",
                                      "INACTIVE    ",
                                      "WALKRATE  ",
                                      "JOGSRATE  ",
                                      "RUNSRATE  "};
    public String EventAry[] = { "","<ENTR>","<EXIT  >"}; //for UI alignment
    private Color c = new Color();
    private TextView textCurActivity ,textHisActivity;
    private View view;
    private Button buttonEnable,buttonDisable;
    public static boolean initFlag = true;
    String completeData = new String();  // Activity history managed by one string.
    String values = new String(); // current activity data

    public int enableActivityEvent(int activityType, int eventType, long reportLatenceNs) {
        return nativeEnableActivityEvent(activityType, eventType, reportLatenceNs);
    }
    public int disableActivityEvent(int activityType, int eventType) {
        return nativeDisableActivityEvent(activityType, eventType);
    }

    private View.OnClickListener mArButtonListener = new View.OnClickListener() {
        public void onClick(View v) {
            switch (v.getId()) {
              case (R.id.button1) :
                 Log.e(TAG, "Enable Activities");
                 for (int i = 0; i < MAX_NO_ACTIVITIES; i++) {
                        enableActivityEvent(i,EVENT_TYPE_ENTER,0);
                        enableActivityEvent(i,EVENT_TYPE_EXIT,0);
                    }
              break;
              case (R.id.button2) :
                 Log.e(TAG, "Disable Activities");
                 for (int i = 0; i < MAX_NO_ACTIVITIES; i++) {
                        disableActivityEvent(i,EVENT_TYPE_ENTER);
                        disableActivityEvent(i,EVENT_TYPE_EXIT);
                 }
              break;
              default:
                 Log.e(TAG, "Should not be here");
            }
        }
    };
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.e(TAG, "AR TEST on Create called");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ar_layout);
        buttonEnable = (Button)this.findViewById(R.id.button1);
        buttonDisable = (Button)this.findViewById(R.id.button2);
        textCurActivity = (TextView) this.findViewById(R.id.text_curActivity);
        textHisActivity = (TextView) this.findViewById(R.id.text_hisActivity);

        /*Handle enable button*/
        buttonEnable.setOnClickListener(mArButtonListener);
        /*Handle disable button*/
        buttonDisable.setOnClickListener(mArButtonListener);

        if (initFlag == true) {
            nativeClassInit();
            nativeInitialize();
            initFlag = false;
        }
   }

    // native bindings
    static {
        System.loadLibrary("arwear_jni");
    }
    public void arCallback(int eventType,int activity,long timeStamp) {
        completeData = values + completeData;
        values = activityNames[activity] + ":"+EventAry[eventType]+ "TS="+timeStamp+"\n";
         Log.e(TAG, "ACTIVITY: " + completeData);
         QArSensor.this.runOnUiThread(new Runnable() {
             @Override
             public void run() {
                  textCurActivity.setTextColor(c.RED);
                  textCurActivity.setText(values);
                  if (completeData != null) {
                      textHisActivity.setTextColor(c.WHITE);
                      textHisActivity.setMovementMethod(ScrollingMovementMethod.getInstance());
                      textHisActivity.setText(completeData);
                  } else {
                      textHisActivity.setText(" ");
                  }
              }
         });
    }

    public static native void nativeClassInit();
    public native void nativeInitialize();
    public native int nativeEnableActivityEvent(
            int activityType,
            int eventType,
            long reportLatenceNs);
    public native int nativeDisableActivityEvent(int activityType, int eventType);
}
