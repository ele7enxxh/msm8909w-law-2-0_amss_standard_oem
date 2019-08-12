/*===========================================================================

                                  R R C R C R

GENERAL DESCRIPTION
 The RRCRCR module consists of RRC CONNECTION RELEASE PROCEDURE 
 implementation. The 3GPP TS 25.331 RRC Protocol specification is used as
 basis for implementation.

EXTERNALIZED FUNCTIONS

  rrcrcr_init_procedure()
    This function will initialize the RRC Connection Release 
    procedure substate to RRCRCR_INITIAL Substate. This will be called in
    the RRCTASK modules along with other Init Procedures.

  rrcrcr_procedure_event_handler()

    This is the entry point for RRC Connection Release procedure. 
    All events for RCR come to this function from RRC Dispatcher. 
    Depending on the UE state of RRC Connection Release Procedure, it
    calls the corresponding state handler. These state handlers call
    substate handlers depending on the the current substate of RRC
    Connection Release procedure.

Copyright (c) 2001-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcrcr.c_v   1.33   02 Jul 2002 17:17:18   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcrcr.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/26/15   sp      Made changes to add call end metrics - FR#30856
03/20/15   sp      Made changes to convert ota metric msg_id to enum
03/20/15   sp      Made changes to log serving cell QSH metrics at metric config 
03/13/15   ad      Made changes to log important parms from OTAs
02/24/15   sp      Made changes for Mav debug snapshot
01/28/14   bc      Changes to try ACQ on the frequency provided in connection release during leaving
                   connected mode scan when rplmn info is present in connection release.
09/03/14   sp      Made changes to save procedure substates in crash debug info
08/08/14   sg      Made changes to check URNTI in TOC also if it is not 
                   present on config ptr in use
08/04/14   vi      Made changes to avoid compiler warnings
07/24/14   sp      Made changes to skip saving RCR OTA in crash debug global if there is identity mismatch
07/17/14   vi      Made changes to add WtoW and WtoG Redirection events
07/11/14   vi      Made changes to convert earfcn type from uint16 to unti32
06/24/14   bc      Added code to expose rrcrcr_is_proc_active_sub API to NAS.
06/23/14   sr      Made changes to unbar frequency info given in IE "redirection info" for 
                   connection reject and connection release OTA
05/02/14   gv      Made changes to attempt IRAT activity only after clean up of pending BPLMN
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
08/14/13   vg      Made changes to implement a new diag event EVENT_WCDMA_RRC_FAILURE
06/25/13   sn      Block IRAT functionality in manual CSG mode
03/22/13   mp      Removed tmc.h include file
01/03/13   ad      Made changes to reduce the redundant F3s
10/14/12   gv      Added code to support exchange of AS ID between WRRC and GRR commands
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/02/12   as      Made changes to udpate release cause after UE id validation
12/09/11   ad      Added code to log WCDMA_RRC_PROTOCOL_ERRORS packet
06/29/11   rl      Fix to NULL check when the fdd_UMTS_Frequency_ListPresent is not present.
05/03/11   rl      Fixed the lint error
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/01/11   vg      made changes for RRC to initate cell selection or redirect
                   on the best cell of current camp freq if that freq comes
                   in RRC Conn rel rplmn or redirection IE. 
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
01/12/11   rl      Migrated to VU_MODEM_WCDMA_RRC.01.88.50
01/05/11   sks     Compilation warning fix.
12/28/10   sks     Made changes to validate EARFCN's provided in LTOW redirection info from n/w.
01/04/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.87.50
12/17/10   sks     Made changes to stop "wait for L2 ack" timer for a new RRC connection release msg
                   only if UE ID check passes for the new msg.
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/11/10   ss      ASN.1 migration to ASN1 9.3.0
10/08/10   av      Fixing compilation errors in RRC
10/08/10   sks     Made changes to access fdd_UMTS_Frequency_List in rplmn_information
                   only when the corresponding bitmask is present.
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
04/07/10   gkg     ASN.1 migration to 8.a.0
03/15/10   as      Updated RRC Code to support ASN.1 8.9 version
11/13/09   sks     Added a check on the max number of GSM cells to be copied from the OTA message.
11/05/02   sks     Fixed compilation error.
11/04/09   sks     Added code to support inter-RAT redirection via connection reject
                   and both inter-RAT and inter-freq redirection via connection release.
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
05/14/09   gkg     Made changes so that RCR doesn't handle any command if RRC
                   state is DISCONNECTED and RCR is in initial substate.
05/08/09   ss      Updated Copyright Information
03/28/09   rm      CMI Featurisation
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
09/14/07   vm      Added fix for CR 126406. On transition to disconnected, the
                   order in which the last camped cell(s), last camped frequency,
                   the frequencies in RPLMN list, and ACQ DB freqs are scanned
                   is now prioritized based on Carrier & Spec requirements.
06/01/07   vm      Added fix for CR 120193. There was a memory leak because RCR
                   was modifying the OSS link list ptrs. The change is to have a 
                   local ptr that points to the link list and traverses it without
                   modifying the link list pointers. 
04/20/07   da      Made changes needed to update ASN.1 version to R6 sept R6 2006
09/28/06   da      When CCCH RCR msg is received while waiting for L2ACK for RCR 
                   complete msg, or while waiting for cell update complete notification, 
                   take UE to DISCONNECTED state immediatly.
09/25/06   vm      Added support to take UE to Idle via CSP. Also removed
                   the definition of the function rrcrcr_send_rrc_channel_config_req_for_srb_release().
                   For details pls see CR 102193.
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/21/06   tkk     Added SRB#5 support.
06/08/06   vm      1. Included header file rrcccm_v.h
                   2. The function validate_fdd_frequency() now takes plmn_id as an additional 
                      argument.
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE REL5 while retaining the HSDPA specifc processing 
                   under FEATURE HSDPA.
04/18/06   da      Modified code to support migration to Mar-06 Rel-5 ASN1 +
                   R6 Hacks
03/16/06   tkk     Moved "local_group_release_list" under FEATURE_REL to suppress
                   compiler warning on Saber.
03/14/06   tkk     Lint warnings/errors cleanup
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1
12/20/05   bd      Changed syntax for suppressing lint error 613
12/01/05   sgk     Added check to send rrc connection release complete with
                   protocol error rrc_messageExtensionNotComprehended when 
                   extension rrc_RRCConnectionRelease_later_than_r3_criticalExtensions_criticalExtensions_chosen 
                   is set.
11/17/05   kc      Added support for Group Release
11/10/05   sm      Added changes to enable switching between Rel99 and Rel5 using nv_item
11/01/05   vm      Added support for trying acquisition on the best cell in the active set on
                   FACH/PCH->Idle transition (in addition to DCH->Idle tx which was added earlier). 
08/16/05   tkk     Correction to RCR release to handle processing when REL-5 
                   is not defined.
08/02/05   tkk     Enhanced processing of RCR message (received on DCCH & CCCH)
                   when later_than_r3 fields are present.
06/22/05   vk      Added the change to check for URNTI under FEATURE HSDPA as
                   the ASN1 Header file used in Saber & Raven builds are different
06/21/05   vm      Added support for trying acquisition on the best cell in the active set on
                   DCH->Idle transition. Added required support to get the scrambling code and pn position
                   of the best cell in the active set when UE sends Connection Release
                   Complete message to the network.
06/09/05   vk      Added the U-RNTI check when RRC Connection Release Message
                   with later than r3 extensions is received
04/17/05   da      Fix for issue where UE sends RCR complete message with protocol 
                   error if UE is in cell_fach and n308 is present.  UE shall ignore
                   this IE in cell_fach.
04/06/05   vr      Include rrclogging.h unconditionally for logging on all platforms
03/02/05    ttl          Support RRC ASN1 Release 5.
10/04/04   vk      Added support for rrc_RRCConnectionRelease_later_than_r3_chosen
                   in RRC Connection Release Message
08/30/04   sgk     Assigned the right enums for rrc_release_cause in function
                   rrcrcr_process_rcr_initial_substate and
                   rrcrcr_process_rcr_wait_for_l2ack_for_rrc_conn_rel_complete_substate
                   to fix lint error Type mismatch (assignment) (enum/enum).
                   Typecast RRC_PROCEDURE_RCR to uint8 in function
                   rrcrcr_process_rcr_initial_substate,
                   rrcrcr_process_rcr_wait_for_l2ack_for_rrc_conn_rel_complete_substate 
                   to fix lint warning: Converting enum to int.
08/25/04   bu      If RPLMN information is present in RRC connection release
                   message, call the CSP function to update acquisition database
                   with the frequency indicated in the message.
06/29/04   vk      Moved a print statement in fn rrcrcr_is_proc_active to 
                   print RRCRCR substate only if RCR is active to avoid 
           unnessary prints 
06/23/04   vk      Used correct ASN1 violation error i.e rrc_asn1_ViolationOrEncodingError_chosen
                   while sending a RRC Status Message from RCR.
06/15/04   vk      Added MSG_HIGH to indicate value of N308 received and also
                   reformatted a copuple of MSG_HIGH's  
06/01/04   vk      Implemented rrcrcr_is_proc_active() fn that specifies if RCR
                   is active
11/10/03   kc      Added new release cause for re-est signalling ind.
03/18/03   rj      Added code to save the Transaction Id when RRC Connection
                   Release is received in CELL_FACH state.
02/04/03   ram     Checked in lint changes, updated copyright.
10/28/02   rj      Updated code to fix RRC Connection Release Complete
                   message re-transmissions.
10/24/02   rj      Updated to use rrc_malloc and rrc_free to use dual heaps 
                   in place of mem_malloc and mem_free to avoid memory
                   fragmentation
08/02/02   rj      Updated code not to err-fatal if MM aborts RRC Connection
                   during RRC Connection Release procedure.
07/02/02   rj      Modified code not to err-fatal if RRC fails to enqueue
                   RRC Connection Release Complete in the uplink.
06/13/02   rj      Updated code to use U_RNTI from the ORDERED_CONFIG instead
                   of from RRCDATA module.
03/05/02   rj      Renamed varible rrc_u_rnti to rrc_u_rnti_internal.
03/05/02   rj      Modified check for status in RRC_DOWNLINK_L2ACK_IND
                   command to SUCCESS from TRUE beacuse LCM uses enum 
                   instead of boolean. 
02/28/02   rj      Added code to check for U-RNTI matching if RRC Connection
                   Release is received on CCCH logical channel. Also fixed
                   some bugs from previous check-in.
02/14/02   rj      Added code to query CELL UPDATE proc for a valid C-RNTI
                   in CELL_FACH state to transmit RRC Connection Release 
                   Complete. Added a new substate 
                   RRCRCR_WAIT_FOR_CELL_UPDATE_COMP_CNF for CELL_FACH state 
                   implementation and added code to process events in 
                   RRCRCR_WAIT_FOR_CELL_UPDATE_COMP_CNF substate. Updated code
                   to read Timer T308 from the IE"rrc_UE_ConnTimersAndConstants"
                   instead of SIB database.
02/01/02   rj      Added code to honor RRC_STATE_CHANGE_IND in RRCRCR_INITIAL
                   substate.
12/04/01   kc      Removed curr_state from RRC_CHANNEL_CONFIG_REQ as it is not
                   supported in current release.  Merge problems.
11/28/01   kc      Code changes for Dec->June Spec conversion.
10/25/01   rj      Modified code to add current state to RRC_CHANNEL_CONFIG_REQ
                   command.
10/24/01   rj      Initialized uninitialized vars to remove compiler warnings
09/19/10   rj      Modified F3 message text to maximum length of 40 charecters.
09/14/01   rj      Modified function rrcrcr_send_connection_rel_complete_msg
                   to check for RLC Id before allocating memory and Fixed
                   protocol error log for CCCH message. Added F3 message
                   to declare critical extensions are not supported and cleaned
                   some F3 messages.
09/14/01   kc      Added code to handle protocol error logging
09/03/01   rj      Removed include file rrcasnutil.h since it is no longer 
                   required after moving send_rrc_status_message to SEND CHAIN.
08/30/01   rj      Added code to implement peeking into OTA message and
                   added check for critical extensions.Changed 
                   UE_MODE_ACKNOWLEDGED_DATA_PLANE to UE_MODE_ACKNOWLEDGED_DATA.
08/08/01   rj      Removed the code under Feature FEATURE AL1 since it is
                   no longer required.
08/03/01   rj      For VST testing the Timer T308 multiplier is reduced to 
                   20 from 200 so that upper layer timer do not expire.                   
07/27/01   rj      The functionality to send RRC_REL_IND to MM is moved
                   to RRCRCE to take care all kinds of RRC Connection
                   Releases. Modified MSG_LOW statements to MSG_HIGH for all
                   UL and DL OTA messages. Updated  function
                   rrcrcr_update_release_cause to return release cause of
                   internal format type.
07/06/01   rj      Registration for state change from any connected mode states
                   to Disconnected state is added to powerup initialization
                   function. And registration for the same in RRC Connection
                   Release procedure is commented.
06/28/01   rj      Fixed a bug for ASN1 encoding failure and added code to 
                   initialize the bit mask for RRC Connection Release Complete
                   message. Removed initialization of bit mask for Integrity
                   Check info since it is initialized in Send Chain.
06/26/01   ram     While sending a RRC Connection Release Complete message, the
                   bit-mask to indicate Integrity protetion status was not
                   being set. Set the bit-mask to 0 indicating integrity 
                   check info is not included.
06/21/01   vn      Changed to accomodate the new HFN representation of an 
                   array in RRC Release Indication message. Merged changes
                   from the branch version 1.9.1.0
06/20/01   ram     The structure "new_state_ind" was being used incorrectly
                   in several places when a RRC_STATE_CHANGE_IND command was
                   received. The correct structure to be used is "state_change_ind".
                   Changed all uses of new_state_ind to state_change_ind.
06/08/01   rj      The timer T308 is multiplied by 200 for VST testing.
05/24/01   rj      Corrected number of retransmissions of RRC Connection
                   Release complete messages and added more comments.
04/29/01   rj      Added code to update U-RNTI in internal format by
                   using translate function. Fixed a bug in function
                   rrcrcr_send_connection_rel_complete_msg.
04/18/01   rj      Updated function rrcrcr_process_rcr_initial_substate 
                   for the changes in function rrc_send_rrc_status_message.
04/06/01   rj      Replaced RRC_NEW_STATE_IND with RRC_STATE_CHANGE_IND.
03/30/01   rj      Updated to be compatible with latest LED based rrcasn1.h and
                   rrcmmif.h. Implemented December Spec.
03/05/01   rj      Modified code to transmit RRC STATUS message without L2
                   ACK confirmation.
02/27/01   rj      If RRC Connection Release is received in invalid state due
                   to race conditions and invalid IEs are present in the 
                   message, RCR procedure sends RRC Connection release 
                   complete message with error indication to the UTRAN.
02/26/01   rj      Implemented flow chart review comments. Deleted substate
                   RRCRCR_WAIT_FOR_L2ACK_FOR_RRC_STATUS and function
                   rrcrcr_process_rcr_wait_for_l2ack_for_rrc_status_substate.
                   Added RRCRCR_WAIT_FOR_L2ACK_FOR_RRC_CONN_REL_COMPLETE 
                   substate to the Disconnected Substate. Modified code for 
                   the function rrcrcr_send_connection_rel_complete_msg. On
                   receiving invalid RRC Connection Release, the RCR procedure
                   sends the RRC STATUS message to the lower layers and resets
                   the state machine. Added code to stop the timer
                   RRCTMR_RCR_L2ACK_TIMER for any valid command except for 
                   RRC_TIMER_FOR_RCR_L2ACK_EXPIRED_IND command.
02/12/01   rj      Updated some comments.Added code to the functions
                   rrcrcr_process_disconnected_state,
                   rrcrcr_get_t308_from_serv_cell_db, rrcrcr_convert_t308_to_ms,
                   rrcrcr_send_rrc_channel_config_req_for_srb_release,
                   rrcrcr_send_connection_rel_complete_msg,
                   rrcrcr_update_release_cause, and rrcrcr_send_rrc_rel_ind.                 
02/06/01   rj      Created file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "wcdma_variation.h"
#include "rex.h"
#include "err.h"
#include "rrcasn1.h"
#include "rrcrcr.h"
#include "rrcrcri.h"
#include "rrcdata_v.h"
#include "rrccmd_v.h"
#include "mmtask.h"
#ifdef FEATURE_CMI
#include "mmtask_v.h"
#endif
#include "rrcsend.h"
#include "rrctmr.h"
#include "rrcsibdb.h"
#include "rrclcm.h"
#include "msg.h"
#include "rrcscmgr.h"
#include "rrcmmif.h"
#include "rrccu.h"
#include "rrcllc.h"
#include "rrclogging.h"
#include "rrccspi.h"
#include "rrccsp.h"
#include "rrcpg1.h"
#include "rrcccm.h"
#include "rrcmisc.h"
#include "rrcasn1util.h"

#include "rr_rrc_if.h"
#include "rrcmcm.h"

#ifdef FEATURE_WCDMA_TO_LTE
#include "lte_rrc_ext_api.h"
#endif

#ifdef FEATURE_QSH_EVENT_METRIC
#include "rrcsmc.h"
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*===========================================================================

      RRC CONNECTION RELEASE PROCEDURE SUBSTATE Variable        

===========================================================================*/
/* RCR Substate variable initialized to RRCRCR_INITIAL during the RRC TASK 
   startup. */              
