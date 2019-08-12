/*!
  @file
  ds_3gpp_bearer_flow_manager.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_bearer_flow_manager.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/26/12   ss      Fixed the issue of signaling indication bit not propagated
                   to app.
10/19/11   nd      Corrected Data rate conversion, when UE decodes pdp
                   activation msg.
10/05/11   ttv     Added DSDS changes for new mode handler.
07/27/11   ttv     Added changes required for source statistics descriptor
                   addition.
04/03/10   vk      Added support for MSG2.0 based logging
02/04/10   vd      Changed function naming format.
01/23/10   vk      Added helper macros for virtual function dispatch
01/20/10   vd      Changes to support UE Qos modification.
11/17/09   vd      Initial version.
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
#include "dstask.h"
#include "ds3gmgr.h"
#include "ds_3gpp_flow_manager.h"
#include "ds_3gpp_flow_context.h"
#ifdef FEATURE_DATA_LTE
#include "ds_eps_flow_manager.h"
#include "ds_eps_bearer_context.h"
#include "ds_eps_bearer_flow_manager.h"
#endif
#include "ds_3gpp_bearer_flow_manager.h"
#include "ds_3gpp_tft.h"
#include "ps_qos_defs.h"
#include "ps_ipfltr_defs.h"
#include "ps_in.h"
#include "ps_iface_defs.h"
#include "dsumtspdpreg.h"
#include "list.h"
#include "modem_mem.h"
#ifdef DS_LTE_DISABLE
#ifdef FEATURE_DATA_WCDMA_PS
#include "ds_wcdma_bearer_context.h"
#endif
#endif /* DS_LTE_DISABLE */
#include "ds_flow_control.h"
#include "ran.h"

#include "ds_umts_bearer_flow_manager.h"
#include "ds_eps_bearer_flow_manager.h"
#include "ds_3gpp_dyn_mem_hdlr.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define DS_BEARER_FLOW_MGR_VFR_CALL(rval, func, subs_id, ...)  \
            DS_3GPPI_VFR_CALL(ds_bearer_flow_manager_ftbl, rval, func, subs_id,\
                              __VA_ARGS__)

#define DS_BEARER_FLOW_MGR_VF_CALL(func,subs_id, ...)  \
            DS_3GPPI_VF_CALL(ds_bearer_flow_manager_ftbl, func, subs_id,\
                              __VA_ARGS__)

#define DS_BEARERE_FLOW_MGR_IS_DEFAULT_BEARER(bearer_idx)  \
            ds_bearer_flow_manager_tbl[bearer_idx].ds_bearer_flow_manager_dyn_p->bearer_ptr->ds_bearer_context_dyn_p->is_default

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*----------------------------------------------------------------------------
  Bearer Flow Manager table. This table contains all the bearer flow managers.
  This table is updated whenever a new bearer flow manager is created or deleted.

  Each flow manager context is tied to a bearer context
----------------------------------------------------------------------------*/
ds_bearer_flow_manager_s ds_bearer_flow_manager_tbl[DS_3GPP_MAX_BEARER_CONTEXT] = {{{NULL}}};


extern ds_pdn_context_s ds_pdn_context_tbl[DS_3GPP_MAX_PDN_CONTEXT];

/*----------------------------------------------------------------------------
  EPS/umts specific function table We allocate memory for SYS_SYS_MODE_MAX
  table. However, we will be using only those corresponding to WCDMA, GPRS
  and EPS.
----------------------------------------------------------------------------*/
ds_3gpp_bearer_flow_manager_ftbl_s ds_bearer_flow_manager_ftbl[DS_3GPPI_SYS_MODE_MAX] = {{NULL}};



/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
static boolean ds_bearer_flow_manager_alloc_dyn_mem
(
  int alloc_index
);

/*===========================================================================

                             MACROS

===========================================================================*/

/*===========================================================================
MACRO ARE_PORT_PARAM_VALID()

DESCRIPTION
  Validates if given port number and range params are valid

PARAMETERS
  port  : starting port number of the range
  range : ending port number of the range

RETURN VALUE
   TRUE  : if port > range 
   FALSE : otherwise
===========================================================================*/
#define ARE_PORT_PARAM_VALID(port, range)                                 \
  ((port > range)? FALSE : TRUE)

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_3GPP_INIT

DESCRIPTION
  Initialize the bearer flow manager module.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_flow_manager_3gpp_init()
{
  int index = 0;
  ds_bearer_flow_manager_s *bearer_flow_mgr_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
 /*-------------------------------------------------------------------------
    Initialize the flow manager array to default values.
  -------------------------------------------------------------------------*/
  bearer_flow_mgr_p = ds_bearer_flow_manager_tbl;
  for(index=0; index<DS_3GPP_MAX_BEARER_CONTEXT; index++)
  {
    bearer_flow_mgr_p = &(ds_bearer_flow_manager_tbl[index]);
    memset((void *)bearer_flow_mgr_p,0,(sizeof(ds_bearer_flow_manager_s)));

  }/* For */
  return;
} /* ds_bearer_flow_manager_3gpp_init */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_VERIFY_APP_FILTER_IDENTIFIER

DESCRIPTION
  Verify if app passed invalid filter identifier value.

PARAMETERS
 input_qos           - Pointer to App passed Qos.
 filter_op           - enum indicating what filters to check.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_verify_app_filter_identifier
( 
  qos_spec_type      *input_qos,
  ds_3gpp_app_filter_op filter_op
)
{
  ip_filter_type       *inp_filter  = NULL;  
  int num_filters = 0;
  int cnt = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(input_qos == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input parameter is NULL");
    return FALSE;
  }
  if(filter_op & DS_3GPP_APP_RX_FLTR_OP)
  {
    /*-----------------------------------------------------------------------
      Check for RX filters.
    -----------------------------------------------------------------------*/
    num_filters = input_qos->rx.fltr_template.num_filters;
    for(cnt =0; cnt> num_filters; cnt++)
    {
      inp_filter = &(input_qos->rx.fltr_template.list_ptr[cnt]);
      if(inp_filter->ipfltr_aux_info.fi_id==DS_DEF_FILTER_ID)
      {
        /*-------------------------------------------------------------------
          Set Filter ID to zero
        -------------------------------------------------------------------*/    
        inp_filter->ipfltr_aux_info.fi_id = 0;  
      }
      else
      {
        DS_3GPP_MSG1_ERROR("Invalid filter identifier:%d for RX",cnt);
        return FALSE;
      }
    }
  }/* DS_3GPP_APP_RX_FLTR_OP */

  if(filter_op & DS_3GPP_APP_TX_FLTR_OP)
  {
    /*-----------------------------------------------------------------------
      Check for TX filters.
    -----------------------------------------------------------------------*/ 
    num_filters = input_qos->tx.fltr_template.num_filters;
    for(cnt =0; cnt> num_filters; cnt++)
    {
      inp_filter = &(input_qos->tx.fltr_template.list_ptr[cnt]);
      if(inp_filter->ipfltr_aux_info.fi_id==DS_DEF_FILTER_ID)
      {
        /*-------------------------------------------------------------------
          Set Filter ID to zero
        -------------------------------------------------------------------*/    
        inp_filter->ipfltr_aux_info.fi_id = 0;  
      }
      else
      {
        DS_3GPP_MSG1_ERROR("Invalid filter identifier:%d for TX",cnt);
        return FALSE;
      }
    }
  } /* DS_3GPP_APP_TX_FLTR_OP */
  return TRUE;
} /* ds_bearer_flow_manager_verify_app_filter_identifier */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_ALLOCATE_FLOW_MANAGER

DESCRIPTION
  Allocate a new flow manager for this bearer.

PARAMETERS
 bearer_context_p     - Pointer to bearer context.

DEPENDENCIES
  None.

RETURN VALUE
 ds_bearer_flow_manager_s * - Pointer to empty flow manager.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_bearer_flow_manager_s * ds_bearer_flow_manager_allocate_flow_manager
( 
  ds_bearer_context_s *bearer_context_p
)
{
  int                       alloc_index = 0;
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;
  sys_modem_as_id_e_type    subs_id = SYS_MODEM_AS_ID_NONE;
 #if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
  sys_sys_mode_e_type	umts_network_mode = SYS_SYS_MODE_NONE;
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if defined FEATURE_WCDMA
  umts_network_mode = SYS_SYS_MODE_WCDMA;
#elif defined FEATURE_TDSCDMA
  umts_network_mode =  SYS_SYS_MODE_TDS;
#elif defined FEATURE_GSM_GPRS
  umts_network_mode = SYS_SYS_MODE_GSM;
#endif

  if (!ds_bearer_cntxt_get_subs_id_from_bearer (bearer_context_p, &subs_id))
  {
    return bflow_manager_p;
  }

  for (alloc_index = 0; alloc_index < DS_3GPP_MAX_BEARER_CONTEXT; alloc_index++)
  {
      
    if(!ds_bearer_flow_manager_validate_flow_manager
       (&(ds_bearer_flow_manager_tbl[alloc_index])))
    {
      if(!ds_bearer_flow_manager_alloc_dyn_mem(alloc_index))
      {
        /* dynamic mem allocation failed, return NULL ;*/
        DS_3GPP_MSG0_ERROR("Dynamic memory alloc failed, return NULL");
        return NULL;
      }

      /*------------------------------------------------------------------------ 
        Only if the bearer flow manager is allocated successfully go ahead
        and allocate umts and eps bearer flow managers
        If UMTS bearer flow manager allocation fails free bearer flow mgr
        If EPS bearer flow manager allocation fails free umts and bearer flow mgr
      ------------------------------------------------------------------------*/
      bflow_manager_p = &(ds_bearer_flow_manager_tbl[alloc_index]);
 
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || defined(FEATURE_TDSCDMA)
      if(!ds_3gpp_dyn_mem_hdlr_allocate_umts_bearer_flow_manager
         (alloc_index, subs_id))
      {
        ds_bearer_flow_manager_free_dyn_mem(bflow_manager_p);
        DS_3GPP_MSG0_ERROR("Dynamic memory alloc failed during umts bearer flow manager allocation, return NULL");
        return NULL;
      }
#endif /* (FEATURE_DATA_WCDMA_PS) || (FEATURE_GSM_GPRS) || (FEATURE_TDSCDMA) */

      if(!ds_3gpp_dyn_mem_hdlr_allocate_eps_bearer_flow_manager
           (alloc_index, subs_id))
      {
        ds_3gpp_dyn_mem_hdlr_free_umts_bearer_flow_manager(alloc_index, subs_id);
        ds_bearer_flow_manager_free_dyn_mem(bflow_manager_p);
        DS_3GPP_MSG0_ERROR("Dynamic memory alloc failed during umts bearer flow manager allocation, return NULL");
        return NULL;
      }

    /*-----------------------------------------------------------------------
      Associate the mode specific blob that is going to be populated to the 
      bearer context pointer.
    -------------------------------------------------------------------------*/

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
      DS_BEARER_FLOW_MGR_VOBJ_REG
      (
       (bflow_manager_p),
        umts_network_mode,
       (void *)(&ds_umts_bflow_manager_tbl[alloc_index])
       );
#endif

#ifdef FEATURE_LTE
      DS_BEARER_FLOW_MGR_VOBJ_REG
      (
        (bflow_manager_p),
        SYS_SYS_MODE_LTE,
        (void *)(&ds_eps_bflow_manager_tbl[alloc_index])
      );
#endif

      /*---------------------------------------------------------------------
       Initialize Lists and allocate a bearer context
      -----------------------------------------------------------------------*/
      bflow_manager_p->ds_bearer_flow_manager_dyn_p->bearer_ptr = 
	                                                      bearer_context_p;

      DS_3GPP_MSG1_HIGH("Associated valid bearer flow manager to bearer: 0x%x",
		         bearer_context_p);      
     
      list_init(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->flow_context));

      /*---------------------------------------------------------------------
        Make sure the flow context list is empty
      ---------------------------------------------------------------------*/
      if(list_size(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->flow_context)) 
           > 0)
      {
        DS_3GPP_MSG1_ERROR("New flow manager has %d flow contexts in the list",
         list_size(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->flow_context))); 
        return NULL;
      }

      break;
    }
  }

  if (alloc_index == DS_3GPP_MAX_BEARER_CONTEXT)
  {
    DS_3GPP_MSG0_ERROR("MAX Bearer Flow contexts have been allocated");
  }


  return bflow_manager_p;
} /* ds_bearer_flow_manager_allocate_flow_manager */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_RELEASE_FLOW_MANAGER

DESCRIPTION
  Release the flow manager associated with the bearer.

PARAMETERS
  bearer_context_p  - Pointer to the bearer context.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_flow_manager_release_flow_manager
(
  ds_bearer_context_s *bearer_context_p
)
{
  int free_index = 0;
  ds_bearer_flow_manager_s  *bflow_manager_p = NULL;
  ds_flow_context_s         *flow_context_p = NULL;
  ds_flow_context_s         *next_flow_cntx = NULL;
  sys_modem_as_id_e_type     subs_id = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return;
  }

  for (free_index = 0; free_index < DS_3GPP_MAX_BEARER_CONTEXT; free_index++)
  {
      bflow_manager_p = &(ds_bearer_flow_manager_tbl[free_index]);
    if((ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))&&
       (bflow_manager_p->ds_bearer_flow_manager_dyn_p->bearer_ptr == bearer_context_p))
    {

      ds_3gpp_dyn_mem_hdlr_free_umts_bearer_flow_manager(free_index, subs_id); 
      ds_3gpp_dyn_mem_hdlr_free_eps_bearer_flow_manager(free_index, subs_id);

      /*---------------------------------------------------------------------
        Empty the flow context list
      ---------------------------------------------------------------------*/          
      flow_context_p = list_peek_front(&(bflow_manager_p->
                                         ds_bearer_flow_manager_dyn_p->flow_context));
      while(ds_flow_context_validate_flow(flow_context_p))
      {
        DS_3GPP_MSG0_HIGH("Pending Flow context list is not empty");
        next_flow_cntx = list_peek_next(&(bflow_manager_p->
                                        ds_bearer_flow_manager_dyn_p->flow_context),
                                      &(flow_context_p->link));
        ds_flow_context_release_flow_context(flow_context_p); 
        list_pop_item(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->flow_context),
                                      &(flow_context_p->link));
        flow_context_p = next_flow_cntx;
        /* Need to remove each of these flow contexts */
      } 

      /*---------------------------------------------------------------------- 
        Release the list mutex
      -----------------------------------------------------------------------*/
      list_destroy(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->flow_context));

      /*---------------------------------------------------------------------
        Initialize the parameters.
      ---------------------------------------------------------------------*/
      bflow_manager_p->ds_bearer_flow_manager_dyn_p->pdn_ptr = NULL;
      bflow_manager_p->ds_bearer_flow_manager_dyn_p->fi_identifier = 0;

      ds_bearer_flow_manager_free_dyn_mem(bflow_manager_p);
 
      break;
    }
  } 
}/* ds_bearer_flow_manager_release_flow_manager */


