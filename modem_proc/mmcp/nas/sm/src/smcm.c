/*===========================================================================
                       S M  C M   S O U R C E   F I L E 

DESCRIPTION

   This file contains the function definitions between the SM and CM entities.
   
EXTERNALIZED FUNCTIONS   

  sm_get_connection_id() : Gets the connection ID for the PDP transaction

  sm_unreserve_connection_id() : Unreserves the SM connection ID.

  sm_send_cm_confirm()   : Sends the confirmation notification to Call Manager

  sm_continue_pdp_modify :  Continues PDP Modification proceduces.

  sm_continue_pdp_activate :  Continues PDP activation procedures.
 
  sm_continue_pdp_deactivate :  Continues PDP deactivation procedures.

                  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2001, 2002, 2003-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/sm/vcs/smcm.c_v   1.16   18 Jul 2002 15:44:00   kishores  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/smcm.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/28/12   am      Fixing compilation warning 
10/09/12   am      Fixing compilation error
10/01/12   am      Back off timer code
07/31/12   us    NAS shall reject any PDP (primary or secondary) activation or modification request if the total length of the 
                 PDU to be sent OTA exceeds the MAX_OCTETS_L3_MESSAGE with a cause SM_MESSAGE_EXCEED_MAX_L2_LIMIT & also include
                 MAX_OCTETS_L3_MESSAGE in reject message to CM.
07/04/12   mv      Adding handling of graceful PDP abort for  PDP state PDP_INACTIVE
06/12/12   am      Fixing compilation warning
02/28/12   us      Removing third party ID references from F3 messages.
02/20/12   us      Fixing compilation wranings: "ps_sig_rel_cnf.as_id" uninitialized 
02/15/12   am       Changes for activating two same PDP address p-PDP simultaneously by UE.
09/30/11   am      Fix for sub 2 loses service when sub 1 is in data transfer
08/04/11   am      Adding range check on trans_id to avoid buffer overflow.
03/25/10   MNK     Featurize LTE<=>GW i-rat code
04/05/09   rk      Reset max_est_timer_count to 0 when unreserving the connection Id
                   or PDP deactivation is initiated before Signalling Connection is established.
08/29/07   ab     Now connection id will not be unreserved when CM call sm_unreserve_connection_id
                            for MT PS call when pdp state is PDP_INACTIVE
08/14/06   pk      Updated to log CFA packet for SM->CM message.
10/12/06   rk       Fixed to continue De-Activation Req after Inter-RAT. (W to G)
08/18/06   rk       Added Extended TI Feature Code
08/08/06   ss      Fixed compiler warning.
07/17/06   ss       Delaying Local deactivation for 2 secs to avoid RAB setup failure.
07/14/06   ss     Validating num_filters in TFT
07/10/06   rk      PDP Secondary Activation and Modify are not features of Release 97.
                          So in this cases a Reject msg is sent to cm.
07/03/06   ss      Added GPRS call control fixes
03/27/06   ss      Now sending SMREG_PDP_DEACTIVATE_CNF to clear the call at CM
                after sending REQUEST_PDP_ACTIVATION_REJ message
03/21/06   kvk     Fixed the dog error fatal by handling the critical section properly
03/09/06   kvk     Now con id will not be set to INVALID_ID if CM calls the function
                   sm_unreserve_connection_id() for MT PS calls.
03/08/06   ss       Now rejecting TFT deletion when already one PDP without TFT exists in the group.
02/16/06   ss       Now setting Teardown Indicator when Primary PDP is deactivated by keypress.
01/04/06   kvk     Now CM_PS_SIG_CON_REL_IND will be sent to CM after the last call is 
                   released to avoid CM to start 5 s back to back PS call blocking timer in case
                   if PS signalling connection is already released.
12/27/05   ss      Reject the secondary PDP activation if already a PDP exist in same group
                   which is without TFT.
11/11/05   ss      Release 5 SM PCO and TFT changes put in.
11/07/05   kvk     Now sending GMMSM_CLOSE_SESSION_REQ when PDP is locally deactivated
11/01/05   kvk     QoS validation merged back
09/28/05   kvk    The TI flag will be set only if all the PDPs in the group were
                  in PDP_ACTIVE state.
09/13/05   kvk     MO PDP Modify supported. Now handling the Application initiated
                   PDP Modification for QoS and TFT.
06/20/05   hj      Added support for new events EVENT_SMGMM_REQUEST_SENT, 
                   EVENT_SMGMM_REJECT_RECEIVED.
06/07/05   kvk     Now deactivating PDP contexts when entering LPM.
05/12/05   kvk     PDP is locally deactivated if SM gets deactivation request when
                   PDP is in PDP_ACTIVE_PENDING.
05/06/05   kvk     Featurized the CM dependent data structures for SecPDP. 
04/18/05   kvk     MT originate flag is always set when MO deactivation is triggered.
03/31/05   kvk     Now normal Deactivation procedure is applied if PDP is in PDP_ACTIVE_PENDING
                   rather than locally deactivating in case of deactivation triggered by CM.
10/21/04   kvk     MT PDP support added
10/20/04   kvk     Now posting RABM_MODIFY_IND to RABM only when RAB is in dormant
10/06/04   kvk     PDP address allocation type is stored in SM call object for
                   checking duplicate PDP contexts from CM.
                   RAB state is changed to RABM_MODIFY_PENDING when MO modification triggered.
09/15/04   kvk     Fixed handling of MO PDP modification.
09/02/04   kvk     Added code feature handling sec pdp. New interfaces with CM for
                   handling sec pdp context request, confirmation, promote of sc PDP. 
                   group_id is passed to CM when primray activated. Tear down indicator 
                   flag to bring all the PDPs in group is supported.
07/28/04   kvk     mt_orig_flag is cleared when SM indicates CM with SMREG_MODIFY_IND
07/13/04   kvk     RABM is notified with ESTABLISH_IND for every timeout
06/30/04   kvk     Added handling for the new primitive ps_sig_rel_ind
06/15/04   kvk     timer_est_pen_flag is reset when PDP context is deactivated abnormally.
                   RABM activation is notified in sm_pdp_activate rather than sm_pdp_continue_act()
                   to avoid multiple notification. Introduced new function SM_CHECK_MPDP_CONN() 
                   to check for pending GMM connections.                   
06/08/04   kvk     Reset the pdp_con_state along with pdp state in sm_get_connection_id()
06/03/04   ks      Added SNDCP h_comp and d_comp params to PDP context info.
06/03/04   kvk     Added support for timer handling for MPDP, new timer interface for 
                   SM timers and support for Duplicate PDP context detection.
02/28/04   ks      Added support for the new PDP connection state pdp_con_state
12/04/03   ks      Added support for TIMER_ESTABLISH_PENDING timer for GMM
                   connection establishment.
10/23/03   mks     QoS validation is avoided if the PDP is already in INACTIVE or
                   INACTIVE_PENDING state. 
08/12/03   ks      Always send a signalling connection establish request to GMM
                   whenever SM initiates PDP context activation, modification or
                   deactivation procedures to avoid SM & GMM going out of sync.
                   If GMM already has a connection it will respond immediately 
                   with a CNF otherwise it would setup a new connection.
08/05/03   mks     Added support for validating the network offered QoS against
                   the minimum QoS requirements of the MS during MO PDP activation
                   and MT PDP modification procedures. If the QoS validation fails,
                   the context deactivation is initiated.
07/15/03   ks      Fixed two MSG-HIGH debug statements.
03/18/03   mks     Added support for PS continuity feature: Corrected the PDP
                   context activation behavior to set the LLC SAPI to "LLC 
                   SAPI not assigned" if MS supports WCDMA mode only. A valid 
                   LLC SAPI is used if MS supports both WCDMA & GPRS.
02/24/03   ks      Added new function sm_unreserve_connection_id() to unreserve
                   the connection_id for Call Manager. Cleaned LINT errors.
01/16/03   ks      Replaced mm_abort_rr() with send_gmm_close_session_req()
12/17/02   ks      Passing the PS domain ID in the mm_abort_rr function.
12/02/02   ks      Indication RABM about PDP context Action, Modification and
                   Deactivation only if the UE is operating in UMTS mode.
10/07/02   ks      Setting the LLC_SAPI field based on the current operation
                   mode, GPRS or UMTS.
06/13/02   ks      Setting max_timer_count to '0' at the time of sending
                   PDP procedure confirmation to CM.
06/13/02   ks      Now aborting a pending PDP context activation request.
04/22/02   ks      Corrected the TI flag indication in Mobile Originated calls.
                   Added checking for PDP states during Deactivation request.
03/11/02   ks      Fixed the command id in the CM command header while sending
                   CM_PDP_MODIFY_IND and CM_PDP_ACTIVATE_REJ.
02/11/02   ks      Setting the correct connection_id in the SM-CM primitive
                   during DEACTIVATE_IND and DEACTIVATE_CNF to the CM.
11/20/01   ks      Added interfaces to send Call manager commands to CM.
11/05/01   ks      Fixed the MSG_HIGH string length.
08/24/01   ks      Added calls to sm_log_pdp_info() & sm_log_qos_info() in 
                   sm_send_cm_confirm, for logging PS info to QXDM/QCAT.
08/01/01   ks      Checking sig_connection_available variable for availability
                   of RRC signaling connection before sending L3 NAS messages.
06/04/01   ks      Locking the critical section in the sm_get_connection_id().
05/23/01   ks      Changed the con_id formal parameter in various functions 
                   from word to byte. Added/fixed various debug statments.
05/17/01   ks      Added primitive SMREG_PDP_DEACTIVATE_REJ to reject a 
                   deactivate request in sm_send_cm_confirm() function.
04/30/01   ks      Added a function sm_pdp_activate_rej_rsp() to process the
                   CM request to send a reject to Mobile terminated PDP req.
01/15/01   ks      Created file.

===========================================================================*/

/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
//#include "sm.h"
/* ----------------------------------------------------------------------- 
** REMOVED compiler warning from inclusion of memheap.h 
** ----------------------------------------------------------------------- */
#include "mmcp_variation.h"
#ifdef FEATURE_MEM_DEBUG
#undef FEATURE_MEM_DEBUG
#endif
#include "cmwll.h"
#include "sm_cm.h"
#include "smix_v.h"
#include "smtask.h"
#include "smtask_v.h"
#include "rabmtask_v.h"
#include "gs_v.h"
#include "memory.h"
#include "string.h"
#include "smtimers.h"
#include "sm_v.h"
#include "mm_v.h"
#include "gmm_int_v.h"
#ifdef FEATURE_GSM_GPRS
#include "gsndcp.h"
#include "gsmsnif.h"
#endif
#include "cfa_v.h"
#ifdef FEATURE_MBMS
#error code not present
#endif
#ifdef FEATURE_LTE
#include "sm_multimode_common.h"
#endif
#include "ds3gpp_api.h"
#include <stringl/stringl.h>

#ifdef FEATURE_DUAL_SIM
#define is_umts_mode() is_ps_umts_mode((sys_modem_as_id_e_type)sm_as_id)
#define is_gsm_mode()  is_ps_gsm_mode((sys_modem_as_id_e_type)sm_as_id)
#define is_wcdma_mode() is_ps_wcdma_mode((sys_modem_as_id_e_type)sm_as_id)
#define is_tdscdma_mode() is_ps_tdscdma_mode((sys_modem_as_id_e_type)sm_as_id) 
#else
#define is_umts_mode() is_ps_umts_mode(0)
#define is_gsm_mode()  is_ps_gsm_mode(0)
#define is_wcdma_mode() is_ps_wcdma_mode(0)
#define is_tdscdma_mode()  is_ps_tdscdma_mode(0)
#endif

extern boolean is_ps_wcdma_mode( sys_modem_as_id_e_type as_id );
extern boolean is_ps_umts_mode(sys_modem_as_id_e_type as_id);
extern boolean is_ps_gsm_mode( sys_modem_as_id_e_type as_id );
extern boolean is_ps_tdscdma_mode( sys_modem_as_id_e_type as_id );

#ifdef FEATURE_DUAL_SIM
extern boolean SM_IS_VALID_AS_ID(sys_modem_as_id_e_type cmd_as_id);
#endif

#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* TEST_FRAMEWORK */
rex_tcb_type *my_tcb_ptr;


#if (defined FEATURE_DUAL_SIM ) || defined(FEATURE_SGLTE)
extern boolean mm_dual_standby_mode;
extern boolean mm_sglte_mode;
#endif

byte nas_negotiated_ota_len = (byte)MAX_OCTETS_L3_MESSAGE;

extern void sm_unset_ps_vote_for_reselection(void);

#ifdef FEATURE_DUAL_DATA
extern boolean sm_ps_vote_for_reselection_sim[MAX_AS_IDS];
#define sm_ps_vote_for_reselection sm_ps_vote_for_reselection_sim[sm_as_id]

extern sm_blocked_apn_list_T sm_blocked_apn_list_sim[MAX_AS_IDS];
#define sm_blocked_apn_list sm_blocked_apn_list_sim[sm_as_id]

#else
extern sm_blocked_apn_list_T sm_blocked_apn_list;
#endif 



/* ========================================================================
** LOCAL FUNCTION PROTOTYPES.
** These are stitic functions and are visible only from this file scope.
** ====================================================================== */

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PDP_ACTIVATE()
** DESCRIPTION  : This function processes PDP activation request by CM.
** ----------------------------------------------------------------------- */
LOCAL void sm_pdp_activate (byte connection_id, 
                              smreg_pdp_activate_req_T *pdp_act);

#ifdef FEATURE_MBMS
#error code not present
#endif


/* -----------------------------------------------------------------------
** FUNCTION     : SM_PDP_DEACTIVATE()
** DESCRIPTION  : This function processes PDP deactivation request by CM.
** ----------------------------------------------------------------------- */
LOCAL void sm_pdp_deactivate (byte con_id, 
                               smreg_pdp_deactivate_req_T *pdp_deactivate);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PDP_MODIFY()
** DESCRIPTION  : This function processes PDP modify request by CM.
** ----------------------------------------------------------------------- */
LOCAL void sm_pdp_modify (byte con_id, smreg_pdp_modify_req_T *pdp_modify);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_SEC_PDP_ACTIVATE()
** DESCRIPTION  : Processes Secondary PDP activate request by CM.
** ----------------------------------------------------------------------- */
LOCAL void sm_sec_pdp_activate(byte con_id, 
                                smreg_pdp_activate_sec_req_T* sec_pdp_req);

/* -----------------------------------------------------------------------
** FUNCTION     : sm_check_mpdp_conn_pend()
** DESCRIPTION  : Checks the pending GMM connections
** ----------------------------------------------------------------------- */
LOCAL boolean sm_check_mpdp_conn_pend(void);

#ifdef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
/* -----------------------------------------------------------------------
** FUNCTION     : SM_GET_LINKED_TI()
** DESCRIPTION  : Gets the group id sharing same APN and PDP address
** ----------------------------------------------------------------------- */
LOCAL byte sm_get_linked_ti(byte group_id);
#else
/* -----------------------------------------------------------------------
** FUNCTION     : SM_GET_PRIMARY_INDEX()
** DESCRIPTION  : Gets the group id of primary PDP
** ----------------------------------------------------------------------- */
LOCAL byte sm_get_primary_index(byte group_id);
#endif

/* -----------------------------------------------------------------------
** FUNCTION     : SM_SEND_SNDCP_CMD

** DESCRIPTION  : This function sends command to SNDCP
** ----------------------------------------------------------------------- */
#ifdef FEATURE_GSM_GPRS
extern void sm_send_sndcp_cmd( pdp_context_info *cur_pdp, gsnsm_prim_type sndcp_cmd);
#endif /*FEATURE_GSM_GPRS*/

/* -------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
** FUNCTION     : SM_IF_TFT_EXIST()
** DESCRIPTION  :This function checks if already existing PDPs in the group has TFT
** ----------------------------------------------------------------------- */
LOCAL boolean sm_if_tft_exist(byte group_id);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PS_SIGNALING_REL_REQ()
** DESCRIPTION  : Processes PS signaling release request by CM.
** ----------------------------------------------------------------------- */
LOCAL void sm_ps_signaling_rel_req
(
#ifdef FEATURE_DUAL_SIM
  smreg_ps_signalling_rel_req_cause_T cause
#else
    void
#endif
);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_GRACEFUL_PDP_ABORT_REQ()
** DESCRIPTION  : Deactivate PDPs gracefully on receiving abort request
** ----------------------------------------------------------------------- */
LOCAL void sm_graceful_pdp_abort_req(void);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_SEND_CM_GRACEFUL_ABORT_CNF()
** DESCRIPTION  : Send abort cnf for CM
** ----------------------------------------------------------------------- */
void sm_send_cm_graceful_abort_cnf(void);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_SEND_CM_GRACEFUL_ABORT_DUMMY_CNF()
** DESCRIPTION  : Send abort cnf for CM
** ----------------------------------------------------------------------- */
void sm_send_cm_graceful_abort_dummy_cnf(sys_modem_as_id_e_type as_id);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PDP_MODIFY()
** DESCRIPTION  : This function processes PDP modify request by CM.
** ----------------------------------------------------------------------- */
LOCAL void sm_pdp_nw_modify_cnf(byte con_id);

/* -----------------------------------------------------------------------
** FUNCTION     : SM_PDP_NW_MODIFY_REJ()
** DESCRIPTION  : This function processes PDP activation request by CM.
** ----------------------------------------------------------------------- */
LOCAL void sm_pdp_nw_modify_rej (byte connection_id, 
                              smreg_pdp_nw_modify_rej_T *pdp_nw_mod_rej);


#ifdef FEATURE_LTE
extern boolean sm_check_if_active_pdp_exists(void);
#endif

#ifdef FEATURE_MMAL
extern void sm_fill_and_send_pdp_context_log_info(const pdp_context_info *cur_pdp);
#endif

#ifdef FEATURE_DUAL_SIM
extern void send_gmm_ok_to_tuneaway_ind(void);
#endif

void sm_process_blocked_apn_list(smreg_blocked_apn_list_ind_T *blocked_apn_list_ind);

/*===========================================================================
** FUNCTION DEFINITIONS 
** =========================================================================*/


/*===========================================================================

FUNCTION SM_GET_CONNECTION_ID_INT

DESCRIPTION
  This function is called by the SM in order reserve connection ID for MT PS call
 
DEPENDENCIES
   To be called only by SM to reserve connection ID for MT call
   
RETURN VALUE

  Returns TRUE if a connection Id is found, otherwise returns FALSE.

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean sm_get_connection_id_int(byte  connection_id)
{
  boolean pdp_entry_found;  


  pdp_entry_found = FALSE;  /* Set default value to FALSE */

  /* ----------------------------------------------------------------------
  **  Enter the Critical Section.
  ** --------------------------------------------------------------------*/
  rex_enter_crit_sect(&sm_crit_sect); 

  if (pdp_list[connection_id] == NULL)
  {
    #ifdef FEATURE_MODEM_HEAP      
    pdp_list[connection_id] = (pdp_context_info*) modem_mem_calloc( 1, sizeof(pdp_context_info), MODEM_MEM_CLIENT_NAS);
    #else
    pdp_list[connection_id] = &pdp_list_static[connection_id];
    #endif
    pdp_entry_found = TRUE;            
  }
  

  if (pdp_entry_found)
  {
    sm_init_pdp_context(pdp_list[connection_id]);
    pdp_list[connection_id]->trans_id = connection_id;
    // *connection_id = (byte)pdp_list[connection_id]->trans_id;
    MSG_HIGH_DS_1(SM_SUB,"Return con_id (TI = %d) to SM", connection_id);      
  }
  
  /* ---------------------------------------------------------------------
  ** Leave the Critical Section 
  ** -------------------------------------------------------------------*/
  rex_leave_crit_sect(&sm_crit_sect);

  return pdp_entry_found;   
}

/*===========================================================================
FUNCTION SM_UNRESERVE_CONNECTION_ID_INT

DESCRIPTION
  This function is called by the SM in order to unreserve a 
  previously secured SM connection ID.

DEPENDENCIES
   TO be used only by SM
  
RETURN VALUE

  Returns TRUE if a connection Id is unreserved, otherwise returns FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

boolean sm_unreserve_connection_id_int(byte connection_id)
{
   boolean                   pdp_entry_found = FALSE;
   //word                      i; 

   /* ----------------------------------------------------------------------
   **  Enter the Critical Section.
   ** --------------------------------------------------------------------*/
   rex_enter_crit_sect(&sm_crit_sect);

   if (pdp_list[connection_id] != NULL)
   {
     // Set the requested transaction Id to INVALID
     MSG_HIGH_DS_1(SM_SUB,"Unreserving SM CON_ID = %d", connection_id);       
     pdp_entry_found = TRUE;
#ifdef FEATURE_MODEM_HEAP       
     modem_mem_free(pdp_list[connection_id], MODEM_MEM_CLIENT_NAS);
#endif
      pdp_list[connection_id] = NULL;            
   }
   /* ----------------------------------------------------------------------
   **  Leave the Critical Section.
   ** --------------------------------------------------------------------*/
   rex_leave_crit_sect(&sm_crit_sect);
   
   if (pdp_entry_found == TRUE)
   {
     #ifndef FEATURE_GPRS_CALLCONTROL
     (void) sm_release_nsapi_sm( connection_id );
     #endif

     //If no signalling connection is pending clear the PDP timer
     if (!sm_check_mpdp_conn_pend())
     {
      #ifndef FEATURE_DUAL_DATA
      sm_clear_pdp_timer (TIMER_ESTABLISH_PENDING);
      #endif
      timer_est_pen_flag = FALSE;
      max_est_timer_count = 0;
     }
     sm_stop_timer (connection_id);
   }
   
#ifdef FEATURE_GSM_GPRS      
   if(pending_sm_pdu_id == connection_id)
   {
     pending_sm_pdu_id = INVALID_ID;
   }
#endif   
   return pdp_entry_found;

}  /* End of function sm_unreserve_connection_id() */

