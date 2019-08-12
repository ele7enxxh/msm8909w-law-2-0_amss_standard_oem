#ifndef DS_IWLAN_S2B_PDN_SM_H
#define DS_IWLAN_S2B_PDN_SM_H
/*===========================================================================
                           DS_IWLAN_S2B_PDN_SM
===========================================================================*/
/*!
  @file
  ds_iwlan_s2b_pdn_sm.h

  @brief
  This module contains helper functions for the iwlan s2b PDN SM

  @detail
*/

/*===========================================================================

  Copyright (c) 2012-2013 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/wlan/iwlan_s2b/inc/ds_iwlan_s2b_pdn_sm.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/23/13    op     Support for requesting ePDG address via DNS query
10/13/12    smanu  Created Module
===========================================================================*/

/*===========================================================================
                      INCLUDE FILES FOR MODULE
===========================================================================*/

#include "customer.h"


#include "stm2.h"

typedef enum
{
  DS_IWLAN_S2B_PDN_SM_MIN_EV = 0,
  DS_IWLAN_S2B_PDN_SM_BRING_UP_EV = DS_IWLAN_S2B_PDN_SM_MIN_EV,
  DS_IWLAN_S2B_PDN_SM_WLAN_PROXY_IFACE_UP_EV,
  DS_IWLAN_S2B_PDN_SM_TUNNEL_UP_EV,
  DS_IWLAN_S2B_PDN_SM_SIO_CONFIG_COMPLETE_EV,
  DS_IWLAN_S2B_PDN_SM_RM_UP_EV,
  DS_IWLAN_S2B_PDN_SM_RM_DOWN_EV,
  DS_IWLAN_S2B_PDN_SM_TUNNEL_DOWN_EV,
  DS_IWLAN_S2B_PDN_SM_RESYNC_EV,
  DS_IWLAN_S2B_PDN_SM_TEARDOWN_EV,
  DS_IWLAN_S2B_PDN_SM_TUNNEL_PRE_CONFIG_COMPLETE_EV,
  DS_IWLAN_S2B_PDN_SM_WLAN_INFO_CHANGE_EV,
  DS_IWLAN_S2B_PDN_SM_MAX_EV = DS_IWLAN_S2B_PDN_SM_WLAN_INFO_CHANGE_EV
}ds_iwlan_s2b_pdn_sm_event_type; 

void ds_iwlan_s2b_pdn_sm_error_hook
(
  stm_status_t                error,
  const char*                 filename,
  uint32                      line,
  struct stm_state_machine_s* sm_ptr
);

/*===========================================================================
FUNCTION      ds_iwlan_s2b_pdn_sm_hdlr_allocate_sm()

DESCRIPTION   Initialize PDN SM machine.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
uint8 ds_iwlan_s2b_pdn_sm_hdlr_alloc_sm
(
  void* pdn_cb_ptr
);

/*===========================================================================
FUNCTION      ds_iwlan_s2b_pdn_sm_hdlr_dealloc_sm()

DESCRIPTION   Dealloc sm 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
int ds_iwlan_s2b_pdn_sm_hdlr_dealloc_sm
(
  uint32 instance 
);

/*===========================================================================
FUNCTION      ds_iwlan_s2b_pdn_sm_hdlr_post_ev()

DESCRIPTION   Send input to PDN State Machine.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_iwlan_s2b_pdn_sm_hdlr_post_ev
( 
  ds_iwlan_s2b_pdn_sm_event_type event,
  uint32                      sm_instance, 
  void*                       info 
);

/*===========================================================================
FUNCTION      ds_iwlan_s2b_pdn_sm_hdlr_get_user_data()

DESCRIPTION   Get user-defined data for EPC SYS State Machine.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void *ds_iwlan_s2b_pdn_sm_hdlr_get_user_data
(
  uint32 sm_instance
);

/*===========================================================================
FUNCTION      ds_iwlan_s2b_pdn_sm_hdlr_get_state()

DESCRIPTION   Get current state of EPC SYS SM

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

===========================================================================*/
stm_state_t ds_iwlan_s2b_pdn_sm_hdlr_get_state
(
  uint32 sm_instance
);


#endif /* DS_IWLAN_S2B_PDN_SM_H */