LOCAL rrcrcr_substate_e_type  rrcrcr_substate;  

LOCAL uint32 rrcrcr_n_308;         /* Variable to store max number of 
                                      RRC CONNECTION RELEASE COMPLETE 
                                      retransmission */
LOCAL boolean rrcrcr_cell_reselection_occured; 
                                   /* This indicates whether cell reselection
                                      occured during RCR procedure */
LOCAL rrc_RRC_TransactionIdentifier rrcrcr_transaction_identifier;
                                   /* This stores the transaction Id for
                                      RRC Connection Release message */
LOCAL rrcrcr_error_indication_status_e_type rrcrcr_error_ind; 
                                   /* This stores the information whether to 
                                      include error cause in the RRC
                                      Connection Release Complete Msg */
                                   
LOCAL rrc_redirect_req_type saved_redirect_info;
LOCAL boolean redirection_info_present;
                                   
boolean rrc_ifreq_rdr_in_prgrs = FALSE;
boolean rrc_rplmn_info_prsnt = FALSE;
                                   
#ifdef FEATURE_QSH_EVENT_METRIC
extern boolean rrc_qsh_dl_committed;
extern rrc_qsh_ul_failure_ota_param_s failure_ota;
extern rrc_qsh_metric_db_type rrc_qsh_metric_db[WRRC_QSH_METRIC_MAX+1];
#endif             
/*===========================================================================

FUNCTION rrcrcr_init_procedure

DESCRIPTION
  This function will initialize the RRC Connection Release procedure
  substate to RRCRCR_INITIAL Substate. This will be called in the RRCTASK
  modules along with other Init Procedures.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrcrcr_init_procedure
(    
  void  
)
{
  /* Resets the RCR state machine */
  rrcrcr_substate = RRCRCR_INITIAL; 
  /* The following code is added since de-registration mechanism is not place.
     It should be deleted once de-registration mechanism in place in State
     change manager */
  rrcscmgr_register_for_scn(RRC_PROCEDURE_RCR,
                            RRC_STATE_WILDCARD,
                            RRC_STATE_DISCONNECTED);     
} /* rrcrcr_init_procedure */

/*===========================================================================

FUNCTION rrcrcr_procedure_event_handler

DESCRIPTION
  This is the entry point for RRC Connection Release procedure. All
  events for RCR come to this function from RRC Dispatcher. Depending on the
  UE state of RRC Connection Release Procedure, it calls the corresponding
  state handler. These state handlers call substate handlers depending on the 
  the current substate of RRC Connection Release procedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  
  rrc_state_e_type rrc_state = rrc_get_state();
  WRRC_MSG1_HIGH_OPT("function rrcrcr_procedure_event_handler is called in state rrc_state_e_type_value%d", rrc_state);
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_RCR,rrcrcr_substate,cmd_ptr->cmd_hdr.cmd_id);

  switch (rrc_state)
  {
    case RRC_STATE_DISCONNECTED:
      rrcrcr_process_disconnected_state(cmd_ptr);
      break;

    case RRC_STATE_CELL_FACH:    
      rrcrcr_process_cell_fach_state(cmd_ptr); 
      break;

    case RRC_STATE_CELL_DCH:     
      rrcrcr_process_cell_dch_state(cmd_ptr); 
      break;
  
    default:  
      WRRC_MSG1_HIGH("Event forwarded in an invalid State: %d",rrc_state);
      break;
   }  
} /* rrcrcr_procedure_event_handler */

/*===========================================================================

FUNCTION rrcrcr_process_cell_fach_state

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Release procedure in CELL_FACH state of Connected  mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_process_cell_fach_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  switch (rrcrcr_substate)
  {
    case RRCRCR_INITIAL: 
      rrcrcr_process_rcr_initial_substate(cmd_ptr);
      break;
  
    case RRCRCR_WAIT_FOR_CELL_UPDATE_COMP_CNF:
      rrcrcr_process_rcr_wait_for_cell_update_comp_cnf_substate(cmd_ptr);
      break;

    case RRCRCR_WAIT_FOR_L2ACK_FOR_RRC_CONN_REL_COMPLETE:
      rrcrcr_process_rcr_wait_for_l2ack_for_rrc_conn_rel_complete_substate(cmd_ptr);
      break;

    case RRCRCR_WAIT_FOR_SRB_RELEASE_CNF:
      rrcrcr_process_rcr_wait_for_srb_release_cnf_substate(cmd_ptr);
      break;

    default:     
      WRRC_MSG1_HIGH("Invalid substate in CELL_FACH state: %d",
                rrcrcr_substate);
     break;
  }  
} /* rrcrcr_process_cell_fach_state */
/*===========================================================================

FUNCTION rrcrcr_process_cell_dch_state

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Release procedure in CELL_DCH state of Connected  mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_process_cell_dch_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  switch (rrcrcr_substate)
  {
    case RRCRCR_INITIAL: 
      rrcrcr_process_rcr_initial_substate(cmd_ptr);
      break;

    case RRCRCR_WAIT_FOR_T308_TIMER_EXPIRE:
      rrcrcr_process_rcr_wait_for_t308_timer_expire_substate(cmd_ptr);
      break;

    case RRCRCR_WAIT_FOR_SRB_RELEASE_CNF:
      rrcrcr_process_rcr_wait_for_srb_release_cnf_substate(cmd_ptr);
      break;

    default:     
      WRRC_MSG1_HIGH("Invalid substate in CELL_DCH state: %d",
                rrcrcr_substate);
     break;
  }  
} /* rrcrcr_process_cell_dch_state */
/*===========================================================================

FUNCTION rrcrcr_process_disconnected_state

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Release procedure in DISCONNECTED state of Idle  mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_process_disconnected_state     
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  switch (rrcrcr_substate)
  {
    case RRCRCR_WAIT_FOR_L2ACK_FOR_RRC_CONN_REL_COMPLETE:
      rrcrcr_process_rcr_wait_for_l2ack_for_rrc_conn_rel_complete_substate(cmd_ptr);
      break;

    case RRCRCR_WAIT_FOR_T308_TIMER_EXPIRE:
      rrcrcr_process_rcr_wait_for_t308_timer_expire_substate(cmd_ptr);
      break;

    case RRCRCR_WAIT_FOR_CELL_UPDATE_COMP_CNF:
      rrcrcr_process_rcr_wait_for_cell_update_comp_cnf_substate(cmd_ptr);
      break;

    case RRCRCR_WAIT_FOR_SRB_RELEASE_CNF:
      rrcrcr_process_rcr_wait_for_srb_release_cnf_substate(cmd_ptr);
      break;

    default:     
      WRRC_MSG2_HIGH("In Idle mode, Invalid cmd %d in RCR substate %d",
                cmd_ptr->cmd_hdr.cmd_id,rrcrcr_substate);
     break;
  }  
} /* rrcrcr_process_disconnected_state */

/*===========================================================================

FUNCTION rrcrcr_convert_t308_to_ms

DESCRIPTION
  This function converts the rrc_T_308 enums to milliseconds

DEPENDENCIES
  None

RETURN VALUE
  int

SIDE EFFECTS
  None
===========================================================================*/
uint32 rrcrcr_convert_t308_to_ms
(
  rrc_T_308 rrcrcr_t_308                   /* enum for Timer T308 */
)
{
  uint32 timer_value;                         /* Timer Value in milli seconds */

  switch (rrcrcr_t_308)
  {
    case rrc_T_308_ms40:
      timer_value = 40;
      break;

    case rrc_T_308_ms80:
      timer_value = 80;
      break;    
  
    case rrc_T_308_ms160:
      timer_value = 160;
      break;

    case rrc_T_308_ms320:
      timer_value = 320;
      break;
  
    default:
      WRRC_MSG0_HIGH("Unknown Timer value rcvd from SIB1");
      timer_value = 40;  /* Set lowest timer value as a default */
      break;
  }
  return (timer_value);
} /* rrcrcr_convert_t308_to_ms*/


/*===========================================================================
FUNCTION rrcrcr_send_connection_rel_complete_msg

DESCRIPTION
  This function builds RRC CONNECTION RELEASE COMPLETE message and send the
  same to the SEND CHAIN to transmit either in Unacknowledged mode or 
  Acknowledged mode.
DEPENDENCIES
  None

RETURN VALUE
  rrcrcr_status_e_type: It returns RRCRCR_SUCCESS if it successfully places 
  RRC CONNECTION RELEASE COMPLETE SDU to the SEND CHAIN. Otherwise it returns
  RRCRCR_FAILURE_MAJOR

SIDE EFFECTS
  None
===========================================================================*/
rrcrcr_status_e_type rrcrcr_send_connection_rel_complete_msg
(
  rrc_RB_Identity rb_id,                    /* Radio bearer Id */ 
  uecomdef_logch_mode_e_type mode,          /* Mode of transmit */  
  rrcrcr_error_indication_status_e_type error_ind_status
                                            /* Indicates whether to include
                                               Error Indication field in the 
                                               RRC Connection Rel comlete Msg*/                                         
)
{
  rrcrcr_status_e_type status = RRCRCR_FAILURE_MAJOR; 
                                            /* Status is initialized to
                                               Failure */
  rrc_UL_DCCH_Message *msg_ptr;             /* Pointer to uplink DCCH message*/

  rlc_lc_id_type ul_lc_id;                  /* Logical Channel Id */
  
 /* Checks whether Logical Channel entity setup for DCCH logical
     Channel type */
  ul_lc_id = rrclcm_check_ul_rlc_lc_id ( UE_LOGCHAN_DCCH,
                                         rb_id,                                           
                                         mode);
  /* If RLC Logical Channel Id is already setup, the send RRC Connection
     Release Complete message through SEND_CHAIN */
  if(ul_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    /* Allocate a buffer for an uplink DCCH Message. This buffer should be 
       released by rrcsend_send_ul_sdu() function using OSS compiler functions */

    msg_ptr = (rrc_UL_DCCH_Message *) rrc_malloc ( 
                                      sizeof(struct rrc_UL_DCCH_Message));
    if (msg_ptr != NULL)
    {

      msg_ptr->message.t = T_rrc_UL_DCCH_MessageType_rrcConnectionReleaseComplete;

      /* Initialize the bit mask for RRC Connection Release Complete */
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionReleaseComplete);

      if(RRCRCR_INCLUDE_ERROR_INDICATION == error_ind_status)
      {
       RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.rrcConnectionReleaseComplete,
          errorIndication);
        msg_ptr->message.u.rrcConnectionReleaseComplete.errorIndication.t =
          T_rrc_FailureCauseWithProtErr_protocolError;
        msg_ptr->message.u.rrcConnectionReleaseComplete.errorIndication.u.protocolError = 
          rtxMemAllocTypeZ(&enc_ctxt, rrc_ProtocolErrorInformation);

        msg_ptr->message.u.rrcConnectionReleaseComplete.errorIndication.u.protocolError->
         diagnosticsType.t = rrc_protocol_error_information.diagnosticsType.t;
        msg_ptr->message.u.rrcConnectionReleaseComplete.errorIndication.u.protocolError->
         diagnosticsType.u.type1 = rrc_protocol_error_information.diagnosticsType.u.type1;
        
        rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                (uint8) RRC_PROCEDURE_RCR,
                                (uint8) T_rrc_FailureCauseWithProtErr_protocolError,
                                (uint8) rrc_protocol_error_information.diagnosticsType.u.type1->protocolErrorCause                               
                               );
      }
    
      /* Set the Transaction Id */
      msg_ptr->message.u.rrcConnectionReleaseComplete.rrc_TransactionIdentifier =
        rrcrcr_transaction_identifier;
#ifdef FEATURE_QSH_EVENT_METRIC
      if(rrc_qsh_dl_committed == FALSE)
      {
        failure_ota.ul_ota_type = RRC_OTA_UL_CPLT;
        failure_ota.msg_id = WRRC_UL_DCCH_MESSAGETYPE_RRCCONNECTIONRELEASECOMPLETE;
        if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status())
        {
          failure_ota.ul_fail_ota.ul_ota_parms.ul_integrity_info_present = 1;
        }
      }
      else if(rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr != NULL)
      {
        if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status())
        {
          ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.ul_ota_parms.ul_integrity_info_present= 1;
        }
      }
#endif
      /* This places SDU into RLC watermark queue */
      if(rb_id == DCCH_UM_RADIO_BEARER_ID)
      {
        if(RRCSEND_SUCCESS ==
           rrcsend_send_ul_sdu(RRC_PROCEDURE_RCR,msg_ptr,ul_lc_id,
                            RRCSEND_L2ACK_NOT_REQUIRED))
        {
          status = RRCRCR_SUCCESS;        

          /* start the timer T308 */
          rrctmr_start_timer(RRCTMR_T_308_TIMER, 
                        rrcrcr_convert_t308_to_ms(RRC_GET_T308()) );  
        }

      }
      else if (rb_id == DCCH_AM_RADIO_BEARER_ID)
      {
        if(RRCSEND_SUCCESS == 
           rrcsend_send_ul_sdu(RRC_PROCEDURE_RCR,msg_ptr,ul_lc_id,
           RRCSEND_L2ACK_REQUIRED))
        {
          status = RRCRCR_SUCCESS;

          /* Set the pointer to NULL as it is already freed in embedded buff */
          rrc_protocol_error_information.diagnosticsType.u.type1 = NULL;

          /* Start the sanity timer for L2 ACK */
          rrctmr_start_timer(RRCTMR_RCR_L2ACK_TIMER, 
                             RRCRCR_TIMER_VALUE_FOR_L2_ACK);
        }   
      }
      else
      {
        WRRC_MSG0_HIGH("Invalid RB for RRC Conn Rel Complete");
      }
    }
  }
  else
  { 
    WRRC_MSG1_HIGH("Can't find RLC ID for UL RB: %d", rb_id);

  } 
  return (status);            
}/*rrcrcr_send_connection_rel_complete_msg */

