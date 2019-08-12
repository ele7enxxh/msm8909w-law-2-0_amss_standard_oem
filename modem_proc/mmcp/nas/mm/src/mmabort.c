/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmabort.c_v   1.21   01 Jul 2002 09:33:20   jault  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmabort.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/08/01   jca     Added support for RRC_ABORT_IND and RRC_ABORT_CNF primitives.
06/18/01   sbk     Added support for RRC_CLOSE_SESSION_IND primitive processing
06/20/01   sbk     Check for states while processing ABORT. Ciphering Key mgmt
08/07/01   ts      Standardized MSG_ERROR and MSG_HIGH usage.
08/15/01   ab      Renamed CM to CNM.
10/19/01   sbk     No need to change state when recving ABORT_CNF
02/01/02   jca     Created centralized Abort procedure function and fixed bugs in same.
04/23/02   jca     Modified MM-CNM interface primtives.
04/24/02   vdr     Added HFN handling
05/02/02   vdr     Modified HFN handling
05/08/02   ks      Added CNM prefix to NORMAL_RELEASE and ESTABLISHMENT_IN_PROGRESS
                   Release type enums.
05/15/02   vdr     Avoided HFN processing when the MS is running in GSM mode or
                   activated by the GSM SIM
06/03/02   jca     Only set mm_lu_reject_info.reject cause to CAUSE_ACCESS_BARRED when
                   RR explicitly indicates ACCESS_CLASS_NOT_ALLOWED.
07/25/02   jca     Added support for GSM Call Reestablishment procedures.
08/02/02   ks      Now setting proper cause in mmr_reg_cnf & mmr_service_ind.
08/09/02   jca     RRC_MAX_ACCESS_FAILURE now treated as low level failure.
09/28/02   vdr     Changed HFN related handling upon reception of the RRC release indication
10/07/02   vdr     Fixed HFN related handling upon reception of the RRC release indication
10/11/02   cd      Modified call to mm_send_mmcnm_est_rej to include passing
                   the reject cause
11/06/02   jca     Modified code to set MM PLMN info status through calls to
                   new function mm_update_mm_serving_plmn_info().
11/07/02   mks     Added support for Integrity protection on CS domain
11/12/02   jca     Modified function mm_abort_rr().
11/26/02   jca     Mod to reflect change to function mm_update_mm_serving_plmn_info().
01/15/03   ks      Removed the call to mm_abort_rr() after receiving a
                   CLOSE_SESSION_IND in mm_connection_release().
01/30/02   jca     Now setting mm_integrity_protected to FALSE on receipt of
                   RRC_CLOSE_SESSION_IND or RRC_CLOSE_SESSION_CNF.
02/10/03   cd      Remove Lint errors
03/18/03   jca     Now calling mm_state_control() with event MM_STD_RR_CONNECTION_RELEASED
                   on receipt of RRC_CLOSE_SESSION_IND.
03/20/03   ks      Changed the ERR_FATAL to MSG_HIGH in mm_abort_rr() if the
                   active RAT is neigher GSM nor UMTS, to support FTM mode.
03/31/03   jca     Now initiate GPRS Detach procedure on rceipt of RRC_CLOSE_SESSION_IND
                   if gmm_gprs_detach_pending flag set to TRUE.
04/29/03   jca     Added support for change of network mode of operation.
06/16/03   jca     Now consistently ERR_FATAL if attempt to allocate memory fails.
06/16/03   jca     Now checking for NULL mmcoord_pending_reg_message before calling
                   function gmm_initiate_gprs_detach().
07/16/03   jca     Added additional checks on blocking restrictions for initiation
                   of GMM procedures including check for GPRS SUSPENDED.
09/18/03   mks     Merged Kurt's changes to support silent redial feature - Added
                   support for handling lower layer failure with "silent redial not
                   allowed" as the reason.
03/05/04   cd      Now processing MMCNM_REL_REQ in state 20
04/06/04   cd      LCS support
04/22/04   jca     Fixed bug where MMCNM_REL_REQ was not being processed even though
                   MM was holding on to a buffered MMCNM_EST_REQ.
06/08/04   cd      LCS support
06/18/04   jca     Added variable gmm_pending_ra_update_type.
07/22/04   jca     Fixed another bug where MMCNM_REL_REQ was not being processed even
                   though MM was holding on to a buffered MMCNM_EST_REQ.
08/04/04   cd      Sending mmcnm_est_rej, in addition to mmcnm_rel_ind,
                   when receiving a RR(C)_REL_IND in state 20
08/12/04   cd      Replaced FEATURE_LCS by FEATURE_MMGPS
11/16/04   jca     Added delay_sig_con_release flag in RRC_ABORT_REQ.
11/15/04   ks      Removed sending the SERVICE_IND after receving ABORT message
                   with cause ILLEGAL_ME, as this is sent after connection release.
11/17/04   sv      Silent Redial causing GCF test case failures. Prevent silent
                   redial when the RRC connection is genuinely released by n/w.
12/22/04   jca     Prevent silent redial when RRC_ABORT_IND received with
                   abort_cause set to RRC_MAX_ACCESS_FAILURE.
12/23/04   jca     Now calling mm_connection_release() on receipt of RR_CLOSE_SESSION_CNF.
01/25/05   jca     Ensure that a MMR_REG_CNF is sent immediately upon receipt
                   of a RRC_CLOSE_SESSION_IND (CS domain) if mm_reg_waiting_for_reg_cnf
                   and mm_ready_to_send_reg_cnf flags are both set to TRUE.
03/17/05   abe     Stop sending the RR_ABORT_REQ for PS domain on GSM RAT.
04/13/05   sv      Guarding Silent Redial with GCF NV item - silent redial shall
                   NOT apply for GCF.
05/23/05   abe     Set the value of the field auth_failed_bar_cells to FALSE in RR(C)_ABORT_REQ
06/06/05   abe     Added code for CCBS support
09/16/05   vdr     Now injecting MM_STD_LOW_LEVEL_FAIL event into MM state machine only when 
                   there is no ongoing Emergency Call establishment during Inter RAT cell reselection
06/14/06   up      Called new mm_update_mm_serving_plmn_info for GSTK event.
06/15/06   nr      Passing cause in mm_notify_reestablishment_fail.
07/21/06   up      Do nothing if LU is aborted with IMM Assignment reject.
                   Perform next LU after cell change occured or T3122 expired.
08/02/06   ajt     While RRC is OOS and PS call is going on, RRC cannot inform OOS to NAS.
                   If T3212 expires now, IDT procedure for LU will fail. After 4 failures, MM retries only after
                   T3212 expiry. This is spec violation due our UE design constraint. As per spec,
                   UE must delay T3212 processing till UE comes back in service, however, that OOS/back 
                   in service, is not informed to NAS by RRC. To take care of such scenarios, RRC will
                   send close session confirm with cause MM_CAUSE_RRC_OOS_CONNECTED_MODE. NAS will mark
                   LU as pending on receiving this reject cause for close session confirm.
04/09/07   rd      SMS optimization changes are incorporated                                                         
09/26/11   sla     Send AS_REJ_DEEP_FADE to CNM in MN CNM REL IND when RRC/RR connection
                   is released because of radio link failure