/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_FIND_FLOW_MANAGER

DESCRIPTION
  Find the flow manager for this bearer.

PARAMETERS
 bearer_context_p     - Pointer to bearer context.

DEPENDENCIES
  None.

RETURN VALUE
 ds_bearer_flow_manager_s * - Pointer to empty flow manager.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
ds_bearer_flow_manager_s * ds_bearer_flow_manager_find_flow_manager
(
  ds_bearer_context_s *bearer_context_p
)
{
  int index = 0;
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return bflow_manager_p;
  }

  for (index = 0; index < DS_3GPP_MAX_BEARER_CONTEXT; index++)
  {
    bflow_manager_p =  &(ds_bearer_flow_manager_tbl[index]);
    if(ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p) &&
       (bflow_manager_p->ds_bearer_flow_manager_dyn_p->bearer_ptr ==
         bearer_context_p))
    {
      break;
    }
  }
  if(index == DS_3GPP_MAX_BEARER_CONTEXT)
  {
    DS_3GPP_MSG0_ERROR("Bearer flow manager not found for the bearer context");
    bflow_manager_p = NULL;
  }
  return bflow_manager_p;
} /* ds_bearer_flow_manager_find_flow_manager */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_ADD_FLOW_CONTEXT_TO_FLOW_MANAGER

DESCRIPTION
  Add a flow context to the bearer flow manager list.

PARAMETERS
 flow_context_s       - Pointer to Flow Context infomation.
 bearer_context_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_add_flow_context_to_flow_manager
( 
  ds_bearer_context_s *bearer_context_p,
  ds_flow_context_s   *flow_context_p
)
{
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;
  ds_flow_context_s        *tflow_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }

  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return FALSE;
  }

  bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return FALSE;
  }
  flow_context_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr = bflow_manager_p;
  
  // Check if flow context is already present in the list.
  tflow_context_p = list_peek_front(&(bflow_manager_p->
                                      ds_bearer_flow_manager_dyn_p->flow_context));
  while(ds_flow_context_validate_flow(tflow_context_p))
  {
    if(tflow_context_p == flow_context_p)
    {
      DS_3GPP_MSG0_ERROR("Flow already present in pdn context list");
      return FALSE;
    }
    tflow_context_p = list_peek_next(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                    ->flow_context),&(tflow_context_p->link));
  }
  list_push_front(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->flow_context),
                  &(flow_context_p->link));
  return TRUE;
} /* ds_bearer_flow_manager_add_flow_context_to_flow_manager */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_REMOVE_FLOW_CONTEXT_FROM_FLOW_MANAGER

DESCRIPTION
  Remove a flow context from the bearer flow manager list.

PARAMETERS
 flow_context_s       - Pointer to Flow Context infomation.
 bearer_context_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_remove_flow_context_from_flow_manager
( 
  ds_bearer_context_s *bearer_context_p,
  ds_flow_context_s   *flow_context_p
)
{
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;
  ds_flow_context_s        *tflow_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }

  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return FALSE;
  }
  
  bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return FALSE;
  }  

  // Check if flow context is  present in the list.
  tflow_context_p = list_peek_front(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->
                                      flow_context));
  while(ds_flow_context_validate_flow(tflow_context_p))
  {
    if(tflow_context_p == flow_context_p)
    {
      break;
    }
    tflow_context_p = list_peek_next(&(bflow_manager_p->
                                          ds_bearer_flow_manager_dyn_p->flow_context),
                                     &(tflow_context_p->link));
  }
  if(tflow_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Flow not present in bearer flow manager list");
    return FALSE;
  }
  list_pop_item(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->flow_context),
                &(flow_context_p->link));
  return TRUE;
} /* ds_bearer_flow_manager_remove_flow_context_from_flow_manager */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_DELETE_FLOW_CONTEXT

DESCRIPTION
  Delete a flow context from a bearer context.

PARAMETERS
  pdn_context_p           -  PDN information
  bearer_context_p        -  Bearer information
  flow_context_p          -  Flow context information.
  

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_flow_manager_delete_flow_context
(
  ds_pdn_context_s                *pdn_context_p,
  ds_bearer_context_s             *bearer_context_p,
  ds_flow_context_s               *flow_context_p
)
{
   /*---------------------------------------------------------------------
       Update the Filter precedence bit mask
   ---------------------------------------------------------------------*/
   if (ds_flow_manager_update_precedence_bit_mask(
                 pdn_context_p, flow_context_p, FALSE, FALSE, FALSE) == FALSE )
   {
     DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
   }

   /*---------------------------------------------------------------------
      Update the Filter Identifier bit mask
    ---------------------------------------------------------------------*/
   if (ds_bearer_flow_manager_update_filter_identifier_bit_mask(
                     bearer_context_p, flow_context_p, FALSE) == FALSE )
   {
     DS_3GPP_MSG0_ERROR("Error in Clearing Filter identifier bit mask");
   }
       
   ds_flow_context_release_flow_context(flow_context_p);      

   return;

} /*ds_bearer_flow_manager_delete_all_flow_context */


/*===========================================================================
FUNCTION ds_bearer_flow_manager_delete_all_flow_context

DESCRIPTION
  Delete all the flow contexts associated with the bearer context.

PARAMETERS
  pdn_context_p           -  PDN information
  bearer_context_p        -  Bearer information
  

DEPENDENCIES
  None.

RETURN VALUE
  boolean        - Returns FALSE if invalid params are passed.
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_delete_all_flow_context
(
 ds_pdn_context_s                *pdn_context_p,
 ds_bearer_context_s             *bearer_context_p
)
{
  ds_flow_context_s *flow_context_p = NULL;
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;
  ds_flow_context_s        *next_flow_cntx = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG2_HIGH("Delete All Flow Contexts for PDN 0x%x, bearer 0x%x",
                     pdn_context_p,bearer_context_p);

  if (pdn_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("PDN pointer is NULL");
    return FALSE;
  }
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }

  bflow_manager_p=ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return FALSE;
  }
  /*---------------------------------------------------------------------
    Empty the flow context list
  ---------------------------------------------------------------------*/          
  flow_context_p = list_peek_front(&(bflow_manager_p->
                                     ds_bearer_flow_manager_dyn_p->flow_context));
  {
    while(ds_flow_context_validate_flow(flow_context_p))
    {
      DS_3GPP_MSG0_HIGH("Flow context list is not empty");

      next_flow_cntx = list_peek_next(&(bflow_manager_p->
                 ds_bearer_flow_manager_dyn_p->flow_context)
                 ,&(flow_context_p->link));

      list_pop_item( &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->flow_context),
                     &(flow_context_p->link));   

      ds_bearer_flow_manager_delete_flow_context(pdn_context_p, bearer_context_p,
                                                 flow_context_p);
                          
      flow_context_p = next_flow_cntx;

    }/* while*/  

  }/* else */

  return TRUE;
} /*ds_bearer_flow_manager_delete_all_flow_context */


/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_UPDATE_FILTER_IDENTIFIER_BIT_MASK

DESCRIPTION
  Update filter identifier bit mask for all the filters in the flow context.

PARAMETERS
 bearer_context_p    - Pointer to bearer Context.
 flow_context_p      - Pointer to flow context.
 set_identifier      - If true, set the bit else clear the bit.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_update_filter_identifier_bit_mask
( 
  ds_bearer_context_s  *bearer_context_p,
  ds_flow_context_s    *flow_context_p,
  boolean              set_identifier
)
{
  ds_bearer_flow_manager_s  *bflow_manager_p = NULL;
  qos_spec_type             orig_spec;  
  ip_filter_type            *fltr = NULL; 
  int                       cnt = 0;          
  uint16                    identifier = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return FALSE;
  }

  if(bearer_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input bearer context Pointer is NULL");
    return FALSE;
  }

  bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return FALSE;
  }
  memset(&orig_spec, 0, sizeof(qos_spec_type));  

  /*------------------------------------------------------------------------
     Assign Filter Space
  -------------------------------------------------------------------------*/
  orig_spec.rx.fltr_template.list_ptr = 
       (ip_filter_type *)modem_mem_calloc(MAX_PACKET_FILTERS,
                                          sizeof(ip_filter_type),
                                          MODEM_MEM_CLIENT_DATA);
  if(orig_spec.rx.fltr_template.list_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Heap Memory Allocation failed");
    return FALSE;
  }

  orig_spec.tx.fltr_template.list_ptr = 
       (ip_filter_type *)modem_mem_calloc(MAX_PACKET_FILTERS,
                                          sizeof(ip_filter_type),
                                          MODEM_MEM_CLIENT_DATA);
  if(orig_spec.tx.fltr_template.list_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Heap Memory Allocation failed");
    modem_mem_free((void *)orig_spec.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }
  
  
  /*------------------------------------------------------------------------
    Retrieve the original filters.
  -------------------------------------------------------------------------*/
  if(ds_flow_context_ps_flow_get_filters(
                                    flow_context_p, 
                                    FALSE,
                                    &orig_spec) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Error retrieving original filter params");
    modem_mem_free((void *)orig_spec.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    modem_mem_free((void *)orig_spec.tx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }
  /*------------------------------------------------------------------------
     Loop thru all filters and update the filter identifier bit mask for RX
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt<orig_spec.rx.fltr_template.num_filters; cnt++)
      {
    fltr = &(orig_spec.rx.fltr_template.list_ptr[cnt]);
    identifier =(fltr->ipfltr_aux_info.fi_id &(0xF));
    if(ds_3gpp_tft_is_filter_identifier_requested(
                                      identifier,
                 &(flow_context_p->ds_flow_context_dyn_p->ignore_fi_id))==TRUE)
    {
      continue;
      }
    if(set_identifier)
    {
      /*-------------------------------------------------------------------
        Set the Requested FI Identifer for Rx
      -------------------------------------------------------------------*/
      ds_3gpp_tft_set_filter_identifier_bit(
                                 identifier,
               &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->fi_identifier));     
      }
    else
    {
      /*-------------------------------------------------------------------
        Clear the FI Identifier for Rx
      -------------------------------------------------------------------*/
      ds_3gpp_tft_clear_filter_identifier_bit(
                                 identifier,
               &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->fi_identifier));  
    }
  }/* RX FOR Loop*/
  /*------------------------------------------------------------------------
     Loop thru all filters and update the filter identifier bit mask for TX
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt<orig_spec.tx.fltr_template.num_filters; cnt++)
  {
    fltr = &(orig_spec.tx.fltr_template.list_ptr[cnt]);
    identifier = (fltr->ipfltr_aux_info.fi_id & 0xf);
    if(ds_3gpp_tft_is_filter_identifier_requested(
                                      identifier,
                      &(flow_context_p->ds_flow_context_dyn_p->ignore_fi_id))==TRUE)
    {
      continue;
    }
    if(set_identifier)
    {
      /*-------------------------------------------------------------------
        Set the Requested FI Identifier for Tx
      -------------------------------------------------------------------*/
      ds_3gpp_tft_set_filter_identifier_bit(
                                 identifier,
                     &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->fi_identifier));     
    }
    else
    {
      /*-------------------------------------------------------------------
        Clear the FI Identifier for Tx
      -------------------------------------------------------------------*/
      ds_3gpp_tft_clear_filter_identifier_bit(
                                 identifier,
                    &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->fi_identifier));  
    }
  }/* FOR TX  Filters */

  modem_mem_free((void *)orig_spec.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
  modem_mem_free((void *)orig_spec.tx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);

  return TRUE; 
}/* ds_bearer_flow_manager_update_filter_identifier_bit_mask*/

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_BIND_FLOW

DESCRIPTION
  Bind the flows associated with the bearer.

PARAMETERS
 bearer_context_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_bind_flow
( 
  ds_bearer_context_s *bearer_context_p
)
{
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;
  ds_flow_context_s        *flow_context_p  = NULL;
  ps_extended_info_code_enum_type ps_eic = PS_EIC_NOT_SPECIFIED; 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(bearer_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input Bearer Pointer is NULL");
    return FALSE;
  }

  bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return FALSE;
  } 
  /*---------------------------------------------------------------------
     Bind the flow on every flow context on the list
  ---------------------------------------------------------------------*/          
  flow_context_p = list_peek_front(&(bflow_manager_p->
                                     ds_bearer_flow_manager_dyn_p->flow_context));
  while(ds_flow_context_validate_flow(flow_context_p))
  {
    DS_3GPP_MSG1_HIGH("Binding Flow context 0x:%x",flow_context_p);
    ds_flow_context_ps_flow_bind_phys_link(flow_context_p); 
    ds_flow_context_ps_flow_activate_ind(flow_context_p,ps_eic);
    flow_context_p = list_peek_next(&(bflow_manager_p->
                                  ds_bearer_flow_manager_dyn_p->flow_context),
                                 &(flow_context_p->link));
  } 
  return TRUE;
}/* ds_bearer_flow_manager_bind_flow */



/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_ACTIVATE_FLOW

DESCRIPTION
  Activate the flows associated with the bearer.

PARAMETERS
 bearer_context_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_activate_flow
( 
  ds_bearer_context_s *bearer_context_p
)
{
#if 0
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;
  ds_flow_context_s        *flow_context_p  = NULL;
  ps_extended_info_code_enum_type ps_eic = PS_EIC_NOT_SPECIFIED; 
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(bearer_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input Bearer Pointer is NULL");
    return FALSE;
  }

  DS_3GPP_MSG0_HIGH(" Flows on the bearer are in Active State");
#if 0
  bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(bflow_manager_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("No Bearer Flow manager associated with bearer");
    return FALSE;
  } 
  /*---------------------------------------------------------------------
    Activate Flow on every flow context on the list
  ---------------------------------------------------------------------*/          
  flow_context_p = list_peek_front(&(bflow_manager_p->flow_context));
  while(flow_context_p != NULL)
  {
    DS_3GPP_MSG0_HIGH("Flow context list is not empty");
    ds_flow_context_ps_flow_activate_ind(flow_context_p, ps_eic); 
    flow_context_p = list_peek_next(&(bflow_manager_p->flow_context),
                                      &(flow_context_p->link));
  } 
#endif
  return TRUE;
}/* ds_bearer_flow_manager_activate_flow */


/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_SUSPEND_FLOW

DESCRIPTION
  Suspend the flows associated with the bearer.

