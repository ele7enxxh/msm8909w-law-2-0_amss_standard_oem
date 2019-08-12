/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmplmnsrch.c_v   1.17   17 Jul 2002 13:28:02   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmplmnsrch.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/14   jca     Fixed bug in RRC_LAI_LIST_CNF processing.
06/03/01   sbk     Global variables are renamed to use mm_ prefix
08/03/01   jca     Fixed bug where MMR_PLMN_SEARCH_CNF contents were overwritten.
08/07/01   ts      Standardized MSG_ERROR and MSG_HIGH usage.
09/18/01   sbk     Intertask primitives are logged and QXDM logging is supported
04/24/02   jca     Moved function to eliminate prototype from mm.h
06/05/02   ks      Added support for Dual mode operation.
07/03/02   jca     Deleted function mm_plmn_list_control().
07/11/02   ks      Now passing RAT in the mm_request_list_from_rr() function.
                   Setting the proper length of the received PLMN List.
08/02/02   ks      Replaced immediate_srch with network_selection_mode
08/15/02   ks      Setting the search_mode in rr_plmn_list_req to either
                   BACKGROUND/IMMEDIATE based on the network_selection_mode
12/12/02   ks      Replaced mm_sim_valid with function mmsim_sim_valid.
12/13/02   mks     Included header file mm_sim.h to fix compiler warning
02/10/03   cd      Fixed lint error
02/14/03   mks     Now setting the mm_serving_plmn.gmm_service_state to
                   SERVICE_NOT_AVAILABLE upon receiving MMR_PLMN_SEARCH_REQ.
04/14/03   ks      Setting MM and GMM service as not available for AUTOMATIC
                   PLMN SRCH request, as it is now supported in lower layers.
06/16/03   jca     Now consistently ERR_FATAL if attempt to allocate memory fails.
08/29/03   cd      Added support for passing the band_preference between REG and RR
12/04/03   ks      Sending the PLMN_LIST_CNF with search_mode to FALSE when a
                   AUTOMATIC search is requested in RRC connected mode.
2/24/04    ks      Fixed a merge issue where the paranthesis was wrongly placed.
3/31/04    ks      If a Data call is up, the RRC connection is not aborted to
                   process background PLMN search.
04/01/04   ks      If background PLMN feature is enabled in GSM, no need to change
                   the MM/GMM service state to NO_SERVICE.
05/12/04   cd      Added support for WCDMA multiband feature.
                   Fixed Lint errors.
05/19/04   cd      Now sending the band_pref in a RR(C)_PLMN_LIST_REQ if either GSM or WCDMA
                   multiband feature is defined
08/02/03   ks      Now aborting RR connection and proceeding with the PLMN search request
                   after receiving the CNF when connection establishment is in progress.
10/12/04   jca     Modified PLMN_LIST_REQ processing to be consistent with new RR
                   implementation of foreground/background PLMN list searching.
11/16/04   jca     Added delay_sig_con_release flag in RRC_ABORT_REQ.
11/14/04   ks      Now passing higher_priority_plmn_list from REG to RR for 
                   Background 3G PLMN search while UE is in 2G.
12/01/04   ks      Now sending background PLMN list request to 2G with PDP is active
                   also rearranged the featurization for better readability. 
01/24/05   abe     Use the dual_mode flag value received through MMR_PLMN_SEARCH_REQ in 
                   RR_PLMN_LIST_REQ instead of taking the value from global variable 
                   mm_dual_mode.
04/18/05   ks      Now setting mmcoord_reg_req_pending to FALSE when a Manual
                   PLMN selection is requested. 
05/23/05   abe     Set the value of the field auth_failed_bar_cells to FALSE in RR(C)_ABORT_REQ
08/08/05   abe     Resetting the mm_plmn_list_req_pending flag to FALSE 
                   after sending PLMN_LIST_REQ to RR or RRC
10/11/05   up      Updated to trigger the BPLMN search when in 3G.
                   Also allow the BPLMN search during PS call.
10/13/05   up      Added code to handle the RRC_LIST_CNF(MM_AS_ABORTED)
                   when BPLMN search is aborted by RRC.
10/20/05   up      Updated for 3G to 2G BPLMN Search.
10/24/05   up      Update net selection mode in MMR_PLMN_SERCH_CNF
                   which is received in RR/RRC PLMN_LIST_CNF.
01/18/06   up      transaction id is added in PLMN_SEARCH_CNF.
05/22/06   ajt     Initialize active_rat while sending plmn_list_req as sending list req
                   activates a RAT.
06/14/06   up      Called new mm_update_mm_serving_plmn_info for GSTK event.
11/23/06   sn      FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
11/11/11   gps    Initiated RRC establish reqquest with all zeros.
09/12/11   HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                   as a part of CMI activity
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "ms.h"
#include "timers_v.h"
#include "gs_v.h"
#include "reg_mm_v.h"
#include "mm_rr.h"
#include "mm_v.h"
#include "rrcmmif.h"
#include "rrccmd.h"
#include "mmtask.h"
#include "mmtask_int.h"
#include "mm_umts_v.h"
#include "err.h"
#include "mm_sim.h"
#include "mm_coord.h"
#include "sm_gmm.h"
#include "sm_v.h"
#include "mm_as_types.h" 

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