10/17/11   gps     checking mmstate before setting lu reject cause
09/26/11   rajesh  Added code to send connection release request to RRC instead of RR, if UE is in WRLF state. 
11/03/11   vijay   Extended Fix for CR- 291101 to cover W->G scenario.
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
11/22/11   gps     pending combimed RAU, until attach procedure is completed
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "ms.h"
#include "timers_v.h"
#include "gs_v.h"
#include "ms_cc_mm.h"
#include "mm_v.h"
#include "rrccmd.h"
#include "cause.h"
#include "mm_coord.h"
#include "mm_sim.h"
#include "mm_umts_v.h"
#include "mmauth.h"
#include "gmm_int_v.h"
#include <stringl/stringl.h>
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
#include "mm_lsm.h"
#endif


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

#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE))
 #error code not present
#endif // TEST_FRAMEWORK


/*===========================================================================

                    DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION MM_ABORT_PROCEDURE

DESCRIPTION
  This function performs Abort procedure processing as defined in
  TS 24.008 section 4.3.5.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_abort_procedure( mm_cmd_type *msg_p )
{
   reject_cause_T         reject_cause;

   reject_cause = ((abort_msg_T *)(&msg_p->cmd))->reject_cause;

   /* -----------------------------------------------------------------------
   ** The mobile station shall treat ABORT message as compatible with current
   ** protocol state only if it is received when at least one MM connection
   ** exists or an MM connection is being established.  TS 24.008 4.3.5
   ** ----------------------------------------------------------------------- */
   if (mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION            ||
       mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION ||
       mm_state == MM_WAIT_FOR_REESTABLISH_DECISION              ||
       mm_state == MM_WAIT_FOR_RR_CONNECTION_REESTABLISHMENT     ||
       mm_state == MM_REESTABLISHMENT_INITIATED                  ||
       mm_state == MM_CONNECTION_ACTIVE               
#ifdef FEATURE_CCBS 
                                                                 ||
       mm_state == MM_PROCESS_CM_SERVICE_PROMPT
#endif /* FEATURE_CCBS */       
                                                )             
   {
      /* -------------------------------------------------------------------------
      ** At the receipt of the ABORT message the mobile station shall abort any MM
      ** connection establishment or call reestablishment procedure and release
      ** all MM connections. TS 24.008 4.3.5.2
      ** ------------------------------------------------------------------------- */
      if (mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION)
      {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
         mm_send_mmcnm_est_rej(OTA_REJECT_CAUSE, reject_cause, MMCC_SMS_CS);
#else
         mm_send_mmcnm_est_rej(OTA_REJECT_CAUSE, reject_cause);
#endif
      }
      else
      {
         mm_send_mmcnm_rel_ind( ABORT_MSG_RECEIVED, FALSE );
      }

      mm_stop_timer( TIMER_T3230 );

      if (reject_cause == ILLEGAL_ME)
      {
         /* ------------------------------------------------------------------------
         ** If cause value #6 is received the mobile station shall delete any TMSI,
         ** LAI, and ciphering key sequence number stored in the SIM, set the update
         ** status to ROAMING_NOT_ALLOWED and consider the SIM invalid.
         ** TS 24.008 4.3.5.2
         ** ------------------------------------------------------------------------ */
         mm_delete_lu_status( ROAMING_NOT_ALLOWED );

         mm_update_mm_serving_plmn_info( SYS_SIM_STATE_CS_INVALID,
                                         SYS_SRV_DOMAIN_NO_SRV,
                                         FALSE,
                                         SYS_SRV_STATUS_LIMITED,
                                         TRUE );

         /* ------------------------------------------------------------------
         ** As a consequence the mobile station enters state MM IDLE, substate
         ** NO IMSI after the release of the RR connection.  TS 24.008 4.3.5.2
         ** ------------------------------------------------------------------ */
         mm_substate_control( MM_STD_NO_IMSI );
      }

      /* -----------------------------------------------------------------
      ** The mobile station shall then wait for the network to release the
      ** RR connection - see section 4.5.3.1. (TS 24.008 4.3.5.2)
      ** ----------------------------------------------------------------- */
      mm_state_control( MM_STD_ABORT_MESSAGE_RECEIVED );
      mm_start_timer( TIMER_T3240, DEFAULT_TIMEOUT );
   }
   else
   {
     MSG_HIGH_DS_0( MM_SUB, "=MM= ABORT msg rxed while there is no MM connection");
   }
}


/*===========================================================================

FUNCTION MM_ABORT_RR

DESCRIPTION
  This function sends an ABORT_REQ to abort an existing RR/RRC connection

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_abort_rr( rrc_cn_domain_identity_e_type domain_id, boolean delay_sig_con_release )
{

 if((!mm_hard_abort_pending)
#ifdef FEATURE_DUAL_SIM
   && (mm_abort_dual_switch_pending == SYS_STOP_MODE_REASON_NONE)
#endif
#if defined FEATURE_SGLTE || defined FEATURE_1XSRLTE
   && (!mm_hard_abort_ue_sglte_mode_switch_pending)
#endif
    )
 {
   if(rrc_connection_status != MMRRC_RELEASE_IN_PROGRESS)
   {
       
#ifdef FEATURE_WRLF_SYSTEM_SEL
     if(mm_wcdma_rlf_state == WCDMA_RLF_SERVICE_CNF_PENDING)
     {
       MSG_HIGH_DS_0(MM_SUB,"Hard abort already sent to RRC, ignoring this abort request");
     }
     else
#endif
     if (is_umts_mode()
#ifdef FEATURE_WRLF_SYSTEM_SEL
       || (mm_wcdma_rlf_state != WCDMA_RLF_IDLE)
#endif
      )
     {
#ifdef FEATURE_WCDMA         
        rrc_cmd_type *rrc_cmd_p = NULL;
#ifdef FEATURE_WRLF_SYSTEM_SEL 
        if (mm_wcdma_rlf_state != WCDMA_RLF_IDLE)
        {
#ifdef FEATURE_MODEM_HEAP
          rrc_cmd_p = mm_get_rrc_cmd_buf( mm_wrlf_serving_plmn_ptr->info.active_rat  );
#else
          rrc_cmd_p = mm_get_rrc_cmd_buf( mm_wrlf_serving_plmn.info.active_rat  );
#endif
        }
        else
#endif /* FEATURE_WRLF_SYSTEM_SEL */
        if (is_umts_mode())
        {
        rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );
        }
		
        if (rrc_cmd_p == NULL)
        {
          MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
        }

        if (((domain_id == RRC_CS_DOMAIN_CN_ID) &&
             (ps_session_status == MMRRC_IDLE)) ||
            ((domain_id == RRC_PS_DOMAIN_CN_ID) &&
             (cs_session_status == MMRRC_IDLE)))
        {
           rrc_cmd_p->cmd_hdr.cmd_id = RRC_ABORT_REQ;
           rrc_cmd_p->cmd.rrc_abort_req.auth_failed_bar_cells = FALSE;
           rrc_cmd_p->cmd.rrc_abort_req.hard_abort = FALSE;
           rrc_cmd_p->cmd.rrc_abort_req.abort_cause = RRC_OTHERS;
           rrc_cmd_p->cmd.rrc_abort_req.delay_sig_con_release = delay_sig_con_release;
        }
        else
        {
          rrc_cmd_p->cmd_hdr.cmd_id = RRC_CLOSE_SESSION_REQ;
          rrc_cmd_p->cmd.close_sess_req.cn_domain_id = domain_id;
          rrc_cmd_p->cmd.close_sess_req.close_session_cause = RRC_OTHERS;
        }

