/*===========================================================================
                       S M  S N D C P   S O U R C E   F I L E 

DESCRIPTION

   This file contains the function definitions between the SM and CM entities.
   
EXTERNALIZED FUNCTIONS   

  sm_process_sndcp_cmd() : Processes the command sent by SNDCP entity.
                  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/sm/vcs/smsndcp.c_v   1.3   18 Jul 2002 15:44:24   kishores  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/smsndcp.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/29/12   us      Setting cm_pending_msg to SMREG_PDP_DEACTIVATE_REQ for a successful PDP deactivation after receiving SNSM_STATUS_REQ. 
02/13/12   us      Added new cause SNSM_MODIFY_FAILURE in snsm_cause_T 
07/13/11   am      Trigger PDP context deactivation when a PDP Modification message 
                   is received with a new PDP address
05/05/08   ab     Process pending message on receivnig SNSM_ACTIVATE_RESP
04/18/07   rk     Ignore SNDCP Deact Resp for PDP in INACTIVE statede
09/22/06   ss     Included smix.h for FEATURE_GSM_SNDCP_IPHC_ROHC
09/15/06   rk      Error handling SNDCP response resulting PDP deactivation failure after W2G CCO
08/04/06   ss      Implemented changes for ROHC and IP header compression.
04/24/06   ss      Sending appropriate primitives for Secondary promotion/deactivation
04/20/06   ss      Setting PDP State to Active conditionally based on LLC State.
03/24/06   ss       Dynamic PFI validation added
01/19/06  ss       Passing the PFI value of the PDP context to SNDCP upon Activation/Modification of the NSAPI.
09/13/05  kvk     MO PDP Modify supported. Now handling the Application initiated
                   PDP Modification for QoS and TFT.
08/11/05   KVK     Downgraded the Error message flagged by SM when SNDCP send SNSM_MODIFY_RSP during 
                   InterRAT resumption.
02/24/05   kvk     Now deactivating Sec PDPs followed by primary PDPs
                   when SM gets GPRS_REL indication from GMM in GPRS.
02/07/05   kvk     Stop the SNDCP guard when SM receives SNDCP_DEACTIVATE_RESP.
01/26/05   kvk     Now deactivating all PDPs associated with llc sapi indicated
                   in snsm_status_req. PDP state set to PDP_INACTIVE_PENDING before
                   SM indicates SNDCP with SNSM_DEACTIVATE_IND during all nsapis deactivated.
01/21/05   kvk     Removed the sync for SNDCP MOSIFY_RSP notification to 
                   indicate RAT_CHANGE_CNF to GMM.
09/28/04  kvk      Added handling for SEC PDP in GPRS.
06/09/04   kvk     During Inter RAT change from G->W SNDCP deactivate cnf is
                   gracefully handled with out changing the PDP state to INACTIVE.
06/08/04   kvk     PDP con state is reset along with PDP state.
06/03/04   ks      Added SNDCP h_comp and d_comp params to PDP context info.
06/03/04   kvk     PDP state is changed to INACTIVE after it receives SNSM_DEACTIVATE_RESP and confirmation
                   is sent to CM.
11/23/03   mks     Added sm_send_sndcp_deact_all_nsapi() to command SNDCP to 
                   deactivate all active NSAPIs.
07/28/03   ks      Added support for GSNSM_SEQUENCE_IND primitive and also 
                   handling of MODIFY-CNF from SNDCP during Inter RAT change.
06/25/03   ks      Added LLC returned cause handling in process_snsm_status_resp()
06/18/03   ks      Sending the Radio Priority in the MODIFY_IND to SNDCP.
03/18/03   mks     Added support for delaying GMMSM_RAT_RESUMPTION_CNF to GMM
                   until SNDCP confirms the PDP context activation.
02/24/03   ks      Fixed LINT warnings.
12/02/02   ks      Added support for PDP Modification procedures for GPRS.
10/07/02   ks      Added processing of SNSM_SEQUENCE_RESP primitive in 
                   sm_process_sndcp_cmd() 
07/18/02   ks      Renamed FEATURE_GPRS with FEATURE_GSM_GPRS
07/01/02   ks      Added processing cmds sent from SNDCP to support 
                   GPRS opration.
01/15/01   ks      Created file.

===========================================================================*/

/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#ifdef FEATURE_GSM_GPRS

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "sm_v.h"
#include "sm_sndcp.h"
#include "smtask.h"
#include "smtask_v.h"
#include "gsndcp.h"
#include "gsmsnif.h"
#include "smtimers.h"
#include "smix_v.h"
#include "mm_v.h"
#ifdef NAS_TEST_FRAMEWORK

#include <tf_stub.h>

/* Declare the interceptor stubs
*/
TF_PROTO_STUB( void, gsndcp_snsm_put_mes, gsmsn_mes_type* );
#define gsndcp_snsm_put_mes  TF_STUB(gsndcp_snsm_put_mes)

#endif /* TEST_FRAMEWORK */
/* -----------------------------------------------------------------------
** FUNCTION definitions
** ----------------------------------------------------------------------- */
LOCAL  void process_snsm_activate_resp(byte nsapi);
LOCAL  void process_snsm_deactivate_resp(byte nsapi);
LOCAL  void process_snsm_modify_resp(byte nsapi);
LOCAL  void process_snsm_status_resp(byte sapi, snsm_cause_T cause);