/*===========================================================================

FUNCTION SM_PROCESS_CM_CMD

DESCRIPTION

  This function processes the command received from the CM task. 
   
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

  None.
  
===========================================================================*/
void sm_process_cm_cmd(sm_cmd_type* cmd_ptr)
{
  // Process the Command sent by CM task 
  MSG_MED_DS_2( SM_SUB,"CM Command received for Connection Id:%d CMD_ID:0x%X", 
              cmd_ptr->header.connection_id, cmd_ptr->header.cmd_id );

  switch (cmd_ptr->header.cmd_id)
  {
    case SMREG_PDP_ACTIVATE_REQ :
      
      sm_pdp_activate(cmd_ptr->header.connection_id, 
                      &cmd_ptr->cmd.pdp_activate_req);
      break;

    case SMREG_PDP_DEACTIVATE_REQ:
      
      sm_pdp_deactivate (cmd_ptr->header.connection_id, 
                         &cmd_ptr->cmd.pdp_deactivate_req);
      break;

    case SMREG_PDP_MODIFY_REQ:
      
      sm_pdp_modify (cmd_ptr->header.connection_id,
                     &cmd_ptr->cmd.pdp_modify_req);
      break;

    case SMREG_PDP_ACTIVATE_SEC_REQ:
      
      sm_sec_pdp_activate (cmd_ptr->header.connection_id, 
                           &cmd_ptr->cmd.pdp_sec_activate_req);
      break;

    case SMREG_PDP_ACTIVATE_REJ_RSP:
      
      sm_pdp_activate_rej_rsp (cmd_ptr->header.connection_id, 
                           &cmd_ptr->cmd.pdp_activate_rej_resp);
      break;

    case SMREG_PDP_ABORT_REQ:
     
     #ifdef FEATURE_DUAL_SIM
       if (!(SM_IS_AS_ID_IN_VALID_RANGE(cmd_ptr->cmd.pdp_abort_req.as_id) == TRUE && 
            (SM_IS_VALID_AS_ID(cmd_ptr->cmd.pdp_abort_req.as_id) == TRUE)))
       {
          MSG_HIGH_DS_2( SM_SUB,"Invalid AS ID in sm_as_id : %d SMREG_PDP_ABORT_REQ : %d", sm_as_id, cmd_ptr->cmd.pdp_abort_req.as_id);
          // NSAPI Clear logic is moved from CM to NAS when PDP Abort is sent
          // This code is to clear nsapi List for calls pended at CM but not reached till NAS.
          // PDP Abort Req on sub where PDP objects were there (at NAS) whould have received earlier.
          sm_release_all_nsapi(); 
          return;
       }
      #endif
      sm_perform_local_deactivation((byte)PPD_UNKNOWN_REASON);

      // NSAPI Clear logic is moved from CM to NAS when PDP Abort is sent
      sm_release_all_nsapi(); 
      break;


    case SMREG_PS_SIGNALING_REL_REQ:
      #ifdef FEATURE_DUAL_SIM
      MSG_MED_DS_1( SM_SUB,"Recd CM PS Signaling Release Request: %d", cmd_ptr->cmd.ps_signalling_rel_req.cause);    
      if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.ps_signalling_rel_req.as_id) == FALSE)
      { 
         MSG_ERROR_DS_2( SM_SUB,"SM AS ID Mismatch sm_as_id : %d, message as_id: %d", sm_as_id , cmd_ptr->cmd.ps_signalling_rel_req.as_id);
         return;
      }
      sm_ps_signaling_rel_req(cmd_ptr->cmd.ps_signalling_rel_req.cause);
      #else
        sm_ps_signaling_rel_req();
      #endif
        break;

    case SMREG_PDP_GRACEFUL_ABORT_REQ:
       #ifdef FEATURE_DUAL_SIM
       if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.pdp_graceful_abort_req.as_id) == FALSE) 
       { 
         MSG_HIGH_DS_2( SM_SUB,"SM AS ID Conflict sm_as_id : %d, message as_id: %d", sm_as_id , cmd_ptr->cmd.pdp_graceful_abort_req.as_id);
         /* Currently CM sends SMREG_PDP_GRACEFUL_ABORT_REQ even on the CS only SUB when NV : NV_PDP_DEACTIVATE_BEFORE_DETACH_SUPPORTED_I is enabled
           * and MMOC timer expires if there is no CNF in cases of sm_as_id mismatch, and CM can't stop sending this on CS SUB due to legacy design
           * decided to send a dummy cnf to CM to address CM issue as mitigation 
          */
         sm_send_cm_graceful_abort_dummy_cnf(cmd_ptr->cmd.pdp_graceful_abort_req.as_id);
         return ;
       }
      #endif
      sm_graceful_pdp_abort_req();
      break;

#ifdef FEATURE_MBMS
#error code not present
#endif


    case SMREG_PDP_NW_MODIFY_CNF:

#ifdef FEATURE_DUAL_SIM
      if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.pdp_nw_modify_cnf.as_id) == FALSE)
      { 
         MSG_HIGH_DS_2( SM_SUB,"SM AS ID Conflict sm_as_id : %d, message as_id: %d", sm_as_id, cmd_ptr->cmd.pdp_nw_modify_cnf.as_id);
         return;   
      }
#endif
      sm_pdp_nw_modify_cnf(cmd_ptr->header.connection_id);
      break;
 
    case SMREG_PDP_NW_MODIFY_REJ:

#ifdef FEATURE_DUAL_SIM
      if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.pdp_nw_modify_rej.as_id) == FALSE)  
      { 
         MSG_HIGH_DS_2( SM_SUB,"SM AS ID Conflict sm_as_id : %d, message as_id: %d", sm_as_id, cmd_ptr->cmd.pdp_nw_modify_rej.as_id);
         return;  
      }
#endif
      sm_pdp_nw_modify_rej(cmd_ptr->header.connection_id, 
                                           &cmd_ptr->cmd.pdp_nw_modify_rej);
      break;

    case SMREG_BLOCKED_APN_LIST:

      sm_process_blocked_apn_list(&(cmd_ptr->cmd.blocked_apn_list_ind));
    break;

    default:
      MSG_ERROR_DS_1(SM_SUB," UNKNOWN cmd from CM : %d", cmd_ptr->header.cmd_id);
      break;
  };
}

/*===========================================================================
FUNCTION SM_PDP_ACTIVATE

DESCRIPTION
  This function processes the PDP context activate request. 
  
DEPENDENCIES
  It is assumed that the Call Manager provides correct connection Id.
  
RETURN VALUE

SIDE EFFECTS

  This function needs to make sure that the GPRS attach is already in effect
  before attempting to send the L3 message to the network.
===========================================================================*/
void sm_pdp_activate 
(
  byte con_id,
  smreg_pdp_activate_req_T *pdp_act
)
{
  pdp_context_info *cur_pdp; // current pdp context in the transaction
  byte nsapi;
#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
  word index;
#endif
#endif
  
  //Set the Signalling connection to FALSE
  sig_connection_available = FALSE; 

#ifdef FEATURE_DUAL_SIM
  #if defined(FEATURE_DUAL_DATA)
      if (SM_IS_AS_ID_IN_VALID_RANGE(pdp_act->as_id) == FALSE)
      {
          MSG_ERROR_DS_1( SM_SUB,"AS ID not valid : %d, sending REJ to CM", pdp_act->as_id );
          sm_send_cm_confirm_ti_cause (con_id, SM_NO_RADIO_AVAILABLE, SMREG_PDP_ACTIVATE_REJ);
          return ;
      }
  #else
     if (sm_as_id == SYS_MODEM_AS_ID_NONE )
     {
       sm_as_id = pdp_act->as_id;
     }
     else if (sm_as_id != SYS_MODEM_AS_ID_NONE && sm_as_id != pdp_act->as_id)
     {
       MSG_ERROR_DS_2( SM_SUB,"SM AS ID Mismatch sm_as_id : %d, message as_id: %d", sm_as_id , pdp_act->as_id);
       // check all the PDP contexts, if there exists another PDP different for the current one
       for ( index = 0 ; index < MAX_PDP_NUM ; index++)
       {
         if (pdp_list[index] != NULL && index != con_id)
         {
          break;
         }
       }
      // Another PDP exits (index) on one Subscription and con_id PDP is requested on a different Subscription.
      // So reject the Request.
      if (index != MAX_PDP_NUM)
      {
        sm_send_cm_confirm_ti_cause (con_id, SM_NO_RADIO_AVAILABLE, SMREG_PDP_ACTIVATE_REJ);
        return ;
      }
      // No PDP exists other than con_id. Reset SM AS ID with current one and proceed with Activation
      sm_as_id = pdp_act->as_id;
    }
  #endif
#endif

  /* ----------------------------------------------------------------------
  ** Reject if the PDP context is already active for the given con_id.
  ** Also the internal reject cause is set properly 
  ** --------------------------------------------------------------------*/
  if(pdp_list[con_id] == NULL && sm_get_connection_id_int (con_id) == FALSE )
  {
    MSG_ERROR_DS_1( SM_SUB,"PDP is NULL, Connection id %d is not reserved", con_id);
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm_ti_cause (con_id, INVALID_CONNECTIN_ID, SMREG_PDP_ACTIVATE_REJ);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
  }
  cur_pdp = pdp_list[con_id];   

  if (   (con_id > MAX_MO_TI && con_id < MIN_MT_TI)     ||
       con_id > MAX_MT_TI                             ||
       (con_id >= MIN_MT_TI && !cur_pdp->mt_orig_type)  ||
       cur_pdp->pdp_state != PDP_INACTIVE)
  {
    MSG_ERROR_DS_0( SM_SUB,"PDP already active, Reject the ACTIVATE req");
    cur_pdp->current_cause = (cc_cause_T)INVALID_CONNECTIN_ID;
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
  }
  else
  {
    /* -----------------------------------------------------------------------
    ** Set the PDP parameters for the PDP context based on the Connection Id.
    ** ---------------------------------------------------------------------*/

    if (con_id < MIN_MT_TI)// this is a mobile initiated PDP activation
#ifdef FEATURE_GPRS_CALLCONTROL
      nsapi = pdp_act->nsapi.nsapi;
#else
      nsapi = sm_per_subs_reserve_nsapi (con_id, (sys_modem_as_id_e_type) sm_as_id);
#endif
    else if (cur_pdp->nsapi.valid)
      nsapi = cur_pdp->nsapi.nsapi;
    else
      nsapi = INVALID_ID;
    
  

    //set the pdp as primary PDP context
    cur_pdp->is_primary_pdp = TRUE;

    // Set the RABM response state for this PDP context to RABM_INITIAL.
    cur_pdp->rab_state = RABM_INITIAL;

    /* set the current pending CM message */
    cur_pdp->cm_pending_msg = (int)SMREG_PDP_ACTIVATE_REQ;

    //Generate group id
    cur_pdp->group_id = sm_generate_group_id();

    /* set the NSAPI for this PDP */
    if (nsapi == INVALID_ID)
    {
      cur_pdp->nsapi.valid = FALSE;
    }
    else
    {
      cur_pdp->nsapi.valid = TRUE;
      cur_pdp->nsapi.nsapi = nsapi;
    }

    /* Set the LLC-SAPI to one of the allowed LLC-SAPI for SNDCP 3,5,9,11 */
    cur_pdp->llc_sapi.valid = TRUE;

    /* Set the QoS valid flag to be TRUE */
    cur_pdp->qos_valid = TRUE;

#ifdef FEATURE_LTE
    if(esm_isr_status_get() == TRUE)
    {
      cur_pdp->isr_pdp_act_status = CTXT_ACT_AFTER_ISR_IN_GW;
    }
    else
    {
      cur_pdp->isr_pdp_act_status = CTXT_ACT_BEFORE_ISR;
    }
#endif


    /* ----------------------------------------------------------------------
    ** 6.1.3.1.1 of TS 24.008:  An MS, which is capable of operating in both 
    ** GSM & UMTS, shall use a valid LLC SAPI, while an MS which is capable 
    ** of operating only in UMTS shall indicate the LLC SAPI as "LLC SAPI not 
    ** assigned" in order to avoid unnecessary value range checking and any 
    ** other possible confusion in the network.
    ** ----------------------------------------------------------------------*/

#if defined(FEATURE_GSM_GPRS) 
#ifdef FEATURE_GPRS_CALLCONTROL
     cur_pdp->llc_sapi.sapi = pdp_act->llc_sapi.sapi;
#else
    cur_pdp->llc_sapi.sapi = LLC_SAPI_3;
#endif
#else 
    cur_pdp->llc_sapi.sapi = LLC_SAPI_UNASSIGNED;
#endif

    /* Set the transaction Id same as the Connection Id */
    cur_pdp->trans_id = con_id;

    /* Set the APN Name */
    cur_pdp->apn_name = pdp_act->apn_name;

    /* Set the PDP address */
    cur_pdp->pdp_addr = pdp_act->pdp_addr;

    /*Set the PDP addr alloc type*/
/*10.5.6.4 of 24.008
Length of PDP address contents (octet 2)
If the value of octet 2 equals 0000 0010, then:
- No PDP address is included in this information element; and
- If the PDP type is IP, dynamic addressing is applicable.*/
    cur_pdp->pdp_addr_alloc_type = (cur_pdp->pdp_addr.pdp_addr_len )
                                   ? PDP_ADDR_STATIC : PDP_ADDR_DYNAMIC;

    /* Set the QOS for the PDP context */
    cur_pdp->qos = pdp_act->qos;

    /* Set the Minimum required QOS for the PDP context */
    cur_pdp->minimum_qos = pdp_act->minimum_qos;

    /* Set the Configuration Options */
    cur_pdp->config_options = pdp_act->config_options;
    /* Set the Compression Requirements */
    cur_pdp->h_comp = pdp_act->h_comp;
    cur_pdp->d_comp = pdp_act->d_comp;

    cur_pdp->context_id = pdp_act->context_id;
    cur_pdp->assoc_context_id = 0xFF;

    /* Set the pdp request type */
    cur_pdp->request_type = pdp_act->request_type;

    MSG_HIGH_DS_2(SM_SUB,"CM PDP Activate Request for NSAPI = %d; Received Ctxt id = %d",nsapi,pdp_act->context_id);

    if ( cur_pdp->nsapi.valid == FALSE )
    {
      //If NSAPIs exceeds max then reject the PDP context
      MSG_ERROR_DS_0( SM_SUB,"NSAPI not available, Reject the ACTIVATE req");
      cur_pdp->current_cause = (cc_cause_T)INVALID_NSAPI;
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
      sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
    }
    /* As per 23.060 Annex A.1,An MS may have one or two subscription records 
    *  with the same PDP type andthe same APN: one with a static PDP address, 
    *  one with a dynamic PDP address. 
    */
    else if (!ds3gpp_per_subs_get_config_multi_pdn_same_apn((sys_modem_as_id_e_type)sm_as_id) && (sm_check_pri_duplicate_pdp(cur_pdp) != INVALID_ID))
    {
      MSG_HIGH_DS_1(SM_SUB,"PDP act Rej, Duplicate context, NSAPI = %d", nsapi);
      cur_pdp->current_cause = (cc_cause_T)PDP_DUPLICATE;
      //Reject the PDP context as duplicate context is detected
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
      sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
    }
    else if(sm_is_apn_blocked(cur_pdp->apn_name))
    {
      MSG_HIGH_DS_0(SM_SUB,"PDP act Rej, APN blocked");
      cur_pdp->current_cause = (cc_cause_T)SM_APN_BLOCKED;
      //Reject the PDP context as duplicate context is detected
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
      sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
     }
    else
    {
      if (is_umts_mode())
      {
        /* Inform RABM that the PDP activation is in progress */
        MSG_LOW_DS_0( SM_SUB,"Inform RABM about PDP activation");
        cur_pdp->rab_state = RABM_ACTIVE_PENDING;
        sm_notify_rabm( (int)RABMSM_ACTIVATE_IND, cur_pdp->nsapi.nsapi );
      }

#ifdef FEATURE_DUAL_SIM
      cur_pdp->as_id = pdp_act->as_id ;
#endif

      /* ----------------------------------------------------------------------
      ** Call sm_continue_pdp_activation function, this function will send the
      ** Signaling message to GMM if the Signaling connection is available. 
      ** Otherwise an ESTABLISH_REQ is sent to GMM for a signaling connection.
      ** ---------------------------------------------------------------------*/
      sm_continue_pdp_activation(cur_pdp);
    }
    
  }
}

/*===========================================================================
FUNCTION SM_PDP_DEACTIVATE

DESCRIPTION
  This function processes the PDP context deactivate request. 
  
DEPENDENCIES
  It is assumed that the Call Manager provides correct connection Id.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_pdp_deactivate 
(
  byte con_id,
  smreg_pdp_deactivate_req_T *pdp_deactivate
)
{
  /* current pdp context in the transaction */
  pdp_context_info *cur_pdp; 
  cur_pdp = pdp_list[con_id];
  
  if(cur_pdp == NULL)
  {
    MSG_ERROR_DS_1( SM_SUB,"PDP is NULL, Connection id %d is unreserved", con_id);
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm_ti_cause (con_id, INVALID_CONNECTIN_ID, SMREG_PDP_DEACTIVATE_REJ);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
    return;
  }

#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
  if (SM_IS_VALID_AS_ID(pdp_deactivate->as_id) == FALSE)
  {
    MSG_ERROR_DS_2( SM_SUB,"SM AS ID Mismatch sm_as_id : %d, message as_id: %d", sm_as_id , pdp_deactivate->as_id);
    cur_pdp->current_cause = (cc_cause_T)SM_NO_RADIO_AVAILABLE;
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_REJ, SM_INTERNAL_CAUSE);
    return ;
  }
#endif
#endif

#ifdef FEATURE_MBMS
  #error code not present
#endif

  /* set the current pending CM message */
  cur_pdp->cm_pending_msg = (int)SMREG_PDP_DEACTIVATE_REQ;

  /*Set the MT orig flag to false*/
  cur_pdp->mt_orig_type = FALSE;

  // Set the RABM response state for this PDP context to RABM_INITIAL
  cur_pdp->rab_state = RABM_INITIAL;

  
  if (cur_pdp->trans_id == INVALID_ID)
  {
    cur_pdp->trans_id = con_id;
    MSG_ERROR_DS_1( SM_SUB, "PDP %d already Deactivated", cur_pdp->trans_id);
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
    return;
  }
#ifdef FEATURE_MBMS
  #error code not present
#endif

#ifdef FEATURE_GSM_GPRS      
  /* If PDU is pending for processing for this PDP context, clear PDU on receiving 
  deactivate message*/
  if(pending_sm_pdu_id == cur_pdp->trans_id)
  {
    pending_sm_pdu_id = INVALID_ID;
  }
