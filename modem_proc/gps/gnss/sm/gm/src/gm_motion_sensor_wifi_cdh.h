/*===========================================================================

                  GM_MOTION Sensor Wifi CDH Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  GM MOTION Sensor Wifi CDH

  Copyright (c) 2014-2015 Qualcomm Atheros, Inc.
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
11/29/14   ssu     GM LOWI Integration
06/11/14   ss   Initial creation of file.
===========================================================================*/
#ifndef GM_MOTION_SENSOR_WIFI_CDH_H
#define GM_MOTION_SENSOR_WIFI_CDH_H
#include "gm_motion_sensor_wifi.h"

#define MAX_WIFI_AP_RANGE_DIA_RELIABLE_HO         (300)       /* 300 meters  */
#define MAX_WIFI_AP_RANGE_DIA_UNRELIABLE_HO      (1000)  /* 1000 meters */

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_CDH_INIT

DESCRIPTION
  Initialize the CDH algorithm.
  
DEPENDENCIES

RETURN VALUE
  void
SIDE EFFECTS
  Begins maintenance of CDH related state
  
===========================================================================*/
void gm_motion_sensor_wifi_cdh_init(void);

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_IS_WIFI_CONNECTED

DESCRIPTION
  Query the CDH algorithm if the phone is connected to an AP. Only a non
  blacklisted AP is considered for this operation. I.e. if the phone is 
  connected to a blacklisted AP, the return value of this function will be
  false.
  
DEPENDENCIES

RETURN VALUE
  void

SIDE EFFECTS
  
===========================================================================*/

boolean gm_motion_sensor_is_wifi_connected(void);

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_CDH_START

DESCRIPTION
  Start distance accumulation using the CDH algorithm. If the state of the 
  CDH algorithm does not allow a good estimation of distance accumulation, 
  this may result asking the scan algorithm to do the distance accumulation 
  instead. 
  
DEPENDENCIES

RETURN VALUE
  TRUE: Distance collection started
  False: Otherwise

SIDE EFFECTS
  
===========================================================================*/
boolean gm_motion_sensor_wifi_cdh_start
(
  float f_distance_orig, 
  float distance_to_check
);

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_CDH_RESUME

DESCRIPTION
  Resume a distance accumulation using the CDH algorithm, that was stopped
  because of a disconnect. The resumption takes place because we are connected
  again.

DEPENDENCIES

RETURN VALUE
  TRUE: Distance collection started
  False: Otherwise

SIDE EFFECTS
  
===========================================================================*/
boolean gm_motion_sensor_wifi_cdh_resume
(
  float f_distance_orig,
  float f_distance_to_check,
  pdsm_wifi_attachment_status_ind_info *p_wifi_attach_status_ind
);

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_CDH_STOP

DESCRIPTION
  This function can be called by external modules to stop a distance 
  accumulation.
  
DEPENDENCIES

RETURN VALUE
  TRUE: Distance collection started
  False: Otherwise

SIDE EFFECTS
  
===========================================================================*/
boolean gm_motion_sensor_wifi_cdh_stop(void);

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_CDH_FEED_WIFI_IND

DESCRIPTION
  Give the CDH algorithm a WiFi indication (Connect, Disconnect, Handover) to 
  process further.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  
===========================================================================*/
void gm_motion_sensor_wifi_cdh_feed_wifi_ind
(
  pdsm_wifi_attachment_status_ind_info *p_wifi_attach_status_ind
);

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_CDH_FEED_IOD_IND

DESCRIPTION
  Give the CDH algorithm an IOD  indication to process further. We use IOD 
  Fooled to mark WiFi Contexts
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  
===========================================================================*/
void gm_motion_sensor_wifi_cdh_feed_iod_ind
(
  gm_iod_ind_info *p_iod_ind
);

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_CDH_FEED_CELL_SET_CHANGE

DESCRIPTION
  Let the CDH algorithm know that a WWAN cell set change has taken place
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  
===========================================================================*/
void gm_motion_sensor_wifi_cdh_feed_cell_set_change(void);

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_CDH_AP_SET_CHANGE

DESCRIPTION
  Let the CDH algorithm know that a WiFi AP set seen by the phone has changed
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  
===========================================================================*/
void gm_motion_sensor_wifi_cdh_ap_set_change(void);

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_IS_MAC_BL

DESCRIPTION
  Check if a particular AP is blacklisted.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  
===========================================================================*/
boolean gm_motion_sensor_wifi_is_mac_bl(uint8 mac[]);

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_CDH_GET_ACCUM_DISTANCE

DESCRIPTION
 Find the distance accumulated by the CDH algorithm
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  
===========================================================================*/
float gm_motion_sensor_wifi_cdh_get_accum_distance();

/*===========================================================================

FUNCTION gm_motion_sensor_wifi_cdh_feed_fix

DESCRIPTION
  Have the CDH algorithm process a  fix
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  
===========================================================================*/
void gm_motion_sensor_wifi_cdh_feed_fix
(
  sm_gm_cached_pos_fix_type *p_fix
);