#ifdef FEATURE_WCDMA_TO_LTE
/*========================================================================
FUNCTION rrcrcr_save_lte_redirection_info

DESCRIPTION
  This function saves the LTE redirection info received in RRC
  Connection Release message. This info is then later passed on to
  CSP for further processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/
void rrcrcr_save_lte_redirection_info
(
  rrc_EUTRA_TargetFreqInfoList *EUTRA_target_freq_info_ptr
)
{
  rrc_EUTRA_BlacklistedCellPerFreqList *blacklisted_cell_info_ptr = NULL;
  uint8 earfcn_count = 0;
  uint8 blacklisted_cells_count = 0;
  uint32 idx, idy;
  uint32 earfcn;
  redirection_info_present = TRUE;

  saved_redirect_info.choice = RRC_INTER_SYSTEM_LTE;
  saved_redirect_info.u.EUTRA_target_cell_info.earfcn_count = 0;

  idx = 0;
  while((EUTRA_target_freq_info_ptr->n > idx) &&
          (earfcn_count < LTE_RRC_IRAT_MAX_REDIR_FREQ))
  {
    earfcn = EUTRA_target_freq_info_ptr->elem[idx].dlEUTRACarrierFreq;
    if(!lte_rrc_earfcn_is_supported(earfcn, rrccsp_get_lte_band_pref()))
    {
      WRRC_MSG1_HIGH("WTOL: EARFCN %d not supported by UE", EUTRA_target_freq_info_ptr->elem[idx].dlEUTRACarrierFreq);
      idx++;
      continue;
    }

    saved_redirect_info.u.EUTRA_target_cell_info.earfcn_info[earfcn_count].earfcn = EUTRA_target_freq_info_ptr->elem[idx].dlEUTRACarrierFreq;
    WRRC_MSG1_HIGH("WTOL: target EARFCN %d", EUTRA_target_freq_info_ptr->elem[idx].dlEUTRACarrierFreq);
  
    /* Check if blacklisted cells also present for the current EARFCN.*/
    blacklisted_cells_count = 0;
    if(EUTRA_target_freq_info_ptr->elem[idx].m.eutraBlacklistedCellPerFreqListPresent)
    {
      idy = 0;
      blacklisted_cell_info_ptr = &(EUTRA_target_freq_info_ptr->elem[idx].eutraBlacklistedCellPerFreqList);
      while((blacklisted_cell_info_ptr->n > idy) &&
               (blacklisted_cells_count < LTE_RRC_IRAT_MAX_EUTRA_CELLS_PER_FREQ))
      {
        saved_redirect_info.u.EUTRA_target_cell_info.earfcn_info[earfcn_count].blacklisted_cells[blacklisted_cells_count++] = 
           blacklisted_cell_info_ptr->elem[idy].physicalCellIdentity;
        idy++;
      }
    }
    saved_redirect_info.u.EUTRA_target_cell_info.earfcn_info[earfcn_count++].blacklisted_cells_count = blacklisted_cells_count;
    idx++;
  }
  saved_redirect_info.u.EUTRA_target_cell_info.earfcn_count = earfcn_count;

  if(earfcn_count == 0)
  {
    WRRC_MSG0_HIGH("WTOL: No target EARFCN's supported. No redirection.");
    redirection_info_present = FALSE;
  }

}
#endif

/*===========================================================================
FUNCTION rrcrcr_process_860non_critical_ext

DESCRIPTION
  Processes 860 non critical extension of RRC Conn Rel OTA.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcrcr_process_860non_critical_ext
(
  rrc_RRCConnectionRelease_v860ext_IEs * rcr_860ext
)
{
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(rcr_860ext,rrc_RRCConnectionRelease_v860ext_IEs,redirectionInfo))
  {
#ifdef FEATURE_WCDMA_TO_LTE
    if(rcr_860ext->redirectionInfo.t == T_rrc_RedirectionInfo_v860ext_interRATInfo)
    {
      if(rcr_860ext->redirectionInfo.u.interRATInfo->t == T_rrc_InterRATInfo_v860ext_eutra)
      {
        rrcrcr_save_lte_redirection_info(&(rcr_860ext->redirectionInfo.u.interRATInfo->u.eutra->eutra_TargetFreqInfoList));
      }
      else
      {
        WRRC_MSG0_ERROR("GSM rat redirection info should be given in IE RedirectionInfo.or RedirectionInfo-r6");
      }
    }
    else
    {
      WRRC_MSG0_ERROR("Inter-freq redirection info should be given in IE RedirectionInfo or RedirectionInfo-r6.");
    }
#else
    WRRC_MSG0_HIGH("REL8: RedirectionInfo-v860ext not supported.");
#endif
  }
}

/*===========================================================================

FUNCTION rrcrcr_update_release_cause

DESCRIPTION
  This function converts the release cause from ASN1 format into RRC-MM 
  interface format.
DEPENDENCIES
  None

RETURN VALUE
  rrc_rel_cause_e_type: 

SIDE EFFECTS
  None
===========================================================================*/
rrc_rel_cause_e_type rrcrcr_update_release_cause
(
  rrc_ReleaseCause release_cause 
)
{  
  rrc_rel_cause_e_type converted_rel_cause;

  switch(release_cause)
  {
    case rrc_ReleaseCause_normalEvent:
      converted_rel_cause = RRC_REL_CAUSE_NORMAL;
      break;
    case rrc_ReleaseCause_unspecified:
      converted_rel_cause = RRC_REL_CAUSE_UNSPEC;
      break;
    case rrc_ReleaseCause_pre_emptiveRelease:
      converted_rel_cause = RRC_REL_CAUSE_PRE_EMPTIVE;
      break;
    case rrc_ReleaseCause_congestion:
      converted_rel_cause = RRC_REL_CAUSE_CONGESTION;
      break;
    case rrc_ReleaseCause_re_establishmentReject:
      converted_rel_cause = RRC_REL_CAUSE_RE_ESTABLISH_REJECT;
      break;
    case rrc_ReleaseCause_userInactivity:
      converted_rel_cause = RRC_REL_USER_INACTIVITY;
      break;
    case rrc_ReleaseCause_directedsignallingconnectionre_establishment:
      converted_rel_cause = RRC_DIRECTED_SIGNALLING_REESTABLISHMENT;
      break;
    default:
      /* For other unknown ASN1 release causes set to Unspecified */
      MSG_MED("Invalid release cause rcvd : %d",release_cause,0,0);
      converted_rel_cause = RRC_REL_CAUSE_UNSPEC;
      break; 
  }

  return (converted_rel_cause);
}/*rrcrcr_update_release_cause */

/*========================================================================
FUNCTION rrcrcr_save_redirection_info

DESCRIPTION
  This function saves the redirection info received in RRC
  Connection Release message. This info is then later passed on to
  CSP for further processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
========================================================================*/

void rrcrcr_save_redirection_info
(
  rrc_RedirectionInfo_r6 *redirection_info_ptr
)
{
  sys_band_T gsm_band;
  rrc_rr_frequence_band_e_type freq_band;
  rrc_GSM_TargetCellInfoList *GSM_target_cell_info_ptr = NULL;
  rrc_csp_acq_entry_type acq_entry;

  WRRC_MSG0_HIGH("Saving redirection info from Conn release message.");
  redirection_info_present = TRUE;

  switch(redirection_info_ptr->t)
  {
    case T_rrc_RedirectionInfo_r6_frequencyInfo:
      saved_redirect_info.u.freq.uarfcn_UL_present = FALSE;
      saved_redirect_info.choice = RRC_FREQ_REDIRECTION;
      saved_redirect_info.u.freq.uarfcn_DL = (uint16)redirection_info_ptr->u.frequencyInfo->modeSpecificInfo.u.fdd->uarfcn_DL;
  
      if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(redirection_info_ptr->u.frequencyInfo->modeSpecificInfo.u.fdd,
        uarfcn_UL))
      {
        saved_redirect_info.u.freq.uarfcn_UL_present = TRUE;
        saved_redirect_info.u.freq.uarfcn_UL = (uint16)redirection_info_ptr->u.frequencyInfo->modeSpecificInfo.u.fdd->uarfcn_UL;
      }

      /* Unbar the frequency if provided in IE redirection info 
        Spec 25304 : 5.2.6.1.3 */
      rrc_csp_int_data.wcdma_cell_time_to_reselect = 0;
      rrc_csp_int_data.cell_bar_info = RRC_CSP_WCDMA_FREQ_UNBAR;
      acq_entry.freq = saved_redirect_info.u.freq.uarfcn_DL;
      rrc_csp_send_l1_cmd(RRC_PROCEDURE_CSP, CPHY_CELL_BAR_REQ, &acq_entry);
      break;
  
    case T_rrc_RedirectionInfo_r6_interRATInfo:
      saved_redirect_info.choice = RRC_INTER_SYSTEM_GSM;
      saved_redirect_info.u.GSM_cell_info_list.size = 0;
      if(RRC_CHECK_MSG_TYPE_IE_PTR(redirection_info_ptr->u.interRATInfo,gsm_TargetCellInfoListPresent))
      {
       uint32 local_idx=0;
        GSM_target_cell_info_ptr = &redirection_info_ptr->u.interRATInfo->gsm_TargetCellInfoList;
       
        while ((GSM_target_cell_info_ptr->n > local_idx) && 
               (saved_redirect_info.u.GSM_cell_info_list.size < MAX_RRC_RR_GSM_CELL_LIST_SIZE))
        {
            /* Let first check the type of the cell to find if we
             * really need to give this cell to L1 */
          switch(GSM_target_cell_info_ptr->elem[local_idx].frequency_band)
          {
             case rrc_Frequency_Band_dcs1800BandUsed:
               freq_band = RRC_RR_DCS_1800;
               break;
    
             case rrc_Frequency_Band_pcs1900BandUsed:
               freq_band = RRC_RR_PCS_1900;
               break;
    
             default:
               WRRC_MSG1_ERROR("Unsupported Band %d rcvd",GSM_target_cell_info_ptr->elem[local_idx].frequency_band);
               freq_band = RRC_RR_MAX_BANDS;
               break;
          }

          if((gsm_band
                  = rr_rrc_get_arfcn_band_internal((uint16)GSM_target_cell_info_ptr->elem[local_idx].bcch_ARFCN, freq_band))
               != INVALID_BAND)
          {
             WRRC_MSG2_HIGH("Adding ARFCN %d and band %d to the list to be sent to RR",
                      GSM_target_cell_info_ptr->elem[local_idx].bcch_ARFCN, freq_band);
    
             /* Get bcch_ARFCN */
             saved_redirect_info.u.GSM_cell_info_list.arfcns[saved_redirect_info.u.GSM_cell_info_list.size].num
                = (uint16) GSM_target_cell_info_ptr->elem[local_idx].bcch_ARFCN;
    
             saved_redirect_info.u.GSM_cell_info_list.arfcns[saved_redirect_info.u.GSM_cell_info_list.size].band
                = gsm_band;
          }
          else
          {
              WRRC_MSG2_HIGH("Band %d for ARFCN %d is invalid. Not adding to list",
                      (rrc_rr_frequence_band_e_type)GSM_target_cell_info_ptr->elem[local_idx].frequency_band,
                      (uint16) GSM_target_cell_info_ptr->elem[local_idx].bcch_ARFCN);
          }
          if(GSM_target_cell_info_ptr->elem[local_idx].m.bsicPresent)
          {
             saved_redirect_info.u.GSM_cell_info_list.BSIC[saved_redirect_info.u.GSM_cell_info_list.size].BSIC_NCC 
                    = GSM_target_cell_info_ptr->elem[local_idx].bsic.ncc;
             saved_redirect_info.u.GSM_cell_info_list.BSIC[saved_redirect_info.u.GSM_cell_info_list.size++].BSIC_BCC 
                    = GSM_target_cell_info_ptr->elem[local_idx].bsic.bcc;
          }
          else
          {
             saved_redirect_info.u.GSM_cell_info_list.BSIC[saved_redirect_info.u.GSM_cell_info_list.size].BSIC_NCC 
                    = 0xFF;
             saved_redirect_info.u.GSM_cell_info_list.BSIC[saved_redirect_info.u.GSM_cell_info_list.size++].BSIC_BCC 
                    = 0xFF;
          }
          local_idx++;
        }   
      }
      break;
   
    default:     
      WRRC_MSG0_ERROR("This should never happen");
      break;
  }
}
#ifdef FEATURE_QSH_EVENT_METRIC
/*===========================================================================
FUNCTION rrcrcr_map_qsh_release_cause

DESCRIPTION
  This function maps rrc release cause to qsh release cause type
DEPENDENCIES
  None
RETURN VALUE
  wrrc_conn_rel_cause_e_type
SIDE EFFECTS
  None
===========================================================================*/
wrrc_conn_rel_cause_e_type rrcrcr_map_qsh_release_cause(rrc_rel_cause_e_type rrc_release_cause)
{
  switch(rrc_release_cause)
  {
    case RRC_REL_CAUSE_NORMAL:
      return(WRRC_CONN_REL_CAUSE_NORMAL_EVENT);
    break;
    case RRC_REL_CAUSE_UNSPEC:
      return(WRRC_CONN_REL_CAUSE_UNSPECIFIED);
    break;
    case RRC_REL_CAUSE_PRE_EMPTIVE:
      return(WRRC_CONN_REL_CAUSE_PRE_EMPTIVE_RELEASE);
    break;
    case RRC_REL_CAUSE_CONGESTION:
      return(WRRC_CONN_REL_CAUSE_CONGESTION);
    break;
    case RRC_REL_CAUSE_RE_ESTABLISH_REJECT:
      return(WRRC_CONN_REL_CAUSE_REESTABLISHMENT_REJECT);
    break;
    case RRC_REL_USER_INACTIVITY:
      return(WRRC_CONN_REL_CAUSE_USER_INACTIVITY);
    break;
    case RRC_DIRECTED_SIGNALLING_REESTABLISHMENT:
      return(WRRC_CONN_REL_CAUSE_DRCTD_SIGNALING_CONN_REEST);
    break;
    default:
      return(WRRC_CONN_REL_CAUSE_OTHER);
    break;
  }
}

