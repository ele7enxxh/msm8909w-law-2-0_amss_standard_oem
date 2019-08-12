/*===========================================================================
         R R C   A C T I V E    S E T    U P D A T E    M O D U L E 

DESCRIPTION

  This module performs the Active Set Update Procedure. It processes
  the active set update message and sends a command to L1 to add and/or
  remove the radio-links as indicated by the message. Then it sends
  an active set update complete message to the UTRAN.
  
  If there is a failure this module sends an Active Set Update failure
  message with the appropriate failure cause.
     
  The UE is required to process upto 4 Active Set Update Messages in
  parallel (Since the Transaction Identifier allows upto 4 different
  ASU messages at a time). However in the initial release only one ASU
  message is allowed at a time. The module is designed to allow four
  separate state machines to process four different messages but currently
  only the first one will be processed by this module due to limitations
  in the RRC-L1 interface.
  
  
EXTERNALIZED FUNCTIONS

  rrcasu_procedure_event_handler   Event handler for this procedure
  rrcasu_init_procedure            Power-up initialization of the procedure

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Procedure must be initialized at power-up by calling rrcasu_init_procedure. 
  
Copyright (c) 2001-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcasu.c_v   1.7   22 Feb 2002 17:39:54   xgao  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcasu.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/16   sg      Made changes to reduce F3s
05/09/16   sp      Made changes to convert ota metric msg_id to enum
05/09/16   ad      Made changes to log important parms from OTAs
05/09/16   sp      Made changes for Mav debug snapshot
10/16/15   as      Added code to support LTA durting activation time
09/03/14   sp      Made changes to save procedure substates in crash debug info
07/18/14   ad      Made changes to reduce F3s
06/03/14   sp      Made changes to fix KW error to not access array out of its bounds in rrcasu_procedure_event_handler function
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
09/16/13   mp      Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
08/14/13   vg      Made changes to implement a new diag event EVENT_WCDMA_RRC_FAILURE
08/13/13   vg      Made changes to clear ASU database when ignoring received cmd due to cell update in progress etc
08/06/13   db      Made changes to abort GPS event when ASU does HS repointing
                   or removes PCCPCH RL
07/16/13   vi      Made changes to read GPS-cell id on HS serving cell if HS is active
04/25/13   sg      Added code for REL9 under FEATURE_WCDMA_REL9
01/03/13   ad      Made changes to reduce the redundant F3s
10/18/12   gv      Replace FEATURE_DUAL_SIM_WCDMA_TUNEAWAY with FEATURE_DUAL_SIM
12/28/11   md      Added fix to process ASU message upon killing GET_SPEC_SIB
                   event for Cell ID in DCH reading when N-BCCH is not being set up
12/09/11   ad      Added code to log WCDMA_RRC_PROTOCOL_ERRORS packet
08/16/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.104.00
08/04/11   su      Added Code changes to kill the GET_SPEC_SIB 
                   event for Cell ID in DCH reading, instead of 
                   Error Fatal when N-BCCH being setup or not is being 
                   checked as part of ASU procedure.
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/19/11   as      Fixed compilation error
04/18/11   as      Made changes to ignore ASU if CU is initiated
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
03/17/10   as      Resolved Errors due to ASN.1 8.9 Migration
03/15/10   as      Updated RRC Code to support ASN.1 8.9 version
03/02/10   as      Fixed Klockwork issues
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
12/08/08   ps      Made changes to reject an ASU if an reconfig msg is in progress
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
06/13/08   gkg     Added code for notification of Cell_Id change through OTAs
                   in Cell_DCH state.
03/07/08   da      Compilation warning fix.
03/06/08   da      Added MBMS phase 2C changes.  MBMS support in CELL_DCH.  
                   Includes mobility and dynamic resource management.
03/06/08   da      Made changes required to migrate to 09.2007 R6 version of
                   ASN.1
01/08/08   ps      Added code to reset OC incase of Channel config failure with
                   cause RRCLLC_CHAN_CFG_FAILURE_INCOMPATIBLE_STATE and 
                   RRCLLC_CHAN_CFG_FAILURE_OTHER
11/28/07   da      Made change needed for migration to R6 Jun 07 version of ASN1
10/23/07   kp      Added support for network sharing feature. Code changes to handle 
                   primary PLMN I.D in OTA message. Changes are under feature flag 
                   FEATURE NETWORK SHARING.
09/10/07   da      Made change to send ASU failure with cause invalid config 
                   if failure cause in chan_config_cnf is RRCLLC_CHAN_CFG_FAILURE_OTHER
08/31/07   ps      rrcgps.h file is removed and code is move to rrcmeasi.h. So 
                   adding rrcmeasi.h in header instead of rrcgps.h
08/08/07   ps      Added support for feature cell id reading in DCH state, 
                   under feature flag FEATURE_WCDMA_GPS_CELL_ID_READING.
05/21/07   ps      Made changes to reject ASU when received in Standby State.
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/02/06   tkk     Added support to return appropriate failure cause for
                   ASU message handling.
03/23/05   da      Set asu substate to initial if cell update procedure
                   is active when proc gets chan_config_cnf 
03/23/05   da      Added check to see if Cell update procedure is active before
                   processing chan_config_cnf
08/19/04   sgk     Set the protocol error cause to enum value 
                   rrc_messageNotCompatibleWithReceiverState (value 2) 
                   defined in rrcasn1.h in function process_asu_message to correct error 
                   Type mismatch (assignment) (int/enum). Earlier it was being set
                   to rrc_messageNotCompatibleWithReceiverState_chosen (value 3)  
11/13/03   jh      Changed to allow an ASU to proceed even if ordered config
                   is set, as long as lower layers have already been configured.
                   Disallowed ASU procedure from clearing ordered config if a
                   reconfiguration message is under process.  Removed clearing
                   of ordered config when RRC moves out of DCH state during an
                   ASU.
02/05/03   xfg     Merged Vikas's change for rrcllc_set_ordered_config.
02/24/03   xfg     Made changes that are required by lint.
10/24/02   rj      Updated to use rrc_malloc and rrc_free to use dual heaps 
                   in place of mem_malloc and mem_free to avoid memory
                   fragmentation
09/20/02   bu      Changed l1_status to chan_cfg_status in chan_config_cnf &
                   call rrcllc_get_ordered_config_status() instead of
                   rrcllc_is_ordered_config_set().
02/22/02   xfg     Removed waiting for L2 ACK substate. The procedure ends 
                   once an ASU Complete message is passed on to RLC. Also
                   Cleard OC set flag if failed to config L1.
12/05/01   xfg     Made the following changes for 12-00 to 06-01 spec.
                   conversion:
                   1) send CN Information to NAS if CN Info is present in ASU
                   message;
                   2) Added a check for decoded_msg NULL case: If decoded_msg
                   is set to NULL, send a ASU failure message to UTRAN with a
                   case of the protocol error;
                   3) Checked the RRC state when the ASU is received. If the RRC
                   is in a state other than CELL_DCH, Send a ASU failure message
                   to UTRAN with the proper cause code.
10/25/01   rj      Added code to initiate CELL UPADTE when ASU fails
                   get a L2 ACK.
10/22/01   xfg     Changed ERR to MSG_MED if there is no active ASU to process
                   the state change event
10/22/01   xfg     Added codes to handle the case of State Change Ind in 
                   function find_transaction_for_command
09/14/01   xfg     Included rrcsend.h instead of rrcasn1util.h and changed
                   RRCSEND_L2ACK_REQUIRED to RRCSEND_L2ACK_NOT_REQUIRED for
                   ASU failure message
09/06/01   ram     Changed UE_MODE_ACKNOWLEDGED_DATA_PLANE to UE_MODE_
                   ACKNOWLEDGED_DATA.
08/31/01   ram     Completed functions send_asu_complete_message and send_asu
                   _failure_msg. Added comments.
08/14/01   ram     Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "err.h"
#include "msg.h"
#include "memheap.h"
#include "rrcasn1.h"
#include "rrcasu.h"
#include "rrcsend.h"
#include "rrccmd_v.h"
#include "rrcdata_v.h"
#include "rrcintif.h"
#include "rrclcm.h"
#include "rrcllc.h"
#include "rrcscmgr.h"
#include "rrcccm.h"
#include "rrcdata_v.h"
#include "rrccu.h"
#include "rrcmcm.h"
#include "rrccsp.h"
#include "rrclogging.h"

#include "rrcsibproc.h"
#include "rrcllcoc.h"
#include "rrcmeasi.h"
#include "rrcasn1util.h"
#include "rrcsmc.h"
#ifdef FEATURE_DUAL_SIM
extern boolean DS_Immediate_preempt_waiting_for_init_cell_sel;
#endif
#ifdef FEATURE_QSH_EVENT_METRIC
extern rrc_qsh_ul_failure_ota_param_s failure_ota;
extern rrc_qsh_metric_db_type rrc_qsh_metric_db[WRRC_QSH_METRIC_MAX+1];
extern boolean rrc_qsh_dl_committed;
#endif

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

#define rrcasu_substates_e_type_value0 RRCASU_INITIAL
#define rrcasu_substates_e_type_value1 RRCASU_WAIT_FOR_CHAN_CFG_CNF
#define rrcasu_substates_e_type_value2 RRCASU_MAX_SUBSTATES

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
/* Maximum number of transactions accepted for the ASU message             */
#define RRCASU_MAX_ASU_TRANSACTIONS 4
/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* Structure to store internal data related one ASU transaction            */
typedef struct
{
  /* Store the current substate    */
  rrcasu_substates_e_type curr_substate;  
  /* Store the error_cause. The error_cause should be one of the choices 
  defined in the structure rrc_FailureCauseWithProtErr in rrcasn1 
  definitions                                                              */

  boolean error_occured;
  /* Indicate if an error occured for this transaction                     */

  uint32 error_cause;                     
  /* Store the protocol error cause. This value is used only if error_cause
  == rrc_FailureCauseWithProtErr_protocolError_chosen */

  rrc_ProtocolErrorCause prot_error_cause;
  /* The protocol error cause, if any. */

}rrcasu_transaction_data_type;

