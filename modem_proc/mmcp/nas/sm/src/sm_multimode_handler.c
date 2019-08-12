/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001,2002,2003,2010 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/sm_multimode_handler.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/14/11   rajesh  Added new function "sm_esm_isr_status_set"
08/04/11   zren    Updated ESM to propagate SM group ID to CM for befault Bearer
                   context activation
03/25/10   MNK     i-RAT updates
02/16/10   MNK     First cut of i-RAT between LTE and GW

===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_LTE
#include "amssassert.h"
#include "sm_multimode_common.h"
#include "esm_bcm.h"
#include <stringl/stringl.h>
#include "cmwll.h"
#include "cfa_v.h"
#include "mm.h"
#include "esm_emm_msgr_enum_msg.h"
#include "esm_utils.h"
#include "esm.h"
#include "esm_cm_msgr_enum_msg.h" 

#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE))
 #error code not present
#endif // TEST_FRAMEWORK

#ifdef FEATURE_DUAL_DATA
sm_deact_isr_on_interrat_status_T sm_deact_isr_on_interrat_status_sim[MAX_AS_IDS];
#define sm_deact_isr_on_interrat_status sm_deact_isr_on_interrat_status_sim[sm_as_id]
#else
sm_deact_isr_on_interrat_status_T sm_deact_isr_on_interrat_status = SM_DEACT_ISR_ON_INTERRAT_NONE;
#endif

#ifdef FEATURE_DUAL_SIM
subs_capability_e_type mm_get_sub_capability(sys_modem_as_id_e_type sub_id);
extern boolean SM_IS_VALID_AS_ID(sys_modem_as_id_e_type cmd_as_id);
#endif

extern void sm_reset_isr_pdp_act_status 
(
  void
);

extern esm_bearer_resource_proc_T*  esm_bpm_find_uicc_proc_state_machine(esm_bpm_T* mgr_ptr, byte pti);

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*===========================================================================

FUNCTION  SM_UPDATE_SM_CONTEXT_FROM_ESM

DESCRIPTION
  This function update corresponding SM PDP context upon bearer context 
  activation and deactivation in ESM

DEPENDENCIES
  None

RETURN VALUE
  SM Group ID 

SIDE EFFECTS
  None

