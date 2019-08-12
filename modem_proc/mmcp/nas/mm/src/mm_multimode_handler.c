/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001,2002,2003,2010-2011 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mm_multimode_handler.c#1 $ 
$DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------   
05/18/15   MNK   Fix for CR 331514 (1xcsfb: NAS needs to distnguish RLF vs OOS failure when indicating the failure to CM) 
07/18/14   MNK   Fix for CR 331514 (1xcsfb: NAS needs to distnguish RLF vs OOS failure when indicating the failure to CM) 
11/15/11   rajesh  Added code to move EMM state to registered no cell available in L to GW interrat 
                   hanlder if the present state is service request initiated.
11/10/11   rajesh  Added code to reject pending calls when interrat from GW to LTE takes place.
10/01/11   abhi    Added function to map RAI from GUTI
09/30/11   rajesh  send MMR_REG_CNF to REG always on interrat from GW to L
09/16/11   rajesh  Added code to handle stop mode request during interrat from GW to L 
                    and GMM state is deregistered or NULL.
08/22/11   MNK     CR: 301219 Handle CSFB call if TAU is rejected/failed  
07/25/11   Abhi     Added function to return CSFB state
07/21/11   rajesh   Made changes for memory alloction for mm_csfb_service_req_message_ptr.
07/13/11   rajesh   Setting mm_previous_idle_substate to NULL whenever lu attempt counter
                    is cleared.

07/13/11   rajesh  Handling expiry of T3412 and T3423 when ISR is ON
06/30/11   rajesh   Changes made for implementing PS detach timer in NAS
07/07/11 abhi      Added Handling of stop mode request on inter-rat from GSM/UMTS to LTE
06/22/11 rajeshwa  Removed code where MM timers are getting stopped after
                              interrat from GSM or WCDMA to LTE
04/07/2011 MNK     Add 1xCSFB abort handling 
04/01/2011 MNK     Enhance error handling for CSFB 
11/29/10   MNK     More 1XCSFB changes including adversarial scenarios
09/17/10   MNK     CS_NOTIFICATION for 3GPPCSFB added
08/09/10   MNK     Make code compatible for 1xCSFB & 3GPPCSFB
08/30/10   MNK     CSFB changes
08/13/10   MNK     Reset reg_req_pended_indication upon deactivate_cnf 
07/08/10   zren    Updated to set 30 and 31 bits to 1 in M-TMSI/P-TMSI for       
                   TAU/RAU after IRAT 
06/29/10   zren    Updated EMM to handle service request procedure and unsent 
                   messages correctly for redirection and reselection 
06/24/10   abhi    Reverting mapping of mapped PTMSI to native PTMSI
06/10/10   zren    Fixed mapping errors of mobile identity from LTE to UMTS
06/10/10   RI      Fixed the byte swap error in mapping of mobile identity from
                   UMTS to LTE
05/21/10   zren    Updated support of handling REG CMD coming during L2GW IRAT
05/21/10   zren    Updated to abort pending service request procedure before 
                   L2GW IRAT
05/18/10   zren    Updated fun param to set only_for_reg flag in REG CNF and 
                   REG IND
05/04/10   zren    Added support for MME routing info setup in RRC CONN EST REQ
                   for different TIN types
04/20/10   MNK     Set EMM state to EMM_REGISTERED_NO_CELL_AVAILABLE only if EMM is REGISTRED for L2GW transition
04/19/10   MNK     Changes to EMM state setting - ISR
04/14/10   zren    Consolidated GMM/MM handling code for combined procedures to
                   mm_multimode_hanlder
04/09/10   zren    Added combined procedure feature
03/22/10   MNK     map_umts_identity_to_lte completed
03/16/10   MNK     More i-RAT changes 
03/05/10   MNK     TIN related changes
03/01/10   MNK     Add a function to store MMR_REG_REQ for MM and EMM
02/22/10   MNK     Setup EMM to correct state to start TAU upon inter system change from GW
02/21/10   MNK     More updates for -iRAT
02/17/10   MNK     Add code to set the EMM state upon attach & RAU in UMTS for multimode UE
02/16/10   MNK     Add code to convert GUTI to UMTS identities
02/12/10   zren    Updated to always abort the existing connection before 
                   sending RRC DEACT REQ
02/12/10   MNK     First cut of i-RAT between LTE and GW
09/06/11   rajesh   Reset "mm_is_mapped_keys_valid" when UE acquires service in LTE.
===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rex.h"
#include "ms.h"
#include "mm_v.h"
#include "err.h"
#include "gmm_int_v.h"
#include "mm_coord.h"
#include "msgr_nas.h"
#include "emm_utility.h"
#include "sys_eplmn_list_v.h"
#ifdef FEATURE_WRLF_SYSTEM_SEL
#include "rrccmd.h"
#endif
#ifdef FEATURE_LTE
#include "amssassert.h"
#include "mm_multimode_common.h"
#include "emm_connection_type.h"
#include "emm_database.h"
#include "emm.h"
#include "emm_rrc_if.h"
#include "emm_rrc_emm_if.h"
#include "emm_esm_if_msg.h"
#include "esm_emm_msg_if.h"
#include "esm_emm_msgr_enum_msg.h"
#include "gmmllcm.h"
#include "emm_service_request_lib.h"
#include "emm_sim_handler.h"
#include "mm_as_types.h"
#include "emm_csfb_handler.h"
#include "mmrat.h"
#include "sm_v.h"
#include "mmsecurity.h"
#include "gmmsecurity.h"
#include "emm_cnm_handler.h"
#include <stringl/stringl.h>
#include "emm_mem.h"
#include "emm_rrc_handler.h"
#include "policyman_sglte.h"
#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE))
 #error code not present
#endif // TEST_FRAMEWORK

#ifdef FEATURE_DUAL_SIM
#include "subs_prio.h"
#endif


/* Used to buffer REG CMD received during GW2L and L2GW IRAT */
mm_cmd_type     mm_irat_pending_reg_cmd;
boolean         mm_irat_reg_cmd_pending;
extern void mm_copy_lau_reject_list_to_forbidden_list(void);
#ifdef FEATURE_DUAL_SIM
sys_radio_access_tech_e_type mm_service_loss_on_multimode_rat_sim[MAX_AS_IDS] = {SYS_RAT_NONE,SYS_RAT_NONE};
#define mm_service_loss_on_multimode_rat  mm_service_loss_on_multimode_rat_sim[mm_sub_id]
#else
sys_radio_access_tech_e_type     mm_service_loss_on_multimode_rat = SYS_RAT_NONE;
#endif

#ifdef FEATURE_DUAL_SIM
extern boolean gmm_lte_deact_isr_timer_expired_sim[MAX_AS_IDS];
#define gmm_lte_deact_isr_timer_expired gmm_lte_deact_isr_timer_expired_sim[mm_sub_id]
#else
extern boolean gmm_lte_deact_isr_timer_expired;
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern sys_drx_cn_coefficient_s1_e_type gmm_drx_cn_coeff_s1_sim[MAX_AS_IDS];
extern boolean mm_mn_sglte_svlte_mode;
#define gmm_drx_cn_coeff_s1 gmm_drx_cn_coeff_s1_sim[mm_sub_id]
#ifdef FEATURE_TDSCDMA
#define sys_eplmn_per_nas_region_update(a) sys_eplmn_per_nas_region_update_per_stack(a,(sys_modem_as_id_e_type)mm_as_id)
#endif
#else
extern sys_drx_cn_coefficient_s1_e_type gmm_drx_cn_coeff_s1;
#endif

#if defined FEATURE_SGLTE || defined FEATURE_1XSRLTE || defined FEATURE_SVLTE_DUAL_SIM
extern sys_ue_mode_e_type mm_current_ue_mode ;
#endif

#ifdef FEATURE_SGLTE
extern void mm_copy_cs_database(mm_as_id_e_type dest,mm_as_id_e_type src);
extern void mmsend_mmr_service_stat_ind( mm_as_id_e_type mm_as_id_in);
extern void mmsend_mmr_stack_deact_ind(void);
extern void mmsend_mmr_stack_act_ind(void);
extern void  mmcoord_process_merging(void );
extern void mmcoord_process_spliting(void );
extern boolean mm_is_split_needed(void);
extern mm_sglte_stack_activity_status mm_sglte_current_stack_status;
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

extern sys_network_selection_mode_e_type mm_waiting_for_list_cnf_type_sim[MAX_NAS_STACKS];
#define  mm_waiting_for_list_cnf_type mm_waiting_for_list_cnf_type_sim[mm_as_id]

extern boolean pending_mm_msg_valid_sim[MAX_NAS_STACKS];
#define  pending_mm_msg_valid (pending_mm_msg_valid_sim[mm_as_id])

extern boolean pending_gmm_msg_valid_sim[MAX_NAS_STACKS];
#define  pending_gmm_msg_valid (pending_gmm_msg_valid_sim[mm_as_id])

extern sys_radio_access_tech_e_type mm_service_loss_on_rat_sim[MAX_NAS_STACKS];
#define  mm_service_loss_on_rat mm_service_loss_on_rat_sim[mm_as_id]

boolean mm_rau_pending_in_conn_mode_sim[MAX_NAS_STACKS]={FALSE};
#define mm_rau_pending_in_conn_mode mm_rau_pending_in_conn_mode_sim[mm_as_id]
#else
extern sys_network_selection_mode_e_type mm_waiting_for_list_cnf_type;
extern boolean pending_mm_msg_valid;
extern boolean pending_gmm_msg_valid;

extern sys_radio_access_tech_e_type mm_service_loss_on_rat;
boolean mm_rau_pending_in_conn_mode = FALSE;
#endif


#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#define sys_eplmn_list_get(a,b) sys_eplmn_per_nas_stacks_list_get(a,b,(sys_modem_as_id_e_type)mm_as_id)
#endif

#if defined (TEST_FRAMEWORK) || defined (TF_UNIT_TEST)
#error code not present
#endif

mm_gw_identity_param_type mm_mapped_gw_identity;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
mm_csfb_state_type mm_csfb_state_sim[MAX_NAS_STACKS];
#define mm_csfb_state mm_csfb_state_sim[mm_as_id]
#else
mm_csfb_state_type mm_csfb_state;
#endif

#ifdef FEATURE_MODEM_HEAP
#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
mm_cmd_type *mm_csfb_service_req_message_ptr_sim[MAX_NAS_STACKS] = {NULL,NULL,NULL};
#define mm_csfb_service_req_message_ptr mm_csfb_service_req_message_ptr_sim[mm_as_id]
#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
mm_cmd_type *mm_csfb_service_req_message_ptr_sim[MAX_NAS_STACKS] = {NULL,NULL};
#define mm_csfb_service_req_message_ptr mm_csfb_service_req_message_ptr_sim[mm_as_id]
#else
mm_cmd_type *mm_csfb_service_req_message_ptr = NULL;
#endif

#else
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
mm_cmd_type mm_csfb_service_req_message_sim[MAX_NAS_STACKS];
#define mm_csfb_service_req_message mm_csfb_service_req_message_sim[mm_as_id]
#else
mm_cmd_type mm_csfb_service_req_message;
#endif

#endif
mm_csfb_mt_call_info_s_type    mm_csfb_mt_call_info;

/*===========================================================================

                STATIC DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#ifdef FEATURE_TDSCDMA
static void mm_csfb_regional_handling(boolean state);
#endif
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
extern void gmm_handle_gprs_detach_low_level_failure( void );

extern void emm_process_timer_expiry(mm_cmd_type *cmd_ptr, emm_ctrl_data_type *emm_ctrl_data_ptr);
extern boolean emm_get_ul_count(byte *ul_count);
#ifdef FEATURE_WRLF_SYSTEM_SEL
#ifdef FEATURE_MODEM_HEAP
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
extern serving_plmn_info_T *mm_wrlf_serving_plmn_ptr_sim[MAX_NAS_STACKS];
#define mm_wrlf_serving_plmn_ptr mm_wrlf_serving_plmn_ptr_sim[mm_as_id]
#else
extern serving_plmn_info_T *mm_wrlf_serving_plmn_ptr;
#endif
#else
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
extern serving_plmn_info_T mm_wrlf_serving_plmn_sim[MAX_NAS_STACKS];
#define mm_wrlf_serving_plmn mm_wrlf_serving_plmn_sim[mm_as_id]
#else
extern serving_plmn_info_T mm_wrlf_serving_plmn;
#endif
#endif
#endif

extern boolean full_srv_req_in_emerg;

boolean mm_get_deep_sleep_alt_scan_status(void)
{
  return mm_deep_sleep_alt_scan;
}

void mm_set_deep_sleep_alt_scan_status(boolean set_status)
{
  mm_deep_sleep_alt_scan = set_status;
  return;
}

boolean mm_emm_get_vowifi_call_status(emm_ctrl_data_type *emm_ctrl_data_ptr)
{
  if(((emm_ctrl_data_ptr->mmtel_call_status.video_call_status == TRUE)||
       (emm_ctrl_data_ptr->mmtel_call_status.voice_call_status == TRUE)) && 
     (emm_ctrl_data_ptr->mmtel_call_status.sys_mode == SYS_SYS_MODE_WLAN))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

void emm_resume_paused_mm_timers(void)
{
  byte i;
  MSG_HIGH_DS_0(MM_SUB,"=MM= DEEP_SLEEP ALT SCAN. Resume all paused timers"); 
  for(i=0;i<MAX_MM_TIMER+1;i++)
  {
    if (mm_timer_status[i] == TIMER_PAUSED)
    {
      if (gs_modify_timer_ext(
                               get_mm_timer_queue(),
                               i,
                               TIMER_MODIFY_RESUME,
                               mm_timer_non_deferrable(i)) != GS_SUCCESS)
      {
        MSG_ERROR_DS(MM_SUB, "Failed to resume timer %d", i, 0, 0);
      }
      else
      {
        MSG_HIGH_DS_1(MM_SUB ,"=MM= Resuming %d", i);
        mm_timer_status[ i ] = TIMER_ACTIVE;
      }  
    }    
  }
  
}


/*===========================================================================
 
FUNCTION      mm_set_gmm_param_lte_to_gw_handover

DESCRIPTION  
 Called when Interrat handover from LTE TO GW happens.
 This function sets the connected mode parameters of GW,
 copies the mapped identities into global varialble "mm_mapped_gw_identity"
 and also udpates the GW authentication parameters in the SIM/USIM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_set_gmm_param_lte_to_gw_handover(void)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
  
 #ifdef FEATURE_SGLTE
     if(MM_IS_SGLTE_ON_PS_STACK_ID)
#endif
  {
  MSG_HIGH_DS(MM_SUB,"Setting GW parameters for intersystem handover from LTE to GW",0,0,0 );
  rrc_connection_status = MMRRC_ACTIVE;
  ps_session_status = MMRRC_ACTIVE;
  gmm_ps_signalling_connected = TRUE;

  /* if TIN is GUTI get the mapped identities from LTE */
  if(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI && 
     emm_ctrl_data_ptr->emm_guti_valid == TRUE)
  {
    emm_get_gw_identity_from_lte(&mm_mapped_gw_identity, emm_ctrl_data_ptr);
  }
  /* Get the security keys */
  emm_get_eps_handover_auth_params(&mm_mapped_gw_identity);
  /* update the mm nas uplink count here */
  emm_get_ul_count(mm_mapped_gw_identity.ul_count_at_generation);
  /* Set the P-TMSI signature to zero for now. Need to be revisited*/
  memset((void *)&mm_mapped_gw_identity.ptmsi_signature.value[1], 0, sizeof(word));

  mm_update_gw_auth_keys(mm_mapped_gw_identity, emm_ctrl_data_ptr,RRC_SIM_UPDATE_VALUE_VALID);
  }
}

/*===========================================================================
 
FUNCTION      mm_set_mm_param_lte_to_gw_srvcc

DESCRIPTION  
 Called when LTE to UMTS SRVCC handover happens 
 This function sets the MM state and connection. Also write derived CS keys to USIM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_set_mm_param_lte_to_gw_srvcc
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  byte eKSI;
  auth_gprs_cipherkey_type    srvcc_Ck;  
  auth_gprs_integritykey_type srvcc_Ik;
  auth_gsm_cipherkey_type     gsm_cipherkey;
  auth_gsm_cipherkey_type     gsm_kc_128bit;

  rrc_connection_status = MMRRC_ACTIVE;
  cs_session_status = MMRRC_ACTIVE;
  mm_cnm_is_active = TRUE;
  mm_is_cs_connected_due_to_srvcc   = TRUE;
  
  if(emm_ctrl_data_ptr->eps_bearer_context_status.emc_voice_bearer_exist == TRUE)
  {
    MSG_HIGH_DS(MM_SUB,"=MM= SRVCC w/ emergency voice bearer existing",0,0,0);
    mm_connection_type = EMERGENCY_MM_CONNECTION;
  }
  else
  {
    mm_connection_type = NORMAL_MM_CONNECTION;
  }

  mm_state_control(MM_STD_SRVCC_HANDOVER);

  if(TRUE == emm_get_srvcc_handover_auth_params(&eKSI, &srvcc_Ck, &srvcc_Ik))
  {
  auth_generate_kc(&srvcc_Ck, &srvcc_Ik, &gsm_cipherkey,&gsm_kc_128bit);

  auth_update_security_keys(RRC_CS_DOMAIN_CN_ID,
                                                  eKSI,
                                                  0xFF,
                                                  &srvcc_Ck,
                                                  &gsm_cipherkey, 
                                                  &srvcc_Ik,
												  &gsm_kc_128bit);

  (void)auth_save_security_context(eKSI,
                                                   RRC_CS_DOMAIN_CN_ID,
                                                   &srvcc_Ck,
                                                   &srvcc_Ik,
                                                   &gsm_cipherkey);

    if(mm_serving_plmn.info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS)
    {
   auth_notify_lower_layer(&srvcc_Ck,
                                              &srvcc_Ik,
                                              &gsm_cipherkey,
                                              RRC_CS_DOMAIN_CN_ID,
                                              RRC_SIM_UPDATE_VALUE_VALID);
    }  
    else if(mm_serving_plmn.info.active_rat == SYS_RAT_GSM_RADIO_ACCESS)
    {
      auth_notify_lower_layer(&srvcc_Ck,
                                        &srvcc_Ik,
                                        &gsm_cipherkey,
                                        RRC_CS_DOMAIN_CN_ID,
                                        RRC_SIM_UPDATE_VALUE_CHANGED);
   }
  }
   /* Reset the send sequence number for the next run */
   send_sequence_number_mm_cc_ss = 0; /* Spec --> 24.007, 11.2.3.2.3.1.1 Send state variable V(SD) */

}

#ifdef FEATURE_SGLTE
/*===========================================================================
 
FUNCTION      mm_emm_process_srvcc_sglte_cs_handover

DESCRIPTION  
 Called when LTE to UMTS SRVCC handover happens 
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_emm_process_srvcc_sglte_cs_handover(mm_cmd_type *cmd_ptr)
{
  mm_as_id_e_type current_mm_as_id;

  /* Suspend DS, as part of TAU it will be resumed */
  if ((gmm_state != GMM_NULL) && (gmm_state != GMM_DEREGISTERED))
  {
    mm_send_cm_ps_data_suspend_ind();
  }

  MSG_HIGH_DS(MM_SUB,"=MM= RR_ACTIVATION_IND cause MM_AS_LTOG_SGLTE_SRVCC_CS_HANDOVER",0,0,0);

  /* set TAU cause to others to start TAU on resuming LTE */
  MSG_HIGH_DS(MM_SUB,"=MM= TAU to be performed on LTE resume ",0,0,0);
  emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;

  /* start the timer to delay TAU */
  mm_start_timer(TIMER_DELAY_TAU, 1000);

  emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE, 
                           LTE_RRC_CONN_ABORT_CAUSE_IRAT_SUCCESS_SGLTE_SRVCC_TO_G);
  
  /* set the current stack as CS stack */  
  current_mm_as_id = mm_as_id;
  mm_as_id = mm_cs_stack_id;

  /* in case of est_req collision with act_ind
     move from 13 to 19 */
  mm_state_control( MM_STD_RADIO_UNAVAILABLE );

  mm_set_mm_param_lte_to_gw_srvcc(emm_ctrl_data_ptr);
  cs_session_status = MMRRC_IDLE;

  /* send SRVCC HO complete ind with trigger = 1 */
  mm_send_mmcnm_srvcc_handover_complete(TRUE);

  /* send the act_rsp to GSM. send as NONE,
     so that we don't change interat_handler */
  mm_send_rr_activation_rsp(SYS_RAT_NONE);

  /* send SRVCC HO complete ind */
  mm_send_mmcnm_srvcc_handover_complete(FALSE);

  /* restore the original stack id */ 
  mm_as_id = current_mm_as_id;

}
#endif 

/*===========================================================================

FUNCTION   MM_EMM_PROCESS_LTE_TO_GW_RAT_CHANGE

DESCRIPTION
  This function processes LTE to GW RAT change

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_emm_process_LTE_to_GW_rat_change
( 
  mm_cmd_type *cmd_ptr,
  mm_emm_dispatcher_fp **mm_emm_dispatcher_fn_ptr,
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  mm_as_status_T local_status = MM_AS_ABORTED;
  emm_failure_type          emm_failure_cause;
  mmr_stop_mode_req_s_type  msg;

  ASSERT(cmd_ptr != (void *)NULL);

  ASSERT(emm_ctrl_data_ptr != NULL);

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

#ifdef FEATURE_TDSCDMA
  mm_csfb_regional_handling(FALSE);
#endif

  /*Ignore RRC_ACTIVATION_IND and abort LTE RRC connection when the following conditions are satisfied
    1] EMM state is EMM-DEREGISTERED, EMM-DEREGISTERD-INITATED or EMM_REGISTERED_INITIATED
    2] ACTIVATION_IND status is MM_AS_INTERRAT_PS_HANDOVER or MM_AS_LTOW_SRVCC_CS_PS_HANDOVER
    This will cause LTE RRC to send HO Abort Request to WRRC */

  if((cmd_ptr->cmd.hdr.message_id == RRC_ACTIVATION_IND)
     &&
     ((EMM_GET_STATE() == EMM_DEREGISTERED) || 
      (EMM_GET_STATE() == EMM_DEREGISTERED_INITIATED) ||
      (EMM_GET_STATE() == EMM_REGISTERED_INITIATED))
     && 
     ((cmd_ptr->cmd.rrc_activation_ind.status == MM_AS_INTERRAT_PS_HANDOVER) ||
      (cmd_ptr->cmd.rrc_activation_ind.status == MM_AS_LTOW_SRVCC_CS_PS_HANDOVER)))
  {
     emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE, LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
     return;
  }

 #ifdef FEATURE_SGLTE
  /* SRVCC over SGLTE, as LTE is suspended only temporarily,
     LTE clean should not be done */
  if((cmd_ptr->cmd.hdr.message_id == RR_ACTIVATION_IND) &&
     (cmd_ptr->cmd.rr_activation_ind.status == MM_AS_LTOG_SGLTE_SRVCC_CS_HANDOVER))
  {
    if ((MM_IS_IN_SGLTE_MODE) &&
        (mm_cs_stack_id != mm_ps_stack_id))
    {
      mm_emm_process_srvcc_sglte_cs_handover(cmd_ptr);
    }
    else
    {
      ERR_FATAL("=MM= ACT_IND w/ SGLTE_SRVCC received in non SGLTE mode",0,0,0);
    }

    *mm_emm_dispatcher_fn_ptr = mm_emm_normal_msg_handler; 
    MSG_HIGH_DS(MM_SUB,"=EMM= Change the main routine handler to mm_emm_normal_msg_handler", 0,0,0 ); 
    return;
  }
#endif

#ifdef FEATURE_SGLTE
     if(MM_IS_SGLTE_ON_PS_STACK_ID)
#endif
  {
   if((EMM_GET_CONNECTION_STATE() != EMM_CONNECTED_STATE) &&
      (cmd_ptr->cmd.rr_activation_ind.status == MM_AS_LTOG_CCO))
   {
     MSG_HIGH_DS(MM_SUB,"=EMM= L2G CCO indication is received in emm connection state %d ignoring it", 
                    EMM_GET_CONNECTION_STATE(),0,0 );
     return;
   }
 }
  if((cmd_ptr->cmd.hdr.message_id == RR_ACTIVATION_IND) &&
     (cmd_ptr->cmd.rr_activation_ind.status == MM_AS_LTOG_CCO) &&
     (EMM_GET_STATE() == EMM_DEREGISTERED_INITIATED))
  {
    if(emm_ctrl_data_ptr->detach_reason == SWITCH_OFF)
    {
      MSG_HIGH_DS(MM_SUB,"=EMM= NAS performing power off detach ignoring CCO indication", 0,0,0 );
      return;
    }
    else if(emm_ctrl_data_ptr->ps_detach_info.status == 
                                       EMM_PS_DETACH_CNF_PENDING)
    {
      emm_ctrl_data_ptr->l2gcco_state = EMM_LTOG_CCO_IN_PROGRESS;
      emm_ps_detach_clean_up(emm_ctrl_data_ptr);
    }
  }

  /* Abort pending service request procedure and notify DS */
  if(EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED)
  {
    emm_failure_cause.cause_type = LTE_NAS_IRAT_INVALID_STATE;
    if(EMM_GET_CONNECTION_STATE() == EMM_RELEASING_RRC_CONNECTION_STATE)
    {
      /* Service request was started in redirection. SR message and possible esm 
         messages are still in buffer. Clean the messages and abort the service
         request procedure. */
      emm_process_unsent_messages(CONN_CNF_LTE_RRC_CONN_EST_FAILURE, 
                                  emm_failure_cause, 
                                  emm_ctrl_data_ptr);
      /* Process all pending irat uplink messages */
      emm_process_pending_irat_msg(LTE_NAS_IRAT_INVALID_STATE,
                                   LTE_NAS_IRAT_FAILURE_CAUSE_NONE,
                                   LTE_NAS_IRAT_ACCESS_BARRING_NONE,
                                   emm_ctrl_data_ptr);
    }
    else if(EMM_GET_CONNECTION_STATE() == EMM_WAITING_FOR_RRC_CONFIRMATION_STATE)
    {
      /* Service request was started in reselection. Abort the service request 
         procedure and connection. Cached message will be cleared upon receiving
         conn cnf with aborted cause. */
      if( emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB )
      {
        emm_abort_service_request(emm_ctrl_data_ptr, TRUE, TRUE,
                                  LTE_NAS_LRRC_CONN_EST_FAILURE_CELL_RESEL,
                                  emm_failure_cause);
      }
      else
      {
        emm_abort_service_request(emm_ctrl_data_ptr, TRUE, TRUE,
                                  LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE,
                                  emm_failure_cause);
      }
    }
  }

  /* At this time if ESR is in progress then connection release must have happened with redirection and as a response to ESR*/
  /* THis code should be retained after the above code which takes care of aborting ESR if UE is in GW due to reselection*/
  switch(emm_ctrl_data_ptr->esr_type_in_progress)
  {
    case NAS_EMM_3GPP_CSFB:
      emm_process_esr_complete(emm_ctrl_data_ptr);  
      break;
    default:
      break;
  }

  /* Set EMM state*/
  /* Do we need to do this. Probably should not. deactivate_cnf processing will 
     do this for us, if we decide to use it.
     But, at this time looks like we can't use it. 
     So for now set the state here */
  switch(EMM_GET_STATE())
  {
    case EMM_REGISTERED:
      emm_ctrl_data_ptr->emm_tau_attempt_counter = 0;
      EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
      break;
    case EMM_REGISTERED_INITIATED:
      emm_ctrl_data_ptr->plmn_service_state->service_status = 
                                                   SYS_SRV_STATUS_LIMITED;
      /* Abort ongoing attach procedure */
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
      emm_remove_release_client(emm_attach_release_indication);
      mm_stop_timer(TIMER_T3410);
      break;
    case EMM_DEREGISTERED:
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
      emm_rat_change_stop_timers();
      break;
    case EMM_TRACKING_AREA_UPDATING_INITIATED:
      mm_stop_timer(TIMER_T3430);
      emm_ctrl_data_ptr->emm_tau_attempt_counter = 0;
      emm_remove_release_client(emm_tau_request_release_indication);
      EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
      break;
    /* During MO CSFB if user ends the call before UE is redirected to GW, then CSFB 
    states and ESR timer will be reset in emm_abort_and_clean_csfb_call. But No 
    change is made to the EMM state. Added this code to set the EMM state correctly
    if UE is redirected to GW later on.
    This covers the scenario where EMM state is service request initiated and 
    UE is redirected to GW.
    */
    case EMM_SERVICE_REQUEST_INITIATED:
      EMM_MOVES_TO_REGISTERED_STATE(EMM_REGISTERED_NO_CELL_AVAILABLE);
      break;
    case EMM_DEREGISTERED_INITIATED:
      if(emm_ctrl_data_ptr->detach_reason == SWITCH_OFF)
      {
        /* pend stop_mode again to process post IRAT */
        mm_nreg_req_received = TRUE;  
    
		msg.message_header.message_set     = MS_MM_REG;
        msg.message_header.message_id      = (byte)MMR_STOP_MODE_REQ;
        msg.message_header.message_len_lsb = (sizeof( mmr_stop_mode_req_s_type ) - sizeof(IMH_T)) % 256;
        msg.message_header.message_len_msb = (sizeof( mmr_stop_mode_req_s_type ) - sizeof(IMH_T)) / 256;

        msg.stop_mode_reason = emm_ctrl_data_ptr->rrc_deact_req.deact_reason;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
        msg.as_id = (sys_modem_as_id_e_type)mm_as_id;
#endif
		memscpy( &mmcoord_pending_reg_message.cmd.mmr_stop_mode_req,sizeof(mmr_stop_mode_req_s_type),
		 		&msg, sizeof(mmr_stop_mode_req_s_type));
		         
        /* Remove connection release CB function */
        emm_remove_release_client(emm_poweroff_detach_release_ind);
        mm_stop_timer(EMM_POWEROFF_DETACH_TIMER);        
      }
      else
      {
        emm_abort_mo_detach(emm_ctrl_data_ptr, FALSE);
      }
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
	  emm_reset_poweroff_detach_params(emm_ctrl_data_ptr);
      break;
    default:
      break;
  }
 
  if( cmd_ptr->cmd.hdr.message_id == RRC_ACTIVATION_IND)
  {
#ifdef FEATURE_TDSCDMA
    if( cmd_ptr->cmd.hdr.message_set == MS_MM_TDSRRC
#ifdef FEATURE_SEGMENT_LOADING
      && get_tdscdma_interface()
#endif
)
    {

      /* Set the active RAT to TDS*/
      if(!mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                      SYS_RAT_TDS_RADIO_ACCESS))
      {
         MSG_FATAL_DS(MM_SUB,"=EMM= Interrat to TDS when rat priority list does not have TDS rat ",0,0,0) ;
      }
      mm_coord_camped_cell_id = 0xFFFF; 
      mm_serving_plmn.info.active_rat = SYS_RAT_TDS_RADIO_ACCESS;
      local_status = cmd_ptr->cmd.rrc_activation_ind.status;
      MSG_HIGH_DS(MM_SUB,"=EMM= LTE to TDS i-RAT in progress(cause %d), active RAT is SYS_RAT_TDS_RADIO_ACCESS", local_status,0,0 );
    }
   else if (TRUE
#ifdef FEATURE_SEGMENT_LOADING
      && get_wcdma_interface()
#endif
)
#endif
    {
      /* Set the active RAT to UMTS*/
      if(!mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                        SYS_RAT_UMTS_RADIO_ACCESS))
  	  {
          MSG_FATAL_DS(MM_SUB,"=EMM= Interrat to UMTS when rat priority list does not have UMTS rat ",0,0,0) ;
  	  }
      mm_serving_plmn.info.active_rat = SYS_RAT_UMTS_RADIO_ACCESS;
      mm_coord_camped_cell_id = 0xFFFF;
      local_status = cmd_ptr->cmd.rrc_activation_ind.status;
      MSG_HIGH_DS(MM_SUB,"=EMM= LTE to WCDMA i-RAT in progress(cause %d), active RAT is SYS_RAT_UMTS_RADIO_ACCESS", local_status,0,0 );
    }
    /* reset the blind redirection flag here*/
    emm_ctrl_data_ptr->rr_blind_redir_pending_call_abort = FALSE;
  }
  else if (cmd_ptr->cmd.hdr.message_id == RR_ACTIVATION_IND)
  {
    if(!mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                      SYS_RAT_GSM_RADIO_ACCESS))
    {
       MSG_FATAL_DS(MM_SUB,"=EMM= Interrat to GSM when rat priority list does not have GSM rat ",0,0,0) ;
    }
    /* Set the active RAT to GSM*/
    mm_serving_plmn.info.active_rat = SYS_RAT_GSM_RADIO_ACCESS;
    mm_coord_camped_cell_id = 0xFFFF; 
    local_status = cmd_ptr->cmd.rr_activation_ind.status;
    MSG_HIGH_DS(MM_SUB,"=EMM= LTE to GSM i-RAT in progress(cause %d), active RAT is  SYS_RAT_GSM_RADIO_ACCESS", local_status,0,0 );
  }
  else
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= We shouldn't be in this function if LTE to GW RAT transition is happening",0,0,0) ;
  }

  /* Looks like we need to inform REG about change is service by setting only_to_reg to TRUE*/
  //mm_send_mmr_service_ind

  switch(local_status)
  {
    case MM_AS_INTERRAT_REDIRECTION:
    case MM_AS_INTERRAT_RESELECTION:
#ifdef  FEATURE_SGLTE    
    case MM_AS_SGLTE_INTER_RAT_RESELECTION:
    case MM_AS_SGLTE_INTER_RAT_REDIRECTION:
    
     if(MM_IS_IN_SGLTE_MODE &&
        ((local_status==MM_AS_SGLTE_INTER_RAT_RESELECTION)||
        (local_status==MM_AS_SGLTE_INTER_RAT_REDIRECTION)))
      {
        mm_sglte_current_stack_status  = SGLTE_MM_LOWER_LAYER_MERGING;
        mmsend_mmr_stack_deact_ind();
      }
 #endif   
      /* Suspend DS first*/
      if ((gmm_state != GMM_NULL) && (gmm_state != GMM_DEREGISTERED))
      {
        mm_send_cm_ps_data_suspend_ind();
        mm_serving_plmn.info.ps_data_suspend = TRUE;
      }
      /*Send service cnf/ind only to REG */
      mm_service_ind_blocked_due_to_irat_cs_reg = TRUE;
      reg_service_report(MMR_CAUSE_NOT_SPECIFIED, TRUE, emm_ctrl_data_ptr) ;

      emm_init_rrc_deact_req(SYS_STOP_MODE_REASON_IRAT, emm_ctrl_data_ptr);
      break;

    case MM_AS_INTERRAT_PS_HANDOVER:
      MSG_HIGH_DS(MM_SUB,"=EMM= received MM_AS_INTERRAT_PS_HANDOVER, sending abort_request to L-RRC",0,0,0);
/*24.008: 4.7.5.1
Set flag to true to initiate RAU in G-rat. The normal routing area updating procedure is initiated: in A/Gb mode, after Inter RAT handover from S1 mode or Iu mode.*/
#ifdef FEATURE_NAS_REL11
      if(is_gsm_mode() && (gmm_state == GMM_REGISTERED || gmm_state == GMM_REGISTERED_INITIATED))
      {
        gmm_is_lw_to_g_interrat_in_progress = TRUE;
      }
#endif
      reg_service_report(MMR_CAUSE_NOT_SPECIFIED, TRUE, emm_ctrl_data_ptr) ;
      emm_init_rrc_deact_req(SYS_STOP_MODE_REASON_IRAT, emm_ctrl_data_ptr);
      if(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI)
      {
        if(sm_check_isr_deact_status_on_interrat())
        {
          MSG_HIGH_DS(MM_SUB,"Deactivate ISR due to PDP context update while in LTE",0,0,0);
          emm_set_and_write_tin_type(NAS_MM_TIN_GUTI);
          mm_pending_lu_request = LTE_IRAT_LU;
        }
      }
      mm_set_gmm_param_lte_to_gw_handover();
      /*marking pending flag for key derivation in case of L2GW PSHO failure */
      mm_rau_pending_in_conn_mode = TRUE;
      break;

    case MM_AS_LTOW_SRVCC_CS_HANDOVER:
    case MM_AS_LTOG_SRVCC_CS_HANDOVER:
      MSG_HIGH_DS(MM_SUB,"=EMM= received SRVCC CS HANDOVER to G/W Act Ind status = %d",local_status,0,0);
      /* Suspend DS first*/
      if ((gmm_state != GMM_NULL) && (gmm_state != GMM_DEREGISTERED))
      {
        mm_send_cm_ps_data_suspend_ind();
        mm_serving_plmn.info.ps_data_suspend = TRUE;
      }
      reg_service_report(MMR_CAUSE_NOT_SPECIFIED, TRUE, emm_ctrl_data_ptr) ;

      emm_init_rrc_deact_req(SYS_STOP_MODE_REASON_IRAT, emm_ctrl_data_ptr);

      mm_set_mm_param_lte_to_gw_srvcc(emm_ctrl_data_ptr);

      if (local_status == MM_AS_LTOG_SRVCC_CS_HANDOVER)
      {
        cs_session_status = MMRRC_IDLE;
      }
      
      /* 24.301 : 5.5.3.3.2 (d) */
      emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_REQ_DUE_TO_SRVCC;
      emm_ctrl_data_ptr->is_srvcc_in_progress = TRUE;

      mm_send_mmcnm_srvcc_handover_complete(TRUE);

      break;

    case MM_AS_LTOW_SRVCC_CS_PS_HANDOVER:
      MSG_HIGH_DS(MM_SUB,"=EMM= received MM_AS_LTOW_SRVCC_CS_PS_HANDOVER",0,0,0);
      reg_service_report(MMR_CAUSE_NOT_SPECIFIED, TRUE, emm_ctrl_data_ptr) ;

      emm_init_rrc_deact_req(SYS_STOP_MODE_REASON_IRAT, emm_ctrl_data_ptr);

      mm_set_mm_param_lte_to_gw_srvcc(emm_ctrl_data_ptr);
      
       /* 24.301 : 5.5.3.3.2 (d) */
      emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_REQ_DUE_TO_SRVCC;

      if(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI)
      {
        if(sm_check_isr_deact_status_on_interrat())
        {
          MSG_HIGH_DS(MM_SUB,"Deactivate ISR due to PDP context update while in LTE",0,0,0);
          emm_ctrl_data_ptr->TIN = NAS_MM_TIN_GUTI;
          mm_pending_lu_request = LTE_IRAT_LU;
        }
      }
      mm_set_gmm_param_lte_to_gw_handover();

      emm_ctrl_data_ptr->is_srvcc_in_progress = TRUE;

	  mm_send_mmcnm_srvcc_handover_complete(TRUE);
      /*marking pending flag for key derivation in case of L2GW SRVCC PS failure */
      mm_rau_pending_in_conn_mode = TRUE;
      break;     

    case MM_AS_LTOG_CCO:
      emm_ctrl_data_ptr->l2gcco_state = EMM_LTOG_CCO_IN_PROGRESS;
      MSG_HIGH_DS(MM_SUB,"=EMM= received MM_AS_LTOG_CCO set L2GCCO state to IN PROGRESS",0,0,0);
#ifdef FEATURE_SGLTE
        if(MM_IS_IN_SGLTE_MODE )
        {
          MSG_HIGH_DS(MM_SUB,"=EMM= SGLTE CCO starting merging..",0,0,0);
          mmsend_mmr_stack_deact_ind();
          mmcoord_process_merging();
          mmsend_mmr_service_stat_ind(mm_ps_stack_id);
        }
#endif
      /* If there are any SMS pending request then reject it to CNM.
         Also need to change this once the release indication with  
         cause enhancement is finished by Pankaj*/
      if(emm_ctrl_data_ptr->emm_sms_active == TRUE)
      {
        /* send release indication to CNM to indicate failure of SMS started in LTE*/
        emm_send_mmcnm_rel_ind(emm_ctrl_data_ptr, PS_SMS_FAIL);

        emm_ctrl_data_ptr->emm_sms_active = FALSE;
      }
      reg_service_report(MMR_CAUSE_NOT_SPECIFIED, TRUE, emm_ctrl_data_ptr) ;
      /* Suspend DS first*/
      if ((gmm_state != GMM_NULL) && (gmm_state != GMM_DEREGISTERED))
      {
        mm_send_cm_ps_data_suspend_ind();
        mm_serving_plmn.info.ps_data_suspend = TRUE;
      }
      mm_send_cm_bearer_ctxt_transfer_ind(mm_serving_plmn.info.active_rat);
#ifdef FEATURE_NAS_FREE_FLOAT
#ifdef FEATURE_GSM_GPRS
      if(gmm_state == GMM_REGISTERED)
      {
        gmmllc_reinit(GMM_STANDBY_STATE) ;
      }
#endif
#endif
      mm_send_rr_activation_rsp(SYS_RAT_LTE_RADIO_ACCESS);
      /* On sending rat resume to SM, SM will send Modify req to SNDCP and SNDCP will
           inturn send LL-ACT-IND to LLC. If user plane is not in suspended state,
           LLC would trigger XID command prior to sending RAU.
           So suspend LLC first and then send rat resume to SM.
      */
#if defined(FEATURE_NAS_FREE_FLOAT) && defined (FEATURE_GSM_GPRS)
      gmm_pended_rat_resume_req = TRUE;
      gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_G_RAT_RESUMED,FALSE,0);