/*===========================================================================

FUNCTION query_ap_from_history

DESCRIPTION
  Check history if this AP is present in history, and return the distance 
  associated.
  
DEPENDENCIES

RETURN VALUE
  Distance associated with AP if AP is present in history
  -1 otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
int32 query_ap_from_history
(
  uint8 mac[]
);

/*===========================================================================

FUNCTION gm_motion_sensor_wifi_cdh_check_connected_ssid_for_network

DESCRIPTION
  A new scan has been received. Check if the SSID we are connected to is a 
  network or not. Based on this, we will adjust the AP range.

  If CDH is connected to an AP, check if that AP is part of a multiple AP
  SSID and adjust the range of the WiFi AP accordingly.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  May change the range associated with a WiFi AP.
  
===========================================================================*/
void gm_motion_sensor_wifi_cdh_check_connected_ssid_for_network
(
  t_wiper_ap_set_struct_type *scan_ind
);

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_FILL_CDH_STATE

DESCRIPTION
  Update the passed structure with CDH algorithm state.  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/

void gm_motion_sensor_wifi_fill_cdh_state
(
  geofence_wifi_mot_detection_state *p_log_state
);

/*===========================================================================
FUNCTION gm_motion_sensor_wifi_cdh_handle_bl_timer_expiry

DESCRIPTION
  This function handles the BL update timer expiry. If we are connected, and
  if we have seen BL events (WWAN cell set changes, AP set changes), then 
  decrement the number of BL events seen.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  
===========================================================================*/
void gm_motion_sensor_wifi_cdh_handle_bl_timer_expiry(void);

/*===========================================================================

FUNCTION gm_motion_sensor_wifi_cdh_handle_vel_update_run_timer_expiry

DESCRIPTION
  Handle the VRUN timer expiry. This means that while we were 
  DISCONNECTED_STATIONARY earlier, we are moving at VRUN now. This will start
  distance accumulation at a rate of VRUN meters per second.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  Fires Distance events if Distance Accumulation is running.
===========================================================================*/
void gm_motion_sensor_wifi_cdh_handle_vel_update_run_timer_expiry(void);

/*===========================================================================

FUNCTION gm_motion_sensor_wifi_cdh_handle_vel_update_vmax_timer_expiry

DESCRIPTION
  This function handles the VMAX update timer expiry 
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  As a side effect, the distance timer is started with a new timeout.
  Change the global state from STATE_DISCONNECTED_VRUN to 
  STATE_DISCONNECTED_VMAX
  
===========================================================================*/
void gm_motion_sensor_wifi_cdh_handle_vel_update_vmax_timer_expiry(void);

/*===========================================================================

FUNCTION gm_motion_sensor_wifi_cdh_handle_dist_timer_expiry

DESCRIPTION
  If the distance timer expires, it means we have moved the distance required
  by the user.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  Fires Distance events if Distance Accumulation is running.
===========================================================================*/
void gm_motion_sensor_wifi_cdh_handle_dist_timer_expiry(void);

/*===========================================================================

FUNCTION gm_motion_sensor_wifi_cdh_disconnect_timer_expiry

DESCRIPTION
  This function handles the disconnect timer expiry. 
  
  If we are still disconnected, and if we have postponed firing a disconnect 
  event because the AP we disconnected from has frequent spurious disconnects, 
  we will fire the disconnect event now.

  If we are connected to the same AP we disconnected from, we will update the 
  disconnect count of the AP.

  If we are connected to a different AP we disconnected from, and we had postponed 
  firing a disconnect event, we will fire the disconnect event now.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  A disconnect event may be fired.
  The disconnect state of the CDH state machine is reset
===========================================================================*/
void gm_motion_sensor_wifi_cdh_disconnect_timer_expiry(void);

/*===========================================================================

FUNCTION gm_motion_sensor_wifi_cdh_update_disconnect_count_timer_expiry

DESCRIPTION
  This function updates the spurious disconnect count of WiFi APs.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void gm_motion_sensor_wifi_cdh_update_disconnect_count_timer_expiry(void);


/*===========================================================================
FUNCTION GM_MOTION_SENSOR_WIFI_CDH_SET_LOWI_AVAILABLE

DESCRIPTION
  This function is called to set the LOWI availability in CDH module. It is 
expected that this will be called once at bootup, and a few times during one 
power cycle of the phone.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
boolean gm_motion_sensor_wifi_cdh_set_lowi_available
(
  boolean b_is_lowi_available
);

/*===========================================================================

FUNCTION GM_MOTION_SENSOR_WIFI_CDH_GET_IOD_FOOLED_STATE

DESCRIPTION
  This function is called to det if the WiFi context indcates IOD Fooled.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
boolean gm_motion_sensor_wifi_cdh_get_iod_fooled_state(void);

#endif /*GM_MOTION_SENSOR_WIFI_CDH_H*/