PARAMETERS
 bearer_context_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_suspend_flow
( 
  ds_bearer_context_s *bearer_context_p
)
{
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;
  ds_flow_context_s        *flow_context_p  = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(bearer_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input Bearer Pointer is NULL");
    return FALSE;
  }

  bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return FALSE;
  } 
  /*---------------------------------------------------------------------
    Suspend Flow on every flow context on the list
  ---------------------------------------------------------------------*/          
  flow_context_p = list_peek_front(&(bflow_manager_p->
                                     ds_bearer_flow_manager_dyn_p->flow_context));
  while(ds_flow_context_validate_flow(flow_context_p))
  {
    DS_3GPP_MSG0_HIGH("Flow context list is not empty");
    ds_flow_context_ps_flow_suspend_ind(flow_context_p); 
    flow_context_p = list_peek_next(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->
                       flow_context),&(flow_context_p->link));
  } 
  return TRUE;
}/* ds_bearer_flow_manager_suspend_flow */


/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_GO_NULL_FLOW

DESCRIPTION
  Post go Null on the flows associated with the bearer.

PARAMETERS
 bearer_context_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_go_null_flow
( 
  ds_bearer_context_s *bearer_context_p
)
{
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;
  ds_flow_context_s        *flow_context_p  = NULL;
  ds_flow_context_s        *pend_flow_context_p  = NULL;
  ps_extended_info_code_enum_type ps_eic = PS_EIC_NOT_SPECIFIED; 
  ds_pdn_context_s         *pdn_context_p = NULL;
  ds_flow_manager_s        *flow_manager_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }
  pdn_context_p = (ds_pdn_context_s *)bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;

  bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return FALSE;
  } 
  /*---------------------------------------------------------------------
     Go NUll indication on every flow context on the list
  ---------------------------------------------------------------------*/          
  flow_context_p = list_peek_front(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                                     ->flow_context));

  while(ds_flow_context_validate_flow(flow_context_p))
  {
    DS_3GPP_MSG0_HIGH("Flow context list is not empty");
    ds_flow_context_ps_flow_go_null_ind(flow_context_p, ps_eic); 
    flow_context_p = list_peek_next(&(bflow_manager_p->
                                      ds_bearer_flow_manager_dyn_p->flow_context),
                                   &(flow_context_p->link));
  }     
  /*---------------------------------------------------------------------
     Go NULL indication on every flow context in the pending flow list.
     When modify is still pending with network but DS gets CALL_END from
     CM then before posting phys_link_gone_ind() during call teardown we
     need to clear all flows(includinf flows from pending flow list) for
     this bearer. 
  ---------------------------------------------------------------------*/          
  DS_3GPP_MSG0_HIGH("Checking Pending Flow context list, if any");
  if (ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    flow_manager_p = ds_flow_manager_find_flow_manager(pdn_context_p);
    if(ds_flow_manager_validate_flow_manager(flow_manager_p))
    {
      pend_flow_context_p = list_peek_front(&(flow_manager_p->ds_flow_manager_dyn_p->pending_flow_cntx));
      while(ds_flow_context_validate_flow(pend_flow_context_p))
      {
        if(pend_flow_context_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr == bflow_manager_p)
        {
          DS_3GPP_MSG1_HIGH("Found flow_context_ptr 0x%x in Pending list",pend_flow_context_p);
          ds_flow_context_release_flow_context(pend_flow_context_p); 
          list_pop_item(&(flow_manager_p->ds_flow_manager_dyn_p->pending_flow_cntx),
                        &(pend_flow_context_p->link));
          //break;
        }
        pend_flow_context_p = list_peek_next(&(flow_manager_p->ds_flow_manager_dyn_p->pending_flow_cntx),
                                             &(pend_flow_context_p->link));
      } /* while ... */
    }     
  }         
  return TRUE;
}/* ds_bearer_flow_manager_go_null_flow */


/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_FIND_NW_FLOW_CONTEXT

DESCRIPTION
  Find the Flow context associated with the NW initiated Qos.

PARAMETERS
 bearer_context_p     - Pointer to Bearer information.
 iface_type           - Indicates V4 or V6 flow.

DEPENDENCIES
  None.

RETURN VALUE
 flow_context_p       - Pointer to Bearer information.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_flow_context_s * ds_bearer_flow_manager_find_nw_flow_context
( 
  ds_bearer_context_s *bearer_context_p,
  uint8                iface_type
)
{
  return ds_bearer_flow_manager_find_flow_context(bearer_context_p, iface_type, TRUE);
}/* ds_bearer_flow_manager_find_nw_flow_context */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_FIND_FLOW_CONTEXT

DESCRIPTION
  Find the Flow context associated with the Qos.

PARAMETERS
 bearer_context_p     - Pointer to Bearer information.
 iface_type           - Indicates V4 or V6 flow.
 is_nw_flow           - flag to indicate whether QOS is nw init

DEPENDENCIES
  None.

RETURN VALUE
 flow_context_p       - Pointer to Bearer information.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_flow_context_s * ds_bearer_flow_manager_find_flow_context
( 
  ds_bearer_context_s *bearer_context_p,
  uint8                iface_type,
  boolean              is_nw_flow
)
{
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;
  ds_flow_context_s        *flow_context_p  = NULL;
  ds_flow_context_s        *ret_flow_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(bearer_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input Bearer Pointer is NULL");
    return NULL;
  }

  bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return NULL;
  } 
  /*---------------------------------------------------------------------
     Find FlowContext in the list
  ---------------------------------------------------------------------*/          
  flow_context_p = list_peek_front(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                                     ->flow_context));
  while(ds_flow_context_validate_flow(flow_context_p))
  {
    if((is_nw_flow == TRUE) && 
      !(flow_context_p->ds_flow_context_dyn_p->flow_type & DS_3GPP_NW_FLOW_TYPE))
    {
    
      flow_context_p = list_peek_next(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->flow_context),
	  	                              &(flow_context_p->link));
      continue;
    }
    if((flow_context_p->ds_flow_context_dyn_p->flow_type & iface_type))
    {
      if(ret_flow_context_p == NULL)
      {
        ret_flow_context_p = flow_context_p;
      }
      else
      {
        DS_3GPP_MSG0_ERROR("More than one  flow context for bearer");
        break;
      }
    }
    flow_context_p = list_peek_next(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                     ->flow_context),&(flow_context_p->link));
  }     
  if(ret_flow_context_p == NULL)
  {  
    DS_3GPP_MSG3_ERROR("No flow context found for given iface type",0, 0, 0);
  }
  return ret_flow_context_p;
}/* ds_bearer_flow_manager_find_flow_context */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_CONVERT_DATA_RATE

DESCRIPTION
  Convert the NW FLOW to APP Flow. Invoke the mode specific handlers to 
  convert the value.

PARAMETERS
  bearer_context_p -  Bearer information
  mode_info_p      -  mode specific call info. 
  app_qos_p        -  Pointer to App Qos structure.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean  ds_bearer_flow_manager_convert_data_rate
(
 ds_bearer_context_s             *bearer_context_p,
 const cm_call_mode_info_s_type  *mode_info_p,
 qos_spec_type                   *app_qos_p,
 ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{
  boolean                   result = DS3G_FAILURE;
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;
  /*-----------------------------------------------------------------------*/
  
  bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(bearer_context_p);

  if(cause_code_p == NULL)
  {
    return DS3G_FAILURE;
  }
  else
  {
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
  }
  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    return DS3G_FAILURE;
  }
  // Update to use the current system mode instead of hardcoding
  result = ds_bearer_flow_manager_nw_data_rate_to_app_data_rate(
                        bflow_manager_p,(void *)mode_info_p, app_qos_p,cause_code_p);
  return(result);                         
} /*  ds_bearer_flow_manager_convert_data_rate */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_CONVERT_APP_QOS_TO_NW_QOS

DESCRIPTION
  Converts the APP passed Flow spec to NW Qos format.

PARAMETERS
 app_flow_spec     -  App passed Qos
 nw_qos            -  network Qos to be filled
 subs_id           -  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
boolean          - returns if conversion is successfull
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_convert_app_qos_to_nw_qos
(
   qos_spec_type           *app_flow_spec,
   void                    *nw_qos,
   sys_modem_as_id_e_type   subs_id
)
{
  boolean                    result = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("Convert App Qos to NW Qos");

  if((nw_qos == NULL) || (app_flow_spec == NULL))
  {
    DS_3GPP_MSG0_ERROR("Null params passed as input");
    return DS3G_FAILURE;
  }
    
  result = ds_bearer_flow_manager_app_data_rate_to_nw_data_rate
           (
             app_flow_spec, 
             nw_qos,
             subs_id
           );

  return (result); 
}/* ds_bearer_flow_manager_convert_app_qos_to_nw_qos */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_NW_DATA_RATE_TO_APP_DATA_RATE

DESCRIPTION
  This function is invoked to convert data rate from the network specific
  format to the application format.

PARAMETERS
  bflow_mgr_p   -  'this' ptr
  nw_qos        -  network Qos 
  app_flow_spec -  App Qos

DEPENDENCIES
  None.

RETURN VALUE
  boolean      - returns DS3G_SUCCESS on success, DS3G_FAILURE otherwise
    
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_nw_data_rate_to_app_data_rate
(
  ds_bearer_flow_manager_s              *bflow_mgr_p,
  void                                  *nw_qos,
  qos_spec_type                         *app_qos,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{
  boolean                rval = DS3G_FAILURE;
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
  /*---------------------------------------------------------------------*/

  if(cause_code_p == NULL)
  {
    DS_3GPP_MSG0_HIGH("Null Cause code ptr.");
    return FALSE;
  }

  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_mgr_p))
  {
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  }

  if (!ds_bearer_cntxt_get_subs_id_from_bearer
      (bflow_mgr_p->ds_bearer_flow_manager_dyn_p->bearer_ptr, &subs_id))
  {

    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  }

  DS_BEARER_FLOW_MGR_VFR_CALL
  (
    rval, 
    nwDataRateToAppDataRate, 
    subs_id,
    bflow_mgr_p,
    nw_qos,
    app_qos,
    cause_code_p
  );

  return rval;
}/* ds_bearer_flow_manager_nw_data_rate_to_app_data_rate*/

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)