#else
#ifdef FEATURE_GSM_GPRS
      gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_LLC_SUSPENSION, FALSE, 0);
#endif
      gmm_sm_send_rat_resume_req();
#endif

#if defined(FEATURE_LTE) && defined (FEATURE_LTE_REL9)
      gmm_handle_emc_ps_srv(emm_ctrl_data_ptr);
#endif

      /* Set rrc_active to FALSE, as LRRC is not active now. It will be set to TRUE on service indication
         on LTE on CCO failure*/
      emm_ctrl_data_ptr->rrc_active = FALSE;
      /* LTE stack is deactivated. Clean up message buffer. */
      emm_clean_msg_buffer(emm_ctrl_data_ptr);
      
      /* Leaving LTE. Clean up PDN CONN REQ for ATTACH if necessary */
      emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr);


      /* Free the memory allocated dynamically */
      emm_procedure_message_mem_free(emm_ctrl_data_ptr);
      if(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI)
      {
        if(sm_check_isr_deact_status_on_interrat())
        {
          MSG_HIGH_DS(MM_SUB,"Deactivate ISR due to PDP context update while in LTE",0,0,0);
          emm_ctrl_data_ptr->TIN = NAS_MM_TIN_GUTI;
          mm_pending_lu_request = LTE_IRAT_LU;
        }
        else if(mm_csfb_state == MM_CSFB_STATE_NULL)
        {
          emm_set_and_write_tin_type(NAS_MM_TIN_GUTI);
          mm_pending_lu_request = LTE_IRAT_LU;
        }
      }
      break;
   
    default:
      break;
  }

#ifdef FEATURE_LTE_REL9
  if(mm_is_reg_needed_for_mmit_on_GW())
  {
    MSG_HIGH_DS(MM_SUB,"=EMM= RAU/LAU to be performed - Mobility Management for IMS voice termination",0,0,0);
    emm_set_and_write_tin_type(NAS_MM_TIN_GUTI);
    mm_pending_lu_request = LTE_IRAT_LU;
  }
#endif
  if(local_status != MM_AS_LTOG_CCO)
  {
  /*Change the main routine handler func ptr */
  *mm_emm_dispatcher_fn_ptr = emm_process_LTE_to_GW_rat_change; 
  MSG_HIGH_DS(MM_SUB,"=EMM= Change the main routine handler to emm_process_LTE_to_GW_rat_change due to %d ",local_status ,0,0 );
  }

}


/*===========================================================================

FUNCTION MM_PROCESS_CCO_COMPLETE_IND

DESCRIPTION
  This function handles the CCO complete indication from LTE RRC.
  Cleans up the CCO states.

DEPENDENCIES
  None

RETURN VALUE
  TRUE

SIDE EFFECTS
  None

===========================================================================*/

void mm_process_cco_complete_ind(emm_ctrl_data_type *emm_ctrl_data_ptr)
{
  MSG_HIGH_DS(MM_SUB,"=EMM= Set L2GCCO state to INACTIVE, previous cco state %d",
                   emm_ctrl_data_ptr->l2gcco_state,0,0);
  emm_ctrl_data_ptr->l2gcco_state = EMM_LTOG_CCO_INACTIVE;
}

/*===========================================================================

FUNCTION EMM_PROCESS_IRAT_RRC_PRIMITIVE

DESCRIPTION
  This function overrides the function emm_process_rrc_primitive()
  It is called only during IRAT

DEPENDENCIES
  None

RETURN VALUE
  TRUE

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_process_irat_rrc_primitive
(
  void                 *rrc_message_router_cmd,
  emm_ctrl_data_type   *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp **mm_emm_dispatcher_fn_ptr
)
{
  emm_rrc_cmd_type   *rrc_msgr_cmd = NULL;

  ASSERT(rrc_message_router_cmd != (void *)NULL);
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL);

  rrc_msgr_cmd = (emm_rrc_cmd_type*)rrc_message_router_cmd;

  switch(rrc_msgr_cmd->rrc_msgr_cmd.msg_hdr.id)
  {
    case LTE_RRC_CONN_EST_CNF:
      emm_process_connection_confirmation(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_conn_est_cnf,emm_ctrl_data_ptr) ;
      break ;
    case LTE_RRC_CONN_REL_IND:
      emm_process_rrc_release_indication(&rrc_msgr_cmd->rrc_msgr_cmd.rrc_conn_rel_ind,emm_ctrl_data_ptr) ;
      break ;
    case LTE_RRC_DEACTIVATE_CNF:
      if(emm_ctrl_data_ptr->reg_req_pended_indication == TRUE)
      {
        emm_ctrl_data_ptr->reg_req_pended_indication = FALSE;
        MSG_HIGH_DS(MM_SUB,"=EMM= Reset reg_req_pended_indication to FALSE",0,0,0);
      }
      
      /* LTE stack is deactivated. Clean up message buffer. */
      emm_clean_msg_buffer(emm_ctrl_data_ptr);

      /* Leaving LTE. Clean up PDN CONN REQ for ATTACH if necessary */
      emm_release_attach_pdn_conn_req(emm_ctrl_data_ptr);

      emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending = FALSE;

      if(emm_ctrl_data_ptr->ps_detach_info.status == EMM_PS_DETACH_REQ_PENDING)
      {
        emm_process_ps_detach_req(emm_ctrl_data_ptr->ps_detach_info.ps_detach_req, 
                                  emm_ctrl_data_ptr);
      }
      else if(emm_ctrl_data_ptr->ps_detach_info.status == 
                                                     EMM_PS_DETACH_CNF_PENDING)
      {
        emm_ps_detach_clean_up(emm_ctrl_data_ptr);  
      }

      /* Free the memory allocated dynamically */
      emm_procedure_message_mem_free(emm_ctrl_data_ptr);

      /* If MM receives reg request during IRAT from LTE to GSM then set the variable
         mm_service_loss_on_multimode_rat to TRUE. This variable is checked before 
         deriving keys for GW for LTE*/
      if((mm_irat_reg_cmd_pending == TRUE)&&
         (mm_irat_pending_reg_cmd.cmd.hdr.message_set == MS_MM_REG) &&
         (mm_irat_pending_reg_cmd.cmd.hdr.message_id == (byte)MMR_REG_REQ))
      {
        emm_service_loss_on_lte();
      }
      /* On sending rat resume to SM, SM will send Modify req to SNDCP and SNDCP will
           inturn send LL-ACT-IND to LLC. If user plane is not in suspended state,
           LLC would trigger XID command prior to sending RAU.
           So suspend LLC first and then send rat resume to SM.
      */
#if defined(FEATURE_NAS_FREE_FLOAT) && defined(FEATURE_GSM_GPRS)
      if(is_gsm_mode())
      {
        if (gmm_state == GMM_REGISTERED)
        {
          gmmllc_reinit(GMM_STANDBY_STATE) ;
          gmm_pended_rat_resume_req = TRUE;
          gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_G_RAT_RESUMED,FALSE,0);
        }
        else
        {          
          gmmllc_reset_llc_conn_state();
          gmm_sm_send_rat_resume_req();
        }
      }
      else
#endif
      {
        /* If we do the rat_resume to SM then we need to wait for resume_cfn, in that case activation_rsp should be sent
           from mmcoord_route_irat_mm_emm_messages */
        gmm_sm_send_rat_resume_req();
      }
      break;

    case LTE_RRC_DL_DATA_IND:
      MSG_ERROR_DS(MM_SUB,"LTE_RRC_DL_DATA_IND is received during IRAT, this message shall be currently discarded",0,0,0);
      emm_free_dsm_memory_in_rrc_data_ind((lte_rrc_dl_data_ind_s *)\
                                           &(rrc_msgr_cmd->rrc_msgr_cmd.rrc_dl_data_ind));
    break;

    default:
      break;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION  MMMULTIMODE_ROUTE_L2GW_IRAT_MM_EMM_MESSAGES

DESCRIPTION
  This function overrides the function mmcoord_route_mm_emm_messages()
  It is called only during L2GW IRAT

DEPENDENCIES
  None

RETURN VALUE
  TRUE

SIDE EFFECTS
  None

===========================================================================*/
boolean mmmultimode_route_L2GW_irat_mm_emm_messages
(
  void                   *command_ptr,
  emm_ctrl_data_type     *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp   **mm_emm_dispatcher_fn_ptr
)
{
  mm_cmd_type  *mm_cmd_ptr = NULL;

  ASSERT(command_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr != NULL);
  ASSERT(mm_emm_dispatcher_fn_ptr != NULL);

  mm_cmd_ptr = (mm_cmd_type*)command_ptr;
  switch(mm_cmd_ptr->cmd.hdr.message_set)
  {
    case MS_GMM_SM:
      switch(mm_cmd_ptr->cmd.hdr.message_id)
      {
        case GMMSM_RAT_RESUME_CNF:
#ifdef FEATURE_SGLTE 
          MSG_HIGH_DS(MM_SUB,"=EMM= mm_irat_reg_cmd_pending = %d  mm_sglte_current_stack_status  = %d  MM_IS_IN_SGLTE_MODE = %d", 
                      mm_irat_reg_cmd_pending, mm_sglte_current_stack_status ,MM_IS_IN_SGLTE_MODE);
#endif
          emm_ctrl_data_ptr->ignore_GW_activation_ind = FALSE;
          mm_send_cm_bearer_ctxt_transfer_ind(mm_serving_plmn.info.active_rat);
          if( mm_serving_plmn.info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS
#ifdef FEATURE_SEGMENT_LOADING
      && get_wcdma_interface()
#endif
)
          {
            mm_send_rrc_activation_rsp(SYS_RAT_LTE_RADIO_ACCESS, SYS_RAT_UMTS_RADIO_ACCESS );
          }
#ifdef FEATURE_TDSCDMA
          else if( mm_serving_plmn.info.active_rat == SYS_RAT_TDS_RADIO_ACCESS
#ifdef FEATURE_SEGMENT_LOADING
      && get_tdscdma_interface()
#endif
)
          {
            mm_send_rrc_activation_rsp(SYS_RAT_LTE_RADIO_ACCESS, SYS_RAT_TDS_RADIO_ACCESS );
          }
#endif
          else
          {
    #if defined (TEST_FRAMEWORK) || defined (TF_UNIT_TEST)
            #error code not present
#endif
    #ifdef FEATURE_SGLTE  
            if(MM_IS_IN_SGLTE_MODE && (mm_sglte_current_stack_status ==SGLTE_MM_LOWER_LAYER_MERGING))
            {    
              /* removing deact indication here */
              /* mmsend_mmr_stack_deact_ind();*/
               mmcoord_process_merging();
               mmsend_mmr_service_stat_ind(mm_ps_stack_id);
               mm_sglte_current_stack_status =SGLTE_INACTIVE_STATE;
            }  
#endif 
            mm_send_rr_activation_rsp(SYS_RAT_LTE_RADIO_ACCESS);
          }
          if(emm_ctrl_data_ptr->is_srvcc_in_progress == TRUE)
          {
            mm_send_mmcnm_srvcc_handover_complete(FALSE); 

            if ((emm_ctrl_data_ptr->early_call_init == TRUE)&&
                (mm_csfb_state== MM_CSFB_STATE_MO_CALL_ORIG))
            {
              MSG_HIGH_DS(MM_SUB,"=EMM= Processing Early CSFB Call for SRVCC ", 0,0,0 );
              mm_csfb_process_pending_call();
            }
            emm_ctrl_data_ptr->is_srvcc_in_progress = FALSE;
          }
          /* Change back to main routine handler. To receive Service_indication on GW*/
          *mm_emm_dispatcher_fn_ptr = mm_emm_normal_msg_handler;
    
#ifdef FEATURE_LTE_REL9
          gmm_handle_emc_ps_srv(emm_ctrl_data_ptr);
#endif  
    
          MSG_HIGH_DS(MM_SUB,"=EMM= Change the main routine handler to mm_emm_normal_msg_handler", 0,0,0 );
    
          if (mm_nreg_req_received)
          {
            mm_nreg_req_received = FALSE;
            mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                            SYS_SRV_DOMAIN_NO_SRV,
                                            FALSE,
                                            SYS_SRV_STATUS_LIMITED,
                                            TRUE );
            if ((gmm_state != GMM_NULL) && (gmm_state != GMM_DEREGISTERED))
            {
              gmm_set_state( GMM_DEREGISTERED );
            }
#ifdef FEATURE_SGLTE
            /*In SGLTE mode , mm might not be able to handle state control directly ,
               So allowing mm to process pended STOP MODE REQ right from beginning */
            if(MM_IS_IN_SGLTE_MODE)
            {
              (void)mmcoord_route_mm_emm_messages(&mmcoord_pending_reg_message, 
                                           emm_ctrl_data_ptr, mm_emm_dispatcher_fn_ptr); 
            }
            else 
#endif
            {
               mm_state_control( MM_STD_MS_DEACTIVATED_NO_ATTACH );
            }
          }
          else
          /* Process pending REG CMD if there is any */
          if(mm_irat_reg_cmd_pending == TRUE)
          {
            mm_irat_reg_cmd_pending = FALSE;
            if(mm_irat_pending_reg_cmd.cmd.hdr.message_set==MS_MM_REG &&
			   mm_irat_pending_reg_cmd.cmd.hdr.message_id==MMR_REG_REQ)
			
			{		 
				 mm_service_loss_on_rat=mm_serving_plmn.info.active_rat;
            }
           (void)mmcoord_route_mm_emm_messages(&mm_irat_pending_reg_cmd,
                                                emm_ctrl_data_ptr,
                                                mm_emm_dispatcher_fn_ptr);

          }
         break;

        default:
          MSG_HIGH_DS(MM_SUB,"=EMM= Dropping SM CMD %d during L2GW IRAT",
                   mm_cmd_ptr->cmd.hdr.message_id,0,0);
          break;
      }
      break;

    case MS_MM_REG:
      switch(mm_cmd_ptr->cmd.hdr.message_id)
      {
        case MMR_PLMN_SEARCH_REQ:
        case MMR_REG_REQ:
        case MMR_STOP_MODE_REQ:
        case MMR_PS_DETACH_REQ:
          /* Buffer the REG CMD. It will be processed when GW is activated. */
          MSG_HIGH_DS(MM_SUB,"=EMM= Buffering REG CMD %d rcved during L2GW IRAT", 
                   mm_cmd_ptr->cmd.hdr.message_id, 0, 0);
          mm_irat_reg_cmd_pending = TRUE;          
          memscpy((void *)&mm_irat_pending_reg_cmd,sizeof(mm_cmd_type), 
                 command_ptr,sizeof(mm_cmd_type));             
          break;

        case MMR_SET_DRX_REQ:
          MSG_HIGH_DS(MM_SUB,"=MM= MM received in GW2L inter-rat, return failure", 0,0,0);
          mm_send_mmr_set_drx_cnf(FALSE);
          break;

        case MMR_GET_DRX_REQ:
          mm_send_mmr_get_drx_cnf();
          break;

#ifdef FEATURE_FEMTO_CSG
        case MMR_FILE_REFRESH_IND:
          mmcoord_route_mm_message(mm_cmd_ptr,mm_emm_dispatcher_fn_ptr); 
          break;
#endif 

        case MMR_SIM_STATE_UPDATE_IND:
          mm_update_sim_state(SYS_SIM_STATE_AVAILABLE);
          break;

        case MMR_BLOCK_PLMN_REQ:
          emm_process_reg_command(mm_cmd_ptr, emm_ctrl_data_ptr);   
          break;

        case MMR_CLEAR_LAI_REJECT_LIST_REQ:
          emm_process_reg_command(mm_cmd_ptr,emm_ctrl_data_ptr);
          mmcoord_route_mm_message(mm_cmd_ptr,mm_emm_dispatcher_fn_ptr);
          break;

#ifdef FEATURE_DUAL_SIM
        case MMR_DS_STAT_CHGD_REQ:
        case MMR_DS_TUNEAWAY_STATUS_CHANGE_IND:
        case MMR_DDS_SWITCH_IND:
          mmcoord_route_mm_message(mm_cmd_ptr,mm_emm_dispatcher_fn_ptr);
          break;
#endif

#ifdef FEATURE_FEMTO_CSG
         case MMR_CSG_SELECT_CONFIG_UPDATE_IND:
          mmcoord_route_mm_message(mm_cmd_ptr,mm_emm_dispatcher_fn_ptr); 
          break;
#endif 

        case MMR_PSM_READY_REQ:
          if (is_umts_mode() || is_gsm_mode())
          {
            mmcoord_route_mm_message(mm_cmd_ptr,mm_emm_dispatcher_fn_ptr);
          }
          else
          {
            mm_send_mmr_psm_ready_rsp(SYS_PSM_STATUS_READY);
          }
          break;

        default:
          MSG_HIGH_DS(MM_SUB,"=EMM= Dropping REG CMD %d during L2GW IRAT",
                   mm_cmd_ptr->cmd.hdr.message_id,0,0);
          break;
      }
      break;

     case MS_TIMER:
       /* T3346 is common to MM/EMM. depending on active rat route it */
       if((mm_cmd_ptr->cmd.timer_expiry.timer_id >= MIN_MM_TIMER && 
               mm_cmd_ptr->cmd.timer_expiry.timer_id <= MAX_MM_ONLY_TIMER)
          &&
          ((mm_cmd_ptr->cmd.timer_expiry.timer_id != TIMER_T3346) ||
           ((is_umts_mode() || is_gsm_mode()))))
       {
         mmcoord_route_mm_message(mm_cmd_ptr,mm_emm_dispatcher_fn_ptr );
       }
       else
       {
         emm_process_timer_expiry(mm_cmd_ptr, emm_ctrl_data_ptr);
       }
       break;

    case MS_MM_RRC:
    case MS_MM_TDSRRC:
      mmcoord_route_mm_message(mm_cmd_ptr,mm_emm_dispatcher_fn_ptr );
      break;
    case MS_MM_RR:
      if ((mm_cmd_ptr->cmd.hdr.message_id == RR_GMM_GPRS_MULTISLOT_CLASS_IND) ||
	  	  (mm_cmd_ptr->cmd.hdr.message_id == RR_SYNC_IND))
      {
        mmcoord_route_mm_message(mm_cmd_ptr,mm_emm_dispatcher_fn_ptr );
      }
      else
      {
        MSG_HIGH_DS(MM_SUB,"=EMM= Dropping message with message set %d message id %d during L2GW IRAT",
              mm_cmd_ptr->cmd.hdr.message_set, mm_cmd_ptr->cmd.hdr.message_id, 0);
      }
      break;
    case MS_CC_MM:
    case MS_CC_MM_PS:
      switch(mm_cmd_ptr->cmd.hdr.message_id)
      {
        case MMCNM_EST_REQ:
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION        
          mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE,MMCC_SMS_CS);
#else
          mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE);
#endif
          break;

        case MMCNM_REL_REQ:
           mm_cnm_rel_req_received = TRUE;
           mm_send_mmcnm_rel_cnf();
           if(mm_csfb_get_state() == MM_CSFB_STATE_MO_CALL_ORIG)
           {
             MSG_HIGH_DS(MM_SUB,"=EMM= MO CSFB Call ended",0,0,0); 
             mm_csfb_clear_csfb_call();
             if(mm_serving_plmn.info.active_rat == SYS_RAT_GSM_RADIO_ACCESS)
             {
               emm_ctrl_data_ptr->rr_blind_redir_pending_call_abort = TRUE;
             }
           }
           break;
        
        case MMCNM_DATA_REQ:
           MSG_HIGH_DS(MM_SUB,"Ignoring MMCNM_DATA_REQ in L2GW I-RAT handler",0,0,0);
           break;
        
        case MMCC_REEST_REQ:
           MSG_HIGH_DS(MM_SUB,"Ignoring MMCC_REEST_REQ in L2GW I-RAT handler",0,0,0);
           break;
        
        case MMCNM_UNIT_DATA_REQ:
           MSG_HIGH_DS(MM_SUB,"Ignoring MMCNM_UNIT_DATA_REQ in L2GW I-RAT handler",0,0,0);
           break;
        
#ifdef FEATURE_CCBS 
        case MMCNM_PROMPT_RES:
           MSG_HIGH_DS(MM_SUB,"Ignoring MMCNM_PROMPT_RES in L2GW I-RAT handler",0,0,0);
           break;
#endif
        
        default:
          MSG_ERROR_DS(MM_SUB,"Unexpected CNM/MN to MM message ID %d",mm_cmd_ptr->cmd.hdr.message_id,0,0);
          break;
      }
      break;

#if defined(FEATURE_NAS_FREE_FLOAT) && defined(FEATURE_GSM_GPRS)
    case MS_LLC_GMM :
      gmmllc_handle_llc_event( mm_cmd_ptr ) ;
     break;
#endif

    default:
      MSG_HIGH_DS(MM_SUB,"=EMM= Dropping message with message set %d message id %d during L2GW IRAT",
               mm_cmd_ptr->cmd.hdr.message_set, mm_cmd_ptr->cmd.hdr.message_id, 0);
      break;
  }

  return TRUE;
}/* mmmultimode_route_L2GW_irat_mm_emm_messages */

/*===========================================================================

FUNCTION emm_get_gw_identity_from_lte

DESCRIPTION
  This function maps the GUTI to RAI, P-TMSI and first byte of P-TMSI signature as per spec 23.003 section 2.8.2 and 
  2.8.2.1. It is called only during IRAT

DEPENDENCIES
  None

RETURN VALUE
  TRUE

SIDE EFFECTS
  None

===========================================================================*/

void emm_get_gw_identity_from_lte(mm_gw_identity_param_type *mm_identity, 
                                  emm_ctrl_data_type  *emm_ctrl_data_ptr) 
{
  ASSERT(emm_ctrl_data_ptr != NULL);

  ASSERT(mm_identity != (void *)NULL);

  memset(mm_identity, 0, sizeof(mm_gw_identity_param_type));

  /* Populate P-TMSI from GUTI. As per spec 23.003, section 2.8.2 and 2.8.2.1*/
  /* Populate from M-TMSI*/
  mm_identity->ptmsi.digit[0] = emm_ctrl_data_ptr->emm_guti.m_tmsi[3] & 0x3f;
  mm_identity->ptmsi.digit[2] = emm_ctrl_data_ptr->emm_guti.m_tmsi[1];
  mm_identity->ptmsi.digit[3] = emm_ctrl_data_ptr->emm_guti.m_tmsi[0]; 

  /* Set bits 30 and 31 to 1*/
  mm_identity->ptmsi.digit[0] = mm_identity->ptmsi.digit[0] | 0xc0;

  /* Populate from MME code. This is the NRI part of P-TMSI*/
  mm_identity->ptmsi.digit[1] = emm_ctrl_data_ptr->emm_guti.mme_code;

  /* Populate from M-TMSI byte 3. */
  mm_identity->ptmsi_signature.value[0] = emm_ctrl_data_ptr->emm_guti.m_tmsi[2];


  /* Populate RA*/
  mm_identity->ra_id.routing_area_code = emm_ctrl_data_ptr->emm_guti.mme_code;
  mm_identity->ra_id.location_area_code.lac[0] = 
                                   emm_ctrl_data_ptr->emm_guti.mme_group_id[1];
  mm_identity->ra_id.location_area_code.lac[1] = 
                                   emm_ctrl_data_ptr->emm_guti.mme_group_id[0];
  mm_identity->ra_id.plmn_id = emm_ctrl_data_ptr->emm_guti.plmn;

}

/*===========================================================================

FUNCTION mm_update_gw_auth_keys

DESCRIPTION
  This function updates the gw related security keys from the lte.

DEPENDENCIES
  None

RETURN VALUE
  TRUE

SIDE EFFECTS
  None

===========================================================================*/

void mm_update_gw_auth_keys(mm_gw_identity_param_type gw_identity, 
                                  emm_ctrl_data_type  *emm_ctrl_data_ptr,
                                  rrc_sim_update_e_type sim_update_type)
{
  auth_gsm_cipherkey_type     gsm_cipherkey;
  auth_gsm_cipherkey_type     gsm_kc_128bit;

  ASSERT(emm_ctrl_data_ptr != NULL);

  MSG_HIGH_DS(MM_SUB,"Updating the mapped security keys for GW", 0,0,0 );

  //Set fourth bit of eKSI to 0
  gw_identity.eKSI = gw_identity.eKSI & 0x07;

  /*************************************
  33.401 9.1.1 Idle mode procedures in UTRAN 
   **  Mapping of EPS security context to UMTS security context
   ** ...............................................
   **Keys CK' and IK' and the KSI shall replace the currently stored values 
   **on the USIM. START shall be reset to 0 on USIM. 
  ****************************************/  
  auth_generate_kc(&gw_identity.Ck, &gw_identity.Ik, &gsm_cipherkey,&gsm_kc_128bit);

  auth_update_security_keys(RRC_PS_DOMAIN_CN_ID,
                            gw_identity.eKSI,
                            0xFF,
                            &gw_identity.Ck,
                            &gsm_cipherkey, 
                            &gw_identity.Ik,
                            &gsm_kc_128bit);

  (void)auth_save_security_context(gw_identity.eKSI,
                                                   RRC_PS_DOMAIN_CN_ID,
                                                   &gw_identity.Ck,
                                                   &gw_identity.Ik,
                                                   &gsm_cipherkey);

   auth_notify_lower_layer(&gw_identity.Ck,
                                              &gw_identity.Ik,
                                              &gsm_cipherkey,
                                              RRC_PS_DOMAIN_CN_ID,
                                              sim_update_type);

}


