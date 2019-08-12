#ifndef DS_3GPP_APN_SWITCH_MGR_H
#define DS_3GPP_APN_SWITCH_MGR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
                       D A T A   S E R V I C E S 
                        DS EPS APN SWITCH MGR HEADER FILE

GENERAL DESCRIPTION

  All the declarations and definitions necessary to support APN Switch MGR file

 Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_apn_switch_mgr.h#1 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/27/15    pvb     Created Module 

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "ds_dsd_apm_ext_i.h"
#include "ds_3gpp_pdn_context.h"
#include "queue.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
typedef struct
{
  boolean                          payload_valid;
  ds_dsd_apm_ind_type              apm_ind;
  ds_dsd_apm_ind_pay_load          apm_ind_payload;
}ds_3gpp_apn_switch_mgr_payload_info_type;


#ifdef FEATURE_DATA_LTE
/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION       DS_3GPP_APN_SWITCH_MGR_CHECK_IF_APN_SWITCH_NEEDED

DESCRIPTION    This function extracts APM rule block entry for a given 
               profile ID and compares the cause code with APN Switching
               cause codes to decide APN Switch is needed or not
 
PARAMETERS     subs_id: Currently active subscription for Packet services 
               profile_id: Profile of the PDN that is brought down
               cause_type: Detach/Reject Network cause
               call_info_ptr: call info passed by lower layers
               apn_switch_is_needed_p: Pointer to be filled with value that
                                       tells whether APN Switch is needed or 
                                       not
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_3gpp_apn_switch_mgr_check_if_apn_switch_needed
(
  cm_call_mode_info_u_type         *call_info_ptr,
  boolean                          *apn_switch_is_needed_p,
  sys_modem_as_id_e_type            subs_id,
  uint16                            profile_id,
  sys_sys_mode_e_type               call_mode,
  dsd_apm_rule_param_name_enum_type cause_type
);

/*===========================================================================
FUNCTION       DS_3GPP_APN_SWITCH_MGR_GET_DETACH_SWITCHING_ACTION_IF_ANY

DESCRIPTION    This function gets the flag which tells whether APN switch 
               is needed for the detach 
 
PARAMETERS     subs_id: Currently active subscription for Packet services 
 
DEPENDENCIES   NONE

RETURN VALUE   switching_action_payload: Switching action related info per subs

SIDE EFFECTS   NONE
===========================================================================*/
ds_3gpp_apn_switch_mgr_payload_info_type * 
                        ds_3gpp_apn_switch_mgr_get_switching_action_payload
(
  sys_modem_as_id_e_type             subs_id
);

/*===========================================================================
FUNCTION       DS_3GPP_APN_SWITCH_MGR_RESET_SWITCHING_ACTION_PAYLOAD

DESCRIPTION    This function resets the switching action payload per subs
 
PARAMETERS     subs_id: Currently active subscription for Packet services 
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_3gpp_apn_switch_mgr_reset_switching_action_payload
(
  sys_modem_as_id_e_type             subs_id
);

/*===========================================================================
FUNCTION       DS_3GPP_APN_SWITCH_MGR_SEND_IND_TO_APM

DESCRIPTION    This function retrieves the switching action payload per 
               subs and indicates it to APM 
 
PARAMETERS     subs_id: Currently active subscription for Packet services 
 
DEPENDENCIES   NONE

RETURN VALUE   ds_dsd_ext_error_code: Error code after posting ind to APM

SIDE EFFECTS   NONE
===========================================================================*/
ds_dsd_ext_error_code ds_3gpp_apn_switch_mgr_send_ind_to_apm
(
  sys_modem_as_id_e_type             subs_id
);

/*===========================================================================
FUNCTION       DS_3GPP_APN_SWITCH_MGR_POPULATE_SWITCHING_PAYLOAD_PER_RAT

DESCRIPTION    This function extracts APM rule block entry for a given 
               profile ID and compares the cause code with APN Switching
               cause codes to decide APN Switch is needed or not
 
PARAMETERS     bearer_call_mode: Call mode of the bearer 
               cause_type: DETACH/REJECT NETWORK cause type
               profile_id: Profile of the PDN detached or rejected
               PDN State: State of PDN Context
               call_info_ptr: Call info passed by lower layers
               subs_id: Current active packet service subscription
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_3gpp_apn_switch_mgr_populate_switching_payload_per_rat
(
  uint64                               bearer_call_mode,
  uint16                               cause_type,
  uint16                               profile_id,
  ds_pdn_context_state_e               pdn_state,
  cm_call_mode_info_u_type            *call_info_ptr,
  sys_modem_as_id_e_type               subs_id
);

#endif /* FEATURE_DATA_LTE */

#endif /* DS_3GPP_APN_SWITCH_MGR_H*/