/* Structure to store all internal data related to the ASU procedure       */
typedef struct
{
  rrcasu_transaction_data_type tr[RRCASU_MAX_ASU_TRANSACTIONS];
  /* Boolean to indicate if a general error occured while processing a ASU
  message. Only used if the transaction id can't be found for that message */

  boolean gen_error_occured;
  /* General error_cause. This is used when the transaction id is not known 
  The error_cause should be one of the choices 
  defined in the structure rrc_FailureCauseWithProtErr in rrcasn1 
  definitions                                                              */

  uint32 gen_error_cause;                     
  /* Store the protocol error cause. This value is used only if error_cause
  == rrc_FailureCauseWithProtErr_protocolError_chosen */

  rrc_ProtocolErrorCause gen_prot_error_cause;
  /* The protocol error cause, if any. */

}rrcasu_int_data_type;

/* Declare an item of the above type                                       */
rrcasu_int_data_type rrcasu;

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*===========================================================================

FUNCTION   SEND_ASU_FAILURE_MSG

DESCRIPTION

  This function sends the Active Set Update Failure message
  on Uplink DCCH. 
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/     
static void send_asu_failure_message
(  
  uint32 trindex  /* Transaction index for this failure message.
                  Set to RRCASU_MAX_ASU_TRANSACTIONS if the msg's
                  transaction id can't be found. */
)
{
  rrc_UL_DCCH_Message *ptr;     /* Pointer to uplink DCCH message type */
  rlc_lc_id_type ul_lc_id;      /* Logical Channel Id for the message  */ 
  rrcsend_status_e_type status; /* Status from send chain module       */

  /* Allocate memory for this message. Note that this buffer is released
  by the SEND CHAIN */
  ptr = (rrc_UL_DCCH_Message *)rrc_malloc( 
                                          sizeof(rrc_UL_DCCH_Message));

  ptr->message.t = T_rrc_UL_DCCH_MessageType_activeSetUpdateFailure;

  /* Set Bit-mask to 0 to indicate non-critical extensions are not
  present */
  RRC_RESET_MSG_IE_PRESENT(ptr->message.u.activeSetUpdateFailure);

  /* Check if this was a general failure or a failure for a 
  particular transaction */
  if( rrcasu.gen_error_occured == TRUE )
  {
    rrcasu.gen_error_occured = FALSE; /* Reset to default */
    /* General error  - we don't know the transaction id.
    Use zero as the transaction id. */
    ptr->message.u.activeSetUpdateFailure.rrc_TransactionIdentifier = 0;

    /* Fill in the general error cause */
    ptr->message.u.activeSetUpdateFailure.failureCause.t =
      rrcasu.gen_error_cause;

    /* If the general error cause is protocol error, then set
    the protocol error cause also. */
    if( rrcasu.gen_error_cause == 
        T_rrc_FailureCauseWithProtErr_protocolError )
    {
      ptr->message.u.activeSetUpdateFailure.failureCause.u.protocolError =
        rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation);
      
      /* Fill in the protocol error t */
      ptr->message.u.activeSetUpdateFailure.failureCause.u.protocolError->
        diagnosticsType.t = T_rrc_ProtocolErrorInformation_diagnosticsType_type1;

      ptr->message.u.activeSetUpdateFailure.failureCause.u.protocolError->
        diagnosticsType.u.type1 = rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation_type1);

      /* And the actual protocol error that occured */
      ptr->message.u.activeSetUpdateFailure.failureCause.u.protocolError->
        diagnosticsType.u.type1->protocolErrorCause = rrcasu.gen_prot_error_cause;

    }
  }
  else if((trindex < RRCASU_MAX_ASU_TRANSACTIONS) && (rrcasu.tr[trindex].error_occured == TRUE))
  {
    /* The error occured for a particular transaction. Use
    the passed in transaction. */
    rrcasu.tr[trindex].error_occured = FALSE; /* Reset to default */

    /* Use the given transaction index as the id. */
    ptr->message.u.activeSetUpdateFailure.rrc_TransactionIdentifier 
      = trindex;

    /* Fill in the general error cause */
    ptr->message.u.activeSetUpdateFailure.failureCause.t =
      rrcasu.tr[trindex].error_cause;

    /* If the general error cause is protocol error, then set
    the protocol error cause also. */
    if( rrcasu.tr[trindex].error_cause == 
        T_rrc_FailureCauseWithProtErr_protocolError )
    {
      ptr->message.u.activeSetUpdateFailure.failureCause.u.protocolError =
	  	  rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation);
      
      /* Fill in the protocol error t */
      ptr->message.u.activeSetUpdateFailure.failureCause.u.protocolError->
      diagnosticsType.t = T_rrc_ProtocolErrorInformation_diagnosticsType_type1;
	  
      ptr->message.u.activeSetUpdateFailure.failureCause.u.protocolError->
		    diagnosticsType.u.type1 = rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation_type1);

      /* And the actual protocol error that occured */
      ptr->message.u.activeSetUpdateFailure.failureCause.u.protocolError->
        diagnosticsType.u.type1->protocolErrorCause = rrcasu.tr[trindex].prot_error_cause;

    }
  }
  else
  {
    WRRC_MSG0_ERROR("Unknown error cause");
    //Not sure what to do here - either return or send
    //an ASUF with some default error cause. But this
    //should not happen.
    return;
  }
  /*log the protocol error*/
  rrc_log_protocol_error(   (uint8) (rrc_get_state()),
                            (uint8) RRC_PROCEDURE_ASU,
                            (uint8) ptr->message.u.activeSetUpdateFailure.failureCause.t,
                            (uint8) ((ptr->message.u.activeSetUpdateFailure.failureCause.t == T_rrc_FailureCauseWithProtErr_protocolError) 
                            ? ptr->message.u.activeSetUpdateFailure.failureCause.u.protocolError->diagnosticsType.u.type1->protocolErrorCause 
                            : RRCLOG_PROTOCOL_ERROR_NA)
                          );


  /* Get the RLC logical channel id for this msg from Logical Channel Manager */
  ul_lc_id = rrclcm_check_ul_rlc_lc_id ( UE_LOGCHAN_DCCH,
                                         DCCH_AM_RADIO_BEARER_ID,
                                         UE_MODE_ACKNOWLEDGED_DATA);
#ifdef FEATURE_QSH_EVENT_METRIC

  if(rrc_qsh_dl_committed == FALSE)
  {
    failure_ota.ul_ota_type = RRC_OTA_ASU;
    failure_ota.msg_id = WRRC_UL_DCCH_MESSAGETYPE_ACTIVESETUPDATEFAILURE;
    if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status())
    {
      failure_ota.ul_fail_ota.ul_asu_info.ul_integrity_info_present = 1;
    }
    failure_ota.ul_fail_ota.ul_asu_info.failure_cause = ptr->message.u.activeSetUpdateFailure.failureCause.t;
    failure_ota.ul_fail_ota.ul_asu_info.trans_id = trindex;
    /* Commit data should happen after dl SDU is committed */
  }
  else
  {
    if(rrc_qsh_metric_db[WRRC_QSH_METRIC_MEAS_ASU_OTA].data_ptr != NULL)
    {
      if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status())
      {
        ((wrrc_qsh_metric_meas_asu_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_MEAS_ASU_OTA].data_ptr)->asu_meas_info.ul_asu_info.ul_integrity_info_present = 1;
      }
      ((wrrc_qsh_metric_meas_asu_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_MEAS_ASU_OTA].data_ptr)->asu_meas_info.ul_asu_info.failure_cause = ptr->message.u.activeSetUpdateFailure.failureCause.t;   
      ((wrrc_qsh_metric_meas_asu_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_MEAS_ASU_OTA].data_ptr)->asu_meas_info.ul_asu_info.trans_id = trindex;   
      /* Commit data will be called in send ul sdu */
    }
  }
#endif
  /* If the logical channel id is not invalid, send the message to the
  send chain module */
  if(ul_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    WRRC_MSG0_HIGH("Sending ASU Failure message");
    status = rrcsend_send_ul_sdu(RRC_PROCEDURE_ASU,  /* Procedure is RBE    */
                                 (void *)ptr,        /* Message pointer     */
                                 ul_lc_id,           /* RLC id              */
                                 RRCSEND_L2ACK_NOT_REQUIRED  /* ACK is not required */
                                );
    if( status != RRCSEND_SUCCESS )
    {
      WRRC_MSG1_ERROR("Failed to send ASU failure msg: err %d", status);
    }
  }
  else
  {
    /* Unexpected failure. ERR for now. May need to ERR_FATAL here */
    WRRC_MSG1_ERROR("Can't send DCCH msg! invalid lc: %d", ul_lc_id ); 
  }
}/* end function send_asu_failure_message */


