/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC IDLE MODE MODULE
***
***
*** DESCRIPTION
***
***  This module administrates the signal reception and the associated actions
***  for idle mode states.
***
*** EXTERNALIZED FUNCTIONS
***
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmacidle.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 09/11/10   sjw     Updated for Dual SIM support
*** 25/10/04   zyu     Added EDGE logging in MAC layer
*** 07/04/01   tlx     Created
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "geran_msgs.h"
#include "string.h"

#include "gmacdef.h"
#include "gmacrb.h"
#include "gmacutil.h"
#include "gmacidle.h"
#include "gmacnull.h"
#include "gmacproc.h"
#include "gmaclog.h"
#include "gmacidle_if.h"
#include "gmacds.h"
#include "stringl/stringl.h"

#if defined(TEST_FRAMEWORK)
#error code not present
#endif // TEST_FRAMEWORK

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/
#define MAX_SIZE_DL_PDA_MSG 40

/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/

/*****************************************************************************
***
***     Private Idle/Idleutil Data Declarations
***
*****************************************************************************/
gmac_idle_data_t gmac_idle_data[NUM_GERAN_DATA_SPACES];

/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/

   static void display_dl_msg_rcvd
   (
     uint8 dl_chan_msg,uint32 line_no,
     gprs_mac_idle_substates_t mac_state,
     gprs_mac_action_t current_action,
     gas_id_t gas_id
   );

   static void display_signal_rcvd
   (
     uint8 sig_id,uint8 task_id,uint32 line_no,
     gprs_mac_idle_substates_t mac_state,
     gprs_mac_action_t current_action,
     gas_id_t gas_id
   );

   static void display_mac_action(gprs_mac_action_t mac_act, gas_id_t gas_id);

   static void gmac_idle_start_no_ps_access_process
   (
     gprs_mac_idle_substates_t   current_mac_state,
     mac_l1_sig_t                *mac_l1_sig_ptr,
     delete_pdu_cause_t          delete_pdu_flag,
     gas_id_t                    gas_id
   );

   /* added to facilitate the new MAC/L1 release procedure */
   /* when a PDA interrupts the ul tbf est                 */
   static boolean gmac_idle_ignore_pda(gprs_mac_action_t mac_act,mac_l1_sig_t *mac_l1_sig_ptr, gas_id_t gas_id);

   static void gmac_idle_csn1_decode_stored_pda_data(uint8 *csn1_encoded_pda_msg_ptr, gas_id_t gas_id);

   static void gmac_idle_ul_release_confirm_handler(gas_id_t gas_id);

   static void gmac_idle_handle_pua_error_result( gas_id_t gas_id );

   static void gmac_idle_simulate_freq_cnf_for_ul_states( gas_id_t gas_id );

   static void gmac_idle_handle_no_freq_condition_in_pua( gas_id_t gas_id );

   static void gmac_idle_dl_release_confirm_handler(gas_id_t gas_id);

/*****************************************************************************
***
***     Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION    gmac_get_idle_data_ptr()
===
===  DESCRIPTION
===  Returns a pointer to idle data space using the current gas_id
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    gmac_idle_data_t*
===
===  SIDE EFFECTS
===
===
===========================================================================*/
gmac_idle_data_t* gmac_get_idle_data_ptr(gas_id_t gas_id)
{
  gmac_idle_data_t *idle_data_ptr = &gmac_idle_data[0];

   /* validate the GAS ID */
  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    idle_data_ptr = &gmac_idle_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)];
  }
  else
  {
    MSG_GERAN_ERROR_1_G("Idle bad gas_id:%d",gas_id);
  }
  return idle_data_ptr;
}

void idle_mode_handler(uint8 task_id, uint8 signal_id, gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  /*****************************************************************
  * pmo shall be forwared to GRR irrspective of the idle sub state *
  ******************************************************************/
/*CR67833*/
  if (
       (task_id == MS_MAC_L1) &&
       (signal_id == (uint8)MAC_PH_DATA_IND ) &&
       (idle_data_ptr->release_cause == GRR_INITIATED_NO_ACCESS )
     )
  {
    /*During GRR_INITIATED_NO_ACCESS release procedures block
      all PACCH control messages until MAC receives the rel confirm
      from Layer1, at which point MAC will transition to a NULL INACTIVE state. */
    MSG_GERAN_MED_0_G( "UL msgs are ignored while performing GRR_INITIATE_NO_ACCESS release procedure");
    return;
  }

  switch( idle_data_ptr->mac_idle_substate)
  {

  /*****************************************************************
  *
  *   PASSIVE, THE INITIAL STATE OF THE IDLE MODE MODULE.
  *
  ******************************************************************/
      case PASSIVE:

         /**********************************************************
         *             signals received from L1                    *
         **********************************************************/
         if(task_id == MS_MAC_L1)
         {

           if(signal_id == (uint8)MAC_PH_DATA_IND)
           {
             display_dl_msg_rcvd(global_data_ptr->channel_msg,__LINE__,idle_data_ptr->mac_idle_substate, global_data_ptr->mac_action,gas_id);
           }
           else
           {
             display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,gas_id);
           }


           switch (signal_id)
           {

             /* reception of MAC_PH_DATA_IND from L1 */
             case MAC_PH_DATA_IND :
                switch (global_data_ptr->channel_msg)
                {
                    /* PSI handling perfromed in gmactrask.c*/

                    case PACKET_DOWNLINK_ASSIGNMENT:

                         global_data_ptr->c_flags.waiting_for_dl_sb_control_msg = FALSE;

                         /*If CCCH cell, then in idle mode because MS rcvd a DL_SB from the N/W   */
                         /*for the purpose of receiving a DL control msg of the type PDA,PMO,PCCO */
                         /*under these conditions L1 is in DL SB transfer state and needs to be   */
                         /*released before returning to NULL mode                                 */

                         /*   in passive pda will be addressed via its tlli   */
                         idle_data_ptr->pda_data_result = gmac_idle_pda_handler(&idle_data_ptr->dl_ass_ind_sig,FALSE,PDA_CHECK_TLLI_ONLY,gas_id);

                         if( idle_data_ptr->pda_data_result == NO_ERRORS)
                         {

                           MSG_GERAN_MED_2_G( "MAC Rcvd PDA in DL_SB_EST,m_s %d,m_a %d",idle_data_ptr->mac_idle_substate,
                                                                                        global_data_ptr->mac_action);
                           rrbp_control_block_handler_2(PCA_DL_MSG,gas_id);

                           /*store PDA in csn1 format*/
                           /*dl_msg is the glaobal msg buffer loaded by rrbp handler when retriving dl msg*/


                           memset(idle_data_ptr->csn1_encoded_pda_msg,0,SIZE_OF_DL_MSG);
                           memscpy(idle_data_ptr->csn1_encoded_pda_msg,sizeof(idle_data_ptr->csn1_encoded_pda_msg),
                                   global_data_ptr->dl_msg, SIZE_OF_DL_MSG);

                           /*needed for releative starting time cal's*/
                           idle_data_ptr->gmac_store_rcvd_fn_of_pda = global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no;
                           idle_data_ptr->release_cause = PDA_RCVD_IN_DL_SB_TBF;
                           gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, DL_TBF_ABNORMAL,MAC_LOG_UL_UNKNOWN_ERROR,MAC_LOG_DL_NORMAL_RELEASE,gas_id );

                           /*When MAC is waiting to receive a DL control msg via a DL_SB_EST                */
                           /*RLC will be blocked, and RR will have rcvd idle block in mac status            */
                           /*done for continuity as the rule is that when establishing a DL RLC is blocked  */
                           /*until the DL has been connected                                                */

                           gmac_util_block_rlc(NO_DEL,GMAC_IDLE,__LINE__,gas_id);
                           (void)mac_log_state(gas_id);

                           MSG_GERAN_MED_0_G( "MAC sent MAC_GRR_MAC_STATUS");
                           gmac_util_send_mac_grr_mac_status(&idle_data_ptr->mac_grr_sig,MAC_IDLE_BLOCKED,gas_id);
                         }
                         else
                         {
                           reset_tbf_parameters(gas_id);

                           /*RLC should have been blocked at the point IA for DL SB TBF was rcvd*/
                           if(gmac_util_is_rlc_blocked(GMAC_IDLE,__LINE__,gas_id)== UNBLOCKED)
                           {
                           gmac_util_block_rlc(NO_DEL,GMAC_IDLE,__LINE__,gas_id);
                           MSG_GERAN_ERROR_1_G( "GMAC should have already blocked rlc at this point mac action is %d",
                                                global_data_ptr->mac_action);
                           }

                           idle_data_ptr->release_cause = REL_DL_SB_TBF;
                           gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, DL_TBF_ABNORMAL,MAC_LOG_UL_UNKNOWN_ERROR,MAC_LOG_DL_NORMAL_RELEASE,
                                                         gas_id );
                         }
                    break;

                    case PACKET_CELL_CHANGE_ORDER :

                         global_data_ptr->c_flags.waiting_for_dl_sb_control_msg = FALSE;

                         if(gmac_idle_check_pcco_addressing(gas_id))
                         {
                           rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);

                           MSG_GERAN_MED_0_G("MAC forwards PCCO to GRR");
                           gmac_send_data_ind_to_grr(gas_id);

                         }
                         else
                         {
                           MSG_GERAN_MED_0_G("MAC Info PCCO not for MS");
                         }

                         /*If CCCH cell, then in idle mode because MS rcvd a DL_SB from the N/W   */
                         /*for the purpose of receiving a DL control msg of the type PDA,PMO,PCCO */
                         /*under these conditions L1 is in DL SB transfer state and needs to be   */
                         /*released before returning to NULL mode                                 */
                         /*RLC should have been blocked at the point IA for DL SB TBF was rcvd*/
                         if(gmac_util_is_rlc_blocked(GMAC_IDLE,__LINE__,gas_id)== UNBLOCKED)
                         {
                           gmac_util_block_rlc(NO_DEL,GMAC_IDLE,__LINE__,gas_id);
                           MSG_GERAN_ERROR_1_G( "GMAC should have already blocked rlc at this point mac action is %d",
                                                global_data_ptr->mac_action);
                         }

                         idle_data_ptr->release_cause = REL_DL_SB_TBF;
                         gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, DL_TBF_ABNORMAL,MAC_LOG_UL_UNKNOWN_ERROR,MAC_LOG_DL_NORMAL_RELEASE,gas_id );

                    break;

                    case PACKET_MEASUREMENT_ORDER:

                         global_data_ptr->c_flags.waiting_for_dl_sb_control_msg = FALSE;

                         /* This functionality has been moved from NULL*/
                         if ( gmac_util_check_pcco_pmo_addressing(gas_id) )
                         {
                           rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);

                           MSG_GERAN_MED_0_G("MAC forwards PMO to GRR");
                           gmac_send_data_ind_to_grr(gas_id);

                         }
                         else
                         {
                           MSG_GERAN_MED_0_G("MAC Info PMO not for MS");
                         }

                         /*If CCCH cell, then in idle mode because MS rcvd a DL_SB from the N/W   */
                         /*for the purpose of receiving a DL control msg of the type PDA,PMO,PCCO */
                         /*under these conditions L1 is in DL SB transfer state and needs to be   */
                         /*released before returning to NULL mode                                 */
                         /*RLC should have been blocked at the point IA for DL SB TBF was rcvd*/
                         if(gmac_util_is_rlc_blocked(GMAC_IDLE,__LINE__,gas_id)== UNBLOCKED)
                         {
                           gmac_util_block_rlc(NO_DEL,GMAC_IDLE,__LINE__,gas_id);
                           MSG_GERAN_ERROR_1_G( "GMAC should have already blocked rlc at this point mac action is %d",
                                                global_data_ptr->mac_action);
                         }

                         idle_data_ptr->release_cause = REL_DL_SB_TBF;
                         gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, DL_TBF_ABNORMAL,MAC_LOG_UL_UNKNOWN_ERROR,MAC_LOG_DL_NORMAL_RELEASE,gas_id );

                    break;


                    default :
                        MSG_GERAN_MED_3_G( "Unexpected DL msg rx. idle state %d task id %d msg id %d",idle_data_ptr->mac_idle_substate,
                                                                                                      task_id,
                                                                                                      global_data_ptr->channel_msg);
                    break;

                }/*end of message switch*/
             break;


             case L1_MAC_51_SB_FAILURE:

                    global_data_ptr->mac_mode = NULL_MODE;
                    MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_51_SB_FAILURE");

                    gmac_util_send_grr_mac_status(MAC_IDLE,gas_id);

                    gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,MAC_INITIATED,GMAC_IDLE,__LINE__,gas_id);

                    /*In donwlink single block case, GMAC already blocked RLC so unblocking*/
                    gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_IDLE,__LINE__,gas_id);

                    global_data_ptr->c_flags.waiting_for_dl_sb_control_msg = FALSE;

                    (void)mac_log_state(gas_id);
             break;

             case L1_MAC_TBF_REL_CONFIRM :

                  global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;

                  global_data_ptr->c_flags.waiting_for_dl_sb_control_msg = FALSE;

                  gmac_idle_ul_release_confirm_handler(gas_id);

                  idle_data_ptr->release_cause = NO_RELEASE_CAUSE;
                  (void)mac_log_state(gas_id);
             break;


             default:

                 /* no l1 to mac signals apart from ph_data_ind    */
                 /* should be received in idle mode passive state  */
                 display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,gas_id);

                 MSG_GERAN_ERROR_3_G( "Unexpected L1 sig rx. idle state %d task id %d sig id 0x%x",
                                      idle_data_ptr->mac_idle_substate,task_id, signal_id);
             break;
           }
         }
         /**********************************************************
         *             signals received from GRR                   *
         **********************************************************/
         else if(task_id == MS_MAC_RR)
         {

           display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,gas_id);

           switch (signal_id)
           {
               case GRR_MAC_NO_PS_ACCESS_IND:

                   MSG_GERAN_MED_3_G("GMAC tbf_release_pending_flg %d,release_cause %d, waiting_for_dl_sb_control_msg %d",
                                      global_data_ptr->c_flags.tbf_release_pending_flg,
                                      idle_data_ptr->release_cause,
                                      global_data_ptr->c_flags.waiting_for_dl_sb_control_msg);

                   if( ((global_data_ptr->c_flags.tbf_release_pending_flg == TRUE)  && (idle_data_ptr->release_cause == REL_DL_SB_TBF) )
                       || (global_data_ptr->c_flags.waiting_for_dl_sb_control_msg == TRUE) )
                   {

                     /* DL SB tbf est  MAC now has to release L1, if PS NO access is rcvd before L  1*/
                     /* can confirm the release ie waiting for any PCA rcvd in the DL control msg to */
                     /* be Tx-ed perform NO ACCESS procedure: abnormal release L1,inform RLC stop    */
                     /* any timers. Note in  passive there are NO system times running that need to  */
                     /* be stopped, Wait for rel confirm before returning back to NULL               */

                     MSG_GERAN_MED_1_G("GMAC Rcvd GRR_MAC_NO_PS_ACCESS_IND pdu deletion type %d",
                                       global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);

                     gmac_util_block_rlc(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,GMAC_IDLE,__LINE__,gas_id);
                     gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, DL_TBF_ABNORMAL,MAC_LOG_UL_UNKNOWN_ERROR,MAC_LOG_DL_NORMAL_RELEASE,
                                                   gas_id );
                     global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
                     idle_data_ptr->release_cause = GRR_INITIATED_NO_ACCESS;

                     global_data_ptr->c_flags.waiting_for_dl_sb_control_msg = FALSE;
                   }
                   else
                   {
                     /**********************************************************************
                     * NO PS ACCESS IND received from GRR. GMAC will block RLC and forward *
                     * delete pdu case to RLC in MR_NO_PS_ACCESS_IND and gmac goes to      *
                     * NULL_INACTIVE state                                                 *
                     ***********************************************************************/

                     /* Complete PCCO/PCCF
                        Case where MAC is retrying access for PCCO and T3176 expires before RLC can
                        send RM_UL_EST_TBF_REQ */
                     gmac_util_complete_pcco_process(PCCO_FAILURE_NO_RESPONSE,GRR_INITIATED,GMAC_IDLE,__LINE__,gas_id);
                     gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,GRR_INITIATED,GMAC_IDLE,__LINE__,gas_id);

                     gmac_util_send_grr_mac_status(MAC_IDLE,gas_id);

                     gmac_util_block_rlc(global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,GMAC_IDLE,__LINE__,gas_id);
                     global_data_ptr->mac_mode = NULL_MODE;
                     global_data_ptr->mac_action = NONE;
                     gmac_idle_send_mac_internal_sig(&idle_data_ptr->mac_int_sig,MAC_IN_NO_SERVICE,gas_id);

                     display_mac_action(global_data_ptr->mac_action,gas_id);

                   }

               break;

               case GRR_MAC_SEND_PCCF:
                    MSG_GERAN_HIGH_0_G("GMAC Rxd Send PCCF when waiting for DL SB (51MF)");
                    gmac_util_start_pccf_process(SENDING_VIA_SB,GMAC_IDLE,__LINE__,(uint8)idle_data_ptr->mac_idle_substate,gas_id);
               break;

               case GRR_MAC_PS_ACCESS_REQ:
                    MSG_GERAN_ERROR_0_G("GRR_MAC_PS_ACCESS_REQ is received in PASSIVE state");
                    MSG_GERAN_ERROR_0_G("GRR_MAC_PS_ACCESS_REQ msg is an unexp msg in PASSIVE state");

               break;

#ifdef FEATURE_GSM_EDTM
               case GRR_MAC_EDTM_PKT_CS_EST_REQ:
                 MSG_GERAN_MED_0_G("GMAC Rejected GRR_MAC_EDTM_PKT_CS_EST_REQ, Not in Contention Resolution");
                 gmac_send_mac_grr_edtm_pkt_cs_est_rej(EDTM_CS_REJ_CANNOT_SERVICE_REQUEST,gas_id);
               break;
#endif /*FEATURE_GSM_EDTM*/

#ifdef FEATURE_GPRS_PS_HANDOVER
               case GRR_MAC_PSHO_REQ:
                 MSG_GERAN_MED_0_G("GMAC Rejected GRR_MAC_PSHO_REQ in idle mode");
                 gmac_util_send_grr_psho_cnf(PSHO_FAILURE,gas_id);
               break;