/*===========================================================================
FUNCTION  DS_3GPP_BEARER_FLOW_MANAGER_CONVERT_NW_QOS_TO_APP
DESCRIPTION
  Converts the UMTS NW passed data rate to App data rate.

PARAMETERS
nw_qos          -  EPS network qos to be converted
app_qos         -  Application qos structure
DEPENDENCIES
  None.

RETURN VALUE
boolean         - return DS3G_SUCCESS or DS3G_FAILURE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_bearer_flow_manager_convert_nw_qos_to_app
(
  ds_bearer_flow_manager_s *bflow_manager_p,
  void           *nw_qos,
  qos_spec_type  *app_qos,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{
  qos_T *nw_alloc_qos;
  ip_flow_type rx_flow, tx_flow;
  int sdu_exp_lookup[8] = { 0, 2, 3, 3, 4, 5, 6, 1 };
  int sdu_mult_lookup[8] = { 0, 1, 7, 1, 1, 1, 1, 1 };

  if(cause_code_p == NULL)
  {
    return DS3G_FAILURE;
  }
  else
  {
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
  }

  if (nw_qos == NULL)
  {
    DS_3GPP_MSG0_ERROR("Cannot convert NW qos to apps, input qos is null");
    return FALSE;
  }

  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return FALSE;
  }

  nw_alloc_qos = (qos_T*) nw_qos;
  /*------------------------------------------------------------------------
    Application requires two kind of flow specs (rx_spec and tx_spec).
    In UMTS, except data rates and guaranteed data rates, other parameters
    are common across UL and DL. The table below describes how we convert
    the network allocated parameters into application specific parameters.

    RX Traffic Class             : Common Traffic Class parameter
    RX Traffic Handling Priority : Common Traffic Handling Priority
    RX RES BER                   : Common RES BER
    RX Packet Err Ratio          : Common SDU Error Ratio

    TX Traffic Class             : Common Traffic Class parameter
    TX Traffic Handling Priority : Common Traffic Handling Priority
    TX RES BER                   : Common RES BER
    TX Packet Err Ratio          : Common SDU Error Ratio

    RX Max Data Rate          :  DL Data Rate
    RX guaranteed Data Rate   :  DL Gauranteed Data Rate


    TX Max Data Rate          :  UL Data Rate
    TX guaranteed Data Rate   :  UL Gauranteed Data Rate

    Note that Data Rate format type will always be
    DATA_RATE_FORMAT_MIN_MAX_TYPE
  ------------------------------------------------------------------------*/
  if(nw_alloc_qos->valid == FALSE )
  {
    DS_3GPP_MSG0_ERROR("Cannot convert NW qos to apps, valid flag not set");
    /* using the earlier default value */
    *cause_code_p = DS_3GPP_INTERNAL_QOS_NOT_ACCEPTED;
    return FALSE;
  }
  else
  {
    DS_3GPP_MSG0_HIGH("Converting Net Allocated QOS into App QOS");

    DS_3GPP_MSG9_HIGH("delay class:%x, rel class :%x, peak :%x, precd class:%x,      \
                       mean :%x, traff class:%x, del order :%x, del err sdu:%x,      \
                       max sdu :%x",                                                 \
                       nw_alloc_qos->delay_class, nw_alloc_qos->reliability_class,   \
                       nw_alloc_qos->peak_throughput, nw_alloc_qos->precedence_class,\
                       nw_alloc_qos->mean_throughput, nw_alloc_qos->traffic_class,   \
                       nw_alloc_qos->deliver_order, nw_alloc_qos->deliver_err_sdu,   \
                       nw_alloc_qos->max_sdu_size);

    DS_3GPP_MSG8_HIGH("Max UL rate:%x, Max DL rate:%x, Res BER :%x,   \
                       sdu err rat:%x, txfer delay:%x, traff prio :%x, \
                       guar UL rate:%x, guar DL rate:%x",              \
                       nw_alloc_qos->max_bit_rare_uplink,              \
                       nw_alloc_qos->max_bit_rare_downlink,            \
                       nw_alloc_qos->residual_ber,                     \
                       nw_alloc_qos->sdu_err_ratio,                    \
                       nw_alloc_qos->tranfer_delay,                    \
                       nw_alloc_qos->traffic_priority,                 \
                       nw_alloc_qos->guarantee_ber_uplink,             \
                       nw_alloc_qos->guarantee_ber_dwnlink);

    memset(&rx_flow, 0, sizeof(ip_flow_type));
    memset(&tx_flow, 0, sizeof(ip_flow_type));

    if( (nw_alloc_qos->max_bit_rare_uplink == QOS_BIT_RATE_SUBSCRIBED)&&
        (nw_alloc_qos->max_bit_rare_downlink == QOS_BIT_RATE_SUBSCRIBED))
    {
       DS_3GPP_MSG0_ERROR("Network supports only R97/98, convert r97/98 to r99 formate to notify AP");

       ds_bearer_flow_manager_convert_nw_r9798_qos_to_app_r99_qos(nw_alloc_qos, &tx_flow, &rx_flow);
    }	
    else 
    {    	
    /*------------------------------------------------------------------------
        RX FLOW SPEC: Convert RX RES BER. RES BER is offset by 1.
      ------------------------------------------------------------------------*/
    /*validations considering offset by 1)*/ 
   	 		 
    if((nw_alloc_qos->residual_ber <= 0) ||
       (nw_alloc_qos->residual_ber > UMTS_RES_BIT_ERR_RATE10 ))
    {
      rx_flow.umts_params.res_ber = UMTS_RES_BIT_ERR_RATE10;
     /* reserved value for res_ber*/
    }
    else
    {
      rx_flow.umts_params.res_ber = (umts_residual_ber_enum_type)
                           (nw_alloc_qos->residual_ber - 1);
      rx_flow.field_mask |= (int)IPFLOW_MASK_UMTS_RES_BER;
    }
      /*------------------------------------------------------------------------
        RX FLOW SPEC: Convert Traffic Class. IP traffic class if ofsset by 1
        as there is no subscribed class. 
      ------------------------------------------------------------------------*/
    if((nw_alloc_qos->traffic_class  <= 0) ||
       (nw_alloc_qos->traffic_class  > IP_TRF_CLASS_MAX ))
    {
      rx_flow.trf_class = IP_TRF_CLASS_MIN;
     /* reserved value for traffic class*/
    }
    else 
    {
      rx_flow.trf_class = (ip_traffic_class_enum_type)
                         (nw_alloc_qos->traffic_class - 1);
      rx_flow.field_mask |= (int)IPFLOW_MASK_TRF_CLASS;
    }
      /*------------------------------------------------------------------------
        RX FLOW SPEC: Convert Traffic Handling Priority
      ------------------------------------------------------------------------*/
     if ( (rx_flow.trf_class == IP_TRF_CLASS_INTERACTIVE) &&
          (rx_flow.field_mask & IPFLOW_MASK_TRF_CLASS) )
      {
        rx_flow.umts_params.trf_pri = (umts_trf_handling_pri_enum_type)
                      (nw_alloc_qos->traffic_priority - 1);
        rx_flow.field_mask |= (int)IPFLOW_MASK_UMTS_TRF_PRI;
      }
      /*------------------------------------------------------------------------
        RX FLOW SPEC: Convert RX Data Rate
      ------------------------------------------------------------------------*/
      rx_flow.data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;
      rx_flow.data_rate.format.min_max.max_rate =
                     dsumtsps_conv_netw_data_rate_to_umts_data_rate( 
                                nw_alloc_qos->max_bit_rare_downlink ) * 1000 +
                    dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_decode_val(
                                nw_alloc_qos->ext_max_bit_rate_downlink);

      /*------------------------------------------------------------------------
        RX FLOW SPEC: Convert RX Guaranteed Data Rate
      ------------------------------------------------------------------------*/
      if ( ((rx_flow.trf_class != IP_TRF_CLASS_INTERACTIVE )&&
           (rx_flow.field_mask & IPFLOW_MASK_TRF_CLASS)) &&
           (( rx_flow.trf_class != IP_TRF_CLASS_BACKGROUND )&&
            (rx_flow.field_mask & IPFLOW_MASK_TRF_CLASS))   &&
            (rx_flow.data_rate.format.min_max.max_rate != 0))
      {
         rx_flow.data_rate.format.min_max.guaranteed_rate =
  	   	          dsumtsps_conv_netw_data_rate_to_umts_data_rate( 
               	   	         nw_alloc_qos->guarantee_ber_dwnlink ) * 1000 +
               	          dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_decode_val(
                                 nw_alloc_qos->ext_gtd_bit_rate_downlink);
      }  
       rx_flow.field_mask |= (int)IPFLOW_MASK_DATA_RATE;

      /*------------------------------------------------------------------------
        RX FLOW SPEC: Convert Packet Error Rate
      ------------------------------------------------------------------------*/
      rx_flow.pkt_err_rate.multiplier =
        (uint16)sdu_mult_lookup[nw_alloc_qos->sdu_err_ratio];
      rx_flow.pkt_err_rate.exponent =
        (uint16)sdu_exp_lookup[nw_alloc_qos->sdu_err_ratio];
      rx_flow.field_mask |= (int)IPFLOW_MASK_PKT_ERR_RATE;
  
      /*------------------------------------------------------------------------
        TX FLOW SPEC: Convert RES BER
      ------------------------------------------------------------------------*/
      /*validations considering offset by 1)*/ 
   	 		 
      if((nw_alloc_qos->residual_ber <= 0) ||
         (nw_alloc_qos->residual_ber > UMTS_RES_BIT_ERR_RATE10 ))
      {
        tx_flow.umts_params.res_ber = UMTS_RES_BIT_ERR_RATE10;
       /* reserved value for res_ber*/
      }
      else
      {
        tx_flow.umts_params.res_ber = (umts_residual_ber_enum_type)
                             (nw_alloc_qos->residual_ber - 1);
        tx_flow.field_mask |= (int)IPFLOW_MASK_UMTS_RES_BER;
      }
      /*------------------------------------------------------------------------
        TX FLOW SPEC: Convert Traffic Class. IP traffic class if ofsset by 1
        as there is no subscribed class. 
      ------------------------------------------------------------------------*/
      if((nw_alloc_qos->traffic_class  <= 0) ||
         (nw_alloc_qos->traffic_class  > IP_TRF_CLASS_MAX ))
      {
        tx_flow.trf_class = IP_TRF_CLASS_MIN;
       /* reserved value for traffic class*/
      }
      else 
      {
        tx_flow.trf_class = (ip_traffic_class_enum_type)
                            (nw_alloc_qos->traffic_class - 1);
        tx_flow.field_mask |= (int)IPFLOW_MASK_TRF_CLASS;
      }
      /*------------------------------------------------------------------------
        TX FLOW SPEC: Convert Traffic Handling Priority
      ------------------------------------------------------------------------*/
      if ( (tx_flow.trf_class == IP_TRF_CLASS_INTERACTIVE)&&
           (tx_flow.field_mask&IPFLOW_MASK_TRF_CLASS) )
      {
        tx_flow.umts_params.trf_pri = (umts_trf_handling_pri_enum_type)
                      ( nw_alloc_qos->traffic_priority -1 );
        tx_flow.field_mask |= (int)IPFLOW_MASK_UMTS_TRF_PRI;
      }
      /*------------------------------------------------------------------------
        TX FLOW SPEC: Convert RX Data Rate
      ------------------------------------------------------------------------*/
      tx_flow.data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;
      tx_flow.data_rate.format.min_max.max_rate =
                              dsumtsps_conv_netw_data_rate_to_umts_data_rate(
                                       nw_alloc_qos->max_bit_rare_uplink ) * 1000 +
                                dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_decode_val(
                                     nw_alloc_qos->ext_max_bit_rate_uplink);
  
      /*------------------------------------------------------------------------
       TX FLOW SPEC: Convert RX Guaranteed Data Rate
        ------------------------------------------------------------------------*/
      if (((tx_flow.trf_class != IP_TRF_CLASS_INTERACTIVE )&&
           (tx_flow.field_mask&IPFLOW_MASK_TRF_CLASS))&&
           (( tx_flow.trf_class != IP_TRF_CLASS_BACKGROUND ) &&
            (tx_flow.field_mask&IPFLOW_MASK_TRF_CLASS))  &&
            (tx_flow.data_rate.format.min_max.max_rate != 0))
      {
          tx_flow.data_rate.format.min_max.guaranteed_rate =
                                 dsumtsps_conv_netw_data_rate_to_umts_data_rate(
               	   	                nw_alloc_qos->guarantee_ber_uplink ) * 1000 +
             	             dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_decode_val(
                                        nw_alloc_qos->ext_gtd_bit_rate_uplink);
      }
      tx_flow.field_mask |= (int)IPFLOW_MASK_DATA_RATE;
  

      /*------------------------------------------------------------------------
        TX FLOW SPEC: Convert Packet Error Rate
      ------------------------------------------------------------------------*/
      tx_flow.pkt_err_rate.multiplier =
        (uint16)sdu_mult_lookup[nw_alloc_qos->sdu_err_ratio];
      tx_flow.pkt_err_rate.exponent =
         (uint16)sdu_exp_lookup[nw_alloc_qos->sdu_err_ratio];
      tx_flow.field_mask |= (int)IPFLOW_MASK_PKT_ERR_RATE;
  
      /* Signaling Indication Bit is valid when Traffic Class is Interactive and
         traffic priority level is UMTS_TRF_HANDLING_PRI1 */
      if ( ((tx_flow.field_mask & IPFLOW_MASK_TRF_CLASS) &&
           (tx_flow.trf_class == IP_TRF_CLASS_INTERACTIVE))&&
           ((((uint8) tx_flow.umts_params.trf_pri) == UMTS_TRF_HANDLING_PRI1)&&
           (tx_flow.field_mask & IPFLOW_MASK_UMTS_TRF_PRI)))
      {
        /*------------------------------------------------------------------------
          Convert Signalling Indication Flag
        ------------------------------------------------------------------------*/
        tx_flow.umts_params.sig_ind =
          nw_alloc_qos->sig_ind;
        tx_flow.field_mask |= ((uint32) IPFLOW_MASK_UMTS_SIG_IND);
      }
  
      if ( (rx_flow.field_mask & IPFLOW_MASK_TRF_CLASS) &&
           (rx_flow.trf_class == IP_TRF_CLASS_INTERACTIVE ) &&
           (rx_flow.field_mask & IPFLOW_MASK_UMTS_TRF_PRI)&&
           (((uint8) rx_flow.umts_params.trf_pri) == UMTS_TRF_HANDLING_PRI1))
      {
        /*------------------------------------------------------------------------
          Convert Signalling Indication Flag
        ------------------------------------------------------------------------*/
        rx_flow.umts_params.sig_ind =
          nw_alloc_qos->sig_ind;
        rx_flow.field_mask |= ((uint32) IPFLOW_MASK_UMTS_SIG_IND);
      }
  
      /* Source statistics Descriptor is valid when Traffic Class is Conversational
       or Streaming */
      if ( (tx_flow.field_mask & IPFLOW_MASK_TRF_CLASS)&&
         ((tx_flow.trf_class == IP_TRF_CLASS_CONVERSATIONAL) ||
          (tx_flow.trf_class == IP_TRF_CLASS_STREAMING)))
      {
        /*------------------------------------------------------------------------
          Convert Source Statistics Descriptor
        ------------------------------------------------------------------------*/
        tx_flow.umts_params.src_stat_desc = 
          nw_alloc_qos->src_stat_desc;
        tx_flow.field_mask |= ((uint32) IPFLOW_MASK_UMTS_SRC_STAT_DESC);
    }
  
      if ( (rx_flow.field_mask & IPFLOW_MASK_TRF_CLASS) &&
           ((rx_flow.trf_class == IP_TRF_CLASS_CONVERSATIONAL) ||
            (rx_flow.trf_class == IP_TRF_CLASS_STREAMING)))
           
      {
          /*------------------------------------------------------------------------
            Convert Signalling Indication Flag
          ------------------------------------------------------------------------*/
          rx_flow.umts_params.src_stat_desc =
            nw_alloc_qos->src_stat_desc;
        rx_flow.field_mask |= ((uint32) IPFLOW_MASK_UMTS_SIG_IND);
      }
      if((tx_flow.data_rate.format.min_max.max_rate == QOS_BIT_RATE_0KBPS) &&
         (rx_flow.data_rate.format.min_max.max_rate == QOS_BIT_RATE_0KBPS))
      {
        *cause_code_p = DS_3GPP_INTERNAL_INVALID_MANDATORY_INFO;
        return FALSE;
      }
    }
    /*lint -e506 -e774 */
    ASSERT ( app_qos != NULL );
    app_qos->rx.flow_template.req_flow = rx_flow;
    app_qos->rx.flow_template.min_req_flow.field_mask = 
                               (ipflow_field_mask_type)IPFLOW_MASK_NONE;
    app_qos->rx.flow_template.num_aux_flows = 0;
    app_qos->rx.flow_template.aux_flow_list_ptr = NULL;

    app_qos->tx.flow_template.req_flow = tx_flow;
    app_qos->tx.flow_template.min_req_flow.field_mask = 
                               (ipflow_field_mask_type)IPFLOW_MASK_NONE;
    app_qos->tx.flow_template.num_aux_flows = 0;
    app_qos->tx.flow_template.aux_flow_list_ptr = NULL;
  }
  /* Mode independent*/
  memscpy((void *)&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->rx_flow),
           sizeof(ip_flow_type),
          (void *)&(app_qos->rx.flow_template.req_flow),sizeof(ip_flow_type));         

  memscpy((void *)&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->tx_flow),
  	   sizeof(ip_flow_type),
          (void *)&(app_qos->tx.flow_template.req_flow),sizeof(ip_flow_type)); 
  /*lint +e506 +e774 */
  return TRUE;
}/* ds_3gpp_bearer_flow_manager_convert_nw_data_rate_to_app*/

#endif
/*===========================================================================
FUNCTION  DS_3GPP_BEARER_FLOW_MANAGER_GET_APP_DATA_RATE

DESCRIPTION
  Gives the App data rate.

PARAMETERS
  bearer_flow_manager        -  Bearer flow manager ptr
  pdn_aggregate_rate         -  aggregate data rate

DEPENDENCIES
  None.

RETURN VALUE
  boolean                    - return DS3G_SUCCESS or DS3G_FAILURE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_bearer_flow_manager_get_app_data_rate
(
  ds_bearer_flow_manager_s *bearer_flow_manager,
  void                     *pdn_aggregate_rate
)
{
  ps_iface_ioctl_data_bearer_rate *rate_info = 
    (ps_iface_ioctl_data_bearer_rate *)pdn_aggregate_rate;

  if ( rate_info == NULL )
  {
    DS_3GPP_MSG0_ERROR( "pdn_aggregate_rate is NULL");
    return FALSE;
  }
  if (!ds_bearer_flow_manager_validate_flow_manager(bearer_flow_manager))
  {
    rate_info->max_rx_bearer_data_rate = 0;
    rate_info->max_tx_bearer_data_rate = 0;
    return FALSE;
  }
  else
  {
    /*-------------------------------------------------------------------------
      Get uplink max rb rate from NW granted UL rate.
    -------------------------------------------------------------------------*/
    rate_info->max_tx_bearer_data_rate = 
            bearer_flow_manager->ds_bearer_flow_manager_dyn_p->
            tx_flow.data_rate.format.min_max.max_rate;
  
    /*-------------------------------------------------------------------------
      Get downlink max rb rate from NW granted DL rate.
    -------------------------------------------------------------------------*/
    rate_info->max_rx_bearer_data_rate = 
              bearer_flow_manager->ds_bearer_flow_manager_dyn_p->
              rx_flow.data_rate.format.min_max.max_rate;
  }/* ds_3gpp_bearer_flow_manager_get_app_data_rate*/
  return TRUE;
}/* ds_3gpp_bearer_flow_manager_get_app_data_rate */

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION  DS_EPS_BEARER_FLOW_MANAGER_CONVERT_NW_SDF_QOS_TO_APP
DESCRIPTION
  Converts the SDF QOS NW passed to App data rate.