#endif

  if ((cur_pdp->pdp_state == PDP_ACTIVE) || 
      (cur_pdp->pdp_state == PDP_MODIFY_PENDING))
  {
    sig_connection_available = FALSE; //Set the Signalling connection to FALSE

    /*Stop the timer in case any timer is running*/
    sm_stop_timer(cur_pdp->trans_id);

    // Set the cause of the deactivation in the PDP context
    cur_pdp->current_cause = (cc_cause_T)pdp_deactivate->cause;

#ifdef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
    cur_pdp->teardown_ind.valid = pdp_deactivate->tear_down_ind.valid;
    cur_pdp->teardown_ind.ti_flag = pdp_deactivate->tear_down_ind.ti_flag; 
#else

    /*If primary pdp set the tear down*/
    if (cur_pdp->is_primary_pdp)
    {
      cur_pdp->teardown_ind.valid = TRUE;
      cur_pdp->teardown_ind.ti_flag = TRUE;
    }
    // Check if the Teardown Indicator is specified in the request message */
    else if (pdp_deactivate->tear_down_ind.valid && 
        !sm_check_group_pdp_pending_deactivation(cur_pdp))
    {
      cur_pdp->teardown_ind.valid = pdp_deactivate->tear_down_ind.valid;
      cur_pdp->teardown_ind.ti_flag = pdp_deactivate->tear_down_ind.ti_flag; 
    }
    else
    {
      cur_pdp->teardown_ind.valid = FALSE;
      cur_pdp->teardown_ind.ti_flag = FALSE;
    }
#endif

#ifdef FEATURE_REL5
    /* Set the Configuration Options */
    cur_pdp->config_options = pdp_deactivate->config_options;
#endif
    

      // Set the state of the PDP context to PDP_INACTIVE_PENDING
      cur_pdp->pdp_state = PDP_INACTIVE_PENDING;
      MSG_HIGH_DS_1( SM_SUB,"PDP De-Activate Req for NSAPI = %d; Changing PDP state to: INACTIVE_PENDING",cur_pdp->nsapi.nsapi);

      if (is_umts_mode())
      {
        // Inform the RABM that the PDP context is being deactivated
        MSG_MED_DS_1( SM_SUB,"Inform RABM PDP Nsapi %d deactivated",cur_pdp->nsapi.nsapi);
        cur_pdp->rab_state = RABM_INACTIVE_PENDING;
        sm_notify_rabm ((int)RABMSM_DEACTIVATE_IND, cur_pdp->nsapi.nsapi);
      }
 

    /*If TI flag is true bring down all the PDP contexts with same APN,
    PDP type and address*/
    if (cur_pdp->teardown_ind.ti_flag)
    {
      word index;
      for (index = 0; index < MAX_PDP_NUM; index++)
      {
        if ( pdp_list[index] != NULL && 
             (pdp_list[index]->trans_id != cur_pdp->trans_id) && 
             (cur_pdp->group_id == pdp_list[index]->group_id) &&
             (pdp_list[index]->pdp_state != PDP_INACTIVE))
        {
          if (pdp_deactivate->cause == SM_LOCAL_PDP_DEACTIVATION)
          {
            sm_perform_local_pdp_deactivation(pdp_list[index]);
          }
          else
          {
           //If the profile matches change the state to PDP_INACTIVE_PENDING
            MSG_HIGH_DS_2( SM_SUB,"PDP state changed to INACTIVE_PENDING TI: %d, NSAPI: %d",pdp_list[index]->trans_id,cur_pdp->nsapi.nsapi);
            pdp_list[index]->pdp_state = PDP_INACTIVE_PENDING;
            //Change the RAB state to RAB_INTIAL
            pdp_list[index]->rab_state = RABM_INITIAL;
            /* set the current pending CM message */
            pdp_list[index]->cm_pending_msg = (int)SMREG_PDP_DEACTIVATE_REQ;
#ifdef FEATURE_MBMS
            #error code not present
#endif
            if (is_umts_mode())
            {
              // Inform the RABM that the PDP context is being deactivated
              MSG_MED_DS_1( SM_SUB,"Inform RABM PDP Nsapi %d deactivated",
                                        pdp_list[index]->nsapi.nsapi);
              pdp_list[index]->rab_state = RABM_INACTIVE_PENDING;
              sm_notify_rabm ((int)RABMSM_DEACTIVATE_IND, pdp_list[index]->nsapi.nsapi);
            }
#ifdef FEATURE_LTE
            MSG_HIGH_DS_2(SM_SUB,"Deleting ESM context trans_id = %d  nsapi = %d", 
                               pdp_list[index]->trans_id,pdp_list[index]->nsapi.nsapi);
            (void)esm_update_esm_context_from_sm(pdp_list[index]);
#endif /*FEATURE_LTE*/
          }
        }
        /* If SM is waiting for secondary PDP activaiton request form CM for 
        MT sec PDP activation*/
        else if( pdp_list[index] != NULL && 
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL 
                    (index != cur_pdp->trans_id) && 
                     (pdp_list[index]->trans_id != INVALID_ID) &&
#endif
                     (cur_pdp->group_id == pdp_list[index]->group_id) &&
                     (pdp_list[index]->mt_orig_type == TRUE)&&
                     (pdp_list[index]->llc_state == LLC_NOT_CONFIRMED && pdp_list[index]->rab_state == RABM_INITIAL ) )
        {
            // Send the SMREG_PDP_DEACTIVATE_IND to the CM.
            sm_send_cm_confirm(pdp_list[index], SMREG_PDP_DEACTIVATE_IND, SM_INTERNAL_CAUSE);
        }
      }  /* end of for loop*/
    }   /*end of TI flag check*/
    
    if (pdp_deactivate->cause == SM_LOCAL_PDP_DEACTIVATION)
    {
      //Locally Deactivate the PDP.
      sm_perform_local_pdp_deactivation(cur_pdp); 
    
      // Close the PS signaling connection if its already setup
      MSG_HIGH_DS_0( SM_SUB,"Send GMM Close Session Request");
      send_gmm_close_session_req();
    }
    else
    {
      /* ----------------------------------------------------------------------
      ** Call sm_continue_pdp_deactivate function, this function will send the
      ** Signaling message to GMM if the Signaling connection is available. 
      ** Otherwise an ESTABLISH_REQ is sent to GMM for a signaling connection.
      ** ---------------------------------------------------------------------*/
      sm_continue_pdp_deactivate(cur_pdp);
    }
  }
  //Do local PDP deactivation if PDP is not in ACTIVE state
  else
  {
    if(cur_pdp->pdp_state == PDP_ACTIVE_PENDING)
    {
      MSG_HIGH_DS_0( SM_SUB,"FEATURE RAB FAILURE ACTIVE");
      sm_stop_timer (con_id);
      sm_start_timer(con_id, TIMER_EARLY_RAB_FAILURE);
      cur_pdp->current_cause = (cc_cause_T)pdp_deactivate->cause;
      cur_pdp->teardown_ind = pdp_deactivate->tear_down_ind;
      cur_pdp->config_options = pdp_deactivate->config_options;
    }
    else if (!(cur_pdp->pdp_state == PDP_INACTIVE &&
          sm_timer_status[cur_pdp->trans_id] == (word) TIMER_RABM_RESP_PENDING))
    {
      MSG_HIGH_DS_1(SM_SUB,"Locally deactivating PDP with NSAPI =%d", cur_pdp->nsapi.nsapi);
      if (is_umts_mode())
      {
        // Inform the RABM that the PDP context is being deactivated
        MSG_HIGH_DS_1( SM_SUB,"Inform RABM PDP Nsapi %d deactivated",cur_pdp->nsapi.nsapi);
        sm_notify_rabm ((int)RABMSM_DEACTIVATE_IMPLICIT_IND, cur_pdp->nsapi.nsapi);
      }
#ifdef FEATURE_GSM_GPRS
      if (cur_pdp->llc_state != LLC_NOT_CONFIRMED && 
            cur_pdp->pdp_state == PDP_INACTIVE_PENDING)
      {
        /*Send SNSM_DEACTIVATE_IND to SNDCP*/
        sm_send_sndcp_cmd (cur_pdp, GSNSM_DEACTIVATE_IND);
      }
#endif
      // Set the PDP state back to PDP_INACTIVE and stop the Establish timer
      cur_pdp->pdp_state = PDP_INACTIVE;
      //Reset the PDP con state
      cur_pdp->pdp_con_state = SIG_CONNECTION_NONE;
      sm_stop_timer (con_id);
      //If no signalling connection is pending clear the PDP timer
      if (!sm_check_mpdp_conn_pend())
      {
        #ifndef FEATURE_DUAL_DATA
        sm_clear_pdp_timer (TIMER_ESTABLISH_PENDING); 
        #endif 
        timer_est_pen_flag = FALSE;
        max_est_timer_count = 0;
      }
      

      // Send the DEACTIVATE_CNF to the Call Manager
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
      if (cur_pdp->is_primary_pdp && !cur_pdp->teardown_ind.ti_flag &&
          sm_promote_sec_pri(cur_pdp) )
      {
        //Send the SMREG_PDP_DEACTIVATE_PROMOTE_IND to the CM
        sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_PROMOTE_CNF,
                           SM_INTERNAL_CAUSE );
      }
      else
      {
#endif
        // Send the SMREG_PDP_DEACTIVATE_CNF to the CM.
        sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_CNF, SM_INTERNAL_CAUSE);

#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
      }
#endif
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/

      // Close the PS signaling connection if its already setup
      MSG_HIGH_DS_0( SM_SUB,"Send GMM Close Session Request");
      send_gmm_close_session_req();
    }
  }
}

/*===========================================================================
FUNCTION SM_PDP_MODIFY 

DESCRIPTION
  This function processes SMREG_PDP_MODIFY_REQ primitive sent from CM.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_pdp_modify (byte con_id, smreg_pdp_modify_req_T *pdp_modify)
{
  pdp_context_info *cur_pdp; /* current pdp context in the transaction */
  ue_rev_level_indicator_type ue_revision;
  ue_revision = sm_get_revision_level_indicator();

  sig_connection_available = FALSE; //Set the Signalling connection to FALSE

  // Process the PDP context modification request.
  MSG_MED_DS_1( SM_SUB,"PDP Modification started for conid %d", con_id);

  cur_pdp = pdp_list[con_id]; /* Set the current PDP context */
  
  if(cur_pdp == NULL)
  {
    MSG_ERROR_DS_1( SM_SUB, "PDP is NULL, Connection id %d is unreserved", con_id);
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm_ti_cause(con_id, INVALID_CONNECTIN_ID, SMREG_PDP_MODIFY_REJ);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
    return;
  }

#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
  if ((SM_IS_VALID_AS_ID(pdp_modify->as_id)==FALSE))
  {
    MSG_ERROR_DS_2( SM_SUB,"SM AS ID Mismatch sm_as_id : %d, message as_id: %d", sm_as_id , pdp_modify->as_id);
    cur_pdp->current_cause = (cc_cause_T)SM_NO_RADIO_AVAILABLE;
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_REJ, SM_INTERNAL_CAUSE);
    return ;
  }
#endif
#endif

  #ifdef FEATURE_MBMS
  #error code not present
#endif

  if (cur_pdp->pdp_state == PDP_MODIFY_PENDING && cur_pdp->cm_pending_msg == UNKNOWN_CMD)
  {
    MSG_HIGH_DS_1(SM_SUB, "MO Modify collision, MT alerady in progress for TI=%d", con_id);
    cur_pdp->current_cause = (cc_cause_T)PDP_MODIFY_COLLISION; 
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
    return;
  }
  
  /* set the current pending CM message */
  cur_pdp->cm_pending_msg = (int)SMREG_PDP_MODIFY_REQ;

  if ( ue_revision == UE_REV_PRE_REL99 )
  {
    MSG_ERROR_DS_1( SM_SUB,"Invalid Modify Request by CM for ConId %d", con_id);
    cur_pdp->current_cause = (cc_cause_T)SM_INSUFFICIENT_RESOURCES;
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
  }
  // Check if the PDP is currently Active, before modifying.
  else if (cur_pdp->pdp_state == PDP_INACTIVE ||
      cur_pdp->pdp_state == PDP_ACTIVE_PENDING)
  {
    MSG_ERROR_DS_1( SM_SUB,"Invalid Modify Request by CM for ConId %d", con_id);
    cur_pdp->current_cause = (cc_cause_T)INVALID_NSAPI;
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
  }
  else if (cur_pdp->pdp_state == PDP_MODIFY_PENDING){
    MSG_ERROR_DS_1( SM_SUB,"PDP Modify is already pending for con id %d", con_id);
    cur_pdp->current_cause = (cc_cause_T)PDP_MODIFY_COLLISION;
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
  }
  /*Already One PDP in the group doesn't have TFT, Reject the Modify req for deleting TFT*/
  else if ((pdp_modify->tft.valid == TRUE) && (((pdp_modify->tft.operation_code == 2) 
              && (!sm_if_tft_exist(cur_pdp->group_id))) || ((pdp_modify->tft.operation_code != 2) && 
              (pdp_modify->tft.operation_code != 6)&& (pdp_modify->tft.num_filters == 0))))
  {
    MSG_ERROR_DS_1( SM_SUB,"Reject the Modify req for deleting TFT for Invalid TFT con_id %d",con_id);
    cur_pdp->current_cause = (cc_cause_T)SM_SEMANTIC_ERR_IN_TFT;
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
  }
  else if(sm_is_apn_blocked(cur_pdp->apn_name))
  {
    MSG_ERROR_DS_0( SM_SUB,"Reject the Modify req because APN is blocked");
    cur_pdp->current_cause = (cc_cause_T)SM_APN_BLOCKED;
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/

  }
  else
  {

    MSG_HIGH_DS_1(SM_SUB,"PDP Modify Request for NSAPI = %d", con_id);

    /* set the current pending CM message */
    cur_pdp->cm_pending_msg = (int)SMREG_PDP_MODIFY_REQ;

    // set the LLC sapi
   /* if (pdp_modify->llc_sapi.valid)
    {
      memcpy(&cur_pdp->req_llc_sapi, &pdp_modify->llc_sapi, sizeof(nsapi_T));
    }*/

    // set the qos if a new qos is included
    if (pdp_modify->qos.valid)
    {
      cur_pdp->req_qos = pdp_modify->qos;
    }

    // set the tft if included
    if (pdp_modify->tft.valid)
    {
      cur_pdp->req_tft = pdp_modify->tft;
    }
#ifdef FEATURE_REL5
    /* Set the Configuration Options */
    cur_pdp->config_options = pdp_modify->config_options;
#endif

    if (pdp_modify->minimum_qos.valid)
    {
      cur_pdp->req_minqos = pdp_modify->minimum_qos;
    }

    //Set the PDP state to modify pending
    cur_pdp->pdp_state = PDP_MODIFY_PENDING;
    MSG_HIGH_DS_2(SM_SUB,"PDP Modify Request for NSAPI = %d; PDP %d state changed to MODIFY_PENDING",con_id,cur_pdp->trans_id);
    // Set the RABM response state for this PDP context to RABM_INITIAL 
    cur_pdp->rab_state = RABM_INITIAL;
    /* ----------------------------------------------------------------------
    ** Call sm_continue_pdp_modify function, this function will send the
    ** Signaling message to GMM if the Signaling connection is available. 
    ** Otherwise an ESTABLISH_REQ is sent to GMM for a signaling connection.
    ** ---------------------------------------------------------------------*/
    sm_continue_pdp_modify(cur_pdp);
  }
}

/*===========================================================================
FUNCTION SM_SEC_PDP_ACTIVATE 

DESCRIPTION
  This function processes SMREG_PDP_MODIFY_REQ sent by CM.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_sec_pdp_activate
(
  byte con_id, 
  smreg_pdp_activate_sec_req_T* sec_pdp_req
)
{
  pdp_context_info *cur_pdp; // current pdp context in the transaction
  byte pri_pdp_index;
  ue_rev_level_indicator_type ue_revision;
  ue_revision = sm_get_revision_level_indicator();

MSG_HIGH_DS_3( SM_SUB,"PDP Secondary Activation started for conid %d, group-id: %d,nsapi valid: %d", con_id, sec_pdp_req->group_id,sec_pdp_req->primary_nsapi.valid);  


#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA 
if(SM_IS_VALID_AS_ID(sec_pdp_req->as_id) == FALSE)
{
    MSG_ERROR_DS_1( SM_SUB,"PDP is NULL, Connection id %d is unreserved", con_id);
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm_ti_cause(con_id, INVALID_CONNECTIN_ID, SMREG_PDP_ACTIVATE_SEC_REJ);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
}
#endif /*#ifdef FEATURE_DUAL_DATA*/
#endif  

  if(pdp_list[con_id] == NULL && sm_get_connection_id_int (con_id) == FALSE )
  {
    MSG_ERROR_DS_1( SM_SUB,"PDP is NULL, Connection id %d is unreserved", con_id);
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm_ti_cause(con_id, INVALID_CONNECTIN_ID, SMREG_PDP_ACTIVATE_SEC_REJ);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
    return;
  }
  
  cur_pdp = pdp_list[con_id];   

  //Set the Signalling connection to FALSE
  sig_connection_available = FALSE; 
  
  /* Reject MO Secondary PDP activation is PDP is alerady active and MT secondary PDP activation
      if SM is not waiting for CM reply
  */
  if ((con_id > MAX_MO_TI && con_id < MIN_MT_TI)     || 
      con_id > MAX_MT_TI                             ||
      (con_id >= MIN_MT_TI && !cur_pdp->mt_orig_type)  ||
      cur_pdp->pdp_state != PDP_INACTIVE)
  {
    MSG_ERROR_DS_0( SM_SUB, "Reject the Secondary PDP ACTIVATE req");
    cur_pdp->current_cause = (cc_cause_T)INVALID_CONNECTIN_ID;
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
    return;
  }
#ifdef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
  pri_pdp_index = sm_get_linked_ti(sec_pdp_req->group_id);
#else
  pri_pdp_index = sm_get_primary_index(sec_pdp_req->group_id);
#endif
  /* Set the QoS valid flag to be TRUE */
  cur_pdp->qos_valid = sec_pdp_req->qos.valid;
  cur_pdp->trans_id = con_id;
  
#ifdef FEATURE_DUAL_SIM
 #ifndef FEATURE_DUAL_DATA
  if (SM_IS_VALID_AS_ID(sec_pdp_req->as_id) == FALSE)
  {
    MSG_ERROR_DS_2( SM_SUB,"SM AS ID Mismatch sm_as_id : %d, message as_id: %d", sm_as_id , sec_pdp_req->as_id);
    cur_pdp->current_cause = (cc_cause_T)SM_NO_RADIO_AVAILABLE;
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_SEC_REJ, SM_INTERNAL_CAUSE);
    return ;
  }
  #endif /*#ifdef FEATURE_DUAL_DATA*/
#endif

if ( ue_revision == UE_REV_PRE_REL99 )
{
    MSG_ERROR_DS_1( SM_SUB,"Invalid Secondary Activate Request by CM for ConId %d", con_id);
    cur_pdp->current_cause = (cc_cause_T)SM_INSUFFICIENT_RESOURCES;
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_SEC_REJ, SM_INTERNAL_CAUSE);
#endif
    return;
}
  /* ----------------------------------------------------------------------
  ** Reject if the PDP context is already active for the given con_id.
  ** Also the internal reject cause is set properly 
  ** --------------------------------------------------------------------*/
  else if (cur_pdp->pdp_state != PDP_INACTIVE)
  {
    MSG_ERROR_DS_0( SM_SUB,"PDP already active, Reject the ACTIVATE req");
    cur_pdp->current_cause = (cc_cause_T)INVALID_NSAPI;
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_SEC_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
  }
  // If the primary context does't exist reject the PDP context 
  else if(pri_pdp_index == INVALID_ID 
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
       || pdp_list[pri_pdp_index] == NULL || 
       pdp_list[pri_pdp_index]->pdp_state == PDP_INACTIVE
#endif
       )
  {
    
    MSG_ERROR_DS_2( SM_SUB,"Primary context for NSAPi %d Doest't exist, Reject the SEC ACTIVATE req con_id %d",
                    sec_pdp_req->primary_nsapi.nsapi, con_id);
    cur_pdp->current_cause = (cc_cause_T)INVALID_PRI_NSAPI;
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_SEC_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
  }  
  else if (((!sm_if_tft_exist(sec_pdp_req->group_id)) && (sec_pdp_req->tft.valid == FALSE))||
                ((sec_pdp_req->tft.operation_code != 2) && (sec_pdp_req->tft.operation_code != 6)&&
                  (sec_pdp_req->tft.num_filters == 0) && (sec_pdp_req->tft.valid == TRUE)))
  {
    MSG_ERROR_DS_1( SM_SUB,"Reject the SEC ACTIVATE req con_id because of Invalid TFT %d",con_id);
    cur_pdp->current_cause = (cc_cause_T)SM_PDP_WITHOUT_TFT_ACTIVE;
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_SEC_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
  }
  else if(sm_is_apn_blocked(pdp_list[pri_pdp_index]->apn_name))
  {
     MSG_ERROR_DS_0( SM_SUB,"Reject the SEC ACTIVATE req because APN is blocked");
    cur_pdp->current_cause = (cc_cause_T)SM_APN_BLOCKED;

    /* Copy the APN from the primary */
    cur_pdp->apn_name = pdp_list[pri_pdp_index]->apn_name;

    /* Set the PDP address */
    cur_pdp->pdp_addr = pdp_list[pri_pdp_index]->pdp_addr;
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_SEC_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
  }
  else
  {
    /* -----------------------------------------------------------------------
    ** Set the PDP parameters for the PDP context based on the Connection Id.
    ** ---------------------------------------------------------------------*/
     /* set the NSAPI for this PDP */
    if (con_id < MIN_MT_TI)
    {
      cur_pdp->nsapi.valid = TRUE;
#ifdef FEATURE_GPRS_CALLCONTROL
      cur_pdp->nsapi.nsapi = sec_pdp_req->nsapi.nsapi;
#else
      cur_pdp->nsapi.nsapi = sm_per_subs_reserve_nsapi (con_id, (sys_modem_as_id_e_type)sm_as_id);
#endif
    }
   

    //set the group id
    cur_pdp->group_id = sec_pdp_req->group_id;

    //set the pdp as primary PDP context
    cur_pdp->is_primary_pdp = FALSE;

    //set the linked trans id
    cur_pdp->linked_ti = pri_pdp_index;
     
    // Set the RABM response state for this PDP context to RABM_INITIAL.
    cur_pdp->rab_state = RABM_INITIAL;

    /* set the current pending CM message */
    cur_pdp->cm_pending_msg = (int)SMREG_PDP_ACTIVATE_SEC_REQ;

    /* Set the LLC-SAPI to one of the allowed LLC-SAPI for SNDCP 3,5,9,11 */
    cur_pdp->llc_sapi.valid = TRUE;

    /* Set the QoS valid flag to be TRUE */
    cur_pdp->qos_valid = TRUE;
  
    cur_pdp->bcm = pdp_list[pri_pdp_index]->bcm;

    /* ----------------------------------------------------------------------
    ** 6.1.3.1.1 of TS 24.008:  An MS, which is capable of operating in both 
    ** GSM & UMTS, shall use a valid LLC SAPI, while an MS which is capable 
    ** of operating only in UMTS shall indicate the LLC SAPI as "LLC SAPI not 
    ** assigned" in order to avoid unnecessary value range checking and any 
    ** other possible confusion in the network.
    ** ----------------------------------------------------------------------*/

#if defined(FEATURE_GSM_GPRS) 
#ifdef FEATURE_GPRS_CALLCONTROL
     cur_pdp->llc_sapi.sapi = sec_pdp_req->llc_sapi.sapi;
#else
    cur_pdp->llc_sapi.sapi = LLC_SAPI_3;
#endif
#else 
    cur_pdp->llc_sapi.sapi = LLC_SAPI_UNASSIGNED;
#endif

    /* Set the transaction Id same as the Connection Id */
    cur_pdp->trans_id = con_id;

    /* Copy the APN from the primary */

    cur_pdp->apn_name = pdp_list[pri_pdp_index]->apn_name;

    /* Set the PDP address */
    cur_pdp->pdp_addr = pdp_list[pri_pdp_index]->pdp_addr;

    //Set the QoS
    cur_pdp->qos = sec_pdp_req->qos;

    /* Set the Minimum required QOS for the PDP context */
    cur_pdp->minimum_qos = sec_pdp_req->minimum_qos;

     //Set the TFT
    cur_pdp->tft = sec_pdp_req->tft;
#ifdef FEATURE_REL5
    /* Set the Configuration Options */

    cur_pdp->config_options = sec_pdp_req->config_options;
#endif

    /* Set the Compression Requirements */
    cur_pdp->h_comp = sec_pdp_req->h_comp;
    cur_pdp->d_comp = sec_pdp_req->d_comp;

    cur_pdp->context_id = sec_pdp_req->context_id;
    cur_pdp->assoc_context_id = sec_pdp_req->assoc_context_id;

    MSG_HIGH_DS_3(SM_SUB,"CM PDP sec Activate Request for NSAPI = %d; NAS - Received Ctxt id P = %d, S = %d", cur_pdp->nsapi.nsapi, sec_pdp_req->context_id,sec_pdp_req->assoc_context_id);

    //Notify RABM about RAB Activation
    //SPDP CHANGE
    if (is_umts_mode())
    {
      /* Inform RABM that the PDP activation is in progress */
      MSG_LOW_DS_0( SM_SUB,"Inform RABM about PDP activation");
      cur_pdp->rab_state = RABM_ACTIVE_PENDING;
      sm_notify_rabm( (int)RABMSM_ACTIVATE_IND, cur_pdp->nsapi.nsapi );
    }

#ifdef FEATURE_DUAL_SIM
      cur_pdp->as_id = sec_pdp_req->as_id ;
#endif

    sm_continue_pdp_activation(cur_pdp);
  }
}

