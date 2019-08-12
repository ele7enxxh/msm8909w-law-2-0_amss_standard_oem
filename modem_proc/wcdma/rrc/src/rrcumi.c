/*===========================================================================
                  R R C  UTRAN MOBILITY INFO

DESCRIPTION
  This header file implements functions for UTRAN Mobility Info
  procedure.  For the current release ciphering and integrity
  protection is not used and is ignored.  Transaction id of
  the UTRAN Mobility Info is copied and sent in Confirm msg.

  The foll. fields of the UTRAN MOBILITY INFO is ignored.
  * cipheringModeInfo
  * dl_CounterSynchronisationInfo
  * integrityProtectionModeInfo
    (in UMI message will be supported when SRNS relocation is
     supported.  integrityProtectionModeInfo can be received in 
     UMI only when SRNS relocation is done. The same applies to
     cipheringModeInfo in the above bullet)

  The foll. fields of the UTRAN Mobility Confirm message is 
  sent uninitialized
  * ul_IntegProtActivationInfo
    (in UMI message will be supported when SRNS relocation is
     supported.  integrityProtectionModeInfo can be received in 
     UMI only when SRNS relocation is done)
  * ul_CounterSynchronisationInfo
  * ul_CounterSynchronisationInfo



EXTERNALIZED FUNCTIONS

  rrcumi_procedure_event_handler
    This function is the event handler for all events that are sent to the
    UTRAN Mobility Info procedure.

  rrcumi_init_procedure
    This function initializes required parameters for UTRAN Mobility Info
    procedure.

  rrcumi_process_utran_mobility_info_command  
   This function processes the UTRAN MOBILITY INFO received on DL DCCH
   and extracts the Transaction ID.  Sends the SYNC_IND cmd to MM and
   also calls the function to send UTRAN Mobility Info

  rrcumi_send_utran_mobility_info_confirm
   This function sends the UTRAN Mobility Info command

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2002-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcumi.c_v   1.4   16 May 2002 16:49:10   hdowlat  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcumi.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/16   ad      Made changes to log important parms from OTAs
05/09/16   sp      Made changes for Mav debug snapshot
09/09/15   sas     Made changes to update LAC,PLMN info to registered clients after UMI.
04/02/15   vg      Made changes to remove wrong and inconsistent rrc_free causing double free to happen.
04/27/15   sp      Made changes to clear UMI procedure if IP check fails for UMI
04/23/15   vi      Made changes to add DMCR IE only when DMCR is supported in SIB3
11/10/14   sp      Made changes to save procedure substates in crash debug info
09/17/14   sg      Made changes to ignore UMI if preemption is in progress
09/15/14   sp      Made changes to trigger UMI failure if UMI is received when delay CU flag is set.
08/04/14   sp      Made changes to fix compilation errors
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
12/09/13   sp      Made changes for OSYS 6.6.5 migration
12/03/13   mn      Moved all declarations to top of the code.
12/03/13   vg      Made changes to implement a new diag event EVENT_WCDMA_RRC_FAILURE
06/12/13   ad      Made changes to configure from OC when cell update confirm takes to 
                   PCH state and OC is set.This will remove the use of 
                   RRCLLC_CHAN_CFG_EPCH_HRNTI_ONLY cause.
03/22/13   mp      Removed tmc.h include file
01/03/13   ad      Made changes to reduce the redundant F3s
10/26/12   db      EFACH Redesign changes
09/27/12   ad      umi.new_hrnti_valid flag is not being reset after UMI procedure 
                   is ended. This is leading to incorrect channel config request
                   being sent in subsequent UMI procedures even though no new hrnti 
                   is provided.Made changes to use same flag for hrnti and ernti.                  

09/28/12   db      Made changes to incorporate the review comments of CRs 377231,389499
09/27/12   pm      Made the changes to extract the HRNTI and ERNTI from REL7 UMI
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/06/12   ad      Changes for HS-RACH feature
12/09/11   ad      Added code to log WCDMA_RRC_PROTOCOL_ERRORS packet
05/03/11   rl      Fixed the lint error
02/07/11   rl      Corrected the condition check for getting the 
                   dedicated prio in UMI
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
07/15/10   rm      Made changes to correctly populate dedicated priorities from OTA 
07/14/10   su      Fixed Lint errors.
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
04/29/10   ss      Corrected typo EATURE_SRNS_RELOCATION -> FEATURE SRNS RELOCATION
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
03/12/10   dm      Added code for supporting Deferred SIB readimg feature.
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
09/11/09   ps      Made changes for reducing the internal RRC command size 
                   under FEATURE_RRC_REDUCE_CMD_SIZE
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
03/28/09   rm      CMI Featurisation
03/05/09   kp      Added security related SRNS relocation support for R7 DCCH message 
                   UMI based on ASN.1-7.9 version. 
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
10/23/07   kp      Added support for network sharing feature. Code changes to handle 
                   primary PLMN I.D in OTA message. Changes are under feature flag 
                   FEATURE NETWORK SHARING.
09/05/06   vm      The function call rrc_transition_to_disconnected_state() now takes an
                   additional argument.
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE REL5 while retaining the HSDPA specifc processing 
                   under FEATURE HSDPA.
04/25/06   da      Added checking the status of RNTI_UPDATE_CNF.  If failed, 
                   send failure to UMI msg and clear procedure.
02/02/06   sgk     Corrected code check if new_urnti_valid and update the new 
                   urnti info. 
01/12/05   vr      Fix for sending correct CN info to NAS
12/05/05   vr      Take UE to idle if L2 ACK for UMICnf(for SRNS) fails 
11/10/05   vr      Changes for switching between HEDGE and WEDGE using NV Item
                   All Rel 5 changes are now under the nv check.
04/08/05   tkk     Added Rel-5 ASN.1 critical extensions processing for UMI message.
07/04/05   ss      Added support to update connected mode sync timers to L1 on 
                   reception of UMI.
04/06/05   vr      Include rrclogging.h unconditionally for logging on all platforms
03/11/05   vk      Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE HSDPA
03/09/05   kc      Added support to update connected mode timers and constants when
                   they are received in v3a0 extensions
01/19/04   kc      Removed incorrect CDLL_DCH restriction check for UMI trig SRNS
10/11/04   sgk     Removed RRCUMI_MSG_VALID from enum rrcumi_msg_status_e_type 
                   as it was not used to fix lint error.
08/30/04   sgk     Typecast umi.new_crnti to uint 16 in function
                   rrcumi_process_rnti_info to fix lint error Loss of precision
                   (assignment) (32 bits to 16 bits).
                   Typecast params of rrc_log_protocol_error to uint8 in function
                   rrcumi_process_utran_mobility_info_command to fix lint error
                   Converting enum to int.
                   Removed break after return in function
                   rrcumi_process_utran_mobility_info_command to fix lint error
                   'unreachable'.
05/04/04   sk      Changed rrcrb_send_rnti_update_req() input parms.
04/22/04   kc      Defensive code to take UMI to INITIAL substate in event_handler
                   itself, if STATE_CHANGE_IND to idle is received
04/13/04   sk      Use the error_status and protocol error cause that are set in 
                   the validation when rrcumi_send_umi_failure() is called after
                   UMI validation is failed.
02/24/03   kc      Fix for Critical Lint Errors.
01/21/03   kc      Support IP-Mode-Info processing even in the case when UMI
                   itself is not used for SRNS relocation, but a reconfig proc
                   is used and later UMI is used to change security info.
11/11/03   kc      Added UMI support for SRNS relocation.
10/08/03   kc      Added UMI support in DCH state too.
09/24/03   vn      Fixed a typo that was causing mem corruption.
08/21/03   sk      Added a function to build and send UMI failure message
                   with appropriate cause.
                   Modified the code to send a UMI failure message instead of 
                   status message when there is a protocol error.
                   Send a UMI failure message with cause "configuration unsupported"
                   when there is unsupported IE in the UMI message.
                   Send UMI failure if "downlink counter sync info" is present in the 
                   UMI message.
08/04/03   vn      Initialize new fields for RLC re-establishment in RNTI_UPDATE_REQ.
02/03/03   kc      Removed lint errors
01/15/03   kc      Updated comments in header regarding support for integrity
                   protectionModeInfo and CipheringModeInfo in UMI message 
                   only in case of SRNS relocation 
10/24/02   rj      Updated to use rrc_malloc and rrc_free to use dual heaps 
                   in place of mem_malloc and mem_free to avoid memory
                   fragmentation
05/17/02   kc      Process STATE_CHANGE_IND in RRC Disconnected state.
05/17/02   kc      Process STATE_CHANGE_IND in RRCUMI_INITIAL substate.
03/06/02   kc      Added more changes for UTRAN Mob Info handling CN_INFO_FULL
                   message type. Added procedure field in UPDATE_REQ msg.
03/04/02   kc      Added changes for UTRAN Mob Info Confirm message and for
                   RNTI Update Request and Cnf messages
10/17/01   kc      Initial Check In for RRC UMI file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "rrccmd_v.h"              
#include "rrcccm.h"             
#include "rrcumi.h"
#include "msg.h"
#include "err.h"
#include "rrcasn1.h"
#include "rrclcm.h"
#include "rrcdata_v.h"
#include "rrcsend.h"
#include "mmtask.h"
#ifdef FEATURE_CMI
#include "mmtask_v.h"
#endif
#include "rrcscmgr.h"
#include "rrclogging.h"
#include "rrcsmc.h"
#include "rrcrbcommon.h"
#include "rrcmisc.h"
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
#include "rrcsibproc.h"
#endif

#include "rrcdormancy.h"

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
#include "rrcmeas.h"
#endif
#include "rrcasn1util.h"
#ifdef FEATURE_WCDMA_HS_RACH
#include "rrcllcoc.h"
#endif
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* Enumerated type of status of validating UMI message                   */
typedef enum
{
  RRCUMI_MSG_UNSUPPORTED_CONFIG, /* Message has un-supported configuration  */
  RRCUMI_MSG_PROTOCOL_ERROR,     /* Message has protocol error              */
  RRCUMI_MSG_INVALID_CONFIG      /* Message has invalid configuration       */
}rrcumi_msg_status_e_type;


/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
/* This variable holds substate for UE Capability Information Procedure
 */

/* Structure to store all internal data related to UMI procedure           */

