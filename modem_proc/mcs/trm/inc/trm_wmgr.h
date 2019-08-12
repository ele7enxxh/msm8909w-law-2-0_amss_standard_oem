#ifndef TRM_WMGR_H
#define TRM_WMGR_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

           T R A N S C E I V E R   R E S O U R C E   M A N A G E R

              Transceiver Resource Manager Wakeup Header File

GENERAL DESCRIPTION

  This file provides API for the clients to register for wakeup information
  with TRM. With the registration on every TRM call the wakeup information
  will be communicated to the client.


EXTERNALIZED FUNCTIONS

  trm_wmgr_reg_wakeup_cb() - Wakup Mgr registers for notification of future wakeups
  and chain owners from TRM 

REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  trm_init() must be called before any other function.

 
 
  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/trm/inc/trm_wmgr.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
06/24/2014   mn      Adding notion of winning client in WMGR (CR: 676454).
06/18/2014   mn      Changes for DR-DSDS (CR: 651800).
11/13/2013   mn      Opp. SGLTE/SGTDS/SVLTE +G DSDA support.
08/12/2013   mn      Fixing duplicate callbacks being sent by WMGR.
03/26/2013   mn      Adding support for Wakeup Manager on Triton DSDS.
08/09/2012   sr      Modified max rf chain and get device from chain api
02/10/2012   ag      Support for SVLTE+noSVDO mode.
12/22/2011   ag      Increase chains to 4.
05/23/2011   sg      Initial Revision

============================================================================*/

/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#include "trm.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*============================================================================

                                   MACROS

============================================================================*/

/* The number of RF chains present on this target excluding GPS */
#define TRM_MAX_RF_CHAINS TRM_MAX_CHAINS


/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

============================================================================*/

/*----------------------------------------------------------------------------
  TRM Wakeup Info Clients
  - This enum holds the list of all the clients that could be interested in
    the next wakeup information and the current chain owner information.
  - These clients can register with TRM for call backs and TRM will
     indicate the clients with the wakeup information
  - Add new members to the end of the list
----------------------------------------------------------------------------*/
typedef enum
{
  /* Wakeup Manager Client */
  TRM_WAKEUP_WMGR,

  /* Last client - Used for reference */
  TRM_WAKEUP_LAST

} trm_wakeup_client_enum_t;


/*----------------------------------------------------------------------------
  TRM Current Info Structure
----------------------------------------------------------------------------*/
typedef struct
{
  /* Client ID */
  trm_client_enum_t client;

} trm_cur_info_type;


/*----------------------------------------------------------------------------
  TRM Future Info Structure
----------------------------------------------------------------------------*/
typedef struct
{
  /* Client ID */
  trm_client_enum_t client;

  /* Category */
  int8              category;

  /* Time */
  int64             time;

  /* Duration */
  trm_duration_t    duration;

  int8              requested_category;

  trm_timestamp_t   reservation_time;

  trm_client_enum_t winning_client;

} trm_fut_info_type;


/*----------------------------------------------------------------------------
  TRM - Wakeup Manager interaction structure
----------------------------------------------------------------------------*/
typedef struct
{
  /* Client Id of the client making the 
     current API call */
  trm_client_enum_t   curr_api_caller;

  /* Current info count */
  uint8               cur_count;

  /* Future info count */
  uint8               fut_count;

  /* Pointer to array of current info */
  trm_cur_info_type   cur_info[TRM_MAX_RF_CHAINS];

  /* Pointer to array of future info */
  trm_fut_info_type   fut_info[TRM_MAX_CLIENTS];

} trm_wakeup_info_type;


/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/


/*============================================================================

FUNCTION TRM_WMGR_CB_TYPE

DESCRIPTION
  Prototype of Wmgr call back function.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.  

============================================================================*/
typedef void (*trm_wmgr_cb_type)
(
  /* The client for the call back function */
  trm_wakeup_client_enum_t client,
    
  /* TRM information required for wakeup manager */
  trm_wakeup_info_type *wakeup_info
);



/*============================================================================

FUNCTION TRM_WMGR_REG_WAKEUP_CB

DESCRIPTION
  Wakup Mgr registers for notification of future wakeups and chain owners
  from TRM.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If the registration with TRM was successful 
  FALSE - If the registration failed

SIDE EFFECTS
  The memory for the result is allocated by the caller and the pointer is
  passed to TRM. TRM does not free to allocate any new memory for the
  result. The onus is on the caller to allocate enough space to hold
  the result.

============================================================================*/
boolean trm_wmgr_reg_wakeup_cb
(
  /* ID of client registering for call back */
  trm_wakeup_client_enum_t client,

  /* Call back function */
  trm_wmgr_cb_type cb,
  
  /* Pointer to hold the result, memory should be allocated by the caller */
  trm_wakeup_info_type *wakeup_info  
);


/*============================================================================

FUNCTION TRM_WMGR_INIT

DESCRIPTION
  Initialize the structure that holds wakeup mgr - TRM information
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void trm_wmgr_init(void);

/*============================================================================

FUNCTION TRM_GET_DEVICE_FOR_CHAIN

DESCRIPTION
  Returns the rf device corresponding to a trm chain number.
  
DEPENDENCIES
  None

RETURN VALUE
  rfm_device_enum_type

SIDE EFFECTS
  None

============================================================================*/
extern rfm_device_enum_type trm_get_device_for_chain
(
  uint32  chain   /* Chain number for which RF device is required */
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TRM_WMGR_H */

