#ifndef AON_INTERNAL_H
#define AON_INTERNAL_H

/*============================================================================
  @file aon_internal.h

  @brief
    This file contains utility and common definitions for aon
 
               Copyright (c) 2014 - 2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               QUALCOMM Proprietary and Confidential.
               Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
               Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/
/*============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/aon/src/aon_internal.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 



when        who  what, where, why
----------  ---  -----------------------------------------------------------
09/30/15    sj    Added hysterisis for unsolicited status indications 
04/06/15    sj    LB 2.0 integration 
03/18/15    sj    Fixed an yield issue with DBT through radius adjustment 
02/10/15    sj    Configured max geofences needed by AON from GM 
07/31/14    sj    Initial creation of file.


=============================================================================*/
/*****************************************************************************
 * Include Files
 * *************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
#include "msg.h"
#ifdef __cplusplus
}

#endif

#define AON_MSG_LOW( str, a, b, c )       MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_LOW, str, a, b, c )
#define AON_MSG_MED( str, a, b, c )       MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_MED, str, a, b, c )
#define AON_MSG_HIGH( str, a, b, c )      MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_HIGH, str, a, b, c )
#define AON_MSG_ERROR( str, a, b, c )     MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_ERROR, str, a, b, c )
#define AON_ERR_FATAL( str, a, b, c )     MSG_3(MSG_SSID_GNSS_LOCMW, MSG_LEGACY_FATAL, str, a, b, c )

#define AON_MIN_GF_RADIUS (1)  // 1 meter

#define AON_63_TO_99_FACTOR  (2.14)
#define AON_SCALE_HEPE_63_TO_99_CONF(UNC) ((uint32)((float)(UNC) * AON_63_TO_99_FACTOR))
#define AON_SESSION_STATUS_HYSTERISIS_INTERVAL_MSEC (10000)

typedef enum
{
  AON_GEOFENCE_EVENT_ENTERING_AREA = 0x1, /*Entering Area Event*/
  AON_GEOFENCE_EVENT_INSIDE_AREA =   0x2, /*Inside Area Event*/
  AON_GEOFENCE_EVENT_OUTSIDE_AREA =  0x4, /*Outside Area Event*/
  AON_GEOFENCE_EVENT_LEAVING_AREA =  0x8, /*Leaving Area Event*/
  AON_GEOFENCE_EVENT_MAX          =  0xf /*Internal user only*/
}aonGMBreachType;

#endif /*AON_INTERNAL_H*/