===========================================================================*/
byte sm_update_sm_context_from_esm
(
  const esm_eps_bearer_context_T  *esm_bc,
  esm_bc_state_T                   bc_state 
)
{
  byte conn_id; 
  
  if(esm_bc == NULL)
  {
    MSG_ERROR_DS_0(SM_SUB,"ESM context pointer NULL");
    return INVALID_ID;   
  }
  if(esm_bc->ti.valid == FALSE || esm_bc->ti.connection_id >= MAX_PDP_NUM
     || esm_bc->neg_qos.valid == FALSE)
  {
    MSG_HIGH_DS_1(SM_SUB,"ESM: TI/NEG QOS not valid w/ BC %d - Not updating SM context",
              esm_bc->eps_bearer_id);    
    return INVALID_ID;
  }
  
  conn_id = esm_bc->ti.connection_id;

  if(bc_state == ESM_BC_ACTIVE)
  {
    byte index;
    MSG_HIGH_DS_2(SM_SUB,"ESM: Creating SM context for BID %d w/ Conn ID (TI) %d", 
             esm_bc->eps_bearer_id, conn_id);

    /* New context activation, allocate memory and NSAPI*/
    if(pdp_list[conn_id] == NULL)
    {
      //if NSAPI is already used by another context then this is error.
      if(nsapi_list[esm_bc->eps_bearer_id] != INVALID_ID)
      {
        MSG_ERROR_DS_0(SM_SUB,"NSAPI used by another context, so not updating SM context");        
        return INVALID_ID;
      }       
      nsapi_list[esm_bc->eps_bearer_id] = conn_id;
      
      (void)sm_get_connection_id_int(conn_id);          
            
      if(pdp_list[conn_id] == NULL)
      {
        MSG_ERROR_DS_1(SM_SUB,"Memory can not allocated for SM context %d", conn_id);
        nsapi_list[esm_bc->eps_bearer_id] = INVALID_ID;
        return INVALID_ID;
      }  
      pdp_list[conn_id]->trans_id = conn_id;
      
      pdp_list[conn_id]->nsapi.valid = TRUE;
      pdp_list[conn_id]->nsapi.nsapi = esm_bc->eps_bearer_id;
      
      // Primary PDP activation genrate group id
      if(esm_bc->lbi == 0)
      {
        pdp_list[conn_id]->group_id = esm_bc->sm_group_id;
        pdp_list[conn_id]->is_primary_pdp = TRUE;
      }   
      else
      {
        pdp_list[conn_id]->is_primary_pdp = FALSE; 
        /* set linked TI and copy group id from primary*/
        pdp_list[conn_id]->linked_ti = nsapi_list[esm_bc->lbi];
        pdp_list[conn_id]->group_id = pdp_list[pdp_list[conn_id]->linked_ti]->group_id;
      } 
      
      if(esm_isr_status_get() == TRUE)
      {
         pdp_list[conn_id]->isr_pdp_act_status = CTXT_ACT_AFTER_ISR_IN_LTE;
      }
      else
      {
         pdp_list[conn_id]->isr_pdp_act_status = CTXT_ACT_BEFORE_ISR;
      }

      /*TBD : PDP Address */
      pdp_list[conn_id]->pdp_addr.valid = TRUE;
      pdp_list[conn_id]->pdp_addr.pdp_type_org = 0x01; //IETF alllocated address
      
      if(esm_bc->pdn_address.pdn_type_val == NAS_ESM_IPV4)
      {
        memset(&pdp_list[conn_id]->pdp_addr.address[0], 0, MAX_PDP_ADR_LEN); 
        memscpy(&pdp_list[conn_id]->pdp_addr.address[0], sizeof(pdp_list[conn_id]->pdp_addr.address),
                &esm_bc->pdn_address.address[0], 4);
        pdp_list[conn_id]->pdp_addr.pdp_addr_len= 4;
        pdp_list[conn_id]->pdp_addr.pdp_type_num = 0x21;
      }
      else
      {
        MSG_ERROR_DS_0(SM_SUB,"ESM: Conversion of other addres type to be done");
      } 
      
      pdp_list[conn_id]->apn_name = esm_bc->apn_name;
      pdp_list[conn_id]->pdp_addr_alloc_type = PDP_ADDR_DYNAMIC;

      // copy h_comp and d_comp values
      pdp_list[conn_id]->h_comp = esm_bc->h_comp;
      pdp_list[conn_id]->d_comp = esm_bc->d_comp;

    }
    
    
    pdp_list[conn_id]->pdp_state = PDP_ACTIVE;
    pdp_list[conn_id]->rab_state = RABM_INITIAL;    

    pdp_list[conn_id]->llc_state = LLC_NOT_CONFIRMED;
    
    pdp_list[conn_id]->llc_sapi = esm_bc->neg_llc_sapi;
    pdp_list[conn_id]->qos = esm_bc->neg_qos;
    pdp_list[conn_id]->qos_valid = TRUE;
    pdp_list[conn_id]->radio_pri = esm_bc->radio_pri;
    pdp_list[conn_id]->flow_id = esm_bc->packet_flow_id;    

    // Set minimum QOS to false
    pdp_list[conn_id]->minimum_qos.valid = FALSE;

#ifdef FEATURE_MBMS
    #error code not present
#endif

    // Update th BCM info
    pdp_list[conn_id]->bcm = esm_bc->bcm;
    /* Set BCM of all PDP in group same as cur pdp*/
    for (index = 0; index < MAX_PDP_NUM; index++)
    {
      if ((index != conn_id) && 
          (pdp_list[index] != NULL) &&
          (pdp_list[conn_id]->group_id == pdp_list[index]->group_id) &&
          (pdp_list[index]->pdp_state != PDP_INACTIVE))
      {
        pdp_list[index]->bcm = pdp_list[conn_id]->bcm;
        MSG_HIGH_DS_2(SM_SUB, "BCM Mode for TI %d is %d", index, pdp_list[index]->bcm);
      }
    }
    //Return group id to ESM
    return (pdp_list[conn_id]->group_id); 
  }
  else if(bc_state == ESM_BC_INACTIVE)
  {
    if(pdp_list[conn_id] != NULL)
    {
      MSG_HIGH_DS_2(SM_SUB,"ESM: Deleting SM context for BID %d w/ Conn ID (TI) %d",
               esm_bc->eps_bearer_id, conn_id);

      if (pdp_list[conn_id]->nsapi.valid == TRUE)
        (void)sm_release_nsapi_sm (pdp_list[conn_id]->trans_id);
    
      (void) sm_unreserve_connection_id_int(pdp_list[conn_id]->trans_id);     
    }
    else
    {
      MSG_HIGH_DS_1(SM_SUB,"ESM: Cannot delete SM conn id %d", conn_id); 
    } 
    return INVALID_ID;
  }  

  return INVALID_ID;
}

