/*===========================================================================

                  GM_MOTION Sensor Wifi Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  GM MOTION Sensor Wifi

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
05/29/13   yg   Initial creation of file.
===========================================================================*/
#ifndef GM_MOTION_SENSOR_WIFI_SCAN_H
#define GM_MOTION_SENSOR_WIFI_SCAN_H

#include "gm_motion_sensor.h"


/*Enum of Motion states*/
typedef enum
{
  GM_MOTION_SENSOR_WIFI_SCAN_STATE_STOP, /*Motion Sensing Not Running*/
  GM_MOTION_SENSOR_WIFI_SCAN_STATE_STARTING, /*Motion Sensing Starting*/
  GM_MOTION_SENSOR_WIFI_SCAN_STATE_STARTED, /*Motion Sensing Started*/
} gm_motion_sensor_wifi_scan_state_e_type;

typedef enum
{
  AP_STATUS_IN_NETWORK_UNKNOWN,  /* We dont know if AP is present in a multiple
                                    AP network or a single AP by itself    */	
  AP_STATUS_IN_NETWORK_KNOWN,    /* AP is present in a multiple AP network */
  AP_STATUS_NOT_IN_NETWORK_KNOWN /* We know that the AP is the only AP with 
                                    its SSID */
} e_gm_motion_sensor_wifi_ap_in_network;

#define GM_MOTION_SENSOR_WIFI_SCAN_OVERLAP_HALF_WIFI_RANGE                  (50)
#define GM_MOTION_SENSOR_WIFI_SCAN_OVERLAP_WALK_THRES                       (25)
#define GM_MOTION_SENSOR_WIFI_SCAN_OVERLAP_DRIVE_THRES                      (10)

/*Worst case 1KM assumed*/
#define GM_MOTION_SENSOR_WIFI_SCAN_MAX_WIFI_RANGE                         (1000)
#define GM_MOTION_SENSOR_WIFI_SCAN_LAST_AP_REINSTATE_THRES                  (30)
#define GM_MOTION_SENSOR_WIFI_SCAN_MIN_AP_FOR_MOTION_DETECTION               (5)

/*===========================================================================
FUNCTION GM_MOTION_SENSOR_WIFI_SCAN_INIT

DESCRIPTION
  Initialize the SCAN Algorithm.
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  
===========================================================================*/
void gm_motion_sensor_wifi_scan_init(void);

/*===========================================================================
FUNCTION GM_MOTION_SENSOR_WIFI_SCAN_START

DESCRIPTION
  Start WiFi Scan distance accumulation.

DEPENDENCIES

RETURN VALUE
  boolean (True/False) depending on WiFi Scan distance accumulation starting
          successfuly or not.

SIDE EFFECTS
  
===========================================================================*/
boolean gm_motion_sensor_wifi_scan_start
(
  float f_distance_orig, 
  float f_distance_to_check
);

/*===========================================================================
FUNCTION GM_MOTION_SENSOR_WIFI_SCAN_STOP

DESCRIPTION
  Stop WiFi Scan distance accumulation.

DEPENDENCIES

RETURN VALUE
float : Return the distance left in the accumulation so far

SIDE EFFECTS
  
===========================================================================*/
float gm_motion_sensor_wifi_scan_stop(float *p_distance_orig);

/*===========================================================================
FUNCTION GM_MOTION_SENSOR_WIFI_SCAN_FEED_WIFI_IND

DESCRIPTION
  Feed WiFi indications (Scans) to WiFi SCAN algorithm.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  Will change the distance accumulated depending on the last scan.
  
===========================================================================*/
void gm_motion_sensor_wifi_scan_feed_wifi_ind
(
  t_wiper_ap_set_struct_type *p_scan_ind
);

float gm_motion_sensor_wifi_scan_get_accum_distance(void);

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_IS_AP_IN_NETWORK

DESCRIPTION
  Query the scan database if the SSID provided has more than one APs associated
  with it, i.e. if the SSID is that of a network of APs. If the scan algorithm
  can not accurately determine this information, the returned value shall be
  TRUE, i.e. the SSID shall be considered to be a network of APs.
  
DEPENDENCIES

RETURN VALUE
  boolean

SIDE EFFECTS
  
===========================================================================*/
e_gm_motion_sensor_wifi_ap_in_network gm_motion_sensor_wifi_is_ap_in_network
(
  char *p_ssid,
  t_wiper_ap_set_struct_type *scan_ind
);

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_FILL_SCAN_STATE

DESCRIPTION
  Update the passed structure with SCAN algorithm state.  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/

void gm_motion_sensor_wifi_fill_scan_state
(
  geofence_wifi_mot_detection_state       *p_state,
  geofence_wifi_mot_detection_scan_ap_set *p_ap_set
);

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
gm_motion_sensor_wifi_scan_handle_scan_collection_timer_expiry

Description:

   This function handles motion sensor WIFI Scan collection timer expiry

Parameters: 
  void
  
Return value: 
  void
=============================================================================*/
void gm_motion_sensor_wifi_scan_handle_scan_collection_timer_expiry(void);
#endif /*GM_MOTION_SENSOR_WIFI_SCAN_H*/
