/*===========================================================================

                  GM_MOTION Sensor Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  GM MOTION Sensor

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
08/19/15    ss     Enable Motion Sensing using Sensors by default
03/31/14   ss    Supporting multiple clients for motion sensing
09/4/12   ss   Initial creation of file.
===========================================================================*/
#ifndef GM_MOTION_DETECTOR_H
#define GM_MOTION_DETECTOR_H

#include "gm_core.h"
#include "gm_ebee.h"
#include "gm_motion_sensor.h"


/****************** MACROS ****************************************************/

#define GM_MOTION_DETECTOR_TYPE_MIN 0
#define GM_MOTION_DETECTOR_TYPE_AMD GM_MOTION_DETECTOR_TYPE_MIN
#define GM_MOTION_DETECTOR_TYPE_RMD 1
#define GM_MOTION_DETECTOR_TYPE_PED 2
#define GM_MOTION_DETECTOR_TYPE_SMD 3
#define GM_MOTION_DETECTOR_TYPE_MAX 4


/****************** Constants/Enums *******************************************/

typedef uint8 gm_motion_detector_type;


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
boolean gm_motion_sensor_motion_detector_init(void);

/*===========================================================================

FUNCTION gm_motion_sensor_feed_motion_state

DESCRIPTION
  This function feeds the motion state to the sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_motion_detector_feed_motion_state(gm_motion_detector_type sensor_type, 
                                        gm_motion_state_e_type motion_state);


/*===========================================================================

FUNCTION gm_motion_sensor_motion_detector_feed_error

DESCRIPTION
  This function feeds the error to the sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_motion_detector_feed_error(gm_motion_detector_type sensor_type);


/*===========================================================================

FUNCTION gm_motion_sensor_motion_detector_start

DESCRIPTION
  This function starts the motion detector
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_motion_sensor_motion_detector_start(float distance_to_check);

/*===========================================================================

FUNCTION gm_motion_sensor_motion_detector_stop

DESCRIPTION
  This function starts the motion detector
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_motion_detector_stop(void);

/*===========================================================================
gm_motion_sensor_motion_detector_handle_backoff_timer_expiry

Description:

   This function handles motion sensor backoff timer expiry

Parameters: 
  void
  
Return value: 
  void
=============================================================================*/
void gm_motion_sensor_motion_detector_handle_backoff_timer_expiry(void);

/*===========================================================================
gm_motion_sensor_motion_detector_handle_cross_check_timer_expiry


Description:

   This function handles motion sensor Cross Check timer expiry

Parameters: 
  void
  
Return value: 
  void
=============================================================================*/
void gm_motion_sensor_motion_detector_handle_cross_check_timer_expiry(void);

/*===========================================================================
gm_motion_sensor_motion_detector_handle_grace_correction_timer_expiry



Description:

   This function handles motion sensor Cross Check timer expiry

Parameters: 
  void
  
Return value: 
  void
=============================================================================*/
void gm_motion_sensor_motion_detector_handle_grace_correction_timer_expiry(void);


/*===========================================================================

FUNCTION gm_motion_sensor_motion_detector_get_accum_distance

DESCRIPTION
  This function gets the accumulated distance so far
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
float gm_motion_sensor_motion_detector_get_accum_distance(void);

/*===========================================================================

FUNCTION gm_motion_sensor_motion_detector_get_stats

DESCRIPTION
  This function fills the collected stats

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gm_motion_sensor_motion_detector_get_stats(geofence_stats_rpts_type* const zp_gf_state_rpts);


#endif /*GM_MOTION_DETECTOR_H*/