/* Declare the mm_plmn_list_req_pending variable */
boolean mm_plmn_list_req_pending_sim[MAX_NAS_STACKS];

/* Declare mm_plmn_search_in_progress variable*/
boolean mm_plmn_search_in_progress_sim[MAX_NAS_STACKS];

/*
** This is to remember the last LIST request type (AUTO/MANUAL) sent to RR/RRC.
** In IDLE mode, inter-RAT reselection has to be ignored if MM is waiting for LIST CNF - MANUAL.
*/
sys_network_selection_mode_e_type mm_waiting_for_list_cnf_type_sim[MAX_NAS_STACKS];

#define  mm_waiting_for_list_cnf_type mm_waiting_for_list_cnf_type_sim[mm_as_id]

#else

/* Declare the mm_plmn_list_req_pending variable */
boolean mm_plmn_list_req_pending ;

/* Declare mm_plmn_search_in_progress variable*/
boolean mm_plmn_search_in_progress;

/*
** This is to remember the last LIST request type (AUTO/MANUAL) sent to RR/RRC.
** In IDLE mode, inter-RAT reselection has to be ignored if MM is waiting for LIST CNF - MANUAL.
*/
sys_network_selection_mode_e_type mm_waiting_for_list_cnf_type;

#endif /* FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/

#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE))
 #error code not present
#endif // TEST_FRAMEWORK

/*===========================================================================

FUNCTION MM_GET_PLMN_LIST

DESCRIPTION
  This function deals with messages relating to PLMN searches.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
void mm_get_plmn_list(mm_cmd_type* msg_p)
{
   uint32 entry, num_plmn;

   mmr_plmn_search_cnf_s_type  *mmr_plmn_search_cnf;

   mmr_service_ind_s_type     mmr_service_ind;
#ifndef FEATURE_MODEM_HEAP
   mmr_plmn_search_cnf = (mmr_plmn_search_cnf_s_type*)gs_alloc(sizeof(mmr_plmn_search_cnf_s_type));
#else
   mmr_plmn_search_cnf = (mmr_plmn_search_cnf_s_type*)modem_mem_calloc(1, sizeof(mmr_plmn_search_cnf_s_type),MODEM_MEM_CLIENT_NAS);
#endif
   mm_check_for_null_ptr((void*)mmr_plmn_search_cnf);   
   switch ( msg_p->cmd.hdr.message_set )
   {
      case MS_MM_REG:
         if (msg_p->cmd.hdr.message_id == (int)MMR_PLMN_SEARCH_REQ)
         {
            /*
             * Handle the request from MMR for a list of the
             * available PLMNs. The status of 'present_all_PLMNs' is stored
             * so that when the list is received from RR, the decision can
             * be taken as to whether to filter out the unavailable PLMNs
             * or not. The actual search is either delayed until the search
             * timer matures, or done immediately, according to the content
             * of immediate search.
             */

            if (msg_p->cmd.mmr_plmn_search_req.network_selection_mode ==
                  SYS_NETWORK_SELECTION_MODE_MANUAL)
            {
               MSG_HIGH_DS_0(MM_SUB, "=MM= PLMN LIST REQ : Manual selection");

               if (mmcoord_reg_req_pending)
               {
                 MSG_HIGH_DS_0(MM_SUB, "=MM= Manual List request overwrites REG_REQ");
                 mmcoord_reg_req_pending = FALSE;
               }
#ifdef FEATURE_WRLF_SYSTEM_SEL
               if(msg_p->cmd.mmr_plmn_search_req.service_search == SYS_SERVICE_SEARCH_NONE &&
                  mm_wcdma_rlf_state != WCDMA_RLF_IDLE && rrc_connection_status != MMRRC_IDLE)
               {
#ifdef FEATURE_WCDMA
                 rrc_cmd_type *rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );
                 if(rrc_cmd_p == NULL) 
                 {
                   ERR_FATAL( "ERROR: Unable to allocate memory", 0,0,0 );
                 }
                 /* Send an Abort Request to RRC */
                 rrc_cmd_p->cmd_hdr.cmd_id = RRC_ABORT_REQ;
                 rrc_cmd_p->cmd.rrc_abort_req.delay_sig_con_release = FALSE;
                 rrc_cmd_p->cmd.rrc_abort_req.auth_failed_bar_cells = FALSE;
                 rrc_cmd_p->cmd.rrc_abort_req.hard_abort = TRUE;
                 rrc_cmd_p->cmd.rrc_abort_req.abort_cause = RRC_OTHERS;
                 send_cmd_to_rrc( rrc_cmd_p, mm_serving_plmn.info.active_rat );
#endif /* FEATURE_WCDMA */

               }
