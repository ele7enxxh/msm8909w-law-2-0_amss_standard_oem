
/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_cnm_handler.c#1 $

when       who     what
--------   ---     ---------------------------------------------------------- 
12/26/11   rajesh   Allowing CSFB call to go through if establishment request is for CSVT 
10/01/11   abhi    Processing REL_REQ only if MO_CSFB call or SMS is active
08/22/11   MNK     CR: 301219 Clear CSFB context and call if aborted by the upper layer  
07/01/10   zren    Updated to include correct length for MMCNM_DATA_IND 

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>

#ifdef FEATURE_LTE

#include "msgr_nas.h"
#include "emm_cnm_handler.h"
#include "emm_service_request_lib.h"
#include "emm_utility.h"
#include "emm_rrc_if.h"
#include "stringl.h"

/*===========================================================================

FUNCTION EMM_SEND_SMS_DATA_REQ_FAILED

DESCRIPTION
  This function handle SMS data req failed and inform CNM to take action.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_sms_data_req_failed
(
  /* RRC failure cause */
  lte_emm_connection_cause_type   cause,

  /* Message ID that this callback is assosiated with */
  dword                           trans_id,

  /* Failure cause type and cause */
  emm_failure_type                emm_failure_cause,

  /* The following TAI is valid only with 
     TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO */
  lte_nas_tai_lst1_type           tai,

  /* Pointer to the massage which failed to be tx */
  struct emm_pended_msg_list_tag *message_ptr,

  /* EMM control data */
  emm_ctrl_data_type             *emm_ctrl_data_ptr
)
{
  mmcc_sync_ind_T            mmcc_sync_ind={{0}};
  boolean is_pended_cp_data = FALSE;
  emm_pended_msg_list_type *pended_message_ptr = (struct emm_pended_msg_list_tag *)message_ptr ;
  ASSERT(pended_message_ptr != (emm_pended_msg_list_type *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  is_pended_cp_data = is_cp_data(pended_message_ptr->nas_out_message.emm_outgoing_msg.\
                        ul_nas_transport.nas_msg_container.msg_payload);
// need to check if following can also move to under condition
// in 3G what if call back is called but no sync indication
  if(is_pended_cp_data == TRUE)
  {
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
    if(mm_sub_id == MM_AS_ID_1)
    {
      CNM_sms_cp_data_l2_ack(FALSE);
    }
    else
    {
      CNM_sms_cp_data_l2_ack_subs_2(FALSE);
    }
#else
    CNM_sms_cp_data_l2_ack(FALSE);
#endif
  }

  if((is_pended_cp_data==TRUE)&&(cause==TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO)&&
      (emm_ctrl_data_ptr->emm_sms_active==TRUE)&&
      ( emm_ctrl_data_ptr->emm_connection_state== EMM_CONNECTED_STATE) &&
      ((emm_ctrl_data_ptr->emm_state== EMM_REGISTERED && 
        emm_ctrl_data_ptr->emm_substate== (emm_substate_type)EMM_REGISTERED_NORMAL_SERVICE ) ||
       (emm_ctrl_data_ptr->emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED))&&         
       (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED) &&
       (emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                    SYS_SRV_STATUS_SRV) &&
       (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                      SYS_SIM_STATE_AVAILABLE)
#ifdef FEATURE_LTE_REL11
          &&
       ((emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL11) 
          ||
       ((mm_timer_status[TIMER_T3346] == TIMER_STOPPED) ||
       (emm_check_high_priority(emm_ctrl_data_ptr)) ||
       (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE)))
#endif
        && (mm_timer_status[TIMER_T3440] == TIMER_STOPPED))
  {   
    mmcc_sync_ind.header.message_set = MS_CC_MM;
    mmcc_sync_ind.header.message_id  = MMCC_SYNC_IND;
    PUT_IMH_LEN( sizeof(mmcc_sync_ind_T) - sizeof(IMH_T),
                                  &mmcc_sync_ind.header );
    mmcc_sync_ind.reason = (int)LTE_TRANSPORT_ESTABLISHED;
   
    MSG_HIGH_DS_0(MM_SUB,"=EMM= EMM sent MMCC_SYNC_IND");
    mm_send_message( &mmcc_sync_ind.header, GS_QUEUE_MN_CM );
    
  }
  else
  {
    MSG_HIGH_DS_8(MM_SUB,"=EMM= Ignored SMS retry, sms active = %d \
       connection = %d,emm_state = %d  emm_ctrl_data_ptr->emm_update_status = %d\
        emm_ctrl_data_ptr->plmn_service_state->service_status= %d\
        timer t3346 = %d  timer t3440 = %d  cause = %d is_cp_data = %d ",
                           emm_ctrl_data_ptr->emm_sms_active,
                           emm_ctrl_data_ptr->emm_connection_state,
                           emm_ctrl_data_ptr->emm_update_status,
                           emm_ctrl_data_ptr->plmn_service_state->service_status,
                           mm_timer_status[TIMER_T3346],mm_timer_status[TIMER_T3440],
                           cause,is_pended_cp_data);
  }

  pended_message_ptr->message_sent_indicator = TRUE ;
  /* Do not perform TAU procedure */
  return FALSE ;

}


/*===========================================================================

FUNCTION EMM_SMS_DATA_REQ_SUCCEEDED

DESCRIPTION
  This function send sucess status to CNM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_sms_data_req_suceeded
(
   dword trans_id
)
{
  
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
  if(mm_sub_id == MM_AS_ID_1)
  {
    CNM_sms_cp_data_l2_ack(TRUE);
  }
  else
  {
    CNM_sms_cp_data_l2_ack_subs_2(TRUE);
  }
#else
  CNM_sms_cp_data_l2_ack(TRUE);
#endif
}


/*===========================================================================

FUNCTION EMM_SEND_MMCNM_EST_CNF

DESCRIPTION
  This function sends MMCNM_EST_CNF.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_send_mmcnm_est_cnf
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  mmcnm_est_cnf_T  mmcnm_est_cnf;

  ASSERT(emm_ctrl_data_ptr != NULL);

  memset((void *)&mmcnm_est_cnf, 0x0, sizeof(mmcnm_est_cnf_T));

  mmcnm_est_cnf.header.message_set = MS_CC_MM_PS;
  mmcnm_est_cnf.header.message_id  = MMCNM_EST_CNF;

  /*lint -save -e778 */
  PUT_IMH_LEN(sizeof(mmcnm_est_cnf_T) - sizeof(IMH_T), &mmcnm_est_cnf.header);
  /*lint -restore */

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending MMCNM_EST_CNF");
  mm_send_message((IMH_T*)&mmcnm_est_cnf, GS_QUEUE_MN_CM);

} /* emm_send_mmcnm_est_cnf */

/*===========================================================================

FUNCTION EMM_SEND_MMCNM_EST_REJ

DESCRIPTION
  This function sends MMCNM_EST_REJ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_send_mmcnm_est_rej
( 
  reject_type_enum_T   cause_type,
  byte                 cause_value,
  emm_ctrl_data_type  *emm_ctrl_data_ptr 
)
{
  mmcnm_est_rej_T  mmcnm_est_rej;

  ASSERT(emm_ctrl_data_ptr != NULL);

  memset((void *)&mmcnm_est_rej, 0x0, sizeof(mmcnm_est_rej_T));

  mmcnm_est_rej.header.message_set = MS_CC_MM_PS;
  mmcnm_est_rej.header.message_id  = MMCNM_EST_REJ;

  mmcnm_est_rej.cause_type = cause_type;
  mmcnm_est_rej.cause_value = cause_value;

#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
  mmcnm_est_rej.domain = MMCC_SMS_PS;
#endif

  /*lint -save -e778 */
  PUT_IMH_LEN(sizeof(mmcnm_est_rej_T) - sizeof(IMH_T), &mmcnm_est_rej.header);
  /*lint -restore */

  MSG_HIGH_DS_2(MM_SUB, "=EMM= Sending MMCNM_EST_REJ w/ type %u, value %d",
                cause_type,
                cause_value);

  mm_send_message((IMH_T*)&mmcnm_est_rej, GS_QUEUE_MN_CM);

  emm_ctrl_data_ptr->emm_sms_active = FALSE;

} /* emm_send_mmcnm_est_rej */

