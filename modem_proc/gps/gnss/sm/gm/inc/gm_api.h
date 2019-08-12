/*======================================================================

               Geofence Manager common declarations and API header file

 GENERAL DESCRIPTION
  This file contains API definitions by which Geofencing services can
  be accessed by clients (e.g Transport Manager).

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2015-2016 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header:

 when      who   what, where, why
 ----      ----  --- --------------------------------------------------- 
1/18/16    ss       Supporting back Max Pos Unc in GF config 
06/11/15   sjk   Adding changes for Forced responsiveness, configuring session timeout/client, accuracy of location used/gf & unable to track ind
05/19/15   ssu     Adding GM in memory logging.
04/10/15   ssu   Adding support for geofence dwell time.
03/02/15   ss     API changes for LPW integration.
11/13/14   ssu     GM LOWI Integration
11/12/14   sj     Make changes for GF Config items.
10/15/14   sj    Adding Custom Responsiveness
04/22/14   ssu  Adding WCMDA/LTE cell meas API
04/22/14   ssu   Adding WCMDA/LTE cell meas API
04/22/14   ssu  Adding WCMDA/LTE cell meas API
04/03/14   ssu   Additional Fix details parameters for Geofence Breach reports
03/24/12   ss    Supporting operation mode settting to effect Geofencing operation mode
02/21/14   ssu   Adding NV for configuring motion sensing distance in case of challenging GPS env.
02/20/14   ssu   Adding a NV for modifying the medium responsiveness backoff value and gps challenging env backoff config.
02/10/14   ssu   Adding a NV for modifying the rate at which GM requests CPI injection.
02/06/14   ss    Supporting Medium Confidence responsivenes value config
01/10/14   ss    Supporting High Confidence Geofence config
 01/28/13  mj    Modified gm_register_task_init_notifier to add new param Client thread id
04/09/11   ss    Initial creation of file.
======================================================================*/

#ifndef GM_API_H
#define GM_API_H

#include "comdef.h"
#include "customer.h"
#include "gps_common.h"
#include "sm_api.h"
#include "tm_common.h"
#include "tm_api.h"

#ifdef FEATURE_GEOFENCE_SENSOR_SUPPORT
#include "loc_sensor_amd.h"
#include "loc_sensor_rmd.h"
#include "loc_sensor_ped.h"
#include "loc_sensor_cmc_db.h"
#include "loc_sensor_smd.h"
#endif
#include "loc_sensor_iod.h"

#include "tm_lm_iface.h"
#include "tm_prtl_iface.h"

#include "wl_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************** MACROS ****************************************************/
#define GEOFENCE_MAX_AREA_ID_LIST_MAX 256 * 32

#define GEOFENCE_MAX_CLIENT_BLOB_SIZE 300

/*Return codes*/
#define GEOFENCE_ERROR_NONE                      0
#define GEOFENCE_ERROR_DUPLICATE_CLIENT_REGISTER 1 /*Duplicate Registeration*/
#define GEOFENCE_ERROR_MAX_CLIENT_REGISTER       2 /*Maximum clients are already registered*/
#define GEOFENCE_ERROR_INVALID_CLIENT_HANDLE     3 /*Invalid client handle*/
#define GEOFENCE_ERROR_IPC_SEND_FAIL             4 /*Error during sending IPC*/
#define GEOFENCE_ERROR_MAX_GEOFENCE_PROGRAMED    5 /*Maximum geofences are already programmed*/
#define GEOFENCE_ERROR_GEOFENCE_NOT_FOUND        6 /*Geofence is not programmed*/
#define GEOFENCE_ERROR_GEOFENCE_WRONG_CLIENT     7 /*Geofence does not belong to this client*/
#define GEOFENCE_ERROR_INCORRECT_PARAMS          8 /*Params check failed*/

#define GEOFENCE_DELETE_ON_CLIENT_REQUEST        9 /*Geofence deleted on client request*/
#define GEOFENCE_DELETE_ON_STOP_TIME             10 /*Geofence deleted on meeting stop time*/
#define GEOFENCE_DELETE_ON_MAX_REPORTS           11 /*Geofence deleted on reporting max reports*/
#define GEOFENCE_DELETE_ON_REPLACE               12 /*Geofence deleted on replace*/
#define GEOFENCE_DELETE_ON_REPLACE_FAIL          13 /*Geofence deleted on replace*/


#define GEOFENCE_ERROR_MEMORY_ALLOC_FAIL         14 /*Memory allocation failed*/
#define GEOFENCE_ERROR_GEOFENCE_ID_ALLOCATION_FAILED         15 /*Memory allocation failed*/
#define GEOFENCE_ERROR_NO_GF_CLIENT_CAP          16 /*Geofence client capability does not allow this operation*/
#define GEOFENCE_ERROR_UNSPECIFIED               60 /*Unspecified error*/
/*Notification types*/
#define GEOFENCE_ADD_NOTIFY                      0 /*Client notification for Add request*/
/*GEOFENCE_ADD_NOTIFY uses sm_gm_notificationt_type structure*/
#define GEOFENCE_EDIT_NOTIFY                     1 /*Client notification for Edit request*/
/*GEOFENCE_EDIT_NOTIFY uses sm_gm_notificationt_type structure*/
#define GEOFENCE_DELETE_NOTIFY                   2 /*Client notification for Delete request*/
/*GEOFENCE_DELETE_NOTIFY uses sm_gm_notificationt_type structure*/
#define GEOFENCE_PURGE_NOTIFY                    3 /*Client notification for Purge request*/
/*GEOFENCE_PURGE_NOTIFY uses sm_gm_notificationt_type structure*/
#define GEOFENCE_BREACH_NOTIFY                   4 /*Client notification for Geofence Breach*/
/*GEOFENCE_BREACH_NOTIFY uses sm_gm_notificationt_type structure*/
#define GEOFENCE_POS_REQUEST_NOTIFY              5 /*Client notification for starting pos request*/
/*GEOFENCE_POS_REQUEST_NOTIFY uses sm_gm_client_pos_req_type structure*/
#define GEOFENCE_CLIENT_BLOB_NOTIFY              6 /*Client notification for Get Geofence Client Blob request*/
                                                   /*Client Blob is an opaque data stored with the geofence in behalf
                                                     of the client. Client can use this blob to store context information.*/
/*GEOFENCE_CLIENT_BLOB_NOTIFY uses sm_gm_geofence_client_blob_notify_type structure*/
#define GEOFENCE_CLIENT_GEOFENCE_HANDELS_NOTIFY  7 /*Client notification for Get Client Geofence handles request*/
/*GEOFENCE_CLIENT_GEOFENCE_HANDLES_NOTIFY uses sm_gm_client_geofence_handels_notification_type structure*/
#define GEOFENCE_HANDELS_NOTIFY                  8 /*Client notification for Get Geofence handles request*/
/*GEOFENCE_HANDLES_NOTIFY uses sm_gm_geofence_handels_notification_type structure*/
#define GEOFENCE_STATUS_NOTIFY                   9 /*Client notification for Query Geofence status request*/
/*GEOFENCE_STATUS_NOTIFY uses sm_gm_geofence_status_notification_type structure*/
#define GEOFENCE_UNSOL_EVENT_NOTIFY              10 /*Client notification for Unsolicited events*/
/*GEOFENCE_STATUS_NOTIFY uses sm_gm_geofence_status_notification_type structure*/
#define GEOFENCE_PRIOR_POS_REQ_IND_NOTIFY              11 /*Client notification for Prior Pos Requesr Indication*/
/*GEOFENCE_PRIOR_POS_REQ_IND_NOTIFY uses sm_gm_geofence_status_notification_type structure*/

#define GEOFENCE_SET_ENGINE_CONFIG_NOTIFY              12 /*Client notification for Prior Pos Requesr Indication*/
/*GEOFENCE_SET_ENGINE_CONFIG_NOTIFY uses sm_gm_set_geofence_engine_config_request_type structure*/
#define GEOFENCE_POS_ABORT_NOTIFY    13 /*Client notification for starting pos abort*/

#define GEOFENCE_BATCH_BREACH_NOTIFY                   14 /*Client notification for Geofence Batch Breach*/

#define GEOFENCE_PROXIMITY_NOTIFY                     15 /*Client notification for Geofence Proximity*/

#define GEOFENCE_REPLACE_NOTIFY                       16 /*Client notification for Add request*/

#define GEOFENCE_BATCH_DWELL_NOTIFY                   17 /*Client notification for Geofence Dwell*/

#define GEOFENCE_TZ_REQ_NOTIFY                        18 /*Client notification for TimeZone injection*/

#define GEOFENCE_SET_CLIENT_CONFIG_NOTIFY             19 /*Client notification for Prior Client Config*/
/*GEOFENCE_SET_CLIENT_CONFIG_NOTIFY uses sm_gm_set_client_config_request_type structure*/



/*MAX MCC/MNC/LAC/CID values*/
#define GEOFENCE_MAX_MCC 999
#define GEOFENCE_MAX_MNC 999
#define GEOFENCE_MAX_LAC 65535
#define GEOFENCE_MAX_GSM_CID 65535
#define GEOFENCE_MAX_WCDMA_CID 268435455
#define GEOFENCE_MAX_LTE_CID 268435455

/*Macors to be used when eitehr MCC/MNC/LAC/CID is not given*/
#define GEOFENCE_NO_MCC  (GEOFENCE_MAX_MCC + 1)
#define GEOFENCE_NO_MNC  (GEOFENCE_MAX_MNC + 1)
#define GEOFENCE_NO_LAC  (GEOFENCE_MAX_LAC)
#define GEOFENCE_NO_GSM_CID (GEOFENCE_MAX_GSM_CID + 1)
#define GEOFENCE_NO_WCDMA_CID (GEOFENCE_MAX_WCDMA_CID + 1)
#define GEOFENCE_NO_LTE_CID (GEOFENCE_MAX_LTE_CID + 1)


