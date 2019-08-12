/*!
  @file
  ds_umts_flow_manager.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/umtsps/src/ds_umts_flow_manager.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/20/13   nd      TFT support for Default bearer.
04/15/10   vrk     Initial version.
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include <stringl/stringl.h>

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#include "dstask.h"
#include "ds3gmgr.h"
#include "ds_3gppi_utils.h"
#include "ds3gmshif.h"
#include "dsumtspdpreg.h"
#include "ds_flow_control.h"
#include "ran.h"
#include "cm.h"
#include "cm_gw.h"
#include "ds_umts_flow_manager.h"
#include "ds_3gpp_flow_manager.h"
#include "ds_3gpp_nv_manager.h"
#include "ds_umts_bearer_context.h"
//#include "ds_eps_bearer_flow_manager.h"
#include "ds_3gpp_bearer_flow_manager.h"
#include "ds_umts_bearer_flow_manager.h"
#include "ds_3gpp_apn_table.h"
#include "modem_mem.h"
#include "ds_3gpp_dyn_mem_hdlr.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_dsd_apm_ext_i.h"
#include "ps_iface_flow.h"
#include "ps_flow.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_CLEAN_UP

DESCRIPTION
  Clean up the state in case of error scenarios.

PARAMETERS
  pdn_context_p    -  Pointer to PDN context to be modified.
  flow_context_p   -  Pointer to Flow context to be modified.
  bearer_clean_up_flag  -  boolean, do we need to clean bearer context?  

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_umts_flow_manager_clean_up
(
  ds_pdn_context_s    *pdn_context_p,
  ds_flow_context_s   *flow_context_p,
  ds_bearer_context_s *bearer_cntx_p  
)
{ 
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  if(ds_flow_manager_update_precedence_bit_mask(
                       pdn_context_p,flow_context_p,FALSE,FALSE,TRUE)==FALSE)
  {
    DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
  }

  if(bearer_cntx_p != NULL)
  {
    ds_bearer_cntxt_free_bearer_inst(bearer_cntx_p);
    ds_bearer_cntxt_return_bearer_to_free_pool(bearer_cntx_p);
  }
  
  ds_flow_context_release_flow_context(flow_context_p); 

  return;
}/* ds_umts_flow_manager_clean_up */

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_CHECK_NW_INPUT

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
boolean ds_umts_flow_manager_check_nw_input
(
  cm_call_mode_info_s_type* mode_info_ptr,
  boolean                   default_bearer,
  boolean                   bearer_alloc
)
{
#if 0
   cm_lte_call_info_s_type       *lte_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("UMTS Flow manager received Check NW Input");

  if(mode_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input parameter is NULL");
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
    DS_3GPP_MSG0_ERROR("Received qos with valid flag set to FALSE");
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
    DS_3GPP_MSG0_ERROR("Received TFT type with valid flag set to FALSE");
    lte_info_ptr->esm_cause.valid = TRUE;
    lte_info_ptr->esm_cause.esm_cause =NAS_ESM_INVALID_MANDATORY_INFORMATION;
    return DS3G_FAILURE;
  }

  if((lte_info_ptr->ul_dl_tft.tft_total_length < 3) ||
	  (lte_info_ptr->ul_dl_tft.tft_total_length > 257))
  {
    DS_3GPP_MSG1_ERROR("Received TFT with invalid length=%d",
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
    if(lte_info_ptr->ul_dl_tft.operation_code!=(byte)DS_3GPP_TFT_OPCODE_CREATE)
    {
      DS_3GPP_MSG1_ERROR("Received TFT with invalid operation:%d",
		               lte_info_ptr->ul_dl_tft.operation_code);
      lte_info_ptr->esm_cause.valid = TRUE;
      lte_info_ptr->esm_cause.esm_cause =
                                  NAS_ESM_SYNTACTICAL_ERR_IN_TFT_OPERATION;
      return DS3G_FAILURE;
    }

    if(lte_info_ptr->ul_dl_tft.e_bit != 0)
    {
      DS_3GPP_MSG0_ERROR("Received TFT with E bit set");
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
      DS_3GPP_MSG1_ERROR("Received TFT with invalid operation:%d",
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
    DS_3GPP_MSG1_ERROR("Received Invalid number of filters:%d",
		               lte_info_ptr->ul_dl_tft.num_filters);
    lte_info_ptr->esm_cause.valid = TRUE;
    lte_info_ptr->esm_cause.esm_cause =NAS_ESM_SYNTACTICAL_ERR_IN_TFT_OPERATION;
    return DS3G_FAILURE;
  }
#endif
  return DS3G_SUCCESS;
}/* ds_umts_flow_manager_check_nw_input*/

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_CONVERT_CAUSE_CODE

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
boolean ds_umts_flow_manager_convert_cause_code
(
  const cm_call_mode_info_s_type *mode_info_ptr,
  ds_3gpp_internal_cause_code_enum_type *ds_int_cause_ptr
)
{

  cm_gw_ps_call_info_s_type       *gw_ps_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((mode_info_ptr == NULL) || (ds_int_cause_ptr == NULL))
  {
    DS_3GPP_MSG0_ERROR("input params are not NULL");
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    get the UMTS call info
  -------------------------------------------------------------------------*/
  gw_ps_info_ptr =(cm_gw_ps_call_info_s_type *)&(mode_info_ptr->info.gw_ps_call);
 
  switch(*ds_int_cause_ptr)
  {
    case DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES:
      gw_ps_info_ptr->cause.ext_cause = SM_INSUFFICIENT_RESOURCES;
    break;

    case DS_3GPP_INTERNAL_FEATURE_NOT_SUPPORTED:
      gw_ps_info_ptr->cause.ext_cause = SM_FEATURE_NOT_SUPPORTED;
    break;

    case DS_3GPP_INTERNAL_QOS_NOT_ACCEPTED :
      gw_ps_info_ptr->cause.ext_cause = SM_QOS_NOT_ACCEPTED;
    break;

    case DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_TFT:
      gw_ps_info_ptr->cause.ext_cause = SM_SEMANTIC_ERR_IN_TFT;
    break;

    case DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT:
      gw_ps_info_ptr->cause.ext_cause = SM_SYNTACTIC_ERR_IN_TFT;
    break;

    case DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER:
      gw_ps_info_ptr->cause.ext_cause = SM_SEMANTIC_ERR_IN_FILTER;
    break;

    case DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER:
      gw_ps_info_ptr->cause.ext_cause = SM_SYNTACTIC_ERR_IN_FILTER;
    break;

    case DS_3GPP_INTERNAL_BEARER_WITH_NO_TFT_ALREADY_PRESENT:
      gw_ps_info_ptr->cause.ext_cause = SM_PDP_WITHOUT_TFT_ACTIVE;
    break;

    case DS_3GPP_INTERNAL_INVALID_MANDATORY_INFO:
      gw_ps_info_ptr->cause.ext_cause = SM_INVALID_MANDATORY_INFO;
    break;

    case DS_3GPP_INTERNAL_ACTIVATION_REJECTED:
      gw_ps_info_ptr->cause.ext_cause = SM_ACTIVATION_REJECTED_UNSPECIFIED;
    break;

    case DS_3GPP_INTERNAL_SERV_OPT_NOT_SUPPORTED:
      gw_ps_info_ptr->cause.ext_cause = SM_SERVICE_OPTION_NOT_SUPPORTED;
    break;
    default:
      DS_3GPP_MSG1_ERROR("Invalid cause code passed 0x%x",*ds_int_cause_ptr);
      return FALSE;
  }/* switch*/

  return TRUE;
}/* ds_umts_flow_manager_convert_cause_code*/

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_TEAR_DOWN_SEC_BEARER

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
boolean ds_umts_flow_manager_tear_down_sec_bearer
(
  ds_flow_context_s   *flow_context_p
)
{
  ds_bearer_flow_manager_s *bearer_flow_mgr_ptr = NULL;
  ds_bearer_context_s      *bearer_context_ptr = NULL;
  int16                     ps_errno;             /* Errno value */
  int                       ret_val;
  ds_pdn_context_s         *pdn_cntx_p = NULL;
  ds_3gpp_iface_s          *ds_iface_p = NULL;
  uint8                     bearer_index;
  boolean                   found_bearer_cntxt_ptr = FALSE;

  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return FALSE;
  }

  bearer_flow_mgr_ptr = (ds_bearer_flow_manager_s *)(flow_context_p->ds_flow_context_dyn_p
                                                     ->bearer_flow_mgr_ptr);

  if(!ds_bearer_flow_manager_validate_flow_manager(bearer_flow_mgr_ptr))
  {
    if( (ds_iface_p = flow_context_p->ds_flow_context_dyn_p->ds_iface_ptr) == NULL)
    {
      DS_3GPP_MSG0_ERROR("DS IFACE PTR is null for flow context");
      return FALSE;
    } 
    pdn_cntx_p = (ds_pdn_context_s*)(ds_iface_p->client_data_ptr);
    if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
    {
      return FALSE;
    }
    /*-----------------------------------------------------------------------
      If N/W has not yet accepted the secondary activation or modification
      is in progress bearer_flow_mgr_ptr will not be there in ds_flow_context_ptr.
      It will be in pending flow list.
    -----------------------------------------------------------------------*/
    for(bearer_index = 0;
        bearer_index < DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT;
        bearer_index++)
    {
      bearer_context_ptr =
         (ds_bearer_context_s*)pdn_cntx_p->ds_pdn_context_dyn_p->bearer_context_ptr_tbl[bearer_index];
      if(!ds_bearer_cntx_validate_bearer_context(bearer_context_ptr))
      {
        /*----------------------------------------------------------------------- 
          This entry in the bearer context table is NULL. Other entries could
          have non-NULL values.
        -----------------------------------------------------------------------*/
        continue;
      }
      if( (bearer_context_ptr != 
              pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr) &&               
          (bearer_context_ptr->ds_bearer_context_dyn_p->sdf_id == 
                               flow_context_p->sdf_id) )
      {
        found_bearer_cntxt_ptr = TRUE;
        DS_3GPP_MSG1_HIGH("Found bearer_cntxt_ptr with sdf_id 0x%x",
                           bearer_context_ptr->ds_bearer_context_dyn_p->sdf_id);
        break;
      }
    }
    if(found_bearer_cntxt_ptr == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Not able to find bearer_cntxt_ptr");
      return FALSE;
    }
  }
  else
  {
    bearer_context_ptr = bearer_flow_mgr_ptr->ds_bearer_flow_manager_dyn_p->bearer_ptr;

    if(!ds_bearer_cntx_validate_bearer_context(bearer_context_ptr))
    {
      return FALSE;
    }
  }
  /* issue phys link down cmd to tear down the bearer */
  ret_val = ps_phys_link_down_cmd(&(bearer_context_ptr->phys_link), 
                                  &ps_errno, NULL);
  if (ret_val == 0) 
  {
    /*-----------------------------------------------------------------------
      If the physical link is already down, initiate a call end. This part of
      the code is executed only when UE tears down secondary pdp if call is
      dormant
    -----------------------------------------------------------------------*/
    if(ds_bearer_cntxt_phys_link_down(&bearer_context_ptr->
                                      phys_link,NULL) != 0)
    {
      DS_3GPP_MSG0_ERROR("Bearer phys link down returned error");
    }
  } /* If the call is dormant */
  else if(ps_errno != DS_EWOULDBLOCK)
  {
    DS_3GPP_MSG0_ERROR("Phys link in invalid state while tear down sec ctx is called");
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_CHECK_SDF_ID

DESCRIPTION
  Handle the bearer call end.

PARAMETERS
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.
    
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_umts_flow_manager_check_sdf_id
(
  ds3g_call_info_cmd_type* call_info_ptr
)
{
#if 0 //VRK:Not used
   cm_call_mode_info_s_type     *mode_info_ptr = NULL;
   ds_pdn_context_s             *pdn_context_p = NULL;
   ds_bearer_context_s          *bearer_context_p = NULL;
   ds_flow_context_s            *flow_context_p = NULL;   
   ds_bearer_flow_manager_s     *bflow_manager_p = NULL;
   ps_extended_info_code_enum_type ps_eic = PS_EIC_NOT_SPECIFIED; 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("UMTS Flow manager received Check SDF ID from Call End");
  mode_info_ptr = &(call_info_ptr->mode_info);
  if(mode_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input parameter is NULL");
    return;
  } 

  bearer_context_p =
     ds_bearer_cntxt_get_bearer_from_call_id(call_info_ptr->call_id);
  
  /*---------------------------------------------------------------------
    Check if any pending request matches the SDF_ID.
  ---------------------------------------------------------------------*/
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);  

  /*-------------------------------------------------------------------------
    Get the bearer_flow_manager    
  -------------------------------------------------------------------------*/    
  bflow_manager_p=ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(bflow_manager_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("No Bearer Flow Manager associated with the Bearer");      
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return;
  }

  /*-----------------------------------------------------------------------
    TFT is filled from the flow context. There should be only one pending 
    flow context in the bearer flow manager list.
  -----------------------------------------------------------------------*/
  if(list_size(&(bflow_manager_p->flow_context)) != 1)
  {
    DS_3GPP_MSG0_ERROR("Found more than one pending flow context");      
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return;
  }
  flow_context_p = list_peek_front(&(bflow_manager_p->flow_context));
  if(flow_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Cound not find a pending flow context");      
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return;
  }    

  pdn_context_p = (ds_pdn_context_s*)bearer_context_p->pdn_context_ptr;
  if(pdn_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("PDN context pointer is not updated");
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return; 
  }

  if((flow_context_p->state!=DS_FLOW_CONTEXT_STATE_PEND_DEL_FLOW_RSP)&&
     (flow_context_p->state!=DS_FLOW_CONTEXT_STATE_PEND_DEL_RSP)&&
     (flow_context_p->state!=DS_FLOW_CONTEXT_STATE_PEND_ADD_RSP)&&
     (flow_context_p->state!=DS_FLOW_CONTEXT_STATE_PEND_REPLACE_RSP)&&
     (flow_context_p->state!=DS_FLOW_CONTEXT_STATE_PEND_NO_OP_RSP)&&
     (flow_context_p->state!=DS_FLOW_CONTEXT_STATE_COMING_UP)&&
     (flow_context_p->state!=DS_FLOW_CONTEXT_STATE_UP))
  {
    DS_3GPP_MSG1_HIGH("Received call_end in an unknown state:%x",
                  flow_context_p->state);
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return;
  }
  
  /*---------------------------------------------------------------------
    Found the pending flow context. Remove the pending flow context.
  ---------------------------------------------------------------------*/
  if(ds_flow_manager_update_precedence_bit_mask(
                 pdn_context_p,flow_context_p,FALSE,FALSE,FALSE)==FALSE)
  {
    DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
  }
  /*---------------------------------------------------------------------
    Update the bearer filter identifier mask- Not needed in UMTS.
    Unbind the flow.
  ---------------------------------------------------------------------*/
  bflow_manager_p = flow_context_p->bearer_flow_mgr_ptr;
  ds_flow_context_ps_flow_go_null_ind(flow_context_p, ps_eic);
  
  /*---------------------------------------------------------------------
    ds_flow_manager_remove_pending_flow_context- Not needed in UMTS.
    modify_req does not add the pending flow_context for UMTS
    Release flow context
  ---------------------------------------------------------------------*/
  
  ds_flow_context_release_flow_context(flow_context_p);
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect); 
  
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

#endif //VRK:Not used  
  return;
}/* ds_eps_flow_manager_check_sdf_id */

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_BEARER_ALLOC_REJ

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
void ds_umts_flow_manager_bearer_alloc_rej
(
  cm_call_mode_info_s_type* mode_info_ptr
)
{
#if 0
   ds_pdn_context_s             *pdn_context_p = NULL;
   ds_flow_context_s             *flow_context_p = NULL;
   cm_lte_call_info_s_type       *lte_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("UMTS Flow manager received Bearer allocation reject");
  if(mode_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input parameter is NULL");
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
    if(flow_context_p == NULL)
    {
      DS_3GPP_MSG1_ERROR("Could not find the flow context matching SDF ID 0x%x",
                                          lte_info_ptr->sdf_id);
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect); 
      return; 
    }

    if(pdn_context_p == NULL)
    {
      DS_3GPP_MSG0_ERROR("PDN context pointer is not updated");
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return; 
    }
    /*---------------------------------------------------------------------
      Found the pending flow context. Remove the flow context.
    ---------------------------------------------------------------------*/
    if(flow_context_p->state != DS_FLOW_CONTEXT_STATE_PEND_ALLOC_RSP)
    {
      DS_3GPP_MSG0_ERROR("Received unexpected Allocation Rejection");
    }
    if(ds_flow_manager_update_precedence_bit_mask(
                          pdn_context_p,flow_context_p,FALSE,TRUE)==FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
    }
    if(ds_flow_manager_remove_pending_flow_context(
                  pdn_context_p,flow_context_p) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Could not remove Pending Flow Context");
    }  
    ds_flow_context_release_flow_context(flow_context_p);
    /*---------------------------------------------------------------------
      If NW reason has to be sent to APP. Add here.
    --------------------------------------------------------------------*/
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return;
  } /* SDF_ID != 0xFFFF */ 
  DS_3GPP_MSG1_ERROR("Invalid SDF ID 0x%x",lte_info_ptr->sdf_id);
#endif
  return;
}/* ds_umts_flow_manager_bearer_alloc_rej */

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_BEARER_ALLOC_FAIL

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
void ds_umts_flow_manager_bearer_alloc_fail
(
  cm_call_mode_info_s_type* mode_info_ptr
)
{
#if 0
   ds_pdn_context_s             *pdn_context_p = NULL;
   ds_flow_context_s             *flow_context_p = NULL;
   cm_lte_call_info_s_type       *lte_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("UMTS Flow manager received Bearer allocation reject");
  if(mode_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input parameter is NULL");
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
    if(flow_context_p == NULL)
    {
      DS_3GPP_MSG1_ERROR("Could not find the flow context matching SDF ID 0x%x",
                                          lte_info_ptr->sdf_id);
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return; 
    }

    if(pdn_context_p == NULL)
    {
      DS_3GPP_MSG0_ERROR("PDN context pointer is not updated");
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return; 
    }
    /*---------------------------------------------------------------------
      Found the pending flow context. Remove the flow context.
    ---------------------------------------------------------------------*/
    if(flow_context_p->state != DS_FLOW_CONTEXT_STATE_PEND_ALLOC_ABORT_RSP)
    {
      DS_3GPP_MSG0_ERROR("Received unexpected allocation failure");
    }
    if(ds_flow_manager_update_precedence_bit_mask(
                          pdn_context_p,flow_context_p,FALSE,TRUE)==FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
    }
    if(ds_flow_manager_remove_pending_flow_context(
                  pdn_context_p,flow_context_p) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Could not remove Pending Flow Context");
    }
    ds_flow_context_release_flow_context(flow_context_p);
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return;
  } /* SDF_ID != 0xFFFF */ 

  DS_3GPP_MSG1_ERROR("Invalid SDF ID :0x%x",lte_info_ptr->sdf_id);
#endif
  return;
}/* ds_umts_flow_manager_bearer_alloc_fail*/

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_BEARER_MODIFY_CNF

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
boolean ds_umts_flow_manager_bearer_modify_cnf
(
  cm_call_id_type           call_id,
  cm_call_mode_info_s_type* mode_info_ptr
)
{
  boolean                       return_value = FALSE;
  ds_pdn_context_s             *pdn_context_p = NULL;
  ds_flow_context_s            *flow_context_p = NULL;
  ds_bearer_context_s          *bearer_context_p = NULL;
  ds_bearer_flow_manager_s     *bflow_manager_p = NULL;
  qos_spec_type                *app_qos_p = NULL;
  ds_3gpp_internal_cause_code_enum_type icause_code;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("UMTS Flow manager received pdp modification cnf");
  if(mode_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input parameter is NULL");
    return DS3G_FAILURE;
  }

  icause_code = DS_3GPP_INTERNAL_CC_INVALID;
  bearer_context_p = ds_bearer_cntxt_get_bearer_from_call_id(call_id);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return DS3G_FAILURE;
  }

  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  flow_context_p = ds_flow_manager_find_pending_flow_context(
                                &pdn_context_p,bearer_context_p->
                                ds_bearer_context_dyn_p->sdf_id);
  if((flow_context_p == NULL) || (pdn_context_p == NULL))
  {
    DS_3GPP_MSG3_ERROR("Flow cntx 0x%x,Pdn cntx 0x%x, non-matching SDF ID 0x%x",
                   flow_context_p, pdn_context_p, bearer_context_p->
                       ds_bearer_context_dyn_p->sdf_id);
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return DS3G_FAILURE;
  }

  if(ds_flow_manager_update_ps_with_network_response(
                                            pdn_context_p,
                                            bearer_context_p,
                                            flow_context_p,
                                            TRUE)== FALSE)
  {
    DS_3GPP_MSG0_ERROR("ps_network_response returned error");
  }

  /*-------------------------------------------------------------------
    Move the flow context from PDN context to bearer.
  ---------------------------------------------------------------------*/
  if(ds_flow_manager_remove_pending_flow_context(
                     pdn_context_p,flow_context_p) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Error in removing the pending context");
  }
  if(ds_bearer_flow_manager_add_flow_context_to_flow_manager(
                bearer_context_p,flow_context_p) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Error in Adding Flow Context to Bearer Flow manager");
  }
  if (mode_info_ptr->info.gw_ps_call.qos.valid)
  {
    bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
    if(bflow_manager_p == NULL)
    {
      DS_3GPP_MSG0_ERROR("No Bearer Flow Manager associated with the Bearer");      
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return DS3G_FAILURE;
    }
    /*-----------------------------------------------------------------------
     Allocate Heap Memory
    -----------------------------------------------------------------------*/
    app_qos_p = (qos_spec_type*)modem_mem_alloc(
                                       sizeof(qos_spec_type),
                                       MODEM_MEM_CLIENT_DATA);

    if(app_qos_p != NULL)        
    {
      ds_umts_bearer_flow_manager_convert_nw_data_rate_to_app( bflow_manager_p,
                                                           (void *)mode_info_ptr,
                                                                  app_qos_p,&icause_code);

      modem_mem_free(app_qos_p, MODEM_MEM_CLIENT_DATA);
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Could not allocate memory to set_granted_flow");      
    }
  }
  if((!ds_flow_context_validate_flow(flow_context_p)))
  {
    if(ds_bearer_cntx_validate_bearer_context(bearer_context_p))
    {
      DS_3GPP_MSG0_HIGH("cleaning up the bearer, sending deactivate");
	  
      ds_bearer_cntxt_phys_link_down_cmd_hdlr(
         (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr, NULL);
    }
     
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return TRUE;	
  }
  ds_flow_context_ps_flow_set_granted_flow(flow_context_p);
  ds_flow_context_set_flow_state(flow_context_p, DS_FLOW_CONTEXT_STATE_UP);
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

  dsUmtsBearerCntxt_SendQOSIndication(bearer_context_p, 
                                      mode_info_ptr->info.gw_ps_call.sys_rat);

  return return_value;
} /* ds_umts_flow_manager_bearer_modify_cnf */ 

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_BEARER_MODIFY_REJ

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
boolean ds_umts_flow_manager_bearer_modify_rej
(
  cm_call_id_type           call_id,
  cm_call_mode_info_s_type* mode_info_ptr
)
{
   ds_pdn_context_s              *pdn_context_p = NULL;
   ds_flow_context_s             *flow_context_p = NULL;
   ds_bearer_context_s           *bearer_context_p = NULL;
   ds_bearer_flow_manager_s      *bflow_manager_p = NULL;
#ifdef FEATURE_DATA_REL10
  ps_iface_net_down_reason_type  down_reason;
  cm_gw_ps_call_info_s_type     *gw_ps_call_info =
    (cm_gw_ps_call_info_s_type*)&(mode_info_ptr->info.gw_ps_call);
  byte                          *apn = NULL;
#endif /*FEATURE_DATA_REL10*/
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("UMTS Flow manager received Bearer mod reject");
  if(mode_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input parameter is NULL");
    return FALSE;
  }

  bearer_context_p = ds_bearer_cntxt_get_bearer_from_call_id(call_id);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }

  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  flow_context_p = ds_flow_manager_find_pending_flow_context(
                                &pdn_context_p, bearer_context_p->
                                ds_bearer_context_dyn_p->sdf_id);
 if((!ds_flow_context_validate_flow(flow_context_p)) ||
         (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p)))
  {
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return FALSE; 
  } 

  if (!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p, &subs_id))
  {
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return FALSE;
  }

  bflow_manager_p = flow_context_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr;
  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return FALSE; 
  }
  
  /*---------------------------------------------------------------------
    Found the pending flow context. Check the flow context state
  ---------------------------------------------------------------------*/
  if((flow_context_p->ds_flow_context_dyn_p->state != 
      DS_FLOW_CONTEXT_STATE_PEND_ADD_RSP)&&
     (flow_context_p->ds_flow_context_dyn_p->state !=
      DS_FLOW_CONTEXT_STATE_PEND_DEL_RSP)&&
     (flow_context_p->ds_flow_context_dyn_p->state != 
      DS_FLOW_CONTEXT_STATE_PEND_DEL_FLOW_RSP)&&
     (flow_context_p->ds_flow_context_dyn_p->state != 
      DS_FLOW_CONTEXT_STATE_PEND_REPLACE_RSP)&&
     (flow_context_p->ds_flow_context_dyn_p->state != 
      DS_FLOW_CONTEXT_STATE_PEND_NO_OP_RSP))
  {
    DS_3GPP_MSG1_ERROR("unexpected mod reject.in state:%x",
                     flow_context_p->ds_flow_context_dyn_p->state);
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return FALSE;
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
      DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
    }
    //update the filter identifier bit mask
    if(ds_bearer_flow_manager_update_filter_identifier_bit_mask(
                    bflow_manager_p->ds_bearer_flow_manager_dyn_p->bearer_ptr,
                    flow_context_p,FALSE)==FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error in clearing Filter identifier bit mask");
    }
    if(ds_flow_manager_remove_pending_flow_context(
                pdn_context_p,flow_context_p) == FALSE)
    { 
      DS_3GPP_MSG0_ERROR("Could not remove Pending Flow Context");
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
      DS_3GPP_MSG0_ERROR("Could not clean up Pending Flow Context");
    }
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  }/* else*/  


  if(ds_3gpp_nv_manager_is_umts_rel_10_enabled())
  {
  if( (gw_ps_call_info->pdp_cause_type == SM_NETWORK_CAUSE) &&
      ((gw_ps_call_info->cause.ext_cause == SM_MISSING_OR_UNKNOWN_APN) ||
       (gw_ps_call_info->cause.ext_cause == SM_INSUFFICIENT_RESOURCES)) )
  {
    /*---------------------------------------------------------------- 
     Perform global APN throttling for these cause codes. Create a 
     PDN throt SM exclusively for globally throttling the given APN
     across all PLMNs.
    ------------------------------------------------------------------*/
    if(gw_ps_call_info->cause.ext_cause == SM_MISSING_OR_UNKNOWN_APN)
    {
      down_reason = PS_NET_DOWN_REASON_UNKNOWN_APN;
    }
    else
    {
      down_reason = PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES;
    }

    apn = pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn;

    if(*apn == '\0')
    {
      (void)ds_pdn_cntxt_get_resolved_apn_name(pdn_context_p, &apn);
    }
    DS_3GPP_MSG2_HIGH("APN being throttled: %s, APN in PDN_ctxt: %s",
                       (char*)apn,
                       (char*)pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn);

    ds_3gpp_pdn_throt_umts_handle_reject_with_t3396_ie(gw_ps_call_info, 
                                                       apn, down_reason,
                                                       subs_id,
                                                       pdn_context_p);
  }
  } //ds_3gpp_pdn_get_nv_lte_rel_10

  return TRUE;
} /*ds_umts_flow_manager_bearer_modify_rej */

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_NW_BEARER_MODIFY_REQ