#endif /*FEATURE_GPRS_PS_HANDOVER*/

               default:
                    MSG_GERAN_ERROR_3_G( "Unexpected GRR sig rx. idle state %d task id %d sig id 0x%x",
                                         idle_data_ptr->mac_idle_substate,task_id, signal_id);
               break;

           } /*end of signal switch*/
         }
         /**********************************************************
         *             signals received from RLC                   *
         **********************************************************/
         else if(task_id == MS_MAC_RLC)
         {
           ;
         }
         /**********************************************************
         *                 MAC internal signals                    *
         **********************************************************/
         else if(task_id == MS_MAC_MAC)
         {
           display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,gas_id);

           switch (signal_id)
           {
             case MAC_NI_UL_1P_ASS:
               MSG_GERAN_LOW_0_G("MAC Rcvd MAC_NI_UL_1P_ASS");

               MSG_GERAN_MED_0_G("GMAC State Idle UL_1P_CONTENTION_RES");
               idle_data_ptr->mac_idle_substate = UL_1P_CONTENTION_RES;

               idle_data_ptr->i_flags.idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION;
               idle_data_ptr->i_flags.pending_realloc_req = FALSE;


               idle_data_ptr->i_flags.rcvd_unsolicited_pua_waiting_for_connected_ind = FALSE;

               (void)mac_log_state(gas_id);

               idle_data_ptr->access_granted = one_phase;
               global_data_ptr->mac_action = UL_DATA_EST;
               display_mac_action(global_data_ptr->mac_action,gas_id);
               break;

             case MAC_NI_UL_2P_ASS:
               MSG_GERAN_LOW_0_G("MAC Rcvd MAC_NI_UL_2P_ASS");

               idle_data_ptr->access_granted = two_phase;
               idle_data_ptr->i_flags.idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION;
               idle_data_ptr->i_flags.pending_realloc_req = FALSE;


               idle_data_ptr->i_flags.rcvd_unsolicited_pua_waiting_for_connected_ind = FALSE;

               if( (global_data_ptr->egprs_2p_sb_mb_alloc == MB_ALLOC_OF_1_BLOCK) ||
                   (global_data_ptr->egprs_2p_sb_mb_alloc == MB_ALLOC_OF_2_BLOCKS))
               {
                  global_data_ptr->tbf_type = (uint8)multi_blk_allocation;
               }
               else
               {
                  global_data_ptr->tbf_type = (uint8)single_blk_alloc;
               }
               global_data_ptr->mac_action = UL_DATA_EST;
               display_mac_action(global_data_ptr->mac_action,gas_id);
               global_data_ptr->idle_mode_ul_tbf_release_action = NO_RE_EST;

               rr_read_public_store(RRPS_CELL_OPTIONS_T3168,&idle_data_ptr->t3168_timeout,gas_id);
               gmac_start_t3168(idle_data_ptr->t3168_timeout,gas_id);

               MSG_GERAN_MED_0_G("GMAC State Idle UL_2P_ASS");
               idle_data_ptr->mac_idle_substate = UL_2P_ASS;
               (void)mac_log_state(gas_id);
             break;

             case MAC_NI_DL_ASS:
               MSG_GERAN_LOW_0_G("MAC Rcvd MAC_NI_DL_ASS");

               idle_data_ptr->i_flags.idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION;
               idle_data_ptr->i_flags.pending_realloc_req = FALSE;


               idle_data_ptr->i_flags.rcvd_unsolicited_pua_waiting_for_connected_ind = FALSE;

               /*load rlc dl config data into dl_ass_ind_sig sig store*/
               idle_data_ptr->dl_ass_ind_sig.rlc_mode = global_data_ptr->gmac_store_dl_rlc_mode;
               idle_data_ptr->dl_ass_ind_sig.tlli     = global_data_ptr->gmac_store_dl_ass_tlli;

               idle_data_ptr->dl_ass_ind_sig.tbf_mode = global_data_ptr->current_tbf_mode;
               idle_data_ptr->dl_ass_ind_sig.ws       = global_data_ptr->current_egprs_dl_ws;

               /* copy NULL stored DL TBF data to IDLE message structure */
               idle_data_ptr->mac_l1_sig.mac_l1_dl_tbf_config = global_data_ptr->gmac_null_store_l1_dl_config;


               MSG_GERAN_MED_0_G("GMAC State Idle DL_ASS");
               idle_data_ptr->mac_idle_substate = DL_ASS;
               (void)mac_log_state(gas_id);
               break;

               case MAC_TI_RE_ESTABLISH:
                    /***************************************************************
                    * Re-establishment of the ul tbf requested during the ul       *
                    * release procedure (rel 99). the re-establishment  is a       *
                    * 2 phase process                                              *
                    * NOTE For both methods of re-est MAC must start in UL_1P_ASS  *
                    * and wait for either PUA with a SB or L1_MAC_QUAL_MEASURMENTS *
                    ***************************************************************/

                    MSG_GERAN_MED_0_G("GMAC State Idle UL_1P_ASS");
                    idle_data_ptr->mac_idle_substate = UL_1P_ASS;

                    (void)mac_log_state(gas_id);
                    idle_data_ptr->access_granted = two_phase;
               break;


               default:
                    MSG_GERAN_ERROR_3_G( "Unexpected MAC sig rx. idle state %d task id %d sig id 0x%x",
                                         idle_data_ptr->mac_idle_substate,task_id, signal_id);
               break;
           }
         }
         else
         {
           /*error*/
           MSG_GERAN_LOW_1_G( "unexpected task_id %d",task_id );
         }
      break;

      /*****************************************************************
      *
      *   UPLINK 1 PHASE ASSIGNMENT  STATE
      *
      ******************************************************************/
      case UL_1P_ASS:

         /**********************************************************
         *             signals received from L1                    *
         **********************************************************/
         if(task_id == MS_MAC_L1)
         {
           display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,
                               gas_id);

           switch (signal_id)
           {
             case MAC_PH_DATA_IND :

                display_dl_msg_rcvd(global_data_ptr->channel_msg,__LINE__,idle_data_ptr->mac_idle_substate, global_data_ptr->mac_action,
                                    gas_id);

                switch (global_data_ptr->channel_msg)
                {
                    case PACKET_CELL_CHANGE_ORDER:
                         if(gmac_idle_check_pcco_addressing(gas_id))
                         {
                           /***********************************************************
                           * when PCCO addresses MS stop relevent rlc/mac timers ref  *
                           * 4.60, 7.4.1 release l1, wait for L1_MAC_TBF_REL_CONFIRM  *
                           * and start pcco cell reselection process                  *
                           ************************************************************/
                           MSG_GERAN_LOW_0_G("MAC Info forwards PCCO to GRR");
                           rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);
                           gmac_send_data_ind_to_grr(gas_id);
                         }
                         else
                         {
                           MSG_GERAN_LOW_0_G("MAC Info PCCO not for MS");
                         }
                    break;

                    case PACKET_UPLINK_ASSIGNMENT:

                        /**************************************************************
                        * check Pkt Req Ref IE if ok, inform l1 of new  pua params &  *
                        * wait for L1_MAC_FREQ_ASS_CNF.note t3186 & t3170 stopped     *
                        * after 1'st pua was received in ul_packet access state       *
                        **************************************************************/

                        if(gmac_idle_check_pua_addressing(global_data_ptr->idle_mode_ul_tbf_release_action,idle_data_ptr->mac_idle_substate,
                                                          gas_id) == TRUE)
                        {
                          /*************************************************************
                          * DL msg address MS correctly, check if pca should be sent   *
                          **************************************************************/
                          MSG_GERAN_MED_2_G( "MAC Rcvd PUA ,m_s %d,m_a %d",idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action);

                          if(global_data_ptr->c_flags.tbf_release_pending_flg == FALSE)
                          {
                            rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);

                            if (gmac_dl_msg_data_valid(gas_id) == FALSE)
                            {
                               /*store in mac idle the  tbf type , 1 phase, 2 phase, sb, short ...etc*/
                               global_data_ptr->tbf_type = (uint8)((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->allocation_flag;

                               MSG_GERAN_MED_2_G("GMAC DEBUG 1P_ASS access_requested:%d, access_granted:%d",global_data_ptr->access_requested,
                                                                                                            idle_data_ptr->access_granted);

                               /*determine the access granted*/
                               idle_data_ptr->access_granted = gmac_ul_est_access_granted( global_data_ptr->access_requested,global_data_ptr->mac_action,
                                                                                           gas_id);

                               MSG_GERAN_MED_1_G("GMAC DEBUG 1P_ASS access_granted modified to:%d",idle_data_ptr->access_granted);

                               idle_data_ptr->pua_data_result = gmac_idle_build_rlc_alloc_and_l1_config_sigs(global_data_ptr->tbf_type, global_data_ptr->mac_action, &idle_data_ptr->mr_ul_alloc_cnf_sig,
                                                                                                             gas_id);



                              /* Log UL TBF EST in pua_xx_handler() with TS Info */
                              /*
                                     mac_log_ul_tbf_establish(gmac_local_tfi_store.ul_tfi);
                              */

                               if(idle_data_ptr->pua_data_result == NO_ERRORS)
                               {
                                  /*******************************************************************
                                  *   no errors in tbf config data, validate frequency parameters    *
                                  *******************************************************************/

                                  if (((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
                                  {
                                     idle_data_ptr->pua_data_result = gmac_util_frequency_param_ie(FALSE,TRUE,gas_id);

                                     if ( idle_data_ptr->pua_data_result == NO_ERRORS )
                                     {
                                       gmac_idle_simulate_freq_cnf_for_ul_states(gas_id);
                                     }
                                  }
                                  else
                                  {
                                    gmac_idle_handle_no_freq_condition_in_pua(gas_id);
                                  }

                                  /***********************************************
                                  *  action on validity of  frequency assignment *
                                  ***********************************************/
                                  if ( idle_data_ptr->pua_data_result != NO_ERRORS )
                                  {
                                    gmac_idle_handle_pua_error_result(gas_id);
                                  }
                               }
                               else
                               {
                                  /**********************************************************************
                                  *  action on errors in tbf config data. if access retry process fails *
                                  *  release l1 and rlc/grr abort uplink est and return to idle         *
                                  **********************************************************************/

                                  /* if ul tbf fails before reaching transfer, L1 must always be */
                                  /* release irrespective if a access retry is to be performed   */
                                  gmac_idle_handle_pua_error_result(gas_id);
                               }
                            }
                            else
                            {
                              /* csn.1 decode error,ignore msg continue as if the */
                              /* 2nd pua was never received                       */
                              MSG_GERAN_ERROR_0_G("MAC CSN.1 decode error in PUA ");
                            }
                          }
                          else
                          {
                            MSG_GERAN_HIGH_0_G("GMAC Ignore PUA when releasing L1");
                          }
                        }
                        else
                        {
                            /* ignore pua msg*/
                            MSG_GERAN_LOW_0_G( "PUA rx, chan_req_ref mis-match");
                        }
                    break;

                    case PACKET_DOWNLINK_ASSIGNMENT:

                      {
                         tbf_mode_T  temp_tbf_mode = global_data_ptr->current_tbf_mode; /*store the current TBF mode as it may need to be restored if
                                                                         the PDA is ignored or has errors*/

                         /* While performing 2p ass. During the interval between receiving the PUA*/
                         /* containing the single block alloc and waiting for the stating time to */
                         /* occur MAC has to handle PDA reception (ref 4.60 7.1.3.1)              */

                         /*********************************************************************
                         * FOR any cell selection/reselection activities ignore PDA           *
                         *********************************************************************/
                         idle_data_ptr->pda_data_result = gmac_idle_pda_handler(&idle_data_ptr->dl_ass_ind_sig,FALSE,PDA_CHECK_TLLI_ONLY,gas_id);

                         if(idle_data_ptr->pda_data_result == NO_ERRORS)
                         {
                           /*************************************************************
                           * when mac is performing any NON CELL RESELECTION activities *
                           * release l1 and store PDA if applicable. wait for release   *
                           * confirm before processing PDA.                             *
                           ************************************************************/
                           if(gmac_idle_ignore_pda(global_data_ptr->mac_action,&idle_data_ptr->mac_l1_sig,gas_id) == FALSE)
                           {

                             MSG_GERAN_MED_1_G("MAC Rcvd PDA DL TFI %d",global_data_ptr->gmac_local_tfi_store.dl_tfi);
                             rrbp_control_block_handler_2(PCA_DL_MSG,gas_id);

                             /*store PDA in csn1 format*/
                             /*dl_msg is the glaobal msg buffer loaded by rrbp handler when retriving dl msg*/


                             memset(idle_data_ptr->csn1_encoded_pda_msg,0,SIZE_OF_DL_MSG);
                             memscpy(idle_data_ptr->csn1_encoded_pda_msg,sizeof(idle_data_ptr->csn1_encoded_pda_msg),
                                     global_data_ptr->dl_msg, SIZE_OF_DL_MSG);


                             /*needed for releative starting time cal's*/
                             idle_data_ptr->gmac_store_rcvd_fn_of_pda = global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no;

                             idle_data_ptr->release_cause = PDA_INTERRUPTS_UL_TBF_EST;
                             global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;

                             if(global_data_ptr->mac_action == UL_DATA_EST)
                             {
                               /*GMC block RLC and will unblock RLC once GMAC connects dl tbf*/
                               gmac_util_block_rlc(NO_DEL,GMAC_IDLE,__LINE__,gas_id);
                             }
                           }
                           else
                           {
                             /* restore the TBF mode as the PDA has been ignored*/
                             global_data_ptr->current_tbf_mode = temp_tbf_mode;

                             MSG_GERAN_HIGH_1_G("MAC info PDA ignored reselection %d",global_data_ptr->mac_action);
                           }
                         }
                         else
                         {
                            if(idle_data_ptr->pda_data_result == FREQ_ERROR_PARTIAL_SYS_ACQ)
                            {
                              /*******************************************************************
                              * note FREQ_ERROR_PARTIAL_SYS_ACQ is the only actionable error case*
                              * for any other error case the PDA is ignored(ref 12.8 and 12.8.1) *
                              * stop access timers and release interrupted ul tbf est            *
                              *******************************************************************/
                              idle_data_ptr->release_cause = PARTIAL_SYS_ACQ;
                              if(global_data_ptr->mac_action == UL_DATA_EST)
                              {
                                /*GMC block RLC and will unblock RLC once GMAC received rel cnf from L1*/
                                gmac_util_block_rlc(NO_DEL,GMAC_IDLE,__LINE__,gas_id);
                              }
                              gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,(mac_log_ul_tbf_release_t)idle_data_ptr->pda_data_result,MAC_LOG_DL_UNKNOWN_ERROR,
                                                            gas_id );
                            }
                            else
                            {
                              /* restore the TBF mode as the PDA has been ignored*/
                              global_data_ptr->current_tbf_mode = temp_tbf_mode;
                            }
                         }

                      }
                    break;

                    case PACKET_MEASUREMENT_ORDER:
                         gmac_util_pmo_handler(gas_id);
                    break;

                    default :
                        MSG_GERAN_MED_3_G( "Unexpected DL msg rx. idle state %d task id %d msg id %d",idle_data_ptr->mac_idle_substate,
                                                                                                      task_id,
                                                                                                      global_data_ptr->channel_msg);
                    break;

                }/*end of message switch*/

             break;

             case L1_MAC_FREQ_ASS_CNF:
                  MSG_GERAN_HIGH_0_G("GMAC Info L1_MAC_FREQ_ASS_CNF rxd, Ignored for freq cnf tidy up");

                  if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_freq_ass_cnf.band_supported == FALSE)
                  {
                    MSG_GERAN_ERROR_0_G("GMAC Freq Cnf Cleanup, L1 indicating band NOT supported");
                  }
             break;

             case L1_MAC_QUAL_MEASURMENTS:
                  /*if there is tbf release already pending ignore this signal*/
                  if(global_data_ptr->c_flags.tbf_release_pending_flg == FALSE)
                  {
                    /*relevant for 2 phase access, measurements needed to be included in prr*/
                    if(global_data_ptr->idle_mode_ul_tbf_release_action == RE_EST_SEND_PRR)
                    {
                      MSG_GERAN_MED_0_G("GMAC State Idle UL_2P_ASS");
                      idle_data_ptr->mac_idle_substate = UL_2P_ASS;
                      (void)mac_log_state(gas_id);
                    }

                    /*still performing 1st half of 2 phase process at this stage*/
                    if(global_data_ptr->egprs_2p_sb_mb_alloc == GPRS_SB_ALLOC)
                    {
                      gmac_send_prr(global_data_ptr->idle_mode_ul_tbf_release_action,FALSE,gas_id);
                    }
                    else
                    {
                      gmac_idle_egprs_rac_procedure_2(TRUE,global_data_ptr->c_flags.egprs_rac_retran_req,FALSE,FALSE,gas_id);
                    }
                  }
                  else
                  {
                    MSG_GERAN_MED_0_G("GMAC releasing L1, so ignoring L1_MAC_QUAL_MEASURMENTS signal");
                  }
             break;

             case MAC_PH_CONNECT_IND:
                  /*if there is tbf release already pending ignore this signal*/
                  if(global_data_ptr->c_flags.tbf_release_pending_flg == FALSE)
                  {
                    /****************************************************
                    * starting time has occurred, no operating on PDTCH *
                    ****************************************************/
                    switch(idle_data_ptr->access_granted)
                    {
                      case one_phase :
                           /*****************************************************************************
                           * wait for L1_MAC_1ST_DATA_BLOCK_SENT before going to 1 phase contention res *
                           *****************************************************************************/

                           if (global_data_ptr->tbf_type == (uint8)dynamic_alloc_e)
                           {
                             /* clear packet channel request references stored during the access phase*/
                             gmac_start_t3164(gas_id);
                             MSG_GERAN_MED_0_G("GMAC State Idle UL_1P_CONTENTION_RES");
                             idle_data_ptr->mac_idle_substate = UL_1P_CONTENTION_RES;
                             (void)mac_log_state(gas_id);
                           }
                           else
                           {
                             MSG_GERAN_LOW_1_G( "ERROR :-TBF type not valid for 1 phase, tbf_type =%d",global_data_ptr->tbf_type);
                           }
                      break;

                      case two_phase :
                           rr_read_public_store(RRPS_CELL_OPTIONS_T3168,&idle_data_ptr->t3168_timeout,gas_id);
                           gmac_start_t3168(idle_data_ptr->t3168_timeout,gas_id);

                           MSG_GERAN_MED_0_G("GMAC State Idle UL_2P_ASS");
                           idle_data_ptr->mac_idle_substate = UL_2P_ASS;
                           (void)mac_log_state(gas_id);
                      break;

                      case single_block :
                           /*No action, wait for L1_MAC_SINGLE_BLOCK_SENT*/
                      break;

                      default:
                           MSG_GERAN_ERROR_3_G( "Unknown access granted in state %d task id %d access granted %d",idle_data_ptr->mac_idle_substate,
                                                                                                                  task_id,
                                                                                                                  idle_data_ptr->access_granted );
                           gmac_idle_stop_access_timers(idle_data_ptr->mac_idle_substate,
                                                        gas_id);

                           if(global_data_ptr->mac_action == UL_DATA_EST)
                           {
                             /*GMAC sends block signal to RLC and will be unblocked rlc once gmac received release conf from L1*/
                             gmac_idle_send_no_access_to_rlc(gas_id);
                           }

                           MSG_GERAN_HIGH_0_G("GMAC INFO release_cause : ACCESS_RETRY");
                           idle_data_ptr->release_cause = ACCESS_RETRY;

                           gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig,
                              UL_TBF_ABNORMAL,
                              MAC_LOG_UL_UNKNOWN_ERROR,
                              MAC_LOG_DL_UNKNOWN_ERROR,
                              gas_id );

                      break;
                    }
                  }
                  else
                  {
                    MSG_GERAN_MED_0_G("GMAC releasing L1, so ignoring MAC_PH_CONNECT_IND signal");
                  }

             break;

             case L1_MAC_SINGLE_BLOCK_SENT:
                  if(global_data_ptr->c_flags.tbf_release_pending_flg == FALSE)
                  {
                    MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_SINGLE_BLOCK_SENT");
                    if((global_data_ptr->mac_action == PMR_EST) || (global_data_ptr->mac_action == PCCF_EST))
                    {
                      if(global_data_ptr->mac_action == PMR_EST )
                      {
                        MSG_GERAN_MED_0_G( "MAC sent MAC_GRR_MEAS_RPT_CNF");
                        gmac_util_send_pmr_cnf_to_grr(MAC_GRR_SUCCESS,gas_id);

                        /*reset to the default PMR params once PMR msg has been sent*/
                        memset(&global_data_ptr->gmac_local_pmr_params,0,sizeof(gmac_pmr_params_t));

                        /*Single block procedure is completed so unblocking RLC*/
                       gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_IDLE,__LINE__,gas_id);
                        /*Resetting idle parameters*/
                        gmac_idle_reset_parameters(idle_data_ptr->mac_idle_substate,gas_id);
                      }
                      else if(global_data_ptr->mac_action == PCCF_EST)
                      {
                        MSG_GERAN_MED_0_G( "MAC sent MAC_GRR_PCCF_SENT");
                        /*sending PCCF_CNF with successful signal to GRR*/
                        gmac_util_complete_pccf_process(SUCCESSFULLY_SENT_PCCF,MAC_INITIATED,GMAC_IDLE,__LINE__,gas_id);

                       gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_IDLE,__LINE__,gas_id);
                        /*resetting idle parameters*/
                        gmac_idle_reset_parameters(idle_data_ptr->mac_idle_substate,gas_id);
                      }

                      MSG_GERAN_MED_0_G( "MAC sent MAC_GRR_MAC_STATUS");
                      gmac_util_send_mac_grr_mac_status(&idle_data_ptr->mac_grr_sig,MAC_IDLE,gas_id);

                      global_data_ptr->mac_action = NONE;
                      display_mac_action(global_data_ptr->mac_action,gas_id);

                      MSG_GERAN_MED_0_G("GMAC State Idle PASSIVE");
                      idle_data_ptr->mac_idle_substate = PASSIVE;
                      (void)mac_log_state(gas_id);

                      reset_tbf_parameters(gas_id);
                      global_data_ptr->mac_mode = NULL_MODE;
                    }
                  }
                  else
                  {
                    MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_SINGLE_BLOCK_SENT, while tbf release is pending");
                  }

             break;

             case L1_MAC_USF_DETECTED:
                  /* no action in idle mode,its the transfer of the     */
                  /* 1'st data block that's critical. ref 4.60, 7.1.2.3 */
             break;

             case L1_MAC_51_SB_FAILURE:
                  MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_51_SB_FAILURE");
                  /*L1_MAC_51_SB_FAILURE acts as implicit release by L1.        */
                  /*L1 has already returned to idle.                            */
                  /*If RLC was establishing an UL, need to block and unblock RLC*/
                  /*If GRR requested the est, then RLC has been blocked and     */
                  /*needs to be unblocked                                       */

                  if(global_data_ptr->c_flags.tbf_release_pending_flg == FALSE)
                  {
                    gmac_reset_rac_params(gas_id);

                    if(global_data_ptr->mac_action == UL_DATA_EST)
                    {
                      gmac_util_block_rlc(NO_DEL,GMAC_IDLE,__LINE__,gas_id);
                    }
                    else if(global_data_ptr->mac_action == PMR_EST)
                    {
                      global_data_ptr->gmac_local_pmr_params.pmr_pending = FALSE;
                      gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
                    }

                    /*Always unblock RLC when rleasing the TBF*/
                    gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_IDLE,__LINE__,gas_id);

                    /*send mac status before attempting to complete PCCO process*/
                    MSG_GERAN_MED_0_G("MAC_GRR_MAC_STATUS");
                    gmac_util_send_mac_grr_mac_status(&idle_data_ptr->mac_grr_sig,MAC_IDLE,gas_id);

                    /*complete PCCF process activities*/
                    gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,MAC_INITIATED,GMAC_IDLE,__LINE__,gas_id);

                    /*complete PCCO process activites*/
                    gmac_util_complete_pcco_process(PCCO_FAILURE_NO_RESPONSE,MAC_INITIATED,GMAC_IDLE,__LINE__,gas_id);

                    global_data_ptr->mac_mode = NULL_MODE;

                    idle_data_ptr->access_attempts_count = 0;

                    MSG_GERAN_MED_0_G("GMAC State Idle PASSIVE");
                    idle_data_ptr->mac_idle_substate = PASSIVE;
                    (void)mac_log_state(gas_id);
                    reset_tbf_parameters(gas_id);
                    gmac_idle_reset_parameters(idle_data_ptr->mac_idle_substate,gas_id);
                    global_data_ptr->mac_action = NONE;
                    display_mac_action(global_data_ptr->mac_action,gas_id);
                  }
                  else
                  {
                    MSG_GERAN_MED_0_G("GMAC releasing L1, so ignoring L1_MAC_51_SB_FAILURE signal");
                  }

             break;


             case L1_MAC_TBF_REL_CONFIRM :

                  global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;

                  gmac_idle_ul_release_confirm_handler(gas_id);

                  idle_data_ptr->release_cause = NO_RELEASE_CAUSE;
                  (void)mac_log_state(gas_id);

             break;

             default:
                   MSG_GERAN_ERROR_3_G( "Unexpected L1 sig rx. idle state %d task id %d sig id 0x%x",idle_data_ptr->mac_idle_substate,
                                                                                                     task_id,
                                                                                                     signal_id);
             break;
           } /*end signal_id switch*/

         }
         /**********************************************************
         *             signals received from GRR                   *
         **********************************************************/
         else if(task_id == MS_MAC_RR)
         {
           display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,gas_id);

           switch (signal_id)
           {
               case GRR_MAC_NO_PS_ACCESS_IND:
                    /*GMAC rcvd NO PS ACCESS IND from GRR. GMAC will abort the access procedure
                      and block RLC. GMAC will move to NULL INACTIVE state once it receives rel
                      cnf from layer1*/
                    MSG_GERAN_MED_1_G("GMAC Rcvd GRR_MAC_NO_PS_ACCESS_IND pdu deletion type %d",
                                      global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);
                    gmac_idle_start_no_ps_access_process(idle_data_ptr->mac_idle_substate,
                                                         &idle_data_ptr->mac_l1_sig,
                                                         global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                                         gas_id);
                    global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
               break;

               case GRR_MAC_SEND_PCCF:
                    MSG_GERAN_MED_0_G("GMAC received GRR_MAC_SEND_PCCF from GRR");
                    /*build up PCCF msg and store it in csn_encoder_ul_msg for use later*/
                    /*when the UL has been established, use PACCH to send PCCF*/
                    if (gmac_util_encode_ul_control_msg_handler(PCCF,FALSE,gas_id) == PASS)
                    {
                      gmac_util_start_pccf_process(SENDING_VIA_UL_PACCH,GMAC_IDLE,__LINE__,(uint8)idle_data_ptr->mac_idle_substate,gas_id);
                    }
                    else
                    {
                      MSG_GERAN_MED_0_G("CSN.1 encode failure of PCCF, wait in state till t3176 exp");
                    }
               break;

               case GRR_MAC_UL_SB_EST:
                 MSG_GERAN_ERROR_0_G("GMAC Rcvd GRR_MAC_UL_SB_EST, Not expected in this state!");
               break;

               case GRR_MAC_DL_TBF_EST:
                 MSG_GERAN_ERROR_0_G("MAC info Can't get this GRR_MAC_DL_TBF_EST in UL_1P_ASS");
               break;

#ifdef FEATURE_GSM_EDTM
               case GRR_MAC_EDTM_PKT_CS_EST_REQ:
                 MSG_GERAN_MED_0_G("GMAC Rejected GRR_MAC_EDTM_PKT_CS_EST_REQ, Not in Contention Resolution");
                 gmac_send_mac_grr_edtm_pkt_cs_est_rej(EDTM_CS_REJ_CANNOT_SERVICE_REQUEST,gas_id);
               break;
#endif /*FEATURE_GSM_EDTM*/

               default:
                   MSG_GERAN_ERROR_3_G( "Unexpected GRR sig rx. idle state %d task id %d sig id 0x%x",idle_data_ptr->mac_idle_substate,
                                                                                                      task_id,
                                                                                                      signal_id);
               break;

           } /*end of signal switch*/

         }
         /**********************************************************
         *             signals received from RLC                   *
         **********************************************************/
         else if(task_id == MS_MAC_RLC)
         {
           display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,gas_id);

           switch (signal_id)
           {
               case RM_UL_EST_TBF_REQ :
                    if (global_data_ptr->mac_action == UL_DATA_EST)
                    {
                      MSG_GERAN_ERROR_1_G( "MAC Rcvd RM_UL_EST_TBF_REQ and mac action is %d",global_data_ptr->mac_action);
                      MSG_GERAN_ERROR_0_G( "Unexpected RM_UL_EST_TBF_REQ rcvd from RLC");
                    }
                    else
                    {
                      if(gmac_util_is_rlc_blocked(GMAC_IDLE,__LINE__,gas_id)== UNBLOCKED)
                      {
                        MSG_GERAN_ERROR_1_G( "GMAC should have already blocked rlc at this point mac action is %d",
                                             global_data_ptr->mac_action);
                      }
                    }
               break;

               default:
                   MSG_GERAN_ERROR_3_G( "Unexpected RLC sig rx. idle state %d task id %d sig id 0x%x",idle_data_ptr->mac_idle_substate,
                                                                                                      task_id, signal_id);
               break;
           }

         }
         /**********************************************************
         *                 MAC internal signals                    *
         **********************************************************/
         else if(task_id == MS_MAC_MAC)
         {
           display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,gas_id);

           switch (signal_id)
           {
               case MAC_TIMER_T3168:
                    /* added t3168 exp handling for the case where l1 fails to send         */
                    /* MAC_PH_CONNECT_IND when sending PRR via SB alloc. Should never happen*/
                    MSG_GERAN_MED_1_G( "MAC info timer t3168 expired ",0 );

                    /* if ul tbf fails before reaching transfer, L1 must always be */
                    /* release irrespective if a access retry is to be performed   */

                    idle_data_ptr->release_cause = ACCESS_RETRY;
                    /*GMAC sends block signal to RLC and will be unblocked rlc once gmac received release conf from L1*/
                    gmac_idle_send_no_access_to_rlc(gas_id);
                    gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,MAC_LOG_UL_EXPIRED_T3168,MAC_LOG_DL_UNKNOWN_ERROR,gas_id );
               break;

               case MAC_TIMER_T3200_1:
               case MAC_TIMER_T3200_2:
               case MAC_TIMER_T3200_3:
               case MAC_TIMER_T3200_4:
               case MAC_TIMER_T3200_5:
               case MAC_TIMER_T3200_6:
               case MAC_TIMER_T3200_7:
               case MAC_TIMER_T3200_8:
                    MSG_GERAN_MED_1_G("MAC info timer t3200 expired si_id %d",signal_id );
                    t3200_expiry_handler((mac_mac_sig_id_t)signal_id,gas_id);
               break;

               default:
                   MSG_GERAN_ERROR_3_G( "Unexpected MAC sig rx. idle state %d task id %d sig id 0x%x",idle_data_ptr->mac_idle_substate,
                                                                                                      task_id, signal_id);
               break;
           }

         }
         else
         {
         /*error*/
         }

      break;

      /*****************************************************************
      *
      *   UPLINK 2 PHASE ASSIGNMENT  STATE
      *
      ******************************************************************/
      case UL_2P_ASS:

         /**********************************************************
         *             signals received from L1                    *
         **********************************************************/
         if(task_id == MS_MAC_L1)
         {
           display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,gas_id);

           switch (signal_id)
           {
             case MAC_PH_DATA_IND :

                display_dl_msg_rcvd(global_data_ptr->channel_msg,__LINE__,idle_data_ptr->mac_idle_substate, global_data_ptr->mac_action,gas_id);

                switch (global_data_ptr->channel_msg)
                {
                    case PACKET_CELL_CHANGE_ORDER :
                         if(gmac_idle_check_pcco_addressing(gas_id))
                         {
                         /***********************************************************
                         * when PCCO addresses MS stop relevent rlc/mac timers ref  *
                         * 4.60, 7.4.1 release l1, wait for L1_MAC_TBF_REL_CONFIRM  *
                         * and start pcco cell reselection process                  *
                         ************************************************************/
                           MSG_GERAN_LOW_0_G("MAC Info forwards PCCO to GRR");
                           rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);
                           gmac_send_data_ind_to_grr(gas_id);
                         }
                         else
                         {
                           MSG_GERAN_LOW_0_G("MAC Info PCCO not for MS");
                         }
                    break;

                    case PACKET_UPLINK_ASSIGNMENT:
                       /*******************************************************************
                       * DL msg address MS correctly, check if pca should be sent         *
                       *******************************************************************/

                       /*cr70960*/
                       /* if UL releasing then ignore freq_confirmation and wait for rel confrim*/
                         if(idle_data_ptr->release_cause == NO_RELEASE_CAUSE)
                         {
                            if(gmac_idle_check_pua_addressing(global_data_ptr->idle_mode_ul_tbf_release_action,idle_data_ptr->mac_idle_substate,gas_id ) == TRUE)
                            {
                              MSG_GERAN_MED_2_G( "MAC Rcvd PUA ,m_s %d,m_a %d",idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action);
                              rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);

                              /*store in mac idle the  tbf type , 1 phase, 2 phase, sb, short ...etc*/
                              global_data_ptr->tbf_type = (uint8)((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->allocation_flag;

                              if (gmac_dl_msg_data_valid(gas_id) == FALSE)
                              {
                                idle_data_ptr->temp_egprs_2p_sb_mb_alloc = global_data_ptr->egprs_2p_sb_mb_alloc;

                                idle_data_ptr->pua_data_result = gmac_idle_build_rlc_alloc_and_l1_config_sigs(global_data_ptr->tbf_type,
                                                                                                              global_data_ptr->mac_action,&idle_data_ptr->mr_ul_alloc_cnf_sig,
                                                                                                              gas_id);


                                  /* Log UL TBF EST in pua_xx_handler() with TS Info */
                                  /*
                                    mac_log_ul_tbf_establish(gmac_local_tfi_store.ul_tfi);
                                  */

                                if(idle_data_ptr->pua_data_result == NO_ERRORS)
                                {
                                  /*******************************************************************
                                  *   no errors in tbf config data, validate frequency parameters    *
                                  *******************************************************************/

                                  if (((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
                                  {
                                    idle_data_ptr->pua_data_result = gmac_util_frequency_param_ie(FALSE,TRUE,gas_id);
                                  }
                                  else
                                  {
                                    gmac_idle_handle_no_freq_condition_in_pua(gas_id);
                                  }

                                  /***********************************************
                                  *  action on validity of  frequency assignment *
                                  ************************************************/

                                  if(idle_data_ptr->pua_data_result == NO_ERRORS)
                                  {
                                    gmac_stop_t3168(gas_id);
                                    /*   reset access_attempts_count   */
                                    idle_data_ptr->access_attempts_count = 0;

                                    if ( ( global_data_ptr->access_tech_req_present   == TRUE ) &&
                                         ( global_data_ptr->current_tbf_mode          == TBF_MODE_EGPRS ) &&
                                         ( global_data_ptr->tbf_type                  == (uint8)dynamic_alloc_e ) &&
                                         ( idle_data_ptr->temp_egprs_2p_sb_mb_alloc == EGPRS_SB_ALLOC ||
                                           idle_data_ptr->temp_egprs_2p_sb_mb_alloc == MB_ALLOC_OF_1_BLOCK ) )
                                    {
                                      MSG_GERAN_MED_0_G("MAC info NW request Access Tech in dynamic alloc, Sending reformatted ARAC");

                                      gmac_idle_egprs_rac_procedure_2(TRUE,global_data_ptr->c_flags.egprs_rac_retran_req,TRUE,FALSE,gas_id);
                                    }
                                    gmac_idle_simulate_freq_cnf_for_ul_states(gas_id);
                                  }
                                  else
                                  {
                                    gmac_idle_handle_pua_error_result(gas_id);
                                  }
                                }
                                else
                                {
                                  gmac_idle_handle_pua_error_result(gas_id);
                                }
                              }
                              else
                              {
                                 /*csn.1 decode error, abnormal release perform retry , ref 4.60 7.1.4*/
                                MSG_GERAN_ERROR_0_G("MAC CSN.1 decode error in PUA ");

                                 /*2 phase access timers (prr)*/
                                gmac_stop_t3168(gas_id);

                                 /* if access retry process fails release l1 and rlc/grr */
                                 /* abort uplink est and return to idle                  */

                                 /* if ul tbf fails before reaching transfer, L1 must always be */
                                 /* release irrespective if a access retry is to be performed   */

                                idle_data_ptr->release_cause = ACCESS_RETRY;
                                /*GMAC sends block signal to RLC and will be unblocked rlc once gmac received release conf from L1*/
                                gmac_idle_send_no_access_to_rlc(gas_id);
                                gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,MAC_LOG_UL_CSN1_DECODING_ERROR,MAC_LOG_DL_UNKNOWN_ERROR,
                                                              gas_id );
                              }
                            }
                            else
                            {
                              /* According to section 7.1.3 from 44.060, mismatch in TLLI should ignore the PUA. GMAC wait for correct PUA with TLLI/T3168
                                 expiry*/
                              MSG_GERAN_MED_0_G( "GMAC Rcvd PUA with tlli mis-match. GMAC Ignores this and wait for PUA with correct TLLI /T3168 expiry");
                            }
                          }
                          else
                          {
                             /* UL releasing then igonre PUA and wait for rel confrim*/
                            MSG_GERAN_HIGH_1_G("MAC info Ul releasing release casue %d PUA ignored",idle_data_ptr->release_cause);
                          }
                    break;

                    case PACKET_ACCESS_REJECT:
                       /**********************************************************
                       *  For 2 phase NO retry is performed when PAR is recieved *
                       *  ref 4.60, 7.1.3.2.1 when addressed correctly and wait  *
                       * ind present.                                            *
                       **********************************************************/

                         if(check_idle_par_addressing(idle_data_ptr->access_granted,gas_id) == TRUE)
                         {
                           MSG_GERAN_MED_2_G( "MAC Rcvd PAR ,m_s %d,m_a %d",idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action);
                           gmac_stop_t3168(gas_id);


                           if( rex_get_timer(&mac_timers_ptr->timer_t3172) != 0 )
                           {
                             /*if t3172 running, don't perfrom access retry*/
                             idle_data_ptr->release_cause = PAR_RCVD;
                             /*GMAC will send del one to GRLC in the following cases.
                               1. PCCO is ACTIVE and establishment cause is CELL UPDATE
                               2. NO PCCO ACTIVE case
                               GMAC will send no del to GRLC only when PCCO is ACTIVE and establishment cause is not CELL UPDATE
                             */
                             gmac_idle_send_del_one_or_no_del_to_rlc(gas_id);
                           }
                           else
                           {
                             idle_data_ptr->release_cause = ACCESS_RETRY;

                             /* if doing PCCO then do not attempt an access retry 44.060 8.4.2*/
                             if ((global_data_ptr->mac_action == UL_DATA_EST) &&
                                 (gmac_util_get_pcco_state(gas_id) == PCCO_ACTIVE))
                             {
                               if(global_data_ptr->gmac_util_data_req_cause == GRR_CAUSE_GMM_CELL_UPDATE)
                               {
                                 /*PCCO is ACTIVE and the establishment cause is CELL UPDATE. So GRLC has to
                                   delete the cell update. so GMAC will send DEL ONE to GRLC*/
                                 gmac_util_block_rlc(DEL_ONE,GMAC_IDLE,__LINE__,gas_id);
                               }
                               else
                               {
                                 /*PCCO ACTIVE and the establishment cause is not CELL UPDATE. So GMAC will
                                   sends NO DEL to GRLC*/
                                 gmac_util_block_rlc(NO_DEL,GMAC_IDLE,__LINE__,gas_id);
                               }
                               idle_data_ptr->access_attempts_count = 0;
                             }
                             else
                             {
                               /*GMAC sends block signal to RLC and will be unblocked rlc once gmac received release conf from L1*/
                               gmac_idle_send_no_access_to_rlc(gas_id);
                             }
                           }
                           /*note only valid actions for 2 phase are ul_data_est and pcco_est*/
                           /*PCCF uses SB alloc no need to release L1                        */
                           /*wait in state for L1_MAC_TBF_REL_CONFIRM                        */
                           gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,MAC_LOG_UL_ACCESS_REJECT,MAC_LOG_DL_UNKNOWN_ERROR,gas_id );
                         }
                    break;

                    case PACKET_MEASUREMENT_ORDER:
                         gmac_util_pmo_handler(gas_id);
                    break;

                    default :
                        MSG_GERAN_MED_3_G( "Unexpected DL msg rx. idle state %d task id %d msg id %d",idle_data_ptr->mac_idle_substate,
                                                                                                      task_id,
                                                                                                      global_data_ptr->channel_msg);
                    break;

                } /*end of channal msg switch*/

             break;

             case L1_MAC_FREQ_ASS_CNF:
                  MSG_GERAN_HIGH_0_G("GMAC Info L1_MAC_FREQ_ASS_CNF rxd, Ignored for freq cnf tidy up");

                  if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_freq_ass_cnf.band_supported == FALSE)
                  {
                    MSG_GERAN_ERROR_0_G("GMAC Freq Cnf Cleanup, L1 indicating band NOT supported");
                  }
             break;

             case MAC_PH_CONNECT_IND:

                   /*if tbf release pending is true then there is no point in going to tranfer
                     GMAC will wait for release confrim in this state itself*/
                  if(global_data_ptr->c_flags.tbf_release_pending_flg == FALSE)
                  {
                    if(global_data_ptr->tbf_type == (uint8)dynamic_alloc_e)
                    {
                      MSG_GERAN_MED_0_G( "MAC sent MAC_IT_UL_DYNAMIC");

                      memset(&(idle_data_ptr->mac_int_sig.msg.mac_it_ul_dynamic),0,sizeof(mac_it_ul_dynamic_t));

                      gmac_idle_send_mac_internal_sig(&idle_data_ptr->mac_int_sig,MAC_IT_UL_DYNAMIC,gas_id);

                      gmac_idle_reset_parameters(idle_data_ptr->mac_idle_substate,gas_id);
                      /*complete the pccf process activities*/
                      gmac_util_complete_pccf_process(SUCCESSFULLY_SENT_PCCF,MAC_INITIATED,GMAC_IDLE,__LINE__,gas_id);

                      global_data_ptr->mac_action = NONE;
                      display_mac_action(global_data_ptr->mac_action,gas_id);
                      MSG_GERAN_MED_0_G("GMAC State Idle PASSIVE");
                      idle_data_ptr->mac_idle_substate = PASSIVE;
                      (void)mac_log_state(gas_id);

                      /*note stopping of timer t3164 is handled in TRANSFER*/
                      /*on reception of L1_MAC_1ST_DATA_BLOCK_SENT         */
                      if(global_data_ptr->tbf_type == (uint8)dynamic_alloc_e)
                      {
                        gmac_start_t3164(gas_id);
                      }

                      MSG_GERAN_MED_0_G("MAC mode TRANSFER");
                      global_data_ptr->mac_mode = TRANSFER_MODE;

                      MSG_GERAN_MED_0_G( "MAC sent MAC_GRR_MAC_STATUS ");
                      gmac_util_send_mac_grr_mac_status(&idle_data_ptr->mac_grr_sig,MAC_TRANSFER,gas_id);
                    }
                    else if(global_data_ptr->tbf_type == (uint8)multi_blk_allocation)
                    {
                      MSG_GERAN_MED_0_G( "2p rac retran completed, wait in state for DYN alloc");
                    }
                    else
                    {
                      MSG_GERAN_MED_1_G( "MAC info ERROR tbf type not relevent for 2p ass tbf_type %d",global_data_ptr->tbf_type);
                    }
                  }
                  else
                  {
                    MSG_GERAN_MED_0_G("GMAC releasing L1, so ignoring MAC_PH_CONNECT_IND signal");
                  }
             break;

             case L1_MAC_USF_DETECTED:
                  /*no action should never get this in 2 phase access*/
             break;

             case L1_MAC_1ST_DATA_BLOCK_SENT:
                  /************************************************************
                  * only relevant for dynamic, used to stop t3164 for 2 phase *
                  * access this  should not be received.The handling of t3164 *
                  * expiry access retry process is  initiated from transfer   *
                  ************************************************************/
             break;

             case L1_MAC_TBF_REL_CONFIRM :

                  global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;

                  gmac_idle_ul_release_confirm_handler(gas_id);

                  idle_data_ptr->release_cause = NO_RELEASE_CAUSE;
                  (void)mac_log_state(gas_id);
             break;

             case L1_MAC_51_SB_FAILURE:
                  /***********************************************
                  * Failure to send PRR during 51 2 phase access *
                  * note l1 implicitly returns to its idle state *
                  ************************************************/

                  if(global_data_ptr->c_flags.tbf_release_pending_flg == FALSE)
                  {

                    MSG_GERAN_MED_0_G("MAC Rcvd L1_MAC_51_SB_FAILURE");
                    MSG_GERAN_MED_0_G("MAC_GRR_MAC_STATUS");
                    /*send mac status before attempting to complete PCCO process*/
                    gmac_util_send_mac_grr_mac_status(&idle_data_ptr->mac_grr_sig,MAC_IDLE,gas_id);
                    if(global_data_ptr->mac_action == UL_DATA_EST)
                    {
                      /*Block RLC and unblock RLC*/
                      gmac_util_block_rlc(NO_DEL,GMAC_IDLE,__LINE__,gas_id);
                      gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_IDLE,__LINE__,gas_id);
                      /*complete PCCF process activities*/
                      gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,MAC_INITIATED,GMAC_IDLE,__LINE__,gas_id);
                      /*complete PCCO process activites*/
                      gmac_util_complete_pcco_process(PCCO_FAILURE_NO_RESPONSE,MAC_INITIATED,GMAC_IDLE,__LINE__,gas_id);
                      global_data_ptr->mac_mode = NULL_MODE;
                      idle_data_ptr->access_attempts_count = 0;
                    }
                    MSG_GERAN_MED_0_G("GMAC State Idle PASSIVE");
                    idle_data_ptr->mac_idle_substate = PASSIVE;
                    (void)mac_log_state(gas_id);
                    reset_tbf_parameters(gas_id);
                    gmac_idle_reset_parameters(idle_data_ptr->mac_idle_substate,gas_id);
                    global_data_ptr->mac_action = NONE;
                    display_mac_action(global_data_ptr->mac_action,gas_id);
                  }
                  else
                  {
                    MSG_GERAN_MED_0_G("GMAC releasing L1, so ignoring L1_MAC_51_SB_FAILURE signal");
                  }
             break;

             default :
                  MSG_GERAN_ERROR_3_G( "Unexpected L1 sig rx. idle state %d task id %d sig id 0x%x",
                                       idle_data_ptr->mac_idle_substate,task_id, signal_id);
             break;

           } /*end of l1 signal switch*/
         }
         /**********************************************************
         *             signals received from GRR                   *
         **********************************************************/
         else if(task_id == MS_MAC_RR)
         {
           display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,gas_id);

           switch (signal_id)
           {
               case GRR_MAC_NO_PS_ACCESS_IND:
                    /*GMAC rcvd NO PS ACCESS IND from GRR. GMAC will abort the access procedure
                      and block RLC. GMAC will move to NULL INACTIVE state once it receives rel
                      cnf from layer1*/
                    MSG_GERAN_MED_1_G("GMAC Rcvd GRR_MAC_NO_PS_ACCESS_IND pdu deletion type %d",
                                      global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);
                    gmac_idle_start_no_ps_access_process(idle_data_ptr->mac_idle_substate,
                                            &idle_data_ptr->mac_l1_sig,
                                             global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                             gas_id);
                    global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
                break;

               case GRR_MAC_SEND_PCCF:
                    MSG_GERAN_MED_0_G("GMAC received GRR_MAC_SEND_PCCF from GRR");
                    /*build up PCCF msg and store it in csn_encoder_ul_msg for use later*/
                    /*when the UL has been established, use PACCH to send PCCF*/
                    if (gmac_util_encode_ul_control_msg_handler(PCCF,FALSE,gas_id) == PASS)
                    {
                      gmac_util_start_pccf_process(SENDING_VIA_UL_PACCH,GMAC_IDLE,__LINE__,(uint8)idle_data_ptr->mac_idle_substate,gas_id);
                    }
                    else
                    {
                      MSG_GERAN_MED_0_G("CSN.1 encode failure of PCCF, wait in state till t3176 exp");
                    }
               break;
#ifdef FEATURE_GSM_EDTM
               case GRR_MAC_EDTM_PKT_CS_EST_REQ:
                 MSG_GERAN_MED_1_G("GMAC Accepted GRR_MAC_EDTM_PKT_CS_EST_REQ rr_est_cause: 0x%x in 2 Phase Cont Res",
                                   global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause);

                 gmac_start_t3196(gas_id);
                 global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_cause =
                     global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause;
               break;
#endif /*FEATURE_GSM_EDTM*/

               default:
                   MSG_GERAN_ERROR_3_G( "Unexpected GRR sig rx. idle state %d task id %d sig id 0x%x",
                                        idle_data_ptr->mac_idle_substate,task_id, signal_id);
               break;

           } /*end of signal switch*/
         }
         /**********************************************************
         *             signals received from RLC                   *
         **********************************************************/
         else if(task_id == MS_MAC_RLC)
         {
           display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,gas_id);

           switch (signal_id)
           {
               case RM_UL_EST_TBF_REQ :
                    if (global_data_ptr->mac_action == UL_DATA_EST)
                    {
                      MSG_GERAN_ERROR_1_G( "MAC Rcvd RM_UL_EST_TBF_REQ and mac action is %d",global_data_ptr->mac_action);
                      MSG_GERAN_ERROR_0_G( "Unexpected RM_UL_EST_TBF_REQ rcvd from RLC");
                    }
                    else
                    {
                      if(gmac_util_is_rlc_blocked(GMAC_IDLE,__LINE__,gas_id)== UNBLOCKED)
                      {
                        MSG_GERAN_ERROR_1_G( "GMAC should have already blocked rlc at this point mac action is %d",
                                             global_data_ptr->mac_action);
                      }
                    }
               break;

               case RM_UL_RELEASE_IND:

                    gmac_stop_t3168(gas_id);

                    /*exception handler:- RLC has detected an expection case*/
                    MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_RELEASE_IND  perform exception recovery");

                    if(idle_data_ptr->release_cause == GRR_INITIATED_NO_ACCESS )
                    {
                      MSG_GERAN_MED_1_G("MAC UL est already being released abnormally rel code =%d. No action required",idle_data_ptr->release_cause);
                    }
                    else
                    {
                      idle_data_ptr->release_cause = UL_EXCEPTION_HANDLING;
                    }
                    /*GMC block RLC and will unblock RLC once GMAC received rel cnf from L1*/
                    gmac_util_block_rlc(NO_DEL,GMAC_IDLE,__LINE__,gas_id);
                    gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,MAC_LOG_UL_UNKNOWN_ERROR,MAC_LOG_DL_UNKNOWN_ERROR,gas_id );
               break;


               default:
                   MSG_GERAN_ERROR_3_G( "Unexpected RLC sig rx. idle state %d task id %d sig id 0x%x",
                                        idle_data_ptr->mac_idle_substate,task_id, signal_id);
               break;
           }
         }
         /**********************************************************
         *                 MAC internal signals                    *
         **********************************************************/
         else if(task_id == MS_MAC_MAC)
         {
           display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,gas_id);

           switch (signal_id)
           {
               case MAC_TIMER_T3168:
                    MSG_GERAN_MED_1_G( "MAC info timer t3168 expired  ",0 );

                    /* Catch race condition where MAC has stopped timer for TBF release
                       but timer still expires */
                    if(global_data_ptr->c_flags.tbf_release_pending_flg == FALSE)
                    {
                      idle_data_ptr->release_cause = ACCESS_RETRY;
                      /*GMAC sends block signal to RLC and will be unblocked rlc once gmac received release conf from L1*/
                      gmac_idle_send_no_access_to_rlc(gas_id);
                      gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,MAC_LOG_UL_EXPIRED_T3168,MAC_LOG_DL_UNKNOWN_ERROR,gas_id );
                      gmac_send_mac_grr_abnormal_release(RR_CONTENTION_FAILED, gas_id);
                    }
                    else
                    {
                      MSG_GERAN_ERROR_0_G("MAC releasing TBF, Ignore T3168 expiry ");
                    }
               break;

               case MAC_TIMER_T3164:
                    MSG_GERAN_HIGH_0_G("MAC timer t3164 expired");
                    /******************************************************
                    * Note Timer t3164 is only relevant for dynamic alloc *
                    * it is started in idle mode but its cancellation or  *
                    * action on expiry of timer is handled in transfer    *
                    ******************************************************/
               break;

               case MAC_TIMER_T3200_1:
               case MAC_TIMER_T3200_2:
               case MAC_TIMER_T3200_3:
               case MAC_TIMER_T3200_4:
               case MAC_TIMER_T3200_5:
               case MAC_TIMER_T3200_6:
               case MAC_TIMER_T3200_7:
               case MAC_TIMER_T3200_8:
                    MSG_GERAN_MED_1_G( "MAC info timer t3200 expired si_id %d",signal_id );
                    t3200_expiry_handler((mac_mac_sig_id_t)signal_id,gas_id );
               break;