typedef struct {
  rrcumi_msg_status_e_type failure_status; /* Failure cause                 */
  rrc_ProtocolErrorCause  prot_err_cause; /* Protocol Error cause          */
}rrcumi_msg_failure_cause_type;

typedef struct {
  rrcumi_substate_e_type curr_substate; /* Store the current substate     */
  rrcumi_msg_failure_cause_type status;   /* Status of message validation   */

  boolean    new_crnti_valid; /*Indicates if a new crnti is recvd */           
  rrc_int_c_rnti_type     new_crnti;
  rrc_int_u_rnti_type          new_urnti;
  boolean                      new_urnti_valid;          /* Indicates if a valid U-RNTI was recd */ 
  boolean                      umi_for_srns_relocation;  /* Indicates SRNS reloc involved with 
                                                          this msg                             */
  uint16  new_hrnti;
  boolean    new_hrnti_valid;          /* Indicates if a valid H-RNTI was recd */ 
}rrcumi_int_data_type;

rrcumi_int_data_type umi;

LOCAL rrcumi_substate_e_type                     rrcumi_substate;
LOCAL rrcumi_ue_timer_const_rcvd_status_e_type   ue_timer_const_rcvd_status;

/*Local variable for confirm msg*/
LOCAL rrc_utran_mob_info_cnf_type                utran_mob_inf_cnf;

#ifdef FEATURE_QSH_EVENT_METRIC
extern rrc_qsh_ul_failure_ota_param_s failure_ota;
extern rrc_qsh_metric_db_type rrc_qsh_metric_db[WRRC_QSH_METRIC_MAX+1];
extern boolean rrc_qsh_dl_committed;
#endif
/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                            Function Implementation
** ======================================================================= */
/*===========================================================================

FUNCTION rrcumi_check_ue_timer_constants_update_status

DESCRIPTION
  Update global rrc_ue_conn_timers_and_constants with timers and constants
  from SIB1.  Function provided for SIB procedure to query if we should
  indeed update connected mode timers and constants database.
  
DEPENDENCIES
  None

RETURN VALUE
  RRC_CONN_TIMER_AND_CONSTANTS_UPDATE_REQ
  :  If SIB Procedure needs to update rrc_ue_conn_timers_and_constants
  RRC_CONN_TIMER_AND_CONSTANTS_UPDATE_NOT_REQ
  : If SIB Procedure need not update rrc_ue_conn_timers_and_constants  
  
SIDE EFFECTS
  None
===========================================================================*/
/*lint -e715*/
rrc_conn_timer_constants_update_status_e_type 
rrcumi_check_ue_timer_constants_update_status
(
  boolean new_cell_ind
)
{

  /*if we have already received conn. mode timers and consts in a 
   *directed message, then do not read from SIBS ever
   *in UTRAN Mobility Info msg, then do not update timers and consts
   */
  if(ue_timer_const_rcvd_status==RRCUMI_UE_TIMER_CONST_RCVD)
  {
    return(RRC_CONN_TIMER_AND_CONSTANTS_UPDATE_NOT_REQ);
  }
  else
  {
    return(RRC_CONN_TIMER_AND_CONSTANTS_UPDATE_REQ);
  }

}/* rrcumi_update_sib1_ue_conn_timers_and_constants */
/*lint +e715*/

/*===========================================================================

FUNCTION rrcumi_send_umi_failure

DESCRIPTION
  This function builds and sends a UMI failure message with appropriate cause
  
DEPENDENCIES
  None

RETURN VALUE
  void
  
SIDE EFFECTS
  None
===========================================================================*/
static void  rrcumi_send_umi_failure
(  
rrc_RRC_TransactionIdentifier  transaction_id, /*transaction id of the message */
rrcumi_msg_status_e_type failure_status,  /* Failure Cause - should be a subset
                                        of the failure causes specified by
                                        the spec. */
rrc_ProtocolErrorCause  prot_err_cause  /* Protocol Error cause. This is checked
                                        only if the Failure cause is set to 
                                        "Protocol Error". */
)
{

  rrc_UL_DCCH_Message *ptr;     /* Pointer to uplink DCCH message type */
  rlc_lc_id_type ul_lc_id; /* Logical Channel Id for the message  */
  rrcsend_status_e_type status; /* Status from send chain module       */

  /* Allocate memory for this message. Note that this buffer is released
  by the SEND CHAIN */
  ptr = (rrc_UL_DCCH_Message *)rrc_malloc( 
                                          sizeof(rrc_UL_DCCH_Message));

  RRC_RESET_MSG_IE_PRESENT_PTR(ptr); /* Integrity Check info not present */
  ptr->message.t = T_rrc_UL_DCCH_MessageType_utranMobilityInformationFailure; /* UMI Failure msg */

  /* mask non critical extensions */
  RRC_RESET_MSG_IE_PRESENT(ptr->message.u.utranMobilityInformationFailure);


  ptr->message.u.utranMobilityInformationFailure.rrc_TransactionIdentifier =
   transaction_id;
    /* We no longer need to keep this id. Make rejected_transaction id
    invalid again */

  switch (failure_status)
  {
  case RRCUMI_MSG_UNSUPPORTED_CONFIG:
    ptr->message.u.utranMobilityInformationFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_configurationUnsupported;
    break;

  case RRCUMI_MSG_PROTOCOL_ERROR:
    ptr->message.u.utranMobilityInformationFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_protocolError;
    
     ptr->message.u.utranMobilityInformationFailure.failureCause.u.protocolError =
        rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation);
     

    /* When there is a Protocol Error, we also have to indicate
    the protocol error cause */
    ptr->message.u.utranMobilityInformationFailure.failureCause.u.protocolError->
      diagnosticsType.t = T_rrc_ProtocolErrorInformation_diagnosticsType_type1;

    ptr->message.u.utranMobilityInformationFailure.failureCause.u.protocolError->
      diagnosticsType.u.type1 = rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation_type1);
    

    /* Copy the given protocol error cause */
    ptr->message.u.utranMobilityInformationFailure.failureCause.u.protocolError->
      diagnosticsType.u.type1->protocolErrorCause = prot_err_cause;
    break;

  case RRCUMI_MSG_INVALID_CONFIG:
    ptr->message.u.utranMobilityInformationFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_invalidConfiguration;
    break;


  default:
    ptr->message.u.utranMobilityInformationFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_configurationUnsupported;
    break;
  }

#ifdef FEATURE_QSH_EVENT_METRIC
    /*uint8 count_c_activation;
      uint8 failure_cause;
      uint8 cs_start_present:1;
      uint8 ps_start_present:1;
      uint8 ul_integrity_info_present:1;
      uint8 ul_counter_sync_info:1;
    */
    /* PHY channel failure means already a DL OTA is logged */
    if(rrc_qsh_dl_committed == FALSE)
    {
      failure_ota.ul_ota_type = RRC_OTA_UL_FAILURE;
      failure_ota.msg_id = WRRC_UL_DCCH_MESSAGETYPE_UTRANMOBILITYINFORMATIONFAILURE;
      if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status())
      {
        failure_ota.ul_fail_ota.ul_ota_parms.ul_integrity_info_present = 1;
      }
      failure_ota.ul_fail_ota.ul_ota_parms.failure_cause = ptr->message.u.utranMobilityInformationFailure.failureCause.t;
     /* Commit data should happen after dl SDU is committed */
    }
    else if(rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr != NULL)
    {
      if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status())
      {
       ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.ul_ota_parms.ul_integrity_info_present= 1;
      }
      ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.ul_ota_parms.failure_cause= ptr->message.u.utranMobilityInformationFailure.failureCause.t;   
      /* Commit data will be called in send ul sdu */
    }
#endif
      /*log the protocol error*/
    rrc_log_protocol_error( (uint8) (rrc_get_state()),
                            (uint8) RRC_PROCEDURE_UMI,
                            (uint8) ptr->message.u.utranMobilityInformationFailure.failureCause.t,
                            (uint8) prot_err_cause
                          );

  /* Get the RLC logical channel id for this msg from Logical Channel Manager ??*/
  ul_lc_id = rrclcm_check_ul_rlc_lc_id ( UE_LOGCHAN_DCCH,
                                         DCCH_AM_RADIO_BEARER_ID,
                                         UE_MODE_ACKNOWLEDGED_DATA);

  /* If the logical channel id is not invalid, send the message to the
  send chain module */
  if (ul_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    WRRC_MSG1_HIGH("Sending UMI Failure with cause %d", 
             ptr->message.u.utranMobilityInformationFailure.failureCause.t);
    status = rrcsend_send_ul_sdu(RRC_PROCEDURE_UMI,  /* Procedure is UMI    */
                                 (void *)ptr,        /* Message pointer     */
                                 ul_lc_id,           /* RLC id              */
                                 RRCSEND_L2ACK_NOT_REQUIRED  /* ACK is required */
                                );
    if ( status != RRCSEND_SUCCESS )
    {
      WRRC_MSG1_ERROR("Send chain failed for UMI failure msg: error cause %d", status);
      /* ptr is already freed within function rrcsend_send_ul_sdu so it must not be freed again here. 
         Otherwise it causes double free causing exception*/
    }
  }
  else
  {
    /* Unexpected failure. ERR for now. May need to ERR_FATAL here */
    WRRC_MSG1_ERROR("Can't send UL DCCH msg! Invalid lc id: %d", ul_lc_id );
    /* It's important to free the memory since we didn't send the
    message. */
    rrc_free( ptr );
  }
}/* end function rrcumi_send_umi_failure */

