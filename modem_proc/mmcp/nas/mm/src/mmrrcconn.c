/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmrrcconn.c_v   1.21   18 Jul 2002 11:41:36   vdrapkin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmrrcconn.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/09/01   jca     Began GMM integration.
05/14/01   jca     Added establishment_cause to send_l3_msg_to_rrc().
05/15/01   sbk     Added logging to the send_cmd_to_rrc
06/08/01   jca     Added support for RRC_ABORT_IND and RRC_ABORT_REQ primitives.
06/20/01   sbk     hfn format has changed
08/07/01   ts      Standardized MSG_ERROR and MSG_HIGH usage.
09/18/01   sbk     Intertask primitives are logged and QXDM logging is supported
09/24/01   jca     Fixed bug where RRC_OPEN_SESSION_REQ is generated while still
                   awaiting an RRC_EST_CNF.
11/02/01   sbk     Send sequence number feature is implemented
12/11/01   sbk     Fixed a bug while setting send seq number
12/12/01   sbk     Implemented Intra-Domain Nas Node function
04/24/02   vdr     Added HFN handling
05/15/02   vdr     Avoided HFN processing when the MS is running in GSM mode or
                   activated by the GSM SIM
05/16/02   mks     Now forwarding signaling message log packet to diag
                   while sending the RRC_OPEN_SESSION_REQ to RRC
05/31/02   ks      Starting the T3220 timer when sending the IMSI DETACH
                   message while sending the RRC_OPEN_SESSION_REQ to RRC.
06/05/02   ks      Added support for Dual mode operation.
07/08/02   vdr     Changed mmrrcconn_send_L3_message_to_rrc() in order to route
                   outgoing GMM message to the LLC layer when the MS operates
                   in GSM mode
07/18/02   vdr     Renamed FEATURE_GPRS to be FEATURE_GSM_GPRS compilation flag
07/23/02   ks      Added handling of RRC_STOP_WCDMA_MODE_REQ in send_cmd_to_rrc()
08/13/02   ks      Checking for the domain type before starting TIMER_T3220
08/13/02   mks     Removed references to ciphering key, integrity key & HFN from
                   RRC_EST_REQ in send_rrc_est_req()
08/13/02   mks     Obsoleted RRC_PAGING_CTRL_REQ as the information conveyed by
                   this primitive is now sent to RRC through RRC_SIM_INSERTED_REQ
                   & RRC_SIM_UPDATE_REQ.
10/10/02   jca     Added support for concurrent PS/CS open session requests.
10/10/02   mks     Now sending RRC_ACTIVATION_RSP to support GSM to WCDMA Interrat
                   procedures
11/12/02   jca     Added support for RRC_CLOSE_SESSION_CNF.
11/14/02    cd     Added MM state transition from MM_WAIT_FOR_RR_CONNECTION_LU
                   to MM_LU_INITIATED when a cs connection is already active
                   Added overwriting the FOR bit in the LOCATION_UPDATING_REQUEST
                   msg just before the message is sent
03/07/03   vdr     Added buffering of the GMM outgoing messages during RAT change
04/07/03   cd      Added call to start T3210 during a LU procedure just before the
                   LU_REQ message is sent
04/15/03   cd      Now using default value for CN choice in open_session_req
05/08/03   cd      Added check for PD_MM when processing a LU_REQ msg
06/04/03   ks      Added support for RRC_MODE_CHANGE_REQ for MODE_CHANGE
                   support for UMTS.
06/16/03   jca     Now consistently ERR_FATAL if attempt to allocate memory fails.
08/01/03   jca     No longer starting the T3220 timer when sending the IMSI DETACH
                   message while sending the RRC_OPEN_SESSION_REQ to RRC.  T3220
                   is started on receipt of the RRC_OPEN_SESSION_CNF as per spec.
11/21/03   jca     Corrected setting of routing basis parameter in Intra Domain NAS
                   Node Selector to be as per 3GPP TS 25.331 section 8.1.8.2.
11/25/03   jca     Expanded function coord_rrc_conn_management() to monitor
                   RR connection status primitives.
05/19/04   jca     Reset all RRC connection state variables when RRC_SERVICE_IND
                   indicates loss of service.
06/08/04   jca     Send pending_mm_msg on receipt of RRC_OPEN_SESSION_CNF if
                   pending_gmm_msg_valid == TRUE.
07/07/04   cd      Fixed lint errors
07/19/40   cd      Setting the mm_follow_on_request and mm_held_cnm_service_req.present
                   to false after a CM_SERVICE_REQ msg is sent in a OPEN_SESSION_REQ
10/09/04   abe     Fix for CR48630: Handling of gmm_pending_msg once OPEN_SESSION_CNF
                   is received for PS connection domain.
11/16/04   jca     Added delay_sig_con_release flag in RRC_ABORT_REQ.
11/29/04   sm      Fix for CR51975. Handling of RRC_OPEN_SESSION_CNF
11/16/04   sv      Addition of new primitive RRC_SERVICE_IND_FROM_OTA_MSG
04/21/05   up      Updated send_cmd_to_rrc() for new primitive 
                   RRC_EQ_PLMN_LIST_CHANGE_IND.
05/10/05   up      Updated send_cmd_to_rrc() for new primitive 
                   RRC_NW_SEL_MODE_RESET_IND.
05/23/05   abe     Set the value of the field auth_failed_bar_cells to FALSE in RR(C)_ABORT_REQ
08/27/05   abe     Added support for establishment cause in RRC_OPEN_SESSION_REQ
01/02/06   up      Updated send_cmd_to_rrc() for RRC_BPLMN_SEARCH_ABORT_REQ.
01/17/06   vdr     Now canceling ongoing authentication attempt if RRC/RR connection 
                   or RRC session has been torn down
01/23/07   pk     Send PTMSI in IDNNS parameter in IDT for PS Domain if PTMSI is valid.
                   GMM_UPDATE_STATUS may be invalid.                   
08/12/11   gps     Truncation of MM to RRC message if message size is larger than "MAX_MESSAGE_SIZE" 
                   in "MMRRCCONN_SEND_L3_MESSAGE_TO_RRC"
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stringl/stringl.h>
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "mm_rr.h"
#include "mm_mm_v.h"
#include "mm_v.h"
#include "mmtask.h"
#include "mmtask_int.h"
#include "mm_umts_v.h"
#include "mm_sim.h"
#include "gmm_v.h"
#include "err.h"
#include "rrcmmif.h"
#include "gmm_int_v.h"
#include "mm_coord.h"
#include "naslog_v.h"
#include "mmauth.h"
#include "gmm_msg.h"

#include "mmsecurity.h"
#include "gmmsecurity.h"
#include "dsumtsps.h"
#include "sm_v.h"

#ifdef FEATURE_GSM_GPRS
#include "gmmllcm.h"
#endif
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif

#ifdef FEATURE_LTE
#include "mmtask_int.h"
#include "lte_nas_common_v.h"
#endif
#ifdef FEATURE_TDSCDMA
#include "tdsrrccmd.h"
#endif /* FEATURE_TDSCDMA */
#ifdef FEATURE_SEGMENT_LOADING
#include "mcpm_api.h"
#include "mcfg_seg_load.h"
#include "ITDSCDMA.h"
#include "IWCDMA.h"
#include "IWCDMA_ext.h"
#endif
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

mmrrc_connection_status_T rrc_connection_status_sim[MAX_NAS_STACKS];
mmrrc_connection_status_T cs_session_status_sim[MAX_NAS_STACKS];
mmrrc_connection_status_T ps_session_status_sim[MAX_NAS_STACKS];

boolean pending_mm_msg_after_release_sim[MAX_NAS_STACKS];
boolean pending_gmm_msg_after_release_sim[MAX_NAS_STACKS];

boolean mm_cs_domain_possible_reest_sim[MAX_NAS_STACKS];
boolean mm_ps_domain_possible_reest_sim[MAX_NAS_STACKS];

extern byte mm_last_paging_cause_sim[MAX_NAS_STACKS];
#define  mm_last_paging_cause mm_last_paging_cause_sim[mm_as_id]
/*
 * The follwoing cache can be dynamically
 * allocated to save memory
 */
#ifdef FEATURE_MODEM_HEAP
byte* pending_gmm_msg_sim[MAX_NAS_STACKS];
#else
byte pending_gmm_msg_sim[MAX_NAS_STACKS][MAX_MESSAGE_SIZE];
#endif

boolean pending_gmm_msg_valid_sim[MAX_NAS_STACKS] ;
rrc_establish_cause_e_type pending_gmm_msg_cause_sim[MAX_NAS_STACKS];
#ifdef FEATURE_MODEM_HEAP
byte* gmm_rat_chng_pended_msg_sim[MAX_NAS_STACKS];
#else
byte gmm_rat_chng_pended_msg_sim[MAX_NAS_STACKS][MAX_MESSAGE_SIZE];
#endif

boolean gmm_rat_chng_pended_msg_valid_sim[MAX_NAS_STACKS];
rrc_establish_cause_e_type gmm_rat_chng_pended_msg_cause_sim[MAX_NAS_STACKS] ;
#ifdef FEATURE_MODEM_HEAP
byte* pending_mm_msg_sim[MAX_NAS_STACKS];
#else
byte pending_mm_msg_sim[MAX_NAS_STACKS][MAX_MESSAGE_SIZE];
#endif
boolean pending_mm_msg_valid_sim[MAX_NAS_STACKS];
rrc_establish_cause_e_type pending_mm_msg_cause_sim[MAX_NAS_STACKS];

byte send_sequence_number_mm_cc_ss_sim[MAX_NAS_STACKS];

extern boolean  data_is_available_sim[MAX_NAS_STACKS] ;
#define  data_is_available data_is_available_sim[mm_as_id]

#else

mmrrc_connection_status_T rrc_connection_status;
mmrrc_connection_status_T cs_session_status;
mmrrc_connection_status_T ps_session_status;

boolean pending_mm_msg_after_release;
boolean pending_gmm_msg_after_release;

boolean mm_cs_domain_possible_reest;
boolean mm_ps_domain_possible_reest;

extern byte mm_last_paging_cause;
/*
 * The follwoing cache can be dynamically
 * allocated to save memory
 */
#ifdef FEATURE_MODEM_HEAP
byte *pending_gmm_msg;
#else
byte pending_gmm_msg[MAX_MESSAGE_SIZE];
#endif
boolean pending_gmm_msg_valid;
rrc_establish_cause_e_type pending_gmm_msg_cause;

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#ifdef FEATURE_MODEM_HEAP
byte *gmm_rat_chng_pended_msg;
#else
byte gmm_rat_chng_pended_msg[MAX_MESSAGE_SIZE];
#endif
boolean gmm_rat_chng_pended_msg_valid;
rrc_establish_cause_e_type gmm_rat_chng_pended_msg_cause ;
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/


#ifdef FEATURE_MODEM_HEAP
byte *pending_mm_msg;
#else
byte pending_mm_msg[MAX_MESSAGE_SIZE];
#endif
boolean pending_mm_msg_valid;
rrc_establish_cause_e_type pending_mm_msg_cause;

byte send_sequence_number_mm_cc_ss;

extern boolean  data_is_available;

#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/

//#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE))
// #include <tf_stub.h>
// #include "rrccmd.h"
//
// TF_PROTO_STUB(rrc_cmd_type *,rrc_get_mm_cmd_buf, void );
// #define rrc_get_mm_cmd_buf TF_STUB(rrc_get_mm_cmd_buf)
//
// TF_PROTO_STUB(void, rrc_put_mm_cmd, rrc_cmd_type *);
// #define rrc_put_mm_cmd TF_STUB(rrc_put_mm_cmd)
//
//#endif // TEST_FRAMEWORK

void mm_fill_intra_domain_nas_node_selector(rrc_cmd_type *rrc_cmd);

extern void mm_log_mm_msg_info(byte pd,byte msg_id);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION MM_TAG_RRC_AS_ID

DESCRIPTION
  This function adds the SIM id context to all MM-> RRC primitives 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_tag_rrc_as_id (rrc_cmd_type *cmd)
{  
  switch (cmd->cmd_hdr.cmd_id)
  {
    case RRC_CLOSE_SESSION_REQ:
      cmd->cmd.close_sess_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      break;
    case RRC_EST_REQ:
      cmd->cmd.rrc_est_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      break;
    case RRC_OPEN_SESSION_REQ:
      cmd->cmd.open_sess_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      mm_log_mm_msg_info(cmd->cmd.open_sess_req.nas_msg.value[0],
                         cmd->cmd.open_sess_req.nas_msg.value[1]);      
      break;
    case RRC_ABORT_REQ:
      cmd->cmd.rrc_abort_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      break;
    case RRC_SERVICE_REQ:
      cmd->cmd.rrc_service_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      break;
    case RRC_DATA_REQ:
      cmd->cmd.data_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      mm_log_mm_msg_info(cmd->cmd.data_req.nas_msg.value[0],
                         cmd->cmd.data_req.nas_msg.value[1]);
      break;
    case RRC_PLMN_LIST_REQ:
      cmd->cmd.plmn_list_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      break;
    case RRC_ACT_REQ:/*NO SIM ID*/
      break;
    case RRC_DEACT_REQ:
      cmd->cmd.deact_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      mm_msg_info = (byte)(cmd->cmd.deact_req.deact_reason);
      break;
    case RRC_STOP_WCDMA_MODE_REQ:
      cmd->cmd.stop_wcdma_mode_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      mm_msg_info = (byte)(cmd->cmd.stop_wcdma_mode_req.cause);
      break;
    case RRC_FORBIDDEN_LAI_LIST_UPDATE_REQ:
      cmd->cmd.forbid_lai_list_update_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      break;
    case RRC_INVALIDATE_SIM_DATA_REQ:/*NO SIM ID*/
      break;
    case RRC_SIM_INSERTED_REQ:
      cmd->cmd.rrc_sim_inserted_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      break;
    case RRC_SIM_UPDATE_REQ:
      cmd->cmd.rrc_sim_update_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      break;
    case RRC_ACTIVATION_RSP:
      cmd->cmd.activation_rsp.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      break;
    case RRC_CHANGE_MODE_IND:
      cmd->cmd.rrc_change_mode_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      break;
    case RRC_MODE_CHANGE_REQ:
      cmd->cmd.deact_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      break;
    case RRC_NW_SEL_MODE_RESET_IND:
      cmd->cmd.rrc_nw_sel_mode_reset_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      break;
#ifdef FEATURE_OOSC_USER_ACTION
    case RRC_EXIT_FROM_PWR_SAVE:
#endif
    case RRC_EQ_PLMN_LIST_CHANGE_IND:
    case RRC_BPLMN_SEARCH_ABORT_REQ:
    case RRC_MODE_PREF_CHANGE_IND:
      cmd->cmd.rrc_subscription_id.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      break;
    case RRC_DS_STATUS_CHANGE_IND:/*NO SIM ID*/
      break;
#ifdef FEATURE_DUAL_SIM
    case RRC_DS_TUNEAWAY_STATUS_CHANGE_IND:/*NO SIM ID*/
      break;
    case RRC_MULTIMODE_SUBS_CHGD_IND:
      break;
#endif
    case RRC_FAST_DORMANCY_REQ:
      cmd->cmd.rrc_fast_dormancy_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      break;
#ifdef FEATURE_NAS_FREE_FLOAT
   case RRC_LAI_REJECT_LIST_IND:
     cmd->cmd.rrc_lai_reject_list_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
     break;

   case RRC_TUNEAWAY_ALLOWED_IND:
     cmd->cmd.rrc_sm_tuneaway_allowed_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
     break;
#endif
#ifdef FEATURE_FEMTO_CSG
   case RRC_CSG_WHITE_LIST_UPDATE_IND:
     cmd->cmd.white_list.as_id = (sys_modem_as_id_e_type)mm_sub_id;
     break;
#endif 

#if defined FEATURE_SGLTE || defined FEATURE_1XSRLTE || defined FEATURE_SVLTE_DUAL_SIM 
    case RRC_UE_MODE_IND :
      cmd->cmd.rrc_ue_mode_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
    break;
#endif 
#if (defined(FEATURE_DUAL_SIM) && defined(FEATURE_LTE))
#ifdef FEATURE_3GPP_CSFB
    case RRC_CSFB_CALL_STATUS_IND:
      cmd->cmd.rrc_csfb_call_status_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      break;
#endif
    case RRC_LTE_RESELECTION_ALLOWED_STATUS_IND:
      cmd->cmd.rrc_lte_resel_allowed_status_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      break;
    case RRC_SET_DRX_IND:
      cmd->cmd.rrc_set_drx_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
      break;
#endif
   case RRC_PSEUDO_LTE_LIST_UPDATE_IND:
     cmd->cmd.rrc_psuedo_lte_list_update_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
     break;

    default:
      MSG_FATAL_DS(MM_SUB, "=MM= MM sent unknown msg %x to RRC", (int)cmd->cmd_hdr.cmd_id, 0,0 );
  }
}
#endif


