/*======================================================================

                         GFC_QMI API header file

 GENERAL DESCRIPTION
  This file contains API declarations by which QMI can communicate with
  the GFC_QMI ( Geofence client for QMI ) for MO Geofencing operations .


 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS
  The caller to GFC_QMI must wait for the GM core to be initialised completely before
  calling into GFC_QMI . The clients to the PDAPI need to wait for the call back
  coming from the API pdsm_set_ready_flag(boolean b_Ready) which indicates ( when b_Ready is TRUE )
  that the TM is intialised ( inturn the GM module ) and ready to accept calls from the clients.

  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014, 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
 
  Copyright (c) 2015 Qualcomm Techonologies, Inc.  
  Qualcomm Techonologies Confidential and Proprietary. All Rights Reserved.
 
 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/inc/gfc_qmi_api.h#1 $

 when      who   what, where, why
 ----      ----  --- --------------------------------------------------
04/02/15    ssu   Batch Dwell notification for geofence.
02/26/15   ssu    Adding new NV items for GM LOWI integration.
01/26/15    ssu   Adding geofence custom responsiveness.
04/03/14    ssu   Additional Fix details parameters for Geofence Breach reports
 03/24/12   ss       Supporting operation mode settting to effect Geofencing operation mode
02/21/14   ssu   Adding NV for configuring motion sensing distance in case of challenging GPS env.
02/20/14   ssu     Adding a NV for modifying the medium responsiveness backoff value and gps challenging env backoff config.
 02/10/14   ssu     Adding a NV for modifying the rate at which GM requests CPI injection.
 01/10/14   ss       Supporting High Confidence Geofence config
 02/29/12   ssu     Adding geofence purge request API.
 04/04/06   ssu   Initial creation of header file.
======================================================================*/

#ifndef GFC_QMI_API_H
#define GFC_QMI_API_H

#include "comdef.h"
#include "pdapi.h"
#include "gm_api.h"
#include "location_service_v02.h"

/* sets the specified bit in a u32 bit mask. bit counts range: 0-31 */
#define GFC_QMI_SET_U32BITMASK_BIT(n) (((uint32)0x1) << (n))

/********* REQUEST Data types QMI -> GFC_QMI ***********************/
/* The bitmask is used to identify the different types of Geofence breach
   events QMI are interested in. */
typedef uint32 gfc_qmi_geofence_breach_event_req_mask;
  /* Breach event when the handset enters the geofence */
#define GFC_QMI_BREACH_GEOFENCE_EVENT_REQ_ENTERING_MASK  GFC_QMI_SET_U32BITMASK_BIT(0) /*0x0001*/
  /* Breach event when the handset exits the geofence */
#define  GFC_QMI_BREACH_GEOFENCE_EVENT_REQ_LEAVING_MASK  GFC_QMI_SET_U32BITMASK_BIT(1) /*0x0002*/


/* The data type is used to request the type of area the QMI is interested with the Geofence. */
typedef enum
{
  GFC_QMI_GEOFENCE_TARGET_SHAPE_CIRCULAR,  /*Circular Target Area*/
  GFC_QMI_GEOFENCE_TARGET_SHAPE_MAX = 0x1FFFFFFF
} gfc_qmi_geofence_target_shape_req_e_type;



/* The data type is used by QMI to specify the responsiveness of the Geofence.
   Responsiveness in Geofencing means, when the geofence breach occurs how soon
   the user gets a notification that he has breached a geofence.
   Higher responsiveness increases the power usage . There is a tradeoff
   between responsiveness and power , and it is important to fine tune these
   parameters for optimum power savings . */
typedef enum
{
  GFC_QMI_RESPONSIVENESS_LOW,
  GFC_QMI_RESPONSIVENESS_MEDIUM,
  GFC_QMI_RESPONSIVENESS_HIGH,
  GFC_QMI_RESPONSIVENESS_ULTRA_HIGH,
  /* The user can input a range of explicit responsiveness values in custom responsiveness.
     We specify the value in a separate field. */
  GFC_QMI_RESPONSIVENESS_CUSTOM,
  GFC_QMI_RESPONSIVENESS_MAX = 0x1FFFFFFF
} gfc_qmi_responsiveness_e_type;


/* The data type is used by QMI to specify the confidence of the Geofence.
   Confidence in Geofencing means, when the geofence breach occurs with how
   much confidence the geofence engine can indicate the geofence breach.
   Higher confidence increases the power usage . There is a tradeoff
   between confidence and power , and it is important to fine tune these
   parameters for optimum power savings .*/
typedef enum
{
  GFC_QMI_CONFIDENCE_LOW,
  GFC_QMI_CONFIDENCE_MEDIUM,
  GFC_QMI_CONFIDENCE_HIGH,
  GFC_QMI_CONFIDENCE_MAX = 0x1FFFFFFF
} gfc_qmi_confidence_e_type;


/* Data type used by QMI to specify and define the actual geofence */
typedef struct
{
  double latitude_d; /* Latitude of the center of the area. Units in degrees.  */
  double longitude_d;/* Longitude of the center of the area. Units in degrees. */
  uint32 radius_m; /* Radius of the area in meters */
} gfc_qmi_geofence_circular_target_area_req_s_type;

/* The data type is used by the QMI to provide the information about the
Area type it is interested with the Geofence. */
typedef struct
{
  gfc_qmi_geofence_target_shape_req_e_type target_shape_type;
  union
  {
    gfc_qmi_geofence_circular_target_area_req_s_type circ_target_area;
  }gfc_qmi_geofence_target_area_req_u;
}gfc_qmi_geofence_target_area_req_s_type;


/* The data type is used to identify the position of the handset with respect to the geofence. */
typedef enum
{
  GFC_QMI_POS_UNKNOWN ,
  GFC_QMI_POS_INSIDE ,
  GFC_QMI_POS_OUTSIDE ,
  GFC_QMI_POS_MAX = 0x1FFFFFFF
} gfc_qmi_pos_rsp_e_type;

/* The data type is used to identify the type of breach which happened for the handset with respect to the geofence.*/
typedef enum
{
  GFC_QMI_BREACH_GEOFENCE_TYPE_ENTERING ,
  GFC_QMI_BREACH_GEOFENCE_TYPE_LEAVING ,
  GFC_QMI_BREACH_GEOFENCE_TYPE_UNKNOWN ,
  GFC_QMI_BREACH_GEOFENCE_TYPE_MAX = 0x1FFFFFFF
} gfc_qmi_breach_geofence_rsp_e_type;

/* The data type used to report the origin of the Geofence to the QMI. */
typedef enum
{
  /* ALl the geofences in the system */
  GFC_QMI_GEOFENCE_ORIGIN_ALL,
  /* Geofence initated from network.  */
  GFC_QMI_GEOFENCE_ORIGIN_NI,
  /* Mobile originated Geofence .*/
  GFC_QMI_GEOFENCE_ORIGIN_MO,
  GFC_QMI_GEOFENCE_ORIGIN_MAX = 0x1FFFFFFF
} gfc_qmi_geofence_origin_rsp_e_type;


