/*!
  @file
  ds_3gpp_flow_context.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_flow_context.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/12   ss      3GPP MH LTE-Off compilation fixes.
10/05/11   ttv     Added DSDS changes for new mode handler.
05/12/10   sa      Added support for dynamic cid for flow context.
04/03/10   vk      Added support for MSG2.0 based logging
02/04/10   vd      Changed function naming format.
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

#ifdef FEATURE_DATA_3GPP
#include "ds_3gpp_bearer_context.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_hdlr.h"
#include "dstask_v.h"
#include "ds3gmgr.h"
#include "ds_3gpp_flow_context.h"
#include "ds_3gpp_flow_manager.h"
#include "ds_3gpp_bearer_flow_manager.h"
#include "ds_3gpp_rmsm_atif.h"
#include "ds_flow_control.h"
#include "modem_mem.h"
#include "ran.h"
#ifdef FEATURE_DATA_LTE
#include "ds_eps_bearer_context.h"
#endif

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*-------------------------------------------------------------------------
  Dynamic CID base
--------------------------------------------------------------------------*/
#define DS_FLOW_DYNAMIC_PROFILE_BASE  100

#define DS_FLOW_DYNAMIC_PROFILE_MAX   255

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
LOCAL ds_flow_context_s* ds_flow_context_get_flow_context_from_cid
( 
  uint32                   cid,
  sys_modem_as_id_e_type   subs_id
);

LOCAL boolean ds_flow_context_alloc_dyn_mem
(
  int index
);

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_ALLOCATE_FLOW_CONTEXT

DESCRIPTION
  Allocate a new flow context.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
 ds_flow_context_s * - Pointer to empty flow context.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_flow_context_s *ds_flow_context_allocate_flow_context(void)
{
  int index = 0;
  ds_flow_context_s *flow_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (index = 0; index < DS_3GPP_MAX_FLOW_CONTEXT; index++)
  {
    if(!ds_flow_context_validate_flow(&ds_flow_context_tbl[index]))
    {

      if(!ds_flow_context_alloc_dyn_mem(index))
      {
        /* dynamic mem allocation failed, return NULL ;*/
        DS_3GPP_MSG0_ERROR("Dynamic memory alloc failed, return NULL");
        return NULL;
      }

      flow_context_p = &ds_flow_context_tbl[index];
      /* allocate a dynamic cid to this flow */
      if ( FALSE == ds_flow_context_alloc_dynamic_cid(
                          &(flow_context_p->ds_flow_context_dyn_p->cid)))
      {
        /* dynamic cid allocation failed, return NULL ;*/
        DS_3GPP_MSG0_ERROR("Dynamic CID allocation failed, return NULL");
        modem_mem_free(flow_context_p->ds_flow_context_dyn_p,MODEM_MEM_CLIENT_DATA);
        flow_context_p->ds_flow_context_dyn_p = NULL;
        return NULL;
      }
      break;
    }
  }

  if(index == DS_3GPP_MAX_FLOW_CONTEXT)
  {
    DS_3GPP_MSG0_ERROR("MAX Flow contexts are allocated");
  }

  return flow_context_p;
} /* ds_flow_context_allocate_flow_context */


/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_RELEASE_FLOW_CONTEXT

DESCRIPTION
  Return the flow context.

PARAMETERS
  flow_context_p  - Pointer to the flow context.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_context_release_flow_context
(
  ds_flow_context_s *flow_context_p
)
{
  int flow_context_id = 0;
  int16 ds_errno = 0;
  ps_extended_info_code_enum_type ps_eic = PS_EIC_NOT_SPECIFIED; 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return;
  }
  
  if(flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr != NULL)
  {
    if(flow_context_p->ds_flow_context_dyn_p->ds_iface_ptr == NULL)
    {
      DS_3GPP_MSG1_ERROR("Cannot Associate 3GPP iface with flow context:0x%x",
                              flow_context_p->sdf_id);    
    }
    else
    {
      ds_flow_context_ps_flow_go_null_ind(flow_context_p, ps_eic);
      if(ps_iface_delete_flow(&(flow_context_p->ds_flow_context_dyn_p
                             ->ds_iface_ptr->ps_iface),
                             flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr,
                             &ds_errno) < 0)
      {
        DS_3GPP_MSG1_ERROR("Cannot Delete the PS FLOW for flow context:0x%x",
                                flow_context_p->sdf_id); 
      }
      else
      {
        DS_3GPP_MSG1_HIGH("Deleted PS Flow for flow context:0x%x",
                              flow_context_p->sdf_id);
      }
    }
  }/* ps_flow_ptr != NULL */
  /*-------------------------------------------------------------------------
    Back up the flow context id.
  -------------------------------------------------------------------------*/
  flow_context_id =(flow_context_p->sdf_id);

  if(flow_context_id >= DS_FLOW_SDF_ID)
  {
    flow_context_id -= DS_FLOW_SDF_ID;
  }
  
  ds_flow_context_free_dyn_mem(flow_context_p);

  /*-------------------------------------------------------------------------
    Restore the the flow context id.
  -------------------------------------------------------------------------*/
  flow_context_p->sdf_id = flow_context_id;

}/* ds_flow_context_release_flow_context */


/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_CONFIGURE_FLOW_CONTEXT

DESCRIPTION
  Configure the IP type and state for the flow context.

PARAMETERS
  flow_context_p      - Flow Context information.
  pdn_context_p       - PDN context information.
  bearer_context_p    - Bearer context information.
  flow_type           - flow flag information.
  ip_type             - V4 or V6 ip type.

DEPENDENCIES
  None.

RETURN VALUE
boolean                - returns FALSE if comparision fails.  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_context_configure_flow_context
(
  ds_flow_context_s     *flow_context_p,
  ds_pdn_context_s      *pdn_context_p,
  ds_bearer_context_s   *bearer_context_p,
  uint8                  flow_type,
  uint8                  ip_type
)
{
  uint8 temp =(DS_3GPP_NW_FLOW_TYPE|DS_3GPP_V4_FLOW_TYPE|
               DS_3GPP_V6_FLOW_TYPE);
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p)) ||
     bearer_context_p==NULL)
  {
    return FALSE;
  }

  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return FALSE;
  }

  if((flow_type==0)||( (flow_type & ~(temp)) !=0 ) )
  {
    DS_3GPP_MSG0_ERROR("invalid flow type input");
    return FALSE;
  }

  if(ip_type > DS_3GPP_PDN_CONTEXT_IPV6_IFACE)
  {
    DS_3GPP_MSG0_ERROR("invalid ip type input");
    return FALSE;
  }
  /*----------------------------------------------------------------
    Update the Flow context state.
  -----------------------------------------------------------------*/ 
  ds_flow_context_set_flow_state(flow_context_p,
                DS_FLOW_CONTEXT_STATE_COMING_UP);
  flow_context_p->ds_flow_context_dyn_p->flow_type= flow_type;
  /*-------------------------------------------------------------------------
    Retrieve the bearer flow manager.
  -------------------------------------------------------------------------*/
  bflow_manager_p=ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
  if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p ))
  {
    return FALSE;
  }
  flow_context_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr =(void *)bflow_manager_p;
  flow_context_p->ds_flow_context_dyn_p->ds_iface_ptr = 
           pdn_context_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[ip_type];

  return TRUE;
} /* ds_flow_context_configure_flow_context*/


