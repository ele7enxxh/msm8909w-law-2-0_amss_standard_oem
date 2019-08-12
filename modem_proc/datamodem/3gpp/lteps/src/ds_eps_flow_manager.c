/*!
  @file
  ds_eps_flow_manager.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/lteps/src/ds_eps_flow_manager.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/08/11   rr      Added support to update p-cscf address from NW bearer modification 
                   indiacation
04/03/10   vk      Added support for MSG2.0 based logging
02/10/10   vd      Replaced lte_nas_esm_message_description.h with cm_gw.h
02/04/10   vd      Changed function naming format.
01/27/10   vd      Using the changed flow manager function names.
01/23/10   vk      Added helper macros for virtual function dispatch
01/20/10   vd      Changes to support UE Qos modification.
07/20/09   vd      Initial version.
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_LTE
#include "dstask.h"
#include "ds3gmgr.h"
#include "ds_eps_flow_manager.h"
#include "ds_3gpp_flow_manager.h"
#include "ds_3gpp_bearer_context.h"
#include "ds_3gpp_nv_manager.h"
#include "ds_eps_bearer_context.h"
#include "ds_eps_bearer_flow_manager.h"
#include "ds_3gpp_bearer_flow_manager.h"
#include "ds_3gppi_utils.h"
#include "ds3gmshif.h"
#include "dsumtspdpreg.h"
#include "ds_flow_control.h"
#include "ran.h"
#include "cm.h"
#include "cm_gw.h"
#include "ps_iface_flow.h"
#include "ps_flow.h"
#include "ds_3gpp_dyn_mem_hdlr.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_3gpp_low_prio_sig_hdlr.h"
/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*---------------------------------------------------------------------------
  EPS Flow manager table.
---------------------------------------------------------------------------*/
ds_eps_flow_manager_s ds_eps_flow_manager_tbl[DS_3GPP_MAX_PDN_CONTEXT] = {{NULL}};


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                                FUNCTIONS

===========================================================================*/


/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_CHECK_NW_INPUT

DESCRIPTION
  Handle the bearer allocation failure.

PARAMETERS
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  Returns True if the NW params are valid.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_flow_manager_check_nw_input
(
  cm_call_mode_info_s_type* mode_info_ptr,
  boolean                   default_bearer,
  boolean                   bearer_alloc
)
{
   cm_lte_call_info_s_type       *lte_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_LTE_MSG0_LOW("EPS Flow manager received Check NW Input");

  if(mode_info_ptr == NULL)
  {
    DS_LTE_MSG0_ERROR("Input parameter is NULL");
    return DS3G_SUCCESS;
  }

  /*-------------------------------------------------------------------------
    get the LTE info 
  -------------------------------------------------------------------------*/
  lte_info_ptr =(cm_lte_call_info_s_type *)&(mode_info_ptr->info.lte_call);

  /*-------------------------------------------------------------------------
    Verify FLOW Fields 
  -------------------------------------------------------------------------*/
  if(lte_info_ptr->sdf_qos.valid != TRUE)
  {
    DS_LTE_MSG0_ERROR("Received qos with valid flag set to FALSE");
    lte_info_ptr->esm_cause.valid = TRUE;
    lte_info_ptr->esm_cause.esm_cause =NAS_ESM_INVALID_MANDATORY_INFORMATION;
    return DS3G_FAILURE;
  }

  if(default_bearer == TRUE)
  {
    return DS3G_SUCCESS;
  } 
  /*-------------------------------------------------------------------------
    Verify Filter Fields 
  -------------------------------------------------------------------------*/
  if(lte_info_ptr->ul_dl_tft.valid != TRUE)
  {
    DS_LTE_MSG0_ERROR("Received TFT type with valid flag set to FALSE");
    lte_info_ptr->esm_cause.valid = TRUE;
    lte_info_ptr->esm_cause.esm_cause =NAS_ESM_INVALID_MANDATORY_INFORMATION;
    return DS3G_FAILURE;
  }

  if((lte_info_ptr->ul_dl_tft.tft_total_length < 3) ||
	  (lte_info_ptr->ul_dl_tft.tft_total_length > 257))
  {
    DS_LTE_MSG1_ERROR("Received TFT with invalid length=%d",
		      lte_info_ptr->ul_dl_tft.tft_total_length);
    lte_info_ptr->esm_cause.valid = TRUE;
    lte_info_ptr->esm_cause.esm_cause =
                                NAS_ESM_SYNTACTICAL_ERR_IN_TFT_OPERATION;
    return DS3G_FAILURE;
  }

  if(bearer_alloc  == TRUE)
  {
    /*-------------------------------------------------------------------------
      Verify fields for new bearer allocation
    -------------------------------------------------------------------------*/ 
    if(lte_info_ptr->ul_dl_tft.operation_code!=(byte)DS_3GPP_TFT_OPCODE_CREATE ||
       lte_info_ptr->ul_dl_tft.e_bit != 0)
    {
      DS_LTE_MSG1_ERROR("Received TFT with invalid operation:%d or with E bit set",
		        lte_info_ptr->ul_dl_tft.operation_code);
      lte_info_ptr->esm_cause.valid = TRUE;
      lte_info_ptr->esm_cause.esm_cause =
                                  NAS_ESM_SYNTACTICAL_ERR_IN_TFT_OPERATION;
      return DS3G_FAILURE;
    }
  } /* bearer creation */
  else
  {
    /*-------------------------------------------------------------------------
      Verify fields for bearer modification
    -------------------------------------------------------------------------*/ 
    if(lte_info_ptr->ul_dl_tft.operation_code==(byte)DS_3GPP_TFT_OPCODE_CREATE)
    {
      DS_LTE_MSG1_ERROR("Received TFT with invalid operation:%d",
		        lte_info_ptr->ul_dl_tft.operation_code);
      lte_info_ptr->esm_cause.valid = TRUE;
      lte_info_ptr->esm_cause.esm_cause =
                                   NAS_ESM_SYNTACTICAL_ERR_IN_TFT_OPERATION;
      return DS3G_FAILURE;
    }
  } /* Else bearer modification*/

  if((lte_info_ptr->ul_dl_tft.num_filters > DS_TFT_MAX_FILTERS)||
	  (lte_info_ptr->ul_dl_tft.num_filters < 1))
  {
    DS_LTE_MSG1_ERROR("Received Invalid number of filters:%d",
		      lte_info_ptr->ul_dl_tft.num_filters);
    lte_info_ptr->esm_cause.valid = TRUE;
    lte_info_ptr->esm_cause.esm_cause =NAS_ESM_SYNTACTICAL_ERR_IN_TFT_OPERATION;
    return DS3G_FAILURE;
  }
  return DS3G_SUCCESS;
}/* ds_eps_flow_manager_check_nw_input*/

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_CONVERT_CAUSE_CODE

DESCRIPTION
  Convert the generic cause codes to EPS specific cause code.

PARAMETERS
  mode_info_ptr      -  CM passed mode information.
  ds_int_cause_ptr   -  DS generic cause  

DEPENDENCIES
  None.

RETURN VALUE
  Returns True if the cause code conversion is successful.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_flow_manager_convert_cause_code
(
  const cm_call_mode_info_s_type *mode_info_ptr,
  ds_3gpp_internal_cause_code_enum_type *ds_int_cause_ptr
)
{
   cm_lte_call_info_s_type       *lte_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((mode_info_ptr == NULL) || (ds_int_cause_ptr == NULL))
  {
    DS_LTE_MSG0_ERROR("input params are not NULL");
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    get the LTE info 
  -------------------------------------------------------------------------*/
  lte_info_ptr =(cm_lte_call_info_s_type *)&(mode_info_ptr->info.lte_call);
 
  switch(*ds_int_cause_ptr)
  {
    case DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES:
      lte_info_ptr->esm_cause.valid = TRUE;
      lte_info_ptr->esm_cause.esm_cause =NAS_ESM_INSUFFICIENT_RESOURCES;
    break;

    case DS_3GPP_INTERNAL_FEATURE_NOT_SUPPORTED:
     lte_info_ptr->esm_cause.valid = TRUE;
     lte_info_ptr->esm_cause.esm_cause =NAS_ESM_FEATURE_NOT_SUPPORTED;
    break;

    case DS_3GPP_INTERNAL_QOS_NOT_ACCEPTED :
      lte_info_ptr->esm_cause.valid = TRUE;
      lte_info_ptr->esm_cause.esm_cause =NAS_ESM_EPS_QOS_NOT_ACCEPTED;
    break;

    case DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_TFT:
     lte_info_ptr->esm_cause.valid = TRUE;
     lte_info_ptr->esm_cause.esm_cause =NAS_ESM_SEMANTIC_ERR_IN_TFT_OPERATION;
    break;

    case DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT:
      lte_info_ptr->esm_cause.valid = TRUE;
      lte_info_ptr->esm_cause.esm_cause =NAS_ESM_SYNTACTICAL_ERR_IN_TFT_OPERATION;
    break;

    case DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER:
     lte_info_ptr->esm_cause.valid = TRUE;
     lte_info_ptr->esm_cause.esm_cause =NAS_ESM_SYMANTIC_ERR_IN_PKT_FILTER;
    break;

    case DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER:
      lte_info_ptr->esm_cause.valid = TRUE;
      lte_info_ptr->esm_cause.esm_cause =NAS_ESM_SYNTACTICAL_ERR_IN_PKT_FILTER;
    break;

    case DS_3GPP_INTERNAL_BEARER_WITH_NO_TFT_ALREADY_PRESENT:
     lte_info_ptr->esm_cause.valid = TRUE;
     lte_info_ptr->esm_cause.esm_cause =NAS_ESM_EPS_BEARER_CONT_WO_TFT_ALREADY_ACT;
    break;

    case DS_3GPP_INTERNAL_INVALID_MANDATORY_INFO:
     lte_info_ptr->esm_cause.valid = TRUE;
     lte_info_ptr->esm_cause.esm_cause =NAS_ESM_INVALID_MANDATORY_INFORMATION;
    break;

    case DS_3GPP_INTERNAL_ACTIVATION_REJECTED:
     lte_info_ptr->esm_cause.valid = TRUE;
     lte_info_ptr->esm_cause.esm_cause =NAS_ESM_ACTIVATION_REJECTED;
    break;

    case DS_3GPP_INTERNAL_SERV_OPT_NOT_SUPPORTED:
     lte_info_ptr->esm_cause.valid = TRUE;
     lte_info_ptr->esm_cause.esm_cause =NAS_ESM_SERV_OPT_NOT_SUPPORTED;
    break;
    default:
      DS_LTE_MSG1_ERROR("Invalid cause code passed 0x%x",*ds_int_cause_ptr);
      return FALSE;
  }/* switch*/
  return TRUE;
}/* ds_eps_flow_manager_convert_cause_code*/

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_UPDATE_APN_AMBR

DESCRIPTION
  Update the APN AMBR with the network request.

PARAMETERS
  mode_info_ptr   -  CM passed mode information.
  pdn_context_p   -  PDN context pointer

DEPENDENCIES
  None.

RETURN VALUE
  Returns True if successfully updated.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_flow_manager_update_apn_ambr
(
  cm_lte_call_info_s_type  *lte_info_ptr,
  ds_pdn_context_s         *pdn_context_p
)
{
  ds_eps_flow_manager_s        *eps_flow_manager_p = NULL;
  ds_flow_manager_s            *flow_manager_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (lte_info_ptr == NULL) || (pdn_context_p == NULL) )
  {
    DS_LTE_MSG0_ERROR("Input parameter is NULL");
    return FALSE;
  }

  /*--------------------------------------------------------------------
    Store the APN AMBR in Mode specific context
  ---------------------------------------------------------------------*/
  flow_manager_p=ds_flow_manager_find_flow_manager(pdn_context_p);

  if(!ds_flow_manager_validate_flow_manager(flow_manager_p))
  {
    lte_info_ptr->esm_cause.valid = TRUE;
    lte_info_ptr->esm_cause.esm_cause =NAS_ESM_INSUFFICIENT_RESOURCES;
    return FALSE;
  }

  eps_flow_manager_p = 
   (ds_eps_flow_manager_s *)(flow_manager_p->ds_flow_manager_dyn_p->
                             vobj_data_ptr_tbl[DS_3GPP_EPS_VOBJ]);
  if(!ds_eps_flow_manager_validate_flow_manager(eps_flow_manager_p))
  {
    DS_LTE_MSG0_ERROR("eps_bearer_context_p is NULL");
    lte_info_ptr->esm_cause.valid = TRUE;
    lte_info_ptr->esm_cause.esm_cause =NAS_ESM_INSUFFICIENT_RESOURCES;
    return FALSE;
  }

  memscpy((void *)&(eps_flow_manager_p->eps_flow_manager_dyn_p->eps_pdn_ambr),
  	   sizeof(apn_ambr_T),
           (void *)&(lte_info_ptr->apn_ambr), sizeof(apn_ambr_T));

  return TRUE;
}
/* ds_eps_flow_manager_update_apn_ambr*/

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_TEAR_DOWN_SEC_BEARER