PARAMETERS
nw_qos          -  SDF qos to be converted
app_qos         -  Application qos structure
DEPENDENCIES
  None.

RETURN VALUE
boolean         - return DS3G_SUCCESS or DS3G_FAILURE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_bearer_flow_manager_convert_nw_sdf_qos_to_app
(
  ds_bearer_flow_manager_s *bflow_manager_p,
  void           *nw_qos,
  qos_spec_type  *app_qos,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{
  boolean                   result = DS3G_SUCCESS; 
  sdf_qos_T                 *lte_qos_ptr = NULL;
  ip_flow_type                *app_rx_flow = NULL;
  ip_flow_type                *app_tx_flow = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cause_code_p == NULL)
  {
    return FALSE;
  }
  else
  {
    *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
  }

  if(nw_qos == NULL || app_qos == NULL)
  {
    DS_3GPP_MSG0_ERROR("input parameters are NULL");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    get the LTE specific info.  
  -------------------------------------------------------------------------*/
  lte_qos_ptr  = (sdf_qos_T *) nw_qos;

  /*------------------------------------------------------------------------
    Convert LTE QOS values to PS-Specific Qos value
  -------------------------------------------------------------------------*/
  if(lte_qos_ptr->qci >=(byte)(LTE_QCI_1)
                && lte_qos_ptr->qci <=(byte)(LTE_QCI_4))
  {
    app_rx_flow = &(app_qos->rx.flow_template.req_flow);    
    if(app_rx_flow->lte_params.lte_qci >=(byte)(LTE_QCI_5)
                && app_rx_flow->lte_params.lte_qci <=(byte)(LTE_QCI_9))
    {
      DS_3GPP_MSG1_ERROR("Received qos with invalid QCI value conversion:%d",
                         lte_qos_ptr->qci);
      *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
      return FALSE;
    }

    DS_3GPP_MSG1_HIGH("Received GBR qos request with QCI value:%d",
		  lte_qos_ptr->qci);
    if(lte_qos_ptr->length!=DS_EPS_QOS_QCI_MIN_LEN_FOR_DATA 
       && lte_qos_ptr->length!=DS_EPS_QOS_QCI_LEN_FOR_DATA 
          && lte_qos_ptr->length!=DS_EPS_QOS_QCI_LEN_FOR_EXT_DATA
          && lte_qos_ptr->length!=DS_EPS_QOS_QCI_LEN_FOR_EXT2_DATA)
    {
      DS_3GPP_MSG2_ERROR("Received invalid qos length:%d for qci:%d",
         lte_qos_ptr->length,lte_qos_ptr->qci); 
      return FALSE;
    }  
    /*----------------------------------------------------------------------
      RX/TX FLOW SPEC: Convert Max and Guaranteed Data Rates (bps)
    -----------------------------------------------------------------------*/
    result = ds_eps_bearer_flow_manager_convert_nw_flow_to_app_flow(
                                                    lte_qos_ptr,
                                                    app_qos,
                                                    cause_code_p);
    if(result == FALSE)
    {
      return result;
    }
  } /* GBR QCI */
    else if(lte_qos_ptr->qci >= 0x80
          && lte_qos_ptr->qci <= 0xFE)
  {
    lte_qci_enum_type converted_qci = LTE_QCI_5;
    DS_3GPP_MSG1_HIGH("Received qos request with operator specified QCI value:%d",
                      lte_qos_ptr->qci);

    //See if we are able to get the data rate parameters.
    result = ds_eps_bearer_flow_manager_convert_nw_flow_to_app_flow(
                                                    lte_qos_ptr,
                                                    app_qos,
                                                    cause_code_p);

    if (FALSE == result)
    {
      //There is no set data rate associated with this bearer. Use non-GBR QCI.
      //Pick the worst QCI since there is no way of mapping to the correct
      //QCI without knowing more information.
      converted_qci = LTE_QCI_9;
    }
    else 
    {
      //There is set data rate. Use GBR QCI.
      converted_qci = LTE_QCI_4;
    }

    app_rx_flow = &(app_qos->rx.flow_template.req_flow);
    app_rx_flow->lte_params.lte_qci = converted_qci;
    app_rx_flow->field_mask |= (int)IPFLOW_MASK_LTE_QCI;

    app_tx_flow = &(app_qos->tx.flow_template.req_flow);
    app_tx_flow->lte_params.lte_qci = converted_qci;
    app_tx_flow->field_mask |= (int)IPFLOW_MASK_LTE_QCI;
  } /* N-GBR QCI*/ 
  else if(lte_qos_ptr->qci >=(byte)(LTE_QCI_5)
          && lte_qos_ptr->qci <=(byte)(LTE_QCI_9))
  {
    app_rx_flow = &(app_qos->rx.flow_template.req_flow); 
    if(app_rx_flow->lte_params.lte_qci >=(byte)(LTE_QCI_1)
                && app_rx_flow->lte_params.lte_qci <=(byte)(LTE_QCI_4))
    {
      DS_3GPP_MSG1_ERROR("Received qos with invalid QCI value conversion:%d",
                         lte_qos_ptr->qci);
      *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
      return FALSE;
    }


    DS_3GPP_MSG1_HIGH("Received NON-GBR qos request with QCI value:%d",
                      lte_qos_ptr->qci);
    app_rx_flow = &(app_qos->rx.flow_template.req_flow);
    app_rx_flow->lte_params.lte_qci =(lte_qci_enum_type)lte_qos_ptr->qci;
    app_rx_flow->field_mask|=(int)IPFLOW_MASK_LTE_QCI;

    app_tx_flow = &(app_qos->tx.flow_template.req_flow);
    app_tx_flow->lte_params.lte_qci =(lte_qci_enum_type)lte_qos_ptr->qci;
    app_tx_flow->field_mask|=(int)IPFLOW_MASK_LTE_QCI;
  } /* N-GBR QCI*/ 
  else
  {
    DS_3GPP_MSG1_ERROR("Received qos with invalid QCI value:%d",lte_qos_ptr->qci);
    *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
    return FALSE;
  }
  return TRUE;
}
#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_APP_DATA_RATE_TO_NW_DATA_RATE

DESCRIPTION
  This function is invoked to convert data rate from the application specific
  format to network format.

PARAMETERS
  app_flow_spec -  App Qos
  nw_qos        -  network Qos
  subs_id       -  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  boolean      - returns DS3G_SUCCESS on success, DS3G_FAILURE otherwise
    
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_app_data_rate_to_nw_data_rate
(
  qos_spec_type         *app_qos,
  void                  *nw_qos,
  sys_modem_as_id_e_type subs_id
)
{
  boolean rval = DS3G_FAILURE;

  DS_BEARER_FLOW_MGR_VFR_CALL
  (
    rval, 
    appDataRateToNwDataRate, 
    subs_id,
    app_qos,
    nw_qos
  );

  return rval;
}/* ds_bearer_flow_manager_app_data_rate_to_nw_data_rate*/

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_GET_BEARER_QOS

DESCRIPTION
  This function is invoked to retrieve bearer QOS details.Qos is stored in 
  NW format. The conversion to Application format has to be invoked
  before passing it to Application.

PARAMETERS
  bearer_flow_manager      -  Bearer flow manager ptr
  pdn_aggregate_rate       -  aggregate data rate
  sys_modem_as_id_e_type  -  subs_id

DEPENDENCIES
  None.

RETURN VALUE
  boolean      - returns DS3G_SUCCESS on success, DS3G_FAILURE otherwise
    
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_get_bearer_qos
(
  ds_bearer_flow_manager_s *bearer_flow_manager,
  void                     *pdn_aggregate_rate,
  sys_modem_as_id_e_type    subs_id
)
{
  boolean rval = DS3G_FAILURE;

  DS_BEARER_FLOW_MGR_VFR_CALL
  (
    rval,
    getBearerRate,
    subs_id,
    bearer_flow_manager,
    pdn_aggregate_rate
  );

  return rval;
}/* ds_bearer_flow_manager_get_bearer_qos*/

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_GET_QOS_FROM_BEARER_ID

DESCRIPTION
  Return the sdfqos ie stored in the bearer flow manager for a specific
  bearer

PARAMETERS
  eps_bearer_id         -  EPS bearer id for the bearer
  subs_id               -  Subscription id

DEPENDENCIES
  None.

RETURN VALUE
 void*             - pointer to the mode specific qos structure
  
SIDE EFFECTS
  None.

===========================================================================*/
void* ds_bearer_flow_manager_get_qos_from_bearer_id
(
  byte                    eps_bearer_id,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_bearer_context_s*           bearer_context_p = NULL;
  ds_bearer_flow_manager_s*      bflow_manager_p = NULL;
  ds_eps_bearer_flow_manager_s*  eps_bflow_manager_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  // get the bearer context pointer
  bearer_context_p = 
    ds_bearer_cntxt_get_bearer_from_eps_id(eps_bearer_id, subs_id);

  if (ds_bearer_cntx_validate_bearer_context(bearer_context_p) == NULL)
  {
    return NULL;
  }

  // get the bearer flow manager pointer
  bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return NULL;
  }

  // retrieve the qos param based on current mode

  if (ds_3gpp_is_context_pref_pkt_mode(bearer_context_p->
        ds_bearer_context_dyn_p->pdn_context_ptr,SYS_SYS_MODE_LTE) == TRUE)
  {
    eps_bflow_manager_p = 
     (ds_eps_bearer_flow_manager_s *)(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                                      ->vobj_data_ptr_tbl[DS_3GPP_EPS_VOBJ]);
    if(!ds_eps_bearer_flow_manager_validate_eps_bearer_flow_manager
       (eps_bflow_manager_p))
    {
      DS_3GPP_MSG0_ERROR("eps_bearer_context_p is NULL");
      return NULL;
    }
    else
    {
      return (void*) (&(eps_bflow_manager_p->eps_bearer_flow_manager_dyn_p->sdf_qos));
    }
  }
  else
  {
    // place holder for other modes
    DS_3GPP_MSG0_ERROR("eps_bearer_context_p is NULL");
  }
  return NULL;
} /* ds_bearer_flow_manager_get_qos_from_bearer_id */
#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_FLOW_VALIDATION

DESCRIPTION
  This function is invoked to validate flow parameters provided by the app.

PARAMETERS
  input_qos_p - pointer to flow/qos provided by the app.
  subs_id     - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  boolean - success/failure
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_flow_validation
(
  qos_spec_type           *input_qos_p,
  sys_modem_as_id_e_type   subs_id
)
{
  boolean rval = FALSE;

  DS_BEARER_FLOW_MGR_VFR_CALL
  (
    rval, 
    flowValidation, 
    subs_id,
    input_qos_p
  );

  return rval;
}/* ds_bearer_flow_manager_flow_validation */
/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_IFACE_OUT_OF_USE

DESCRIPTION
  The iface has gone out of use. Check for an UE initiated bearers and
  clean them up. Update the TFT information that needs to be sent out to 
  the network.

PARAMETERS
  pdn_context_p            -  Pointer to PDN context.
  bearer_context_p         -  Pointer to Bearer context.
  flow_type                -  IP Type.
  delete_nw_flow           -  boolean. IF true NW flow needs to be deleted.

DEPENDENCIES
  None.

