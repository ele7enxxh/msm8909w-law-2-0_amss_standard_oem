/*!
  @file
  ds_3gpp_tft.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2010-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_tft.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/15/13   nd      If secondary PDP activation request contains more than 
                   one filter with same filter id UE should reject the request.
04/03/10   vk      Added support for MSG2.0 based logging
02/04/10   vd      Using the changed flow function names.
01/27/19   vd      Using changed flow manager function names.
01/20/10   vd      Initial version.
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_3GPP
#include "ds_3gpp_bearer_context.h"
#include "ds_3gpp_pdn_context.h"
#include "dstask_v.h"
#include "ds3gmgr.h"
#include "ds_3gpp_flow_context.h"
#include "ds_3gpp_flow_manager.h"
#include "ds_flow_control.h"
#include "modem_mem.h"
#include "ran.h"
#include "ds_eps_bearer_context.h"

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
FUNCTION DS_3GPP_TFT_SET_FILTER_PRECED_BIT

DESCRIPTION
  Set the filter precedence bit in the given precedence bit mask.

PARAMETERS
  fi_preced         - Filter precedence.
  preced_bit_mask   - Precedence bit mask to be updated.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_tft_set_filter_preced_bit
(
  uint16 fi_preced,
  uint32    *preced_bit_mask
)
{
  uint16 index;
  uint32 *mask = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (fi_preced > DS_MAX_FI_PRECED)
  {
    DS_3GPP_MSG1_ERROR("Invalid filter precedence 0x%x received, ignored.",
                        fi_preced);
    return;
  }
  if(preced_bit_mask == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input parameter precedence bit mask is NULL");
    return;
  }

  if(fi_preced == 0)
  {
    index = 0;
  }
  else
  {
    index = fi_preced/32;
  }
  DS_3GPP_MSG1_HIGH("Set 0x%x for Precedence",fi_preced);
  mask = &preced_bit_mask[index];
  if(fi_preced == 0)
  {
    DS_SET_FI_BIT(*mask, 0);
  }
  else
  {
    DS_SET_FI_BIT((*mask), (fi_preced%32));
  }
  return;
} /* ds_3gpp_tft_set_filter_preced_bit */

/*===========================================================================
FUNCTION DS_3GPP_TFT_CLEAR_FILTER_PRECED_BIT

DESCRIPTION
  Clear the filter precedence bit in the given precedence bit mask

PARAMETERS
  fi_preced         - Filter precedence.
  preced_bit_mask   - Precedence bit mask to be updated.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_tft_clear_filter_preced_bit
(
  uint16 fi_preced,
  uint32    *preced_bit_mask
)
{
  uint16 index;
  uint32 *mask = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (fi_preced > DS_MAX_FI_PRECED)
  {
    DS_3GPP_MSG1_ERROR("Invalid filter precedence 0x%x received, ignored.",
                        fi_preced);
    return;
  }

  if(preced_bit_mask == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input parameter precedence bit mask is NULL");
    return;
  }

  if(fi_preced == 0)
  {
    index = 0;
  }
  else
  {
    index = fi_preced/32;
  }
  DS_3GPP_MSG1_HIGH("Cleared 0x%x as Precedence",fi_preced);
  mask = &preced_bit_mask[index];
  if(fi_preced == 0)
  {
    DS_CLEAR_FI_BIT((*mask), 0);
  }
  else
  {
    DS_CLEAR_FI_BIT((*mask),(uint16)(fi_preced%32));
  }
  return;
}/* ds_3gpp_tft_clear_filter_preced_bit*/

/*===========================================================================
FUNCTION dFLOWMANAGER_ISFILTERPRECEDBITSET

DESCRIPTION
  Return whether the filter precedence is in use for a give precedence bit mask

PARAMETERS
  fi_preced         - Filter precedence.
  preced_bit_mask   - Precedence bit mask to be updated.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - the bit for this filter precedence is set
  FALSE - otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_is_filter_preced_bit_set
(
  uint16 fi_preced,
  uint32    *preced_bit_mask
)
{
  uint16 index;
  uint32 *mask = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (fi_preced > DS_MAX_FI_PRECED)
  {
    DS_3GPP_MSG1_ERROR("Invalid filter precedence 0x%x received, ignored.",
                        fi_preced);
    return FALSE;
  }
  if(preced_bit_mask == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input parameter preced bit mask is NULL");
    return FALSE;
  }
  if(fi_preced == 0)
  {
    index = 0;
  }
  else
  {
    index = fi_preced/32;
  }
  mask = &preced_bit_mask[index];
  if(fi_preced == 0)
  {
    return (DS_IS_FI_BIT_SET((*mask), 0)); 
  }
  else
  {
    return (DS_IS_FI_BIT_SET((*mask),(fi_preced%32)));  
  }
}/* ds_3gpp_tft_is_filter_preced_bit_set */


/*===========================================================================
FUNCTION DS_3GPP_TFT_IS_FILTER_IDENTIFIER_REQUESTED

DESCRIPTION
  Return whether this filter identifier is already in use.

PARAMETERS
  fi_identifier       - Filter identifier.
  fi_bit_mask         - Filter bitmask to check filter identifier 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - the bit for this filter identifier is set
  FALSE - otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_is_filter_identifier_requested
(
  uint16 fi_identifier,
  uint32    *fi_bit_mask
)
{
  uint32 mask = 0;

  if (fi_identifier > DS_MAX_FI_ID)
    {
    DS_3GPP_MSG1_ERROR("Invalid filter identifier: %d received, ignored.",
                        fi_identifier);
    return FALSE;
  }
  mask = 1 << fi_identifier;
  
  if (((*fi_bit_mask) & mask) == mask)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* ds_3gpp_tft_is_filter_identifier_requested */

/*===========================================================================
FUNCTION DS_3GPP_TFT_SET_FILTER_IDENTIFIER_BIT

DESCRIPTION
  Set the bit for this filter identifier.

PARAMETERS
  fi_identifier       - Filter identifier.
  fi_bit_mask         - Filter bitmask to check filter identifier 

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_tft_set_filter_identifier_bit
(
  uint16 fi_identifier,
  uint32    *fi_bit_mask
)
{
  uint32    mask = 0;
  mask = 1 << fi_identifier;
  *fi_bit_mask |= mask;
  return;
} /* ds_3gpp_tft_set_filter_identifier_bit */

/*===========================================================================
FUNCTION DS_3GPP_CLEAR_FILTER_IDENTIFIER_BIT

DESCRIPTION
  unset the bit for this filter identifier.

PARAMETERS
  fi_identifier       - Filter identifier.
  fi_bit_mask         - Filter bitmask to check filter identifier 

DEPENDENCIES
  None.

RETURN VALUE
 None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_tft_clear_filter_identifier_bit
(
  uint16 fi_identifier,
  uint32    *fi_bit_mask
)
{
  uint32    mask = 0;
  mask = 1 << fi_identifier;
  *fi_bit_mask &= (~mask);
  return;
} /* ds_3gpp_tft_clear_filter_identifier_bit */