/*Area-id RAT type*/
#define GEOFENCE_AREA_ID_RAT_NONE_TYPE 0x0  /*RAT None*/
#define GEOFENCE_AREA_ID_RAT_GSM_TYPE 0x1  /*GSM Area id type*/
#define GEOFENCE_AREA_ID_RAT_WCDMA_TYPE 0x2 /*WCDMA Area id type*/
#define GEOFENCE_AREA_ID_RAT_LTE_TYPE 0x4 /*LTE Area id type*/
#define GEOFENCE_AREA_ID_RAT_TDSCDMA_TYPE 0x8 /*TD-SCDMA Area id type*/
#define GEOFENCE_AREA_ID_RAT_WIFI_TYPE 0x8 /*Wifi Area id type*/
#define GEOFENCE_AREA_ID_RAT_UNKNOWN_TYPE 0xFF /*Unknown Area-id RAT type*/

/*Area-id Type*/
#define GEOFENCE_AREA_ID_BORDER_TYPE 0  /*Border Area Id*/
#define GEOFENCE_AREA_ID_WITHIN_TYPE 1 /*Within Area id*/
#define GEOFENCE_AREA_ID_UNKNOWN_TYPE 2 /*Unknown type*/
#define GEOFENCE_AREA_ID_NONE_TYPE 0xFF /*Internal Use Only*/

/*Responsiveness Requirement*/
#define  GEOFENCE_RESPONSIVENESS_HIGH        0 /* High Responsiveness*/
#define  GEOFENCE_RESPONSIVENESS_MEDIUM      1 /* Medium Responsiveness*/
#define  GEOFENCE_RESPONSIVENESS_LOW         2 /* Low Responsiveness*/
#define  GEOFENCE_RESPONSIVENESS_ULTRA_HIGH  3 /* Ultra High Responsiveness*/
#define  GEOFENCE_RESPONSIVENESS_CUSTOM      4 /* Custom Responsiveness*/
#define  GEOFENCE_RESPONSIVENESS_MAX    GEOFENCE_RESPONSIVENESS_CUSTOM

/*Confidence Requirement*/
#define GEOFENCE_CONFIDENCE_HIGH    0 /*High Confidence*/
#define GEOFENCE_CONFIDENCE_MEDIUM  1 /*Medium Confidence*/
#define GEOFENCE_CONFIDENCE_LOW     2 /*Low Confidence*/
#define GEOFENCE_CONFIDENCE_MAX     GEOFENCE_CONFIDENCE_LOW

/*Geofence EDIT Bitmask*/
#define SM_GM_GEOFENCE_EDIT_START_TIME                 0x1 /*Edit start time*/
#define SM_GM_GEOFENCE_EDIT_STOP_TIME                  0x2 /*Edit stop time*/
#define SM_GM_GEOFENCE_EDIT_EVENT_TYPE                 0x4 /*Edit event type*/
#define SM_GM_GEOFENCE_EDIT_LOC_EST                    0x8 /*Edit location estimate*/
#define SM_GM_GEOFENCE_EDIT_MIN_INTERVAL_TIME          0x10 /*Edit minimum interval time*/
#define SM_GM_GEOFENCE_EDIT_MAX_NUMBER_OF_REPORTS      0x20 /*Edit max number of reports*/
#define SM_GM_GEOFENCE_EDIT_RESPONSIVENESS             0x40 /*Edit responsiveness*/
#define SM_GM_GEOFENCE_EDIT_CONFIDENCE                 0x80 /*Edit confidence*/


/*Max Dwell Time*/
#define GEOFENCE_DWELL_TIME_MAX             0xFFFF /*in seconds*/

/* The minimum number of scanned 2.4Ghz WLAN frequencies which are accepted by GM*/
#define GM_MIN_NUM_SCANNED_WLAN_FREQ_2_4_GHZ 6

/*NV Mask for Goefencing Position Sources*/
#define GEOFENCE_POS_SOURCE_NV_CPI_MASK 0x1
#define GEOFENCE_POS_SOURCE_NV_LPW_MASK 0x2


/****************** Constants/Enums *******************************************/
/*List all the client types here*/
typedef enum
{
  GEOFENCE_CLIENT_SUPL, /*SUPL client*/
  GEOFENCE_CLIENT_QMI, /*QMI client*/
  GEOFENCE_CLIENT_BEST_AVAIL_POS, /*Best Available position client*/
  GEOFENCE_CLIENT_CPI,  /*Coarse Position Injection client*/
  GEOFENCE_CLIENT_AON, /*Always ON Services client*/
  GEOFENCE_CLIENT_LPW, /*Low Power Wifi client*/
  GEOFENCE_CLIENT_MAX /*Internal use*/
} sm_gm_client_id;

/*Operation Status types*/
typedef enum
{
  SUCCEEDED,
  FAILED,
  PENDING
} gm_operation_status_type;

/*Bitmask of client capability*/
typedef enum
{
  GEOFENCE_CLIENT_QMI_NOTIFICATION_CAPABLE = 0x1, /*Whether client is capable to notifying to QMI*/
  GEOFENCE_CLIENT_POS_FIX_CAPABLE = 0x2, /*Whether client is capable of making a fix*/
  GEOFENCE_CLIENT_GEOFENCE_SERVICE_REQ = 0x4 /*Whether client adds\removes geofences*/
} sm_gm_client_capabilities;

/*Pos fix method supported by client*/
typedef enum
{
  GEOFENCE_CLIENT_POS_FIX_AGNSS_UP_CAPABLE = 0x1, /*User Plane AGPS capable*/
  GEOFENCE_CLIENT_POS_FIX_AGNSS_CP_CAPABLE = 0x2, /*Control Plane AGPS capable*/
  GEOFENCE_CLIENT_POS_FIX_STANDALONE_CAPABLE = 0x4, /*Control Plane AGPS capable*/
  GEOFENCE_CLIENT_POS_FIX_WIFI_CAPABLE = 0x8, /*Wifi Fix capable*/
  GEOFENCE_CLIENT_POS_FIX_CELL_ID_CAPABLE = 0x10, /*Cell Id Fix capable*/
  GEOFENCE_CLIENT_POS_FIX_INDOOR_CAPABLE = 0x20, /*Indoor fix capable*/
  GEOFENCE_CLIENT_POS_FIX_BEST_AVAIL_CAPABLE = 0x40, /*Best Available Position Capable*/
  GEOFENCE_CLIENT_POS_FIX_CPI_CAPABLE = 0x80, /*CPI Capable*/
} sm_gm_client_pos_fix_method_capabilities;

/*Fix accuracy that can be provided by the client*/
typedef enum
{
  GEOFENCE_CLIENT_POS_FIX_ACCURACY_LOW, /*Low accuracy Fix*/
  GEOFENCE_CLIENT_POS_FIX_ACCURACY_MEDIUM, /*Medium accuracy Fix*/
  GEOFENCE_CLIENT_POS_FIX_ACCURACY_HIGH /*High accuracy Fix*/
} sm_gm_client_pos_fix_accuracy_capabilities;


/*Power consumption by client while making a fix */
typedef enum
{
  /*All below mw numbers assyme 3.75 volts battery
    Power in mW = Incremental mA * Time taken for the fix * 3.75
    All numbers assume typical sunny day scenario*/
  /*Note: Please keep these enum in increasing order of incremental power*/
  GEOFENCE_CLIENT_POS_FIX_POWER_PROFILE_0 = 0x1, /*Incremental power: 0mw*/
  GEOFENCE_CLIENT_POS_FIX_POWER_PROFILE_1 = 0x2, /*Incremtal power: < 150mw Example: LPW: 75 * 0.5 * 3.75 **/
  GEOFENCE_CLIENT_POS_FIX_POWER_PROFILE_2 = 0x4, /*Incremtal power: < 225mw. Example: GPS: 40 * 1.5 * 3.75*/
  GEOFENCE_CLIENT_POS_FIX_POWER_PROFILE_3 = 0x8, /*Incremtal power: < 1350mw. Example: CPI: 120 * 3 * 3.75*/  
  GEOFENCE_CLIENT_POS_FIX_POWER_PROFILE_4 = 0x10 /*Incremtal power: > 1350mw*/
} sm_gm_client_pos_fix_power_cap;

/*Higher power proc wakeup capabilities when computing a fix*/
typedef enum
{
  GEOFENCE_CLIENT_HIGH_POWER_PROC_WAKEUP_PROFILE_0 = 0x0, /*No Higher power proc is woken up*/
  GEOFENCE_CLIENT_HIGH_POWER_PROC_WAKEUP_PROFILE_1 = 0x1, /*Higher power proc is woken up if assistance is required. Not always*/
  GEOFENCE_CLIENT_HIGH_POWER_PROC_WAKEUP_PROFILE_2 = 0x2  /*Higher power proc is worken up always*/
} sm_gm_client_pos_fix_high_power_proc_wakeup_cap;

/*Network usage capabilities when computing a fix*/
typedef enum
{
  GEOFENCE_CLIENT_NW_USAGE_PROFILE_0 = 0x0, /*No Network usage*/
  GEOFENCE_CLIENT_NW_USAGE_PROFILE_1 = 0x1, /*Network is used if assistance is required. Not always*/
  GEOFENCE_CLIENT_NW_USAGE_PROFILE_2 = 0x2  /*Network is used always*/
} sm_gm_client_pos_fix_nw_usage_cap;

/*Types of Geofences*/
typedef enum
{
  GEOFENCE_TYPE_ANY, /*Any Geofence MO or NI*/
  GEOFENCE_TYPE_MO, /*Mobile originated Geofence*/
  GEOFENCE_TYPE_NI  /*Network initiated Geofence*/
}sm_gm_geofence_origin_type;