/* The bitmask is used to identify the different types of Geofence breach
   events QMI are interested in. */
typedef uint32 gfc_qmi_geofence_dwell_type_mask;

/* No dwell event is requested  */
#define  GFC_QMI_GEOFENCE_DWELL_TYPE_NONE_MASK  GFC_QMI_SET_U32BITMASK_BIT(0) /*0x0001*/
/* Dwell event when the handset enters the geofence */
#define GFC_QMI_GEOFENCE_DWELL_TYPE_INSIDE_MASK  GFC_QMI_SET_U32BITMASK_BIT(1) /*0x0002*/
  /* Dwell event when the handset exits the geofence */
#define  GFC_QMI_GEOFENCE_DWELL_TYPE_OUTSIDE_MASK  GFC_QMI_SET_U32BITMASK_BIT(2) /*0x0004*/

/* Bitmask to indicate the valid optional parameters for adding the geofence */
typedef uint32 gfc_qmi_geofence_valid_opt_param_req_mask;
#define GFC_QMI_GEOFENCE_VALID_OPT_PARAM_REQ_START_TIME_MASK        GFC_QMI_SET_U32BITMASK_BIT(0) /*0x0001*/
#define GFC_QMI_GEOFENCE_VALID_OPT_PARAM_REQ_DURATION_MASK          GFC_QMI_SET_U32BITMASK_BIT(1) /*0x0002*/
#define GFC_QMI_GEOFENCE_VALID_OPT_PARAM_REQ_CONFIDENCE_MASK        GFC_QMI_SET_U32BITMASK_BIT(2) /*0x0004*/
#define GFC_QMI_GEOFENCE_VALID_OPT_PARAM_REQ_RESPONSIVENESS_MASK    GFC_QMI_SET_U32BITMASK_BIT(3) /*0x0008*/

/* The bitmask is used by QMI to identify the optional requested features for the geofence. */
typedef uint32 gfc_qmi_geofence_opt_feature_req_mask;
  /* Mask to indicate if the Geofence needs to be saved in the modem.
     If the Geofence is saved in the modem then the EFS needs on the modem
     need to be accounted for.
  */
#define GFC_QMI_GEOFENCE_OPT_FEATURE_REQ_PERSISTENCE_MASK         GFC_QMI_SET_U32BITMASK_BIT(0) /*0x0001*/
  /* Mask to indicate if position required to be reported when breached */
#define GFC_QMI_GEOFENCE_OPT_FEATURE_REQ_POS_REQ_ON_BREACH_MASK   GFC_QMI_SET_U32BITMASK_BIT(1) /*0x0002*/

/* The data type used by QMI for request to add the Geofence in the modem.  */

typedef struct

{
  /* Bitmask to indicate which of the optional parameters are valid.
     <value specification> : Mandatory. */
  gfc_qmi_geofence_valid_opt_param_req_mask valid_opt_param_req_mask;
  /* Bitmask indicating the additional features for the geofence.
     <value specification> : Optional.
     <default value if the value of the parameter is not valid> : 0 */
  gfc_qmi_geofence_opt_feature_req_mask opt_feature_req_mask;

  /* The start time indicates the duration after which the geofence would be active in the system.This value can be 0 if it is not desired,
     then the geofence is active immediately.The start time of the geofence is relative to the time the geofence was added. This value is the duration
     , not an absolute value .
     Ex:: If the start time of the geofence is 30 seconds, then the geofence would be active after 30 seconds from the time the geofence was added.
     <units> : seconds
     <parameter kind> : Optional
     <default value if the value of the parameter is not valid> : 0 */
  uint32 start_time_s;
  /* The duration until which the geofence is active , relative to the start tim eof the geofence. If this value is not specifed then the geofence is active for ever.
     This value is the duration, not an absolute value .
     <units> : seconds
     <value specification> : Optional
     <default value if value of the parameter is not valid> : 0xFFFFFFFF */
  uint32 duration_s;

  /* Interested breach event type.These bits are used to identify the interested breach events for QMI.
     <value specification> : Mandatory */
  gfc_qmi_geofence_breach_event_req_mask breach_event_req_mask;


  /* Target area provided by QMI for the Geofence. */
  gfc_qmi_geofence_target_area_req_s_type target_area;

  /* Responsiveness of the geofence.
     <value specification> : Optional.
     <default value if the value of the parameter is not valid> : GFC_QMI_RESPONSIVENESS_MEDIUM */
  gfc_qmi_responsiveness_e_type responsiveness;

  /* Confidence factor for the geofence.
     <value specification> : Optional.
     <default value if the value of the parameter is not valid> : GFC_QMI_CONFIDENCE_MEDIUM */
  gfc_qmi_confidence_e_type confidence;

  /* Unique transaction ID generated by the QMI for the geofencing operation requested.
     This transaction ID is returned to the QMI by the GFC_QMI client in its response message when the Geofence operation is complete.
     The transaction ID is used only while adding the geofences. Once the geofence ID is returned the transaction id should not be used for that geofence.
     <value specification> : Mandatory
  */
  uint32 transaction_id;

  /*
    Dwell time for geofence in seconds.
    <value specification> : Optional.
    <default value if the value of the parameter is not valid> : 0
  */
  uint32 q_dwell_time_s;

  /*
    Dwell type for geofence.
    <value specification> : Optional.
    <default value if the value of the parameter is not valid> : If dwell type is specified to be valid then the default value is IN|OUT
  */
  gfc_qmi_geofence_dwell_type_mask q_dwell_type_mask;

  /* Geofence custom responsiveness in seconds.
     The user can input a range of explicit responsiveness values in custom responsiveness.
     The range of values we can use is 1 second to 65535 secs. 
     */
  uint32 q_custom_responsiveness_value;

}gfc_qmi_add_geofence_req_s_type;

/* Bitmask to indicate the mutable request parameters for editing the geofence. */
typedef uint32 gfc_qmi_edit_geofence_param_mask;
#define GFC_QMI_EDIT_GEOFENCE_PARAM_START_TIME_MASK          GFC_QMI_SET_U32BITMASK_BIT(0)/*0x0001*/
#define GFC_QMI_EDIT_GEOFENCE_PARAM_DURATION_MASK            GFC_QMI_SET_U32BITMASK_BIT(1)/*0x0002*/
#define GFC_QMI_EDIT_GEOFENCE_PARAM_RESPONSIVENESS_MASK      GFC_QMI_SET_U32BITMASK_BIT(2)/*0x0004*/
#define GFC_QMI_EDIT_GEOFENCE_PARAM_CONFIDENCE_MASK          GFC_QMI_SET_U32BITMASK_BIT(3)/*0x0008*/
#define GFC_QMI_EDIT_GEOFENCE_PARAM_BREACH_EVENT_TYPE_MASK   GFC_QMI_SET_U32BITMASK_BIT(4)/*0x0010*/

