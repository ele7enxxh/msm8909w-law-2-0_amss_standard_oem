/*!
  @file
  ds_3gpp_auth_mgr.h

  @brief
  Internal utility functions and routines

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2011-2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_auth_mgr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/11   rr      Auth manager, initial verstion 
 
===========================================================================*/

#ifndef DS_3GPP_AUTH_MGR_H
#define DS_3GPP_AUTH_MGR_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "sys.h"
#include "dsumtspdpreg.h"
#include "ds_3gpp_apn_table.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/



/*===========================================================================

                           FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_INIT

DESCRIPTION
  This function initializes the auth manager. It sets up the auth
  table (ds_3gpp_auth_fb_tbl).
 
PARAMETERS   :
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void 
ds_3gpp_auth_mgr_init( void );

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_NEED_REDIAL

DESCRIPTION
  Queris the auth manager if a redial is required.
 
PARAMETERS   :
  apn                  - Apn for which the redial query is made
  requdested_auth_type - The auth type that the app originally
                         requested
  cause_code           - PS IFACE cause code specifying the reason for
                         prior termination
  subs_id              - Subscription Id
DEPENDENCIES 
  None

RETURN VALUE
  TRUE - if redialing is required
  FALSE - Otherwise

SIDE EFFECTS
  
===========================================================================*/
boolean
ds_3gpp_auth_mgr_need_redial
(
  byte                          *apn,
  ds_umts_pdp_auth_enum_type    requested_auth_type,
  ps_iface_net_down_reason_type cause_code,
  boolean                       call_was_connected,
  sys_modem_as_id_e_type        subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_GET_AUTH_TYPE

DESCRIPTION
  Queries the auth manager for the appropriate auth_type to use for an
  outgoing call.
 
PARAMETERS   : 
  IN
  apn                  - Apn for which the redial query is made
  requdested_auth_type - The auth type that the app originally
                         requested
  subs_id              - Subscription Id
  OUT
  auth_type_to_use_p  - Pointer to the auth_type to be used

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/ 
void 
ds_3gpp_auth_mgr_get_auth_type 
(
  const byte                  *apn, 
  ds_umts_pdp_auth_enum_type  requested_auth_type, 
  ds_umts_pdp_auth_enum_type  *auth_type_to_use_p,
  sys_modem_as_id_e_type      subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_CALL_FAILURE

DESCRIPTION
  This function notifies the auth manager of a call failure event..
 
PARAMETERS   : 
  apn                  - Apn for which the redial query is made
  requdested_auth_type - The auth type that the app originally
                         requested
  subs_id              - Subscription Id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/ 
void 
ds_3gpp_auth_mgr_call_failure 
(
  byte                          *apn, 
  ds_umts_pdp_auth_enum_type    requested_auth_type, 
  ps_iface_net_down_reason_type cause_code,
  boolean                       call_was_connected,
  sys_modem_as_id_e_type        subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_CALL_SUCCESS

DESCRIPTION
  This function notifies the auth manager of a call success event.
 
PARAMETERS   : 
  apn                  - Apn for which the redial query is made
  requdested_auth_type - The auth type that the app originally
                         requested
  subs_id              - Subscription Id 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/ 
void 
ds_3gpp_auth_mgr_call_success 
(
  byte                        *apn, 
  ds_umts_pdp_auth_enum_type   requested_auth_type,
  sys_modem_as_id_e_type      subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_AUTH_MGR_PER_SUBS_INFO_INIT

DESCRIPTION
  This function initializes the subscription-specific variables in auth manager.
 
PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ds_3gpp_auth_mgr_per_subs_info_init
(
  sys_modem_as_id_e_type      subs_id
);
/*===========================================================================
FUNCTION ds_3gpp_auth_mgr_nv_refresh_per_subs_info

DESCRIPTION
  This function clears/resets the subscription-specific variables in auth manager.
 
PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void ds_3gpp_auth_mgr_nv_refresh_per_subs_info
(
  sys_modem_as_id_e_type  subs_id 
);

#endif /* DS_3GPP_AUTH_MGR_H */