DESCRIPTION
  Handle secondary bearer tear down

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_flow_manager_tear_down_sec_bearer
(
  ds_flow_context_s   *flow_context_p
)
{
  ds_cmd_type                       *cmd_ptr = NULL;
  ds_bearer_flow_manager_s          *bflow_manager_p = NULL;
  ds_flow_manager_qos_cmd_info_type *qos_cmd_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check the flow context state.and call appropriate handler function. 
  -------------------------------------------------------------------------*/
  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return FALSE;
  }

  if(flow_context_p->ds_flow_context_dyn_p->state 
     == DS_FLOW_CONTEXT_STATE_PEND_ALLOC_RSP)
  {
    /*-----------------------------------------------------------------------
      Post Flow go null command
    -----------------------------------------------------------------------*/
    if (ds_flow_context_ps_flow_go_null_cmd(flow_context_p) == -1 )
    {
      DS_LTE_MSG0_ERROR("Flow context go null command failed");
       return FALSE;
    }

    /*-----------------------------------------------------------------------
      Post Command to initiate bearer modification request.
    -----------------------------------------------------------------------*/
    cmd_ptr = 
      ds_allocate_cmd_buf(sizeof(ds_flow_manager_qos_cmd_info_type));
    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      return FALSE;
    }

    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_FLOW_INVOKE_LOWER_LAYER;
    qos_cmd_info_ptr = 
      (ds_flow_manager_qos_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

    qos_cmd_info_ptr->event_type = DS_3GPP_FLOW_BEARER_ALLOC_ABORT;
    qos_cmd_info_ptr->flow_context_p = (void*)flow_context_p;

    memset(&(qos_cmd_info_ptr->nw_tft),0,sizeof(tft_type_T)); 

    memset(&(qos_cmd_info_ptr->app_qos),0,sizeof(qos_spec_type));

    qos_cmd_info_ptr->pdn_context_p = NULL;
    qos_cmd_info_ptr->flow_state = 
      (int)flow_context_p->ds_flow_context_dyn_p->state;

    ds_put_cmd_ext(cmd_ptr);
  }/* DS_FLOW_CONTEXT_STATE_PEND_ALLOC_RSP */
  else
  {
    /*---------------------------------------------------------------------
      ESM has responded back to the initial requet. 
      Check for race condition before we return the confirmation to CM
    ----------------------------------------------------------------------*/
    bflow_manager_p = flow_context_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr;
    if(bflow_manager_p == NULL)
    {
      DS_LTE_MSG0_ERROR("Flow context does not have bearer flow manager");
      return FALSE;
    }

    /*-----------------------------------------------------------------------
      Post Flow go null command
    -----------------------------------------------------------------------*/
    if (ds_flow_context_ps_flow_go_null_cmd(flow_context_p) == -1 )
    {
      DS_LTE_MSG0_ERROR("Flow context go null command failed");
      return FALSE;
    }

    /*-----------------------------------------------------------------------
      Update the flow context state
    -----------------------------------------------------------------------*/ 
    ds_flow_context_set_flow_state(flow_context_p,
                                 DS_FLOW_CONTEXT_STATE_PEND_DEL_FLOW_RSP);

    /*-----------------------------------------------------------------------
      Post Command to initiate bearer modification request.
    -----------------------------------------------------------------------*/
    cmd_ptr = 
      ds_allocate_cmd_buf(sizeof(ds_flow_manager_qos_cmd_info_type));
    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      return FALSE;
    }

    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_FLOW_INVOKE_LOWER_LAYER;
    qos_cmd_info_ptr = 
      (ds_flow_manager_qos_cmd_info_type*)cmd_ptr->cmd_payload_ptr;


    qos_cmd_info_ptr->event_type = DS_3GPP_FLOW_BEARER_MODIFY;
    qos_cmd_info_ptr->flow_context_p = (void*)flow_context_p;

    memset(&(qos_cmd_info_ptr->nw_tft),0,sizeof(tft_type_T)); 

    memset(&(qos_cmd_info_ptr->app_qos), 0, sizeof(qos_spec_type));

    qos_cmd_info_ptr->pdn_context_p = NULL;
    qos_cmd_info_ptr->flow_state = 
      (int)flow_context_p->ds_flow_context_dyn_p->state;

    ds_put_cmd_ext(cmd_ptr);
  }/* else */

  DS_LTE_MSG1_LOW("Posted invoke low layer on flow context 0x:%x",
                    flow_context_p);
  return TRUE;
}

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_BEARER_ALLOC_REJ

DESCRIPTION
  Handle the bearer allocation rejection.

PARAMETERS
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_flow_manager_bearer_alloc_rej
(
  cm_call_mode_info_s_type* mode_info_ptr
)
{
   ds_pdn_context_s             *pdn_context_p = NULL;
   ds_flow_context_s             *flow_context_p = NULL;
   cm_lte_call_info_s_type       *lte_info_ptr = NULL;
#ifdef FEATURE_DATA_REL10
  ps_iface_net_down_reason_type down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
  byte                         *apn = NULL;
#endif /*FEATURE_DATA_REL10*/
  sys_modem_as_id_e_type         subs_id = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_LTE_MSG0_HIGH("EPS Flow manager received Bearer allocation reject");
  if(mode_info_ptr == NULL)
  {
    DS_LTE_MSG0_ERROR("Input parameter is NULL");
    return;
  }

  /*-------------------------------------------------------------------------
    get the LTE info 
  -------------------------------------------------------------------------*/
  lte_info_ptr =(cm_lte_call_info_s_type *)&(mode_info_ptr->info.lte_call);

  /*------------------------------------------------------------------------
    Check if SDF ID is valid.
  -------------------------------------------------------------------------*/
  if(lte_info_ptr->sdf_id != 0xFFFF)
  {
    /*---------------------------------------------------------------------
      Check if any pending request matches the SDF_ID.
    ---------------------------------------------------------------------*/
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    flow_context_p = ds_flow_manager_find_pending_flow_context(
                                  &pdn_context_p,lte_info_ptr->sdf_id);
    if(!ds_flow_context_validate_flow(flow_context_p))
    {
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect); 
      return; 
    }


    if (!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p, &subs_id))
    {
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return; 
    }
    /*---------------------------------------------------------------------
      Found the pending flow context. Remove the flow context.
    ---------------------------------------------------------------------*/
    if(flow_context_p->ds_flow_context_dyn_p->state != DS_FLOW_CONTEXT_STATE_PEND_ALLOC_RSP)
    {
      DS_LTE_MSG0_LOW("Received unexpected Allocation Rejection");
    }
    if(ds_flow_manager_update_precedence_bit_mask(
                      pdn_context_p,flow_context_p,FALSE,FALSE,TRUE)==FALSE)
    {
      DS_LTE_MSG0_LOW("Error in clearing precedence bit mask");
    }
    if(ds_flow_manager_remove_pending_flow_context(
                  pdn_context_p,flow_context_p) == FALSE)
    {
      DS_LTE_MSG0_LOW("Could not remove Pending Flow Context");
    }  
    ds_flow_context_release_flow_context(flow_context_p);
    /*---------------------------------------------------------------------
      If NW reason has to be sent to APP. Add here.
    --------------------------------------------------------------------*/
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

#ifdef FEATURE_DATA_REL10
   if(ds_3gpp_nv_manager_get_nv_lte_rel_10(subs_id))
   {
    /*Map the NW cause code to PS Iface cause code*/
    if (lte_info_ptr->esm_cause.valid)
    {
      ds_eps_bearer_cntxt_map_cause_codes(CALL_END_ESM_CAUSE_TO_PS_IFACE_CAUSE_CODES,
                                          &down_reason,
                                          (void *)mode_info_ptr);
    }
    else
    {
      down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
    }

    if( (down_reason == PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES) ||
        (down_reason == PS_NET_DOWN_REASON_UNKNOWN_APN) )
    {
      /*---------------------------------------------------------------- 
       Perform global APN throttling for these cause codes. Create a 
       PDN throt SM exclusively for globally throttling the given APN
       across all PLMNs.
      ------------------------------------------------------------------*/
      apn = pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn;
  
      if(*apn == '\0')
      {
        (void)ds_pdn_cntxt_get_resolved_apn_name(pdn_context_p, &apn);
      }

      DS_LTE_MSG2_HIGH("APN being throttled: %s, APN in PDN_ctxt: %s",
               (char*)apn,
               (char*)pdn_context_p->ds_pdn_context_dyn_p->
                       pdp_profile.context.apn);

      ds_3gpp_pdn_throt_lte_handle_reject_with_t3396_ie(lte_info_ptr, apn, 
                                                        down_reason, subs_id,
                                                        pdn_context_p);
    }
   }// get_nv_lte_rel_10()
#endif /*FEATURE_DATA_REL10*/
    return;
  } /* SDF_ID != 0xFFFF */ 
  DS_LTE_MSG1_LOW("Invalid SDF ID 0x%x",lte_info_ptr->sdf_id);
  return;
}/* ds_eps_flow_manager_bearer_alloc_rej */

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_BEARER_ALLOC_FAIL

DESCRIPTION
  Handle the bearer allocation failure.

PARAMETERS
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_flow_manager_bearer_alloc_fail
(
  cm_call_mode_info_s_type* mode_info_ptr
)
{
   ds_pdn_context_s             *pdn_context_p = NULL;
   ds_flow_context_s             *flow_context_p = NULL;
   cm_lte_call_info_s_type       *lte_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_LTE_MSG0_HIGH("EPS Flow manager received Bearer allocation failure");
  if(mode_info_ptr == NULL)
  {
    DS_LTE_MSG0_ERROR("Input parameter is NULL");
    return;
  }

  /*-------------------------------------------------------------------------
    get the LTE info 
  -------------------------------------------------------------------------*/
  lte_info_ptr =(cm_lte_call_info_s_type *)&(mode_info_ptr->info.lte_call);

  /*------------------------------------------------------------------------
    Check if SDF ID is valid.
  -------------------------------------------------------------------------*/
  if(lte_info_ptr->sdf_id != 0xFFFF)
  {
    /*---------------------------------------------------------------------
      Check if any pending request matches the SDF_ID.
    ---------------------------------------------------------------------*/
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    flow_context_p = ds_flow_manager_find_pending_flow_context(
                                  &pdn_context_p,lte_info_ptr->sdf_id);

    if(!ds_flow_context_validate_flow(flow_context_p))
    {
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return; 
    }

    if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
    {
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return; 
    }
    /*---------------------------------------------------------------------
      Found the pending flow context. Remove the flow context.
    ---------------------------------------------------------------------*/
    if(flow_context_p->ds_flow_context_dyn_p->state != 
       DS_FLOW_CONTEXT_STATE_PEND_ALLOC_ABORT_RSP)
    {
      DS_LTE_MSG0_LOW("Received unexpected allocation failure");
    }
    if(ds_flow_manager_update_precedence_bit_mask(
                     pdn_context_p,flow_context_p,FALSE,FALSE,TRUE)==FALSE)
    {
      DS_LTE_MSG0_LOW("Error in clearing precedence bit mask");
    }
    if(ds_flow_manager_remove_pending_flow_context(
                  pdn_context_p,flow_context_p) == FALSE)
    {
      DS_LTE_MSG0_LOW("Could not remove Pending Flow Context");
    }
    ds_flow_context_release_flow_context(flow_context_p);
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return;
  } /* SDF_ID != 0xFFFF */ 

  DS_LTE_MSG1_LOW("Invalid SDF ID :0x%x",lte_info_ptr->sdf_id);
  return;
}/* ds_eps_flow_manager_bearer_alloc_fail*/


/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_BEARER_MODIFY_REJ

DESCRIPTION
  Handle the bearer modification rejection.

