/*===========================================================================

                  GM_MOTION Sensor Wifi Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  GM MOTION Sensor Wifi

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
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
11/29/14   ssu     GM LOWI Integration.
05/29/13   yg   Initial creation of file.
===========================================================================*/
#ifndef GM_MOTION_SENSOR_WIFI_H
#define GM_MOTION_SENSOR_WIFI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "gm_motion_sensor.h"

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_INIT

DESCRIPTION
  This function is called to initialize the GM Motion Sensor WiFi module

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void gm_motion_sensor_wifi_init(void);

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_START

DESCRIPTION
  This function is called to start distance accumulation by the WiFi distance
  accumulation module

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
boolean gm_motion_sensor_wifi_start(float distance_to_check);

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_STOP

DESCRIPTION
  This function is called to stop distance accumulation by the WiFi distance
  accumulation module

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
boolean gm_motion_sensor_wifi_stop(void);

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_FEED_WIFI_IND

DESCRIPTION
  This function is called to feed WiFi indications (Scans, CDH) to the WiFi
  distance accumulation module

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void gm_motion_sensor_wifi_feed_wifi_ind(gm_wifi_ind_info *wifi_ind_p);

/*===========================================================================

FUNCTION gm_motion_sensor_wifi_cdh_done

DESCRIPTION
  This function is called by the CDH algorithm when VMAX is the best speed
  prediction it can perform.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void gm_motion_sensor_wifi_cdh_done(float f_dist_orig, float f_dist_left);

/*===========================================================================

FUNCTION gm_motion_sensor_wifi_motion_detected

DESCRIPTION
  This function is called by the SCAN and CDH algorithms when the distance 
  requested for detection has been accumulated.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void gm_motion_sensor_wifi_motion_detected(void);

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_GET_ACCUM_DISTANCE

DESCRIPTION
  This function is called to det the distance accumulated by the WiFi module
  so far.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
float gm_motion_sensor_wifi_get_accum_distance(void);

/*===========================================================================


FUNCTION GM_MOTION_SENSOR_WIFI_SET_LOWI_AVAILABLE

DESCRIPTION
  This function is called to set the LOWI availability. It is expected that 
this will be called once at bootup, and a few times during one power cycle
of the phone.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
boolean gm_motion_sensor_wifi_set_lowi_available(boolean b_is_lowi_available);

/*===========================================================================
gm_motion_sensor_wifi_scan_handle_distance_timer_expiry

Description:

   This function handles motion sensor WIFI distance timer expiry

Parameters: 
  void
  
Return value: 
  void
=============================================================================*/
void gm_motion_sensor_wifi_scan_handle_distance_timer_expiry(void);

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_GET_IOD_FOOLED_STATE

DESCRIPTION
  This function is called to det if the WiFi context indcates IOD Fooled.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
boolean gm_motion_sensor_wifi_get_iod_fooled_state(void);

#ifdef __cplusplus
}
#endif


#endif /*GM_MOTION_SENSOR_WIFI_H*/

