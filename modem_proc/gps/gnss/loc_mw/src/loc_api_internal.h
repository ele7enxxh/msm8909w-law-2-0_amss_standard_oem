/******************************************************************************
  @file:  loc_api_internal.h
  @brief: Location Middleware internal API header

  DESCRIPTION
      Declares the internal data types and macros
      used by loc middleware

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2010 - 2015 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014, 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  -----------------------------------------------------------------------------
 ******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ------------------------------------------------------- 
01/11/16   yh    Loc_Wifi cleanup
10/21/15   sudj  Fix Crash caused by bad AON configuration
09/30/15   sj    Added hysterisis for unsolicited status indications 
06/11/15   sj    LB 2.0 integration
04/02/15   ssu   Batch Dwell notification for geofence.
03/02/15   sj    Use replaceGf() API from GM instead of Del/Add seq  
02/08/13   ssu   Move to the DOG Heartbeat mechanism on DIME and beyond   
04/07/10   ns    Added loc_mw_task_started flag to denote that loc mw
                 task has started

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_api_internal.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 07:59:45 $
======================================================================*/

#ifndef FEATURE_LOC_API_INTERNAL_H
#define FEATURE_LOC_API_INTERNAL_H

#include "gps_variation.h"
#include "loc_api_2.h"
#include "pdapi.h"
#include "loc_task.h"
#include "loc_conn.h"
#include "loc_xtra.h"
#include "loc_pd.h"
#include "loc_pa.h"
#include "loc_ni.h"
#include "loc_wifi.h"
#include "loc_client.h"
#include "msg.h"
#include "aries_os_globals.h"
#include "aries_os_api.h"
#include "loc_gdt.h"

/*Include to get the data-type definitions for QMI-LOC */
#include "location_service_v02.h"

#include "slim_task.h"

#define LOC_HANDLE_REGISTERED_CLIENTS (-2)

#define LOC_MSG_LOW( str, a, b, c )       MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_LOW, str, a, b, c )
#define LOC_MSG_MED( str, a, b, c )       MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_MED, str, a, b, c )
#define LOC_MSG_HIGH( str, a, b, c )      MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_HIGH, str, a, b, c )
#define LOC_MSG_ERROR( str, a, b, c )     MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR, str, a, b, c )
#define LOC_ERR_FATAL( str, a, b, c )     MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_FATAL, str, a, b, c )

// MACRO Initialize Critical Section
#define LOC_MW_INIT_CRIT_SECTION( mutex_ctrl_block )                     \
  { os_MutexInit( mutex_ctrl_block, MUTEX_DATA_ONLY_CONTEXT ); }

// MACRO Enter Critical Section
#define LOC_MW_ENTER_CRIT_SECTION( mutex_ctrl_block )                    \
  { os_MutexLock( mutex_ctrl_block ); }

// MACRO Leave Critical Section
#define LOC_MW_LEAVE_CRIT_SECTION( mutex_ctrl_block )                    \
  { os_MutexUnlock( mutex_ctrl_block ); }

typedef struct
{
   // Module initialization is done when the first client registers for the service
   volatile boolean         loc_mw_task_started;
   boolean                  qmi_loc_enabled;
   boolean                  pd_module_initialized;
   boolean                  nmea_module_initialized;
   boolean                  pa_module_initialized;
   boolean                  conn_module_initialized;
   boolean                  xtra_module_initialized;
   boolean                  ni_module_initialized;
   boolean                  geofence_module_initialized;
   boolean                  aon_module_initialized;
   boolean                  v_nv_module_initialized;
   boolean                  nmea_module_needed;    // Whether NMEA module is needed or not

   pdsm_client_id_type      pdapi_client_id;       // Used for pdapi events
   pdsm_client_id_type      xtra_client_id;  // Used for xtra events
   pdsm_client_id_type      ni_client_id;    // Used for ni events

   loc_client_module_data_s_type   client_module_data;
   loc_pd_module_data_s_type       pd_module_data;
   loc_xtra_module_data_s_type     xtra_module_data;
   loc_conn_module_data_s_type     conn_module_data;
   loc_pa_module_data_s_type       pa_module_data;
   loc_ni_module_data_s_type       ni_module_data;
   os_MutexBlockType	           crit_sec; // critical section for the module
   os_MutexBlockType               pd_crit_sec; // critical section for position module
   boolean                         wifi_module_initialized;
   locWifi_ModuleDataType          z_locWifi_Data;
   pdsm_client_id_type             wifi_client_id;
   os_MutexBlockType               wifi_crit_sec; // critical section for wifi module
} loc_middleware_data_s_type;

extern loc_middleware_data_s_type loc_middleware_data;

