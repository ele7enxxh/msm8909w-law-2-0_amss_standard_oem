/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       GeoFencer module

GENERAL DESCRIPTION
  GeoFencer module

  Copyright (c) 2013 Qualcomm Atheros, Inc.
  All Rights Reserved.
  Qualcomm Atheros Confidential and Proprietary.

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential
=============================================================================*/

#ifndef GEOFENCER_H
#define GEOFENCER_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <map>
#include "hardware/gps.h"
#include "geofence.h"
#include "GeofenceCallbacks.h"
#include "GeofenceAdapter.h"

typedef void (*gps_ext_set_thread_event)();
class GeofenceAdapter;

class GeoFencer {

public:

    GeofenceCallbacks* mCallbacks;

    // constructor for gps client
    inline GeoFencer(GeofenceCallbacks* callbacks, gps_create_thread create_thread_cb) :
        mCallbacks(callbacks),
        mAdapter(GeofenceAdapter::get(this,
                                      (LocThread::tCreate)create_thread_cb)) {
    }

    // constructor for flp client
    inline GeoFencer(GeofenceCallbacks* callbacks, LocMsg* assoMsg) :
        mCallbacks(callbacks),
        mAdapter(GeofenceAdapter::get(this, assoMsg)) {
    }

    inline ~GeoFencer() {}

    GeofenceAdapter* mAdapter;

    void addCommand(int32_t geofence_id, double latitude,
                    double longitude, double radius_meters,
                    int last_transition, int monitor_transitions,
                    int notification_responsiveness_ms,
                    int unknown_timer_ms, int confidence,
                    uint32_t dwell_time_s = 0,
                    uint8_t dwell_time_mask = 0);

    void removeCommand(int32_t geofence_id);

    void pauseCommand(int32_t geofence_id);

    void resumeCommand(int32_t geofence_id, int monitor_transitions);

    void modifyCommand(int32_t geofence_id, GeofenceExtOptions* options);

};

#endif /* GEOFENCER_H */