/*Shape of Geofence*/
typedef enum
{
  GEOFENCE_TARGET_AREA_CIRCULAR_TYPE,  /*Circular Target Area*/
  GEOFENCE_TARGET_AREA_MAX_TYPE = 0xFF
} sm_gm_target_area_type;

/*Status of the Geofence w.r.t Geofence*/
typedef enum
{
  GEOFENCE_POSITION_STATUS_UNKNOWN, /*Status unknown: due to unavailability of GPS fixes*/
  GEOFENCE_POSITION_STATUS_INSIDE,  /*Inside the geofence*/
  GEOFENCE_POSITION_STATUS_OUTSIDE  /*Outside the geofence*/
} sm_gm_geofence_position_status_type;

/*Event type client is interested to receive*/
typedef enum
{
  GEOFENCE_EVENT_ENTERING_AREA = 0x1, /*Entering Area Event*/
  GEOFENCE_EVENT_INSIDE_AREA =   0x2, /*Inside Area Event*/
  GEOFENCE_EVENT_OUTSIDE_AREA =  0x4, /*Outside Area Event*/
  GEOFENCE_EVENT_LEAVING_AREA =  0x8, /*Leaving Area Event*/
  GEOFENCE_EVENT_MAX          =  0xf /*Internal user only*/
}sm_gm_geofence_event_type;

/*Unsolicited Event Type*/
typedef enum
{
  GEOFENCE_UNSOL_EVENT_GPS_LOST, /*GPS Lost Event*/
  GEOFENCE_UNSOL_EVENT_GPS_AVAIL, /*GPS Available Event*/
  GEOFENCE_UNSOL_EVENT_OUT_OF_SERVICE,  /*Out Of Service Event*/
  GEOFENCE_UNSOL_EVENT_TIME_INVALID, /*Time Invalid Event*/
  GEOFENCE_UNSOL_EVENT_UNABLE_TO_TRACK, /*No location source available*/
  GEOFENCE_UNSOL_EVENT_ABLE_TO_TRACK /*A suitable location source is available*/
} sm_gm_unsol_event_type;


/*Type of fix. Dont change the mapping. Used by Apex parsing*/
typedef enum
{
  GEOFENCE_POS_FIX_SRC_NONE = 0, /*For Internal Use*/
  GEOFENCE_POS_FIX_SRC_CPI = 0x1, /*CPI pos fix*/
  GEOFENCE_POS_FIX_SRC_GNSS = 0x2, /*GNSS FIX*/
  GEOFENCE_POS_FIX_SRC_BEST_AVAIL_POS = 0x4,  /*XTRA-T Best Avail Pos Fix*/
  GEOFENCE_POS_FIX_SRC_LPW = 0x8, /*LPW pos fix*/
  GEOFENCE_POS_FIX_SRC_NO_FIX = 0x10, /*For Internal Use*/
  GEOFENCE_POS_FIX_SRC_ANY = 0x20, /*For Internal Use*/
  GEOFENCE_POS_FIX_SRC_MAX = 0xFF /*Internal Use: For forcing to a Max value*/
} sm_gm_pos_fix_src;

typedef enum
{
  GEOFENCE_POS_FIX_TYPE_INTERMEDIATE, /*Intermediate fix*/
  GEOFENCE_POS_FIX_TYPE_FINAL  /*Final fix*/
} sm_gm_pos_fix_type;

/*Position fix fail reason*/
typedef enum
{
  GEOFENCE_POS_FIX_FAIL_REASON_TIMEOUT, /*Timed out*/
  GEOFENCE_POS_FIX_FAIL_SRC_DISABLED, /*Src is diabled. For example, Wifi is disabled or GPS is disabled*/
  GEOFENCE_POS_FIX_FAIL_NO_NW, /*No Network*/
  GEOFENCE_POS_FIX_FAIL_NO_MEASUREMENT, /*No measuremtn. For exampple, No wifi Aps is scan*/

  /*LPW wifi specific fail reason*/
  GEOFENCE_POS_FIX_FAIL_LPW_NO_MP_ASST_DATA, /*Assistance data is not present on Modem processor*/
  GEOFENCE_POS_FIX_FAIL_LPW_NO_AP_ASST_DATA, /*Assistance data is not present on Application processor*/
}sm_gm_pos_fix_fail_reason_e;

/*The geofence DWELL event*/
typedef enum
{
  GEOFENCE_DWELL_TYPE_NONE =  0x1, /*Internal user only*/
  GEOFENCE_DWELL_TYPE_IN = 0x2,      /*Inside Dwell Type*/
  GEOFENCE_DWELL_TYPE_OUT =   0x4,   /*Outside Dwell Type*/  
}sm_gm_dwell_type;

typedef struct
{  
  boolean b_is_nw_asst_fix; /*whether fix is network assisted*/  
} sm_gm_gnss_pos_ext_info;

typedef struct
{  
  wl_ResultFlagsType z_wifi_result; /*Wifi Result Flag*/
} sm_gm_wifi_pos_ext_info;

typedef union
{
  sm_gm_gnss_pos_ext_info z_gnss_fix_ext_info;
  sm_gm_wifi_pos_ext_info z_wifi_fix_ext_info;
} sm_gm_pos_fix_ext_info_u;

typedef struct
{
  sm_gm_pos_fix_src fix_src;
  sm_gm_pos_fix_type fix_type;
  sm_GnssFixRptStructType pos_fix;
  sm_gm_pos_fix_ext_info_u y_pos_fix_ext_info;
}sm_gm_pos_fix_info;

/*Motion Detectors*/
typedef enum
{
  GM_MOTION_DETECTOR_NONE = 0x0, /*Internal state indicating no motion detection*/
  GM_MOTION_DETECTOR_SENSORS = 0x1, /*Sensors being used for Motion Detection*/
  GM_MOTION_DETECTOR_WIFI = 0x2, /*Wifi being used for Motion Detection*/
  GM_MOTION_DETECTOR_WWAN = 0x4 /*WWAN being used for Motion Detection*/
} gm_motion_detector_e_type;

/*structure for configuring motion state speeds used by Geofencing*/
typedef struct
{
  boolean b_walking_valid;
  float gm_ms_walk_speed ;
  boolean b_running_valid;
  float gm_ms_run_speed;
} gm_motion_state_speed_config;



typedef struct {
  tm_TaskInitNotifierCb pNotifierCb;
  uint8 clientIndex;
  boolean broadcast;
} gm_api_notifier_type;


/****************** Data Structures*********************************************/
/*Client Handle Type: A unique handle assigned to registered client*/
typedef uint8 sm_gm_client_handle;

/*Area-id RAT type GSM/WCDMA/LTE/CDMA/etc*/
typedef uint8 sm_gm_area_id_rat_type;

/*Area-id type: WITHIN/BORDER*/
typedef uint8 sm_gm_area_id_type;

/* Responsiveness in Geofencing means, when the geofence breach occurs how soon
   the user gets a notification that he has breached a geofence.
   Higher responsiveness increases the power usage . There is a tradeoff
   between responsiveness and power , and it is important to fine tune these
   parameters for optimum power savings . */
typedef uint8 sm_gm_geofence_responsiveness;

/* Confidence in Geofencing means, when the geofence breach is reported, how
   much confidence is required with the report. There is a tradeoff
   between confidence and power , and it is important to fine tune these
   parameters for optimum power savings . */
typedef uint8 sm_gm_geofence_confidence;

/*Geofence Edit Mask*/
typedef uint32 sm_gm_edit_geoefence_params_mask;


/* Bitmask to indicate the parameters for setting the geofence engine configuration. */
typedef uint64 sm_gm_engine_config_mask;

/* Timeout for sending GNSS UNAVAIL indication from GM in BAD GPS env */
#define SM_GM_ENGINE_CONFIG_GNSS_UNAVAIL_TIMEOUT_MASK                 (0x0001)
/* Enable motion detection using sensors in GM*/
#define SM_GM_ENGINE_CONFIG_ENABLE_SENSOR_MOTION_DETECTION_MASK       (0x0002)
/* Enable motion detection using Wifi in GM*/
#define SM_GM_ENGINE_CONFIG_ENABLE_WIFI_MOTION_DETECTION_MASK         (0x0004)
/* Enable motion detection using WWAN in GM*/
#define SM_GM_ENGINE_CONFIG_ENABLE_WWAN_MOTION_DETECTION_MASK         (0x0008)
/* Enable external position injection into GM*/
#define SM_GM_ENGINE_CONFIG_ENABLE_POSITION_INJECTION_MASK            (0x0010)
/* Enable batch breach reporting */
#define SM_GM_ENGINE_CONFIG_ENABLE_BATCH_BREACH_REPORT_MASK           (0x0020)
/* Max pos unc accepted */
#define SM_GM_ENGINE_CONFIG_USE_NW_ASST_FIXES_MASK                    (0x0040)
/*High Responsiveness Config*/
#define SM_GM_ENGINE_CONFIG_HIGH_RESPONSIVENESS_CONFIG_MASK           (0x0080)
/*Medium Responsiveness value config*/
#define SM_GM_ENGINE_CONFIG_MED_RESPONSIVENESS_VALUE_CONFIG_MASK      (0x0100)
/* The Motion State Speed Mask */
#define SM_GM_MOTION_STATE_SPEED_MASK                                 (0x0200)
/*GM Pos Operation Mode*/
#define SM_GM_ENGINE_CONFIG_POS_OPERATION_MODE_MASK                   (0x0400)
/* Enable GM registration with LOWI service. */
#define SM_GM_ENGINE_CONF_LOWI_REGISTER_ENABLE_MASK                   (0x0800)
/* Subscribe to the different services supported on LOWI from GM  */                        
#define SM_GM_ENGINE_CONF_LOWI_SUBSCRIPTION_MASK                      (0x1000)
/* Enable different registration timers for LOWI  */
#define SM_GM_ENGINE_CONF_LOWI_REG_TIMER_MASK                         (0x2000)
/* The retry count for short term registration timer   */
#define SM_GM_ENGINE_CONF_LOWI_SHORT_TERM_TIMER_RETRY_CNT_MASK        (0x4000)
/*Comprehensive Logging rate mask*/
#define SM_GM_ENGINE_CONFIG_COMP_LOGGING_RATE_MASK                    (0x8000)
/*GM GNSS ALS IOD */
#define SM_GM_ENGINE_CONFIG_IOD_SWITCH_MASK                           (0x10000)
/* Enable Low Power Wifi in GM*/
#define SM_GM_ENGINE_CONFIG_ENABLE_LPW_MASK                           (0x20000)
/*GM Filter WLAN freq */
#define SM_GM_ENGINE_CONFIG_FILTER_WLAN_FREQ_MASK                     (0x40000)
/*GM MAX POS UNC*/
#define SM_GM_ENGINE_CONFIG_GNSS_MAX_POS_UNC_ACCEPTED_MASK            (0x80000)


