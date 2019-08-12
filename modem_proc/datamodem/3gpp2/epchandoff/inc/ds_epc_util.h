#ifndef DS_EPC_UTIL_H
#define DS_EPC_UTIL_H
/*===========================================================================

                      DS_EPC_PDN_SM_CTL.H
                   
DESCRIPTION
  

EXTERNALIZED FUNCTIONS

 Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/epchandoff/inc/ds_epc_util.h#1 $
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
#include "ds_epc_pdn_ctl.h"
#include "sys.h"
#include "ps_acl.h"
#include "ps_iface_defs.h"

/*===========================================================================
FUNCTION      DS_EPC_IS_ATTACH_PDN

DESCRIPTION: Check if this is attach PDN on eHRPD. It used when receiving LTE_ATTACHING_EVENT
             from SYS SM.

PARAMETERS: attach_pf_id - LTE attach PDN profile id 
   
RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_util_is_attach_pdn
(
  ds_epc_cb_type          * epc_instance_ptr,
  uint16                  attach_pf_id
);

/*===========================================================================
FUNCTION      DS_EPC_ALLOW_3GPP_CALL

DESCRIPTION 
  External function. Query EPC for if a call can be originated. 
 
  The purpose of this function is to make sure that in e->L or iwlan->L,
  before all handoff are initiated, if there is a handoff PDN that is the
  same as the one for the call, the handoff PDN should be served.
  To do this, the call is not allowed, and app needs to retry later.
 
  Note that ds_epc_wait_for_handoff flag is only set to TRUE when there is
  something to handoff in emm attach handler.
 
DEPENDENCIES  None.

RETURN VALUE 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_epc_allow_3gpp_call
(
  uint16                           pf_id,
  ps_sys_subscription_enum_type    subs_id
);

/*===========================================================================
FUNCTION      DS_EPC_ALLOW_3GPP2_CALL

DESCRIPTION 
  External function. Query EPC for if a call can be originated on 3GPP2. 
 
  The purpose of this function is to check that if there is a 
  handoff PDN that is the same as the one for the call, the 
  handoff PDN should be served. If it is a new 3GPP2 call happening
  during the handoff, the call is not allowed.
 
DEPENDENCIES  None.

RETURN VALUE  TRUE: call is allowed on 3GPP2
              FALSE: call is not allowed on 3GPP2

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_epc_allow_3gpp2_call
(
  uint32                          pf_id,
  ps_sys_subscription_enum_type   subs_id
);

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION      DS_EPC_ALLOW_IWLAN_CALL

DESCRIPTION 
  External function. Query EPC for if a call can be originated. 
 
  The purpose of this function is to make sure that in e->iwlan or L->iwlan,
  before all handoff are initiated, if there is a handoff PDN that is the
  same as the one for the call, the handoff PDN should be served.
  To do this, the call is not allowed, and app needs to retry later.
 
  If the call is allowed, EPC sets if this call is initial or handover,
  and provide handoff_ctxt in caller's buffer.
 
DEPENDENCIES  None. 
 
PARATMETERS: pf_id - profile id 
             subs_id - subscription id
             is_v6_call - if this is call is v6 call
             is_ho - indicate is handover type to caller
             handoff_ctxt - set handoff context in caller's buffer for handover type

RETURN VALUE TRUE - call allowed 
             FALSE - call does not allowed. 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_epc_allow_iwlan_call
(
  uint32                         pf_id,
  ps_sys_subscription_enum_type  subs_id,
  boolean                        is_v6_call,
  boolean                       *is_ho,
  acl_handoff_ctxt_type         *handoff_ctxt
);

/*===========================================================================
FUNCTION      DS_EPC_UTIL_MAP_TO_NET_DOWN_REASON

DESCRIPTION:  Translate EPC internal errors to PS net down reason

PARAMETERS    EPC internal errors

DEPENDENCIES  None

RETURN VALUE  PS net down reason translated

SIDE EFFECTS  None
===========================================================================*/
ps_iface_net_down_reason_type ds_epc_util_map_to_net_down_reason
(
  ds_epc_status_type  internal_error
);

/*===========================================================================
FUNCTION      DS_EPC_UTIL_REPORT_HO_FAILURE

DESCRIPTION:  Report handoff failure event to framework through 
              ps_iface_generic_ind

PARAMETERS    1st: Pointer the data cache which need to be filled. 
              2nd: Handoff failure reason
              3rd: instance ptr to current PDN

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_util_report_ho_failure
(
  ps_iface_handoff_failure_event_info_type * ho_fail_info_ptr,
  ps_iface_net_down_reason_type              ho_failure_reason,
  ds_epc_cb_type                           * instance_ptr
);
#endif /*FEATURE_DATA_WLAN_MAPCON*/