#ifdef FEATURE_GSM_EDTM
               case MAC_TIMER_T3196:
                 MSG_GERAN_MED_0_G("GMAC EDTM T3196 Timer expired");
                 gmac_send_mac_grr_edtm_pkt_cs_est_rej(EDTM_CS_REJ_T3196_EXPIRY,gas_id);
               break;
#endif /*FEATURE_GSM_EDTM*/

               default:
                   MSG_GERAN_ERROR_3_G( "Unexpected MAC sig rx. idle state %d task id %d sig id 0x%x",
                                        idle_data_ptr->mac_idle_substate,task_id, signal_id);
               break;

           }/*end mac mac sig switch*/

         }
         else
         {
         /*error*/
         }

      break;

      /*****************************************************************
      *
      *   UPLINK 1 PHASE CONTENTION RESOLUTION STATE
      *
      ******************************************************************/
      case UL_1P_CONTENTION_RES:

         if(task_id == MS_MAC_L1)
         {
           display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,gas_id);

           switch (signal_id)
           {
             case MAC_PH_DATA_IND :

                display_dl_msg_rcvd(global_data_ptr->channel_msg,__LINE__,idle_data_ptr->mac_idle_substate, global_data_ptr->mac_action,gas_id);

                switch (global_data_ptr->channel_msg)
                {
                    case PACKET_MEASUREMENT_ORDER :
                         if( ((packet_measurement_order_t *)global_data_ptr->csn_dec_data_ptr)->tlli_gtfi_pkt_ref_valid == tlli_valid)
                         {
                           rr_read_public_store(RRPS_TLLI,&idle_data_ptr->rrps_tlli,gas_id);
                           if (idle_data_ptr->rrps_tlli == ((packet_measurement_order_t *)global_data_ptr->csn_dec_data_ptr)->tlli)
                           {
                             MSG_GERAN_MED_0_G("MAC Rcvd PACKET_MEASUREMENT_ORDER addressed by TLLI");
                             gmac_send_data_ind_to_grr(gas_id);
                             rrbp_control_block_handler_2(PCA_DL_MSG,gas_id);
                           }
                         }
                         else
                         {
                           MSG_GERAN_MED_0_G("MAC info pmo rcvd not addressed by TLLI in 1p cont res");
                         }
                    break;

                    case PACKET_CELL_CHANGE_ORDER :

                         if(gmac_idle_check_pcco_addressing(gas_id))
                         {
                           if (((packet_cell_change_order_t *)global_data_ptr->csn_dec_data_ptr)->tlli_gtfi_pkt_ref_valid == tlli_valid)
                           {
                              /***********************************************************
                              * when PCCO addresses MS stop relevent rlc/mac timers ref  *
                              * 4.60, 7.4.1 release l1, wait for L1_MAC_TBF_REL_CONFIRM  *
                              * and start pcco cell reselection process                  *
                              ************************************************************/
                              MSG_GERAN_LOW_0_G("MAC Info forwards PCCO to GRR");
                              rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);
                              gmac_send_data_ind_to_grr(gas_id);
                           }
                           else
                           {
                             MSG_GERAN_LOW_0_G("MAC Info PCCO incorrectly addressing MS using UL TFI msg ignored");
                           }
                         }
                         else
                         {
                           MSG_GERAN_LOW_0_G("MAC Info PCCO not for MS");
                         }
                    break;

                    case PACKET_UPLINK_ASSIGNMENT:
                         /*******************************************************************
                         * DL msg address MS correctly, check if pca should be sent         *
                         *******************************************************************/

                         if(global_data_ptr->c_flags.tbf_release_pending_flg == FALSE)
                         {
                           /*If 1st data block has not been sent ie t3164 is running,   */
                           /*then ignore PUA while in contention res ref 44.060 7.1.2.3 */
                           if(rex_get_timer(&mac_timers_ptr->timer_t3164) ==0 )
                           {
                             idle_data_ptr->egprs_pua_contention_result = gmac_idle_egprs_pua_contention_res_checking(idle_data_ptr->mac_idle_substate,gas_id);

                             if( (gmac_idle_check_pua_addressing(global_data_ptr->idle_mode_ul_tbf_release_action,idle_data_ptr->mac_idle_substate, gas_id ) == TRUE
                                 && idle_data_ptr->egprs_pua_contention_result == PUA_CONT_RES_NOT_APPLICABLE))
                             {
                                 MSG_GERAN_MED_2_G( "MAC Rcvd PUA ,m_s %d,m_a %d",
                                                    idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action);
                                 rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);

                                 /*store in mac idle the  tbf type , 1 phase, 2 phase, sb, short ...etc*/
                                 global_data_ptr->tbf_type = (uint8)((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->allocation_flag;

                                 if (gmac_dl_msg_data_valid(gas_id) == FALSE)
                                 {


                                    /*unsolicited PUA rcvd if idle_ul_realloc_state were set then UL  */
                                    /*realloc is underway so the implication is that pua has been rcvd*/
                                    /* as part of realloc procedure                                   */
                                    if(idle_data_ptr->i_flags.idle_ul_realloc_state == IDLE_REALLOC_NO_ACTION)
                                    {
                                      idle_data_ptr->i_flags.rcvd_unsolicited_pua_waiting_for_connected_ind = TRUE;
                                    }

                                    idle_data_ptr->pua_data_result = gmac_idle_build_rlc_alloc_and_l1_config_sigs(global_data_ptr->tbf_type,
                                                                                                                  global_data_ptr->mac_action,
                                                                                                                 &idle_data_ptr->mr_ul_alloc_cnf_sig,
                                                                                                                  gas_id);

                                    if(idle_data_ptr->pua_data_result == NO_ERRORS)
                                    {
                                      /*********************************************************************
                                      * no errors in tbf config data, validate frequency parameters        *
                                      * Note during con res, MS is currently on PDTCH(ie PACCH) so if      *
                                      * there are no vlaid frequency params in PUA MS uses those currently *
                                      * being use by L1.                                                   *
                                      *********************************************************************/
                                      if (((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->freq_param_flag == valid)
                                      {
                                        idle_data_ptr->pua_data_result = gmac_util_frequency_param_ie(FALSE,TRUE,gas_id);

                                        if ( idle_data_ptr->pua_data_result == NO_ERRORS )
                                        {
                                          gmac_idle_simulate_freq_cnf_for_ul_states(gas_id);
                                        }
                                      }
                                      else
                                      {
                                        gmac_idle_handle_no_freq_condition_in_pua(gas_id);
                                      }
                                      /***********************************************
                                      *  action on validity of  frequency assignment *
                                      ***********************************************/
                                      if(idle_data_ptr->pua_data_result == NO_ERRORS)
                                      {
                                        gmac_stop_t3168(gas_id);

                                        /*if resource re-allocation during 1 phase contention res is in progress  */
                                        /*then idle_realloc_attempts_count will be greater then 0 hence PUA was   */
                                        /*rcvd as a result of resource re-allocation procedure                    */
                                        if(idle_data_ptr->i_flags.idle_ul_realloc_state != IDLE_REALLOC_NO_ACTION)
                                        {
                                          idle_data_ptr->i_flags.idle_ul_realloc_state = IDLE_REALLOC_WAITING_TO_CONNECT;
                                          MSG_GERAN_MED_0_G("GMAC info set idle_ul_realloc_state = IDLE_REALLOC_WAITING_TO_CONNECT");
                                        }

                                        idle_data_ptr->idle_realloc_attempts_count = 0;
                                      }
                                      else
                                      {

                                        idle_data_ptr->i_flags.rcvd_unsolicited_pua_waiting_for_connected_ind = FALSE;

                                        gmac_idle_handle_pua_error_result(gas_id);
                                      }
                                    }
                                    else
                                    {


                                      idle_data_ptr->i_flags.rcvd_unsolicited_pua_waiting_for_connected_ind = FALSE;

                                      gmac_idle_handle_pua_error_result(gas_id);
                                    }
                                 }
                                 else
                                 {
                                   /*csn.1 decode error, abnormal release perform retry , ref 4.60 7.1.4*/
                                   MSG_GERAN_ERROR_0_G("MAC CSN.1 decode error in PUA ");

                                   /*2 phase access timers (prr)*/
                                   gmac_stop_t3168(gas_id);

                                   idle_data_ptr->release_cause = ACCESS_RETRY;
                                   /*GMAC sends block signal to RLC and will be unblocked rlc once gmac received release conf from L1*/
                                   gmac_idle_send_no_access_to_rlc(gas_id);
                                   gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,MAC_LOG_UL_CSN1_DECODING_ERROR,MAC_LOG_DL_UNKNOWN_ERROR,
                                                                 gas_id );
                                 }
                             }
                             else
                             {
                               if(gmac_idle_check_pua_addressing(global_data_ptr->idle_mode_ul_tbf_release_action,idle_data_ptr->mac_idle_substate,
                                                                 gas_id ) == TRUE )
                               {
                                 if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
                                 {
                                   if(idle_data_ptr->egprs_pua_contention_result == PUA_CONT_RES_PASSED)
                                   {

                                     if (global_data_ptr->tbf_type == (uint8)dynamic_alloc_e)
                                     {

                                       MSG_GERAN_MED_0_G( "MAC sent MAC_IT_UL_DYN_EGPRS_PUA_RCVD_IN_CONT_RES");

                                       memset( &(idle_data_ptr->mac_int_sig.msg.mac_it_ul_epua_rcvd),0,
                                                sizeof(mac_it_ul_dyn_egprs_pua_rcvd_in_cont_res_t));

                                       idle_data_ptr->mac_int_sig.msg.mac_it_ul_epua_rcvd.idle_realloc_req_count = 0;

                                       /* NOTE if performing resource reallocation during contention res, *
                                       *  then the fact that we rcvd PUA completes the access procedure   *
                                       *  all relevant UL re-allocation counters should be re-set to zero*/

                                       MSG_GERAN_MED_1_G( "MAC info contention res UL re-alloc attempts count %d",
                                                          idle_data_ptr->idle_realloc_attempts_count);
                                       idle_data_ptr->idle_realloc_attempts_count = 0;


                                       /* note PUA is sent in MAC_IT_UL_DYN_EGPRS_PUA_RCVD_IN_CONT_RES sig         */
                                       /* Transfer then parses the PUA and switches to TRANSFER_UL_DYNAMIC_REALLOC */
                                       if(idle_data_ptr->i_flags.idle_ul_realloc_state != IDLE_REALLOC_NO_ACTION)
                                       {
                                         MSG_GERAN_MED_0_G("GMAC info set idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION");
                                         idle_data_ptr->i_flags.idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION;


                                         /*load and clear the pending realloc req  flag */
                                         idle_data_ptr->mac_int_sig.msg.mac_it_ul_epua_rcvd.pending_realloc_req = idle_data_ptr->i_flags.pending_realloc_req;
                                         idle_data_ptr->i_flags.pending_realloc_req = FALSE;
                                       }

                                       /*store over the air PUA format to be used by TRANSFER*/
                                       memset(&(idle_data_ptr->egprs_cont_res_pua),0,sizeof(packet_uplink_assignment_t));
                                       idle_data_ptr->egprs_cont_res_pua = * (packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr;

                                       idle_data_ptr->mac_int_sig.msg.mac_it_ul_epua_rcvd.egprs_c_r_pua_ptr = &idle_data_ptr->egprs_cont_res_pua;
                                       gmac_idle_send_mac_internal_sig(&idle_data_ptr->mac_int_sig,MAC_IT_UL_DYN_EGPRS_PUA_RCVD_IN_CONT_RES,
                                                                       gas_id);

                                     }

                                     /*gmac completes the pcco process activities*/
                                     gmac_util_complete_pcco_process(PCCO_SUCCESS,MAC_INITIATED,GMAC_IDLE,__LINE__,gas_id);
                                     global_data_ptr->mac_action = NONE;
                                     display_mac_action(global_data_ptr->mac_action,gas_id);
                                     MSG_GERAN_MED_0_G("GMAC State Idle PASSIVE");
                                     idle_data_ptr->mac_idle_substate = PASSIVE;
                                     (void)mac_log_state(gas_id);

                                     gmac_stop_t3166(gas_id);

                                     global_data_ptr->mac_mode = TRANSFER_MODE;
                                     MSG_GERAN_MED_1_G("MAC set mode %d",global_data_ptr->mac_mode);

                                     MSG_GERAN_MED_0_G( "MAC sent MAC_GRR_MAC_STATUS");
                                     gmac_util_send_mac_grr_mac_status(&idle_data_ptr->mac_grr_sig,MAC_TRANSFER,gas_id);
                                   }
                                   else if(idle_data_ptr->egprs_pua_contention_result == PUA_CONT_RES_FAILED)
                                   {
                                     /*******************************************************************
                                     * contention resolution failed, perform access retry if applicable *
                                     *******************************************************************/

                                     /* if ul tbf fails before reaching transfer, L1 must always be */
                                     /* release irrespective if a access retry is to be performed   */
                                     idle_data_ptr->release_cause = ACCESS_RETRY;
                                     /*GMAC sends block signal to RLC and will be unblocked rlc once gmac received release conf from L1*/
                                     gmac_idle_send_no_access_to_rlc(gas_id);
                                     /*GMAC stops contention resolution timers*/
                                     gmac_idle_stop_access_timers(idle_data_ptr->mac_idle_substate,gas_id);
                                     gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,MAC_LOG_UL_FAILED_CONTENTION_RES,MAC_LOG_DL_UNKNOWN_ERROR,gas_id );
                                   }
                                 }
                                 else
                                 {
                                   /* ignore pua msg*/
                                   MSG_GERAN_MED_0_G( "MAC Info ERROR GPRS PUA rcvd error");
                                 }
                               }
                               else
                               {
                                  /* ignore pua msg*/
                                 MSG_GERAN_MED_0_G( "MAC Rcvd PUA addressing error");
                               }
                             }

                           }
                           else
                           {
                             MSG_GERAN_MED_0_G(" PUA rcvd before 1st data block sent ignore");
                           }
                         }
                         else
                         {
                           MSG_GERAN_HIGH_0_G("GMAC Ignore PUA when releasing L1");
                         }
                    break;

                    case PACKET_UPLINK_ACK_NACK:
                         /*************************************************************
                         * perform 1 phase contention resolution.  check tfi and tlli *
                         * MR_UL_CONTENTION_IND sent from gmac_idle_puan_handler      *
                         *************************************************************/

                         /*During release of UL tbf while in contention res ignore further reception of PUAN's*/
                         if(global_data_ptr->c_flags.tbf_release_pending_flg == FALSE)
                         {
                           idle_data_ptr->contention_result =  gmac_idle_puan_handler(gas_id);

                           if(idle_data_ptr->contention_result != IGNORE_PUAN)
                           {
                             gmac_stop_t3166(gas_id);
                           }

                           if( idle_data_ptr->contention_result == CON_RES_TRANSFER )
                           {
                              /*****************************************************************
                              * DL msg address MS correctly,check if pca should be sent format *
                              * pca and send. note no action if PCA is not sent                *
                              *****************************************************************/
                              MSG_GERAN_MED_2_G( "MAC Rcvd PUAN ,m_s %d,m_a %d",
                                                 idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action);

                              rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);

                              /**********************************************************************
                              * contention resolution passed,change from idle mode to transfer mode *
                              * send the appropriate internal signal dependent of the tbf type      *
                              **********************************************************************/

                              if (global_data_ptr->tbf_type == (uint8)dynamic_alloc_e)
                              {
                                 MSG_GERAN_MED_1_G( "MAC sent MAC_IT_UL_DYNAMIC, contention res UL re-alloc attempts count %d",
                                                    idle_data_ptr->idle_realloc_attempts_count);

                                 memset(&(idle_data_ptr->mac_int_sig.msg.mac_it_ul_dynamic),0,sizeof(mac_it_ul_dynamic_t));

                                 /*load realloc counter*/
                                 idle_data_ptr->mac_int_sig.msg.mac_it_ul_dynamic.idle_realloc_req_count = idle_data_ptr->idle_realloc_attempts_count;

                                 if(idle_data_ptr->i_flags.idle_ul_realloc_state == IDLE_REALLOC_WAITING_TO_CONNECT)
                                 {
                                   /*As we've not rcvd ph connect ind before moving to transfer load        */
                                   /*ul_alloc_cnf which will be sent in transfer on recpetion of connect ind*/
                                   idle_data_ptr->mac_int_sig.msg.mac_it_ul_dynamic.rlc_realloc_data = idle_data_ptr->mr_ul_alloc_cnf_sig;

                                   idle_data_ptr->mac_int_sig.msg.mac_it_ul_dynamic.rlc_realloc_data.sig = MR_UL_ALLOC_CNF;
                                   idle_data_ptr->mac_int_sig.msg.mac_it_ul_dynamic.waiting_to_connect_realloc_flg = TRUE;

                                   /*load and clear the pending realloc req  flag */
                                   idle_data_ptr->mac_int_sig.msg.mac_it_ul_dynamic.pending_realloc_req = idle_data_ptr->i_flags.pending_realloc_req;
                                   idle_data_ptr->i_flags.pending_realloc_req = FALSE;

                                   MSG_GERAN_MED_0_G("GMAC info set idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION");
                                   idle_data_ptr->i_flags.idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION;
                                 }

                                 else if (idle_data_ptr->i_flags.rcvd_unsolicited_pua_waiting_for_connected_ind == TRUE)
                                 {
                                   MSG_GERAN_MED_0_G("tlx PUAN rcvd before connect ind for unsolicited was PUA not rcvd");

                                   /*As we've not rcvd ph connect ind before moving to transfer load        */
                                   /*ul_alloc_cnf which will be sent in transfer on recpetion of connect ind*/
                                   idle_data_ptr->mac_int_sig.msg.mac_it_ul_dynamic.rlc_realloc_data = idle_data_ptr->mr_ul_alloc_cnf_sig;

                                   idle_data_ptr->mac_int_sig.msg.mac_it_ul_dynamic.rlc_realloc_data.sig = MR_UL_ALLOC_CNF;
                                   idle_data_ptr->mac_int_sig.msg.mac_it_ul_dynamic.waiting_to_connect_realloc_flg = TRUE;
                                   idle_data_ptr->mac_int_sig.msg.mac_it_ul_dynamic.pending_realloc_req = FALSE;

                                   /*re set flag. */
                                   idle_data_ptr->i_flags.rcvd_unsolicited_pua_waiting_for_connected_ind = FALSE;

                                 }
                                 else
                                 {
                                   MSG_GERAN_MED_0_G("GMAC info set idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION");
                                   idle_data_ptr->i_flags.idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION;
                                   idle_data_ptr->mac_int_sig.msg.mac_it_ul_dynamic.waiting_to_connect_realloc_flg = FALSE;

                                   /*load and clear the pending realloc req  flag */
                                   idle_data_ptr->mac_int_sig.msg.mac_it_ul_dynamic.pending_realloc_req = idle_data_ptr->i_flags.pending_realloc_req;
                                   idle_data_ptr->i_flags.pending_realloc_req = FALSE;
                                 }

                                 gmac_idle_send_mac_internal_sig(&idle_data_ptr->mac_int_sig,MAC_IT_UL_DYNAMIC,gas_id);

                              }

                              /*GMAC will complete the pcco process activities*/
                              gmac_util_complete_pcco_process(PCCO_SUCCESS,MAC_INITIATED,GMAC_IDLE,__LINE__,gas_id);

                              global_data_ptr->mac_action = NONE;
                              display_mac_action(global_data_ptr->mac_action,gas_id);
                              MSG_GERAN_MED_0_G("GMAC State Idle PASSIVE");
                              idle_data_ptr->mac_idle_substate = PASSIVE;
                              (void)mac_log_state(gas_id);

                              global_data_ptr->mac_mode = TRANSFER_MODE;
                              MSG_GERAN_MED_1_G("MAC set mode %d",global_data_ptr->mac_mode);

                              MSG_GERAN_MED_0_G( "MAC sent MAC_GRR_MAC_STATUS");
                              gmac_util_send_mac_grr_mac_status(&idle_data_ptr->mac_grr_sig,MAC_TRANSFER,gas_id);
                           }
                           else if(idle_data_ptr->contention_result == CON_RES_RETRY)
                           {
                               /*******************************************************************
                               * contention resolution failed, perform access retry if applicable *
                               *******************************************************************/

                               /* if ul tbf fails before reaching transfer, L1 must always be */
                               /* release irrespective if a access retry is to be performed   */
                               idle_data_ptr->release_cause = ACCESS_RETRY;
                               /*GMAC sends block signal to RLC and will be unblocked rlc once gmac received release conf from L1*/
                               gmac_idle_send_no_access_to_rlc(gas_id);

                               if(idle_data_ptr->i_flags.idle_ul_realloc_state != IDLE_REALLOC_NO_ACTION)
                               {
                                 MSG_GERAN_MED_0_G("GMAC info set idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION");
                                 idle_data_ptr->i_flags.idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION;
                                 idle_data_ptr->i_flags.pending_realloc_req = FALSE;
                               }


                               idle_data_ptr->i_flags.rcvd_unsolicited_pua_waiting_for_connected_ind = FALSE;

                               gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,MAC_LOG_UL_FAILED_CONTENTION_RES,MAC_LOG_DL_UNKNOWN_ERROR,
                                                             gas_id );
                           }
                           else if(idle_data_ptr->contention_result == CON_RES_RELEASE)
                           {
                              /************************************************************
                              * release ul tbf, data transfer successful completed within *
                              * the contention resolution process                         *
                              ************************************************************/

                              /*When data transfer is completed with in contenion res STOP*/
                              /* T3180 if running                                         */
                              gmac_stop_t3180(gas_id);

                              if (global_data_ptr->mac_action == PMR_EST || global_data_ptr->mac_action == PCCF_EST)
                              {
                                MSG_GERAN_HIGH_1_G("MAC abnormal allocation for mac_action %d",global_data_ptr->mac_action);
                              }

                              /*rel l1 and wait in state for L1_MAC_TBF_REL_CONFIRM         */
                              idle_data_ptr->release_cause = CONT_RES_COMPLETE;
                              global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
                              /*GMAC completes the pcco process activities*/
                              gmac_util_complete_pcco_process(PCCO_SUCCESS,MAC_INITIATED,GMAC_IDLE,__LINE__,gas_id);

                              if(idle_data_ptr->i_flags.idle_ul_realloc_state != IDLE_REALLOC_NO_ACTION)
                              {
                                MSG_GERAN_MED_0_G("GMAC info set idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION");
                                idle_data_ptr->i_flags.idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION;
                                idle_data_ptr->i_flags.pending_realloc_req = FALSE;
                              }

                              idle_data_ptr->i_flags.rcvd_unsolicited_pua_waiting_for_connected_ind = FALSE;


                              gmac_idle_stop_access_timers(idle_data_ptr->mac_idle_substate,gas_id);

                              (void)gmac_normal_ul_tbf_release_proc( FALSE,FALSE,FALSE,gas_id);
                           }

                         }

                    break;

                    case PACKET_TBF_RELEASE :
                          MSG_GERAN_MED_0_G("MAC Rcvd P_TBF_REL");

                          idle_data_ptr->tbf_rel = gmac_idle_packet_tbf_release(TRUE,gas_id);

                          if( idle_data_ptr->tbf_rel == UL_TBF_ABNORMAL)
                          {
                             MSG_GERAN_MED_2_G( "MAC Rcvd PKT_TBF_REL ,m_s %d,m_a %d",
                                                idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action);

                             /*******************************************************************
                             * contention resolution failed, perform access retry if applicable *
                             *******************************************************************/

                             /* if ul tbf fails before reaching transfer, L1 must always be */
                             /* release irrespective if a access retry is to be performed   */
                             idle_data_ptr->release_cause = ACCESS_RETRY;
                             /*GMAC sends block signal to RLC and will be unblocked rlc once gmac received release conf from L1*/
                             gmac_idle_send_no_access_to_rlc(gas_id);

                             gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,MAC_LOG_UL_TBF_RELEASE,MAC_LOG_DL_UNKNOWN_ERROR,
                                                           gas_id );
                          }
                          else if (idle_data_ptr->tbf_rel == UL_TBF_NORMAL)
                          {
                            MSG_GERAN_MED_0_G( "MAC Rcvd PKT_TBF_REL UL Normal rel");
                          }
                          else
                          {
                            MSG_GERAN_HIGH_0_G("MAC Info P_TBF_REL, Invalid release");
                          }
                    break;

                    case PACKET_PAGING_REQUEST:
                         MSG_GERAN_LOW_0_G("MAC Rcvd PACKET_PAGING_REQUEST");
                         gmac_util_pkt_paging_on_pacch(gas_id);
                    break;

                    case PACKET_DOWNLINK_ASSIGNMENT:
                       /*PDA is only handled in state if releasing UL because contention res was completed  */
                       /*successfully without a need to go to transfer OR it failed to get to transfer failed*/
                      if(global_data_ptr->c_flags.tbf_release_pending_flg == TRUE)
                      {
                       tbf_mode_T  temp_tbf_mode = global_data_ptr->current_tbf_mode; /*store the current TBF mode as it may need to be restored if
                                                                         the PDA is ignored or has errors*/

                       MSG_GERAN_MED_0_G("MAC Rcvd PACKET_DOWNLINK_ASSIGNMENT");
                       /*********************************************************************
                       * FOR any cell selection/reselection activities ignore PDA           *
                       *********************************************************************/

                       if (idle_data_ptr->release_cause == CONT_RES_COMPLETE)
                       {
                         /* Contention has passed and MAC is releasing for PUAN FAI=1, so address check using TFI
                            as it is now valid */
                         idle_data_ptr->pda_data_result = gmac_idle_pda_handler(&idle_data_ptr->dl_ass_ind_sig,FALSE,PDA_CHECK_TFI_AND_TLLI,gas_id);
                       }
                       else
                       {
                         idle_data_ptr->pda_data_result = gmac_idle_pda_handler(&idle_data_ptr->dl_ass_ind_sig,FALSE,PDA_CHECK_TLLI_ONLY,gas_id);
                       }

                       if(idle_data_ptr->pda_data_result == NO_ERRORS)
                       {
                         /*no need to release UL as its already being released*/

                         if(global_data_ptr->mac_action == UL_DATA_EST)
                         {

                           idle_data_ptr->i_flags.pda_ul_pacch = TRUE;

                           rrbp_control_block_handler_2(PCA_DL_MSG,gas_id);

                           /*store PDA in csn1 format*/
                           /*dl_msg is the glaobal msg buffer loaded by rrbp handler when retriving dl msg*/


                           memset(idle_data_ptr->csn1_encoded_pda_msg,0,SIZE_OF_DL_MSG);
                           memscpy(idle_data_ptr->csn1_encoded_pda_msg,
                                   sizeof(idle_data_ptr->csn1_encoded_pda_msg),
                                   global_data_ptr->dl_msg, SIZE_OF_DL_MSG);

                           /*save PDA FN needed when calculating relative stating times*/
                           idle_data_ptr->gmac_store_rcvd_fn_of_pda = global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no;

                           idle_data_ptr->release_cause = PDA_INTERRUPTS_UL_TBF_EST;
                         }
                         else
                         {
                           MSG_GERAN_HIGH_1_G("MAC info PDA ignored reselection %d",global_data_ptr->mac_action);

                           /* restore the TBF mode as the PDA has been ignored*/
                           global_data_ptr->current_tbf_mode = temp_tbf_mode;
                         }
                       }
                       else
                       {
                         if(idle_data_ptr->pda_data_result == FREQ_ERROR_PARTIAL_SYS_ACQ)
                         {
                           /*******************************************************************
                           * note FREQ_ERROR_PARTIAL_SYS_ACQ is the only actionable error case*
                           * for any other error case the PDA is ignored(ref 12.8 and 12.8.1) *
                           * stop access timers and release interrupted ul tbf est            *
                           *******************************************************************/

                           /*wait in state for L1_MAC_TBF_REL_CONFIRM*/
                           idle_data_ptr->release_cause = PARTIAL_SYS_ACQ;
                           /*GMC block RLC and will unblock RLC once GMAC received rel cnf from L1*/
                           gmac_util_block_rlc(NO_DEL,GMAC_IDLE,__LINE__,gas_id);

                           gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,(mac_log_ul_tbf_release_t)FREQ_ERROR_PARTIAL_SYS_ACQ,MAC_LOG_DL_UNKNOWN_ERROR,
                                                         gas_id );
                         }
                         else
                         {
                           /* restore the TBF mode as the PDA has been ignored*/
                           global_data_ptr->current_tbf_mode = temp_tbf_mode;
                         }
                       }
                      }
                    break;

                    default :
                        MSG_GERAN_MED_3_G( "Unexpected DL msg rx. idle state %d task id %d msg id %d",idle_data_ptr->mac_idle_substate,
                                                                                                      task_id, global_data_ptr->channel_msg);
                    break;

                }/*end of message switch*/

             break;

             case L1_MAC_FREQ_ASS_CNF:
                MSG_GERAN_HIGH_0_G("GMAC Info L1_MAC_FREQ_ASS_CNF rxd, Ignored for freq cnf tidy up");

                if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_freq_ass_cnf.band_supported == FALSE)
                {
                  MSG_GERAN_ERROR_0_G("GMAC Freq Cnf Cleanup, L1 indicating band NOT supported");
                }
             break;

             case MAC_PH_CONNECT_IND:
                  /*if there is tbf release already pending ignore this signal*/
                  if(global_data_ptr->c_flags.tbf_release_pending_flg == FALSE)
                  {
                    /* note sb_alloc is not applicable for 2 phase */

                    if(global_data_ptr->tbf_type == (uint8)dynamic_alloc_e)
                    {
                      /*PUA can only be rcvd in contention res once the 1st ul data block */
                      /*has been sent hence t3180 should be started on connection of this */
                      /*assignment (not t3164) ref 7.1.2.3a                               */
                      MSG_GERAN_MED_0_G("T3180 started");
                      gmac_start_t3180(gas_id);
                    }
                    (void)mac_log_state(gas_id);

                    if(idle_data_ptr->i_flags.idle_ul_realloc_state == IDLE_REALLOC_WAITING_TO_CONNECT)
                    {
                      MSG_GERAN_MED_0_G( "MAC sent MR_UL_ALLOC_CNF");

                      /*resource reallocation assignment, rcvd during 1p contention res, has         */
                      /* connected . Update current TFI and TS to those rcvd in the reassignment PUA */
                      global_data_ptr->gmac_local_tfi_store.ul_tfi = global_data_ptr->gmac_re_assign_ul_tfi;
                      global_data_ptr->gmac_local_ul_ts_alloc = global_data_ptr->gmac_local_ul_realloc_ts;

                      /*Send RLC the new assignment */
                      gmac_idle_send_mr_ul_alloc_cnf(idle_data_ptr->mac_rlc_sig_ptr,&idle_data_ptr->mr_ul_alloc_cnf_sig,gas_id);
                      idle_data_ptr->i_flags.idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION;
                      MSG_GERAN_MED_0_G("GMAC info set idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION");


                      if(idle_data_ptr->i_flags.pending_realloc_req == TRUE)
                      {
                        /*Completed current ul realloc request so now start processing */
                        /*any pending ul realloc requests                              */
                        idle_data_ptr->i_flags.pending_realloc_req = FALSE;

                        idle_data_ptr->idle_realloc_attempts_count = 0;

                        MSG_GERAN_MED_0_G("GMAC info set idle_ul_realloc_state = IDLE_REALLOC_REQUESTED");
                        idle_data_ptr->i_flags.idle_ul_realloc_state = IDLE_REALLOC_REQUESTED;
                        global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_QUAL;

                        /* send MAC_L1_QUALITY_RPT with parameters */
                        MSG_GERAN_MED_0_G("MAC Sent MAC_L1_QUALITY_RPT");
                        idle_data_ptr->mac_l1_sig.mac_l1_quality_rpt.i_level_ind = TRUE;
                        idle_data_ptr->mac_l1_sig.mac_l1_quality_rpt.rrbp_value = MAC_L1_RRBP_VALUE_NOT_VALID;
                        idle_data_ptr->mac_l1_sig.mac_l1_quality_rpt.message_header.message_set = MS_MAC_L1;
                        idle_data_ptr->mac_l1_sig.mac_l1_quality_rpt.message_header.message_id = (uint8)MAC_L1_QUALITY_RPT;
                        gprs_l1_mac_acc_func(&idle_data_ptr->mac_l1_sig,gas_id);

                      }

                    }
                    else if(idle_data_ptr->i_flags.idle_ul_realloc_state == IDLE_REALLOC_REQUESTED)
                    {
                      idle_data_ptr->i_flags.idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION;
                      MSG_GERAN_MED_0_G("GMAC info set idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION");
                    }
                    else
                    {

                      /*NOTE: for unsoliciated PUA then idle_ul_realloc_state  */
                      /*      should only be in IDLE_REALLOC_NO_ACTION         */
                      if (idle_data_ptr->i_flags.rcvd_unsolicited_pua_waiting_for_connected_ind == TRUE)
                      {

                        MSG_GERAN_MED_0_G( "MAC sent MR_UL_ALLOC_CNF");

                      MSG_GERAN_MED_0_G("tlx connect ind for unsolicited PUA not rcvd before PUAN that completes contention res");

                        /*resource reallocation assignment, rcvd during 1p contention res, has         */
                        /*connected . Update current TFI and TS to those rcvd in the reassignment PUA */
                        global_data_ptr->gmac_local_tfi_store.ul_tfi = global_data_ptr->gmac_re_assign_ul_tfi;
                        global_data_ptr->gmac_local_ul_ts_alloc = global_data_ptr->gmac_local_ul_realloc_ts;

                        /*Send RLC the new assignment */
                        gmac_idle_send_mr_ul_alloc_cnf(idle_data_ptr->mac_rlc_sig_ptr,&idle_data_ptr->mr_ul_alloc_cnf_sig,gas_id);
                        idle_data_ptr->i_flags.rcvd_unsolicited_pua_waiting_for_connected_ind = FALSE;

                      }
                    }

                  }
                  else
                  {
                    MSG_GERAN_MED_0_G("GMAC releasing L1, so ignoring MAC_PH_CONNECT_IND signal");
                  }
             break;

             /* l1 to Mac signals*/
             case L1_MAC_USF_DETECTED:
                  /*if there is tbf release already pending ignore this signal*/
                  if(global_data_ptr->c_flags.tbf_release_pending_flg == FALSE)
                  {
                    gmac_start_t3180(gas_id);
                  }
                  else
                  {
                    MSG_GERAN_MED_0_G("GMAC releasing L1, so ignoring L1_MAC_USF_DETECTED signal");
                  }
             break;

             case L1_MAC_1ST_DATA_BLOCK_SENT:
                  /*if there is tbf release already pending ignore this signal*/
                  if(global_data_ptr->c_flags.tbf_release_pending_flg == FALSE)
                  {
                    MSG_GERAN_MED_2_G( "MAC Rcvd L1_MAC_1ST_DATA_BLOCK_SENT, mac state =%d mac_act %d",
                                       idle_data_ptr->mac_idle_substate ,global_data_ptr->mac_action);
                    /*******************************************************************************
                    * L1_MAC_1ST_DATA_BLOCK_SENT can only be received after ph_connect_ind and     *
                    * indicates that the first rlc data block has been sent for a 1phase ul est    *
                    ********************************************************************************/
                    if((global_data_ptr->tbf_type == (uint8)fixed_alloc ) ||  (global_data_ptr->tbf_type == (uint8)dynamic_alloc_e))
                    {
                        /*note t3166 is started by L1 ONLY for 1 phase access*/
                        if (global_data_ptr->tbf_type == (uint8)dynamic_alloc_e)
                        {
                           gmac_stop_t3164(gas_id);
                           /*complete pccf process activities*/
                           gmac_util_complete_pccf_process(SUCCESSFULLY_SENT_PCCF,MAC_INITIATED,GMAC_IDLE,__LINE__,gas_id);
                        }
                    }
                    else
                    {
                      MSG_GERAN_HIGH_1_G("MAC error TBF type not valid for 1 phase, tbf_type =%d",global_data_ptr->tbf_type);
                    }
                  }
                  else
                  {
                    MSG_GERAN_MED_0_G("GMAC releasing L1, so ignoring L1_MAC_1ST_DATA_BLOCK_SENT signal");
                  }
             break;

             case L1_MAC_QUAL_MEASURMENTS:
                 /*if there is tbf release already pending ignore this signal*/
                 if(global_data_ptr->c_flags.tbf_release_pending_flg == FALSE)
                 {
                   /**************************************************************
                   *  call re-est as triggered by TBF_EST bit in PUAN being set  *
                   *  will not be support in con res state                       *
                   ***************************************************************/

                   global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_USF;

                   /*For re-alloc procedure in contention res PRR is formatted  */
                   /*without RAC IE irrespective of EGPRS or GPRS               */
                   gmac_send_prr(NO_RE_EST,TRUE,gas_id);

                 }
                 else
                 {
                   MSG_GERAN_MED_0_G("GMAC releasing L1, so ignoring L1_MAC_QUAL_MEASURMENTS signal");
                 }
             break;

             case L1_MAC_SINGLE_BLOCK_SENT:
                  MSG_GERAN_MED_1_G("L1_MAC_SINGLE_BLOCK_SENT rcvd. ul re-alloc on UL PACCH gmac_prr_on_pacch_state %d",
                                    global_data_ptr->c_flags.gmac_prr_on_pacch_state);
                  if (global_data_ptr->c_flags.gmac_prr_on_pacch_state == GMAC_PRR_ON_PACCH_AWAIT_USF)
                  {
                    idle_data_ptr->idle_realloc_attempts_count++;
                    rr_read_public_store(RRPS_CELL_OPTIONS_T3168,&idle_data_ptr->t3168_timeout,gas_id);
                    gmac_start_t3168(idle_data_ptr->t3168_timeout,gas_id);
                    global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_REQ;
                  }
             break;

             case L1_MAC_TBF_REL_CONFIRM :
                  global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;

                  gmac_stop_t3180(gas_id);
                  gmac_stop_t3166(gas_id);

                  gmac_idle_ul_release_confirm_handler(gas_id);

                  idle_data_ptr->release_cause = NO_RELEASE_CAUSE;
                  (void)mac_log_state(gas_id);

             break;

             default:
               MSG_GERAN_ERROR_3_G( "Unexpected L1 sig rx. idle state %d task id %d sig id 0x%x",
                                    idle_data_ptr->mac_idle_substate,task_id, signal_id);
               break;
           } /*end signal_id switch*/

         }
         /**********************************************************
         *             signals received from GRR                   *
         **********************************************************/
         else if(task_id == MS_MAC_RR)
         {
           display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,gas_id);

           switch (signal_id)
           {
               case GRR_MAC_NO_PS_ACCESS_IND:
                    /*GMAC rcvd NO PS ACCESS IND from GRR. GMAC will abort the access procedure
                      and block RLC. GMAC will move to NULL INACTIVE state once it receives rel
                      cnf from layer1*/
                    MSG_GERAN_MED_1_G("GMAC Rcvd GRR_MAC_NO_PS_ACCESS_IND pdu deletion type %d",
                                      global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);
                    gmac_idle_start_no_ps_access_process(idle_data_ptr->mac_idle_substate,&idle_data_ptr->mac_l1_sig,global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                                         gas_id);
                    global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
               break;

               case GRR_MAC_SEND_PCCF:
                    MSG_GERAN_MED_0_G("GMAC received GRR_MAC_SEND_PCCF from GRR");
                    /*build up PCCF msg and send it directly layer1 for transmission*/
                    if (gmac_util_encode_ul_control_msg_handler(PCCF,FALSE,gas_id) == PASS)
                    {
                      gmac_util_send_ul_control_msg_handler(PCCF, UL_PRESENT, GRR_CNF_NEEDED,gas_id);
                      MSG_GERAN_MED_0_G("GMAC PCCF encoding is successful and no need to wait gmac sends PCCF to layer1 directly");
                    }
                    else
                    {
                      MSG_GERAN_MED_0_G("CSN.1 encode failure of PCCF, wait in state till t3176 exp");
                    }
               break;
#ifdef FEATURE_GSM_EDTM
               case GRR_MAC_EDTM_PKT_CS_EST_REQ:

                 /* 44.060 7.1.2.3 Contention resolution starts when then first data block is sent, at which
                    point T3166 is started*/
                 if( rex_get_timer(&mac_timers_ptr->timer_t3166) != 0 )
                 {
                   MSG_GERAN_MED_1_G("GMAC Accepted GRR_MAC_EDTM_PKT_CS_EST_REQ rr_est_cause: 0x%x in 1 phase Cont Res (T3166 running)",
                                      global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause);

                   gmac_start_t3196(gas_id);
                   global_data_ptr->gmac_local_edtm_pcr_params.edtm_pcr_cause =
                     global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_edtm_pkt_cs_est_req.rr_est_cause;
                 }
                 else
                 {
                   MSG_GERAN_MED_0_G("GMAC Rejected GRR_MAC_EDTM_PKT_CS_EST_REQ in 1 phase Cont Res, T3166 NOT running");
                   gmac_send_mac_grr_edtm_pkt_cs_est_rej(EDTM_CS_REJ_CANNOT_SERVICE_REQUEST,gas_id);
                 }
               break;
#endif /*FEATURE_GSM_EDTM*/

               default:
                    MSG_GERAN_ERROR_3_G( "Unexpected GRR sig rx. idle state %d task id %d sig id 0x%x",
                                         idle_data_ptr->mac_idle_substate,task_id, signal_id);
               break;

           } /*end of signal switch*/

         }
         /**********************************************************
         *             signals received from RLC                   *
         **********************************************************/
         else if(task_id == MS_MAC_RLC)
         {
           display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,gas_id);

           switch (signal_id)
           {
               case RM_UL_N3104_EXPIRY_IND :
                 if(global_data_ptr->c_flags.tbf_release_pending_flg == FALSE)
                 {
                    /**************************************************************
                    * indicates N3104 has exceeded max value abnormal  release    *
                    * ref 4.60, chapter 7.1.4. Perform access retry if applicable *
                    ***************************************************************/

                    /* if ul tbf fails before reaching transfer, L1 must always be */
                    /* release irrespective if a access retry is to be performed   */

                    idle_data_ptr->release_cause = ACCESS_RETRY;
                    /*GMAC sends block signal to RLC and will be unblocked rlc once gmac received release conf from L1*/
                    gmac_idle_send_no_access_to_rlc(gas_id);

                    gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,MAC_LOG_UL_EXPIRED_N3104,MAC_LOG_DL_UNKNOWN_ERROR,gas_id );
                 }
               break;

               case RM_UL_EST_TBF_REQ :
                    if (global_data_ptr->mac_action == UL_DATA_EST)
                    {
                      MSG_GERAN_ERROR_1_G( "MAC Rcvd RM_UL_EST_TBF_REQ and mac action is %d",global_data_ptr->mac_action);
                      MSG_GERAN_ERROR_0_G( "Unexpected RM_UL_EST_TBF_REQ rcvd from RLC");
                    }
                    else
                    {
                      if(gmac_util_is_rlc_blocked(GMAC_IDLE,__LINE__,gas_id)== UNBLOCKED)
                      {
                        MSG_GERAN_ERROR_1_G( "GMAC should have already blocked rlc at this point mac action is %d",global_data_ptr->mac_action);
                      }
                    }
               break;

               case RM_UL_REALLOC_REQ:

                    MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_REALLOC_REQ");

                    if(idle_data_ptr->i_flags.idle_ul_realloc_state == IDLE_REALLOC_WAITING_TO_CONNECT)
                    {
                      /* Currently processing an UL realloc request: buffer subsequent        */
                      /* request to be sent after we have connected the 1st re-alloc request  */
                      /* this could be in either idle or transfer depedning on starting times */

                      MSG_GERAN_MED_1_G("GMAC info pending subsequent UL realloc req. idle_ul_realloc_state =%d",
                                        idle_data_ptr->i_flags.idle_ul_realloc_state);

                      global_data_ptr->gmac_local_stored_ul_chan_req =
                      global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.chan_req_desc;

                      idle_data_ptr->i_flags.pending_realloc_req = TRUE;
                    }
                    else
                    {

                      idle_data_ptr->idle_realloc_attempts_count = 0;

                      global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_QUAL;

                      MSG_GERAN_MED_0_G("GMAC info set idle_ul_realloc_state = IDLE_REALLOC_REQUESTED");
                      idle_data_ptr->i_flags.idle_ul_realloc_state = IDLE_REALLOC_REQUESTED;

                      if ( global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.pfi_present )
                      {
                        global_data_ptr->pfi_present = TRUE;
                        global_data_ptr->pfi = global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.pfi;
                      }
                      else
                      {
                        global_data_ptr->pfi_present = FALSE;
                      }

                      /* store RLC parameters and request quality report for PRR */
                      global_data_ptr->gmac_local_stored_ul_chan_req =
                      global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_realloc_req.chan_req_desc;

                      /* send MAC_L1_QUALITY_RPT with parameters */
                      MSG_GERAN_MED_0_G("MAC Sent MAC_L1_QUALITY_RPT");
                      idle_data_ptr->mac_l1_sig.mac_l1_quality_rpt.i_level_ind = TRUE;
                      idle_data_ptr->mac_l1_sig.mac_l1_quality_rpt.rrbp_value = MAC_L1_RRBP_VALUE_NOT_VALID;
                      idle_data_ptr->mac_l1_sig.mac_l1_quality_rpt.message_header.message_set = MS_MAC_L1;
                      idle_data_ptr->mac_l1_sig.mac_l1_quality_rpt.message_header.message_id = (uint8)MAC_L1_QUALITY_RPT;
                      gprs_l1_mac_acc_func(&idle_data_ptr->mac_l1_sig,gas_id);
                    }

               break;

               case RM_UL_RELEASE_IND:
                    if(global_data_ptr->msg_ptr->rlc_mac_sig.msg.ul_release_ind.cause == REL_UL_ABNORMAL_WITH_ACCESS_RETRY)
                    {
                      gmac_idle_stop_access_timers(idle_data_ptr->mac_idle_substate,gas_id);
                      /*GMAC can receive this signal from GRLC only when GRLC receives
                        suspend request from LLC in tranfer state*/
                      MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_RELEASE_IND from GRLC. so GMAC doing abnormal release of UL TBF");
                      /*GMC block RLC and will unblock RLC once GMAC received rel cnf from L1*/
                      gmac_util_block_rlc(NO_DEL,GMAC_IDLE,__LINE__,gas_id);

                      if(idle_data_ptr->release_cause == GRR_INITIATED_NO_ACCESS )
                      {
                        MSG_GERAN_MED_1_G("MAC UL est already being released abnormally rel code =%d. No action required",idle_data_ptr->release_cause);
                      }
                      else
                      {
                        idle_data_ptr->release_cause = UL_EXCEPTION_HANDLING;
                      }

                      gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,MAC_LOG_UL_UNKNOWN_ERROR,MAC_LOG_DL_UNKNOWN_ERROR,gas_id );
                    }
                    else
                    {
                      MSG_GERAN_MED_0_G("MAC Rcvd RM_UL_RELEASE_IND with wrong release cause");
                    }
               break;

               default:
                    MSG_GERAN_ERROR_3_G( "Unexpected RLC sig rx. idle state %d task id %d sig id 0x%x",
                                         idle_data_ptr->mac_idle_substate,task_id, signal_id);
               break;
           }

         }
         /**********************************************************
         *                 MAC internal signals                    *
         **********************************************************/
         else if(task_id == MS_MAC_MAC)
         {
           display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,gas_id);

           switch (signal_id)
           {
               case MAC_TIMER_T3164:
               case MAC_TIMER_T3166:
               case MAC_TIMER_T3180:
                     /**********************************************************************
                     * contention resolution failed ref 4.60, chapter 7.1.4 Load and send  *
                     * mr_ul_contention_ind. Rlc resets n3104,stops sending data blocks    *
                     **********************************************************************/

                     if(signal_id == (uint8)MAC_TIMER_T3166 )
                     {
                       MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3166");
                     }
                     else if(signal_id == (uint8)MAC_TIMER_T3180)
                     {
                       MSG_GERAN_MED_0_G("MAC Rcvd MAC_TIMER_T3180");
                     }
                     else if(signal_id == (uint8)MAC_TIMER_T3164)
                     {
                       MSG_GERAN_MED_1_G( "MAC info timer t3164 expired ",0 );
                     }

                     if(idle_data_ptr->release_cause == GRR_INITIATED_NO_ACCESS )
                     {
                       MSG_GERAN_MED_1_G("MAC UL est already being released abnormally rel code =%d. No action required",idle_data_ptr->release_cause);
                     }
                     else
                     {
                       MSG_GERAN_MED_0_G( "MAC wont send MR UL CONTENTION IND to RLC. Instead of this gmac sends no access ind to RLC");
                        /*ref 44.060 chap 7.1.4 when t3164 exp also perform 4 retries*/
                       idle_data_ptr->release_cause = ACCESS_RETRY;
                       /*GMAC sends block signal to RLC and will be unblocked rlc once gmac received release conf from L1*/
                       gmac_idle_send_no_access_to_rlc(gas_id);
                       gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,MAC_LOG_UL_FAILED_CONTENTION_RES,MAC_LOG_DL_UNKNOWN_ERROR,
                                                     gas_id );
                       gmac_send_mac_grr_abnormal_release(RR_CONTENTION_FAILED, gas_id);
                     }

               break;

               case MAC_TIMER_T3168:
                    MSG_GERAN_MED_1_G("MAC Rcvd MAC_TIMER_T3168 durng contention res, realloc attempts count = %d",idle_data_ptr->idle_realloc_attempts_count);
                    /* NOTE idle_realloc_attempts_count is incremented when PRR has been sent  *
                    *  ie when mac receives L1_MAC_SINGLE_BLOCK_SENT                    */

                    if (idle_data_ptr->idle_realloc_attempts_count < MAX_RETRY_ATTEMPTS)
                    {
                      /* request quality reports again for PRR */
                      global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_QUAL;

                      /* send MAC_L1_QUALITY_RPT with parameters */
                      MSG_GERAN_MED_0_G("MAC Sent MAC_L1_QUALITY_RPT");
                      idle_data_ptr->mac_l1_sig.mac_l1_quality_rpt.i_level_ind = TRUE;
                      idle_data_ptr->mac_l1_sig.mac_l1_quality_rpt.rrbp_value = MAC_L1_RRBP_VALUE_NOT_VALID;
                      idle_data_ptr->mac_l1_sig.mac_l1_quality_rpt.message_header.message_set = MS_MAC_L1;
                      idle_data_ptr->mac_l1_sig.mac_l1_quality_rpt.message_header.message_id = (uint8)MAC_L1_QUALITY_RPT;
                      gprs_l1_mac_acc_func(&idle_data_ptr->mac_l1_sig,gas_id);

                    }
                    else
                    {
                      /* Failed to re-allocate UL TBF during contention resolution  *
                      *  phase after max retries Abort UL, return to idle          */

                      idle_data_ptr->release_cause = CONT_RES_UL_RE_ALLOC_FAILURE;

                      if(global_data_ptr->mac_action == UL_DATA_EST)
                      {
                       /*GMC block RLC and will unblock RLC once GMAC connects dl tbf*/
                       gmac_util_block_rlc(DEL_ONE,GMAC_IDLE,__LINE__,gas_id);
                      }

                      gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,MAC_LOG_UL_FAILED_CONTENTION_RES,MAC_LOG_DL_UNKNOWN_ERROR,
                                                    gas_id );
                      mac_log_ul_tbf_release(MAC_LOG_UL_EXPIRED_T3168,gas_id);
                    }
               break;

               case MAC_TIMER_T3200_1:
               case MAC_TIMER_T3200_2:
               case MAC_TIMER_T3200_3:
               case MAC_TIMER_T3200_4:
               case MAC_TIMER_T3200_5:
               case MAC_TIMER_T3200_6:
               case MAC_TIMER_T3200_7:
               case MAC_TIMER_T3200_8:
                    MSG_GERAN_MED_1_G( "MAC info timer t3200 expired si_id %d",signal_id );
                    t3200_expiry_handler((mac_mac_sig_id_t)signal_id,gas_id );
               break;

