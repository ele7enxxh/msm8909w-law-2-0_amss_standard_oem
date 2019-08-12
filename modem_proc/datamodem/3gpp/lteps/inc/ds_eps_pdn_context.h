#ifndef DSEPSPDNCONTEXT_H
#define DSEPSPDNCONTEXT_H
/*===========================================================================

            E P S   S P E C I F I C   M O D E   H A N D L E R

===========================================================================*/
/*!
  @file
  ds_eps_pdn_context.h

  @brief
  This header file defines the externalized function that interface DSEPS 
  and ESM.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/        
/*==============================================================================

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/
/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/lteps/inc/ds_eps_pdn_context.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/17/12   dvk     Merged fix to tear down 3gpp2 call when moving from 1x/HDR 
                   to 3gpp system if handoff is not allowed. 
06/17/10   as      Fusion additions
05/17/10   hs      Added APN disable and PDN inactivity feature support.
04/04/10   vk      Added support for attach profile configuration
==============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_bearer_context.h"
#include "ps_iface_defs.h"
#include "ds_dsd_apm_ext_i.h"
#include "ds_eps_throughput_hdlr.h"
#include "lte_cphy_rssi_msg.h"

#if defined(FEATURE_EPC_HANDOFF) && defined(FEATURE_DATA_LTE)
#include "ds_3gpp_epc_if.h"
#endif /* FEATURE_EPC_HANDOFF && FEATURE_DATA_LTE*/


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*--------------------------------------------------------------------------- 
  Structure to store the failure reason (if the current attach fails)
  The recorded values are to be used during the next attempt attempt 
  ---------------------------------------------------------------------------*/
typedef struct
{
  boolean                           is_attach_fail;
   /** Current attach fail status*/
  ps_iface_net_down_reason_type     down_reason;
  /** Attach failure reason */
}ds_eps_prev_attach_status_type;

/*--------------------------------------------------------------------------- 
  DS_EPS_PROCESSING_GET_PDN_CONN_IND is the state of attach sm when we are
  servicing a GET_PDN_CONN_REQ from NAS.
  The attach sm state is changed to DS_EPS_PROCESS_GET_PDN_CONN_IND when we have a
  cached sequence number and there is a  need to re originate a PDN_CONN_REQ
  with the cached sequence number.
  ---------------------------------------------------------------------------*/
typedef enum 
{
   DS_EPS_ATTACH_STATE_INVALID = -1,
   DS_EPS_ATTACH_STATE_NULL,
   DS_EPS_CACHED_GET_PDN_CONN_IND,
   DS_EPS_REDIAL_GET_PDN_CONN_IND,
   DS_EPS_PROCESSING_GET_PDN_CONN_IND, 
   DS_EPS_PROCESSED_CNTXT_TXFR_IND,
   DS_EPS_ATTACHED,
   DS_EPS_EMERGENCY_ATTACHED,
   DS_EPS_MAX_STATES
} ds_eps_attach_state_e;

typedef struct
{
  boolean			               ota_attach_performed;
  ds_eps_attach_state_e            attach_state;  
  uint8				               seq_num;
  uint8				               cached_seq_num; 
  boolean                          is_gw_originated;
  ds_eps_prev_attach_status_type   prev_attach_status;
  boolean                          responding_to_page;
  boolean                          cached_paging_response;
} ds_eps_attach_sm_s;

/*-------------------------------------------------------------------
   Attach fall back IP type. Used during LTE dual ip attach fall back
  -------------------------------------------------------------------*/
typedef enum
{
  DS_EPS_ATTACH_FB_IP_TYPE_MIN =-1,
    /** Min value. Used only for Min value check after NV read*/
  DS_EPS_ATTACH_FB_IP_TYPE_IPV4,
    /** Attach Fall back IP type IPV4 */
  DS_EPS_ATTACH_FB_IP_TYPE_IPV6,
    /** Attach Fall back type IPV6 */
  DS_EPS_ATTACH_FB_IP_TYPE_MAX
    /** Max Value: Used only for Max value check after NV read */
}ds_eps_attach_fb_type;

typedef struct
{
  boolean                    fallback_enabled;
  ds_eps_attach_fb_type      fallback_ip_type;
}ds_eps_attach_fb_info_type;

