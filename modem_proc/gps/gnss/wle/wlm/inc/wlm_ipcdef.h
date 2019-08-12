/*============================================================================
   FILE:        wlm_ipcdef.h

   OVERVIEW:    Contains the internal IPC definitions of the WLM task.  They are 
                only visible by other modules in TLM task and should never be 
                included in modules in other task.

 DEPENDENCIES:
                NOTE on sharing global variables in this file:
                To declare external variables in this file, #define EXTERN to
                  blank space (nothing) this is most likely done in nf_task.c
                To reference these  variables in this file, #define EXTERN to
                  extern this is most likely done in other files in NF task

  Copyright (c) 2015 QUALCOMM Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/

  /*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  Version Control

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/wle/wlm/inc/wlm_ipcdef.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 

  when       who what, where, why
  ---------- --- -----------------------------------------------------------
  2015-05-01 dc       Created file
============================================================================*/

#ifndef _WLM_IPCDEF_H
#define _WLM_IPCDEF_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "aries_os_globals.h"

/*---------------------------------------------------------------------------- 
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* IPC message ID defines */
#define M_C_WLE_MSG_FIRST              C_USR_MSG_ID_FIRST

/* WLM message types - enumerated IDs for messages placed into WLM's queue. */

/*---------------------------------------------------------------------------- 
 * Type Declarations
 * -------------------------------------------------------------------------*/
 
typedef enum
{
  /* IPC 0... C_USR_MSG_ID_FIRST-1 is reserved by OS */
  C_WLM_MSG_FIRST                                = M_C_WLE_MSG_FIRST,  /* First message to WLM */
  /* Messages from APSS */
  C_WLM_MSG_INJECT_APCACHE_DATA,
  C_WLM_MSG_INJECT_NOAPCACHE_DATA,
  /* Messages from LOWI-LP */
  C_WLM_MSG_INJECT_WIFI_SCAN_IND,
  C_WLM_MSG_INJECT_LOWI_SUBSCRIPTION_EVENT,
  /* Messages from GM */
  C_WLM_MSG_GM_START,
  C_WLM_MSG_GM_END = C_WLM_MSG_GM_START + 100,
  /* Add the new ID's here */
  C_WLM_MSG_END,
} wlm_IPCMsgEnumType;

#endif  /* #ifndef _WLM_IPCDEF_H */


/* End of wlm_ipcdef.h */