PARAMETERS
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_flow_manager_bearer_modify_rej
(
  cm_call_mode_info_s_type* mode_info_ptr
)
{
   ds_pdn_context_s             *pdn_context_p = NULL;
   ds_flow_context_s            *flow_context_p = NULL;
   cm_lte_call_info_s_type      *lte_info_ptr = NULL;
   ds_bearer_flow_manager_s     *bflow_manager_p = NULL;
   sys_modem_as_id_e_type        subs_id;
#ifdef FEATURE_DATA_REL10
  ps_iface_net_down_reason_type down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
  byte                         *apn = NULL;
#endif /*FEATURE_DATA_REL10*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_LTE_MSG0_HIGH("EPS Flow manager received Bearer modify reject");
  if(mode_info_ptr == NULL)
  {
    DS_LTE_MSG0_ERROR("Input parameter is NULL");
    return;
  }

  /*-------------------------------------------------------------------------
    get the LTE info 
  -------------------------------------------------------------------------*/
  lte_info_ptr =(cm_lte_call_info_s_type *)&(mode_info_ptr->info.lte_call);

  /*------------------------------------------------------------------------
    Check if SDF ID is valid.
  -------------------------------------------------------------------------*/
  if(lte_info_ptr->sdf_id != 0xFFFF)
  {
    /*---------------------------------------------------------------------
      Check if any pending request matches the SDF_ID.
    ---------------------------------------------------------------------*/
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    flow_context_p = ds_flow_manager_find_pending_flow_context(
                                  &pdn_context_p,lte_info_ptr->sdf_id);
    if((!ds_flow_context_validate_flow(flow_context_p)))
    {
      DS_LTE_MSG3_ERROR("flow context 0x%x Pdn context 0x%x non-matching SDF ID 0x",
                 flow_context_p, pdn_context_p, lte_info_ptr->sdf_id);
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return; 
    } 

    if (!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p, &subs_id))
    {
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return;
    }

    bflow_manager_p = flow_context_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr;

    if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
    {
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return; 
    }
    if(bflow_manager_p->ds_bearer_flow_manager_dyn_p->bearer_ptr == NULL)
    {
      DS_LTE_MSG0_ERROR("cannot get bearer from bearer flow manager");
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return; 
    }
    
    /*---------------------------------------------------------------------
      Found the pending flow context. Check the flow context state
    ---------------------------------------------------------------------*/
    if((flow_context_p->ds_flow_context_dyn_p->state 
        !=DS_FLOW_CONTEXT_STATE_PEND_ADD_RSP)&&
       (flow_context_p->ds_flow_context_dyn_p->state 
        !=DS_FLOW_CONTEXT_STATE_PEND_DEL_RSP)&&
       (flow_context_p->ds_flow_context_dyn_p->state 
        != DS_FLOW_CONTEXT_STATE_PEND_DEL_FLOW_RSP)&&
       (flow_context_p->ds_flow_context_dyn_p->state != 
        DS_FLOW_CONTEXT_STATE_PEND_REPLACE_RSP)&&
       (flow_context_p->ds_flow_context_dyn_p->state 
        != DS_FLOW_CONTEXT_STATE_PEND_NO_OP_RSP))
    {
      DS_LTE_MSG1_ERROR("unexpected mod reject.in state:%x",
                        flow_context_p->ds_flow_context_dyn_p->state);
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return;
    }

    if(flow_context_p->ds_flow_context_dyn_p->state == 
       DS_FLOW_CONTEXT_STATE_PEND_DEL_FLOW_RSP)
    {
      /*---------------------------------------------------------------------
        Delete the flow context on UE side.
      ---------------------------------------------------------------------*/
      if(ds_flow_manager_update_precedence_bit_mask(
                   pdn_context_p,flow_context_p,FALSE,FALSE,FALSE)==FALSE)
      {
        DS_LTE_MSG0_LOW("Error in clearing precedence bit mask");
      }
      //update the filter identifier bit mask
      if(ds_bearer_flow_manager_update_filter_identifier_bit_mask(
                      bflow_manager_p->ds_bearer_flow_manager_dyn_p
                      ->bearer_ptr,
                      flow_context_p,FALSE)==FALSE)
      {
        DS_LTE_MSG0_LOW("Error in clearing Filter identifier bit mask");
      }
      if(ds_flow_manager_remove_pending_flow_context(
                  pdn_context_p,flow_context_p) == FALSE)
      { 
        DS_LTE_MSG0_LOW("Could not remove Pending Flow Context");
      }
      ds_flow_context_release_flow_context(flow_context_p);
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    }/* DEL_FLOW_RSP*/
    else
    {
      if(ds_flow_manager_update_ps_with_network_response(
                                                pdn_context_p,
         bflow_manager_p->ds_bearer_flow_manager_dyn_p->bearer_ptr,
                                                flow_context_p,
                                                FALSE)== FALSE)
      {
        DS_LTE_MSG0_LOW("Could not clean up Pending Flow Context");
      }
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    }/* else*/  

#ifdef FEATURE_DATA_REL10
  if(ds_3gpp_nv_manager_get_nv_lte_rel_10(subs_id))
  {
  
    /*Map the NW cause code to PS Iface cause code*/
    if (lte_info_ptr->esm_cause.valid)
    {
      ds_eps_bearer_cntxt_map_cause_codes(CALL_END_ESM_CAUSE_TO_PS_IFACE_CAUSE_CODES,
                                          &down_reason,
                                          (void *)mode_info_ptr);
    }
    else
    {
      down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
    }

    if( (down_reason == PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES) ||
        (down_reason == PS_NET_DOWN_REASON_UNKNOWN_APN) )
    {
      /*---------------------------------------------------------------- 
       Perform global APN throttling for these cause codes. Create a 
       PDN throt SM exclusively for globally throttling the given APN
       across all PLMNs.
      ------------------------------------------------------------------*/
      apn = pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn;
  
      if(*apn == '\0')
      {
        (void)ds_pdn_cntxt_get_resolved_apn_name(pdn_context_p, &apn);
      }

      DS_LTE_MSG2_HIGH("APN being throttled: %s, APN in PDN_ctxt: %s",
               (char*)apn,
                       (char*)pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn);

      ds_3gpp_pdn_throt_lte_handle_reject_with_t3396_ie(lte_info_ptr, apn, 
                                                        down_reason, subs_id,
                                                        pdn_context_p);
    }
  }// get_nv_lte_rel_10()
#endif /*FEATURE_DATA_REL10*/
      return;        
  } /* SDF_ID != 0xFFFF */ 

  DS_LTE_MSG1_LOW("Invalid SDF ID :0x%x",lte_info_ptr->sdf_id);
  return;
} /*ds_eps_flow_manager_bearer_modify_rej */

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_BEARER_MODIFY_IND

DESCRIPTION
  Handle the bearer modification Indication.

PARAMETERS
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS or DS3G_FAILURE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_flow_manager_bearer_modify_ind
(
  cm_call_mode_info_s_type* mode_info_ptr,
  cm_call_id_type           call_id
)
{
  ds_pdn_context_s                      *pdn_context_p = NULL;
  cm_lte_call_info_s_type               *lte_info_ptr = NULL;
  ds_bearer_flow_manager_s              *bflow_manager_p = NULL;
  ds_flow_context_s                     *flow_context_p = NULL;
  ds_bearer_context_s                   *bearer_context_p = NULL;
  ds_3gpp_internal_cause_code_enum_type icause_code
                                          = DS_3GPP_INTERNAL_CC_NONE;
  boolean                                return_value = FALSE;
#ifdef FEATURE_DATA_REL10
  byte                                   *apn = NULL;
#endif /*FEATURE_DATA_REL10*/
  sys_modem_as_id_e_type                 subs_id = SYS_MODEM_AS_ID_NONE;
  ds_eps_bearer_context_flow_dir_e	 bearer_flow_dir;
  ds_eps_bearer_context_flow_dir_e 	 modified_bearer_flow_dir;
  uint32                                 rat_mask = 
                                           DS_3GPP_THROT_RAT_MASK_GLOBAL;
                                         /* RATs on which throttling needs to
                                            be unblocked */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(mode_info_ptr == NULL)
  {
    DS_LTE_MSG0_ERROR("Input parameter is NULL");
    return DS3G_FAILURE;
  }
  /*-------------------------------------------------------------------------
    get the LTE info 
  -------------------------------------------------------------------------*/
  lte_info_ptr =(cm_lte_call_info_s_type *)&(mode_info_ptr->info.lte_call);
  DS_LTE_MSG1_HIGH("EPS Flow manager received Bearer modification indication "
                   "Modify Ind for SDF ID :0x%x",lte_info_ptr->sdf_id);

   bearer_context_p= ds_bearer_cntxt_get_bearer_from_call_id(call_id);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }

   /*   Caching the previous Bearer Flow Direction    */
   bearer_flow_dir = ds_eps_bearer_cntxt_getFlowDirection(bearer_context_p);

  /*------------------------------------------------------------------------
    Check if SDF ID is valid.
  -------------------------------------------------------------------------*/
  if(lte_info_ptr->sdf_id != 0xFFFF)
  {
    /*---------------------------------------------------------------------
      Check if any pending request matches the SDF_ID.
    ---------------------------------------------------------------------*/
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    flow_context_p = ds_flow_manager_find_pending_flow_context(
                                  &pdn_context_p,lte_info_ptr->sdf_id);


   if((!ds_flow_context_validate_flow(flow_context_p)) ||
         (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p)))
    {
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      lte_info_ptr->esm_cause.valid = TRUE;
      lte_info_ptr->esm_cause.esm_cause =NAS_ESM_ACTIVATION_REJECTED;
      return DS3G_FAILURE;
    }
    /*-----------------------------------------------------------------------
      Bearer modification should have atleast one of Qos or TFT set.
    -----------------------------------------------------------------------*/
    if ((lte_info_ptr->sdf_qos.valid != TRUE) &&
       (lte_info_ptr->ul_dl_tft.valid != TRUE))
    {
      DS_LTE_MSG0_ERROR("invalid params passed");
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      lte_info_ptr->esm_cause.valid = TRUE;
      lte_info_ptr->esm_cause.esm_cause =NAS_ESM_INVALID_MANDATORY_INFORMATION;
      return DS3G_FAILURE;
    }
    /*---------------------------------------------------------------------
      Found the pending flow context. Retrieve bearer context
    ---------------------------------------------------------------------*/
    bflow_manager_p = flow_context_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr;

    if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
    {
      /* Case where UE bearer request is NW bearer mod */
      bearer_context_p= 
                ds_bearer_cntxt_get_bearer_from_call_id(call_id);
      if(bearer_context_p == NULL)
      {
        DS_LTE_MSG0_ERROR("Bearer is NULL");
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        lte_info_ptr->esm_cause.valid = TRUE;
        lte_info_ptr->esm_cause.esm_cause =NAS_ESM_ACTIVATION_REJECTED;
        return DS3G_FAILURE;
      }
      bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(
                                                    bearer_context_p);

      if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
      {
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        lte_info_ptr->esm_cause.valid = TRUE;
        lte_info_ptr->esm_cause.esm_cause =NAS_ESM_ACTIVATION_REJECTED;
        return DS3G_FAILURE;
      }     
    }
    if(bearer_context_p == NULL)
    {
      bearer_context_p = bflow_manager_p->ds_bearer_flow_manager_dyn_p
        ->bearer_ptr;
    }
    if(!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p,  &subs_id))
    {
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      lte_info_ptr->esm_cause.valid = TRUE;
      lte_info_ptr->esm_cause.esm_cause =NAS_ESM_ACTIVATION_REJECTED;
      return DS3G_FAILURE;
    } 
    /*---------------------------------------------------------------------
      Verify if requested flow and granted matches.
    ---------------------------------------------------------------------*/
    if (ds_flow_manager_verify_nw_response(
                            pdn_context_p,
                            bearer_context_p,
                            flow_context_p,
                            mode_info_ptr,
                            &(lte_info_ptr->ul_dl_tft),
                            &icause_code) == DS3G_FAILURE)
    {
      if(ds_flow_manager_update_ps_with_network_response(
                                                pdn_context_p,
                                                bearer_context_p,
                                                flow_context_p,
                                                FALSE)== FALSE)
      {
        DS_LTE_MSG0_LOW("ps_network_response returned error");
      }
  
      /*-------------------------------------------------------------------
        Update the cause codes.
      -------------------------------------------------------------------*/
      if( (icause_code == DS_3GPP_INTERNAL_CC_NONE) ||
            (icause_code == DS_3GPP_INTERNAL_CC_INVALID))
      {
        DS_LTE_MSG0_LOW("cause code not set");
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        lte_info_ptr->esm_cause.valid = TRUE;
        lte_info_ptr->esm_cause.esm_cause =NAS_ESM_ACTIVATION_REJECTED;
        return DS3G_FAILURE;
      }
      if(ds_flow_manager_convert_cause_codes
           (mode_info_ptr,&icause_code, subs_id) == FALSE)
      {
        DS_LTE_MSG0_LOW("cause code conversion failed");
      }
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return DS3G_FAILURE;
    }/* verify_nwResponse == DS3G_FAILURE*/
    else
    { 
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      if(lte_info_ptr->apn_ambr.valid == TRUE)
      {
        /*--------------------------------------------------------------------
         Store the APN AMBR in Mode specific context
        ---------------------------------------------------------------------*/
        if(ds_eps_flow_manager_update_apn_ambr(
                                                lte_info_ptr, 
                                                pdn_context_p
                                               ) == FALSE)
        {
          lte_info_ptr->esm_cause.valid = TRUE;
          lte_info_ptr->esm_cause.esm_cause =NAS_ESM_INSUFFICIENT_RESOURCES;
          DS_LTE_MSG0_LOW("Unexpected error in apn ambr");
          // We need to revert to old flow stuff or teardown the flow.
          return DS3G_FAILURE;
        }     
      } /* if apn_ambr is valid */
    }/*  verify_nwResponse == DS3G_SUCCESS */

    if (bearer_context_p != NULL)
    {
      ds_eps_bearer_cntxt_send_QOS_indication(bearer_context_p);
    }

    ds_flow_context_ps_flow_set_granted_flow(flow_context_p); 

    return DS3G_SUCCESS;   
  } /* SDF_ID != 0xFFFF */ 

  /*---------------------------------------------------------------------
    Process the request for NW initiated Bearer Modification.
  ---------------------------------------------------------------------*/
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  return_value = ds_eps_flow_manager_nw_bearer_modify_ind(
                                           mode_info_ptr, call_id);
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
#ifdef FEATURE_DATA_REL10
  if(ds_3gpp_nv_manager_get_nv_lte_rel_10(subs_id))
  {
   /* Unblocking apn irespective of whether modification was succesful or not. */
    if(pdn_context_p == NULL)
    {
      bearer_context_p = 
                ds_bearer_cntxt_get_bearer_from_call_id(call_id);
      if(bearer_context_p == NULL)
      {
        DS_LTE_MSG0_ERROR("Bearer is NULL, not unblocking APN");
        return return_value;
      }
      /*-----------------------------------------------------------------------
        Retrieve the PDN Context
      -----------------------------------------------------------------------*/
      pdn_context_p = (ds_pdn_context_s*)bearer_context_p->
                       ds_bearer_context_dyn_p->pdn_context_ptr;
    }

    if (!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p, &subs_id))
    {
      return return_value;
    }

    /*---------------------------------------------------------------------
    EPS Bearer context modification got successfull processed by UE.
    Clear APN throttling state machine if it was globally blocked.
    ----------------------------------------------------------------------*/
    apn = pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn;
    /*-------------------------------------------------------- 
       KW Fix: Add an explicit Null pointer check for 'apn'
    --------------------------------------------------------*/
    if((apn != NULL) && (*apn == '\0'))
    {
      (void)ds_pdn_cntxt_get_resolved_apn_name(pdn_context_p, &apn);
    }
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,
                           "Trying to unblock potentially blocked APN: %s",
                           (char*)apn);
    /*---------------------------------------------------------------------
      Unblock the APN on Global RAT and also LTE RAT if throttling per RAT is
      enabled
    ----------------------------------------------------------------------*/
    if (ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat(subs_id))
    {
      rat_mask |= DS_3GPP_THROT_RAT_MASK_LTE;
    }

    ds_3gpp_pdn_throt_handle_nw_inited_unblock_apn(apn,subs_id, rat_mask);
  } //get_nv_lte_rel_10()