/*==============================================================================

  FUNCTION:  sm_esm_isr_status_set

==============================================================================*/
void sm_esm_isr_status_set(boolean isr_status)
{
  if((esm_isr_status_get() == TRUE)&&
     (isr_status == FALSE))
  {
    //reset isr context activation status
    esm_reset_isr_bearer_ctxt_act_status();
    sm_reset_isr_pdp_act_status();
    sm_deact_isr_on_interrat_status_set(SM_DEACT_ISR_ON_INTERRAT_NONE);
  }
  esm_isr_status_set(isr_status);
}


/*==============================================================================

  FUNCTION:  sm_deact_isr_on_interrat_status_set

==============================================================================*/
void sm_deact_isr_on_interrat_status_set
(
  sm_deact_isr_on_interrat_status_T deact_isr_on_interrat_status
)
{
  MSG_HIGH_DS_2(SM_SUB,"ESM: Deact ISR on interrat status change from %d to %d",sm_deact_isr_on_interrat_status,deact_isr_on_interrat_status);
  sm_deact_isr_on_interrat_status = deact_isr_on_interrat_status;
}


/*==============================================================================

  FUNCTION:  sm_deact_isr_on_interrat_status_get

==============================================================================*/
sm_deact_isr_on_interrat_status_T sm_deact_isr_on_interrat_status_get
(
  void
)
{
  return sm_deact_isr_on_interrat_status;
}