/*===========================================================================
FUNCTION SM_CONTINUE_PDP_ACTIVATION 

DESCRIPTION
  This function processes PDP context activation after receiving the 
  a successful GPRS attach indication from GMM. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_continue_pdp_activation(pdp_context_info* pdp)
{
  word mesg_bit_len;              /* Length of the packed L3 mesg in bits */
  xlate_status_T status;          /* status of the packed L3 message */

  uint16 max_l3_message = gmm_max_L3_msg_length(MM_PS_DOMAIN);
  byte *l3_buf; /* Buffer to store the packed L3 msg*/

  rrc_establish_cause_e_type cause_type = convert_to_rrc_est_cause(pdp->qos.traffic_class);

  // Do not allow Reselections/HOs when RRC quries as the PDP activation is in progress
  sm_ps_vote_for_reselection = FALSE;

  /* ----------------------------------------------------------------------
  ** Make sure the RRC Signaling Connection is available, before sending
  ** the SM Signaling message. Request GMM to establish a connection if it
  ** is not available.
  ** ---------------------------------------------------------------------*/
  if (!sig_connection_available)
  {    
    MSG_HIGH_DS_0( SM_SUB,"Send GMMSM_ESTABLISH_REQ, REQ CONN ");
    // Change the PDP SIG connect state to SIG_CONNECTION_PENDING
    pdp->pdp_con_state = SIG_CONNECTION_PENDING;
    /* ------------------------------------------------------------------
    ** Set T3380 timer and put connection Id into the timers_pending.
    ** Wait for the Response message from GMM.
    ** -----------------------------------------------------------------*/
   
#ifdef FEATURE_DUAL_DATA
    if ( (!timer_est_pen_flag) || (sm_timer_status[pdp->trans_id] == (word)SM_TIMER_STOPPED) )
#else
    if ( !timer_est_pen_flag )
#endif
    {
      
      #ifdef FEATURE_DUAL_DATA
      sm_start_timer(pdp->trans_id, TIMER_ESTABLISH_PENDING);
      #else
      timers_pending[TIMER_PENDING] = pdp->trans_id;
      sm_set_pdp_timer(TIMER_ESTABLISH_PENDING);  
      #endif  

      //Set the timer_est_pen_flag
      timer_est_pen_flag = TRUE;
      //sm_start_timer(pdp->trans_id, TIMER_ESTABLISH_PENDING)
      
      // Send an Establish Request GMM task to send 
#ifdef FEATURE_MBMS
      #error code not present
#endif
      send_gmm_establish_req(cause_type);    
    }
    else
    {
      max_est_timer_count = 0;
    }
  }
  else
  {
#ifndef FEATURE_MODEM_HEAP
    l3_buf = (uint8*) gs_alloc(max_l3_message);
#else
    l3_buf = (uint8*) modem_mem_calloc(1, max_l3_message, MODEM_MEM_CLIENT_NAS);
#endif
    sm_check_for_null_ptr((void*)l3_buf);    
    if (is_umts_mode())
    {
      /* Inform RABM that the PDP activation is in progress */
      MSG_LOW_DS_1( SM_SUB,"Inform RABM about PDP activation for RAB %d", pdp->nsapi.nsapi);
      pdp->rab_state = RABM_ACTIVE_PENDING;
      sm_notify_rabm( (int)RABMSM_ACTIVATE_IND, pdp->nsapi.nsapi );
    }
    /* ------------------------------------------------------------------
    ** Set T3380 timer and put connection Id into the timers_pending.
    ** Wait for the Response message from the Network.
    ** -----------------------------------------------------------------*/
    //timers_pending[TIMER_PENDING] = pdp->trans_id;

    sm_start_timer(pdp->trans_id, TIMER_ACTIVE_PENDING);

    /* Pack the Layer 3 Message and send it to GMM */
    if (pdp->is_primary_pdp == TRUE)
    {
      status = pack_layer3_mesg(&mesg_bit_len, l3_buf, pdp, ACTIVATE_PDP_REQUEST);
    }
    else
    {
      status = pack_layer3_mesg(&mesg_bit_len, l3_buf, pdp, ACTIVATE_SEC_PDP_REQUEST);
    }

    /* --------------------------------------------------------------------
    ** If this request is being sent to the Network due to the Mobile 
    ** terminated PDP context request, set the TI_FLAG to 1.  Only Mobile
    ** Originated PS calls are supported, no need to set the TI_FLAG as 1.
    ** -------------------------------------------------------------------*/
    if(pdp->mt_orig_type)
    {
    //  l3_buf[0] |= 0x80;          // Set the TI_FLAG to 1      
    }

    if (status == L3_PARSE_SUCCESS)
    {
      /* Change the PDP context state */
      pdp->pdp_state = PDP_ACTIVE_PENDING;
      MSG_HIGH_DS_0(SM_SUB,"PDP State changed to ACTIVE_PENDING");

      // Send the packed SM-PDU to the GMM to be sent to the Network
      send_gmm_smpdu(mesg_bit_len, l3_buf, FALSE,TRUE); 

#ifdef FEATURE_DUAL_SIM
      send_gmm_ok_to_tuneaway_ind();
#endif

#ifdef FEATURE_MMAL
      MSG_HIGH_DS_1(SM_SUB,"NAS - Calling sm_fill_and_send_pdp_context_log_info for TI : %d ", pdp->trans_id);
      (void) sm_fill_and_send_pdp_context_log_info(pdp);
#endif
      //Log the event if ACTIVATE_PDP_REQUEST is sent.
      if (pdp->is_primary_pdp == TRUE)
      {
         sm_log_pdp_request_reject_event(TRUE,(int)SMGMM_EVENT_PDP_ACTIVATE_REQUEST,0);
      }      
    }
    else
    {
          
      if (pdp->current_cause != SM_MESSAGE_EXCEED_MAX_L2_LIMIT)
      {
        pdp->current_cause = (cc_cause_T)INVALID_FIELD;
        MSG_ERROR_DS_0( SM_SUB,"Unable to Encode OTA mesg, INVALID field");
      
      }
      else
      {
        MSG_ERROR_DS_0( SM_SUB,"Unable to Encode OTA mesg, PDU exceeding maximum OTA length");
      }
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
      if (pdp->is_primary_pdp == TRUE)
      {
        sm_send_cm_confirm(pdp, SMREG_PDP_ACTIVATE_REJ, SM_INTERNAL_CAUSE);
      }
      else
      {
        sm_send_cm_confirm(pdp, SMREG_PDP_ACTIVATE_SEC_REJ, SM_INTERNAL_CAUSE);
      }
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
    }
    if(l3_buf != NULL)
    {
#ifdef FEATURE_MODEM_HEAP
      modem_mem_free( l3_buf , MODEM_MEM_CLIENT_NAS);
      l3_buf = NULL;
#else
      gs_free( l3_buf);
      l3_buf = NULL;
#endif
    }
  }  
}

/*===========================================================================
FUNCTION SM_CONTINUE_PDP_MODIFY 

DESCRIPTION
  This function processes PDP context modification.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_continue_pdp_modify(pdp_context_info* cur_pdp)
{

  byte *l3_buf;

  word mesg_bit_len;
  xlate_status_T status;          /* status of the packed L3 message */

 
  /* ----------------------------------------------------------------------
  ** Make sure the RRC Signaling Connection is available, before sending
  ** the SM Signaling message. Request GMM to establish a connection if it
  ** is not available.
  ** ---------------------------------------------------------------------*/
  if (!sig_connection_available)
  {    
    MSG_HIGH_DS_0( SM_SUB,"Send GMMSM_ESTABLISH_REQ, REQ CONN ");

    // Change the PDP SIG connect state to SIG_CONNECTION_PENDING
    cur_pdp->pdp_con_state = SIG_CONNECTION_PENDING;

#ifdef FEATURE_DUAL_DATA
    if ( (!timer_est_pen_flag) || (sm_timer_status[cur_pdp->trans_id] == (word)SM_TIMER_STOPPED) )
#else
    if ( !timer_est_pen_flag )
#endif
    {
      
      #ifdef FEATURE_DUAL_DATA
      sm_start_timer(cur_pdp->trans_id, TIMER_ESTABLISH_PENDING);
      #else
      timers_pending[TIMER_PENDING] = cur_pdp->trans_id;
      sm_set_pdp_timer(TIMER_ESTABLISH_PENDING);
      #endif 


      //Set the timer_est_pen_flag
      timer_est_pen_flag = TRUE;
      //sm_start_timer(pdp->trans_id, TIMER_ESTABLISH_PENDING)

      // Send an Establish Request GMM task to send 
      send_gmm_establish_req(RRC_EST_HI_PRI_SIGNALLING);
    }
    else
    {
       max_est_timer_count = 0;
    }
  }
  else
  {
#ifndef FEATURE_MODEM_HEAP
     l3_buf = (uint8*) gs_alloc(gmm_max_L3_msg_length(MM_PS_DOMAIN));
#else
     l3_buf = (uint8*) modem_mem_calloc(1,gmm_max_L3_msg_length(MM_PS_DOMAIN),MODEM_MEM_CLIENT_NAS);
#endif
     sm_check_for_null_ptr((void*)l3_buf);

    if (is_umts_mode())
    {
      // Inform the RABM that the PDP context is being modified
      MSG_MED_DS_1( SM_SUB,"Inform RABM PDP Nsapi %d modified",cur_pdp->nsapi.nsapi);
      sm_notify_rabm((int)RABMSM_MODIFY_IND, cur_pdp->nsapi.nsapi);
      cur_pdp->rab_state = RABM_MODIFY_PENDING;
    }
    
    /* --------------------------------------------------------------------
    **  Start the T3381 timer,  Put the connection Id into the 
    ** timers_pending array and wait for the respnose message from Network.
    ** -------------------------------------------------------------------*/
    //timers_pending[TIMER_PENDING] = cur_pdp->trans_id;
    sm_start_timer (cur_pdp->trans_id, TIMER_MODIFY_PENDING);
    
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
    cur_pdp->sm_msg_to_resend   = (sm_layer_msg_T)0xFF;
    cur_pdp->status_ind_flag = FALSE;
#endif

    /* Pack the Layer 3 Message and send it to GMM */
    status = pack_layer3_mesg(&mesg_bit_len, l3_buf, cur_pdp, 
                            MS_MODIFY_PDP_REQUEST);

    if (status == L3_PARSE_SUCCESS)
    {    
      // Send the packed SM-PDU to the GMM to be sent to the Network
      send_gmm_smpdu(mesg_bit_len, l3_buf, FALSE,FALSE);   

#ifdef FEATURE_DUAL_SIM
      send_gmm_ok_to_tuneaway_ind();
#endif

    }
    else
    {
      if (cur_pdp->current_cause != SM_MESSAGE_EXCEED_MAX_L2_LIMIT)
      {
        cur_pdp->current_cause = (cc_cause_T)INVALID_FIELD;
        MSG_ERROR_DS_0( SM_SUB,"Unable to Encode OTA mesg, INVALID field");
      
      }
      else
      {
        MSG_ERROR_DS_0( SM_SUB,"Unable to Encode OTA mesg, PDU exceeding maximum OTA length");
      }
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
      sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
    }
#ifdef FEATURE_MODEM_HEAP
    modem_mem_free( l3_buf , MODEM_MEM_CLIENT_NAS);
    l3_buf = NULL;
#else
    gs_free( l3_buf);
    l3_buf = NULL;
#endif
  }
}

/*===========================================================================
FUNCTION SM_CONTINUE_PDP_DEACTIVATE

DESCRIPTION
  This function continues the PDP context deactivate request procedure.
  It packs the Layer 3 message in to the OTA format and sends the 
  SM-PDU to the GMM to be sent to the Network.
  
DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void sm_continue_pdp_deactivate(pdp_context_info *cur_pdp)
{
  xlate_status_T status;
  word mesg_bit_len;

  byte *l3_buf ;


  /* ----------------------------------------------------------------------
  ** Make sure the RRC Signaling Connection is available, before sending
  ** the SM Signaling message. Request GMM to establish a connection if it
  ** is not available.
  ** ---------------------------------------------------------------------*/
  if (!sig_connection_available)
  {    
    MSG_HIGH_DS_0( SM_SUB,"Send GMMSM_ESTABLISH_REQ, REQ CONN ");

    // Change the PDP SIG connect state to SIG_CONNECTION_PENDING
    cur_pdp->pdp_con_state = SIG_CONNECTION_PENDING;

#ifdef FEATURE_DUAL_DATA
    if ( (!timer_est_pen_flag) || (sm_timer_status[cur_pdp->trans_id] == (word)SM_TIMER_STOPPED) )
#else
    if ( !timer_est_pen_flag )
#endif
    {
      #ifdef FEATURE_DUAL_DATA
      sm_start_timer(cur_pdp->trans_id, TIMER_ESTABLISH_PENDING);
      #else
      timers_pending[TIMER_PENDING] = cur_pdp->trans_id;
      sm_set_pdp_timer(TIMER_ESTABLISH_PENDING);
      #endif 

      //Set the timer_est_pen_flag
      timer_est_pen_flag = TRUE;
      //sm_start_timer(pdp->trans_id, TIMER_ESTABLISH_PENDING)

      // Send an Establish Request GMM task to send 
      send_gmm_establish_req(RRC_EST_HI_PRI_SIGNALLING);
    }
    else
    {
        max_est_timer_count = 0;
    }
  }
  else
  {
#ifndef FEATURE_MODEM_HEAP
    l3_buf = (uint8*) gs_alloc(gmm_max_L3_msg_length(MM_PS_DOMAIN));
#else
    l3_buf = (uint8*) modem_mem_calloc(1, gmm_max_L3_msg_length(MM_PS_DOMAIN), MODEM_MEM_CLIENT_NAS);
#endif
    sm_check_for_null_ptr((void*)l3_buf);

    if (is_umts_mode())
    {
      /* Inform RABM that the PDP activation is in progress */
      MSG_LOW_DS_1( SM_SUB,"Inform RABM about PDP deactivation for RAB %d", cur_pdp->nsapi.nsapi);
      cur_pdp->rab_state = RABM_INACTIVE_PENDING;
      sm_notify_rabm( (int)RABMSM_DEACTIVATE_IND, cur_pdp->nsapi.nsapi );
    }
    /* --------------------------------------------------------------------
    ** Start the timer and the Put the connection Id into the
    ** timers_pending array and wait for the response from the Network.
    ** -------------------------------------------------------------------*/
    sm_start_timer (cur_pdp->trans_id, TIMER_INACTIVE_PENDING);

    /* Pack the Layer 3 Message and send it to GMM */
    status = pack_layer3_mesg(&mesg_bit_len, l3_buf, cur_pdp, 
                            DEACTIVATE_PDP_REQUEST);

    if (status == L3_PARSE_SUCCESS)
    {    
      boolean pdp_inactive_flag = FALSE;
      
      #ifdef FEATURE_GPRS_PS_HANDOVER
      pdp_inactive_flag = check_pdp_inactive_state();
      #endif
    
      //Log the event
      sm_log_pdp_request_reject_event(TRUE,(int)SMGMM_EVENT_PDP_DEACTIVATE_REQUEST,cur_pdp->current_cause);
            
      /* Change the PDP context state */
      cur_pdp->pdp_state = PDP_INACTIVE_PENDING;
      MSG_HIGH_DS_0(SM_SUB,"PDP State changed to INACTIVE_PENDING");     

      // Send the packed SM-PDU to the GMM to be sent to the Network
      send_gmm_smpdu(mesg_bit_len, l3_buf, pdp_inactive_flag,FALSE);    

#ifdef FEATURE_DUAL_SIM
      send_gmm_ok_to_tuneaway_ind();
#endif

    }
    else if ( cur_pdp->qos_valid )
    {
      MSG_ERROR_DS_0( SM_SUB,"Unable to Encode OTA mesg, INVALID field");
      cur_pdp->current_cause = (cc_cause_T)INVALID_FIELD;
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
      sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_REJ, SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
    }
#ifdef FEATURE_MODEM_HEAP
    modem_mem_free( l3_buf , MODEM_MEM_CLIENT_NAS);
    l3_buf = NULL;
#else
    gs_free( l3_buf);
    l3_buf = NULL;
#endif
  }
#ifdef FEATURE_LTE
  
  (void)esm_update_esm_context_from_sm(cur_pdp);
#endif /*FEATURE_LTE*/

}

/*===========================================================================
FUNCTION SM_PDP_ACTIVATE_REJ_RSP 

DESCRIPTION
  This function sends a reject message for a Mobile Terminated PDP Activation
  request. 

DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_pdp_activate_rej_rsp 
(
  byte con_id, 
  smreg_pdp_activate_rej_resp_T* rej_rsp
)
{
  pdp_context_info *cur_pdp; /* current pdp context in the transaction */

  MSG_HIGH_DS_1( SM_SUB,"MS Reject to Network PDP activation %d", con_id);
  /*Verify the con id falls in the MT PDP range*/
  if (con_id < MIN_MT_TI || con_id > MAX_MT_TI)
  {
   
    MSG_ERROR_DS_1( SM_SUB,"ConId out of range: %d", con_id);
    return;
  }
  
#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
  sm_as_id = rej_rsp->as_id;
  if(SM_IS_VALID_AS_ID(rej_rsp->as_id) == FALSE)
  {
    MSG_ERROR_DS_2( SM_SUB,"SM AS ID Mismatch sm_as_id : %d, message as_id: %d", sm_as_id , rej_rsp->as_id);

	if (pdp_list[con_id] == NULL)
	{
      MSG_ERROR_DS_1( SM_SUB,"PDP is NULL, Connection id %d is unreserved", con_id);
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
      sm_send_cm_confirm_ti_cause(con_id, INVALID_CONNECTIN_ID, SMREG_PDP_ACTIVATE_REJ);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
      return;
	}
	else
	{
	  cur_pdp = pdp_list[con_id]; /* Set the current PDP context */
      cur_pdp->current_cause = (cc_cause_T)SM_NO_RADIO_AVAILABLE;
      sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_REJ, SM_INTERNAL_CAUSE);
	}
  }
#endif
#endif

  if (pdp_list[con_id] == NULL)
  {
    MSG_ERROR_DS_1( SM_SUB,"PDP is NULL, Connection id %d is unreserved", con_id);
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
    sm_send_cm_confirm_ti_cause(con_id, INVALID_CONNECTIN_ID, SMREG_PDP_ACTIVATE_REJ);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
    return;
  }

  #ifdef FEATURE_MBMS
  #error code not present
#endif

  cur_pdp = pdp_list[con_id]; /* Set the current PDP context */

  /* ---------------------------------------------------------------------
  ** Reject the CM request if the PDP context is in wrong state 
  ** -------------------------------------------------------------------*/
  if (!cur_pdp->mt_orig_type || cur_pdp->pdp_state != PDP_INACTIVE)
  {
    
    MSG_ERROR_DS_1( SM_SUB,"Invalid Reject msg by CM for ConId %d", con_id);
   
  }
  else
  {

    sig_connection_available = FALSE; //Set the Signalling connection to FALSE

    // Set the cause of the rejection in the PDP context
    cur_pdp->current_cause = (cc_cause_T)rej_rsp->cause;
#ifdef FEATURE_REL5
    /* Set the Configuration Options */
    cur_pdp->config_options = rej_rsp->config_options;
#endif
    //Set the CM pending message to SMREG_PDP_ACTIVATE_REJ_RSP
    cur_pdp->cm_pending_msg = (int)SMREG_PDP_ACTIVATE_REJ_RSP;

    sm_continue_pdp_activate_rej_rsp (cur_pdp);
    
  }
}

/*===========================================================================
FUNCTION SM_CONTINUE_PDP_ACTIVATE_REJ_RSP

DESCRIPTION
  This function continues the request PDP activation reject response procedure.
  It packs the Layer 3 message in to the OTA format and sends the 
  SM-PDU to the GMM to be sent to the Network.
  
DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void sm_continue_pdp_activate_rej_rsp(pdp_context_info *cur_pdp)
{
  xlate_status_T status;
  word mesg_bit_len;

  byte *l3_buf ;

  /* ----------------------------------------------------------------------
  ** Make sure the RRC Signaling Connection is available, before sending
  ** the SM Signaling message. Request GMM to establish a connection if it
  ** is not available.
  ** ---------------------------------------------------------------------*/
  if (!sig_connection_available)
  {    
    MSG_HIGH_DS_0( SM_SUB,"Send GMMSM_ESTABLISH_REQ, REQ CONN ");

    // Change the PDP SIG connect state to SIG_CONNECTION_PENDING
    cur_pdp->pdp_con_state = SIG_CONNECTION_PENDING;

    /* ------------------------------------------------------------------
    ** Set TIMER_ESTABLISH_PEND timer and put connection Id into the timers_pending.
    ** Wait for the Response message from GMM.
    ** -----------------------------------------------------------------*/
   
#ifdef FEATURE_DUAL_DATA
    if ( (!timer_est_pen_flag) || (sm_timer_status[cur_pdp->trans_id] == (word)SM_TIMER_STOPPED) )
#else
    if ( !timer_est_pen_flag )
#endif
    {
      #ifdef FEATURE_DUAL_DATA
      sm_start_timer(cur_pdp->trans_id, TIMER_ESTABLISH_PENDING);
      #else
      timers_pending[TIMER_PENDING] = cur_pdp->trans_id;
      sm_set_pdp_timer(TIMER_ESTABLISH_PENDING);  
      #endif   

      //Set the timer_est_pen_flag
      timer_est_pen_flag = TRUE;
      //sm_start_timer(pdp->trans_id, TIMER_ESTABLISH_PENDING)
      
      // Send an Establish Request GMM task to send 
      send_gmm_establish_req(RRC_EST_HI_PRI_SIGNALLING);
    }
    else
    {
      max_est_timer_count = 0;
    }
  }
  else
  {
#ifndef FEATURE_MODEM_HEAP
    l3_buf = (uint8*) gs_alloc(gmm_max_L3_msg_length(MM_PS_DOMAIN));
#else
    l3_buf = (uint8*) modem_mem_calloc(1, gmm_max_L3_msg_length(MM_PS_DOMAIN), MODEM_MEM_CLIENT_NAS);
#endif
    sm_check_for_null_ptr((void*)l3_buf);
    /* Pack the Layer 3 Message and send it to GMM */
    if (!cur_pdp->is_primary_pdp)
    {
      status = pack_layer3_mesg(&mesg_bit_len, l3_buf, cur_pdp, REQUEST_SEC_PDP_ACTIVATION_REJ);
    }
    else 
    {
    status = pack_layer3_mesg(&mesg_bit_len, l3_buf, cur_pdp, REQUEST_PDP_ACTIVATION_REJ);
    }


    if (status == L3_PARSE_SUCCESS)
    {  
      MSG_HIGH_DS_0( SM_SUB,"Send SMREG_PDP_DEACTIVATE_CNF, Req PDP Rej Sent ");
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
      sm_send_cm_confirm(cur_pdp, SMREG_PDP_DEACTIVATE_CNF, 
                                                SM_INTERNAL_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/

      // Send the packed SM-PDU to the GMM to be sent to the Network
      send_gmm_smpdu(mesg_bit_len, l3_buf, FALSE,FALSE);   
    }
    else
    {
      if (cur_pdp->current_cause != SM_MESSAGE_EXCEED_MAX_L2_LIMIT)
      {
        cur_pdp->current_cause = (cc_cause_T)INVALID_FIELD;
        MSG_ERROR_DS_0( SM_SUB,"Unable to Encode OTA mesg, INVALID field");
      
      }
      else
      {
        MSG_ERROR_DS_0( SM_SUB,"Unable to Encode OTA mesg, PDU exceeding maximum OTA length");
      }
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
      if (cur_pdp->is_primary_pdp)
      {
        sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_REJ, SM_INTERNAL_CAUSE);
      }
      else 
      {
        sm_send_cm_confirm(cur_pdp, SMREG_PDP_ACTIVATE_SEC_REJ, SM_INTERNAL_CAUSE);
      }
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
    }
    if(l3_buf != NULL)
    {
#ifdef FEATURE_MODEM_HEAP
      modem_mem_free( l3_buf , MODEM_MEM_CLIENT_NAS);
      l3_buf = NULL;
#else
      gs_free( l3_buf);
      l3_buf = NULL;
#endif
    }
  }
}