/*===========================================================================

FUNCTION   SEND_ASU_COMPLETE_MSG

DESCRIPTION

  This function sends the Active Set Update Complete message
  on Uplink DCCH. 
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/     
static void send_asu_complete_message
(  
  uint32 trindex  /* Transaction index for this complete message. */
)
{
  rrc_UL_DCCH_Message *ptr;     /* Pointer to uplink DCCH message type */
  rlc_lc_id_type ul_lc_id;      /* Logical Channel Id for the message  */ 
  rrcsend_status_e_type status; /* Status from send chain module       */

  /* Allocate memory for this message. Note that this buffer is released
  by the SEND CHAIN */
  ptr = (rrc_UL_DCCH_Message *)rrc_malloc( 
                                          sizeof(rrc_UL_DCCH_Message));

  ptr->message.t = T_rrc_UL_DCCH_MessageType_activeSetUpdateComplete;

  /* Set the bit-mask to zero. None of the optional
  IEs are present, and non-critical extensions are not
  present. */
 RRC_RESET_MSG_IE_PRESENT(ptr->message.u.activeSetUpdateComplete);


  /* Set the given index as the transaction id */
  ptr->message.u.activeSetUpdateComplete.rrc_TransactionIdentifier
    = trindex;

  /* Get the RLC logical channel id for this msg from Logical Channel Manager */
  ul_lc_id = rrclcm_check_ul_rlc_lc_id ( UE_LOGCHAN_DCCH,
                                         DCCH_AM_RADIO_BEARER_ID,
                                         UE_MODE_ACKNOWLEDGED_DATA);

#ifdef FEATURE_QSH_EVENT_METRIC
  /*
     uint8 count_c_activation;
     uint8 failure_cause;
     uint8 cs_start_present:1;
     uint8 ps_start_present:1;
     uint8 ul_integrity_info_present:1;
     uint8 ul_counter_sync_info:1;
  */
  if(rrc_qsh_metric_db[WRRC_QSH_METRIC_MEAS_ASU_OTA].data_ptr != NULL)
  {
    if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status())
    {
      ((wrrc_qsh_metric_meas_asu_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_MEAS_ASU_OTA].data_ptr)->asu_meas_info.ul_asu_info.ul_integrity_info_present= 1;
    }
    ((wrrc_qsh_metric_meas_asu_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_MEAS_ASU_OTA].data_ptr)->asu_meas_info.ul_asu_info.trans_id= trindex;
    /* Commit data will be called in send ul sdu */
  }
#endif

  /* If the logical channel id is not invalid, send the message to the
  send chain module */
  if(ul_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    WRRC_MSG0_HIGH("Sending ASU Complete message");
    status = rrcsend_send_ul_sdu(RRC_PROCEDURE_ASU,  /* Procedure is RBE    */
                                 (void *)ptr,        /* Message pointer     */
                                 ul_lc_id,           /* RLC id              */
                                 RRCSEND_L2ACK_NOT_REQUIRED  /* ACK is not
                                                                required */
                                );
    if( status != RRCSEND_SUCCESS )
    {
      WRRC_MSG1_ERROR("Failed to send ASU complete msg: err %d", status);
    }
  }
  else
  {
    /* Unexpected failure. ERR for now. May need to ERR_FATAL here */
    WRRC_MSG1_ERROR("Can't send DCCH msg! invalid lc: %d", ul_lc_id ); 
  }


}/* end function send_asu_complete_message */



/*===========================================================================

FUNCTION  RRCASU_CLEAR_VARS

DESCRIPTION

  This function clears all the procedure variables and puts the initial
  values wherever needed.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcasu_clear_vars( void )
{
  uint32 count;

  rrcasu.gen_error_occured = FALSE;

  for(count=0; count<RRCASU_MAX_ASU_TRANSACTIONS; count++)
  {
    /* Reset variables for each transaction */
    rrcasu.tr[count].curr_substate = RRCASU_INITIAL;
    rrcasu.tr[count].error_occured = FALSE;
  }
}



/*===========================================================================

FUNCTION   VALIDATE_ASU_MESSAGE

DESCRIPTION

  This function validates the received 
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_asu_message
(
  uint32 trindex                    /* Transaction index for this msg */
)
{

  /* Assume the message is valid to start with. */
  rrcasu.tr[trindex].error_occured = FALSE;

  /* When we get here the transaction id would have already
  been validated. Also ASN.1 failure would have already
  been checked. So we don't need to check them here. 
  
  NOTE: Most of the IEs - Activation Time, Radio link
  addition info, Radio link removal info, TX diversity
  mode, SSDT info are validated by the ordered-config
  module. So currently there is nothing to do here. */


}/* end function validate_asu_message */



/*===========================================================================

FUNCTION   RRCASU_SEND_CHANNEL_CONFIG_REQ

DESCRIPTION

  This function sends the RRC_CHANNEL_CONFIG_REQ command to LLC
  to 
        
DEPENDENCIES

  ORDERED_CONFIG must be set before calling this function.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcasu_send_channel_config_req ( void )
{

  /* Get the command buffer */
  rrc_cmd_type *cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command id  */
  cmd_ptr->cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;

  /* Fill in procedure id    */
  cmd_ptr->cmd.chan_config_req.procedure = RRC_PROCEDURE_ASU;

  /* Indicate that state change is not required,
  since we will be in CELL_DCH only */
  cmd_ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;
  
  /* Indicate that we would like to get a confirmation 
  back from LLC */
  cmd_ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = TRUE;

  /* The ASU procedure is not required to fill in any
  other parameters since the radio-links to be added
  and removed are given by ORDERED_CONFIG to LLC. Note
  that more parameters (such as transaction id) may
  be added later when we support multiple simultaneous
  ASU messages. */
  WRRC_MSG0_HIGH(" Sending CHAN_CONFIG_REQ for ASU");
  rrc_put_int_cmd( cmd_ptr );

}/* end function rrcasu_send_channel_config_req */



/*===========================================================================

FUNCTION   PROCESS_ASU_MESSAGE

DESCRIPTION

  This function processes the received ASU Message.         

DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_asu_message_r6
(
  rrc_DL_DCCH_Message *msg_ptr,    /* Pointer to the DCCH message      */ 
  uint32 trindex,                   /* Transaction index for this message   */
  rrc_ActiveSetUpdate *asu_ptr         /* Pointer to the ASU message */   
)
{
  rrcllc_oc_status_e_type oc_status;/* To hold ordered_config status          */
  rrc_ActiveSetUpdate_r6_IEs *asu_r6_ptr = 
    &asu_ptr->u.later_than_r3.criticalExtensions.u.r6.activeSetUpdate_r6;

  if (asu_ptr->u.later_than_r3.criticalExtensions.u.r6.m.activeSetUpdate_r6_add_extPresent)
  {
    WRRC_MSG0_ERROR("rrc_activeSetUpdate_r6_add_ext not supported");
  }

  if (asu_ptr->u.later_than_r3.criticalExtensions.u.r6.m.v6b0NonCriticalExtensionsPresent)
  {
    WRRC_MSG0_ERROR("r6_nonCriticalExtensions not supported");
  }
  
  validate_asu_message(trindex);
   
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(asu_r6_ptr,rrc_ActiveSetUpdate_r6_IEs,cn_InformationInfo))
  {
    MSG_LOW("CN info present in ASU msg", 0, 0, 0);
/*lint -e534 it is no need to check the returned value */
    rrc_ccm_send_nas_cn_info_r6(&(asu_r6_ptr->cn_InformationInfo));
/*lint +e534 */
  }

  if(rrcmcm_get_rrc_mode() == RRC_MODE_STANDBY)
  {
    WRRC_MSG0_HIGH("Received ASU in Standby Mode");
    oc_status = ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG;
  }
  else
  {
    /* Now set ordered config for this message. Note that the
    next state is always CELL_DCH for this procedure. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_ASU,
                                          RRC_STATE_CELL_DCH,
                                          rrc_DL_DCCH_Message_PDU,
                                          (void *)msg_ptr
                                         );
  }
  switch( oc_status )
  {
    case ORDERED_CONFIG_SET:

      /* If the SIB reading is active for getting Cell ID in DCH state,
       * and if the RL for which the PSC is requested is removed from
       * the Active set, then abort the SIB reading event.
       * Also abort if hs-repointing happens
       */
      if(rrc_sib_is_current_event_sib_read_in_dch() == TRUE)
      {
        boolean stop_gps_sib_read = FALSE;
        uint32 stored_psc = 0;
        if(
           (rrcllc_is_hs_repointing_happening() == TRUE) ||
           (FAILURE == rrcllc_get_bcch_psc(&stored_psc))
          )
        {
          stop_gps_sib_read = TRUE;
        }
        else if(RRC_MSG_COMMON_BITMASK_IE_PTR(asu_r6_ptr,rrc_ActiveSetUpdate_r6_IEs,rl_RemovalInformationList))
        {
          rrc_RL_RemovalInformationList *rrc_local_rl_RemovalInformationList;
          uint32 idx =0;
          rrc_local_rl_RemovalInformationList = &asu_r6_ptr->rl_RemovalInformationList;
          while(rrc_local_rl_RemovalInformationList->n > idx )
          {
            if(rrc_local_rl_RemovalInformationList->elem[idx].primaryScramblingCode == stored_psc)
            {
              WRRC_MSG1_HIGH(" RRCGPS: The Rl %d  for which cell id is requested is removed ",stored_psc);
              stop_gps_sib_read = TRUE;
              break;
            }
            idx++;
          }
        }

        WRRC_MSG1_HIGH("RRCGPS event stop_gps_sib_read %d due to ASU ",stop_gps_sib_read);
          
        if(stop_gps_sib_read == TRUE)
        {
          rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
        }
      }
      /* Ordered Config has been set. We can send
      the RRC_CHANNEL_CONFIG_REQ command. */
      MSG_LOW("Sending RRC_CHANNEL_CONFIG_REQ to LLC", 0, 0, 0);
      /* Call the function that creates and sends a channel
      config request to LLC. */
      rrcasu_send_channel_config_req( );

      /* Change the substate since we're waiting for the
      confirmation from LLC for the channel config */
      rrcasu.tr[trindex].curr_substate = RRCASU_WAIT_FOR_CHAN_CFG_CNF;
      break;

    case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

      WRRC_MSG0_HIGH("LLC returned simultaneous reconfig!");
      rrcasu.tr[trindex].error_occured = TRUE;
      rrcasu.tr[trindex].error_cause =
        T_rrc_FailureCauseWithProtErr_incompatibleSimultaneousReconfiguration;
      /* Send the ASU Failure Message */
      send_asu_failure_message( trindex );

      /* Go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

    case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

      WRRC_MSG0_HIGH("LLC returned unsupported config!");
      rrcasu.tr[trindex].error_occured = TRUE;
      rrcasu.tr[trindex].error_cause =
        T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      /* Send the ASU Failure Message */
      send_asu_failure_message( trindex );

      /* Go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

    case ORDERED_CONFIG_CONFIGURATION_INVALID:

      WRRC_MSG0_HIGH("LLC returned invalid config!");
      rrcasu.tr[trindex].error_occured = TRUE;
      rrcasu.tr[trindex].error_cause =
        T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      /* Send the ASU Failure Message */
      send_asu_failure_message( trindex );

      /* Go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

    case ORDERED_CONFIG_NOT_SET_OTHER:
      WRRC_MSG0_ERROR("LLC returned other failure!");
      /* When LLC sends this, there is a serious problem
      and LLC will go back to disconnected state. No need
      to send the failure message here, we can just
      go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
    case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
      WRRC_MSG0_HIGH("ASU processing deferred due to LTA!");
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      
      break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
    default:

      WRRC_MSG0_ERROR("Unknown return value from LLC");
      /* Unknown error, just go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;
  }/* end switch */

}


