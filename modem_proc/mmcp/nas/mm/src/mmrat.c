/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmrat.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/29/11   gps     pended stop_mode_req until abort procedure completes
07/08/02   ks      Created file, this supports RAT change procedures.
08/03/02   mks     Added support for Inter-RAT handover/cell reselection
                   procedures
08/13/02   ks      Activating the MM state before changing to new RAT during
                   Manual RAT change mode procedure.
09/26/02   ks      Resetting mm_serv_change_pending to FALSE after performing
                   a change of service mode operation.
10/10/02   mks     Added support for GSM to WCDMA Inter-RAT HO/Cell selection
                   procedures
11/17/02   mks     Added a check to ensure that IMSI detach is performed
                   (during RAT change or service change) only when the
                   currently registered service domain is SYS_SRV_DOMAIN_CS_ONLY
                   or SYS_SRV_DOMAIN_CS_PS.
11/22/02   mks     Now writing the PS & CS Hyper Frame Numbers received during
                   WCDMA mode deactivation to the SIM
10/29/02   ks      Setting the service_domain in RR_ACTIVATION_RSP to the
                   registered service domain, after the Handover/Reselection.
11/07/02   mks     Resetting mm_integrity_protected flag to FALSE after
                   performing Inter-RAT W2G handover
11/14/02   ks      Added support for STOP_MODE processing for DEEP Sleep
                   reason
11/20/02   mks     Added support for deactivating the active RAT before
                   forwarding the PLMN search request to inactive RAT.
12/05/02   jca     Changes to support PS_ONLY<->CS_ONLY mode switches in GSM mode.
12/09/02   mks     Added support for sending service indication with new
                   active RAT information to REG after successful Inter-RAT
                   handover/cell re-selection.
01/10/03   jca     Changes to support CS<->PS<->CS/PS mode switching.
01/10/03   mks     Now sending MMR_STOP_MODE_CNF to REG (if there is a pending
                   stop mode request) upon deactivation of the active RAT.
01/27/03   jca     Changes to support PS_ONLY<->CS/PS mode switching.
                   Changes to support IMSI Detach via Combined GPRS Detach procedure.
01/31/03   mks     Added a fix to avoid sending REG_CNF when RAT change is still
                   pending.
02/10/03   mks     Now clean up mm_serving_plmn.gmm_service_state before resuming
                   on the new RAT.
02/21/03   ks      Added support for STOP_MODE processing for GSM DEEP Sleep.
02/21/03   ks      Setting the correct msg length mm_send_rr_stop_mode_req.
02/27/03   mks     Added support for PS data continuity feature - Now notifying
                   SM and REG about suspension of packet data services. Added
                   support for propagating GPRS registration information to RR
                   in activation response (after successful inter-RAT handover/
                   cell reselection) primitive.
03/07/03   mks     Updated connection and session state variables to reflect the
                   state of the RRC connection during inter-RAT changes.
03/10/03   mks     Now setting service domain to be same as the MS operation
                   mode in RR activation response. This is to take care of the
                   scenario when inter-RAT cell reselection happens before the
                   registration procedure finishes.
03/18/03   mks     Added support for PS continuity feature: Now setting the
                   ps_attach flag in rr_activation_rsp to true if inter-RAT
                   change occurs when MS is executing GMM RAU, service request
                   or attach procedures.
03/25/03   jca     Do not wait for GMM to perform Detach before performing RAT change
                   or service change if GMM service is not currently available.
04/17/03   mks     Reset the mm_nreg_req_received flag upon completion of the
                   MS power-down request
04/22/03   ks      Initializing GMM-LLC and passing RAC/LAC in the
                   mm_send_rr_activation_rsp() to support PS continuity.
04/22/03   mks     Added a fix to avoid sending mmr_service_ind twice to REG
                   during inter-RAT change.
05/23/03   jca     Added support for PS on demand during CS active connection.
06/06/03   jca     Fixed a bug where MMR_REG_CNF generated at wrong point during
                   RAT change commanded by MMR_REG_REQ.
06/11/03   jca     Fixed a bug where MMR_REG_CNF generated at wrong point during
                   service change when MM service not available.
06/16/03   jca     Now consistently ERR_FATAL if attempt to allocate memory fails.
06/19/03   ks      Added support for PS service continuity due to loss of service
                   during Inter RAT changes.
06/20/03   ks      Added Featurization while initializing LLC.
07/10/03   mks     Reset the gmm_rau_is_required flag if MS regained service on
                   the same RAT that it lost service earlier.
07/11/03   mks     If both RAT change and service mode change are pending, first
                   complete the RAT change procedures prior to initiating service
                   mode change procedures.
07/16/03   ks      Initializing the LLC and SM-SNDCP only if the UE is registered
                   in PS during an inter RAT change in mm_send_rr_activation_rsp()
07/16/03   mks     Added support for PS service continuity due to loss of service
                   during Inter RAT changes.
07/29/03   jca     Fixed PS Attach-on-demand problem related to clearing of the
                   mm_serv_change_pending boolean flag.
07/30/03   mks     GMM now transitions out of suspended substate during G2W
                   handover. Now properly handling the data services resumption
                   when MS loses service, suspends data services and regains service
                   on the same RAT.
07/30/03   jca     Clear mm_serv_change_pending flag when changing service domain.
07/31/03   jca     Added input parameter to function mm_activate_state() to indicate
                   whether call to function was due to RAT change.
08/01/03   vdr     Featured out GSM GPRS related vars (FEATURE_GSM_GPRS)
08/22/03   jca     Added call to gmmllc_reinit( GMM_STANDBY_STATE ) on
                   REG-commanded RAT switch from WCDMA to GSM.
09/10/03   mks     Notify Inter-RAT change to SM only upon regaining service on
                   the new RAT.
09/22/03   ks      Resetting the RRC connection info flags during inter RAT.
10/06/03   ks      Removed the gmm state check for GMM_REGISTERED_INITIATED
                   before moving to STANDBY state during inter RAT change.
10/09/03   mks     Added support for data suspension in GMM-REGISTERED.LIMITED-SERVICE.
11/04/03   jca     Treat ACTIVATION_IND as implicit ABORT_IND when in the
                   process of setting up an RR connection.
11/17/03   vdr     Modified processing of the RRC_STOP_WCDMA_MODE_CNF primitive
12/01/03   mks     Now swapping the LAC digits sent to RR in activation response so that
                   first octet in LAC is the most significant octet and the second octet
                   is least significant octet.
01/27/04   vdr     Removed the MM service state check when GMM goes to NO SERVICE
03/01/04   ks      Now calling rr_activate_rsp() only upon receiving the
                   GMMSM_RAT_RESUME_CNF from SM. Also now chaning MM state to
                   IDLE if MM is already connected in case of RESELECTION.
03/02/04   ks      Sending the ACTIVATION_RSP to RR only if it is due to the
                   ACTIVATION_IND.
03/22/04   jca     Do not overwrite GMM state/substate when changing RAT
                   due to receipt of a MMR_PLMN_SEARCH_REQ.
03/31/04   jca     Added additional checks in function mm_rat_handle_srv_status_update()
                   before clearing ps_data_suspend flag.
04/06/04   cd      LCS support
04/06/04   cd      Removed local declaration for mm_rat_handler_status and made it global
04/19/04   jca     Ensure T3212 is running when switching ms op mode from PS to CS_ONLY.
05/21/04   jca     Do not unconditionally reset RRC connection state variables
                   when a RR_STOP_GSM_MODE_CNF is received.
06/02/04   ks      Removed the any_cell_selection_mode field from rrc_activation_rsp
                   primitive as it is no longer needed.
06/08/04   cd      LCS support.
06/18/04   jca     Always treat RRC_ACTIVATION_IND as a lower layer failure condition when
                   a GPRS Attach, Detach, or Routing Area Updating procedure is active.
07/26/04   jca     gmm_rau_is_required now always set to TRUE when GPRS service is lost
                   regardless of the current state of the ps_data_suspend flag.
08/12/04   cd      Bringing changes from IOT testing. Sending a LCS_ABORT_IND after an
                   interRAT HO stop mode cnf is received.
08/12/04   cd      Replaced FEATURE_LCS by FEATURE_MMGPS
10/12/04   jca     Reject interRAT MMR_PLMN_SEARCH_REQ if RR(C) connection
                   is active when request is received.
10/19/04   ks      Added fix to not to reset the rrc_connection_established and
                   rrc_connection_in_progress during a W to G handover.
10/25/04   ks      Do not reset the rrc_connection_established and
                   rrc_connection_in_progress after receiving STOP_MODE_CNF.
11/11/04   jca     Ensure that PS registration attempt has successfully completed
                   on new cell before clearing the ps_data_suspend flag.
11/17/04   jca     Determine if ps_data_suspend flag can be cleared by comparing
                   mm_serving_plmm.rai to gmm_stored_rai (from GSLOCI) instead of
                   comparing to gmm_previous_serving_rai.
11/19/04   jca     Don't store MMR_PLMN_SEARCH_REQ in mmcoord_pending_reg_message
                   structure until it is firmly established that a RAT change
                   will actually occur (i.e., command will not be rejected).
                   If the MMR_PLMN_SEARCH_REQ command is rejected, make sure to
                   clear the mm_rat_change_pending_flag.
12/03/04   ks      Now resetting the RRC connection variables when the UE changes
                   RAT from W to G due to cell change order during packet transmission.
01/12/05   jca     Abort RRC connection when a manual MMR_PLMN_LIST_REQ is received.
02/02/05   jca     Key off gmm_state instead of mm_serving_plmn.info.reg_service_domain
                   in function mm_rat_handle_srv_status_update().
02/18/05   sm      Posting EVENT_RAT_CHANGE incase of RAT changes
03/16/05   abe     Incase of inter RAT reselection from G -> W or W -> G if there is
                   an upper layer connection pending then reject it with cause
                   AS_REJ_LOW_LEVEL_FAIL (instead of AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED )
                   to enable upper for triggering silent redial.
03/18/05   abe     RR(C) connection abort request is sent to the current active RAT
                   when a manual MMR_PLMN_SEARCH_REQ is received.
04/14/05   jca     Added boolean flag mm_waiting_for_stop_mode_cnf to ensure
                   RRC_STOP_WCDMA_MODE_REQ (or RR_STOP_GSM_MODE_REQ) is only
                   sent once before receiving CNF.
04/20/05   jca     Now block Detach procedure due to access class restrictions.
04/25/05   abe     Incase of inter RAT reselection/handover from G -> W or W -> G, not re-setting
                   the active RAT to SYS_RAT_NONE after STOP_MODE_CNF is received from
                   lower layer.
05/04/05   abe     While doing inter-RAT change from W->G query data services to check
                   whether any data is available in SNDCP queue before stoping the W RAT.
05/23/05   abe     Set the value of the field auth_failed_bar_cells to FALSE in RR(C)_ABORT_REQ
06/06/05   abe     Added code for CCBS support
06/17/05   vdr     Added GMM_TRIGGER_TLLI_UNASSIGNMENT
07/20/05   abe     Added support for FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER.
07/26/05   abe     While WtoG CCO in progress, delay sending RAT-CHANGE and RAT_RESUME
                   request to SM till CCO completes successfully. Incase of failure
                   no need to send RAT-CHANGE and RAT-RESUME request to SM and data should
                   resume on its own in W.
07/27/05   ks      Added featurization around CCO code that were missed earlier
08/18/05   jca     Ensure MMR_REG_CNF is first response sent to REG task after a
                   MMR_REG_REQ is received (instead of a MMR_SERVICE_IND).
08/23/05   kvk     Added handling for PS continuity when Manual PLMN search is triggered while
                   PS call is active. Data will suspended after STOP_MODE_CNF is received and
                   mm_service_loss_on_rat will be updated with the current RAT.
09/15/05   jca     No longer unasign TLLI when sending a RR_STOP_GSM_MODE_REQ.
09/16/05   vdr     Stoped sending CNM reject for Emergency calls because of Inter RAT 
                   reselection from W2G
09/23/05   kvk     Removed the check for mm_dual_mode set to TRUE in order to allow PS continuity
                   even UE mode changes to singale mode.
10/06/05   abe     Abnormally terminating LAU after WtoG CCO failed would cause CS key 
                   deleted. To preserve CS keys just stop the LAU timer and bring mm state to IDLE 
10/08/05   abe     Added support for FEATURE_INTERRAT_PCCO_GTOW
10/24/05   up      Send net selection mode in MMR_PLMN_SEARCH_CNF when rejected.
11/02/05   abe     Treat RRC_ACTIVATION_IND as a low level failure even if MM is in state 9
11/04/05   abe     Process the pending MMR_REG_REQ after processing STO_MODE_CNF. 
11/11/05   ss      Updated for Rel5 changes.
11/18/05   kvk     mm_rat_handle_srv_status_update will be called irrespective of PDP is active
                   or not to avoid in consistensy of handling srv status update
12/05/05   vdr     Stoped calling gmmllc_reinit() during handling of _STOP_MODE_CNF
12/28/05   abe     set gmm_ra_update_pending to FALSE while receiving RR_ACTIVATION_IND
                   with status MM_AS_WTOG_INTERRAT_CC_ORDER for not carrying the pending RAU                
01/19/05   abe     Check if there is data in SNDCP queue during G to W inter RAT. This would help in 
                   setting FOR bit in subsequent RAU              
01/18/06   up      transaction id is added in PLMN_SEARCH_CNF.
02/03/06   sn      Allow Manual Search when PDP is active without
                   signaling connection.
03/22/06   ajt     On WtoG CCO failure, set gmm_ra_update_pending to FALSE
                   as this would have been set to TRUE in non-DTM UEs 
                   (GPRS mode B) as MM would have started LU.
05/22/06   ajt     After upper layer directed inter-rat finishes stopping one RAT, 
                   call mmcoord_process_mmr_plmn_search_req to process plmn search request.
                   This is inline with design decision for processing pending reg req due to
                   upper layer directed inter-rat.
07/03/06   pk     re-setting the mm_service_loss_on_rat SYS_RAT_NONE irrespective of
                   ps_data_suspend flag once UE has gained service
07/14/06   pk      Updated for Feature DSAC
07/21/06   up      Updated mm_rat_handle_srv_status_update to set
                   data suspend and rau required flag unconditionally.
11/23/06   sn      Feature FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
04/09/07   rd      SMS optimization changes are incorporated.
05/14/08   abhi   Setting PMM mode to Idle on receiving RRC_WTOG_CCO_COMPLETE_IND
06/30/11   rajesh    Changes made for implementing PS detach timer in NAS.
07/19/11   rajesh    Made changes to process pending list request on receiving stop
                     mode confirmation.
09/21/11   sla      UE will not send activation response if the timers are getting 
                    expired          
10/01/11   abhi    sending mapped/registered RAI to GRR in activation response
10/05/11   gps     Initiated some primitives with all zeros
09/12/11   HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                   as a part of CMI activity 
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
#include "ms.h"
#include "timers_v.h"
#include "gs_v.h"
#include "mm_rr.h"
#include "mm_v.h"
#include "err.h"
#include "gmm_v.h"
#include "rrccmd.h"
#include "mm_umts_v.h"
#include "mm_coord.h"
#include "naslog_v.h"
#include "mmauth.h"
#include "mmrat.h"
#include "gmm_int_v.h"
#include "reg_mm_v.h"
#include "mmtask.h"
#include "mmtask_int.h"
#include "mm_sim.h"
#include "sm_gmm.h"
#include "sm_v.h"

#ifdef FEATURE_GSM_GPRS
#include "gmmllcm.h"
#include "gmmsuspend.h"
#endif
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#include "dsumtsps.h"
#if defined(FEATURE_LTE)
#include "mmsecurity.h"
#include "gmmsecurity.h"
#endif

/*===========================================================================

                    DATA DECLARATIONS

===========================================================================*/

#ifdef FEATURE_SGLTE
extern mm_sglte_stack_activity_status mm_sglte_current_stack_status;
extern void mmsend_mmr_service_stat_ind( mm_as_id_e_type mm_as_id_in);
extern void mmsend_mmr_stack_deact_ind(void);
extern void mmsend_mmr_stack_act_ind(void);
extern void  mmcoord_process_merging(void );
extern void mmcoord_process_spliting(void );
extern void mmcoord_process_pstimer_expiry_merging(void);
extern void mm_copy_cs_database(mm_as_id_e_type dest,mm_as_id_e_type src);
extern boolean mm_is_split_needed(void);
#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

mm_interrat_change_state_type    mm_interrat_state_sim[MAX_NAS_STACKS];
sys_radio_access_tech_e_type     mm_service_loss_on_rat_sim[MAX_NAS_STACKS];

extern boolean data_is_available_sim[MAX_NAS_STACKS];
extern sys_network_selection_mode_e_type mm_waiting_for_list_cnf_type_sim[MAX_NAS_STACKS];

#define  mm_waiting_for_list_cnf_type mm_waiting_for_list_cnf_type_sim[mm_as_id]

#define  mm_service_loss_on_rat mm_service_loss_on_rat_sim[mm_as_id]

#define  data_is_available data_is_available_sim[mm_as_id]
#else

mm_interrat_change_state_type    mm_interrat_state;
sys_radio_access_tech_e_type     mm_service_loss_on_rat;

extern boolean data_is_available;
extern sys_network_selection_mode_e_type mm_waiting_for_list_cnf_type;

#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/

extern void mmcoord_process_mmr_reg_req( mm_cmd_type *message );
extern void mmcoord_process_mmr_plmn_search_req( mm_cmd_type *message );
extern void gmm_handle_gprs_detach_low_level_failure( void );

#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#ifdef FEATURE_MODEM_HEAP
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern byte *gmm_rat_chng_pended_msg_sim[];
#else
extern byte *gmm_rat_chng_pended_msg;
#endif
#endif
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/


#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern boolean pending_gmm_msg_valid_sim[MAX_NAS_STACKS];
extern boolean pending_mm_msg_valid_sim[MAX_NAS_STACKS];
#else
extern boolean pending_gmm_msg_valid;
extern boolean pending_mm_msg_valid;
#endif

#ifdef FEATURE_LTE
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
extern boolean mm_multimode_rat_change_pending_sim[MAX_NAS_STACKS];
#define  mm_multimode_rat_change_pending mm_multimode_rat_change_pending_sim[mm_as_id]
#else
extern boolean mm_multimode_rat_change_pending;
#endif
#endif

#ifdef FEATURE_LTE
#ifdef TEST_FRAMEWORK
#error code not present
#endif
#endif