/*===========================================================================

FUNCTION EMM_SEND_MMCNM_REL_IND

DESCRIPTION
  This function sends MMCNM_REL_IND to CNM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_send_mmcnm_rel_ind
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr,
  lower_layer_cause_T  rel_cause  
)
{
  mmcnm_rel_ind_T  mmcnm_rel_ind;

  ASSERT(emm_ctrl_data_ptr != NULL);

  memset((void *)&mmcnm_rel_ind, 0x0, sizeof(mmcnm_rel_ind_T));

  mmcnm_rel_ind.header.message_set = MS_CC_MM_PS;
  mmcnm_rel_ind.header.message_id  = MMCNM_REL_IND;
  mmcnm_rel_ind.rel_cause = rel_cause;

  /*lint -save -e778 */
  PUT_IMH_LEN(sizeof(mmcnm_rel_ind_T) - sizeof(IMH_T), &mmcnm_rel_ind.header);
  /*lint -restore */

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending MMCNM_REL_IND");
  mm_send_message((IMH_T*)&mmcnm_rel_ind, GS_QUEUE_MN_CM);

  emm_ctrl_data_ptr->emm_sms_active = FALSE;

} /* emm_send_mmcnm_rel_ind */

/*===========================================================================

FUNCTION EMM_PROCESS_MMCNM_REL_REQ

DESCRIPTION
  This function processes MMCNM_REL_REQ and will send MMCNM_REL_CNF if SMS is
  active on EMM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_process_mmcnm_rel_req
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  mmcnm_rel_cnf_T  mmcnm_rel_cnf;

  ASSERT(emm_ctrl_data_ptr != NULL);

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Rcved MMCNM_REL_REQ");

  /* Always send a cnf directly. 
     No need to tear down the connection. */
  memset((void *)&mmcnm_rel_cnf, 0x0, sizeof(mmcnm_rel_cnf_T));
  mmcnm_rel_cnf.header.message_set = MS_CC_MM;
  mmcnm_rel_cnf.header.message_id  = MMCNM_REL_CNF;

  /*lint -save -e778 */
  PUT_IMH_LEN(sizeof(mmcnm_rel_cnf_T) - sizeof(IMH_T), &mmcnm_rel_cnf.header);
  /*lint -restore */

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending MMCNM_REL_CNF");
  mm_send_message((IMH_T*)&mmcnm_rel_cnf, GS_QUEUE_MN_CM);

  /* Reset flag */
  emm_ctrl_data_ptr->emm_sms_active = FALSE;

} /* emm_process_mmcnm_rel_req */

