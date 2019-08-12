#ifndef WLM_API_H
#define WLM_API_H
/*===========================================================================

                  GTP-WIFI-LP Location Manager Header

GENERAL DESCRIPTION
   This file contains constants, structures and functions prototypes
   related to GTP-WIFI-LP WLM functionality.


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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/wle/wlm/inc/wlm_api.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 

when        who     what, where, why
--------    ---   ----------------------------------------------------------
04/30/2015  dc      Initial Checkin

===========================================================================*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/

#include "wle_api.h"
#include "wlm_ipcdef.h"
#include "wle_common.h"
#include "wle_log.h"


/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/


/*=============================================================================

       Macros

=============================================================================*/


/*=============================================================================

       Structures and enumerated types

=============================================================================*/


/* WLM Timers */
typedef enum
{
  WLM_FIRST_TIMER_ID = 0x0001,
  WLM_LOWI_REG_SHORT_TERM_TIMER_ID = WLM_FIRST_TIMER_ID,
  WLM_LOWI_REG_LONG_TERM_TIMER_ID,
  WLM_LAST_TIMER_ID = WLM_LOWI_REG_LONG_TERM_TIMER_ID,
  WLM_MAX_TIMER_ID = 0xFFFF
} wlm_timer_id_type;

/* Enum to identify the different events from LOWI service into WLM */
typedef enum
{
  WLM_LOWI_LP_NONE,
  WLM_LOWI_LP_SERVICE_ERROR,
  WLM_LOWI_LP_PERMANENT_ERROR,
  WLM_LOWI_EVENT_MAX,
} wlm_lowi_event_e_type;


/* Structure for receiving the response events from LOWI into WLM  */
typedef struct
{
  /* Type of the event */
  wlm_lowi_event_e_type e_lowi_event_type;
} wlm_lowi_response_event_s_type;


/*=============================================================================

       Functions

=============================================================================*/

/*===========================================================================
FUNCTION wlm_InjectApCacheData

DESCRIPTION
  This function is used by WLE to inject ApCache Data into WLM.
 
PARAMETERS 
  pz_Data - Points to the ApCache data which is received from
  (input)    APSS in raw format
 
DEPENDENCIES

RETURN VALUE 
  TRUE:  The IPC is sent to WLM
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
boolean wlm_InjectApCacheData(const wle_InjectApCacheDataReqType * pz_Data);

/*===========================================================================
FUNCTION wlm_InjectNoApCacheData

DESCRIPTION
  This function is used by WLE to inject NoApCache Data into WLM.
 
PARAMETERS 
  pz_Data - Points to the NoApCache data which is received from
  (input)    APSS in raw format
 
DEPENDENCIES

RETURN VALUE 
  TRUE:  The IPC is sent to WLM
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
boolean wlm_InjectNoApCacheData(const wle_InjectNoApCacheDataReqType * pz_Data);

// This is so that wlm_lowi_wrapper.cpp may call these functions
#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================
wlm_inject_wifi_scan_indication

Description:

   This function injects Wifi scan indication

Parameters: 
  wifi_scan_indication: Pointer to Wifi Scan indication structure
  
Return value: 
  void
=============================================================================*/
void wlm_inject_wifi_scan_indication(wle_ap_set_struct_type *wifi_scan_indication);

/********************************************************************
*
* wlm_inject_lowi_subscription_events
*
* Function description:
* Used to inject subscription events from LOWI into WLM
*
* Parameters: 
*  p_lowi_event_info : The different events from LOWI 
*
* Return:
* void
*  
*********************************************************************
*/  
void wlm_inject_lowi_subscription_events
                                 (wlm_lowi_response_event_s_type *p_lowi_event_info);

#ifdef __cplusplus
}
#endif

#endif /* WLM_API_H */