DESCRIPTION
  Handle the bearer modification request from NW

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_umts_flow_manager_nw_bearer_modify_req
(
  cm_call_id_type                 call_id,
  ds_cmd_type                    *cmd_ptr,
  cm_pdp_act_res_params_s_type   *act_res_params_ptr
)
{
  ds_pdn_context_s                       *pdn_context_p = NULL;
  ds_bearer_context_s                    *bearer_context_p = NULL;
  cm_call_mode_info_s_type               *mode_info_ptr;
  ds_3gpp_internal_cause_code_enum_type   icause_code;
  boolean                                 return_value = FALSE;
  qos_spec_type                           app_qos;
  ds_bearer_flow_manager_s               *bflow_manager_p = NULL;
  ds_flow_context_s                      *flow_context_p = NULL;
  ds_flow_manager_s                      *flow_manager_ptr = NULL;
  cm_gw_ps_call_info_s_type              *ps_call_info;
  uint32                                  decode_mask = 0;
  ds_3gpp_bcm_info_e                      prev_bcm_info = DS_3GPP_BCM_INVALID;
  ds_3gpp_iface_s                        *ds3gpp_iface_v4_p = NULL; 
#ifdef FEATURE_DATA_PS_IPV6
  ds_3gpp_iface_s                        *ds3gpp_iface_v6_p = NULL;
#endif
  ds_umts_pdp_type_enum_type              pdn_pdp_type; 
  sys_modem_as_id_e_type                  subs_id = SYS_MODEM_AS_ID_NONE;
  ds3g_call_info_cmd_type                *call_info_cmd_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("UMTS Flow manager received NW Context Modification Req");

  if( (act_res_params_ptr == NULL) ||
      (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DS_3GPP_MSG0_ERROR("Input parameter is NULL");
    return FALSE;
  }

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  icause_code = DS_3GPP_INTERNAL_CC_INVALID;
  /*-------------------------------------------------------------------------
    Initialize response
  -------------------------------------------------------------------------*/
  mode_info_ptr = &(call_info_cmd_ptr->mode_info);
  ps_call_info = &(mode_info_ptr->info.gw_ps_call);
  act_res_params_ptr->accept = FALSE;
  act_res_params_ptr->cause = SM_UNKNOWN_PDP_CONTEXT;

  /*-------------------------------------------------------------------------
    Get the bearer context and the PDN context
  -------------------------------------------------------------------------*/
  bearer_context_p= 
                ds_bearer_cntxt_get_bearer_from_call_id(call_id);
  /* Can bearer be in going down state and receive modification
     request? We should bring up the bearer */ 
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    act_res_params_ptr->cause = SM_UNKNOWN_PDP_CONTEXT;
    return DS3G_FAILURE;
  }
  /*-------------------------------------------------------------------------
    Retrieve the PDN Context
  -------------------------------------------------------------------------*/
  pdn_context_p = (ds_pdn_context_s*)bearer_context_p->ds_bearer_context_dyn_p
    ->pdn_context_ptr; 

  if(!(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p, &subs_id)))
  {
    act_res_params_ptr->cause = SM_UNKNOWN_PDP_CONTEXT;
    return DS3G_FAILURE;
  }

  if(pdn_context_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_GOING_DOWN)
  {
    DS_3GPP_MSG0_HIGH("Received Bearer Modification when PDN is going down");
    act_res_params_ptr->cause = SM_INSUFFICIENT_RESOURCES;
    return DS3G_FAILURE;
  } /* PDN context is going down */

  /*-------------------------------------------------------------------------
    Decode PCO as it could contain BCM update
  -------------------------------------------------------------------------*/
  pdn_pdp_type = ds_pdn_cntxt_get_pdn_context_ip_support(pdn_context_p);
  if(pdn_pdp_type == DS_UMTS_PDP_PPP ||
     pdn_pdp_type == DS_UMTS_PDP_IPV4)
  {
    ds3gpp_iface_v4_p = 
      (pdn_context_p->ds_pdn_context_dyn_p->
       ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE]);
    DS_3GPP_MSG0_HIGH("PDP type IPV4 is selected in setupPdpIP"); 
  }