/*===========================================================================

FUNCTION   PROCESS_ASU_MESSAGE_R7

DESCRIPTION

  This function processes the received ASU Message.         

DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_asu_message_r7
(
  rrc_DL_DCCH_Message *msg_ptr,    /* Pointer to the DCCH message      */ 
  uint32 trindex,                   /* Transaction index for this message   */
  rrc_ActiveSetUpdate *asu_ptr         /* Pointer to the ASU message */   
)
{
  rrcllc_oc_status_e_type oc_status;/* To hold ordered_config status          */
  rrc_ActiveSetUpdate_r7_IEs *asu_r7_ptr = 
    &asu_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r7.activeSetUpdate_r7;

  if (asu_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r7.m.
  	activeSetUpdate_r7_add_extPresent)
  {
    WRRC_MSG0_ERROR("rrc_activeSetUpdate_r7_add_ext not supported");
  }

  if ((asu_ptr->u.later_than_r3.criticalExtensions.u.
  	criticalExtensions.u.r7.m.v780NonCriticalExtensionsPresent)
     && (asu_ptr->u.later_than_r3.criticalExtensions.u.
     criticalExtensions.u.r7.v780NonCriticalExtensions.m.v7f0NonCriticalExtensionsPresent))
  {
    WRRC_MSG0_ERROR("rrc_later_than_r3_criticalExtensions_criticalExtensions_r7_v780NonCriticalExtensions_nonCriticalExtensions_present not supported");
  }
  
  validate_asu_message(trindex);
   
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(asu_r7_ptr,rrc_ActiveSetUpdate_r7_IEs,cn_InformationInfo))
  {
    MSG_LOW("CN info present in ASU msg", 0, 0, 0);
/*lint -e534 it is no need to check the returned value */
    rrc_ccm_send_nas_cn_info_r6(&(asu_r7_ptr->cn_InformationInfo));
/*lint +e534 */
  }

  if(rrcmcm_get_rrc_mode() == RRC_MODE_STANDBY)
  {
    WRRC_MSG0_HIGH("Received ASU in Standby Mode");
    oc_status = ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG;
  }
  else
  {
    /* Now set ordered config for this message. Note that the
    next state is always CELL_DCH for this procedure. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_ASU,
                                          RRC_STATE_CELL_DCH,
                                          rrc_DL_DCCH_Message_PDU,
                                          (void *)msg_ptr
                                         );
  }
  switch( oc_status )
  {
    case ORDERED_CONFIG_SET:
      
      /* If the SIB reading is active for getting Cell ID in DCH state,
       * and if the RL for which the PSC is requested is removed from
       * the Active set, then abort the SIB reading event.
       */
      if(rrc_sib_is_current_event_sib_read_in_dch() == TRUE)
      {
        boolean stop_gps_sib_read = FALSE;
        uint32 stored_psc = 0;
        if(
           (rrcllc_is_hs_repointing_happening() == TRUE) ||
           (FAILURE == rrcllc_get_bcch_psc(&stored_psc))
          )
        {
          stop_gps_sib_read = TRUE;
        }
        else if(RRC_MSG_COMMON_BITMASK_IE_PTR(asu_r7_ptr,rrc_ActiveSetUpdate_r7_IEs,rl_RemovalInformationList))
        {
          uint32 idx =0;
          rrc_RL_RemovalInformationList *rrc_local_rl_RemovalInformationList;
          rrc_local_rl_RemovalInformationList = &asu_r7_ptr->rl_RemovalInformationList;
          while(rrc_local_rl_RemovalInformationList->n > idx )
          {
            if(rrc_local_rl_RemovalInformationList->elem[idx].primaryScramblingCode == stored_psc)
            {
              WRRC_MSG1_HIGH(" RRCGPS: The Rl %d  for which cell id is requested is removed ",stored_psc);
              stop_gps_sib_read = TRUE;
              break;
            }
            idx++;
          }
        }

        WRRC_MSG1_HIGH("RRCGPS event stop_gps_sib_read %d due to ASU ",stop_gps_sib_read);
        
        if(stop_gps_sib_read == TRUE)
        {
          rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
        }
      }
      /* Ordered Config has been set. We can send
      the RRC_CHANNEL_CONFIG_REQ command. */
      MSG_LOW("Sending RRC_CHANNEL_CONFIG_REQ to LLC", 0, 0, 0);
      /* Call the function that creates and sends a channel
      config request to LLC. */
      rrcasu_send_channel_config_req( );

      /* Change the substate since we're waiting for the
      confirmation from LLC for the channel config */
      rrcasu.tr[trindex].curr_substate = RRCASU_WAIT_FOR_CHAN_CFG_CNF;
      break;

    case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

      WRRC_MSG0_HIGH("LLC returned simultaneous reconfig!");
      rrcasu.tr[trindex].error_occured = TRUE;
      rrcasu.tr[trindex].error_cause =
        T_rrc_FailureCauseWithProtErr_incompatibleSimultaneousReconfiguration;
      /* Send the ASU Failure Message */
      send_asu_failure_message( trindex );

      /* Go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

    case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

      WRRC_MSG0_HIGH("LLC returned unsupported config!");
      rrcasu.tr[trindex].error_occured = TRUE;
      rrcasu.tr[trindex].error_cause =
        T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      /* Send the ASU Failure Message */
      send_asu_failure_message( trindex );

      /* Go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

    case ORDERED_CONFIG_CONFIGURATION_INVALID:

      WRRC_MSG0_HIGH("LLC returned invalid config!");
      rrcasu.tr[trindex].error_occured = TRUE;
      rrcasu.tr[trindex].error_cause =
        T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      /* Send the ASU Failure Message */
      send_asu_failure_message( trindex );

      /* Go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

    case ORDERED_CONFIG_NOT_SET_OTHER:
      WRRC_MSG0_ERROR("LLC returned other failure!");
      /* When LLC sends this, there is a serious problem
      and LLC will go back to disconnected state. No need
      to send the failure message here, we can just
      go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
    case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
      WRRC_MSG0_HIGH("ASU processing deferred due to LTA!");
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      
      break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
    default:

      WRRC_MSG0_ERROR("Unknown return value from LLC");
      /* Unknown error, just go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;
  }/* end switch */

}