/*===========================================================================

FUNCTION      emm_update_rplmn_type

DESCRIPTION

  Function updates the rplmn type in the emm ctrl database
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_update_rplmn_type
(
  mm_rplmn_type_enum_T rplmn_type
)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);
  if(emm_ctrl_data_ptr->rplmn_info.rplmn_type != rplmn_type)
  {
    MSG_HIGH_DS_2(MM_SUB,"RPLMN type changed from %d to %d",
        emm_ctrl_data_ptr->rplmn_info.rplmn_type,rplmn_type);
    emm_ctrl_data_ptr->rplmn_info.rplmn_type = rplmn_type;
  }
}

/*===========================================================================

FUNCTION      emm_update_rplmn

DESCRIPTION

  Function updates the rplmn in the emm ctrl database

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_update_rplmn
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  boolean rplmn_changed = FALSE;
  ASSERT(emm_ctrl_data_ptr != NULL);
  if((emm_ctrl_data_ptr->rplmn_info.rplmn_type == MM_CS_RPLMN_TYPE) &&
     (mm_location_information.location_update_status == UPDATED))
  {
    if(memcmp(emm_ctrl_data_ptr->rplmn_info.rplmn.identity,
                   mm_location_information.lai,PLMN_SIZE) != 0 )
    {
      rplmn_changed = TRUE;
      memscpy(emm_ctrl_data_ptr->rplmn_info.rplmn.identity,sizeof(sys_plmn_id_s_type),
                        mm_location_information.lai,PLMN_SIZE);
    }
  }
  else if((emm_ctrl_data_ptr->rplmn_info.rplmn_type == MM_PS_RPLMN_TYPE) &&
          (gmm_update_status == GMM_GU1_UPDATED))
  {
    if(memcmp(emm_ctrl_data_ptr->rplmn_info.rplmn.identity,
                   gmm_stored_rai.plmn_id.identity,PLMN_SIZE) != 0 )
    {
      rplmn_changed = TRUE;
      memscpy(emm_ctrl_data_ptr->rplmn_info.rplmn.identity,sizeof(sys_plmn_id_s_type),
                        gmm_stored_rai.plmn_id.identity,PLMN_SIZE);
    }
  }
  else if((emm_ctrl_data_ptr->rplmn_info.rplmn_type == MM_EPS_RPLMN_TYPE) &&
          (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED))
  {
    if(memcmp(emm_ctrl_data_ptr->rplmn_info.rplmn.identity,
                   emm_ctrl_data_ptr->last_registered_tai.plmn.identity,PLMN_SIZE) != 0 )
    {
      rplmn_changed = TRUE;
      memscpy(emm_ctrl_data_ptr->rplmn_info.rplmn.identity,sizeof(sys_plmn_id_s_type),
                    emm_ctrl_data_ptr->last_registered_tai.plmn.identity,PLMN_SIZE);
    }
  }
  else if(sys_plmn_id_is_undefined(emm_ctrl_data_ptr->rplmn_info.rplmn) == FALSE)
  {
    rplmn_changed = TRUE;
    sys_plmn_undefine_plmn_id(&emm_ctrl_data_ptr->rplmn_info.rplmn);
  }
  MSG_HIGH_DS_5(MM_SUB,"rplmn_type %d rplmn_changed %d location_update_status %d gmm_update_status %d emm_update_status %d",
	emm_ctrl_data_ptr->rplmn_info.rplmn_type,rplmn_changed,mm_location_information.location_update_status,
	gmm_update_status,emm_ctrl_data_ptr->emm_update_status);
  return rplmn_changed;
}



/*===========================================================================

FUNCTION      emm_update_rrc_rplmn_info

DESCRIPTION

  Function updates the rplmn info to the lower layers if it changed from the previous update

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_update_rrc_rplmn_info
(
  void
)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);
  if(emm_update_rplmn(emm_ctrl_data_ptr) == TRUE)
  {
    if(mm_is_multimode_sub())
    {
      emm_send_rrc_system_update(emm_ctrl_data_ptr);
    }
  }
}



/*===========================================================================

FUNCTION      emm_set_state_multimode_attach_failure

DESCRIPTION

  Called to set EMM state, when ATTACH failure happens in GW and when LTE is present in multimode

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void emm_set_state_multimode_attach_failure(lte_nas_emm_cause_type cause)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);

  /* Set flag to indicate GW reject handling */
  emm_ctrl_data_ptr->gw_reject_update = TRUE;
  emm_process_attach_failure_multimode(cause,emm_ctrl_data_ptr);
}

/*===========================================================================

FUNCTION    emm_set_state_multimode_registration_success  

DESCRIPTION
  Called when ATTACH/RAU succeed in GW, to set the EMM state when LTE is present in multimode

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void emm_set_state_multimode_registration_success
(
  boolean  is_attach
)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
  /* We can't call the macro EMM_MOVES_TO_REGISTERED_STATE(x)  as it is doing more than what is needed*/
  EMM_SET_STATE(EMM_REGISTERED) ;
  emm_ctrl_data_ptr->emm_substate = (emm_substate_type)EMM_REGISTERED_NO_CELL_AVAILABLE ;
  if(is_attach == TRUE)
  {
    emm_ctrl_data_ptr->tau_pending_gw_attach = TRUE;
#ifdef FEATURE_LTE_REL11
    if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)
    {
      emm_ctrl_data_ptr->emm_attach_attempt_counter = 0;
      emm_ctrl_data_ptr->emm_attach_rej_esm_fail_counter = 0;
      emm_ctrl_data_ptr->apn_switch = FALSE;
    }
#endif
  }
  emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_NONE;
}


/*===========================================================================

FUNCTION    emm_set_radio_cap_update_state  

DESCRIPTION
  This function checks if UE needs to perform TAU due to update radio capabilities when UE moves to LTE
  next time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void emm_set_radio_cap_update_state
(
  uint32  additional_info
)
{
  emm_radio_cap_update_enum_T prev_radio_cap_update;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  boolean radio_cap_update_flag;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);

  prev_radio_cap_update = emm_ctrl_data_ptr->radio_cap_update;
  radio_cap_update_flag = emm_bit_mask_check(additional_info,
                  (byte)SYS_ADDTNL_INFO_FORCE_RADIO_CAP_UPDATE);

  if((radio_cap_update_flag == TRUE) &&
     (EMM_GET_STATE() == EMM_REGISTERED))
  {
    if(emm_ctrl_data_ptr->radio_cap_update == EMM_RADIO_CAP_UPDATE_NONE)
    {
      emm_ctrl_data_ptr->radio_cap_update = EMM_RADIO_CAP_UPDATE_PENDING;
    }
    else if(emm_ctrl_data_ptr->radio_cap_update == EMM_RADIO_CAP_UPDATE_PENDING)
    {
      emm_ctrl_data_ptr->radio_cap_update = EMM_RADIO_CAP_UPDATE_NONE;
    }
  }
  MSG_HIGH_DS(MM_SUB,"=EMM= Additional info radio cap flag %d, prev radio_cap_update %d new radio_cap_update %d",
                     radio_cap_update_flag,prev_radio_cap_update,emm_ctrl_data_ptr->radio_cap_update);
}


/*===========================================================================
 
FUNCTION      emm_set_state_multimode_rau_failure

DESCRIPTION
  Called when RAU failure happens when LTE is present in multimode

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void emm_set_state_multimode_rau_failure(lte_nas_emm_cause_type cause)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);

  MSG_HIGH_DS(MM_SUB,"=EMM= GW RAU REJ - Updating EMM w/ cause %d", cause,0,0);

#ifndef FEATURE_NAS_REL11
  if((cause == LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED) &&
     (gmm_ra_update_type != GMM_COMBINED_RA_LA_UPDATING) &&
     (gmm_ra_update_type != GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH))
  {
    MSG_MED_DS(MM_SUB,"=EMM= RAU REJ w/ cause 8 is abnormal for rau type %d",
            gmm_ra_update_type,0,0);
  }
  else
#endif
  {  
    /* Set flag to indicate GW reject handling */
    emm_ctrl_data_ptr->gw_reject_update = TRUE;
    emm_process_rau_sr_reject(TRACKING_AREA_REJECT,cause,emm_ctrl_data_ptr);
  }
}

/*===========================================================================
 
FUNCTION      emm_set_update_status_rau_abnormal_failure

DESCRIPTION  
 Called when RAU fails abnormally. This will set EMM update status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void emm_set_update_status_rau_abnormal_failure(emm_eps_update_status_type update_type)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);

  emm_ctrl_data_ptr->emm_update_status = update_type;
  emm_save_mobility_info(emm_ctrl_data_ptr);
}


/*===========================================================================
 
FUNCTION      emm_set_emm_param_attach_abnormal_failure

DESCRIPTION  
 Called when GW attach fails abnormally. This will set EMM state, EPS
update status, GUTI, last visited registered TAI, TAI list and KSI

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void emm_set_emm_param_attach_abnormal_failure(void)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
  
  /* EMM UPDATE STATUS */
  /* Delete GUTI */
  /* Delete last registered visited TAI */
  /* TAI List */
  /* Invalidate NAS Security parameters */ 
  emm_invalidate_mobility_info(EMM_STATUS_NOT_UPDATED,
                               SYS_SIM_STATE_AVAILABLE,
                               emm_ctrl_data_ptr, FALSE);
  EMM_SET_STATE(EMM_DEREGISTERED);
}


/*===========================================================================
 
FUNCTION      mm_set_update_status_tau_abnormal_failure

DESCRIPTION  
 Called when TAU fails abnormally. This will set GMM update status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_set_update_status_tau_abnormal_failure(gmm_update_status_type update_type)
{
  gmm_update_status = update_type;
  mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                         gmm_stored_ptmsi_signature,
                                         gmm_stored_rai,
                                         gmm_update_status );
}


/*===========================================================================
 
FUNCTION      mm_set_gmm_param_attach_abnormal_failure

DESCRIPTION  
 Called when LTE attach fails abnormally in MS. This will set GPRS update status, P-TMSI, 
 P-TMSI signature, RAI and GPRS ciphering key sequence number.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_set_gmm_param_attach_abnormal_failure(void)
{
  gmm_delete_ps_location_information();
  gmm_delete_gprs_cksn();
  gmm_update_status = GMM_GU2_NOT_UPDATED;
  mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                      gmm_stored_ptmsi_signature,
                                      gmm_stored_rai,
                                      gmm_update_status );
  
  gmm_set_state( GMM_DEREGISTERED ); 
}


/*===========================================================================
 
FUNCTION      gmm_handle_connection_management_gw_to_lte

DESCRIPTION
    

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void gmm_handle_connection_management_gw_to_lte(void)
{
  rabm_cmd_type            *rabm_cmd;

  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);

  gmm_ps_signalling_connected = FALSE;
  gmm_integrity_protection_activated = FALSE;
  
  switch(gmm_state)
  {
    case GMM_SERVICE_REQUEST_INITIATED:
     /*Cosider abnormal failure of service request*/
      gmm_set_state( GMM_REGISTERED );
      mm_stop_timer( TIMER_T3317 );
      
      if (gmm_service_request_initiator == MS_GMM_SM)
      {
        MSG_HIGH_DS(MM_SUB, "Sending GMMSM_ESTABLISH_REJ", 0,0,0 );
        gmm_send_gmmsm_establish_rej(PPD_UNKNOWN_REASON);
      }
      else if (gmm_service_request_initiator == MS_GMM_RABM)
      {
        rabm_cmd = rabm_get_cmd_buf();
        
        if (rabm_cmd == NULL)
        {
          MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
        }
        
        rabm_cmd->header.message_set = MS_GMM_RABM;
        rabm_cmd->header.cmd_id = GMMRABM_REESTABLISH_REJ;
        rabm_cmd->cmd.gmm_reestablish_rej.cause = PROTOCOL_ERROR_UNSPECIFIED;
        
        MSG_HIGH_DS(MM_SUB, "Sending GMMRABM_REESTABLISH_REJ", 0,0,0 );
        
        rabm_put_cmd( rabm_cmd );
      }
      else if (gmm_service_request_initiator == MS_CC_MM)
      {
        /* Send MMCNM_EST_REJ to CNM */
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
        mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL, MMCC_SMS_PS);
#else
        mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL);
#endif
      }
      
      /* Set the Service_request_initiator to NULL after sending the response to Initiator */
      gmm_service_request_initiator = NULL;      
      break; 

    case GMM_ROUTING_AREA_UPDATING_INITIATED:
      /* If L2G CCO fails due to PS RACH failure, then do not increment the rau
          attempt counter and do not start the rau attempt timers. */
      if(emm_ctrl_data_ptr->l2gcco_state == EMM_LTOG_CCO_IN_PROGRESS)
      {
        gmm_set_state( GMM_REGISTERED );
        mm_stop_timer( TIMER_T3330 );
      }
      else
      {
	 if(mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_STOP_MODE_REQ)
	 { 
            mm_irat_reg_cmd_pending = TRUE;          
           (void)memscpy( &mm_irat_pending_reg_cmd,sizeof(mm_cmd_type),&mmcoord_pending_reg_message, sizeof(mm_cmd_type));          
            mmcoord_pending_reg_message.cmd.hdr.message_id = 0;
            mm_nreg_req_received = FALSE;
	  }

      gmm_increment_routing_area_update_attempt_counter
      (
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
        FALSE
#endif
      );
      mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
      }
      if((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY)&&
          ((gmm_state == GMM_NULL)||(gmm_state == GMM_DEREGISTERED)))
      {
        mm_serving_plmn.info.active_rat = SYS_RAT_UMTS_RADIO_ACCESS;
        emm_multimode_detach();
        mm_serving_plmn.info.active_rat = SYS_RAT_LTE_RADIO_ACCESS;
        
      }
      break;

    case GMM_DEREGISTERED_INITIATED:
      /*If MM is processing stop mode request at time of inter-rat then save REG message and process it in 
          EMM after sending activation response. GMM/EMM state is set to deregistered
          in gmm_handle_gprs_detach_low_level_failure() */
      if(mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_STOP_MODE_REQ)
      { 
        mm_irat_reg_cmd_pending = TRUE;          
        (void)memscpy( &mm_irat_pending_reg_cmd,sizeof(mm_cmd_type),&mmcoord_pending_reg_message, sizeof(mm_cmd_type));          
        mmcoord_pending_reg_message.cmd.hdr.message_id = 0;
        mm_nreg_req_received = FALSE;
      }
      gmm_handle_gprs_detach_low_level_failure();      
	  mmsend_nas_reg_event_packet(PROC_NAS_MO_DETACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
      break;

    case GMM_REGISTERED_INITIATED:
      gmm_increment_gprs_attach_attempt_counter();
	  mmsend_nas_reg_event_packet(PROC_NAS_ATTACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);

    case GMM_REGISTERED:  
      /*If MM is processing stop mode request at time of inter-rat then save REG message and process it in 
          EMM after sending activation response. GMM/EMM state is set to deregistered  */
      if(mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_STOP_MODE_REQ)
      { 
        mm_irat_reg_cmd_pending = TRUE;          
        (void)memscpy( &mm_irat_pending_reg_cmd, sizeof(mm_cmd_type), &mmcoord_pending_reg_message, sizeof(mm_cmd_type));          
        mmcoord_pending_reg_message.cmd.hdr.message_id = 0;
        mm_nreg_req_received = FALSE;
        gmm_set_state(GMM_DEREGISTERED);
      }
      break;

    case GMM_NULL:
    case GMM_DEREGISTERED:
      /*If MM is processing stop mode request at time of inter-rat then save REG message and process it in 
          EMM after sending activation response.*/
      if(mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_STOP_MODE_REQ)
      { 
        mm_irat_reg_cmd_pending = TRUE;          
        (void)memscpy( &mm_irat_pending_reg_cmd, sizeof(mm_cmd_type), &mmcoord_pending_reg_message, sizeof(mm_cmd_type));          
        mmcoord_pending_reg_message.cmd.hdr.message_id = 0;
      }
#ifdef FEATURE_NAS_FREE_FLOAT
      if(mm_timer_status[TIMER_NW_INIT_DETACH] == TIMER_ACTIVE && 
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
         gmm_last_signalling_message_type == GMM_DETACH_ACCEPT)
#endif
      {
          if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF)
          {
             mm_send_mmr_ps_detach_cnf();
             mm_ps_detach_state = MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ;
          }
          if(gmm_gprs_attach_pending)
          {
            gmm_set_substate(GMM_ATTACH_NEEDED);
          }
          gmm_detach_in_progress = FALSE;
          gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_GMM_DETACHED, FALSE, 0 );
          gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_PTMSI_ASSIGNED,FALSE,0) ;
          gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_TLLI_UNASSIGNMENT,FALSE,0);
		  mm_stop_timer(TIMER_NW_INIT_DETACH);
      }
#endif
      break;


    default:
      MSG_HIGH_DS(MM_SUB, "default gmm_state = %d", gmm_state,0,0 );
      break;
    

  }

  /* 
  ** If GMM is in registered state and T3312 is not running then 
  ** handover from WCDMA or interrat from GSM when ready timer
  ** is running has happened so start T3312 in these cases
  */
  if((gmm_state == GMM_REGISTERED) &&
     (gmm_t3312_timer_value != 0) &&
     (mm_timer_status[ TIMER_T3312 ] == TIMER_STOPPED))
  {
    mm_start_timer(TIMER_T3312, gmm_t3312_timer_value);
  }

}

/*===========================================================================

FUNCTION  mm_emm_process_GW_to_LTE_rat_change    

DESCRIPTION
  This function is called in LTE when LTE NAS receives ACTIVATION_IND.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_emm_process_GW_to_LTE_rat_change
( 
  lte_rrc_act_ind_s      *act_ind_ptr,
  mm_emm_dispatcher_fp  **mm_emm_dispatcher_fn_ptr,
  emm_ctrl_data_type     *emm_ctrl_data_ptr
)
{
   mmr_service_ind_s_type  mmr_service_ind;
   sys_radio_access_tech_e_type previous_active_rat;
#ifdef FEATURE_SGLTE
    boolean mm_is_split_allowed = FALSE;
#endif

   ASSERT(act_ind_ptr != (void *)NULL);
   ASSERT(emm_ctrl_data_ptr != NULL);

   if (mm_nreg_req_received )
   {
     MSG_HIGH_DS(MM_SUB, "MMR_STOP_MODE_REQ pending - Received ACTIVATION_IND", 0, 0, 0);
   }

   if (mm_rat_change_pending)
   {
     MSG_HIGH_DS(MM_SUB, "RAT change pending - Ignoring ACTIVATION_IND", 0, 0, 0);
   }
   else if(mm_waiting_for_abort_cnf && mm_serving_plmn.info.active_rat == SYS_RAT_TDS_RADIO_ACCESS)
   {
     MSG_HIGH_DS(MM_SUB, "Abort CNF pending in TDS - Ignoring ACTIVATION_IND", 0, 0, 0);
   }
   else if (mm_waiting_for_deact_cnf)
   {
     MSG_HIGH_DS(MM_SUB, "Deactivation pending - Ignoring ACTIVATION_IND", 0, 0, 0);
   }
   else if (mm_waiting_for_stop_mode_cnf)
   {
     MSG_HIGH_DS(MM_SUB, "RAT Stop request pending - Ignoring ACTIVATION_IND", 0, 0, 0);
   }
   else if (mm_waiting_for_service_cnf)
   {
     MSG_HIGH_DS(MM_SUB, "Service or PLMN select request pending - Ignoring ACTIVATION_IND", 0, 0, 0);
   }
   else if ((rrc_connection_status == MMRRC_IDLE) &&
            (mm_waiting_for_list_cnf_type == SYS_NETWORK_SELECTION_MODE_MANUAL)
           )
   {
     MSG_HIGH_DS(MM_SUB, "Waiting for LIST CNF - Manual in Idle mode - Ignoring ACTIVATION_IND", 0, 0, 0);
   }
#ifdef FEATURE_WRLF_SYSTEM_SEL
   else if(mm_wcdma_rlf_state != WCDMA_RLF_IDLE)
   {
     MSG_HIGH_DS(MM_SUB, "Ignoring LTE_RRC_ACTIVATION_IND message rlf state %d", mm_wcdma_rlf_state,0,0 );
   }
#endif
   else if(emm_ctrl_data_ptr->l2gcco_state == EMM_LTOG_CCO_ABORTED)
   {
     MSG_HIGH_DS(MM_SUB, "Ignoring LTE_RRC_ACTIVATION_IND in CCO aborted state", 0,0,0 );
   }
   else if(!mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                      SYS_RAT_LTE_RADIO_ACCESS))
   {
     MSG_FATAL_DS(MM_SUB,"=EMM= Interrat to LTE when rat priority list does not have LTE rat ",0,0,0) ;
   }
   else
   {
    switch(act_ind_ptr->act_reason)
    {
       case LTE_RRC_ACT_IRAT_REDIR:
       case LTE_RRC_ACT_IRAT_CELL_RESEL:
       case LTE_RRC_ACT_IRAT_CCO_FAILURE:  
       case LTE_RRC_ACT_IRAT_HO:
         MSG_HIGH_DS(MM_SUB, "LTE activation received with cause %d", act_ind_ptr->act_reason, 0, 0);
         /* Stop MM rel timer here. We might have started detach while in GW and got reselected to LTE 
            before RRC/RR connection is established.*/
 #ifdef FEATURE_SGLTE 
         mm_is_split_allowed =(MM_IS_IN_SGLTE_MODE &&
		                    (mmcoord_pending_reg_message.cmd.hdr.message_id != (int)MMR_STOP_MODE_REQ))?TRUE :FALSE; 
         if(MM_SUB_IS_SGLTE)
         {
           MSG_HIGH_DS(MM_SUB,"=EMM=MM_IS_IN_SGLTE_MODE =%d mm_sglte_current_stack_status = %d ",MM_IS_IN_SGLTE_MODE,mm_sglte_current_stack_status,0);
         }
         /* Don't want to handle CCO failure case here ,because of high priority request */
         if (MM_IS_IN_SGLTE_STATE_4 && (act_ind_ptr->act_reason != LTE_RRC_ACT_IRAT_CCO_FAILURE) &&
             (mm_is_split_allowed == TRUE))
         {
            mm_copy_cs_database(MM_SGLTE_STACK_2, mm_ps_stack_id);
            mm_sglte_current_stack_status  = SGLTE_MM_LOWER_LAYER_SPLITING;
            mmsend_mmr_stack_act_ind();
         }
 #endif             
         mm_stop_timer(TIMER_MM_REL);
         /* allocate the memory for EMM ctrl data variables */
         emm_procedure_message_mem_alloc(emm_ctrl_data_ptr);
       
         if(act_ind_ptr->act_reason != LTE_RRC_ACT_IRAT_HO)
         {
           /* Suspend DS first for redirection or reselection */ 
         mm_send_cm_ps_data_suspend_ind();
         }
         else
         {
           emm_make_mapped_ctxt_current_and_compute_ck_ik(emm_ctrl_data_ptr);
           EMM_SET_CONNECTION_STATE(EMM_CONNECTED_STATE);
           mm_stop_timer(TIMER_T3412);
         }

         /*
         ** If interrat happens while PS detach is going on, call the low level failure function
         ** so as to do a local detach. Also set the EMM state to "EMM_DEREGISTERED_PLMN_SEARCH".
         ** This way we dont start the registration in EMM as we are expecting service req from CM.
         */
         if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF)
         {
           gmm_set_state( GMM_DEREGISTERED_INITIATED );
           gmm_detach_type = GMM_GPRS_DETACH;
           gmm_handle_gprs_detach_low_level_failure();
           EMM_SET_STATE(EMM_DEREGISTERED);
           EMM_SET_SUBSTATE(EMM_DEREGISTERED_PLMN_SEARCH); 
         }
         else if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ)
         {
           EMM_SET_STATE(EMM_DEREGISTERED);
           EMM_SET_SUBSTATE(EMM_DEREGISTERED_PLMN_SEARCH); 
         }

         if((gmm_state == GMM_DEREGISTERED) && 
            (mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF) &&
            (is_lte_mode() == FALSE))
         {
           mm_stop_timer(TIMER_MM_PS_DETACH);
           gmm_gprs_attach_pending = FALSE;
           mm_send_mmr_ps_detach_cnf();
           mm_ps_detach_state = MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ;
		   if (mm_nreg_release_pending)
		   {
			 mm_stop_timer(TIMER_DETACH_REL);
			 mm_nreg_release_pending = FALSE;
		   }
#ifdef FEATURE_MODEM_HEAP
           if(mm_pending_mmr_ps_detach_req_ptr != NULL)
           {
             modem_mem_free(mm_pending_mmr_ps_detach_req_ptr,MODEM_MEM_CLIENT_NAS);
             mm_pending_mmr_ps_detach_req_ptr = NULL;
           }
#else
           memset(&mm_pending_mmr_ps_detach_req,0,sizeof(mm_cmd_type));
#endif
         }

         /* Send service cnf/ind only to REG */
         mm_to_only_reg = TRUE;
         mm_previous_serving_rat = mm_serving_plmn.info.active_rat;
         previous_active_rat = mm_serving_plmn.info.active_rat;
         ASSERT(previous_active_rat != SYS_RAT_NONE);

         /* Set the active RAT to TDS*/
         mm_serving_plmn.info.active_rat = SYS_RAT_LTE_RADIO_ACCESS;
         mm_coord_camped_cell_id = 0xFFFF;    
#ifdef FEATURE_ENHANCED_NW_SELECTION
         mm_inform_updated_foreground_search_list();
#endif

         if (mmcoord_reg_req_pending)
         {
            mmcoord_reg_req_pending  = FALSE;
            mm_plmn_list_req_pending = FALSE;
            /*Lower layer rat change happens to LTE network and copy the pended REG request context to LTE stack */

            MSG_HIGH_DS(MM_SUB, "=EMM= Buffering MMR_REG_REQ pended during IRAT in LTE context ",0, 0, 0);
            mm_irat_reg_cmd_pending = TRUE;          
            memscpy((void *)&mm_irat_pending_reg_cmd, sizeof(mm_cmd_type),&mmcoord_pending_reg_message, sizeof(mm_cmd_type));
         }
         else if (mm_deep_sleep_alt_scan && mm_hard_abort_pending)
         {
           mm_deep_sleep_alt_scan = FALSE;
           mm_hard_abort_pending  = FALSE;
            /*Lower layer rat change happens to LTE network and copy the pended stop mode request Alt Scan context to LTE stack */

           MSG_HIGH_DS(MM_SUB, "=EMM= Buffering Alt Scan Stop mode Req (%d) pended during IRAT in LTE context ",SYS_STOP_MODE_REASON_ALT_SCAN, 0, 0);
           mm_irat_reg_cmd_pending = TRUE;
           mmcoord_pending_reg_message.cmd.hdr.message_id = MMR_STOP_MODE_REQ;
           mmcoord_pending_reg_message.cmd.mmr_stop_mode_req.stop_mode_reason = SYS_STOP_MODE_REASON_ALT_SCAN;
           memscpy((void *)&mm_irat_pending_reg_cmd, sizeof(mm_cmd_type),&mmcoord_pending_reg_message, sizeof(mm_cmd_type));
         }

         if ((previous_active_rat == SYS_RAT_UMTS_RADIO_ACCESS) ||
             (previous_active_rat == SYS_RAT_TDS_RADIO_ACCESS))
         {
           if (rrc_connection_status != MMRRC_IDLE)
           {
             if(pmm_mode == PMM_CONNECTED)
             {
               if(act_ind_ptr->act_reason == LTE_RRC_ACT_IRAT_CELL_RESEL)
               {
                 if(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI)
                 {
                   emm_set_and_write_tin_type(NAS_MM_TIN_P_TMSI);
                 }
                 MSG_HIGH_DS(MM_SUB,"=EMM= TAU to be performed - PMM CONNECTED MODE",0,0,0);
                 emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
               }
               gmm_sm_send_con_release_ind(SIG_CONNECTION_RELEASE);
             }
             rrc_connection_status = MMRRC_IDLE;
             pending_mm_msg_valid   = FALSE;
             pending_gmm_msg_valid  = FALSE;             
           }
             /* Reject any pending CM service request */
           if(mm_held_cnm_service_req.present)
           {
             #ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
               mm_send_mmcnm_est_rej( AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL, MMCC_SMS_CS);
             #else
               mm_send_mmcnm_est_rej( AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL );
             #endif
           }

           mm_nreg_req_received = FALSE;

           mm_state_control( MM_STD_LOW_LEVEL_FAIL );
           /* Set mm substate to no cell available so that LU will not be triggered 
           in NMO1 network if max rau attempts are done.*/
           mm_substate_control( MM_STD_NO_SERVICE );

           gmm_handle_connection_management_gw_to_lte();           
         }
         else if (previous_active_rat == SYS_RAT_GSM_RADIO_ACCESS)
         {
		   /*In state 4 if stop mode is pending then allowed to reset cs variable */
#ifdef FEATURE_SGLTE
           if(!MM_IS_IN_SGLTE_STATE_4 ||( mm_is_split_allowed == FALSE)) 
#endif      
           {
		      if ((mm_state == MM_WAIT_FOR_RR_CONNECTION_MM) ||
               (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU) ||
               (mm_state == MM_WAIT_FOR_RR_CONNECTION_IMSI_DETACH) ||
               (mm_state == MM_WAIT_FOR_NETWORK_COMMAND))
              {
                  rrc_connection_status = MMRRC_IDLE;

                  /* Reject any pending CM service request */
                  if(mm_held_cnm_service_req.present)
                  {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                    mm_send_mmcnm_est_rej( AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL, MMCC_SMS_CS);
#else
                    mm_send_mmcnm_est_rej( AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL );
#endif
                  }
                  mm_nreg_req_received = FALSE;
                  if((act_ind_ptr->act_reason == LTE_RRC_ACT_IRAT_CCO_FAILURE) &&
                    (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU))
                  {
                    mm_state_control( MM_STD_CCO_FAILED );
                    mm_stop_timer( TIMER_T3210 );
                    // set the lu pending flag, so as to perform LU if UE moves to GW later.
                    mm_pending_lu_request = LTE_IRAT_LU;
                  }
                  else
                  {
                    mm_state_control( MM_STD_LOW_LEVEL_FAIL );
                    /* The following code to reset CSFB state is for scenario where CS call is triggered
                    ** while UE is in LTE but lower layers have started CCO. So after UE moves to 
                    ** GSM and places the call, if CCO fails UE recovers on LTE then we need to
                    ** reset CSFB state as CSFB state could be LU pending.
                    */
                    if(mm_csfb_state != MM_CSFB_STATE_NULL)
                    {
                      mm_csfb_set_state(MM_CSFB_STATE_NULL);
                    }
                 }
               }

               /* Set mm substate to no cell available so that LU will not be triggered 
               in NMO1 network if max rau attempts are done.*/
               mm_substate_control( MM_STD_NO_SERVICE );
            }
            if(act_ind_ptr->act_reason == LTE_RRC_ACT_IRAT_REDIR)
            {
              emm_set_tau_trm_priority(TRUE);
            }
            if (gmm_state != GMM_SERVICE_REQUEST_INITIATED)
            {
              gmm_handle_connection_management_gw_to_lte();
            }
         }
         else
         {
           MSG_FATAL_DS(MM_SUB,"=EMM= This shouldnot occur if GW to LTE RAT transition is happening",0,0,0) ;   
         }


         if (mm_reg_waiting_for_reg_cnf)
         {
           mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
           mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
           mm_send_mmr_reg_cnf();

           mm_reg_waiting_for_reg_cnf = FALSE;
           mm_ready_to_send_reg_cnf   = FALSE;
         }
         else
         {
           mmr_service_ind.service_state = mm_serving_plmn.info;
           mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
           mm_send_mmr_service_ind( &mmr_service_ind );
         }
         mm_to_only_reg = FALSE; 
         mm_previous_serving_rat = SYS_RAT_NONE;
         if (previous_active_rat == SYS_RAT_UMTS_RADIO_ACCESS)
         {
           mm_send_rrc_stop_mode_req(RRC_RAT_CHANGE, SYS_RAT_UMTS_RADIO_ACCESS);
         }
         else if(previous_active_rat == SYS_RAT_TDS_RADIO_ACCESS)
         {
           mm_send_rrc_stop_mode_req(RRC_RAT_CHANGE, SYS_RAT_TDS_RADIO_ACCESS);
         }
         else
         {
		   /*if stop mode is pending then we should honour stop mode than handle 
             CCO failure	   */
           if( act_ind_ptr->act_reason == LTE_RRC_ACT_IRAT_CCO_FAILURE
#ifdef FEATURE_SGLTE
             &&((MM_IS_NOT_IN_SGLTE_MODE)||(mm_is_split_allowed == TRUE))
#endif		    
			)
           {
             MSG_HIGH_DS(MM_SUB,"=EMM= Set L2GCCO state to INACTIVE, previous cco state %d",
                      emm_ctrl_data_ptr->l2gcco_state,0,0);
             emm_ctrl_data_ptr->l2gcco_state = EMM_LTOG_CCO_INACTIVE;
             mm_send_rr_stop_mode_req(RR_LTOG_CCO_FAILURE);             
             mm_csfb_rej_pending_call();
#ifdef FEATURE_SGLTE
             if (MM_IS_IN_SGLTE_MODE && (act_ind_ptr->act_reason == LTE_RRC_ACT_IRAT_CCO_FAILURE))
             {
               mmsend_mmr_stack_act_ind();
               mm_copy_cs_database(MM_SGLTE_STACK_2, mm_sub_id);
               mm_sglte_current_stack_status = SGLTE_MM_LOWER_LAYER_SPLITING;
             }
#endif
           }
          else
           {
#ifdef FEATURE_SGLTE
             if (MM_IS_IN_SGLTE_STATE_4)
             {
               if ((mm_irat_reg_cmd_pending==TRUE ) && 
                    (mm_irat_pending_reg_cmd.cmd.hdr.message_id == (int)MMR_STOP_MODE_REQ))
               {
                 mm_send_rr_stop_mode_req(RR_MM_RAT_CHANGE);
               }
               else
               {
                 mm_send_rr_stop_mode_req(RR_SGLTE_RAT_CHANGE);
               }
             }
             else
#endif    
             {
                mm_send_rr_stop_mode_req(RR_MM_RAT_CHANGE);
             }  
           }
         }

         // send RAT change to SM layer
         gmm_sm_send_rat_change_req(previous_active_rat, TRUE);
#ifdef FEATURE_LTE_REL9
         if(mm_is_reg_needed_for_mmit_on_LTE(previous_active_rat))
         {
           MSG_HIGH_DS(MM_SUB,"=EMM= TAU to be performed - Mobility Management for IMS voice termination",0,0,0);
           emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
           emm_set_and_write_tin_type(NAS_MM_TIN_P_TMSI);
         }
#endif
         if(mm_location_information.location_update_status != UPDATED)
         {
           emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC;
           MSG_HIGH_DS_0(MM_SUB,"=MM= Set SGs assosicaiton flag - MM update status is not updated on IRAT");
         }

         if ((mm_psm_ready_req_rejected == TRUE) &&
             (mm_check_ready_for_psm() == SYS_PSM_STATUS_READY))
         {
           mm_send_mmr_psm_ready_ind();
         }

         mm_cancel_authentication();
         gmm_cancel_authentication();

         //mm_rat_handler_status = MM_RAT_INITIAL;
         /*Change the main routine handler func ptr */
         *mm_emm_dispatcher_fn_ptr = mm_process_GW_to_LTE_rat_change;
         MSG_HIGH_DS(MM_SUB,"=EMM= MM: Change the main routine handler to mm_process_GW_to_LTE_rat_change due to %d", 
              act_ind_ptr->act_reason,0,0 );
         break;

      default:
         break;
       }
    }
}

