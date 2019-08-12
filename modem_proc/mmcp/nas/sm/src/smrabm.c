/*===========================================================================
                       S M  R A B M   S O U R C E   F I L E 

DESCRIPTION

   This file contains the function definitions between the SM and RABM entities.
   
EXTERNALIZED FUNCTIONS   

  sm_process_rabm_cmd() : This processes a command received on RABM cmd Queue.

  sm_notify_rabm()  :  This function will notify the RABM about a PDP context.
                  
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

$PVCSPath:  L:/src/asw/MSM5200/sm/vcs/smrabm.c_v   1.6   23 Apr 2002 16:02:04   kishores  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/smrabm.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/13/11   am      Trigger PDP context deactivation when a PDP Modification message 
                   is received with a new PDP address
08/14/07  rk      removed FEATURE_NAS_KLOCWORK flag
05/23/07  rk      Some of the commands for RABM are handled through Signals and 
                         some are direct handled by function calls. These difference in handling the commands 
                         gives raise to process the commands in a order different than that of order they were sent.
01/17/06   ss       Not promoting secondary PDP when teardown is set.
 09/13/05  kvk     MO PDP Modify supported. Now handling the Application initiated
                   PDP Modification for QoS and TFT.
06/27/05   kvk     Now sm_modify_rab() is called with flag indicating MO or MT trigger.
04/08/05   kvk     PDP state is verified along with MT orig flag before sending
                   PDP deactivate indication to CM.
10/18/04   kvk     MT PDP suppport added
09/14/04   kvk     sig_connection_available flag is set to true when 
                   sm_proc_reest_ind() is called. Corrected handling of 
                   RABMSM_DEACTIVATE_RESP
09/02/04   kvk     Added handling the streaming and conversational RAB modication
                   indication from RABM. Handled promotions of SEC pdp.
07/22/04   kvk     RABM santiy timer now cleared properly when RABM indicates
                   RAB_activate_rsp or RAB_DEACTIVATE_RSP. 
06/08/04   kvk     PDP con state is reset along with PDP state.
06/07/04   kvk     Added processing for RABM_DEACTIVATE_IMPLICIT in sm_notify_rabm()
                   function.
02/27/03   mks     Added support for PS continuity feature - Added code for
                   notifying RABM about inter-RAT change and also to command
                   RABM to resume its operation on a new RAT.
02/24/03   ks      Fixed LINT warnings.
04/22/02   ks      Added processing of the SM commands in sm_notify_rabm().
08/24/01   ks      Fixed the parameter in sm_notify_rabm() function.
06/04/01   ks      Added a prefix 'SM' to RABM_SM message_set field.
05/17/01   ks      Removed the external function declarations and added to
                   sm.h file.
04/30/01   ks      Added handling of Mobile Terminated notification to CM.
                   Also added more comments.
01/15/01   ks      Created file.

===========================================================================*/

/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "mmcp_variation.h"
#include "sm_v.h"
#include "ms.h"
#include "sm_cm.h"
#include "smix_v.h"
#include "smtask.h"
#include "smtask_v.h"
#include "rabmtask_v.h"
//#include "cause.h"
#include "smtimers.h"

#ifdef FEATURE_DUAL_DATA
extern LOCAL word sm_timer_status_sim[MAX_AS_IDS][MAX_PDP_NUM];    /* RABM Timer Status List */
#define sm_timer_status sm_timer_status_sim[sm_as_id]
#endif
/*===========================================================================
FUNCTION : SM_PROCESS_RABM_REEST_IND 

DESCRIPTION: This function process RABM resestabliment ind command
===========================================================================*/
LOCAL void sm_process_rabm_reest_ind(void);
/*===========================================================================
** FUNCTION DEFINITIONS 
** =========================================================================*/