RETURN VALUE
  flow_context_p          -    Returns pointer to flow context to modify.
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
ds_flow_context_s * ds_bearer_flow_manager_iface_out_of_use
(
  ds_pdn_context_s                *pdn_context_p,
  ds_bearer_context_s             *bearer_context_p,
  uint8                            flow_type,
  boolean                          delete_nw_flow
)
{
  ds_flow_context_s        *flow_context_p  = NULL;
  ds_flow_context_s        *nw_flow_context_p = NULL;
  ds_flow_context_s        *old_flow_context_p = NULL;
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (bearer_context_p == NULL) ||(pdn_context_p == NULL))
  {
    DS_3GPP_MSG0_ERROR("Input param is null");
    return NULL;
  }

  if((flow_type != DS_3GPP_V4_FLOW_TYPE)&&(flow_type != DS_3GPP_V6_FLOW_TYPE))
  {
    DS_3GPP_MSG0_ERROR("invalid flow type passed");
    return NULL;
  }

  bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(
                                                      bearer_context_p);
  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return NULL;
  }

  /*---------------------------------------------------------------------
    Step through all the flow contexts.
  ---------------------------------------------------------------------*/  
  flow_context_p = list_peek_front(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                                     ->flow_context));
  while(ds_flow_context_validate_flow(flow_context_p))
  {
    old_flow_context_p = NULL;
    if(flow_context_p->ds_flow_context_dyn_p->flow_type == flow_type)
    {
      if(flow_context_p->ds_flow_context_dyn_p->state != DS_FLOW_CONTEXT_STATE_UP)
      {
        DS_3GPP_MSG0_ERROR("Flow context is not in up state");
      }
      else
      {
        if(nw_flow_context_p == NULL)
        {
          nw_flow_context_p = flow_context_p;
          DS_3GPP_MSG1_HIGH("TFT in flow context:%x used for NW update",
                                            nw_flow_context_p);
        }
        if(ds_3gpp_tft_update_nw_delete_tft(&(nw_flow_context_p->
                          ds_flow_context_dyn_p->tft_mod),flow_context_p)== FALSE)
        {
          DS_3GPP_MSG1_ERROR("Error when deleting flow:%x",flow_context_p);
          return NULL;
        }
        // if this not the nw flow context, delete the flow context.
        if(nw_flow_context_p != flow_context_p)     
        {
          old_flow_context_p = flow_context_p;
        }
      }/* else */
    }/* flow type matches */
    flow_context_p = list_peek_next(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                     ->flow_context),&(flow_context_p->link));
    if(old_flow_context_p)
    {
       DS_3GPP_MSG1_HIGH("Removing the flow context 0x:%x",
                                                     old_flow_context_p);
      /*---------------------------------------------------------------------
        Delete the precedence Bit Mask.
      ---------------------------------------------------------------------*/
      if(ds_flow_manager_update_precedence_bit_mask(
                    pdn_context_p,old_flow_context_p,FALSE,FALSE,FALSE)==FALSE)
      {
        DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
      }
      /*---------------------------------------------------------------------
        Delete the Identifier Bit Mask.
      ---------------------------------------------------------------------*/
      if(ds_bearer_flow_manager_update_filter_identifier_bit_mask(
                      bflow_manager_p->ds_bearer_flow_manager_dyn_p->bearer_ptr,
                      old_flow_context_p,FALSE)==FALSE)
      {
        DS_3GPP_MSG0_ERROR("Error setting Filter identifier mask");
      }
      /*-------------------------------------------------------------------
        Remove the flow context from bearer flow manager.
       ---------------------------------------------------------------------*/
      if(ds_bearer_flow_manager_remove_flow_context_from_flow_manager(
               bflow_manager_p->ds_bearer_flow_manager_dyn_p->
               bearer_ptr,old_flow_context_p) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("Error in removing the pending context");
      }
      /*---------------------------------------------------------------------
        Delete the flow context
      ---------------------------------------------------------------------*/
      ds_flow_context_release_flow_context(old_flow_context_p);
      // clear pointer.
      old_flow_context_p = NULL;
    }/* old_flow_context_p != NULL*/

    /* There is a potential race condition here that before NW process and deletes
       this request, iface might come back and might request. The race condition w
       will happen based on NW.
    */
  }/* while flow context_p*/       

  /*---------------------------------------------------------------------
    Step through all the flow contexts for NW flow context
  ---------------------------------------------------------------------*/  
  if(delete_nw_flow)
  {
    DS_3GPP_MSG0_HIGH("Del NW flow is requested");
    flow_type = (flow_type | DS_3GPP_NW_FLOW_TYPE);
    flow_context_p = list_peek_front(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->
                                       flow_context));
    while(ds_flow_context_validate_flow(flow_context_p))
    {
      old_flow_context_p = NULL;
      if(flow_context_p->ds_flow_context_dyn_p->flow_type == flow_type)
      {
        if(flow_context_p->ds_flow_context_dyn_p->state != 
           DS_FLOW_CONTEXT_STATE_UP)
        {
          DS_3GPP_MSG0_ERROR("Flow context is not in up state");
        }
        else
        {
          old_flow_context_p = flow_context_p;
        }
      } /* if flowtype matches*/
      flow_context_p = list_peek_next(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                                        ->flow_context),
                                &(flow_context_p->link));
      if(old_flow_context_p)
      {
      
        DS_3GPP_MSG1_HIGH("Del NW flow context 0x:%x",old_flow_context_p);
      
        /*-------------------------------------------------------------------
          Remove the flow context from bearer flow manager.
        ---------------------------------------------------------------------*/
        if(ds_bearer_flow_manager_remove_flow_context_from_flow_manager(
                  bflow_manager_p->ds_bearer_flow_manager_dyn_p->bearer_ptr,
                  old_flow_context_p) == FALSE)
        {
          DS_3GPP_MSG0_ERROR("Error in removing the pending context");
        }
        /*---------------------------------------------------------------------
          Delete the flow context
        ---------------------------------------------------------------------*/
        ds_flow_context_release_flow_context(old_flow_context_p);
        // clear pointer.
        old_flow_context_p = NULL;
      } /* old_flow_conext_p != NULL */
   }/* while loop */
  }/* delete_nw_flow*/

  /*---------------------------------------------------------------------
    Update TFT Length,other params
  ---------------------------------------------------------------------*/
  if(nw_flow_context_p != NULL)
  {
    nw_flow_context_p->ds_flow_context_dyn_p->tft_mod.valid = TRUE;
    nw_flow_context_p->ds_flow_context_dyn_p->tft_mod.tft_total_length =
             1 + nw_flow_context_p->ds_flow_context_dyn_p->tft_mod.num_filters;
    nw_flow_context_p->ds_flow_context_dyn_p->tft_mod.operation_code =
      (byte) DS_3GPP_TFT_OPCODE_DEL_FILTER;
    nw_flow_context_p->ds_flow_context_dyn_p->tft_mod.e_bit = 
      DS_FLOW_E_BIT_DISABLE;
    nw_flow_context_p->ds_flow_context_dyn_p->state = 
      DS_FLOW_CONTEXT_STATE_PEND_DEL_FLOW_RSP;

    if(list_size(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->flow_context))
        == 1)
    {
      DS_3GPP_MSG0_HIGH("Request bearer deletion");  
    }
  }
  return nw_flow_context_p;
}/*ds_bearer_flow_manager_iface_out_of_use */

/*===========================================================================
FUNCTION DS_3GPP_COVERT_TO_EPS_QOS

DESCRIPTION
3gpp spec ref: 23.401 Annex E  

PARAMETERS
 pre_rel8_qos_p           - input
 eps_qos_p                - output

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_convert_pre_rel8_to_eps_qos
(
  qos_T         *pre_rel8_qos_p,
  sdf_qos_T     *eps_qos_p
)
{  
  byte    temp_var   = 0x0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!DS_3GPP_QOS_IS_NETW_PRE_REL8_QOS_VALID(pre_rel8_qos_p))
  {
    DS_3GPP_MSG0_ERROR("Invalid Pre Rel8 qos !!");
    return FALSE;
  }

  if (eps_qos_p == NULL )
  {
    DS_3GPP_MSG0_ERROR("NULL eps qos pointer!!");
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    When pre-Rel-8 QoS parameters are mapped to EPS bearer QoS parameters the
    values of the pre-Rel-8 parameter SDU Error Ratio are ignored.        
  -------------------------------------------------------------------------*/  
  eps_qos_p->valid = TRUE;
  eps_qos_p->length = 1;
  switch(pre_rel8_qos_p->traffic_class)
  {
    case (byte)DS_UMTS_QOS_TCLASS_SUBSCRIBE:      
    {
      eps_qos_p->qci = (byte)DS_3GPP_LTE_QCI_0;
    }
    break;
    case (byte)DS_UMTS_QOS_TCLASS_CONV:      
    {
      /* speech = 0x1 */
      if(pre_rel8_qos_p->src_stat_desc == 0x1)
      {
        eps_qos_p->qci = (byte)DS_3GPP_LTE_QCI_1;
      }
      else
      {
        temp_var = (pre_rel8_qos_p->tranfer_delay >> 2) & 0x3F; /*Extract 6 leftmost */
        if( temp_var >= 0x0F )
        {
          eps_qos_p->qci = (byte)DS_3GPP_LTE_QCI_2;
        }
        else
        {
          eps_qos_p->qci = (byte)DS_3GPP_LTE_QCI_3;
        }
      }
    }
    break;
    case (byte)DS_UMTS_QOS_TCLASS_STRM:
    {
      eps_qos_p->qci = (byte)DS_3GPP_LTE_QCI_4;
    }
    break;
    case (byte)DS_UMTS_QOS_TCLASS_INTR:
    {
      if(pre_rel8_qos_p->traffic_priority == 0x1)
      {
        if(pre_rel8_qos_p->sig_ind == TRUE)
        {
          eps_qos_p->qci = (byte)DS_3GPP_LTE_QCI_5;
        }
        else
        {
          eps_qos_p->qci = (byte)DS_3GPP_LTE_QCI_6;
        }
      }
      else if(pre_rel8_qos_p->traffic_priority == 0x2)
      {
        eps_qos_p->qci = (byte)DS_3GPP_LTE_QCI_7;
      }
      else if(pre_rel8_qos_p->traffic_priority == 0x3) 
      {
        eps_qos_p->qci = (byte)DS_3GPP_LTE_QCI_8;
      }
    }
    break;
    case (byte)DS_UMTS_QOS_TCLASS_BACK:
    {
      eps_qos_p->qci = (byte)DS_3GPP_LTE_QCI_9;
    }
    break;
    default:
    {
      DS_3GPP_MSG1_HIGH("Invalid traffic_class:0x%x",pre_rel8_qos_p->traffic_class);
    }
  }// switch traffic_class
  DS_3GPP_MSG1_HIGH("ds_3gpp_convert_pre_rel8_to_eps_qos:QCI value set to:%d after moving to L", 
  	               eps_qos_p->qci );

  /*-------------------------------------------------------------------------
    Convert the bit rates. 
    The EPS bearer parameters GBR and MBR of a GBR EPS bearer are mapped 
    one-to-one to/from the pre-Rel-8 bearer parameters GBR and MBR of a PDP 
    context associated with Traffic class 'conversational' or 'streaming'.    
  -------------------------------------------------------------------------*/  

  eps_qos_p->gbr_ul = pre_rel8_qos_p->guarantee_ber_uplink; 
  eps_qos_p->gbr_dl = pre_rel8_qos_p->guarantee_ber_dwnlink; 
  eps_qos_p->max_bit_rate_ul = pre_rel8_qos_p->max_bit_rare_uplink;
  eps_qos_p->max_bit_rate_dl = pre_rel8_qos_p->max_bit_rare_downlink;

  eps_qos_p->gbr_ul_ex = pre_rel8_qos_p->ext_gtd_bit_rate_uplink;
  eps_qos_p->gbr_dl_ex = pre_rel8_qos_p->ext_gtd_bit_rate_downlink;
  eps_qos_p->max_bit_rate_ul_ex = pre_rel8_qos_p->ext_max_bit_rate_uplink;
  eps_qos_p->max_bit_rate_dl_ex = pre_rel8_qos_p->ext_max_bit_rate_downlink;

  eps_qos_p->gbr_ul_ex2 = pre_rel8_qos_p->ext2_gtd_bit_rate_uplink;
  eps_qos_p->gbr_dl_ex2 = pre_rel8_qos_p->ext2_gtd_bit_rate_downlink;
  eps_qos_p->max_bit_rate_ul_ex2 = pre_rel8_qos_p->ext2_max_bit_rate_uplink;
  eps_qos_p->max_bit_rate_dl_ex2 = pre_rel8_qos_p->ext2_max_bit_rate_downlink;

  // if any of the extended 2 bit is none zero, we need to include the extended 2 data
  if (eps_qos_p->max_bit_rate_dl_ex2 || eps_qos_p->gbr_dl_ex2 || 
      eps_qos_p->max_bit_rate_ul_ex2 || eps_qos_p->gbr_ul_ex2)
  {
    eps_qos_p->length = 13; //DS_EPS_QOS_QCI_LEN_FOR_EXT2_DATA
  }
  // if any of the extended bit is none zero, we need to include the extended data
  else if (eps_qos_p->max_bit_rate_dl_ex || eps_qos_p->gbr_dl_ex || 
      eps_qos_p->max_bit_rate_ul_ex || eps_qos_p->gbr_ul_ex)
  {
    eps_qos_p->length = 9; //DS_EPS_QOS_QCI_LEN_FOR_EXT_DATA
  }
  else if (eps_qos_p->max_bit_rate_dl || eps_qos_p->gbr_dl || 
      eps_qos_p->max_bit_rate_ul || eps_qos_p->gbr_ul)
  {
    eps_qos_p->length = 5; //DS_EPS_QOS_QCI_LEN_FOR_EXT_DATA
  }
  else
  {
    eps_qos_p->length = 1; //DS_EPS_QOS_QCI_LEN_FOR_DATA
  }

  return TRUE;
}