#endif /*FEATURE_DATA_REL10*/

  /*   Caching the new Bearer Flow Direction    */
  modified_bearer_flow_dir = ds_eps_bearer_cntxt_getFlowDirection(bearer_context_p);

  if(modified_bearer_flow_dir != bearer_flow_dir)
  {
    DS_LTE_MSG2_HIGH("Modify Ind, flow_dir: %d modified_flow_dir: %d",
			               bearer_flow_dir,modified_bearer_flow_dir);

    ds_eps_bearer_cntxt_re_register_wm_with_pdcp(bearer_flow_dir,
  	                                             modified_bearer_flow_dir,call_id);
  }
  return return_value;
} /* ds_eps_flow_manager_bearer_modify_ind */ 

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_BEARER_ALLOCATION_IND

DESCRIPTION
  Handle the bearer allocation Indication.

PARAMETERS
  pdn_ptr         -  Ptr to the PDN context.
  bearer_ptr      -  Ptr to the bearer context.
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS or DS3G_FAILURE.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_flow_manager_bearer_allocation_ind
(
  ds_pdn_context_s*         pdn_context_p,
  ds_bearer_context_s*      bearer_context_p,
  const cm_call_mode_info_s_type* mode_info_ptr
)
{
  cm_lte_call_info_s_type      *lte_info_ptr = NULL;
  ds_bearer_flow_manager_s     *bflow_manager_p = NULL;
  ds_flow_manager_s            *flow_manager_p = NULL;
  ds_flow_context_s            *flow_context_p = NULL;
  ds_3gpp_internal_cause_code_enum_type icause_code=DS_3GPP_INTERNAL_CC_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_LTE_MSG1_HIGH("Eps bearer allocation ind for bearer 0x%x",bearer_context_p);
  if((pdn_context_p== NULL)||(bearer_context_p== NULL)||(mode_info_ptr == NULL))
  {
    DS_LTE_MSG0_ERROR("input params are NULL");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    get the LTE info 
  -------------------------------------------------------------------------*/
  lte_info_ptr =(cm_lte_call_info_s_type *)&(mode_info_ptr->info.lte_call);

  /*-------------------------------------------------------------------------
    Verify FLOW Fields 
  -------------------------------------------------------------------------*/
  if(lte_info_ptr->sdf_qos.valid != TRUE)
  {
    DS_LTE_MSG0_ERROR("Received qos with valid flag set to FALSE");
    lte_info_ptr->esm_cause.valid = TRUE;
    lte_info_ptr->esm_cause.esm_cause =NAS_ESM_INVALID_MANDATORY_INFORMATION;
    return DS3G_FAILURE;
  }
  /*------------------------------------------------------------------------
    Check if the PDN has flow manager associated with it.
  -------------------------------------------------------------------------*/
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  flow_manager_p = ds_flow_manager_find_flow_manager(pdn_context_p);
  if(flow_manager_p == NULL)
  {
    DS_LTE_MSG0_ERROR("PDN does not have flow manager");
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    lte_info_ptr->esm_cause.valid = TRUE;
    lte_info_ptr->esm_cause.esm_cause =NAS_ESM_INSUFFICIENT_RESOURCES;
    return DS3G_FAILURE;
  }

  /*------------------------------------------------------------------------
    Check if SDF ID is valid. Then it is a response to UE initiated 
    bearer allocation request.
  -------------------------------------------------------------------------*/
  if(lte_info_ptr->sdf_id != 0xFFFF)
  {
    /*---------------------------------------------------------------------
      Check if any pending request matches the SDF_ID.
    ---------------------------------------------------------------------*/
    flow_context_p=ds_flow_manager_find_pending_flow_context_for_pdn(
                                                 pdn_context_p,
                                                 lte_info_ptr->sdf_id);
    if(!ds_flow_context_validate_flow(flow_context_p))
    {
      DS_LTE_MSG1_ERROR("Could not find the flow context matching SDF ID:0x%x",
                        lte_info_ptr->sdf_id);
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      lte_info_ptr->esm_cause.valid = TRUE;
      lte_info_ptr->esm_cause.esm_cause =NAS_ESM_ACTIVATION_REJECTED;
      return DS3G_FAILURE; 
    }
    /*-----------------------------------------------------------------------
       New bearer allocation should have both Qos and TFT set.
    -----------------------------------------------------------------------*/
    if ((lte_info_ptr->sdf_qos.valid != TRUE) ||
       (lte_info_ptr->ul_dl_tft.valid != TRUE))
    {
      DS_LTE_MSG0_ERROR("invalid params passed");
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      lte_info_ptr->esm_cause.valid = TRUE;
      lte_info_ptr->esm_cause.esm_cause =NAS_ESM_INVALID_MANDATORY_INFORMATION;
      return DS3G_FAILURE;
    }

    /*---------------------------------------------------------------------
      Verify if requested flow and granted matches.
    ---------------------------------------------------------------------*/
    if (ds_flow_manager_verify_nw_response(
                            pdn_context_p,
                            bearer_context_p,
                            flow_context_p,
                            mode_info_ptr,
                            &(lte_info_ptr->ul_dl_tft),
                            &icause_code) == DS3G_FAILURE)
    {

      /*---------------------------------------------------------------------
        Remove the pending flow context.
      ---------------------------------------------------------------------*/
      if(ds_flow_manager_update_ps_with_network_response(
                                                pdn_context_p,
                                                bearer_context_p,
                                                flow_context_p,
                                                FALSE)== FALSE)
      {
        DS_LTE_MSG0_LOW("ps_network_response returned error");
      }

      /* Update the cause codes */
      if( (icause_code == DS_3GPP_INTERNAL_CC_NONE) ||
            (icause_code == DS_3GPP_INTERNAL_CC_INVALID))
      {
        DS_LTE_MSG0_LOW("cause code not set");
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        lte_info_ptr->esm_cause.valid = TRUE;
        lte_info_ptr->esm_cause.esm_cause =NAS_ESM_ACTIVATION_REJECTED;
        return DS3G_FAILURE;
      }
      if(ds_eps_flow_manager_convert_cause_code(mode_info_ptr,&icause_code)
         ==FALSE)
      {
        DS_LTE_MSG0_LOW("cause code conversion failed");
      }
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return DS3G_FAILURE;
    }/* verify_nwResponse == DS3G_FAILURE*/
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return DS3G_SUCCESS;   
  }/* lte_info_ptr->sdf_id != 0xFFFF */

  /*-------------------------------------------------------------------------
    Verify if the bearer already has a flow manager.
  -------------------------------------------------------------------------*/
  bflow_manager_p=ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(bflow_manager_p != NULL) 
  {
    DS_LTE_MSG0_ERROR("Bearer associated with a bearer flow manager");
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    lte_info_ptr->esm_cause.valid = TRUE;
    lte_info_ptr->esm_cause.esm_cause =NAS_ESM_INSUFFICIENT_RESOURCES;
    return DS3G_FAILURE;
  }

  /*-------------------------------------------------------------------------
    Allocate a new bearer flow manager.
  -------------------------------------------------------------------------*/
  bflow_manager_p = 
              ds_bearer_flow_manager_allocate_flow_manager(bearer_context_p);
  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
      ds_3gpp_dyn_mem_hdlr_free_bearer_flow_mgr_dyn_mem(bearer_context_p);
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      lte_info_ptr->esm_cause.valid = TRUE;
      lte_info_ptr->esm_cause.esm_cause =NAS_ESM_INSUFFICIENT_RESOURCES;
      return DS3G_FAILURE;
  }
  DS_LTE_MSG0_LOW("Bearer Allocated a bearer flow manager");
  /*---------------------------------------------------------------------
   Update the PDN PTR.
  ---------------------------------------------------------------------*/
  bflow_manager_p->ds_bearer_flow_manager_dyn_p->pdn_ptr = pdn_context_p;

  if(ds_flow_manager_create_flow_context(
                                        pdn_context_p,
                                        bearer_context_p,
                                        mode_info_ptr,
                                        &(lte_info_ptr->ul_dl_tft),
                                        &icause_code) == DS3G_FAILURE)
  {
    /*Release the Bearer flow manager in case of failure; 
     this is required to prevent further failures*/
    ds_bearer_flow_manager_release_flow_manager(bearer_context_p);
    if(lte_info_ptr->esm_cause.valid != TRUE)
    {
      DS_LTE_MSG0_ERROR("Create flow context failure with no error code");
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      lte_info_ptr->esm_cause.valid = TRUE;
      lte_info_ptr->esm_cause.esm_cause =NAS_ESM_INSUFFICIENT_RESOURCES;
      return DS3G_FAILURE;
    }
    else
    {
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return DS3G_FAILURE;
    }
  }
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  return DS3G_SUCCESS;
}/* ds_eps_flow_manager_bearer_allocation_ind */

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_CHECK_SDF_ID

DESCRIPTION
  Handle the bearer modificaiion rejection.