#ifdef FEATURE_DATA_PS_IPV6
  else if(pdn_pdp_type == DS_UMTS_PDP_IPV6)
  {
    ds3gpp_iface_v6_p = 
      (pdn_context_p->ds_pdn_context_dyn_p->
       ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE]);
    DS_3GPP_MSG0_HIGH("PDP type IPV6 is selected in setupPdpIP"); 
  }
  else if (pdn_pdp_type == DS_UMTS_PDP_IPV4V6)
  {
    ds3gpp_iface_v4_p = 
      (pdn_context_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
       [DS_3GPP_PDN_CONTEXT_IPV4_IFACE]);
    ds3gpp_iface_v6_p = 
      (pdn_context_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
       [DS_3GPP_PDN_CONTEXT_IPV6_IFACE]);
  }
#endif /* FEATURE_DATA_PS_IPV6 */
  else
  {
    DATA_ERR_FATAL("PDP type is not PPP or IP V4 or IP V6. Dual IP bearer not sup yet");
  }

  if((ps_call_info->pdp_addr.valid == TRUE)&&
     (ds_3gpp_pdn_cntxt_compare_pdp_address(
             bearer_context_p->ds_bearer_context_dyn_p->nsapi, 
             bearer_context_p->ds_bearer_context_dyn_p->call_mode, 
             subs_id,
             &(ps_call_info->pdp_addr)) == FALSE))
  {  
    DS_3GPP_MSG0_ERROR("Modify PDP address not matching original, initiating call end");

    ds_bearer_cntxt_phys_link_down(&bearer_context_p->phys_link,NULL);
    return FALSE;
  }
  
  flow_manager_ptr = ds_flow_manager_find_flow_manager(pdn_context_p);

  if(!ds_flow_manager_validate_flow_manager(flow_manager_ptr))
  {
    return FALSE;
  }
  prev_bcm_info = flow_manager_ptr->ds_flow_manager_dyn_p->bcm_info;
  decode_mask = ds_pdn_cntxt_decodeProtocolParams(
                         ds3gpp_iface_v4_p,ds3gpp_iface_v6_p, &
                         ps_call_info->config_options);
 
  if((decode_mask & DS_3GPP_BEARER_CONTROL_MODE_DECODED_MASK) == 0 )
  {
    if(ds_3gpp_nv_manager_get_bcm_info_validation_nv() == TRUE)
    {
      /* As per spec, if BCM is not present set to UE only , if 
         BCM validation NV is set to FALSE, don't modify BCM info, 
         it will retain old BCM value */
      flow_manager_ptr->ds_flow_manager_dyn_p->bcm_info = DS_3GPP_BCM_UE_ONLY;
    }
  }
 
  if((ps_call_info->tft.valid)&&
  	 (flow_manager_ptr->ds_flow_manager_dyn_p->bcm_info != DS_3GPP_BCM_UE_NW))
  {
    DS_3GPP_MSG0_HIGH("BCM violation, call rejected");
    act_res_params_ptr->cause = SM_ACTIVATION_REJECTED_BCM_VIOLATION;
    flow_manager_ptr->ds_flow_manager_dyn_p->bcm_info = prev_bcm_info;
    return FALSE;
  }

#ifndef FEATURE_SUPPORT_TFT_ON_DEFAULT_BEARER

  /*-------------------------------------------------------------------------
    Verify if this is the default bearer.
  -------------------------------------------------------------------------*/
  if(pdn_context_p->ds_pdn_context_dyn_p->def_bearer_context_ptr == bearer_context_p)
  {
    DS_3GPP_MSG0_HIGH("Bearer Modification received on default bearer");

    if(ps_call_info->tft.valid == TRUE)
    {
      DS_3GPP_MSG0_ERROR("TFT is not supported on default bearer");
      act_res_params_ptr->cause = SM_QOS_NOT_ACCEPTED;
      flow_manager_ptr->ds_flow_manager_dyn_p->bcm_info = prev_bcm_info;
      return DS3G_FAILURE;
    }
    
    if(ps_call_info->qos.valid == TRUE)
    {
      memset(&app_qos, 0, sizeof(qos_spec_type));
      DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      if(ds_bearer_flow_manager_convert_data_rate(bearer_context_p,
                                             mode_info_ptr,
                                             &app_qos,&icause_code) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("NW modified data  rate not accepted");
        if(ds_flow_manager_convert_cause_codes
             (mode_info_ptr, &icause_code, subs_id)== FALSE)
        {
          DS_3GPP_MSG3_ERROR("Error in converting cause code",0,0,0);
        } 
        else
        {
          /* update cause to end param */
          act_res_params_ptr->cause = mode_info_ptr->info.gw_ps_call.cause.ext_cause;
        }
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        flow_manager_ptr->ds_flow_manager_dyn_p->bcm_info = prev_bcm_info;
        return DS3G_FAILURE;
      }
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    }/* qos.valid == TRUE*/
    else
    {
      DS_3GPP_MSG0_ERROR("No QOS to modify on primary pdp");
      act_res_params_ptr->cause = SM_QOS_NOT_ACCEPTED;
    }
    return DS3G_SUCCESS;
  }/* if default bearer */