/* The structure used by GFC_QMI Client for editing the mutable request parameters of the geofence. */
typedef struct
{
  /* The parameters to be edited. */
  gfc_qmi_edit_geofence_param_mask edit_param_mask;
  uint32 geofence_id;
  uint32 start_time_s;
  /* The duration of the geofence in seconds.
     If the geofence is not yet active, the duration it is from start time of the geofence.
     If the geofence is already active, the duration is from present time */

  uint32 duration_s;
  gfc_qmi_responsiveness_e_type responsiveness;
  gfc_qmi_confidence_e_type confidence;
  gfc_qmi_geofence_breach_event_req_mask breach_event_req_mask;
  /* Unique transaction ID generated by the QMI for the geofencing operation requested.
     This transaction ID is returned to the QMI by the GFC_QMI client in its response message when the Geofence operation is complete.
  */
  uint32 transaction_id;
}gfc_qmi_edit_geofence_req_s_type;

/* The structure used by GFC_QMI Client for querying a particular geofence. */
typedef struct
{
  /* The geofence ID to be queried */
  uint32 geofence_id;
  /*
     Unique transaction ID generated by the QMI for the geofencing operation requested.
     This transaction ID is returned to the QMI by the GFC_QMI client in its response message when the Geofence operation is complete.
  */
  uint32 transaction_id;
}gfc_qmi_query_geofence_req_s_type;

/* The structure used by GFC_QMI Client for deleting a particular geofence. */
typedef struct
{
  /* The geofence ID to be deleted */
  uint32 geofence_id;
  /*
     Unique transaction ID generated by the QMI for the geofencing operation requested.
     This transaction ID is returned to the QMI by the GFC_QMI client in its response message when the Geofence operation is complete.
  */
  uint32 transaction_id;
}gfc_qmi_delete_geofence_req_s_type;

/* The structure used by GFC_QMI Client for deleting a particular geofence. */
typedef struct
{
  /*
     Unique transaction ID generated by the QMI for the geofencing operation requested.
     This transaction ID is returned to the QMI by the GFC_QMI client in its response message when the Geofence operation is complete.
  */
  uint32 transaction_id;
  /* The type of geofence whose handles are requested */
  gfc_qmi_geofence_origin_rsp_e_type origin;
}gfc_qmi_query_geofence_ids_req_s_type;

/*Report GNSS unavailability after a timeout period */
#define GFC_QMI_GM_ENGINE_CONF_GNSS_UNAVAIL_TIMEOUT_MASK   GFC_QMI_SET_U32BITMASK_BIT(0)/*0x0001*/
/*Configure the max geofences supported by GM  */
#define GFC_QMI_GM_ENGINE_CONF_MAX_GF_MASK                        GFC_QMI_SET_U32BITMASK_BIT(1)/*0x0002*/
/*Enable external position injection usage by GM */
#define GFC_QMI_GM_ENGINE_CONF_ENABLE_CPI_USAGE_MASK               GFC_QMI_SET_U32BITMASK_BIT(2)/*0x0004*/
/* Enable motion detection by GM using sensors */
#define GFC_QMI_GM_ENGINE_CONF_ENABLE_SENSORS_FOR_MOTION_DETECTION_MASK  GFC_QMI_SET_U32BITMASK_BIT(3)/*0x0008*/
/* Enable motion detection by GM using wifi */
#define GFC_QMI_GM_ENGINE_CONF_ENABLE_WIFI_FOR_MOTION_DETECTION_MASK     GFC_QMI_SET_U32BITMASK_BIT(4)/*0x0010*/
/* Enable motion detection by GM using WWAN */
#define GFC_QMI_GM_ENGINE_CONF_ENABLE_WWAN_FOR_MOTION_DETECTION_MASK     GFC_QMI_SET_U32BITMASK_BIT(5)/*0x0020*/
/*Use Session timeout*/
#define GFC_QMI_GM_ENGINE_CONF_GNSS_POS_QOS_SESS_TIMEOUT_MASK   GFC_QMI_SET_U32BITMASK_BIT(6)/*0x0040*/

/*Enable batch breach reporting from GM */
#define GFC_QMI_GM_ENGINE_CONF_ENABLE_BATCH_BREACH_MASK         GFC_QMI_SET_U32BITMASK_BIT(7)/*0x0080*/

/*Max Position unc accepted*/
#define GFC_QMI_GM_ENGINE_CONF_GNSS_MAX_POS_UNC_ACCEPTED_MASK   GFC_QMI_SET_U32BITMASK_BIT(8)/*0x0100*/

/*Use NW asst fixes*/
#define GFC_QMI_GM_ENGINE_CONF_USE_NW_ASST_FIXES_MASK          GFC_QMI_SET_U32BITMASK_BIT(9)/*0x0200*/

/*High Responsiveness Setting*/
#define GFC_QMI_GM_ENGINE_CONF_HIGH_RESPONSIVENESS_CONFIG_MASK   GFC_QMI_SET_U32BITMASK_BIT(10)/*0x0400*/

/* GM CPI request rate  */
#define GFC_QMI_GM_ENGINE_CONF_CPI_REQUEST_RATE_CONFIG_MASK   GFC_QMI_SET_U32BITMASK_BIT(11)/*0x0800*/

/* GM Medium responsiveness backoff config   */
#define GFC_QMI_GM_ENGINE_CONF_MED_RESP_BACKOFF_MASK           GFC_QMI_SET_U32BITMASK_BIT(12)/*0x1000*/

/* GM challenging gps env backoff min config  */
#define GFC_QMI_GM_ENGINE_CONF_CHAL_GPS_ENV_BO_MIN_MASK               GFC_QMI_SET_U32BITMASK_BIT(13)/*0x2000*/

/* GM challenging gps env backoff max config  */
#define GFC_QMI_GM_ENGINE_CONF_CHAL_GPS_ENV_BO_MAX_MASK               GFC_QMI_SET_U32BITMASK_BIT(14)/*0x4000*/

/* The motion sensing distance for GPS in case of challenging GPS env  */
#define GFC_QMI_GM_ENGINE_CONF_CHAL_GPS_ENV_MS_DIST_GPS_MASK          GFC_QMI_SET_U32BITMASK_BIT(15)/*0x8000*/

/* The motion sensing distance for CPI in case of challenging GPS env  */
#define GFC_QMI_GM_ENGINE_CONF_CHAL_GPS_ENV_MS_DIST_CPI_MASK          GFC_QMI_SET_U32BITMASK_BIT(16)/*0x10000*/

/* The motion sensing speed config  */
#define GFC_QMI_GM_ENGINE_CONF_MS_SPEED_MASK                          GFC_QMI_SET_U32BITMASK_BIT(17)/*0x20000*/