/*===========================================================================

FUNCTION EMM_PROCESS_MMCNM_EST_REQ

DESCRIPTION
  This function processes MMCNM_EST_REQ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_process_mmcnm_est_req
( 
  mm_cmd_type         *cc_msg_ptr, 
  emm_ctrl_data_type  *emm_ctrl_data_ptr 
)
{
  emm_csfb_rsp                     csfb_rsp;
  ASSERT(cc_msg_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL);
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Processing MMCNM_EST_REQ");

  memset((void *)&csfb_rsp,0,sizeof(emm_csfb_rsp));

  if(((EMM_GET_STATE() == EMM_REGISTERED) && 
      (EMM_GET_SUBSTATE() == (emm_substate_type)EMM_REGISTERED_NORMAL_SERVICE))
     &&
     (emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                            SYS_SRV_STATUS_SRV)
     &&
     (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED)
     &&
     (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                         SYS_SIM_STATE_AVAILABLE)
     &&
     (emm_reg_container == NULL)
#ifdef FEATURE_LTE_REL11 
     &&
     ((emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL11) ||
      (mm_timer_status[TIMER_T3346] == TIMER_STOPPED)||
      (emm_check_high_priority(emm_ctrl_data_ptr)) ||
      (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE))
#endif
     &&(emm_ctrl_data_ptr->aborting_conn == FALSE)
    )
  {
    emm_ctrl_data_ptr->emm_sms_active = TRUE;

	if(EMM_GET_CONNECTION_STATE() != EMM_CONNECTED_STATE)
    {
      if(emm_check_esr_for_ps(emm_ctrl_data_ptr) == TRUE)
      {
        /* Start extended service request procedure for PS */
        emm_start_nas_extended_service_request_procedure(LTE_RRC_EST_CAUSE_MO_DATA,LTE_PACKET_SERVICES_VIA_S1,
                                                         csfb_rsp, NAS_EMM_CSFB_NONE);
      }
      else
      {
        /* Start service request procedure with the cause of MO_DATA */
        emm_start_nas_service_request_procedure(emm_ctrl_data_ptr,
                                                LTE_RRC_EST_CAUSE_MO_DATA);
      }
    }
    else
    {
      /* Connection exists. Send est cnf directly. */
      emm_send_mmcnm_est_cnf(emm_ctrl_data_ptr);
    }
  }
  else if(EMM_GET_STATE() == EMM_TRACKING_AREA_UPDATING_INITIATED)
  {
      /* SMS could go through after TAU/SR procedure done. */
    emm_send_mmcnm_est_rej(EMM_REJECT_CAUSE, 
                           EMM_REJ_TEMP_STATE, 
                           emm_ctrl_data_ptr);
  }
  else
  {
    /* Invalid state. Reject est req. */
    emm_send_mmcnm_est_rej(MM_REJECT_CAUSE, 
                           MM_REJ_WRONG_STATE, 
                           emm_ctrl_data_ptr);
  }

} /* emm_process_mmcnm_est_req */

