/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  GM Util Header file

GENERAL DESCRIPTION
  This module makes up the Geofence Utility.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/gm/src/gm_util.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/16/15   ss   Initial creation of file.
======================================================================*/
#ifndef GM_UTIL_H
#define GM_UTIL_H

#include "gm_core.h"

boolean gm_util_getSunriseSunsetTimes
(
  double latitude_in_degrees,   // Latitude in degrees
  double longitude_in_degrees,  // Longitude in degrees
  uint32       utc_time_secs,   // UTC time in seconds  
  uint32      *sunrisetime_sec, // Sunrise time (in UTC seconds)
  uint32      *sunsettime_sec   // Sunset time (in utc seconds)
);


#endif /*GM_UTIL_H*/