/*===========================================================================

FUNCTION  RRCUMI_CLEAR_PROCEDURE

DESCRIPTION

  This function clears all the internal variables in
  the UMI procedure and brings them to their default
  values.
    
DEPENDENCIES

  The Substate of the procedure should be changed
  to initial substate before calling this function.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcumi_clear_procedure( void )
{
  /* Clear the C-RNTI valid flag */
  umi.new_crnti_valid = FALSE;
  umi.new_urnti_valid = FALSE;
  umi.new_hrnti_valid = FALSE;
  umi.umi_for_srns_relocation = FALSE;
  WRRC_MSG0_HIGH("Clearing UMI internal database");
}
/*===========================================================================

FUNCTION rrcumi_validate_utran_mobility_info_command

DESCRIPTION
  This function validates that UTRAN MOBILITY INFO does not contain 
  Critical extensions.
  
DEPENDENCIES
  None

RETURN VALUE
  rrcumi_status_e_type : It returns RRCUMI_FAILURE is Critical extensions
  are present.  Else RRCUMI_SUCCESS is returned.
  
SIDE EFFECTS
  None
===========================================================================*/
static rrcumi_status_e_type rrcumi_validate_utran_mobility_info_command
(
  rrc_DL_DCCH_Message *msg_ptr              /* Pointer to Downlink DCCH Msg */
)
{
  rrcumi_status_e_type status = RRCUMI_FAILURE; 
  boolean              pdcp_info_list_present = FALSE;
  boolean              dl_counter_sync_info_present = FALSE;

  rrc_UTRANMobilityInformation *umi_msg_ptr;

  /* Get the pointer to the actual UMI message */
  umi_msg_ptr = &(msg_ptr->message.u.utranMobilityInformation);

  /* Check if critical extensions are present */
  if(msg_ptr->message.u.utranMobilityInformation.t == 
              T_rrc_UTRANMobilityInformation_later_than_r3)
  {
    if((msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.criticalExtensions.t ==
        T_rrc_UTRANMobilityInformation_criticalExtensions_2_r5) &&
       (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5))
    {
      WRRC_MSG0_HIGH("R5 Critical extensions was choosen");

      if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.
         criticalExtensions.u.r5.utranMobilityInformation_r5,
         rrc_UTRANMobilityInformation_r5_IEs,dl_CounterSynchronisationInfo))
      {
        dl_counter_sync_info_present = TRUE;

        if(RRC_MSG_COMMON_BITMASK_IE_TYPE2(msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.
           criticalExtensions.u.r5.utranMobilityInformation_r5.
           dl_CounterSynchronisationInfo,
           rb_WithPDCP_InfoList))
        {
          pdcp_info_list_present = TRUE;
        }
      }
      if((umi_msg_ptr->u.later_than_r3.criticalExtensions.u.r5.m.v690NonCriticalExtensionsPresent) &&
        (umi_msg_ptr->u.later_than_r3.criticalExtensions.u.r5.
        v690NonCriticalExtensions.m.v860NonCriticalExtentionsPresent))
      {
        WRRC_MSG0_ERROR("REL8:v860NonCriticalExtentions not supported yet");
      }
    }
    else
    if((msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.criticalExtensions.t == 
          T_rrc_UTRANMobilityInformation_criticalExtensions_2_criticalExtensions )
          && (msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.criticalExtensions.u.criticalExtensions.t == 
          T_rrc_UTRANMobilityInformation_criticalExtensions_1_r7 ) &&
       (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7))
    {
       rrc_UTRANMobilityInformation_r7_IEs * r7_umi_msg_ptr= 
        &msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r7.utranMobilityInformation_r7;
      WRRC_MSG0_HIGH("R7 Critical extensions was choosen");
     
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(r7_umi_msg_ptr,
         rrc_UTRANMobilityInformation_r7_IEs,dl_CounterSynchronisationInfo))
      {
        dl_counter_sync_info_present = TRUE;

        if(RRC_MSG_COMMON_BITMASK_IE_TYPE2(r7_umi_msg_ptr->dl_CounterSynchronisationInfo,
           rb_WithPDCP_InfoList))
        {
          pdcp_info_list_present = TRUE;
        }
      }
#ifndef FEATURE_WCDMA_ABSOLUTE_PRIORITY
      if(msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.criticalExtensions.u.
        criticalExtensions.u.r7.m.v860NonCriticalExtensionsPresent)
      {
        WRRC_MSG0_ERROR("REL8:860ext is not supported yet");
      }
#endif
    }
    else

    {
      WRRC_MSG0_ERROR("Critical extensions are not supported");
      umi.status.failure_status = RRCUMI_MSG_PROTOCOL_ERROR;
      umi.status.prot_err_cause = rrc_ProtocolErrorCause_messageExtensionNotComprehended;
      return(status); 
    } 
  } 
  else
  {
    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.utranMobilityInformation.u.r3.
       utranMobilityInformation_r3,
       rrc_UTRANMobilityInformation_r3_IEs,dl_CounterSynchronisationInfo))
    {
      dl_counter_sync_info_present = TRUE;

      if(RRC_MSG_COMMON_BITMASK_IE(umi_msg_ptr->u.r3.utranMobilityInformation_r3.
         dl_CounterSynchronisationInfo,
         rrc_DL_CounterSynchronisationInfo,rB_WithPDCP_InfoList))
      {
        pdcp_info_list_present = TRUE;
      }
    } 
    if((umi_msg_ptr->u.r3.m.v3a0NonCriticalExtensionsPresent) &&
      (umi_msg_ptr->u.r3.v3a0NonCriticalExtensions.m.laterNonCriticalExtensionsPresent) &&
      (umi_msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.m.v690NonCriticalExtensionsPresent) &&
      (umi_msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v690NonCriticalExtensions.m.
       v860NonCriticalExtentionsPresent))
    {
      WRRC_MSG0_ERROR("REL8:v860NonCriticalExtentions not supported");
    }

  }

  if (dl_counter_sync_info_present == TRUE)
  {

    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has 
    PDCP info list. If it is true then this configuration is unsupported */
    if (pdcp_info_list_present == TRUE)
    {
      WRRC_MSG0_HIGH("UMI: PDCP info in DL counter sync info not supported");
      umi.status.failure_status=RRCUMI_MSG_UNSUPPORTED_CONFIG;
      return status;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation. 
        Set the flag that indicates that this procedure
      involves SRNS relocation */
      umi.umi_for_srns_relocation = TRUE;

      status = RRCUMI_SUCCESS;
    }
  }
  else
  {
    status = RRCUMI_SUCCESS;
  }
  return(status);

}/* rrcumi_validate_utran_mobility_info_command */

/*===========================================================================

FUNCTION rrcumi_build_utran_mobility_info_confirm_msg

DESCRIPTION
  This function builds the UTRAN Mobility Info command which will be
  sent on UL DCCH

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcumi_build_utran_mobility_info_confirm_msg
(
  rrc_utran_mob_info_cnf_type *utran_mob_inf_cnf_ptr,
  rrc_UL_DCCH_Message *msg_ptr
)
{

  /* Build a UTRAN Mobility Info msg.
   * Identify this message as an UTRAN Mobility Info
   */
  msg_ptr->message.t = T_rrc_UL_DCCH_MessageType_utranMobilityInformationConfirm;

  /* Set bit_mask to zero */
  RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.utranMobilityInformationConfirm); 

  /* If SRNS relocation was triggered as a result of this message then 
  append the start list */
  if (umi.umi_for_srns_relocation == TRUE)
  {
    /*start list is included in ul_counter_synchronization_info. So enable this IE in the
    bitmask */
    RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.utranMobilityInformationConfirm,
    rrc_UTRANMobilityInformationConfirm,ul_CounterSynchronisationInfo);

    RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.utranMobilityInformationConfirm.ul_CounterSynchronisationInfo);


    /* call SMC API to append the start list */
    if (SUCCESS != rrcsmc_append_start_list(&msg_ptr->message.u.utranMobilityInformationConfirm.
                                            ul_CounterSynchronisationInfo.startList,
                                            RRCSMC_EXTRACT_AND_APPEND_START))
    {
      /*should not come here */
      WRRC_MSG0_ERROR("UMI : Could not append Start list");
    }
  }
  /* Copy the transaction ID of UTRAN MOBILITY INFO to  Msg*/
  msg_ptr->message.u.utranMobilityInformationConfirm.rrc_TransactionIdentifier = 
    utran_mob_inf_cnf_ptr->transaction_id; 

  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
  {
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
    if(rrcsib_is_dmcr_ie_needed() == TRUE)
    {
      msg_ptr->message.u.utranMobilityInformationConfirm.m.laterNonCriticalExtensionsPresent = 1;
 
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.utranMobilityInformationConfirm.laterNonCriticalExtensions);
 
      msg_ptr->message.u.utranMobilityInformationConfirm.laterNonCriticalExtensions.m.v770NonCriticalExtensionPresent = 1;
 
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.utranMobilityInformationConfirm.laterNonCriticalExtensions.v770NonCriticalExtension);
 
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.utranMobilityInformationConfirm.laterNonCriticalExtensions.v770NonCriticalExtension.
        utranMobilityInformationConfirm_v770ext);
 
      msg_ptr->message.u.utranMobilityInformationConfirm.laterNonCriticalExtensions.v770NonCriticalExtension.
        utranMobilityInformationConfirm_v770ext.m.deferredMeasurementControlReadingPresent = 1;
      
      msg_ptr->message.u.utranMobilityInformationConfirm.laterNonCriticalExtensions.v770NonCriticalExtension.
        utranMobilityInformationConfirm_v770ext.deferredMeasurementControlReading = rrc_UTRANMobilityInformationConfirm_v770ext_IEs_deferredMeasurementControlReading_true;
    }
#endif
  }

  return;

} /* rrcumi_build_utran_mobility_info_confirm */


/*===========================================================================

FUNCTION   rrcumi_validate_umi_ip_mode

DESCRIPTION

  This function checks whether IP mode info is present or not
        
DEPENDENCIES

  None.
 
RETURN VALUE

  Boolean. TRUE : IP mode info is present
           FALSE : IP mode info is not present

SIDE EFFECTS

  None.

===========================================================================*/
static boolean rrcumi_validate_umi_ip_mode
(
rrc_UTRANMobilityInformation *msg_ptr /* Pointer to the UMI message        */
)
{
  boolean  ip_mode_present = FALSE;

  /*check whether dl_counter_sync info is present or not . Do not check for
  the contents of DL_counter_sync_info. First the integrity check has tobe made
  with new integrity protection mode info in the message. Later we can check
  the dl_counter_sync_info contents and send a failure if PDCP info is present */

  if((msg_ptr->t == T_rrc_UTRANMobilityInformation_later_than_r3) &&
     (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5))
  {
    if(msg_ptr->u.later_than_r3.criticalExtensions.t == 
        T_rrc_UTRANMobilityInformation_criticalExtensions_2_r5)
    {
      if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.r5.
         utranMobilityInformation_r5,
         rrc_UTRANMobilityInformation_r5_IEs,integrityProtectionModeInfo))
      {
        ip_mode_present = TRUE;
      }
    }
  }
  else if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.utranMobilityInformation_r3,
      rrc_UTRANMobilityInformation_r3_IEs,integrityProtectionModeInfo))
  {
    ip_mode_present = TRUE;
  }

  return ip_mode_present;
} /* rrcumi_validate_umi_ip_mode */