/*===========================================================================

FUNCTION MM_HANDLE_RAT_CHANGE

DESCRIPTION
  This function deals with the RAT Change procedure, when the UE changes
  the ACTIVE RAT mode from (GSM to UMTS or other way)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_handle_rat_change(mm_cmd_type* msg_p)
{
   mmr_plmn_search_cnf_s_type *mmr_plmn_search_cnf;
#ifdef FEATURE_WRLF_SYSTEM_SEL   
   boolean mplmn_search_during_rlf = FALSE;
#endif

#ifndef FEATURE_MODEM_HEAP
   mmr_plmn_search_cnf = (mmr_plmn_search_cnf_s_type*)gs_alloc(sizeof(mmr_plmn_search_cnf_s_type));
#else
   mmr_plmn_search_cnf = (mmr_plmn_search_cnf_s_type*)modem_mem_calloc(1, sizeof(mmr_plmn_search_cnf_s_type), MODEM_MEM_CLIENT_NAS);
#endif
   mm_check_for_null_ptr((void*)mmr_plmn_search_cnf);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   switch ( msg_p->cmd.hdr.message_set )
   {
      case MS_MM_REG:
         if (msg_p->cmd.hdr.message_id == (int) MMR_REG_REQ)
         {

            if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
                (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
            {
               if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS) &&
                   (msg_p->cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_PS_ONLY) &&
                   (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
               {
                  if (!mm_rat_change_pending)
                  {
                     mm_ready_to_send_reg_cnf = TRUE;
                  }
               }
               else if ((mm_state != MM_IDLE) &&
                        (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) &&
                        (msg_p->cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_CS_PS))
               {
                  if (is_umts_mode())
                  {
                     if (!mm_rat_change_pending)
                     {
                        mm_ready_to_send_reg_cnf = TRUE;
                     }
                  }
                  else if (is_gsm_mode())
                  {
                     mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                     
                     /*setting the attach failed indication to waiting for gmm*/
                     if(mm_mmr_attach_ind_status == ATTACH_FAILED_IND_REQUIRED)
                     {
                       mm_mmr_attach_ind_status = ATTACH_FAILED_IND_WAITING_FOR_GMM;
                     }
                     mm_send_mmr_reg_cnf();
                     if(mm_mmr_attach_ind_status != ATTACH_FAILED_IND_NOT_REQUIRED)
                     {
                       mm_mmr_attach_ind_status = ATTACH_FAILED_IND_REQUIRED;
                     }
                     mm_reg_waiting_for_reg_cnf = FALSE;
                     mm_ready_to_send_reg_cnf   = FALSE;
                  }
               }
               else if ((msg_p->cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_CS_ONLY) &&
                        (mm_serving_plmn.mm_service_state  != SERVICE_NOT_AVAILABLE) &&
                        (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE) &&
                        (!mm_rat_change_pending) && (!mm_serv_change_pending))
               {
                  mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
                  mm_send_mmr_reg_cnf();
                  mm_reg_waiting_for_reg_cnf = FALSE;
                  mm_ready_to_send_reg_cnf   = FALSE;
               }
               else if (mm_state == MM_IDLE )
               {
                  mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_NONE);               
                  if ((mm_system_information.ATT) &&
                      (mm_check_if_cs_cell_access_allowed(cs_cell_access) == TRUE) &&
                      ((mm_idle_substate == MM_NULL_SUBSTATE) ||
                       (mm_idle_substate == MM_LOCATION_UPDATE_NEEDED) ||
                       (mm_idle_substate == MM_PLMN_SEARCH_NORMAL_SERVICE) ||
                       (mm_idle_substate == MM_NORMAL_SERVICE)))
                  {
                     if (msg_p->cmd.mmr_reg_req.req_service_domain ==
                           SYS_SRV_DOMAIN_PS_ONLY)
                     {
                        /* -----------------------------------------------------
                        ** Send an IMSI DETACH INDICATION message to the network
                        ** ----------------------------------------------------- */
                        mm_state_control( MM_STD_MS_DEACTIVATED_ATTACH_ALLOWED );

                        /* ------------------------------------
                        ** An IMSI attach may be required later
                        ** ------------------------------------ */
                        mm_possible_imsi_attach = TRUE;
                     }
                     else
                     {
                        if (!mm_rat_change_pending)
                        {
                           mm_ready_to_send_reg_cnf = TRUE;
                        }
                     }
                  }
                  else
                  {  
                     if ((msg_p->cmd.mmr_reg_req.req_service_domain ==
                           SYS_SRV_DOMAIN_PS_ONLY) &&
                         (mm_serving_plmn.mm_service_state  == SERVICE_NOT_AVAILABLE) &&
                         (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE)    )
                     {
                       mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                    SYS_SRV_DOMAIN_NO_SRV,
                                    FALSE,
                                    mm_serving_plmn.info.service_status,
                                    FALSE );
#ifdef FEATURE_SGLTE
                       if(!MM_IS_IN_SGLTE_MODE )
#endif
					   {
                         mm_possible_imsi_attach = TRUE;
					   }
                     }
                     if (!mm_rat_change_pending)
                     {
                        mm_ready_to_send_reg_cnf = TRUE;
                     }
                  }
               }
               else if (mm_state == MM_WAIT_FOR_NETWORK_COMMAND)
               {
                  mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_NONE);               
                  if ((mm_rat_change_pending ||
                      (msg_p->cmd.mmr_reg_req.req_service_domain ==
                          SYS_SRV_DOMAIN_PS_ONLY)) &&
                      (mm_system_information.ATT) &&
                      (mm_check_if_cs_cell_access_allowed(cs_cell_access) == TRUE))
                  {
                     /* -----------------------------------------------------
                     ** Send an IMSI DETACH INDICATION message to the network
                     ** ----------------------------------------------------- */
                     mm_state_control( MM_STD_MS_DEACTIVATED_ATTACH_ALLOWED );

                     /* ------------------------------------
                     ** An IMSI attach may be required later
                     ** ------------------------------------ */
                     mm_possible_imsi_attach    = TRUE;
                  }
                  else
                  {
                     if (!mm_rat_change_pending)
                     {
                        mm_ready_to_send_reg_cnf = TRUE;
                     }
                  }
               }
#ifdef FEATURE_CCBS
               else if (mm_state == MM_PROCESS_CM_SERVICE_PROMPT)
               {
                  mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_NONE);               
                  if ((mm_rat_change_pending ||
                      (msg_p->cmd.mmr_reg_req.req_service_domain ==
                          SYS_SRV_DOMAIN_PS_ONLY)) &&
                      (mm_system_information.ATT) &&
                      (mm_check_if_cs_cell_access_allowed(cs_cell_access) == TRUE))
                  {
                     /* -----------------------------------------------------
                     ** Send an IMSI DETACH INDICATION message to the network
                     ** ----------------------------------------------------- */
                     mm_state_control( MM_STD_MS_DEACTIVATED_ATTACH_ALLOWED );

                     /* ------------------------------------
                     ** An IMSI attach may be required later
                     ** ------------------------------------ */
                     mm_possible_imsi_attach    = TRUE;
                  }
                  else
                  {
                     if (!mm_rat_change_pending)
                     {
                        mm_ready_to_send_reg_cnf = TRUE;
                     }
                  }
               }
#endif /* FEATURE_CCBS */

               else if (mm_state == MM_WAIT_FOR_RR_ACTIVE)
               {
                  mm_state_control( MM_STD_MS_DEACTIVATED_NO_ATTACH );
               }
               else if ((mm_state == MM_CONNECTION_ACTIVE ) ||
                        (mm_state == MM_LOCATION_UPDATE_INITIATED ) ||
                        (mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION ) ||
                        (mm_state == MM_REESTABLISHMENT_INITIATED ) ||
                        (mm_state == MM_IMSI_DETACH_INITIATED ) ||
                        (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU))
               {
                  if (!mm_rat_change_pending)
                  {
                     mm_ready_to_send_reg_cnf = TRUE;
                  }

               }

               if ((mm_state == MM_IDLE) &&
                   ((gmm_state == GMM_NULL) || (gmm_state == GMM_DEREGISTERED) ||
                    (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE)))
               {
                  if (mm_rat_change_pending)
                  {
                    if (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY)
                    {
                     /* -------------------------------------------------------
                     ** GMM is now in a state where MM can take further actions
                     ** ------------------------------------------------------- */
                     /* ----------------------------------------
                     ** Send a deactivation command to RR or RRC
                     ** (only deactivate on power down)
                     ** ---------------------------------------- */
                      mm_state_control( MM_STD_MS_DEACTIVATED_NO_ATTACH );
                    }
                    else
                    {
                      mm_stop_current_rat();
                    }
                  }
                  else if (mm_serv_change_pending)
                  {
                     mm_change_service_domain();
                     mm_serv_change_pending = FALSE;
                  }
               }
            }
            else if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) ||
                     (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_NO_SRV))
            {
               if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) &&
                   (msg_p->cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_CS_PS) &&
                   (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1))
               {
                  if (!mm_rat_change_pending)
                  {
                     mm_ready_to_send_reg_cnf = TRUE;
                  }
               }
               else if ((gmm_state == GMM_NULL) || (gmm_state == GMM_DEREGISTERED) ||
                        (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE))
               {
                  if (mm_rat_change_pending)
                  {
                     /* -------------------------------------------------------
                     ** GMM is now in a state where MM can take further actions
                     ** ------------------------------------------------------- */
                     /* ----------------------------------------
                     ** Send a deactivation command to RR or RRC
                     ** (only deactivate on power down)
                     ** ---------------------------------------- */
                     mm_state_control( MM_STD_MS_DEACTIVATED_NO_ATTACH );
                  }
                  else if (mm_serv_change_pending)
                  {
                     if (msg_p->cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_CS_ONLY)
                     {
                        if((gmm_state != GMM_NULL) && (gmm_state != GMM_DEREGISTERED))
                        {
                          sm_cmd_type *sm_cmd;
                          sm_cmd = sm_get_cmd_buf();
                          if (sm_cmd == NULL)
                          {
                             MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                          }
                          sm_cmd->header.message_set = MS_GMM_SM;
                          sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
                          sm_cmd->cmd.gmm_gprs_rel_ind.cause = NO_GPRS_CONTEXT;
#ifdef FEATURE_DUAL_SIM
                          sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif
                          MSG_HIGH_DS_0(MM_SUB, "=MM= Sending GMMSM_GPRS_RELEASE_IND");
                          sm_put_cmd( sm_cmd );
                        }
                        gmm_set_state( GMM_NULL );
                        gmm_set_substate( GMM_NULL_SUBSTATE );

                        if((mm_idle_substate == MM_NORMAL_SERVICE) ||
                           (mm_idle_substate == MM_ATTEMPTING_TO_UPDATE))
                        {
#ifdef FEATURE_NAS_REL10
                          if(mm_is_per_ms_t3212_present)
                          {
                            if((mm_stored_per_ms_t3212_value != 0) &&
                               (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                            {
                               mm_start_timer( TIMER_T3212,
                                                          mm_stored_per_ms_t3212_value);
                            }
                          }
                          else
#endif
                          if ((mm_system_information.T3212_timer_value != 0 ) &&
                              (mm_timer_status[TIMER_T3212] == TIMER_STOPPED))
                          {
                               mm_start_timer( TIMER_T3212,
                                                          mm_system_information.T3212_timer_value );
                          }
                        }

                     }
                     mm_change_service_domain();
                     mm_serv_change_pending = FALSE;
                  }
               }
            }
         }
         else if (msg_p->cmd.hdr.message_id == (int) MMR_PLMN_SEARCH_REQ)
         {
#ifdef FEATURE_WRLF_SYSTEM_SEL
            mplmn_search_during_rlf = (mm_wcdma_rlf_state != WCDMA_RLF_IDLE && 
                      msg_p->cmd.mmr_plmn_search_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL &&
                       msg_p->cmd.mmr_plmn_search_req.service_search == SYS_SERVICE_SEARCH_NONE);
#endif
            if (((rrc_connection_status != MMRRC_IDLE) ||
#ifdef FEATURE_DUAL_SIM
                (sm_is_pdp_active((sys_modem_as_id_e_type)mm_sub_id) &&
#else
                (sm_is_pdp_active() &&
#endif
                 (msg_p->cmd.mmr_plmn_search_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC))) 
#ifdef FEATURE_WRLF_SYSTEM_SEL
                &&(mplmn_search_during_rlf == FALSE)
#endif
                )
               {
                 if (msg_p->cmd.mmr_plmn_search_req.network_selection_mode ==
                       SYS_NETWORK_SELECTION_MODE_MANUAL)
                 {
                    /* If PS session is active or RRC connection active, Abort it */

                    if(rrc_connection_status != MMRRC_RELEASE_IN_PROGRESS)
                    {
                       if (is_umts_mode())
                       {
                         rrc_cmd_type *rrc_cmd_p = NULL;

#ifdef FEATURE_WCDMA
                         rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );
#endif /* FEATURE_WCDMA */

                         if(rrc_cmd_p == NULL) 
                         {
                           MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
                         }
                         /* Send an Abort Request to RRC */
                         rrc_cmd_p->cmd_hdr.cmd_id = RRC_ABORT_REQ;
                         rrc_cmd_p->cmd.rrc_abort_req.delay_sig_con_release = TRUE;
                         rrc_cmd_p->cmd.rrc_abort_req.auth_failed_bar_cells = FALSE;
                         rrc_cmd_p->cmd.rrc_abort_req.hard_abort = FALSE;
                         rrc_cmd_p->cmd.rrc_abort_req.abort_cause = RRC_OTHERS;
                         send_cmd_to_rrc( rrc_cmd_p, mm_serving_plmn.info.active_rat );
                      }
                      else if (is_gsm_mode())
                      {
                         MSG_HIGH_DS_0( MM_SUB, "=MM= MM sent RR_ABORT_REQ");
                         mm_send_rr_abort_req(FALSE);
                      }
                      else
                      {
                         MSG_HIGH_DS_0(MM_SUB, "=MM= Unknown RAT, no RR(C) Deactivation needed");
                      }
                    }

                    /* ---------------------------------------------------------------------
                    ** Store the command. UE needs to access this command after a RAT change
                    ** --------------------------------------------------------------------- */
                    mm_plmn_list_req_pending = TRUE;
                    mmcoord_reg_req_pending = FALSE;
                    mmcoord_pending_reg_list_req_message.cmd.hdr = msg_p->cmd.hdr;
                    mmcoord_pending_reg_list_req_message.cmd.mmr_plmn_search_req =
                       msg_p->cmd.mmr_plmn_search_req;
                 }
                 else
                 {
                    MSG_HIGH_DS_0(MM_SUB, "=MM= AUTO PLMN_LIST_REQ in connected mode, return");
#if defined (FEATURE_WTOG_BACKGROUND_PLMN_SEARCH) || defined (FEATURE_WTOW_BACKGROUND_PLMN_SEARCH)
                    mmr_plmn_search_cnf->network_selection_mode = msg_p->cmd.mmr_plmn_search_req.network_selection_mode;
#endif
                    mmr_plmn_search_cnf->plmn_search_result = REG_MM_PLMN_SEARCH_ABORTED;
                    mmr_plmn_search_cnf->found_plmn_list.plmn_list.length = 0;
                    mmr_plmn_search_cnf->transaction_id = msg_p->cmd.mmr_plmn_search_req.transaction_id;
#ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
                    mmr_plmn_search_cnf->service_search = msg_p->cmd.mmr_plmn_search_req.service_search;
#endif
                    mm_send_mmr_plmn_search_cnf( mmr_plmn_search_cnf );
                    mm_rat_change_pending = FALSE;
                 }

               }
            else
            {
               /* -----------------------------------------------------------------------
               ** Store the command. UE needs to access this command after a RAT change
               ** ----------------------------------------------------------------------*/

               mm_plmn_list_req_pending = TRUE;
               mmcoord_pending_reg_list_req_message.cmd.hdr = msg_p->cmd.hdr;
               mmcoord_pending_reg_list_req_message.cmd.mmr_plmn_search_req =
                  msg_p->cmd.mmr_plmn_search_req;

               /* ------------------
               ** Perform RAT change
               ** ------------------ */
               mm_state_control( MM_STD_MS_DEACTIVATED_NO_ATTACH );
            }
         }
         else
         {
            MSG_ERROR_DS( MM_SUB, "=MM= Unexpected REG message id %d", msg_p->cmd.hdr.message_id, 0,0 );
         }
         break;

      case MS_MM_RR:
      case MS_MM_RRC:
      case MS_MM_TDSRRC:
         mm_handle_inter_rat_change(msg_p);
         break;

      default:
         MSG_ERROR_DS( MM_SUB, "=MM= Unexpected message set %d", msg_p->cmd.hdr.message_set, 0,0 );
         break;
   }
#ifndef FEATURE_MODEM_HEAP
   gs_free(mmr_plmn_search_cnf);
#else
   modem_mem_free(mmr_plmn_search_cnf, MODEM_MEM_CLIENT_NAS);
#endif

}


/*===========================================================================

FUNCTION MM_STOP_GSM_MODE

DESCRIPTION
  This function requests RR deactivation
  done when the MS is powered down

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_stop_rat_mode( void )
{
  /* ----------------------------------------------------
  ** Send STOP MODE REQUEST to RRC if UE is in UMTS mode
  ** ---------------------------------------------------- */
  if (is_umts_mode())
  {
#ifdef FEATURE_SGLTE
   if (MM_IS_IN_SGLTE_MODE && 
       (mm_wcdma_rlf_state == WCDMA_RLF_STARTED))
   {
     mm_send_rrc_abort_req_during_wrlf();
   }
   else
#endif
   {
     mm_send_rrc_stop_mode_req( RRC_USER_RAT_CHANGE,
                               mm_serving_plmn.info.active_rat );
   }
  }
  /* ----------------------------------------------------
  ** Send STOP MODE REQUEST to RR if UE is in GSM mode
  ** ---------------------------------------------------- */
  else if (is_gsm_mode())
  {
#ifdef FEATURE_SGLTE
    if(MM_SUB_IS_SGLTE)
    {
      MSG_HIGH_DS_2(MM_SUB, "=MM= stop rat mode MM_IS_IN_SGLTE_MODE =%d mm_sglte_current_stack_status  =%d ",MM_IS_IN_SGLTE_MODE,
                                         mm_sglte_current_stack_status);
    }
    if (MM_IS_IN_SGLTE_MODE && (mm_sglte_current_stack_status == SGLTE_MM_UPPER_LAYER_SPLITING))
    {
      /* changing stop mode reason to RR_SGLTE_USER_RAT_CHANGE so that RR 
           stop any ongoing interrat activity similar to user rat change*/
      mm_send_rr_stop_mode_req(RR_SGLTE_USER_RAT_CHANGE);
    }
    else
#endif
    {
      mm_send_rr_stop_mode_req(RR_MM_USER_RAT_CHANGE);
    }
  }
}