/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_ACTIVATE_CMD

DESCRIPTION
  Invoke the activate command on the PS Flow associated with the flow context.

PARAMETERS
  flow_context_p   - Flow Context information.

DEPENDENCIES
  None.

RETURN VALUE
   int              - Indicates sucess/ failure
SIDE EFFECTS
  None.

===========================================================================*/
int ds_flow_context_ps_flow_activate_cmd
( 
  ds_flow_context_s   *flow_context_p
)
{
  ps_flow_type         *ps_flow_ptr =NULL;
  int16                ds_errno = 0;   
  int                  return_value = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --*/
  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return -1;
  }
  DS_3GPP_MSG1_HIGH("POST PS FLOW ACTIVATE CMD on flow context:0x%x",
                         flow_context_p->sdf_id);
  ps_flow_ptr = flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr;
  if(ps_flow_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("ps flow is NULL in flowActivateCmd");
    return -1;
  }
  return_value =ps_flow_activate_cmd(ps_flow_ptr,&ds_errno, NULL);

  if((return_value == -1) && (ds_errno != DS_EWOULDBLOCK))
  {
    DS_3GPP_MSG0_HIGH("ps_flow_activate_cmd returned -1");
    return -1;
  }
  ds_flow_context_set_flow_state(flow_context_p,
                                  DS_FLOW_CONTEXT_STATE_COMING_UP);
  return 0;
} /*  ds_flow_context_ps_flow_activate_cmd */

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_GO_NULL_CMD

DESCRIPTION
  Invoke the go null command on the PS flow associated with the flow context.

PARAMETERS
  flow_context_p   - Flow Context information.

DEPENDENCIES
  None.

RETURN VALUE
  int              - Indicates sucess/ failure

SIDE EFFECTS
  None.

===========================================================================*/
int ds_flow_context_ps_flow_go_null_cmd
( 
  ds_flow_context_s   *flow_context_p
)
{
  ps_flow_type         *ps_flow_ptr =NULL;
  int16                ds_errno = 0;   
  int                  return_value = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --*/
  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return -1;
  }
  DS_3GPP_MSG1_HIGH("POST PS FLOW GO NULL CMD on flow context:0x%x",
                         flow_context_p->sdf_id);
  ps_flow_ptr = flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr;
  if(ps_flow_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("ps flow is NULL in flowGoNullCmd");
    return -1;
  }
  return_value =ps_flow_go_null_cmd(ps_flow_ptr,&ds_errno, NULL);
  if((return_value == -1) && (ds_errno != DS_EWOULDBLOCK))
  {
    DS_3GPP_MSG0_HIGH("ps_flow_go_null_cmd failed");
    return -1;
  }
  return 0;
} /* ds_flow_context_ps_flow_go_null_cmd */

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_GO_NULL_IND

DESCRIPTION
  Post the go null indication on the PS flow associated with the flow context.

PARAMETERS
  flow_context_p   - Flow Context information.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_context_ps_flow_go_null_ind
( 
  ds_flow_context_s               *flow_context_p,
  ps_extended_info_code_enum_type ps_eic
)
{
  ps_flow_type         *ps_flow_ptr =NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --*/
  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return;
  }
  DS_3GPP_MSG1_HIGH("Indicate PS FLOW is gone NULL on flow context:0x%x",
                         flow_context_p->sdf_id);
  ps_flow_ptr = flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr;
  if(ps_flow_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("ps flow is NULL in flowGoNullInd");
    return;
  }
  ps_flow_go_null_ind(ps_flow_ptr, ps_eic); 
  ds_flow_context_set_flow_state(flow_context_p,
                DS_FLOW_CONTEXT_STATE_DOWN);
} /* ds_flow_context_ps_flow_go_null_ind */

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_ACTIVATE_IND

DESCRIPTION
  Post activated indication on the PS flow associated with the flow context.

PARAMETERS
  flow_context_p   - Flow Context information.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_context_ps_flow_activate_ind
( 
  ds_flow_context_s               *flow_context_p,
  ps_extended_info_code_enum_type ps_eic
)
{
  ps_flow_type         *ps_flow_ptr =NULL;
  int16 ps_errno = 0;  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --*/
  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return;
  }
  DS_3GPP_MSG1_HIGH("Indicate PS FLOW is activated on flow context:0x%x",
                         flow_context_p->sdf_id);
  ps_flow_ptr = flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr;
  if(ps_flow_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("ps flow is NULL in flowActivateInd");
    return;
  }
  if(flow_context_p->ds_flow_context_dyn_p->granted_flow_set != TRUE)
  {
    ds_flow_context_ps_flow_set_granted_flow(flow_context_p);
    flow_context_p->ds_flow_context_dyn_p->granted_flow_set = TRUE;
  }
  if(ds_flow_context_ps_flow_filtering_control(flow_context_p,TRUE) != TRUE)
  {
    DS_3GPP_MSG1_ERROR("error enabling filters on the flow:%d",ps_errno);
  }
  ps_flow_activate_ind(ps_flow_ptr,ps_eic);
  ds_flow_context_set_flow_state(flow_context_p,
                DS_FLOW_CONTEXT_STATE_UP); 
  
} /* ds_flow_context_ps_flow_activate_ind */

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_SUSPEND_IND

DESCRIPTION
  Post the suspend indication on the PS flow associated with the flow context.

PARAMETERS
  flow_context_p   - Flow Context information.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_context_ps_flow_suspend_ind
( 
  ds_flow_context_s *flow_context_p
)
{
  ps_flow_type         *ps_flow_ptr =NULL;
  ps_extended_info_code_enum_type ps_eic = PS_EIC_NOT_SPECIFIED;   
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --*/
  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return;
  }
  DS_3GPP_MSG1_HIGH("Indicate PS FLOW is suspended on flow context:0x%x",
                 flow_context_p->sdf_id);
  ps_flow_ptr = flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr;
  if(ps_flow_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("ps flow is NULL in flowSuspendInd");
    return;
  }
  ps_flow_suspend_ind(ps_flow_ptr,ps_eic); 
} /* ds_flow_context_ps_flow_suspend_ind */


/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_FILTERING_CONTROL

DESCRIPTION
  Disable or Enbale flow filtering control based on the input flag.

PARAMETERS
  flow_context_p   - Flow Context information.
  enable          - TRUE or FALSE

DEPENDENCIES
  None.

RETURN VALUE
  boolean: TRUE if the operation is successful.
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_context_ps_flow_filtering_control
( 
  ds_flow_context_s *flow_context_p,
  boolean           enable
)
{
  ps_flow_type         *ps_flow_ptr =NULL;
  int16 ps_errno = 0;   
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --*/
  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return FALSE;
  }
  DS_3GPP_MSG1_HIGH("Indicate PS FLOW filtering control on flow context:0x%x",
                 flow_context_p->sdf_id);
  ps_flow_ptr = flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr;
  if(ps_flow_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("ps flow is NULL in flowSuspendInd");
    return FALSE;
  }
  if(ps_iface_filtering_control(
                     &(flow_context_p->ds_flow_context_dyn_p->ds_iface_ptr->ps_iface),
                       flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr,
                       enable,
                       &ps_errno) != 0)
  {
    if(enable == TRUE)
    {
      DS_3GPP_MSG1_ERROR("error:%d while enable filtering on flow",ps_errno);
      return FALSE;
    }
    else
    {
      DS_3GPP_MSG1_ERROR("error%d while disable filtering on flow",ps_errno);
      return FALSE;
    }
  }
  return TRUE;
} /* ds_flow_context_ps_flow_filtering_control */

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_MODIFY_RESULT

