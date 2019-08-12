/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmdeact.c_v   1.12   01 Jul 2002 11:27:04   jault  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmdeact.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/09/11  abhi    Sending cause RR_MM_LPM_POWER_OFF to GRR on receiving ph_status_change_req
07/04/11   abhi   After deactivating WRRC/GRR due to ph_status_chg_req, 
                             send ph_status_chg_req to EMM.
05/14/01   jca     Added mm_ prefix to MM/GMM shared variables.
06/03/01   sbk     Global variables are renamed to use mm_ prefix
06/08/01   jca     Major restructuring of MMR_NREG_REQ processing.
08/03/01   jca     Added parameter to mm_send_mmr_nreg_cnf().
08/07/01   ts      Standardized MSG_ERROR and MSG_HIGH usage.
11/16/01   jca     Fixed bug where IMSI Detach was initiated during PS operation.
12/06/01   jca     Now sending Dynamic ID log packet when TMSI reset.
06/05/02   ks      Added support for Dual mode operation
07/01/02   jca     Added processing of RR_STOP_GSM_MODE_CNF.
08/09/02   jca     Stop T3212 timer when MM deactivated (as per spec).
01/07/03   mks     Now stopping the active RAT (that was activated through
                   early power scan) if MMR_STOP_MODE_REQ is received in
                   NULL state.
02/07/03   ks      Added support for SYS_STOP_MODE_REASON_MODE_CHANGE reason.
03/18/03   ks      removed the unnecessary check for MM_WAIT_FOR_RR_ACTIVE in
                   mm_handle_deactivation(). Added deactivation reason in the
                   the function mm_deactivate_rr().
03/19/03   ks      Setting mm_service_state to SERVICE_NOT_AVAILABLE after
                   deactivating RR.
04/10/03   ks      Setting the deact_reason in the RRC_DEACT_REQ primitive.
06/04/03   ks      Added support for RRC_MODE_CHANGE_REQ/CNF for MODE_CHANGE
                   support for UMTS.
06/06/03   jca     Added code to ensure all MM/GMM timers stopped when MM deactivated.
06/16/03   jca     Now consistently ERR_FATAL if attempt to allocate memory fails.
10/02/03   jca     Corrected bug when setting value of rr_deact_req.deact_reason.
04/06/04   cd      LCS support
06/14/04   ks      Now sending IMSI detach to the Network only if the UE is
                   registered in CS domain.
07/07/04   cd      Fixed Lint errors
08/12/04   cd      Replaced FEATURE_LCS by FEATURE_MMGPS
11/12/04   jca     If MM is in state 9 when a MMR_STOP_MODE_REQ is received, then
                   immediately abort the RR connection is it is not necessary to
                   send an IMSI DETACH INDICATION.
04/20/05   jca     Now block Detach procedure due to access class restrictions.
10/07/05   kvk     Setting mm_waiting_for_deact_cnf flag to FALSE after MM receives 
                   RRC_MODE_CHANGE_CNF from RRC
06/09/06   sn      Added SYS_STOP_MODE_REASON_LPM
07/14/06   pk      Updated for Feature DSAC
10/13/06   up      Reset mm_waiting_for_stop_mode_cnf to FALSE and active_rat to NONE
                   when DEACT_CNF received from RR/RRC.
10/05/11   gps     Initiated rr deact req primitive with all zeros
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
#include "sys_plmn_selection_v.h"
#include "mm_sim.h"
#include "gmm_int_v.h"
#ifdef FEATURE_NAS_REL11
#include "mmauth.h"
#endif

#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE))
 #error code not present
#endif // TEST_FRAMEWORK

