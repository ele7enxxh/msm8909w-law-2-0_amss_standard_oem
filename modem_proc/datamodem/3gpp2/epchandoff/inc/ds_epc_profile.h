#ifndef DS_EPC_PROFILE_H
#define DS_EPC_PROFILE_H
/*===========================================================================

                      DS_EPC_PROFILE.H
                   
DESCRIPTION
  

EXTERNALIZED FUNCTIONS

 Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/epchandoff/inc/ds_epc_profile.h#1 $
  $Author: mplcsds1 $ 

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/03/11    fjia    Created Module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_EPC_HANDOFF
#include "ds_profile.h"
#include "ps_iface_defs.h"
#include "ds_epc_pdn_ctl.h"
#include "ps_acl.h"
#include "sys.h"

/*===========================================================================

                      FORWARD  DECLARATIONS FOR MODULE

===========================================================================*/
boolean ds_epc_alloc_and_fill_profile_apn_string
(
  ds_profile_tech_etype   profile1_tech,
  uint32                  profile1_id,
  ds_profile_info_type  * profile_apn_info,
  uint32                  subs_id
);

boolean ds_epc_get_profile_pdn_type
(
  ds_profile_tech_etype     profile_tech,
  uint32                    profile_id,
  ps_iface_bearer_ip_type * profile_pdn_bearer,
  uint32                    subs_id
);

boolean ds_epc_verify_pdn_type_compatibility
(
  ps_iface_bearer_ip_type    profile_pdp_type_value,
  ps_iface_addr_family_type  policy_pdp_type_val
);

boolean ds_epc_compare_profile_policy_pdn_type
(
  ds_profile_tech_etype     profile_tech,
  uint32                    profile_id,
  ps_iface_addr_family_type policy_pdp_type_val,
  uint32                    subs_id
);

ps_iface_bearer_ip_type ds_epc_convert_pdn_bearer_type
(
  ds_profile_tech_etype     profile_tech,
  uint8                     prf_bearer_type
);

boolean ds_epc_get_profile_param_info
(
  ds_profile_tech_etype       profile_tech,
  uint32                      profile_id,
  ds_profile_identifier_type  profile_param_id,
  ds_profile_info_type       *profile_param_info_ptr,
  uint32                      subs_id
);

boolean ds_epc_verify_apn_compatibility
(
  ds_profile_info_type   apn_name_1,
  ds_profile_info_type   apn_name_2
);

boolean ds_epc_compare_profile_apn_string
(
  ds_profile_tech_etype   profile1_tech,
  uint32                  profile1_id,
  ds_profile_tech_etype   profile2_tech,
  uint32                  profile2_id,
  uint32                  subs_id
);

void ds_epc_set_acl_info
(
  ps_iface_type             * this_iface_ptr,
  acl_policy_info_type      * policy_info_ptr
);

acl_policy_info_type    * ds_epc_get_acl_info
(
  ps_iface_type         * this_iface_ptr
);

/*void ds_epc_update_pdn_profile_info
(
  ds_epc_cb_type             * instance_ptr
);*/

void ds_epc_reset_pdn_profile_info
(
  ds_epc_cb_type            * instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_PROFILE_TO_POLICY_MATCH

DESCRIPTION   This function checks if the input 3gpp profile matches the
              policy.

DEPENDENCIES  None

RETURN VALUE
              -1: No match
               0: Match

SIDE EFFECTS  None
===========================================================================*/
int ds_epc_profile_to_policy_match
(
  uint16                 attach_profile,
  acl_policy_info_type * acl_policy_ptr
);
#endif /*FEATURE_EPC_HANDOFF*/
#endif /* DS_EPC_PROFILE.H */


