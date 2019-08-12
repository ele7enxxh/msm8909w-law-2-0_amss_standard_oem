/*===========================================================================

                  GM_MOTION Sensor SMD Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  GM MOTION Sensor SMD

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
01/28/13   ss   Initial creation of file.
===========================================================================*/
#ifndef GM_MOTION_SENSOR_SMD_H
#define GM_MOTION_SENSOR_SMD_H

#include "customer.h"

#ifdef FEATURE_GEOFENCE_SENSOR_SUPPORT

#include "gm_motion_sensor_motion_detectors.h"


/*===========================================================================

FUNCTION gm_motion_sensor_smd_init

DESCRIPTION
  This function initialized SMD
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_smd_init(void);


/*===========================================================================

FUNCTION gm_motion_sensor_smd_start_for_motion_detection

DESCRIPTION
  This function handles SMD Start Request for Motion Check
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_motion_sensor_smd_start_for_motion_detection(uint32 distance_to_check);

/*===========================================================================

FUNCTION gm_motion_sensor_smd_stop_for_cross_check

DESCRIPTION
  This function handles SMD Stop Request for cross check
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_motion_sensor_smd_start_for_cross_check(void);


/*===========================================================================

FUNCTION gm_motion_sensor_smd_stop_for_motion_sensing

DESCRIPTION
  This function handles SMD Stop Request for motion sensing
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_motion_sensor_smd_stop_for_motion_sensing(void);


/*===========================================================================

FUNCTION gm_motion_sensor_smd_stop_for_cross_check

DESCRIPTION
  This function handles SMD Stop Request for cross check
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_motion_sensor_smd_stop_for_cross_check(void);

/*===========================================================================

FUNCTION gm_motion_sensor_smd_feed_motion_state

DESCRIPTION
  This function feeds the motion state to the sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_smd_feed_smd_ind(loc_geofence_smd_ind *p_gm_smd_ind);
/*===========================================================================

FUNCTION gm_motion_sensor_smd_get_motion_state

DESCRIPTION
  This function feeds the gets the motion state from the smd sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_smd_get_motion_state(void *p_gm_motion_smd_state);
/*===========================================================================
gm_motion_sensor_smd_handle_thresh_timer_expiry

Description:

   This function handles motion sensor smd thresh timer expiry

Parameters: 
  void
  
Return value: 
  void
=============================================================================*/
void gm_motion_sensor_smd_handle_keep_on_thresh_timer_expiry(void);
/*===========================================================================
gm_motion_sensor_smd_handle_on_wait_timer_expiry

Description:

   This function handles motion sensor smd on wait timer expiry

Parameters: 
  void
  
Return value: 
  void
=============================================================================*/
void gm_motion_sensor_smd_handle_on_wait_timer_expiry(void);


#endif /*FEATURE_GEOFENCE_SENSOR_SUPPORT*/

#endif /*GM_MOTION_SENSOR_SMD_H*/
