/*===========================================================================

                  GM_MOTION Sensor Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  GM MOTION Sensor

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/19/15    ss     Enable Motion Sensing using Sensors by default
03/02/15   ss     Supporting Low Power Wifi position in Geofencing
04/22/14   ssu  Registratiom mechanism for motion state events from motion sensors.
03/31/14   ss    Supporting multiple clients for motion sensing
09/4/12   ss   Initial creation of file.
===========================================================================*/
#ifndef GM_MOTION_SENSOR_H
#define GM_MOTION_SENSOR_H

#include "gm_ebee.h"
#include "gm_log.h"

/****************** MACROS ****************************************************/
typedef uint8 gm_motion_sensor_type;
#define GM_MOTION_SENSOR_TYPE_MIN 0
#define GM_MOTION_SENSOR_TYPE_MOTION_DETECTOR GM_MOTION_SENSOR_TYPE_MIN
#define GM_MOTION_SENSOR_TYPE_CMC 1
#define GM_MOTION_SENSOR_TYPE_WIFI 2
#define GM_MOTION_SENSOR_TYPE_WWAN 3
#define GM_MOTION_SENSOR_TYPE_MAX 4


/****************** MACROS ****************************************************/

typedef uint8  gm_motion_sensor_read_cmd_type;
#define GM_MOTION_SENSOR_READ_MOTION_SENSOR_STATE_ARRAY                     (1)
#define GM_MOTION_SENSOR_READ_IOD_FOOLED_STATE_CMD                          (2)

/*Motion Sensor Timer Id's*/
#define GM_EBEE_MOTION_SENSOR_MOTION_DETECTOR_BACKOFF_TIMER_ID          (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST)
#define GM_EBEE_MOTION_SENSOR_MOTION_DETECTOR_CROSS_CHECK_TIMER_ID      (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 1)
#define GM_EBEE_MOTION_SENSOR_MOTION_DETECTOR_GRACE_CORRECTION_TIMER_ID (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 2)
#define GM_EBEE_MOTION_SENSOR_AMD_KEEP_ON_THRESH_TIMER_ID               (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 3)
#define GM_EBEE_MOTION_SENSOR_AMD_ON_WAIT_TIMER_ID                      (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 4)
#define GM_EBEE_MOTION_SENSOR_RMD_KEEP_ON_THRESH_TIMER_ID               (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 5)
#define GM_EBEE_MOTION_SENSOR_RMD_ON_WAIT_TIMER_ID                      (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 6)
#define GM_EBEE_MOTION_SENSOR_PED_KEEP_ON_THRESH_TIMER_ID               (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 7)
#define GM_EBEE_MOTION_SENSOR_PED_ON_WAIT_TIMER_ID                      (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 8)
#define GM_EBEE_MOTION_SENSOR_CMC_DB_KEEP_ON_THRESH_TIMER_ID            (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 9)
#define GM_EBEE_MOTION_SENSOR_CMC_DB_ON_WAIT_TIMER_ID                   (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 10)
#define GM_EBEE_MOTION_SENSOR_WIFI_BACKOFF_TIMER_ID                     (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 11)
#define GM_EBEE_MOTION_SENSOR_SMD_KEEP_ON_THRESH_TIMER_ID               (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 12)
#define GM_EBEE_MOTION_SENSOR_SMD_ON_WAIT_TIMER_ID                      (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 13)
#define GM_EBEE_MOTION_SENSOR_CMC_DB_CROSS_CHECK_TIMER_ID               (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 14)
#define GM_EBEE_MOTION_SENSOR_WWAN_MEAS_REQ_TIMER_ID                    (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 15)
#define GM_EBEE_MOTION_SENSOR_WWAN_BACKOFF_TIMER_ID                     (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 16)
#define GM_EBEE_MOTION_SENSOR_WIFI_CDH_VRUN_TIMER_ID                    (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 17)
#define GM_EBEE_MOTION_SENSOR_WIFI_CDH_VMAX_TIMER_ID                    (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 18)
#define GM_EBEE_MOTION_SENSOR_WIFI_CDH_DIST_TIMER_ID                    (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 19)
#define GM_EBEE_MOTION_SENSOR_WIFI_CDH_BL_TIMER_ID                      (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 20)
#define GM_EBEE_MOTION_SENSOR_WIFI_SCAN_COLLECTION_TIMER_ID             (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 21)
#define GM_EBEE_MOTION_SENSOR_WIFI_WITHOUT_WWAN_WAIT_TIMER_ID           (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 22)
#define GM_EBEE_MOTION_SENSOR_WIFI_POSTPONE_DISCONNECT_TIMER_ID         (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 23)
#define GM_EBEE_MOTION_SENSOR_WIFI_UPDATE_DISCONNECT_CNT_TIMER_ID       (GM_EBEE_MOTION_SENSOR_TIMER_ID_FIRST + 24)
/***MAKE SURE THE ABOVE TIMERS ARE ALWAYS WITHIN GM_EBEE_MOTION_SENSOR_TIMER_ID_LAST*****/