/*===========================================================================

FUNCTION MM_HANDLE_DEACTIVATION

DESCRIPTION
  This function deals with the deactivation messages. These are
  MMR_NREG_REG, and RR_DEACT_CNF
  In the case of a request to deactivated from REG, this function
  initiates the deactivation of RR following notification
  from MM that power off has been initiated, or handles the case
  where the SIM is removed. If there are any MM connections active at
  the time of power down or SIM removal, then these are released by CM
  prior to issuing the NREG request. IF there are any MM procedures
  active at the time, then the detach request is flagged, so that it can
  be checked when the 'idle' or 'wait for network command' state is
  entered.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_handle_deactivation(mm_cmd_type* msg_p)
{

#ifdef FEATURE_LTE  
   emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

   emm_ctrl_data_ptr = emm_db_get_ctrl_data();

   ASSERT(emm_ctrl_data_ptr != NULL);   
#endif 

   switch ( msg_p->cmd.hdr.message_set )
   {
      case MS_MM_REG:
         if (msg_p->cmd.hdr.message_id == (int)MMR_STOP_MODE_REQ)
         {
            /* ------------------------------------
            ** An IMSI attach may be required later
            ** ------------------------------------ */
            mm_possible_imsi_attach = TRUE;
            /* --------------------------------------------------------------------
            ** The T3212 timer is stopped when the mobile station is deactivated
            ** (i.e., equipment powered down or SIM removed) (3GPP TS 24.008 4.2.2)
            ** -------------------------------------------------------------------- */
            mm_stop_timer( TIMER_T3212 );

            if (mm_state == MM_NULL)
            {
               if (mm_serving_plmn.info.active_rat == SYS_RAT_NONE)
               {
                  mm_send_mmr_stop_mode_cnf();
               }
               else if (!mm_waiting_for_deact_cnf)
               {
                  /* ----------------------------------------------
                  ** Remember that a MMR_NREG_REQ has been received.
                  ** ---------------------------------------------- */
                  mm_nreg_req_received = TRUE;                      

                  memscpy( &mmcoord_pending_reg_message.cmd.mmr_stop_mode_req, sizeof(mmr_stop_mode_req_s_type),
                          &msg_p->cmd.mmr_stop_mode_req, sizeof(msg_p->cmd.mmr_stop_mode_req) );
                  /* -------------------------------
                  ** Deactivate the Active RAT now
                  ** ------------------------------- */
                  mm_stop_rat_mode();
               }
            }  
#if defined(FEATURE_LTE)                    
            /*Case scenario when UE got SYS_STOP_MODE_REASON_IRAT, doing local deactivation*/
            else if((msg_p->cmd.mmr_stop_mode_req.stop_mode_reason==SYS_STOP_MODE_REASON_IRAT)&& 
                    (mm_service_loss_on_multimode_rat == SYS_RAT_LTE_RADIO_ACCESS)&& 
                    (gmm_state==GMM_REGISTERED)) 
            {
               mm_nreg_req_received = FALSE; /* we don't need this flag anymore as we are going for deact*/

               memscpy( &mmcoord_pending_reg_message.cmd.mmr_stop_mode_req, sizeof(mmr_stop_mode_req_s_type),
                          &msg_p->cmd.mmr_stop_mode_req, sizeof(msg_p->cmd.mmr_stop_mode_req) );

               mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                        SYS_SRV_DOMAIN_NO_SRV,
                                                        FALSE,
                                                        SYS_SRV_STATUS_LIMITED,
                                                        TRUE );
               mm_state_control( MM_STD_MS_DEACTIVATED_NO_ATTACH );
            }  