/*===========================================================================

FUNCTION   RRCUMI_VALIDATE_SRNS_RELOCATION_INFO()

DESCRIPTION

  This function checks whether SRNS relocation info is present or not
        
DEPENDENCIES

  None.
 
RETURN VALUE

  Boolean. TRUE : SRNS relocation info is present
           FALSE : SRNS relocation info is not present

SIDE EFFECTS

  None.

===========================================================================*/
static boolean rrcumi_validate_srns_relocation_info
(
rrc_UTRANMobilityInformation *msg_ptr /* Pointer to the UMI message        */
)
{
  boolean  srns_relocation_present = FALSE;

  /*check whether dl_counter_sync info is present or not . Do not check for
  the contents of DL_counter_sync_info. First the integrity check has tobe made
  with new integrity protection mode info in the message. Later we can check
  the dl_counter_sync_info contents and send a failure if PDCP info is present */

  if((msg_ptr->t == T_rrc_UTRANMobilityInformation_later_than_r3) &&
     (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5))
  {
    if(msg_ptr->u.later_than_r3.criticalExtensions.t == 
        T_rrc_UTRANMobilityInformation_criticalExtensions_2_r5)
    {
      if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.r5.
         utranMobilityInformation_r5,
         rrc_UTRANMobilityInformation_r5_IEs,dl_CounterSynchronisationInfo))
      {
        srns_relocation_present = TRUE;
      }
    }
    else if(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.t == 
        T_rrc_UTRANMobilityInformation_criticalExtensions_1_r7)
    {
      if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r7.
        utranMobilityInformation_r7,
         rrc_UTRANMobilityInformation_r7_IEs,dl_CounterSynchronisationInfo))
      {
        srns_relocation_present = TRUE;
      }
    }
  }
  else if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.utranMobilityInformation_r3, 
      rrc_UTRANMobilityInformation_r3_IEs,dl_CounterSynchronisationInfo))
  {
    srns_relocation_present = TRUE;
  }

  return srns_relocation_present;
}


/*===========================================================================

FUNCTION rrcumi_send_utran_mobility_info_confirm

DESCRIPTION
  This function sends the UTRAN Mobility Info command on UL DCCH

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcumi_send_utran_mobility_info_confirm
(
 void
)
{
  rrc_UL_DCCH_Message *msg_ptr=NULL;    /* Pointer to uplink DCCH message */ 

  rlc_lc_id_type ul_lc_id;              /* Uplink logical channel ID */
  
  rrcsend_status_e_type send_status;    /* The status of sending the Initial
                                         * Direct Transfer message */


  /* Build a UTRAN Mobility Info msg. Start by allocating an Uplink 
   * DCCH message. This memory is released by rrcsend_send_ul_sdu().
   */
  /* Determine the logical channel id for the Uplink DCCH
   */
  ul_lc_id = rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH,
                                       DCCH_AM_RADIO_BEARER_ID, 
                                       UE_MODE_ACKNOWLEDGED_DATA);
 /* Ensure that LCM found a valid logical channel id
   */
  if (ul_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {  
    /* Found a valid RLC Logical Channel Id */
    msg_ptr = (rrc_UL_DCCH_Message *)rrc_malloc( 
                sizeof(struct rrc_UL_DCCH_Message));

  /*call the build function to build the confirm message.  build message is
   *seperate in this procedure as Cell Update also needs to build the message
   *and this is exerned.
   */  
    rrcumi_build_utran_mobility_info_confirm_msg(&utran_mob_inf_cnf, msg_ptr);

   /* Send the UTRAN Mobility Info on the Uplink DCCH
    */
    if (umi.umi_for_srns_relocation == TRUE)
    {
      send_status = rrcsend_send_ul_sdu(RRC_PROCEDURE_UMI,
                                          msg_ptr,
                                          ul_lc_id,
                                        RRCSEND_L2ACK_REQUIRED);

      umi.curr_substate = RRCUMI_WAIT_FOR_L2_ACK;
      rrcumi_substate = RRCUMI_WAIT_FOR_L2_ACK; 
    }
    else
    {
      send_status = rrcsend_send_ul_sdu(RRC_PROCEDURE_UMI,
                                        msg_ptr,
                                        ul_lc_id,
                                        RRCSEND_L2ACK_NOT_REQUIRED);
      umi.curr_substate = RRCUMI_INITIAL;
      rrcumi_substate = RRCUMI_INITIAL;
      rrcumi_clear_procedure();
    }
    

    if (send_status == RRCSEND_SUCCESS)
    {
      WRRC_MSG0_HIGH("UTRAN Mobility Info Message Sent");
    }
    else
    {
      WRRC_MSG1_ERROR("Error %d sending UTRAN Mobility Info",
                send_status);
    }
  }/* Found a valid RLC Logical Channel Id */
  else
  {
    WRRC_MSG0_HIGH("Can't find RLC ID for RB 2");
  }
} /* rrcumi_send_utran_mobility_info_confirm */

/*===========================================================================

FUNCTION rrcumi_process_rnti_info

DESCRIPTION
   

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcumi_process_rnti_info ( void )
{
  /*if neither c_rnti or u_rnti needs updating, then do not send the
     *update_req command
     */

  rrcrb_rnti_update_info_type rnti_update_info; /* structure to be used to request RNTI update */

  /* send rnti update request if URNTI was present in the rebsetup message */
  if ((umi.new_urnti_valid == TRUE) || (umi.new_crnti_valid == TRUE))
  {
    /* Send the RNTI update request to rrcllc to update with new-URNTI */
    /* Send a RNTI Update Request */
    if (umi.new_crnti_valid)
    {
      rnti_update_info.crnti_action    = RRC_RNTI_UPDATE;
    }
    else
    {
      rnti_update_info.crnti_action    = RRC_RNTI_NOCHANGE;
    }
    if (umi.new_urnti_valid)
    {
      rnti_update_info.urnti_action    = RRC_RNTI_UPDATE;
    }
    else
    {
      rnti_update_info.urnti_action    = RRC_RNTI_NOCHANGE;
    }
    rnti_update_info.urnti                = umi.new_urnti;
    rnti_update_info.crnti                = (uint16)umi.new_crnti;
    rnti_update_info.procedure            = RRC_PROCEDURE_UMI;
    rnti_update_info.rlc_re_establish_srb = FALSE;
    rnti_update_info.rlc_re_establish_rab = FALSE;
    rnti_update_info.cnf_reqd             = TRUE;

    /*change rrcumi_substate to wait for rnti update confirm*/
    rrcrb_send_rnti_update_req(rnti_update_info);
    rrcumi_substate = RRCUMI_WAIT_FOR_RNTI_UPDATE_CNF;
    umi.curr_substate = RRCUMI_WAIT_FOR_RNTI_UPDATE_CNF;
  }
  else
  {
    if(rrc_get_state() == RRC_STATE_CELL_DCH)
    {
      rrc_notify_cell_id_in_dch();
    }
    /*no update of rnti is required.  so just send UMI CNF message*/
    rrcumi_send_utran_mobility_info_confirm( );
  }
}

/*===========================================================================

FUNCTION rrcumi_process_rnti_update_cnf_substate  

DESCRIPTION
   This function processes UMI messages in WAIT_FOR_RNTI_UPDATE_CNF state.
   We could receive an update cnf or a state change ind in this substate.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcumi_process_rnti_update_cnf_substate
(
rrc_cmd_type *cmd_ptr                         /* Pointer to the RRC Command */  
)
{

  rrcumi_msg_status_e_type       failure_status;  /* Failure Cause - should be a subset
                                                   * of the failure causes specified by
                                                   * the spec. 
                                                   */
  rrc_ProtocolErrorCause protocolErrorCause;
  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_RNTI_UPDATE_CNF:
      if (cmd_ptr->cmd.rnti_update_cnf.status == SUCCESS) 
      {
        /*if the received message in update cnf, then send confirm message
         * with the saved data in utran_mob_inf_cnf
         */
        WRRC_MSG0_HIGH("Received RNTI_UPDATE_CNF from LLC");

  
#ifdef FEATURE_WCDMA_REL7_SPECRS
        rrc_save_rnc_cap_change_support();
#endif

        rrcumi_send_utran_mobility_info_confirm( );
        if(rrc_get_state() == RRC_STATE_CELL_DCH)
        {
          rrc_notify_cell_id_in_dch();
        }
      }
      else
      {
        /* RNTI update didn't succeed.  Send UMI failure with cause Invalid_Config */
        failure_status = RRCUMI_MSG_INVALID_CONFIG;
        protocolErrorCause = rrc_ProtocolErrorCause_spare1;
        
        rrcumi_send_umi_failure(utran_mob_inf_cnf.transaction_id, failure_status,protocolErrorCause);
        
        umi.curr_substate = RRCUMI_INITIAL;
        rrcumi_substate = RRCUMI_INITIAL;
        rrcumi_clear_procedure();
      }

      break;
    
    default:
      WRRC_MSG1_HIGH("Invalid message rcvd in UMI substate: %d", rrcumi_substate);
      break;
  }
}
/*===========================================================================

FUNCTION   rrcumi_check_and_get_primary_plmn_id

DESCRIPTION

  This function will check whether primary plmn id is present 
  in r3,r5 path of UMI message. If present then it will return 
  primary plmn id.
        
DEPENDENCIES
  None.

RETURN VALUE
TRUE : IF Primary PLMN I.D is present in UMI message in r3,r5 path
FFALSE: Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
static boolean rrcumi_check_and_get_primary_plmn_id
(
  /* Pointer to the UMI message */
  rrc_UTRANMobilityInformation *msg_ptr,
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
      /*Check to see if primary PLMN I.D is present in UMI message ext
       * rrc_UTRANMobilityInformation-v690ext which is present in 
       * v690NonCriticalExtensions in r3 message
       */
      if((msg_ptr->u.r3.m.v3a0NonCriticalExtensionsPresent)&&
          (msg_ptr->u.r3.v3a0NonCriticalExtensions.m.laterNonCriticalExtensionsPresent)&&
          (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          m.v690NonCriticalExtensionsPresent)&&
          (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v690NonCriticalExtensions.utranMobilityInformation_v690ext.m.
            primary_plmn_IdentityPresent)
        )
      {
        WRRC_MSG0_HIGH("NWS:UMI msg contains primary_plmn_Id in r3 path");
        primary_plmn_Id_present = TRUE;
        *primary_plmn_Identity = msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v690NonCriticalExtensions.utranMobilityInformation_v690ext.primary_plmn_Identity;
      }
      break;
    case MSG_REL5:
      /*Check to see if primary PLMN I.D is present in UMI message ext
       * rrc_UTRANMobilityInformation-v690ext which is present in 
       * v690NonCriticalExtensions in r5 message
       */
      if((msg_ptr->u.later_than_r3.criticalExtensions.t == T_rrc_UTRANMobilityInformation_criticalExtensions_2_r5)&&
          (msg_ptr->u.later_than_r3.criticalExtensions.u.r5.m.v690NonCriticalExtensionsPresent)&&
          (msg_ptr->u.later_than_r3.criticalExtensions.u.r5.v690NonCriticalExtensions.
            utranMobilityInformation_v690ext.m.primary_plmn_IdentityPresent)
        )
      {
        WRRC_MSG0_HIGH("NWS:UMI msg contains primary_plmn_Id in r5 path");
        primary_plmn_Id_present = TRUE;
        *primary_plmn_Identity = msg_ptr->u.later_than_r3.criticalExtensions.u.r5.
             v690NonCriticalExtensions.utranMobilityInformation_v690ext.primary_plmn_Identity;
      }
      break;
   
    default:
      WRRC_MSG1_ERROR("NWS: Msg version = %d, Not supported",msg_version);
      break;
  }
  
  return primary_plmn_Id_present;
}