/*===========================================================================
FUNCTION SM_SEND_CM_CONFIRM 

DESCRIPTION
  This function sends a confirmation to the CM task after processing the REQ. 
  This includes both the CNF primitives and REJ primitives.

DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
void sm_send_cm_confirm_ti_cause
(
  byte   con_id,
  sm_cm_status_T   cause,
  cm_sm_cmd_type      cm_cmd
)
{

  cm_rpt_type *cm_rpt;
  #ifdef FEATURE_MBMS
  #error code not present
#endif



  switch (cm_cmd)
  {
    case SMREG_PDP_ACTIVATE_REJ:
          /* ---------------------------------------------------------------
          *    Sending PDP_ACTIVATE_REJ to CM
          * ----------------------------------------------------------------*/
          
          // Set the connection Id
          cm_rpt = cm_get_cmd_buf();
          if(cm_rpt!=NULL)
          {
            cm_rpt->cmd.pdp_act_rej.connection_id = con_id;
    
            // Pack the Cause based on if its internal or external
            cm_rpt->cmd.pdp_act_rej.cause_type = SM_INTERNAL_CAUSE;             
            cm_rpt->cmd.pdp_act_rej.cause.int_cause = cause;     
            MSG_HIGH_DS_2(SM_SUB,"Send PDP_ACTIVATE_REJ to CM Cause:%d, type:%d",
                         cm_rpt->cmd.pdp_act_rej.cause.int_cause, cm_rpt->cmd.pdp_act_rej.cause_type);
            // Set the CM report header and the command
            cm_rpt->hdr.cmd = CM_PDP_ACTIVATE_REJ;
#ifdef FEATURE_DUAL_SIM
            cm_rpt->cmd.pdp_act_rej.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
            sm_add_message_to_debug_buffer(MS_CM_SM,
                                          (dword)cm_rpt->hdr.cmd,
                                           FROM_SM,
                                           cm_rpt->cmd.pdp_act_rej.as_id);
            cm_put_cmd(cm_rpt);
          }
          else
          {
            MSG_ERROR_DS_0(SM_SUB,"CM Error: Not able to Allocate CM Buffer");
          }

        break;

    case SMREG_PDP_DEACTIVATE_REJ:
          // Set the connection Id
          cm_rpt = cm_get_cmd_buf();
          if(cm_rpt!=NULL)
          {
            cm_rpt->cmd.pdp_deact_rej.connection_id = con_id;
    
            // Pack the NSAPI 
            cm_rpt->cmd.pdp_deact_rej.cause_type = SM_INTERNAL_CAUSE;             
            cm_rpt->cmd.pdp_deact_rej.cause.int_cause = cause; 
          MSG_HIGH_DS_2(SM_SUB,"Send PDP_DEACTIVATE_REJ to CM Cause:%d, type:%d",
                             cm_rpt->cmd.pdp_deact_rej.cause.int_cause, cm_rpt->cmd.pdp_deact_rej.cause_type);
            // Set the CM report header and the command
            cm_rpt->hdr.cmd = CM_PDP_DEACTIVATE_REJ;
#ifdef FEATURE_DUAL_SIM
            cm_rpt->cmd.pdp_deact_rej.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
            sm_add_message_to_debug_buffer(MS_CM_SM,
                                          (dword)cm_rpt->hdr.cmd,
                                           FROM_SM,
                                           cm_rpt->cmd.pdp_deact_rej.as_id);

            cm_put_cmd(cm_rpt);
          }
          else
          {
            MSG_ERROR_DS_0(SM_SUB,"CM Error: Not able to Allocate CM Buffer");
          }

        break;

    case SMREG_PDP_MODIFY_REJ:
          // Set the connection Id
          cm_rpt = cm_get_cmd_buf();
          if(cm_rpt!=NULL)
          {
            cm_rpt->cmd.pdp_modify_rej.connection_id = con_id;
    
            cm_rpt->cmd.pdp_modify_rej.cause_type = SM_INTERNAL_CAUSE;             
            cm_rpt->cmd.pdp_modify_rej.cause.int_cause = cause;     
          MSG_HIGH_DS_2(SM_SUB,"Send PDP_MODIFY_REJ to CM Cause:%d, type:%d",
                             cm_rpt->cmd.pdp_modify_rej.cause.int_cause, cm_rpt->cmd.pdp_modify_rej.cause_type);
            // Set the CM report header and the command
            cm_rpt->hdr.cmd = CM_PDP_MODIFY_REJ;
#ifdef FEATURE_DUAL_SIM
            cm_rpt->cmd.pdp_modify_rej.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
            sm_add_message_to_debug_buffer(MS_CM_SM,
                                          (dword)cm_rpt->hdr.cmd,
                                           FROM_SM,
                                           cm_rpt->cmd.pdp_modify_rej.as_id);
            cm_put_cmd(cm_rpt);
          }
          else
          {
            MSG_ERROR_DS_0(SM_SUB,"CM Error: Not able to Allocate CM Buffer");
          }
        break;

    case SMREG_PDP_ACTIVATE_SEC_REJ:
          // Set the connection Id
          cm_rpt = cm_get_cmd_buf();
          if(cm_rpt!=NULL)
          {
            cm_rpt->cmd.pdp_act_sec_rej.connection_id = con_id;

            cm_rpt->cmd.pdp_act_sec_rej.cause_type = SM_INTERNAL_CAUSE;
            cm_rpt->cmd.pdp_act_sec_rej.cause.int_cause = cause;
           
          MSG_HIGH_DS_2(SM_SUB,"Send PDP_ACTIVATE_SEC_REJ to CM Cause:%d, type:%d",
                         cm_rpt->cmd.pdp_act_sec_rej.cause.int_cause, cm_rpt->cmd.pdp_act_sec_rej.cause_type);
    
            // Set the CM report header and the command
            cm_rpt->hdr.cmd = CM_PDP_ACTIVATE_SEC_REJ;
#ifdef FEATURE_DUAL_SIM
            cm_rpt->cmd.pdp_act_sec_rej.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
            sm_add_message_to_debug_buffer(MS_CM_SM,
                                          (dword)cm_rpt->hdr.cmd,
                                           FROM_SM,
                                           (sys_modem_as_id_e_type)cm_rpt->cmd.pdp_act_sec_rej.as_id);
            cm_put_cmd(cm_rpt);
          }
          else
          {
            MSG_ERROR_DS_0(SM_SUB,"CM Error: Not able to Allocate CM Buffer");
          }
        break;

#ifdef FEATURE_MBMS
#error code not present
#endif
    default :
        MSG_HIGH_DS_1(SM_SUB,"Unknown command received %x", cm_cmd);
        break;
  }

}


/*===========================================================================
FUNCTION SM_SEND_CM_CONFIRM 

DESCRIPTION
  This function sends a confirmation to the CM task after processing the REQ. 
  This includes both the CNF primitives and REJ primitives.

DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void sm_send_cm_confirm
(
  pdp_context_info*   cur_pdp, 
  cm_sm_cmd_type      cm_cmd,
  cause_enum_type_T   cause_type
)
{

  cm_rpt_type *cm_rpt;

  boolean all_pdp_inactive = TRUE; 
  word index;

  #ifdef FEATURE_MBMS
  #error code not present
#endif


  // In case of sig conn rel inform to CM right away and return
  if (cm_cmd == SMREG_PDP_SIG_CON_REL_IND)
  {
    // Get the Call Manager's command report for posting the command.
    cm_rpt = cm_get_cmd_buf();
    if(cm_rpt == NULL)
    {
      MSG_HIGH_DS_0(SM_SUB,"Unable to get allocate CM Buffer");
      return ;
    }
    /* ---------------------------------------------------------------
    *    Sending PDP_SIG_CON_REL_IND to CM
    * ----------------------------------------------------------------*/
    //Set the cause type as INTERNAL cause
    cm_rpt->cmd.ps_sig_rel_ind.cause_type = cause_type;
    // Set the CM report header and the command
    cm_rpt->hdr.cmd = CM_PS_SIG_CON_REL_IND;
    
#ifdef FEATURE_DUAL_SIM
    #ifdef FEATURE_DUAL_DATA
       cm_rpt->cmd.ps_sig_rel_ind.as_id= (sys_modem_as_id_e_type)sm_as_id; 
    #else
    if ( (sm_as_id == SYS_MODEM_AS_ID_NONE) ||  ((ctxt_as_id != SYS_MODEM_AS_ID_NONE ) && (sm_as_id != ctxt_as_id)) )
        cm_rpt->cmd.ps_sig_rel_ind.as_id= ctxt_as_id;  
    else
      cm_rpt->cmd.ps_sig_rel_ind.as_id= sm_as_id;  
    #endif /*#ifdef FEATURE_DUAL_DATA*/
#endif

    /* Log using CFA packet */
    cfa_log_packet_ex( GS_QUEUE_CM,
                       MS_CM_SM,
                       (byte)(cm_rpt->hdr.cmd),
                       MIN(250, sizeof(cm_rpt->cmd)),
                       (byte*)&(cm_rpt->cmd) );

    sm_add_message_to_debug_buffer(MS_CM_SM,
                              (dword)cm_rpt->hdr.cmd,
                               FROM_SM,
                               (sys_modem_as_id_e_type)cm_rpt->cmd.ps_sig_rel_ind.as_id);
    
    cm_put_cmd(cm_rpt);
    return;
     
  }
  
  // Initialize the Maximum timer count to 0. Reset pending counters.
  if ( !((cm_cmd == SMREG_PDP_MODIFY_CNF) && (cur_pdp->cm_pending_msg != (int)SMREG_PDP_MODIFY_REQ)))
  {
    MSG_HIGH_DS_2( SM_SUB," cm_cmd = %d : cm_pending_msg = %d", cm_cmd,cur_pdp->cm_pending_msg);
    if(cur_pdp->trans_id < MAX_PDP_NUM)
    {
      sm_max_timer_count[cur_pdp->trans_id] = 0;
    }
    max_est_timer_count = 0;
  }

  // Log the current PDP context information to the QXDM using DIAG.
  if (!sm_log_pdp_info())
  {
    MSG_MED_DS_0(SM_SUB,"Unable to get a Log packet");
  }

 
#ifdef FEATURE_MBMS  
  #error code not present