#endif /* FEATURE_SUPPORT_TFT_ON_DEFAULT_BEARER */

  /*-------------------------------------------------------------------------
    If no TFT present 
  -------------------------------------------------------------------------*/
  if(ps_call_info->tft.valid != TRUE)
  {
    DS_3GPP_MSG0_HIGH("TFT is not valid for bearer modification");

    if(ps_call_info->qos.valid == TRUE)  
    {
      DS_3GPP_MSG0_HIGH("Qos with out Tft for bearer mod");
      /*--------------------------------------------------------------------
       Store the SDF QOS in Mode specific context
      ---------------------------------------------------------------------*/
      memset(&app_qos, 0, sizeof(qos_spec_type));
      DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      if(ds_bearer_flow_manager_convert_data_rate(bearer_context_p,
                                             mode_info_ptr,
                                             &app_qos,&icause_code) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("NW modified data rate not accepted");
        if(ds_flow_manager_convert_cause_codes
             (mode_info_ptr, &icause_code, subs_id)== FALSE)
        {
          DS_3GPP_MSG3_ERROR("Error in converting cause code",0,0,0);
        } 
        else
        {
          /* update cause to end param */
          act_res_params_ptr->cause = mode_info_ptr->info.gw_ps_call.cause.ext_cause;
        }
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        flow_manager_ptr->ds_flow_manager_dyn_p->bcm_info = prev_bcm_info;
        return DS3G_FAILURE;
      }
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

      /*--------------------------------------------------------------------
        Find the Bearer flow manager and update the flow contexts.
      ---------------------------------------------------------------------*/
      bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(
                                                         bearer_context_p);

      if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
      {
        flow_manager_ptr->ds_flow_manager_dyn_p->bcm_info = prev_bcm_info;
        return FALSE;
      }
      flow_context_p = list_peek_front(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                                         ->flow_context));
      while(flow_context_p != NULL)
      {
        ds_flow_context_ps_flow_set_granted_flow(flow_context_p);
        flow_context_p = list_peek_next(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                                          ->flow_context),
                                &(flow_context_p->link));
      }   
    } /* sdf_qos.valid == TRUE */ 
    act_res_params_ptr->accept = TRUE;
    return DS3G_SUCCESS;
  } /* TFT Not Valid */


  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  if(ps_call_info->qos.valid == FALSE)
  {
    return_value = ds_flow_manager_process_tft_for_nw_bearer_mod(
                                  pdn_context_p,
                                  bearer_context_p,
                                  &ps_call_info->tft,
                                  NULL,
                                  &icause_code,
                                  FALSE);
  }
  else
  {
    return_value = ds_flow_manager_process_tft_for_nw_bearer_mod(
                                  pdn_context_p,
                                  bearer_context_p,
                                  &ps_call_info->tft,
                                  mode_info_ptr,
                                  &icause_code,
                                  FALSE);
  }
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

  /*-------------------------------------------------------------------------
    translate cause and set value
  -------------------------------------------------------------------------*/
  act_res_params_ptr->accept = return_value;
  if(return_value == FALSE)
  {
    if(ds_flow_manager_convert_cause_codes
        (mode_info_ptr, &icause_code, subs_id)== FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error in converting cause code");
    } 
    else
    {
      /* update cause to end param */
      act_res_params_ptr->cause = mode_info_ptr->info.gw_ps_call.cause.ext_cause;
    }
  }
  return return_value;   
} /* ds_umts_flow_manager_nw_bearer_modify_req */


/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_BEARER_MODIFY_IND

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
boolean ds_umts_flow_manager_bearer_modify_ind
(
  cm_call_mode_info_s_type* mode_info_ptr,
  cm_call_id_type           call_id
)
{

  boolean                       return_value = FALSE;
#if 0
  ds_pdn_context_s             *pdn_context_p = NULL;
  ds_bearer_flow_manager_s     *bflow_manager_p = NULL;
  ds_flow_context_s            *flow_context_p = NULL;
  ds_bearer_context_s          *bearer_context_p = NULL;
  ds_3gpp_internal_cause_code_enum_type icause_code=DS_3GPP_INTERNAL_CC_NONE;  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("UMTS Flow manager received Bearer modification indication");
  if(mode_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input parameter is NULL");
    return DS3G_FAILURE;
  }

  /*-------------------------------------------------------------------------
    get the LTE info 
  -------------------------------------------------------------------------*/
  lte_info_ptr =(cm_lte_call_info_s_type *)&(mode_info_ptr->info.lte_call);
  DS_3GPP_MSG1_HIGH("Modify Ind for SDF ID :0x%x",lte_info_ptr->sdf_id);

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
    if((flow_context_p == NULL) || (pdn_context_p == NULL))
    {
      DS_3GPP_MSG0_ERROR("Flow cntx 0x%x,Pdn cntx 0x%x, non-matching SDF ID 0x%x",
                     flow_context_p, pdn_context_p,lte_info_ptr->sdf_id);
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
      DS_3GPP_MSG0_ERROR("invalid params passed");
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      lte_info_ptr->esm_cause.valid = TRUE;
      lte_info_ptr->esm_cause.esm_cause =NAS_ESM_INVALID_MANDATORY_INFORMATION;
      return DS3G_FAILURE;
    }
    /*---------------------------------------------------------------------
      Found the pending flow context. Retrieve bearer context
    ---------------------------------------------------------------------*/
    bflow_manager_p = flow_context_p->bearer_flow_mgr_ptr;
    if(bflow_manager_p == NULL)
    {
      DS_3GPP_MSG0_HIGH("Bearer flow Manager is NULL");
      /* Case where UE bearer request is NW bearer mod */
      bearer_context_p= 
                ds_bearer_cntxt_get_bearer_from_call_id(call_id);
      if(bearer_context_p == NULL)
      {
        DS_3GPP_MSG0_ERROR("Bearer is NULL");
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        lte_info_ptr->esm_cause.valid = TRUE;
        lte_info_ptr->esm_cause.esm_cause =NAS_ESM_ACTIVATION_REJECTED;
        return DS3G_FAILURE;
      }
      bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(
                                                    bearer_context_p);
      if(bflow_manager_p == NULL)
      {
        DS_3GPP_MSG0_ERROR("Bearer Flow Manager is NULL");
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        lte_info_ptr->esm_cause.valid = TRUE;
        lte_info_ptr->esm_cause.esm_cause =NAS_ESM_ACTIVATION_REJECTED;
        return DS3G_FAILURE;
      }     
    }
    if(bearer_context_p == NULL)
    {
      bearer_context_p = bflow_manager_p->bearer_ptr;
    }
    if(bearer_context_p == NULL)
    {
      DS_3GPP_MSG0_ERROR("Bearer context not found");
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
        DS_3GPP_MSG0_ERROR("ps_network_response returned error");
      }
  
      /*-------------------------------------------------------------------
        Update the cause codes.
      -------------------------------------------------------------------*/
      if( (icause_code == DS_3GPP_INTERNAL_CC_NONE) ||
            (icause_code == DS_3GPP_INTERNAL_CC_INVALID))
      {
        DS_3GPP_MSG0_ERROR("cause code not set");
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        lte_info_ptr->esm_cause.valid = TRUE;
        lte_info_ptr->esm_cause.esm_cause =NAS_ESM_ACTIVATION_REJECTED;
        return DS3G_FAILURE;
      }
      if(ds_flow_manager_convert_cause_codes(
                                mode_info_ptr,&icause_code)==FALSE)
      {
        DS_3GPP_MSG0_ERROR("cause code conversion failed");
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
          DATA_MSG0_ERROR("Unexpected error in apn ambr");
          // We need to revert to old flow stuff or teardown the flow.
          return DS3G_FAILURE;
        }     
      } /* if apn_ambr is valid */
    }/*  verify_nwResponse == DS3G_SUCCESS */
    return DS3G_SUCCESS;   
  } /* SDF_ID != 0xFFFF */ 

  /*---------------------------------------------------------------------
    Process the request for NW initiated Bearer Modification.
  ---------------------------------------------------------------------*/
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  return_value = ds_eps_flow_manager_nw_bearer_modify_ind(
                                           mode_info_ptr, call_id);
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
#endif //if 0
  return return_value;
} /* ds_eps_flow_manager_bearer_modify_ind */ 

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_BEARER_ALLOCATION_IND

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
boolean ds_umts_flow_manager_bearer_allocation_ind
(
  ds_pdn_context_s*         pdn_context_p,
  ds_bearer_context_s*      bearer_context_p,
  const cm_call_mode_info_s_type* mode_info_ptr
)
{  
  ds_bearer_flow_manager_s     *bflow_manager_p = NULL;
  ds_flow_context_s            *flow_context_p = NULL;
  ds_3gpp_internal_cause_code_enum_type icause_code=DS_3GPP_INTERNAL_CC_NONE;
  tft_type_T                    *nw_tft = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG1_HIGH("bearer allocation ind for bearer 0x%x",
                    bearer_context_p);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }

  if((pdn_context_p== NULL)||(mode_info_ptr == NULL))
  {
    DS_3GPP_MSG0_ERROR("input params are NULL");
    return FALSE;
  }  

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }


  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  /*-------------------------------------------------------------------------
    Get the app specified QOS parameters saved in bearer_flow_manager    
  -------------------------------------------------------------------------*/    
  bflow_manager_p=ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(bflow_manager_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("No Bearer Flow Manager associated with the Bearer");      
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return DS3G_FAILURE;
  }

  /* In the case of UE Init QOS; there is no incoming call event. Rather DS
     only get a call_connected event. So we need to store the bearer QOS
     in our data structures. */
  ds_umts_bearer_flow_manager_store_nw_data_rate(&(mode_info_ptr->info.gw_ps_call), 
                                                 bflow_manager_p);

  /*-----------------------------------------------------------------------
    TFT is filled from the flow context. There should be only one pending 
    flow context in the bearer flow manager list.
  
    if(list_size(&(bflow_manager_p->flow_context)) != 1)
    {
      DS_3GPP_MSG0_ERROR("Found more than one pending flow context");      
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return DS3G_FAILURE;
    }
    flow_context_p = list_peek_front(&(bflow_manager_p->flow_context));
    VRK: Getting the flow_context from the PDN context
  -----------------------------------------------------------------------*/
    
  flow_context_p = ds_flow_manager_find_pending_flow_context_for_pdn(
                        (ds_pdn_context_s*)bearer_context_p->ds_bearer_context_dyn_p
                        ->pdn_context_ptr,
                        bearer_context_p->ds_bearer_context_dyn_p->sdf_id);
  if(flow_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Cound not find a pending flow context");      
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return DS3G_FAILURE;
  }  

  nw_tft = (tft_type_T *)modem_mem_alloc(
                    sizeof(tft_type_T),
                    MODEM_MEM_CLIENT_DATA_CRIT );

  if (nw_tft == NULL ) 
  {
    DS_3GPP_MSG0_ERROR("FAIL modem_mem_alloc for ds_umts_flow_manager_bearer_allocation_ind");
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return DS3G_FAILURE;
  }

  memset(nw_tft,0x0,sizeof(tft_type_T));
  memscpy(nw_tft, sizeof(tft_type_T),
          &(mode_info_ptr->info.gw_ps_call.tft),
          sizeof(tft_type_T));

  /*---------------------------------------------------------------------
    Verify if requested flow and granted matches.
  ---------------------------------------------------------------------*/
  if (ds_flow_manager_verify_nw_response(
                          pdn_context_p,
                          bearer_context_p,
                          flow_context_p,
                          mode_info_ptr,
                          nw_tft,
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
      DS_3GPP_MSG0_ERROR("ps_network_response returned error");
    }

    /* Update the cause codes */
    if( (icause_code == DS_3GPP_INTERNAL_CC_NONE) ||
          (icause_code == DS_3GPP_INTERNAL_CC_INVALID))
    {
      DS_3GPP_MSG0_ERROR("cause code not set");
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      //lte_info_ptr->esm_cause.valid = TRUE;
      //lte_info_ptr->esm_cause.esm_cause =NAS_ESM_ACTIVATION_REJECTED;
      modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
      return DS3G_FAILURE;
    }
    if(ds_umts_flow_manager_convert_cause_code(mode_info_ptr,&icause_code)
       ==FALSE)
    {
      DS_3GPP_MSG0_ERROR("cause code conversion failed");
    }
    
    /*--------------------------------------------------------------------------
      Release Bearer Flow Manager associated with the bearer.
      VRK:TODO Do we need to do this since returning failure
     will give us call_end which shd cleanup everything
    --------------------------------------------------------------------------*/
//     if(ds_flow_manager_remove_bearer_context_from_flow_manager(
//                                        pdn_context_p, bearer_context_p) == FALSE)
//     {
//       DS_3GPP_MSG0_ERROR("error when removing Bearer context from Flow mnger");
//     }

    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
    return DS3G_FAILURE;
  }/* verify_nwResponse == DS3G_FAILURE*/
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
  return DS3G_SUCCESS;       
}/* ds_umts_flow_manager_bearer_allocation_ind */


