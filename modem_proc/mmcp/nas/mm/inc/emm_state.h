
/*!
  @file
  emm_state.h

  @brief
  EMM state machine definitions.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/inc/emm_state.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/09/10   zren    Added combined procedure feature
10/06/09   vdr     Compiler warnings fixed
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
05/29/09   zren    Added define EMM_UNKNOWN_SUBSTATE
===========================================================================*/
#ifndef _EMM_STATE_H_
#define _EMM_STATE_H_
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"

/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/

typedef enum
{
  EMM_NULL,
  EMM_DEREGISTERED,
  EMM_REGISTERED_INITIATED,
  EMM_REGISTERED,
  EMM_TRACKING_AREA_UPDATING_INITIATED,
  EMM_SERVICE_REQUEST_INITIATED,
  EMM_DEREGISTERED_INITIATED,
  EMM_INVALID_STATE // FOR INTRNAL USE ONLY
} emm_state_type; 

typedef enum
{
  EMM_DEREGISTERED_NO_IMSI,
  EMM_DEREGISTERED_PLMN_SEARCH,
  EMM_DEREGISTERED_ATTACH_NEEDED,
  EMM_DEREGISTERED_NO_CELL_AVAILABLE,
  EMM_DEREGISTERED_ATTEMPTING_TO_ATTACH,
  EMM_DEREGISTERED_NORMAL_SERVICE,
  EMM_DEREGISTERED_LIMITED_SERVICE,
  EMM_DEGEGISTERED_WAITING_PDN_CONN_REQ
} emm_deregistered_substate_type; 

typedef enum
{
  EMM_WAITING_FOR_NW_RESPONSE, 
  EMM_WAITING_FOR_ESM_RESPONSE
} emm_registered_initiated_substate_type ;

typedef enum
{
  EMM_REGISTERED_NORMAL_SERVICE,
  EMM_REGISTERED_UPDATE_NEEDED,
  EMM_REGISTERED_ATTEMPTING_TO_UPDATE,
  EMM_REGISTERED_NO_CELL_AVAILABLE,
  EMM_REGISTERED_PLMN_SEARCH,
  EMM_REGISTERED_LIMITED_SERVICE,
  EMM_REGISTERED_ATTEMPTING_TO_UPDATE_MM,
  EMM_REGISTERED_IMSI_DETACH_INITIATED,
  EMM_REGISTERED_WAITING_FOR_ESM_ISR_STATUS
} emm_registered_substate_type; 

typedef dword emm_substate_type ;

#define EMM_UNKNOWN_STATE 0xFFFF
#define EMM_UNKNOWN_SUBSTATE (emm_substate_type)0xFFFF


#endif /* _EMM_STATE_H_ */
#endif /*FEATURE_LTE*/