/*===========================================================================
FUNCTION      DS_EPC_UTIL_ATTACH_PDN_EXIST
 
DESCRIPTION: Check if there is an attach PDN on ehrpd/iwlan and 
             eligible for handoff. 

PARAMETERS: attach_pf_id - LTE attach PDN profile id 
   
RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_util_attach_pdn_exist
( 
  uint16                           attach_pf_id,
  ps_sys_subscription_enum_type    subs_id
);
/*===========================================================================
FUNCTION      DS_EPC_TEAR_DOWN

DESCRIPTION 
  External function. This function tears down EPCs belonging to this profile_id
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_tear_down(uint16 pf_id);

/*===========================================================================
FUNCTION      DS_EPC_CLIENT_NOTIFY

DESCRIPTION 
  External function. 
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_epc_client_notify
(
  boolean    epc_iface_present,
  uint32     subs_id
);

/*===========================================================================
FUNCTION      DS_EPC_IS_ANY_EPC_IFACE_UP

DESCRIPTION 
  External function. 
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_epc_is_any_epc_iface_up
(
  uint32   subs_id
);

/*===========================================================================
FUNCTION     DS_EPC_UTIL_IS_PERMANENT_FAILURE

DESCRIPTION   This function checks the cause code from physical iface down
              event info and determines if the EPC iface should be torn down.

DEPENDENCIES  None

RETURN VALUE  TRUE/FALSE: yes/no

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_util_is_permanent_failure
(
  ps_iface_net_down_reason_type cause_code
);

/*===========================================================================
FUNCTION      DS_EPC_UTIL_GET_APN_NAME

DESCRIPTION   Get the APN name of a EPC instance

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None  
===========================================================================*/
boolean      ds_epc_util_get_apn_name
(
  ds_profile_info_type                  **apn_info,
  ds_epc_cb_type                         *instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_UTIL_GET_SUBS_ID

DESCRIPTION   Get the subscription id of an EPC instance 
 
PARAMETERS    instance_ptr: EPC instance control block ptr 
              subs_id_buf: save the subs_id found in EPC instance ptr 

DEPENDENCIES  None.

RETURN VALUE  TRUE: operation success
              FALSE: operation fail

SIDE EFFECTS  None  
===========================================================================*/
boolean      ds_epc_util_get_subs_id
(
  ds_epc_cb_type                         *instance_ptr,
  uint32                                 *subs_id_buf
);

/*===========================================================================
FUNCTION      DS_EPC_UTIL_IS_IWLAN_WWAN_HO

DESCRIPTION   Check if the handoff is between iwlan and WWAN.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None  
===========================================================================*/
boolean      ds_epc_util_is_iwlan_wwan_ho
(
  ds_epc_cb_type                         *instance_ptr,
  ps_iface_name_enum_type                 trat_name
);

/*===========================================================================
FUNCTION      DS_EPC_UTIL_REPORT_PDN_STATUS_INFO

DESCRIPTION   MPPM needs the PDN UP or DOWN info for T_MinOnWwan timer support.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None  
===========================================================================*/
void ds_epc_util_report_pdn_status_info
(
  ds_epc_cb_type                       * instance_ptr,
  ps_sys_pdn_status_enum_type            pdn_status
);

/*===========================================================================
FUNCTION      DS_EPC_UNTIL_GET_PREF_RAT_TYPE

DESCRIPTION:  Get the TRAT rat type from prefer system. Design for caculating 
              TRAT tech rat when report handoff init event.
  

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds_sys_rat_ex_enum_type ds_epc_util_get_pref_rat_type 
(
  ds_epc_cb_type         * instance_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_UTIL_IFACE_NAME_TO_RAT

DESCRIPTION:  Translate IFACE NAME to DS tech type

PARAMETERS    current prefer system

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds_sys_rat_ex_enum_type ds_epc_util_iface_name_to_rat
(
  ps_iface_type              *iface_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_UTIL_GET_DS_SYS_RAT_BY_PDN_RAT

DESCRIPTION:  Get the ds_sys_rat_ex_enum_type type from PDN's RAT. 
              Design for caculating TRAT tech rat when report handoff init event.
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
ds_sys_rat_ex_enum_type ds_epc_util_get_ds_sys_rat_by_pdn_rat
(
  ds_epc_cb_type         * instance_ptr
);

/*---------------------------------------------------------------------------
  Function pointer to check if the APN is UP on 3GPP
---------------------------------------------------------------------------*/
typedef boolean (*ds_epc_util_if_apn_up_on_3gpp_f_type)
(
  char                             *apn,
  ps_iface_type                   **v4_iface_ptr,
  ps_iface_type                   **v6_iface_ptr,
  ps_sys_subscription_enum_type     ps_subs_id
);


/*---------------------------------------------------------------------------
  Variable to check if PDN is UP on 3GPP
---------------------------------------------------------------------------*/
extern ds_epc_util_if_apn_up_on_3gpp_f_type  
                                ds_3gpp_util_if_apn_up_on_3gpp_f_ptr;

/*===========================================================================
FUNCTION DS_EPC_UTIL_REG_IF_APN_UP_ON_3GPP

DESCRIPTION
  This function is used to register PDN is up on 3GPP with 3GPP MH.
 
PARAMETERS
  

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_epc_util_reg_if_apn_up_on_3gpp
(
  ds_epc_util_if_apn_up_on_3gpp_f_type          apn_up_3gpp_f
);

#endif /*FEATURE_EPC_HANDOFF*/
#endif /* DS_EPC_UTIL_H */