#if defined(FEATURE_DUAL_DATA)
extern uint32 sm_tlli_sim[MAX_AS_IDS];
#define sm_tlli sm_tlli_sim[sm_as_id]
#else
extern uint32 sm_tlli;
#endif


#if defined(FEATURE_DUAL_SIM)
extern boolean SM_IS_VALID_AS_ID(sys_modem_as_id_e_type cmd_as_id);
#endif
                   
/*===========================================================================
FUNCTION SM_PROCESS_SNDCP_CMD 

DESCRIPTION
  This function processes the received command from SNDCP entity in case of
  GPRS mode.
    
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_process_sndcp_cmd( sm_cmd_type *sm_cmd_ptr )
{
  #ifdef FEATURE_DUAL_SIM
  boolean process_sndcp_cmd = TRUE;
  #endif

  MSG_HIGH_DS_0(SM_SUB,"Received command from SNDCP");

#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
  switch (sm_cmd_ptr->header.cmd_id)
  {
    case SNSM_ACTIVATE_RESP :
      process_sndcp_cmd = SM_IS_VALID_AS_ID(sm_cmd_ptr->cmd.sndcp_activate_resp.as_id);
      break;

    case SNSM_DEACTIVATE_RESP:
      process_sndcp_cmd = SM_IS_VALID_AS_ID(sm_cmd_ptr->cmd.sndcp_deactivate_resp.as_id);
      break;

    case SNSM_MODIFY_RESP:
      process_sndcp_cmd = SM_IS_VALID_AS_ID(sm_cmd_ptr->cmd.sndcp_modify_resp.as_id);
      break;

    case SNSM_STATUS_REQ:
       process_sndcp_cmd = SM_IS_VALID_AS_ID(sm_cmd_ptr->cmd.sndcp_status_req.as_id);
      break;

    default:
      MSG_ERROR_DS_0(SM_SUB,"Unknown cmd received from SNDCP");
      break;
  }
#endif
#endif

#ifdef FEATURE_DUAL_SIM
  if (process_sndcp_cmd == TRUE)
  {
#endif 

  switch (sm_cmd_ptr->header.cmd_id)
  {
    case SNSM_ACTIVATE_RESP :
      process_snsm_activate_resp(sm_cmd_ptr->cmd.sndcp_activate_resp.nsapi);
      break;

    case SNSM_DEACTIVATE_RESP:
      process_snsm_deactivate_resp(sm_cmd_ptr->cmd.sndcp_deactivate_resp.nsapi);
      break;

    case SNSM_MODIFY_RESP:
      process_snsm_modify_resp(sm_cmd_ptr->cmd.sndcp_modify_resp.nsapi);
      break;

    case SNSM_STATUS_REQ:
      process_snsm_status_resp(sm_cmd_ptr->cmd.sndcp_status_req.sapi, 
                               sm_cmd_ptr->cmd.sndcp_status_req.cause);
      break;

    case SNSM_SEQUENCE_RESP:
      MSG_ERROR_DS_0(SM_SUB,"PDP Sequence Res is not supported yet");
      break;
    
    default:
      MSG_ERROR_DS_0(SM_SUB,"Unknown cmd received from SNDCP");
    break;
  }

#ifdef FEATURE_DUAL_SIM
  }
#endif
}

/*===========================================================================
FUNCTION sm_send_sndcp_cmd 

DESCRIPTION
  This function sends the primitive from SM to SNDCP during PDP context during
  Activation, Deactivation and Modification procedures.
      
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_send_sndcp_cmd( pdp_context_info *cur_pdp, gsnsm_prim_type sndcp_cmd)
{
  gsmsn_mes_type *cmd_ptr;

  if (cur_pdp->nsapi.valid == FALSE || cur_pdp->nsapi.nsapi >= MAX_NSAPI_NUM)
  {
    MSG_ERROR_DS_3(SM_SUB,"Invalid NSAPI: valid: %d, nsapi: %d, cmd: %d",cur_pdp->nsapi.valid, cur_pdp->nsapi.nsapi,sndcp_cmd); 
    return;
  }
  cmd_ptr = gsndcp_snsm_get_mes_buf( );
  if (cmd_ptr != NULL)
  {
    switch (sndcp_cmd)
    {
      case GSNSM_ACTIVATE_IND:           /* gsnsm_activ_ind_type             */
        cmd_ptr->mes.snsm_activ_ind.prim  = GSNSM_ACTIVATE_IND;
        cmd_ptr->mes.snsm_activ_ind.tlli  = sm_tlli;
        cmd_ptr->mes.snsm_activ_ind.nsapi = cur_pdp->nsapi.nsapi;
        cmd_ptr->mes.snsm_activ_ind.sapi  = cur_pdp->llc_sapi.sapi;
        cmd_ptr->mes.snsm_activ_ind.qos   = cur_pdp->qos;
        cmd_ptr->mes.snsm_activ_ind.radio_priority = cur_pdp->radio_pri.radio_priority;
#ifdef FEATURE_DUAL_SIM
        cmd_ptr->mes.snsm_activ_ind.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
