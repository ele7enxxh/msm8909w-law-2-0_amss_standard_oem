/*===========================================================================

  Copyright (c) 2009-2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_flow_manager.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/14/13   ss      Fixed the issue of IPV6 address decoding.
10/05/11   ttv     Added DSDS changes for new mode handler.
04/13/11   ttv     Added changes for PS_IFACE_IOCTL_UMTS_GET_IM_CN_FLAG.
04/03/10   vk      Added support for MSG2.0 based logging
02/04/10   vd      Changed function naming format.
01/27/10   vd      Changed function naming format.
01/23/10   vk      Added helper macros for virtual function dispatch
01/20/10   vd      Changes to support UE Qos modification.
07/17/09   vd      Initial version.
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include <stringl/stringl.h>

#if defined(FEATURE_DATA_3GPP) 

#include "ds_3gpp_bearer_context.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_apn_table.h"
#include "dstask_v.h"
#include "ds3gmgr.h"
#include "ds_3gpp_flow_manager.h"
#include "ds_3gpp_bearer_flow_manager.h"
#include "ds_3gpp_flow_context.h"
#ifdef FEATURE_DATA_LTE
#include "ds_eps_flow_manager.h"
#include "ds_eps_bearer_flow_manager.h"
#include "ds_eps_bearer_context.h"
#endif
#include "ds_3gpp_tft.h"
#include "ds_3gppi_utils.h"
#include "ps_qos_defs.h"
#include "ps_ipfltr_defs.h"
#include "ps_in.h"
#include "ps_iface_defs.h"
#include "dsumtspdpreg.h"
#include "list.h"
#include "cm_gw.h"
#include "rex.h"
#include "modem_mem.h"
#ifdef DS_LTE_DISABLE
#ifdef FEATURE_DATA_WCDMA_PS
#include "ds_wcdma_bearer_context.h"
#endif
#endif /* DS_LTE_DISABLE */
#include "ds_flow_control.h"
#include "ran.h"
#include "ds_3gpp_qos.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_3gpp_dyn_mem_hdlr.h"
#include "ps_crit_sect.h"
#include "ps_iface.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                         GLOBAL VARIABLES

===========================================================================*/
/*-----------------------------------------------------------------------------
  Global critical section
-----------------------------------------------------------------------------*/
rex_crit_sect_type ds_3gpp_flow_crit_sect = {{NULL}};    


/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*----------------------------------------------------------------------------
  Flow Manager table. This table contains all the flow managers. This table
  is updated whenever a new flow manager is created or deleted.

  Each flow manager context is tied to a PDN context
----------------------------------------------------------------------------*/
ds_flow_manager_s ds_flow_manager_tbl[DS_3GPP_MAX_PDN_CONTEXT] = {{NULL}};

/*----------------------------------------------------------------------------
 Flow context table. This table contains all the flow contexts. The table is 
 updated whenever a new flow context is created or deleted.

----------------------------------------------------------------------------*/
ds_flow_context_s ds_flow_context_tbl[DS_3GPP_MAX_FLOW_CONTEXT] = {{{NULL}}};

extern ds_pdn_context_s ds_pdn_context_tbl[DS_3GPP_MAX_PDN_CONTEXT];

/*----------------------------------------------------------------------------
  EPS/umts specific function table We allocate memory for SYS_SYS_MODE_MAX
  table. However, we will be using only those corresponding to WCDMA, GPRS
  and EPS.
----------------------------------------------------------------------------*/
ds_3gpp_flow_manager_ftbl_s ds_flow_manager_ftbl[DS_3GPPI_SYS_MODE_MAX] = {{NULL}};

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

static boolean ds_flow_manager_alloc_dyn_mem
(
  int                    alloc_index,
  sys_modem_as_id_e_type subs_id
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
FUNCTION DS_FLOW_MANAGER_3GPP_INIT

DESCRIPTION
  Initialize the flow manager module.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_manager_3gpp_init()
{
  int index = 0;
  ds_flow_context_s *flow_cntx_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the flow context array to default values.
  -------------------------------------------------------------------------*/
  for(index=0; index<DS_3GPP_MAX_FLOW_CONTEXT;index++)
  {
    flow_cntx_p = &(ds_flow_context_tbl[index]);
	memset(flow_cntx_p, 0, sizeof(ds_flow_context_s));
	flow_cntx_p->sdf_id  = index +1 ;
  }

  return;
} /* ds_flow_manager_3gpp_init */


/*===========================================================================
FUNCTION DS_FLOW_MANAGER_IS_FILTER_PRECED_REQUESTED

DESCRIPTION
  Return whether this filter precedence is used for this flow manager context

PARAMETERS
  fi_preced     - Filter precedence.
  flow_mgr_ptr  - Pointer to the flow manager context.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - the bit for this filter precedence is set
  FALSE - otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_flow_manager_is_filter_preced_requested
(
  uint16 fi_preced,
  ds_flow_manager_s* flow_mgr_ptr
)
{
  uint32    *mask = NULL;
  uint32    *mask_req = NULL;
  boolean   result = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(fi_preced > DS_MAX_FI_PRECED)
  {
    DS_3GPP_MSG1_ERROR("Invalid filter precedence 0x%x received, ignored.",
              fi_preced);
    return FALSE;
  }

  if(!ds_flow_manager_validate_flow_manager(flow_mgr_ptr))
  {
    return FALSE;
  }

  mask = flow_mgr_ptr->ds_flow_manager_dyn_p->fi_preced;
  mask_req = flow_mgr_ptr->ds_flow_manager_dyn_p->req_fi_preced;
  
  result = (ds_3gpp_tft_is_filter_preced_bit_set(fi_preced,mask) || 
          ds_3gpp_tft_is_filter_preced_bit_set(fi_preced,mask_req));
  return result;
} /* ds_flow_manager_is_filter_preced_requested*/

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_FIND_AND_SET_FILTER_PRECED

DESCRIPTION
  Returns the first available preced value for flow manager. 

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
static int ds_flow_manager_find_and_set_filter_preced
(
  ds_flow_manager_s* flow_mgr_ptr,
  uint16    lower_limit,
  uint16    upper_limit
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((lower_limit > DS_MAX_FI_PRECED) || (upper_limit > DS_MAX_FI_PRECED) ||
           (lower_limit > upper_limit))
  {
    DS_3GPP_MSG2_ERROR("Invalid filter precedence: lowerl 0x%x and Upperl 0x%x,ignored.",
              lower_limit,
              upper_limit);
    return -1;
  }

  if(!ds_flow_manager_validate_flow_manager(flow_mgr_ptr))
  {
    return -1;
  }

  while (lower_limit <= upper_limit)
  {
    if(ds_flow_manager_is_filter_preced_requested(
                                    lower_limit,flow_mgr_ptr)==FALSE)
    {
      ds_3gpp_tft_set_filter_preced_bit(
                lower_limit, flow_mgr_ptr->ds_flow_manager_dyn_p->req_fi_preced);
      return lower_limit;
    }
    lower_limit++;
  } /* while*/
  return -1;
} /* ds_flow_manager_find_and_set_filter_preced */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_ALLOCATE_NEW_FLOW_MANAGER

DESCRIPTION
  Allocate a new flow manager for this PDN.

PARAMETERS
 pdn_context_p     - Pointer to PDN context information.

DEPENDENCIES
  None.

RETURN VALUE
 ds_flow_manager_s * - Pointer to empty flow manager.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_flow_manager_s * ds_flow_manager_allocate_new_flow_manager
( 
  ds_pdn_context_s *pdn_context_p
)
{
  int                    alloc_index = 0;
  ds_flow_manager_s      *flow_manager_p = NULL;
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p, &subs_id)== FALSE)
  {
    return flow_manager_p;
  }


  for (alloc_index = 0; alloc_index < DS_3GPP_MAX_PDN_CONTEXT; alloc_index++)
  {
    flow_manager_p = &ds_flow_manager_tbl[alloc_index];
    if(!ds_flow_manager_validate_flow_manager(flow_manager_p))
    {
      /*--------------------------------------------------------------------
        If flow_manager is allocated successfully assign a PDN context pointer 
        and initialize lists 
       --------------------------------------------------------------------*/
      if(!ds_flow_manager_alloc_dyn_mem(alloc_index, subs_id))
      {
        /* dynamic mem allocation failed, return NULL ;*/
        DS_3GPP_MSG0_ERROR("Dynamic memory alloc failed, return NULL");
        return NULL;
      }

      flow_manager_p->ds_flow_manager_dyn_p->pdn_ptr = pdn_context_p;
      list_init(&(flow_manager_p->ds_flow_manager_dyn_p->bearer_flow_mgr));   
      list_init(&(flow_manager_p->ds_flow_manager_dyn_p->pending_flow_cntx));    
      /*-----------------------------------------------------------------------
      Associate the mode specific blob that is going to be populated to the 
      bearer context pointer.
      -----------------------------------------------------------------------*/
#ifdef FEATURE_LTE
      DS_FLOW_MGR_VOBJ_REG
      (
        flow_manager_p->ds_flow_manager_dyn_p, 
        SYS_SYS_MODE_LTE,
        (void *)(&ds_eps_flow_manager_tbl[alloc_index])
      );
#endif

      /*---------------------------------------------------------------------
        Make sure the bearer flow  manager list is empty
      ---------------------------------------------------------------------*/
      if(list_size(&(flow_manager_p->ds_flow_manager_dyn_p->bearer_flow_mgr)) 
         > 0)
      {
        DS_3GPP_MSG1_ERROR("Flow manager has %d bearers in the list",
        list_size(&(flow_manager_p->ds_flow_manager_dyn_p->bearer_flow_mgr)));
        return NULL;
      }

      /*---------------------------------------------------------------------
        Make sure the pending flow context list is empty
      ---------------------------------------------------------------------*/
      if(list_size(&(flow_manager_p->ds_flow_manager_dyn_p->pending_flow_cntx))
          > 0)
      {
        DS_3GPP_MSG1_ERROR("Flow manager has %d flow contexts in the list",
        list_size(&(flow_manager_p->ds_flow_manager_dyn_p->bearer_flow_mgr)));
        return NULL;
      }

      break;
    }
  }

  if(alloc_index == DS_3GPP_MAX_PDN_CONTEXT)
  {
    DS_3GPP_MSG0_ERROR("MAX flow managers have been allocated");
  }

  return flow_manager_p;
} /* ds_flow_manager_allocate_new_flow_manager */

/*===========================================================================
FUNCTION DSFLOWMANAGER_RELEASELOWMANAGER

DESCRIPTION
  Release the flow manager associated with the PDN connection.

PARAMETERS
  pdn_context_p  - Pointer to the PDN context.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_manager_release_flow_manager
(
  ds_pdn_context_s *pdn_context_p
)
{
  int                          free_index = 0;
  ds_flow_manager_s            *flow_manager_p = NULL;
  ds_bearer_flow_manager_s     *bearer_flow_mgr_p = NULL;
  ds_bearer_flow_manager_s     *next_bflow_mgr = NULL;
  ds_flow_context_s            *flow_context_p = NULL;
  ds_flow_context_s            *next_pflow_cntxt = NULL;
  sys_modem_as_id_e_type        subs_id = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p, &subs_id)== FALSE)
  {
    return;
  }

  for (free_index = 0; free_index < DS_3GPP_MAX_PDN_CONTEXT; free_index++)
  {
    flow_manager_p = &(ds_flow_manager_tbl[free_index]);   

    if(ds_flow_manager_validate_flow_manager(flow_manager_p) && 
       (flow_manager_p->ds_flow_manager_dyn_p->pdn_ptr == pdn_context_p))
    {
      /*---------------------------------------------------------------------
        Empty the bearer flow manager list
      ---------------------------------------------------------------------*/          
      bearer_flow_mgr_p = list_peek_front(&(flow_manager_p->
                                      ds_flow_manager_dyn_p->bearer_flow_mgr));
      while(bearer_flow_mgr_p != NULL)
      {
        DS_3GPP_MSG0_HIGH("Bearer flow manager list is not empty");
        next_bflow_mgr = list_peek_next(&(flow_manager_p->ds_flow_manager_dyn_p
                         ->bearer_flow_mgr),&(bearer_flow_mgr_p->
                                              link));
        list_pop_item(&(flow_manager_p->ds_flow_manager_dyn_p->bearer_flow_mgr)
                      ,&(bearer_flow_mgr_p->link));
          bearer_flow_mgr_p = next_bflow_mgr;
          /** Do we need to clean up each bearer flow manager? */
      }

      /*---------------------------------------------------------------------
        Empty the pending flow context list
      ---------------------------------------------------------------------*/          
      flow_context_p = list_peek_front(&(flow_manager_p->ds_flow_manager_dyn_p
                                         ->pending_flow_cntx));
      while((flow_context_p != NULL) && (ds_flow_context_validate_flow(flow_context_p)))
      {
        DS_3GPP_MSG3_HIGH("Pending Flow context list is not empty",0,0,0);
        next_pflow_cntxt = list_peek_next(&(flow_manager_p->ds_flow_manager_dyn_p
                   ->pending_flow_cntx),&(flow_context_p->link));
        list_pop_item(&(flow_manager_p->ds_flow_manager_dyn_p->pending_flow_cntx),
                      &(flow_context_p->link));
        flow_context_p = next_pflow_cntxt;
          /** Do we need to clean up each flow context? */
      }

      /*--------------------------------------------------------------------- 
        Release the list mutexes
      ---------------------------------------------------------------------*/
      list_destroy(&(flow_manager_p->ds_flow_manager_dyn_p->bearer_flow_mgr));   
      list_destroy(&(flow_manager_p->ds_flow_manager_dyn_p->pending_flow_cntx));

      /*---------------------------------------------------------------------
        Release the dynamic memory
      ---------------------------------------------------------------------*/
      ds_3gpp_dyn_mem_hdlr_free_eps_flow_manager(free_index, subs_id);
      ds_flow_manager_free_dyn_mem(flow_manager_p);

      break;
    }
  } 
}/* ds_flow_manager_release_flow_manager */


/*===========================================================================
FUNCTION DS_FLOW_MANAGER_FIND_FLOW_MANAGER

DESCRIPTION
  Find the flow manager for this PDN.

PARAMETERS
 pdn_context_p     - Pointer to PDN context information.

DEPENDENCIES
  None.

RETURN VALUE
 ds_flow_manager_s * - Pointer to empty flow manager.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_flow_manager_s * ds_flow_manager_find_flow_manager
( 
  ds_pdn_context_s *pdn_context_p
)
{
  int index = 0;
  ds_flow_manager_s *flow_manager_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    return flow_manager_p;
  }

  for (index = 0; index < DS_3GPP_MAX_PDN_CONTEXT; index++)
  {
      flow_manager_p = &(ds_flow_manager_tbl[index]);
    if(ds_flow_manager_validate_flow_manager(flow_manager_p)&&(flow_manager_p
       ->ds_flow_manager_dyn_p->pdn_ptr == pdn_context_p))
    {
      break;
    }
  }
  if(index == DS_3GPP_MAX_PDN_CONTEXT)
  {
    flow_manager_p = NULL;
    DS_3GPP_MSG0_ERROR("Flow manager not found for PDN context");
  }
  return flow_manager_p;
} /* ds_flow_manager_find_flow_manager */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_ADD_BEARER_CONTEXT_TO_FLOW_MANAGER

DESCRIPTION
  Add a bearer context to the flow manager list.

PARAMETERS
 pdn_context_s        - Pointer to PDN Context.
 bearer_context_p     - Pointer to Bearer.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_add_bearer_context_to_flow_manager
( 
  ds_pdn_context_s   *pdn_context_p,
  ds_bearer_context_s *bearer_context_p
  
)
{
  ds_bearer_flow_manager_s *bflow_manager_p = NULL,*tbflow_manager_p = NULL; 
  ds_flow_manager_s *flow_manager_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }

  if(!ds_3gpp_pdn_cntx_validate_pdn_context
                                 (pdn_context_p))
  {
    return FALSE;
  }

  flow_manager_p = ds_flow_manager_find_flow_manager(pdn_context_p);

  if(!ds_flow_manager_validate_flow_manager(flow_manager_p))
  {
    return FALSE;
  }

  bflow_manager_p=ds_bearer_flow_manager_find_flow_manager(bearer_context_p);

  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return FALSE;
  }

  // Check to see if the bearer is already part of the list.
  tbflow_manager_p = list_peek_front(&(flow_manager_p->ds_flow_manager_dyn_p
                                       ->bearer_flow_mgr));

  while(ds_bearer_flow_manager_validate_flow_manager(tbflow_manager_p))
  {
    if(tbflow_manager_p == bflow_manager_p)
    {
      DS_3GPP_MSG0_ERROR("Bearer already present in flow manager list");
      return TRUE; //This is not an error.
    }

    tbflow_manager_p = list_peek_next(&(flow_manager_p->
         ds_flow_manager_dyn_p->bearer_flow_mgr),
        &(tbflow_manager_p->link));

  }

  list_push_front(&(flow_manager_p->ds_flow_manager_dyn_p->bearer_flow_mgr),
                       &(bflow_manager_p->link));
  return TRUE;
} /* ds_flow_manager_add_bearer_context_to_flow_manager */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_REMOVE_BEARER_CONTEXT_FROM_FLOW_MANAGER

DESCRIPTION
  Remove a bearer context from the flow manager list.

PARAMETERS
 pdn_context_s       - Pointer to PDN Contex.
 bearer_context_p     - Pointer to Bearer.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_remove_bearer_context_from_flow_manager
( 
    ds_pdn_context_s   *pdn_context_p,
    ds_bearer_context_s *bearer_context_p
)
{
  ds_bearer_flow_manager_s *bflow_manager_p = NULL,*tbflow_manager_p = NULL;
  ds_flow_manager_s        *flow_manager_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(bearer_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input Bearer Pointer is NULL");
    return FALSE;
  }

  if(pdn_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input PDN context Pointer is NULL");
    return FALSE;
  }

  flow_manager_p = ds_flow_manager_find_flow_manager(pdn_context_p);

  if(!ds_flow_manager_validate_flow_manager(flow_manager_p))
  {
    return FALSE;
  }

  bflow_manager_p=ds_bearer_flow_manager_find_flow_manager(bearer_context_p);

  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return FALSE;
  }  
  // Check to see if the bearer is part of the list.
  tbflow_manager_p = list_peek_front(&(flow_manager_p->ds_flow_manager_dyn_p
                                       ->bearer_flow_mgr));
  while(tbflow_manager_p != NULL)
  {
    if(tbflow_manager_p == bflow_manager_p)
    {
      break;
    }
    tbflow_manager_p = list_peek_next(&(flow_manager_p->ds_flow_manager_dyn_p
     ->bearer_flow_mgr),&(tbflow_manager_p->link));
  }
  if(tbflow_manager_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Bearer not present in flow manager list");
    return FALSE;  
  }
  list_pop_item(&(flow_manager_p->ds_flow_manager_dyn_p->bearer_flow_mgr),
                        &(bflow_manager_p->link));
  return TRUE;
} /* ds_flow_manager_remove_bearer_context_from_flow_manager */


/*===========================================================================
FUNCTION DS_FLOW_MANAGER_ADD_PENDING_FLOW_CONTEXT

DESCRIPTION
  Add a flow context to the pending flow context list.

PARAMETERS
 pdn_context_s        - Pointer to PDN Context.
 flow_context_p       - Pointer to Flow Context.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_add_pending_flow_context
( 
  ds_pdn_context_s   *pdn_context_p,
  ds_flow_context_s  *flow_context_p
  
)
{
  ds_flow_manager_s *flow_manager_p = NULL;
  ds_flow_context_s *tflow_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(flow_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input FLOW Pointer is NULL");
    return FALSE;
  }

  if(pdn_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input PDN context Pointer is NULL");
    return FALSE;
  }

  flow_manager_p = ds_flow_manager_find_flow_manager(pdn_context_p);
  if(!ds_flow_manager_validate_flow_manager(flow_manager_p))
  {
    return FALSE;
  }

  // Check if flow context is already present in the list.
  tflow_context_p = list_peek_front(&(flow_manager_p->ds_flow_manager_dyn_p
                                      ->pending_flow_cntx));
  while(ds_flow_context_validate_flow(tflow_context_p))
  {
    if(tflow_context_p == flow_context_p)
    {
      DS_3GPP_MSG0_ERROR("Flow already present in pdn context list");
      return FALSE;
    }
    tflow_context_p = list_peek_next(&(flow_manager_p->ds_flow_manager_dyn_p
               ->pending_flow_cntx),&(tflow_context_p->link));
  }
  list_push_front(&(flow_manager_p->ds_flow_manager_dyn_p->pending_flow_cntx),
                          &(flow_context_p->link));
  return TRUE;
} /* ds_flow_manager_add_pending_flow_context */


/*===========================================================================
FUNCTION DS_FLOW_MANAGER_REMOVE_PENDING_FLOW_CONTEXT

DESCRIPTION
  Remove a pending flow context from the flow manager list.

PARAMETERS
 pdn_context_s       - Pointer to PDN Contex.
 flow_context_p     - Pointer to Bearer.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_remove_pending_flow_context
( 
    ds_pdn_context_s   *pdn_context_p,
    ds_flow_context_s  *flow_context_p
)
{
  ds_flow_manager_s        *flow_manager_p = NULL;
  ds_flow_context_s        *tflow_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(flow_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input Flow Context Pointer is NULL");
    return FALSE;
  }

  if(pdn_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input PDN context Pointer is NULL");
    return FALSE;
  }

  flow_manager_p = ds_flow_manager_find_flow_manager(pdn_context_p);
  if(!ds_flow_manager_validate_flow_manager(flow_manager_p))
  {
    return FALSE;
  }
  // Check if flow context is present in the list.
  tflow_context_p = list_peek_front(&(flow_manager_p->
                                ds_flow_manager_dyn_p->pending_flow_cntx));
  while(ds_flow_context_validate_flow(flow_context_p))
  {
    if(tflow_context_p == flow_context_p)
    {
      break;
    }
    tflow_context_p = list_peek_next(&(flow_manager_p->ds_flow_manager_dyn_p
                 ->pending_flow_cntx),&(tflow_context_p->link));
  }
  if(tflow_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Flow not present in pending flow context list");
    return FALSE;
  }
  list_pop_item(&(flow_manager_p->ds_flow_manager_dyn_p->pending_flow_cntx),
                           &(flow_context_p->link));
  return TRUE;
} /* ds_flow_manager_remove_pending_flow_context*/

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_UPDATE_PRECEDENCE_BIT_MASK

DESCRIPTION
  Update precednce bit mask for the given flow context.

PARAMETERS
 pdn_context_s       - Pointer to PDN Context.
 flow_context_p      - Pointer to flow context.
 req_modify          - boolean. If true, request from modify filter set.
 set_precedence      - boolean. If true, set the bit else clear the bit.
 req_precedence      - boolean. If true, update requested precedence else
                       update precedence bit mask.  

DEPENDENCIES
  None.

RETURN VALUE
 boolean             - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_update_precedence_bit_mask
( 
  ds_pdn_context_s   *pdn_context_p,
  ds_flow_context_s  *flow_context_p,
  boolean            req_modify,
  boolean            set_precedence,
  boolean            req_precedence     
)
{
  ds_flow_manager_s        *flow_manager_p = NULL;
  qos_spec_type            orig_spec;  
  ip_filter_type           *fltr = NULL; 
  int                      cnt = 0;          
  uint16                   preced = 0;
  uint16                   identifier = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return FALSE;
  }

  if(pdn_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input PDN context Pointer is NULL");
    return FALSE;
  }

  flow_manager_p = ds_flow_manager_find_flow_manager(pdn_context_p);
  if(!ds_flow_manager_validate_flow_manager
                                  (flow_manager_p))
  {
    DS_3GPP_MSG0_ERROR("No Flow manager associated with pdn");
    return FALSE;
  }

  memset(&orig_spec, 0, sizeof(qos_spec_type));

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
                                    req_modify,
                                    &orig_spec) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Error retrieving original filter params");
    modem_mem_free((void *)orig_spec.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    modem_mem_free((void *)orig_spec.tx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }
 
  /*------------------------------------------------------------------------
     Loop thru all filters and update the precedence bit mask for RX
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt<orig_spec.rx.fltr_template.num_filters; cnt++)
  {
    fltr = &(orig_spec.rx.fltr_template.list_ptr[cnt]);
    preced = fltr->ipfltr_aux_info.fi_precedence;
    identifier = (fltr->ipfltr_aux_info.fi_id & 0xf);
    if(ds_3gpp_tft_is_filter_identifier_requested(
                                      identifier,
      &(flow_context_p->ds_flow_context_dyn_p->ignore_fi_id))==TRUE)
    {
      continue;
    }
    if(set_precedence)
    {
      if(req_precedence)
      {
        /*-------------------------------------------------------------------
           Set the Requested FI Preced for Rx
        -------------------------------------------------------------------*/
        ds_3gpp_tft_set_filter_preced_bit(
                  preced,flow_manager_p->ds_flow_manager_dyn_p->req_fi_preced);  
      }
      else
      {
        /*-------------------------------------------------------------------
           Set the FI Preced for Rx
        -------------------------------------------------------------------*/
        ds_3gpp_tft_set_filter_preced_bit(
                      preced,flow_manager_p->ds_flow_manager_dyn_p->fi_preced);  
      }
    }
    else
    {
      if(req_precedence)
      {
        /*-------------------------------------------------------------------
           Clear the Requested FI Preced for Rx
        -------------------------------------------------------------------*/
        ds_3gpp_tft_clear_filter_preced_bit(
                   preced,flow_manager_p->ds_flow_manager_dyn_p->req_fi_preced);  
      }
      else
      {
        /*-------------------------------------------------------------------
           Clear the FI Preced for Rx
        -------------------------------------------------------------------*/
        ds_3gpp_tft_clear_filter_preced_bit(
                      preced,flow_manager_p->ds_flow_manager_dyn_p->fi_preced);  
      }
    }
  }/* RX FOR Loop*/
  /*------------------------------------------------------------------------
     Loop thru all filters and update the precedence bit mask for TX
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt<orig_spec.tx.fltr_template.num_filters; cnt++)
  {
    fltr = &(orig_spec.tx.fltr_template.list_ptr[cnt]);
    preced = fltr->ipfltr_aux_info.fi_precedence;
    identifier = (fltr->ipfltr_aux_info.fi_id & 0xf);
    if(ds_3gpp_tft_is_filter_identifier_requested(
                                      identifier,
      &(flow_context_p->ds_flow_context_dyn_p->ignore_fi_id))==TRUE)
    {
      continue;
    }
    if(set_precedence)
    {
      if(req_precedence)
      {
        /*-------------------------------------------------------------------
           Set the Requested FI Preced for Tx
        -------------------------------------------------------------------*/
        ds_3gpp_tft_set_filter_preced_bit(
                 preced,flow_manager_p->ds_flow_manager_dyn_p->req_fi_preced);  
      }
      else
      {
        /*-------------------------------------------------------------------
           Set the FI Preced for Tx
        -------------------------------------------------------------------*/
        ds_3gpp_tft_set_filter_preced_bit(
                      preced,flow_manager_p->ds_flow_manager_dyn_p->fi_preced);  
      }
    }
    else
    {
      if(req_precedence)
      {
        /*-------------------------------------------------------------------
           Clear the Requested FI Preced for Tx
        -------------------------------------------------------------------*/
        ds_3gpp_tft_clear_filter_preced_bit(
                 preced,flow_manager_p->ds_flow_manager_dyn_p->req_fi_preced);  
      }
      else
      {
        /*-------------------------------------------------------------------
           Clear the FI Preced for Tx
        -------------------------------------------------------------------*/
        ds_3gpp_tft_clear_filter_preced_bit(
                     preced,flow_manager_p->ds_flow_manager_dyn_p->fi_preced);  
      }
    }
  }/* FOR TX  Filters */

  modem_mem_free((void *)orig_spec.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
  modem_mem_free((void *)orig_spec.tx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
  return TRUE; 
}/* ds_flow_manager_update_precedence_bit_mask*/

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_FIND_PENDING_FLOW_CONTEXT_FOR_PDN

DESCRIPTION
  Return a pending flow context based on SDF_ID.

PARAMETERS
 pdn_context_s       - Pointer to PDN Contex.
 sdf_id              -  SDF ID to find.

DEPENDENCIES
  None.

RETURN VALUE
 ds_flow_context_s * - Return valid pointer if flow context exists. Else NULL.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_flow_context_s *ds_flow_manager_find_pending_flow_context_for_pdn
( 
  ds_pdn_context_s   *pdn_context_p,
  uint16             sdf_id
)
{
  ds_flow_manager_s        *flow_manager_p = NULL;
  ds_flow_context_s        *flow_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(pdn_context_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input PDN context Pointer is NULL");
    return FALSE;
  }
  flow_manager_p = ds_flow_manager_find_flow_manager(pdn_context_p);
  if(!ds_flow_manager_validate_flow_manager(flow_manager_p))
  {
    return FALSE;
  }
  /*---------------------------------------------------------------------
    Find the pending flow context list
  ---------------------------------------------------------------------*/          
  flow_context_p = list_peek_front(&(flow_manager_p->ds_flow_manager_dyn_p
                                     ->pending_flow_cntx));
  while(ds_flow_context_validate_flow(flow_context_p))
  {
    if(flow_context_p->sdf_id == sdf_id)
    {
      DS_3GPP_MSG1_HIGH("Found flow_context for SDF_ID 0x%x",sdf_id);
      return flow_context_p;
    }
    flow_context_p = list_peek_next(&(flow_manager_p->ds_flow_manager_dyn_p
                ->pending_flow_cntx),&(flow_context_p->link));
  } /* while*/
  DS_3GPP_MSG1_HIGH("Flow Context not found for SDF_ID 0x%x",sdf_id);
  return flow_context_p;
}/* ds_flow_manager_find_pending_flow_context_for_pdn */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_FIND_PENDING_FLOW_CONTEXT

DESCRIPTION
  Return a pending flow context based on SDF_ID.

PARAMETERS
 pdn_cntx_p       -  Output parameter. The pointer is updated, if flow
                        context is found.
 sdf_id              -  SDF ID to find.

DEPENDENCIES
  None.

RETURN VALUE
 ds_flow_context_s * - Return valid pointer if flow context exists. Else NULL.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_flow_context_s *ds_flow_manager_find_pending_flow_context
( 
  ds_pdn_context_s   **pdn_cntx_p,
  uint16             sdf_id
)
{
  ds_flow_manager_s        *flow_manager_p = NULL;
  ds_flow_context_s        *flow_context_p = NULL;
  ds_pdn_context_s         *pdn_context_p  = NULL;  
  int i = 0;  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for (i =0; i < DS_3GPP_MAX_PDN_CONTEXT; i++)
  {
      flow_manager_p = &(ds_flow_manager_tbl[i]);
    if(ds_flow_manager_validate_flow_manager(flow_manager_p))
    {
      pdn_context_p = flow_manager_p->ds_flow_manager_dyn_p->pdn_ptr; 
      if(pdn_context_p == NULL)
      {
        DS_3GPP_MSG0_ERROR("Input PDN context Pointer is NULL");
        return NULL;
      }
      flow_context_p = ds_flow_manager_find_pending_flow_context_for_pdn(
                                   pdn_context_p,
                                   sdf_id);
      if(ds_flow_context_validate_flow(flow_context_p))
      {
        *pdn_cntx_p = pdn_context_p;
        return flow_context_p;
      }
    } /* if ds_flow_manager_tbl[i]*/
  } /* for*/
  return NULL;
}/* ds_flow_manager_find_pending_flow_context*/

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_VERIFY_APP_FILTER_PRECEDENCE

DESCRIPTION
  Verify the app did not pass invalid precedence value.
  This function will also assign the precedence value.

PARAMETERS
 pdn_context_s       - Pointer to PDN Context.
 input_qos           - Pointer to App passed Qos.
 check_default       - boolean indicating to verify preced is default.
 filter_op           - enum indicating what filters to check.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_verify_app_filter_precedence
( 
  ds_pdn_context_s   *pdn_context_p,
  qos_spec_type      *input_qos,
  boolean            check_default,
  ds_3gpp_app_filter_op filter_op
)
{
  ip_filter_type      *inp_filter  = NULL;  
  int                 num_filters  = 0;
  int                 cnt = 0, inc=0;
  int                 fi_precedence = -1;
  ds_flow_manager_s   *flow_manager_p = NULL; 
  uint32              temp_preced[DS_FI_PRECED_ARR_SIZE]; 
  boolean             preced_failed = FALSE;
 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (input_qos==NULL) || ((!check_default) && (pdn_context_p == NULL) ))
  {
    DS_3GPP_MSG0_ERROR("Input parameter is NULL");
    return FALSE;
  }

  if(check_default)
  {
    if(filter_op & DS_3GPP_APP_RX_FLTR_OP)
    {
      /*---------------------------------------------------------------------
        Check for RX filters.
      ---------------------------------------------------------------------*/    
      num_filters = input_qos->rx.fltr_template.num_filters;
      for(cnt =0; cnt< num_filters; cnt++)
      {
        inp_filter = &(input_qos->rx.fltr_template.list_ptr[cnt]);
        if(inp_filter->ipfltr_aux_info.fi_precedence!=DS_DEF_FILTER_PRECD)
        {
          DS_3GPP_MSG1_ERROR("RX Filter preced not set to defualt for %d"
                                                               ,cnt);
          return FALSE;
        }
      }
    }/* filter_op & DS_3GPP_APP_RX_FLTR_OP */

    if(filter_op & DS_3GPP_APP_TX_FLTR_OP)
    {
      /*---------------------------------------------------------------------
        Check for TX filters.
      ---------------------------------------------------------------------*/    
      num_filters = input_qos->tx.fltr_template.num_filters;
      for(cnt =0; cnt< num_filters; cnt++)
      {
        inp_filter = &(input_qos->tx.fltr_template.list_ptr[cnt]);
        if(inp_filter->ipfltr_aux_info.fi_precedence!=DS_DEF_FILTER_PRECD)
        {
          DS_3GPP_MSG1_ERROR("TX Filter preced not set to defualt for %d",
                                                                   cnt);
          return FALSE;
        }
      }
    }/* filter_op & DS_3GPP_APP_TX_FLTR_OP */  
    return TRUE;
  }/* check_default == TRUE*/

  /*-----------------------------------------------------------------------
    Find Flow Manager associated with PDN
  -----------------------------------------------------------------------*/    
  flow_manager_p = ds_flow_manager_find_flow_manager(pdn_context_p);
  if(!ds_flow_manager_validate_flow_manager(flow_manager_p))
  {
    return FALSE;
  }

  /*-----------------------------------------------------------------------
    Keep track of the precedence being allocated
  -----------------------------------------------------------------------*/  
  memset(temp_preced, 0, (sizeof(uint32) *DS_FI_PRECED_ARR_SIZE));
  
  if(filter_op & DS_3GPP_APP_RX_FLTR_OP)
  {
    /*-----------------------------------------------------------------------
      Check for RX filters.
    -----------------------------------------------------------------------*/    
    num_filters = input_qos->rx.fltr_template.num_filters;
    for(cnt =0; cnt< num_filters; cnt++)
    {
      inp_filter = &(input_qos->rx.fltr_template.list_ptr[cnt]);
      if(inp_filter->ipfltr_aux_info.fi_precedence!=DS_DEF_FILTER_PRECD)
      {
        /*-------------------------------------------------------------------
         Check if Precedence already allocated
        -------------------------------------------------------------------*/    
        if(ds_flow_manager_is_filter_preced_requested(
              inp_filter->ipfltr_aux_info.fi_precedence,
              flow_manager_p) == TRUE)
        {
          DS_3GPP_MSG0_ERROR("Filter precedence already allocated");
          preced_failed = TRUE;
          break;
        }
        /*-------------------------------------------------------------------
          Update the Preced Bit
        -------------------------------------------------------------------*/ 
        ds_3gpp_tft_set_filter_preced_bit(
              inp_filter->ipfltr_aux_info.fi_precedence,
              flow_manager_p->ds_flow_manager_dyn_p->req_fi_preced);
        /*-------------------------------------------------------------------
           Keep track of the precedence being allocated
        --------------------------------------------------------------------*/
        fi_precedence = inp_filter->ipfltr_aux_info.fi_precedence;
        ds_3gpp_tft_set_filter_preced_bit(fi_precedence, temp_preced);
        
      }/* Fi_precedence != FILTER_PRECEDENCE */
      else
      {
        if((fi_precedence = ds_flow_manager_find_and_set_filter_preced(
                                      flow_manager_p, 
                                      DS_MIN_FI_PRECED,
                                      DS_MAX_FI_PRECED)) == -1)
        {
          DS_3GPP_MSG1_ERROR("Invalid filter precedence 0x%x for RX",cnt);
          inp_filter->ipfltr_aux_info.fi_precedence = DS_DEF_FILTER_PRECD;
          preced_failed = TRUE;
          break;
        }  
        else
        {
          inp_filter->ipfltr_aux_info.fi_precedence = fi_precedence;
          /*-----------------------------------------------------------------
            Keep track of the precedence being allocated
           ----------------------------------------------------------------*/
          ds_3gpp_tft_set_filter_preced_bit(fi_precedence, temp_preced);
        }/* findAndSetFilterPreced != -1*/
      } /* else */
    } /* for cnt */
  }/* if & DS_3GPP_APP_RX_FLTR_OP */

  /*-----------------------------------------------------------------------
    Check for TX filters.
  -----------------------------------------------------------------------*/ 
  if(filter_op & DS_3GPP_APP_TX_FLTR_OP)
  {
    num_filters = input_qos->tx.fltr_template.num_filters;
    for(cnt =0; cnt< num_filters; cnt++)
    {
      inp_filter = &(input_qos->tx.fltr_template.list_ptr[cnt]);
      if(inp_filter->ipfltr_aux_info.fi_precedence != DS_DEF_FILTER_PRECD)
      {
        /*-------------------------------------------------------------------
         Check if Precedence already allocated
        -------------------------------------------------------------------*/    
        if(ds_flow_manager_is_filter_preced_requested(
             inp_filter->ipfltr_aux_info.fi_precedence,
             flow_manager_p) == TRUE)
        {
          DS_3GPP_MSG0_ERROR("Filter precedence already allocated");
          preced_failed = TRUE;
          break;
        }
        /*-------------------------------------------------------------------
         Update the Preced Bit
        -------------------------------------------------------------------*/ 
        ds_3gpp_tft_set_filter_preced_bit(
              inp_filter->ipfltr_aux_info.fi_precedence,
              flow_manager_p->ds_flow_manager_dyn_p->req_fi_preced);   
        /*-------------------------------------------------------------------
          Keep track of the precedence being allocated
        --------------------------------------------------------------------*/
        ds_3gpp_tft_set_filter_preced_bit(fi_precedence, temp_preced);   
      }/* Fi_precedence != FILTER_PRECEDENCE */
      else
      {
        if((fi_precedence = ds_flow_manager_find_and_set_filter_preced(
                                      flow_manager_p, 
                                      DS_MIN_FI_PRECED,
                                      DS_MAX_FI_PRECED)) == -1)
        {
          DS_3GPP_MSG1_ERROR("Invalid filter precedence :%d for RX",cnt);
          inp_filter->ipfltr_aux_info.fi_precedence = DS_DEF_FILTER_PRECD;
          preced_failed = TRUE;
          break;
        }
        else
        {
          inp_filter->ipfltr_aux_info.fi_precedence = fi_precedence;
          /*-----------------------------------------------------------------
            Keep track of the precedence being allocated
           ----------------------------------------------------------------*/
          ds_3gpp_tft_set_filter_preced_bit(fi_precedence, temp_preced);
        }/* findAndSetFilterPreced != -1*/
      }/* else */
    } /* for */
  }/* if & DS_3GPP_APP_TX_FLTR_OP */
  
  if(preced_failed == TRUE)
  {
    /*-----------------------------------------------------------------
      Precedence allocation failed. Clean up the allocated values      
    ----------------------------------------------------------------*/
    for(cnt=0; cnt< DS_FI_PRECED_ARR_SIZE; cnt++)
    {
      inc = 0;
      while(inc < 32)
      {
        if( (temp_preced[cnt]&(1 << inc)) == (1 << inc))
        {
          fi_precedence = (cnt * 32)|(1 << inc); 
          ds_3gpp_tft_clear_filter_preced_bit(fi_precedence, 
                     flow_manager_p->ds_flow_manager_dyn_p->req_fi_preced);
        } /* if */
        inc++;
      } /* while */
    }/* for */
  }
  return TRUE;
}/* ds_flow_manager_verify_app_filter_precedence */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_VALIDATE_APP_QOS_FILTER

DESCRIPTION
  Verify the app did not pass invalid src/dest IP address

PARAMETERS
 input_filter           - Pointer to filter.
 flow_type              - Mask indicating the flow type

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_validate_app_qos_filter
(
  ip_filter_type                  *inp_filter,
  qos_spec_field_mask_enum_type    flow_type
)
{
  uint8               temp = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(inp_filter == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input parameter is NULL");
    return FALSE;
  }
  if ((flow_type & QOS_MASK_RX_FLOW) ||
      (flow_type & QOS_MODIFY_MASK_RX_FLTR_MODIFY) )
  {
    DS_3GPP_MSG0_HIGH("Filter validation for RX");
    if(inp_filter->ip_vsn == IP_V4)
    {
      temp = (uint8)
        (inp_filter->ip_hdr.v4.field_mask &((uint8)IPFLTR_MASK_IP4_DST_ADDR));
      if(temp != 0)
      {
        DS_3GPP_MSG0_ERROR("DST_ADDR mask set for DL filter");
        return FALSE;
      }
    }
    else if (inp_filter->ip_vsn == IP_V6)
    {
      temp=(uint8)
       (inp_filter->ip_hdr.v6.field_mask &((uint8)IPFLTR_MASK_IP6_DST_ADDR));
      if(temp != 0)
      {
        DS_3GPP_MSG0_ERROR("DST_ADDR mask set for DL filter");
        return FALSE;
      }
    }
    else
    {
      DS_3GPP_MSG1_ERROR("Invalid filter IP version: %d",inp_filter->ip_vsn);
      return FALSE;
    }
  }/* if flow_type = RX*/
  if ((flow_type & QOS_MASK_TX_FLOW) ||
      (flow_type & QOS_MODIFY_MASK_TX_FLTR_MODIFY) )
  { 
    DS_3GPP_MSG0_HIGH("Filter validation for TX");
    if(inp_filter->ip_vsn == IP_V4)
    {
      temp = (uint8)
        (inp_filter->ip_hdr.v4.field_mask &((uint8)IPFLTR_MASK_IP4_SRC_ADDR));
      if(temp != 0)
      {
        DS_3GPP_MSG0_ERROR("SRC_ADDR mask set for UL filter");
        return FALSE;
      }
    }
    else if (inp_filter->ip_vsn == IP_V6)
    {
      temp=(uint8)
       (inp_filter->ip_hdr.v6.field_mask &((uint8)IPFLTR_MASK_IP6_SRC_ADDR));
      if(temp != 0)
      {
        DS_3GPP_MSG0_ERROR("SRC_ADDR mask set for UL filter");
        return FALSE;
      }
    }
    else
    {
      DS_3GPP_MSG1_ERROR("Invalid filter IP version: %d",inp_filter->ip_vsn);
      return FALSE;
    }
  }/* if flow_type = TX*/  
  return TRUE;
}/* ds_flow_manager_validate_app_qos_filter */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_CONVERT_NW_FILTER_TO_APP_FILTER

DESCRIPTION
  Converts the NW passed filters spec to App filter spec.

PARAMETERS
 nw_filter            -  network filter to be converted
 ip_filter            -  App filter to store the converted info.
 filter_id            -  filter_id passed by NW
 direction            -  direction in which the filter should be applied.
 flow_context_p       -  Pointer to the flow context

DEPENDENCIES
  None.

RETURN VALUE
boolean          - returns if conversion is successfull
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
boolean ds_flow_manager_convert_nw_filter_to_app_filter
(
  packet_filter_type_T                    *nw_filter,
  ip_filter_type                          *ip_filter,
  byte                                    filter_id,
  ds_tft_pkt_filter_direction_enum_type   direction,
  ip_version_enum_type                    ip_type,
  ds_flow_context_s                       *flow_context_p,
  ds_3gpp_internal_cause_code_enum_type    *cause_code_p
)
{
  int inc = 0;
  byte filter_length = 0;
  byte *filter_contents = NULL;
  ds_tft_comp_type_id_enum_type filter_type = DS_3GPP_TFT_EMPTY_TYPE;
  uint32 ip_address = 0;
  uint32 subnet_address = 0;
  byte  protocol_identifier =0;
  uint16 port_number = 0;
  uint16 port_range = 0;
  byte temp_prefix = 0;
  int ip6_inc =0;
  uint32 esp_spi = 0; uint32 flow_label =0;
  boolean prefix_len_found = FALSE;
  ds_3gpp_filter_mask_enum_type filter_mask = DS_3GPP_FILTER_NONE;
#ifdef FEATURE_DATA_REL11
  ds_3gpp_filter_mask_enum_type local_address_mask = DS_3GPP_FILTER_NONE;
#endif /* FEATURE_DATA_REL11 */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG3_HIGH("Convert NW filters ID=%d and direction is %d",filter_id,(int)direction,0);
  
  if( cause_code_p == NULL) 
  {
     DS_3GPP_MSG0_ERROR("cause code is  NULL");
	 return FALSE;
  }
  
  if(nw_filter == NULL || ip_filter == NULL)
  {
    DS_3GPP_MSG0_ERROR("input parameters are NULL");
    *cause_code_p = DS_3GPP_INTERNAL_INVALID_MANDATORY_INFO;
    return DS3G_FAILURE;
  }

  filter_length = nw_filter->content_length;
  filter_contents = (nw_filter->filter_contents);
 
  if((filter_length >= MAX_FILTER_CONTENT_LENGTH) ||
     (filter_length == 0))
  { 
    DS_3GPP_MSG1_ERROR("Filter length is invalid.Len=%d",filter_length);
    *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
    return DS3G_FAILURE;
  }
 
  /*----------------------------------------------------------------------
     Copy the filter ID and filter precedence
     Filter ID should be unique.
  -----------------------------------------------------------------------*/   
  if(ds_flow_context_validate_flow(flow_context_p))
  {
    ip_filter->ipfltr_aux_info.fi_id=
                       ((flow_context_p->sdf_id & 0xFF) << 8) | filter_id;
  }
  else
  {
    DS_3GPP_MSG0_MED("Flow cntx is NULL in convert_nw_filter_to_app_filter");
    ip_filter->ipfltr_aux_info.fi_id=filter_id;
  }
  ip_filter->ipfltr_aux_info.fi_precedence = nw_filter->eval_precedence;

  /*----------------------------------------------------------------------
     Initialize the Filter Version
   -----------------------------------------------------------------------*/   
  ip_filter->ip_vsn = ip_type;

  while(inc < filter_length)
  {
    filter_type =(ds_tft_comp_type_id_enum_type)filter_contents[inc];
    inc++;
    switch(filter_type)
    {
      case DS_3GPP_TFT_IPV4_ADDR:
        /* check if the two remote ip address are received */
        if( ((uint32)filter_mask & (uint32)DS_3GPP_FILTER_IP_ADDR_TYPE)
#ifdef FEATURE_DATA_REL11
            && (filter_type == DS_3GPP_TFT_IPV4_ADDR)
#endif /* FEATURE_DATA_REL11 */
          )
        {
          DS_3GPP_MSG0_ERROR("Received two remote ip addr filters");
          *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER; 
          return DS3G_FAILURE;
        }
        else
#ifdef FEATURE_DATA_REL11
        if (filter_type == DS_3GPP_TFT_IPV4_ADDR)
#endif /* FEATURE_DATA_REL11 */
        {
          filter_mask =(ds_3gpp_filter_mask_enum_type)
               ((uint32)filter_mask |(uint32)DS_3GPP_FILTER_IP_ADDR_TYPE);
        }
#ifdef FEATURE_DATA_REL11
      case DS_3GPP_TFT_IPV4_LOCAL_ADDR:
        /* check if the two local ip address are received */
        if((((uint32)local_address_mask & (uint32)DS_3GPP_FILTER_IP_LOCAL_ADDR_TYPE) &&
             filter_type == DS_3GPP_TFT_IPV4_LOCAL_ADDR ))
        {
          DS_3GPP_MSG0_ERROR("Received two local ip addr filters");
          *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER; 
          return DS3G_FAILURE;
        }

        /* set the local address mask if local address is received */
        if(filter_type == DS_3GPP_TFT_IPV4_LOCAL_ADDR)
        {
          local_address_mask =(ds_3gpp_filter_mask_enum_type)
            ((uint32)local_address_mask  | (uint32)DS_3GPP_FILTER_IP_LOCAL_ADDR_TYPE);
          DS_3GPP_MSG1_HIGH("local addresss mask %d",(uint32)local_address_mask);
        }
#endif /* FEATURE_DATA_REL11 */
        
        /* checking if the filter length meets the ipv4 networking standards */
        if(filter_length < (inc+8))
        {
          DS_3GPP_MSG2_ERROR("Invalid filter data for:%d and filter length:%d",
	                   (int)filter_type,filter_length); 
          *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
          return DS3G_FAILURE;
        }
        else
        {
          /*-----------------------------------------------------------------
            Overwrite the IP Version.
          -----------------------------------------------------------------*/  
          if(ip_filter->ip_vsn != IP_V4)
          {
            memset(&(ip_filter->ip_hdr),0, sizeof(ip_filter->ip_hdr));
            memset(&(ip_filter->next_prot_hdr),0, sizeof(ip_filter->next_prot_hdr));
            filter_mask = DS_3GPP_FILTER_NONE;
            inc = 0;
            ip_filter->ip_vsn = IP_V4;
            DS_3GPP_MSG3_HIGH("Resetting Ip version: %d for filter type:%d",
	                   (int)ip_filter->ip_vsn,(int)filter_type,0); 
            break;
          }
          ip_address =(uint32)(filter_contents[inc++] << 24);
          ip_address |=(uint32)(filter_contents[inc++] <<16);
          ip_address |=(uint32)(filter_contents[inc++] <<8);
          ip_address |=(uint32)(filter_contents[inc++]);
      
          subnet_address =(uint32)(filter_contents[inc++] << 24);
          subnet_address |=(uint32)(filter_contents[inc++] <<16);
          subnet_address |=(uint32)(filter_contents[inc++] <<8);
          subnet_address |=(uint32)(filter_contents[inc++]);
          if(direction == DS_3GPP_TFT_DOWNLINK)
          {
#ifdef FEATURE_DATA_REL11
            if(filter_type == DS_3GPP_TFT_IPV4_ADDR)
#endif /* FEATURE_DATA_REL11 */
            {
            ip_filter->ip_hdr.v4.src.addr.ps_s_addr = ps_ntohl(ip_address);
            ip_filter->ip_hdr.v4.src.subnet_mask.ps_s_addr =ps_ntohl(subnet_address);
            ip_filter->ip_hdr.v4.field_mask |=(int)IPFLTR_MASK_IP4_SRC_ADDR;
              DS_3GPP_MSG3_HIGH("Recvd downlink IPv4 Address remote address:0x%x"
                                "and subnet mask 0x%x",
                                ip_filter->ip_hdr.v4.src.addr.ps_s_addr,
                          ip_filter->ip_hdr.v4.src.subnet_mask.ps_s_addr, 0);
              DS_3GPP_MSG1_HIGH("Setting downlink field mask to %d",
                                ip_filter->ip_hdr.v4.field_mask);
            }
#ifdef FEATURE_DATA_REL11
           /*------------------------------------------------------------------ 
              if the direction is downlink and received filter type is local 
              address then update destination parameters and set the destination 
              field mask.
              (Destination)(local) UE <--downlink---( Source )(remote) Server 
            ------------------------------------------------------------------*/
            if(filter_type == DS_3GPP_TFT_IPV4_LOCAL_ADDR)
            {
              ip_filter->ip_hdr.v4.dst.addr.ps_s_addr = ps_ntohl(ip_address);
              ip_filter->ip_hdr.v4.dst.subnet_mask.ps_s_addr =ps_ntohl(subnet_address);
              ip_filter->ip_hdr.v4.field_mask |=(int)IPFLTR_MASK_IP4_DST_ADDR;
              DS_3GPP_MSG3_HIGH("Recvd downlink IPv4 Address local address:0x%x"
                                "and subnet mask 0x%x",
                                ip_filter->ip_hdr.v4.dst.addr.ps_s_addr,
                                ip_filter->ip_hdr.v4.dst.subnet_mask.ps_s_addr, 0);
              DS_3GPP_MSG1_HIGH("Setting downlink field mask to %d",
                                ip_filter->ip_hdr.v4.field_mask);
            }
#endif /* FEATURE_DATA_REL11 */
          }
          if(direction == DS_3GPP_TFT_UPLINK)
          {
#ifdef FEATURE_DATA_REL11
            if(filter_type == DS_3GPP_TFT_IPV4_ADDR)
#endif /* FEATURE_DATA_REL11 */
            {
            ip_filter->ip_hdr.v4.dst.addr.ps_s_addr =ps_ntohl(ip_address);
            ip_filter->ip_hdr.v4.dst.subnet_mask.ps_s_addr = ps_ntohl(subnet_address);
            ip_filter->ip_hdr.v4.field_mask |=(int)IPFLTR_MASK_IP4_DST_ADDR;
              DS_3GPP_MSG3_HIGH("Recvd uplink IPv4 Address remote address:0x%x"
                                "and subnet mask 0x%x",
                                ip_filter->ip_hdr.v4.dst.addr.ps_s_addr,
                          ip_filter->ip_hdr.v4.dst.subnet_mask.ps_s_addr,0);
          }
#ifdef FEATURE_DATA_REL11
            /*----------------------------------------------------------------
            if the direction is uplink and received filter type is local 
            address then update source parameters and set the source 
            field mask 
            (Source)(local) UE --uplink-->( Destination )(remote) Server 
             ----------------------------------------------------------------*/
            if(filter_type == DS_3GPP_TFT_IPV4_LOCAL_ADDR)
            {
              ip_filter->ip_hdr.v4.src.addr.ps_s_addr =ps_ntohl(ip_address);
              ip_filter->ip_hdr.v4.src.subnet_mask.ps_s_addr = ps_ntohl(subnet_address);
              ip_filter->ip_hdr.v4.field_mask |=(int)IPFLTR_MASK_IP4_SRC_ADDR;
              DS_3GPP_MSG3_HIGH("Recvd uplink IPv4 Address local address:0x%x"
                                "and subnet mask 0x%x",
                                ip_filter->ip_hdr.v4.src.addr.ps_s_addr,
                                ip_filter->ip_hdr.v4.src.subnet_mask.ps_s_addr,0);
            }
#endif /* FEATURE_DATA_REL11 */
          }

        }/* else filter_length */
      break;
      case DS_3GPP_TFT_IPV6_ADDR:
        /* check if the two remote ip address are received */
        if((uint32)filter_mask & (uint32)DS_3GPP_FILTER_IP_ADDR_TYPE
#ifdef FEATURE_DATA_REL11
            && filter_type == DS_3GPP_TFT_IPV6_ADDR
#endif /* FEATURE_DATA_REL11 */
          )
        {
          DS_3GPP_MSG0_ERROR("Received two remote ip addr filters");
          *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER; 
          return DS3G_FAILURE;
        }
        else
#ifdef FEATURE_DATA_REL11
        if (filter_type == DS_3GPP_TFT_IPV6_ADDR)
#endif /* FEATURE_DATA_REL11 */
        {
          filter_mask =(ds_3gpp_filter_mask_enum_type)
                   ((uint32)filter_mask|(uint32)DS_3GPP_FILTER_IP_ADDR_TYPE);
        }

#ifdef FEATURE_DATA_REL11
      case DS_3GPP_TFT_IPV6_REM_PRE_ADDR:
      case DS_3GPP_TFT_IPV6_LOCAL_PRE_ADDR:
        /* check if the two local ip address/prefix type are received */
        if(((uint32)local_address_mask & (uint32)DS_3GPP_FILTER_IP_LOCAL_PREFIX_TYPE)
           && filter_type == DS_3GPP_TFT_IPV6_LOCAL_PRE_ADDR )
        {
          DS_3GPP_MSG0_ERROR("Received two local ip addr/prefix filters");
          *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER; 
          return DS3G_FAILURE;
        }
        /* check if the two remote ip address /prefix type are received */
        if(((uint32)filter_mask & (uint32)DS_3GPP_FILTER_IP_REMOTE_PREFIX_TYPE)
             && filter_type == DS_3GPP_TFT_IPV6_REM_PRE_ADDR)
        {
          DS_3GPP_MSG0_ERROR("Received two remote ip addr/prefix filters");
          *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER; 
          return DS3G_FAILURE;
        }

       /*---------------------------------------------------------------------------
        Case 1: 
        Old address format received after new address format: Here we are checking if 
        previous component  received was remote/prefix typeand current component  is 
        remote address,if "yes" then we ignore remote address and continue with other 
        components else we continue with remote address 
        ---------------------------------------------------------------------------*/
        if(filter_type == DS_3GPP_TFT_IPV6_ADDR)
        {
          if((uint32)filter_mask & (uint32)DS_3GPP_FILTER_IP_REMOTE_PREFIX_TYPE)
          {
            DS_3GPP_MSG0_HIGH("received a filter of remote type DS_3GPP_TFT_IPV6_ADDR"
            "ignore since DS_3GPP_FILTER_IP_REMOTE_PREFIX_TYPE is already received");
            inc = inc+32;
            continue;
          }
        }

         /*----------------------------------------------------------------------------
            Case 2: New address format received after Old address format 
            Check 1:outer if checks if the current component ipv6 remote/prefix 
            address type.
            Check 2:internal if statement check if previously received component 
             was remote address or not
          ----------------------------------------------------------------------------*/
        if(filter_type == DS_3GPP_TFT_IPV6_REM_PRE_ADDR)
        { 
          if((uint32)filter_mask & (uint32)DS_3GPP_FILTER_IP_ADDR_TYPE)
          {
            /* -------------------------------------------------------------------------
             if both the checks are true which means we replace the remote address type 
             with remote/prefix type according to 3gpp spec and reset the field masks 
             --------------------------------------------------------------------------*/
            if(direction == DS_3GPP_TFT_DOWNLINK )
            {
              memset(&(ip_filter->ip_hdr.v6.src),0, sizeof(ip_filter->ip_hdr.v6.src));
              ip_filter->ip_hdr.v6.field_mask ^= IPFLTR_MASK_IP6_SRC_ADDR;
            }
            if(direction == DS_3GPP_TFT_UPLINK)
            {
              memset(&(ip_filter->ip_hdr.v6.dst),0, sizeof(ip_filter->ip_hdr.v6.dst));
              ip_filter->ip_hdr.v6.field_mask ^= IPFLTR_MASK_IP6_DST_ADDR;
            }
          }
        }

        /*---------------------------------------------------------------------------------
         set the filter mask to remote/prefix type and enable the received_prefix_from_network
         flag if the filter type is remote/prefix address type 
        ----------------------------------------------------------------------------------*/
        if(filter_type == DS_3GPP_TFT_IPV6_REM_PRE_ADDR)
        {
          filter_mask =(ds_3gpp_filter_mask_enum_type)
                ((uint32)filter_mask|(uint32)DS_3GPP_FILTER_IP_REMOTE_PREFIX_TYPE);
        }
        /*---------------------------------------------------------------------------------
        set the filter mask to local /prefix type and enable the received_prefix_from_network
                   flag  if the filter type is local/prefix address type.
        ----------------------------------------------------------------------------------*/
        if(filter_type == DS_3GPP_TFT_IPV6_LOCAL_PRE_ADDR)
        {
          local_address_mask = (ds_3gpp_filter_mask_enum_type)
                ((uint32)local_address_mask|(uint32)DS_3GPP_FILTER_IP_LOCAL_PREFIX_TYPE);
        }

        /* --------------------------------------------------------------------------------
           checks if the filter length meets the ipv6 networking standards 
           since in remote/local prefix type first 16 octets is address and 1 octet is prefix 
         ---------------------------------------------------------------------------------*/
        if(filter_type == DS_3GPP_TFT_IPV6_REM_PRE_ADDR || 
           filter_type == DS_3GPP_TFT_IPV6_LOCAL_PRE_ADDR)
        {
          if(filter_length < (inc+17))
          {
            DS_3GPP_MSG3_ERROR("Invalid filter data for:%d and filter length:%d",
                              (int)filter_type,filter_length,0); 
            *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
            return DS3G_FAILURE;
          }
        }
       
        /*----------------------------------------------------------------------------------
           checks if the filter length meets the ipv6 networking standards 
           since first 16 octets is address and next 16 octets is the prefix 
        -----------------------------------------------------------------------------------*/

        if(filter_type == DS_3GPP_TFT_IPV6_ADDR)
#endif /* FEATURE_DATA_REL11 */
        {
        if(filter_length < (inc+32))
        {
          DS_3GPP_MSG3_ERROR("Invalid filter data for:%d and filter length:%d",
	                   (int)filter_type,filter_length,0); 
          *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
          return DS3G_FAILURE;
        }
        }

        /*-----------------------------------------------------------------
          Overwrite the IP Version.
        -----------------------------------------------------------------*/         
        if(ip_filter->ip_vsn != IP_V6)
        {
          memset(&(ip_filter->ip_hdr),0, sizeof(ip_filter->ip_hdr));
          memset(&(ip_filter->next_prot_hdr),0, sizeof(ip_filter->next_prot_hdr));
          filter_mask = DS_3GPP_FILTER_NONE;
          inc = 0;
          ip_filter->ip_vsn = IP_V6;
          DS_3GPP_MSG3_HIGH("Resetting Ip version: %d for filter type:%d",
	                   (int)ip_filter->ip_vsn,(int)filter_type,0); 
          break;
        }
        if(direction == DS_3GPP_TFT_DOWNLINK)
        {
          for(ip6_inc=0;ip6_inc<16;ip6_inc++)
          {
#ifdef FEATURE_DATA_REL11
            /*------------------------------------------------------------------ 
            if the direction is downlink and received filter type is remote 
            address then update destination parameters and set the destination 
            field mask.
            (Destination)(local) UE <--downlink---( Source )(remote) Server 
            ------------------------------------------------------------------*/
            if( (filter_type == DS_3GPP_TFT_IPV6_ADDR) ||
                (filter_type == DS_3GPP_TFT_IPV6_REM_PRE_ADDR) )
#endif /* FEATURE_DATA_REL11 */
            {
            ip_filter->ip_hdr.v6.src.addr.in6_u.u6_addr8[ip6_inc] =
                        filter_contents[inc];
            inc++;
          }
#ifdef FEATURE_DATA_REL11
          /*------------------------------------------------------------------ 
          if the direction is downlink and received filter type is local 
          address then update destination parameters and set the destination 
          field mask.
          (Destination)(local) UE <--downlink---( Source )(remote) Server 
            ------------------------------------------------------------------*/
            if(filter_type == DS_3GPP_TFT_IPV6_LOCAL_PRE_ADDR)
            {
              ip_filter->ip_hdr.v6.dst.addr.in6_u.u6_addr8[ip6_inc] =
                        filter_contents[inc];
              inc++;
            }
#endif /* FEATURE_DATA_REL11 */
          }

#ifdef FEATURE_DATA_REL11
     /*----------------------------------------------------------------------------
        check if we have received a prefix length from network 
        incase of remote address type we need to compute it 
        incase of remote/local prefix address type network provides us 
        with the ipv6 prefix (first 16 octets is address and 17th octet is prefix  
     -----------------------------------------------------------------------------*/ 
        if(filter_type == DS_3GPP_TFT_IPV6_ADDR)
        {
#endif /* FEATURE_DATA_REL11 */
            prefix_len_found = FALSE;
          for(ip6_inc=0;ip6_inc<16;ip6_inc++)
          {
            if(prefix_len_found == FALSE)
            {
              if(filter_contents[inc] != 0xFF)
              {
                ip_filter->ip_hdr.v6.src.prefix_len = ip6_inc * 8;
                temp_prefix = (1 << 7);
                if (filter_contents[inc] != 0)
                {
                  while((filter_contents[inc] & temp_prefix) == temp_prefix)
                  {
                    temp_prefix = temp_prefix >> 1;
                    ip_filter->ip_hdr.v6.src.prefix_len++; 
                  } 
                  inc++; 
                }
                else
                {
                  inc++;
                }
                prefix_len_found = TRUE;
                DS_3GPP_MSG1_HIGH("prefix length %d",
                                  ip_filter->ip_hdr.v6.src.prefix_len );
              }
              else
              {
                inc++;
              }
            }  
            else
            {
              inc++;
            }
          }/* for */
#ifdef FEATURE_DATA_REL11
          }
          else
          { 
            /* if the 17th octet is not null then update the coressponding src/dst contents */
            if(filter_contents[inc] != 0)
            {
              if(filter_type == DS_3GPP_TFT_IPV6_REM_PRE_ADDR )
              {
                ip_filter->ip_hdr.v6.src.prefix_len = filter_contents[inc];
                inc++;
                DS_3GPP_MSG1_HIGH("remote prefix length %d <- src",
                                ip_filter->ip_hdr.v6.src.prefix_len );
              }
              if(filter_type == DS_3GPP_TFT_IPV6_LOCAL_PRE_ADDR)
              {
                ip_filter->ip_hdr.v6.dst.prefix_len = filter_contents[inc];
                inc++;
                DS_3GPP_MSG1_HIGH("local prefix length %d <- dst",
                                ip_filter->ip_hdr.v6.dst.prefix_len );
              }
            }
           /* otherwise use the default value 128 to update */
            else
            {
              if(filter_type == DS_3GPP_TFT_IPV6_REM_PRE_ADDR)
              {
                ip_filter->ip_hdr.v6.src.prefix_len = 128;
                inc++;
                DS_3GPP_MSG1_HIGH("remote prefix length %d <- src",
                ip_filter->ip_hdr.v6.src.prefix_len );
              }
              if(filter_type == DS_3GPP_TFT_IPV6_LOCAL_PRE_ADDR)
              {
                ip_filter->ip_hdr.v6.dst.prefix_len = 128;
                inc++;
                DS_3GPP_MSG1_HIGH("local prefix length %d <- dst",
                ip_filter->ip_hdr.v6.dst.prefix_len );
              }
            }
          }
#endif /* FEATURE_DATA_REL11 */
          if(prefix_len_found == FALSE && filter_type == DS_3GPP_TFT_IPV6_ADDR )
          {
            ip_filter->ip_hdr.v6.src.prefix_len = 128;
          }
          /* set the field mask to src address(downlink) if remote address is received otherwise
                      set local address mask */
#ifdef FEATURE_DATA_REL11
          if(filter_type == DS_3GPP_TFT_IPV6_REM_PRE_ADDR || 
             filter_type == DS_3GPP_TFT_IPV6_ADDR)
          { 
#endif /* FEATURE_DATA_REL11 */
          ip_filter->ip_hdr.v6.field_mask |=(int)IPFLTR_MASK_IP6_SRC_ADDR;
#ifdef FEATURE_DATA_REL11
          }
          if(filter_type == DS_3GPP_TFT_IPV6_LOCAL_PRE_ADDR)
          {
            ip_filter->ip_hdr.v6.field_mask |=(int)IPFLTR_MASK_IP6_DST_ADDR;
          }
#endif /* FEATURE_DATA_REL11 */
          DS_3GPP_MSG0_HIGH("Received IPv6 Address and subnet mask for downlink");
        } /* DS_3GPP_TFT_DOWNLINK*/
        if(direction == DS_3GPP_TFT_UPLINK)
        {
          for(ip6_inc=0;ip6_inc<16;ip6_inc++)
          {
#ifdef FEATURE_DATA_REL11
         /*------------------------------------------------------------------
         if the direction is uplink and received filter type is remote 
         address then update source parameters and set the source 
         field mask 
         (Source)(local) UE --uplink-->( Destination )(remote) Server 
         ----------------------------------------------------------------*/
            if(filter_type == DS_3GPP_TFT_IPV6_ADDR 
              || filter_type == DS_3GPP_TFT_IPV6_REM_PRE_ADDR)
#endif /* FEATURE_DATA_REL11 */
            {
            ip_filter->ip_hdr.v6.dst.addr.in6_u.u6_addr8[ip6_inc] =
                                           filter_contents[inc];
            inc++;
          }
#ifdef FEATURE_DATA_REL11
          /*------------------------------------------------------------------
          if the direction is uplink and received filter type is local 
          address then update source parameters and set the source 
          field mask 
          (Source)(local) UE --uplink-->( Destination )(remote) Server 
           ----------------------------------------------------------------*/
            if(filter_type == DS_3GPP_TFT_IPV6_LOCAL_PRE_ADDR)
            {
              ip_filter->ip_hdr.v6.src.addr.in6_u.u6_addr8[ip6_inc] =
                                           filter_contents[inc];
              inc++;
            }
#endif /* FEATURE_DATA_REL11 */
          }

          prefix_len_found = FALSE;
#ifdef FEATURE_DATA_REL11
       /*------------------------------------------------------------------------
       check if we have received a prefix length from network 
       incase of remote address type we need to compute it 
       incase of remote/local prefix address type network provides us 
       with the ipv6 prefix (first 16 octets is address and 17th octet is prefix  
       -------------------------------------------------------------------------*/
          if(filter_type == DS_3GPP_TFT_IPV6_ADDR)
          {
#endif /* FEATURE_DATA_REL11 */
          for(ip6_inc=0;ip6_inc<16;ip6_inc++)
          {
            if(prefix_len_found == FALSE)
            {
              if(filter_contents[inc] != 0xFF)
              {
                ip_filter->ip_hdr.v6.dst.prefix_len = ip6_inc * 8;
                temp_prefix = (1 << 7);
                if (filter_contents[inc] != 0)
                {
                  while((filter_contents[inc] & temp_prefix) != temp_prefix)
                  {
                    temp_prefix = temp_prefix >> 1;
                    ip_filter->ip_hdr.v6.dst.prefix_len++; 
                  } 
                  inc++; 
                }
                else
                {
                  inc++;
                }
                prefix_len_found = TRUE;
                  DS_3GPP_MSG1_HIGH("prefix length %d",
                                   ip_filter->ip_hdr.v6.dst.prefix_len );
              }
              else
              {
                inc++;
              }
            }/* prefix_len_found == FALSE*/  
            else
            {
              inc++;
            }
          }/* for */
#ifdef FEATURE_DATA_REL11
          }
          else
          {
            /* if the 17th octet is not null then update the coressponding src/dst contents */
            if(filter_contents[inc] != 0)
            {
              if(filter_type == DS_3GPP_TFT_IPV6_REM_PRE_ADDR)
              {
                ip_filter->ip_hdr.v6.dst.prefix_len = filter_contents[inc];
                inc++;
                DS_3GPP_MSG1_HIGH("remote prefix length %d <- dst",
                                ip_filter->ip_hdr.v6.dst.prefix_len );
              }
              if(filter_type == DS_3GPP_TFT_IPV6_LOCAL_PRE_ADDR)
              {
                ip_filter->ip_hdr.v6.src.prefix_len = filter_contents[inc];
                inc++;
                DS_3GPP_MSG1_HIGH("local prefix length %d <- src",
                                ip_filter->ip_hdr.v6.src.prefix_len );
              }
            }
            /* otherwise use the default value 128 to update */
            else
            {
              if(filter_type == DS_3GPP_TFT_IPV6_REM_PRE_ADDR)
              {
                ip_filter->ip_hdr.v6.dst.prefix_len = 128;
                inc++;
                DS_3GPP_MSG1_HIGH("remote prefix length %d <- dst",
                               ip_filter->ip_hdr.v6.dst.prefix_len );
              }
              if(filter_type == DS_3GPP_TFT_IPV6_LOCAL_PRE_ADDR)
              {
                ip_filter->ip_hdr.v6.src.prefix_len = 128;
                inc++;
                DS_3GPP_MSG1_HIGH("local prefix length %d <- src",
                               ip_filter->ip_hdr.v6.src.prefix_len );
              }
            }
          }
#endif /* FEATURE_DATA_REL11 */
          if(prefix_len_found == FALSE && filter_type == DS_3GPP_TFT_IPV6_ADDR)
          {
            ip_filter->ip_hdr.v6.dst.prefix_len = 128;
          }
#ifdef FEATURE_DATA_REL11
          /* set the field mask to dst address(uplink) if remote address is received otherwise 
                      set local address mask */
          if(filter_type == DS_3GPP_TFT_IPV6_REM_PRE_ADDR ||
             filter_type == DS_3GPP_TFT_IPV6_ADDR)
          {
#endif /* FEATURE_DATA_REL11 */
          ip_filter->ip_hdr.v6.field_mask |=(int)IPFLTR_MASK_IP6_DST_ADDR;
#ifdef FEATURE_DATA_REL11
          }
          if(filter_type == DS_3GPP_TFT_IPV6_LOCAL_PRE_ADDR)
          {
            ip_filter->ip_hdr.v6.field_mask |=(int)IPFLTR_MASK_IP6_SRC_ADDR;
          }
#endif /* FEATURE_DATA_REL11 */
          DS_3GPP_MSG0_HIGH("Received IPv6 Address and subnet mask for uplink");
        }/* UPLINK */
      break;
      case DS_3GPP_TFT_NEXT_HDR_TYPE:
        if((uint32)filter_mask & (uint32)DS_3GPP_FILTER_NXT_HDR_TYPE)
        {
          DS_3GPP_MSG0_ERROR("Received two next hdr type filters");
          *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;   
          return DS3G_FAILURE;
        }
        else
        {
          filter_mask =(ds_3gpp_filter_mask_enum_type)
            ((uint32)filter_mask|(uint32)DS_3GPP_FILTER_NXT_HDR_TYPE);
        }
        if(filter_length < (inc + 1))
        {
          DS_3GPP_MSG2_ERROR("Invalid filter data for:%d and filter length:%d",
	                 (int)filter_type,filter_length); 
          *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
          return DS3G_FAILURE;
        }
        else
        {
          protocol_identifier = filter_contents[inc];
          inc++;
          if(ip_filter->ip_vsn == IP_V4)
          {
            ip_filter->ip_hdr.v4.next_hdr_prot = protocol_identifier;
            ip_filter->ip_hdr.v4.field_mask |=
				                (int)IPFLTR_MASK_IP4_NEXT_HDR_PROT;
          }
          if(ip_filter->ip_vsn == IP_V6)
          {
            ip_filter->ip_hdr.v6.next_hdr_prot = protocol_identifier;
            ip_filter->ip_hdr.v6.field_mask |=
				                (int)IPFLTR_MASK_IP6_NEXT_HDR_PROT;
          }
          /* If the port information is sent before the next header*/
          if (((direction == DS_3GPP_TFT_DOWNLINK) &&
               (filter_mask & DS_3GPP_FILTER_REMOTE_PORT_TYPE)) ||
               ((direction == DS_3GPP_TFT_UPLINK) &&
               (filter_mask & DS_3GPP_FILTER_LOCAL_PORT_TYPE)))
          {
            DS_3GPP_MSG0_HIGH("Previously received SRC port information");
            if(protocol_identifier == (byte)PS_IPPROTO_TCP)
            {
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                           (int)IPFLTR_MASK_TCP_SRC_PORT;
            }
            if(protocol_identifier == (byte)PS_IPPROTO_UDP)
            {
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                          (int)IPFLTR_MASK_UDP_SRC_PORT;
            }
          }
          if (((direction == DS_3GPP_TFT_DOWNLINK) &&
               (filter_mask & DS_3GPP_FILTER_LOCAL_PORT_TYPE)) ||
               ((direction == DS_3GPP_TFT_UPLINK) &&
               (filter_mask & DS_3GPP_FILTER_REMOTE_PORT_TYPE)))
          {
            DS_3GPP_MSG0_HIGH("Previously received DST port information");
            if(protocol_identifier == (byte)PS_IPPROTO_TCP)
            {
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                           (int)IPFLTR_MASK_TCP_DST_PORT;
            }
            if(protocol_identifier == (byte)PS_IPPROTO_UDP)
            {
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                          (int)IPFLTR_MASK_UDP_DST_PORT;
            }
          }
          DS_3GPP_MSG1_HIGH("Received %d as the next header",protocol_identifier);  
        }/* else valid data */
      break;
      case DS_3GPP_TFT_SINGLE_LOCAL_PORT:
        if((uint32)filter_mask & (uint32)DS_3GPP_FILTER_LOCAL_PORT_TYPE)
        {
          DS_3GPP_MSG0_ERROR("Received two local port type filters");
          *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER; 
          return DS3G_FAILURE;
        }
        else
        {
          filter_mask =(ds_3gpp_filter_mask_enum_type)
               ((uint32)filter_mask|(uint32)DS_3GPP_FILTER_LOCAL_PORT_TYPE);
        }
        if(filter_length < (inc + 2))
        {
          DS_3GPP_MSG2_ERROR("Invalid filter data for:%d and filter length:%d",
	                      (int)filter_type,filter_length); 
          *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
          return DS3G_FAILURE;
        }
        else
        {
          port_number =(uint16)(filter_contents[inc++] << 8);
          port_number |=(uint16)(filter_contents[inc++]);
          if(direction == DS_3GPP_TFT_DOWNLINK)
          {
            if(protocol_identifier ==(byte)PS_IPPROTO_TCP)
            {
              ip_filter->next_prot_hdr.tcp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.tcp.dst.range = 0;
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                               (int)IPFLTR_MASK_TCP_DST_PORT;
              DS_3GPP_MSG2_HIGH("Received local port number 0x%x, for TCP in downlink",
                        port_number,protocol_identifier); 
            }
            else if(protocol_identifier ==(byte)PS_IPPROTO_UDP)
            {
              ip_filter->next_prot_hdr.udp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.dst.range = 0;
              ip_filter->next_prot_hdr.udp.field_mask |=
                                             (int)IPFLTR_MASK_UDP_DST_PORT;
              DS_3GPP_MSG2_HIGH("Received local port number 0x%x, for UDP in downlink",
	                      port_number,protocol_identifier); 
            }
            else
            {
              /*use either TCP or UDP as both are of same format and size*/
              ip_filter->next_prot_hdr.udp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.dst.range =0;
              DS_3GPP_MSG2_HIGH("Received local port number 0x%x, but protocol is 0x%x",
                                       port_number,protocol_identifier); 
            }
          }/* direction == DOWNLINK */
          if(direction == DS_3GPP_TFT_UPLINK)
          {
            if(protocol_identifier ==(byte)PS_IPPROTO_TCP)
            {
              ip_filter->next_prot_hdr.tcp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.tcp.src.range =0;
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                             (int)IPFLTR_MASK_TCP_SRC_PORT;
              DS_3GPP_MSG2_HIGH("Received local port number 0x%x, for TCP in Uplink",
   	                      port_number,protocol_identifier); 
            }
            else if(protocol_identifier ==(byte)PS_IPPROTO_UDP)
            {
              ip_filter->next_prot_hdr.udp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.src.range = 0;
              ip_filter->next_prot_hdr.udp.field_mask |=
                                           (int)IPFLTR_MASK_UDP_SRC_PORT;
              DS_3GPP_MSG2_HIGH("Received local port number 0x%x, for UDP in Uplink",
                      port_number,protocol_identifier); 
            }
            else
            {
              /*use either TCP or UDP as both are of same format and size*/
              ip_filter->next_prot_hdr.udp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.src.range =0;
              DS_3GPP_MSG2_HIGH("Received local port number 0x%x,prot is 0x%x",
                                    port_number,protocol_identifier); 
            }
          } /* direction == UPLINK */
        } /* else filter_length */
      break;
      case DS_3GPP_TFT_LOCAL_PORT_RANGE:
        if((uint32)filter_mask & (uint32)DS_3GPP_FILTER_LOCAL_PORT_TYPE)
        {
          DS_3GPP_MSG0_ERROR("Received two local port type filters");
          *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
          return DS3G_FAILURE;
        }
        else
        {
          filter_mask =(ds_3gpp_filter_mask_enum_type)
             ((uint32)filter_mask|(uint32)DS_3GPP_FILTER_LOCAL_PORT_TYPE);
        }
        if(filter_length < (inc + 4))
        {
          DS_3GPP_MSG2_ERROR("Invalid filter data for:%d and filter length:%d",
                               (int)filter_type,filter_length); 
          *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
          return DS3G_FAILURE;
        }
        else
        {
          port_number =(uint16)(filter_contents[inc++] << 8);
          port_number |=(uint16)(filter_contents[inc++]);

          port_range =(uint16)(filter_contents[inc++] << 8);
          port_range |=(uint16)(filter_contents[inc++]);

          if(direction == DS_3GPP_TFT_DOWNLINK)
          {
            if(protocol_identifier ==(byte)PS_IPPROTO_TCP)
            { 
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                DS_3GPP_MSG2_ERROR("Invalid values port 0x%x and range 0x%x",
                              port_number,port_range);
                *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                return DS3G_FAILURE;
              }
              DS_3GPP_MSG2_HIGH("Rec local port no 0x%x,range 0x%x for TCP in downlink",
                                port_number, port_range); 
              port_range = port_range - port_number;              
              ip_filter->next_prot_hdr.tcp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.tcp.dst.range = (port_range);
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                            (int)IPFLTR_MASK_TCP_DST_PORT;             
            } /* PROTO_TCP */
            else if(protocol_identifier ==(byte)PS_IPPROTO_UDP)
            {
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                DS_3GPP_MSG3_ERROR("Invalid values for port 0x%x and range 0x%x",
                              port_number,port_range,0);
                *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                return DS3G_FAILURE;
              }
              DS_3GPP_MSG2_HIGH("Rec local port 0x%x,range 0x%x for UDP in downlink",
                                port_number, port_range); 
              port_range = port_range - port_number; 
              ip_filter->next_prot_hdr.udp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.dst.range = (port_range);             
              ip_filter->next_prot_hdr.udp.field_mask |=
                                           (int)IPFLTR_MASK_UDP_DST_PORT;
            }/* PROTO_UDP */
            else
            {
              /*use either TCP or UDP as both are of same format and size*/
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                DS_3GPP_MSG2_ERROR("Invalid values port 0x%x and range0x%x",
                              port_number,port_range);
                *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                return DS3G_FAILURE;
              }
              DS_3GPP_MSG3_HIGH("Rec downlink local pno 0x%x,range 0x%x prot is 0x%x",
                     port_number,port_range, protocol_identifier);
              port_range = port_range - port_number;
              ip_filter->next_prot_hdr.udp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.dst.range = (port_range);  
            }
          }/* direction == DOWNLINK */
          if(direction == DS_3GPP_TFT_UPLINK)
          {
            if(protocol_identifier ==(byte)PS_IPPROTO_TCP)
            {  
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                DS_3GPP_MSG2_ERROR("Invalid values port 0x%x and range 0x%x",
                              port_number,port_range);
                *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                return DS3G_FAILURE;
              }
              DS_3GPP_MSG2_HIGH("Rec local port 0x%x, range 0x%x for TCP in Uplink",
	                     port_number,port_range); 
              port_range = port_range - port_number;
              ip_filter->next_prot_hdr.tcp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.tcp.src.range = (port_range);
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                           (int)IPFLTR_MASK_TCP_SRC_PORT;            
            }/* PROTO_TCP */
            else if(protocol_identifier ==(byte)PS_IPPROTO_UDP)
            { 
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                DS_3GPP_MSG2_ERROR("Invalid values port 0x%x and range 0x%x",
                              port_number,port_range);
                *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                return DS3G_FAILURE;
              }
              DS_3GPP_MSG3_HIGH("Rec local port 0x%x,range 0x%x for UDP in Uplink",
                                        port_number,port_range,0); 
              port_range = port_range - port_number;
              ip_filter->next_prot_hdr.udp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.src.range = (port_range);
              ip_filter->next_prot_hdr.udp.field_mask |=
                                           (int)IPFLTR_MASK_UDP_SRC_PORT;             
            } /* PROTO_UDP */
            else
            {
              /*use either TCP or UDP as both are of same format and size*/
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                DS_3GPP_MSG2_ERROR("Invalid values for port 0x%x and range 0x%x",
                              port_number,port_range);
                *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                return DS3G_FAILURE;
              }
              DS_3GPP_MSG3_HIGH("Rec uplink local pno 0x%x,range 0x%x protocol is 0x%x",
                     port_number,port_range, protocol_identifier);
              port_range = port_range - port_number;
              ip_filter->next_prot_hdr.udp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.src.range = (port_range);                            
            }/* else */
          } /* direction == UPLINK */
        }/* else filter_length*/ 
      break;
      case DS_3GPP_TFT_SINGLE_REMOTE_PORT:
        if((uint32)filter_mask & (uint32)DS_3GPP_FILTER_REMOTE_PORT_TYPE)
        {
          DS_3GPP_MSG0_ERROR("Received two remote port type filters");
          *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER; 
          return DS3G_FAILURE;
        }
        else
        {
          filter_mask =(ds_3gpp_filter_mask_enum_type)
             ((uint32)filter_mask|(uint32)DS_3GPP_FILTER_REMOTE_PORT_TYPE);
        }
        if(filter_length < (inc + 2))
        {
          DS_3GPP_MSG2_ERROR("Invalid filter data for:%d and filter length:%d",
	                    (int)filter_type,filter_length); 
          *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER; 
          return DS3G_FAILURE;
        }
        else
        {
          port_number =(uint16)(filter_contents[inc++] << 8);
          port_number |=(uint16)(filter_contents[inc++]);
          if(direction == DS_3GPP_TFT_DOWNLINK)
          {
            if(protocol_identifier ==(byte)PS_IPPROTO_TCP)
            {
              ip_filter->next_prot_hdr.tcp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.tcp.src.range = 0;
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                             (int)IPFLTR_MASK_TCP_SRC_PORT;
              DS_3GPP_MSG1_HIGH("Received remote port number:%d, for TCP in downlink",
	                      port_number); 
            }
            else if(protocol_identifier ==(byte)PS_IPPROTO_UDP)
            {
              ip_filter->next_prot_hdr.udp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.src.range = 0;
              ip_filter->next_prot_hdr.udp.field_mask |=
                                              (int)IPFLTR_MASK_UDP_SRC_PORT;
              DS_3GPP_MSG1_HIGH("Received remote port number:%d, for UDP in downlink",
                              port_number); 
            }
            else
            {
              /*use either TCP or UDP as both are of same format and size*/
              ip_filter->next_prot_hdr.udp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.src.range = 0;
              DS_3GPP_MSG2_HIGH("Received remote port no:%d, but protocol is:%d",
	                      port_number,protocol_identifier); 
            }
          }/* direction == DOWNLINK */
          if(direction == DS_3GPP_TFT_UPLINK)
          {
            if(protocol_identifier ==(byte)PS_IPPROTO_TCP)
            {
              ip_filter->next_prot_hdr.tcp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.tcp.dst.range =0;
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                           (int)IPFLTR_MASK_TCP_DST_PORT;
              DS_3GPP_MSG1_HIGH("Received remote port number:%d, for TCP in Uplink",
                    port_number); 
            }
            else if(protocol_identifier ==(byte)PS_IPPROTO_UDP)
            {
              ip_filter->next_prot_hdr.udp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.dst.range = 0;
              ip_filter->next_prot_hdr.udp.field_mask |=
                                             (int)IPFLTR_MASK_UDP_DST_PORT;
              DS_3GPP_MSG1_HIGH("Received remote port number:%d, for UDP in Uplink",
                    port_number); 
            }
            else
            {
              /*use either TCP or UDP as both are of same format and size*/
              ip_filter->next_prot_hdr.udp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.dst.range = 0;
              DS_3GPP_MSG2_HIGH("Received remote port number:%d, but protocol is:%d",
	                     port_number,protocol_identifier); 
            }
          } /* direction == UPLINK */
        } /* else filter_length*/
      break;
      case DS_3GPP_TFT_REMOTE_PORT_RANGE:
        if((uint32)filter_mask & (uint32)DS_3GPP_FILTER_REMOTE_PORT_TYPE)
        {
          DS_3GPP_MSG0_ERROR("Received two remote port type filters");
          *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER; 
          return DS3G_FAILURE;
        }
        else
        {
          filter_mask =(ds_3gpp_filter_mask_enum_type)
            ((uint32)filter_mask |(uint32)DS_3GPP_FILTER_REMOTE_PORT_TYPE);
        }
        if(filter_length < (inc + 4))
        {
          DS_3GPP_MSG2_ERROR("Invalid filter data for:%d and filter length:%d",
	                      (int)filter_type,filter_length); 
          *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
          return DS3G_FAILURE;
        }
        else
        {
          port_number =(uint16)(filter_contents[inc++] << 8);
          port_number |=(uint16)(filter_contents[inc++]);

          port_range =(uint16)(filter_contents[inc++] << 8);
          port_range |=(uint16)(filter_contents[inc++]);
		 

          if(direction == DS_3GPP_TFT_DOWNLINK)
          {
            if(protocol_identifier ==(byte)PS_IPPROTO_TCP)
            {
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
               DS_3GPP_MSG2_ERROR("Invalid values for port:%d and range:%d",
                              port_number,port_range);
               *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                return DS3G_FAILURE;
              }
              DS_3GPP_MSG2_HIGH("Rec remote port no:%d, range:%d for TCP in downlink",
	                      port_number,port_range); 
              port_range = port_range - port_number;
              ip_filter->next_prot_hdr.tcp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.tcp.src.range = (port_range);              
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                              (int)IPFLTR_MASK_TCP_SRC_PORT;          
            }
            else if(protocol_identifier ==(byte)PS_IPPROTO_UDP)
            {
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                DS_3GPP_MSG2_ERROR("Invalid values for port:%d and range:%d",
                              port_number,port_range);
                *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                return DS3G_FAILURE;
              }  
              DS_3GPP_MSG2_HIGH("Rec remote port no:%d, range:%d for UDP in downlink",
                     port_number,port_range);
              port_range = port_range - port_number;           
              ip_filter->next_prot_hdr.udp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.src.range = (port_range);         
              ip_filter->next_prot_hdr.udp.field_mask |=
                                               (int)IPFLTR_MASK_UDP_SRC_PORT;
            }
            else
            {
              /*use either TCP or UDP as both are of same format and size*/
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                DS_3GPP_MSG2_ERROR("Invalid values for port:%d and range:%d",
                              port_number,port_range);
                *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                return DS3G_FAILURE;
              }
              DS_3GPP_MSG3_HIGH("Rec downlink remote pno:%d,range:%d but prot is:%d",
	                      port_number,port_range,protocol_identifier); 
              port_range = port_range - port_number;
              ip_filter->next_prot_hdr.udp.src.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.src.range = (port_range);
            }
          }/* direction == DOWNLINK */
          if(direction == DS_3GPP_TFT_UPLINK)
          {
            if(protocol_identifier ==(byte)PS_IPPROTO_TCP)
            {
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                DS_3GPP_MSG2_ERROR("Invalid values for port:%d and range:%d",
                              port_number,port_range);
                *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                return DS3G_FAILURE;
              }
              DS_3GPP_MSG2_HIGH("Rec remote port no:%d,range:%d for TCP in Uplink",
			                     port_number,port_range); 
              port_range = port_range - port_number;
              ip_filter->next_prot_hdr.tcp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.tcp.dst.range =(port_range);
              ip_filter->next_prot_hdr.tcp.field_mask |=
                                        (int)IPFLTR_MASK_TCP_DST_PORT;
            }
            else if(protocol_identifier ==(byte)PS_IPPROTO_UDP)
            {
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                DS_3GPP_MSG3_ERROR("Invalid values for port:%d and range:%d",
                              port_number,port_range,0);
                *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                return DS3G_FAILURE;
              }
              DS_3GPP_MSG2_HIGH("Rec remote port no:%d,range:%d for UDP in Uplink",
			                      port_number,port_range); 
              port_range = port_range - port_number;
              ip_filter->next_prot_hdr.udp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.dst.range = (port_range);
              ip_filter->next_prot_hdr.udp.field_mask |=
                                       (int)IPFLTR_MASK_UDP_DST_PORT;
            }
            else
            {
              /*use either TCP or UDP as both are of same format and size*/
              if(!(ARE_PORT_PARAM_VALID(port_number,port_range)))
              {
                DS_3GPP_MSG2_ERROR("Invalid values for port:%d and range:%d",
                              port_number,port_range);
                *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
                return DS3G_FAILURE;
              }
              DS_3GPP_MSG3_HIGH("Rec uplink remote pno:%d,range:%d but prot is:%d",
	                     port_number,port_range,protocol_identifier); 
              port_range = port_range - port_number;
              ip_filter->next_prot_hdr.udp.dst.port = ps_ntohs(port_number);
              ip_filter->next_prot_hdr.udp.dst.range = (port_range);
            }
          } /* direction == UPLINK */
        } /* else filter length*/
      break;
      case DS_3GPP_TFT_TYPE_OF_SRVC:
      //case DS_3GPP_TFT_IP6_TRAFFIC_CLASS:
        if((uint32)filter_mask & (uint32)DS_3GPP_FILTER_TOS_TYPE)
        {
          DS_3GPP_MSG0_ERROR("Received two TOS type filters");
          *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER; 
          return DS3G_FAILURE;
        }
        else
        {
          filter_mask =(ds_3gpp_filter_mask_enum_type)
              ((uint32)filter_mask|(uint32)DS_3GPP_FILTER_TOS_TYPE);
        }
        if(filter_length < (inc + 2))
        {
          DS_3GPP_MSG2_ERROR("Invalid filter data for:%d and filter length:%d",
		                      (int)filter_type,filter_length); 
          *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
          return DS3G_FAILURE;
        }
        else
        {
          if(ip_filter->ip_vsn == IP_V4)
          {
            ip_filter->ip_hdr.v4.field_mask|=(uint32)IPFLTR_MASK_IP4_TOS;
            ip_filter->ip_hdr.v4.tos.val =  filter_contents[inc];
            inc++;
            ip_filter->ip_hdr.v4.tos.mask = filter_contents[inc];
            inc++;
            DS_3GPP_MSG2_HIGH("Received TOS-value 0x%x and TOS-mask 0x%x",
         	  ip_filter->ip_hdr.v4.tos.val,ip_filter->ip_hdr.v4.tos.mask);
          }
          else if(ip_filter->ip_vsn == IP_V6)
          {
            ip_filter->ip_hdr.v6.field_mask|=
                                   (uint32)IPFLTR_MASK_IP6_TRAFFIC_CLASS;
            ip_filter->ip_hdr.v6.trf_cls.val =  filter_contents[inc];
            inc++;
            ip_filter->ip_hdr.v6.trf_cls.mask = filter_contents[inc];
            inc++;
            DS_3GPP_MSG2_HIGH("Received TRF_CLASS-value=%d and TRF_CLASS-mask = %d",
         	                    ip_filter->ip_hdr.v6.trf_cls.val,
                                ip_filter->ip_hdr.v6.trf_cls.mask);
          }
        }/* else filter_length */
      break;
      case DS_3GPP_TFT_SEC_PARAM_IDX: 
        if((uint32)filter_mask & (uint32)DS_3GPP_FILTER_SEC_TYPE)
        {
          DS_3GPP_MSG0_ERROR("Received two SEC type filters");
          *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER; 
          return DS3G_FAILURE;
        }
        else
        {
          filter_mask =(ds_3gpp_filter_mask_enum_type)
               ((uint32)filter_mask|(uint32)DS_3GPP_FILTER_SEC_TYPE);
        }
        if(filter_length < (inc + 4))
        {
          DS_3GPP_MSG2_ERROR("Invalid filter data for:%d and filter length:%d",
		                    (int)filter_type,filter_length); 
          *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
          return DS3G_FAILURE;
        }
        else
        {
          ip_filter->next_prot_hdr.esp.field_mask |=
                                             (uint32)IPFLTR_MASK_ESP_SPI;
          esp_spi =(uint32)(filter_contents[inc++] << 24);
          esp_spi |=(uint32)(filter_contents[inc++] <<16);
          esp_spi |=(uint32)(filter_contents[inc++] <<8);
          esp_spi |=(uint32)(filter_contents[inc++]);
          ip_filter->next_prot_hdr.esp.spi = ps_ntohl(esp_spi);
		  
          DS_3GPP_MSG1_HIGH("Received SEC Param Index-value 0x%x",
         	  ip_filter->next_prot_hdr.esp.spi);
        }
      break;
      case DS_3GPP_TFT_FLOW_LABEL:
        if((uint32)filter_mask & (uint32)DS_3GPP_FILTER_FLOW_LABEL_TYPE)
        {
          DS_3GPP_MSG0_ERROR("Received two TOS type filters");
          *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER; 
          return DS3G_FAILURE;
        }
        else
        {
          filter_mask =(ds_3gpp_filter_mask_enum_type)
              ((uint32)filter_mask|(uint32)DS_3GPP_FILTER_FLOW_LABEL_TYPE);
        }
        if(filter_length < (inc + 3))
        {
          DS_3GPP_MSG2_ERROR("Invalid filter data for:%d and filter length:%d",
		                    (int)filter_type,filter_length); 
          *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
          return DS3G_FAILURE;
        }
        else
        {
          ip_filter->ip_hdr.v6.field_mask|=
                                (uint32)IPFLTR_MASK_IP6_FLOW_LABEL;
          flow_label = (filter_contents[inc])&(0xF);
          flow_label = flow_label<< 8;
          inc++;
          flow_label |= filter_contents[inc];
          flow_label = flow_label << 8;
          inc++;
          flow_label |= filter_contents[inc];        
          inc++;
          ip_filter->ip_hdr.v6.flow_label = flow_label;
          DS_3GPP_MSG1_HIGH("Received V6 FLOW LABEL 0x%x",flow_label);
        }
      	break;
      default:
        *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
        DS_3GPP_MSG3_ERROR("Received unexpected filter type 0x%x filter length 0x%x",
	                   (int)filter_type,filter_length,0); 
        return DS3G_FAILURE;
    }/* switch filter_type */
  }/* while */

  if(( ((uint32)filter_mask & (uint32)DS_3GPP_FILTER_REMOTE_PORT_TYPE)||
       ((uint32)filter_mask & (uint32)DS_3GPP_FILTER_LOCAL_PORT_TYPE) ) &&
       (!((uint32)filter_mask & (uint32)DS_3GPP_FILTER_NXT_HDR_TYPE)) )
  {
    DS_3GPP_MSG0_HIGH("Recieved filter with port number only");
    protocol_identifier = (byte)PS_IPPROTO_TCP_UDP;
    if(ip_filter->ip_vsn == IP_V4)
    {
      ip_filter->ip_hdr.v4.next_hdr_prot = protocol_identifier;
      ip_filter->ip_hdr.v4.field_mask |=(int)IPFLTR_MASK_IP4_NEXT_HDR_PROT;
    }
    if(ip_filter->ip_vsn == IP_V6)
    {
      ip_filter->ip_hdr.v6.next_hdr_prot = protocol_identifier;
      ip_filter->ip_hdr.v6.field_mask |=(int)IPFLTR_MASK_IP6_NEXT_HDR_PROT;
    }
    /* If the port information is sent before the next header*/
    if (((direction == DS_3GPP_TFT_DOWNLINK) &&
         (filter_mask & DS_3GPP_FILTER_REMOTE_PORT_TYPE)) ||
         ((direction == DS_3GPP_TFT_UPLINK) &&
         (filter_mask & DS_3GPP_FILTER_LOCAL_PORT_TYPE)))
    {
      DS_3GPP_MSG0_HIGH("Previously received SRC port information");
      ip_filter->next_prot_hdr.tcp.field_mask |=
                                   (int)IPFLTR_MASK_TCP_UDP_SRC_PORT;
    }
    if (((direction == DS_3GPP_TFT_DOWNLINK) &&
         (filter_mask & DS_3GPP_FILTER_LOCAL_PORT_TYPE)) ||
         ((direction == DS_3GPP_TFT_UPLINK) &&
         (filter_mask & DS_3GPP_FILTER_REMOTE_PORT_TYPE)))
    {
      DS_3GPP_MSG0_HIGH("Previously received DST port information");
      ip_filter->next_prot_hdr.tcp.field_mask |=
                                     (int)IPFLTR_MASK_TCP_UDP_DST_PORT;
    } 
  }/* if port only */

  return DS3G_SUCCESS;
} /* ds_flow_manager_convert_nw_filter_to_app_filter */
/*lint -restore Restore lint error 655*/

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_CONVERT_NW_TFT_TO_APP_TFT

DESCRIPTION
  Converts the NW passed TFT spec to App filter spec.

PARAMETERS
 nw_tft            -  network TFT to be converted
 app_filter        -  App flow to store the converted info.
 ip_type           -  IP type of the PDN.
 flow_context_p    -  Pointer to the flow context

DEPENDENCIES
  None.

RETURN VALUE
boolean          - returns if conversion is successfull
  
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_flow_manager_convert_nw_tft_to_app_tft
(
  tft_type_T      *nw_tft,
  qos_spec_type   *app_filter,
  ip_version_enum_type     ip_type,
  ds_flow_context_s        *flow_context_p,
  ds_3gpp_internal_cause_code_enum_type *icause_code
)
{
  packet_filter_type_T *nw_filter = NULL;
  int index = 0;
  byte filter_id =0;
  byte direction =0;
  ds_tft_pkt_filter_direction_enum_type filter_direction;
  boolean result =DS3G_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG1_HIGH("Convert NW TFT to App TFT for ip_type:%d",(int)ip_type);

  if(nw_tft ==NULL || app_filter == NULL)
  {
    DS_3GPP_MSG0_ERROR("NULL parameters passed");
    *icause_code = DS_3GPP_INTERNAL_INVALID_MANDATORY_INFO;
    return DS3G_FAILURE;
  }

  if(nw_tft->num_filters == 0)
  {
    DS_3GPP_MSG0_ERROR("Number of filter is 0");
    *icause_code = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
    return DS3G_FAILURE;
  }

  for(index=0; index < nw_tft->num_filters; index++)
  {
    /*----------------------------------------------------------------------
     Process Each packet filter
    -----------------------------------------------------------------------*/    
    nw_filter = &(nw_tft->filter_list[index]);
    if(nw_filter->content_length == 0)
    {
      DS_3GPP_MSG0_ERROR("Filter length is 0");
      *icause_code = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
    }
    filter_id = nw_filter->identifier & 0xF;
    direction = ((nw_filter->identifier & 0x30)>>4);
    DS_3GPP_MSG2_HIGH("processing filter with ID:%d and direction 0x%x",
                                                     filter_id,direction);

    filter_direction =(ds_tft_pkt_filter_direction_enum_type)direction; 
    switch(filter_direction)
    {
      case DS_3GPP_TFT_UPLINK:
        result =ds_flow_manager_convert_nw_filter_to_app_filter(
                  nw_filter,
                  &(app_filter->tx.fltr_template.list_ptr[
                            app_filter->tx.fltr_template.num_filters]),
                  filter_id, filter_direction,ip_type, flow_context_p,
                  icause_code);
        if(result == DS3G_FAILURE)
        {
          DS_3GPP_MSG1_ERROR("Error returned for filter No:%d,in Uplink",index);
          return DS3G_FAILURE;
        }
        app_filter->tx.fltr_template.num_filters++;
        app_filter->field_mask |=(qos_spec_field_mask_type)QOS_MASK_TX_FLOW;
      break;

      case DS_3GPP_TFT_DOWNLINK:
      case DS_3GPP_TFT_PRE_REL_7:
        result =ds_flow_manager_convert_nw_filter_to_app_filter(nw_filter,
	        &(app_filter->rx.fltr_template.list_ptr[
                           app_filter->rx.fltr_template.num_filters]),
			filter_id, DS_3GPP_TFT_DOWNLINK, ip_type, flow_context_p,
                        icause_code);
        if(result == DS3G_FAILURE)
        {
          DS_3GPP_MSG1_ERROR("Error returned for filter No:%d,in Downlink",index);
          return DS3G_FAILURE;
        }
        app_filter->rx.fltr_template.num_filters++;
        app_filter->field_mask |=(qos_spec_field_mask_type)QOS_MASK_RX_FLOW;
      break;

      case DS_3GPP_TFT_BIDIRECTIONAL:
        result =ds_flow_manager_convert_nw_filter_to_app_filter(nw_filter,
	        &(app_filter->rx.fltr_template.list_ptr[
                         app_filter->rx.fltr_template.num_filters]),
			filter_id, DS_3GPP_TFT_DOWNLINK, ip_type,flow_context_p,
                        icause_code);
        if(result == DS3G_FAILURE)
        {
          DS_3GPP_MSG1_ERROR("Error returned for filter No:%d,in Downlink",index);
          return DS3G_FAILURE;
        }
        app_filter->rx.fltr_template.num_filters++;
        app_filter->field_mask |=(qos_spec_field_mask_type)QOS_MASK_RX_FLOW;

        result =ds_flow_manager_convert_nw_filter_to_app_filter(nw_filter,
	        &(app_filter->tx.fltr_template.list_ptr[
                        app_filter->tx.fltr_template.num_filters]),
			filter_id, DS_3GPP_TFT_UPLINK, ip_type,flow_context_p,
                        icause_code);
        if(result == DS3G_FAILURE)
        {
          DS_3GPP_MSG1_ERROR("Error returned for filter No:%d,in Uplink",index);
          return DS3G_FAILURE;
        }
        app_filter->tx.fltr_template.num_filters++;
        app_filter->field_mask |=(qos_spec_field_mask_type)QOS_MASK_TX_FLOW;
      break;
      default:
        DS_3GPP_MSG0_ERROR("unexpected direction received");
    }/* switch */   
  } /* for filters */
  return (DS3G_SUCCESS);
} /* ds_flow_manager_convert_nw_tft_to_app_tft */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_CONVERT_APP_FILTER_TO_NW_FILTER

DESCRIPTION
  Convert the App filter spec to NW filter spec.

PARAMETERS
 app_filter            -  App filter to be converted
 nw_filter             -  NW filter to store the converted info.
 direction             -  Direction of the filter
 ip_type               -  IP type for the filters.

DEPENDENCIES
  None.

RETURN VALUE
int                   - returns Non zero value if conversion is successfull
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
static int ds_flow_manager_convert_app_filter_to_nw_filter
(
  ip_filter_type                         *app_filter,
  packet_filter_type_T                   *nw_filter,
  ds_tft_pkt_filter_direction_enum_type   direction
)
{
  int byte_count = 0, loop =0;
  int fltr_len = 0;
  boolean next_hdr_prot = FALSE;
  uint8 temp_mask;
  uint8 ip6_addr_len=16,ip6_prefix_len=0, ip6_prefix_mod=0;
  uint16 temp_portnum, temp_portrng; 
  uint32               temp_address,temp_addr_mask;
  byte                 identifier=0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("Convert APP Filter to NW Filter");
  if(nw_filter ==NULL || app_filter == NULL)
  {
    DS_3GPP_MSG0_ERROR("NULL parameters passed");
    return byte_count;
  }
  /*-----------------------------------------------------------------------
   Update the network packet filter identifier, precedence and direction
  -----------------------------------------------------------------------*/
  nw_filter->eval_precedence =(uint8)app_filter->ipfltr_aux_info.fi_precedence;

  if((direction != DS_3GPP_TFT_UPLINK)&&(direction != DS_3GPP_TFT_DOWNLINK))
  {
    DS_3GPP_MSG0_ERROR("Unexpected Filter Direction");
    return byte_count;
  }
  else
  {
    nw_filter->identifier =(byte)(((byte)direction) <<4);
    nw_filter->direction = (packet_filter_direction_T) direction;
  }
  /*-----------------------------------------------------------------------
   Check if this is new filter or an existing filter.
  -----------------------------------------------------------------------*/
  identifier = (byte)(app_filter->ipfltr_aux_info.fi_id & 0xF);
  if(identifier != 0)
  {
    DS_3GPP_MSG1_HIGH("Translating for existing filter %d",identifier);
    nw_filter->identifier |= identifier;
  }
  /*-----------------------------------------------------------------------
    Convert the packet filter content
  -----------------------------------------------------------------------*/
  if(app_filter->ip_vsn == IP_V4)
  {
    if(direction == DS_3GPP_TFT_DOWNLINK)
    {
      if(app_filter->ip_hdr.v4.field_mask &(int)IPFLTR_MASK_IP4_SRC_ADDR)
      {
        temp_address = ps_ntohl(app_filter->ip_hdr.v4.src.addr.ps_s_addr);
        /*------------------------------------------------------------------
          Set the Packet Filter Identifier Mask:
        ----------------------------------------------------------------- -*/
        nw_filter->filter_contents[fltr_len++]=(int)DS_3GPP_TFT_IPV4_ADDR;
        nw_filter->filter_contents[fltr_len++]=(byte)((temp_address & 0xFF000000)>>24);
        nw_filter->filter_contents[fltr_len++]=(byte)((temp_address & 0x00FF0000)>>16);
        nw_filter->filter_contents[fltr_len++]=(byte)((temp_address & 0x0000FF00)>>8);
        nw_filter->filter_contents[fltr_len++]=(byte)(temp_address & 0x000000FF);
    
        /*---------------------------------------------------------------------
          Set the Packet Filter Address Mask
         ---------------------------------------------------------------------*/
        temp_addr_mask = ps_ntohl(app_filter->ip_hdr.v4.src.subnet_mask.ps_s_addr);
        nw_filter->filter_contents[fltr_len++]=(byte)((temp_addr_mask & 0xFF000000)>>24);
        nw_filter->filter_contents[fltr_len++]=(byte)((temp_addr_mask & 0x00FF0000)>>16);
        nw_filter->filter_contents[fltr_len++]=(byte)((temp_addr_mask & 0x0000FF00)>>8);
        nw_filter->filter_contents[fltr_len++]=(byte)(temp_addr_mask & 0x000000FF);
      } /* IPV4_SRC_ADDR */
    } /* DS_3GPP_TFT_UPLINK */
    else if(direction == DS_3GPP_TFT_UPLINK)
    {
      if(app_filter->ip_hdr.v4.field_mask &(int)IPFLTR_MASK_IP4_DST_ADDR)
      {
        temp_address = ps_ntohl(app_filter->ip_hdr.v4.dst.addr.ps_s_addr);
        /*------------------------------------------------------------------
          Set the Packet Filter Identifier Mask:
        ----------------------------------------------------------------- -*/
        nw_filter->filter_contents[fltr_len++]=(int)DS_3GPP_TFT_IPV4_ADDR;
        nw_filter->filter_contents[fltr_len++]=(byte)((temp_address & 0xFF000000)>>24);
        nw_filter->filter_contents[fltr_len++]=(byte)((temp_address & 0x00FF0000)>>16);
        nw_filter->filter_contents[fltr_len++]=(byte)((temp_address & 0x0000FF00)>>8);
        nw_filter->filter_contents[fltr_len++]=(byte)(temp_address & 0x000000FF);
    
        /*---------------------------------------------------------------------
          Set the Packet Filter Address Mask
         ---------------------------------------------------------------------*/
        temp_addr_mask = ps_ntohl(app_filter->ip_hdr.v4.dst.subnet_mask.ps_s_addr);
        nw_filter->filter_contents[fltr_len++]=(byte)((temp_addr_mask & 0xFF000000)>>24);
        nw_filter->filter_contents[fltr_len++]=(byte)((temp_addr_mask & 0x00FF0000)>>16);
        nw_filter->filter_contents[fltr_len++]=(byte)((temp_addr_mask & 0x0000FF00)>>8);
        nw_filter->filter_contents[fltr_len++]=(byte)(temp_addr_mask & 0x000000FF);
      } /* IPV4_DST_ADDR */
    } /* DS_3GPP_TFT_DOWNLINK */   
    /*-----------------------------------------------------------------------
     Set the Packet Filter next header protocol
    -----------------------------------------------------------------------*/
    if(app_filter->ip_hdr.v4.field_mask & (int)IPFLTR_MASK_IP4_NEXT_HDR_PROT)
    {
      /*---------------------------------------------------------------------
        Set the Packet Filter Identifier Mask : Next Header Type
      ---------------------------------------------------------------------*/
      nw_filter->filter_contents[fltr_len++]=(int)DS_3GPP_TFT_NEXT_HDR_TYPE;
      nw_filter->filter_contents[fltr_len++]= app_filter->ip_hdr.v4.next_hdr_prot; 
      next_hdr_prot = TRUE;
    }/* NEXT HDR PROT */

    /*----------------------------------------------------------------------- 
      Set the Packet Filter type of service: value and mask
    -----------------------------------------------------------------------*/
    if (app_filter->ip_hdr.v4.field_mask & (int)IPFLTR_MASK_IP4_TOS )
    {
      /*--------------------------------------------------------------------- 
        Set the Packet Filter Identifier Mask : Type of Service 
      ---------------------------------------------------------------------*/
      nw_filter->filter_contents[fltr_len++] = (int)DS_3GPP_TFT_TYPE_OF_SRVC;
      nw_filter->filter_contents[fltr_len++] = app_filter->ip_hdr.v4.tos.val;
      nw_filter->filter_contents[fltr_len++] = app_filter->ip_hdr.v4.tos.mask;

      DS_3GPP_MSG3_HIGH ( " Filter len 0x%x : TOS VAL 0x%x, TOS MASK 0x%x",
                 fltr_len, app_filter->ip_hdr.v4.tos.val,
                 app_filter->ip_hdr.v4.tos.mask);
    }
  } /* case ip_type == IP_V4*/   

  else if(app_filter->ip_vsn == IP_V6)
  {
    if(direction == DS_3GPP_TFT_DOWNLINK)
    {
      /*----------------------------------------------------------------------- 
        Set the Packet Filter IPV6 Source Address and Mask 
      -----------------------------------------------------------------------*/
      if (app_filter->ip_hdr.v6.field_mask & (int)IPFLTR_MASK_IP6_SRC_ADDR )
      { 
        nw_filter->filter_contents[fltr_len++] = (int)DS_3GPP_TFT_IPV6_ADDR;
        /*------------------------------------------------------------------- 
          Set the Packet Filter Identifier Mask : IPV6 Addr 
        ------------------------------------------------------------------*/
        for(loop = 0; loop < ip6_addr_len; loop++)    
        {
          nw_filter->filter_contents[fltr_len++] = 
          app_filter->ip_hdr.v6.src.addr.in6_u.u6_addr8[loop];
        }
        /*------------------------------------------------------------------ 
          Set the Packet Filter Address Mask 
        ------------------------------------------------------------------*/
        ip6_prefix_len = app_filter->ip_hdr.v6.src.prefix_len;
        ip6_prefix_mod  = ip6_prefix_len%8;
        /*------------------------------------------------------------------ 
          Set the bits for ip6_prefix_len
        ------------------------------------------------------------------*/
        for(loop = 0; loop < ip6_prefix_len /8; loop++)    
        { 
          nw_filter->filter_contents[fltr_len++] = 0xFF;
        }
        /*------------------------------------------------------------------ 
          Initialize the byte 
        ------------------------------------------------------------------*/
        if(ip6_prefix_mod != 0)
        {
          nw_filter->filter_contents[fltr_len] = 0;
          temp_mask = (1 << 7);
          while(ip6_prefix_mod != 0)
          {
            nw_filter->filter_contents[fltr_len] |= temp_mask;
            ip6_prefix_mod--;          
            temp_mask = temp_mask >> 1;
          }
          if(nw_filter->filter_contents[fltr_len] != 0xFF)
          {
            fltr_len++;
	    loop++;
          }
        }
        /*------------------------------------------------------------------ 
          Set the rest of the bytes 
        ------------------------------------------------------------------*/
        for(; loop < 16; loop++)    
        {
          nw_filter->filter_contents[fltr_len++] = 0x00;
        }
        DS_3GPP_MSG2_HIGH ( " Filter Len %d : SRC ADDR 0x%x ",
                   fltr_len, app_filter->ip_hdr.v6.src.addr.in6_u.u6_addr32);
        DS_3GPP_MSG2_HIGH ( " Filter  Len%d : SNET MSK 0x%x ",
                fltr_len,app_filter->ip_hdr.v6.src.prefix_len); 
      } /* address */
    } /* DS_3GPP_TFT_DOWNLINK*/
    else if(direction == DS_3GPP_TFT_UPLINK)
    {
      /*----------------------------------------------------------------------- 
        Set the Packet Filter IPV6 Source Address and Mask 
      -----------------------------------------------------------------------*/
      if (app_filter->ip_hdr.v6.field_mask & (int)IPFLTR_MASK_IP6_DST_ADDR )
      { 
        nw_filter->filter_contents[fltr_len++] = (int)DS_3GPP_TFT_IPV6_ADDR;
        /*------------------------------------------------------------------- 
          Set the Packet Filter Identifier Mask : IPV6 Addr 
        ------------------------------------------------------------------*/
        for(loop = 0; loop < ip6_addr_len; loop++)    
        {
          nw_filter->filter_contents[fltr_len++] = 
          app_filter->ip_hdr.v6.dst.addr.in6_u.u6_addr8[loop];
        }
        /*------------------------------------------------------------------ 
          Set the Packet Filter Address Mask 
        ------------------------------------------------------------------*/
        ip6_prefix_len = app_filter->ip_hdr.v6.dst.prefix_len;
        ip6_prefix_mod  = ip6_prefix_len%8;
        /*------------------------------------------------------------------ 
          Set the bits for ip6_prefix_len
        ------------------------------------------------------------------*/
        for(loop = 0; loop < ip6_prefix_len /8; loop++)    
        { 
          nw_filter->filter_contents[fltr_len++] = 0xFF;
        }
        /*------------------------------------------------------------------ 
          Initialize the byte 
        ------------------------------------------------------------------*/
        if(ip6_prefix_mod != 0)
        {
          nw_filter->filter_contents[fltr_len] = 0;
          temp_mask = (1 << 7);
          while(ip6_prefix_mod != 0)
          {
            nw_filter->filter_contents[fltr_len] |= temp_mask;
            ip6_prefix_mod--;          
            temp_mask = temp_mask >> 1;       
          }
          if(nw_filter->filter_contents[fltr_len] != 0xFF)
          {
            fltr_len++;
            loop++;
          }
        }
        /*------------------------------------------------------------------ 
          Set the rest of the bytes 
        ------------------------------------------------------------------*/
        for(; loop < 16; loop++)    
        {
          nw_filter->filter_contents[fltr_len++] = 0x00;
        }     
        DS_3GPP_MSG2_HIGH ( " Filter Len %d : DST ADDR : %d ",
                   fltr_len, app_filter->ip_hdr.v6.dst.addr.in6_u.u6_addr32);
        DS_3GPP_MSG2_HIGH ( " Filter  Len%d : DNET MSK : %d ",
                fltr_len,app_filter->ip_hdr.v6.dst.prefix_len); 
      } /* address */
    } /* DS_3GPP_TFT_DOWNLINK*/
    
    /*-----------------------------------------------------------------------
      Set the Packet Filter next header protocol 
    -----------------------------------------------------------------------*/
    if(app_filter->ip_hdr.v6.field_mask & (int)IPFLTR_MASK_IP6_NEXT_HDR_PROT )
    {   
      /*--------------------------------------------------------------------- 
        Set the Packet Filter Identifier Mask : Next Header Type 
      ---------------------------------------------------------------------*/
      nw_filter->filter_contents[fltr_len++] = (int)DS_3GPP_TFT_NEXT_HDR_TYPE;
      nw_filter->filter_contents[fltr_len++] = app_filter->ip_hdr.v6.next_hdr_prot;
      DS_3GPP_MSG2_HIGH ( " Filter len %d : NXT HDR PROTOCOL : %d ",
                 fltr_len,app_filter->ip_hdr.v6.next_hdr_prot);
      next_hdr_prot = TRUE;
    } /* Next header */

    /*----------------------------------------------------------------------- 
      Set the Packet Filter Security Parameter Index (Supported in IPV6) 
    -----------------------------------------------------------------------*/

    /*----------------------------------------------------------------------- 
      Set the Packet Filter type of service: value and mask
    -----------------------------------------------------------------------*/
    if (app_filter->ip_hdr.v6.field_mask & (int)IPFLTR_MASK_IP6_TRAFFIC_CLASS )
    {
      /*--------------------------------------------------------------------- 
        Set the Packet Filter Identifier Mask : Type of Service 
      ---------------------------------------------------------------------*/
      nw_filter->filter_contents[fltr_len++] = (int)DS_3GPP_TFT_IP6_TRAFFIC_CLASS;
      nw_filter->filter_contents[fltr_len++] = app_filter->ip_hdr.v6.trf_cls.val;
      nw_filter->filter_contents[fltr_len++] = app_filter->ip_hdr.v6.trf_cls.mask;
      DS_3GPP_MSG3_HIGH ( " Filter len %d : TOS VAL: %d, TOS MASK: %d ",
             fltr_len, app_filter->ip_hdr.v6.trf_cls.val, 
                 app_filter->ip_hdr.v6.trf_cls.mask);
    } /* traffic class */

    /*----------------------------------------------------------------------- 
      Set the Packet Filter Flow Label (Supported in IPV6) 
    -----------------------------------------------------------------------*/
    
    if (app_filter->ip_hdr.v6.field_mask & (int)IPFLTR_MASK_IP6_FLOW_LABEL )
    {
      /*--------------------------------------------------------------------- 
        Set the Packet Filter Identifier Mask : Flow Label 
      ---------------------------------------------------------------------*/
      nw_filter->filter_contents[fltr_len++] = (int)DS_3GPP_TFT_FLOW_LABEL;

      nw_filter->filter_contents[fltr_len++] = 
            (byte) ((app_filter->ip_hdr.v6.flow_label & 0x000F0000) >> 16);
      nw_filter->filter_contents[fltr_len++] = 
            (byte) ((app_filter->ip_hdr.v6.flow_label & 0x0000FF00) >> 8);
      nw_filter->filter_contents[fltr_len++] = 
            (byte) ((app_filter->ip_hdr.v6.flow_label & 0x000000FF));

      DS_3GPP_MSG3_HIGH ( " Filter len %d : Flow Label %d ", fltr_len,
                     app_filter->ip_hdr.v6.flow_label & 0x000FFFFF,0);
    } /* Flow Label */
  } /* ip_type == IP_V6 */
  if(next_hdr_prot == TRUE)
  {
    /*--------------------------------------------------------------------- 
      Set the Packet Filter destination and Source port range 
    ---------------------------------------------------------------------*/
    if ((app_filter->ip_hdr.v4.next_hdr_prot == (int)PS_IPPROTO_TCP ) ||
        (app_filter->ip_hdr.v6.next_hdr_prot == (int)PS_IPPROTO_TCP ))
    {
      /*------------------------------------------------------------------- 
        TCP destination Port has been set 
      -------------------------------------------------------------------*/
      if (app_filter->next_prot_hdr.tcp.field_mask&(int)IPFLTR_MASK_TCP_DST_PORT)
      {
        if (app_filter->next_prot_hdr.tcp.dst.range == 0 )
        {
          temp_portnum =0;
          /*--------------------------------------------------------------- 
            Set the Packet Filter Identifier Mask : Single Dest Port 
          ---------------------------------------------------------------*/
          if(direction == DS_3GPP_TFT_DOWNLINK)
          {
            nw_filter->filter_contents[fltr_len++]=(int)DS_3GPP_TFT_SINGLE_LOCAL_PORT;
          }
          else if(direction == DS_3GPP_TFT_UPLINK)
          {
            nw_filter->filter_contents[fltr_len++]=(int)DS_3GPP_TFT_SINGLE_REMOTE_PORT;
          }
          temp_portnum = ps_ntohs(app_filter->next_prot_hdr.tcp.dst.port);
          nw_filter->filter_contents[fltr_len++] = (temp_portnum & 0xFF00)>>8;
          nw_filter->filter_contents[fltr_len++] = (temp_portnum & 0x00FF);
          DS_3GPP_MSG2_HIGH ("Filter len %d:SINGLE TCP DEST PORT:%d",fltr_len,temp_portnum);
        }
        else
        {
          /*---------------------------------------------------------------
            Set the Packet Filter Identifier Mask : Dest Port Range Type 
          ---------------------------------------------------------------*/
          temp_portnum = 0;
          temp_portrng = 0;       
          if(direction == DS_3GPP_TFT_DOWNLINK)
          {
            nw_filter->filter_contents[fltr_len++]=(int)DS_3GPP_TFT_LOCAL_PORT_RANGE;
          }
          else if(direction == DS_3GPP_TFT_UPLINK)
          {
            nw_filter->filter_contents[fltr_len++]=(int)DS_3GPP_TFT_REMOTE_PORT_RANGE;
          }
          temp_portnum = ps_ntohs(app_filter->next_prot_hdr.tcp.dst.port );
          temp_portrng = app_filter->next_prot_hdr.tcp.dst.range;   
          nw_filter->filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;
          nw_filter->filter_contents[fltr_len++] = (temp_portnum & 0x00FF);
          nw_filter->filter_contents[fltr_len++] = ((temp_portnum + temp_portrng) & 0xFF00) >> 8;
          nw_filter->filter_contents[fltr_len++] = ( ( temp_portnum + temp_portrng ) & 0x00FF);
          DS_3GPP_MSG3_HIGH ( " Filter Len %d : TCP DEST PORT RANGE: %d, %d ",
                     fltr_len, temp_portnum, temp_portnum+temp_portrng);
        }
      } /* if (DST PORT) */
      /*------------------------------------------------------------------- 
        TCP source Port has been set 
      -------------------------------------------------------------------*/
      if (app_filter->next_prot_hdr.tcp.field_mask & (int)IPFLTR_MASK_TCP_SRC_PORT )
      {
        if (app_filter->next_prot_hdr.tcp.src.range == 0 )
        {
          /*--------------------------------------------------------------- 
            Set the Packet Filter Identifier Mask : Single Source Port 
          ---------------------------------------------------------------*/
          temp_portnum = 0;
          if(direction == DS_3GPP_TFT_DOWNLINK)
          {
            nw_filter->filter_contents[fltr_len++]=(int)DS_3GPP_TFT_SINGLE_REMOTE_PORT;
          }
          else if(direction == DS_3GPP_TFT_UPLINK)
          {
            nw_filter->filter_contents[fltr_len++]=(int)DS_3GPP_TFT_SINGLE_LOCAL_PORT;
          }
          temp_portnum = ps_ntohs(app_filter->next_prot_hdr.tcp.src.port );
          nw_filter->filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;
          nw_filter->filter_contents[fltr_len++] = (temp_portnum & 0x00FF);
          DS_3GPP_MSG3_HIGH ( " Filter Len %d : SINGLE TCP SRC PORT: %d ",
                       fltr_len, temp_portnum, 0);
        }
        else
        {
          /*--------------------------------------------------------------- 
            Set the Packet Filter Identifier Mask : Source Port Range 
          ---------------------------------------------------------------*/
          temp_portnum = 0;
          temp_portrng = 0;
          if(direction == DS_3GPP_TFT_DOWNLINK)
          {
            nw_filter->filter_contents[fltr_len++]=(int)DS_3GPP_TFT_REMOTE_PORT_RANGE;
          }
          else if(direction == DS_3GPP_TFT_UPLINK)
          {
            nw_filter->filter_contents[fltr_len++]=(int)DS_3GPP_TFT_LOCAL_PORT_RANGE;
          }
          temp_portnum = ps_ntohs(app_filter->next_prot_hdr.tcp.src.port );
          temp_portrng = app_filter->next_prot_hdr.tcp.src.range;
          nw_filter->filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;
          nw_filter->filter_contents[fltr_len++] = (temp_portnum & 0x00FF);
          nw_filter->filter_contents[fltr_len++] =
            ( ( temp_portnum + temp_portrng) & 0xFF00) >> 8;
          nw_filter->filter_contents[fltr_len++] =
            ( ( temp_portnum + temp_portrng) & 0x00FF);
          DS_3GPP_MSG3_HIGH ( " Filter Len %d : TCP SRC PORT RANGE: %d, %d ",
                     fltr_len, temp_portnum, temp_portnum+temp_portrng);
        }
      }
    }
    else if ((app_filter->ip_hdr.v4.next_hdr_prot == (int)PS_IPPROTO_UDP ) ||
             (app_filter->ip_hdr.v6.next_hdr_prot == (int)PS_IPPROTO_UDP ))
    {
      /*------------------------------------------------------------------- 
        UDP destination Port has been set 
      -------------------------------------------------------------------*/
      if (app_filter->next_prot_hdr.udp.field_mask & (int)IPFLTR_MASK_UDP_DST_PORT )
      {
        if (app_filter->next_prot_hdr.udp.dst.range == 0 )
        {
          /*--------------------------------------------------------------- 
            Set the Packet Filter Identifier Mask : Single Dest Port 
          ---------------------------------------------------------------*/
          temp_portnum = 0;
          if(direction == DS_3GPP_TFT_DOWNLINK)
          {
            nw_filter->filter_contents[fltr_len++]=(int)DS_3GPP_TFT_SINGLE_LOCAL_PORT;
          }
          else if(direction == DS_3GPP_TFT_UPLINK)
          {
            nw_filter->filter_contents[fltr_len++]=(int)DS_3GPP_TFT_SINGLE_REMOTE_PORT;
          }
          temp_portnum = ps_ntohs(app_filter->next_prot_hdr.udp.dst.port );
          nw_filter->filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;
          nw_filter->filter_contents[fltr_len++] = (temp_portnum & 0x00FF);
          DS_3GPP_MSG3_HIGH ( " Filter Len %d : SINGLE UDP DEST PORT : %d ",
                    fltr_len, temp_portnum, 0);
        }
        else
        {
          /*--------------------------------------------------------------- 
            Set the Packet Filter Identifier Mask : Dest Port Range Type 
          ---------------------------------------------------------------*/
          temp_portnum = 0;
          temp_portrng = 0;
          if(direction == DS_3GPP_TFT_DOWNLINK)
          {
            nw_filter->filter_contents[fltr_len++]=(int)DS_3GPP_TFT_LOCAL_PORT_RANGE;
          }
          else if(direction == DS_3GPP_TFT_UPLINK)
          {
            nw_filter->filter_contents[fltr_len++]=(int)DS_3GPP_TFT_REMOTE_PORT_RANGE;
          }
          temp_portnum = ps_ntohs( app_filter->next_prot_hdr.udp.dst.port );
          temp_portrng = app_filter->next_prot_hdr.udp.dst.range;
          nw_filter->filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;
          nw_filter->filter_contents[fltr_len++] = (temp_portnum & 0x00FF);
          nw_filter->filter_contents[fltr_len++] =
             ( ( temp_portnum + temp_portrng) & 0xFF00) >> 8;
          nw_filter->filter_contents[fltr_len++] =
              ( ( temp_portnum + temp_portrng) & 0x00FF);
          DS_3GPP_MSG3_HIGH ( " Filter Len %d : UDP DEST PORT RANGE: %d, %d ",
                       fltr_len, temp_portnum, temp_portnum+temp_portrng);
        }
      }

      /*------------------------------------------------------------------- 
        UDP Source Port has been set 
      -------------------------------------------------------------------*/
      if (app_filter->next_prot_hdr.udp.field_mask & (int)IPFLTR_MASK_UDP_SRC_PORT )
      {
        if (app_filter->next_prot_hdr.udp.src.range == 0 )
        {
          /*--------------------------------------------------------------- 
            Set the Packet Filter Identifier Mask : Single Source Port 
          ---------------------------------------------------------------*/
          temp_portnum = 0;
          if(direction == DS_3GPP_TFT_DOWNLINK)
          {
            nw_filter->filter_contents[fltr_len++]=(int)DS_3GPP_TFT_SINGLE_REMOTE_PORT;
          }
          else if(direction == DS_3GPP_TFT_UPLINK)
          {
            nw_filter->filter_contents[fltr_len++]=(int)DS_3GPP_TFT_SINGLE_LOCAL_PORT;
          }
          temp_portnum = ps_ntohs(app_filter->next_prot_hdr.udp.src.port );
          nw_filter->filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;
          nw_filter->filter_contents[fltr_len++] = (temp_portnum & 0x00FF);
          DS_3GPP_MSG2_HIGH ( " Filter len %d : SINGLE UDP SRC PORT: %d ",
                       fltr_len, temp_portnum);
        }
        else
        {
          /*--------------------------------------------------------------- 
            Set the Packet Filter Identifier Mask : Source Port Range Type 
          ---------------------------------------------------------------*/
          temp_portnum = 0;
          temp_portrng = 0;
          if(direction == DS_3GPP_TFT_DOWNLINK)
          {
            nw_filter->filter_contents[fltr_len++]=(int)DS_3GPP_TFT_REMOTE_PORT_RANGE;
          }
          else if(direction == DS_3GPP_TFT_UPLINK)
          {
            nw_filter->filter_contents[fltr_len++]=(int)DS_3GPP_TFT_LOCAL_PORT_RANGE;
          }
          temp_portnum = ps_ntohs(app_filter->next_prot_hdr.udp.src.port );
          temp_portrng = app_filter->next_prot_hdr.udp.src.range;
          nw_filter->filter_contents[fltr_len++] = (temp_portnum & 0xFF00) >> 8 ;
          nw_filter->filter_contents[fltr_len++] = (temp_portnum & 0x00FF);
          nw_filter->filter_contents[fltr_len++] =
            ((temp_portnum + temp_portrng) & 0xFF00) >> 8;
          nw_filter->filter_contents[fltr_len++] =
            ((temp_portnum + temp_portrng) & 0x00FF);
           DS_3GPP_MSG3_HIGH ( " Filter Len %d : UDP SRC PORT RANGE: %d, %d ",
                       fltr_len, temp_portnum, temp_portnum+temp_portrng);
        }
      }  
    } /* if(PS_IPPROTO_UDP) */
  } /* IF (next_hdr_prot == TRUE) */
  ASSERT( fltr_len < MAX_FILTER_CONTENT_LENGTH );

  DS_3GPP_MSG1_HIGH ( " Filter Component Length : %d ", fltr_len);
  nw_filter->content_length = fltr_len;
  /* +3 bytes for the size of other filter components*/
  byte_count = fltr_len + 3;
  return byte_count;
} /* ds_flow_manager_convert_app_filter_to_nw_filter */


/*===========================================================================
FUNCTION DS_FLOW_MANAGER_CONVERT_APP_TFT_TO_NW_TFT

DESCRIPTION
  Converts the APP passed TFT spec to NW filter spec.

PARAMETERS
 nw_tft            -  network TFT to be converted
 app_filter        -  App flow to store the converted info.

DEPENDENCIES
  None.

RETURN VALUE
boolean          - returns if conversion is successfull
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_convert_app_tft_to_nw_tft
(
  tft_type_T      *nw_tft,
  qos_spec_type   *app_filter_spec
)
{
  packet_filter_type_T *nw_filter = NULL;
  int cnt = 0;
  ds_tft_pkt_filter_direction_enum_type filter_direction;
  int num_filters = 0;
  int tft_len = 0, fltr_len = 0;
  ip_filter_type *inp_filter = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG0_HIGH("Convert NW TFT to App TFT");
  if(nw_tft ==NULL || app_filter_spec == NULL)
  {
    DS_3GPP_MSG0_ERROR("NULL parameters passed");
    return FALSE;
  }

  /*-----------------------------------------------------------------------
    Translate RX 
  -----------------------------------------------------------------------*/    
  num_filters = app_filter_spec->rx.fltr_template.num_filters;
  filter_direction = DS_3GPP_TFT_DOWNLINK;
  for(cnt =0; cnt< num_filters; cnt++)
  {
    inp_filter = &(app_filter_spec->rx.fltr_template.list_ptr[cnt]);
    nw_filter = &(nw_tft->filter_list[nw_tft->num_filters]);
    /*---------------------------------------------------------------------
       Translate each filter.
     ---------------------------------------------------------------------*/
    fltr_len = ds_flow_manager_convert_app_filter_to_nw_filter(
                                   inp_filter,nw_filter,filter_direction);
    if (fltr_len > 0)
    {
      tft_len = tft_len + fltr_len;
      nw_tft->num_filters++;      
    }
    else
    {
      DS_3GPP_MSG1_ERROR("Wrong filter params for filter num:%d",nw_tft->num_filters);
      return FALSE;
    }
  }/* for RX*/  
  /*-----------------------------------------------------------------------
    Translate TX filters.
  -----------------------------------------------------------------------*/    
  num_filters = app_filter_spec->tx.fltr_template.num_filters;
  filter_direction = DS_3GPP_TFT_UPLINK;
  for(cnt =0; cnt < num_filters; cnt++)
  {
    inp_filter = &(app_filter_spec->tx.fltr_template.list_ptr[cnt]);
    nw_filter = &(nw_tft->filter_list[nw_tft->num_filters]);
    /*---------------------------------------------------------------------
      Translate each filter.
    ---------------------------------------------------------------------*/
    fltr_len = ds_flow_manager_convert_app_filter_to_nw_filter(
                            inp_filter,nw_filter,filter_direction);
     if (fltr_len > 0)
     {
       tft_len = tft_len + fltr_len;
       nw_tft->num_filters++;      
     }
     else
     {
       DS_3GPP_MSG1_ERROR("Wrong filter params for filter num:%d",nw_tft->num_filters);
       return FALSE;
     }
  }/* for  TX */
  /*---------------------------------------------------------------------
     Update TFT Length,other params
  ---------------------------------------------------------------------*/
  nw_tft->valid = TRUE;
  nw_tft->tft_total_length = tft_len + 1;
  nw_tft->operation_code =(byte) DS_3GPP_TFT_OPCODE_CREATE;
  nw_tft->e_bit = DS_FLOW_E_BIT_DISABLE;
  return TRUE;
} /* ds_flow_manager_convert_app_tft_to_nw_tft */


/*===========================================================================
FUNCTION DS_FLOW_MANAGER_UPDATE_APP_FILTERS_TO_DELETE

DESCRIPTION
  Update the NW TFT with the filters to be deleted.

PARAMETERS
 nw_tft            -  network TFT to be converted
 flow_context_p    -  App flow to store the converted info.

DEPENDENCIES
  None.

RETURN VALUE
boolean          - returns TRUE if update is successfull
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_update_app_filter_to_delete
(
  tft_type_T          *nw_tft,
  ds_flow_context_s   *flow_context_p
)
{
  qos_spec_type           orig_spec;  
  ip_filter_type          *fltr = NULL; 
  int                     cnt = 0;          
  uint8                   fi_identifier = 0;
  uint16                  temp_fi_id =0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return FALSE;
  }

  if(nw_tft == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input NW TFT Pointer is NULL");
    return FALSE;
  }

  memset(&orig_spec, 0, sizeof(qos_spec_type));

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
     Loop thru all filters and update the NW TFT
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt<orig_spec.rx.fltr_template.num_filters; cnt++)
  {
    fltr = &(orig_spec.rx.fltr_template.list_ptr[cnt]);
    fi_identifier = (uint8)(fltr->ipfltr_aux_info.fi_id);
    /* Filter identifier is in first 4 bits.*/
    fi_identifier = fi_identifier & 0xF;
    temp_fi_id = fi_identifier;
    if(ds_3gpp_tft_is_filter_identifier_requested(
                                      temp_fi_id,
           &(flow_context_p->ds_flow_context_dyn_p->ignore_fi_id))==FALSE)
    {
      nw_tft->filter_list[nw_tft->num_filters].identifier = fi_identifier;
      nw_tft->num_filters= nw_tft->num_filters + 1;
    }
    else
    {
      // clear the precedence
      // should be done before we post the command.
    }
  }/* RX FOR Loop*/

  /*------------------------------------------------------------------------
     Loop thru all filters and update the precedence bit mask for TX
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt<orig_spec.tx.fltr_template.num_filters; cnt++)
  {
    fltr = &(orig_spec.tx.fltr_template.list_ptr[cnt]);
    fi_identifier = (uint8)(fltr->ipfltr_aux_info.fi_id);
    // Filter identifier is in first 4 bits.
    fi_identifier = fi_identifier & 0xF;
    temp_fi_id = fi_identifier;
    if(ds_3gpp_tft_is_filter_identifier_requested(
                                      temp_fi_id,
          &(flow_context_p->ds_flow_context_dyn_p->ignore_fi_id))== FALSE)
    {
      nw_tft->filter_list[nw_tft->num_filters].identifier = fi_identifier;
      nw_tft->num_filters= nw_tft->num_filters + 1;    
    }
  }/* FOR TX  Filters */

  
  modem_mem_free((void *)orig_spec.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
  modem_mem_free((void *)orig_spec.tx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);

  return TRUE;
} /*  ds_flow_manager_update_app_filter_to_delete */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_CREATE_FLOW_CONTEXT_PER_IFACE

DESCRIPTION
  Create a new flow context.

PARAMETERS
  pdn_context_p       -  PDN information
  bearer_context_p    -  Bearer information
  mode_info_p         -  mode specific call info.try to reuse for all modes.
  nw_tft_p            -  Pointer to TFT in network format. 
  ds_iface_p          -  Information about the 3GPP IFace.
  ip_type             -  Indicate if it is a V4 or V6 iface.
  icause_code         -  internal cause code to be updated in case of failure.
  is_bearer_activated -  boolean to indicate if bearer is already activated.
                         if bearer is not activated, flow cntx needs to be saved
                         in pdn context as a pending flow (this is NW init qos
                         for UMTS case)
DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_create_flow_context_per_iface
(
  ds_pdn_context_s                *pdn_context_p,
  ds_bearer_context_s             *bearer_context_p,
  const cm_call_mode_info_s_type  *mode_info_p,
  tft_type_T                      *nw_tft_p,
  ds_3gpp_iface_s                 *ds_iface,
  ip_version_enum_type            ip_type,
  ds_3gpp_internal_cause_code_enum_type *icause_code,
  boolean                          is_bearer_activated
)
{ 
  ps_flow_create_param_type   ps_flow_param;
  qos_spec_type               ps_qos_spec; 
  int16                       ds_errno = 0;
  ds_flow_context_s           *flow_cntx_p = NULL;
  boolean                     result = DS3G_SUCCESS;  
  ps_iface_type               *iface_ptr = NULL;
  int                         ret_val = 0;
  ds_tft_opcode_enum_type     tft_opcode = DS_3GPP_TFT_OPCODE_CREATE;
  ds_pdn_context_s            *pdn_context_ptr = NULL;
  ds_bearer_flow_manager_s    *bflow_manager_p;
  
   
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG2_HIGH("Create Flow Context for PDN:0x%x, bearer:0x%x",
                                             pdn_context_p,bearer_context_p);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }

  memset(&ps_flow_param, 0, sizeof(ps_flow_create_param_type));
  memset(&ps_qos_spec, 0, sizeof(qos_spec_type));  

  /*------------------------------------------------------------------------
    Convert NW Flow values to PS-Specific Qos value
    The conversion should be done only once. Should copy the value
    for the other flows.. *** Do this after stabilizing ***
  -------------------------------------------------------------------------*/
  if(ds_bearer_flow_manager_convert_data_rate(bearer_context_p,
                                       mode_info_p,
                                       &ps_qos_spec,
                                       icause_code) == DS3G_FAILURE)
  {
    return DS3G_FAILURE;
  }
  /*-------------------------------------------------------------------------
    Retrieve an empty flow context.
  -------------------------------------------------------------------------*/
  flow_cntx_p = ds_flow_context_allocate_flow_context();
  if(!ds_flow_context_validate_flow(flow_cntx_p))
  {
    *icause_code = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    return DS3G_FAILURE;
  }
  else
  {
    DS_3GPP_MSG0_HIGH("Allocated a free flow context");
    /*---------------------------------------------------------------------
       Find the bearer flow manager.
    ---------------------------------------------------------------------*/ 
    bflow_manager_p = 
              ds_bearer_flow_manager_find_flow_manager(bearer_context_p);

    if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
    { 
      ds_flow_context_release_flow_context(flow_cntx_p);
      *icause_code = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
      return DS3G_FAILURE;
    }

    flow_cntx_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr = NULL;
    if(ip_type == IP_V4)
    {
      flow_cntx_p->ds_flow_context_dyn_p->ds_iface_ptr = ds_iface;
      flow_cntx_p->ds_flow_context_dyn_p->flow_type = 
             ((DS_3GPP_NW_FLOW_TYPE)|(DS_3GPP_V4_FLOW_TYPE));
    }
    else if(ip_type == IP_V6)
    {
      flow_cntx_p->ds_flow_context_dyn_p->ds_iface_ptr = ds_iface;
      flow_cntx_p->ds_flow_context_dyn_p->flow_type = 
             ((DS_3GPP_NW_FLOW_TYPE)|(DS_3GPP_V6_FLOW_TYPE));
    }
    else
    {
      DS_3GPP_MSG1_ERROR("unsupported IP_TYPE:%d",ip_type);
      ds_flow_context_release_flow_context(flow_cntx_p);
      *icause_code = DS_3GPP_INTERNAL_ACTIVATION_REJECTED;
      return DS3G_FAILURE;
    }
  }

  /*-------------------------------------------------------------------------
    Verify Filter Fields 
  -------------------------------------------------------------------------*/
  if(nw_tft_p->valid)
  {
    if((nw_tft_p->tft_total_length < 3) ||
  	  (nw_tft_p->tft_total_length > 257))
    {
      DS_3GPP_MSG1_ERROR("Received TFT with invalid length=%d",
  		      nw_tft_p->tft_total_length);
      *icause_code = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
      ds_flow_context_release_flow_context(flow_cntx_p);
      return DS3G_FAILURE;
    }
  
    if(nw_tft_p->e_bit != DS_FLOW_E_BIT_DISABLE)
    {
      DS_3GPP_MSG0_ERROR("Received TFT with E bit set");
      ds_flow_context_release_flow_context(flow_cntx_p);
      *icause_code = DS_3GPP_INTERNAL_SERV_OPT_NOT_SUPPORTED;
      return DS3G_FAILURE;
    }
  
    if(nw_tft_p->operation_code != (byte)tft_opcode)
    {
      DS_3GPP_MSG1_ERROR("Received Unsupported TFT operation:%d",
  		               nw_tft_p->operation_code);
      ds_flow_context_release_flow_context(flow_cntx_p);
      *icause_code = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_TFT;
      return DS3G_FAILURE;
    }
   
    if((nw_tft_p->num_filters > DS_TFT_MAX_FILTERS)||
  	  (nw_tft_p->num_filters < 1))
    {
      DS_3GPP_MSG1_ERROR("Received Invalid number of filters:%d",
  		               nw_tft_p->num_filters);
      ds_flow_context_release_flow_context(flow_cntx_p);
      *icause_code = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
      return DS3G_FAILURE;
    }
   
    /*------------------------------------------------------------------------
       Process the Filters 
    -------------------------------------------------------------------------*/    
    ps_qos_spec.rx.fltr_template.list_ptr = 
        (ip_filter_type *)modem_mem_calloc(MAX_PACKET_FILTERS,
                                           sizeof(ip_filter_type),
                                           MODEM_MEM_CLIENT_DATA);
    if(ps_qos_spec.rx.fltr_template.list_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("Heap Memory Allocation failed");
      return DS3G_FAILURE;
    }

    ps_qos_spec.tx.fltr_template.list_ptr = 
        (ip_filter_type *)modem_mem_calloc(MAX_PACKET_FILTERS,
                                           sizeof(ip_filter_type),
                                           MODEM_MEM_CLIENT_DATA);
    if(ps_qos_spec.tx.fltr_template.list_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("Heap Memory Allocation failed");
      modem_mem_free((void *)ps_qos_spec.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
      return DS3G_FAILURE;
    }    
      
    result = ds_flow_manager_convert_nw_tft_to_app_tft(nw_tft_p,
  	              &ps_qos_spec, ip_type, flow_cntx_p, icause_code);
    if(result == DS3G_FAILURE)
    {
      ds_flow_context_release_flow_context(flow_cntx_p);
      modem_mem_free((void *)ps_qos_spec.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
      modem_mem_free((void *)ps_qos_spec.tx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
      return(result);
    }
  }
  else
  {
    DS_3GPP_MSG0_ERROR("TFT valid flag is not set, skipping related ops");
  }
  /*-------------------------------------------------------------------------
    Store the Flow Filters into the flow context.
  -------------------------------------------------------------------------*/
 /* memcpy(&(flow_cntx_p->nw_tft),&(lte_info_ptr->ul_dl_tft),
                 sizeof(tft_type_T)); VD should be deleted*/

  /*------------------------------------------------------------------------
     Create the flow 
  -------------------------------------------------------------------------*/
  ps_flow_param.qos_spec            = &ps_qos_spec;
  ps_flow_param.flow_validate_f_ptr = NULL;
  ps_flow_param.fltr_validate_f_ptr = NULL;
  ps_flow_param.fltr_priority       = PS_IFACE_IPFLTR_PRIORITY_FCFS;
  ps_flow_param.subset_id           = 5; 
  ps_flow_param.enable_filtering    = TRUE;
  ps_flow_param.is_nw_initiated     = TRUE;
  ps_flow_param.opcode              = PS_IFACE_IOCTL_QOS_REQUEST_OP;

  iface_ptr = &(ds_iface->ps_iface);

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section); 

  ret_val = ps_iface_create_flow(iface_ptr, NULL,
             &ps_flow_param, &(flow_cntx_p->ds_flow_context_dyn_p->ps_flow_ptr),&ds_errno);

  //We are done with our filters. Free them.
  if(ps_qos_spec.rx.fltr_template.list_ptr != NULL)
  {
    modem_mem_free((void *)ps_qos_spec.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    ps_qos_spec.rx.fltr_template.list_ptr = NULL;
  }
  if(ps_qos_spec.tx.fltr_template.list_ptr != NULL)
  {
    modem_mem_free((void *)ps_qos_spec.tx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    ps_qos_spec.tx.fltr_template.list_ptr = NULL;
  }

  if(ret_val == -1)
  {
    DS_3GPP_MSG0_ERROR("Create Flow failed");
    ds_flow_context_release_flow_context(flow_cntx_p);
    *icause_code = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return DS3G_FAILURE;
  }
  DS_3GPP_MSG0_HIGH("Created a new PS FLOW for bearer:");

  /*-----------------------------------------------------------------------
     Send Flow Activate CMD
  -----------------------------------------------------------------------*/
  if (ds_flow_context_ps_flow_activate_cmd(flow_cntx_p) == -1 )
  {
    DS_3GPP_MSG0_ERROR("Flow activate command failed ");
    ds_flow_context_release_flow_context(flow_cntx_p);
    *icause_code = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return DS3G_FAILURE;
  }

  
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);  

  if(!is_bearer_activated)
  {
    pdn_context_ptr = (ds_pdn_context_s*) bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;
    /*-------------------------------------------------------------------------
      Update the PDN context pending list.
    -------------------------------------------------------------------------*/
    if(pdn_context_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("Invalid PDN context associated with bearer.");
      ds_flow_context_release_flow_context(flow_cntx_p);
      *icause_code = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
      return DS3G_FAILURE;
    }
    /* copy tft into the flow context */
    memscpy(&(flow_cntx_p->ds_flow_context_dyn_p->tft_mod),
             sizeof(tft_type_T),
             nw_tft_p,
             sizeof(tft_type_T));
    if(ds_flow_manager_add_pending_flow_context(pdn_context_ptr,flow_cntx_p) == 
        FALSE)
    {
      if(ds_flow_manager_update_precedence_bit_mask(
                          pdn_context_ptr,flow_cntx_p,FALSE,FALSE,TRUE)==FALSE)
      {
        DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
      }
      ds_flow_context_release_flow_context(flow_cntx_p);
    }
    bearer_context_p->ds_bearer_context_dyn_p->sdf_id = flow_cntx_p->sdf_id;
  }
  else
  {
    /*------------------------------------------------------------------------
       Update the bearer and flow context link
    -------------------------------------------------------------------------*/
    result = ds_bearer_flow_manager_add_flow_context_to_flow_manager(
                       bearer_context_p,
                       flow_cntx_p);
    if(result == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Flow Context cannot be added to Bearer");
      ds_flow_context_release_flow_context(flow_cntx_p);
      *icause_code = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
      return DS3G_FAILURE;
    }
  
    /*-----------------------------------------------------------------------
       Set Precedence bit mask
    -----------------------------------------------------------------------*/
    if(ds_flow_manager_update_precedence_bit_mask(
                       pdn_context_p,flow_cntx_p,FALSE,TRUE,FALSE)==FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
    }
  
    /*-----------------------------------------------------------------------
       Set Identifier bit mask
    -----------------------------------------------------------------------*/
    if(ds_bearer_flow_manager_update_filter_identifier_bit_mask(
                        bearer_context_p,flow_cntx_p,TRUE)==FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error in setting Filter identifier bit mask");
    }
  
    /*-----------------------------------------------------------------------
       Track Filter Identifier for each flow context
    -----------------------------------------------------------------------*/
    if(ds_flow_context_update_filter_identifier_bit_mask(flow_cntx_p,TRUE)==FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error in setting Filter identifier bit mask");
    }
  }

  return DS3G_SUCCESS;
} /* ds_flow_manager_create_flow_context_per_iface */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_CREATE_FLOW_CONTEXT_FOR_V6_IFACE

DESCRIPTION
  Create a flow context for a v6 iface in a dual IP bearer and activates the
  flow

PARAMETERS
  pdn_context_p    -  PDN information
  bearer_context_p -  Bearer information
  mode_info_p      -  mode specific call info.try to reuse for all modes.
  nw_tft_p         -  Pointer to TFT in network format. 
  ds_iface_p       -  Information about the 3GPP IFace.
  ip_type          -  Indicate if it is a V4 or V6 iface.
  icause_code      -  internal cause code to be updated in case of failure.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_create_flow_context_for_v6_iface
(
  ds_pdn_context_s                *pdn_context_p,
  ds_bearer_context_s             *bearer_context_p,
  tft_type_T                      *nw_tft_p,
  ds_3gpp_iface_s                 *ds_iface,
  ds_3gpp_internal_cause_code_enum_type *icause_code
)
{
  ps_flow_create_param_type   ps_flow_param;
  qos_spec_type               ps_qos_spec;
  int16                       ds_errno = 0;
  ds_flow_context_s           *flow_cntx_p = NULL;
  boolean                     result = DS3G_SUCCESS;
  ps_iface_type               *iface_ptr = NULL;
  int                         ret_val = 0;
  ds_bearer_flow_manager_s    *bflow_manager_p;  
   
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG2_HIGH("Create V6 Flow Context for PDN:0x%x, bearer:0x%x",
                                             pdn_context_p,bearer_context_p);
  memset(&ps_qos_spec, 0, sizeof(qos_spec_type));
  memset(&ps_flow_param, 0, sizeof(ps_flow_create_param_type));  

  /*-------------------------------------------------------------------------
    Retrieve an empty flow context.
  -------------------------------------------------------------------------*/
  flow_cntx_p = ds_flow_context_allocate_flow_context();
  if(!ds_flow_context_validate_flow(flow_cntx_p))
  {
    *icause_code = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    return DS3G_FAILURE;
  }
  else
  {
    DS_3GPP_MSG0_HIGH("Allocated a free flow context");
    /*---------------------------------------------------------------------
       Find the bearer flow manager.
    ---------------------------------------------------------------------*/ 
    bflow_manager_p = 
              ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
    if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
    { 
      ds_flow_context_release_flow_context(flow_cntx_p);
      *icause_code = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
      return DS3G_FAILURE;
    }

    flow_cntx_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr = NULL;
    flow_cntx_p->ds_flow_context_dyn_p->ds_iface_ptr = ds_iface;
    flow_cntx_p->ds_flow_context_dyn_p->flow_type = 
             ((DS_3GPP_NW_FLOW_TYPE)|(DS_3GPP_V6_FLOW_TYPE));  
  }

  /*-------------------------------------------------------------------------
    Verify Filter Fields 
  -------------------------------------------------------------------------*/
  if(nw_tft_p->valid != TRUE) 
  {
    DS_3GPP_MSG0_ERROR("TFT valid flag is not set, cannot create v6 flow");
    ds_flow_context_release_flow_context(flow_cntx_p);
    *icause_code = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
    return(result);
  }

  if((nw_tft_p->tft_total_length < 3) ||
     (nw_tft_p->tft_total_length > 257))
  {  
    DS_3GPP_MSG1_ERROR("Received TFT with invalid length=%d",
                       nw_tft_p->tft_total_length);
    *icause_code = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
    ds_flow_context_release_flow_context(flow_cntx_p);
    return DS3G_FAILURE;
  }
  
  if(nw_tft_p->e_bit != DS_FLOW_E_BIT_DISABLE)
  {
    DS_3GPP_MSG0_ERROR("Received TFT with E bit set");
    ds_flow_context_release_flow_context(flow_cntx_p);
    *icause_code = DS_3GPP_INTERNAL_SERV_OPT_NOT_SUPPORTED;
    return DS3G_FAILURE;
  }
  
  if(nw_tft_p->operation_code != DS_3GPP_TFT_OPCODE_CREATE)
  {
    DS_3GPP_MSG1_ERROR("Received Unsupported TFT operation:%d",
                       nw_tft_p->operation_code);
    ds_flow_context_release_flow_context(flow_cntx_p);
    *icause_code = DS_3GPP_INTERNAL_SERV_OPT_NOT_SUPPORTED;
    return DS3G_FAILURE;
  }
   
  if((nw_tft_p->num_filters > DS_TFT_MAX_FILTERS)||
     (nw_tft_p->num_filters < 1))
  {
    DS_3GPP_MSG1_ERROR("Received Invalid number of filters:%d",
                       nw_tft_p->num_filters);
    ds_flow_context_release_flow_context(flow_cntx_p);
    *icause_code = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
    return DS3G_FAILURE;
  }
   
  /*------------------------------------------------------------------------
      Process the Filters 
   -------------------------------------------------------------------------*/
  ps_qos_spec.rx.fltr_template.list_ptr = 
       (ip_filter_type *)modem_mem_calloc(MAX_PACKET_FILTERS,
                                          sizeof(ip_filter_type),
                                          MODEM_MEM_CLIENT_DATA);
  if(ps_qos_spec.rx.fltr_template.list_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Heap Memory Allocation failed");
    return FALSE;
  }

  ps_qos_spec.tx.fltr_template.list_ptr = 
       (ip_filter_type *)modem_mem_calloc(MAX_PACKET_FILTERS,
                                          sizeof(ip_filter_type),
                                          MODEM_MEM_CLIENT_DATA);
  if(ps_qos_spec.tx.fltr_template.list_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Heap Memory Allocation failed");
    modem_mem_free((void *)ps_qos_spec.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }    
    
  result = ds_flow_manager_convert_nw_tft_to_app_tft(nw_tft_p,
                               &ps_qos_spec,IP_V6, flow_cntx_p, icause_code);
  if(result == DS3G_FAILURE)
  {
    ds_flow_context_release_flow_context(flow_cntx_p);
    modem_mem_free((void *)ps_qos_spec.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);    
    modem_mem_free((void *)ps_qos_spec.tx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);    
    return(result);
  }   

  /*------------------------------------------------------------------------
     Get flow info from bearer flow mgr
  -------------------------------------------------------------------------*/
  memscpy(&(ps_qos_spec.rx.flow_template.req_flow),
           sizeof(ip_flow_type),
           &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->rx_flow), 
           sizeof(ip_flow_type));

  memscpy(&(ps_qos_spec.tx.flow_template.req_flow), 
  	  sizeof(ip_flow_type),
          &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->tx_flow), 
          sizeof(ip_flow_type));

  /*------------------------------------------------------------------------
     Create the flow 
  -------------------------------------------------------------------------*/
  ps_flow_param.qos_spec            = &ps_qos_spec;
  ps_flow_param.flow_validate_f_ptr = NULL;
  ps_flow_param.fltr_validate_f_ptr = NULL;
  ps_flow_param.fltr_priority       = PS_IFACE_IPFLTR_PRIORITY_FCFS;
  ps_flow_param.subset_id           = 5; 
  ps_flow_param.enable_filtering    = TRUE;
  ps_flow_param.is_nw_initiated     = TRUE;
  ps_flow_param.opcode              = PS_IFACE_IOCTL_QOS_REQUEST_OP;

  iface_ptr = &(ds_iface->ps_iface);


  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  ret_val = ps_iface_create_flow(iface_ptr, NULL,
             &ps_flow_param, &(flow_cntx_p->ds_flow_context_dyn_p->ps_flow_ptr),&ds_errno);

  //We are done with our filters. Free them.
  modem_mem_free((void *)ps_qos_spec.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
  ps_qos_spec.rx.fltr_template.list_ptr = NULL;
  modem_mem_free((void *)ps_qos_spec.tx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
  ps_qos_spec.tx.fltr_template.list_ptr = NULL;  

  if(ret_val == -1)
  {
    DS_3GPP_MSG0_ERROR("Create Flow failed");
    ds_flow_context_release_flow_context(flow_cntx_p);
    *icause_code = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return DS3G_FAILURE;
  }
  DS_3GPP_MSG0_HIGH("Created a new PS FLOW for bearer:");

  /*------------------------------------------------------------------------
     Update the bearer and flow context link
  -------------------------------------------------------------------------*/
  result = ds_bearer_flow_manager_add_flow_context_to_flow_manager(
                     bearer_context_p,
                     flow_cntx_p);
  if(result == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Flow Context cannot be added to Bearer");
    ds_flow_context_release_flow_context(flow_cntx_p);
    *icause_code = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return DS3G_FAILURE;
  } 

  /*------------------------------------------------------------------------
     bind this flow to phys link
  -------------------------------------------------------------------------*/
  ds_flow_context_ps_flow_bind_phys_link(flow_cntx_p);

  /*-----------------------------------------------------------------------
     Send Flow Activate CMD
  -----------------------------------------------------------------------*/
  if (ds_flow_context_ps_flow_activate_cmd(flow_cntx_p) == -1 )
  {
    DS_3GPP_MSG0_ERROR("Flow activate command failed ");
    ds_flow_context_release_flow_context(flow_cntx_p);
    *icause_code = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return DS3G_FAILURE;
  }
  
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return DS3G_SUCCESS;
}

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_HANDLE_FLTR_ID_CONFLICT

DESCRIPTION
  Handles the filter Id conflict. Posts a command.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow context
  nw_bk                 -  Network Mod book keeping info
 

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_flow_manager_handle_fltr_id_conflict
(
 ds_pdn_context_s                  *pdn_context_p,
 ds_bearer_context_s               *bearer_context_p,
 ds_flow_context_s                 *flow_context_p,
 ds_3gpp_nw_modify_qos_bk_s        *nw_bk    
)
{
  ds_bearer_flow_manager_s      *bflow_manager_p = NULL;
  ds_flow_context_s             *flow_cntx = NULL;
  ds_cmd_type                   *cmd_ptr = NULL;
  ds_flow_manager_cmd_info_type *cmd_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(pdn_context_p==NULL||bearer_context_p==NULL
      ||nw_bk == NULL||(!ds_flow_context_validate_flow(flow_context_p)))
  {
    DS_3GPP_MSG3_ERROR("PDn:0x%x:Bearer_cntxt:0x%x:nw_bk:0x%x",
                       pdn_context_p,bearer_context_p,nw_bk);
    return FALSE;
  }
 
  bflow_manager_p=ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return FALSE;
  }

  /*---------------------------------------------------------------------
    Loop thu all the flows on the bearer to find conflict
  -----------------------------------------------------------------------*/
  flow_cntx = list_peek_front(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                                ->flow_context));
  while(ds_flow_context_validate_flow(flow_cntx))
  {
    if(flow_cntx != flow_context_p)
    {
      /*---------------------------------------------------------------------
        Verify if the flow has any of the filter ID conflicts
      -----------------------------------------------------------------------*/
      if((flow_cntx->ds_flow_context_dyn_p->fi_identifier & nw_bk->bearer_fltr_id) > 0)
      {
        DS_3GPP_MSG1_HIGH("Flow context 0x%x has Fi-ID conflict",flow_cntx);
        /*-----------------------------------------------------------------
          Post a command.
        -----------------------------------------------------------------*/
        cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_flow_manager_cmd_info_type));
        if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
        {
          ASSERT(0);
          return FALSE;
        }

        cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_FLOW_FLTR_ID_CONFLICT;
        cmd_info_ptr = (ds_flow_manager_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

        cmd_info_ptr->flow_context_p = (void*)flow_cntx;
        cmd_info_ptr->fltr_id_conflict = nw_bk->bearer_fltr_id;

        DS_3GPP_MSG1_HIGH("Posted identifier conflict on flow context 0x:%x",
                          flow_cntx);

        ds_put_cmd_ext(cmd_ptr);

       /*-----------------------------------------------------------------
          Disable filtering on the flow.
        -----------------------------------------------------------------*/
        if(ds_flow_context_ps_flow_filtering_control(flow_context_p, FALSE)==FALSE)
        {
          DS_3GPP_MSG1_ERROR("Disable filtering failed on flow 0x%x",flow_context_p);
        }
      }/* if flow_context has a conflict */
    }
    flow_cntx = list_peek_next(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                        ->flow_context),&(flow_cntx->link));
  }/* while flow_cntx != NULL */
  return TRUE;
}/* ds_flow_manager_handle_fltr_id_conflict */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_HANDLE_FLTR_PRECED_CONFLICT

DESCRIPTION
  Handles the fltr preced conflict. Posts a command.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow context
  nw_bk                 -  Network Mod book keeping info
 

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_flow_manager_handle_fltr_preced_conflict
(
 ds_pdn_context_s                  *pdn_context_p,
 ds_bearer_context_s               *bearer_context_p,
 ds_flow_context_s                 *flow_context_p,
 ds_3gpp_nw_modify_qos_bk_s        *nw_bk       
)
{
  uint32                        cnt =0;
  ds_cmd_type                   *cmd_ptr = NULL;
  ds_flow_manager_cmd_info_type *cmd_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((pdn_context_p==NULL)||(nw_bk == NULL))
  {
    DS_3GPP_MSG0_ERROR("invalid input params");
    return FALSE;
  }

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }
  /*---------------------------------------------------------------------
   If any of the bearers with the precedence conflict is the default bearer; 
   we need to release the PDN connection.
   Loop thru all the bearers
  -----------------------------------------------------------------------*/
  for (cnt=0; cnt < nw_bk->conflict_bearer_cnt; cnt++)
  {
    ds_bearer_context_s *bearer_w_conflict_p = nw_bk->conflict_bearer[cnt];

    if(nw_bk->prec_conf_on_same_bearer_create && 
       ((void*)bearer_context_p == (void*)(nw_bk->conflict_bearer[cnt])) )
    {
      DS_3GPP_MSG1_MED("Not posting command because precedence conflict on"
               "same bearer and opcode is create:%x", nw_bk->conflict_bearer[cnt]);
      continue;
   }
    if((!ds_bearer_cntx_validate_bearer_context(bearer_context_p)))
    {
      continue;
    }

    if (TRUE == bearer_w_conflict_p->ds_bearer_context_dyn_p->is_default)
    {
      ps_iface_type *iface_p = NULL;
      int16 ps_errno;

      iface_p = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_context_p);
      if (iface_p != NULL)
      {        
        (void)ps_iface_active_tear_down_cmd(iface_p, &ps_errno, NULL);     
      }

      iface_p = DSPDNCNTXT_GET_V6_PS_IFACE_PTR(pdn_context_p);
      if (iface_p != NULL)
      {
        (void)ps_iface_active_tear_down_cmd(iface_p, &ps_errno, NULL);       
      }      

      pdn_context_p->ds_pdn_context_dyn_p->state = 
                                      DS_PDN_CONTEXT_STATE_GOING_DOWN;
      return TRUE;
    }
    /*---------------------------------------------------------------
      Post a command.
    ----------------------------------------------------------------*/
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_flow_manager_cmd_info_type));

    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      return FALSE;
    }

    /* -----------------------------------------------------------------
       In modify bearer when Precedence conflict occures on same bearer
       (nw_bk->prec_conf_on_same_bearer set to TRUE)
       and OPCODE is create, do not post Precedence conflict command since 
       complete TFT is getting replaced so we will not have precedence conflict
       in resultant TFT.      
       -----------------------------------------------------------------*/

    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_FLOW_FLTR_PRECED_CONFLICT;

    cmd_info_ptr = (ds_flow_manager_cmd_info_type*)cmd_ptr->cmd_payload_ptr;
    cmd_info_ptr->flow_context_p = (void*)flow_context_p;
    cmd_info_ptr->bearer_context_p = (void*)(nw_bk->conflict_bearer[cnt]);
    cmd_info_ptr->prec_conf_on_same_bearer_modify  = FALSE;
   
    if(nw_bk->prec_conf_on_same_bearer_modify == TRUE)
    {
      cmd_info_ptr->prec_conf_on_same_bearer_modify = TRUE;
    }

    memscpy((cmd_info_ptr->preced_conflict),
            (sizeof(uint32)*DS_FI_PRECED_ARR_SIZE),
            (nw_bk->preced_conflict),
            (sizeof(uint32)*DS_FI_PRECED_ARR_SIZE));

    DS_3GPP_MSG1_HIGH("Posted precedence conflict on bearer 0x:%x",
                      nw_bk->conflict_bearer[cnt]);

    ds_put_cmd_ext(cmd_ptr);
    
  }/* for */
  return TRUE;
}/* ds_flow_manager_handle_fltr_preced_conflict */


/*===========================================================================
FUNCTION DS_FLOW_MANAGER_HANDLE_FLTR_CONFLICT

DESCRIPTION
  Handles the fltr precedence and identifier conflicts.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow context
  nw_bk                 -  Network Mod book keeping info
 

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_flow_manager_handle_fltr_conflict
(
 ds_pdn_context_s                  *pdn_context_p,
 ds_bearer_context_s               *bearer_context_p,
 ds_flow_context_s                 *flow_context_p,
 ds_3gpp_nw_modify_qos_bk_s        *nw_bk       
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((pdn_context_p==NULL)||(bearer_context_p==NULL)
      || (nw_bk == NULL)||(!ds_flow_context_validate_flow(flow_context_p)))
  {
    DS_3GPP_MSG3_ERROR("PDN:0x%x:Bearer:0x%x:nw_bk:0x%x",pdn_context_p,
                       bearer_context_p,nw_bk);
    return FALSE;
  }
  
  if(nw_bk->bearer_fltr_id != 0)
  {
    /*-------------------------------------------------------------------
      Resolve the filter ID conflict on the bearer.
    ---------------------------------------------------------------------*/
    if(ds_flow_manager_handle_fltr_id_conflict(pdn_context_p,
                                         bearer_context_p,
                                         flow_context_p,
                                         nw_bk) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("unable to handle filter-id conflict");
      return FALSE;
    }
  }

  if(nw_bk->conflict_bearer_cnt != 0)
  {
   /*-------------------------------------------------------------------
      Resolve the precedence conflict.
    ---------------------------------------------------------------------*/
    if(ds_flow_manager_handle_fltr_preced_conflict(pdn_context_p,
                                              bearer_context_p,
                                              flow_context_p,
                                              nw_bk) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("unable to handle precedence conflict");
      return FALSE;
    }
  }
  return TRUE;
}/* ds_flow_manager_handle_fltr_conflict*/


/*===========================================================================
FUNCTION DS_FLOW_MANAGER_ADD_FILTER_TO_FLOW

DESCRIPTION
  Modify existing TFT.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow context to add filters.
  nw_tft                -  TFT information
  nw_bk                 -  Book keeping information.
  mode_info_p           -  Mode specific Qos.
  new_flow              -  Indicates a new NW flow is created.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_flow_manager_add_filter_to_flow
(
  ds_pdn_context_s                      *pdn_context_p,
  ds_bearer_context_s                   *bearer_context_p,
  ds_flow_context_s                     *flow_context_p,
  tft_type_T                            *nw_tft,
  ds_3gpp_nw_modify_qos_bk_s            *nw_bk,
  const cm_call_mode_info_s_type        *mode_info_p,
  boolean                                new_flow,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{
  int16                       ds_errno = 0;
  ps_iface_type               *iface_ptr = NULL;
  int                         ret_val = 0;
  ps_flow_modify_param_type   ps_flow_param;
  ps_flow_create_param_type   new_ps_flow_param;
  packet_filter_type_T       *nw_filter=NULL;
  uint16                     filter_id = 0;
  byte                        direction  =0;
  ds_bearer_flow_manager_s   *bflow_manager_p = NULL;
  ds_flow_manager_s          *flow_manager_p  = NULL;
  ds_tft_pkt_filter_direction_enum_type filter_direction;
  int cnt = 0;
  boolean result = DS3G_FAILURE;
  ip_version_enum_type                    ip_type; 
  uint8              orig_rx_filter_cnt=0,orig_tx_filter_cnt=0;
  uint8              torig_rx_filter_cnt=0,torig_tx_filter_cnt=0;  
  ps_extended_info_code_enum_type ps_eic = PS_EIC_NOT_SPECIFIED;   
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  if((pdn_context_p == NULL)||(bearer_context_p==NULL)
     ||(nw_tft == NULL)||(nw_bk == NULL)||
     (!ds_flow_context_validate_flow(flow_context_p))
      || (cause_code_p ==NULL))
  { 

    DS_3GPP_MSG3_ERROR("PDN:0x%x:Bearer:0x%x:nw_bk:0x%x",pdn_context_p,
                       bearer_context_p,nw_bk);

    DS_3GPP_MSG2_ERROR("nw_tft:0x%x cause_code_p:0x%x",nw_tft,
                        cause_code_p);

    if(cause_code_p != NULL)
    {
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    }
    return FALSE;
  }

  *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;

  bflow_manager_p=ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return FALSE;
  }

  flow_manager_p = ds_flow_manager_find_flow_manager(pdn_context_p);
  if(flow_manager_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("pdn does not have flow manager");
    return FALSE;
  }
              
  /*-------------------------------------------------------------------------
    Configure the QOS flow spec
  -------------------------------------------------------------------------*/
  if(ds_3gpp_tft_configure_nw_bk_flow_spec(nw_bk)==FALSE)
  {
    DS_3GPP_MSG0_ERROR("unable to configure the nw book-keeping info");
    return FALSE;
  }

  /*-------------------------------------------------------------------
    Retrieve current filter info for flow context.
  ---------------------------------------------------------------------*/
  if(new_flow == FALSE)
  {
    if(ds_flow_context_ps_flow_get_filters(
                                flow_context_p,
                                FALSE,
                               &(nw_bk->curr_qos)) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Could not retrieve previous filters");
      return FALSE; 
    }
  }
  /*-----------------------------------------------------------------------
    Find the right iface pointer.
  -----------------------------------------------------------------------*/ 
  if(ds_flow_context_retrieve_iface_ip_type(flow_context_p,
                                            &ip_type,
                                            &iface_ptr) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("3GPP iface is not present");
    return FALSE;
  }

  /*-------------------------------------------------------------------
    Keep track of the orig rx and orig tx filter cnt
  ---------------------------------------------------------------------*/
  orig_rx_filter_cnt = nw_bk->curr_qos.rx.fltr_template.num_filters;
  orig_tx_filter_cnt = nw_bk->curr_qos.tx.fltr_template.num_filters;

  /*-------------------------------------------------------------------
    If there is a filter-identifier clash on NW Flow.
  ---------------------------------------------------------------------*/

 if(nw_tft->operation_code ==(byte) DS_3GPP_TFT_OPCODE_CREATE)
 {

    nw_bk->curr_qos.rx.fltr_template.list_ptr = nw_bk->mod_rx_fltr;
    nw_bk->curr_qos.tx.fltr_template.list_ptr = nw_bk->mod_tx_fltr;
    nw_bk->curr_qos.rx.fltr_template.num_filters = 
                                  nw_bk->mod_qos.rx.fltr_template.num_filters;
    nw_bk->curr_qos.tx.fltr_template.num_filters = 
                                  nw_bk->mod_qos.tx.fltr_template.num_filters;
    nw_bk->flow_cntx_fltr_id = 0;
    nw_bk->bearer_fltr_id = 0;
 }
 else
 {
  if(nw_bk->flow_cntx_fltr_id != 0)
  {
    /*-------------------------------------------------------------------
      Remove the flow filters.
    ---------------------------------------------------------------------*/
    if(ds_flow_context_delete_filter_from_flow(
                                         flow_context_p,
                                         nw_bk->flow_cntx_fltr_id,
                                         &nw_bk->curr_qos, 
                                         &nw_bk->mod_qos)==FALSE)
    {
      DS_3GPP_MSG0_ERROR("error deleting filter from flow");
      return FALSE;
    }
  
    /*-------------------------------------------------------------------
      Update the filter pointers. The filter count is already stored.
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.list_ptr = nw_bk->mod_rx_fltr;
    nw_bk->curr_qos.tx.fltr_template.list_ptr = nw_bk->mod_tx_fltr;
    nw_bk->curr_qos.rx.fltr_template.num_filters = 
                                  nw_bk->mod_qos.rx.fltr_template.num_filters;
    nw_bk->curr_qos.tx.fltr_template.num_filters = 
                                  nw_bk->mod_qos.tx.fltr_template.num_filters;
  }/* nw_bk->flow_cntx_fltr_idd != 0 */

 }
  /*-----------------------------------------------------------------------
    Loop through all the filters and add them to the NW flow context.
  -----------------------------------------------------------------------*/
  for(cnt=0; cnt< nw_tft->num_filters; cnt++)
  {
    nw_filter = &(nw_tft->filter_list[cnt]);
    filter_id = nw_filter->identifier & 0xF;
    direction = ((nw_filter->identifier & 0x30)>>4);
    filter_direction =(ds_tft_pkt_filter_direction_enum_type)direction; 
    switch(filter_direction)
    {
      case DS_3GPP_TFT_UPLINK:
        if(nw_bk->curr_qos.tx.fltr_template.num_filters >= MAX_PACKET_FILTERS)
        {
          DS_3GPP_MSG0_ERROR("Excedding UE supported max filters for uplink ");
          return FALSE;
        }
        result =ds_flow_manager_convert_nw_filter_to_app_filter(
                  nw_filter,
                  &(nw_bk->curr_qos.tx.fltr_template.list_ptr[
                               nw_bk->curr_qos.tx.fltr_template.num_filters]),
                  (byte)filter_id, filter_direction, ip_type, flow_context_p,cause_code_p);
        if(result == DS3G_FAILURE)
        {
          DS_3GPP_MSG1_ERROR("Error returned for filter No:%d,in Uplink",cnt);
          return FALSE;
        }
        nw_bk->curr_qos.tx.fltr_template.num_filters++;
      break;

      case DS_3GPP_TFT_DOWNLINK:
      case DS_3GPP_TFT_PRE_REL_7:
        if(nw_bk->curr_qos.rx.fltr_template.num_filters >= MAX_PACKET_FILTERS)
        {
          DS_3GPP_MSG0_ERROR("Excedding UE supported max filters for downlink");
          return FALSE;
        }
        result =ds_flow_manager_convert_nw_filter_to_app_filter(
               nw_filter,
	           &(nw_bk->curr_qos.rx.fltr_template.list_ptr[
                              nw_bk->curr_qos.rx.fltr_template.num_filters]),
			   (byte)filter_id, DS_3GPP_TFT_DOWNLINK, ip_type, flow_context_p,
                            cause_code_p);
        if(result == DS3G_FAILURE)
        {
          DS_3GPP_MSG1_ERROR("Error returned for filter No:%d,in Downlink",cnt);
          return FALSE;
        }
        nw_bk->curr_qos.rx.fltr_template.num_filters++;
      break;

      case DS_3GPP_TFT_BIDIRECTIONAL:
        if(nw_bk->curr_qos.rx.fltr_template.num_filters >= MAX_PACKET_FILTERS)
        {
          DS_3GPP_MSG0_ERROR("Excedding UE supported max filters for downlink ");
          return FALSE;
        }
        result =ds_flow_manager_convert_nw_filter_to_app_filter(nw_filter,
	            &(nw_bk->curr_qos.rx.fltr_template.list_ptr[
                              nw_bk->curr_qos.rx.fltr_template.num_filters]),
			(byte)filter_id, DS_3GPP_TFT_DOWNLINK, ip_type,flow_context_p,
                         cause_code_p);
        if(result == DS3G_FAILURE)
        {
          DS_3GPP_MSG1_ERROR("Error returned for filter No:%d,in Downlink",cnt);
          return FALSE;
        }
        nw_bk->curr_qos.rx.fltr_template.num_filters++;

        if(nw_bk->curr_qos.tx.fltr_template.num_filters >= MAX_PACKET_FILTERS)
        {
          DS_3GPP_MSG0_ERROR("Excedding UE supported max filters for uplink ");
          return FALSE;
        }
        result =ds_flow_manager_convert_nw_filter_to_app_filter(nw_filter,
	        &(nw_bk->curr_qos.tx.fltr_template.list_ptr[
                         nw_bk->curr_qos.tx.fltr_template.num_filters]),
			(byte)filter_id, DS_3GPP_TFT_UPLINK, ip_type,flow_context_p,
                         cause_code_p);
        if(result == DS3G_FAILURE)
        {
          DS_3GPP_MSG1_ERROR("Error returned for filter No:%d,in Uplink",cnt);
          return FALSE;
        }
        nw_bk->curr_qos.tx.fltr_template.num_filters++;
      break;
      default:
        DS_3GPP_MSG0_ERROR("unexpected direction received");
    }/* switch */   
  }/* for */
  /*-------------------------------------------------------------------
    Update the TX Filter Mask.
  ---------------------------------------------------------------------*/
  if(nw_bk->curr_qos.tx.fltr_template.num_filters > 0)
  {
    if(orig_tx_filter_cnt == 0)
    {
      nw_bk->curr_qos.field_mask |=
               (qos_spec_field_mask_type)QOS_MASK_TX_FLOW;
      if(mode_info_p == NULL)
      {
        DS_3GPP_MSG0_HIGH("New Filter added on TX, but no data rate");
        // Extract the flow from bearer
      }
    }
    else
    {
      nw_bk->curr_qos.field_mask |=
            (qos_spec_field_mask_type)QOS_MODIFY_MASK_TX_FLTR_MODIFY;
    }
  }
  /*-------------------------------------------------------------------
    Update the RX Filter Mask.
  ---------------------------------------------------------------------*/ 
  if(nw_bk->curr_qos.rx.fltr_template.num_filters > 0)
  {
    if(orig_rx_filter_cnt == 0)
    {
      nw_bk->curr_qos.field_mask |=
                   (qos_spec_field_mask_type)QOS_MASK_RX_FLOW;
      if(mode_info_p == NULL)
      {
        DS_3GPP_MSG0_HIGH("New Filter added on RX, but no data rate");
        // Extact the flow from Bearer.
      }
    }
    else
    {
      nw_bk->curr_qos.field_mask |=
            (qos_spec_field_mask_type)QOS_MODIFY_MASK_RX_FLTR_MODIFY;
    }
  }
  /*-------------------------------------------------------------------
    Update the QOS.
  ---------------------------------------------------------------------*/
  if(mode_info_p != NULL)
  {
    /*---------------------------------------------------------------------
      Convert NW Qos to App Qos
    ---------------------------------------------------------------------*/
    if(ds_bearer_flow_manager_convert_data_rate(bearer_context_p,
                                               mode_info_p,
                                               &(nw_bk->curr_qos),cause_code_p
                                               )== DS3G_FAILURE)
    {
      DS_3GPP_MSG0_ERROR("Network Qos is not accepted");
      return FALSE;
    }
    // Need to check data rate before configuring the field mask.
    if(nw_bk->curr_qos.rx.flow_template.req_flow.field_mask)                        
    {
      if(nw_bk->curr_qos.rx.fltr_template.num_filters > 0)
      {
        nw_bk->curr_qos.field_mask |= ((qos_spec_field_mask_type)
                                            QOS_MASK_RX_FLOW);
      }
    }
    if(nw_bk->curr_qos.tx.flow_template.req_flow.field_mask)                        
    {
      if(nw_bk->curr_qos.tx.fltr_template.num_filters > 0)
      {
        nw_bk->curr_qos.field_mask |= ((qos_spec_field_mask_type)
                                            QOS_MASK_TX_FLOW);
      } 
    } 
  }/* mode_info_p  != NULL*/

  if(new_flow)
  {
    /*-------------------------------------------------------------------
      Copy the bearer Qos to ps flow and then update create params
    ---------------------------------------------------------------------*/
    memset(&new_ps_flow_param, 0, sizeof(ps_flow_create_param_type));

    /*-------------------------------------------------------------------
      Copy the previous bearer Qos to flow.
    ---------------------------------------------------------------------*/
    if(mode_info_p == NULL)
    {
      memscpy(&(nw_bk->curr_qos.rx.flow_template.req_flow),
	      sizeof(ip_flow_type),
              &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->rx_flow), 
              sizeof(ip_flow_type));
      memscpy(&(nw_bk->curr_qos.tx.flow_template.req_flow),
   	      sizeof(ip_flow_type),
              &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->tx_flow),
              sizeof(ip_flow_type));
    }
    /*-----------------------------------------------------------------------
      Clear the filter modify flag.
    -----------------------------------------------------------------------*/
    if(nw_bk->curr_qos.tx.fltr_template.num_filters > 0) 
    {
      nw_bk->curr_qos.field_mask &=~((qos_spec_field_mask_type)
                       (QOS_MODIFY_MASK_TX_FLTR_MODIFY));
      nw_bk->curr_qos.field_mask |= ((qos_spec_field_mask_type)
                                        QOS_MASK_TX_FLOW);
    }
    if(nw_bk->curr_qos.rx.fltr_template.num_filters > 0)
    {
      nw_bk->curr_qos.field_mask &=~((qos_spec_field_mask_type)
                       (QOS_MODIFY_MASK_RX_FLTR_MODIFY));
      nw_bk->curr_qos.field_mask |= ((qos_spec_field_mask_type)
                                            QOS_MASK_RX_FLOW);
    }

    new_ps_flow_param.qos_spec            = &(nw_bk->curr_qos);
    new_ps_flow_param.flow_validate_f_ptr = NULL;
    new_ps_flow_param.fltr_validate_f_ptr = NULL;
    new_ps_flow_param.fltr_priority       = PS_IFACE_IPFLTR_PRIORITY_FCFS;
    new_ps_flow_param.subset_id           = 5; 
    new_ps_flow_param.enable_filtering    = FALSE;
    new_ps_flow_param.is_nw_initiated     = (flow_context_p->ds_flow_context_dyn_p->flow_type 
                                             & DS_3GPP_NW_FLOW_TYPE)? TRUE : FALSE;
    new_ps_flow_param.opcode              = PS_IFACE_IOCTL_QOS_REQUEST_OP;
    /*-----------------------------------------------------------------------
      Invoke the Flow creation
    -----------------------------------------------------------------------*/
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
    ret_val = ps_iface_create_flow(iface_ptr,NULL,
             &new_ps_flow_param,&(flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr),&ds_errno);
    if(ret_val == -1)
    {
      DS_3GPP_MSG0_ERROR("create Flow failed");
      ds_flow_context_release_flow_context(flow_context_p);
      *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return FALSE;
    }    

    /*-----------------------------------------------------------------------
	      Send Flow Activate CMD
	  ----------------------------------------------------------------------*/
    if (ds_flow_context_ps_flow_activate_cmd(flow_context_p) == -1 )
    {
      DS_3GPP_MSG0_ERROR("Flow activate command failed ");
	 	  ds_flow_context_release_flow_context(flow_context_p);
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
 	 	  return FALSE;
	 	}
   
    
     PS_LEAVE_CRIT_SECTION(&global_ps_crit_section); 
    /*------------------------------------------------------------------------
      Update the bearer and flow context link
    -------------------------------------------------------------------------*/
    result = ds_bearer_flow_manager_add_flow_context_to_flow_manager(
                     bearer_context_p,
                     flow_context_p);
    if(result == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Flow Context cannot be added to Bearer");
      ds_flow_context_release_flow_context(flow_context_p);
      return FALSE;
    }

    ds_flow_context_ps_flow_bind_phys_link(flow_context_p);

    /*-----------------------------------------------------------------------
      Send Flow Activate Indication.
    -----------------------------------------------------------------------*/
    ds_flow_context_ps_flow_activate_ind(flow_context_p, PS_EIC_NOT_SPECIFIED);    

  }/* if new flow*/
  else
  {
    /*-----------------------------------------------------------------------
      Update the Flow with the filter information.
    -----------------------------------------------------------------------*/                               
    memset(&ps_flow_param, 0, sizeof(ps_flow_modify_param_type));
    ps_flow_param.qos_spec            = &(nw_bk->curr_qos);
    ps_flow_param.flow_validate_f_ptr = NULL;
    ps_flow_param.fltr_validate_f_ptr = NULL;
    ps_flow_param.fltr_priority       = PS_IFACE_IPFLTR_PRIORITY_FCFS;
    ps_flow_param.subset_id           = 5; 

    /*-------------------------------------------------------------------
      Disable Filtering on the Flow.
    ---------------------------------------------------------------------*/
    if(ds_flow_context_ps_flow_filtering_control(flow_context_p, FALSE)!= TRUE)
    {
      DS_3GPP_MSG0_ERROR("flow filtering control failed");
      //return FALSE;
    }
    /*-----------------------------------------------------------------------
      Invoke the Flow Modify.
    -----------------------------------------------------------------------*/
    ret_val = ps_iface_modify_flow(iface_ptr,(flow_context_p->ds_flow_context_dyn_p
                                          ->ps_flow_ptr),&ps_flow_param, &ds_errno);
    if(ret_val == -1)
    {
      DS_3GPP_MSG0_ERROR("modify Flow failed");
      /*-------------------------------------------------------------------
        Enable Filtering on the Flow.
      ---------------------------------------------------------------------*/
      if(ds_flow_context_ps_flow_filtering_control(flow_context_p, TRUE)!= TRUE)
      {
        DS_3GPP_MSG0_ERROR("flow filtering control failed");
        //return FALSE;
      }
      return FALSE;
    }
  }/* new_flow == FALSE */

  if(new_flow == FALSE)
  {
    /* if this is a NW flow then we may need to wait for lower layer to confirm
       the modification */
    if((flow_context_p->ds_flow_context_dyn_p->flow_type & DS_3GPP_NW_FLOW_TYPE) 
       == DS_3GPP_NW_FLOW_TYPE)
    {
      ds_flow_context_set_flow_state(flow_context_p,
                                     DS_FLOW_CONTEXT_STATE_PENDING_LL_CONFIRM);
    }
    else
    {
      /*-------------------------------------------------------------------
        Modify Flow Accepted.
      ---------------------------------------------------------------------*/
      if(ds_flow_context_ps_flow_modify_result(
                                        flow_context_p,TRUE,ps_eic)!=TRUE)
      {
        DS_3GPP_MSG0_ERROR("flow modify result failed");
        return FALSE;
      }
      if(ds_flow_context_ps_flow_filtering_control(flow_context_p, TRUE)!= TRUE)
      {
        DS_3GPP_MSG0_ERROR("flow filtering control failed");
        //return FALSE;
      }
    }
  } /* new_flow == FALSE*/
  else if(nw_bk->curr_qos.field_mask&QOS_MASK_TX_FLOW)
  {
    /*-------------------------------------------------------------------
      Enable Filtering on the Flow.
    ---------------------------------------------------------------------*/
    if(ds_flow_context_ps_flow_filtering_control(flow_context_p, TRUE)!= TRUE)
    {
      DS_3GPP_MSG0_ERROR("flow filtering control failed");
     // return FALSE;
    }
  }
  else
  {
    DS_3GPP_MSG0_HIGH("Network is adding only RX filters, added successfully");
  }


  
  /*-------------------------------------------------------------------
    Update the Identifier and Precedence if there was Flter Id conflict
    on the flow ;also including the case where operator code is create; 
    here then we clear the filter id and precendence for all the initial filters.
  ---------------------------------------------------------------------*/
  if(nw_bk->flow_cntx_fltr_id != 0 ||(nw_tft->operation_code ==(byte) DS_3GPP_TFT_OPCODE_CREATE))
  {
    /*-------------------------------------------------------------------
      Update the filter pointers and filter count to the original cnt
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.list_ptr  = nw_bk->curr_rx_fltr;
    nw_bk->curr_qos.tx.fltr_template.list_ptr  = nw_bk->curr_tx_fltr;
    /*-------------------------------------------------------------------
      Store the current filter count
    ---------------------------------------------------------------------*/
    torig_rx_filter_cnt = nw_bk->curr_qos.rx.fltr_template.num_filters;
    torig_tx_filter_cnt = nw_bk->curr_qos.tx.fltr_template.num_filters;
    /*-------------------------------------------------------------------
      Retrieve the filter count before modification
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.num_filters = orig_rx_filter_cnt;
    nw_bk->curr_qos.tx.fltr_template.num_filters = orig_tx_filter_cnt;
  
    /*-------------------------------------------------------------------
      clear Precedence and Filter ID information.
    ---------------------------------------------------------------------*/
    if(ds_flow_manager_update_filter_id_precedence(
                                              pdn_context_p,
                                              bearer_context_p,
                                              flow_context_p,
                                              nw_bk->flow_cntx_fltr_id,
                                              &(nw_bk->curr_qos),
                                              TRUE) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("update filter id and precedence failed");
      return FALSE;
    }
    /*-------------------------------------------------------------------
      Update the filter count to current count
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.num_filters = torig_rx_filter_cnt;
    nw_bk->curr_qos.tx.fltr_template.num_filters = torig_tx_filter_cnt;
    /*-------------------------------------------------------------------
      Update the filter pointer.
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.list_ptr    = nw_bk->mod_rx_fltr;
    nw_bk->curr_qos.tx.fltr_template.list_ptr    = nw_bk->mod_tx_fltr;  
  } /* nw_bk->flow_cntx_fltr_id != 0*/

  /*-------------------------------------------------------------------
    Set the precedence and filter_identifier.
  ---------------------------------------------------------------------*/
  if(ds_flow_manager_update_filter_id_precedence(pdn_context_p,
      bearer_context_p,flow_context_p,0,&(nw_bk->curr_qos),FALSE)==FALSE)
  {
    DS_3GPP_MSG0_ERROR("filter update failed");
    return FALSE;
  }
  return TRUE;
} /*ds_flow_manager_add_filter_to_flow*/

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_REPLACE_FILTER_IN_FLOW

DESCRIPTION
  Replace Filter in a flow .

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow context.
  nw_tft                -  TFT information.
  mode_info_p           -  Mode specific Qos.
  nw_bk                 -  network book keeping info.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_flow_manager_replace_filter_in_flow
(
  ds_pdn_context_s                      *pdn_context_p,
  ds_bearer_context_s                   *bearer_context_p,
  ds_flow_context_s                     *flow_context_p,
  tft_type_T                            *nw_tft,
  ds_3gpp_nw_modify_qos_bk_s            *nw_bk,
  const cm_call_mode_info_s_type        *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{
  ps_flow_modify_param_type    ps_flow_param;
  int16                        ds_errno = 0;
  ps_iface_type                *iface_ptr = NULL;
  int                          ret_val = 0;
  ip_version_enum_type                    ip_type;
  int cnt = 0;
  packet_filter_type_T       *nw_filter=NULL;
  uint16                     filter_id = 0;
  byte                        direction  =0;
  boolean result = DS3G_FAILURE;
  ds_tft_pkt_filter_direction_enum_type filter_direction; 
  uint8              orig_rx_filter_cnt=0,orig_tx_filter_cnt=0;
  uint8              torig_rx_filter_cnt=0,torig_tx_filter_cnt=0;
  ps_extended_info_code_enum_type ps_eic = PS_EIC_NOT_SPECIFIED; 
  boolean                    update_filter_info = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  

  if((pdn_context_p == NULL) || (bearer_context_p == NULL) || (nw_tft == NULL)
     || (nw_bk == NULL) ||
     (!ds_flow_context_validate_flow(flow_context_p))
     || (cause_code_p == NULL) )
  {
    if(cause_code_p != NULL)
    {
       *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    }
    DS_3GPP_MSG0_ERROR("input params are NULL");
    return FALSE;
  }

  *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;  

  /*-------------------------------------------------------------------------
    Configure the QOS flow spec
  -------------------------------------------------------------------------*/
  if(ds_3gpp_tft_configure_nw_bk_flow_spec(nw_bk) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("error configuring nw book-keeping");
    return FALSE;
  }

  /*-------------------------------------------------------------------
    Retrieve current filter info for flow context.
  ---------------------------------------------------------------------*/
  if(ds_flow_context_ps_flow_get_filters(
                                flow_context_p,
                                FALSE,
                               &(nw_bk->curr_qos)) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Could not retrieve previous filters");
    return FALSE; 
  }

  /*-----------------------------------------------------------------------
    Store the orginal filter count before modification.
  -----------------------------------------------------------------------*/
  torig_rx_filter_cnt = nw_bk->curr_qos.rx.fltr_template.num_filters;
  torig_tx_filter_cnt = nw_bk->curr_qos.tx.fltr_template.num_filters;
  /*-----------------------------------------------------------------------
  According to spec 24.008:Section: 6.1.3.3.3
  1) TFT operation = "Create a new TFT" when there is already an existing TFT 
  for the PDP context.The receiver shall further process the new activation 
  request and, if it was processed successfully, delete the old TFT.
  -----------------------------------------------------------------------*/
  if(nw_tft->operation_code == DS_3GPP_TFT_OPCODE_CREATE)
  {
    /*-------------------------------------------------------------------
      Remove the flow filters.
    ---------------------------------------------------------------------*/
    DS_3GPP_MSG3_HIGH("replace_filter_in_flow op-code %d, flow cntxfltr id "
                      "mask =%d non_match_fltr_id= %d", 
                       nw_tft->operation_code,
	               flow_context_p->ds_flow_context_dyn_p->fi_identifier,
  	               nw_bk->non_match_fltr_id);
	
    nw_bk->flow_cntx_fltr_id = 0;
    update_filter_info = TRUE;
  }
  else if((nw_bk->flow_cntx_fltr_id != 0) )
  {
    /*-------------------------------------------------------------------
     flow_cntx_fltr_id will be set only if there is any filter already installed
     with same filter ID, Remove the flow filters.
    ---------------------------------------------------------------------*/
    DS_3GPP_MSG3_HIGH("replace_filter_in_flow op-code:%d,flow_cntx_fltr_id =%d"
                      " non_match_fltr_id= %d",
                      nw_tft->operation_code,
                      nw_bk->flow_cntx_fltr_id,
                      nw_bk->non_match_fltr_id);
  
    if(ds_flow_context_delete_filter_from_flow(flow_context_p,
                                          nw_bk->flow_cntx_fltr_id,
                                          &(nw_bk->curr_qos),
                                          &(nw_bk->mod_qos))==FALSE)
    {
      DS_3GPP_MSG0_ERROR("error deleting filter from flow");
      return FALSE;
    }
    update_filter_info = TRUE;
  }/* flow_filter_id != 0*/
  
  if(update_filter_info == TRUE)
  {
    /*-------------------------------------------------------------------
      Update the filter pointers and number of filters.
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.list_ptr = nw_bk->mod_rx_fltr;
    nw_bk->curr_qos.tx.fltr_template.list_ptr = nw_bk->mod_tx_fltr;
    orig_rx_filter_cnt = nw_bk->curr_qos.rx.fltr_template.num_filters;
    orig_tx_filter_cnt = nw_bk->curr_qos.tx.fltr_template.num_filters;
    nw_bk->curr_qos.rx.fltr_template.num_filters = 
                                nw_bk->mod_qos.rx.fltr_template.num_filters;
    nw_bk->curr_qos.tx.fltr_template.num_filters = 
                                nw_bk->mod_qos.tx.fltr_template.num_filters;
  }/* flow_filter_id != 0*/

  /*-----------------------------------------------------------------------
    Find the right iface pointer.
  -----------------------------------------------------------------------*/ 
  if(ds_flow_context_retrieve_iface_ip_type(flow_context_p,
                                            &ip_type,
                                            &iface_ptr) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("3GPP iface is not present");
    return FALSE;
  }
  /*-----------------------------------------------------------------------
    Add the filters requested by NW.
  -----------------------------------------------------------------------*/          
  for(cnt=0; cnt< nw_tft->num_filters; cnt++)
  {
    nw_filter = &(nw_tft->filter_list[cnt]);
    filter_id = nw_filter->identifier & 0xF;
    direction = ((nw_filter->identifier & 0x30)>>4);
    filter_direction =(ds_tft_pkt_filter_direction_enum_type)direction; 
    switch(filter_direction)
    {
      case DS_3GPP_TFT_UPLINK:
        if(nw_bk->curr_qos.tx.fltr_template.num_filters >= MAX_PACKET_FILTERS)
        {
          DS_3GPP_MSG0_ERROR("Excedding UE supported max filters for uplink ");
          return FALSE;
        }
        result =ds_flow_manager_convert_nw_filter_to_app_filter(
                  nw_filter,
                  &(nw_bk->curr_qos.tx.fltr_template.list_ptr[
                               nw_bk->curr_qos.tx.fltr_template.num_filters]),
                  (byte)filter_id, filter_direction, ip_type, flow_context_p,
                   cause_code_p);
        if(result == DS3G_FAILURE)
        {
          DS_3GPP_MSG1_ERROR("Error returned for filter No:%d,in Uplink",cnt);
          return FALSE;
        }
        nw_bk->curr_qos.tx.fltr_template.num_filters++;

      break;

      case DS_3GPP_TFT_DOWNLINK:
      case DS_3GPP_TFT_PRE_REL_7:
        if(nw_bk->curr_qos.rx.fltr_template.num_filters >= MAX_PACKET_FILTERS)
        {
          DS_3GPP_MSG0_ERROR("Excedding UE supported max filters for downlink");
          return FALSE;
        }
        result =ds_flow_manager_convert_nw_filter_to_app_filter(nw_filter,
	     &(nw_bk->curr_qos.rx.fltr_template.list_ptr[
                          nw_bk->curr_qos.rx.fltr_template.num_filters]),
	     (byte)filter_id, DS_3GPP_TFT_DOWNLINK, ip_type, flow_context_p,
              cause_code_p);
        if(result == DS3G_FAILURE)
        {
          DS_3GPP_MSG1_ERROR("Error returned for filter No:%d,in Downlink",cnt);
          return FALSE;
        }
        nw_bk->curr_qos.rx.fltr_template.num_filters++;
      break;

      case DS_3GPP_TFT_BIDIRECTIONAL:
        if(nw_bk->curr_qos.rx.fltr_template.num_filters >= MAX_PACKET_FILTERS)
        {
          DS_3GPP_MSG0_ERROR("Excedding UE supported max filters for downlink ");
          return FALSE;
        }
        result =ds_flow_manager_convert_nw_filter_to_app_filter(nw_filter,
	        &(nw_bk->curr_qos.rx.fltr_template.list_ptr[
                            nw_bk->curr_qos.rx.fltr_template.num_filters]),
			(byte)filter_id, DS_3GPP_TFT_DOWNLINK, ip_type,flow_context_p,
                         cause_code_p);
        if(result == DS3G_FAILURE)
        {
          DS_3GPP_MSG1_ERROR("Error returned for filter No:%d,in Downlink",cnt);
          return FALSE;
        }
        nw_bk->curr_qos.rx.fltr_template.num_filters++;

        if(nw_bk->curr_qos.tx.fltr_template.num_filters >= MAX_PACKET_FILTERS)
        { 
          DS_3GPP_MSG0_ERROR("Excedding UE supported max filters for uplink ");
          return FALSE;
        }
        result =ds_flow_manager_convert_nw_filter_to_app_filter(nw_filter,
	        &(nw_bk->curr_qos.tx.fltr_template.list_ptr[
                             nw_bk->curr_qos.tx.fltr_template.num_filters]),
			(byte)filter_id, DS_3GPP_TFT_UPLINK, ip_type,flow_context_p,
                         cause_code_p);
        if(result == DS3G_FAILURE)
        {
          DS_3GPP_MSG1_ERROR("Error returned for filter No:%d,in Uplink",cnt);
          return FALSE;
        }
        nw_bk->curr_qos.tx.fltr_template.num_filters++;
      break;
      default:
        DS_3GPP_MSG0_ERROR("unexpected direction received");
    }/* switch */   
  }/* for */

  /*-------------------------------------------------------------------
    Update the TX Filter Mask.
  ---------------------------------------------------------------------*/
  if(nw_bk->curr_qos.tx.fltr_template.num_filters > 0)
  {
    if(torig_tx_filter_cnt == 0)
    {
      nw_bk->curr_qos.field_mask |=
               (qos_spec_field_mask_type)QOS_MASK_TX_FLOW;
      if(mode_info_p == NULL)
      {
        DS_3GPP_MSG0_HIGH("Filter Add/Rep on TX, but no data rate");
        // Extract the flow from bearer
      }
    }
    else
    {
      nw_bk->curr_qos.field_mask |=
            (qos_spec_field_mask_type)QOS_MODIFY_MASK_TX_FLTR_MODIFY;
    }
  }
  /*-------------------------------------------------------------------
    Update the RX Filter Mask.
  ---------------------------------------------------------------------*/ 
  if(nw_bk->curr_qos.rx.fltr_template.num_filters > 0)
  {
    if(torig_rx_filter_cnt == 0)
    {
      nw_bk->curr_qos.field_mask |=
                   (qos_spec_field_mask_type)QOS_MASK_RX_FLOW;
      if(mode_info_p == NULL)
      {
        DS_3GPP_MSG0_HIGH("Filter add/Rep on RX, but no data rate");
        // Extact the flow from Bearer.
      }
    }
    else
    {
      nw_bk->curr_qos.field_mask |=
            (qos_spec_field_mask_type)QOS_MODIFY_MASK_RX_FLTR_MODIFY;
    }
  }
  /*---------------------------------------------------------------------
    Convert NW Qos to App Qos
  ---------------------------------------------------------------------*/
  if(mode_info_p != NULL)
  {
    if(ds_bearer_flow_manager_convert_data_rate(bearer_context_p,
                                       mode_info_p,
                                       &nw_bk->curr_qos,
                                       cause_code_p) == DS3G_FAILURE)
    {
      return FALSE;
    }
    //  Need to check data rate before configuring the field mask.

    if(nw_bk->curr_qos.rx.flow_template.req_flow.field_mask)                        
    {
      if(nw_bk->curr_qos.rx.fltr_template.num_filters > 0)
      {
        nw_bk->curr_qos.field_mask |= ((qos_spec_field_mask_type)
                                            QOS_MASK_RX_FLOW);
      }
    }
    if(nw_bk->curr_qos.tx.flow_template.req_flow.field_mask)                        
    {
      if(nw_bk->curr_qos.tx.fltr_template.num_filters > 0)
      {	  
        nw_bk->curr_qos.field_mask |= ((qos_spec_field_mask_type)
                                            QOS_MASK_TX_FLOW);
      } 
    } 
  }/* mode_info_p != NULL */
  /*-----------------------------------------------------------------------
    If deletion of filter has made num_filters 0 in some direction. 
    We need to Update IP_FLOW_MASK_NONE so that framework deletes all filters.
  -----------------------------------------------------------------------*/
  if( (torig_rx_filter_cnt > 0) &&
      (nw_bk->curr_qos.rx.fltr_template.num_filters == 0))
  {
    DS_3GPP_MSG1_HIGH("Deleted all (%d) filters in RX direction",torig_rx_filter_cnt);
    nw_bk->curr_qos.field_mask |= QOS_MASK_RX_FLOW;
    nw_bk->curr_qos.rx.flow_template.req_flow.field_mask = (ipflow_field_mask_type)IPFLOW_MASK_NONE;
  }
  if( (torig_tx_filter_cnt > 0) &&
      (nw_bk->curr_qos.tx.fltr_template.num_filters == 0))
  {
    DS_3GPP_MSG1_HIGH("Deleted all (%d) filters in TX direction",torig_tx_filter_cnt);
    nw_bk->curr_qos.field_mask |= QOS_MASK_TX_FLOW;
    nw_bk->curr_qos.tx.flow_template.req_flow.field_mask = (ipflow_field_mask_type)IPFLOW_MASK_NONE;
  }
  /*-----------------------------------------------------------------------
    Update the new Flow with the filter information.
  -----------------------------------------------------------------------*/                               
  memset(&ps_flow_param, 0, sizeof(ps_flow_modify_param_type));
  ps_flow_param.qos_spec            = &(nw_bk->curr_qos);
  ps_flow_param.flow_validate_f_ptr = NULL;
  ps_flow_param.fltr_validate_f_ptr = NULL;
  ps_flow_param.fltr_priority       = PS_IFACE_IPFLTR_PRIORITY_FCFS;
  ps_flow_param.subset_id           = 5; 
  
  /*-------------------------------------------------------------------
    Disable Filtering on the Flow.
  ---------------------------------------------------------------------*/
  if(ds_flow_context_ps_flow_filtering_control(flow_context_p,FALSE)!= TRUE)
  {
    DS_3GPP_MSG0_ERROR("flow filtering control failed");
    //return FALSE;
  }

  /*-----------------------------------------------------------------------
    Invoke the Flow Modify.
  -----------------------------------------------------------------------*/
  ret_val = ps_iface_modify_flow(iface_ptr,
  (flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr),&ps_flow_param, &ds_errno);
  if(ret_val == -1)
  {
    DS_3GPP_MSG0_ERROR("modify Flow failed");
    /*-------------------------------------------------------------------
      Enable Filtering on the Flow.
    ---------------------------------------------------------------------*/
    if(ds_flow_context_ps_flow_filtering_control(flow_context_p,TRUE)!= TRUE)
    {
      DS_3GPP_MSG0_ERROR("flow filtering control failed");
     // return FALSE;
    }
    return FALSE;
  }

  if((nw_bk->flow_cntx_fltr_id != 0) ||
     (nw_tft->operation_code ==(byte) DS_3GPP_TFT_OPCODE_CREATE))
  {
    /*-------------------------------------------------------------------
      Update the filter pointers and filter count to the original cnt
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.list_ptr  = nw_bk->curr_rx_fltr;
    nw_bk->curr_qos.tx.fltr_template.list_ptr  = nw_bk->curr_tx_fltr;
    /*-------------------------------------------------------------------
      Store the current filter count
    ---------------------------------------------------------------------*/
    torig_rx_filter_cnt = nw_bk->curr_qos.rx.fltr_template.num_filters;
    torig_tx_filter_cnt = nw_bk->curr_qos.tx.fltr_template.num_filters;
    /*-------------------------------------------------------------------
      Retrieve the filter count before modification
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.num_filters = orig_rx_filter_cnt;
    nw_bk->curr_qos.tx.fltr_template.num_filters = orig_tx_filter_cnt;
    DS_3GPP_MSG3_HIGH("Store the current filter count: %d filter count before modification: %d", 
  	torig_rx_filter_cnt, orig_rx_filter_cnt, nw_bk->flow_cntx_fltr_id );
    /*-------------------------------------------------------------------
      clear Precedence and Filter ID information.
    ---------------------------------------------------------------------*/
    if(ds_flow_manager_update_filter_id_precedence(
                                              pdn_context_p,
                                              bearer_context_p,
                                              flow_context_p,
                                              nw_bk->flow_cntx_fltr_id,
                                              &(nw_bk->curr_qos),
                                              TRUE) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("update filter id and precedence failed");
      return FALSE;
    }
    /*-------------------------------------------------------------------
      Update the filter count to current count
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.num_filters = torig_rx_filter_cnt;
    nw_bk->curr_qos.tx.fltr_template.num_filters = torig_tx_filter_cnt;
    /*-------------------------------------------------------------------
      Update the filter pointer.
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.list_ptr    = nw_bk->mod_rx_fltr;
    nw_bk->curr_qos.tx.fltr_template.list_ptr    = nw_bk->mod_tx_fltr;  
  } /* nw_bk->flow_cntx_fltr_id != 0*/

  if(ds_flow_manager_update_filter_id_precedence(pdn_context_p,
                                            bearer_context_p,
                                            flow_context_p,
                                            0,
                                            &(nw_bk->curr_qos), 
                                            FALSE) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("update filter id and precedence failed");
    return FALSE;
  }

  if((flow_context_p->ds_flow_context_dyn_p->flow_type & DS_3GPP_NW_FLOW_TYPE) 
     == DS_3GPP_NW_FLOW_TYPE)
  {
    ds_flow_context_set_flow_state(flow_context_p,
                                     DS_FLOW_CONTEXT_STATE_PENDING_LL_CONFIRM);
  }
  else
  {  
    /*-------------------------------------------------------------------
      Modify Flow Accepted.
    ---------------------------------------------------------------------*/
    if(ds_flow_context_ps_flow_modify_result(
                                      flow_context_p,TRUE,ps_eic)!=TRUE)
    {
      DS_3GPP_MSG0_ERROR("flow modify result failed");
      return FALSE;
    }
  
    /*-------------------------------------------------------------------
      Enable Filtering on the Flow.
    ---------------------------------------------------------------------*/
    if(ds_flow_context_ps_flow_filtering_control(flow_context_p,TRUE)!= TRUE)
    {
      DS_3GPP_MSG0_ERROR("flow filtering control failed");
      //return FALSE;
    }
  }

  return TRUE;
} /* ds_flow_manager_replace_filter_in_flow */


/*===========================================================================
FUNCTION DS_FLOW_MANAGER_DETECT_CONFLICT_IN_TFT

DESCRIPTION
  Detects precedence and identifier conflicts on the Nw input.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow context information
  nw_tft                -  TFT information
  nw_bk                 -  Network book keeping info.
  cause_code_p          -  Cause code NW TFT is not accurate

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_flow_manager_detect_conflict_in_tft
(
  ds_pdn_context_s                      *pdn_context_p,
  ds_bearer_context_s                   *bearer_context_p,
  ds_flow_context_s                     *flow_context_p,
  tft_type_T                            *nw_tft,
  ds_3gpp_nw_modify_qos_bk_s            *nw_bk,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p,
  boolean                                del_filter
)
{

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
    Verify Input params
  -----------------------------------------------------------------------*/
  if((pdn_context_p==NULL)||(bearer_context_p==NULL)||(nw_tft == NULL)
     ||(cause_code_p== NULL)||(flow_context_p == NULL)||(nw_bk==NULL)||
     (!ds_flow_context_validate_flow(flow_context_p)))
  {
    DS_3GPP_MSG0_ERROR("Input params are NULL in addFilterToTft");
    if(cause_code_p != NULL)
    {
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    }
    return FALSE;
  }
  /*-------------------------------------------------------------------
    Check if the new fltr has any fltr ID or fltr preced conflicts.
  -------------------------------------------------------------------*/
  if(ds_3gpp_tft_check_nw_fltr_id_preced(pdn_context_p,
                                         bearer_context_p,
                                         flow_context_p,
                                         nw_tft,
                                         nw_bk,
                                         del_filter) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Invalid Filter Input");
    *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
    return FALSE;
  }

  /*-------------------------------------------------------------------
    Check if there is a precedence conflict and make sure there is no
    conlfict on the bearer with modification
  -------------------------------------------------------------------*/
  if(nw_bk->preced_conflict_cnt != 0)
  {
    if(ds_3gpp_tft_find_bearer_with_preced_conflict(pdn_context_p,
                                                   bearer_context_p,
                                                   flow_context_p,
                                                   nw_bk) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Un expected filter precedence error ");
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
      return FALSE;
    }

    if(ds_3gpp_tft_preced_conflict_on_bearer(
                             nw_bk,(void *)bearer_context_p)==TRUE)
    {
      /* When op code is not create and precendence conflict is on the same bearer
      for now continuing to make it a semantic error; still understanding the best 
      way to resolve; but when opcode is create we are setting nw_bk->prec_conf_on_same_bearer
      and not returing an error */
      if( nw_tft->operation_code !=(byte) DS_3GPP_TFT_OPCODE_CREATE )
      {
          DS_3GPP_MSG0_HIGH("preced_conflict on same bearer and"
                            " op code is add or replace ");
          nw_bk->prec_conf_on_same_bearer_modify = TRUE;
      }          
      else
      {
         DS_3GPP_MSG0_HIGH("preced_conflict on same bearer and "
                           " op code is create"); 
         nw_bk->prec_conf_on_same_bearer_create = TRUE;
      }
    }          
  } /* if preced_conflict_cnt != 0 */
  if(nw_bk->is_add == TRUE)
  {
    /*-------------------------------------------------------------------
      Verify flow filter id coflict when it is new flow.
    -------------------------------------------------------------------*/     
    if((nw_bk->new_flow == TRUE) && (nw_bk->flow_cntx_fltr_id != 0))
    {
      DS_3GPP_MSG1_ERROR("filter ID conflict on filters 0x%x",
                            nw_bk->flow_cntx_fltr_id);
      *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
      return FALSE; 
    }
  }
  return TRUE;
}/* ds_flow_manager_detect_conflict_in_tft */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_ADD_REPLACE_FILTER_TO_TFT_HELPER

DESCRIPTION
  Add filters to existing TFT.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow context information
  nw_tft                -  TFT information
  nw_bk                 -  Network book keeping info.
  mode_info_p           -  Mode specific Qos.
  cause_code_p          -  Cause code NW TFT is not accurate

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_flow_manager_add_replace_filter_to_tft_helper
(
  ds_pdn_context_s                      *pdn_context_p,
  ds_bearer_context_s                   *bearer_context_p,
  ds_flow_context_s                     *flow_context_p,
  tft_type_T                            *nw_tft,
  ds_3gpp_nw_modify_qos_bk_s            *nw_bk,
  const cm_call_mode_info_s_type              *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
    Verify Input params
  -----------------------------------------------------------------------*/
  if((pdn_context_p==NULL)||(bearer_context_p==NULL)||(nw_tft == NULL)
     ||(cause_code_p== NULL)||(flow_context_p == NULL)||(nw_bk==NULL)||
     (!ds_flow_context_validate_flow(flow_context_p)))
  {

    DS_3GPP_MSG3_ERROR("PDN:0x%x:Bearer:0x%x:nw_bk:0x%x",pdn_context_p,
                       bearer_context_p,nw_bk);

    DS_3GPP_MSG2_ERROR("nw_tft:0x%x cause_code_p:0x%x",nw_tft,
                        cause_code_p);

    if(cause_code_p != NULL)
    {
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    }
    return FALSE;
  }
  /*-------------------------------------------------------------------
    Check if the network fltr has any fltr ID or fltr preced conflicts.
  -------------------------------------------------------------------*/
  if(ds_flow_manager_detect_conflict_in_tft(pdn_context_p,
                                         bearer_context_p,
                                         flow_context_p,
                                         nw_tft,
                                         nw_bk,
                                         cause_code_p,
                                         FALSE) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Unresolved Conflict in TFT");
    return FALSE;
  }

  if(nw_bk->is_add == TRUE)
  {
    /*-------------------------------------------------------------------
      Add the filters to this flow.
    -------------------------------------------------------------------*/        
    if(ds_flow_manager_add_filter_to_flow(
                                  pdn_context_p,
                                  bearer_context_p,
                                  flow_context_p,
                                  nw_tft,
                                  nw_bk,
                                  mode_info_p,
                                  nw_bk->new_flow,
                                  cause_code_p)!= TRUE)
    { 
      return FALSE;
    }
  }/* is_add == TRUE */
  else
  {
    /*-------------------------------------------------------------------
      Replace the filters to this flow.
    -------------------------------------------------------------------*/        
    if(ds_flow_manager_replace_filter_in_flow(
                                  pdn_context_p,
                                  bearer_context_p,
                                  flow_context_p,
                                  nw_tft,
                                  nw_bk,
                                  mode_info_p,
                                  cause_code_p)!= TRUE)
    { 
      return FALSE;
    }
  }/* is_add == FALSE*/
  /*-------------------------------------------------------------------
     Resolve any filter conflict.
  ---------------------------------------------------------------------*/
  if(ds_flow_manager_handle_fltr_conflict(pdn_context_p,
                                         bearer_context_p,
                                         flow_context_p,
                                         nw_bk) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("unable to handle filter conflict");
    return FALSE;
  }
  return TRUE;
}/* ds_flow_manager_add_replace_filter_to_tft_helper */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_ADD_FILTER_TO_TFT

DESCRIPTION
  Add filters to existing TFT.

PARAMETERS
  pdn_context_p         -  PDN information.
  bearer_context_p      -  Bearer information.
  flow_context_p        -  Flow context information.
  nw_tft                -  TFT information.
  mode_info_p           -  Mode specific Qos.
  cause_code_p          -  Cause code NW TFT is not accurate.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_add_filter_to_tft
(
  ds_pdn_context_s                      *pdn_context_p,
  ds_bearer_context_s                   *bearer_context_p,
  ds_flow_context_s                     *flow_context_p,
  tft_type_T                            *nw_tft,
  const cm_call_mode_info_s_type        *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p,
  boolean                                allow_flow_create
)
{
  ds_flow_context_s            *flow_cntxv4 = NULL;
  ds_flow_context_s            *flow_cntxv6 = NULL; 
  ds_3gpp_nw_modify_qos_bk_s   *nw_bk;
  ip_version_enum_type         ip_type;
  ds_3gpp_iface_s              *ds_iface_ptr = NULL;
  ps_iface_type                *iface_ptr = NULL;
  ps_extended_info_code_enum_type ps_eic = PS_EIC_NOT_SPECIFIED;
  boolean                      return_value = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
    Verify Input params
  -----------------------------------------------------------------------*/
  if((!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
     ||(nw_tft == NULL)||(cause_code_p== NULL))
  {
    if(cause_code_p != NULL)
    {
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    }
    return FALSE;
  }

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }
  /*-----------------------------------------------------------------------
    Check if this is NW initiated Qos Mod, or NW response to UE request
  -----------------------------------------------------------------------*/
  if(ds_flow_context_validate_flow(flow_context_p))
  {
    DS_3GPP_MSG1_HIGH("Add filter to TFT for flow:0x%x",flow_context_p);
  }
  else
  {
    DS_3GPP_MSG0_HIGH("Add filter to TFT for NW flow");
  }
  
  /*-----------------------------------------------------------------------
     Check for TFT Semantic Errors
  -----------------------------------------------------------------------*/
  if(nw_tft->valid)
  {
    if(ds_3gpp_tft_check_mod_fltr_semantic_error(nw_tft) == TRUE)
    {
      *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
      return FALSE;
    }
    else if(ds_3gpp_tft_check_for_repeated_precedence(nw_tft) == TRUE)
    {
      *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
      return FALSE;
    }
  }
  /*-----------------------------------------------------------------------
     Allocate Heap Memory
  -----------------------------------------------------------------------*/
  nw_bk = (ds_3gpp_nw_modify_qos_bk_s *)modem_mem_alloc(
                                       sizeof(ds_3gpp_nw_modify_qos_bk_s),
                                       MODEM_MEM_CLIENT_DATA);
  if(nw_bk == NULL)
  {
    DS_3GPP_MSG2_ERROR("Heap Memory Allocation failed for client: %d size: %d", 
       MODEM_MEM_CLIENT_DATA,(sizeof(ds_3gpp_nw_modify_qos_bk_s)));
    *cause_code_p =  DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  }
  memset(nw_bk, 0,sizeof(ds_3gpp_nw_modify_qos_bk_s));

  if(flow_context_p == NULL)
  {
    switch(ds_pdn_cntxt_get_pdn_context_ip_support(pdn_context_p))
    {
      case DS_UMTS_PDP_IPV4:
        flow_cntxv4 = ds_bearer_flow_manager_find_nw_flow_context(
                                 bearer_context_p,DS_3GPP_V4_FLOW_TYPE);
        if(flow_cntxv4 == NULL)
        {
          DS_3GPP_MSG0_ERROR("No previous NW flow context associated flow manager");
          if(!allow_flow_create)
          {
            // flow create is not allowed, this is a BCM violation, nw tries to 
            // install tft on a UE init flow
            *cause_code_p = DS_3GPP_INTERNAL_BCM_VIOLATION;
            modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
            return DS3G_FAILURE;
          }
          /* 
           There is a race condition here, the flow could be getting
           deleted on the PDN, while this request comes thru. In this case
           we bearer flow manager will have 0 flows.
           One other race condition is the between when UE wants to delete
           the bearer, and NW wants to add to this.
          */
          /*-----------------------------------------------------------------
             Retrieve an empty flow context.
          -----------------------------------------------------------------*/
          flow_cntxv4 = ds_flow_context_allocate_flow_context();
          if((flow_cntxv4 == NULL) ||(!ds_flow_context_validate_flow(flow_cntxv4)))
          {
            *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
            modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
            return DS3G_FAILURE;
          }
          else
          { 
            DS_3GPP_MSG0_HIGH("Allocated a free V4 flow context");
            if(ds_flow_context_configure_flow_context(
                                                  flow_cntxv4,
                                                  pdn_context_p,
                                                  bearer_context_p,
                              (DS_3GPP_NW_FLOW_TYPE|DS_3GPP_V4_FLOW_TYPE),
                                  DS_3GPP_PDN_CONTEXT_IPV4_IFACE) == FALSE)
            {
              DS_3GPP_MSG0_ERROR("Error configuring v4 flow context.");
              *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
              modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
              return FALSE;
            }
            nw_bk->new_flow = TRUE;
          }/* else */
        nw_bk->is_add = TRUE;   
        }/*flow_cntxv4 == NULL*/
		
        else if( nw_tft->operation_code == DS_3GPP_TFT_OPCODE_ADD)
        {
          nw_bk->is_add = TRUE; 
        }
        else 
        {
          nw_bk->is_add = FALSE;
        }

        /*-------------------------------------------------------------------
          Invoke the add filter to tft helper.
        -------------------------------------------------------------------*/
        return_value = ds_flow_manager_add_replace_filter_to_tft_helper(
                                                    pdn_context_p,
                                                    bearer_context_p,
                                                    flow_cntxv4,
                                                    nw_tft,
                                                    nw_bk,
                                                    mode_info_p,
                                                    cause_code_p);
        //If this is a new flow then we created a new TFT.
        //For the default bearer; we need to treat this differently by disallowing
        //data on the default flow.
        if ((return_value == TRUE) && (TRUE == nw_bk->new_flow))
        {
          if (TRUE == bearer_context_p->ds_bearer_context_dyn_p->is_default)
          {
            int i;
            // Packets not matching the installed filters will be dropped.
            for (i = 0; i < DS_3GPP_MAX_IFACE_PER_PDN_CONTEXT; i++)
            {
              ds_3gpp_iface_s *iface_p = pdn_context_p->ds_pdn_context_dyn_p
                ->ds_3gpp_iface_tbl[i];

              if (iface_p != NULL)
              {      
                ps_iface_type *ps_iface = &(iface_p->ps_iface);
                PS_FLOW_SET_CAPABILITY(PS_IFACE_GET_DEFAULT_FLOW(ps_iface), 
                                       PS_FLOW_CAPABILITY_DATA_DISALLOWED);        
              }
            }
          }
        }
        
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);    
        return return_value;

      case DS_UMTS_PDP_IPV6:
        flow_cntxv6 = ds_bearer_flow_manager_find_nw_flow_context(
                                bearer_context_p,DS_3GPP_V6_FLOW_TYPE);
        if(flow_cntxv6 == NULL)
        {
          DS_3GPP_MSG0_ERROR("No previous NW flow context associated flow manager");
          if(!allow_flow_create)
          {
            // flow create is not allowed, this is a BCM violation, nw tries to 
            // install tft on a UE init flow
            *cause_code_p = DS_3GPP_INTERNAL_BCM_VIOLATION;
            modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
            return DS3G_FAILURE;
          }
          /*-----------------------------------------------------------------
             Retrieve an empty flow context.
          -----------------------------------------------------------------*/
          flow_cntxv6 = ds_flow_context_allocate_flow_context();
          if(!ds_flow_context_validate_flow(flow_cntxv6))
          {
            DS_3GPP_MSG0_ERROR(" No free flow contexts");
            *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
            modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
            return DS3G_FAILURE;
          }
          else
          { 
            DS_3GPP_MSG0_HIGH("Allocated a free v6 flow context");
            if(ds_flow_context_configure_flow_context(
                                                  flow_cntxv6,
                                                  pdn_context_p,
                                                  bearer_context_p,
                              (DS_3GPP_NW_FLOW_TYPE|DS_3GPP_V6_FLOW_TYPE),
                                  DS_3GPP_PDN_CONTEXT_IPV6_IFACE) == FALSE)
            {
              DS_3GPP_MSG0_ERROR("Error configuring v6 flow context.");
              *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
              modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
              return FALSE;
            }
            nw_bk->new_flow = TRUE;
          }
        nw_bk->is_add = TRUE; 
        }/*flow_cntxv6 == NULL*/ 
		
        else if( nw_tft->operation_code == DS_3GPP_TFT_OPCODE_ADD)
        {
          nw_bk->is_add = TRUE; 
        }
        else 
        {
          nw_bk->is_add = FALSE;
        }
        /*-------------------------------------------------------------------
          Invoke the add filter to tft helper.
        -------------------------------------------------------------------*/
        return_value = ds_flow_manager_add_replace_filter_to_tft_helper(
                                                    pdn_context_p,
                                                    bearer_context_p,
                                                    flow_cntxv6,
                                                    nw_tft,
                                                    nw_bk,
                                                    mode_info_p,
                                                    cause_code_p);
        
        //If this is a new flow then we created a new TFT.
        //For the default bearer; we need to treat this differently by disallowing
        //data on the default flow.
        if ((return_value == TRUE) && (TRUE == nw_bk->new_flow))
        {
          if (TRUE == bearer_context_p->ds_bearer_context_dyn_p->is_default)
          {
            int i;
            // Packets not matching the installed filters will be dropped.
            for (i = 0; i < DS_3GPP_MAX_IFACE_PER_PDN_CONTEXT; i++)
            {
              ds_3gpp_iface_s *iface_p = pdn_context_p->ds_pdn_context_dyn_p
                ->ds_3gpp_iface_tbl[i];

              if (iface_p != NULL)
              {      
                ps_iface_type *ps_iface = &(iface_p->ps_iface);
                PS_FLOW_SET_CAPABILITY(PS_IFACE_GET_DEFAULT_FLOW(ps_iface), 
                                       PS_FLOW_CAPABILITY_DATA_DISALLOWED);        
              }
            }
          }
        }

        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);

        return return_value;
      case DS_UMTS_PDP_IPV4V6:
        /*-----------------------------------------------------------------
          Handle it for V4
        -----------------------------------------------------------------*/
        flow_cntxv4 = ds_bearer_flow_manager_find_nw_flow_context(
                                 bearer_context_p,DS_3GPP_V4_FLOW_TYPE);
        if(flow_cntxv4 == NULL)
        {
          DS_3GPP_MSG0_ERROR("No previous NW flow context associated flow manager");
          if(!allow_flow_create)
          {
            // flow create is not allowed, this is a BCM violation, nw tries to 
            // install tft on a UE init flow
            *cause_code_p = DS_3GPP_INTERNAL_BCM_VIOLATION;
            modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
            return DS3G_FAILURE;
          }
          /*-----------------------------------------------------------------
             Retrieve an empty flow context.
          -----------------------------------------------------------------*/
          flow_cntxv4 =ds_flow_context_allocate_flow_context();
          if(!ds_flow_context_validate_flow(flow_cntxv4))
          {
            *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
            modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
            return FALSE;
          }
          else
          {  
            DS_3GPP_MSG0_HIGH("Allocated a free V4 flow context");
            if(ds_flow_context_configure_flow_context(
                                                  flow_cntxv4,
                                                  pdn_context_p,
                                                  bearer_context_p,
                              (DS_3GPP_NW_FLOW_TYPE|DS_3GPP_V4_FLOW_TYPE),
                                  DS_3GPP_PDN_CONTEXT_IPV4_IFACE) == FALSE)
            {
              DS_3GPP_MSG0_ERROR("Error configuring v4 flow context.");
              *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
              modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
              return FALSE;
            }
            nw_bk->new_flow = TRUE;
          }
          nw_bk->is_add = TRUE;   
        }/*flow_cntxv4 == NULL*/
        else if( nw_tft->operation_code == DS_3GPP_TFT_OPCODE_ADD)
        {
          nw_bk->is_add = TRUE; 
        }
        else
        {
          nw_bk->is_add = FALSE; 
        }
        /*-----------------------------------------------------------------
          Handle it for V6
        -----------------------------------------------------------------*/
        flow_cntxv6 = ds_bearer_flow_manager_find_nw_flow_context(
                                bearer_context_p,DS_3GPP_V6_FLOW_TYPE);
        if(flow_cntxv6 == NULL)
        {
          if(!allow_flow_create)
          {
            // flow create is not allowed, this is a BCM violation, nw tries to 
            // install tft on a UE init flow
            *cause_code_p = DS_3GPP_INTERNAL_BCM_VIOLATION;
            modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
            return DS3G_FAILURE;
          }
          DS_3GPP_MSG0_ERROR("No previous NW flow context associated flow manager");
          if(nw_bk->new_flow != TRUE)
          {
            DS_3GPP_MSG0_ERROR("Mismatch of Network flows");
            *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
            modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
            return FALSE;
          }
          /*-----------------------------------------------------------------
             Retrieve an empty flow context.
          -----------------------------------------------------------------*/
          flow_cntxv6 = ds_flow_context_allocate_flow_context();
          if(!ds_flow_context_validate_flow(flow_cntxv6))
          {
            *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
            modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
            return DS3G_FAILURE;
          }
          else
          { 
            DS_3GPP_MSG0_HIGH("Allocated a free V6 flow context");
            if(ds_flow_context_configure_flow_context(
                                                  flow_cntxv6,
                                                  pdn_context_p,
                                                  bearer_context_p,
                              (DS_3GPP_NW_FLOW_TYPE|DS_3GPP_V6_FLOW_TYPE),
                                  DS_3GPP_PDN_CONTEXT_IPV6_IFACE) == FALSE)
            {
              DS_3GPP_MSG0_ERROR("Error configuring v6 flow context.");
              *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
              modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
              return FALSE;
            }
            nw_bk->new_flow = TRUE;
          }
          nw_bk->is_add = TRUE;   
        }/*flow_cntxv6 == NULL*/
        else if(nw_bk->new_flow == TRUE)
        {
          DS_3GPP_MSG0_ERROR("Mismatch of Network flows");
          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
          return FALSE;
        }
	else if( nw_tft->operation_code == DS_3GPP_TFT_OPCODE_ADD)
        {
          nw_bk->is_add = TRUE; 
        }
        else
        {
          nw_bk->is_add = FALSE;     
        }
        /*-------------------------------------------------------------------
          Check if the network fltr has any fltr ID or fltr preced conflicts.
        -------------------------------------------------------------------*/
        if(ds_flow_manager_detect_conflict_in_tft(pdn_context_p,
                                         bearer_context_p,
                                         flow_cntxv4,
                                         nw_tft,
                                         nw_bk,
                                         cause_code_p,
                                         FALSE) == FALSE)
        {
          DS_3GPP_MSG0_ERROR("Unresolved Conflict in TFT");
          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return FALSE;
        }
        /*-------------------------------------------------------------------
           Add the filters to V4 and V6 flows if there is no previous NW flow
           or opcode is add, Otherwise Replace the filters from V4 and V6 flow 
           with new filters 
        -------------------------------------------------------------------*/        
        if(nw_bk->is_add == TRUE)
	{
          if(ds_flow_manager_add_filter_to_flow(
                                  pdn_context_p,
                                  bearer_context_p,
                                  flow_cntxv4,
                                  nw_tft,
                                  nw_bk,
                                  mode_info_p,
                                  nw_bk->new_flow,
                                  cause_code_p)!= TRUE)
        { 
          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return FALSE;
        }
     
          if(ds_flow_manager_add_filter_to_flow(
                                  pdn_context_p,
                                  bearer_context_p,
                                  flow_cntxv6,
                                  nw_tft,
                                  nw_bk,
                                  mode_info_p,
                                  nw_bk->new_flow,
                                  cause_code_p)!= TRUE)
          { 
            modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
            return FALSE;
          }
        }
        else
        {
          if(ds_flow_manager_replace_filter_in_flow(
                                           pdn_context_p,
                                           bearer_context_p,
                                           flow_cntxv4,
                                           nw_tft,
                                           nw_bk,
                                           mode_info_p,
                                           cause_code_p)!= TRUE)
          { 
            modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
            return FALSE;
          } 

          if(ds_flow_manager_replace_filter_in_flow(
                                  pdn_context_p,
                                  bearer_context_p,
                                  flow_cntxv6,
                                  nw_tft,
                                  nw_bk,
                                  mode_info_p,
                                  cause_code_p)!= TRUE)
          { 
            modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
            return FALSE;
          }
        }
        /*-------------------------------------------------------------------
          Resolve any filter conflict.
        ---------------------------------------------------------------------*/
        if(ds_flow_manager_handle_fltr_conflict(pdn_context_p,
                                         bearer_context_p,
                                         flow_cntxv6,
                                         nw_bk) == FALSE)
        {
          DS_3GPP_MSG0_ERROR("unable to handle filter conflict");
          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return FALSE;
        }
        
        //If this is a new flow then we created a new TFT.
        //For the default bearer; we need to treat this differently by disallowing
        //data on the default flow.
        if (TRUE == nw_bk->new_flow)
        {
          if (TRUE == bearer_context_p->ds_bearer_context_dyn_p->is_default)
          {
            int i;
            // Packets not matching the installed filters will be dropped.
            for (i = 0; i < DS_3GPP_MAX_IFACE_PER_PDN_CONTEXT; i++)
            {
              ds_3gpp_iface_s *iface_p = pdn_context_p->ds_pdn_context_dyn_p
                ->ds_3gpp_iface_tbl[i];

              if (iface_p != NULL)
              {      
                ps_iface_type *ps_iface = &(iface_p->ps_iface);
                PS_FLOW_SET_CAPABILITY(PS_IFACE_GET_DEFAULT_FLOW(ps_iface), 
                                       PS_FLOW_CAPABILITY_DATA_DISALLOWED);        
              }
            }
          }
        }
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return TRUE;
      default:
        DS_3GPP_MSG0_ERROR("Unknown IP type");
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return FALSE;
    }/* switch */ 
  }/* flow_context_p == NULL */
  else
  {
    if(!ds_flow_context_validate_flow(flow_context_p))
    {
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return FALSE;
    }

    /*-------------------------------------------------------------------
      This is a response to UE requested modification, or a UMTS NW init
      QoS (which is treated like a UE request)
    -------------------------------------------------------------------*/  
    /*-------------------------------------------------------------------------
      Configure the QOS flow spec
    -------------------------------------------------------------------------*/
    if(ds_3gpp_tft_configure_nw_bk_flow_spec(nw_bk)==FALSE)
    {
      DS_3GPP_MSG0_ERROR("unable to configure the nw book-keeping info");
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return FALSE;
    } 
    /*-------------------------------------------------------------------
      If tft is present in the NW response, we will perform checks
    -------------------------------------------------------------------*/   
    if(nw_tft->valid)
    {
      /*-------------------------------------------------------------------
        Verify if requested Filters are present in the NW response.
      -------------------------------------------------------------------*/
      if(ds_3gpp_tft_verify_nw_tft(&(flow_context_p->ds_flow_context_dyn_p->tft_mod),
                                   nw_tft, flow_context_p) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("Nw response does not match UE request");
        *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return FALSE; 
      }

      //Update the QOS in the request with what the network granted. 
      if(ds_bearer_flow_manager_convert_data_rate(bearer_context_p,
                                       mode_info_p,
                                       &(nw_bk->curr_qos),
                                       cause_code_p) == DS3G_FAILURE)
      {
        DS_3GPP_MSG0_ERROR("Unable to process the QOS granted by NW");
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return FALSE;
      }

      nw_bk->is_add = TRUE;
      /*-------------------------------------------------------------------
        Check if the network fltr has any fltr ID or fltr preced conflicts.
      -------------------------------------------------------------------*/
      if(ds_flow_manager_detect_conflict_in_tft(pdn_context_p,
                                             bearer_context_p,
                                             flow_context_p,
                                             nw_tft,
                                             nw_bk,
                                             cause_code_p,
                                             FALSE) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("Unresolved Conflict in TFT");
        *cause_code_p =DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_TFT;
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return FALSE;
      }
  
      /*-------------------------------------------------------------------
        In case of Add we can have the case where NW sends more filters than 
        requested, which need to be added to the NW flow context.
       ---------------------------------------------------------------------*/
  
  
      if(flow_context_p->ds_flow_context_dyn_p->state == 
         DS_FLOW_CONTEXT_STATE_PEND_ADD_RSP)
      {
        /*-------------------------------------------------------------------
          Retrieve modify filter info for flow context.
        ---------------------------------------------------------------------*/
        if(ds_flow_context_ps_flow_get_filters(
                                  flow_context_p,
                                  TRUE,
                                 &(nw_bk->curr_qos)) == FALSE)
        {
          DS_3GPP_MSG0_ERROR("Could not retrieve previous filters");
          *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return FALSE; 
        }
      }/* state == PEND_ADD_RSP*/
      else
      {
       /*-------------------------------------------------------------------
          Retrieve requested filter info for flow context. 
          Case where flow is added to old bearer.
        ---------------------------------------------------------------------*/
        if(ds_flow_context_ps_flow_get_filters(
                                  flow_context_p,
                                  FALSE,
                                 &(nw_bk->curr_qos)) == FALSE)
        {
          DS_3GPP_MSG0_ERROR("Could not retrieve previous filters");
          *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return FALSE; 
        }
      }/* else state != PEND_ADD_RSP*/
      /*-----------------------------------------------------------------------
        Find the right iface pointer.
       -----------------------------------------------------------------------*/ 
      if(ds_flow_context_retrieve_iface_ip_type(flow_context_p,
                                              &ip_type,
                                              &iface_ptr) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("3GPP iface is not present");
        *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return FALSE;
      }
    
      /*-------------------------------------------------------------------
        Convert NW TFT to PS filters
      ---------------------------------------------------------------------*/
      if(ds_flow_manager_convert_nw_tft_to_app_tft(nw_tft,
  	              &(nw_bk->mod_qos), ip_type, flow_context_p, cause_code_p) 
                                                            == DS3G_FAILURE)
      {
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return FALSE;
      }
      /*-------------------------------------------------------------------  
        Find the pending filters and update the filter identifier value
      ---------------------------------------------------------------------*/
      if ((ds_flow_context_update_filter_identifier(
                             &(nw_bk->curr_qos.rx.fltr_template),
                             &(nw_bk->mod_qos.rx.fltr_template))) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("Req RX filters do not match NW granted RX filters");
        *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return FALSE;
      }
      if ((ds_flow_context_update_filter_identifier(
                             &(nw_bk->curr_qos.tx.fltr_template),
                             &(nw_bk->mod_qos.tx.fltr_template))) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("Req TX filters do not match NW granted TX filters");
        *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return FALSE;
      }
      if(flow_context_p->ds_flow_context_dyn_p->state == DS_FLOW_CONTEXT_STATE_PEND_ADD_RSP)
      {
        /*---------------------------------------------------------------------
          Update Flow Context with the Qos
        ---------------------------------------------------------------------*/
        if(nw_bk->curr_qos.rx.fltr_template.num_filters > 0)
        {
          if(ps_flow_set_rx_filter_spec(
                        iface_ptr,
                        flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr,
                        &nw_bk->curr_qos.rx.fltr_template,
                        TRUE) == FALSE)
          {
            DS_3GPP_MSG0_ERROR("Error setting RX filter spec");
            *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
            modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
            return FALSE;
          }
        }
    
        if(nw_bk->curr_qos.tx.fltr_template.num_filters > 0)
        {
          if(ps_iface_flow_set_tx_filter_spec(
                        iface_ptr,
                        flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr,
                        &nw_bk->curr_qos.tx.fltr_template,
                        TRUE) == FALSE)
          {
            DS_3GPP_MSG0_ERROR("Error setting the TX filter spec");
            *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
            modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
            return FALSE;
          }
        }
      
        /*-------------------------------------------------------------------
          Confirm modification to PS frame work.
        -------------------------------------------------------------------*/
        ps_eic = PS_EIC_NOT_SPECIFIED;
        if (ds_flow_context_ps_flow_modify_result(
                       flow_context_p,TRUE,ps_eic) == FALSE)
        {
          DS_3GPP_MSG0_ERROR("Flow Modify accept failed");
          *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return FALSE;
        }
        /*-------------------------------------------------------------------
          Enable filtering on the flow.
        -------------------------------------------------------------------*/
        if(ds_flow_context_ps_flow_filtering_control(flow_context_p,TRUE)!= TRUE)
        {
          DS_3GPP_MSG0_ERROR("enabling filters failed");
        }
      }/* state == PEND_ADD_RSP*/
      else
      {
        /*---------------------------------------------------------------------
          Update Flow Context with the Qos
        ---------------------------------------------------------------------*/
        if(nw_bk->curr_qos.rx.fltr_template.num_filters > 0)
        {
          if(ps_flow_set_rx_filter_spec(
                        iface_ptr,
                        flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr,
                        &nw_bk->curr_qos.rx.fltr_template,
                        FALSE) == FALSE)
          {
            DS_3GPP_MSG0_ERROR("Error setting RX filter spec");
            *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
            modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
            return FALSE;
          }
        }
        
        if(nw_bk->curr_qos.tx.fltr_template.num_filters > 0)
        {
          if(ps_iface_flow_set_tx_filter_spec(
                        iface_ptr,
                        flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr,
                        &nw_bk->curr_qos.tx.fltr_template,
                        FALSE) == FALSE)
          {
            DS_3GPP_MSG0_ERROR("Error setting the TX filter spec");
            *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
            modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
            return FALSE;
          }
        }
      }/* Else */
    } /* if nw_tft is valid */
    else
    {
      /*-------------------------------------------------------------------
        NW did not provide tft in the response, use requested params
      ---------------------------------------------------------------------*/
      if(ds_flow_context_ps_flow_get_filters(
                                flow_context_p,
                                FALSE,
                               &(nw_bk->curr_qos)) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("Could not retrieve previous filters");
        *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return FALSE; 
      }

      //Update the QOS in the request with what the network granted. 
      if(ds_bearer_flow_manager_convert_data_rate(bearer_context_p,
                                       mode_info_p,
                                       &(nw_bk->curr_qos),
                                       cause_code_p) == DS3G_FAILURE)
      {
        DS_3GPP_MSG0_ERROR("Unable to process the QOS granted by NW");
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return FALSE;
      }

      /*-------------------------------------------------------------------
        If flow context is a NW flow - it is a NW init qos. For dual IP case,
        we need to extend v4 flow to v6 as well since only v4 flow is created
        upon request
      ---------------------------------------------------------------------*/
      if((ds_pdn_cntxt_get_pdn_context_ip_support(pdn_context_p) == DS_UMTS_PDP_IPV4V6) &&
         (flow_context_p->ds_flow_context_dyn_p->flow_type & DS_3GPP_NW_FLOW_TYPE))
      {
        ds_iface_ptr =
         pdn_context_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE];
        if(ds_iface_ptr == NULL)
        {
          DS_3GPP_MSG0_ERROR("Could not find v6 iface for dual IP PDN");
          *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return FALSE; 
        }
        if(ds_flow_manager_create_flow_context_for_v6_iface(
                                     pdn_context_p,
                                     bearer_context_p,
                                     &(flow_context_p->ds_flow_context_dyn_p->tft_mod),
                                     ds_iface_ptr,
                                     cause_code_p) != DS3G_SUCCESS)
        {
          DS_3GPP_MSG0_ERROR("Could not create v6 flow for dual IP pdn");
          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return FALSE;
        }
      }

    }
    /*-------------------------------------------------------------------
      Clear the requested precedence bit mask.
    -------------------------------------------------------------------*/
    if(ds_flow_manager_update_precedence_bit_mask(
                  pdn_context_p,flow_context_p,FALSE,FALSE,TRUE)==FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
    }
    /*-----------------------------------------------------------------
      Set the precedence and filter_identifier bit masks
    ------------------------------------------------------------------*/
    if(ds_flow_manager_update_filter_id_precedence(pdn_context_p,
        bearer_context_p,flow_context_p,0,&(nw_bk->curr_qos),FALSE)==FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error updating filter ID precedence");
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
    /*-------------------------------------------------------------------
      Bind the flow to phys link, if this is a new flow.
    ---------------------------------------------------------------------*/
    if(flow_context_p->ds_flow_context_dyn_p->state == DS_FLOW_CONTEXT_STATE_PEND_ALLOC_RSP)
    {
      /*-------------------------------------------------------------------
        Bind the flow to phys link, if this is a new flow.
      ---------------------------------------------------------------------*/
      ds_flow_context_ps_flow_bind_phys_link(flow_context_p);

      /*-------------------------------------------------------------------
        Post flow activated indication.
      -------------------------------------------------------------------*/
      ds_flow_context_ps_flow_activate_ind(flow_context_p,ps_eic );
    }
    /*-------------------------------------------------------------------
      Resolve any filter conflicts.
    ---------------------------------------------------------------------*/
    if(ds_flow_manager_handle_fltr_conflict(pdn_context_p,
                                         bearer_context_p,
                                         flow_context_p,
                                         nw_bk) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("unable to handle filter conflict");
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return FALSE;
    }

    ds_flow_context_set_flow_state(flow_context_p,
                DS_FLOW_CONTEXT_STATE_UP);
    modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
    return TRUE;
  } /* flow _context_p != NULL */

} /* ds_flow_manager_add_filter_to_tft */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_UPDATE_FILTER_ID_PRECEDENCE

DESCRIPTION
  Update the filter ID and precedence for the filters.

PARAMETERS
  pdn_context_p    - PDN information
  bearer_context_p - Bearer information
  flow_context_p   - Flow Context information.
  filter_id        - Filter to be removed.
  app_qos_spec     - Original filter spec.
  clear_bit        -  If True will clear the bit else will set.
DEPENDENCIES
  None.

RETURN VALUE
  boolean          - Returns true if filters are retrieved
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_update_filter_id_precedence
( 
 ds_pdn_context_s     *pdn_context_p,
 ds_bearer_context_s  *bearer_context_p,
 ds_flow_context_s    *flow_context_p, 
 uint32                filter_id,
 qos_spec_type        *app_qos_spec,
 boolean               clear_bit
)
{
  ip_filter_type            *fltr = NULL;
  int                        cnt = 0;       
  uint16                    identifier = 0;
  ds_bearer_flow_manager_s  *bflow_manager_p = NULL;
  ds_flow_manager_s         *flow_manager_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
      ||(bearer_context_p == NULL)||(app_qos_spec == NULL)||
     (!ds_flow_context_validate_flow(flow_context_p)))
  {
    return FALSE;
  }

  bflow_manager_p=ds_bearer_flow_manager_find_flow_manager(bearer_context_p);

  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return FALSE;
  }

  flow_manager_p = ds_flow_manager_find_flow_manager(pdn_context_p);
  if(!ds_flow_manager_validate_flow_manager(flow_manager_p))
  {
    return FALSE;
  }
  
  /*------------------------------------------------------------------------
    Loop thru all filters and update the filter identifier bit mask for RX
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt<app_qos_spec->rx.fltr_template.num_filters; cnt++)
  {
    fltr = &(app_qos_spec->rx.fltr_template.list_ptr[cnt]);
    /*---------------------------------------------------------------------
      NW Assigned filter id is in the last 4 bits.
    ---------------------------------------------------------------------*/    
    identifier = (fltr->ipfltr_aux_info.fi_id & (0xF));
    if (((1 << identifier) & filter_id) || (filter_id == 0))
    {
      if(clear_bit)
      {
        /*-------------------------------------------------------------------
          Clear the FI Identifier
        -------------------------------------------------------------------*/
        ds_3gpp_tft_clear_filter_identifier_bit(
                                 identifier,
               &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->fi_identifier));
        /*-------------------------------------------------------------------
          Clear the FI Identifier
        -------------------------------------------------------------------*/
        ds_3gpp_tft_clear_filter_identifier_bit(
                                 identifier,
                      &(flow_context_p->ds_flow_context_dyn_p->fi_identifier));  
        /*-------------------------------------------------------------------
          Clear the FI precedence
        -------------------------------------------------------------------*/
        ds_3gpp_tft_clear_filter_preced_bit(
                                 fltr->ipfltr_aux_info.fi_precedence,
                           (flow_manager_p->ds_flow_manager_dyn_p->fi_preced));
      }/* if clear_bit*/
      else
      {
        /*-------------------------------------------------------------------
          Set the FI Identifier
        -------------------------------------------------------------------*/
        ds_3gpp_tft_set_filter_identifier_bit(
                                 identifier,
              &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->fi_identifier));
        /*-------------------------------------------------------------------
          Set the FI Identifier
        -------------------------------------------------------------------*/
        ds_3gpp_tft_set_filter_identifier_bit(
                                 identifier,
                       &(flow_context_p->ds_flow_context_dyn_p->fi_identifier));  
        /*-------------------------------------------------------------------
          Set the FI precedence
        -------------------------------------------------------------------*/
        ds_3gpp_tft_set_filter_preced_bit(
                                 fltr->ipfltr_aux_info.fi_precedence,
                           (flow_manager_p->ds_flow_manager_dyn_p->fi_preced));
      }/* else clear_bit*/
    }/* if filter id matches*/    
  }/* RX FOR Loop*/

  /*------------------------------------------------------------------------
    Loop thru all filters and update the filter identifier bit mask for TX
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt<app_qos_spec->tx.fltr_template.num_filters; cnt++)
  {
    fltr = &(app_qos_spec->tx.fltr_template.list_ptr[cnt]);
    /*---------------------------------------------------------------------
      NW Assigned filter id is in the last 4 bits.
    ---------------------------------------------------------------------*/
    identifier = (fltr->ipfltr_aux_info.fi_id & (0xF));
    if (((1 << identifier) & filter_id) || (filter_id == 0))
    {
      if(clear_bit)
      {
        /*-------------------------------------------------------------------
          Clear the FI Identifier
        -------------------------------------------------------------------*/
        ds_3gpp_tft_clear_filter_identifier_bit(
                                 identifier,
                  &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->fi_identifier));
        /*-------------------------------------------------------------------
          Clear the FI Identifier
        -------------------------------------------------------------------*/
        ds_3gpp_tft_clear_filter_identifier_bit(
                                 identifier,
                     &(flow_context_p->ds_flow_context_dyn_p->fi_identifier));  
        /*-------------------------------------------------------------------
          Clear the FI precedence
        -------------------------------------------------------------------*/
        ds_3gpp_tft_clear_filter_preced_bit(
                                 fltr->ipfltr_aux_info.fi_precedence,
                          (flow_manager_p->ds_flow_manager_dyn_p->fi_preced));
      }/* if clear_bit*/
      else
      {
        /*-------------------------------------------------------------------
          Set the FI Identifier
        -------------------------------------------------------------------*/
        ds_3gpp_tft_set_filter_identifier_bit(
                                 identifier,
                &(bflow_manager_p->ds_bearer_flow_manager_dyn_p->fi_identifier));
        /*-------------------------------------------------------------------
          Set the FI Identifier
        -------------------------------------------------------------------*/
        ds_3gpp_tft_set_filter_identifier_bit(
                                 identifier,
                     &(flow_context_p->ds_flow_context_dyn_p->fi_identifier));
        /*-------------------------------------------------------------------
          Set the FI precedence
        -------------------------------------------------------------------*/
        ds_3gpp_tft_set_filter_preced_bit(
                                 fltr->ipfltr_aux_info.fi_precedence,
                         (flow_manager_p->ds_flow_manager_dyn_p->fi_preced));
      }/* else clear_bit*/
    }/* if filter id matches */
  }/* TX FOR Loop*/
  return TRUE;
}/* ds_flow_manager_update_filter_id_precedence*/

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_DELETE_FILTER_FROM_FLOW

DESCRIPTION
  Delete Filter ID from a flow .

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow context.
  nw_tft                -  TFT information
  nw_bk                 -  Network book keeping info.
  mode_info_p           -  Mode specific Qos.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_flow_manager_delete_filter_from_flow
(
  ds_pdn_context_s                      *pdn_context_p,
  ds_bearer_context_s                   *bearer_context_p,
  ds_flow_context_s                     *flow_context_p,
  tft_type_T                            *nw_tft,
  ds_3gpp_nw_modify_qos_bk_s            *nw_bk,
  const cm_call_mode_info_s_type        *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{
  ps_flow_modify_param_type   ps_flow_param;
  int16                       ds_errno = 0;
  ps_iface_type               *iface_ptr = NULL;
  int                         ret_val = 0;
  uint8        orig_rx_filter_cnt=0;
  uint8        orig_tx_filter_cnt=0;
  ps_extended_info_code_enum_type ps_eic = PS_EIC_NOT_SPECIFIED;
  ip_version_enum_type               ip_type; 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cause_code_p == NULL)
  {
    return FALSE;
  }
  else
  {
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
  }

  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    Configure the QOS flow spec
  -------------------------------------------------------------------------*/
  if(ds_3gpp_tft_configure_nw_bk_flow_spec(nw_bk)==FALSE)
  {
    DS_3GPP_MSG0_ERROR("unable to configure the nw book-keeping info");
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  }

  if(nw_bk->flow_cntx_fltr_id != 0)
  {
    /*-------------------------------------------------------------------
      Retrieve current filter info for flow context.
    ---------------------------------------------------------------------*/
    if(ds_flow_context_ps_flow_get_filters(
                                flow_context_p,
                                FALSE,
                               &(nw_bk->curr_qos)) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Could not retrieve previous filters");
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
      return FALSE; 
    }    

    /*-------------------------------------------------------------------
      Delete the filters.
    ---------------------------------------------------------------------*/
    if(ds_flow_context_delete_filter_from_flow(flow_context_p,
                                        nw_bk->flow_cntx_fltr_id,
                                        &(nw_bk->curr_qos),
                                        &(nw_bk->mod_qos))==FALSE)
    {
      DS_3GPP_MSG0_ERROR("error deleting filter from flow");
      return FALSE;
    }
    /*-------------------------------------------------------------------
      Update the filter pointers and number of filters.
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.list_ptr = nw_bk->mod_rx_fltr;
    nw_bk->curr_qos.tx.fltr_template.list_ptr = nw_bk->mod_tx_fltr;
    orig_rx_filter_cnt = nw_bk->curr_qos.rx.fltr_template.num_filters;
    orig_tx_filter_cnt = nw_bk->curr_qos.tx.fltr_template.num_filters;
    nw_bk->curr_qos.rx.fltr_template.num_filters = 
                                nw_bk->mod_qos.rx.fltr_template.num_filters;
    nw_bk->curr_qos.tx.fltr_template.num_filters = 
                                nw_bk->mod_qos.tx.fltr_template.num_filters;
    /*-------------------------------------------------------------------
      Update the TX Filter Mask.
    ---------------------------------------------------------------------*/
    if((nw_bk->curr_qos.tx.fltr_template.num_filters > 0) &&
       (orig_tx_filter_cnt > nw_bk->curr_qos.tx.fltr_template.num_filters))
    {
      nw_bk->curr_qos.field_mask |=
            (qos_spec_field_mask_type)QOS_MODIFY_MASK_TX_FLTR_MODIFY;  
    }
    /*-------------------------------------------------------------------
      Update the RX Filter Mask.
    ---------------------------------------------------------------------*/ 
    if((nw_bk->curr_qos.rx.fltr_template.num_filters > 0) &&
       (orig_rx_filter_cnt > nw_bk->curr_qos.rx.fltr_template.num_filters) )
    {
      nw_bk->curr_qos.field_mask |=
            (qos_spec_field_mask_type)QOS_MODIFY_MASK_RX_FLTR_MODIFY;  
    } 
    /*---------------------------------------------------------------------
      Convert NW Qos to App Qos
    ---------------------------------------------------------------------*/
    if(mode_info_p != NULL)
    {
      if(ds_bearer_flow_manager_convert_data_rate(bearer_context_p,
                                       mode_info_p,
                                       &nw_bk->curr_qos,
                                       cause_code_p) == DS3G_FAILURE)
      {
        return FALSE;
      }
      // Need to check data rate before configuring the field mask.
      if(nw_bk->curr_qos.rx.flow_template.req_flow.field_mask)                        
      {
        if (orig_rx_filter_cnt != 0)
        {
          nw_bk->curr_qos.field_mask |= ((qos_spec_field_mask_type)
                                            QOS_MASK_RX_FLOW);
        }
      }
      if(nw_bk->curr_qos.tx.flow_template.req_flow.field_mask)                        
      {
        if(orig_tx_filter_cnt != 0)
        {		
          nw_bk->curr_qos.field_mask |= ((qos_spec_field_mask_type)
                                            QOS_MASK_TX_FLOW);
        } 
      } 
    }/* mode_info_p != NULL */
    /*-----------------------------------------------------------------------
      If deletion of filter has made num_filters 0 in some direction. 
      We need to Update IP_FLOW_MASK_NONE so that frameowrk deletes all filters.
    -----------------------------------------------------------------------*/
    if( (orig_rx_filter_cnt > 0) &&
        (nw_bk->curr_qos.rx.fltr_template.num_filters == 0))
    {
      DS_3GPP_MSG1_HIGH("Deleted all (%d) filters in RX direction",orig_rx_filter_cnt);
      nw_bk->curr_qos.field_mask |= QOS_MASK_RX_FLOW;
      nw_bk->curr_qos.rx.flow_template.req_flow.field_mask = (ipflow_field_mask_type)IPFLOW_MASK_NONE;
    }
    if( (orig_tx_filter_cnt > 0) &&
        (nw_bk->curr_qos.tx.fltr_template.num_filters == 0))
    {
      DS_3GPP_MSG1_HIGH("Deleted all (%d) filters in TX direction",orig_tx_filter_cnt);
      nw_bk->curr_qos.field_mask |= QOS_MASK_TX_FLOW;
      nw_bk->curr_qos.tx.flow_template.req_flow.field_mask = (ipflow_field_mask_type)IPFLOW_MASK_NONE;
    }
    /*-----------------------------------------------------------------------
      Update the new Flow with the filter information.
    -----------------------------------------------------------------------*/                               
    memset(&ps_flow_param, 0, sizeof(ps_flow_modify_param_type));
    ps_flow_param.qos_spec            = &(nw_bk->curr_qos);
    ps_flow_param.flow_validate_f_ptr = NULL;
    ps_flow_param.fltr_validate_f_ptr = NULL;
    ps_flow_param.fltr_priority       = PS_IFACE_IPFLTR_PRIORITY_FCFS;
    ps_flow_param.subset_id           = 5; 

    /*-----------------------------------------------------------------------
      Find the right iface pointer.
    -----------------------------------------------------------------------*/ 
    if(ds_flow_context_retrieve_iface_ip_type(flow_context_p,
                                            &ip_type,
                                            &iface_ptr) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("3GPP iface is not present");
      return FALSE;
    }
    /*-------------------------------------------------------------------
      Disable Filtering on the Flow.
    ---------------------------------------------------------------------*/
    if(ds_flow_context_ps_flow_filtering_control(flow_context_p, FALSE)!= TRUE)
    {
      DS_3GPP_MSG0_ERROR("flow filtering control failed");
      //return FALSE;
    }

    /*-----------------------------------------------------------------------
      Invoke the Flow Modify.
    -----------------------------------------------------------------------*/
    ret_val = ps_iface_modify_flow(iface_ptr,(flow_context_p->
                                   ds_flow_context_dyn_p->ps_flow_ptr),
                                   &ps_flow_param, &ds_errno);
    if(ret_val == -1)
    {
      DS_3GPP_MSG0_ERROR("modify Flow failed");
      /*-------------------------------------------------------------------
        Enable Filtering on the Flow.
      ---------------------------------------------------------------------*/
      if(ds_flow_context_ps_flow_filtering_control(flow_context_p, TRUE) != TRUE)
      {
        DS_3GPP_MSG0_ERROR("flow filtering control failed");
        //return FALSE;
      }
      return FALSE;
    }

    if((flow_context_p->ds_flow_context_dyn_p->flow_type & DS_3GPP_NW_FLOW_TYPE)
        == DS_3GPP_NW_FLOW_TYPE)
    {
      ds_flow_context_set_flow_state(flow_context_p,
                                       DS_FLOW_CONTEXT_STATE_PENDING_LL_CONFIRM);
    }
    else
    {
      /*-------------------------------------------------------------------
        Modify Flow Accepted.
      ---------------------------------------------------------------------*/
      if(ds_flow_context_ps_flow_modify_result(
                                        flow_context_p,TRUE,ps_eic)!=TRUE)
      {
        DS_3GPP_MSG0_ERROR("flow modify result failed");
        return FALSE;
      }
      /*-------------------------------------------------------------------
        Enable Filtering on the Flow.
      ---------------------------------------------------------------------*/
      if(ds_flow_context_ps_flow_filtering_control(flow_context_p, TRUE) != TRUE)
      {
        DS_3GPP_MSG0_ERROR("flow filtering control failed");
       // return FALSE;
      }
    }

    /*-------------------------------------------------------------------
      Update the filter pointers and filter count to the original cnt
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.list_ptr  = nw_bk->curr_rx_fltr;
    nw_bk->curr_qos.tx.fltr_template.list_ptr  = nw_bk->curr_tx_fltr;
    /*-------------------------------------------------------------------
      Retrieve the filter count before modification
    ---------------------------------------------------------------------*/
    nw_bk->curr_qos.rx.fltr_template.num_filters = orig_rx_filter_cnt;
    nw_bk->curr_qos.tx.fltr_template.num_filters = orig_tx_filter_cnt;
  
    /*-------------------------------------------------------------------
      Update Precedence and Filter ID information.
    ---------------------------------------------------------------------*/
    if(ds_flow_manager_update_filter_id_precedence(
                                              pdn_context_p,
                                              bearer_context_p,
                                              flow_context_p,
                                              nw_bk->flow_cntx_fltr_id,
                                              &(nw_bk->curr_qos),
                                              TRUE) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error updating filter Id and precedence");
      return FALSE;
    }
  }/* nw_bk->flow_cntx_fltr_id != 0*/ 
  return TRUE;
} /* ds_flow_manager_delete_filter_from_flow */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_REMOVE_FILTER_FROM_TFT_HELPER

DESCRIPTION
  remove filters to existing TFT.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow information.
  nw_tft                -  TFT information.
  nw_bk                 -  Network book keeping info.
  mode_info_p           -  Mode specific Qos.
  cause_code_p          -  Cause code NW TFT is not accurate

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_flow_manager_remove_filter_from_tft_helper
(
  ds_pdn_context_s                      *pdn_context_p,
  ds_bearer_context_s                   *bearer_context_p,
  ds_flow_context_s                     *flow_context_p,
  tft_type_T                            *nw_tft,
  ds_3gpp_nw_modify_qos_bk_s            *nw_bk,
  const cm_call_mode_info_s_type        *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((pdn_context_p==NULL)||(nw_tft == NULL)
     ||(cause_code_p== NULL)||(nw_bk== NULL)||(flow_context_p == NULL)
     ||(!ds_flow_context_validate_flow(flow_context_p)))
  {

    DS_3GPP_MSG3_ERROR("PDN:0x%x:Bearer:0x%x:nw_bk:0x%x",pdn_context_p,
                       bearer_context_p,nw_bk);

    DS_3GPP_MSG2_ERROR("nw_tft:0x%x cause_code_p:0x%x",nw_tft,
                        cause_code_p);

    if(cause_code_p != NULL)
    {
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    }

    return FALSE;
  }

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------
    Check if the network fltr has any fltr ID or fltr preced conflicts.
  -------------------------------------------------------------------*/
  if(ds_flow_manager_detect_conflict_in_tft(pdn_context_p,
                                         bearer_context_p,
                                         flow_context_p,
                                         nw_tft,
                                         nw_bk,
                                         cause_code_p,
                                         TRUE) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Unresolved Conflict in TFT");
    return FALSE;
  }   
  /*-----------------------------------------------------------------------
    Check for Additional TFT Semantic Errors
  -----------------------------------------------------------------------*/
  bflow_manager_p = 
               ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    if((nw_bk->non_match_fltr_id == 0)&&
       ((bflow_manager_p->ds_bearer_flow_manager_dyn_p->fi_identifier & 
      (nw_bk->bearer_fltr_id|nw_bk->flow_cntx_fltr_id))
                 == (bflow_manager_p->ds_bearer_flow_manager_dyn_p->fi_identifier)))
    {
      DS_3GPP_MSG0_HIGH("Network deleting all filters,tft is empty");
      
      if(bearer_context_p->ds_bearer_context_dyn_p->is_default)
      {
        //In this case this is equivalent to a Delete TFT operation.
        return ds_flow_manager_process_delete_tft(pdn_context_p,
                                                  bearer_context_p,
                                                  nw_tft,
                                                  mode_info_p,
                                                  cause_code_p);
      }
      else
      {
        *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_TFT;
        return FALSE;     
      }      	
    }
    /*-----------------------------------------------------------------------
     5) TFT operation = "Delete packet filters from existing TFT" with a packet 
     filter list also including packet filters in addition to the packet filter identifiers.
    -----------------------------------------------------------------------*/    
    else if(bearer_context_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_LTE &&
            nw_tft->tft_total_length != nw_tft->num_filters + 1)
    {
      DS_3GPP_MSG0_ERROR("Network TFT IE has packet filters, in addition to "
   	                     "filter identifiers. Syntactical  error");
      *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
      return FALSE;
    }	
    else if((nw_bk->non_match_fltr_id > 0)&&
            ((nw_bk->bearer_fltr_id|nw_bk->flow_cntx_fltr_id) > 0))      	 
    {
      DS_3GPP_MSG0_HIGH("Network TFT IE has filter other than those installed"
                        " on this flow. Ignore");            
    }
  }
  else
  {
    DS_3GPP_MSG0_ERROR("could not retrieve bearer flow manager");
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  }
  /*-------------------------------------------------------------------
    Check if NW is deleting non-existing filters.
  -------------------------------------------------------------------*/
  if((nw_bk->bearer_fltr_id == 0) && (nw_bk->flow_cntx_fltr_id == 0))
  {
    DS_3GPP_MSG0_ERROR("Network trying to delete non-existing filters");
    return TRUE;
  }
  
  /*-------------------------------------------------------------------
    Check if NW is deleting UE requested filters.
  -------------------------------------------------------------------*/
  if(nw_bk->bearer_fltr_id != 0)
  {
    DS_3GPP_MSG0_ERROR("Network trying to delete filters set up by UE");
    //*cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_TFT;
    //return FALSE;
  }
  /*-------------------------------------------------------------------
    Delete the filters.
  ---------------------------------------------------------------------*/
  if(ds_flow_manager_delete_filter_from_flow(
                                          pdn_context_p,
                                          bearer_context_p,
                                          flow_context_p,
                                          nw_tft, 
                                          nw_bk,
                                          mode_info_p,
                                          cause_code_p)== FALSE)
  {
    DS_3GPP_MSG0_ERROR("Could not delete filter from Flow");
    return FALSE; 
  }
  /*-------------------------------------------------------------------
     Resolve any filter conflict.
  ---------------------------------------------------------------------*/
  if(ds_flow_manager_handle_fltr_conflict(pdn_context_p,
                                         bearer_context_p,
                                         flow_context_p,
                                         nw_bk) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("unable to handle filter conflict");
    return FALSE;
  }

  return TRUE;
}/* ds_flow_manager_remove_filter_from_tft_helper */
/*===========================================================================
FUNCTION DS_FLOW_MANAGER_REMOVE_FILTER_FROM_TFT

DESCRIPTION
  remove filters to existing TFT.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow information.
  nw_tft                -  TFT information.
  mode_info_p           -  Mode specific Qos.
  cause_code_p          -  Cause code NW TFT is not accurate

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_remove_filter_from_tft
(
  ds_pdn_context_s                      *pdn_context_p,
  ds_bearer_context_s                   *bearer_context_p,
  ds_flow_context_s                     *flow_context_p,
  tft_type_T                            *nw_tft,
  const cm_call_mode_info_s_type        *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{
  ds_flow_context_s            *flow_cntxv4 = NULL;
  ds_flow_context_s            *flow_cntxv6 = NULL; 
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;
  ds_3gpp_nw_modify_qos_bk_s     *nw_bk;
  ps_extended_info_code_enum_type ps_eic = PS_EIC_NOT_SPECIFIED;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((pdn_context_p==NULL)||(nw_tft == NULL)
     ||(cause_code_p== NULL))
  {
    DS_3GPP_MSG0_ERROR("Input params are NULL in removeFilterFromTft");
    if(cause_code_p != NULL)
    {
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    }
    return FALSE;
  }

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }

  /*-----------------------------------------------------------------------
    Check if this is NW initiated Qos Mod, or NW response to UE request
  -----------------------------------------------------------------------*/
  if(flow_context_p != NULL)
  {
    DS_3GPP_MSG1_HIGH("Remove filter from TFT for flow:0x%x",flow_context_p);
  }
  else
  {
    DS_3GPP_MSG3_HIGH("Remove filter from TFT for NW flow",0,0,0);
  }

  /*-----------------------------------------------------------------------
    Check for TFT Semantic Errors
  -----------------------------------------------------------------------*/
  if(ds_3gpp_tft_check_mod_fltr_semantic_error(nw_tft) == TRUE)
  {
    *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
    return FALSE;
  } 
   /*-----------------------------------------------------------------------
     Allocate Heap Memory
  -----------------------------------------------------------------------*/
  nw_bk = (ds_3gpp_nw_modify_qos_bk_s *)modem_mem_alloc(
                                       sizeof(ds_3gpp_nw_modify_qos_bk_s),
                                       MODEM_MEM_CLIENT_DATA);
  if(nw_bk == NULL)
  {
    DS_3GPP_MSG2_ERROR("Heap Memory Allocation failed for client: %d size: %d", 
       MODEM_MEM_CLIENT_DATA,(sizeof(ds_3gpp_nw_modify_qos_bk_s)));
    *cause_code_p =  DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  }
  memset(nw_bk, 0,sizeof(ds_3gpp_nw_modify_qos_bk_s));

  if(flow_context_p == NULL)
  {
    switch(ds_pdn_cntxt_get_pdn_context_ip_support(pdn_context_p))
    {
      case DS_UMTS_PDP_IPV4:
        flow_cntxv4 = ds_bearer_flow_manager_find_nw_flow_context(
                                 bearer_context_p,DS_3GPP_V4_FLOW_TYPE);
        if(flow_cntxv4 == NULL)
        {
          DS_3GPP_MSG0_HIGH("No previous NW flow context associated flow manager");
          *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return FALSE;
        }     
        /*-------------------------------------------------------------------
          Check if the network fltr has any fltr ID or fltr preced conflicts.
        -------------------------------------------------------------------*/
        if(ds_flow_manager_remove_filter_from_tft_helper(pdn_context_p,
                                                         bearer_context_p,
                                                         flow_cntxv4,
                                                         nw_tft,
                                                         nw_bk,
                                                         mode_info_p,
                                                     cause_code_p) == FALSE)
        {
          DS_3GPP_MSG0_ERROR("Error in remove filter helper ");
          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return FALSE;
        }  
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return TRUE;

      case DS_UMTS_PDP_IPV6:
        flow_cntxv6 = ds_bearer_flow_manager_find_nw_flow_context(
                                bearer_context_p,DS_3GPP_V6_FLOW_TYPE);
        if(flow_cntxv6 == NULL)
        {
          DS_3GPP_MSG0_ERROR("No previous NW flow context associated flow manager");
          *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return FALSE;
        }
        /*-------------------------------------------------------------------
          Check if the network fltr has any fltr ID or fltr preced conflicts.
        -------------------------------------------------------------------*/
        if(ds_flow_manager_remove_filter_from_tft_helper(pdn_context_p,
                                                         bearer_context_p,
                                                         flow_cntxv6,
                                                         nw_tft,
                                                         nw_bk,
                                                         mode_info_p,
                                                     cause_code_p) == FALSE)
        {
          DS_3GPP_MSG0_ERROR("Error in remove filter helper ");
          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return FALSE;
        } 
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA); 
      return TRUE;

      case DS_UMTS_PDP_IPV4V6:
      /*-------------------------------------------------------------------
        IP TYPE is V4
      ---------------------------------------------------------------------*/
      flow_cntxv4 = ds_bearer_flow_manager_find_nw_flow_context(
                                     bearer_context_p,DS_3GPP_V4_FLOW_TYPE);
      if(flow_cntxv4 == NULL)
      {
        DS_3GPP_MSG0_ERROR("No previous NW flow context associated flow manager");
        *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return FALSE;
      }
      /*-------------------------------------------------------------------
        IP TYPE is V6
      ---------------------------------------------------------------------*/
      flow_cntxv6 = ds_bearer_flow_manager_find_nw_flow_context(
                                      bearer_context_p,DS_3GPP_V6_FLOW_TYPE);
      if(flow_cntxv6 == NULL)
      {
        DS_3GPP_MSG0_ERROR("No previous NW flow context associated flow manager");
        *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return FALSE;
      }
      /*-------------------------------------------------------------------
        Check if the network fltr has any fltr ID or fltr preced conflicts.
      -------------------------------------------------------------------*/
      if(ds_flow_manager_detect_conflict_in_tft(pdn_context_p,
                                         bearer_context_p,
                                         flow_cntxv4,
                                         nw_tft,
                                         nw_bk,
                                         cause_code_p,
                                         TRUE) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("Unresolved Conflict in TFT");
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return FALSE;
      }   
      /*---------------------------------------------------------------------
        Check for Additional TFT Semantic Errors
      ---------------------------------------------------------------------*/
      bflow_manager_p = 
              ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
      if(ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
      {
        if((bflow_manager_p->ds_bearer_flow_manager_dyn_p->fi_identifier & 
            (nw_bk->bearer_fltr_id|nw_bk->flow_cntx_fltr_id))
                     == (bflow_manager_p->ds_bearer_flow_manager_dyn_p->fi_identifier))
        {
          DS_3GPP_MSG0_ERROR("Network deleting all filters,tft is empty");
          if(bearer_context_p->ds_bearer_context_dyn_p->is_default)
          {
            //Deleting all the installed filters is allowed only on the default bearer.
            //In this case this is equivalent to a Delete TFT operation.
            modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
            return ds_flow_manager_process_delete_tft(pdn_context_p,
                                                      bearer_context_p,
                                                      nw_tft,
                                                      mode_info_p,
                                                      cause_code_p);
          }
          else
          {
            *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_TFT;
            modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
            return FALSE;
          }
        }
      /*-----------------------------------------------------------------------
        5) TFT operation = "Delete packet filters from existing TFT" with a 
        packet filter list also including packet filters in addition to the
        packet filter identifiers.
     -----------------------------------------------------------------------*/          
        else if(bearer_context_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_LTE &&
       	        nw_tft->tft_total_length != nw_tft->num_filters + 1)
        {
          DS_3GPP_MSG0_ERROR("Network TFT IE has packet filters, in addition to "
   	                         "filter identifiers. Syntactical  error");
          *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return FALSE;
        }
        else if((nw_bk->non_match_fltr_id > 0)&&
                ((nw_bk->bearer_fltr_id|nw_bk->flow_cntx_fltr_id) > 0))      	 
        {
          DS_3GPP_MSG0_HIGH("Network TFT IE has filter other than those installed"
                            " on this flow. Ignore");            
        }
      }
      else
      {
        DS_3GPP_MSG0_ERROR("could not retrieve bearer flow manager");
        *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return FALSE;
      }
      /*------------------------------------------------------------------
         Check if NW is deleting non-existing filters.
       -------------------------------------------------------------------*/
      if((nw_bk->bearer_fltr_id == 0) && (nw_bk->flow_cntx_fltr_id == 0))
      {
        DS_3GPP_MSG0_ERROR("Network trying to delete non-existing filters");
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return TRUE;
      }
      /*-------------------------------------------------------------------
        Check if NW is deleting UE requested filters.
      -------------------------------------------------------------------*/
      if(nw_bk->bearer_fltr_id != 0)
      {
        DS_3GPP_MSG0_ERROR("Network trying to delete filters set up by UE");
        //*cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_TFT;
        //modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        //return FALSE;
        // Allow NW to delete these filters and delete the affected
        
      }
       /*-------------------------------------------------------------------
        Delete the V4 filters.
      ---------------------------------------------------------------------*/
      if(ds_flow_manager_delete_filter_from_flow(
                                          pdn_context_p,
                                          bearer_context_p,
                                          flow_cntxv4,
                                          nw_tft, 
                                          nw_bk,
                                          mode_info_p,
                                          cause_code_p)== FALSE)
      {
        DS_3GPP_MSG0_ERROR("Could not delete filter from Flow");
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return FALSE; 
      }
      /*-------------------------------------------------------------------
       Delete the V6 filters.
      ---------------------------------------------------------------------*/
      if(ds_flow_manager_delete_filter_from_flow(
                                          pdn_context_p,
                                          bearer_context_p,
                                          flow_cntxv6,
                                          nw_tft, 
                                          nw_bk,
                                          mode_info_p,
                                          cause_code_p)== FALSE)
      {
        DS_3GPP_MSG0_ERROR("Could not delete filter from Flow");
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return FALSE; 
      }
      /*-------------------------------------------------------------------
        Resolve any filter conflict.
      ---------------------------------------------------------------------*/
      if(ds_flow_manager_handle_fltr_conflict(pdn_context_p,
                                         bearer_context_p,
                                         flow_cntxv4,
                                         nw_bk) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("unable to handle filter conflict");
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return FALSE;
      }
    break;

    default:
      DS_3GPP_MSG0_ERROR("Unknow IP Type");
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return FALSE; 
    }/* switch*/ 

  }/* if flow_context_p == NULL*/
  else
  {
    /*-------------------------------------------------------------------
     Handle NW response to UE request.
     Delete can have additional NW filters. For now this is not being
     handled by code.
    ---------------------------------------------------------------------*/
    /*-------------------------------------------------------------------
      Verify if requested Filters are present in the NW response.
    -------------------------------------------------------------------*/
    if(ds_3gpp_tft_verify_nw_tft(&(flow_context_p->ds_flow_context_dyn_p
                                ->tft_mod),nw_tft, flow_context_p) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Nw response does not match UE request");
      *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return FALSE; 
    }
    nw_bk->is_add = FALSE;
    /*-------------------------------------------------------------------
      Check if the network fltr has any fltr ID or fltr preced conflicts.
    -------------------------------------------------------------------*/
    if(ds_flow_manager_detect_conflict_in_tft(pdn_context_p,
                                         bearer_context_p,
                                         flow_context_p,
                                         nw_tft,
                                         nw_bk,
                                         cause_code_p,
                                         TRUE) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Unresolved Conflict in TFT");
      *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_TFT;
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return FALSE;
    }

    if((flow_context_p->ds_flow_context_dyn_p->state!=DS_FLOW_CONTEXT_STATE_PEND_DEL_RSP)
     &&(flow_context_p->ds_flow_context_dyn_p->state!=DS_FLOW_CONTEXT_STATE_PEND_DEL_FLOW_RSP))
    {
      DS_3GPP_MSG1_ERROR("Unsupported flow context mode:0x%x",
              flow_context_p->ds_flow_context_dyn_p->state);
      *cause_code_p = DS_3GPP_INTERNAL_SERV_OPT_NOT_SUPPORTED;
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return FALSE;
    }

    /*-------------------------------------------------------------------------
      Configure the QOS flow spec
    -------------------------------------------------------------------------*/
    if(ds_3gpp_tft_configure_nw_bk_flow_spec(nw_bk)==FALSE)
    {
      DS_3GPP_MSG0_ERROR("unable to configure the nw book-keeping info");
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return FALSE;
    }
    /*-------------------------------------------------------------------
      Retrieve current filter info for flow context.
    ---------------------------------------------------------------------*/
    if(ds_flow_context_ps_flow_get_filters(
                                flow_context_p,
                                FALSE,
                               &nw_bk->curr_qos) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Could not retrieve previous filters");
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return FALSE; 
    }

    if(flow_context_p->ds_flow_context_dyn_p->state == 
       DS_FLOW_CONTEXT_STATE_PEND_DEL_RSP)
    {      
      /*-----------------------------------------------------------------
        Confirm flow modification to PS framework.
      -----------------------------------------------------------------*/
      ps_eic = PS_EIC_NOT_SPECIFIED;
      if (ds_flow_context_ps_flow_modify_result(
                     flow_context_p,TRUE,ps_eic) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("Flow Modify accepted returned error:");
        *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return FALSE;
      } 
      /*-------------------------------------------------------------------
        Enable filtering on the flow.
      -------------------------------------------------------------------*/
      if(ds_flow_context_ps_flow_filtering_control(flow_context_p,TRUE)!= TRUE)
      {
        DS_3GPP_MSG0_ERROR("error enabling filters on the flow");
      }
    }/*DS_FLOW_CONTEXT_STATE_PEND_DEL_RSP */
    /*-------------------------------------------------------------------
      clear Precedence and Filter ID information
    ---------------------------------------------------------------------*/
    if(ds_flow_manager_update_filter_id_precedence(
                                              pdn_context_p,
                                              bearer_context_p,
                                              flow_context_p,
                                              nw_bk->flow_cntx_fltr_id,
                                              &(nw_bk->curr_qos),
                                              TRUE) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error updating filter Id and precedence"); 
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return FALSE;
    }   
    /*-------------------------------------------------------------------
      Move the flow context from PDN context to bearer.
     ---------------------------------------------------------------------*/
    if(ds_flow_manager_remove_pending_flow_context(
                       pdn_context_p,flow_context_p) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error in removing the pending context");
    }
    if(flow_context_p->ds_flow_context_dyn_p->state == DS_FLOW_CONTEXT_STATE_PEND_DEL_RSP)
    {
      if(ds_bearer_flow_manager_add_flow_context_to_flow_manager(
                  bearer_context_p,flow_context_p) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("Error in Adding Flow Context to Bearer Flow manager");
      }
      /*-------------------------------------------------------------------
        Change the flow context state.
      -------------------------------------------------------------------*/
      ds_flow_context_set_flow_state(flow_context_p,
                DS_FLOW_CONTEXT_STATE_UP);
    }
    else
    {
      ds_flow_context_ps_flow_go_null_ind(flow_context_p,ps_eic);
      ds_flow_context_release_flow_context(flow_context_p);
    }
    modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
    return TRUE;
  }/* flow_context_p != NULL */

  modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
  return TRUE;
}/* ds_flow_manager_remove_filter_from_tft */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_REPLACE_FILTER_IN_TFT

DESCRIPTION
  replace filters to existing TFT.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow context to replace filters.
  nw_tft_p              -  TFT information.
  mode_info_p           -  Mode specific Qos.
  cause_code_p          -  Cause code.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_replace_filter_in_tft
(
  ds_pdn_context_s                      *pdn_context_p,
  ds_bearer_context_s                   *bearer_context_p,
  ds_flow_context_s                     *flow_context_p,
  tft_type_T                            *nw_tft,
  const cm_call_mode_info_s_type        *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{
  ds_flow_context_s            *flow_cntxv4 = NULL;
  ds_flow_context_s            *flow_cntxv6 = NULL;
  ds_3gpp_nw_modify_qos_bk_s   *nw_bk; 
  ps_extended_info_code_enum_type ps_eic = PS_EIC_NOT_SPECIFIED;
  boolean                      return_value = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((pdn_context_p==NULL)||(bearer_context_p==NULL)||(nw_tft == NULL)
     ||(cause_code_p== NULL))
  {
    DS_3GPP_MSG0_ERROR("Input params are NULL in replaceFilterFromTft");
    if(cause_code_p != NULL)
    {
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    }
    return FALSE;
  }

    /*-----------------------------------------------------------------------
    Check if this is NW initiated Qos Mod, or NW response to UE request
  -----------------------------------------------------------------------*/
  if(flow_context_p != NULL)
  {
    DS_3GPP_MSG1_HIGH("Replace filter to TFT for flow:0x%x",
                                                 flow_context_p);
  }
  else
  {
    DS_3GPP_MSG0_HIGH("Replace filter to TFT for NW flow");
  }
  /*-----------------------------------------------------------------------
     Check for TFT Syntactic Errors
  -----------------------------------------------------------------------*/
  if(ds_3gpp_tft_check_mod_fltr_semantic_error(nw_tft) == TRUE)
  {
    *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
    return FALSE;
  }
  else if(ds_3gpp_tft_check_for_repeated_precedence(nw_tft) == TRUE)
  {
    *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
    return FALSE;
  }
  /*-----------------------------------------------------------------------
     Allocate Heap Memory
  -----------------------------------------------------------------------*/
  nw_bk = (ds_3gpp_nw_modify_qos_bk_s *)modem_mem_alloc(
                                       sizeof(ds_3gpp_nw_modify_qos_bk_s),
                                       MODEM_MEM_CLIENT_DATA);
  if(nw_bk == NULL)
  {
    DS_3GPP_MSG2_ERROR("Heap Memory Allocation failed for client: %d size: %d", 
       MODEM_MEM_CLIENT_DATA,(sizeof(ds_3gpp_nw_modify_qos_bk_s)));
    *cause_code_p =  DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  }
  memset(nw_bk, 0,sizeof(ds_3gpp_nw_modify_qos_bk_s));

  if(flow_context_p == NULL)
  {
    /*-----------------------------------------------------------------------
      Handle NW initiated Modification
    -----------------------------------------------------------------------*/
    switch(ds_pdn_cntxt_get_pdn_context_ip_support(pdn_context_p))
    {
      case DS_UMTS_PDP_IPV4:
        flow_cntxv4 = ds_bearer_flow_manager_find_nw_flow_context(
                                 bearer_context_p,DS_3GPP_V4_FLOW_TYPE);
        if(flow_cntxv4 == NULL)
        {
          DS_3GPP_MSG0_ERROR("No previous NW flow context associated flow manager");

          *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
          return_value = ds_flow_manager_process_create_tft(
              pdn_context_p,
              bearer_context_p,
              nw_tft,
              mode_info_p,
              cause_code_p
          ); 
               
          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return return_value;
        }
        nw_bk->is_add = FALSE;
        /*-------------------------------------------------------------------
          Invoke the replace filter tft helper.
        -------------------------------------------------------------------*/
        return_value = ds_flow_manager_add_replace_filter_to_tft_helper(
                                                    pdn_context_p,
                                                    bearer_context_p,
                                                    flow_cntxv4,
                                                    nw_tft,
                                                    nw_bk,
                                                    mode_info_p,
                                                    cause_code_p);
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return (return_value);
  
      case DS_UMTS_PDP_IPV6:
        flow_cntxv6 = ds_bearer_flow_manager_find_nw_flow_context(
                                bearer_context_p,DS_3GPP_V6_FLOW_TYPE);
        if(flow_cntxv6 == NULL)
        {
          DS_3GPP_MSG0_ERROR("No previous NW flow context associated flow manager");
          *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;

          return_value = ds_flow_manager_process_create_tft(
              pdn_context_p,
              bearer_context_p,
              nw_tft,
              mode_info_p,
              cause_code_p
          ); 

          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return return_value;
        }
        nw_bk->is_add = FALSE;
        /*-------------------------------------------------------------------
          Invoke the replace filter tft helper.
        -------------------------------------------------------------------*/
        return_value = ds_flow_manager_add_replace_filter_to_tft_helper(
                                                    pdn_context_p,
                                                    bearer_context_p,
                                                    flow_cntxv6,
                                                    nw_tft,
                                                    nw_bk,
                                                    mode_info_p,
                                                    cause_code_p);
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return (return_value);
      case DS_UMTS_PDP_IPV4V6:
        /*-------------------------------------------------------------------
          IP TYPE is V4
        ---------------------------------------------------------------------*/
        flow_cntxv4 = ds_bearer_flow_manager_find_nw_flow_context(
                                      bearer_context_p,DS_3GPP_V4_FLOW_TYPE);
        if(flow_cntxv4 == NULL)
        {
          DS_3GPP_MSG0_ERROR("No previous NW flow context associated flow manager");
          *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;

          return_value = ds_flow_manager_process_create_tft(
              pdn_context_p,
              bearer_context_p,
              nw_tft,
              mode_info_p,
              cause_code_p
          ); 

          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return return_value;
        }
        /*-------------------------------------------------------------------
          IP TYPE is V6
        ---------------------------------------------------------------------*/
        flow_cntxv6 = ds_bearer_flow_manager_find_nw_flow_context(
                                      bearer_context_p,DS_3GPP_V6_FLOW_TYPE);
        if(flow_cntxv6 == NULL)
        {
          DS_3GPP_MSG0_ERROR("No previous NW flow context associated flow manager");
          *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;

          return_value = ds_flow_manager_process_create_tft(
              pdn_context_p,
              bearer_context_p,
              nw_tft,
              mode_info_p,
              cause_code_p
          ); 

          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return return_value;
        }
        nw_bk->is_add = FALSE;
        /*-------------------------------------------------------------------
          Check if the network fltr has any fltr ID or fltr preced conflicts.
        -------------------------------------------------------------------*/
        if(ds_flow_manager_detect_conflict_in_tft(pdn_context_p,
                                         bearer_context_p,
                                         flow_cntxv4,
                                         nw_tft,
                                         nw_bk,
                                         cause_code_p,
                                         FALSE) == FALSE)
        {
          DS_3GPP_MSG0_ERROR("Unresolved Conflict in TFT");
          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return FALSE;
        }
        /*-------------------------------------------------------------------
          Replace filters for V4.
        -------------------------------------------------------------------*/
        if(ds_flow_manager_replace_filter_in_flow(
                                  pdn_context_p,
                                  bearer_context_p,
                                  flow_cntxv4,
                                  nw_tft,
                                  nw_bk,
                                  mode_info_p,
                                  cause_code_p)!= TRUE)
        { 
          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return FALSE;
        }
        /*-------------------------------------------------------------------
          Replace filters for V6.
        -------------------------------------------------------------------*/
        if(ds_flow_manager_replace_filter_in_flow(
                                  pdn_context_p,
                                  bearer_context_p,
                                  flow_cntxv6,
                                  nw_tft,
                                  nw_bk,
                                  mode_info_p,
                                  cause_code_p)!= TRUE)
        { 
          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return FALSE;
        }
        /*-------------------------------------------------------------------
          Resolve any filter conflict.
        ---------------------------------------------------------------------*/
        if(ds_flow_manager_handle_fltr_conflict(pdn_context_p,
                                         bearer_context_p,
                                         flow_cntxv6,
                                         nw_bk) == FALSE)
        {
          DS_3GPP_MSG0_ERROR("unable to handle filter conflict");
          modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
          return FALSE;
        }
      break;
      default:
        DS_3GPP_MSG0_ERROR("Unknow IP Type");
        *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
        modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
        return FALSE; 
    }/* switch*/ 
  }/* if flow_context_p == NULL*/
  else
  {
    if(!ds_flow_context_validate_flow(flow_context_p))
    {
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return FALSE; 
    }
    /*-------------------------------------------------------------------
      Handle NW response to UE request.
    ---------------------------------------------------------------------*/
    /*-------------------------------------------------------------------
      Verify if requested Filters are present in the NW response.
    -------------------------------------------------------------------*/
    if(ds_3gpp_tft_verify_nw_tft(&(flow_context_p->ds_flow_context_dyn_p->
                                   tft_mod),nw_tft, flow_context_p) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Nw response does not match UE request");
      *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return FALSE; 
    }
    nw_bk->is_add = FALSE;
    /*-------------------------------------------------------------------
      Check if the network fltr has any fltr ID or fltr preced conflicts.
    -------------------------------------------------------------------*/
    if(ds_flow_manager_detect_conflict_in_tft(pdn_context_p,
                                         bearer_context_p,
                                         flow_context_p,
                                         nw_tft,
                                         nw_bk,
                                         cause_code_p,
                                         FALSE) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Unresolved Conflict in TFT");
      *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_TFT;
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return FALSE;
    }
    /*--------------------------------------------------------------------
      In case of replace, if the NW response does not match the UE request
      it will be rejected.
     ---------------------------------------------------------------------*/
    if(flow_context_p->ds_flow_context_dyn_p->state != 
       DS_FLOW_CONTEXT_STATE_PEND_REPLACE_RSP)
    {
      DS_3GPP_MSG1_ERROR("Unsupported flow context mode:0x%x",
              flow_context_p->ds_flow_context_dyn_p->state);
      *cause_code_p = DS_3GPP_INTERNAL_SERV_OPT_NOT_SUPPORTED;
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return FALSE;
    }
 
    /*-------------------------------------------------------------------
      Confirm flow modification to PS framework.
    -------------------------------------------------------------------*/
    ps_eic = PS_EIC_NOT_SPECIFIED;
    if (ds_flow_context_ps_flow_modify_result(
                     flow_context_p,TRUE,ps_eic) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Flow Modify accepted returned error");
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return FALSE;
    }
    /*-------------------------------------------------------------------
      Enable filtering on the flow.
    -------------------------------------------------------------------*/
    if(ds_flow_context_ps_flow_filtering_control(flow_context_p,TRUE)!= TRUE)
    {
      DS_3GPP_MSG1_ERROR("enabling filters failed on flow: 0x%x",
                           flow_context_p);
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

    /*-------------------------------------------------------------------
      Resolve any filter conflict.
    ---------------------------------------------------------------------*/
    if(ds_flow_manager_handle_fltr_conflict(pdn_context_p,
                                         bearer_context_p,
                                         flow_context_p,
                                         nw_bk) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("unable to handle filter conflict");
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return FALSE;
    }
    modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
    /*-------------------------------------------------------------------
      Update the flow context state.
    ---------------------------------------------------------------------*/
    ds_flow_context_set_flow_state(flow_context_p,
                DS_FLOW_CONTEXT_STATE_UP);
    return TRUE;
  }/* else flow_context_p != NULL */
  modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
  return TRUE;
} /* ds_flow_manager_replace_filter_in_tft */


/*===========================================================================
FUNCTION DSFLOWMANAGER_UPDATEDATARATEINFLOW

DESCRIPTION
  Update the data rate in the flow.

PARAMETERS
  bearer_context_p      -  Bearer information
  mode_info_p           -  Mode information

DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_update_data_rate_in_flow
(
 ds_bearer_context_s                   *bearer_context_p,
  const cm_call_mode_info_s_type        *mode_info_p,
 ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{
  boolean                           return_value = FALSE;
  qos_spec_type                     app_qos;
  ds_bearer_flow_manager_s          *bflow_manager_p = NULL;
  ds_flow_context_s                 *flow_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (bearer_context_p==NULL)||(mode_info_p == NULL) )
  {
    DS_3GPP_MSG0_ERROR("Input params are NULL in update data rate in flow");
    return FALSE;
  }
  memset(&app_qos, 0, sizeof(qos_spec_type));

  /* Find the bearer flow manager */
  bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(
                                                         bearer_context_p);

  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return FALSE;
  }

  return_value = ds_bearer_flow_manager_convert_data_rate(bearer_context_p,
                                             mode_info_p,
                                             &app_qos,
                                             cause_code_p);
  if (return_value == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Data Rate not updated in the flow");
    return return_value;
  }
  /*---------------------------------------------------------------------
    Update all the Flows with the updated flow rate
    Need to optimize for the case, there will be some flows in going
    down state and will not be intereseted in flow rate change.
  ---------------------------------------------------------------------*/          
  flow_context_p = list_peek_front(&(bflow_manager_p->
                                     ds_bearer_flow_manager_dyn_p->flow_context));
  while(flow_context_p != NULL)
  {
    ds_flow_context_ps_flow_set_granted_flow(flow_context_p);
    flow_context_p = list_peek_next(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                    ->flow_context),&(flow_context_p->link));
  }
  return TRUE;
} /* ds_flow_manager_update_data_rate_in_flow */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_UPDATE_FLOW_IN_TFT

DESCRIPTION
  update the flow values in a TFT.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  nw_tft_p              -  TFT information
  mode_info_p           -  Mode specific Qos.
  cause_code_p          -  Cause code 
  check_tft             -  Indicates if the TFT needs to checked.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_update_flow_in_tft
(
  ds_pdn_context_s                      *pdn_context_p,
  ds_bearer_context_s                   *bearer_context_p,
  tft_type_T                            *nw_tft,
  const cm_call_mode_info_s_type        *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p,
  boolean                                check_tft
)
{
  ds_flow_context_s            *flow_cntxv4 = NULL;
  ds_flow_context_s            *flow_cntxv6 = NULL; 
  parameter_filter_type_T      *param_type = NULL;
  byte                          param_data = 0x00;
  uint32                        temp_filter_id =0;
  uint16                        filter_id =0;
  int                           cnt = 0;
  int                           inc = 0;
  boolean                       skip_tft = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((pdn_context_p==NULL)||(bearer_context_p==NULL)||(nw_tft == NULL)
     ||(cause_code_p== NULL)||(mode_info_p == NULL))
  {
    DS_3GPP_MSG0_ERROR("Input params are NULL in update flow in tft");
    if(cause_code_p != NULL)
    {
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    }
    return FALSE;
  }
  
  if(check_tft)
  {
    DS_3GPP_MSG0_HIGH("Processing check tft in updated flow ");
    if(nw_tft->valid == FALSE)
    {
      DS_3GPP_MSG0_HIGH("TFT not valid");  
      skip_tft = TRUE;
    }
    else
    {
      if(nw_tft->operation_code != (DS_3GPP_TFT_OPCODE_NO_OP))
      {
        DS_3GPP_MSG0_ERROR("Unexpected  TFT opcode");
        *cause_code_p = DS_3GPP_INTERNAL_QOS_NOT_ACCEPTED;
        return FALSE;
      }
      skip_tft = FALSE;
    }
  }

  if(!skip_tft)
  {
    /*-----------------------------------------------------------------------
       Check for TFT Syntactic Errors
    -----------------------------------------------------------------------*/
    if(nw_tft->e_bit != DS_FLOW_E_BIT_ENABLE)
    {
      DS_3GPP_MSG0_ERROR("invalid TFT input");
      *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
      return FALSE;
    }

    if(nw_tft->num_filters != 0)
    {
      DS_3GPP_MSG0_ERROR("invalid TFT input");
      *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
      return FALSE;
    }
  
    /*-----------------------------------------------------------------------
       Extract the filter identifier information.
    -----------------------------------------------------------------------*/
    for(cnt =0; cnt < MAX_PACKET_FILTERS; cnt++)
    {
      param_type = &(nw_tft->param_list[cnt]);
      inc = 0;
      if(param_type->identifier != 
             (byte)DS_3GPP_TFT_PACKET_FILTER_IDENTIFIER)
      {
        DS_3GPP_MSG1_ERROR("received unsupported param type:%d",
                                 param_type->identifier);
      }
      else
      {
        /* we should receive in byte multiples. */
        if((param_type->content_length == 0) || 
           (param_type->content_length > MAX_FILTER_CONTENT_LENGTH))
        {
          DS_3GPP_MSG1_ERROR("received unsupported content len:%d",
                                 param_type->content_length);
          *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
          return FALSE;
        }
        else
        {
          DS_3GPP_MSG0_HIGH("Processing the filters from no-op");
          while(inc < param_type->content_length)
          {
            param_data = param_type->filter_contents[inc];
            filter_id  = (param_data & 0xF);
            inc++;
            if(ds_3gpp_tft_is_filter_identifier_requested(
                                     filter_id, &temp_filter_id) == FALSE)
            {
              ds_3gpp_tft_set_filter_identifier_bit(filter_id,&temp_filter_id);
            }
            else
            {
              DS_3GPP_MSG0_ERROR("Invalid NW input.FI-ID repeated");
              *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
              return FALSE;
            }
          }/* while param_type->content_len*/
        }/* else good params */
      }/* else param identifier is FILTER ID*/ 
    }/* for cnt < MAX_PACKET_FILTERS*/
  }/* skip_tft == FALSE */
  
  if( (temp_filter_id == 0) && (!skip_tft) )
  {
    DS_3GPP_MSG0_ERROR("Did not receive filter information");
    *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
    return FALSE;
  }

  if(!check_tft)
  {
    switch(ds_pdn_cntxt_get_pdn_context_ip_support(pdn_context_p))
    {
      case DS_UMTS_PDP_IPV4:
        flow_cntxv4 = ds_bearer_flow_manager_find_nw_flow_context(
                                 bearer_context_p,DS_3GPP_V4_FLOW_TYPE);
        if(flow_cntxv4 == NULL)
        {
          DS_3GPP_MSG0_ERROR("No previous NW flow context associated flow manager");
          *cause_code_p = DS_3GPP_INTERNAL_QOS_NOT_ACCEPTED;
          return FALSE; 
        }
      break;
      case DS_UMTS_PDP_IPV6:
        flow_cntxv6 = ds_bearer_flow_manager_find_nw_flow_context(
                                bearer_context_p,DS_3GPP_V6_FLOW_TYPE);
        if(flow_cntxv6 == NULL)
        {
          DS_3GPP_MSG0_ERROR("No previous NW flow context associated flow manager");
          *cause_code_p = DS_3GPP_INTERNAL_QOS_NOT_ACCEPTED;
          return FALSE; 
        }
      break;
      case DS_UMTS_PDP_IPV4V6:
        /*-------------------------------------------------------------------
           IP TYPE is V4
        ---------------------------------------------------------------------*/
        flow_cntxv4 = ds_bearer_flow_manager_find_nw_flow_context(
                                     bearer_context_p,DS_3GPP_V4_FLOW_TYPE);
        if(flow_cntxv4 == NULL)
        {
          DS_3GPP_MSG0_ERROR("No previous NW flow context associated flow manager");
          *cause_code_p = DS_3GPP_INTERNAL_QOS_NOT_ACCEPTED;
          return FALSE;
        }
        /*-------------------------------------------------------------------
          IP TYPE is V6
        ---------------------------------------------------------------------*/
        flow_cntxv6 = ds_bearer_flow_manager_find_nw_flow_context(
                                      bearer_context_p,DS_3GPP_V6_FLOW_TYPE);
        if(flow_cntxv6 == NULL)
        { 
          DS_3GPP_MSG0_ERROR("No previous NW flow context associated flow manager");
          *cause_code_p = DS_3GPP_INTERNAL_QOS_NOT_ACCEPTED;
          return FALSE;
        }
      break;
      default:
        DS_3GPP_MSG0_ERROR("Unknow IP Type");
        *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
        return FALSE; 
    }/* switch*/ 
  }/* !check_tft */

  /*-------------------------------------------------------------------
    Upating the data rate in the bearer.
  ---------------------------------------------------------------------*/
  if(ds_flow_manager_update_data_rate_in_flow(bearer_context_p,
                                                  mode_info_p,cause_code_p) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Could not update data rate in the Flow");
    return FALSE; 
  }
  return TRUE;
}/* ds_flow_manager_update_flow_in_tft */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_ACCEPT_MODIFY_ENABLE_FILTER

DESCRIPTION
  This function accepts the filter modification and enable the filtering.
  This happens immediately after filter modify is completed for LTE and
  after receiving nw_pdp_modify_ind for UMTS

PARAMETERS


DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_accept_modify_enable_filter
(
  ds_bearer_context_s                   *bearer_context_p
)
{
  ds_bearer_flow_manager_s        *bflow_manager_p = NULL;
  ds_flow_context_s               *flow_context_p = NULL;
  ps_extended_info_code_enum_type  ps_eic = PS_EIC_NOT_SPECIFIED;
  /*-----------------------------------------------------------------------
    Get Bearer Flow Manager
  -----------------------------------------------------------------------*/
  bflow_manager_p =
              ds_bearer_flow_manager_find_flow_manager(bearer_context_p);

  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    return DS3G_FAILURE;
  }

  /*---------------------------------------------------------------------
    Find NW FlowContext in the list
  ---------------------------------------------------------------------*/          
  flow_context_p = list_peek_front(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                                     ->flow_context));
  while(ds_flow_context_validate_flow(flow_context_p))
  {
    if(flow_context_p->ds_flow_context_dyn_p->state == 
       DS_FLOW_CONTEXT_STATE_PENDING_LL_CONFIRM)
    {
      /*-------------------------------------------------------------------
        Modify Flow Accepted.
      ---------------------------------------------------------------------*/
      if(ds_flow_context_ps_flow_modify_result(
                                          flow_context_p,TRUE,ps_eic)!=TRUE)
      {
        DS_3GPP_MSG0_ERROR("flow modify result failed");
        return DS3G_FAILURE;
      }

        /*-------------------------------------------------------------------
        Updated the Granted Flow in case of Network Initiated Qos Modification scenarios
        ---------------------------------------------------------------------*/
        ds_flow_context_ps_flow_set_granted_flow(flow_context_p);

      	if(ds_flow_context_ps_flow_filtering_control(flow_context_p, TRUE)!= TRUE)
      	{
        	DS_3GPP_MSG0_ERROR("flow filtering control failed");
        	//return DS3G_FAILURE;
      	}     
    ds_flow_context_set_flow_state(flow_context_p, DS_FLOW_CONTEXT_STATE_UP); 
    }
    flow_context_p = list_peek_next(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                      ->flow_context),&(flow_context_p->link));
  } 
  return DS3G_SUCCESS;

}

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_PROCESS_TFT_FOR_NW_BEARER_MOD

DESCRIPTION
  Verify parameters in the NW bearer modification request.
  Check if the filter identifier, filter precedence are unique. Update the TFT
  appropriately.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  nw_tft_p              -  TFT information
  app_qos_p             -  Qos in App format
  mode_info_p           -  Mode specific Qos.
  cause_code_p          -  Cause code NW TFT is not accurate

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_process_tft_for_nw_bearer_mod
(
  ds_pdn_context_s                      *pdn_context_p,
  ds_bearer_context_s                   *bearer_context_p,
  tft_type_T                            *nw_tft_p,
  const cm_call_mode_info_s_type        *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p,
  boolean                                allow_flow_create
)
{

  ds_bearer_flow_manager_s    *bflow_manager_p = NULL;  
  boolean                     rval = DS3G_SUCCESS;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((pdn_context_p == NULL) || (bearer_context_p == NULL) || (nw_tft_p == NULL)
         || (cause_code_p == NULL))
  {
    DS_3GPP_MSG0_ERROR("Input params are NULL in processTftForNw");
    if(cause_code_p != NULL)
    {
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    }
    return DS3G_FAILURE;
  }  

  /*-----------------------------------------------------------------------
    Get Bearer Flow Manager
  -----------------------------------------------------------------------*/
  bflow_manager_p =
              ds_bearer_flow_manager_find_flow_manager(bearer_context_p);

  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    return DS3G_FAILURE;
  }
  
  DS_3GPP_MSG1_HIGH("Received %d from NW",nw_tft_p->operation_code);
  switch(nw_tft_p->operation_code)
  {
    /* could be different for UMTS/LTE. Need mode specific*/
    case DS_3GPP_TFT_OPCODE_CREATE:
      /* This will need to throw away the old tft, delete all the flows.
         and bring up the new flow. */      
      *cause_code_p = DS_3GPP_INTERNAL_FEATURE_NOT_SUPPORTED;   
      rval = ds_flow_manager_process_create_tft(pdn_context_p,bearer_context_p,nw_tft_p,mode_info_p,cause_code_p);      
      break;

    /* could be different for UMTS/LTE. Need mode specific*/
    case DS_3GPP_TFT_OPCODE_DEL_TFT:
      *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_TFT;      
      rval = ds_flow_manager_process_delete_tft(pdn_context_p,bearer_context_p,nw_tft_p,mode_info_p,cause_code_p);             
      break;

    /* Add the TFT to the flow context */
    case DS_3GPP_TFT_OPCODE_ADD:
      if(ds_flow_manager_add_filter_to_tft(
                                       pdn_context_p,
                                       bearer_context_p,
                                       NULL,
                                       nw_tft_p, 
                                       mode_info_p, 
                                       cause_code_p,
                                       allow_flow_create) == FALSE)
      {
        return DS3G_FAILURE;
      }
      /* confirm the modification */

    break;
    /* Replace given TFT filter set */
    case DS_3GPP_TFT_OPCODE_REPLACE:
      if(ds_flow_manager_replace_filter_in_tft(
                                           pdn_context_p,
                                           bearer_context_p,
                                           NULL,
                                           nw_tft_p, 
                                           mode_info_p,
                                           cause_code_p) == FALSE)
      {
        return DS3G_FAILURE;
      }        
    break;
    /* Delete given TFT filter set */
    case DS_3GPP_TFT_OPCODE_DEL_FILTER:
      if(ds_flow_manager_remove_filter_from_tft(
                                            pdn_context_p,
                                            bearer_context_p,
                                            NULL,
                                            nw_tft_p, 
                                            mode_info_p,
                                            cause_code_p) == FALSE)
      {
        return DS3G_FAILURE;
      }


    break;
    case DS_3GPP_TFT_OPCODE_NO_OP:
         /* Handle no opcode. Check if the E bit is set*/
      if(ds_flow_manager_update_flow_in_tft(
                                        pdn_context_p,
                                        bearer_context_p,
                                        nw_tft_p, 
                                        mode_info_p, 
                                        cause_code_p,
                                        FALSE) == FALSE)
      {
        return DS3G_FAILURE;
      }

    break;
   case DS_3GPP_TFT_OPCODE_SPARE:
     DS_3GPP_MSG0_HIGH("OP code DS_3GPP_TFT_OPCODE_SPARE, No operation as of now,"
                     " returning Failure");
     return DS3G_FAILURE;

    break;
   default:
    return rval;
  }/* switch */


  return rval;
} /* ds_flow_manager_process_tft_for_nw_bearer_mod*/


/*===========================================================================
FUNCTION DS_FLOW_MANAGER_ALLOCATE_NEW_BEARER_FOR_TFT

DESCRIPTION
  Verify parameters in the NW response to allocate a new bearer. 
  Check if the filter identifier and filter precedence are unique. Check UE 
  requested filters and NW  responded filters.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow information.
  mode_info_p           -  mode specific call info
  nw_tft                -  TFT information in network format
  icause_code           -  Internal cause code.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_flow_manager_allocate_new_bearer_for_tft
(
  ds_pdn_context_s                *pdn_context_p,
  ds_bearer_context_s             *bearer_context_p,
  ds_flow_context_s               *flow_context_p,
  const cm_call_mode_info_s_type  *mode_info_p,
  tft_type_T                      *nw_tft,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{
  ds_bearer_flow_manager_s  *bflow_manager_p = NULL;
  ds_3gpp_nw_modify_qos_bk_s   *nw_bk;
  ip_version_enum_type         ip_type;
  ps_iface_type                *iface_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(pdn_context_p==NULL||flow_context_p==NULL||mode_info_p==NULL
      ||bearer_context_p==NULL||nw_tft == NULL||
     (!ds_flow_context_validate_flow(flow_context_p)))
  {
    DS_3GPP_MSG3_ERROR("Received NULL param:pdn_p 0x%x,flow_p 0x%x,mode_info_p 0x%x",
		  pdn_context_p, bearer_context_p, mode_info_p);
    return DS3G_FAILURE;
  }

  /*-----------------------------------------------------------------------
     Make sure this is a create operation.
  -----------------------------------------------------------------------*/
  if(nw_tft->operation_code !=(byte)DS_3GPP_TFT_OPCODE_CREATE)
  {
    DS_3GPP_MSG0_ERROR("invalid TFT operation");
    *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_TFT;
    return DS3G_FAILURE;
  }

  /*-----------------------------------------------------------------------
     Check for TFT Semantic Errors
  -----------------------------------------------------------------------*/
  if(ds_3gpp_tft_check_mod_fltr_semantic_error(nw_tft) == TRUE)
  {
    *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
    return FALSE;
  }
  else if(ds_3gpp_tft_check_for_repeated_precedence(nw_tft) == TRUE)
  {
    *cause_code_p = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
    return FALSE;
  }
  /*-----------------------------------------------------------------------
     Allocate Heap Memory
  -----------------------------------------------------------------------*/
  nw_bk = (ds_3gpp_nw_modify_qos_bk_s *)modem_mem_alloc(
                                       sizeof(ds_3gpp_nw_modify_qos_bk_s),
                                       MODEM_MEM_CLIENT_DATA);
  if(nw_bk == NULL)
  {
    DS_3GPP_MSG2_ERROR("Heap Memory Allocation failed for client: %d size: %d", 
       MODEM_MEM_CLIENT_DATA,(sizeof(ds_3gpp_nw_modify_qos_bk_s)));
    *cause_code_p =  DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    return FALSE;
  }
  memset(nw_bk, 0,sizeof(ds_3gpp_nw_modify_qos_bk_s));

  /*-------------------------------------------------------------------
    Verify if requested Filters are present in the NW response.
  -------------------------------------------------------------------*/
  if(ds_3gpp_tft_verify_nw_tft(&(flow_context_p->ds_flow_context_dyn_p
                           ->tft_mod),nw_tft, flow_context_p) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Nw response does not match UE request");
    *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
    modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
    return FALSE; 
  }
  nw_bk->is_add = FALSE;

  /*---------------------------------------------------------------------
    Verify if the bearer already has a flow manager.
  ---------------------------------------------------------------------*/
  bflow_manager_p = 
             ds_bearer_flow_manager_find_flow_manager(bearer_context_p);

  if(ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }
  /*-------------------------------------------------------------------------
   Allocate a new bearer flow manager.
  -------------------------------------------------------------------------*/
  bflow_manager_p = 
              ds_bearer_flow_manager_allocate_flow_manager(bearer_context_p);

  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    ds_3gpp_dyn_mem_hdlr_free_bearer_flow_mgr_dyn_mem(bearer_context_p);
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
    return DS3G_FAILURE;
  }  
  DS_3GPP_MSG0_HIGH("Bearer Allocated a bearer flow manager");
 
  /*---------------------------------------------------------------------
   Update the PDN PTR.
  ---------------------------------------------------------------------*/
  bflow_manager_p->ds_bearer_flow_manager_dyn_p->pdn_ptr = pdn_context_p;

  /*-------------------------------------------------------------------
    Check if the network fltr has any fltr ID or fltr preced conflicts.
  -------------------------------------------------------------------*/
  if(ds_flow_manager_detect_conflict_in_tft(pdn_context_p,
                                           bearer_context_p,
                                           flow_context_p,
                                           nw_tft,
                                           nw_bk,
                                           cause_code_p,
                                           FALSE) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Unresolved Conflict in TFT");
    *cause_code_p =DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_TFT;
    modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    Configure the QOS flow spec
  -------------------------------------------------------------------------*/
  if(ds_3gpp_tft_configure_nw_bk_flow_spec(nw_bk)==FALSE)
  {
    DS_3GPP_MSG0_ERROR("unable to configure the nw book-keeping info");
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }
  /*-------------------------------------------------------------------
    Retrieve requested filter info for flow context. 
  ---------------------------------------------------------------------*/
  if(ds_flow_context_ps_flow_get_filters(
                                    flow_context_p, 
                                    FALSE,
                               &(nw_bk->curr_qos)) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Could not retrieve requested filters");
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
    return FALSE; 
  }

  /*-----------------------------------------------------------------------
     Find the right iface pointer.
  -----------------------------------------------------------------------*/
  if(ds_flow_context_retrieve_iface_ip_type(flow_context_p,
                                            &ip_type,
                                            &iface_ptr) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("3GPP iface is not present");
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }

  /*-------------------------------------------------------------------
    Convert NW TFT to PS filters
  ---------------------------------------------------------------------*/
  if(ds_flow_manager_convert_nw_tft_to_app_tft(nw_tft,
	     &(nw_bk->mod_qos), ip_type, flow_context_p,cause_code_p) == DS3G_FAILURE)
  {
    modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }

  /*-------------------------------------------------------------------  
    Find the pending filters and update the filter identifier value
  ---------------------------------------------------------------------*/
  if ((ds_flow_context_update_filter_identifier(
                           &(nw_bk->curr_qos.rx.fltr_template),
                           &(nw_bk->mod_qos.rx.fltr_template))) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Req RX filters do not match NW granted RX filters");
    *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
    modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }
  if ((ds_flow_context_update_filter_identifier(
                          &(nw_bk->curr_qos.tx.fltr_template),
                           &(nw_bk->mod_qos.tx.fltr_template))) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Req TX filters do not match NW granted TX filters");
    *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
    modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }

  /*---------------------------------------------------------------------
    Convert NW Qos to App Qos
  ---------------------------------------------------------------------*/
  if(ds_bearer_flow_manager_convert_data_rate(bearer_context_p,
                                       mode_info_p,
                                       &(nw_bk->curr_qos),
                                       cause_code_p) == DS3G_FAILURE)
  {
    *cause_code_p = DS_3GPP_INTERNAL_QOS_NOT_ACCEPTED;
    modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
    return DS3G_FAILURE;
  }

  /*---------------------------------------------------------------------
    Update Flow Context with the Info.
  ---------------------------------------------------------------------*/
  if(nw_bk->curr_qos.rx.fltr_template.num_filters > 0)
  {
    if(ps_flow_set_rx_filter_spec(
                    iface_ptr,
                    flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr,
                    &(nw_bk->curr_qos.rx.fltr_template),
                    FALSE) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error setting RX filter spec");
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return DS3G_FAILURE;
    }
  }
  if(nw_bk->curr_qos.tx.fltr_template.num_filters > 0)
  {
    if(ps_iface_flow_set_tx_filter_spec(
                    iface_ptr,
                    flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr,
                    &(nw_bk->curr_qos.tx.fltr_template),
                    FALSE) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error setting the TX filter spec");
      *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return DS3G_FAILURE;
    }
  }
  /*-------------------------------------------------------------------
   Add bearer context to flow manager.    
  -------------------------------------------------------------------*/
  if (ds_flow_manager_add_bearer_context_to_flow_manager(
                     pdn_context_p,bearer_context_p) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Error in Adding bearer context to Flow manager");
  }
  /*-------------------------------------------------------------------
    Clear the requested precedence bit mask.
  -------------------------------------------------------------------*/
  if(ds_flow_manager_update_precedence_bit_mask(
                  pdn_context_p,flow_context_p,FALSE,FALSE,TRUE)==FALSE)
  {
    DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
  }
  /*-----------------------------------------------------------------
    Set the precedence and filter_identifier bit masks
  ------------------------------------------------------------------*/
  if(ds_flow_manager_update_filter_id_precedence(pdn_context_p,
        bearer_context_p,flow_context_p,0,&(nw_bk->curr_qos),FALSE)==FALSE)
  {
    DS_3GPP_MSG0_ERROR("Error updating filter ID precedence");
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

  /*-------------------------------------------------------------------
    Resolve any filter conflicts.
  ---------------------------------------------------------------------*/
  if(ds_flow_manager_handle_fltr_conflict(pdn_context_p,
                                         bearer_context_p,
                                         flow_context_p,
                                         nw_bk) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("unable to handle filter conflict");
    *cause_code_p = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
    return FALSE;
  }
  modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
  ds_flow_context_set_flow_state(flow_context_p,
                DS_FLOW_CONTEXT_STATE_UP);
  return DS3G_SUCCESS;
} /* ds_flow_manager_allocate_new_bearer_for_tft */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_VERIFY_NW_RESPONSE

DESCRIPTION
  Verify parameters in the NW response. Check if the filter identifier
  filter precedence are unique. Check UE requested filters and NW
  responded filters.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  flow_context_p        -  Flow information.
  mode_info_p           -  mode specific call info
  nw_tft                -  TFT information in network format
  icause_code           -  Internal cause code.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_verify_nw_response
(
 ds_pdn_context_s                *pdn_context_p,
 ds_bearer_context_s             *bearer_context_p,
 ds_flow_context_s               *flow_context_p,
 const cm_call_mode_info_s_type  *mode_info_p,
 tft_type_T                      *nw_tft,
 ds_3gpp_internal_cause_code_enum_type *icause_code
)
{
  ds_bearer_flow_manager_s  *bflow_manager_p = NULL;   
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(pdn_context_p == NULL || flow_context_p == NULL || mode_info_p == NULL ||
      (!ds_flow_context_validate_flow(flow_context_p)))
  {
    DS_3GPP_MSG3_ERROR("Received NULL param:pdn_p=%d,flow_p=%d,mode_info_p=%d",
		  pdn_context_p, bearer_context_p, mode_info_p);
    return DS3G_FAILURE;
  }

  /*---------------------------------------------------------------------
    if the pending request is in "pend abort", "pend mod". reject bearer
   ---------------------------------------------------------------------*/
  if((flow_context_p->ds_flow_context_dyn_p->state==
      DS_FLOW_CONTEXT_STATE_PEND_ALLOC_ABORT_RSP)
       ||(flow_context_p->ds_flow_context_dyn_p->state==
          DS_FLOW_CONTEXT_STATE_PEND_MODIFY_ABORT_RSP)
       ||(flow_context_p->ds_flow_context_dyn_p->state==
          DS_FLOW_CONTEXT_STATE_PEND_MODIFY_ABORT_RSP_DEL))
  {
    DS_3GPP_MSG1_ERROR("UE aborted the bearer operation: state:%d",
               flow_context_p->ds_flow_context_dyn_p->state); 
    *icause_code = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
     if(flow_context_p->ds_flow_context_dyn_p->state == 
        DS_FLOW_CONTEXT_STATE_PEND_ALLOC_ABORT_RSP)
     {
       return DS3G_FAILURE;
     }
     if(flow_context_p->ds_flow_context_dyn_p->state == 
        DS_FLOW_CONTEXT_STATE_PEND_MODIFY_ABORT_RSP)
     {
         // Need to revert to orginal filter params
       return DS3G_FAILURE;
     }/* ABORT_RSP*/
     if(flow_context_p->ds_flow_context_dyn_p->state==
        DS_FLOW_CONTEXT_STATE_PEND_MODIFY_ABORT_RSP_DEL)
     {
         //Revert to orginal filter params
       return DS3G_FAILURE;
     }/* ABORT_RSP_DEL*/
  }/* if pending request was aborted*/

  bflow_manager_p=ds_bearer_flow_manager_find_flow_manager(bearer_context_p);

  switch(flow_context_p->ds_flow_context_dyn_p->state)
  {
    case DS_FLOW_CONTEXT_STATE_PEND_ALLOC_RSP:
      if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
      {
        return(ds_flow_manager_allocate_new_bearer_for_tft(pdn_context_p,
                                           bearer_context_p,
                                           flow_context_p,
                                           mode_info_p,
                                           nw_tft,
                                           icause_code));
      }
      else
      {
        return(ds_flow_manager_add_filter_to_tft(pdn_context_p,
                                            bearer_context_p,
                                            flow_context_p,
                                            nw_tft,
                                            mode_info_p,
                                            icause_code,
                                            TRUE));
      }      

    case DS_FLOW_CONTEXT_STATE_PEND_ADD_RSP:
      if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
      {
        *icause_code = DS_3GPP_INTERNAL_SERV_OPT_NOT_SUPPORTED;
        // need to inform UE about add failure.
        return DS3G_FAILURE;
      }
      else
      {
        return(ds_flow_manager_add_filter_to_tft(pdn_context_p,
                                            bearer_context_p,
                                            flow_context_p,
                                            nw_tft,
                                            mode_info_p,
                                            icause_code,
                                            TRUE));
      }

    case DS_FLOW_CONTEXT_STATE_PEND_DEL_RSP:
    case DS_FLOW_CONTEXT_STATE_PEND_DEL_FLOW_RSP: 
      if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p)) 
      {
        *icause_code = DS_3GPP_INTERNAL_SERV_OPT_NOT_SUPPORTED;
        return DS3G_FAILURE;
      }
      else
      {
        return(ds_flow_manager_remove_filter_from_tft(pdn_context_p,
                                                bearer_context_p,
                                                flow_context_p,
                                                nw_tft,
                                                mode_info_p,
                                                icause_code));
      }
    
    case DS_FLOW_CONTEXT_STATE_PEND_REPLACE_RSP:
      if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
      {
        *icause_code = DS_3GPP_INTERNAL_SERV_OPT_NOT_SUPPORTED;
        return DS3G_FAILURE;
      }
      else
      {
        return(ds_flow_manager_replace_filter_in_tft(pdn_context_p,
                                                bearer_context_p,
                                                flow_context_p,
                                                nw_tft,
                                                mode_info_p,
                                                icause_code));
      }
    case DS_FLOW_CONTEXT_STATE_PEND_NO_OP_RSP:
      if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
      {
        *icause_code = DS_3GPP_INTERNAL_SERV_OPT_NOT_SUPPORTED;
        return DS3G_FAILURE;
      }
      else
      {
        return(ds_flow_manager_update_flow_in_tft(
                                        pdn_context_p,
                                        bearer_context_p,
                                        nw_tft, 
                                        mode_info_p, 
                                        icause_code,
                                        FALSE));
      }
    default:
      DS_3GPP_MSG1_ERROR("unexpected flow context state 0x%x",
             (uint8)flow_context_p->ds_flow_context_dyn_p->state);
      *icause_code = DS_3GPP_INTERNAL_SERV_OPT_NOT_SUPPORTED;
      return DS3G_FAILURE;
  }/* switch flow_context_p->state */
}/* ds_flow_manager_verify_nw_response */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_CREATE_FLOW_CONTEXT

DESCRIPTION
  Create a new flow context.

PARAMETERS
  pdn_context_p         -  PDN information
  bearer_context_p      -  Bearer information
  mode_info_p           -  mode specific call info
  nw_tft_p              -  TFT in network format.
  icause_code           -  internal cause code to be updated.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_create_flow_context
(
  ds_pdn_context_s                       *pdn_context_p,
  ds_bearer_context_s                    *bearer_context_p,
  const cm_call_mode_info_s_type         *mode_info_p,
  tft_type_T                             *nw_tft_p,
  ds_3gpp_internal_cause_code_enum_type  *icause_code
)
{
  ds_3gpp_iface_s              *ds_v4_iface = NULL;
  ds_3gpp_iface_s              *ds_v6_iface = NULL;
  ip_version_enum_type         ip_type; 
  int                          index =  0;
  packet_filter_type_T         *nw_filter =  NULL;
  uint16                       filter_id = 0, preced = 0;
  uint32                       temp_filter_id = 0;
  uint32                       temp_preced[DS_FI_PRECED_ARR_SIZE];
  ds_flow_manager_s            *flow_manager_p;
  int                          preced_conflict = 0;
  uint32                       preced_conflict_ar[DS_FI_PRECED_ARR_SIZE];
  ds_3gpp_nw_modify_qos_bk_s   *nw_bk = NULL;
  sys_modem_as_id_e_type       subs_id = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG2_HIGH("Create Flow Context for PDN:%d, bearer:%d",
                                            pdn_context_p,bearer_context_p);


  if((!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p, &subs_id))
     ||(bearer_context_p==NULL)||(mode_info_p== NULL))
  {
    DS_3GPP_MSG3_ERROR("Received NULL input parameters:pdn_p=%d,bearer_p=%d,mode_p=%d",
		  pdn_context_p, bearer_context_p, mode_info_p);
    return DS3G_FAILURE;
  }

  memset(temp_preced, 0, (sizeof(uint32) *DS_FI_PRECED_ARR_SIZE));
  memset(preced_conflict_ar, 0, (sizeof(uint32) *DS_FI_PRECED_ARR_SIZE));
  /*---------------------------------------------------------------------
     Find the flow manager.
  ---------------------------------------------------------------------*/ 
  flow_manager_p = 
              ds_flow_manager_find_flow_manager(pdn_context_p);
  if((!ds_flow_manager_validate_flow_manager
                                 (flow_manager_p)))
  { 
    *icause_code = DS_3GPP_INTERNAL_INSUFFICIENT_RESOURCES;
    return DS3G_FAILURE;
  }
  /*-------------------------------------------------------------------------
    Verify Filter Fields 
  -------------------------------------------------------------------------*/
  if(nw_tft_p == NULL || (nw_tft_p->valid != TRUE))
  {
    DS_3GPP_MSG0_ERROR("Received TFT type with valid flag set to FALSE");
    *icause_code = DS_3GPP_INTERNAL_INVALID_MANDATORY_INFO;
    return DS3G_FAILURE;
  }
  /*-------------------------------------------------------------------------
    Check filter content
  -------------------------------------------------------------------------*/
  for(index=0; index < nw_tft_p->num_filters; index++)
  {
    /*----------------------------------------------------------------------
     Check each filter
    -----------------------------------------------------------------------*/    
    nw_filter = &(nw_tft_p->filter_list[index]);
    filter_id = nw_filter->identifier & 0xF;
    preced = nw_filter->eval_precedence;

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
      DS_3GPP_MSG1_ERROR("Invalid NW input.FI-ID 0x:%x repeated",filter_id);
      *icause_code = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_FILTER;
      if(ds_flow_manager_convert_cause_codes
           (mode_info_p,icause_code, subs_id)== FALSE)
      {
          DS_3GPP_MSG0_ERROR("Error in converting cause code");
      }
      return DS3G_FAILURE;
    }

    /*---------------------------------------------------------------------
      Verify precedence is repeated 
    ---------------------------------------------------------------------*/
    if(ds_3gpp_tft_is_filter_preced_bit_set(preced,temp_preced)==FALSE) 
    {
      ds_3gpp_tft_set_filter_preced_bit(preced, temp_preced);
    }
    else
    {
      DS_3GPP_MSG1_ERROR("Invalid NW input.precedence 0x:%x repeated",preced);
      *icause_code = DS_3GPP_INTERNAL_SEMANTIC_ERR_IN_FILTER;
      if(ds_flow_manager_convert_cause_codes
           (mode_info_p,icause_code, subs_id)== FALSE)
      {
        DS_3GPP_MSG0_ERROR("Error in converting cause code");
      }
      return DS3G_FAILURE;
    } 

    /*---------------------------------------------------------------------
      Detect Precedence Conflict
    ---------------------------------------------------------------------*/
    if(ds_3gpp_tft_is_filter_preced_bit_set(preced,
                       flow_manager_p->ds_flow_manager_dyn_p->fi_preced)==TRUE)
    {
      DS_3GPP_MSG1_HIGH("Precedence Conflict 0x:%x",preced);
      preced_conflict++;
      ds_3gpp_tft_set_filter_preced_bit(preced, preced_conflict_ar);
    }
  }/* for index */

  /*-------------------------------------------------------------------------
    Find out the PDP Type supported by this PDN.
  -------------------------------------------------------------------------*/
  switch(ds_pdn_cntxt_get_pdn_context_ip_support(pdn_context_p))
  {
    case DS_UMTS_PDP_IPV4:
      ds_v4_iface = 
       pdn_context_p->ds_pdn_context_dyn_p->
        ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE];
      ip_type = IP_V4; 
      if(ds_flow_manager_create_flow_context_per_iface(
                                   pdn_context_p,
                                   bearer_context_p,
                                   mode_info_p,
                                   nw_tft_p,
                                   ds_v4_iface,
                                   ip_type,
                                   icause_code,
                                   TRUE) != DS3G_SUCCESS)
      {
        ds_bearer_flow_manager_release_flow_manager(bearer_context_p);
        if( (*icause_code == DS_3GPP_INTERNAL_CC_NONE) ||
            (*icause_code == DS_3GPP_INTERNAL_CC_INVALID))
        {
          DS_3GPP_MSG0_ERROR("cause code not set");
          return DS3G_FAILURE;
        }
        if(ds_flow_manager_convert_cause_codes
             (mode_info_p,icause_code, subs_id)== FALSE)
        {
          DS_3GPP_MSG0_ERROR("Error in converting cause code");
        }
        return DS3G_FAILURE;
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
                                   mode_info_p,
                                   nw_tft_p,
                                   ds_v6_iface,
                                   ip_type,
                                   icause_code,
                                   TRUE) != DS3G_SUCCESS)
      {
        ds_bearer_flow_manager_release_flow_manager(bearer_context_p);
        if( (*icause_code == DS_3GPP_INTERNAL_CC_NONE) ||
            (*icause_code == DS_3GPP_INTERNAL_CC_INVALID))
        {
          DS_3GPP_MSG0_ERROR("cause code not set");
          return DS3G_FAILURE;
        }
        if(ds_flow_manager_convert_cause_codes
             (mode_info_p,icause_code, subs_id)==FALSE)
        {
          DS_3GPP_MSG0_ERROR("Error in converting cause code");
        }       
        return DS3G_FAILURE;
      } 
    break;
    case DS_UMTS_PDP_IPV4V6:
      ds_v4_iface = 
        pdn_context_p->ds_pdn_context_dyn_p->
        ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE];
      ip_type = IP_V4;
      if(ds_flow_manager_create_flow_context_per_iface(
                                   pdn_context_p,
                                   bearer_context_p,
                                   mode_info_p,
                                   nw_tft_p,
                                   ds_v4_iface,
                                   ip_type,
                                   icause_code,
                                   TRUE) != DS3G_SUCCESS)
      {
        ds_bearer_flow_manager_release_flow_manager(bearer_context_p);
        if( (*icause_code == DS_3GPP_INTERNAL_CC_NONE) ||
            (*icause_code == DS_3GPP_INTERNAL_CC_INVALID))
        {
          DS_3GPP_MSG0_ERROR("cause code not set");
          return DS3G_FAILURE;
        }
        if(ds_flow_manager_convert_cause_codes
             (mode_info_p,icause_code, subs_id)==FALSE)
        {
          DS_3GPP_MSG0_ERROR("Error in converting cause code");
        }
        return DS3G_FAILURE;
      } 
      ds_v6_iface =
       pdn_context_p->ds_pdn_context_dyn_p->
        ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE];
      ip_type = IP_V6; 
      if(ds_flow_manager_create_flow_context_per_iface(
                                   pdn_context_p,
                                   bearer_context_p,
                                   mode_info_p,
                                   nw_tft_p,
                                   ds_v6_iface,
                                   ip_type,
                                   icause_code,
                                   TRUE) != DS3G_SUCCESS)
      {
        /* Clean up all the flow contexts */
        if(ds_bearer_flow_manager_delete_all_flow_context(
                     pdn_context_p,bearer_context_p)== FALSE)
        {
          DS_3GPP_MSG0_ERROR("Error returned when deleting flow contexts");
        }
        ds_bearer_flow_manager_release_flow_manager(bearer_context_p);
        if( (*icause_code == DS_3GPP_INTERNAL_CC_NONE) ||
            (*icause_code == DS_3GPP_INTERNAL_CC_INVALID))
        {
          DS_3GPP_MSG0_ERROR("cause code not set");
          return DS3G_FAILURE;
        }
        if(ds_flow_manager_convert_cause_codes
             (mode_info_p,icause_code, subs_id)==FALSE) 
        {
          DS_3GPP_MSG0_ERROR("Error in converting cause code");
        }
        return DS3G_FAILURE;
      } 
    break;
    default:
      DS_3GPP_MSG0_ERROR("Unsuported PDP Type in profile");
      ds_bearer_flow_manager_release_flow_manager(bearer_context_p);
      return DS3G_FAILURE;
  }/* switch Profile's PDP TYPE */

  if(preced_conflict >  0)
  {
    /*-----------------------------------------------------------------------
      Allocate Heap Memory
    -----------------------------------------------------------------------*/
    nw_bk = (ds_3gpp_nw_modify_qos_bk_s *)modem_mem_alloc(
                                       sizeof(ds_3gpp_nw_modify_qos_bk_s),
                                       MODEM_MEM_CLIENT_DATA);
    if(nw_bk == NULL)
    {
      DS_3GPP_MSG2_ERROR("Heap Memory Allocation failed for client: %d size: %d", 
      MODEM_MEM_CLIENT_DATA,(sizeof(ds_3gpp_nw_modify_qos_bk_s)));
      return DS3G_FAILURE;
    }
    memset(nw_bk, 0,sizeof(ds_3gpp_nw_modify_qos_bk_s));
    
    // Update the preced conflict information.
    nw_bk->preced_conflict_cnt = preced_conflict;
    memscpy(nw_bk->preced_conflict,(sizeof(uint32) * DS_FI_PRECED_ARR_SIZE),
	    preced_conflict_ar,(sizeof(uint32) * DS_FI_PRECED_ARR_SIZE));

    if(ds_3gpp_tft_find_bearer_with_preced_conflict(pdn_context_p,
                                                   bearer_context_p,
                                                   NULL,
                                                   nw_bk) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Un expected filter precedence error ");
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return DS3G_FAILURE;
    }
  
   /* Handle precedence conflict */
    if(ds_flow_manager_handle_fltr_preced_conflict(pdn_context_p,
                                              bearer_context_p,
                                              NULL,
                                              nw_bk) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("unable to handle precedence conflict");
      modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
      return DS3G_FAILURE;
    }

    /* release heap memory */
    modem_mem_free(nw_bk, MODEM_MEM_CLIENT_DATA);
  }/* preced_conflict >  0 */

  /*-------------------------------------------------------------------------
    Add Bearer Flow manager to Flow manager.
  -------------------------------------------------------------------------*/
  if ((ds_flow_manager_add_bearer_context_to_flow_manager(
                pdn_context_p,bearer_context_p)) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Error in adding bearer flow manager to flow manager");
    return DS3G_FAILURE;
  }
  return DS3G_SUCCESS;
} /*ds_flow_manager_create_flow_context */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_FLOW_IOCTL_HDLR

DESCRIPTION
  Flow ioctl Callback called from application context for secondary PDP
  context activation. Central function for embedded applications deactivate a 
  secondary context,resume a dormant secondary context or modify qos 
  parameters of a secondary context.

PARAMETERS
  *flow_ptr         - Ptr to the flow to operate on (validated by calling fn)
  ioctl_name        - Type of request
  *argval_ptr        - QoS specified by the application (validated by calling fn)
  *ps_errno         - Ptr to error code (if any)


DEPENDENCIES
  None.

RETURN VALUE
  0  - On Success
  -1 - On Failure
   
SIDE EFFECTS

===========================================================================*/
int ds_flow_manager_flow_ioctl_hdlr
(
  ps_flow_type            *flow_ptr,
  ps_flow_ioctl_type      ioctl_name,
  void                    *argval_ptr,
  int16                   *ps_errno
  /*lint -esym(715,argval_ptr) */
)
{
  int ret_val = -1;
  ds_flow_context_s             *flow_context_p  = NULL;
  ps_flow_ioctl_qos_modify_type *modify_info      = NULL;
  ds_3gpp_iface_s               *ds_iface_ptr   = NULL;
  ds_pdn_context_s              *pdn_context_p  = NULL;  
  sys_modem_as_id_e_type         subs_id        = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    For bringing up or down primary PDP context, phys link command callback
    functions of ps iface are used.
    For bringing up the secondary PDP context, ps flow IOCTL function call
    back and phys link command call back function are used.
    For bring down or modifying secondary PDP context, phys link ioctl
    callback and phys link command callback functions of phys link are used.

    Bringup Secondary context            : dsumtsps_iface_ioctl_hdlr
    Teardown Secondary context           : dsumtsps_flow_ioctl_hdlr
    Bringup Secondary context phys link  : dsumtsps_phys_link_up
    Teardown Secondary context phys link : dsumtsps_phys_link_down
    Modify S-PDP Context                 : dsumtsps_modify_sec_ctx
    Modify P-PDP Context                 : dsumtsps_modify_pri_ctx
    Query netw allocated QoS on P-PDP    : dsumtsps_conv_umts_qos_to_app_qos
  -------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
    It is expected that pointers flow_ptr and ps_errno has been validated 
    by calling routine. since this is a handler routine we'll be trapping 
    annomalies in debug environment by ASSERT
  -------------------------------------------------------------------------*/
  ASSERT (flow_ptr != NULL );
  ASSERT (ps_errno != NULL );

  /* From flow ptr find flow context
     from flow context find flow manager
     from flow manager find its either bearer context or PDn context.
  */
   
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    
  switch ( ioctl_name )
  {
    case PS_FLOW_IOCTL_QOS_RELEASE:
      DS_3GPP_MSG0_HIGH("Rcvd Secondary context release request from IOCTL");
      /*-------------------------------------------------------------------
        Received IOCTL to tear down secondary context.
      -------------------------------------------------------------------*/
      flow_context_p = ds_flow_context_find_flow_context(flow_ptr);
      if(ds_flow_context_validate_flow(flow_context_p))
      {      
#ifdef FEATURE_EMERGENCY_PDN
        ds_pdn_context_s     *pdn_cntx_ptr = NULL;     
        ds_iface_ptr = flow_context_p->ds_flow_context_dyn_p->ds_iface_ptr;
        if(ds_iface_ptr == NULL)
        {
          DATA_ERR_FATAL("flow context's ds iface ptr is NULL");
        }
        /*-------------------------------------------------------------------------
          Find the PDN associated with 3GPP Iface.
        -------------------------------------------------------------------------*/ 
        pdn_cntx_ptr = (ds_pdn_context_s *)(ds_iface_ptr->client_data_ptr);        
        if (ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_ptr))
        {
          if (DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_cntx_ptr) == TRUE)
          {
            DS_3GPP_MSG0_ERROR("Emergency PDN: Bearer Modification is not allowed.");
            ret_val = -1;
            break;
          }
        }           
#endif /* FEATURE_EMERGENCY_PDN */           
        ret_val = ds_flow_manager_teardown_sec_ctx(flow_context_p,ps_errno );
      }
      else 
      {
        DS_3GPP_MSG0_ERROR("PS FLow does not have an associated flow context");
        *ps_errno = DS_EINVAL;
      }
      break;

    case PS_FLOW_IOCTL_QOS_MODIFY:
      DS_3GPP_MSG0_HIGH("Rcvd Modify flow request from IOCTL");
      /*-------------------------------------------------------------------
        Received IOCTL to modify secondary context.
      -------------------------------------------------------------------*/
      modify_info = (ps_flow_ioctl_qos_modify_type *)argval_ptr;
      if(modify_info == NULL)
      {
        DS_3GPP_MSG0_ERROR("Rcvd NULL as modify info");
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        *ps_errno = DS_EINVAL;
        return -1;
      }

      flow_context_p = ds_flow_context_find_flow_context(flow_ptr);
      if(ds_flow_context_validate_flow(flow_context_p))
      {      
        /*-------------------------------------------------------------------------
          Find the 3GPP Iface associated with PS iface.
        -------------------------------------------------------------------------*/
        ds_iface_ptr = flow_context_p->ds_flow_context_dyn_p->ds_iface_ptr;
        if(ds_iface_ptr == NULL)
        {
          DATA_ERR_FATAL("flow context's ds iface ptr is NULL");
        }
        /*-------------------------------------------------------------------------
          Find the PDN associated with 3GPP Iface.
        -------------------------------------------------------------------------*/ 
        pdn_context_p = (ds_pdn_context_s *)(ds_iface_ptr->client_data_ptr);

        if (!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p, &subs_id))
        {
          *ps_errno = DS_EINVAL;
           ret_val = -1;
           break;
        }

#ifdef FEATURE_EMERGENCY_PDN
        if (DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_context_p) == TRUE)
        {
            DS_3GPP_MSG0_ERROR("Emergency PDN: Bearer Modification is not allowed.");
            ret_val = -1;
            *ps_errno = DS_ECONNREFUSED;
            break;
        }
#endif /* FEATURE_EMERGENCY_PDN */

        if( ds_3gpp_pdn_throt_is_apn_globally_throttled
              (pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
               subs_id))
        {
          DS_3GPP_MSG0_ERROR("APN is blocked, can't proceed with QOS req");
          ret_val = -1;
          *ps_errno = DS_ECONNREFUSED;
        }
        else
        {      
          ret_val = ds_flow_manager_modify_sec_ctx(
                                               flow_context_p,
                                               modify_info,
                                               ps_errno,
                                               subs_id);
        }
      }
      else 
      {
        DS_3GPP_MSG0_ERROR("PS FLow does not have an associated flow context");
        *ps_errno = DS_EINVAL;
      }      
      break;

    case PS_FLOW_IOCTL_PRIMARY_QOS_GET_GRANTED_FLOW_SPEC:
      DS_3GPP_MSG0_HIGH("Rcvd PS_FLOW_IOCTL_PRIMARY_QOS_GET_GRANTED_FLOW_SPEC");
      {
        ds_pdn_context_s *pdn_cntx_p = NULL;
        ds_bearer_context_s *default_bearer_cntx_p = NULL;
        ds_bearer_flow_manager_s *bearer_flow_manager = NULL;
        ps_iface_type *ps_iface = PS_FLOW_GET_IFACE(flow_ptr);
        ds_3gpp_iface_s  *ds3gpp_iface_p = NULL;

        if (ps_iface != NULL)
        {
          ds3gpp_iface_p = (ds_3gpp_iface_s *)ps_iface->client_data_ptr;
        }        
                                                                                                              
        if(ds3gpp_iface_p != NULL)
        {
          pdn_cntx_p = (ds_pdn_context_s *)ds3gpp_iface_p->client_data_ptr;
        }
        if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
        {
          default_bearer_cntx_p = (ds_bearer_context_s *)pdn_cntx_p->ds_pdn_context_dyn_p->
             def_bearer_context_ptr;
        }
        if(default_bearer_cntx_p != NULL)
        {
          bearer_flow_manager = ds_bearer_flow_manager_find_flow_manager(default_bearer_cntx_p);
        }
        if(ds_bearer_flow_manager_validate_flow_manager(bearer_flow_manager))
        {
          ps_flow_ioctl_primary_qos_get_granted_flow_spec_type *flow_spec_type =
            (ps_flow_ioctl_primary_qos_get_granted_flow_spec_type *)argval_ptr;
            
          if(flow_spec_type != NULL)
          {
            memscpy(&(flow_spec_type->tx_ip_flow), sizeof(ip_flow_type), &(bearer_flow_manager->
                      ds_bearer_flow_manager_dyn_p->tx_flow),sizeof(ip_flow_type));
            memscpy(&(flow_spec_type->rx_ip_flow),sizeof(ip_flow_type), &(bearer_flow_manager->
                      ds_bearer_flow_manager_dyn_p->rx_flow),sizeof(ip_flow_type));
            ret_val = 0;   //Success
          }          
        }
        else 
        {
          DS_3GPP_MSG0_ERROR("Error in handling ioctl");
          ret_val = -1; //Failure
          *ps_errno = DS_EFAULT;
        }

      }
      break;

    case PS_FLOW_IOCTL_QOS_RESUME:
    default:
        DS_3GPP_MSG1_ERROR("FLOW_IOCTL_HDLR: Invalid IOCTL: %d",ioctl_name);
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        *ps_errno =  DS_EINVAL;
        return -1;
  }

  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  return ret_val;
} /* ds_flow_manager_flow_ioctl_hdlr() */
  /*lint +esym(715,argval_ptr) */



/*===========================================================================
FUNCTION DS_FLOW_MANAGER_3GPP_PROCESS_SEC_CTX

DESCRIPTION
  Handles the APP request to create a secondary flow. Creates the new PS flow
  and invoke mode specific handler to send the request to NW.
  Called from application context.

PARAMETERS
  *pdn_ptr          - Ptr to PDN
  *ds_iface_ptr     - Ptr to DS iface
  *iface_ptr        - Ptr to PS iface.
  *ioctl_info       - Ptr to App passed ioctl information

DEPENDENCIES
  None.

RETURN VALUE
  int               - "0" if the sec ctx is brought up.
SIDE EFFECTS
  The function access data structures from APP context.

===========================================================================*/
int ds_flow_manager_3gpp_process_sec_ctx
(
  ds_pdn_context_s                          *pdn_ptr,
  ds_3gpp_iface_s                           *ds_iface_ptr,
  ps_iface_type                             *iface_ptr,
  ps_iface_ioctl_qos_request_type           *ioctl_info
)
{
  ps_flow_create_param_type         req_qos_param;
  ds_flow_context_s                 *flow_context_p = NULL;
  qos_spec_type                     *app_qos = NULL;
  int                               ret_val = 0;
  int16                             ps_errno = 0;
  tft_type_T                        *nw_tft = NULL;
  ds_cmd_type                       *cmd_ptr = NULL;
#ifdef FEATURE_DATA_IMS
  ds_pdn_context_s                  *pdn_cntx_ptr = NULL;
  ds_bearer_context_s               *bearer_context_ptr = NULL;
#endif /* FEATURE_DATA_IMS */
  ds_flow_manager_qos_cmd_info_type *qos_cmd_info_ptr = NULL;
  sys_modem_as_id_e_type             subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((ds_iface_ptr == NULL)|| (iface_ptr == NULL) || (ioctl_info == NULL))
  {
    DS_3GPP_MSG0_ERROR("Input params are NULL");
    return -1;
  }

  if(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_ptr, &subs_id)== FALSE)
  {
    return -1;
  }

  /*-------------------------------------------------------------------------
    Call the mode specific function to validate Qos. 
  -------------------------------------------------------------------------*/
  //if(ds_eps_bearer_flow_manager_flow_validation(ioctl_info->qos_ptr) == FALSE)
  if(ds_bearer_flow_manager_flow_validation(ioctl_info->qos_ptr, 
                                            subs_id) == FALSE)  
  {
    DS_3GPP_MSG0_ERROR("App QoS is not valid");
    return -1;
  }
 /*-------------------------------------------------------------------------
    Call ps_phys_link API to set QoS, TFT
  -------------------------------------------------------------------------*/
  req_qos_param.qos_spec            = ioctl_info->qos_ptr;
  req_qos_param.flow_validate_f_ptr = NULL;
  req_qos_param.fltr_validate_f_ptr = ds_flow_manager_validate_app_qos_filter;
  req_qos_param.fltr_priority       = PS_IFACE_IPFLTR_PRIORITY_FCFS;
  req_qos_param.subset_id           = ioctl_info->subset_id;
  req_qos_param.enable_filtering    = FALSE; /* Filter is disabled*/
  req_qos_param.opcode              = PS_IFACE_IOCTL_QOS_REQUEST_OP;
#ifdef FEATURE_DATA_IMS
  /*-------------------------------------------------------------------------
    If IM CN Flag is specified, add the im cn flag to call tbl for this
    call instance.
  -------------------------------------------------------------------------*/
  pdn_cntx_ptr = (ds_pdn_context_s*)(ds_iface_ptr->client_data_ptr);
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_ptr))
  {
    return -1;
  }

  bearer_context_ptr = 
         (ds_bearer_context_s*)(pdn_cntx_ptr->ds_pdn_context_dyn_p->def_bearer_context_ptr);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_ptr))
  {
    return -1;
  }

  if (req_qos_param.qos_spec->field_mask &
      (int)IPFLOW_MASK_UMTS_IM_CN_FLAG )
  {
    bearer_context_ptr->ds_bearer_context_dyn_p->im_cn_subsystem_flag =
      req_qos_param.qos_spec->tx.flow_template.req_flow.umts_params.im_cn_flag;
  }

  DS_3GPP_MSG1_HIGH("Sec im cn = %d",
           bearer_context_ptr->ds_bearer_context_dyn_p->im_cn_subsystem_flag);
#endif /* FEATURE_DATA_IMS */

  /*-------------------------------------------------------------------------
    Find an empty Flow context. 
  -------------------------------------------------------------------------*/
  flow_context_p = ds_flow_context_allocate_flow_context();
  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return -1;
  }

  DS_3GPP_MSG0_HIGH("Allocated a free flow context");
  flow_context_p->ds_flow_context_dyn_p->ds_iface_ptr = ds_iface_ptr;
  if(pdn_ptr->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE]==ds_iface_ptr)
  {
    flow_context_p->ds_flow_context_dyn_p->flow_type = (DS_3GPP_V4_FLOW_TYPE);
  }
  else
  {
   flow_context_p->ds_flow_context_dyn_p->flow_type = (DS_3GPP_V6_FLOW_TYPE);
  }
  flow_context_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr = NULL;
  ds_flow_context_set_flow_state(flow_context_p,
                DS_FLOW_CONTEXT_STATE_DOWN);

  /*-------------------------------------------------------------------------
    Create FLow
  -------------------------------------------------------------------------*/

  
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  req_qos_param.is_nw_initiated = FALSE;
  ret_val = ps_iface_create_flow(iface_ptr,
                       NULL,
                       &req_qos_param,
                       &(flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr),
                       (int16 *)&ps_errno);
  if(ret_val == -1)
  {
    ds_flow_context_release_flow_context(flow_context_p);
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return ret_val;
  }
  app_qos = (req_qos_param.qos_spec);
  flow_context_p->sdf_id   = flow_context_p->sdf_id + DS_FLOW_SDF_ID;

  /*-------------------------------------------------------------------------
    Convert App requested TFT to NW specific TFT
  -------------------------------------------------------------------------*/

  nw_tft = (tft_type_T *)modem_mem_alloc(
                                 sizeof(tft_type_T),
                                 MODEM_MEM_CLIENT_DATA_CRIT );

  if (nw_tft == NULL ) 
  {
    DS_3GPP_MSG0_ERROR("FAIL modem_mem_alloc for nw_tft");
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return -1;
  }
  
  memset(nw_tft, 0, sizeof(tft_type_T));
  
  if(ds_flow_manager_convert_app_tft_to_nw_tft(nw_tft,app_qos) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("APP TFT conversion to NW TFT failed");
    if(ds_flow_manager_update_precedence_bit_mask(
                        pdn_ptr,flow_context_p,FALSE,FALSE,TRUE)==FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
    }
    ds_flow_context_release_flow_context(flow_context_p);
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
    return -1;
  }

    /*-------------------------------------------------------------------------
    Update the PDN context pending list.
  -------------------------------------------------------------------------*/
  if(ds_flow_manager_add_pending_flow_context(pdn_ptr,flow_context_p) == 
      FALSE)
  {
    if(ds_flow_manager_update_precedence_bit_mask(
                         pdn_ptr,flow_context_p,FALSE,FALSE,TRUE)==FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
    }
    ds_flow_context_release_flow_context(flow_context_p);
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
    return -1;
  }

  /*-------------------------------------------------------------------------
    Post flow activate Command.
  -------------------------------------------------------------------------*/
  if( ds_flow_context_ps_flow_activate_cmd(flow_context_p) == -1 )
  {
    if(ds_flow_manager_update_precedence_bit_mask(
                         pdn_ptr,flow_context_p,FALSE,FALSE,TRUE)==FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
    }  
    ds_flow_context_release_flow_context(flow_context_p);
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
    return -1;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  
  /*-----------------------------------------------------------------------
    Post Command to initiate bearer modification request.
  -----------------------------------------------------------------------*/
  cmd_ptr = 
    ds_allocate_cmd_buf(sizeof(ds_flow_manager_qos_cmd_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
    return -1;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_FLOW_INVOKE_LOWER_LAYER;
  qos_cmd_info_ptr = 
    (ds_flow_manager_qos_cmd_info_type*)cmd_ptr->cmd_payload_ptr;
  qos_cmd_info_ptr->event_type = DS_3GPP_FLOW_BEARER_ALLOC;
  qos_cmd_info_ptr->flow_context_p = (void*)flow_context_p;

  memscpy(&(qos_cmd_info_ptr->nw_tft),sizeof(tft_type_T),
          nw_tft, sizeof(tft_type_T));

  memscpy(&(qos_cmd_info_ptr->app_qos),sizeof(qos_spec_type),
          app_qos, sizeof(qos_spec_type));

  qos_cmd_info_ptr->pdn_context_p = (void *)pdn_ptr;
  qos_cmd_info_ptr->flow_state = 
    (int)(flow_context_p->ds_flow_context_dyn_p->state);

  DS_3GPP_MSG1_HIGH("Posted invoke bearer modify on flow context 0x:%x",
                    flow_context_p);

  ds_put_cmd_ext(cmd_ptr);

  /*-------------------------------------------------------------------------
    Update Flow params. 
  -------------------------------------------------------------------------*/
  flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr->ps_flow_ioctl_f_ptr=
                                           ds_flow_manager_flow_ioctl_hdlr;
  ioctl_info->flow_ptr = flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr;
  modem_mem_free((void *)nw_tft, MODEM_MEM_CLIENT_DATA_CRIT );
  return 0;
} /* ds_flow_manager_3gpp_process_sec_ctx */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_TEARDOWN_SEC_CTX

DESCRIPTION
  Function to process the secondary ctx tear down request from App. Invokes 
  mode specific function to process the teardown.

PARAMETERS
  *flow_context_p   - Ptr to the flow context
  *ps_errno         - Ptr to error code (if any)


DEPENDENCIES
  None.

RETURN VALUE
  0  - On Success
  -1 - On Failure
   
SIDE EFFECTS

===========================================================================*/
int ds_flow_manager_teardown_sec_ctx
(
  ds_flow_context_s   *flow_context_p ,
  int16                   *ps_errno
  /*lint -esym(715,argval_ptr) */
)
{  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((ps_errno == NULL) || 
     (!ds_flow_context_validate_flow(flow_context_p)))
  {
    DS_3GPP_MSG2_ERROR("Invalid input params: flow_p:0x%x,ps_err:0x:%x",
                                flow_context_p, ps_errno);
    if(ps_errno != NULL)
    {
    *ps_errno = DS_EFAULT;
    }
    return -1;
  }
  /*-------------------------------------------------------------------------
    Check the flow context state
  -------------------------------------------------------------------------*/
  if(flow_context_p->ds_flow_context_dyn_p->state==DS_FLOW_CONTEXT_STATE_DOWN)
  {
    DS_3GPP_MSG0_ERROR("Flow context not in use");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-----------------------------------------------------------------------
    Post Flow go null command
  -----------------------------------------------------------------------*/
  if (ds_flow_context_ps_flow_go_null_cmd(flow_context_p) == -1 )
  {
    DS_3GPP_MSG0_ERROR("Flow context go null command failed");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-----------------------------------------------------------------------
    Call mode specific processing
  -----------------------------------------------------------------------*/
  if (ds_flow_manager_tear_down_sec_bearer(flow_context_p) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("error tearing down secondary bearer");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  
  return 0;
}/* ds_flow_manager_teardown_sec_ctx */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_TEARDOWN_BEARER

DESCRIPTION
  Function to process the secondary bearer tear down. Invokes 
  mode specific function to process the bearer teardown.

PARAMETERS
  *bearer_context_p      - Ptr to bearer context
  *preced_conflict       - Ptr to array of precedence conflict values
   local_deactivate      - Boolean indicating if the bearer needs to be
                           locally deactivated.


DEPENDENCIES
  None.

RETURN VALUE
  0  - On Success
  -1 - On Failure
   
SIDE EFFECTS

===========================================================================*/
int ds_flow_manager_teardown_bearer
(
  ds_bearer_context_s  *bearer_context_p,
  uint32               *preced_conflict,
  boolean              local_deactivate
  /*lint -esym(715,argval_ptr) */
)
{
  ds_flow_manager_s             *flow_manager_p = NULL;
  ds_bearer_flow_manager_s      *bflow_manager_p = NULL;
  ds_flow_context_s             *flow_context_p = NULL;
  ds_flow_context_s             *next_flow_cntx = NULL;
  boolean                        last_flow = FALSE;
  uint16                         loop =0;
  uint32                         temp_value = 1;
  tft_type_T                     nw_tft;
  uint32                         temp_fi_id = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((bearer_context_p == NULL) || (preced_conflict == NULL))
  {
    DS_3GPP_MSG2_ERROR("Invalid input params: flow_p:0x%x,ps_err:0x:%x",
                              bearer_context_p, preced_conflict);
    return -1;
  }
  /*---------------------------------------------------------------------
    Find the bearer flow manager
  ---------------------------------------------------------------------*/
  bflow_manager_p = ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
  {
    DS_3GPP_MSG1_ERROR("Bearer 0x:%x does not have bflow manager",
                                      bearer_context_p);
    return -1;
  }

  /*---------------------------------------------------------------------
    Find the flow manager
  ---------------------------------------------------------------------*/
  flow_manager_p = ds_flow_manager_find_flow_manager(bflow_manager_p->
                                       ds_bearer_flow_manager_dyn_p->pdn_ptr);
  if (!ds_flow_manager_validate_flow_manager(flow_manager_p))
  {
    DS_3GPP_MSG1_ERROR("PDN 0x:%x does not have flow manager",
                    bflow_manager_p->ds_bearer_flow_manager_dyn_p->pdn_ptr);
    return -1;
  }
  /*---------------------------------------------------------------------
    Initialize the network TFT
  ---------------------------------------------------------------------*/
  memset(&nw_tft, 0, sizeof(tft_type_T));

  /*---------------------------------------------------------------------
    Enter Critical section
  ---------------------------------------------------------------------*/
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

  // Copy filter identifier before clearing.
  temp_fi_id = bflow_manager_p->ds_bearer_flow_manager_dyn_p->fi_identifier;

  /*---------------------------------------------------------------------
    Go through all the flow context list for the bearer. 
    Keep the last flow context.
  ---------------------------------------------------------------------*/
  flow_context_p = list_peek_front(&(bflow_manager_p->
                               ds_bearer_flow_manager_dyn_p->flow_context)); 

  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return -1;
  }

  if(list_size(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                 ->flow_context)) == 1)
  {
    last_flow = TRUE;
  }
  while(!last_flow)
  {
    /*---------------------------------------------------------------------
      Update the Filter precedence bit mask
    ---------------------------------------------------------------------*/
    if(ds_flow_manager_update_precedence_bit_mask(
                     bflow_manager_p->ds_bearer_flow_manager_dyn_p->pdn_ptr,
                   flow_context_p,FALSE,FALSE,FALSE)==FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
    }
    /*---------------------------------------------------------------------
      Update the Filter Identifier bit mask
    ---------------------------------------------------------------------*/
    if(ds_bearer_flow_manager_update_filter_identifier_bit_mask(
                      bflow_manager_p->ds_bearer_flow_manager_dyn_p->bearer_ptr,
                       flow_context_p,FALSE)==FALSE)
    {
      DS_3GPP_MSG0_ERROR("Error in Clearing Filter identifier bit mask");
    }
 
    next_flow_cntx = list_peek_next(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p
                                      ->flow_context),
                               &(flow_context_p->link));
    if(next_flow_cntx != NULL)
    {
      list_pop_item(&(bflow_manager_p->ds_bearer_flow_manager_dyn_p->flow_context),
                               &(flow_context_p->link));
      /*---------------------------------------------------------------------
        Complete list operations and then release the Flow context instance.
      ---------------------------------------------------------------------*/  
      ds_flow_context_release_flow_context(flow_context_p);
      flow_context_p = next_flow_cntx;
    }
    else
    {
      last_flow = TRUE;  
    }  
  }/* while*/  
  /*---------------------------------------------------------------------
    Update the Filter precedence bit mask for last flow context
   ---------------------------------------------------------------------*/
  if(ds_flow_manager_update_precedence_bit_mask(
                     bflow_manager_p->ds_bearer_flow_manager_dyn_p->pdn_ptr,
                   flow_context_p,FALSE,FALSE,FALSE)==FALSE)
  {
    DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
  }
  /*---------------------------------------------------------------------
    Update the Filter Identifier bit mask for last flow context
  ---------------------------------------------------------------------*/
  if(ds_bearer_flow_manager_update_filter_identifier_bit_mask(
                     bflow_manager_p->ds_bearer_flow_manager_dyn_p->bearer_ptr,
                       flow_context_p,FALSE)==FALSE)
  {
    DS_3GPP_MSG0_ERROR("Error in Clearing Filter identifier bit mask");
  }

  /*---------------------------------------------------------------------
    Reset the Filter precedence bit mask for conflict filter precedence
  ---------------------------------------------------------------------*/
  for(loop=0; loop<256; loop++)
  {
    if(ds_3gpp_tft_is_filter_preced_bit_set(loop, preced_conflict))
      {
          ds_3gpp_tft_set_filter_preced_bit(
                  loop,flow_manager_p->ds_flow_manager_dyn_p->fi_preced);
      
      DS_3GPP_MSG1_HIGH("Precedence set for 0x:%x",loop);
        }
  }/* for loop */

  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    DATA_ERR_FATAL("Last Flow context is INVALID");
  }
  /*---------------------------------------------------------------------
    Configure the flow state
  ---------------------------------------------------------------------*/
  flow_context_p->ds_flow_context_dyn_p->state = 
    DS_FLOW_CONTEXT_STATE_PEND_DEL_FLOW_RSP;

  /*---------------------------------------------------------------------
    Leave Critical section
  ---------------------------------------------------------------------*/
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

  /*---------------------------------------------------------------------
    Configure the Filter identifier in the TFT
  ---------------------------------------------------------------------*/
  temp_value = 1;
  loop = 0;
  while (loop < 32)
  {
    if(temp_fi_id  & (temp_value << loop)) 
    {
      if(loop > 15)
      {
        DS_3GPP_MSG1_ERROR("Invalid FI-ID for 0x:%x",loop);
      }
      else
      {
        nw_tft.filter_list[nw_tft.num_filters].identifier =(byte)(loop);
        nw_tft.num_filters++; 
        DS_3GPP_MSG1_HIGH("FI-ID set for 0x:%x",loop);
      }
    }
    loop++;
  }/* while loop*/

 /*---------------------------------------------------------------------
    Update TFT Length,other params
  ---------------------------------------------------------------------*/
  nw_tft.valid = TRUE;
  nw_tft.tft_total_length = 1 + nw_tft.num_filters;
  nw_tft.operation_code =(byte) DS_3GPP_TFT_OPCODE_DEL_FILTER;
  nw_tft.e_bit = DS_FLOW_E_BIT_DISABLE; 

  /*---------------------------------------------------------------------
    Invoke the modify specific bearer modify.
  ---------------------------------------------------------------------*/
  if(ds_flow_manager_ue_bearer_modify_req(
                                          flow_context_p,
                                          (tft_type_T *)&nw_tft,
                                          NULL,
                        flow_context_p->ds_flow_context_dyn_p->state)==-1)
  {
    DS_3GPP_MSG0_ERROR("UE bearer modify req failed");
    return -1;
  }  
  return 0;
}/* ds_flow_manager_teardown_bearer */


/*===========================================================================
FUNCTION DS_FLOW_MANAGER_MODIFY_SEC_CTX

DESCRIPTION
  Function to process the secondary ctx modify request from App. Invokes 
  mode specific function to process the modification.

PARAMETERS
  *flow_context_p   - Ptr to the flow to context
  *ps_errno         - Ptr to error code (if any)
   subs_id          - Subscription Id


DEPENDENCIES
  None.

RETURN VALUE
  0  - On Success
  -1 - On Failure
   
SIDE EFFECTS

===========================================================================*/
int ds_flow_manager_modify_sec_ctx
(
  ds_flow_context_s                *flow_context_p,
  ps_flow_ioctl_qos_modify_type    *modify_info,
  int16                            *ps_errno,
  sys_modem_as_id_e_type            subs_id
  /*lint -esym(715,argval_ptr) */
)
{
  ps_flow_modify_param_type         modify_qos_param;
  qos_spec_type                     orig_spec;
  qos_spec_type                     *modify_qos = NULL;  
  int                               ret_val = -1;
  ds_3gpp_app_modify_qos_bk_s       *app_bk;
  tft_type_T                        *tft = NULL;
  ds_pdn_context_s                  *pdn_context_p = NULL;
  ds_3gpp_iface_s                   *ds_iface_p = NULL;
  ds_cmd_type                       *cmd_ptr = NULL;
  ds_3gpp_app_filter_op             app_filter_op=DS_3GPP_APP_FLTR_INVALID_OP;
  ds_flow_manager_qos_cmd_info_type *qos_cmd_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((flow_context_p == NULL)||(modify_info == NULL)||(ps_errno == NULL)||
     (!ds_flow_context_validate_flow(flow_context_p)))
  {
    DS_3GPP_MSG3_ERROR("Null params passed. flow_p 0x%x,mod_in 0x%x, ps_errno 0x%x",
                flow_context_p, modify_info, ps_errno);
    if(ps_errno != NULL)
    {
      *ps_errno = DS_EFAULT;
    }
    return -1;
  }

  if( (ds_iface_p = flow_context_p->ds_flow_context_dyn_p->ds_iface_ptr) == NULL)
  {
    DS_3GPP_MSG0_ERROR("DS IFACE PTR is null for flow context");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  
  if(ds_iface_p->client_data_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("PDN CNTX PTR is null");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  pdn_context_p = ds_iface_p->client_data_ptr;
  
  memset(&modify_qos_param, 0, sizeof(ps_flow_modify_param_type));
  /*-------------------------------------------------------------------------
    Check the flow context state
  -------------------------------------------------------------------------*/
  if(flow_context_p->ds_flow_context_dyn_p->state==DS_FLOW_CONTEXT_STATE_DOWN)
  {
    DS_3GPP_MSG0_ERROR("Flow context not in use");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Check the flow context state is in the valid state
  -------------------------------------------------------------------------*/
  if(flow_context_p->ds_flow_context_dyn_p->state != DS_FLOW_CONTEXT_STATE_UP)
  {
    DS_3GPP_MSG1_ERROR("Flow context invalid state 0x%x",flow_context_p->
                       ds_flow_context_dyn_p->state);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Verify the new filters have Fltr id and precedence as expected.
  -------------------------------------------------------------------------*/
  modify_qos = modify_info->qos_ptr;
  if(modify_qos == NULL)
  {
    DS_3GPP_MSG0_ERROR("modified Qos input is null");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Check the flow flags.
  -------------------------------------------------------------------------*/
  if(!(modify_qos->field_mask & ( QOS_MASK_RX_FLOW | 
                                QOS_MASK_TX_FLOW |
                                QOS_MODIFY_MASK_RX_FLTR_MODIFY |
                                QOS_MODIFY_MASK_TX_FLTR_MODIFY )))
  {
    DS_3GPP_MSG0_ERROR("invalid Qos mask passed");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if(modify_qos->field_mask & QOS_MODIFY_MASK_RX_FLTR_MODIFY)
  {
    app_filter_op |= DS_3GPP_APP_RX_FLTR_OP;
  }

  if(modify_qos->field_mask & QOS_MODIFY_MASK_TX_FLTR_MODIFY)
  {
    app_filter_op |= DS_3GPP_APP_TX_FLTR_OP;
  }
  if(app_filter_op != DS_3GPP_APP_FLTR_INVALID_OP)
  {
    /*-----------------------------------------------------------------------
      Verify the Identifer for the filters in APP Qos.
    -----------------------------------------------------------------------*/
    if(ds_bearer_flow_manager_verify_app_filter_identifier(
                                          modify_qos, app_filter_op)==FALSE)
    {
      DS_3GPP_MSG0_ERROR("App Filter Identifier are not valid");
      *ps_errno = DS_EFAULT;
      return -1;
    }
  
    /*-------------------------------------------------------------------------
      Verify the Precedence for the filters in APP Qos.
    -------------------------------------------------------------------------*/
//    if(ds_flow_manager_verify_app_filter_precedence(
//                            NULL,modify_qos,FALSE, app_filter_op, TRUE)==FALSE)
//    {
//      DS_3GPP_MSG0_ERROR("App Filter Precedence is not valid");
//      *ps_errno = DS_EFAULT;
//      return -1;
//    }
  }/* if != DS_3GPP_APP_FLTR_INVALID_OP */

  if(modify_qos->field_mask & QOS_MASK_RX_FLOW)
  {
    app_filter_op |= DS_3GPP_APP_RX_DATA_RATE_OP;
  }

  if(modify_qos->field_mask & QOS_MASK_TX_FLOW)
  {
    app_filter_op |= DS_3GPP_APP_TX_DATA_RATE_OP;
  }
  if(app_filter_op&(DS_3GPP_APP_RX_DATA_RATE_OP|DS_3GPP_APP_TX_DATA_RATE_OP))
  {
    /*-------------------------------------------------------------------------
     Call the mode specific function to validate Qos. 
    -------------------------------------------------------------------------*/
    if(ds_bearer_flow_manager_flow_validation(modify_qos, subs_id) == FALSE)  
    {
      DS_3GPP_MSG0_ERROR("App QoS is not valid");
      return -1;
    }
  }
  /*-------------------------------------------------------------------------
    Memset parameters before making the function call
  -------------------------------------------------------------------------*/
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
    *ps_errno = DS_NOMEMORY;
    return -1; 
  }

  orig_spec.tx.fltr_template.list_ptr = 
       (ip_filter_type *)modem_mem_calloc(MAX_PACKET_FILTERS,
                                          sizeof(ip_filter_type),
                                          MODEM_MEM_CLIENT_DATA);
  if(orig_spec.tx.fltr_template.list_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Heap Memory Allocation failed");
    modem_mem_free((void *)orig_spec.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    *ps_errno = DS_NOMEMORY;
    return -1;   
  }  

  /*-------------------------------------------------------------------------
    Retrieve the current filters
  -------------------------------------------------------------------------*/
  if(ds_flow_context_ps_flow_get_filters(
                                    flow_context_p,FALSE,&orig_spec)== FALSE)
  {
    DS_3GPP_MSG0_ERROR("unable to retrieve orginal filter");
    *ps_errno = DS_EFAULT;
    modem_mem_free((void *)orig_spec.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    modem_mem_free((void *)orig_spec.tx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Validate QCI did not change.
    Must be moved to mode specific.
  -------------------------------------------------------------------------*/
  if(orig_spec.rx.flow_template.req_flow.lte_params.lte_qci !=
      modify_qos->rx.flow_template.req_flow.lte_params.lte_qci)
  {
    DS_3GPP_MSG0_ERROR("QCI Value cannot be modified");
    *ps_errno = DS_EFAULT;
    modem_mem_free((void *)orig_spec.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    modem_mem_free((void *)orig_spec.tx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    return -1;
  }
  
  /*-----------------------------------------------------------------------
     Allocate Heap Memory
  -----------------------------------------------------------------------*/
  app_bk = (ds_3gpp_app_modify_qos_bk_s *)modem_mem_alloc(
                                       sizeof(ds_3gpp_app_modify_qos_bk_s),
                                       MODEM_MEM_CLIENT_DATA);
  if(app_bk == NULL)
  {
    DS_3GPP_MSG2_ERROR("Heap Memory Allocation failed for client: %d size: %d", 
       MODEM_MEM_CLIENT_DATA,(sizeof(ds_3gpp_app_modify_qos_bk_s)));
    *ps_errno =  DS_NOMEMORY;
    modem_mem_free((void *)orig_spec.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    modem_mem_free((void *)orig_spec.tx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    return -1;
  }

  tft = (tft_type_T *)modem_mem_alloc(
                          sizeof(tft_type_T),
                          MODEM_MEM_CLIENT_DATA_CRIT );
  
  if(tft == NULL)
  {
    DS_3GPP_MSG0_ERROR("memory allocation failed");
    return -1;
  }

  memset(app_bk, 0,sizeof(ds_3gpp_app_modify_qos_bk_s));
  memset(tft,0, sizeof(tft_type_T));
  app_bk->app_op = app_filter_op;

  /*-------------------------------------------------------------------------
    Compare new request with the existing filters
  -------------------------------------------------------------------------*/
  if(ds_flow_manager_compare_modify_qos(
                                    modify_qos,
                                    &orig_spec, 
                                    flow_context_p,
                                    app_bk,
                                    tft) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("unable to handle the new modify request");
    *ps_errno = DS_EFAULT;
    modem_mem_free((void *)app_bk, MODEM_MEM_CLIENT_DATA);
    modem_mem_free((void *)orig_spec.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    modem_mem_free((void *)orig_spec.tx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
    modem_mem_free(tft, MODEM_MEM_CLIENT_DATA_CRIT);
    return -1;
  }

  //We are done with the orig spec filter space. Free it.
  modem_mem_free((void *)orig_spec.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
  orig_spec.rx.fltr_template.list_ptr = NULL;
  modem_mem_free((void *)orig_spec.tx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
  orig_spec.tx.fltr_template.list_ptr = NULL;

  DS_3GPP_MSG1_HIGH("Network operation required is 0x%x",app_bk->network_op); 
  if( (app_bk->network_op == DS_3GPP_NW_FLTR_UNSUPP_OP) || 
    (app_bk->network_op == DS_3GPP_NW_FLTR_INVALID_OP) )
  {
    DS_3GPP_MSG0_ERROR("unable to handle the new modify request");
    *ps_errno = DS_EFAULT;
    modem_mem_free((void *)app_bk, MODEM_MEM_CLIENT_DATA);
    modem_mem_free(tft, MODEM_MEM_CLIENT_DATA_CRIT);
    return -1;
  }
  /*-------------------------------------------------------------------------
    Set the QOS and TFT parameters to be processed and validated by PS.
  -------------------------------------------------------------------------*/
  modify_qos_param.qos_spec            = modify_qos;
  modify_qos_param.flow_validate_f_ptr = NULL; // need to give 3GPP check
  modify_qos_param.fltr_validate_f_ptr = NULL;
  modify_qos_param.fltr_priority       = PS_IFACE_IPFLTR_PRIORITY_FCFS;
  modify_qos_param.fltr_compare_f_ptr  = NULL;
  modify_qos_param.subset_id           = modify_info->subset_id;
  
  ret_val = ps_iface_modify_flow(&(flow_context_p->ds_flow_context_dyn_p
                                   ->ds_iface_ptr->ps_iface),
                      flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr,
                                 &modify_qos_param,
                                 (int16 *)ps_errno);
  if (ret_val == -1)
  {
    DS_3GPP_MSG1_ERROR("Error Modifying QOS Spec for flow context %d",
                                               flow_context_p->sdf_id);
    //If add operation, then we need to undo requested precedence
    *ps_errno = DS_EFAULT;
    modem_mem_free((void *)app_bk, MODEM_MEM_CLIENT_DATA);
    modem_mem_free(tft, MODEM_MEM_CLIENT_DATA_CRIT);
    return -1;
  }
                                          
  ds_flow_context_set_flow_state(flow_context_p,
                                    DS_FLOW_CONTEXT_STATE_PEND_MODIFY_RSP);

  /*-----------------------------------------------------------------------
    Verify if the network operation is FLTR Add request.
    This may be handled differently for UMTS. Move all this code to 
    mode specific file.
  ----------------------------------------------------------------------*/
  if(app_bk->network_op == DS_3GPP_NW_FLTR_ADD_OP)
  {
    DS_3GPP_MSG0_HIGH("App Qos Modify results in adding filters");
    /*-----------------------------------------------------------------------
      Extract the PDN context from the 3GPP iface.
     ----------------------------------------------------------------------*/
    if( (ds_iface_p = flow_context_p->ds_flow_context_dyn_p->ds_iface_ptr) == NULL)
    {
      DS_3GPP_MSG0_ERROR("DS IFACE PTR is null for flow context");
      modem_mem_free((void *)app_bk, MODEM_MEM_CLIENT_DATA);
      modem_mem_free(tft, MODEM_MEM_CLIENT_DATA_CRIT);
      *ps_errno = DS_EFAULT;
      // cancel the flow modification.
      return -1;
    }  

    if(ds_iface_p->client_data_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("PDN CNTX PTR is null");
      modem_mem_free((void *)app_bk, MODEM_MEM_CLIENT_DATA);
      modem_mem_free(tft, MODEM_MEM_CLIENT_DATA_CRIT);
      *ps_errno = DS_EFAULT;
      // cancel the flow modification.
      return -1;
    }
    pdn_context_p = ds_iface_p->client_data_ptr;
    /*-----------------------------------------------------------------------
      Post Command to initiate bearer modification request.
    -----------------------------------------------------------------------*/
    cmd_ptr = 
      ds_allocate_cmd_buf(sizeof(ds_flow_manager_qos_cmd_info_type));
    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      modem_mem_free(tft, MODEM_MEM_CLIENT_DATA_CRIT);
      return -1;
    }

    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_FLOW_INVOKE_LOWER_LAYER;
    qos_cmd_info_ptr = 
      (ds_flow_manager_qos_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

    qos_cmd_info_ptr->event_type = DS_3GPP_FLOW_BEARER_ALLOC;
    qos_cmd_info_ptr->flow_context_p = (void*)flow_context_p;

    memscpy(&(qos_cmd_info_ptr->nw_tft),sizeof(tft_type_T),
            tft, sizeof(tft_type_T));

    memscpy(&(qos_cmd_info_ptr->app_qos),sizeof(qos_spec_type),
            modify_qos, sizeof(qos_spec_type));

    qos_cmd_info_ptr->pdn_context_p = pdn_context_p;
    qos_cmd_info_ptr->flow_state = 
      (int)flow_context_p->ds_flow_context_dyn_p->state;

    DS_3GPP_MSG1_HIGH("Posted invoke bearer modify on flow context 0x:%x",
                      flow_context_p);

    ds_put_cmd_ext(cmd_ptr);

    modem_mem_free((void *)app_bk, MODEM_MEM_CLIENT_DATA);
    modem_mem_free(tft, MODEM_MEM_CLIENT_DATA_CRIT);
    return 0;
  }/* network_op == DS_3GPP_NW_FLTR_ADD_OP */

  /*-----------------------------------------------------------------------
    Post Command to initiate bearer modification request.
  -----------------------------------------------------------------------*/
  cmd_ptr = 
    ds_allocate_cmd_buf(sizeof(ds_flow_manager_qos_cmd_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    modem_mem_free(tft, MODEM_MEM_CLIENT_DATA_CRIT);
    return -1;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_FLOW_INVOKE_LOWER_LAYER;
  qos_cmd_info_ptr = 
    (ds_flow_manager_qos_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

  qos_cmd_info_ptr->event_type = DS_3GPP_FLOW_BEARER_MODIFY;
  qos_cmd_info_ptr->flow_context_p = (void*)flow_context_p;
   
  memscpy(&(qos_cmd_info_ptr->nw_tft),sizeof(tft_type_T),
          tft, sizeof(tft_type_T));

  memscpy(&(qos_cmd_info_ptr->app_qos),sizeof(qos_spec_type),
          modify_qos, sizeof(qos_spec_type));

  qos_cmd_info_ptr->pdn_context_p = NULL;
  qos_cmd_info_ptr->flow_state = 
    (int)flow_context_p->ds_flow_context_dyn_p->state;

  DS_3GPP_MSG1_HIGH("Posted invoke bearer modify on flow context 0x:%x",
                    flow_context_p);

  ds_put_cmd_ext(cmd_ptr);

  modem_mem_free((void *)app_bk, MODEM_MEM_CLIENT_DATA);
  modem_mem_free(tft, MODEM_MEM_CLIENT_DATA_CRIT);
  return 0;
} /* ds_flow_manager_modify_sec_ctx */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_PROCESS_PS_CMD

DESCRIPTION
  This function processes flow specific PS commands received by the DS task

PARAMETERS
  IN
    const ds_cmd_type* - Pointer to the command buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_manager_process_ps_cmd
(
 const ds_cmd_type *cmd_ptr 
)
{
  int16                             ps_errno = 0;
  ds_flow_context_s                 *flow_context_p = NULL;
  ds_flow_manager_cmd_info_type     *cmd_info_ptr = NULL;
  ds_flow_manager_qos_cmd_info_type *qos_cmd_info_ptr = NULL;
  ds_bearer_context_s *bearer_context_p = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL))
  {
    DATA_ERR_FATAL("NULL cmd_ptr passed, return");
    return;
  }

  /*-------------------------------------------------------------------------
    Invoke the appropriate function to process the command, based on the
    command id.
  -------------------------------------------------------------------------*/
  switch(cmd_ptr->hdr.cmd_id)
  {
  
    case DS_CMD_3GPP_FLOW_FLTR_ID_CONFLICT:
    /*---------------------------------------------------------------------
      Command to tear down the flow context on which the filter ID conflict 
      occured.
    ---------------------------------------------------------------------*/
      cmd_info_ptr = (ds_flow_manager_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

      DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

      flow_context_p=(ds_flow_context_s *)(cmd_info_ptr->flow_context_p);

      if(ds_flow_context_validate_flow(flow_context_p))
      {
        flow_context_p->ds_flow_context_dyn_p->ignore_fi_id = 
          cmd_info_ptr->fltr_id_conflict;

        DS_3GPP_MSG1_HIGH("Handle Filter ID conflict for flow 0x:%x",
                          flow_context_p);
        if (ds_flow_manager_teardown_sec_ctx(
           flow_context_p, &ps_errno)!= 0)
        {
          DS_3GPP_MSG0_ERROR("flow teardown returned error");
        }
      }
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      break;

    case DS_CMD_3GPP_FLOW_FLTR_PRECED_CONFLICT:
      /*---------------------------------------------------------------------
        Command to tear down the flow context on which the filter precedence
        conflict occured.
        1) Construct a NW message, that will teardown all the flows, associated
           with the bearer.
        2) if Flow is in modification than, we will have to reject after 
           response and retry the reject.
        3) Introduce a new state indication bearer is going down. so it is
           easier.
      ---------------------------------------------------------------------*/
      cmd_info_ptr = (ds_flow_manager_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

      DS_3GPP_MSG1_HIGH("Handle Precedence conflict for bearer 0x:%x",
                        cmd_info_ptr->bearer_context_p);

      DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      bearer_context_p = cmd_info_ptr->bearer_context_p;
     if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
      {
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        break;
      }    
      if(bearer_context_p->ds_bearer_context_dyn_p->call_mode == SYS_SYS_MODE_LTE)
      {
        DS_3GPP_MSG0_HIGH("initiating eps Bearer modify to teardown bearer ");
        ds_flow_manager_teardown_bearer(
            cmd_info_ptr->bearer_context_p,
            (uint32 *)
            (cmd_info_ptr->preced_conflict),
             FALSE);            
      }
      else if (cmd_info_ptr->prec_conf_on_same_bearer_modify == FALSE)
      {
        /* This is UMTS case, precedence conflict happend on other bearer, call  
          ds_bearer_cntxt_phys_link_down_cmd_hdlr to tear down the bearer     */

        ds_bearer_cntxt_phys_link_down_cmd_hdlr(
	  	(uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr,
	  	                          NULL);
      }
      else
      {
        DS_3GPP_MSG0_HIGH("bearer mode is UMTS and precedence conflict on same bearer," 
                          "deactivate this bearer in modify IND");
        bearer_context_p->ds_bearer_context_dyn_p->teardown_on_modify_ind = TRUE;
        ps_phys_link_down_ind_ex(&(bearer_context_p
                                ->phys_link), 
                              PS_EIC_NOT_SPECIFIED);   
      }
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    break;

    case DS_CMD_3GPP_FLOW_INVOKE_LOWER_LAYER:
    /*---------------------------------------------------------------------
      Command to invoke the lower layer functions to either request 
      a new bearer or to modify a new bearerer.
    ---------------------------------------------------------------------*/
      qos_cmd_info_ptr = 
        (ds_flow_manager_qos_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

      DS_3GPP_MSG1_HIGH("Invoke Lower layer command for event type %d",
                        qos_cmd_info_ptr->event_type);

      if(qos_cmd_info_ptr->event_type == DS_3GPP_FLOW_BEARER_ALLOC)
      {
        DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        if(ds_flow_manager_ue_bearer_alloc_req(
                qos_cmd_info_ptr->pdn_context_p,
                qos_cmd_info_ptr->flow_context_p,
                (qos_spec_type *)&(qos_cmd_info_ptr->app_qos),
                (tft_type_T *)&(qos_cmd_info_ptr->nw_tft))
           == -1)
        {
          DS_3GPP_MSG0_ERROR("UE bearer alloc req failed");
        }
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      }/* DS_3GPP_FLOW_BEARER_ALLOC */
      else if(qos_cmd_info_ptr->event_type 
              == DS_3GPP_FLOW_BEARER_ALLOC_ABORT)
      {
        DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        if(ds_flow_manager_ue_bearer_alloc_abort_req(
           qos_cmd_info_ptr->flow_context_p) 
           == -1)
        {
          DS_3GPP_MSG0_ERROR("UE bearer alloc req failed");
        }
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      }/* DS_3GPP_FLOW_BEARER_ALLOC */
      else if (qos_cmd_info_ptr->event_type 
               == DS_3GPP_FLOW_BEARER_MODIFY)
      {
        DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        if(ds_flow_manager_ue_bearer_modify_req(
                qos_cmd_info_ptr->flow_context_p,
                (tft_type_T *)&(qos_cmd_info_ptr->nw_tft),
                (qos_spec_type *)&(qos_cmd_info_ptr->app_qos),
               qos_cmd_info_ptr->flow_state) 
           == -1)
        {
          DS_3GPP_MSG0_ERROR("UE bearer modify req failed");
        }
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      }/* DS_3GPP_FLOW_BEARER_MODIFY */
      else
      {
        DS_3GPP_MSG0_ERROR("Unsupported event type");
      }  
      break;

    default:
      DS_3GPP_MSG1_ERROR("Unknown cmd to 3GPP flow Process handler %d",
                         cmd_ptr->hdr.cmd_id);
      break;
  } /* switch( cmd_id ) */
} /*ds_flow_manager_process_ps_cmd*/


/*===========================================================================
FUNCTION DS_FLOW_MANAGER_ADD_FILTERS_TO_FLOW

DESCRIPTION
  Handles adding new filters to flow. This will be called for handling App
  QOS modification request.

PARAMETERS
  modiy_qos                -  App requested modify Qos.
  flow_context_p           -  Flow context being modified.
  flow_manager_p           -  Flow manager associated with the PDN.
  app_bk                   -  Book keeping info about the filters.
  nw_tft                   -  TFT info in network format.

DEPENDENCIES
  None.

RETURN VALUE
boolean                - returns FALSE if comparision fails.
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
static boolean ds_flow_manager_add_filters_to_flow
(
  qos_spec_type               *modify_qos,
  ds_flow_context_s           *flow_context_p,
  ds_flow_manager_s           *flow_manager_p,
  ds_3gpp_app_modify_qos_bk_s *app_bk,
  tft_type_T                  *nw_tft
)
{
  ip_filter_type            *mod_filter = NULL;
  ip_filter_spec_type       *mod_filter_spec = NULL;
  int                       preced= -1;
  int                       loop = 0;
  int                       tft_len=0,fltr_len=0;
  ds_tft_pkt_filter_direction_enum_type filter_direction;
  packet_filter_type_T      *nw_filter = NULL;
  /*-----------------------------------------------------------------------*/
  if((modify_qos==NULL)||(flow_context_p==NULL)||(nw_tft == NULL)||
     (flow_manager_p==NULL)||(app_bk==NULL)||
     (!ds_flow_context_validate_flow(flow_context_p))) 
  {
    DS_3GPP_MSG3_ERROR("input params are NULL. Qos:0x%x FLow:0x%x Fmanager:0x%x",
                          modify_qos,flow_context_p,flow_manager_p);
    return FALSE;
  }
  DS_3GPP_MSG1_HIGH("Add UE filters in the FLow:0x%x",flow_context_p);

  if((app_bk->rx_mod_non_match_id ==0)&&(app_bk->tx_mod_non_match_id == 0))
  {
    DS_3GPP_MSG0_ERROR("add filters called with no new filter info");
    return FALSE;
  }
  // Assign precedence and filter Id for new filters
  if(app_bk->rx_mod_non_match_id != 0)
  {
    mod_filter_spec = &(modify_qos->rx.fltr_template);
    for(loop =0; loop<mod_filter_spec->num_filters; loop++)
    {
      if(((app_bk->rx_mod_non_match_id)&(1 << loop))== (1 << loop))
      {
        mod_filter = &(mod_filter_spec->list_ptr[loop]);
        mod_filter->ipfltr_aux_info.fi_id = 0;
        preced = ds_flow_manager_find_and_set_filter_preced(
                                 flow_manager_p,
                                 DS_MIN_FI_PRECED,
                                 DS_MAX_FI_PRECED);
        if(preced == -1)
        {
          DS_3GPP_MSG0_ERROR("Error allocating precedence");
          return FALSE;
        }
        mod_filter->ipfltr_aux_info.fi_precedence = preced;
       /*---------------------------------------------------------------------
         Translate filter to NW format.
        ---------------------------------------------------------------------*/
        nw_filter = &(nw_tft->filter_list[nw_tft->num_filters]);
        filter_direction = DS_3GPP_TFT_DOWNLINK;
        fltr_len = ds_flow_manager_convert_app_filter_to_nw_filter(
                                                            mod_filter,
                                                            nw_filter,
                                                            filter_direction);
        if(fltr_len > 0)
        {
          tft_len = tft_len + fltr_len;
          nw_tft->num_filters++;
        }
        else
        {
          DS_3GPP_MSG1_ERROR("invalid RX filter params for index no:%d",loop);
          return FALSE;
        }             
      }
    }/* for */
  }/* rx_mod_non_match_id != 0 */

  if(app_bk->tx_mod_non_match_id != 0)
  {
    mod_filter_spec = &(modify_qos->tx.fltr_template);
    for(loop =0; loop<mod_filter_spec->num_filters; loop++)
    {
      if(((app_bk->tx_mod_non_match_id)&(1 << loop))== (1 << loop))
      {
        mod_filter = &(mod_filter_spec->list_ptr[loop]);
        mod_filter->ipfltr_aux_info.fi_id = 0;
        preced =ds_flow_manager_find_and_set_filter_preced(
                                              flow_manager_p,
                                              DS_MIN_FI_PRECED,
                                              DS_MAX_FI_PRECED);
        if(preced == -1)
        {
          DS_3GPP_MSG0_ERROR("Error allocating precedence");
          return FALSE;
        }
        mod_filter->ipfltr_aux_info.fi_precedence = preced; 
        /*---------------------------------------------------------------------
         Translate filter to NW format.
        ---------------------------------------------------------------------*/
        nw_filter = &(nw_tft->filter_list[nw_tft->num_filters]);
        filter_direction = DS_3GPP_TFT_UPLINK;
        fltr_len = ds_flow_manager_convert_app_filter_to_nw_filter(
                                                            mod_filter,
                                                            nw_filter,
                                                            filter_direction);
        if(fltr_len > 0)
        {
          tft_len = tft_len + fltr_len;
          nw_tft->num_filters++;
        }
        else
        {
          DS_3GPP_MSG1_ERROR("invalid TX filter params for index no:%d",loop);
          return FALSE;
        }              
      }
    }/* for */
  }/* tx_mod_non_match_id != 0 */
  app_bk->network_op |= DS_3GPP_NW_FLTR_ADD_OP;
  /*---------------------------------------------------------------------
     Update TFT Length,other params
  ---------------------------------------------------------------------*/
  nw_tft->valid = TRUE;
  nw_tft->tft_total_length = tft_len + 1;
  nw_tft->operation_code =(byte) DS_3GPP_TFT_OPCODE_CREATE;
  nw_tft->e_bit = DS_FLOW_E_BIT_DISABLE;
  return TRUE;
}/* ds_flow_manager_add_filters_to_flow*/

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_REPLACE_FILTERS_IN_FLOW

DESCRIPTION
  Handles replacing filters in a flow. This will be called for handling App
  QOS modification request.

PARAMETERS
  modiy_qos                -  App requested modify Qos.
  flow_context_p           -  Flow context being modified.
  current_qos              -  Qos currently allocated to App.
  app_bk                   -  Book keeping info about the filters.
  nw_tft                   -  TFT info in network format.

DEPENDENCIES
  None.

RETURN VALUE
boolean                - returns FALSE if comparision fails.
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
static boolean ds_flow_manager_replace_filters_in_flow
(
  qos_spec_type               *modify_qos,
  ds_flow_context_s           *flow_context_p,
  qos_spec_type               *current_qos,
  ds_3gpp_app_modify_qos_bk_s *app_bk,
  tft_type_T                  *nw_tft
)
{
  ip_filter_type            *mod_filter = NULL;
  ip_filter_type            *curr_filter = NULL;
  ip_filter_spec_type       *mod_rx_filter_spec = NULL;
  ip_filter_spec_type       *mod_tx_filter_spec = NULL;
  ip_filter_spec_type       *curr_filter_spec = NULL;
  uint16                     identifier = 0;
  uint32                       loop = 0,iloop=0, tloop=0;
  int                       tft_len=0,fltr_len=0;
  ds_tft_pkt_filter_direction_enum_type filter_direction;
  packet_filter_type_T      *nw_filter = NULL;

  /*-----------------------------------------------------------------------*/
  if((modify_qos==NULL)||(flow_context_p==NULL)||(current_qos==NULL)||
     (!ds_flow_context_validate_flow(flow_context_p))) 
  {
    DS_3GPP_MSG3_ERROR("input params are NULL. Mod Qos:0x%x FLow:0x%x Curr Qos:0x%x",
                          modify_qos,flow_context_p,current_qos);
    return FALSE;
  }

  DS_3GPP_MSG1_HIGH("Replace UE filters in the FLow:0x%x",flow_context_p);
  if((nw_tft==NULL)||(app_bk==NULL)) 
  {
    DS_3GPP_MSG2_ERROR("input params are NULL.nw tft:0x%x 0x%x",nw_tft,app_bk);
    return FALSE;
  }
  mod_rx_filter_spec = &(modify_qos->rx.fltr_template);
  mod_tx_filter_spec = &(modify_qos->tx.fltr_template);
  curr_filter_spec =  &(current_qos->rx.fltr_template);

  for(loop =0; loop<curr_filter_spec->num_filters; loop++)
  {
    curr_filter = &(curr_filter_spec->list_ptr[loop]);
    identifier = (curr_filter->ipfltr_aux_info.fi_id & (0xF));
  
  if(ds_3gpp_tft_is_filter_identifier_requested(identifier, 
                            &app_bk->match_filter_id)== FALSE)
    {
      if(app_bk->rx_mod_non_match_id != 0)
      {
        for(iloop=0;iloop<mod_rx_filter_spec->num_filters;iloop++)
        {
          if((app_bk->rx_mod_non_match_id & (1 <<iloop)) == (1 <<iloop))
          {
            tloop = (1 << iloop);
            app_bk->rx_mod_non_match_id &= ~(tloop);
            mod_filter = &(mod_rx_filter_spec->list_ptr[iloop]);
            mod_filter->ipfltr_aux_info.fi_id = 
                             curr_filter->ipfltr_aux_info.fi_id;
            mod_filter->ipfltr_aux_info.fi_precedence =
                          curr_filter->ipfltr_aux_info.fi_precedence; 
            if(app_bk->rx_mod_non_match_id == 0)
            {
              iloop = 0;
            }
            ds_3gpp_tft_set_filter_identifier_bit(identifier,
                                          &app_bk->replaced_filter_id);
            /*---------------------------------------------------------------
              Translate filter to NW format.
            ---------------------------------------------------------------*/
            nw_filter = &(nw_tft->filter_list[nw_tft->num_filters]);
            filter_direction = DS_3GPP_TFT_DOWNLINK;
            fltr_len = ds_flow_manager_convert_app_filter_to_nw_filter(
                                                            mod_filter,
                                                            nw_filter,
                                                            filter_direction);
            if(fltr_len > 0)
            {
              tft_len = tft_len + fltr_len;
              nw_tft->num_filters++;
            }
            else
            {
              DS_3GPP_MSG1_ERROR("invalid RX filter params for index no:%d",iloop);
              return FALSE;
            }      
          }/* if filter to be replaced*/
        } /* for iloop */
      }/* rx_mod_non_match_id != 0*/
      else if(app_bk->tx_mod_non_match_id != 0)
      {
        for(iloop=0;iloop<mod_tx_filter_spec->num_filters;iloop++)
        {
          if((app_bk->tx_mod_non_match_id & (1 <<iloop)) == (1 <<iloop))
          {
            tloop = (1 << iloop);
            app_bk->tx_mod_non_match_id &= ~(tloop);
            mod_filter = &(mod_tx_filter_spec->list_ptr[iloop]);
            mod_filter->ipfltr_aux_info.fi_id = 
                             curr_filter->ipfltr_aux_info.fi_id;
            mod_filter->ipfltr_aux_info.fi_precedence =
                          curr_filter->ipfltr_aux_info.fi_precedence; 
            
            if(app_bk->rx_mod_non_match_id == 0)
            {
              iloop = 0;
            }
            ds_3gpp_tft_set_filter_identifier_bit(identifier,
                                          &app_bk->replaced_filter_id);
            /*---------------------------------------------------------------
              Translate filter to NW format.
            ---------------------------------------------------------------*/
            nw_filter = &(nw_tft->filter_list[nw_tft->num_filters]);
            filter_direction = DS_3GPP_TFT_UPLINK;
            fltr_len = ds_flow_manager_convert_app_filter_to_nw_filter(
                                                            mod_filter,
                                                            nw_filter,
                                                            filter_direction);
            if(fltr_len > 0)
            {
              tft_len = tft_len + fltr_len;
              nw_tft->num_filters++;
            }
            else
            {
              DS_3GPP_MSG1_ERROR("invalid TX filter params for index no:%d",iloop);
              return FALSE;
            }      
          }/* if filter to be replaced*/
        }/* for iloop */
      }/* tx_mod_non_match_id != 0 */
      else
      {
        DS_3GPP_MSG0_ERROR("Unexpected parameters in Replace filters");
        return FALSE;
      }
    }/* if filter to be replaced*/
  }/* for */

  curr_filter_spec =  &(current_qos->tx.fltr_template);
  for(loop =0; loop<curr_filter_spec->num_filters; loop++)
  {
    curr_filter = &(curr_filter_spec->list_ptr[loop]);
    identifier = (curr_filter->ipfltr_aux_info.fi_id & (0xF));
    if(ds_3gpp_tft_is_filter_identifier_requested(identifier, 
                            &app_bk->match_filter_id)== FALSE)
    {
      if(app_bk->rx_mod_non_match_id != 0)
      {
        for(iloop=0;iloop<mod_rx_filter_spec->num_filters;iloop++)
        {
          if((app_bk->rx_mod_non_match_id & (1 <<iloop)) == (1 <<iloop))
          {
            tloop = (1 << iloop);
            app_bk->rx_mod_non_match_id &= ~(tloop);
            mod_filter = &(mod_rx_filter_spec->list_ptr[iloop]);
            mod_filter->ipfltr_aux_info.fi_id = 
                             curr_filter->ipfltr_aux_info.fi_id;
            mod_filter->ipfltr_aux_info.fi_precedence =
                          curr_filter->ipfltr_aux_info.fi_precedence; 
            if(app_bk->rx_mod_non_match_id == 0)
            {
              iloop = 0;
            }
            ds_3gpp_tft_set_filter_identifier_bit(identifier,
                                          &app_bk->replaced_filter_id);
            /*---------------------------------------------------------------
              Translate filter to NW format.
            ---------------------------------------------------------------*/
            nw_filter = &(nw_tft->filter_list[nw_tft->num_filters]);
            filter_direction = DS_3GPP_TFT_DOWNLINK;
            fltr_len = ds_flow_manager_convert_app_filter_to_nw_filter(
                                                            mod_filter,
                                                            nw_filter,
                                                            filter_direction);
            if(fltr_len > 0)
            {
              tft_len = tft_len + fltr_len;
              nw_tft->num_filters++;
            }
            else
            {
              DS_3GPP_MSG1_ERROR("invalid RX filter params for index no:%d",iloop);
              return FALSE;
            }      
          }/* if filter to be replaced*/
        }/* for iloop */
      }/* rx_mod_non_match_id != 0*/
      else if(app_bk->tx_mod_non_match_id != 0)
      {
        for(iloop=0;iloop<mod_tx_filter_spec->num_filters;iloop++)
        {
          if((app_bk->tx_mod_non_match_id & (1 <<iloop)) == (1 <<iloop))
          {
            tloop = (1 << iloop);
            app_bk->tx_mod_non_match_id &= ~(tloop);
            mod_filter = &(mod_tx_filter_spec->list_ptr[iloop]);
            mod_filter->ipfltr_aux_info.fi_id = 
                             curr_filter->ipfltr_aux_info.fi_id;
            mod_filter->ipfltr_aux_info.fi_precedence =
                          curr_filter->ipfltr_aux_info.fi_precedence; 
            
            if(app_bk->rx_mod_non_match_id == 0)
            {
              iloop = 0;
            }
            ds_3gpp_tft_set_filter_identifier_bit(identifier,
                                          &app_bk->replaced_filter_id);
            /*---------------------------------------------------------------
              Translate filter to NW format.
            ---------------------------------------------------------------*/
            nw_filter = &(nw_tft->filter_list[nw_tft->num_filters]);
            filter_direction = DS_3GPP_TFT_UPLINK;
            fltr_len = ds_flow_manager_convert_app_filter_to_nw_filter(
                                                            mod_filter,
                                                            nw_filter,
                                                            filter_direction);
            if(fltr_len > 0)
            {
              tft_len = tft_len + fltr_len;
              nw_tft->num_filters++;
            }
            else
            {
              DS_3GPP_MSG1_ERROR("invalid TX filter params for index no:%d",iloop);
              return FALSE;
            }      
          }/* if filter to be replaced*/
        }/* for iloop */
      }/* tx_mod_non_match_id != 0 */
      else
      {
        DS_3GPP_MSG0_ERROR("Unexpected parameters in Replace filters");
        return FALSE;
      }
    }/* if filter to be replaced*/
  }/* for */
  app_bk->network_op |= DS_3GPP_NW_FLTR_REPLACE_OP;
  /*---------------------------------------------------------------------
    Update TFT Length,other params
  ---------------------------------------------------------------------*/
  nw_tft->valid = TRUE;
  nw_tft->tft_total_length = tft_len + 1;
  nw_tft->operation_code =(byte) DS_3GPP_TFT_OPCODE_REPLACE;
  nw_tft->e_bit = DS_FLOW_E_BIT_DISABLE; 
  return TRUE;
}/* ds_flow_manager_replace_filters_in_flow*/

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_DELETE_FILTERS_IN_FLOW

DESCRIPTION
  Handles deleting filters to flow. This will be called for handling App
  QOS modification request. 

PARAMETERS
  flow_context_p           -  Flow context being modified.
  current_qos              -  Qos currently allocated to App.
  app_bk                   -  Book keeping info about the filters.
  nw_tft                   -  TFT info in network format.

DEPENDENCIES
  None.

RETURN VALUE
boolean                - returns FALSE if comparision fails.
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
static boolean ds_flow_manager_delete_filters_in_flow
(
  ds_flow_context_s           *flow_context_p,
  qos_spec_type               *current_qos,
  ds_3gpp_app_modify_qos_bk_s *app_bk,
  tft_type_T                  *nw_tft
)
{
  ip_filter_type            *curr_filter = NULL;
  ip_filter_spec_type       *curr_filter_spec = NULL;
  uint16                     identifier =0;
  int                       loop = 0;
  /*-----------------------------------------------------------------------*/
  if((current_qos==NULL)||(flow_context_p==NULL)||
  (nw_tft==NULL)||(app_bk==NULL)||(!ds_flow_context_validate_flow(flow_context_p))) 
  {
    DS_3GPP_MSG3_ERROR("input params are NULL. Qos:0x%x FLow:0x%x Nwtft:0x%x",
                          current_qos,flow_context_p,nw_tft);
    return FALSE;
  }
  DS_3GPP_MSG1_HIGH("Delete UE filters in the FLow:0x%x",flow_context_p);
  /* RX */
  curr_filter_spec =  &(current_qos->rx.fltr_template);
  for(loop =0; loop<curr_filter_spec->num_filters; loop++)
  {
    curr_filter = &(curr_filter_spec->list_ptr[loop]);
    identifier = (curr_filter->ipfltr_aux_info.fi_id & (0xF));
    if(ds_3gpp_tft_is_filter_identifier_requested(identifier, 
                            &app_bk->match_filter_id)== FALSE)
    {
      /*---------------------------------------------------------------
        Translate filter to NW format.
      ---------------------------------------------------------------*/
      nw_tft->filter_list[nw_tft->num_filters].identifier =(byte)identifier;
      nw_tft->num_filters++;      
    } 
  }

  /* TX */
  curr_filter_spec =  &(current_qos->tx.fltr_template);
  for(loop =0; loop<curr_filter_spec->num_filters; loop++)
  {
    curr_filter = &(curr_filter_spec->list_ptr[loop]);
    identifier = (curr_filter->ipfltr_aux_info.fi_id & (0xF));
    if(ds_3gpp_tft_is_filter_identifier_requested(identifier, 
                            &app_bk->match_filter_id)==FALSE)
    {
      /*---------------------------------------------------------------
        Translate filter to NW format.
      ---------------------------------------------------------------*/
      nw_tft->filter_list[nw_tft->num_filters].identifier =(byte)identifier;
      nw_tft->num_filters++;      
    } 
  }  
  if((app_bk->rx_match_cnt + app_bk->tx_match_cnt + nw_tft->num_filters) !=
            (app_bk->rx_curr_fltr_cnt + app_bk->tx_curr_fltr_cnt))
  {
    DS_3GPP_MSG1_ERROR("data filter count does not match.del fltrcnt %d",
                                        nw_tft->num_filters);
    return FALSE;
  }
  app_bk->network_op |= DS_3GPP_NW_FLTR_DEL_OP;
 /*---------------------------------------------------------------------
    Update TFT Length,other params
  ---------------------------------------------------------------------*/
  nw_tft->valid = TRUE;
  nw_tft->tft_total_length = 1 + nw_tft->num_filters;
  nw_tft->operation_code =(byte) DS_3GPP_TFT_OPCODE_DEL_FILTER;
  nw_tft->e_bit = DS_FLOW_E_BIT_DISABLE; 

  return TRUE; 
}/*  ds_flow_manager_delete_filters_in_flow*/

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_HANDLE_NO_FILTER_OPERATION_IN_FLOW

DESCRIPTION
  Handles no filter operation in the flow. This will be called while
  handling App QOS modification request. 

PARAMETERS
  flow_context_p           -  Flow context being modified.
  current_qos              -  Qos currently allocated to App.
  app_bk                   -  Book keeping info about the filters.
  nw_tft                   -  TFT info in network format.

DEPENDENCIES
  None.

RETURN VALUE
boolean                - returns FALSE if comparision fails.
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
static boolean ds_flow_manager_handle_no_filter_operation_in_flow
(
  ds_flow_context_s           *flow_context_p,
  qos_spec_type               *current_qos,
  ds_3gpp_app_modify_qos_bk_s *app_bk,
  tft_type_T                  *nw_tft
)
{
  ip_filter_type            *curr_filter = NULL;
  ip_filter_spec_type       *curr_filter_spec = NULL;
  uint16                     identifier =0;
  int                       loop = 0;
  int                       cnt = 0;
  int                       p_cnt = 0;
  /*-----------------------------------------------------------------------*/
  if((current_qos==NULL)||(flow_context_p==NULL)||
  (nw_tft==NULL)||(app_bk==NULL)||(!ds_flow_context_validate_flow(flow_context_p))) 
  {
    DS_3GPP_MSG3_ERROR("input params are NULL. Qos:0x%x FLow:0x%x Nwtft:0x%x",
                          current_qos,flow_context_p,nw_tft);
    return FALSE;
  }

  /*---------------------------------------------------------------
    Initialize the Parameter List.
  ---------------------------------------------------------------*/
  nw_tft->num_filters = 0;
  nw_tft->param_list[cnt].identifier =
                         (byte)DS_3GPP_TFT_PACKET_FILTER_IDENTIFIER;
  nw_tft->param_list[cnt].content_length = 0;

  /*---------------------------------------------------------------
    Loop through the RX filters.
  ---------------------------------------------------------------*/
  curr_filter_spec =  &(current_qos->rx.fltr_template);
  for(loop =0; 
      (loop<curr_filter_spec->num_filters) && (p_cnt < MAX_FILTER_CONTENT_LENGTH); 
      loop++)
  {
    curr_filter = &(curr_filter_spec->list_ptr[loop]);
    identifier = (curr_filter->ipfltr_aux_info.fi_id & (0xF));
    if(ds_3gpp_tft_is_filter_identifier_requested(identifier, 
                            &app_bk->match_filter_id)== TRUE)
    {
      /*---------------------------------------------------------------
        Translate filter to NW format.
      ---------------------------------------------------------------*/
      nw_tft->param_list[cnt].filter_contents[p_cnt] =(byte)identifier;
      p_cnt++;        
    } 
  }

  /*---------------------------------------------------------------
    Loop through the TX filters.
  ---------------------------------------------------------------*/
  curr_filter_spec =  &(current_qos->tx.fltr_template);
  for(loop =0; 
      (loop<curr_filter_spec->num_filters) && p_cnt < MAX_FILTER_CONTENT_LENGTH;
      loop++)
  {
    curr_filter = &(curr_filter_spec->list_ptr[loop]);
    identifier = (curr_filter->ipfltr_aux_info.fi_id & (0xF));
    if(ds_3gpp_tft_is_filter_identifier_requested(identifier, 
                            &app_bk->match_filter_id)==TRUE)
    {
      /*---------------------------------------------------------------
        Translate filter to NW format.
      ---------------------------------------------------------------*/
      nw_tft->param_list[cnt].filter_contents[p_cnt] =(byte)identifier;
      p_cnt++;     
    } 
  }  

  if(p_cnt == 0)
  {
    DS_3GPP_MSG0_ERROR("no filters affected by data rate change");
    return FALSE; 
  }
  app_bk->network_op |= DS_3GPP_NW_DATA_RATE_OP;
  /*---------------------------------------------------------------------
    Update TFT Length,other params
  ---------------------------------------------------------------------*/
  nw_tft->valid = TRUE;
  // +2 to include the param_list content_length and param_list identifier
  nw_tft->tft_total_length = 1 + p_cnt+2;
  nw_tft->operation_code =(byte) DS_3GPP_TFT_OPCODE_NO_OP;
  nw_tft->e_bit = DS_FLOW_E_BIT_ENABLE;
  nw_tft->param_list[cnt].content_length = p_cnt;

  return TRUE;
}/* ds_flow_manager_handle_no_filter_operation_in_flow */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_COMPARE_MODIFY_QOS

DESCRIPTION
  Compare the App requested modify Qos to current Qos

PARAMETERS
  modiy_qos                -  App requested modify Qos.
  current_qos              -  Current App Qos.
  flow_context_p           -  Flow context being modified.
  app_bk                   -  App book keeping info.
  tft                      -  TFT in network format.

DEPENDENCIES
  None.

RETURN VALUE
  boolean                - returns FALSE if comparision fails.
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
boolean ds_flow_manager_compare_modify_qos
(
  qos_spec_type                *modify_qos,
  qos_spec_type                *current_qos,
  ds_flow_context_s            *flow_context_p,
  ds_3gpp_app_modify_qos_bk_s  *app_bk,
  tft_type_T                   *nw_tft
)
{
  uint16 loop =0, outer_loop =0;
  ip_filter_type            *curr_filter = NULL;
  ip_filter_type            *mod_filter = NULL;
  ip_filter_spec_type       *curr_filter_spec = NULL;
  ip_filter_spec_type       *mod_filter_spec = NULL;
  ds_3gpp_nw_filter_op rx_op = DS_3GPP_NW_FLTR_INVALID_OP;
  ds_3gpp_nw_filter_op tx_op = DS_3GPP_NW_FLTR_INVALID_OP;
  ds_flow_manager_s         *flow_manager_p = NULL;
  boolean                  filter_found = FALSE; 
  /*-----------------------------------------------------------------------*/

  if((current_qos==NULL)||(modify_qos == NULL)|| (nw_tft == NULL)||
     (flow_context_p == NULL)||(app_bk==NULL)||
     (!ds_flow_context_validate_flow(flow_context_p)))
  {
    DS_3GPP_MSG3_ERROR("Input params are null.curr_qos 0x%x, mod_qos 0x%x",
                current_qos,modify_qos, 0);
    return FALSE;
  }
  /*-------------------------------------------------------------------
    Compare each Modified filter with all the current filter
  -------------------------------------------------------------------*/
  if(! (app_bk->app_op & 
                    (DS_3GPP_APP_RX_FLTR_OP | DS_3GPP_APP_TX_FLTR_OP)))
  {
    DS_3GPP_MSG0_HIGH(" App modifying only data rate");
    if(app_bk->app_op & DS_3GPP_APP_RX_DATA_RATE_OP)
    {
      /*-------------------------------------------------------------------
        Extract Rx filter ID
      -------------------------------------------------------------------*/
      curr_filter_spec = &(current_qos->rx.fltr_template);
      for(loop =0; loop < curr_filter_spec->num_filters; loop++)
      {
        curr_filter = &(curr_filter_spec->list_ptr[loop]);
        ds_3gpp_tft_set_filter_identifier_bit(
                       ((curr_filter->ipfltr_aux_info.fi_id)&(0xF)),
                                  &(app_bk->match_filter_id));
      }
    }
 
    if(app_bk->app_op & DS_3GPP_APP_TX_DATA_RATE_OP)
    {
      /*-------------------------------------------------------------------
        Extract Tx filter ID
      -------------------------------------------------------------------*/
      curr_filter_spec = &(current_qos->tx.fltr_template);
      for(loop =0; loop < curr_filter_spec->num_filters; loop++)
      {
        curr_filter = &(curr_filter_spec->list_ptr[loop]);
        ds_3gpp_tft_set_filter_identifier_bit(
                       ((curr_filter->ipfltr_aux_info.fi_id)&(0xF)),
                                  &(app_bk->match_filter_id));
      }
    }
    return(ds_flow_manager_handle_no_filter_operation_in_flow(
                                                 flow_context_p,
                                                 current_qos,
                                                 app_bk,
                                                 nw_tft));
  
  }/* only data rate case */

  if(!(app_bk->app_op & 
        (DS_3GPP_APP_RX_DATA_RATE_OP | DS_3GPP_APP_TX_DATA_RATE_OP)))
  {
    DS_3GPP_MSG0_HIGH("App modifying only TFT"); 
  }
  else
  {
    DS_3GPP_MSG0_HIGH("App modifying TFT and data rate"); 
  }

  /*-------------------------------------------------------------------
     Verify RX
  -------------------------------------------------------------------*/
  // modify code to look based on the flag value only
  curr_filter_spec = &(current_qos->rx.fltr_template);
  mod_filter_spec = &(modify_qos->rx.fltr_template);
  app_bk->rx_curr_fltr_cnt = curr_filter_spec->num_filters;
  for(outer_loop =0; outer_loop<mod_filter_spec->num_filters; outer_loop++)
  {
    mod_filter = &(mod_filter_spec->list_ptr[outer_loop]);
    filter_found = FALSE;
    /*-------------------------------------------------------------------
      Compare each Modified filter with all the current filter
    -------------------------------------------------------------------*/      
    for(loop =0; loop < curr_filter_spec->num_filters; loop++)
    {
      curr_filter = &(curr_filter_spec->list_ptr[loop]);
      /*-------------------------------------------------------------------
       Compare App filter with NW filter not in comparision bit mask
      -------------------------------------------------------------------*/
      if(ds_flow_context_compare_filters(curr_filter,mod_filter) == TRUE)
      {
        /*-------------------------------------------------------------------
            Update bit mask to avoid comparing this NW filter.
            Also update filter identifier and filter precedence.
        -------------------------------------------------------------------*/
        mod_filter->ipfltr_aux_info.fi_id = 
                             curr_filter->ipfltr_aux_info.fi_id;
        mod_filter->ipfltr_aux_info.fi_precedence =
                          curr_filter->ipfltr_aux_info.fi_precedence;
        filter_found = TRUE;
        app_bk->rx_match_cnt++;
        ds_3gpp_tft_set_filter_identifier_bit(
                                 ((mod_filter->ipfltr_aux_info.fi_id)&(0xF)),
                                  &(app_bk->match_filter_id));
      } /* if match found */
    }/* loop*/
    if(filter_found == FALSE) 
    {
      app_bk->rx_mod_non_match_id |= 1 <<outer_loop;      
      app_bk->rx_non_match_cnt++;
    }
  } /* outer loop*/
  if((app_bk->rx_non_match_cnt > 0)&&
                    (app_bk->rx_match_cnt == curr_filter_spec->num_filters))
  {
    DS_3GPP_MSG1_HIGH("Adding %d RX filters",app_bk->rx_non_match_cnt);
    rx_op = DS_3GPP_NW_FLTR_ADD_OP;
  }
  else if((app_bk->rx_non_match_cnt==0)&&
                      (app_bk->rx_match_cnt < curr_filter_spec->num_filters))
  {
    DS_3GPP_MSG1_HIGH("deleting %d RX filters",
                      curr_filter_spec->num_filters - app_bk->rx_match_cnt);
    rx_op = DS_3GPP_NW_FLTR_DEL_OP;
  }
  else if((app_bk->rx_non_match_cnt==0)&&(app_bk->rx_match_cnt == curr_filter_spec->num_filters))
  {
    DS_3GPP_MSG0_HIGH("RX filter spec unchanged");
    rx_op = DS_3GPP_NW_FLTR_NO_OP;
  }
  else
  {
    DS_3GPP_MSG2_HIGH("Replacing RX filters: %d match :%d replace",
                             app_bk->rx_match_cnt,app_bk->rx_non_match_cnt);
    rx_op = DS_3GPP_NW_FLTR_REPLACE_OP;
  }
  
  /*-------------------------------------------------------------------
     Verify TX
  -------------------------------------------------------------------*/
  curr_filter_spec = &(current_qos->tx.fltr_template);
  mod_filter_spec = &(modify_qos->tx.fltr_template);
  app_bk->tx_curr_fltr_cnt = curr_filter_spec->num_filters;
  for(outer_loop =0; outer_loop<mod_filter_spec->num_filters; outer_loop++)
  {
    mod_filter = &(mod_filter_spec->list_ptr[outer_loop]);
    filter_found = FALSE;
    /*-------------------------------------------------------------------
      Compare each Modified filter with all the current filter
    -------------------------------------------------------------------*/      
    for(loop =0; loop < curr_filter_spec->num_filters; loop++)
    {
      curr_filter = &(curr_filter_spec->list_ptr[loop]);
      /*-------------------------------------------------------------------
       Compare App filter with NW filter not in comparision bit mask
      -------------------------------------------------------------------*/
      if(ds_flow_context_compare_filters(curr_filter,mod_filter) == TRUE)
      {
        /*-------------------------------------------------------------------
            Update bit mask to avoid comparing this NW filter.
            Also update filter identifier and filter precedence.
        -------------------------------------------------------------------*/
        mod_filter->ipfltr_aux_info.fi_id = 
                             curr_filter->ipfltr_aux_info.fi_id;
        mod_filter->ipfltr_aux_info.fi_precedence =
                          curr_filter->ipfltr_aux_info.fi_precedence;
        filter_found = TRUE;
        app_bk->tx_match_cnt++;
        ds_3gpp_tft_set_filter_identifier_bit(
                                 ((mod_filter->ipfltr_aux_info.fi_id)&(0xF)),
                                  &(app_bk->match_filter_id));
      } /* if match found */
    }/* loop*/
    if(filter_found == FALSE) 
    {
      app_bk->tx_mod_non_match_id |= 1 <<outer_loop;      
      app_bk->tx_non_match_cnt++;
    }
  } /* outer loop*/
  if((app_bk->tx_non_match_cnt>0)&&
                 (app_bk->tx_match_cnt==curr_filter_spec->num_filters))
  {
    DS_3GPP_MSG1_HIGH("Adding %d TX filters",app_bk->tx_non_match_cnt);
    tx_op = DS_3GPP_NW_FLTR_ADD_OP;
  }
  else if((app_bk->tx_non_match_cnt==0)
              &&(app_bk->tx_match_cnt < curr_filter_spec->num_filters))
  {
    DS_3GPP_MSG1_HIGH("deleting %d TX filters",
                       curr_filter_spec->num_filters - app_bk->tx_match_cnt);
    tx_op = DS_3GPP_NW_FLTR_DEL_OP;
  }
  else if((app_bk->tx_non_match_cnt==0)&&
             (app_bk->tx_match_cnt == curr_filter_spec->num_filters))
  {
    DS_3GPP_MSG0_HIGH("TX filter spec unchanged");
    tx_op = DS_3GPP_NW_FLTR_NO_OP;
  }
  else
  {
    DS_3GPP_MSG2_HIGH("Replacing TX filters: %d match :%d replace",
                            app_bk->tx_match_cnt,app_bk->tx_non_match_cnt);
    tx_op = DS_3GPP_NW_FLTR_REPLACE_OP;
  }
  
  /* Retriever Flow manager */
  if(flow_context_p->ds_flow_context_dyn_p->ds_iface_ptr != NULL)
  {
    flow_manager_p = ds_flow_manager_find_flow_manager((ds_pdn_context_s *)
          flow_context_p->ds_flow_context_dyn_p->ds_iface_ptr->client_data_ptr);
    if(flow_manager_p == NULL)
    {
      DS_3GPP_MSG0_ERROR("could not retrieve the flow manager");
      return FALSE;
    }
  }
  else
  {
    DS_3GPP_MSG0_ERROR("flow context not associated with 3gpp iface");
    return FALSE;
  }

  switch(rx_op)
  {
    case DS_3GPP_NW_FLTR_ADD_OP:
      if((tx_op == DS_3GPP_NW_FLTR_ADD_OP)||(tx_op == DS_3GPP_NW_FLTR_NO_OP))
      {
        DS_3GPP_MSG0_HIGH("NW FLTR OP is add filters");
        return(ds_flow_manager_add_filters_to_flow(
                                              modify_qos,
                                              flow_context_p,
                                              flow_manager_p,
                                              app_bk,
                                              nw_tft));
      }
      else
      {
        DS_3GPP_MSG0_HIGH("NW FLTR OP is replace or 2 step operation");
        if((app_bk->rx_match_cnt+ app_bk->rx_non_match_cnt+ 
                         app_bk->tx_match_cnt+ app_bk->tx_non_match_cnt)
             ==(app_bk->rx_curr_fltr_cnt + app_bk->tx_curr_fltr_cnt))
        {
          return(ds_flow_manager_replace_filters_in_flow(
                                                    modify_qos,
                                                    flow_context_p,
                                                    current_qos,
                                                    app_bk,
                                                    nw_tft));
        }
        else
        {
          DS_3GPP_MSG0_HIGH("The operation is not currently supported");
          app_bk->network_op = DS_3GPP_NW_FLTR_UNSUPP_OP;
        }
      }
    break;
    case DS_3GPP_NW_FLTR_DEL_OP:
      if((tx_op == DS_3GPP_NW_FLTR_DEL_OP) || (tx_op == DS_3GPP_NW_FLTR_NO_OP))
      {
        DS_3GPP_MSG0_HIGH("NW FLTR OP is DEL filters");
        return(ds_flow_manager_delete_filters_in_flow(
                                                 flow_context_p,
                                                 current_qos,
                                                 app_bk,
                                                 nw_tft));
      }
      else
      {
        DS_3GPP_MSG0_HIGH("NW FLTR OP is replace or 2 step operation");
        if((app_bk->rx_match_cnt+ app_bk->rx_non_match_cnt+ 
                         app_bk->tx_match_cnt+ app_bk->tx_non_match_cnt)
             ==(app_bk->rx_curr_fltr_cnt + app_bk->tx_curr_fltr_cnt))
        {
          return(ds_flow_manager_replace_filters_in_flow(
                                                    modify_qos,
                                                    flow_context_p,
                                                    current_qos,
                                                    app_bk,
                                                    nw_tft));
        }
        else
        {
          DS_3GPP_MSG0_HIGH("The operation is not currently supported");
          app_bk->network_op = DS_3GPP_NW_FLTR_UNSUPP_OP;
        }
      }
    break;
    case DS_3GPP_NW_FLTR_NO_OP:
      if(tx_op == DS_3GPP_NW_FLTR_ADD_OP)
      {
        DS_3GPP_MSG0_HIGH("NW FLTR OP is ADD filters");
        return(ds_flow_manager_add_filters_to_flow(modify_qos,
                                              flow_context_p,
                                              flow_manager_p,
                                              app_bk,
                                              nw_tft));
      }
      else if(tx_op == DS_3GPP_NW_FLTR_DEL_OP)
      {
        DS_3GPP_MSG0_HIGH("NW FLTR OP is DEL filters");
        return(ds_flow_manager_delete_filters_in_flow(
                                                 flow_context_p,
                                                 current_qos,
                                                 app_bk,
                                                 nw_tft));
      }
      else
      {
        DS_3GPP_MSG0_HIGH("NW FLTR OP is replace or 2 step operation");
        if((app_bk->rx_match_cnt+ app_bk->rx_non_match_cnt+ 
                         app_bk->tx_match_cnt+ app_bk->tx_non_match_cnt)
             ==(app_bk->rx_curr_fltr_cnt + app_bk->tx_curr_fltr_cnt))
        {
          return(ds_flow_manager_replace_filters_in_flow(
                                                    modify_qos,
                                                    flow_context_p,
                                                    current_qos,
                                                    app_bk,
                                                    nw_tft));
        }
        else
        {
          DS_3GPP_MSG0_HIGH("The operation is not currently supported");
          app_bk->network_op = DS_3GPP_NW_FLTR_UNSUPP_OP;
        }
      }
    break;
    default:
      DS_3GPP_MSG0_HIGH("NW FLTR is replace or 2 step operation");
      if((app_bk->rx_match_cnt+ app_bk->rx_non_match_cnt+ 
                         app_bk->tx_match_cnt+ app_bk->tx_non_match_cnt)
             ==(app_bk->rx_curr_fltr_cnt + app_bk->tx_curr_fltr_cnt))
      {
        return(ds_flow_manager_replace_filters_in_flow(
                                                  modify_qos,
                                                  flow_context_p,
                                                  current_qos,
                                                  app_bk,
                                                  nw_tft));
      }
      else
      {
        DS_3GPP_MSG0_HIGH("The operation is not currently supported");
        app_bk->network_op = DS_3GPP_NW_FLTR_UNSUPP_OP;
      }
    break;
  }/* switch */
  return TRUE;
}/* ds_flow_manager_compare_modify_qos */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_UPDATE_PS_WITH_NETWORK_RESPONSE

DESCRIPTION
  Update App with network response to UE bearer flow modification request.

PARAMETERS
  pdn_context_p            -  Pointer to PDN context.
  bearer_context_p         -  Pointer to Bearer context.
  flow_context_p           -  Pointer to Flow context.
  is_accepted              -  Indicates if the request was accepted.

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
boolean ds_flow_manager_update_ps_with_network_response
(
  ds_pdn_context_s         *pdn_context_p,
  ds_bearer_context_s      *bearer_context_p,
  ds_flow_context_s        *flow_context_p,
  boolean                  is_accepted
)
{
  ps_extended_info_code_enum_type ps_eic = PS_EIC_NOT_SPECIFIED;
  int16                           ps_errno = 0;
  ds_bearer_flow_manager_s       *bflow_manager_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("Updating PS with NW repsosne for UE request");
  if(pdn_context_p==NULL ||bearer_context_p==NULL||flow_context_p==NULL||
     (!ds_flow_context_validate_flow(flow_context_p)))
  {
    DS_3GPP_MSG2_ERROR("PDN:0x%x:Bearer:0x%x",pdn_context_p,bearer_context_p);
    return FALSE;
  }

  if(is_accepted)
  {
    DS_3GPP_MSG1_HIGH("NW granted UE request for flow:0x%x",flow_context_p);
    /*-------------------------------------------------------------------
       If this is a flow modify request.
    -------------------------------------------------------------------*/
    if( (flow_context_p->ds_flow_context_dyn_p->state == 
         DS_FLOW_CONTEXT_STATE_PEND_ADD_RSP)||
        (flow_context_p->ds_flow_context_dyn_p->state == 
         DS_FLOW_CONTEXT_STATE_PEND_DEL_RSP)||
        (flow_context_p->ds_flow_context_dyn_p->state == 
         DS_FLOW_CONTEXT_STATE_PEND_REPLACE_RSP)||
        (flow_context_p->ds_flow_context_dyn_p->state == 
         DS_FLOW_CONTEXT_STATE_PEND_NO_OP_RSP))
    {
      /*-------------------------------------------------------------------
        Confirm modification to PS frame work.
      -------------------------------------------------------------------*/
      ps_eic = PS_EIC_NOT_SPECIFIED;
      if (ds_flow_context_ps_flow_modify_result(
                     flow_context_p,is_accepted,ps_eic) == FALSE)
      {
        DS_3GPP_MSG1_ERROR("Flow Modify accepted returned error:%d",ps_errno);
        return FALSE; 
      }
      /*-------------------------------------------------------------------
        Enable filtering on the flow.
      -------------------------------------------------------------------*/
      if(ds_flow_context_ps_flow_filtering_control(flow_context_p,TRUE)!=TRUE)
      {
        DS_3GPP_MSG1_ERROR("enabling filters failed on flow 0x%x",flow_context_p);
      }
      /*-------------------------------------------------------------------
        Update flow context state.
      -------------------------------------------------------------------*/
      ds_flow_context_set_flow_state(flow_context_p,
                                                 DS_FLOW_CONTEXT_STATE_UP);
    }/* ADD_RSP||DEL_RSP||REPLACE_RSP*/

    /*-------------------------------------------------------------------
       If this is a new flow.
    -------------------------------------------------------------------*/
    else if(flow_context_p->ds_flow_context_dyn_p->state==
            DS_FLOW_CONTEXT_STATE_PEND_ALLOC_RSP)
    { 
      /*-------------------------------------------------------------------
        Post flow activated indication.
      -------------------------------------------------------------------*/
      ds_flow_context_ps_flow_activate_ind(flow_context_p, ps_eic);

    }/* PEND_ALLOC_RSP*/

   /*-------------------------------------------------------------------
       If this is a flow that needs to deleted.
    -------------------------------------------------------------------*/
    else if(flow_context_p->ds_flow_context_dyn_p->state==
            DS_FLOW_CONTEXT_STATE_PEND_DEL_FLOW_RSP)
    {
      /*-------------------------------------------------------------------
       Confirm modification
      -------------------------------------------------------------------*/
      ps_eic = PS_EIC_NOT_SPECIFIED;
      if ((!(flow_context_p->ds_flow_context_dyn_p->flow_type&DS_3GPP_NW_FLOW_TYPE))&&
          (ds_flow_context_ps_flow_modify_result(
                     flow_context_p,is_accepted,ps_eic) == FALSE))
      {
        DS_3GPP_MSG1_ERROR("Flow Modify accepted returned error:%d",ps_errno);
        return FALSE;
      }
      /*---------------------------------------------------------------------
        Update the precedence Bit Mask.
      ---------------------------------------------------------------------*/
      if(ds_flow_manager_update_precedence_bit_mask(
                     pdn_context_p,flow_context_p,FALSE,FALSE,FALSE)==FALSE)
      {
        DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
      }
      /*---------------------------------------------------------------------
        Update the Identifier Bit Mask.
      ---------------------------------------------------------------------*/
      bflow_manager_p = flow_context_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr;
      if(ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
      {
        if(ds_bearer_flow_manager_update_filter_identifier_bit_mask(
                      bflow_manager_p->ds_bearer_flow_manager_dyn_p->bearer_ptr,
                      flow_context_p,TRUE)==FALSE)
        {
          DS_3GPP_MSG0_ERROR("Error in setting Filter identifier bit mask");
        }
      }
      /*---------------------------------------------------------------------
        Remove and delete the pending flow context.
      ---------------------------------------------------------------------*/
      if(ds_flow_manager_remove_pending_flow_context(
                  pdn_context_p,flow_context_p) == FALSE)
      { 
        DS_3GPP_MSG0_ERROR("Could not remove Pending Flow Context");
      }

      ds_flow_context_release_flow_context(flow_context_p);
    }/* PEND_DEL_FLOW_RSP*/    
    else
    {
      DS_3GPP_MSG1_ERROR("Unexpeceted flow context state:0x%x",
                              flow_context_p->ds_flow_context_dyn_p->state);
      return FALSE;
    }
  }/*is_accepted == TRUE */
  else
  {
    DS_3GPP_MSG1_HIGH("NW rejected UE bearer modification request for flow:0x%x",
                                                     flow_context_p);
    if( (flow_context_p->ds_flow_context_dyn_p->state == 
         DS_FLOW_CONTEXT_STATE_PEND_DEL_RSP)||
        (flow_context_p->ds_flow_context_dyn_p->state == 
         DS_FLOW_CONTEXT_STATE_PEND_REPLACE_RSP)||
        (flow_context_p->ds_flow_context_dyn_p->state == 
         DS_FLOW_CONTEXT_STATE_PEND_NO_OP_RSP))
    {
      /*---------------------------------------------------------------------
        Inform about modification rejection
      ---------------------------------------------------------------------*/
      ps_eic = PS_EIC_QOS_NOT_AVAILABLE;
      if(ds_flow_context_ps_flow_modify_result(
                     flow_context_p,is_accepted,ps_eic) == FALSE)
      {
        DS_3GPP_MSG1_ERROR("Flow Modify rejected error:%d",ps_errno);
        return FALSE;
      }
      /*-------------------------------------------------------------------
        Update flow context state.
      -------------------------------------------------------------------*/
      ds_flow_context_set_flow_state(flow_context_p,
                                             DS_FLOW_CONTEXT_STATE_UP);
      return TRUE;
    } /* DEL_RSP||REPLACE_RSP || NO_OP_RSP*/
    else if(flow_context_p->ds_flow_context_dyn_p->state == 
            DS_FLOW_CONTEXT_STATE_PEND_ALLOC_RSP)
    {
      /*---------------------------------------------------------------------
        Clear the precedence and remove the pending flow context.
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

      /*-------------------------------------------------------------------
        Teardown the flow context.
      -------------------------------------------------------------------*/
      ds_flow_context_release_flow_context(flow_context_p);
      return TRUE;
    }/* PEND_ALLOC_RSP*/
    else if(flow_context_p->ds_flow_context_dyn_p->state == 
            DS_FLOW_CONTEXT_STATE_PEND_DEL_FLOW_RSP)
    {
      /*---------------------------------------------------------------------
        Update the precedence Bit Mask.
      ---------------------------------------------------------------------*/
      if(ds_flow_manager_update_precedence_bit_mask(
                    pdn_context_p,flow_context_p,FALSE,FALSE,FALSE)==FALSE)
      {
        DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
      }
      /*---------------------------------------------------------------------
        Update the Identifier Bit Mask.
      ---------------------------------------------------------------------*/
      bflow_manager_p = flow_context_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr;
      if(ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
      {
        if(ds_bearer_flow_manager_update_filter_identifier_bit_mask(
                      bflow_manager_p->ds_bearer_flow_manager_dyn_p->bearer_ptr,
                      flow_context_p,FALSE)==FALSE)
        {
          DS_3GPP_MSG0_ERROR("Error clearing Filter identifier mask");
        }
      }
      /*---------------------------------------------------------------------
        Remove and delete the pending flow context.
      ---------------------------------------------------------------------*/
      if(ds_flow_manager_remove_pending_flow_context(
                  pdn_context_p,flow_context_p) == FALSE)
      { 
        DS_3GPP_MSG0_ERROR("Could not remove Pending Flow Context");
      }

      ds_flow_context_release_flow_context(flow_context_p);
    }/* DS_FLOW_CONTEXT_STATE_PEND_DEL_FLOW_RSP */
    else if(flow_context_p->ds_flow_context_dyn_p->state == 
            DS_FLOW_CONTEXT_STATE_PEND_ADD_RSP)
    {
      /*---------------------------------------------------------------------
        Update the precedence Bit Mask.
      ---------------------------------------------------------------------*/
      if(ds_flow_manager_update_precedence_bit_mask(
                    pdn_context_p,flow_context_p,TRUE,FALSE,TRUE)==FALSE)
      {
        DS_3GPP_MSG0_ERROR("Error in clearing precedence bit mask");
      }

      /*---------------------------------------------------------------------
        Update about modification rejection.
      ---------------------------------------------------------------------*/
      ps_eic = PS_EIC_QOS_NOT_AVAILABLE;
      if(ds_flow_context_ps_flow_modify_result(
                     flow_context_p,is_accepted,ps_eic) == FALSE)
      {
        DS_3GPP_MSG1_ERROR("Flow Modify rejected error:%d",ps_errno);
        return FALSE;
      }
      /*-------------------------------------------------------------------
        Update flow context state.
      -------------------------------------------------------------------*/
      ds_flow_context_set_flow_state(flow_context_p,DS_FLOW_CONTEXT_STATE_UP);
    }
    else
    {
      DS_3GPP_MSG1_ERROR("Unexpeceted flow context state:0x%x",
                         flow_context_p->ds_flow_context_dyn_p->state);
      return FALSE;
    }   
  }/* NW rejected UE modification */
  return TRUE;
}/* ds_flow_manager_update_ps_with_network_response */
    
/*===========================================================================
FUNCTION DS_FLOW_MANAGER_IFACE_OUT_OF_USE

DESCRIPTION
  The iface has gone out of use. clean up any pending flow contexts and
  update the TFT information to inform network about filter cleanup.

PARAMETERS
  pdn_context_p            -  Pointer to PDN context.
  ds_iface_p               -  Pointer to DS Iface context.
  ip_type                  -  Iface IP type.
  delete_nw_flow           -  boolean. IF true NW flow needs to be deleted.

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
void ds_flow_manager_iface_out_of_use
(
  ds_pdn_context_s         *pdn_context_p,
  ds_3gpp_iface_s          *ds_iface_p,
  ds_pdn_context_type_e    ip_type,
  boolean                  delete_nw_flow
)
{
  ds_flow_manager_s         *flow_manager_p = NULL;
  ds_bearer_context_s       *bearer_context_p = NULL;
  int                        cnt = 0;
  ds_flow_context_s         *flow_context_p = NULL;
  uint8                      flow_type = 0;
  /*-------------------------------------------------------------------------*/
  if((!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
      || (ds_iface_p == NULL))
  {
    DS_3GPP_MSG0_ERROR("Invalid input params passed");  
  }
  
  flow_manager_p = ds_flow_manager_find_flow_manager(pdn_context_p);
  if(flow_manager_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Flow manager is null ");
    return;
  }

  if(pdn_context_p->ds_pdn_context_dyn_p->num_active_bearer_context <= 1)
  {
    DS_3GPP_MSG0_HIGH("No dedicated bearers associated with PDN");
    return;
  }
  if((ip_type !=DS_3GPP_PDP_TYPE_IPV4) && (ip_type != DS_3GPP_PDP_TYPE_IPV6))
  {
    DS_3GPP_MSG1_ERROR("invalid ip type passed %d",ip_type);
    return;
  }
  if(ip_type == DS_3GPP_PDP_TYPE_IPV4)
  {
    flow_type = DS_3GPP_V4_FLOW_TYPE;
  }
  if(ip_type == DS_3GPP_PDP_TYPE_IPV6)
  {
    flow_type  = DS_3GPP_V6_FLOW_TYPE;
  }
  for(cnt =0; cnt<DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT; cnt++)
  {
    bearer_context_p = pdn_context_p->ds_pdn_context_dyn_p->bearer_context_ptr_tbl[cnt];
    if(bearer_context_p != NULL)
    {
      if(bearer_context_p != pdn_context_p->ds_pdn_context_dyn_p->def_bearer_context_ptr)
      {
        DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        flow_context_p = ds_bearer_flow_manager_iface_out_of_use(
                                                  pdn_context_p,
                                                  bearer_context_p,
                                                  flow_type,
                                                  delete_nw_flow);
        if(ds_flow_context_validate_flow(flow_context_p))
        {
          if(ds_flow_manager_ue_bearer_modify_req(
              flow_context_p,
              &flow_context_p->ds_flow_context_dyn_p->tft_mod,
              NULL,
              flow_context_p->ds_flow_context_dyn_p->state)==-1)
          {
            DS_3GPP_MSG0_ERROR("UE bearer modify req failed");
          }         
        }
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      }/* not the default bearer */
    }/* if bearer pointer != NULL */
  }/* for bearer cnt */
  return;
} /* ds_flow_manager_iface_out_of_use*/

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_CONFIGURE_DEFAULT_FLOW

DESCRIPTION
  This function is invoked to configure the default flow for a given IFACE.
  Here we bind flow to the default bearer phys_link and configure the
  flow_ioctl_hdlr for the default flow.

PARAMETERS
  iface_p            -  Pointer to IFACE.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_manager_configure_default_flow
(  
  ds_pdn_context_s          *pdn_context_p,
  ds_bearer_context_s       *bearer_context_p,
  ps_iface_addr_family_type  iface_type
)
{  
  int                  ret_val;
  ps_iface_type        *iface_p = NULL;
  ps_flow_type  * default_flow_ptr;
  /*-------------------------------------------------------------------------*/
  
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    return;
  }

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  if(iface_type == IPV4_ADDR)
  {    
    iface_p = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_context_p);

    if (iface_p)
    {
      DS_3GPP_MSG2_HIGH("binding flow to phys link for V4 IFACE [0x%x:%x] ",
                         iface_p->name, iface_p->instance);
    }
    else
    {
      DS_3GPP_MSG0_HIGH("ds_flow_manager_configure_default_flow: Iface Pointer is NULL");
      return;
    }
  }
  else
  {    
    iface_p = DSPDNCNTXT_GET_V6_PS_IFACE_PTR(pdn_context_p);

    if (iface_p)
    {
      DS_3GPP_MSG2_HIGH("binding flow to phys link for V6 IFACE [0x%x:%x] ",
      iface_p->name, iface_p->instance);
    }
    else
    {
      DS_3GPP_MSG0_HIGH("ds_flow_manager_configure_default_flow: Iface Pointer is NULL");
      return;
    }
  }

  /*----------------------------------------------------------------------------
    Bind the default flow of the Iface with the Phys link from the bearer
  ----------------------------------------------------------------------------*/
  ret_val = ps_flow_bind_phys_link(PS_IFACE_GET_DEFAULT_FLOW(iface_p),
                                    &(bearer_context_p->phys_link));

  if(ret_val != 0)
  {
    DS_3GPP_MSG1_ERROR("binding flow to phys link failed:error=%d",ret_val);
    ASSERT(0);
  }

  default_flow_ptr = PS_IFACE_GET_DEFAULT_FLOW(iface_p);

  if(default_flow_ptr != NULL)
  {
    default_flow_ptr->ps_flow_ioctl_f_ptr = ds_flow_manager_flow_ioctl_hdlr;
  }
  else
  {
    DATA_ERR_FATAL("Default flow pointer is NULL");
  }

  return;

}

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_FREE_NW_FLOW_CONTEXT

DESCRIPTION
  Utility function used to free the nw flow contexts associated with a given
  bearer. Also cleans up the filter ids and precedences in the flow managers.

PARAMETERS
  bearer_context_p     (in) -  Bearer context info. 
 
DEPENDENCIES
  None.

RETURN VALUE
 DS3G_SUCCESS - Success
 DS3G_FAILURE - Failure.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_free_nw_flow_context(ds_bearer_context_s *bearer_context_p)
{
  ds_flow_context_s         *flow_context_p = NULL;
  qos_spec_type             curr_qos; /* current qos flow spec */
  ip_filter_type            *curr_tx_fltr = NULL;/* current tx fltrs*/
  ip_filter_type            *curr_rx_fltr = NULL;/* current rx fltrs*/
  boolean                   result = DS3G_FAILURE;

  do
  {
  
    memset(&curr_qos,0, sizeof(qos_spec_type));   

    curr_tx_fltr = (ip_filter_type *)modem_mem_calloc(MAX_PACKET_FILTERS,
                                                      sizeof(ip_filter_type),
                                                      MODEM_MEM_CLIENT_DATA);
    if (NULL == curr_tx_fltr)
    {
      result = DS3G_FAILURE;
      break;
    }

    curr_rx_fltr = (ip_filter_type *)modem_mem_calloc(MAX_PACKET_FILTERS,
                                                      sizeof(ip_filter_type),
                                                      MODEM_MEM_CLIENT_DATA);
    if (NULL == curr_rx_fltr)
    {
      result = DS3G_FAILURE;
      break;
    }

    if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
    {
      result = DS3G_FAILURE;
      break;
    }


    curr_qos.rx.fltr_template.list_ptr = curr_rx_fltr;
    curr_qos.tx.fltr_template.list_ptr = curr_tx_fltr;
  

    //Find the IPV4 flow context and release it.
    flow_context_p = ds_bearer_flow_manager_find_nw_flow_context(
                                   bearer_context_p,DS_3GPP_V4_FLOW_TYPE);
    if(ds_flow_context_validate_flow(flow_context_p))
    {    
      result = ds_flow_context_ps_flow_get_filters( flow_context_p,
                                                    FALSE,
                                                    &curr_qos);
      if (FALSE == result)
      {
        break;
      }
         
      result = ds_flow_manager_update_filter_id_precedence(
                        (ds_pdn_context_s *)bearer_context_p->ds_bearer_context_dyn_p->
                        pdn_context_ptr,
                        bearer_context_p,
                        flow_context_p,
                        0,
                        &curr_qos,
                        TRUE);
      if (FALSE == result)
      {
        break;
      }
  
      (void)ds_bearer_flow_manager_remove_flow_context_from_flow_manager(
                                                            bearer_context_p,
                                                            flow_context_p);
      ds_flow_context_release_flow_context(flow_context_p);
    }
  
    //Find the IPV6 flow context and release it.
    flow_context_p = ds_bearer_flow_manager_find_nw_flow_context(
                                   bearer_context_p,DS_3GPP_V6_FLOW_TYPE);
    if(ds_flow_context_validate_flow(flow_context_p))
    {    
      memset(&curr_qos,0, sizeof(qos_spec_type)); 
      memset(curr_tx_fltr, 0, sizeof(ip_filter_type) *MAX_PACKET_FILTERS);
      memset(curr_rx_fltr, 0, sizeof(ip_filter_type) *MAX_PACKET_FILTERS);

      curr_qos.rx.fltr_template.list_ptr = curr_rx_fltr;      
      curr_qos.tx.fltr_template.list_ptr = curr_tx_fltr;      

      result = ds_flow_context_ps_flow_get_filters( flow_context_p,
                                                    FALSE,
                                                    &curr_qos);
      if (FALSE == result)
      {
        break;
      }

      result = ds_flow_manager_update_filter_id_precedence(
                        (ds_pdn_context_s *)bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr,
                        bearer_context_p,
                        flow_context_p,
                        0,
                        &curr_qos,
                        TRUE);
      if (FALSE == result)
      {
        break;
      }

      (void)ds_bearer_flow_manager_remove_flow_context_from_flow_manager(bearer_context_p,
                                                                         flow_context_p);
      ds_flow_context_release_flow_context(flow_context_p);
    }    

    result = DS3G_SUCCESS;
  
  } while(0);

  modem_mem_free(curr_tx_fltr, MODEM_MEM_CLIENT_DATA);
  modem_mem_free(curr_rx_fltr, MODEM_MEM_CLIENT_DATA);

  return result;
}

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_CONVERT_CAUSE_CODES

DESCRIPTION
  This function is invoked to convert generic cause codes to mode specific
  cause codes.

PARAMETERS
  mode_info_ptr     -  mode into ptr.
  ds_internal_cause -  internal cause code.
  subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_convert_cause_codes
(
  const cm_call_mode_info_s_type         *mode_info_ptr,
  ds_3gpp_internal_cause_code_enum_type  *ds_internal_cause,
  sys_modem_as_id_e_type                  subs_id
)
{
  boolean rval = DS3G_FAILURE;

  DS_FLOW_MGR_VFR_CALL
  (
    rval, 
    convertCauseCodes, 
    subs_id,
    mode_info_ptr, 
    ds_internal_cause
  );

  return rval;
}/* ds_flow_manager_convert_cause_codes*/

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_TEAR_DOWN_SEC_BEARER

DESCRIPTION
  This function is invoked to handle secondary bearer tear down
PARAMETERS
  mode_info_ptr     -  mode into ptr.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_tear_down_sec_bearer
(
  ds_flow_context_s   *flow_context_p
)
{
  boolean                        rval = TRUE;
  ds_3gpp_iface_s                *ds_iface_p = NULL;
  ds_sys_subscription_enum_type  ds_subs_id;
  sys_modem_as_id_e_type         subs_id;
    
  /*----------------------------------------------------------------------*/

  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return FALSE;
  }

  ds_iface_p = flow_context_p->ds_flow_context_dyn_p->ds_iface_ptr;

  if (ds_iface_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Invalid DS iface");
    return FALSE;
  }

  ds_subs_id = PS_IFACE_GET_SUBS_ID(&(ds_iface_p->ps_iface));

  subs_id = ds3gsubsmgr_subs_id_ds_to_cm(ds_subs_id);

  DS_FLOW_MGR_VFR_CALL
  (
    rval, 
    TearDownSecBearer, 
    subs_id,
    flow_context_p 
  );

  return rval;
}/*  ds_flow_manager_tear_down_sec_bearer*/

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_NW_BEARER_ALLOC_REJ

DESCRIPTION
  This function is invoked to handle bearer allocation rejection by the 
  network.

PARAMETERS
  mode_info_ptr     - Mode into ptr
 subs_id            - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void  ds_flow_manager_nw_bearer_alloc_rej
(
  cm_call_mode_info_s_type* mode_info_ptr,
  sys_modem_as_id_e_type    subs_id
)
{
  DS_FLOW_MGR_VF_CALL
  (
    NwBearerAllocRej, 
    subs_id,
    mode_info_ptr 
  );
}/*  ds_flow_manager_nw_bearer_alloc_rej*/
                               
/*===========================================================================
FUNCTION DS_FLOW_MANAGER_NW_BEARER_ALLOC_FAIL

DESCRIPTION
  This function is invoked to handle bearer allocation failure by the 
  network.

PARAMETERS
  mode_info_ptr     -  mode into ptr.
  subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_manager_nw_bearer_alloc_fail
(
  cm_call_mode_info_s_type* mode_info_ptr,
  sys_modem_as_id_e_type    subs_id
)
{
  DS_FLOW_MGR_VF_CALL
  (
    NwBearerAllocFail, 
    subs_id,
    mode_info_ptr 
  );
}/* ds_flow_manager_nw_bearer_alloc_fail*/

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_NW_BEARER_MODIFY_REJ

DESCRIPTION
  This function is invoked to handle bearer modification rejection by the
  network.

PARAMETERS
  mode_info_ptr     -  mode into ptr.
  subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_manager_nw_bearer_modify_rej
(
  cm_call_mode_info_s_type* mode_info_ptr,
  sys_modem_as_id_e_type    subs_id
)
{
  DS_FLOW_MGR_VF_CALL
  (
    NwBearerModifyRej, 
    subs_id,
    mode_info_ptr 
  );
} /* ds_flow_manager_nw_bearer_modify_rej */
                               
/*===========================================================================
FUNCTION DS_FLOW_MANAGER_NW_BEARER_MODIFY_IND

DESCRIPTION
  This function is invoked to handle Network bearer modification indication

PARAMETERS
  mode_info_ptr     -  mode into ptr.
  subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_nw_bearer_modify_ind
(
  cm_call_mode_info_s_type* mode_info_ptr,
  cm_call_id_type           call_id,
  sys_modem_as_id_e_type    subs_id
)
{
  boolean rval = DS3G_FAILURE;

  DS_FLOW_MGR_VFR_CALL
  (
    rval, 
    NwBearerModifyInd, 
    subs_id,
    mode_info_ptr,
    call_id
  );

  return rval;
}/*ds_flow_manager_nw_bearer_modify_ind*/

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_NW_CONTEXT_MODIFY_CNF

DESCRIPTION
  This function is invoked to handle Network context modification confirmation

PARAMETERS
  mode_info_ptr     -  mode into ptr.
  subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_nw_context_modify_cnf
(
  cm_call_id_type           call_id,
  cm_call_mode_info_s_type* mode_info_ptr,
  sys_modem_as_id_e_type    subs_id
)
{
  boolean                   rval = DS3G_FAILURE;
  /*----------------------------------------------------------------------*/

  DS_FLOW_MGR_VFR_CALL
  (
    rval, 
    NwContextModifyCnf, 
    subs_id,
    call_id,
    mode_info_ptr
  );

  return rval;
}/*ds_flow_manager_nw_context_modify_cnf*/

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_NW_CONTEXT_MODIFY_REJ

DESCRIPTION
  This function is invoked to handle Network context modification rejection

PARAMETERS
  mode_info_ptr     -  mode into ptr.
  subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_nw_context_modify_rej
(
  cm_call_id_type           call_id,
  cm_call_mode_info_s_type* mode_info_ptr,
  sys_modem_as_id_e_type    subs_id
)
{
  boolean                   rval = DS3G_FAILURE;
  /*----------------------------------------------------------------------*/

  DS_FLOW_MGR_VFR_CALL
  (
    rval, 
    NwContextModifyRej, 
    subs_id,
    call_id,
    mode_info_ptr
  );

  return rval;
}/*ds_flow_manager_nw_context_modify_rej*/

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_NW_CONTEXT_MODIFY_IND

DESCRIPTION
  This function is invoked to handle Network context modification indication

PARAMETERS
  mode_info_ptr     -  mode into ptr.
  subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_nw_context_modify_ind
(
  cm_call_id_type           call_id,
  cm_call_mode_info_s_type* mode_info_ptr,
  sys_modem_as_id_e_type    subs_id
)
{
  boolean                   rval = DS3G_FAILURE;
  /*----------------------------------------------------------------------*/

  DS_FLOW_MGR_VFR_CALL
  (
    rval, 
    NwContextModifyInd, 
    subs_id,
    call_id,
    mode_info_ptr
  );

  return rval;
}

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_NW_CONTEXT_MODIFY_REQ

DESCRIPTION
  This function is invoked to handle Network context modification request

PARAMETERS
  subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_nw_context_modify_req
(
  cm_call_id_type                 call_id,
  ds_cmd_type                    *cmd_ptr,
  cm_pdp_act_res_params_s_type   *act_res_params_ptr,
  sys_modem_as_id_e_type          subs_id
)
{
  boolean                   rval = DS3G_FAILURE;
  /*----------------------------------------------------------------------*/

  DS_FLOW_MGR_VFR_CALL
  (
    rval, 
    NwContextModifyReq, 
    subs_id,
    call_id,
    cmd_ptr,
    act_res_params_ptr
  );

  return rval;
}/*ds_flow_manager_nw_context_modify_req*/


/*===========================================================================
FUNCTION DS_FLOW_MANAGER_NW_BEARER_ALLOCATION_IND

DESCRIPTION
  This function is invoked to handle Network bearer allocation indication

PARAMETERS
  pdn_context_p     -  ptr to pdn context
  bearer_context_p  -  ptr to bearer context
  mode_info_ptr     -  mode info ptr

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_nw_bearer_allocation_ind
(
  ds_pdn_context_s               *pdn_context_p,
  ds_bearer_context_s            *bearer_context_p,
  const cm_call_mode_info_s_type *mode_info_ptr
)
{
  boolean                rval = DS3G_FAILURE;
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
  /*----------------------------------------------------------------------*/

  if (!ds_bearer_cntxt_get_subs_id_from_bearer (bearer_context_p, &subs_id))
  {
    return rval;
  }

  DS_FLOW_MGR_VFR_CALL
  (
    rval, 
    NwBearerAllocationInd, 
    subs_id,
    pdn_context_p,
    bearer_context_p,
    mode_info_ptr
  );

  return rval;
}/* ds_flow_manager_nw_bearer_allocation_ind */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_NW_BEARER_ACTIVATION_REQ

DESCRIPTION
  This function is invoked to handle Network bearer activation indication

PARAMETERS
  pdn_context_p     -  ptr to pdn context
  bearer_context_p  -  ptr to bearer context
  mode_info_ptr     -  mode info ptr

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_nw_bearer_activation_req
(
  ds_pdn_context_s         *pdn_context_p,
  ds_bearer_context_s      *bearer_context_p,
  const cm_call_mode_info_s_type *mode_info_ptr,
  cm_end_params_s_type     *end_params
)
{
  boolean                rval = DS3G_FAILURE;
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
  /*----------------------------------------------------------------------*/

  if (!ds_bearer_cntxt_get_subs_id_from_bearer (bearer_context_p, &subs_id))
  {
    return FALSE;
  }

  DS_FLOW_MGR_VFR_CALL
  (
    rval, 
    NwBearerActivationReq, 
    subs_id,
    pdn_context_p,
    bearer_context_p,
    mode_info_ptr,
    end_params,
    subs_id
  );

  return rval;
}/* ds_flow_manager_nw_bearer_allocation_ind */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_CHECK_SDF_ID

DESCRIPTION
  This function is invoked to verify if UE has any pending requests matching
  the SDF ID passed in the NW call tear down inidication.

PARAMETERS
  mode_info_ptr     -  mode info ptr
  subs_id           - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_manager_check_sdf_id
(
  cm_call_mode_info_s_type* mode_info_ptr,
  sys_modem_as_id_e_type    subs_id
)
{
  DS_FLOW_MGR_VF_CALL
  (
    checkSdfId, 
    subs_id,
    mode_info_ptr 
  );
}/* ds_flow_manager_check_sdf_id */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_UE_BEARER_MODIFY_REQ

DESCRIPTION
  This function is invoked to handle UE request to modify an existing bearer
  bearer. 

PARAMETERS
  flow_context_p    -  ptr to flow context
  nw_tft_p          -  tft ptr
  app_qos           -  qos in app format
  flow_state        -  flow state when the command was posted

DEPENDENCIES
  None.

RETURN VALUE
  int  - Returns 0 if success, -1 otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
int ds_flow_manager_ue_bearer_modify_req
(
  ds_flow_context_s  *flow_context_p,
  tft_type_T         *nw_tft_p,
  qos_spec_type      *app_qos,
  int                 flow_state
)
{
  int                            rval = -1;
  ds_3gpp_iface_s               *ds_iface_p = NULL;
  ds_sys_subscription_enum_type  ds_subs_id;
  sys_modem_as_id_e_type         subs_id;
    
  /*----------------------------------------------------------------------*/

  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return -1;
  }

  ds_iface_p = flow_context_p->ds_flow_context_dyn_p->ds_iface_ptr;

  if (ds_iface_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Invalid DS iface");
    return -1;
  }

  ds_subs_id = PS_IFACE_GET_SUBS_ID(&(ds_iface_p->ps_iface));

  subs_id = ds3gsubsmgr_subs_id_ds_to_cm(ds_subs_id);

  DS_FLOW_MGR_VFR_CALL
  (
    rval, 
    UeBearerModifyReq, 
    subs_id,
    flow_context_p,
    nw_tft_p,
    app_qos,
    flow_state
  );

  return rval;
}/* ds_flow_manager_ue_bearer_modify_req */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_UE_BEARER_ALLOC_REQ

DESCRIPTION
  This function is invoked to handle UE request to allocate dedicated bearer.

PARAMETERS
  pdn_context_p     -  ptr to pdn context
  flow_context_p    -  ptr to flow context
  app_qos           -  qos in app format
  nw_tft            -  tft in Nw format

DEPENDENCIES
  None.

RETURN VALUE
  int  - Returns 0 if success, -1 otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
int ds_flow_manager_ue_bearer_alloc_req
(
  ds_pdn_context_s  *pdn_context_p,
  ds_flow_context_s *flow_context_p,
  qos_spec_type     *app_qos,
  tft_type_T        *nw_tft
)
{
  int                    rval = -1;
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
  /*----------------------------------------------------------------------*/

  
  if(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p, &subs_id)== FALSE)
  {
    return rval;
  }

  DS_FLOW_MGR_VFR_CALL
  (
    rval, 
    UeBearerAllocReq, 
    subs_id,
    pdn_context_p,
    flow_context_p,
    app_qos,
    nw_tft
  );

  return rval;
}/*ds_flow_manager_ue_bearer_alloc_req*/

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_UE_BEARER_ALLOC_ABORT_REQ

DESCRIPTION
  This function is invoked to hanlde UE request to abort a pending dedicated
  bearer request (allocation/modification).

PARAMETERS
  flow_context_p    -  ptr to flow context

DEPENDENCIES
  None.

RETURN VALUE
  int  - Returns 0 if success, -1 otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
int ds_flow_manager_ue_bearer_alloc_abort_req
(
  ds_flow_context_s  *flow_context_p
)
{
  int rval = -1;
  ds_3gpp_iface_s               *ds_iface_p = NULL;
  ds_sys_subscription_enum_type  ds_subs_id;
  sys_modem_as_id_e_type         subs_id;
    
  /*----------------------------------------------------------------------*/

  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return -1;
  }

  ds_iface_p = flow_context_p->ds_flow_context_dyn_p->ds_iface_ptr;

  if (ds_iface_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Invalid DS iface");
    return -1;
  }

  ds_subs_id = PS_IFACE_GET_SUBS_ID(&(ds_iface_p->ps_iface));

  subs_id = ds3gsubsmgr_subs_id_ds_to_cm(ds_subs_id);

  DS_FLOW_MGR_VFR_CALL
  (
    rval, 
    UeBearerAllocAbortReq, 
    subs_id,
    flow_context_p
  );

  return rval;
}/* ds_flow_manager_ue_bearer_alloc_abort_req */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_PROCESS_CREATE_TFT

DESCRIPTION
  This function is invoked to handle the Create TFT operation.

PARAMETERS
  pdn_context_p     -  PDN context info.
  bearer_context_p  -  Bearer context info
  nw_tft_p          -  TFT information.
  mode_info_ptr     -  Extended information about this operation.
  cause_code_p      -  Cause code in case of failure.

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_process_create_tft
(
  ds_pdn_context_s                      *pdn_context_p,
  ds_bearer_context_s                   *bearer_context_p,
  const tft_type_T*                      nw_tft_p,
  const cm_call_mode_info_s_type        *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{
  boolean                   rval = DS3G_FAILURE;
  sys_modem_as_id_e_type    subs_id;
  /*----------------------------------------------------------------------*/

  if (!ds_bearer_cntxt_get_subs_id_from_bearer (bearer_context_p, &subs_id))
  {
    return rval;
  }

  DS_FLOW_MGR_VFR_CALL
  (
    rval,      
    ProcessCreateTFT,
    subs_id,
    pdn_context_p,
    bearer_context_p,
    nw_tft_p,
    mode_info_p,
    cause_code_p
  );

  return rval;
}/* ds_flow_manager_process_create_tft */

/*===========================================================================
FUNCTION DS_FLOW_MANAGER_PROCESS_DELETE_TFT

DESCRIPTION
  This function is invoked to handle the Delete Existing TFT operation.

PARAMETERS
  pdn_context_p     -  PDN context info.
  bearer_context_p  -  Bearer context info
  nw_tft_p          -  TFT information.
  mode_info_ptr     -  Extended information about this operation.
  cause_code_p      -  Cause code in case of failure.

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns True if success.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_manager_process_delete_tft
(
  ds_pdn_context_s         *pdn_context_p,
  ds_bearer_context_s      *bearer_context_p,
  const tft_type_T*         nw_tft_p,
  const cm_call_mode_info_s_type *mode_info_p,
  ds_3gpp_internal_cause_code_enum_type *cause_code_p
)
{
  boolean                   rval = DS3G_FAILURE;
  sys_modem_as_id_e_type    subs_id;
  /*----------------------------------------------------------------------*/

  if((nw_tft_p->operation_code == DS_3GPP_TFT_OPCODE_DEL_TFT) &&
     (nw_tft_p->num_filters > 0))
  {
    DS_3GPP_MSG0_ERROR("non-empty TFT with op-code Delete-existing TFT recieved, "
                       "Reject modify");
    *cause_code_p = DS_3GPP_INTERNAL_SYNTACTICAL_ERR_IN_TFT;
    return rval;
  }

  if (!ds_bearer_cntxt_get_subs_id_from_bearer (bearer_context_p, &subs_id))
  {
    return rval;
  }

  DS_FLOW_MGR_VFR_CALL
  (
    rval,      
    ProcessDeleteTFT,
    subs_id,
    pdn_context_p,
    bearer_context_p,
    nw_tft_p,
    mode_info_p,
    cause_code_p
  );

  return rval;
}/* ds_flow_manager_process_delete_tft */

/*===========================================================================
FUNCTION ds_flow_manager_update_uplink_flow_capability 

DESCRIPTION
  This function is used to enable or disable uplink flow based on 
  flow direction (This is based on GBR values granted by NW) 

PARAMETERS
 bearer_context_p: Bearer for which data path is to be setup
 boolean:  to Enable or Disable flow
 
DEPENDENCIES
  None.

RETURN VALUE
None
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_manager_update_uplink_flow_capability
(
  ds_bearer_context_s *bearer_context_p,
  boolean              ul_flow_enable
)
{
  ds_pdn_context_s        *pdn_context_p = NULL;
  ds_flow_context_s       *v4_flow_cntx_p = NULL;
  ds_flow_context_s       *v6_flow_cntx_p = NULL;
  /*-----------------------------------------------------  */
  pdn_context_p = bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    return;
  }
  /*  Get V4 &V6flow context pointers, this is to handle Dual IP case as well*/
  v4_flow_cntx_p = ds_bearer_flow_manager_find_flow_context(bearer_context_p,
                                                        DS_3GPP_V4_FLOW_TYPE,
                                                        FALSE);
  
  v6_flow_cntx_p = ds_bearer_flow_manager_find_flow_context(bearer_context_p,
                                                        DS_3GPP_V6_FLOW_TYPE,
                                                        FALSE);
  
  DS_3GPP_MSG3_HIGH("V4 flow_ptr: 0x%x, V6 flow_ptr: 0x%x, Enable_flag: %d",
                      v4_flow_cntx_p, v6_flow_cntx_p, ul_flow_enable );
  if (ul_flow_enable)
  {
    DS_3GPP_MSG0_HIGH("Enabling Phys_link flow and resetting PS flow Capability");
    /* Enable Phys link flow */
    ps_phys_link_enable_flow(
                        &(bearer_context_p->phys_link),
                        DS_FLOW_UMTS_MASK);
	
    /* Check validity of flow_ptrs  reset the flow capabiltiy */
  if (ds_flow_context_validate_flow(v4_flow_cntx_p))
  {
      PS_FLOW_RESET_CAPABILITY(v4_flow_cntx_p->ds_flow_context_dyn_p->ps_flow_ptr, 
                             PS_FLOW_CAPABILITY_DATA_DISALLOWED);
  }

  if (ds_flow_context_validate_flow(v6_flow_cntx_p))
  {
       PS_FLOW_RESET_CAPABILITY(v6_flow_cntx_p->ds_flow_context_dyn_p->ps_flow_ptr, 
                             PS_FLOW_CAPABILITY_DATA_DISALLOWED);
  }
  }
  else
  {
    DS_3GPP_MSG0_HIGH("Disabling Phys_link flow and setting PS flow Capability");
    /* Disable the flow */
       ps_phys_link_disable_flow(
                        &(bearer_context_p->phys_link),
                        DS_FLOW_UMTS_MASK);
	 
     /* Check validity of flow_ptrs  set the flow capabiltiy */
    if (ds_flow_context_validate_flow(v4_flow_cntx_p))
    {
      PS_FLOW_SET_CAPABILITY(v4_flow_cntx_p->ds_flow_context_dyn_p->ps_flow_ptr, 
                             PS_FLOW_CAPABILITY_DATA_DISALLOWED);
    }

    if (ds_flow_context_validate_flow(v6_flow_cntx_p))
    {
      PS_FLOW_SET_CAPABILITY(v6_flow_cntx_p->ds_flow_context_dyn_p->ps_flow_ptr, 
                             PS_FLOW_CAPABILITY_DATA_DISALLOWED);
    }
  }
  return;
  
}

/*===========================================================================
FUNCTION ds_flow_manager_alloc_dyn_mem

DESCRIPTION
  This function allocates dyn memory.
 
PARAMETERS
  index   - index where the memory would be allocated
  subs_id - Subscription id
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

=============================================================================*/
static boolean ds_flow_manager_alloc_dyn_mem
(
  int                    alloc_index,
  sys_modem_as_id_e_type subs_id
)
{
  boolean ret_val = FALSE;
  ds_flow_manager_dyn_s *flow_manager_dyn_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((alloc_index < 0) || (alloc_index >= DS_3GPP_MAX_PDN_CONTEXT))
  {
    DS_3GPP_MSG3_ERROR("Index:%d is out of bounds",alloc_index,0,0);
    return ret_val;
  }

  flow_manager_dyn_p = (ds_flow_manager_dyn_s *)ds_flow_manager_tbl[alloc_index].
                                                          ds_flow_manager_dyn_p;
                                                 
  if(flow_manager_dyn_p == NULL)
  {
      flow_manager_dyn_p = (ds_flow_manager_dyn_s *)modem_mem_alloc
        (sizeof(ds_flow_manager_dyn_s),MODEM_MEM_CLIENT_DATA);

      if(flow_manager_dyn_p != NULL)
      {
        DS_3GPP_MSG1_MED("Dynamic memory allocation successful at index:%d",
                        alloc_index);
        memset(flow_manager_dyn_p,0,
               sizeof(ds_flow_manager_dyn_s));
        flow_manager_dyn_p->cookie = FLOW_MANAGER_VALID_COOKIE;
        flow_manager_dyn_p->index = alloc_index;
        ds_flow_manager_tbl[alloc_index].ds_flow_manager_dyn_p = flow_manager_dyn_p;

        ret_val = ds_3gpp_dyn_mem_hdlr_allocate_eps_flow_manager
                    (alloc_index, subs_id);

        if(!ret_val)
        {
          DS_3GPP_MSG0_ERROR("EPS Flow Mgr Allocation failure, Free Flow Mgr");
          ds_flow_manager_free_dyn_mem(&ds_flow_manager_tbl[alloc_index]);
        }
      }
      else
      {
        DS_3GPP_MSG3_ERROR("Failed to allocate Flow mgr heap memory",0,0,0);
      }
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Flow manager dyn mem has already been allocated "
                       "at index:%d",alloc_index);
  }                   


  return ret_val;
}

/*===========================================================================
FUNCTION ds_flow_manager_validate_flow_manager

DESCRIPTION
  This function checks if the flow manager tbl has been allocated dynamic memory

PARAMETERS
  flow_manager_p       - flow_manager to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_flow_manager_validate_flow_manager
(
  ds_flow_manager_s *flow_manager_p
)
{
  boolean ret_val = FALSE;
  ds_flow_manager_dyn_s *flow_manager_dyn_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(flow_manager_p == NULL)
  {
    DS_3GPP_MSG0_LOW("Input parameter flow_context is NULL");
    return ret_val;
  }

  flow_manager_dyn_p = flow_manager_p->ds_flow_manager_dyn_p;

  if((flow_manager_dyn_p != NULL) &&
    (flow_manager_dyn_p->cookie == FLOW_MANAGER_VALID_COOKIE))
  {
    DS_3GPP_MSG1_LOW("Flow Manager dyn mem is valid at index:%d",
                     flow_manager_p->ds_flow_manager_dyn_p->index);
    ret_val = TRUE;
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Flow manager:0x%x is INVALID",flow_manager_p);
  }

  return ret_val;
}

/*===========================================================================
FUNCTION ds_flow_manager_free_dyn_mem

DESCRIPTION
  This function allocates dyn memory.
 
PARAMETERS
 pdn_context_p 
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

=============================================================================*/
void ds_flow_manager_free_dyn_mem
(
   ds_flow_manager_s *flow_manager_p
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------- 
   
    Don't care about the cookie, We are anyways freeing the memory
   -------------------------------------------------------------------------*/
  if((flow_manager_p == NULL) || (flow_manager_p->ds_flow_manager_dyn_p == NULL))
  {
    DS_3GPP_MSG0_ERROR("Invalid Flow manaager passed");
    return;
  }

  modem_mem_free(flow_manager_p->ds_flow_manager_dyn_p,MODEM_MEM_CLIENT_DATA);
  flow_manager_p->ds_flow_manager_dyn_p = NULL;
  DS_3GPP_MSG0_HIGH("Released the flow manager associated with PDN");
}
#endif /* FEATURE_DATA_3GPP */