/*===========================================================================

FUNCTION rrcumi_process_utran_mobility_info_command  

DESCRIPTION
   This function processes the UTRAN MOBILITY INFO received on DL DCCH
   and extracts the Transaction ID.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcumi_process_utran_mobility_info_command
(
  rrc_cmd_type *cmd_ptr                         /* Pointer to the RRC Command */
)
{
  rrc_DL_DCCH_Message *dcch_msg_ptr=NULL;       /* Pointer to Downlink DCCH */
  
  uint8 message_t;                         /* Local variable for message
                                                   t */
  rrc_RRC_TransactionIdentifier  transaction_id; /*transaction id of the message */
  rrcumi_msg_status_e_type       failure_status;  /* Failure Cause - should be a subset
                                                of the failure causes specified by
                                                  the spec. */
  rrc_ProtocolErrorCause protocolErrorCause;

  rrc_UE_ConnTimersAndConstants* ue_conn_timers_and_constants_ptr;
  rrc_UE_ConnTimersAndConstants_v3a0ext * ext_ptr;
  rrc_msg_ver_enum_type msg_version = MSG_REL99;

  rrc_UE_ConnTimersAndConstants_v860ext *ue_conn_timers_and_constants_v860ext_ptr = NULL;

  rrc_rlc_re_est_for_srns_rb_e_type rb_type; /*rb type tobe reestablished */
  /* Get a pointer to the UTRAN Mobility Information message */
  rrc_UTRANMobilityInformation            *msg_ptr=NULL;
  /* IP check status */
  rrcsmc_srns_ip_check_status_e_type      ip_check_status;

   /*Boolean used to indicate primary plmn i.d is present in OTA msg*/
  boolean primary_plmn_Id_present = FALSE;
   /*Variable to hold primary plmn i.d if present in OTA msg*/
  rrc_PLMN_Identity primary_plmn_Identity;

  rrc_UTRANMobilityInformation_r7_IEs * r7_umi_msg_ptr=NULL;

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
  rrc_DedicatedPriorityInformation *ded_pri_info_ptr = NULL;
#endif
#ifdef FEATURE_WCDMA_HS_FACH
  rrc_state_e_type rrc_state = rrc_get_state();
  boolean new_hrnti_or_ernti_present = FALSE;
#endif

  message_t = rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                                cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);
  /* Check if the received message is UTRAN Mobility Info message */
  if( message_t != T_rrc_DL_DCCH_MessageType_utranMobilityInformation)
  {
    WRRC_MSG1_HIGH("Invalid Message: %d is forwarded", message_t);
    return;
  }

  /* Get the OTA message pointer */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *) 
                  cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  WRRC_MSG0_HIGH("Received UTRAN Mobility Info Message");

  if(dcch_msg_ptr == NULL) /* DCCH ASN1 decoding is un-successful */
  {
    /*log the protocol error*/
    rrc_log_protocol_error( (uint8) (rrc_get_state()),
                            (uint8) RRC_PROCEDURE_UMI,
                            (uint8) T_rrc_FailureCauseWithProtErr_protocolError,
                            (uint8) rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError                            
                            );
	
    /*can not find the transaction id. So use zero */ 
    transaction_id = 0;
    failure_status = RRCUMI_MSG_PROTOCOL_ERROR;
    protocolErrorCause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;

    rrcumi_send_umi_failure (transaction_id, failure_status,protocolErrorCause);

    umi.curr_substate = RRCUMI_INITIAL;

    rrcumi_clear_procedure();

  }
  else
  { 
    rrc_CN_InformationInfoFull  *core_network_info = NULL;
    ASN1BitStr32                *msg_crnti_ptr = NULL;
    rrc_U_RNTI                  *msg_urnti_ptr = NULL;
    rrc_H_RNTI                  *msg_hrnti_ptr = NULL;
    /* Clear UMI internal database to avoid stale values */    
    rrcumi_clear_procedure();
    /* Get the pointer to the actual UMI message */
    msg_ptr = &(dcch_msg_ptr->message.u.utranMobilityInformation);

    
    /*Get the transaction id from the UMI message */
    if(dcch_msg_ptr->message.u.utranMobilityInformation.t == 
                                 T_rrc_UTRANMobilityInformation_r3)
    {
      msg_version = MSG_REL99;
      transaction_id = 
                   dcch_msg_ptr->message.u.utranMobilityInformation.
                   u.r3.utranMobilityInformation_r3.rrc_TransactionIdentifier;
    }
    else
    {
      if ((dcch_msg_ptr->message.u.utranMobilityInformation.t == 
           T_rrc_UTRANMobilityInformation_later_than_r3)
          && (dcch_msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.criticalExtensions.t == 
          T_rrc_UTRANMobilityInformation_criticalExtensions_2_criticalExtensions )
          && (dcch_msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.criticalExtensions.u.criticalExtensions.t == 
          T_rrc_UTRANMobilityInformation_criticalExtensions_1_r7 ))
      {
        /* Strictly speaking non-R99 message doesn't mean Rel-5 but if the 
           received message belongs to any other release, it would be filtered by
           "rrcumi_validate_utran_mobility_info_command" call below. */
        msg_version = MSG_REL7;
        r7_umi_msg_ptr = &dcch_msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r7.utranMobilityInformation_r7;
      }
      else
      {
        /* Strictly speaking non-R99 message doesn't mean Rel-5 but if the 
           received message belongs to any other release, it would be filtered by
           "rrcumi_validate_utran_mobility_info_command" call below. */
        msg_version = MSG_REL5;
      }
      transaction_id = dcch_msg_ptr->message.u.utranMobilityInformation.u.
                         later_than_r3.rrc_TransactionIdentifier;
    }
#ifdef FEATURE_WCDMA_HS_RACH
    if(TRUE == rrccu_get_hsrach_sib_change())
    {
      MSG_HIGH("Delay CU flag is set. Do not handle UMI. Send UMI failure",0,0,0);
            /* Set the failure status */
      failure_status = RRCUMI_MSG_INVALID_CONFIG;
      /* Send UMI failure */
      rrcumi_send_umi_failure (transaction_id,
                               failure_status,
                               umi.status.prot_err_cause);
      return;
    }
#endif
    /*Check to see if primary PLMN I.D is present in UMI message ext
     * rrc_UTRANMobilityInformation-v690ext which is present in 
     * v690NonCriticalExtensions
     */
    if(msg_version == MSG_REL7)
    {
       if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(r7_umi_msg_ptr, primary_plmn_Identity))
       {
           primary_plmn_Id_present = TRUE;
           primary_plmn_Identity = r7_umi_msg_ptr->primary_plmn_Identity;
       }
    }
    else
    {
      primary_plmn_Id_present = rrcumi_check_and_get_primary_plmn_id(msg_ptr,
                                                               &primary_plmn_Identity,msg_version);
    }


    /* ASN1 decoding is successful */
    /* Validate the RRC UTRAN MOBILITY INFO.  If validation==FALSE then return */
    if (RRCUMI_SUCCESS != rrcumi_validate_utran_mobility_info_command(dcch_msg_ptr) )
    {
      WRRC_MSG0_HIGH("UTRAN Mobility InfoMSG VALIDATION FAILED");

      /* Set the failure status */
      failure_status = RRCUMI_MSG_UNSUPPORTED_CONFIG;

      /* Send UMI failure */
      rrcumi_send_umi_failure (transaction_id,
                               umi.status.failure_status,
                               umi.status.prot_err_cause);

      return;
    }


    /* If the UMI msg contains information for doing SRNS relocation */
    if (TRUE == rrcumi_validate_srns_relocation_info(msg_ptr))
    {
      ip_check_status = rrcsmc_check_integrity_for_srns_relocation(cmd_ptr, msg_version);

      switch (ip_check_status)
      {
      case RRCSMC_SRNS_INTEGRITY_CHECK_SUCCESS:
        /*continue further processing with the message */
        break;

      case RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE:
        /*Discard the message and continue with ongoing procedure as if this
        message was not received */
        WRRC_MSG0_ERROR("IP check failed. Discarding UMI msg");
        rrcumi_clear_procedure();
        return;

      case RRCSMC_SRNS_SECURITY_INFO_INVALID:
      case RRCSMC_INCOMPATIBLE_SECURITY_RECONFIGURATION:

        umi.status.failure_status = RRCUMI_MSG_INVALID_CONFIG;
        /* Clear UMI procedure here. No other UMI processing will be active
           as only one UMI is processed at one time */
        rrcumi_clear_procedure();
        return;

      default:
        /*should not come here */
        break;
      }
    }
    //code to process integrity info in UMI message..
    else if(rrcumi_validate_umi_ip_mode(msg_ptr) == TRUE)
    {
      ip_check_status = rrcsmc_check_integrity_for_srns_relocation(cmd_ptr, msg_version);


      switch (ip_check_status)
      {
      case RRCSMC_SRNS_INTEGRITY_CHECK_SUCCESS:
        //commit fresh values for srns relocation
        rrcsmc_commit_fresh_for_srns_relocation();
        break;

      case RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE:
        /*Discard the message and continue with ongoing procedure as if this
        message was not received */
        WRRC_MSG0_ERROR("IP check failed. Discarding UMI msg");
        rrcumi_clear_procedure();
        return;

      case RRCSMC_SRNS_SECURITY_INFO_INVALID:
      case RRCSMC_INCOMPATIBLE_SECURITY_RECONFIGURATION:

        umi.status.failure_status = RRCUMI_MSG_INVALID_CONFIG;
        /* Clear UMI procedure here. No other UMI processing will be active
           as only one UMI is processed at one time */
        rrcumi_clear_procedure();
        return;

      default:
        /*should not come here */
        break;
      }
    }

    ue_conn_timers_and_constants_ptr = NULL;
    ext_ptr = NULL;

  ue_conn_timers_and_constants_v860ext_ptr = NULL;

    /* Update UE Conn Timers and Constants if present and set internal flag*/
    if(msg_version == MSG_REL99)
    {
      if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.
         utranMobilityInformation_r3,
         rrc_UTRANMobilityInformation_r3_IEs,ue_ConnTimersAndConstants))
      {
        ue_timer_const_rcvd_status = RRCUMI_UE_TIMER_CONST_RCVD;
        ue_conn_timers_and_constants_ptr = &(msg_ptr->u.r3.utranMobilityInformation_r3.
                    ue_ConnTimersAndConstants);
      }

      if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3,
         rrc_UTRANMobilityInformation_r3,v3a0NonCriticalExtensions))
      {
        ue_timer_const_rcvd_status = RRCUMI_UE_TIMER_CONST_RCVD;

        ext_ptr = &(msg_ptr->u.r3.v3a0NonCriticalExtensions.
          utranMobilityInformation_v3a0ext.ue_ConnTimersAndConstants_v3a0ext);

        if(msg_ptr->u.r3.v3a0NonCriticalExtensions.m.laterNonCriticalExtensionsPresent)
        {
          if(msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.m.v690NonCriticalExtensionsPresent)
          {
            if(msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              v690NonCriticalExtensions.m.v860NonCriticalExtentionsPresent)
            {
              if(msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
                 v690NonCriticalExtensions.v860NonCriticalExtentions.utranMobilityInformation_v860ext.
                 ue_ConnTimersAndConstants.m.t_323Present)
              {
                ue_conn_timers_and_constants_v860ext_ptr = &(msg_ptr->u.r3.v3a0NonCriticalExtensions.
                  laterNonCriticalExtensions.v690NonCriticalExtensions.v860NonCriticalExtentions.
                  utranMobilityInformation_v860ext.ue_ConnTimersAndConstants);
              }
              else
              {
                ue_conn_timers_and_constants_v860ext_ptr = NULL;
              }              
            }
          }
        }
      }

      if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.
         utranMobilityInformation_r3,
         rrc_UTRANMobilityInformation_r3_IEs,cn_InformationInfo))
      {
        core_network_info = &(msg_ptr->
                          u.r3.utranMobilityInformation_r3.cn_InformationInfo);
        WRRC_MSG0_HIGH("Extracted core network info from R3 IE");
      }

      if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.
         utranMobilityInformation_r3, 
         rrc_UTRANMobilityInformation_r3_IEs,new_C_RNTI))
      {
        msg_crnti_ptr = &msg_ptr->u.r3.
             utranMobilityInformation_r3.new_C_RNTI;
      }

      if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.
         utranMobilityInformation_r3,
         rrc_UTRANMobilityInformation_r3_IEs,new_U_RNTI))
      {
        msg_urnti_ptr = &(msg_ptr->u.
         r3.utranMobilityInformation_r3.new_U_RNTI);
      }
    }
    else if(msg_version == MSG_REL7)
    {
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(r7_umi_msg_ptr,
         rrc_UTRANMobilityInformation_r7_IEs,ue_ConnTimersAndConstants))
      {
        ue_timer_const_rcvd_status = RRCUMI_UE_TIMER_CONST_RCVD;

        /*lint -e740 */
        ue_conn_timers_and_constants_ptr =  (rrc_UE_ConnTimersAndConstants *) &(r7_umi_msg_ptr->ue_ConnTimersAndConstants);
        /*lint +e740 */
        
        if(dcch_msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r7.m.v860NonCriticalExtensionsPresent)
        {
          ue_conn_timers_and_constants_v860ext_ptr = &(dcch_msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.
                                                      criticalExtensions.u.criticalExtensions.u.r7.v860NonCriticalExtensions.
                                                      utranMobilityInformation_v860ext.ue_ConnTimersAndConstants);
        }
        else
        {
          ue_conn_timers_and_constants_v860ext_ptr = NULL;
        }
      }

      if(RRC_MSG_COMMON_BITMASK_IE_PTR(r7_umi_msg_ptr, 
         rrc_UTRANMobilityInformation_r7_IEs,cn_InformationInfo))
      {
        core_network_info = &(r7_umi_msg_ptr->cn_InformationInfo);
        WRRC_MSG0_HIGH("Extracted core network info from R5 IE");
      }