#ifdef FEATURE_GPRS_COMP_ENUM_TYPES
        if (cur_pdp->pdp_addr.pdp_type_num == PPP_TYPE)
        {
          cmd_ptr->mes.snsm_activ_ind.pdp_type = GSNSM_PDP_INVALID;
        }
        else if ( cur_pdp->pdp_addr.pdp_type_num == IPV4_TYPE )
        {
          cmd_ptr->mes.snsm_activ_ind.pdp_type = GSNSM_PDP_IPV4;
        }
        else if ( cur_pdp->pdp_addr.pdp_type_num == IPV6_TYPE )
        {
          cmd_ptr->mes.snsm_activ_ind.pdp_type = GSNSM_PDP_IPV6;
        }
#ifdef FEATURE_DUAL_IP_BEARER 
        /*
         * Temporarily sending IPV4 as pdp_type to SNDCP when IPV4V6_TYPE
         * is received from network. This should be changed once support for 
         * dual ip bearer is added in SNDCP 
         */
        else if ( cur_pdp->pdp_addr.pdp_type_num == IPV4V6_TYPE )
        {
          cmd_ptr->mes.snsm_activ_ind.pdp_type = GSNSM_PDP_IPV4;
        }
#endif
        cmd_ptr->mes.snsm_activ_ind.pcomp_alg= cur_pdp->h_comp;
        cmd_ptr->mes.snsm_activ_ind.dcomp_alg= cur_pdp->d_comp;
#else
        cmd_ptr->mes.snsm_activ_ind.h_comp = cur_pdp->h_comp;
        cmd_ptr->mes.snsm_activ_ind.d_comp = cur_pdp->d_comp;
#endif
        
#ifdef FEATURE_GSM_GPRS_PFC
        if( cur_pdp->flow_id.valid )
        {
          cmd_ptr->mes.snsm_activ_ind.pfi = cur_pdp->flow_id.packet_flow_id;
        }
        else
        {      
          /* Use Best Effort PFI value "0" if no PFI has been allocated */
          cmd_ptr->mes.snsm_activ_ind.pfi = 0;
        }
#endif
                  
        MSG_HIGH_DS_1(SM_SUB,"Sent GSNSM_ACTIVATE_IND, PDP : %d",cur_pdp->nsapi.nsapi);                                                                   
      break;
      
      case GSNSM_DEACTIVATE_IND:         /* gsnsm_deactiv_ind_type           */
        cmd_ptr->mes.snsm_deactiv_ind.prim  = GSNSM_DEACTIVATE_IND;
        cmd_ptr->mes.snsm_deactiv_ind.tlli  = sm_tlli;
        
        /* ------------------------------------------------------------------
        ** Only one PDP context deactivation is supported now, Should be
        ** modified when support for multiple PDP deactivation is supported
        ** ----------------------------------------------------------------*/
        cmd_ptr->mes.snsm_deactiv_ind.num_nsapi = 1;
        cmd_ptr->mes.snsm_deactiv_ind.nsapi[0]  = cur_pdp->nsapi.nsapi;
        cmd_ptr->mes.snsm_deactiv_ind.local     = TRUE; 
        
#ifdef FEATURE_DUAL_SIM
        cmd_ptr->mes.snsm_deactiv_ind.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif

        if (cur_pdp->llc_state != LLC_MT_DEACTIVATE_CNF_PENDING && 
              cur_pdp->llc_state != LLC_MO_DEACTIVATE_CNF_PENDING)
        {
          cur_pdp->llc_state = LLC_NOT_CONFIRMED;
        }
        MSG_HIGH_DS_1(SM_SUB,"Sent GSNSM_DEACTIVATE_IND, PDP : %d",cur_pdp->nsapi.nsapi);
        
      break;
      
      case GSNSM_MODIFY_IND:             /* gsnsm_modify_ind_type            */
        cmd_ptr->mes.snsm_modify_ind.prim  = GSNSM_MODIFY_IND;
        cmd_ptr->mes.snsm_modify_ind.tlli  = sm_tlli;
        cmd_ptr->mes.snsm_modify_ind.nsapi = cur_pdp->nsapi.nsapi;
        cmd_ptr->mes.snsm_modify_ind.sapi  = cur_pdp->llc_sapi.sapi;
        cmd_ptr->mes.snsm_modify_ind.qos   = cur_pdp->qos;
        cmd_ptr->mes.snsm_modify_ind.radio_priority = cur_pdp->radio_pri.radio_priority;

        /* ------------------------------------------------------------------
        ** TS 04.65 5.1.2.23 - If the NSAPI being modified is a new NSAPI and
        ** if the QoS reliability class indicates acknowledged peer-to-peer
        ** operation, then SNDCP's Send and Rcv NPDU numbers must be set as
        ** given. Not immediately obvious where these are obtained from.
        ** ----------------------------------------------------------------*/   
        cmd_ptr->mes.snsm_modify_ind.snd_npdu_num = 0;
        cmd_ptr->mes.snsm_modify_ind.rcv_npdu_num = 0;