PARAMETERS
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.
    
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_flow_manager_check_sdf_id
(
  cm_call_mode_info_s_type* mode_info_ptr
)
{
   ds_pdn_context_s             *pdn_context_p = NULL;
   ds_flow_context_s             *flow_context_p = NULL;
   cm_lte_call_info_s_type       *lte_info_ptr = NULL;
   ds_bearer_flow_manager_s      *bflow_manager_p = NULL;
   ps_extended_info_code_enum_type ps_eic = PS_EIC_NOT_SPECIFIED; 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_LTE_MSG0_LOW("EPS Flow manager received Check SDF ID from Call End");
  if(mode_info_ptr == NULL)
    {
    DS_LTE_MSG0_ERROR("Input parameter is NULL");
    return;
    }

  /*-------------------------------------------------------------------------
    get the LTE info 
  -------------------------------------------------------------------------*/
  lte_info_ptr =(cm_lte_call_info_s_type *)&(mode_info_ptr->info.lte_call);

  /*------------------------------------------------------------------------
    Check if SDF ID is valid.
  -------------------------------------------------------------------------*/
  if(lte_info_ptr->sdf_id != 0xFFFF)
  {
    /*---------------------------------------------------------------------
      Check if any pending request matches the SDF_ID.
    ---------------------------------------------------------------------*/
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    flow_context_p = ds_flow_manager_find_pending_flow_context(
                                  &pdn_context_p,lte_info_ptr->sdf_id);
    if(!ds_flow_context_validate_flow(flow_context_p))
    {
      DS_LTE_MSG1_ERROR("Could not find the flow context matching SDF ID:0x%x",
                        lte_info_ptr->sdf_id);
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return; 
    } 

    if(pdn_context_p == NULL)
    {
      DS_LTE_MSG0_ERROR("PDN context pointer is not updated");
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return; 
    }
    if((flow_context_p->ds_flow_context_dyn_p->state
        !=DS_FLOW_CONTEXT_STATE_PEND_DEL_FLOW_RSP)&&
       (flow_context_p->ds_flow_context_dyn_p->state
        !=DS_FLOW_CONTEXT_STATE_PEND_DEL_RSP)&&
       (flow_context_p->ds_flow_context_dyn_p->state
        !=DS_FLOW_CONTEXT_STATE_PEND_ADD_RSP)&&
       (flow_context_p->ds_flow_context_dyn_p->state
        !=DS_FLOW_CONTEXT_STATE_PEND_REPLACE_RSP)&&
       (flow_context_p->ds_flow_context_dyn_p->state
        !=DS_FLOW_CONTEXT_STATE_PEND_NO_OP_RSP)&&
       (flow_context_p->ds_flow_context_dyn_p->state
        !=DS_FLOW_CONTEXT_STATE_COMING_UP)&&
       (flow_context_p->ds_flow_context_dyn_p->state
        !=DS_FLOW_CONTEXT_STATE_UP))
    {
      DS_LTE_MSG1_HIGH("Received SDF ID in an unknow state:%x",
                       flow_context_p->ds_flow_context_dyn_p->state);
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return;
    }
    //if(flow_context_p->state == DS_FLOW_CONTEXT_STATE_PEND_DEL_FLOW_RSP)
    //{
    /*---------------------------------------------------------------------
      Found the pending flow context. Remove the pending flow context.
    ---------------------------------------------------------------------*/
    if(ds_flow_manager_update_precedence_bit_mask(
                   pdn_context_p,flow_context_p,FALSE,FALSE,FALSE)==FALSE)
    {
      DS_LTE_MSG0_LOW("Error in clearing precedence bit mask");
    }
    /*---------------------------------------------------------------------
      Update the bearer filter identifier mask.
      and unbind the flow.
    ---------------------------------------------------------------------*/
    bflow_manager_p = flow_context_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr;
    if(ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
    {
      if(ds_bearer_flow_manager_update_filter_identifier_bit_mask(
                      bflow_manager_p->ds_bearer_flow_manager_dyn_p->bearer_ptr,
                      flow_context_p,FALSE)==FALSE)
      {
        DS_LTE_MSG0_LOW("Error in setting Filter identifier bit mask");
      }
      ds_flow_context_ps_flow_go_null_ind(flow_context_p, ps_eic);
    }

    if(ds_flow_manager_remove_pending_flow_context(
                  pdn_context_p,flow_context_p) == FALSE)
    {
      DS_LTE_MSG0_LOW("Could not remove Pending Flow Context");
    }

    ds_flow_context_release_flow_context(flow_context_p);
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return;
    //}
    /* For other states, we need to anyway delete all the flows associated
       with bearer.
       Revert back to the orginal configuration, and move the flow context 
       to the bearer, so will be deleted.
    */
     /* As the bearer is gone, we should also walk thru the list on the PDN
        and delete allt he flow contexts associated with this bearer, if
        the SDF-ID does not match (when we subsequently get responses , ignore 
        and return as usual. */
     /* If the bearer is gone, then we get bearer_mod fail, we should then 
        remove the */
  } /* SDF_ID != 0xFFFF */ 
  
  DS_LTE_MSG1_LOW("Invalid SDF ID :0x%x",lte_info_ptr->sdf_id); 
  return;
}/* ds_eps_flow_manager_check_sdf_id */

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_BEARER_MODIFY_REQ

DESCRIPTION
  Handle the bearer modification request.

PARAMETERS
  flow_context_p   -  Pointer to Flow context to be modified.
  nw_tft_p         -  Pointer to Network TFT.
  app_qos          -  Pointer to App Qos.
  flow_state        -  flow state when the command was posted

DEPENDENCIES
  None.

RETURN VALUE
  int              - -1 if it fails, 0 if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
int ds_eps_flow_manager_bearer_modify_req
(
  ds_flow_context_s             *flow_context_p,
  tft_type_T                    *nw_tft_p,
  qos_spec_type                 *app_qos,
  int                           flow_state
)
{
  cm_res_mod_s_type             *res_mod = NULL;  
  ds_pdn_context_s              *pdn_context_p = NULL;
  ds_3gpp_iface_s               *ds_iface_p    = NULL;
  ds_bearer_context_s           *bearer_context_p = NULL;
  ds_bearer_flow_manager_s      *bflow_manager_p = NULL;
  tft_type_T                    *nw_tft = NULL;
  ds_eps_bearer_context_s       *eps_bearer_p = NULL;
  sys_modem_as_id_e_type        subs_id  = SYS_MODEM_AS_ID_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    DS_LTE_MSG0_ERROR("Invalid flow context passed");
    return -1;
  }
  
  if(flow_context_p->ds_flow_context_dyn_p->state != 
     (ds_flow_context_state_e)flow_state)
  {
    DS_LTE_MSG2_ERROR("flow states do not match. current:0x%x; cmd for:0x%x",
                      flow_context_p->ds_flow_context_dyn_p->state, flow_state);
    return -1;
  }
   
  res_mod = (cm_res_mod_s_type*)modem_mem_alloc(sizeof(cm_res_mod_s_type),
                                   MODEM_MEM_CLIENT_DATA_CRIT); 

   if(res_mod == NULL)
   {
     DS_LTE_MSG0_LOW("memory allocation failed for res_mod");
     return -1;
   }
   
  memset(res_mod, 0, sizeof(cm_res_mod_s_type));
  res_mod->esm_cause.valid = FALSE;

  if( (ds_iface_p = flow_context_p->ds_flow_context_dyn_p->ds_iface_ptr) == NULL)
  {
    DS_LTE_MSG0_ERROR("DS IFACE PTR is null for flow context");
    modem_mem_free(res_mod, MODEM_MEM_CLIENT_DATA_CRIT);
    return -1;
  } 
 
  if(ds_iface_p->client_data_ptr == NULL)
  {
    DS_LTE_MSG0_ERROR("PDN CNTX PTR is null");
    modem_mem_free(res_mod, MODEM_MEM_CLIENT_DATA_CRIT);
    return -1;
  }
  pdn_context_p = ds_iface_p->client_data_ptr;
  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p,&subs_id))
  {
    modem_mem_free(res_mod, MODEM_MEM_CLIENT_DATA_CRIT);
    return -1;
  }
  bflow_manager_p = flow_context_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr;

  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    modem_mem_free(res_mod, MODEM_MEM_CLIENT_DATA_CRIT);
    return -1;
  }

  if(( (bearer_context_p = bflow_manager_p->ds_bearer_flow_manager_dyn_p
       ->bearer_ptr) == NULL) || ((!ds_bearer_cntx_validate_bearer_context
                                   (bearer_context_p))))
  {
    modem_mem_free(res_mod, MODEM_MEM_CLIENT_DATA_CRIT);
    return -1;
  }
  // Assign nw tft pointer
  nw_tft = &(res_mod->ul_dl_tft);

  if(flow_context_p->ds_flow_context_dyn_p->state == 
     DS_FLOW_CONTEXT_STATE_PEND_DEL_FLOW_RSP)
  {
    /*-------------------------------------------------------------------
      Request deleting only the filters associated with the FLOW.
    -------------------------------------------------------------------*/
    if((nw_tft_p->valid) && 
          (nw_tft_p->operation_code == (byte) DS_3GPP_TFT_OPCODE_DEL_FILTER))
    {
      DS_LTE_MSG0_HIGH("Processing filled in tft del filter");
      memscpy(nw_tft, sizeof(tft_type_T), nw_tft_p, sizeof(tft_type_T));
    }
    else
    {
      nw_tft->valid = TRUE;
      nw_tft->operation_code = (byte) DS_3GPP_TFT_OPCODE_DEL_FILTER;
      nw_tft->e_bit = 0;  
      if(ds_flow_manager_update_app_filter_to_delete(
                                 nw_tft,flow_context_p) == FALSE)
      {
        DS_LTE_MSG0_ERROR("Error in updating the TFT info");
        modem_mem_free(res_mod, MODEM_MEM_CLIENT_DATA_CRIT);
        return -1;
      }
      nw_tft->tft_total_length = 1 + nw_tft->num_filters;
    }/* nw_tft_p->Valid == FALSE */
    if(list_size(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->flow_context)) 
       == 1)
    {
      res_mod->esm_cause.valid = TRUE;
      res_mod->esm_cause.esm_cause = NAS_ESM_REGULAR_DEACTIVATION;
      // This logic is off if we have more than 1 bearer modification
      // on the same bearer.
    }
  }/* delete Flow*/ 

  if(flow_context_p->ds_flow_context_dyn_p->state == 
     DS_FLOW_CONTEXT_STATE_PEND_MODIFY_RSP)
  {
    if(nw_tft_p == NULL)
    {
      DS_LTE_MSG0_ERROR("Input TFT param is NULL");
      modem_mem_free(res_mod, MODEM_MEM_CLIENT_DATA_CRIT);
      return -1;
    }
    memscpy(nw_tft, sizeof(tft_type_T), nw_tft_p, sizeof(tft_type_T));
    if(app_qos != NULL)
    {
      if(app_qos->field_mask & (QOS_MASK_RX_FLOW|QOS_MASK_TX_FLOW))
      {
        if(ds_bearer_flow_manager_convert_app_qos_to_nw_qos
           (app_qos, (void *)&res_mod->sdf_qos, subs_id) == FALSE)
        {
          DS_LTE_MSG0_ERROR("APP TFT conversion to NW TFT failed");
          modem_mem_free(res_mod, MODEM_MEM_CLIENT_DATA_CRIT);
          return -1;
        /* We need to send modify reject to PS. App is aware that request
           did not go thru */
        }
      }
    }/* app_qos != NULL */
    else
    {
      res_mod->sdf_qos.valid = FALSE;
    }
  } /* modify Flow*/
  /*---------------------------------------------------------------------
    Update the params and send out the NW request.
  ---------------------------------------------------------------------*/
  eps_bearer_p = (ds_eps_bearer_context_s *)
        DS_BEARER_CTXT_VOBJ(bearer_context_p, SYS_SYS_MODE_LTE);
 
  if(!ds_eps_bearer_cntxt_validate_bearer_cntxt(eps_bearer_p))
  {
    modem_mem_free(res_mod, MODEM_MEM_CLIENT_DATA_CRIT);
    return -1;
  }
  res_mod->sdf_id   = flow_context_p->sdf_id;
  res_mod->lbi      = eps_bearer_p->ds_eps_bearer_context_dyn_p->eps_bearer_id;
  res_mod->protocol_config_options.valid = FALSE;

  res_mod->low_priority_signaling =
    ds_3gpp_get_low_priority_signaling_bit(pdn_context_p);

  DS_LTE_MSG1_HIGH("Low Priority signaling bit is %d",
                   res_mod->low_priority_signaling);

  if ((ds3g_msh_modify_bearer_req_ex( (res_mod) ,
                                       subs_id )) == DS3G_FAILURE )
  {
    DS_LTE_MSG0_ERROR("DS3G Bearer modify req failed"); 
    modem_mem_free(res_mod, MODEM_MEM_CLIENT_DATA_CRIT);
    return -1;
    // Will have to add a re-try mechanism to try sending out again
  }

  if(flow_context_p->ds_flow_context_dyn_p->state == 
     DS_FLOW_CONTEXT_STATE_PEND_MODIFY_RSP)
  {
    if(nw_tft_p->operation_code == (byte)DS_3GPP_TFT_OPCODE_DEL_FILTER)
    {
      ds_flow_context_set_flow_state(flow_context_p,
                                  DS_FLOW_CONTEXT_STATE_PEND_DEL_RSP);
    }
    else if(nw_tft_p->operation_code==(byte)DS_3GPP_TFT_OPCODE_REPLACE)
    {
      ds_flow_context_set_flow_state(flow_context_p,
                                  DS_FLOW_CONTEXT_STATE_PEND_REPLACE_RSP);
    }
    else if(nw_tft_p->operation_code ==(byte)DS_3GPP_TFT_OPCODE_NO_OP)
    {
      ds_flow_context_set_flow_state(flow_context_p,
                                  DS_FLOW_CONTEXT_STATE_PEND_NO_OP_RSP);
    }
    else
    {
      DS_LTE_MSG0_ERROR("Unsupported nw tft operation");
      modem_mem_free(res_mod, MODEM_MEM_CLIENT_DATA_CRIT);
      return -1;
    }
  }/* if STATE_PEND_MODIFY_RSP */

  if(ds_bearer_flow_manager_remove_flow_context_from_flow_manager(
                           bearer_context_p, flow_context_p) == FALSE)
  {
    DS_LTE_MSG0_LOW("Error when remove flow context from Bflow mgr");
  }

  //Update the bearer context with the sdf id so that if ever IRAT into GW;
  //we have a way of associating the flow with the bearer.

  bearer_context_p->ds_bearer_context_dyn_p->sdf_id = flow_context_p->sdf_id;

  /*---------------------------------------------------------------------
    Move the flow context from bearer flow manager to pending context
    Convert App Qos to NW Qos.
  ---------------------------------------------------------------------*/
  if(ds_flow_manager_add_pending_flow_context(
                           pdn_context_p,flow_context_p) == FALSE)
  {
    DS_LTE_MSG0_LOW("Error when adding pending flow context to flow mgr");
  }
  /*---------------------------------------------------------------------
    Keep a copy of the UE request to NW.
  ---------------------------------------------------------------------*/
  memscpy(&(flow_context_p->ds_flow_context_dyn_p->tft_mod), sizeof(tft_type_T),
          nw_tft, sizeof(tft_type_T));
  modem_mem_free(res_mod, MODEM_MEM_CLIENT_DATA_CRIT);
  return 0;
}/* ds_eps_flow_manager_bearer_modify_req */

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_BEARER_ALLOC_REQ

DESCRIPTION
  Handle the bearer allocation request.

PARAMETERS
  pdn_context_p    -  Pointer to PDN context to be modified.
  flow_context_p   -  Pointer to Flow context to be modified.
  nw_tft_p         -  Pointer to NW TFT
  app_qos          -  Pointer to App Qos.

DEPENDENCIES
  None.

RETURN VALUE
  int              - -1 if it fails, 0 if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