DESCRIPTION
  Notify if the PS flow modify has been accepted or rejected.
  
PARAMETERS
  flow_context_p   - Flow Context information.
  result           - TRUE or FALSE
  ps_eic        - Error code indicating reason for reject.

DEPENDENCIES
  None.

RETURN VALUE
  boolean: TRUE if successful.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_context_ps_flow_modify_result
( 
  ds_flow_context_s *flow_context_p,
  boolean           result,
  ps_extended_info_code_enum_type ps_eic
)
{
  ps_flow_type         *ps_flow_ptr =NULL;
  int16 ps_errno = 0;   
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --*/
  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return FALSE;
  }
  DS_3GPP_MSG3_HIGH("Indicate PS FLOW Modify result on flow context:0x%x",
                 flow_context_p->sdf_id,0,0);
  ps_flow_ptr = flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr;
  if(ps_flow_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("ps flow is NULL in flowSuspendInd");
    return FALSE;
  }
  if(result == TRUE)
  {
    if(ps_iface_modify_flow_accepted(&(flow_context_p->ds_flow_context_dyn_p->
                                ds_iface_ptr->ps_iface),
                                flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr,
                                    ps_eic,
                                    &ps_errno)!= 0)
    {
      DS_3GPP_MSG1_ERROR("modify_flow_accepted failed:%d",ps_errno);
      return FALSE;
    }                  
  }
  else
  {
    if(ps_iface_modify_flow_rejected(&(flow_context_p->ds_flow_context_dyn_p->
                                  ds_iface_ptr->ps_iface),
                                  flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr,
                                    ps_eic,
                                    &ps_errno) != 0)
    {
      DS_3GPP_MSG1_ERROR("modify_flow_rejected failed:%d",ps_errno);
      return FALSE;
    }
  }
  return TRUE;
}/* ds_flow_context_ps_flow_modify_result*/

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_BIND_PHYS_LINK

DESCRIPTION
 Bind the PS flow associated with the flow context with the phys_link

PARAMETERS
  flow_context_p   - Flow Context information.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_context_ps_flow_bind_phys_link
( 
  ds_flow_context_s    *flow_context_p
)
{
  ps_flow_type         *ps_flow_ptr =NULL;
  ps_phys_link_type    *phys_link_ptr = NULL;
  ds_bearer_context_s  *bearer_context_p = NULL;
  ds_bearer_flow_manager_s *bearer_flow_mgr_p = NULL;
  int return_value = 0;  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --*/
  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return;
  }
  DS_3GPP_MSG1_HIGH("Bind flow to phys link on flow context:0x%x",
                 flow_context_p->sdf_id);
  ps_flow_ptr = flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr;
  if(ps_flow_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("ps flow is NULL in flowBindPhysLink");
    return;
  }
  bearer_flow_mgr_p =(ds_bearer_flow_manager_s *)
                       (flow_context_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr); 
  if(!ds_bearer_flow_manager_validate_flow_manager(bearer_flow_mgr_p))
  {
    return;
  }
  bearer_context_p = bearer_flow_mgr_p->ds_bearer_flow_manager_dyn_p->bearer_ptr;

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }
  phys_link_ptr = &(bearer_context_p->phys_link);
  return_value = ps_flow_bind_phys_link(ps_flow_ptr, phys_link_ptr);
  if(return_value == -1)
  {
    DS_3GPP_MSG0_HIGH("ps_flow_bind_phys_link returned -1");
  }
} /* ds_flow_context_ps_flow_bind_phys_link */


/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_SET_GRANTED_FLOW

DESCRIPTION
 Set the granted flow.

PARAMETERS
  flow_context_p   - Flow Context information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_context_ps_flow_set_granted_flow
( 
  ds_flow_context_s    *flow_context_p
)
{
  ps_flow_type         *ps_flow_ptr =NULL;
  ip_flow_type         *rx_flow_p = NULL;
  ip_flow_type         *tx_flow_p = NULL;
  ds_bearer_flow_manager_s *bearer_flow_mgr_p = NULL;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return;
  }
  DS_3GPP_MSG1_HIGH("Set Granted flow on flow context:0x%x",
                 flow_context_p->sdf_id);

  ps_flow_ptr = flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr;
  if(ps_flow_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("ps flow is NULL in FlowSetGrantedFlow");
    return;
  }
  bearer_flow_mgr_p =(ds_bearer_flow_manager_s *)
                   (flow_context_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr); 

  if(!ds_bearer_flow_manager_validate_flow_manager(bearer_flow_mgr_p))
  {
    return;
  }
  rx_flow_p = &(bearer_flow_mgr_p->ds_bearer_flow_manager_dyn_p->rx_flow);
  tx_flow_p = &(bearer_flow_mgr_p->ds_bearer_flow_manager_dyn_p->tx_flow);
  ps_flow_set_granted_flow(ps_flow_ptr, rx_flow_p, tx_flow_p);
} /*ds_flow_context_ps_flow_set_granted_flow*/


/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_PS_FLOW_GET_FILTERS

DESCRIPTION
  Retrieve the current filters assigned to the flow context.

PARAMETERS
  flow_context_p   - Flow Context information.
  modify           - boolean indicates current copy or modify set.
  app_fltr_spec    - Application filter spec to be populated.
DEPENDENCIES
  None.