#endif
               mm_request_list_from_rr( &msg_p->cmd.mmr_plmn_search_req );
            }
            else if (msg_p->cmd.mmr_plmn_search_req.network_selection_mode ==
                       SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
            {
               /* ---------------------------------------------------------
               ** In Automatic mode selection, return back an empty list
               ** without aborting a RR(C) connection, REG would retry later
               ** ---------------------------------------------------------*/
               
#ifndef FEATURE_RAT_PRIORITY_LIST
#ifndef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
               if (is_umts_mode() && ((rrc_connection_status != MMRRC_IDLE) ||
#ifdef FEATURE_DUAL_SIM
                  (sm_is_pdp_active(mm_sub_id))
#else
                  (sm_is_pdp_active())
#endif
                  ))
#else
 #ifndef FEATURE_WTOG_BACKGROUND_PLMN_SEARCH
               if (is_umts_mode() && (rrc_connection_status != MMRRC_IDLE) 
  #ifdef FEATURE_WTOW_BACKGROUND_PLMN_SEARCH
                && ((msg_p->cmd.mmr_plmn_search_req.dual_mode) ||
                   (cs_session_status != MMRRC_IDLE))
  #endif
                   )
 #endif
#endif
#ifndef FEATURE_WTOG_BACKGROUND_PLMN_SEARCH
               {
                  MSG_HIGH_DS_0(MM_SUB, "=MM= AUTO PLMN_LIST_REQ in connected mode, return");
#if defined (FEATURE_WTOG_BACKGROUND_PLMN_SEARCH) || defined (FEATURE_WTOW_BACKGROUND_PLMN_SEARCH)
                  mmr_plmn_search_cnf->network_selection_mode = SYS_NETWORK_SELECTION_MODE_AUTOMATIC;
#endif
                  mmr_plmn_search_cnf->plmn_search_result = REG_MM_PLMN_SEARCH_ABORTED;
                  mmr_plmn_search_cnf->available_plmn_list.length = 0;
                  mmr_plmn_search_cnf->transaction_id = msg_p->cmd.mmr_plmn_search_req.transaction_id;
#ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
                  mmr_plmn_search_cnf->service_search = msg_p->cmd.mmr_plmn_search_req.service_search;
#endif
                  mm_send_mmr_plmn_search_cnf( mmr_plmn_search_cnf );
               }
               else
#endif
#endif /* FEATURE_RAT_PRIORITY_LIST */
               {
                  MSG_HIGH_DS_0(MM_SUB, "=MM= PLMN LIST REQ : Automatic selection");
                  mm_request_list_from_rr(&msg_p->cmd.mmr_plmn_search_req);
               }
            }
         }
         else
         {
            MSG_ERROR_DS( MM_SUB, "=MM= Unexpected REG message id %d", msg_p->cmd.hdr.message_id, 0,0 );
         }
         break;

      case MS_MM_RR:
      case MS_MM_TDSRRC:
         /*
          * Processing to receive the LAI list from RR and from it, form an
          * available PLMN list for transferring to REG. This is presented
          * in signal strength order, with only a single entry for each PLMN.
          * It may or may not contain PLMNs for forbidden LAIs, depending on
          * the state of mm_present_all_plmns.
          */

         if (  mm_idle_substate == MM_PLMN_SEARCH_NORMAL_SERVICE )
         {

            /*
             * plmn search is done, so go back to normal service
             */

            mm_substate_control( MM_STD_NORMAL_SERVICE );
         }
         else if (mm_idle_substate == MM_PLMN_SEARCH)
         {
            if (!mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY))
            {
               mm_substate_control( MM_STD_NO_IMSI );
            }
            else if (mm_serving_plmn.mm_service_state == SERVICE_NOT_AVAILABLE)
            {
               mm_substate_control( MM_STD_NO_SERVICE );
            }
            else
            {
               mm_substate_control( MM_STD_LIMITED_SERVICE );
            }
         }

         mmr_plmn_search_cnf->found_plmn_list.plmn_list.length = 0;

         switch(msg_p->cmd.hdr.message_id)
         {

            case RR_PLMN_LIST_CNF:
               MSG_HIGH_DS_1(MM_SUB ,"=MM= MM received RR_PLMN_LIST_CNF TI (%d)", msg_p->cmd.rr_plmn_list_cnf.trans_id);
#ifdef FEATURE_DUAL_SIM
               if(msg_p->cmd.rr_plmn_list_cnf.status != MM_AS_SEARCH_PARTIAL)
               {
                 mm_send_rr_ps_resume_ind();
               }
#endif
               /* Set mm_plmn_search_in_progress as FALSE */
               if(mm_plmn_search_transaction_id == msg_p->cmd.rr_plmn_list_cnf.trans_id)
               {
                 mm_plmn_search_in_progress = FALSE;
               }
#ifdef FEATURE_SGLTE
               if(MM_IS_IN_SGLTE_STATE_4)
               {
               /*reseting pending context here after getting response from lower layer
                                No collsion case occur so no need this pended message now   */
                  mmcoord_pending_reg_list_req_message.cmd.hdr.message_set = 0;
               }
#endif

#ifdef FEATURE_LTE
               if(mm_ps_detach_state != MM_PS_DETACH_STATE_NONE)
               {
                 msg_p->cmd.rr_plmn_list_cnf.status = MM_AS_ABORTED;
                 MSG_HIGH_DS_1(MM_SUB ,"=MM= Received RR_PLMN_LIST_CNF in ps detach state %d set cnf status as aborted", 
                             mm_ps_detach_state);
               }
#endif
               
#if defined (FEATURE_WTOG_BACKGROUND_PLMN_SEARCH) || defined (FEATURE_WTOW_BACKGROUND_PLMN_SEARCH) \
    || defined (FEATURE_RAT_PRIORITY_LIST)
               mmr_plmn_search_cnf->network_selection_mode = msg_p->cmd.rr_plmn_list_cnf.net_selection_mode;
               /*
               ** Back to back LIST REQ (first one for AUTO and second is for MANUAL) case, 
               ** flag should not be reset when LIST_CNF for 1st LIST REQ - AUTO is received.
               */
               if (mm_waiting_for_list_cnf_type == msg_p->cmd.rr_plmn_list_cnf.net_selection_mode)
               {
                 mm_waiting_for_list_cnf_type = SYS_NETWORK_SELECTION_MODE_NONE;
               }
#else
               mm_waiting_for_list_cnf_type = SYS_NETWORK_SELECTION_MODE_NONE;
#endif
#ifdef FEATURE_DUAL_SIM
               if (msg_p->cmd.rr_plmn_list_cnf.failure_cause == MM_RR_NO_RESOURCE_AVAILABLE)
               {
                 mmr_plmn_search_cnf->plmn_search_result = REG_MM_PLMN_SEARCH_ABORTED;
                 mmr_plmn_search_cnf->found_plmn_list.plmn_list.length = 0;
               } 
               else
#endif
               if ( msg_p->cmd.rr_plmn_list_cnf.status == MM_AS_ABORTED
#ifdef FEATURE_SGLTE
                   || (msg_p->cmd.rr_plmn_list_cnf.status == MM_AS_INTERRAT_HANDOVER)
#endif
                   )
               {
                 mmr_plmn_search_cnf->plmn_search_result = REG_MM_PLMN_SEARCH_ABORTED;
                  if(mm_emm_is_qrd_device == FALSE)
                  {
                    msg_p->cmd.rr_plmn_list_cnf.available_plmn_list.length = 0;
                  }
               }
               else if( msg_p->cmd.rr_plmn_list_cnf.status == MM_AS_SEARCH_PARTIAL)                  
               {
                    mmr_plmn_search_cnf->plmn_search_result = REG_MM_PLMN_SEARCH_PARTIAL;
                    mm_plmn_search_in_progress = TRUE;
                    mm_waiting_for_list_cnf_type = SYS_NETWORK_SELECTION_MODE_MANUAL;
               }
               else
               {
                 mmr_plmn_search_cnf->plmn_search_result = REG_MM_PLMN_SEARCH_SUCCESS;
               }
               num_plmn = msg_p->cmd.rr_plmn_list_cnf.available_plmn_list.length;

                 /* ---------------------------------------------------------------------
                 ** Report the received PLMN List from RR back to REG, note: No filtering
                 ** for Forbidden PLMN is done here, because the RR layer would filter
                 ** out the PLMNS based on the forbidden LAI list passed to them.
                 ** -------------------------------------------------------------------*/
                 for (entry = 0; entry < num_plmn; entry++)
                 {
                    mmr_plmn_search_cnf->found_plmn_list.plmn_list.info[entry] =
                       msg_p->cmd.rr_plmn_list_cnf.available_plmn_list.info[entry];
                 }

               mmr_plmn_search_cnf->found_plmn_list.plmn_list.length = num_plmn;

               mmr_plmn_search_cnf->transaction_id = msg_p->cmd.rr_plmn_list_cnf.trans_id;
#if defined (FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) || defined (FEATURE_LTE)
               mmr_plmn_search_cnf->service_search = msg_p->cmd.rr_plmn_list_cnf.service_search;
#endif
#ifdef FEATURE_FEMTO_CSG
               if(mmr_plmn_search_cnf->service_search == SYS_SERVICE_SEARCH_CSG && 
                   mmr_plmn_search_cnf->plmn_search_result == REG_MM_PLMN_SEARCH_SUCCESS)
               {
                 mm_process_csg_search_list(mm_csg_search_type,
                                     &mmr_plmn_search_cnf->found_plmn_list.plmn_list);
               }
#endif 
               if((mmr_plmn_search_cnf->plmn_search_result != REG_MM_PLMN_SEARCH_PARTIAL) ||
                  ((mm_emm_is_qrd_device == TRUE) &&
                   (mmr_plmn_search_cnf->found_plmn_list.plmn_list.length != 0 ) &&
                   (mmr_plmn_search_cnf->plmn_search_result == REG_MM_PLMN_SEARCH_PARTIAL)))
               {
                 mm_send_mmr_plmn_search_cnf( mmr_plmn_search_cnf );
               }
               /*
               ** Send service_ind to REG immediately if not Partial Search result
               */
               if( msg_p->cmd.rr_plmn_list_cnf.status != MM_AS_SEARCH_PARTIAL)                        
               {
                 if(mm_reg_cause != MMR_CAUSE_NONE)
                 {
                   mmr_service_ind.cause = mm_reg_cause;
                 }
                 else
                 {
                   mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
                 }
                 mmr_service_ind.service_state = mm_serving_plmn.info;
                 mm_send_mmr_service_ind( &mmr_service_ind);
               }
               break;

            case RRC_PLMN_LIST_CNF:
               MSG_HIGH_DS_1(MM_SUB ,"=MM= Received RRC_PLMN_LIST_CNF TI (%d)",msg_p->cmd.rrc_plmn_list_cnf.trans_id);
#ifdef FEATURE_DUAL_SIM
               if(msg_p->cmd.rrc_plmn_list_cnf.status != MM_AS_SEARCH_PARTIAL)
               {
                 mm_send_rr_ps_resume_ind();
               }
#endif
               /* Set mm_plmn_search_in_progress as FALSE */
               if(mm_plmn_search_transaction_id == msg_p->cmd.rrc_plmn_list_cnf.trans_id)
               {
                 mm_plmn_search_in_progress = FALSE;
               }
#ifdef FEATURE_LTE
               if(mm_ps_detach_state != MM_PS_DETACH_STATE_NONE)
               {
                 msg_p->cmd.rrc_plmn_list_cnf.status = MM_AS_ABORTED;
                 MSG_HIGH_DS_1(MM_SUB ,"=MM= Received RRC_PLMN_LIST_CNF in ps detach state %d set cnf status as aborted", 
                             mm_ps_detach_state);
               }
#endif
#if defined (FEATURE_WTOG_BACKGROUND_PLMN_SEARCH) || defined (FEATURE_WTOW_BACKGROUND_PLMN_SEARCH) \
    || defined (FEATURE_RAT_PRIORITY_LIST)
               mmr_plmn_search_cnf->network_selection_mode = msg_p->cmd.rrc_plmn_list_cnf.network_select_mode;
               /*
               ** Back to back LIST REQ (first one for AUTO and second is for MANUAL) case, 
               ** flag should not be reset when LIST_CNF for 1st LIST REQ - AUTO is received.
               */
               if (mm_waiting_for_list_cnf_type == msg_p->cmd.rrc_plmn_list_cnf.network_select_mode)
               {
                 mm_waiting_for_list_cnf_type = SYS_NETWORK_SELECTION_MODE_NONE;
               }
#else
               mm_waiting_for_list_cnf_type = SYS_NETWORK_SELECTION_MODE_NONE;
#endif
               if (msg_p->cmd.rrc_plmn_list_cnf.status == MM_AS_ABORTED)
               {
                 mmr_plmn_search_cnf->plmn_search_result = REG_MM_PLMN_SEARCH_ABORTED;
                 mmr_plmn_search_cnf->found_plmn_list.plmn_list.length = 0;
                 if(mm_emm_is_qrd_device == FALSE)
                 {
                   msg_p->cmd.rrc_plmn_list_cnf.avail_plmn_list.num_plmns = 0;
                 }
               }
               else if(msg_p->cmd.rrc_plmn_list_cnf.status == MM_AS_SEARCH_PARTIAL)              
               {
                    mmr_plmn_search_cnf->plmn_search_result = REG_MM_PLMN_SEARCH_PARTIAL;
                    mm_plmn_search_in_progress = TRUE;
                    mm_waiting_for_list_cnf_type = SYS_NETWORK_SELECTION_MODE_MANUAL;
               }
               else
               {
                 mmr_plmn_search_cnf->plmn_search_result = REG_MM_PLMN_SEARCH_SUCCESS;
			   } 	 
               num_plmn = msg_p->cmd.rrc_plmn_list_cnf.avail_plmn_list.num_plmns;

                 /* ---------------------------------------------------------------------
                 ** Report the received PLMN List from RRC back to REG, note: No filtering
                 ** for Forbidden PLMN is done here, because the RRC layer would filter
                 ** out the PLMNS based on the forbidden LAI list passed to them.
                 ** -------------------------------------------------------------------*/
               for (entry = 0; entry < num_plmn; entry++)
               {
                    mmr_plmn_search_cnf->found_plmn_list.plmn_list.info[entry].list_category =
                       SYS_DETAILED_PLMN_LIST_CATEGORY_OTHER;

                    mmr_plmn_search_cnf->found_plmn_list.plmn_list.info[entry].plmn =
                       mmumts_convert_rrc_plmn_id_to_nas_plmn_id(
                          msg_p->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[entry].plmn);

                    mmr_plmn_search_cnf->found_plmn_list.plmn_list.info[entry].plmn_forbidden = FALSE;

                    mmr_plmn_search_cnf->found_plmn_list.plmn_list.info[entry].plmn_service_capability =
                       msg_p->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[entry].service_capability;

                    mmr_plmn_search_cnf->found_plmn_list.plmn_list.info[entry].rat =
                       msg_p->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[entry].rat;

                    mmr_plmn_search_cnf->found_plmn_list.plmn_list.info[entry].signal_quality =
                       msg_p->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[entry].signal_quality;

                    mmr_plmn_search_cnf->found_plmn_list.plmn_list.info[entry].signal_strength =
                       msg_p->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[entry].signal_strength;
                    mmr_plmn_search_cnf->found_plmn_list.plmn_list.info[entry].earfcn =                              
					   msg_p->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[entry].earfcn;
#ifdef FEATURE_FEMTO_CSG
                   if(msg_p->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[entry].rat == SYS_RAT_TDS_RADIO_ACCESS)
                   {
                     mmr_plmn_search_cnf->found_plmn_list.plmn_list.info[entry].csg_info.csg_id = SYS_CSG_ID_INVALID;
                   }
                   else
                   {
                     mmr_plmn_search_cnf->found_plmn_list.plmn_list.info[entry].csg_info = 
                       msg_p->cmd.rrc_plmn_list_cnf.avail_plmn_list.plmn[entry].csg_info;
                   }
#endif  
               }

               mmr_plmn_search_cnf->found_plmn_list.plmn_list.length = num_plmn;


               mmr_plmn_search_cnf->transaction_id = msg_p->cmd.rrc_plmn_list_cnf.trans_id;
#if defined (FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) || defined (FEATURE_LTE)
               mmr_plmn_search_cnf->service_search = msg_p->cmd.rrc_plmn_list_cnf.service_search;
#endif

#ifdef FEATURE_FEMTO_CSG
               if(mmr_plmn_search_cnf->service_search == SYS_SERVICE_SEARCH_CSG && 
                   mmr_plmn_search_cnf->plmn_search_result == REG_MM_PLMN_SEARCH_SUCCESS)
               {
                 mm_process_csg_search_list(mm_csg_search_type,
                                     &mmr_plmn_search_cnf->found_plmn_list.plmn_list);
               }
#endif 
               if((mmr_plmn_search_cnf->plmn_search_result != REG_MM_PLMN_SEARCH_PARTIAL) ||
                  ((mm_emm_is_qrd_device == TRUE) &&
                   (mmr_plmn_search_cnf->found_plmn_list.plmn_list.length != 0 ) &&
                   (mmr_plmn_search_cnf->plmn_search_result == REG_MM_PLMN_SEARCH_PARTIAL)))
               {
                 mm_send_mmr_plmn_search_cnf( mmr_plmn_search_cnf );
               }
               /*
               ** Send service_ind to REG immediately  if not Partial Search complete
               */
               if( msg_p->cmd.rrc_plmn_list_cnf.status != MM_AS_SEARCH_PARTIAL)
               {
                 if(mm_reg_cause != MMR_CAUSE_NONE)
                 {
                   mmr_service_ind.cause = mm_reg_cause;
                 }
                 else
                 {
                   mmr_service_ind.cause         = MMR_CAUSE_NOT_SPECIFIED;
                 }
               
                 mmr_service_ind.service_state = mm_serving_plmn.info;
                 mm_send_mmr_service_ind( &mmr_service_ind);
			   } 
               break;

            default:
                MSG_ERROR_DS( MM_SUB, "=MM= Unexpected RR(C) message id %d", msg_p->cmd.hdr.message_id, 0,0);
                break;
         }
         break;

      default:
         MSG_ERROR_DS( MM_SUB, "=MM= Unexpected message set %d",  msg_p->cmd.hdr.message_set, 0,0 );
         break;
   }