/*===========================================================================

FUNCTION SEND_CMD_TO_UMTS_RRC

DESCRIPTION
  This function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_cmd_to_umts_rrc( rrc_cmd_type *cmd )
{
#ifdef FEATURE_SEGMENT_LOADING   
   interface_t* ptr_W = get_wcdma_interface();

   if (ptr_W == NULL)
   {
       MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Wrong segment", 0,0,0 );
   }
#endif


   switch (cmd->cmd_hdr.cmd_id)
   {
      case RRC_CLOSE_SESSION_REQ:
         if (rrc_connection_status == MMRRC_EST_IN_PROGRESS)
         {
            /* ---------------------------------------------------------
            ** If we have not yet established an RRC connection when the
            ** RRC_CLOSE_SESSION_REQ command is received, then override
            ** the command with an RRC_ABORT_REQ command
            ** --------------------------------------------------------- */
            cmd->cmd_hdr.cmd_id = RRC_ABORT_REQ;
            cmd->cmd.rrc_abort_req.delay_sig_con_release = TRUE;
            cmd->cmd.rrc_abort_req.auth_failed_bar_cells = FALSE;
            cmd->cmd.rrc_abort_req.hard_abort = FALSE;            
            cmd->cmd.rrc_abort_req.abort_cause = RRC_OTHERS;
            MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_ABORT_REQ to RRC");
            mm_waiting_for_abort_cnf= TRUE;
            rrc_connection_status = MMRRC_RELEASE_IN_PROGRESS;
            cs_session_status = MMRRC_IDLE;
            ps_session_status = MMRRC_IDLE;
            pending_gmm_msg_valid = FALSE;
            pending_mm_msg_valid = FALSE;
            pending_mm_msg_after_release = FALSE;
            pending_gmm_msg_after_release = FALSE;
            mm_cs_domain_possible_reest = FALSE;
            mm_ps_domain_possible_reest = FALSE; 
            gmm_controls_resel_dec_mt_ps_call = FALSE;	    
            
#ifdef FEATURE_MODEM_HEAP
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
             if(pending_mm_msg != NULL)
             {
               modem_mem_free(pending_mm_msg,MODEM_MEM_CLIENT_NAS);
               pending_mm_msg = NULL;
             }
             if(pending_gmm_msg != NULL)
             {
               modem_mem_free(pending_gmm_msg,MODEM_MEM_CLIENT_NAS);
               pending_gmm_msg = NULL;
             }
#endif
#endif
            /******************************************************
            *Stop the timer T3340.Other wise when Ps signalling 
            *message has to be sent, there can be a chance that,UE
            *will not send Ps signalling message until T3340 expiry
            ******************************************************/
            mm_stop_timer(TIMER_T3340);
            mm_stop_timer(TIMER_PS_CONN_REL);

         }
         else if(((cmd->cmd.close_sess_req.cn_domain_id == RRC_CS_DOMAIN_CN_ID)&&
                  (cs_session_status == MMRRC_RELEASE_IN_PROGRESS))||
                 ((cmd->cmd.close_sess_req.cn_domain_id == RRC_PS_DOMAIN_CN_ID) && 
                  (ps_session_status == MMRRC_RELEASE_IN_PROGRESS)))
         {
           MSG_HIGH_DS_0(MM_SUB,"=MM= Blocking RRC_CLOSE_SESSION_REQ as release already in progress");
           if (cmd->cmd.close_sess_req.cn_domain_id == RRC_CS_DOMAIN_CN_ID)
           {
               mm_cs_domain_possible_reest = FALSE;
               pending_mm_msg_valid = FALSE;
#ifdef FEATURE_MODEM_HEAP
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
               if(pending_mm_msg != NULL)
               {
                 modem_mem_free(pending_mm_msg,MODEM_MEM_CLIENT_NAS);
                 pending_mm_msg = NULL;
               }
#endif
#endif
           }
           if (cmd->cmd.close_sess_req.cn_domain_id == RRC_PS_DOMAIN_CN_ID)
           {
                mm_ps_domain_possible_reest = FALSE;            
                pending_gmm_msg_valid = FALSE;
#ifdef FEATURE_MODEM_HEAP
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
                if(pending_gmm_msg != NULL)
                {
                  modem_mem_free(pending_gmm_msg,MODEM_MEM_CLIENT_NAS);
                  pending_gmm_msg = NULL;
                }
#endif
#endif
           }

#ifdef FEATURE_MODEM_HEAP
           modem_mem_free(cmd,MODEM_MEM_CLIENT_NAS);
#else
           gs_free(cmd);
#endif
           return;
     }
         else
         {
            MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_CLOSE_SESSION_REQ to RRC");

            if (cmd->cmd.close_sess_req.cn_domain_id == RRC_CS_DOMAIN_CN_ID)
            {
               cs_session_status = MMRRC_RELEASE_IN_PROGRESS;;
               pending_mm_msg_valid = FALSE;
               mm_cs_domain_possible_reest = FALSE;        
            }
            else
            {
               gmm_controls_resel_dec_mt_ps_call = FALSE;
               ps_session_status = MMRRC_RELEASE_IN_PROGRESS;
               pending_gmm_msg_valid = FALSE;
               mm_ps_domain_possible_reest = FALSE;
               /******************************************************
               *Stop the timer T3340.Other wise when Ps signalling 
               *message has to be sent, there can be a chance that,UE
               *will not send Ps signalling message until T3340 expiry
               ******************************************************/
               mm_stop_timer(TIMER_T3340);
               mm_stop_timer(TIMER_PS_CONN_REL);

            }
         }
         break;

      case RRC_FAST_DORMANCY_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_FAST_DORMANCY_REQ to RRC");
         break;

      case RRC_EST_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_EST_REQ to RRC");
         rrc_connection_status = MMRRC_EST_IN_PROGRESS;
         break;

      case RRC_OPEN_SESSION_REQ:
         mm_print_outgoing_ota_message(
            cmd->cmd.open_sess_req.nas_msg.value[0],
            cmd->cmd.open_sess_req.nas_msg.value[1]);

         /*
         ** If the message type is a LOCATION_UPDATE_REQUEST,  overwrite the FOR bit
         ** This is an optimization in case the FOR has been set to TRUE, and the LU_REQUEST
         ** message is not sent yet
         */
         if ((cmd->cmd.open_sess_req.nas_msg.value[1] == LOCATION_UPDATING_REQUEST) &&
             (cmd->cmd.open_sess_req.nas_msg.value[0] == PD_MM))
         {
           if (mm_follow_on_request == TRUE)
           {
             cmd->cmd.open_sess_req.nas_msg.value[2] |= 0x08;
             MSG_HIGH_DS_0(MM_SUB, "=MM= FOR is 1 in LOCATION_UPDATING_REQUEST");
           }
           else
           {
             cmd->cmd.open_sess_req.nas_msg.value[2] &= 0xF7;
           }
           mm_start_timer( TIMER_T3210, DEFAULT_TIMEOUT );
         }
         else if((cmd->cmd.open_sess_req.nas_msg.value[1] == CM_SERVICE_REQUEST) &&
             (cmd->cmd.open_sess_req.nas_msg.value[0] == PD_MM))
         {
           mm_held_cnm_service_req.present = FALSE;
           mm_follow_on_request = FALSE;

           if (cmd->cmd.open_sess_req.est_cause == RRC_EST_LOW_PRI_SIGNALLING)
           {
             mm_start_timer( TIMER_T3230, DEFAULT_TIMEOUT );
           }
#ifdef FEATURE_NAS_ECALL
           if(mm_call_est_cause == NO_CNM_SERVICE)
           {
             mm_call_est_cause = (cmd->cmd.open_sess_req.nas_msg.value[2]) & 0x0F;
           }
           if (mm_call_est_cause == EMERGENCY_CALL_ESTABLISHMENT)
           {
             mm_stop_timer(TIMER_T3242);
             mm_stop_timer(TIMER_T3243);
           }
#endif
         }

         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_OPEN_SESSION_REQ to RRC");

         if (cmd->cmd.open_sess_req.cn_domain_id == RRC_CS_DOMAIN_CN_ID)
         {

            byte pd = 0;

            /* Reset the send sequence number for the next run */
            send_sequence_number_mm_cc_ss = 0;

            /* PD is the lower nibble */
            pd = cmd->cmd.open_sess_req.nas_msg.value[0] & 0x0F;

            /* Send seq num is only applicable to MM, CC, SS msgs */
            if( pd == PD_MM || pd == PD_CC || pd == PD_SS)
            {
               /*
                * The send seq number in a message type is 0 until it is changed
                * No need to update here as the send seq number is always 0 on
                * the first MM/CC/SS L3 message to the network
                */

               /* Increment the send sequence number
                * and do modulo 4 arithmetic for the next run
                */
               ++send_sequence_number_mm_cc_ss;
               send_sequence_number_mm_cc_ss = send_sequence_number_mm_cc_ss % 4;
            }
            /* CS session is now in Establishment phase */
            cs_session_status = MMRRC_EST_IN_PROGRESS;
         }
         else
         {
           if (!data_is_available)
           {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS) 
             data_is_available = dsumtsps_is_ps_data_available();
#endif
           }
           if (data_is_available)
           {
             MSG_HIGH_DS_0(MM_SUB, "=MM= GMM: User data IS available") ;
           }
           else
           {
             MSG_HIGH_DS_0(MM_SUB, "=MM= GMM: User data IS NOT available") ;
           }
           if (cmd->cmd.open_sess_req.nas_msg.value[1] == GMM_ROUTING_AREA_UPDATE_REQUEST &&
                                        (cmd->cmd.open_sess_req.nas_msg.value[0] == PD_GMM))
           {
             if ((is_umts_mode()) && ( 
#ifdef FEATURE_DUAL_SIM
                          sm_is_pdp_activation_pending((sys_modem_as_id_e_type)mm_sub_id) == TRUE ||
#else
                          sm_is_pdp_activation_pending() == TRUE ||
#endif
                          data_is_available || gmm_follow_on_request ||
                          (gmm_service_request_initiator == MS_CC_MM) ||
                          (gmm_extend_connection_due_to_mt_csfb)))
             {
               cmd->cmd.open_sess_req.nas_msg.value[2] |= 0x08;
#ifdef FEATURE_REL6_PS_FOP
               if ( 
#ifdef FEATURE_DUAL_SIM
                    sm_is_pdp_activation_pending((sys_modem_as_id_e_type)mm_sub_id) == TRUE || 
#else
                    sm_is_pdp_activation_pending() == TRUE || 
#endif
                    (gmm_service_request_initiator == MS_CC_MM) )
                  {
                    gmm_ps_fop_req = TRUE;
                  }
#endif             
             }
             else
             {
               cmd->cmd.open_sess_req.nas_msg.value[2] &= 0xF7;
             }
           }
           else if(cmd->cmd.open_sess_req.nas_msg.value[1] == GMM_ATTACH_REQUEST &&
                                        (cmd->cmd.open_sess_req.nas_msg.value[0] == PD_GMM))
           {
             byte attach_type_index = 0;
             if ((is_umts_mode()) && ( 
#ifdef FEATURE_DUAL_SIM
                          sm_is_pdp_activation_pending((sys_modem_as_id_e_type)mm_sub_id) == TRUE ||
#else
                          sm_is_pdp_activation_pending() == TRUE ||
#endif
                          gmm_follow_on_request))
             {
               attach_type_index=cmd->cmd.open_sess_req.nas_msg.value[2]+3;
               cmd->cmd.open_sess_req.nas_msg.value[attach_type_index] |= 0x08;
#ifdef FEATURE_REL6_PS_FOP
               if ( 
#ifdef FEATURE_DUAL_SIM
                    sm_is_pdp_activation_pending((sys_modem_as_id_e_type)mm_sub_id) == TRUE
#else
                    sm_is_pdp_activation_pending() == TRUE
#endif
                  )
                  {
                    gmm_ps_fop_req = TRUE;
                  }
#endif
             }
             else
             {
               attach_type_index = cmd->cmd.open_sess_req.nas_msg.value[2]+3;
               cmd->cmd.open_sess_req.nas_msg.value[attach_type_index] &= 0xF7;
             }
           }
           
           data_is_available = FALSE;
           ps_session_status = MMRRC_EST_IN_PROGRESS;
         }
         mm_send_ota_msg_log_packet (UE_TO_CN_DIRECTION, cmd->cmd.open_sess_req.nas_msg.length,
                                     cmd->cmd.open_sess_req.nas_msg.value);
         break;

      case RRC_ABORT_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_ABORT_REQ to RRC");
         rrc_connection_status = MMRRC_RELEASE_IN_PROGRESS;
         cs_session_status = MMRRC_IDLE;
         ps_session_status = MMRRC_IDLE;
         pending_gmm_msg_valid = FALSE;
         pending_mm_msg_valid = FALSE;
         mm_waiting_for_abort_cnf= TRUE;
         pending_mm_msg_after_release = FALSE;
         pending_gmm_msg_after_release = FALSE;
         mm_cs_domain_possible_reest = FALSE;
         mm_ps_domain_possible_reest = FALSE; 
         gmm_controls_resel_dec_mt_ps_call = FALSE;	 
         
#ifdef FEATURE_MODEM_HEAP
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
          if(pending_mm_msg != NULL)
          {
            modem_mem_free(pending_mm_msg,MODEM_MEM_CLIENT_NAS);
            pending_mm_msg = NULL;
          }
          if(pending_gmm_msg != NULL)
          {
            modem_mem_free(pending_gmm_msg,MODEM_MEM_CLIENT_NAS);
            pending_gmm_msg = NULL;
          }
#endif
#endif
         /******************************************************
         *Stop the timer T3340.Other wise when Ps signalling 
         *message has to be sent, there can be a chance that,UE
         *will not send Ps signalling message until T3340 expiry
         ******************************************************/
         mm_stop_timer(TIMER_T3340);
         mm_stop_timer(TIMER_PS_CONN_REL);
         break;

      case RRC_SERVICE_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_SERVICE_REQ to RRC");
#ifdef FEATURE_LTE
#ifdef TF_UNIT_TEST
         #error code not present
