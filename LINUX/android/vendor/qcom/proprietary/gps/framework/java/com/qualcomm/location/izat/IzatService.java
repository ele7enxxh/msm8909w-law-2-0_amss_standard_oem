/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
package com.qualcomm.location.izat;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.IBinder;
import android.os.UserHandle;
import android.os.ServiceManager;
import android.os.RemoteException;
import android.util.Log;
import com.qualcomm.location.izat.flp.FlpServiceProvider;
import com.qualcomm.location.izat.flp.IFlpService;
import com.qualcomm.location.izat.flp.ITestService;
import com.qualcomm.location.izat.geofence.GeofenceServiceProvider;
import com.qualcomm.location.izat.geofence.IGeofenceService;

public class IzatService extends Service {
    private static final String TAG = "IzatService";
    private static final String Service_Version = "1.0.1";
    private static final boolean VERBOSE_DBG = Log.isLoggable(TAG, Log.VERBOSE);
    private static final String IZAT_SERVICE_NAME = "com.qualcomm.location.izat.IzatService";
    private static final String ACCESS_FINE_LOCATION =
            android.Manifest.permission.ACCESS_FINE_LOCATION;
    private Context mContext;

    @Override
    public void onCreate() {
        if (VERBOSE_DBG)
            Log.d(TAG, "onCreate");
        mContext = this;
        // register izat service on entry
        if (ServiceManager.getService(IZAT_SERVICE_NAME) == null) {
            Log.d(TAG, "register Izat service on entry");
            ServiceManager.addService(IZAT_SERVICE_NAME, mBinder.asBinder());
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        if (VERBOSE_DBG)
            Log.d(TAG, "onBind");
        if ("com.qualcomm.location.izat.IzatService".equals(intent.getAction())) {
            if (VERBOSE_DBG) {
                Log.d(TAG, "Got a binding request.");
            }
            return mBinder;
        } else {
            return null;
        }
    }

    /* Remote binder */
    private final IIzatService.Stub mBinder = new IIzatService.Stub() {
        public IFlpService getFlpService() {
            if (mContext.checkCallingPermission(ACCESS_FINE_LOCATION) !=
                    PackageManager.PERMISSION_GRANTED) {
                throw new SecurityException("Requires ACCESS_FINE_LOCATION permission");
            }
            FlpServiceProvider flpServiceProvider =
                FlpServiceProvider.getInstance();
            return flpServiceProvider.getFlpBinder();
        }
        public ITestService getTestService() {
            FlpServiceProvider flpServiceProvider =
                FlpServiceProvider.getInstance();
            return flpServiceProvider.getFlpTestingBinder();
        }
        public IGeofenceService getGeofenceService() {
            if (mContext.checkCallingPermission(ACCESS_FINE_LOCATION) !=
                    PackageManager.PERMISSION_GRANTED) {
                throw new SecurityException("Requires ACCESS_FINE_LOCATION permission");
            }
            GeofenceServiceProvider geofenceServiceProvider =
                GeofenceServiceProvider.getInstance();
            return geofenceServiceProvider.getGeofenceBinder();
        }
        public String getVersion() {
            return Service_Version;
        }
    };
}