#ifdef FEATURE_GPRS_COMP_ENUM_TYPES
        if (cur_pdp->pdp_addr.pdp_type_num == PPP_TYPE)
        {
          cmd_ptr->mes.snsm_modify_ind.pdp_type = GSNSM_PDP_INVALID;
        }
        else if ( cur_pdp->pdp_addr.pdp_type_num == IPV4_TYPE )
        {
          cmd_ptr->mes.snsm_modify_ind.pdp_type = GSNSM_PDP_IPV4;
        }
        else if ( cur_pdp->pdp_addr.pdp_type_num == IPV6_TYPE )
        {
          cmd_ptr->mes.snsm_modify_ind.pdp_type = GSNSM_PDP_IPV6;
        }
#ifdef FEATURE_DUAL_IP_BEARER 
        /*
         * Temporarily sending IPV4 as pdp_type to SNDCP when IPV4V6_TYPE
         * is received from network. This should be changed once support for 
         * dual ip bearer is added in SNDCP 
         */
        else if ( cur_pdp->pdp_addr.pdp_type_num == IPV4V6_TYPE )
        {
          cmd_ptr->mes.snsm_activ_ind.pdp_type = GSNSM_PDP_IPV4;
        }
#endif
        cmd_ptr->mes.snsm_modify_ind.pcomp_alg= cur_pdp->h_comp;
        cmd_ptr->mes.snsm_modify_ind.dcomp_alg= cur_pdp->d_comp;
#else
        cmd_ptr->mes.snsm_modify_ind.h_comp = cur_pdp->h_comp;
        cmd_ptr->mes.snsm_modify_ind.d_comp = cur_pdp->d_comp;
#endif
#ifdef FEATURE_GSM_GPRS_PFC
        if( cur_pdp->flow_id.valid )
        {
          cmd_ptr->mes.snsm_modify_ind.pfi = cur_pdp->flow_id.packet_flow_id;
        }
        else
        {      
          /* Use Best Effort PFI value "0" if no PFI has been allocated */
          cmd_ptr->mes.snsm_modify_ind.pfi = 0;
        }
#endif
        
#ifdef FEATURE_DUAL_SIM
        cmd_ptr->mes.snsm_modify_ind.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif

        MSG_HIGH_DS_1(SM_SUB,"Sent GSNSM_MODIFY_IND, PDP : %d",cur_pdp->nsapi.nsapi);                                                                     
      break;
      
      case GSNSM_SEQUENCE_IND:            /* gsnsm_sequence_ind_type          */    
        cmd_ptr->mes.snsm_seq_ind.prim          = GSNSM_SEQUENCE_IND;
        cmd_ptr->mes.snsm_seq_ind.tlli          = sm_tlli;
        cmd_ptr->mes.snsm_seq_ind.nsapi         = cur_pdp->nsapi.nsapi;
        cmd_ptr->mes.snsm_seq_ind.rcv_npdu_num  = 0;

#ifdef FEATURE_DUAL_SIM
        cmd_ptr->mes.snsm_seq_ind.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif

        MSG_HIGH_DS_1(SM_SUB,"Sent GSNSM_SEQUENCE_IND, PDP : %d",cur_pdp->nsapi.nsapi);
      break;

      default:
        MSG_ERROR_DS_0(SM_SUB,"Unknown cmd request from SM->SNDCP");
      break;
    }
                              
    /* Send the Primitive to the SNDCP layer */
    gsndcp_snsm_put_mes (cmd_ptr);                             
  }
  else
  {
    MSG_ERROR_DS_0(SM_SUB, "Unable to get a SNDCP command buffer");
  }
   
}

/*===========================================================================
FUNCTION PROCESS_SNSM_ACTIVATE_RESP 

DESCRIPTION
  Process SNSM_ACTIVATE_RESP command received from SNDCP.
      
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void process_snsm_activate_resp(byte nsapi)
{
  byte trans_id;
  trans_id = (byte)GET_TRANSID(nsapi);

  if (trans_id != INVALID_ID && pdp_list[trans_id] != NULL && 
       pdp_list[trans_id]->llc_state == LLC_MO_ACTIVATE_CNF_PENDING)
  {
    MSG_HIGH_DS_0(SM_SUB,"Received LLC Activate confirm");

    /* reset the LLC state of this PDP context */
    pdp_list[trans_id]->llc_state = LLC_ACTIVE;
    
    /* Notify Call Manager that the PDP context establishment is successful */
    if (pdp_list[trans_id]->is_primary_pdp)
    {
      sm_send_cm_confirm(pdp_list[trans_id], SMREG_PDP_ACTIVATE_CNF, SM_INTERNAL_CAUSE);
    }
    else
    {
      sm_send_cm_confirm(pdp_list[trans_id], SMREG_PDP_ACTIVATE_SEC_CNF, SM_INTERNAL_CAUSE);
    }

#ifdef FEATURE_GSM_GPRS      
    if(pending_sm_pdu_id == trans_id)
    {
      MSG_HIGH_DS_0(SM_SUB,"Processing pending SM PDU");
      pending_sm_pdu_id = INVALID_ID;
      sm_process_sm_pdu(&pending_sm_pdu);
    }
