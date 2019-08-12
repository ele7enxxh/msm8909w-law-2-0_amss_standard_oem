/*!
  @file
  ds_3gpp_redial_hdlr.h

  @brief
  Redial interface

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008-2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_pdn_redial_hdlr.h#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
06/21/11   rr      Support for pdn redial functionality
==============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_bearer_context.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_DUAL_IP_REDIAL_REQUIRED

DESCRIPTION 
  This function is used to determine whether to redialing based on dual IP cause
  codes. Cause codes are set in NV.

PARAMETERS
  down_reason   - Cause code(reason) returned by NW.
                  This is just the reason part of net_down_reason_type

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Redial required
  FALSE: Not a dual IP redial cause code
   
SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_dual_ip_redial_required
(
  ds_pdn_context_s                *pdn_context_p,
  uint16                           down_reason,
  sys_modem_as_id_e_type           subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_SETUP_REDIAL

DESCRIPTION
  This function checks with the 3gpp bearers if redialing is required.
  If so, sets up the pdn context for redial and sends out
  DS3GPP_RESEND_PDN_CONN_REQ_CMD.

PARAMETERS   
  bearer_cntx_p      - bearer context that was originally deactivted and
                       needs to be checked for redial
  down_reason        - reason for bearer deactivation
  call_was_connected - whether the call was connected or not
    
DEPENDENCIES
  Should always be called from within a critical section 
  ds3gpp_global_crit_sect

RETURN VALUE
  Returns
  DS_3GPP_REDIAL_PERFORMED if DS3GPP_RESEND_PDN_CONN_REQ_CMD is enqueued
  DS_3GPP_REDIAL_PENDING if redialing is to be performed but no explicit action
  is taken.
  DS_3GPP_REDIAL_NOT_PERFORMED if redialing is not required
 
SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_pdn_cntxt_redial_hdlr
(
  ds_pdn_context_s * pdn_cntx_p, 
  ds_bearer_context_s *bearer_cntx_p,
  ds_3gpp_down_reason_t down_reason,
  boolean call_was_connected
);

