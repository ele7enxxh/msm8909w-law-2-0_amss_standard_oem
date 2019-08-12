/*===========================================================================

                  GM_PROXIMITY Sensor Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  GM Proximity Sensor

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
#ifndef GM_PROXIMITY_SENSOR_H
#define GM_PROXIMITY_SENSOR_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "gm_ebee.h"
#include "gm_log.h"

/****************** MACROS ****************************************************/
#define GM_PROXIMITY_SENSOR_MAX_LEARNING_SIZE  100

#define GM_PROXIMITY_SENSOR_TYPE_MIN 0
#define GM_PROXIMITY_SENSOR_TYPE_WIFI GM_PROXIMITY_SENSOR_TYPE_MIN /*Wifi proximity Sensor*/
#define GM_PROXIMITY_SENSOR_TYPE_BREACH 1
#define GM_PROXIMITY_SENSOR_TYPE_MAX 2

typedef uint32 gm_proximity_sensor_type;

#define GM_PROXIMITY_IND_CONF_HIGH   0 /*Low confidence Proximity*/
#define GM_PROXIMITY_IND_CONF_MED   1 /*Medium confidence Proximity*/
#define GM_PROXIMITY_IND_CONF_LOW  2 /*High confidence Proximity*/
#define GM_PROXIMITY_IND_CONF_MAX   2 /*Internal only*/



/****************** Constants/Enums *******************************************/


/****************** Structures *******************************************/
extern sm_gm_geofence_info_type* gm_proximity_learning_gf_list[GM_PROXIMITY_SENSOR_MAX_LEARNING_SIZE];

/****************** Fuction Declaration****************************************/
/*===========================================================================

FUNCTION gm_proximity_sensor_init

DESCRIPTION
  This function enables the proximity Sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_proximity_sensor_init(void);


/*===========================================================================

FUNCTION gm_proximity_sensor_handle_proximity_ind

DESCRIPTION
  This function handles proximity Indication from Proximity Sensors
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_proximity_sensor_handle_proximity_ind(gm_proximity_sensor_type proximity_sensor, 
                                              sm_gm_geofence_info_type *geofence_node, 
                                              sm_gm_proximity_ind_type proximity_ind_type);

/*===========================================================================

FUNCTION gm_proximity_sensor_feed_ind

DESCRIPTION
  This function feeds the wifi report to the proximity sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_proximity_sensor_feed_ind(gm_wifi_ind_info *wifi_ind_ptr);


#ifdef __cplusplus
}
#endif

#endif /*GM_PROXIMITY_SENSOR_H*/
