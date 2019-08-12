/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef GPS_GEOFENCE_CALLBACKS_H
#define GPS_GEOFENCE_CALLBACKS_H

#include "GeofenceCallbacks.h"

class GpsGeofenceCb : public GeofenceCallbacks{

    GpsGeofenceCallbacks* gCallbacks;

public:

    inline GpsGeofenceCb(GpsGeofenceCallbacks* callbacks) : GeofenceCallbacks()
    {
        gCallbacks = callbacks;
    }
    inline ~GpsGeofenceCb() {}

    inline void convertFlpExtLocationToGpsLocation(GpsLocation* gpsLocation,
                                                   FlpExtLocation* flpLocation) {
        gpsLocation->size = sizeof(GpsLocation);
        if (flpLocation->flags & FLP_EXTENDED_LOCATION_HAS_LAT_LONG) {
            gpsLocation->latitude = flpLocation->latitude;
            gpsLocation->longitude = flpLocation->longitude;
            gpsLocation->flags |= GPS_LOCATION_HAS_LAT_LONG;
        }
        if (flpLocation->flags & FLP_EXTENDED_LOCATION_HAS_ALTITUDE) {
            gpsLocation->altitude = flpLocation->altitude;
            gpsLocation->flags |= GPS_LOCATION_HAS_ALTITUDE;
        }
        if (flpLocation->flags & FLP_EXTENDED_LOCATION_HAS_SPEED) {
            gpsLocation->speed = flpLocation->speed;
            gpsLocation->flags |= GPS_LOCATION_HAS_SPEED;
        }
        if (flpLocation->flags & FLP_EXTENDED_LOCATION_HAS_BEARING) {
            gpsLocation->bearing = flpLocation->bearing;
            gpsLocation->flags |= GPS_LOCATION_HAS_BEARING;
        }
        if (flpLocation->flags & FLP_EXTENDED_LOCATION_HAS_ACCURACY) {
            gpsLocation->accuracy = flpLocation->accuracy;
            gpsLocation->flags |= GPS_LOCATION_HAS_ACCURACY;
        }
        gpsLocation->timestamp = flpLocation->timestamp;
    }

    inline void geofence_transition_callback (int32_t geofence_id,
                                             FlpExtLocation* location,
                                             int32_t transition,
                                             int64_t timestamp,
                                             uint32_t sources_used)
    {
        if (location != NULL) {
            GpsLocation gpsLocation;
            convertFlpExtLocationToGpsLocation(&gpsLocation, location);
            gCallbacks->geofence_transition_callback(geofence_id,
                                                     &gpsLocation,
                                                     transition,
                                                     timestamp);
        } else {
            gCallbacks->geofence_transition_callback(geofence_id,
                                                     NULL,
                                                     transition,
                                                     timestamp);
        }
    }
    inline void geofence_status_callback (int32_t status,
                                         uint32_t source,
                                         FlpExtLocation* last_location)
    {
        if (last_location != NULL) {
            GpsLocation gpsLocation;
            convertFlpExtLocationToGpsLocation(&gpsLocation, last_location);
            gCallbacks->geofence_status_callback(status, &gpsLocation);
        } else {
            gCallbacks->geofence_status_callback(status, NULL);
        }

    }
    inline void geofence_add_callback (int32_t geofence_id, int32_t result)
    {
        gCallbacks->geofence_add_callback(geofence_id, result);
    }
    inline void geofence_remove_callback (int32_t geofence_id, int32_t result)
    {
        gCallbacks->geofence_remove_callback(geofence_id, result);
    }
    inline void geofence_pause_callback (int32_t geofence_id, int32_t result)
    {
        gCallbacks->geofence_pause_callback(geofence_id, result);
    }
    inline void geofence_resume_callback (int32_t geofence_id, int32_t result)
    {
        gCallbacks->geofence_resume_callback(geofence_id, result);
    }
};

#endif /* GPS_GEOFENCE_CALLBACKS_H */