/*===========================================================================
FUNCTION : SM_PROCESS_RABM_CMD 

DESCRIPTION
  This function processes the commands sent by RABM.

DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/ 
void sm_process_rabm_cmd(sm_cmd_type* cmd_ptr)
{
  byte con_id;
  pdp_context_info *cur_pdp;
  word index;

  MSG_HIGH_DS_0( SM_SUB, "Processing RABM command"); 

  switch (cmd_ptr->header.cmd_id)
  {
    case  RABMSM_ACTIVATE_RSP:    
      /* -------------------------------------------------------------------
      ** Process the RABMSM ACTIVATE_RSP sent by RABM.
      ** -----------------------------------------------------------------*/
      // Get the transaction ID from the received NSAPI value.
      con_id = (byte)GET_TRANSID(cmd_ptr->cmd.rabm_activate_rsp.nsapi);
      if (con_id == INVALID_ID)
      {
        MSG_HIGH_DS_1( SM_SUB,"Receibed RABMSM_ACTIVATE_RSP %d in wrong state", cmd_ptr->cmd.rabm_activate_rsp.nsapi);
        break;
      }
      cur_pdp = pdp_list[con_id];     
      if (cur_pdp == NULL)
      {
          MSG_HIGH_DS_2( SM_SUB, "Received RABM command for rabId: %d, TransId: %d",
                                                cmd_ptr->cmd.rabm_activate_rsp.nsapi, con_id);
          break ;
      }
      MSG_HIGH_DS_1( SM_SUB, "Received RABMSM_ACTIVATE_RSP for RAB %d from RABM",
                                            cur_pdp->nsapi.nsapi);
       
      
      // Set the RAB state for this PDP context
      cur_pdp->rab_state = RABM_ACTIVATED;

      /* --------------------------------------------------------------------
      ** Send Confirmation to CM if the ACCEPT message is received from 
      ** the Network, otherwise wait for the ACCEPT message. 
      ** ------------------------------------------------------------------*/
      if (cur_pdp->pdp_state == PDP_ACTIVE && (sm_timer_status[cur_pdp->trans_id] != (word) TIMER_EARLY_RAB_FAILURE))
      {
        /* Clear RABM Response sanity timer */
        sm_stop_timer(cur_pdp->trans_id); 
#ifdef FEATURE_WCDMA
        if (cur_pdp->is_primary_pdp)
        {
          sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_CNF, SM_INTERNAL_CAUSE);
        }
        else
        {
          sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_SEC_CNF, SM_INTERNAL_CAUSE);
        }
#endif /*#ifdef FEATURE_WCDMA*/
      }
      #ifdef FEATURE_MBMS // CRITICAL_MBMS_PENDING
      #error code not present
#endif

      break;

    case  RABMSM_DEACTIVATE_RSP:
      /* -------------------------------------------------------------------
      ** Process the RABMSM RABMSM_DEACTIVATE_RSP sent by RABM.
      ** -----------------------------------------------------------------*/
      // Get the transaction ID from the received NSAPI value.
      con_id = (byte)GET_TRANSID(cmd_ptr->cmd.rabm_deactivate_rsp.nsapi);
      if (con_id == INVALID_ID)
      {
         MSG_HIGH_DS_1( SM_SUB,"Receibed RABMSM_DEACTIVATE_RSP %d in wrong state", cmd_ptr->cmd.rabm_deactivate_rsp.nsapi);
         break;
      }

      cur_pdp = pdp_list[con_id];     
      if (cur_pdp == NULL)
      {
          MSG_HIGH_DS_2( SM_SUB, "Received RABM command for rabId: %d, TransId: %d",
                                                cmd_ptr->cmd.rabm_deactivate_rsp.nsapi, con_id);
          break ;
      }
      MSG_HIGH_DS_2( SM_SUB, "Received RABMSM_DEACTIVATE_RSP for NSAPI: %d, TI: %d",cur_pdp->nsapi.nsapi, cur_pdp->trans_id);

       // Set the RAB state for this PDP context
       cur_pdp->rab_state = RABM_DEACTIVATED;

      // Check if the PDP ACCEPT message has been received.
      // RABMSM_DEACTIVATE_RSP before sending the confirmation to the CM.
      if (cur_pdp->cm_pending_msg == (int)SMREG_PDP_ACTIVATE_REQ &&
          cur_pdp->pdp_state == PDP_INACTIVE)              
      {
        
        //reset the PDP con state
        cur_pdp->pdp_con_state = SIG_CONNECTION_NONE;

        cur_pdp->current_cause = (cc_cause_T)RAB_SETUP_FAILURE;
#ifdef FEATURE_WCDMA
        sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_REJ, SM_INTERNAL_CAUSE);
#endif /*#ifdef FEATURE_WCDMA*/
      }
      else if(cur_pdp->cm_pending_msg == (int)SMREG_PDP_ACTIVATE_SEC_REQ &&
              cur_pdp->pdp_state == PDP_INACTIVE)
      {
         //reset the PDP con state
        cur_pdp->pdp_con_state = SIG_CONNECTION_NONE;

        cur_pdp->current_cause = (cc_cause_T)RAB_SETUP_FAILURE;

#ifdef FEATURE_WCDMA
        sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_SEC_REJ, SM_INTERNAL_CAUSE);