#endif                   
            else
            {
               mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_NONE);
               
               mm_nreg_req_received = TRUE;                      
               memscpy( &mmcoord_pending_reg_message.cmd.mmr_stop_mode_req,sizeof(mmr_stop_mode_req_s_type),
                       &msg_p->cmd.mmr_stop_mode_req, sizeof(msg_p->cmd.mmr_stop_mode_req) );
               
               if (mm_state == MM_IDLE )
               {
                  if ((mm_system_information.ATT) &&
                      ((mm_idle_substate == MM_NULL_SUBSTATE) ||
                       (mm_idle_substate == MM_LOCATION_UPDATE_NEEDED) ||
                       (mm_idle_substate == MM_PLMN_SEARCH_NORMAL_SERVICE) ||
                       (mm_idle_substate == MM_NORMAL_SERVICE)) &&
                      ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
                       (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS)) &&
                      (mm_check_if_cs_cell_access_allowed(cs_cell_access) == TRUE) &&
                      (mm_serving_plmn.mm_service_state == SERVICE_AVAILABLE)
#ifdef FEATURE_LTE
                      &&((msg_p->cmd.hdr.message_id == (int)MMR_STOP_MODE_REQ) &&
                         (msg_p->cmd.mmr_stop_mode_req.stop_mode_reason != 
                                                                  SYS_STOP_MODE_REASON_LOCAL_DETACH))
                      && ((emm_ctrl_data_ptr->l2gcco_state != EMM_LTOG_CCO_ABORTED)

#ifdef FEATURE_SGLTE
                          ||  (MM_IS_IN_SGLTE_MODE && MM_IS_SGLTE_ON_CS_STACK_ID)
#endif
                          )
#endif
                     )
                  { 
                  
                     mm_state_control( MM_STD_MS_DEACTIVATED_ATTACH_ALLOWED );
                    
                  }                   
                  else if ((gmm_state == GMM_NULL) || (gmm_state == GMM_DEREGISTERED)) 
                  {                     
                     if (rrc_connection_status != MMRRC_IDLE)
                     {
                        if(rrc_connection_status != MMRRC_RELEASE_IN_PROGRESS)
                        {
                           mm_abort_rr(RRC_PS_DOMAIN_CN_ID, TRUE);
                        }
                     }
                     /* -------------------------------------------------------
                     ** GMM is now in a state where MM can take further actions
                     ** ------------------------------------------------------- */
                     else
                     {
                         /* ----------------------------------------
                         ** Send a deactivation command to RR or RRC
                         ** (only deactivate on power down)
                         ** ---------------------------------------- */
                        mm_nreg_req_received = FALSE; /* we don't need this flag anymore as we are going for deact*/
                        mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                        SYS_SRV_DOMAIN_NO_SRV,
                                                        FALSE,
                                                        SYS_SRV_STATUS_LIMITED,
                                                        TRUE );
                        mm_state_control( MM_STD_MS_DEACTIVATED_NO_ATTACH );
                     }
                  }
                  else
                  {
                      /* nothing else to do*/
                  }
               }
               else if ((mm_state == MM_WAIT_FOR_NETWORK_COMMAND) ||
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
                        (mm_state == MM_RR_CONNECTION_RELEASE_NOT_ALLOWED) ||
#endif 
#ifdef FEATURE_CCBS
                        (mm_state == MM_PROCESS_CM_SERVICE_PROMPT) ||
#endif  
                        (mm_state == MM_CONNECTION_ACTIVE))
               {                  
                  if ((mm_system_information.ATT) &&
                      ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
                          (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS)) &&
                      (mm_check_if_cs_cell_access_allowed(cs_cell_access) == TRUE))
                  {
                     /* -----------------------------------------------------
                     ** Send an IMSI DETACH INDICATION message to the network
                     ** ----------------------------------------------------- */
                     if (mm_state == MM_CONNECTION_ACTIVE)
                     {
                        mm_send_mmcnm_rel_ind( OTHER_CAUSE, FALSE );
                     }
                     mm_state_control( MM_STD_MS_DEACTIVATED_ATTACH_ALLOWED );
                  }
                  else if ((gmm_state == GMM_NULL) || (gmm_state == GMM_DEREGISTERED)) 
                  {                     
                     /* ----------------------------------------
                     ** Abort the RR connection and transition to idle.
                     ** After the abort is confirmed, send a
                     ** deactivation request
                     ** ---------------------------------------- */
                     if (mm_timer_status[ TIMER_T3240 ] != TIMER_STOPPED)
                     {
                        mm_stop_timer( TIMER_T3240 );
                     }
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
                     if (mm_timer_status[ TIMER_T3241 ] != TIMER_STOPPED)
                     {
                        mm_stop_timer( TIMER_T3241 );
                     }
#endif
                     if ((dword) gs_enquire_timer (
                                                   get_mm_timer_queue(),
                                                   TIMER_MM_REL
                                                  ) > 10000)
                     {
                       MSG_HIGH_DS_0(MM_SUB, "=MM= No PS detach needed-starting TIMER_MM_REL with 10 secs,as 60 secs is too long");
                       mm_start_timer( TIMER_MM_REL, 10000 );  
                     }
                  }
                  else
                  {
                     /*nothing to do*/
                  }
               }            
               else
               {
                  /*no other action required in other states*/
               }
            }
         }
         else if (msg_p->cmd.hdr.message_id == (int)MMR_PH_STATUS_CHANGE_REQ)
         {
        #ifdef FEATURE_FEMTO_CSG
            if(mm_using_allowed_csg_nv == TRUE)
            {
              mm_sim_write_allowed_csg_to_nv(&mm_sim_active_allowed_csg_list);
              mm_using_allowed_csg_nv = FALSE;
            }
        #endif
            mm_waiting_for_ph_status_change_cnf = TRUE;
            
#ifdef FEATURE_LTE
            memscpy( &mmcoord_pending_reg_message.cmd.mmr_ph_status_change_req,sizeof(mmr_ph_status_change_req_s_type),
                          &msg_p->cmd.mmr_ph_status_change_req, sizeof(msg_p->cmd.mmr_ph_status_change_req) );
#endif
            if(!mm_waiting_for_deact_cnf)
            {
               if (mm_last_active_rat_stopped == SYS_RAT_GSM_RADIO_ACCESS)
               {
                  /*
                  ** Form RR_DEACT_REQ message
                  */
                  rr_deact_req_T rr_deact_req;
                  memset(&rr_deact_req,0x00,sizeof(rr_deact_req_T));
            
                  rr_deact_req.message_header.message_set = MS_MM_RR;
                  rr_deact_req.message_header.message_id  = (int)RR_DEACT_REQ;
            
                  if(msg_p->cmd.mmr_ph_status_change_req.stop_mode_reason == SYS_STOP_MODE_REASON_LPM_POWER_OFF)
                  {
                    rr_deact_req.deact_reason = RR_MM_LPM_POWER_OFF;
                  }
                  else
                  {
                    rr_deact_req.deact_reason = RR_MM_POWER_OFF;
                  }
                  PUT_IMH_LEN( sizeof( rr_deact_req_T ) - sizeof( IMH_T ),
                               &rr_deact_req.message_header );
            
                  MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent RR_DEACT_REQ; cause = %d", rr_deact_req.deact_reason);
                  mm_send_message( (IMH_T *)&rr_deact_req, GS_QUEUE_RR );
               }
#ifdef FEATURE_WCDMA             
               else if (mm_last_active_rat_stopped == SYS_RAT_UMTS_RADIO_ACCESS ||
                        mm_last_active_rat_stopped == SYS_RAT_TDS_RADIO_ACCESS
#ifdef FEATURE_LTE
                           || mm_last_active_rat_stopped == SYS_RAT_LTE_RADIO_ACCESS
#endif
                          )
               {
                  rrc_cmd_type *rrc_cmd = NULL;

                  rrc_cmd = mm_get_rrc_cmd_buf( mm_last_active_rat_stopped );
            
                  if (rrc_cmd == NULL)
                  {
                    MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
                  }
            
                  /* Set the Stop mode reason in the RRC_DEACT_REQ */
                  rrc_cmd->cmd.deact_req.deact_reason = msg_p->cmd.mmr_ph_status_change_req.stop_mode_reason;
                  rrc_cmd->cmd_hdr.cmd_id = RRC_DEACT_REQ;

#ifdef FEATURE_LTE
                  if (mm_last_active_rat_stopped == SYS_RAT_LTE_RADIO_ACCESS)
                  {
                     mm_state_control(MM_STD_LOCALLY_DEACTIVATE_MM);
                  }
#endif
            
                  MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent RRC_DEACT_REQ; cause = %d", rrc_cmd->cmd.deact_req.deact_reason);

                  send_cmd_to_rrc( rrc_cmd, mm_last_active_rat_stopped );
               }
#endif
               else
               {
                  mm_send_mmr_ph_status_change_cnf();
               }

            }
            else
            {
              MSG_ERROR_DS( MM_SUB, "=MM= MM received Phone status change event in wrong state; cause = %d", msg_p->cmd.mmr_ph_status_change_req.stop_mode_reason,0,0 );
            }
         }
         else
         {
            MSG_ERROR_DS( MM_SUB, "=MM= Unexpected REG message id %d", msg_p->cmd.hdr.message_id, 0,0 );
         }
         break;

      case MS_MM_RR:
      case MS_MM_TDSRRC:
         if ((msg_p->cmd.hdr.message_id == (int)RR_DEACT_CNF) ||
             (msg_p->cmd.hdr.message_id == (int)RR_STOP_GSM_MODE_CNF) ||
             (msg_p->cmd.hdr.message_id == (int)RRC_DEACT_CNF) ||
             (msg_p->cmd.hdr.message_id == (int)RRC_MODE_CHANGE_CNF))
         {
            if (msg_p->cmd.hdr.message_id == (int)RR_STOP_GSM_MODE_CNF)
            {
               MSG_HIGH_DS_0(MM_SUB, "=MM= MM received RR_STOP_GSM_MODE_CNF");
            }
            else if (msg_p->cmd.hdr.message_id == (int)RR_DEACT_CNF)
            {
               MSG_HIGH_DS_0(MM_SUB, "=MM= MM received RR_DEACT_CNF");
               if (!mm_waiting_for_ph_status_change_cnf)
               {
                  /* If this RR_DEACT_CNF is for first step power off (STOP_MODE_REQ),
                  ** remember the RAT to deactivate the same in second step of power off (PH_STATUS_CHNGD_REQ).
                  */
                  mm_last_active_rat_stopped = SYS_RAT_GSM_RADIO_ACCESS;
               }
#ifdef FEATURE_LTE
              if( !(mmcoord_pending_reg_message.cmd.mmr_stop_mode_req.stop_mode_reason== SYS_STOP_MODE_REASON_IRAT &&
                 (rrc_connection_status == MMRRC_IDLE) &&
                 (mm_service_loss_on_multimode_rat == SYS_RAT_LTE_RADIO_ACCESS) &&
                 (gmm_state==GMM_REGISTERED)))
#endif
              {
                mm_serving_plmn.info.active_rat = SYS_RAT_NONE;
              }
               mm_reset_global_variables();
               gmm_reset_global_variables();
            }
            else if (msg_p->cmd.hdr.message_id == (int)RRC_DEACT_CNF)
            {
               mm_serving_plmn.info.active_rat = SYS_RAT_NONE;
               mm_initiate_lu_upon_entry_into_new_cell = FALSE;
               mm_serving_cell_valid = FALSE;
               mm_reset_global_variables();
               gmm_reset_global_variables();
            }
            else if (msg_p->cmd.hdr.message_id == (int)RRC_MODE_CHANGE_CNF)
            {
               MSG_HIGH_DS_0(MM_SUB, "=MM= MM received RRC_MODE_CHANGE_CNF");
               if ( msg_p->cmd.hdr.message_set == MS_MM_TDSRRC
#ifdef FEATURE_SEGMENT_LOADING
&&get_tdscdma_interface()
#endif
)
               {
                 mm_last_active_rat_stopped = SYS_RAT_TDS_RADIO_ACCESS;
               }
               else if (TRUE
#ifdef FEATURE_SEGMENT_LOADING
&&get_wcdma_interface()
#endif
)               
               {
                 mm_last_active_rat_stopped = SYS_RAT_UMTS_RADIO_ACCESS;
               }
               mm_serving_plmn.info.active_rat = SYS_RAT_NONE;
               mm_reset_global_variables();
               gmm_reset_global_variables();
            }

            if (mm_waiting_for_ph_status_change_cnf)
            {
#ifdef FEATURE_LTE
              if(mm_is_multimode_sub())
              {
                mm_waiting_for_ph_status_change_cnf = FALSE; 
                emm_process_ph_status_change_req(&mmcoord_pending_reg_message, emm_ctrl_data_ptr);
              }
              else
#endif
              {
                mm_send_mmr_ph_status_change_cnf();
              }
            }
            else
            {
               mm_send_mmr_stop_mode_cnf();
            }

            mm_nreg_req_received = FALSE;
            mm_serving_plmn.mm_service_state  = SERVICE_NOT_AVAILABLE;
            mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;

            mm_substate_control( MM_STD_DEACTIVATE );
         }
         else
         {
            MSG_ERROR_DS( MM_SUB, "=MM= Unexpected RR message id %d", msg_p->cmd.hdr.message_id, 0,0 );
         }
         break;

      default:
         MSG_ERROR_DS( MM_SUB, "=MM= Unexpected message set %d", msg_p->cmd.hdr.message_set, 0,0 );
         break;
   }
}