/*===========================================================================
FUNCTION rrcrcr_qsh_update_release_cause

DESCRIPTION
  This function updates rrc connection release cause to qsh
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void rrcrcr_qsh_update_release_cause(void)
{
  wrrc_call_end_data_log_type call_end_data;
  call_end_data.type = WRRC_CALL_END_TYPE_RRC_CONNECTION_RELEASE;
  call_end_data.cause.conn_rel_cause = rrcrcr_map_qsh_release_cause(rrc_release_cause);
  wrrc_qsh_log_conn_end_info(WRRC_METRIC_LOG,&call_end_data);
}
#endif
/*===========================================================================

FUNCTION rrcrcr_process_rcr_initial_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Release procedure in RRCRCR_INITIAL substate of either CELL_FACH State or
  CELL_DCH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_process_rcr_initial_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  rrc_DL_DCCH_Message *dcch_msg_ptr = NULL;  /* Pointer to Downlink DCCH
                                                message */
  rrc_DL_CCCH_Message *ccch_msg_ptr = NULL;  /* Pointer to Downlink CCCH
                                                message */
  rlc_lc_id_type lc_id;                      /* Local variable to store 
                                                RLC LC Id */
  rrc_state_e_type current_state;            /* Local Varibale to store 
                                                the current state */
  rrc_int_u_rnti_type u_rnti=0;                /* Local varibale to store U-RNTI 
                                                received through RCR message */ 
 
  rrc_int_u_rnti_type u_rnti_local=0;          /* Local varibale to store U-RNTI 
                                                received from RRCLLC */

  uint8 message_choice;                      /* Local varibel to store messgae
                                                t */

  rrc_Rplmn_Information *local_rplmn_information = NULL; /* Local structure for RPLMN info */

  rrc_FDD_UMTS_Frequency_List * fdd_freq_list_ptr = NULL; /* Ptr to the freq list present in RRC Connection
                                                          * Release Message */

  boolean acq_db_updated = FALSE;            /* determines if acq db was updated or not
                                                when Connection Release Request from the 
                                                Network had a frequency information. This
                                                variable is subsequently used to determine
                                                if UE should try acq on the best cell in 
                                                acq db during DCH->Idle transition. If acq_db
                                                is updated then acquisition on the best cell in 
                                                active set is not attempted */

  boolean fdd_freq_list_contains_camped_freq = FALSE;  
                                              /* stores the state related to whether camped freq
                                                 is present in the fdd freq list or not */

  rrc_plmn_identity_type camped_plmn = rrc_ccm_get_curr_camped_plmn();
                                              /* used to compare the camped plmn with Japan to determine
                                               * the priority order in which freq(s) present in
                                               * active set, rplmn freq list, acq db should be searched.
                                               * If MCC is Japan and RPLMN freq list is present then
                                               * the priority order is:
                                               *      RPLMN Freq List --> Rem. Freqs in ACQ DB.
                                               * Else, the priority order is 
                                               *      Camped Cell --> Camped Freq --> RPLMN Freq List --> Rem. Freqs in ACQ DB. 
                                               * 
                                               * Also, if last camped frequency is present in the RPLMN list
                                               * then it is scanned before any other entry in RPLMN List or ACQ DB.
                                               */

  rrc_ReleaseCause     rrc_release_cause_recvd;

  rrc_RedirectionInfo_r6 *local_redirect_info_ptr = NULL;

#ifdef FEATURE_QSH_EVENT_METRIC
  rrc_log_rrc_conn_rel_ota conn_rel_params;   
  memset(&conn_rel_params,0,sizeof(rrc_log_rrc_conn_rel_ota));  