/*==============================================================================

  FUNCTION:  sm_build_and_send_deactivate_ind

==============================================================================*/
void sm_build_and_send_deactivate_ind
(
  const esm_eps_bearer_context_T  *esm_bc,
  cc_cause_T current_cause
)
{
  cm_rpt_type *cm_rpt;
  cm_rpt = cm_get_cmd_buf();
  if(cm_rpt == NULL)
  {
    MSG_HIGH_DS_0(SM_SUB,"Unable to get allocate CM Buffer");
    return ;
  }
  MSG_HIGH_DS_1( SM_SUB," Sending SMREG_PDP_DEACTIVATE_CNF for non transferrable bearer to CM Bearer ID %d",
            esm_bc->eps_bearer_id);
  
  // Set the connection Id to 0xff as for non transferrable bearer, connection id will not be available.
  cm_rpt->cmd.pdp_deact_ind.connection_id = INVALID_ID;
  
  // Pack the NSAPI 
  cm_rpt->cmd.pdp_deact_ind.nsapi.valid = TRUE;
  cm_rpt->cmd.pdp_deact_ind.nsapi.nsapi = esm_bc->eps_bearer_id;
  
  // This bearer deactivation happens due to its linked bearer getting deactivated, populate the cause as internal.
  cm_rpt->cmd.pdp_deact_ind.cause_type = SM_INTERNAL_CAUSE;
  cm_rpt->cmd.pdp_deact_ind.cause.int_cause = (sm_cm_status_T)current_cause;     

  // Pack Protocol Configuration Optoins
  cm_rpt->cmd.pdp_deact_ind.config_options = esm_bc->config_options;

  // Set the CM report header and the command
  cm_rpt->hdr.cmd = CM_PDP_DEACTIVATE_IND;
#ifdef FEATURE_DUAL_SIM
  cm_rpt->cmd.pdp_deact_ind.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
  cfa_log_packet_ex( GS_QUEUE_CM,
             MS_CM_SM,
             (byte)(cm_rpt->hdr.cmd),
             MIN(250, sizeof(cm_rpt->cmd)),
             (byte*)&(cm_rpt->cmd) );

  sm_add_message_to_debug_buffer(MS_CM_SM,
                  (dword)cm_rpt->hdr.cmd,
                   FROM_SM,
                   (sys_modem_as_id_e_type)cm_rpt->cmd.pdp_deact_ind.as_id);

  MSG_HIGH_DS_0(SM_SUB,"posting command to CM");
  cm_put_cmd(cm_rpt);
}


/*==============================================================================

  FUNCTION:  sm_esm_deact_assoc_non_transferrable_ctxts

==============================================================================*/
void sm_esm_deact_assoc_non_transferrable_ctxts
(
  esm_bcm_T* mgr_ptr,
  byte linked_bearer_id,
  cc_cause_T current_cause
)
{

  byte index = 0;
  esm_bc_state_T   cur_state;

  for(index=0; index < ESM_MAX_EPS_BEARERS; index++)
  {
    if((mgr_ptr->esm_bc[index] != NULL)&&
	  (mgr_ptr->esm_bc[index]->lbi == linked_bearer_id)&&
      (mgr_ptr->esm_bc[index]->ti.valid == FALSE ||
      mgr_ptr->esm_bc[index]->neg_qos.valid == FALSE))
    {
      cur_state = mgr_ptr->esm_bc[index]->eps_bearer_state_ptr->state();
      if(cur_state != ESM_BC_INACTIVE)
      {
        sm_build_and_send_deactivate_ind(mgr_ptr->esm_bc[index],current_cause);
      }
      esm_bcm_reset_ctxt(mgr_ptr->esm_bc[index]);

    }
  }
}

#ifdef FEATURE_DUAL_SIM

/*==============================================================================

  FUNCTION:  sm_esm_check_if_ps_active

==============================================================================*/
boolean sm_esm_check_if_ps_active
(
  esm_bcm_T         *mgr_ptr
)
{
  word index;
  boolean is_ps_active = FALSE;
  if(mgr_ptr->on_attach_def_bearer_pending == ESM_BC_ON_ATTACH_DEF_BEARER_SUCCESS)
  {
    is_ps_active = TRUE;
  }
  else
  {
    for ( index = 0 ; index < MAX_PDP_NUM ; index++)
    {
      if (pdp_list[index] != NULL)
      {
        is_ps_active = TRUE;
        break;
      }
    }
  }

  return is_ps_active;
}