/*Motion Scenario Speed Constants*/
#define GEOFENCE_MOTION_SENSOR_MAX_STATIONARY_SPEED 0 /*5 m\sec*/
#define GEOFENCE_MOTION_SENSOR_MAX_WALKING_SPEED 3 /*3 m\sec*/
#define GEOFENCE_MOTION_SENSOR_MAX_RUNNING_SPEED 8 /*8 m\sec*/
#define GEOFENCE_MOTION_SENSOR_MAX_DRIVING_SPEED  GM_MAX_DRIVING_SPEED
#define GEOFENCE_MOTION_SENSOR_MAX_UNKNOWN_SPEED  GEOFENCE_MOTION_SENSOR_MAX_DRIVING_SPEED

 
#define GEOFENCE_MOTION_SENSOR_MAX_SENSOR_USAGE_DIST 10000 /*10Km*/

#define GEOFENCE_MOTION_SENSOR_INVALID_DISTANCE 0xFFFFFFFF

/****************** Constants/Enums *******************************************/

/*Enum of Motion states*/
typedef enum
{
  GM_MOTION_STATE_UNKNOWN,
  GM_MOTION_STATE_STATIONARY,
  GM_MOTION_STATE_WALKING,
  GM_MOTION_STATE_RUNNING,
  GM_MOTION_STATE_DRIVING,
  /*Internal States*/
  GM_MOTION_STATE_MOVING,
  GM_MOTION_STATE_MOVED,
  GM_MOTION_STATE_MAX =  GM_MOTION_STATE_MOVED/*Internal Only*/
} gm_motion_state_e_type;


typedef enum
{  
  GM_MOTION_SENSOR_STATE_NOT_RUNNING,   /*Motion Sensor is not running*/  
  GM_MOTION_SENSOR_STATE_WAIT_TO_RUN,   /*Motion Sensor is waiting to run*/  
  GM_MOTION_SENSOR_STATE_RUNNING_ACTIVE, /*Motion Sensor is actively checking motion state*/
  GM_MOTION_SENSOR_STATE_RUNNING_NOT_ACTIVE, /*Motion Sensor is actively not actively checking motion state*/
} gm_motion_sensor_state_e_type;

typedef enum
{
  GM_MOTION_SENSOR_AMD_INDICATION,
  GM_MOTION_SENSOR_RMD_INDICATION,
  GM_MOTION_SENSOR_PED_INDICATION,
  GM_MOTION_SENSOR_CMC_INDICATION,
  GM_MOTION_SENSOR_WIFI_INDICATION,
  GM_MOTION_SENSOR_SMD_INDICATION,
  GM_MOTION_SENSOR_WCDMA_CELL_MEAS_INDICATION,
  GM_MOTION_SENSOR_LTE_CELL_MEAS_INDICATION,
  GM_MOTION_SENSOR_WWAN_SRV_SYS_UPDATE_INDICATION
} gm_motion_sensor_indication_e_type;


/*Clients of motion sensing*/
typedef enum
{
  GM_MOTION_SENSOR_CLIENT_EBEE, 
  GM_MOTION_SENSOR_CLIENT_WIFI_POS,
  GM_MOTION_SENSOR_CLIENT_MAX
}gm_motion_sensor_client;