/* GM Operation Mode  */
#define GFC_QMI_GM_ENGINE_CONF_GM_OPERATION_MODE_CONFIG_MASK    GFC_QMI_SET_U32BITMASK_BIT(18)/*0x40000*/

/* Enable GM registration with LOWI service. */
#define GFC_QMI_GM_ENGINE_CONF_LOWI_REGISTER_ENABLE             GFC_QMI_SET_U32BITMASK_BIT(19)/*0x80000*/

/* Subscribe to the different services supported on LOWI from GM  */
#define GFC_QMI_GM_ENGINE_CONF_LOWI_SUBSCRIPTION_MASK           GFC_QMI_SET_U32BITMASK_BIT(20)/*0x100000*/

/* Enable different registration timers for LOWI  */
#define GFC_QMI_GM_ENGINE_CONF_LOWI_REG_TIMER_MASK                  GFC_QMI_SET_U32BITMASK_BIT(21)/*0x200000*/

/* The retry count for short term registration timer   */
#define GFC_QMI_GM_ENGINE_CONF_LOWI_SHORT_TERM_TIMER_RETRY_CNT      GFC_QMI_SET_U32BITMASK_BIT(22)/*0x400000*/

/* GM Comprehensive Logging Rate Mask  */
#define GFC_QMI_GM_ENGINE_CONF_COMP_LOGGING_RATE_CONFIG_MASK    GFC_QMI_SET_U32BITMASK_BIT(23)/*0x800000*/

/* GM IOD Config Mask  */
#define GFC_QMI_GM_ENGINE_CONF_IOD_CONFIG_MASK                  GFC_QMI_SET_U32BITMASK_BIT(24)/*0x1000000*/

/*Enable Low Power Wifi usage by GM */
#define GFC_QMI_GM_ENGINE_CONF_ENABLE_LPW_USAGE_MASK               GFC_QMI_SET_U32BITMASK_BIT(25)/*0x2000000*/

/* GM filter WLAN freq Mask  */
#define GFC_QMI_GM_ENGINE_CONF_FILTER_WLAN_FREQ_MASK            GFC_QMI_SET_U32BITMASK_BIT(26)/*0x4000000*/


/* The structure used by GFC_QMI Client for set geofence engine config. */
typedef struct
{
  /*
     Unique transaction ID generated by the QMI for the geofencing operation requested.
     This transaction ID is returned to the QMI by the GFC_QMI client in its response message when the Geofence operation is complete.
  */
  uint32 transaction_id;

  /* Mask indicating which config parameters need to be set for the GM engine */
  uint64 t_gfc_qmi_gm_engine_conf_mask;

  /*
    In a bad GNSS environment,the timeout after which the geofence engine sends out a GNSS unavailable indication.
  */
  uint32 gnss_unavailable_indication_timeout;

  /*Enable Sensors for motion detection in GM*/
  boolean  q_gm_enable_sensor_motion_detection;

  /*Enable wifi for motion detection in GM*/
  boolean  q_gm_enable_wifi_motion_detection;

  /*Enable wwan for motion detection in GM*/
  boolean  q_gm_enable_wwan_motion_detection;

  /*GNSS Max Session timeout to be used by GM in position requests*/
  uint8  gnss_pos_qos_sess_timeout;

  /*Enable Position Injection Usage in GM*/
  boolean  q_gm_enable_position_injection_usage;

  uint32 q_max_gf;

  /* Enable/disable batch breach report from GM */
  boolean b_enable_batch_breach_report;

  /* Max pos unc accepted by geofence engine */
  uint32 q_gm_gnss_max_pos_unc_accepted;

  /* Enable GM use of NW asst fixes */
  boolean b_use_nw_asst_fixes;

  /*High Responsiveness Config*/
  uint32 q_gm_high_responsiveness_config;

  /* GM CPI request rate */
  uint32 q_gm_cpi_request_rate;

  /*GM Medium resp. backoff in seconds */
  uint32 q_gm_med_resp_backoff;

  /*GM Pos Operation mode*/
  pdsm_pd_session_operation_e_type q_gm_pos_operation_mode;

  /* Minimum backoff when in Challenging GPS Env in seconds */
  uint32 q_gm_chal_gps_env_bo_min;

  /* Maximum backoff when in Challenging GPS Env in seconds */
  uint32 q_gm_chal_gps_env_bo_max;

  /* The motion sensing distance for GPS in case of challenging GPS env */
  uint32 q_gm_chal_gps_env_ms_dist_gps;

  /* The motion sensing distance for CPI in case of challenging GPS env */
  uint32 q_gm_chal_gps_env_ms_dist_cpi;

  /*The motion sensing speed config*/
  gm_motion_state_speed_config  q_gm_ms_speed_config;

  /* Enable GM LOWI registration */
  boolean b_lowi_reg_enable;

  /* Subscribe to different services on LOWI */
  uint32 q_lowi_subs_mask;

  /* Mask to identify which timer to enable for LOWI registration retry */
  uint32 q_lowi_enable_reg_timer_mask; 

  /* Maximum retry count for the short term timer  */
  uint32 q_lowi_short_term_timer_max_retry_cnt; 

  /*Comprehensive Logging Rate*/
  uint32 q_gm_comp_logging_rate;  

  /*The ALS IOD sensor config mask used by Geofencing*/
  uint32 q_sm_gm_iod_config_switch_mask;

  /* GM filtering of WLAN frequencies */
  uint32 q_sm_gm_filter_wlan_freq_mask;

  /*Enable Low Power Wifi in GM*/
  boolean  q_gm_enable_lpw_usage;
}gfc_qmi_set_geofence_engine_config_req_s_type;


/********* RESPONSE Data types GFC_QMI -> QMI ***********************/

/* The position fix sent by GFC_QMI to QMI  */
typedef struct
{
  pdsm_utc_time_s_type utc_time; /*  Utc time containing year,month,utc_days,hours,nin,secs,tenths*/
  double  latitude;              /* Latitude, referenced to WGS-84 */
  double  longitude;             /* Longitude, referenced to WGS-84 */
  float   loc_uncrtnty_ang;      /* Angle of axis wrt true north
                                    for position uncertainty */
  float   loc_uncrtnty_a;        /* Standard deviation of axis along angle
                                    specified for position uncertainty */
  float   loc_uncrtnty_p;        /* Standard deviation of axis perpendicular to
                                   angle specified for position uncertainty */
  byte    velocity_incl;         /* Velocity information included */
  float  velocity_hor;          /* Horizontal velocity magnitude */
  byte    heading_valid;
  uint16  heading;               /* Heading */
  float    velocity_ver;          /* Vertical velocity magnitude */
  byte    height_incl;           /* Height information included */
  byte    height_uncrtnty_incl;  /* Height uncertainty included */
  float   height;                /* Height, referenced to WGS-84 */
  float   loc_uncrtnty_v;        /* Standard deviation of vertical
                                    error for position uncertainty */
  float   headingUnc;            /* Heading Uncertainty */
  float   vertUnc;               /* Vertical uncertainty */
  float   speedUnc;              /* 3-D speed uncertainty */
  byte    horConfidence;         /* Horizontal uncertainty confidence.*/
  byte    vertConfidence;        /* Vertical uncertainty confidence.*/
  qmiLocDOPStructT_v02 DOP;      /* Dilution of precision associated with this position. */
  uint32  gnssSvUsedList_len;    /* The number of SVs */
  uint16  gnssSvUsedList[QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02];  /* Each entry in the list contains the SV ID of a satellite
                                                                    used for calculating this position report. */
}qfc_qmi_pos_data_rsp_s_type;


