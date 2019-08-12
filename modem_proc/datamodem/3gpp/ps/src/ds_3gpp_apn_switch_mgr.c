/*===========================================================================
                     DS_3GPP_APN_SWITCH_MGR. C

DESCRIPTION


EXTERNALIZED FUNCTIONS


 Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_apn_switch_mgr.c#1 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/27/15    pvb     Created Module 
 
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"
#include "assert.h"
#include "ds_profile.h"

#include "queue.h"
#include "dsutil.h"
#include "modem_mem.h"
#include "ds_3gpp_apn_switch_mgr.h"
#include "ds_3gpp_nv_manager.h"

#ifdef FEATURE_DATA_LTE
#include "ds3gsubsmgr.h"
#include "ds3gmgr.h"
#endif /* FEATURE_DATA_LTE */

#include <stringl/stringl.h>

#define DS_3GPP_APN_SWITCH_MGR_SERVICES_NOT_ALLOWED_CAUSE 7

#ifdef FEATURE_DATA_LTE
/*===========================================================================

            GLOBAL VARIABLE DEFINATIONS

===========================================================================*/
ds_3gpp_apn_switch_mgr_payload_info_type    ds_3gpp_switching_action_payload
                                                 [DS3GSUBSMGR_SUBS_ID_COUNT];