#endif    
  {
    cm_rpt = cm_get_cmd_buf();
    if(cm_rpt == NULL)
    {
      MSG_HIGH_DS_0(SM_SUB,"Unable to get allocate CM Buffer");
      return ;
    }
	MSG_HIGH_DS_3( SM_SUB," Sending CMD :0x%X to CM TI: %d, NSAPI: %d",
                     cm_cmd, cur_pdp->trans_id, cur_pdp->nsapi.nsapi);
    switch (cm_cmd)
    {
      case SMREG_PDP_ACTIVATE_CNF:
        /* ---------------------------------------------------------------
        *    Sending SMREG_PDP_ACTIVATE_CNF to CM
        * ----------------------------------------------------------------*/
        cur_pdp->mt_orig_type = FALSE;    // Clear the MT indication flag 
  
         // Log the QOS of this new PDP context to the DIAG
        if (!sm_log_qos_info(cur_pdp))
        {
              MSG_MED_DS_0(SM_SUB,"Unable to get a Log packet");
        }
     
        // Set the connection Id
        cm_rpt->cmd.pdp_act_conf.connection_id = cur_pdp->trans_id;
        //Set the Group id
        cm_rpt->cmd.pdp_act_conf.group_id = cur_pdp->group_id;
        //Set the net_qos_diff_flag
        cm_rpt->cmd.pdp_act_conf.is_net_alloc_qos_diff_from_req = cur_pdp->is_net_alloc_qos_diff_from_req;
  

        // Pack the NSAPI
        cm_rpt->cmd.pdp_act_conf.nsapi = cur_pdp->nsapi;
        // Pack PDP address     
        cm_rpt->cmd.pdp_act_conf.pdp_addr = cur_pdp->pdp_addr;
        // Pack QOS
        cm_rpt->cmd.pdp_act_conf.qos = cur_pdp->qos;
        // Pack Protocol Configuration Optoins
        cm_rpt->cmd.pdp_act_conf.config_options = cur_pdp->config_options;
  
#ifdef FEATURE_DUAL_SIM  
        cm_rpt->cmd.pdp_act_conf.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif

        /* -------------------------------------------------------------------
        ** Set the cm_pending_msg to UNKNOWN_CMD, as the CM cmd has been
        ** processed and confirmation/rejection is being sent to the CM task
        ** -----------------------------------------------------------------*/
        cur_pdp->cm_pending_msg = UNKNOWN_CMD;
        
        // Set the CM report header and the command
        cm_rpt->hdr.cmd = CM_PDP_ACTIVATE_CNF;

#ifdef FEATURE_DUAL_SIM
        if(SM_MODEM_STATISTICS_SUB1)
#endif
        {
          ps_call_info_ti = cur_pdp->trans_id;
          mm_set_signal_for_cm(SYS_NAS_PS_CALL_INFO_MASK);
        }

        MSG_HIGH_DS_2(SM_SUB, "BCM Mode for TI %d is %d", cur_pdp->trans_id, cur_pdp->bcm);
#ifdef FEATURE_LTE
        MSG_HIGH_DS_1(SM_SUB,"PDP activation, updating ESM context for %d", cur_pdp->trans_id);
        cur_pdp->esm_conn_id = esm_update_esm_context_from_sm(cur_pdp);
        cm_rpt->cmd.pdp_act_conf.esm_conn_id = cur_pdp->esm_conn_id;
#endif /*FEATURE_LTE*/
#ifdef FEATURE_DUAL_IP_BEARER
        cm_rpt->cmd.pdp_act_conf.cause_type = SM_NETWORK_CAUSE;
        cm_rpt->cmd.pdp_act_conf.cause.ext_cause = (sm_status_T)cur_pdp->current_cause;
#endif
        break;
        
      case SMREG_PDP_ACTIVATE_IND:
        /* ---------------------------------------------------------------
        *    Sending SMREG_PDP_ACTIVATE_IND to CM
        * ----------------------------------------------------------------*/
 
        //Set the Group id
        cm_rpt->cmd.pdp_act_ind.group_id = cur_pdp->group_id;
        // Pack PDP address
        cm_rpt->cmd.pdp_act_ind.pdp_addr = cur_pdp->pdp_addr;
        // Pack the APN 
        cm_rpt->cmd.pdp_act_ind.apn_name = cur_pdp->apn_name;
  
#ifdef FEATURE_REL5
        // Pack Protocol Configuration Optoins
        cm_rpt->cmd.pdp_act_ind.config_options = cur_pdp->config_options;
#endif
#ifdef FEATURE_GPRS_CALLCONTROL
        cm_rpt->cmd.pdp_act_ind.llc_sapi.valid = TRUE;
        cm_rpt->cmd.pdp_act_ind.llc_sapi.sapi = LLC_SAPI_3;
        cm_rpt->cmd.pdp_act_ind.msg_type = (byte)ACTIVATE_PDP_REQUEST;
        cm_rpt->cmd.pdp_act_ind.nsapi.nsapi = cur_pdp->nsapi.nsapi;
        cm_rpt->cmd.pdp_act_ind.sm_pd = SM_PD;
#endif
        // Set the connection Id
        cm_rpt->cmd.pdp_act_ind.connection_id = cur_pdp->trans_id;
  
#ifdef FEATURE_DUAL_SIM
        cm_rpt->cmd.pdp_act_ind.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif

        /* -------------------------------------------------------------------
        ** Set the cm_pending_msg to UNKNOWN_CMD, as the CM cmd has been
        ** processed and confirmation/rejection is being sent to the CM task
        ** -----------------------------------------------------------------*/
        cur_pdp->cm_pending_msg = UNKNOWN_CMD;
  
        // Set the CM report header and the command
        cm_rpt->hdr.cmd = CM_PDP_ACTIVATE_IND;
        break;
  
      case SMREG_PDP_DEACTIVATE_CNF:
        /* ---------------------------------------------------------------
        *    Sending SMREG_PDP_DEACTIVATE_CNF to CM
        * ----------------------------------------------------------------*/
 
        // Set the connection Id
        cm_rpt->cmd.pdp_deact_conf.connection_id = cur_pdp->trans_id;
  
        // Pack the NSAPI 
        cm_rpt->cmd.pdp_deact_conf.nsapi = cur_pdp->nsapi;
#ifdef FEATURE_REL5
        // Pack Protocol Configuration Optoins
        cm_rpt->cmd.pdp_deact_conf.config_options = cur_pdp->config_options;
#endif
  
#ifdef FEATURE_DUAL_SIM   
        cm_rpt->cmd.pdp_deact_conf.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif

        // Set the CM report header and the command
        cm_rpt->hdr.cmd = CM_PDP_DEACTIVATE_CNF;
        cur_pdp->pdp_state = PDP_INACTIVE;
#ifdef FEATURE_LTE
        MSG_HIGH_DS_1(SM_SUB,"Deleting ESM context for %d", cur_pdp->trans_id);
        (void)esm_update_esm_context_from_sm(cur_pdp);
        /* From 24.008 section 6.3 Coordination between SM and GMM for supporting ISR
        ** The MS shall change its TIN to "P-TMSI" to deactivate ISR:
        **    b)upon deactivation of the last PDP context in the MS
        */
        if((esm_isr_status_get() == TRUE) && (sm_check_if_active_pdp_exists() == FALSE))
        {
          MSG_HIGH_DS_0(SM_SUB,"No pdp context exists, Deactivate ISR on interrat to LTE");
          sm_deact_isr_on_interrat_status_set(SM_DEACT_ISR_ON_INTERRAT_TO_LTE);
        }
#endif /*FEATURE_LTE*/


        if (cur_pdp->nsapi.valid == TRUE)
        (void)sm_release_nsapi_sm (cur_pdp->trans_id);

        (void) sm_unreserve_connection_id_int(cur_pdp->trans_id);
        
        break;
  
      case SMREG_PDP_DEACTIVATE_IND:
        /* ---------------------------------------------------------------
        *    Sending SMREG_PDP_DEACTIVATE_IND to CM
        * ----------------------------------------------------------------*/

  
        // Set the connection Id
        cm_rpt->cmd.pdp_deact_ind.connection_id = cur_pdp->trans_id;
        
        // Pack the NSAPI 
        cm_rpt->cmd.pdp_deact_ind.nsapi = cur_pdp->nsapi;
        /* From 24.008 section 6.3 Coordination between SM and GMM for supporting ISR
        ** The MS shall change its TIN to "P-TMSI" to deactivate ISR:
        **    b)upon deactivation of the last PDP context in the MS
        */
#ifdef FEATURE_LTE
        if((esm_isr_status_get() == TRUE) &&(sm_check_if_active_pdp_exists() == FALSE))
        {
          MSG_HIGH_DS_0(SM_SUB,"No pdp context exists, Deactivate ISR on interrat to LTE");
          sm_deact_isr_on_interrat_status_set(SM_DEACT_ISR_ON_INTERRAT_TO_LTE);
        }
#endif

        // Pack the Cause based on if its internal or external
        cm_rpt->cmd.pdp_deact_ind.cause_type = cause_type;             
        if (cause_type == SM_INTERNAL_CAUSE)
        {
            cm_rpt->cmd.pdp_deact_ind.cause.int_cause = (sm_cm_status_T)cur_pdp->current_cause;     
        }
        else
        {
            cm_rpt->cmd.pdp_deact_ind.cause.ext_cause = (sm_status_T)cur_pdp->current_cause;
        }
        
        //Set the Default Values to backoff timer
        cm_rpt->cmd.pdp_deact_ind.backoff_timer.active = TRUE;
        cm_rpt->cmd.pdp_deact_ind.backoff_timer.timer_count = 0;
		
#ifdef FEATURE_REL5
        // Pack Protocol Configuration Optoins
        cm_rpt->cmd.pdp_deact_ind.config_options = cur_pdp->config_options;
#endif

#ifdef FEATURE_DUAL_SIM
        cm_rpt->cmd.pdp_deact_ind.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
#ifdef FEATURE_NAS_REL11
        cm_rpt->cmd.pdp_deact_ind.backoff_timer = cur_pdp->sm_gprs_type3_timer;
#endif
        // Set the CM report header and the command
        cm_rpt->hdr.cmd = CM_PDP_DEACTIVATE_IND;
        cur_pdp->pdp_state = PDP_INACTIVE;
#ifdef FEATURE_LTE
         MSG_HIGH_DS_1(SM_SUB,"Deleting ESM context for %d", cur_pdp->trans_id);
         (void)esm_update_esm_context_from_sm(cur_pdp);
#endif /*FEATURE_LTE*/

        if (cur_pdp->nsapi.valid == TRUE)
        (void)sm_release_nsapi_sm (cur_pdp->trans_id);
         
        (void) sm_unreserve_connection_id_int(cur_pdp->trans_id);
  
        break;
  
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
  
      case SMREG_PDP_DEACTIVATE_PROMOTE_IND:
  #ifdef FEATURE_SECONDARY_PDP
        /* ---------------------------------------------------------------
        *    Sending SMREG_PDP_DEACTIVATE_PROMOTE_IND to CM
        * ----------------------------------------------------------------*/

  
        // Set the connection Id
        cm_rpt->cmd.pdp_deact_prom_ind.connection_id = cur_pdp->trans_id;
        
        // Pack the NSAPI 
        cm_rpt->cmd.pdp_deact_prom_ind.nsapi = cur_pdp->nsapi;
        //Pack the promoted secondary to pri con id
        cm_rpt->cmd.pdp_deact_prom_ind.promote_con_id = cur_pdp->linked_ti;
    
        // Pack the Cause based on if its internal or external
        cm_rpt->cmd.pdp_deact_prom_ind.cause_type = cause_type;             
        if (cause_type == SM_INTERNAL_CAUSE)
        {
            cm_rpt->cmd.pdp_deact_prom_ind.cause.int_cause = (sm_cm_status_T)cur_pdp->current_cause;     
        }
        else
        {
            cm_rpt->cmd.pdp_deact_prom_ind.cause.ext_cause = (sm_status_T)cur_pdp->current_cause;
        }
#ifdef FEATURE_REL5
        // Pack Protocol Configuration Optoins
        cm_rpt->cmd.pdp_deact_prom_ind.config_options = cur_pdp->config_options;
#endif

#ifdef FEATURE_DUAL_SIM
        cm_rpt->cmd.pdp_deact_prom_ind.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif

        // Set the CM report header and the command
        cm_rpt->hdr.cmd = CM_PDP_DEACTIVATE_PROMOTE_IND;
        cur_pdp->pdp_state = PDP_INACTIVE;
#ifdef FEATURE_LTE
        MSG_HIGH_DS_1(SM_SUB,"Deleting ESM context for %d", cur_pdp->trans_id);
        (void)esm_update_esm_context_from_sm(cur_pdp);
#endif /*FEATURE_LTE*/

        if (cur_pdp->nsapi.valid == TRUE)
        (void)sm_release_nsapi_sm (cur_pdp->trans_id);

  
        (void) sm_unreserve_connection_id_int(cur_pdp->trans_id);
  #endif
          break;
  
      case SMREG_PDP_DEACTIVATE_PROMOTE_CNF:
  #ifdef FEATURE_SECONDARY_PDP
       /* ---------------------------------------------------------------
        *    Sending SMREG_PDP_DEACTIVATE_PROMOTE_CNF to CM
        * ----------------------------------------------------------------*/
  
        // Set the connection Id
        cm_rpt->cmd.pdp_deact_prom_cnf.connection_id = cur_pdp->trans_id;
        
        // Pack the NSAPI 
        cm_rpt->cmd.pdp_deact_prom_cnf.nsapi = cur_pdp->nsapi;
        //Pack the promoted secondary to pri con id
        cm_rpt->cmd.pdp_deact_prom_cnf.promote_con_id = cur_pdp->linked_ti;
#ifdef FEATURE_REL5
        // Pack Protocol Configuration Optoins
        cm_rpt->cmd.pdp_deact_prom_cnf.config_options = cur_pdp->config_options;
#endif

#ifdef FEATURE_DUAL_SIM
        cm_rpt->cmd.pdp_deact_prom_cnf.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif

        // Set the CM report header and the command
        cm_rpt->hdr.cmd = CM_PDP_DEACTIVATE_PROMOTE_CNF;
        cur_pdp->pdp_state = PDP_INACTIVE;
#ifdef FEATURE_LTE
        MSG_HIGH_DS_1(SM_SUB,"Deleting ESM context for %d", cur_pdp->trans_id);
        (void)esm_update_esm_context_from_sm(cur_pdp);
#endif /*FEATURE_LTE*/          
        if (cur_pdp->nsapi.valid == TRUE)
        (void)sm_release_nsapi_sm (cur_pdp->trans_id);

  
        (void) sm_unreserve_connection_id_int(cur_pdp->trans_id);
  #endif
      break;
#endif
  
  
      case SMREG_PDP_MODIFY_CNF:
        /* ---------------------------------------------------------------
        *    Sending SMREG_PDP_MODIFY_CNF to CM
        * ----------------------------------------------------------------*/

  
        // Log the QOS of this new PDP context to the DIAG
        if (!sm_log_qos_info(cur_pdp))
        {
              MSG_MED_DS_0(SM_SUB,"Unable to get a Log packet");
        }
  
        // Set the connection Id
        cm_rpt->cmd.pdp_modify_conf.connection_id = cur_pdp->trans_id;
  
        // Pack the NSAPI 
        cm_rpt->cmd.pdp_modify_conf.nsapi = cur_pdp->nsapi;
        // Pack the qos
        cm_rpt->cmd.pdp_modify_conf.qos = cur_pdp->qos; 
#ifdef FEATURE_REL5
        // Pack Protocol Configuration Optoins
        cm_rpt->cmd.pdp_modify_conf.config_options = cur_pdp->config_options;
#endif
        /* -------------------------------------------------------------------
        ** Set the cm_pending_msg to UNKNOWN_CMD, as the CM cmd has been
        ** processed and confirmation/rejection is being sent to the CM task
        ** -----------------------------------------------------------------*/
        if ( !(cur_pdp->cm_pending_msg != (int)SMREG_PDP_MODIFY_REQ))
        {
          MSG_HIGH_DS_2( SM_SUB," cm_cmd = %d : cm_pending_msg = %d", cm_cmd,cur_pdp->cm_pending_msg);
          cur_pdp->cm_pending_msg = UNKNOWN_CMD;
#ifdef FEATURE_LTE
          MSG_HIGH_DS_1(SM_SUB,"Modifying ESM context for %d", cur_pdp->trans_id);
          (void)esm_update_esm_context_from_sm(cur_pdp);

          // If PDP was activated before ISR then deactivate ISR
          if((esm_isr_status_get() == TRUE) && (cur_pdp->isr_pdp_act_status == CTXT_ACT_BEFORE_ISR))
          {
            MSG_HIGH_DS_0(SM_SUB,"Modified PDP created before ISR got activated , Deactivate ISR on interrat to LTE");
            sm_deact_isr_on_interrat_status_set(SM_DEACT_ISR_ON_INTERRAT_TO_LTE);
          }
#endif /*FEATURE_LTE*/
        }
  
#ifdef FEATURE_DUAL_SIM
        cm_rpt->cmd.pdp_modify_conf.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif

        // Set the CM report header and the command
        cm_rpt->hdr.cmd = CM_PDP_MODIFY_CNF;
        break;
  
      case SMREG_PDP_MODIFY_IND:
        /* ---------------------------------------------------------------
        *    Sending SMREG_PDP_MODIFY_IND to CM
        * ----------------------------------------------------------------*/
  
        cur_pdp->mt_orig_type = FALSE;    // Clear the MT indication flag 
  
        // Log the QOS of this new PDP context to the DIAG
        if (!sm_log_qos_info(cur_pdp))
        {
              MSG_MED_DS_0(SM_SUB,"Unable to get a Log packet");
        }
  
        // Set the connection Id
        cm_rpt->cmd.pdp_modify_ind.connection_id = cur_pdp->trans_id;
        //Reset the MT orig flag
        cur_pdp->mt_orig_type = FALSE;
  
        // Pack the NSAPI 
        cm_rpt->cmd.pdp_modify_ind.nsapi = cur_pdp->nsapi;
        // Pack the qos
        cm_rpt->cmd.pdp_modify_ind.qos = cur_pdp->qos;
#ifdef FEATURE_REL5
        // Pack Protocol Configuration Optoins
        cm_rpt->cmd.pdp_modify_ind.config_options = cur_pdp->config_options;
#endif

#ifdef FEATURE_DUAL_SIM
        cm_rpt->cmd.pdp_modify_ind.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif

        /* -------------------------------------------------------------------
        ** Set the cm_pending_msg to UNKNOWN_CMD, as the CM cmd has been
        ** processed and confirmation/rejection is being sent to the CM task
        ** -----------------------------------------------------------------*/
        cur_pdp->cm_pending_msg = UNKNOWN_CMD;
  
        // Set the CM report header and the command
        cm_rpt->hdr.cmd = CM_PDP_MODIFY_IND;
        cm_rpt->cmd.pdp_modify_ind.tft = cur_pdp->tft;
        MSG_HIGH_DS_2(SM_SUB, "BCM Mode for TI %d is %d", cur_pdp->trans_id, cur_pdp->bcm);
#ifdef FEATURE_LTE
        MSG_HIGH_DS_1(SM_SUB,"NW initiated Modifying ESM context for %d", cur_pdp->trans_id);
        (void)esm_update_esm_context_from_sm(cur_pdp);

        // If PDP was activated before ISR then deactivate ISR
        if((esm_isr_status_get() == TRUE) && (cur_pdp->isr_pdp_act_status == CTXT_ACT_BEFORE_ISR))
        {
          MSG_HIGH_DS_0(SM_SUB,"NW Modified PDP created before ISR got activated , Deactivate ISR on interrat to LTE");
          sm_deact_isr_on_interrat_status_set(SM_DEACT_ISR_ON_INTERRAT_TO_LTE);
        }
#endif /*FEATURE_LTE*/ 
        break;
  
  
      case SMREG_PDP_ACTIVATE_SEC_CNF:
        /* ---------------------------------------------------------------
        *    Sending CM_PDP_ACTIVATE_SEC_CNF to CM
        * ----------------------------------------------------------------*/

        cur_pdp->mt_orig_type = FALSE;    // Clear the MT indication flag
        //set the connection id
        cm_rpt->cmd.pdp_act_sec_conf.connection_id = cur_pdp->trans_id;
        //Set the net_qos_diff_flag
        cm_rpt->cmd.pdp_act_sec_conf.is_net_alloc_qos_diff_from_req = cur_pdp->is_net_alloc_qos_diff_from_req;
        //Pack the QoS
        cm_rpt->cmd.pdp_act_sec_conf.qos = cur_pdp->qos;
        //Pack the NSAPI
        cm_rpt->cmd.pdp_act_sec_conf.nsapi = cur_pdp->nsapi;
#ifdef FEATURE_REL5
        // Pack Protocol Configuration Optoins
        cm_rpt->cmd.pdp_act_sec_conf.config_options = cur_pdp->config_options;
#endif

#ifdef FEATURE_DUAL_SIM
        cm_rpt->cmd.pdp_act_sec_conf.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif

        /* -------------------------------------------------------------------
        ** Set the cm_pending_msg to UNKNOWN_CMD, as the CM cmd has been
        ** processed and confirmation/rejection is being sent to the CM task
        ** -----------------------------------------------------------------*/
        cur_pdp->cm_pending_msg = UNKNOWN_CMD;
        
        cm_rpt->hdr.cmd = CM_PDP_ACTIVATE_SEC_CNF;

        MSG_HIGH_DS_2(SM_SUB, "BCM Mode for TI %d is %d", cur_pdp->trans_id, cur_pdp->bcm);
#ifdef FEATURE_LTE
        MSG_HIGH_DS_1(SM_SUB,"Secodnary PDP activation, updating ESM context for %d", cur_pdp->trans_id);
        cur_pdp->esm_conn_id = esm_update_esm_context_from_sm(cur_pdp);
        cm_rpt->cmd.pdp_act_sec_conf.esm_conn_id = cur_pdp->esm_conn_id;
#endif /*FEATURE_LTE*/

        break;
  
      case SMREG_PDP_ACTIVATE_REJ:
        /* ---------------------------------------------------------------
        *    Sending PDP_ACTIVATE_REJ to CM
        * ----------------------------------------------------------------*/
        cur_pdp->mt_orig_type = FALSE;    // Clear the MT indication flag 
        
        // Pack the NSAPI 
        cm_rpt->cmd.pdp_act_rej.nsapi = cur_pdp->nsapi;
  
        // Set the connection Id
        cm_rpt->cmd.pdp_act_rej.connection_id = cur_pdp->trans_id;
  
        // Pack the Cause based on if its internal or external
        cm_rpt->cmd.pdp_act_rej.cause_type = cause_type;             
        if (cause_type == SM_INTERNAL_CAUSE)
        {
          cm_rpt->cmd.pdp_act_rej.cause.int_cause = (sm_cm_status_T)cur_pdp->current_cause;     
        }
        else
        {
          cm_rpt->cmd.pdp_act_rej.cause.ext_cause = (sm_status_T)cur_pdp->current_cause;
        }
        cm_rpt->cmd.pdp_act_rej.nas_max_ota_negotiated_len = nas_negotiated_ota_len;
  
        MSG_HIGH_DS(SM_SUB,"Send PDP_ACTIVATE_REJ to CM TI: %d, Cause:%d, type:%d",
                       cur_pdp->trans_id, cm_rpt->cmd.pdp_act_rej.cause.int_cause, cm_rpt->cmd.pdp_act_rej.cause_type);
  
        // Pack Protocol Configurations
        cm_rpt->cmd.pdp_act_rej.config_options = cur_pdp->config_options;
        cm_rpt->cmd.pdp_act_rej.backoff_timer = cur_pdp->sm_gprs_type3_timer;
        cm_rpt->cmd.pdp_act_rej.pdp_reject_param.apn_name = cur_pdp->apn_name;
        cm_rpt->cmd.pdp_act_rej.pdp_reject_param.pdp_type = cur_pdp->pdp_addr.pdp_type_num;        

  
        if (cur_pdp->current_cause != (cc_cause_T)INVALID_CONNECTIN_ID)
        {
          if (cur_pdp->nsapi.valid == TRUE)
          (void)sm_release_nsapi_sm (cur_pdp->trans_id);
          
          (void) sm_unreserve_connection_id_int(cur_pdp->trans_id);
        }
       
#ifdef FEATURE_DUAL_SIM
        cm_rpt->cmd.pdp_act_rej.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif

        // Set the CM report header and the command
        cm_rpt->hdr.cmd = CM_PDP_ACTIVATE_REJ;

        break;
  
      case SMREG_PDP_ACTIVATE_SEC_REJ:
        /* ---------------------------------------------------------------
        *    Sending PDP_ACTIVATE_SEC_REJ to CM
        * ----------------------------------------------------------------*/
        cur_pdp->mt_orig_type = FALSE;    // Clear the MT indication flag
  
        // Pack the NSAPI
        cm_rpt->cmd.pdp_act_sec_rej.nsapi = cur_pdp->nsapi;
  
        // Set the connection Id
        cm_rpt->cmd.pdp_act_sec_rej.connection_id = cur_pdp->trans_id;
  
        // Pack the Cause based on if its internal or external
        cm_rpt->cmd.pdp_act_sec_rej.cause_type = cause_type;
        if (cause_type == SM_INTERNAL_CAUSE)
        {
            cm_rpt->cmd.pdp_act_sec_rej.cause.int_cause = (sm_cm_status_T)cur_pdp->current_cause;
        }
        else
        {
            cm_rpt->cmd.pdp_act_sec_rej.cause.ext_cause = (sm_status_T)cur_pdp->current_cause;
        }
        cm_rpt->cmd.pdp_act_sec_rej.nas_max_ota_negotiated_len = nas_negotiated_ota_len;
        MSG_HIGH_DS(SM_SUB," Send PDP_ACTIVATE_SEC_REJ to CM for TI: %d, Cause:%d, type:%d",
                       cur_pdp->trans_id, cm_rpt->cmd.pdp_act_sec_rej.cause.int_cause, cm_rpt->cmd.pdp_act_sec_rej.cause_type);
  
#ifdef FEATURE_REL5
        // Pack Protocol Configuration Optoins
        cm_rpt->cmd.pdp_act_sec_rej.config_options = cur_pdp->config_options;
#endif
        cm_rpt->cmd.pdp_act_sec_rej.backoff_timer = cur_pdp->sm_gprs_type3_timer;
        cm_rpt->cmd.pdp_act_sec_rej.pdp_reject_param.apn_name = cur_pdp->apn_name;
        cm_rpt->cmd.pdp_act_sec_rej.pdp_reject_param.pdp_type = cur_pdp->pdp_addr.pdp_type_num; 
        if (cur_pdp->nsapi.valid == TRUE)
        (void)sm_release_nsapi_sm (cur_pdp->trans_id);

        (void) sm_unreserve_connection_id_int(cur_pdp->trans_id);
         

#ifdef FEATURE_DUAL_SIM
        cm_rpt->cmd.pdp_act_sec_rej.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif

        // Set the CM report header and the command
        cm_rpt->hdr.cmd = CM_PDP_ACTIVATE_SEC_REJ;

        break;
  
  
      case    SMREG_PDP_MODIFY_REJ:
        /* ---------------------------------------------------------------
        *    Sending PDP_MODIFY_REJ to CM
        * ----------------------------------------------------------------*/
  
        // Set the connection Id
        cm_rpt->cmd.pdp_modify_rej.connection_id = cur_pdp->trans_id;
  
        /* -------------------------------------------------------------------
        ** Set the cm_pending_msg to UNKNOWN_CMD, as the CM cmd has been
        ** processed and confirmation/rejection is being sent to the CM task
        ** -----------------------------------------------------------------*/
        cur_pdp->cm_pending_msg = UNKNOWN_CMD;
        
        // Pack the NSAPI 
        cm_rpt->cmd.pdp_modify_rej.nsapi = cur_pdp->nsapi;
        // Pack the Cause based on if its internal or external
        cm_rpt->cmd.pdp_modify_rej.cause_type = cause_type;             
        if (cause_type == SM_INTERNAL_CAUSE)
        {
            cm_rpt->cmd.pdp_modify_rej.cause.int_cause = (sm_cm_status_T)cur_pdp->current_cause;     
        }
        else
        {
            cm_rpt->cmd.pdp_modify_rej.cause.ext_cause = (sm_status_T)cur_pdp->current_cause;
        }
        cm_rpt->cmd.pdp_modify_rej.nas_max_ota_negotiated_len = nas_negotiated_ota_len;
#ifdef FEATURE_REL5
        // Pack Protocol Configuration Optoins
        cm_rpt->cmd.pdp_modify_rej.config_options = cur_pdp->config_options;
#endif
        MSG_HIGH_DS(SM_SUB,"Send PDP_MODIFY_REJ to CM for TI: %d, Cause:%d, type:%d",
                     cur_pdp->trans_id,cm_rpt->cmd.pdp_modify_rej.cause.int_cause,
                     cm_rpt->cmd.pdp_modify_rej.cause_type);

#ifdef FEATURE_DUAL_SIM
        cm_rpt->cmd.pdp_modify_rej.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
        cm_rpt->cmd.pdp_modify_rej.backoff_timer = cur_pdp->sm_gprs_type3_timer;

        // Set the CM report header and the command
        cm_rpt->hdr.cmd = CM_PDP_MODIFY_REJ;

  
        break;
  
      case    SMREG_PDP_DEACTIVATE_REJ:
        /* ---------------------------------------------------------------
        *    Sending PDP_DEACTIVATE_REJ to CM
        * ----------------------------------------------------------------*/
  
        // Set the connection Id
        cm_rpt->cmd.pdp_deact_rej.connection_id = cur_pdp->trans_id;
  
        // Pack the NSAPI 
        cm_rpt->cmd.pdp_deact_rej.nsapi = cur_pdp->nsapi;
        // Pack the Cause based on if its internal or external
        cm_rpt->cmd.pdp_deact_rej.cause_type = cause_type;             
        if (cause_type == SM_INTERNAL_CAUSE)
        {
            cm_rpt->cmd.pdp_deact_rej.cause.int_cause = (sm_cm_status_T)cur_pdp->current_cause; 
            MSG_HIGH_DS_3(SM_SUB,"Send PDP_DEACTIVATE_REJ to CM for TI: %d, Cause:%d, type:%d",
                         cur_pdp->trans_id, cm_rpt->cmd.pdp_deact_rej.cause.int_cause, 
                         cm_rpt->cmd.pdp_deact_rej.cause_type);
        }
        else
        {
            cm_rpt->cmd.pdp_deact_rej.cause.ext_cause = (sm_status_T)cur_pdp->current_cause;
            MSG_HIGH_DS_3(SM_SUB,"Send PDP_DEACTIVATE_REJ to CM for TI: %d, Cause:%d, type:%d",
                         cur_pdp->trans_id, 
                         cm_rpt->cmd.pdp_deact_rej.cause.ext_cause, 
                         cm_rpt->cmd.pdp_deact_rej.cause_type);
        }
        cur_pdp->pdp_state = PDP_INACTIVE;
#ifdef FEATURE_LTE
        MSG_HIGH_DS_1(SM_SUB,"Deleting ESM context for %d", cur_pdp->trans_id);
        (void)esm_update_esm_context_from_sm(cur_pdp);
#endif /*FEATURE_LTE*/          
        if (cur_pdp->nsapi.valid == TRUE)
        (void)sm_release_nsapi_sm (cur_pdp->trans_id);
          
        (void) sm_unreserve_connection_id_int(cur_pdp->trans_id);
  
#ifdef FEATURE_DUAL_SIM
         cm_rpt->cmd.pdp_deact_rej.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif

#ifdef FEATURE_LTE
        if((esm_isr_status_get() == TRUE) &&(sm_check_if_active_pdp_exists() == FALSE))
        {
          MSG_HIGH_DS_0(SM_SUB,"No pdp context exists, Deactivate ISR on interrat to LTE");
          sm_deact_isr_on_interrat_status_set(SM_DEACT_ISR_ON_INTERRAT_TO_LTE);
        }
#endif

        // Set the CM report header and the command
        cm_rpt->hdr.cmd = CM_PDP_DEACTIVATE_REJ;

        break;
        case SMREG_PDP_ACTIVATE_SEC_IND:
          /* ---------------------------------------------------------------
          *    Sending SMREG_PDP_ACTIVATE_SEC_IND to CM
          * ----------------------------------------------------------------*/
 
          cm_rpt->cmd.pdp_act_sec_ind.connection_id = cur_pdp->trans_id;
          cm_rpt->cmd.pdp_act_sec_ind.group_id = pdp_list[cur_pdp->linked_ti]->group_id;
          
          // Pack Protocol Configuration Optoins
          cm_rpt->cmd.pdp_act_sec_ind.config_options = cur_pdp->config_options;

          //Pack the QoS
          cm_rpt->cmd.pdp_act_sec_ind.qos = cur_pdp->qos;

          //Pack the TFT
          cm_rpt->cmd.pdp_act_sec_ind.tft = cur_pdp->tft;

#ifdef FEATURE_GPRS_CALLCONTROL
          cm_rpt->cmd.pdp_act_sec_ind.nsapi = cur_pdp->nsapi;
          cm_rpt->cmd.pdp_act_sec_ind.llc_sapi.valid = TRUE;
          cm_rpt->cmd.pdp_act_sec_ind.llc_sapi.sapi = LLC_SAPI_3;
          
          cm_rpt->cmd.pdp_act_sec_ind.msg_type = (byte)ACTIVATE_SEC_PDP_REQUEST;          
          cm_rpt->cmd.pdp_act_sec_ind.sm_pd = SM_PD;      
#endif /*FEATURE_GPRS_CALLCONTROL*/           
         
        /* -------------------------------------------------------------------
        ** Set the cm_pending_msg to UNKNOWN_CMD, as the CM cmd has been
        ** processed and confirmation/rejection is being sent to the CM task
        ** -----------------------------------------------------------------*/
        cur_pdp->cm_pending_msg = UNKNOWN_CMD;
  
        // Set the CM report header and the command
        cm_rpt->hdr.cmd = CM_PDP_ACTIVATE_SEC_IND;
#ifdef FEATURE_DUAL_SIM 
        cm_rpt->cmd.pdp_act_sec_ind.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
        break;

        case SMREG_PDP_NW_MODIFY_REQ:
          /* ---------------------------------------------------------------
          *    Sending SMREG_PDP_NW_MODIFY_REQ to CM
          * ----------------------------------------------------------------*/

          cm_rpt->cmd.pdp_nw_modify_req.connection_id = cur_pdp->trans_id;
          cm_rpt->cmd.pdp_nw_modify_req.config_options = cur_pdp->config_options;           
          cm_rpt->cmd.pdp_nw_modify_req.tft = cur_pdp->tft;
          cm_rpt->cmd.pdp_nw_modify_req.nsapi = cur_pdp->nsapi;         
          cm_rpt->cmd.pdp_nw_modify_req.qos = cur_pdp->req_qos;

          /* -------------------------------------------------------------------
          ** Set the cm_pending_msg to UNKNOWN_CMD, as the CM cmd has been
          ** processed and confirmation/rejection is being sent to the CM task
          ** -----------------------------------------------------------------*/
          cur_pdp->cm_pending_msg = UNKNOWN_CMD;
  
          // Set the CM report header and the command
          cm_rpt->hdr.cmd = CM_PDP_NW_MODIFY_REQ;
#ifdef FEATURE_DUAL_SIM
          cm_rpt->cmd.pdp_nw_modify_req.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
        break;
        
        
        default:
              MSG_ERROR_DS_0( SM_SUB,"Unknown Response type to CM");
        break;
        }
        /* Log using CFA packet */
     cfa_log_packet_ex( GS_QUEUE_CM,
                       MS_CM_SM,
                       (byte)(cm_rpt->hdr.cmd),
                       MIN(250, sizeof(cm_rpt->cmd)),
                       (byte*)&(cm_rpt->cmd) );

     MSG_HIGH_DS_0(SM_SUB,"posting command to CM");
       
     sm_add_message_to_debug_buffer(MS_CM_SM,
                                   (dword)cm_rpt->hdr.cmd,
                                    FROM_SM,
                                    (sys_modem_as_id_e_type)sm_as_id);

     cm_put_cmd(cm_rpt);
     
  }  