/*===========================================================================

FUNCTION      mm_multimode_send_sim_update

DESCRIPTION
  This function sends SIM_UPDATE to all technologies

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_multimode_send_sim_update(void)
{

#ifdef FEATURE_LTE
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);

  emm_send_sim_update(emm_ctrl_data_ptr) ;
#endif

#ifdef FEATURE_GSM_GPRS
  mm_send_rr_sim_update_req();
#endif

#ifdef FEATURE_WCDMA
  /* For now do it for PS*/
#ifdef FEATURE_SEGMENT_LOADING
   if (get_wcdma_interface())
   {
 #endif
  mm_send_rrc_sim_update_req(RRC_PS_DOMAIN_CN_ID, SYS_RAT_UMTS_RADIO_ACCESS);
#ifdef FEATURE_SEGMENT_LOADING
   }
#endif

#endif
#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING
  if (get_tdscdma_interface())
  {
#endif
  mm_send_rrc_sim_update_req(RRC_PS_DOMAIN_CN_ID, SYS_RAT_TDS_RADIO_ACCESS);
#ifdef FEATURE_SEGMENT_LOADING
  }
#endif
#endif /* FEATURE_TDSCDMA */

}

/*===========================================================================

FUNCTION      MM_ACTIVATE_GW

DESCRIPTION
  This function initilize the MM/GMM state. EMM should call this function for every REG request for LTE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_activate_gw
(
  void
)
{
  // if mm state is NULL set it to idle, no cell available
  if(mm_state == NULL)
  {
    mm_activate_state( FALSE );
    mm_substate_control( MM_STD_NO_SERVICE );     
  }
  // if gmm state is NULL and PS is enabled set GMM state
  if(gmm_state == GMM_NULL && 
     (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY ||
      mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS))
  {
    gmm_set_state( GMM_DEREGISTERED );
    gmm_set_substate( GMM_NO_CELL_AVAILABLE );  
  }
  // if PS is not enabled set GMM states to NULL
  else if ( mm_serving_plmn.ms_op_mode != SYS_SRV_DOMAIN_PS_ONLY &&
             mm_serving_plmn.ms_op_mode != SYS_SRV_DOMAIN_CS_PS)
  {
    gmm_set_state( GMM_NULL );
    gmm_set_substate( GMM_NULL_SUBSTATE );
  }
}


/*===========================================================================

FUNCTION      MM_UPDATE_PS_REGISTRATION_SUCCESS_STATUS

DESCRIPTION
  This updates the GMM state to registered. 
  This function is called by EMM when ever EPS only registration is successfull in LTE. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_update_ps_registration_success_status
(
  void
)
{  
  MSG_HIGH_DS(MM_SUB,"EMM registration success, Updating GMM state",0,0,0);
  gmm_set_state( GMM_REGISTERED );
  gmm_set_substate( GMM_NO_CELL_AVAILABLE );  
}

/*===========================================================================

FUNCTION      MM_UPDATE_PS_REGISTRATION_SUCCESS_STATUS

DESCRIPTION
  This updates the GMM state to registered. 
  This function is called by EMM when ever EPS only registration is successfull in LTE. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_update_abnormal_failure
(
  void
)
{  
  MSG_HIGH_DS(MM_SUB,"EMM registration success, Updating GMM state",0,0,0);
  gmm_set_state( GMM_REGISTERED );
  gmm_set_substate( GMM_NO_CELL_AVAILABLE );  
}

/*===========================================================================

FUNCTION  MM_UPDATE_CS_UPDATE_SUCCESS_STATUS

DESCRIPTION
  This function updates MM state and parameters when combined attach or 
  combined TAU is successfull.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_update_cs_update_success_status
(
  lte_nas_emm_incoming_msg_type  *msg_ptr
)
{  
  boolean                 lai_exists = FALSE;
  inter_task_lai_T       *lai_ptr = NULL;
  boolean                 ms_id_exists = FALSE;
  gmm_ie_LV_mobile_id_T  *ms_id_ptr = NULL;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  byte    null_lai[LAI_SIZE] = { 0xff, 0xff, 0xff, 0xff, 0xfe };
   
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
   
  ASSERT(emm_ctrl_data_ptr != NULL);


  ASSERT(msg_ptr != NULL);
  
  switch(msg_ptr->hdr.msg_id)
  {
    case ATTACH_ACCEPT:
      lai_exists = msg_ptr->attach_accept.loc_area_id_exists;
      lai_ptr = &msg_ptr->attach_accept.loc_area_id;
      ms_id_exists = msg_ptr->attach_accept.ms_id_exists;
      ms_id_ptr = &msg_ptr->attach_accept.ms_id;
      break;
    case TRACKING_AREA_UPDATE_ACCEPT:
      lai_exists = msg_ptr->tau_accept.loc_area_id_exists;
      lai_ptr = &msg_ptr->tau_accept.loc_area_id;
      ms_id_exists = msg_ptr->tau_accept.ms_id_exists;
      ms_id_ptr = &msg_ptr->tau_accept.ms_id;
      break;
    default:
      MSG_ERROR_DS(MM_SUB,"Invalid msg %u rcved to update mm status",msg_ptr->hdr.msg_id,0,0);
      return;
  }

  /* Reset LU attempt counter */
  mm_lu_attempt_counter = 0 ;

  mm_previous_idle_substate = MM_NULL_SUBSTATE;

  /* Set LU update status to UPDATED */
  mm_location_information.location_update_status = UPDATED;

  /* If LAI has been provided... */
  if(lai_exists == TRUE)
  {
    /* Extract LAI */
    memscpy((void *)mm_location_information.lai, LAI_SIZE, (void *)lai_ptr, LAI_SIZE);
    emm_ctrl_data_ptr->emm_cs_loci_info.lai_exists = TRUE;
    memscpy((void *)emm_ctrl_data_ptr->emm_cs_loci_info.lai, LAI_SIZE, (void *)lai_ptr, LAI_SIZE);

    /* Remove UPDATED LAI from the forbidden lists */
    mm_remove_forbidden_national_roaming(lai_ptr);
    mm_remove_forbidden_regional_roaming(lai_ptr);
#ifdef FEATURE_ENHANCED_NW_SELECTION
    if(reg_sim_read_ens_flag())
    {
      mm_remove_lai_from_foreground_search_lai_list(mm_location_information.lai);
    }
#endif
  }
  else
  {
    emm_ctrl_data_ptr->emm_cs_loci_info.lai_exists = FALSE;
    memscpy( (void *)emm_ctrl_data_ptr->emm_cs_loci_info.lai, LAI_SIZE, null_lai, LAI_SIZE );
  }

  /* If Attach Accept carrying MS ID process it. Otherwise keep using 
     earlier allocated TMSI */
  if(ms_id_exists == TRUE)
  {
    /* If the UE addressed by IMSI... */
    if(ms_id_ptr->id_type == (byte)LTE_NAS_EMM_IMSI)
    {
      /* Invalidate TMSI */
      memset((void *)mm_location_information.tmsi, 0xFF, TMSI_SIZE);
    }
    else if(ms_id_ptr->id_type == (byte)LTE_NAS_EMM_TMSI)
    {
      /* Start using new TMSI */
      memscpy((void *)mm_location_information.tmsi, TMSI_SIZE, 
             (void *)ms_id_ptr->value, TMSI_SIZE) ;
    }
  }

  /* Write LOCI back to USIM */
  mmsim_write_loci() ;

  /* Set MM state */
  mm_state = MM_IDLE;
  mm_idle_substate = MM_NO_CELL_AVAILABLE;

  mm_possible_imsi_attach = FALSE; 

  if(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI)
  {
    mm_pending_lu_request = LTE_IRAT_LU;     
  }
  else
  {
    mm_pending_lu_request = NO_LU;
  }
  mm_lu_start_reason = NO_LU;

  /* -----------------------------------------------------------
  ** Any timer used for triggering the location update procedure
  ** (e.g., T3211, T3212) shall be stopped if running.
  ** ----------------------------------------------------------- */
  mm_stop_timer( TIMER_T3211 );
  mm_stop_timer( TIMER_T3212 );
  mm_stop_timer( TIMER_T3213 );
  mm_idle_transition_timer = NULL;

  if(mm_csfb_state == MM_CSFB_STATE_LU_PENDING)
  {
    mm_csfb_set_state(MM_CSFB_STATE_NULL);
  }
}/* mm_update_cs_update_success_status */


/*===========================================================================

FUNCTION  MM_UPDATE_START_TAU_PROCEDURE

DESCRIPTION
  This function updates MM state and parameters when combined attach or 
  combined TAU is successfull.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_update_start_tau_procedure(lte_nas_emm_eps_update_val_type   ta_update_type)
{

  if(ta_update_type == COMBINED_TA_LA_UPDATING ||
       ta_update_type == COMBINED_TA_LA_UPDATING_IMSI_ATTACH)
  {  
    mm_gs_association_established = FALSE;   
    mm_pending_lu_request = LTE_IRAT_LU;
  }
  emm_set_lte_deact_isr_timer_expired(FALSE);
}


/*===========================================================================

FUNCTION      MM_UPDATE_ATTACH_REJECT_STATUS

DESCRIPTION
  This function will update MM/GMM when EPS or combined attach is rejected.   

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_update_attach_reject_status
(
  byte reject_cause
)
{
#ifdef FEATURE_LTE
   emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  sys_plmn_id_s_type  current_lte_plmn;
   emm_ctrl_data_ptr = emm_db_get_ctrl_data();
   ASSERT(emm_ctrl_data_ptr != NULL);
#endif
  MSG_HIGH_DS_1(MM_SUB,"=EMM= Attach rejected with cause %d, updating GMM/MM ",
           reject_cause);

  switch(reject_cause)
  {
    case ILLEGAL_MS:
    case ILLEGAL_ME:
    case GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED:       
      /*--------------------------------------------------
      **24.301 - If A/Gb mode or Iu mode is supported by the UE, the UE shall in addition handle
      ** the MM parameters update status, TMSI, LAI and ciphering key sequence number, 
      **and the GMM parameters GMM state, GPRS update status, P-TMSI, P-TMSI signature, 
      **RAI and GPRS ciphering key sequence number as specified in 3GPP TS 24.008 [13] for 
      **the case when the normal attach procedure is rejected with the GMM cause with 
      **the same value-------------------------------------------------*/ 
      gmm_delete_ps_location_information();
      gmm_delete_gprs_cksn();
      gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;
      mmsim_save_ps_location_information(gmm_stored_ptmsi,
                                         gmm_stored_ptmsi_signature,
                                         gmm_stored_rai,
                                         gmm_update_status);
      gmm_set_state(GMM_DEREGISTERED);
      if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
      {
        mm_delete_lu_status( ROAMING_NOT_ALLOWED );        
      }
#ifdef FEATURE_LTE                       
      current_lte_plmn = emm_convert_rrc_plmn_id_to_nas_plmn_id(
                                    emm_ctrl_data_ptr->last_rrc_service_ind_ptr->camped_svc_info.\
                                                                              selected_plmn);
      MSG_HIGH_DS_3(MM_SUB,"=EMM= Is Accepted On LTE %d Eps Storage Supported : %d emm_plmn_block_on_rej_15_8 : %d",emm_ctrl_data_ptr->is_accepted_on_lte,emm_ctrl_data_ptr->eps_storage_supported,emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_8);
      if((reject_cause==LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED) &&
         (emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 ==TRUE)&&
         (emm_ctrl_data_ptr->is_accepted_on_lte==FALSE) &&
         (emm_ctrl_data_ptr->eps_storage_supported == FALSE) &&
    #ifdef FEATURE_DUAL_SIM
    #ifdef FEATURE_EQUIVALENT_HPLMN
                    (reg_sim_per_subs_is_ehplmn(current_lte_plmn,(sys_modem_as_id_e_type)mm_sub_id) )
    #else
                    (sys_plmn_match(reg_sim_per_subs_read_hplmn((sys_modem_as_id_e_type)mm_sub_id),current_lte_plmn) )
    #endif
    #else
    #ifdef FEATURE_EQUIVALENT_HPLMN
                    (reg_sim_is_ehplmn(current_lte_plmn) )
    #else
                    (sys_plmn_match(reg_sim_read_hplmn(),current_lte_plmn) )
    #endif
    #endif
        )
      {
         gmm_update_mm_serving_plmn_info( SYS_SIM_STATE_AVAILABLE,
                                          SYS_SRV_DOMAIN_NO_SRV,
                                          TRUE,
                                          SYS_SRV_STATUS_LIMITED,
                                          TRUE );
      }
      else
#endif
      {
      /* USIM shall be considered as invalid also for non-EPS services until 
         switching off or the UICC is removed. */
      gmm_update_mm_serving_plmn_info( SYS_SIM_STATE_CS_PS_INVALID,
                                       SYS_SRV_DOMAIN_NO_SRV,
                                       TRUE,
                                       SYS_SRV_STATUS_LIMITED,
                                       TRUE );
      }
      break;
    case GPRS_SERVICES_NOT_ALLOWED:
       /* ----------------------------------
       **24.301 - If A/Gb mode or Iu mode is supported by the UE, the UE shall in addition handle the 
       **GMM parameters GMM state, GPRS update status, P-TMSI, P-TMSI signature, RAI, 
       **GPRS ciphering key sequence number  as specified in 3GPP TS 24.008 [13] for the case 
       **when the normal attach procedure is rejected with the GMM cause with the same value.
       ***-----------------------------------*/
      gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;   
      gmm_delete_ps_location_information();
      gmm_delete_gprs_cksn();
      mmsim_save_ps_location_information(gmm_stored_ptmsi,
                                         gmm_stored_ptmsi_signature,
                                         gmm_stored_rai,
                                         gmm_update_status ); 
      gmm_set_state( GMM_DEREGISTERED );              
      break;   
    case PLMN_NOT_ALLOWED:
      /* -----------------------------------------------------
     **24.301 -  If A/Gb mode or Iu mode is supported by the UE, the UE shall in addition 
     **handle the MM parameters update status, TMSI, LAI, ciphering key sequence 
     **number and location update attempt counter, and the GMM parameters GMM state, 
     **GPRS update status, P-TMSI, P-TMSI signature, RAI, GPRS ciphering key sequence
     **number and GPRS attach attempt counter as specified in 3GPP TS 24.008 [13] for 
     **the case when the normal attach procedure is rejected with the GMM cause with 
     **the same value and no RR connection exists.
     **-------------------------------------------------*/ 
      gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;   
      gmm_delete_ps_location_information();
      gmm_delete_gprs_cksn();
      mmsim_save_ps_location_information(gmm_stored_ptmsi,
                                         gmm_stored_ptmsi_signature,
                                         gmm_stored_rai,
                                         gmm_update_status );       
      gprs_attach_attempt_counter = 0;
      MSG_HIGH_DS(MM_SUB,"GPRS Attempt counter = %d", gprs_attach_attempt_counter, 0,0 );
      gmm_set_state( GMM_DEREGISTERED );
      if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
      {    
        mm_delete_lu_status( ROAMING_NOT_ALLOWED );
        mm_clear_lu_attempt_counter( );   
        mm_previous_idle_substate = NULL;
      }
      break;    

    case LA_NOT_ALLOWED:
    case NATIONAL_ROAMING_NOT_ALLOWED:
    case NO_SUITABLE_CELLS_IN_LA:
       /* ----------------------------------
       **24.301 - If A/Gb mode or Iu mode is supported by the UE, the UE shall in addition handle the 
       **GMM parameters GMM state, GPRS update status, P-TMSI, P-TMSI signature, RAI, 
       **GPRS ciphering key sequence number and GPRS attach attempt counter as specified 
       **in 3GPP TS 24.008 [13] for the case when the normal attach procedure is rejected with the GMM 
       **cause with the same value.
       ***-----------------------------------*/
      gmm_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;   
      gmm_delete_ps_location_information();
      gmm_delete_gprs_cksn();
      mmsim_save_ps_location_information(gmm_stored_ptmsi,
                                         gmm_stored_ptmsi_signature,
                                         gmm_stored_rai,
                                         gmm_update_status);       
      gprs_attach_attempt_counter = 0;
      MSG_HIGH_DS(MM_SUB,"GPRS Attempt counter = %d", gprs_attach_attempt_counter, 0,0 );
      if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
      {    
        mm_delete_lu_status( ROAMING_NOT_ALLOWED );
        mm_clear_lu_attempt_counter();   
        mm_previous_idle_substate = NULL;
      }
      gmm_set_state( GMM_DEREGISTERED );              
      break;

    case GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN:
       /* ----------------------------------
       **24.301 - If A/Gb mode or Iu mode is supported by the UE, the UE shall in addition handle the 
       **GMM parameters GMM state, GPRS update status, P-TMSI, P-TMSI signature, RAI, 
       **GPRS ciphering key sequence number and GPRS attach attempt counter as specified 
       **in 3GPP TS 24.008 [13] for the case when the normal attach procedure is rejected with the GMM 
       **cause with the same value.
       ***-----------------------------------*/
      gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;   
      gmm_delete_ps_location_information();
      gmm_delete_gprs_cksn();
      mmsim_save_ps_location_information(gmm_stored_ptmsi,
                                         gmm_stored_ptmsi_signature,
                                         gmm_stored_rai,
                                         gmm_update_status);       
      gprs_attach_attempt_counter = 0;
      MSG_HIGH_DS(MM_SUB,"GPRS Attempt counter = %d", gprs_attach_attempt_counter, 0,0 );
      gmm_set_state( GMM_DEREGISTERED );              
      break;

#ifdef FEATURE_FEMTO_CSG
    case CSG_NOT_AUTHORIZED:
      gmm_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;
      mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                          gmm_stored_ptmsi_signature,
                                          gmm_stored_rai,
                                          gmm_update_status );
  
      /* -------------------------------------
      ** Reset the GPRS attach attempt counter
      ** ------------------------------------- */
      gprs_attach_attempt_counter = 0;
      MSG_HIGH_DS( MM_SUB, "=MM= GPRS Attach Attempt counter = %d",
                gprs_attach_attempt_counter, 0,0 );

      break;
#endif 
     case LTE_NAS_SEVERE_NW_FAILURE:
       gmm_update_status = GMM_GU2_NOT_UPDATED;   
       gmm_delete_ps_location_information();
       gmm_delete_gprs_cksn();
       mmsim_save_ps_location_information(gmm_stored_ptmsi,
                                          gmm_stored_ptmsi_signature,
                                          gmm_stored_rai,
                                          gmm_update_status ); 
       gmm_set_state( GMM_DEREGISTERED ); 
       break;	   
    default:
      break;
  }
} /* mm_update_attach_reject_status */

/*===========================================================================

FUNCTION      MM_UPDATE_TAU_SR_REJECT_STATUS

DESCRIPTION
   This function will update MM/GMM when TAU or Service Request gets reject 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_update_tau_sr_reject_status
(
  lte_nas_message_id  msg_id, 
  byte                reject_cause
)
{
  MSG_HIGH_DS_1(MM_SUB,"EMM SR/TAU reject with cause %d, Updating MM ",reject_cause);
  switch(reject_cause)
  {
    case ILLEGAL_MS:
    case ILLEGAL_ME:
    case GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED:       
      /*--------------------------------------------------
      **24.301 - If A/Gb mode or Iu mode is supported by the UE, the UE shall in addition handle
      ** the MM parameters update status, TMSI, LAI and ciphering key sequence number, 
      **and the GMM parameters GMM state, GPRS update status, P-TMSI, P-TMSI signature, 
      **RAI and GPRS ciphering key sequence number as specified in 3GPP TS 24.008 [13] for 
      **the case when the normal attach procedure is rejected with the GMM cause with 
      **the same value-------------------------------------------------*/ 
      gmm_delete_ps_location_information();
      gmm_delete_gprs_cksn();
      gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;
      mmsim_save_ps_location_information(gmm_stored_ptmsi,
                                         gmm_stored_ptmsi_signature,
                                         gmm_stored_rai,
                                         gmm_update_status );

      gmm_set_state( GMM_DEREGISTERED );
       
      if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
      {
        mm_delete_lu_status( ROAMING_NOT_ALLOWED );        
      }
      break;

    case GPRS_SERVICES_NOT_ALLOWED:
       /* ----------------------------------
       **24.301 - If A/Gb mode or Iu mode is supported by the UE, the UE shall in addition handle the 
       **GMM parameters GMM state, GPRS update status, P-TMSI, P-TMSI signature, RAI, 
       **GPRS ciphering key sequence number  as specified in 3GPP TS 24.008 [13] for the case 
       **when the normal attach procedure is rejected with the GMM cause with the same value.
       ***-----------------------------------*/
      gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;   
      
      gmm_delete_ps_location_information();
      gmm_delete_gprs_cksn();
       
      mmsim_save_ps_location_information(gmm_stored_ptmsi,
                                         gmm_stored_ptmsi_signature,
                                         gmm_stored_rai,
                                         gmm_update_status );       
       
      gmm_set_state( GMM_DEREGISTERED );              
      if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
      {
        mm_delete_lu_status( NOT_UPDATED );        
      }
      break; 

    case MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK:  
     /*-----------------------------------------------------
     **24.301 - If A/Gb mode or Iu mode is supported by the UE, the UE shall handle the 
     **GMM parameters GMM state, GPRS update status, P-TMSI, P-TMSI signature, RAI 
     **and GPRS ciphering key sequence number as specified in 3GPP TS 24.008 [13] for the 
     **case when the normal routing area updating procedure is rejected with the GMM cause 
     ** with the same value. ----------------------------------*/
  
      gmm_update_status = GMM_GU2_NOT_UPDATED;
      gmm_delete_ps_location_information();
      gmm_delete_gprs_cksn();
      mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                          gmm_stored_ptmsi_signature,
                                          gmm_stored_rai,
                                          gmm_update_status );

      gmm_set_state( GMM_DEREGISTERED );
      if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
      {
        mm_delete_lu_status( NOT_UPDATED );        
      }
      break; 
    
    case IMPLICITLY_DETACHED:
      /** -----------------------------------------------
      ** If A/Gb mode or Iu mode is supported by the UE, the UE shall handle the GMM 
      **state as specified in 3GPP TS 24.008 [13] for the case when the normal routing 
      **area updating procedure is rejected with the GMM cause with the same value.
      **----------------------------------------------*/
      gmm_set_state( GMM_DEREGISTERED ); 
      if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
      {
        mm_delete_lu_status( NOT_UPDATED );        
      }
      break;

    case PLMN_NOT_ALLOWED:
      /* -----------------------------------------------------
     **24.301 - If A/Gb mode or Iu mode is supported by the UE, the UE shall handle
     ** the GMM parameters GMM state, GPRS update status, P-TMSI, P-TMSI signature, 
     ** RAI, GPRS ciphering key sequence number and routing area updating attempt 
     ** counter and the MM parameters update status, TMSI, LAI, ciphering key sequence
     ** number and the location update attempt counter as specified in 3GPP TS 24.008 [13]
     ** for the case when the normal routing area updating procedure is rejected with the
     ** GMM cause with the same value and no RR connection exists
     **-------------------------------------------------*/ 
      gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;   
      
      gmm_delete_ps_location_information();
      gmm_delete_gprs_cksn();
       
      mmsim_save_ps_location_information(gmm_stored_ptmsi,
                                         gmm_stored_ptmsi_signature,
                                         gmm_stored_rai,
                                         gmm_update_status );       
       
      gmm_set_state( GMM_DEREGISTERED );
 
      gmm_rau_attempt_counter = 0;
      MSG_HIGH_DS(MM_SUB, "RAU Attempt counter = %d", gmm_rau_attempt_counter, 0,0 );
      t3330_timeout_counter   = 0;
      MSG_HIGH_DS(MM_SUB, "T3330 timeout counter = %d", t3330_timeout_counter, 0,0 );

      if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
      {    
        mm_delete_lu_status( ROAMING_NOT_ALLOWED );
        mm_clear_lu_attempt_counter( );   
        mm_previous_idle_substate = NULL;
      }
      break;
    
    case LA_NOT_ALLOWED:
         /* ----------------------------------------------------
      ** 24-301 - If A/Gb mode or Iu mode is supported by the UE, the UE shall handle the GMM 
      **parameters GMM state, GPRS update status, P-TMSI, P-TMSI signature, RAI, GPRS 
      **ciphering key sequence number and routing area updating attempt counter as specified
      **in 3GPP TS 24.008 [13] for the case when the normal routing area updating procedure is rejected 
      **with the GMM cause with the same value.
       ***-----------------------------------*/
      gmm_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;   
      
      gmm_delete_ps_location_information();
      gmm_delete_gprs_cksn();
       
      mmsim_save_ps_location_information(gmm_stored_ptmsi,
                                         gmm_stored_ptmsi_signature,
                                         gmm_stored_rai,
                                         gmm_update_status );       
      gmm_rau_attempt_counter = 0;
      if((msg_id == TRACKING_AREA_REJECT) &&         
         (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS))
      {    
        mm_delete_lu_status( ROAMING_NOT_ALLOWED );
        mm_clear_lu_attempt_counter( );   
        mm_previous_idle_substate = NULL;
      }
      MSG_HIGH_DS(MM_SUB, "RAU Attempt counter = %d", gmm_rau_attempt_counter, 0,0 );
      t3330_timeout_counter   = 0;
      MSG_HIGH_DS(MM_SUB, "T3330 timeout counter = %d", t3330_timeout_counter, 0,0 );
       
      gmm_set_state( GMM_DEREGISTERED );              
      break;   

    case GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN:

       /* ----------------------------------------------------
      ** 24-301 - If A/Gb mode or Iu mode is supported by the UE, the UE shall handle the GMM 
      **parameters GMM state, GPRS update status, P-TMSI, P-TMSI signature, RAI, GPRS 
      **ciphering key sequence number and routing area updating attempt counter as specified
      **in 3GPP TS 24.008 [13] for the case when the normal routing area updating procedure is rejected 
      **with the GMM cause with the same value.
       ***-----------------------------------*/
      gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;   
      
      gmm_delete_ps_location_information();
      gmm_delete_gprs_cksn();
       
      mmsim_save_ps_location_information(gmm_stored_ptmsi,
                                         gmm_stored_ptmsi_signature,
                                         gmm_stored_rai,
                                         gmm_update_status );       
      gmm_rau_attempt_counter = 0;
      if((msg_id == TRACKING_AREA_REJECT) && 
         (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS))
      {    
        mm_delete_lu_status( NOT_UPDATED );        
      }
      MSG_HIGH_DS(MM_SUB, "RAU Attempt counter = %d", gmm_rau_attempt_counter, 0,0 );
      t3330_timeout_counter   = 0;
      MSG_HIGH_DS(MM_SUB, "T3330 timeout counter = %d", t3330_timeout_counter, 0,0 );
       
      gmm_set_state( GMM_DEREGISTERED );              
      break;   


    case NATIONAL_ROAMING_NOT_ALLOWED:
    case NO_SUITABLE_CELLS_IN_LA:
#ifdef FEATURE_FEMTO_CSG
    case CSG_NOT_AUTHORIZED:
#endif 
      /* ----------------------------------
      **24.301 - If A/Gb mode or Iu mode is supported by the UE, the UE shall handle
     ** the GMM parameters GMM state, GPRS update status and routing area updating 
     ** attempt counter as specified in 3GPP TS 24.008 [13] for the case when the normal 
     **routing area updating procedure is rejected with the GMM cause with the same value.
      ***-----------------------------------*/
      gmm_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;       

      gmm_rau_attempt_counter = 0;
      MSG_HIGH_DS(MM_SUB, "RAU Attempt counter = %d", gmm_rau_attempt_counter, 0,0 );
      t3330_timeout_counter   = 0;
      MSG_HIGH_DS(MM_SUB, "T3330 timeout counter = %d", t3330_timeout_counter, 0,0 );
      
      gmm_set_state( GMM_REGISTERED );
      if((msg_id == TRACKING_AREA_REJECT) &&
         (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS))
      {    
        /* ---------------------------------------------------------
        ** If the MS is IMSI attached, the MS shall in addition set
        ** the update status to U3 ROAMING NOT ALLOWED and shall
        ** reset the location update attempt counter.
        ** --------------------------------------------------------- */
        mm_location_information.location_update_status = ROAMING_NOT_ALLOWED;
  
        /* --------------------------------
        ** Copy location information to SIM
        ** -------------------------------- */
        mmsim_write_loci();
  
        mm_clear_lu_attempt_counter( );
        mm_previous_idle_substate = NULL;
      }
      break;
    case NO_PDP_CONTEXT_ACTIVATED:
      gmm_set_state(GMM_DEREGISTERED); 
      break;
    case LTE_NAS_SEVERE_NW_FAILURE:       
      /*24.301 -cause #42 
      If A/Gb mode or Iu mode is supported by the UE, 
      the UE shall in addition set the GMM state to GMM-DEREGISTERED, 
      GPRS update status to GU2 NOT UPDATED, and shall delete the P-TMSI, P-TMSI signature,                                                     
      RAI and GPRS ciphering key sequence number.*/ 
      gmm_delete_ps_location_information();
      gmm_delete_gprs_cksn();
      gmm_update_status = GMM_GU2_NOT_UPDATED;
      mmsim_save_ps_location_information(gmm_stored_ptmsi,
                                         gmm_stored_ptmsi_signature,
                                         gmm_stored_rai,
                                         gmm_update_status );
      gmm_set_state( GMM_DEREGISTERED );
      break;
    case CONGESTTION:
      /* ----------------------------------
      **24.301 - If A/Gb mode or Iu mode is supported by the UE, the UE shall handle
     ** the GMM parameters GMM state, GPRS update status and routing area updating 
     ** attempt counter as specified in 3GPP TS 24.008 [13] for the case when the normal 
     **routing area updating procedure is rejected with the GMM cause with the same value.
      ***-----------------------------------*/
      if(msg_id == TRACKING_AREA_REJECT)
      {
        gmm_update_status = GMM_GU2_NOT_UPDATED;
        mmsim_save_ps_location_information(gmm_stored_ptmsi,
                                           gmm_stored_ptmsi_signature,
                                           gmm_stored_rai,
                                           gmm_update_status );
        gmm_rau_attempt_counter = 0;
        MSG_HIGH_DS_1(MM_SUB, "RAU Attempt counter = %d", gmm_rau_attempt_counter);
      }
      break;
    default:
      break;

  }
}
/*===========================================================================

FUNCTION  emm_get_mmr_attach_failed_ind_status

DESCRIPTION
  This function returns attach failed ind status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
mm_attach_failed_status_type emm_get_mmr_attach_failed_ind_status
(
  void
)
{
  return mm_mmr_attach_ind_status;
}
/*===========================================================================

FUNCTION  emm_send_reg_attach_failed_ind

DESCRIPTION
  This function processes MMR_ATTACH_FAILED_IND 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void emm_send_reg_attach_failed_ind                             
(
  void
)
{
  if (emm_get_mmr_attach_failed_ind_status() !=  ATTACH_FAILED_IND_NOT_REQUIRED )
  {
    mmr_attach_failed_ind_s_type                     attached_failed_ind;
    log_lte_nas_emm_outgoing_msg_event_type   event;

    attached_failed_ind.message_header.message_set = MS_MM_REG;
    attached_failed_ind.message_header.message_id  = (int)MMR_ATTACH_FAILED_IND;

    PUT_IMH_LEN( sizeof( mmr_camped_ind_s_type ) - sizeof(IMH_T), &attached_failed_ind.message_header );

    MSG_HIGH_DS(MM_SUB, "=EMM= MM sent MMR_ATTACH_FAILED_IND", 0,0,0);

    mm_send_message( (IMH_T*)&attached_failed_ind, GS_QUEUE_REG );
    event.emm_out_msg = (byte)MMR_ATTACH_FAILED_IND;
    event_report_payload(EVENT_LTE_REG_OUTGOING_MSG,sizeof(event), (void*)&event);
    mm_mmr_attach_ind_status = ATTACH_FAILED_IND_NOT_REQUIRED;
  }
} /* end of emm_send_reg_attach_failed_ind() */
/*===========================================================================

FUNCTION      emm_store_attach_ind_status_type

DESCRIPTION
Set the attach failed ind flag

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_store_attach_ind_status_type(mmr_reg_req_s_type        *reg_req_ptr)
{
   if(((reg_req_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL ) ||
        (reg_req_ptr->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC )) &&
        !(( mm_serving_plmn.info.service_status == SYS_SRV_STATUS_SRV) &&
          ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) || 
          (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY))) &&
      ((reg_req_ptr->req_service_domain == SYS_SRV_DOMAIN_CS_PS )|| 
       (reg_req_ptr->req_service_domain == SYS_SRV_DOMAIN_PS_ONLY)))
   {
     mm_mmr_attach_ind_status          = ATTACH_FAILED_IND_REQUIRED;
   }
   else
   {
     mm_mmr_attach_ind_status          = ATTACH_FAILED_IND_NOT_REQUIRED;
   }
    MSG_HIGH_DS(MM_SUB, "=EMM= Setting the ATTACH FAILED IND flag Value = %d ( ATTACH_FAILED_IND_REQUIRED = 0 && ATTACH_FAILED_IND_NOT_REQUIRED = 2)",mm_mmr_attach_ind_status,0,0);
}
/*===========================================================================

FUNCTION      emm_reset_attach_ind_status_type

DESCRIPTION
Set the attach failed ind flag

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_reset_attach_ind_status_type(void)
{

   mm_mmr_attach_ind_status = ATTACH_FAILED_IND_NOT_REQUIRED;
   MSG_HIGH_DS(MM_SUB, "=EMM= resetting the ATTACH FAILED IND flag Value",0,0,0);
}


/*===========================================================================

FUNCTION      mm_emm_store_reg_req

DESCRIPTION
  Store MMR_REG_REQ for MM and EMM separately for now. 
  Need to figure out how to combine them into one later.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_emm_store_reg_req(mmr_reg_req_s_type* reg_req_ptr,   emm_ctrl_data_type     *emm_ctrl_data_ptr)
{
  ASSERT(emm_ctrl_data_ptr != NULL);

  ASSERT(reg_req_ptr != NULL);
  if(emm_ctrl_data_ptr->reg_req_pending_ptr == NULL)
  {
    MSG_ERROR_DS(MM_SUB,"=EMM= Wrong sub this should not happen, return from function mm_emm_store_reg_req", 0,0,0 );
    return;
  }

  /* Save the reg req for EMM after check of nw selection mode change */
  memscpy((void *)emm_ctrl_data_ptr->reg_req_pending_ptr, sizeof(mmr_reg_req_s_type),
          (void *)reg_req_ptr, sizeof(mmr_reg_req_s_type));
  /* Save the reg_req for MM*/
  memscpy( (void *)&mmcoord_pending_reg_message.cmd.mmr_reg_req, sizeof(mmr_reg_req_s_type),(void *)reg_req_ptr,sizeof(mmr_reg_req_s_type)) ;
}