#endif

    if( pending_sm_pdu.data != NULL)
    {
#ifdef FEATURE_MODEM_HEAP
      modem_mem_free( pending_sm_pdu.data , MODEM_MEM_CLIENT_NAS);
#else
      gs_free( pending_sm_pdu.data);
#endif
      pending_sm_pdu.data = NULL;
    }

  }
  else
  {
      MSG_ERROR_DS_2(SM_SUB,"Received LLC ACT CNF in unknown state nsapi: %d, trans_id: %d ", nsapi, trans_id);
  }
}

/*===========================================================================
FUNCTION PROCESS_SNSM_DEACTIVATE_RESP 

DESCRIPTION
  Process SNSM_DEACTIVATE_RESP command received from SNDCP..
      
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void process_snsm_deactivate_resp(byte nsapi)
{
  word index;
  byte trans_id = (byte)GET_TRANSID(nsapi);
  if (trans_id >= MAX_PDP_NUM || pdp_list[trans_id] == NULL )
  {
    MSG_HIGH_DS_2(SM_SUB,"Invalid Transaction ID for SNSM Deactivate Resp NSAPi:%d, TransId: %d",nsapi, trans_id);
    return;
  }

MSG_HIGH_DS_3(SM_SUB,"Sending SNSM Deactivate Resp NSAPi:%d, PDP_STATE:%d, TI: %d",nsapi, 
                  pdp_list[trans_id]->pdp_state, trans_id);

  if (pdp_list[trans_id]->llc_state == LLC_MO_DEACTIVATE_CNF_PENDING)
  {
    MSG_HIGH_DS_0(SM_SUB,"Received LLC Deactivate confirm");
    //change the PDP state to INACTIVE
    pdp_list[trans_id]->pdp_state = PDP_INACTIVE;
    //Reset the PDP con state
    pdp_list[trans_id]->pdp_con_state = SIG_CONNECTION_NONE;

#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
    /* Notify Call Manager that the PDP context deactivate is successful */
    if (pdp_list[trans_id]->is_primary_pdp
      && !pdp_list[trans_id]->teardown_ind.ti_flag
      &&sm_promote_sec_pri(pdp_list[trans_id]))
    {
       sm_send_cm_confirm(pdp_list[trans_id], SMREG_PDP_DEACTIVATE_PROMOTE_CNF,
                                                     SM_NETWORK_CAUSE);
    }
    else
    {
#endif
      if ( pdp_list[trans_id]->is_primary_pdp
#ifdef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
          && pdp_list[trans_id]->teardown_ind.ti_flag
#endif
      )
      {
        for (index = 0; index < MAX_PDP_NUM; index++)
        {
            if ((pdp_list[index] != NULL) &&
                pdp_list[trans_id]->trans_id != index && 
                pdp_list[trans_id]->group_id == pdp_list[index]->group_id )
              {
                MSG_HIGH_DS_1(SM_SUB,"Locally deactivating TI: %d", pdp_list[index]->trans_id);
                sm_stop_timer(pdp_list[index]->trans_id);

#ifdef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
                sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_CNF,
                                                     SM_NETWORK_CAUSE);
#else
                if(pdp_list[index]->mt_orig_type == TRUE)
                {
                  sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_IND, SM_NETWORK_CAUSE);
                }
                else
                {
                   sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_CNF, SM_INTERNAL_CAUSE);
                }
#endif
              }
        }
      }
#ifdef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
       sm_send_cm_confirm(pdp_list[trans_id], SMREG_PDP_DEACTIVATE_CNF,
                                        SM_NETWORK_CAUSE);
#else
      if(pdp_list[trans_id]->mt_orig_type == TRUE)
      {
         sm_send_cm_confirm(pdp_list[trans_id], SMREG_PDP_DEACTIVATE_IND, SM_NETWORK_CAUSE);
       }
       else
       {
          sm_send_cm_confirm(pdp_list[trans_id], SMREG_PDP_DEACTIVATE_CNF, SM_INTERNAL_CAUSE);
       }
    }
#endif

    /* reset the LLC state of this PDP context */
    // pdp_list[trans_id]->llc_state = LLC_NOT_CONFIRMED;
  }
  else if (pdp_list[trans_id]->llc_state == LLC_MT_DEACTIVATE_CNF_PENDING)
  {
    MSG_HIGH_DS_0(SM_SUB,"Received LLC Deactivate confirm");
    //Stop in case of timer pending
    sm_stop_timer(trans_id);
    //change the PDP state to INACTIVE
    pdp_list[trans_id]->pdp_state = PDP_INACTIVE;
    //Reset the PDP con state
    pdp_list[trans_id]->pdp_con_state = SIG_CONNECTION_NONE;

    /* Notify Call Manager that the PDP context deactivate is successful */
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
    if (pdp_list[trans_id]->is_primary_pdp
      && !pdp_list[trans_id]->teardown_ind.ti_flag
      &&sm_promote_sec_pri(pdp_list[trans_id]))
    {
       sm_send_cm_confirm(pdp_list[trans_id], SMREG_PDP_DEACTIVATE_PROMOTE_IND,
                                                     SM_NETWORK_CAUSE);
    }
    else
    {
#endif
      if ( pdp_list[trans_id]->is_primary_pdp
#ifdef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
          && pdp_list[trans_id]->teardown_ind.ti_flag
#endif
      )
      {
        for (index = 0; index < MAX_PDP_NUM; index++)
        {
            if ((pdp_list[index] != NULL) &&
                pdp_list[trans_id]->trans_id != index && 
                pdp_list[trans_id]->group_id == pdp_list[index]->group_id )
              {
                MSG_HIGH_DS_1(SM_SUB,"Locally deactivating TI: %d", pdp_list[index]->trans_id);
                sm_stop_timer(pdp_list[index]->trans_id);
#ifdef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
                sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_IND, pdp_list[trans_id]->cause_type);
#else
                sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_IND, pdp_list[trans_id]->cause_type);
#endif
              }
        }
      }