/*===========================================================================
FUNCTION   process_asu_message_r8

DESCRIPTION
  This function processes the received ASU Message.         

DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void process_asu_message_r8
(
  rrc_DL_DCCH_Message *msg_ptr,    /* Pointer to the DCCH message      */ 
  uint32 trindex,                   /* Transaction index for this message   */
  rrc_ActiveSetUpdate *asu_ptr         /* Pointer to the ASU message */   
)
{
  rrcllc_oc_status_e_type oc_status;/* To hold ordered_config status          */
  rrc_ActiveSetUpdate_r8_IEs *asu_r8_ptr = 
    &asu_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.activeSetUpdate_r8;

  if (asu_ptr->u.later_than_r3.criticalExtensions.u.
  	criticalExtensions.u.criticalExtensions.u.r8.m.activeSetUpdate_r8_add_extPresent)
  {
    WRRC_MSG0_ERROR("rrc_activeSetUpdate_r8_add_ext_present not supported");
  }

  if (asu_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.m.v890NonCriticalExtensionsPresent)
  {
    WRRC_MSG0_ERROR("rrc_later_than_r3_criticalExtensions_criticalExtensions_criticalExtensions_r8_nonCriticalExtensions_present not supported");
  }
  
  validate_asu_message(trindex);
   
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(asu_r8_ptr,rrc_ActiveSetUpdate_r8_IEs,
  	cn_InformationInfo))
  {
    MSG_LOW("CN info present in ASU msg", 0, 0, 0);
/*lint -e534 it is no need to check the returned value */
    rrc_ccm_send_nas_cn_info_r6(&(asu_r8_ptr->cn_InformationInfo));
/*lint +e534 */
  }

  if(rrcmcm_get_rrc_mode() == RRC_MODE_STANDBY)
  {
    WRRC_MSG0_HIGH("Received ASU in Standby Mode");
    oc_status = ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG;
  }
  else
  {
    /* Now set ordered config for this message. Note that the
    next state is always CELL_DCH for this procedure. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_ASU,
                                          RRC_STATE_CELL_DCH,
                                          rrc_DL_DCCH_Message_PDU,
                                          (void *)msg_ptr
                                         );
  }
  switch( oc_status )
  {
    case ORDERED_CONFIG_SET:

      /* If the SIB reading is active for getting Cell ID in DCH state,
       * and if the RL for which the PSC is requested is removed from
       * the Active set, then abort the SIB reading event.
       */
      if(rrc_sib_is_current_event_sib_read_in_dch() == TRUE)
      {
        boolean stop_gps_sib_read = FALSE;
        uint32 stored_psc = 0;
        if(
           (rrcllc_is_hs_repointing_happening() == TRUE) ||
           (FAILURE == rrcllc_get_bcch_psc(&stored_psc))
          )
        {
          stop_gps_sib_read = TRUE;
        }
        else if(RRC_MSG_COMMON_BITMASK_IE_PTR(asu_r8_ptr,rrc_ActiveSetUpdate_r8_IEs,rl_RemovalInformationList))
        {
          rrc_RL_RemovalInformationList * rrc_local_rl_RemovalInformationList;
          uint32 idx =0;
          rrc_local_rl_RemovalInformationList = &asu_r8_ptr->rl_RemovalInformationList;
          while(rrc_local_rl_RemovalInformationList->n > idx )
          {
            if(rrc_local_rl_RemovalInformationList->elem[idx].primaryScramblingCode == stored_psc)
            {
              WRRC_MSG1_HIGH(" RRCGPS: The Rl %d  for which cell id is requested is removed ",stored_psc);
              stop_gps_sib_read = TRUE;
              break;
            }
            idx++;
          }
        }

        WRRC_MSG1_HIGH("RRCGPS event stop_gps_sib_read %d due to ASU ",stop_gps_sib_read);
        
        if(stop_gps_sib_read == TRUE)
        {
          rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
        }
      }
      /* Ordered Config has been set. We can send
      the RRC_CHANNEL_CONFIG_REQ command. */
      MSG_LOW("Sending RRC_CHANNEL_CONFIG_REQ to LLC", 0, 0, 0);
      /* Call the function that creates and sends a channel
      config request to LLC. */
      rrcasu_send_channel_config_req( );

      /* Change the substate since we're waiting for the
      confirmation from LLC for the channel config */
      rrcasu.tr[trindex].curr_substate = RRCASU_WAIT_FOR_CHAN_CFG_CNF;
      break;

    case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

      WRRC_MSG0_HIGH("LLC returned simultaneous reconfig!");
      rrcasu.tr[trindex].error_occured = TRUE;
      rrcasu.tr[trindex].error_cause =
        T_rrc_FailureCauseWithProtErr_incompatibleSimultaneousReconfiguration;
      /* Send the ASU Failure Message */
      send_asu_failure_message( trindex );

      /* Go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

    case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

      WRRC_MSG0_HIGH("LLC returned unsupported config!");
      rrcasu.tr[trindex].error_occured = TRUE;
      rrcasu.tr[trindex].error_cause =
        T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      /* Send the ASU Failure Message */
      send_asu_failure_message( trindex );

      /* Go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

    case ORDERED_CONFIG_CONFIGURATION_INVALID:

      WRRC_MSG0_HIGH("LLC returned invalid config!");
      rrcasu.tr[trindex].error_occured = TRUE;
      rrcasu.tr[trindex].error_cause =
        T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      /* Send the ASU Failure Message */
      send_asu_failure_message( trindex );

      /* Go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

    case ORDERED_CONFIG_NOT_SET_OTHER:
      WRRC_MSG0_ERROR("LLC returned other failure!");
      /* When LLC sends this, there is a serious problem
      and LLC will go back to disconnected state. No need
      to send the failure message here, we can just
      go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME      
    case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
      WRRC_MSG0_HIGH("ASU processing deferred due to LTA!");
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
        
      break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
    default:

      WRRC_MSG0_ERROR("Unknown return value from LLC");
      /* Unknown error, just go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;
  }/* end switch */

}

#ifdef FEATURE_WCDMA_REL9
/*===========================================================================
FUNCTION   process_asu_message_r9

DESCRIPTION
  This function processes the received ASU Message.         

DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void process_asu_message_r9
(
  rrc_DL_DCCH_Message *msg_ptr,    /* Pointer to the DCCH message      */ 
  uint32 trindex,                   /* Transaction index for this message   */
  rrc_ActiveSetUpdate *asu_ptr         /* Pointer to the ASU message */   
)
{
  rrcllc_oc_status_e_type oc_status;/* To hold ordered_config status          */
  rrc_ActiveSetUpdate_r9_IEs *asu_r9_ptr = 
    &asu_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.activeSetUpdate_r9;

  if (asu_ptr->u.later_than_r3.criticalExtensions.u.
    criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.m.activeSetUpdate_r9_add_extPresent)
  {
    WRRC_MSG0_ERROR("rrc_activeSetUpdate_r9_add_ext_present not supported");
  }

  if (asu_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.m.v950NonCriticalExtensionsPresent)
  {
    WRRC_MSG0_ERROR("rrc_later_than_r3_criticalExtensions_criticalExtensions_criticalExtensions_later_r8_criticalExtensions_r9_nonCriticalExtensions_present not supported");
  }
  
  validate_asu_message(trindex);
   
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(asu_r9_ptr,rrc_ActiveSetUpdate_r9_IEs,
    cn_InformationInfo))
  {
    MSG_LOW("CN info present in ASU msg", 0, 0, 0);
/*lint -e534 it is no need to check the returned value */
    rrc_ccm_send_nas_cn_info_r6(&(asu_r9_ptr->cn_InformationInfo));
/*lint +e534 */
  }

  if(rrcmcm_get_rrc_mode() == RRC_MODE_STANDBY)
  {
    WRRC_MSG0_HIGH("Received ASU in Standby Mode");
    oc_status = ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG;
  }
  else
  {
    /* Now set ordered config for this message. Note that the
    next state is always CELL_DCH for this procedure. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_ASU,
                                          RRC_STATE_CELL_DCH,
                                          rrc_DL_DCCH_Message_PDU,
                                          (void *)msg_ptr
                                         );
  }
  switch( oc_status )
  {
    case ORDERED_CONFIG_SET:
      /* If the SIB reading is active for getting Cell ID in DCH state,
       * and if the RL for which the PSC is requested is removed from
       * the Active set, then abort the SIB reading event.
       */
      if(rrc_sib_is_current_event_sib_read_in_dch() == TRUE)
      {
        boolean stop_gps_sib_read = FALSE;
        uint32 stored_psc = 0;
        if(
           (rrcllc_is_hs_repointing_happening() == TRUE) ||
           (FAILURE == rrcllc_get_bcch_psc(&stored_psc))
          )
        {
          stop_gps_sib_read = TRUE;
        }
        else if(RRC_MSG_COMMON_BITMASK_IE_PTR(asu_r9_ptr,rrc_ActiveSetUpdate_r9_IEs,rl_RemovalInformationList))
        {
          rrc_RL_RemovalInformationList * rrc_local_rl_RemovalInformationList;
          uint32 idx =0;
          rrc_local_rl_RemovalInformationList = &asu_r9_ptr->rl_RemovalInformationList;
          while(rrc_local_rl_RemovalInformationList->n > idx )
          {
            if(rrc_local_rl_RemovalInformationList->elem[idx].primaryScramblingCode == stored_psc)
            {
              WRRC_MSG1_HIGH(" RRCGPS: The Rl %d  for which cell id is requested is removed ",stored_psc);
              stop_gps_sib_read = TRUE;
              break;
            }
            idx++;
          }
        }

        WRRC_MSG1_HIGH("RRCGPS event stop_gps_sib_read %d due to ASU ",stop_gps_sib_read);
        
        if(stop_gps_sib_read == TRUE)
        {
          rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
        }
      }
      /* Ordered Config has been set. We can send
      the RRC_CHANNEL_CONFIG_REQ command. */
      MSG_LOW("Sending RRC_CHANNEL_CONFIG_REQ to LLC", 0, 0, 0);
      /* Call the function that creates and sends a channel
      config request to LLC. */
      rrcasu_send_channel_config_req( );

      /* Change the substate since we're waiting for the
      confirmation from LLC for the channel config */
      rrcasu.tr[trindex].curr_substate = RRCASU_WAIT_FOR_CHAN_CFG_CNF;
      break;

    case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

      WRRC_MSG0_HIGH("LLC returned simultaneous reconfig!");
      rrcasu.tr[trindex].error_occured = TRUE;
      rrcasu.tr[trindex].error_cause =
        T_rrc_FailureCauseWithProtErr_incompatibleSimultaneousReconfiguration;
      /* Send the ASU Failure Message */
      send_asu_failure_message( trindex );

      /* Go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

    case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

      WRRC_MSG0_HIGH("LLC returned unsupported config!");
      rrcasu.tr[trindex].error_occured = TRUE;
      rrcasu.tr[trindex].error_cause =
        T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      /* Send the ASU Failure Message */
      send_asu_failure_message( trindex );

      /* Go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

    case ORDERED_CONFIG_CONFIGURATION_INVALID:

      WRRC_MSG0_HIGH("LLC returned invalid config!");
      rrcasu.tr[trindex].error_occured = TRUE;
      rrcasu.tr[trindex].error_cause =
        T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      /* Send the ASU Failure Message */
      send_asu_failure_message( trindex );

      /* Go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

    case ORDERED_CONFIG_NOT_SET_OTHER:
      WRRC_MSG0_ERROR("LLC returned other failure!");
      /* When LLC sends this, there is a serious problem
      and LLC will go back to disconnected state. No need
      to send the failure message here, we can just
      go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME      
    case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
      WRRC_MSG0_HIGH("ASU processing deferred due to LTA!");
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
    default:

      WRRC_MSG0_ERROR("Unknown return value from LLC");
      /* Unknown error, just go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;
  }/* end switch */

}
#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif /* FEATURE_WCDMA_REL10 */