#ifndef FEATURE_MODEM_HEAP
   gs_free(mmr_plmn_search_cnf);
#else
   modem_mem_free(mmr_plmn_search_cnf, MODEM_MEM_CLIENT_NAS);
#endif

}


/*===========================================================================

FUNCTION MM_REQUEST_LIST_FROM_RR

DESCRIPTION
  This function sends the request for the LAI list to RR. The
  camped on flag is determined from the current MM-IDLE sub-
  state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_request_list_from_rr(mmr_plmn_search_req_s_type *plmn_srch_ptr)
{
   /*
   * When in UMTS mode, use RRC_PLMN_REQ to get the list of LAIs from RRC
   */
   if ((plmn_srch_ptr->rat == SYS_RAT_UMTS_RADIO_ACCESS) ||
       (plmn_srch_ptr->rat == SYS_RAT_TDS_RADIO_ACCESS))
   {
      rrc_cmd_type *rrc_cmd_p = NULL;
#ifdef FEATURE_WCDMA
      rrc_cmd_p = mm_get_rrc_cmd_buf( plmn_srch_ptr->rat );
#endif
      if (rrc_cmd_p == NULL)
      {
         MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
      }

#ifndef FEATURE_RAT_PRIORITY_LIST
#ifndef FEATURE_WTOG_BACKGROUND_PLMN_SEARCH
 #ifndef FEATURE_WTOW_BACKGROUND_PLMN_SEARCH
      mm_serving_plmn.mm_service_state  = SERVICE_NOT_AVAILABLE;
      mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;

      mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                      mm_serving_plmn.info.reg_service_domain,
                                      TRUE,
                                      SYS_SRV_STATUS_NO_SRV,
                                      TRUE );
 #else
      if (plmn_srch_ptr->dual_mode == TRUE)
      {
        mm_serving_plmn.mm_service_state  = SERVICE_NOT_AVAILABLE;
        mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;

        mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                        mm_serving_plmn.info.reg_service_domain,
                                        TRUE,
                                        SYS_SRV_STATUS_NO_SRV,
                                        TRUE ); 
      }
 #endif

      if ((rrc_connection_status != MMRRC_IDLE) 
 #ifdef FEATURE_WTOW_BACKGROUND_PLMN_SEARCH
          &&
          (plmn_srch_ptr->dual_mode)
 #endif
         )
      {
        /* If PS session is active or RRC connection active, Abort it */
        mm_plmn_list_req_pending = TRUE;
        mmcoord_pending_reg_list_req_message.cmd.hdr = plmn_srch_ptr->message_header;
        mmcoord_pending_reg_list_req_message.cmd.mmr_plmn_search_req = *plmn_srch_ptr;
        mmcoord_reg_req_pending = FALSE;

        /* Send an Abort Request to RRC */
        rrc_cmd_p->cmd_hdr.cmd_id = RRC_ABORT_REQ;
        rrc_cmd_p->cmd.rrc_abort_req.delay_sig_con_release = TRUE;
        rrc_cmd_p->cmd.rrc_abort_req.auth_failed_bar_cells = FALSE;
        rrc_cmd_p->cmd.rrc_abort_req.hard_abort = FALSE;
        rrc_cmd_p->cmd.rrc_abort_req.abort_cause = RRC_OTHERS;
      }
      else