#ifdef FEATURE_WRLF_SYSTEM_SEL 
        if (mm_wcdma_rlf_state != WCDMA_RLF_IDLE)
        {
#ifdef FEATURE_MODEM_HEAP
          send_cmd_to_rrc( rrc_cmd_p, mm_wrlf_serving_plmn_ptr->info.active_rat  );
#else
          send_cmd_to_rrc( rrc_cmd_p, mm_wrlf_serving_plmn.info.active_rat  );
#endif
        }
        else
#endif /* FEATURE_WRLF_SYSTEM_SEL */
        if (is_umts_mode())
        {
          send_cmd_to_rrc( rrc_cmd_p, mm_serving_plmn.info.active_rat );
        }
#endif /* FEATURE_WCDMA */      
     }

     else if( is_gsm_mode())
     {
        rr_abort_req_T    rr_abort_req;
        memset(&rr_abort_req,0x00,sizeof(rr_abort_req_T));
        /*
         * form RR_ABORT_REQ message with reject_cause
         */
        if (domain_id == RRC_PS_DOMAIN_CN_ID)
        {
           MSG_HIGH_DS_0( MM_SUB, "=MM= PS on GSM, no RR(C) Deactivation needed");
        }
        else 
        {
           MSG_HIGH_DS_0( MM_SUB, "=MM= MM sent RR_ABORT_REQ");
           mm_send_rr_abort_req(FALSE);
        }
     }
     else
     {
        MSG_HIGH_DS_0( MM_SUB, "=MM= Unknown RAT, no RR(C) Deactivation needed");
     }
   }
   else
   {
      MSG_HIGH_DS_0(MM_SUB, "=MM= Doing Nothing as RRC connection is in MMRRC_RELEASE_IN_PROGRESS");
   }

 }
 else
 {
    MSG_HIGH_DS_0( MM_SUB, "=MM= Already Hard Abort pending.");
 }
}


/*===========================================================================

FUNCTION MM SEND RR ABORT REQ

DESCRIPTION
  This function forms PDU for RR_ABORT_REQ and sends it. It also sets relavant flags.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_rr_abort_req (boolean auth_failed_bar_cells)
{
  
  rr_abort_req_T    rr_abort_req;
  memset(&rr_abort_req,0x00,sizeof(rr_abort_req_T));

  rr_abort_req.message_header.message_set = MS_MM_RR;
  rr_abort_req.message_header.message_id    = (int) RR_ABORT_REQ;
  /*lint -save -e778 */
  PUT_IMH_LEN( sizeof( rr_abort_req_T ) - sizeof(IMH_T),
           &rr_abort_req.message_header);
  /*lint -restore */
  
  rr_abort_req.auth_failed_bar_cells = auth_failed_bar_cells;

#ifdef FEATURE_LTE
  if ((mm_per_nas_stacks_is_mt_page_in_progress((sys_modem_as_id_e_type)mm_as_id)) &&
       (mm_timer_status[TIMER_T3240] != TIMER_ACTIVE) &&
       (mm_location_information.location_update_status == UPDATED) &&
       (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0) &&
       (mm_state != MM_CONNECTION_ACTIVE) &&
       (mm_waiting_for_csmt_response == TRUE))
  {
    rr_abort_req.csmt_fail_depriotize_cells  = TRUE;
  }
  else
#endif
  {
    rr_abort_req.csmt_fail_depriotize_cells  = FALSE;
  }
  
  MSG_HIGH_DS_1(MM_SUB, "=MM= RR_ABORT_REQ with csmt_fail_depriotize_cells = %d", rr_abort_req.csmt_fail_depriotize_cells);
  
  rrc_connection_status = MMRRC_RELEASE_IN_PROGRESS;
  mm_send_message( (IMH_T *)&rr_abort_req, GS_QUEUE_RR );
  
}





/*===========================================================================

FUNCTION MM_CONNECTION_RELEASE

DESCRIPTION
  This function deal with messages relating to MM connection release.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_connection_release( mm_cmd_type *msg_p )
{
   reject_cause_T reject_cause = REJ_UNSPECIFIED;

#ifdef FEATURE_CONNECTED_MODE_SERV_IND   
   byte           local_rel_lu_abort = FALSE;
#endif 

   if (is_OTA_message(msg_p) && get_OTA_msg_type(msg_p) == ABORT)
   {
      mm_abort_procedure( msg_p );
   }
   else if (((msg_p->cmd.hdr.message_set == MS_MM_RR) ||
             ((msg_p->cmd.hdr.message_set == MS_MM_TDSRRC)
#ifdef FEATURE_SEGMENT_LOADING
             &&get_tdscdma_interface()
#endif
         ))
      && (     (msg_p->cmd.hdr.message_id == (int) RR_REL_IND)
            || (msg_p->cmd.hdr.message_id == (int) RRC_REL_IND)
            || (msg_p->cmd.hdr.message_id == (int) RRC_CLOSE_SESSION_IND)
            || (msg_p->cmd.hdr.message_id == (int) RRC_CLOSE_SESSION_CNF)
         )
      )
   {

     /* --------------------------------------------------------------------
     ** 24.008 4.4.4.9, 4.5.1.2: cases f.1 
     ** RR connection release in Iu mode before the completion of the procedure:
     ** The procedure shall be initiated again conditionally
     ** -------------------------------------------------------------------- */
     if((mm_cs_domain_possible_reest &&
        ((msg_p->cmd.hdr.message_id == (int) RRC_CLOSE_SESSION_CNF) ||
         (((mm_state == MM_LOCATION_UPDATE_INITIATED)||
           (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU) ||
           (mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION) ||
           (mm_state == MM_WAIT_FOR_RR_CONNECTION_MM)) &&
           (msg_p->cmd.hdr.message_id == (int) RRC_REL_IND))))
#ifdef FEATURE_MODEM_HEAP
           && (pending_mm_msg != NULL)
#endif          
           )
         
     {
       MSG_HIGH_DS_0(MM_SUB, "=MM= Re-establishing after unintended abort during MM procedure ");
#ifdef FEATURE_MODEM_HEAP
       if(pending_mm_msg == NULL)
       {
          pending_mm_msg = (byte *) modem_mem_calloc(1,MAX_MESSAGE_SIZE,MODEM_MEM_CLIENT_NAS);
          mm_check_for_null_ptr((void*)pending_mm_msg);
       }
#endif
       if ((mm_state == MM_LOCATION_UPDATE_INITIATED)||
           (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU))
       {
         mm_stop_timer( TIMER_T3210); 
         mm_start_timer( TIMER_T3210, DEFAULT_TIMEOUT );

       }
       else if((mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION) ||
              (mm_state == MM_WAIT_FOR_RR_CONNECTION_MM))
       {
          mm_stop_timer( TIMER_T3230); 
          mm_start_timer( TIMER_T3230, DEFAULT_TIMEOUT );
       }
       mmrrcconn_send_L3_message_to_rrc(
              pending_mm_msg, pending_mm_msg_cause, RRC_CS_DOMAIN_CN_ID );

       mm_state_control( MM_STD_REESTABLISHMENT_REQUIRED );

       return;
     }
     else
     {
       MSG_HIGH_DS_1(MM_SUB, "=MM= Not reestablishing. Flag: %d", mm_cs_domain_possible_reest);
     }
     mm_cs_domain_possible_reest = FALSE;

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
      /* -------------------------------
      ** Abort a LCS session in progress
      ** ------------------------------- */
      if (mm_lcs_active)
      {
         mm_abort_lcs();
      }
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */

#ifdef FEATURE_TC
      mm_tc_is_active = FALSE;
