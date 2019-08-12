/*****************************************************************************
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/

#include <hardware/gps.h>
#include <hardware/fused_location.h>
#include "log_util.h"
#include "android_runtime/AndroidRuntime.h"
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <inttypes.h>
#include <map>

#define LOG_TAG "LocSvc_test"

static const FlpLocationInterface* sFlpInterface = NULL;
static const FlpGeofencingInterface* sFlpGeoFencingInterface = NULL;

static const GpsInterface* sGpsInterface = NULL;
static const GpsGeofencingInterface* sGpsGeoFencingInterface = NULL;
static const AGpsInterface* sAGpsInterface = NULL;

typedef std::map<int32_t, GeofenceData> GeoFencesMap;
static GeoFencesMap sGpsGeoFencesRunning;
static GeoFencesMap sGpsGeoFencesPaused;
static GeoFencesMap sFlpGeoFencesRunning;
static GeoFencesMap sFlpGeoFencesPaused;

enum GeoFenceCommands {
    ADD_COMMAND = 0,
    PAUSE_COMMAND = 1,
    RESUME_COMMAND = 2,
    MODIFY_COMMAND = 3,
    REMOVE_COMMAND = 4,
};

bool gFlpGeofenceAddCallbackCalled = false;
bool gFlpGeofenceRemoveCallbackCalled = false;
bool gFlpGeofencePauseCallbackCalled = false;
bool gFlpGeofenceResumeCallbackCalled = false;
bool gGpsGeofenceAddCallbackCalled = false;
bool gGpsGeofenceRemoveCallbackCalled = false;
bool gGpsGeofencePauseCallbackCalled = false;
bool gGpsGeofenceResumeCallbackCalled = false;
pthread_cond_t gCondGeofence = PTHREAD_COND_INITIALIZER;
pthread_mutex_t gMutexGeofence = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gCondLocation = PTHREAD_COND_INITIALIZER;
pthread_mutex_t gMutexLocation = PTHREAD_MUTEX_INITIALIZER;

typedef void (*ThreadStart) (void *);
ThreadStart g_pfnThreadStart;

// Command Line arguments
bool gGpsVerifyTestInProgress = false;
int gStopOnMinSvs = 0; // part of conditions that cause test to stop. Minimum number of SVs
float gStopOnMinSnr = 0; // part of conditions that casue test to stop. Minimum number of SNR

void *my_thread_fn (void *arg)
{
    g_pfnThreadStart (arg);
    return NULL;
}

pthread_t create_thread_callback(const char *name, void (*start) (void *), void *arg)
{
    pthread_t thread_id;
    printf("%s ", name);
    LOC_LOGW("%s ", name);
    g_pfnThreadStart = start;

    if (0 > pthread_create (&thread_id, NULL, my_thread_fn, arg)) {
        printf("error creating thread\n");
        LOC_LOGW("error creating thread");
    } else {
        printf("created thread\n");
        LOC_LOGW("created thread");
    }

    return thread_id;
}

static void gps_loc_location_callback(GpsLocation* location)
{
    printf("EVENT LOCATION_CALLBACK\n");
    LOC_LOGW("EVENT LOCATION_CALLBACK");
    printf("LAT: %8.4f LON: %9.4f ACC: %8.2f TIME: %13lld\n",
          location->latitude, location->longitude, location->accuracy, location->timestamp);
    LOC_LOGW("LAT: %8.4f LON: %9.4f ACC: %8.2f TIME: %13lld",
          location->latitude, location->longitude, location->accuracy, location->timestamp);

    if (gGpsVerifyTestInProgress)
    {
        printf("GPS VERIFY TEST SUCCESSFUL\n");
        LOC_LOGW("GPS VERIFY TEST SUCCESSFUL");
        pthread_mutex_lock(&gMutexLocation);
        pthread_cond_signal(&gCondLocation);
        pthread_mutex_unlock(&gMutexLocation);
    }

}

static void gps_loc_status_callback(GpsStatus* status)
{
    printf("EVENT GPS STATUS_CALLBACK status=%d\n", status->status);
    LOC_LOGW("EVENT GPS STATUS_CALLBACK status=%d", status->status);
}

static void gps_loc_sv_status_callback(GpsSvStatus* sv_info)
{
    printf("EVENT GPS SV_STATUS_CALLBACK Svs=%d\n", sv_info->num_svs);
    LOC_LOGW("EVENT GPS SV_STATUS_CALLBACK Svs=%d", sv_info->num_svs);

    if (gGpsVerifyTestInProgress) {
        for(int i=0;i<sv_info->num_svs; ++i)
        {
            printf("%02d : PRN: %04d, SNR: %09.4f, ELE: %09.4f, AZI: %09.4f\n",
                          i+1,sv_info->sv_list[i].prn,
                              sv_info->sv_list[i].snr,
                              sv_info->sv_list[i].elevation,
                              sv_info->sv_list[i].azimuth);
            LOC_LOGW("%02d : PRN: %04d, SNR: %09.4f, ELE: %09.4f, AZI: %09.4f",
                          i+1,sv_info->sv_list[i].prn,
                              sv_info->sv_list[i].snr,
                              sv_info->sv_list[i].elevation,
                              sv_info->sv_list[i].azimuth);
        }
        printf("Ephemeris Mask : 0x%X, ",sv_info->ephemeris_mask);
        LOC_LOGW("Ephemeris Mask : 0x%X",sv_info->ephemeris_mask);
        printf("Almanac Mask: 0x%X, ",sv_info->almanac_mask);
        LOC_LOGW("Almanac Mask: 0x%X",sv_info->almanac_mask);
        printf("Used in Fix Mask: 0x%X:\n",sv_info->used_in_fix_mask);
        LOC_LOGW("Used in Fix Mask: 0x%X:",sv_info->used_in_fix_mask);

        // if the minimum number of SVs with minimum number of SNR
        // has been satisfied then stop session
        if (gStopOnMinSvs > 0 && gStopOnMinSnr > 0) {
            if (gStopOnMinSvs <= sv_info->num_svs) {
                int minCnSvCount = 0;
                // count number of SVs that meet the min SNR
                for(int i=0; i<sv_info->num_svs; ++i) {
                    if (sv_info->sv_list[i].snr >= gStopOnMinSnr) {
                        minCnSvCount++;
                    }
                }
                if (minCnSvCount >= gStopOnMinSvs){
                    printf("GPS VERIFY TEST SUCCESSFUL, as %d SVs are seen with at least a SNR of %f\n",
                              gStopOnMinSvs, gStopOnMinSnr);
                    LOC_LOGW("GPS VERIFY TEST SUCCESSFUL, as %d SVs are seen with at least a SNR of %f",
                              gStopOnMinSvs, gStopOnMinSnr);
                    pthread_mutex_lock(&gMutexLocation);
                    pthread_cond_signal(&gCondLocation);
                    pthread_mutex_unlock(&gMutexLocation);
                }
            }
        }
    }

}

static void gps_loc_nmea_callback(GpsUtcTime timestamp, const char* nmea, int length)
{
    //printf("EVENT NMEA_CALLBACK\n");
    //LOC_LOGW("EVENT NMEA_CALLBACK");
}

static void gps_loc_set_capabilities_callback(uint32_t capabilities)
{
    printf("EVENT GPS SET_CAPABILITIES_CALLBACK\n");
    LOC_LOGW("EVENT GPS SET_CAPABILITIES_CALLBACK");
}

static void gps_loc_acquire_wakelock_callback()
{
    printf("EVENT GPS ACQUIRE_WAKELOCK_CALLBACK\n");
    LOC_LOGW("EVENT GPS ACQUIRE_WAKELOCK_CALLBACK");
}

static void gps_loc_release_wakelock_callback()
{
    printf("EVENT GPS RELEASE_WAKELOCK_CALLBACK\n");
    LOC_LOGW("EVENT GPS RELEASE_WAKELOCK_CALLBACK");
}

static void gps_loc_request_utc_time_callback()
{
    printf("EVENT GPS REQUEST_UTC_TIME_CALLBACK\n");
    LOC_LOGW("EVENT GPS REQUEST_UTC_TIME_CALLBACK");
}

static void agps_loc_status_callback(AGpsStatus* status)
{
    printf("EVENT AGPS STATUS %d\n", status->status);
    LOC_LOGW("EVENT AGPS STATUS %d", status->status);
}

#ifdef GNSS_CALLBACKS_PRESENT
static void gnss_loc_set_system_info_callback(const GnssSystemInfo* info)
{
    printf("EVENT GNSS SET SYSTEM INFO %d\n", info->year_of_hw);
    LOC_LOGW("EVENT GNSS SET SYSTEM INFO %d", info->year_of_hw);
}

static void gnss_loc_sv_status_callback(GnssSvStatus* sv_info)
{
    printf("EVENT GNSS SV STATUS num of svs %d\n", sv_info->num_svs);
    LOC_LOGW("EVENT GNSS SV STATUS num of svs %d", sv_info->num_svs);

    if (gGpsVerifyTestInProgress) {
        for(int i=0;i<sv_info->num_svs; ++i)
        {
            printf("%02d : PRN:%03d SNR:%06.2f ELE:%08.2f AZI:%08.2f EPH:%u ALM:%u USED:%u\n",
                          i+1,sv_info->gnss_sv_list[i].svid,
                              sv_info->gnss_sv_list[i].c_n0_dbhz,
                              sv_info->gnss_sv_list[i].elevation,
                              sv_info->gnss_sv_list[i].azimuth,
                              (sv_info->gnss_sv_list[i].flags & GNSS_SV_FLAGS_HAS_EPHEMERIS_DATA) ==
                                                                GNSS_SV_FLAGS_HAS_EPHEMERIS_DATA,
                              (sv_info->gnss_sv_list[i].flags & GNSS_SV_FLAGS_HAS_ALMANAC_DATA) ==
                                                                GNSS_SV_FLAGS_HAS_ALMANAC_DATA,
                              (sv_info->gnss_sv_list[i].flags & GNSS_SV_FLAGS_USED_IN_FIX) ==
                                                                GNSS_SV_FLAGS_USED_IN_FIX);
            LOC_LOGW("%02d : PRN:%03d SNR:%06.2f ELE:%08.2f AZI:%08.2f EPH:%u ALM:%u USED:%u",
                          i+1,sv_info->gnss_sv_list[i].svid,
                              sv_info->gnss_sv_list[i].c_n0_dbhz,
                              sv_info->gnss_sv_list[i].elevation,
                              sv_info->gnss_sv_list[i].azimuth,
                              (sv_info->gnss_sv_list[i].flags & GNSS_SV_FLAGS_HAS_EPHEMERIS_DATA) ==
                                                                GNSS_SV_FLAGS_HAS_EPHEMERIS_DATA,
                              (sv_info->gnss_sv_list[i].flags & GNSS_SV_FLAGS_HAS_ALMANAC_DATA) ==
                                                                GNSS_SV_FLAGS_HAS_ALMANAC_DATA,
                              (sv_info->gnss_sv_list[i].flags & GNSS_SV_FLAGS_USED_IN_FIX) ==
                                                                GNSS_SV_FLAGS_USED_IN_FIX);
        }

        // if the minimum number of SVs with minimum number of SNR
        // has been satisfied then stop session
        if (gStopOnMinSvs > 0 && gStopOnMinSnr > 0) {
            if (gStopOnMinSvs <= sv_info->num_svs) {
                int minCnSvCount = 0;
                // count number of SVs that meet the min SNR
                for(int i=0; i<sv_info->num_svs; ++i) {
                    if (sv_info->gnss_sv_list[i].c_n0_dbhz >= gStopOnMinSnr) {
                        minCnSvCount++;
                    }
                }
                if (minCnSvCount >= gStopOnMinSvs){
                    printf("GPS VERIFY TEST SUCCESSFUL, as %d SVs are seen with at least a SNR of %f\n",
                              gStopOnMinSvs, gStopOnMinSnr);
                    LOC_LOGW("GPS VERIFY TEST SUCCESSFUL, as %d SVs are seen with at least a SNR of %f",
                              gStopOnMinSvs, gStopOnMinSnr);
                    pthread_mutex_lock(&gMutexLocation);
                    pthread_cond_signal(&gCondLocation);
                    pthread_mutex_unlock(&gMutexLocation);
                }
            }
        }
    }

}
#endif

GpsCallbacks sGpsCallbacks = {
    sizeof(GpsCallbacks),
    gps_loc_location_callback,
    gps_loc_status_callback,
    gps_loc_sv_status_callback,
    gps_loc_nmea_callback,
    gps_loc_set_capabilities_callback,
    gps_loc_acquire_wakelock_callback,
    gps_loc_release_wakelock_callback,
    create_thread_callback,
    gps_loc_request_utc_time_callback,
#ifdef GNSS_CALLBACKS_PRESENT
    gnss_loc_set_system_info_callback,
    gnss_loc_sv_status_callback,
#endif
};

AGpsCallbacks sAGpsCallbacks = {
    agps_loc_status_callback,
    create_thread_callback,
};

static void flp_loc_location_callback(int32_t num_locations, FlpLocation** locations) {

    printf("EVENT FLP LOCATION num_locations=%d\n", num_locations);
    LOC_LOGW("EVENT FLP LOCATION num_locations=%d", num_locations);

    if (num_locations <= 0)
        return;

    printf(" flags | latitude | longitude | altitude | accuracy | speed | bearing |     time      | sources \n");
    LOC_LOGW(" flags | latitude | longitude | altitude | accuracy | speed | bearing |     time      | sources ");

    for (int i=0; i<num_locations; ++i) {
        printf("%6d | %8.4f | %9.4f | %8.2f | %8.2f | %5.2f | %7.2f | %13lld | %d\n",
              locations[i]->flags, locations[i]->latitude, locations[i]->longitude, locations[i]->altitude,
              locations[i]->accuracy, locations[i]->speed, locations[i]->bearing, locations[i]->timestamp,
              locations[i]->sources_used);
        LOC_LOGW("%6d | %8.4f | %9.4f | %8.2f | %8.2f | %5.2f | %7.2f | %13lld | %d",
              locations[i]->flags, locations[i]->latitude, locations[i]->longitude, locations[i]->altitude,
              locations[i]->accuracy, locations[i]->speed, locations[i]->bearing, locations[i]->timestamp,
              locations[i]->sources_used);
    }
}

static void flp_loc_acquire_wakelock() {

    printf("EVENT FLP ACQUIRE_WAKELOCK\n");
    LOC_LOGW("EVENT FLP ACQUIRE_WAKELOCK");
}

static void flp_loc_release_wakelock() {

    printf("EVENT FLP RELEASE_WAKELOCK\n");
    LOC_LOGW("EVENT FLP RELEASE_WAKELOCK");
}

static int flp_loc_set_thread_event(ThreadEvent event) {

    printf("EVENT FLP SET THREAD EVENT\n");
    LOC_LOGW("EVENT FLP SET THREAD EVENT");
    return FLP_RESULT_SUCCESS;
}

static void flp_loc_capabilities_callback(int capabilities) {
    printf("EVENT FLP_CAPABILITIES = %d ", capabilities);
    LOC_LOGW("EVENT FLP_CAPABILITIES = %d ", capabilities);
    if (capabilities & CAPABILITY_GNSS) {
        printf("GNSS ");
        LOC_LOGW("GNSS ");
    }
    if (capabilities & CAPABILITY_WIFI) {
        printf("WIFI ");
        LOC_LOGW("WIFI ");
    }
    if (capabilities & CAPABILITY_CELL) {
        printf("CELL ");
        LOC_LOGW("CELL ");
    }
    printf("\n");

}

static void flp_loc_status_callback(int32_t status) {
    if (FLP_STATUS_LOCATION_AVAILABLE == status) {
        printf("EVENT FLP_STATUS = AVAILABLE\n");
        LOC_LOGW("EVENT FLP_STATUS = AVAILABLE");
    } else if (FLP_STATUS_LOCATION_UNAVAILABLE == status) {
        printf("EVENT FLP_STATUS = AVAILABLE\n");
        LOC_LOGW("EVENT FLP_STATUS = AVAILABLE");
    } else {
        printf("EVENT FLP_STATUS = INVALID STATUS!\n");
        LOC_LOGE("EVENT FLP_STATUS = INVALID_STATUS!");
    }
}

static void geofence_transition_callback(int32_t geofence_id,  GpsLocation* location,
                                         int32_t transition, GpsUtcTime timestamp) {
    printf("EVENT GPS GEOFENCE TRANSITION id=%d transition=%d lat=%8.2f long=%8.2f accuracy=%8.2f speed=%8.2f bearing=%8.2f time=%lld\n",
              geofence_id, transition, location->latitude, location->longitude, location->accuracy, location->speed, location->bearing, timestamp);
    LOC_LOGW("EVENT GPS GEOFENCE TRANSITION id=%d transition=%d lat=%8.2f long=%8.2f accuracy=%8.2f speed=%8.2f bearing=%8.2f time=%lld",
              geofence_id, transition, location->latitude, location->longitude, location->accuracy, location->speed, location->bearing, timestamp);

}

static void geofence_status_callback(int32_t status, GpsLocation* last_location) {

    printf("EVENT GPS GEOFENCE STATUS status=%d\n", status);
    LOC_LOGW("EVENT GPS GEOFENCE STATUS status=%d", status);
}

static void geofence_add_callback(int32_t geofence_id, int32_t status) {

    printf("EVENT GPS GEOFENCE ADD geofence_id=%d status=%d\n", geofence_id, status);
    LOC_LOGW("EVENT GPS GEOFENCE ADD geofence_id=%d status=%d", geofence_id, status);

    pthread_mutex_lock(&gMutexGeofence);
    gGpsGeofenceAddCallbackCalled = true;
    pthread_cond_broadcast(&gCondGeofence);
    pthread_mutex_unlock(&gMutexGeofence);
}

static void geofence_remove_callback(int32_t geofence_id, int32_t status) {

    printf("EVENT GPS GEOFENCE REMOVE geofence_id=%d status=%d\n", geofence_id, status);
    LOC_LOGW("EVENT GPS GEOFENCE REMOVE geofence_id=%d status=%d", geofence_id, status);

    pthread_mutex_lock(&gMutexGeofence);
    gGpsGeofenceRemoveCallbackCalled = true;
    pthread_cond_broadcast(&gCondGeofence);
    pthread_mutex_unlock(&gMutexGeofence);
}

static void geofence_pause_callback(int32_t geofence_id, int32_t status) {

    printf("EVENT GPS GEOFENCE PAUSE geofence_id=%d status=%d\n", geofence_id, status);
    LOC_LOGW("EVENT GPS GEOFENCE PAUSE geofence_id=%d status=%d", geofence_id, status);

    pthread_mutex_lock(&gMutexGeofence);
    gGpsGeofencePauseCallbackCalled = true;
    pthread_cond_broadcast(&gCondGeofence);
    pthread_mutex_unlock(&gMutexGeofence);
}

static void geofence_resume_callback(int32_t geofence_id, int32_t status) {

    printf("EVENT GPS GEOFENCE RESUME geofence_id=%d status=%d\n", geofence_id, status);
    LOC_LOGW("EVENT GPS GEOFENCE RESUME geofence_id=%d status=%d", geofence_id, status);

    pthread_mutex_lock(&gMutexGeofence);
    gGpsGeofenceResumeCallbackCalled = true;
    pthread_cond_broadcast(&gCondGeofence);
    pthread_mutex_unlock(&gMutexGeofence);
}

GpsGeofenceCallbacks sGpsGeofenceCallbacks = {
    geofence_transition_callback,
    geofence_status_callback,
    geofence_add_callback,
    geofence_remove_callback,
    geofence_pause_callback,
    geofence_resume_callback,
    create_thread_callback
};

FlpCallbacks sFlpCallbacks = {
    sizeof(FlpCallbacks),
    flp_loc_location_callback,
    flp_loc_acquire_wakelock,
    flp_loc_release_wakelock,
    flp_loc_set_thread_event,
    flp_loc_capabilities_callback,
    flp_loc_status_callback
};

static void flp_loc_geofence_transition_callback(int32_t geofence_id,  FlpLocation* location, int32_t transition,
                                                 FlpUtcTime timestamp, uint32_t sources_used) {
    printf("EVENT FLP GEOFENCE TRANSITION id=%d transition=%d lat=%8.2f long=%8.2f accuracy=%8.2f speed=%8.2f bearing=%8.2f time=%lld\n",
              geofence_id, transition, location->latitude, location->longitude, location->accuracy, location->speed, location->bearing, timestamp);
    LOC_LOGW("EVENT FLP GEOFENCE TRANSITION id=%d transition=%d lat=%8.2f long=%8.2f accuracy=%8.2f speed=%8.2f bearing=%8.2f time=%lld",
              geofence_id, transition, location->latitude, location->longitude, location->accuracy, location->speed, location->bearing, timestamp);
}

static void flp_loc_geofence_monitor_status_callback(int32_t status, uint32_t source, FlpLocation* last_location) {

    printf("EVENT FLP GEOFENCE STATUS status=%d\n", status);
    LOC_LOGW("EVENT FLP GEOFENCE STATUS status=%d", status);
}

static void flp_loc_geofence_add_callback(int32_t geofence_id, int32_t result) {

    printf("EVENT FLP GEOFENCE ADD geofence_id=%d result=%d\n", geofence_id, result);
    LOC_LOGW("EVENT FLP GEOFENCE ADD geofence_id=%d result=%d", geofence_id, result);

    pthread_mutex_lock(&gMutexGeofence);
    gFlpGeofenceAddCallbackCalled = true;
    pthread_cond_broadcast(&gCondGeofence);
    pthread_mutex_unlock(&gMutexGeofence);

}

static void flp_loc_geofence_remove_callback(int32_t geofence_id, int32_t result) {

    printf("EVENT FLP GEOFENCE REMOVE geofence_id=%d result=%d\n", geofence_id, result);
    LOC_LOGW("EVENT FLP GEOFENCE REMOVE geofence_id=%d result=%d", geofence_id, result);

    pthread_mutex_lock(&gMutexGeofence);
    gFlpGeofenceRemoveCallbackCalled = true;
    pthread_cond_broadcast(&gCondGeofence);
    pthread_mutex_unlock(&gMutexGeofence);
}

static void flp_loc_geofence_pause_callback(int32_t geofence_id, int32_t result) {

    printf("EVENT FLP GEOFENCE PAUSE geofence_id=%d result=%d\n", geofence_id, result);
    LOC_LOGW("EVENT FLP GEOFENCE PAUSE geofence_id=%d result=%d", geofence_id, result);

    pthread_mutex_lock(&gMutexGeofence);
    gFlpGeofencePauseCallbackCalled = true;
    pthread_cond_broadcast(&gCondGeofence);
    pthread_mutex_unlock(&gMutexGeofence);
}

static void flp_loc_geofence_resume_callback(int32_t geofence_id, int32_t result) {

    printf("EVENT FLP GEOFENCE RESUME geofence_id=%d result=%d\n", geofence_id, result);
    LOC_LOGW("EVENT FLP GEOFENCE RESUME geofence_id=%d result=%d", geofence_id, result);

    pthread_mutex_lock(&gMutexGeofence);
    gFlpGeofenceResumeCallbackCalled = true;
    pthread_cond_broadcast(&gCondGeofence);
    pthread_mutex_unlock(&gMutexGeofence);
}

FlpGeofenceCallbacks sFlpGeofenceCallbacks = {
    sizeof(FlpGeofenceCallbacks),
    flp_loc_geofence_transition_callback,
    flp_loc_geofence_monitor_status_callback,
    flp_loc_geofence_add_callback,
    flp_loc_geofence_remove_callback,
    flp_loc_geofence_pause_callback,
    flp_loc_geofence_resume_callback,
    flp_loc_set_thread_event
};

bool init()
{
    int err;
    hw_module_t* gps_module;
    hw_module_t* flp_module;

    err = hw_get_module(GPS_HARDWARE_MODULE_ID, (hw_module_t const**)&gps_module);
    if (err == 0) {
        hw_device_t* device;
        err = gps_module->methods->open(gps_module, GPS_HARDWARE_MODULE_ID, &device);
        if (err == 0) {
            gps_device_t* gps_device = (gps_device_t *)device;
            sGpsInterface = gps_device->get_gps_interface(gps_device);
        } else {
            printf("gps_module->methods->open returned: %i\n", err);
            LOC_LOGE("gps_module->methods->open returned: %i", err);
        }
    } else {
        printf("hw_get_module GPS returned: %i\n", err);
        LOC_LOGE("hw_get_module GPS returned: %i", err);
    }

    if (sGpsInterface) {
        int ret = sGpsInterface->init(&sGpsCallbacks);
        if (ret) {
            printf("sGpsInterface->init returned: %i\n", ret);
            LOC_LOGE("sGpsInterface->init returned: %i", ret);
        }
        sAGpsInterface =
            (const AGpsInterface*)sGpsInterface->get_extension(AGPS_INTERFACE);

        if (sAGpsInterface) {
            sAGpsInterface->init(&sAGpsCallbacks);
        }

        sGpsGeoFencingInterface =
                (const GpsGeofencingInterface*)sGpsInterface->get_extension(GPS_GEOFENCING_INTERFACE);

        if (sGpsGeoFencingInterface) {
            sGpsGeoFencingInterface->init(&sGpsGeofenceCallbacks);
        }
    }

    err = hw_get_module(FUSED_LOCATION_HARDWARE_MODULE_ID, (hw_module_t const**)&flp_module);
    if (err == 0) {
        hw_device_t* device;
        err = flp_module->methods->open(flp_module, FUSED_LOCATION_HARDWARE_MODULE_ID, &device);
        if (err == 0) {
            flp_device_t* flp_device = (flp_device_t *)device;
            sFlpInterface = flp_device->get_flp_interface(flp_device);
        } else {
            printf("flp_module->methods->open returned: %i\n", err);
            LOC_LOGE("flp_module->methods->open returned: %i", err);
        }
    } else {
        printf("hw_get_module FLP returned: %i\n", err);
        LOC_LOGE("hw_get_module FLP returned: %i", err);
    }

    if (sFlpInterface) {
        int ret = sFlpInterface->init(&sFlpCallbacks);
        if (ret) {
            printf("sFlpInterface->init returned: %i\n", ret);
            LOC_LOGE("sFlpInterface->init returned: %i", ret);
        }
        sFlpGeoFencingInterface =
                (const FlpGeofencingInterface*)sFlpInterface->get_extension(FLP_GEOFENCING_INTERFACE);

        if (sFlpGeoFencingInterface) {
            sFlpGeoFencingInterface->init(&sFlpGeofenceCallbacks);
        }
        return true;

    }
    return false;
}

float randomFloat(float low, float high)
{
     return (low + (float)rand()/((float)RAND_MAX/(high-low)));
}

int randomInt(int low, int high)
{
     return(low + (rand() % (high-low+1)));
}

void addRandomGeofence()
{
    Geofence geofence;
    GeofenceData data;
    GeofenceOptions options;
    geofence.data = &data;
    geofence.options = &options;
    geofence.data->type = TYPE_CIRCLE;
    bool isFlpGeofence = randomInt(0,1);
    GeoFencesMap::iterator it;
    GeoFencesMap& runningMap(isFlpGeofence ? sFlpGeoFencesRunning : sGpsGeoFencesRunning);

    do {
        geofence.geofence_id = randomInt(0,4096);
        it = runningMap.find(geofence.geofence_id);
    } while (it != runningMap.end());

    geofence.data->geofence.circle.latitude = randomFloat(-90,90);
    geofence.data->geofence.circle.longitude = randomFloat(-180,180);
    geofence.data->geofence.circle.radius_m = randomFloat(1,1000);
    geofence.options->monitor_transitions = FLP_GEOFENCE_TRANSITION_ENTERED |
                                  FLP_GEOFENCE_TRANSITION_EXITED  |
                                  FLP_GEOFENCE_TRANSITION_UNCERTAIN;
    geofence.options->notification_responsivenes_ms = randomInt(1,0x40000);

    runningMap[geofence.geofence_id] = *geofence.data;

    if (isFlpGeofence) {
        printf("\nADD FLP GEOFENCE -> id=%2u | lat=%8.2f | lon=%8.2f | rad=%2.2f | resp=%d(ms)\n",
                geofence.geofence_id, geofence.data->geofence.circle.latitude,
                geofence.data->geofence.circle.longitude, geofence.data->geofence.circle.radius_m,
                geofence.options->notification_responsivenes_ms);
        LOC_LOGW("ADD FLP GEOFENCE -> id=%2u | lat=%8.2f | lon=%8.2f | rad=%2.2f | resp=%d(ms)",
                geofence.geofence_id, geofence.data->geofence.circle.latitude,
                geofence.data->geofence.circle.longitude, geofence.data->geofence.circle.radius_m,
                geofence.options->notification_responsivenes_ms);

        Geofence* geofences[1];
        geofences[0] = &geofence;

        gFlpGeofenceAddCallbackCalled = false;
        sFlpGeoFencingInterface->add_geofences(1, (Geofence**)&geofences);
        pthread_mutex_lock(&gMutexGeofence);
        printf("Waiting on Callback...\n");
        while (gFlpGeofenceAddCallbackCalled == false)
            pthread_cond_wait(&gCondGeofence, &gMutexGeofence);
        pthread_mutex_unlock(&gMutexGeofence);

    } else {
        printf("\nADD GPS GEOFENCE -> id=%2u | lat=%8.2f | lon=%8.2f | rad=%2.2f | resp=%d(ms)\n",
                geofence.geofence_id, geofence.data->geofence.circle.latitude,
                geofence.data->geofence.circle.longitude, geofence.data->geofence.circle.radius_m,
                geofence.options->notification_responsivenes_ms);
        LOC_LOGW("ADD GPS GEOFENCE -> id=%2u | lat=%8.2f | lon=%8.2f | rad=%2.2f | resp=%d(ms)",
                geofence.geofence_id, geofence.data->geofence.circle.latitude,
                geofence.data->geofence.circle.longitude, geofence.data->geofence.circle.radius_m,
                geofence.options->notification_responsivenes_ms);

        gGpsGeofenceAddCallbackCalled = false;
        sGpsGeoFencingInterface->add_geofence_area(geofence.geofence_id,
                                                   geofence.data->geofence.circle.latitude,
                                       geofence.data->geofence.circle.longitude,
                                       geofence.data->geofence.circle.radius_m,
                                       GPS_GEOFENCE_UNCERTAIN,
                                       GPS_GEOFENCE_ENTERED | GPS_GEOFENCE_EXITED | GPS_GEOFENCE_UNCERTAIN,
                                       geofence.options->notification_responsivenes_ms,
                                       0);
        pthread_mutex_lock(&gMutexGeofence);
        printf("Waiting on Callback...\n");
        while (gGpsGeofenceAddCallbackCalled == false)
            pthread_cond_wait(&gCondGeofence, &gMutexGeofence);
        pthread_mutex_unlock(&gMutexGeofence);
    }

}

void removeRandomGeofence()
{
    bool isFlpGeofence = randomInt(0,1);
    GeoFencesMap& runningMap(isFlpGeofence ? sFlpGeoFencesRunning : sGpsGeoFencesRunning);
    GeoFencesMap& pausedMap(isFlpGeofence ? sFlpGeoFencesPaused : sGpsGeoFencesPaused);

    bool removeRunningGeofence = true;
    if (!runningMap.empty() && !pausedMap.empty())
    {
        removeRunningGeofence = randomInt(0,1);
    }
    else if (!runningMap.empty())
    {
        removeRunningGeofence = true;
    }
    else if (!pausedMap.empty())
    {
        removeRunningGeofence = false;
    }
    else
    {
        return;
    }

    if (removeRunningGeofence)
    {
        int index = randomInt(0, runningMap.size()-1);
        GeoFencesMap::iterator it = runningMap.begin();

        for (int i=0; i < index; ++i)
            it++;

        int32_t geofence_id = it->first;

        runningMap.erase(it);

        if (isFlpGeofence) {
            printf("REMOVE-RUNNING FLP GEOFENCE -> %5u \n", geofence_id);
            LOC_LOGW("REMOVE-RUNNING FLP GEOFENCE -> %5u ", geofence_id);
            int32_t geofence_ids[1];
            geofence_ids[0] = geofence_id;

            gFlpGeofenceRemoveCallbackCalled = false;
            sFlpGeoFencingInterface->remove_geofences(1, (int32_t*)geofence_ids);
            pthread_mutex_lock(&gMutexGeofence);
            printf("Waiting on Callback...\n");
            while (gFlpGeofenceRemoveCallbackCalled == false)
                pthread_cond_wait(&gCondGeofence, &gMutexGeofence);
            pthread_mutex_unlock(&gMutexGeofence);

        } else {
            printf("REMOVE-RUNNING GPS GEOFENCE -> %5u \n", geofence_id);
            LOC_LOGW("REMOVE-RUNNING GPS GEOFENCE -> %5u ", geofence_id);

            gGpsGeofenceRemoveCallbackCalled = false;
            sGpsGeoFencingInterface->remove_geofence_area(geofence_id);
            pthread_mutex_lock(&gMutexGeofence);
            printf("Waiting on Callback...\n");
            while (gGpsGeofenceRemoveCallbackCalled == false)
                pthread_cond_wait(&gCondGeofence, &gMutexGeofence);
            pthread_mutex_unlock(&gMutexGeofence);

        }
    }
    else
    {
        int index = randomInt(0, pausedMap.size()-1);
        GeoFencesMap::iterator it = pausedMap.begin();

        for (int i=0; i < index; ++i)
            it++;

        int32_t geofence_id = it->first;

        pausedMap.erase(it);

        if (isFlpGeofence) {
            printf("REMOVE-PAUSED FLP GEOFENCE -> %5u \n", geofence_id);
            LOC_LOGW("REMOVE-PAUSED FLP GEOFENCE -> %5u ", geofence_id);
            int32_t geofence_ids[1];
            geofence_ids[0] = geofence_id;

            gFlpGeofenceRemoveCallbackCalled = false;
            sFlpGeoFencingInterface->remove_geofences(1, (int32_t*)geofence_ids);
            pthread_mutex_lock(&gMutexGeofence);
            printf("Waiting on Callback...\n");
            while (gFlpGeofenceRemoveCallbackCalled == false)
                pthread_cond_wait(&gCondGeofence, &gMutexGeofence);
            pthread_mutex_unlock(&gMutexGeofence);

        } else {
            printf("REMOVE-PAUSED GPS GEOFENCE -> %5u \n", geofence_id);
            LOC_LOGW("REMOVE-PAUSED GPS GEOFENCE -> %5u ", geofence_id);

            gGpsGeofenceRemoveCallbackCalled = false;
            sGpsGeoFencingInterface->remove_geofence_area(geofence_id);
            pthread_mutex_lock(&gMutexGeofence);
            printf("Waiting on Callback...\n");
            while (gGpsGeofenceRemoveCallbackCalled == false)
                pthread_cond_wait(&gCondGeofence, &gMutexGeofence);
            pthread_mutex_unlock(&gMutexGeofence);
        }
    }

}

void pauseRandomGeofence()
{
    bool isFlpGeofence = randomInt(0,1);
    GeoFencesMap& runningMap(isFlpGeofence ? sFlpGeoFencesRunning : sGpsGeoFencesRunning);
    GeoFencesMap& pausedMap(isFlpGeofence ? sFlpGeoFencesPaused : sGpsGeoFencesPaused);

    if (!runningMap.empty())
    {
        int index = randomInt(0,runningMap.size()-1);
        GeoFencesMap::iterator it = runningMap.begin();

        for (int i=0; i < index; ++i)
            it++;

        int32_t geofence_id = it->first;

        pausedMap[it->first] = it->second;
        runningMap.erase(it);

        if (isFlpGeofence) {
            printf("PAUSE FLP GEOFENCE -> %5u \n", geofence_id);
            LOC_LOGW("PAUSE FLP GEOFENCE -> %5u ", geofence_id);

            gFlpGeofencePauseCallbackCalled = false;
            sFlpGeoFencingInterface->pause_geofence(geofence_id);
            pthread_mutex_lock(&gMutexGeofence);
            printf("Waiting on Callback...\n");
            while (gFlpGeofencePauseCallbackCalled == false)
                pthread_cond_wait(&gCondGeofence, &gMutexGeofence);
            pthread_mutex_unlock(&gMutexGeofence);

        } else {
            printf("PAUSE GPS GEOFENCE -> %5u \n", geofence_id);
            LOC_LOGW("PAUSE GPS GEOFENCE -> %5u ", geofence_id);

            gGpsGeofencePauseCallbackCalled = false;
            sGpsGeoFencingInterface->pause_geofence(geofence_id);
            pthread_mutex_lock(&gMutexGeofence);
            printf("Waiting on Callback...\n");
            while (gGpsGeofencePauseCallbackCalled == false)
                pthread_cond_wait(&gCondGeofence, &gMutexGeofence);
            pthread_mutex_unlock(&gMutexGeofence);
        }

    }

}

void resumeRandomGeofence()
{
    bool isFlpGeofence = randomInt(0,1);
    GeoFencesMap& runningMap(isFlpGeofence ? sFlpGeoFencesRunning : sGpsGeoFencesRunning);
    GeoFencesMap& pausedMap(isFlpGeofence ? sFlpGeoFencesPaused : sGpsGeoFencesPaused);

    if (!pausedMap.empty())
    {
        int index = randomInt(0,pausedMap.size()-1);
        GeoFencesMap::iterator it = pausedMap.begin();

        for (int i=0; i < index; ++i)
            it++;

        int32_t geofence_id = it->first;

        GeofenceOptions options;
        options.last_transition = FLP_GEOFENCE_TRANSITION_UNCERTAIN;
        options.monitor_transitions = FLP_GEOFENCE_TRANSITION_ENTERED |
                                       FLP_GEOFENCE_TRANSITION_EXITED  |
                                       FLP_GEOFENCE_TRANSITION_UNCERTAIN;

        runningMap[it->first] = it->second;
        pausedMap.erase(it);

        if (isFlpGeofence) {
            printf("RESUME FLP GEOFENCE -> %5u \n", geofence_id);
            LOC_LOGW("RESUME FLP GEOFENCE -> %5u ", geofence_id);

            gFlpGeofenceResumeCallbackCalled = false;
            sFlpGeoFencingInterface->resume_geofence(geofence_id, options.monitor_transitions);
            pthread_mutex_lock(&gMutexGeofence);
            printf("Waiting on Callback...\n");
            while (gFlpGeofenceResumeCallbackCalled == false)
                pthread_cond_wait(&gCondGeofence, &gMutexGeofence);
            pthread_mutex_unlock(&gMutexGeofence);
        } else {
            printf("RESUME GPS GEOFENCE -> %5u \n", geofence_id);
            LOC_LOGW("RESUME GPS GEOFENCE -> %5u ", geofence_id);

            gGpsGeofenceResumeCallbackCalled = false;
            sGpsGeoFencingInterface->resume_geofence(geofence_id, options.monitor_transitions);
            pthread_mutex_lock(&gMutexGeofence);
            printf("Waiting on Callback...\n");
            while (gGpsGeofenceResumeCallbackCalled == false)
                pthread_cond_wait(&gCondGeofence, &gMutexGeofence);
            pthread_mutex_unlock(&gMutexGeofence);
        }

    }

}

void modifyRandomGeofence()
{
    bool modifyRunningGeofence = true;
    if (!sFlpGeoFencesRunning.empty() && !sFlpGeoFencesPaused.empty())
    {
        modifyRunningGeofence = randomInt(0,1);
    }
    else if (!sFlpGeoFencesRunning.empty())
    {
        modifyRunningGeofence = true;
    }
    else if (!sFlpGeoFencesPaused.empty())
    {
        modifyRunningGeofence = false;
    }
    else
    {
        return;
    }

    if (modifyRunningGeofence)
    {
        int index = randomInt(0, sFlpGeoFencesRunning.size()-1);
        GeoFencesMap::iterator it = sFlpGeoFencesRunning.begin();

        for (int i=0; i < index; ++i)
            it++;

        int32_t geofence_id = it->first;
        GeofenceOptions options;
        options.last_transition = FLP_GEOFENCE_TRANSITION_UNCERTAIN;
        options.monitor_transitions = FLP_GEOFENCE_TRANSITION_ENTERED |
                                       FLP_GEOFENCE_TRANSITION_EXITED  |
                                       FLP_GEOFENCE_TRANSITION_UNCERTAIN;
        options.notification_responsivenes_ms = randomInt(1,0x40000);

        printf("MODIFY-RUNNING FLP GEOFENCE -> %5u | %d \n", geofence_id, options.notification_responsivenes_ms);
        LOC_LOGW("MODIFY-RUNNING FLP GEOFENCE -> %5u | %d ", geofence_id, options.notification_responsivenes_ms);

        sFlpGeoFencingInterface->modify_geofence_option(geofence_id, &options);
    }
    else
    {
        int index = randomInt(0, sFlpGeoFencesPaused.size()-1);
        GeoFencesMap::iterator it = sFlpGeoFencesPaused.begin();

        for (int i=0; i < index; ++i)
            it++;

        int32_t geofence_id = it->first;
        GeofenceOptions options;
        options.last_transition = FLP_GEOFENCE_TRANSITION_UNCERTAIN;
        options.monitor_transitions = FLP_GEOFENCE_TRANSITION_ENTERED |
                                       FLP_GEOFENCE_TRANSITION_EXITED  |
                                       FLP_GEOFENCE_TRANSITION_UNCERTAIN;
        options.notification_responsivenes_ms = randomInt(1,0x40000);

        printf("MODIFY-PAUSED FLP GEOFENCE -> %5u | %d \n", geofence_id, options.notification_responsivenes_ms);
        LOC_LOGW("MODIFY-PAUSED FLP GEOFENCE -> %5u | %d ", geofence_id, options.notification_responsivenes_ms);

        sFlpGeoFencingInterface->modify_geofence_option(geofence_id, &options);
    }

}

void dump()
{
    printf("GPS  | afwId | latitude  | longitude |  radius  | paused \n");
    LOC_LOGW("GPS  | afwId | latitude  | longitude |  radius  | paused ");

    for (GeoFencesMap::iterator it = sGpsGeoFencesRunning.begin(); it != sGpsGeoFencesRunning.end(); it++)
    {
        uint32_t afwId = it->first;
        GeofenceData data(it->second);
        printf("     | %5u | %8.4f  | %9.4f | %4.4f | %d \n",
               afwId, data.geofence.circle.latitude,
               data.geofence.circle.longitude, data.geofence.circle.radius_m, 0);
        LOC_LOGW("     | %5u | %8.4f  | %9.4f | %4.4f | %d ",
               afwId, data.geofence.circle.latitude,
               data.geofence.circle.longitude, data.geofence.circle.radius_m, 0);
    }

    for (GeoFencesMap::iterator it = sGpsGeoFencesPaused.begin(); it != sGpsGeoFencesPaused.end(); it++)
    {
        uint32_t afwId = it->first;
        GeofenceData data = it->second;
        printf("     | %5u | %8.4f  | %9.4f | %4.4f | %d \n",
               afwId, data.geofence.circle.latitude,
               data.geofence.circle.longitude, data.geofence.circle.radius_m, 1);
        LOC_LOGW("     | %5u | %8.4f  | %9.4f | %4.4f | %d ",
               afwId, data.geofence.circle.latitude,
               data.geofence.circle.longitude, data.geofence.circle.radius_m, 1);
    }

    printf("\nFLP  | afwId | latitude  | longitude |  radius  | paused \n");
    LOC_LOGW("FLP  | afwId | latitude  | longitude |  radius  | paused ");

    for (GeoFencesMap::iterator it = sFlpGeoFencesRunning.begin(); it != sFlpGeoFencesRunning.end(); it++)
    {
        uint32_t afwId = it->first;
        GeofenceData data(it->second);
        printf("     | %5u | %8.4f  | %9.4f | %4.4f | %d \n",
               afwId, data.geofence.circle.latitude,
               data.geofence.circle.longitude, data.geofence.circle.radius_m, 0);
        LOC_LOGW("     | %5u | %8.4f  | %9.4f | %4.4f | %d ",
               afwId, data.geofence.circle.latitude,
               data.geofence.circle.longitude, data.geofence.circle.radius_m, 0);
    }

    for (GeoFencesMap::iterator it = sFlpGeoFencesPaused.begin(); it != sFlpGeoFencesPaused.end(); it++)
    {
        uint32_t afwId = it->first;
        GeofenceData data = it->second;
        printf("     | %5u | %8.4f  | %9.4f | %4.4f | %d \n",
               afwId, data.geofence.circle.latitude,
               data.geofence.circle.longitude, data.geofence.circle.radius_m, 1);
        LOC_LOGW("     | %5u | %8.4f  | %9.4f | %4.4f | %d ",
               afwId, data.geofence.circle.latitude,
               data.geofence.circle.longitude, data.geofence.circle.radius_m, 1);
    }
}

void get_batch_size()
{
    printf("\ncalling get_batch_size...");
    int batchSize = sFlpInterface->get_batch_size();
    printf("\nget_batch_size returned %i", batchSize);
}

void start_batching()
{
    char buf[16], *p;
    FlpBatchOptions options;
    options.flags = 0;
    options.period_ns = 0;
    options.smallest_displacement_meters = 0;

    printf ("\nFLP_BATCH_WAKEUP_ON_FIFO_FULL (y/n) :");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p[0] == 'y')
        options.flags |= FLP_BATCH_WAKEUP_ON_FIFO_FULL;

    printf ("\nFLP_BATCH_CALLBACK_ON_LOCATION_FIX (y/n) :");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p[0] == 'y')
        options.flags |= FLP_BATCH_CALLBACK_ON_LOCATION_FIX;

    printf ("\nEnter period in seconds:");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    int period = atoi(p);
    printf ("\nseconds entered = %i:", period);
    options.period_ns = (int64_t)period*1000000000;

    printf ("\nEnter Smallest Displacement in meters (default 0):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atof(p) != 0)
        options.smallest_displacement_meters = atof(p);

    printf("\ncalling start_batching with flags=%i period_ns=%lld smallest_displacement=%f...",
               options.flags, options.period_ns, options.smallest_displacement_meters);
    int ret = sFlpInterface->start_batching(1, &options);
    printf("\nstart_batching returned %i", ret);
}

void update_batching()
{
    char buf[16], *p;
    FlpBatchOptions options;
    options.flags = 0;
    options.period_ns = 0;

    printf ("\nFLP_BATCH_WAKEUP_ON_FIFO_FULL (y/n) :");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p[0] == 'y')
        options.flags |= FLP_BATCH_WAKEUP_ON_FIFO_FULL;

    printf ("\nFLP_BATCH_CALLBACK_ON_LOCATION_FIX (y/n) :");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p[0] == 'y')
        options.flags |= FLP_BATCH_CALLBACK_ON_LOCATION_FIX;

    printf ("\nEnter period in seconds:");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    int period = atoi(p);
    printf ("\nseconds entered = %i:", period);
    options.period_ns = (int64_t)period*1000000000;

    printf("\ncalling update_batching with flags=%i period_ns=%lld ...",
    options.flags, options.period_ns);
    int ret = sFlpInterface->update_batching_options(1, &options);
    printf("\nstart_batching returned %i", ret);
}

void stop_batching()
{
    printf("\ncalling stop_batching...");
    int ret = sFlpInterface->stop_batching(1);
    printf("\nstop_batching returned %i", ret);
}

void get_batched_location()
{
    char buf[16], *p;
    printf ("\nEnter number of locations:");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    int n = atoi(p);
    printf("\ncalling get_batched_location..n=%i.\n", n);
    sFlpInterface->get_batched_location(n);
    printf("\nget_batch_size returned");
}

void batching_clean_up()
{
    printf("\ncalling cleanup...");
    sFlpInterface->cleanup();
    printf("\ncleanup returned");
}

#include <ctime>

void inject_location()
{
    FlpLocation location;
    location.size = sizeof(FlpLocation);
    location.flags = 31;
    location.latitude = 32.896535;
    location.longitude = -117.201025;
    location.accuracy = 50;
    location.speed = 0;
    location.bearing = 0;
    location.timestamp = std::time(0);

    printf("\ncalling inject_location time=%lld...", location.timestamp);

    sFlpInterface->inject_location(&location);
    printf("\ninject_location returned");
}

void flp_add_geofence()
{
    Geofence geofence;
    GeofenceData data;
    GeofenceOptions options;
    geofence.data = &data;
    geofence.options = &options;
    geofence.geofence_id = 1;
    geofence.data->type = TYPE_CIRCLE;
    geofence.data->geofence.circle.latitude = 32.896535;
    geofence.data->geofence.circle.longitude = -117.201025;
    geofence.data->geofence.circle.radius_m = 50;
    geofence.options->last_transition = FLP_GEOFENCE_TRANSITION_UNCERTAIN;
    geofence.options->monitor_transitions = FLP_GEOFENCE_TRANSITION_ENTERED |
                                  FLP_GEOFENCE_TRANSITION_EXITED  |
                                  FLP_GEOFENCE_TRANSITION_UNCERTAIN;
    geofence.options->notification_responsivenes_ms = 4000;

    char buf[16], *p;
    printf ("\nEnter id (default 1):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atoi(p) != 0)
        geofence.geofence_id = atoi(p);
    printf ("\nEnter latitude (default 32.896535):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atof(p) != 0)
        geofence.data->geofence.circle.latitude = atof(p);
    printf ("\nEnter longitude (default -117.201025):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atof(p) != 0)
        geofence.data->geofence.circle.longitude = atof(p);
    printf ("\nEnter radius (default 50):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atof(p) != 0)
        geofence.data->geofence.circle.radius_m = atof(p);
    printf ("\nEnter responsiveness in seconds: (default 4):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atoi(p) != 0)
        geofence.options->notification_responsivenes_ms = atoi(p)*1000;

    printf("\nFLP GEOFENCE ADD -> id=%2u | lat=%8.2f | lon=%8.2f | rad=%2.2f | resp=%d(ms)\n",
            geofence.geofence_id, geofence.data->geofence.circle.latitude,
            geofence.data->geofence.circle.longitude, geofence.data->geofence.circle.radius_m,
            geofence.options->notification_responsivenes_ms);
    LOC_LOGW("FLP GEOFENCE ADD -> id=%2u | lat=%8.2f | lon=%8.2f | rad=%2.2f | resp=%d(ms)",
            geofence.geofence_id, geofence.data->geofence.circle.latitude,
            geofence.data->geofence.circle.longitude, geofence.data->geofence.circle.radius_m,
            geofence.options->notification_responsivenes_ms);

    Geofence* geofences[1];
    geofences[0] = &geofence;
    gFlpGeofenceAddCallbackCalled = false;
    sFlpGeoFencingInterface->add_geofences(1, (Geofence**)&geofences);
    pthread_mutex_lock(&gMutexGeofence);
    printf("Waiting on Callback...\n");
    while (gFlpGeofenceAddCallbackCalled == false)
        pthread_cond_wait(&gCondGeofence, &gMutexGeofence);
    pthread_mutex_unlock(&gMutexGeofence);

}

void flp_pause_geofence()
{
    int32_t geofence_id = 1;

    char buf[16], *p;
    printf ("\nEnter id (default 1):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atoi(p) != 0)
        geofence_id = atoi(p);

    printf("\nFLP GEOFENCE PAUSE -> id=%2u \n",
            geofence_id);
    LOC_LOGW("FLP GEOFENCE PAUSE -> id=%2u ",
            geofence_id);

    gFlpGeofencePauseCallbackCalled = false;
    sFlpGeoFencingInterface->pause_geofence(geofence_id);
    pthread_mutex_lock(&gMutexGeofence);
    printf("Waiting on Callback...\n");
    while (gFlpGeofencePauseCallbackCalled == false)
        pthread_cond_wait(&gCondGeofence, &gMutexGeofence);
    pthread_mutex_unlock(&gMutexGeofence);

}

void flp_resume_geofence()
{
    int32_t geofence_id = 1;
    int monitor_transitions = FLP_GEOFENCE_TRANSITION_ENTERED |
                              FLP_GEOFENCE_TRANSITION_EXITED  |
                              FLP_GEOFENCE_TRANSITION_UNCERTAIN;
    char buf[16], *p;
    printf ("\nEnter id (default 1):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atoi(p) != 0)
        geofence_id = atoi(p);

    printf("\nFLP GEOFENCE RESUME -> id=%2u monitor_transitions=%d\n",
            geofence_id, monitor_transitions);
    LOC_LOGW("FLP GEOFENCE RESUME -> id=%2u monitor_transitions=%d",
            geofence_id, monitor_transitions);

    gFlpGeofenceResumeCallbackCalled = false;
    sFlpGeoFencingInterface->resume_geofence(geofence_id, monitor_transitions);
    pthread_mutex_lock(&gMutexGeofence);
    printf("Waiting on Callback...\n");
    while (gFlpGeofenceResumeCallbackCalled == false)
        pthread_cond_wait(&gCondGeofence, &gMutexGeofence);
    pthread_mutex_unlock(&gMutexGeofence);

}

void flp_modify_geofence()
{
    int32_t geofence_id = 1;
    GeofenceOptions options;
    options.last_transition = FLP_GEOFENCE_TRANSITION_UNCERTAIN;
    options.monitor_transitions = FLP_GEOFENCE_TRANSITION_ENTERED |
                                  FLP_GEOFENCE_TRANSITION_EXITED  |
                                  FLP_GEOFENCE_TRANSITION_UNCERTAIN;
    options.notification_responsivenes_ms = 4000;

    char buf[16], *p;
    printf ("\nEnter id (default 1):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atoi(p) != 0)
        geofence_id = atoi(p);
    printf ("\nEnter responsiveness in seconds: (default 4):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atoi(p) != 0)
        options.notification_responsivenes_ms = atoi(p)*1000;

    printf("\nFLP GEOFENCE MODIFY -> id=%2u | resp=%d(ms)\n",
            geofence_id, options.notification_responsivenes_ms);
    LOC_LOGW("FLP GEOFENCE MODIFY -> id=%2u | resp=%d(ms)",
            geofence_id, options.notification_responsivenes_ms);

    sFlpGeoFencingInterface->modify_geofence_option(geofence_id, &options);

}

void flp_remove_geofence()
{
    int32_t geofence_id = 1;

    char buf[16], *p;
    printf ("\nEnter id (default 1):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atoi(p) != 0)
        geofence_id = atoi(p);

    printf("\nFLP GEOFENCE REMOVE -> id=%2u \n",
            geofence_id);
    LOC_LOGW("FLP GEOFENCE REMOVE -> id=%2u ",
            geofence_id);

    int32_t geofence_ids[1];
    geofence_ids[0] = geofence_id;

    gFlpGeofenceRemoveCallbackCalled = false;
    sFlpGeoFencingInterface->remove_geofences(1, (int32_t*)geofence_ids);
    pthread_mutex_lock(&gMutexGeofence);
    printf("Waiting on Callback...\n");
    while (gFlpGeofenceRemoveCallbackCalled == false)
        pthread_cond_wait(&gCondGeofence, &gMutexGeofence);
    pthread_mutex_unlock(&gMutexGeofence);

}

void geofence_stress_test_2()
{
    uint32_t testCount = 50;

    char buf[16], *p;
    printf ("\nEnter number of times to add/remove 50 geofences (default 50):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atoi(p) != 0)
        testCount = atoi(p);

    while (testCount-- > 0)
    {
        for (uint32_t i=0; i < 50; ++i)
            addRandomGeofence();

        for (uint32_t i=0; i < 50; ++i)
            removeRandomGeofence();
    }

    printf("CLEARING ALL GEOFENCES\n");
    LOC_LOGW("CLEARING ALL GEOFENCES");
    sGpsGeoFencesRunning.clear();
    sGpsGeoFencesPaused.clear();
    sFlpGeoFencesRunning.clear();
    sFlpGeoFencesPaused.clear();
}

void geofence_stress_test()
{
    uint32_t testCount = 50;
    uint32_t geofencesToStartWith = 50;

    char buf[16], *p;
    printf ("\nEnter number of initial geofences to add (default 50):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atoi(p) != 0)
        geofencesToStartWith = atoi(p);
    printf ("\nEnter number of random geofences commands to call (default 50):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atoi(p) != 0)
        testCount = atoi(p);

    srand (time(NULL));

    for (uint32_t i=0; i < geofencesToStartWith; ++i)
        addRandomGeofence();

    while (testCount-- > 0)
    {
        GeoFenceCommands commandId = (GeoFenceCommands)randomInt(0,4);
        switch (commandId)
        {
            case ADD_COMMAND:
                printf("ADD_COMMAND\n");
                LOC_LOGW("ADD_COMMAND");
                addRandomGeofence();
            break;

            case PAUSE_COMMAND:
                printf("PAUSE_COMMAND\n");
                LOC_LOGW("PAUSE_COMMAND");
                pauseRandomGeofence();
            break;

            case RESUME_COMMAND:
                printf("RESUME_COMMAND\n");
                LOC_LOGW("RESUME_COMMAND");
                resumeRandomGeofence();
            break;

            case MODIFY_COMMAND:
                printf("MODIFY_COMMAND\n");
                LOC_LOGW("MODIFY_COMMAND");
                modifyRandomGeofence();
            break;

            case REMOVE_COMMAND:
                printf("REMOVE_COMMAND\n");
                LOC_LOGW("REMOVE_COMMAND");
                removeRandomGeofence();
            break;

        }

        dump();
    }

    printf("CLEARING ALL GEOFENCES\n");
    LOC_LOGW("CLEARING ALL GEOFENCES");
    sGpsGeoFencesRunning.clear();
    sGpsGeoFencesPaused.clear();
    sFlpGeoFencesRunning.clear();
    sFlpGeoFencesPaused.clear();

    printf("DONE\n");
    LOC_LOGW("DONE");
}

void gps_add_geofence()
{
    Geofence geofence;
    GeofenceData data;
    GeofenceOptions options;
    geofence.data = &data;
    geofence.options = &options;
    geofence.geofence_id = 1;
    geofence.data->type = TYPE_CIRCLE;
    geofence.data->geofence.circle.latitude = 32.896535;
    geofence.data->geofence.circle.longitude = -117.201025;
    geofence.data->geofence.circle.radius_m = 50;
    geofence.options->last_transition = GPS_GEOFENCE_UNCERTAIN;
    geofence.options->monitor_transitions = GPS_GEOFENCE_ENTERED |
                                  GPS_GEOFENCE_EXITED  |
                                  GPS_GEOFENCE_UNCERTAIN;
    geofence.options->notification_responsivenes_ms = 4000;

    char buf[16], *p;
    printf ("\nEnter id (default 1):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atoi(p) != 0)
        geofence.geofence_id = atoi(p);
    printf ("\nEnter latitude (default 32.896535):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atof(p) != 0)
        geofence.data->geofence.circle.latitude = atof(p);
    printf ("\nEnter longitude (default -117.201025):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atof(p) != 0)
        geofence.data->geofence.circle.longitude = atof(p);
    printf ("\nEnter radius (default 50):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atof(p) != 0)
        geofence.data->geofence.circle.radius_m = atof(p);
    printf ("\nEnter responsiveness in seconds: (default 4):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atoi(p) != 0)
        geofence.options->notification_responsivenes_ms = atoi(p)*1000;

    printf("\nGPS GEOFENCE ADD -> id=%2u | lat=%8.2f | lon=%8.2f | rad=%2.2f | resp=%d(ms)\n",
            geofence.geofence_id, geofence.data->geofence.circle.latitude,
            geofence.data->geofence.circle.longitude, geofence.data->geofence.circle.radius_m,
            geofence.options->notification_responsivenes_ms);
    LOC_LOGW("GPS GEOFENCE ADD -> id=%2u | lat=%8.2f | lon=%8.2f | rad=%2.2f | resp=%d(ms)",
            geofence.geofence_id, geofence.data->geofence.circle.latitude,
            geofence.data->geofence.circle.longitude, geofence.data->geofence.circle.radius_m,
            geofence.options->notification_responsivenes_ms);

    gGpsGeofenceAddCallbackCalled = false;
    sGpsGeoFencingInterface->add_geofence_area(geofence.geofence_id,
                                               geofence.data->geofence.circle.latitude,
                                               geofence.data->geofence.circle.longitude,
                                               geofence.data->geofence.circle.radius_m,
                                               geofence.options->last_transition,
                                               geofence.options->monitor_transitions,
                                               geofence.options->notification_responsivenes_ms,
                                               0);
    pthread_mutex_lock(&gMutexGeofence);
    printf("Waiting on Callback...\n");
    while (gGpsGeofenceAddCallbackCalled == false)
        pthread_cond_wait(&gCondGeofence, &gMutexGeofence);
    pthread_mutex_unlock(&gMutexGeofence);


}

void gps_pause_geofence()
{
    int32_t geofence_id = 1;

    char buf[16], *p;
    printf ("\nEnter id (default 1):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atoi(p) != 0)
        geofence_id = atoi(p);

    printf("\nGPS GEOFENCE PAUSE -> id=%2u \n",
            geofence_id);
    LOC_LOGW("GPS GEOFENCE PAUSE -> id=%2u ",
            geofence_id);

    gGpsGeofencePauseCallbackCalled = false;
    sGpsGeoFencingInterface->pause_geofence(geofence_id);
    pthread_mutex_lock(&gMutexGeofence);
    printf("Waiting on Callback...\n");
    while (gGpsGeofencePauseCallbackCalled == false)
        pthread_cond_wait(&gCondGeofence, &gMutexGeofence);
    pthread_mutex_unlock(&gMutexGeofence);
}

void gps_resume_geofence()
{
    int32_t geofence_id = 1;
    int monitor_transitions = GPS_GEOFENCE_ENTERED |
                              GPS_GEOFENCE_EXITED  |
                              GPS_GEOFENCE_UNCERTAIN;
    char buf[16], *p;
    printf ("\nEnter id (default 1):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atoi(p) != 0)
        geofence_id = atoi(p);

    printf("\nGPS GEOFENCE RESUME -> id=%2u monitor_transitions=%d\n",
            geofence_id, monitor_transitions);
    LOC_LOGW("GPS GEOFENCE RESUME -> id=%2u monitor_transitions=%d",
            geofence_id, monitor_transitions);

    gGpsGeofenceResumeCallbackCalled = false;
    sGpsGeoFencingInterface->resume_geofence(geofence_id, monitor_transitions);
    pthread_mutex_lock(&gMutexGeofence);
    printf("Waiting on Callback...\n");
    while (gGpsGeofenceResumeCallbackCalled == false)
        pthread_cond_wait(&gCondGeofence, &gMutexGeofence);
    pthread_mutex_unlock(&gMutexGeofence);

}

void gps_remove_geofence()
{
    int32_t geofence_id = 1;

    char buf[16], *p;
    printf ("\nEnter id (default 1):");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (atoi(p) != 0)
        geofence_id = atoi(p);

    printf("\nGPS GEOFENCE REMOVE -> id=%2u \n",
            geofence_id);
    LOC_LOGW("GPS GEOFENCE REMOVE -> id=%2u ",
            geofence_id);

    gGpsGeofenceRemoveCallbackCalled = false;
    sGpsGeoFencingInterface->remove_geofence_area(geofence_id);
    pthread_mutex_lock(&gMutexGeofence);
    printf("Waiting on Callback...\n");
    while (gGpsGeofenceRemoveCallbackCalled == false)
        pthread_cond_wait(&gCondGeofence, &gMutexGeofence);
    pthread_mutex_unlock(&gMutexGeofence);
}

void gps_start_tracking()
{
    printf("\nSTART TRACKING\n");
    LOC_LOGW("START TRACKING");

    if (sGpsInterface) {
        sGpsInterface->set_position_mode(GPS_POSITION_MODE_STANDALONE,
                                         GPS_POSITION_RECURRENCE_PERIODIC,
                                         1000, 89, 10000);
        sGpsInterface->start();
    }

}

void gps_stop_tracking()
{
    printf("\nSTOP TRACKING\n");
    LOC_LOGW("STOP TRACKING");

    if (sGpsInterface) {
        sGpsInterface->stop();
    }
}

void flp_batching_menu()
{
    char buf[16], *p;
    bool exit_loop = false;

    while(!exit_loop)
    {
        printf("\n\n");
        printf ("1: get batch size\n"
            "2: start batching\n"
            "3: update batching\n"
            "4: stop batching\n"
            "5: get batched location\n"
            "6: batching clean up\n"
            "7: inject location\n"
            "b: back\n"
            "q: quit\n"
            "\nEnter Command:");
        fflush (stdout);
        p = fgets (buf, 16, stdin);

        switch(p[0])
        {
        case '1':
            get_batch_size();
            break;
        case '2':
            start_batching();
            break;
        case '3':
            update_batching();
            break;
        case '4':
            stop_batching();
            break;
        case '5':
            get_batched_location();
            break;
        case '6':
            batching_clean_up();
            break;
        case '7':
            inject_location();
            break;
        case 'b':
            exit_loop = true;
            break;
        case 'q':
            exit(0);
            break;
        default:
            printf("\ninvalid command\n");
        }
    }

}

void flp_geofence_menu()
{
    char buf[16], *p;
    bool exit_loop = false;

    while(!exit_loop)
    {
        printf("\n\n");
        printf ("1: add_geofence\n"
            "2: pause_geofence\n"
            "3: resume geofence\n"
            "4: modify geofence\n"
            "5: remove geofence\n"
            "b: back\n"
            "q: quit\n"
            "\nEnter Command:");
        fflush (stdout);
        p = fgets (buf, 16, stdin);

        switch(p[0])
        {
        case '1':
            flp_add_geofence();
            break;
        case '2':
            flp_pause_geofence();
            break;
        case '3':
            flp_resume_geofence();
            break;
        case '4':
            flp_modify_geofence();
            break;
        case '5':
            flp_remove_geofence();
            break;
        case 'b':
            exit_loop = true;
            break;
        case 'q':
            exit(0);
        default:
            printf("\ninvalid command\n");
        }
    }

}

void gps_positioning_menu()
{
    char buf[16], *p;
    bool exit_loop = false;

    while(!exit_loop)
    {
        printf("\n\n");
        printf ("1: start tracking\n"
            "2: stop tracking\n"
            "b: back\n"
            "q: quit\n"
            "\nEnter Command:");
        fflush (stdout);
        p = fgets (buf, 16, stdin);

        switch(p[0])
        {
        case '1':
            gps_start_tracking();
            break;
        case '2':
            gps_stop_tracking();
            break;
        case 'b':
            exit_loop = true;
            break;
        case 'q':
            exit(0);
            break;
        default:
            printf("\ninvalid command\n");
        }
    }

}

void gps_geofence_menu()
{
    char buf[16], *p;
    bool exit_loop = false;

    while(!exit_loop)
    {
        printf("\n\n");
        printf ("1: add_geofence\n"
            "2: pause_geofence\n"
            "3: resume geofence\n"
            "4: remove geofence\n"
            "b: back\n"
            "q: quit\n"
            "\nEnter Command:");
        fflush (stdout);
        p = fgets (buf, 16, stdin);

        switch(p[0])
        {
        case '1':
            gps_add_geofence();
            break;
        case '2':
            gps_pause_geofence();
            break;
        case '3':
            gps_resume_geofence();
            break;
        case '4':
            gps_remove_geofence();
            break;
        case 'b':
            exit_loop = true;
            break;
        case 'q':
            exit(0);
        default:
            printf("\ninvalid command\n");
        }
    }

}

void gps_verify_test_menu()
{
    char buf[16], *p;
    gStopOnMinSvs = 0;
    gStopOnMinSnr = 0.0;
    gGpsVerifyTestInProgress = true;

    printf ("\nComplete test with SVs SNR instead of position? (y/n) (default n) :");
    fflush (stdout);
    p = fgets (buf, 16, stdin);
    if (p[0] == 'y')
    {
        printf ("\nEnter minimum SVs needed to complete test? (default 1):");
        fflush (stdout);
        p = fgets (buf, 16, stdin);
        gStopOnMinSvs = 1;
        if (atoi(p) != 0)
            gStopOnMinSvs = atoi(p);
        printf ("\nEnter minimum SNR for SVs to complete test (default 28):");
        fflush (stdout);
        p = fgets (buf, 16, stdin);
        gStopOnMinSnr = 28;
        if (atof(p) != 0)
            gStopOnMinSnr = atof(p);
    }

}

void gps_verify_test()
{
    printf("GPS VERIFY TEST START...\n");
    LOC_LOGW("GPS VERIFY TEST START");
    printf("\ncalling gps_start_tracking with StopOnMinSvs=%i StopOnMinSnr=%f...\n",
              gStopOnMinSvs, gStopOnMinSnr);
    gps_start_tracking();

    pthread_mutex_lock(&gMutexLocation);
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 300;
    int ret = pthread_cond_timedwait(&gCondLocation, &gMutexLocation, &ts);
    pthread_mutex_unlock(&gMutexLocation);
    if (ret == ETIMEDOUT) {
        printf("GPS VERIFY TEST FAILED...Timeout\n");
        LOC_LOGW("GPS VERIFY TEST FAILED...Timeout");
    }
    gps_stop_tracking();
}

int main (int argc, char *argv[])
{
     if (argc <= 1) {
         printf("Optional command line arguments to start gps verify test without menu:\n");
         printf("    -T:  1 to start gps verify test (ex: location_hal_test -T 1)\n");
         printf("    -A:  Minimum number of SVs seen in combination with -B option\n");
         printf("         to determine when to stop the test without actually getting\n");
         printf("         a position report to save test time\n");
         printf("    -B:  Minimum SNR for each SV seen in -A option to determine\n");
         printf("         when to stop the test  without actually getting a position\n");
         printf("         report to save test time\n\n");
    }
    int result = 0;
    int opt;
    extern char *optarg;

    while ((opt = getopt (argc, argv, "t:T:a:A:b:B:")) != -1) {
        switch (opt) {
        case 't':
        case 'T':
            gGpsVerifyTestInProgress = (atoi(optarg) == 1);
            printf("GPS Verify Test:%d\n", gGpsVerifyTestInProgress);
            break;
        case 'a':
        case 'A':
            gStopOnMinSvs = atoi(optarg);
            printf("Stop on Minimum Svs: %d\n",gStopOnMinSvs);
            break;
        case 'b':
        case 'B':
            gStopOnMinSnr = atof(optarg);
            printf("Stop on Minimum SNR: %f\n",gStopOnMinSnr);
            break;
        }
    }

    bool success = init();
    if (!success) {
        printf("init failed!\n");
        LOC_LOGW("init failed!");
        return 1;
    }

    if (gGpsVerifyTestInProgress)
    {
        gps_verify_test();
        return 0;
    }

    char buf[16], *p;
    bool exit_loop = false;

    while(!exit_loop)
    {
        usleep(100000);
        printf("\n\n"
                "1: GPS Verify Test\n"
                "2: GPS Positioning\n"
                "3: GPS Geofence\n"
                "4: FLP Batching\n"
                "5: FLP Geofence\n"
                "6: Geofence Stress Test 1\n"
                "7: Geofence Stress Test 2\n"
                "q: quit\n"
                "\nEnter Command:");
        fflush (stdout);
        p = fgets (buf, 16, stdin);

        switch(p[0])
        {
        case '1':
            gps_verify_test_menu();
            gps_verify_test();
            break;
        case '2':
            gps_positioning_menu();
            break;
        case '3':
            gps_geofence_menu();
            break;
        case '4':
            flp_batching_menu();
            break;
        case '5':
            flp_geofence_menu();
            break;
        case '6':
            geofence_stress_test();
            break;
        case '7':
            geofence_stress_test_2();
            break;
        case 'q':
            exit_loop = true;
            break;
        default:
            printf("\ninvalid command\n");
        }
    }


}