#endif /*#ifdef FEATURE_WCDMA*/
      }
      else if (cur_pdp->cm_pending_msg == (int)SMREG_PDP_MODIFY_REQ)
      {
        cur_pdp->pdp_state = PDP_ACTIVE;
#ifdef FEATURE_WCDMA
        sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_REJ, SM_INTERNAL_CAUSE);
#endif /*#ifdef FEATURE_WCDMA*/
      }
      else if ((cur_pdp->cm_pending_msg == (int)SMREG_PDP_DEACTIVATE_REQ) && 
                  (cur_pdp->pdp_state == PDP_INACTIVE))
      {
        /* Clear RABM Response sanity timer */
        sm_stop_timer(cur_pdp->trans_id);
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL 
        if (cur_pdp->is_primary_pdp && !cur_pdp->teardown_ind.ti_flag && sm_promote_sec_pri(cur_pdp))
        {
          //Send the SMREG_PDP_DEACTIVATE_PROMOTE_IND to the CM
#ifdef FEATURE_WCDMA
          sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_PROMOTE_CNF, SM_INTERNAL_CAUSE);
#endif /*#ifdef FEATURE_WCDMA*/
        }
        else
        {
          if ( cur_pdp->is_primary_pdp 
#ifdef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL 
               && cur_pdp->teardown_ind.ti_flag
#endif
        )
          {
            for (index = 0; index < MAX_PDP_NUM; index++)
            {
                if ((pdp_list[index] != NULL) &&
                    cur_pdp->trans_id != index && 
                    cur_pdp->group_id == pdp_list[index]->group_id )
                  {
                    MSG_HIGH_DS_1( SM_SUB,"Locally deactivating TI: %d", pdp_list[index]->trans_id);
                    sm_stop_timer(pdp_list[index]->trans_id);
                    sm_notify_rabm ((int)RABMSM_DEACTIVATE_IND, pdp_list[index]->nsapi.nsapi);
                    #ifdef FEATURE_WCDMA
                    if(cur_pdp->mt_orig_type == TRUE)
                    {
                      sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_IND, SM_NETWORK_CAUSE);
                    }
                    else
                    {
                      sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_CNF, SM_INTERNAL_CAUSE);
                    }
                    #endif /*#ifdef FEATURE_WCDMA*/
                  }
            }
          }
#endif
          #ifdef FEATURE_WCDMA
          if(cur_pdp->mt_orig_type == TRUE)
          {
            // Send the SMREG_PDP_DEACTIVATE_IND to the CM.
            sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_IND, SM_NETWORK_CAUSE);         
          }
          else
          {
            // Send the SMREG_PDP_DEACTIVATE_CNF to the CM.
            sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_CNF, SM_INTERNAL_CAUSE);          
          }
          #endif /*#ifdef FEATURE_WCDMA*/
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
        }
#endif
      }


      // Send the SMREG_PDP_DEACTIVATE_IND to the CM for Mobile terminated req.
      else if (cur_pdp->mt_orig_type && cur_pdp->pdp_state == PDP_INACTIVE)
      {
        /* Clear RABM Response sanity timer */
        sm_stop_timer(cur_pdp->trans_id); 
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
        if (cur_pdp->is_primary_pdp
          && !cur_pdp->teardown_ind.ti_flag
          &&sm_promote_sec_pri(cur_pdp))
        {
#ifdef FEATURE_WCDMA
           sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_PROMOTE_IND,
                                                         SM_NETWORK_CAUSE);
#endif /*#ifdef FEATURE_WCDMA*/
        }
        else
        {
#endif
          if ( cur_pdp->is_primary_pdp 
#ifdef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
              && cur_pdp->teardown_ind.ti_flag
#endif
             )
          {
            for (index = 0; index < MAX_PDP_NUM; index++)
            {
                if ((pdp_list[index] != NULL) &&
                    cur_pdp->trans_id != index && 
                    cur_pdp->group_id == pdp_list[index]->group_id )
                  {
                    MSG_HIGH_DS_1( SM_SUB,"Locally deactivating TI: %d", pdp_list[index]->trans_id);
                    sm_stop_timer(pdp_list[index]->trans_id);
                    sm_notify_rabm ((int)RABMSM_DEACTIVATE_IND, pdp_list[index]->nsapi.nsapi);
#ifdef FEATURE_WCDMA
                    sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_IND, SM_INTERNAL_CAUSE);
#endif /*#ifdef FEATURE_WCDMA*/
                  }
            }
          }