#ifdef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
      sm_send_cm_confirm(pdp_list[trans_id], SMREG_PDP_DEACTIVATE_IND,
                                                  pdp_list[trans_id]->cause_type);
#else
      sm_send_cm_confirm(pdp_list[trans_id], SMREG_PDP_DEACTIVATE_IND, pdp_list[trans_id]->cause_type);
    }
#endif
    /* reset the LLC state of this PDP context */
    // pdp_list[trans_id]->llc_state = LLC_NOT_CONFIRMED;
  }
  else
  {
    MSG_HIGH_DS_3(SM_SUB,"RAT change or Received LLC DEACT CNF in unknown state nsapi: %d, TransId: %d, pdp state:%d", nsapi, trans_id, pdp_list[trans_id]->pdp_state);
  }
}

/*===========================================================================
FUNCTION PROCESS_SNSM_MODIFY_RESP 

DESCRIPTION
  Process SNSM_MODIFY_RESP command received from SNDCP
      
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void process_snsm_modify_resp(byte nsapi)
{
  byte trans_id = (byte)GET_TRANSID(nsapi);
  
  
  if (trans_id == INVALID_ID || pdp_list[trans_id] == NULL )
  {
    MSG_HIGH_DS_2(SM_SUB,"Invalid Transaction ID for SNSM Modify Resp NSAPi:%d, TransId: %d",nsapi, trans_id);
    return;
  }
  
  if (pdp_list[trans_id]->llc_state == LLC_MO_MODIFY_CNF_PENDING)
  {
    MSG_HIGH_DS_0(SM_SUB,"Received LLC MO Modify confirm");
    if (pdp_list[trans_id]->pdp_state == PDP_MODIFY_PENDING){
        pdp_list[trans_id]->pdp_state = PDP_ACTIVE;
    }

    /* Notify Call Manager that the PDP context modification is successful */
    sm_send_cm_confirm(pdp_list[trans_id], SMREG_PDP_MODIFY_CNF, SM_INTERNAL_CAUSE);

    /* reset the LLC state of this PDP context to Active */
    pdp_list[trans_id]->llc_state = LLC_ACTIVE;
  }
  else if (pdp_list[trans_id]->llc_state == LLC_MT_MODIFY_CNF_PENDING)
  {
    MSG_HIGH_DS_0(SM_SUB,"Received LLC MT Modify confirm");
    if (pdp_list[trans_id]->pdp_state == PDP_MODIFY_PENDING){
        pdp_list[trans_id]->pdp_state = PDP_ACTIVE;
    }

    /* Notify Call Manager that the PDP context modification is successful */
    sm_send_cm_confirm(pdp_list[trans_id], SMREG_PDP_MODIFY_IND, SM_INTERNAL_CAUSE);

    /* reset the LLC state of this PDP context to Active*/
    pdp_list[trans_id]->llc_state = LLC_ACTIVE;
  }
  else
  {
    MSG_HIGH_DS_2(SM_SUB,"Received LLC MODIFY CNF in unknown state nsapi: %d, trans_id: %d", nsapi, trans_id);
  }
}

