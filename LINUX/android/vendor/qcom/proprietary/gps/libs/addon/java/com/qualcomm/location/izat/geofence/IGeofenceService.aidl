/* ======================================================================
*  Copyright (c) 2015 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qualcomm.location.izat.geofence;

import java.com.qualcomm.location.izat.geofence.IGeofenceCallback;

interface IGeofenceService {

    void registerCallback (in IGeofenceCallback callback);

    void unregisterCallback (in IGeofenceCallback callback);

    int addGeofence (in double latitude,
                     in double longitude,
                     in double radius,
                     in int transitionTypes,
                     in int responsiveness,
                     in int confidence,
                     in int dwellTime,
                     in int dwellTimeMask);

    void removeGeofence (in int geofenceId);

    void updateGeofence (in int geofenceId,
                         in int transitionTypes,
                         in int notifyResponsiveness);

    void pauseGeofence (in int geofenceId);

    void resumeGeofence (in int geofenceId,
                         in int transitionTypes);
}