#endif

  switch(cmd_ptr->cmd_hdr.cmd_id)
  {     
    case RRC_DOWNLINK_SDU_IND:
      /* Sets the cell reselection to false at the start of RRC Connection
         Release procedure */
      rrcrcr_cell_reselection_occured = FALSE;

      /* Set boolean not to include error cause */
      rrcrcr_error_ind = RRCRCR_NOT_INCLUDE_ERROR_INDICATION;

      rrc_protocol_error_reject = FALSE;

      /* Assert the receieved message is of type rrc_DL_CCCH_Message */
      if(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == rrc_DL_DCCH_Message_PDU)
      {
        message_choice = 
          rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);

        if(cmd_ptr->cmd.downlink_sdu_ind.decoded_msg != NULL)
        {
          /* Get the OTA message pointer */
          dcch_msg_ptr = (rrc_DL_DCCH_Message *) 
                         cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;
        }
        else
        { /* ASN1 decoding failed */
          rrc_protocol_error_reject = TRUE;
        }

        if( message_choice == 
            T_rrc_DL_DCCH_MessageType_rrcConnectionRelease)
        {
          WRRC_MSG0_HIGH("Rcvd RRC Connection Release DCCH");

          redirection_info_present = FALSE;

          if(rrc_protocol_error_reject == TRUE) /* DCCH ASN1 decoding failed */
          {
            rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                    (uint8) RRC_PROCEDURE_RCR,
                                    (uint8) T_rrc_FailureCauseWithProtErr_protocolError,
                                    (uint8) rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError                               
                                  );

            lc_id = rrclcm_check_ul_rlc_lc_id(    
                    UE_LOGCHAN_DCCH,          /* Uplink Logical Channel Type. */
                    DCCH_AM_RADIO_BEARER_ID,  /* Radio Bearer Id assoicated with
                                                the above Downlink Logical 
                                                Channel type. */
                    UE_MODE_ACKNOWLEDGED_DATA);
                                              /* Mode of transmission for the RLC 
                                                 service entity. */
            if(RRCLCM_RLC_LC_ID_NOT_FOUND == lc_id)
            {
              WRRC_MSG0_HIGH("Can't find RLC ID for UL RB 2");
            } 
            else /* Valid RLC ID found */
            {
              if(SUCCESS != rrcsend_send_rrc_status_message(
                 T_rrc_ProtocolErrorMoreInformation_type1_asn1_ViolationOrEncodingError,
                                               // The protocol error cause to be 
                                               // placed in the message
                 0,                            // Set transaction id to 0 since
                                               // ASN1 decoding failed
                 rrc_ReceivedMessageType_rrcConnectionRelease,     // Id  of the received message
                 RRC_PROCEDURE_RCR,            // RRC procedure
                 lc_id,                        /* Logical Channel Id */
                 RRCSEND_L2ACK_NOT_REQUIRED  ) ) 
                                         /* Indicates whether L2 Ack 
                                                  required or not */
              {
                WRRC_MSG0_HIGH("Unable to transmit RRC STATUS message");
              }  
              /* Reset the state machine and procedure ends */
              rrcrcr_substate = RRCRCR_INITIAL;
            } /* Valid RLC ID found */
        
          }
          else /* DCCH ASN1 decoding is successful */
          {
            boolean n308_presence       = FALSE;

            boolean rel5_critical_extn_supported = FALSE;

            /*lint -save -e613 */
            if(dcch_msg_ptr->message.u.rrcConnectionRelease.t ==
              T_rrc_RRCConnectionRelease_later_than_r3 )
            {
              /* Update Transaction Id */
              rrcrcr_transaction_identifier = dcch_msg_ptr->message.u.
                   rrcConnectionRelease.u.later_than_r3.rrc_TransactionIdentifier;

              /* Critical extensions go here */
              if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
              {
                if(dcch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.t ==
                  T_rrc_RRCConnectionRelease_criticalExtensions_1_r4)
                {
                  rel5_critical_extn_supported = TRUE;      
                  if(dcch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.
                      criticalExtensions.u.r4.rrcConnectionRelease_r4.m.rplmn_informationPresent)
                  {
                    /*lint -save -e740 */
                    local_rplmn_information = (rrc_Rplmn_Information *)&(dcch_msg_ptr->message.u.
                      rrcConnectionRelease.u.later_than_r3.criticalExtensions.u.r4.
                      rrcConnectionRelease_r4.rplmn_information);
                    /*lint -restore */
                  }
                  if(dcch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.
                      criticalExtensions.u.r4.m.v4d0NonCriticalExtensionsPresent)
                  {
                    if(dcch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.
						criticalExtensions.u.r4.v4d0NonCriticalExtensions.m.v690NonCriticalExtensionsPresent)
                    {
                      if(dcch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.u.r4.v4d0NonCriticalExtensions.v690NonCriticalExtensions.
                          rrcConnectionRelease_v690ext.m.redirectionInfo_v690extPresent)
                      {
                        local_redirect_info_ptr = (rrc_RedirectionInfo_r6 *)&(dcch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.u.r4.v4d0NonCriticalExtensions.
                          v690NonCriticalExtensions.rrcConnectionRelease_v690ext.redirectionInfo_v690ext);
                        if(local_redirect_info_ptr != NULL)
                        {
                           rrcrcr_save_redirection_info(local_redirect_info_ptr);
                        }
                      }
                    }
                  }
                  if (rrc_ReleaseCause_unspecified == dcch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.
				  	                                    criticalExtensions.u.r4.rrcConnectionRelease_r4.releaseCause)
                  {
                    rrc_log_protocol_error( (uint8) rrc_get_state(),
                                            (uint8) RRC_PROCEDURE_RCR,
                                            (uint8) RRCLOG_releaseCauseUnspecified,
                                            (uint8) RRCLOG_PROTOCOL_ERROR_NA
                                          );  
                  }
                  /* Update RRC connection release cause */
                  rrc_release_cause = 
                    rrcrcr_update_release_cause(dcch_msg_ptr->message.u.
                    rrcConnectionRelease.u.later_than_r3.criticalExtensions.u.r4.
                   rrcConnectionRelease_r4.releaseCause);
#ifdef FEATURE_QSH_EVENT_METRIC
                  rrcrcr_qsh_update_release_cause();
#endif
  
                  if(dcch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.
				  	u.r4.rrcConnectionRelease_r4.m.n_308Present)
                  {
                    rrcrcr_n_308 = 
                     dcch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.u.r4.
                    rrcConnectionRelease_r4.n_308;
                    n308_presence = TRUE;
                  } 

            
                  if((rrc_get_state() == RRC_STATE_CELL_DCH)
                    &&(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7))         
                  {
                    if((dcch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.
						criticalExtensions.u.r4.m.v4d0NonCriticalExtensionsPresent) &&
                        (dcch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.
                        criticalExtensions.u.r4.v4d0NonCriticalExtensions.m.
                        v690NonCriticalExtensionsPresent))
                    {
                      rrcrcr_process_690non_critical_ext(&dcch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.u.r4.
                          v4d0NonCriticalExtensions.v690NonCriticalExtensions);
                    }
                  }
                  else
                  {
                    rrc_high_mobility_ind_in_rcr = FALSE;
                    WRRC_MSG0_HIGH("High Mobility Ind set to FALSE.");                  
                  }
                  if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8)
                  {
                    if((dcch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions
						.u.r4.m.v4d0NonCriticalExtensionsPresent) &&
                      (dcch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.
                      u.r4.v4d0NonCriticalExtensions.m.v690NonCriticalExtensionsPresent) &&
                      (dcch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.
                      u.r4.v4d0NonCriticalExtensions.v690NonCriticalExtensions.m.v770NonCriticalExtensionsPresent) &&
                      (dcch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.u.r4.v4d0NonCriticalExtensions.
                      v690NonCriticalExtensions.v770NonCriticalExtensions.m.v860NonCriticalExtensionsPresent))
                    {
                      rrcrcr_process_860non_critical_ext(&dcch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.u.r4.
                          v4d0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.rrcConnectionRelease_v860ext);
                    }
                  }

                }
                else
                {
                  WRRC_MSG0_HIGH("RCR Critical Extensions not supported");      
                  /* Send RRC Connection Release Complete Message with Protocol Error
                  and update the release cause to "unspecified" */
                  rrc_release_cause = RRC_REL_CAUSE_UNSPEC;
#ifdef FEATURE_QSH_EVENT_METRIC
                  rrcrcr_qsh_update_release_cause();
#endif
                  /* Set the Error Indication */
                  rrc_protocol_error_information.diagnosticsType.t = 
                  T_rrc_ProtocolErrorInformation_diagnosticsType_type1;
                  
                  rrc_protocol_error_information.diagnosticsType.u.type1 = 
                    rtxMemAllocTypeZ(&enc_ctxt,rrc_ProtocolErrorInformation_type1);

                  rrc_protocol_error_information.diagnosticsType.u.type1->protocolErrorCause = 
                    rrc_ProtocolErrorCause_messageExtensionNotComprehended ;

                  rrcrcr_n_308 = 0;
                  rrcrcr_error_ind = RRCRCR_INCLUDE_ERROR_INDICATION;
                }
              }
              else
              {
                /* Send RRC Connection Release Complete Message with Protocol Error
                   and update the release cause to "unspecified" */
                rrc_release_cause = RRC_REL_CAUSE_UNSPEC;
#ifdef FEATURE_QSH_EVENT_METRIC
                rrcrcr_qsh_update_release_cause();
#endif
                /* Set the Error Indication */
                rrc_protocol_error_information.diagnosticsType.t = 
                T_rrc_ProtocolErrorInformation_diagnosticsType_type1;

                rrc_protocol_error_information.diagnosticsType.u.type1 = 
                  rtxMemAllocTypeZ(&enc_ctxt,rrc_ProtocolErrorInformation_type1);

                rrc_protocol_error_information.diagnosticsType.u.type1->protocolErrorCause = 
                  rrc_ProtocolErrorCause_messageExtensionNotComprehended ;

                rrcrcr_n_308 = 0;
                rrcrcr_error_ind = RRCRCR_INCLUDE_ERROR_INDICATION;
              }
            } /* End of if of later than release 3 present */
            else
            {
              if(RRC_MSG_COMMON_BITMASK_IE(dcch_msg_ptr->message.u.rrcConnectionRelease.u.r3.
               rrcConnectionRelease_r3,rrc_RRCConnectionRelease_r3_IEs,rplmn_information))
              {
                local_rplmn_information = &(dcch_msg_ptr->message.u.rrcConnectionRelease.u.r3.
                   rrcConnectionRelease_r3.rplmn_information);
              }

              if(RRC_MSG_COMMON_BITMASK_IE(dcch_msg_ptr->message.u.rrcConnectionRelease.u.r3,
                  rrc_RRCConnectionRelease_r3,laterNonCriticalExtensions))
              {
                if(dcch_msg_ptr->message.u.rrcConnectionRelease.u.r3.laterNonCriticalExtensions.
					m.v690NonCriticalExtensionsPresent)
                {
                  if(RRC_MSG_COMMON_BITMASK_IE(dcch_msg_ptr->message.u.rrcConnectionRelease.u.r3.laterNonCriticalExtensions.v690NonCriticalExtensions.
                      rrcConnectionRelease_v690ext,rrc_RRCConnectionRelease_v690ext_IEs,redirectionInfo_v690ext))
                  {
                    local_redirect_info_ptr = (rrc_RedirectionInfo_r6 *)&(dcch_msg_ptr->message.u.rrcConnectionRelease.u.r3.laterNonCriticalExtensions.v690NonCriticalExtensions.
                      rrcConnectionRelease_v690ext.redirectionInfo_v690ext);
                    if(local_redirect_info_ptr != NULL)
                    {
                       rrcrcr_save_redirection_info(local_redirect_info_ptr);
                    }
                  }
                }
              }
              if (rrc_ReleaseCause_unspecified == dcch_msg_ptr->message.u.rrcConnectionRelease.u.r3.
			  	                                        rrcConnectionRelease_r3.releaseCause)
              {
                rrc_log_protocol_error( (uint8) rrc_get_state(),
                                        (uint8) RRC_PROCEDURE_RCR,
                                        (uint8) RRCLOG_releaseCauseUnspecified,
                                        (uint8) RRCLOG_PROTOCOL_ERROR_NA
                                      );    
              }
              /* Update RRC connection release cause */
              rrc_release_cause = 
                  rrcrcr_update_release_cause(dcch_msg_ptr->message.u.
                  rrcConnectionRelease.u.r3.rrcConnectionRelease_r3.
                  releaseCause);  
#ifdef FEATURE_QSH_EVENT_METRIC
              rrcrcr_qsh_update_release_cause();
#endif
                
              /* Update Transaction Id */
              rrcrcr_transaction_identifier = dcch_msg_ptr->message.u.
                  rrcConnectionRelease.u.r3.rrcConnectionRelease_r3.
                  rrc_TransactionIdentifier;

              if(RRC_MSG_COMMON_BITMASK_IE(dcch_msg_ptr->message.u.rrcConnectionRelease.u.r3.
                   rrcConnectionRelease_r3,rrc_RRCConnectionRelease_r3_IEs,n_308))
              {
                rrcrcr_n_308 = 
                   dcch_msg_ptr->message.u.rrcConnectionRelease.u.r3.
                   rrcConnectionRelease_r3.n_308;
                n308_presence = TRUE;
              }
              
              if((rrc_get_state() == RRC_STATE_CELL_DCH)
                 && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7))
              {
                if((RRC_MSG_COMMON_BITMASK_IE(dcch_msg_ptr->message.u.rrcConnectionRelease.u.r3,
                    rrc_RRCConnectionRelease_r3,laterNonCriticalExtensions)) &&
                    (dcch_msg_ptr->message.u.rrcConnectionRelease.u.r3.laterNonCriticalExtensions.m.
                     v690NonCriticalExtensionsPresent))
                {
                  rrcrcr_process_690non_critical_ext((rrc_RRCConnectionRelease_v690NonCriticalExtensions_1 *) (&dcch_msg_ptr->message.u.rrcConnectionRelease.u.r3.
                      laterNonCriticalExtensions.v690NonCriticalExtensions));
                }
              }
              else
              {
                rrc_high_mobility_ind_in_rcr = FALSE;
                WRRC_MSG0_HIGH("High Mobility Ind set to FALSE.");                  
              }
              if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8)
              {
                if((dcch_msg_ptr->message.u.rrcConnectionRelease.u.r3.m.laterNonCriticalExtensionsPresent) &&
                    (dcch_msg_ptr->message.u.rrcConnectionRelease.u.r3.laterNonCriticalExtensions.
                    m.v690NonCriticalExtensionsPresent) &&
                    (dcch_msg_ptr->message.u.rrcConnectionRelease.u.r3.laterNonCriticalExtensions.
                    v690NonCriticalExtensions.m.v770NonCriticalExtensionsPresent) &&
                    (dcch_msg_ptr->message.u.rrcConnectionRelease.u.r3.laterNonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.m.
                     v860NonCriticalExtensionsPresent))
                {
                  rrcrcr_process_860non_critical_ext(&dcch_msg_ptr->message.u.rrcConnectionRelease.u.r3.laterNonCriticalExtensions.
                        v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.rrcConnectionRelease_v860ext);
                }
              }
            }

            if(local_rplmn_information != NULL)
            {
              uint32 local_idx=0;
              WRRC_MSG0_HIGH("RPLMN Info in RRC Conn Rel msg");

              /* Validate the Frequency Information and then add it to
               * the frequency/frequencies to the acquisition database
               */
              if(local_rplmn_information->m.fdd_UMTS_Frequency_ListPresent)
              {
                WRRC_MSG0_HIGH("Ignoring RPLMN info as fdd_UMTS_Frequency_List is not present.");

                fdd_freq_list_ptr = &local_rplmn_information->fdd_UMTS_Frequency_List;
              }

              while((fdd_freq_list_ptr != NULL) && (fdd_freq_list_ptr->n > local_idx))
              {
                /* Validate the uarfcn_UL (if present) and uarfcn_dl based on
                 * UE's band preference settings before adding it to the 
                 * acquisition database
                 */
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
                if(validate_fdd_frequency(
                    &fdd_freq_list_ptr->elem[local_idx], rrc_ccm_get_curr_camped_plmn())
                     == FAILURE)
#else
                if(validate_fdd_frequency(&fdd_freq_list_ptr->elem[local_idx])
                     == FAILURE)
#endif
                {
                  WRRC_MSG1_HIGH("Band for freq %d not supported", fdd_freq_list_ptr->elem[local_idx].uarfcn_DL);
                }
                else
                {
                  if(rrc_ccm_get_curr_camped_freq() != (uint16)fdd_freq_list_ptr->elem[local_idx].uarfcn_DL)
                  {
                    if(rrccsp_update_acq_db((uint16)fdd_freq_list_ptr->elem[local_idx].uarfcn_DL) == SUCCESS)
                    {
                      WRRC_MSG0_HIGH("ACQ entry added to ACQ DB");
                      acq_db_updated = TRUE;
                      rrc_rplmn_info_prsnt = TRUE;
                    }
                    else
                    {
                      WRRC_MSG0_ERROR("ACQ entry not added to ACQ DB");
                      break;
                    }
                  }
                  else
                  {
                    WRRC_MSG0_HIGH("fdd list contains curr camped freq");
                    fdd_freq_list_contains_camped_freq = TRUE;
                  }
                }

                local_idx++;  
              }

              /* this is to get curr camped freq at the top of acq db so that
               * if on tx to idle, acq fails on the psc then CSP can still try
               * the camped freq before trying acq on other freqs in the acq db
               */
              if((fdd_freq_list_contains_camped_freq)
                 ||
                 (   (acq_db_updated)
                  && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC1)
                  && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC2)
                  && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC3)
                  && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC4)
                 )
                )
              {
                WRRC_MSG0_HIGH("Add curr camped freq to the ACQ DB");
                (void) rrccsp_update_acq_db(rrc_ccm_get_curr_camped_freq());
                acq_db_updated = TRUE;
              }
            } /* End of if: rplmn_info_presence */

            /* Gets the current state of the RRC */
            current_state = rrc_get_state();

            /* Switch on RRC state */
            switch(current_state)
            {
              case RRC_STATE_CELL_DCH:  
                /* reset the  counters */
                rrc_v_308 = 0;

                if(rel5_critical_extn_supported == FALSE &&
                    (dcch_msg_ptr->message.u.rrcConnectionRelease.t ==
                  T_rrc_RRCConnectionRelease_later_than_r3))
                {
                  /* Build RRC Connection Release complete message and send the
                  same to the SEND CHAIN */ 

                  if(RRCRCR_SUCCESS == 
                    rrcrcr_send_connection_rel_complete_msg(
                    DCCH_UM_RADIO_BEARER_ID,
                    UE_MODE_UNACKNOWLEDGED,
                    rrcrcr_error_ind) )
                  /* This function starts the Timer T308 */
                  {
                     /* Increment the number of retransmission */
                     rrc_v_308++; 

                     /* Also store the scr and pn info of the best cell in the 
                      * active set so that UE first attempts acquisition on this
                      * cell before trying ACQ DB 
                      */
                     if(   ((!acq_db_updated)
                        || ((RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC1)
                         && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC2)
                         && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC3)
                         && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC4)))
                        && (!redirection_info_present)
                       )
                     {
                       rrc_csp_get_aset_scr_pn();
                     }

                     rrcrcr_substate = RRCRCR_WAIT_FOR_T308_TIMER_EXPIRE;
                  }
                  else
                  {
                    /* It is OK even if UE fails to transmit RRC Connection
                    Release Complete. Eventually UTRAN will release the 
                    resources */
                    WRRC_MSG0_ERROR("Unable to send RRC Conn Rel Complete");
                  }
                  return;
                }

                /* Check whether Max number of retransmissions field is present */
                if(n308_presence == TRUE)
                {
                  WRRC_MSG1_HIGH("N308 is %d", rrcrcr_n_308);
                  
                  /* Build RRC Connection Release complete message and send the
                     same to the SEND CHAIN */              
                  if(RRCRCR_SUCCESS == 
                       rrcrcr_send_connection_rel_complete_msg(
                       DCCH_UM_RADIO_BEARER_ID,
                       UE_MODE_UNACKNOWLEDGED, 
                       rrcrcr_error_ind) )

                  /* This function starts the Timer T308 */
                  {
                    /* Increment the number of retransmission */
                    rrc_v_308++; 

                    /* If Connection Release Request from the Network had a 
                     * frequency information then do not try acquisition on
                     * the best cell in the active set
                     */
                    if(   ((!acq_db_updated)
                       || ((RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC1)
                        && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC2)
                        && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC3)
                        && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC4)))
                       && (!redirection_info_present)
                      )
                    {
                      /* Since Connection Release Request from the Network did
                       * not come with frequency information, store the scr and
                       * pn info of the best cell in the active set so that UE 
                       * first attempts acquisition on this cell before trying 
                       * ACQ DB 
                       */       
                      rrc_csp_get_aset_scr_pn();
                    }

                    rrcrcr_substate = RRCRCR_WAIT_FOR_T308_TIMER_EXPIRE;
                  }
                  else
                  {
                    /* It is OK even if UE fails to transmit RRC Connection
                    Release Complete. Eventually UTRAN will release the 
                    resources */
                    WRRC_MSG0_ERROR("Unable to send RRC Conn Rel Complete");
                  }
                }
                else
                {
                  // This is a race condition and  Cell Updated would have started . 
                  // RRC Connection Release procedure transmits RRC Connection
                  // Release Complete and includes error indication.               
                  MSG_LOW("The msg do not conatin conditional field",0,0,0);

                  /* Set the Error Indication */
                  rrc_protocol_error_information.diagnosticsType.t = 
                    T_rrc_ProtocolErrorInformation_diagnosticsType_type1;

                  rrc_protocol_error_information.diagnosticsType.u.type1 = 
                    rtxMemAllocTypeZ(&enc_ctxt,rrc_ProtocolErrorInformation_type1);

                  rrc_protocol_error_information.diagnosticsType.u.type1->
                  protocolErrorCause = rrc_ProtocolErrorCause_messageNotCompatibleWithReceiverState ;  

                  /* Build RRC Connection Release complete message and send the
                     same to the SEND CHAIN */              
                  if(RRCRCR_SUCCESS == 
                       rrcrcr_send_connection_rel_complete_msg(
                       DCCH_UM_RADIO_BEARER_ID,
                       UE_MODE_UNACKNOWLEDGED,
                       rrcrcr_error_ind) )
                    /* This function starts the Timer T308 */
                  {
                    /* Increment the number of retransmission */
                    rrc_v_308++; 

                    /* If Connection Release Request from the Network had a 
                     * frequency information then do not try acquisition on
                     * the best cell in the active set
                     */
                    if(   ((!acq_db_updated)
                        || ((RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC1)
                         && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC2)
                         && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC3)
                         && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC4)))
                         && (!redirection_info_present)
                       )
                    {
                      /* Since Connection Release Request from the Network did
                       * not come with frequency information, store the scr and
                       * pn info of the best cell in the active set so that UE 
                       * first attempts acquisition on this cell before trying 
                       * ACQ DB 
                       */
                      rrc_csp_get_aset_scr_pn();
                    }

                    rrcrcr_substate = RRCRCR_WAIT_FOR_T308_TIMER_EXPIRE;
                  
                  }
                  else
                  {
                    /* It is OK even if UE fails to transmit RRC Connection
                    Release Complete. Eventually UTRAN will release the 
                    resources */
                    WRRC_MSG0_ERROR("Unable to send RRC Conn Rel Complete");
                  }
                }
                break;

              case RRC_STATE_CELL_FACH:

                /* Check whether Max number of retransmissions field is present */
                if(n308_presence == TRUE)
                {
                  // This is a race condition and  Cell Updated would have started . 
                  // RRC Connection Release procedure transmits RRC Connection
                  // Release Complete and includes error indication.          
                  WRRC_MSG0_HIGH("N308 in rrcrcr in CELL_FACH. Ignoring");
                }

                /* Query if CELL UPDATE has a valid C-RNTI to tranmit in
                   CELL_FACH state */
                if(RRCCU_NOT_COMPLETED == 
                     rrccu_get_cell_update_complete_status(RRC_PROCEDURE_RCR) )
                {
                  if(   ((!acq_db_updated)
                     || ((RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC1)
                         && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC2)
                         && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC3)
                         && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC4)))
                      && (!redirection_info_present)
                    )
                  {
                    /* Since Connection Release Request from the Network did
                     * not come with frequency information, store the scr and
                     * pn info of the best cell in the active set so that UE 
                     * first attempts acquisition on this cell before trying 
                     * ACQ DB 
                     */       
                    rrc_csp_get_aset_scr_pn();
                  }

                  /* CELL UPDATE is in process of getting a valid C-RNTI. */
                  rrcrcr_substate = RRCRCR_WAIT_FOR_CELL_UPDATE_COMP_CNF;
                }
                else
                {
                  /* Build RRC Connection Release complete message and send the
                     same to the SEND CHAIN */              
                  if(RRCRCR_SUCCESS == 
                       rrcrcr_send_connection_rel_complete_msg(
                       DCCH_AM_RADIO_BEARER_ID,
                       UE_MODE_ACKNOWLEDGED_DATA, 
                       rrcrcr_error_ind) )
                  /* This function starts the Timer RRCRCR_TIMER_VALUE_FOR_L2_ACK */
                  {
                    if(   ((!acq_db_updated)
                      || ((RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC1)
                          && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC2)
                          && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC3)
                          && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC4)))
                      && (!redirection_info_present)
                     )
                    {
                      /* Since Connection Release Request from the Network did
                       * not come with frequency information, store the scr and
                       * pn info of the best cell in the active set so that UE 
                       * first attempts acquisition on this cell before trying 
                       * ACQ DB 
                       */       
                      rrc_csp_get_aset_scr_pn();
                    }

                    rrcrcr_substate = 
                      RRCRCR_WAIT_FOR_L2ACK_FOR_RRC_CONN_REL_COMPLETE;
                  }
                  else
                  {
                    /* It is OK even if UE fails to transmit RRC Connection
                    Release Complete. Eventually UE will release the 
                    resources after L2 ACK expires */
                    WRRC_MSG0_ERROR("Unable to send RRC Conn Rel Complete");
                  } 
                }
                break;

              default:
                WRRC_MSG1_HIGH("RRC Con Rel Msg rcvd in Invalid state:%d",
                          current_state);
                break;
            }
            /*lint -restore */
          }/* DCCH ASN1 decoding is successful */
        }
        else /* Invalid message is routed to RCR procedure */
        {
          WRRC_MSG1_HIGH("Invalid t: %d for RCR proc",
                    message_choice);
        }
      }
      else if (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == 
                rrc_DL_CCCH_Message_PDU)
      {

        //local var to see if group_release match was success/failure
        boolean group_release_match = FALSE;
        boolean group_release_present = FALSE;
        rrc_RRCConnectionRelease_CCCH_groupIdentity * local_group_release_list;


        message_choice =
          rrc_get_dl_ccch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                               cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);

        if(cmd_ptr->cmd.downlink_sdu_ind.decoded_msg != NULL)
        {
          /* Get the OTA message pointer */
          ccch_msg_ptr = (rrc_DL_CCCH_Message *) 
                       cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;
        }
        else
        { /* ASN1 decoding failed */
          rrc_protocol_error_reject = TRUE;
        }
      
        if(rrc_protocol_error_reject != TRUE) /* CCCH ASN1 decoding successful */
        {
          if( message_choice  == 
             T_rrc_DL_CCCH_MessageType_rrcConnectionRelease)
          {
            WRRC_MSG0_HIGH("Rcvd RRC Connection Release CCCH");
            redirection_info_present = FALSE;
            rrc_release_cause_recvd = rrc_ReleaseCause_unspecified;

            /*lint -save -e613 */
            if(ccch_msg_ptr->message.u.rrcConnectionRelease.t ==
              T_rrc_RRCConnectionRelease_CCCH_later_than_r3 )
            {
              /* Critical externsions go here */
              if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
              {
               
                /* Translate the U-RNTI */
                rrc_translate_urnti(&ccch_msg_ptr->message.u.rrcConnectionRelease.
                                   u.later_than_r3.u_RNTI,
                                   &u_rnti);
                //Check if Group-Release-IE's are received and see if a Group-Release Match is met
                if(ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.t == 
                   T_rrc_RRCConnectionRelease_CCCH_criticalExtensions_3_criticalExtensions)
                {
                  if(ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.u.
                     criticalExtensions.m.groupIdentityPresent)
                  {
                    OSRTDListNode * grp_rel_info_ptr=NULL;
                    group_release_present = TRUE;
  
                    local_group_release_list = &ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.
                      criticalExtensions.u.criticalExtensions.groupIdentity;
                    grp_rel_info_ptr =  local_group_release_list->head;
                    
                    while(grp_rel_info_ptr!=NULL)
                    {
                      if(SUCCESS == rrc_compare_group_release_info(grp_rel_info_ptr->data))
                      {
                        WRRC_MSG0_HIGH("Group-Release Matched");
                        group_release_match = TRUE;
                        break;
                      }
                      grp_rel_info_ptr = grp_rel_info_ptr->next;
                    }
                  }
                  else
                  {
                    WRRC_MSG0_HIGH("groupIdentity is NOT Present");
                  }
                }
  
                if(ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.t == 
                    T_rrc_RRCConnectionRelease_CCCH_criticalExtensions_3_criticalExtensions)
                {
                  if (ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.u.
                      criticalExtensions.criticalExtensions.t == 
                      T_rrc_RRCConnectionRelease_CCCH_criticalExtensions_1_r5)
                  {
                    rrc_release_cause_recvd = ccch_msg_ptr->message.u.rrcConnectionRelease.
                      u.later_than_r3.criticalExtensions.u.criticalExtensions.criticalExtensions.u.r5.
                      rrcConnectionRelease_CCCH_r5.rrcConnectionRelease.releaseCause;
                    WRRC_MSG1_HIGH("Rel cause is %d", rrc_release_cause_recvd);

                    if(ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.
						criticalExtensions.u.criticalExtensions.criticalExtensions.u.r5.m.v690NonCriticalExtensionsPresent)
                    {
                      if(ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.u.criticalExtensions.criticalExtensions.u.r5.
                            v690NonCriticalExtensions.rrcConnectionRelease_v690ext.m.redirectionInfo_v690extPresent)
                      {
                        local_redirect_info_ptr = (rrc_RedirectionInfo_r6 *)&(ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.u.
                           criticalExtensions.criticalExtensions.u.r5.v690NonCriticalExtensions.rrcConnectionRelease_v690ext.redirectionInfo_v690ext);
                        if(local_redirect_info_ptr != NULL)
                        {
                           rrcrcr_save_redirection_info(local_redirect_info_ptr);
                        }
                      }
                    }
                    if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8)
                    {
                      if((ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.u.criticalExtensions.
                        criticalExtensions.u.r5.m.v690NonCriticalExtensionsPresent)
                        && (ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.u.criticalExtensions.
                        criticalExtensions.u.r5.v690NonCriticalExtensions.m.v860NonCriticalExtensionsPresent))
                      {
                        rrcrcr_process_860non_critical_ext(&ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.
                            criticalExtensions.u.criticalExtensions.criticalExtensions.u.r5.v690NonCriticalExtensions.v860NonCriticalExtensions.
                            rrcConnectionRelease_v860ext);
                      }
                    }
                  }
                }
              }
            }
            else
            {
              /* Translate the U-RNTI */
              rrc_translate_urnti(&ccch_msg_ptr->message.u.rrcConnectionRelease.
                                 u.r3.rrcConnectionRelease_CCCH_r3.u_RNTI,
                                 &u_rnti);

              rrc_release_cause_recvd = ccch_msg_ptr->message.u.rrcConnectionRelease.
              u.r3.rrcConnectionRelease_CCCH_r3.rrcConnectionRelease.releaseCause;

              if((ccch_msg_ptr->message.u.rrcConnectionRelease.u.r3.m.laterNonCriticalExtensionsPresent))
              {
                if (ccch_msg_ptr->message.u.rrcConnectionRelease.u.r3.laterNonCriticalExtensions.m.v690NonCriticalExtensionsPresent)
                {
                  if(ccch_msg_ptr->message.u.rrcConnectionRelease.u.r3.laterNonCriticalExtensions.v690NonCriticalExtensions.
                     rrcConnectionRelease_v690ext.m.redirectionInfo_v690extPresent)
                  {
                    local_redirect_info_ptr = (rrc_RedirectionInfo_r6 *)&(ccch_msg_ptr->message.u.rrcConnectionRelease.u.r3.laterNonCriticalExtensions.
                      v690NonCriticalExtensions.rrcConnectionRelease_v690ext.redirectionInfo_v690ext);
                    if(local_redirect_info_ptr != NULL)
                    {
                       rrcrcr_save_redirection_info(local_redirect_info_ptr);
                    }
                  }
                }
              }
              if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8)
              {
                if((ccch_msg_ptr->message.u.rrcConnectionRelease.u.r3.m.laterNonCriticalExtensionsPresent)
                  && (ccch_msg_ptr->message.u.rrcConnectionRelease.u.r3.
                  laterNonCriticalExtensions.m.v690NonCriticalExtensionsPresent) &&
                  (ccch_msg_ptr->message.u.rrcConnectionRelease.u.r3.laterNonCriticalExtensions.
                  v690NonCriticalExtensions.m.v860NonCriticalExtensionsPresent))
                {
                  rrcrcr_process_860non_critical_ext(&ccch_msg_ptr->message.u.rrcConnectionRelease.u.r3.
                    laterNonCriticalExtensions.v690NonCriticalExtensions.v860NonCriticalExtensions.rrcConnectionRelease_v860ext);
                }
              }
            }

            if(U_RNTI_NOT_PRESENT == rrcllc_get_current_urnti(&u_rnti_local) )
            {
              if(U_RNTI_NOT_PRESENT == rrcllc_get_urnti_from_toc(&u_rnti_local))
              {
                /* as u_rnti_local is initialized to 0, but 0 is a valid URNTI, 
                if URNTI is not present in any of the DBs, u_rnti_local is made different from 
                u_rnti by below logic (This logic is needed only if received u_rnti is 0) */
                u_rnti_local = u_rnti+1;
              }
            }

            MSG_HIGH("u_rnti_local %d, u_rnti %d, group_release_match %d is", u_rnti_local, u_rnti, group_release_match);

            //release connection if group release is present and if it was a match  or
            //if group release is absent and the urnti's matched
            if(((TRUE == group_release_present) &&  (TRUE == group_release_match)) ||
               ((FALSE == group_release_present) && (u_rnti_local == u_rnti)))
            {
              if (rrc_ReleaseCause_unspecified == rrc_release_cause_recvd)
              {
                rrc_log_protocol_error( (uint8) rrc_get_state(),
                                        (uint8) RRC_PROCEDURE_RCR,
                                        (uint8) RRCLOG_releaseCauseUnspecified,
                                        (uint8) RRCLOG_PROTOCOL_ERROR_NA
                                      ); 
              }
              //update the release cause
              rrc_release_cause = rrcrcr_update_release_cause(rrc_release_cause_recvd);
#ifdef FEATURE_QSH_EVENT_METRIC
              rrcrcr_qsh_update_release_cause();
#endif  
              if(!redirection_info_present)
              {
                /* Take UE to Idle via CSP by sending init cell sel with tx to discon */
                rrc_transition_to_disconnected_state(RRC_PROCEDURE_RCR, RRC_TX_TO_DISCON_OTHER);

                  WRRC_MSG0_HIGH("Sending init cell sel ind to CSP for tx to discon");
  
                  if(   (!acq_db_updated)
                      || ((RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC1)
                          && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC2)
                          && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC3)
                          && (RRC_CSP_GET_INT_MCC(camped_plmn) != JAPAN_MCC4))
                    )
                  {
                    /* Since Connection Release Request from the Network did
                     * not come with frequency information, store the scr and
                     * pn info of the best cell in the active set so that UE 
                     * first attempts acquisition on this cell before trying 
                     * ACQ DB 
                     */       
                    rrc_csp_get_aset_scr_pn();
                  }
  
                  /* RCR waits for the release confimration from the RRC LLC */
                  rrcrcr_substate = RRCRCR_WAIT_FOR_SRB_RELEASE_CNF;
                }
              else
              {
                rrcrcr_send_redirection_req();
              }
            }
            else
            {
              WRRC_MSG0_HIGH("U_RNTI Mismatch. Ignore");
              /* Delete this message in Crash debug info as this OTA is for other UEs*/
              rrc_crash_debug_remove_previous_ota();
            }
            /*lint -restore */
          } 
          else
          {
            /* Incompatible message is received. Hence Trash it */
            WRRC_MSG2_HIGH("Invalid msg: %d for SDU type: %d rcvd",
                      message_choice,cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type );
          }
        }/* CCCH ASN1 decoding successful */
        else
        {
            /*log the protocol error*/
            rrc_log_protocol_error( (uint8) rrc_get_state(),
                                    (uint8) RRC_PROCEDURE_RCR,
                                    (uint8) T_rrc_FailureCauseWithProtErr_protocolError,
                                    (uint8) rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError
                                  );
        }
      }
      else
      {
        WRRC_MSG1_HIGH("Invalid PDU type: %d is rcvd", 
                  cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type);
      }