/*===========================================================================
FUNCTION DS_3GPP_TFT_CONFIGURE_NW_BK_FLOW_SPEC

DESCRIPTION
  Configure the flow spec and filters in the Network book keeping 
  data strutcure.

PARAMETERS
  nw_bk                       -  Boook keeping info.
                      
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_configure_nw_bk_flow_spec
(
  ds_3gpp_nw_modify_qos_bk_s      *nw_bk
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(nw_bk ==NULL)
  {
    DS_3GPP_MSG1_ERROR("Invalid Input. nw_bk:0x%x",nw_bk);
    return FALSE;
  }

  /*------------------------------------------------------------------------
    Clear the memory.
  -------------------------------------------------------------------------*/
  memset(&(nw_bk->curr_qos),0, sizeof(qos_spec_type));                      
  memset(nw_bk->curr_rx_fltr,0,sizeof(ip_filter_type)*MAX_PACKET_FILTERS);   
  memset(nw_bk->curr_tx_fltr,0,sizeof(ip_filter_type)*MAX_PACKET_FILTERS);

  memset(&(nw_bk->mod_qos),0, sizeof(qos_spec_type));                      
  memset(nw_bk->mod_rx_fltr,0,sizeof(ip_filter_type)*MAX_PACKET_FILTERS);   
  memset(nw_bk->mod_tx_fltr,0,sizeof(ip_filter_type)*MAX_PACKET_FILTERS);

  /*------------------------------------------------------------------------
     Assign Filter Space
  -------------------------------------------------------------------------*/
  nw_bk->curr_qos.rx.fltr_template.list_ptr = nw_bk->curr_rx_fltr;
  nw_bk->curr_qos.tx.fltr_template.list_ptr = nw_bk->curr_tx_fltr;

  nw_bk->mod_qos.rx.fltr_template.list_ptr = nw_bk->mod_rx_fltr;
  nw_bk->mod_qos.tx.fltr_template.list_ptr = nw_bk->mod_tx_fltr;

  return TRUE;
}/* ds_3gpp_tft_configure_nw_bk_flow_spec */

/*===========================================================================
FUNCTION DS_3GPP_TFT_CHECK_MOD_FLTR_SEMANTIC_ERROR

DESCRIPTION
  Verify if the input TFT has any semantic errors.

PARAMETERS
  nw_tft                -  TFT information  

DEPENDENCIES
  None.

RETURN VALUE
  boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_check_mod_fltr_semantic_error
(
  tft_type_T                            *nw_tft  
)
{  
  int filter_idx_outer = 0, filter_idx_inner = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(nw_tft == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input NW TFT is null");
    return TRUE;
  }

  if(nw_tft->e_bit != DS_FLOW_E_BIT_DISABLE)
  {
    DS_3GPP_MSG0_ERROR("E Bit set");
    return TRUE;
  }

  if(nw_tft->num_filters <= 0)
  {
    DS_3GPP_MSG0_ERROR("Input filters is zero");
    return TRUE;
  }
  /* Fail if there are two or more filters with identical packet filter identifiers */
  for ( filter_idx_outer = 0; filter_idx_outer < nw_tft->num_filters; filter_idx_outer++ )
  {
    DS_3GPP_MSG1_ERROR("Debug Filter identifier %d",
                        nw_tft->filter_list[filter_idx_outer].identifier);
    for ( filter_idx_inner = filter_idx_outer + 1; filter_idx_inner < nw_tft->num_filters; filter_idx_inner++ )
    {
      if( (nw_tft->filter_list[filter_idx_outer].identifier & 0x0F) == 
                     (nw_tft->filter_list[filter_idx_inner].identifier & 0x0F ))
      {
        DS_3GPP_MSG1_ERROR("Input filters have two TFT with identical identifier %d",
                            nw_tft->filter_list[filter_idx_outer].identifier);
          return TRUE;
      }
    }
  }
  return FALSE;
}/* ds_3gpp_tft_check_mod_fltr_semantic_error */
/*===========================================================================
FUNCTION DS_3GPP_TFT_CHECK_FOR_REPEATED_PRECEDENCE

DESCRIPTION
  Verify if the input TFT has any semantic errors.

PARAMETERS
  nw_tft                -  TFT information  

DEPENDENCIES
  None.

RETURN VALUE
  boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_check_for_repeated_precedence
(
  tft_type_T                            *nw_tft  
)
{  
  int filter_idx_outer = 0, filter_idx_inner = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(nw_tft == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input NW TFT is null");
    return TRUE;
  }

  if(nw_tft->e_bit != DS_FLOW_E_BIT_DISABLE)
  {
    DS_3GPP_MSG0_ERROR("E Bit set");
    return TRUE;
  }

  if(nw_tft->num_filters <= 0)
  {
    DS_3GPP_MSG0_ERROR("Input filters is zero");
    return TRUE;
  }
  /* Fail if there are two or more filters with identical packet filter eval precedence */
  for ( filter_idx_outer = 0; filter_idx_outer < nw_tft->num_filters; filter_idx_outer++ )
  {
    DS_3GPP_MSG2_HIGH("Debug Filter identifier %d, precedence",
                        nw_tft->filter_list[filter_idx_outer].identifier,
                        nw_tft->filter_list[filter_idx_outer].eval_precedence);
    for ( filter_idx_inner = filter_idx_outer + 1; filter_idx_inner < nw_tft->num_filters; filter_idx_inner++ )
    {
      if( (nw_tft->filter_list[filter_idx_outer].eval_precedence) == 
                     (nw_tft->filter_list[filter_idx_inner].eval_precedence))
      {
        DS_3GPP_MSG1_ERROR("Input filters have two TFT with identical precedences %d",
                            nw_tft->filter_list[filter_idx_outer].eval_precedence);
          return TRUE;
      }
    }
  }
  return FALSE;
}/* ds_3gpp_tft_check_mod_fltr_semantic_error */

