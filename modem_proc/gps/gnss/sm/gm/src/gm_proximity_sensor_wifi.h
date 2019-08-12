/*===========================================================================

                  GM_PROXIMITY_WIFI Sensor Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  GM Proximity Wifi Sensor

  Copyright (c) 2014 Qualcomm Atheros, Inc.
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
06/17/14   ss   Initial creation of file.
===========================================================================*/
#ifndef GM_PROXIMITY_WIFI_SENSOR_H
#define GM_PROXIMITY_WIFI_SENSOR_H

#include "gm_proximity_sensor.h"

/****************** MACROS ****************************************************/
/****************** Constants/Enums *******************************************/

/****************** Fuction Declaration****************************************/
/*===========================================================================

FUNCTION gm_proximity_sensor_wifi_init

DESCRIPTION
  This function enables the Wifi proximity Sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_proximity_sensor_wifi_init(void);


/*===========================================================================

FUNCTION gm_proximity_sensor_wifi_start_learning

DESCRIPTION
  This function starts Wifi assistance data learning
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_proximity_sensor_wifi_start_learning(void);

/*===========================================================================

FUNCTION gm_proximity_sensor_wifi_stop_learning

DESCRIPTION
  This function stops Wifi assistance data learning
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_proximity_sensor_wifi_stop_learning(void);

/*===========================================================================

FUNCTION gm_proximity_sensor_wifi_remove_gf_learning

DESCRIPTION
  This function removes learnt Wifi Ap info for this Geofence.
  
DEPENDENCIES

RETURN VALUE
  Void: 

SIDE EFFECTS

===========================================================================*/
void gm_proximity_sensor_wifi_remove_gf_learning(sm_gm_geofence_info_type *geofence_info_ptr);


/*===========================================================================

FUNCTION gm_proximity_sensor_wifi_remove_gf_learning

DESCRIPTION
  This function removes learnt Wifi Ap info for this Geofence.
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_proximity_sensor_wifi_remove_gf_learning(sm_gm_geofence_info_type *geofence_info_ptr);

/*===========================================================================

FUNCTION gm_proximity_sensor_wifi_get_stats

DESCRIPTION
  This function gets Wifi proximity stats
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
geofence_gm_proximity_stats_type* gm_proximity_sensor_wifi_get_stats(void);

#endif /*GM_PROXIMITY_WIFI_SENSOR_H*/