/*===========================================================================

FUNCTION MM_CHANGE_SERVICE_DOMAIN

DESCRIPTION
  This function is called after a successful IMSI/GPRS DETACH procedure in
  the current RAT and during a change of Service domain (CS to PS or PS to CS).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_change_service_domain( void )
{
  if (!mm_serv_change_pending)
  {
    MSG_ERROR_DS( MM_SUB, "=MM= No Service Change Pending",0,0,0);
  }
  /* -------------------------------------------------------------------
  ** If RAT change is pending, first complete the RAT change procedures
  ** before attempting to process service mode change.
  ** ------------------------------------------------------------------- */
  else if (mm_rat_change_pending)
  {
    mm_stop_current_rat();
  }
  else
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= Changing Service to %d (1=CS/2=PS)",
               mmcoord_pending_reg_message.cmd.mmr_reg_req.req_service_domain);

    mmcoord_process_lu_procedure( &mmcoord_pending_reg_message );
  }
}


/*===========================================================================

FUNCTION MM_HANDLE_INTER_RAT_CHANGE

DESCRIPTION
  This function handles inter-RAT transitions

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_handle_inter_rat_change (mm_cmd_type *msg_p)
{
  switch (msg_p->cmd.hdr.message_id)
  {
    case RR_ACTIVATION_IND:
    case RRC_ACTIVATION_IND:
      mm_handle_activation_ind(msg_p);
      break;
    case RRC_STOP_WCDMA_MODE_CNF:
    case RR_STOP_GSM_MODE_CNF:
      mm_handle_stop_mode_cnf(msg_p);
      break;

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
    case RRC_WTOG_CCO_COMPLETE_IND:
      /* MM receives this primitive once WtoG Cell Change Order is
         successfully completed. That means once RACH is successful on G side
         then only G informs W that WtoGCCO is successful and then RRC sends this
         primitive to MM. After receiving the RR_ACTIVATION_IND indicating WTOG CCO,
         RRC connection and session variables are retained in MM. Now reset all of them.
         Also reset the variable WtoGCCO_in_progress indicating that WtoG CCO is finished.
      */

      MSG_HIGH_DS_0(MM_SUB,"=MM= MM received RRC_WTOG_CCO_COMPLETE_IND");

      /* Now we need to send the RAT chnage and RAT resume req */

      if ( msg_p->cmd.hdr.message_set == MS_MM_TDSRRC
#ifdef FEATURE_SEGMENT_LOADING
&&get_tdscdma_interface()
#endif
)
      {
        gmm_sm_send_rat_change_req( SYS_RAT_TDS_RADIO_ACCESS
#if defined(FEATURE_LTE)
                                  , FALSE
#endif
                                  );
      }
      else
      {
        gmm_sm_send_rat_change_req (SYS_RAT_UMTS_RADIO_ACCESS
                                                 #if defined(FEATURE_LTE)
                                                           , FALSE
                                                 #endif
                                                              );
      }
#ifdef FEATURE_NAS_FREE_FLOAT
      gmm_pended_rat_resume_req = TRUE;
      gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_G_RAT_RESUMED,FALSE,0);
#else
      gmm_sm_send_rat_resume_req();
#endif

#ifdef FEATURE_SGLTE
      if (MM_IS_NOT_IN_SGLTE_MODE)
#endif
      {
        if(mm_state != MM_WAIT_FOR_NETWORK_COMMAND && mm_state != MM_LOCATION_UPDATE_INITIATED)
        {
         rrc_connection_status = MMRRC_IDLE;
        }
      }

      /* Stop umts PS timer for CCO */
      mm_stop_timer(TIMER_T3340);
      cs_session_status = MMRRC_IDLE;
      ps_session_status = MMRRC_IDLE;


      gmm_ps_signalling_connected = FALSE;
      gmm_integrity_protection_activated = FALSE;
      pmm_mode = PMM_IDLE;

      WtoGCCO_state = CCO_INACTIVE;

      break;
 #endif /* FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER */
  
#ifdef FEATURE_INTERRAT_PCCO_GTOW
    case RR_GTOW_CCO_COMPLETE_IND:    
     MSG_HIGH_DS_0(MM_SUB,"=MM= MM received RR_GTOW_CCO_COMPLETE_IND");
     
     /* Now we need to send the RAT chnage and RAT resume req */
      
     gmm_sm_send_rat_change_req(SYS_RAT_GSM_RADIO_ACCESS
                                #if defined(FEATURE_LTE)
                                , FALSE
                                #endif
                                );
     
     gmmllc_reinit(GMM_IDLE_STATE) ;
     
     GtoWCCO_state = CCO_INACTIVE;
     break;
#endif /* FEATURE_INTERRAT_PCCO_GTOW */ 

    default:
      MSG_ERROR_DS( MM_SUB, "=MM= MM received Unexpected RR Cmd", msg_p->cmd.hdr.message_id,0,0 );
      break;
  }
} /* mm_handle_inter_rat_transition */


/*===========================================================================

FUNCTION MM_HANDLE_ACTIVATION_IND

DESCRIPTION
  This function handles ACTIVATION_IND message from RR/RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_handle_activation_ind (mm_cmd_type *msg_p)
{
  mmr_service_ind_s_type       mmr_service_ind;
  sys_radio_access_tech_e_type previous_active_rat = SYS_RAT_NONE;

  if (mm_nreg_req_received )
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= MMR_STOP_MODE_REQ pending - Received ACTIVATION_IND");
  }

  if (mm_rat_change_pending)
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= RAT change pending - Ignoring ACTIVATION_IND");
  }
  else if(mm_waiting_for_abort_cnf && mm_serving_plmn.info.active_rat == SYS_RAT_TDS_RADIO_ACCESS)
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= ABORT cnf pending in TDS - Ignoring ACTIVATION_IND");
  }
  else if (mm_waiting_for_deact_cnf)
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= Deactivation pending - Ignoring ACTIVATION_IND");
  }
#ifdef FEATURE_SGLTE
  else if(!mm_sglte_activation_ind_allowed() && (msg_p->cmd.hdr.message_id == RR_ACTIVATION_IND))
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= SGLTE mode - ACTIVATION_IND not allowed");
  }
#endif
  else if (mm_waiting_for_stop_mode_cnf)
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= RAT Stop request pending - Ignoring ACTIVATION_IND");
  }
  else if (mm_waiting_for_service_cnf)
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= Service or PLMN select request pending - Ignoring ACTIVATION_IND");
  }
  else if ((rrc_connection_status == MMRRC_IDLE) &&
           (mm_waiting_for_list_cnf_type == SYS_NETWORK_SELECTION_MODE_MANUAL)
#ifdef FEATURE_GTOW_PS_HANDOVER
           && !((msg_p->cmd.hdr.message_id == RRC_ACTIVATION_IND) && (msg_p->cmd.rrc_activation_ind.status == MM_AS_INTERRAT_PS_HANDOVER))
#endif 
          )
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= Waiting for LIST CNF - Manual in Idle mode - Ignoring ACTIVATION_IND");
  }
  else
  {
    switch (msg_p->cmd.hdr.message_id)
    {
      case RR_ACTIVATION_IND:
        MSG_HIGH_DS_1(MM_SUB ,"=MM= MM received RR_ACTIVATION_IND with status %d", msg_p->cmd.rr_activation_ind.status);
        
        mm_serv_change_pending = FALSE;
        mm_to_only_reg         = TRUE;        
        mm_previous_serving_rat     = mm_serving_plmn.info.active_rat;
        previous_active_rat = mm_serving_plmn.info.active_rat;
        mm_serving_plmn.info.active_rat = SYS_RAT_GSM_RADIO_ACCESS;
        mm_initiate_lu_upon_entry_into_new_cell = FALSE;
        mm_coord_camped_cell_id = 0xFFFF;
        gmm_extend_connection_due_to_mt_csfb = FALSE;

        if(mm_waiting_for_abort_cnf == TRUE)
        {
          mm_waiting_for_abort_cnf = FALSE;
        }

            /* ------------------------------------------------------
         ** Remember that deactivation of WCDMA RAT is in progress
         ** ------------------------------------------------------ */

        if (previous_active_rat == SYS_RAT_TDS_RADIO_ACCESS)
        {
          mm_rat_handler_status = MM_RAT_STOP_TDSCDMA_IN_PROGRESS;
        }
        else
        {
          mm_rat_handler_status = MM_RAT_STOP_WCDMA_IN_PROGRESS;
        }

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER

        /*  Incase of WtoG CCO, NAS should retain the RRC session/connection info
           till the RACH is successful on G. Once RACH is successful RRC send
           RRC_WTOG_CCO_COMPLETE_IND and MM should clear the
           RRC session and connection info */

        if (msg_p->cmd.rr_activation_ind.status == MM_AS_WTOG_INTERRAT_CC_ORDER)
        {
          /* WtoGCCO will only receive only when PMM is in connected state */

          if (pmm_mode == PMM_CONNECTED)
          {
            /* This flag gets reset once RR_EST_CNF is received or
               RRC_ACTIVATION_IND with cause WtoGCCO_failed.  */

            WtoGCCO_state = CCO_IN_PROGRESS;

            /* Reject any pending CM service request */
            if(mm_held_cnm_service_req.present)
            {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
               mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL , MMCC_SMS_CS);
#else
               mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL );
#endif
            }

            mm_state_control( MM_STD_LOW_LEVEL_FAIL );
            /* Incase of any pending RAU, clear it here. Otherwise it would cause 
               triggering RAU in the function gmm_handle_connection_management which is 
               not desired. Anyway RAU would be triggered once we receive RR_SERVICE_IND 
               after CCO
            */
            gmm_ra_update_pending = FALSE;
            gmm_handle_connection_management( msg_p );

          }
          else
          {
            MSG_HIGH_DS_0(MM_SUB,"=MM= Error!! WtoGCCO received not in PMM connected mode");
          }
        }
        else
#endif /* FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER */

        if ((
#ifdef FEATURE_WTOG_PS_HANDOVER
        #error code not present
#endif
         (msg_p->cmd.rr_activation_ind.status == MM_AS_SGLTE_INTER_RAT_RESELECTION)||
        (msg_p->cmd.rr_activation_ind.status == MM_AS_INTERRAT_RESELECTION) ||
            (msg_p->cmd.rr_activation_ind.status == MM_AS_INTERRAT_CC_ORDER)) &&
            (rrc_connection_status != MMRRC_IDLE))
        {
          if(rrc_connection_status == MMRRC_EST_IN_PROGRESS)
          {
            if(gmm_state == GMM_REGISTERED_INITIATED)
            {
              gmm_set_substate( GMM_ATTACH_NEEDED );
              gmm_set_state( GMM_DEREGISTERED );
            }
            else if(gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
            {
              gmm_set_substate( GMM_UPDATE_NEEDED );
              gmm_set_state( GMM_REGISTERED );
            }
          }

          rrc_connection_status = MMRRC_IDLE;
          pending_mm_msg_valid   = FALSE;
          pending_gmm_msg_valid  = FALSE;

/*24.008: 4.7.5.1
The normal routing area updating procedure is initiated: in A/Gb mode, after Inter RAT handover from S1 mode or Iu mode. Set RAU initiation flag to true on handover to G-rat*/       
#ifdef FEATURE_WTOG_PS_HANDOVER
#error code not present
#endif

          /* Reject any pending CM service request */
          if ((mm_held_cnm_service_req.present) && 
              (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL))
          {
             /* mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL); */

             mm_held_cnm_service_req.interat_status = msg_p->cmd.rr_activation_ind.status ;
          }
          else
          {
            if(mm_pending_lu_request != NO_LU ||
               mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
            {
              mm_substate_control(MM_STD_UPDATE_PENDING);
              mm_pending_lu_request = NO_LU;

            if(mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
            {
               mm_lu_reject_info.reject_cause = CAUSE_LAI_CHANGED;
            }
            }
            mm_state_control( MM_STD_LOW_LEVEL_FAIL );
            gmm_handle_connection_management( msg_p );
          }
        }

#ifdef FEATURE_INTERRAT_PCCO_GTOW
        if (msg_p->cmd.rr_activation_ind.status == MM_AS_GTOW_CC_ORDER_FAILED)
        {
          /* Reject any pending CM service request */

          if(mm_held_cnm_service_req.present)
          {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
             mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL , MMCC_SMS_CS);
#else
             mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL );
#endif
          }
      
          /* If LAU is stared incase of GtoW and GtoW failed then 
             bring back the mm state to Idle and stop LAU timer. 
          */
          if (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
          {
            mm_state_control( MM_STD_CCO_FAILED );
            mm_stop_timer( TIMER_T3210 );
          }
          else if(mm_state == MM_WAIT_FOR_RR_CONNECTION_MM)
          {
            mm_state_control( MM_STD_LOW_LEVEL_FAIL );
          }
              
          /* As PCCO failed, RAU had initiated which caused GMM state change to 
            RAU_INITIATED. Now bring back to REGISTERED 
          */
          if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
          {
            gmm_set_state( GMM_REGISTERED );
            mm_stop_timer( TIMER_T3330 );
          }
            
          rrc_connection_status = MMRRC_IDLE;
          pending_mm_msg_valid   = FALSE;
          pending_gmm_msg_valid  = FALSE;
          
          GtoWCCO_state = CCO_FAILED;
        }
#endif /* FEATURE_INTERRAT_PCCO_GTOW */  
        if (msg_p->cmd.rr_activation_ind.status == MM_AS_INTERRAT_HANDOVER)
        {
          if ((gmm_state == GMM_REGISTERED_INITIATED) ||
              (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) ||
              (gmm_state == GMM_DEREGISTERED_INITIATED) ||
             (gmm_state == GMM_SERVICE_REQUEST_INITIATED))
          {
            gmm_handle_connection_management( msg_p );
          }
        }
        if ((gmm_state != GMM_NULL) && (gmm_state != GMM_DEREGISTERED))
        {
          /* If CCO is in progress the don't send the RAT-CHANGE req to SM.
             Becaue this will cause RABM to Release the RAB and once CCO failed
             then in W RABM has to REESTABLISH the RAB which is not proper for this
             feature. SM will be notified if CCO is successful (upon receiving
             RRC_WTOG_CCO_COMPLETE_IND.
          */
#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
          if ( WtoGCCO_state == CCO_INACTIVE )
#endif /* FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER */
          {
#ifdef FEATURE_INTERRAT_PCCO_GTOW
            if ( GtoWCCO_state == CCO_INACTIVE )
#endif /* FEATURE_INTERRAT_PCCO_GTOW */
            {
              /* --------------------------------------------------
              ** Notify RAT change to Session Mangement Entity
              ** -------------------------------------------------- */
              gmm_sm_send_rat_change_req( previous_active_rat
#if defined(FEATURE_LTE)
                                        , FALSE
#endif
                                         );
            }
          }
#ifdef FEATURE_WTOG_PS_HANDOVER
          #error code not present
#endif
          {
#ifdef FEATURE_INTERRAT_PCCO_GTOW
            if ( GtoWCCO_state == CCO_INACTIVE )
            {
              /* ---------------------------------------------
              ** Remember that PS data services are suspended
              ** --------------------------------------------- */
              mm_serving_plmn.info.ps_data_suspend = TRUE;
              MSG_HIGH_DS_0(MM_SUB,"=MM= Suspending Packet Data services");
            }
            else 
            {
              /* ---------------------------------------------
              ** Remember that PS data services are suspended
              ** --------------------------------------------- */
              mm_serving_plmn.info.ps_data_suspend = FALSE;
              MSG_HIGH_DS_0(MM_SUB,"=MM= Resuming Packet Data services after GtoW CCO Failed");          
            }
 #else 
            /* ---------------------------------------------
            ** Remember that PS data services are suspended
            ** --------------------------------------------- */
            mm_serving_plmn.info.ps_data_suspend = TRUE;
            MSG_HIGH_DS_0(MM_SUB,"=MM= Suspending Packet Data services");
 #endif /* FEATURE_INTERRAT_PCCO_GTOW */
 
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS) 
            /* Check if there is data in SNDCP queue during RAT change */
            data_is_available = dsumtsps_is_ps_data_available();
#endif
          }
        }

        mm_serving_plmn.info.active_rat = SYS_RAT_GSM_RADIO_ACCESS;

        /* --------------------------
        ** Notify RAT change to REG
        ** -------------------------- */
        if (mm_serving_plmn.info.ps_data_suspend == TRUE)
        {
          mm_send_cm_ps_data_suspend_ind(); 
        }
        
        if (mm_reg_waiting_for_reg_cnf)
        {
          if (mm_ready_to_send_reg_cnf)
          {
            mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
            mm_send_mmr_reg_cnf();

            mm_reg_waiting_for_reg_cnf = FALSE;
            mm_ready_to_send_reg_cnf   = FALSE;
          }
        }
        else
        {
          if ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                   SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
              ((mm_timer_status[TIMER_T3211] == TIMER_ACTIVE) || 
               (mm_timer_status[TIMER_T3311] == TIMER_ACTIVE)))
          {
            mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
          }
          else
          {
            mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
          }          
          mmr_service_ind.service_state = mm_serving_plmn.info;
          mm_send_mmr_service_ind( &mmr_service_ind );
        }

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
        /* ---------------------------------------------------------
        ** Perform cleanup of RRC connection/session state variables
        ** if WtoGCCO not in progress
        ** --------------------------------------------------------- */
        if ( WtoGCCO_state == CCO_INACTIVE )
        {
          cs_session_status = MMRRC_IDLE;
          ps_session_status = MMRRC_IDLE;
        }

        /* Retain the PMM mode if WtoGCCO is in progress */

        if ((pmm_mode == PMM_CONNECTED) &&
            (WtoGCCO_state == CCO_INACTIVE))
        {
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
          gmm_is_prev_mode_connected = TRUE;
          MSG_HIGH_DS_1(MM_SUB ,"=MM= GMM Previous mode Ready or Connected = %d", gmm_is_prev_mode_connected);
#endif /* FEATURE_WCDMA */
          pmm_mode = PMM_IDLE;
          MSG_HIGH_DS_0(MM_SUB, "=MM= pmm_mode = PMM_IDLE");
          mm_stop_timer( TIMER_T3319 );
        }

        /* -------------------------
        ** Deactivate the WCDMA RAT
        ** ------------------------- */
        if (WtoGCCO_state == CCO_IN_PROGRESS)
        {
#ifdef FEATURE_SGLTE
          if(MM_IS_IN_SGLTE_MODE )
          {
            MSG_HIGH_DS_0(MM_SUB,"=MM= SGLTE CCO starting merging..");
            mmsend_mmr_stack_deact_ind();
            mmcoord_process_merging();
            mmsend_mmr_service_stat_ind(mm_ps_stack_id);
        }
#endif
          mm_send_rrc_stop_mode_req( RRC_WTOG_CC_ORDER,
                                     previous_active_rat );
        }
        else                         
        {
#ifdef FEATURE_SGLTE
          if(MM_IS_IN_SGLTE_MODE &&  
              ((msg_p->cmd.rr_activation_ind.status ==  MM_AS_SGLTE_INTER_RAT_RESELECTION)||
                (msg_p->cmd.rr_activation_ind.status == MM_AS_SGLTE_INTER_RAT_REDIRECTION))) 
           {
          mm_send_rrc_stop_mode_req( RRC_RAT_CHANGE,
                                     previous_active_rat );
              MSG_HIGH_DS_0(MM_SUB,"=MM= Activation from GSM on stack 1 start merge   ");
              mm_sglte_current_stack_status  = SGLTE_MM_LOWER_LAYER_MERGING;
              mmsend_mmr_stack_deact_ind();
        }
          else 
#endif
           {
              mm_send_rrc_stop_mode_req( RRC_RAT_CHANGE,
                                     previous_active_rat );
            }
        }
