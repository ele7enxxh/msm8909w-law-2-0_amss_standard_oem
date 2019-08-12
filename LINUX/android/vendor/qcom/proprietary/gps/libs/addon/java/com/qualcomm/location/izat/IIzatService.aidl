/* ======================================================================
*  Copyright (c) 2015 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qualcomm.location.izat;

import android.os.IBinder;
import java.com.qualcomm.location.izat.flp.IFlpService;
import java.com.qualcomm.location.izat.flp.ITestService;
import java.com.qualcomm.location.izat.geofence.IGeofenceService;

interface IIzatService {
    IFlpService getFlpService();
    ITestService getTestService();
    IGeofenceService getGeofenceService();
    String getVersion();
}