/*===========================================================================
FUNCTION DS_3GPP_TFT_CLEAR_PREC_CONFLICT_IF_INTERCHANGED_WITHIN_TFT

DESCRIPTION
   Clear the precedence conflict if the old filter (in any direction )corresponce to give
   precedence also getting changed 
eg:(<filter id>, <precedence>)
1st create: (1,1),(2,2)
now replace: (1,2)(2,1)

PARAMETERS
  preced                      -  precedence for which it has conflict
  filter_id                   -  filter id
  nw_tft_p                    -  TFT information
  nw_bk                       -  Boook keeping info.  
                      
DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
static  void ds_3gpp_tft_clear_prec_conflict_if_interchanged_within_tft
(
  uint16                      preced, 
  uint16                      filter_id, 
  tft_type_T                  *nw_tft,
  ds_3gpp_nw_modify_qos_bk_s  *nw_bk

)
{
  int                      rep_cnt=0,cnt_temp = 0;
  packet_filter_type_T	  *nw_filter_temp = NULL;
  uint16                   rep_filter_id=0, filter_id_temp =0;
  uint16                   rep_preced =0;
  ip_filter_type           *fltr = NULL;
  /*---------------------------------------------------------------------------
   Clear the precedence conflict if the old filter corresponce to this precedence 
   also getting changed
  ----------------------------------------------------------------------*/
  for(rep_cnt=0;rep_cnt<nw_bk->curr_qos.tx.fltr_template.num_filters; rep_cnt++)
  {
    fltr = &(nw_bk->curr_qos.tx.fltr_template.list_ptr[rep_cnt]);		 
    /*---------------------------------------------------------------
    extract the filter id  and precedence.
    ---------------------------------------------------------------*/
    rep_preced = fltr->ipfltr_aux_info.fi_precedence;
    rep_filter_id = fltr->ipfltr_aux_info.fi_id & (0xF);
    
    if(preced == rep_preced)
    {
      //check if rep_filter_id is present in new filters
      for(cnt_temp=0; cnt_temp< nw_tft->num_filters; cnt_temp++)
      {
        nw_filter_temp = &(nw_tft->filter_list[cnt_temp]);
        filter_id_temp = nw_filter_temp->identifier & 0xF;
        if((filter_id_temp == rep_filter_id)&&
			(ds_3gpp_tft_is_filter_preced_bit_set(preced, nw_bk->preced_conflict)))
        {
          DS_3GPP_MSG3_HIGH("changing precedence of both the filters %d, %d"
          	" dont set precedence conflict:%d", filter_id,rep_filter_id, nw_bk->preced_conflict_cnt);
          ds_3gpp_tft_clear_filter_preced_bit(preced, 
          						nw_bk->preced_conflict);
             nw_bk->preced_conflict_cnt--;
        }
      }    
    }
  } /* for tx */ 
  for(rep_cnt=0;rep_cnt<nw_bk->curr_qos.rx.fltr_template.num_filters;rep_cnt++)
  {
    fltr = &(nw_bk->curr_qos.rx.fltr_template.list_ptr[rep_cnt]);		 
    /*-----------------------------------------------------------------
      extract the filter id  and precedence.
    -----------------------------------------------------------------*/
    rep_preced = fltr->ipfltr_aux_info.fi_precedence;
    rep_filter_id = fltr->ipfltr_aux_info.fi_id & (0xF);
  
    if(preced == rep_preced)
    {
    
      //check if rep_filter_id is present in new filters
      for(cnt_temp=0; cnt_temp< nw_tft->num_filters; cnt_temp++)
      {
        nw_filter_temp = &(nw_tft->filter_list[cnt_temp]);
        filter_id_temp = nw_filter_temp->identifier & 0xF;		
        if((filter_id_temp == rep_filter_id)&&
           (ds_3gpp_tft_is_filter_preced_bit_set(preced, nw_bk->preced_conflict)))
        {
          DS_3GPP_MSG3_HIGH("changing precedence of both the filters %d, %d"
          " dont set precedence conflict", filter_id,rep_filter_id, 
          nw_bk->preced_conflict_cnt); 
          ds_3gpp_tft_clear_filter_preced_bit(preced, 
          		 nw_bk->preced_conflict);
          nw_bk->preced_conflict_cnt--;
        }
      }			  
    }
  }/*for rx*/

}/*ds_3gpp_tft_clear_prec_conflict_if_interchanged_within_tft*/