RETURN VALUE
  boolean          - Returns true if filters are retrieved
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_context_ps_flow_get_filters
( 
 ds_flow_context_s    *flow_context_p,
 boolean               modify,
 qos_spec_type        *app_fltr_spec
)
{
  ip_flow_type *       granted_flow_p = NULL;
  int16                ps_errno;

  if((!ds_flow_context_validate_flow(flow_context_p)) || 
     (app_fltr_spec == NULL))
  {
    DS_3GPP_MSG1_ERROR("Input params are NULL, app_fltr_spec:0x%x",
                       app_fltr_spec);
    return FALSE;
  }

  /*------------------------------------------------------------------------
     Retrieve RX Filters
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------
    As we are using the same nw_bk/app_fltr_spec to detect conflict in tft. 
    We should reset the tx and rx list as ps framework just increment these 
    counters.
  ---------------------------------------------------------------------*/
  DS_3GPP_MSG1_HIGH("Before ds_flow_context_ps_flow_get_filters "
                    "rx.fltr_template.num_filters: %d resetting to 0",
                     app_fltr_spec->rx.fltr_template.num_filters);
  app_fltr_spec->rx.fltr_template.num_filters = 0;
  if(ps_flow_get_rx_fltr_spec(
                         &(flow_context_p->ds_flow_context_dyn_p->
                           ds_iface_ptr->ps_iface),
                          flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr,
                          &(app_fltr_spec->rx.fltr_template),
                          modify,
                          MAX_PACKET_FILTERS,
                          &ps_errno) == FALSE)
  {
    DS_3GPP_MSG0_HIGH("Error Retrieving RX filter_set");
    return FALSE;
  }
  if(app_fltr_spec->rx.fltr_template.num_filters > 0)
  {
    app_fltr_spec->field_mask |=(qos_spec_field_mask_type)QOS_MASK_RX_FLOW;
  }
  /*------------------------------------------------------------------------
     Retrieve TX Filters
  -------------------------------------------------------------------------*/
  DS_3GPP_MSG1_HIGH("Before ds_flow_context_ps_flow_get_filters "
                    "tx.fltr_template.num_filters: %d resetting to 0",
                     app_fltr_spec->tx.fltr_template.num_filters);
  app_fltr_spec->tx.fltr_template.num_filters = 0;
  if(ps_iface_flow_get_tx_fltr_spec(
                         &(flow_context_p->ds_flow_context_dyn_p->
                           ds_iface_ptr->ps_iface),
                          flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr,
                          &(app_fltr_spec->tx.fltr_template),
                          modify,
                          MAX_PACKET_FILTERS,
                          &ps_errno) == FALSE)
  {
    DS_3GPP_MSG0_HIGH("Error Retrieving TX filter_set");
    return FALSE;
  }
  if(app_fltr_spec->tx.fltr_template.num_filters > 0)
  {
    app_fltr_spec->field_mask |=(qos_spec_field_mask_type)QOS_MASK_TX_FLOW;
  }
  /*------------------------------------------------------------------------
     Retrieve RX Granted Flow
  -------------------------------------------------------------------------*/
  granted_flow_p= PS_FLOW_GET_RX_GRANTED_FLOW(flow_context_p->ds_flow_context_dyn_p
                                              ->ps_flow_ptr);
  if(granted_flow_p != NULL)
  {
    memscpy(&(app_fltr_spec->rx.flow_template.req_flow),
	    sizeof(ip_flow_type),
            granted_flow_p,
            sizeof(ip_flow_type));
    granted_flow_p = NULL;
  }
  else
  {
    DS_3GPP_MSG0_HIGH("Error retrieving RX flow set");
  }

  /*------------------------------------------------------------------------
     Retrieve TX Granted Flow
  -------------------------------------------------------------------------*/
  granted_flow_p= PS_FLOW_GET_TX_GRANTED_FLOW(flow_context_p->ds_flow_context_dyn_p->
                                              ps_flow_ptr);
  if(granted_flow_p != NULL)
  {
    memscpy(&(app_fltr_spec->tx.flow_template.req_flow),
	     sizeof((app_fltr_spec->tx.flow_template.req_flow)),
             granted_flow_p,
             sizeof(ip_flow_type));
  }
  else
  {
    DS_3GPP_MSG0_HIGH("Error retrieving RX flow set");
  }
  return TRUE;
}/* ds_flow_context_ps_flow_get_filters */

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_FIND_FLOW_CONTEXT

DESCRIPTION
 Find the flow context associated with the PS FLOW

PARAMETERS
  ps_flow_p     -  PS Flow information.
   
DEPENDENCIES
  None.

RETURN VALUE
  Flow context Ptr

SIDE EFFECTS
  None.

===========================================================================*/
ds_flow_context_s *ds_flow_context_find_flow_context
( 
  ps_flow_type     *ps_flow_p
)
  {
  ds_flow_context_s        *flow_context_p =NULL;
  int index = 0;  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --*/
  if(ps_flow_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("ps flow ptr is NULL in findFlowContext");
    return flow_context_p;
  }

  /*-------------------------------------------------------------------
     Go through the array and find the flow context.
  -------------------------------------------------------------------*/
  for (index = 0; index < DS_3GPP_MAX_FLOW_CONTEXT; index++)
  {
    flow_context_p = &(ds_flow_context_tbl[index]);
    if(ds_flow_context_validate_flow(flow_context_p) &&
        (flow_context_p->ds_flow_context_dyn_p->ps_flow_ptr == ps_flow_p))
    {
      DS_3GPP_MSG0_HIGH("Flow context found for the ps flow");
      return flow_context_p;
    }
  }

  DS_3GPP_MSG1_HIGH("Flow Context not found for ps flow:0x%x",ps_flow_p);
  flow_context_p = NULL;
  return flow_context_p;
}/* ds_flow_context_find_flow_context*/


/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_UPDATE_FILTER_IDENTIFIER_BIT_MASK

DESCRIPTION
  Update requested filter identifier bit mask.

PARAMETERS
 flow_context_p      - Pointer to flow context.
 set_identifier      - If true, set the bit else clear the bit.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_context_update_filter_identifier_bit_mask
( 
  ds_flow_context_s    *flow_context_p,
  boolean              set_identifier
)
{
  qos_spec_type             orig_spec;  
  ip_filter_type            *fltr = NULL; 
  int                       cnt = 0;          
  uint16                    identifier = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_flow_context_validate_flow(flow_context_p))
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
    /*---------------------------------------------------------------------
      NW Assigned filter id is in the last 4 bits.
    ---------------------------------------------------------------------*/
    identifier = (fltr->ipfltr_aux_info.fi_id)&(0xF);
    if(set_identifier)
    {
      /*-------------------------------------------------------------------
        Set the Requested Filter Identifier for Rx
      -------------------------------------------------------------------*/
      ds_3gpp_tft_set_filter_identifier_bit(
                                 identifier,
                   &(flow_context_p->ds_flow_context_dyn_p->fi_identifier));     
      }
    else
    {
      /*-------------------------------------------------------------------
        Clear the Filter Identifier for Rx
      -------------------------------------------------------------------*/
      ds_3gpp_tft_clear_filter_identifier_bit(
                                 identifier,
                    &(flow_context_p->ds_flow_context_dyn_p->fi_identifier));  
    }
  }/* RX FOR Loop*/
  /*------------------------------------------------------------------------
     Loop thru all filters and update the filter identifier bit mask for TX
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt<orig_spec.tx.fltr_template.num_filters; cnt++)
  {
    fltr = &(orig_spec.tx.fltr_template.list_ptr[cnt]);
    /*---------------------------------------------------------------------
      NW Assigned filter id is in the last 4 bits.
    ---------------------------------------------------------------------*/
    identifier = (fltr->ipfltr_aux_info.fi_id&(0xF));
    if(set_identifier)
    {
      /*-------------------------------------------------------------------
        Set the Requested Filter Identifier for Tx
      -------------------------------------------------------------------*/
      ds_3gpp_tft_set_filter_identifier_bit(
                                 identifier,
                   &(flow_context_p->ds_flow_context_dyn_p->fi_identifier));     
    }
    else
    {
      /*-------------------------------------------------------------------
        Clear the Filter Identifier for Tx
      -------------------------------------------------------------------*/
      ds_3gpp_tft_clear_filter_identifier_bit(
                                 identifier,
                  &(flow_context_p->ds_flow_context_dyn_p->fi_identifier));  
    }
  }/* FOR TX  Filters */

  modem_mem_free((void *)orig_spec.rx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);
  modem_mem_free((void *)orig_spec.tx.fltr_template.list_ptr, MODEM_MEM_CLIENT_DATA);

  return TRUE; 
}/* ds_flow_context_update_filter_identifier_bit_mask */

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_DELETE_FILTER_FROM_FLOW