/*===========================================================================

FUNCTION      emm_set_possible_imsi_attach

DESCRIPTION
store the imsi attach for mm to decide the regsitrattion type

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_set_possible_imsi_attach(boolean imsi_attach_required)
{
  mm_possible_imsi_attach = imsi_attach_required;
}
/*===========================================================================

FUNCTION      mm_emm_store_reg_req_nw_selection_mode

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_emm_store_reg_req_nw_selection_mode(sys_network_selection_mode_e_type   network_selection_mode)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);

  /* Save the reg_req network sel for MM and EMM*/
  mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode = network_selection_mode;
  if(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL)
  {
    emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode = network_selection_mode;
  }
}

/*===========================================================================

FUNCTION     mm_map_umts_identity_to_lte 

DESCRIPTION
Functions maps UMTS identities to LTE identities. RAI & PTMSI into GUTI and P-TMSi signature into old P-TMSi signature

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_map_umts_identity_to_lte
(
  lte_nas_emm_eps_mobile_id_type  *guti,
  byte                            *ptmsi_sig,
  boolean         *ptmsi_sig_present
)
{
  byte i;
  ASSERT(guti != (void *)NULL);
  ASSERT(ptmsi_sig != (void *)NULL);
  
  memset(guti, 0, sizeof(lte_nas_emm_eps_mobile_id_type));

  /* Populate GUTI from P-TMSI. As per spec 23.003, section 2.8.2 and 2.8.2.2*/
  /* Populate from M-TMSI*/

  guti->ue_id = GUTI_ID;
  guti->ue_id_info.guti.ue_id = GUTI_ID;

  mm_map_ptmsi_rai_to_gumme_info(&guti->ue_id_info.guti);
 
  guti->ue_id_info.guti.m_tmsi[0] = gmm_stored_ptmsi.digit[3];
  guti->ue_id_info.guti.m_tmsi[1] = gmm_stored_ptmsi.digit[2];
  guti->ue_id_info.guti.m_tmsi[2] = gmm_stored_rai.routing_area_code;
  guti->ue_id_info.guti.m_tmsi[3] = gmm_stored_ptmsi.digit[0] & 0x3f;  
   
  /* Set bits 30 and 31 to 1*/  
  guti->ue_id_info.guti.m_tmsi[3] = guti->ue_id_info.guti.m_tmsi[3] | 0xc0;

  *ptmsi_sig_present = FALSE;
   
  for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
  {
    if (gmm_stored_ptmsi_signature.value[i] != 0xFF)
    {
     *ptmsi_sig_present = TRUE;
    }
    ptmsi_sig[i] = gmm_stored_ptmsi_signature.value[i];
  }
}


/*===========================================================================

FUNCTION      emm_populate_guti

DESCRIPTION
             Populate GUTI from emm database
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void emm_populate_guti(lte_nas_emm_eps_mobile_id_type* guti, emm_ctrl_data_type     *emm_ctrl_data_ptr)
{
  ASSERT(guti != NULL );
  ASSERT(emm_ctrl_data_ptr != NULL );

  guti->ue_id = GUTI_ID ;
  guti->ue_id_info.guti = emm_ctrl_data_ptr->emm_guti;
}

/*===========================================================================

FUNCTION      emm_is_ptmsi_valid

DESCRIPTION
           P-TMSI validity check
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_is_ptmsi_valid(void)
{
  byte index = 0;

  for (index=0; index < PTMSI_SIZE; index++)
  {
    if (gmm_stored_ptmsi.digit[index] != 0xFF)
    {
      return TRUE;
    }
  } 
  return FALSE;
}

/*===========================================================================

FUNCTION      emm_populate_imsi

DESCRIPTION
            Poulates imsi from emm database
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void emm_populate_imsi(lte_nas_emm_eps_mobile_id_type* imsi, emm_ctrl_data_type     *emm_ctrl_data_ptr)
{

  ASSERT(emm_ctrl_data_ptr != NULL);

  ASSERT(imsi != NULL );

  emm_ctrl_data_ptr->imsi_attach = TRUE;

  imsi->ue_id = IMSI_ID ;
  imsi->ue_id_info.imsi.num_id_digits_len = 
                                            emm_ctrl_data_ptr->imsi.num_digits;
  if(emm_ctrl_data_ptr->imsi.num_digits <= MAX_NO_OF_IMSI)
  {
    memscpy(imsi->ue_id_info.imsi.num_id_digits, MAX_NO_OF_IMSI ,
           emm_ctrl_data_ptr->imsi.imsi_digits, emm_ctrl_data_ptr->imsi.num_digits) ;
  }
  else
  {
    MSG_ERROR_DS(MM_SUB,"=EMM= num_digits %d exceeds imsi length, concatinating to %d",
                      emm_ctrl_data_ptr->imsi.num_digits,MAX_NO_OF_IMSI,0);
    memscpy(imsi->ue_id_info.imsi.num_id_digits, MAX_NO_OF_IMSI,
                      emm_ctrl_data_ptr->imsi.imsi_digits,MAX_NO_OF_IMSI) ;
  }
}

/*===========================================================================

FUNCTION  GMM_MULTIMODE_SET_STATE

DESCRIPTION 
  This function sets given GMM state if GSM or UMTS is available
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_multimode_set_state
(
  gmm_state_type     gmm_state_arg, 
  gmm_substate_type  gmm_sub_state
)
{
  if (is_lte_mode() ) 
  { 
    if((gmm_state_arg == GMM_DEREGISTERED)&& 
       (gmm_state !=  GMM_DEREGISTERED) &&
       (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS))
    {
      mm_possible_imsi_attach = TRUE;
      mm_pending_lu_request = NO_LU;
    }
    gmm_set_state( gmm_state_arg );
    gmm_set_substate( gmm_sub_state );  
  }
}/* gmm_multimode_set_state */

/*===========================================================================

FUNCTION MMMULTIMODE_ROUTE_GW2L_IRAT_MM_EMM_MESSAGES

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  TRUE

SIDE EFFECTS
  None

===========================================================================*/
boolean  mmmultimode_route_GW2L_irat_mm_emm_messages
(
  void                   *command_ptr,
  emm_ctrl_data_type     *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp   **mm_emm_dispatcher_fn_ptr
)
{
   mm_cmd_type* mm_cmd_ptr = NULL;
   mm_cmd_ptr = (mm_cmd_type*)command_ptr;
   ASSERT(emm_ctrl_data_ptr != NULL);
   switch(mm_cmd_ptr->cmd.hdr.message_set)
   {
     case MS_MM_RRC:
     case MS_MM_TDSRRC:
     case MS_MM_RR:
       if(((mm_cmd_ptr->cmd.hdr.message_id == RRC_STOP_WCDMA_MODE_CNF) &&
            ((mm_cmd_ptr->cmd.hdr.message_set == MS_MM_RRC)|| (mm_cmd_ptr->cmd.hdr.message_set == MS_MM_TDSRRC))) ||
           ((mm_cmd_ptr->cmd.hdr.message_id == RR_STOP_GSM_MODE_CNF) &&
            (mm_cmd_ptr->cmd.hdr.message_set == MS_MM_RR)))
       {
         mmcoord_route_mm_message(mm_cmd_ptr,mm_emm_dispatcher_fn_ptr);
         /* Write HFS keys to SIM. This is to cover the scenario where NAS receives
            activation indication directly without connection release while in connected state.*/
#ifdef FEATURE_INTERRAT_HANDOVER_WTOG
         if(((mm_cmd_ptr->cmd.hdr.message_id == (byte)RRC_STOP_WCDMA_MODE_CNF) &&
            (mm_cmd_ptr->cmd.hdr.message_set == MS_MM_RRC)) &&
           (mm_sim_card_mode == MMGSDI_APP_USIM))
         {
           /* -----------------------------------------------------------
           ** Write the received CS & PS Hyper Frame Numbers  to the SIM
           ** ----------------------------------------------------------- */
           auth_handle_hfn_release_ind( mm_cmd_ptr->cmd.rrc_stop_wcdma_mode_cnf.start_cs,
                                       mm_cmd_ptr->cmd.rrc_stop_wcdma_mode_cnf.start_ps );
         }
#endif
         mm_substate_control( MM_STD_NO_SERVICE );
         gmm_set_substate( GMM_NO_CELL_AVAILABLE );
         mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE; 
         mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE; 

         /*Clear MM serving LAI */
         mm_serving_lai[0] = 0xFF;
         mm_serving_lai[1] = 0xFF;
         mm_serving_lai[2] = 0xFF;
         mm_serving_lai[3] = 0xFF;
         mm_serving_lai[4] = 0xFE;

/* MNK ESM ISR begin*/
#if 1
         reg_service_report = (emm_reg_service_report_type)emm_send_reg_ind;

         mm_send_cm_bearer_ctxt_transfer_ind(mm_serving_plmn.info.active_rat);
         emm_send_rrc_act_rsp(TRUE,emm_ctrl_data_ptr);

         /* Get HPLMN/EHPLMN list in case UE powered up on GW and comes to LTE
            for the first time  */
#if defined FEATURE_DUAL_SIM|| defined FEATURE_SGLTE
         emm_ctrl_data_ptr->emm_home_plmn = reg_sim_per_subs_hplmn_get((sys_modem_as_id_e_type)mm_as_id);
#else
         emm_ctrl_data_ptr->emm_home_plmn = reg_sim_hplmn_get();
#endif
         /* Change back to main routine handler. To receive Service_indication on GW*/
         /* MNK ESM ISR begin*/
         *mm_emm_dispatcher_fn_ptr = mm_emm_normal_msg_handler;
         /* MNK ESM ISR END*/
         MSG_HIGH_DS(MM_SUB,"=EMM= Change the main routine handler to  mm_emm_normal_msg_handler", 0,0,0 );
#ifdef FEATURE_SGLTE
         if(MM_SUB_IS_SGLTE)
         {
           MSG_HIGH_DS(MM_SUB,"=EMM= mm_irat_reg_cmd_pending = %d mm_sglte_current_stack_status = %d pending msgid = %d  ", 
         mm_irat_reg_cmd_pending,mm_sglte_current_stack_status ,mm_irat_pending_reg_cmd.cmd.hdr.message_id );
         }
#endif
 #ifdef FEATURE_SGLTE 
         if (MM_IS_IN_SGLTE_STATE_4 &&
                (mm_sglte_current_stack_status  == SGLTE_MM_LOWER_LAYER_SPLITING))
         {
            mmcoord_process_spliting();  
            mmsend_mmr_service_stat_ind(mm_cs_stack_id);
            mm_sglte_current_stack_status = SGLTE_INACTIVE_STATE;
         }  
#endif  
         if (mm_irat_reg_cmd_pending)
         {
           mm_irat_reg_cmd_pending = FALSE;
           (void)mmcoord_route_mm_emm_messages(&mm_irat_pending_reg_cmd,
                                         emm_ctrl_data_ptr,mm_emm_dispatcher_fn_ptr);
                                         
         }
         //Processing pended stop mode on other stack if any
#ifdef FEATURE_SGLTE          
         if(MM_IS_IN_SGLTE_MODE && 
            (mm_cmd_ptr->cmd.hdr.message_id == RR_STOP_GSM_MODE_CNF))
         {         

           mm_as_id = mm_cs_stack_id;
           if (mm_pended_stop_mode_req.valid)
           {
              mmcoord_pending_reg_message.cmd.mmr_stop_mode_req= mm_pended_stop_mode_req.mmr_stop_mode_req;
              mmcoord_route_mm_message(&mmcoord_pending_reg_message,mm_emm_dispatcher_fn_ptr); 
           }
            mm_as_id = mm_ps_stack_id;
         }
#endif    
#endif
/* MNK ESM ISR END*/
       }
       else if(mm_cmd_ptr->cmd.hdr.message_id == RR_GMM_GPRS_MULTISLOT_CLASS_IND)
       {
         mmcoord_route_mm_message(mm_cmd_ptr,mm_emm_dispatcher_fn_ptr);
       }
       else
       {
         MSG_HIGH_DS(MM_SUB,"=EMM= MM: NAS received msg %d during INTER_RAT, this message shall be currently discarded",
                 mm_cmd_ptr->cmd.hdr.message_id,0,0);
       }
       break;

     case MS_MM_REG:
       switch(mm_cmd_ptr->cmd.hdr.message_id)
       {
         case MMR_REG_REQ:
         case MMR_STOP_MODE_REQ:
         case MMR_PS_DETACH_REQ:
           mm_stop_timer(TIMER_POLICY_CHANGE_PENDING);
           mm_irat_reg_cmd_pending = TRUE;          
          (void)memscpy( &mm_irat_pending_reg_cmd, sizeof(mm_cmd_type), command_ptr, sizeof(mm_cmd_type));          
           MSG_HIGH_DS_1(MM_SUB,"=EMM= MM: NAS received msg %d during INTER_RAT, this message is pended until we receive STOP_MODE_CNF",
                   mm_cmd_ptr->cmd.hdr.message_id);
           break;


         case MMR_PLMN_SEARCH_REQ:
           mm_irat_reg_cmd_pending = TRUE;          
          (void)memscpy( &mm_irat_pending_reg_cmd, sizeof(mm_cmd_type), command_ptr, sizeof(mm_cmd_type));          
           MSG_HIGH_DS(MM_SUB,"=EMM= MM: NAS received msg %d during INTER_RAT, this message is pended until we receive STOP_MODE_CNF",
                   mm_cmd_ptr->cmd.hdr.message_id,0,0);
           break;

         case MMR_SET_DRX_REQ:
            MSG_HIGH_DS(MM_SUB,"=MM= MM received in GW2L inter-rat, return failure", 0,0,0);
            mm_send_mmr_set_drx_cnf(FALSE);
            break;

         case MMR_GET_DRX_REQ:
            mm_send_mmr_get_drx_cnf();
            break;

#ifdef FEATURE_FEMTO_CSG
        case MMR_FILE_REFRESH_IND:
            mmcoord_route_mm_message(mm_cmd_ptr,mm_emm_dispatcher_fn_ptr); 
            break;
#endif 

        case MMR_SIM_STATE_UPDATE_IND:
          mm_update_sim_state(SYS_SIM_STATE_AVAILABLE);
          break;

         case MMR_BLOCK_PLMN_REQ:
           emm_process_reg_command(mm_cmd_ptr, emm_ctrl_data_ptr );   
           break;
		   
        case MMR_CLEAR_LAI_REJECT_LIST_REQ:
          emm_process_reg_command(mm_cmd_ptr, emm_ctrl_data_ptr);
          mmcoord_route_mm_message(mm_cmd_ptr,mm_emm_dispatcher_fn_ptr);
          break;

#ifdef FEATURE_DUAL_SIM
        case MMR_DS_STAT_CHGD_REQ:
        case MMR_DS_TUNEAWAY_STATUS_CHANGE_IND:
        case MMR_DDS_SWITCH_IND:
          mmcoord_route_mm_message(mm_cmd_ptr,mm_emm_dispatcher_fn_ptr);
          break;
#endif

#ifdef FEATURE_FEMTO_CSG
         case MMR_CSG_SELECT_CONFIG_UPDATE_IND:
          mmcoord_route_mm_message(mm_cmd_ptr,mm_emm_dispatcher_fn_ptr); 
          break;
#endif
        case MMR_PSM_READY_REQ:
#ifdef FEATURE_LTE      
          if(is_lte_mode())
          {
            emm_process_reg_command(mm_cmd_ptr,emm_ctrl_data_ptr);
          }
          else
#endif
          {
            mmcoord_route_mm_message(mm_cmd_ptr,mm_emm_dispatcher_fn_ptr);
          }
          break;

        default:
           MSG_ERROR_DS(MM_SUB,"Unexpected REG message ID %d",mm_cmd_ptr->cmd.hdr.message_id,0,0);
           break;
       }
       break;


     case MS_GMM_SM:
       switch(mm_cmd_ptr->cmd.hdr.message_id)
       {
         case GMMSM_ESTABLISH_REQ:
           MSG_HIGH_DS(MM_SUB,"Ignoring GMMSM_ESTABLISH_REQ in GW2L I-RAT handler",0,0,0);
           break;

         case GMMSM_UNITDATA_REQ:
           MSG_HIGH_DS(MM_SUB,"Ignoring GMMSM_UNITDATA_REQ in GW2L I-RAT handler",0,0,0);
           break;

         case GMMSM_CLOSE_SESSION_REQ:
           MSG_HIGH_DS(MM_SUB,"Ignoring GMMSM_CLOSE_SESSION_REQ in GW2L I-RAT handler",0,0,0);
           break;

         case GMMSM_RAT_RESUME_CNF:
           MSG_HIGH_DS(MM_SUB,"Ignoring GMMSM_RAT_RESUME_CNF in GW2L I-RAT handler",0,0,0);
           break;
 
         case GMMSM_FAST_DORMANCY_REQ:
           MSG_HIGH_DS(MM_SUB,"Ignoring GMMSM_FAST_DORMANCY_REQ in GW2L I-RAT handler",0,0,0);
           break;
 
         default:
           MSG_ERROR_DS(MM_SUB,"Unexpected SM to GMM message ID %d",mm_cmd_ptr->cmd.hdr.message_id,0,0);
           break;
       }
       break;

     case MS_GMM_RABM:
       switch(mm_cmd_ptr->cmd.hdr.message_id)
       {
         case GMMRABM_REESTABLISH_REQ:
           mmcoord_route_mm_message(mm_cmd_ptr, mm_emm_dispatcher_fn_ptr);
           break;

         case GMMRABM_RAB_REL_IND:
           MSG_HIGH_DS(MM_SUB,"Ignoring GMMRABM_RAB_REL_IND in GW2L I-RAT handler",0,0,0);
           break;
 
         default:
           MSG_ERROR_DS(MM_SUB,"Unexpected RABM to GMM message ID %d",mm_cmd_ptr->cmd.hdr.message_id,0,0);
           break;
       }
       break;

     case MS_CC_MM:
     case MS_CC_MM_PS:
       switch(mm_cmd_ptr->cmd.hdr.message_id)
       {
         case MMCNM_EST_REQ:
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION        
           mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE,MMCC_SMS_CS);
#else
           mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, MM_REJ_WRONG_STATE);
#endif
           break;
         
         case MMCNM_REL_REQ:
           mm_cnm_rel_req_received = TRUE;
           mm_send_mmcnm_rel_cnf();
           break;
  
         case MMCNM_DATA_REQ:
           MSG_HIGH_DS(MM_SUB,"Ignoring MMCNM_DATA_REQ in GW2L I-RAT handler",0,0,0);
           break;
  
         case MMCC_REEST_REQ:
           MSG_HIGH_DS(MM_SUB,"Ignoring MMCC_REEST_REQ in GW2L I-RAT handler",0,0,0);
           break;
  
         case MMCNM_UNIT_DATA_REQ:
           MSG_HIGH_DS(MM_SUB,"Ignoring MMCNM_UNIT_DATA_REQ in GW2L I-RAT handler",0,0,0);
           break;
  
#ifdef FEATURE_CCBS 
         case MMCNM_PROMPT_RES:
           MSG_HIGH_DS(MM_SUB,"Ignoring MMCNM_PROMPT_RES in GW2L I-RAT handler",0,0,0);
           break;
#endif
  
         default:
           MSG_ERROR_DS(MM_SUB,"Unexpected CNM/MN to MM message ID %d",mm_cmd_ptr->cmd.hdr.message_id,0,0);
           break;
       }
       break;


     case MS_TIMER:
       /* T3346 is common to MM/EMM. depending on active rat route it */
       if((mm_cmd_ptr->cmd.timer_expiry.timer_id >= MIN_MM_TIMER && 
            mm_cmd_ptr->cmd.timer_expiry.timer_id <= MAX_MM_ONLY_TIMER)
          &&
          ((mm_cmd_ptr->cmd.timer_expiry.timer_id != TIMER_T3346) ||
           ((is_umts_mode() || is_gsm_mode()))))
       {
         mmcoord_route_mm_message(mm_cmd_ptr,mm_emm_dispatcher_fn_ptr );
       }
       else
       {
         emm_process_timer_expiry(mm_cmd_ptr, emm_ctrl_data_ptr);
       }
       break;

     default:
       MSG_HIGH_DS(MM_SUB,"=EMM= MM: NAS received msg %d during INTER_RAT, this message shall be currently discarded",
               mm_cmd_ptr->cmd.hdr.message_id,0,0);
       break;
   }
   return TRUE;
}

/*===========================================================================

FUNCTION      emm_reset_attempt_counter

DESCRIPTION
This Function will reset attach and tau attempt counter
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_reset_attempt_counter(void)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
	
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
	
  ASSERT(emm_ctrl_data_ptr != NULL);

  emm_ctrl_data_ptr->emm_attach_attempt_counter = 0;
  emm_ctrl_data_ptr->emm_attach_rej_esm_fail_counter = 0;
  emm_ctrl_data_ptr->emm_tau_attempt_counter = 0;
}

/*===========================================================================

FUNCTION      EMM_ACTIVATE_LTE

DESCRIPTION
  This function initilize the EMM state. MM should call this function for every REG request for LTE
  This function sets the state if LTE is part of the RAT priority list and the state is NULL

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_activate_lte(void)
{
  /* Is LTE in RAT pri list */
  if (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, SYS_RAT_LTE_RADIO_ACCESS) ) 
  {   
    // if mm state is NULL set it to DEREGISTERED and NO_CELL_AVAILABLE
    if(EMM_GET_STATE() == EMM_NULL)
    {
      EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
      /*Do not send LIMITED_SERVICE_EVENT to GSTK*/
    }
  }
}

/*===========================================================================
FUNCTION    EMM_LOCAL_DETACH

DESCRIPTION
  This function performs the NAS local detach 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_multimode_detach(void)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);

  MSG_HIGH_DS(MM_SUB,"=EMM= Moving to DEREGISTERED STATE, Update EPS security_context...",0,0,0);
  emm_update_stored_security_context(emm_ctrl_data_ptr, FALSE);

  EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE) ; 
  /*Not sure if we need to send LIMITED_SERVICE event to GSTK*/
  /* Should we do this?*/
  //emm_reset_ctrl_data(emm_ctrl_data_ptr); /*Commenting this as per QIPL's suggestion*/
}


/*===========================================================================
FUNCTION    EMM_SEND_MULTIMODE_RRC_SYSTEM_UPDATE_FROM_GW


DESCRIPTION
  This function will send LTE RRC SYSTEM UPDATE REQ to LTE RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_multimode_rrc_system_update_from_GW
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  if(mm_is_multimode_sub())
  {
  lte_rrc_system_update_req_s *rrc_system_update_req_ptr;
  sys_plmn_id_s_type  rplmn;
  sys_plmn_list_s_type eplmn;
#ifdef FEATURE_EQUIVALENT_HPLMN
  uint32 plmn_index = 0;
#endif

#ifdef FEATURE_FEMTO_CSG
  uint16 index;
  uint8 length;
#endif
    ASSERT(emm_ctrl_data_ptr != NULL);
    ASSERT(emm_ctrl_data_ptr->rrc_forbidden_list_ptr != NULL);
  /* Allocate memory and initialize it */
  rrc_system_update_req_ptr = emm_modem_mem_alloc(sizeof(lte_rrc_system_update_req_s),
                                                  emm_ctrl_data_ptr);
  mm_check_for_null_ptr((void*)rrc_system_update_req_ptr);
  /* Get rplmn and eplmn */
  sys_eplmn_list_get(&eplmn,&rplmn);
  //If EPLMN list is zero then get the correct RPLMN from emm ctrl data
  if(eplmn.length == 0)
  {
    memscpy(rplmn.identity,sizeof(sys_plmn_id_s_type),emm_ctrl_data_ptr->rplmn_info.rplmn.identity,PLMN_SIZE);
  }

  msgr_init_hdr(&rrc_system_update_req_ptr->msg_hdr, MSGR_NAS_EMM, LTE_RRC_SYSTEM_UPDATE_REQ) ;

 /* Populate EHPLMN list or HPLMN, and RPLMN */
  rrc_system_update_req_ptr->rplmn_info_is_valid = FALSE;
  rrc_system_update_req_ptr->hplmn_info_is_valid = FALSE;
  rrc_system_update_req_ptr->ehplmn_list.num_plmns = 0;

  if((emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_AVAILABLE) || 
     (emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_CS_INVALID))
  {
#ifdef FEATURE_EQUIVALENT_HPLMN
      reg_sim_plmn_list_s_type *ehplmn_list_ptr;
      ehplmn_list_ptr = (reg_sim_plmn_list_s_type *) emm_modem_mem_alloc(
                                            sizeof(reg_sim_plmn_list_s_type),
                                            emm_ctrl_data_ptr);
	  mm_check_for_null_ptr((void*) ehplmn_list_ptr);
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
    reg_sim_per_nas_stacks_ehplmn_list_get_ptr(ehplmn_list_ptr, (sys_modem_as_id_e_type)mm_as_id);
#else
    reg_sim_ehplmn_list_get_ptr(ehplmn_list_ptr);
#endif
      if(ehplmn_list_ptr->length > LTE_RRC_MAX_PLMN_LIST_SIZE)
    {
        MSG_HIGH_DS(MM_SUB,"=EMM= EHPLMN list too long %d - RRC SYTEM UPDATE REQ",
               ehplmn_list_ptr->length, 0, 0);
      rrc_system_update_req_ptr->ehplmn_list.num_plmns = LTE_RRC_MAX_PLMN_LIST_SIZE;
  }
  else
  {
      rrc_system_update_req_ptr->ehplmn_list.num_plmns = 
                                       ehplmn_list_ptr->length;
    }
    for(plmn_index=0; plmn_index<rrc_system_update_req_ptr->ehplmn_list.num_plmns; plmn_index++)
    {
      rrc_system_update_req_ptr->ehplmn_list.plmn[plmn_index] = 
        emm_convert_nas_plmn_id_to_rrc_plmn_id(ehplmn_list_ptr->plmn[plmn_index]);
    }
      emm_modem_mem_free(ehplmn_list_ptr, emm_ctrl_data_ptr);
      ehplmn_list_ptr = NULL;
#else
      /* HPLMN */
      rrc_system_update_req_ptr->hplmn_info_is_valid = TRUE;
#if defined FEATURE_DUAL_SIM|| defined FEATURE_SGLTE
      emm_ctrl_data_ptr->emm_home_plmn = reg_sim_per_subs_hplmn_get(mm_as_id);
#else
      emm_ctrl_data_ptr->emm_home_plmn = reg_sim_hplmn_get();
#endif

      rrc_system_update_req_ptr->hplmn = 
                   emm_convert_nas_plmn_id_to_rrc_plmn_id(emm_ctrl_data_ptr->emm_home_plmn);
#endif
    /* RPLMN */
    rrc_system_update_req_ptr->rplmn = emm_convert_nas_plmn_id_to_rrc_plmn_id(rplmn);
    rrc_system_update_req_ptr->rplmn_info_is_valid = TRUE ;
  }
  rrc_system_update_req_ptr->forbidden_ta_list.num_entries = 
                      emm_ctrl_data_ptr->rrc_forbidden_list_ptr->tai_lst_length ;
  if (rrc_system_update_req_ptr->forbidden_ta_list.num_entries > 0)
  {
    dword index     = 0 ;
    for (index=0; index<rrc_system_update_req_ptr->forbidden_ta_list.num_entries; index++)
    {
      rrc_system_update_req_ptr->forbidden_ta_list.tracking_area[index].plmn = 
        emm_convert_nas_plmn_id_to_rrc_plmn_id(emm_ctrl_data_ptr->rrc_forbidden_list_ptr->diff_plmn[index].plmn) ;
      rrc_system_update_req_ptr->forbidden_ta_list.tracking_area[index].tac = 
                                                 emm_ctrl_data_ptr->rrc_forbidden_list_ptr->diff_plmn[index].tac ;
    } /* for (index=0; index<rrc_service_req.ehplmn_list.num_plmns; index++) */
  }

  rrc_system_update_req_ptr->forbidden_manual_ta_list.num_entries = 
                            emm_ctrl_data_ptr->manual_ftai_list_ptr->tai_lst_length ;
  if (rrc_system_update_req_ptr->forbidden_manual_ta_list.num_entries > 0)
  {
    dword index     = 0 ;
    for (index=0; index<rrc_system_update_req_ptr->forbidden_manual_ta_list.num_entries; index++)
    {
      rrc_system_update_req_ptr->forbidden_manual_ta_list.tracking_area[index].plmn = 
        emm_convert_nas_plmn_id_to_rrc_plmn_id(emm_ctrl_data_ptr->manual_ftai_list_ptr->diff_plmn[index].plmn) ;
      rrc_system_update_req_ptr->forbidden_manual_ta_list.tracking_area[index].tac = 
                                               emm_ctrl_data_ptr->manual_ftai_list_ptr->diff_plmn[index].tac ;
    }
  }
  /* EPLMN list */
  emm_set_rrc_eplmn_list(&rrc_system_update_req_ptr->eplmn_list, emm_ctrl_data_ptr);

#ifdef FEATURE_FEMTO_CSG
  if(*(emm_ctrl_data_ptr->nv_csg_support) > SYS_CSG_SUPPORT_DISABLED)
  {
    rrc_system_update_req_ptr->csg_whitelist_type = LTE_RRC_CSG_LIST_INITIALIZED;
    
    length = 0;
  
    for(index = 0; index < emm_ctrl_data_ptr->operator_csg_list->length; index++)
    {
      if(emm_ctrl_data_ptr->operator_csg_list->csg[index].timer_count == MM_TIMER_DISABLED)
      {
        rrc_system_update_req_ptr->csg_whitelist.csg_info[length].plmn = 
                                         emm_convert_nas_plmn_id_to_rrc_plmn_id(emm_ctrl_data_ptr->operator_csg_list->csg[index].plmn);
        rrc_system_update_req_ptr->csg_whitelist.csg_info[length].csg_id = emm_ctrl_data_ptr->operator_csg_list->csg[index].csg_id;
        length++;
      }
    }
  
    for(index = 0; index < emm_ctrl_data_ptr->allowed_csg_list->length; index++)
    {
      if(emm_ctrl_data_ptr->allowed_csg_list->csg[index].csg_id != SYS_CSG_ID_INVALID)
      {
        rrc_system_update_req_ptr->csg_whitelist.csg_info[length].plmn = 
                                  emm_convert_nas_plmn_id_to_rrc_plmn_id(emm_ctrl_data_ptr->allowed_csg_list->csg[index].plmn);
        rrc_system_update_req_ptr->csg_whitelist.csg_info[length].csg_id = emm_ctrl_data_ptr->allowed_csg_list->csg[index].csg_id;
        length++;
      }
    }
  
    rrc_system_update_req_ptr->csg_whitelist.num_entries = length;
  }
  else
  {
    rrc_system_update_req_ptr->csg_whitelist_type = LTE_RRC_CSG_LIST_UNINITIALIZED;
  }
#endif 

  if(emm_msgr_send((msgr_hdr_s *)rrc_system_update_req_ptr, sizeof(lte_rrc_system_update_req_s)) != E_SUCCESS)
  {
      MSG_FATAL_DS(MM_SUB, "=EMM= emm_msgr_send failed!", 0,0,0 );
  } 
  else
  {
      MSG_HIGH_DS(MM_SUB, "=EMM= Sent LTE_RRC_SYSTEM_UPDATE_REQ", 0,0,0 );
  }
  /* Free the memory block */
  emm_modem_mem_free(rrc_system_update_req_ptr, emm_ctrl_data_ptr);
} 
} 

/*===========================================================================

FUNCTION  mm_is_tmsi_valid

DESCRIPTION
           TMSI validity check
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_is_tmsi_valid(byte *tmsi, word tmsi_length)
{
  byte index = 0;

  for (index=0; index < tmsi_length; index++)
  {
    if (tmsi[index] != 0xFF)
    {
      return TRUE;
    }
  } 
  return FALSE;
} /* emm_is_tmsi_valid */

/*===========================================================================

FUNCTION MM_INVALIDATE_LU_INFO

DESCRIPTION
  This function is wrapper function of mm_delete_lu_status for EMM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  mm_invalidate_lu_info(byte update_status)
{
  mm_delete_lu_status(update_status);

}/* mm_invalidate_lu_info */

/*===========================================================================

FUNCTION  MM_UPDATE_CS_REJECT_INVALID_IMSI_STATUS

DESCRIPTION
  This function updates MM state and parameters when combined Attach/TAU is
  successful for EPS only because of IMSI unknown in HSS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_update_cs_reject_invalid_imsi_status(void)
{
  /* Set the update status to U3 ROAMING NOT ALLOWED and delete TMSI, LAI and
     ciphering key sequence number.*/
  mm_invalidate_lu_info(ROAMING_NOT_ALLOWED);

  /* USIM shall be considered as invalid for non-EPS services */
  mm_manual_mode_user_selection_procedure = FALSE;
  gmm_update_mm_serving_plmn_info(SYS_SIM_STATE_CS_INVALID,
                                  SYS_SRV_DOMAIN_PS_ONLY,
                                  FALSE,
                                  SYS_SRV_STATUS_SRV,
                                  TRUE);

  /* Set MM state */
  mm_state = MM_IDLE;
  mm_idle_substate = MM_NO_IMSI;

}/* mm_update_cs_reject_invalid_imsi_status */

