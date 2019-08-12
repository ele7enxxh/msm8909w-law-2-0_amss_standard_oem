/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemId

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_DATAITEMID_H__
#define __IZAT_MANAGER_DATAITEMID_H__

namespace izat_manager {

/**
 * Enumeration of Data Item types
 * When add/remove/update changes are made to Data Items, this file needs to be updated
 * accordingly
 */
typedef enum e_DataItemId {
    INVALID_DATA_ITEM_ID = -1,
    // 0 - 4
    AIRPLANEMODE_DATA_ITEM_ID,
    ENH_DATA_ITEM_ID,
    GPSSTATE_DATA_ITEM_ID,
    NLPSTATUS_DATA_ITEM_ID,
    WIFIHARDWARESTATE_DATA_ITEM_ID,
    // 5 - 9
    NETWORKINFO_DATA_ITEM_ID,
    RILVERSION_DATA_ITEM_ID,
    RILSERVICEINFO_DATA_ITEM_ID,
    RILCELLINFO_DATA_ITEM_ID,
    SERVICESTATUS_DATA_ITEM_ID,
    // 10 - 14
    MODEL_DATA_ITEM_ID,
    MANUFACTURER_DATA_ITEM_ID,
    VOICECALL_DATA_ITEM,
    ASSISTED_GPS_DATA_ITEM_ID,
    SCREEN_STATE_DATA_ITEM_ID,
    // 15 - 19
    POWER_CONNECTED_STATE_DATA_ITEM_ID,
    TIMEZONE_CHANGE_DATA_ITEM_ID,
    TIME_CHANGE_DATA_ITEM_ID,
    WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID,
    SHUTDOWN_STATE_DATA_ITEM_ID,
    // 20 -
    TAC_DATA_ITEM_ID,
    MCCMNC_DATA_ITEM_ID,
    MAX_DATA_ITEM_ID
} DataItemId;

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_DATAITEMID_H__
