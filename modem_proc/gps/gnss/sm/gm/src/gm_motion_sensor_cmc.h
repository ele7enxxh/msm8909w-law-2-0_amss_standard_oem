/*===========================================================================

                  GM_MOTION Sensor CMC Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  GM MOTION Sensor CMC

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

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
03/31/14   ss    Supporting floats for motion speed and configuration of motion state speed
01/02/12   ss   Initial creation of file.
===========================================================================*/
#ifndef GM_MOTION_SENSOR_CMC_H
#define GM_MOTION_SENSOR_CMC_H

#include "customer.h"

#ifdef FEATURE_GEOFENCE_SENSOR_SUPPORT

#include "gm_motion_sensor.h"
#include "gm_log.h"


/*Enum of Motion states*/
typedef enum
{
  GM_MOTION_SENSOR_CMC_STATE_STOP, /*Motion Sensing Not Running*/
  GM_MOTION_SENSOR_CMC_STATE_STARTING, /*Motion Sensing Starting*/
  GM_MOTION_SENSOR_CMC_STATE_STARTED, /*Motion Sensing Started*/
  GM_MOTION_SENSOR_CMC_STATE_UNKNOWN, /*Unknown motion state*/  
  GM_MOTION_SENSOR_CMC_STATE_STATIONARY, /*Stationary motion state*/
  GM_MOTION_SENSOR_CMC_STATE_WALKING, /*Walking motion state*/
  GM_MOTION_SENSOR_CMC_STATE_RUNNING, /*Running motion state*/
  GM_MOTION_SENSOR_CMC_STATE_DRIVING, /*Driving motion state*/
  GM_MOTION_SENSOR_CMC_STATE_MOVED 	   /*Handset has moved the distance*/
} gm_motion_sensor_cmc_state_e_type;

/*===========================================================================

FUNCTION gm_motion_sensor_cmc_db_init

DESCRIPTION
  This function initialized CMC
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_cmc_db_init(void);
/*===========================================================================

FUNCTION gm_motion_sensor_cmc_db_start

DESCRIPTION
  This function handles CMC start Request
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_motion_sensor_cmc_db_start(float distance_to_check);
/*===========================================================================

FUNCTION gm_motion_sensor_cmc_db_stop

DESCRIPTION
  This function handles CMC Stop Request
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_motion_sensor_cmc_db_stop(void);
/*===========================================================================

FUNCTION gm_motion_sensor_cmc_db_feed_motion_state

DESCRIPTION
  This function feeds the motion state to the sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_cmc_db_feed_cmc_db_ind(loc_geofence_cmc_db_ind *p_gm_cmc_db_ind);

/*===========================================================================
gm_motion_sensor_cmc_db_handle_thresh_timer_expiry

Description:

   This function handles motion sensor cmc_db thresh timer expiry

Parameters: 
  void
  
Return value: 
  void
=============================================================================*/
void gm_motion_sensor_cmc_db_handle_keep_on_thresh_timer_expiry(void);

/*===========================================================================
gm_motion_sensor_cmc_handle_on_wait_timer_expiry

Description:

   This function handles motion sensor cmc on wait timer expiry

Parameters: 
  void
  
Return value: 
  void
=============================================================================*/
void gm_motion_sensor_cmc_db_handle_on_wait_timer_expiry(void);

/*===========================================================================
gm_motion_sensor_cmc_db_handle_cross_check_timer_expiry

Description:

   This function handles motion sensor cmc cross check expiry

Parameters: 
  void
  
Return value: 
  void
=============================================================================*/
void gm_motion_sensor_cmc_db_handle_cross_check_timer_expiry(void);

/*===========================================================================

FUNCTION gm_motion_sensor_cmc_db_start_cross_check

DESCRIPTION
  This function handles CMC CrossCheck
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_motion_sensor_cmc_db_start_cross_check(void);

/*===========================================================================

FUNCTION gm_motion_sensor_cmc_db_stop_cross_check

DESCRIPTION
  This function handles CMC CrossCheck Stop
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_motion_sensor_cmc_db_stop_cross_check(void);

/*===========================================================================

FUNCTION gm_motion_sensor_cmc_db_reset_cross_check

DESCRIPTION
  This function handles resetting  states for start of cross check period
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_cmc_db_reset_cross_check(void);

/*===========================================================================

FUNCTION gm_motion_sensor_cmc_db_feed_motion_state_for_cross_check

DESCRIPTION
  This function handles motion states for cross check
  
DEPENDENCIES

RETURN VALUE
  void: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_cmc_db_feed_motion_state_for_cross_check(gm_motion_state_e_type motion_state);

/*===========================================================================

FUNCTION gm_motion_sensor_cmc_db_get_accum_distance

DESCRIPTION
  This function gets accumulated distance
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
float gm_motion_sensor_cmc_db_get_accum_distance(void);

/*===========================================================================

FUNCTION gm_motion_sensor_cmc_db_get_stats

DESCRIPTION
  This function fills the collected stats

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gm_motion_sensor_cmc_db_get_stats(geofence_stats_rpts_type* const zp_gf_state_rpts);


#endif /*FEATURE_GEOFENCE_SENSOR_SUPPORT*/
#endif /*GM_MOTION_SENSOR_CMC_H*/
