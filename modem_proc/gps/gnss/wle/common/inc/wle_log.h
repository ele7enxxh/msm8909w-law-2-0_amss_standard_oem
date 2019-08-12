#ifndef WLE_LOG_H
#define WLE_LOG_H
/*==============================================================================

                  GTP-WIFI-LP Location Engine Log

GENERAL DESCRIPTION
   This file contains constants, structures and functions prototypes necessary
   for logging and debugging WLE.  This includes F3 message definitions, logs,
   and events.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  
  Copyright (c) 2015 QUALCOMM Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

==============================================================================*/


/*==============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
Version Control

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/wle/common/inc/wle_log.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 

when          who     what, where, why
--------      ---     ----------------------------------------------------------
04/13/15      dc      Initial release
==============================================================================*/

/*==============================================================================

                          INCLUDE FILES FOR MODULE

==============================================================================*/

#include "err.h"
#include "msg.h"
#include "msg_mask.h"

#include "ULogFront.h"


/*==============================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/



/*==============================================================================

                                  CONSTANTS

==============================================================================*/

/*==============================================================================

                                    MACROS

==============================================================================*/

void wle_printf(  const char* p_file, uint32 q_line, const char *p_Format, uint32 q_Data1, 
                  uint32 q_Data2, uint32 q_Data3 );

#ifdef FEATURE_WLE_SMART

#define WLE_MSG_LOW(a,b,c,d)  wle_printf(__FILE__, __LINE__, a,b,c,d)
#define WLE_MSG_MED(a,b,c,d)  wle_printf(__FILE__, __LINE__, a,b,c,d)
#define WLE_MSG_HIGH(a,b,c,d) wle_printf(__FILE__, __LINE__, a,b,c,d)
#define WLE_MSG_ERR(a,b,c,d)  wle_printf(__FILE__, __LINE__, a,b,c,d)

#define WLM_MSG_ERR(a,b,c,d)   wle_printf(__FILE__,__LINE__,a,b,c,d)
#define WLM_MSG_FATAL(a,b,c,d) wle_printf(__FILE__,__LINE__,a,b,c,d)

#define WLM_MSG_LOW(a,b,c,d)     wle_printf(__FILE__,__LINE__,"=WLM TASK= " a,b,c,d)
#define WLM_MSG_MED(a,b,c,d)     wle_printf(__FILE__,__LINE__,"=WLM TASK= " a,b,c,d)
#define WLM_MSG_HIGH(a,b,c,d)    wle_printf(__FILE__,__LINE__,"=WLM TASK= " a,b,c,d)

#else


#define WLE_MSG_LOW(a,b,c,d)  MSG_3(MSG_SSID_WLE, MSG_LEGACY_LOW, a,b,c,d)
#define WLE_MSG_MED(a,b,c,d)  MSG_3(MSG_SSID_WLE, MSG_LEGACY_MED, a,b,c,d)
#define WLE_MSG_HIGH(a,b,c,d) MSG_3(MSG_SSID_WLE, MSG_LEGACY_HIGH, a,b,c,d)
#define WLE_MSG_ERR(a,b,c,d)  MSG_3(MSG_SSID_WLE, MSG_LEGACY_ERROR, a,b,c,d)
#define WLM_MSG_LOW(a,b,c,d)  MSG_3(MSG_SSID_WLM, MSG_LEGACY_LOW, a,b,c,d)
#define WLM_MSG_MED(a,b,c,d)  MSG_3(MSG_SSID_WLM, MSG_LEGACY_MED, a,b,c,d)
#define WLM_MSG_HIGH(a,b,c,d) MSG_3(MSG_SSID_WLM, MSG_LEGACY_HIGH, a,b,c,d)
#define WLM_MSG_ERR(a,b,c,d)  MSG_3(MSG_SSID_WLM, MSG_LEGACY_ERROR, a,b,c,d)

#endif /* FEATURE_WLE_SMART */

extern ULogHandle wlm_ulog_handle;

#define WLE_ULOG_MSG(msg) \
  ULOG_RT_PRINTF_0(wlm_ulog_handle, msg)