#endif
#endif
         break;

      case RRC_DATA_REQ:
         mm_print_outgoing_ota_message(
            cmd->cmd.data_req.nas_msg.value[0],
            cmd->cmd.data_req.nas_msg.value[1]);

         MSG_HIGH_DS_2(MM_SUB, "=MM= MM sent RRC_DATA_REQ %x-%x (pd/type)",
            cmd->cmd.data_req.nas_msg.value[0], cmd->cmd.data_req.nas_msg.value[1]);

         {
            /* PD is the lower nibble */
            byte pd = cmd->cmd.data_req.nas_msg.value[0] & 0x0F;

            /* Send seq num is only applicable to MM, CC, SS msgs */
            if( pd == PD_MM || pd == PD_CC || pd == PD_SS)
            {
               /*
                * Second byte is the send sequence number + message type
                * The first two bits will carry the send seq number
                */
               cmd->cmd.data_req.nas_msg.value[1] |= (byte)(send_sequence_number_mm_cc_ss << 6);

               /* Increment the send sequence number
                * and do modulo 4 arithmetic for the next run
                */
               ++send_sequence_number_mm_cc_ss;
               send_sequence_number_mm_cc_ss = send_sequence_number_mm_cc_ss % 4;

            }
         }
         mm_send_ota_msg_log_packet(UE_TO_CN_DIRECTION,
                                    cmd->cmd.data_req.nas_msg.length ,
                                    cmd->cmd.data_req.nas_msg.value);

         break;

      case RRC_PLMN_LIST_REQ:
         MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent  RRC_PLMN_LIST_REQ TI(%d) to RRC", cmd->cmd.plmn_list_req.trans_id);
         break;

      case RRC_ACT_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_ACT_REQ to RRC");
         break;

      case RRC_DEACT_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_DEACT_REQ to RRC");
         break;

       case RRC_STOP_WCDMA_MODE_REQ:
           MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent RRC_STOP_WCDMA_MODE_REQUEST reason: %d", cmd->cmd.stop_wcdma_mode_req.cause);
         break;

       case RRC_FORBIDDEN_LAI_LIST_UPDATE_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_FORBIDDEN_LAI_LIST_UPDATE_REQ to RRC");
         break;

       case RRC_INVALIDATE_SIM_DATA_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_INVALIDATE_SIM_DATA_REQ to RRC");
         break;

       case RRC_SIM_INSERTED_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_SIM_INSERTED_REQ to RRC");
         break;

       case RRC_SIM_UPDATE_REQ:
         MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent RRC_SIM_UPDATE_REQ to RRC status:%d", cmd->cmd.rrc_sim_update_req.security_update_info.security_update_status);
         break;

       case RRC_ACTIVATION_RSP:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_ACTIVATION_RSP to RRC");
         break;

       case RRC_CHANGE_MODE_IND:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_CHANGE_MODE_IND to RRC");
         break;

       case RRC_MODE_CHANGE_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_MODE_CHANGE_REQ to RRC");
         break;

       case RRC_EQ_PLMN_LIST_CHANGE_IND:
         MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent RRC_EQ_PLMN_LIST_CHANGE_IND to RRC");
         break;

       case RRC_NW_SEL_MODE_RESET_IND:
         MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent RRC_NW_SEL_MODE_RESET_IND to RRC [%d]", cmd->cmd.rrc_nw_sel_mode_reset_ind.network_select_mode);
         break;

       case RRC_BPLMN_SEARCH_ABORT_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_BPLMN_SEARCH_ABORT_REQ to RRC");
         break;

#ifdef FEATURE_OOSC_USER_ACTION
       case RRC_EXIT_FROM_PWR_SAVE:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_EXIT_FROM_PWR_SAVE to RRC");
         break;
#endif

#ifdef FEATURE_DUAL_SIM
       case RRC_MODE_PREF_CHANGE_IND:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_MODE_PREF_CHANGE_IND to RRC");
         break;

       case RRC_DS_STATUS_CHANGE_IND:
         MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent RRC_DS_STATUS_CHANGE_IND to RRC: %d",
           cmd->cmd.rrc_ds_status_change_ind.dual_standby_mode);
         break;
#endif

#if defined(FEATURE_DUAL_SIM)
       case RRC_DS_TUNEAWAY_STATUS_CHANGE_IND:
         MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent RRC_DS_TUNEAWAY_STATUS_CHANGE_IND to RRC: %d",
           cmd->cmd.rrc_ds_tuneaway_status_change_ind.tuneaway_status);
         break;
       case RRC_MULTIMODE_SUBS_CHGD_IND:
          MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_MULTIMODE_SUBS_CHGD_IND to RRC: 0");
         break;
#endif

#ifdef FEATURE_LTE
       case RRC_CSFB_CALL_STATUS_IND:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_CSFB_CALL_STATUS_IND to RRC");
         break;

       case RRC_LTE_RESELECTION_ALLOWED_STATUS_IND:
          MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_LTE_RESELECTION_ALLOWED_STATUS_IND to RRC"); 
          break;

       case RRC_SET_DRX_IND:
          MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent RRC_SET_DRX_IND to RRC DRX = %d", cmd->cmd.rrc_set_drx_ind.drx_coefficient); 
          break;
#endif /*FEATURE_LTE*/
#ifdef FEATURE_NAS_FREE_FLOAT
       case RRC_LAI_REJECT_LIST_IND:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_LAI_REJECT_LIST_IND to RRC");
         break;

       case RRC_TUNEAWAY_ALLOWED_IND:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_TUNEAWAY_ALLOWED_IND to RRC");
         break;
#endif

#ifdef FEATURE_FEMTO_CSG
       case RRC_CSG_WHITE_LIST_UPDATE_IND:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent RRC_CSG_WHITE_LIST_UPDATE_IND to RRC");
         break;
#endif 

#if defined FEATURE_SGLTE || defined FEATURE_1XSRLTE || defined FEATURE_SVLTE_DUAL_SIM
       case RRC_UE_MODE_IND :
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent  RRC_UE_MODE_CHANGE_IND to RRC"); 
         break;
#endif 

      case RRC_PSEUDO_LTE_LIST_UPDATE_IND:
        MSG_HIGH_DS_0(MM_SUB ,"=MM= MM sent RRC_PSEUDO_LTE_LIST_UPDATE_IND to RRC "); 
        break;


      default:
         MSG_FATAL_DS(MM_SUB, "=MM= MM sent unknown msg %x to RRC", (int)cmd->cmd_hdr.cmd_id, 0,0 );
   } /* end switch */
#ifdef FEATURE_DUAL_SIM
   mm_tag_rrc_as_id(cmd);
#endif
/* Add sent messages to the debug buffer */
  mm_add_message_to_debug_buffer(MS_MM_RRC,
                                           (dword)(cmd->cmd_hdr.cmd_id),
                                           FROM_MM
                                           ,(sys_modem_as_id_e_type)mm_as_id
                                           );

#ifndef FEATURE_SEGMENT_LOADING
   rrc_put_mm_cmd( cmd );
#else
   IWCDMA_rrc_put_mm_cmd( ptr_W, cmd );
#endif
} /* end send_cmd_to_umts_rrc() */

/*===========================================================================

FUNCTION SEND_CMD_TO_TDSCDMA_RRC

DESCRIPTION
  This function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_cmd_to_tdscdma_rrc( rrc_cmd_type *cmd )
{
#ifdef FEATURE_TDSCDMA
   tdsrrc_mm_cmd_type *tdsrrc_cmd = NULL;
#ifdef FEATURE_SEGMENT_LOADING   
   interface_t* ptr_T = get_tdscdma_interface();

   if (ptr_T == NULL)
   {
       MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Wrong segment", 0,0,0 );
   }

   tdsrrc_cmd = (tdsrrc_mm_cmd_type *) ITDSCDMA_tdsrrc_get_mm_cmd_buf(ptr_T);
#else
   tdsrrc_cmd = (tdsrrc_mm_cmd_type *) tdsrrc_get_mm_cmd_buf();
#endif
   if (tdsrrc_cmd == NULL)
   {
      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
   }

     /* Add sent messages to the debug buffer */
    mm_add_message_to_debug_buffer( MS_MM_TDSRRC,
                                    (dword)(cmd->cmd_hdr.cmd_id),
                                    FROM_MM
                                    ,(sys_modem_as_id_e_type)mm_as_id
                                                );

#ifdef FEATURE_DUAL_SIM
   mm_tag_rrc_as_id( cmd );