#endif

      if (msg_p->cmd.hdr.message_id == (int) RRC_REL_IND)
      {
         if (is_umts_mode() && (mm_sim_card_mode == MMGSDI_APP_USIM))
         {
            /* -----------------------------------------------------
            ** Integrity protection on CS domain is no longer active
            ** ----------------------------------------------------- */
            mm_integrity_protected = FALSE;

            /* ----------------------------------------------------------
            ** Save the HFN values to the SIM on a RRC release indication
            ** ---------------------------------------------------------- */
            auth_handle_hfn_release_ind(
                msg_p->cmd.rrc_rel_ind.hfn_cs, msg_p->cmd.rrc_rel_ind.hfn_ps) ;
         }

         MSG_LOW_DS( MM_SUB, "=MM= RRC Conn Released normally with cause %d",
                   msg_p->cmd.rrc_rel_ind.rel_cause, 0, 0);
         reject_cause = AS_REJ_RRC_REL_IND;

         /* ----------------------------------------------------
         ** Silent Redial issue for GCF test cases: For specific
         ** causes of RRC Connection release by the network, send
         ** an appropriate cause to CM to prevent silent redial
         ** -------------------------------------------------- */

         /* ------------------------------------------------
         ** To improve performance, do NOT attempt redial if
         ** NV_GPRS_ANITE_GCF_I is turned ON, else attempt
         ** ----------------------------------------------*/
         if (gmm_anite_gcf_flag)
         {
            if ( (msg_p->cmd.rrc_rel_ind.rel_cause == RRC_REL_CAUSE_NORMAL) ||
                 (msg_p->cmd.rrc_rel_ind.rel_cause == RRC_REL_CAUSE_UNSPEC) )
            {
                /* -----------------------------------------------------------
                ** Silent Redial by CM not required, set rej cause accordingly
                ** -------------------------------------------------------- */
                reject_cause = RR_ABORT_REDIAL_NOT_ALLOWED;
            }
         }
         if ( (msg_p->cmd.rrc_rel_ind.rel_cause != RRC_REL_CAUSE_UNSPEC) && 
              ((mm_state == MM_LOCATION_UPDATE_INITIATED)||
               (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU))
            )  
         {
            /*-----------------------------------------------
            ** LU is failed because of d  or f of section 4.4.4.9 of 24.0.08
            ** ------------------------------------------------*/
            
            if (mm_serving_cell_valid)
            {
               mm_lu_failed_cell_id = mm_serving_cell_id;
               mm_initiate_lu_upon_entry_into_new_cell = TRUE;
            }
         }

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
         mm_stop_timer( TIMER_T3241 );
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
      }
      else if (msg_p->cmd.hdr.message_id == (int) RR_REL_IND)
      {
         MSG_HIGH_DS_1( MM_SUB, "=MM= MM received RR_REL_IND with cause %d", msg_p->cmd.rr_rel_ind.RR_cause);
         if((mm_per_nas_stacks_is_mt_page_in_progress((sys_modem_as_id_e_type)mm_as_id)) && 
            (mm_state!=MM_CONNECTION_ACTIVE) && 
            (( (mm_timer_status[TIMER_T3240] != TIMER_ACTIVE) || (mm_connection_activated == FALSE) ) ||(mm_waiting_for_csmt_response == TRUE)))
         {
           if(mm_waiting_for_csmt_response == TRUE)
           {
             mm_send_cm_mt_call_fail_ind(CM_MM_MT_CALL_FAIL_CAUSE_MT_CSFB_NO_RESPONSE_FROM_NW);
#ifdef FEATURE_LTE
             if ((mm_location_information.location_update_status == UPDATED) &&
                 (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0))
             {
               mm_send_rr_mt_csfb_failure_ind(TRUE);
             }
#endif
           }
           else
           {
             mm_send_cm_mt_call_fail_ind(CM_MM_MT_CALL_FAIL_CAUSE_NO_RESPONSE_FROM_NW);
           }
         }

         mm_waiting_for_csmt_response = FALSE;
         
#ifdef FEATURE_DUAL_SIM
         if(msg_p->cmd.rr_rel_ind.failure_cause == MM_RR_NO_RESOURCE_AVAILABLE)
         {
           reject_cause = AS_REJ_ABORT_RADIO_UNAVAILABLE;
         }
         else
#endif
         {
           reject_cause = AS_REJ_RR_REL_IND;
         }

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
         mm_stop_timer( TIMER_T3241 );
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */

         if ( (msg_p->cmd.rr_rel_ind.RR_cause != RR_ABNORMAL_RELEASE_UNSPECIFIED ) && 
              ((mm_state == MM_LOCATION_UPDATE_INITIATED)||
               (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU))
            )
         {

            /*-----------------------------------------------
            **LU is failed because of d  or f of section 4.4.4.9 of 24.0.08
            **------------------------------------------------*/
         
            mm_initiate_lu_upon_entry_into_new_cell = TRUE;
          
         }
   
      }
      else /* RRC_CLOSE_SESSION_IND */
      {
         if (mm_state == MM_LOCATION_UPDATE_INITIATED)
         {
            /*-----------------------------------------------
            **LU is failed because of d  or f of section 4.4.4.9 of 24.0.08
            **------------------------------------------------*/
            
            if (mm_serving_cell_valid)
            {
               mm_lu_failed_cell_id = mm_serving_cell_id;
               mm_initiate_lu_upon_entry_into_new_cell = TRUE;
            }
         }
            /* -----------------------------------------------------
         ** Integrity protection on CS domain is no longer active
         ** ----------------------------------------------------- */
         mm_integrity_protected = FALSE;

         reject_cause = AS_REJ_RRC_CLOSE_SESSION_IND;

         if ((mm_reg_waiting_for_reg_cnf) && (mm_ready_to_send_reg_cnf))
         {
            mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
            mm_send_mmr_reg_cnf();

            mm_reg_waiting_for_reg_cnf = FALSE;
            mm_ready_to_send_reg_cnf   = FALSE;
         }
         if((mm_per_nas_stacks_is_mt_page_in_progress((sys_modem_as_id_e_type)mm_as_id)) && 
            (mm_state!=MM_CONNECTION_ACTIVE) &&
            (( (mm_timer_status[TIMER_T3240] != TIMER_ACTIVE) || (mm_connection_activated == FALSE) ) ||(mm_waiting_for_csmt_response == TRUE))) 
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
         mm_waiting_for_csmt_response = FALSE;
      }

      /* Report the RR connection release indication to the MM control */
      mm_stop_timer( TIMER_T3240 );
      mm_stop_timer( TIMER_T3230 );

      if (mm_cnm_is_active)
      {
         /*
          * check if this release is a normal release while there is
          * a pending service request, as in this case, the original
          * request from CNM is still being honoured, so dont tell it
          * the request has failed, yet...
          */

         if  ( mm_held_cnm_service_req.present == FALSE )
         {

            /*
             * if during reestablishment, send reestablishment fail to CNM
             */

            if ( ( mm_state == MM_WAIT_FOR_RR_CONNECTION_REESTABLISHMENT ) ||
                 ( mm_state == MM_REESTABLISHMENT_INITIATED ) )
            {
               mm_notify_reestablishment_fail(AS_REJECT_CAUSE, reject_cause );
            }
            else if ((mm_state == MM_WAIT_FOR_RR_CONNECTION_MM) ||
                     (mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION))
            {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
               mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, reject_cause , MMCC_SMS_CS);
#else
               mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, reject_cause );
#endif
            }
            else
            {
               if (mm_cnm_rel_req_received)
               {
                  mm_send_mmcnm_rel_cnf();
               }
               else
               {
                  if (mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION)
                  {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                     mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, reject_cause , MMCC_SMS_CS);
#else
                     mm_send_mmcnm_est_rej(AS_REJECT_CAUSE, reject_cause );
#endif
                  }
                  if (((msg_p->cmd.hdr.message_id == (int) RRC_REL_IND)  &&
                       (msg_p->cmd.rrc_rel_ind.rel_cause == RRC_REL_DEEP_FADE )) ||
                      ((msg_p->cmd.hdr.message_id == (int) RRC_CLOSE_SESSION_IND)  &&
                       (msg_p->cmd.rrc_close_session_ind.cause == RRC_SESSION_CLOSE_RL_FAILURE  ))
                     )
                  {
                     mm_send_mmcnm_rel_ind( AS_REJ_DEEP_FADE, FALSE );
                  }
                  else
                  {
                     mm_send_mmcnm_rel_ind( OTHER_CAUSE, FALSE );
                  }
               }
            }
         }
      }