/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_BEARER_MODIFY_REQ

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
int ds_umts_flow_manager_bearer_modify_req
(
  ds_flow_context_s             *flow_context_p,
  tft_type_T                    *nw_tft_p,
  qos_spec_type                 *app_qos,
  int                           flow_state
)
{  
  cm_pdp_modify_params_s_type    pdp_mod;  
  ds_pdn_context_s              *pdn_context_p = NULL;
  ds_3gpp_iface_s               *ds_iface_p    = NULL;
  ds_bearer_context_s           *bearer_context_p = NULL;
  ds_bearer_flow_manager_s      *bflow_manager_p = NULL;
  tft_type_T                    *nw_tft = NULL;
  ds_umts_bearer_flow_manager_s *umts_bflow_manager_p = NULL;
  sys_modem_as_id_e_type         subs_id = SYS_MODEM_AS_ID_NONE;
  sys_sys_mode_e_type            umts_network_mode = SYS_SYS_MODE_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
#if defined FEATURE_WCDMA
  umts_network_mode = SYS_SYS_MODE_WCDMA;
#elif defined FEATURE_TDSCDMA
  umts_network_mode =  SYS_SYS_MODE_TDS;
#elif defined FEATURE_GSM_GPRS
  umts_network_mode = SYS_SYS_MODE_GSM;
#endif

  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return -1;
  }
  
  if(flow_context_p->ds_flow_context_dyn_p->state != (ds_flow_context_state_e)flow_state)
  {
    DS_3GPP_MSG2_ERROR("flow states do not match. current:0x%x; cmd for:0x%x",
                         flow_context_p->ds_flow_context_dyn_p->state, flow_state);
    return -1;
  }
   
  memset(&pdp_mod, 0, sizeof(cm_pdp_modify_params_s_type));

  if( (ds_iface_p = flow_context_p->ds_flow_context_dyn_p->ds_iface_ptr) == NULL)
  {
    DS_3GPP_MSG0_ERROR("DS IFACE PTR is null for flow context");
    return -1;
  } 
 
  if(ds_iface_p->client_data_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("PDN CNTX PTR is null");
    return -1;
  }
  pdn_context_p = ds_iface_p->client_data_ptr;

  bflow_manager_p = flow_context_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr;

  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return -1;
  }

  /*---------------------------------------------------------------------
   Get the mode specific blob.
   WCDMA/GSM both point to the same object
  ---------------------------------------------------------------------*/
  umts_bflow_manager_p = (ds_umts_bearer_flow_manager_s *)
    DS_BEARER_FLOW_MGR_VOBJ(bflow_manager_p
                            ,umts_network_mode);

  if(umts_bflow_manager_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("umts_bearer_context_p is NULL");   
    return -1;
  }

   bearer_context_p = bflow_manager_p->ds_bearer_flow_manager_dyn_p
       ->bearer_ptr;

  if(!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return -1;
  }

  // Assign nw tft pointer
  nw_tft = &(pdp_mod.tft);

  if(flow_context_p->ds_flow_context_dyn_p->state == 
     DS_FLOW_CONTEXT_STATE_PEND_DEL_FLOW_RSP)
  {
    /*-------------------------------------------------------------------
      Request deleting only the filters associated with the FLOW.
    -------------------------------------------------------------------*/
    if((nw_tft_p->valid) && 
          (nw_tft_p->operation_code == (byte) DS_3GPP_TFT_OPCODE_DEL_FILTER))
    {
      DS_3GPP_MSG0_HIGH("Processing filled in tft del filter");
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
        DS_3GPP_MSG0_ERROR("Error in updating the TFT info");
        return -1;
      }
      nw_tft->tft_total_length = 1 + nw_tft->num_filters;
    }/* nw_tft_p->Valid == FALSE */
  }/* delete Flow*/ 

  if(flow_context_p->ds_flow_context_dyn_p->state == 
     DS_FLOW_CONTEXT_STATE_PEND_MODIFY_RSP)
  {
    if(nw_tft_p == NULL)
    {
      DS_3GPP_MSG0_ERROR("Input TFT param is NULL");
      return -1;
    }
    memscpy(nw_tft, sizeof(tft_type_T), nw_tft_p, sizeof(tft_type_T));
    if(app_qos != NULL)
    {
      if(app_qos->field_mask & (QOS_MASK_RX_FLOW|QOS_MASK_TX_FLOW))
      {
        if(ds_bearer_flow_manager_convert_app_qos_to_nw_qos
           (app_qos, (void *)umts_bflow_manager_p, subs_id) == FALSE)
        {
          DS_3GPP_MSG0_ERROR("APP TFT conversion to NW TFT failed");
          return -1;
        /* We need to send modify reject to PS. App is aware that request
           did not go thru */
        }
        if(umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p != NULL && 
           umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p->cookie== UMTS_BEARER_FLOW_MANAGER_VALID_COOKIE )
        {
          if(umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p->req_qos.valid)
        {
            memscpy(&pdp_mod.qos, sizeof(pdp_mod.qos), 
                    &umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p->req_qos, 
                    sizeof(qos_T));
        }
          if(umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p->min_qos.valid)
        {
            memscpy(&pdp_mod.minimum_qos, sizeof(pdp_mod.minimum_qos), 
                    &umts_bflow_manager_p->ds_umts_bearer_flow_manager_dynamic_p->min_qos, 
                    sizeof(qos_T));
          }       
        }
        else{
           DS_3GPP_MSG0_ERROR("Invalid Umts Bearer Flow Manager");
           return -1;
        }       
        /* store app qos in blfow mgr */       
        memscpy( &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->tx_flow),
                 sizeof(ip_flow_type),
                 &app_qos->tx.flow_template.req_flow, sizeof(ip_flow_type));
        memscpy( &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->rx_flow),
	         sizeof(ip_flow_type),
                 &app_qos->rx.flow_template.req_flow, sizeof(ip_flow_type));
      }
    }/* app_qos != NULL */
    else
    {
      pdp_mod.qos.valid = FALSE;
    }
  } /* modify Flow*/

  pdp_mod.nsapi.valid = TRUE;
  pdp_mod.nsapi.nsapi = bearer_context_p->ds_bearer_context_dyn_p->nsapi;
  
  if ((ds3g_msh_pdp_modify_req(bearer_context_p->ds_bearer_context_dyn_p->
                               cm_call_id, &pdp_mod)) == DS3G_FAILURE)
  {
    DS_3GPP_MSG0_ERROR("DS3G pdp modify req failed"); 
    return -1;
    // Will have to add a re-try mechanism to try sending out again
  }

  if(flow_context_p->ds_flow_context_dyn_p->state == DS_FLOW_CONTEXT_STATE_PEND_MODIFY_RSP)
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
    else if(nw_tft_p->operation_code ==(byte)DS_3GPP_TFT_OPCODE_ADD)
    {
      ds_flow_context_set_flow_state(flow_context_p,
                                  DS_FLOW_CONTEXT_STATE_PEND_ADD_RSP);
    }
    else if(nw_tft_p->operation_code ==(byte)DS_3GPP_TFT_OPCODE_NO_OP)
    {
      ds_flow_context_set_flow_state(flow_context_p,
                                  DS_FLOW_CONTEXT_STATE_PEND_NO_OP_RSP);
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Unsupported nw tft operation");
      return -1;
    }
  }/* if STATE_PEND_MODIFY_RSP */
  bearer_context_p->ds_bearer_context_dyn_p->sdf_id = flow_context_p->sdf_id;
  if(ds_bearer_flow_manager_remove_flow_context_from_flow_manager(
                           bearer_context_p, flow_context_p) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Error when remove flow context from Bflow mgr");
  }

  /*---------------------------------------------------------------------
    Move the flow context from bearer flow manager to pending context
    Convert App Qos to NW Qos.
  ---------------------------------------------------------------------*/
  if(ds_flow_manager_add_pending_flow_context(
                           pdn_context_p,flow_context_p) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Error when adding pending flow context to flow mgr");
  }

  /*---------------------------------------------------------------------
    Keep a copy of the UE request to NW.
  ---------------------------------------------------------------------*/
  memscpy(&(flow_context_p->ds_flow_context_dyn_p->tft_mod),
            sizeof(tft_type_T),
            nw_tft,sizeof(tft_type_T));
  return 0;
}/* ds_umts_flow_manager_bearer_modify_req */

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_BEARER_ALLOC_REQ

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
int ds_umts_flow_manager_bearer_alloc_req
(
  ds_pdn_context_s    *pdn_context_p,
  ds_flow_context_s   *flow_context_p,
  qos_spec_type       *app_qos,
  tft_type_T          *nw_tft
)
{  
  ds_bearer_context_s              *def_bearer_p = NULL;   
  ds_bearer_context_s              *bearer_cntx_p = NULL;
  ds_bearer_flow_manager_s         *bflow_manager_p = NULL;
  ds_umts_bearer_flow_manager_s    *umts_bflow_manager_p = NULL;
  int16                             ps_errno;           /* Errno value       */
  int                               ret_val;
  sys_modem_as_id_e_type            subs_id = SYS_MODEM_AS_ID_NONE;
  sys_sys_mode_e_type               umts_network_mode = SYS_SYS_MODE_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

#if defined FEATURE_WCDMA
  umts_network_mode = SYS_SYS_MODE_WCDMA;
#elif defined FEATURE_TDSCDMA
  umts_network_mode =  SYS_SYS_MODE_TDS;
#elif defined FEATURE_GSM_GPRS
  umts_network_mode = SYS_SYS_MODE_GSM;
#endif  

  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

  if((!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p, &subs_id)) ||
     (app_qos== NULL) ||
     (nw_tft == NULL) ||
     (!ds_flow_context_validate_flow(flow_context_p)))
  {
    DS_3GPP_MSG3_ERROR("Input ptr is NULL.Flow_ptr:0x%x pdn_ptr:0x%x app_qos:0x%x",
               flow_context_p,pdn_context_p,app_qos);
    DS_3GPP_MSG0_ERROR("Ran out of free bearers"); 
    /* No bearer clenup required */
    ds_umts_flow_manager_clean_up(pdn_context_p, flow_context_p, NULL);    
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return  -1;
  }

  /*-------------------------------------------------------------------------
    Verify flow context state
  -------------------------------------------------------------------------*/
  if(flow_context_p->ds_flow_context_dyn_p->state == DS_FLOW_CONTEXT_STATE_PEND_ADD_RSP)
  {
    // UE init modify, call modify function
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    // Update the OP code to reflect an add
    nw_tft->operation_code = DS_3GPP_TFT_OPCODE_ADD;
    return ds_umts_flow_manager_bearer_modify_req(flow_context_p,nw_tft,app_qos,
                               (int) flow_context_p->ds_flow_context_dyn_p->state);
  }
  else if (flow_context_p->ds_flow_context_dyn_p->state != DS_FLOW_CONTEXT_STATE_COMING_UP) 
  {
    DS_3GPP_MSG1_HIGH("Bearer Alloc received in unexpeced flow state:0x%x",
               flow_context_p->ds_flow_context_dyn_p->state);
    DS_3GPP_MSG0_ERROR("Ran out of free bearers"); 
    /* No bearer clenup required */
    ds_umts_flow_manager_clean_up(pdn_context_p, flow_context_p, NULL);
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return -1;
  }

  /*-----------------------------------------------------------------------
    Make sure, def_bearer is present
  -----------------------------------------------------------------------*/
  def_bearer_p=(ds_bearer_context_s *)
                pdn_context_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;
  if(def_bearer_p ==  NULL)
  {
    DS_3GPP_MSG0_ERROR("default bearer pointer is NULL");
    DS_3GPP_MSG0_ERROR("Ran out of free bearers"); 
    /* No bearer clenup required */
    ds_umts_flow_manager_clean_up(pdn_context_p, flow_context_p, NULL);
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return -1;
  }  

  /*-----------------------------------------------------------------------
    Get a bearer by calling the alloc function.
  -----------------------------------------------------------------------*/    
  bearer_cntx_p = ds_bearer_cntxt_allocate_new_bearer(subs_id);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_cntx_p))
  {
    /* No bearer clenup required */
    ds_umts_flow_manager_clean_up(pdn_context_p, flow_context_p, NULL);
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return  -1;
  } 

  /*-------------------------------------------------------------------------
    Allocate a new bearer flow manager.
  -------------------------------------------------------------------------*/
  bflow_manager_p = 
              ds_bearer_flow_manager_allocate_flow_manager(bearer_cntx_p);

  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
      /* bearer clenup required */
      ds_umts_flow_manager_clean_up(pdn_context_p, 
                                    flow_context_p, 
                                    bearer_cntx_p);                      
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return  -1;
  }
  
  DS_3GPP_MSG0_HIGH("Bearer Allocated a bearer flow manager");
  
  /*---------------------------------------------------------------------
   Update the PDN PTR.
  ---------------------------------------------------------------------*/
  bflow_manager_p->ds_bearer_flow_manager_dyn_p->pdn_ptr = pdn_context_p;

  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  
  /*---------------------------------------------------------------------
   Get the mode specific blob.
   WCDMA/GSM both point to the same object
  ---------------------------------------------------------------------*/
  umts_bflow_manager_p = (ds_umts_bearer_flow_manager_s *)
    DS_BEARER_FLOW_MGR_VOBJ(bflow_manager_p
                            ,umts_network_mode);

  if(umts_bflow_manager_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("umts_bearer_context_p is NULL");

    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    
    ds_bearer_flow_manager_release_flow_manager(bearer_cntx_p);
    /* bearer clenup required */
    ds_umts_flow_manager_clean_up(pdn_context_p, 
                                  flow_context_p,
                                  bearer_cntx_p);
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    
    return -1;
  }
  /* Mode specific 
  memcpy((void *)&(umts_bflow_manager_p->sdf_qos),
         (void *)&(lte_info_ptr->sdf_qos), sizeof(sdf_qos_T));
  */

  /*-------------------------------------------------------------------------
    Convert App requested Qos to NW specific Qos
    //VRK:TODO save in ds_umts_bearer_flow_manager_s (re_qos,min_qos and neg/granted qos),
     reUse dsumtsps_sec_call_qos_param_process in dsumtspsqos.c
  -------------------------------------------------------------------------*/
  if(ds_bearer_flow_manager_convert_app_qos_to_nw_qos
     (app_qos,(void *)umts_bflow_manager_p, subs_id) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("APP QOS conversion to NW QOS failed");    
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);      
    ds_bearer_flow_manager_release_flow_manager(bearer_cntx_p);    
    /* bearer clenup required */
    ds_umts_flow_manager_clean_up(pdn_context_p, 
                                  flow_context_p,
                                  bearer_cntx_p);
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    
    return -1;    
  }  

  //VRK:TODO Check the return val
  ds_umts_bearer_flow_manager_update_filter_id(nw_tft);
  /*---------------------------------------------------------------------
    Keep a copy of the TFT.
  ---------------------------------------------------------------------*/
  memscpy(&(flow_context_p->ds_flow_context_dyn_p->tft_mod),
            sizeof(tft_type_T), nw_tft, sizeof(tft_type_T));  
  /*---------------------------------------------------------------------
    Save requested qos into bearer flow manager
  ---------------------------------------------------------------------*/
  memscpy( &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->tx_flow), 
           sizeof(ip_flow_type),
           &app_qos->tx.flow_template.req_flow, sizeof(ip_flow_type));
  memscpy( &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->rx_flow), 
  	   sizeof(ip_flow_type),
           &app_qos->rx.flow_template.req_flow, sizeof(ip_flow_type));

  ret_val = ps_phys_link_up_cmd(&(bearer_cntx_p->phys_link),
                                &ps_errno,NULL);        
               
  if (ret_val == 0)
  {
    DS_3GPP_MSG0_HIGH(" Bringing up phys link when already up / coming up:");
  }
  else if ( ps_errno != DS_EWOULDBLOCK)
  {
    DS_3GPP_MSG1_ERROR("ps_phys_link_up_cmd failed errno:%d",ps_errno);
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    
    ds_bearer_flow_manager_release_flow_manager(bearer_cntx_p);    
    /* bearer clenup required */
    ds_umts_flow_manager_clean_up(pdn_context_p, 
                                  flow_context_p,
                                  bearer_cntx_p);
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    
    return -1;
  }

  /*----------------------------------------------------------------------
    Update the bearer information in the pdn cntx
  ---------------------------------------------------------------------*/
  if(ds_pdn_cntxt_store_bearer_context(pdn_context_p,(void *)bearer_cntx_p) != \
                                              DS3G_SUCCESS)
  {
    DS_3GPP_MSG0_HIGH("Could not store the allocated bearer");
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);    
    ds_bearer_flow_manager_release_flow_manager(bearer_cntx_p);
    /* bearer clenup required */
    ds_umts_flow_manager_clean_up(pdn_context_p, 
                                  flow_context_p,
                                  bearer_cntx_p);
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    
    return  -1;
  }

  /*-------------------------------------------------------------------
   Add bearer context to flow manager.       
   VR:TODO untie in case of err
  -------------------------------------------------------------------*/
  if (ds_flow_manager_add_bearer_context_to_flow_manager(
                     pdn_context_p,bearer_cntx_p) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Error in Adding bearer context to Flow manager");
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    
    ds_bearer_flow_manager_release_flow_manager(bearer_cntx_p);    
    if(ds_pdn_cntxt_release_bearer_context(pdn_context_p,(void *)bearer_cntx_p) !=\
                                           DS3G_SUCCESS)
    {
      DS_3GPP_MSG0_HIGH("could not release the allocated bearer");
    }    
    /* bearer clenup required */
    ds_umts_flow_manager_clean_up(pdn_context_p, 
                                  flow_context_p,
                                  bearer_cntx_p);
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    
  }
  