#endif

   switch (cmd->cmd_hdr.cmd_id)
   {
      case RRC_CLOSE_SESSION_REQ:
         if (rrc_connection_status == MMRRC_EST_IN_PROGRESS)
         {
            /* ---------------------------------------------------------
            ** If we have not yet established an RRC connection when the
            ** RRC_CLOSE_SESSION_REQ command is received, then override
            ** the command with an RRC_ABORT_REQ command
            ** --------------------------------------------------------- */
            tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_ABORT_REQ;
            tdsrrc_cmd->cmd.rrc_abort_req.delay_sig_con_release = TRUE;
            tdsrrc_cmd->cmd.rrc_abort_req.auth_failed_bar_cells = FALSE;
            tdsrrc_cmd->cmd.rrc_abort_req.hard_abort = FALSE;            
            tdsrrc_cmd->cmd.rrc_abort_req.abort_cause = RRC_OTHERS;
            tdsrrc_cmd->cmd.rrc_abort_req.as_id = (sys_modem_as_id_e_type)mm_sub_id;
            MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_ABORT_REQ to TDSRRC");
            mm_waiting_for_abort_cnf= TRUE;
            rrc_connection_status = MMRRC_RELEASE_IN_PROGRESS;
            cs_session_status =MMRRC_IDLE;
            ps_session_status =MMRRC_IDLE;
            pending_gmm_msg_valid = FALSE;
            pending_mm_msg_valid = FALSE;
            pending_mm_msg_after_release = FALSE;
            pending_gmm_msg_after_release = FALSE;
            mm_cs_domain_possible_reest = FALSE;
            mm_ps_domain_possible_reest = FALSE;            
#ifdef FEATURE_MODEM_HEAP
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
            if(pending_mm_msg != NULL)
            {
              modem_mem_free(pending_mm_msg,MODEM_MEM_CLIENT_NAS);
              pending_mm_msg = NULL;
            }
            if(pending_gmm_msg != NULL)
            {
              modem_mem_free(pending_gmm_msg,MODEM_MEM_CLIENT_NAS);
              pending_gmm_msg = NULL;
            }
#endif
#endif
            /******************************************************
            *Stop the timer T3340.Other wise when Ps signalling 
            *message has to be sent, there can be a chance that,UE
            *will not send Ps signalling message until T3340 expiry
            ******************************************************/
            mm_stop_timer(TIMER_T3340);
            mm_stop_timer(TIMER_PS_CONN_REL);

         }
         else if(((cmd->cmd.close_sess_req.cn_domain_id == RRC_CS_DOMAIN_CN_ID)&&
                  (cs_session_status == MMRRC_RELEASE_IN_PROGRESS))||
                 ((cmd->cmd.close_sess_req.cn_domain_id == RRC_PS_DOMAIN_CN_ID) && 
                  (ps_session_status == MMRRC_RELEASE_IN_PROGRESS)))
         {
           MSG_HIGH_DS_0(MM_SUB,"=MM= Blocking RRC_CLOSE_SESSION_REQ as release already in progress");
           if (cmd->cmd.close_sess_req.cn_domain_id == RRC_CS_DOMAIN_CN_ID)
           {
               mm_cs_domain_possible_reest = FALSE;
               pending_mm_msg_valid = FALSE;
#ifdef FEATURE_MODEM_HEAP
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
               if(pending_mm_msg != NULL)
               {
                 modem_mem_free(pending_mm_msg,MODEM_MEM_CLIENT_NAS);
                 pending_mm_msg = NULL;
               }
#endif
#endif
           }
           if (cmd->cmd.close_sess_req.cn_domain_id == RRC_PS_DOMAIN_CN_ID)
           {
                mm_ps_domain_possible_reest = FALSE;            
                pending_gmm_msg_valid = FALSE;
#ifdef FEATURE_MODEM_HEAP
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
                if(pending_gmm_msg != NULL)
                {
                  modem_mem_free(pending_gmm_msg,MODEM_MEM_CLIENT_NAS);
                  pending_gmm_msg = NULL;
                }
#endif
#endif
           }

#ifdef FEATURE_MODEM_HEAP
           modem_mem_free(cmd,MODEM_MEM_CLIENT_NAS);
           modem_mem_free(tdsrrc_cmd,MODEM_MEM_CLIENT_NAS);
           
#else
           gs_free(cmd);
           gs_free(tdsrrc_cmd);
#endif
           return;
         }
         else
         {
            tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_CLOSE_SESSION_REQ;
            tdsrrc_cmd->cmd.close_sess_req = cmd->cmd.close_sess_req;

            MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_CLOSE_SESSION_REQ");

            if (cmd->cmd.close_sess_req.cn_domain_id == RRC_CS_DOMAIN_CN_ID)
            {
               cs_session_status = MMRRC_RELEASE_IN_PROGRESS;
               pending_mm_msg_valid = FALSE;
               mm_cs_domain_possible_reest = FALSE;        
            }
            else
            {
               ps_session_status = MMRRC_RELEASE_IN_PROGRESS;
               pending_gmm_msg_valid = FALSE;
               mm_ps_domain_possible_reest = FALSE;
               /******************************************************
               *Stop the timer T3340.Other wise when Ps signalling 
               *message has to be sent, there can be a chance that,UE
               *will not send Ps signalling message until T3340 expiry
               ******************************************************/
               mm_stop_timer(TIMER_T3340);
               mm_stop_timer(TIMER_PS_CONN_REL);
            }
         }
         break;

      case RRC_FAST_DORMANCY_REQ:
         MSG_HIGH_DS_0(MM_SUB,"MM sent TDSRRC_FAST_DORMANCY_REQ");
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_FAST_DORMANCY_REQ;
         tdsrrc_cmd->cmd.rrc_fast_dormancy_req = cmd->cmd.rrc_fast_dormancy_req;
         break;

      case RRC_EST_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_EST_REQ");
         rrc_connection_status = MMRRC_EST_IN_PROGRESS;
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_EST_REQ;
         tdsrrc_cmd->cmd.rrc_est_req = cmd->cmd.rrc_est_req;
         break;

      case RRC_OPEN_SESSION_REQ:
        mm_print_outgoing_ota_message(
            cmd->cmd.open_sess_req.nas_msg.value[0],
            cmd->cmd.open_sess_req.nas_msg.value[1]);

         /*
         ** If the message type is a LOCATION_UPDATE_REQUEST,  overwrite the FOR bit
         ** This is an optimization in case the FOR has been set to TRUE, and the LU_REQUEST
         ** message is not sent yet
         */
         if ((cmd->cmd.open_sess_req.nas_msg.value[1] == LOCATION_UPDATING_REQUEST) &&
             (cmd->cmd.open_sess_req.nas_msg.value[0] == PD_MM))
         {
           if (mm_follow_on_request == TRUE)
           {
             cmd->cmd.open_sess_req.nas_msg.value[2] |= 0x08;
             MSG_HIGH_DS_0(MM_SUB, "=MM= FOR is 1 in LOCATION_UPDATING_REQUEST");
           }
           mm_start_timer( TIMER_T3210, DEFAULT_TIMEOUT );
         }
         else if((cmd->cmd.open_sess_req.nas_msg.value[1] == CM_SERVICE_REQUEST) &&
             (cmd->cmd.open_sess_req.nas_msg.value[0] == PD_MM))
         {
           mm_held_cnm_service_req.present = FALSE;
           mm_follow_on_request = FALSE;

           if (cmd->cmd.open_sess_req.est_cause == RRC_EST_LOW_PRI_SIGNALLING)
           {
             mm_start_timer( TIMER_T3230, DEFAULT_TIMEOUT );
           }
#ifdef FEATURE_NAS_ECALL
           if(mm_call_est_cause == NO_CNM_SERVICE)
           {
             mm_call_est_cause = (cmd->cmd.open_sess_req.nas_msg.value[2]) & 0x0F;
           }
           if (mm_call_est_cause == EMERGENCY_CALL_ESTABLISHMENT)
           {
             mm_stop_timer(TIMER_T3242);
             mm_stop_timer(TIMER_T3243);
           }
#endif
         }

         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_OPEN_SESSION_REQ");

         if (cmd->cmd.open_sess_req.cn_domain_id == RRC_CS_DOMAIN_CN_ID)
         {

            byte pd = 0;

            /* Reset the send sequence number for the next run */
            send_sequence_number_mm_cc_ss = 0;

            /* PD is the lower nibble */
            pd = cmd->cmd.open_sess_req.nas_msg.value[0] & 0x0F;

            /* Send seq num is only applicable to MM, CC, SS msgs */
            if( pd == PD_MM || pd == PD_CC || pd == PD_SS)
            {
               /*
                * The send seq number in a message type is 0 until it is changed
                * No need to update here as the send seq number is always 0 on
                * the first MM/CC/SS L3 message to the network
                */

               /* Increment the send sequence number
                * and do modulo 4 arithmetic for the next run
                */
               ++send_sequence_number_mm_cc_ss;
               send_sequence_number_mm_cc_ss = send_sequence_number_mm_cc_ss % 4;
            }

            /* CS session is now in Establishment phase */
            cs_session_status = MMRRC_EST_IN_PROGRESS;
         }
         else
         {
           ps_session_status = MMRRC_EST_IN_PROGRESS;
         }
         mm_send_ota_msg_log_packet (UE_TO_CN_DIRECTION, cmd->cmd.open_sess_req.nas_msg.length,
                                     cmd->cmd.open_sess_req.nas_msg.value);
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_OPEN_SESSION_REQ;
         tdsrrc_cmd->cmd.open_sess_req = cmd->cmd.open_sess_req;
         tdsrrc_cmd->cmd.open_sess_req.nas_msg.value =
#ifndef FEATURE_SEGMENT_LOADING
           tdsrrc_malloc_for_external_cmd( cmd->cmd.open_sess_req.nas_msg.length );
#else
         ITDSCDMA_tdsrrc_malloc_for_external_cmd(ptr_T, cmd->cmd.open_sess_req.nas_msg.length );
#endif
         memscpy( (void *)tdsrrc_cmd->cmd.open_sess_req.nas_msg.value,cmd->cmd.open_sess_req.nas_msg.length,
                 (void *)cmd->cmd.open_sess_req.nas_msg.value,
                 cmd->cmd.open_sess_req.nas_msg.length );
#ifndef FEATURE_MODEM_HEAP
         gs_free( cmd->cmd.open_sess_req.nas_msg.value );
#else
         modem_mem_free(
           cmd->cmd.open_sess_req.nas_msg.value,MODEM_MEM_CLIENT_NAS );
#endif
         break;

      case RRC_ABORT_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_ABORT_REQ");
         rrc_connection_status = MMRRC_RELEASE_IN_PROGRESS;
         cs_session_status = MMRRC_IDLE;
         ps_session_status = MMRRC_IDLE;
         pending_gmm_msg_valid = FALSE;
         pending_mm_msg_valid = FALSE;
         mm_waiting_for_abort_cnf = TRUE;
         pending_mm_msg_after_release = FALSE;
         pending_gmm_msg_after_release = FALSE;
         mm_cs_domain_possible_reest = FALSE;
         mm_ps_domain_possible_reest = FALSE;  
         gmm_controls_resel_dec_mt_ps_call = FALSE;	 
         
#ifdef FEATURE_MODEM_HEAP
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
         if(pending_mm_msg != NULL)
         {
           modem_mem_free(pending_mm_msg,MODEM_MEM_CLIENT_NAS);
           pending_mm_msg = NULL;
         }
         if(pending_gmm_msg != NULL)
         {
           modem_mem_free(pending_gmm_msg,MODEM_MEM_CLIENT_NAS);
           pending_gmm_msg = NULL;
         }
#endif
#endif
         /******************************************************
         *Stop the timer T3340.Other wise when Ps signalling 
         *message has to be sent, there can be a chance that,UE
         *will not send Ps signalling message until T3340 expiry
         ******************************************************/
         mm_stop_timer(TIMER_T3340);
         mm_stop_timer(TIMER_PS_CONN_REL);

         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_ABORT_REQ;
         tdsrrc_cmd->cmd.rrc_abort_req = cmd->cmd.rrc_abort_req;
         break;

      case RRC_SERVICE_REQ:
         MSG_HIGH_DS_0(MM_SUB,"MM sent TDSRRC_SERVICE_REQ");
#ifdef FEATURE_LTE
#ifdef TF_UNIT_TEST
         #error code not present
#endif
#endif
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_SERVICE_REQ;
         tdsrrc_cmd->cmd.rrc_service_req = cmd->cmd.rrc_service_req;
         break;

      case RRC_DATA_REQ:
         mm_print_outgoing_ota_message(
            cmd->cmd.data_req.nas_msg.value[0],
            cmd->cmd.data_req.nas_msg.value[1]);

         MSG_HIGH_DS_2(MM_SUB, "=MM= MM sent TDSRRC_DATA_REQ %x-%x (pd/type)",
            cmd->cmd.data_req.nas_msg.value[0], cmd->cmd.data_req.nas_msg.value[1]);

         {
            /* PD is the lower nibble */
            byte pd = cmd->cmd.data_req.nas_msg.value[0] & 0x0F;

            /* Send seq num is only applicable to MM, CC, SS msgs */
            if( pd == PD_MM || pd == PD_CC || pd == PD_SS)
            {
               /*
                * Second byte is the send sequence number + message type
                * The first two bits will carry the send seq number
                */
               cmd->cmd.data_req.nas_msg.value[1] |= (byte)(send_sequence_number_mm_cc_ss << 6);

               /* Increment the send sequence number
                * and do modulo 4 arithmetic for the next run
                */
               ++send_sequence_number_mm_cc_ss;
               send_sequence_number_mm_cc_ss = send_sequence_number_mm_cc_ss % 4;

            }
         }
         mm_send_ota_msg_log_packet(UE_TO_CN_DIRECTION,
                                    cmd->cmd.data_req.nas_msg.length ,
                                    cmd->cmd.data_req.nas_msg.value);

         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_DATA_REQ;
         tdsrrc_cmd->cmd.data_req = cmd->cmd.data_req;
         tdsrrc_cmd->cmd.data_req.nas_msg.value =
#ifndef FEATURE_SEGMENT_LOADING
           tdsrrc_malloc_for_external_cmd( cmd->cmd.data_req.nas_msg.length );
#else
         ITDSCDMA_tdsrrc_malloc_for_external_cmd(ptr_T, cmd->cmd.data_req.nas_msg.length );
#endif
         memscpy( (void *)tdsrrc_cmd->cmd.data_req.nas_msg.value, cmd->cmd.data_req.nas_msg.length,
                 (void *)cmd->cmd.data_req.nas_msg.value,
                 cmd->cmd.data_req.nas_msg.length );
#ifndef FEATURE_MODEM_HEAP
         gs_free( cmd->cmd.data_req.nas_msg.value );
#else
         modem_mem_free(
           cmd->cmd.data_req.nas_msg.value,MODEM_MEM_CLIENT_NAS );
#endif
         break;

      case RRC_PLMN_LIST_REQ:
         MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent TDSRRC_PLMN_LIST_REQ TI(%d)", cmd->cmd.plmn_list_req.trans_id);
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_PLMN_LIST_REQ;
         tdsrrc_cmd->cmd.plmn_list_req = cmd->cmd.plmn_list_req;
         break;

      case RRC_ACT_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_ACT_REQ");
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_ACT_REQ;
         break;

      case RRC_DEACT_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_DEACT_REQ");
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_DEACT_REQ;
         tdsrrc_cmd->cmd.deact_req = cmd->cmd.deact_req;
         break;

       case RRC_STOP_WCDMA_MODE_REQ:
         MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent RRC_STOP_TDS_MODE_REQ reason: %d", cmd->cmd.stop_wcdma_mode_req.cause);
         tdsrrc_cmd->cmd_hdr.cmd_id = RRC_STOP_TDS_MODE_REQ;
         tdsrrc_cmd->cmd.stop_wcdma_mode_req = cmd->cmd.stop_wcdma_mode_req;
         break;

       case RRC_FORBIDDEN_LAI_LIST_UPDATE_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_FORBIDDEN_LAI_LIST_UPDATE_REQ");
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_FORBIDDEN_LAI_LIST_UPDATE_REQ;
         tdsrrc_cmd->cmd.forbid_lai_list_update_req =
           cmd->cmd.forbid_lai_list_update_req;
         break;

       case RRC_INVALIDATE_SIM_DATA_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_INVALIDATE_SIM_DATA_REQ");
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_INVALIDATE_SIM_DATA_REQ;
         break;

       case RRC_SIM_INSERTED_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_SIM_INSERTED_REQ");
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_SIM_INSERTED_REQ;
         tdsrrc_cmd->cmd.rrc_sim_inserted_req = cmd->cmd.rrc_sim_inserted_req;
         break;

       case RRC_SIM_UPDATE_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_SIM_UPDATE_REQ");
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_SIM_UPDATE_REQ;
         tdsrrc_cmd->cmd.rrc_sim_update_req = cmd->cmd.rrc_sim_update_req;
         break;

       case RRC_ACTIVATION_RSP:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_ACTIVATION_RSP");
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_ACTIVATION_RSP;
         tdsrrc_cmd->cmd.activation_rsp = cmd->cmd.activation_rsp;
         break;

       case RRC_CHANGE_MODE_IND:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_CHANGE_MODE_IND");
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_CHANGE_MODE_IND;
         tdsrrc_cmd->cmd.rrc_change_mode_ind = cmd->cmd.rrc_change_mode_ind;
         break;

       case RRC_MODE_CHANGE_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_MODE_CHANGE_REQ");
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_MODE_CHANGE_REQ;
         tdsrrc_cmd->cmd.deact_req = cmd->cmd.deact_req;
         break;

       case RRC_EQ_PLMN_LIST_CHANGE_IND:
         MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent TDSRRC_EQ_PLMN_LIST_CHANGE_IND");
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_EQ_PLMN_LIST_CHANGE_IND;
         tdsrrc_cmd->cmd.rrc_subscription_id = cmd->cmd.rrc_subscription_id;
         break;

       case RRC_NW_SEL_MODE_RESET_IND:
         MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent TDSRRC_NW_SEL_MODE_RESET_IND [%d]", cmd->cmd.rrc_nw_sel_mode_reset_ind.network_select_mode);
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_NW_SEL_MODE_RESET_IND;
         tdsrrc_cmd->cmd.rrc_nw_sel_mode_reset_ind =
           cmd->cmd.rrc_nw_sel_mode_reset_ind;
         break;

       case RRC_BPLMN_SEARCH_ABORT_REQ:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_BPLMN_SEARCH_ABORT_REQ");
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_BPLMN_SEARCH_ABORT_REQ;
         tdsrrc_cmd->cmd.rrc_subscription_id = cmd->cmd.rrc_subscription_id;
         break;

#ifdef FEATURE_OOSC_USER_ACTION
       case RRC_EXIT_FROM_PWR_SAVE:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_EXIT_FROM_PWR_SAVE");
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_EXIT_FROM_PWR_SAVE;
         tdsrrc_cmd->cmd.rrc_subscription_id = cmd->cmd.rrc_subscription_id;
         break;
#endif

#ifdef FEATURE_DUAL_SIM
       case RRC_DS_STATUS_CHANGE_IND:
         MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent TDSRRC_DS_STATUS_CHANGE_IND: %d",
           cmd->cmd.rrc_ds_status_change_ind.dual_standby_mode);
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_DS_STATUS_CHANGE_IND;
         tdsrrc_cmd->cmd.rrc_ds_status_change_ind = cmd->cmd.rrc_ds_status_change_ind;
         break;
       case RRC_MULTIMODE_SUBS_CHGD_IND:
          MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_MULTIMODE_SUBS_CHGD_IND to TDS-RRC");
          tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_MULTIMODE_SUBS_CHGD_IND;
          tdsrrc_cmd->cmd.rrc_multimode_subs_chgd_ind = cmd->cmd.rrc_multimode_subs_chgd_ind;
         break;
#endif

       case RRC_LAI_REJECT_LIST_IND:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_LAI_REJECT_LIST_IND to RRC");
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_LAI_REJECT_LIST_IND;
         tdsrrc_cmd->cmd.rrc_lai_reject_list_ind = cmd->cmd.rrc_lai_reject_list_ind;
         break;


#ifdef FEATURE_DUAL_SIM
       case RRC_DS_TUNEAWAY_STATUS_CHANGE_IND:
         MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent TDSRRC_DS_TUNEAWAY_STATUS_CHANGE_IND: %d",
           cmd->cmd.rrc_ds_tuneaway_status_change_ind.tuneaway_status);
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_DS_TUNEAWAY_STATUS_CHANGE_IND;
         tdsrrc_cmd->cmd.rrc_ds_tuneaway_status_change_ind = cmd->cmd.rrc_ds_tuneaway_status_change_ind;
         break;

       case RRC_TUNEAWAY_ALLOWED_IND:
         MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent TDSRRC_TUNEAWAY_ALLOWED_IND: %d",
           cmd->cmd.rrc_ds_tuneaway_status_change_ind.tuneaway_status);
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_TUNEAWAY_ALLOWED_IND;
         tdsrrc_cmd->cmd.rrc_sm_tuneaway_allowed_ind = cmd->cmd.rrc_sm_tuneaway_allowed_ind;
         break;
#endif

       case RRC_MODE_PREF_CHANGE_IND:
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_MODE_PREF_CHANGE_IND;
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_MODE_PREF_CHANGE_IND to RRC");
         tdsrrc_cmd->cmd.rrc_subscription_id = cmd->cmd.rrc_subscription_id;
         break;

#ifdef FEATURE_LTE
       case RRC_CSFB_CALL_STATUS_IND:
         MSG_HIGH_DS_0(MM_SUB,"MM sent TDSRRC_CSFB_CALL_STATUS_IND");
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_CSFB_CALL_STATUS_IND;
         tdsrrc_cmd->cmd.rrc_csfb_call_status_ind = cmd->cmd.rrc_csfb_call_status_ind;
         break;

       case RRC_LTE_RESELECTION_ALLOWED_STATUS_IND:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_LTE_RESELECTION_ALLOWED_STATUS_IND to RRC");          
         tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_LTE_RESELECTION_ALLOWED_STATUS_IND;
         tdsrrc_cmd->cmd.rrc_lte_resel_allowed_status_ind =
            cmd->cmd.rrc_lte_resel_allowed_status_ind;
          break;
#endif /*FEATURE_LTE*/

#if defined FEATURE_SGLTE || defined FEATURE_1XSRLTE || defined FEATURE_SVLTE_DUAL_SIM
       case RRC_UE_MODE_IND :
          MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TDSRRC_UE_MODE_CHANGE_IND to RRC"); 
          tdsrrc_cmd->cmd_hdr.cmd_id = TDSRRC_UE_MODE_IND;
          tdsrrc_cmd->cmd.rrc_ue_mode_ind = cmd->cmd.rrc_ue_mode_ind;
          break;
#endif
      default:
         MSG_FATAL_DS(MM_SUB, "=MM= MM sent unknown msg %x to TDSRRC", (int)cmd->cmd_hdr.cmd_id, 0,0 );
   } /* end switch */
#ifndef FEATURE_SEGMENT_LOADING
   tdsrrc_put_mm_cmd( tdsrrc_cmd );
#else
   ITDSCDMA_tdsrrc_put_mm_cmd(ptr_T, tdsrrc_cmd );
#endif
#ifndef FEATURE_MODEM_HEAP
   gs_free( cmd );
#else
   modem_mem_free( cmd, MODEM_MEM_CLIENT_NAS);