/*===========================================================================

FUNCTION MM_DEACTIVATE_RR

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

void mm_deactivate_rr( void )
{
   byte i;
#ifdef FEATURE_NAS_REL10
   gmm_t3346_backoff_timer_value = 0;
   mm_t3246_backoff_timer_value  = 0;
#endif 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if ((mmcoord_pending_reg_message.cmd.mmr_stop_mode_req.stop_mode_reason ==
          SYS_STOP_MODE_REASON_MODE_CHANGE) ||
#ifdef FEATURE_DUAL_SIM
       (mmcoord_pending_reg_message.cmd.mmr_stop_mode_req.stop_mode_reason ==
          SYS_STOP_MODE_REASON_MODE_CAP_CHANGE) || 
#endif
       (mmcoord_pending_reg_message.cmd.mmr_stop_mode_req.stop_mode_reason ==
          SYS_STOP_MODE_REASON_IRAT) || 
       (mmcoord_pending_reg_message.cmd.mmr_stop_mode_req.stop_mode_reason ==
          SYS_STOP_MODE_REASON_POWER_OFF) ||
       (mmcoord_pending_reg_message.cmd.mmr_stop_mode_req.stop_mode_reason ==
          SYS_STOP_MODE_REASON_OFFLINE) ||
       (mmcoord_pending_reg_message.cmd.mmr_stop_mode_req.stop_mode_reason ==
          SYS_STOP_MODE_REASON_LPM)
#ifdef FEATURE_LTE
       ||(mmcoord_pending_reg_message.cmd.mmr_stop_mode_req.stop_mode_reason ==
          SYS_STOP_MODE_REASON_LOCAL_DETACH)
#endif
       )
   {
   
#ifdef FEATURE_NAS_REL10
     if (mmcoord_pending_reg_message.cmd.mmr_stop_mode_req.stop_mode_reason !=
          SYS_STOP_MODE_REASON_IRAT)
     {
       if(mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
       {
         /* Find out how much timeout is left */
       gmm_t3346_backoff_timer_value = (dword) gs_enquire_timer_ext ( 
                                                 get_mm_timer_queue(), 
                                                 TIMER_T3346,
                                                 TRUE
                                              );
        mm_stop_timer(TIMER_T3346);
      } 

      if(mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
      {   
        /* Find out how much timeout is left */
        mm_t3246_backoff_timer_value = (dword) gs_enquire_timer ( get_mm_timer_queue(), 
                                      #ifdef FEATURE_DUAL_SIM
                                               MM_TIMER_TAG_AS_ID(TIMER_T3246) 
                                      #else
                                               TIMER_T3246
                                      #endif
                                            ); 
         mm_stop_timer(TIMER_T3246);
       }
     }