#ifdef FEATURE_QSH_EVENT_METRIC
	  conn_rel_params.conn_rel_cause=rrc_release_cause;
	  conn_rel_params.redir_info_present= redirection_info_present;
	  conn_rel_params.redir_rat= saved_redirect_info.choice;
	  rrc_qsh_log_conn_rel_params(&conn_rel_params);
#endif
      break;
    case RRC_STATE_CHANGE_IND:
      MSG_MED("Unexpected Event:%d in the substate: %d",
               cmd_ptr->cmd_hdr.cmd_id,rrcrcr_substate,0);
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcrcr_substate);
  }
 
} /* rrcrcr_process_rcr_initial_substate */


/*===========================================================================

FUNCTION rrcrcr_process_rcr_wait_for_t308_timer_expire_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Release procedure in RRCRCR_WAIT_FOR_T308_TIMER_EXPIRE  substate of 
  CELL_DCH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_process_rcr_wait_for_t308_timer_expire_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{

  switch(cmd_ptr->cmd_hdr.cmd_id)
  {     
    case RRC_DOWNLINK_SDU_IND:       
      WRRC_MSG0_HIGH("RCR proc already started. Msg Ignored");
      /* Delete this message in Crash debug info as this OTA is for other UEs*/
      rrc_crash_debug_remove_previous_ota();
      break;

    case RRC_STATE_CHANGE_IND:
      /* stop the timer T308 */
        rrctmr_stop_timer(RRCTMR_T_308_TIMER);
      
      if((cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED) &&
         ( rrc_get_state() == RRC_STATE_DISCONNECTED))
      {
        /* Reset the RCE state machine */
        rrcrcr_substate = RRCRCR_INITIAL;

      }
      else
      {
        ERR_FATAL("RRCSCMGR posted an unregistered event",
                   cmd_ptr->cmd.state_change_ind.new_state ,0,0);
      }
      break;

     case RRC_T308_EXPIRED_IND:
      if(rrc_v_308 <= rrcrcr_n_308)               
      {        
        /* Resend RRC connection Release Complete Message and this function 
           restarts the T308 Timer */
        if(RRCRCR_SUCCESS == 
             rrcrcr_send_connection_rel_complete_msg(
             DCCH_UM_RADIO_BEARER_ID,
             UE_MODE_UNACKNOWLEDGED,
             RRCRCR_NOT_INCLUDE_ERROR_INDICATION))
        
        {
          rrc_v_308++;               
        }
        else
        { 
          /* It is OK even if UE fails to transmit RRC Connection
          Release Complete. Eventually UTRAN will release the 
          resources */
          WRRC_MSG0_ERROR("Unable to send RRC Conn Rel Complete");
        }
      }
      else /* Reached maximum allowed re-tries. Wait for T308 time before
              releasing the Radio resources */
      {
        /* If saved redirection info from Conn Release is present, send the redirection req to CSP*/
        if(redirection_info_present)
        {
          rrcrcr_send_redirection_req();
        }
        else
        {
          /* Take UE to Idle via CSP by sending init cell sel with tx to discon */
          rrc_transition_to_disconnected_state(RRC_PROCEDURE_RCR, RRC_TX_TO_DISCON_OTHER);
          WRRC_MSG0_HIGH("Sending init cell sel ind to CSP for tx to discon");
          /* RCR waits for the release confimration from the RRC LLC */         
          rrcrcr_substate = RRCRCR_WAIT_FOR_SRB_RELEASE_CNF;    
        }
      }
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcrcr_substate);
      break;
  }          
} /* rrcrcr_process_rcr_wait_for_t308_timer_expire_substate */