#ifdef FEATURE_WCDMA
          sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_IND, SM_NETWORK_CAUSE);
#endif /*#ifdef FEATURE_WCDMA*/
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
        }
#endif
      }

      break;

    case  RABMSM_MODIFY_RSP:
      /* -------------------------------------------------------------------
      ** Process the RABMSM RABMSM_MODIFY_RSP sent by RABM.
      ** -----------------------------------------------------------------*/
      // Get the transaction ID from the received NSAPI value.
      con_id = (byte)GET_TRANSID(cmd_ptr->cmd.rabm_modify_rsp.nsapi);
      if (con_id == INVALID_ID)
      {
         MSG_HIGH_DS_1( SM_SUB,"Receibed RABMSM_MODIFY_RSP %d in wrong state", cmd_ptr->cmd.rabm_modify_rsp.nsapi);
         break;
      }
      cur_pdp = pdp_list[con_id];     
      if (cur_pdp == NULL)
      {
          MSG_HIGH_DS_2( SM_SUB, "Received RABM command for rabId: %d, TransId: %d",
                                                cmd_ptr->cmd.rabm_modify_rsp.nsapi, con_id);
          break ;
      }
      MSG_HIGH_DS_1( SM_SUB, "Received RABMSM_MODIFY_RSP for RAB %d  from RABM",
                                               cur_pdp->nsapi.nsapi);

      
      // Set the RAB state for this PDP context
      cur_pdp->rab_state = RABM_MODIFIED;

      // Check if the PDP ACCEPT message has been received.
      // RABMSM_MODIFY_RSP before sending the confirmation to the CM.
      if (cur_pdp->mt_orig_type)
      {
        // set pdp context to active, because it is Mobile Terminated. 
        cur_pdp->pdp_state = PDP_ACTIVE;
        /* Clear RABM Response sanity timer */
        sm_stop_timer(cur_pdp->trans_id); 
        // Send the SMREG_PDP_MODIFY_IND to the CM for Mobile terminated req.
#ifdef FEATURE_WCDMA
        sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_IND, SM_NETWORK_CAUSE);
#endif /*#ifdef FEATURE_WCDMA*/
      }
      else if (cur_pdp->pdp_state == PDP_ACTIVE)
      {
        /* Clear RABM Response sanity timer */
        sm_stop_timer(cur_pdp->trans_id); 
#ifdef FEATURE_WCDMA
        if (cur_pdp->cm_pending_msg == (int)SMREG_PDP_MODIFY_REQ)
        {
          // Send the SMREG_PDP_MODIFY_CNF to the CM.
          sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_CNF, SM_INTERNAL_CAUSE);
        }
        else
        {
          // Send the SMREG_PDP_MODIFY_IND to the CM.
          sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_IND, SM_INTERNAL_CAUSE);
        }
#endif /*#ifdef FEATURE_WCDMA*/
      }

      break;

    case  RABMSM_STATUS_REQ: 
      con_id = (byte)GET_TRANSID(cmd_ptr->cmd.rabm_status_req.nsapi);
      if (con_id == INVALID_ID)
      {
         MSG_HIGH_DS_1( SM_SUB,"Receibed RABMSM_STATUS_REQ %d in wrong state", cmd_ptr->cmd.rabm_status_req.nsapi);
         break;
      }
      cur_pdp = pdp_list[con_id];     
      if (cur_pdp == NULL)
      {
          MSG_HIGH_DS_2( SM_SUB, "Received RABM command for rabId: %d, TransId: %d",
                                                cmd_ptr->cmd.rabm_status_req.nsapi, con_id);
          break ;
      }
      MSG_HIGH_DS_0( SM_SUB,"Received RABMSM_STATUS_REQ");
      // cur_pdp->rab_state = RABM_DEACTIVATED;
      if (cur_pdp->pdp_state == PDP_ACTIVE && 
         cur_pdp->rab_state == RABM_ACTIVE_PENDING &&
         cur_pdp->cm_pending_msg == (int)SMREG_PDP_ACTIVATE_REQ )
      {
        /*stop the timer*/
        sm_stop_timer(cur_pdp->trans_id);
        /*Set the cause as RAB_SETUP_FAILURE*/
        cur_pdp->current_cause = (cc_cause_T)RAB_SETUP_FAILURE;
        /*Send Modify notification to CM*/
#ifdef FEATURE_WCDMA
        if (TRUE == cur_pdp->is_primary_pdp)
        {
          sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_REJ, SM_INTERNAL_CAUSE);
        }
        else
        {
          sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_SEC_REJ, SM_INTERNAL_CAUSE);
        }
