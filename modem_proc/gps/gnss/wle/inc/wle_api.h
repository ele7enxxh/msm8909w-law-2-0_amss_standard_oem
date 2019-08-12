#ifndef WLE_API_H
#define WLE_API_H
/*===========================================================================

                  WLE Header
GENERAL DESCRIPTION
   This file contains constants, structures and functions prototypes
   related to WLE (e.g GTP-Wifi-LP Location Manager).


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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/wle/inc/wle_api.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when        who     what, where, why
--------    ---   ----------------------------------------------------------
04/30/15    dc    Initial version

===========================================================================*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "gps_types.h"
#include "cgps_api.h"
#include "gnss_gdt.h" /* GDT_BIG_ENDIAN/GDT_LITTLE_ENDIAN */
#include "location_service_v02.h"
#include "aries_os_api.h"

/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/

/*=============================================================================

       Macros

=============================================================================*/

/*==============================================================================

       Variables

==============================================================================*/

/*=============================================================================

       Structures and enumerated types

=============================================================================*/

/*---------------------------*/
/* IPC Message Payload       */
/*---------------------------*/

/* QMI-IPC types */

typedef struct
{
  qmiLocInjectApCacheDataReqMsgT_v02 z_ApCacheReq;
  int             l_Client_handle;
  uint32          q_TransactionId;
} wle_InjectApCacheDataReqType;

typedef struct
{
  qmiLocInjectApDoNotCacheDataReqMsgT_v02 z_NoApCacheReq;
  int             l_Client_handle;
  uint32          q_TransactionId;
} wle_InjectNoApCacheDataReqType;
/*=============================================================================

       Functions

=============================================================================*/


/*=============================================================================

  FUNCTION      wle_isEnabled

  DESCRIPTION   This function returns if WLE is enabled or not
                
  DEPENDENCIES  None

  PARAMETERS
    void

  RETURN VALUE  None
 
  SIDE EFFECTS  

=============================================================================*/
boolean wle_isEnabled(void);

/*=============================================================================

  FUNCTION      wle_InjectApCacheData

  DESCRIPTION
    This API is called by loc_mw to inject ApCache data.
 
  PARAMETERS
    cpz_Data - Points to the received encoded ApCacheData
    (input)    in raw format 
 
  DEPENDENCIES  None

  PARAMETERS
     
  RETURN VALUE  
    TRUE  - If the IPC is sent to WLM
    FALSE - Otherwise 
 
  SIDE EFFECTS  

=============================================================================*/
boolean wle_InjectApCacheData(const wle_InjectApCacheDataReqType * cpz_Data);

/*=============================================================================

  FUNCTION      wle_InjectNoApCacheData

  DESCRIPTION
    This API is called by loc_mw to inject NoApCache data.
 
  PARAMETERS
    cpz_Data - Points to the received encoded NoApCacheData
    (input)    in raw format 
 
  DEPENDENCIES  None

  PARAMETERS
     
  RETURN VALUE  
    TRUE  - If the IPC is sent to WLM
    FALSE - Otherwise 
 
  SIDE EFFECTS  

=============================================================================*/
boolean wle_InjectNoApCacheData(const wle_InjectNoApCacheDataReqType * cpz_Data);

#endif /* WLE_API_H */
