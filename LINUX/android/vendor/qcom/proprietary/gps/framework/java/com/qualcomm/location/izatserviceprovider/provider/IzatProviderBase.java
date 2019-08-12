/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION

  IzatProviderBase - Base class for network and unified providers

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

public class IzatProviderBase
    extends LocationProviderBase {

    protected IzatProviderBase
    (
        Context context,
        Looper looper,
        String providerName,
        String tag,
        ProviderPropertiesUnbundled providerPropertiesUnbundled
    )
    {
        super (tag, providerPropertiesUnbundled);
        if (VERBOSE_DBG) Log.v (TAG, "IzatProviderBase");

        mContext = context;
        mTag = tag;
        mProviderName = providerName;

        if (mIzatProviderEngine == null) {
            mIzatProviderEngine =
                IzatProviderEngine.getInstance (context, looper);
        }

        mIzatProviderEngine.subscribe
        (
            mProviderName, this
        );
    }

    // LocationProviderBase overrides
    public void onEnable () {
        if (VERBOSE_DBG) Log.v (TAG, "onEnable");
    }

    public void onDisable () {
        if (VERBOSE_DBG) Log.v (TAG, "onDisable");
    }

    public void bindProvider () {
        if (VERBOSE_DBG) Log.v (TAG, "bindProvider: " + mProviderName);
        mIzatProviderEngine.enable(mProviderName);
    }

    public void unbindProvider () {
        if (VERBOSE_DBG) Log.v (TAG, "unbindProvider: "+ mProviderName);
        mIzatProviderEngine.disable(mProviderName);
    }

    public void
    onSetRequest (ProviderRequestUnbundled request, WorkSource source) {
        if (VERBOSE_DBG) Log.v (TAG, "onSetRequest");
        mIzatProviderEngine.setRequest
        (
            request,
            source,
            mProviderName
        );
    }

    public int onGetStatus (Bundle extras) {
        if (VERBOSE_DBG) Log.v (TAG, "onGetStatus");
        return mIzatProviderEngine.getStatus (extras);
    }

    public long onGetStatusUpdateTime () {
        if (VERBOSE_DBG) Log.v (TAG, "onGetStatiusUpdateTime");
        return mIzatProviderEngine.getStatusUpdateTime ();
    }

    protected Context mContext;
    protected IzatProviderEngine mIzatProviderEngine = null;
    protected String mProviderName;
    protected String mTag;
    private static final String TAG = "IzatProviderBase";
    private static final boolean VERBOSE_DBG = Log.isLoggable (TAG, Log.VERBOSE);
}
