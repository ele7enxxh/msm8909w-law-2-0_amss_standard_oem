/*!
  @file
  ds_3gpp_pdn_redial_hdlr.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2010-2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_pdn_redial_hdlr.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/06/13   nd      NV item to make dual IP fallback cause codes configurable.
01/25/12   rr      Auth fallback support for cause code 31 - prioritizes auth fallback
                   over dual IP fallback
09/09/11   rr      Added support for auth fallback feature. 
07/27/11   rr      Support for redial handling


===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "sys.h"

#include "ds_3gpp_pdn_redial_hdlr.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_hdlr.h"
#include "ds_3gpp_bearer_context.h"
#include "ds_umts_bearer_context.h"
#include "ds_3gpp_auth_mgr.h"
#include "ds_3gpp_apn_switch_mgr.h"
#include "ds_eps_pdn_context.h"
#include "ds_3gppi_utils.h"
#include "ds_3gpp_pdn_throttle_sm.h"
/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/
#define REASON_FROM_IFACE_CAUSE_CODE(code) (0x0000FFFF & code)

/* Defining the Timer value of 2 Mins for Pending Calls existence */
#define PENDING_REDIAL_TIMER_VALUE 2
/*---------------------------------------------------------------------------
  Dual IP fallback cause codes: Filled in EFS file.
  We will read this NV item on first network reject and set the read flag.
  In consecutive reject we will use the read value stored in array.
---------------------------------------------------------------------------*/

static uint16 dual_ip_fallback_codes[DS_3GPP_DUAL_IP_FALLBACK_CAUSE_CODE_MAX] = {0};
static boolean fallback_cause_code_read = FALSE;

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

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
)
{
  int cause_code_counter = 0;

  /*-------------------------------------------------------------------------
    Read the Dual IP fallback cause codes filled in NV.
    No redial if APN REJECT THROTTLING IS SET
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_LTE
  if((ds_3gpp_is_context_pref_pkt_mode(pdn_context_p, SYS_SYS_MODE_LTE) &&
      ds_eps_get_lte_attach_dual_ip_fb_status(subs_id) == FALSE) ||
     (ds_3gpp_pdn_throt_get_apn_reject_pdn_blocked_status(subs_id)))
  {
    return FALSE;
  }
#endif

  if(fallback_cause_code_read == FALSE)
  {
    if(ds_3gpp_cfg_get_subsid(DS_3GPP_DUAL_IP_FALLBACK_CAUSE_CODES, 
                      (unsigned char *) &dual_ip_fallback_codes,
                      (sizeof(uint16) * DS_3GPP_DUAL_IP_FALLBACK_CAUSE_CODE_MAX),
                       subs_id)
                      == -1)
    {
      DS_3GPP_MSG0_ERROR("Cannot read EFS item DS_3GPP_DUAL_IP_FALLBACK_CAUSE_CODES");
      /* write default values */
      dual_ip_fallback_codes[0] = 
        REASON_FROM_IFACE_CAUSE_CODE(PS_NET_DOWN_REASON_UNKNOWN_PDP); /* 28 */
      for(cause_code_counter = 1; 
          cause_code_counter < DS_3GPP_DUAL_IP_FALLBACK_CAUSE_CODE_MAX;
          cause_code_counter++)
      {
        dual_ip_fallback_codes[cause_code_counter] = 
            REASON_FROM_IFACE_CAUSE_CODE(PS_NET_DOWN_REASON_MAX);
      }
    }
    fallback_cause_code_read = TRUE;
  }
  for(cause_code_counter = 0; 
      cause_code_counter < DS_3GPP_DUAL_IP_FALLBACK_CAUSE_CODE_MAX;
      cause_code_counter++)
  {
      if(dual_ip_fallback_codes[cause_code_counter] 
         == down_reason)
    {
      DS_3GPP_MSG2_HIGH("ds_3gpp_pdn_cntxt_dual_ip_redial_required: Redial for"
                      "cause code %d set at index %d",
                       down_reason, cause_code_counter);
      return TRUE;
    }
  }
  DS_3GPP_MSG1_HIGH("ds_3gpp_pdn_cntxt_dual_ip_redial_required: No Redial for"
                    "cause code %d Not set in NV",
                     down_reason);
  return FALSE;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_PERFORM_DUAL_IP_REDIAL_CHECK