/*===========================================================================

            FUNCTION DEFINATIONS

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
)
{
  uint16                           int_cause = 0;
  cm_lte_call_info_s_type         *lte_call_info_ptr = NULL;
  cm_gw_ps_call_info_s_type       *gw_call_info_ptr = NULL;
  boolean                          switching_action_decided = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  do
  {
    if(apn_switch_is_needed_p == NULL)
    {
      DS_LTE_MSG0_ERROR ("Invalid APN switch flag passed ");
      break;
    }

    if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
    {
      break;
    }

    if(call_info_ptr == NULL)
    {
      DS_LTE_MSG0_ERROR("Input Params are not valid");
      break;
    }
    /*----------------------------------------------------------------------- 
     In this function we take the following course of action:
     1. If SYS mode is LTE we check to see if APN Switch on Reject NV is set,
          in that case we perform APN Switch on reject for:
          a. ESM cause codes only
          b. EMM cause codes when cause type is not IRAT
     
        We also set the value of flag, switching_action_needed to imply no
        more code needs to be exercised to decide whether APN switch is needed
     
     2. If the above NV is not set, then we perform the cause code based checks
        which are applicable to both LTE & WCDMA/GSM
     
        For cause code based switching we switch for all ESM Cause codes for
        LTE and all NETWORK_CAUSES for UMTS
        In addition to that we perform APN switch for ESM Local Cause 7
        (EPS_SERVICES_NOT_ALLOWED_LOCAL) and SM_INTERNAL_CAUSE 22
        (GPRS_SERVICE_NOT_ALLOWED) for LTE and UMTS respectively.
     
     3. We set the local_variable "int_cause" to the value returned by lower
        layers depending on step2 and compare it will Rule block entry for the
        detached/rejected APN's profile which says whether APN Switch is needed
        for the given cause or not.
    -----------------------------------------------------------------------*/ 

    *apn_switch_is_needed_p = FALSE;

    if(call_mode == SYS_SYS_MODE_LTE)
    {
      lte_call_info_ptr = &(call_info_ptr->lte_call);

      DS_LTE_MSG3_HIGH("ESM Local is valid: %d, EMM Failure Cause: %d,"
                       "ESM is valid: %d ", 
                       lte_call_info_ptr->esm_local_cause.valid,
                       lte_call_info_ptr->emm_failure_cause.cause_type,
                       lte_call_info_ptr->esm_cause.valid);

      if(ds_3gpp_nv_manager_get_switch_apn_on_reject(subs_id))
      {
        /*----------------------------------------------------------------------- 
         For DCM we have a special requirement saying when T3402 timer expires
         NAS sends local cause code saying ATTACH_FAILED, in that case we need
         to switch APN and acquire PS services on next available one
        -----------------------------------------------------------------------*/ 
        if((ds_3gpp_nv_manager_get_global_throttling(subs_id) == TRUE) &&
           (lte_call_info_ptr->esm_local_cause.valid == TRUE &&
           lte_call_info_ptr->esm_local_cause.local_cause == EMM_ATTACH_FAILED))
        {
          *apn_switch_is_needed_p = TRUE;
        }
        else
        {
          if (lte_call_info_ptr->esm_local_cause.valid == TRUE ||
              lte_call_info_ptr->emm_failure_cause.cause_type != 
                                                             LTE_NAS_IRAT_NONE)
          {
            *apn_switch_is_needed_p = FALSE;
          }
          else if (lte_call_info_ptr->esm_cause.valid == TRUE &&
                   lte_call_info_ptr->esm_cause.esm_cause == 
                                                NAS_ESM_PDN_CONN_DOES_NOT_EXIST)
          {
            DS_LTE_MSG1_HIGH("ESM Cause: %d", 
                             lte_call_info_ptr->esm_cause.esm_cause);
            *apn_switch_is_needed_p = FALSE;
          }
          else
          {
            *apn_switch_is_needed_p = TRUE;
          }
        }
        switching_action_decided = TRUE;
      }
      else
      {
        /*----------------------------------------------------------------------- 
         We perform a comparison for ESM Local cause and if its cause 7 we store
         it to compare with APM Rule block entry for the given APN
         
         The same comparison is performed for all ESM cause codes
        -----------------------------------------------------------------------*/ 
        if(lte_call_info_ptr->esm_local_cause.valid == TRUE)
        {
          if(lte_call_info_ptr->esm_local_cause.local_cause == 
                                        LTE_NAS_EPS_SERVICES_NOT_ALLOWED_LOCAL)
          {
            int_cause = lte_call_info_ptr->esm_local_cause.local_cause;
          }
          else
          {
            DS_3GPP_MSG1_HIGH("LTE Internal Cause provided by lower layer: %d "
                              "No APN Switching",
                              lte_call_info_ptr->esm_local_cause.local_cause);
            switching_action_decided = TRUE;
          }
        }
        else if(lte_call_info_ptr->esm_cause.valid == TRUE)
        {
          int_cause = lte_call_info_ptr->esm_cause.esm_cause;
        }
        else
        {
          switching_action_decided = TRUE;
        }
      }
    }
    else if (call_mode == SYS_SYS_MODE_WCDMA ||
             call_mode == SYS_SYS_MODE_GSM ||
             call_mode == SYS_SYS_MODE_TDS)
    {
      /*----------------------------------------------------------------------- 
       We perform a comparison for SM Internal Cause and if its cause 7 we store
       it to compare with APM Rule block entry for the given APN
       
       The same comparison is performed for all SM Network causes
      -----------------------------------------------------------------------*/ 
      gw_call_info_ptr = &(call_info_ptr->gw_ps_call);
      if(gw_call_info_ptr->pdp_cause_type == SM_INTERNAL_CAUSE)
      {
        if(gw_call_info_ptr->cause.int_cause == SM_GPRS_SERVICES_NOT_ALLOWED)
        {
          int_cause = DS_3GPP_APN_SWITCH_MGR_SERVICES_NOT_ALLOWED_CAUSE;
        }
        else
        {
          DS_3GPP_MSG1_HIGH("UMTS Internal Cause provided by lower layer: %d "
                            "No APN Switching",
                            gw_call_info_ptr->cause.int_cause);
          switching_action_decided = TRUE;
        }
      }
      else if(gw_call_info_ptr->pdp_cause_type == SM_NETWORK_CAUSE)
      {
        int_cause = gw_call_info_ptr->cause.ext_cause;
      }
      else
      {
        DS_3GPP_MSG1_ERROR("Invalid Cause type: %d",
                           gw_call_info_ptr->pdp_cause_type);
        switching_action_decided = TRUE;
      }
    }

    if(switching_action_decided == FALSE)
    {
      /*----------------------------------------------------------------------- 
       Once we know the cause code send by the Network, we see if an entry
       for that cause code is present in rule block for the APN, if it is
       we indicate saying APN Switch is needed.
      -----------------------------------------------------------------------*/ 
      if(ds_dsd_apm_match_cause_codes(profile_id,
                                      subs_id,
                                      int_cause,
                                      cause_type))
      {
        *apn_switch_is_needed_p = TRUE;
      }
      else
      {
        *apn_switch_is_needed_p = FALSE;
        DS_3GPP_MSG1_HIGH("No APN Switch needed for Cause Code: %d",
                          int_cause);
      }   
    }
    
    DS_3GPP_MSG1_HIGH("APN Switch is needed: %d", *apn_switch_is_needed_p);
  }while (0);  
}

/*===========================================================================
FUNCTION       DS_3GPP_APN_SWITCH_MGR_GET_SWITCHING_ACTION_PAYLOAD

DESCRIPTION    This function gets the switching action info payload per subs
 
PARAMETERS     subs_id: Currently active subscription for Packet services 
 
DEPENDENCIES   NONE

RETURN VALUE   switching_action_payload: Switching action related info per subs

SIDE EFFECTS   NONE
===========================================================================*/
ds_3gpp_apn_switch_mgr_payload_info_type * 
                        ds_3gpp_apn_switch_mgr_get_switching_action_payload
