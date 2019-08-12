/*=============================================================
@file AR.java

@brief
Java Native interface to enable the AR_jni Activity Recognition.

@attention
It is JNI inerface file for Activity Recognition.

Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================*/
package com.qualcomm.qti.sensors.core.sensortest;

import android.app.Activity;
import android.app.Fragment;
import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;
import android.widget.Button;
import android.util.Log;
import android.view.View.OnClickListener;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.Toast;
import com.qualcomm.qti.sensors.qsensortest.R;

public class AR extends Fragment{

    private Color c = new Color();
    public static final String TAG = "ArActivity";
    public static final int EVENT_TYPE_FLUSH_COMPLETE = 0;
    public static final int EVENT_TYPE_ENTER = 1;
    public static final int EVENT_TYPE_EXIT = 2;
    public static int MAX_NO_ACTIVITIES = 6;
    public String []activityNames = {"IN_VEHICLE :",
                                      "ON_BICYCLE :",
                                      "WALKING    :",
                                      "RUNNING    :",
                                      "STILL           :",
                                      "TILTING       :"}; // for UI alignment
    public String EventAry[] = { "","ENTER,","EXIT,    "}; //for UI alignment
    private TextView textCurActivity ,textHisActivity;
    private View view;
    private Button buttonEnable,buttonDisable;
    public static boolean initFlag = true;
    String completeData = new String();  // Activity history managed by one string.
    String values = new String(); // current activity data
    /**
    * Called when the activity is first created.
    */
    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }
    @Override
    public View onCreateView (LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        if (null == this.view) {
            this.view = inflater.inflate(R.layout.activity_attributes_layout, null);
            this.view = inflater.inflate(R.layout.activity_ar_layout, null);
            textCurActivity = (TextView) view.findViewById(R.id.text_curActivity); // current activity data display text
            textHisActivity = (TextView) view.findViewById(R.id.text_hisActivity); // the compleate activity data display text
            buttonEnable = (Button)view.findViewById(R.id.button1);
            buttonDisable = (Button)view.findViewById(R.id.button2);
            buttonEnable.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                Toast.makeText(getActivity(),"Enabled Successfully",Toast.LENGTH_SHORT).show();
                for (int i = 0; i < MAX_NO_ACTIVITIES; i++) {
                    Log.d(TAG, " Enter enableActivityEvent "+enableActivityEvent(i, EVENT_TYPE_ENTER, 0));
                    Log.d(TAG, " Exit enableActivityEvent "+enableActivityEvent(i, EVENT_TYPE_EXIT, 0));
                }
                values = " AR Enabled ";
                textCurActivity.setText(values);
                completeData = " "; // erase complete data.
                textHisActivity.setText(completeData);
            }
            });
            buttonDisable.setOnClickListener(new OnClickListener(){
            @Override
            public void onClick(View view) {
                Toast.makeText(getActivity(),"Disabled Successfully",Toast.LENGTH_SHORT).show();
                for (int i = 0; i < MAX_NO_ACTIVITIES; i++) {
                    Log.d(TAG, " Enter disableActivityEvent "+disableActivityEvent(i, EVENT_TYPE_ENTER));
                    Log.d(TAG, " Exit disableActivityEvent "+disableActivityEvent(i, EVENT_TYPE_EXIT));
                }
                values = " AR Disabled";
                textCurActivity.setText(values);
            }
            });

        }
        textCurActivity.setText(values);
        textHisActivity.setText(completeData);
        // Initialize only once.
        if (initFlag == true) {
            nativeClassInit();
            nativeInitialize();
            initFlag = false;
        }
        return this.view;
    }

    // native bindings
    static {
        System.loadLibrary("AR_jni");
    }
    public int enableActivityEvent(int activityType, int eventType, long reportLatenceNs) {
        return nativeEnableActivityEvent(activityType, eventType, reportLatenceNs);
    }
     public int disableActivityEvent(int activityType, int eventType) {
        return nativeDisableActivityEvent(activityType, eventType);
    }
    public void arCallback(int eventType,int activity,long timeStamp) {
         values = activityNames[activity] + "    "+EventAry[eventType]+ "      TS(ns) = "+timeStamp +"\n";
         completeData = values +completeData;
         getActivity().runOnUiThread(new Runnable() {
             @Override
             public void run() {
                  textCurActivity.setTextColor(c.RED);
                  textCurActivity.setText(values);
                  if (completeData != null) {
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