/*-------------------------------------------------------------------
   Structure to hold LTE p and s cell information
  -------------------------------------------------------------------*/
typedef struct
{
  lte_cphy_cmapi_cell_info_s pcell_info;
  lte_cphy_cmapi_cell_info_s scell_info;
}ds_eps_lte_cell_info_type;

typedef struct 
{
  uint16  version ;          /* version of the NV item               */
  uint16  ue_category;       /* UE Category                          */
  uint32  aggr_dl_bw;        /* Aggregated Downlink Bandwidth in MHz */
  uint32  aggr_ul_bw;        /* Aggregated Uplink Bandwidth in MHz   */
  uint8   notif_mechanism;   /* Notification Mechanism for CA        */
  uint8   reserved_field[3]; /* reserve fields for future use        */
}ds_eps_ca_display_threshold_s;

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

extern ds_eps_attach_sm_s ds_eps_attach_sm[DS3GSUBSMGR_SUBS_ID_MAX];

/*===========================================================================
FUNCTION DS_EPS_PDN_CNTXT_GET_T3402_TIMER_VAL

DESCRIPTION
  Returns T3402 timer value maintained within DS that is obtained via
  NAS_EMM_T3402_CHANGED_IND.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
 T3402 timer value

SIDE EFFECTS
  None.

===========================================================================*/
uint32 ds_eps_pdn_cntxt_get_t3402_timer_val(void);


/*===========================================================================
FUNCTION DS_EPS_GET_ATTACH_PROF

DESCRIPTION
  This function returns the profile number to be used to establish the 
  default PDN connection during LTE attach. If an attach profile number is 
  configured then that one is returned. Otherwise, the default profile
  number is returned. 
  
PARAMETERS  
  None.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  uint16 - the profile number of the profile to be used for default PDN.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
uint16 ds_eps_get_attach_prof
( 
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_EPS_SET_LTE_ATTACH_DUAL_IP_FB_IP_TYPE

DESCRIPTION 
  This function sets Attach Dual IP FB IP type.
 
PARAMETERS
  None
DEPENDENCIES
  The fall back IP type is to be used when the network responds with a
  PDN Conn Rej with ESM cause #28 for Initial Attach request (only). 

RETURN VALUE
  None
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_set_lte_attach_dual_ip_fb_ip_type
(
  boolean                           activation_status,
  ds_eps_attach_fb_type             attach_fb_ip_type,
  sys_modem_as_id_e_type            subs_id
);
/*===========================================================================
FUNCTION DS_EPS_PDN_CNTXT_INIT

DESCRIPTION
  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_cntxt_init( void );

/*===========================================================================
FUNCTION  DSLTEPS_LTE_GET_DEFAULT_V4_IFACE_PTR

DESCRIPTION
  This function returns the DEFAULT LTE v4 PS IFace ptr. 
  This is used for the test framework.

PARAMETERS
  subs_id: Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
ps_iface_type *ds_eps_pdn_cntxt_get_default_v4_iface_ptr
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION  DSLTEPS_LTE_GET_DEFAULT_V6_IFACE_PTR

DESCRIPTION
  This function returns the default LTE v6 PS IFace ptr.
  This is used for the test framework.

PARAMETERS
  subs_id: Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
ps_iface_type *ds_eps_pdn_cntxt_get_default_v6_iface_ptr
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION  DSLTEPS_LTE_GET_V4_IFACE_PTR_FROM_EPS_BEARER_ID

DESCRIPTION
  This function returns the LTE v4 PS IFace ptr corresponding to the EPS
  Bearer ID. This is used for the test framework. 
  
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
ps_iface_type *ds_eps_pdn_cntxt_get_v4_iface_ptr_from_eps_bearer_id
(
  eps_bearer_id_T        eps_bearer,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION  DSLTEPS_LTE_GET_V6_IFACE_PTR_FROM_EPS_BEARER_ID

DESCRIPTION
  This function returns the LTE v6 PS IFace ptr corresponding to the EPS
  Bearer ID. This is used for the test framework.

PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
ps_iface_type *ds_eps_pdn_cntxt_get_v6_iface_ptr_from_eps_bearer_id
(
  eps_bearer_id_T        eps_bearer,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION  DSEPSBEARERCNTXT_PHYSLINK_PTRFROMBEARERID

DESCRIPTION
  This function returns the phys link associated with the EPS Bearer ID 
  
PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
ps_phys_link_type *ds_eps_bearer_cntxt_phys_link_ptr_from_eps_bearer_id
(
  eps_bearer_id_T        eps_bearer,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION  DS_EPS_PDN_CNTXT_GET_PDN_CONN_IND_HDLR

DESCRIPTION
  This function is responsible for the initiation of sending out PDN conn
  request during EMM attach process. This internally calls another function,
  ds_eps_pdn_cntxt_bring_up_attach_pdn to get the job done
  
PARAMETERS
  attach_info_p

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS - if all operations went fine
  DS3G_FAILURE - if the attach APN is disabled

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_pdn_cntxt_get_pdn_conn_ind_hdlr
(
  ds3g_eps_attach_sm_info_type  *attach_info_ptr,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION  DS_EPS_PDN_CNTXT_PDN_CONN_REJ_IND_HDLR

DESCRIPTION
  This function is used to handle the PDN connection reject indication.
  The only function that is to be done here is to update the APN table

  If a connection of the other IP type has already been made and if the APN
  table says that the APN supports single IP bearer, it has to be changed to
  the IP type of the existing call.

  Ex. V4 call was already made. APN table has Single IP bearer support for 
  this APN. V6 call was rejected. Update the APN type to V4 only.

  Note: However when the V4 call goes away, we no longer have any info about
  the IP type of the APN. Hence when the V4 call goes away, we will send out
  a request of type V6 if a V6 app comes up.

PARAMETERS
  mode_info_ptr_i:  Mode specific information

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_cntxt_pdn_conn_rej_ind_hdlr
(
  ds_pdn_context_s           *pdn_cntx_p,
  cm_call_mode_info_s_type   *mode_info_p
);

/*===========================================================================
FUNCTION      DS_EPS_PDN_CNTXT_GET_LTE_ATTACH_PARAMS

DESCRIPTION
  Updates the Attach profile APN info when an IOCTL query is received

PARAMETERS
  ioctl_name[in]  -      Operation name
  arg_val_ptr[in] -      Pointer to operation specific argument
  ps_errno [out]  -       Specifies type of error in case of error.
                             DS_EFAULT if parameter is invalid
                             DS_EINVAL if operation is invalid
                             DS_NOMEMORY if memory allocation fails

DEPENDENCIES
  None.

RETURN VALUE  
  0  - Success
  -1 - Failure

SIDE EFFECTS
  None.
===========================================================================*/
int ds_eps_pdn_cntxt_get_lte_attach_params
(
  void                    *arg_val_ptr,
  sys_modem_as_id_e_type   subs_id,
  int16                   *ps_errno
);