/**************************************************************************** 
  VRK: Lets not add flow_context to the bflowmgr yet, but save the sdf_if.
****************************************************************************/
  bearer_cntx_p->ds_bearer_context_dyn_p->sdf_id = flow_context_p->sdf_id;
/****************************************************************************
  if(ds_bearer_flow_manager_add_flow_context_to_flow_manager(                       
                  bearer_cntx_p,flow_context_p) == FALSE)                           
  {                                                                                 
    DS_3GPP_MSG0_ERROR("Err in Adding Flow Cntx to Bearer Flow mgr");
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);                            
                                                                                    
    ds_flow_manager_remove_bearer_context_from_flow_manager(pdn_context_p,          
                                                            bearer_cntx_p);         
    ds_bearer_flow_manager_release_flow_manager(bearer_cntx_p);                     
    if(ds_pdn_cntxt_release_bearer_context(pdn_context_p,
                                          (void *)bearer_cntx_p) !=\ 
                                           DS3G_SUCCESS)                            
    {                                                                               
      DS_3GPP_MSG0_HIGH("could not release the allocated bearer");            
    }                                                                               
    // bearer clenup required                                                    
    ds_umts_flow_manager_clean_up(pdn_context_p,                                    
                                  flow_context_p,                                   
                                  bearer_cntx_p);                                   
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);                            
                                                                                    
  }                                                                                 
****************************************************************************/


   /*-----------------------------------------------------------------------
    Update PDN cntx info in bearer cntx. Set is_default to false
    since this is a dediacted bearer for that PDN context.
   -----------------------------------------------------------------------*/
   ds_bearer_cntxt_register_pdn_context(
                                pdn_context_p, 
		                            bearer_cntx_p,
		                            FALSE
		                            );

   // Increment the number of active bearer contexts in the PDN
   pdn_context_p->ds_pdn_context_dyn_p->num_active_bearer_context++;

  /*-----------------------------------------------------------------------
    Send out the NW request. 
  -----------------------------------------------------------------------*/  
  ds_flow_context_set_flow_state(flow_context_p,
                         DS_FLOW_CONTEXT_STATE_PEND_ALLOC_RSP);  

  
  return 0;
}/* ds_umts_flow_manager_bearer_alloc_req*/

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_INCOMING_CALL

DESCRIPTION
  Handle the incoming secondary PDP context.

PARAMETERS
  pdn_context_p    -  Pointer to PDN context.
  flow_context_p   -  Pointer to Flow context.
  mode_info_ptr    -  Mode specific info ptr.


DEPENDENCIES
  None.

RETURN VALUE
  int              - -1 if it fails, 0 if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_umts_flow_manager_incoming_call