/* The data type is used to synchronously return the status of the Geofencing operation from GFC_QMI . */
typedef enum
{
  GFC_QMI_OPERATION_STATUS_SUCCEEDED,
  GFC_QMI_OPERATION_STATUS_FAILED,
  GFC_QMI_OPERATION_STATUS_PENDING,
  GFC_QMI_OPERATION_STATUS_MAX = 0x1FFFFFFF
} gfc_qmi_operation_status_e_type;

/* The data type is used for the return codes associated with the Geofencing operations . */
typedef enum
{
  /*Return codes*/
  GFC_QMI_ERROR_NONE = 0,
  /* Communication between the GFC_QMI client and the GM core failed. */
  GFC_QMI_ERROR_COMMUNICATION_FAILURE,
  /* Internal error in the GM. Insufficient memory to allocate for the geofence */
  GFC_QMI_ERROR_INSUFFICIENT_MEMORY,
  /*Geofence is not programmed*/
  GFC_QMI_ERROR_GEOFENCE_NOT_FOUND,
  /* Invalid parameter for the geofence operation  */
  GFC_QMI_ERROR_INVALID_PARAMETER,
  /* The mandtatory parameter not specified. */
  GFC_QMI_ERROR_MANDATORY_PARAMETER_UNSPECIFIED,
  /* Invalid client handle */
  GFC_QMI_ERROR_INVALID_CLIENT_HANDLE,
  /*Maximum geofences are already programmed*/
  GFC_QMI_ERROR_MAX_GEOFENCE_PROGRAMED,
  /*Geofence does not belong to this client*/
  GFC_QMI_ERROR_GEOFENCE_WRONG_CLIENT,
  /*Params check failed*/
  GFC_QMI_ERROR_INCORRECT_PARAMS,
  /* General failure case.  */
  GFC_QMI_ERROR_GENERAL,
  GFC_QMI_ERROR_MAX = 0x1FFFFFFF,
}gfc_qmi_operation_result_e_type;


/* The data type is used to provide general notifications back to QMI */
typedef enum
{
  /*Position fix cannot be determined because GPS unavail*/
  GFC_QMI_GENERAL_NOTIFY_GPS_OFF,
  /*GPS is now avail*/
  GFC_QMI_GENERAL_NOTIFY_GPS_ON,
  /*The time returned by the modem APIs is invalid. */
  GFC_QMI_GENERAL_NOTIFY_TIMESTAMP_INVALID,
  /* Out of service area, no Cell ID coverage info available*/
  GFC_QMI_GENERAL_NOTIFY_OOS,
  GFC_QMI_GENERAL_NOTIFY_MAX_TYPE = 0x1FFFFFFF
} gfc_qmi_general_notify_rsp_e_type;


/* The data type used to indicate the reason for deletion of the geofence  */
typedef enum
{
  /* Geofence deleted because of NI client request.  */
  GFC_QMI_GEOFENCE_DEL_REASON_NI_CLIENT_REQ,
  /* Geofence deleted because of MO client request.  */
  GFC_QMI_GEOFENCE_DEL_REASON_MO_CLIENT_REQ,
  /* Geofence deleted because its duration is up */
  GFC_QMI_GEOFENCE_DEL_REASON_DURATION ,
  GFC_QMI_GEOFENCE_DEL_REASON_UNKNOWN ,
  GFC_QMI_GEOFENCE_DEL_REASON_MAX = 0x1FFFFFFF
} gfc_qmi_geofence_del_reason_rsp_e_type;

/* The data type used by GFC_QMI Client for sending the data to QMI resulting from Geofence add request from it. */
typedef struct
{
  /* Unique transaction ID generated by the QMI for the geofencing operation requested. This transaction ID is returned to the QMI by
     the GFC_QMI client in its response message when the Geofence operation is complete.
     For geofences added by the network or unsolicited add response messgaes will have transaction ID as 0xFFFF . */
  uint32 transaction_id;
  /* The id generated for each succesfully created geofence . This id is passed back from GFC_QMI Client to QMI when the geofence
     is created succesfully.   */
  uint32 geofence_id;
  /* Origin of the geofence . NI/MO */
  gfc_qmi_geofence_origin_rsp_e_type origin;
  /* Return code from the add geofence operation */
  gfc_qmi_operation_result_e_type return_code;
}gfc_qmi_add_geofence_rsp_s_type;

/* The structure used by GFC_QMI Client for sending the data to QMI resulting from Geofence delete request from it. */
typedef struct
{
  /* The geofence to be deleted. */
  uint32 geofence_id;
  /* Origin of the geofence . NI/MO */
  gfc_qmi_geofence_origin_rsp_e_type origin;

  /* The mask which indicates the reason for deletion of the indicated geofence */
  gfc_qmi_geofence_del_reason_rsp_e_type del_reason;

  uint32 transaction_id;

  /* Return code from the delete geofence operation */
  gfc_qmi_operation_result_e_type return_code;
}gfc_qmi_delete_geofence_rsp_s_type;


/* The structure used by GFC_QMI Client for sending the data to QMI resulting from Geofence edit request from it. */
typedef struct
{
  /* The geofence which was edited. */
  uint32 geofence_id;
  /* The transaction ID which is passed in by the QMI.This will allow the QMI to identify the
     operation in case it of multiple edit operations on the same geofence. */
  uint32 transaction_id;
  /* The parameters which were edited succesfully*/
  gfc_qmi_edit_geofence_param_mask edit_mask;
  /* Return code from the edit operation.
    If any of the edit parameters failed, return_code
    will be set GFC_QMI_ERROR_GENERAL ToDo:: need to debate */
  gfc_qmi_operation_result_e_type return_code;
}gfc_qmi_edit_geofence_rsp_s_type;