/* Config switche bits for IOD configuration passed in GM engine config param
 * */
#define SM_GM_ENABLE_IOD              (0x00000001ul)


typedef uint64 sm_gm_client_config_mask;

/* Max geofences supported by client  */
#define SM_GM_CLIENT_CONFIG_MAX_GEOFENCES_MASK                        (0x0001)
/*GNSS Pos Session Timeout*/
#define SM_GM_CLIENT_CONFIG_GNSS_POS_QOS_SESS_TIMEOUT_MASK            (0x0002)
/*GM CPI request rate*/
#define SM_GM_CLIENT_CONFIG_CPI_REQUEST_RATE_MASK                     (0x0004)
/* Minimum backoff when in Challenging GPS Env in seconds */
#define SM_GM_CLIENT_CHAL_GPS_ENV_BO_MIN_MASK                         (0x0008)
/* Maximum backoff when in Challenging GPS Env in seconds */
#define SM_GM_CLIENT_CHAL_GPS_ENV_BO_MAX_MASK                         (0x0010)
/* The motion sensing distance for GPS in case of challenging GPS env */
#define SM_GM_CLIENT_CHAL_GPS_ENV_MS_DIST_GPS_MASK                    (0x0020)
/* The motion sensing distance for GPS in case CPI*/
#define SM_GM_CLIENT_CHAL_GPS_ENV_MS_DIST_CPI_MASK                    (0x0040)
/*GM GNSS Unavailable Aggresively Exit*/
#define SM_GM_CLIENT_CONFIG_GNSS_UNAVAIL_AGGR_EXIT_MASK               (0x0080)

#define SM_GM_CLIENT_CONFIG_ALL (SM_GM_CLIENT_CONFIG_MAX_GEOFENCES_MASK| \
                                 SM_GM_CLIENT_CONFIG_GNSS_POS_QOS_SESS_TIMEOUT_MASK | \
                                 SM_GM_CLIENT_CONFIG_CPI_REQUEST_RATE_MASK | \
                                 SM_GM_CLIENT_CHAL_GPS_ENV_BO_MIN_MASK | \
                                 SM_GM_CLIENT_CHAL_GPS_ENV_BO_MAX_MASK | \
                                 SM_GM_CLIENT_CHAL_GPS_ENV_MS_DIST_GPS_MASK | \
                                 SM_GM_CLIENT_CHAL_GPS_ENV_MS_DIST_CPI_MASK | \
                                 SM_GM_CLIENT_CONFIG_GNSS_UNAVAIL_AGGR_EXIT_MASK)

/*Pos fix capability of the client*/
typedef struct
{
  sm_gm_client_pos_fix_method_capabilities pos_fix_method_cap; /*Position Fix Method capabilites*/
  sm_gm_client_pos_fix_accuracy_capabilities pos_fix_accuracy_cap; /*Position Fix Accuracy capabilites*/
  uint32 pos_fix_accuracy; /*Typical Position Fix accuracy provided by the client*/
  uint32 pos_fix_wait_secs; /*Time to wait for position fix*/
  boolean is_prior_fix_request_indication_required; /*Does client require prior fix request
                                                      indication, indicating that GM
                                                      will be requesting a fix soon to this
                                                      client*/
  uint8 prior_fix_request_indication_secs; /*How many seconds beforhand does client require
                                            fix request indication*/
  sm_gm_client_pos_fix_power_cap pos_fix_power_cap; /*Position Fix Power capabilities*/
  sm_gm_client_pos_fix_high_power_proc_wakeup_cap high_power_proc_wakeup_cap; /*Higher power processor wakeup capabilities*/
  sm_gm_client_pos_fix_nw_usage_cap nw_usage_cap; /*Network usage capability*/
  boolean is_imp_location_notification_required; /*Whether client interested in getting notification for important location*/
  /*Important location are lat\long that Geofencing deems important and is an indication
    from geofencing that client may be used actively for positioning in this location.
    Client can use this notification for example to make sure the tile for this locaiton is always present, etc*/
}sm_gm_pos_fix_capability;

/*Client capabilities*/
typedef struct
{
  sm_gm_client_capabilities client_capabilites; /*Bitmask of client capabilities*/

  sm_gm_pos_fix_capability pos_fix_capability; /*Position fix capability*/
}sm_gm_client_cap;

/*Client type structure*/
typedef struct
{
  sm_gm_client_id client_id; /*A unique client id*/
  uint32  q_client_thread_id; /*Client's Thread Id*/
  uint32  q_base_msg_id; /*Base Message Id*/
  uint32  q_base_transaction_id; /*Transaction Id for pos request*/
  sm_gm_client_cap client_cap; /*Client capabilities*/
} sm_gm_client_type;

/*GSM Area-id*/
typedef struct
{
  uint16 mcc; /* Mobile Country Code */
  uint16 mnc;/* Mobile Network Code */
  uint16 lac; /* Location Area Code */
  uint32 cell_identity;/*GSM  Cell Id */
}sm_gm_area_gsm_id;

/*WCDMA Area-id*/
typedef struct
{
  uint16 mcc; /* Mobile Country Code */
  uint16 mnc;/* Mobile Network Code */
  uint16 lac; /* Location Area Code */
  uint32 cell_identity;/* WCDMA Cell Id */
}sm_gm_area_wcdma_id;

/*LTE Area-id*/
typedef struct
{
  uint16 mcc; /* Mobile Country Code */
  uint16 mnc;/* Mobile Network Code */
  uint32 cell_identity; /* LTE Cell-Id including CSG bit */
}sm_gm_area_lte_id;

/*Area-id Structure*/
typedef struct
{
  uint16 mcc; /* Mobile Country Code */
  uint16 mnc;/* Mobile Network Code */
  uint32 cell_identity; /* TD-SCDMA Cell-Id */
}sm_gm_area_tdscdma_id;

#define GM_WIFI_MAC_ADDR_LENGTH WIPER_MAC_ADDR_LENGTH

typedef struct
{
  uint8 ap_mac_addr[GM_WIFI_MAC_ADDR_LENGTH];
} sm_gm_wifi_id;


typedef struct
{
  sm_gm_area_id_rat_type  area_id_rat_type; /*RAT type*/
  sm_gm_area_id_type  area_id_type; /*Area-id type, Border/Within*/
  uint32 area_id_event_timestamp; /*Time when the Area-id event was received*/

  union {
      sm_gm_area_gsm_id  gSMAreaId;  /*GSM Area-id*/
      sm_gm_area_wcdma_id wCDMAAreaId;  /*WCDMA Area-id*/
      sm_gm_area_lte_id  lTEAreaId;  /*LTE Area-id*/
      sm_gm_area_tdscdma_id  tDSCDMAAreaId;  /*tDSCDMA Area-id*/
      sm_gm_wifi_id  wIFIAreaId; /*Wifi Area ID*/
  } u;
}sm_gm_area_id;

/*Circular Shape Geofence*/
typedef struct
{
  sint31 latitude; /*Latitude of the center of the area*/
  sint31 longitude;/*Longitude of the center of the area*/
  uint32 radius; /*Radius of the area*/
} sm_gm_circular_target_area;

/*Target Area Structure*/
typedef struct
{
  sm_gm_target_area_type target_area_type; /*Target Area Shape*/
  union {
      sm_gm_circular_target_area circ_target_area; /*Circular Target Area*/
    } u;
}sm_gm_target_area;