#endif
#endif /* FEATURE_RAT_PRIORITY_LIST */
      {
        /* Send the PLMN LIST REQUEST Request to RRC */
        rrc_cmd_p->cmd_hdr.cmd_id = RRC_PLMN_LIST_REQ;
#ifndef FEATURE_RAT_PRIORITY_LIST
#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
        rrc_cmd_p->cmd.plmn_list_req.band_pref = get_preferred_band_class();
#endif
#else
        /* The following two lines have to be removed once code changes for
          * rat priority list in RRC layer is completed */
        rrc_cmd_p->cmd.plmn_list_req.dual_mode = plmn_srch_ptr->dual_mode;
#ifdef FEATURE_GSM_BAND_PREF
        rrc_cmd_p->cmd.plmn_list_req.band_pref = plmn_srch_ptr->band_pref;
#endif /*FEATURE_GSM_BAND_PREF*/
        rrc_cmd_p->cmd.plmn_list_req.rat_pri_list_info = plmn_srch_ptr->rat_pri_list_info;
#endif /* FEATURE_RAT_PRIORITY_LIST */  
#if defined (FEATURE_WTOG_BACKGROUND_PLMN_SEARCH) || defined (FEATURE_WTOW_BACKGROUND_PLMN_SEARCH) \
    || defined (FEATURE_RAT_PRIORITY_LIST)  
        rrc_cmd_p->cmd.plmn_list_req.network_select_mode = plmn_srch_ptr->network_selection_mode;
        rrc_cmd_p->cmd.plmn_list_req.plmn_list = plmn_srch_ptr->higher_priority_plmn_list;
#endif

#ifndef FEATURE_RAT_PRIORITY_LIST
#ifdef FEATURE_WTOG_BACKGROUND_PLMN_SEARCH
        rrc_cmd_p->cmd.plmn_list_req.dual_mode = plmn_srch_ptr->dual_mode;
#else
        rrc_cmd_p->cmd.plmn_list_req.dual_mode = FALSE;
#endif
#endif /* FEATURE_RAT_PRIORITY_LIST */

        rrc_cmd_p->cmd.plmn_list_req.trans_id = plmn_srch_ptr->transaction_id;
        mm_plmn_search_transaction_id = plmn_srch_ptr->transaction_id;
#if defined (FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) || defined (FEATURE_LTE)
        rrc_cmd_p->cmd.plmn_list_req.service_search = plmn_srch_ptr->service_search;
#endif
        if((rrc_cmd_p->cmd.plmn_list_req.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL) &&
           (rrc_cmd_p->cmd.plmn_list_req.service_search == SYS_SERVICE_SEARCH_NONE))
        {
            MSG_HIGH_DS_3(MM_SUB,"=MM= Manual PLMN Timer value FIRST %u  MORE %u  MAX %u",
                         mm_emm_manual_search_timer_value.FirstPLMNTimer,
                         mm_emm_manual_search_timer_value.MorePLMNTimer,
                         mm_emm_manual_search_timer_value.MaxManualPLMNTimer);
            rrc_cmd_p->cmd.plmn_list_req.search_timer_value = mm_emm_manual_search_timer_value;   
            MSG_HIGH_DS_1( MM_SUB, "=MM= BST changes length of mcc mode filter =%d",
                                       plmn_srch_ptr->mcc_mode_band_filter.num_items);
#ifdef FEATURE_RAT_PRIORITY_LIST
            rrc_cmd_p->cmd.plmn_list_req.mcc_mode_band_filter = plmn_srch_ptr->mcc_mode_band_filter;  
#endif
        }

        rrc_cmd_p->cmd.plmn_list_req.scan_scope = plmn_srch_ptr->scan_scope; 

        mm_plmn_list_req_pending = FALSE;

        mm_plmn_search_in_progress = TRUE;

        MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent RRC_PLMN_LIST_REQ");
      }

      mm_waiting_for_list_cnf_type = plmn_srch_ptr->network_selection_mode;

      /* Manual search is allowed now even in ALT scan state - FR 3427
         MMODE and NAS become active when Manual search started. 
         ALT scan will happen again after Manual search before scanning CDMA
         */
      mm_deep_sleep_alt_scan = FALSE;

      if (plmn_srch_ptr->rat == SYS_RAT_UMTS_RADIO_ACCESS)
      {
        mm_serving_plmn.info.active_rat = SYS_RAT_UMTS_RADIO_ACCESS;
      }
      else if (plmn_srch_ptr->rat == SYS_RAT_TDS_RADIO_ACCESS)
      {
        mm_serving_plmn.info.active_rat = SYS_RAT_TDS_RADIO_ACCESS;
      }

      send_cmd_to_rrc( rrc_cmd_p, plmn_srch_ptr->rat );
   }
   else if( plmn_srch_ptr->rat == SYS_RAT_GSM_RADIO_ACCESS)
   {
      rr_plmn_list_req_T*       rr_plmn_list_req;
#ifndef FEATURE_MODEM_HEAP
      rr_plmn_list_req = (rr_plmn_list_req_T*)gs_alloc(sizeof(rr_plmn_list_req_T));
#else
      rr_plmn_list_req = (rr_plmn_list_req_T*)modem_mem_calloc(1, sizeof(rr_plmn_list_req_T), MODEM_MEM_CLIENT_NAS);
#endif
      mm_check_for_null_ptr((void*)rr_plmn_list_req);      

      memset(rr_plmn_list_req,0x00,sizeof(rr_plmn_list_req_T));

      rr_plmn_list_req->message_header.message_set = MS_MM_RR;
      rr_plmn_list_req->message_header.message_id  = (int)RR_PLMN_LIST_REQ;

#ifndef FEATURE_RAT_PRIORITY_LIST
#if defined (FEATURE_GSM_BAND_PREF) || defined (FEATURE_WCDMA_MULTIBAND)
      rr_plmn_list_req->band_pref = get_preferred_band_class();
#endif
#else
      /* The following two lines have to be removed once code changes for
        * rat priority list in RR layer is completed */
      rr_plmn_list_req->band_pref = plmn_srch_ptr->band_pref;
      rr_plmn_list_req->gsm_only = !plmn_srch_ptr->dual_mode;

      rr_plmn_list_req->rat_pri_list_info = plmn_srch_ptr->rat_pri_list_info;
#endif /* FEATURE_RAT_PRIORITY_LIST */  
      rr_plmn_list_req->network_selection_mode =
          plmn_srch_ptr->network_selection_mode;
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH) || defined(FEATURE_RAT_PRIORITY_LIST)
#ifndef FEATURE_RAT_PRIORITY_LIST
      rr_plmn_list_req->gsm_only = !plmn_srch_ptr->dual_mode;