#define WLE_ULOG_MSG_SPRINTF_1(msg, a1) \
  ULOG_RT_PRINTF_1(wlm_ulog_handle, msg, a1)

#define WLE_ULOG_MSG_SPRINTF_2(msg, a1, a2) \
  ULOG_RT_PRINTF_2(wlm_ulog_handle, msg, a1, a2)

#define WLE_ULOG_MSG_SPRINTF_3(msg, a1, a2, a3) \
  ULOG_RT_PRINTF_3(wlm_ulog_handle, msg, a1, a2, a3)

#define WLE_ULOG_MSG_SPRINTF_4(msg, a1, a2, a3, a4) \
  ULOG_RT_PRINTF_4(wlm_ulog_handle, msg, a1, a2, a3, a4)

#define WLE_ULOG_MSG_SPRINTF_9(msg, a1, a2, a3, a4, a5, a6, a7, a8, a9) \
  ULOG_RT_PRINTF_9(wlm_ulog_handle, msg, a1, a2, a3, a4, a5, a6, a7, a8, a9)

/*==============================================================================

                          STRUCTURES AND ENUMERATIONS

==============================================================================*/

/*==============================================================================

                      FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/*=============================================================================

  FUNCTION      wle_PrintUint64MacMed()

  DESCRIPTION   This function outputs a MAC address message in real time

  DEPENDENCIES  None

  PARAMETERS
    t_macAddress - MAC of AP

  RETURN VALUE  

  SIDE EFFECTS  

=============================================================================*/
void wle_PrintUint64MacMed(uint64 t_macAddress);

/*=============================================================================

  FUNCTION      wle_PrintCacheEntryMed()

  DESCRIPTION   This function outputs a CACHE ENTRY message in real time

  DEPENDENCIES  None

  PARAMETERS
    macAddress - MAC of AP
  u_xLat       - x coordinate of AP, in degrees
  u_yLon       - y coordinate of AP, in degrees
  u_mar        - maximum antenna range of AP, in meters

  RETURN VALUE  

  SIDE EFFECTS  

=============================================================================*/
void wle_PrintCacheEntryMed(uint64 macAddress, float u_xLatDeg, float u_yLonDeg, float u_marM);

// This is so that wlm_lowi_wrapper.cpp may call these functions
#ifdef __cplusplus
extern "C"
{
#endif

/*=============================================================================

  FUNCTION      wle_PrintWifiMeasurementsMed()

  DESCRIPTION   This function outputs a wifi measurement message

  DEPENDENCIES  None

  PARAMETERS
    t_macAddress    - MAC of AP
    p_ssid          - pointer to SSID
    l_frequency     - Meas frequency
    l_numMeas       - Number of measurements
    l_rssi0pt5dBm   - rssi in 0.5dBm
    x_channel       - channel
    l_meas_age_mSec - measurement age, in msec

  RETURN VALUE  

  SIDE EFFECTS  

=============================================================================*/
void wle_PrintWifiMeasurementsMed(uint64 t_macAddress, char *p_ssid, int l_frequency, int l_numMeas,
                                  int32 l_rssi0pt5dBm, int16 x_channel, int32 l_meas_age_mSec);

#ifdef __cplusplus
}
#endif

/*=============================================================================

  FUNCTION      wle_PrintWifiStoredMeasurementsMed()

  DESCRIPTION   This function outputs a wifi stored measurement message
                It differs from previous function in that it does not include frequency.

  DEPENDENCIES  None

  PARAMETERS
    t_macAddress    - MAC of AP
    p_ssid          - pointer to SSID
    l_rssi0pt5dBm   - rssi in 0.5 dBm
    x_channel       - channel
    l_meas_age_mSec - measurement age, in msec

  RETURN VALUE  

  SIDE EFFECTS  

=============================================================================*/
void wle_PrintWifiStoredMeasurementsMed(uint64 t_macAddress, char *p_ssid,
                                        int32 l_rssi0pt5dBm, int16 x_channel, int32 l_meas_age_mSec);

#endif /* WLE_LOG_H */