DESCRIPTION
  Removes the filter from the passed ps qos spec.

PARAMETERS
  flow_context_p   - Flow Context information.
  filter_id        - Filter to be removed.
  app_qos_spec    - filter spec from which the filter has to be deleted.
  modified_qos_spec  - filter spec which has the modified filter changes
DEPENDENCIES
  None.

RETURN VALUE
  boolean          - Returns true if filters are retrieved
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_context_delete_filter_from_flow
( 
  ds_flow_context_s    *flow_context_p,
  uint32               filter_id,
  qos_spec_type        *app_qos_spec,
  qos_spec_type        *modified_qos_spec
)
{
  ip_filter_type            *fltr = NULL, *mod_fltr=NULL; 
  int                       cnt = 0, mod_cnt=0;          
  uint32                    identifier = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if((flow_context_p == NULL)||(app_qos_spec == NULL)||(filter_id==0)||
      (modified_qos_spec == NULL))
  {
    DS_3GPP_MSG0_ERROR("invalid input params");
    return FALSE;
  }
  
  /*------------------------------------------------------------------------
    Loop thru all filters and update the filter identifier bit mask for RX
  -------------------------------------------------------------------------*/
  for(cnt =0; cnt<app_qos_spec->rx.fltr_template.num_filters; cnt++)
  {
    fltr = &(app_qos_spec->rx.fltr_template.list_ptr[cnt]);
    mod_fltr = &(modified_qos_spec->rx.fltr_template.list_ptr[mod_cnt]);
    /*---------------------------------------------------------------------
      NW Assigned filter id is in the last 4 bits.
    ---------------------------------------------------------------------*/
    identifier = (1 << (fltr->ipfltr_aux_info.fi_id & (0xF)));
    if((identifier & filter_id) == identifier)
    {
      /* delete the filter from Qos spec */
      DS_3GPP_MSG1_HIGH("Deleting filter id %d from Rx Flow",identifier); 
    }
    else
    {
      memscpy(mod_fltr,sizeof(ip_filter_type), fltr, sizeof(ip_filter_type));
      mod_cnt++;
     
    }
  }/* RX FOR Loop*/
  /*---------------------------------------------------------------------
    Update the filter count
  ---------------------------------------------------------------------*/
  modified_qos_spec->rx.fltr_template.num_filters = mod_cnt;

  /*------------------------------------------------------------------------
    Loop thru all filters and update the filter identifier bit mask for TX
  -------------------------------------------------------------------------*/
  mod_cnt = 0;
  for(cnt =0; cnt<app_qos_spec->tx.fltr_template.num_filters; cnt++)
  {
    fltr = &(app_qos_spec->tx.fltr_template.list_ptr[cnt]);
    mod_fltr = &(modified_qos_spec->tx.fltr_template.list_ptr[mod_cnt]);
    /*---------------------------------------------------------------------
      NW Assigned filter id is in the last 4 bits.
    ---------------------------------------------------------------------*/
    identifier = (1 << (fltr->ipfltr_aux_info.fi_id & (0xF)));
    if((identifier & filter_id) == identifier)
    {
      /* delete the filter from Qos spec */
      DS_3GPP_MSG1_HIGH("Deleting filter id %d from Tx Flow",identifier);
    }
    else
    {
      memscpy(mod_fltr,sizeof(ip_filter_type), fltr, sizeof(ip_filter_type));
      mod_cnt++;
    }
  }/* TX FOR Loop*/
  /*---------------------------------------------------------------------
    Update the filter count
  ---------------------------------------------------------------------*/
  modified_qos_spec->tx.fltr_template.num_filters = mod_cnt;

  return TRUE;
}/* ds_flow_context_delete_filter_from_flow */

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_COMPARE_FILTERS

DESCRIPTION
  Compares the two input filters

PARAMETERS
  filter1             -  Filter1 to be compared.
  filter2             -  Filter2 to be compared.

DEPENDENCIES
  None.

RETURN VALUE
boolean                - returns FALSE if comparision fails.
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
boolean ds_flow_context_compare_filters
(
  ip_filter_type            *filter1,
  ip_filter_type            *filter2
)
{
  /*-----------------------------------------------------------------------*/
  if ((filter1 ==NULL) || (filter2 == NULL))
  {
    DS_3GPP_MSG2_ERROR("Input params are null filter1:0x%x,filter2:0x%x",
                  filter1,filter2);
    return FALSE;
  }
  if((memcmp(&(filter1->ip_hdr),
                   &(filter2->ip_hdr),
                  sizeof(filter1->ip_hdr)) == 0) &&

          (memcmp(&(filter1->next_prot_hdr),
                   &(filter2->next_prot_hdr),
                  sizeof(filter1->next_prot_hdr)) == 0) &&

           (filter1->ip_vsn == filter2->ip_vsn) )
  {
    return TRUE;
  }
  return FALSE;
}/* ds_flow_context_compare_filters */

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_UPDATE_FILTER_IDENTIFIER

DESCRIPTION
  Compare the App filter spec to NW returned filter spec.
  This will also update the filter identifier and filter precedence.

PARAMETERS
 app_filter_spec       -  App requested filter spec.
 nw_filter_spec        -  NW granted filter speco.

DEPENDENCIES
  None.

RETURN VALUE
boolean                - returns FALSE if comparision fails.
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
boolean ds_flow_context_update_filter_identifier(
  ip_filter_spec_type            *app_filter_spec,
  ip_filter_spec_type            *nw_filter_spec
)
{
  int compared_filter_list = 0;
  int loop =0, outer_loop =0, nw_fltr_cnt=0;
  ip_filter_type *nw_filter = NULL;
  ip_filter_type *app_filter = NULL;
  boolean filter_found = FALSE;
  /*-----------------------------------------------------------------------*/
  if((app_filter_spec == NULL) || (nw_filter_spec == NULL))
  {
    DS_3GPP_MSG2_ERROR("Invalid input. Null params. app_spec:0x%x,nw_spec:0x%x",
                app_filter_spec, nw_filter_spec);
    return FALSE;
  }

  for(outer_loop =0; outer_loop<app_filter_spec->num_filters; outer_loop++)
  {
    app_filter = &(app_filter_spec->list_ptr[outer_loop]);
    /*-------------------------------------------------------------------
      Compare each App filter with all the NW filter
    -------------------------------------------------------------------*/      
    filter_found = FALSE;    
    for(loop =0; loop < nw_filter_spec->num_filters; loop++)
    {
      nw_filter = &(nw_filter_spec->list_ptr[loop]);
      if( (compared_filter_list & (1<<loop)) != (1 << loop))
      {
       /*-------------------------------------------------------------------
        Compare App filter with NW filter not in comparision bit mask
       -------------------------------------------------------------------*/
        if(ds_flow_context_compare_filters(app_filter,nw_filter) == TRUE)
        {
          /*-------------------------------------------------------------------
            Update bit mask to avoid comparing this NW filter.
            Also update filter identifier and filter precedence.
          -------------------------------------------------------------------*/
          if(app_filter->ipfltr_aux_info.fi_precedence ==
                          nw_filter->ipfltr_aux_info.fi_precedence)
          {
            filter_found = TRUE;
            app_filter->ipfltr_aux_info.fi_id = 
                             nw_filter->ipfltr_aux_info.fi_id;
            compared_filter_list |= 1 << loop;
            nw_fltr_cnt++;
          }/* if precedence == equal */
        }/* compareFilters is success */
      } /* compared_filter_list is not set*/
    }/* loop*/
    /*-------------------------------------------------------------------
       Check bit mask to verify the requested app filter was present
    -------------------------------------------------------------------*/
    if(!filter_found)
    {
      DS_3GPP_MSG1_HIGH("App requested filter %d not found",
                                 app_filter->ipfltr_aux_info.fi_id);
    }
  } /* outer loop*/
  if(nw_fltr_cnt != nw_filter_spec->num_filters)
  { 
    DS_3GPP_MSG0_ERROR("Network sent more filters than UE requested");
    return TRUE;
  }
  return TRUE;
}/*ds_flow_context_update_filter_identifier*/