/*===========================================================================

FUNCTION   rrcasu_check_and_get_primary_plmn_id

DESCRIPTION

  This function will check whether primary plmn id is present 
  in r3/r99 path of ASU message. If present then it will return 
  primary plmn id.
        
DEPENDENCIES
  None.

RETURN VALUE
TRUE : IF Primary PLMN I.D is present in ASU message in R3 path
FFALSE: Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
static boolean rrcasu_check_and_get_primary_plmn_id
(
  /* Pointer to the ASU message */
  rrc_ActiveSetUpdate *msg_ptr,
  /*This will copy primary plmn i.d if present in OTA msg*/
  rrc_PLMN_Identity *primary_plmn_Identity,
  /*This will indicate OTA msg version*/
  rrc_msg_ver_enum_type msg_version
)
{
  boolean primary_plmn_Id_present = FALSE;

  if(msg_ptr == NULL)
  {
    /*msg ptr is NULL so return from here*/
    return FALSE;
  }
  switch(msg_version)
  {
    case MSG_REL99:
      /*Check to see if primary PLMN I.D is present in URA update comfirmation CCCH 
       *message ext URA update comfirmation-v690ext which is present in 
       * v690NonCriticalExtensions in r99 path
       */
      if((msg_ptr->u.r3.m.laterNonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.laterNonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.laterNonCriticalExtensions.v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.
          v690NonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
           v590NonCriticalExtensions.v690NonCriticalExtensions.activeSetUpdate_v690ext.m.
           primary_plmn_IdentityPresent)
        )
      {
        WRRC_MSG0_HIGH("NWS:ASU msg contains primary_plmn_Id in r3 path");
        primary_plmn_Id_present = TRUE;
        *primary_plmn_Identity = msg_ptr->u.r3.laterNonCriticalExtensions.v4b0NonCriticalExtensions.
           v590NonCriticalExtensions.v690NonCriticalExtensions.activeSetUpdate_v690ext.primary_plmn_Identity;
      }
      break;
   
    default:
      WRRC_MSG1_ERROR("NWS: Msg version = %d, Not supported",msg_version);
      break;
  }
  
  return primary_plmn_Id_present;
}


/*===========================================================================

FUNCTION   PROCESS_ASU_MESSAGE

DESCRIPTION

  This function processes the received ASU Message.         

DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_asu_message
(
  rrc_DL_DCCH_Message *msg_ptr,    /* Pointer to the DCCH message      */ 
  uint32 trindex                   /* Transaction index for this message   */
)
{
  rrc_ActiveSetUpdate *ptr;         /* Pointer to the ASU message */ 
  rrcllc_oc_status_e_type oc_status;/* To hold ordered_config status          */
  rrc_proc_e_type proc_id = RRC_PROCEDURE_NONE;
  rrcllc_oc_process_state_e_type process_state;
   /*Boolean used to indicate primary plmn i.d is present in OTA msg*/
  boolean primary_plmn_Id_present = FALSE;
   /*Variable to hold primary plmn i.d if present in OTA msg*/
  rrc_PLMN_Identity primary_plmn_Identity;
  
  (void)rrcllc_get_ordered_config_state_and_proc(&proc_id,&process_state);
  WRRC_MSG0_HIGH("Received ASU message");

  ptr = &(msg_ptr->message.u.activeSetUpdate);

  /* Get current stste to make sure it is in CELL_DCH */
  if ( rrc_get_state() != RRC_STATE_CELL_DCH )
  {
    WRRC_MSG0_HIGH("ASU is received at a wrong state");
    rrcasu.tr[trindex].error_occured = TRUE;
    rrcasu.tr[trindex].error_cause = 
      T_rrc_FailureCauseWithProtErr_protocolError;
    rrcasu.tr[trindex].prot_error_cause =
      rrc_ProtocolErrorCause_messageNotCompatibleWithReceiverState;
    send_asu_failure_message( trindex );
    return;
  }

  /* For the first release only one ASU message can be
  processed. So check if ordered config is set. In the long
  run we will support upto 4 different ASU messages in 
  parallel so this code should be removed later */

  if( rrcllc_get_ordered_config_status_wo_f3() != OC_NOT_SET)
  {
    WRRC_MSG0_HIGH("ORDERED_CONFIG is Set! rejecting ASU");
    rrcasu.tr[trindex].error_occured = TRUE;
    rrcasu.tr[trindex].error_cause = 
      T_rrc_FailureCauseWithProtErr_incompatibleSimultaneousReconfiguration;
    send_asu_failure_message( trindex );
    return;
  }

  if (T_rrc_ActiveSetUpdate_later_than_r3 == ptr->t)
  {
    if (T_rrc_ActiveSetUpdate_criticalExtensions_3_r6 == 
       ptr->u.later_than_r3.criticalExtensions.t)
    {
      process_asu_message_r6(msg_ptr, trindex, ptr);
    }
    else

    if (T_rrc_ActiveSetUpdate_criticalExtensions_2_r7 == 
       ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.t)
    {
      process_asu_message_r7(msg_ptr, trindex, ptr);
    }
    else

    if (T_rrc_ActiveSetUpdate_criticalExtensions_1_r8 == 
       ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t)
    {
      process_asu_message_r8(msg_ptr, trindex, ptr);
    }
    else
#ifdef FEATURE_WCDMA_REL9
    if (T_rrc_ActiveSetUpdate_later_r8_criticalExtensions_r9 == 
       ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.t)
    {
      process_asu_message_r9(msg_ptr, trindex, ptr);
    }
    else
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
    #error code not present
#endif /* FEATURE_WCDMA_REL10 */

    {
      WRRC_MSG0_ERROR("RRCEUL: later_than_r3_criticalExtensions_criticalExtensions not supported, UE should have rejected the message earlier");
    }
    return;
  }

  validate_asu_message(trindex);
   
  if( rrcasu.tr[trindex].error_occured == TRUE )
  {
    send_asu_failure_message( trindex );
    return;
  }

  MSG_LOW("Processing ASU message", 0, 0, 0);

  if( RRC_MSG_COMMON_BITMASK_IE(ptr->u.r3.activeSetUpdate_r3,
       rrc_ActiveSetUpdate_r3_IEs,cn_InformationInfo))
  {

    /* Send CN Information to NAS */
      /*Check to see if primary PLMN I.D is present in ASU message ext
       * rrc_ActiveSetUpdate-v690ext which is present in 
       * v690NonCriticalExtensions
       */
       primary_plmn_Id_present = rrcasu_check_and_get_primary_plmn_id(ptr,
                                                                      &primary_plmn_Identity,
                                                                      MSG_REL99);
/*lint -e534 it is no need to check the returned value */
    rrc_ccm_send_nas_cn_info(
                     &(ptr->u.r3.activeSetUpdate_r3.cn_InformationInfo)
                     ,primary_plmn_Id_present
                     ,&primary_plmn_Identity
      );
/*lint +e534 */
  }

  rrc_msg_failure_cause = RRC_MSG_VALID;

  /* Now set ordered config for this message. Note that the
  next state is always CELL_DCH for this procedure. */

  if(rrcmcm_get_rrc_mode() == RRC_MODE_STANDBY)
  {
    WRRC_MSG0_HIGH("Received ASU in Standby Mode");
    oc_status = ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG;
  }
  else
  {
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_ASU,
                                           RRC_STATE_CELL_DCH,
                                           rrc_DL_DCCH_Message_PDU,
                                           (void *)msg_ptr
                                         );
  }
  if(rrc_msg_failure_cause == RRC_MSG_INVALID_CONFIG)
  { 
    rrc_msg_failure_cause = RRC_MSG_VALID;
    oc_status = ORDERED_CONFIG_CONFIGURATION_INVALID;
  }

  switch( oc_status )
  {
    case ORDERED_CONFIG_SET:
      
      /* If the SIB reading is active for getting Cell ID in DCH state,
       * and if the RL for which the PSC is requested is removed from
       * the Active set, then abort the SIB reading event.
       */
      /*Abort cell ID in DCH if hs-repointing happens*/
      if(rrc_sib_is_current_event_sib_read_in_dch() == TRUE)
      {
        boolean stop_gps_sib_read = FALSE;
        uint32 stored_psc = 0;
        if(
           (rrcllc_is_hs_repointing_happening() == TRUE) ||
           (FAILURE == rrcllc_get_bcch_psc(&stored_psc))
          )
        {
          stop_gps_sib_read = TRUE;
        }
        else if(RRC_MSG_COMMON_BITMASK_IE(ptr->u.r3.activeSetUpdate_r3,rrc_ActiveSetUpdate_r3_IEs,rl_RemovalInformationList))
        {
          rrc_RL_RemovalInformationList *rrc_local_rl_RemovalInformationList;
          uint32 idx =0;
          rrc_local_rl_RemovalInformationList = &ptr->u.r3.activeSetUpdate_r3.rl_RemovalInformationList;
          while(rrc_local_rl_RemovalInformationList->n > idx )
          {
            if(rrc_local_rl_RemovalInformationList->elem[idx].primaryScramblingCode == stored_psc)
            {
              WRRC_MSG1_HIGH(" RRCGPS: The Rl %d  for which cell id is requested is removed ",stored_psc);
              stop_gps_sib_read = TRUE;
              break;
            }
            idx++;
          }
        }

        WRRC_MSG1_HIGH("RRCGPS event stop_gps_sib_read %d due to ASU ",stop_gps_sib_read);
        
        if(stop_gps_sib_read == TRUE)
        {
          rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
        }
      }

      /* Ordered Config has been set. We can send
      the RRC_CHANNEL_CONFIG_REQ command. */
      MSG_LOW("Sending RRC_CHANNEL_CONFIG_REQ to LLC", 0, 0, 0);
      /* Call the function that creates and sends a channel
      config request to LLC. */
      rrcasu_send_channel_config_req( );

      /* Change the substate since we're waiting for the
      confirmation from LLC for the channel config */
      rrcasu.tr[trindex].curr_substate = RRCASU_WAIT_FOR_CHAN_CFG_CNF;
      break;

    case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

      WRRC_MSG0_HIGH("LLC returned simultaneous reconfig!");
      rrcasu.tr[trindex].error_occured = TRUE;
      rrcasu.tr[trindex].error_cause =
        T_rrc_FailureCauseWithProtErr_incompatibleSimultaneousReconfiguration;
      /* Send the ASU Failure Message */
      send_asu_failure_message( trindex );

      /* Go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

    case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

      WRRC_MSG0_HIGH("LLC returned unsupported config!");
      rrcasu.tr[trindex].error_occured = TRUE;
      rrcasu.tr[trindex].error_cause =
        T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      /* Send the ASU Failure Message */
      send_asu_failure_message( trindex );

      /* Go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

    case ORDERED_CONFIG_CONFIGURATION_INVALID:

      WRRC_MSG0_HIGH("LLC returned invalid config!");
      rrcasu.tr[trindex].error_occured = TRUE;
      rrcasu.tr[trindex].error_cause =
        T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      /* Send the ASU Failure Message */
      send_asu_failure_message( trindex );

      /* Go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

    case ORDERED_CONFIG_NOT_SET_OTHER:
      WRRC_MSG0_ERROR("LLC returned other failure!");
      /* When LLC sends this, there is a serious problem
      and LLC will go back to disconnected state. No need
      to send the failure message here, we can just
      go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME    
    case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
      WRRC_MSG0_HIGH("ASU processing deferred due to LTA!");
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
    default:

      WRRC_MSG0_ERROR("Unknown return value from LLC");
      /* Unknown error, just go back to initial substate */
      rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
      break;
  }/* end switch */


}/* end function process_asu_message */