(
  ds_pdn_context_s*         pdn_context_p,
  ds_bearer_context_s*      bearer_context_p,
  const cm_call_mode_info_s_type* mode_info_ptr,
  cm_end_params_s_type*     end_params,
  sys_modem_as_id_e_type    subs_id
)
{
  tft_type_T                   *nw_tft = NULL;
  ds_bearer_flow_manager_s    *bflow_manager_p = NULL;
  ds_3gpp_iface_s             *ds_v4_iface = NULL;
  ds_3gpp_iface_s             *ds_v6_iface = NULL;
  ip_version_enum_type         ip_type; 
  ds_3gpp_internal_cause_code_enum_type icause_code=DS_3GPP_INTERNAL_CC_NONE;
  ds_flow_context_s           *flow_context_p;
  ds_flow_manager_s           *flow_manager_ptr;
  int                          ret_val;
  int16                        ps_errno;
  int                          index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  nw_tft = (tft_type_T *)modem_mem_alloc(
                            sizeof(tft_type_T),
                            MODEM_MEM_CLIENT_DATA_CRIT );

  if (nw_tft == NULL ) 
  {
    DS_3GPP_MSG0_ERROR("FAIL modem_mem_alloc for ds_umts_flow_manager_incoming_call");
    return -1;
  }
  
  memset(nw_tft,0x0,sizeof(tft_type_T));
  memscpy(nw_tft, sizeof(tft_type_T),
          &(mode_info_ptr->info.gw_ps_call.tft),
          sizeof(tft_type_T));


  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
    return FALSE;
  }

  /* param validation */
  /*-----------------------------------------------------------------------
     Make sure BCM is NW/UE
  -----------------------------------------------------------------------*/
  flow_manager_ptr = ds_flow_manager_find_flow_manager(pdn_context_p);

  if(!ds_flow_manager_validate_flow_manager(flow_manager_ptr))
  {
    modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
    return FALSE;
  }
  else if(flow_manager_ptr->ds_flow_manager_dyn_p->bcm_info != DS_3GPP_BCM_UE_NW)
  {
    DS_3GPP_MSG0_HIGH("BCM violation, call rejected");
    end_params->end_params.gw_ps_end.cause = SM_ACTIVATION_REJECTED_BCM_VIOLATION;
    modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
    return FALSE;
  }

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
    return FALSE;
  }


  /* validate NW param */
  if(nw_tft->valid)
  {
    /*-----------------------------------------------------------------------
       Make sure this is a create/NOOP operation.
    -----------------------------------------------------------------------*/
    if(nw_tft->operation_code !=(byte)DS_3GPP_TFT_OPCODE_CREATE && 
       nw_tft->operation_code !=(byte)DS_3GPP_TFT_OPCODE_NO_OP)
    {
      DS_3GPP_MSG0_ERROR("invalid TFT operation");
      end_params->end_params.gw_ps_end.cause = SM_SEMANTIC_ERR_IN_TFT;
      modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
      return FALSE;
    }
  
    /*-----------------------------------------------------------------------
       Check for TFT Semantic Errors for create case
    -----------------------------------------------------------------------*/
    if(nw_tft->operation_code == (byte)DS_3GPP_TFT_OPCODE_CREATE)
    {
      if(ds_3gpp_tft_check_mod_fltr_semantic_error(nw_tft) == TRUE)
      {
        end_params->end_params.gw_ps_end.cause = SM_SYNTACTIC_ERR_IN_TFT;
        modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
        return FALSE;
      }
      else if(ds_3gpp_tft_check_for_repeated_precedence(nw_tft) == TRUE)
      {
        end_params->end_params.gw_ps_end.cause = SM_SEMANTIC_ERR_IN_FILTER;
        modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
        return FALSE;
      }
   }
    /*-----------------------------------------------------------------------
       HACK: due to the fact that SM process identifier byte differently
       than ESM, in order to use the common function to process TFT, copy
       direction bits back to identifier field. 
    -----------------------------------------------------------------------*/
    for(index=0; index < nw_tft->num_filters; index++)
    {
      if (((nw_tft->filter_list[index].identifier) & (0xF0)) == 0)
      {
        nw_tft->filter_list[index].identifier |= 
                                ((nw_tft->filter_list[index].direction) << 4);

      }
    }
  }

  /*-------------------------------------------------------------------------
    Allocate a new bearer flow manager.
  -------------------------------------------------------------------------*/
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  bflow_manager_p = 
              ds_bearer_flow_manager_allocate_flow_manager(bearer_context_p);

  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
      ds_3gpp_dyn_mem_hdlr_free_bearer_flow_mgr_dyn_mem(bearer_context_p);
      end_params->end_params.gw_ps_end.cause = SM_INSUFFICIENT_RESOURCES;
      modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return FALSE;
  }
  
  DS_3GPP_MSG0_HIGH("Bearer Allocated a bearer flow manager");
  
  /*---------------------------------------------------------------------
   Update the PDN PTR.
  ---------------------------------------------------------------------*/
  bflow_manager_p->ds_bearer_flow_manager_dyn_p->pdn_ptr = pdn_context_p;

  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

  /*-------------------------------------------------------------------------
    Find out the PDP Type supported by this PDN.
  -------------------------------------------------------------------------*/
  switch(ds_pdn_cntxt_get_pdn_context_ip_support(pdn_context_p))
  {
    case DS_UMTS_PDP_IPV4:
    case DS_UMTS_PDP_IPV4V6: // for v4v6 case, we will setup v4 flow first
      ds_v4_iface = 
       pdn_context_p->ds_pdn_context_dyn_p->
        ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE];
      ip_type = IP_V4; 
      if(ds_flow_manager_create_flow_context_per_iface(
                                   pdn_context_p,
                                   bearer_context_p,
                                   mode_info_ptr,
                                   nw_tft,
                                   ds_v4_iface,
                                   ip_type,
                                   &icause_code,
                                   FALSE) != DS3G_SUCCESS)
      {
      
        DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        ds_bearer_flow_manager_release_flow_manager(bearer_context_p);
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        if( (icause_code == DS_3GPP_INTERNAL_CC_NONE) ||
            (icause_code == DS_3GPP_INTERNAL_CC_INVALID))
        {
          DS_3GPP_MSG0_ERROR("cause code not set");
          modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
          return FALSE;
        }

        if(ds_flow_manager_convert_cause_codes
             (mode_info_ptr, &icause_code, subs_id)== FALSE)
        {
          DS_3GPP_MSG0_ERROR("Error in converting cause code");
        } 
        else
        {
          /* update cause to end param */
          end_params->end_params.gw_ps_end.cause = mode_info_ptr->info.gw_ps_call.cause.ext_cause;
        }
        modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
        return FALSE;
      }
      break; 
    case DS_UMTS_PDP_IPV6:
      ds_v6_iface =
       pdn_context_p->ds_pdn_context_dyn_p->
        ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE];
      ip_type = IP_V6;
      if(ds_flow_manager_create_flow_context_per_iface(
                                   pdn_context_p,
                                   bearer_context_p,
                                   mode_info_ptr,
                                   nw_tft,
                                   ds_v6_iface,
                                   ip_type,
                                   &icause_code,
                                   FALSE) != DS3G_SUCCESS)
      {
		  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        ds_bearer_flow_manager_release_flow_manager(bearer_context_p);
		  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        if( (icause_code == DS_3GPP_INTERNAL_CC_NONE) ||
            (icause_code == DS_3GPP_INTERNAL_CC_INVALID))
        {
          DS_3GPP_MSG0_ERROR("cause code not set");
          modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
          return FALSE;
        }
        /* TODO: add cause code mapping
        if(ds_flow_manager_convert_cause_codes(mode_info_p,icause_code)==FALSE)
        {
          DS_3GPP_MSG0_ERROR("Error in converting cause code");
        } 
        */ 
        modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
        return FALSE;
      } 
      break;
    default:
      DS_3GPP_MSG0_ERROR("Unsuported PDP Type in profile");
	  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      ds_bearer_flow_manager_release_flow_manager(bearer_context_p);
	  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
      return FALSE;
  }/* switch Profile's PDP TYPE */

  /* store the NW QOS in blow mgr */
  ds_umts_bearer_flow_manager_store_nw_data_rate(&(mode_info_ptr->
                                                   info.gw_ps_call), 
                                                 bflow_manager_p);

  flow_context_p = ds_flow_manager_find_pending_flow_context_for_pdn(
                                                   pdn_context_p,
                                                   bearer_context_p->
                                                   ds_bearer_context_dyn_p->sdf_id);


  ds_flow_context_set_flow_state(flow_context_p,
                         DS_FLOW_CONTEXT_STATE_PEND_ALLOC_RSP);  
  /* set bearer state to answer */
  bearer_context_p->ds_bearer_context_dyn_p->state = DS_BEARER_CONTEXT_STATE_ANSWER;

  ret_val = ps_phys_link_up_cmd(&(bearer_context_p->phys_link),
                                &ps_errno,NULL);        
               
  if (ret_val == 0)
  {
    DS_3GPP_MSG0_HIGH(" Bringing up phys link when already up / coming up:");
  }
  else if ( ps_errno != DS_EWOULDBLOCK)
  {
    DS_3GPP_MSG1_ERROR("ps_phys_link_up_cmd failed errno:%d",ps_errno);
	DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    ds_bearer_flow_manager_release_flow_manager(bearer_context_p);  
	DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    ds_flow_manager_remove_pending_flow_context(pdn_context_p, flow_context_p);
    ds_flow_context_release_flow_context(flow_context_p);
    modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
    return FALSE;
  }

  /*-------------------------------------------------------------------
   Add bearer context to flow manager.       
   VR:TODO untie in case of err
  -------------------------------------------------------------------*/
  if (ds_flow_manager_add_bearer_context_to_flow_manager(
                     pdn_context_p,bearer_context_p) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Error in Adding bearer context to Flow manager");  
	DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    ds_bearer_flow_manager_release_flow_manager(bearer_context_p);    
	DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    ds_flow_manager_remove_pending_flow_context(pdn_context_p, flow_context_p);
    ds_flow_context_release_flow_context(flow_context_p);      
  }

   // Increment the number of active bearer contexts in the PDN
   pdn_context_p->ds_pdn_context_dyn_p->num_active_bearer_context++;
   modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
  return TRUE;
}


/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_BEARER_ALLOC_ABORT_REQ

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
int ds_umts_flow_manager_bearer_alloc_abort_req
(
  ds_flow_context_s             *flow_context_p
)
{

#if 0
  cm_res_alloc_abort_s_type        alloc_abort;   
  ds_pdn_context_s                 *pdn_context_p = NULL; 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&alloc_abort, 0, sizeof(cm_res_alloc_abort_s_type));
  if(flow_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input ptr for flow context is NULL");
    return  -1;
  }

  /*---------------------------------------------------------------------
    Update the params and send out the NW request.
  ---------------------------------------------------------------------*/
  alloc_abort.sdf_id = flow_context_p->sdf_id;
  alloc_abort.esm_cause.valid = TRUE;
  alloc_abort.esm_cause.esm_cause = NAS_ESM_REGULAR_DEACTIVATION;
  alloc_abort.esm_local_cause.valid = FALSE;
 

  if(ds3g_msh_abort_bearer_req((void *)(&alloc_abort)) == DS3G_FAILURE)
  {
    DS_3GPP_MSG1_ERROR("DS3G Bearer Alloc Abort req failed for sdf_id:0x%x",
               flow_context_p->sdf_id); 
    /*---------------------------------------------------------------------
      Do a local clean up of the flow context and return success.
    ---------------------------------------------------------------------*/
    flow_context_p = ds_flow_manager_find_pending_flow_context(
                                  &pdn_context_p,flow_context_p->sdf_id);
    if(flow_context_p == NULL)
    {
      DS_3GPP_MSG1_ERROR("Could not find the flow context matching SDF ID:0x%x",
                                          flow_context_p->sdf_id);
      return 0;
    }
    if(pdn_context_p == NULL)
    {
      DS_3GPP_MSG0_ERROR("PDN context pointer is not updated");
      return 0;
    } 
    /*---------------------------------------------------------------------
      Found the pending flow context. Remove the flow context.
    ---------------------------------------------------------------------*/
    if(ds_flow_manager_update_precedence_bit_mask(
                       pdn_context_p,flow_context_p,FALSE,FALSE,TRUE)==FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
    }
    if(ds_flow_manager_remove_pending_flow_context(
                  pdn_context_p,flow_context_p) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Could not remove Pending Flow Context");
    }
    ds_flow_context_release_flow_context(flow_context_p);
    return 0;
  } /* if abort_bearer_req failed */

  /*---------------------------------------------------------------------
    Update the Flow state
  ---------------------------------------------------------------------*/
  ds_flow_context_set_flow_state(flow_context_p,
                DS_FLOW_CONTEXT_STATE_PEND_ALLOC_ABORT_RSP);
#endif
  return 0;

}/* ds_umts_flow_manager_bearer_alloc_abort_req */


/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_NW_BEARER_MODIFY_IND

DESCRIPTION
  Handle the network requested bearer modification Indication.

PARAMETERS 
  call_id         -  CM allocated call identifier.
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_umts_flow_manager_nw_bearer_modify_ind
(
  cm_call_id_type           call_id,
  cm_call_mode_info_s_type* mode_info_ptr
)
{
  boolean                                 return_value = FALSE;
  ds_bearer_context_s                     *bearer_context_p = NULL;
  ds_3gpp_iface_s                         *ds3gpp_iface_v4_p = NULL; 
                                            /* Pointer to the DS v4 Iface */
  #ifdef FEATURE_DATA_PS_IPV6           
  ds_3gpp_iface_s                         *ds3gpp_iface_v6_p = NULL; 
                                            /* Pointer to the DS v6 Iface */
  #endif                                
  ds_umts_pdp_type_enum_type               pdn_pdp_type; 
  ds_pdn_context_s                        *pdn_context_p = NULL;
  cm_gw_ps_call_info_s_type               *gw_ps_call_info = NULL;
  ds_3gpp_internal_cause_code_enum_type    icause_code = 
                                             DS_3GPP_INTERNAL_CC_INVALID;
#ifdef FEATURE_DATA_REL10
  byte                                    *apn = NULL;
#endif /*FEATURE_DATA_REL10*/
  sys_modem_as_id_e_type                   subs_id  = SYS_MODEM_AS_ID_NONE;
  uint32                                   rat_mask = 
                                             DS_3GPP_THROT_RAT_MASK_GLOBAL;
                                           /* Mask of RATs on which throttling
                                              needs to be unblocked */
  boolean                                  is_attach_apn = FALSE;
  /*------------------------------------------------------------------------*/

  if(mode_info_ptr == NULL )
  {
    DS_3GPP_MSG0_ERROR("Input parameter is NULL");
    return return_value;
  }

  /*-------------------------------------------------------------------------
    Get the bearer context and the PDN context
  -------------------------------------------------------------------------*/
  bearer_context_p= 
                ds_bearer_cntxt_get_bearer_from_call_id(call_id);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return DS3G_FAILURE;
  }
  DS_3GPP_MSG1_HIGH("NW mod ind for bearer is_default flag: %d", 
                   bearer_context_p->ds_bearer_context_dyn_p->is_default);
  gw_ps_call_info =  (cm_gw_ps_call_info_s_type*)
                     &(mode_info_ptr->info.gw_ps_call);

  if ( gw_ps_call_info->tft.valid == TRUE )
  {
    return_value = 
      ds_flow_manager_accept_modify_enable_filter(bearer_context_p);
  }
  else
  {
    /* NW only modified the qos param, inform PS by set granted flow */
    return_value = ds_flow_manager_update_data_rate_in_flow(bearer_context_p, 
                                                            mode_info_ptr,
                                                            &icause_code);
  }


  /*-------------------------------------------------------------------------
    Decode PCO as it could contain BCM update
  -------------------------------------------------------------------------*/
  pdn_context_p = (ds_pdn_context_s*)bearer_context_p->ds_bearer_context_dyn_p->
     pdn_context_ptr; 


  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p,&subs_id))
  {
    return DS3G_FAILURE;
  }

  DS_3GPP_MSG1_HIGH("UMTS Flow manager received NW Context "
                    "Modification Ind on sub id %d",subs_id);
  pdn_pdp_type = ds_pdn_cntxt_get_pdn_context_ip_support(pdn_context_p);
  if(pdn_pdp_type == DS_UMTS_PDP_PPP || pdn_pdp_type == DS_UMTS_PDP_IPV4)
  {
    ds3gpp_iface_v4_p = 
            (pdn_context_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE]);
    DS_3GPP_MSG0_HIGH("PDP type IPV4 is selected in setupPdpIP"); 
  }
  #ifdef FEATURE_DATA_PS_IPV6
    else if(pdn_pdp_type == DS_UMTS_PDP_IPV6)
    {
      ds3gpp_iface_v6_p = 
          (pdn_context_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE]);
      DS_3GPP_MSG0_HIGH("PDP type IPV6 is selected in setupPdpIP"); 
    }
    else if (pdn_pdp_type == DS_UMTS_PDP_IPV4V6)
    {
      ds3gpp_iface_v4_p = 
          (pdn_context_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE]);
      ds3gpp_iface_v6_p = 
          (pdn_context_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE]);
    }
  #endif /* FEATURE_DATA_PS_IPV6 */
  else
  {
    DATA_ERR_FATAL("PDP type is not PPP or IP V4 or IP V6. Dual IP bearer not sup yet");
  }
  
  ds_pdn_cntxt_decodeProtocolParams(ds3gpp_iface_v4_p,ds3gpp_iface_v6_p,
                                    &gw_ps_call_info->config_options);

 if(ds_3gpp_nv_manager_is_umts_rel_10_enabled())
 { 
    /*---------------------------------------------------------------------
     PDP context modification got successfully processed by UE.
     Clear APN throttling state machine if it was globally blocked.         
     Unblocking apn irespective of whether modification was succesful                                                             .         
    ----------------------------------------------------------------------*/
    apn = pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn;

    if(*apn == '\0')
    {
      (void)ds_pdn_cntxt_get_resolved_apn_name(pdn_context_p, &apn);
    }
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,
                          "Trying to unblock potentially blocked APN: %s",
                          (char*)apn);
  if (ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat(subs_id))
  {
    rat_mask |= ds_3gpp_map_cm_sys_mode_to_rat_mask
                (bearer_context_p->ds_bearer_context_dyn_p->call_mode);
  }

  ds_3gpp_pdn_throt_handle_nw_inited_unblock_apn (apn, subs_id, rat_mask);

  if (ds_dsd_apm_is_apn_in_attach_pdn_list(subs_id, (char*)apn))
  {
    is_attach_apn = TRUE;
  }