/*=========================================================================== 
 
FUNCTION    mm_update_detach_req_status

DESCRIPTION
  Updates GMM/MM status, based upon the EMM_CAUSE received during DETACH_REQ
  if A/Gb mode or Iu mode is supported

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void mm_update_detach_req_status
(
  byte  cause
)
{
  if((mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                      SYS_RAT_GSM_RADIO_ACCESS) == TRUE)
     ||
     (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                      SYS_RAT_UMTS_RADIO_ACCESS) == TRUE))
  {
    MSG_HIGH_DS(MM_SUB,"=EMM= Performed MT Detach, Updating GMM/MM Cause %d", cause,0,0);

    switch(cause)
    {
      case LTE_NAS_IMSI_UNKNOWN_IN_HSS:      
        /*The MS shall set the update status to U3 ROAMING NOT ALLOWED and
        shall delete any TMSI, LAI and ciphering key sequence number.
        The SIM shall be considered as invalid for non-GPRS services
        until switching off or the SIM is removed.*/
        mm_delete_lu_status( ROAMING_NOT_ALLOWED );
        break;
 
      case ILLEGAL_MS:
      case ILLEGAL_ME:
      case GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED:
        /* ------------------------------------------------------------------
        ** The MS shall set the GPRS update status to GU3 ROAMING NOT ALLOWED
        ** (and shall store it according to clause 4.1.3.2) and shall delete
        ** any P-TMSI, P-TMSI signature, RAI and GPRS ciphering key sequence
        ** number.The new GMM state is GMM-DEREGISTERED.
        ** ------------------------------------------------------------------ */
        gmm_delete_ps_location_information();
        gmm_delete_gprs_cksn();
        
        gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;
        
        mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                            gmm_stored_ptmsi_signature,
                                            gmm_stored_rai,
                                            gmm_update_status );
        
        gmm_set_state( GMM_DEREGISTERED);
    
        if (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS)
        {
          /* -----------------------------------------------------------------
          ** A GPRS MS operating in MS operation mode A or B shall in addition
          ** set the update status to U3 ROAMING NOT ALLOWED, shall delete
          ** any TMSI, LAI and ciphering key sequence number.
          ** ----------------------------------------------------------------- */
          mm_delete_lu_status( ROAMING_NOT_ALLOWED );    
        }
        break;
       
      case GPRS_SERVICES_NOT_ALLOWED:
        /* ------------------------------------------------------------------
        ** The MS shall set the GPRS update status to GU3 ROAMING NOT ALLOWED
        ** (and shall store it according to clause 4.1.3.2) and shall delete
        ** any P-TMSI, P-TMSI signature, RAI and GPRS ciphering key sequence
        ** number. The new GMM state is GMM-DEREGISTERED. The SIM shall be
        ** considered as invalid for GPRS services until switching off or the
        ** SIM is removed.
        ** ------------------------------------------------------------------ */
        gmm_delete_ps_location_information();
        gmm_delete_gprs_cksn();
        gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED; 
        mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                            gmm_stored_ptmsi_signature,
                                            gmm_stored_rai,
                                            gmm_update_status );
     
        gmm_set_state( GMM_DEREGISTERED);  
        
        break;
       
      case PLMN_NOT_ALLOWED:
      case LA_NOT_ALLOWED:
      case NATIONAL_ROAMING_NOT_ALLOWED:
      case NO_SUITABLE_CELLS_IN_LA:
        /* -------------------------------------------------------------
        ** The MS shall delete any RAI, P-TMSI, P-TMSI signature and
        ** GPRS ciphering key sequence number, shall set the GPRS update
        ** status to GU3 ROAMING NOT ALLOWED and enter the state
        ** GMM-DEREGISTERED.
        ** ------------------------------------------------------------- */
        if (cause == PLMN_NOT_ALLOWED)
        {
          gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;   
        }
        else
        {
          gmm_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;
        }
       
        gmm_set_state( GMM_DEREGISTERED);
       
        gmm_delete_ps_location_information();
        gmm_delete_gprs_cksn();
        mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                            gmm_stored_ptmsi_signature,
                                            gmm_stored_rai,
                                            gmm_update_status );
       
        if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)
        {
          /* ------------------------------------------------------------
          ** If the MS is operating in MS operation mode A , the MS shall
          ** in addition set the update status to U3 ROAMING NOT ALLOWED,
          ** shall delete any TMSI, LAI and ciphering key sequence number
          ** and shall reset the location update attempt counter.
          ** ------------------------------------------------------------ */
          mm_delete_lu_status( ROAMING_NOT_ALLOWED );
          mm_clear_lu_attempt_counter( );
		  mm_previous_idle_substate = NULL;
        }
       
        break;

      case GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN:
        /* -------------------------------------------------------------
        ** The MS shall delete any RAI, P-TMSI, P-TMSI signature and
        ** GPRS ciphering key sequence number, shall set the GPRS update
        ** status to GU3 ROAMING NOT ALLOWED and enter the state
        ** GMM-DEREGISTERED.
        ** ------------------------------------------------------------- */
        gmm_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;
        gmm_set_state( GMM_DEREGISTERED);  
            
        gmm_delete_ps_location_information();
        gmm_delete_gprs_cksn();
        mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                                  gmm_stored_ptmsi_signature,
                                                  gmm_stored_rai,
                                                  gmm_update_status );   
        
        break;
    
#ifdef FEATURE_FEMTO_CSG
       case CSG_NOT_AUTHORIZED:
         /*----------------------------------------------------------------------
         ** 5.5.2.3.2, 24.301
         ** UE shall handle the GMM parameters GMM state, GPRS update status and
         ** GPRS attach attempt counter as specified in 3GPP TS 24.008 [13] for
         ** the case when a DETACH REQUEST is received with GMM cause with the
         ** same value and with detach type set to "re-attach not required".
         **
         ** 4.7.4.2.2, 24.008
         ** The MS shall set the GPRS update status to GU3 ROAMING NOT ALLOWED
         ** and shall reset the GPRS attach attempt counter. The state is changed
         ** to GMM-DEREGISTERED.LIMITED-SERVICE
         ** --------------------------------------------------------------------*/
         gmm_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;  
         mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                             gmm_stored_ptmsi_signature,
                                             gmm_stored_rai,
                                             gmm_update_status );
         gprs_attach_attempt_counter = 0;
         /* When UE is camped in LTE GMM sub-state should remain no_cell_available */
         gmm_set_state( GMM_DEREGISTERED );
        break;
#endif

      default:
        /* -------------------------------------------------------------
        ** UE shall set the GPRS update status to GU2 NOT UPDATED and shall 
        ** delete the GMM parameters P-TMSI, P-TMSI signature, RAI, GPRS 
        ** ciphering key sequence number and shall enter the state GMM-DEREGISTERED.
        ** ------------------------------------------------------------- */
        gmm_update_status = GMM_GU2_NOT_UPDATED;
        gmm_delete_ps_location_information();
        gmm_delete_gprs_cksn();
        mmsim_save_ps_location_information(gmm_stored_ptmsi,
                                           gmm_stored_ptmsi_signature,
                                           gmm_stored_rai,
                                           gmm_update_status);
        gmm_set_state( GMM_DEREGISTERED);      
        break;
    }
  }
}/* mm_update_detach_req_status */

/*===========================================================================

FUNCTION  EMM SERVICE LOSS ON LTE

DESCRIPTION
  This function sets the global variable service_loss_on_multimode_rat to LTE rat.
  This function is called whenever there is loss of service in LTE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_service_loss_on_lte(void)
{

  if((mm_service_loss_on_multimode_rat == SYS_RAT_NONE) && 
      (mm_service_loss_on_rat == SYS_RAT_NONE) && 
      ((EMM_GET_STATE() == EMM_REGISTERED) ||
      (EMM_GET_STATE() == EMM_TRACKING_AREA_UPDATING_INITIATED) ||
      (EMM_GET_STATE() == EMM_DEREGISTERED_INITIATED) ||
      (EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED)))
  {
    MSG_HIGH_DS(MM_SUB, "=EMM= Loss of service in LTE", 0,0,0 );
    mm_service_loss_on_multimode_rat = SYS_RAT_LTE_RADIO_ACCESS;
     
    if(mm_serving_plmn.info.ps_data_suspend == FALSE)
    {
      mm_send_cm_ps_data_suspend_ind();
    }
    mm_serving_plmn.info.ps_data_suspend = TRUE;
/*4.7.1.7a	Intersystem change from S1 mode to A/Gb mode or S1 mode to Iu mode with ISR activated*/
#ifdef FEATURE_NAS_REL11
    if(emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI 
                         && gmm_is_prev_mode_connected == FALSE &&
						 mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL11)
    {
      gmm_rau_is_required = TRUE;
    }
#endif
  }
}

/*===========================================================================

FUNCTION  EMM_GET_GMM_REG_LTE_CS_PARAM

DESCRIPTION
  This function returns gmm_reg_lte_cs_param

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
sys_lte_cs_domain_param_s_type emm_get_gmm_reg_lte_cs_param(void)
{
  return gmm_reg_lte_cs_param;
}

/*===========================================================================

FUNCTION  EMM SERVICE ACQUIRED ON LTE

DESCRIPTION
  This function sends rat change request to sm and resets the loss on service global variables
  of lte and gw

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_service_acquired_on_lte
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != NULL);

  if(mm_service_loss_on_rat != SYS_RAT_NONE)
  {
    gmm_sm_send_rat_change_req(mm_service_loss_on_rat, TRUE);
    mm_send_cm_bearer_ctxt_transfer_ind(mm_serving_plmn.info.active_rat);
#ifdef FEATURE_LTE_REL9
    if(mm_is_reg_needed_for_mmit_on_LTE(mm_service_loss_on_rat))
    {
      MSG_HIGH_DS(MM_SUB,"=EMM= TAU to be performed - Mobility Management for IMS voice termination",0,0,0);
      emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
      emm_set_and_write_tin_type(NAS_MM_TIN_P_TMSI);
    }
#endif
    if(mm_location_information.location_update_status != UPDATED)
    {
      emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_REQ_DUE_TO_CS_OR_CSPS_PROC;
      MSG_HIGH_DS_0(MM_SUB,"=MM= Set SGs assosicaiton flag - MM update status is not updated on upper layer rat change");
    }
    mm_service_loss_on_rat = SYS_RAT_NONE;
  }
  mm_service_loss_on_multimode_rat = SYS_RAT_NONE;
#ifdef FEATURE_GSM_GPRS        
  if(gmmllc_get_glif_state() == GMM_READY_STATE)
  {
    MSG_HIGH_DS(MM_SUB,"=EMM= TAU to be performed - GMM READY STATE",0,0,0);
#ifdef FEATURE_LTE_REL11
    if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) &&
       (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI))
    {
      emm_set_and_write_tin_type(NAS_MM_TIN_P_TMSI);
    }
#endif
    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
/*4.7.1.7a	Intersystem change from S1 mode to A/Gb mode or S1 mode to Iu mode with ISR activated*/
#ifdef FEATURE_NAS_REL11
	if(mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL11)
	{
    gmm_is_prev_mode_connected = TRUE;
    MSG_HIGH_DS_1(MM_SUB, "=EMM= GMM Previous mode Ready or Connected = %d", gmm_is_prev_mode_connected);
	}
#endif
  }
        
  if(gmmllc_get_glif_state() != GMM_IDLE_STATE)
  {
    gmmllc_reinit(GMM_IDLE_STATE);
  }
#endif
  if(emm_ctrl_data_ptr->sg_association_to_be_established == NAS_EMM_SG_ASSOC_REQ_FOR_CSFB_RAU)          
  {
    emm_ctrl_data_ptr->sg_association_to_be_established = NAS_EMM_SG_ASSOC_NOT_REQ;
  } 
  
#ifdef FEATURE_LTE_REL11
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) &&
     (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI) &&
     (emm_ctrl_data_ptr->ps_resumption_status == EMM_PS_RESUMPTION_PENDING))
  {
    emm_set_and_write_tin_type(NAS_MM_TIN_P_TMSI);
  }
#endif
  mm_csfb_is_blind_redir_to_lte_req_to_be_sent = FALSE;
  gmm_previous_network_op_mode = PS_DOMAIN_SYS_INFO_UNAVAILABLE;
  mm_serving_plmn.network_op_mode = PS_DOMAIN_SYS_INFO_UNAVAILABLE;

}

/*===========================================================================

FUNCTION  mm_map_guti_to_ptmsi

DESCRIPTION
  This function converts GUTI to PTMSI

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_map_guti_to_ptmsi(gmm_ptmsi_type *ptmsi, emm_ctrl_data_type     *emm_ctrl_data_ptr) 
{
  
  ASSERT(emm_ctrl_data_ptr != NULL);

  ASSERT(ptmsi != (void *)NULL);

  memset(ptmsi, 0, sizeof(gmm_ptmsi_type));

  /* Populate P-TMSI from GUTI. As per spec 23.003, section 2.8.2 and 2.8.2.1*/
  /* Populate from M-TMSI*/
  ptmsi->digit[0] = emm_ctrl_data_ptr->emm_guti.m_tmsi[3] & 0x3f; 
  ptmsi->digit[2] = emm_ctrl_data_ptr->emm_guti.m_tmsi[1];
  ptmsi->digit[3] = emm_ctrl_data_ptr->emm_guti.m_tmsi[0]; 
  /* Populate from MME code. This is the NRI part of P-TMSI*/
  ptmsi->digit[1] = emm_ctrl_data_ptr->emm_guti.mme_code;

  /* Set bits 30 and 31 to 1*/
  ptmsi->digit[0] = ptmsi->digit[0] | 0xc0;

}

/*===========================================================================

FUNCTION  emm_map_guti_to_rai

DESCRIPTION
  Maps RAI to GUTI

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_map_guti_to_rai(routing_area_id_T *ra_id, emm_ctrl_data_type *emm_ctrl_data_ptr) 
{
  ASSERT(emm_ctrl_data_ptr != NULL);

  ASSERT(ra_id != (void *)NULL);

  memset(ra_id, 0, sizeof(routing_area_id_T));

  ra_id->routing_area_code = emm_ctrl_data_ptr->emm_guti.mme_code;
  ra_id->location_area_code.lac[0] = 
                                   emm_ctrl_data_ptr->emm_guti.mme_group_id[1];
  ra_id->location_area_code.lac[1] = 
                                   emm_ctrl_data_ptr->emm_guti.mme_group_id[0];
  ra_id->plmn_id = emm_ctrl_data_ptr->emm_guti.plmn;
}


/*===========================================================================
FUNCTION  MM_MAP_PTMSI_RAI_TO_GUMMEI_INFO 

DESCRIPTION
  Functions maps P-TMSI and RAI to GUMMEI info - MCC + MNC + MMEGI + MMEC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  mm_map_ptmsi_rai_to_gumme_info
(
  lte_nas_emm_guti_type  *guti_ptr
)
{
  ASSERT(guti_ptr != NULL);

  MSG_LOW_DS(MM_SUB,"=EMM= Mapping P-TMSI and RAI to GUMMEI info",0,0,0);

  guti_ptr->plmn  = gmm_stored_rai.plmn_id;
  guti_ptr->mme_group_id[0]  =  gmm_stored_rai.location_area_code.lac[1];
  guti_ptr->mme_group_id[1]  =  gmm_stored_rai.location_area_code.lac[0];
  guti_ptr->mme_code = gmm_stored_ptmsi.digit[1];

} /* mm_map_ptmsi_rai_to_gumme_info */

/*===========================================================================

FUNCTION  MM_IS_RAI_VALID

DESCRIPTION
  This functions validates RAI

DEPENDENCIES
  None

RETURN VALUE
  TRUE: RAI is valid
  FALSE: RAI is invalid

SIDE EFFECTS
  None

===========================================================================*/
boolean  mm_is_rai_valid(void)
{
  return has_valid_rai();
} /* mm_is_rai_valid */


/*===========================================================================

FUNCTION emm_get_mapped_gw_identity

DESCRIPTION
  This function maps the GUTI to to RAI, P-TMSI and P-TMSI signature to be used in GW Attach Request Message

DEPENDENCIES
  None

RETURN VALUE
  TRUE

SIDE EFFECTS
  None

===========================================================================*/

void emm_get_mapped_gw_identity(mm_gw_identity_param_type *mm_identity, 
                                  emm_ctrl_data_type  *emm_ctrl_data_ptr) 
{
  ASSERT(emm_ctrl_data_ptr != NULL);

  ASSERT(mm_identity != (void *)NULL);

  memset(mm_identity, 0, sizeof(mm_gw_identity_param_type));

  /* Populate P-TMSI from GUTI. As per spec 23.003, section 2.8.2 and 2.8.2.1*/
  /* Populate from M-TMSI*/
  mm_identity->ptmsi.digit[0] = emm_ctrl_data_ptr->emm_guti.m_tmsi[3] & 0x3f;
  mm_identity->ptmsi.digit[2] = emm_ctrl_data_ptr->emm_guti.m_tmsi[1];
  mm_identity->ptmsi.digit[3] = emm_ctrl_data_ptr->emm_guti.m_tmsi[0]; 

  /* Set bits 30 and 31 to 1*/
  mm_identity->ptmsi.digit[0] = mm_identity->ptmsi.digit[0] | 0xc0;

  /* Populate from MME code. This is the NRI part of P-TMSI*/
  mm_identity->ptmsi.digit[1] = emm_ctrl_data_ptr->emm_guti.mme_code;

  /* Populate from M-TMSI byte 3. */
  mm_identity->ptmsi_signature.value[0] = emm_ctrl_data_ptr->emm_guti.m_tmsi[2];
  /* Get the latest ul count and also check the context exist or not.*/
  if(emm_get_ul_count(mm_identity->ul_count_at_generation)==TRUE) 

   {
      /* key need to derive for L2GW PSHO failure and normal cases.*/
      if((mm_rau_pending_in_conn_mode == TRUE) ||
         (memcmp(mm_identity->ul_count_at_generation,mm_mapped_gw_identity.ul_count_at_generation,
         sizeof(mm_identity->ul_count_at_generation))!=0))   
      {
        /* generate the token and compute keys at this point */
        emm_get_eps_params(mm_identity);
        /*Save the latest count value as it is being increased in the token generation */
        emm_get_ul_count(mm_identity->ul_count_at_generation);
        /* update and save the generated keys. */
        mm_update_gw_auth_keys(*mm_identity,emm_ctrl_data_ptr,RRC_SIM_UPDATE_VALUE_LTE_MAPPED);
      }
      else
      {
        /* if uplink count match then don't generate the new keys use old one */
        memscpy(mm_identity,sizeof(mm_gw_identity_param_type),&mm_mapped_gw_identity,sizeof(mm_gw_identity_param_type));
      }
   }
  else
   {  /* if security context doesnot exist then set key to no available and ptmsi signature
          to zero */   
      mm_identity->eKSI = 0x07;
      memset((void *)&(mm_identity->ptmsi_signature.value[1]), 0, sizeof(word));
   } 
  /* Populate RA*/
  mm_identity->ra_id.routing_area_code = emm_ctrl_data_ptr->emm_guti.mme_code;
  mm_identity->ra_id.location_area_code.lac[0] = 
                                   emm_ctrl_data_ptr->emm_guti.mme_group_id[1];
  mm_identity->ra_id.location_area_code.lac[1] = 
                                   emm_ctrl_data_ptr->emm_guti.mme_group_id[0];
  mm_identity->ra_id.plmn_id = emm_ctrl_data_ptr->emm_guti.plmn;
  /*save the keys and uplink count in the global data structure */
  memscpy(&mm_mapped_gw_identity,sizeof(mm_gw_identity_param_type),mm_identity,sizeof(mm_gw_identity_param_type));

}

/*===========================================================================

FUNCTION MM_GET_NATIVE_GW_IDENTITY

DESCRIPTION
  This function copies the PTMSI native identity to the passed variable
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_get_native_gw_identity
(
  mm_gw_identity_param_type *gw_identity,
  auth_security_context_id cksn_context
)
{
  byte i;
  for (i=0; i < PTMSI_SIZE; i++)
  {
    gw_identity->ptmsi.digit[i] = gmm_stored_ptmsi.digit[i];
  }
  
  for (i=0; i < PLMN_SIZE; i++)
  {
    gw_identity->ra_id.plmn_id.identity[i] = gmm_stored_rai.plmn_id.identity[i];
  }
  
  for (i=0; i < LAC_SIZE; i++)
  {
    gw_identity->ra_id.location_area_code.lac[i] = gmm_stored_rai.location_area_code.lac[i];
  }
  
  gw_identity->ra_id.routing_area_code = gmm_stored_rai.routing_area_code;
  
  for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
  {
    gw_identity->ptmsi_signature.value[i] = gmm_stored_ptmsi_signature.value[i];
  }
  gw_identity->eKSI = (byte)(auth_get_ciphering_key_sequence_number(cksn_context));
}

/*===========================================================================
 
FUNCTION  EMM_UPDATE_MULTIMODE_SR_REJECT

DESCRIPTION
  This function updates EMM state and parameters upon GW serivice reject

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_update_multimode_sr_reject
(
  lte_nas_emm_cause_type  emm_cause
)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);

  MSG_HIGH_DS(MM_SUB,"=EMM= GW SRV REJ - Updating EMM w/ cause %d", emm_cause,0,0);

  /* Set flag to indicate GW reject handling */
  emm_ctrl_data_ptr->gw_reject_update = TRUE;

  emm_process_rau_sr_reject(SERVICE_REJECT, emm_cause, emm_ctrl_data_ptr);

}/* emm_update_multimode_sr_reject */

/*=========================================================================== 
 
FUNCTION  EMM_UPATE_MULTIMODE_MT_DETACH

DESCRIPTION
  This function updates EMM state and parameters upon GW MT DETACH

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 
===========================================================================*/
void emm_update_multimode_mt_detach
(
  lte_nas_emm_cause_type  emm_cause
)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);

  /* Set flag to indicate GW reject handling */
  emm_ctrl_data_ptr->gw_reject_update = TRUE;

  MSG_HIGH_DS(MM_SUB,"=EMM= GW MT Detach - Updating EMM w/ cause %d", emm_cause,0,0);

  /* Update update status, GUTI, last registered TAI, TAI list, KSI */
  emm_mt_detach_invalidate_mobility(emm_cause, emm_ctrl_data_ptr);

  /* Reset Attach Attempt Counter */
  switch (emm_cause)
  {
    case LTE_NAS_PLMN_NOT_ALLOWED:
    case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
    case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
    case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
    case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
    case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
      MSG_HIGH_DS(MM_SUB,"=EMM= Resetting attach attempt counter - Multimode detach ",
               0,0,0);
      emm_ctrl_data_ptr->emm_attach_attempt_counter = 0;
      break ;
    default:
      break;
  } 

  /* Update EMM state */
  if(emm_cause != LTE_NAS_IMSI_UNKNOWN_IN_HSS)
  {
    EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_NO_CELL_AVAILABLE);
  }

}/* mm_update_detach_req_status */

/*===========================================================================

FUNCTION  EMM_MULTIMODE_COMPLETE_UPDATE

DESCRIPTION
  This function is called to complete EMM status update upon GW ATTACH/RAU/SR/
  MT DETACH handling

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_multimode_complete_update(void)
{
  emm_ctrl_data_type  *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);

  /* Update RRC */
  emm_send_sim_update(emm_ctrl_data_ptr);
  emm_send_rrc_system_update(emm_ctrl_data_ptr);

  /* Reset flag*/
  emm_ctrl_data_ptr->gw_reject_update = FALSE;

}/* emm_multimode_complete_update */


/*===========================================================================

FUNCTION  MM_HANDLE_TIMER_T3412_EXPIRY

DESCRIPTION
  This function does the neccessary action when T3412 exprires and ISR is ON and UE is
  GSM/UMTS mode.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_handle_timer_t3412_expiry(emm_ctrl_data_type *emm_ctrl_data_ptr)
{
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ;
  /*
   * 24.008 Section 4.4.1
   *  The normal location updating procedure shall also be started if the MS is configured to use CS
   *  fallback and SMS over SGs, or SMS over SGs only, and the TIN indicates "RAT-related TMSI",
   *   a) when the periodic tracking area update timer T3412 expires and the network operates in 
   *       network operation mode II or III; or
   *   b) when the MS enters a GERAN or UTRAN cell in network operation mode II or III and the 
   *       E-U1TRAN deactivate ISR timer T3423 is running.
   */
  if((is_umts_mode() || is_gsm_mode()) &&
     (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI) &&
     (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
     (mm_serving_plmn.info.service_status == SYS_SRV_STATUS_SRV))
  {
    if((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_2) ||
       (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_3))
    {
       if(mm_state != MM_IDLE)
       {
         MSG_HIGH_DS(MM_SUB,"Pending LU as MM state is not IDLE",0,0,0);
         mm_pending_lu_request = NORMAL_LU;
       }
       else if ( (mm_idle_substate == MM_NORMAL_SERVICE )
  #ifdef FEATURE_NAS_REL10
                 &&((mm_timer_status[TIMER_T3246] == TIMER_STOPPED)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
                   ||(mm_is_rrc_access_class_11_to_15() == TRUE)
#endif
                   )
  #endif 
  #ifdef FEATURE_WRLF_SYSTEM_SEL
                 &&(mm_wcdma_rlf_state == WCDMA_RLF_IDLE)
  #endif
           )
       {
         /*
         ** T3412 is expired but LU is blocked due to access class restriction or other causes.
         ** In that case, remember the pending LU type, so that when restriction is removed,
         ** UE should perform Normal LU.
         ** If LU is not blocked then the lu pending flag will be reset to NO_LU.
         */
         mm_pending_lu_request = NORMAL_LU;
         mm_initiate_lu( MM_LU_CAUSE_NORMAL);
       }
    }
    else if(mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)
    {
      if (gmm_state == GMM_REGISTERED)
      {
        gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
        gmm_initiate_routing_area_update();
      }
      else if (gmm_state == GMM_SERVICE_REQUEST_INITIATED)
      {
        gmm_ra_update_pending = TRUE;
        gmm_ra_update_type    = GMM_PERIODIC_UPDATING;
      }
      else if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED &&
                  gmm_ra_update_type == GMM_PERIODIC_UPDATING)
      {
        gmm_pending_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
        gmm_ra_update_pending = TRUE;
      }
    }
  }
}

/*===========================================================================

FUNCTION  EMM_ATTACH_TAU_SUCCESS_UPDATE_GW

DESCRIPTION
  This function updates GW flags upon successful TAU & ATTACH

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_attach_tau_success_update_gw(void)
{
  emm_ctrl_data_type      *emm_ctrl_data_ptr;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL);
  if(gmm_is_suspended == TRUE)
  {
    MSG_HIGH_DS(MM_SUB,"=EMM= Reset GW PS session suspended flag",0,0,0);
    gmm_is_suspended = FALSE;
  }
#ifdef FEATURE_GSM_GPRS
#ifdef FEATURE_LTE_REL11
  if (emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11) 
  {
     /* Rel 11. reset GPRS atatch attempt counter after successful EPS attach*/
     gprs_attach_attempt_counter = 0;
  }
#endif
  gmm_rau_is_required = FALSE;
#endif
  gmm_force_registration = FALSE;

  return;
}

#ifdef FEATURE_WRLF_SYSTEM_SEL
/*===========================================================================

FUNCTION  MM_SEND_RRC_ABORT_REQ_DURING_WRLF

DESCRIPTION
  This function sends RRC_ABORT_REQ to RRC during RLF situation 
  in WCDMA

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  mm_send_rrc_abort_req_during_wrlf(void)
{
  mm_send_rrc_hard_abort_req( mm_wrlf_serving_plmn_ptr->info.active_rat );
  mmcoord_set_wrlf_state(WCDMA_RLF_IDLE);
  mm_received_suitable_search_end_ind = FALSE;
  MSG_HIGH_DS(MM_SUB,"=MM= WCDMA RLF IDLE STATE",0,0,0);
  mm_substate_control(MM_STD_NO_SERVICE);
  gmm_set_substate( GMM_NO_CELL_AVAILABLE );
}
#endif

/*===========================================================================

FUNCTION  mm_csfb_set_state

DESCRIPTION: This function modifies CSB state variable. 
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mm_csfb_set_state(mm_csfb_state_type new_csfb_state)
{

#ifdef FEATURE_DUAL_SIM
  sp_mt_call_info mt_call_subs_update = {SP_TECH_MT_CALL_STATE_NONE, SYS_MODEM_AS_ID_NO_CHANGE, SYS_SYS_MODE_MAX};
#endif
	
#ifdef FEATURE_MODEM_HEAP
  if(new_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)
  {
    if( mm_csfb_service_req_message_ptr == NULL )
    {
      mm_csfb_service_req_message_ptr =
                (mm_cmd_type *)modem_mem_alloc(sizeof( mm_cmd_type ),MODEM_MEM_CLIENT_NAS);
      mm_check_for_null_ptr((void*)mm_csfb_service_req_message_ptr );
    }
  }
  if(new_csfb_state == MM_CSFB_STATE_NULL || 
      new_csfb_state == MM_CSFB_STATE_LU_PENDING)
  {
    if(mm_csfb_service_req_message_ptr != NULL)
    {
      modem_mem_free(mm_csfb_service_req_message_ptr,MODEM_MEM_CLIENT_NAS);
      mm_csfb_service_req_message_ptr = NULL;
    }
  }
#endif
  /*
  ** Sending the csfb call status indication to RRC layer whenever
  **  a) CSFB MO or MT call has started
  **  b) CSFB call has ended.
  */
  if(((new_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG) ||
     (new_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)) &&
     (mm_csfb_state == MM_CSFB_STATE_NULL))
  {
    mm_send_rrc_csfb_call_status_ind(TRUE);
    emm_send_rrc_csfb_call_status_ind(LTE_RRC_CSFB_CALL_STARTED);
    if (new_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG)
    {
      mm_send_rr_csfb_call_status_ind(TRUE, TRUE);
    }
    else
    {
      mm_send_rr_csfb_call_status_ind(TRUE, FALSE);
    }
#ifdef FEATURE_TDSCDMA
    mm_csfb_regional_handling(TRUE);    
#endif
  }
  else if(((new_csfb_state == MM_CSFB_STATE_NULL) ||
             (new_csfb_state == MM_CSFB_STATE_LU_PENDING)) &&
             ((mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG)||
              (mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)))
  {
    mm_send_rrc_csfb_call_status_ind(FALSE);
    emm_send_rrc_csfb_call_status_ind(LTE_RRC_CSFB_CALL_ENDED);
    mm_send_rr_csfb_call_status_ind(FALSE, FALSE);
#ifdef FEATURE_TDSCDMA
    if(mm_state == MM_IDLE)
    {
      mm_resume_policyman();
    }
#endif 
  }

  if ((new_csfb_state == MM_CSFB_STATE_NULL) &&
      (mm_psm_ready_req_rejected == TRUE) &&
      (mm_check_ready_for_psm() == SYS_PSM_STATUS_READY))
  {
    mm_send_mmr_psm_ready_ind();
  }

#ifdef FEATURE_TDSCDMA
  if((new_csfb_state != MM_CSFB_STATE_MT_CALL_ORIG) && (new_csfb_state != MM_CSFB_STATE_MO_CALL_ORIG))    
  {
    mm_csfb_regional_handling(FALSE);    
  }
#endif
  
#ifdef FEATURE_DUAL_SIM
  if((mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG) && (new_csfb_state == MM_CSFB_STATE_NULL) &&
      (mm_state == MM_IDLE))
  {
       mt_call_subs_update.state = SP_TECH_MT_CALL_RELEASE;
       mt_call_subs_update.subscription_id = (sys_modem_as_id_e_type)mm_sub_id;
       mm_convert_rat_to_tech_id(emm_ctrl_data_ptr->plmn_service_state->active_rat, &mt_call_subs_update.tech_id);
       MSG_HIGH_DS( MM_SUB, "=MM= MCS being informed of SP_TECH_MT_CALL_RELEASE by NAS state = %d, subs_id = %d, tech_id = %d", mt_call_subs_update.state, 
                                                                       mt_call_subs_update.subscription_id, 
                                                                       mt_call_subs_update.tech_id );
      (void)sp_subscription_mt_call_info_update(mt_call_subs_update);
  }
#endif
  
  mm_csfb_state = new_csfb_state;
  MSG_HIGH_DS(MM_SUB,"CSFB State set to %d",new_csfb_state,0,0);
}

/*===========================================================================

FUNCTION  mm_reset_mt_csfb_cco_state

DESCRIPTION: This function checks if present csfb state is mt csfb then
 it will set the CSFB state to LU pending if LU needs to performed after call
 end or sets it to NULL if no LU is required.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mm_reset_mt_csfb_cco_state()
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);

  if(mm_csfb_state == MM_CSFB_STATE_MT_CALL_CCO)
  {
    if((mm_serving_plmn.network_op_mode !=NETWORK_OP_MODE_1) &&
       (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI))
    {
      mm_csfb_set_state(MM_CSFB_STATE_LU_PENDING);
    }
    else
    {
      mm_csfb_set_state(MM_CSFB_STATE_NULL);
    }
  }
}



/*===========================================================================

FUNCTION  mm_csfb_start_mo_call_orig

DESCRIPTION : This function 
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mm_csfb_start_mo_call_orig(mm_cmd_type *msg_p)
{
  emm_csfb_rsp csfb_rsp;
  lte_csfb_service_type svc_type;
  lte_rrc_est_cause_e rrc_est_cause;

  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL); 

  //If MO or MT CSFB is going on, reject call non emergency calls
  if(((mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG) || 
       (mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG)) && 
      (msg_p->cmd.mmcnm_est_req.CNM_service_type != EMERGENCY_CALL_ESTABLISHMENT))
  {
    MSG_HIGH_DS(MM_SUB, "CSFB: Another FB procedure in progress, send EST_REJ - %d", mm_csfb_state, 0, 0);
    
  #ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
    mm_send_mmcnm_est_rej( MM_REJECT_CAUSE, MM_REJ_WRONG_STATE , MMCC_SMS_CS);
  #else
    mm_send_mmcnm_est_rej( MM_REJECT_CAUSE, MM_REJ_WRONG_STATE );
  #endif
  }
#ifdef FEATURE_LTE_REL10
  /* If T3442 or T3246 is running and it is not emergency call then reject the ESR procedure */
  else if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
     (msg_p->cmd.mmcnm_est_req.CNM_service_type != EMERGENCY_CALL_ESTABLISHMENT) &&
          ((mm_timer_status[TIMER_T3442] == TIMER_ACTIVE) ||
          ((mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
/*4.4.4.9 (j)Timer T3246 is running
The MM connection establishment shall not be initiated unless the MS is accessing the network with access class 11 – 15. */
#ifdef FEATURE_NAS_REL11
              &&(mm_is_rrc_access_class_11_to_15() == FALSE)
#endif
           )))
  {
    MSG_HIGH_DS(MM_SUB,"=EMM= TIMER T3442/T3246 is running - ESR cannot be started, TIMER_T3442 is %d TIMER_T3246 is %d",mm_timer_status[TIMER_T3442],
                                                                                                           mm_timer_status[TIMER_T3246],0);
  #ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
    mm_send_mmcnm_est_rej( MM_REJECT_CAUSE, MM_REJ_ACCESS_CLASS_BLOCKED , MMCC_SMS_CS);
  #else
    mm_send_mmcnm_est_rej( MM_REJECT_CAUSE, MM_REJ_ACCESS_CLASS_BLOCKED );
  #endif
  }