#endif
#else
   MSG_ERROR_DS(MM_SUB, "=MM= FEATURE_TDSCDMA not enabled", 0,0,0);
#endif /* FEATURE_TDSCDMA */
} /* end send_cmd_to_tdscdma_rrc() */

/*===========================================================================

FUNCTION SEND_CMD_TO_RRC

DESCRIPTION
  This function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void send_cmd_to_rrc

( 
  rrc_cmd_type                 *cmd,
  sys_radio_access_tech_e_type rat
)
{
#ifdef FEATURE_SEGMENT_LOADING    
    interface_t *ptr_W = get_wcdma_interface();    
    if ((rat == SYS_RAT_UMTS_RADIO_ACCESS) ||
       ((ptr_W != NULL) && (rat != SYS_RAT_TDS_RADIO_ACCESS)))
    {
        send_cmd_to_umts_rrc( cmd );
    }
    else
    {
        send_cmd_to_tdscdma_rrc( cmd );
    }
#else

  if (rat == SYS_RAT_TDS_RADIO_ACCESS)
  {
    send_cmd_to_tdscdma_rrc( cmd );
  }
  /* Note that there are cases where the command will be sent to WRRC
  ** even if the active RAT is GSM or LTE
  */
  else
  {
    send_cmd_to_umts_rrc( cmd );
  }
#endif
}

/*===========================================================================

FUNCTION COORD_RRC_CONN_MANAGEMENT

DESCRIPTION
  This function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
void coord_rrc_conn_management( mm_cmd_type *message )
{
   switch (message->cmd.hdr.message_set)
   {
      case MS_MM_RR:
         switch (message->cmd.hdr.message_id)
         {
            case RR_EST_CNF:
               rrc_connection_status = MMRRC_ACTIVE;
               MSG_HIGH_DS_0(MM_SUB, "=MM= RR Connection Established");
#ifdef FEATURE_NAS_REL11			   
               if((mm_state == MM_WAIT_FOR_NETWORK_COMMAND) && (mm_last_paging_cause == (byte)CS_PAGING))
               {
                 mm_start_timer(TIMER_T3240, DEFAULT_TIMEOUT); 
               }
#endif  
               break;

            case RR_REL_IND:
            case RR_ABORT_IND:
            case RR_ABORT_CNF:
              if((message->cmd.hdr.message_id == RR_ABORT_IND) &&
                 (mm_per_nas_stacks_is_mt_page_in_progress((sys_modem_as_id_e_type)mm_as_id)) &&
                  (rrc_connection_status == MMRRC_EST_IN_PROGRESS))
               {
                 mm_send_cm_mt_call_fail_ind(CM_MM_MT_CALL_FAIL_CAUSE_CONNECTION_EST_FAILURE);
               }
#ifdef FEATURE_SGLTE
               /*in connected state, SGLTE_SRVCC abort is not expected 
                 we may receive due to cross over with ACT_IND srvcc */
               if((rrc_connection_status != MMRRC_ACTIVE ) 
                  ||
                  ((message->cmd.hdr.message_id != RR_ABORT_IND) ||
                   (message->cmd.rr_abort_ind.RR_abort_cause != OTHER_REASON) ||
                   (message->cmd.rr_abort_ind.failure_cause  != MM_RR_SGLTE_SRVCC_CS_HANDOVER)))
#endif 
               {
               MSG_HIGH_DS_0( MM_SUB, "=MM= RR Connection Released");
#ifdef FEATURE_LTE
               mm_csfb_is_blind_redir_to_lte_req_to_be_sent = FALSE;
#endif
               
               rrc_connection_status = MMRRC_IDLE;

               /* Cancel obsolete CS authentication attempts */
               mm_cancel_authentication() ;
               }
               break;

            default:
               break;
         } /* end switch (message->cmd.hdr.message_id) */
         break;

      case MS_MM_RRC:
      case MS_MM_TDSRRC:
         switch (message->cmd.hdr.message_id)
         {
            case RRC_EST_CNF:
               if (rrc_connection_status == MMRRC_EST_IN_PROGRESS)
               {
                  rrc_connection_status = MMRRC_ACTIVE;
                  gmm_cause_directed_signalling_reest = FALSE;

                  if (is_umts_mode() && (mm_sim_card_mode == MMGSDI_APP_USIM))
                  {
#ifndef FEATURE_NAS_REL11
/*24.008: 4.1.1.8	Handling of security related parameters at switch on and switch off*/
                    /* Invalidate CS and PS HFNs */
                    auth_delete_hfns() ;
#endif
                  }

                  MSG_HIGH_DS_0(MM_SUB,"=MM= RRC Connection Established");
#ifdef FEATURE_NAS_REL11
                  if ((mm_state == MM_WAIT_FOR_NETWORK_COMMAND) && (mm_last_paging_cause != 0xFF))
                  {
                    mm_start_timer(TIMER_T3240, DEFAULT_TIMEOUT); 
                  }
#endif
                  if (pending_mm_msg_valid)
                  {
                     send_open_session_req( RRC_CS_DOMAIN_CN_ID );
                  }

                  if (pending_gmm_msg_valid)
                  {
                     send_open_session_req( RRC_PS_DOMAIN_CN_ID );
                  }
               }
               break;

            case RRC_OPEN_SESSION_CNF:
               if (message->cmd.rrc_open_session_cnf.cn_domain_id == RRC_CS_DOMAIN_CN_ID)
               {

                  if (message->cmd.rrc_open_session_cnf.status == RRC_OPEN_SESS_SUCCESS)
                  {
                     cs_session_status = MMRRC_ACTIVE;
                     MSG_HIGH_DS_0(MM_SUB,"=MM= CS Session Active");

#ifdef FEATURE_NAS_REL11
                     if ((mm_state == MM_WAIT_FOR_NETWORK_COMMAND) && (mm_last_paging_cause != 0xFF)
                            && (gmm_anite_gcf_flag == FALSE))
                     {
                       mm_start_timer(TIMER_T3240, DEFAULT_TIMEOUT); 
                     }
#endif
                    if (pending_mm_msg_valid)
                    {
  #ifdef FEATURE_MODEM_HEAP
                      if(pending_mm_msg == NULL)
                      {
                        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Null pointer for pending_mm_msg", 0,0,0 );
                      }
  #endif
                      mmrrcconn_send_L3_message_to_rrc(
                        pending_mm_msg, pending_mm_msg_cause, RRC_CS_DOMAIN_CN_ID );
                      pending_mm_msg_valid = FALSE;
                    }

                  }
                  else
                  {
                     cs_session_status = MMRRC_IDLE;
                     /* Cancel obsolete CS authentication attempts */
                     mm_cancel_authentication() ;
                     MSG_HIGH_DS_0(MM_SUB,"Session Activation Failed");
                     /* reset retransmission variables*/
                     mm_cs_domain_possible_reest = FALSE;
                     pending_mm_msg_valid        = FALSE;
                     pending_mm_msg_after_release = FALSE; 


                  }

                  if ((pending_gmm_msg_valid) && (ps_session_status != MMRRC_ACTIVE))
                  {
                    if (ps_session_status != MMRRC_EST_IN_PROGRESS)
                    {
                       MSG_HIGH_DS_0(MM_SUB, "=MM= PS domain is sending RRC_OPENSESSION_REQ");
                       send_open_session_req( RRC_PS_DOMAIN_CN_ID );
                    }
                    else 
                    {
                       MSG_HIGH_DS_0(MM_SUB, "=MM= PS domain has sent RRC_OPEN_SESSION_REQ and not received RRC_OPEN_SESSION_CNF");
                    }
                  }
               }
               else if (message->cmd.rrc_open_session_cnf.cn_domain_id == RRC_PS_DOMAIN_CN_ID)
               {
                  if (message->cmd.rrc_open_session_cnf.status == RRC_OPEN_SESS_SUCCESS)
                  {
                     ps_session_status = MMRRC_ACTIVE;
                     MSG_HIGH_DS_0(MM_SUB,"=MM= PS Session Active");

                    if (pending_gmm_msg_valid)
                    {
  #ifdef FEATURE_MODEM_HEAP
                      if(pending_gmm_msg == NULL)
                      {
                        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Null pointer for pending_gmm_msg", 0,0,0 );
                      }
  #endif
                      mmrrcconn_send_L3_message_to_rrc(
                        pending_gmm_msg, pending_gmm_msg_cause, RRC_PS_DOMAIN_CN_ID );
                      pending_gmm_msg_valid = FALSE;
                    }

                  }
                  else
                  {
                     gmm_controls_resel_dec_mt_ps_call = FALSE;
                     ps_session_status = MMRRC_IDLE;
                     /* Cancel obsolete PS authentication attempts */
                     gmm_cancel_authentication() ;

                     MSG_HIGH_DS_0(MM_SUB,"=MM= Session Activation Failed");
                     if(message->cmd.rrc_open_session_cnf.status  != RRC_OPEN_SESS_FAILURE_CONN_REL_IN_PROG)
                     {
                       /* reset retransmission variables*/
                       mm_ps_domain_possible_reest = FALSE;
                       pending_gmm_msg_valid        = FALSE;
                       pending_gmm_msg_after_release = FALSE;
                     }
                  }

                  if ((pending_mm_msg_valid) && (cs_session_status != MMRRC_ACTIVE))
                  {
                     if (cs_session_status != MMRRC_EST_IN_PROGRESS)
                     {
                        MSG_HIGH_DS_0(MM_SUB, "=MM= CS domain is sending RRC_OPENSESSION_REQ");
                        send_open_session_req( RRC_CS_DOMAIN_CN_ID );
                     }else {
                                MSG_HIGH_DS_0(MM_SUB, "=MM= CS domain has sent RRC_OPEN_SESSION_REQ and not received RRC_OPEN_SESSION_CNF");
                       }

                  }
               }
               break;

            case RRC_REL_IND:
            case RRC_ABORT_IND:
            case RRC_ABORT_CNF:
#if defined(FEATURE_WRLF_SYSTEM_SEL) && !defined (FEATURE_LTE)
               if( mm_rlf_stop_mode_pending != MM_RLF_STOP_MODE_PENDING_NONE && 
               ((message->cmd.hdr.message_id == RRC_REL_IND) || (message->cmd.hdr.message_id == RRC_ABORT_IND)))
               {
                 MSG_HIGH_DS_0(MM_SUB,"WRLF Stop mode confirmation is pending ignoring the REL IND /ABORT IND ");
                 return;
               }
#endif
#if defined FEATURE_SGLTE
               /* Puting this check for the case when RRC abort received after PS merged to GSM
                              STAT4 check will take care that active rat will GSM , and as it is sglte mode message from Wrrc not expected
                        */
               if (MM_IS_IN_SGLTE_STATE_4 && (message->cmd.hdr.message_id == RRC_ABORT_IND ) )
               {
                 MSG_HIGH_DS_0(MM_SUB,"=MM= RRC ABORT received after PS merged to GSM , ignoring RRC_ABORT_IND ");
                 return;
               }                 
#endif
               if((message->cmd.rrc_abort_cnf.conn_rel_cause == RRC_UE_INIT_DORMANCY_UE_IN_OOS) &&
                  (message->cmd.hdr.message_id == (byte)RRC_ABORT_CNF))
               {
                 MSG_HIGH_DS_0(MM_SUB, "=MM= RRC could not process RRC_ABORT_REQ due to OOS");
               }
               else if(((message->cmd.rrc_abort_cnf.conn_rel_cause == RRC_UE_INIT_DORMANCY_ABORT_SUCCESS)
#ifdef FEATURE_DUAL_SIM
                         || (message->cmd.rrc_abort_cnf.conn_rel_cause == RRC_REL_DUAL_STANDBY_PS_ABORT_SUCCESS)
#endif
                       ) &&
                          (message->cmd.hdr.message_id == (byte)RRC_ABORT_CNF))
               {
#ifdef FEATURE_DUAL_SIM
                 if(message->cmd.rrc_abort_cnf.conn_rel_cause == RRC_REL_DUAL_STANDBY_PS_ABORT_SUCCESS)
                 {
                   rrc_connection_status = MMRRC_IDLE;
                 }
#endif
                 MSG_HIGH_DS_0(MM_SUB, "=MM= RRC is already in idle mode");
               }
               else
               {
                 MSG_HIGH_DS_0(MM_SUB,"=MM= RRC Connection Released");
                 if((message->cmd.hdr.message_id == (byte)RRC_ABORT_IND) &&
                    (mm_per_nas_stacks_is_mt_page_in_progress((sys_modem_as_id_e_type)mm_as_id)) &&                    
                    (rrc_connection_status == MMRRC_EST_IN_PROGRESS))
                 {
                    mm_send_cm_mt_call_fail_ind(CM_MM_MT_CALL_FAIL_CAUSE_CONNECTION_EST_FAILURE);
                 }
                 else if((message->cmd.hdr.message_id == (byte)RRC_REL_IND) &&
                        (mm_per_nas_stacks_is_mt_page_in_progress((sys_modem_as_id_e_type)mm_as_id) ||
                        ((mm_serving_plmn.info.active_rat == SYS_RAT_LTE_RADIO_ACCESS) &&  (mm_last_paging_cause != 0xFF))) &&
                        (mm_state!=MM_CONNECTION_ACTIVE) && 
                        (( (mm_timer_status[TIMER_T3240] != TIMER_ACTIVE) || (mm_connection_activated == FALSE) ) ||(mm_waiting_for_csmt_response == TRUE))) 
                 {
#ifdef FEATURE_WRLF_SYSTEM_SEL
                    if(mm_wcdma_rlf_state == WCDMA_RLF_STARTED)
                    {
                      mm_send_cm_mt_call_fail_ind(CM_MM_MT_CALL_FAIL_CAUSE_CONNECTION_FAILURE);
                    }
                    else
#endif
                    {
                      if(mm_waiting_for_csmt_response == TRUE)
                      {
                        mm_send_cm_mt_call_fail_ind(CM_MM_MT_CALL_FAIL_CAUSE_MT_CSFB_NO_RESPONSE_FROM_NW);
                      }
                      else
                      {
                        mm_send_cm_mt_call_fail_ind(CM_MM_MT_CALL_FAIL_CAUSE_NO_RESPONSE_FROM_NW);
                      }
                    }
                 }
#ifdef FEATURE_WRLF_SYSTEM_SEL
                 if(mm_wcdma_rlf_state == WCDMA_RLF_STARTED)
                 {
                   mm_cs_domain_possible_reest = FALSE; 
                   mm_ps_domain_possible_reest = FALSE;
                   pending_mm_msg_after_release = FALSE;
                   pending_gmm_msg_after_release = FALSE;
                   mmcoord_set_wrlf_state(WCDMA_RLF_IDLE);
                   mm_received_suitable_search_end_ind = FALSE;
                 }
#endif
                 gmm_controls_resel_dec_mt_ps_call = FALSE;
                 mm_waiting_for_csmt_response = FALSE;
                 rrc_connection_status = MMRRC_IDLE;
                 cs_session_status = MMRRC_IDLE;
                 ps_session_status = MMRRC_IDLE;
                 pending_mm_msg_valid = FALSE;
                 pending_gmm_msg_valid = FALSE;
  
                 /* Cancel obsolete CS & PS authentication attempts */
                 gmm_cancel_authentication() ;
                 mm_cancel_authentication() ;

#ifdef FEATURE_MODEM_HEAP
                 if((pending_mm_msg != NULL) && (!pending_mm_msg_after_release)
                       &&  (mm_cs_domain_possible_reest ==  FALSE)
                   )

                 {
                   modem_mem_free(pending_mm_msg,MODEM_MEM_CLIENT_NAS);
                   pending_mm_msg = NULL;
                 }
                 if((pending_gmm_msg != NULL) && (!pending_gmm_msg_after_release)
                       &&  (mm_ps_domain_possible_reest ==  FALSE)
                   )
                 {
                   modem_mem_free(pending_gmm_msg,MODEM_MEM_CLIENT_NAS);
                   pending_gmm_msg = NULL;
                 }
                 if((gmm_rat_chng_pended_msg_valid == FALSE) &&
                    (gmm_rat_chng_pended_msg != NULL))
                 {
                   modem_mem_free(gmm_rat_chng_pended_msg,MODEM_MEM_CLIENT_NAS);
                   gmm_rat_chng_pended_msg = NULL;
                 }
#endif
               }

               if (pending_mm_msg_after_release && (pending_mm_msg != NULL))
               {
                 mmrrcconn_send_L3_message_to_rrc(
                         pending_mm_msg, pending_mm_msg_cause, RRC_CS_DOMAIN_CN_ID );
               }
               if (pending_gmm_msg_after_release && (pending_gmm_msg != NULL))
               {
                 mmrrcconn_send_L3_message_to_rrc(
                         pending_gmm_msg, pending_gmm_msg_cause, RRC_PS_DOMAIN_CN_ID );
               }
           
              break;

            case RRC_CLOSE_SESSION_IND:
            case RRC_CLOSE_SESSION_CNF:
#if defined(FEATURE_WRLF_SYSTEM_SEL) && !defined (FEATURE_LTE)
              if( mm_rlf_stop_mode_pending != MM_RLF_STOP_MODE_PENDING_NONE)
              {
                MSG_HIGH_DS_0(MM_SUB,"WRLF Stop mode confirmation is pending ignoring the CLOSE SESSION IND/CNF ");
                return;
              }
#endif
               if ( ((message->cmd.hdr.message_id == RRC_CLOSE_SESSION_IND) && (message->cmd.rrc_close_session_ind.cn_domain_id == RRC_CS_DOMAIN_CN_ID)) ||
                    ((message->cmd.hdr.message_id == RRC_CLOSE_SESSION_CNF) && (message->cmd.rrc_close_session_cnf.cn_domain_id == RRC_CS_DOMAIN_CN_ID)))
               {
                  cs_session_status = MMRRC_IDLE;
                  gmm_controls_resel_dec_mt_ps_call = FALSE;
		  
                  /* Cancel obsolete CS authentication attempts */
                  mm_cancel_authentication() ;

#ifdef FEATURE_MODEM_HEAP
                  if((pending_mm_msg != NULL)
                       &&  (mm_cs_domain_possible_reest ==  FALSE)
                  )
                  {
                    modem_mem_free(pending_mm_msg,MODEM_MEM_CLIENT_NAS);
                    pending_mm_msg = NULL;
                    pending_mm_msg_valid = FALSE;
                  }
#endif
                  MSG_HIGH_DS_0(MM_SUB,"=MM= CS Session Released");
               }
               else if ( ((message->cmd.hdr.message_id == RRC_CLOSE_SESSION_IND) && (message->cmd.rrc_close_session_ind.cn_domain_id == RRC_PS_DOMAIN_CN_ID)) ||
                         ((message->cmd.hdr.message_id == RRC_CLOSE_SESSION_CNF) && (message->cmd.rrc_close_session_cnf.cn_domain_id == RRC_PS_DOMAIN_CN_ID)))
               {
                 if((message->cmd.rrc_close_session_cnf.status == RRC_CLOSE_SESS_FAILURE_UE_INIT_DORMANCY_UE_IN_OOS) &&
                    (message->cmd.hdr.message_id == (byte)RRC_CLOSE_SESSION_CNF))
                 {
                   MSG_HIGH_DS_0(MM_SUB, "=MM= RRC could not process RRC_CLOSE_SESSION_REQ due to OOS");
                 }
                 else if ((message->cmd.rrc_close_session_cnf.status == RRC_CLOSE_SESS_SUCCESS_UE_INIT_DORMANCY) &&
                          (message->cmd.hdr.message_id == (byte)RRC_CLOSE_SESSION_CNF))
                 {
                   MSG_HIGH_DS_0(MM_SUB, "=MM= RRC doesn't have PS Signaling connection");
                 }                 
                 else
                 {
                    gmm_controls_resel_dec_mt_ps_call = FALSE;
                    ps_session_status = MMRRC_IDLE;

                    /* Cancel obsolete PS authentication attempts */
                    gmm_cancel_authentication() ;
#ifdef FEATURE_MODEM_HEAP
                    if((pending_gmm_msg != NULL)
                       &&  (mm_ps_domain_possible_reest ==  FALSE)
                   )
                    {
                      modem_mem_free(pending_gmm_msg,MODEM_MEM_CLIENT_NAS);
                      pending_gmm_msg = NULL;
                      pending_gmm_msg_valid =  FALSE;
                    }
                    if((gmm_rat_chng_pended_msg_valid == FALSE) &&
                       (gmm_rat_chng_pended_msg != NULL))
                    {
                      modem_mem_free(gmm_rat_chng_pended_msg,MODEM_MEM_CLIENT_NAS);
                      gmm_rat_chng_pended_msg = NULL;
                    }
#endif
                    MSG_HIGH_DS_0(MM_SUB,"=MM= PS Session Released");
                 }
               }

               if (pending_mm_msg_after_release && (pending_mm_msg != NULL))
               {
                 mmrrcconn_send_L3_message_to_rrc(
                         pending_mm_msg, pending_mm_msg_cause, RRC_CS_DOMAIN_CN_ID );
                 pending_mm_msg_after_release = FALSE;
               }
               if (pending_gmm_msg_after_release && (pending_gmm_msg != NULL))
               {
                 mmrrcconn_send_L3_message_to_rrc(
                         pending_gmm_msg, pending_gmm_msg_cause, RRC_PS_DOMAIN_CN_ID );
                 pending_gmm_msg_after_release = FALSE;
               }

               /* #FIXME- What happens if both sessions are closed */
               break;

            default:
               break;
         } /* end switch (message->cmd.hdr.message_id) */
         break;

      default:
         MSG_FATAL_DS( MM_SUB, "=MM= Unexpected message set %d", message->cmd.hdr.message_set, 0,0 );
   } /* end switch (message->cmd.hdr.message_set) */
} /* end coord_rrc_conn_management( mm_cmd_type message ) */
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/