/*Geofence Structure*/
typedef struct
{
  uint32 start_time; /*start time of geofence in secs*/
  uint32 stop_time; /*stop time relative to start time of geofence in secs*/
  sm_gm_geofence_event_type event_type;/*Interested event type*/
  boolean location_estimate; /*Is position required to be reported when breached*/
  boolean persistency_needed; /*If persistency needed for the geofence*/
  uint32 min_interval_time; /*Minimum time interval between breaches*/
  uint32 max_num_reports; /*Maximum number of breach reports*/
  sm_gm_geofence_responsiveness responsiveness; /*Geofence Responsiveness Required*/
  uint32 q_custom_responsiveness_secs; /* valid if responsiveness indicates custom */
  uint32 q_dwell_time_secs; /*Dwell time in seconds*/
  /*Dwell time is the time in seconds that handset need to spend after entering or leaving
      to produce a breach report. Min Dwell time is 0 seconds and Max Dwell time is 65535 seconds*/
   /*In Normal functioning, responsiveness dictates the time between position fix requests by
    Geofencing engine. When handset is far away from geofence, then time between fixes can be more
    that the responsiveness setting. But when closer, responsiveness value dictates the minimum time
    between position fixes which shall be adhered to for this Geofence.
    But in many cases, to provide better user experience, lowering the responsiveness time and hence
    time between positions in a power efficient way is advisable.
    Below params controls exactly how the Geofencing can change responsivenss*/
    
  /*When dynamic responsiveness is enabled, higher user experience is provided by temporarily 
      decreasing the responsiveness time where ever possible*/
  boolean dynamic_responsiveness; /*Dynamic responsiveness required*/
   /*When forced responsiveness is enabled, irrespective of the handsets distance from geofence
    boundary, responsievenss setting provided will be adhered to */
  boolean forced_responsiveness; /*Forced responsiveness. Meaning for no reason, 
                                                        responsivenss of this geofence can be altered*/
  boolean context_usage; /*Whether context usage is allowed*/
  sm_gm_geofence_confidence confidence; /*Geofence Confidence required*/
  boolean target_area_provided; /*Is target area provided*/
  sm_gm_target_area target_area;/*Target area*/
  boolean hysteresis_required; /*Hysteresis Required*/
  uint32 num_of_area_ids; /*Number of Area id's provided*/
  sm_gm_area_id *area_id_list; /*Pointer to array containing the area-id list
                                 GM module will be responsible for freeing
                                 this pointer*/
  uint32 client_blob_size; /*Client blob size*/
  byte *client_blob; /*Pointer to array containing the client-blob
                                 GM module will be responsible for freeing
                                 this pointer*/
  sm_gm_dwell_type q_dwell_type_mask; /* Type of dwell indication requested */
  boolean gnss_max_pos_unc_acceptable_valid; /* set to true if client provides the 
                                                        uncertainity threshold to be used */
  uint32 gnss_max_pos_unc_acceptable; /*Specified in meters. Maxium GNSS uncertainity  
                                                       acceptable, i.e the quality of the GNSS location information
                                                       which shall be used for breach detection*/
  boolean b_force_use_cached_fix; /*Force use cached fix*/
}sm_gm_geofence_type;

/*Add Geofence Request Type*/
typedef struct
{
  sm_gm_client_handle   client_handle; /*Geofence client handle*/
  uint32  q_transaction_id; /*Unique Transaction Id provided by the client*/
  sm_gm_geofence_type geofence; /*Geofence to be added*/
} sm_gm_add_request_type;


/*Edit Geofence Request Type*/
typedef struct
{
  sm_gm_client_handle   client_handle; /*Geofence client handle*/
  uint32  q_transaction_id; /*Unique Transaction Id provided by the client*/
  uint32 geofence_id; /*Unique Geofence id indentifying the geofence*/
  sm_gm_edit_geoefence_params_mask geofence_edit_mask; /*Edit mask*/

  /*Params to edit*/
  uint32 start_time; /*start time of geofence in secs*/
  uint32 stop_time; /*stop time relative to start time of geofence in secs*/
  sm_gm_geofence_event_type event_type;/*Interested event type*/
  boolean location_estimate; /*Is position required to be reported when breached*/
  uint32 min_interval_time; /*Minimum time interval between breaches*/
  uint32 max_num_reports; /*Maximum number of breach reports*/
  sm_gm_geofence_responsiveness responsiveness; /*Responsivess setting of geofence*/
  sm_gm_geofence_confidence confidence; /*Confidence setting of geofence*/
} sm_gm_edit_request_type;


/*Delete Request Type: Deleted a particular Geofence*/
typedef struct
{
  sm_gm_client_handle   client_handle; /*Geofence client handle*/
  uint32  q_transaction_id; /*Unique Transaction Id provided by the client*/
  uint32 geofence_id;  /*Unique Geofence id indentifying the geofence*/
} sm_gm_delete_request_type;

/*Purge Request Type: Deletes all geofences added by a client*/
typedef struct
{
  sm_gm_client_handle   client_handle; /*Geofence client handle*/
  uint32  q_transaction_id; /*Unique Transaction Id provided by the client*/
} sm_gm_purge_request_type;

/*Replace Geofence Request Type*/
typedef struct
{
  sm_gm_client_handle   client_handle; /*Geofence client handle*/
  uint32  q_transaction_id; /*Unique Transaction Id provided by the client*/
  uint32 geofence_id;  /*Unique Geofence id indentifying the geofence*/
  sm_gm_geofence_type geofence; /*Geofence to be replaced with*/  
} sm_gm_replace_request_type;

/*Notification Strcture: Geofences uses this structure to notify client about
  ADD/Delete/Edit/Purge/etc operation response*/
typedef struct
{
  uint8   return_code; /*Return code*/
  uint32  geofence_id; /*Unique Geofence id indentifying the geofence*/
  sm_gm_geofence_origin_type geofence_type; /*Geofence Type*/
} sm_gm_notificationt_type;

/*Client Notification Structure: This has the notification structure and transaction id*/
typedef struct
{
  uint32 q_transaction_id; /*Unique Transaction Id provided by the client*/
  sm_gm_notificationt_type notification; /*Actual Notification*/
} sm_gm_client_notification_type;

/*Cell DB notification Structure, used by TM to provide cell db notification to GM*/
typedef struct
{
  cgps_ActiveRATEnumType e_TriggerRAT; /*Trigger RAT which triggered the notification*/
  cgps_CellInfoCachedStruct cell_db_data; /*Cell DB info*/
}sm_gm_cell_db_notification_type;


/*Position Fix notification Structure, used by TM to provide position fix*/
typedef struct
{
  sm_gm_pos_fix_info pos_info;  
}sm_gm_position_notification_type;

/*Client Registeration Notification: Notificatin sent by GM_API to GM Core on client registeration*/
typedef struct
{
  sm_gm_client_handle   client_handle; /*Geofence client handle*/
} sm_gm_client_register_notify_type;

/*Breach Acknowledgement Structure: Client used to ack/nack the Goefence Breach Report*/
typedef struct
{
  boolean accepted; /*Ack/Nack of breach report*/
  uint32  geofence_id; /*Unique Geofence id indentifying the geofence*/
} sm_gm_breach_ack_notify_type;


/*Position Estimation*/
typedef struct
{
  sint31 l_latitude; /*Latitude of the center of the area*/
  sint31 l_longitude;/*Longitude of the center of the area*/
  uint32 hor_unc; /*Horizontal uncertainity*/
} sm_gm_pos_estimation;

/*Posiiton Request Structure: GM uses this structure to ask for a position to the client*/
typedef struct
{
  uint32 q_transaction_id; /*Unique Transaction Id provided by the client*/
  dword  num_fixes; /*Number of fixes*/
  dword  time_between_fixes; /*Time Between Fixes*/
  uint8  horizontal_accuracy; /*Horizontal Accuracy in meters needed*/
  uint8  session_timeout;  /*Session Timeout*/
  sm_gm_pos_fix_src pos_fix_src; /*Pos fix Src*/
  pdsm_pd_session_operation_e_type pos_operation_mode; /*Position operation mode*/

  /*Position fix restrictions that client shall adhere to*/  
  sm_gm_client_pos_fix_power_cap pos_fix_power_cap; /*Position Fix Power capabilities*/
  sm_gm_client_pos_fix_high_power_proc_wakeup_cap high_power_proc_wakeup_cap; /*Higher power processor wakeup capabilities*/
  sm_gm_client_pos_fix_nw_usage_cap nw_usage_cap; /*Network usage capability*/

  /*Age in seconds that a client can use a measurement for computing a fix.
    If set to 0, then a fresh measurement is required.*/
  uint32 max_measurement_age_secs; 
  /*Position estimation from Geofencing engine*/
  /*This can be used to pre-check if the position source can work
    in this region or not*/
  sm_gm_pos_estimation pos_estimate;
} sm_gm_client_pos_req_type;

/*Positon Abort Request Structure: GM uses this structure to ask for a position abort to the client*/
typedef struct
{
  uint32 q_transaction_id; /*Unique Transaction Id provided by the client*/
  sm_gm_pos_fix_src pos_fix_src;
} sm_gm_client_abort_pos_req_type;


/*Breach Notification Structure: GM uses to notify breach to client*/
typedef struct
{
  uint32 q_transaction_id; /*Unique Transaction Id provided by the client*/
  uint32  geofence_id;  /*Unique Geofence id indentifying the geofence*/
  sm_gm_geofence_origin_type geofence_type; /*Geofence Origin Type MO/NI/etc*/
  sm_gm_geofence_event_type breach_type; /*Breach event: Entering/Leaving/Inside/Outside/etc*/
  uint8 breach_confidence; /*Breach confidence assosicated with the fix*/
  boolean fix_included; /*Whether fix is included with the report*/
  boolean is_last_report; /*Whether this is the last report and geofence will be deleted
                            after acceptance of the report by the client*/
  sm_GnssFixRptStructType pos_fix_rpt;/*Position fix assosiated with the breach*/
} sm_gm_client_breach_notify_type;

typedef uint8 sm_gm_proximity_ind_confidence;

#define GM_PROXIMITY_IND_IN_PROXIMITY   0
#define GM_PROXIMITY_IND_OUT_PROXIMITY  1
#define GM_PROXIMITY_IND_MAX_PROXIMITY  2


typedef uint8 sm_gm_proximity_ind_type;

/*Breach Notification Structure: GM uses to notify breach to client*/
typedef struct
{
  uint32 q_transaction_id; /*Unique Transaction Id provided by the client*/
  uint32  geofence_id;  /*Unique Geofence id indentifying the geofence*/
  sm_gm_proximity_ind_type proximity_type; /*Proximity type*/
  uint8 context_id_valid; /*Whether context id is valid*/
  uint32 context_id; /*Context Id*/
} sm_gm_client_proximity_notify_type;

/*On demand Time Zone injection Req Structure: GM uses to notify client to 
 * inject TimeZone information */
typedef struct
{
  uint32 q_transaction_id; /*Unique Transaction Id provided by the client*/
} sm_gm_client_tz_req_notify_type;