/*===========================================================================
FUNCTION ds_flow_context_retrieve_iface_ip_type

DESCRIPTION
  Retrieve the PS iface and PS IP Type from the flow context.

PARAMETERS
 flow_context_p        - Pointer to flow context.
 ip_type               - Pointer to IP type. Filled in by function.
 iface_ptr             - Pointer to the Ps iface. filled in by function.

DEPENDENCIES
  None.

RETURN VALUE
 boolean                - Return True/False
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
boolean  ds_flow_context_retrieve_iface_ip_type
(
  ds_flow_context_s      *flow_context_p,
  ip_version_enum_type   *ip_type,
  ps_iface_type           **iface_ptr
)
{
  /*-----------------------------------------------------------------------*/
  if(ip_type==NULL||
     (!ds_flow_context_validate_flow(flow_context_p)))
  {
    DS_3GPP_MSG2_ERROR("Invalid input.flow:0x%x,ip:0x%x",flow_context_p,ip_type);
    return FALSE;  
  }

  if(flow_context_p->ds_flow_context_dyn_p->ds_iface_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("3GPP iface is not present");
    return FALSE;
  }
  *iface_ptr = &(flow_context_p->ds_flow_context_dyn_p->ds_iface_ptr->ps_iface);
  if(*iface_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("PS iface is not present");
    return FALSE;
  }
  
  if(flow_context_p->ds_flow_context_dyn_p->flow_type & DS_3GPP_V4_FLOW_TYPE)
  {
    *ip_type = IP_V4;
  }
  else if(flow_context_p->ds_flow_context_dyn_p->flow_type & DS_3GPP_V6_FLOW_TYPE)
  {
    *ip_type = IP_V6;
  }
  else
  {
    DS_3GPP_MSG0_ERROR("flow context flow type is not configured");
    return FALSE;
  }
  return TRUE;
}/* ds_flow_context_retrieve_iface_ip_type */

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_SET_FLOW_STATE

DESCRIPTION
 Set the flow state.

PARAMETERS
  flow_context_p   - Flow Context information.
  new_state            - New Flow State.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_context_set_flow_state
( 
  ds_flow_context_s        *flow_context_p,
  ds_flow_context_state_e  new_state
)
{
  /*-----------------------------------------------------------------------*/
  if(!ds_flow_context_validate_flow(flow_context_p))
  {
    return;  
  }
  
  DS_3GPP_MSG3_HIGH("Flow state change for 0x:%x old_state 0x:%x new_state 0x:%x",
              flow_context_p,flow_context_p->ds_flow_context_dyn_p->state,new_state);
  flow_context_p->ds_flow_context_dyn_p->state = new_state;
  return;
}/* ds_flow_context_set_flow_state */

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_CHECK_DUPL_CID

DESCRIPTION
  This function is invoked to check if a duplicate cid is present.

PARAMETERS
  cid    -   dynamic cid number

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns TRUE if duplicate is present , FALSE otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean ds_flow_context_check_dupl_cid
(
  uint32  cid
)
{
  uint8 loop =0;

  for(loop=0;(loop < DS_3GPP_MAX_FLOW_CONTEXT );loop++)
  {
    if((ds_flow_context_validate_flow(&ds_flow_context_tbl[loop])) && 
        (cid == ds_flow_context_tbl[loop].ds_flow_context_dyn_p->cid) )
    {
      return TRUE;
    }
  }
  return FALSE;
}/* ds_flow_context_check_dupl_cid */

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_ALLOC_DYNAMIC_CID

DESCRIPTION
  This function is invoked to allocate a dynamic cid number for a 
  flow request.

PARAMETERS
  cid_p    -  ptr to dynamic cid number

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns TRUE if success, FALSE otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_context_alloc_dynamic_cid
(
  uint32* cid_p
)
{

#define INC_DYN_CID(x)     \
      if (x == DS_FLOW_DYNAMIC_PROFILE_MAX) {\
      x = DS_FLOW_DYNAMIC_PROFILE_BASE;}else {\
      x =(x+1);}

  uint8 loop =0;
  static uint32 dyn_cid =DS_FLOW_DYNAMIC_PROFILE_BASE;

  ASSERT(cid_p != NULL);
  
  for(loop=0;(loop < DS_3GPP_MAX_FLOW_CONTEXT );loop++)
  {
    if (TRUE == ds_flow_context_check_dupl_cid(dyn_cid))
    {
      DS_3GPP_MSG1_HIGH("dynamic cid %d is already in use",dyn_cid);
      INC_DYN_CID(dyn_cid)
    }
    else
    {
      break;
    }
  }
  *cid_p = dyn_cid;

  /* to allocate dynamic cid's in a cycle */
  INC_DYN_CID(dyn_cid)
  return TRUE;
}/* ds_flow_context_alloc_dynamic_cid */

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_UPDATE_DYNAMIC_CID

DESCRIPTION
  This function is invoked to update a dynamic cid number for a 
  flow request.

PARAMETERS
  flow_p      - flow pointer whose dynamic cid needs to be updated.
  cid    -  cid number associated with the flow p

DEPENDENCIES
  None.

RETURN VALUE
  boolean  - Returns TRUE if success, FALSE otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_context_update_dynamic_cid
(
  ps_flow_type *            flow_p,
  uint32                    cid
)
{
  uint8 loop =0;

  ASSERT(flow_p != NULL);

  for(loop=0;(loop < DS_3GPP_MAX_FLOW_CONTEXT );loop++)
  {
    if((ds_flow_context_validate_flow(&ds_flow_context_tbl[loop])) &&
     (flow_p == ds_flow_context_tbl[loop].ds_flow_context_dyn_p->ps_flow_ptr))
    {
      /* update the cid number */
      ds_flow_context_tbl[loop].ds_flow_context_dyn_p->cid = cid;
      return TRUE;
    }
  }
  /* No valid flow pointer is available, return false */
  DS_3GPP_MSG1_ERROR("Not a valid cid %d ,return false ",cid);
  return FALSE;
  
}/* ds_flow_manager_update_dynamic_cid */

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_GET_ALL_CIDS

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
void ds_flow_context_get_all_cids
(
  ds_3gpp_atif_flow_cid_list_type*   cid_list_ptr
)
{
  uint8 loop = 0;

  ASSERT(cid_list_ptr != NULL);
  memset (cid_list_ptr,0x0,sizeof(ds_3gpp_atif_flow_cid_list_type));
  
  for(loop=0;loop <DS_3GPP_MAX_FLOW_CONTEXT;loop++)
  {
    if(ds_flow_context_validate_flow(&ds_flow_context_tbl[loop]))  
    {
      cid_list_ptr->data_ptr[cid_list_ptr->data_len++] = 
          ds_flow_context_tbl[loop].ds_flow_context_dyn_p->cid;
    }
  }
}/* ds_flow_context_get_all_cids */

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_GET_ATI_DYNAMIC_INFO