#ifdef FEATURE_WCDMA_REL7_SPECRS
         /*If UMI OTA provides "supportForChangeOfUE_Capability" IE, we need to store it in
         "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
         "rrc_rnc_cap_change_support" can be updated accordingly. */ 
      if(RRC_MSG_COMMON_BITMASK_IE_PTR(r7_umi_msg_ptr,
        rrc_UTRANMobilityInformation_r7_IEs,supportForChangeOfUE_Capability))    
      {
        if(r7_umi_msg_ptr->supportForChangeOfUE_Capability == TRUE)
        {
          rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_SUPPORTED;
        }
        else
        {
          rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_UNSUPPORTED;
        }
      }
      else
      {
        rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_NONE;
      }  
  #endif

      if(RRC_MSG_COMMON_BITMASK_IE_PTR(r7_umi_msg_ptr,
         rrc_UTRANMobilityInformation_r7_IEs,new_C_RNTI))
      {
        msg_crnti_ptr = &r7_umi_msg_ptr->new_C_RNTI;
      }

      if(RRC_MSG_COMMON_BITMASK_IE_PTR(r7_umi_msg_ptr,
         rrc_UTRANMobilityInformation_r7_IEs,new_U_RNTI))
      {
        msg_urnti_ptr = &(r7_umi_msg_ptr->new_U_RNTI);
      }

      if(RRC_MSG_COMMON_BITMASK_IE_PTR(r7_umi_msg_ptr, 
         rrc_UTRANMobilityInformation_r7_IEs,new_H_RNTI))
      {
        msg_hrnti_ptr = &(r7_umi_msg_ptr->new_H_RNTI);
#ifndef FEATURE_WCDMA_HS_FACH
        WRRC_MSG0_ERROR("HRNTI not yet supported in UMI");
#endif
      }
    }

    else if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
    {
      if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.r5.
         utranMobilityInformation_r5,
         rrc_UTRANMobilityInformation_r5_IEs,ue_ConnTimersAndConstants))
      {
        ue_timer_const_rcvd_status = RRCUMI_UE_TIMER_CONST_RCVD;

        /*lint -e740 */
        ue_conn_timers_and_constants_ptr =  (rrc_UE_ConnTimersAndConstants *) &(msg_ptr->u.
            later_than_r3.criticalExtensions.u.r5.utranMobilityInformation_r5.
            ue_ConnTimersAndConstants);
        /*lint +e740 */
        
        if(msg_ptr->u.later_than_r3.criticalExtensions.u.r5.m.v690NonCriticalExtensionsPresent)
          
        {
          if(msg_ptr->u.later_than_r3.criticalExtensions.u.r5.v690NonCriticalExtensions.m.v860NonCriticalExtentionsPresent)
          {
            if(msg_ptr->u.later_than_r3.criticalExtensions.u.r5.v690NonCriticalExtensions.v860NonCriticalExtentions.
				utranMobilityInformation_v860ext.ue_ConnTimersAndConstants.m.t_323Present)
            {
              ue_conn_timers_and_constants_v860ext_ptr = &(msg_ptr->u.later_than_r3.criticalExtensions.u.r5.
                                                        v690NonCriticalExtensions.v860NonCriticalExtentions.
                                                        utranMobilityInformation_v860ext.ue_ConnTimersAndConstants);
            }
            else
            {
               ue_conn_timers_and_constants_v860ext_ptr = NULL;
            }
          }
        }
      }

      if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.
        criticalExtensions.u.r5.
         utranMobilityInformation_r5,
         rrc_UTRANMobilityInformation_r5_IEs,cn_InformationInfo))
      {
        core_network_info = &(msg_ptr->u.later_than_r3.criticalExtensions.
                            u.r5.utranMobilityInformation_r5.cn_InformationInfo);
        WRRC_MSG0_HIGH("Extracted core network info from R5 IE");
      }

      if(RRC_MSG_COMMON_BITMASK_IE(dcch_msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.
         criticalExtensions.u.r5.utranMobilityInformation_r5,
         rrc_UTRANMobilityInformation_r5_IEs,new_C_RNTI))
      {
        msg_crnti_ptr = &dcch_msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.
         criticalExtensions.u.r5.utranMobilityInformation_r5.new_C_RNTI;
      }

      if(RRC_MSG_COMMON_BITMASK_IE(dcch_msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.
         criticalExtensions.u.r5.utranMobilityInformation_r5,
         rrc_UTRANMobilityInformation_r5_IEs,new_U_RNTI))
      {
        msg_urnti_ptr = &(msg_ptr->u.
          later_than_r3.criticalExtensions.u.r5.utranMobilityInformation_r5.
          new_U_RNTI);
      }
    }

    if((ue_conn_timers_and_constants_ptr != NULL) || 
        (ext_ptr != NULL))
    {    
      (void)rrc_update_ue_conn_timers_and_constants
                  (ue_conn_timers_and_constants_ptr,ext_ptr,
                  ue_conn_timers_and_constants_v860ext_ptr);

      if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
      {
        if(msg_version == MSG_REL5)
        {  
          (void)rrc_update_constants_n312_and_n315_r5(&(msg_ptr->u.
              later_than_r3.criticalExtensions.u.r5.utranMobilityInformation_r5.
              ue_ConnTimersAndConstants));
        } 
        else if(msg_version == MSG_REL7)
        {  
          (void)rrc_update_constants_n312_and_n315_r5(&(r7_umi_msg_ptr->ue_ConnTimersAndConstants));
        }
      }

      if(rrc_get_state() == RRC_STATE_CELL_DCH)
      {
        l1_upd_dl_sync_parms_cmd_type  umi_info;
        rrc_cmd_type                  *rrc_cmd_ptr;
        l1_req_cmd * rrc_cmd_l1_req = NULL;

        /* Fill the contents of the connected mode timers */
        umi_info.N315 = (uint16)RRC_GET_N315();
        umi_info.N312 = (uint16)RRC_GET_N312();
        umi_info.N313 = rrc_translate_n313(RRC_GET_N313());
        umi_info.T312 = (uint8)RRC_GET_T312();
        umi_info.T313 = (uint8)RRC_GET_T313();

        rrc_cmd_ptr = rrc_get_int_cmd_buf();
        RRC_GET_POINTER_L1_REQ_VALUE(rrc_cmd_ptr->cmd.rrc_llc_req.l1_req,rrc_cmd_l1_req);

       
        rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_LLC_CMD_REQ;
        rrc_cmd_ptr->cmd.rrc_llc_req.procedure = RRC_PROCEDURE_UMI;
        rrc_cmd_ptr->cmd.rrc_llc_req.cmd_dest  = LAYER1_PHY;

        rrc_cmd_l1_req->connected_timer_info = umi_info;
        rrc_cmd_ptr->cmd.rrc_llc_req.l1_cmd.cmd_id = CPHY_UPD_DL_SYNC_PARMS;
        rrc_put_int_cmd(rrc_cmd_ptr);
        } 
    }

    /* Get the transaction identifier from the UTRAN MOBILITY INFO and
           call the UTRAN Mobility Info command
    */
    utran_mob_inf_cnf.transaction_id = transaction_id;

    /* Get the cn information if present and update upper layers */
    if(core_network_info != NULL)
    {
      if( rrc_ccm_send_nas_cn_info_full(core_network_info,
                                    primary_plmn_Id_present,
                                    &primary_plmn_Identity) == FAILURE)
      {
        /*flag an error if not successful*/
        WRRC_MSG0_ERROR("Nas CN Info send failed");
      }
    }

    /*check if a new c_rnti is present and update rnti_update_req struct*/
    if(msg_crnti_ptr != NULL)
    {
      rrc_translate_crnti(msg_crnti_ptr, &umi.new_crnti);
      umi.new_crnti_valid = TRUE;
    }

    /*check if a new u_rnti is present and update rnti_update_req struct*/
    if(msg_urnti_ptr != NULL)
    {
      rrc_translate_urnti(msg_urnti_ptr, &umi.new_urnti);
      umi.new_urnti_valid = TRUE;
    }
    /*check if a new u_rnti is present and update rnti_update_req struct*/
    if(msg_hrnti_ptr != NULL)
    {
      (void)rrc_translate_hrnti(msg_hrnti_ptr, &umi.new_hrnti);
      umi.new_hrnti_valid = TRUE;
#ifdef FEATURE_WCDMA_HS_FACH
    if ((TRUE == rrcenhstate_camped_cell_supports_e_fach())
     &&((rrc_state == RRC_STATE_CELL_FACH))
     )
      {
        WRRC_MSG1_HIGH("EFACH:RECEIVED H-RNTI %d",umi.new_hrnti);
        new_hrnti_or_ernti_present=TRUE;
        ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.h_rnti = umi.new_hrnti; // OC will be overwritten anyway
        transition_config.toc_ptr->l1_hsdpa_info.hs_dsch_info.h_rnti = umi.new_hrnti; // CC will finally get from TOC: important
        /*HRNTI will already be valid by now but still for protection set it*/
        rrc_set_hrnti_status(ordered_config_ptr, HRNTI_VALID);
        rrc_set_hrnti_status(transition_config.toc_ptr, HRNTI_VALID);
      }
#endif
    }
    /*check whether this reconfiguration involves SRNS relocation */
    if (umi.umi_for_srns_relocation == TRUE)
    {
      //commit fresh values for srns relocation
      rrcsmc_commit_fresh_for_srns_relocation();

      /* Re-establish SRB2 before sending the response message */
      rb_type  = RRC_RE_EST_RB_2;
      rrcrb_send_rlc_re_est_req_to_smc(RRC_PROCEDURE_UMI, rb_type, TRUE);

      umi.curr_substate = RRCUMI_WAIT_FOR_REESTABLISH_SRB2;
      rrcumi_substate = RRCUMI_WAIT_FOR_REESTABLISH_SRB2;

      rrcdormancy_update_dormancy_for_srns_hho_cco(RRC_PROCEDURE_UMI);
    }
    else
    { /*normal reconfiguration without SRNS relocation */   
      /* Send the UMI Complete message */
      rrcumi_process_rnti_info();
    }