/****************** Data Structures*********************************************/
typedef struct
{
  gm_motion_sensor_client e_client_id;
  float q_rem_dist;         // Updated distance for the client
  float q_dist_to_check; // Initial distance when the request was started
 }gm_motion_sensor_client_info_s_type;

typedef struct
{
  /* Command to Motion Sensor */
  gm_motion_sensor_read_cmd_type read_cmd;
  union
  {
    boolean                               b_is_iod_fooled;
    gm_motion_sensor_client_info_s_type   *p_motion_sensor_state;
  }u;
}gm_motion_sensor_read_cmd_s_type;

/****************** Data Structures*********************************************/
typedef struct
{
	float distance_to_check; /*Distance in meters to check*/
  gm_motion_sensor_client motion_sensor_client; /*Clinet who has requested for motion sensing*/
} gm_motion_sensor_start_prams_type;


typedef struct
{
  gm_motion_sensor_client motion_sensor_client; /*Clinet who has requested for motion sensing*/
} gm_motion_sensor_stop_prams_type;

typedef struct
{
  gm_motion_sensor_indication_e_type indication_type;
  union
  {
#ifdef FEATURE_GEOFENCE_SENSOR_SUPPORT
    loc_geofence_amd_ind *amd_ind_p;
    loc_geofence_rmd_ind *rmd_ind_p;
    loc_geofence_ped_ind *ped_ind_p;    
    loc_geofence_cmc_db_ind *cmc_ind_p;    
    loc_geofence_smd_ind *smd_ind_p;
#endif /*FEATURE_GEOFENCE_SENSOR_SUPPORT*/
    gm_wifi_ind_info *wifi_ind_p;
    lte_ml1_gps_mrl_cell_meas_update_t *lte_cell_meas_update_p;
    wl1_lsm_cell_meas_update_type *wcdma_cell_meas_update_p;
  }u;
} gm_motion_sensor_indication;

extern float gm_motion_state_speed[GM_MOTION_STATE_MAX + 1];

extern gm_motion_sensor_client_info_s_type q_gm_motion_sensing_request_queue[GM_MOTION_SENSOR_CLIENT_MAX];

/****************** Fuction Declaration****************************************/

/*===========================================================================

FUNCTION gm_motion_sensor_init

DESCRIPTION
  This function enables the sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_motion_sensor_init(void);


/*===========================================================================

FUNCTION gm_motion_sensor_get_motion_state

DESCRIPTION
  This function feeds the gets the motion state from the sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_get_motion_state(void *p_gm_motion_state);

/*===========================================================================

FUNCTION gm_motion_sensor_indicate_moved

DESCRIPTION
  This function indicates moved state to motion sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_indicate_moved(gm_motion_sensor_type sensor_type);

/*===========================================================================

FUNCTION gm_motion_sensor_indicate_error

DESCRIPTION
  This function indicates error to motion sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_indicate_error(gm_motion_sensor_type sensor_type);

/*===========================================================================

FUNCTION gm_motion_sensor_log_motion_start_event

DESCRIPTION
  This function logs Sensor Start event
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_log_motion_start_event(geofence_motion_detector_type detector_type,
                                             uint32 distance_to_check);
/*===========================================================================

FUNCTION gm_motion_sensor_log_motion_stop_event

DESCRIPTION
  This function logs Sensor Stop event
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_log_motion_stop_event(geofence_motion_detector_type detector_type);


/*===========================================================================

FUNCTION gm_motion_sensor_log_motion_dist_accum_event

DESCRIPTION
  This function logs distance accum event
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_log_motion_dist_accum_event(geofence_motion_detector_type detector_type,
                                                  uint32 dist_accumulated);

/*===========================================================================

FUNCTION gm_motion_sensor_log_motion_dist_met_event

DESCRIPTION
  This function logs distance met event
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_log_motion_dist_met_event(geofence_motion_detector_type detector_type);


/*===========================================================================

FUNCTION gm_motion_sensor_log_motion_error_event

DESCRIPTION
  This function logs Sensor Error event
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_log_motion_error_event(geofence_motion_detector_type detector_type);

#endif /*GM_MOTION_SENSOR_H*/