#ifdef FEATURE_CONNECTED_MODE_SERV_IND
      if (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU &&
          msg_p->cmd.hdr.message_id == (int) RRC_CLOSE_SESSION_CNF &&
          msg_p->cmd.rrc_close_session_cnf.status == RRC_CLOSE_SESS_UE_OOS)
      {
         mm_lu_reject_info.reject_cause = MM_CAUSE_RRC_OOS_CONNECTED_MODE;
      }    
      else if (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU &&
               msg_p->cmd.hdr.message_id == (int) RRC_REL_IND &&
               msg_p->cmd.rrc_rel_ind.rel_cause == RRC_REL_CAUSE_OOS)
      {
         mm_lu_reject_info.reject_cause = MM_CAUSE_RRC_CONN_LOCAL_REL_OOS;
         local_rel_lu_abort = TRUE;
      }
#endif      
      mm_state_control( MM_STD_RR_CONNECTION_RELEASED );

#ifdef FEATURE_CONNECTED_MODE_SERV_IND
      if (local_rel_lu_abort == TRUE &&
          mm_get_lu_cause() == MM_LU_CAUSE_PERIODIC)
      {
         mm_pending_lu_request = T3212_EXPIRED;
      }
#endif
      if ((msg_p->cmd.hdr.message_id == (int) RRC_CLOSE_SESSION_IND) ||
          (msg_p->cmd.hdr.message_id == (int) RRC_CLOSE_SESSION_CNF))
      {
         if (gmm_gprs_attach_pending)
         {
           gmm_initiate_gprs_attach();
         }
#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)         
         else if (gmm_gprs_detach_pending)
         {
            if (mmcoord_pending_reg_message.cmd.hdr.message_set != 0)
            {
               gmm_initiate_gprs_detach( &mmcoord_pending_reg_message );
            }
            else
            {
               MSG_HIGH_DS_0( MM_SUB, "=MM= Unable to proceed with GPRS Detach");
            }
         }
         else 
#endif /*#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/          
         if (gmm_ra_update_pending) 
         {
           if ((mm_state != MM_IDLE ) &&
              ((gmm_pending_ra_update_type == GMM_COMBINED_RA_LA_UPDATING) ||
               (gmm_pending_ra_update_type == GMM_COMBINED_RA_LA_UPDATING_WITH_IMSI_ATTACH)) &&
               (gmm_state == GMM_REGISTERED) &&
            (!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                                 gmm_stored_rai.routing_area_code,
                                 mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
              (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn)))) &&
              (gmm_update_status == GMM_GU1_UPDATED) 
#ifdef FEATURE_LTE                
             && (emm_ctrl_data_ptr->TIN != NAS_MM_TIN_GUTI)
#endif               
               )
           {
              MSG_HIGH_DS_2(MM_SUB,"=MM= RAU should not be initiated since UE is already registered, gmm_pending_ra_update_type %d, gmm_update_status %d,,,",                        
                                                             gmm_pending_ra_update_type,gmm_update_status);
           }
 
           else if((gmm_state != GMM_REGISTERED_INITIATED) &&  
              (gmm_state != GMM_ROUTING_AREA_UPDATING_INITIATED))
           {
              gmm_ra_update_type = gmm_pending_ra_update_type;
              gmm_initiate_routing_area_update();
           }
           else
           {
              MSG_HIGH_DS_0(MM_SUB,"=MM= Pending Combined RAU as NOrmal RAU in Progress");
           }
         }
      }

      mm_send_mmcc_sig_conn_rel_ind();
      mm_send_cm_cipher_ind(SYS_SRV_DOMAIN_CS_ONLY,FALSE);

   }
   else if ((msg_p->cmd.hdr.message_set == MS_CC_MM) &&
            (msg_p->cmd.hdr.message_id  == MMCNM_REL_REQ))
   {
#ifdef FEATURE_LTE
      if(mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG)
      {
        MSG_HIGH_DS_0(MM_SUB,"CSFB: Clearing pending MO call orignation");
        mm_follow_on_request = FALSE;      
        mm_send_mmcnm_rel_cnf();
        mm_csfb_set_state(MM_CSFB_STATE_NULL);

        if((is_gsm_mode()) &&
           ((mm_csfb_is_blind_redir_to_lte_req_to_be_sent == TRUE)||
            (emm_ctrl_data_ptr->rr_blind_redir_pending_call_abort == TRUE)))
        {
          mm_send_rr_csfb_blind_redir_to_lte_req(TRUE);
        }
      }
      else
#endif
#ifdef FEATURE_TC
      if (mm_tc_is_active)
      {
        MSG_HIGH_DS_0(MM_SUB,"=MM= TC is CS active, loopback CNM REL CNF");
        mm_cnm_rel_req_received = TRUE;
        mm_send_mmcnm_rel_cnf();
      }
      else
#endif
      {
        switch (mm_state)
        {
           case MM_WAIT_FOR_RR_CONNECTION_MM:
           case MM_WAIT_FOR_OUTGOING_MM_CONNECTION:
           case MM_WAIT_FOR_RR_CONNECTION_REESTABLISHMENT:
           case MM_REESTABLISHMENT_INITIATED:
              if (msg_p->cmd.mmcnm_rel_req.rel_type == CNM_ESTABLISHMENT_IN_PROGRESS)
              {
                 mm_cnm_rel_req_received = TRUE;
                 mm_cnm_is_active        = TRUE;
  
                 /*
                  * If we have not received a CM_SERVICE_ACCEPT/REJECT, stop
                  * T3230
                  */
                 if (mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION)
                 {
                   mm_stop_timer( TIMER_T3230 );
                 }
  
                 if ((gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED) &&
				     (pmm_mode == PMM_IDLE) &&
				     (pending_gmm_msg_valid))
                 {
                   gmm_ra_update_pending = TRUE;
                 }

                 mm_state_control( MM_STD_LAST_CONNECTION_RELEASED );
  
              }
              else
              {
                 MSG_ERROR_DS( MM_SUB, "=MM= Unexpected rel_type %d", msg_p->cmd.mmcnm_rel_req.rel_type, 0,0 );
              }
              break;
  
           case MM_CONNECTION_ACTIVE:
           case MM_WAIT_FOR_NETWORK_COMMAND:
           case MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION:
              if (msg_p->cmd.mmcnm_rel_req.rel_type == CNM_NORMAL_RELEASE)
              {
                 mm_cnm_rel_req_received = TRUE;
  
                 /*
                  * If we have not received a CM_SERVICE_ACCEPT/REJECT, stop
                  * T3230
                  */
                 if (mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION)
                 {
                   mm_stop_timer( TIMER_T3230 );
                 }

  #if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
                 if (mm_lcs_active)
                 {
                    mm_state_control(MM_STD_RR_CONNECTION_REL_NOT_ALLOWED);
                 }
                 else
                 {
                    mm_state_control(MM_STD_LAST_CONNECTION_RELEASED);
                 }
  #else
                 mm_state_control( MM_STD_LAST_CONNECTION_RELEASED );
  
  #endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
              }
              else
              {
                 MSG_ERROR_DS(MM_SUB, "=MM= Unexpected rel_type %d in state %d",
                          msg_p->cmd.mmcnm_rel_req.rel_type,
                          mm_state,
                          0);
              }
              break;
  
           case MM_IDLE:
           case MM_WAIT_FOR_REESTABLISH_DECISION:
              if ((msg_p->cmd.mmcnm_rel_req.rel_type == CNM_NORMAL_RELEASE) ||
                  ((msg_p->cmd.mmcnm_rel_req.rel_type == CNM_ESTABLISHMENT_IN_PROGRESS) &&
                   ((mm_cnm_is_active) || (mm_held_cnm_service_req.present))))
              {
                 mm_cnm_rel_req_received = TRUE;
                 mm_state_control( MM_STD_LAST_CONNECTION_RELEASED );
              }
              else
              {
                 MSG_ERROR_DS( MM_SUB, "=MM= Unexpected rel_type %d in state %d",
                       msg_p->cmd.mmcnm_rel_req.rel_type, mm_state, 0 );
              }
              break;
  
           default:
              mm_send_mmcnm_rel_cnf();
#ifdef FEATURE_LTE
              if((is_gsm_mode()) &&
                   ((mm_csfb_is_blind_redir_to_lte_req_to_be_sent == TRUE)||
                   (emm_ctrl_data_ptr->rr_blind_redir_pending_call_abort == TRUE)))
              {
                mm_send_rr_csfb_blind_redir_to_lte_req(TRUE);
              }
#endif
              break;
        } 
      }
   }