#ifdef FEATURE_WCDMA_ABSOLUTE_PRIORITY
    if((msg_version == MSG_REL7) &&
        (dcch_msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.
        criticalExtensions.u.criticalExtensions.u.r7.m.v860NonCriticalExtensionsPresent) &&
         (dcch_msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r7.
          v860NonCriticalExtensions.utranMobilityInformation_v860ext.m.dedicatedPriorityInformationPresent))
    {
      WRRC_MSG0_HIGH("ABSOL PRI: Dedicated priority information present in UMI message");
      ded_pri_info_ptr =
        &(dcch_msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r7.
          v860NonCriticalExtensions.utranMobilityInformation_v860ext.dedicatedPriorityInformation);

      /*Call the measurement function to update the dedicated priority information stored*/
      rrcmeas_update_dedicated_priority_info_from_ota(ded_pri_info_ptr);

      /*Post the command to measurement, so that meas can send the meas_req to WL1*/
      rrcmeas_post_dedicated_priority_change_indication();
    }
#endif
     /* E-RNTI assignment in UMI is limited to the UE in CELL_FACH. 25.331_CR4773  */
#ifdef FEATURE_WCDMA_HS_RACH
    if((rrchsrach_camped_cell_supports_hsrach() == TRUE) 
      && (rrc_state== RRC_STATE_CELL_FACH)
      && (msg_version == MSG_REL7) &&
        (dcch_msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.
        criticalExtensions.u.criticalExtensions.u.r7.m.v860NonCriticalExtensionsPresent) &&
         (dcch_msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r7.
          v860NonCriticalExtensions.utranMobilityInformation_v860ext.m.newPrimary_E_RNTIPresent))
    {
      new_hrnti_or_ernti_present = TRUE;
      /*store the ernti*/
      ordered_config_ptr->l1_hs_rach_e_info.e_dl_info.primary_e_rnti_present = TRUE;
      (void)rrc_translate_ernti(&dcch_msg_ptr->message.u.utranMobilityInformation.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r7.
      v860NonCriticalExtensions.utranMobilityInformation_v860ext.newPrimary_E_RNTI, &ordered_config_ptr->l1_hs_rach_e_info.e_dl_info.primary_e_rnti);

      WRRC_MSG1_HIGH("RRCHSRACH:Received E-RNTI %d ",ordered_config_ptr->l1_hs_rach_e_info.e_dl_info.primary_e_rnti);

      transition_config.toc_ptr->l1_hs_rach_e_info.e_dl_info.primary_e_rnti_present = TRUE;
      transition_config.toc_ptr->l1_hs_rach_e_info.e_dl_info.primary_e_rnti = ordered_config_ptr->l1_hs_rach_e_info.e_dl_info.primary_e_rnti;
    }
#endif

#ifdef FEATURE_WCDMA_HS_FACH
    /*Send a single channel config req if either hrnti or ernti is provided*/
    if(new_hrnti_or_ernti_present)
    {
      rrcllc_establish_srb_rab_in_efach(RRC_E_FACH_DEDICATED,RRC_PROCEDURE_UMI,FALSE);
    }
#endif	
  }
} /* rrcumi_process_utran_mobility_info_command */

/*===========================================================================

FUNCTION   rrcumi_is_srns_in_progress

DESCRIPTION
  This function returns if SRNS is in progress with UMI procedure

DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
boolean rrcumi_is_srns_in_progress
(
void
)
{
  return umi.umi_for_srns_relocation;
}

/*===========================================================================

FUNCTION   RRCUMI_PROCESS_RLC_REESTABLISHMENT_CNF

DESCRIPTION

  This function processes the received rlc reestablishment cnf from SMC
  <more later>.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcumi_process_rlc_reestablishment_cnf
(
void
)
{
  /* Check if rnti update has to be done */
  rrcumi_process_rnti_info();

}

/*===========================================================================

FUNCTION   RRCUMI_PROCESS_L2_ACK_IND

DESCRIPTION

  This function processes the received Layer 2 ACK indication.
  If the L2 ACK indicates that the UMI Complete message has
  been succesfully transmitted, the function clears ORDERED_CONFIG
  and goes back to the initial state.
  
  If the L2 ack indicates failure, it means that there is
  a serious failure in the air interface, so the procedure 
  sends a command to LLC to release all Radio Bearers and go
  to disconnected state.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcumi_process_l2_ack_ind
(
  rrc_downlink_l2ack_ind_type *ptr /* Pointer to the L2 Ack Ind cmd */
)
{

  rrc_rlc_re_est_for_srns_rb_e_type rb_type; /*rb type tobe reestablished */


  WRRC_MSG1_HIGH("L2 ack status %d [0=SUCCESS, 1= FAILURE] for UMI Complete msg received", ptr->status);
      

  if ( ptr->status == SUCCESS )
  {

    /* if SRNS relocation is involved with this message then re
    establish the RBs other than RB2 here */
    if (umi.umi_for_srns_relocation)
    {
      /* Initiate the re-establishment of remaining RBs before 
         RBRC procedure goes back to initial substate. 
         No need to wait for the confirmation */
      rb_type  = RRC_RE_EST_NON_RB_2_UM_AM;
      rrcrb_send_rlc_re_est_req_to_smc(RRC_PROCEDURE_UMI, rb_type, FALSE);
    }
    else
    {
      WRRC_MSG1_ERROR("SRNS reloc flag is ON in rrc state %d", rrc_get_state());
    }
  }
  else
  {
	/* L2 Ack has failed.Take UE to idle */
	(void)rrc_transition_to_disconnected_state(RRC_PROCEDURE_UMI, RRC_TX_TO_DISCON_OTHER);
  }

  /* Go to the next sub-state in the state machine. That's
  the initial substate since we've sent the TCRC complete
  message */
  umi.curr_substate = RRCUMI_INITIAL;
  rrcumi_substate = RRCUMI_INITIAL;

  /* Clear our internal variables */
  rrcumi_clear_procedure();
}