DESCRIPTION 
  This function is used to determine if dual ip redial check is necessary

PARAMETERS
  IN
    pdn_context_p      - Pointer to the pdn context we are working on
    bearer_context_p   - Pointer to the bearer context we are working on
    down_reason        - Netdown reason for current call end.
                         (since we may redial for specific cause codes)
    call_was_connected - Whether the call was connected or not

DEPENDENCIES
  None.

RETURN VALUE 
  boolean 
   
SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_perform_dual_ip_redial_check
(
  ds_pdn_context_s           *pdn_context_p,
  ds_bearer_context_s        *bearer_context_p,
  ds_3gpp_down_reason_t      down_reason,
  boolean                    call_was_connected
)
{
  sys_modem_as_id_e_type    subs_id = SYS_MODEM_AS_ID_NONE;
  boolean                   ret_val = FALSE;
  sys_sys_mode_e_type       mode = SYS_SYS_MODE_NONE;
  sys_sys_mode_e_type       bearer_mode = SYS_SYS_MODE_NONE;
#ifdef FEATURE_DATA_LTE
  boolean                   is_apn_throttled  = FALSE;
#endif /* FEATURE_DATA_LTE */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - -*/

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return ret_val;
  }

  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p,&subs_id))
  {
    return ret_val;
  }

  mode = ds_3gpp_get_apn_current_mode(pdn_context_p);

  bearer_mode = (sys_sys_mode_e_type) 
  bearer_context_p->ds_bearer_context_dyn_p->call_mode;

  if((down_reason.reason_T == DS_3GPP_PS_IFACE_NET_DOWN_REASON_T) &&
     (bearer_context_p->ds_bearer_context_dyn_p->state == 
      DS_BEARER_CONTEXT_STATE_PEND_REDIAL) &&
     (pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type
       == DS_UMTS_PDP_IPV4V6) &&
     (call_was_connected == FALSE) &&
     (DS_3GPP_IS_3GPP_CURRENT_MODE(mode)) &&
     (DS_3GPP_IS_3GPP_CURRENT_MODE(bearer_mode))
    )
  {
#ifdef FEATURE_DATA_LTE
    if(mode == SYS_SYS_MODE_LTE)
    {
      is_apn_throttled = 
        ds_pdn_cntxt_is_apn_reject_sm_throttled(pdn_context_p);

      /*----------------------------------------------------------- 
        If apn_throttled and state is ATTACHED, then no redial
        -----------------------------------------------------------*/
      if(!(is_apn_throttled && (ds_eps_attach_sm_get_state(subs_id) 
                               == DS_EPS_ATTACHED)))
      {
        ret_val = TRUE;
      }

    }
    else
#endif
    {
      ret_val = TRUE;
    }

  }

  DS_3GPP_MSG1_HIGH("ds_3gpp_pdn_cntxt_perform_dual_ip_redial_check "
                    "returns:%d",ret_val);

  return ret_val;
}


/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_NEEDS_REDIAL

DESCRIPTION 
  This function is used to determine if redialing is required 

PARAMETERS
  IN
    pdn_context_p      - Pointer to the pdn context we are working on
    bearer_context_p   - Pointer to the bearer context we are working on
    down_reason        - Netdown reason for current call end.
                         (since we may redial for specific cause codes)
    call_was_connected - Whether the call was connected or not

DEPENDENCIES
  None.

RETURN VALUE
   DS_3GPP_NO_REDIAL if there is no need to redial
   DS_3GPP_REDIAL_UMTS_LTE_IRAT in case we are redialing because of GW_L_IRAT
   DS_3GPP_REDIAL_DUAL_IP_FALLBACK in case we are redialing because of
   Dual IP fallback for UMTS
   DS_3GPP_REDIAL_AUTH_FALLBACK in case we are redialing because of
   authentication fallback
 
   Care should be taken to see that we return DS_3GPP_NO_REDIAL only
   once in the function, at the very end. If a particular check
   fails, we do not return DS_3GPP_NO_REDIAL. We proceed to the next
   check. If all checks for redial fail, we return DS_3GPP_NO_REDIAL.
   