/*===========================================================================
FUNCTION DS_3GPP_COVERT_TO_PRE_REL8_QOS

DESCRIPTION
3gpp spec ref: 23.401 Annex E  

PARAMETERS
 eps_qos_p                - input
 pre_rel8_qos_p           - output

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates success or failure.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_convert_eps_to_pre_rel8_qos
(
  sdf_qos_T     *eps_qos_p,
  qos_T         *pre_rel8_qos_p
)
{  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if (!DS_3GPP_QOS_IS_NETW_EPS_QOS_VALID(eps_qos_p))
  {
    DS_3GPP_MSG0_ERROR("Invalid input EPS qos !!");
    return FALSE;
  }

  if (pre_rel8_qos_p == NULL )
  {
    DS_3GPP_MSG0_ERROR("NULL pre-rel8 qos pointer!!");
    return FALSE;
  }
  
  /*-------------------------------------------------------------------------
    3gpp spec ref: 23.401 Annex E   
    When EPS bearer QoS parameters are mapped to pre-Rel-8 QoS parameters the
    setting of the values of the pre-Rel-8 parameters Transfer Delay and SDU
    Error Ratio is derived from the corresponding QCI's Packet Delay Budget 
    and Packet Loss Rate, respectively. 
    3GPP TS 23.203 Table 6.1.7 - QCI-> pkt delay budget and pkt loss rate

    24.008: The Traffic handling priority value is ignored if the Traffic 
    Class is Conversational class, Streaming class or Background class.

    The Transfer delay value is ignored if the Traffic Class is Interactive
    class or Background class.
  -------------------------------------------------------------------------*/
  pre_rel8_qos_p->valid = TRUE;  
  switch(eps_qos_p->qci)
  {
    case (byte)DS_3GPP_LTE_QCI_0:      
    {
      pre_rel8_qos_p->traffic_class = (byte)DS_UMTS_QOS_TCLASS_SUBSCRIBE;
      pre_rel8_qos_p->src_stat_desc = 0x0;
    }
    break;
    case (byte)DS_3GPP_LTE_QCI_1:      
    {            
      pre_rel8_qos_p->traffic_class = (byte)DS_UMTS_QOS_TCLASS_CONV;
      /* speech = 0x1 */
      pre_rel8_qos_p->src_stat_desc = 0x1;
      /* 3GPP TS 23.203 Table 6.1.7 - 100ms */
      pre_rel8_qos_p->tranfer_delay = 0xA; 
      pre_rel8_qos_p->sdu_err_ratio = 0x1;
    }
    break;
    case (byte)DS_3GPP_LTE_QCI_2:      
    {            
      pre_rel8_qos_p->traffic_class = (byte)DS_UMTS_QOS_TCLASS_CONV;
      /* src_stat_desc = unknown = 0x0 */
      pre_rel8_qos_p->src_stat_desc = 0x0;
      /* tranfer_delay = 150ms */
      pre_rel8_qos_p->tranfer_delay = 0x0F;
      /* 3GPP TS 23.203 Table 6.1.7 */
      pre_rel8_qos_p->sdu_err_ratio = 0x3;
    }
    case (byte)DS_3GPP_LTE_QCI_3:      
    {                    
      pre_rel8_qos_p->traffic_class = (byte)DS_UMTS_QOS_TCLASS_CONV;
      /* src_stat_desc = unknown = 0x0 */
      pre_rel8_qos_p->src_stat_desc = 0x0;
      /* 
        VRK:TODO
        As per 3GPP TS 23.203 Table 6.1.7, QCI3 maps to transfer_delay 50  
      */      
      /* tranfer_delay = 80ms */
      pre_rel8_qos_p->tranfer_delay = 0x08;     
      /* 3GPP TS 23.203 Table 6.1.7 */
      pre_rel8_qos_p->sdu_err_ratio = 0x3;
    }
    break;
    case (byte)DS_3GPP_LTE_QCI_4:
    {
      pre_rel8_qos_p->traffic_class = (byte)DS_UMTS_QOS_TCLASS_STRM;
      /* src_stat_desc = unknown = 0x0 */
      pre_rel8_qos_p->src_stat_desc = 0x0;
      /* 3GPP TS 23.203 Table 6.1.7 - 300ms */
      pre_rel8_qos_p->tranfer_delay = 0x1E; 
      /* 3GPP TS 23.203 Table 6.1.7 */
      pre_rel8_qos_p->sdu_err_ratio = 0x6;
    }
    break;
    case (byte)DS_3GPP_LTE_QCI_5:
    {
      pre_rel8_qos_p->traffic_class = (byte)DS_UMTS_QOS_TCLASS_INTR;
      pre_rel8_qos_p->traffic_priority = 0x1;
      pre_rel8_qos_p->sig_ind = TRUE;     
      /* 3GPP TS 23.203 Table 6.1.7 */
      pre_rel8_qos_p->sdu_err_ratio = 0x6;
    }
    break;
    case (byte)DS_3GPP_LTE_QCI_6:
    {
      pre_rel8_qos_p->traffic_class = (byte)DS_UMTS_QOS_TCLASS_INTR;
      pre_rel8_qos_p->traffic_priority = 0x1;
      pre_rel8_qos_p->sig_ind = FALSE;
      /* 3GPP TS 23.203 Table 6.1.7 */
      pre_rel8_qos_p->sdu_err_ratio = 0x6;
    }
    break;
    case (byte)DS_3GPP_LTE_QCI_7:
    {
      pre_rel8_qos_p->traffic_class = (byte)DS_UMTS_QOS_TCLASS_INTR;
      pre_rel8_qos_p->traffic_priority = 0x2;
      pre_rel8_qos_p->sig_ind = FALSE;
      /* 3GPP TS 23.203 Table 6.1.7 */
      pre_rel8_qos_p->sdu_err_ratio = 0x3;
    }
    break;
    case (byte)DS_3GPP_LTE_QCI_8:
    {
      pre_rel8_qos_p->traffic_class = (byte)DS_UMTS_QOS_TCLASS_INTR;
      pre_rel8_qos_p->traffic_priority = 0x3;
      pre_rel8_qos_p->sig_ind = FALSE;
      /* 3GPP TS 23.203 Table 6.1.7 */
      pre_rel8_qos_p->sdu_err_ratio = 0x6;
    }
    break;     
    case (byte)DS_3GPP_LTE_QCI_9:
    {
      pre_rel8_qos_p->traffic_class = (byte)DS_UMTS_QOS_TCLASS_BACK;      
      /* 3GPP TS 23.203 Table 6.1.7 */
      pre_rel8_qos_p->sdu_err_ratio = 0x6;
    }
    break;
    default:
    {
      DS_3GPP_MSG1_HIGH("Invalid QCI:0x%x",eps_qos_p->qci);
    }
  }// switch traffic_class

  /*-------------------------------------------------------------------------
    Convert the bit rates. 
    The EPS bearer parameters GBR and MBR of a GBR EPS bearer are mapped 
    one-to-one to/from the pre-Rel-8 bearer parameters GBR and MBR of a PDP 
    context associated with Traffic class 'conversational' or 'streaming'.

    When EPS bearer QoS parameters are mapped to pre-Rel-8 QoS parameters 
    the pre-Rel-8 bearer parameter MBR of PDP contexts associated with 
    Traffic Class 'interactive' or 'background' is set equal to the value of
    the authorized APN-AMBR. The PGW shall enforce the APN-AMBR across all 
    PDP contexts with Traffic Class 'interactive' and 'background' for that 
    APN.

  -------------------------------------------------------------------------*/
  

  if(eps_qos_p->length == 13)  //DS_EPS_QOS_QCI_LEN_FOR_EXT2_DATA
  {  
    pre_rel8_qos_p->ext2_gtd_bit_rate_uplink = eps_qos_p->gbr_ul_ex2;
    pre_rel8_qos_p->ext2_gtd_bit_rate_downlink = eps_qos_p->gbr_dl_ex2;
    pre_rel8_qos_p->ext2_max_bit_rate_uplink = eps_qos_p->max_bit_rate_ul_ex2;
    pre_rel8_qos_p->ext2_max_bit_rate_downlink = eps_qos_p->max_bit_rate_dl_ex2;
  }
  else
  {
    pre_rel8_qos_p->ext2_gtd_bit_rate_uplink = 0;
    pre_rel8_qos_p->ext2_gtd_bit_rate_downlink = 0;
    pre_rel8_qos_p->ext2_max_bit_rate_uplink = 0;
    pre_rel8_qos_p->ext2_max_bit_rate_downlink = 0;
  }

  if(eps_qos_p->length == 9)  //DS_EPS_QOS_QCI_LEN_FOR_EXT_DATA
  {  
    pre_rel8_qos_p->ext_gtd_bit_rate_uplink = eps_qos_p->gbr_ul_ex;
    pre_rel8_qos_p->ext_gtd_bit_rate_downlink = eps_qos_p->gbr_dl_ex;
    pre_rel8_qos_p->ext_max_bit_rate_uplink = eps_qos_p->max_bit_rate_ul_ex;
    pre_rel8_qos_p->ext_max_bit_rate_downlink = eps_qos_p->max_bit_rate_dl_ex;
  }
  else
  {
    pre_rel8_qos_p->ext_gtd_bit_rate_uplink = 0;
    pre_rel8_qos_p->ext_gtd_bit_rate_downlink = 0;
    pre_rel8_qos_p->ext_max_bit_rate_uplink = 0;
    pre_rel8_qos_p->ext_max_bit_rate_downlink = 0;
  }

  if(eps_qos_p->length >= 5)  //
  {  
    pre_rel8_qos_p->guarantee_ber_uplink = eps_qos_p->gbr_ul; 
    pre_rel8_qos_p->guarantee_ber_dwnlink = eps_qos_p->gbr_dl; 
    pre_rel8_qos_p->max_bit_rare_uplink = eps_qos_p->max_bit_rate_ul;
    pre_rel8_qos_p->max_bit_rare_downlink = eps_qos_p->max_bit_rate_dl;
  }
  else
  {
    pre_rel8_qos_p->guarantee_ber_uplink = 0; 
    pre_rel8_qos_p->guarantee_ber_dwnlink = 0; 
    pre_rel8_qos_p->max_bit_rare_uplink = 0;
    pre_rel8_qos_p->max_bit_rare_downlink = 0;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_FIND_AND_SET_FILTER_ID

DESCRIPTION
  Returns the first available filter id value for flow manager. 

PARAMETERS
  flow_mgr_ptr     - Pointer to the flow manager context.
  lower_limit      - Assigned precedence should be >=  lower limit.
  upper_limit      - Assigned precedence should be <=  upper limit.
DEPENDENCIES
  None.

RETURN VALUE
  int           - Returns -1 if the precedence cannot be assigned.
  
SIDE EFFECTS
  None.

===========================================================================*/
int ds_bearer_flow_manager_find_and_set_filter_id
(
  uint32    *fi_id_p,
  uint16    lower_limit,
  uint16    upper_limit
)
{
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((lower_limit > DS_MAX_FI_PRECED) || (upper_limit > DS_MAX_FI_PRECED) ||
           (lower_limit > upper_limit))
  {
    DS_3GPP_MSG2_ERROR("Invalid filter id: lowerl 0x%x and Upperl 0x%x,ignored.",
              lower_limit,
              upper_limit);
    return -1;
  }

  if(fi_id_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input parameter is NULL");
    return -1;
  }  

  while (lower_limit <= upper_limit)
  {
    if(ds_3gpp_tft_is_filter_identifier_requested(
                                                 lower_limit,fi_id_p)==FALSE)
    {
      ds_3gpp_tft_set_filter_identifier_bit(lower_limit, fi_id_p);      
      return lower_limit;
    }
    lower_limit++;
  } /* while*/
  return -1;
} /* ds_bearer_flow_manager_find_and_set_filter_id */

/*===========================================================================
FUNCTION ds_bearer_flow_manager_validate_flow_manager

DESCRIPTION
  This function checks if the bearer flow manager tbl
  has been allocated dynamic memory

PARAMETERS
  flow_manager_p       - bearer_flow_manager to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_bearer_flow_manager_validate_flow_manager
(
  ds_bearer_flow_manager_s *bearer_flow_manager_p
)
{
  boolean ret_val = FALSE;
  ds_bearer_flow_manager_dyn_s *bearer_flow_manager_dyn_p = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(bearer_flow_manager_p == NULL)
  {
    DS_3GPP_MSG0_LOW("Bearer flow manager is NULL");
    return ret_val;
  }
  
  bearer_flow_manager_dyn_p = bearer_flow_manager_p->ds_bearer_flow_manager_dyn_p;

  if((bearer_flow_manager_dyn_p != NULL) &&
    (bearer_flow_manager_dyn_p->cookie == BEARER_FLOW_MANAGER_VALID_COOKIE))
  {
    DS_3GPP_MSG1_LOW("Bearer Flow Manager is valid,index:%d",
                     bearer_flow_manager_dyn_p->index);
    ret_val = TRUE;
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Bearer flow Mgr:0x%x is INVALID",bearer_flow_manager_p);
  }

  return ret_val;
}
/*===========================================================================
FUNCTION ds_flow_context_alloc_dyn_mem

DESCRIPTION
  This function allocates dyn memory.
 
PARAMETERS
  index        - index where the memory would be allocated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

=============================================================================*/
static boolean ds_bearer_flow_manager_alloc_dyn_mem
(
  int alloc_index
)
{
  boolean ret_val = FALSE;
  ds_bearer_flow_manager_dyn_s *bearer_flow_manager_dyn_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((alloc_index < 0) || (alloc_index >= DS_3GPP_MAX_BEARER_CONTEXT))
  {
    DS_3GPP_MSG1_ERROR("Index:%d is out of bounds",alloc_index);
    return ret_val;
  }

    bearer_flow_manager_dyn_p = ds_bearer_flow_manager_tbl[alloc_index].
                                                    ds_bearer_flow_manager_dyn_p;

    if(bearer_flow_manager_dyn_p == NULL)
    {
      bearer_flow_manager_dyn_p = (ds_bearer_flow_manager_dyn_s *)modem_mem_alloc
        (sizeof(ds_bearer_flow_manager_dyn_s),MODEM_MEM_CLIENT_DATA);

      if(bearer_flow_manager_dyn_p != NULL)
      {
        DS_3GPP_MSG1_MED("Bearer flow manager mem alloc successful:Index:%d",
                         alloc_index);
        memset(bearer_flow_manager_dyn_p,0,
                 sizeof(ds_bearer_flow_manager_dyn_s));
        bearer_flow_manager_dyn_p->cookie = BEARER_FLOW_MANAGER_VALID_COOKIE;
        ds_bearer_flow_manager_tbl[alloc_index].ds_bearer_flow_manager_dyn_p =  
          bearer_flow_manager_dyn_p;
        bearer_flow_manager_dyn_p->index = alloc_index;
        ret_val = TRUE;
      }
      else
      {
        DS_3GPP_MSG0_ERROR("Dyn mem allocation failed");
      }
    }
    else
    {
      DS_3GPP_MSG1_ERROR("Berar Flow Mgr Dyn memory has already been allocated "
                         "at index:%d",alloc_index);
    }


    return ret_val;
}


/*===========================================================================
FUNCTION ds_bearer_flow_manager_free_dyn_mem

DESCRIPTION
  This function frees dyn memory.
 
PARAMETERS
   NULL
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

=============================================================================*/
void ds_bearer_flow_manager_free_dyn_mem
(
   ds_bearer_flow_manager_s *bflow_manager_p
)
{
/*---------------------------------------------------------------------------*/
  if((bflow_manager_p == NULL) || 
     (bflow_manager_p->ds_bearer_flow_manager_dyn_p == NULL))
  {
    DS_3GPP_MSG0_ERROR("Bearer flow manager is invalid");
    return;
  }

  modem_mem_free(bflow_manager_p->ds_bearer_flow_manager_dyn_p,MODEM_MEM_CLIENT_DATA);
  bflow_manager_p->ds_bearer_flow_manager_dyn_p = NULL;
  DS_3GPP_MSG0_HIGH("Released the Bearer flow manager associated with Bearer");
}

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_GET_ALL_CIDS

DESCRIPTION
  This function returns the list of all flow cid's that are 
  currently active.
  
PARAMETERS   
  cid_list_ptr  - list of cid's which are active.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_flow_manager_get_all_cids
(
  sys_modem_as_id_e_type            subs_id,
  ds_3gpp_atif_flow_cid_list_type * cid_list_ptr
)
{
  uint8  bearer_idx;
  ds_bearer_flow_manager_dyn_s *bearer_flow_mgr_p;
  ds_flow_context_s *flow_context_p;
  sys_modem_as_id_e_type local_subs_id = SYS_MODEM_AS_ID_NONE;

  if (NULL == cid_list_ptr)
  {
    DS_3GPP_MSG0_ERROR("cid_list_ptr is NULL");
    return;
  }

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ;
  }
  
  for (bearer_idx = 0; 
       bearer_idx < DS_3GPP_MAX_BEARER_CONTEXT && 
       cid_list_ptr->data_len < DS_3GPP_MAX_FLOW_CONTEXT; 
       bearer_idx++)
  {
    if ( ds_bearer_flow_manager_validate_flow_manager(
                                   &(ds_bearer_flow_manager_tbl[bearer_idx])) )
    {
      if (DS_BEARERE_FLOW_MGR_IS_DEFAULT_BEARER(bearer_idx))
      {
        continue;
      }
      else
      {
        bearer_flow_mgr_p = 
           ds_bearer_flow_manager_tbl[bearer_idx].ds_bearer_flow_manager_dyn_p;

        if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_flow_mgr_p->bearer_ptr,
                       &local_subs_id))
        {
          continue;

        }
        /* update only if  subs_id matches, could be other subs_id than requested */
        if (local_subs_id == subs_id)
        {
          /* Retrieve only one context as all the 
            flow contexts are bound to same bearer */
          flow_context_p = list_peek_front(&(bearer_flow_mgr_p->flow_context));
          if (ds_flow_context_validate_flow(flow_context_p))
          {
            cid_list_ptr->data_ptr[cid_list_ptr->data_len++] = 
                           (flow_context_p->ds_flow_context_dyn_p->cid);
          }
        }
      }
    }
  }
}/* ds_bearer_flow_manager_get_all_cids */