/*===========================================================================

FUNCTION MM_VALID_MSG_FOR_REESTABLISH

DESCRIPTION
  This function returns TRUE if the procedure can be retried immediately if not aborted intentionally (Rel7-CR1099)
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if procedure can be initiated again.

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_valid_msg_for_reestablish
(
   byte pd,
   byte msg_id
)
{
   boolean reinit_procedure = FALSE;

   if (pd==PD_MM)
   {
     if((rrc_connection_status == MMRRC_RELEASE_IN_PROGRESS) || 
     (cs_session_status == MMRRC_RELEASE_IN_PROGRESS))
     {
       reinit_procedure = TRUE;
     }
     else
     switch (msg_id)
     {
       case LOCATION_UPDATING_REQUEST :
       case CM_SERVICE_REQUEST :
         reinit_procedure = TRUE;
         break;
       default :
         break;
     }
   }
   else if(pd==PD_GMM)
   {
     if((rrc_connection_status == MMRRC_RELEASE_IN_PROGRESS) || 
     (ps_session_status == MMRRC_RELEASE_IN_PROGRESS))
     {
       reinit_procedure = TRUE;
     }
     else
     switch (msg_id)
     {
       case GMM_ATTACH_REQUEST :
         if(t3310_timeout_counter == 0)
         {
           reinit_procedure = TRUE;
         }
         break;
       case GMM_ROUTING_AREA_UPDATE_REQUEST :
         if(t3330_timeout_counter == 0)
         {
           reinit_procedure = TRUE;
         }
         break;
       case GMM_DETACH_REQUEST :
         if(!gmm_power_down_active)
         {
           reinit_procedure = TRUE;
         }
         break;
       case GMM_SERVICE_REQUEST:
         reinit_procedure = TRUE;
         break;
       default :
         break;
     }
   }

   return reinit_procedure;
}



/*===========================================================================

FUNCTION MMRRCCONN_SEND_L3_MESSAGE_TO_RRC

DESCRIPTION
  This function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mmrrcconn_send_L3_message_to_rrc
(
  void                          *message,
  rrc_establish_cause_e_type    cause,
  rrc_cn_domain_identity_e_type domain
)
{
   uint32 length;

#ifdef FEATURE_GSM_GPRS

   byte protocol_discriminator = (byte) (((byte *)message + sizeof(IMH_T))[0] & 0x0F);
   byte message_id             = (byte) (((byte *)message + sizeof(IMH_T))[1]) ;
   boolean buffer_outgoing_msg ;

   /* Get message length */
   GET_IMH_LEN( length, (IMH_T*)message) ;

   if(length > (MAX_MESSAGE_SIZE - sizeof(IMH_T)))
   {
      MSG_HIGH_DS_1(MM_SUB ,"=MM= Unexpected length:%d",length);
      length=MAX_MESSAGE_SIZE - sizeof(IMH_T);
      PUT_IMH_LEN( length, (IMH_T*)message);
   }
   if (gmm_rau_is_required && (domain == RRC_PS_DOMAIN_CN_ID))
   {
     if (protocol_discriminator==PD_GMM)
     {
       switch (message_id)
       {
         case GMM_ATTACH_REQUEST :
         case GMM_SERVICE_REQUEST :
         case GMM_PTMSI_REALLOCATION_COMPLETE :
         case GMM_AUTHENTICATION_AND_CIPHERING_RESPONSE :
         case GMM_AUTHENTICATION_AND_CIPHERING_FAILURE :
         case GMM_IDENTITY_RESPONSE :
         case GMM_STATUS :
           buffer_outgoing_msg = TRUE ;
           break ;
         case GMM_ROUTING_AREA_UPDATE_REQUEST:
         case GMM_DETACH_ACCEPT:
           buffer_outgoing_msg = FALSE ;
           break ;
         case GMM_DETACH_REQUEST :
           {
            byte gmm_detach_cause = ((gmm_mo_detach_request_msg_T*)message)->detach_type_spare;                               
            
            if (gmm_detach_cause & 0x08)
            {
              buffer_outgoing_msg = FALSE ;
            }
            else
            {
              buffer_outgoing_msg = TRUE ;
            }
           }
           break ;
         default :
           MSG_ERROR_DS(MM_SUB, "=MM= Unidentified GMM message is not supposed to be sent %d",message_id,0,0);
           buffer_outgoing_msg = TRUE ;
           break ;
       }
     }
     else
     {
       buffer_outgoing_msg = TRUE ;
     }
   }
   else
   {
     buffer_outgoing_msg = FALSE ;
   }

   if (buffer_outgoing_msg)
   {
     length += sizeof(IMH_T) ;

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#ifdef FEATURE_MODEM_HEAP
     if(( gmm_rat_chng_pended_msg == NULL ) &&
        (( gmm_rat_chng_pended_msg = (byte *) modem_mem_calloc(1,MAX_MESSAGE_SIZE,MODEM_MEM_CLIENT_NAS) ) == NULL) )
     {
       mm_check_for_null_ptr((void*)gmm_rat_chng_pended_msg);
     }
#endif
     memscpy(gmm_rat_chng_pended_msg,MAX_MESSAGE_SIZE,message,length) ;

     gmm_rat_chng_pended_msg_cause = cause ;

     gmm_rat_chng_pended_msg_valid = TRUE ;
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/     

     /* Initiate RAU */
     if (((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) ||
         (protocol_discriminator == PD_GMM && message_id == GMM_DETACH_REQUEST))&&
         (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
     {
       gmm_ra_update_type = GMM_COMBINED_RA_LA_UPDATING;
     }
     else
     {
       gmm_ra_update_type = GMM_RA_UPDATING;
     }

     gmm_initiate_routing_area_update() ;

     return ;
   }
   else if (is_gsm_mode() && (domain == RRC_PS_DOMAIN_CN_ID))
   {
     gmmllc_gmm_send_ota_message(length
                                 , ((byte *)message + sizeof(IMH_T))
#ifdef FEATURE_GPRS_PS_HANDOVER
                                 ,FALSE
#endif  
                                ) ;
   }
   else

#endif

   {

    if(domain == RRC_CS_DOMAIN_CN_ID)
    {
      mm_cs_domain_possible_reest = FALSE;
    }
    if(domain == RRC_PS_DOMAIN_CN_ID)
    {
      mm_ps_domain_possible_reest = FALSE;
    }

    if ((domain == RRC_CS_DOMAIN_CN_ID) && (cs_session_status == MMRRC_ACTIVE))
    {
      /*
      ** If the cs connection is active and MM is in wait_for_rr_conneciton_LU
      ** Transition states to LU initiated
      **/
      if (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
      {
          mm_state_control(MM_STD_RR_CONNECTION_ESTABLISHED);
      }

      /* --------------------------------------------------------------------
      ** 24.008 4.4.4.9, 4.5.1.2: cases f.1 
      ** The procedure shall be initiated again, if 
      ** i)The original service request procedure was initiated over an existing RR connection
      ** -------------------------------------------------------------------- */
      if(mm_valid_msg_for_reestablish(protocol_discriminator,message_id))
      {
         mm_cs_domain_possible_reest = TRUE;

         /* Store the inital transfer message for sending later by
            re-establishing RRC connection if required*/
         GET_IMH_LEN( length, message );
         length += sizeof(IMH_T);
#ifdef FEATURE_MODEM_HEAP
         if(( pending_mm_msg == NULL ) && 
            (( pending_mm_msg = (byte *) modem_mem_calloc(1,MAX_MESSAGE_SIZE,MODEM_MEM_CLIENT_NAS) ) == NULL) )
         {
           mm_check_for_null_ptr((void*)pending_mm_msg);
         }
#endif
         (void)memscpy( pending_mm_msg,MAX_MESSAGE_SIZE, message, length );
         pending_mm_msg_cause = cause;
      }

      mm_send_nas_msg_to_rrc_by_domain((byte*) message, RRC_CS_DOMAIN_CN_ID );
    }
    else if ((domain == RRC_PS_DOMAIN_CN_ID) && (ps_session_status == MMRRC_ACTIVE))
    {

      /* --------------------------------------------------------------------
      ** 24.008 4.7.3.1.5,4.7.4.1.4, 4.7.5.1.5, 4.7.13.5: cases b.1 and b.2
      ** The procedure shall be initiated again, if 
      ** i)The original service request procedure was initiated over an existing PS signalling/RR connection
      ** -------------------------------------------------------------------- */
      if(mm_valid_msg_for_reestablish(protocol_discriminator,message_id))
      {
         mm_ps_domain_possible_reest = TRUE;

         /* Store the inital transfer message for sending later by
            re-establishing RRC connection  if required*/
         GET_IMH_LEN( length, message );
         length += sizeof(IMH_T);
#ifdef FEATURE_MODEM_HEAP
         if(( pending_gmm_msg == NULL ) && 
            (( pending_gmm_msg = (byte *) modem_mem_calloc(1,MAX_MESSAGE_SIZE,MODEM_MEM_CLIENT_NAS) ) == NULL) )
         {
           mm_check_for_null_ptr((void*)pending_gmm_msg);
         }
#endif
         (void)memscpy( pending_gmm_msg,MAX_MESSAGE_SIZE, message, length );
         pending_gmm_msg_cause = cause;
      }
        
      mm_send_nas_msg_to_rrc_by_domain( (byte*) message, RRC_PS_DOMAIN_CN_ID );
    }
    else /* No valid session */
    {
      /* Get message length */
      GET_IMH_LEN( length, message );

      /* Add in header size */
      length += sizeof(IMH_T);

      if (domain == RRC_CS_DOMAIN_CN_ID)
      {
         /* Store the inital transfer message for sending after successful
            RRC connection establishment */
#ifdef FEATURE_MODEM_HEAP
         if(( pending_mm_msg == NULL ) && 
            (( pending_mm_msg = (byte *) modem_mem_calloc(1,MAX_MESSAGE_SIZE,MODEM_MEM_CLIENT_NAS) ) == NULL) )
         {
           mm_check_for_null_ptr((void*)pending_mm_msg);
         }
#endif
         (void)memscpy( pending_mm_msg,MAX_MESSAGE_SIZE, message, length );
         pending_mm_msg_cause = cause;
         pending_mm_msg_valid = TRUE;
      }
      else if (domain == RRC_PS_DOMAIN_CN_ID)
      {
         /* Store the inital transfer message for sending after successful
            RRC connection establishment */
#ifdef FEATURE_MODEM_HEAP
         if(( pending_gmm_msg == NULL ) && 
            (( pending_gmm_msg = (byte *) modem_mem_calloc(1,MAX_MESSAGE_SIZE,MODEM_MEM_CLIENT_NAS) ) == NULL) )
         {
           mm_check_for_null_ptr((void*)pending_gmm_msg);
         }
#endif
         (void)memscpy( pending_gmm_msg,MAX_MESSAGE_SIZE, message, length );
         pending_gmm_msg_cause = cause;
         pending_gmm_msg_valid = TRUE;
      }

      if (rrc_connection_status != MMRRC_ACTIVE)
      {
         if (rrc_connection_status != MMRRC_EST_IN_PROGRESS)
         {
            send_rrc_est_req( domain, cause);
         }
      }
      else /* RRC connection_already established */
      {
         if(mm_valid_msg_for_reestablish(protocol_discriminator,message_id))
         {
            if(domain == RRC_CS_DOMAIN_CN_ID)
            {
              mm_cs_domain_possible_reest = TRUE;
            }
            else if(domain == RRC_PS_DOMAIN_CN_ID)
            {
              mm_ps_domain_possible_reest = TRUE;
            }
         }
          if (((domain == RRC_CS_DOMAIN_CN_ID) && (cs_session_status != MMRRC_EST_IN_PROGRESS)) ||
              ((domain == RRC_PS_DOMAIN_CN_ID) && (ps_session_status != MMRRC_EST_IN_PROGRESS)))
         {
            send_open_session_req( domain );
         }
      }
    }
   }
} /* end send_L3_message_to_rrc() */


/*===========================================================================

FUNCTION SEND_RRC_EST_REQ

DESCRIPTION
  This function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_rrc_est_req( rrc_cn_domain_identity_e_type cn_domain,
                       rrc_establish_cause_e_type cause )
{
#ifdef FEATURE_WCDMA

   dword threshold ;
   rrc_cmd_type* rrc_est_req_cmd_p;
   rrc_est_req_type     *rrc_est_req_p ;
   hfn_type hfns;

#ifdef FEATURE_LTE
#ifdef TF_UNIT_TEST
   #error code not present
#endif
#endif

   if (cn_domain == RRC_CS_DOMAIN_CN_ID)
   {
      if(rrc_connection_status == MMRRC_RELEASE_IN_PROGRESS)
      {
        pending_mm_msg_after_release = TRUE;
        MSG_HIGH_DS_0(MM_SUB,"=MM= Release in progress: MM msg pended");
        return;
      }
       
      if((mm_timer_status [ TIMER_T3211] != TIMER_STOPPED) &&
         (mm_idle_substate == MM_LIMITED_SERVICE))
      {
        MSG_HIGH_DS_0(MM_SUB,"=MM= RRC conn req: mm_previous_idle_substate is reset");
        mm_previous_idle_substate = NULL;
      }
      mm_stop_timer( TIMER_T3211 );
   }
   else if (cn_domain == RRC_PS_DOMAIN_CN_ID)
   {
      if(rrc_connection_status == MMRRC_RELEASE_IN_PROGRESS)
      {
        pending_gmm_msg_after_release = TRUE;
        MSG_HIGH_DS_0(MM_SUB,"=MM= Release in progress: MM msg pended");
        return;
      }
       
      /* do PS sepcific processing */
   }
   
   rrc_est_req_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );
   if (rrc_est_req_cmd_p == NULL)
   {
     MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
   }
   
   rrc_est_req_cmd_p->cmd_hdr.cmd_id = RRC_EST_REQ;

   rrc_est_req_p = &(rrc_est_req_cmd_p->cmd.rrc_est_req);

   rrc_est_req_p->cause             = cause;

   rrc_est_req_p->initial_ue_id     = mm_get_initial_ue_id(&rrc_est_req_p->ue_id_choice);

   rrc_est_req_p->cn_domain_id = cn_domain;
   
   if((cn_domain == RRC_CS_DOMAIN_CN_ID) && 
       ((cause == RRC_EST_ORIG_CONV_CALL) || (cause == RRC_EST_EMERGENCY_CALL)))
   {
     switch (call_type_for_rrc_est_req)
     {
      case SPEECH_CALL:
           rrc_est_req_p ->call_type = RRC_CALL_TYPE_SPEECH;
         break;     
      case VIDEO_CALL:
           rrc_est_req_p ->call_type = RRC_CALL_TYPE_VIDEO;
         break;  
      case OTHER_CALL:
           rrc_est_req_p ->call_type = RRC_CALL_TYPE_OTHERS;
         break;
      default :
           rrc_est_req_p ->call_type =   RRC_CALL_TYPE_NONE;
     }
   }
   else if((cn_domain == RRC_CS_DOMAIN_CN_ID) && (cause == RRC_EST_TERM_CONV_CALL))
   {
     rrc_est_req_p ->call_type = RRC_CALL_TYPE_SPEECH;    
   }
   else
   {
     rrc_est_req_p ->call_type = RRC_CALL_TYPE_OTHERS;
   }