#ifdef FEATURE_MBMS
  #error code not present
#endif 
  
  // Unset the SM PS Vote for Reselections 
  sm_unset_ps_vote_for_reselection();

  /*If the PS siganlling connection is already released when the last was ended
   indicate the siganlling conncetion release to CM to avoid back to back PS timer 
  get kicked */
  if ( (cm_cmd == SMREG_PDP_DEACTIVATE_CNF) && 
        (ps_signalling_conn_state == FALSE) &&
        !sm_check_pdp_exist())
  {
    cm_rpt = cm_get_cmd_buf();
    if(cm_rpt == NULL)
    {
      MSG_HIGH_DS_0(SM_SUB,"Unable to get allocate CM Buffer");
      return ;
    }
    /* ---------------------------------------------------------------
    *    Sending PDP_SIG_CON_REL_IND to CM
    * ----------------------------------------------------------------*/
    //Set the cause type as INTERNAL cause
    cm_rpt->cmd.ps_sig_rel_ind.cause_type = SM_INTERNAL_CAUSE;
    // Set the CM report header and the command
    cm_rpt->hdr.cmd = CM_PS_SIG_CON_REL_IND;

#ifdef FEATURE_DUAL_SIM
    cm_rpt->cmd.ps_sig_rel_ind.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif

    /* Log using CFA packet */
    cfa_log_packet_ex( GS_QUEUE_CM,
                       MS_CM_SM,
                       (byte)(cm_rpt->hdr.cmd),
                       MIN(250, sizeof(cm_rpt->cmd)),
                       (byte*)&(cm_rpt->cmd) );

    sm_add_message_to_debug_buffer(MS_CM_SM,
                          (dword)cm_rpt->hdr.cmd,
                           FROM_SM,
                           (sys_modem_as_id_e_type)cm_rpt->cmd.ps_sig_rel_ind.as_id);
    cm_put_cmd(cm_rpt);     
  }

  if(sm_abort_cnf_pending)
  {     
     for(index = 0; index < MAX_PDP_NUM; index++)
     {
       if(pdp_list[index] != NULL)
       {
         all_pdp_inactive = FALSE;
         break;
       }       
     }
     if(all_pdp_inactive)
     {
       sm_send_cm_graceful_abort_cnf();
     }
  }

#ifdef FEATURE_DUAL_SIM
  send_gmm_ok_to_tuneaway_ind();
#endif

}
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/


/*===========================================================================
FUNCTION SM_CHECK_MPDP_CONN 

DESCRIPTION
  The functions checks any PDP waiting for GMM connection

DEPENDENCIES
  Returns true if any PDP is waiting for connection otherwise false
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean sm_check_mpdp_conn_pend(void)
{
  word index;
  for( index = 0; index < MAX_PDP_NUM; index++)
  {
    if (pdp_list[index] != NULL && 
        pdp_list[index]->pdp_con_state == SIG_CONNECTION_PENDING)
    {
      return TRUE;
    }
        
  }
  return FALSE;
}
/*===========================================================================
FUNCTION SM_GENERATE_GROUP_ID 

DESCRIPTION
  This function generates group id.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  group id

SIDE EFFECTS
   None

===========================================================================*/
byte sm_generate_group_id(void)
{
  word i, index;
  byte group_id = 1;
  boolean is_available;

  for (i = 0; i < MAX_PDP_NUM; i++)
  {
    is_available = TRUE;
    for (index = 0; index < MAX_PDP_NUM; index++)
    {
      if (pdp_list[index] != NULL && 
          pdp_list[index]->trans_id != INVALID_ID &&
          pdp_list[index]->group_id == group_id)
      {
        group_id++;
        is_available = FALSE;
        break;
      }
    }
    if (is_available)
      break;
    else
      continue;
  }
  return group_id;
}

#ifdef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
/*===========================================================================
FUNCTION SM_GET_LINKED_TI 

DESCRIPTION
  This function gets the index sharing the same APN and PDP address
  
DEPENDENCIES
  None.
  
RETURN VALUE
  index of pri pdp

SIDE EFFECTS
   None

===========================================================================*/
byte sm_get_linked_ti(byte group_id)
{
  word index;
  for (index = 0; index < MAX_PDP_NUM; index++)
  {
    if (pdp_list[index] != NULL && 
        (pdp_list[index]->group_id == group_id) &&
        (pdp_list[index]->pdp_state == PDP_ACTIVE || pdp_list[index]->pdp_state == PDP_MODIFY_PENDING) )
        return ((byte)index);
  }
  return INVALID_ID;
}

#else
/*===========================================================================
FUNCTION SM_GET_PRIMARY_INDEX 

DESCRIPTION
  This function gets the index of primary PDP
  
DEPENDENCIES
  None.
  
RETURN VALUE
  index of pri pdp

SIDE EFFECTS
   None

===========================================================================*/
byte sm_get_primary_index(byte group_id)
{
  word index;
  for (index = 0; index < MAX_PDP_NUM; index++)
  {
    if (pdp_list[index] != NULL && 
        (pdp_list[index]->group_id == group_id) && 
        (pdp_list[index]->is_primary_pdp == TRUE))
        return ((byte)index);
  }
  return INVALID_ID;
}
#endif

/*===========================================================================
FUNCTION SM_GET_NSAPI_FROM_LTI 

DESCRIPTION
  This function gets the NSAPI of the primary PDP from the linked TI
  
DEPENDENCIES
  None.
  
RETURN VALUE
  NSAPI

SIDE EFFECTS
   None

===========================================================================*/

byte sm_get_nsapi_from_lti(byte ti)
{
    if ((ti < MAX_PDP_NUM) && (pdp_list[ti] != NULL)) 
    {
      if ((pdp_list[ti]->pdp_state == PDP_ACTIVE)&&
      (pdp_list[ti]->nsapi.valid == TRUE))
      return ((byte)pdp_list[ti]->nsapi.nsapi);
    }
  
  return INVALID_ID;

}
#if 0
/*===========================================================================
FUNCTION SM_CM_DEACT_GROUP 

DESCRIPTION
  This function sends deactivate ind to cm for all the PDPs in group
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None

SIDE EFFECTS
   None

===========================================================================*/
void sm_cm_deact_group(
  pdp_context_info *pdp,
  cm_sm_cmd_type cm_cmd
)
{
  word index;
  for (index = 0; index < MAX_PDP_NUM; index++)
  {
    if (pdp_list[index] != NULL && pdp_list[index]->group_id == pdp->group_id)
    {
      switch(cm_cmd)
      {
       case SMREG_PDP_DEACTIVATE_IND:
         sm_send_cm_confirm(pdp, SMREG_PDP_DEACTIVATE_IND,
                                                    SM_NETWORK_CAUSE);
       break;
       case SMREG_PDP_DEACTIVATE_CNF:
         sm_send_cm_confirm(pdp, SMREG_PDP_DEACTIVATE_CNF,
                                                    SM_NETWORK_CAUSE);
       break;
       case SMREG_PDP_DEACTIVATE_REJ:
         sm_send_cm_confirm(pdp, SMREG_PDP_DEACTIVATE_REJ,
                                                    SM_NETWORK_CAUSE);
       break;
       default:
         MSG_ERROR_DS_1(SM_SUB,"Wrong cm command type %d", pdp->trans_id);
       break;
      }
    }
  }
}