int ds_eps_flow_manager_bearer_alloc_req
(
  ds_pdn_context_s    *pdn_context_p,
  ds_flow_context_s   *flow_context_p,
  qos_spec_type       *app_qos,
  tft_type_T          *nw_tft
)
{
  cm_res_alloc_s_type               *res_alloc_req = NULL; 
  ds_bearer_context_s              *def_bearer_p = NULL; 
  ds_eps_bearer_context_s          *eps_bearer_p = NULL;
  ds_bearer_context_s              *bearer_context_p = NULL;
  ds_bearer_flow_manager_s         *bflow_manager_p = NULL;
  boolean                           result = FALSE;
  sys_modem_as_id_e_type            subs_id  = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  res_alloc_req = (cm_res_alloc_s_type*)modem_mem_alloc(sizeof(cm_res_alloc_s_type),
                                                          MODEM_MEM_CLIENT_DATA_CRIT); 

   if(res_alloc_req == NULL)
   {
     DS_LTE_MSG0_ERROR("memory allocation failed for res_alloc_req");
     return -1;
   }

  memset(res_alloc_req, 0, sizeof(cm_res_alloc_s_type));

  
  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p,&subs_id))
  {
    modem_mem_free(res_alloc_req, MODEM_MEM_CLIENT_DATA_CRIT);
    return -1;
  }
  if((app_qos== NULL)
     ||(nw_tft == NULL)||(!ds_flow_context_validate_flow(flow_context_p)))
  {
    DS_LTE_MSG3_ERROR("Input ptr is NULL.Flow_ptr:0x%x pdn_ptr:0x%x app_qos:0x%x",
               flow_context_p,pdn_context_p,app_qos);
    modem_mem_free(res_alloc_req, MODEM_MEM_CLIENT_DATA_CRIT);
    return  -1;
  }

  /*-------------------------------------------------------------------------
    Verify flow context state
  -------------------------------------------------------------------------*/
  if( (flow_context_p->ds_flow_context_dyn_p->state != 
       DS_FLOW_CONTEXT_STATE_COMING_UP) &&
      (flow_context_p->ds_flow_context_dyn_p->state !=
       DS_FLOW_CONTEXT_STATE_PEND_MODIFY_RSP))
  {
    DS_LTE_MSG1_HIGH("Bearer Alloc received in unexpeced flow state:0x%x",
               flow_context_p->ds_flow_context_dyn_p->state);
    modem_mem_free(res_alloc_req, MODEM_MEM_CLIENT_DATA_CRIT);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Convert App requested Qos to NW specific Qos
  -------------------------------------------------------------------------*/
  if(ds_bearer_flow_manager_convert_app_qos_to_nw_qos
     (app_qos,(void *)&res_alloc_req->sdf_qos, subs_id) == FALSE)
  {
    DS_LTE_MSG0_ERROR("APP TFT conversion to NW TFT failed");
    if(ds_flow_manager_update_precedence_bit_mask(
                     pdn_context_p,flow_context_p,FALSE,FALSE,TRUE)==FALSE)
    {
      DS_LTE_MSG0_LOW("Error in clearing precedence bit mask");
    }  
    if(flow_context_p->ds_flow_context_dyn_p->state == 
       DS_FLOW_CONTEXT_STATE_COMING_UP)
    {
      ds_flow_context_release_flow_context(flow_context_p);
      modem_mem_free(res_alloc_req, MODEM_MEM_CLIENT_DATA_CRIT);
      return -1;
    }
    if(flow_context_p->ds_flow_context_dyn_p->state == 
       DS_FLOW_CONTEXT_STATE_PEND_MODIFY_RSP)
      {
      DS_LTE_MSG0_ERROR("Post a modify failed to APP and change flow context state");
      // need seperate function to take care of this..
      modem_mem_free(res_alloc_req, MODEM_MEM_CLIENT_DATA_CRIT);
      return -1;
    }
  }

  /*-------------------------------------------------------------------------
    Handling  in the case of a new flow
  -------------------------------------------------------------------------*/
  if(flow_context_p->ds_flow_context_dyn_p->state == DS_FLOW_CONTEXT_STATE_COMING_UP)
  {
    /*-----------------------------------------------------------------------
      Fill in the required parameters
    -----------------------------------------------------------------------*/
    def_bearer_p=(ds_bearer_context_s *)
                         pdn_context_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;
      
    if(!ds_bearer_cntx_validate_bearer_context(def_bearer_p))
    {
      modem_mem_free(res_alloc_req, MODEM_MEM_CLIENT_DATA_CRIT);
      return -1;
    }
    eps_bearer_p = (ds_eps_bearer_context_s *)
        DS_BEARER_CTXT_VOBJ(def_bearer_p, SYS_SYS_MODE_LTE);
 
    if(!ds_eps_bearer_cntxt_validate_bearer_cntxt(eps_bearer_p))
    {  
      modem_mem_free(res_alloc_req, MODEM_MEM_CLIENT_DATA_CRIT);
      return -1;
    }

    res_alloc_req->sdf_id   = flow_context_p->sdf_id;
    res_alloc_req->lbi      = eps_bearer_p->ds_eps_bearer_context_dyn_p
                             ->eps_bearer_id;
    res_alloc_req->protocol_config_options.valid = FALSE;
    memscpy(&res_alloc_req->ul_dl_tft, sizeof(tft_type_T),
            nw_tft, sizeof(tft_type_T));

    res_alloc_req->low_priority_signaling =
      ds_3gpp_get_low_priority_signaling_bit(pdn_context_p);

    DS_LTE_MSG1_HIGH("Low priority signaling bit is %d",
                    res_alloc_req->low_priority_signaling);
    /*-----------------------------------------------------------------------
      Send out the NW request. 
    -----------------------------------------------------------------------*/
    if((ds3g_msh_alloc_bearer_req_ex(res_alloc_req,subs_id)) == DS3G_FAILURE)
    {
      DS_LTE_MSG0_ERROR("DS3G Bearer alloc req failed"); 
      if(ds_flow_manager_update_precedence_bit_mask(
               pdn_context_p,flow_context_p,FALSE,FALSE,TRUE)==FALSE)
      {
        DS_LTE_MSG0_LOW("Error in clearing precedence bit mask");
      }
      result=ds_flow_manager_remove_pending_flow_context(
                                 pdn_context_p,flow_context_p);
      if (result == FALSE)
      {
        DS_LTE_MSG0_LOW("Could not remove Pending Flow Context");
      }
      ds_flow_context_release_flow_context(flow_context_p);
      modem_mem_free(res_alloc_req, MODEM_MEM_CLIENT_DATA_CRIT);
      return -1;
    }/* if ds3g_msh_alloc_bearer failed */
    ds_flow_context_set_flow_state(flow_context_p,
                           DS_FLOW_CONTEXT_STATE_PEND_ALLOC_RSP);
  } /* flow_context_p->state == DS_FLOW_CONTEXT_STATE_COMING_UP */
  /*-------------------------------------------------------------------------
    Handling in the case of a flow modification.
  -------------------------------------------------------------------------*/
  if (flow_context_p->ds_flow_context_dyn_p->state == 
      DS_FLOW_CONTEXT_STATE_PEND_MODIFY_RSP)
    {
    /* Flow should always have a bearer*/
    if(flow_context_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr == NULL)
    {
      DATA_ERR_FATAL("flow context does not have bearer flow manager");
      modem_mem_free(res_alloc_req, MODEM_MEM_CLIENT_DATA_CRIT);
      return -1;
    }
    bearer_context_p = flow_context_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr;
    if(bearer_context_p == NULL)
    {
      DS_LTE_MSG0_ERROR("bearer context is NULL");
      modem_mem_free(res_alloc_req, MODEM_MEM_CLIENT_DATA_CRIT);
      return -1;
    }
    /*-----------------------------------------------------------------------
      Fill in the required parameters. 
    -----------------------------------------------------------------------*/
    def_bearer_p=(ds_bearer_context_s *)
                      pdn_context_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;
    if(def_bearer_p ==  NULL)
    {
      DS_LTE_MSG0_ERROR("defualt bearer is NULL");
      modem_mem_free(res_alloc_req, MODEM_MEM_CLIENT_DATA_CRIT);
      return -1;
    }
    eps_bearer_p = (ds_eps_bearer_context_s *)\
        (def_bearer_p->ds_bearer_context_dyn_p->vobj_data_ptr_tbl[DS_3GPP_EPS_VOBJ]);
 

    if(!ds_eps_bearer_cntxt_validate_bearer_cntxt(eps_bearer_p))
    {  
      modem_mem_free(res_alloc_req, MODEM_MEM_CLIENT_DATA_CRIT);
      return -1;
    }
    res_alloc_req->sdf_id   = flow_context_p->sdf_id;
    res_alloc_req->lbi      = eps_bearer_p->ds_eps_bearer_context_dyn_p->eps_bearer_id;
    res_alloc_req->protocol_config_options.valid = FALSE;
    memscpy(&res_alloc_req->ul_dl_tft, sizeof(res_alloc_req->ul_dl_tft),
	     nw_tft, sizeof(tft_type_T));

    res_alloc_req->low_priority_signaling =
      ds_3gpp_get_low_priority_signaling_bit(pdn_context_p);

    DS_LTE_MSG1_HIGH("Low priority signaling bit is %d",
                    res_alloc_req->low_priority_signaling);
    /*-----------------------------------------------------------------------
      Send out the NW request. 
    -----------------------------------------------------------------------*/
    if((ds3g_msh_alloc_bearer_req_ex(res_alloc_req ,subs_id) == DS3G_FAILURE))
    {
      DS_LTE_MSG0_ERROR("DS3G Bearer alloc req failed"); 
      if(ds_flow_manager_update_precedence_bit_mask(
                 pdn_context_p,flow_context_p,FALSE,FALSE,TRUE)==FALSE)
      {
        DS_LTE_MSG0_LOW("Error in clearing precedence bit mask");
      }
      modem_mem_free(res_alloc_req, MODEM_MEM_CLIENT_DATA_CRIT);
      return -1;
    }
    /*---------------------------------------------------------------------
      Remove the flow context from bearer flow manager
    ---------------------------------------------------------------------*/
    bflow_manager_p =(ds_bearer_flow_manager_s *)
                    (flow_context_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr);
    if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
    {
      modem_mem_free(res_alloc_req, MODEM_MEM_CLIENT_DATA_CRIT);
      return -1;
    }
    bearer_context_p = bflow_manager_p->ds_bearer_flow_manager_dyn_p->bearer_ptr;

    if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
    {
      modem_mem_free(res_alloc_req, MODEM_MEM_CLIENT_DATA_CRIT);
      return -1;
    }

    if(ds_bearer_flow_manager_remove_flow_context_from_flow_manager(
                           bearer_context_p, flow_context_p) == FALSE)
    {
      DS_LTE_MSG0_LOW("Error when remove flow context from Bflow mgr");
    }

    //Update the bearer context with the sdf id so that if ever IRAT into GW;
    //we have a way of associating the flow with the bearer.
    bearer_context_p->ds_bearer_context_dyn_p->sdf_id = flow_context_p->sdf_id;

    /*---------------------------------------------------------------------
      Move the flow context from bearer flow manager to PDN pending context
    ---------------------------------------------------------------------*/
    if(ds_flow_manager_add_pending_flow_context(
                           pdn_context_p,flow_context_p) == FALSE)
    {
      DS_LTE_MSG0_LOW("Error when adding pending flow context to flow mgr");
    }
    ds_flow_context_set_flow_state(flow_context_p,
                DS_FLOW_CONTEXT_STATE_PEND_ADD_RSP);
  }/* flow_context_state->state == DS_FLOW_CONTEXT_STATE_PEND_MODIFY_RSP */

  /*---------------------------------------------------------------------
    Keep a copy of the UE request to NW.
  ---------------------------------------------------------------------*/
  memscpy(&(flow_context_p->ds_flow_context_dyn_p->tft_mod), sizeof(tft_type_T),
           nw_tft, sizeof(tft_type_T));
  modem_mem_free(res_alloc_req, MODEM_MEM_CLIENT_DATA_CRIT);
  return 0;
}/* ds_eps_flow_manager_bearer_alloc_req*/

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_BEARER_ALLOC_ABORT_REQ

DESCRIPTION
  Send out the bearer allocation abort request.

PARAMETERS
  flow_context_p   -  Pointer to Flow context to be aborted.

DEPENDENCIES
  None.

RETURN VALUE
  int              - -1 if it fails, 0 if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
int ds_eps_flow_manager_bearer_alloc_abort_req
(
  ds_flow_context_s             *flow_context_p
)
{

  cm_res_alloc_abort_s_type        alloc_abort;   
  ds_pdn_context_s                 *pdn_context_p = NULL; 
  sys_modem_as_id_e_type           subs_id  = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&alloc_abort, 0, sizeof(cm_res_alloc_abort_s_type));
  if(flow_context_p == NULL)
  {
    DS_LTE_MSG0_ERROR("Input ptr for flow context is NULL");
    return  -1;
  }

  /*---------------------------------------------------------------------
    Update the params and send out the NW request.
  ---------------------------------------------------------------------*/
  alloc_abort.sdf_id = flow_context_p->sdf_id;
  alloc_abort.esm_cause.valid = TRUE;
  alloc_abort.esm_cause.esm_cause = NAS_ESM_REGULAR_DEACTIVATION;
  alloc_abort.esm_local_cause.valid = FALSE;
 
  if( ds3g_msh_abort_bearer_req_ex((void *)(&alloc_abort),subs_id) 
       == DS3G_FAILURE )
  {
    DS_LTE_MSG1_ERROR("DS3G Bearer Alloc Abort req failed for sdf_id:0x%x",
                      flow_context_p->sdf_id); 
    /*---------------------------------------------------------------------
      Do a local clean up of the flow context and return success.
    ---------------------------------------------------------------------*/
    flow_context_p = ds_flow_manager_find_pending_flow_context(
                                  &pdn_context_p,flow_context_p->sdf_id);
   if(!ds_flow_context_validate_flow(flow_context_p))
    {
      return 0;
    }

  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p,&subs_id))
    {
      return 0;
    } 
    /*---------------------------------------------------------------------
      Found the pending flow context. Remove the flow context.
    ---------------------------------------------------------------------*/
    if(ds_flow_manager_update_precedence_bit_mask(
                    pdn_context_p,flow_context_p,FALSE,FALSE,TRUE)==FALSE)
    {
      DS_LTE_MSG0_LOW("Error in clearing precedence bit mask");
    }
    if(ds_flow_manager_remove_pending_flow_context(
                  pdn_context_p,flow_context_p) == FALSE)
    {
      DS_LTE_MSG0_LOW("Could not remove Pending Flow Context");
    }
    ds_flow_context_release_flow_context(flow_context_p);
    return 0;
  } /* if abort_bearer_req failed */

  /*---------------------------------------------------------------------
    Update the Flow state
  ---------------------------------------------------------------------*/
  ds_flow_context_set_flow_state(flow_context_p,
                DS_FLOW_CONTEXT_STATE_PEND_ALLOC_ABORT_RSP);
  return 0;

}/* ds_eps_flow_manager_bearer_alloc_abort_req */