/*===========================================================================
FUNCTION DS_3GPP_TFT_CHECK_NW_FLTR_ID_PRECED

DESCRIPTION
  Verify the filter ID and precedence for each filter passed by the network.
  This checks if the filter ID or precedence is repeated.
  Filter ID conflict is detected. Precedence conflict is detected.
  Returns the filters with conflict, without conflict.
  Returns the precedence values with conflict.
  This function is called for NW initiated Qos Modification check.

PARAMETERS
  pdn_context_p               -  PDN information
  bearer_context_p            -  Bearer information
  flow_context_p              -  Flow context
  nw_tft_p                    -  TFT information
  nw_bk                       -  Boook keeping info.
  del_filter                  -  Indicate delete filter operation
                      
DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_check_nw_fltr_id_preced
(
  ds_pdn_context_s                *pdn_context_p,
  ds_bearer_context_s             *bearer_context_p,
  ds_flow_context_s               *flow_context_p,
  tft_type_T                      *nw_tft,
  ds_3gpp_nw_modify_qos_bk_s      *nw_bk,
  boolean                          del_filter
)
{
  int                       cnt = 0, rep_cnt=0;
  uint32                    temp_filter_id = 0;//filter-id repeats
  packet_filter_type_T      *nw_filter=NULL;
  uint16                    filter_id = 0, rep_filter_id=0;
  ds_bearer_flow_manager_s  *bflow_manager_p = NULL;
  ds_flow_manager_s         *flow_manager_p = NULL;
  uint16                    preced = 0, rep_preced =0;
  uint32                    temp_preced[DS_FI_PRECED_ARR_SIZE];//precd repeat
  ip_filter_type            *fltr = NULL;
  boolean                   rep_fltr_found = FALSE, replacing_same_filter = FALSE;
  byte                      direction = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(pdn_context_p==NULL||nw_tft==NULL||bearer_context_p==NULL||
      flow_context_p==NULL||nw_bk==NULL)
  {
    DS_3GPP_MSG0_ERROR("Input params are NULL in check");
    return FALSE;
  }
  /*-----------------------------------------------------------------------
     Retrieve the bearer flow manager and flow manager.
  -----------------------------------------------------------------------*/
  bflow_manager_p = 
              ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(bflow_manager_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Bearer does not have flow manager");
    return FALSE;
  }

  flow_manager_p = ds_flow_manager_find_flow_manager(pdn_context_p);
  if ((flow_manager_p == NULL) || (!ds_flow_manager_validate_flow_manager(flow_manager_p)))
  {
    return FALSE;
  }

  if( !del_filter &&  
      ((nw_tft->operation_code == DS_3GPP_TFT_OPCODE_ADD) || 
       (nw_tft->operation_code == DS_3GPP_TFT_OPCODE_REPLACE)) &&
      (PS_FLOW_IS_VALID(flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr))
     )
  {
    /*---------------------------------------------------------------------
      For replace retrieve current filter spec. 
    ---------------------------------------------------------------------*/
    /*--------------------------------------------------------------------
      Configure the qos flow spec.
    -----------------------------------------------------------------------*/
    if(ds_3gpp_tft_configure_nw_bk_flow_spec(nw_bk) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error configuring the flow spec.");
      return FALSE;
    }
        
    /*-------------------------------------------------------------------
      Retrieve the Filters
    --------------------------------------------------------------------*/
    if(ds_flow_context_ps_flow_get_filters(
                      flow_context_p, FALSE,&(nw_bk->curr_qos)) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("psFlowGetFilters failed");
      return FALSE;
    }
  }/* replace only */

  /*-----------------------------------------------------------------------
    For each filter verify if the filter-id is present in the bearer 
    identifier and also if the filter-id is present in the flow context.
  -----------------------------------------------------------------------*/
  memset(temp_preced, 0, (sizeof(uint32) *DS_FI_PRECED_ARR_SIZE));
  for(cnt=0; cnt< nw_tft->num_filters; cnt++)
  {
    nw_filter = &(nw_tft->filter_list[cnt]);
    filter_id = nw_filter->identifier & 0xF;
    preced    = nw_filter->eval_precedence;
    direction = ((nw_filter->identifier & 0x30)>>4);
    if(!del_filter)    
    {
      /*---------------------------------------------------------------------
        Verify precedence is repeated 
      ---------------------------------------------------------------------*/
      if(ds_3gpp_tft_is_filter_preced_bit_set(preced,temp_preced)==FALSE) 
      {
        ds_3gpp_tft_set_filter_preced_bit(preced, temp_preced);
      }
      else
      {
        DS_3GPP_MSG0_ERROR("Invalid NW input.precedence repeated");
        return FALSE;
      }
      /*---------------------------------------------------------------------
        Check for precedence conflict
      ---------------------------------------------------------------------*/
      if(ds_3gpp_tft_is_filter_preced_bit_set(preced,
                  flow_manager_p->ds_flow_manager_dyn_p->fi_preced)==TRUE) 
      {
        if(nw_tft->operation_code != DS_3GPP_TFT_OPCODE_CREATE)
        {
          rep_fltr_found = FALSE;
          replacing_same_filter = FALSE;

        /*check existing uplink filters*/
            for(rep_cnt=0;rep_cnt<nw_bk->curr_qos.tx.fltr_template.num_filters;
                  rep_cnt++)
            {
              fltr = &(nw_bk->curr_qos.tx.fltr_template.list_ptr[rep_cnt]);        
              /*---------------------------------------------------------------
                extract the filter id  and precedence.
              ---------------------------------------------------------------*/
              rep_preced = fltr->ipfltr_aux_info.fi_precedence;
              rep_filter_id = fltr->ipfltr_aux_info.fi_id & (0xF);
              if(filter_id == rep_filter_id)
              {
                if(preced != rep_preced)
                {
                  /*--------------------------------------------------------
                   Update only if the ID matches but preced is different. 
                  ----------------------------------------------------------*/
                  DS_3GPP_MSG3_HIGH("Precedence conflict on TX for 0x%x, prec cnt: %d filter_id:%d",
                                     preced, nw_bk->preced_conflict_cnt, filter_id);
                  ds_3gpp_tft_set_filter_preced_bit(preced,
                                              nw_bk->preced_conflict);
                  nw_bk->preced_conflict_cnt++;
                  rep_fltr_found = TRUE;
                }
                else
                {
                /*-----------------------------------------------------------------
                  Replacing a filter with same fid, precedence and direction
                  don't set precedence conflict
                 -----------------------------------------------------------------*/                 
                 DS_3GPP_MSG3_HIGH("Replacing a filter with same fid,precedence and tx direction",0,0,0);
                  replacing_same_filter = TRUE;
                }
              }/* filter_id == rep_filter_id*/
           }
          /*In uplink filter check existing downlink filters*/
            for(rep_cnt=0;rep_cnt<nw_bk->curr_qos.rx.fltr_template.num_filters;
                  rep_cnt++)
            {
              fltr = &(nw_bk->curr_qos.rx.fltr_template.list_ptr[rep_cnt]);        
            /*---------------------------------------------------------------
                extract the filter id  and precedence.
            --------------------------------------------------------------*/
              rep_preced = fltr->ipfltr_aux_info.fi_precedence;
              rep_filter_id = fltr->ipfltr_aux_info.fi_id & (0xF);
              if(filter_id == rep_filter_id)
              {
                 if(preced != rep_preced)
                 {
                 /*only if repeated filter not found*/
                   if (!rep_fltr_found)
                   {
                   /*--------------------------------------------------------
                    Update only if the ID matches but preced is different. 
                   ----------------------------------------------------------*/
                     DS_3GPP_MSG3_HIGH("Precedence conflict on RX for 0x%x, prec cnt: %d filter_id:%d",
                                       preced, nw_bk->preced_conflict_cnt, filter_id);
                     ds_3gpp_tft_set_filter_preced_bit(preced,
                                                 nw_bk->preced_conflict);
                     nw_bk->preced_conflict_cnt++;
                     rep_fltr_found = TRUE;
                   }
                }
                else
                {
                 /*-----------------------------------------------------------------
                Replacing a filter with same fid, precedence and different direction
                don't set precedence conflict
                  -----------------------------------------------------------------*/
                DS_3GPP_MSG3_HIGH("Replacing a filter with same fid,precedence with RX"
                                   "direction",0,0,0);
		  replacing_same_filter = TRUE;
                }
              }/* filter_id == rep_filter_id*/
          }
				  
            if((rep_fltr_found == TRUE)&& 
               ((nw_tft->operation_code == DS_3GPP_TFT_OPCODE_REPLACE)||
                (nw_tft->operation_code == DS_3GPP_TFT_OPCODE_ADD)) )
            {
              ds_3gpp_tft_clear_prec_conflict_if_interchanged_within_tft(preced, 
                                                  filter_id, nw_tft, nw_bk );
            }  
          
	  if((rep_fltr_found == FALSE)&&(replacing_same_filter == FALSE)&&
             (ds_3gpp_tft_is_filter_preced_bit_set(preced, nw_bk->preced_conflict)== FALSE ))
            {
           /*-----------------------------------------------------------------
            precedence conflict on other bearers
            -----------------------------------------------------------------*/		 
            DS_3GPP_MSG3_HIGH("Precedence conflict on other bearers for	0x%x prec cnt:%d,"
                       "filter_id:%d ", preced, nw_bk->preced_conflict_cnt,filter_id);
              ds_3gpp_tft_set_filter_preced_bit(preced,
                                              nw_bk->preced_conflict);
              nw_bk->preced_conflict_cnt++;
          }/* rep_fltr_found == TRUE */
        }/* replace */
      }/* if preced conflict */
    } /* del_filter == FALSE*/
    /*---------------------------------------------------------------------
      Verify if the filter ID is repeated in the request.
    ---------------------------------------------------------------------*/
    if(ds_3gpp_tft_is_filter_identifier_requested(
                                     filter_id, &temp_filter_id) == FALSE)
    {
      ds_3gpp_tft_set_filter_identifier_bit(filter_id,&temp_filter_id);
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Invalid NW input.FI-ID repeated");
      return FALSE;
    }
    /*---------------------------------------------------------------------
      Track filter id conflict per bearer and flow.
      Increment bearer only if conflict is not on the flow.
    ---------------------------------------------------------------------*/
    if(ds_3gpp_tft_is_filter_identifier_requested(
                      filter_id,&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                                  ->fi_identifier))==TRUE)
    {
      if(ds_3gpp_tft_is_filter_identifier_requested(
         filter_id, &(flow_context_p->ds_flow_context_dyn_p->
                      fi_identifier)) == TRUE)
      {
        ds_3gpp_tft_set_filter_identifier_bit(filter_id,
                                             &(nw_bk->flow_cntx_fltr_id));
      }
      else
      {
        ds_3gpp_tft_set_filter_identifier_bit(filter_id,
                                             &(nw_bk->bearer_fltr_id));
      }
    }/* filter id part of the bearer */
    else
    {
      DS_3GPP_MSG1_HIGH("Filter id:%d is not present in existing FLow",
                         filter_id);
      ds_3gpp_tft_set_filter_identifier_bit(filter_id,
                                            &(nw_bk->non_match_fltr_id));
    }  
  }/* for */
  return TRUE;
} /* ds_3gpp_tft_check_nw_fltr_id_preced */