#endif
/*===========================================================================
FUNCTION SM_IF_TFT_EXIST 

DESCRIPTION
  This function checks if already existing PDPs in the group has TFT
  
DEPENDENCIES
  None.
  
RETURN VALUE
  boolean

SIDE EFFECTS
   None

===========================================================================*/
boolean sm_if_tft_exist(byte group_id)
{
  word index;
  for (index = 0; index < MAX_PDP_NUM; index++)
  {
    if (pdp_list[index] != NULL &&
       (pdp_list[index]->group_id == group_id) && 
       (pdp_list[index]->tft.valid == FALSE))
        return FALSE;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION SM_PS_SIGNALING_REL_REQ

DESCRIPTION
  This function process ps signaling release request from CM
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None

SIDE EFFECTS
   None

===========================================================================*/

void sm_ps_signaling_rel_req
(
#ifdef FEATURE_DUAL_SIM
  smreg_ps_signalling_rel_req_cause_T cause
#else
    void
#endif
)
{
 
 if (cm_initiated_ps_signalling_release)
 {
   MSG_MED_DS_0( SM_SUB,"Rejecting CM PS signaling rel request as PS rel request is alerady pending:");    
   return; 
 }

#ifdef FEATURE_DUAL_SIM
  if (cause ==  SMREG_DUAL_STANDBY_CS_MO_CALL)
  {
    if(is_wcdma_mode() && (gmm_get_cm_ds_tuneaway_status() == TRUE))
    {
      // PS abort is not needed in case of WRRC 
      // Send confirmation to CM indicating failure.
      sm_send_cm_ps_sig_rel_confirm(SM_PS_SIG_REL_FAILURE);
      return;
    }
    if(is_tdscdma_mode() && (gmm_get_cm_ds_tuneaway_status() == TRUE))
    {
      // PS abort is not needed in case of TDSRRC 
      // Send confirmation to CM indicating failure.
      sm_send_cm_ps_sig_rel_confirm(SM_PS_SIG_REL_FAILURE);
      return;
    }
    cm_initiated_ps_signalling_release_cause = cause;
    cm_initiated_ps_signalling_release = TRUE;
    send_gmm_close_session_req();    
    return; 
 }
 else if(cause == SMREG_DUAL_TRIPLE_STANDBY_CHANGE)
 {
    cm_initiated_ps_signalling_release_cause = cause;
    cm_initiated_ps_signalling_release = TRUE;
    send_gmm_close_session_req();    
    return; 
 }
#endif
 if (timer_est_pen_flag)
 {
   sm_send_cm_ps_sig_rel_confirm(SM_PS_SIG_REL_FAILURE);
   MSG_MED_DS_0( SM_SUB,"Rejecting CM PS signaling rel request as Signaling connection establish is pending");    
   return;
 }
 if (sm_check_pdp_procedure_pending())
 {
      sm_send_cm_ps_sig_rel_confirm(SM_PS_SIG_REL_FAILURE);
      MSG_MED_DS_0( SM_SUB,"Rejecting CM PS signaling rel request as SM procedure is pending");    
      return;
 }
 if (rabm_check_procedure_pending())
 {
   sm_send_cm_ps_sig_rel_confirm(SM_PS_SIG_REL_FAILURE);
   MSG_MED_DS_0( SM_SUB,"Rejecting CM PS signaling rel request as RABM procedure is pending");    
   return;
 }

 cm_initiated_ps_signalling_release = TRUE;
#ifdef FEATURE_DUAL_SIM
 cm_initiated_ps_signalling_release_cause = cause;
#endif
 send_gmm_fast_dormancy_req();
 return;
}

/*===========================================================================
FUNCTION SM_SEND_CM_PS_SIG_REL_CONFIRM
DESCRIPTION
  Sends a PS signal release confirmation to the Call Manager.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None

SIDE EFFECTS
   None

===========================================================================*/

void sm_send_cm_ps_sig_rel_confirm(ps_sig_rel_status_T status_t)
{
  cm_rpt_type *cm_rpt;   
  smreg_ps_sig_rel_cnf_T      ps_sig_rel_cnf;
  memset((void *)&ps_sig_rel_cnf, 0, sizeof(smreg_ps_sig_rel_cnf_T));
   // Get the Call Manager's command report for posting the command.
  cm_rpt = cm_get_cmd_buf();
  MSG_HIGH_DS_0( SM_SUB," Sending CM_PS_SIG_CON_REL_CNF to CM ");

  /* ---------------------------------------------------------------
   *    Sending PDP_SIG_CON_REL_CNF to CM
  * ----------------------------------------------------------------*/    
  ps_sig_rel_cnf.status = status_t;    
#ifdef FEATURE_DUAL_SIM  
  ps_sig_rel_cnf.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
  if(cm_rpt!=NULL)
  {
    // Set the CM report header and the command
    cm_rpt->hdr.cmd = CM_PS_SIG_CON_REL_CNF;
    cm_rpt->cmd.ps_sig_rel_cnf = ps_sig_rel_cnf;
    
    /* Log using CFA packet */
    cfa_log_packet_ex( GS_QUEUE_CM,
                       MS_CM_SM,
                      (byte)(cm_rpt->hdr.cmd),
                       MIN(250, sizeof(cm_rpt->cmd)),
                      (byte*)&(cm_rpt->cmd) );

    sm_add_message_to_debug_buffer(MS_CM_SM,
                          (dword)cm_rpt->hdr.cmd,
                           FROM_SM,
                           (sys_modem_as_id_e_type)cm_rpt->cmd.ps_sig_rel_cnf.as_id);
    
    cm_put_cmd(cm_rpt);
    return;           
  }
  else
  {
    MSG_ERROR_DS_0(SM_SUB,"CM Error: Not able to Allocate CM Buffer");
  }
 
}


#ifdef FEATURE_MBMS
#error code not present
#endif


/*===========================================================================

FUNCTION SM_GRACEGUL_PDP_ABORT_REQ

DESCRIPTION

  This function processes the abort command received from CM task 
   
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

  None.
  
===========================================================================*/

void sm_graceful_pdp_abort_req(void)
{
  word index;
  word sec_index;

//TODO: in case DUAL_DATA NOT defined
//#ifdef FEATURE_DUAL_SIM
//#ifndef FEATURE_DUAL_DATA
//  if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.pdp_graceful_abort_req.as_id) == FALSE)
//  {
//    MSG_HIGH_DS_2( SM_SUB,"SM AS ID Conflict sm_as_id : %d, message as_id: %d", sm_as_id , cmd_ptr->cmd.pdp_graceful_abort_req.as_id);
//    return;
//  }
//#endif
//#endif
           
  MSG_HIGH_DS_0( SM_SUB,"SM received Graceful PDP Abort Request");
  
#ifdef FEATURE_GSM_GPRS      
  pending_sm_pdu_id = INVALID_ID;
#endif
  //Reset the timer_est_pen flag
  timer_est_pen_flag = FALSE;  

  // Set the Signaling Connection variable as False
  sig_connection_available = FALSE;
  max_est_timer_count = 0;

  for (index = 0; index < MAX_PDP_NUM; index++)
  {
    if(pdp_list[index] != NULL)
    {       
      // for all PDPs in active pending state do local deactivation
      if(pdp_list[index]->pdp_state == PDP_ACTIVE_PENDING)
      {
        MSG_HIGH_DS_1(SM_SUB,"PDP Abort - PDP %d in active pending, local cleanup", index);
        sm_perform_local_pdp_deactivation(pdp_list[index]);    
      }
      //PDPs for which deactivation has been sent set timer count to MAX
      else if (pdp_list[index]->pdp_state == PDP_INACTIVE_PENDING && 
                   ((pdp_list[index]->pdp_con_state == SIG_CONNECTION_PENDING ||
                        sm_timer_status[index] != (word)SM_TIMER_STOPPED)))
      {
        MSG_HIGH_DS_1(SM_SUB,"PDP Abort -PDP %d in incative pending, set timer count to max", index); 
        sm_max_timer_count[index] = MAX_TIMER_CNT;
      }
      else if(pdp_list[index]->pdp_state == PDP_INACTIVE)
      {
         if (pdp_list[index]->mt_orig_type == TRUE)
         {
             sig_connection_available = FALSE;
             // Set the cause of the rejection in the PDP context
             pdp_list[index]->current_cause = (cc_cause_T)SM_ACTIVATION_REJECTED_UNSPECIFIED;             

             //Set the CM pending message to SMREG_PDP_ACTIVATE_REJ_RSP
             pdp_list[index]->cm_pending_msg = (int)SMREG_PDP_ACTIVATE_REJ_RSP;
             sm_continue_pdp_activate_rej_rsp (pdp_list[index]);
         }
         else if (pdp_list[index]->cm_pending_msg == (int)SMREG_PDP_ACTIVATE_REQ )
         {
           #if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
           sm_send_cm_confirm(pdp_list[index], SMREG_PDP_ACTIVATE_REJ, SM_INTERNAL_CAUSE);
           #endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
         }
         else if (pdp_list[index]->cm_pending_msg == (int)SMREG_PDP_ACTIVATE_SEC_REQ )
         {
            #if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
            sm_send_cm_confirm(pdp_list[index], SMREG_PDP_ACTIVATE_SEC_REJ, SM_INTERNAL_CAUSE);
            #endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/
         }
      }
      
    }
  }

  /*For all prmiary PDPs in active or modify pending state 
  send deactivaiton request with TI set as true*/
  for (index = 0; index < MAX_PDP_NUM; index++)
  {
     if(pdp_list[index] != NULL)
     {
       if((pdp_list[index]->pdp_state == PDP_ACTIVE ||
                  pdp_list[index]->pdp_state == PDP_MODIFY_PENDING)
#ifndef FEATURE_SECONDARY_PDP_PROMOTION_REMOVAL
                  && pdp_list[index]->is_primary_pdp
#endif
                  )
       {
         MSG_HIGH_DS_1(SM_SUB,"PDP Abort - Deactivating primary pdp %d", index); 
   #ifdef FEATURE_MBMS
         #error code not present
#endif       
         sig_connection_available = FALSE; //Set the Signalling connection to FALSE 
  
         /*Stop the timer in case any timer is running*/
         sm_stop_timer(pdp_list[index]->trans_id);
         sm_max_timer_count[index] = MAX_TIMER_CNT; 
         // Set the cause of the deactivation in the PDP context
         pdp_list[index]->current_cause = (cc_cause_T)SM_REGULAR_DEACTIVATION;
         
         pdp_list[index]->pdp_state = PDP_INACTIVE_PENDING;
  
         pdp_list[index]->cm_pending_msg = (int)SMREG_PDP_GRACEFUL_ABORT_REQ;
   
         if (is_umts_mode())
         {
           pdp_list[index]->rab_state = RABM_INACTIVE_PENDING;
           sm_notify_rabm ((int)RABMSM_DEACTIVATE_IND, pdp_list[index]->nsapi.nsapi);
         }
  
         pdp_list[index]->teardown_ind.valid = TRUE;
         pdp_list[index]->teardown_ind.ti_flag = TRUE;  
  
         pdp_list[index]->config_options.valid = FALSE;
  
         /*For all secondary PDPs set state to incative pending */      
         for (sec_index = 0; sec_index < MAX_PDP_NUM; sec_index++)
         {
           if(pdp_list[sec_index] != NULL)
           {
             if ((index != sec_index) &&                  
                 (pdp_list[index]->group_id == pdp_list[sec_index]->group_id) &&
                 (pdp_list[sec_index]->pdp_state != PDP_INACTIVE))
             {
                //If the profile matches change the state to PDP_INACTIVE_PENDING
           MSG_HIGH_DS_1( SM_SUB,"PDP Abort : Secondary PDP state changed to INACTIVE_PENDING %d", sec_index);
   #ifdef FEATURE_MBMS
               #error code not present
#endif       
               pdp_list[sec_index]->pdp_state = PDP_INACTIVE_PENDING;
    
               pdp_list[sec_index]->current_cause = (cc_cause_T)SM_REGULAR_DEACTIVATION;
               //Change the RAB state to RAB_INTIAL
               pdp_list[sec_index]->rab_state = RABM_INITIAL;          
               if (is_umts_mode())
               {
                  pdp_list[sec_index]->rab_state = RABM_INACTIVE_PENDING;
                  sm_notify_rabm ((int)RABMSM_DEACTIVATE_IND, pdp_list[sec_index]->nsapi.nsapi);
               }
             }
           }
         }
         //send deactivate request for primary PDP
         sm_continue_pdp_deactivate(pdp_list[index]);
        
       }
     }
  }

  sm_abort_cnf_pending = FALSE;
  for(index = 0; index < MAX_PDP_NUM; index++)
  {
     if(pdp_list[index] != NULL)
     {
       sm_abort_cnf_pending = TRUE;
       break;
     }
  }
  /* If all PDPs are in Inactive State send abort confirm to CM*/ 
  if(!sm_abort_cnf_pending)
  {
    sm_send_cm_graceful_abort_cnf();
  }  
}


/*===========================================================================

FUNCTION SM_SEND_CM_GRACEFUL_ABORT_CNF

DESCRIPTION

  This function send abort confrim to CM task
   
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

  None.
  
===========================================================================*/

void sm_send_cm_graceful_abort_cnf(void)
{
  cm_rpt_type *cm_rpt;     
  // Get the Call Manager's command report for posting the command.
  cm_rpt = cm_get_cmd_buf();
  MSG_HIGH_DS_0( SM_SUB,"Sending PDP Abort Confirm to CM");
  
  // Set the CM report header and the command
  cm_rpt->hdr.cmd = CM_PDP_GRACEFUL_ABORT_CNF;    
#ifdef FEATURE_DUAL_SIM
  cm_rpt->cmd.pdp_graceful_abort_cnf.as_id = (sys_modem_as_id_e_type)sm_as_id;
#endif
  /* Log using CFA packet */
  cfa_log_packet_ex( GS_QUEUE_CM,
                    MS_CM_SM,
                    (byte)(cm_rpt->hdr.cmd),
                    MIN(250, sizeof(cm_rpt->cmd)),
                   (byte*)&(cm_rpt->cmd) );

  sm_add_message_to_debug_buffer(MS_CM_SM,
                          (dword)cm_rpt->hdr.cmd,
                           FROM_SM,
                           (sys_modem_as_id_e_type)cm_rpt->cmd.pdp_graceful_abort_cnf.as_id);
    
  cm_put_cmd(cm_rpt);
  sm_abort_cnf_pending = FALSE;
             
}

/*===========================================================================

FUNCTION SM_SEND_CM_GRACEFUL_ABORT_DUMMY_CNF

DESCRIPTION

  This function sends dummy abort confrim to CM task when as_id mismatches.
   
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

  None.
  
===========================================================================*/

void sm_send_cm_graceful_abort_dummy_cnf(sys_modem_as_id_e_type as_id)
{
  cm_rpt_type *cm_rpt;     
  // Get the Call Manager's command report for posting the command.
  cm_rpt = cm_get_cmd_buf();
  MSG_HIGH_DS_0( SM_SUB,"Sending Graceful PDP Abort Confirm to CM");
  
  // Set the CM report header and the command
  cm_rpt->hdr.cmd = CM_PDP_GRACEFUL_ABORT_CNF;    
  cm_rpt->cmd.pdp_graceful_abort_cnf.as_id = as_id;

  /* Log using CFA packet */
  cfa_log_packet_ex( GS_QUEUE_CM,
                    MS_CM_SM,
                    (byte)(cm_rpt->hdr.cmd),
                    MIN(250, sizeof(cm_rpt->cmd)),
                   (byte*)&(cm_rpt->cmd) );
    
  cm_put_cmd(cm_rpt);
             
}



/*===========================================================================
FUNCTION SM_PDP_NW_MODIFY_CNF

DESCRIPTION
  This function processes the PDP context NW modification confirmation
  
DEPENDENCIES
  It is assumed that the Call Manager provides correct connection Id.
  
RETURN VALUE

SIDE EFFECTS

  This function needs to make sure that the GPRS attach is already in effect
  before attempting to send the L3 message to the network.
===========================================================================*/

void sm_pdp_nw_modify_cnf(byte con_id)
{

//TODO:
//#ifdef FEATURE_DUAL_SIM
//#ifndef FEATURE_DUAL_DATA
//  if(SM_IS_VALID_AS_ID(cmd_ptr->cmd.pdp_nw_modify_cnf.as_id) == FALSE)
//   {
//      MSG_HIGH_DS_2( SM_SUB,"SM AS ID Conflict sm_as_id : %d, message as_id: %d", sm_as_id, cmd_ptr->cmd.pdp_nw_modify_cnf.as_id);
//      return;
//   }
//#endif
//#endif

  pdp_context_info *cur_pdp; // current pdp context in the transaction
  cur_pdp = pdp_list[con_id];
  
  sig_connection_available = FALSE;
  
  if (cur_pdp != NULL && 
      cur_pdp->pdp_state == PDP_MODIFY_PENDING && cur_pdp->cm_pending_msg == UNKNOWN_CMD)
  {
    cur_pdp->cm_pending_msg = (int)MS_MODIFY_PDP_ACCEPT;
    sm_continue_pdp_nw_modify_cnf(cur_pdp);
  }
}

/*===========================================================================
FUNCTION SM_PDP_NW_MODIFY_REJ

DESCRIPTION
  This function processes the PDP context NW modification rej
  
DEPENDENCIES
  It is assumed that the Call Manager provides correct connection Id.
  
RETURN VALUE

SIDE EFFECTS

  This function needs to make sure that the GPRS attach is already in effect
  before attempting to send the L3 message to the network.
===========================================================================*/

void sm_pdp_nw_modify_rej (byte con_id,
          smreg_pdp_nw_modify_rej_T *pdp_nw_mod_rej)
{
   pdp_context_info *cur_pdp =NULL; // current pdp context in the transaction

#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
   if(SM_IS_VALID_AS_ID(pdp_nw_mod_rej->as_id) == FALSE)  
   { 
     MSG_HIGH_DS_2( SM_SUB,"SM AS ID Conflict sm_as_id : %d, message as_id: %d", sm_as_id, pdp_nw_mod_rej->as_id);
     return;  
   }
#endif
#endif

 

  cur_pdp = pdp_list[con_id];

  MSG_MED_DS_1(SM_SUB, "PDP NW Modify reject for conid %d", con_id);
  //Set the Signalling connection to FALSE
  sig_connection_available = FALSE;  
  if (cur_pdp != NULL && 
      cur_pdp->pdp_state == PDP_MODIFY_PENDING && cur_pdp->cm_pending_msg == UNKNOWN_CMD)
  {
    cur_pdp->current_cause = (cc_cause_T)pdp_nw_mod_rej->cause;

    cur_pdp->cm_pending_msg = (int)MODIFY_PDP_REJECT;
    
    sm_continue_pdp_nw_modify_rej(cur_pdp);
  }
  
}

/*===========================================================================
FUNCTION SM_CONTINUE_PDP_NW_MODIFY_REJ 

DESCRIPTION
  This function processes PDP context Network modify reject after receiving the 
  a successful GPRS attach indication from GMM. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void sm_continue_pdp_nw_modify_rej(pdp_context_info* cur_pdp)
{
  byte *l3_buf ;
  word mesg_bit_len;
  xlate_status_T status;          /* status of the packed L3 message */

 
  /* ----------------------------------------------------------------------
  ** Make sure the RRC Signaling Connection is available, before sending
  ** the SM Signaling message. Request GMM to establish a connection if it
  ** is not available.
  ** ---------------------------------------------------------------------*/
  if (!sig_connection_available)
  {    
    MSG_HIGH_DS_0(SM_SUB, "Send GMMSM_ESTABLISH_REQ, REQ CONN ");

    // Change the PDP SIG connect state to SIG_CONNECTION_PENDING
    cur_pdp->pdp_con_state = SIG_CONNECTION_PENDING;

#ifdef FEATURE_DUAL_DATA
    if ( (!timer_est_pen_flag) || (sm_timer_status[cur_pdp->trans_id] == (word)SM_TIMER_STOPPED) )
#else
    if ( !timer_est_pen_flag )
#endif
    {
      #ifdef FEATURE_DUAL_DATA
      sm_start_timer(cur_pdp->trans_id, TIMER_ESTABLISH_PENDING);
      #else
      timers_pending[TIMER_PENDING] = cur_pdp->trans_id;
      sm_set_pdp_timer(TIMER_ESTABLISH_PENDING);
      #endif 

      //Set the timer_est_pen_flag
      timer_est_pen_flag = TRUE;
      //sm_start_timer(pdp->trans_id, TIMER_ESTABLISH_PENDING)

      // Send an Establish Request GMM task to send 
      send_gmm_establish_req(RRC_EST_HI_PRI_SIGNALLING);
    }
    else
    {
       max_est_timer_count = 0;
    }
  }
  else
  {
#ifndef FEATURE_MODEM_HEAP
    l3_buf = (uint8*) gs_alloc(gmm_max_L3_msg_length(MM_PS_DOMAIN));
#else
    l3_buf = (uint8*) modem_mem_calloc(1, gmm_max_L3_msg_length(MM_PS_DOMAIN), MODEM_MEM_CLIENT_NAS);
#endif
    sm_check_for_null_ptr((void*)l3_buf); 
    status = pack_layer3_mesg(&mesg_bit_len, l3_buf, cur_pdp, MODIFY_PDP_REJECT);
      
    cur_pdp->pdp_state = PDP_ACTIVE;
      
    cur_pdp->req_radio_pri.valid = FALSE;
    cur_pdp->req_llc_sapi.valid = FALSE;
    cur_pdp->req_qos.valid = FALSE;
    cur_pdp->req_flow_id.valid = FALSE;
      
    if (status == L3_PARSE_SUCCESS)
    {
      // Send the Modify PDP accept message to network
      MSG_HIGH_DS_0(SM_SUB, "Sending SM-PDU to GMM ");
      send_gmm_smpdu(mesg_bit_len, l3_buf, FALSE,FALSE);                   
    }  
#ifdef FEATURE_MODEM_HEAP
    modem_mem_free( l3_buf , MODEM_MEM_CLIENT_NAS);
    l3_buf = NULL;
#else
    gs_free( l3_buf);
    l3_buf = NULL;
#endif
  }
}

/*===========================================================================
FUNCTION SM_CONTINUE_PDP_NW_MODIFY_CNF 

DESCRIPTION
  This function processes PDP context Network modify cnf after receiving the 
  a successful GPRS attach indication from GMM. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void sm_continue_pdp_nw_modify_cnf(pdp_context_info* cur_pdp)
{

  byte *l3_buf;

  word mesg_bit_len;
  xlate_status_T status;          /* status of the packed L3 message */
  word index;
  
  /* ----------------------------------------------------------------------
  ** Make sure the RRC Signaling Connection is available, before sending
  ** the SM Signaling message. Request GMM to establish a connection if it
  ** is not available.
  ** ---------------------------------------------------------------------*/
  if (!sig_connection_available)
  {    
    MSG_HIGH_DS_0(SM_SUB, "Send GMMSM_ESTABLISH_REQ, REQ CONN ");

    // Change the PDP SIG connect state to SIG_CONNECTION_PENDING
    cur_pdp->pdp_con_state = SIG_CONNECTION_PENDING;

#ifdef FEATURE_DUAL_DATA
    if ( (!timer_est_pen_flag) || (sm_timer_status[cur_pdp->trans_id] == (word)SM_TIMER_STOPPED) )
#else
    if ( !timer_est_pen_flag )
#endif
    {
      #ifdef FEATURE_DUAL_DATA
      sm_start_timer(cur_pdp->trans_id, TIMER_ESTABLISH_PENDING);
      #else
      timers_pending[TIMER_PENDING] = cur_pdp->trans_id;
      sm_set_pdp_timer(TIMER_ESTABLISH_PENDING);
      #endif 

      //Set the timer_est_pen_flag
      timer_est_pen_flag = TRUE;
      //sm_start_timer(pdp->trans_id, TIMER_ESTABLISH_PENDING)

      // Send an Establish Request GMM task to send 
      send_gmm_establish_req(RRC_EST_HI_PRI_SIGNALLING);
    }
    else
    {
       max_est_timer_count = 0;
    }
  }
  else
  {
#ifndef FEATURE_MODEM_HEAP
    l3_buf = (uint8*) gs_alloc(gmm_max_L3_msg_length(MM_PS_DOMAIN));
#else
    l3_buf = (uint8*) modem_mem_calloc(1, gmm_max_L3_msg_length(MM_PS_DOMAIN), MODEM_MEM_CLIENT_NAS);
#endif
    sm_check_for_null_ptr((void*)l3_buf);

    /* Pack the Layer 3 Message and send it to GMM */
    status = pack_layer3_mesg(&mesg_bit_len, l3_buf, cur_pdp, MS_MODIFY_PDP_ACCEPT);
            
    if (status == L3_PARSE_SUCCESS)
    {
      // Send the Modify PDP accept message to network
      MSG_HIGH_DS_0(SM_SUB, "Sending SM-PDU to GMM ");
      send_gmm_smpdu(mesg_bit_len, l3_buf, FALSE,FALSE);       
              
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
      cur_pdp->sm_msg_to_resend = MS_MODIFY_PDP_ACCEPT;
      cur_pdp->status_ind_flag = FALSE;
#endif

      /*Copy data from temporary locations*/
      cur_pdp->radio_pri = cur_pdp->req_radio_pri;
      cur_pdp->llc_sapi = cur_pdp->req_llc_sapi;
      cur_pdp->qos = cur_pdp->req_qos;
      if(cur_pdp->req_flow_id.valid)
      {
        cur_pdp->flow_id = cur_pdp->req_flow_id; 
      }  
      
      cur_pdp->req_radio_pri.valid = FALSE;
      cur_pdp->req_llc_sapi.valid = FALSE;
      cur_pdp->req_qos.valid = FALSE;
      cur_pdp->req_flow_id.valid = FALSE;
      
      cur_pdp->bcm = sm_get_bcm_from_pco(&cur_pdp->config_options);
      
      /* Set BCM of all PDP in group same as cur pdp*/
      for (index = 0; index < MAX_PDP_NUM; index++)
      {
        if ((index != cur_pdp->trans_id) && 
            (pdp_list[index] != NULL) &&
            (cur_pdp->group_id == pdp_list[index]->group_id) &&
            (pdp_list[index]->pdp_state != PDP_INACTIVE))
        {
          pdp_list[index]->bcm = cur_pdp->bcm;
          MSG_HIGH_DS_2(SM_SUB, "BCM Mode for TI %d is %d", index, pdp_list[index]->bcm);
        }
      }
           
           
#ifdef FEATURE_GSM_GPRS
      if (is_gsm_mode())
      {
        cur_pdp->pdp_state = PDP_MODIFY_PENDING;
        /* ------------------------------------------------------------------
         ** Check if the UE is running in GPRS mode of operation and send
         ** GSNSM_MODIFY_IND to the SNDCP.
        ** -----------------------------------------------------------------*/
        sm_send_sndcp_cmd (cur_pdp, GSNSM_MODIFY_IND);  
       
        /* Set the llc_state to LLC_CONFIRM_PENDING, start sanity timer */
        cur_pdp->llc_state = LLC_MT_MODIFY_CNF_PENDING;
      
        /* ---------------------------------------------------------------------
         The PDP context has been activated, but LLC RAB has to be established
         ** before sending the SMREG_PDP_MODIFY_IND primitive to CM.
           ** --------------------------------------------------------------------*/      
        MSG_HIGH_DS_1(SM_SUB,"PDP MODIFIED %d, Waiting LLC Confirm",cur_pdp->trans_id);
      }
      
#endif /*FEATURE_GSM_GPRS*/
        
      if (is_umts_mode())
      {
        MSG_HIGH_DS_0(SM_SUB, "UMTS mode, send modify indication of CM ");
        /* -------------------------------------------------------------------
           6.1.3.3.1 : "if the PDP context is preserved, then the network may re-establish a 
           Radio Access Bearer with the new QoS indicated in the MODIFY PDP CONTEXT 
           REQUEST message."
           So it is not mandatory for NW to establsih RAB as part of modify procedure
        ** ------------------------------------------------------------------*/
          
          // set pdp context to active, because it is Mobile Terminated. 
          cur_pdp->pdp_state = PDP_ACTIVE;
      
          // Send the SMREG_PDP_MODIFY_IND to the CM for Mobile terminated req.
#if defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)
          sm_send_cm_confirm(cur_pdp, SMREG_PDP_MODIFY_IND, SM_NETWORK_CAUSE);
#endif /*defined (FEATURE_GSM_GPRS) || defined (FEATURE_WCDMA)*/

      }
    }  
    if(l3_buf != NULL)
    {
#ifdef FEATURE_MODEM_HEAP
      modem_mem_free( l3_buf , MODEM_MEM_CLIENT_NAS);
      l3_buf = NULL;
#else
      gs_free( l3_buf);
      l3_buf = NULL;
#endif
    }
  }
}



/*===========================================================================
FUNCTION SM_PRO_BLOCKED_APN_LIST 

DESCRIPTION
  This function overwrites the existing SM APN blocking list with one provide in this  funtion call.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void sm_process_blocked_apn_list(smreg_blocked_apn_list_ind_T *blocked_apn_list_ind)
{
     int index;
#ifdef FEATURE_DUAL_SIM
#ifndef FEATURE_DUAL_DATA
     if(SM_IS_VALID_AS_ID(blocked_apn_list_ind->as_id) == FALSE)
     {
        MSG_HIGH_DS_2( SM_SUB,"SM AS ID Conflict sm_as_id : %d, message as_id: %d", sm_as_id, blocked_apn_list_ind->as_id);
        return ;
     }
#endif
#endif

     if(blocked_apn_list_ind->length > MAX_BLOCKED_APN)
     {
        MSG_ERROR_DS_1( SM_SUB,"Number of blocked APN exceed limit %d no bloked apn", blocked_apn_list_ind->length );
        return ;
     }

     (void) memset(&sm_blocked_apn_list,0,sizeof(sm_blocked_apn_list_T));
     sm_blocked_apn_list.length = blocked_apn_list_ind->length;

     for (index = 0; index < blocked_apn_list_ind->length; index++)
     { 
       if(blocked_apn_list_ind->apn_name[index].valid != TRUE)
       {
         MSG_HIGH_DS_1( SM_SUB,"Invalid APN %d", index );
         return ;
       }       
       sm_blocked_apn_list.apn_name[index] = blocked_apn_list_ind->apn_name[index];
     }
}

/*===========================================================================
FUNCTION SM_UNBLOCK_ALL_APN

DESCRIPTION
  This function sends CM_UNBLOCK_ALL_APN to CM

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS

===========================================================================*/
void sm_unblock_all_apn(sys_modem_as_id_e_type as_id)
{
  cm_rpt_type *cm_rpt;   

  sm_blocked_apn_list.length = 0;

   // Get the Call Manager's command report for posting the command.
  cm_rpt = cm_get_cmd_buf();
  MSG_HIGH_DS_0( SM_SUB," Sending CM_UNBLOCK_ALL_APN to CM ");

  if(cm_rpt!=NULL)
  {
    // Set the CM report header and the command
    cm_rpt->hdr.cmd = CM_UNBLOCK_ALL_APN;
    #ifdef FEATURE_DUAL_SIM
    cm_rpt->cmd.unblock_all_apn.as_id = as_id;
    #endif
    
    /* Log using CFA packet */
    cfa_log_packet_ex( GS_QUEUE_CM,
                       MS_CM_SM,
                      (byte)(cm_rpt->hdr.cmd),
                       MIN(250, sizeof(cm_rpt->cmd)),
                      (byte*)&(cm_rpt->cmd) );

    sm_add_message_to_debug_buffer(MS_CM_SM,
                      (dword)cm_rpt->hdr.cmd,
                       FROM_SM,
                       (sys_modem_as_id_e_type)cm_rpt->cmd.unblock_all_apn.as_id);

    cm_put_cmd(cm_rpt);
    return;           
  }
  else
  {
    MSG_ERROR_DS_0(SM_SUB,"CM Error: Not able to Allocate CM Buffer");
  }
 
}
void sm_check_for_null_ptr( void* msg_p)
{
   if(msg_p == NULL)
   {
     MSG_FATAL_DS(SM_SUB,"=SM= Error: Not able to Allocate MM Buffer", 0, 0, 0);
   }
}