#endif
  else
  {
    MSG_HIGH_DS(MM_SUB,"CSFB: Received MO CSFB call",0,0,0);
    mm_csfb_set_state(MM_CSFB_STATE_MO_CALL_ORIG);
    emm_ctrl_data_ptr->rr_blind_redir_pending_call_abort = FALSE;

#ifdef FEATURE_MODEM_HEAP
    memscpy(mm_csfb_service_req_message_ptr, sizeof(mm_cmd_type), msg_p, sizeof(mm_cmd_type));
#else
    memscpy(&mm_csfb_service_req_message, sizeof(mm_cmd_type), msg_p, sizeof(mm_cmd_type));
#endif   

    if(msg_p->cmd.mmcnm_est_req.CNM_service_type == EMERGENCY_CALL_ESTABLISHMENT)
    {
      svc_type = LTE_MO_CSFB_ECALL_1XCSFB_ECALL;
      rrc_est_cause = LTE_RRC_EST_CAUSE_EMERGENCY;
    }
    else
    {
      svc_type = LTE_MO_CSFB_1xCSFB;
#ifdef FEATURE_LTE_REL10
      if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
      {
        rrc_est_cause = LTE_RRC_EST_CAUSE_MO_CSFB;
      }
      else
      {
      rrc_est_cause = LTE_RRC_EST_CAUSE_MO_DATA;
    }
#else
      rrc_est_cause = LTE_RRC_EST_CAUSE_MO_DATA;
#endif
    }
    mm_follow_on_request = TRUE;
    //Start ESR Procedure
    csfb_rsp.valid = FALSE;
    emm_start_nas_extended_service_request_procedure(rrc_est_cause, svc_type, csfb_rsp,NAS_EMM_3GPP_CSFB);
  }
}


/*===========================================================================

FUNCTION  mm_csfb_process_mt_page

DESCRIPTION: This function is called by EMM when it receives MT page  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mm_csfb_process_mt_page(lte_nas_paging_identity_type ue_page_id)
{  
  if(mm_location_information.location_update_status == UPDATED)
  {
    if((FALSE == mm_is_tmsi_valid(mm_location_information.tmsi,TMSI_SIZE)) &&
        (ue_page_id == EMM_CSFB_PAGINGI_TYPE_TMSI))
    {
      MSG_ERROR_DS(MM_SUB,"=EMM= TMSI is invalid, ignore the PAGE_IND",0,0,0);
      return FALSE;
    }        
  }
  else
  { 
    MSG_ERROR_DS(MM_SUB,"=EMM= TMSI does not exist, ignore the PAGE_IND",0,0,0);
    return FALSE;
  }

  if((mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)||
     (mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG))
  {
    return FALSE;     
  }

  mm_csfb_mt_call_info.ue_identity = ue_page_id;
  /*
  ** Set the higher priority flag to False for now, it will be set to TRUE
  ** if rrc connection release has the higher priority signalling flag set 
  ** to TRUE 
  */
  mm_csfb_mt_call_info.mt_csfb_highPriority = FALSE;
 
  mm_csfb_set_state(MM_CSFB_STATE_MT_CALL_ORIG);

  return TRUE;
}


/*===========================================================================

FUNCTION  mm_csfb_process_csfb_failure

DESCRIPTION: This function is called by EMM is ESR is not successful. For MO call reject is sent to CM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mm_csfb_process_csfb_failure(emm_ctrl_data_type* emm_ctrl_data_ptr, emm_csfb_failure_cause_type failure_cause_type)
{   

  reject_type_enum_T cause_type;
  reject_cause_T cause_value;

  boolean move_to_gw = FALSE;

  cause_type = AS_REJECT_CAUSE;
  cause_value =  AS_REJ_LRRC_LOWER_LAYER_FAILURE;

 
  //For ESR failure for emergency MO call send reject to CM for all causes. 
  // But if NV full_srv_req_in_emerg is enabled, then start PLMN selection in G/W.
  if((mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG) 
  #ifdef FEATURE_MODEM_HEAP    
       &&(mm_csfb_service_req_message_ptr->cmd.mmcnm_est_req.CNM_service_type 
              == EMERGENCY_CALL_ESTABLISHMENT)
  #else
       &&(mm_csfb_service_req_message.cmd.mmcnm_est_req.CNM_service_type 
              == EMERGENCY_CALL_ESTABLISHMENT) 
  #endif
       &&(full_srv_req_in_emerg == FALSE)
       )
  {        
 #ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
    mm_send_mmcnm_est_rej( AS_REJECT_CAUSE, AS_REJ_LRRC_LOWER_LAYER_FAILURE , MMCC_SMS_CS);
#else
    mm_send_mmcnm_est_rej( AS_REJECT_CAUSE, AS_REJ_LRRC_LOWER_LAYER_FAILURE );
#endif
    mm_follow_on_request = FALSE;
    mm_csfb_set_state(MM_CSFB_STATE_NULL);
  }  
  else if(mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG ||
           mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)
  {

    if(failure_cause_type.failure_type == OTA_REJECT_CAUSE)
    {
      MSG_HIGH_DS(MM_SUB,"CSFB: ESR reject with OTA cause type = %d", failure_cause_type.failure_cause.lte_nas_ota_emm_cause,0,0);
      switch(failure_cause_type.failure_cause.lte_nas_ota_emm_cause)
      {
        case LTE_NAS_ILLEGAL_UE: 
        case LTE_NAS_ILLEGAL_ME:
        case LTE_NAS_EPS_SERVICES_NOT_ALLOWED: 
        case LTE_NAS_UE_ID_CANNOT_BE_DERIVED_BY_NW: 
        case LTE_NAS_IMPLICITLY_DETACHED: 
        case LTE_NAS_PLMN_NOT_ALLOWED: 
        case LTE_NAS_TRACKING_AREA_NOT_ALLOWED: 
        case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA: 
        case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
        /* Added the following for the TAU reject resulting in the buffered ESR rejection. The TAU reject cause is passed as is to this function from EMM as the OTA cause*/
        case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
           move_to_gw = FALSE; 
           cause_type = EMM_REJECT_CAUSE;
           cause_value = EMM_REJ_SERVICE_REQ_FAILURE_LTE_NW_REJECT;
           break;
  
        case LTE_NAS_CS_DOMAIN_NOT_AVAILABLE:      
           move_to_gw = FALSE; 
           cause_type = EMM_REJECT_CAUSE;
           cause_value = EMM_REJ_SERVICE_REQ_FAILURE_CS_DOMAIN_NOT_AVAILABLE;
           break;

        case LTE_NAS_CS_DOMAIN_TEMP_NOT_AVAILABLE:  
#ifdef FEATURE_LTE_REL10 
           /* On reject cause #39 for an MO call send reject to CM with reason as access barred
           ** so that CM will not do silent redial. For MO and MT call don't initiate PLMN
           ** selection on GW through REG for this reject cause.
           */
           if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
           {   
             move_to_gw = FALSE; 
             cause_type = MM_REJECT_CAUSE;
             cause_value = MM_REJ_ACCESS_CLASS_BLOCKED;
           }
           else
#endif
           {
             move_to_gw = TRUE; 
           }
           break;
 
       case LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED:
#ifdef FEATURE_LTE_REL10            
           if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10)
           {   
             move_to_gw = TRUE;
           }
           else 
#endif
           {
             move_to_gw = FALSE; 
             cause_type = EMM_REJECT_CAUSE;
             cause_value = EMM_REJ_SERVICE_REQ_FAILURE_LTE_NW_REJECT;
           }
           break;
        default:
           move_to_gw = TRUE;          
           break;          
      }
    }
#ifdef FEATURE_LTE_REL10
    else if((mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG) &&
       (mm_timer_status[TIMER_T3346] == TIMER_ACTIVE))
    {
      if(mm_timer_status[TIMER_T3246] == TIMER_STOPPED)
      {
        move_to_gw = TRUE;
      }
    }
#endif
    else
    {
      MSG_HIGH_DS(MM_SUB,"CSFB: ESR reject with NON OTA cause type = %d", failure_cause_type.failure_cause.lte_nas_failure_cause,0,0);
      switch(failure_cause_type.failure_cause.lte_nas_failure_cause)
      {
        case EMM_T3417_EXT_EXPIRED: 
           if(mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)
           {             
             move_to_gw = TRUE;             
           }  
           else if (mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG)
           {
             move_to_gw = FALSE;
           }  
           break;

        case LTE_NAS_DETACH_WITH_REATTACH_LTE_NW_DETACH:
        case LTE_NAS_DETACH_WITH_OUT_REATTACH_LTE_NW_DETACH:
           move_to_gw = FALSE; 
           cause_type = EMM_REJECT_CAUSE;
           cause_value = EMM_REJ_SERVICE_REQ_FAILURE_LTE_NW_REJECT;
           break;

        case LTE_NAS_LRRC_CONN_EST_FAILURE_ACCESS_BARRED:
#ifdef FEATURE_NAS_EAB
        case LTE_NAS_LRRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB:
#endif
#ifdef FEATURE_LTE_REL10
          if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL10) &&
             (emm_connection_establishment_barred(LTE_RRC_EST_CAUSE_MO_CSFB,
                                                  NULL, 
                                                  emm_ctrl_data_ptr) == TRUE))
 
          {
            /* Stay in LTE is only MO CSFB access barring applies */
            move_to_gw = FALSE;  
            cause_type = AS_REJECT_CAUSE;
#ifdef FEATURE_NAS_EAB
            if(emm_chk_eab_barring(LTE_RRC_EST_CAUSE_MO_CSFB, emm_ctrl_data_ptr) == TRUE)
            {
              cause_value = CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB;
            }
            else
#endif
            {
              cause_value = AS_REJ_LRRC_CONN_EST_FAILURE_ACCESS_BARRED;
            }
          }
          else
#endif 
          {
            move_to_gw = TRUE; 
          }
          break;

        case LTE_NAS_LRRC_CONN_REL_NORMAL:   
           if(((EMM_GET_SUBSTATE()==EMM_REGISTERED_NORMAL_SERVICE) && 
              (EMM_GET_CONNECTION_STATE() == EMM_RELEASING_RRC_CONNECTION_STATE))&&
              !((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED) && 
                (EMM_GET_STATE()== EMM_TRACKING_AREA_UPDATING_INITIATED)))	
                 /*If connection is released when ESR is buffered due to ongoing TAU
                     ,tuneaway to GW by setting move_to_gw flag*/
           {
             cause_type = AS_REJECT_CAUSE;
             cause_value=AS_REJ_RRC_REL_IND;
             move_to_gw = FALSE;
           }
           else
           {
             move_to_gw =TRUE;
           }
           break;

        case LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_CONN_REL:
           move_to_gw = FALSE; 
           cause_type = AS_REJECT_CAUSE;
           cause_value = AS_REJ_LRRC_UL_DATA_CNF_FAILURE_CONN_REL;
           break;
 
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
      case LTE_NAS_LRRC_CONN_EST_FAILURE_RF_UNAVAILABLE:
      case LTE_NAS_REJ_NO_SERVICE_TRM_UNAVAILABLE:
        move_to_gw = FALSE; 
        cause_type = AS_REJECT_CAUSE;
        cause_value = AS_REJ_ABORT_RADIO_UNAVAILABLE;
        break;
#endif 

        default:
           move_to_gw = TRUE;          
           break;
      }
    }
    /* ASK REG to start PLMN selection on GW */
    
    if(move_to_gw == TRUE)
    {
      /*False case is taken care in service reject cause handling*/
      reg_service_report(MMR_CAUSE_INITIATE_PLMN_SELECTION_ON_GW, TRUE, emm_ctrl_data_ptr);
      if (full_srv_req_in_emerg == TRUE)
	  {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
        byte diag_event[1];
		diag_event[0] = mm_as_id;
        event_report_payload(EVENT_FULL_SRV_EMC_CALL,(uint8)1,(void*)diag_event);
#else
        event_report_payload(EVENT_FULL_SRV_EMC_CALL,(uint8)0,(void*)NULL); 
#endif
      }
    }
    else
    {
      if(mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)
      {    
 #ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
        mm_send_mmcnm_est_rej( cause_type, cause_value , MMCC_SMS_CS);
#else
        mm_send_mmcnm_est_rej( cause_type, cause_value );
#endif
       mm_follow_on_request = FALSE;      
      }
      else if(mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG)
      {
        mm_send_cm_mt_call_fail_ind(CM_MM_MT_CALL_FAIL_CAUSE_ESR_FAILURE);
      }
      mm_csfb_set_state(MM_CSFB_STATE_NULL);   
    }
    mm_csfb_mt_call_info.is_mt_call_recv_in_conn_state = FALSE;
  }
}

/*===========================================================================

FUNCTION  mm_csfb_rej_pending_call

DESCRIPTION: This function will rejct the pending CSFB call and clear CSFB states.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mm_csfb_rej_pending_call(void)
{  
   emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
   emm_ctrl_data_ptr = emm_db_get_ctrl_data();
   ASSERT(emm_ctrl_data_ptr != NULL);

  if(mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)
  {
 #ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
    mm_send_mmcnm_est_rej( MM_REJECT_CAUSE, MM_REJ_WRONG_STATE, MMCC_SMS_CS);
 #else
    mm_send_mmcnm_est_rej( MM_REJECT_CAUSE, MM_REJ_WRONG_STATE );
  #endif
    mm_follow_on_request = FALSE;
    mm_csfb_set_state(MM_CSFB_STATE_NULL);
  } 
  else if(mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG)
  {
    mm_csfb_set_state(MM_CSFB_STATE_NULL);
  }
  else 
  {
    /*Whenever pending call is rejected due to other higher priority request 
      like MMR_REG_REQ or PS_DETACH_REQ then we need to set the csfb state correctly
      so as to perform LU if needed. */
    mm_reset_mt_csfb_cco_state();
  }
  /*calling this function to clean up the pended ESR if any during the processing of rejection of pended CSFB call*/
  emm_abort_and_clean_csfb_call(emm_ctrl_data_ptr);
}


/*===========================================================================

FUNCTION  mm_csfb_clear_csfb_call

DESCRIPTION: 
  This function will clear variables that were set for CSFB call 
  and also sets CSFB state to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mm_csfb_clear_csfb_call(void)
{
  mm_csfb_set_state(MM_CSFB_STATE_NULL);
  mm_follow_on_request = FALSE;
  emm_ctrl_data_ptr->rr_blind_redir_pending_call_abort = FALSE;
}


/*===========================================================================

FUNCTION  mm_csfb_get_state

DESCRIPTION: 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
mm_csfb_state_type mm_csfb_get_state(void)
{
  return mm_csfb_state;
}

/*===========================================================================

FUNCTION  emm_process_csfb_failure

DESCRIPTION: This function is used by both 3GPP CSFB & 1XCSFB

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_process_multimode_csfb_failure(emm_ctrl_data_type* emm_ctrl_data_ptr, emm_csfb_failure_cause_type failure_cause_type)
{
  reject_type_enum_T cause_type;
  reject_cause_T cause_value = REJ_UNSPECIFIED;   

  ASSERT(emm_ctrl_data_ptr != NULL);
  
  MSG_HIGH_DS(MM_SUB,"ESR failed for CSFB call, ESR type = %d, CSFB failure cause_type = %d", emm_ctrl_data_ptr->esr_type_in_progress,failure_cause_type.failure_type,0);

  if(failure_cause_type.failure_type != ONEX_CSFB_REJECT_CAUSE)
  {
    if(failure_cause_type.failure_type != OTA_REJECT_CAUSE)
    {
      switch(failure_cause_type.failure_cause.lte_nas_failure_cause)
      {
        case EMM_T3417_EXT_EXPIRED:
          cause_type = EMM_REJECT_CAUSE;
          cause_value =  EMM_REJ_TIMER_T3417_EXT_EXP;
          break;

        case NAS_SIG_CONN_RELEASED:
          cause_value =  AS_REJ_LRRC_LOWER_LAYER_FAILURE;         
          cause_type = AS_REJECT_CAUSE;
          break;

        case EMM_T3417_EXPIRED:
          cause_type = EMM_REJECT_CAUSE;
          cause_value =  EMM_REJ_TIMER_T3417_EXP;
          break;

        case LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_TXN:
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_UL_DATA_CNF_FAILURE_TXN;
          break;
        case LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_HO:
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_UL_DATA_CNF_FAILURE_HO;
          break;
        case LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_CONN_REL:
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_UL_DATA_CNF_FAILURE_CONN_REL;
          break;
        case LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_RLF:
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_UL_DATA_CNF_FAILURE_RLF;
          break;
        case LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN:
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN;
          break;
        case LTE_NAS_LRRC_CONN_EST_FAILURE:
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_CONN_EST_FAILURE;
          break;
        case LTE_NAS_LRRC_CONN_EST_FAILURE_ABORTED:         
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_CONN_EST_FAILURE_ABORTED;
          break;
        case LTE_NAS_LRRC_CONN_EST_FAILURE_ACCESS_BARRED:  
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_CONN_EST_FAILURE_ACCESS_BARRED;
          break;
#ifdef FEATURE_NAS_EAB
        case LTE_NAS_LRRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB:  
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB;
          break;
#endif
        case LTE_NAS_LRRC_CONN_EST_FAILURE_CELL_RESEL:      
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_CONN_EST_FAILURE_CELL_RESEL;
          break;
        case LTE_NAS_LRRC_CONN_EST_FAILURE_CONFIG_FAILURE:  
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_CONN_EST_FAILURE_CONFIG_FAILURE;
          break;
        case LTE_NAS_LRRC_CONN_EST_FAILURE_LINK_FAILURE:    
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_CONN_EST_FAILURE_LINK_FAILURE;
          break;
        case LTE_NAS_LRRC_CONN_EST_FAILURE_NOT_CAMPED:      
          cause_type = AS_REJECT_CAUSE;
          cause_value = AS_REJ_LRRC_CONN_EST_FAILURE_NOT_CAMPED;
          break;
        case LTE_NAS_LRRC_CONN_EST_FAILURE_SI_FAILURE:      
          cause_type = AS_REJECT_CAUSE;
          cause_value = AS_REJ_LRRC_CONN_EST_FAILURE_SI_FAILURE;
          break;
        case LTE_NAS_LRRC_CONN_EST_FAILURE_CONN_REJECT:  
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_CONN_EST_FAILURE_CONN_REJECT;
          break;
        case LTE_NAS_LRRC_CONN_EST_FAILURE_TIMER_EXPIRED:
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_CONN_EST_FAILURE_TIMER_EXPIRED;
          break;
       case LTE_NAS_LRRC_CONN_REL_NORMAL:       
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_CONN_REL_NORMAL;
          break;
        case LTE_NAS_LRRC_CONN_REL_RLF:                     
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_CONN_REL_RLF;
          break;
        case LTE_NAS_LRRC_CONN_REL_CRE_FAILURE:            
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_CONN_REL_CRE_FAILURE;
          break;
        case LTE_NAS_LRRC_CONN_REL_OOS_DURING_CRE:         
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_CONN_REL_OOS_DURING_CRE;
          break;
        case LTE_NAS_LRRC_CONN_REL_ABORTED:                 
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_CONN_REL_ABORTED;
          break;
        case LTE_NAS_LRRC_CONN_REL_SIB_READ_ERROR:         
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_CONN_REL_SIB_READ_ERROR;
          break;
       case LTE_NAS_DETACH_WITH_REATTACH_LTE_NW_DETACH:
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_DETACH_WITH_REATTACH_LTE_NW_DETACH;
          break;
      case LTE_NAS_DETACH_WITH_OUT_REATTACH_LTE_NW_DETACH:
        cause_type = AS_REJECT_CAUSE;
        cause_value = AS_REJ_DETACH_WITH_OUT_REATTACH_LTE_NW_DETACH;
        break;
      case LTE_NAS_REJ_LRRC_RADIO_LINK_FAILURE:
        cause_type = AS_REJECT_CAUSE;
        cause_value =  AS_REJ_LRRC_RADIO_LINK_FAILURE;
        break;
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
      case LTE_NAS_LRRC_CONN_EST_FAILURE_RF_UNAVAILABLE:
        cause_type = AS_REJECT_CAUSE;
        cause_value = AS_REJ_ABORT_RADIO_UNAVAILABLE;
        break;
#endif 
       default:
          cause_type = AS_REJECT_CAUSE;
          cause_value =  AS_REJ_LRRC_LOWER_LAYER_FAILURE;
          break;
      } 

    }/* OTA reject cause*/
    else
    {
      cause_type = EMM_REJECT_CAUSE;
      cause_value =  EMM_REJ_SERVICE_REQ_FAILURE_LTE_NW_REJECT;
    }
  }/* 1X reject case*/
  else
  {
    cause_type = ONEX_CSFB_REJECT_CAUSE;
    cause_value = failure_cause_type.failure_cause.onex_failure_cause;
  }
  MSG_HIGH_DS(MM_SUB, "CSFB: ESR failed, sending reject to CM for: type= %d cause = %d", cause_type, cause_value, 0);

  if( ((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB) || 
      (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_3GPP_CSFB_BUFFERED)) && 
      (failure_cause_type.failure_cause.lte_nas_failure_cause != LTE_NAS_LRRC_CONN_EST_FAILURE_CELL_RESEL ) )
  {
    mm_csfb_process_csfb_failure(emm_ctrl_data_ptr, failure_cause_type);
  }

  if((emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1xCSFB) 
     || (emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_1X_CSFB_BUFFERED) )
  {
      /* Skip TAU if OTA msg for ESR is not sent */
    if (failure_cause_type.failure_cause.lte_nas_failure_cause != LTE_NAS_LRRC_CONN_EST_FAILURE_NOT_CAMPED &&
       failure_cause_type.failure_cause.lte_nas_failure_cause != LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_CONN_REL &&
       failure_cause_type.failure_cause.lte_nas_failure_cause != LTE_NAS_LRRC_CONN_EST_FAILURE &&
       failure_cause_type.failure_cause.lte_nas_failure_cause != LTE_NAS_LRRC_CONN_EST_FAILURE_ABORTED &&
       failure_cause_type.failure_cause.lte_nas_failure_cause != LTE_NAS_LRRC_CONN_EST_FAILURE_ACCESS_BARRED &&
#ifdef FEATURE_NAS_EAB
       failure_cause_type.failure_cause.lte_nas_failure_cause != LTE_NAS_LRRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB &&
#endif
       failure_cause_type.failure_cause.lte_nas_failure_cause != LTE_NAS_LRRC_CONN_EST_FAILURE_CELL_RESEL &&
       failure_cause_type.failure_cause.lte_nas_failure_cause != LTE_NAS_LRRC_CONN_EST_FAILURE_CONFIG_FAILURE &&
       failure_cause_type.failure_cause.lte_nas_failure_cause != LTE_NAS_LRRC_CONN_EST_FAILURE_LINK_FAILURE &&
       failure_cause_type.failure_cause.lte_nas_failure_cause != LTE_NAS_LRRC_CONN_EST_FAILURE_NOT_CAMPED &&
       failure_cause_type.failure_cause.lte_nas_failure_cause != LTE_NAS_LRRC_CONN_EST_FAILURE_SI_FAILURE &&
       failure_cause_type.failure_cause.lte_nas_failure_cause != LTE_NAS_LRRC_CONN_EST_FAILURE_CONN_REJECT &&
       failure_cause_type.failure_cause.lte_nas_failure_cause != LTE_NAS_LRRC_CONN_EST_FAILURE_TIMER_EXPIRED &&
       failure_cause_type.failure_cause.lte_nas_failure_cause != LTE_NAS_LRRC_CONN_EST_FAILURE_TAI_CHANGED 
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
       && failure_cause_type.failure_cause.lte_nas_failure_cause != LTE_NAS_LRRC_CONN_EST_FAILURE_RF_UNAVAILABLE
#endif
      )
    {
#ifdef FEATURE_1XSRLTE
      if(((emm_ue_is_in_srlte_state(emm_ctrl_data_ptr) == TRUE)
          ||(emm_ctrl_data_ptr->partial_esr == TRUE))
          &&(emm_ctrl_data_ptr->srlte_tau_needed == TRUE))
      {
        emm_ctrl_data_ptr->emm_update_status = EMM_STATUS_NOT_UPDATED;
      }
      else
#endif
      {
        // For TX failure/ connection establishment failure cases, so not trigger TAU
        /* setting tau cause to others will start TAU on coming back to LTE */
        emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_OTHERS;
        MSG_HIGH_DS_1(MM_SUB,"Need to initiate after 1X failure for failure casue %d",failure_cause_type.failure_cause.lte_nas_failure_cause);
      }
    }
#ifdef FEATURE_1XSRLTE
    else
    {
      emm_ctrl_data_ptr->srlte_tau_needed = FALSE;
    }
#endif
    emm_build_and_send_esr_failure_ind(cause_type,cause_value,emm_ctrl_data_ptr->esm_srv_req_trans_id);
  }
}


/*===========================================================================
FUNCTION    emm_lte_cs_domain_param_match

DESCRIPTION
  This function compares lte_cs_domain_param.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if match is successful, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_lte_cs_domain_param_match
(
  const sys_lte_cs_domain_param_s_type lte_cs_domain_param_from_reg, 
  const sys_lte_cs_domain_param_s_type lte_cs_domain_param_in_emm
)
{
  /* Return successful match if is_voice_domain_ie_valid
     is FALSE in lte_cs_domain_param in MMR_REG_REQ */
  if((lte_cs_domain_param_from_reg.is_voice_domain_ie_valid == FALSE) 
     ||
     ((is_umts_mode() || is_gsm_mode()) &&
      ((gmm_state == GMM_DEREGISTERED)||
       (gmm_state == GMM_NULL) ||
       (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                        SYS_RAT_LTE_RADIO_ACCESS) == FALSE)))
     ||
     (is_lte_mode() &&
      ((EMM_GET_STATE() == EMM_DEREGISTERED)||
       (EMM_GET_STATE() == EMM_NULL)))
     )
  {
    return TRUE;
  }
  else if(lte_cs_domain_param_in_emm.is_voice_domain_ie_valid == TRUE)
  {
    if((lte_cs_domain_param_from_reg.voice_or_data_centric !=
        lte_cs_domain_param_in_emm.voice_or_data_centric) ||
       (lte_cs_domain_param_from_reg.voice_domain_pref !=
        lte_cs_domain_param_in_emm.voice_domain_pref))
    {
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
  return TRUE;
}


/*===========================================================================

FUNCTION EMM_COPY_FTAI_LIST_TO_MANUAL_LIST

DESCRIPTION
  This function copy manual ftai list from ftai list
  and update same to the LTE RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void emm_copy_ftai_list_to_manual_ftai_list( emm_ctrl_data_type* emm_ctrl_data_ptr)
{
  uint8 num_of_entries = 0;
  uint8 index = 0;
  ASSERT(emm_ctrl_data_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->rrc_forbidden_list_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->manual_ftai_list_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr->rrc_forbidden_list_ptr->tai_lst_length <= 
                                              LTE_RRC_MAX_NUM_TRACKING_AREAS);
  num_of_entries = emm_ctrl_data_ptr->rrc_forbidden_list_ptr->tai_lst_length;
  MSG_HIGH_DS(MM_SUB,"=EMM= Populating Manual FTAI from FTAI list of length %d",num_of_entries,0,0);
  /*copying the manual FTAI list from FTAI list copy fresh list 
    from beginning*/
  for(index = 0; index < num_of_entries;index++)
  {
    emm_ctrl_data_ptr->manual_ftai_list_ptr->diff_plmn[index].plmn =
                        emm_ctrl_data_ptr->rrc_forbidden_list_ptr->diff_plmn[index].plmn;
    emm_ctrl_data_ptr->manual_ftai_list_ptr->diff_plmn[index].tac = 
                      emm_ctrl_data_ptr->rrc_forbidden_list_ptr->diff_plmn[index].tac;
  }  
  emm_ctrl_data_ptr->manual_ftai_list_ptr->tai_lst_length = num_of_entries;
  /* Sending System Update to LTE RRC so that it can update
   manual ftai list  its list */
  if (num_of_entries >0 )
  {
    emm_send_rrc_system_update(emm_ctrl_data_ptr);
  }
}




/*===========================================================================

FUNCTION  mm_set_emm_manual_selection_flag

DESCRIPTION: This used by MM to set EMM manual user selection flag. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mm_set_emm_manual_selection_flag(boolean value)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);

  MSG_HIGH_DS(MM_SUB,"=EMM= EMM Manual user PLMN selection set to %d",value,0,0);
  /* checking if the complete manual selection has finished
     then only copy the manual ftai list from ftai list Avoid unnecessary sending */
  if (mm_is_multimode_sub() &&(value == FALSE)
                   &&(emm_ctrl_data_ptr->manual_user_sel == TRUE))
  {
    emm_copy_ftai_list_to_manual_ftai_list(emm_ctrl_data_ptr);
  }
  emm_ctrl_data_ptr->manual_user_sel = value;  
}

/*===========================================================================

FUNCTION  emm_set_mm_manual_selection_flag

DESCRIPTION: This function is used by EMM to set MM manual selection flag

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void emm_set_mm_manual_selection_flag(boolean value)
{ 
  MSG_HIGH_DS(MM_SUB,"=MM= MM/GMM Manual user PLMN selection set to %d",value,0,0); 
   /* copy for the case of manual selection has finished and also to avoid the case
    when unnecessary  setting of flag false */ 

  if( value == FALSE && (gmm_manual_mode_user_selection_procedure == TRUE))
  {
    mm_copy_lau_reject_list_to_forbidden_list();
  } 
  gmm_manual_mode_user_selection_procedure = value;
  mm_manual_mode_user_selection_procedure = value;  
}

/*===========================================================================

FUNCTION  emm_set_mm_ps_detach_state

DESCRIPTION: This function is used by EMM to set MM PS DETACH STATE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_set_mm_ps_detach_state
(
  mm_ps_detach_state_enum_T  state
)
{ 
  MSG_HIGH_DS(MM_SUB,"=EMM= Set MM PS DETACH STATE - %d", state,0,0); 
  mm_ps_detach_state = state;

  if (mm_ps_detach_state == MM_PS_DETACH_STATE_NONE)
  {
    mm_ps_detach_type  = SYS_PS_DETACH_TYPE_NONE;
  }
}

/*===========================================================================

FUNCTION  emm_set_lte_deact_isr_timer_expired

DESCRIPTION: This function is used by EMM to set "gmm_lte_deact_isr_timer_expired"

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_set_lte_deact_isr_timer_expired(boolean expired_status)
{
  MSG_HIGH_DS(MM_SUB,"=EMM= Set gmm_lte_deact_isr_timer_expired - %d", expired_status,0,0); 
  gmm_lte_deact_isr_timer_expired = expired_status;
}


/*===========================================================================

FUNCTION  emm_set_gmm_drx_cn_coff_s1

DESCRIPTION: This function is used by EMM to set "gmm_drx_cn_coeff_s1"

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_set_gmm_drx_cn_coff_s1(sys_drx_cn_coefficient_s1_e_type drx_value)
{
  gmm_drx_cn_coeff_s1 = drx_value;
}

/*===========================================================================

FUNCTION  emm_get_gmm_drx_cn_coff_s1

DESCRIPTION: This function is used by EMM to get "gmm_drx_cn_coeff_s1"

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
sys_drx_cn_coefficient_s1_e_type emm_get_gmm_drx_cn_coff_s1(void)
{
  return gmm_drx_cn_coeff_s1;
}

#ifdef FEATURE_LTE_REL9

/*===========================================================================

FUNCTION  mm_emm_is_mmit_feature_enabled

DESCRIPTION: 
  This function checks if mobility management for IMS voice Termination(MMIT) is enabled.

DEPENDENCIES
  None

RETURN VALUE
  Return boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean mm_emm_is_mmit_enabled(void)
{
  if(mm_is_multimode_sub())
  {
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);
    ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);
  /*
  ** An MS activates mobility management for IMS voice termination when:
  **  1) the MS's availability for voice calls in the IMS (see 3GPP TS 24.301 [120], subclause 3.1) 
  **      changes from "not available" to "available";
  **  2) the MS is configured with "Mobility Management for IMS Voice Termination" enabled as defined in
  **      3GPP TS 24.167 [134];
  **  3) the IMS voice over PS session indicator received for Iu mode has the value
  **        "IMS voice over PS session supported in Iu mode, but not supported in A/Gb mode", or
  **      the IMS voice over PS session indicator received for S1 mode has the value
  **        "IMS voice over PS session in S1 mode supported"; and
  **  4) at least one of the two parameters voice domain preference for UTRAN and voice domain 
  **      preference forE-UTRAN as defined in 3GPP TS 24.167 [134] is not "CS voice only".
  */
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9) &&
     (emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.is_ims_reg_for_voice_tag) &&
     (emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.mm_voims_dm_mo) &&
     (((emm_ctrl_data_ptr->mmit_parameters.eutran_ps_voice_parameters.is_last_eutran_voims_supported)&&
      (emm_ctrl_data_ptr->mmit_parameters.eutran_ps_voice_parameters.is_ps_voice_on_eutran_supported)) ||
      ((emm_ctrl_data_ptr->mmit_parameters.utran_ps_voice_parameters_ptr->is_last_utran_voims_supported)&&
       (emm_ctrl_data_ptr->mmit_parameters.utran_ps_voice_parameters_ptr->is_ps_voice_on_utran_supported))) &&
     (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI) &&
       (emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == SYS_SRV_DOMAIN_CS_PS))
  {
    return TRUE;
  }
  }
  return FALSE;
}