/*===========================================================================

FUNCTION   rrcumi_process_cell_fach_state

DESCRIPTION
   This function processes the UTRAN MOBILITY INFO received on DL DCCH
   and extracts the Transaction ID.  

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcumi_process_cell_fach_state
(
  rrc_cmd_type *cmd_ptr                         /* Pointer to the RRC Command */
)
{

  switch (rrcumi_substate)
  {
    case RRCUMI_INITIAL:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_DOWNLINK_SDU_IND:
          rrcumi_process_utran_mobility_info_command(cmd_ptr);
          break;

        default:
          WRRC_MSG2_HIGH("Invalid event: %d in Substate: %d",
                   cmd_ptr->cmd_hdr.cmd_id,rrcumi_substate);
          break;
      }
      break;

    case RRCUMI_WAIT_FOR_RNTI_UPDATE_CNF:
      rrcumi_process_rnti_update_cnf_substate(cmd_ptr);
      break;

  case RRCUMI_WAIT_FOR_REESTABLISH_SRB2:
    rrcumi_process_rlc_reestablishment_cnf();
    break;

  case RRCUMI_WAIT_FOR_L2_ACK:
    if (cmd_ptr->cmd_hdr.cmd_id == RRC_DOWNLINK_L2ACK_IND )
    {
      if (umi.umi_for_srns_relocation == TRUE)
      {
        rrcumi_process_l2_ack_ind(&(cmd_ptr->cmd.downlink_l2ack_ind));
      }
      else
      {
        WRRC_MSG0_ERROR("UMI not expecting L2 ACK now");
      }
      
    }
    break;
  
  default:
    WRRC_MSG1_HIGH("Invalid Substate for UMI: %d", rrcumi_substate);
    break;
  }
}

/*===========================================================================

FUNCTION   rrcumi_process_cell_dch_state

DESCRIPTION
   This function processes the UTRAN MOBILITY INFO received on DL DCCH
   and extracts the Transaction ID.  

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcumi_process_cell_dch_state
(
rrc_cmd_type *cmd_ptr                         /* Pointer to the RRC Command */
)
{

  switch (rrcumi_substate)
  {
  case RRCUMI_INITIAL:
    switch (cmd_ptr->cmd_hdr.cmd_id)
    {
    case RRC_DOWNLINK_SDU_IND:
      rrcumi_process_utran_mobility_info_command(cmd_ptr);
      break;

    default:
      WRRC_MSG2_HIGH("Invalid event: %d in Substate: %d",
      cmd_ptr->cmd_hdr.cmd_id,rrcumi_substate);
      break;
    }
    break;

  case RRCUMI_WAIT_FOR_RNTI_UPDATE_CNF:
    rrcumi_process_rnti_update_cnf_substate(cmd_ptr);
    break;

  case RRCUMI_WAIT_FOR_REESTABLISH_SRB2:
    rrcumi_process_rlc_reestablishment_cnf();
    break;

  case RRCUMI_WAIT_FOR_L2_ACK:
    if (cmd_ptr->cmd_hdr.cmd_id == RRC_DOWNLINK_L2ACK_IND )
    {
      if (umi.umi_for_srns_relocation == TRUE)
      {
        rrcumi_process_l2_ack_ind(&(cmd_ptr->cmd.downlink_l2ack_ind));
      }
      else
      {
        WRRC_MSG0_ERROR("UMI not expecting L2 ACK now");
      }
      
    }
    break;

    default:
      WRRC_MSG1_HIGH("Invalid Substate for UMI: %d", rrcumi_substate);
      break;
  }
}

/*===========================================================================

FUNCTION   rrcumi_process_disconnected_state

DESCRIPTION
   This function processes UMI in disconnected state.  This function will be
   called with there is state change while UMI is in wait for rnti update cnf

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
static void rrcumi_process_disconnected_state
(
  rrc_cmd_type *cmd_ptr                         /* Pointer to the RRC Command */
)
{
  switch (rrcumi_substate)
  {
    case RRCUMI_WAIT_FOR_RNTI_UPDATE_CNF:
      rrcumi_process_rnti_update_cnf_substate(cmd_ptr);
      break;
    
    default:
      WRRC_MSG1_HIGH("Invalid Substate for UMI: %d", rrcumi_substate);
      break;
  }
}/*rrcumi_process_disconnected_state*/
/*===========================================================================

FUNCTION rrcumi_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for UTRAN MOBILITY INFO
  The following events are handled by this procedure:

  RRC_DOWNLINK_SDU_IND Primitive (received from RLC) containing 
  UTRAN MOBILITY INFO and only in DCH state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcumi_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
)
{
 
  rrc_state_e_type rrc_state = rrc_get_state();

  WRRC_MSG2_HIGH_OPT("function rrcumi_procedure_event_handler is called in rrc_state = %d and rrcumi_substate = %d", rrc_state, rrcumi_substate);
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_UMI,rrcumi_substate,cmd_ptr->cmd_hdr.cmd_id);

  //UMI only registers from wildcard to disconnected, handle this right
  //here in procedure event handler
  if(cmd_ptr->cmd_hdr.cmd_id == RRC_STATE_CHANGE_IND)
  {
    rrcumi_substate             = RRCUMI_INITIAL;
    umi.umi_for_srns_relocation = FALSE;
    umi.new_crnti_valid         = FALSE;
    umi.new_urnti_valid         = FALSE;
    umi.new_hrnti_valid         = FALSE;
    ue_timer_const_rcvd_status  = RRCUMI_UE_TIMER_CONST_NOT_RCVD;
    return;
  }
#ifdef FEATURE_DUAL_SIM
  /* Ignore UMI if preemption is in progress. This is to avoid sending 'CPHY_UPD_DL_SYNC_PARMS'.
   L1 will be in idle state and if CPHY_UPD_DL_SYNC_PARMS is sent, it sends CPHY_ERR_IND and we ERR FATAL*/
  /* Ignoring is better than sending UMI failure because, failure message will anyways be purged due to CU */
  if((rrc_csp_waiting_to_release_lock()) && (cmd_ptr->cmd_hdr.cmd_id == RRC_DOWNLINK_SDU_IND))
  {
    MSG_HIGH("CSP is waiting to release lock. UMI is ignored", 0,0, 0);
    return;
  }
#endif
  switch (rrc_state)
  {

    case RRC_STATE_CELL_FACH:    
      /* CELL_FACH only valid state to receive UTRAN MOBILITY INFO message */
    rrcumi_process_cell_fach_state(cmd_ptr); 
    break;


    case RRC_STATE_DISCONNECTED: 
      /*we may receive this message when there is a transition to idle discon
        state while waiting for crnti_update_req_cnf.  so reset RRCUMI's state
        machine in this case*/
      rrcumi_process_disconnected_state(cmd_ptr);
      break;

  case RRC_STATE_CELL_DCH:
    rrcumi_process_cell_dch_state(cmd_ptr);
    break;

    case RRC_STATE_CONNECTING:
    case RRC_STATE_CELL_PCH:
    case RRC_STATE_URA_PCH:
      /* UTRAN MOBILITY INFO is valid only in cell fach state */
      WRRC_MSG1_HIGH("RRCUMI Msg received in Invalid State: %d",
                rrc_state); 
      break;

    default: 
      /* This should never happen */
      ERR_FATAL("RRCUMI Msg received in Invalid State: %d",rrc_state,0,0);
      break;
   }  
} /* rrcumi_procedure_event_handler */
/*===========================================================================

FUNCTION rrcumi_init_procedure

DESCRIPTION
  This function initializes required parameters for UTRAN MOBILITY INFO
  procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcumi_init_procedure
( 
  void
)
{
  /*initialize UMI substate to INITIAL and const_timer received state to
   *not received
   */
  rrcumi_substate            = RRCUMI_INITIAL;
  ue_timer_const_rcvd_status = RRCUMI_UE_TIMER_CONST_NOT_RCVD;
  
  umi.umi_for_srns_relocation = FALSE;
  umi.new_crnti_valid = FALSE;
  umi.new_urnti_valid = FALSE;
  umi.new_hrnti_valid = FALSE;
  rrcscmgr_register_for_scn(RRC_PROCEDURE_UMI,
                            RRC_STATE_WILDCARD,
                            RRC_STATE_DISCONNECTED);     
} /* rrcumi_init_procedure */
/*===========================================================================
FUNCTION rrcumi_get_current_substate()

DESCRIPTION
    This function will return current UMI substate
DEPENDENCIES
  None

RETURN VALUE
  return current rrcumi_substate.
SIDE EFFECTS
none
===========================================================================*/
uint8  rrcumi_get_current_substate(void)
{
  return ((uint8)rrcumi_substate);
} /*rrcumi_get_current_state*/  
/*===========================================================================

FUNCTION rrcumi_return_substate

DESCRIPTION
Returns current umi substate

DEPENDENCIES
  None.

RETURN VALUE
  rrcumi_substate_e_type.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
rrcumi_substate_e_type rrcumi_return_substate(void)
{
  return umi.curr_substate;
}

#ifdef FEATURE_QSH_EVENT_METRIC
/*===========================================================================

FUNCTION rrcqsh_log_umi_ota_params

DESCRIPTION
Returns current umi substate

DEPENDENCIES
  None.

RETURN VALUE
  rrcumi_substate_e_type.

===========================================================================*/
void rrcqsh_log_umi_ota_params(void)
{
  ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.umi_ota_params.new_crnti_valid = umi.new_crnti_valid; 
  ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.umi_ota_params.new_urnti_valid = umi.new_urnti_valid; 
  ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.umi_ota_params.umi_for_srns_relocation = umi.umi_for_srns_relocation; 
  ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.umi_ota_params.new_hrnti_valid = umi.new_hrnti_valid; 
  ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.umi_ota_params.timer_const_rcvd = ue_timer_const_rcvd_status; 
}
#endif