#else
        /* ---------------------------------------------------------
        ** Perform cleanup of RRC connection/session state variables
        ** --------------------------------------------------------- */
        cs_session_status = MMRRC_IDLE;
        ps_session_status = MMRRC_IDLE;

        if (pmm_mode == PMM_CONNECTED)
        {
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
          gmm_is_prev_mode_connected = TRUE;
          MSG_HIGH_DS_1(MM_SUB ,"=MM= GMM Previous mode Ready or Connected = %d", gmm_is_prev_mode_connected);
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
          pmm_mode = PMM_IDLE;
          MSG_HIGH_DS_0(MM_SUB, "=MM= pmm_mode = PMM_IDLE");
          mm_stop_timer( TIMER_T3319 );
        }

        /* -------------------------
        ** Deactivate the WCDMA RAT
        ** ------------------------- */
#ifdef FEATURE_SGLTE
        if(MM_IS_IN_SGLTE_MODE && 
        ((msg_p->cmd.rr_activation_ind.status == MM_AS_SGLTE_INTER_RAT_REDIRECTION) ||
        (msg_p->cmd.rr_activation_ind.status ==  MM_AS_SGLTE_INTER_RAT_RESELECTION))) 
        {
          mm_send_rrc_stop_mode_req( RRC_RAT_CHANGE,
          previous_active_rat );
          mm_sglte_current_stack_status  = SGLTE_MM_LOWER_LAYER_MERGING;
          mmsend_mmr_stack_deact_ind();
        /* call merge */
        }
        else 
#endif
        {
          mm_send_rrc_stop_mode_req( RRC_RAT_CHANGE,
          previous_active_rat );
        }                         

#endif /* FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER */

        break;

      case RRC_ACTIVATION_IND:
        MSG_HIGH_DS_1(MM_SUB ,"=MM= MM received RRC_ACTIVATION_IND with status %d", msg_p->cmd.rrc_activation_ind.status);
#ifdef FEATURE_LTE
        mm_csfb_is_blind_redir_to_lte_req_to_be_sent = FALSE;
#endif
#ifdef FEATURE_SGLTE
        if((MM_IS_IN_SGLTE_STATE_4)&&
           ((msg_p->cmd.rrc_activation_ind.status == MM_AS_INTERRAT_RESELECTION)||
           (msg_p->cmd.rrc_activation_ind.status == MM_AS_INTERRAT_REDIRECTION)) &&
            mm_nreg_req_received == FALSE) 
        {
          mm_copy_cs_database(MM_SGLTE_STACK_2, mm_ps_stack_id);
          mmsend_mmr_stack_act_ind();
          mm_sglte_current_stack_status  = SGLTE_MM_LOWER_LAYER_SPLITING;
        }
#endif          
        mm_serv_change_pending = FALSE;
        mm_to_only_reg         = TRUE;  
        mm_previous_serving_rat     = mm_serving_plmn.info.active_rat;
        mm_coord_camped_cell_id = 0xFFFF;
        if ( msg_p->cmd.hdr.message_set == MS_MM_TDSRRC
#ifdef FEATURE_SEGMENT_LOADING
&&get_tdscdma_interface()
#endif
)
        {
          mm_serving_plmn.info.active_rat = SYS_RAT_TDS_RADIO_ACCESS;
        }
        else if (TRUE
#ifdef FEATURE_SEGMENT_LOADING
&&get_wcdma_interface()
#endif
)
        {
          mm_serving_plmn.info.active_rat = SYS_RAT_UMTS_RADIO_ACCESS;
        }

        mm_initiate_lu_upon_entry_into_new_cell = FALSE;
 
         /* ---------------------------------------------------
         ** Remember that GSM RAT deactivation is in progress
         ** --------------------------------------------------- */
             
        mm_rat_handler_status = MM_RAT_STOP_GSM_IN_PROGRESS;

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER        
        if ( msg_p->cmd.rrc_activation_ind.status == MM_AS_WTOG_CC_ORDER_FAILED)
        {
          if(mm_held_cnm_service_req.present)
          {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
            mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL, MMCC_SMS_CS);
#else
            mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL);
#endif
          }
          
          if (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
          {
            /* MM_STD_LOW_LEVEL_FAIL is not used here to terminate LU procedure abnormally. 
               Doing that would delete the CS keys and make update status to NOT updated. 
             */            
            mm_state_control( MM_STD_CCO_FAILED );
            mm_stop_timer( TIMER_T3210 );
          } 
          if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
          {
            gmm_set_state( GMM_REGISTERED );
            mm_stop_timer( TIMER_T3330 );
          }
          
          if((cs_session_status == MMRRC_EST_IN_PROGRESS || cs_session_status == MMRRC_ACTIVE ||
              ps_session_status == MMRRC_EST_IN_PROGRESS || ps_session_status == MMRRC_ACTIVE))
          {
            rrc_connection_status = MMRRC_ACTIVE;
          }

          WtoGCCO_state = CCO_FAILED;
          gmm_ra_update_pending = FALSE;
#ifdef FEATURE_SGLTE
          if (MM_IS_IN_SGLTE_MODE )
          {
            mmsend_mmr_stack_act_ind();
            mm_copy_cs_database(MM_SGLTE_STACK_2,mm_ps_stack_id);
            mm_sglte_current_stack_status  = SGLTE_MM_LOWER_LAYER_SPLITING;
          }
#endif
          }
        
#endif /* FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER */

#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE
        if ((msg_p->cmd.rrc_activation_ind.status == MM_AS_GTOW_REDIRECTION) &&
            (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU))
        {
          mm_lu_reject_info.reject_cause = CAUSE_LAI_CHANGED;
        }
#endif

        if ((mm_state == MM_WAIT_FOR_RR_CONNECTION_MM) ||
            (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU) ||
            (mm_state == MM_WAIT_FOR_RR_CONNECTION_IMSI_DETACH) ||
            (mm_state == MM_WAIT_FOR_NETWORK_COMMAND))
        {

          rrc_connection_status = MMRRC_IDLE;
         
         /* Reject any pending CM service request */
#ifdef FEATURE_SGLTE
          if(!(MM_IS_IN_SGLTE_MODE && mm_sglte_current_stack_status  == SGLTE_MM_LOWER_LAYER_SPLITING))
#endif
          { 
             if(mm_held_cnm_service_req.present)
             {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
               mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL, MMCC_SMS_CS);
#else
               mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL);
#endif
             }
             mm_state_control( MM_STD_LOW_LEVEL_FAIL );
          }
        }

#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE
        if ((msg_p->cmd.rrc_activation_ind.status == MM_AS_GTOW_REDIRECTION) &&
            ((gmm_state == GMM_REGISTERED_INITIATED) ||
             (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)))
        {
          if (((gmm_state == GMM_REGISTERED_INITIATED) && 
               (gmm_attach_type == GMM_COMBINED_GPRS_IMSI_ATTACH)) ||
              ((gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) && 
               (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH)))
          {
            mm_gs_association_established = FALSE;
          }
          if (gmm_state == GMM_REGISTERED_INITIATED)
          {
            gmm_set_state( GMM_DEREGISTERED );
          }
          else
          {
            gmm_set_state( GMM_REGISTERED );
            if (gmm_update_status == GMM_GU1_UPDATED  && ((!gmm_rai_changed(gmm_stored_rai.location_area_code, gmm_stored_rai.routing_area_code, 
                                                          mm_serving_plmn.lac, mm_serving_plmn.rac)) && 
                                                         (!gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))
               )
            {
              gmm_set_substate( GMM_NORMAL_SERVICE );
            }
            else
            {
               gmm_set_substate( GMM_UPDATE_NEEDED);
            }
          }
        }
        else 
#endif
        if ((gmm_state == GMM_REGISTERED_INITIATED) ||
            (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) ||
            (gmm_state == GMM_DEREGISTERED_INITIATED))
        {
          gmm_handle_connection_management( msg_p );
        }
#ifdef FEATURE_NAS_FREE_FLOAT
        else if(mm_timer_status[TIMER_NW_INIT_DETACH] == TIMER_ACTIVE &&
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                gmm_last_signalling_message_type == GMM_DETACH_ACCEPT)
#endif
        {
#ifdef FEATURE_LTE
          if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF)
          {
             mm_send_mmr_ps_detach_cnf();
             mm_ps_detach_state = MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ;
          }
#endif
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

#ifdef FEATURE_INTERRAT_PCCO_GTOW

        /* Incase of WtoG CCO, NAS should retain the RRC session/connection info 
           till the RACH is successful on G. Once RACH is successful RRC send
           RRC_WTOG_CCO_COMPLETE_IND and MM should clear the 
           RRC session and connection info */
           
        if (msg_p->cmd.rrc_activation_ind.status == MM_AS_GTOW_CC_ORDER)
        {  
          /* WtoGCCO will only receive only when PMM is in connected state */
          GtoWCCO_state = CCO_IN_PROGRESS;    
        }
#endif /* FEATURE_INTERRAT_PCCO_GTOW */        

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
        if (msg_p->cmd.rrc_activation_ind.status == MM_AS_WTOG_CC_ORDER_FAILED)
        {

          WtoGCCO_state = CCO_FAILED;
        }
#endif /* FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER */

        if ((gmm_state != GMM_NULL) && (gmm_state != GMM_DEREGISTERED))
        {
#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
          if (WtoGCCO_state == CCO_INACTIVE)
          {
          
#ifdef FEATURE_INTERRAT_PCCO_GTOW
            if ( GtoWCCO_state == CCO_INACTIVE )
#endif /* FEATURE_INTERRAT_PCCO_GTOW */
            {
              /* --------------------------------------------------
              ** Notify RAT change to Session Mangement Entity
              ** -------------------------------------------------- */
              gmm_sm_send_rat_change_req(SYS_RAT_GSM_RADIO_ACCESS 
                                         #if defined(FEATURE_LTE)
                                         , FALSE
                                         #endif
                                         );
            }
            
#ifdef FEATURE_GTOW_PS_HANDOVER 
            if (msg_p->cmd.rrc_activation_ind.status == MM_AS_INTERRAT_PS_HANDOVER)
            {
                mm_serving_plmn.info.ps_data_suspend = FALSE;
                MSG_HIGH_DS_0(MM_SUB,"=MM= Packet Data services are NOT suspended-IRAT_PSHO in progress");
            }
            else
#endif /* FEATURE_GTOW_PS_HANDOVER */
            {  
               /* ---------------------------------------------
               ** Remember that PS data services are suspended
               ** --------------------------------------------- */
               mm_serving_plmn.info.ps_data_suspend = TRUE;
               MSG_HIGH_DS_0(MM_SUB,"=MM= Suspending Packet Data services");
            }
          }
          else
          {
            mm_serving_plmn.info.ps_data_suspend = FALSE;
            MSG_HIGH_DS_0(MM_SUB,"=MM= Resuming Packet Data services after CCO Failed");
          }
#else

#ifdef FEATURE_INTERRAT_PCCO_GTOW
          if ( GtoWCCO_state == CCO_INACTIVE )
#endif /* FEATURE_INTERRAT_PCCO_GTOW */
          {
            /* --------------------------------------------------
            ** Notify RAT change to Session Mangement Entity
            ** -------------------------------------------------- */
            gmm_sm_send_rat_change_req(SYS_RAT_GSM_RADIO_ACCESS       
                                       #if defined(FEATURE_LTE)
                                       , FALSE
                                       #endif
                                       );
          }

#ifdef FEATURE_GTOW_PS_HANDOVER 
          if (msg_p->cmd.rrc_activation_ind.status == MM_AS_INTERRAT_PS_HANDOVER)
          {
              mm_serving_plmn.info.ps_data_suspend = FALSE;
              MSG_HIGH_DS_0(MM_SUB,"=MM= Packet Data services are NOT suspended-IRAT_PSHO in progress");
          }
          else
#endif /* FEATURE_GTOW_PS_HANDOVER */
          {  
             /* ---------------------------------------------
             ** Remember that PS data services are suspended
             ** --------------------------------------------- */
             mm_serving_plmn.info.ps_data_suspend = TRUE;
             MSG_HIGH_DS_0(MM_SUB,"=MM= Suspending Packet Data services");
          }

#endif /* FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS) 
          /* Check if there is data in SNDCP queue during RAT change */
          data_is_available = dsumtsps_is_ps_data_available();
#endif
        }

        if ( msg_p->cmd.hdr.message_set == MS_MM_TDSRRC
#ifdef FEATURE_SEGMENT_LOADING
&&get_tdscdma_interface()
#endif
)
        {
          mm_serving_plmn.info.active_rat = SYS_RAT_TDS_RADIO_ACCESS;
        }
        else if (TRUE
#ifdef FEATURE_SEGMENT_LOADING
&&get_wcdma_interface()
#endif
)
        {
          mm_serving_plmn.info.active_rat = SYS_RAT_UMTS_RADIO_ACCESS;
        }

        /* -------------------------------
        ** Notify RAT change to REG layer
        ** ------------------------------- */
        if (mm_serving_plmn.info.ps_data_suspend == TRUE)
        {
          mm_send_cm_ps_data_suspend_ind(); 
        }
        
        if (mm_reg_waiting_for_reg_cnf)
        {
          if (mm_ready_to_send_reg_cnf)
          {
            mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
            mm_send_mmr_reg_cnf();

            mm_reg_waiting_for_reg_cnf = FALSE;
            mm_ready_to_send_reg_cnf   = FALSE;
          }
        }
        else
        {
          if ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
                   SYS_NETWORK_SELECTION_MODE_AUTOMATIC) &&
              ((mm_timer_status[TIMER_T3211] == TIMER_ACTIVE) || 
               (mm_timer_status[TIMER_T3311] == TIMER_ACTIVE)))
          {
            mmr_service_ind.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
          }
          else
          {
            mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
          }

          mmr_service_ind.service_state = mm_serving_plmn.info;
          mm_send_mmr_service_ind( &mmr_service_ind );
        }

        /* -----------------------------------------------------------------
        ** RRC connection & CS session will be active upon handoff to WCDMA
        ** ----------------------------------------------------------------- */
        if (msg_p->cmd.rrc_activation_ind.status ==  MM_AS_INTERRAT_HANDOVER)
        {
          rrc_connection_status = MMRRC_ACTIVE;
          cs_session_status = MMRRC_ACTIVE;
          
#ifdef FEATURE_GSM_GPRS
          if(gmm_is_suspended)
          {
             MSG_HIGH_DS_0(MM_SUB,"=MM= Marking gmm_rau_is_required to TRUE due to G2W handover");
             gmm_rau_is_required = TRUE;
          }
#endif /*FEATURE_GSM_GPRS*/
        }
#ifdef FEATURE_GTOW_PS_HANDOVER        
        else if (msg_p->cmd.rrc_activation_ind.status ==  MM_AS_INTERRAT_PS_HANDOVER)
        {
          rrc_connection_status = MMRRC_ACTIVE;
          ps_session_status = MMRRC_ACTIVE;          
          gmm_ps_signalling_connected = TRUE;
            
        }
#endif         

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
        if (mm_state == MM_RR_CONNECTION_RELEASE_NOT_ALLOWED)
        {
           if(rrc_connection_status != MMRRC_RELEASE_IN_PROGRESS)
           {
             MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent RR_ABORT_REQ");
             mm_send_rr_abort_req(FALSE);
           }
           /* ---------------------------------------------------
           ** Remember that GSM RAT deactivation is pending an abort
           ** --------------------------------------------------- */
           mm_rat_handler_status = MM_RAT_STOP_GSM_PENDING_ABORT;
        }
        else
        {
           /* -------------------------
           ** Deactivate the GSM RAT
           ** ------------------------- */
           
#ifdef FEATURE_INTERRAT_PCCO_GTOW
           if (GtoWCCO_state == CCO_IN_PROGRESS)
           {
             mm_send_rr_stop_mode_req(RR_GTOW_CC_ORDER);
           }
           else 
           {
#ifdef FEATURE_SGLTE
           if((MM_IS_IN_SGLTE_STATE_4)&&
             ((msg_p->cmd.rrc_activation_ind.status == MM_AS_INTERRAT_RESELECTION)||
             (msg_p->cmd.rrc_activation_ind.status == MM_AS_INTERRAT_REDIRECTION) ||
         (msg_p->cmd.rrc_activation_ind.status == MM_AS_WTOG_CC_ORDER_FAILED)) &&
          mm_nreg_req_received == FALSE) 
             {
               mm_send_rr_stop_mode_req(RR_SGLTE_RAT_CHANGE);
               mm_sglte_current_stack_status  = SGLTE_MM_LOWER_LAYER_SPLITING;
             }
             else
#endif
             {
               mm_send_rr_stop_mode_req(RR_MM_RAT_CHANGE);
             }
           }
#else          
#ifdef FEATURE_SGLTE
        if((MM_IS_IN_SGLTE_STATE_4)&&
           ((msg_p->cmd.rrc_activation_ind.status == MM_AS_INTERRAT_RESELECTION)||
           (msg_p->cmd.rrc_activation_ind.status == MM_AS_INTERRAT_REDIRECTION) ||
           (msg_p->cmd.rrc_activation_ind.status == MM_AS_WTOG_CC_ORDER_FAILED)) &&
          mm_nreg_req_received == FALSE) 
           {
             mm_send_rr_stop_mode_req(RR_SGLTE_RAT_CHANGE);
             mm_sglte_current_stack_status  = SGLTE_MM_LOWER_LAYER_SPLITING;
           }
           else
#endif     
           {
             mm_send_rr_stop_mode_req(RR_MM_RAT_CHANGE);
           } 
#endif /* FEATURE_INTERRAT_PCCO_GTOW */
        }