#ifdef FEATURE_DATA_LTE
  if(is_attach_apn)
  {
    ds_3gpp_pdn_cntxt_enable_lte_if_attach_profile_available(subs_id);
  }
#endif /* FEATURE_DATA_LTE */

}//ds_3gpp_pdn_get_nv_lte_rel_10

  
  dsUmtsBearerCntxt_SendQOSIndication(bearer_context_p, 
                                      mode_info_ptr->info.gw_ps_call.sys_rat);
  /* ---------------------------------------------------------------------
   if tear down on modify ind set for NW inti bearer call phys_link_down_cmd_hdlr
    to  deactivate the bearer 
   ---------------------------------------------------------------------*/
  if(bearer_context_p->ds_bearer_context_dyn_p->teardown_on_modify_ind == TRUE)
  {
    ds_bearer_cntxt_phys_link_down_cmd_hdlr(
	  (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr,
	  	                          NULL);
  }

  return return_value;                                
}/*ds_umts_flow_manager_nw_bearer_modify_ind */

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_HANDLE_CREATE_TFT

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
boolean ds_umts_flow_manager_handle_create_tft
(
  ds_pdn_context_s         *pdn_context_p,
  ds_bearer_context_s      *bearer_context_p,
  const tft_type_T         *nw_tft_p,
  const cm_call_mode_info_s_type  *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{    
  int                      result = DS3G_FAILURE;    

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
     return FALSE; 
  }
  
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

  //Set the capability of the default flow to PS_FLOW_CAPABILITY_DATA_DISALLOWED. done in add filter
  // Packets not matching the installed filters will be dropped.

  result = ds_flow_manager_add_filter_to_tft( pdn_context_p,
                                              bearer_context_p,
                                              NULL,
                                              (tft_type_T *)nw_tft_p,
                                              mode_info_p,
                                              cause_code_p,
                                              TRUE
                                            );
  if (result != DS3G_SUCCESS )
  {
    DS_3GPP_MSG0_ERROR("add_filter_to_tft: failed ");
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);     
    return DS3G_FAILURE;
  }

  result = DS3G_SUCCESS;

  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  return result;

}

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_HANDLE_DELETE_EXISTING_TFT

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
boolean ds_umts_flow_manager_handle_delete_existing_tft
(
  ds_pdn_context_s         *pdn_context_p,
  ds_bearer_context_s      *bearer_context_p,
  const tft_type_T         *nw_tft_p,
  const cm_call_mode_info_s_type  *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{      
  int                       i;
  int                       result = DS3G_FAILURE;
  ds_3gpp_iface_s*          ds_iface_p = NULL;
  ps_iface_type*            ps_iface = NULL;
  ps_flow_type*             ps_flow_ptr = NULL;
  
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }
  if (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    return FALSE;
  }

  
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

  do
  {    
    //Release all the flow contexts associated with this bearer.
    result = ds_flow_manager_free_nw_flow_context(bearer_context_p);    
    if (result != DS3G_SUCCESS)
    {
      if(cause_code_p != NULL)
      {
        *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
      }
      break;
    }

    //Reset the capability of the default flow from PS_FLOW_CAPABILITY_DATA_DISALLOWED.
    //No packet on default flow will be dropped.  
    for (i = 0; i < DS_3GPP_MAX_IFACE_PER_PDN_CONTEXT; i++)
    {
      ds_iface_p = pdn_context_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[i];
      if (ds_iface_p != NULL)
      {              
        ps_iface = &(ds_iface_p->ps_iface);    
        ps_flow_ptr = PS_IFACE_GET_DEFAULT_FLOW(ps_iface);
        if (ps_flow_ptr != NULL) 
        {
          PS_FLOW_RESET_CAPABILITY(ps_flow_ptr, PS_FLOW_CAPABILITY_DATA_DISALLOWED);
        }
           
      }
    }
    result = DS3G_SUCCESS;
    
  } while(0);
  
  if(FALSE == bearer_context_p->ds_bearer_context_dyn_p->is_default)
  {
	  DS_3GPP_MSG0_HIGH("bearer mode is UMTS. will be deactivated on modify IND");
	  bearer_context_p->ds_bearer_context_dyn_p->teardown_on_modify_ind = TRUE;
	  ps_phys_link_down_ind_ex(&(bearer_context_p
							  ->phys_link), 
							PS_EIC_NOT_SPECIFIED); 

  }
  
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  return result;
} /* ds_umts_flow_manager_handle_delete_existing_tft */

/*===========================================================================
FUNCTION DS_UMTS_FLOW_MANAGER_INIT

DESCRIPTION
  Register UMTS specific functions

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_umts_flow_manager_init( void )
{
  ds_3gpp_flow_manager_ftbl_s  *flow_manager_ftbl_p = NULL;  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Initialize WCDMA flow manager Function table
  -------------------------------------------------------------------------*/
#ifdef FEATURE_WCDMA
  flow_manager_ftbl_p = &(ds_flow_manager_ftbl[DS_3GPPI_SYS_MODE_WCDMA]);
  memset(
         (void*)flow_manager_ftbl_p,
         0x0,
         (sizeof(ds_3gpp_flow_manager_ftbl_s))
        );

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, checkNetworkInput,
                     ds_umts_flow_manager_check_nw_input);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, convertCauseCodes,
                     ds_umts_flow_manager_convert_cause_code);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, NwBearerAllocRej,
                     ds_umts_flow_manager_bearer_alloc_rej);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, NwBearerAllocFail,
                     ds_umts_flow_manager_bearer_alloc_fail);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, TearDownSecBearer,
                     ds_umts_flow_manager_tear_down_sec_bearer);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, NwContextModifyRej,
                     ds_umts_flow_manager_bearer_modify_rej);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, NwContextModifyInd,
                     ds_umts_flow_manager_nw_bearer_modify_ind);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, NwContextModifyReq,
                     ds_umts_flow_manager_nw_bearer_modify_req);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, NwBearerModifyInd,
                     ds_umts_flow_manager_bearer_modify_ind);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, NwContextModifyCnf,
                     ds_umts_flow_manager_bearer_modify_cnf);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, NwBearerAllocationInd,
                     ds_umts_flow_manager_bearer_allocation_ind);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, NwBearerActivationReq,
                     ds_umts_flow_manager_incoming_call);
  
  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, UeBearerAllocReq,
                     ds_umts_flow_manager_bearer_alloc_req);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, UeBearerAllocAbortReq,
                     ds_umts_flow_manager_bearer_alloc_abort_req);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, UeBearerModifyReq,
                     ds_umts_flow_manager_bearer_modify_req);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, ProcessCreateTFT,
                     ds_umts_flow_manager_handle_create_tft);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_WCDMA, ProcessDeleteTFT,
                     ds_umts_flow_manager_handle_delete_existing_tft);

#endif /*FEATURE_WCDMA */

  /*------------------------------------------------------------------------
    Initialize TDCDMA flow manager Function table
  -------------------------------------------------------------------------*/
#ifdef FEATURE_TDSCDMA
  flow_manager_ftbl_p = &(ds_flow_manager_ftbl[DS_3GPPI_SYS_MODE_TDS]);
  memset(
         (void*)flow_manager_ftbl_p,
         0x0,
         (sizeof(ds_3gpp_flow_manager_ftbl_s))
        );

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, checkNetworkInput,
                     ds_umts_flow_manager_check_nw_input);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, convertCauseCodes,
                     ds_umts_flow_manager_convert_cause_code);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, NwBearerAllocRej,
                     ds_umts_flow_manager_bearer_alloc_rej);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, NwBearerAllocFail,
                     ds_umts_flow_manager_bearer_alloc_fail);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, TearDownSecBearer,
                     ds_umts_flow_manager_tear_down_sec_bearer);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, NwContextModifyRej,
                     ds_umts_flow_manager_bearer_modify_rej);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, NwContextModifyInd,
                     ds_umts_flow_manager_nw_bearer_modify_ind);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, NwContextModifyReq,
                     ds_umts_flow_manager_nw_bearer_modify_req);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, NwBearerModifyInd,
                     ds_umts_flow_manager_bearer_modify_ind);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, NwContextModifyCnf,
                     ds_umts_flow_manager_bearer_modify_cnf);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, NwBearerAllocationInd,
                     ds_umts_flow_manager_bearer_allocation_ind);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, NwBearerActivationReq,
                     ds_umts_flow_manager_incoming_call);
  
  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, UeBearerAllocReq,
                     ds_umts_flow_manager_bearer_alloc_req);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, UeBearerAllocAbortReq,
                     ds_umts_flow_manager_bearer_alloc_abort_req);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, UeBearerModifyReq,
                     ds_umts_flow_manager_bearer_modify_req);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, ProcessCreateTFT,
                     ds_umts_flow_manager_handle_create_tft);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_TDS, ProcessDeleteTFT,
                     ds_umts_flow_manager_handle_delete_existing_tft);

#endif /*FEATURE_TDSCDMA */

#ifdef FEATURE_GSM_GPRS
  /*------------------------------------------------------------------------
    Initialize GSM flow manager Function table
  -------------------------------------------------------------------------*/
  flow_manager_ftbl_p = &(ds_flow_manager_ftbl[DS_3GPPI_SYS_MODE_GSM]);
  memset(
         (void*)flow_manager_ftbl_p,
         0x0,
         (sizeof(ds_3gpp_flow_manager_ftbl_s))
        );

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, checkNetworkInput,
                     ds_umts_flow_manager_check_nw_input);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, convertCauseCodes,
                     ds_umts_flow_manager_convert_cause_code);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, NwBearerAllocRej,
                     ds_umts_flow_manager_bearer_alloc_rej);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, NwBearerAllocFail,
                     ds_umts_flow_manager_bearer_alloc_fail);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, TearDownSecBearer,
                     ds_umts_flow_manager_tear_down_sec_bearer);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, NwContextModifyRej,
                     ds_umts_flow_manager_bearer_modify_rej);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, NwContextModifyInd,
                     ds_umts_flow_manager_nw_bearer_modify_ind);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, NwContextModifyReq,
                     ds_umts_flow_manager_nw_bearer_modify_req);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, NwBearerModifyInd,
                     ds_umts_flow_manager_bearer_modify_ind);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, NwContextModifyCnf,
                     ds_umts_flow_manager_bearer_modify_cnf);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, NwBearerAllocationInd,
                     ds_umts_flow_manager_bearer_allocation_ind);
  
  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, NwBearerActivationReq,
                     ds_umts_flow_manager_incoming_call);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, UeBearerAllocReq,
                     ds_umts_flow_manager_bearer_alloc_req);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, UeBearerAllocAbortReq,
                     ds_umts_flow_manager_bearer_alloc_abort_req);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, UeBearerModifyReq,
                     ds_umts_flow_manager_bearer_modify_req);
  
  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, ProcessCreateTFT,
                     ds_umts_flow_manager_handle_create_tft);

  DS_FLOW_MGR_VF_REG(SYS_SYS_MODE_GSM, ProcessDeleteTFT,
                     ds_umts_flow_manager_handle_delete_existing_tft);
#endif /* FEATURE_GSM_GPRS */
  
} /* ds_umts_flow_manager_init */


#endif /*FEATURE_DATA_LTE */