/*===========================================================================

FUNCTION rrcrcr_process_rcr_wait_for_cell_update_comp_cnf_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Release procedure in RRCRCR_WAIT_FOR_CELL_UPDATE_COMP_CNF  substate of 
  CELL_FACH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_process_rcr_wait_for_cell_update_comp_cnf_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  rrc_DL_CCCH_Message *ccch_msg_ptr;         /* Pointer to Downlink CCCH
                                                message */
  rrc_int_u_rnti_type u_rnti = 0;            /* Local varibale to store U-RNTI */
 
  rrc_int_u_rnti_type u_rnti_local = 0;          /* Local varibale to store U-RNTI 
                                                received from RRCLLC */

  rrc_ReleaseCause     rrc_release_cause_recvd = rrc_ReleaseCause_unspecified;

  switch(cmd_ptr->cmd_hdr.cmd_id)
  {     
    case RRC_DOWNLINK_SDU_IND:       

      if (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == 
          rrc_DL_CCCH_Message_PDU)
      {
        //local var to see if group_release match was success/failure
        boolean group_release_match = FALSE;
        boolean group_release_present = FALSE;

        if(cmd_ptr->cmd.downlink_sdu_ind.decoded_msg != NULL)
        {                 /* CCCH ASN1 decoding successful */
          ccch_msg_ptr = (rrc_DL_CCCH_Message *) 
                         cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

          if(ccch_msg_ptr->message.t == 
             T_rrc_DL_CCCH_MessageType_rrcConnectionRelease)
          {
            WRRC_MSG0_HIGH("RRC Conn Rel received on CCCH");
            redirection_info_present = FALSE;
            if(ccch_msg_ptr->message.u.rrcConnectionRelease.t ==
              T_rrc_RRCConnectionRelease_CCCH_later_than_r3)
            {
              if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
              {
                rrc_RRCConnectionRelease_CCCH_groupIdentity * local_group_release_list;

                /* Translate the U-RNTI */
                rrc_translate_urnti(&ccch_msg_ptr->message.u.rrcConnectionRelease.
                                   u.later_than_r3.u_RNTI,
                                   &u_rnti);


                //check if Group Release IE's are present and see if Group Release Match is met
                if(ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.t == 
                   T_rrc_RRCConnectionRelease_CCCH_criticalExtensions_3_criticalExtensions)
                {
                  OSRTDListNode * grp_rel_info_ptr=NULL;
                  if(ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.u.
                     criticalExtensions.m.groupIdentityPresent)
                  {
                    group_release_present = TRUE;

                    local_group_release_list = &ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.
                      criticalExtensions.u.criticalExtensions.groupIdentity;
					grp_rel_info_ptr= local_group_release_list->head;

                    while(grp_rel_info_ptr != NULL)
                    {
                      if(SUCCESS == rrc_compare_group_release_info(grp_rel_info_ptr->data))
                      {
                        WRRC_MSG0_HIGH("Group-Release Matched");
                        group_release_match = TRUE;
                        break;
                      }
                      grp_rel_info_ptr = grp_rel_info_ptr->next;;
                    }
                  }
                }

                if(ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.t == 
                    T_rrc_RRCConnectionRelease_CCCH_criticalExtensions_3_criticalExtensions)
                {
                  if (ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.u.
                      criticalExtensions.criticalExtensions.t == 
                      T_rrc_RRCConnectionRelease_CCCH_criticalExtensions_1_r5)
                  {
                    rrc_release_cause_recvd = ccch_msg_ptr->message.u.rrcConnectionRelease.
                      u.later_than_r3.criticalExtensions.u.criticalExtensions.criticalExtensions.u.r5.
                      rrcConnectionRelease_CCCH_r5.rrcConnectionRelease.releaseCause;
                    WRRC_MSG1_HIGH("Rel cause is %d", rrc_release_cause_recvd);
                  }
                }
              }
            }
            else
            {
                rrc_release_cause_recvd = ccch_msg_ptr->message.u.rrcConnectionRelease.
              u.r3.rrcConnectionRelease_CCCH_r3.rrcConnectionRelease.releaseCause;
               /* Translate the U-RNTI */
               rrc_translate_urnti(&ccch_msg_ptr->message.u.rrcConnectionRelease.
                                 u.r3.rrcConnectionRelease_CCCH_r3.u_RNTI,
                                 &u_rnti); 
            }


            if(U_RNTI_NOT_PRESENT == rrcllc_get_current_urnti(&u_rnti_local) )
            {
              if(U_RNTI_NOT_PRESENT == rrcllc_get_urnti_from_toc(&u_rnti_local))
              {
                /* as u_rnti_local is initialized to 0, but 0 is a valid URNTI, 
                if URNTI is not present in any of the DBs, u_rnti_local is made different from 
                u_rnti by below logic (This logic is needed only if received u_rnti is 0) */
                u_rnti_local = u_rnti+1;
              }
            }

            MSG_HIGH("u_rnti_local %d, u_rnti %d, group_release_match %d is", u_rnti_local, u_rnti, group_release_match);

            //release connection if group release is present and if it was a match  or
            //if group release is absent and the urnti's matched
            if(((TRUE == group_release_present) &&  (TRUE == group_release_match)) ||
               ((FALSE == group_release_present) && (u_rnti_local == u_rnti)))
            {
              if (rrc_ReleaseCause_unspecified == rrc_release_cause_recvd)
              {
                rrc_log_protocol_error( (uint8) rrc_get_state(),
                                        (uint8) RRC_PROCEDURE_RCR,
                                        (uint8) RRCLOG_releaseCauseUnspecified,
                                        (uint8) RRCLOG_PROTOCOL_ERROR_NA
                                      );  
              }
              //update the release cause
              rrc_release_cause = rrcrcr_update_release_cause(rrc_release_cause_recvd);
#ifdef FEATURE_QSH_EVENT_METRIC
              rrcrcr_qsh_update_release_cause();
#endif     
              if(!redirection_info_present)
              {
                /* Take UE to Idle via CSP by sending init cell sel with tx to discon */
                rrc_transition_to_disconnected_state(RRC_PROCEDURE_RCR, RRC_TX_TO_DISCON_OTHER);
                  WRRC_MSG0_HIGH("Sending init cell sel ind to CSP for tx to discon");
  
                  /* RCR waits for the release confimration from the RRC LLC */         
                  rrcrcr_substate = RRCRCR_WAIT_FOR_SRB_RELEASE_CNF;    

              }
              else
              {
                rrcrcr_send_redirection_req();
              }
            }
            else
            {
               WRRC_MSG0_HIGH("U-RNTI Mismatch. Ignore");
              /* Delete this message in Crash debug info as this OTA is for other UEs*/
              rrc_crash_debug_remove_previous_ota();
            }
          } 

        }/* CCCH ASN1 decoding successful */
        else
        {
          WRRC_MSG0_ERROR("Failed to DECODE RCR message");

          rrc_log_protocol_error( (uint8) rrc_get_state(),
                                  (uint8)RRC_PROCEDURE_RCR,
                                  (uint8) T_rrc_FailureCauseWithProtErr_protocolError,
                                  (uint8) rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError
                                );
        }
      }
      else
      {
          /* Incompatible message is received. Hence Trash it */
          WRRC_MSG1_HIGH("Incompatible for SDU type: %d received",
                    cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type );
      }

      break;

    case RRC_STATE_CHANGE_IND:
            
      if((cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED) &&
         ( rrc_get_state() == RRC_STATE_DISCONNECTED))
      {
        /* Reset the RCE state machine */
        rrcrcr_substate = RRCRCR_INITIAL;

      }
      else
      {
        ERR_FATAL("RRCSCMGR posted an unregistered event",
                   cmd_ptr->cmd.state_change_ind.new_state ,0,0);
      }
      break;

    case RRC_CELL_UPDATE_COMPLETE_CNF:
      if(cmd_ptr->cmd.cu_complete_cnf.procedure == RRC_PROCEDURE_RCR)
      {                
        /* Send RRC connection Release Complete Message since RRC has valid
           C-RNTI */
        if(RRCRCR_SUCCESS == 
             rrcrcr_send_connection_rel_complete_msg(
             DCCH_UM_RADIO_BEARER_ID,
             UE_MODE_UNACKNOWLEDGED,
             rrcrcr_error_ind))
        
        {
          rrcrcr_substate = 
            RRCRCR_WAIT_FOR_L2ACK_FOR_RRC_CONN_REL_COMPLETE;
        }
        else
        {
          ERR_FATAL("Unable to send RRC Conn Rel Complete",0,0,0);
        }
      }
      {
        WRRC_MSG1_HIGH("RRC_CELL_UPDATE_COMPLETE_CNF's proc: rrc_proc_e_type_value%d",
                  cmd_ptr->cmd.cu_complete_cnf.procedure);
      }
      
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcrcr_substate);
      break;
  }          
} /* rrcrcr_process_rcr_wait_for_cell_update_comp_cnf_substate */


/*===========================================================================

FUNCTION rrcrcr_process_rcr_wait_for_l2ack_for_rrc_conn_rel_complete_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Release procedure in RRCRCR_WAIT_FOR_L2ACK_FOR_RRC_CONN_REL_COMPLETE
  substate of CELL_FACH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_process_rcr_wait_for_l2ack_for_rrc_conn_rel_complete_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  rrc_DL_CCCH_Message *ccch_msg_ptr;         /* Pointer to Downlink CCCH
                                                message */
  rrc_int_u_rnti_type u_rnti = 0;            /* Local varibale to store U-RNTI */
 
  rrc_int_u_rnti_type u_rnti_local = 0;          /* Local varibale to store U-RNTI 
                                                received from RRCLLC */

  rrc_ReleaseCause     rrc_release_cause_recvd = rrc_ReleaseCause_unspecified;

  switch(cmd_ptr->cmd_hdr.cmd_id)
  {     
    case RRC_DOWNLINK_SDU_IND:

      if (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == 
          rrc_DL_CCCH_Message_PDU) 
      {
        //local var to see if group_release match was success/failure
        boolean group_release_match = FALSE;
        boolean group_release_present = FALSE;

        if(cmd_ptr->cmd.downlink_sdu_ind.decoded_msg != NULL)
        {                 /* CCCH ASN1 decoding successful */
          ccch_msg_ptr = (rrc_DL_CCCH_Message *) 
                         cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

          if(ccch_msg_ptr->message.t == 
             T_rrc_DL_CCCH_MessageType_rrcConnectionRelease)
          {
            WRRC_MSG0_HIGH("RRC Conn Rel received on CCCH");
            redirection_info_present = FALSE;

            if(ccch_msg_ptr->message.u.rrcConnectionRelease.t ==
              T_rrc_RRCConnectionRelease_CCCH_later_than_r3)
            {
              if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
              {
                rrc_RRCConnectionRelease_CCCH_groupIdentity * local_group_release_list;

                /* Translate the U-RNTI */
                rrc_translate_urnti(&ccch_msg_ptr->message.u.rrcConnectionRelease.
                                   u.later_than_r3.u_RNTI,
                                   &u_rnti);


                //check if Group Release IE's are present and see if Group Release Match is met
                if(ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.t == 
                   T_rrc_RRCConnectionRelease_CCCH_criticalExtensions_3_criticalExtensions)
                {
                  OSRTDListNode * grp_rel_info_ptr=NULL;
                  if(ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.u.
                     criticalExtensions.m.groupIdentityPresent)
                  {
                    group_release_present = TRUE;
  
                    local_group_release_list = &ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.
                      criticalExtensions.u.criticalExtensions.groupIdentity;
					grp_rel_info_ptr= local_group_release_list->head;
  
                    while(NULL != grp_rel_info_ptr)
                    {
                      if(SUCCESS == rrc_compare_group_release_info(grp_rel_info_ptr->data))
                      {
                        WRRC_MSG0_HIGH("Group-Release Matched");
                        group_release_match = TRUE;
                        break;
                      }
                      grp_rel_info_ptr = grp_rel_info_ptr->next;
                    }
                  }
                }
  
                if(ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.t == 
                    T_rrc_RRCConnectionRelease_CCCH_criticalExtensions_3_criticalExtensions)
                {
                  if (ccch_msg_ptr->message.u.rrcConnectionRelease.u.later_than_r3.criticalExtensions.u.
                      criticalExtensions.criticalExtensions.t == 
                      T_rrc_RRCConnectionRelease_CCCH_criticalExtensions_1_r5)
                  {
                    rrc_release_cause_recvd = ccch_msg_ptr->message.u.rrcConnectionRelease.
                      u.later_than_r3.criticalExtensions.u.criticalExtensions.criticalExtensions.u.r5.
                      rrcConnectionRelease_CCCH_r5.rrcConnectionRelease.releaseCause;
                    WRRC_MSG1_HIGH("Rel cause is %d", rrc_release_cause_recvd);
                  }
                }
              }
            }
            else
            {
                rrc_release_cause_recvd = ccch_msg_ptr->message.u.rrcConnectionRelease.
              u.r3.rrcConnectionRelease_CCCH_r3.rrcConnectionRelease.releaseCause;
               /* Translate the U-RNTI */
               rrc_translate_urnti(&ccch_msg_ptr->message.u.rrcConnectionRelease.
                                 u.r3.rrcConnectionRelease_CCCH_r3.u_RNTI,
                                 &u_rnti); 
            }

            if(U_RNTI_NOT_PRESENT == rrcllc_get_current_urnti(&u_rnti_local) )
            {
              if(U_RNTI_NOT_PRESENT == rrcllc_get_urnti_from_toc(&u_rnti_local))
              {
                /* as u_rnti_local is initialized to 0, but 0 is a valid URNTI, 
                if URNTI is not present in any of the DBs, u_rnti_local is made different from 
                u_rnti by below logic (This logic is needed only if received u_rnti is 0) */
                u_rnti_local = u_rnti+1;
              }
            }

            MSG_HIGH("u_rnti_local %d, u_rnti %d, group_release_match %d is", u_rnti_local, u_rnti, group_release_match);

            //release connection if group release is present and if it was a match  or
            //if group release is absent and the urnti's matched
            if(((TRUE == group_release_present) &&  (TRUE == group_release_match)) ||
               ((FALSE == group_release_present) && (u_rnti_local == u_rnti)))
            {
            if (rrc_ReleaseCause_unspecified == rrc_release_cause_recvd)
            {
              rrc_log_protocol_error( (uint8) rrc_get_state(),
                                      (uint8) RRC_PROCEDURE_RCR,
                                      (uint8) RRCLOG_releaseCauseUnspecified,
                                      (uint8) RRCLOG_PROTOCOL_ERROR_NA
                                    );  
            }
              //update the release cause
              rrc_release_cause = rrcrcr_update_release_cause(rrc_release_cause_recvd);
#ifdef FEATURE_QSH_EVENT_METRIC
              rrcrcr_qsh_update_release_cause();
#endif      
              /* Stop the timer for L2 ACK  */
              rrctmr_stop_timer(RRCTMR_RCR_L2ACK_TIMER);

              if(!redirection_info_present)
              {
                /* Take UE to Idle via CSP by sending init cell sel with tx to discon */
                rrc_transition_to_disconnected_state(RRC_PROCEDURE_RCR, RRC_TX_TO_DISCON_OTHER);
                WRRC_MSG0_HIGH("Sending init cell sel ind to CSP for tx to discon");
  
                /* RCR waits for the release confimration from the RRC LLC */         
                rrcrcr_substate = RRCRCR_WAIT_FOR_SRB_RELEASE_CNF;    

              }
              else
              {
                rrcrcr_send_redirection_req();
              }
            }
            else
            {
               WRRC_MSG0_HIGH("U-RNTI Mismatch. Ignore");
              /* Delete this message in Crash debug info as this OTA is for other UEs*/
              rrc_crash_debug_remove_previous_ota();
            }
          } 

        }/* CCCH ASN1 decoding successful */
        else
        {
          WRRC_MSG0_ERROR("Failed to DECODE RCR message");

          rrc_log_protocol_error( (uint8) rrc_get_state(),
                                  (uint8) RRC_PROCEDURE_RCR,
                                  (uint8) T_rrc_FailureCauseWithProtErr_protocolError,
                                  (uint8) rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError
                                 );
        }
      }
      else
      {
          /* Incompatible message is received. Hence Trash it */
          WRRC_MSG1_HIGH("Incompatible for SDU type: %d received",
                    cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type );
      }

      break;

    case RRC_STATE_CHANGE_IND:
      /* Stop the timer for L2 ACK  */
      rrctmr_stop_timer(RRCTMR_RCR_L2ACK_TIMER);

      if((cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED) &&
         ( rrc_get_state() == RRC_STATE_DISCONNECTED))
      {        
        /* Reset the RCR state machine */
        rrcrcr_substate = RRCRCR_INITIAL;

      }
      else
      {
        ERR_FATAL("RRCSCMGR posted an unregistered event:%d",
                   cmd_ptr->cmd.state_change_ind.new_state ,0,0);
      }
      break; 

    case RRC_NEW_CELL_IND:
      /* Stop the timer for L2 ACK  */
      rrctmr_stop_timer(RRCTMR_RCR_L2ACK_TIMER);

      rrcrcr_cell_reselection_occured = TRUE;
      MSG_LOW("New cell ind %d", rrcrcr_cell_reselection_occured,0,0);
      break;

    case RRC_TIMER_FOR_RCR_L2ACK_EXPIRED_IND:
      /* There is a serious problem with RLC for Acknowledged mode
         transmission. However inform the NAS  and Release radio 
         resources and reset the state machine */
      WRRC_MSG0_HIGH("RCR L2 ACK sanity Timer expired"); 

      /* If saved redirection info from Conn Release is present, send the redirection req to CSP*/
      if(redirection_info_present)
      {
        rrcrcr_send_redirection_req();
      }
      else 
      {
        /* Take UE to Idle via CSP by sending init cell sel with tx to discon */
         rrc_transition_to_disconnected_state(RRC_PROCEDURE_RCR, RRC_TX_TO_DISCON_OTHER);
          WRRC_MSG0_HIGH("Sending init cell sel ind to CSP for tx to discon");
  
          /* Reset the RCR state machine */
          rrcrcr_substate = RRCRCR_INITIAL;
  
          /* Update the release cause */
          rrc_release_cause = RRC_REL_DEEP_FADE;
        }
      break;

    case RRC_DOWNLINK_L2ACK_IND:
      /* Stop the timer for L2 ACK  */
      rrctmr_stop_timer(RRCTMR_RCR_L2ACK_TIMER);

      if(cmd_ptr->cmd.downlink_l2ack_ind.status == SUCCESS)
      {
        /* If saved redirection info from Conn Release is present, send the redirection req to CSP*/
        if(redirection_info_present)
        {
          rrcrcr_send_redirection_req();
        }
        else
        {
          /* Take UE to Idle via CSP by sending init cell sel with tx to discon */
          rrc_transition_to_disconnected_state(RRC_PROCEDURE_RCR, RRC_TX_TO_DISCON_OTHER);
            WRRC_MSG0_HIGH("Sending init cell sel ind to CSP for tx to discon");
  
            /* RCR waits for the release confirmation from the RRC LLC */         
            rrcrcr_substate = RRCRCR_WAIT_FOR_SRB_RELEASE_CNF;    
        }
      }
      else /* Failed to recieve successful L2ACK */
      {
        WRRC_MSG0_HIGH("L2ACK for RRC Conn Rel Comp Msg not rcvd");

        /* If saved redirection info from Conn Release is present, send the redirection req to CSP*/
        if(redirection_info_present)
        {
          rrcrcr_send_redirection_req();
        }
        else
        {
          /* Take UE to Idle via CSP by sending init cell sel with tx to discon */
          rrc_transition_to_disconnected_state(RRC_PROCEDURE_RCR, RRC_TX_TO_DISCON_OTHER);
          WRRC_MSG0_HIGH("Sending init cell sel ind to CSP for tx to discon");
            /* Reset the procedure. */   
          rrcrcr_substate = RRCRCR_INITIAL;
  
          /* Update the release cause */
          rrc_release_cause = RRC_REL_DEEP_FADE;
          }
      }
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcrcr_substate);
      break;
  }          
} /* rrcrcr_process_rcr_wait_for_l2ack_for_rrc_conn_rel_complete_substate */
/*===========================================================================

FUNCTION rrcrcr_process_rcr_wait_for_srb_release_cnf_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Release procedure in RRCRCR_WAIT_FOR_SRB_RELEASE_CNF substate of either
  CELL_DCH State or CELL_FACH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_process_rcr_wait_for_srb_release_cnf_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_STATE_CHANGE_IND:
      if((cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED) &&
         ( rrc_get_state() == RRC_STATE_DISCONNECTED) )
      {        
        /* Reset the RCR state machine */
        rrcrcr_substate = RRCRCR_INITIAL;
      }
      else
      {
        ERR_FATAL("RRCSCMGR posted an unregistered event:%d",
                   cmd_ptr->cmd.state_change_ind.new_state ,0,0);
      }
      break; 

    case RRC_CHANNEL_CONFIG_CNF:
      /* It doesn't matter whether RRC LLC successfully releases Radio
         resources or not */
      if(cmd_ptr->cmd.chan_config_cnf.procedure == RRC_PROCEDURE_RCR)
      {
        /* The RCR procedure ends */
        rrcrcr_substate = RRCRCR_INITIAL; 
      }
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcrcr_substate);
      break;
  }

} /* rrcrcr_process_rcr_wait_for_srb_release_cnf_substate */