#endif /* FEATURE_RAT_PRIORITY_LIST */
      rr_plmn_list_req->plmn_list = plmn_srch_ptr->higher_priority_plmn_list;
#endif
      rr_plmn_list_req->trans_id = plmn_srch_ptr->transaction_id;
      mm_plmn_search_transaction_id = plmn_srch_ptr->transaction_id;
#if defined (FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH) || defined (FEATURE_LTE)
      rr_plmn_list_req->service_search = plmn_srch_ptr->service_search;
#endif
      if((rr_plmn_list_req->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL) &&              
         (rr_plmn_list_req->service_search == SYS_SERVICE_SEARCH_NONE))
      {

         MSG_HIGH_DS_3(MM_SUB,"Manual PLMN Timer  value FIRST %d	MORE %d  MAX %d",
                       mm_emm_manual_search_timer_value.FirstPLMNTimer,
                       mm_emm_manual_search_timer_value.MorePLMNTimer,
                       mm_emm_manual_search_timer_value.MaxManualPLMNTimer);
         rr_plmn_list_req->search_timer_value = mm_emm_manual_search_timer_value;
         MSG_HIGH_DS_1( MM_SUB, "=MM= BST changes length of mcc mode filter =%d",
                                      plmn_srch_ptr->mcc_mode_band_filter.num_items);
#ifdef FEATURE_RAT_PRIORITY_LIST
         rr_plmn_list_req->mcc_mode_band_filter = plmn_srch_ptr->mcc_mode_band_filter;   
#endif

      }
      rr_plmn_list_req->scan_scope = plmn_srch_ptr->scan_scope;                      

      mm_plmn_search_in_progress = TRUE;
      mm_serving_plmn.info.active_rat = SYS_RAT_GSM_RADIO_ACCESS;
      mm_plmn_list_req_pending = FALSE;

      mm_waiting_for_list_cnf_type = plmn_srch_ptr->network_selection_mode;