/*===========================================================================
FUNCTION DS_3GPP_TFT_FIND_BEARER_WITH_FLTR_PRECED_CONFLICT

DESCRIPTION
  Find the bearers on which the precedence conflict occurs.
  This checks if the filter ID or precedence is repeated.
  Filter ID conflict is detected. Precedence conflict is detected.
  Returns the filters with conflict, without conflict.
  Returns the precedence values with conflict.
  This function is called for NW initiated Qos Modification check.

PARAMETERS
  pdn_context_p               -  PDN information.
  bearer_context_p            -  Bearer information.
  flow_context_p              -  Flow context.
  nw_bk                       -  Boook keeping info.
                      
DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_find_bearer_with_preced_conflict
(
  ds_pdn_context_s                *pdn_context_p,
  ds_bearer_context_s             *bearer_context_p,
  ds_flow_context_s               *flow_context_p,
  ds_3gpp_nw_modify_qos_bk_s      *nw_bk
)
{
  //uint16                    filter_id = 0;
  ds_bearer_flow_manager_s  *bflow_manager_p = NULL;
  ds_flow_manager_s         *flow_manager_p = NULL;
  ds_flow_context_s         *flow_cntx = NULL;
  uint16                    preced = 0;
  uint32                    temp_preced[DS_FI_PRECED_ARR_SIZE];
  int                       cnt = 0;
  ip_filter_type            *fltr = NULL;
  int                       tpreced_conflict_cnt = 0;
  int                       loop = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(pdn_context_p==NULL||bearer_context_p==NULL||nw_bk==NULL)
  {
    DS_3GPP_MSG0_ERROR("Input params are NULL in check");
    return FALSE;
  }

  bflow_manager_p=ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(bflow_manager_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Bearer does not have flow manager");
    return FALSE;
  }
  /*-----------------------------------------------------------------------
    Retrieve the flow manager.
  -----------------------------------------------------------------------*/
  flow_manager_p = ds_flow_manager_find_flow_manager(pdn_context_p);
  if ((flow_manager_p == NULL) || (!ds_flow_manager_validate_flow_manager
                                   (flow_manager_p)))
  {
    return FALSE;
  }

  /*-----------------------------------------------------------------------
    Copy the precedence conflict count and precedence
  -----------------------------------------------------------------------*/
  tpreced_conflict_cnt = nw_bk->preced_conflict_cnt;
  memscpy(temp_preced,(sizeof(uint32)*DS_FI_PRECED_ARR_SIZE),
  	  (nw_bk->preced_conflict), (sizeof(uint32)*DS_FI_PRECED_ARR_SIZE));

  /*---------------------------------------------------------------------
    Loop thru all the bearer flow managers on PDN. 
    Also need to search the pending flow contexts!
  -----------------------------------------------------------------------*/
  bflow_manager_p = list_peek_front(&(flow_manager_p->
                                   ds_flow_manager_dyn_p->bearer_flow_mgr));
  while((bflow_manager_p != NULL) && 
    (ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p)))
  {
    /*-------------------------------------------------------------------
       Loop thru all the flow contexts on the bearer.
    --------------------------------------------------------------------*/
    flow_cntx = list_peek_front(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                                  ->flow_context));
    while(flow_cntx != NULL)
    {  
      /*-----------------------------------------------------------------------
        Configure the qos flow spec.
      -----------------------------------------------------------------------*/
      if(ds_3gpp_tft_configure_nw_bk_flow_spec(nw_bk)==FALSE)
      {
        DS_3GPP_MSG0_ERROR("Error configuring network bookkeeping info");
        return FALSE;
      }
      /*-------------------------------------------------------------------
        Retrieve the Filters
      --------------------------------------------------------------------*/
      if(ds_flow_context_ps_flow_get_filters(
                            flow_cntx, FALSE,&(nw_bk->curr_qos)) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("psFlowGetFilters failed");
        return FALSE;
      }
      /*-------------------------------------------------------------------
        Check every filter precedence.
      --------------------------------------------------------------------*/
      for(cnt =0;cnt<nw_bk->curr_qos.rx.fltr_template.num_filters; cnt++)
      {
        fltr = &(nw_bk->curr_qos.rx.fltr_template.list_ptr[cnt]);        
        preced = fltr->ipfltr_aux_info.fi_precedence;
        if(ds_3gpp_tft_is_filter_preced_bit_set(
                             preced,temp_preced) == TRUE)
        {
          /*-----------------------------------------------------------------
            Bearer with precedence conflict found
          -----------------------------------------------------------------*/
          DS_3GPP_MSG2_HIGH("conflict preced:0x%x on bearer  0x;%x",
                                      preced, bflow_manager_p->
                             ds_bearer_flow_manager_dyn_p->bearer_ptr);

          /*-----------------------------------------------------------------
            Check if the bearer is already in conflict bearer list. In that
            case do not add again.
          -----------------------------------------------------------------*/      
          loop = nw_bk->conflict_bearer_cnt;
          while(loop > 0)
          {
            if(nw_bk->conflict_bearer[loop-1] == 
                                (void *)(bflow_manager_p->
                                         ds_bearer_flow_manager_dyn_p->bearer_ptr))
            {
              break;
            }
            loop--;
          }/* while */
          if(loop == 0)
          {
          /*-----------------------------------------------------------------
            Add the bearer in conflict bearer list. 
          -----------------------------------------------------------------*/ 
          nw_bk->conflict_bearer[nw_bk->conflict_bearer_cnt] 
                 = (void *)bflow_manager_p->ds_bearer_flow_manager_dyn_p->bearer_ptr;
            nw_bk->conflict_bearer_cnt++;
          }
          else
          {
            DS_3GPP_MSG1_HIGH("bearer already 0x;%x on conflict list",
                               bflow_manager_p->
                               ds_bearer_flow_manager_dyn_p->bearer_ptr);
          }
          tpreced_conflict_cnt--;
          ds_3gpp_tft_clear_filter_preced_bit(preced, temp_preced);
          if(tpreced_conflict_cnt == 0)
          {
            return TRUE;
        }
        }/* conflict preced found in bearer */
      }/* for RX*/ 

      for(cnt =0;cnt<nw_bk->curr_qos.tx.fltr_template.num_filters; cnt++)
      {
        fltr = &(nw_bk->curr_qos.tx.fltr_template.list_ptr[cnt]);        
        /*-----------------------------------------------------------------
          NW Assigned filter id is in the last 4 bits.
        -----------------------------------------------------------------*/
        preced = fltr->ipfltr_aux_info.fi_precedence;
        if(ds_3gpp_tft_is_filter_preced_bit_set(
                             preced,temp_preced) == TRUE)
        {
          /*-----------------------------------------------------------------
            Bearer with precedence conflict found
          -----------------------------------------------------------------*/
          DS_3GPP_MSG2_HIGH("conflict preced 0x:%x found on bearer 0x;%x",
                             preced,bflow_manager_p->
                             ds_bearer_flow_manager_dyn_p->bearer_ptr);
          /*-----------------------------------------------------------------
            Check if the bearer is already in conflict bearer list. In that
            case do not add again.
          -----------------------------------------------------------------*/      
          loop = nw_bk->conflict_bearer_cnt;
          while(loop > 0)
          {
            if(nw_bk->conflict_bearer[loop-1] == 
                    (void *)(bflow_manager_p->ds_bearer_flow_manager_dyn_p->bearer_ptr))
            {
              break;
            }
            loop--;
          }/* while */
          if(loop == 0)
          {
          /*-----------------------------------------------------------------
            Add the bearer in conflict bearer list. 
          -----------------------------------------------------------------*/ 
            nw_bk->conflict_bearer[nw_bk->conflict_bearer_cnt] 
                = (void *)bflow_manager_p->ds_bearer_flow_manager_dyn_p->bearer_ptr;
            nw_bk->conflict_bearer_cnt++;
          }
          else
          {
            DS_3GPP_MSG1_HIGH("bearer 0x;%x on conflict list",
                               bflow_manager_p->
                               ds_bearer_flow_manager_dyn_p->bearer_ptr);
          }
          tpreced_conflict_cnt--;
          ds_3gpp_tft_clear_filter_preced_bit(preced, temp_preced);
          if(tpreced_conflict_cnt == 0)
          {
            return TRUE;
          }
        }
      }/* for TX*/ 
      flow_cntx = list_peek_next(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                                   ->flow_context),
                                 &(flow_cntx->link));
    }/* flow_cntx != NULL */
    bflow_manager_p = list_peek_next(&(flow_manager_p->ds_flow_manager_dyn_p->
                      bearer_flow_mgr),&(bflow_manager_p->link));
  }/* bflow_manager_p != NULL*/

  DS_3GPP_MSG1_ERROR("Did not find all the bearers for preced cnt:%d",
                      nw_bk->preced_conflict_cnt);
  return  FALSE;
}/* ds_3gpp_tft_find_bearer_with_preced_conflict */