SIDE EFFECTS 
  None.
===========================================================================*/
ds_3gpp_redial_e_type ds_3gpp_pdn_cntxt_needs_redial
(
  ds_pdn_context_s           *pdn_context_p,
  ds_bearer_context_s        *bearer_context_p,
  ds_3gpp_down_reason_t      down_reason,
  boolean                    call_was_connected
)
{
  sys_modem_as_id_e_type    subs_id = SYS_MODEM_AS_ID_NONE;
  ds_umts_pdp_auth_enum_type requested_auth_type;
  network_params_info_type*  ppp_info_p = NULL;

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return DS_3GPP_NO_REDIAL;
  }

  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p,&subs_id))
  {
    return DS_3GPP_NO_REDIAL;
  }

  /*-------------------------------------------
       Redial occurs for default bearers only
   -------------------------------------------*/
  if(!bearer_context_p->ds_bearer_context_dyn_p->is_default)
  {
    DS_3GPP_MSG0_HIGH("ds_3gpp_pdn_cntxt_needs_redial: Cannot redial."
                      "Redial not supported for non-default bearers");
    return DS_3GPP_NO_REDIAL;
  }
  if(!(pdn_context_p->ds_pdn_context_dyn_p->info_null))
  {
    ppp_info_p = &(pdn_context_p->ds_pdn_context_dyn_p->info);
  }

  /*---------------------------------------------------------------------------- 
  Redial occurrs in one of the following cases
  Case (a) UE tries to bring up default PDN and NW redirects to LTE
              during RRC connection establishment.
           Redial only for the default PDN.
           Redial only if the orig call was placed in UMTS mode
   
  Case (b) UE requests auth type PAP and CHAP. 
           If NW sends call end because of AUTH failure, UE retries with
           PAP and NO Auth (initial try with CHAP) 

  Case (c) UE requests for dual ip type (V4V6) and NW rejects with error codes:
           28 (Unknown address or PDP type),
           30 (Activation rejected by GGSN, PDN-GW or S-GW)
           31 (Activation rejected, unspecified)
           33 (Requested service option not subscribed).
           In these cases, it is assumed that the NW does not understand
           IP type V4V6.
           If call mode is UMTS/LTE
           Redial if the original call was made in UMTS/LTE mode and if it is
           still the preferred packet mode
           
           If RPM is enabled, we assume that apn is being throttled and
           we do not redial.
   
     -------------------------------------------------------------------------------*/
  

  /*-------------------------------------------
    Case (a)
   -------------------------------------------*/

  if((down_reason.reason_T == DS_3GPP_EXTENDED_NET_DOWN_REASON_T)
  && ((down_reason.reason.internal_reason == UE_GW_L_RAT_CHANGE) 
      #if defined FEATURE_DATA_LTE
       || (ds_eps_attach_sm_get_state(subs_id) == DS_EPS_CACHED_GET_PDN_CONN_IND)
      #endif
      )
  && (bearer_context_p->ds_bearer_context_dyn_p->state == 
      DS_BEARER_CONTEXT_STATE_PEND_REDIAL)
  && (call_was_connected == FALSE)
  && (ds_pdn_cntxt_is_default(pdn_context_p))
  && (pdn_context_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_PEND_REDIAL))
  {
        return DS_3GPP_REDIAL_UMTS_LTE_IRAT;
  }

  /*------------------------------------------- 
    Case (b)
   -------------------------------------------*/
  if(ppp_info_p && ((ppp_info_p->valid_fields & NET_CFG_PRM_AUTH_PREF_MASK)||
      (ppp_info_p->valid_fields & NET_CFG_PRM_AUTH_USERNAME_MASK)||
      (ppp_info_p->valid_fields & NET_CFG_PRM_AUTH_PASSWORD_MASK) ))
  {
    requested_auth_type = (ds_umts_pdp_auth_enum_type) 
                          pdn_context_p->ds_pdn_context_dyn_p->
                          info.auth_info.auth_type;
    DS_3GPP_MSG1_HIGH("Read auth_type %d from info", requested_auth_type);
  }
  else
  {
    requested_auth_type = pdn_context_p->ds_pdn_context_dyn_p->
                          pdp_profile.auth.auth_type;
    DS_3GPP_MSG1_HIGH("Read auth_type %d from profile", requested_auth_type);
  } 
  if((down_reason.reason_T == DS_3GPP_PS_IFACE_NET_DOWN_REASON_T)
     && ds_3gpp_auth_mgr_need_redial(pdn_context_p->ds_pdn_context_dyn_p->
                                     pdp_profile.context.apn,
                                     requested_auth_type,
                                     down_reason.reason.ps_iface_down_reason,
                                     call_was_connected,
                                     subs_id))
  {
    return DS_3GPP_REDIAL_AUTH_FALLBACK;
  }

  /*------------------------------------------- 
    Case (c)
   -------------------------------------------*/

  if((ds_3gpp_pdn_cntxt_perform_dual_ip_redial_check
      (pdn_context_p,bearer_context_p,down_reason,call_was_connected)) &&
      ds_3gpp_pdn_cntxt_dual_ip_redial_required(pdn_context_p,
     REASON_FROM_IFACE_CAUSE_CODE(down_reason.reason.ps_iface_down_reason),
                                                subs_id))
  {
    return DS_3GPP_REDIAL_DUAL_IP_FALLBACK;
  }
 
  return DS_3GPP_NO_REDIAL;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_SETUP_REDIAL