/* The structure used by GFC_QMI Client for sending the data to QMI resulting from Geofence query request from it. */
typedef struct
{
  /* The geofence to be queried. */
  uint32 geofence_id;
  /* The transaction ID which is passed in by the QMI.This will allow the QMI to identify the
     operation in case it of multiple query operations on the same geofence. */
  uint32 transaction_id;
  /* Origin of the geofence */
  gfc_qmi_geofence_origin_rsp_e_type geofence_origin;
  /* INSIDE/OUTSIDE/UNKNOWN */
  gfc_qmi_pos_rsp_e_type pos_status;
  /* Specification of the geofence */
  gfc_qmi_geofence_target_area_req_s_type target_area;
  /* The remaining time this geofence would be active in secs.  */
  uint32 active_duration_s;
  /* Responsiveness value for the queried geofence */
  gfc_qmi_responsiveness_e_type responsiveness;
  /* Confidence value for the queried geofence */
  gfc_qmi_confidence_e_type confidence;

  /* Return code from the query operation.
    If any of the edit parameters failed, return_code
    will be set GFC_QMI_ERROR_GENERAL ToDo:: need to debate */
  gfc_qmi_operation_result_e_type return_code;

}gfc_qmi_query_geofence_rsp_s_type;

/* Bitmask to indicate if the position fix is reported on a breach */
typedef uint32 gfc_qmi_pos_report_on_breach_mask;
#define GFC_QMI_POS_REPORT_ON_BREACH_FALSE   0x0
#define GFC_QMI_POS_REPORT_ON_BREACH_TRUE    GFC_QMI_SET_U32BITMASK_BIT(0)/*0x0001*/

/* Bitmask to indicate if the confidence is reported on a breach */
typedef uint32 gfc_qmi_confidence_report_on_breach_mask;
#define GFC_QMI_CONFIDENCE_REPORT_ON_BREACH_FALSE   0x0
#define GFC_QMI_CONFIDENCE_REPORT_ON_BREACH_TRUE    GFC_QMI_SET_U32BITMASK_BIT(0)/*0x0001*/



/* The structure used by GFC_QMI Client for sending the breach notifications to QMI of the Geofence in modem.*/
typedef struct
{
  /* The geofence id with respect to which the handset changed its status . */
  uint32 geofence_id;

  /* The type of the breach which happened */
  gfc_qmi_breach_geofence_rsp_e_type breach_type;

  /* Origin of the geofence . NI/MO */
  gfc_qmi_geofence_origin_rsp_e_type origin;

  gfc_qmi_pos_report_on_breach_mask pos_rep_mask;

  /* The GPS position information */
  qfc_qmi_pos_data_rsp_s_type pos_fix;

  gfc_qmi_confidence_report_on_breach_mask confidence_rep_mask;

  /* Confidence associated with breach */
  gfc_qmi_confidence_e_type confidence;

}gfc_qmi_breach_geofence_rsp_s_type;

/* The data type used to report theProximity type of the Proximity Indication. */
typedef enum
{
  GFC_QMI_PROXIMITY_IND_TYPE_IN,
  GFC_QMI_PROXIMITY_IND_TYPE_OUT,
  GFC_QMI_PROXIMITY_IND_TYPE_MAX = 0x1FFFFFFF
} gfc_qmi_proximity_ind_e_type;

/* The structure used by GFC_QMI Client for sending the proximity notifications to QMI of the Geofence in modem.*/
typedef struct
{
  /* The geofence id with respect to which the handset changed its status . */
  uint32 geofence_id;

  /* Proximity type of the Indication */
  gfc_qmi_proximity_ind_e_type proximity_type;

  /*Whether Context Id is valid*/
  uint8 context_id_valid;

  /*Context Id associated with the proximity indication*/
  uint32 context_id;
}gfc_qmi_proximity_geofence_rsp_s_type;



/* The structure used by GFC_QMI Client for sending the batch breach notifications to QMI of the Geofence in modem.*/
typedef struct
{

  /* The type of the breach which happened */
  gfc_qmi_breach_geofence_rsp_e_type breach_type;

  /* Origin of the geofence . NI/MO */
  gfc_qmi_geofence_origin_rsp_e_type origin;

  gfc_qmi_pos_report_on_breach_mask pos_rep_mask;

  /* The GPS position information */
  qfc_qmi_pos_data_rsp_s_type pos_fix;

  gfc_qmi_confidence_report_on_breach_mask confidence_rep_mask;

  /* Confidence associated with breach */
  gfc_qmi_confidence_e_type confidence;

  /* Number of elements in the discrete list */
  uint32 q_discrete_list_len;

  /* Number of elements in the continuous list */
  uint32 q_cont_list_len;

  /* List containing discrete GIDs and continuous GIDs */
  uint32 data[1];

}gfc_qmi_batch_breach_geofence_rsp_s_type;


/* The structure used by GFC_QMI Client for sending the batch dwell notifications to QMI of the Geofence in modem.*/
typedef struct
{

  /* The type of the dwell which happened */
  gfc_qmi_geofence_dwell_type_mask dwell_type;

  /* Origin of the geofence . NI/MO */
  gfc_qmi_geofence_origin_rsp_e_type origin;

  /* Mask to indicate if the position is reported on dwell */
  gfc_qmi_pos_report_on_breach_mask pos_rep_mask;

  /* The GPS position information */
  qfc_qmi_pos_data_rsp_s_type pos_fix;

  /* Number of elements in the discrete list */
  uint32 q_discrete_list_len;

  /* Number of elements in the continuous list */
  uint32 q_cont_list_len;

  /* List containing discrete GIDs and continuous GIDs */
  uint32 data[1];

}gfc_qmi_batch_dwell_geofence_rsp_s_type;

/* The structure used by GFC_QMI Client for sending the geofence ids of all the geofences int he system .*/
typedef struct
{
  /* The transaction ID which is passed in by the QMI.This will allow the QMI to identify the
     operation in case it of multiple query operations on the same geofence. */
  uint32 transaction_id;
  /* The number of geofence Ids present in the system */
  uint32 num_ids;
  /* The actual geofence Ids */
  uint32 *geofence_ids;

  /* The origin of the geofence */
  gfc_qmi_geofence_origin_rsp_e_type origin;

  /* Return code from the operation. */
  gfc_qmi_operation_result_e_type return_code;

}gfc_qmi_query_geofence_ids_rsp_s_type;


/* The structure containing the list if continuous GIDs in the form of a range */
typedef struct
{
  /* The low range of the gid */
  int gid_low;
  /* The high range of the gid */
  int gid_high;
}gfc_qmi_cont_gid_s_type;

/* The structure used by GFC_QMI Client for setting the GM engine configuration .*/
typedef struct
{
  /* The transaction ID which is passed in by the QMI.This will allow the QMI to identify the
     operation in case it of multiple query operations on the same geofence. */
  uint32 transaction_id;

  /* Return code from the operation. */
  gfc_qmi_operation_result_e_type return_code;

  /* Mask indicating which config parameters were not set in the GM engine */
  uint64 t_gfc_qmi_gm_engine_conf_mask;

}gfc_qmi_set_geofence_engine_config_rsp_s_type;



/********* Function types used by GFC_QMI to communicate with the QMI for geofence operations *************/

