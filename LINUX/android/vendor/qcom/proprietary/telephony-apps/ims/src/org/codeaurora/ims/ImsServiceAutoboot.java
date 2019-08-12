/*
 * Copyright (c) 2015, 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package org.codeaurora.ims;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningServiceInfo;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.os.SystemProperties;
import android.os.UserHandle;

import com.qualcomm.ims.utils.Log;

public class ImsServiceAutoboot extends BroadcastReceiver {

    private final static String ACTION_SIM_STATE_CHANGED =
                              "android.intent.action.SIM_STATE_CHANGED";
    private final static String mClassName = ImsService.class.getName();

    @Override
    public void onReceive(Context context, Intent intent) {
        if (UserHandle.myUserId() != UserHandle.USER_OWNER) {
            Log.d(this, "Not primary user, ignore broadcast intents");
            return;
        }
        String intentAction = intent.getAction();
        if (Intent.ACTION_BOOT_COMPLETED.equals(intentAction) ||
            Intent.ACTION_LOCKED_BOOT_COMPLETED.equals(intentAction) ||
            ACTION_SIM_STATE_CHANGED.equals(intentAction)) {
            if (!isServiceRunning(context)) {
                Log.i(this, "Starting " + mClassName + " : " + intentAction + " received. ");
                startService(context);
            } else {
                Log.i(this, mClassName + " is already running. " +
                           intentAction + " ignored. ");
            }
        } else {
            Log.e(this, "Received Intent: " + intent.toString());
        }
    }

    private void startService(Context context) {
        ComponentName comp = new ComponentName(context.getPackageName(), mClassName);
        ComponentName service = context.startService(new Intent().setComponent(comp));
        if (service == null) {
            Log.e(this, "Could Not Start Service " + comp.toString());
        } else {
            Log.e(this, mClassName + " service Started Successfully");
        }
    }

    private boolean isServiceRunning(Context context) {
        ActivityManager manager =
              (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        for (RunningServiceInfo service : manager.getRunningServices(Integer.MAX_VALUE)) {
            if (mClassName.equals(service.service.getClassName())) {
                return true;
            }
        }
        return false;
    }
}