DESCRIPTION
  This function returns the state of the flow 
  a. flow pointer.
  b. associated primary profile
  c. bearer id (to do send nsapi )
  
PARAMETERS   
  profile_num  - secondary profile number whose state info needs to retrieved.
  
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE -- if valid flow state is found
  FALSE -- if no valid flow is present.

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_flow_context_get_ati_dynamic_info
(
  uint32                        cid,
  sys_modem_as_id_e_type        subs_id,
  ds_flow_dynamic_info_s*   flow_ati_info_p
)
{

  ds_bearer_flow_manager_s *bearer_flow_mgr_p = NULL;
  ds_bearer_context_s * bearer_context_p = NULL;
  ds_pdn_context_s     *pdn_ptr = NULL;
#ifdef FEATURE_DATA_LTE
  ds_eps_bearer_context_s * eps_bearer_cntx_p = NULL;
#endif /* FEATURE_DATA_LTE */
  ds_flow_context_s   *flow_ctxt_p = NULL;
  boolean              result = TRUE;
  sys_sys_mode_e_type  current_mode = SYS_SYS_MODE_LTE;
/*---------------------------------------------------------------------------*/  
  ASSERT(flow_ati_info_p != NULL);

  flow_ctxt_p = ds_flow_context_get_flow_context_from_cid(cid, subs_id);

  if(!ds_flow_context_validate_flow(flow_ctxt_p))
  {
    DS_3GPP_MSG0_ERROR("Not a valid profile,return false ");
    return FALSE;
  }

  /* get flow bearer context */
  bearer_flow_mgr_p = (ds_bearer_flow_manager_s*)\
                    flow_ctxt_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr;

  if(!ds_bearer_flow_manager_validate_flow_manager(bearer_flow_mgr_p))
  {
    return FALSE;
  }
  /* get the bearer context */
  bearer_context_p  = bearer_flow_mgr_p->ds_bearer_flow_manager_dyn_p
    ->bearer_ptr;

  if(ds_bearer_cntx_validate_bearer_context(bearer_context_p) == FALSE)
  {
    ASSERT(0);
    return FALSE;
  }
  
  /* get the pdn context */
  pdn_ptr = bearer_flow_mgr_p->ds_bearer_flow_manager_dyn_p->pdn_ptr;
  if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_ptr) == FALSE)
  {
    ASSERT(0);
    return FALSE;
  }

  current_mode = ds_3gpp_get_apn_current_mode (pdn_ptr);
  
#ifdef FEATURE_DATA_LTE
  if ( SYS_SYS_MODE_LTE == current_mode )
  {
    eps_bearer_cntx_p = (ds_eps_bearer_context_s *)\
                   (bearer_context_p->ds_bearer_context_dyn_p->
                    vobj_data_ptr_tbl[DS_3GPP_EPS_VOBJ]);

    if(ds_eps_bearer_cntxt_validate_bearer_cntxt(eps_bearer_cntx_p) == FALSE)
    {
      ASSERT(0);
      return FALSE;
    }

    flow_ati_info_p->bearer_id = eps_bearer_cntx_p->ds_eps_bearer_context_dyn_p
      ->eps_bearer_id;
  }
  else 
#endif /* FEATURE_DATA_LTE */
  if( (current_mode == SYS_SYS_MODE_WCDMA) ||
           (current_mode == SYS_SYS_MODE_GSM) )
  {
    flow_ati_info_p->bearer_id = bearer_context_p->ds_bearer_context_dyn_p
                                                                  ->nsapi;
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Invalid network mode %d",(int)current_mode);
    return FALSE;
  }
  /* update Associated profile id */
  flow_ati_info_p->p_cid = pdn_ptr->ds_pdn_context_dyn_p->pdp_profile_num;

  /* Update flow pointer */
  flow_ati_info_p->ps_flow_ptr = flow_ctxt_p->
                                 ds_flow_context_dyn_p->ps_flow_ptr;

  /* get APP spec from flow context */
  result =  ds_flow_context_ps_flow_get_filters(flow_ctxt_p,
                                  FALSE,
                                  &(flow_ati_info_p->qos_spec));
  if(result == TRUE)
  {
    flow_ati_info_p->flow_type = flow_ctxt_p->ds_flow_context_dyn_p->flow_type;
  }
  
  return result;
}/* ds_flow_context_get_ati_dynamic_info */


/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_GET_ATI_DYNAMIC_INFO_FOR_DDS

DESCRIPTION
  This function returns the state of the flow 
  a. flow pointer.
  b. associated primary profile
  c. bearer id (to do send nsapi )
  
PARAMETERS   
  profile_num  - secondary profile number whose state info needs to retrieved.
  
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE -- if valid flow state is found
  FALSE -- if no valid flow is present.

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_flow_context_get_ati_dynamic_info_for_dds
(
  uint32                        cid,
  ds_flow_dynamic_info_s*   flow_ati_info_p
)
{

  ds_bearer_flow_manager_s *bearer_flow_mgr_p = NULL;
  ds_bearer_context_s * bearer_context_p = NULL;
  ds_pdn_context_s     *pdn_ptr = NULL;
#ifdef FEATURE_DATA_LTE
  ds_eps_bearer_context_s * eps_bearer_cntx_p = NULL;
#endif /* FEATURE_DATA_LTE */
  ds_flow_context_s   *flow_ctxt_p = NULL;
  boolean              result = TRUE;
  sys_sys_mode_e_type  current_mode = SYS_SYS_MODE_LTE;
  sys_modem_as_id_e_type     subs_id = ds3g_get_ps_subs_id();
/*---------------------------------------------------------------------------*/  
  ASSERT(flow_ati_info_p != NULL);

  flow_ctxt_p = ds_flow_context_get_flow_context_from_cid(cid, subs_id);

  if(!ds_flow_context_validate_flow(flow_ctxt_p))
  {
    return FALSE;
  }

  /* get flow bearer context */
  bearer_flow_mgr_p = (ds_bearer_flow_manager_s*)\
                    flow_ctxt_p->ds_flow_context_dyn_p->bearer_flow_mgr_ptr;

  if(!ds_bearer_flow_manager_validate_flow_manager(bearer_flow_mgr_p))
  {
    return FALSE;
  }
  /* get the bearer context */
  bearer_context_p  = bearer_flow_mgr_p->ds_bearer_flow_manager_dyn_p
    ->bearer_ptr;

  if(ds_bearer_cntx_validate_bearer_context(bearer_context_p) == FALSE)
  {
    ASSERT(0);
    return FALSE;
  }
  
  /* get the pdn context */
  pdn_ptr = bearer_flow_mgr_p->ds_bearer_flow_manager_dyn_p->pdn_ptr;
  if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_ptr) == FALSE)
  {
    ASSERT(0);
    return FALSE;
  }

  current_mode = ds_3gpp_get_apn_current_mode (pdn_ptr);
  
