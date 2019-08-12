/*===========================================================================

                  GM_MOTION Sensor AMD Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  GM MOTION Sensor AMD

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
#ifndef GM_MOTION_SENSOR_AMD_H
#define GM_MOTION_SENSOR_AMD_H

#include "customer.h"

#ifdef FEATURE_GEOFENCE_SENSOR_SUPPORT

#include "gm_motion_sensor_motion_detectors.h"


/*===========================================================================

FUNCTION gm_motion_sensor_amd_init

DESCRIPTION
  This function initialized AMD
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_amd_init(void);

/*===========================================================================

FUNCTION gm_motion_sensor_amd_start_for_motion_detection

DESCRIPTION
  This function handles AMD Stop Request for motion detection
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_motion_sensor_amd_start_for_motion_detection(uint32 distance_to_check);

/*===========================================================================

FUNCTION gm_motion_sensor_amd_stop_for_cross_check

DESCRIPTION
  This function handles AMD Stop Request for cross check
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_motion_sensor_amd_start_for_cross_check(void);

/*===========================================================================

FUNCTION gm_motion_sensor_amd_stop_for_motion_sensing

DESCRIPTION
  This function handles AMD Stop Request for motion sensing
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_motion_sensor_amd_stop_for_motion_sensing(void);

/*===========================================================================

FUNCTION gm_motion_sensor_amd_stop_for_cross_check

DESCRIPTION
  This function handles AMD Stop Request for cross check
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_motion_sensor_amd_stop_for_cross_check(void);

/*===========================================================================

FUNCTION gm_motion_sensor_amd_feed_motion_state

DESCRIPTION
  This function feeds the motion state to the sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_amd_feed_amd_ind(loc_geofence_amd_ind *p_gm_amd_ind);
/*===========================================================================

FUNCTION gm_motion_sensor_amd_get_motion_state

DESCRIPTION
  This function feeds the gets the motion state from the amd sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_amd_get_motion_state(void *p_gm_motion_amd_state);
/*===========================================================================
gm_motion_sensor_amd_handle_thresh_timer_expiry

Description:

   This function handles motion sensor amd thresh timer expiry

Parameters: 
  void
  
Return value: 
  void
=============================================================================*/
void gm_motion_sensor_amd_handle_keep_on_thresh_timer_expiry(void);
/*===========================================================================
gm_motion_sensor_amd_handle_on_wait_timer_expiry

Description:

   This function handles motion sensor amd on wait timer expiry

Parameters: 
  void
  
Return value: 
  void
=============================================================================*/
void gm_motion_sensor_amd_handle_on_wait_timer_expiry(void);



#endif /*FEATURE_GEOFENCE_SENSOR_SUPPORT*/

#endif /*GM_MOTION_SENSOR_AMD_H*/