/*===========================================================================

FUNCTION   RRCASU_PROCESS_CHANNEL_CONFIG_CNF

DESCRIPTION

  This function processes the received Channel Config Confirmation
  from LLC. 
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcasu_process_channel_config_cnf
(
  rrc_channel_config_cnf_type *ptr,  /* Pointer to the Channel cfg cnf cmd */
  uint32 trindex                     /* Transaction index for this cmd     */
)
{

  if (RRCCU_STARTED == rrccu_get_cell_update_started_status())
  {
    WRRC_MSG0_HIGH("Cell Update is Active.  Ignoring Chan config cnf");
    rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
    return;
  }

  if( ptr->chan_cfg_status == RRCLLC_CHAN_CFG_SUCCESS )
  {
    /*Notify the Cell_Id to registered entities*/
    rrc_notify_cell_id_in_dch();  
    /* ASU succeeded. Nothing else to do here except
    send the ASU Complete Message */
    send_asu_complete_message( trindex );

  }
  else /* Different failure status' should be handled here */
  {
    WRRC_MSG0_ERROR("Channel config failed for ASU");
    if (ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_OTHER) 
    {
      rrcasu.tr[trindex].error_occured = TRUE;
      rrcasu.tr[trindex].error_cause =  
        T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      send_asu_failure_message( trindex );
    }
    else
    {
      rrcasu.tr[trindex].error_occured = TRUE;
      rrcasu.tr[trindex].error_cause =  
        T_rrc_FailureCauseWithProtErr_physicalChannelFailure;
      send_asu_failure_message( trindex );
    }
  }

  /* The procedure ends */
  rrcasu.tr[trindex].curr_substate = RRCASU_INITIAL;
  /* We can't clear ordered config if there is currently a reconfiguration
     in progress.  The reconfiguration processing will take care of it. */
  if(!rrcllc_reconfig_in_progress())
  {
    if((ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_INCOMPATIBLE_STATE)
        ||(ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_OTHER))
    {
      /* Reset ORDERED_CONFIG - UE can now accept other ordered
      configurations */
      (void)rrcllc_reset_ordered_config(RRC_PROCEDURE_ASU,TRUE);
    }
    else
    {
      rrcllc_clear_ordered_config();
    }
  }
  else
  {
    WRRC_MSG0_HIGH("ASU during reconfig, not clearing OC");
  }
}





/*===========================================================================

FUNCTION   RRCASU_PROCESS_STATE_CHANGE_IND

DESCRIPTION

  This function processes the received State Change indication command.
  Only state changes from CELL_DCH to any other state is received by
  this procedure. The function does the necessary cleanup since this
  procedure can be active only in CELL_DCH.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcasu_process_state_change_ind ( void )
{
  /* This procedure cannot be active in any other state
  except CELL_DCH. Since we registered for any state change
  from DCH, we're not in CELL_DCH now. So clear the procedure
  variables. */
  WRRC_MSG0_HIGH("Clearing ASU procedure due to state change");
  rrcasu_clear_vars();
}





/*===========================================================================

FUNCTION   RRCASU_INITIAL_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the ASU procedure is in the RRCASU_INITIAL substate. This function
  looks at the received event and does the appropriate processing
  of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcasu_initial_substate_event_handler
(
  rrc_cmd_type *cmd_ptr, /* Received command                   */
  uint32 trindex         /* Transaction index for this command */
)
{
  rrc_DL_DCCH_Message *dcch_msg_ptr;      /* Pointer to the DCCH message */

  /* Switch on the received command. The only command expected
  in this state is RRC_DOWNLINK_SDU_IND. We will ignore all
  other commands */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_DOWNLINK_SDU_IND:
      
      /* When we get here, it's already verified that we have a decoded
      ASU message, so we can process the message now. */
      dcch_msg_ptr=(rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;
      process_asu_message( dcch_msg_ptr, trindex );
      break;

    case RRC_STATE_CHANGE_IND:

      /* Although state change is not expected, it's not abnormal
      so just print a message and ignore the command. */
      MSG_MED("Ignoring state change in ASU initial substate", 0, 0, 0);
      break;

    default:

      /* No other events are normal in this
      substate */
      WRRC_MSG2_HIGH("Ignoring ASU event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, rrcasu.tr[trindex].curr_substate);
      break;
  }
}



/*===========================================================================

FUNCTION   RRCASU_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the ASU procedure is in the RRCASU_WAIT_FOR_CHAN_CFG_CNF substate.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcasu_wfchcnf_substate_event_handler
(
  rrc_cmd_type *cmd_ptr, /* Received command                   */
  uint32 trindex         /* Transaction index for this command */
)
{
  /* Switch on the received command. */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_CHANNEL_CONFIG_CNF:
      
      /* Process the channel config confirm command */
      MSG_MED("Processing CHANNEL_CONFIG_CNF cmd", 0, 0, 0);
      rrcasu_process_channel_config_cnf
        ( &(cmd_ptr->cmd.chan_config_cnf), trindex ); 
      break;

    case RRC_DOWNLINK_SDU_IND:
      
      /* In this substate, this event is always for a duplicate
      ASU message. (I.e. message with the same transaction id as
      the previous one). Hence this can be ignored. */
      WRRC_MSG0_HIGH("Ignoring duplicate ASU message");
      break;

    case RRC_STATE_CHANGE_IND:

      /* State changed from CELL_DCH to some other state. */
      rrcasu_process_state_change_ind( );
      break;      

    default:

      /* No other events are normal in this
      substate */
      WRRC_MSG2_HIGH("Ignoring ASU event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, rrcasu.tr[trindex].curr_substate);
      break;
  }
}