#ifdef FEATURE_DATA_LTE
  if ( SYS_SYS_MODE_LTE == current_mode )
  {
    eps_bearer_cntx_p = (ds_eps_bearer_context_s *)\
                   (bearer_context_p->ds_bearer_context_dyn_p->
                    vobj_data_ptr_tbl[DS_3GPP_EPS_VOBJ]);

    if(ds_eps_bearer_cntxt_validate_bearer_cntxt(eps_bearer_cntx_p) == FALSE)
    {
      ASSERT(0);
      return FALSE;
    }

    flow_ati_info_p->bearer_id = eps_bearer_cntx_p->ds_eps_bearer_context_dyn_p
      ->eps_bearer_id;
  }
  else 
#endif /* FEATURE_DATA_LTE */
  if( (current_mode == SYS_SYS_MODE_WCDMA) ||
           (current_mode == SYS_SYS_MODE_GSM) )
  {
    flow_ati_info_p->bearer_id = bearer_context_p->ds_bearer_context_dyn_p
                                                                  ->nsapi;
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Invalid network mode %d",(int)current_mode);
    return FALSE;
  }
  /* update Associated profile id */
  flow_ati_info_p->p_cid = pdn_ptr->ds_pdn_context_dyn_p->pdp_profile_num;

  /* Update flow pointer */
  flow_ati_info_p->ps_flow_ptr = flow_ctxt_p->
                                 ds_flow_context_dyn_p->ps_flow_ptr;

  /* get APP spec from flow context */
  result =  ds_flow_context_ps_flow_get_filters(flow_ctxt_p,
                                  FALSE,
                                  &(flow_ati_info_p->qos_spec));
  if(result == TRUE)
  {
    flow_ati_info_p->flow_type = flow_ctxt_p->ds_flow_context_dyn_p->flow_type;
  }
  
  return result;
}/* ds_flow_context_get_ati_dynamic_info_for_dds */

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_GET_FLOW_CONTEXT_FROM_CID

DESCRIPTION
  Retrieve the flow context from context id.

PARAMETERS
  cid              - context id.
.
DEPENDENCIES
  None.

RETURN VALUE
  NULL          - Returns NULL if no valid flow pointers
                else valid flow_pointer.
  
SIDE EFFECTS
  None.

===========================================================================*/
LOCAL ds_flow_context_s* ds_flow_context_get_flow_context_from_cid
( 
  uint32                   cid,
  sys_modem_as_id_e_type   subs_id
)
{
  uint8 loop = 0;
  ds_flow_context_s   *flow_cntxt_p = NULL;
  ds_bearer_flow_manager_s *bearer_flow_mgr_p = NULL;
  sys_modem_as_id_e_type local_subs_id = SYS_MODEM_AS_ID_NONE;

  for(loop =0;loop<DS_3GPP_MAX_FLOW_CONTEXT;loop++)
  {
    if(ds_flow_context_validate_flow(&ds_flow_context_tbl[loop]) &&
        (cid == ds_flow_context_tbl[loop].ds_flow_context_dyn_p->cid))
    {
      bearer_flow_mgr_p = (ds_bearer_flow_manager_s*)
          ds_flow_context_tbl[loop].ds_flow_context_dyn_p->bearer_flow_mgr_ptr;
      if(!ds_bearer_flow_manager_validate_flow_manager(bearer_flow_mgr_p))
      {
        continue;
      } 
      if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_flow_mgr_p->
                      ds_bearer_flow_manager_dyn_p->bearer_ptr, &local_subs_id))
      {
        continue;
      }
      if(local_subs_id == subs_id)
      {
        flow_cntxt_p = &ds_flow_context_tbl[loop];
        break;
      }
    }
  }
  return flow_cntxt_p;
}/* ds_flow_context_get_flow_context_from_cid */

/*===========================================================================
FUNCTION ds_flow_context_validate_flow

DESCRIPTION
  This function checks if an index in the Flow context tbl has been allocated
  memory

PARAMETERS
  flow_context_p - flow_context to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_flow_context_validate_flow
(
  ds_flow_context_s *flow_context_p
)
{
  boolean ret_val = FALSE;
  ds_flow_context_dyn_s *flow_context_dyn_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(flow_context_p == NULL)
  {
    DS_3GPP_MSG0_LOW("Input parameter flow_context is NULL");
    return ret_val;
  }

  flow_context_dyn_p = flow_context_p->ds_flow_context_dyn_p;

  if((flow_context_dyn_p != NULL) && 
     (flow_context_dyn_p->cookie == FLOW_CONTEXT_VALID_COOKIE))
  {
    ret_val = TRUE;
    DS_3GPP_MSG1_LOW("Flow context is valid at index:%d",
                     flow_context_dyn_p->index);
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Flow context:0x%x is INVALID",
                       flow_context_p);
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
LOCAL boolean ds_flow_context_alloc_dyn_mem
(
  int index
)
{
  boolean ret_val = FALSE;
  ds_flow_context_dyn_s *flow_context_dyn_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((index < 0) || (index >= DS_3GPP_MAX_FLOW_CONTEXT))
  {
    DS_3GPP_MSG1_ERROR("Index:%d is out of bounds",index);
    return ret_val;
  }

    flow_context_dyn_p = ds_flow_context_tbl[index].ds_flow_context_dyn_p;

    if(flow_context_dyn_p == NULL)
    {
      flow_context_dyn_p = (ds_flow_context_dyn_s *)modem_mem_alloc
        (sizeof(ds_flow_context_dyn_s),MODEM_MEM_CLIENT_DATA);

      if (flow_context_dyn_p != NULL)
      {
        DS_3GPP_MSG3_MED("Dynamic memory allocation at index %d successful",index,0,0);
        memset(flow_context_dyn_p,0,sizeof(ds_flow_context_dyn_s));
        flow_context_dyn_p->cookie = FLOW_CONTEXT_VALID_COOKIE;
        flow_context_dyn_p->index = index;
        ds_flow_context_tbl[index].ds_flow_context_dyn_p = flow_context_dyn_p;
        ret_val = TRUE;
      }
      else
      {
        DS_3GPP_MSG0_ERROR("Failed to allocate memory");
      }
    }
    else
    {
      DS_3GPP_MSG1_ERROR("Flow context is already allocated at index:%d",index);
    }


    return ret_val;
}


/*===========================================================================
FUNCTION ds_flow_context_free_dyn_mem

DESCRIPTION
  This function frees dyn memory.
 
PARAMETERS
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

=============================================================================*/
void ds_flow_context_free_dyn_mem
( 
   ds_flow_context_s  *flow_context_p
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((flow_context_p == NULL) || 
     (flow_context_p->ds_flow_context_dyn_p == NULL))
  {
    DS_3GPP_MSG0_ERROR("Flow context passed is invalid");
    return;
  }
    
  modem_mem_free(flow_context_p->ds_flow_context_dyn_p,MODEM_MEM_CLIENT_DATA);
  flow_context_p->ds_flow_context_dyn_p = NULL;
}

#endif /* FEATURE_DATA_3GPP */