#else

        /* -------------------------
        ** Deactivate the GSM RAT
        ** ------------------------- */
#ifdef FEATURE_INTERRAT_PCCO_GTOW
           if (GtoWCCO_state == CCO_IN_PROGRESS)
           {
             mm_send_rr_stop_mode_req(RR_GTOW_CC_ORDER);
           }
           else 
           {
             mm_send_rr_stop_mode_req(RR_MM_RAT_CHANGE);
           }
#else           
#ifdef FEATURE_SGLTE
           if((MM_IS_IN_SGLTE_STATE_4)&&
             ((msg_p->cmd.rrc_activation_ind.status == MM_AS_INTERRAT_RESELECTION)||
             (msg_p->cmd.rrc_activation_ind.status == MM_AS_INTERRAT_REDIRECTION) ||
             (msg_p->cmd.rrc_activation_ind.status == MM_AS_WTOG_CC_ORDER_FAILED)))
           {
             mm_send_rr_stop_mode_req(RR_SGLTE_RAT_CHANGE);
             mm_sglte_current_stack_status  = SGLTE_MM_LOWER_LAYER_SPLITING;
           }
           else
#endif    
           {
             mm_send_rr_stop_mode_req(RR_MM_RAT_CHANGE);
           }
#endif /* FEATURE_INTERRAT_PCCO_GTOW */ 
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
        break;

      default:
        MSG_ERROR_DS( MM_SUB, "=MM= Unknown activation event id (%d) rcvd by MM",
                   msg_p->cmd.hdr.message_id, 0, 0);
        break;
    }
  }
} /* mm_handle_activation_ind () */


/*===========================================================================

FUNCTION MM_HANDLE_STOP_MODE_CNF

DESCRIPTION
  This function handles stop mode confirmation event from RR/RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_handle_stop_mode_cnf (mm_cmd_type *msg_p)
{
  sys_radio_access_tech_e_type active_rat = mm_serving_plmn.info.active_rat;
#ifdef FEATURE_LTE
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
  /* --------------------------------
  ** Abort a LCS session in progress
  ** -------------------------------- */
  if (mm_lcs_active)
  {
     mm_abort_lcs();
  }
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */

  mm_waiting_for_stop_mode_cnf = FALSE;

  /* Clear pending cell service indication to be sent */
  mm_mmr_cell_service_ind_pending = FALSE;
#ifdef FEATURE_WRLF_SYSTEM_SEL
  if((mm_wcdma_rlf_state != WCDMA_RLF_IDLE) &&
      (msg_p->cmd.hdr.message_id == (int) RRC_STOP_WCDMA_MODE_CNF))
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= MM received RRC_STOP_WCDMA_MODE_CNF DURING RLF");
  }
  else
#endif
  if (msg_p->cmd.hdr.message_id == (int) RRC_STOP_WCDMA_MODE_CNF)
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= MM received RRC_STOP_WCDMA_MODE_CNF");

    mm_serving_plmn.hsdpa_hsupa_ind = SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL;
    mm_coord_camped_cell_id = 0xFFFF;  

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
    /* ---------------------------------------------------------
    ** Perform cleanup of RRC connection/session state variables
    ** if WtoGCCO not in progress
    ** --------------------------------------------------------- */

    if (WtoGCCO_state == CCO_INACTIVE)
#endif
    {
      cs_session_status = MMRRC_IDLE;
      ps_session_status = MMRRC_IDLE;

#ifdef FEATURE_MODEM_HEAP
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
      if((pending_mm_msg_valid == FALSE) &&
         (pending_mm_msg != NULL))
      {
        modem_mem_free(pending_mm_msg,MODEM_MEM_CLIENT_NAS);
        pending_mm_msg = NULL;
#ifdef FEATURE_NAS_REL7
        mm_cs_domain_possible_reest = FALSE;
#endif
      }
      if((pending_gmm_msg_valid == FALSE) &&
         (pending_gmm_msg != NULL))
      {
        modem_mem_free(pending_gmm_msg,MODEM_MEM_CLIENT_NAS);
        pending_gmm_msg = NULL;
#ifdef FEATURE_NAS_REL7
        mm_ps_domain_possible_reest = FALSE;
#endif
      }
      if((gmm_rat_chng_pended_msg_valid == FALSE) &&
         (gmm_rat_chng_pended_msg != NULL))
      {
        modem_mem_free(gmm_rat_chng_pended_msg,MODEM_MEM_CLIENT_NAS);
        gmm_rat_chng_pended_msg = NULL;
      }
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
#endif

      /* Retain the PMM mode if WtoGCCO is in progress */
      if (pmm_mode == PMM_CONNECTED)
      {
        pmm_mode = PMM_IDLE;
        MSG_HIGH_DS_0(MM_SUB, "=MM= pmm_mode = PMM_IDLE");
        mm_stop_timer( TIMER_T3319 );
      }
    }
  }
  else if(msg_p->cmd.hdr.message_id == (int) RR_STOP_GSM_MODE_CNF)
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= MM received RR_STOP_GSM_MODE_CNF");

    mm_coord_camped_cell_id = 0xFFFF;
#ifdef FEATURE_GSM_DTM
    mm_serving_plmn.dtm_supported = FALSE;
#endif
    mm_serving_plmn.egprs_supported = FALSE;
  }

  MSG_HIGH_DS_2(MM_SUB, "=MM= mm_rat_change_pending: %d, mm_service_loss_on_rat %d", 
           mm_rat_change_pending, mm_service_loss_on_rat);
  /* If the RAT change is pending, Clear the Active Rat, Service_state */

#ifdef FEATURE_SGLTE
   if(MM_SUB_IS_SGLTE)
   {
     MSG_HIGH_DS_2(MM_SUB, "=MM= mm_multimode_rat_change_pending = %d  mm_sglte_current_stack_status  = %d ", mm_multimode_rat_change_pending,
                 mm_sglte_current_stack_status);
   }
#endif

#ifdef FEATURE_SGLTE
  if(MM_IS_IN_SGLTE_MODE && 
     (msg_p->cmd.hdr.message_id == (int) RRC_STOP_WCDMA_MODE_CNF) &&
     (mm_sglte_current_stack_status == SGLTE_MM_UPPER_LAYER_MERGING) &&
     (((mmcoord_reg_req_pending == TRUE) && 
       (mmcoord_pending_reg_message.cmd.mmr_reg_req.type != MMR_REG_REQ_SGLTE_PSTIMER_EXPIRY)) ||
      (emm_ctrl_data_ptr->reg_req_pended_indication) || 
      (mm_plmn_list_req_pending == TRUE)||(mm_nreg_req_received == TRUE)||
      (mm_ps_detach_state == MM_PS_DETACH_STATE_DETACH_REQ_PENDING)))
  {
     /* if REG overrides merge, merge context before processing new REG command  */
     mmcoord_process_pstimer_expiry_merging();
     mm_rat_change_pending = FALSE;
     mm_multimode_rat_change_pending = FALSE;
  }
#endif
  if (mm_rat_change_pending)
  {
    mm_serving_plmn.mm_service_state  = SERVICE_NOT_AVAILABLE;
    mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;
 #ifdef FEATURE_FEMTO_CSG
    mm_serving_csg_id = SYS_CSG_ID_INVALID;
 #endif 

    if ((gmm_state != GMM_NULL) &&
        (gmm_state != GMM_DEREGISTERED))
    {
      boolean data_suspend_status = mm_serving_plmn.info.ps_data_suspend;
      /* -------------------------------------------------------------------
      ** If PS call is active then suspend the PS data and save the active RAT
      ** information
      ** ------------------------------------------------------------------- */
      mm_rat_handle_srv_status_update(SYS_SRV_STATUS_NO_SRV);

      /* -------------------------------------------------------------------
      ** If PS data is not suspended then suspend the data
      ** ------------------------------------------------------------------- */
      if (!data_suspend_status &&
         mm_serving_plmn.info.ps_data_suspend)
      {
        mm_send_cm_ps_data_suspend_ind();
      }
    }
    active_rat = mm_serving_plmn.info.active_rat;
    mm_serving_plmn.info.active_rat   = SYS_RAT_NONE;
    mm_substate_control( MM_STD_DEACTIVATE );

    MSG_HIGH_DS_0(MM_SUB, "=MM= ACTIVATE MM before entering new RAT");
    mm_activate_state( TRUE );
    mm_rat_change_pending = FALSE;

#ifdef FEATURE_LTE
    /* If there is a pending PS detach req then process the request. Otherwise if 
    ** detach confirmation needs to sent then send the confirmation
    */
#ifdef FEATURE_SGLTE    
    if(MM_SUB_IS_SGLTE)
    {
      MSG_HIGH_DS_1(MM_SUB ,"=MM=   mm_sglte_current_stack_status = %d  ", mm_sglte_current_stack_status);
    }
   if(MM_IS_IN_SGLTE_MODE && (mm_sglte_current_stack_status==SGLTE_MM_UPPER_LAYER_MERGING))
   {
     MSG_HIGH_DS_0(MM_SUB,"=MM= Merging finished for ps timer in TDS  ");
     mmcoord_process_pstimer_expiry_merging();
     mm_sglte_current_stack_status = SGLTE_INACTIVE_STATE;
   }
   else if (MM_IS_IN_SGLTE_MODE && 
      (mm_sglte_current_stack_status ==SGLTE_MM_UPPER_LAYER_SPLITING))
   {
     MSG_HIGH_DS_0(MM_SUB,"=MM= Got RR stop mode cnf split complete reg ps split req or ccofailed req .  ");
     mm_sglte_current_stack_status = SGLTE_INACTIVE_STATE;
     mmcoord_process_spliting();
     mmsend_mmr_service_stat_ind(mm_cs_stack_id);
     if (mmcoord_reg_req_pending)
     {
       mmcoord_reg_req_pending = FALSE;
       mmcoord_process_mmr_reg_req(&mmcoord_pending_reg_message);
     }
     else if(mm_ps_detach_state == MM_PS_DETACH_STATE_DETACH_REQ_PENDING)
     {
      mm_plmn_list_req_pending = FALSE;
      mmcoord_reg_req_pending = FALSE;
#ifdef FEATURE_MODEM_HEAP
      if(mm_pending_mmr_ps_detach_req_ptr != NULL)
      {
        mmcoord_process_mmr_ps_detach_req(mm_pending_mmr_ps_detach_req_ptr);
        modem_mem_free(mm_pending_mmr_ps_detach_req_ptr,MODEM_MEM_CLIENT_NAS);
        mm_pending_mmr_ps_detach_req_ptr = NULL;
      }
#else
      mmcoord_process_mmr_ps_detach_req(&mm_pending_mmr_ps_detach_req);
      memset(&mm_pending_mmr_ps_detach_req,0,sizeof(mm_cmd_type));
#endif
      /* Active rat was set to none at the start of this function. So EMM state
      ** will not be changed to deregistered when GMM is moving to deregistered.
      ** Call MM multimode detach to set the EMM states accordingly
      */
      emm_multimode_detach();

    }
    /* -------------------------------------------------------------------
    ** MM is activated now and ready to process the pending REG command
    ** ------------------------------------------------------------------- */
    else if ( (mm_plmn_list_req_pending == TRUE) && 
              ((mmcoord_pending_reg_list_req_message.cmd.hdr.message_set == MS_MM_REG) &&
              (mmcoord_pending_reg_list_req_message.cmd.hdr.message_id  == (int) MMR_PLMN_SEARCH_REQ)))
    {
      MSG_HIGH_DS_0(MM_SUB, "=MM= RAT mode stopped, Continue search on stopped rat");
      mmcoord_pending_reg_list_req_message.cmd.mmr_plmn_search_req.rat = mm_serving_plmn.info.active_rat;
      mmcoord_process_mmr_plmn_search_req(&mmcoord_pending_reg_list_req_message);
      mm_serving_plmn.info.service_status = SYS_SRV_STATUS_NO_SRV;
      mmcoord_pending_reg_list_req_message.cmd.hdr.message_set = 0;
    }
   }
   else
#endif
    if(mm_ps_detach_state == MM_PS_DETACH_STATE_DETACH_REQ_PENDING)
    {
      mm_plmn_list_req_pending = FALSE;
      mmcoord_reg_req_pending = FALSE;
#ifdef FEATURE_MODEM_HEAP
      if(mm_pending_mmr_ps_detach_req_ptr != NULL)
      {
        mmcoord_process_mmr_ps_detach_req(mm_pending_mmr_ps_detach_req_ptr);
        if(mm_ps_detach_state != MM_PS_DETACH_STATE_DETACH_REQ_PENDING)
        {
          modem_mem_free(mm_pending_mmr_ps_detach_req_ptr,MODEM_MEM_CLIENT_NAS);
          mm_pending_mmr_ps_detach_req_ptr = NULL;
        }
      }
#else
      mmcoord_process_mmr_ps_detach_req(&mm_pending_mmr_ps_detach_req);
      memset(&mm_pending_mmr_ps_detach_req,0,sizeof(mm_cmd_type));
#endif
      /* Active rat was set to none at the start of this function. So EMM state
      ** will not be changed to deregistered when GMM is moving to deregistered.
      ** Call MM multimode detach to set the EMM states accordingly
      */
      emm_multimode_detach();

    }
    else if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF)
    {
      mm_plmn_list_req_pending = FALSE;
      mmcoord_reg_req_pending = FALSE;
      mm_reg_waiting_for_reg_cnf = FALSE;
      mm_stop_timer(TIMER_MM_PS_DETACH);
      gmm_gprs_attach_pending = FALSE;
      mm_send_mmr_ps_detach_cnf();
      mm_ps_detach_state = MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ;
      /* Active rat was set to none at the start of this function. So EMM state
      ** will not be changed to deregistered when GMM is moving to deregistered.
      ** Call MM multimode detach to set the EMM states accordingly
      */
      emm_multimode_detach();
    }else
#endif
    /* -------------------------------------------------------------------
    ** MM is activated now and ready to process the pending REG command
    ** ------------------------------------------------------------------- */
    if ( (mm_plmn_list_req_pending == TRUE) && 
         ((mmcoord_pending_reg_list_req_message.cmd.hdr.message_set == MS_MM_REG) &&
         (mmcoord_pending_reg_list_req_message.cmd.hdr.message_id  == (int) MMR_PLMN_SEARCH_REQ)))
    {
      MSG_HIGH_DS_0(MM_SUB, "=MM= RAT mode stopped, Continue PLMN search on new RAT");

      mmcoord_process_mmr_plmn_search_req(&mmcoord_pending_reg_list_req_message);

      mm_serving_plmn.info.service_status   = SYS_SRV_STATUS_NO_SRV;

      mmcoord_pending_reg_list_req_message.cmd.hdr.message_set = 0;

    }
    else
    {
      MSG_HIGH_DS_0(MM_SUB, "=MM= RAT mode stopped, Continue REG on new RAT");
      mmcoord_reg_req_pending = FALSE;
      
      mmcoord_process_lu_procedure (&mmcoord_pending_reg_message);   

      if (active_rat == SYS_RAT_GSM_RADIO_ACCESS)
      {
            if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
            {
              mm_reg_waiting_for_reg_cnf = FALSE;
              gmm_increment_routing_area_update_attempt_counter
              (
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
                FALSE
#endif
              );
              mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
              mm_reg_waiting_for_reg_cnf = TRUE;
            }
            else if (gmm_state == GMM_REGISTERED_INITIATED)
            {
              mm_reg_waiting_for_reg_cnf = FALSE;
              gmm_increment_gprs_attach_attempt_counter();
              mmsend_nas_reg_event_packet(PROC_NAS_ATTACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
              mm_reg_waiting_for_reg_cnf = TRUE;
            }
      }

    }
  } /* if (mm_rat_change_pending) */
  else if ( mm_nreg_req_received && mm_serving_plmn.info.active_rat != SYS_RAT_LTE_RADIO_ACCESS )
  {
#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
    if (WtoGCCO_state == CCO_FAILED)
    {
      mm_rat_handler_status = MM_RAT_INITIAL;
      mm_send_rrc_activation_rsp( SYS_RAT_GSM_RADIO_ACCESS, active_rat );
      if (((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
           (mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1)) ||
          (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY))
         
      {
        gmm_handle_message( &mmcoord_pending_reg_message );
      }
      else /* NETWORK_OP_MODE_2 */
      {
        mm_handle_message( &mmcoord_pending_reg_message );
        gmm_handle_message( &mmcoord_pending_reg_message );
      }
    }
    else
#endif
    { 
      mm_send_activation_rsp_if_needed();
      if ((gmm_state != GMM_NULL) && (gmm_state != GMM_DEREGISTERED))
      {
        gmm_set_state( GMM_DEREGISTERED );
        gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                         SYS_SRV_DOMAIN_NO_SRV,
                                         FALSE,
                                         SYS_SRV_STATUS_LIMITED,
                                         FALSE );
      }

      if (rrc_connection_status != MMRRC_IDLE)
      {
         if (is_gsm_mode())
         {
            mm_abort_rr( RRC_CS_DOMAIN_CN_ID, FALSE );
         }
         else if (is_umts_mode())
         {
            rrc_cmd_type *rrc_cmd_p = NULL;

#ifdef FEATURE_WCDMA
            rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );
#endif

             if (rrc_cmd_p == NULL)
             {
                 MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
             }               
              rrc_cmd_p->cmd_hdr.cmd_id = RRC_ABORT_REQ;
              rrc_cmd_p->cmd.rrc_abort_req.auth_failed_bar_cells = FALSE;
              rrc_cmd_p->cmd.rrc_abort_req.delay_sig_con_release = FALSE;
              rrc_cmd_p->cmd.rrc_abort_req.hard_abort = TRUE;
              rrc_cmd_p->cmd.rrc_abort_req.abort_cause = RRC_OTHERS;
              send_cmd_to_rrc( rrc_cmd_p, mm_serving_plmn.info.active_rat );
         }
         if ((mm_state == MM_CONNECTION_ACTIVE) && mm_cnm_is_active)
         {
            mm_send_mmcnm_rel_ind( OTHER_CAUSE, FALSE );
         }
         mm_state_control( MM_STD_RR_CONNECTION_ABORTED ); 
      }
      else if (!mm_waiting_for_deact_cnf)
      {
        mm_nreg_req_received = FALSE;
        mm_deactivate_rr();
      }
    }
  } /* if ( mm_nreg_req_received ) */
  else if ((mm_stop_mode_req_pended == TRUE) &&(mm_timer_status[ TIMER_MM_REL ] == TIMER_ACTIVE))
  {
    mm_send_activation_rsp_if_needed();
    mm_nreg_req_received = FALSE;
    if ((gmm_state != GMM_NULL) && (gmm_state != GMM_DEREGISTERED))
    {
      gmm_set_state( GMM_DEREGISTERED );
      gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                       SYS_SRV_DOMAIN_NO_SRV,
                                       FALSE,
                                       SYS_SRV_STATUS_LIMITED,
                                       FALSE );
    }
    mm_state_control( MM_STD_MS_DEACTIVATED_NO_ATTACH );
  }
  else if(mm_hard_abort_pending)
  {
    mm_send_activation_rsp_if_needed();
    mm_hard_abort_pending = FALSE;
    mmcoord_handle_deep_sleep_alt_scan();
  }