/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_NW_BEARER_MODIFY_IND

DESCRIPTION
  Handle the network requested bearer modification Indication.

PARAMETERS
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_flow_manager_nw_bearer_modify_ind
(
  cm_call_mode_info_s_type* mode_info_ptr,
  cm_call_id_type           call_id
)
{
  ds_pdn_context_s             *pdn_context_p = NULL;
  ds_bearer_context_s          *bearer_context_p = NULL;
  cm_lte_call_info_s_type      *lte_info_ptr = NULL;
  ds_3gpp_internal_cause_code_enum_type icause_code;
  boolean                      return_value = FALSE;
  qos_spec_type                app_qos;
  ds_bearer_flow_manager_s          *bflow_manager_p = NULL;
  ds_flow_context_s                 *flow_context_p = NULL;
  ds_bearer_flow_manager_s     *bflow_manager_d = NULL;
  ds_flow_context_s            *flow_context_d = NULL;
  ps_iface_type               *iface_ptr = NULL;
  ip_version_enum_type         ip_type = IP_V4;
  int16                        ds_errno = 0;
  ps_flow_modify_param_type    ps_flow_param;
  int32                        ps_modify_ret_val = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_LTE_MSG0_HIGH("EPS Flow manager received NW Bearer Modification Indication");

  icause_code = DS_3GPP_INTERNAL_CC_INVALID;
  /*-------------------------------------------------------------------------
    get the LTE info 
  -------------------------------------------------------------------------*/
  lte_info_ptr =(cm_lte_call_info_s_type *)&(mode_info_ptr->info.lte_call);

  if(mode_info_ptr == NULL)
  {
    DS_LTE_MSG0_ERROR("Input parameter is NULL");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Get the bearer context and the PDN context
  -------------------------------------------------------------------------*/
  bearer_context_p= 
                ds_bearer_cntxt_get_bearer_from_call_id(call_id);
  /* Can bearer be in going down state and receive modification
     request? We should bring up the bearer */ 
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    lte_info_ptr->esm_cause.valid = TRUE;
    lte_info_ptr->esm_cause.esm_cause = NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT;
    return DS3G_FAILURE;
  }
  /*-------------------------------------------------------------------------
    Retrieve the PDN Context
  -------------------------------------------------------------------------*/
  pdn_context_p = (ds_pdn_context_s*)bearer_context_p->ds_bearer_context_dyn_p->
     pdn_context_ptr; 
    

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    lte_info_ptr->esm_cause.valid = TRUE;
    lte_info_ptr->esm_cause.esm_cause = NAS_ESM_INSUFFICIENT_RESOURCES;
    return DS3G_FAILURE;
  }

  if(pdn_context_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_GOING_DOWN)
  {
    DS_LTE_MSG0_HIGH("Received Bearer Modification when PDN is going down");
    lte_info_ptr->esm_cause.valid = TRUE;
    lte_info_ptr->esm_cause.esm_cause = NAS_ESM_INSUFFICIENT_RESOURCES;
    return DS3G_FAILURE;
  } /* PDN context is going down */

#ifndef FEATURE_SUPPORT_TFT_ON_DEFAULT_BEARER

  /*-------------------------------------------------------------------------
    Verify if this is the default bearer.
   -------------------------------------------------------------------------*/
  if(pdn_context_p->ds_pdn_context_dyn_p->def_bearer_context_ptr == bearer_context_p)
  {
    DS_LTE_MSG0_HIGH("Bearer Modification received on default bearer");

    if(lte_info_ptr->ul_dl_tft.valid == TRUE)
    {
      DS_LTE_MSG0_ERROR("TFT filters will not be supported on default bearer");      
    }

    if( (lte_info_ptr->sdf_qos.valid != TRUE) &&
        (lte_info_ptr->apn_ambr.valid != TRUE) )
    {
      DS_LTE_MSG0_HIGH("ds_eps_flow_manager_nw_bearer_modify_ind:"
                       "QOS or AMBR not specified for default bearer");
      return DS3G_SUCCESS;
    }
    
    if(lte_info_ptr->sdf_qos.valid == TRUE)
    {
      memset(&app_qos, 0, sizeof(qos_spec_type));
      DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

      /* Find the bearer flow manager */
      bflow_manager_d = ds_bearer_flow_manager_find_flow_manager(
                                                         bearer_context_p);

      if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_d))
      {
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);	
        return FALSE;
      }

      flow_context_d = list_peek_front(&(bflow_manager_d->
                                     ds_bearer_flow_manager_dyn_p->flow_context));
      if(ds_flow_context_validate_flow(flow_context_d))
      {
        if(ds_flow_context_ps_flow_get_filters(flow_context_d, FALSE,
                                         &app_qos) == FALSE)     
        {
          DS_3GPP_MSG0_LOW("Could not retrieve previous filters");
          *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
           DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);	  	  
           return FALSE; 
        }
      }

      if(ds_bearer_flow_manager_convert_data_rate(bearer_context_p,
                                             mode_info_ptr,
                                             &app_qos,
                                             &icause_code) == FALSE)
      {
        DS_LTE_MSG0_LOW("NW modified data  rate not accepted");
        if(ds_eps_flow_manager_convert_cause_code(mode_info_ptr,
                                             &icause_code) == FALSE)
        {
          DS_LTE_MSG0_LOW("Cause code conversion failed");
        }
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        return DS3G_FAILURE;
      }
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    }/* sdf_qos.valid == TRUE*/

    if(lte_info_ptr->apn_ambr.valid == TRUE)
    {
      /*--------------------------------------------------------------------
       Store the APN AMBR in Mode specific context
      ---------------------------------------------------------------------*/
      if (ds_eps_flow_manager_update_apn_ambr(
                                              lte_info_ptr, 
                                              pdn_context_p
                                             ) == FALSE)
      {
        lte_info_ptr->esm_cause.valid = TRUE;
        lte_info_ptr->esm_cause.esm_cause =NAS_ESM_INSUFFICIENT_RESOURCES;
        return DS3G_FAILURE;
      }  
    } /* apn_ambr.valid == TRUE */ 
      return DS3G_SUCCESS;
  }/* if default bearer */

#endif //#ifndef FEATURE_SUPPORT_TFT_ON_DEFAULT_BEARER

  
  /*-------------------------------------------------------------------------
    Check the flags before passing it to process NW Bearer Modification
  -------------------------------------------------------------------------*/
  if ( (lte_info_ptr->sdf_qos.valid != TRUE) &&
       (lte_info_ptr->ul_dl_tft.valid != TRUE) &&
       (lte_info_ptr->apn_ambr.valid != TRUE))
  {
    DS_LTE_MSG0_HIGH("ds_eps_flow_manager_nw_bearer_modify_ind:"
                     "QOS/TFT/AMBR not specified for bearer");
    return DS3G_SUCCESS;
  }

  /*-------------------------------------------------------------------------
    Update the APN AMBR 
  -------------------------------------------------------------------------*/
  if(lte_info_ptr->ul_dl_tft.valid != TRUE)
  {
    DS_LTE_MSG0_HIGH("TFT is not valid for bearer modification");
    if(lte_info_ptr->apn_ambr.valid == TRUE)
    {
      DS_LTE_MSG0_HIGH("Update APN AMBR for dedicated bearer mod");
      /*--------------------------------------------------------------------
       Store the APN AMBR in Mode specific context
      ---------------------------------------------------------------------*/
      if (ds_eps_flow_manager_update_apn_ambr(
                                              lte_info_ptr, 
                                              pdn_context_p
                                             ) == FALSE)
      {
        lte_info_ptr->esm_cause.valid = TRUE;
        lte_info_ptr->esm_cause.esm_cause =NAS_ESM_INSUFFICIENT_RESOURCES;
        return DS3G_FAILURE;
      }       
    } /* apn_ambr.valid == TRUE */ 

    if(lte_info_ptr->sdf_qos.valid == TRUE)  
    {
      DS_LTE_MSG0_HIGH("Qos with out Tft for dedicated bearer mod");
      /*--------------------------------------------------------------------
       Store the SDF QOS in Mode specific context
      ---------------------------------------------------------------------*/
      memset(&app_qos, 0, sizeof(qos_spec_type));

      app_qos.rx.fltr_template.list_ptr = 
        (ip_filter_type *)modem_mem_calloc(MAX_PACKET_FILTERS,
                                         sizeof(ip_filter_type),
                                         MODEM_MEM_CLIENT_DATA);
      if(app_qos.rx.fltr_template.list_ptr == NULL)
      {
        DS_LTE_MSG0_LOW("Heap Memory Allocation failed");
        lte_info_ptr->esm_cause.valid = TRUE;
        lte_info_ptr->esm_cause.esm_cause =  NAS_ESM_INSUFFICIENT_RESOURCES;
        return DS3G_FAILURE;
      }

      app_qos.tx.fltr_template.list_ptr = 
        (ip_filter_type *)modem_mem_calloc(MAX_PACKET_FILTERS,
                                         sizeof(ip_filter_type),
                                         MODEM_MEM_CLIENT_DATA);
      if(app_qos.tx.fltr_template.list_ptr == NULL)
      {
        DS_LTE_MSG0_LOW("Heap Memory Allocation failed");
        lte_info_ptr->esm_cause.valid = TRUE;
        lte_info_ptr->esm_cause.esm_cause =  NAS_ESM_INSUFFICIENT_RESOURCES;
        return DS3G_FAILURE;
      }  

      DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

      bflow_manager_d = ds_bearer_flow_manager_find_flow_manager(
                                                         bearer_context_p);

      if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_d))
      {
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);	
        return FALSE;
      }

      flow_context_d = list_peek_front(&(bflow_manager_d->
                                     ds_bearer_flow_manager_dyn_p->flow_context));
      if(ds_flow_context_validate_flow(flow_context_d))
      {
        if(ds_flow_context_ps_flow_get_filters(flow_context_d, FALSE,
                                         &app_qos) == FALSE)    
        {
          DS_3GPP_MSG0_LOW("Could not retrieve previous filters");
          icause_code = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
          DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);		  
          return FALSE; 
        }
      }

      if(ds_bearer_flow_manager_convert_data_rate(bearer_context_p,
                                             mode_info_ptr,
                                             &app_qos,
                                             &icause_code) == FALSE)
      {
        DS_LTE_MSG0_LOW("NW modified data rate not accepted");
        if(ds_eps_flow_manager_convert_cause_code(mode_info_ptr,
                                                  &icause_code) == FALSE)
        {
          DS_LTE_MSG0_LOW("Cause code conversion failed");
        }
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        return DS3G_FAILURE;
      }
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

      ds_eps_bearer_cntxt_send_QOS_indication(bearer_context_p);

      /*--------------------------------------------------------------------
        Find the Bearer flow manager and update the flow contexts.
      ---------------------------------------------------------------------*/
      bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(
                                                         bearer_context_p);
      if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
      {
        return FALSE;
      }
      flow_context_p = list_peek_front(&(bflow_manager_p->
                                ds_bearer_flow_manager_dyn_p->flow_context));
      while(flow_context_p != NULL)
      {

      /*-----------------------------------------------------------------------
          Update the Flow with the filter information.
       -----------------------------------------------------------------------*/ 
        memset((void *)&ps_flow_param,0, sizeof(ps_flow_modify_param_type));
        ps_flow_param.flow_validate_f_ptr = NULL;
        ps_flow_param.fltr_validate_f_ptr = NULL;
        ps_flow_param.fltr_priority       = PS_IFACE_IPFLTR_PRIORITY_FCFS;
        ps_flow_param.subset_id           = 5; 

        DS_LTE_MSG0_LOW("Entering to modify set granted flow");
        ds_flow_context_ps_flow_set_granted_flow(flow_context_p);
        app_qos.rx.fltr_template.num_filters = 0;
        app_qos.tx.fltr_template.num_filters = 0;
        app_qos.field_mask = 0;

        if(ds_flow_context_ps_flow_get_filters(flow_context_p, FALSE,
                                         &app_qos) == FALSE)
        {
          DS_LTE_MSG0_LOW("3GPP iface is not present");
          lte_info_ptr->esm_cause.valid = TRUE;
          lte_info_ptr->esm_cause.esm_cause =  NAS_ESM_INSUFFICIENT_RESOURCES;
          return DS3G_FAILURE;
        }
      /* Updating the available filters to Flow param  */

        ps_flow_param.qos_spec = &app_qos;

        if(ds_flow_context_retrieve_iface_ip_type(flow_context_p,
                                                    &ip_type,
                                                    &iface_ptr) == FALSE)
        {
          DS_LTE_MSG0_ERROR("3GPP iface is not present");
          lte_info_ptr->esm_cause.valid = TRUE;
          lte_info_ptr->esm_cause.esm_cause =  NAS_ESM_INSUFFICIENT_RESOURCES;
          return DS3G_FAILURE;
        }
        if( app_qos.rx.fltr_template.num_filters > 0 || app_qos.tx.fltr_template.num_filters > 0)
        {
          DS_LTE_MSG0_LOW("Modify PS Iface flow");

          ps_modify_ret_val = ps_iface_modify_flow(iface_ptr,
                   (flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr),&ps_flow_param, &ds_errno);

          if(ps_modify_ret_val == 0)
          {
            if(ps_iface_modify_flow_accepted(&(flow_context_p->ds_flow_context_dyn_p->
                                ds_iface_ptr->ps_iface),
                                flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr,
                                    PS_EIC_NOT_SPECIFIED,
                                    &ds_errno)!= 0)
            {
              DS_LTE_MSG1_ERROR("modify_flow_accepted failed :%d",ds_errno);
            }
            else
            {
              DS_LTE_MSG1_LOW("modify_flow_accepted passed:%d",ds_errno);
            }
          }
        }
        else
        {
          if(ps_iface_modify_flow_rejected(&(flow_context_p->ds_flow_context_dyn_p->
                                  ds_iface_ptr->ps_iface),
                                  flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr,
                                    PS_EIC_NOT_SPECIFIED,
                                    &ds_errno) != 0)
          {
            DS_LTE_MSG1_ERROR("modify_flow_rejected failed:%d",ds_errno);     
          }
          else
          {
            DS_LTE_MSG1_LOW("modify_flow_accepted passed:%d",ds_errno);
          }
        }
        
        flow_context_p = list_peek_next(&(bflow_manager_p->
                                   ds_bearer_flow_manager_dyn_p->flow_context),
                                &(flow_context_p->link));
      } /* while */  

      modem_mem_free(app_qos.rx.fltr_template.list_ptr,MODEM_MEM_CLIENT_DATA);
      modem_mem_free(app_qos.tx.fltr_template.list_ptr,MODEM_MEM_CLIENT_DATA);
      app_qos.rx.fltr_template.list_ptr = NULL;
      app_qos.tx.fltr_template.list_ptr = NULL;
    } /* sdf_qos.valid == TRUE */ 
    return DS3G_SUCCESS;
  } /* TFT Not Valid */


  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  if(lte_info_ptr->sdf_qos.valid == FALSE)
  {
    return_value = ds_flow_manager_process_tft_for_nw_bearer_mod(
                                  pdn_context_p,
                                  bearer_context_p,
                                  &lte_info_ptr->ul_dl_tft,
                                  NULL,
                                  &icause_code,
                                  TRUE);
  }
  else
  {
    return_value = ds_flow_manager_process_tft_for_nw_bearer_mod(
                                  pdn_context_p,
                                  bearer_context_p,
                                  &lte_info_ptr->ul_dl_tft,
                                  mode_info_ptr,
                                  &icause_code,
                                  TRUE);
  }
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

  /* Propagate cause code to NAS in case of failure */
  if( return_value == DS3G_FAILURE )
  {
    if(ds_eps_flow_manager_convert_cause_code(mode_info_ptr,&icause_code)
                                                                 ==FALSE)
    {
      DS_LTE_MSG0_LOW("Cause code conversion failed");
    }
  }

  /*-------------------------------------------------------------------------
    Update the APN AMBR if NW passed this value. 
  -------------------------------------------------------------------------*/
  if ( return_value == DS3G_SUCCESS)
  {
    if (bearer_context_p != NULL)
    {
      ds_eps_bearer_cntxt_send_QOS_indication(bearer_context_p);
    }

    if (lte_info_ptr->apn_ambr.valid == TRUE)
  {
    /*--------------------------------------------------------------------
     Store the APN AMBR in Mode specific context
    ---------------------------------------------------------------------*/
    (void) ds_eps_flow_manager_update_apn_ambr(lte_info_ptr, pdn_context_p); 
  } /* if apn_ambr is valid */
  }

  /*--------------------------------------------------------------------
    LTE does not need lower layer to confirm modify, so send flow accept
    and enable filtering right away
  ---------------------------------------------------------------------*/  
  (void)ds_flow_manager_accept_modify_enable_filter(bearer_context_p);
  return return_value;                                
}/*ds_eps_flow_manager_nw_bearer_modify_ind */


