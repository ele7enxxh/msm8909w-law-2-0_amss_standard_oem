/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Not a Contribution, Apache license notifications and
  license are retained for attribution purposes only.
=============================================================================*/

/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.location.izatserviceprovider.service;

import com.qualcomm.location.osagent.OsAgent;
import android.app.Service;
import android.content.Intent;
import android.content.Context;
import android.os.IBinder;
import android.os.UserHandle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.os.Looper;
import android.util.Log;
import android.content.pm.PackageManager;
import android.content.pm.PackageInfo;
import java.util.HashSet;
import java.util.List;
import com.qualcomm.location.LBSSystemMonitorService;
import com.qualcomm.location.LocationService;
import com.qualcomm.location.osnp.GNPProxy;
import com.qualcomm.location.izat.IzatService;

public class IzatServiceBase extends Service implements Handler.Callback {
    // private members
    private static final String TAG = "IzatServiceBase";
    private static final int MSG_INIT_OSSUBSCRIPTION = 100;
    private static final int MSG_INIT_FRAMEWORK = 101;
    private static final int MSG_INIT_GNPPROXY = 102;
    private static final boolean VERBOSE_DBG = Log.isLoggable(TAG, Log.VERBOSE);

    private static boolean mInitilized = false;
    private static Context mAppContext;
    private static OsAgent mOSAgent = null;
    private static GNPProxy mGNPProxy = null;
    private static HandlerThread mFrameworkThread;
    private static Handler mMsgHandler;

    @Override
    public void onCreate() {
        super.onCreate();
        if (VERBOSE_DBG) Log.v (TAG, "onCreate()");

        if (!mInitilized) {
            startAdditionalServices(getApplicationContext());
            initInstance(getApplicationContext(), this);
        }
    }

    private static void initInstance(Context ctx, Handler.Callback cb) {
        if (VERBOSE_DBG) Log.v(TAG, "initInstance()");

        // cache the Application context
        mAppContext = ctx;

        // create the framework thread
        mFrameworkThread = new HandlerThread(TAG);
        mFrameworkThread.start();
        if (VERBOSE_DBG)
            Log.v
            (
                TAG,
                "FrameworkThread ID : " + mFrameworkThread.getId () + " created"
            );
        mMsgHandler = new Handler(mFrameworkThread.getLooper(), cb);

        // init messages to the handler
        mMsgHandler.sendEmptyMessage(MSG_INIT_OSSUBSCRIPTION);
        mMsgHandler.sendEmptyMessage(MSG_INIT_GNPPROXY);
        mMsgHandler.sendEmptyMessage(MSG_INIT_FRAMEWORK);

        mInitilized = true;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (VERBOSE_DBG) Log.v (TAG, "onDestroy()");
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public boolean onUnbind(Intent intent) {
        return true;
    }

    @Override
    public boolean handleMessage(Message msg) {
        int msgID = msg.what;
        if (VERBOSE_DBG) Log.v(TAG, "handleMessage what - " + msgID);

        switch(msgID)
        {
          case MSG_INIT_OSSUBSCRIPTION:
              handleOSSubscriptionInit(Looper.myLooper());
          break;
          case MSG_INIT_FRAMEWORK:
              handleFrameworkInit();
          break;
          case MSG_INIT_GNPPROXY:
              handleGNPProxyInit();
          break;
        }

        return true;
    }

    private void handleOSSubscriptionInit(Looper looperObj) {
        if (VERBOSE_DBG) Log.v(TAG, "handleOSSubscriptionInit()");
        mOSAgent = new OsAgent(mAppContext, looperObj);
    }

    private void handleFrameworkInit() {
        if (VERBOSE_DBG) Log.v(TAG, "handleFrameworkInit");
        native_framework_init();
    }

    private void handleGNPProxyInit() {
        Log.v(TAG, "handleGNPProxyInit");
        mGNPProxy = GNPProxy.createInstance(mAppContext);
    }

    private void handleFrameworkDeinit() {
        if (VERBOSE_DBG) Log.v(TAG, "handleFrameworkDeinit");
        native_framework_deinit();
    }

    private void startAdditionalServices(Context context) {
        if (VERBOSE_DBG) Log.v(TAG, "+startAdditionalServices");
        Intent i = new Intent(context, LBSSystemMonitorService.class);
        context.startServiceAsUser(i, UserHandle.OWNER);

        Intent intentLocationService = new Intent(context, LocationService.class);
        intentLocationService.setAction("com.qualcomm.location.LocationService");
        context.startServiceAsUser(intentLocationService, UserHandle.OWNER);

        Intent intentIzatService = new Intent(context, IzatService.class);
        intentIzatService.setAction("com.qualcomm.location.izat.IzatService");
        context.startServiceAsUser(intentIzatService, UserHandle.OWNER);

        Intent j = new Intent();
        j.setPackage("com.qualcomm.location.XT");
        j.setAction("com.android.location.XT.IXTSrv");
        context.startServiceAsUser(j, UserHandle.OWNER);

        Intent alarmServicesIntent = new Intent();
        alarmServicesIntent.setAction("com.qualcomm.services.location.LocationAlarmService");
        alarmServicesIntent.setPackage("com.qualcomm.services.location");
        context.startServiceAsUser(alarmServicesIntent, UserHandle.OWNER);

        PackageManager pm = context.getPackageManager();
        List<PackageInfo> packages = pm.getInstalledPackages(0);
        for (PackageInfo packInfo : packages) {
            if (packInfo.packageName.equals("com.example.ppkservicetest")) {
                Intent bootLocationTriggerIntent = new Intent();
                bootLocationTriggerIntent.setPackage("com.example.ppkservicetest");
                bootLocationTriggerIntent.setAction("com.example.ppkservicetest.ppkService");
                context.startServiceAsUser(bootLocationTriggerIntent, UserHandle.OWNER);
                break;
            }
        }

        if (VERBOSE_DBG) Log.v(TAG, "-startAdditionalServices exiting");
    }

    protected Looper getLooper() {
        return mFrameworkThread.getLooper();
    }

    private static native void native_framework_class_init();
    private static native void native_framework_init();
    private static native void native_framework_deinit();

    static {
        System.loadLibrary("locationservice_glue");
        native_framework_class_init();
    }
}