#ifdef FEATURE_GSM_EDTM
               case MAC_TIMER_T3196:
                 MSG_GERAN_MED_0_G("GMAC EDTM T3196 Timer expired");
                 gmac_send_mac_grr_edtm_pkt_cs_est_rej(EDTM_CS_REJ_T3196_EXPIRY,gas_id);
               break;
#endif /*FEATURE_GSM_EDTM*/

               default:
                    MSG_GERAN_ERROR_3_G( "Unexpected MAC sig rx. idle state %d task id %d sig id 0x%x",
                                         idle_data_ptr->mac_idle_substate,task_id, signal_id);
               break;
           }

         }
         else
         {
         /*error*/
         }

      break;

      /*****************************************************************
      *
      *  DOWNLINK ASSIGNMENT STATE
      *
      ******************************************************************/
      case DL_ASS:

         if(task_id == MS_MAC_L1)
         {
           display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,gas_id);

           switch (signal_id)
           {
             case MAC_PH_DATA_IND :

                display_dl_msg_rcvd(global_data_ptr->channel_msg,__LINE__,idle_data_ptr->mac_idle_substate, global_data_ptr->mac_action,gas_id);

                switch (global_data_ptr->channel_msg)
                {
                    case PACKET_CELL_CHANGE_ORDER :
                         if(gmac_idle_check_pcco_addressing(gas_id))
                         {
                           /***********************************************************
                           * when PCCO addresses MS stop relevent rlc/mac timers ref  *
                           * 4.60, 7.4.1 release l1, wait for L1_MAC_TBF_REL_CONFIRM  *
                           * and start pcco cell reselection process                  *
                           ************************************************************/
                           MSG_GERAN_LOW_0_G("MAC Info forwards PCCO to GRR");
                           rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);
                           gmac_send_data_ind_to_grr(gas_id);
                         }
                         else
                         {
                           MSG_GERAN_LOW_0_G("MAC Info PCCO not for MS");
                         }
                    break;

                    case PACKET_DOWNLINK_ASSIGNMENT :
                         /**************************************************************
                         * PDA pda will be addressed via its tlli any pervious PDA     *
                         *  data is ignored (4.60, 7.2.1.1)                            *
                         ***************************************************************/
                         idle_data_ptr->pda_data_result = gmac_idle_pda_handler(&idle_data_ptr->dl_ass_ind_sig,TRUE,PDA_CHECK_TLLI_ONLY,gas_id);
                         /**************************************************************
                         * if no errors remain in dl_ass otherwise return to passive,  *
                         * inform gmm of state change and release l1                   *
                         **************************************************************/
                         if( idle_data_ptr->pda_data_result != NO_ERRORS)
                         {
                           /*if address error don't release DL just ignore PDA*/
                           if(idle_data_ptr->pda_data_result != ASSIGNMENT_ADDRESSING_ERROR)
                           {
                             /* if ul tbf fails before reaching transfer, L1 must always be */
                             /* release irrespective if a access retry is to be performed   */
                             if(idle_data_ptr->pda_data_result == FREQ_ERROR_PARTIAL_SYS_ACQ)
                             {
                               idle_data_ptr->release_cause = PARTIAL_SYS_ACQ;
                             }
                             else
                             {
                               idle_data_ptr->release_cause = REL_DL_TBF;
                             }

                             MSG_GERAN_MED_2_G("MAC info PDA abnormal rel, pda_result =%d cause =%d ",
                                               idle_data_ptr->pda_data_result,idle_data_ptr->release_cause);
                             gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, DL_TBF_ABNORMAL,MAC_LOG_UL_UNKNOWN_ERROR ,(mac_log_dl_tbf_release_t)idle_data_ptr->pda_data_result,
                                                           gas_id);
                           }
                           else
                           {
                              MSG_GERAN_MED_1_G("PDA addressing error ignore pda_data_result = %d",idle_data_ptr->pda_data_result);
                           }
                         }
                         else
                         {

                            /*If no TA value in PDA and S/P bit set to 1 and camped on a 51 cell then */
                            /*IA for DL was rcvd. so check if the IA had TA info if so for the purpose*/
                            /*of sending RRBP need to indicate that TA info is present.               */
                            if(global_data_ptr->gmac_null_store_l1_dl_config.ta_params.ta_value_ie.present == TRUE)
                            {
                              ((packet_downlink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->pkt_timing_adv.valid_options = timing_value_valid;
                            }

                            MSG_GERAN_MED_2_G( "MAC Rcvd PDA ,m_s %d,m_a %d",idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action);
                            rrbp_control_block_handler_2(PCA_DL_MSG,gas_id);
                            MSG_GERAN_MED_0_G( "MAC sent MAC_L1_DL_TBF_CONFIG");
                            idle_data_ptr->mac_l1_sig.mac_l1_dl_tbf_config.message_header.message_set = MS_MAC_L1;
                            idle_data_ptr->mac_l1_sig.mac_l1_dl_tbf_config.message_header.message_id = (uint8)MAC_L1_DL_TBF_CONFIG;
                            gprs_l1_mac_acc_func(&idle_data_ptr->mac_l1_sig,gas_id);
                         }
                    break;

                    case PACKET_POLLING_REQUEST :
                         /***********************************************************
                         *    Check PPR address correctly otherwise msg ignored     *
                         ***********************************************************/
                         gmac_idle_pkt_polling_request_handler(gas_id);
                    break;

                    case PACKET_DOWNLINK_DUMMY_CNTRL_BLK :
                         /*handled in gmactask*/
                    break;

                    case PACKET_PAGING_REQUEST:
                         MSG_GERAN_LOW_0_G("MAC Rcvd PACKET_PAGING_REQUEST");
                         gmac_util_pkt_paging_on_pacch(gas_id);
                    break;

                    case PACKET_MEASUREMENT_ORDER:
                         gmac_util_pmo_handler(gas_id);
                    break;

                    default :
                        MSG_GERAN_MED_3_G( "Unexpected DL msg rx. idle state %d task id %d msg id %d",
                                           idle_data_ptr->mac_idle_substate,task_id, global_data_ptr->channel_msg);
                    break;
                }


             break; /*end of dl msg signal reception for dl_ass state*/

             /*            other l1 to mac signal reception                */

             case L1_MAC_FREQ_ASS_CNF:
                  MSG_GERAN_MED_0_G("MAC info Rcvd Freq cnf no action");

                  if(global_data_ptr->msg_ptr->l1_mac_sig.msg.tbf_freq_ass_cnf.band_supported == FALSE)
                  {
                    MSG_GERAN_ERROR_0_G("GMAC Freq Cnf Cleanup, L1 indicating band NOT supported");
                  }
             break;

             case MAC_PH_CONNECT_IND:
                  /******************************************************************
                  * note :- the signal data for MR_DL_ASS_IND was preloaded when    *
                  * PDA was received in passive,ul_packet_access or dl_ass. (t3190  *
                  * is RLC timer and started see ref 4.60 , 7.2.1.1                 *
                  ******************************************************************/
                   if(global_data_ptr->c_flags.tbf_release_pending_flg == FALSE)
                   {
                     {
                       /* copy NULL stored RLC DL ASS data to IDLE message structure */
                       idle_data_ptr->mac_rlc_sig_ptr = grlc_get_cmd_buf_mac();
                       if (idle_data_ptr->mac_rlc_sig_ptr != NULL)
                       {
                         MSG_GERAN_MED_0_G("MAC Sent MR_DL_ASS_IND");
                         /*copy stored MR_UL_ALLOC_CNF data into signal*/
                         idle_data_ptr->mac_rlc_sig_ptr->msg.dl_ass_ind= idle_data_ptr->dl_ass_ind_sig;

                         idle_data_ptr->mac_rlc_sig_ptr->msg.dl_ass_ind.num_dl_ts = gmac_utils_calculate_nts(global_data_ptr->gmac_local_dl_ts_alloc,
                                                                                                             gas_id);

                         idle_data_ptr->mac_rlc_sig_ptr->msg.dl_ass_ind.sig = MR_DL_ASS_IND;
                         gmac_rlc_mac_insert_gas_id(idle_data_ptr->mac_rlc_sig_ptr,gas_id);
                         grlc_put_cmd_mac(gas_id,idle_data_ptr->mac_rlc_sig_ptr);
                       }
                       else
                       {
                         ERR_GERAN_FATAL_0_G("MAC ERROR failed to allocated message memory");
                       }
                       MSG_GERAN_MED_0_G("GMAC State Idle PASSIVE");
                       idle_data_ptr->mac_idle_substate = PASSIVE;
                       (void)mac_log_state(gas_id);

                       global_data_ptr->mac_mode = TRANSFER_MODE;
                       MSG_GERAN_MED_0_G("MAC mode TRANSFER");

                       MSG_GERAN_MED_0_G( "MAC sent MAC_GRR_MAC_STATUS");
                       gmac_util_send_mac_grr_mac_status(&idle_data_ptr->mac_grr_sig,MAC_TRANSFER,
                                                         gas_id);
                       /*GMAC tranfer mode will unblock rlc once tranfer mode process
                         MAC_IT_DL_DYNAMIC signal*/
                       /* depending on the mac mode received in PDA */
                       MSG_GERAN_MED_0_G( "MAC sent MAC_IT_DL_DYNAMIC");
                       gmac_idle_send_mac_internal_sig(&idle_data_ptr->mac_int_sig,MAC_IT_DL_DYNAMIC,
                                                       gas_id);

                       memset(&global_data_ptr->gmac_null_store_l1_dl_config,0,sizeof(mac_l1_dl_tbf_config_t));
                     }
                   }
                   else
                   {
                     MSG_GERAN_MED_0_G("GMAC releasing L1, so ignoring MAC_PH_CONNECT_IND signal");
                   }

             break;

             case L1_MAC_TBF_REL_CONFIRM :

                  global_data_ptr->c_flags.tbf_release_pending_flg = FALSE;

                  reset_tbf_parameters(gas_id);

                  gmac_reset_rac_params(gas_id);

                  global_data_ptr->gmac_local_tfi_store.dl_tfi_valid =  FALSE;

                  memset(&global_data_ptr->gmac_null_store_l1_dl_config,0,sizeof(mac_l1_dl_tbf_config_t));

                  gmac_idle_dl_release_confirm_handler(gas_id);

                  idle_data_ptr->release_cause = NO_RELEASE_CAUSE;
                  (void)mac_log_state(gas_id);

             break;

             default:
                  MSG_GERAN_ERROR_3_G( "Unexpected L1 sig rx. idle state %d task id %d sig id 0x%x",
                                       idle_data_ptr->mac_idle_substate,task_id, signal_id);
             break;
           }

         }
         /**********************************************************
         *             signals received from GRR                   *
         **********************************************************/
         else if(task_id == MS_MAC_RR)
         {
           display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,
                               gas_id);

           switch (signal_id)
           {
             case GRR_MAC_RACH_REQ:
               /* Required for CCCH procedures.
               ** Prevent GRR performing RACH for UL whilst performing DL TBF EST
               */
               MSG_GERAN_MED_0_G("MAC Rcvd GRR_MAC_RACH_REQ");
               gmac_util_send_grr_mac_rach_rej(gas_id);
               break;

               case GRR_MAC_NO_PS_ACCESS_IND:
                    /*GMAC rcvd NO PS ACCESS IND from GRR. GMAC will abort the dl tbf.
                      GMAC will move to NULL INACTIVE state once it receives rel
                      cnf from layer1*/
                    MSG_GERAN_MED_1_G("GMAC Rcvd GRR_MAC_NO_PS_ACCESS_IND pdu deletion type %d",
                                      global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu);
                    gmac_idle_start_no_ps_access_process(idle_data_ptr->mac_idle_substate,&idle_data_ptr->mac_l1_sig,
                                                         global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_no_ps_access_ind.delete_pdu,
                                                         gas_id);
                    global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;
               break;

               case GRR_MAC_SEND_PCCF:
                    MSG_GERAN_MED_0_G("GMAC received GRR_MAC_SEND_PCCF from GRR");
                    /*build up PCCF msg and store it in csn_encoder_ul_msg for use later*/
                    /*when the DL is connected and gmac sends pccf when it polled in data block*/
                    if (gmac_util_encode_ul_control_msg_handler(PCCF,FALSE,gas_id) == PASS)
                    {
                      gmac_util_send_ul_control_msg_handler(PCCF, DL_ONLY_PRESENT, GRR_CNF_NEEDED, gas_id);
                      MSG_GERAN_MED_0_G("GMAC PCCF encoding is successful and GMAC gives pccf sets pccf sending to true");
                    }
                    else
                    {
                      MSG_GERAN_MED_0_G("CSN.1 encode failure of PCCF, wait in state till t3176 exp");
                    }
               break;

               case GRR_MAC_SEND_MEAS_RPT:
               break;

               case GRR_MAC_DL_TBF_EST:
                    MSG_GERAN_ERROR_0_G("MAC info Can't get this GRR_MAC_DL_TBF_EST in UL_1P_ASS");
               break;

#ifdef FEATURE_GSM_EDTM
               case GRR_MAC_EDTM_PKT_CS_EST_REQ:
                 MSG_GERAN_MED_0_G("GMAC Rejected GRR_MAC_EDTM_PKT_CS_EST_REQ, Not in Contention Resolution");
                 gmac_send_mac_grr_edtm_pkt_cs_est_rej(EDTM_CS_REJ_CANNOT_SERVICE_REQUEST,gas_id);
               break;
#endif /*FEATURE_GSM_EDTM*/

               default:
                    MSG_GERAN_ERROR_3_G( "Unexpected GRR sig rx. idle state %d task id %d sig id 0x%x",
                                         idle_data_ptr->mac_idle_substate,task_id, signal_id);
               break;

           } /*end of signal switch*/
         }
         /**********************************************************
         *             signals received from RLC                   *
         **********************************************************/
         else if(task_id == MS_MAC_RLC)
         {
           display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,gas_id);

           switch (signal_id)
           {
               case RM_UL_EST_TBF_REQ :
                    if(gmac_util_is_rlc_blocked(GMAC_IDLE,__LINE__,gas_id)== UNBLOCKED)
                    {
                        MSG_GERAN_ERROR_0_G( "GMAC should have already blocked rlc at this point ");
                    }
               break;

               default:
                    MSG_GERAN_ERROR_3_G( "Unexpected RLC sig rx. idle state %d task id %d sig id 0x%x",
                                         idle_data_ptr->mac_idle_substate,task_id, signal_id);
               break;
           }
         }
         /**********************************************************
         *                 MAC internal signals                    *
         **********************************************************/
         else if(task_id == MS_MAC_MAC)
         {
           display_signal_rcvd(signal_id,task_id,__LINE__,idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action,gas_id);

           switch (signal_id)
           {

               case MAC_TIMER_T3172:
                    /**********************************************************************
                    * the wait indicator received in PAR has expired if PAR was a response*
                    * to RLC attempting an UL tbf est for DATA then RLC must be deblocked *
                    **********************************************************************/
                    MSG_GERAN_MED_1_G( "MAC info timer t3172 expired ",0 );
                    /*GMAC will send access ind once it is connected the downlink tbf*/
                    MSG_GERAN_MED_1_G( "MAC will send access ind once it is connected the downlink tbf",0 );

               break;

               case MAC_TIMER_T3200_1:
               case MAC_TIMER_T3200_2:
               case MAC_TIMER_T3200_3:
               case MAC_TIMER_T3200_4:
               case MAC_TIMER_T3200_5:
               case MAC_TIMER_T3200_6:
               case MAC_TIMER_T3200_7:
               case MAC_TIMER_T3200_8:
                    MSG_GERAN_MED_1_G( "MAC info timer t3200 expired si_id %d",signal_id );
                    t3200_expiry_handler((mac_mac_sig_id_t)signal_id,gas_id );
               break;

#ifdef FEATURE_GSM_EDTM
               case MAC_TIMER_T3196:
                 MSG_GERAN_MED_0_G("GMAC EDTM T3196 Timer expired");
                 gmac_send_mac_grr_edtm_pkt_cs_est_rej(EDTM_CS_REJ_T3196_EXPIRY,gas_id);
               break;
#endif /*FEATURE_GSM_EDTM*/

               default:
                    MSG_GERAN_ERROR_3_G( "Unexpected MAC sig rx. idle state %d task id %d sig id 0x%x",
                                         idle_data_ptr->mac_idle_substate,task_id, signal_id);
               break;
           }
         }
         else
         {
         /*error*/
         }

      break;

      default:
         if(geran_get_nv_recovery_restart_enabled(gas_id))
         {
           MSG_GERAN_ERROR_1_G( "Unknown substate %d", idle_data_ptr->mac_idle_substate);
           MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
           geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
         }
         else
         {
           ERR_GERAN_FATAL_2_G("Unknown mac substate %d",gas_id, idle_data_ptr->mac_idle_substate);
         }
      break;

  }/*end of mac idle substate switch*/
}
/*===========================================================================
===
===  FUNCTION      gmac_idle_start_no_ps_access_process()
===
===  DESCRIPTION
===
===  This function will send NO ACCESS IND to RLC, stop access timers and also sends
===  release req to layer. This function also sets release_cause to GRR_INITIATED_NO_ACCESS
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===
===
===  SIDE EFFECTS
===========================================================================*/
static void gmac_idle_start_no_ps_access_process
(
 gprs_mac_idle_substates_t   current_mac_state,
 mac_l1_sig_t                *mac_l1_sig_ptr,
 delete_pdu_cause_t           delete_pdu_flag,
 gas_id_t                     gas_id
)
{
  gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);
  idle_data_ptr->release_cause = GRR_INITIATED_NO_ACCESS;
  /*GMC block RLC and will unblock RLC once GMAC received rel cnf from L1*/
  gmac_util_block_rlc(delete_pdu_flag,GMAC_IDLE,__LINE__,gas_id);
  /*Depenidng upon the gmac substate, GMC stops all access timers */
  gmac_idle_stop_access_timers(idle_data_ptr->mac_idle_substate,gas_id);

  if(current_mac_state == DL_ASS )
  {
    gmac_idle_send_l1_release_sig(mac_l1_sig_ptr, DL_TBF_NORMAL,MAC_LOG_UL_UNKNOWN_ERROR,MAC_LOG_DL_CELL_RESELECTION,gas_id );
  }
  else
  {
    gmac_idle_send_l1_release_sig(mac_l1_sig_ptr, UL_TBF_NORMAL,MAC_LOG_UL_CELL_RESELECTION,MAC_LOG_DL_UNKNOWN_ERROR,gas_id );
  }
}
/*===========================================================================
===
===  FUNCTION      gmac_idle_ignore_pda()
===
===  DESCRIPTION
===
===  Called when PDA interrupts an ul tbf est, determines when to
===  release l1.
===
===  NOTE use ABNORMAL release of L1 , don't want to wait for any
===  PCA that may be in the RRBP buffer.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  True if PDA is to be ignored.
===
===  SIDE EFFECTS
===========================================================================*/
static boolean gmac_idle_ignore_pda(gprs_mac_action_t mac_act,mac_l1_sig_t *mac_l1_sig_ptr,
                                    gas_id_t gas_id)
{

  boolean   ignore_pda = FALSE;

  if(mac_act == UL_DATA_EST)
  {
    if(gmac_util_get_pcco_state(gas_id) == PCCO_ACTIVE)
    {
        ignore_pda = TRUE;
    }
    else
    {
      gmac_idle_send_l1_release_sig(mac_l1_sig_ptr, UL_TBF_ABNORMAL,MAC_LOG_UL_PDA_ABORT,MAC_LOG_DL_UNKNOWN_ERROR,gas_id);
    }
  }
  else if(mac_act == PMR_EST)
  {
    gmac_idle_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
    gmac_idle_send_l1_release_sig(mac_l1_sig_ptr, UL_TBF_ABNORMAL,MAC_LOG_UL_PDA_ABORT,MAC_LOG_DL_UNKNOWN_ERROR,gas_id);
  }
  else if(mac_act == PCCF_EST)
  {
    ignore_pda = TRUE;
  }

  return(ignore_pda);
}
/*===========================================================================
===
===  FUNCTION      gmac_idle_csn1_decode_stored_pda_data()
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  global csn1 vars
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===========================================================================*/
static void gmac_idle_csn1_decode_stored_pda_data(uint8 *csn1_encoded_pda_msg_ptr,
                                                  gas_id_t gas_id)
{
    /*used for loging not relevent here BUT needed in gcsn1_decoder()*/
    uint16  msg_bit_length =0;
    gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

    /*retrieve message from csn1 */

    global_data_ptr->csn_decoder_result = gcsn1_decoder(
      csn1_encoded_pda_msg_ptr,
      &msg_bit_length,
      SIZE_OF_DL_MSG,
      gas_id
    );
    global_data_ptr->csn_dec_result_ptr = &global_data_ptr->csn_decoder_result;
    global_data_ptr->csn_dec_data_ptr = global_data_ptr->csn_decoder_result.data_ptr;
    global_data_ptr->csn_dec_hdr_ptr = global_data_ptr->csn_decoder_result.header_ptr;
}