#endif /*#ifdef FEATURE_WCDMA*/
      }
      else if (cur_pdp->pdp_state == PDP_ACTIVE && 
        cur_pdp->rab_state == RABM_MODIFY_PENDING )
      {
        /*stop the timer*/
        sm_stop_timer(cur_pdp->trans_id);
        /*Set the cause as RAB_SETUP_FAILURE*/
        cur_pdp->current_cause = (cc_cause_T)RAB_SETUP_FAILURE;
        /*Send Modify notification to CM*/
#ifdef FEATURE_WCDMA
        (cur_pdp->mt_orig_type)? sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_IND, SM_INTERNAL_CAUSE):
                               sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_CNF, SM_INTERNAL_CAUSE);
#endif /*#ifdef FEATURE_WCDMA*/
        cur_pdp->rab_state = RABM_DEACTIVATED;
      }
      else
      {
        cur_pdp->rab_state = RABM_DEACTIVATED;
      }
      break;

    case RABMSM_REESTABLISH_IND:
      MSG_HIGH_DS_0( SM_SUB,"Received RABMSM_REETABLISH_IND from RABM");
      /*Initiate the PDP context modification procedure for PDPs
        with traffic class streaming and conversational*/
      sm_process_rabm_reest_ind();
    break;

    default:
      MSG_ERROR_DS_0( SM_SUB,"Received unknown msg from RABM");
    break;
  }
}
/*===========================================================================
FUNCTION : SM_NOTIFY_RABM 

DESCRIPTION
  This function sends a command to the RABM subtask.

DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_notify_rabm (int rab_cmd_id, byte nsapi )
{
  rabm_cmd_type *rabm_cmd;
  rabm_cmd = rabm_get_cmd_buf();  /* Get rabm command */
  if (rabm_cmd != NULL)
  {
    rabm_cmd->header.cmd_id = (rabm_cmd_enum_type)rab_cmd_id;
    rabm_cmd->header.message_set = MS_RABM_SM;
    MSG_HIGH_DS_0( SM_SUB, "Sending Primitive to RABM");
    switch (rab_cmd_id)
    {
      case RABMSM_ACTIVATE_IND:
        rabm_cmd->cmd.sm_activate_ind.nsapi = nsapi;   
#ifdef FEATURE_DUAL_SIM
        rabm_cmd->cmd.sm_activate_ind.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
      break;

      case RABMSM_DEACTIVATE_IND:
      case RABMSM_DEACTIVATE_IMPLICIT_IND:
        rabm_cmd->cmd.sm_deactivate_ind.nsapi = nsapi;
#ifdef FEATURE_DUAL_SIM
        rabm_cmd->cmd.sm_deactivate_ind.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
      break;

      case RABMSM_MODIFY_IND:
        rabm_cmd->cmd.sm_modify_ind.nsapi = nsapi;
#ifdef FEATURE_DUAL_SIM
       rabm_cmd->cmd.sm_modify_ind.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
      break;

      case RABMSM_SIG_CON_RELEASE_IND:
      // no need to set the the nsapi here
#ifdef FEATURE_DUAL_SIM
      rabm_cmd->cmd.sm_release_ind.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
      break;          

      case RABMSM_RAT_CHANGE_IND:
#ifdef FEATURE_DUAL_SIM
      rabm_cmd->cmd.sm_rat_change_ind.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
      case RABMSM_RAT_RESUME_IND:
#ifdef FEATURE_DUAL_SIM
      rabm_cmd->cmd.sm_rat_resume_ind.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
        break;
            
      default:
        MSG_ERROR_DS_0( SM_SUB,"Unknown RABM command ");
      break;
    }

    rabm_put_cmd(rabm_cmd);
  }
  else
  {
    MSG_ERROR_DS_0( SM_SUB, "Unable to get a RABM command buffer");
  }
}


/*===========================================================================
FUNCTION : SM_PROCESS_RABM_REEST_IND 

DESCRIPTION
  This function process RABM resestabliment ind command

DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_process_rabm_reest_ind(void)
{
  //Set the sig_connection_available flag to true
  sig_connection_available = TRUE;

  /*GMM indicates the PS signalling connection due to the RRC page ind. Iniate the 
  RAB modification procedures for any RAB which is in dormant with traffic class
  straming or conversational*/
  sm_modify_rab(FALSE);
}
