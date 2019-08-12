/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef FLP_LOCATION_CLIENT_API_H
#define FLP_LOCATION_CLIENT_API_H

#define FLP_CONF_CAPABILITY_GEOFENCE 0x1
#define FLP_CONF_CAPABILITY_BATCHING 0x2

#include <LocDualContext.h>
#include <log_util.h>
#include <loc_cfg.h>
#include <fused_location_extended.h>

class FlpLocationAdapter;
#define FLP_SUCCESS                    0
#define FLP_ERROR                     -1

typedef struct {
    flp_ext_location_callback location_cb;
    flp_ext_acquire_wakelock acquire_wakelock_cb;
    flp_ext_release_wakelock release_wakelock_cb;
    flp_ext_max_pw_allocated_callback max_pw_allocated_cb;
} FlpClientCallbacks;

class FlpLocationClient {

    FlpClientCallbacks mFlpClientCb;
    FlpLocationAdapter* mFlpAdapter;
    bool mGetBatchSizeCalled;

    static bool sFlpConfHasBeenRead;
    static int sBatchSize;
    static int sBatchTimeout;
    static int sAccuracy;
    static int sAllowLB_1_0;
    static unsigned long sCapabilities;

    FlpLocationClient();
    inline ~FlpLocationClient() {}
    void adjust_flp_request(FlpExtBatchOptions* options);

public:

    static FlpLocationClient *createInstance();

    int flp_init(FlpExtCallbacks* callbacks);
    int flp_get_batch_size();
    int flp_get_all_supported_features();
    int flp_start_session(int id, FlpExtBatchOptions* options);
    int flp_update_session(int id, FlpExtBatchOptions* options);
    int flp_stop_session(int id);
    void flp_cleanup();
    void flp_get_batched_location(int last_n_locations);
    void flp_pull_all_locations();
    int flp_inject_location(FlpExtLocation* location);
    unsigned long getCapabilities();
    int delete_aiding_data(GnssAidingData flags);
    double flp_get_max_power_allocated_in_mW();
};

#endif /* FLP_LOCATION_CLIENT_API_H */