/*===========================================================================
===
===  FUNCTION      gmac_idle_ul_release_confirm_handler()
===
===  DESCRIPTION
===
===  On reception of L1_MAC_TBF_REL_CONFIRM this function is called and depending
===  on the current "release_cause"  performs the appropriate release.
===
===  DEPENDENCIES
===
===       release_cause
===       mac_action
===       mac_idle_substate
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===  can alter vars
===                mac_action
===                mac_idle_substate
===                mac_mode
===========================================================================*/
static void gmac_idle_ul_release_confirm_handler(gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_MED_2_G("MAC info current idle sub state %d current mac action",idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action);

  gmac_reset_rac_params(gas_id);
  if(global_data_ptr->mac_action == PMR_EST)
  {
    global_data_ptr->gmac_local_pmr_params.pmr_pending = FALSE;
  }

#ifdef FEATURE_GSM_EDTM
  /* If T3196 is still running then indicate Pkt CS Est Rej to GRR*/
  if( rex_get_timer(&mac_timers_ptr->timer_t3196) != 0 )
  {
    MSG_GERAN_MED_0_G("GMAC EDTM T3196 running on release from Idle");
    gmac_stop_t3196(gas_id);
    gmac_send_mac_grr_edtm_pkt_cs_est_rej(EDTM_CS_REJ_TBF_RELEASE_IN_PROGRESS,gas_id);
  }
#endif /*FEATURE_GSM_EDTM*/

  /* When cause is ACCESS_RETRY, access_attempts_count is always set by gmac_idle_send_no_access_to_rlc
     otherwise if cause is not ACCESS_RETRY reset to zero so that PCCO/PCCF are completed in
     gmac_idle_abnormal_release_handler*/
  if(idle_data_ptr->release_cause != ACCESS_RETRY)
  {
    idle_data_ptr->access_attempts_count = 0;
  }

  switch(idle_data_ptr->release_cause)
  {
      case ACCESS_RETRY:
      case FAIL_TO_SEND_PMR_PCCF:
      case T3164_EXP:
      case UL_EXCEPTION_HANDLING:
      case PAR_RCVD:
      case CONT_RES_COMPLETE:
      case CONT_RES_UL_RE_ALLOC_FAILURE:
           /* For all the above release cause give access ind to RLC if pcco procedure is not on
              and reset all the idle parameters, mac action and mac idle substate to PASSIVE*/
           gmac_idle_abnormal_release_handler(global_data_ptr->mac_action,idle_data_ptr->mac_idle_substate,gas_id);
           gmac_idle_reset_parameters(idle_data_ptr->mac_idle_substate,gas_id);
           global_data_ptr->mac_action = NONE;
           MSG_GERAN_MED_0_G("GMAC State Idle PASSIVE");
           idle_data_ptr->mac_idle_substate = PASSIVE;

           reset_tbf_parameters(gas_id);
      break;

      case PARTIAL_SYS_ACQ:
           /* For this release cause send abnormal release to GRR if pcco is not active and give
              access ind to RLC and reset all the idle parameters, mac action and mac idle substate
              to PASSIVE*/
           if(gmac_util_get_pcco_state(gas_id) == NO_PCCO_ACTIVE)
           {
             gmac_send_mac_grr_abnormal_release(RR_PART_SYS_INFO_REQ,gas_id);
           }
           gmac_idle_abnormal_release_handler(global_data_ptr->mac_action,idle_data_ptr->mac_idle_substate,gas_id);
           gmac_idle_reset_parameters(idle_data_ptr->mac_idle_substate,gas_id);
           global_data_ptr->mac_action = NONE;
           MSG_GERAN_MED_0_G("GMAC State Idle PASSIVE");
           idle_data_ptr->mac_idle_substate = PASSIVE;

           reset_tbf_parameters(gas_id);
      break;

      case PDA_INTERRUPTS_UL_TBF_EST:
      case PDA_RCVD_IN_DL_SB_TBF:
           /*To have got here the PDA msg is error free*/
           gmac_idle_csn1_decode_stored_pda_data(idle_data_ptr->csn1_encoded_pda_msg,gas_id);

           /*needed for releative starting time cal's*/
           global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no = idle_data_ptr->gmac_store_rcvd_fn_of_pda;

           /*PDA address has already been checked when received, processing saved PDA here*/
           idle_data_ptr->pda_data_result = gmac_idle_pda_handler(&idle_data_ptr->dl_ass_ind_sig,TRUE,PDA_NO_ADDRESS_CHECK,gas_id);

           if (idle_data_ptr->release_cause == PDA_RCVD_IN_DL_SB_TBF)
           {
             /*Reset the PCCF state machine*/
             gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,MAC_INITIATED,GMAC_IDLE,__LINE__,gas_id);
           }

           MSG_GERAN_MED_0_G( "MAC sent MAC_L1_DL_TBF_CONFIG");
           idle_data_ptr->mac_l1_sig.mac_l1_dl_tbf_config.message_header.message_set = MS_MAC_L1;
           idle_data_ptr->mac_l1_sig.mac_l1_dl_tbf_config.message_header.message_id = (uint8)MAC_L1_DL_TBF_CONFIG;
           gprs_l1_mac_acc_func(&idle_data_ptr->mac_l1_sig,gas_id);

           /*free up csn.1 buffers*/
           free_csn_decoder_ptrs_t(global_data_ptr->csn_dec_result_ptr);

           idle_data_ptr->i_flags.pda_ul_pacch = FALSE;

           MSG_GERAN_MED_0_G("GMAC State Idle DL_ASS");
           idle_data_ptr->mac_idle_substate = DL_ASS;

           (void)mac_log_state(gas_id);
           global_data_ptr->mac_action = NONE;
           display_mac_action(global_data_ptr->mac_action,gas_id);
      break;

      case GRR_INITIATED_NO_ACCESS:
           /* For this release cause set mac mode to NULL mode and send MAC_IN_NO_SERVICE signal to GMAC
              so that GMAC will move to NULL_INACTIVE state. Reset idle parameters, mac action and mac idle substate
              to PASSIVE*/
           /*send mac status before attempting to complete PCCO process*/
           gmac_util_send_mac_grr_mac_status(&idle_data_ptr->mac_grr_sig,MAC_IDLE,gas_id);
           if(global_data_ptr->mac_action == UL_DATA_EST)
           {
             /*complete the pcco process activites*/
             gmac_util_complete_pcco_process(PCCO_FAILURE_NO_RESPONSE,GRR_INITIATED,GMAC_IDLE,__LINE__,gas_id);
             /*complete pccf process activities*/
             gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,GRR_INITIATED,GMAC_IDLE,__LINE__,gas_id);
           }
           else if(global_data_ptr->mac_action == PMR_EST)
           {
             gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
           }
           else if(global_data_ptr->mac_action == PCCF_EST)
           {
             /*complete pccf process activities*/
             gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,GRR_INITIATED,GMAC_IDLE,__LINE__,gas_id);
           }
           else if(global_data_ptr->mac_action == NONE)
           {
             /*Reset the PCCF state machine*/
             gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,GRR_INITIATED,GMAC_IDLE,__LINE__,gas_id);
           }

           global_data_ptr->mac_mode = NULL_MODE;
           global_data_ptr->mac_action = NONE;
           MSG_GERAN_MED_0_G("GMAC State Idle PASSIVE");
           idle_data_ptr->mac_idle_substate = PASSIVE;
           gmac_idle_send_mac_internal_sig(&idle_data_ptr->mac_int_sig,MAC_IN_NO_SERVICE,gas_id);
           MSG_GERAN_MED_0_G("MAC_GRR_MAC_STATUS");
           gmac_idle_reset_parameters(idle_data_ptr->mac_idle_substate,gas_id);

           reset_tbf_parameters(gas_id);
      break;

      case REL_DL_SB_TBF:
           global_data_ptr->mac_mode = NULL_MODE;
           MSG_GERAN_MED_0_G("MAC set mode to NULL_MODE");
           gmac_util_send_grr_mac_status(MAC_IDLE,gas_id);
           gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,MAC_INITIATED,GMAC_IDLE,__LINE__,gas_id);
           /*In donwlink single block case, GMAC already blocked RLC so unblocking*/
           gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_IDLE,__LINE__,gas_id);
           (void)mac_log_state(gas_id);

           reset_tbf_parameters(gas_id);
      break;

      default:
         MSG_GERAN_HIGH_1_G("MAC info Unexpected UL release cause %d",idle_data_ptr->release_cause);
       break;
  }

  MSG_GERAN_MED_2_G("MAC info current idle sub state %d current mac action",idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action);

}