#if defined(FEATURE_EPC_HANDOFF) && defined(FEATURE_DATA_LTE)
/*===========================================================================
FUNCTION DS_EPS_PDN_CNTXT_REG_EMM_ATTACH_HDLR

DESCRIPTION
  This function is used to register the EMM Attach handler with the LTE
  Mode Handler. This API is called by EPC Mode Handler. Whenever an attach
  indication is received in the LTE mode, we check if the EPC mode handler
  has registered with LTE MH. If it has, the attach handler provided by EPC
  MH is invoked.

PARAMETERS 
  emm_attach_hdlr_f - Pointer to the EMM attach handler provided by EPC MH
 
DEPENDENCIES
  None.

RETURN VALUE
  None.
 
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_cntxt_reg_emm_attach_hdlr
(
  ds_3gpp_if_emm_attach_hdlr_f_type emm_attach_hdlr_f
);

#endif /* #if defined(FEATURE_EPC_HANDOFF) && defined(FEATURE_DATA_LTE) */

/*===========================================================================
FUNCTION  DS_EPS_PDN_CNTXT_SYS_CHANGE_HANDLER

DESCRIPTION
  This function is responsible for the teardown of the context when the mode
  changes to anything other than LTE

  Note that this function deals with tearing down only the default contexts
  However if the non default bearers have to be torn down as well, this
  section has to be revisited

  This function handles two different cases:
  1. EPC mode handler available - In this case, this tears down a PDN context
     that may have Ifaces in out_of_use state. This is because such contexts
     are not transferred on handover and will have to be deleted by LTE MH
     locally.
  2. EPC mode handler unavailable - In this case, this function just tears 
     down all PDN contexts.
  
PARAMETERS
  new_mode - New mode we are in
  old_mode - Old mode we were in

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_cntxt_sys_change_handler
(
  sys_sys_mode_e_type           new_mode,
  sys_sys_mode_e_type           old_mode,
  sys_modem_as_id_e_type        subs_id
);

#if 0
/*===========================================================================
FUNCTION  DS_EPS_PDN_CNTXT_PREF_SYS_CHG_HDLR

DESCRIPTION
  This function is responsible for the teardown of the context when the
  preferred system changes, as indication from DSD

PARAMETERS
  sys_sys_mode_e_type old_pref_sys: New preferred system we are in
  sys_sys_mode_e_type new_pref_sys: Old preferred system we are in

DEPENDENCIES
  presence of cne-dsd api's

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_pdn_cntxt_pref_sys_chg_hdlr
( 
  sys_sys_mode_e_type old_pref_sys,
  sys_sys_mode_e_type new_pref_sys
);
#endif


/*===========================================================================
FUNCTION DS_EPS_ATTACH_SM_HANDLE_LAST_BEARER_DEACT

DESCRIPTION
  If there are any pending calls, then GET_PDN_CONN_IND is cached and its
  processing is delayed till all pending calls are ended.
  
PARAMETERS  
  None.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None. 
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_eps_attach_sm_handle_last_bearer_deact
(
  sys_modem_as_id_e_type    subs_id
);


/*===========================================================================
FUNCTION DS_EPS_ATTACH_SM_GET_STATE

DESCRIPTION
  Returns current state for attach SM. 
  
PARAMETERS  
  None.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
ds_eps_attach_state_e ds_eps_attach_sm_get_state
(
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_EPS_ATTACH_SM_IS_UE_ATTACHED

DESCRIPTION
  Queries the EPS attach SM whether the UE is attached or not. 
  
PARAMETERS  
  None.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if UE is attached in LTE mode
  FALSE otherwise
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_eps_attach_sm_is_ue_attached
(
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_EPS_PDN_CNTXT_HANDLE_CNTXT_TRANSFER_IND

DESCRIPTION
  This function is used to invoke any LTE specific operations on
  receiving context-transfer indication.

PARAMETERS   
  None.
    
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_eps_pdn_cntxt_handle_cntxt_txfr_ind
(
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_EPS_PDN_CNTX_IS_PDN_CONN_REQ_PENDING

DESCRIPTION
  This function is used to check if a PDN CONN request to be sent to NAS
  is pending 

PARAMETERS 
  None
 
DEPENDENCIES
  None.

RETURN VALUE
  True  - If cached seq_num is > 0
  False - If cached seq number is 0
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_pdn_cntx_is_pdn_conn_req_pending
(
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_EPS_ATTACH_SM_SET_STATE

DESCRIPTION
  Returns current state for attach SM.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_attach_sm_set_state
(
  ds_eps_attach_state_e   attach_state,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_EPS_GET_DEFAULT_SOCKET_PROFILE

DESCRIPTION
  This function is used to retrieve the default socket profile from pdp
  registry

PARAMETERS 
  subs_id: data sub id
 
DEPENDENCIES
  None.

RETURN VALUE
  uint16 : default socket profile id
 
SIDE EFFECTS
  None.
===========================================================================*/