#endif
   
      for (i=0; i < MAX_MM_TIMER+1; i++)
      {
         if ((mm_timer_status[i] != TIMER_STOPPED) &&
             (i != TIMER_T3346) && (i != TIMER_T3246) &&
             ((i != EMM_FORBIDDEN_TAI_CLEAR_TIMER) ||
              (mmcoord_pending_reg_message.cmd.mmr_stop_mode_req.stop_mode_reason !=
                       SYS_STOP_MODE_REASON_IRAT)))
         {
            mm_stop_timer( i );
            mm_timer_status[i] = TIMER_STOPPED;
         }
      }
#ifdef FEATURE_NAS_REL11
/*4.1.1.8	Handling of security related parameters at switch on and switch off*/
       auth_update_hfns_to_sim_card(FALSE);
#endif
   }

   if (is_umts_mode())
   {
      rrc_cmd_type *rrc_cmd = NULL;

#ifdef FEATURE_WCDMA
      rrc_cmd = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );
#endif /* FEATURE_WCDMA */

      if (rrc_cmd == NULL)
      {
        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
      }

      /* Set the Stop mode reason in the RRC_MODE_CHANGE_REQ */
      /* We don't want lower layer to know about IRAT or local detach. Lower layers should treat it as mode change*/
      if((mmcoord_pending_reg_message.cmd.mmr_stop_mode_req.stop_mode_reason ==
          SYS_STOP_MODE_REASON_IRAT)
#ifdef FEATURE_DUAL_SIM
        ||(mmcoord_pending_reg_message.cmd.mmr_stop_mode_req.stop_mode_reason ==
          SYS_STOP_MODE_REASON_MODE_CAP_CHANGE)
#endif
#ifdef FEATURE_LTE
        ||(mmcoord_pending_reg_message.cmd.mmr_stop_mode_req.stop_mode_reason ==
          SYS_STOP_MODE_REASON_LOCAL_DETACH)
#endif
        )
      {
         rrc_cmd->cmd.deact_req.deact_reason = SYS_STOP_MODE_REASON_MODE_CHANGE;
      }
      else
      {
      rrc_cmd->cmd.deact_req.deact_reason =
        mmcoord_pending_reg_message.cmd.mmr_stop_mode_req.stop_mode_reason;
      }
      rrc_cmd->cmd_hdr.cmd_id = RRC_MODE_CHANGE_REQ;
      
      mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE;

      send_cmd_to_rrc( rrc_cmd, mm_serving_plmn.info.active_rat );
   }
   else
   {
      /*
      ** Form RR_DEACT_REQ message
      */
      rr_deact_req_T rr_deact_req;
      memset(&rr_deact_req,0x00,sizeof(rr_deact_req_T));

      rr_deact_req.message_header.message_set = MS_MM_RR;
      rr_deact_req.message_header.message_id  = (int)RR_DEACT_REQ;

      if ((mmcoord_pending_reg_message.cmd.mmr_stop_mode_req.stop_mode_reason ==
            SYS_STOP_MODE_REASON_POWER_OFF) ||
          (mmcoord_pending_reg_message.cmd.mmr_stop_mode_req.stop_mode_reason ==
            SYS_STOP_MODE_REASON_LPM))
      {
         rr_deact_req.deact_reason = RR_MM_POWER_OFF;
      }
      else if (mmcoord_pending_reg_message.cmd.mmr_stop_mode_req.stop_mode_reason ==
            SYS_STOP_MODE_REASON_OFFLINE)
      {
         rr_deact_req.deact_reason = RR_MM_OFF_LINE;
      }
      else
      {
         rr_deact_req.deact_reason = RR_MM_MODE_CHANGE;
      }

      mm_serving_plmn.mm_service_state = SERVICE_NOT_AVAILABLE;
      mm_serving_plmn.gmm_service_state = SERVICE_NOT_AVAILABLE;

      /* Set the Pending message_set to default value */
      mmcoord_pending_reg_message.cmd.hdr.message_set = 0;


      PUT_IMH_LEN( sizeof( rr_deact_req_T ) - sizeof( IMH_T ),
                   &rr_deact_req.message_header );

      MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent RR_DEACT_REQ; cause = %d", rr_deact_req.deact_reason);
      mm_send_message( (IMH_T *)&rr_deact_req, GS_QUEUE_RR );
   }

   mm_waiting_for_deact_cnf = TRUE;
}