(
  sys_modem_as_id_e_type             subs_id
)
{
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return NULL;
  }

  return &ds_3gpp_switching_action_payload[subs_id];
}

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
)
{
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  memset(&(ds_3gpp_switching_action_payload[subs_id]), 0, 
         sizeof(ds_3gpp_apn_switch_mgr_payload_info_type));
}

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
)
{
  ds_dsd_ext_error_code              apm_ind_err_code = DS_DSD_APM_UNSEPCIFIED;
  ds3gsubsmgr_subs_id_e_type         subs_index = DS3GSUBSMGR_SUBS_ID_MIN;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  subs_index = ds3gsubsmgr_subs_id_cm_to_ds3g( subs_id );
  if(!ds3gsubsmgr_is_ds3g_subs_index_valid(subs_index))
  {
    return apm_ind_err_code;
  }

  if(ds_3gpp_switching_action_payload[subs_id].payload_valid == TRUE)
  {
    apm_ind_err_code = ds_dsd_apm_ind(
                          ds_3gpp_switching_action_payload[subs_id].apm_ind, 
                &ds_3gpp_switching_action_payload[subs_id].apm_ind_payload);
  }

  /*-------------------------------------------------------------------
    Reset the Switching Action payload for next iteration
  --------------------------------------------------------------------*/
  ds_3gpp_apn_switch_mgr_reset_switching_action_payload(subs_id);

  return apm_ind_err_code;
}

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
)
{
  boolean                                     apn_switch_is_needed = FALSE;
  ds_3gpp_apn_switch_mgr_payload_info_type   *local_switch_payload_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  do
  {
    if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
    {
      break;
    }

    if(call_info_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("Input Params are not valid");
      break;
    }
    /*-----------------------------------------------------------------
      We extract the Switching action payload info and see if its valid
     
      if its not we store the APN switching indication & payload info
      which is sent to APN when PDN is cleaned-up
     
      We decide whether to send a REJECT ind or DETACH Ind based on
      PDN state, if the PDN is coming up we consider it as PDP reject

      If the PDN is up, then we consider it as a PDP Deactivate
     
      Then we invoke the function to see if APN Switch is needed or not
      and accordingly fill the APN Switching action payload
    ------------------------------------------------------------------*/
    if(bearer_call_mode == SYS_SYS_MODE_WCDMA ||
       bearer_call_mode == SYS_SYS_MODE_GSM ||
       bearer_call_mode == SYS_SYS_MODE_TDS)
    {
      if(pdn_state == DS_PDN_CONTEXT_STATE_UP)
      {
        cause_type = DS_DSD_APM_DETACH_NETWORK_CAUSE;
      }
      else if(pdn_state == DS_PDN_CONTEXT_STATE_COMING_UP)
      {
        cause_type = DS_DSD_APM_REJECT_NETWORK_CAUSE;
      }
    }

    local_switch_payload_ptr = 
      ds_3gpp_apn_switch_mgr_get_switching_action_payload(subs_id);

    if(local_switch_payload_ptr != NULL &&
       local_switch_payload_ptr->payload_valid == FALSE)
    {
      ds_3gpp_apn_switch_mgr_check_if_apn_switch_needed(call_info_ptr,
                                                        &apn_switch_is_needed,
                                                        subs_id,
                                                        profile_id,
                                                        bearer_call_mode,
                                                        cause_type);

      memset(local_switch_payload_ptr, 0, 
             sizeof(ds_3gpp_apn_switch_mgr_payload_info_type));

      if(cause_type == DS_DSD_APM_REJECT_NETWORK_CAUSE)
      {
        local_switch_payload_ptr->apm_ind = DS_DSD_APM_ATTACH_REJECT_IND;
        DS_3GPP_APM_SET_PROFILE_ID(local_switch_payload_ptr->apm_ind_payload,
                           profile_id);
      }
      else if(cause_type == DS_DSD_APM_DETACH_NETWORK_CAUSE)
      {
        local_switch_payload_ptr->apm_ind = DS_DSD_APM_DETACH_IND;
      }
      else
      {
        DS_3GPP_MSG1_ERROR("Invalid Cause type: %d, Breaking OUT !!!!", 
                           cause_type);
        break;
      }

      DS_3GPP_APM_SET_SWITCH_APN_FLAG(local_switch_payload_ptr->apm_ind_payload,
                                      apn_switch_is_needed, subs_id);
      local_switch_payload_ptr->payload_valid = TRUE;

      DS_3GPP_MSG2_MED("%d Indication will be sent to APM for Cause: %d",
                       local_switch_payload_ptr->apm_ind, cause_type);
    }
    else
    {
      DS_3GPP_MSG1_HIGH("APN Switch Action Payload is invalid or "
                        "already set: 0x%x",local_switch_payload_ptr);
    }
  }while(0);
}

#endif /*FEATURE_DATA_LTE*/