uint16 ds_eps_get_default_socket_profile 
(
  sys_modem_as_id_e_type    subs_id
);


/*===========================================================================
FUNCTION ds_eps_get_attach_sm_per_sub_id

DESCRIPTION
  This function sets the value of NV 70334 REMOVE_UNUSED_PDN .
  This function is only used in Test framework and should not be used to set
  anywhere else other than test cases.
PARAMETERS 
  None
 
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
ds_eps_attach_sm_s* ds_eps_get_attach_sm_per_sub_id
(
  sys_modem_as_id_e_type  subs_id
);



/*===========================================================================
FUNCTION  ds_eps_pdn_cntxt_allocate_pdn_and_bringup_ifaces

DESCRIPTION
  This functions allocates a new PDN context and new DS ifaces and brings up
  the ifaces.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS - if all operations went fine
  DS3G_FAILURE - on error

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_pdn_cntxt_allocate_pdn_and_bringup_ifaces
(
  uint16                    profile_no,
  ps_iface_type           **iface,
  ds_umts_pdp_profile_type *prof_context,
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_EPS_PDN_CNTX_RESET_ATTACH_SM_PARAMS

DESCRIPTION
  This function is used to reset the attch state machine parameters

PARAMETERS 
  None
 
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_pdn_cntx_reset_attach_sm_params
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_EPS_PDN_CNTXT_UPDATE_IP_TYPE_FOR_V4V6_WITH_INVALID_ESM_CAUSE

DESCRIPTION
  This function is used to handle cases where esm cause code valid flag is
  set to 0. The IP support and redial mechanism is updated based on the global variable
  ds_3gpp_redial_for_no_cause_code .

PARAMETERS 
  IP support   : APN type for the PDN being brought up
  Pdn_type_val : Network returns PDP_TYPE
 
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_pdn_cntxt_update_ip_type_for_v4v6_with_invalid_esm_cause
(
  ds_apn_ip_support_type_e  *ip_support,
  pdn_type_T                 pdn_type_val,
  sys_modem_as_id_e_type     subs_id
);

/*===========================================================================
FUNCTION DS_EPS_PDN_CNTXT_UPDATE_PARAM_FOR_NEW_IP_TPYE

DESCRIPTION
  This function is used to update the APM cause code based on the ip support
  provided

PARAMETERS 
  ind_payload  : APM Payload to be filled
  IP support   : APN type for the PDN being brought up
  Pdn_type_val : Network returns PDP_TYPE
 
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_pdn_cntxt_update_param_for_new_ip_type
(
  ds_apn_ip_support_type_e     ip_support,
  ds_pdn_context_s            *pdn_cntxt_p,
  ds_dsd_apm_ind_pay_load     *ind_payload
);

/*===========================================================================
FUNCTION DS_3GPP_APN_PARAM_CHANGE_CMD_HDLR

DESCRIPTION
  This function detects if any of following profile params has changed or not
  1.  APN disabled flag
  2.  APN Name
  3.  Maximum PDN connections per block
  4.  Maximum PDN connection wait timer
  5.  PDN request wait timer
  6.  APN bearer
  7.  APN class
  8.  PDP Type
 
  If any of those above params have changed then it calls another which handles
  PDN disconnect processing if any PDN is up with the changed profile
 
PARAMETERS 
  prof_num:                 Profile whose contents have changed
  subs_id                   Profile Subscription ID
  prf_data:                 Profile specific data (mask, new values ....)
 
DEPENDENCIES
  None.

RETURN VALUE
  void 
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_apn_param_change_cmd_hdlr
(
  uint16                           prof_num,
  sys_modem_as_id_e_type           subs_id,
  void                            *prf_data
);

/*===========================================================================
FUNCTION DS_EPS_PDN_CNTXT_MOVE_BEARER_FROM_PEND_REDIAL_TO_DOWN_IF_NEEDED

DESCRIPTION
   Make a few validations and make a decision, if the bearer and PDN
   should continue to remain in PENDING_REDIAL mode or move it to GOING_DOWN
   Basically if the validation fails, a redial on the same PDN is not
   possible and hence move the state to GOING DOWN.
 
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_pdn_cntxt_move_bearer_from_pending_redial_to_down_if_needed
(
   ds_bearer_context_s *bearer_context_p
);

/*===========================================================================
FUNCTION DS_EPS_HANDLE_APN_PARAM_CHG_REDIAL

DESCRIPTION
 Fetch the apn parameters from profile and send out a pdn connectivity request.
 
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_handle_apn_param_chg_redial
(
   ds_pdn_context_s *pdn_context_p,
   ds_bearer_context_s *bearer_context_p
);

/*===========================================================================
FUNCTION DS_EPS_APN_PARAM_CHANGE_CMD_HDLR

DESCRIPTION
  This function sends out a pdn disconnect request and reconnects with
  the new changed parameters.

PARAMETERS 
  uint8 prof_num
 
DEPENDENCIES
  None.

RETURN VALUE
  void 
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_apn_param_change_cmd_hdlr
(
   uint16                prof_num
);

/*===========================================================================
FUNCTION DS_EPS_PDN_CNTXT_RESET_TARGET_RAT

DESCRIPTION
  This function resets target rat after attach failure and updates 3GPP network
  mode to current preferred mode.
PARAMETERS 
  pdn_cntxt_p
 
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_pdn_cntxt_reset_target_rat
(
  ds_pdn_context_s  *pdn_cntxt_p
);

#ifdef FEATURE_EMERGENCY_PDN
/*===========================================================================
FUNCTION DS_EPS_PDN_CNTX_DEACT_NON_EMC_BEARER_IND_HDLR

DESCRIPTION
  This function is used to handle the message from NAS. This message is sent
  whenever T3420 timer expires

  Here we validate the message ID and we update ds_eps_emergency_auth_fail
  
PARAMETERS
  msgr_type - UMID of the message sent
  dsmsg_ptr - Pointer to the payload

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/ 
void ds_eps_pdn_cntxt_deact_non_emc_bearer_ind_hdlr
(
  msgr_umid_type             msgr_type,
  const msgr_hdr_struct_type *dsmsg_ptr
);
#endif /*FEATURE_EMERGENCY_PDN*/

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================
FUNCTION DS_EPS_GET_LTE_ATTACH_DUAL_IP_FB_STATUS

