/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef GEOFENCER_CALLBACKS_H
#define GEOFENCER_CALLBACKS_H

struct FlpExtLocation_s;

class GeofenceCallbacks {

public:

    GeofenceCallbacks() {}
    inline virtual ~GeofenceCallbacks() {}

    virtual void geofence_transition_callback (int32_t geofence_id,
                                              struct FlpExtLocation_s * location,
                                              int32_t transition,
                                              int64_t timestamp,
                                              uint32_t sources_used) = 0;
    virtual void geofence_status_callback (int32_t status,
                                          uint32_t source,
                                          struct FlpExtLocation_s * last_location) = 0;
    virtual void geofence_add_callback (int32_t geofence_id, int32_t result) = 0;
    virtual void geofence_remove_callback (int32_t geofence_id, int32_t result) = 0;
    virtual void geofence_pause_callback (int32_t geofence_id, int32_t result) = 0;
    virtual void geofence_resume_callback (int32_t geofence_id, int32_t result) = 0;
};

#endif /* GEOFENCER_CALLBACKS_H */