#ifdef FEATURE_ABORT_ESTB_ON_RA_LA_MISMATCH
   rrc_est_req_p->rai.lai.plmn = 
      mmumts_convert_nas_plmn_id_to_rrc_plmn_id(mm_serving_plmn.info.plmn);

   rrc_est_req_p->rai.lai.lac[0] = mm_serving_plmn.lac.lac[0];
   
   rrc_est_req_p->rai.lai.lac[1] = mm_serving_plmn.lac.lac[1];
   
   rrc_est_req_p->rai.rac[0] = mm_serving_plmn.rac;   

#endif /* FEATURE_ABORT_ESTB_ON_RA_LA_MISMATCH */

   if ((mm_sim_card_mode == MMGSDI_APP_USIM)
#ifdef FEATURE_NAS_REL11
        ||(mm_sim_card_mode == MMGSDI_APP_SIM)
#endif
      )
   {
      auth_get_cs_hfn(hfns.cs_hfn);

      auth_get_ps_hfn(hfns.ps_hfn);

      threshold = auth_get_hfn_threshold() ;

      /** 33.102:R11  6.4.3 When the UE has powered on and before attempting to connect to any network,
           the ME reads the  START values from the USIM and stores them in the volatile memory of ME. 
           The ME then marks the START values in the USIM as invalid by setting STARTCS and STARTPS to THRESHOLD **/
               
#ifdef FEATURE_NAS_REL11
      if (mm_is_first_rrc_connection_attempt)
      {
        auth_update_hfns_to_sim_card(TRUE);
        mm_is_first_rrc_connection_attempt = FALSE;
      }
#endif

      switch (cn_domain)
      {
         case RRC_CS_DOMAIN_CN_ID:
            if (auth_translate_hfn(hfns.cs_hfn) >= threshold)
            {
               auth_delete_cksn(AUTH_CS_GPRS_SECURITY_CONTEXT) ;
            }
            break;
         case RRC_PS_DOMAIN_CN_ID:
            if (auth_translate_hfn(hfns.ps_hfn) >= threshold)
            {
               auth_delete_cksn(AUTH_PS_GPRS_SECURITY_CONTEXT) ;
            }
            break;
         default:
           /* There is do nothing */
           break ;
      }
   }

   send_cmd_to_rrc( rrc_est_req_cmd_p, mm_serving_plmn.info.active_rat );
#endif /* FEATURE_WCDMA */
} /* end send_rrc_est_req() */


/*===========================================================================

FUNCTION SEND_OPEN_SESSION_REQ

DESCRIPTION
  This function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_open_session_req( rrc_cn_domain_identity_e_type cn_domain )
{
#ifdef FEATURE_WCDMA

   IMH_T *pending_msg = NULL;
   rrc_cmd_type *rrc_cmd_p;
   uint32 length;
   rrc_establish_cause_e_type est_cause;
   rrc_call_type_in_est_req_e_type  call_type = RRC_CALL_TYPE_OTHERS;
   gmm_ie_pdp_context_status_T new_pdp_context_status, new_uplink_data_status;
#ifdef FEATURE_SEGMENT_LOADING   
   interface_t *ptr_W = get_wcdma_interface();
#endif
   if (cn_domain == RRC_CS_DOMAIN_CN_ID)
   {
      if((rrc_connection_status == MMRRC_RELEASE_IN_PROGRESS) || 
         (cs_session_status == MMRRC_RELEASE_IN_PROGRESS))
      {
        pending_mm_msg_after_release = TRUE;
        MSG_HIGH_DS_0(MM_SUB,"=MM= Release in progress: MM msg pended");
        return;
      }
       
      if( pending_mm_msg_valid == FALSE)
      {
         MSG_ERROR_DS(MM_SUB, "=MM= Trying to open CS session with out a direct transfer msg", 0,0,0);
         return;
      }
#ifdef FEATURE_MODEM_HEAP
      if(pending_mm_msg == NULL)
      {
        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Null pointer for pending_mm_msg", 0,0,0 );
      }
#endif
      pending_msg = (IMH_T *)pending_mm_msg;
      pending_mm_msg_valid = FALSE;
      est_cause = pending_mm_msg_cause;
      
      if((est_cause == RRC_EST_ORIG_CONV_CALL) || (est_cause == RRC_EST_EMERGENCY_CALL))
      {
        switch (call_type_for_rrc_est_req)
        {
         case SPEECH_CALL:
              call_type = RRC_CALL_TYPE_SPEECH;
            break;     
         case VIDEO_CALL:
              call_type = RRC_CALL_TYPE_VIDEO;
            break;  
         case OTHER_CALL:
              call_type = RRC_CALL_TYPE_OTHERS;
            break;
         default :
              call_type =   RRC_CALL_TYPE_NONE;
        }
      }
      else if(est_cause == RRC_EST_TERM_CONV_CALL)
      {
       call_type = RRC_CALL_TYPE_SPEECH;
      }
   }
   else /* PS domain */
   {
      if((rrc_connection_status == MMRRC_RELEASE_IN_PROGRESS) || 
         (ps_session_status == MMRRC_RELEASE_IN_PROGRESS))
      {
        pending_gmm_msg_after_release = TRUE;
        MSG_HIGH_DS_0(MM_SUB,"=MM= Release in progress: GMM msg pended");
        return;
      }
       
      if( pending_gmm_msg_valid == FALSE)
      {
         MSG_ERROR_DS(MM_SUB, "=MM= Trying to open PS session with out a direct transfer msg", 0,0,0);
         return;
      }
#ifdef FEATURE_MODEM_HEAP
      if(pending_gmm_msg == NULL)
      {
        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Null pointer for pending_gmm_msg", 0,0,0 );
      }
#endif
/* 
    if pending message is gmm_service_req then we need to refresh our PDP contexts and Uplink data status before sending open session req, as it may be the case 
    that PDP, for which RRC connection was requested is deactivated meanwhile we got RR_EST_CNF. So we need to handle this situation and need to send the abort 
    if PDP got deactivated in between(same is true for Uplink data), otherwise we need to send service_request with refreshed values.
*/
  
      if((pending_gmm_msg[4] & (0x0F)) == PD_GMM && pending_gmm_msg[5] == GMM_SERVICE_REQUEST)
      {
        if(gmm_service_request_type == GMM_SIGNALLING)
        {
        /* for the first time, when PDP activate req is coming the value would be all zero in context status, so don't check for the first time */          
          if(((gmm_service_request_msg_T*)pending_gmm_msg)->data[2] || ((gmm_service_request_msg_T*)pending_gmm_msg)->data[3])
          {
            new_pdp_context_status = get_pdp_context_status((sys_modem_as_id_e_type) mm_sub_id);
            MSG_HIGH_DS_2( MM_SUB, "=MM= Old PDP context status %d, %d\n", ((gmm_service_request_msg_T*)pending_gmm_msg)->data[2], ((gmm_service_request_msg_T*)pending_gmm_msg)->data[3]);
            MSG_HIGH_DS_2( MM_SUB, "=MM= New PDP context status %d, %d\n", new_pdp_context_status.value[0], new_pdp_context_status.value[1]);  
            if((new_pdp_context_status.value[0] & ((gmm_service_request_msg_T*)pending_gmm_msg)->data[2]) ||
                (new_pdp_context_status.value[1] & ((gmm_service_request_msg_T*)pending_gmm_msg)->data[3]))
            {
              //we need to send service req with refreshed value
              MSG_HIGH_DS_0( MM_SUB, "=MM= sending service request after refreshing the value of pdp context status\n");
              ((gmm_service_request_msg_T*)pending_gmm_msg)->data[2] = new_pdp_context_status.value[0];
              ((gmm_service_request_msg_T*)pending_gmm_msg)->data[3] = new_pdp_context_status.value[1];
            }
            else
            {
              // need to abort the established connection.as pdp for which it was requested is deactivated now.
              MSG_HIGH_DS_0( MM_SUB, "=MM= MM sending ABORT_REQ to lower layer as PDP(s) for which connection was requested is/are deactivated now\n" );
              mm_abort_rr( RRC_PS_DOMAIN_CN_ID, TRUE );
              return;
            }
          }
        }
        else if(gmm_service_request_type == GMM_DATA)
        {
          
          byte index = 6;
  #ifdef FEATURE_MBMS
          #error code not present
#endif
          new_uplink_data_status = get_uplink_data_status((sys_modem_as_id_e_type) mm_sub_id);
          MSG_HIGH_DS_2( MM_SUB, "=MM= Old Uplink data status: %d, %d\n", ((gmm_service_request_msg_T*)pending_gmm_msg)->data[index], ((gmm_service_request_msg_T*)pending_gmm_msg)->data[index+1]);
          MSG_HIGH_DS_2( MM_SUB, "=MM= New Uplink data status: %d, %d\n", new_uplink_data_status.value[0], new_uplink_data_status.value[1]);  
          if((new_uplink_data_status.value[0] & ((gmm_service_request_msg_T*)pending_gmm_msg)->data[index]) ||
              (new_uplink_data_status.value[1] & ((gmm_service_request_msg_T*)pending_gmm_msg)->data[index+1]))
          {
            //we need to send service req with refreshed value
            MSG_HIGH_DS_0( MM_SUB, "=MM= sending service request after refreshing the value of uplink data status\n");
            ((gmm_service_request_msg_T*)pending_gmm_msg)->data[index] = new_uplink_data_status.value[0];
            ((gmm_service_request_msg_T*)pending_gmm_msg)->data[index +1] = new_uplink_data_status.value[1];
          }
          else
          {
            // need to abort the connection as uplink data status is changed now.
            MSG_HIGH_DS_0( MM_SUB, "=MM= MM sending ABORT_REQ to lower layer as new uplink data status is not the subset of what was requested at first place\n");
            mm_abort_rr( RRC_PS_DOMAIN_CN_ID, TRUE );
            return;
          }
        }
      }
      pending_msg = (IMH_T *)pending_gmm_msg;
      pending_gmm_msg_valid = FALSE;
      est_cause = pending_gmm_msg_cause;
   }

   GET_IMH_LEN(length, pending_msg);

   rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );

   if (rrc_cmd_p == NULL)
   {
      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
   }

   rrc_cmd_p->cmd_hdr.cmd_id = RRC_OPEN_SESSION_REQ;

   rrc_cmd_p->cmd.open_sess_req.cn_domain_id = cn_domain;
   rrc_cmd_p->cmd.open_sess_req.nas_msg.length = length;