DESCRIPTION 
  This function returns the flag which tells whether dual IP FB feature is
  enabled or not
 
PARAMETERS
  None
 
DEPENDENCIES
  None 

RETURN VALUE
  TRUE: If the feature is enabled
  FALSE: Otherwise
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_get_lte_attach_dual_ip_fb_status
(
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_EPS_PDN_CNTX_RESET_ATTACH_APN_IND_HDLR

DESCRIPTION
  This function is used to notify APM to reset Attach APN to Class 1 APN
  on successful PLMN blk cback from CM

PARAMETERS
  client_data_ptr: Current active PS subs

DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_pdn_cntxt_sys_sel_pref_no_err_cmd_hdlr
(
  void                           *client_data_ptr
);

/*===========================================================================
FUNCTION DS_EPS_PDN_CNTXT_SET_LTE_CELL_INFO

DESCRIPTION
  This function is used to update the ds_eps_lte_cell_info cache

PARAMETERS 
  subs_id - subscription on which cell info is to be cached 
  cell_info - info to be cached

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/ 
void ds_eps_pdn_cntxt_set_lte_cell_info
(
   sys_modem_as_id_e_type    subs_id,
   ds_eps_lte_cell_info_type cell_info
);

/*===========================================================================
FUNCTION DS_EPS_PDN_CNTXT_GET_LTE_CELL_INFO

DESCRIPTION
  This function is used to retreive the ds_eps_lte_cell_info cache

PARAMETERS
  subs_id - subscription on which to retreive cached cell info

DEPENDENCIES
  None.

RETURN VALUE
  cell_info - cached cell info.

SIDE EFFECTS
  None.
===========================================================================*/ 
ds_eps_lte_cell_info_type ds_eps_pdn_cntxt_get_lte_cell_info
(
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION DS_EPS_PDN_CNTXT_GET_ACTIVE_SCELL

DESCRIPTION
  This function returns if scell is active on a given subscription
  for carrier aggregation

PARAMETERS
  subs_id - subscription to retrieve scells

DEPENDENCIES
  None.

RETURN VALUE
  Number of active scells.  

SIDE EFFECTS
  None.
===========================================================================*/ 
uint8 ds_eps_pdn_cntxt_get_active_scell
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_EPS_PDN_CNTXT_GET_LTE_BANDWIDTH

DESCRIPTION
  This function return LTE bandwidth in KHz from Number of Resource Bearers

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Bandwidth in KHz

SIDE EFFECTS
  None.
===========================================================================*/ 
uint32 ds_eps_pdn_cntxt_get_lte_bandwidth
(
  lte_bandwidth_e lte_bw_nrb
);

/*===========================================================================
FUNCTION  DS_EPS_PDN_CNTXT_GET_RESOLVED_APN_PER_ATTACH_SM

DESCRIPTION
  This function gets the resolved apn stored in attach sm,

PARAMETERS
  resolved_apn_ptr
  subs_id
 
DEPENDENCIES
  None.

RETURN VALUE 
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_pdn_cntxt_get_resolved_apn_per_attach_sm
(
   byte                    *resolved_apn_ptr,
   sys_modem_as_id_e_type   subs_id
);

#endif    /* DSEPSPDNCONTEXT_H */