#ifdef FEATURE_SGLTE
      if (MM_IS_IN_SGLTE_STATE_4)
      {
         /*saving search context so that we can send abort search in thec case upper layer split
               use this variable to save context other wise need two global for trasns id and serch type  */
         mmcoord_pending_reg_list_req_message.cmd.hdr = plmn_srch_ptr->message_header;
         mmcoord_pending_reg_list_req_message.cmd.mmr_plmn_search_req = *plmn_srch_ptr;     
      }
    
#endif
      
      /* Manual search is allowed now even in ALT scan state - FR 3427
         MMODE and NAS become active when Manual search started. 
         ALT scan will happen again after Manual search before scanning CDMA
         */
      mm_deep_sleep_alt_scan = FALSE;

      
      PUT_IMH_LEN( sizeof(rr_plmn_list_req_T) - sizeof(IMH_T), &rr_plmn_list_req->message_header );

      MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent RR_PLMN_LIST_REQ TI (%d)", rr_plmn_list_req->trans_id);

      mm_send_message( (IMH_T *)rr_plmn_list_req, GS_QUEUE_RR );
#ifndef FEATURE_MODEM_HEAP
      gs_free(rr_plmn_list_req);
#else
      modem_mem_free(rr_plmn_list_req, MODEM_MEM_CLIENT_NAS);
#endif


   }
   else
   {
      MSG_FATAL_DS( MM_SUB, "=MM= Unknown mode (UMTS or GSM)", 0,0,0 );
   }
}
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/