/*===========================================================================
FUNCTION DS_3GPP_TFT_PRECED_CONFLICT_ON_BEARER

DESCRIPTION
  Verifies if the input bearer is in the conflict bearer array.

PARAMETERS
  nw_bk                       -  Boook keeping info.
  input                       -  bearer_ptr
                      
DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_preced_conflict_on_bearer
(
  ds_3gpp_nw_modify_qos_bk_s      *nw_bk,
  void                            *input
)
{
  uint32 cnt = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(nw_bk ==NULL || input ==NULL)
  {
    DS_3GPP_MSG2_ERROR("Invalid Input. nw_bk:0x%x, input:0x%x",nw_bk,input);
    return FALSE;
  }
  
  for(cnt=0; cnt<nw_bk->conflict_bearer_cnt; cnt++)
  {
    if(nw_bk->conflict_bearer[cnt] == input)
    {
      DS_3GPP_MSG1_HIGH("Conflict array index:%d has input",cnt);
      return TRUE;
    }
  }
  return FALSE;
}/* ds_3gpp_tft_preced_conflict_on_bearer */

/*===========================================================================
FUNCTION DS_3GPP_TFT_VERIFY_FILTER_CONTENT

DESCRIPTION
  Verifies if both the filters have the same components and contents.
  UE_TFT will not have filter component violations.

PARAMETERS
  ue_tft                      -  UE requested TFT.
  nw_tft                      -  NW requested TFT.
  tft_len                   -  Len of the TFT.
                      
DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_3gpp_tft_verify_filter_content
(
  packet_filter_type_T    *ue_filter,
  packet_filter_type_T    *nw_filter,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{
  ip_filter_type    ue_ip_fltr, nw_ip_fltr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (ue_filter==NULL) || (nw_filter ==NULL) || (cause_code_p ==NULL) )
  {
    if(cause_code_p != NULL)
    {
       *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    }
    DS_3GPP_MSG0_ERROR("NULL Input in tft_verify_filter_content or cause code");
    return FALSE;
  }

  /*------------------------------------------------------------------------
    Clear the memory.
  -------------------------------------------------------------------------*/                     
  memset(&ue_ip_fltr,0,sizeof(ip_filter_type));   
  memset(&nw_ip_fltr,0,sizeof(ip_filter_type));

  /*------------------------------------------------------------------------
    Convert NW filter format to UE PS filter format for UE filter.
    Hard code the last 4 parameters, there value is not required.
  -------------------------------------------------------------------------*/  
  if(ds_flow_manager_convert_nw_filter_to_app_filter(ue_filter,
                                                  &ue_ip_fltr,
                                                  1,
                                                  DS_3GPP_TFT_DOWNLINK,
                                                  IP_V4,
                                                  NULL,
                                                  cause_code_p) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Error converting UE NW filter to UE App filter");
    return FALSE;
  }

  /*------------------------------------------------------------------------
    Convert NW filter format to UE PS filter format for NW filter. 
    Hard code the last 4 parameters, there value is not required.
  -------------------------------------------------------------------------*/  
  if(ds_flow_manager_convert_nw_filter_to_app_filter(nw_filter,
                                                  &nw_ip_fltr,
                                                  1,
                                                  DS_3GPP_TFT_DOWNLINK,
                                                  IP_V4,
                                                  NULL,
                                                  cause_code_p) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Error converting UE NW filter to UE App filter");
    return FALSE;
  }

  if (memcmp(&ue_ip_fltr, &nw_ip_fltr, sizeof(ip_filter_type)) != 0)
  {
     DS_3GPP_MSG0_MED("NW Filter does not match the UE Filter");
     return FALSE;
  } 
  DS_3GPP_MSG0_MED("NW Filter matches the UE Filter"); 
  return TRUE;
}/* ds_3gpp_tft_verify_filter_content */
/*===========================================================================
FUNCTION DS_3GPP_TFT_VERIFY_NW_TFT_ADD_REPLACE

DESCRIPTION
  Verifies if the NW response addresses the UE request.

PARAMETERS
  ue_tft                      -  UE requested TFT.
  nw_tft                      -  NW responsed TFT.
  is_replace                  -  boolean, indicating if replace.
  compare_identifier          -  boolean,compare the identifier.
                      
DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_3gpp_tft_verify_nw_tft_add_replace
(
  tft_type_T                    *ue_tft,
  tft_type_T                    *nw_tft,
  boolean                        is_replace,
  boolean                        compare_identifier  
)
{
  int nw_cnt = 0, ue_cnt=0;
  packet_filter_type_T    *ue_filter=NULL,  *nw_filter=NULL;
  boolean match_found = FALSE;
  ds_3gpp_internal_cause_code_enum_type icause_code;
  icause_code=DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ue_tft==NULL||nw_tft==NULL)
  {
    DS_3GPP_MSG0_ERROR("Input params are NULL in verify_nw_tft_add_replace");
    return FALSE;
  }
  /*-------------------------------------------------------------------
     Check if UE request is more than NW response
  --------------------------------------------------------------------*/
  if(ue_tft->num_filters > nw_tft->num_filters)
  {
    DS_3GPP_MSG2_ERROR("Ue req filters %d. Nw ret filters %d",
                        ue_tft->num_filters, nw_tft->num_filters);
    return FALSE;
  }

  /*-------------------------------------------------------------------
     Check if UE request is same as NW response
  --------------------------------------------------------------------*/
  if(is_replace)
  {
    if(ue_tft->num_filters != nw_tft->num_filters)
    {
      DS_3GPP_MSG2_ERROR("Ue req filters %d. Nw ret filters %d",
                          ue_tft->num_filters, nw_tft->num_filters);
      return FALSE;
    }
  }
  /*-------------------------------------------------------------------
     Check if UE request is same as NW response
  --------------------------------------------------------------------*/
  for(ue_cnt=0; ue_cnt<ue_tft->num_filters; ue_cnt++)
  {
    ue_filter = &(ue_tft->filter_list[ue_cnt]);
    match_found = FALSE;
    for(nw_cnt=0; nw_cnt<nw_tft->num_filters; nw_cnt++)
    {
      nw_filter = &(nw_tft->filter_list[nw_cnt]);
      if((ue_filter->eval_precedence==nw_filter->eval_precedence)&&
          (ue_filter->content_length == nw_filter->content_length)&&
         (ds_3gpp_tft_verify_filter_content(ue_filter,
                                            nw_filter,
                                            &icause_code)== TRUE))
      {
        if((is_replace) && (compare_identifier))
        {
          /* compare only when UE request and NW response are
             both replace.
          */  
          if(ue_filter->identifier == nw_filter->identifier)
          {
            match_found = TRUE;
            break;
          }
        }
        else
        {
          match_found = TRUE;
          break;   
        }/* is_replace*/
      }/* each filter */
    }/* for nw_cnt*/
    if(!match_found)
    {
      DS_3GPP_MSG1_ERROR("UE Request not supported for index:%d",ue_cnt);
      return FALSE;
    }
  }/* for ue_cnt */
  return TRUE;
}/* ds_3gpp_tft_verify_nw_tft_add_replace */