#ifdef FEATURE_CCBS 
   else if ((msg_p->cmd.hdr.message_set == MS_CC_MM) &&
            (msg_p->cmd.hdr.message_id  == MMCNM_PROMPT_RES))
   {
      switch (mm_state)
      {
         case MM_PROCESS_CM_SERVICE_PROMPT:
         
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
               if (mm_lcs_active)
               {
                  mm_state_control(MM_STD_RR_CONNECTION_REL_NOT_ALLOWED);
               }
               else
               {
                  mm_state_control(MM_STD_LAST_CONNECTION_RELEASED);
               }
#else

            mm_state_control( MM_STD_LAST_CONNECTION_RELEASED );
            
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */
            break;
         default:
            MSG_ERROR_DS( MM_SUB, "=MM= MMCNM_PROMPT_RES rxed in unexpected state %d", mm_state, 0,0 );
      }
   }
#endif /* FEATURE_CCBS */

   else
   {
      MSG_ERROR_DS(MM_SUB, "=MM= MM: Unexpected message %d",
          (msg_p->cmd.hdr.message_set)*0x100 + (msg_p->cmd.hdr.message_id), 0,0);
   }
}


/*===========================================================================

FUNCTION MM_REPORT_LOW_LEVEL_FAILURE

DESCRIPTION
  This function flags the occurrence of a low level failure when an RR/RRC
  connection exists.

  The information in the input message indicating the reason for the
  abortion is not used, except, perhaps, for diagnostic information. Input
  parameter is pointer to RR_ABORT_IND message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_report_low_level_failure( mm_cmd_type *msg_p )
{

   mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION);   

   boolean init_lu_cs_access_allowed = FALSE;
   /*
    * in case this was in a LU procedure, set up the cause to default value
    */

   
   if((msg_p->cmd.hdr.message_set == MS_MM_RR)&&
      (msg_p->cmd.hdr.message_id == RR_ABORT_IND)&&
      (mm_state == MM_LOCATION_UPDATE_REJECTED))
   {
     MSG_HIGH_DS_0( MM_SUB, "=MM= MM received Unexpected RR_ABORT_IND after LU reject");
   }
   else
   {
     mm_lu_reject_info.reject_cause = 0;
   } 


#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
   /* --------------------------------
   ** Abort a LCS session in progress
   ** -------------------------------- */
   if (mm_lcs_active)
   {
      mm_abort_lcs();
   }
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */

#ifdef FEATURE_TC
   mm_tc_is_active = FALSE;