typedef enum
{
  LM_MIDDLEWARE_MSG_ID_PDAPI = C_USR_MSG_ID_FIRST,
  LM_MIDDLEWARE_MSG_ID_QMI_SHIM,

  LM_MIDDLEWARE_MSG_ID_SDP_SET_SPI_STATUS_IND =             6000,
  LM_MIDDLEWARE_MSG_ID_SDP_INJECT_SENSOR_DATA_IND =         6001,
  LM_MIDDLEWARE_MSG_ID_SDP_INJECT_TIME_SYNC_DATA_IND =      6002,
  LM_MIDDLEWARE_MSG_ID_SDP_GET_CRADLE_MOUNT_CONFIG_IND =    6003,
  LM_MIDDLEWARE_MSG_ID_SDP_GET_EXTERNAL_POWER_CONFIG_IND =  6004,
  LM_MIDDLEWARE_MSG_ID_SDP_SET_CRADLE_MOUNT_CONFIG_IND =    6005,
  LM_MIDDLEWARE_MSG_ID_SDP_SET_EXTERNAL_POWER_CONFIG_IND =  6006,
  LM_MIDDLEWARE_MSG_ID_SDP_EVENT_SENSOR_STREAMING_READINESS_IND =   6007,
  LM_MIDDLEWARE_MSG_ID_SDP_EVENT_TIME_SYNC_NEEDED_IND =     6008,
  LM_MIDDLEWARE_MSG_ID_SDP_EVENT_SWITCH_SPI_STREAMING_REPORT_IND =  6009,
  LM_MIDDLEWARE_MSG_ID_SET_SENSOR_CONTROL_CONFIG_IND                     =  6010,
  LM_MIDDLEWARE_MSG_ID_GET_SENSOR_CONTROL_CONFIG_IND                     =  6011,
  LM_MIDDLEWARE_MSG_ID_SET_SENSOR_PROPERTIES_IND                         =  6012,
  LM_MIDDLEWARE_MSG_ID_GET_SENSOR_PROPERTIES_IND                         =  6013,
  LM_MIDDLEWARE_MSG_ID_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_IND  =  6014,
  LM_MIDDLEWARE_MSG_ID_GET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_IND  =  6015,
  LM_MIDDLEWARE_MSG_ID_GEOFENCE_NI_IND     =                6016,
  LM_MIDDLEWARE_MSG_ID_GEOFENCE_GEN_ALERT_IND  =            6017,
  LM_MIDDLEWARE_MSG_ID_GEOFENCE_BREACH_IND =                6018,
  LM_MIDDLEWARE_MSG_ID_GEOFENCE_ADD_IND =                   6019,
  LM_MIDDLEWARE_MSG_ID_GEOFENCE_DELETE_IND =                6020,
  LM_MIDDLEWARE_MSG_ID_GEOFENCE_QUERY_IND  =                6021,
  LM_MIDDLEWARE_MSG_ID_GEOFENCE_EDIT_IND   =                6022,
  LM_MIDDLEWARE_MSG_ID_EVENT_PEDOMETER_CONTROL_IND  =       6023,
  LM_MIDDLEWARE_MSG_ID_EVENT_MOTION_DATA_CONTROL_IND  =     6024,
  LM_MIDDLEWARE_MSG_ID_PEDOMTER_REPORT_IND            =     6025,
  LM_MIDDLEWARE_MSG_ID_INJECT_MOTION_DATA_IND         =     6026,
  LM_MIDDLEWARE_MSG_ID_GEOFENCE_SET_ENGINE_CONFIG_IND =     6027,
  LM_MIDDLEWARE_MSG_ID_SDP_EVENT_VEHICLE_SENSOR_INJECTION_READINESS_IND = 6028,
  LM_MIDDLEWARE_MSG_ID_SDP_INJECT_VEHICLE_SENSOR_DATA_IND         =     6029,
  LM_MIDDLEWARE_MSG_ID_GEOFENCE_BATCH_BREACH_IND      =     6030,
  LM_MIDDLEWARE_MSG_ID_PD_EVENT_WIFI_AP_SCAN_INJECT_REQ_IND              =  6031,
  LM_MIDDLEWARE_MSG_ID_PD_INJECT_WIFI_AP_SCAN_IND     =     6032,
  LM_MIDDLEWARE_MSG_ID_GEOFENCE_PROXIMITY_IND         =     6033,
  LM_MIDDLEWARE_MSG_ID_GTP_WWAN_CLIENT_DLD_DATA_IND   =     6034,
  LM_MIDDLEWARE_MSG_ID_GDT_BEGIN_IND                  =     6035, /* MP sending indication to start a session */
  LM_MIDDLEWARE_MSG_ID_GDT_END_IND                    =     6036, /* MP sending indication to end a session */
  LM_MIDDLEWARE_MSG_ID_GDT_BEGIN_REQ_IND              =     6037, /* Ind resp. to the request received from AP */
  LM_MIDDLEWARE_MSG_ID_GDT_END_REQ_IND                =     6038, /* Ind resp. to therequest received from AP */
  LM_MIDDLEWARE_MSG_ID_WLE_APCACHE_REQ_IND            =     6039, /* Ind resp. to APCACHE request received from AP */
  LM_MIDDLEWARE_MSG_ID_WLE_NOAPCACHE_REQ_IND          =     6040, /* Ind resp. to NOAPCACHE request received from AP */
  LM_MIDDLEWARE_MSG_ID_GEOFENCE_BATCH_DWELL_IND       =     6041,
  LM_MIDDLEWARE_MSG_ID_TZ_REQ_IND                     =     6042,
  /* SLIM message ids reserve range 9000-9999 */
  LM_MIDDLEWARE_MSG_ID_SLIM_RESERVERD_MIN = IPC_SLIM_TO_TASK_MIN,
  LM_MIDDLEWARE_MSG_ID_SLIM_RESERVERD_MAX = IPC_SLIM_TO_TASK_MAX,

  LM_MIDDLEWARE_MSG_ID_QMI_LOC_WIFI_IND               =     6100,

  LM_MIDDLEWARE_MSG_ID_AON_MIN                           =     7000,
/*  LM_MIDDLEWARE_MSG_ID_GM_ADD_RESPONSE                 =     7000,
  LM_MIDDLEWARE_MSG_ID_GM_EDIT_NOTIFY                    =     7001,
  LM_MIDDLEWARE_MSG_ID_GM_DELETE_RESPONSE                =     7002,
  LM_MIDDLEWARE_MSG_ID_GM_PURGE_RESPONSE                 =     7003,
  LM_MIDDLEWARE_MSG_ID_GM_BREACH_NOTIFY                  =     7004,
  LM_MIDDLEWARE_MSG_ID_GM_POS_REQ                        =     7005,
  LM_MIDDLEWARE_MSG_ID_GM_CLIENT_BLOB_NOTIFY             =     7006,
  LM_MIDDLEWARE_MSG_ID_GM_CLIENT_GEOFENCE_HANDELS_NOTIFY =     7007,
  LM_MIDDLEWARE_MSG_ID_GM_GEOFENCE_HANDELS_NOTIFY        =     7008,
  LM_MIDDLEWARE_MSG_ID_GM_QUERY_NOTIFY                   =     7009,
  LM_MIDDLEWARE_MSG_ID_GM_UNSOL_EVENT_NOTIFY             =     7010,
  LM_MIDDLEWARE_MSG_ID_GM_PRIO_POS_REQ_INDICATION_NOTIFY =     7011,
  LM_MIDDLEWARE_MSG_ID_GM_SET_ENGINE_CONFIG_NOTIFY       =     7012,
  LM_MIDDLEWARE_MSG_ID_GM_ABORT_POS_REQ                  =     7013,
  LM_MIDDLEWARE_MSG_ID_GM_BATCH_BREACH_NOTIFY            =     7014,
  LM_MIDDLEWARE_MSG_ID_GM_PROXIMITY_NOTIFY               =     7015,
  LM_MIDDLEWARE_MSG_ID_GM_REPLACE_NOTIFY                 =     7016 */
  LM_MIDDLEWARE_MSG_ID_AON_MAX                           =     7016,

  LM_MIDDLEWARE_MSG_ID_AON_STATUS_CB                     =     7200,
  LM_MIDDLEWARE_MSG_ID_AON_LOCATION_CB                   =     7201,
  

} loc_middleware_msg_id_e_type;

#define LOC_MIDDLEWARE_TIMER_ID_FIX  (2)
#define LOC_MIDDLEWARE_TIMER_ID_XTRA (3)
#define LOC_MIDDLEWARE_TIMER_ID_PA   (4)
#define LOC_MIDDLEWARE_TIMER_ID_SLIM_RESERVED_MIN IPC_SLIM_TIMER_ID_MIN
#define LOC_MIDDLEWARE_TIMER_ID_SLIM_RESERVED_MAX IPC_SLIM_TIMER_ID_MAX

#define LOC_MIDDLEWARE_TIMER_ID_RESERVED_AON_MIN (10000)
#define LOC_MIDDLEWARE_TIMER_ID_RESERVED_AON_HYSTERISIS (10999)
#define LOC_MIDDLEWARE_TIMER_ID_RESERVED_AON_MAX (LOC_MIDDLEWARE_TIMER_ID_RESERVED_AON_HYSTERISIS)

#endif // FEATURE_LOC_API_INTERNAL_H