DESCRIPTION
  This function checks with the 3gpp bearers if redialing is required.
  If so, sets up the pdn context for redial and sends out
  DS3GPP_RESEND_PDN_CONN_REQ_CMD.
  Otherwise, it cleans up the bearer and returns

PARAMETERS   
  pdn_cntx_p         - pdn context that we are workin on
  down_reason        - reason for bearer deactivation
  call_was_connected - whether the call was connected or not
    
DEPENDENCIES
  Should always be called from within a critical section 
  ds3gpp_global_crit_sect

RETURN VALUE
  None
  
 
SIDE EFFECTS
  None

===========================================================================*/
/* fucntion sets up pdn for redial.  */
void ds_3gpp_pdn_cntxt_redial_hdlr
( 
  ds_pdn_context_s * pdn_cntx_p, 
  ds_bearer_context_s *bearer_cntx_p,
  ds_3gpp_down_reason_t down_reason,
  boolean call_was_connected
)
{
  ds_3gpp_iface_s                      *ds_v4_iface_ptr;
  ds_3gpp_iface_s                      *ds_v6_iface_ptr;
  ds_pdn_context_s                     *new_pdn_cntx_p;
  ds_3gpp_redial_e_type                needs_redial_res;
  boolean                              redial_not_performed;
  boolean                              v4_iface_in_use;
  boolean                              v6_iface_in_use;
  boolean                              v4_iface_throttled;
  boolean                              v6_iface_throttled;
  boolean                              redial_v4;
  boolean                              redial_v6;
  sys_modem_as_id_e_type               subs_id  = SYS_MODEM_AS_ID_NONE;
  ds_bearer_cntxt_per_subs_info_type  *per_subs_info_hndl = NULL;
  ds_cmd_type                         *cmd_ptr = NULL;
  ds_pdn_context_s                    **data_ptr = NULL;
  /*------------------------------------------------------------------*/
  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return;
  }

  per_subs_info_hndl = 
    ds_bearer_cntxt_get_per_subs_info_hndl(subs_id);

  if (!ds_bearer_cntxt_validate_per_subs_info_hndl
        (per_subs_info_hndl))
  {
    DS_3GPP_MSG1_ERROR("Bearer Cntxt per subs info handle is invalid for"
                       "subs id",subs_id);
    return;
  }

  if(!ds_bearer_cntx_validate_bearer_context(bearer_cntx_p))
  {
    return;
  }

  needs_redial_res = ds_3gpp_pdn_cntxt_needs_redial(pdn_cntx_p,
                                 bearer_cntx_p, down_reason, call_was_connected);
  
  redial_not_performed = TRUE;
 
  switch (needs_redial_res)
  {
  case DS_3GPP_REDIAL_DUAL_IP_FALLBACK:
      DS_3GPP_MSG3_HIGH("ds_3gpp_pdn_cntxt_redial_hdlr: Dual IP fallback"
                        "call id:%d Bearer Inst:%d Call mode:%d",  bearer_cntx_p->
                        ds_bearer_context_dyn_p->cm_call_id,
	                    (uint32)bearer_cntx_p->
                        ds_bearer_context_dyn_p->client_data_ptr,
                        bearer_cntx_p->
                        ds_bearer_context_dyn_p->call_mode); 

      ds_v4_iface_ptr = DSPDNCNTXT_GET_V4_DS_IFACE_PTR(pdn_cntx_p);
      ds_v6_iface_ptr = DSPDNCNTXT_GET_V6_DS_IFACE_PTR(pdn_cntx_p);

      if(ds_v4_iface_ptr ==  NULL || ds_v6_iface_ptr == NULL)
      {
        DS_3GPP_MSG0_HIGH("ds_3gpp_pdn_cntxt_redial_hdlr: V4/v6 ifaces NULL."
                          "Not redialing");
        break;
      }
      
      v4_iface_in_use = PS_IFACE_IS_IN_USE(&(ds_v4_iface_ptr->ps_iface));
      v6_iface_in_use = PS_IFACE_IS_IN_USE(&(ds_v6_iface_ptr->ps_iface));

      v4_iface_throttled = FALSE;
      v6_iface_throttled = FALSE;

      v4_iface_throttled = v4_iface_in_use 
                           && ds_3gpp_pdn_cntxt_is_apn_throttled(pdn_cntx_p, 
                                                    &(ds_v4_iface_ptr->ps_iface));
      v6_iface_throttled = v6_iface_in_use
                           && ds_3gpp_pdn_cntxt_is_apn_throttled(pdn_cntx_p, 
                                                    &(ds_v6_iface_ptr->ps_iface));

      redial_v4 = v4_iface_in_use && !v4_iface_throttled;
      redial_v6 = v6_iface_in_use && !v6_iface_throttled;
    
      if (redial_v4 && redial_v6)
      {
         DS_3GPP_MSG0_HIGH("ds_3gpp_pdn_cntxt_redial_hdlr: Dissociate ifaces");

        /*---------------------------------------------------------------------- 
          We use the recorded subs id from the old PDN Context
        -----------------------------------------------------------------------*/
        new_pdn_cntx_p = ds_pdn_cntxt_allocate_new_pdn_context(subs_id);

         if(!ds_3gpp_pdn_cntx_validate_pdn_context(new_pdn_cntx_p))
         {
           break;
         }

         ds_3gpp_pdn_cntxt_setup_pdn_with_ds_iface 
           (new_pdn_cntx_p, &(pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile),
            ds_v6_iface_ptr, DS_3GPP_PDN_CONTEXT_IPV6_IFACE,
            pdn_cntx_p);

         /*----------------------------------------------------------------- 
           Dissociate v6 ds iface from the old pdncontext
         ------------------------------------------------------------*/
         pdn_cntx_p->ds_pdn_context_dyn_p->
           ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE]= NULL;

         /*------------------------------------------------------------------ 
           Update PDP type
          ------------------------------------------------------------------*/
         pdn_cntx_p->ds_pdn_context_dyn_p->
           pdp_profile.context.pdp_type = DS_UMTS_PDP_IPV4;
                   
         /*----------------------------------------------------------------- 
           Update PDN state
         ---------------------------------------------------------------*/
         pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_COMING_UP;
         new_pdn_cntx_p->ds_pdn_context_dyn_p->state = 
           DS_PDN_CONTEXT_STATE_COMING_UP;

         /*----------------------------------------------------------------- 
           Post for pdn_cntx_p
         ---------------------------------------------------------------*/
         cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_pdn_context_s*));
         if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
         {
           ASSERT(0);
           return;
         }

         cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_CONN_REQ_RESEND;
         data_ptr = (ds_pdn_context_s**)cmd_ptr->cmd_payload_ptr;
         *data_ptr = pdn_cntx_p;

         ds_put_cmd_ext(cmd_ptr);

         /*----------------------------------------------------------------- 
           Post for new_pdn_cntx_p
         ---------------------------------------------------------------*/
         cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_pdn_context_s*));
         if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
         {
           ASSERT(0);
           return;
         }

         cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_CONN_REQ_RESEND;
         data_ptr = (ds_pdn_context_s**)cmd_ptr->cmd_payload_ptr;
         *data_ptr = new_pdn_cntx_p;

         ds_put_cmd_ext(cmd_ptr);

         redial_not_performed = FALSE;

      }
      else if(redial_v4)
      {
         /*---------------------------------------------------------- 
            Tear down v6 iface
            ---------------------------------------------------------*/
         DS_3GPP_MSG0_HIGH("ds_3gpp_pdn_cntxt_redial_hdlr: Tearing down V6 iface");
         ds_3gpp_pdn_cntxt_tearDownDSIface(ds_v6_iface_ptr, 
                                DS_3GPP_PS_IFACE_NET_DOWN_REASON(down_reason));
         pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type = DS_UMTS_PDP_IPV4;
         (void)ds_pdn_cntxt_update_apn_table(pdn_cntx_p->ds_pdn_context_dyn_p->
                                             pdp_profile.context.apn,
                                            DS_IPV4_SINGLE_BEARER, pdn_cntx_p); 

         cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_pdn_context_s*));
         if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
         {
           ASSERT(0);
           return;
         }

         cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_CONN_REQ_RESEND;
         data_ptr = (ds_pdn_context_s**)cmd_ptr->cmd_payload_ptr;
         *data_ptr = pdn_cntx_p;

         ds_put_cmd_ext(cmd_ptr);

         redial_not_performed = FALSE;
      }
      else if(redial_v6)
      {
        /*------------------------------------------------------------------
           Tear down v4 iface
           ----------------------------------------------------------------*/
        DS_3GPP_MSG0_HIGH("ds_3gpp_pdn_cntxt_redial_hdlr: Tearing down V4 iface");
        ds_3gpp_pdn_cntxt_tearDownDSIface(ds_v4_iface_ptr, 
                                DS_3GPP_PS_IFACE_NET_DOWN_REASON(down_reason));
        pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type = DS_UMTS_PDP_IPV6;
         (void)ds_pdn_cntxt_update_apn_table(pdn_cntx_p->ds_pdn_context_dyn_p->
                                             pdp_profile.context.apn,
                                            DS_IPV6_SINGLE_BEARER, pdn_cntx_p);

         cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_pdn_context_s*));
         if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
         {
           ASSERT(0);
           return;
         }

         cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_CONN_REQ_RESEND;
         data_ptr = (ds_pdn_context_s**)cmd_ptr->cmd_payload_ptr;
         *data_ptr = pdn_cntx_p;

         ds_put_cmd_ext(cmd_ptr);

         redial_not_performed = FALSE;
      } 
      break;

  case  DS_3GPP_REDIAL_UMTS_LTE_IRAT:
    DS_3GPP_MSG3_HIGH("ds_3gpp_pdn_cntxt_setup_redial: Redial beacuse of"
                      "UMTS to LTE IRAT. call id:%d Bearer Inst:%d",
                       bearer_cntx_p->ds_bearer_context_dyn_p->cm_call_id, 
                      (uint32)bearer_cntx_p->ds_bearer_context_dyn_p->client_data_ptr,0);
     
    DS_3GPP_MSG0_HIGH("Starting Pending Redial Timer for 2 mins");
    (void)rex_set_timer(&(per_subs_info_hndl->pending_redial_tmr), 
                         (PENDING_REDIAL_TIMER_VALUE*60000));  
	  
    per_subs_info_hndl->pending_redial_tmr_running = TRUE;
	  
    /* Nothing to do. Redial is performed in LTE mode during attach */
    redial_not_performed = FALSE;
    break;
  
  case DS_3GPP_REDIAL_AUTH_FALLBACK:
      DS_3GPP_MSG3_HIGH("ds_3gpp_pdn_cntxt_redial_hdlr: Redial beacuse of"
                      "Auth fallback. call id:%d Bearer Inst:%d",
                       bearer_cntx_p->ds_bearer_context_dyn_p->cm_call_id, 
                      (uint32)bearer_cntx_p->ds_bearer_context_dyn_p->client_data_ptr,0);

      ds_v4_iface_ptr = DSPDNCNTXT_GET_V4_DS_IFACE_PTR(pdn_cntx_p);
      ds_v6_iface_ptr = DSPDNCNTXT_GET_V6_DS_IFACE_PTR(pdn_cntx_p);    

      if (((NULL != ds_v6_iface_ptr) &&
                 PS_IFACE_IS_IN_USE(&(ds_v6_iface_ptr->ps_iface))) ||
           ((NULL != ds_v4_iface_ptr) &&
                 PS_IFACE_IS_IN_USE(&(ds_v4_iface_ptr->ps_iface))))
      {
        #ifdef FEATURE_DATA_LTE          
        if(bearer_cntx_p->ds_bearer_context_dyn_p->call_mode != SYS_SYS_MODE_LTE       
         || ds_eps_attach_sm_is_ue_attached(subs_id))
        #endif
        {
          /*------------------------------------------------------------------ 
            In all scenarios other than LTE attach, we redial explicitly.
            ------------------------------------------------------------------*/
          redial_not_performed = FALSE;

          cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_pdn_context_s*));
          if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
          {
            ASSERT(0);
            return;
          }

          cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_CONN_REQ_RESEND;
          data_ptr = (ds_pdn_context_s**)cmd_ptr->cmd_payload_ptr;
          *data_ptr = pdn_cntx_p;

          ds_put_cmd_ext(cmd_ptr);
        }
      }
      break;
  case DS_3GPP_NO_REDIAL:  
      DS_3GPP_MSG2_HIGH("ds_3gpp_pdn_cntxt_redial_hdlr: No Redial"
                      "call id:%d Bearer Inst:%d",
                       bearer_cntx_p->ds_bearer_context_dyn_p->cm_call_id, 
                      (uint32)bearer_cntx_p->ds_bearer_context_dyn_p->client_data_ptr);   
      break;

   default:
    break;

  }

  if(redial_not_performed)
  {
    bearer_cntx_p->ds_bearer_context_dyn_p->state = DS_BEARER_CONTEXT_STATE_DOWN;
    DS_3GPP_MSG3_HIGH("ds_3gpp_pdn_cntxt_redial_hdlr: Invoking go_null"
                      "call id:%d Bearer Inst:%d",
                       bearer_cntx_p->ds_bearer_context_dyn_p->cm_call_id, 
                      (uint32)bearer_cntx_p->ds_bearer_context_dyn_p->client_data_ptr,0);

    ds_3gpp_bearer_cntxt_go_null(bearer_cntx_p, 
                                down_reason, call_was_connected);
  }
  else
  {
#ifdef FEATURE_DATA_LTE  
    /*-------------------------------------------------------------------------
      Since we are performing PDN redial we need to reset APN Switchng payload
      to ensure in case of PDN redial reject we propagate consistent info
      to both APM and AP
    -------------------------------------------------------------------------*/

    DS_3GPP_MSG0_HIGH("Performing Redial so re-setting APN Switching Payload");

    ds_3gpp_apn_switch_mgr_reset_switching_action_payload(subs_id);
#endif /* FEATURE_DATA_LTE */    
  }
}