/*===========================================================================

FUNCTION EMM_PROCESS_CNM_COMMAND

DESCRIPTION
  This function processes commands from CNM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_process_cnm_command
( 
  mm_cmd_type         *cnm_msg_ptr, 
  emm_ctrl_data_type  *emm_ctrl_data_ptr 
)
{
  emm_state_type emm_state = EMM_INVALID_STATE; 

  ASSERT(cnm_msg_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL);

  emm_state = EMM_GET_STATE();

  if((cnm_msg_ptr->cmd.hdr.message_set != MS_CC_MM) &&
     (cnm_msg_ptr->cmd.hdr.message_set != MS_CC_MM_PS))
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= Can not handle message set %u",
                   cnm_msg_ptr->cmd.hdr.message_set);
    return;
  }

  MSG_HIGH_DS_2(MM_SUB, "=EMM= Rcved MS_CC_MM msg %d in srv domain %d",
                cnm_msg_ptr->cmd.hdr.message_id,
                emm_ctrl_data_ptr->plmn_service_state->reg_service_domain);

  switch(cnm_msg_ptr->cmd.hdr.message_id)
  {
    case MMCNM_EST_REQ:
      if((emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                                          SYS_SRV_DOMAIN_CS_PS)
         &&
         (cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type
                                       == SHORT_MESSAGE_SERVICE_ESTABLISHMENT))
      {
        emm_process_mmcnm_est_req(cnm_msg_ptr, emm_ctrl_data_ptr);
      }
      else if((((emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                                          SYS_SRV_DOMAIN_CS_PS)&&
                 (emm_state != EMM_REGISTERED_INITIATED) &&
                 (emm_state != EMM_DEREGISTERED_INITIATED))||
                (emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED)||
                ((mm_timer_status[TIMER_T3411] == TIMER_ACTIVE) &&
                (emm_ctrl_data_ptr->TIN != NAS_MM_TIN_GUTI))||
                (emm_ctrl_data_ptr->cell_barred_tau_pending==TRUE))&&
              (cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type
                                    == MO_CALL_ESTABLISHMENT  ||
               cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type 
                                    == MO_CALL_ESTABLISHMENT_HALF_SPEECH ||
               cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type 
                                   == MO_CALL_ESTABLISHMENT_FULL_SPEECH ||
               cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type 
                                  == EMERGENCY_CALL_ESTABLISHMENT ||
               cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type 
                                  == SUPPLEMENTARY_SERVICE_ACTIVATION ||
               cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type 
                                  == MO_CALL_ESTABLISHMENT_DATA_9600 ||
               cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type 
                                  == MO_CALL_ESTABLISHMENT_DATA_4800))
      {
        mm_csfb_start_mo_call_orig(cnm_msg_ptr);
      }

      else
      {
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Rejecting MMCNM_EST_REQ w/ service type - %u",
                      cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type);
        if(cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type 
                                  == EMERGENCY_CALL_ESTABLISHMENT)
        {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
          mm_send_mmcnm_est_rej( AS_REJECT_CAUSE, AS_REJ_LRRC_LOWER_LAYER_FAILURE , MMCC_SMS_CS);
#else
          mm_send_mmcnm_est_rej( AS_REJECT_CAUSE, AS_REJ_LRRC_LOWER_LAYER_FAILURE );
#endif
        }
        else if(emm_state == EMM_REGISTERED_INITIATED ||
                emm_state == EMM_DEREGISTERED_INITIATED)
        {
          emm_send_mmcnm_est_rej(MM_REJECT_CAUSE, 
                                 MM_REJ_WRONG_STATE,
                                 emm_ctrl_data_ptr);
        }
        else
        {
          emm_send_mmcnm_est_rej(MM_REJECT_CAUSE, 
                                 MM_REJ_NO_SERVICE,
                                 emm_ctrl_data_ptr);
        }
      }
      break;
    case MMCNM_DATA_REQ:
      if((emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                                          SYS_SRV_DOMAIN_CS_PS)
         &&
         ((cnm_msg_ptr->cmd.mmcnm_data_req.data[0] & 0x0F) == (byte)SMS_MESSGES) &&
         (emm_ctrl_data_ptr->rrc_active == TRUE))
      {
        emm_process_ul_sms(cnm_msg_ptr, emm_ctrl_data_ptr);
      }
      else
      {
        MSG_ERROR_DS_2(MM_SUB, "=EMM= Discarding MMCNM_DATA_REQ w/ PD %d / rrc_active %d",
                       cnm_msg_ptr->cmd.mmcnm_data_req.data[0] & 0x0F,
                       emm_ctrl_data_ptr->rrc_active);
      }
      break;
    case MMCNM_REL_REQ:
      emm_process_mmcnm_rel_req(emm_ctrl_data_ptr);
      /* Remember the ESR procedure in case UE gets redirected
      ** to GSM later. This is to remeber if PS data needs to be resumed later */
      if((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI) &&
         (mm_csfb_get_state() != MM_CSFB_STATE_NULL))
      {
        emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_ESR_STATE;
      }
      if(mm_csfb_get_state() == MM_CSFB_STATE_MO_CALL_ORIG)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= MO CSFB Call ended");
        mm_csfb_clear_csfb_call();
        /*set this flag to know if blind redirection indication needs to be sent to rr*/
        emm_ctrl_data_ptr->rr_blind_redir_pending_call_abort = TRUE;
        /* Autonomously clean up the pended ESR messages and reset the ESR type and STOP the TIMER*/
        /* It is a conscious decision to let the ESR run even after the call is borted by the upper lyaer/user. If UE is redirected it will go to GW.
         If ESR fails, it will not be retransmitted or failure is indicated to the upper layers */
        emm_abort_and_clean_csfb_call(emm_ctrl_data_ptr);
      }
      break;
    default:
      MSG_ERROR_DS_1(MM_SUB, "=EMM= Discarding MS_CC_MM msg - %d",
                     cnm_msg_ptr->cmd.hdr.message_id);
      break;
  }

} /* emm_process_cnm_command */