/* Batch breach notification structure GM uses to notify the client */
typedef struct
{
  uint32 q_transaction_id; /*Unique Transaction Id provided by the client*/
  uint32 q_cont_list_len;  /* The size of the list where each element is a range of continuous geofence ids which breached */
  uint32 q_discrete_list_len; /* The size of the list where each element is a  geofence id which breached for the position*/
  sm_gm_geofence_origin_type geofence_type; /*Geofence Origin Type MO/NI/etc*/
  sm_gm_geofence_event_type q_breach_type; /*Breach event: Entering/Leaving/Inside/Outside/etc*/
  uint8 u_confidence_type; /*Breach confidence assosicated with the fix*/
  boolean b_fix_included; /*Whether fix is included with the report*/
  sm_GnssFixRptStructType z_pos_fix_rpt;/*Position fix assosiated with the breach*/
  /* This is the pointer to the list of discrete and continuous GID elements in the list .
  The discrete list is followed by the continuous GID list */
  uint32 data[1];
}sm_gm_client_batch_breach_notify_type;


/* Batch dwell breach notification structure GM uses to notify the client */
typedef struct
{
  uint32 q_transaction_id; /*Unique Transaction Id provided by the client*/
  uint32 q_cont_list_len;  /* The size of the list where each element is a range of continuous geofence ids which breached */
  uint32 q_discrete_list_len; /* The size of the list where each element is a  geofence id which breached for the position*/
  sm_gm_geofence_origin_type geofence_type; /*Geofence Origin Type MO/NI/etc*/
  sm_gm_dwell_type e_dwell_event_type; /*DWELL event: IN / OUT*/
  boolean b_fix_included; /*Whether fix is included with the report*/
  sm_GnssFixRptStructType z_pos_fix_rpt;/*Position fix assosiated with the breach*/
  /* This is the pointer to the list of discrete and continuous GID elements in the list .
  The discrete list is followed by the continuous GID list */
  uint32 data[1];
}sm_gm_client_batch_dwell_notify_type;


/* The structure containing the list if continuous GIDs in the form of a range */
typedef struct
{
  /* The low range of the gid */
  int gid_low;
  /* The high range of the gid */
  int gid_high;
}sm_gm_cont_gid_type;

/*Client Blob request Structure: Client uses this to request for client blob assosiated with the geofence*/
typedef struct
{
  sm_gm_client_handle   client_handle; /*Geofence client handle*/
  uint32  q_transaction_id;  /*Unique Transaction Id provided by the client*/
  uint32  geofence_id; /*Unique Geofence id indentifying the geofence*/
} sm_gm_get_geofence_client_blob_request_type;

/*Client Blob notificaton Structure: GM uses this to notify clint about the client blob assosiated with the geofence*/
typedef struct
{
  uint32  geofence_id; /*Unique Geofence id indentifying the geofence*/
  uint32 client_blob_size; /*Size of client blob*/
  byte client_blob[GEOFENCE_MAX_CLIENT_BLOB_SIZE]; /*Opaque client blob provided by the client at the time of geofence addition*/
} sm_gm_geofence_client_blob_notify_type;

/*Client Blob notification Structure: This has the notification structure and transaction id*/
typedef struct
{
  uint32 q_transaction_id;  /*Unique Transaction Id provided by the client*/
  sm_gm_geofence_client_blob_notify_type geofences_client_blob_info; /*Cient Blob notification*/
} sm_gm_geofence_client_blob_notification_type;

/*Client Geofence Handles Request Structure: Client uses this to get all geofence handles added by the client*/
typedef struct
{
  sm_gm_client_handle   client_handle; /*Geofence client handle*/
  uint32  q_transaction_id; /*Unique Transaction Id provided by the client*/
} sm_gm_client_geofences_handles_request_type;

/*Geofence Handles Notification Structure: GM uses this to notify client about all geofence handles added by the client*/
typedef struct
{
  uint32 q_transaction_id; /*Unique Transaction Id provided by the client*/
  uint32 num_of_client_geofence_handles; /*Number of geofence handles in this notification*/
  uint32 geofence_handles[1]; /*Start of the array which contains the geofence handles*/
} sm_gm_client_geofence_handels_notification_type;

/*Geofence Handles Request Structure: Client uses this to get all geofence handles, irrespective of who added the geofence*/
typedef struct
{
  sm_gm_client_handle   client_handle; /*Geofence client handle*/
  uint32  q_transaction_id;   /*Unique Transaction Id provided by the client*/
  sm_gm_geofence_origin_type geofence_origin; /*Origin Type of Geofence MO/NI to query for*/
} sm_gm_geofences_handles_request_type;

/*Geofence Handles Notification Structure: GM uses this to notify client about all geofence handles*/
typedef struct
{
  uint32 q_transaction_id; /*Unique Transaction Id provided by the client*/
  uint32 num_of_geofence_handles; /*Number of geofence handles in this notification*/
  uint32 geofence_handles[1]; /*Start of the array which contains the geofence handles*/
} sm_gm_geofence_handels_notification_type;

/*Query Geofence Request Structure: Client uses this to query for a particular geofence*/
typedef struct
{
  sm_gm_client_handle   client_handle; /*Geofence client handle*/
  uint32  q_transaction_id; /*Unique Transaction Id provided by the client*/
  uint32 geofence_id; /*Unique Geofence id indentifying the geofence*/
} sm_gm_query_geofence_status_request_type;

/*Query Geofence Notification Structure: GM uses this to notify client about a Geofence*/
typedef struct
{
  uint32 q_transaction_id; /*Unique Transaction Id provided by the client*/
  uint8   return_code; /*Return code assosiated with the query response*/
  uint32 geofence_id; /*Unique Geofence id indentifying the geofence*/
  sm_gm_target_area target_area;/*Target area*/
  sm_gm_geofence_origin_type geofence_type; /*Geofence origin type MO/NI/etc*/
  sm_gm_geofence_responsiveness responsiveness; /*Responsiveness Setting of a geofence*/
  sm_gm_geofence_confidence confidence; /*Confidence Setting of a geofence*/
  sm_gm_geofence_position_status_type geofence_pos_status; /*Present status of Geofence outside/inside/unknown/etc*/
} sm_gm_geofence_status_notification_type;

/*Unsolicited Notification Structure: GM uses this to notify client about the unsolicited events*/
typedef struct
{
  uint32 q_transaction_id; /*Unique Transaction Id provided by the client*/
  sm_gm_unsol_event_type unsol_event; /*Unsolicited Event*/
} sm_gm_client_unsol_event_notify_type;


/*Prior Posiiton Request Structure: GM uses this structure to indicate client that it will be asking for a pos
  requestion soon
*/
typedef struct
{
  uint32 q_transaction_id; /*Unique Transaction Id provided by the client*/
} sm_gm_client_prior_pos_req_indication_type;


/*Set Geofence Engine Config Request Type*/
typedef struct
{
  sm_gm_client_handle   client_handle; /*Geofence client handle*/
  uint32  q_transaction_id; /*Unique Transaction Id provided by the client*/

  /* Mask to identify which geofence engine parameters are being set */
  sm_gm_engine_config_mask q_mask;

  uint32 q_gnss_unavailable_indication_timeout;

  /* Enable position injection */
  boolean q_enable_position_injection;

  /*Enable Sensor Motion Detection*/
  boolean q_enable_sensor_motion_detection;

  /*Enable Wifi Motion Detection*/
  boolean q_enable_wifi_motion_detection;

  /*Enable WWAN Motion Detection*/
  boolean q_enable_wwan_motion_detection;

  /* Enable batch breach reporting */
  boolean b_enable_batch_breach_report;

  /* Use NW asst fixes for breaches*/
  boolean b_use_nw_asst_fixes;

  /*High Responsiveness Config*/
  uint32 q_high_responsiveness_config;

  /*Medium Responsiveness Seconds*/
  uint16 q_medium_resp_value_secs;

  /*The motion sensing speed config*/
  gm_motion_state_speed_config q_gm_motion_state_speeds;

  /*GM Operation mode mask*/
  pdsm_pd_session_operation_e_type q_gm_pos_operation_mode;
  
  /*Comprehensive Logging Rate*/
  uint32 q_gm_comp_logging_rate;

  /* Enable GM LOWI registration */
  boolean b_lowi_reg_enable;

  /* Subscribe to different services on LOWI */
  uint32 q_lowi_subs_mask;

  /*The ALS IOD sensor config mask used by Geofencing*/
  uint32 q_sm_gm_iod_config_switch_mask;

  /* Mask to identify which timer to enable for LOWI registration retry */
  uint32 q_lowi_enable_reg_timer_mask; 

  /* Maximum retry count for the short term timer  */
  uint32 q_lowi_short_term_timer_max_retry_cnt;

  /* GM filter WLAN freq. mask */
  uint32 q_filter_wlan_freq_mask;

  /* Enable Low power wifi */
  boolean q_enable_lpw; 
  
  /* Max pos unc accepted */
  uint32 q_gnss_max_pos_unc_accepted;
} sm_gm_set_geofence_engine_config_request_type;

/*Set Geofence Client Config Request Type*/
typedef struct
{
  /*Client need not set client_handle when sing this struct during client registeration*/
  sm_gm_client_handle client_handle;
  
  /* Mask to identify which geofence engine parameters are being set */
  sm_gm_client_config_mask q_mask;

  /* Max number of geofences to be set */
  uint32 q_max_geofences;

  /*GNSS Pos QOS Session Timeout*/
  uint8 q_gnss_pos_qos_sess_timeout;

  /* GM CPI request rate */
  uint32 q_cpi_request_rate;

  /* Minimum backoff when in Challenging GPS Env in seconds */
  uint32 q_chal_gps_env_bo_min;

  /* Maximum backoff when in Challenging GPS Env in seconds */
  uint32 q_chal_gps_env_bo_max;

  /* The motion sensing distance for GPS in case of challenging GPS env */
  uint32 q_chal_gps_env_ms_dist_gps;

  /* The motion sensing distance for GPS in case of challenging CPI env */
  uint32 q_chal_gps_env_ms_dist_cpi;

  /*Whether to try to exit gnss unavail aggresively*/
  boolean gnss_unavail_aggresive_exit;  
} sm_gm_set_client_config_type;