/*==============================================================================

  FUNCTION:  esm_validate_and_set_sm_as_id

==============================================================================*/
boolean esm_validate_and_set_sm_as_id
(
  esm_bcm_T            *mgr_ptr,
  esm_bpm_T            *proc_mgr,
  esm_cmd_type *msg_ptr
)
{
  boolean valid_as_id_received = FALSE;
  boolean is_ps_active = FALSE;

  sys_modem_as_id_e_type as_id = INST_ID_TO_SYS_AS_ID(MSGR_GET_INST(&msg_ptr->cmd.msg_hdr));

  if(( SM_IS_VALID_AS_ID(as_id) == TRUE ) 
#ifndef FEATURE_DUAL_DATA
     ||
     ((sys_modem_as_id_e_type)sm_as_id == SYS_MODEM_AS_ID_NONE ) ||
     ((is_ps_active = sm_esm_check_if_ps_active(mgr_ptr)) == FALSE)
#endif
    )
  {
    sm_as_id = as_id;
    valid_as_id_received = TRUE;    
  }
#ifndef FEATURE_DUAL_DATA
  else 
  {
    /* special handling*/
    switch (msg_ptr->cmd.msg_hdr.id)
    {
      case NAS_ESM_SIG_CONNECTION_RELEASED_IND:
        if ((is_ps_active == TRUE) &&
           (mgr_ptr->on_attach_def_bearer_pending == ESM_BC_ON_ATTACH_DEF_BEARER_NONE))
        {
          /* when PS is active on other sub, it is possible to receive ESM_SIG_CONN_RELEASE
             Do not change as_id for this message */
          MSG_HIGH_DS(SM_SUB,"sig con rel indication on wrong sub, PS active on SUB %d",sm_as_id+1,0,0);
          valid_as_id_received = TRUE;
        }
        break;

      
      case NAS_ESM_PDN_CONNECT_PACKED_CNF:
	    if ((esm_bpm_find_uicc_proc_state_machine(proc_mgr,msg_ptr->cmd.esm_pdn_connectivity_pkd_cnf.pti) == NULL))
        {
          // Ignore packed_cnf if no response is pending from card. This block is specific to cases where slow card sends a delayed response
          // & we do not have this procedure running at ESM any more.
          valid_as_id_received = TRUE;
        }	
	    break;

      default:
        break;
    }
  }
#endif

  MSG_HIGH_DS(SM_SUB,"sm_esm_check_if_ps_active returning %d attach def bearer status %d",
                     is_ps_active, mgr_ptr->on_attach_def_bearer_pending, 0);

  if (valid_as_id_received == FALSE)
  {
    MSG_FATAL_DS(SM_SUB, " ESM: ERR FATAL Invalid AS ID:%d UMID %d ",
                                                 as_id,msg_ptr->cmd.msg_hdr.id,0);
  }

  return valid_as_id_received;
}

/*==============================================================================

  FUNCTION:  esm_get_sm_as_id

==============================================================================*/
sys_modem_as_id_e_type esm_get_sm_as_id
(
  void
)
{
  return (sys_modem_as_id_e_type)sm_as_id;
}

#endif
/*===========================================================================
FUNCTION SM_IS_MULTIMODE_SUB

DESCRIPTION
  This function tells if current sub is multimode sub.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS

===========================================================================*/
boolean sm_is_multimode_sub(void)
{
#ifdef FEATURE_DUAL_SIM
  if(mm_get_sub_capability((sys_modem_as_id_e_type)sm_as_id) == SUBS_CAPABILITY_MULTIMODE)
  {
    return TRUE;
  }
  return FALSE;
#else
  return TRUE;
#endif
}

#ifdef FEATURE_DUAL_DATA
/*===========================================================================
FUNCTION sm_get_multimode_sub

DESCRIPTION
  This function returns multimode sub.

DEPENDENCIES
  None.

RETURN VALUE
  AS_ID

SIDE EFFECTS

===========================================================================*/
sys_modem_as_id_e_type sm_get_multimode_sub(void)
{
  sys_modem_as_id_e_type sm_mmode_as_id = SYS_MODEM_AS_ID_NONE;

  for (sm_mmode_as_id = (sys_modem_as_id_e_type)SM_AS_ID_1; sm_mmode_as_id < (sys_modem_as_id_e_type)MAX_AS_IDS; sm_mmode_as_id++)
  {
    if(mm_get_sub_capability((sys_modem_as_id_e_type)sm_mmode_as_id) == SUBS_CAPABILITY_MULTIMODE)
    {
      return sm_mmode_as_id;
    }
  }

  return sm_mmode_as_id;
}
#endif

#endif 