/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_HANDLE_CREATE_TFT

DESCRIPTION
  Handle the create TFT opcode sent on the default bearer.

PARAMETERS
  pdn_context_p        (in) -  PDN context info.
  bearer_context_p     (in) -  Bearer context info.
  nw_tft_p             (in) -  Traffic Flow Template.
  mode_info_p          (in) -  Extended CM information about the operation.
  cause_code_p         (out) -  Cause code information.
 
DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS or DS3G_FAILURE.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_flow_manager_handle_create_tft
(
  ds_pdn_context_s         *pdn_context_p,
  ds_bearer_context_s      *bearer_context_p,
  const tft_type_T         *nw_tft_p,
  const cm_call_mode_info_s_type  *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{    
  int                          result = DS3G_FAILURE;    

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
     return FALSE; 
  }

  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);


    result = ds_flow_manager_add_filter_to_tft( pdn_context_p,
                                                bearer_context_p,
                                                NULL,
                                                (tft_type_T *)nw_tft_p,
                                                mode_info_p,
                                                cause_code_p,
                                                TRUE);
    if (result != DS3G_SUCCESS )
    {      
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);     
      return DS3G_FAILURE;
    }

     DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

return DS3G_SUCCESS;

}

/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_HANDLE_DELETE_EXISTING_TFT

DESCRIPTION
  Handle the delete existing TFT opcode sent on the default bearer.

PARAMETERS
  pdn_ptr        (in) -  Ptr to the PDN context.
  bearer_ptr     (in) -  Ptr to the bearer context.
  nw_tft         (in) -  Ptr to the Traffic Flow Template.
  mode_info_p    (in) -  Extended CM information about the operation.
  cause_code_p   (out) -  Cause code information.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS or DS3G_FAILURE.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_flow_manager_handle_delete_existing_tft
(
  ds_pdn_context_s         *pdn_context_p,
  ds_bearer_context_s      *bearer_context_p,
  const tft_type_T         *nw_tft_p,
  const cm_call_mode_info_s_type  *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{      
  int                          i;
  int                          result = DS3G_FAILURE;
  ps_flow_type*             ps_flow_ptr = NULL; 
  
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }


  if (FALSE == bearer_context_p->ds_bearer_context_dyn_p->is_default)
  {
     DS_LTE_MSG0_ERROR("Delete existing TFT is supported only on the default bearer");  
     *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_TFT;
     return DS3G_FAILURE;       
  }
  
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

  do
  {    

    //Release all the flow contexts associated with this bearer.
    result = ds_flow_manager_free_nw_flow_context(bearer_context_p);    
    if (result != DS3G_SUCCESS)
    {
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
      break;
    }

    //Reset the capability of the default flow from PS_FLOW_CAPABILITY_DATA_DISALLOWED.
    //No packet on default flow will be dropped.  
    for (i = 0; i < DS_3GPP_MAX_IFACE_PER_PDN_CONTEXT; i++)
    {
      ds_3gpp_iface_s *ds_iface_p = pdn_context_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[i];
      if (ds_iface_p != NULL)
      {              
        ps_iface_type *ps_iface = &(ds_iface_p->ps_iface);        
        ps_flow_ptr = PS_IFACE_GET_DEFAULT_FLOW(ps_iface);
        if (ps_flow_ptr != NULL)
        {
           PS_FLOW_RESET_CAPABILITY(ps_flow_ptr, PS_FLOW_CAPABILITY_DATA_DISALLOWED); 
        }
      }
    }

    result = DS3G_SUCCESS;
    
  } while(0);
  
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  return result;

}


/*===========================================================================
FUNCTION DS_EPS_FLOW_MANAGER_INIT

DESCRIPTION
  Register EPS specific functions

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_flow_manager_init( void )
{
  ds_3gpp_flow_manager_ftbl_s  *flow_manager_ftbl_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Initialize EPS flow manager Function table
  -------------------------------------------------------------------------*/
  flow_manager_ftbl_p = &(ds_flow_manager_ftbl[DS_3GPPI_SYS_MODE_LTE]);
  memset(
         (void*)flow_manager_ftbl_p,
         0x0,
         (sizeof(ds_3gpp_flow_manager_ftbl_s))
        );

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, checkNetworkInput,
                     ds_eps_flow_manager_check_nw_input);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, convertCauseCodes,
                     ds_eps_flow_manager_convert_cause_code);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, TearDownSecBearer,
                     ds_eps_flow_manager_tear_down_sec_bearer);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, NwBearerAllocRej,
                     ds_eps_flow_manager_bearer_alloc_rej);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, NwBearerAllocFail,
                     ds_eps_flow_manager_bearer_alloc_fail);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, NwBearerModifyRej,
                     ds_eps_flow_manager_bearer_modify_rej);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, NwBearerModifyInd,
                     ds_eps_flow_manager_bearer_modify_ind);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, NwBearerAllocationInd,
                     ds_eps_flow_manager_bearer_allocation_ind);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, checkSdfId,
                     ds_eps_flow_manager_check_sdf_id);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, UeBearerAllocReq,
                     ds_eps_flow_manager_bearer_alloc_req);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, UeBearerAllocAbortReq,
                     ds_eps_flow_manager_bearer_alloc_abort_req);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, UeBearerModifyReq,
                     ds_eps_flow_manager_bearer_modify_req);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, ProcessCreateTFT,
                     ds_eps_flow_manager_handle_create_tft);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, ProcessDeleteTFT,
                     ds_eps_flow_manager_handle_delete_existing_tft);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, allocate_eps_flow_manager,
                     ds_eps_flow_manager_alloc_eps_flow_manager);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_LTE, free_eps_flow_manager,
                     ds_eps_flow_manager_free_eps_flow_manager);

} /* ds_eps_flow_manager_init */

/*===========================================================================
FUNCTION ds_eps_flow_manager_alloc_eps_flow_manager

DESCRIPTION
  This function allocates dynamic memory for the index in the eps flow manager
  table

PARAMETERS
int alloc_index
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE 
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_eps_flow_manager_alloc_eps_flow_manager
(
   int alloc_index
)
{
  ds_eps_flow_manager_dyn_s *eps_flow_manager_dyn_p = NULL;
  boolean ret_val = FALSE;
 /*------------------------------------------------------------------------*/
  if((alloc_index < 0) || (alloc_index >= DS_3GPP_MAX_PDN_CONTEXT))
  {
    DS_LTE_MSG1_LOW("Alloc index out of range:%d",alloc_index);
    return ret_val;
  }

  eps_flow_manager_dyn_p = ds_eps_flow_manager_tbl[alloc_index].
                                                    eps_flow_manager_dyn_p;

  if(eps_flow_manager_dyn_p == NULL)
  {
    eps_flow_manager_dyn_p = (ds_eps_flow_manager_dyn_s *)
    modem_mem_alloc(sizeof(ds_eps_flow_manager_dyn_s),
                    MODEM_MEM_CLIENT_DATA);
    if(eps_flow_manager_dyn_p != NULL)
    {
      memset(eps_flow_manager_dyn_p,0,
             sizeof(ds_eps_flow_manager_dyn_s));
      eps_flow_manager_dyn_p->cookie = EPS_FLOW_MGR_VALID_COOKIE;
      eps_flow_manager_dyn_p->index = alloc_index;
      DS_LTE_MSG1_MED("EPS flow mgr allocated at index:%d",
                       eps_flow_manager_dyn_p->index);
      ds_eps_flow_manager_tbl[alloc_index].eps_flow_manager_dyn_p = 
        eps_flow_manager_dyn_p;
      ret_val = TRUE;
    }
    else
    {

      DS_LTE_MSG1_LOW("Failure to allocate EPS FLOW MGR at index:%d",
                         alloc_index);

    }

  }
  else
  {
    DS_LTE_MSG1_LOW("EPS FLOW MGR dyn memory already allocated at index:%d",
                       alloc_index);
  }

  return ret_val;
}

/*===========================================================================
FUNCTION ds_eps_flow_manager_validate_eps_flow_manager

DESCRIPTION
  This function validates dynamic memory for the index in the eps 
  flow manager table

PARAMETERS
int alloc_index
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE 
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_eps_flow_manager_validate_flow_manager
(
   ds_eps_flow_manager_s *eps_flow_manager_p
)
{
    boolean ret_val = FALSE;
 /*------------------------------------------------------------------------*/
    if(eps_flow_manager_p == NULL)
    {
      DS_LTE_MSG0_ERROR("Input EPS BFLOW MGR is NULL");
      return ret_val;
    }

    if((eps_flow_manager_p->eps_flow_manager_dyn_p != NULL) &&
       (eps_flow_manager_p->eps_flow_manager_dyn_p->cookie 
        == EPS_FLOW_MGR_VALID_COOKIE))
    {
      DS_LTE_MSG1_LOW("EPS FLOW MGR is allocated at index:%d",
                       eps_flow_manager_p->eps_flow_manager_dyn_p->index);
      ret_val = TRUE;
    }

    return ret_val;
}

/*===========================================================================
FUNCTION ds_eps_flow_manager_free_eps_flow_manager

DESCRIPTION
  This function frees dynamic memory for the index in the
  eps flow manager table

PARAMETERS
int index
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE 
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_eps_flow_manager_free_eps_flow_manager
(
   int index
)
{
   boolean ret_val = FALSE;
   ds_eps_flow_manager_s *eps_flow_manager_p = NULL;
 /*------------------------------------------------------------------------*/

   if((index < 0) || (index >= DS_3GPP_MAX_PDN_CONTEXT))
   {
     DS_LTE_MSG1_LOW("Index:%d is out of bounds",index);
     return ret_val;
   }

   eps_flow_manager_p = &ds_eps_flow_manager_tbl[index];

   if(eps_flow_manager_p->eps_flow_manager_dyn_p != NULL)
   {
     modem_mem_free(eps_flow_manager_p->eps_flow_manager_dyn_p,MODEM_MEM_CLIENT_DATA);
     eps_flow_manager_p->eps_flow_manager_dyn_p = NULL;
     ret_val = TRUE;
   }

   return ret_val;

}

#endif /*FEATURE_DATA_LTE */