/*===========================================================================

FUNCTION rrcrcr_is_proc_active

DESCRIPTION
  Returns rcr sub-state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean rrcrcr_is_proc_active
(
  void
)
{
  
  /* Resets the RCR state machine */
  if (rrcrcr_substate == RRCRCR_INITIAL)
  {
    return FALSE;
  }
  else
  {
  WRRC_MSG2_HIGH("RRCRCR st:%d, rrc_st:%d", rrcrcr_substate, rrc_get_state());
    return TRUE;
  }

}
/*===========================================================================

FUNCTION rrcrcr_is_proc_active_sub

DESCRIPTION
  Indicates whether RCR is active or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if RCR is active.
  FALSE if RCR is not active.

SIDE EFFECTS
  None
===========================================================================*/
boolean rrcrcr_is_proc_active_sub
(
  sys_modem_as_id_e_type as_id
)
{
#ifdef FEATURE_DUAL_SIM
  if((!rrc_is_sim_id_valid) || (as_id != rrc_sim_id_recvd))
  {
    return FALSE;
  }
#endif
  /* Resets the RCR state machine */
  if (rrcrcr_substate == RRCRCR_INITIAL)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}
/*===========================================================================

FUNCTION rrcrcr_process_690non_critical_ext

DESCRIPTION
  Processes 690 non critical extension of RRC Conn Rel OTA.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_process_690non_critical_ext
(
  rrc_RRCConnectionRelease_v690NonCriticalExtensions_1 * rcr_msg_ptr
)
{
  if((rcr_msg_ptr->m.v770NonCriticalExtensionsPresent) &&
      (rcr_msg_ptr->v770NonCriticalExtensions.rrcConnectionRelease_v770ext.m.
      ueMobilityStateIndicatorPresent))
  {
    rrc_high_mobility_ind_in_rcr = TRUE;
  }
  else
  {
    rrc_high_mobility_ind_in_rcr = FALSE;
  }
  WRRC_MSG1_HIGH("rrc_high_mobility_ind_in_rcr = %d",rrc_high_mobility_ind_in_rcr);
}

/*===========================================================================
FUNCTION rrcrcr_send_redirection_req

DESCRIPTION
  This function builds the RRC_REDIRECT_REQ command with all parameters
  received through RRC CONNECTION REJECT message and sends the command to
  Cell Selection Procedure
  
DEPENDENCIES
  None

RETURN VALUE
  rrcrcr_status_e_type : Returns RRCRCR_SUCCESS if it successfully sends
  RRC_REDIRECT_REQ command. Otherwise returns RRCRCR_FAILURE_MAJOR

SIDE EFFECTS
  None
===========================================================================*/
void rrcrcr_send_redirection_req()
{
 
  rrc_cmd_type *cmd_ptr;                     /* Pointer to RRC Command */
  uint32 i;
  rrc_state_e_type rrc_curr_state = rrc_get_state();
                                             /* The status is initialized to
                                                Failure */
  rrclog_wcdma_to_wcdma_redir_start_event_type wtow_redir_event;
  switch(saved_redirect_info.choice)
  {
    case RRC_FREQ_REDIRECTION: 
      WRRC_MSG0_HIGH("Freq redirection due to connection release. Send init cell sel ind to CSP for tx to discon");
  
  #ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
      if((SUCCESS == validate_dl_arfcn(
                        saved_redirect_info.u.freq.uarfcn_DL,
                        rrc_ccm_get_curr_camped_plmn(),
                        W_UARFCN_NOT_FROM_GSM_NBR_LIST
                        , TRUE
                        )) )
  #else
      if(SUCCESS == validate_dl_arfcn(
                        saved_redirect_info.u.freq.uarfcn_DL
                        , TRUE
                        ) )
  #endif
      {
        if(rrccsp_update_acq_db(saved_redirect_info.u.freq.uarfcn_DL) == SUCCESS)
        {
          WRRC_MSG0_HIGH("Redirect frequency info added to ACQ DB");
        }
      }
      /* Take UE to Idle via CSP by sending init cell sel with tx to discon */
      rrc_transition_to_disconnected_state(RRC_PROCEDURE_RCR, RRC_TX_TO_DISCON_OTHER);
      wtow_redir_event.redir_cause = WTOW_REDIR_CONN_RELEASE;
      wtow_redir_event.target_freq = saved_redirect_info.u.freq.uarfcn_DL;
      event_report_payload(EVENT_WCDMA_TO_WCDMA_REDIRECTION_START, sizeof(wtow_redir_event),
                 (void *)&wtow_redir_event);
      rrc_ifreq_rdr_in_prgrs = TRUE;
      break;
  
    case RRC_INTER_SYSTEM_GSM:
#ifdef FEATURE_WCDMA_TO_LTE
    case RRC_INTER_SYSTEM_LTE:
#endif
      WRRC_MSG0_HIGH("Interrat redirection due to connection rej");
  
      if(rrcmcm_is_dualmode_enabled() && saved_redirect_info.choice == RRC_INTER_SYSTEM_GSM
                 && ((rrc_curr_state == RRC_STATE_CELL_DCH) || (rrc_curr_state == RRC_STATE_CELL_FACH))
#ifdef FEATURE_FEMTO_CSG
                 && (rrc_csp_int_data.network_select_mode != SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)
#endif
        )
      {
        if(WTOW_SEARCH_STOP != rrc_csp_bplmn_srch_in_progress())
        {
          rrccsp_cleanup_bplmn_state();
        }

        /* Allocates the buffer to RRC Internal command */
        if( (cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
        {     
          /* Fill in the command parameters to RRC_REDIRECT_REQ Command */
          cmd_ptr->cmd_hdr.cmd_id = RRC_REDIRECT_REQ;
          cmd_ptr->cmd.redirect_req.proc_id = RRC_PROCEDURE_RCR;
          cmd_ptr->cmd.redirect_req.wait_time = 0;
          cmd_ptr->cmd.redirect_req.psc = 513; /* initialize the psc to an invalid 
      value */
      
          cmd_ptr->cmd.redirect_req.choice = saved_redirect_info.choice;
  
          if(saved_redirect_info.u.GSM_cell_info_list.size != 0)
          {
            for(i = 0;i < saved_redirect_info.u.GSM_cell_info_list.size;i++)
            {
                   /* Get bcch_ARFCN */
                   cmd_ptr->cmd.redirect_req.u.GSM_cell_info_list.arfcns[i].num
                      = saved_redirect_info.u.GSM_cell_info_list.arfcns[i].num;
        
                   cmd_ptr->cmd.redirect_req.u.GSM_cell_info_list.arfcns[i].band
                      = saved_redirect_info.u.GSM_cell_info_list.arfcns[i].band;
         
                   cmd_ptr->cmd.redirect_req.u.GSM_cell_info_list.BSIC[i].BSIC_NCC 
                      = saved_redirect_info.u.GSM_cell_info_list.BSIC[i].BSIC_NCC;
        
                   cmd_ptr->cmd.redirect_req.u.GSM_cell_info_list.BSIC[i].BSIC_BCC
                      = saved_redirect_info.u.GSM_cell_info_list.BSIC[i].BSIC_BCC;
            }
            cmd_ptr->cmd.redirect_req.u.GSM_cell_info_list.size = saved_redirect_info.u.GSM_cell_info_list.size;
          }
          /* sends the command to Cell Selection Procedure */
          rrc_put_int_cmd(cmd_ptr);         
        }
      }
      else
#ifdef FEATURE_WCDMA_TO_LTE
      if(rrcmcm_is_lte_mode_enabled() && saved_redirect_info.choice == RRC_INTER_SYSTEM_LTE
           && ((rrc_curr_state == RRC_STATE_CELL_DCH) || (rrc_curr_state == RRC_STATE_CELL_FACH))
#ifdef FEATURE_FEMTO_CSG
           && (rrc_csp_int_data.network_select_mode != SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)
#endif
       )
      {
        WRRC_MSG0_HIGH("Send LTE redirection info");

        if(WTOW_SEARCH_STOP != rrc_csp_bplmn_srch_in_progress())
        {
          rrccsp_cleanup_bplmn_state();
        }

        /* Allocates the buffer to RRC Internal command */
        if( (cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
        {
          /* Fill in the command parameters to RRC_REDIRECT_REQ Command */
          cmd_ptr->cmd_hdr.cmd_id = RRC_REDIRECT_REQ;
          cmd_ptr->cmd.redirect_req.proc_id = RRC_PROCEDURE_RCR;
          cmd_ptr->cmd.redirect_req.wait_time = 0;
          cmd_ptr->cmd.redirect_req.psc = 513; /* initialize the psc to an invalid 
      value */

          cmd_ptr->cmd.redirect_req.choice = saved_redirect_info.choice;

          cmd_ptr->cmd.redirect_req.u.EUTRA_target_cell_info = saved_redirect_info.u.EUTRA_target_cell_info;

          /* sends the command to Cell Selection Procedure */
          rrc_put_int_cmd(cmd_ptr);         
        }
      }
      else
#endif
      {
        WRRC_MSG0_HIGH("iRAT Redirection rcvd when Interrat Mode is not enabled.or feature disabled Trans to disconn");
  #ifdef FEATURE_DISCONNECT_ON_UNSUPPORTED_IRAT_REDIRECTION
        if ((rrc_establishment_cause == RRC_EST_EMERGENCY_CALL) &&
             rrccsp_check_gsm_bands_supported())                               
        {
          rrc_transition_to_disconnected_state(RRC_PROCEDURE_RCR,
                   RRC_TX_TO_DISCON_UNSUPPORTED_REDIRECTION);
        }
        else
  #endif
        {
          /* Take UE to Idle via CSP by sending init cell sel with tx to discon */
          rrc_transition_to_disconnected_state(RRC_PROCEDURE_RCR, RRC_TX_TO_DISCON_OTHER);
        }
      } 

      break;
  
    default:     
      WRRC_MSG0_ERROR("This should never happen");
      break;
  }      
  
  rrcrcr_substate = RRCRCR_WAIT_FOR_SRB_RELEASE_CNF;
   
} /* rrcrcr_send_redirection_req */   
/*===========================================================================
FUNCTION rrcrcr_get_current_sub_state()
       
DESCRIPTION
  This function will return current RRC Connection Release 
  procedure's sub state.
DEPENDENCIES
  None

RETURN VALUE
  return current rcr sub state.
SIDE EFFECTS
none
===========================================================================*/
rrcrcr_substate_e_type  rrcrcr_get_current_sub_state(void)
{
  return rrcrcr_substate;
} /*rrcrce_get_rcr_sub_state*/         
/*===========================================================================
FUNCTION rrcrcr_return_substate

DESCRIPTION
returns rcr substate 
DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  rrcrcr_substate_e_type
===========================================================================*/       
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
uint8 rrcrcr_return_substate(void)
{
  return rrcrcr_substate;
}