typedef struct {
  uint32  q_transaction_id; /*Unique Transaction Id provided by the client*/  
  sm_gm_set_client_config_type config_request;
} sm_gm_set_geofence_client_config_request_type;

/* Structure to provide the info about Position fix fail */
typedef struct
{
  uint32 reserved;
}sm_gm_position_fix_fail_type;

/* Structure to provide the info about Position fix request fail */
typedef struct
{
  sm_gm_pos_fix_src pos_fix_src;
}sm_gm_position_fix_request_fail_type;

/* Structure to provide PD events */
typedef struct
{
  pdsm_pd_event_type  e_pd_event;
}sm_gm_pd_event_update_type;

/*SRV SYS notification Structure, used by TM to provide notification to GM*/
typedef struct
{
  sys_sys_mode_e_type e_curr_srv_sys;
}sm_gm_srv_sys_notification_type;

/* Time Zone information */
typedef struct
{
  uint32 q_up_time_stamp;     /* Time stamp for when the UTC time was provided */
  uint64 qq_time_utc_secs;    /* UTC time */
  uint64 qq_raw_offset_secs;  /* Raw offset from UTC time */
  uint64 qq_dst_offset_secs;  /* Daylight savings time offset */
} sm_gm_time_zone_info;

/*The GM in memory logging configuration structure */
typedef struct
{
  uint32 q_max_log_size;
  uint32 q_period_s;
}sm_gm_in_mem_log_config_type;

/***************IPC Headers***************************************************/

/****************** Constants/Enums ******************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/* Enum to identify the source of WLAN attachment information. */
typedef enum
{
  /* The attachment information comes from source LOWI */
  GM_WIFI_ATTACH_INFO_SRC_LOWI,
  /* The attachment information comes from source HLOS */
  GM_WIFI_ATTACH_INFO_SRC_HLOS,
  GM_WIFI_ATTACH_INFO_SRC_MAX = 0x10000000 /* used to force enum to 32-bit */
}sm_gm_wifi_attach_info_src_e_type;

/* Enum to identify the different events from LOWI service into GM */
typedef enum
{
  GM_LOWI_LP_NONE,
  GM_LOWI_LP_SERVICE_ERROR,
  GM_LOWI_EVENT_MAX = 0x10000000 /* used to force enum to 32-bit */,
}sm_gm_lowi_status_event_e_type;

/****************** Data Structures*********************************************/

/* Structure for receiving the response events from LOWI into GM  */
typedef struct
{
  /* Type of the event */
  sm_gm_lowi_status_event_e_type e_lowi_event_type;
}sm_gm_lowi_status_event_s_type;

/* Structure for storing wifi attachment status information from different sources. */
typedef struct
{
  sm_gm_wifi_attach_info_src_e_type e_src;
  pdsm_wifi_attachment_status_ind_info z_attach_info;  
}gm_wifi_attachment_status_ind_s_type;

#ifdef __cplusplus
}
#endif

/****************** Function Declarations**************************************/
/*===========================================================================
gm_register_task_init_notifier

Description:

   This function is used by clients to register a callback which will be called
   when GM task is up

Parameters:

Return value:

  boolean: SUCCESS or FAIL
=============================================================================*/
boolean gm_register_task_init_notifier(tm_TaskInitNotifierCb pNotifierCb, os_ThreadIdEnumType clientThreadId);

/*===========================================================================
gm_client_register

Description:

   This function is used by clients to register to Geofencing Services.
   Any client who is interested in adding geofences should register to GM using this function.
   Any client who is interested in providing fixes to GM (though not interested in adding Geofences)
    should register to GM using this function.

Parameters:

  sm_gm_client_type* p_client: Pointer to client structure
  uint8 *handle: Pointer to client handle which will be filled by this function
  uint8* error_coe: Error code in case of error will be returned in this param.

Return value:

  boolean: SUCCESS or FAIL
=============================================================================*/

gm_operation_status_type gm_client_register(sm_gm_client_type *p_client, 
                                            sm_gm_client_handle *handle, 
                                            sm_gm_set_client_config_type *p_client_config,
                                            uint8* error_code);

/*===========================================================================
gm_client_deregister

Description:

   This function is used by clients to deregister to Geofencing Services.

Parameters:

  sm_gm_client_handle client_handle: Client Handle.
  uint8* error_coe: Error code in case of error will be returned in this param.

Return value:

  boolean: SUCCEEDED or FAILED
=============================================================================*/
gm_operation_status_type gm_client_deregister(sm_gm_client_handle client_handle, uint8* error_code);


/*===========================================================================
gm_add_geofence

Description:

   This function is used by clients to add a Geofence.
   An asynchronous add notification will be sent to client, if add was succesfull notification will
   containing a unique geofence id given to the added geofence.
   This geofence_id then shall be used henceforth for any geofence operation like edit, delete, etc.

Parameters:
  sm_gm_add_request_type* request: Pointer to client request
  uint8* error_code: Pointer to error code


Return value:

  boolean: PENDING or FAILED
=============================================================================*/
gm_operation_status_type gm_add_geofence(sm_gm_add_request_type* request, uint8 * error_code);

/*===========================================================================
gm_edit_geofence

Description:

   This function is used by clients to edit a Geofence.
   An asynchronous edit notification will be sent to client.

Parameters:
  sm_gm_edit_request_type* request: Pointer to client request
  uint8* error_code: Pointer to error code


Return value:

  boolean: PENDING or FAILED
=============================================================================*/
gm_operation_status_type gm_edit_geofence(sm_gm_edit_request_type* request, uint8 * error_code);

/*===========================================================================
gm_delete_geofence

Description:

   This function is used by clients to delete a Geofence.
   An asynchronous delete notification will be sent to client

Parameters:
  sm_gm_delete_request_type* request: Pointer to client request
  uint8* error_code: Pointer to error code


Return value:

  boolean: PENDING or FAILED
=============================================================================*/
gm_operation_status_type gm_delete_geofence(sm_gm_delete_request_type* request, uint8 * error_code);

/*===========================================================================
gm_purge_geofence

Description:

   This function is used by clients to purge a Geofence
   An asynchronous purge notification will be sent to client

Parameters:

  sm_gm_purge_request_type* request: Pointer to client request
  uint8* error_code: Pointer to error code


Return value:

  boolean: PENDING or FAILED
=============================================================================*/
gm_operation_status_type gm_purge_geofence(sm_gm_purge_request_type* request, uint8* error_code);

/*===========================================================================
gm_replace_geofence

Description:

   This function is used by clients to repleace a Geofence.
   An asynchronous replace notification will be sent to client

Parameters: 
  sm_gm_replace_request_type* request: Pointer to client request
  uint8* error_code: Pointer to error code


Return value: 

  boolean: PENDING or FAILED
=============================================================================*/
gm_operation_status_type gm_replace_geofence(sm_gm_replace_request_type* request, uint8 * error_code);

/*===========================================================================
gm_breach_ack

Description:

   This function is used by client to send breach acknowledgement.
   If breach was accepted, GM will increment the number of reports and go to minimal interval timer sleep.
   If breach was not accepted, GM will wait for next breach event to occur and will not increment the number of reports.

Parameters:
  sm_gm_breach_ack_notify_type *notify: Pointer to breach acknowledgement structure

Return value:
  void
=============================================================================*/
void gm_breach_ack(sm_gm_breach_ack_notify_type*  notify, uint8* error_code);

/*===========================================================================
gm_cell_db_update

Description:

   This function is used by TM core to update the cell db information

Parameters:
  cgps_ActiveRATEnumType e_TriggerRAT: RAT that triggered the update
  const cgps_CellInfoCachedStruct *cell_db_data_ptr: Pointer to cached cell db information

Return value:
  void
=============================================================================*/
void gm_cell_db_update(const cgps_ActiveRATEnumType e_TriggerRAT, const cgps_CellInfoCachedStruct *cell_db_data_ptr );

/*===========================================================================
gm_position_update

Description:

   This function is used by TM core to send position fix

Parameters:
  sm_gm_pos_fix_info fix_info: Pointer to Fix Info

Return value:
  void
=============================================================================*/
void gm_position_update(const sm_gm_pos_fix_info*  const p_pos_info);

/*===========================================================================
gm_get_geofence_client_blob

Description:

   This function is used by clients to retrieve the geofence client blob.

Parameters:

  sm_gm_get_geofence_client_info_request_type* request: Pointer to client request
  uint8* error_code: Pointer to error code

Return value:

  boolean: PENDING or FAILED
=============================================================================*/
gm_operation_status_type gm_get_geofence_client_blob(sm_gm_get_geofence_client_blob_request_type* request, uint8* error_code);

/*===========================================================================
gm_get_client_geofences_handles

Description:

   This function is used by clients to retrieve the geofence handles of all geofences
   the client owns.

Parameters:

  sm_gm_client_geofences_handles_request_type* request: Pointer to client request
  uint8* error_code: Pointer to error code

Return value:

  boolean: PENDING or FAILED
=============================================================================*/
gm_operation_status_type gm_get_client_geofences_handles(sm_gm_client_geofences_handles_request_type* request, uint8* error_code);
/*===========================================================================
gm_get_geofences_handles

Description:

   This function is used by clients to retrieve the geofence handles of all geofences.

Parameters:

  sm_gm_geofences_handles_request_type* request: Pointer to client request
  uint8* error_code: Pointer to error code

Return value:

  boolean: PENDING or FAILED
=============================================================================*/
gm_operation_status_type gm_get_geofences_handles(sm_gm_geofences_handles_request_type* request, uint8* error_code);