/*===========================================================================

FUNCTION  mm_emm_mmit_eval_on_irat_between_lte_and_umts

DESCRIPTION: 
  This function checks if TAU on LTE or RAU on UMTS is needed to be 
  performed for mobility management for IMS voice Termination(MMIT)

DEPENDENCIES
  None

RETURN VALUE
  Return boolean

SIDE EFFECTS
  None
===========================================================================*/

boolean mm_emm_mmit_eval_on_irat_between_lte_and_umts(void)
{
  if(mm_is_multimode_sub())
  {
  boolean is_ue_network_support_ps_voice_eutran = FALSE;
  boolean is_ue_network_support_ps_voice_utran = FALSE;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);
    ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);

  is_ue_network_support_ps_voice_eutran = 
         ((emm_ctrl_data_ptr->mmit_parameters.eutran_ps_voice_parameters.is_last_eutran_voims_supported)&&
          (emm_ctrl_data_ptr->mmit_parameters.eutran_ps_voice_parameters.is_ps_voice_on_eutran_supported));

  is_ue_network_support_ps_voice_utran = 
         ((emm_ctrl_data_ptr->mmit_parameters.utran_ps_voice_parameters_ptr->is_last_utran_voims_supported)&&
          (emm_ctrl_data_ptr->mmit_parameters.utran_ps_voice_parameters_ptr->is_ps_voice_on_utran_supported));

  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9) &&
     (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI) &&
     (emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.is_ims_reg_for_voice_tag) &&
     (emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.mm_voims_dm_mo) &&
     (is_ue_network_support_ps_voice_eutran != is_ue_network_support_ps_voice_utran) &&
       (emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == SYS_SRV_DOMAIN_CS_PS))
  {
    return TRUE;
  }
  }
  return FALSE;
}

/*===========================================================================

FUNCTION  mm_emm_mmit_eval_on_irat_between_lte_and_gsm

DESCRIPTION: 
  This function checks if TAU on LTE or RAU on GSM is needed to be  performed for 
  mobility management for IMS voice Termination(MMIT)

DEPENDENCIES
  None

RETURN VALUE
  Return boolean

SIDE EFFECTS
  None
===========================================================================*/

boolean mm_emm_mmit_eval_on_irat_between_lte_and_gsm(void)
{
  if(mm_is_multimode_sub())
  {
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);
    ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);

  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9) &&
     (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI) &&
     (emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.is_ims_reg_for_voice_tag) &&
     (emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.mm_voims_dm_mo) &&
     ((emm_ctrl_data_ptr->mmit_parameters.eutran_ps_voice_parameters.is_last_eutran_voims_supported)&&
      (emm_ctrl_data_ptr->mmit_parameters.eutran_ps_voice_parameters.is_ps_voice_on_eutran_supported)) &&
       (emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain == SYS_SRV_DOMAIN_CS_PS))
  {
    return TRUE;
  }
  }
  return FALSE;
}

/*===========================================================================

FUNCTION  mm_emm_is_reg_needed_for_mmit_on_LTE

DESCRIPTION: 
  This function checks if TAU on LTE is needed to be  performed for 
  mobility management for IMS voice Termination(MMIT).
  This function uses "mm_emm_mmit_eval_on_irat_between_lte_and_umts" and
  "mm_emm_mmit_eval_on_irat_between_lte_and_gsm" functions to dervie the outcome.

DEPENDENCIES
  None

RETURN VALUE
  Return boolean

SIDE EFFECTS
  None
===========================================================================*/

boolean mm_is_reg_needed_for_mmit_on_LTE(sys_radio_access_tech_e_type previous_rat)
{
  if(((previous_rat == SYS_RAT_GSM_RADIO_ACCESS) && mm_emm_mmit_eval_on_irat_between_lte_and_gsm())||
     ((previous_rat == SYS_RAT_UMTS_RADIO_ACCESS) && mm_emm_mmit_eval_on_irat_between_lte_and_umts()))
  {
    return TRUE;
  }
  return FALSE;
}


/*===========================================================================

FUNCTION  mm_emm_is_reg_needed_for_mmit_on_GW

DESCRIPTION: 
  This function checks if RAU on GSM/UMTS is needed to be  performed for 
  mobility management for IMS voice Termination(MMIT).
  This function uses "mm_emm_mmit_eval_on_irat_between_lte_and_umts" and
  "mm_emm_mmit_eval_on_irat_between_lte_and_gsm" functions to dervie the outcome.

DEPENDENCIES
  None

RETURN VALUE
  Return boolean

SIDE EFFECTS
  None
===========================================================================*/

boolean mm_is_reg_needed_for_mmit_on_GW(void)
{
  if((is_gsm_mode() && mm_emm_mmit_eval_on_irat_between_lte_and_gsm())||
     (is_umts_mode() && mm_emm_mmit_eval_on_irat_between_lte_and_umts()) )
  {
    return TRUE;
  }
  return FALSE;
}

#endif
/*===========================================================================

FUNCTION  emm_set_gmm_manual_selection_flag

DESCRIPTION: This function is used by EMM to set GMM manual selection flag

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void emm_set_gmm_manual_selection_flag(boolean value)
{ 
  MSG_HIGH_DS(MM_SUB,"=EMM= GMM Manual user PLMN selection set to %d",value,0,0); 
  gmm_manual_mode_user_selection_procedure = value;
}


#ifdef FEATURE_LTE_REL9
/*===========================================================================

FUNCTION  GMM_HANDLE_EMC_PS_SRV

DESCRIPTION: This function handles the emergency bearer service established 
             on LTE in L to G/W/TD RAT change scenarios

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  gmm_handle_emc_ps_srv
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  ASSERT(emm_ctrl_data_ptr != NULL);

  if((gmm_state == GMM_REGISTERED) &&
     (emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY))
{
    gmm_set_state(GMM_DEREGISTERED);
    /* Delete EPLMN list */
    mm_serving_plmn.update_equiv_plmn_list = TRUE;
    memset(&mm_serving_plmn.equiv_plmn_list, 0x00, sizeof(equiv_plmn_list_s_type));
  }
}/* gmm_handle_emc_ps_srv */
#endif

/*===========================================================================

FUNCTION  mm_reset_ul_count_at_generation

DESCRIPTION: This function is used by EMM to reset the nas uplink count at generation 
             which resides inside the mm global data structure mm_mapped_gw_identity

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mm_reset_ul_count_at_generation(void)
{
 memset(mm_mapped_gw_identity.ul_count_at_generation,0xff,
              sizeof(mm_mapped_gw_identity.ul_count_at_generation));
}

/*===========================================================================

FUNCTION  gmm_invalidate_parameters

DESCRIPTION: A function to invalidate P-TMSI, P-TMSI signature, RAI and GPRS ciphering key sequence number. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void gmm_invalidate_parameters(void)
{
  gmm_update_status = GMM_GU2_NOT_UPDATED;
  gmm_delete_ps_location_information();
  gmm_delete_gprs_cksn();
  mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                      gmm_stored_ptmsi_signature,
                                      gmm_stored_rai,
                                      gmm_update_status );
}

/*===========================================================================

FUNCTION  mm_is_ue_camped_on_gw

DESCRIPTION: This function returns TRUE if UE is camped for full service on GW 

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/

boolean mm_is_ue_camped_on_gw(void)
{
  if((is_umts_mode() || is_gsm_mode()) &&
     (mm_serving_plmn.info.service_status == SYS_SRV_STATUS_SRV))
  {
    return TRUE;
  }
  return FALSE;
}

#ifdef FEATURE_1XSRLTE
/*===========================================================================

FUNCTION  emm_get_current_ue_mode

DESCRIPTION: 
 
This function returns current UE mode 
  
DEPENDENCIES:
  None

RETURN VALUE
  Return sys_ue_mode_e_type

SIDE EFFECTS
  None

===========================================================================*/
sys_ue_mode_e_type emm_get_current_ue_mode(void)
{
    MSG_HIGH_DS(MM_SUB,"=MM= current ue mode = %d ",mm_current_ue_mode,0,0);
    return mm_current_ue_mode;
}
#endif


/*===========================================================================

FUNCTION  gmm_set_gmm_is_congestion_due_to_rau

DESCRIPTION: Interface function between GW and LTE to set value for gmm_is_congestion_due_to_rau

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/

#ifdef FEATURE_NAS_REL10
void gmm_set_gmm_is_congestion_due_to_rau(boolean value)
{
  MSG_HIGH_DS_1(MM_SUB,"Setting gmm_is_congestion_due_to_rau to %d",value);
  gmm_is_congestion_due_to_rau = value;
}
#endif


/*===========================================================================

FUNCTION  gmm_force_set_state

DESCRIPTION: 
 
This function forcefully sets GMM state and substate only when LTE is not active
  
DEPENDENCIES:
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void gmm_force_set_state
(
  gmm_state_type     gmm_state_arg, 
  gmm_substate_type  gmm_sub_state
)
{
  //Set only when LTE is not active
  if(FALSE==is_lte_mode())
  {
    gmm_set_state( gmm_state_arg );
    gmm_set_substate( gmm_sub_state ); 
  }
}
/* gmm_force_set_state */


/*===========================================================================

FUNCTION  mm_set_plmn_search_in_progress_flag

DESCRIPTION: 
 
This function sets MM variable mm_plmn_search_in_progress
  
DEPENDENCIES:
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_set_plmn_search_in_progress_flag(boolean value)
{
  MSG_HIGH_DS_1(MM_SUB,"=MM= Setting mm_plmn_search_in_progress to %d",value);
  mm_plmn_search_in_progress = value;
}



#ifdef FEATURE_SGLTE
mm_as_id_e_type mm_get_ps_stack_id(void)
{
  return mm_ps_stack_id;
}
mm_as_id_e_type mm_get_cs_stack_id(void)
{
  return mm_cs_stack_id;
}

boolean mm_is_sglteplmn_in_sglte_roam(sys_plmn_id_s_type  pPlmnId)
{
  boolean val = FALSE;
  boolean result = FALSE;
  policyman_status_t out_put = POLICYMAN_STATUS_SUCCESS;

  if(!MM_SUB_IS_SGLTE)
  {
    return FALSE;
  }

  out_put = policyman_is_sglte_allowed(&val);
  MSG_HIGH_DS(MM_SUB,"=MM= The val of policyman_is_sglte_allowed = %d ",val,0,0);
  if (((out_put==POLICYMAN_STATUS_SUCCESS) && (val==TRUE))&&
      (mm_current_ue_mode == SYS_UE_MODE_SGLTE_ROAM ) && 
      (mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_GSM_RADIO_ACCESS)==TRUE))
   {
     out_put= policyman_sglte_plmn_is_sglte( &pPlmnId,&val);
     if ((out_put == POLICYMAN_STATUS_SUCCESS) && (val==TRUE))
       {
         result = TRUE;
         MSG_HIGH_DS(MM_SUB,"=MM= Found SGLTE PLMN in SGLTE ROAM mode ",0,0,0); 
       }
      else
      {
        result = FALSE;
      }
   } 
  return result; 
}
boolean mm_is_non_sglte_plmn_in_sglte_home(sys_plmn_id_s_type  pPlmnId)
{
  boolean val = FALSE;
  policyman_status_t out_put = POLICYMAN_STATUS_SUCCESS;
  boolean result= FALSE;

  if(!MM_SUB_IS_SGLTE)
  {
    return FALSE;
  }

  if (mm_current_ue_mode == SYS_UE_MODE_SGLTE_TEST_ONLY )
  {
    MSG_HIGH_DS(MM_SUB, "=MM= CM_UE_MODE_SGLTE_TEST_ONLY not blocking registration ",0,0,0) ;
    return result;
  }
  if(MM_IS_IN_SGLTE_STATE_4)
  {
    out_put =  policyman_sglte_plmn_is_sglte( &pPlmnId,&val);
    if ((out_put == POLICYMAN_STATUS_SUCCESS)&&(val==FALSE))
    {
       result = TRUE;
       MSG_HIGH_DS(MM_SUB,"=MM= FOUND NON SGLTE PLMN in SGLTE HOME ",0,0,0); 
    }    
  }
  return result;
}


boolean mm_process_if_both_stack_has_camped_non_sglte_plmn(mm_as_id_e_type in_as_id)
{
  boolean val1 =FALSE;
  boolean val2 =FALSE;
  boolean result = FALSE;
  policyman_status_t out_put1  = POLICYMAN_STATUS_SUCCESS;
  policyman_status_t out_put2 = POLICYMAN_STATUS_SUCCESS;
  emm_ctrl_data_type *emm_ctrl_data_ptr;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);

  if(!MM_SUB_IS_SGLTE)
  {
    return FALSE;
  }

  if (mm_current_ue_mode == SYS_UE_MODE_SGLTE_TEST_ONLY)
  {
    MSG_HIGH_DS(MM_SUB, "CM_UE_MODE_SGLTE_TEST_ONLY not blocking registration..",0,0,0) ;
    return FALSE;
  }
  if (MM_IS_IN_SGLTE_MODE && 
   ((((mm_serving_plmn_sim[mm_ps_stack_id].info.active_rat == SYS_RAT_TDS_RADIO_ACCESS)
   &&(mmcoord_pending_reg_message_sim[mm_ps_stack_id].cmd.mmr_reg_req.network_selection_mode 
                                                                 ==SYS_NETWORK_SELECTION_MODE_MANUAL ))
    ||(((mm_serving_plmn_sim[mm_ps_stack_id].info.active_rat == SYS_RAT_LTE_RADIO_ACCESS)) &&
    (emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode ==SYS_NETWORK_SELECTION_MODE_MANUAL)))
     &&(mmcoord_pending_reg_message_sim[mm_cs_stack_id].cmd.mmr_reg_req.network_selection_mode 
                                                                 ==SYS_NETWORK_SELECTION_MODE_MANUAL)))
  {
    if (mm_serving_plmn_sim[mm_ps_stack_id].info.active_rat == SYS_RAT_LTE_RADIO_ACCESS)
    {
      out_put1 = policyman_sglte_plmn_is_sglte(&(emm_ctrl_data_ptr->reg_req_pending_ptr->plmn),&val1);
    }
    else
    {
      out_put1 = policyman_sglte_plmn_is_sglte(&(mmcoord_pending_reg_message_sim[mm_ps_stack_id].cmd.mmr_reg_req.plmn),
                                                 &val1);
    }
    out_put2 =policyman_sglte_plmn_is_sglte(&(mmcoord_pending_reg_message_sim[mm_cs_stack_id].cmd.mmr_reg_req.plmn),
                                                 &val2);
    if ((out_put1 == POLICYMAN_STATUS_SUCCESS) && 
        (out_put2 == POLICYMAN_STATUS_SUCCESS)&&(val1== FALSE)
         &&(val2==FALSE))
    {
      result = TRUE;
      MSG_HIGH_DS(MM_SUB,"=MM= In MANUAL Selection Mode Found NON-SGLTE PLMN in SGLTE HOME ",0,0,0); 
    }
  }
  else if (mm_serving_plmn_sim[mm_ps_stack_id].info.active_rat == SYS_RAT_LTE_RADIO_ACCESS &&
     (mm_serving_plmn_sim[mm_cs_stack_id].info.active_rat==SYS_RAT_GSM_RADIO_ACCESS) )
  {
    emm_ctrl_data_type *emm_ctrl_data_ptr;
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    ASSERT(emm_ctrl_data_ptr != NULL);
    if (MM_IS_IN_SGLTE_MODE &&
       (emm_ctrl_data_ptr->plmn_service_state->service_status == SYS_SRV_STATUS_SRV)&&
        mm_serving_plmn_sim[mm_cs_stack_id].mm_service_state== SERVICE_AVAILABLE)
    {
      out_put1 = policyman_sglte_plmn_is_sglte( &(emm_ctrl_data_ptr->plmn_service_state->plmn),&val1);
      out_put2 = policyman_sglte_plmn_is_sglte( &(mm_serving_plmn_sim[mm_cs_stack_id].info.plmn),&val2);
    
      if ((out_put1 == POLICYMAN_STATUS_SUCCESS) && 
        (out_put2 == POLICYMAN_STATUS_SUCCESS)&&(val1== FALSE)
         &&(val2==FALSE))
      {      
        result = TRUE;
        MSG_HIGH_DS(MM_SUB,"=MM= In SGLTE HOME Both Stack camp on NON SGLTE PLMN ",0,0,0); 
     }
    }
  }
  else if (mm_serving_plmn_sim[mm_ps_stack_id].info.active_rat == SYS_RAT_TDS_RADIO_ACCESS &&
     (mm_serving_plmn_sim[mm_cs_stack_id].info.active_rat==SYS_RAT_GSM_RADIO_ACCESS) )
  {
    if(MM_IS_IN_SGLTE_MODE && 
      (mm_serving_plmn_sim[mm_cs_stack_id].mm_service_state== SERVICE_AVAILABLE)&&
      (mm_serving_plmn_sim[mm_ps_stack_id].mm_service_state== SERVICE_AVAILABLE))
    {
      out_put1 = policyman_sglte_plmn_is_sglte( &(mm_serving_plmn_sim[mm_cs_stack_id].info.plmn),&val2);
      out_put2 =policyman_sglte_plmn_is_sglte( &(mm_serving_plmn_sim[mm_ps_stack_id].info.plmn),&val1);
    
      if ((out_put1 == POLICYMAN_STATUS_SUCCESS) && 
        (out_put2 == POLICYMAN_STATUS_SUCCESS)&&(val1== FALSE)
         &&(val2==FALSE))
      {
        result = TRUE;
        MSG_HIGH_DS(MM_SUB,"=MM= In SGLTE HOME both stack camp on NON SGLTE PLMN  ",0,0,0); 
      }
    }
  }
  
  return result;
}

boolean mm_process_sglte_plmn_state4_gsm_oos(sys_plmn_id_s_type  pPlmnId)
{
  boolean val=FALSE;
  if(!MM_SUB_IS_SGLTE)
  {
    return FALSE;
  }
  ASSERT(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL);

  if (MM_IS_IN_SGLTE_STATE_4 && 
      ((mmcoord_pending_reg_message.cmd.hdr.message_id == (int)MMR_REG_REQ) &&
        (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode
                 != SYS_NETWORK_SELECTION_MODE_LIMITED)) &&
      ((mm_serving_plmn_sim[mm_ps_stack_id].info.active_rat== SYS_RAT_LTE_RADIO_ACCESS )||
        (mm_serving_plmn_sim[mm_ps_stack_id].info.active_rat== SYS_RAT_TDS_RADIO_ACCESS )))
   {
     mmsend_mmr_stack_act_ind();
     mm_copy_cs_database(MM_SGLTE_STACK_2, mm_sub_id);
     mmcoord_process_spliting();
     emm_ctrl_data_ptr->reg_req_pending_ptr->req_service_domain = SYS_SRV_DOMAIN_PS_ONLY;
     /* making no servcie on the stack2 , so that CM can act accordingly*/
     mm_serving_plmn_sim[mm_cs_stack_id].info.service_status = SYS_SRV_STATUS_NO_SRV;
	 mm_serving_plmn_sim[mm_cs_stack_id].info.plmn.identity[0] = 0xFF;
     mm_serving_plmn_sim[mm_cs_stack_id].info.plmn.identity[1] = 0xFF;
     mm_serving_plmn_sim[mm_cs_stack_id].info.plmn.identity[2] = 0xFF;
     mm_serving_plmn_sim[mm_cs_stack_id].lac.lac[0]            = 0xFF;
     mm_serving_plmn_sim[mm_cs_stack_id].lac.lac[1]            = 0xFF;
     mm_serving_plmn_sim[mm_cs_stack_id].gmm_service_state     = SERVICE_NOT_AVAILABLE;
     mm_serving_plmn_sim[mm_cs_stack_id].mm_service_state     = SERVICE_NOT_AVAILABLE;
     mm_serving_plmn_sim[mm_cs_stack_id].info.plmn_service_capability = SYS_SRV_CAPABILITY_NOT_AVAILABLE;
     mm_serving_plmn_sim[mm_cs_stack_id].network_op_mode  = PS_DOMAIN_SYS_INFO_UNAVAILABLE;
     mm_serving_plmn_sim[mm_cs_stack_id].rac = 0xFF ;
     mmsend_mmr_service_stat_ind(mm_cs_stack_id);
     val = TRUE;
   }
  return val;
}
boolean mm_sglte_state4_trigger_split(void)
{
  boolean val = FALSE;

  if(!MM_SUB_IS_SGLTE)
  {
    return FALSE;
  }

  if (MM_IS_IN_SGLTE_STATE_4 && (mm_serving_plmn_sim[mm_ps_stack_id].info.active_rat== 
                                                                 SYS_RAT_GSM_RADIO_ACCESS )
        &&(mm_is_split_needed()==TRUE ))
   {
     mm_copy_cs_database(MM_SGLTE_STACK_2, mm_sub_id);
     /* changing stop mode reason to RR_SGLTE_USER_RAT_CHANGE so that RR 
        stop any ongoing interrat activity similar to user rat change*/
     mm_send_rr_stop_mode_req(RR_SGLTE_USER_RAT_CHANGE);
     mmsend_mmr_stack_act_ind();
	 mm_service_loss_on_rat_sim[mm_ps_stack_id] = SYS_RAT_GSM_RADIO_ACCESS;
     mm_serving_plmn_sim[mm_ps_stack_id].info.ps_data_suspend = TRUE;
     mm_sglte_current_stack_status = SGLTE_MM_UPPER_LAYER_SPLITING;
     val = TRUE;
   }
  return val;
}
boolean mm_is_split_needed(void)
{
  boolean val = FALSE;

  if(!MM_SUB_IS_SGLTE)
  {
    return FALSE;
  }

  /* keep adding condition for the validity for split in coming future */
  if ((mm_serving_plmn_sim[mm_cs_stack_id].info.plmn_service_capability == 
                                                      SYS_SRV_CAPABILITY_CS_SERVICE_ONLY )
      ||(gmm_check_gprs_forbidden_rai(mm_serving_plmn_sim[mm_cs_stack_id].info.plmn,
           mm_serving_plmn_sim[mm_cs_stack_id].lac,mm_serving_plmn_sim[mm_cs_stack_id].rac)==TRUE)
      )
  {
    val = TRUE;
  }
  return val;
}

void mm_if_needed_abort_plmn_search_and_loop_back_to_reg( mmr_reg_req_e_type req_type)
{
  if ( MM_IS_IN_SGLTE_STATE_4 && (mm_plmn_search_in_progress == TRUE) && 
      ( req_type == MMR_REG_REQ_SGLTE_PS_ONLY_ACQUISITION) &&
      ((mmcoord_pending_reg_list_req_message.cmd.hdr.message_set == MS_MM_REG) &&
      ( mmcoord_pending_reg_list_req_message.cmd.hdr.message_id  == (int) MMR_PLMN_SEARCH_REQ))
     )
  {
    mmr_plmn_search_cnf_s_type  *mmr_plmn_search_cnf;
    mmr_service_ind_s_type    *mmr_service_ind;

#ifndef FEATURE_MODEM_HEAP
    mmr_plmn_search_cnf = (mmr_plmn_search_cnf_s_type*)gs_alloc(sizeof(mmr_plmn_search_cnf_s_type));
    mmr_service_ind = (mmr_service_ind_s_type*) gs_alloc(sizeof(mmr_service_ind_s_type));
#else
    mmr_plmn_search_cnf = (mmr_plmn_search_cnf_s_type*)modem_mem_calloc(1, sizeof(mmr_plmn_search_cnf_s_type), MODEM_MEM_CLIENT_NAS);
    mmr_service_ind = (mmr_service_ind_s_type*) modem_mem_calloc(1, sizeof(mmr_service_ind_s_type), MODEM_MEM_CLIENT_NAS);
#endif
    mm_check_for_null_ptr((void*)mmr_plmn_search_cnf);
    mm_check_for_null_ptr((void*)mmr_service_ind);

    MSG_HIGH_DS(MM_SUB,"=MM= Aborting PLMN search in state4, as Service req for split in process ",0,0,0); 
     /* Set mm_plmn_search_in_progress as FALSE */
    mm_plmn_search_in_progress = FALSE;
    
    mmr_plmn_search_cnf->network_selection_mode = mm_waiting_for_list_cnf_type;
    mm_waiting_for_list_cnf_type = SYS_NETWORK_SELECTION_MODE_NONE;
    
    mmr_plmn_search_cnf->plmn_search_result = REG_MM_PLMN_SEARCH_ABORTED;
    
	memset(&(mmr_plmn_search_cnf->found_plmn_list),0,sizeof(sys_found_plmn_list_u_type));
    
    mmr_plmn_search_cnf->transaction_id = 
                       mmcoord_pending_reg_list_req_message.cmd.mmr_plmn_search_req.transaction_id;
    mmr_plmn_search_cnf->service_search = 
                       mmcoord_pending_reg_list_req_message.cmd.mmr_plmn_search_req.service_search;;
    
    mmcoord_pending_reg_list_req_message.cmd.hdr.message_set = 0;
    mmcoord_pending_reg_list_req_message.cmd.hdr.message_id = 0;
    mm_send_mmr_plmn_search_cnf( mmr_plmn_search_cnf );
    
 /*Sending service indication as in legacy also send service indication   */
    mmr_service_ind->cause = MMR_CAUSE_NOT_SPECIFIED;
    mmr_service_ind->service_state = mm_serving_plmn.info;
    mm_send_mmr_service_ind( mmr_service_ind);
    
#ifndef FEATURE_MODEM_HEAP
    gs_free(mmr_plmn_search_cnf);
    gs_free(mmr_service_ind);
#else
    modem_mem_free(mmr_plmn_search_cnf, MODEM_MEM_CLIENT_NAS);
    modem_mem_free(mmr_service_ind, MODEM_MEM_CLIENT_NAS);
#endif
  }
 
}

#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
void mm_abort_service_request_if_needed(void)
{
  emm_failure_type     emm_failure_cause;
  emm_ctrl_data_type   *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));

  emm_failure_cause.cause_type = LTE_NAS_IRAT_CONN_EST_FAILURE;
  emm_failure_cause.cause.est_status = LTE_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE;
  
  if(emm_ctrl_data_ptr->esr_type_in_progress != NAS_EMM_CSFB_NONE )
  {
    MSG_HIGH_DS_0(MM_SUB,"Sending MO call reject as MT call already in progress on other sub");
    if (EMM_GET_CONNECTION_STATE() == EMM_WAITING_FOR_RRC_CONFIRMATION_STATE ||
        EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE)
    {
      emm_abort_service_request(emm_ctrl_data_ptr, emm_ctrl_data_ptr->esr_est_conn, TRUE,
                                LTE_NAS_LRRC_CONN_EST_FAILURE_RF_UNAVAILABLE,
                                emm_failure_cause);
      emm_update_lte_trm_priorty(emm_ctrl_data_ptr);
    }
    else
    {
       mm_csfb_rej_pending_call();
       emm_ctrl_data_ptr->rr_blind_redir_pending_call_abort = TRUE;
    }
  }
  else if (emm_ctrl_data_ptr->emm_sms_active &&
           (EMM_GET_CONNECTION_STATE() == EMM_WAITING_FOR_RRC_CONFIRMATION_STATE ||
            EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE))
  {
    emm_abort_service_request(emm_ctrl_data_ptr, FALSE, TRUE,
                              LTE_NAS_LRRC_CONN_EST_FAILURE_RF_UNAVAILABLE,
                              emm_failure_cause);
    emm_update_lte_trm_priorty(emm_ctrl_data_ptr);
  }
}
#endif

void mm_clear_serving_lai_on_moving_to_lte(void)
{
  mm_serving_lai[0] = 0xFF;
  mm_serving_lai[1] = 0xFF;
  mm_serving_lai[2] = 0xFF;
  mm_serving_lai[3] = 0xFF;
  mm_serving_lai[4] = 0xFE;
}

void mm_local_cs_detach()
{
  mm_state=MM_IDLE;
  mm_substate_control( MM_STD_LIMITED_SERVICE );
  mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                 SYS_SRV_DOMAIN_NO_SRV,
                                 FALSE,
                                 SYS_SRV_STATUS_LIMITED,
                                 TRUE );
}

void emm_set_mm_msg_info_for_debug_buffer(byte msg_info)
{
  mm_msg_info = msg_info;
}


#ifdef FEATURE_TDSCDMA
/*===========================================================================
FUNCTION  mm_csfb_regional_handling(boolean state)

DESCRIPTION
 This function performs special handling of CFSB case depending on the region
 where the UE is currently camped.

DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void mm_csfb_regional_handling(boolean state)
{
    boolean region = FALSE;
    sys_plmn_id_s_type rplmn = {0xFF, 0xFF, 0xFF};  
    boolean plmn_id_is_undefined;
    boolean mnc_includes_pcs_digit;
    uint32  rplmn_mcc;
    uint32  rplmn_mnc;
    
    /* Enable region handling only for CMCC cards*/
    if (reg_mode_is_usim_compliant_for_cm((sys_modem_as_id_e_type)mm_as_id))
    {
        MSG_HIGH_DS_0(MM_SUB,"CSFB determine the region");      
        if((mm_serving_plmn.info.active_rat == SYS_RAT_LTE_RADIO_ACCESS) && (state))
        {       
            rplmn = reg_sim_read_rplmn(SYS_SRV_DOMAIN_CS_PS);   
            sys_plmn_get_mcc_mnc
            (
              rplmn,
              &plmn_id_is_undefined,
              &mnc_includes_pcs_digit,
              &rplmn_mcc,
              &rplmn_mnc
           );
          /* Set the flag for CMCC or HK*/
           if ((rplmn_mcc == 460) || (rplmn_mcc == 454))
          {
            region = TRUE;
          }
       }   
      /* Notify the SYS database about the camped region */
        sys_eplmn_per_nas_region_update(region);
        MSG_HIGH_DS_1(MM_SUB,"CSFB Region set to %d", region);
   }
}
#endif

#ifdef FEATURE_DUAL_DATA
/*************************************************************
FUNCTION emm_send_rr_ps_resume_ind ()

DESCRIPTION
  This function checks if UE should resume ps on RR 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 *************************************************************/
void emm_send_rr_ps_resume_ind(emm_ctrl_data_type  *emm_ctrl_data_ptr)
{
  mm_as_id_e_type      mm_other_as_id, mm_tmp_as_id;
  mm_tmp_as_id = mm_as_id;
  ASSERT(emm_ctrl_data_ptr != NULL);

  if (MMCNM_IS_MODE_DUAL_ACTIVE && (mm_mn_sglte_svlte_mode == FALSE))
  {
    return;
  }
  for (mm_other_as_id = MM_AS_ID_1; mm_other_as_id < (mm_as_id_e_type)MAX_NAS_STACKS ; mm_other_as_id++ )
  {
    if (mm_other_as_id != mm_tmp_as_id )
    {
      mm_as_id = mm_other_as_id;
      mm_sub_id = mm_sub_id_stack[mm_as_id];
      if((mm_conn_mode_manual_search_enabled == TRUE) &&
          (mm_serving_plmn_sim[mm_tmp_as_id].info.active_rat == SYS_RAT_LTE_RADIO_ACCESS)&&
          (emm_ctrl_data_ptr->plmn_search_in_progress == TRUE) &&
         ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) ||
          (mm_serving_plmn.ms_op_mode ==  SYS_SRV_DOMAIN_PS_ONLY) ||
          ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY) && 
            (mm_serv_change_pending == TRUE) && 
            (gmm_gprs_detach_pending == TRUE))))
      {
        rr_ps_resume_ind_T     rr_ps_resume_ind;
        rr_ps_resume_ind.message_header.message_set = MS_MM_RR;
        rr_ps_resume_ind.message_header.message_id  = (int) RR_PS_RESUME_IND;
        PUT_IMH_LEN( sizeof( rr_ps_resume_ind_T ) - sizeof(IMH_T),
                     &rr_ps_resume_ind.message_header);
        MSG_HIGH_DS_0( MM_SUB, "=MM= MM sent RR_PS_RESUME_IND");
        mm_send_message( &rr_ps_resume_ind.message_header, GS_QUEUE_RR );
      }
    }
  }
  mm_as_id = mm_tmp_as_id;
  mm_sub_id = mm_sub_id_stack[mm_as_id];
}

/*************************************************************
FUNCTION emm_send_data_priority_on_other_sub ()

DESCRIPTION
  This function sends data prioirty indication to GRR after LTE SUB moves to BG traffic (TRM Prioirty LOW).
  NAS will bump up SUB2 GSM BG traffic TRM priority (Medium) when LTE is in connected mode with TRM state as BG traffic 
  in VOLTE + PS mode. Hence, G’s BG_TRAFFIC will now have higher priority (95) to ensure
  L’s BG_TRAFFIC or ACQ doesn’t pre-empt G’s BG_TRAFFIC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 *************************************************************/

void emm_send_data_priority_on_other_sub(sys_data_priority_e_type priority)
{
  mm_as_id_e_type      mm_other_as_id, mm_tmp_as_id;
  mm_tmp_as_id = mm_as_id;

  if (mmode_nv_setting_for_device_mode != SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE)
  {
    for( mm_other_as_id = MM_AS_ID_1; mm_other_as_id < (mm_as_id_e_type)MAX_NAS_STACKS ; mm_other_as_id++ )
    {
      if( mm_other_as_id != mm_tmp_as_id )
      {
        mm_as_id = mm_other_as_id;
        mm_sub_id = mm_sub_id_stack[mm_as_id];
        if (is_gsm_mode() == TRUE)
        { 
          mm_update_data_priority(MM_DATA_PRI_CLIENT_EMM, priority);
        }
      }
    }
    mm_as_id = mm_tmp_as_id;
    mm_sub_id = mm_sub_id_stack[mm_as_id];
  }
}


#endif
#endif