/*===========================================================================

FUNCTION EMM_PROCESS_DL_NAS_TRANSPORT

DESCRIPTION
  This function passes SMS carried by downlink nas transport message to CNM 
  SMS handler

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_dl_nas_transport
(
  lte_nas_emm_dl_nas_transport  *dl_nas_transport_ptr
)
{
  word  msg_len = 0;
  mmcnm_data_ind_T  mmcnm_data_ind;

  ASSERT(dl_nas_transport_ptr != NULL);

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending MMCNM_DATA_IND w/ SMS rcved in NAS Transport msg");

#ifdef FEATURE_LTE_REL11 
  if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)
  {
    mm_stop_timer(TIMER_T3246);
  }
#endif

  /* Init */
  memset((void *)&mmcnm_data_ind, 0x0, sizeof(mmcnm_data_ind_T));

  mmcnm_data_ind.msg_head.IMH.message_set = MS_CC_MM_PS;
  mmcnm_data_ind.msg_head.IMH.message_id = MMCNM_DATA_IND;

  /* SMS format 
     Byte 1                     Protocol Discriminator + Transaction Identifier
     Byte 2                     Message Type
     Byte 3 - 251(Optional)     User data  */


  /*lint -save -e778 */
  PUT_IMH_LEN(dl_nas_transport_ptr->nas_msg_container.msg_len, 
              &mmcnm_data_ind.msg_head.IMH);
  /*lint -restore */

  ASSERT((dl_nas_transport_ptr->nas_msg_container.msg_len >= EMM_SMS_DATA_OFFSET)&&
         (dl_nas_transport_ptr->nas_msg_container.msg_len <= MAX_OCTETS_L3_MESSAGE));

  msg_len = dl_nas_transport_ptr->nas_msg_container.msg_len - 
                                                           EMM_SMS_DATA_OFFSET;

  mmcnm_data_ind.msg_head.ti_pd = 
      dl_nas_transport_ptr->nas_msg_container.msg_payload[EMM_SMS_TIPD_OFFSET];

  mmcnm_data_ind.msg_head.type = 
      dl_nas_transport_ptr->nas_msg_container.msg_payload[EMM_SMS_TYPE_OFFSET];

  if(msg_len > 0)
  {
    if(msg_len <= (MAX_OCTETS_L3_MESSAGE - EMM_SMS_DATA_OFFSET))
    {
      memscpy((void *)&mmcnm_data_ind.data[0], 
              sizeof(mmcnm_data_ind.data),
             (void *)&dl_nas_transport_ptr->nas_msg_container.\
                                              msg_payload[EMM_SMS_DATA_OFFSET],
             msg_len);
    }
    else
    {
      MSG_ERROR_DS_1(MM_SUB, "=EMM= DL SMS is too long to be sent - %d", msg_len);
    }
  }