/*===========================================================================
===
===  FUNCTION      gmac_idle_dl_release_confirm_handler()
===
===  DESCRIPTION
===
===  On reception of L1_MAC_TBF_REL_CONFIRM this function is called from only DL_ASS
===  idle substate
===
===  DEPENDENCIES
===
===       release_cause
===       mac_action
===       mac_idle_substate
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===  can alter vars
===                mac_action
===                mac_idle_substate
===                mac_mode
===========================================================================*/
static void gmac_idle_dl_release_confirm_handler(gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  MSG_GERAN_MED_2_G("MAC info current idle sub state %d current mac action",idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action);

  gmac_reset_rac_params(gas_id);

#ifdef FEATURE_GSM_EDTM
  /* If T3196 is still running then indicate Pkt CS Est Rej to GRR*/
  if( rex_get_timer(&mac_timers_ptr->timer_t3196) != 0 )
  {
    MSG_GERAN_MED_0_G("GMAC EDTM T3196 running on release from Idle");
    gmac_stop_t3196(gas_id);
    gmac_send_mac_grr_edtm_pkt_cs_est_rej(EDTM_CS_REJ_TBF_RELEASE_IN_PROGRESS,gas_id);
  }
#endif /*FEATURE_GSM_EDTM*/

  switch(idle_data_ptr->release_cause)
  {
      /********************************************************
      *     COMMON to more than 1 Idle mode sub state        *
      ********************************************************/
      case REL_DL_TBF:
           if( rex_get_timer(&mac_timers_ptr->timer_t3172) == 0 )
           {
             gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_IDLEUTIL,__LINE__,gas_id);
           }
           gmac_idle_reset_parameters(idle_data_ptr->mac_idle_substate,gas_id);
           global_data_ptr->mac_action = NONE;
           MSG_GERAN_MED_0_G("GMAC State Idle PASSIVE");
           idle_data_ptr->mac_idle_substate = PASSIVE;
           idle_data_ptr->access_attempts_count=0;
           global_data_ptr->mac_mode = NULL_MODE;
           MSG_GERAN_MED_0_G("MAC_GRR_MAC_STATUS");
           gmac_util_send_mac_grr_mac_status(&idle_data_ptr->mac_grr_sig,MAC_IDLE,gas_id);
      break;

      case PARTIAL_SYS_ACQ:
           gmac_send_mac_grr_abnormal_release(RR_PART_SYS_INFO_REQ,gas_id);
           if( rex_get_timer(&mac_timers_ptr->timer_t3172) == 0 )
           {
             gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_IDLEUTIL,__LINE__,gas_id);
           }

           gmac_idle_reset_parameters(idle_data_ptr->mac_idle_substate,gas_id);
           global_data_ptr->mac_action = NONE;
           MSG_GERAN_MED_0_G("GMAC State Idle PASSIVE");
           idle_data_ptr->mac_idle_substate = PASSIVE;
           idle_data_ptr->access_attempts_count=0;
           global_data_ptr->mac_mode = NULL_MODE;
           MSG_GERAN_MED_0_G("MAC_GRR_MAC_STATUS");
           gmac_util_send_mac_grr_mac_status(&idle_data_ptr->mac_grr_sig,MAC_IDLE,gas_id);
      break;

      case GRR_INITIATED_NO_ACCESS:

           global_data_ptr->mac_mode = NULL_MODE;
           global_data_ptr->mac_action = NONE;
           MSG_GERAN_MED_0_G("GMAC State Idle PASSIVE");
           idle_data_ptr->mac_idle_substate = PASSIVE;
           gmac_idle_send_mac_internal_sig(&idle_data_ptr->mac_int_sig,MAC_IN_NO_SERVICE,gas_id);
           gmac_idle_reset_parameters(idle_data_ptr->mac_idle_substate,gas_id);
           MSG_GERAN_MED_0_G("MAC_GRR_MAC_STATUS");
           gmac_util_send_mac_grr_mac_status(&idle_data_ptr->mac_grr_sig,MAC_IDLE,gas_id);
           idle_data_ptr->access_attempts_count=0;
      break;

      default:
         MSG_GERAN_HIGH_1_G("MAC info Unexpected UL release cause %d",idle_data_ptr->release_cause);
       break;
  }
  MSG_GERAN_MED_2_G("MAC info current idle sub state %d current mac action",idle_data_ptr->mac_idle_substate,global_data_ptr->mac_action);
}
/*===========================================================================
===
===  FUNCTION      display_dl_msg_rcvd()
===
===  DESCRIPTION
===
===  Display DL logical channel msg received
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void display_dl_msg_rcvd
(
  uint8 dl_chan_msg,uint32 line_no,
  gprs_mac_idle_substates_t mac_state,
  gprs_mac_action_t current_action,
  gas_id_t          gas_id
)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  NOTUSED(current_action);
  NOTUSED(mac_state);
  NOTUSED(line_no);

  switch(dl_chan_msg)
  {
      case PACKET_ACCESS_REJECT:
           MSG_GERAN_LOW_3_G( "MAC Rcvd PAR, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_CELL_CHANGE_ORDER:
           MSG_GERAN_LOW_3_G( "MAC Rcvd PCCO, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_DOWNLINK_ASSIGNMENT:
           MSG_GERAN_LOW_3_G( "MAC Rcvd PDA, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_MEASUREMENT_ORDER:
           MSG_GERAN_LOW_3_G( "MAC Rcvd PMO, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_PAGING_REQUEST:
           MSG_GERAN_LOW_3_G( "MAC Rcvd PPR, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_PDCH_RELEASE:
           MSG_GERAN_LOW_3_G( "MAC Rcvd P_PDCH_REL, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_POLLING_REQUEST :
           MSG_GERAN_LOW_3_G( "MAC Rcvd P_POLLING_REQ, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_PWR_CTRL_TMNG_ADV:
           MSG_GERAN_LOW_3_G( "MAC Rcvd PPCTA, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_PRACH_PARAMETERS :
           MSG_GERAN_LOW_3_G( "MAC Rcvd P_PRACH_PARAM, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_QUEUEING_NOTIFIACTION:
           MSG_GERAN_LOW_3_G( "MAC Rcvd PQN, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_TIMESLOT_RECONFIGURE:
           MSG_GERAN_LOW_3_G( "MAC Rcvd PTR, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_TBF_RELEASE:
           MSG_GERAN_LOW_3_G( "MAC Rcvd P_TBF_REL, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_UPLINK_ACK_NACK:
           MSG_GERAN_LOW_3_G( "MAC Rcvd PUAN, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_UPLINK_ASSIGNMENT:
           MSG_GERAN_LOW_3_G( "MAC Rcvd PUA, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_DOWNLINK_DUMMY_CNTRL_BLK :
           MSG_GERAN_LOW_3_G( "MAC Rcvd PDDCB, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_PSI1:
           MSG_GERAN_LOW_3_G( "MAC Rcvd PSI1, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_PSI2:
           MSG_GERAN_LOW_3_G( "MAC Rcvd PSI2, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_PSI3:
           MSG_GERAN_LOW_3_G( "MAC Rcvd PSI3, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_PSI3_BIS:
           MSG_GERAN_LOW_3_G( "MAC Rcvd PSI3_BIS, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_PSI4:
           MSG_GERAN_LOW_3_G( "MAC Rcvd PSI4, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_PSI5:
           MSG_GERAN_LOW_3_G( "MAC Rcvd PSI5, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      case PACKET_PSI13:
           MSG_GERAN_LOW_3_G( "MAC Rcvd PSI13, ln %d, m_state %d, m_act %d ",line_no,mac_state ,current_action );
      break;

      default:
           MSG_GERAN_LOW_3_G( "MAC Rcvd DL ln  %d, msg %d, m_state %d",line_no,global_data_ptr->channel_msg,mac_state );
      break;
  }
}

/*===========================================================================
===
===  FUNCTION      display_signal_rcvd()
===
===  DESCRIPTION
===
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void display_signal_rcvd
(
  uint8 sig_id,uint8 task_id,uint32 line_no,
  gprs_mac_idle_substates_t mac_state,
  gprs_mac_action_t current_action,
  gas_id_t          gas_id
)
{
   /* this occurs becasue */
   /* "DEFAULT" condition does not reference "current_action"         */
   NOTUSED(current_action);

   switch (task_id)
   {
     case MS_MAC_L1 :
           switch (sig_id)
           {
             case L1_MAC_FREQ_ASS_CNF:
                  MSG_GERAN_MED_3_G( "MAC Rcvd L1_MAC_FREQ_ASS_CNF, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case L1_MAC_USF_DETECTED:
                  MSG_GERAN_MED_3_G( "MAC Rcvd L1_MAC_USF_DETECTED, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case L1_MAC_1ST_DATA_BLOCK_SENT:
                  MSG_GERAN_MED_3_G( "MAC Rcvd L1_MAC_1ST_DATA_BLOCK_SENT, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case L1_MAC_QUAL_MEASURMENTS:
                  MSG_GERAN_LOW_3_G( "MAC Rcvd L1_MAC_QUAL_MEASURMENTS, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case L1_MAC_TBF_REL_CONFIRM:
                  MSG_GERAN_MED_3_G( "MAC Rcvd L1_MAC_TBF_REL_CONFIRM, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case MAC_PH_CONNECT_IND:
                  MSG_GERAN_MED_3_G( "MAC Rcvd MAC_PH_CONNECT_IND , ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case MAC_PH_DATA_IND:
                  MSG_GERAN_MED_3_G( "MAC Rcvd MAC_PH_DATA_IND, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case MAC_PH_RANDOM_ACCESS_CONF:
                  MSG_GERAN_MED_3_G( "MAC Rcvd MAC_PH_RANDOM_ACCESS_CONF, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case L1_MAC_51_SB_FAILURE:
                  MSG_GERAN_MED_3_G( "MAC Rcvd L1_MAC_51_SB_FAILURE, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case L1_MAC_SINGLE_BLOCK_SENT:
               MSG_GERAN_MED_1_G( "MAC sent L1_MAC_SINGLE_BLOCK_SENT, ln %d, m_state %d",line_no);
             break;

             case L1_MAC_RRBP_FAILED_IND:
               MSG_GERAN_MED_1_G( "MAC sent L1_MAC_RRBP_FAILED_IND, ln %d, m_state %d",line_no);
             break;

             default:
                  MSG_GERAN_ERROR_3_G( "MAC Rcvd UNEXPECTED sig 0x%x, m_state =%d, ln  %d",sig_id,mac_state ,line_no );
             break;
           }
     break;

     case MS_MAC_RR :
           switch (sig_id)
           {
             case GRR_MAC_NO_PS_ACCESS_IND:
                  MSG_GERAN_MED_3_G( "MAC Rcvd GRR_MAC_NO_PS_ACCESS_IND, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case GRR_MAC_PS_ACCESS_REQ:
                  MSG_GERAN_MED_3_G( "MAC Rcvd GRR_MAC_PS_ACCESS_REQ, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case GRR_MAC_SEND_MEAS_RPT:
                  MSG_GERAN_LOW_3_G( "MAC Rcvd GRR_MAC_SEND_MEAS_RPT, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case GRR_MAC_SEND_PCCF:
                  MSG_GERAN_MED_3_G( "MAC Rcvd GRR_MAC_SEND_PCCF, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case GRR_MAC_DL_TBF_EST:
                  MSG_GERAN_MED_3_G( "MAC Rcvd GRR_MAC_DL_TBF_EST, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case GRR_MAC_DL_SB_EST:
                  MSG_GERAN_MED_3_G( "MAC Rcvd GRR_MAC_DL_SB_EST, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case GRR_MAC_UL_TBF_EST:
                  MSG_GERAN_MED_3_G( "MAC Rcvd GRR_MAC_UL_TBF_EST, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case GRR_MAC_UL_SB_EST:
                  MSG_GERAN_MED_3_G( "MAC Rcvd GRR_MAC_UL_SB_EST, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

#ifdef FEATURE_GSM_EDTM
             case GRR_MAC_EDTM_PKT_CS_EST_REQ:
                  MSG_GERAN_MED_3_G( "MAC Rcvd GRR_MAC_EDTM_PKT_CS_EST_REQ, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;
#endif /*FEATURE_GSM_EDTM*/

             default:
                  MSG_GERAN_ERROR_3_G( "MAC Rcvd UNEXPECTED sig 0x%x, m_state =%d, ln  %d",sig_id,mac_state ,line_no );
             break;

           }
     break;

     case MS_MAC_RLC :
           switch (sig_id)
           {
             case RM_UL_EST_TBF_REQ:
                  MSG_GERAN_MED_3_G( "MAC Rcvd RM_UL_EST_TBF_REQ, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case RM_UL_REALLOC_REQ:
                  MSG_GERAN_MED_3_G( "MAC Rcvd RM_UL_REALLOC_REQ, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case RM_UL_RELEASE_IND:
                  MSG_GERAN_MED_3_G( "MAC Rcvd RM_UL_RELEASE_IND, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case RM_UL_N3104_EXPIRY_IND:
                  MSG_GERAN_MED_3_G( "MAC Rcvd RM_UL_N3104_EXPIRY_IND, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case RM_DL_PKT_ACKNACK_REQ:
                  MSG_GERAN_MED_3_G( "MAC Rcvd RM_DL_PKT_ACKNACK_REQ, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case RM_DL_PKT_CTRL_ACK_REQ:
                  MSG_GERAN_MED_3_G( "MAC Rcvd RM_DL_PKT_CTRL_ACK_REQ, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case RM_DL_FIRST_BLOCK_IND:
                  MSG_GERAN_MED_3_G( "MAC Rcvd RM_DL_FIRST_BLOCK_IND, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case RM_DL_RELEASE_IND:
                  MSG_GERAN_MED_3_G( "MAC Rcvd RM_DL_RELEASE_IND, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             case RM_PS_ACCESS_CNF:
                  MSG_GERAN_MED_3_G( "MAC Rcvd RM_PS_ACCESS_CNF, ln %d, m_state %d, m_act %d",line_no,mac_state ,current_action );
             break;

             default:
                  MSG_GERAN_ERROR_3_G( "MAC Rcvd UNEXPECTED sig 0x%x, m_state =%d, ln  %d",sig_id,mac_state ,line_no );
             break;
           }
     break;

     case MS_MAC_MAC:
     break;

    default:
          MSG_GERAN_ERROR_3_G( "MAC Rcvd UNEXPECTED sig 0x%x, m_state =%d, ln  %d",sig_id,mac_state ,line_no );
    break;
   } /*end of main switch*/
}



/*===========================================================================
===
===  FUNCTION      display_mac_action()
===
===  DESCRIPTION
===  display mac action
===
===  DEPENDENCIES
===  RETURN VALUE
===  SIDE EFFECTS
===========================================================================*/
static void display_mac_action(gprs_mac_action_t mac_act,
                               gas_id_t gas_id)
{

 switch (mac_act)
 {
   case NONE:
      MSG_GERAN_HIGH_0_G( "MAC Info mac_action NONE");
   break;

   case UL_DATA_EST:
      MSG_GERAN_HIGH_0_G( "MAC Info mac_action UL_DATA_EST");
   break;

   case PCCF_EST:
      MSG_GERAN_HIGH_0_G( "MAC Info mac_action PCCF_EST");
   break;

   case PMR_EST:
      MSG_GERAN_HIGH_0_G( "MAC Info mac_action PMR_EST");
   break;

   default :
      MSG_GERAN_ERROR_1_G( "MAC unknown action %d",mac_act);
   break;
 }

}

/*===========================================================================
===
===  FUNCTION    gmac_init_idle_flags()
===
===  DESCRIPTION
===  Initialises the idle flags, should only be called once at power on.
===
===  PARAMS
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_idle_flags( gas_id_t gas_id )
{
  gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);

  idle_data_ptr->i_flags.pda_ul_pacch = FALSE;
  idle_data_ptr->i_flags.idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION;


  idle_data_ptr->i_flags.rcvd_unsolicited_pua_waiting_for_connected_ind = FALSE;

  idle_data_ptr->i_flags.pending_realloc_req = FALSE;
}

/*===========================================================================
===
===  FUNCTION    gmac_init_idle_data()
===
===  DESCRIPTION
===  Initialises the idle data, used for lower layer resets.
===
===  PARAMS
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_idle_data( gas_id_t gas_id )
{
  gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);

  gmac_init_idle_data_non_zero(gas_id);

  idle_data_ptr->mac_rlc_sig_ptr = NULL;
  memset(&idle_data_ptr->mac_int_sig,0,sizeof(idle_data_ptr->mac_int_sig));
  memset(&idle_data_ptr->mr_ul_alloc_cnf_sig,0,sizeof(idle_data_ptr->mr_ul_alloc_cnf_sig));
  memset(&idle_data_ptr->dl_ass_ind_sig,0,sizeof(idle_data_ptr->dl_ass_ind_sig));
  idle_data_ptr->rrps_tlli = 0;
  memset(&idle_data_ptr->contention_result,0,sizeof(idle_data_ptr->contention_result));
  idle_data_ptr->t3168_timeout = 0;
  memset(idle_data_ptr->csn1_encoded_pda_msg,0,sizeof(idle_data_ptr->csn1_encoded_pda_msg));
  idle_data_ptr->gmac_store_rcvd_fn_of_pda = 0;
  memset(&idle_data_ptr->tbf_rel,0,sizeof(idle_data_ptr->tbf_rel));
  memset(&idle_data_ptr->egprs_cont_res_pua,0,sizeof(idle_data_ptr->egprs_cont_res_pua));
  memset(&idle_data_ptr->temp_egprs_2p_sb_mb_alloc,0,sizeof(idle_data_ptr->temp_egprs_2p_sb_mb_alloc));
}

/*===========================================================================
===
===  FUNCTION    gmac_init_idle_data_non_zero()
===
===  DESCRIPTION
===  Initialises non zero (including all enums) idle data..
===
===  PARAMS
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_idle_data_non_zero( gas_id_t gas_id )
{
  gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);
  idle_data_ptr->mac_idle_substate = PASSIVE;
  idle_data_ptr->pua_data_result = NO_ERRORS;
  idle_data_ptr->pda_data_result = NO_ERRORS;
  idle_data_ptr->release_cause = NO_RELEASE_CAUSE;
  idle_data_ptr->egprs_pua_contention_result = PUA_CONT_RES_NOT_APPLICABLE;
}

/*===========================================================================
===
===  FUNCTION    gmac_idle_simulate_freq_cnf_for_ul_states()
===
===  DESCRIPTION
===  Simulates the reception of L1_MAC_FREQ_CNF in the UL Idle states.
===
===  PARAMS
===    none
===
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===    Reads:
===
===    Writes:
===
===
===========================================================================*/
static void gmac_idle_simulate_freq_cnf_for_ul_states( gas_id_t gas_id )
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);

  /*********** dynamic handler **************/
  if ( global_data_ptr->tbf_type == (uint8)dynamic_alloc_e )
  {
    switch ( idle_data_ptr->mac_idle_substate )
    {
      case UL_1P_ASS:
        if (global_data_ptr->mac_action == PMR_EST || global_data_ptr->mac_action == PCCF_EST)
        {
            MSG_GERAN_HIGH_1_G("MAC error abnormal alloc with mac_action %d",global_data_ptr->mac_action);
            idle_data_ptr->release_cause = FAIL_TO_SEND_PMR_PCCF;
            gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,(mac_log_ul_tbf_release_t)idle_data_ptr->pua_data_result,MAC_LOG_DL_UNKNOWN_ERROR,
                                          gas_id );
        }
        else
        {
          if (gmac_util_get_pccf_state(gas_id) == SENDING_VIA_UL_PACCH)
          {
#ifdef FEATURE_GSM_EDTM
             /*Delete PCCF message if already present in L1 queues*/
             gmac_send_mac_l1_delete_ul_ctrl_msg(PACKET_CELL_CHANGE_FAILURE,gas_id);
#endif/*FEATURE_GSM_EDTM*/
             /*case where pcco interrupted a UL transfer, but pcco fialed and need to send PCCF*/
             MSG_GERAN_MED_0_G( "MAC sent MAC_PH_DATA_REQ with PCCF");
             gmac_util_send_ul_control_msg_handler(PCCF, UL_PRESENT, GRR_CNF_NOT_NEEDED,gas_id);
          }
          /*for 1 phase*/
          if( (idle_data_ptr->access_granted == one_phase) &&
              (global_data_ptr->access_tech_req_present == TRUE) &&
              (global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)  )
          {
             gmac_idle_egprs_rac_procedure_2(FALSE,global_data_ptr->c_flags.egprs_rac_retran_req,FALSE,FALSE,gas_id);
          }

          /*note for 2 phase perfrom RAC procedure when L1_MAC_QUAL_MEASURMENTS is rcvd ie at the point of
          generating the PRR for the second part of 2p access.    */

          gmac_ul_dummy_blk_to_l1(gas_id);
          /*   send MAC_L1_DYNAMIC_TBF_CONFIG  to L1 and MR_UL_ALLOC_CNF to RLC         */
          MSG_GERAN_MED_0_G( "MAC sent MAC_L1_DYNAMIC_TBF_CONFIG");
          MSG_GERAN_MED_0_G( "MAC sent MR_UL_ALLOC_CNF");
          gmac_idle_send_tbf_config_sigs(&idle_data_ptr->mac_l1_sig,idle_data_ptr->mac_rlc_sig_ptr,MAC_L1_DYNAMIC_TBF_CONFIG,&idle_data_ptr->mr_ul_alloc_cnf_sig,
                                         gas_id);
        }
      break;

      case UL_2P_ASS:
        gmac_ul_dummy_blk_to_l1(gas_id);

        /*complete pcco process activities*/
        gmac_util_complete_pcco_process(PCCO_SUCCESS,MAC_INITIATED,GMAC_IDLE,__LINE__,gas_id);

        if (global_data_ptr->mac_action == PMR_EST || global_data_ptr->mac_action == PCCF_EST)
        {

          MSG_GERAN_HIGH_1_G("MAC error abnormal alloc with mac_action %d",global_data_ptr->mac_action);
          idle_data_ptr->release_cause = FAIL_TO_SEND_PMR_PCCF;
          gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,(mac_log_ul_tbf_release_t)idle_data_ptr->pua_data_result,MAC_LOG_DL_UNKNOWN_ERROR,
                                          gas_id );
          }
        else
        {
          if (gmac_util_get_pccf_state(gas_id) == SENDING_VIA_UL_PACCH)
          {
#ifdef FEATURE_GSM_EDTM
             /*Delete PCCF message if already present in L1 queues*/
             gmac_send_mac_l1_delete_ul_ctrl_msg(PACKET_CELL_CHANGE_FAILURE,gas_id);
#endif/*FEATURE_GSM_EDTM*/
             /*case where pcco interrupted a UL transfer, but pcco fialed and need to send PCCF*/
             MSG_GERAN_MED_0_G( "MAC sent MAC_PH_DATA_REQ with PCCF");
             gmac_util_send_ul_control_msg_handler(PCCF, UL_PRESENT, GRR_CNF_NOT_NEEDED, gas_id);
          }
          /*change 2p rac*/

          if((global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS) &&
               ( global_data_ptr->access_tech_req_present == TRUE ) &&
               ( idle_data_ptr->temp_egprs_2p_sb_mb_alloc == EGPRS_SB_ALLOC ||
                 idle_data_ptr->temp_egprs_2p_sb_mb_alloc == MB_ALLOC_OF_1_BLOCK ) )
          {
            /*case where SB alloc or MB alloc of 1 block was rcvd and there was insufficent space to fit*/
            /*all RAC info into the PRR so network sends Access Tech req in PUA Dyn and expects the ARAC to*/
            /*sent*/

            MSG_GERAN_MED_1_G( "MAC info ARAC retran from PUA DYN, ARAC reformatted",0 );
            gmac_send_add_rac_info_msg(NO_RE_EST,gas_id);

          } else if ( ( global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS ) &&
             (global_data_ptr->c_flags.egprs_arac_sent == TRUE) &&
             (global_data_ptr->c_flags.egprs_rac_retran_req == TRUE))
          {
            /* or NW allocates a MB (2 block) and then request a retrans by sending a dyn PUA
               with the ARAC retrans bit set*/
            MSG_GERAN_MED_1_G( "MAC info ARAC retran, from PUA DYN with ARAC retran bit set",0 );
            gmac_send_add_rac_info_msg(NO_RE_EST,gas_id);
          }

          MSG_GERAN_MED_0_G( "MAC sent MR_UL_ALLOC_CNF");
          MSG_GERAN_MED_0_G( "MAC sent MAC_L1_DYNAMIC_TBF_CONFIG");
          gmac_idle_send_tbf_config_sigs(&idle_data_ptr->mac_l1_sig,idle_data_ptr->mac_rlc_sig_ptr,MAC_L1_DYNAMIC_TBF_CONFIG,&idle_data_ptr->mr_ul_alloc_cnf_sig,
                                         gas_id);
        }
      break;

      case UL_1P_CONTENTION_RES:
        gmac_ul_dummy_blk_to_l1(gas_id);
        /*complete pccf process activities*/
        gmac_util_complete_pcco_process(PCCO_SUCCESS,MAC_INITIATED,GMAC_IDLE,__LINE__,gas_id);
        if (global_data_ptr->mac_action == PMR_EST || global_data_ptr->mac_action == PCCF_EST)
        {
          MSG_GERAN_HIGH_1_G("MAC abnormal allocation for mac_action %d",global_data_ptr->mac_action);
        }
        else
        {
          if (gmac_util_get_pccf_state(gas_id) == SENDING_VIA_UL_PACCH)
          {
#ifdef FEATURE_GSM_EDTM
             /*Delete PCCF message if already present in L1 queues*/
             gmac_send_mac_l1_delete_ul_ctrl_msg(PACKET_CELL_CHANGE_FAILURE,gas_id);
#endif/*FEATURE_GSM_EDTM*/
             /*case where pcco interrupted a UL transfer, but pcco fialed and need to send PCCF*/
             MSG_GERAN_MED_0_G( "MAC sent MAC_PH_DATA_REQ with PCCF");
             gmac_util_send_ul_control_msg_handler(PCCF, UL_PRESENT, GRR_CNF_NOT_NEEDED, gas_id);
          }

          if(idle_data_ptr->i_flags.idle_ul_realloc_state == IDLE_REALLOC_REQUESTED)
          {
            /* dont send MR_UL_ALLOC_CNF do this if PH CONNECT IND is rcvd in UL_1P_CONTENTION_RES*/
            MSG_GERAN_MED_0_G("MAC info performing UL re-alloc, wait for con ind before sending MR_UL_ALLOC_CNF ");
            MSG_GERAN_MED_0_G("MAC sent MAC_L1_DYNAMIC_TBF_CONFIG");
            idle_data_ptr->mac_l1_sig.mac_l1_dynamic_tbf_config.message_header.message_set = MS_MAC_L1;
            idle_data_ptr->mac_l1_sig.mac_l1_dynamic_tbf_config.message_header.message_id = (uint8)MAC_L1_DYNAMIC_TBF_CONFIG;
            gprs_l1_mac_acc_func(&idle_data_ptr->mac_l1_sig, gas_id);
          }
          else
          {
            /*do the old stuff*/


            /*note even for un solicotied PUA, we should not have sent */
            /* MR_UL_ALLOC_CNF to RLC until the connected ind was rcvd !!! */
            MSG_GERAN_MED_0_G("MAC info performing unsolicited pua UL re-alloc, wait for con ind before sending MR_UL_ALLOC_CNF ");
            MSG_GERAN_MED_0_G( "MAC sent MAC_L1_DYNAMIC_TBF_CONFIG");
            idle_data_ptr->mac_l1_sig.mac_l1_dynamic_tbf_config.message_header.message_set = MS_MAC_L1;
            idle_data_ptr->mac_l1_sig.mac_l1_dynamic_tbf_config.message_header.message_id = (uint8)MAC_L1_DYNAMIC_TBF_CONFIG;
            gprs_l1_mac_acc_func(&idle_data_ptr->mac_l1_sig, gas_id);

          }
        }
      break;

      default:

          MSG_GERAN_ERROR_1_G("GMAC INFO dynamic in BAD mac idle substate:0x%x",idle_data_ptr->mac_idle_substate);

          if(global_data_ptr->mac_action == UL_DATA_EST)
        {
           /*GMAC sends block signal to RLC and will be unblocked rlc once gmac received release conf from L1*/
           idle_data_ptr->release_cause = ACCESS_RETRY;
           gmac_idle_send_no_access_to_rlc(gas_id);
        }

          if (global_data_ptr->mac_action == PMR_EST || global_data_ptr->mac_action == PCCF_EST)
        {
            idle_data_ptr->release_cause = FAIL_TO_SEND_PMR_PCCF;
        }

          gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,(mac_log_ul_tbf_release_t)idle_data_ptr->pua_data_result,MAC_LOG_DL_UNKNOWN_ERROR,
                                          gas_id );
      break;
    }
  }
  /*********** single block handler **************/
  else if ( global_data_ptr->tbf_type == (uint8)single_blk_alloc )
  {
    switch ( idle_data_ptr->mac_idle_substate )
    {
      case UL_1P_ASS:
        MSG_GERAN_MED_0_G( "MAC sent MAC_L1_SINGLE_BLOCK_CONFIG");
        idle_data_ptr->mac_l1_sig.mac_l1_single_block_config.message_header.message_set = MS_MAC_L1;
        idle_data_ptr->mac_l1_sig.mac_l1_single_block_config.message_header.message_id = (uint8)MAC_L1_SINGLE_BLOCK_CONFIG;
        gprs_l1_mac_acc_func(&idle_data_ptr->mac_l1_sig, gas_id);
        /*load l1 with appropriate ul control channel msg*/

        if(global_data_ptr->mac_action == PMR_EST)
        {
           MSG_GERAN_MED_0_G( "MAC sent MAC_PH_DATA_REQ with PMR");
           gmac_util_send_ul_control_msg_handler(PMR, UL_PRESENT, GRR_CNF_NOT_NEEDED, gas_id);
        }
        else if (global_data_ptr->mac_action == PCCF_EST)
        {
           MSG_GERAN_MED_0_G( "MAC sent MAC_PH_DATA_REQ with PCCF");
           gmac_util_send_ul_control_msg_handler(PCCF, UL_PRESENT, GRR_CNF_NOT_NEEDED, gas_id);
        }
      break;

      case UL_2P_ASS:

        MSG_GERAN_HIGH_0_G("MAC error abnormal SB alloc in 2 phase");

        if(global_data_ptr->mac_action == UL_DATA_EST)
        {
          /*GMAC sends block signal to RLC and will be unblocked rlc once gmac received release conf from L1*/
          idle_data_ptr->release_cause = ACCESS_RETRY;
          gmac_idle_send_no_access_to_rlc(gas_id);
        }

        gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,(mac_log_ul_tbf_release_t)idle_data_ptr->pua_data_result,MAC_LOG_DL_UNKNOWN_ERROR,
                                          gas_id );
      break;

      default:

        MSG_GERAN_ERROR_1_G("GMAC INFO single block in BAD mac idle substate:0x%x",idle_data_ptr->mac_idle_substate);

        if(global_data_ptr->mac_action == UL_DATA_EST)
        {
          /*GMAC sends block signal to RLC and will be unblocked rlc once gmac received release conf from L1*/
          idle_data_ptr->release_cause = ACCESS_RETRY;
          gmac_idle_send_no_access_to_rlc(gas_id);
        }

        if (global_data_ptr->mac_action == PMR_EST || global_data_ptr->mac_action == PCCF_EST)
        {
          idle_data_ptr->release_cause = FAIL_TO_SEND_PMR_PCCF;
        }

        gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,(mac_log_ul_tbf_release_t)idle_data_ptr->pua_data_result,MAC_LOG_DL_UNKNOWN_ERROR,
                                          gas_id );
      break;
    }
  }
  /*********** multi block handler **************/
  else if ( global_data_ptr->tbf_type == (uint8)multi_blk_allocation )
  {
    switch ( idle_data_ptr->mac_idle_substate )
    {
      case UL_1P_ASS:
        MSG_GERAN_MED_0_G( "MAC sent MAC_L1_SINGLE_BLOCK_CONFIG");
        idle_data_ptr->mac_l1_sig.mac_l1_single_block_config.message_header.message_set = MS_MAC_L1;
        idle_data_ptr->mac_l1_sig.mac_l1_single_block_config.message_header.message_id = (uint8)MAC_L1_SINGLE_BLOCK_CONFIG;
        gprs_l1_mac_acc_func(&idle_data_ptr->mac_l1_sig, gas_id);

        /*When a MB alloc of 2 blocks is Rcvd always send MAC_L1_DUMMY_CTRL_BLOCK to L1*/
        if(global_data_ptr->egprs_2p_sb_mb_alloc == MB_ALLOC_OF_2_BLOCKS)
        {
          gmac_ul_dummy_blk_to_l1(gas_id);
        }
      break;

      case UL_2P_ASS:
        MSG_GERAN_MED_0_G( "MAC sent MAC_L1_SINGLE_BLOCK_CONFIG");
        idle_data_ptr->mac_l1_sig.mac_l1_single_block_config.message_header.message_set = MS_MAC_L1;
        idle_data_ptr->mac_l1_sig.mac_l1_single_block_config.message_header.message_id = (uint8)MAC_L1_SINGLE_BLOCK_CONFIG;
        gprs_l1_mac_acc_func(&idle_data_ptr->mac_l1_sig, gas_id);

        MSG_GERAN_MED_1_G( "MAC info ARAC retran via MB alloc",0 );
        gmac_send_add_rac_info_msg(NO_RE_EST, gas_id);
      break;

      default:

        MSG_GERAN_ERROR_1_G("GMAC INFO multi block in BAD mac idle substate:0x%x",idle_data_ptr->mac_idle_substate);

        if(global_data_ptr->mac_action == UL_DATA_EST)
        {
          /*GMAC sends block signal to RLC and will be unblocked rlc once gmac received release conf from L1*/
          idle_data_ptr->release_cause = ACCESS_RETRY;
          gmac_idle_send_no_access_to_rlc(gas_id);
        }

        if (global_data_ptr->mac_action == PMR_EST || global_data_ptr->mac_action == PCCF_EST)
        {
          idle_data_ptr->release_cause = FAIL_TO_SEND_PMR_PCCF;
        }

        gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,(mac_log_ul_tbf_release_t)idle_data_ptr->pua_data_result,MAC_LOG_DL_UNKNOWN_ERROR,
                                          gas_id );
      break;
    }
  }
  else
  {

    MSG_GERAN_ERROR_1_G("MAC info bad tbf_type: 0x%x",global_data_ptr->tbf_type);

    if(global_data_ptr->mac_action == UL_DATA_EST)
    {
      /*GMAC sends block signal to RLC and will be unblocked rlc once gmac received release conf from L1*/
      idle_data_ptr->release_cause = ACCESS_RETRY;
      gmac_idle_send_no_access_to_rlc(gas_id);
    }

    if (global_data_ptr->mac_action == PMR_EST || global_data_ptr->mac_action == PCCF_EST)
    {
      idle_data_ptr->release_cause = FAIL_TO_SEND_PMR_PCCF;
    }

    gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig, UL_TBF_ABNORMAL,(mac_log_ul_tbf_release_t)idle_data_ptr->pua_data_result,MAC_LOG_DL_UNKNOWN_ERROR,
                                          gas_id );
  }
}
/*===========================================================================
===
===  FUNCTION    gmac_idle_handle_pua_error_result()
===
===  DESCRIPTION
===  Handles the frequency check result of the PUA in Idle.
===
===  PARAMS
===    none
===
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===    Reads:    pua_data_result
===              mac_action
===    Writes:   release_cause
===              access_attempts_count
===
===
===========================================================================*/
static void gmac_idle_handle_pua_error_result( gas_id_t gas_id )
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);

  gmac_idle_stop_access_timers(idle_data_ptr->mac_idle_substate, gas_id);

  switch ( idle_data_ptr->pua_data_result )
  {
    case FREQ_ERROR_PARTIAL_SYS_ACQ:

         if(global_data_ptr->mac_action == UL_DATA_EST)
         {
           /*GMAC will send del one to GRLC in the following cases.
             1. PCCO is ACTIVE and establishment cause is CELL UPDATE
             2. NO PCCO ACTIVE case
             GMAC will send no del to GRLC only when PCCO is ACTIVE and establishment cause is not CELL UPDATE
           */
           gmac_idle_send_del_one_or_no_del_to_rlc(gas_id);
         }
         idle_data_ptr->access_attempts_count = 0;

         MSG_GERAN_HIGH_0_G("GMAC INFO release_cause : PARTIAL_SYS_ACQ");
         idle_data_ptr->release_cause = PARTIAL_SYS_ACQ;
    break;

    case FREQ_ERROR_ACCESS_RETRY:
    case MULTISLOT_ERROR_ACCESS_RETRY:
    case TBF_STARTING_TIME_ERROR_ACCESS_RETRY:
    case ASSIGNMENT_DATA_ERROR:
    case FREQ_CONFIG_FAILED_CHECK:
         if(global_data_ptr->mac_action == UL_DATA_EST)
         {
           /*GMAC sends block signal to RLC and will be unblocked rlc once gmac received release conf from L1*/
           gmac_idle_send_no_access_to_rlc(gas_id);
         }
         MSG_GERAN_HIGH_0_G("GMAC INFO release_cause : ACCESS_RETRY");
         idle_data_ptr->release_cause = ACCESS_RETRY;
    break;

    default:
      MSG_GERAN_HIGH_1_G("GMAC Info unhandled Error cause in Idle PUA handler:0x%x",idle_data_ptr->pua_data_result);
    break;
  }

  gmac_idle_send_l1_release_sig(&idle_data_ptr->mac_l1_sig,
                                      UL_TBF_ABNORMAL,
                                      (mac_log_ul_tbf_release_t)idle_data_ptr->pua_data_result,
                                      MAC_LOG_DL_UNKNOWN_ERROR,
                                      gas_id );
}
/*===========================================================================
===
===  FUNCTION    gmac_idle_handle_no_freq_condition_in_pua()
===
===  DESCRIPTION
===  Handles the condition where there is no frequency in the PUA.
===
===  PARAMS
===    none
===
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===    Reads:    access_tech_req_present
===              current_tbf_mode
===              tbf_type
===              temp_egprs_2p_sb_mb_alloc
===              mac_l1_sig
===              pua_data_result
===              c_flags.egprs_rac_retran_req
===
===    Writes:   mac_idle_substate
===              valid_tqi
===              re_trans_pcr_cnt
===              release_cause
===
===
===========================================================================*/
static void gmac_idle_handle_no_freq_condition_in_pua( gas_id_t gas_id )
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);

  switch ( idle_data_ptr->mac_idle_substate )
  {
    case UL_1P_ASS:
      if((idle_data_ptr->access_granted == one_phase) || (global_data_ptr->tbf_type == (uint8)single_blk_alloc) )
      {
         /*this is an error case , PUA should have frequency params      */
         /*ref 4.60 7.1.2.2.1, perfrom access retry ref 7.1.4 paragraph 5*/
         idle_data_ptr->pua_data_result = FREQ_ERROR_ACCESS_RETRY;
      }
      /*rel 99 re-est during ul tbf release procedure*/
      else if((idle_data_ptr->access_granted == two_phase)
         && ( global_data_ptr->idle_mode_ul_tbf_release_action == RE_EST_WAIT_FOR_SB_ALLOC) )
      {
         MSG_GERAN_MED_0_G("MAC error No Freq params in SB alloc");
      }
    break;

    case UL_2P_ASS:
      MSG_GERAN_MED_0_G("MAC Sent MAC_L1_FREQ_CONFIG");
      idle_data_ptr->mac_l1_sig.mac_l1_freq_config.reuse_sb_frq_alloc = TRUE;
      idle_data_ptr->mac_l1_sig.mac_l1_freq_config.message_header.message_set = MS_MAC_L1;
      idle_data_ptr->mac_l1_sig.mac_l1_freq_config.message_header.message_id = (uint8)MAC_L1_FREQ_CONFIG;
      gprs_l1_mac_acc_func(&idle_data_ptr->mac_l1_sig, gas_id);
    break;

    case UL_1P_CONTENTION_RES:
      /* No freq parmas in PUA, use the freq params for the previous PUA.   *
       *  note the content of the dummy control block will not have change so*
       *  only need to send L1 the new Pdtch config                         */
       if(global_data_ptr->tbf_type == (uint8)dynamic_alloc_e )
       {
         /*complete pccf process activities*/
         gmac_util_complete_pcco_process(PCCO_SUCCESS,MAC_INITIATED,GMAC_IDLE,__LINE__,gas_id);

         if (global_data_ptr->mac_action == PMR_EST || global_data_ptr->mac_action == PCCF_EST)
         {
           if(geran_get_nv_recovery_restart_enabled(gas_id))
           {
             MSG_GERAN_ERROR_1_G("MAC abnormal allocation for mac_action %d",global_data_ptr->mac_action);
             MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
             geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
           }
           else
           {
             ERR_GERAN_FATAL_2_G("MAC abnormal allocation for mac_action %d",gas_id, global_data_ptr->mac_action);
           }
         }
         else
         {
           if (gmac_util_get_pccf_state(gas_id) == SENDING_VIA_UL_PACCH)
           {
#ifdef FEATURE_GSM_EDTM
             /*Delete PCCF message if already present in L1 queues*/
              gmac_send_mac_l1_delete_ul_ctrl_msg(PACKET_CELL_CHANGE_FAILURE,gas_id);
#endif/*FEATURE_GSM_EDTM*/
              /*case where pcco interrupted a UL transfer, but pcco fialed and need to send PCCF*/
              MSG_GERAN_MED_0_G( "MAC sent MAC_PH_DATA_REQ with PCCF");
              gmac_util_send_ul_control_msg_handler(PCCF, UL_PRESENT, GRR_CNF_NOT_NEEDED,gas_id);
           }

           if(idle_data_ptr->i_flags.idle_ul_realloc_state == IDLE_REALLOC_REQUESTED)
           {
             /* dont send MR_UL_ALLOC_CNF do this if PH CONNECT IND is rcvd in UL_1P_CONTENTION_RES*/
             MSG_GERAN_MED_0_G("MAC info performing UL re-alloc, wait for con ind before sending MR_UL_ALLOC_CNF ");
             MSG_GERAN_MED_0_G("MAC sent MAC_L1_DYNAMIC_TBF_CONFIG");
             idle_data_ptr->mac_l1_sig.mac_l1_dynamic_tbf_config.message_header.message_set = MS_MAC_L1;
             idle_data_ptr->mac_l1_sig.mac_l1_dynamic_tbf_config.message_header.message_id = (uint8)MAC_L1_DYNAMIC_TBF_CONFIG;
             gprs_l1_mac_acc_func(&idle_data_ptr->mac_l1_sig,gas_id);

             MSG_GERAN_MED_1_G("gmac_local_ul_ts_alloc =%d",global_data_ptr->gmac_local_ul_ts_alloc);
           }
           else
           {
             /*do the old stuff */

             /*note even for un solicotied PUA, we should not have sent */
             /* MR_UL_ALLOC_CNF to RLC until the connected ind was rcvd !!! */
             MSG_GERAN_MED_0_G("MAC info performing unsolicited pua UL re-alloc, wait for con ind before sending MR_UL_ALLOC_CNF ");
             MSG_GERAN_MED_0_G( "MAC sent MAC_L1_DYNAMIC_TBF_CONFIG");
             idle_data_ptr->mac_l1_sig.mac_l1_dynamic_tbf_config.message_header.message_set = MS_MAC_L1;
             idle_data_ptr->mac_l1_sig.mac_l1_dynamic_tbf_config.message_header.message_id = (uint8)MAC_L1_DYNAMIC_TBF_CONFIG;
             gprs_l1_mac_acc_func(&idle_data_ptr->mac_l1_sig,gas_id);

             MSG_GERAN_MED_1_G("gmac_local_ul_ts_alloc =%d",global_data_ptr->gmac_local_ul_ts_alloc);


           }
         }
       }
       else if(global_data_ptr->tbf_type == (uint8)extension_alloc)
       {
         MSG_GERAN_ERROR_0_G("PUA with extension_alloc, not supported ");
         idle_data_ptr->pua_data_result = ASSIGNMENT_DATA_ERROR;
       }
       else
       {

         MSG_GERAN_ERROR_1_G("MAC info Unexpected tbf_type %d ",global_data_ptr->tbf_type);
         idle_data_ptr->pua_data_result = ASSIGNMENT_DATA_ERROR;
       }
     break;

    default:
      if(geran_get_nv_recovery_restart_enabled(gas_id))
      {
        MSG_GERAN_ERROR_1_G("MAC Info, called in wrong IDLE state:0x%x",idle_data_ptr->mac_idle_substate);
        MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
        geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
      }
      else
      {
        ERR_GERAN_FATAL_2_G("MAC Info, called in wrong IDLE state:0x%x",gas_id, idle_data_ptr->mac_idle_substate);
      }
    break;
  }
}


/*===========================================================================
===
===  FUNCTION    gmac_idle_get_idle_state()
===
===  DESCRIPTION
===  This is an access function for external entities to get the mac_idle_substate.
===
===  PARAMS
===
===
===  RETURN VALUE
===  gprs_mac_idle_substates_t
===
===  SIDE EFFECTS
===
===
===========================================================================*/
gprs_mac_idle_substates_t gmac_idle_get_idle_state(gas_id_t gas_id)
{
  gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);

  return idle_data_ptr->mac_idle_substate;
}

/*===========================================================================
===
===  FUNCTION    whats_idle_ul_realloc_state()
===
===  DESCRIPTION
===  This is an access function for external entities to get the
===  status of idle UL reallocation procedure in state 1 phase
===  contention resoltion..
===
===  PARAMS
===
===
===  RETURN VALUE
===  boolean
===
===  SIDE EFFECTS
===
===
===========================================================================*/
boolean idle_ul_realloc_in_progress(gas_id_t gas_id)
{
 boolean status;
 gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);

 if(idle_data_ptr->i_flags.idle_ul_realloc_state == IDLE_REALLOC_NO_ACTION)
 {
  status = FALSE;


  if(idle_data_ptr->i_flags.rcvd_unsolicited_pua_waiting_for_connected_ind == TRUE)
  {
   MSG_GERAN_MED_1_G("GMAC info RCVD unsolicited pua in 1p contention res",
                     idle_data_ptr->i_flags.rcvd_unsolicited_pua_waiting_for_connected_ind);
   status = TRUE;
  }

 }
 else
 {
  status = TRUE;
 }

 return status;
}

/*** EOF: don't remove! ***/