/*===========================================================================
FUNCTION DS_3GPP_TFT_VERIFY_NW_TFT_DEL

DESCRIPTION
  Verifies if the NW response addresses the UE request.

PARAMETERS
  ue_tft                      -  UE requested TFT.
  nw_tft                      -  NW responsed TFT.
                      
DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_3gpp_tft_verify_nw_tft_del
(
  tft_type_T                    *ue_tft,
  tft_type_T                    *nw_tft
)
{
  int nw_cnt = 0, ue_cnt=0;
  packet_filter_type_T    *ue_filter=NULL,  *nw_filter=NULL;
  boolean match_found = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ue_tft==NULL||nw_tft==NULL)
  {
    DS_3GPP_MSG0_ERROR("Input params are NULL in verify_nw_tft_del");
    return FALSE;
  }
  if(ue_tft->num_filters > nw_tft->num_filters)
  {
    DS_3GPP_MSG2_ERROR("Ue req filters %d. Nw ret filters %d",
                        ue_tft->num_filters, nw_tft->num_filters);
    return FALSE;
  }
  for(ue_cnt=0; ue_cnt<ue_tft->num_filters; ue_cnt++)
  {
    ue_filter = &(ue_tft->filter_list[ue_cnt]);
    match_found = FALSE;
    for(nw_cnt=0; nw_cnt<nw_tft->num_filters; nw_cnt++)
    {
      nw_filter = &(nw_tft->filter_list[nw_cnt]);
      if(ue_filter->identifier == nw_filter->identifier)
      {
        match_found = TRUE;
        break;
      } /* if check */            
    }/* for nw_cnt*/
    if(!match_found)
    {
      DS_3GPP_MSG1_ERROR("UE Request not supported for index:%d",ue_cnt);
      return FALSE;
    }
  }/* for ue_cnt */
  return TRUE;
}/* ds_3gpp_tft_verify_nw_tft_del */