#ifndef FEATURE_MODEM_HEAP
   rrc_cmd_p->cmd.open_sess_req.nas_msg.value = (uint8*) mm_mem_alloc(length);
#else
   if (is_wcdma_mode())
   {  
#ifdef FEATURE_SEGMENT_LOADING
     rrc_cmd_p->cmd.open_sess_req.nas_msg.value = (uint8*)IWCDMA_rrc_malloc_for_external_cmd(ptr_W, length);
#else
     rrc_cmd_p->cmd.open_sess_req.nas_msg.value = (uint8*)rrc_malloc_for_external_cmd(length);
#endif
   }
#ifdef FEATURE_TDSCDMA
   else if (is_tdscdma_mode())
   {
     rrc_cmd_p->cmd.open_sess_req.nas_msg.value =
       mm_mem_alloc( rrc_cmd_p->cmd.open_sess_req.nas_msg.length );
   }
#endif /* FEATURE_TDSCDMA */
#endif
   if (rrc_cmd_p->cmd.open_sess_req.nas_msg.value == NULL)
   {
      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
   }

   memscpy( (void *)rrc_cmd_p->cmd.open_sess_req.nas_msg.value,length,
           (void *)((uint8*)pending_msg+sizeof(IMH_T)), length );
           
   rrc_cmd_p->cmd.open_sess_req.est_cause = est_cause;
   rrc_cmd_p->cmd.open_sess_req.call_type = call_type;

#ifdef FEATURE_NETWORK_SHARING
   rrc_cmd_p->cmd.open_sess_req.selected_plmn_id =
         mmumts_convert_nas_plmn_id_to_rrc_plmn_id( mm_serving_plmn.info.plmn);
#endif

   /* Fill-in the intra domain nas node selector */
   mm_fill_intra_domain_nas_node_selector(rrc_cmd_p);

   send_cmd_to_rrc(rrc_cmd_p, mm_serving_plmn.info.active_rat );
#endif /* FEATURE_WCDMA */

} /* end send_open_session_req() */



/*===========================================================================

FUNCTION MM_FILL_IMSI_ROUTING_PARAMETER

DESCRIPTION
  This function fills the routing paramater of the intra domain nas node
  selector with the IMSI value. The IMSI value is reduced to 10 bits using
  a formula and filled in the routing parameter

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_fill_imsi_routing_parameter(
                        byte *data)
{
   rrc_imsi_gsm_map_type mm_rrc_imsi = convert_imsi_to_digits(mm_stored_imsi);

   uint16 decimal_routing_param = 0;

   /* The formula used to compute the decimal number is given in
    * 25.331 10.3.1.6.
    * DecimalToBinary[ (IMSI div 10) mod 1000 ]
    * First convert IMSI to digits
    * Then the last byte is ignored to do div 10 - removes the last digit
    * Then mod 1000 yields the next three bytes from the last byte
    * ( For example given IMSI 440 793 1787902 first the remove the
    * last digit, then choose the last three remaining digits - 790
    */

   decimal_routing_param  = (uint16)(mm_rrc_imsi.value[mm_rrc_imsi.num_digits - 2]);
   decimal_routing_param += (uint16)(mm_rrc_imsi.value[mm_rrc_imsi.num_digits - 3] * 10 );
   decimal_routing_param += (uint16)(mm_rrc_imsi.value[mm_rrc_imsi.num_digits - 4] * 100 );

   /* The number is shifted by 6 bits to push the value to the
    * most significant side. The routing param formatting requires this
    * please refere rrcmmif.h
    */
   decimal_routing_param <<= 6;

   /*
    * copy into the routing param into
    * the first 10 bits of the array
    */
   data[0] = (byte)(decimal_routing_param / 256);
   data[1] = (byte)(decimal_routing_param % 256);
}



/*===========================================================================

FUNCTION MM_FILL_IMEI_ROUTING_PARAMETER

DESCRIPTION
  This function fills the routing paramater of the intra domain nas node
  selector with the IMEI value. The IMEI value is reduced to 10 bits using
  a formula and filled in the routing parameter

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_fill_imei_routing_parameter(
                        byte *data)
{
   rrc_imei_gsm_map_type rrc_imei = get_imei_as_digits();

   uint16 decimal_routing_param = 0;

   /* The formulae use to compute the decimal number is given in
    * 25.331 10.3.1.6.
    * DecimalToBinary[ (IMEI div 10) mod 1000 ]
    * First convert IMEI to digits
    * Then the last byte is ignored to do div 10 - removes the last digit
    * Then mod 1000 yields the next three bytes from the last byte
    * ( For example given IMEI 440 793 1787902 first the remove the
    * last digit, then choose the last three remaining digits - 790
    */

   /* IMEI is always 15 digits.. */
   decimal_routing_param  = (uint16)(rrc_imei.value[13]);
   decimal_routing_param += (uint16)(rrc_imei.value[12] * 10 );
   decimal_routing_param += (uint16)(rrc_imei.value[11] * 100 );

   /* The number is shifted by 6 bits to push the value to the
    * most significant side. The routing param formatting requires this
    * plese refere rrcmmif.h
    */
   decimal_routing_param <<= 6;

   /*
    * copy into the routing param
    * the first 10 bits of this array now contain the routing param
    */
   data[0] = (byte)(decimal_routing_param / 256);
   data[1] = (byte)(decimal_routing_param % 256);
}



/*===========================================================================

FUNCTION MM_FILL_PTMSI_ROUTING_PARAMETER

DESCRIPTION
  This function fills the routing parameter of the intra domain nas node
  selector with bit 14 to bit 23 of the PTMSI value

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_fill_ptmsi_routing_parameter(byte *data, const gmm_ptmsi_type *ptmsi)
{
   /* bit b14 to b23 is 2nd byte and top 2 bits of 3rd byte */
   data[0] = ptmsi->digit[1];
   data[1] = ptmsi->digit[2] & 0xc0;
}



/*===========================================================================

FUNCTION MM_FILL_TMSI_ROUTING_PARAMETER

DESCRIPTION
  This function fills the routing parameter of the intra domain nas node
  selector with bit 14 to bit 23 of the TMSI value

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_fill_tmsi_routing_parameter(byte *data)
{
   /* bit b14 to b23 is 2nd byte and top 2 bits of 3rd byte */
   data[0] = mm_location_information.tmsi[1];
   data[1] = mm_location_information.tmsi[2] & 0xc0;
}



/*===========================================================================

FUNCTION MM_FILL_INTRA_DOMAIN_NAS_NODE_SELECTOR

DESCRIPTION
  This function fills the intra domain nas node selector structure
  with relevant fields

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_fill_intra_domain_nas_node_selector( rrc_cmd_type *rrc_cmd )
{

#if defined(FEATURE_LTE)
   emm_ctrl_data_type *emm_ctrl_data_ptr = emm_db_get_ctrl_data();   
   gmm_ptmsi_type   ptmsi;
   ASSERT(emm_ctrl_data_ptr != NULL);
#endif

   /*
   ** Fill in default value for CN choice
   */
   rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector.corenetwork_choice = RRC_GSM_MAP;

   /* ----------------------------------------------------------------------
      Intra domain nas node selector is information directed towards the RNC
      to help choose a particular CN node out of the available CN nodes for
      load balancing and reduced MM signalling. The following information
      is filled according to spec 25.331 10.3.1.6.  The routing_basis
      parameter is set according to 25.331 section 8.1.8.2.
      ---------------------------------------------------------------------- */

   /* On valid IMSI */
   if( mm_stored_imsi.length != 0)
   {
      if( rrc_cmd->cmd.open_sess_req.nas_msg.length <= 2)
      {
         MSG_ERROR_DS(MM_SUB, "=MM= Nas message is too short", 0,0,0);
         return;
      }

      /* On valid CS update status */
      if (rrc_cmd->cmd.open_sess_req.cn_domain_id == RRC_CS_DOMAIN_CN_ID &&
          has_valid_tmsi())
      {
         /* Fill in the TMSI */
         mm_fill_tmsi_routing_parameter(
            rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
            .routing_info.routing_parameter);

         /* Determine routing basis type */

         /* On same PLMN */
         if( memcmp(&mm_serving_plmn.info.plmn,
                    mm_location_information.lai,
                    PLMN_ID_SIZE) == 0)
         {
            /* TMSI allocated in the current LA */
            if( memcmp(&mm_serving_plmn.lac,
                       mm_location_information.lai + PLMN_ID_SIZE,
                       LAC_SIZE) == 0)
            {
               rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
                  .routing_info.routing_basis = RRC_LOCAL_TMSI_OR_PTMSI;
            }
            /* TMSI allocated in different LA */
            else
            {
               rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
                  .routing_info.routing_basis =
                     RRC_TMSI_OR_PTMSI_SAME_PLMN_AND_DIFFERENT_RA_OR_LA;
            }
         } /* different PLMN */
         else
         {
            rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
                  .routing_info.routing_basis = RRC_TMSI_OR_PTMSI_OF_DIFFERENT_PLMN;

         }
      }

#if defined(FEATURE_LTE)
      /*If TIN is GUTI then use mapped identity */
      else if(rrc_cmd->cmd.open_sess_req.cn_domain_id == RRC_PS_DOMAIN_CN_ID
                  && emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI
                  && emm_ctrl_data_ptr->emm_guti_valid == TRUE)
      {
        /*Fill in mapped PTMSI*/
        emm_map_guti_to_ptmsi(&ptmsi, emm_ctrl_data_ptr);
        
        mm_fill_ptmsi_routing_parameter(
            rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
             .routing_info.routing_parameter, &ptmsi);       

        /* Determine routing basis type */
        if( memcmp(mm_serving_plmn.info.plmn.identity,
                           emm_ctrl_data_ptr->emm_guti.plmn.identity,
                           PLMN_ID_SIZE) == 0)
        {
          /*Same PLMN*/
          rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
            .routing_info.routing_basis =
            RRC_TMSI_OR_PTMSI_SAME_PLMN_AND_DIFFERENT_RA_OR_LA;

        }
        else
        {
          /*Different PLMN*/
          rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
            .routing_info.routing_basis = RRC_TMSI_OR_PTMSI_OF_DIFFERENT_PLMN;
        }  
      }
#endif

      /*-----------------------
       On valid PTMSI
       -----------------------*/
      else if( rrc_cmd->cmd.open_sess_req.cn_domain_id == RRC_PS_DOMAIN_CN_ID &&
               mm_has_valid_ptmsi() == TRUE)
      {
         /* Fill in the PTMSI */
         mm_fill_ptmsi_routing_parameter(
            rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
            .routing_info.routing_parameter, &gmm_stored_ptmsi);

         /* Determine routing basis type */
         if( memcmp(mm_serving_plmn.info.plmn.identity,
                    gmm_stored_rai.plmn_id.identity,
                    PLMN_ID_SIZE) == 0)
         {
            /* PTMSI allocated in the current RA */
            if( (memcmp(mm_serving_plmn.lac.lac,
                        gmm_stored_rai.location_area_code.lac, LAC_SIZE) == 0) &&
                mm_serving_plmn.rac == gmm_stored_rai.routing_area_code )
            {
               rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
                  .routing_info.routing_basis = RRC_LOCAL_TMSI_OR_PTMSI;

            }
            /* PTMSI allocated in different RA */
            else
            {
              rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
               .routing_info.routing_basis =
               RRC_TMSI_OR_PTMSI_SAME_PLMN_AND_DIFFERENT_RA_OR_LA;

            }
         } /* different PLMN */
         else
         {
            rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
                  .routing_info.routing_basis = RRC_TMSI_OR_PTMSI_OF_DIFFERENT_PLMN;

         }
      }
#if defined(FEATURE_LTE)
	  /*
	    *if the TIN is not available and the MS holds a valid GUTI, but no valid P-TMSI, the MS NAS shall provide the lower layers 
	    *with the P-TMSI mapped from the GUTI (see 3GPP TS 23.003 [10]).
	    */
	  else if((mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_LTE_RADIO_ACCESS)) &&
	  	       rrc_cmd->cmd.open_sess_req.cn_domain_id == RRC_PS_DOMAIN_CN_ID &&
               mm_has_valid_ptmsi() == FALSE &&
               emm_ctrl_data_ptr->TIN == NAS_MM_TIN_NONE &&
               emm_ctrl_data_ptr->emm_guti_valid == TRUE)
	  	{
			/*Fill in mapped PTMSI*/
       		emm_map_guti_to_ptmsi(&ptmsi, emm_ctrl_data_ptr);
        
        	mm_fill_ptmsi_routing_parameter(
            	rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
             		.routing_info.routing_parameter, &ptmsi);       

	        /* Determine routing basis type */
    	    if( memcmp(mm_serving_plmn.info.plmn.identity,
                           emm_ctrl_data_ptr->emm_guti.plmn.identity,
                           PLMN_ID_SIZE) == 0)
        	{
          		/*Same PLMN*/
          		rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
            		.routing_info.routing_basis =
            		RRC_TMSI_OR_PTMSI_SAME_PLMN_AND_DIFFERENT_RA_OR_LA;

        	}
        	else
        	{
          		/*Different PLMN*/
          		rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
            			.routing_info.routing_basis = RRC_TMSI_OR_PTMSI_OF_DIFFERENT_PLMN;
        	}  


	  	}
#endif

      else
      {
         /*
          * In response to IMSI paging:
          * In a page respons message,
          * if the mobile identity used is IMSI, then this paging response
          * is due to IMSI paging
         */
         if (rrc_cmd->cmd.open_sess_req.cn_domain_id == RRC_CS_DOMAIN_CN_ID &&
             rrc_cmd->cmd.open_sess_req.nas_msg.value[1] == PAGING_RESPONSE &&
             (rrc_cmd->cmd.open_sess_req.nas_msg.value[8] & 0x07) == IMSI)
         {
            rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
               .routing_info.routing_basis = RRC_IMSI_RESP_TO_IMSI_PAGING;

            mm_fill_imsi_routing_parameter(
               rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
               .routing_info.routing_parameter);

         }
         else
         {
            /* IMSI UE Initiated event */
            rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
               .routing_info.routing_basis = RRC_IMSI_UE_INITIATED_EVENT;

            mm_fill_imsi_routing_parameter(
               rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
               .routing_info.routing_parameter);
         }
      }
   } /* End of Valid IMSI */

   /* Use IMEI if IMSI not available */
   else
   {
      /* IMEI */
      rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
         .routing_info.routing_basis = RRC_IMEI;

      mm_fill_imei_routing_parameter(
         rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
         .routing_info.routing_parameter);
   }

   /* Now fill-in the entered paramter...
      Entered_parameter will be set to TRUE
      if most significant byte of the current
      LAI/RAI is different compared to the
      most significant byte of the LAI/RAI
      stored in SIM. Otherwise it will
      set to FALSE
    */
   if( rrc_cmd->cmd.open_sess_req.cn_domain_id == RRC_CS_DOMAIN_CN_ID)
   {
      if( mm_serving_plmn.info.plmn.identity[0] != mm_location_information.lai[0])
      {
         rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
            .routing_info.entered_parameter = TRUE;
      }
      else
      {
         rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
            .routing_info.entered_parameter = FALSE;
      }
   }
   else /* PS Domain */
   {
      if( mm_serving_plmn.info.plmn.identity[0] != gmm_stored_rai.plmn_id.identity[0])
      {
         rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
         .routing_info.entered_parameter = TRUE;
      }
      else
      {
         rrc_cmd->cmd.open_sess_req.intra_domain_nas_node_selector
         .routing_info.entered_parameter = FALSE;
      }
   }
}