/*===========================================================================
FUNCTION PROCESS_SNSM_STATUS_RESP 

DESCRIPTION
 Process SNSM_STATUS_RESP command received from SNDCP
      
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void process_snsm_status_resp(byte sapi, snsm_cause_T cause)
{
  byte cnt; 
  boolean entry_found = FALSE;

  /* Received an Error Status from SNDCP */
  /* Find the PDP context with a LLC confirmation pending */

  for (cnt =0; cnt < MAX_PDP_NSAPI_NUM; cnt++) // later check for MAX_PDP_NSAPI_NUM
  {
    if ( pdp_list[cnt] != NULL && pdp_list[cnt]->llc_sapi.sapi == sapi )
    {
      entry_found = TRUE;
      switch(cause)
      {
        case SNSM_NORMAL_RELEASE:
          /* Part of the Context Modification procedure. */
          MSG_HIGH_DS_1(SM_SUB,"Rcvd SNDCP status SNSM_NORMAL_RELEASE, SAPI %d", sapi);
        break;

        case SNSM_NO_PEER_RESP:
          /* LLC is re-establishing */
          MSG_HIGH_DS_1(SM_SUB,"Rcvd SNDCP status SNSM_NO_PEER_RESP, SAPI %d", sapi);
        break;

        case SNSM_DM_RECEIVED:
        case SNSM_INVALID_XID_RESP:
        case SNSM_MODIFY_FAILURE:
          /* Deactivate all PDP contexts for the SAPI */
          MSG_HIGH_DS_1(SM_SUB,"Rcvd SNDCP status DM Rcvd or Invld XID, SAPI %d", sapi);

          switch(pdp_list[cnt]->llc_state)
          {
            case LLC_MO_ACTIVATE_CNF_PENDING:
              MSG_HIGH_DS_1(SM_SUB,"SNDCP Error in Activation, Nsapi %d",
                pdp_list[cnt]->nsapi.nsapi);   

             /* Start the deactivation procedures, due to LLC setup failure */
              pdp_list[cnt]->current_cause = (cc_cause_T)SM_PROTOCOL_ERR;
#ifdef FEATURE_MBMS
              #error code not present
#endif        
              sm_continue_pdp_deactivate(pdp_list[cnt]);         
              break;
        
            case LLC_MO_MODIFY_CNF_PENDING:
            case LLC_MT_MODIFY_CNF_PENDING:
              MSG_HIGH_DS_1(SM_SUB,"SNDCP Error in Modification, Nsapi %d",
                pdp_list[cnt]->nsapi.nsapi);   
#ifdef FEATURE_DUAL_SIM
               if( mm_per_subs_is_current_plmn_fplmn((sys_modem_as_id_e_type)sm_as_id) )
#else
               if( mm_is_current_plmn_fplmn() )
#endif                

              {
                llc_state_T llc_state;
                /*Send SNSM_DEACTIVATE_IND and SNSM_MODIFY_IND to keep SNDCP in active state*/
                MSG_HIGH_DS_1(SM_SUB,"FPLMN Case, sending GSNSM_DEACTIVATE_IND and GSNSM_MODIFY_IND to SNDCP, Nsapi %d", sapi);
                llc_state = pdp_list[cnt]->llc_state;
                sm_send_sndcp_cmd (pdp_list[cnt], GSNSM_DEACTIVATE_IND);
                pdp_list[cnt]->llc_state = llc_state ;
                sm_send_sndcp_cmd (pdp_list[cnt], GSNSM_MODIFY_IND);
              }
              else
              {
                /* Start the deactivation procedures, due to LLC setup failur */
                pdp_list[cnt]->current_cause = (cc_cause_T)SM_PROTOCOL_ERR;
#ifdef FEATURE_MBMS
                #error code not present
#endif
                pdp_list[cnt]->cm_pending_msg = SMREG_PDP_DEACTIVATE_REQ;
                sm_continue_pdp_deactivate(pdp_list[cnt]);
              }
              break;
        
            case LLC_MO_DEACTIVATE_CNF_PENDING:
            case LLC_MT_DEACTIVATE_CNF_PENDING:
              MSG_HIGH_DS_1(SM_SUB,"SNDCP Error in Deactivation, Nsapi %d",
                pdp_list[cnt]->nsapi.nsapi);   
              
              /* Send an Deactivate reject to Call manager */
              sm_send_cm_confirm( pdp_list[cnt], SMREG_PDP_DEACTIVATE_CNF,
                                  SM_INTERNAL_CAUSE );
              // pdp_list[cnt]->llc_state = LLC_NOT_CONFIRMED;
              break;
        
            case LLC_NOT_CONFIRMED:          
            case LLC_ACTIVE:
              if ( pdp_list[cnt]->pdp_state == PDP_ACTIVE )
              {
                MSG_HIGH_DS_1(SM_SUB,"SNDCP Error in Active PDP, Nsapi %d",
                  pdp_list[cnt]->nsapi.nsapi);   

                /* Start the deactivation procedures, due to link layer errors. */
                pdp_list[cnt]->current_cause = (cc_cause_T)SM_PROTOCOL_ERR;
#ifdef FEATURE_MBMS
                #error code not present
#endif
                sm_continue_pdp_deactivate(pdp_list[cnt]);

              }
              break;

            default:
              MSG_ERROR_DS_0(SM_SUB,"Invld LLC state in status_rsp from SNDCP");
              break;    
          }
          break;

        default:
          MSG_ERROR_DS_0(SM_SUB,"Invld SNDCP status in status_rsp from SNDCP");
          break;
      }
    }
  }

  if (!entry_found)
  {
    MSG_ERROR_DS_1(SM_SUB,"Invld SNDCP SAPI %d in status_req from SNDCP",sapi);
    return;
  }

  
}