/*===========================================================================
gm_query_geofences_status

Description:

   This function is used by clients to retrieve the geofence status.

Parameters:

  sm_gm_query_geofence_status_request_type* request: Pointer to client request
  uint8* error_code: Pointer to error code

Return value:

  boolean: PENDING or FAILED
=============================================================================*/
gm_operation_status_type gm_query_geofence_status(sm_gm_query_geofence_status_request_type* request, uint8* error_code);

#ifdef FEATURE_CGPS_WCDMA
/*===========================================================================
gm_rrc_cell_info_dch_report


Description:

   This function handles the rrc cell info DCH report to GM module

Parameters:
   Cell ID information:
      rrcmeas_plmn_cell_info_struct_type cell_id_info
   Status: (RRC_NEW_CELL_ID_AVAIL/RRC_NO_CELL_ID_AVAIL)
      rrc_cell_id_status_type            status


Return value:
  void
=============================================================================*/
void gm_rrc_cell_info_dch_report(rrcmeas_plmn_cell_info_struct_type cell_id_info,
                                 rrc_cell_id_status_type            status );

#endif

/*===========================================================================
gm_update_gps_lock_state


Description:

   This function handles the gps lock state update from TM

Parameters:
  pdsm_gps_lock_e_type: GPS lock state

Return value:
  void
=============================================================================*/
void gm_update_gps_lock_state(pdsm_gps_lock_e_type  gps_lock);

/*===========================================================================
gm_update_dpo_status_indication

Description:

   This function updates the motion data

Parameters:
  p_dpo_status_indication: Pointer to DPO status indication structure

Return value:
  void
=============================================================================*/
void gm_update_dpo_status_indication(sm_DpoStatusReportStructType *p_dpo_status_indication);

#ifdef FEATURE_GEOFENCE_SENSOR_SUPPORT  

/*===========================================================================
gm_inject_iod_indication

Description:

   This function injects an IOD indication

Parameters: 
  amd_indication: Pointer to IOD indication structure
  
Return value: 
  void
=============================================================================*/
void gm_inject_iod_indication(loc_geofence_iod_ind *iod_indication);

/*===========================================================================
gm_inject_amd_indication

Description:

   This function injects AMD indication

Parameters:
  amd_indication: Pointer to AMD indication structure

Return value:
  void
=============================================================================*/
void gm_inject_amd_indication(loc_geofence_amd_ind *amd_indication);
/*===========================================================================
gm_inject_rmd_indication

Description:

   This function injects RMD indication

Parameters:
  amd_indication: Pointer to RMD indication structure

Return value:
  void
=============================================================================*/
void gm_inject_rmd_indication(loc_geofence_rmd_ind *rmd_indication);
/*===========================================================================
gm_inject_ped_indication

Description:

   This function injects PED indication

Parameters:
  amd_indication: Pointer to PED indication structure

Return value:
  void
=============================================================================*/
void gm_inject_ped_indication(loc_geofence_ped_ind *ped_indication);
/*===========================================================================
gm_inject_cmc_db_indication

Description:

   This function injects CMC indication

Parameters:
  amd_indication: Pointer to CMC indication structure

Return value:
  void
=============================================================================*/
void gm_inject_cmc_db_indication(loc_geofence_cmc_db_ind *cmc_db_indication);

/*===========================================================================
gm_inject_smd_indication

Description:

   This function injects SMD indication

Parameters:
  smd_indication: Pointer to SMD indication structure

Return value:
  void
=============================================================================*/
void gm_inject_smd_indication(loc_geofence_smd_ind *smd_indication);

#endif /*FEATURE_GEOFENCE_SENSOR_SUPPORT*/

/*===========================================================================
gm_set_geofence_engine_config

Description:

   This function is used to set geofence system configuration parameters. 
   The configuration applies to all geofences.
   Only GFC_QMI client is allowed to use this api.

Parameters:
  p_request: Input request identifying the parameters to be set .
  p_error_code: Error code if any .

Return value:
  status of the operation.
=============================================================================*/
gm_operation_status_type gm_set_geofence_engine_config( sm_gm_set_geofence_engine_config_request_type *p_request, 
                                                        uint8* p_error_code);

/*===========================================================================
gm_set_geofence_client_config

Description:

   This function is used to set geofence client configuration parameters. 
   The configuration applies to all geofences added by the client.

Parameters:
  p_request: Input request identifying the parameters to be set .
  p_error_code: Error code if any .

Return value:
  status of the operation.
=============================================================================*/
gm_operation_status_type gm_set_geofence_client_config(sm_gm_set_geofence_client_config_request_type *p_request, 
                                                       uint8* p_error_code);


/*===========================================================================
gm_position_fix_fail_update

Description:

   This function is used by TM core to send the update in case position fix
   was not succesfull.

Parameters:
  sm_gm_pos_fix_src fix_src: Fix src that failed.
  sm_gm_pos_fix_fail_reason_e fail_reason: Failure reason

Return value:
  void
=============================================================================*/
void gm_position_fix_fail_update(sm_gm_pos_fix_src fix_src, sm_gm_pos_fix_fail_reason_e fail_reason);

/*===========================================================================
gm_position_fix_fail_update

Description:

   This function is used by client to indicate the position fix requested by GM could not be
   executed

Parameters:
  None.

Return value:
  void
=============================================================================*/
void gm_position_fix_request_fail(sm_gm_pos_fix_src pos_fix_src);

/*===========================================================================
gm_update_pd_event

Description:

   This function is used by TM core to update GM of PD events

Parameters:
  None.

Return value:
  void
=============================================================================*/
void gm_update_pd_event(pdsm_pd_event_type  e_pd_event);


/*===========================================================================
gm_srv_sys_update

Description:

   This function is used by TM core to update the current serving system RAT

Parameters:
  p_srv_sys_notify: The current serving system .

Return value:
  void
=============================================================================*/
void gm_srv_sys_update(sm_gm_srv_sys_notification_type* p_srv_sys_notify);


#ifdef FEATURE_CGPS_UMTS_CELLDB
/*===========================================================================
gm_wcdma_cell_meas_update

Description:
  The function is used to send the WCDMA cell measurements to GM module .

Parameters:
  None.

Return value:
  void
=============================================================================*/
void gm_wcdma_cell_meas_update(wl1_lsm_cell_meas_update_type *cell_meas_update_ptr);
#endif


#ifdef FEATURE_CGPS_LTE_CELLDB
/*===========================================================================
gm_lte_cell_meas_update

Description:
  The function is used to send the LTE cell measurements to GM module .

Parameters:
  None.

Return value:
  void
=============================================================================*/
void gm_lte_cell_meas_update(lte_ml1_gps_mrl_cell_meas_update_t *cell_meas_update_ptr);
#endif

#ifdef __cplusplus
extern "C"
{
#endif
/********************************************************************
*
* gm_inject_wifi_attach_status_ind
*
* Function description:
* Injets Wifi attachment status indication info
*
* Parameters:
*  p_wifi_attachment_status_ind_info - Pointer to Wifi attachment status inidcation info stuct
*
* Return:
* void
*
*********************************************************************
*/  
void gm_inject_wifi_attach_status_ind(gm_wifi_attachment_status_ind_s_type *p_wifi_attachment_status_ind_info);

/********************************************************************
*
* gm_inject_lowi_status_events
*
* Function description:
* Function used to inject the subscription events from LOWI into GM 
*
* Parameters: 
*  p_lowi_event_info : The information about the subscription event. 
*
* Return:
* void
*  
*********************************************************************
*/  
void gm_inject_lowi_status_events(const sm_gm_lowi_status_event_s_type *p_lowi_event_info);


/*===========================================================================

FUNCTION gm_in_mem_logging_start

DESCRIPTION
  This function is for starting GM in memory logging.

DEPENDENCIES

RETURN VALUE
  TRUE/FAIL
SIDE EFFECTS
  None.

===========================================================================*/
boolean gm_in_mem_logging_start(sm_gm_in_mem_log_config_type *p_log_config);

/*===========================================================================

FUNCTION gm_in_mem_logging_stop

DESCRIPTION
  This function is for stopping in mem GM logging.

DEPENDENCIES

RETURN VALUE
  TRUE/FAIL
SIDE EFFECTS
  None.

===========================================================================*/
boolean gm_in_mem_logging_stop(void);

/*===========================================================================
gm_inject_wifi_scan_indication

Description:

   This function injects Wifi scan indication

Parameters: 
  wifi_scan_indication: Pointer to Wifi Scan indication structure
  
Return value: 
  void
=============================================================================*/
void gm_inject_wifi_scan_indication(t_wiper_ap_set_struct_type *wifi_scan_indication);

#ifdef __cplusplus
}
#endif

/********************************************************************
*
* gm_inject_wifi_enabled_status_ind
*
* Function description:
* Injets Wifi enabled status indication info
*
* Parameters: 
*  e_wifi_enabled_status_ind_info : The information about wifi enabled status.
*
* Return:
* void
*  
*********************************************************************
*/  
void gm_inject_wifi_enabled_status_ind(pdsm_wifi_enabled_status_ind_type e_wifi_enabled_status_ind_info);

/*===========================================================================
gm_inject_time_zone_req

Description:
   This function is used to inject the TimeZone information into GM

Parameters:   
  z_tz_info: Time Zone structure

Return value: 
  None

=============================================================================*/
void gm_inject_time_zone_req
(
  sm_gm_time_zone_info z_tz_info
);

#ifdef __cplusplus
}
#endif
#endif /*GM_API_H*/
