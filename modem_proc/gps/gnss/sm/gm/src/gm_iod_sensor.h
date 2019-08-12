/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         GM_IOD_SENSOR

GENERAL DESCRIPTION
  This module makes up the Geofence IOD Sensor

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

Copyright (c) 2015 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/gm/src/gm_iod_sensor.h#1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
======================================================================*/

#include "customer.h"
#include "gm_api.h"

typedef enum
{
  IOD_COMMAND_START_AUTOCAL,
  IOD_COMMAND_START_IOD
} iod_command_e_type;

typedef struct
{
  iod_command_e_type   e_iod_cmd;
  sm_gm_time_zone_info z_tz_info;
  double d_latitude;
  double d_longitude;
} iod_start_param;

//#define ALS_IOD_UNIT_TEST
#define GM_EBEE_IOD_SENSOR_RESP_TIMER_ID   (GM_EBEE_IOD_SENSOR_TIMER_ID_FIRST)
#define GM_EBEE_IOD_SENSOR_IND_TIMER_ID    (GM_EBEE_IOD_SENSOR_TIMER_ID_FIRST+1)
#ifdef ALS_IOD_UNIT_TEST
#define GM_EBEE_IOD_SENSOR_UT_TIMER_ID     (GM_EBEE_IOD_SENSOR_TIMER_ID_FIRST+2)
#endif

/*===========================================================================

FUNCTION gm_iod_sensor_init

DESCRIPTION
  This function enables the IOD sensor
  
DEPENDENCIES

RETURN VALUE
  TRUE: 

SIDE EFFECTS

===========================================================================*/
boolean gm_iod_sensor_init(void);