/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_GET_ALL_CIDS_FOR_DDS

DESCRIPTION
  This function returns the list of all flow cid's that are 
  currently active.
  
PARAMETERS   
  cid_list_ptr  - list of cid's which are active.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_flow_manager_get_all_cids_for_dds
(
  ds_3gpp_atif_flow_cid_list_type * cid_list_ptr
)
{
  uint8  bearer_idx;
  ds_bearer_flow_manager_dyn_s *bearer_flow_mgr_p;
  ds_flow_context_s *flow_context_p;
  sys_modem_as_id_e_type     subs_id = ds3g_get_ps_subs_id();
  sys_modem_as_id_e_type local_subs_id = SYS_MODEM_AS_ID_NONE;

  if (NULL == cid_list_ptr)
  {
    DS_3GPP_MSG0_ERROR("cid_list_ptr is NULL");
    return;
  }

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ;
  }
  
  for (bearer_idx = 0; 
       bearer_idx < DS_3GPP_MAX_BEARER_CONTEXT && 
       cid_list_ptr->data_len < DS_3GPP_MAX_FLOW_CONTEXT; 
       bearer_idx++)
  {
    if ( ds_bearer_flow_manager_validate_flow_manager(
                                   &(ds_bearer_flow_manager_tbl[bearer_idx])) )
    {
      if (DS_BEARERE_FLOW_MGR_IS_DEFAULT_BEARER(bearer_idx))
      {
        continue;
      }
      else
      {
        bearer_flow_mgr_p = 
           ds_bearer_flow_manager_tbl[bearer_idx].ds_bearer_flow_manager_dyn_p;

        if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_flow_mgr_p->bearer_ptr,
                       &local_subs_id))
        {
          continue;

        }
        /* update only if  subs_id matches, could be other subs_id than requested */
        if (local_subs_id == subs_id)
        {
          /* Retrieve only one context as all the 
            flow contexts are bound to same bearer */
          flow_context_p = list_peek_front(&(bearer_flow_mgr_p->flow_context));
          if (ds_flow_context_validate_flow(flow_context_p))
          {
            cid_list_ptr->data_ptr[cid_list_ptr->data_len++] = 
                           (flow_context_p->ds_flow_context_dyn_p->cid);
          }
        }
      }
    }
  }
}/* ds_bearer_flow_manager_get_all_cids_for_dds */

/*===========================================================================
FUNCTION ds_bearer_flow_manager_convert_nw_r9798_qos_to_app_r99_qos

DESCRIPTION
  This function converts Release 97/98 into Release 99 specific QOS parameters.
  Octet 3 to Octet 5 of the QOS parameters in PDP context req are
  R97/98 qos parameters.

PARAMETERS
  *nw_alloc_qos - Ptr to NW QoS parameters for the current call
  *tx_flow - Ptr to tx flow Qos
  *rx_flow - Ptr to rx flow Qos


DEPENDENCIES
  For origination, assumes that the R99 parameters have been filled in 
  already in the orig_params_ptr

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_flow_manager_convert_nw_r9798_qos_to_app_r99_qos
(
  qos_T         *nw_alloc_qos,
  ip_flow_type  *tx_flow,
  ip_flow_type  *rx_flow
)
{
  uint32  max_bit_rate = QOS_BIT_RATE_SUBSCRIBED;
  int     sdu_exp_lookup[8] = { 0, 2, 3, 3, 4, 5, 6, 1 };
  int     sdu_mult_lookup[8] = { 0, 1, 7, 1, 1, 1, 1, 1 };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((nw_alloc_qos == NULL) || (tx_flow == NULL) || (rx_flow == NULL))
  {
    DS_3GPP_MSG0_ERROR("nw_r9798_qos_to_app_r99_qos: input params is NULL returning!!!");
    return;
  }
/* -------------------------------------------------------------------------
  Deduce R99 params from R97/R98 parameters based on mapping rules defined
  in TS23.107 Section 9.1.2.2 and fill in
  
  Table 6: Rules for determining R99 attributes from R97/98 attributes 
  ------------------------------------------------------------------------ 	
  Resulting R99 Attribute                    Derived from R97/98 Attribute 
  ------------------------------------------------------------------------	
  Name                   Value                  Value 		Name 
  ------------------------------------------------------------------------ 	
  Traffic class:      Interactive             1, 2, 3   :Delay class
                       Background             4  	
  ------------------------------------------------------------------------ 			   
  Traffic handling priority:  1                1       :Delay class
  			      2                2       :Delay class
  			      3                3       :Delay class							
  ------------------------------------------------------------------------
  SDU error ratio :    	10e-6		    1,2	 :Reliability class 
  			10e-4				 3	     
  			10e-3				 4,5	 
  ------------------------------------------------------------------------
  Residual bit error ratio: 10e-5              1, 2, 3, 4 :Reliability class 
    			    10e-4              5       
  ------------------------------------------------------------------------
  Maximum bitrate [kbps]: 8                     1      :Peak throughput class 			  
  			 16 	                2 		
  			 32 	                3 		
  			 64 		        4 		
  			 128 			5 		
  			 256 			6 		
  			 512 			7 		
  			 1024 			8 		
  			 2048 			9 		
------------------------------------------------------------------------*/

  if((nw_alloc_qos->delay_class >= QOS_DELAY_CLASS_1) &&
  	 (nw_alloc_qos->delay_class <= QOS_DELAY_CLASS_3))
  {
    tx_flow->trf_class  = IP_TRF_CLASS_INTERACTIVE;
    tx_flow->field_mask |= (int)IPFLOW_MASK_TRF_CLASS;
	
    rx_flow->trf_class  = IP_TRF_CLASS_INTERACTIVE;
    rx_flow->field_mask |= (int)IPFLOW_MASK_TRF_CLASS;
    switch(nw_alloc_qos->delay_class)
    {
     case QOS_DELAY_CLASS_1:
       tx_flow->umts_params.trf_pri =  UMTS_TRF_HANDLING_PRI1;
       tx_flow->field_mask |= (int)IPFLOW_MASK_UMTS_TRF_PRI;
  	
       rx_flow->umts_params.trf_pri =  UMTS_TRF_HANDLING_PRI1;
       rx_flow->field_mask |= (int)IPFLOW_MASK_UMTS_TRF_PRI;
	    break;
     case QOS_DELAY_CLASS_2:
       tx_flow->umts_params.trf_pri =  UMTS_TRF_HANDLING_PRI2;
       tx_flow->field_mask |= (int)IPFLOW_MASK_UMTS_TRF_PRI;
  	
       rx_flow->umts_params.trf_pri =  UMTS_TRF_HANDLING_PRI2;
       rx_flow->field_mask |= (int)IPFLOW_MASK_UMTS_TRF_PRI;
       break;
     case QOS_DELAY_CLASS_3:
       tx_flow->umts_params.trf_pri =  UMTS_TRF_HANDLING_PRI3;
       tx_flow->field_mask |= (int)IPFLOW_MASK_UMTS_TRF_PRI;
  	
       rx_flow->umts_params.trf_pri =  UMTS_TRF_HANDLING_PRI3;
       rx_flow->field_mask |= (int)IPFLOW_MASK_UMTS_TRF_PRI;
       break;
     default:
       DS_3GPP_MSG1_ERROR("ignoring traffic prioroty as traffic class is:",
                           tx_flow->trf_class );
       break;
    }	
  }
  else if(nw_alloc_qos->delay_class == QOS_DELAY_CLASS_4)
  {
    tx_flow->trf_class = IP_TRF_CLASS_BACKGROUND;	 
    tx_flow->field_mask |= (int)IPFLOW_MASK_TRF_CLASS;

    rx_flow->trf_class = IP_TRF_CLASS_BACKGROUND;	 
    rx_flow->field_mask |= (int)IPFLOW_MASK_TRF_CLASS;
	
	DS_3GPP_MSG1_ERROR("ignoring traffic prioroty as traffic class is:",
	tx_flow->trf_class );
  }

  if ((nw_alloc_qos->reliability_class == QOS_REL_CLASS_1)||
    	(nw_alloc_qos->reliability_class == QOS_REL_CLASS_2))
  {
    tx_flow->pkt_err_rate.multiplier  = 	
	(uint16)sdu_mult_lookup[DS_UMTS_QOS_SDU_ERR_1E6];
    tx_flow->pkt_err_rate.exponent = 
		(uint16)sdu_exp_lookup[DS_UMTS_QOS_SDU_ERR_1E6];
    tx_flow->field_mask |= (int)IPFLOW_MASK_PKT_ERR_RATE;
	
    rx_flow->pkt_err_rate.multiplier  = 	
	(uint16)sdu_mult_lookup[DS_UMTS_QOS_SDU_ERR_1E6];
    rx_flow->pkt_err_rate.exponent = 
		(uint16)sdu_exp_lookup[DS_UMTS_QOS_SDU_ERR_1E6];
    rx_flow->field_mask |= (int)IPFLOW_MASK_PKT_ERR_RATE;
  }
  else if (nw_alloc_qos->reliability_class == QOS_REL_CLASS_3 )
  {
  
    tx_flow->pkt_err_rate.multiplier  = 	
	(uint16)sdu_mult_lookup[DS_UMTS_QOS_SDU_ERR_1E4];
    tx_flow->pkt_err_rate.exponent = 
		(uint16)sdu_exp_lookup[DS_UMTS_QOS_SDU_ERR_1E4];
    tx_flow->field_mask |= (int)IPFLOW_MASK_PKT_ERR_RATE;
	
    rx_flow->pkt_err_rate.multiplier  = 	
	(uint16)sdu_mult_lookup[DS_UMTS_QOS_SDU_ERR_1E4];
    rx_flow->pkt_err_rate.exponent = 
		(uint16)sdu_exp_lookup[DS_UMTS_QOS_SDU_ERR_1E4];
    rx_flow->field_mask |= (int)IPFLOW_MASK_PKT_ERR_RATE;
  }
  else if ((nw_alloc_qos->reliability_class == QOS_REL_CLASS_4)||
    	(nw_alloc_qos->reliability_class == QOS_REL_CLASS_5))
  {
    tx_flow->pkt_err_rate.multiplier  = 	
	(uint16)sdu_mult_lookup[DS_UMTS_QOS_SDU_ERR_1E3];
    tx_flow->pkt_err_rate.exponent = 
		(uint16)sdu_exp_lookup[DS_UMTS_QOS_SDU_ERR_1E3];
    tx_flow->field_mask |= (int)IPFLOW_MASK_PKT_ERR_RATE;
	
    rx_flow->pkt_err_rate.multiplier  = 	
	(uint16)sdu_mult_lookup[DS_UMTS_QOS_SDU_ERR_1E3];
    rx_flow->pkt_err_rate.exponent = 
		(uint16)sdu_exp_lookup[DS_UMTS_QOS_SDU_ERR_1E3];
    rx_flow->field_mask |= (int)IPFLOW_MASK_PKT_ERR_RATE;
  }
  if ((nw_alloc_qos->reliability_class >= QOS_REL_CLASS_1)||
		  (nw_alloc_qos->reliability_class <= QOS_REL_CLASS_4))
  {
    tx_flow->umts_params.res_ber	= UMTS_RES_BIT_ERR_RATE7;	  
    tx_flow->field_mask |= (int)IPFLOW_MASK_UMTS_RES_BER;
	  
    rx_flow->umts_params.res_ber	= UMTS_RES_BIT_ERR_RATE7;
    rx_flow->field_mask |= (int)IPFLOW_MASK_UMTS_RES_BER;
  }
  else if(nw_alloc_qos->reliability_class == QOS_REL_CLASS_5)
  {
    tx_flow->umts_params.res_ber	= UMTS_RES_BIT_ERR_RATE4;	
    tx_flow->field_mask |= (int)IPFLOW_MASK_UMTS_RES_BER;

    rx_flow->umts_params.res_ber	= UMTS_RES_BIT_ERR_RATE4;	
    rx_flow->field_mask |= (int)IPFLOW_MASK_UMTS_RES_BER;
  }

  switch(nw_alloc_qos->peak_throughput)
  {
    case QOS_PEAK_TPUT_1KBYTES:
      max_bit_rate = QOS_BIT_RATE_8KBPS;
      break;
    case QOS_PEAK_TPUT_4KBYTES:	
      max_bit_rate = QOS_BIT_RATE_16KBPS;
      break;
    case QOS_PEAK_TPUT_8KBYTES:		
      max_bit_rate = QOS_BIT_RATE_32KBPS;		
      break;
    case QOS_PEAK_TPUT_16KBYTES:
      max_bit_rate = QOS_BIT_RATE_64KBPS;
      break;
    case QOS_PEAK_TPUT_32BYTES:
      max_bit_rate = QOS_BIT_RATE_128KBPS;
      break;
    case QOS_PEAK_TPUT_64KBYTES:
      max_bit_rate = QOS_BIT_RATE_512KBPS;
      break;
    case QOS_PEAK_TPUT_128KBYTES:
      max_bit_rate = QOS_BIT_RATE_1024KBPS;
      break;
    case QOS_PEAK_TPUT_256KBYTES:
      max_bit_rate = QOS_BIT_RATE_2048KBPS;
      break;
    default:
      max_bit_rate = QOS_BIT_RATE_SUBSCRIBED;
  }

  if(max_bit_rate != QOS_BIT_RATE_SUBSCRIBED)
  {
    tx_flow->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;
    tx_flow->data_rate.format.min_max.max_rate = max_bit_rate;
    tx_flow->field_mask |= (int)IPFLOW_MASK_DATA_RATE;
	
    rx_flow->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;
    rx_flow->data_rate.format.min_max.max_rate = max_bit_rate;
    rx_flow->field_mask |= (int)IPFLOW_MASK_DATA_RATE;
  }
  return;

} /* ds_bearer_flow_manager_convert_nw_r9798_qos_to_app_r99_qos */

#endif /* FEATURE_DATA_3GPP */