#ifdef FEATURE_DUAL_SIM
  /* Append AS ID */
  {
    uint2 length;
    GET_IMH_LEN(length, &mmcnm_data_ind.msg_head.IMH);
    mmcnm_data_ind.data[msg_len] = (byte) mm_sub_id;
    PUT_IMH_LEN((length + sizeof(byte)), &mmcnm_data_ind.msg_head.IMH);
  }
#endif
  

  mm_send_message((IMH_T*)&mmcnm_data_ind, GS_QUEUE_MN_CM);

} /* emm_process_dl_nas_transport */

/*===========================================================================

FUNCTION EMM_PROCESS_UL_SMS

DESCRIPTION
  This function passes uplink SMS messages to NW

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_ul_sms
(
  mm_cmd_type         *cnm_msg_ptr, 
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  emm_state_type                 emm_state;
  emm_substate_type              emm_substate;
  emm_connection_state_type      emm_conn_state;
  lte_nas_emm_ul_nas_transport  *ul_nas_transport_ptr = NULL;

  ASSERT(cnm_msg_ptr != NULL); 
  ASSERT(emm_ctrl_data_ptr != NULL);

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending UL NAS Transport msg carrying SMS");

  /* Init */
  emm_state = EMM_GET_STATE();
  emm_substate = EMM_GET_SUBSTATE();
  emm_conn_state = EMM_GET_CONNECTION_STATE();

  memset((void *)emm_ctrl_data_ptr->out_msg_ptr,
         0x0,
         sizeof(lte_nas_outgoing_msg_type));

  ul_nas_transport_ptr = &emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.ul_nas_transport;

  /* Compose uplink nas transport message */
  ul_nas_transport_ptr->lte_nas_hdr.pd = EPS_MOBILITY_MANAGEMENT_MESSAGES;
  ul_nas_transport_ptr->lte_nas_hdr.msg_id = UPLINK_NAS_TRANSPORT;
  ul_nas_transport_ptr->security_hdr = PLAIN_NAS_MSG;

  GET_IMH_LEN(ul_nas_transport_ptr->nas_msg_container.msg_len, 
              &cnm_msg_ptr->cmd.hdr);

  if(ul_nas_transport_ptr->nas_msg_container.msg_len <= MAX_OCTETS_L3_MESSAGE)
  {
    memscpy((void *)&ul_nas_transport_ptr->nas_msg_container.msg_payload[0],
             sizeof(ul_nas_transport_ptr->nas_msg_container.msg_payload),
             (void *)&cnm_msg_ptr->cmd.mmcnm_data_req.data[0],
             ul_nas_transport_ptr->nas_msg_container.msg_len);
  }
  else
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= SMS is too long to be sent in UL NAS Transport msg - %d(byte)",
                   ul_nas_transport_ptr->nas_msg_container.msg_len);
    return;
  }

  switch(emm_state)
  {  
    case EMM_REGISTERED:
    case EMM_TRACKING_AREA_UPDATING_INITIATED:
      if((emm_conn_state == EMM_CONNECTED_STATE) &&
         ((emm_state == EMM_REGISTERED && emm_substate == (emm_substate_type)EMM_REGISTERED_NORMAL_SERVICE ) ||
          (emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED))&&         
         (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED) &&
         (emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                    SYS_SRV_STATUS_SRV) &&
         (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                      SYS_SIM_STATE_AVAILABLE)
#ifdef FEATURE_LTE_REL11
          &&
         ((emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL11) 
          ||
          ((mm_timer_status[TIMER_T3346] == TIMER_STOPPED) ||
          (emm_check_high_priority(emm_ctrl_data_ptr)) ||
          (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE)))
#endif
        && (mm_timer_status[TIMER_T3440] == TIMER_STOPPED))
      {        
        (void)emm_send_message( emm_ctrl_data_ptr->out_msg_ptr, 
                                emm_get_nas_transaction_id(), 
                                MSGR_NAS_EMM,
                                emm_sms_data_req_suceeded,
                                emm_sms_data_req_failed,
                                emm_ctrl_data_ptr);
      }
      else
      {
        MSG_HIGH_DS_4(MM_SUB, "=EMM= Discarding UL SMS in REGISTERED state - %u, %u, %u, %u",
                 emm_state,
                 emm_conn_state, 
                 emm_substate, 
                 emm_ctrl_data_ptr->emm_update_status);
      }
      break ;
    default:
      MSG_ERROR_DS_1(MM_SUB, "=EMM= Discarding UL SMS in EMM state - %u", emm_state);
      break;
  }
} /* emm_process_ul_sms */
#endif /*FEATURE_LTE*/