/*===========================================================================

FUNCTION FIND_TRANSACTION_FOR_COMMAND


DESCRIPTION

  This function looks at the received command and determines which
  transaction this command belongs to. 
  
DEPENDENCIES

  None

RETURN VALUE

  A transaction id from 0 to 3 if a valid transaction id is found.
  Else returns RRCASU_MAX_ASU_TRANSACTIONS.

SIDE EFFECTS

  None
  
===========================================================================*/
static uint32 find_transaction_for_command
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  rrc_DL_DCCH_Message *dcch_msg_ptr;      /* Pointer to the DCCH message */
  uint32 index=RRCASU_MAX_ASU_TRANSACTIONS;      /* Index to be returned */
  uint32 count;                           /* Local counter               */

  /* Switch on the command type */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_DOWNLINK_SDU_IND:
      /* Check the ASU message to make sure an valid message is received */
      dcch_msg_ptr=(rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;
      if (dcch_msg_ptr != NULL) 
      {
        /* An valid ASU message has been received. Look into the 
        message's transaction id. First make sure it is a ASU message. */
        if( dcch_msg_ptr->message.t != T_rrc_DL_DCCH_MessageType_activeSetUpdate )
        {
          WRRC_MSG0_ERROR("Invalid DCCH message given to ASU proc");
          rrcasu.gen_error_occured = TRUE;
          rrcasu.gen_error_cause = T_rrc_FailureCauseWithProtErr_protocolError;
          rrcasu.gen_prot_error_cause = rrc_ProtocolErrorCause_messageTypeNonexistent;
        } 
        /* And make sure that only valid critical extensions are included */
        else if(T_rrc_ActiveSetUpdate_later_than_r3 ==
                 dcch_msg_ptr->message.u.activeSetUpdate.t)
        {
          if (T_rrc_ActiveSetUpdate_criticalExtensions_3_r6 == 
             dcch_msg_ptr->message.u.activeSetUpdate.u.later_than_r3.criticalExtensions.t)
          {
            index = dcch_msg_ptr->message.u.activeSetUpdate.u.
               later_than_r3.rrc_TransactionIdentifier;

            WRRC_MSG1_HIGH("RRCEUL: Rel-6 ASU received, transaction id: %d", index);

            if( index >= RRCASU_MAX_ASU_TRANSACTIONS )
            {
              /* Transaction id must be between 0 and 3 */
              WRRC_MSG0_ERROR("Invalid transaction id in ASU msg");
              rrcasu.gen_error_occured = TRUE;
              rrcasu.gen_error_cause = T_rrc_FailureCauseWithProtErr_protocolError;
              rrcasu.gen_prot_error_cause = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
              index = RRCASU_MAX_ASU_TRANSACTIONS;
            }
          }
          else
          if (T_rrc_ActiveSetUpdate_criticalExtensions_2_r7 == 
             dcch_msg_ptr->message.u.activeSetUpdate.u.later_than_r3.criticalExtensions.u.criticalExtensions.t)
          {
            index = dcch_msg_ptr->message.u.activeSetUpdate.u.
               later_than_r3.rrc_TransactionIdentifier;

            WRRC_MSG1_HIGH("RRCEUL: Rel-7 ASU received, transaction id: %d", index);

            if( index >= RRCASU_MAX_ASU_TRANSACTIONS )
            {
              /* Transaction id must be between 0 and 3 */
              WRRC_MSG0_ERROR("Invalid transaction id in ASU msg");
              rrcasu.gen_error_occured = TRUE;
              rrcasu.gen_error_cause = T_rrc_FailureCauseWithProtErr_protocolError;
              rrcasu.gen_prot_error_cause = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
              index = RRCASU_MAX_ASU_TRANSACTIONS;
            }
          }
          else
          if (T_rrc_ActiveSetUpdate_criticalExtensions_1_r8 == 
             dcch_msg_ptr->message.u.activeSetUpdate.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t)
          {
            index = dcch_msg_ptr->message.u.activeSetUpdate.u.
               later_than_r3.rrc_TransactionIdentifier;

            WRRC_MSG1_HIGH("RRCEUL: Rel-8 ASU received, transaction id: %d", index);

            if( index >= RRCASU_MAX_ASU_TRANSACTIONS )
            {
              /* Transaction id must be between 0 and 3 */
              WRRC_MSG0_ERROR("Invalid transaction id in ASU msg");
              rrcasu.gen_error_occured = TRUE;
              rrcasu.gen_error_cause = T_rrc_FailureCauseWithProtErr_protocolError;
              rrcasu.gen_prot_error_cause = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
              index = RRCASU_MAX_ASU_TRANSACTIONS;
            }
          }
          else
#ifdef FEATURE_WCDMA_REL9
          if (T_rrc_ActiveSetUpdate_later_r8_criticalExtensions_r9 == 
             dcch_msg_ptr->message.u.activeSetUpdate.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
             criticalExtensions.u.later_r8_criticalExtensions.t)
          {
            index = dcch_msg_ptr->message.u.activeSetUpdate.u.
               later_than_r3.rrc_TransactionIdentifier;
  
            WRRC_MSG1_HIGH("RRCEUL: Rel-9 ASU received, transaction id: %d", index);
  
            if( index >= RRCASU_MAX_ASU_TRANSACTIONS )
            {
              /* Transaction id must be between 0 and 3 */
              WRRC_MSG0_ERROR("Invalid transaction id in ASU msg");
              rrcasu.gen_error_occured = TRUE;
              rrcasu.gen_error_cause = T_rrc_FailureCauseWithProtErr_protocolError;
              rrcasu.gen_prot_error_cause = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
              index = RRCASU_MAX_ASU_TRANSACTIONS;
            }
          }
          else
#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
          #error code not present
#endif /* FEATURE_WCDMA_REL10 */

          {
            WRRC_MSG0_ERROR("ASU message contains critical extensions!");
            rrcasu.gen_error_occured = TRUE;
            rrcasu.gen_error_cause = T_rrc_FailureCauseWithProtErr_protocolError;
            rrcasu.gen_prot_error_cause = rrc_ProtocolErrorCause_messageExtensionNotComprehended;
          }
        }
        else
        {
          index = dcch_msg_ptr->message.u.activeSetUpdate.u.r3.
                  activeSetUpdate_r3.rrc_TransactionIdentifier;
          if( index >= RRCASU_MAX_ASU_TRANSACTIONS )
          {
            /* Transaction id must be between 0 and 3 */
            WRRC_MSG0_ERROR("Invalid transaction id in ASU msg");
            rrcasu.gen_error_occured = TRUE;
            rrcasu.gen_error_cause = T_rrc_FailureCauseWithProtErr_protocolError;
            rrcasu.gen_prot_error_cause = rrc_ProtocolErrorCause_ie_ValueNotComprehended;
            index = RRCASU_MAX_ASU_TRANSACTIONS;
          }
        }
      } /* end of valid ASU */
      else
      {
        WRRC_MSG0_ERROR("Invalid ASU is received");
        rrcasu.gen_error_occured = TRUE;
        rrcasu.gen_error_cause = T_rrc_FailureCauseWithProtErr_protocolError;
        rrcasu.gen_prot_error_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
      } /* end of invalid ASU message */
      break;

    case RRC_CHANNEL_CONFIG_CNF:

      /* NOTE: Channel config confirm command will be modified
      to include transaction id. Until then, we will support only
      one ASU transaction at a time.
      
      For now, just find which transaction is waiting for
      a channel config confirm command. */
      for( count=0; count < RRCASU_MAX_ASU_TRANSACTIONS; count++ )
      {
        if( rrcasu.tr[count].curr_substate == RRCASU_WAIT_FOR_CHAN_CFG_CNF )
        {
          index = count; /* Found the correct transaction */
          break; /* Break out of the for loop */
        }
      }
      if( index == RRCASU_MAX_ASU_TRANSACTIONS )
      {
        /* None of the transactions are in the right state. */
        WRRC_MSG0_HIGH("Invalid CHAN_CONFIG_CNF cmd");
      }
      break;

    case RRC_STATE_CHANGE_IND:
      /* NOTE: State Change Ind will apply to all ASU transitions.
      Since ASU can be active in CELL_DCH, when RRC state is
      changed a state other than CELL_DCH, all ASU procedures
      will be cleared. So, only need to find the first active ASU 
      transition, the action will clear all the procedures */
      for( count=0; count < RRCASU_MAX_ASU_TRANSACTIONS; count++ )
      {
        if( rrcasu.tr[count].curr_substate != RRCASU_INITIAL )
        {
          index = count; /* Found the correct transaction */
          break; /* Break out of the for loop */
        }
      }
      break;
    default:
      break;
  }/* end switch */

  return( index );
}


/*===========================================================================

FUNCTION RRCASU_PROCEDURE_EVENT_HANDLER


DESCRIPTION

  This is the main function handling external events received by the
  Active Set Update procedure. This function is called by the
  dispatcher whenever an event is received for this module.  
  
DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
  
===========================================================================*/
void rrcasu_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  uint32 index; /* To store transaction id as index */

  if((rrccu_get_cell_update_started_status() == RRCCU_STARTED) 
#ifdef FEATURE_DUAL_SIM
      || (DS_Immediate_preempt_waiting_for_init_cell_sel == TRUE)
#endif
    )
  {
    WRRC_MSG1_HIGH("Ignoring, clearing ASU message, As CU is initiated, cmd_id= %d", cmd_ptr->cmd_hdr.cmd_id);
    rrcasu_clear_vars();
    return;
  }
  /* Since we may have multiple ASU state machines active,
  for each command we have to find the transaction (index)
  to which the command belongs. */
  index = find_transaction_for_command( cmd_ptr );
  /* If the index is valid, find the substate for that
  index and call the appropriate function to process
  the command. */
  if( index < RRCASU_MAX_ASU_TRANSACTIONS )
  {
    WRRC_MSG2_HIGH_OPT("function rrcasu_procedure_event_handler is called in rrcasu.tr[%d].curr_substate = rrcasu_substates_e_type_value%d", index, rrcasu.tr[index].curr_substate);
    rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_ASU,rrcasu.tr[index].curr_substate,cmd_ptr->cmd_hdr.cmd_id);
    switch( rrcasu.tr[index].curr_substate )
    {
      case RRCASU_INITIAL:
        /* Call the event handler for Initial substate */
        rrcasu_initial_substate_event_handler( cmd_ptr, index );
        break;

      case RRCASU_WAIT_FOR_CHAN_CFG_CNF:
        /* Call the event handler for waiting for chan cfg cnf substate */
        rrcasu_wfchcnf_substate_event_handler( cmd_ptr, index );
        break;
      
      case RRCASU_MAX_SUBSTATES: 
      default:
        /* Invalid substate - should never happen. Clear
        all procedure vars */
        WRRC_MSG1_ERROR("Invalid ASU Substate %d", rrcasu.tr[index].curr_substate);
        rrcasu_clear_vars();
        break;
    }/* end switch */
  }
  else
  {
    /* Switch on the command type */
    switch ( cmd_ptr->cmd_hdr.cmd_id )
    {
      case RRC_DOWNLINK_SDU_IND:
        /* Could not find the index for this command. If the command
        is a "RRC_DOWNLINK_SDU_IND" for a ASU message, we were not
        able to find the transaction id, so we will send a ASU failure
        message. */
        send_asu_failure_message( index );
        break;

      case RRC_STATE_CHANGE_IND:
        /* Currently there is no active ASU to handle this event */
        MSG_MED("NO Active ASU for State Change Ind", 0, 0, 0);
        break;

      default:
        /* For any other command we should have been able to find
        the right index. Declare an error and reset the procedure
        variables. */
        WRRC_MSG0_ERROR("Invalid transaction for ASU command");
        rrcasu_clear_vars();
    } /* end of switch for command type */
  }
}





/*===========================================================================

FUNCTION  RRCASU_INIT_PROCEDURE

DESCRIPTION

  This function initializes the ASU procedure.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcasu_init_procedure( void )
{
  rrcasu_clear_vars(); /* Clear all the procedure variables */

  /* Register for state change  - When state changes from DCH
  to any other state we want to be notified since this procedure
  is active only in the DCH state. */
  rrcscmgr_register_for_scn( RRC_PROCEDURE_ASU,  /* Procedure name */
                             RRC_STATE_CELL_DCH, /* From State     */
                             RRC_STATE_WILDCARD  /* To State       */
                           );
}
/*===========================================================================

FUNCTION  rrcasu_return_curr_substate

DESCRIPTION
returns curr substate    
DEPENDENCIES

  None.
 
RETURN VALUE
  rrcasu_substates_e_type


SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif

rrcasu_substates_e_type rrcasu_return_curr_substate(void)
{
  return rrcasu.tr[0].curr_substate;
}