#ifdef FEATURE_DUAL_SIM
  else if(mm_abort_dual_switch_pending == SYS_STOP_MODE_REASON_DSDS_SUSPEND)
  {
    mm_send_activation_rsp_if_needed();
    mm_abort_dual_switch_pending = SYS_STOP_MODE_REASON_NONE;
    mm_stop_timer(TIMER_MM_REL);
    MSG_HIGH_DS_0(MM_SUB, "=MM= DSDS Suspend. Pause all running timers (except periodic timers)");           
    mmcoord_handle_deep_sleep_alt_scan();
  }
  else if(mm_abort_dual_switch_pending == SYS_STOP_MODE_REASON_DUAL_SWITCH)
  {
    mm_send_activation_rsp_if_needed();
    mm_abort_dual_switch_pending = SYS_STOP_MODE_REASON_NONE;
    mm_stop_timer(TIMER_MM_REL);
    mmcoord_handle_deep_sleep_dual_switch();
  }
  else if(mm_abort_dual_switch_pending == SYS_STOP_MODE_REASON_ALT_SCAN)
  {
    mm_send_activation_rsp_if_needed();
    mm_abort_dual_switch_pending = SYS_STOP_MODE_REASON_NONE;
    mm_stop_timer(TIMER_MM_REL);
    mmcoord_handle_deep_sleep_alt_scan();
  }
#endif

#if defined(FEATURE_LTE)
  /* If the RAT change is pending, Clear the Active Rat, Service_state */
  else if (mm_multimode_rat_change_pending)
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= MM multimode rat change pending");
    mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE;
    mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;
    mm_multimode_rat_change_pending = FALSE;
 #ifdef FEATURE_FEMTO_CSG
    mm_serving_csg_id = SYS_CSG_ID_INVALID;
 #endif 
    if ((gmm_state != GMM_NULL) &&
        (gmm_state != GMM_DEREGISTERED))
    {

      boolean data_suspend_status = mm_serving_plmn.info.ps_data_suspend;
      mm_rat_handle_srv_status_update(SYS_SRV_STATUS_NO_SRV);
      /* -------------------------------------------------------------------
      ** If PS data is not suspended then suspend the data
      ** ------------------------------------------------------------------- */
      if (!data_suspend_status &&
         mm_serving_plmn.info.ps_data_suspend)
      {
        mm_send_cm_ps_data_suspend_ind();
      }
    }    
#ifdef FEATURE_WRLF_SYSTEM_SEL
    if(mm_wcdma_rlf_state == WCDMA_RLF_IDLE)
#endif
    {
      if (mm_serving_plmn.info.active_rat == SYS_RAT_GSM_RADIO_ACCESS)
      {
        if (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
        {
          gmm_increment_routing_area_update_attempt_counter
          (
    #ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
            FALSE
    #endif
          );        
          mmsend_nas_reg_event_packet(PROC_NAS_RAU_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
        }
        else if (gmm_state == GMM_REGISTERED_INITIATED)
        {       
          gmm_increment_gprs_attach_attempt_counter();        
          mmsend_nas_reg_event_packet(PROC_NAS_ATTACH_EVENT,PROC_END,PROC_END_LOWER_LAYER_FAILURE,MT_DETACH_TYPE_NONE,MT_DETACH_CAUSE_NONE);
        }
      }
      mm_substate_control( MM_STD_NO_SERVICE );
      gmm_set_substate( GMM_NO_CELL_AVAILABLE );
      mm_cancel_authentication();
      gmm_cancel_authentication();
    }

    active_rat = mm_serving_plmn.info.active_rat;
    mm_serving_plmn.info.active_rat = SYS_RAT_NONE;

    if (mm_state == MM_NULL)
    {
       mm_activate_state( TRUE );
    }

    /*Clear MM serving LAI */
    mm_serving_lai[0] = 0xFF;
    mm_serving_lai[1] = 0xFF;
    mm_serving_lai[2] = 0xFF;
    mm_serving_lai[3] = 0xFF;
    mm_serving_lai[4] = 0xFE;

    /* If there is a pending PS detach req then process the request. Otherwise if 
    ** detach confirmation needs to sent then send the confirmation
    */
#ifdef FEATURE_SGLTE
    if (MM_IS_IN_SGLTE_MODE && (mm_sglte_current_stack_status ==SGLTE_MM_UPPER_LAYER_SPLITING))
    {
      mm_sglte_current_stack_status = SGLTE_INACTIVE_STATE;
      MSG_HIGH_DS_0(MM_SUB,"=MM= reg ps split completed ...  ");
      mmcoord_process_spliting();
      mmsend_mmr_service_stat_ind(mm_cs_stack_id);
      if(mm_ps_detach_state == MM_PS_DETACH_STATE_DETACH_REQ_PENDING)
      {
        mm_serving_plmn.info.active_rat   = SYS_RAT_NONE;
#ifdef FEATURE_MODEM_HEAP
        if(mm_pending_mmr_ps_detach_req_ptr != NULL)
        {
          mmcoord_process_mmr_ps_detach_req(mm_pending_mmr_ps_detach_req_ptr);
          modem_mem_free(mm_pending_mmr_ps_detach_req_ptr,MODEM_MEM_CLIENT_NAS);
          mm_pending_mmr_ps_detach_req_ptr = NULL;
        }
#else
        mmcoord_process_mmr_ps_detach_req(&mm_pending_mmr_ps_detach_req);
        memset(&mm_pending_mmr_ps_detach_req,0,sizeof(mm_cmd_type));
#endif
       /* Active rat was set to none at the start of this function. So EMM state
       ** will not be changed to deregistered when GMM is moving to deregistered.
       ** Call MM multimode detach to set the EMM states accordingly
       */
        emm_multimode_detach();
      }
      /* -------------------------------------------------------------------
      ** MM is activated now and ready to process the pending REG command
      ** ------------------------------------------------------------------- */
      else if ( (mm_plmn_list_req_pending == TRUE) && 
              ((mmcoord_pending_reg_list_req_message.cmd.hdr.message_set == MS_MM_REG) &&
              (mmcoord_pending_reg_list_req_message.cmd.hdr.message_id  == (int) MMR_PLMN_SEARCH_REQ)))
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= RAT mode stopped, Continue search on stopped rat");
        mmcoord_pending_reg_list_req_message.cmd.mmr_plmn_search_req.rat =  mm_serving_plmn.info.active_rat;
        mmcoord_process_mmr_plmn_search_req(&mmcoord_pending_reg_list_req_message);
        mm_serving_plmn.info.service_status = SYS_SRV_STATUS_NO_SRV;
        mmcoord_pending_reg_list_req_message.cmd.hdr.message_set = 0;
      }
    }
    else
#endif
    if(mm_ps_detach_state == MM_PS_DETACH_STATE_DETACH_REQ_PENDING)
    {
      mm_serving_plmn.info.active_rat   = SYS_RAT_NONE;
#ifdef FEATURE_MODEM_HEAP
      if(mm_pending_mmr_ps_detach_req_ptr != NULL)
      {
        mmcoord_process_mmr_ps_detach_req(mm_pending_mmr_ps_detach_req_ptr);
        if(mm_ps_detach_state != MM_PS_DETACH_STATE_DETACH_REQ_PENDING)
        {
          modem_mem_free(mm_pending_mmr_ps_detach_req_ptr,MODEM_MEM_CLIENT_NAS);
          mm_pending_mmr_ps_detach_req_ptr = NULL;
        }
      }
#else
      mmcoord_process_mmr_ps_detach_req(&mm_pending_mmr_ps_detach_req);
      memset(&mm_pending_mmr_ps_detach_req,0,sizeof(mm_cmd_type));
#endif
      /* Active rat was set to none at the start of this function. So EMM state
      ** will not be changed to deregistered when GMM is moving to deregistered.
      ** Call MM multimode detach to set the EMM states accordingly
      */
      emm_multimode_detach();
    }
    else if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF)
    {
      mm_serving_plmn.info.active_rat   = SYS_RAT_NONE;
      mm_stop_timer(TIMER_MM_PS_DETACH);
      gmm_gprs_attach_pending = FALSE;
      mm_send_mmr_ps_detach_cnf();
      mm_ps_detach_state = MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ;
      /* Active rat was set to none at the start of this function. So EMM state
      ** will not be changed to deregistered when GMM is moving to deregistered.
      ** Call MM multimode detach to set the EMM states accordingly
      */
      emm_multimode_detach();
    }
    /* -------------------------------------------------------------------
    ** MM is activated now and ready to process the pending REG command
    ** ------------------------------------------------------------------- */
    else if ( (mm_plmn_list_req_pending == TRUE) && 
         ((mmcoord_pending_reg_list_req_message.cmd.hdr.message_set == MS_MM_REG) &&
         (mmcoord_pending_reg_list_req_message.cmd.hdr.message_id  == (int) MMR_PLMN_SEARCH_REQ)))
    {
      MSG_HIGH_DS_0(MM_SUB, "=MM= RAT mode stopped, Continue search on stopped rat");

      mmcoord_pending_reg_list_req_message.cmd.mmr_plmn_search_req.rat = active_rat;

      mmcoord_process_mmr_plmn_search_req(&mmcoord_pending_reg_list_req_message);

      mm_serving_plmn.info.service_status   = SYS_SRV_STATUS_NO_SRV;

      mmcoord_pending_reg_list_req_message.cmd.hdr.message_set = 0;

    }

  }
#endif

#ifdef FEATURE_WRLF_SYSTEM_SEL
  else if (mm_wrlf_rat_change_pending)
  {
    mm_serving_plmn.mm_service_state  = SERVICE_NOT_AVAILABLE;
    mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;
    mm_wrlf_rat_change_pending = FALSE;
    mmcoord_reg_req_pending = FALSE;
    mm_rat_handle_srv_status_update(SYS_SRV_STATUS_NO_SRV);

    mm_send_service_req( &mmcoord_pending_reg_message,
                                            mmcoord_pending_reg_message.cmd.mmr_reg_req.rat);
  }
#endif
#ifdef FEATURE_SGLTE
  else if (MM_IS_IN_SGLTE_STATE_4 && 
      (mm_sglte_current_stack_status == SGLTE_MM_UPPER_LAYER_SPLITING)&&
      (mm_is_split_needed() == TRUE))
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= MM splitting in State 4 probably because PS is not available on GSM");
    mm_sglte_current_stack_status = SGLTE_INACTIVE_STATE;
    mmcoord_process_spliting();
    mmsend_mmr_service_stat_ind(mm_cs_stack_id);

        /* Main stack has no service now --> reset service context now */ 
    mm_serving_plmn_sim[mm_ps_stack_id].info.service_status = SYS_SRV_STATUS_NO_SRV;
    mm_serving_plmn_sim[mm_ps_stack_id].mm_service_state = SERVICE_NOT_AVAILABLE;
    mm_serving_plmn_sim[mm_ps_stack_id].gmm_service_state = SERVICE_NOT_AVAILABLE;
    mm_serving_plmn_sim[mm_ps_stack_id].info.plmn.identity[0] = 0xFF;
    mm_serving_plmn_sim[mm_ps_stack_id].info.plmn.identity[1] = 0xFF;
    mm_serving_plmn_sim[mm_ps_stack_id].info.plmn.identity[2] = 0xFF;
    mm_serving_plmn_sim[mm_ps_stack_id].lac.lac[0] = 0xFF;
    mm_serving_plmn_sim[mm_ps_stack_id].lac.lac[1] = 0xFF;
    mm_serving_plmn_sim[mm_ps_stack_id].info.plmn_service_capability =  SYS_SRV_CAPABILITY_NOT_AVAILABLE;
    mm_serving_plmn_sim[mm_ps_stack_id].network_op_mode = PS_DOMAIN_SYS_INFO_UNAVAILABLE;
    mm_serving_plmn_sim[mm_ps_stack_id].rac = 0xFF ;  

    if (mm_reg_waiting_for_reg_cnf)
    {
      mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
      mmcoord_mmr_reg_cnf.service_state.reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;
      mmcoord_mmr_reg_cnf.service_state.service_status = SYS_SRV_STATUS_NO_SRV;
      mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
      mmcoord_mmr_reg_cnf.as_id = (sys_modem_as_id_e_type)mm_ps_stack_id;
      if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_LTE_RADIO_ACCESS))
      {
        mmcoord_mmr_reg_cnf.service_state.active_rat = SYS_RAT_LTE_RADIO_ACCESS;
      }
      else if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_TDS_RADIO_ACCESS))
      {
        mmcoord_mmr_reg_cnf.service_state.active_rat = SYS_RAT_TDS_RADIO_ACCESS;
      }
      else 
      {
        mmcoord_mmr_reg_cnf.service_state.active_rat = SYS_RAT_NONE;
      }
      mm_send_mmr_reg_cnf();
      mm_reg_waiting_for_reg_cnf = FALSE;
      mm_ready_to_send_reg_cnf   = FALSE;
    }
    else
    {
      mmr_service_ind_s_type mmr_service_ind;
      mmr_service_ind.service_state = mm_serving_plmn_sim[mm_ps_stack_id].info;
      mmr_service_ind.service_state.reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;
      mmr_service_ind.service_state.service_status = SYS_SRV_STATUS_NO_SRV;
      mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED; 
      if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_LTE_RADIO_ACCESS))
      {
        mmr_service_ind.service_state.active_rat = SYS_RAT_LTE_RADIO_ACCESS;
      }
      else if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info,SYS_RAT_TDS_RADIO_ACCESS))
      {
        mmr_service_ind.service_state.active_rat = SYS_RAT_TDS_RADIO_ACCESS;
      }
      else 
      {
        mmr_service_ind.service_state.active_rat = SYS_RAT_NONE;
      }
      mmr_service_ind.as_id = (sys_modem_as_id_e_type)mm_ps_stack_id;
      mm_send_mmr_service_ind( &mmr_service_ind );
    }
  }