/*===========================================================================
FUNCTION DS_3GPP_TFT_VERIFY_NW_TFT

DESCRIPTION
  Verifies if the NW response addresses the UE request.

PARAMETERS
  ue_tft                      -  UE requested TFT.
  nw_tft                      -  NW responsed TFT.
  flow_context_p              -  Flow context on which to perform operation.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_verify_nw_tft
(
  tft_type_T                    *ue_tft,
  tft_type_T                    *nw_tft,
  ds_flow_context_s             *flow_context_p
)
{
  ds_tft_opcode_enum_type ue_opcode;
  ds_tft_opcode_enum_type nw_opcode;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ue_tft==NULL||nw_tft==NULL||flow_context_p==NULL)
  {
    DS_3GPP_MSG0_ERROR("Input params are NULL in verify_nw_tft");
    return FALSE;
  }
  ue_opcode = (ds_tft_opcode_enum_type)ue_tft->operation_code;
  nw_opcode = (ds_tft_opcode_enum_type)nw_tft->operation_code;

  switch(ue_opcode)
  {
    case DS_3GPP_TFT_OPCODE_CREATE:
      if ( (nw_opcode != DS_3GPP_TFT_OPCODE_ADD) && 
           (nw_opcode != DS_3GPP_TFT_OPCODE_REPLACE) &&
           (nw_opcode != DS_3GPP_TFT_OPCODE_CREATE))
      {
        DS_3GPP_MSG1_ERROR("Invalid network tft opcode: 0x%x",
                            nw_tft->operation_code);
        return FALSE;
      }

      if(nw_opcode == DS_3GPP_TFT_OPCODE_ADD)
      {
        if(ds_3gpp_tft_verify_nw_tft_add_replace(
                                      ue_tft,nw_tft,FALSE,FALSE)== FALSE)
        {
          DS_3GPP_MSG0_ERROR("Ue create request, nw add response failed");
          return FALSE;
        }
        return TRUE;
      }/* nw_opcode == DS_3GPP_TFT_OPCODE_ADD */

      if(nw_opcode == DS_3GPP_TFT_OPCODE_CREATE)
      {
        if(ds_3gpp_tft_verify_nw_tft_add_replace(
                                      ue_tft,nw_tft,FALSE,FALSE)== FALSE)
        {
          DS_3GPP_MSG0_ERROR("Ue create request, nw create response failed");
          return FALSE;
        }
        return TRUE;
      }/* nw_opcode == DS_3GPP_TFT_OPCODE_CREATE */

      if(nw_opcode == DS_3GPP_TFT_OPCODE_REPLACE)
      {
        if(ds_3gpp_tft_verify_nw_tft_add_replace(
                                      ue_tft,nw_tft,TRUE,FALSE)== FALSE)
        {
          DS_3GPP_MSG0_ERROR("Ue create request,nw replace response failed");
          return FALSE;
        }
        return TRUE;
      }/* nw_opcode == DS_3GPP_TFT_OPCODE_REPLACE */
    break;
    case DS_3GPP_TFT_OPCODE_REPLACE:
      if (nw_opcode != DS_3GPP_TFT_OPCODE_REPLACE) 
      {
        DS_3GPP_MSG1_ERROR("Invalid network tft opcode: 0x%x",
                            nw_tft->operation_code);
        return FALSE;
      }
      if(ds_3gpp_tft_verify_nw_tft_add_replace(
                                      ue_tft,nw_tft,TRUE,TRUE)== FALSE)
      {
        DS_3GPP_MSG0_ERROR("Ue replace request,nw replace response failed");
        return FALSE;
      }
      return TRUE;
    //break;
    case DS_3GPP_TFT_OPCODE_DEL_FILTER:
      if((nw_opcode != DS_3GPP_TFT_OPCODE_REPLACE) &&
         (nw_opcode != DS_3GPP_TFT_OPCODE_DEL_FILTER) &&
         (nw_opcode != DS_3GPP_TFT_OPCODE_DEL_TFT))
      {
        DS_3GPP_MSG1_ERROR("Invalid network tft opcode: 0x%d",
                            nw_tft->operation_code);
        return FALSE;
      }
      if(nw_opcode != DS_3GPP_TFT_OPCODE_DEL_TFT)
      {
        if(ds_3gpp_tft_verify_nw_tft_del(ue_tft,nw_tft)==FALSE)
        {
          DS_3GPP_MSG0_ERROR("Ue del request,nw del/replace response failed");
          return FALSE;
        }
        return TRUE;
      }
      DS_3GPP_MSG0_ERROR("DEL_TFT is not valid operation");
      return FALSE;
    //break;

    case DS_3GPP_TFT_OPCODE_NO_OP:
      if(nw_opcode != DS_3GPP_TFT_OPCODE_NO_OP) 
      {
        DS_3GPP_MSG1_ERROR("Invalid network tft opcode: 0x%x",
                            nw_tft->operation_code);
        return FALSE;
      }
      DS_3GPP_MSG0_ERROR("TFT_NO_OP is not supported operation");
      return FALSE;
    //break;
    default:
     DS_3GPP_MSG2_ERROR("Invalid ue opcode:0x%x nw opcode:0x%x",
                         ue_tft->operation_code,nw_tft->operation_code);
     return FALSE;
  }/* switch*/
  return FALSE;
}/* ds_3gpp_tft_verify_nw_tft*/

/*===========================================================================
FUNCTION DS_3GPP_TFT_UPDATE_NW_DELETE_TFT

DESCRIPTION
  Update the NW TFT request to delete filters from a given flow.

PARAMETERS
  nw_tft                      -  NW responsed TFT.
  flow_context_p              -  Flow context on which to perform operation.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_tft_update_nw_delete_tft
(
  tft_type_T                    *nw_tft,
  ds_flow_context_s             *flow_context_p
) 
{
  ip_filter_type            *curr_filter = NULL;
  ip_filter_spec_type       *curr_filter_spec = NULL;
  uint16                     identifier =0;
  int                        loop = 0;
  qos_spec_type              current_qos;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((nw_tft == NULL) || (flow_context_p == NULL))
  {
    DS_3GPP_MSG0_ERROR("Input param are null");
    return FALSE;
  }
  memset(&current_qos, 0, sizeof(qos_spec_type));

  current_qos.rx.fltr_template.list_ptr = 
       (ip_filter_type *)modem_mem_calloc(MAX_PACKET_FILTERS,
                                          sizeof(ip_filter_type),
                                          MODEM_MEM_CLIENT_DATA);
  if(current_qos.rx.fltr_template.list_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Heap Memory Allocation failed");
    return FALSE;
  }

  current_qos.tx.fltr_template.list_ptr = 
       (ip_filter_type *)modem_mem_calloc(MAX_PACKET_FILTERS,
                                          sizeof(ip_filter_type),
                                          MODEM_MEM_CLIENT_DATA);
  if(current_qos.tx.fltr_template.list_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Heap Memory Allocation failed");
    modem_mem_free((void *)current_qos.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }

  if(ds_flow_context_ps_flow_get_filters(
               flow_context_p,FALSE,&current_qos) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Error retrieving the filters from flow");
    modem_mem_free((void *)current_qos.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    modem_mem_free((void *)current_qos.tx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }

  /* RX */
  curr_filter_spec =  &(current_qos.rx.fltr_template);
  for(loop =0; loop<curr_filter_spec->num_filters; loop++)
  {
    curr_filter = &(curr_filter_spec->list_ptr[loop]);
    if (!curr_filter)
    {
      DS_3GPP_MSG0_ERROR("Current filter(Rx) is NULL");
       continue;
    }
    identifier = (curr_filter->ipfltr_aux_info.fi_id & (0xF));
    if(nw_tft->num_filters < MAX_PACKET_FILTERS)
    {
      /*---------------------------------------------------------------
        Translate filter to NW format.
      ---------------------------------------------------------------*/
      nw_tft->filter_list[nw_tft->num_filters].identifier =(byte)identifier;
      nw_tft->num_filters++;       
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Filters exceeed max supported per bearer");
      modem_mem_free((void *)current_qos.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
      modem_mem_free((void *)current_qos.tx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
      return FALSE;
    }
  }

  /* TX */
  curr_filter_spec =  &(current_qos.tx.fltr_template);
  for(loop =0; loop<curr_filter_spec->num_filters; loop++)
  {
    curr_filter = &(curr_filter_spec->list_ptr[loop]);
    if (!curr_filter)
    {
      DS_3GPP_MSG0_ERROR("Current filter(Tx) is NULL");
       continue;
    }
    identifier = (curr_filter->ipfltr_aux_info.fi_id & (0xF));
    if(nw_tft->num_filters < MAX_PACKET_FILTERS)
    {
      /*---------------------------------------------------------------
        Translate filter to NW format.
      ---------------------------------------------------------------*/
      nw_tft->filter_list[nw_tft->num_filters].identifier =(byte)identifier;
      nw_tft->num_filters++;       
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Filters exceeed max supported per bearer");
      modem_mem_free((void *)current_qos.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
      modem_mem_free((void *)current_qos.tx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
      return FALSE;
    }
  }

  modem_mem_free((void *)current_qos.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
  modem_mem_free((void *)current_qos.tx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
  return TRUE;
}/* ds_3gpp_tft_verify_nw_delete_tft */

#endif /* FEATURE_DATA_3GPP */
