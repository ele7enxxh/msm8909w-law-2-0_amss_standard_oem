/*===========================================================================

                  GM_MOTION Sensor RMD Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  GM MOTION Sensor RMD

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
#ifndef GM_MOTION_SENSOR_RMD_H
#define GM_MOTION_SENSOR_RMD_H

#include "customer.h"

#ifdef FEATURE_GEOFENCE_SENSOR_SUPPORT

#include "gm_motion_sensor_motion_detectors.h"

/*===========================================================================

FUNCTION gm_motion_sensor_rmd_init

DESCRIPTION
  This function initialized RMD
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_rmd_init(void);
/*===========================================================================

FUNCTION gm_motion_sensor_rmd_start

DESCRIPTION
  This function handles RMD start Request
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_motion_sensor_rmd_start(uint32 distance_to_check);
/*===========================================================================

FUNCTION gm_motion_sensor_rmd_start_for_crosscheck

DESCRIPTION
  This function handles RMD Start for performing crosscheck
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_motion_sensor_rmd_start_for_crosscheck(uint32 cross_check_secs);
/*===========================================================================

FUNCTION gm_motion_sensor_rmd_stop

DESCRIPTION
  This function handles RMD Stop Request
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_motion_sensor_rmd_stop(void);
/*===========================================================================

FUNCTION gm_motion_sensor_rmd_feed_motion_state

DESCRIPTION
  This function feeds the motion state to the sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_rmd_feed_rmd_ind(loc_geofence_rmd_ind *p_gm_rmd_ind);
/*===========================================================================

FUNCTION gm_motion_sensor_rmd_get_motion_state

DESCRIPTION
  This function feeds the gets the motion state from the rmd sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_rmd_get_motion_state(void *p_gm_motion_rmd_state);
/*===========================================================================
gm_motion_sensor_rmd_handle_thresh_timer_expiry

Description:

   This function handles motion sensor rmd thresh timer expiry

Parameters: 
  void
  
Return value: 
  void
=============================================================================*/
void gm_motion_sensor_rmd_handle_keep_on_thresh_timer_expiry(void);
/*===========================================================================
gm_motion_sensor_rmd_handle_on_wait_timer_expiry

Description:

   This function handles motion sensor rmd on wait timer expiry

Parameters: 
  void
  
Return value: 
  void
=============================================================================*/
void gm_motion_sensor_rmd_handle_on_wait_timer_expiry(void);

#endif /*FEATURE_GEOFENCE_SENSOR_SUPPORT*/
#endif /*GM_MOTION_SENSOR_RMD_H*/
