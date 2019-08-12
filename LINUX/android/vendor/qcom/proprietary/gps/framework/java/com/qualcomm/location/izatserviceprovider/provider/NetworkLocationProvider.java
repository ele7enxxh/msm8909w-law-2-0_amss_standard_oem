/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION

  Network Location Provider

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.

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

package com.qualcomm.location.izatserviceprovider.provider;

import java.util.Properties;

import android.os.Bundle;
import android.os.Looper;
import android.os.WorkSource;
import android.os.IBinder;
import android.os.WorkSource;

import android.content.Context;

import android.location.Criteria;
import android.location.LocationProvider;
import android.location.LocationManager;

import android.util.Log;
import android.util.Slog;

import com.android.location.provider.LocationProviderBase;
import com.android.location.provider.ProviderRequestUnbundled;
import com.android.location.provider.ProviderPropertiesUnbundled;

import com.qualcomm.location.izatserviceprovider.provider.IzatProviderEngine;

import com.qualcomm.location.izatserviceprovider.provider.IzatProviderBase;

public class NetworkLocationProvider
    extends IzatProviderBase {

    public static NetworkLocationProvider
    getInstance (Context context, Looper looper) {
        if (VERBOSE_DBG) Log.v (TAG, "getInstance");
        if (mInstance == null) {
            mInstance = new NetworkLocationProvider (context, looper);
        }
        return mInstance;
    }

    public static void destroyInstance () {
        if (VERBOSE_DBG) Log.v (TAG, "destroyInstance");
        mInstance.mIzatProviderEngine.unsubscribe
        (
            mInstance.mProviderName
        );
        IzatProviderEngine.destroyInstance ();
        mInstance.mIzatProviderEngine = null;
        mInstance = null;
    }

    public NetworkLocationProvider (Context context, Looper looper) {
        super
        (
            context,
            looper,
            LocationManager.NETWORK_PROVIDER,
            TAG,
            ProviderPropertiesUnbundled.create
            (
                true,
                false,
                true,
                false,
                false,
                false,
                false,
                Criteria.POWER_LOW,
                Criteria.ACCURACY_COARSE
            )
        );
        if (VERBOSE_DBG) Log.v (TAG, "NetworkLocationProvider");
    }

    // LocationProviderBase overrides
    public void onEnable () {
        if (VERBOSE_DBG) Log.v (TAG, "onEnable");
        super.onEnable ();
    }

    public void onDisable () {
        if (VERBOSE_DBG) Log.v (TAG, "onDisable");
        super.onDisable ();
    }

    public void
    onSetRequest (ProviderRequestUnbundled request, WorkSource source) {
        if (VERBOSE_DBG) Log.v (TAG, "onSetRequest");
        super.onSetRequest (request, source);
    }

    public int onGetStatus (Bundle extras) {
        if (VERBOSE_DBG) Log.v (TAG, "onGetStatus");
        return super.onGetStatus (extras);
    }

    public long onGetStatusUpdateTime () {
        if (VERBOSE_DBG) Log.v (TAG, "onGetStatusUpdateTime");
        return super.onGetStatusUpdateTime ();
    }

    private static NetworkLocationProvider mInstance = null;
    private static final String TAG = "NetworkLocationProvider";
    private static final boolean VERBOSE_DBG = Log.isLoggable(TAG, Log.VERBOSE);
}
