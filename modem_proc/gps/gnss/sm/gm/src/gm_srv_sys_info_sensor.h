/*===========================================================================

                  GM SRV SYS INFO SENSOR Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  GM Serving System info sensor.

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
04/21/14   ssu   Initial creation of file.
===========================================================================*/
#ifndef GM_SRV_SYS_INFO_SENSOR_H
#define GM_SRV_SYS_INFO_SENSOR_H

#include "gm_ebee.h"

/*===========================================================================

FUNCTION gm_srv_sys_info_sensor_init

DESCRIPTION
  This function enables the sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_srv_sys_info_sensor_init(void);


/*===========================================================================

FUNCTION gm_srv_sys_info_sensor_start

DESCRIPTION
  This function starts the srv sys info sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_srv_sys_info_sensor_start(void *p_start_param);

/*===========================================================================

FUNCTION gm_srv_sys_info_sensor_stop

DESCRIPTION
  This function stops the srv sys info sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_srv_sys_info_sensor_stop(void *p_stop_param);


/*===========================================================================

FUNCTION gm_srv_sys_info_sensor_feed_cellinfo

DESCRIPTION
  This function feeds the srv sys information into the sensor.
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_srv_sys_info_sensor_feed_info(void *p_gm_area_id);


/*===========================================================================

FUNCTION gm_srv_sys_info_sensor_get_cellinfo

DESCRIPTION
  This function gets the srv sys information from the sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
void gm_srv_sys_info_sensor_get_info(void *p_gm_area_id);

#endif /*GM_SRV_SYS_INFO_SENSOR_H*/