#endif

  else /* Process Inter-RAT Handover/Cell Reselection */
  {
    switch (msg_p->cmd.hdr.message_id)
    {
      case RR_STOP_GSM_MODE_CNF:
        if (mm_rat_handler_status == MM_RAT_STOP_GSM_IN_PROGRESS)
        {
            /* ---------------------------------------------------
            ** GSM is deactivated now - update RAT handler status
            ** --------------------------------------------------- */
            mm_rat_handler_status = MM_RAT_INITIAL;

            /* ---------------------------------
            ** Send ACTIVATION RESPONSE to RRC
            ** --------------------------------- */
 #ifdef FEATURE_SGLTE 
            if (MM_IS_IN_SGLTE_STATE_4&&
             (mm_sglte_current_stack_status == SGLTE_MM_LOWER_LAYER_SPLITING))
            {
               mmcoord_process_spliting();         
               mmsend_mmr_service_stat_ind(mm_cs_stack_id);
               mm_sglte_current_stack_status = SGLTE_INACTIVE_STATE;
            }  
#endif
            mm_send_rrc_activation_rsp( SYS_RAT_GSM_RADIO_ACCESS, active_rat );
        }
        else if (mm_deep_sleep_pending)
        {
            mm_deep_sleep_pending = FALSE;
            mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE;
            mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;
            MSG_HIGH_DS_0(MM_SUB,"=MM= Sending STOP_MODE_CNF for Deep sleep");
#ifdef FEATURE_WRLF_SYSTEM_SEL
            if(mm_wcdma_rlf_state == WCDMA_RLF_STARTED)
            {
              MSG_HIGH_DS_0(MM_SUB,"=MM= STOP_MODE_CNF during RLF deep sleep so setting the service status to 0");
              mm_serving_plmn.info.service_status = SYS_SRV_STATUS_NO_SRV;
            }
#endif
            mm_send_mmr_stop_mode_cnf();
#ifdef FEATURE_LTE
            /* If there is a pending PS detach req then process the request. Otherwise if 
            ** detach confirmation needs to sent then send the confirmation
            */
            if(mm_ps_detach_state == MM_PS_DETACH_STATE_DETACH_REQ_PENDING)
            {
#ifdef FEATURE_MODEM_HEAP
              if(mm_pending_mmr_ps_detach_req_ptr != NULL)
              {
                mmcoord_process_mmr_ps_detach_req(mm_pending_mmr_ps_detach_req_ptr);
                if(mm_ps_detach_state != MM_PS_DETACH_STATE_DETACH_REQ_PENDING)
                {
                  modem_mem_free(mm_pending_mmr_ps_detach_req_ptr,MODEM_MEM_CLIENT_NAS);
                  mm_pending_mmr_ps_detach_req_ptr = NULL;
                }
              }
#else
              mmcoord_process_mmr_ps_detach_req(&mm_pending_mmr_ps_detach_req);
              memset(&mm_pending_mmr_ps_detach_req,0,sizeof(mm_cmd_type));
#endif
            }
            else if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF)
            {
              mm_stop_timer(TIMER_MM_PS_DETACH);
              gmm_gprs_attach_pending = FALSE;
              mm_send_mmr_ps_detach_cnf();
              mm_ps_detach_state = MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ;
            }
#endif
        }
        else
        {
            MSG_HIGH_DS_1(MM_SUB ,"=MM= Ignore RR_STOP_GSM_MODE_CNF in RAT state (%d)",
                       mm_rat_handler_status);
            break;
        }

        break;

      case RRC_STOP_WCDMA_MODE_CNF:
        MSG_HIGH_DS_0(MM_SUB,"=MM= MM received RRC_STOP_WCDMA_MODE_CNF");
        if ((mm_rat_handler_status == MM_RAT_STOP_WCDMA_IN_PROGRESS) ||
            (mm_rat_handler_status == MM_RAT_STOP_TDSCDMA_IN_PROGRESS))
        {
            /* ----------------------------------------------------
            ** WCDMA is deactivated now - update RAT handler status
            ** ---------------------------------------------------- */

#ifdef FEATURE_INTERRAT_HANDOVER_WTOG
            if(mm_sim_card_mode == MMGSDI_APP_USIM)
            {
              /* -----------------------------------------------------------
              ** Write the received CS & PS Hyper Frame Numbers  to the SIM
              ** ----------------------------------------------------------- */
              auth_handle_hfn_release_ind( msg_p->cmd.rrc_stop_wcdma_mode_cnf.start_cs,
                                         msg_p->cmd.rrc_stop_wcdma_mode_cnf.start_ps );
            }
#endif
            /* ---------------------------------
            ** Send ACTIVATION RESPONSE to RR
            ** --------------------------------- */

#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER

            if (( WtoGCCO_state == CCO_INACTIVE )
#ifdef FEATURE_INTERRAT_PCCO_GTOW
                 && (GtoWCCO_state == CCO_INACTIVE )
#endif           
                                                    )
            {
              /* ----------------------------------------------------------------------
              ** Notify SM about RAT resume, SM would inform SNDCP incase of active PDP
              ** contexts. SM layer before sending the Activation RSP to the GSM stack
              ** -------------------------------------------------------------------- */
#ifdef FEATURE_NAS_FREE_FLOAT

#ifdef FEATURE_GSM_GPRS
              if ((gmm_state == GMM_REGISTERED) ||
                  (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) ||
                  (gmm_state == GMM_SERVICE_REQUEST_INITIATED))
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
#endif

#else
              gmm_sm_send_rat_resume_req();
#endif
            }
            else
            {
              mm_rat_handler_status = MM_RAT_INITIAL;
              if ( msg_p->cmd.hdr.message_set == MS_MM_TDSRRC
#ifdef FEATURE_SEGMENT_LOADING
&&get_tdscdma_interface()
#endif
)
              {
                mm_send_rr_activation_rsp(SYS_RAT_TDS_RADIO_ACCESS);
              }
              else if (TRUE
#ifdef FEATURE_SEGMENT_LOADING
&&get_wcdma_interface()
#endif
)
              {
                mm_send_rr_activation_rsp(SYS_RAT_UMTS_RADIO_ACCESS);
              }
            }
#else

            /* ----------------------------------------------------------------------
            ** Notify SM about RAT resume, SM would inform SNDCP incase of active PDP
            ** contexts. SM layer before sending the Activation RSP to the GSM stack
            ** -------------------------------------------------------------------- */
#ifdef FEATURE_NAS_FREE_FLOAT
            
#ifdef FEATURE_GSM_GPRS
            if ((gmm_state == GMM_REGISTERED) ||
                (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) ||
                (gmm_state == GMM_SERVICE_REQUEST_INITIATED))
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
#endif

#else
            gmm_sm_send_rat_resume_req();
#endif

#endif /* FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER */

        }
        else if (mm_deep_sleep_pending)
        {
            mm_deep_sleep_pending = FALSE;
            MSG_HIGH_DS_0(MM_SUB, "=MM= Sending STOP_MODE_CNF for Deep sleep");
            mm_serving_plmn.mm_service_state  = SERVICE_NOT_AVAILABLE;
            mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;
#ifdef FEATURE_WRLF_SYSTEM_SEL
            if(mm_wcdma_rlf_state == WCDMA_RLF_STARTED)
            {
              MSG_HIGH_DS_0(MM_SUB,"=MM= STOP_MODE_CNF during RLF deep sleep so setting the service status to 0");
              mm_serving_plmn.info.service_status = SYS_SRV_STATUS_NO_SRV;
            }
#endif
            mm_send_mmr_stop_mode_cnf();
#ifdef FEATURE_LTE
            /* If there is a pending PS detach req then process the request. Otherwise if 
            ** detach confirmation needs to sent then send the confirmation
            */
            if(mm_ps_detach_state == MM_PS_DETACH_STATE_DETACH_REQ_PENDING)
            {
#ifdef FEATURE_MODEM_HEAP
              if(mm_pending_mmr_ps_detach_req_ptr != NULL)
              {
                mmcoord_process_mmr_ps_detach_req(mm_pending_mmr_ps_detach_req_ptr);
                if(mm_ps_detach_state != MM_PS_DETACH_STATE_DETACH_REQ_PENDING)
                {
                  modem_mem_free(mm_pending_mmr_ps_detach_req_ptr,MODEM_MEM_CLIENT_NAS);
                  mm_pending_mmr_ps_detach_req_ptr = NULL;
                }
              }
#else
              mmcoord_process_mmr_ps_detach_req(&mm_pending_mmr_ps_detach_req);
              memset(&mm_pending_mmr_ps_detach_req,0,sizeof(mm_cmd_type));
#endif
            }
            else if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF)
            {
              mm_stop_timer(TIMER_MM_PS_DETACH);
              gmm_gprs_attach_pending = FALSE;
              mm_send_mmr_ps_detach_cnf();
              mm_ps_detach_state = MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ;
            }
#endif
        }
        else
        {
            MSG_HIGH_DS_1(MM_SUB ,"=MM= Ignore RRC_STOP_WCDMA_MODE_CNF in RAT state (%d)",
                       mm_rat_handler_status);
        }
        break;

      default:
        MSG_ERROR_DS( MM_SUB, "=MM= Unknown message id (%d) rcvd by MM", msg_p->cmd.hdr.message_id, 0, 0);
        break;
    }
    if (mmcoord_reg_req_pending &&
		(mm_rat_handler_status == MM_RAT_INITIAL))
    { 
      mm_serving_plmn.mm_service_state  = SERVICE_NOT_AVAILABLE;
      mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;
      mmcoord_reg_req_pending = FALSE;
      if (mm_state == MM_NULL)  
      {
         mm_activate_state( TRUE );
      }
      mmcoord_process_mmr_reg_req(&mmcoord_pending_reg_message);
    }
#ifdef FEATURE_LTE
    else if(mm_ps_detach_state == MM_PS_DETACH_STATE_DETACH_REQ_PENDING &&
            (mm_rat_handler_status == MM_RAT_INITIAL))
    {
       /* do a local detach */
       mm_stop_timer(TIMER_MM_PS_DETACH);
       mm_ps_detach_state = MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ;
       gmm_detach_type =GMM_GPRS_DETACH;
       gmm_handle_gprs_detach_low_level_failure();
       mm_send_mmr_ps_detach_cnf();
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
#endif
    else if(mm_plmn_list_req_pending &&
            (mm_rat_handler_status == MM_RAT_INITIAL))
    {
      mmcoord_process_mmr_plmn_search_req(&mmcoord_pending_reg_list_req_message);
    }
  }
  if (mm_pended_stop_mode_req.valid &&
      (mm_rat_handler_status == MM_RAT_INITIAL))
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= Processing pended stop mode req (%d)",mm_pended_stop_mode_req.mmr_stop_mode_req.stop_mode_reason);
    mmcoord_reg_req_pending = FALSE;
    mm_plmn_list_req_pending = FALSE;

#ifdef FEATURE_SGLTE
    if(!(msg_p->cmd.hdr.message_id == RR_STOP_GSM_MODE_CNF && 
         MM_IS_IN_SGLTE_DUAL_STACK_MODE && MM_IS_SGLTE_ON_PS_STACK_ID))
#endif
    {   
      if (mm_serving_plmn.info.active_rat == SYS_RAT_NONE)
      {
         mm_serving_plmn.info.active_rat = active_rat;
      }
    }      
    mmcoord_pending_reg_message.cmd.mmr_stop_mode_req= mm_pended_stop_mode_req.mmr_stop_mode_req;
    mmcoord_process_mmr_stop_mode_req(&mmcoord_pending_reg_message);
  }

#ifdef FEATURE_SGLTE
//In Dual stack mode , process pended stop mode on hybrid stack if any
  if( msg_p->cmd.hdr.message_id == RR_STOP_GSM_MODE_CNF &&
      MM_IS_IN_SGLTE_DUAL_STACK_MODE && MM_IS_SGLTE_ON_PS_STACK_ID)
  {
    mm_as_id = mm_cs_stack_id ;
    if (mm_pended_stop_mode_req.valid)
    {
      MSG_HIGH_DS_1(MM_SUB ,"=MM= Processing pended stop mode req (%d)",mm_pended_stop_mode_req.mmr_stop_mode_req.stop_mode_reason);
      mmcoord_reg_req_pending = FALSE;
      mm_plmn_list_req_pending = FALSE;
      if (mm_serving_plmn.info.active_rat == SYS_RAT_NONE)
      {
        mm_serving_plmn.info.active_rat = active_rat;
      }
      mmcoord_pending_reg_message.cmd.mmr_stop_mode_req= mm_pended_stop_mode_req.mmr_stop_mode_req;
      mmcoord_process_mmr_stop_mode_req(&mmcoord_pending_reg_message);
    }
    mm_as_id = mm_ps_stack_id ;
  }
#endif 
} /* mm_handle_stop_mode_cnf () */

/*===========================================================================

FUNCTION MM_HANDLE_SM_RESUME_CNF

DESCRIPTION
  This function is called when SM sends a RAT_RESUME_CNF to MM after notifying the
  SNDCP about the PDP context info during the W to G inter-RAT change

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_handle_sm_resume_cnf (void)
{
  MSG_HIGH_DS_0(MM_SUB, "=MM= Received SM_RAT_RESUME_CNF from SM");

  if (mm_rat_handler_status == MM_RAT_STOP_WCDMA_IN_PROGRESS)
  {
    mm_rat_handler_status = MM_RAT_INITIAL;
    mm_send_rr_activation_rsp(SYS_RAT_UMTS_RADIO_ACCESS);
  }
  else if (mm_rat_handler_status == MM_RAT_STOP_TDSCDMA_IN_PROGRESS)
  {
#ifdef FEATURE_SGLTE  
    if(MM_IS_IN_SGLTE_MODE && (mm_sglte_current_stack_status ==SGLTE_MM_LOWER_LAYER_MERGING))
    {
      if (mm_waiting_for_stop_mode_cnf == FALSE ) 
      {
        mmsend_mmr_service_stat_ind(mm_ps_stack_id);
      }
      mmcoord_process_merging();
      mm_sglte_current_stack_status=SGLTE_INACTIVE_STATE;
    }  
#endif 
    mm_rat_handler_status = MM_RAT_INITIAL;
    mm_send_rr_activation_rsp(SYS_RAT_TDS_RADIO_ACCESS);
  }

  if (mmcoord_reg_req_pending)
  { 
    mm_serving_plmn.mm_service_state  = SERVICE_NOT_AVAILABLE;
    mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;
    mmcoord_reg_req_pending = FALSE;
    mmcoord_process_mmr_reg_req(&mmcoord_pending_reg_message);
  }
#ifdef FEATURE_LTE
  else if(mm_ps_detach_state == MM_PS_DETACH_STATE_DETACH_REQ_PENDING)
  {
     /* do a local detach */
    mm_stop_timer(TIMER_MM_PS_DETACH);
    mm_ps_detach_state = MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ;
    gmm_detach_type =GMM_GPRS_DETACH;
    gmm_handle_gprs_detach_low_level_failure();
    mm_send_mmr_ps_detach_cnf();
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
#endif
  else if(mm_plmn_list_req_pending)
  {
    mmcoord_process_mmr_plmn_search_req(&mmcoord_pending_reg_list_req_message);
  }
 
  if (mm_pended_stop_mode_req.valid)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= Processing pended stop mode req (%d)",mm_pended_stop_mode_req.mmr_stop_mode_req.stop_mode_reason);
    mmcoord_reg_req_pending = FALSE;
    mm_plmn_list_req_pending = FALSE;
    mmcoord_pending_reg_message.cmd.mmr_stop_mode_req= mm_pended_stop_mode_req.mmr_stop_mode_req;
    mmcoord_process_mmr_stop_mode_req(&mmcoord_pending_reg_message);
  }
}


/*===========================================================================

FUNCTION MM_SEND_RR_ACTIVATION_RSP

DESCRIPTION
  This function sends activation response to RR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rr_activation_rsp (sys_radio_access_tech_e_type last_active_rat)
{
  byte index;
  rr_activation_rsp_T rr_activation_rsp;
  word ota_lac = 0;

#ifdef FEATURE_LTE
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_ctrl_data_ptr != NULL);
#endif

  memset(&rr_activation_rsp,0x00,sizeof(rr_activation_rsp_T));
  mm_serving_plmn.info.active_rat = SYS_RAT_GSM_RADIO_ACCESS;
  mmsend_rat_change_event();
  rr_activation_rsp.message_header.message_set = MS_MM_RR;
  rr_activation_rsp.message_header.message_id  = (int) RR_ACTIVATION_RSP;
  rr_activation_rsp.service_domain = mm_serving_plmn.ms_op_mode;

  rr_activation_rsp.any_cell_selection_mode =
   sys_plmn_id_is_undefined(mm_serving_plmn.info.plmn);

  for (index=0; index < PLMN_SIZE; index++)
  {
    rr_activation_rsp.PLMN_id.identity[index] =
            mm_serving_plmn.info.plmn.identity[index];
  }

  /* ----------------------------------------------
  ** Propagate GPRS registration information to RR
  ** ---------------------------------------------- */
  rr_activation_rsp.PS_Attached =
               ((gmm_state == GMM_REGISTERED) ||
                (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) ||
                (gmm_state == GMM_SERVICE_REQUEST_INITIATED));

  PUT_IMH_LEN( sizeof( rr_activation_rsp_T ) - sizeof( IMH_T ),
               &rr_activation_rsp.message_header );

#ifndef FEATURE_NAS_FREE_FLOAT
  /* ---------------------------------------------------------------------
  ** If the UE is already attached on PS domain, and the Inter RAT change
  ** occured to GPRS, then initiate the LLC and SNDCP layers
  ** -------------------------------------------------------------------*/
#ifdef FEATURE_GSM_GPRS
  if (rr_activation_rsp.PS_Attached)
  {
#ifdef FEATURE_INTERRAT_PCCO_GTOW
    if ( GtoWCCO_state == CCO_INACTIVE )
#endif
    {
      gmmllc_reinit(GMM_STANDBY_STATE) ;
    }
  }
#endif
#endif

  /* Provide the RAC to the RR to be used in GRR suspension request */
  rr_activation_rsp.routing_area_code = mm_serving_plmn.rac;
  memscpy(&rr_activation_rsp.location_area_code,sizeof(word), &mm_serving_plmn.lac, 2);
  /* ---------------------------------------------------------------------------
  ** Per 10.5.1.3 of TS24.008, first octet in LAC is the most significant octet
  ** and the second octet in LAC is the least significant octet.
  ** --------------------------------------------------------------------------- */
  ota_lac = (word) ((rr_activation_rsp.location_area_code & 0xff00) >> 8);
  ota_lac = (word) (ota_lac | ((rr_activation_rsp.location_area_code & 0x00ff) << 8));
  rr_activation_rsp.location_area_code = ota_lac;

#ifdef FEATURE_LTE
  if(((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI) &&
      (emm_ctrl_data_ptr->emm_guti_valid == TRUE)

#ifdef FEATURE_SGLTE
                            &&     MM_IS_SGLTE_ON_PS_STACK_ID)
#else
                                    )
#endif

      )
  {
    emm_map_guti_to_rai(&rr_activation_rsp.registered_rai, emm_ctrl_data_ptr);
  }
  else if(((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_P_TMSI ||
              emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI) &&
              mm_is_rai_valid()


#ifdef FEATURE_SGLTE
                            &&     MM_IS_SGLTE_ON_PS_STACK_ID)
#else
                                    )
#endif
              )
  {
    memscpy((void *)&rr_activation_rsp.registered_rai,sizeof(routing_area_id_T),
                  (void *)&gmm_stored_rai,sizeof(routing_area_id_T)) ;     
  }
  else if(mm_is_rai_valid())
  {
    memscpy((void *)&rr_activation_rsp.registered_rai,sizeof(routing_area_id_T),
                  (void *)&gmm_stored_rai,sizeof(routing_area_id_T)) ;    
  }
  else if( emm_ctrl_data_ptr->emm_guti_valid == TRUE

#ifdef FEATURE_SGLTE
                            &&     MM_IS_SGLTE_ON_PS_STACK_ID
#endif
  )
  {
    emm_map_guti_to_rai(&rr_activation_rsp.registered_rai, emm_ctrl_data_ptr);
  }
  else
  {
     /*This will only happen when GMM is not in registered state
        This will sned registered_rai as invalid to GRR */
    memscpy((void *)&rr_activation_rsp.registered_rai,sizeof(routing_area_id_T),
                  (void *)&gmm_stored_rai,sizeof(routing_area_id_T)) ;    
  }
#else /*FEATURE_LTE*/
  memscpy((void *)&rr_activation_rsp.registered_rai,sizeof(routing_area_id_T),
                  (void *)&gmm_stored_rai,sizeof(routing_area_id_T)) ;  

#endif   /*FEATURE_LTE */


  MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent RR_ACTIVATION_RSP");
  mm_send_message( (IMH_T *)&rr_activation_rsp, GS_QUEUE_RR );

  /* -----------------------------------------
  ** Transition to MM_INTERRAT_CHANGE_STATE
  ** ----------------------------------------- */
  if ((last_active_rat == SYS_RAT_UMTS_RADIO_ACCESS) ||
      (last_active_rat == SYS_RAT_TDS_RADIO_ACCESS))
  {
    if((gmm_state != GMM_NULL) && (gmm_state != GMM_DEREGISTERED))
    {
      mm_interrat_state =  MM_UMTS_TO_GSM_INTERRAT_CHANGE_IN_PROGRESS;
    }
  }
#ifdef FEATURE_LTE
  else if (last_active_rat == SYS_RAT_LTE_RADIO_ACCESS)
  {
    mm_interrat_state =  MM_LTE_TO_GSM_INTERRAT_CHANGE_IN_PROGRESS;
  }
#endif
  mm_to_only_reg         = FALSE;  
  mm_previous_serving_rat     = SYS_RAT_NONE;
  mm_needs_to_send_mmr_service_ind = TRUE;  
} /* mm_send_rr_activation_rsp () */