#endif

   if ( ( mm_state == MM_CONNECTION_ACTIVE ) ||
        ( mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION ) )
   {
#ifdef FEATURE_SGLTE
      if ((msg_p->cmd.hdr.message_id == RR_ABORT_IND) &&
          (msg_p->cmd.rr_abort_ind.RR_abort_cause == OTHER_REASON) &&
          (msg_p->cmd.rr_abort_ind.failure_cause  == MM_RR_SGLTE_SRVCC_CS_HANDOVER))
      {
        /* Possible due to crossover, on ACT_IND cause SRVCC we move to IDLE*/
        MSG_HIGH_DS_1( MM_SUB,"=MM= ignore RR_ABORT_IND failure_cause %d", 
                                       msg_p->cmd.rr_abort_ind.failure_cause);
      }
      else 
#endif
      if ( mm_system_information.reestablishment_allowed )
      {
         if ((msg_p->cmd.hdr.message_id == RR_ABORT_IND) &&
             (msg_p->cmd.rr_abort_ind.RR_abort_cause  == RR_CONNECTION_FAILURE))
         {
           mm_send_mmcnm_rel_ind(AS_REJ_DEEP_FADE,TRUE);
         }
         else
         {
           mm_send_mmcnm_rel_ind(OTHER_CAUSE,TRUE);
         }
         mm_state_control( MM_STD_LOW_LEVEL_FAIL_REEST_ALLOWED );
      }
      else
      {
         if ((msg_p->cmd.hdr.message_id == RR_ABORT_IND) &&
             (msg_p->cmd.rr_abort_ind.RR_abort_cause  == RR_CONNECTION_FAILURE))
         {
           mm_send_mmcnm_rel_ind(AS_REJ_DEEP_FADE,FALSE);
         }
         else
         {
           mm_send_mmcnm_rel_ind(OTHER_CAUSE,FALSE);
         }
         mm_state_control( MM_STD_LOW_LEVEL_FAIL_REEST_NOT_ALLOWED );
      }
   }
   else
   {
      switch (msg_p->cmd.hdr.message_id)
      {
         case RR_ABORT_IND:

            MSG_HIGH_DS_1( MM_SUB, "=MM= MM received RR_ABORT_IND with cause %d", msg_p->cmd.rr_abort_ind.RR_abort_cause);

            mm_waiting_for_csmt_response = FALSE;
            
#ifdef FEATURE_SGLTE
            if ((msg_p->cmd.rr_abort_ind.RR_abort_cause == OTHER_REASON) &&
                (msg_p->cmd.rr_abort_ind.failure_cause  == MM_RR_SGLTE_SRVCC_CS_HANDOVER))
            {
              if(mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
              {  
                  mm_lu_reject_info.reject_cause = CAUSE_SRVCC_SGLTE_CS_HANDOVER;
              }
              mm_state_control( MM_STD_RADIO_UNAVAILABLE );
            }
            else
#endif 
#ifdef FEATURE_DUAL_SIM
            if (msg_p->cmd.rr_abort_ind.failure_cause == MM_RR_NO_RESOURCE_AVAILABLE)
            {
              if(mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
              {  
                  mm_lu_reject_info.reject_cause = CAUSE_RADIO_UNAVAILABLE;
                  mm_lu_reject_info.mm_conn_rej_timer_value = 0;
              }
              mm_state_control( MM_STD_RADIO_UNAVAILABLE );
            }
            else
#endif
#ifdef FEATURE_LTE
            // CSFB MT call for CCO failed. Make another attempt to send the page.
            if(mm_csfb_state == MM_CSFB_STATE_MT_CALL_CCO)
            {
              MSG_HIGH_DS_0(MM_SUB,"=MM= CSFB MT call for CCO failed. Make another attempt to send the page");
              mm_csfb_send_paging_response();
             /*We have performed second attempt to send page as well
               Set the.CSFB state to LU pending if LU is need to be performed
               after call end, otherwise set it to NULL*/
              mm_reset_mt_csfb_cco_state();
            }
            else
#endif
            if (msg_p->cmd.rr_abort_ind.RR_abort_cause == RANDOM_ACCESS_FAILURE )
            {
               if(mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
               {
                 mm_lu_reject_info.reject_cause = CAUSE_RANDOM_ACCESS_FAILURE;
               }
               mm_state_control( MM_STD_RANDOM_ACCESS_FAILURE );
            }
            else
            {
               if (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU || mm_state == MM_LOCATION_UPDATE_INITIATED)
               {
                  /*-----------------------------------------------
                  **LU is failed because of d  or f of section 4.4.4.9 of 24.0.08
                  **------------------------------------------------*/
                  mm_initiate_lu_upon_entry_into_new_cell = TRUE;
                  
               }

               if ((msg_p->cmd.rr_abort_ind.RR_abort_cause == ACCESS_CLASS_NOT_ALLOWED) &&
                   (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU))
               {
                  mm_lu_reject_info.reject_cause = CAUSE_ACCESS_BARRED;
               }
               else if ((msg_p->cmd.rr_abort_ind.RR_abort_cause == RR_ABORT_LAI_CHANGED) &&
                        (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU))
               {
                 mm_lu_reject_info.reject_cause = CAUSE_LAI_CHANGED;
               }
               else if ((msg_p->cmd.rr_abort_ind.RR_abort_cause == RR_ABORT_CON_EST_NOT_ATTEMPTED ) &&
                        (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU))
               {
                   mm_lu_reject_info.reject_cause = CAUSE_CONN_EST_NOT_ATTEMPTED;
               }
               /* 
               **  Due to UOOS restrictions RR, when not able to camp back after conn rel, aborts any 
               **  pending RR_EST_REQ(RLF) after reporting NO_SRV.In such case, NAS needs to 
               **  ignore LU failure and retain LA info. 
               */               
               else if ((mm_location_information.location_update_status == UPDATED) &&
                         (msg_p->cmd.rr_abort_ind.RR_abort_cause == RR_CONNECTION_FAILURE) &&
                         (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU) &&
                         (mm_serving_plmn.info.service_status   == SYS_SRV_STATUS_NO_SRV))
               {
                 mm_lu_reject_info.reject_cause = CAUSE_LAI_CHANGED;                 
               }
#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE
               else if ((msg_p->cmd.rr_abort_ind.RR_abort_cause == RR_ABORT_GTOW_REDIRECTION_IN_PROGRESS) &&
                        (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU))
               {
                 mm_lu_reject_info.reject_cause = CAUSE_LAI_CHANGED;
                 mm_substate_control(MM_STD_UPDATE_PENDING);

#if defined(FEATURE_LTE)
                 /* 
                 *If LU is aborted in an updated Cell due to reject cause for RR_EST_REQ from RR
                 * then set the "mm_pending_lu_request" to "LU_ABORTED_IN_UPDATED_CELL".
                 * This is to do LU if blind redirection from G2L fails and UE camps on G again.
                 */
                 if((mm_location_information.location_update_status == UPDATED ) &&
                    (memcmp( mm_serving_lai,mm_location_information.lai,LAI_SIZE ) == 0 ) &&
                    (mm_possible_imsi_attach == FALSE))
                 {
                   MSG_HIGH_DS_0(MM_SUB, "=MM= Set mm_pending_lu_request to LU_ABORTED_IN_UPDATED_CELL");
                   mm_pending_lu_request = LU_ABORTED_IN_UPDATED_CELL;
                 }
#endif

               }
#endif

               //as part of undoing LU, ensure reg service domain is restored
               if (mm_lu_reject_info.reject_cause == CAUSE_LAI_CHANGED)
               {
                 if(mm_location_information.location_update_status == UPDATED)
                 {
                   mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                   SYS_SRV_DOMAIN_CS_ONLY,
                                                   FALSE,
                                                   SYS_SRV_STATUS_SRV,
                                                   FALSE );
                   (void) memscpy( mm_previous_serving_lai, LAI_SIZE,
                                  mm_location_information.lai,
                                  LAI_SIZE );
                 }
                 mm_previous_idle_substate = NULL;
               }
               if (mm_lu_reject_info.reject_cause == CAUSE_CONN_EST_NOT_ATTEMPTED ||
                   mm_lu_reject_info.reject_cause == CAUSE_ACCESS_BARRED)
               {
                 mm_previous_idle_substate = NULL;
               }
               if (msg_p->cmd.rr_abort_ind.RR_abort_cause == RR_ABORT_REDIAL_NOT_ALLOWED)
               {
                  mm_state_control( MM_STD_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED );
               }

               else if (msg_p->cmd.rr_abort_ind.RR_abort_cause == RR_ABORT_IA_REJECT_RECEIVED)
               {
                  if ( mm_state == MM_WAIT_FOR_RR_CONNECTION_LU )
                  {
                    /*
                    ** Spec Ref. 24.008 v3.j.0, sec - 4.4.4.9 (b).
                    ** RR will start T3122 timer and try to reselect to new Cell when IMM Assignment for LU is rejected.
                    ** After cell change occured or T3122 expiry RR should send service indication and MM should initiate LU.
                    ** Reject cause is set to prevent LU (due to timers T3211, T3212) until T3122 is running.
                    ** LU pending flag is marked, so that in next RR service indication MM must perform LU.
                    */
                    mm_lu_reject_info.reject_cause = CAUSE_IMM_ASSIGN_REJECT;
                    mm_pending_lu_request = T3122_EXPIRED;
                  }
                  mm_state_control( MM_STD_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED );
               }
               else
               {
                 if(msg_p->cmd.rr_abort_ind.RR_abort_cause == RR_ABORT_LAI_CHANGED)
                 {
                   mm_state_control( MM_STD_LOW_LEVEL_FAIL_LAI_CHANGE );
                 }
                 else
                 {
                   mm_state_control( MM_STD_LOW_LEVEL_FAIL );
                 }
               }
            }
            break;

         case RRC_ABORT_IND:

            /* -----------------------------------------------------
            ** Integrity protection on CS domain is no longer active
            ** ----------------------------------------------------- */
            mm_integrity_protected = FALSE;
    
            if ( msg_p->cmd.rrc_abort_ind.abort_cause != RRC_FAILURE_UNSPECIFIED &&
                 msg_p->cmd.rrc_abort_ind.abort_cause != RRC_FAILURE_DUE_TO_CONGESTION &&
#ifdef FEATURE_SILENT_REDIAL_AFTER_RA_LA_MISMATCH
                 msg_p->cmd.rrc_abort_ind.abort_cause != RRC_ABORT_LAI_RAI_CHANGED &&
#endif
                 mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
            {
               /*-----------------------------------------------
               **LU is failed because of d  or f of section 4.4.4.9 of 24.0.08
               **------------------------------------------------*/
               
               if (mm_serving_cell_valid)
               {
                   mm_initiate_lu_upon_entry_into_new_cell = TRUE;
                   mm_lu_failed_cell_id = mm_serving_cell_id;
               }
            }

            if (((msg_p->cmd.rrc_abort_ind.abort_cause == RRC_AC_NOT_ALLOWED_CS_DOMAIN )||
                 (msg_p->cmd.rrc_abort_ind.abort_cause == RRC_AC_NOT_ALLOWED_PS_DOMAIN ))&&
                 (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU))
            {
               mm_lu_reject_info.reject_cause = CAUSE_ACCESS_BARRED;
               mm_substate_control(MM_STD_UPDATE_PENDING);
               mm_previous_idle_substate = NULL;
               if((msg_p->cmd.rrc_abort_ind.abort_cause == RRC_AC_NOT_ALLOWED_PS_DOMAIN ) &&
                  (mm_check_if_cs_cell_access_allowed(cs_cell_access) == TRUE)&&
                  (mm_serving_plmn.network_op_mode != NETWORK_OP_MODE_1))
               {
                 init_lu_cs_access_allowed = TRUE;
               }
            }
#ifdef FEATURE_SILENT_REDIAL_AFTER_RA_LA_MISMATCH
            else if((msg_p->cmd.rrc_abort_ind.abort_cause == RRC_ABORT_LAI_RAI_CHANGED) && 
                    (mm_state == MM_WAIT_FOR_RR_CONNECTION_LU))
            {
               mm_lu_reject_info.reject_cause = CAUSE_LAI_CHANGED;

               /* as part of undoing LU, ensure reg service domain is restored */
               if(mm_location_information.location_update_status == UPDATED)
               {
                 mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                                 SYS_SRV_DOMAIN_CS_ONLY,
                                                 FALSE,
                                                 SYS_SRV_STATUS_SRV,
                                                 FALSE );
                 (void) memscpy( mm_previous_serving_lai, LAI_SIZE,
                                mm_location_information.lai,
                                LAI_SIZE );
               }
               mm_previous_idle_substate = NULL;
               mm_lu_pended_nmo_unknown = TRUE;
            }
#endif
#ifdef FEATURE_DUAL_SIM
            if (msg_p->cmd.rrc_abort_ind.abort_cause== RRC_NO_RESOURCE_AVAILABLE ||
                msg_p->cmd.rrc_abort_ind.abort_cause == RRC_ABORT_CONN_REJECT_WAIT_TMR)
            {
              if(mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
              {
                if(msg_p->cmd.rrc_abort_ind.abort_cause == RRC_ABORT_CONN_REJECT_WAIT_TMR)
                {
                  mm_lu_reject_info.mm_conn_rej_timer_value = msg_p->cmd.rrc_abort_ind.conn_rej_wait_timer;
                }
                else
                {
                  mm_lu_reject_info.mm_conn_rej_timer_value = 0;
                }
                mm_lu_reject_info.reject_cause = CAUSE_RADIO_UNAVAILABLE;
              }
              mm_state_control( MM_STD_RADIO_UNAVAILABLE );
            }
            else
#endif
            if (msg_p->cmd.rrc_abort_ind.abort_cause == RRC_MAX_ACCESS_FAILURE)
            {
               if(mm_state == MM_WAIT_FOR_RR_CONNECTION_LU)
               {
                 mm_lu_reject_info.reject_cause = CAUSE_RANDOM_ACCESS_FAILURE;
               }
               if(gmm_anite_gcf_flag)
               {
                 mm_state_control( MM_STD_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED );
               }
               else
               {
                 mm_state_control(MM_STD_LOW_LEVEL_FAIL);
               }
            }
            else if((msg_p->cmd.rrc_abort_ind.abort_cause == RRC_AC_NOT_ALLOWED_CS_DOMAIN )||
                     (msg_p->cmd.rrc_abort_ind.abort_cause == RRC_AC_NOT_ALLOWED_PS_DOMAIN ))
            {
              mm_update_cell_status(msg_p->cmd.rrc_abort_ind.abort_cause);

              if(msg_p->cmd.rrc_abort_ind.abort_cause == RRC_AC_NOT_ALLOWED_CS_DOMAIN )
              {
                 if((mm_held_cnm_service_req.present) &&
                  (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL)
                   &&(!mm_retry_em_call_on_access_blocked_cell))
                {
                  mm_state_control( MM_STD_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED );
                }
                else
                {
              
                 mm_state_control( MM_STD_LOW_LEVEL_FAIL );
                }
              }
              else
              {
                if ((mm_state == MM_WAIT_FOR_NETWORK_COMMAND) && (mm_last_paging_cause != 0xFF))
                {
                  mm_paging_response(msg_p);
                }
                else
                {
                  mm_state_control( MM_STD_LOW_LEVEL_FAIL );
                }
              }

              if (init_lu_cs_access_allowed)
              {
                   MSG_HIGH_DS_1(MM_SUB, "=MM= CS Registration started due to AC blocking for PS domain %d",
                                                                                               cs_cell_access);
                   mm_initiate_lu( MM_LU_CAUSE_NORMAL );
              }
            }
            else if (!((mm_held_cnm_service_req.present) && 
                       (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL) &&
                       ((mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_RESELECTION) || 
                        (mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_CC_ORDER))))
            {
#ifdef FEATURE_SILENT_REDIAL_AFTER_RA_LA_MISMATCH
               if (msg_p->cmd.rrc_abort_ind.abort_cause == RRC_ABORT_LAI_RAI_CHANGED)
               {
                 mm_reject_call_on_rai_change = TRUE;
                 /*Reject pended call if rai_lai changed and waitign for service indication*/
                 /*leave LU fail handling to legacy behaviour*/
                 if ((mm_held_cnm_service_req.present == TRUE) &&
                      (mm_held_cnm_service_req.establishment_cause != (int)MM_RR_EMERGENCY_CALL) &&
                       (mm_lu_reject_info.reject_cause != CAUSE_LAI_CHANGED))
                 {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                   /*Final argument is added as a default*/
                   mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL,MMCC_SMS_CS);
#else
                   mm_send_mmcnm_est_rej(MM_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL);
#endif
                 }
                 mm_state_control( MM_STD_LOW_LEVEL_FAIL_LAI_CHANGE );
               }
               else
#endif
               {
                 mm_state_control( MM_STD_LOW_LEVEL_FAIL );
               }
            }
            break;

         default:
            MSG_FATAL_DS( MM_SUB, "=MM= Unexpected message id %d", msg_p->cmd.hdr.message_id, 0,0 );
      } /* end switch (message_id) */
      mm_send_mmcc_sig_conn_rel_ind();
      mm_send_cm_cipher_ind(SYS_SRV_DOMAIN_CS_ONLY,FALSE);

   }

   mm_stop_timer( TIMER_T3230 );

   mm_stop_timer( TIMER_T3240 );

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
      mm_stop_timer( TIMER_T3241 );
#endif /* end of (FEATURE_MMGPS || FEATURE_CGPS_UMTS_CP) */

}