/*===========================================================================

FUNCTION TYPE: gfc_qmi_add_geofence_rsp_f_type

DESCRIPTION:
  This function is used by GFC_QMI to notify the status of geofence add request operation to QMI.
===========================================================================*/
typedef void (gfc_qmi_add_geofence_rsp_f_type)
(
  const gfc_qmi_add_geofence_rsp_s_type *add_rsp
);

/*===========================================================================

FUNCTION TYPE: gfc_qmi_delete_geofence_rsp_f_type

DESCRIPTION:
  This function is used by GFC_QMI to notify the status of geofence delete request operation to QMI.
===========================================================================*/
typedef void (gfc_qmi_delete_geofence_rsp_f_type)
(
  const gfc_qmi_delete_geofence_rsp_s_type *delete_rsp
);

/*===========================================================================

FUNCTION TYPE: gfc_qmi_purge_geofence_rsp_f_type

DESCRIPTION:
  This function is used by GFC_QMI to notify the status of geofence purge request operation to QMI.
===========================================================================*/
typedef void (gfc_qmi_purge_geofence_rsp_f_type)
(
  const gfc_qmi_operation_result_e_type notify_code
);

/*===========================================================================

FUNCTION TYPE: gfc_qmi_edit_geofence_rsp_f_type

DESCRIPTION:
  This function is used by GFC_QMI to send the response to the
  edit request for the geofence.

===========================================================================*/
typedef void (gfc_qmi_edit_geofence_rsp_f_type)
(
  const gfc_qmi_edit_geofence_rsp_s_type *edit_rsp
);

/*===========================================================================

FUNCTION TYPE: gfc_qmi_query_geofence_rsp_f_type

DESCRIPTION:
  This function is used by GFC_QMI to notify the status of geofence query request operation to QMI.
===========================================================================*/
typedef void (gfc_qmi_query_geofence_rsp_f_type)
(
  const gfc_qmi_query_geofence_rsp_s_type *query_rsp
);

/*===========================================================================

FUNCTION TYPE: gfc_qmi_breach_geofence_rsp_f_type

DESCRIPTION:
  This function is used by GFC_QMI to notify the QMI when the position of the handset with
  respect to the geofence changes .
===========================================================================*/
typedef void (gfc_qmi_breach_geofence_rsp_f_type)
(
  const gfc_qmi_breach_geofence_rsp_s_type *breach_rsp
);

/*===========================================================================

FUNCTION TYPE: gfc_qmi_general_notify_rsp_f_type

DESCRIPTION:
  This function is used by GFC_QMI to provide general notifications during geofence operation.For example
the Geofence operation fails because the GPS engine is turned OFF, time retuned by modem APIs is invalid etc..
===========================================================================*/
typedef void (gfc_qmi_general_notify_rsp_f_type)
(
  const gfc_qmi_general_notify_rsp_e_type notify_code
);

/*===========================================================================

FUNCTION TYPE: gfc_qmi_query_geofence_ids_rsp_f_type

DESCRIPTION:
  This function is used by GFC_QMI Client to send all the available
  geofence ids in the system.

Parameters:
  num_geofence_ids: The number of ids being sent to the QMI.
  geofence_ids: The geofence ids available in the system.

===========================================================================*/
typedef void (gfc_qmi_query_geofence_ids_rsp_f_type)
(
  const gfc_qmi_query_geofence_ids_rsp_s_type *query_ids_rsp
);

/*===========================================================================

FUNCTION TYPE: gfc_qmi_set_geofence_engine_config_rsp_f_type

DESCRIPTION:
  This function is used by GFC_QMI Client to send the status of the set geofence
  engine config request.

===========================================================================*/
typedef void (gfc_qmi_set_geofence_engine_config_rsp_f_type)
(
  const gfc_qmi_set_geofence_engine_config_rsp_s_type *set_gf_eng_conf_rsp
);

/*===========================================================================

FUNCTION TYPE: gfc_qmi_batch_breach_geofence_rsp_f_type

DESCRIPTION:
  This function is used by GFC_QMI to notify the QMI when the position of the handset with
  respect to the geofence changes .
===========================================================================*/
typedef void (gfc_qmi_batch_breach_geofence_rsp_f_type)
(
  const gfc_qmi_batch_breach_geofence_rsp_s_type *breach_rsp
);

/*===========================================================================

FUNCTION TYPE: gfc_qmi_batch_dwell_geofence_rsp_f_type

DESCRIPTION:
  This function is used by GFC_QMI to notify the QMI when the UE dwells in the geofence for
  the specified amount of time. 
===========================================================================*/
typedef void (gfc_qmi_batch_dwell_geofence_rsp_f_type)
(
  const gfc_qmi_batch_dwell_geofence_rsp_s_type *dwell_rsp
);


/*===========================================================================

FUNCTION TYPE: gfc_qmi_proximity_geofence_rsp_f_type

DESCRIPTION:
  This function is used by GFC_QMI to notify the QMI when the position of the handset with
  respect to the geofence changes .
===========================================================================*/
typedef void (gfc_qmi_proximity_geofence_rsp_f_type)
(
  const gfc_qmi_proximity_geofence_rsp_s_type *proximity_rsp
);

/*===========================================================================

FUNCTION TYPE: gfc_qmi_geofence_inject_tz_f_type

DESCRIPTION:
  This function is used by GFC_QMI to notify the QMI when the GF engine needs
  an injection of TimeZone information
===========================================================================*/
typedef void (gfc_qmi_geofence_inject_tz_f_type)
(
  void
);


/* The structure used by GFC_QMI for sending the status change notifications to QMI of the Geofence in modem.*/
typedef struct
{
  gfc_qmi_add_geofence_rsp_f_type     *add_rsp_func;
  gfc_qmi_delete_geofence_rsp_f_type  *delete_rsp_func;
  gfc_qmi_purge_geofence_rsp_f_type   *purge_rsp_func;
  gfc_qmi_edit_geofence_rsp_f_type    *edit_rsp_func;
  gfc_qmi_query_geofence_rsp_f_type   *query_gf_rsp_func;
  gfc_qmi_breach_geofence_rsp_f_type  *breach_rsp_func;
  gfc_qmi_general_notify_rsp_f_type             *general_notify_rsp_func;
  gfc_qmi_query_geofence_ids_rsp_f_type         *geofence_ids_rsp_func;
  gfc_qmi_set_geofence_engine_config_rsp_f_type *set_gf_eng_conf_rsp_func;
  gfc_qmi_batch_breach_geofence_rsp_f_type *batch_breach_rsp_func;
  gfc_qmi_proximity_geofence_rsp_f_type  *proximity_rsp_func;
  gfc_qmi_batch_dwell_geofence_rsp_f_type *batch_dwell_rsp_func;
  gfc_qmi_geofence_inject_tz_f_type   *tz_inject_func;  
}gfc_qmi_cb_rsp_s_type;