/*===========================================================================

FUNCTION MM_SEND_RRC_ACTIVATION_RSP

DESCRIPTION
  This function sends activation response to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_activation_rsp
(
  sys_radio_access_tech_e_type last_active_rat,
  sys_radio_access_tech_e_type current_rat
)
{
#ifdef FEATURE_WCDMA
  rrc_activation_rsp_type *rrc_activation_rsp_p ;
  rrc_cmd_type *rrc_cmd;
#endif

  mm_serving_plmn.info.active_rat = current_rat;

  mmsend_rat_change_event();

#ifdef FEATURE_WCDMA
  rrc_cmd = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );

  if(rrc_cmd == NULL)
  {
    MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
  }
  rrc_activation_rsp_p = &(rrc_cmd->cmd.activation_rsp);
  rrc_cmd->cmd_hdr.cmd_id = RRC_ACTIVATION_RSP;
  rrc_activation_rsp_p->plmn_id =
    mmumts_convert_nas_plmn_id_to_rrc_plmn_id(mm_serving_plmn.info.plmn);

  send_cmd_to_rrc( rrc_cmd, mm_serving_plmn.info.active_rat );
#endif /* FEATURE_WCDMA */

  /* -----------------------------------------
  ** Transition to MM_INTERRAT_CHANGE_STATE
  ** ----------------------------------------- */
  if(last_active_rat == SYS_RAT_GSM_RADIO_ACCESS)
  {
    if ((gmm_state != GMM_NULL) && (gmm_state != GMM_DEREGISTERED))
    {
      mm_interrat_state =  MM_GSM_TO_UMTS_INTERRAT_CHANGE_IN_PROGRESS;
    }
  
    /* ----------------------------------------------------------
    ** Transition out of GMM suspended state during G2W handover
    ** ---------------------------------------------------------- */
  #ifdef FEATURE_GSM_GPRS
    gmm_handle_gprs_resumption_during_g2w_ho ();
  #endif
  }
#ifdef FEATURE_LTE
  else if(last_active_rat == SYS_RAT_LTE_RADIO_ACCESS)
  {
    mm_interrat_state =  MM_LTE_TO_UMTS_INTERRAT_CHANGE_IN_PROGRESS;  
  }
#endif
  mm_to_only_reg         = FALSE; 
  mm_previous_serving_rat     = SYS_RAT_NONE;

  mm_needs_to_send_mmr_service_ind = TRUE;
} /* mm_send_rrc_activation_rsp () */

/*===========================================================================

FUNCTION MM_SEND_ACTIVATION_RSP_IF_NEEDED

DESCRIPTION
  This function sends activation response to RR/RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_activation_rsp_if_needed (void)
{
  if (mm_rat_handler_status == MM_RAT_STOP_TDSCDMA_IN_PROGRESS)
  {     
    mm_send_rr_activation_rsp(SYS_RAT_TDS_RADIO_ACCESS);
  }
  else if (mm_rat_handler_status == MM_RAT_STOP_WCDMA_IN_PROGRESS)
  {
    mm_send_rr_activation_rsp(SYS_RAT_UMTS_RADIO_ACCESS);
  }
  else if (mm_rat_handler_status == MM_RAT_STOP_GSM_IN_PROGRESS)
  {
    mm_send_rrc_activation_rsp( SYS_RAT_GSM_RADIO_ACCESS,
                                     mm_serving_plmn.info.active_rat );
  }  

  mm_rat_handler_status = MM_RAT_INITIAL;
}


/*===========================================================================

FUNCTION MM_SEND_RR_STOP_MODE_REQ

DESCRIPTION
  This function sends stop mode request to RR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rr_stop_mode_req( rr_mm_gsm_stop_reason_T stop_reason )
{
  
  if (!mm_waiting_for_stop_mode_cnf)
  {
    rr_stop_gsm_mode_req_T rr_stop_mode_req;
    memset(&rr_stop_mode_req,0x00,sizeof(rr_stop_gsm_mode_req_T));
    rr_stop_mode_req.message_header.message_set = MS_MM_RR;
    rr_stop_mode_req.message_header.message_id  = (int) RR_STOP_GSM_MODE_REQ;
    rr_stop_mode_req.rr_mm_gsm_stop_reason = stop_reason;

    PUT_IMH_LEN( sizeof(rr_stop_gsm_mode_req_T) - sizeof(IMH_T),
                 &rr_stop_mode_req.message_header);

    mm_send_message( &rr_stop_mode_req.message_header, GS_QUEUE_RR );

    MSG_HIGH_DS_0(MM_SUB, "=MM=  MM sent a RR_STOP_GSM_MODE_REQ");

    if(stop_reason == RR_MM_USER_RAT_CHANGE
#ifdef FEATURE_SGLTE
      || stop_reason == RR_SGLTE_USER_RAT_CHANGE
#endif
      )
    {
      mm_serving_plmn.mm_service_state  = SERVICE_NOT_AVAILABLE;
      mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;
    }
    mm_waiting_for_stop_mode_cnf = TRUE;
  }
}/* mm_send_rr_stop_mode_req() */


/*===========================================================================

FUNCTION MM_SEND_RRC_STOP_MODE_REQ

DESCRIPTION
  This function sends stop mode request to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_rrc_stop_mode_req
(
  rrc_stop_cause_e_type        stop_reason,
  sys_radio_access_tech_e_type rat
)
{
#ifdef FEATURE_WCDMA
  rrc_cmd_type *rrc_cmd;
#endif

  if (!mm_waiting_for_stop_mode_cnf)
  {
#ifdef FEATURE_WCDMA
      rrc_cmd = mm_get_rrc_cmd_buf( rat );

    if (rrc_cmd == NULL)
    {
      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
    }
    rrc_cmd->cmd_hdr.cmd_id = RRC_STOP_WCDMA_MODE_REQ;
    rrc_cmd->cmd.stop_wcdma_mode_req.cause = stop_reason;

  
    send_cmd_to_rrc( rrc_cmd, rat );
#endif /* FEATURE_WCDMA */

    if(stop_reason == RRC_USER_RAT_CHANGE)
    {
      mm_serving_plmn.mm_service_state  = SERVICE_NOT_AVAILABLE;
      mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;
    }
    mm_waiting_for_stop_mode_cnf = TRUE;
	mm_reject_call_on_rai_change = FALSE;
  }
}/* mm_send_rrc_stop_mode_req() */


/*===========================================================================

FUNCTION MM_RAT_HANDLE_NEW_MSG

DESCRIPTION
  This function sends activation response to RR/RRC when there is an input
  message on MM queue and MM is waiting for stop mode response from RR/RRC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_rat_handle_new_msg (mm_cmd_type *msg_p)
{
 
  if (((((msg_p->cmd.hdr.message_set == MS_CC_MM)|| (msg_p->cmd.hdr.message_set == MS_CC_MM_PS)) &&
       ((msg_p->cmd.hdr.message_id  == MMCNM_REL_REQ) ||
#ifdef FEATURE_CCBS
        (msg_p->cmd.hdr.message_id  == MMCNM_PROMPT_RES) ||
#endif
        (msg_p->cmd.hdr.message_id  == MMCNM_DATA_REQ)))
      ||
      ((msg_p->cmd.hdr.message_set == MS_GMM_SM) &&
       ((msg_p->cmd.hdr.message_id  == GMMSM_UNITDATA_REQ) ||
        (msg_p->cmd.hdr.message_id  == GMMSM_CLOSE_SESSION_REQ) ||
        (msg_p->cmd.hdr.message_id  == GMMSM_FAST_DORMANCY_REQ)))
      ||
      ((msg_p->cmd.hdr.message_set == MS_LLC_GMM) &&
       (msg_p->cmd.hdr.message_id  == GLLCMM_LL_UNITDATA_IND))
      ||
      (((msg_p->cmd.hdr.message_set == MS_MM_RRC)|| (msg_p->cmd.hdr.message_set == MS_MM_TDSRRC)) &&      
       ((msg_p->cmd.hdr.message_id  == RRC_DATA_IND)||
        (msg_p->cmd.hdr.message_id  == RRC_SYNC_IND)||
        (msg_p->cmd.hdr.message_id  == RRC_SERVICE_IND_FROM_OTA_MSG)))
      ||
      ((msg_p->cmd.hdr.message_set == MS_MM_RR)&&
       ((msg_p->cmd.hdr.message_id  == RR_PLMN_SELECT_REQ)||
        (msg_p->cmd.hdr.message_id  == RR_STOP_GSM_MODE_REQ)||
        (msg_p->cmd.hdr.message_id  == RR_DATA_IND)||
        (msg_p->cmd.hdr.message_id  == RR_SYNC_IND)||
#ifdef FEATURE_DUAL_SIM
        (msg_p->cmd.hdr.message_id  == RR_DS_DATA_IND)||
#endif
        (msg_p->cmd.hdr.message_id  == RR_SERVICE_IND_FROM_OTA_MSG))))
      &&(msg_p->cmd.hdr.message_set !=  MS_LLC_GMM ||
          msg_p->cmd.hdr.message_id != (int) GLLCSMS_LL_XID_IND) 
     )
  {
    switch (mm_rat_handler_status)
    {
      case MM_RAT_STOP_WCDMA_IN_PROGRESS:
      case MM_RAT_STOP_TDSCDMA_IN_PROGRESS:
        if (mm_rat_handler_status == MM_RAT_STOP_TDSCDMA_IN_PROGRESS)
        {
#ifdef FEATURE_SGLTE  
          if(MM_IS_IN_SGLTE_MODE && (mm_sglte_current_stack_status ==SGLTE_MM_LOWER_LAYER_MERGING))
           {    
            
             if (mm_waiting_for_stop_mode_cnf == FALSE ) 
             {
               mmsend_mmr_service_stat_ind(mm_ps_stack_id);
             }
             mmcoord_process_merging();
             mm_sglte_current_stack_status=SGLTE_INACTIVE_STATE;
           }  
#endif             
          mm_send_rr_activation_rsp(SYS_RAT_TDS_RADIO_ACCESS);
        }
        else
        {
          mm_send_rr_activation_rsp(SYS_RAT_UMTS_RADIO_ACCESS);
        }
        mm_rat_handler_status = MM_RAT_INITIAL;
        mm_integrity_protected = FALSE;
        break;

      case MM_RAT_STOP_GSM_IN_PROGRESS:
        mm_rat_handler_status = MM_RAT_INITIAL;
#ifdef FEATURE_SGLTE
        if(MM_IS_IN_SGLTE_STATE_4 && (mm_sglte_current_stack_status ==SGLTE_MM_LOWER_LAYER_SPLITING))
        {    
          mmcoord_process_spliting();
          mmsend_mmr_service_stat_ind(mm_cs_stack_id);          
          mm_sglte_current_stack_status=SGLTE_INACTIVE_STATE;
          mm_waiting_for_stop_mode_cnf_sim[mm_ps_stack_id]=FALSE;
        }  
#endif 
        mm_send_rrc_activation_rsp( SYS_RAT_GSM_RADIO_ACCESS,
                                    mm_serving_plmn.info.active_rat );
        break;

      default:
        break;
    }
  }
}/* mm_rat_handle_new_msg() */


/*===========================================================================

FUNCTION MM_RAT_HANDLE_SRV_STATUS_UPDATE

DESCRIPTION
  This function handles service status changes and ensures PS data continuity
  during RAT changes due to loss of service. Data services are suspended upon
  loss of service and resumed upon regaining service on the new RAT.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_rat_handle_srv_status_update ( sys_srv_status_e_type service_status )
{
  mmr_service_ind_s_type mmr_service_ind;

#if defined(FEATURE_LTE)
   emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
   emm_ctrl_data_ptr = emm_db_get_ctrl_data();
   ASSERT(emm_ctrl_data_ptr != NULL);
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((gmm_state == GMM_REGISTERED) ||
      (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) ||
      (gmm_state == GMM_SERVICE_REQUEST_INITIATED) ||
      (gmm_state == GMM_DEREGISTERED_INITIATED))
  {
    switch ( service_status )
    {
      case SYS_SRV_STATUS_NO_SRV:
        MSG_HIGH_DS_1(MM_SUB ,"=MM=  SRV update during NO SRV mm_service_loss_on_rat %d",
                   mm_service_loss_on_rat );
        if ((mm_service_loss_on_rat == SYS_RAT_NONE) &&
            (mm_serving_plmn.gmm_service_state    == SERVICE_NOT_AVAILABLE) 
  #ifdef FEATURE_LTE
            && ((mm_service_loss_on_multimode_rat == SYS_RAT_NONE) 
#ifdef FEATURE_SGLTE
             && (MM_IS_SGLTE_ON_PS_STACK_ID)
#endif
            )
  #endif 
           )
        {
          mm_service_loss_on_rat               = mm_serving_plmn.info.active_rat;
          mm_serving_plmn.info.ps_data_suspend = TRUE;
#ifdef FEATURE_GSM_GPRS
          gmm_rau_is_required = TRUE;
#endif /* #ifdef FEATURE_GSM_GPRS */
        }
        break;

      case SYS_SRV_STATUS_SRV:
        MSG_HIGH_DS_3(MM_SUB, "=MM=  SRV update during SRV gmm_substate %d, PS data suspension status %d Active RAT %d",
                   gmm_substate , mm_serving_plmn.info.ps_data_suspend, mm_serving_plmn.info.active_rat);
        if ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode !=
              SYS_NETWORK_SELECTION_MODE_LIMITED) &&
            (mm_serving_plmn.info.ps_data_suspend))
        {
#ifdef FEATURE_GSM_GPRS        
          if (is_umts_mode() && (gmmllc_get_glif_state() != GMM_IDLE_STATE) )
          {
            gmmllc_reinit(GMM_IDLE_STATE);
          }
#endif

          if 
             (
             (mm_serving_plmn.info.active_rat == gmm_last_signalling_sent_rat)
/*4.7.1.7a	Intersystem change from S1 mode to A/Gb mode or S1 mode to Iu mode with ISR activated*/
#if defined(FEATURE_LTE)
              ||
		     (
#ifdef FEATURE_SGLTE
             ( 
#endif
                ( (mm_service_loss_on_multimode_rat == SYS_RAT_LTE_RADIO_ACCESS) &&
                    (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI) )
#ifdef FEATURE_SGLTE
                  && (MM_IS_SGLTE_ON_PS_STACK_ID)
             )
#endif
#ifdef FEATURE_NAS_REL11
			  && (mm_nv_nas_rel_compliance < NV_NAS_COMPLIANCE_REL11)
#endif
			  )
#endif /* FEATURE_LTE */
             )
          {
            if (((gmm_update_status == GMM_GU1_UPDATED) &&
                (!(gmm_rai_changed( gmm_stored_rai.location_area_code,
                                    gmm_stored_rai.routing_area_code,
                                    mm_serving_plmn.lac, mm_serving_plmn.rac ) ||
                   gmm_plmn_changed(gmm_stored_rai.plmn_id,mm_serving_plmn.info.plmn)))) ||
                (gmm_substate == GMM_LIMITED_SERVICE))

            {
              mm_serving_plmn.info.ps_data_suspend = FALSE;
#ifdef FEATURE_GSM_GPRS
              if( gmm_is_suspended == FALSE )
              {
                gmm_rau_is_required                  = FALSE;
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
                gmm_is_prev_mode_connected  = FALSE;
#endif
              } 
#endif /* #ifdef FEATURE_GSM_GPRS */

              /* -------------------------------------------------------
              ** Send indication to REG that PS data no longer suspended
              ** ------------------------------------------------------- */
              if((gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) &&
                 ((gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) ||
                  (gmm_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH)) &&
                  (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY))
              {
                MSG_HIGH_DS_1(MM_SUB, "=MM= Combined RAU is in progress, inform srv_status after rau completion, reg_srv_domain: %d", mm_serving_plmn.info.reg_service_domain);
              }
              else
              {
                if (mm_reg_waiting_for_reg_cnf)
                {
                  if (mm_ready_to_send_reg_cnf)
                  {
                    mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
                    mm_send_mmr_reg_cnf();

                    mm_reg_waiting_for_reg_cnf = FALSE;
                    mm_ready_to_send_reg_cnf   = FALSE;
                  }
                }
                else
                {
                  mmr_service_ind.service_state = mm_serving_plmn.info;
                  mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                  mm_send_mmr_service_ind( &mmr_service_ind );
                }
              }
            }
#if defined(FEATURE_LTE) && defined(FEATURE_GSM_GPRS)
            if(is_gsm_mode() && 
               (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_RAT_RELATED_TMSI) &&
               (mm_service_loss_on_multimode_rat == SYS_RAT_LTE_RADIO_ACCESS)
#ifdef FEATURE_SGLTE
               &&(MM_IS_SGLTE_ON_PS_STACK_ID)
#endif
               )
            {
              /* Resume LLC */
              gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_LLC_RESUMPTION,FALSE,0);
            }
#endif
          }
          
          if ((mm_service_loss_on_rat != mm_serving_plmn.info.active_rat) &&
              (mm_service_loss_on_rat != SYS_RAT_NONE))
          {
            gmm_sm_send_rat_change_req(mm_service_loss_on_rat    
                                       #if defined(FEATURE_LTE)
                                       , FALSE
                                       #endif
                                      );
            gmm_sm_send_rat_resume_req();
          }
        }
#if defined(FEATURE_LTE)
        if ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode !=
                     SYS_NETWORK_SELECTION_MODE_LIMITED) &&
                    (mm_service_loss_on_multimode_rat == SYS_RAT_LTE_RADIO_ACCESS))
        {
          gmm_sm_send_rat_resume_req();
          mm_send_cm_bearer_ctxt_transfer_ind(mm_serving_plmn.info.active_rat);
        }
        mm_service_loss_on_multimode_rat = SYS_RAT_NONE;
#endif
        mm_service_loss_on_rat = SYS_RAT_NONE;
        break;

      default:
        break;
    }
  }
}
/*===========================================================================
 
FUNCTION    mm_stop_current_rat

DESCRIPTION

This function takes care of actions to be  done before stopping the current rat

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
===========================================================================*/
void mm_stop_current_rat(void)
{
#ifdef FEATURE_NAS_FREE_FLOAT
  if(is_gsm_mode() && mm_timer_status[TIMER_NW_INIT_DETACH] == TIMER_ACTIVE 
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM             
                    && gmm_last_signalling_message_type == GMM_DETACH_ACCEPT)
#endif                           
  {
#ifdef FEATURE_LTE
    if(mm_ps_detach_state == MM_PS_DETACH_STATE_WAITING_TO_SEND_CNF)
    {
       mm_send_mmr_ps_detach_cnf();
       mm_ps_detach_state = MM_PS_DETACH_STATE_WAITING_FOR_REG_REQ;
    }
#endif
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
  mm_stop_rat_mode();
}