/*===========================================================================
FUNCTION SM_SEND_SNDCP_DEACT_ALL_NSAPI

DESCRIPTION
 Process SNSM_STATUS_RESP command received from SNDCP
      
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_send_sndcp_deact_all_nsapi(cc_cause_T cause)
{
  gsmsn_mes_type *cmd_ptr;
  word           index = 0;
  uint8          num_active_nsapi = 0;

  cmd_ptr = gsndcp_snsm_get_mes_buf( );
  if (cmd_ptr !=NULL)
  {
    cmd_ptr->mes.snsm_deactiv_ind.prim  = GSNSM_DEACTIVATE_IND;
    cmd_ptr->mes.snsm_deactiv_ind.tlli  = sm_tlli;
    cmd_ptr->mes.snsm_deactiv_ind.local = TRUE;
#ifdef FEATURE_DUAL_SIM
    #ifndef FEATURE_DUAL_DATA
    if( sm_as_id == SYS_MODEM_AS_ID_NONE)
       cmd_ptr->mes.snsm_deactiv_ind.as_id = ctxt_as_id;
    else
      cmd_ptr->mes.snsm_deactiv_ind.as_id = sm_as_id ;
    #else
       cmd_ptr->mes.snsm_deactiv_ind.as_id = (sys_modem_as_id_e_type)sm_as_id ;   
    #endif
#endif

    /*Deactivate all secondary PDPs*/
    for ( index = 0; index < MAX_PDP_NUM; index ++ )
    {
      if ( pdp_list[index] != NULL && 
           pdp_list[index]->pdp_state != PDP_INACTIVE &&
           !pdp_list[index]->is_primary_pdp
          )
      {
        //Change the PDP state to INACTIVE_PENDING
        pdp_list[index]->pdp_state = PDP_INACTIVE_PENDING;
        
        pdp_list[index]->current_cause = (cc_cause_T)NO_GPRS_CONTEXT;

        /* Clear pending timers before notifying SNDCP & upper layers */
        sm_stop_timer (index);

        pdp_list[index]->current_cause = cause;
        
        cmd_ptr->mes.snsm_deactiv_ind.nsapi[num_active_nsapi]  = pdp_list[index]->nsapi.nsapi;
        MSG_HIGH_DS_1(SM_SUB,"Sent GSNSM_DEACTIVATE_IND, PDP : %d",pdp_list[index]->nsapi.nsapi);

        num_active_nsapi ++;

        /* Set the llc_state to LLC_CONFIRM_PENDING, start RABM insanity timer */
        pdp_list[index]->llc_state = LLC_MT_DEACTIVATE_CNF_PENDING;


        /* Set the cause type as internal cause */
        pdp_list[index]->cause_type  =  SM_INTERNAL_CAUSE;

        /* Just use the first PDP entry for obtaining a connection ID for the
           timers_pending array */
        //timers_pending[TIMER_PENDING] = pdp_list[index].trans_id;

        /* Set the LLC (RAB) Response Timer and wait for the LLC to respond */
        sm_start_timer(index, TIMER_RABM_RESP_PENDING);

        /* ---------------------------------------------------------------------
        ** The PDP context has been activated, but LLC RAB has to be established
        ** before sending the SMREG_PDP_DEACTIVATE_CNF primitive to CM.
        ** --------------------------------------------------------------------*/
        MSG_HIGH_DS_1(SM_SUB,"NSAPI %d DEACTIVATED, Waiting LLC Confirm", pdp_list[index]->nsapi.nsapi);
      }
    }

      /*Deactivate all Primary PDPs*/
    for ( index = 0; index < MAX_PDP_NUM; index ++ )
    {
      if ( pdp_list[index] != NULL &&
           pdp_list[index]->pdp_state != PDP_INACTIVE  &&
           pdp_list[index]->is_primary_pdp)
      {
        //Change the PDP state to INACTIVE_PENDING
        pdp_list[index]->pdp_state = PDP_INACTIVE_PENDING;
        
        pdp_list[index]->current_cause = cause;

        pdp_list[index]->teardown_ind.valid = TRUE;

        pdp_list[index]->teardown_ind.ti_flag = TRUE;

        /* Clear pending timers before notifying SNDCP & upper layers */
        sm_stop_timer (index);
        if(num_active_nsapi < 16)/*size of nsapi array is 16*/
        {
          cmd_ptr->mes.snsm_deactiv_ind.nsapi[num_active_nsapi]  = pdp_list[index]->nsapi.nsapi;
        }
        else
        {
          MSG_ERROR_DS_0(SM_SUB,"Index out of bound and buffer overflow");
        }
        MSG_HIGH_DS_1(SM_SUB,"Sent GSNSM_DEACTIVATE_IND, PDP : %d",pdp_list[index]->nsapi.nsapi);

        num_active_nsapi ++;

        /* Set the llc_state to LLC_CONFIRM_PENDING, start RABM insanity timer */
        pdp_list[index]->llc_state = LLC_MT_DEACTIVATE_CNF_PENDING;


        /* Set the cause type as internal cause */
        pdp_list[index]->cause_type  =  SM_INTERNAL_CAUSE;

        /* Just use the first PDP entry for obtaining a connection ID for the
           timers_pending array */
        //timers_pending[TIMER_PENDING] = pdp_list[index].trans_id;

        /* Set the LLC (RAB) Response Timer and wait for the LLC to respond */
        sm_start_timer(index, TIMER_RABM_RESP_PENDING);

        /* ---------------------------------------------------------------------
        ** The PDP context has been activated, but LLC RAB has to be established
        ** before sending the SMREG_PDP_DEACTIVATE_CNF primitive to CM.
        ** --------------------------------------------------------------------*/
        MSG_HIGH_DS_1(SM_SUB,"NSAPI %d DEACTIVATED, Waiting LLC Confirm", pdp_list[index]->nsapi.nsapi);
      }
    }

    cmd_ptr->mes.snsm_deactiv_ind.num_nsapi = num_active_nsapi;

    /* Send the Primitive to the SNDCP layer */
    gsndcp_snsm_put_mes (cmd_ptr);
  }
  else
  {
    MSG_ERROR_DS_0(SM_SUB, "Unable to get a SNDCP command buffer");
  }
}

#endif /* FEATURE_GSM_GPRS */