/********* APIs used by the QMI to communicate with GFC_QMI for geofence operations *************/


/*===========================================================================

FUNCTION gfc_qmi_register

DESCRIPTION
  This function allows the QMI to register with the GFC_QMI client.
  The QMI registers the following functions to receive the notification
  via callbacks asynchronously.


DEPENDENCIES
  None.

RETURN VALUE
  Boolean.
  1: Success
  2: Failure

SIDE EFFECTS
  none

===========================================================================*/
boolean gfc_qmi_register(
  const gfc_qmi_cb_rsp_s_type *geofence_func_cbs
);



/*===========================================================================
gfc_qmi_add_geofence

Description:
   This function is used by the QMI to add a Geofence, initiated by the apps side.This function immediately returns.
   When the Geofence is actually added by the GM it sends a notification to the GFC_QMI Client which inturn notifies
   the QMI that the Geofence is actually added.

Parameters:
  add_request: The data from QMI required to initialize and add a Geofence in the modem.
  return_code: The code which results in processing this operation.The actual response message to this
  operation is sent back by GFC_QMI Client to QMI through an asynchronous message.

Return value:
  The status data associated with the add Geofence request from QMI.
=============================================================================*/
gfc_qmi_operation_status_e_type gfc_qmi_add_geofence(
  const gfc_qmi_add_geofence_req_s_type *add_request,
  gfc_qmi_operation_result_e_type *return_code
);

/*===========================================================================
gfc_qmi_delete_geofence

Description:
   This function is used by the QMI to delete a Geofence, initiated by the apps side. This function immediately returns.
   When the Geofence is actually deleted by the GM it sends a notification to the GFC_QMI Client which inturn notifies
   the QMI that the Geofence is actually deleted.

Parameters:
  del_request: The geofence delete request data.
  return_code: The code which results in processing this operation. The actual response message to this
  operation is sent back by GFC_QMI Client to QMI through an asynchronous message.

Return value:
The status data associated with the delete Geofence request from QMI.
=============================================================================*/
gfc_qmi_operation_status_e_type gfc_qmi_delete_geofence(
  gfc_qmi_delete_geofence_req_s_type *del_request,
  gfc_qmi_operation_result_e_type *return_code
);


/*===========================================================================
gfc_qmi_purge_geofence

Description:
   This function is used by the QMI to purge the Geofence. This would delete all the geofences
   created by the QMI .
   There would be an asynchronous notification for the purge request from GM, which would contain
   the errors ( if any ) which occured during the purge operation.

Parameters:
  return_code: The code which results in processing this operation. The actual response message to this
  operation is sent back by GFC_QMI Client to QMI through an asynchronous message.

Return value:
The status data associated with the delete Geofence request from QMI.
=============================================================================*/
gfc_qmi_operation_status_e_type gfc_qmi_purge_geofence(
  gfc_qmi_operation_result_e_type *return_code
);


/*===========================================================================
gfc_qmi_edit_geofence

Description:
   This function is used by the QMI to edit the previously added geofence.
   Only mutable parameters can be edited. Only one parameter can be edited at a time.

Parameters:
  edit_request: The geofence edit request data .
  return_code: The code which results in processing this operation. The actual response message to this
  operation is sent back by GFC_QMI Client to QMI through an asynchronous message.

Return value:
  The status data associated with the status query request from QMI.

=============================================================================*/

gfc_qmi_operation_status_e_type gfc_qmi_edit_geofence(
  const gfc_qmi_edit_geofence_req_s_type *edit_request,
  gfc_qmi_operation_result_e_type *return_code
);


/*===========================================================================
gfc_qmi_query_geofence

Description:
   This function is used by the QMI to query the staus of a previously defined Geofence. This function immediately returns.
   When the status of the Geofence is known, the GM sends a notification to the GFC_QMI Client which inturn notifies
   the QMI giving the geofence status along with the position fix.

Parameters:
  query_gf_request: The geofence query request data .
  return_code: The code which results in processing this operation. The actual response message to
  this operation is sent back by GFC_QMI Client to QMI through an asynchronous message.

Return value:
  The status data associated with the status query request from QMI.

=============================================================================*/

gfc_qmi_operation_status_e_type gfc_qmi_query_geofence(
  const gfc_qmi_query_geofence_req_s_type *query_gf_request,
  gfc_qmi_operation_result_e_type *return_code
);



/*===========================================================================
gfc_qmi_query_geofence_ids

Description:
   This function is used by the QMI to query the geofence ids of all the added geofences in the
   system.

Parameters:

  query_gf_ids_request: The data for querying the geofence handles .
  return_code: The code which results in processing this operation. The actual response message to
  this operation is sent back by GFC_QMI Client to QMI through an asynchronous message.

Return value:
  The status data associated with the status query request from QMI.

=============================================================================*/

gfc_qmi_operation_status_e_type gfc_qmi_query_geofence_ids(
  const gfc_qmi_query_geofence_ids_req_s_type *query_gf_ids_request,
  gfc_qmi_operation_result_e_type *return_code
);

/*===========================================================================
gfc_qmi_set_geofence_engine_config

Description:
   This function is used by the QMI to set the geofence engine configuration

Parameters:
  query_gf_request: The geofence query request data .
  return_code: The code which results in processing this operation. The actual response message to
  this operation is sent back by GFC_QMI Client to QMI through an asynchronous message.

Return value:
  The status data associated with the status query request from QMI.

=============================================================================*/

gfc_qmi_operation_status_e_type gfc_qmi_set_geofence_engine_config(
  const gfc_qmi_set_geofence_engine_config_req_s_type *p_set_gf_engine_config_request,
  gfc_qmi_operation_result_e_type *return_code
);

/*===========================================================================
gfc_qmi_handle_batch_breach_req

Description:
   This function is used by the QMI to set the batch breach reporting from GM

Parameters:
  b_enable: Enable/disable batch breach reporting from GM

Return value:
  None

=============================================================================*/
void gfc_qmi_handle_batch_breach_req(boolean b_enable);

/*===========================================================================
gfc_qmi_handle_operation_mode_req

Description:
   This function is used by the QMI to set the operation mode

Parameters:
  op_mode: Operation Mode

Return value:
  None

=============================================================================*/
void gfc_qmi_handle_operation_mode_req(pdsm_pd_session_operation_e_type op_mode);

/*===========================================================================
gfc_qmi_handle_inject_time_zone

Description:
  This function is used by the QMI to set the operation mode

Parameters:   
  qq_time_utc_secs   : UTC Time in seconds
  qq_raw_offset_secs : Raw offset from UTC Time
  qq_dst_offset_secs : Daylight savings time offset


Return value: 
  None

=============================================================================*/
void gfc_qmi_handle_inject_time_zone
(
  uint64 qq_time_utc_secs,
  uint64 qq_raw_offset_secs,
  uint64 qq_dst_offset_secs
);

#endif /* GFC_QMI_API_H */
