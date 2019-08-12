/*===========================================================================
R R C  C E L L  C H A N G E   O R D E R   P R O C E D U R E

DESCRIPTION
    This header file contains implementation for the RRC Cell Change Order
    Procedure.



EXTERNALIZED FUNCTIONS

    rrccho_init_procedure
        This function initializes everything that the RRC Cell Change Order procedure
         needs to have initialized.

    rrccho_procedure_event_handler
        This procedure is the event handler for the RRC Cell Change Order procedure.
        The following events are handled by this procedure:


INITIALIZATION AND SEQUENCING REQUIREMENTS
    None.

Copyright (c) 2001-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                                                EDIT HISTORY FOR MODULE

    This section contains comments describing changes made to the module.
    Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrccho.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/16   sp      Made changes to convert ota metric msg_id to enum
05/09/16   ad      Made changes to log important parms from OTAs
05/09/16   sp      Made changes for Mav debug snapshot
10/16/15   as      Added code to support LTA durting activation time
01/06/14   bc      Made changes to support W2G CCO Abort mechanism.
01/20/15   bc      Changes to crash the UE if there is no response to ACTIVATION_IND 
                   from NAS during WTOG CCO.
01/07/15   sas     Fixed compiler warnings
12/05/14   sr      Made changes to send dummy service ind and move to idle SIB DB 
                   is not available for the current camped cell after WTOG CCO fail
11/27/14   sn      Changes to not release lock for G2W CCO success 
09/03/14   sp      Made changes to save procedure substates in crash debug info
08/08/14   rmn     Made changes to request lock on supported band indicated by NAS rather
                   than deriving it from Japan PLMN and freq.
08/04/14   bc      Made changes to fix compiler warnings.
07/14/14   rmn     Made changes to attempt trm_exchange for all XtoW IRAT scenarios 
06/19/14   rmn     Made changes for DR-DSDS Feature
05/14/14   bc      Made changes to ignore GTOW CCO after getting suspend confirm from
                   L1 if PS session is closed during the interval between suspend req 
                   and confirm.
05/06/14   geg     Use safer versions of memcpy() and memmove()
05/02/14   sr      Made changes to update failure cause if PS domain is closed before suspend is completed for CCO
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
02/19/14   sn      Ignore CCO if PS domain release in progress
03/21/14   sr      Fix compilation errors
01/15/14   sn      Set failure cause to configuration unaaceptable if GSM band 
                   given in CCO message is not supported 
12/18/13   bc      Made changes to not to dereference a null pointer
08/28/13   bc      Made changes to handle GTOW CCO abort and WTOG cco abort requests
08/14/13   vg      Made changes to implement a new diag event EVENT_WCDMA_RRC_FAILURE
06/25/13   sn      Block IRAT functionality in manual CSG mode
06/27/13   gv      Made changes to buffer the MM and RR commands for crash debugging
05/13/13   vi      Fixed compiler and KW errors for Bolt
04/22/13   as      Made changes to to send CCO failure if CU is active
01/03/13   ad      Made changes to reduce the redundant F3s
12/27/12   esr     Added boundary checks during memcpy to avoid buffer overflow
12/20/12   sr      Added changes to ignore CCO from UTRAN when ABORT is in procress
10/14/12   gv      Added code to support exchange of AS ID between WRRC and GRR commands
08/31/12   rd      Transfer dedicated priority parameters to/from WCDMA and GERAN
                   during IRAT procedure
12/09/11   ad      Added code to log WCDMA_RRC_PROTOCOL_ERRORS packet
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/20/11   vg      added support for mdsp halt recovery
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
09/16/10   ss      Added code to send RABM_EST_IND to upper layes in case of CCO failure.
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
04/09/10   sks     Added code to populate as_id with RRC_WTOG_CCO_COMPLETE_IND sent to NAS.
03/15/10   rm      Added code to support FEATURE_RAT_PRIORITY_LIST for W<->G IRAT
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
07/20/09   rm      Modified the call to funtion rrcrce_force_initialize
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
03/27/09   rm      CMI Featurization
12/29/08   rm      Added support for 3G<->GAN Handover
11/08/06   vr      Do not send CellChangeOrderFromUTRANFailure message to the 
                   network if the signaling connection status check fails.
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
05/11/06   vr      Removed NV GCF flag dependency
05/08/06   vr      Fixed Lint warnings
04/18/06   da      Modified code to support migration to Mar-06 Rel-5 ASN1 +
                   R6 Hacks
04/07/06   vr      Fixed bug in checking the bitmask for the presence of Activation
                   time in CellChangeOrderFromUTRAN message.
03/15/06   vg      fixed lint warnings
03/03/06   vm      Added support to send BPLMN SUSPEND REQ (if BPLMN is active)
                   to CSP when UTRAN WTOG CCO is received in CELL_DCH state (earlier
                   suspend req was sent only in CELL_FACH state).
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1
02/27/06   vr      Suspend BPLMN(if needed) before suspending L1 for WTOG CCO.
                   This would give more time for RR to cleanup BPLMN before handling
                   the Cell Change Order request from RRC
02/21/06   vr      Fill Interrat reason in change mode request
02/21/06   vr      Ignore NACC if UE is in non-DCH state
02/01/06   vr      Support for feature NACC
01/23/06   vm      BPLMN Optimization for Automatically re-starting Manual
                   BPLMN search after completion of LAU/RAU updates & added
                   support for not rejecting Manual PLMN search request when RRC state
                   is Connecting/FACH/DCH and the establishment cause is Registration.
01/04/06   vm      Added support for barring the correct GSM cell when WtoG CCO
                   fails because GSM cell is barred. Added new function
                   rrccho_fill_gsm_cell_info_for_cell_bar().
12/20/05   bd      Changed syntax for suppressing lint error 613
10/26/05   vr      Added new function defn rrccho_is_wtog_cco_active() to check
                   if WTOG CCO is active or not
10/18/05   vr      If WTOG CCO failed due to barred G cell,inform CSP
                   to start the barred cell timer
10/12/05   vr      Fixed compiler warning
10/04/05   vr      Implementation of GTOW Packet Cell Change Order
07/20/05   vr      Implementation of Spec Compliant WTOG Cell Change Order
07/15/05   vr      Use the function rr_rrc_get_arfcn_band() to see if the GSM
                   band specified in W->G CCO msg is supported or not.If not,
                   send a failure message to the network.
                   Remove references to band_pref and frequency_band fields in
                   the procedure
07/08/05   vr      Delay suspension of L1 during W->G CCO by 100 ms if GCF
                   flag is defined.Otherwise the delay time would be 60ms.
05/27/05   vm      Cell bar request is now sent with an integer Tbarred timer
                   value instead of an enum type.
05/05/05   vm/bu   Added support for T-barred Feature. If W to G CCO fails with failure
                   reason RR_GSM_CCO_CELL_BARRED then send CPHY_CELL_BAR_REQ with
                   Tbarred timer value of 320s to L1.
05/03/05   vr      In function rrccho_is_wtog_cco_in_progress(),return FALSE
                   if CHO is in wait_cell_update_cnf_substate.Otherwise we
                   would block UE going to idle and the UE would end up in
                   a hung state.
03/25/05   vr      If we get a CCOFromUTRAN command when only
                   a CS call is up,set the failure cause in the failure message
                   as configurationUnacceptable instead of protocolError.
03/04/05   kc      Added support for resumption-failure after a WCDMA on a WtoG
                   CHO failure.
08/19/04   sgk     Made the check and assigned the right enum values to fix
                   lint Error Type mismatch (assignment) (enum/enum) in
                   function rrccho_validate_and_update_info_from_cco_from_utran_msg
                   problem due to 2 enum declarations for the frequency type
                   one in rrc_r_types.h and the other in rrcasn1.h.
                   Included rrccsp.h to fix lint error Symbol
                   'rrccsp_fill_info_for_rr' undeclared,
                   assumed to return int and the error call to function
                   'rrccsp_fill_info_for_rr()' not made in the presence of a
                   prototype
                   Prototype for function rrc_csp_send_interrat_cell_change_ind
                   was included in rrcdata.h to fix lint error Symbol
                   'rrc_csp_send_interrat_cell_change_ind' undeclared, assumed to
                   return int and lint error call to function
                   'rrc_csp_send_interrat_cell_change_ind()' not made in the
                   presence of a prototype
                   Removed local structure member send_abort_ind to fix lint
                   error local structure member 'send_abort_ind' not referenced
                   Set rrccho_proc_info.cell_change_order_req.T309_value =
                   RRC_GET_T309() instead of
                   rrccho_proc_info.cell_change_order_req.T309_value =
                   RRC_GET_T315();to fix lint warning Converting
                   enum to int in function
                   rrccho_validate_and_update_info_from_cco_from_utran_msg
08/16/04   kc      Added defensive code to set cco_send_ind_to_csp to false.
08/06/04   kc      Reset CHO internal data at all times when CHO proc goes to
                   CHO_INITIAL substate.
08/05/04   kc      Added new functions rrccho_initialize_wtog_internal_data() and
                   rrccho_is_wtog_cco_in_progress(). Also added support to send
                   RRC_CSP_WCDMA_RESUMED_IND to CSP if it had queried
                   CHO earlier and found that CHO was active.
06/01/04   bu      Added support for network_select_mode and band_pref.
04/03/04   kc      Delaying of WtoG CCO by 60 ms is now done only when we go
                   to standby mode.  Also added additional IE's to cc_order_rej
03/28/04   kc      Added checks for CHO to return failure if PS Sig conn is not
                   active or if CS sig conn is active and also if OC is set
03/19/04   kc      Added support for GtoW PCCO.
02/24/04   bu      Corrected the check to see if Activation time IE is present
                   in the Cell Change Order from Utran message.
12/16/03   bu      Added GSM quadband support.
12/10/03   bu      Added handling of STATE_CHANGE_IND (to Idle) in
                   rrccho_procedure_event_handler() itself. Removed handling
                   of STATE_CHANGE_IND in other functions.
11/03/03   bu      Changed ERR_FATAL to ERR if a Cell Change Order message is
                   received in states other than FACH or DCH. Changed
                   rrccho_process_cell_fach_and_disconnected_state() to
                   rrccho_process_cell_fach_state().
08/12/03   bu      Added support for activation time if specified in the CCO
                   message. If activation time is not specified delay sending
                   SUSPEND to L1 by 60ms to give enough time for L2 ACK for CCO
                   message to be sent. This change is under
                   FEATURE_RRC_DELAY_CELL_CHANGE_ORDER.
06/20/03   bu      Added appropriate dualmode definitions to resolve
                   singlemode compilation issues.
06/03/03   rj      Added complete support for Cell Change Order from UTRAN for
                   both CELL_FACH and CELL_DCH states. When UE is unable to
                   camp-on on the GSM side, it resumes in the WCDMA side and
                   sends Cell Change Order from UTRAN Failure message with
                   suitable cause.
04/10/03   rj      Fixed a compilation error
04/10/03   rj      Fixed lint error and added more comments.
04/10/03   rj      Created file

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "rrclcm.h"            /* Logical Channel Manager */
#include "rrcsend.h"              /* For sending a signalling message */
#include "msg.h"
#include "err.h"
#include "rrcasn1.h"
#include "rrcscmgr.h"
#include "rrcdata_v.h"
#include "rrccu.h"
#include "rrccui.h"
#include "rrcmcm.h"
#include "rrctmr.h"
#include "rrcccm.h"
#include "rrcsibdb.h"
#include "mmtask.h"
#ifdef FEATURE_CMI
#include "mmtask_v.h"
#endif
#include "rrcrce.h"
#include "l1const.h"
#include "rrcsmc.h"
#include "rrcllc.h"
#include "rrcdt.h"
#include "rrccsp.h"
#include "rrccspi.h"
#include "rr_rrc_if.h"
#include "mm_umts.h"
#include "rrccho.h"
#include "rrcsibproc.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif

#ifdef FEATURE_DUAL_SIM
#include "rrcwrm.h"
#endif

#include "rrcdormancy.h"

#include "rrcasn1util.h"
#include "rrclogging.h"

#ifdef FEATURE_WCDMA_DEDICATED_PRI_INFO
#include "rrcmeas.h"
#endif

#include "stringl.h"

#ifdef FEATURE_QSH_EVENT_METRIC
extern rrc_qsh_ul_failure_ota_param_s failure_ota;
extern boolean rrc_qsh_dl_committed;
#endif
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* Enumerated type of the different substates for this procedure           */
typedef enum
{
  RRCCHO_INITIAL,                             /* Initial Substate                    */
  RRCCHO_WAIT_STANDBY_CNF,                    /* Waiting for mode change comfirmation
                                                 from MCM (to Standby)               */
  RRCCHO_WAIT_CELL_CHANGE_ORDER_CNF,
  /* Waiting for Cell Change Order
     comfirmation from MM       */

  RRCCHO_WAIT_CELL_CHANGE_ORDER_ABORT_CNF,
  /* Waiting for Cell change order
     abort confirmation from RR */
  RRCCHO_WAIT_ACTIVE_CNF,                     /* Waiting for mode change comfirmation
                                                 from MCM (to Active)                */
  RRCCHO_WAIT_CHANNEL_CONFIG_CNF,             /* Waiting for revert back to UTRAN
                                                 configuration comfirmation from LLC */
  RRCCHO_CELL_UPDATE_COMPLETE_CNF,
  /* Waiting for cell update comfirmation
    from CU                         */
  RRCCHO_WAIT_IRAT_IN_PROGRESS_CNF,           /* Waiting for Mode change confirm from MCM */
  RRCCHO_WAIT_CAMP_CNF,                       /* Waiting for CAMP CNF from CSP */
  RRCCHO_WAIT_ACTIVATION_RSP,                 /* Waiting for ACTIVATION_RSP from MM */
  RRCCHO_WAIT_CONN_SETUP_IND,                 /* Waiting for CONN IND from RCE */
  RRCCHO_WAIT_INACTIVE_CNF                    /* Waiting for INACTIVE_CNF from MCM */
} rrccho_state_e_type;
/* Below #defines are added for logging purpose only */
#define rrccho_state_e_type_value0 RRCCHO_INITIAL  
#define rrccho_state_e_type_value1 RRCCHO_WAIT_STANDBY_CNF		  
#define rrccho_state_e_type_value2 RRCCHO_WAIT_CELL_CHANGE_ORDER_CNF
#define rrccho_state_e_type_value3 RRCCHO_WAIT_CELL_CHANGE_ORDER_ABORT_CNF
#define rrccho_state_e_type_value4 RRCCHO_WAIT_ACTIVE_CNF
#define rrccho_state_e_type_value5 RRCCHO_WAIT_CHANNEL_CONFIG_CNF
#define rrccho_state_e_type_value6 RRCCHO_CELL_UPDATE_COMPLETE_CNF					 
#define rrccho_state_e_type_value7 RRCCHO_WAIT_IRAT_IN_PROGRESS_CNF
#define rrccho_state_e_type_value8 RRCCHO_WAIT_CAMP_CNF
#define rrccho_state_e_type_value9 RRCCHO_WAIT_ACTIVATION_RSP
#define rrccho_state_e_type_value10 RRCCHO_WAIT_CONN_SETUP_IND
#define rrccho_state_e_type_value11 RRCCHO_WAIT_INACTIVE_CNF 

/* Cause for Activation Indication */
typedef enum
{
  RRCCHO_ACT_CAUSE_NONE,
  RRCCHO_INTERRAT_CC_ORDER_GTOW,
  RRCCHO_INTERRAT_CC_ORDER_WTOG_FAILED
} rrccho_activation_cause_e_type;

/* Procedure status */
typedef struct
{
  rrc_RRC_TransactionIdentifier transaction_id;
  rr_interrat_cc_order_req_type cell_change_order_req;
  rr_interrat_cc_order_abort_req_type cell_change_abort_req;
  boolean act_time_present;
  uint32 activation_time;
  uint32 failure_cause;
  rrc_ProtocolErrorCause protocol_error;
  boolean cco_in_progress;                //flag to keep track whether wtog is active
  boolean cco_send_ind_to_csp;            //flag to remember to send csp ind
} rrccho_procedure_info_type;

typedef struct
{
  boolean                           cc_order_in_progress;   /* GTOW CC order in progress */
  rrc_scr_code_type                 scr_code;               /* Scrambling code */
  rrc_freq_type                     freq;                   /* UARFCN */
  rrc_plmn_identity_type            requested_plmn;         /* PLMN */
  sys_network_selection_mode_e_type network_select_mode;    /* Network Select Mode*/
  uint32                            t3174;                  /* PCCO timer */

  rr_interrat_cc_order_status_e_type status;                /* Status of PCCO */
  rrc_interrat_reject_reason_T failure_reason;              /* Failure cause */

} rrccho_gtow_info_type;

/* Declare a local object of type gtow_info*/
LOCAL rrccho_gtow_info_type gtow_cho_info;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

extern boolean rrcrce_send_abort_cnf;


/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

#ifdef FEATURE_RRC_DELAY_CELL_CHANGE_ORDER
/* We need to introduce a delay before suspending the WCDMA side and moving
 * to GSM
 */
#define RRCCHO_TEMP_TIMER_SIG 0x4002
rex_sigs_type rrccho_sig;
rex_timer_type rrccho_temp_timer;
#endif /* FEATURE_RRC_DELAY_CELL_CHANGE_ORDER */

#define RRCTMR_CHO_ACT_RSP_EF_IN_MS 2000

/* RRCCHO Substate */
rrccho_state_e_type rrccho_substate = RRCCHO_INITIAL;

/* Procedure  statue */
rrccho_procedure_info_type rrccho_proc_info;

/* Field to remember the Activation cause */
rrccho_activation_cause_e_type rrccho_act_cause = RRCCHO_ACT_CAUSE_NONE;

boolean ps_clsd_after_suspend_req_in_cco = FALSE;

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*===========================================================================

FUNCTION   rrccho_initialize_internal_data

DESCRIPTION

  Initizlize internal data for WtoG and GtoW CCO

DEPENDENCIES

  None.

RETURN VALUE

  None

SIDE EFFECTS

  None.

===========================================================================*/
static void rrccho_initialize_internal_data
(
  void
)
{
  //set cho state to initial
  rrccho_substate = RRCCHO_INITIAL;

  //reset wtog cco flags
  rrccho_proc_info.cco_in_progress = FALSE;
  rrccho_proc_info.cco_send_ind_to_csp = FALSE;
  rrccho_proc_info.act_time_present = FALSE;

  //reset gtow cco flags
  gtow_cho_info.cc_order_in_progress = FALSE;
  gtow_cho_info.failure_reason = RRC_WCDMA_NONE;

  //reset GERAN sysinfo
  rrccho_proc_info.cell_change_order_req.geran_sysinfo_choice = GERAN_INFO_TYPE_NONE;
}

#ifdef FEATURE_WCDMA_TABASCO_MODEM
/*===========================================================================

FUNCTION   rrccho_send_wl1_stop_ind

DESCRIPTION

  This function sends the RR_INTERRAT_WL1_STOP_IND to RR

DEPENDENCIES

  None.

RETURN VALUE

  None

SIDE EFFECTS

  None.

===========================================================================*/

static void rrccho_send_wl1_stop_ind(void)
{
  rr_interrat_wl1_stop_ind_type rrc_stop_ind;

  rrc_stop_ind.cause = RR_WL1_STOP_IND_W2G_CCO;
  
  rr_send_rrc_message_to_rr_internal((void *)(&(rrc_stop_ind)), RR_INTERRAT_WL1_STOP_IND,
                              sizeof(rr_interrat_wl1_stop_ind_type)); 

}
#endif
/*===========================================================================

FUNCTION   rrccho_send_csp_wcdma_resume_ind

DESCRIPTION

  Send resume-ind to CSP, if CSP queried CHO was active.
  CSP queries this only when CU asks CSP to move to Disconnected substate.

DEPENDENCIES

  None.

RETURN VALUE

  None

SIDE EFFECTS

  None.

===========================================================================*/
static void rrccho_send_csp_wcdma_resume_ind
(
  void
)
{
  rrc_cmd_type *cmd_ptr;   //pointer to send internal command to csp

  if(rrccho_proc_info.cco_send_ind_to_csp == TRUE)
  {

    /* Get a RRC internal command buffer */
    cmd_ptr = rrc_get_int_cmd_buf();

    cmd_ptr->cmd_hdr.cmd_id = RRC_CSP_WCDMA_RESUMED_IND;

    /* Put the command on the internal queue */
    rrc_put_int_cmd( cmd_ptr );
    WRRC_MSG0_HIGH("RRC_CSP_WCDMA_RESUMED_IND sent to CSP");
  }
}

/*===========================================================================

FUNCTION   RRCCHO_SEND_CHANGE_MODE_REQ

DESCRIPTION

  This function sends a RRC_CHANGE_MODE_REQ to MCM to request to change the
  RRC mode to Standby.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrccho_send_change_mode_req( rrc_mode_e_type mode )
{
  rrc_cmd_type *cmd_ptr;

  /* Get a RRC internal command buffer */
  cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command id  */
  cmd_ptr->cmd_hdr.cmd_id = RRC_CHANGE_MODE_REQ;

  /* Fill in procedure id    */
  cmd_ptr->cmd.change_mode_req.procedure = RRC_PROCEDURE_CHO;

  /* Fill in the requested mode */
  cmd_ptr->cmd.change_mode_req.mode = mode;

  /* Fill reason for change mode req */
  cmd_ptr->cmd.change_mode_req.reason = INTERRAT_CCO;

  /* Put the command on the internal queue */
  rrc_put_int_cmd( cmd_ptr );
  WRRC_MSG1_HIGH("RRC_CHANGE_MODE_REQ sent to MCM,requested mode:%d",mode);

  return;
} /* rrccho_send_change_mode_req */

/*===========================================================================

FUNCTION   RRCCHO_IS_WTOG_CCO_ACTIVE

DESCRIPTION

  This function returns true/false based on whether WtoG PCCO is active
  or not.

DEPENDENCIES

  None.

RETURN VALUE

  TRUE/FALSE depending on whether WtoG CellChangeOrder was in active or not

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrccho_is_wtog_cco_active
(
  void
)
{
  if(rrccho_proc_info.cco_in_progress == TRUE)
  {
    return TRUE;
  }
  return FALSE;
}
/*===========================================================================

FUNCTION   rrccho_is_wtog_cco_in_progress

DESCRIPTION

  This function returns true/false based on whether WtoG CCO is in progress
  or not.

DEPENDENCIES

  None.

RETURN VALUE

  TRUE/FALSE depending on whether WtoG CellChangeOrder was in progress or not.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrccho_is_wtog_cco_in_progress
(
  void
)
{
  /* If we are waiting for a CU cnf,GSM side is already done.
     So we can let the UE go to idle  */
  if( rrccho_substate == RRCCHO_CELL_UPDATE_COMPLETE_CNF )
  {
    rrccho_proc_info.cco_send_ind_to_csp = FALSE;
    return FALSE;
  }
  else
  {
    if(rrccho_proc_info.cco_in_progress == TRUE)
    {
      rrccho_proc_info.cco_send_ind_to_csp = TRUE;
      return TRUE;
    }
    else
    {
      rrccho_proc_info.cco_send_ind_to_csp = FALSE;
      return FALSE;
    }
  }
}

/*===========================================================================

FUNCTION   RRCCHO_SEND_PCCO_CNF

DESCRIPTION

  This function sends the RRC_INTERRAT_CC_ORDER_CNF to RR with appropriate
  status(Success/Failure) and failure cause

DEPENDENCIES

  None.

RETURN VALUE

  None

SIDE EFFECTS

  None.

===========================================================================*/

static void rrccho_send_pcco_cnf(void)
{
  /* Cnf command */
  rrc_interrat_cc_order_cnf_type cc_order_cnf;

  /* Populate the command with status cause and failure reason(set to NONE in case of Success) */
  cc_order_cnf.status = gtow_cho_info.status;
  cc_order_cnf.failure_reason = gtow_cho_info.failure_reason;

  /* Send the RRC_INTERRAT_CC_ORDER_CNF message to RR */
  rr_send_rrc_message_to_rr_internal((void *)(&(cc_order_cnf)), RRC_INTERRAT_CC_ORDER_CNF,
                            sizeof(rrc_interrat_cc_order_cnf_type));

  WRRC_MSG2_HIGH("RRC_INTERRAT_CC_ORDER_CNF sent to RR.Status:%d,Fail_reason:%d",
                          cc_order_cnf.status,cc_order_cnf.failure_reason);

#ifdef FEATURE_DUAL_SIM
  /*Do not release lock for success case as RCE is in progress*/
  if (gtow_cho_info.status != CCO_SUCCESS) 
  {
    rrctmr_stop_timer(RRCTMR_WRM_LOCK_TIMER);
    rrc_release_lock();
  }
#endif
}

/*===========================================================================

FUNCTION rrccho_process_cell_change_order_cnf

DESCRIPTION
    This function process the Cell Chnage Order Cnf received from the GSM

DEPENDENCIES
    None.

RETURN VALUE
    None

===========================================================================*/

static void rrccho_process_cell_change_order_cnf
(
  rr_interrat_cc_order_cnf_type cc_order_cnf  /* Cell Change Order Cnf */
)
{
  mm_cmd_type *mm_cmd_ptr = NULL;
  switch(cc_order_cnf.status)
  {
    case CCO_SUCCESS:
#ifdef FEATURE_WCDMA_TABASCO_MODEM
    case CCO_ABORTED_DUE_TO_HIGH_PRI_REQ:
#endif

      WRRC_MSG0_HIGH("Received CCO_SUCCESS from RR");

    /* Send CCO Completion Ind to MM */
      mm_cmd_ptr = mm_rrc_get_cmd_buf(RRC_WTOG_CCO_COMPLETE_IND);

#ifdef FEATURE_DUAL_SIM
      mm_cmd_ptr->cmd.rrc_wtog_cco_complete_ind.as_id = rrc_get_as_id();
#endif

      rrc_send_rrc_message_to_mm(mm_cmd_ptr);
      WRRC_MSG0_HIGH("Dispatching out MM cmd RRC_WTOG_CCO_COMPLETE_IND sent to MM");

    /* Change mode to Inactive */
      rrccho_send_change_mode_req(RRC_MODE_INACTIVE);
      rrccho_substate = RRCCHO_WAIT_INACTIVE_CNF;
      
      /* Abort Dormancy if CCO proceed to successful processing. */
      rrcdormancy_update_dormancy_for_srns_hho_cco(RRC_PROCEDURE_CHO);
      break;

    case CCO_FAILURE_BEFORE_ACT_IND:

      WRRC_MSG0_HIGH("Received CCO_FAILURE_BEFORE_ACT_IND from RR");

      /* If the failure was due to barred cell,inform CSP to start the barred cell timer */
      if(cc_order_cnf.failure_reason == RR_GSM_CCO_CELL_BARRED)
      {
        rrccsp_send_cphy_cell_bar_req(RRC_CSP_GSM_CELL_BARRED, 320);
      }

      rrccho_proc_info.failure_cause = T_rrc_InterRAT_ChangeFailureCause_physicalChannelFailure;
      rrccho_send_change_mode_req(RRC_MODE_ACTIVE);
      rrccho_substate = RRCCHO_WAIT_ACTIVE_CNF;
      break;

    case CCO_FAILURE_AFTER_ACT_IND:

      WRRC_MSG0_HIGH("Received CCO_FAILURE_AFTER_ACT_IND from RR");
      rrccho_proc_info.failure_cause = T_rrc_InterRAT_ChangeFailureCause_physicalChannelFailure;

      /* Send activation ind to nas*/
      mm_cmd_ptr = mm_rrc_get_cmd_buf(RRC_ACTIVATION_IND);
      mm_cmd_ptr->cmd.rrc_activation_ind.status = MM_AS_WTOG_CC_ORDER_FAILED;

#ifdef FEATURE_DUAL_SIM
      mm_cmd_ptr->cmd.rrc_activation_ind.as_id = rrc_get_as_id();
#endif

      /* Put the command on MM queue */
      rrc_send_rrc_message_to_mm(mm_cmd_ptr);
      rrctmr_start_timer(RRCTMR_ACT_RSP_EF_TIMER,RRCTMR_CHO_ACT_RSP_EF_IN_MS);
      WRRC_MSG0_HIGH("Dispatching out MM cmd RRC_ACTIVATION_IND with status MM_AS_WTOG_CC_ORDER_FAILED sent to MM");

      /*update the cause for activation indication*/
      rrccho_act_cause = RRCCHO_INTERRAT_CC_ORDER_WTOG_FAILED;
      rrcmcm_update_wcdma_activation_cause(RRCMCM_INTERRAT_CC_ORDER_WTOG_FAILED);
      rrccho_substate = RRCCHO_WAIT_ACTIVATION_RSP;
      break;

    default:
      WRRC_MSG1_ERROR("Invalid status in cc_order_cnf command from RR:%d",cc_order_cnf.status);
      break;
  }
}

/*===========================================================================

FUNCTION rrccho_process_cell_change_order_reject

DESCRIPTION
    This function process the Cell Chnage Order Reject received from the GSM
    and updates the failure cause

DEPENDENCIES
    None.

RETURN VALUE
    FALSE: if validation fails. Other wise TRUE.

===========================================================================*/

static void rrccho_process_cell_change_order_reject
(
  rr_interrat_cc_order_rej_type reject    /* Cell Change Order Reject*/
)
{

    switch(reject.failure_reason)
    {
      case RR_GSM_CCO_OTHER_REASON:
        rrccho_proc_info.failure_cause =
          T_rrc_InterRAT_ChangeFailureCause_unspecified;
        break;

      case RR_GSM_CCO_CELL_BARRED:
        rrccsp_send_cphy_cell_bar_req(RRC_CSP_GSM_CELL_BARRED, 320);
        rrccho_proc_info.failure_cause =
          T_rrc_InterRAT_ChangeFailureCause_configurationUnacceptable;
        break;

      case RR_GSM_CCO_CELL_FORBIDDEN:
        rrccho_proc_info.failure_cause =
          T_rrc_InterRAT_ChangeFailureCause_configurationUnacceptable;
        break;

      case RR_GSM_CCO_BAND_NOT_SUPPORTED:
      case RR_GSM_CCO_BSIC_NOT_FOUND:
      case RR_GSM_CCO_BCCH_NOT_FOUND:
      case RR_GSM_CCO_BCCH_DECODE_FAIL:
        rrccho_proc_info.failure_cause =
          T_rrc_InterRAT_ChangeFailureCause_physicalChannelFailure;
        break;

      case RR_GSM_CCO_T309_EXPIRED:
        rrccho_proc_info.failure_cause =
          T_rrc_InterRAT_ChangeFailureCause_unspecified;
        break;

      case RR_GSM_CCO_CELL_LOW_C1:
        rrccho_proc_info.failure_cause =
          T_rrc_InterRAT_ChangeFailureCause_physicalChannelFailure;
        break;

      case RR_GSM_CCO_INVALID_STATE:
        rrccho_proc_info.failure_cause =
          T_rrc_InterRAT_ChangeFailureCause_unspecified;
        break;

      default:
        ERR_FATAL("Invalid failure cause",0,0,0);
        break;
    }

}/* rrccho_process_cell_change_order_reject */
/*===========================================================================

FUNCTION rrccho_validate_and_update_info_from_cco_from_utran_msg

DESCRIPTION
    This function validates the Cell Chnage Order from UTRAN message and
    updates suitable failure cause. Also updates Cell Change Order Request
    information from the message.

DEPENDENCIES
    None.

RETURN VALUE
    FALSE: if validation fails. Other wise TRUE.

===========================================================================*/

static boolean rrccho_validate_and_update_info_from_cco_from_utran_msg
(
  rrc_CellChangeOrderFromUTRAN *msg_ptr    /* Pointer to Cell Change Order */
)
{
  rrc_rr_frequence_band_e_type   frequency_band;
  sys_band_T gsm_band;

#ifdef FEATURE_FEMTO_CSG
  if(rrc_csp_int_data.network_select_mode == SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)
  {
     /* set transaction id */
    rrccho_proc_info.transaction_id =
      msg_ptr->u.r3.cellChangeOrderFromUTRAN_IEs.rrc_TransactionIdentifier;
    rrccho_proc_info.failure_cause =
        T_rrc_InterRAT_ChangeFailureCause_unspecified;
    rrccho_proc_info.protocol_error = rrc_ProtocolErrorCause_messageNotCompatibleWithReceiverState;

    WRRC_MSG0_ERROR("CSG: Rejecting CCO in manual CSG mode");
    return(FALSE);   
  }
#endif

  //if ordered-config is already set for any other transition, we should not trigger
  //a CCO to G as we may in the middle doing other important configurations
  //Return an error with
  if ((rrcllc_get_ordered_config_status() != OC_NOT_SET) 
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
       || (TRUE == rrcdata_check_if_ota_is_buffered())
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
      )
  {
    /* set transaction id */
    rrccho_proc_info.transaction_id =
      msg_ptr->u.r3.cellChangeOrderFromUTRAN_IEs.rrc_TransactionIdentifier;
    rrccho_proc_info.failure_cause =
        T_rrc_InterRAT_ChangeFailureCause_protocolError;
    rrccho_proc_info.protocol_error = rrc_ProtocolErrorCause_messageNotCompatibleWithReceiverState;

    WRRC_MSG0_ERROR("Simultaneous Reconfig.");
    return(FALSE);
  }

  if(msg_ptr->t == T_rrc_CellChangeOrderFromUTRAN_later_than_r3)

  {
    rrccho_proc_info.transaction_id = 0;
    rrccho_proc_info.failure_cause =
    T_rrc_InterRAT_ChangeFailureCause_protocolError;
    rrccho_proc_info.protocol_error = rrc_ProtocolErrorCause_messageExtensionNotComprehended;
    WRRC_MSG0_ERROR("Critical extensions are not supported");
    return(FALSE);
  }

  if(msg_ptr->u.r3.cellChangeOrderFromUTRAN_IEs.
       interRAT_TargetCellDescription.technologySpecificInfo.t !=
       T_rrc_InterRAT_TargetCellDescription_technologySpecificInfo_gsm)
  {
    /* set transaction id */
    rrccho_proc_info.transaction_id =
      msg_ptr->u.r3.cellChangeOrderFromUTRAN_IEs.rrc_TransactionIdentifier;
    rrccho_proc_info.failure_cause =
        T_rrc_InterRAT_ChangeFailureCause_unspecified;
    WRRC_MSG0_ERROR("Unsupported RAT is rcvd");
    return(FALSE);
  }

  if(RRC_MSG_COMMON_BITMASK_IE_TYPE2(msg_ptr->u.r3.cellChangeOrderFromUTRAN_IEs,
       rab_InformationList))
  {
    WRRC_MSG0_ERROR("RAB Info list is ignored");

  }

  /* set transaction id */
  rrccho_proc_info.transaction_id =
      msg_ptr->u.r3.cellChangeOrderFromUTRAN_IEs.rrc_TransactionIdentifier;

  /* Extract the ARFCN and Freq band */
  rrccho_proc_info.cell_change_order_req.BCCH_ARFCN.num = (uint16)
    msg_ptr->u.r3.cellChangeOrderFromUTRAN_IEs.interRAT_TargetCellDescription.
    technologySpecificInfo.u.gsm->bcch_ARFCN;

  if(msg_ptr->u.r3.cellChangeOrderFromUTRAN_IEs.interRAT_TargetCellDescription.
      technologySpecificInfo.u.gsm->frequency_band == rrc_Frequency_Band_dcs1800BandUsed)
  {
    frequency_band = RRC_RR_DCS_1800;
  }
  else
  {
    frequency_band = RRC_RR_PCS_1900;
  }

  /* Check to see if this band is supported */
  if( (gsm_band = rr_rrc_get_arfcn_band_internal(rrccho_proc_info.cell_change_order_req.BCCH_ARFCN.num,
                                        frequency_band)) == INVALID_BAND)
  {
    /* set transaction id */
    rrccho_proc_info.transaction_id = msg_ptr->u.r3.cellChangeOrderFromUTRAN_IEs.rrc_TransactionIdentifier;
    rrccho_proc_info.failure_cause = T_rrc_InterRAT_ChangeFailureCause_configurationUnacceptable;
    WRRC_MSG0_ERROR("Unsupported GSM band in CellChangeOrderFromUTRAN Command");
    return(FALSE);
  }

  /* Fill in the band value */
  rrccho_proc_info.cell_change_order_req.BCCH_ARFCN.band = gsm_band;

  rrccho_proc_info.cell_change_order_req.BSIC_BCC = (uint8)
      msg_ptr->u.r3.cellChangeOrderFromUTRAN_IEs.
      interRAT_TargetCellDescription.technologySpecificInfo.u.gsm->bsic.bcc;

  rrccho_proc_info.cell_change_order_req.BSIC_NCC = (uint8)
      msg_ptr->u.r3.cellChangeOrderFromUTRAN_IEs.
      interRAT_TargetCellDescription.technologySpecificInfo.u.gsm->bsic.ncc;

  if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(msg_ptr->u.r3.cellChangeOrderFromUTRAN_IEs.
       interRAT_TargetCellDescription.technologySpecificInfo.u.gsm,
       ncMode))
  {
    rrccho_proc_info.cell_change_order_req.NC_Mode_present = TRUE;

    /*No Need to check for length of NC mode since it is fixed */
    switch(*(msg_ptr->u.r3.cellChangeOrderFromUTRAN_IEs.
        interRAT_TargetCellDescription.technologySpecificInfo.u.gsm->ncMode.data))
    {
      case 0:
        rrccho_proc_info.cell_change_order_req.NC_Mode = NC_MODE_0;
        break;
      case 4:
        rrccho_proc_info.cell_change_order_req.NC_Mode = NC_MODE_1;
        break;
      case 2:
        rrccho_proc_info.cell_change_order_req.NC_Mode = NC_MODE_2;
        break;
      default:
        WRRC_MSG0_ERROR("Invalid NC Mode");
        break;
    }
  }
  else
  {
    rrccho_proc_info.cell_change_order_req.NC_Mode_present = FALSE;
  }

  rrccho_proc_info.cell_change_order_req.T309_value = RRC_GET_T309();

  if((rrc_get_state() == RRC_STATE_CELL_DCH) &&
     (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.cellChangeOrderFromUTRAN_IEs,
        rrc_CellChangeOrderFromUTRAN_r3_IEs,activationTime))
     )
  {
    /* Set the activation time if present in CELL_DCH state */
    rrccho_proc_info.act_time_present = TRUE;
    rrccho_proc_info.activation_time =
    msg_ptr->u.r3.cellChangeOrderFromUTRAN_IEs.activationTime;
  }
  else
  {
    /* Activation time is not present. For CELL_FACH it should not be set by UTRAN */
    rrccho_proc_info.act_time_present = FALSE;
  }

  if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
  {
    /* Check to see of non critical extensions are present */
    if(msg_ptr->u.r3.m.laterNonCriticalExtensionsPresent)
    {
      

      if(msg_ptr->u.r3.laterNonCriticalExtensions.m.v590NonCriticalExtensionsPresent)
      {
        /* Have a local type to avoid long indirections */
        rrc_CellChangeOrderFromUTRAN_v590ext_IEs* cco_v590ext_ie;

        

        cco_v590ext_ie = &(msg_ptr->u.r3.laterNonCriticalExtensions.v590NonCriticalExtensions.
                       cellChangeOrderFromUTRAN_v590ext);

        /* Check to see if NACC info is present */
        if(RRC_MSG_COMMON_BITMASK_IE_PTR(cco_v590ext_ie,
            rrc_CellChangeOrderFromUTRAN_v590ext_IEs,geran_SystemInfoType))
        {
          rrc_GERAN_SystemInformation * local_geran_SI;
          uint8 block_count = 0;

          if(RRC_STATE_CELL_DCH == rrc_get_state())
          {
            
            if(cco_v590ext_ie->geran_SystemInfoType.t == T_rrc_CellChangeOrderFromUTRAN_v590ext_IEs_geran_SystemInfoType_sI)
            {
              

              /* Assign the SI to the local ptr so that while looping we dont tamper the links in the list */
              local_geran_SI = &cco_v590ext_ie->geran_SystemInfoType.u.sI;

              /* Set the SI type in the CCO req ommand */
              rrccho_proc_info.cell_change_order_req.geran_sysinfo_choice = GERAN_INFO_TYPE_SI;
            }
            else
            {
              

              /* Assign the SI to the local ptr so that while looping we dont tamper the links in the list */
              local_geran_SI = &cco_v590ext_ie->geran_SystemInfoType.u.pSI;

              /* Set the SI type in the CCO req ommand */
              rrccho_proc_info.cell_change_order_req.geran_sysinfo_choice = GERAN_INFO_TYPE_PSI;
            }

            /* Extract the SI blocks and populate the cco_req */
            while(local_geran_SI->n > block_count)
            {
              WCDMA_MEMCPY(rrccho_proc_info.cell_change_order_req.geran_SI[block_count].msg_string,
                           sizeof(rrccho_proc_info.cell_change_order_req.geran_SI[block_count].msg_string),
                     local_geran_SI->elem[block_count].data,
                     MIN(local_geran_SI->elem[block_count].numocts,RRC_RR_MAX_SI_BLOCK_LENGTH));

              rrccho_proc_info.cell_change_order_req.geran_SI[block_count].msg_length =
                local_geran_SI->elem[block_count].numocts;

              block_count++;
            }

            /* Populate the number of blocks */
            rrccho_proc_info.cell_change_order_req.num_blocks = block_count;

            WRRC_MSG2_HIGH("Geran Info populated.Type %d, num_blocks %d",
                      rrccho_proc_info.cell_change_order_req.geran_sysinfo_choice,
                      rrccho_proc_info.cell_change_order_req.num_blocks);
          }
          else
          {
            WRRC_MSG0_HIGH("Ignoring NACC since UE is in non-DCH state");
          }
        }
      }
    }
    else
    {
        WRRC_MSG0_HIGH("r3 later non critical extensions not present");
    }
  }
  return(TRUE);
}/* rrccho_validate_and_update_info_from_cco_from_utran_msg */
/*===========================================================================

FUNCTION   RRCCHO_SEND_CELL_CHANGE_ORDER_REQ

DESCRIPTION

  This function sends a RR_INTERRAT_CC_ORDER_REQ to GSM RR to initiate the
  Cell Change Order from UTRAN.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrccho_send_cell_change_order_req( void )
{

  rrccsp_fill_info_for_rr( &(rrccho_proc_info.cell_change_order_req.network_select_mode));

#ifdef FEATURE_RAT_PRIORITY_LIST
  rrc_csp_get_rat_pri_list_info(&rrccho_proc_info.cell_change_order_req.rat_pri_list_info);
#endif

#ifdef FEATURE_WCDMA_DEDICATED_PRI_INFO
/*copying the dedicated priority info from global structure(rrc_dedicated_priority_info) to rr_interrat_cc_order_req_type*/
  memset(&rrccho_proc_info.cell_change_order_req.dedicated_priority_info,0,sizeof(lte_irat_dedicated_priority_info_s));
  rrcmeas_populate_dedicated_priority_info_for_irat_req(&rrccho_proc_info.cell_change_order_req.dedicated_priority_info);
#endif
  /* Send the RR_INTERRAT_CC_ORDER_REQ message to RR */
  rr_send_rrc_message_to_rr_internal((void *)(&(rrccho_proc_info.cell_change_order_req)),
                             RR_INTERRAT_CC_ORDER_REQ,
                            sizeof(rr_interrat_cc_order_req_type));

  WRRC_MSG0_HIGH("RR_INTERRAT_CC_ORDER_REQ sent to GSM RR");

  return;

} /* rrccho_send_cell_change_order_req */

/*===========================================================================

FUNCTION   RRCCHO_SEND_CELL_CHANGE_ORDER_ABORT_REQ

DESCRIPTION

  This function sends a RR_INTERRAT_CC_ORDER_ABORT_REQ to GSM RR to abort the
  Cell Change to GSM.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrccho_send_cell_change_order_abort_req( void )
{
  rr_send_rrc_message_to_rr_internal((void *)(&rrccho_proc_info.cell_change_abort_req),
                                               RR_INTERRAT_CC_ORDER_ABORT_REQ,
                                               sizeof(rr_interrat_cc_order_abort_req_type));
	
  WRRC_MSG0_HIGH("RR_INTERRAT_CC_ORDER_ABORT_REQ sent to GSM RR");
  return;
}

/*===========================================================================

FUNCTION   rrccho_send_channel_config_req_to_llc

DESCRIPTION

  This function sends a RRC_CHANNEL_CONFIG_REQ to the other RRC LLC to revert
  physical channels to previous UTRAN configuration in WCDMA L1.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrccho_send_channel_config_req_to_llc(void)
{
  rrc_cmd_type *cmd_ptr;

  /* Get a RRC internal command buffer */
  cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command id  */
  cmd_ptr->cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;

  /* Fill in procedure id */
  cmd_ptr->cmd.chan_config_req.procedure = RRC_PROCEDURE_CHO;

  /* Indicate that state change is not required */
  cmd_ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;

  /* Indicate that no confirmation is required from LLC */
  cmd_ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = TRUE;

  /* Put the command on the internal queue */
  rrc_put_int_cmd( cmd_ptr );
  WRRC_MSG0_HIGH("RRC_CHANNEL_CONFIG_REQ sent to LLC");

  return;
} /* rrccho_send_channel_config_req_to_llc */
/*==========================================================================

FUNCTION rrcho_map_inter_rat_cause_to_diag_cause

DESCRIPTION
    This function maps interrat failure causes to failure cause to be reported to diag.

DEPENDENCIES
    None.

RETURN VALUE
    Failure cause to Diag.

===========================================================================*/

uint8 rrcho_map_inter_rat_cause_to_diag_cause
(
  uint8 inter_rat_failure_cause
)
{
  switch (inter_rat_failure_cause)
  {
    case T_rrc_InterRAT_ChangeFailureCause_configurationUnacceptable:
      return(RRCLOG_InterRatfailureCause_configurationUnacceptable);

    case T_rrc_InterRAT_ChangeFailureCause_physicalChannelFailure:
      return(RRCLOG_InterRatfailureCause_physicalChannelFailure);

    case T_rrc_InterRAT_ChangeFailureCause_protocolError:
      return(RRCLOG_InterRatfailureCause_protocolError );

    case T_rrc_InterRAT_ChangeFailureCause_unspecified:
      return(RRCLOG_InterRatfailureCause_unspecified );

    default:
      WRRC_MSG2_HIGH("Invalid failure_cause: %d mapped to: %d",
                inter_rat_failure_cause, T_rrc_FailureCauseWithProtErr_configurationUnsupported);
      return(RRCLOG_InterRatfailureCause_unspecified);
  }
  
}
/*===========================================================================

FUNCTION rrccho_send_cell_change_order_failure_msg

DESCRIPTION
    This function builds the Cell Change Order from UTRAN Failure and
    sends the same in the uplink.

DEPENDENCIES
    None.

RETURN VALUE
    None

===========================================================================*/

static void rrccho_send_cell_change_order_failure_msg
(
  void
)
{

  rlc_lc_id_type lc_id;                    /* Local variable to store
                                              RLC logical channel Id */
  rrc_UL_DCCH_Message *msg = NULL;         /* Pointer to UL message */

  lc_id = rrclcm_check_ul_rlc_lc_id(
            UE_LOGCHAN_DCCH,              /* Uplink Logical Channel Type. */
            DCCH_AM_RADIO_BEARER_ID,      /* Radio Bearer Id assoicated with
                                            the above Downlink Logical
                                            Channel type. */
            UE_MODE_ACKNOWLEDGED_DATA);   /* Mode of transmission for the RLC
                                            service entity. */
  if(RRCLCM_RLC_LC_ID_NOT_FOUND == lc_id)
  {
    ERR_FATAL("Can't find RLC ID for UL RB 2",0,0,0);
  }
  /* Allocate a buffer for an uplink DCCH message. This buffer is released
   * by rrcsend_send_ul_sdu().
   */
  msg = (rrc_UL_DCCH_Message *)
        rrc_malloc(sizeof (struct rrc_UL_DCCH_Message));

  msg->message.t = T_rrc_UL_DCCH_MessageType_cellChangeOrderFromUTRANFailure;

  msg->message.u.cellChangeOrderFromUTRANFailure.t =
    T_rrc_CellChangeOrderFromUTRANFailure_r3;

  /* Set the bit mask not to include non-critical extensions */
  RRC_RESET_MSG_IE_PRESENT(msg->message.u.cellChangeOrderFromUTRANFailure.u.r3);

  /* Set the bit mask not to include Dummy IE */
  RRC_RESET_MSG_IE_PRESENT(msg->message.u.cellChangeOrderFromUTRANFailure.u.r3.
      cellChangeOrderFromUTRANFailure_r3);

  /* set transaction id */
  msg->message.u.cellChangeOrderFromUTRANFailure.u.r3.
      cellChangeOrderFromUTRANFailure_r3.rrc_TransactionIdentifier =
      rrccho_proc_info.transaction_id;

  /* Set the Failure cause */
  msg->message.u.cellChangeOrderFromUTRANFailure.u.r3.
      cellChangeOrderFromUTRANFailure_r3.interRAT_ChangeFailureCause.t =
      rrccho_proc_info.failure_cause;

  if(rrccho_proc_info.failure_cause ==
       T_rrc_InterRAT_ChangeFailureCause_protocolError)
  {
     msg->message.u.cellChangeOrderFromUTRANFailure.u.r3.
      cellChangeOrderFromUTRANFailure_r3.interRAT_ChangeFailureCause.u.protocolError =
        rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation);

    msg->message.u.cellChangeOrderFromUTRANFailure.u.r3.
      cellChangeOrderFromUTRANFailure_r3.interRAT_ChangeFailureCause.u.
      protocolError->diagnosticsType.t =
      T_rrc_ProtocolErrorInformation_diagnosticsType_type1;

    msg->message.u.cellChangeOrderFromUTRANFailure.u.r3.cellChangeOrderFromUTRANFailure_r3.
      interRAT_ChangeFailureCause.u.protocolError->diagnosticsType.u.type1 = 
      rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation_type1);

    /* Set the protocol error cause */
    msg->message.u.cellChangeOrderFromUTRANFailure.u.r3.cellChangeOrderFromUTRANFailure_r3.
      interRAT_ChangeFailureCause.u.protocolError->diagnosticsType.u.type1->protocolErrorCause =
      rrccho_proc_info.protocol_error;


  }

#ifdef FEATURE_QSH_EVENT_METRIC
  if(rrc_qsh_dl_committed == FALSE)
  {
    failure_ota.ul_ota_type = RRC_OTA_CHO;
    failure_ota.msg_id = WRRC_UL_DCCH_MESSAGETYPE_CELLCHANGEORDERFROMUTRANFAILURE;
    failure_ota.ul_fail_ota.ul_ota_parms.failure_cause = rrccho_proc_info.failure_cause;
  }
  else
  {
    rrc_qsh_log_cco_fail_params(rrccho_proc_info.failure_cause);
  }
#endif
  
  /*log the error*/
    rrc_log_protocol_error( (uint8) (rrc_get_state()),
                            (uint8)RRC_PROCEDURE_CHO,
                            (uint8) (rrcho_map_inter_rat_cause_to_diag_cause(rrccho_proc_info.failure_cause)),
                            (uint8) rrccho_proc_info.protocol_error                           
                          );

  /* Send the signalling message */
  if(rrcsend_send_ul_sdu(RRC_PROCEDURE_CHO,
                         msg,
                         lc_id,
                         RRCSEND_L2ACK_NOT_REQUIRED
                        ) == RRCSEND_SUCCESS)
  {
    WRRC_MSG0_HIGH("Cell Change Order from UTRAN Failure is sent");
  }
  else
  {
    ERR_FATAL("failed to send Cell Change Order from UTRAN Failure",0,0,0);
  }

  /* Reset the act_time_present flag */
  rrccho_proc_info.act_time_present = FALSE;

}/* rrccho_send_cell_change_order_failure_msg */

/*===========================================================================

FUNCTION rrccho_process_cell_change_order_from_utran_msg

DESCRIPTION
    This function checks whether RRC_DOWNLINK_SDU_IND is for DCCH logical
    channel. If the message is received on DCCH logical channel and checks for
    Cell Chnage Order message and responds with Cell Change Order failure
    message

DEPENDENCIES
    None.

RETURN VALUE
    None

===========================================================================*/

static void rrccho_process_cell_change_order_from_utran_msg
(
  rrc_cmd_type *cmd_ptr                        /* Pointer to the RRC Command */
)
{

  rrc_DL_DCCH_Message *dcch_msg_ptr=NULL;  /* Pointer to DCCH message*/

  uint8 message_choice;                   /* Local varibale for message t*/

  rrc_state_e_type rrc_state = rrc_get_state();
  rrc_CellUpdateCause       cu_cause;  /*To store CU cause from CU procedure*/
#ifdef FEATURE_QSH_EVENT_METRIC
  rrc_log_rrc_cco_ota cco_params; 
#endif
  rrccho_proc_info.failure_cause = T_rrc_InterRAT_HO_FailureCause_unspecified;



  /* Assert the receieved message is of type rrc_DL_CCCH_Message */
  if(RRC_DOWNLINK_SDU_IND == cmd_ptr->cmd_hdr.cmd_id)
  {
    if(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == rrc_DL_DCCH_Message_PDU)
    {

      message_choice =
      rrc_get_dl_dcch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                   cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);

      /*check if decoding was successful otherwise set error flag to true*/
      rrc_protocol_error_reject = FALSE;

      if(cmd_ptr->cmd.downlink_sdu_ind.decoded_msg != NULL)
      {
        dcch_msg_ptr = (rrc_DL_DCCH_Message *)
                       cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;
      } 
      else
      { /* ASN1 decoding failed */
        rrc_protocol_error_reject = TRUE;
      }

      /* Assert the received message is Cell Change Order from UTRAN */
      if( message_choice == T_rrc_DL_DCCH_MessageType_cellChangeOrderFromUTRAN )
      {
        boolean cs_open_status = FALSE;
        boolean ps_open_status = FALSE;

        WRRC_MSG0_HIGH("Cell Change Order from UTRAN is rcvd");

#ifdef FEATURE_QSH_EVENT_METRIC
        memset(&cco_params,0,sizeof(rrc_log_rrc_cco_ota));
#endif
        // UE can receive WtoG CHO only when PS-Signalling Connection is established.
        // If CS+PS is present, even then we should not process the CCO message.
        // According to 25.331 section 8.3.11.3, if the signaling connection status is 
        // not suitable to proceed with CCO, UE should just ignore the message and not
        // send a failure back to the network.
        ps_open_status = (rrcscr_get_signalling_connection_status(RRC_PS_DOMAIN_CN_ID) &&
                          !rrcscrr_is_conn_rel_in_progress(RRC_PS_DOMAIN_CN_ID) &&
                          !rrcdormancy_is_dormancy_active());
        cs_open_status = rrcscr_get_signalling_connection_status(RRC_CS_DOMAIN_CN_ID);

        // check based on foll. truth table condition met
        // PS  CS
        //     0   0  (ps_false error)
        //     0   1  (ps_false and also cs_true error )
        //     1   0  (ps_true and also cs_false, so don't error)
        //     1   1  (ps_true but cs_true error)
        if((ps_open_status == FALSE) || (cs_open_status == TRUE))
        {
          WRRC_MSG2_HIGH("Ignoring CellChangeOrderFromUTRAN due to signaling connection incompatibility. Sig conn status: PS %d, CS %d", ps_open_status, cs_open_status);
          return;
        }

        if(rrcrce_send_abort_cnf == TRUE)
        {
          WRRC_MSG0_HIGH("ABORT is in progress, do not process CCO");
          return;
        }

        if(rrc_protocol_error_reject == TRUE)
        {
          rrccho_proc_info.transaction_id = 0;
          rrccho_proc_info.failure_cause = T_rrc_InterRAT_ChangeFailureCause_protocolError;
          rrccho_proc_info.protocol_error = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
          WRRC_MSG0_ERROR("ASN.1 decoding failed");
        }
        /*lint -save -e613 */
        if((rrc_protocol_error_reject == TRUE) || (rrccho_validate_and_update_info_from_cco_from_utran_msg(
           (rrc_CellChangeOrderFromUTRAN *) &(dcch_msg_ptr->message.
           u.cellChangeOrderFromUTRAN)) == FALSE))
        {
          if(rrc_state == RRC_STATE_CELL_FACH)
          {
            if(RRCCU_NOT_COMPLETED ==
               rrccu_get_cell_update_complete_status(RRC_PROCEDURE_CHO) )
            {
              /* CELL UPDATE is in process of getting a valid C-RNTI. */
              rrccho_substate = RRCCHO_CELL_UPDATE_COMPLETE_CNF;
            }
            else
            {
              rrccho_send_cell_change_order_failure_msg();
            }
          }
          else if(rrc_state == RRC_STATE_CELL_DCH)
          {
            rrccho_send_cell_change_order_failure_msg();
          }
          else
          {
            WRRC_MSG0_ERROR("This should not happen");
          }

        }
        /*lint -restore */
        else if (rrcmcm_is_dualmode_enabled() == FALSE)
        {
          WRRC_MSG0_HIGH("Dual-mode is not enabled");
          /* UE is not enabled for dual-mode, send failure to UTRAN */
          rrccho_proc_info.failure_cause =
            T_rrc_InterRAT_HO_FailureCause_configurationUnacceptable;

          if(RRCCU_NOT_COMPLETED ==
               rrccu_get_cell_update_complete_status(RRC_PROCEDURE_CHO) )
          {
            /* CELL UPDATE is in process of getting a valid C-RNTI. */
            rrccho_substate = RRCCHO_CELL_UPDATE_COMPLETE_CNF;
          }
          else
          {
            rrccho_send_cell_change_order_failure_msg();
          }
        }
        else
        {
#ifdef FEATURE_QSH_EVENT_METRIC
          cco_params.BCCH_ARFCN.band = rrccho_proc_info.cell_change_order_req.BCCH_ARFCN.band;
          cco_params.BCCH_ARFCN.num = rrccho_proc_info.cell_change_order_req.BCCH_ARFCN.num;
          cco_params.BSIC_BCC = rrccho_proc_info.cell_change_order_req.BSIC_BCC;
          cco_params.BSIC_NCC = rrccho_proc_info.cell_change_order_req.BSIC_NCC;
          rrc_qsh_log_cco_params(&cco_params);
#endif
        
            if (rrccu_get_cell_update_started_status_with_cause(&cu_cause) == RRCCU_STARTED) 
            {
              if(rrc_CellUpdateCause_periodicalCellUpdate != cu_cause)
              {
                rrccho_proc_info.failure_cause =
                  T_rrc_InterRAT_HO_FailureCause_unspecified;
                rrccho_send_cell_change_order_failure_msg();
                return;
              }
            }
          /* Request MCM to change the RRC mode from Active to Standby */
#ifdef FEATURE_RRC_DELAY_CELL_CHANGE_ORDER

          /* If the activation time is not present delay acting on the CHO message
           * by 60 ms
           */
          if(!rrccho_proc_info.act_time_present)
          {
            /* Start a timer for 100 milli seconds, so that the
               base station can get RLC L2 Ack for Cell Change Order message. */

            WRRC_MSG0_HIGH("Delaying SUSPEND for CHO by 100 ms");
            (void) rex_clr_sigs(rex_self(), RRCCHO_TEMP_TIMER_SIG);
            (void) rex_set_timer(&rrccho_temp_timer, 100);

            rrccho_sig = rex_wait(RRCCHO_TEMP_TIMER_SIG);
            MSG_LOW("rrccho_sig %d",rrccho_sig,0,0);
            (void) rex_clr_sigs(rex_self(), RRCCHO_TEMP_TIMER_SIG);
          }

#endif /* FEATURE_RRC_DELAY_CELL_CHANGE_ORDER */

          /* Suspend BPLMN if it is active before sending the cco req to GSM side
             This is to ensure that BPLMN on GSM is aborted before the cco request is sent to RR */
          if(WTOW_SEARCH_STOP != rrc_csp_bplmn_srch_in_progress())
          {
            WRRC_MSG0_HIGH("BPLMN active during WTOG CHO. Sending BPLMN Suspend Request to CSP");

			/* Send a request to suspend BPLMN and stay in the same substate to process suspend cnf*/
            rrc_csp_send_bplmn_suspend_req(RRC_PROCEDURE_CHO, BPLMN_SUSPEND_REASON_OTHER);
          }

          rrccho_send_change_mode_req(RRC_MODE_STANDBY);
          rrccho_substate = RRCCHO_WAIT_STANDBY_CNF;
          rrccho_proc_info.cco_in_progress = TRUE;
          ps_clsd_after_suspend_req_in_cco = FALSE;
        }
      }
    } /* Assertion for SDU type */
    else
    {
      /* This should never happen */
      WRRC_MSG1_ERROR("Invalid SDU type is fwded to CHO Proc: %d",
                 cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type);
    }
  }
  else
  {
    WRRC_MSG1_HIGH("Invalid Event: %d is forwarded to CHO Proc",
              cmd_ptr->cmd_hdr.cmd_id);
  }
}/* rrccho_process_cell_change_order_from_utran_msg */
/*===========================================================================

FUNCTION rrccho_process_cho_initial_substate

DESCRIPTION
  This function processes all events that are dispatched to Cell Change
  Order procedure in RRCCHO_INITIAL substate of either CELL_FACH or
    CELL_DCH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrccho_process_cho_initial_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_DOWNLINK_SDU_IND:
      rrccho_process_cell_change_order_from_utran_msg(cmd_ptr);
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d",
                cmd_ptr->cmd_hdr.cmd_id, rrccho_substate);
      break;
  }
} /* rrccho_process_cho_initial_substate */
/*===========================================================================

FUNCTION rrccho_process_cho_wait_standby_cnf_substate

DESCRIPTION
  This function processes all events that are dispatched to Cell Change
  Order procedure in RRCCHO_WAIT_STANDBY_CNF substate of either CELL_FACH or
    CELL_DCH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrccho_process_cho_wait_standby_cnf_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{

  boolean ps_open_status = FALSE;

  ps_open_status = (rrcscr_get_signalling_connection_status(RRC_PS_DOMAIN_CN_ID) &&
                    !rrcscrr_is_conn_rel_in_progress(RRC_PS_DOMAIN_CN_ID) &&
                    !rrcdormancy_is_dormancy_active());

  /* Switch on the received command. */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    /* Change mode confirmation from the MCM */
    case RRC_CHANGE_MODE_CNF:

      /* If either PS session is not open or if the PS session is closed and opened,
         ignore CCO and resume W.*/
      if( ps_open_status && (!ps_clsd_after_suspend_req_in_cco))
      
      {
        /* Initiate Cell Change Order from UTRAN */
        rrccho_send_cell_change_order_req();

        /* Change the procedure state to wait for Cell Change Order confirmation */
        rrccho_substate = RRCCHO_WAIT_CELL_CHANGE_ORDER_CNF;
      }
      else
      {
        ps_clsd_after_suspend_req_in_cco = FALSE;
        rrccho_send_change_mode_req(RRC_MODE_ACTIVE);
        rrccho_substate = RRCCHO_WAIT_ACTIVE_CNF;
      }
      
      break;

    case RRC_IRAT_ABORT_IND:
      rrccho_send_change_mode_req(RRC_MODE_ACTIVE);
      rrccho_substate = RRCCHO_WAIT_ACTIVE_CNF;
      break;

    default:
      /* No other events are normal in this
      substate */
      WRRC_MSG2_ERROR("Unexpected event %x in state %d",
           cmd_ptr->cmd_hdr.cmd_id, rrccho_substate);
      break;
  }

} /* rrccho_process_cho_wait_standby_cnf_substate */
/*===========================================================================

FUNCTION rrccho_process_cho_wait_cell_change_order_cnf

DESCRIPTION
  This function processes all events that are dispatched to Cell Change
  Order procedure in RRCCHO_WAIT_CELL_CHANGE_ORDER_CNF substate of
    either CELL_FACH or CELL_DCH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrccho_process_cho_wait_cell_change_order_cnf
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  if((((uint32)cmd_ptr->cmd_hdr.cmd_id & RRC_RR_CMD_ID_BASE) == RRC_RR_CMD_ID_BASE) )
  {
    switch( (rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id )
    {
      /*lint -e408*/
      case RR_INTERRAT_CC_ORDER_CNF:
        /*lint +e408*/
        WRRC_MSG0_HIGH("Received RR_INTERRAT_CC_ORDER_CNF from RR");
        /*lint -e740*/
        rrccho_process_cell_change_order_cnf(((rrc_rr_cmd_type *) cmd_ptr)->cmd.interrat_cc_order_cnf);
        /*lint +e740*/
        break;
      /* Cell Change Order Reject from GSM */
      /*lint -e408*/
      case RR_INTERRAT_CC_ORDER_REJ:
        /*lint +e408*/
        /* GSM Rejected Cell Change Order Request. So Continue in WCDMA */
        /*lint -e740*/
        rrccho_process_cell_change_order_reject(
          ((rrc_rr_cmd_type *) cmd_ptr)->cmd.interrat_cc_order_rej);
        /*lint +e740*/

        /* Request MCM to change the RRC mode from Standby to Active */
        rrccho_send_change_mode_req(RRC_MODE_ACTIVE);

        rrccho_substate = RRCCHO_WAIT_ACTIVE_CNF;
        break;

      default:
        /* No other events are normal in this
                  substate */
        WRRC_MSG2_ERROR("Unexpected event %x in state %d",
           (rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id, rrccho_substate);
        break;
    }
  }
  else
  {
    switch(cmd_ptr->cmd_hdr.cmd_id )
    {
      case RRC_IRAT_ABORT_IND:
        rrccho_send_cell_change_order_abort_req();
        rrccho_substate = RRCCHO_WAIT_CELL_CHANGE_ORDER_ABORT_CNF;
        break;

      default:
        /* No other events are normal in this
                 substate */
        WRRC_MSG2_ERROR("Unexpected event %x in state %d",
           cmd_ptr->cmd_hdr.cmd_id, rrccho_substate);
        break;
    }
  }
} /* rrccho_process_cho_wait_cell_change_order_cnf */
#ifdef FEATURE_WCDMA_TABASCO_MODEM
/*===========================================================================

FUNCTION rrccho_process_cell_change_order_abort_cnf

DESCRIPTION
  This function processes the RR_INTERRAT_CC_ORDER_CNF received in 
  RRCCHO_WAIT_CELL_CHANGE_ORDER_ABORT_CNF substate of either CELL_FACH 
  or CELL_DCH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrccho_process_cell_change_order_abort_cnf
(
  rr_interrat_cc_order_abort_cnf_type cc_order_abort_cnf  /* Cell Change Order Cnf */
)
{
  mm_cmd_type *mm_cmd_ptr = NULL;
  switch(cc_order_abort_cnf.abort_status)
  {  
    case RRC_RR_SUCCESS:

      rrccho_proc_info.failure_cause = T_rrc_InterRAT_ChangeFailureCause_unspecified;
      rrccho_send_change_mode_req(RRC_MODE_ACTIVE);
      rrccho_substate = RRCCHO_WAIT_ACTIVE_CNF;
      break;

    default:
      ERR_FATAL("Invalid status in cc_order_cnf command from RR:%d",cc_order_abort_cnf.abort_status,0,0);
      break;
  }
}
#else
/*===========================================================================

FUNCTION rrccho_process_cell_change_order_abort_cnf

DESCRIPTION
  This function processes the RR_INTERRAT_CC_ORDER_CNF received in 
  RRCCHO_WAIT_CELL_CHANGE_ORDER_ABORT_CNF substate of either CELL_FACH 
  or CELL_DCH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrccho_process_cell_change_order_abort_cnf
(
  rr_interrat_cc_order_cnf_type cc_order_cnf  /* Cell Change Order Cnf */
)
{
  mm_cmd_type *mm_cmd_ptr = NULL;
  switch(cc_order_cnf.status)
  {  
    case CCO_FAILURE_BEFORE_ACT_IND:

      WRRC_MSG0_HIGH("Received CCO_FAILURE_BEFORE_ACT_IND from RR");

      /* If the failure was due to barred cell,inform CSP to start the barred cell timer */
      if(cc_order_cnf.failure_reason == RR_GSM_CCO_CELL_BARRED)
      {
        rrccsp_send_cphy_cell_bar_req(RRC_CSP_GSM_CELL_BARRED, 320);
      }

      rrccho_proc_info.failure_cause = T_rrc_InterRAT_ChangeFailureCause_physicalChannelFailure;
      rrccho_send_change_mode_req(RRC_MODE_ACTIVE);
      rrccho_substate = RRCCHO_WAIT_ACTIVE_CNF;
      break;

    case CCO_FAILURE_AFTER_ACT_IND:

      WRRC_MSG0_HIGH("Received CCO_FAILURE_AFTER_ACT_IND from RR");
      rrccho_proc_info.failure_cause = T_rrc_InterRAT_ChangeFailureCause_physicalChannelFailure;

      /* Send activation ind to nas*/
      if((mm_cmd_ptr = mm_rrc_get_cmd_buf(RRC_ACTIVATION_IND)) == NULL)
      {
        ERR_FATAL("No MM buffer for RRC_ACTIVATION_IND", 0, 0, 0);
      }
      mm_cmd_ptr->cmd.rrc_activation_ind.status = MM_AS_WTOG_CC_ORDER_FAILED;

#ifdef FEATURE_DUAL_SIM
      mm_cmd_ptr->cmd.rrc_activation_ind.as_id = rrc_get_as_id();
#endif

      /* Put the command on MM queue */
      mm_put_cmd(mm_cmd_ptr);
      WRRC_MSG0_HIGH("RRC_ACTIVATION_IND with status MM_AS_WTOG_CC_ORDER_FAILED sent to MM");

      /*update the cause for activation indication*/
      rrccho_act_cause = RRCCHO_INTERRAT_CC_ORDER_WTOG_FAILED;
      rrcmcm_update_wcdma_activation_cause(RRCMCM_INTERRAT_CC_ORDER_WTOG_FAILED);
      rrccho_substate = RRCCHO_WAIT_ACTIVATION_RSP;
      break;

    default:
      WRRC_MSG1_ERROR("Invalid status in cc_order_cnf command from RR:%d",cc_order_cnf.status);
      break;
  }
}

/* rrccho_process_cho_wait_cell_change_order_cnf */
#endif
#ifdef FEATURE_WCDMA_TABASCO_MODEM 
/*===========================================================================

FUNCTION rrccho_process_cho_wait_cell_change_order_abort_cnf

DESCRIPTION
  This function processes all events that are dispatched to Cell Change
  Order procedure in RRCCHO_WAIT_CELL_CHANGE_ORDER_ABORT_CNF substate 
  of either CELL_FACH or CELL_DCH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrccho_process_cho_wait_cell_change_order_abort_cnf
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
    /* Switch on the received command. */
  switch( (rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id )
  {
    /*lint -e408*/
    case RR_INTERRAT_CC_ORDER_ABORT_CNF:
      /*lint +e408*/
      WRRC_MSG0_HIGH("Received RR_INTERRAT_CC_ORDER_ABORT_CNF from RR");
      /*lint -e740*/
      rrccho_process_cell_change_order_abort_cnf(((rrc_rr_cmd_type *) cmd_ptr)->cmd.interrat_cc_order_abort_cnf);
      /*lint +e740*/
      break;
    /* Cell Change Order Reject from GSM */
    /*lint -e408*/
    case RR_INTERRAT_CC_ORDER_CNF:
    case RR_INTERRAT_CC_ORDER_REJ:
      WRRC_MSG0_HIGH("Waiting for CC order ABORT CNF ignore CC ORDER CNF/REJ");
      break;

    default:
      /* No other events are normal in this
      substate */
      WRRC_MSG2_ERROR("Unexpected event %x in state %d",
           cmd_ptr->cmd_hdr.cmd_id, rrccho_substate);
      break;
  }

} 

#else
/*===========================================================================

FUNCTION rrccho_process_cho_wait_cell_change_order_abort_cnf

DESCRIPTION
  This function processes all events that are dispatched to Cell Change
  Order procedure in RRCCHO_WAIT_CELL_CHANGE_ORDER_ABORT_CNF substate 
  of either CELL_FACH or CELL_DCH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrccho_process_cho_wait_cell_change_order_abort_cnf
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
    /* Switch on the received command. */
  switch( (rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id )
  {
    /*lint -e408*/
    case RR_INTERRAT_CC_ORDER_CNF:
      /*lint +e408*/
      WRRC_MSG0_HIGH("Received RR_INTERRAT_CC_ORDER_CNF from RR");
      /*lint -e740*/
      rrccho_process_cell_change_order_abort_cnf(((rrc_rr_cmd_type *) cmd_ptr)->cmd.interrat_cc_order_cnf);
      /*lint +e740*/
      break;
    /* Cell Change Order Reject from GSM */
    /*lint -e408*/
    case RR_INTERRAT_CC_ORDER_REJ:
      /*lint +e408*/
      /* GSM Rejected Cell Change Order Request. So Continue in WCDMA */
      /*lint -e740*/
      rrccho_process_cell_change_order_reject(
        ((rrc_rr_cmd_type *) cmd_ptr)->cmd.interrat_cc_order_rej);
      /*lint +e740*/
      /* Request MCM to change the RRC mode from Standby to Active */
      rrccho_send_change_mode_req(RRC_MODE_ACTIVE);
      rrccho_substate = RRCCHO_WAIT_ACTIVE_CNF;
      break;

    default:
      /* No other events are normal in this
      substate */
      WRRC_MSG2_ERROR("Unexpected event %x in state %d",
           cmd_ptr->cmd_hdr.cmd_id, rrccho_substate);
      break;
  }

} 
#endif
/*===========================================================================

FUNCTION rrccho_process_cho_wait_active_cnf

DESCRIPTION
  This function processes all events that are dispatched to Cell Change
  Order procedure in RRCCHO_WAIT_ACTIVE_CNF substate of either CELL_FACH or
    CELL_DCH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrccho_process_cho_wait_active_cnf
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  rrc_cmd_type *int_cmd_ptr;

  /* Switch on the received command. */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    /* Change mode confirmation from the MCM */
    case RRC_CHANGE_MODE_CNF:
      if(rrccho_proc_info.cco_send_ind_to_csp == TRUE)
      {
        //send csp resume-ind if needed
        rrccho_send_csp_wcdma_resume_ind();
        rrccho_initialize_internal_data();
      }
      else
      {
        if(cmd_ptr->cmd.change_mode_cnf.status == TRUE)
        {
          rrccho_send_channel_config_req_to_llc();
          rrccho_substate = RRCCHO_WAIT_CHANNEL_CONFIG_CNF;
        }
        else /* change mode failure */
        {
          WRRC_MSG0_HIGH("Rx: RRC_CHANGE_MODE_CNF (Failure) ");
          if(RRC_STATE_CELL_FACH == rrc_get_state())
          {
            if(RRCCU_NOT_COMPLETED == rrccu_get_cell_update_complete_status(RRC_PROCEDURE_CHO) )
            {
              /* CELL UPDATE is in process of getting a valid C-RNTI. */
              rrccho_substate = RRCCHO_CELL_UPDATE_COMPLETE_CNF;
            }
            else //there is no cell update process currently active.
            {
              int_cmd_ptr = rrc_get_int_cmd_buf();
              int_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_UPDATE_INITIATE_REQ;
              int_cmd_ptr->cmd.initiate_cell_update.cause = RRC_PHY_CHAN_FAILURE;
              int_cmd_ptr->cmd.initiate_cell_update.cu_complete_cnf_required = TRUE;
              int_cmd_ptr->cmd.initiate_cell_update.procedure = RRC_PROCEDURE_CHO;

              WRRC_MSG0_HIGH("CELL_UPDATE_INITIATE_REQ sent by CHO on resume failure in CELL_FACH state");
              rrc_put_int_cmd(int_cmd_ptr);

              rrccho_substate = RRCCHO_CELL_UPDATE_COMPLETE_CNF;
            }
          }
          else //we are in cell_dch
          {
            int_cmd_ptr = rrc_get_int_cmd_buf();
            int_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_UPDATE_INITIATE_REQ;
            int_cmd_ptr->cmd.initiate_cell_update.cause = RRC_RADIO_LINK_FAILURE;
            int_cmd_ptr->cmd.initiate_cell_update.cu_complete_cnf_required = TRUE;
            int_cmd_ptr->cmd.initiate_cell_update.procedure = RRC_PROCEDURE_CHO;

            WRRC_MSG0_HIGH("CELL_UPDATE_INITIATE_REQ sent by CHO on resume failure in CELL_DCH state");
            rrc_put_int_cmd(int_cmd_ptr);

            rrccho_substate = RRCCHO_CELL_UPDATE_COMPLETE_CNF;
          }
        }
      }

    break;
    default:
      /* No other events are normal in this
      substate */
      WRRC_MSG2_ERROR("Unexpected event %x in state %d",
           cmd_ptr->cmd_hdr.cmd_id, rrccho_substate);
      break;
  }
} /* rrccho_process_cho_wait_active_cnf */
/*===========================================================================

FUNCTION rrccho_process_cho_wait_channel_config_cnf

DESCRIPTION
  This function processes all events that are dispatched to Cell Change
  Order procedure in RRCCHO_WAIT_CHANNEL_CONFIG_CNF substate of either
  CELL_FACH or CELL_DCH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrccho_process_cho_wait_channel_config_cnf
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  rrc_cmd_type *int_cmd_ptr = NULL;

  /* Switch on the received command. */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    /* Change mode confirmation from the MCM */
    case RRC_CHANNEL_CONFIG_CNF:
      if(rrccho_proc_info.cco_send_ind_to_csp == TRUE)
      {
        //send csp resume-ind if needed
        rrccho_send_csp_wcdma_resume_ind();
        rrccho_initialize_internal_data();
      }
      else
      {
        if (cmd_ptr->cmd.chan_config_cnf.chan_cfg_status == RRCLLC_CHAN_CFG_SUCCESS)
        {
          WRRC_MSG0_HIGH("Rx: RRC_CHANNEL_CONFIG_CNF (Success) ");
          if(RRC_STATE_CELL_FACH == rrc_get_state())
          {
            /*  Send Cell Change Order Reject in the UL*/
            if(RRCCU_NOT_COMPLETED == rrccu_get_cell_update_complete_status(RRC_PROCEDURE_CHO) )
            {
              /* CELL UPDATE is in process of getting a valid C-RNTI. */
              rrccho_substate = RRCCHO_CELL_UPDATE_COMPLETE_CNF;
            }
            else
            {
              rrccho_send_cell_change_order_failure_msg();
              rrccho_initialize_internal_data();
#if defined (FEATURE_GAN_3G) || defined (FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE)
              #error code not present
#endif
            }
          }
          else //we are in cell_dch
          {
            rrccho_send_cell_change_order_failure_msg();
            rrccho_initialize_internal_data();
#if defined (FEATURE_GAN_3G) || defined (FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE)
            #error code not present
#endif
          }
        }
        else //channel config failed.Try to do a cell update
        {
          WRRC_MSG0_HIGH("Rx: RRC_CHANNEL_CONFIG_CNF (Failure) ");
          if(RRC_STATE_CELL_FACH == rrc_get_state())
          {
            if(RRCCU_NOT_COMPLETED == rrccu_get_cell_update_complete_status(RRC_PROCEDURE_CHO) )
            {
              /* CELL UPDATE is in process of getting a valid C-RNTI. */
              rrccho_substate = RRCCHO_CELL_UPDATE_COMPLETE_CNF;
            }
            else //there is no cell update process currently active.
            {
              if ((int_cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
              {
                int_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_UPDATE_INITIATE_REQ;
                int_cmd_ptr->cmd.initiate_cell_update.cause = RRC_PHY_CHAN_FAILURE;
                int_cmd_ptr->cmd.initiate_cell_update.cu_complete_cnf_required = TRUE;
                int_cmd_ptr->cmd.initiate_cell_update.procedure = RRC_PROCEDURE_CHO;

                WRRC_MSG0_HIGH("CELL_UPDATE_INITIATE_REQ sent by CHO on resume failure in CELL_FACH state");
                rrc_put_int_cmd(int_cmd_ptr);

                rrccho_substate = RRCCHO_CELL_UPDATE_COMPLETE_CNF;
              }
            }
          }
          else //we are in cell_dch
          {
            if ((int_cmd_ptr = rrc_get_int_cmd_buf()) != NULL)
            {
              int_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_UPDATE_INITIATE_REQ;
              int_cmd_ptr->cmd.initiate_cell_update.cause = RRC_RADIO_LINK_FAILURE;
              int_cmd_ptr->cmd.initiate_cell_update.cu_complete_cnf_required = TRUE;
              int_cmd_ptr->cmd.initiate_cell_update.procedure = RRC_PROCEDURE_CHO;

              WRRC_MSG0_HIGH("CELL_UPDATE_INITIATE_REQ sent by CHO on resume failure in CELL_DCH state");
              rrc_put_int_cmd(int_cmd_ptr);

              rrccho_substate = RRCCHO_CELL_UPDATE_COMPLETE_CNF;
            }
          }
        }
      }
     break;

    default:
      /* No other events are normal in this substate */
      WRRC_MSG2_ERROR("Unexpected event %x in state %d",cmd_ptr->cmd_hdr.cmd_id, rrccho_substate);
      break;
  }
} /* rrccho_process_cho_wait_channel_config_cnf */
/*===========================================================================

FUNCTION rrccho_process_cho_cell_update_cnf

DESCRIPTION
  This function processes all events that are dispatched to Cell Change
  Order procedure in RRCCHO_CELL_UPDATE_CNF substate of either
    CELL_FACH State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrccho_process_cho_cell_update_cnf
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  rrc_cmd_type *int_cmd_ptr = NULL;
  rrc_state_e_type rrc_state;

  rrc_state = rrc_get_state();

  /* Switch on the received command. */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    /* Cell Update Completed confirmation */
    case RRC_CELL_UPDATE_COMPLETE_CNF:
      if(rrccho_proc_info.cco_send_ind_to_csp == TRUE)
      {
        //send csp resume-ind if needed
        rrccho_send_csp_wcdma_resume_ind();
        rrccho_initialize_internal_data();
      }
      else
      {
        if( (RRC_STATE_CELL_PCH == rrc_state) || (RRC_STATE_URA_PCH == rrc_state) )
        {
          // Initiate a cell update with cause RRC_UL_DATA_TRANSMISSION
          int_cmd_ptr = rrc_get_int_cmd_buf();
          int_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_UPDATE_INITIATE_REQ;
          int_cmd_ptr->cmd.initiate_cell_update.cause = RRC_UL_DATA_TRANSMISSION;
          int_cmd_ptr->cmd.initiate_cell_update.cu_complete_cnf_required = TRUE;
          int_cmd_ptr->cmd.initiate_cell_update.procedure = RRC_PROCEDURE_CHO;

          WRRC_MSG0_HIGH("CELL_UPDATE_INITIATE_REQ sent by CHO on resume failure.Cause:RRC_UL_DATA_TRANSMISSION ");
          rrc_put_int_cmd(int_cmd_ptr);
        }
        else  // We are in cell_dch/cell_fach
        {
          /* Send Cell Change Order Reject in the UL*/
          rrccho_send_cell_change_order_failure_msg();
          rrccho_initialize_internal_data();
#if defined (FEATURE_GAN_3G) || defined (FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE)
          #error code not present
#endif
        }
      }
      break;

    default:
      /* No other events are normal in this substate */
      WRRC_MSG2_ERROR("Unexpected event %x in state %d",cmd_ptr->cmd_hdr.cmd_id, rrccho_substate);
      break;
  }

} /* rrccho_process_cho_cell_update_cnf */

/*===========================================================================

FUNCTION  rrccho_process_wait_activation_rsp_event_hdlr

DESCRIPTION

  This function handles the events received by this module when
  the procedure is Waiting for Activation Rsp from MM.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrccho_process_wait_activation_rsp_event_hdlr( rrc_cmd_type *cmd_ptr )
{
    rrc_SysInfoType3   *sib3_ptr;
#ifdef FEATURE_WCDMA_TABASCO_MODEM
    mm_cmd_type *mm_cmd_ptr = NULL;
#endif
  if((((uint32)cmd_ptr->cmd_hdr.cmd_id & RRC_RR_CMD_ID_BASE) == RRC_RR_CMD_ID_BASE) )
  {
    switch ((rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id)
    {
      case RRC_INTERRAT_CC_ORDER_ABORT_REQ:
        gtow_cho_info.status = CCO_FAILURE_AFTER_ACT_IND;
        gtow_cho_info.failure_reason = RRC_WCDMA_NONE;
        rrccho_send_change_mode_req(RRC_MODE_INACTIVE);
        /* go to inactive substate so that we abort WCDMA*/
        rrccho_substate = RRCCHO_WAIT_INACTIVE_CNF;
        break;

#ifdef FEATURE_WCDMA_TABASCO_MODEM
      case RRC_GRR_INTERRAT_PROC_ABORT_IND:
        mm_cmd_ptr = mm_rrc_get_cmd_buf(RRC_WTOG_CCO_COMPLETE_IND);

#ifdef FEATURE_DUAL_SIM
        mm_cmd_ptr->cmd.rrc_wtog_cco_complete_ind.as_id = rrc_get_as_id();
#endif

        rrc_send_rrc_message_to_mm(mm_cmd_ptr);
        WRRC_MSG0_HIGH("Dispatching out MM cmd RRC_WTOG_CCO_COMPLETE_IND sent to MM");

        /* Change mode to Inactive */
        rrccho_send_change_mode_req(RRC_MODE_INACTIVE);
        rrccho_substate = RRCCHO_WAIT_INACTIVE_CNF;
      
        /* Abort Dormancy if CCO proceed to successful processing. */
        rrcdormancy_update_dormancy_for_srns_hho_cco(RRC_PROCEDURE_CHO);
        break;
#endif
      default:
        /* Unexpected command */
        WRRC_MSG2_ERROR("Unexpected cmd %x in CHO state %d", (rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id, rrccho_substate);
        break;      
    }
  }
  else
  {
    switch( cmd_ptr->cmd_hdr.cmd_id )
    {
      case RRC_ACTIVATION_RSP:
        if(rrccho_act_cause == RRCCHO_INTERRAT_CC_ORDER_WTOG_FAILED)
        {

          WRRC_MSG0_HIGH("Rxed: RRC_ACTIVATION_RSP after WTOG_CC_ORDER Failure");
 
          sib3_ptr = (rrc_SysInfoType3 *) rrc_sibdb_return_sib_for_cell(
                     rrc_SIB3, 
                     rrc_csp_int_data.curr_acq_entry.plmn_id, 
                     rrc_csp_int_data.curr_acq_entry.cell_id);

          if(sib3_ptr == NULL)
          {
            /* If SIB DB is not available, send last backed up service ind and set cco flag such
               that leaving conn mode scan can be triggered */
            rrc_csp_send_service_ind_dummy();
            rrccho_proc_info.cco_send_ind_to_csp = TRUE;
          }
          else
          {
            /* Send a service ind to MM.MM should not send any RAU/EST_REQ */
            rrc_csp_send_service_ind();
          }

          WRRC_MSG0_HIGH("RRC_SERVICE_IND sent to MM");
  
          /* Resume WCDMA mode */
          rrccho_send_change_mode_req(RRC_MODE_ACTIVE);
  
          /* In case of CCO failure send RABM_EST_IND to upper layes */
          rrcrbe_send_rabm_est_ind_to_upper_layers();
  
          rrccho_substate = RRCCHO_WAIT_ACTIVE_CNF;
        }
        else if(rrccho_act_cause == RRCCHO_INTERRAT_CC_ORDER_GTOW)
        {
          /* go to initial substate */
          WRRC_MSG0_HIGH("Rxed: RRC_ACTIVATION_RSP for GTOW_CC_ORDER");
  
          /* Send a service ind to MM.This would trigger RAU from MM */
          rrc_csp_send_service_ind();
          WRRC_MSG0_HIGH("RRC_SERVICE_IND sent to MM");
  
          /* In case of CCO failure send RABM_EST_IND to upper layes */
          rrcrbe_send_rabm_est_ind_to_upper_layers();
  
          /* Wait for RRC_CONN_SETUP_IND */
          rrccho_substate = RRCCHO_WAIT_CONN_SETUP_IND;
        }
        else
        {
          WRRC_MSG0_ERROR("Rxed: RRC_ACTIVATION_RSP for unknown cause");
          rrccho_initialize_internal_data();
        }
        /* Reset the act_cause variable */
        rrccho_act_cause = RRCCHO_ACT_CAUSE_NONE;
        break;

      default:
       /* Unexpected command */
       WRRC_MSG2_ERROR("Unexpected cmd %x in CHO state %d", cmd_ptr->cmd_hdr.cmd_id, rrccho_substate);
       break;
    }
  }
} /* rrccho_process_wait_activation_rsp_event_hdlr */

/*===========================================================================

FUNCTION rrccho_process_cell_fach_state

DESCRIPTION
  This function processes all events that are dispatched to Cell Change
  Order from UTRAN procedure in either CELL_FACH state of Connected  mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrccho_process_cell_fach_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  rrc_state_e_type rrc_state;
  rrc_state = rrc_get_state();
  switch (rrccho_substate)
  {
    
    case RRCCHO_INITIAL:
      rrccho_process_cho_initial_substate(cmd_ptr);
      break;
    case RRCCHO_WAIT_STANDBY_CNF:
      rrccho_process_cho_wait_standby_cnf_substate(cmd_ptr);
      break;
    case RRCCHO_WAIT_CELL_CHANGE_ORDER_CNF:
      rrccho_process_cho_wait_cell_change_order_cnf(cmd_ptr);
      break;
    case RRCCHO_WAIT_ACTIVE_CNF:
      rrccho_process_cho_wait_active_cnf(cmd_ptr);
      break;
    case RRCCHO_CELL_UPDATE_COMPLETE_CNF:
      rrccho_process_cho_cell_update_cnf(cmd_ptr);
      break;
    case RRCCHO_WAIT_CHANNEL_CONFIG_CNF:
      rrccho_process_cho_wait_channel_config_cnf(cmd_ptr);
      break;
    case RRCCHO_WAIT_ACTIVATION_RSP:
      rrccho_process_wait_activation_rsp_event_hdlr(cmd_ptr);
      break;
    case RRCCHO_WAIT_CELL_CHANGE_ORDER_ABORT_CNF:
      rrccho_process_cho_wait_cell_change_order_abort_cnf(cmd_ptr);
      break;
    default:
      WRRC_MSG2_HIGH("Invalid substate in %d State: %d",rrc_state,rrccho_substate);
      break;
  }
} /* rrccho_process_cell_fach_state */

/*===========================================================================

FUNCTION   rrccho_get_activation_time_for_wtog_cho

DESCRIPTION

  This function populates activation time for WtoG cell change order into the received
  ptr and also returns success/failure if activation is present or not.

DEPENDENCIES

  None.

RETURN VALUE

  SUCCESS/FAILURE depending if Activation Time was appended or not.

SIDE EFFECTS

  None.

===========================================================================*/
uecomdef_status_e_type rrccho_get_activation_time_for_wtog_cho
(
uint16 * act_time
)
{
  /*if activation time was present in the message then send the same*/
  if(rrccho_proc_info.act_time_present == TRUE)
  {
    *act_time = (uint16)(rrccho_proc_info.activation_time);
    return SUCCESS;
  }
  else
  {
    return FAILURE;
  }
} /* rrccho_get_activation_time_for_wtog_cho */

/*===========================================================================

FUNCTION   rrccho_is_gtow_pcco_in_progress

DESCRIPTION

  This function returns true/false based on whether GtoW PCCO is in progress
  or not.

DEPENDENCIES

  None.

RETURN VALUE

  TRUE/FALSE depending if Activation Time was appended or not.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrccho_is_gtow_pcco_in_progress(void)
{
  return gtow_cho_info.cc_order_in_progress;
}

/*===========================================================================

FUNCTION   rrccho_process_wait_inactive_cnf_event_handler

DESCRIPTION

  This function handles the events received by this module when
  the procedure is Waiting for Inactive Confirmation from MCM

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrccho_process_wait_inactive_cnf_event_handler( rrc_cmd_type *cmd_ptr )
{
  /* Switch on the received command. */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_CHANGE_MODE_CNF:
      WRRC_MSG0_HIGH("Rx: RRC_CHANGE_MODE_CNF(Inactive)");

      if(cmd_ptr->cmd.change_mode_cnf.mode == RRC_MODE_INACTIVE)
      {
        if(gtow_cho_info.cc_order_in_progress)
        {
          /* Force initialise RCE so that RCE does not send an ABORT_IND to MM */
          rrcrce_force_initialize(TRUE);

          /* Send PCCO Cnf.Failure cause and status(Failure) would already be set */
          rrccho_send_pcco_cnf();
        }
#ifdef FEATURE_WCDMA_TABASCO_MODEM
        if(rrccho_proc_info.cco_in_progress == TRUE)
        {
          rrccho_send_wl1_stop_ind();
        }
#endif
        /* Reset state machine */
        rrccho_initialize_internal_data();
      }
      break;

    default:
      /* Unexpected command */
      WRRC_MSG2_ERROR("Unexpected CMD %x in state %d", cmd_ptr->cmd_hdr.cmd_id, rrccho_substate);
      break;
  }
} /* rrccho_process_wait_inactive_cnf_event_handler */

/*===========================================================================

FUNCTION  rrccho_process_wait_conn_setup_ind_event_hdlr

DESCRIPTION

  This function handles the events received by this module when
  the procedure is Waiting for Connection Indication from RCE

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrccho_process_wait_conn_setup_ind_event_hdlr( rrc_cmd_type *cmd_ptr )
{
  /* Switch on the received command. */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_CONN_SETUP_IND:

      if(cmd_ptr->cmd.rrc_conn_setup_ind.status == SUCCESS)
      {
        WRRC_MSG0_HIGH("Rxed: RRC_CONN_SETUP_IND with Success");

        /* Stop Timer T3174 */
        rrctmr_stop_timer(RRCTMR_T_3174_TIMER);

        /* Update RRC Mode to Active since the PCCO was successful */
        rrcmcm_update_rrc_mode_to_active();

        /* Send the PCCO CNF to RR indicating Success */
        gtow_cho_info.status = CCO_SUCCESS;
        gtow_cho_info.failure_reason = RRC_WCDMA_NONE;
        rrccho_send_pcco_cnf();

        /* Initialize internal data & reset state machine */
        rrccho_initialize_internal_data();
      }
      else
      {
        WRRC_MSG0_HIGH("Rxed: RRC_CONN_SETUP_IND with Failure");

         /* Stop Timer T3174 */
        rrctmr_stop_timer(RRCTMR_T_3174_TIMER);

        /* Set PCCO status and failure reason */
        gtow_cho_info.status = CCO_FAILURE_AFTER_ACT_IND;
        if(cmd_ptr->cmd.rrc_conn_setup_ind.conn_failure_reason == RRC_MAX_RACH_ATTEMPTS)
        {
          gtow_cho_info.failure_reason = RRC_WCDMA_MAX_RACH_ATTEMPTS;
        }
        else if(cmd_ptr->cmd.rrc_conn_setup_ind.conn_failure_reason == RRC_CONNECTION_REJECT)
        {
          gtow_cho_info.failure_reason = RRC_WCDMA_CONNECTION_REJECT;
        }
        else
        {
          /* Set a generic failure cause */
          gtow_cho_info.failure_reason = RRC_WCDMA_CELL_CHANGE_FAILURE;
        }

        /* Change mode to Inactive */
        rrccho_send_change_mode_req(RRC_MODE_INACTIVE);
        rrccho_substate = RRCCHO_WAIT_INACTIVE_CNF;
      }
      break;

    default:
      /* Unexpected command */
      WRRC_MSG2_ERROR("Unexpected cmd %x in CHO state %d", cmd_ptr->cmd_hdr.cmd_id, rrccho_substate);
      break;
  }
} /* rrccho_process_wait_conn_setup_ind_event_hdlr */

/*===========================================================================

FUNCTION  rrccho_process_wait_camp_cnf_event_hdlr

DESCRIPTION

  This function handles the events received by this module when
  the procedure is Waiting for Camping confirmation from CSP

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrccho_process_wait_camp_cnf_event_hdlr( rrc_cmd_type *cmd_ptr )
{
  mm_cmd_type *mm_cmd_ptr = NULL;

  if((((uint32)cmd_ptr->cmd_hdr.cmd_id & RRC_RR_CMD_ID_BASE) == RRC_RR_CMD_ID_BASE) )
  {
    switch((rrc_rr_cmd_e_type) cmd_ptr->cmd_hdr.cmd_id )
    {
      case RRC_INTERRAT_CC_ORDER_ABORT_REQ:
        gtow_cho_info.status = CCO_FAILURE_BEFORE_ACT_IND;
        gtow_cho_info.failure_reason = RRC_WCDMA_NONE;
        rrccho_send_change_mode_req(RRC_MODE_INACTIVE);
        /* go to inactive substate so that we abort WCDMA*/
        rrccho_substate = RRCCHO_WAIT_INACTIVE_CNF;
        break;

      default:
        /* Unexpected command */
      WRRC_MSG2_ERROR("Unexpected cmd %x in CHO state %d", cmd_ptr->cmd_hdr.cmd_id, rrccho_substate);
        break;
    }    
  }
  else
  {
    switch( cmd_ptr->cmd_hdr.cmd_id )
    {
      case RRC_CSP_CAMP_CNF:
       if(cmd_ptr->cmd.rrc_csp_camp_cnf.camping_status == SUCCESS)
       {
         WRRC_MSG0_HIGH("Rxed: RRC_CSP_CAMP_CNF(Success)");

         /* Send Activation Ind to MM since we have camped successfully */
         mm_cmd_ptr = mm_rrc_get_cmd_buf(RRC_ACTIVATION_IND);
         mm_cmd_ptr->cmd.rrc_activation_ind.status = MM_AS_GTOW_CC_ORDER;

#ifdef FEATURE_DUAL_SIM
         mm_cmd_ptr->cmd.rrc_activation_ind.as_id = rrc_get_as_id();
#endif
         /* Put the command in MM queue */
         rrc_send_rrc_message_to_mm(mm_cmd_ptr);

        WRRC_MSG0_HIGH("Dispatching out MM cmd RRC_ACTIVATION_IND sent to MM");

          /* Update the cause for activation indication */
          rrccho_act_cause = RRCCHO_INTERRAT_CC_ORDER_GTOW;
          rrcmcm_update_wcdma_activation_cause(RRCMCM_INTERRAT_CC_ORDER_GTOW);

          /* Wait for Activation Rsp from MM*/
          rrccho_substate = RRCCHO_WAIT_ACTIVATION_RSP;
        }
        else
        {
          WRRC_MSG0_HIGH("Rxed: RRC_CSP_CAMP_CNF(Failure)");
        
          /* Stop Timer T3174 */
          rrctmr_stop_timer(RRCTMR_T_3174_TIMER);
        
          /* Set PCCO status and failure reason */
          gtow_cho_info.status = CCO_FAILURE_BEFORE_ACT_IND;
          gtow_cho_info.failure_reason = cmd_ptr->cmd.rrc_csp_camp_cnf.failure_cause;
        
          /* Change mode to Inactive */
          rrccho_send_change_mode_req(RRC_MODE_INACTIVE);
          rrccho_substate = RRCCHO_WAIT_INACTIVE_CNF;
        }
        break;

      default:
        /* Unexpected command */
        WRRC_MSG2_ERROR("Unexpected cmd %x in CHO state %d", cmd_ptr->cmd_hdr.cmd_id, rrccho_substate);
        break;
    }
  }
} /* rrccho_process_wait_camp_cnf_event_hdlr */

/*===========================================================================

FUNCTION   rrccho_send_camp_req

DESCRIPTION

  This function sends a request to CSP to camp on a WCDMA cell.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrccho_send_camp_req(void)
{
  rrc_cmd_type *cmd_ptr;

  /* Get a RRC internal command buffer */
  cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command id  */
  cmd_ptr->cmd_hdr.cmd_id = RRC_CSP_CAMP_REQ;

  /* Fill in the Frequency on which we want to acquire */
  cmd_ptr->cmd.rrc_csp_camp_req.freq = gtow_cho_info.freq;

  /* Fill in the scrambling code */
  cmd_ptr->cmd.rrc_csp_camp_req.scr_code = (uint16)(gtow_cho_info.scr_code * 16);

  /* Fill in the PLMN Id */
  cmd_ptr->cmd.rrc_csp_camp_req.requested_plmn = gtow_cho_info.requested_plmn;

  /* Fill the network select mode */
  cmd_ptr->cmd.rrc_csp_camp_req.network_select_mode = gtow_cho_info.network_select_mode;

  /* Put the command on the internal queue */
  rrc_put_int_cmd( cmd_ptr );

  WRRC_MSG0_HIGH("RRC_CSP_CAMP_REQ sent to CSP");

}/*rrccho_send_camp_req*/

/*===========================================================================

FUNCTION  rrccho_process_wait_irat_in_progress_cnf_event_hdlr

DESCRIPTION

  This function handles the events received by this module when
  the procedure is Waiting for Interrat in Progress Cnf from MCM

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrccho_process_wait_irat_in_progress_cnf_event_hdlr( rrc_cmd_type *cmd_ptr )
{
  if((((uint32)cmd_ptr->cmd_hdr.cmd_id & RRC_RR_CMD_ID_BASE) == RRC_RR_CMD_ID_BASE) )
  {
    switch( (rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id )
    {
      case RRC_INTERRAT_CC_ORDER_ABORT_REQ:
        gtow_cho_info.status = CCO_FAILURE_BEFORE_ACT_IND;
        gtow_cho_info.failure_reason = RRC_WCDMA_NONE;
        rrccho_send_change_mode_req(RRC_MODE_INACTIVE);
        /* go to inactive substate so that we abort WCDMA*/
        rrccho_substate = RRCCHO_WAIT_INACTIVE_CNF;
        break;
  
        default:
          /* Unexpected command */
          WRRC_MSG2_ERROR("Unexpected cmd %x in CHO state %d", (rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id, rrccho_substate);
          break;
      }    

  }
  else
  {
    switch( cmd_ptr->cmd_hdr.cmd_id )
    {
      /* RRC_CHANGE_MODE_CNF */
      case RRC_CHANGE_MODE_CNF:
  
        WRRC_MSG0_HIGH("Rx: RRC_CHANGE_MODE_CNF(IRAT_PROG)");
  
        /* Request CSP to camp on the cell */
        rrccho_send_camp_req();
  
        /* Set the procedure state to wait for Acquisition confirmation */
        rrccho_substate = RRCCHO_WAIT_CAMP_CNF;
  
        break;
  
      default:
        /* Unexpected command */
        WRRC_MSG2_ERROR("Unexpected cmd %x in CHO state %d", cmd_ptr->cmd_hdr.cmd_id, rrccho_substate);
        break;
    }  
  }
} /* end of rrccho_process_wait_irat_in_progress_cnf_event_hdlr */

/*===========================================================================

FUNCTION   RRCCHO_PROCESS_PCCO_TO_UTRAN_COMMAND

DESCRIPTION

  This function processes the Packet Cell Change Order to UTRAN request from
  GSM RR.Copies the contents into local structure and activates the WCDMA stack .

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrccho_process_pcco_to_utran_command
(
  rrc_rr_cmd_type * cmd_ptr
)
{
#ifdef FEATURE_DUAL_SIM
  wrm_freq_input_type lock_req_bands;
#endif 
  /* Copy the contents of the PCCO Request to local variables */
  gtow_cho_info.requested_plmn = mmumts_convert_nas_plmn_id_to_rrc_plmn_id(cmd_ptr->cmd.interrat_cc_order_req.requested_PLMN);
  gtow_cho_info.freq = cmd_ptr->cmd.interrat_cc_order_req.UARFCN;
  gtow_cho_info.scr_code = cmd_ptr->cmd.interrat_cc_order_req.scrambling_code;
  gtow_cho_info.network_select_mode = cmd_ptr->cmd.interrat_cc_order_req.network_select_mode;
  gtow_cho_info.t3174 = cmd_ptr->cmd.interrat_cc_order_req.t3174;

#ifdef FEATURE_WCDMA_DEDICATED_PRI_INFO
  /*updating the dedicated priority info received in rrc_interrat_cc_order_req into global structure (rrc_dedicated_priority_info)*/
  rrcmeas_update_dedicated_priority_info_from_irat_req(&cmd_ptr->cmd.interrat_cc_order_req.dedicated_priority_info,
                           mmumts_convert_nas_plmn_id_to_rrc_plmn_id(cmd_ptr->cmd.interrat_cc_order_req.requested_PLMN));
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  rrc_csp_update_rat_pri_list_info(&cmd_ptr->cmd.interrat_cc_order_req.rat_pri_list_info);
#endif

  MSG_HIGH("PCCO params: UARFCN %d,PSC %d,T3174 %d msec",
	          gtow_cho_info.freq,
	          gtow_cho_info.scr_code,
            gtow_cho_info.t3174);

  WRRC_MSG1_HIGH("PCCO params contd: Network Select Mode is %d",
              gtow_cho_info.network_select_mode);

  /* Set T3174 to atleast 5 secs */
  if(gtow_cho_info.t3174 < 5000)
  {
    WRRC_MSG0_HIGH("Setting T3174 to 5 secs");
    gtow_cho_info.t3174 = 5000;
  }

#ifdef FEATURE_DUAL_SIM
  rrc_perform_trm_exchange();
  rrc_wrm_populate_bands_for_lock_req(&lock_req_bands, gtow_cho_info.freq);

  /* Acquire lock before starting WCDMA for GTOW CCO */
  if(rrc_acquire_lock_for_IHO_or_CCO(gtow_cho_info.t3174, lock_req_bands) != RRC_WRM_LOCK_ACQUIRED)
  {
    WRRC_MSG0_HIGH("DSIM: Lock not acquired for CCO.");
    /* Set PCCO status and failure reason */
    gtow_cho_info.status = CCO_FAILURE_BEFORE_ACT_IND;
    gtow_cho_info.failure_reason = RRC_WCDMA_INAVALID_STATE;

    /* Send PCCO Cnf.Failure cause and status(Failure) would already be set */
    rrccho_send_pcco_cnf();
    /* Reset state machine */

    rrccho_initialize_internal_data();
    return;
  }
  WRRC_MSG0_HIGH("DSIM: Lock acquired for CCO.");
  /* Start timer T3174 with the remaining time of lock wait timer */
  rrctmr_start_timer(RRCTMR_T_3174_TIMER,rrctmr_get_remaining_time(RRCTMR_WRM_LOCK_TIMER));
  rrctmr_stop_timer(RRCTMR_WRM_LOCK_TIMER);
#else
  /* Start timer T3174.T3174 is in ms */
  rrctmr_start_timer(RRCTMR_T_3174_TIMER,(gtow_cho_info.t3174));
#endif

  /* Request MCM to change the RRC mode Inactive to Interrat in Progress */
  rrccho_send_change_mode_req(RRC_MODE_INTERRAT_IN_PROGRESS);

  /* Change the procedure state to wait for change mode confirmation */
  rrccho_substate = RRCCHO_WAIT_IRAT_IN_PROGRESS_CNF;
} /* rrccho_process_pcco_to_utran_command */


/*===========================================================================

FUNCTION   RRCCHO_FILL_GSM_CELL_INFO_FOR_CELL_BAR

DESCRIPTION

  This function is called by CSP when WtoG CCO fails because the GSM cell is
  barred. This function populates the GSM cell bar structure with the arfcn
  and bsic values from rrccho_proc_info global structure so that the GSM cell
  can be appropriately barred.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrccho_fill_gsm_cell_info_for_cell_bar
(
  W_ARFCN_T *gsm_arfcn,
  uint8     *gsm_ncc,
  uint8     *gsm_bcc
)
{
  WRRC_MSG0_HIGH("Fn rrccho_fill_gsm_cell_info_for_cell_bar called");
  *gsm_arfcn = rrccho_proc_info.cell_change_order_req.BCCH_ARFCN;
  *gsm_ncc   = rrccho_proc_info.cell_change_order_req.BSIC_NCC;
  *gsm_bcc   = rrccho_proc_info.cell_change_order_req.BSIC_BCC;
} /* rrccho_fill_gsm_cell_info_for_cell_bar */


/*===========================================================================

FUNCTION rrccho_procedure_event_handler

DESCRIPTION
    This procedure is the event handler for the RRC Cell Change Order procedure.
    The following events are handled by this procedure.


DEPENDENCIES
    None.

RETURN VALUE
    None.

===========================================================================*/
void rrccho_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr      /* Pointer to RRC command */
)
{
  rrc_state_e_type rrc_state = rrc_get_state();

  WRRC_MSG2_HIGH_OPT("function rrccho_procedure_event_handler is called in rrc_state = rrc_state_e_type_value%d and rrccho_substate rrccho_state_e_type_value%d", rrc_state, rrccho_substate);
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_CHO,rrccho_substate,cmd_ptr->cmd_hdr.cmd_id);

  if(cmd_ptr->cmd_hdr.cmd_id == RRCTMR_ACT_RSP_EF_TIMER_EXPIRED_IND)
  {
    if(rrccho_substate == RRCCHO_WAIT_ACTIVATION_RSP)
    {
      ERR_FATAL("NO response to ACTIVATION_IND from NAS for more than 2 seconds",0,0,0);
    }
  }

  if(cmd_ptr->cmd_hdr.cmd_id == RRC_STATE_CHANGE_IND)
  {
    if(gtow_cho_info.cc_order_in_progress)
    {
      WRRC_MSG1_HIGH("RRC_STATE_CHANGE_IND when GTOW PCCO in progress,cho_substate %d",rrccho_substate);

      /* If substate is not Wait_Inactive_cnf,some other procedure has taken the UE to
         idle.Set the status and failure cause and send pcco_cnf */
      if(rrccho_substate != RRCCHO_WAIT_INACTIVE_CNF)
      {
        /* Stop Timer T3174 */
        rrctmr_stop_timer(RRCTMR_T_3174_TIMER);

        /* Set PCCO status and failure reason */
        if( (rrccho_substate == RRCCHO_WAIT_IRAT_IN_PROGRESS_CNF)||
            (rrccho_substate == RRCCHO_WAIT_CAMP_CNF) )
        {
          gtow_cho_info.status = CCO_FAILURE_BEFORE_ACT_IND;
        }
        else
        {
           gtow_cho_info.status = CCO_FAILURE_AFTER_ACT_IND;
        }

        /* We do not know the exaect reason for UE to Idle since CHO
           did not initiate it.Hence set a generic reason for PCCO failure */
        gtow_cho_info.failure_reason = RRC_WCDMA_CELL_CHANGE_FAILURE;

        /* Send PCCO_CNF */
        rrccho_send_pcco_cnf();
      }
      else
      {
        /* Else,the mode change to Inactive was initiated by CHO procedure.The status and failure
           cause would have been set already when the mode change was initiated.Just send
           the pcco_cnf */
        rrccho_send_pcco_cnf();
      }
    }
    else
    {
      MSG_MED("RRC State Change to IDLE in %d", rrccho_substate, 0, 0);
    }
#ifdef FEATURE_WCDMA_TABASCO_MODEM
    if(rrccho_substate == RRCCHO_WAIT_INACTIVE_CNF)
    {
      if(rrccho_proc_info.cco_in_progress == TRUE)
      {
        rrccho_send_wl1_stop_ind();
      }
    }
#endif
    /* Reset state machine */
    rrccho_initialize_internal_data();
    rrccho_act_cause = RRCCHO_ACT_CAUSE_NONE;
  }
  else if(cmd_ptr->cmd_hdr.cmd_id == RRC_T3174_EXPIRED_IND)
  {
    if(gtow_cho_info.cc_order_in_progress)
    {
      WRRC_MSG1_HIGH("RRC_T3174_EXPIRED_IND in CHO sub-state %d", rrccho_substate);

      /* If CHO is already waiting for an Inactive_Cnf,it means we are already in the
         process of deactivating WCDMA due to some other failure.Hence we can safely
         ignore the Timer expiry Ind.Hence process the expired ind only if CHO is not
         in wait_inactive_cnf substate */
      if(rrccho_substate != RRCCHO_WAIT_INACTIVE_CNF)
      {
        WRRC_MSG1_HIGH("Stopping WCDMA in CHO sub-state %d", rrccho_substate);

        /* Set PCCO status and failure reason */
        if( (rrccho_substate == RRCCHO_WAIT_IRAT_IN_PROGRESS_CNF)||
            (rrccho_substate == RRCCHO_WAIT_CAMP_CNF) )
        {
          gtow_cho_info.status = CCO_FAILURE_BEFORE_ACT_IND;
        }
        else
        {
           gtow_cho_info.status = CCO_FAILURE_AFTER_ACT_IND;
        }
        gtow_cho_info.failure_reason = RRC_WCDMA_T3174_EXPIRED;

        /* Send Change Mode Request to Inactive */
        rrccho_send_change_mode_req(RRC_MODE_INACTIVE);
        rrccho_substate = RRCCHO_WAIT_INACTIVE_CNF;
      }
      else
      {
        WRRC_MSG0_HIGH("CHO already in the process of de-activating WCDMA.Hence,Timer Expiry not handled");
      }
    }
    else
    {
      WRRC_MSG1_HIGH("RRC_T3174_EXPIRED_IND when GTOW PCCO is not active.Ignoring cmd.rrc_state: %d",rrc_state);
    }
  }
  else
  {
    switch(rrc_state)
    {
      case RRC_STATE_CELL_FACH:
      case RRC_STATE_CELL_DCH:
        rrccho_process_cell_fach_state(cmd_ptr);
        break;

      case RRC_STATE_DISCONNECTED:
      case RRC_STATE_CONNECTING:
        switch(rrccho_substate)
        {
          case RRCCHO_INITIAL:
            switch((rrc_rr_cmd_e_type)cmd_ptr->cmd_hdr.cmd_id)
            {
              /*lint -e408*/
              case RRC_INTERRAT_CC_ORDER_REQ:
              /*lint +e408*/
                if(gtow_cho_info.cc_order_in_progress)
                {
                  WRRC_MSG0_HIGH("Dropping PCCO request from RR since PCCO is already active.");
                }
                else
                {
                  /* Received an Interrat PCCO Request from RR */
                  WRRC_MSG0_HIGH("Received RRC_INTERRAT_CC_ORDER_REQ from RR");

                  /* Set gtow flag to True */
                  gtow_cho_info.cc_order_in_progress = TRUE;

                  /* Process the GTOW PCCO request */
                  /*lint -e740*/
                  rrccho_process_pcco_to_utran_command( (rrc_rr_cmd_type *)cmd_ptr);
                  /*lint +e740*/
                }
                break;

              default:
                /* Unexpected event */
                MSG_HIGH("Unexpected cmd %x in CHO: RRC st: %d, CHO st: %d",
				          cmd_ptr->cmd_hdr.cmd_id,rrc_state,rrccho_substate);
                break;
            }
            break;

          case RRCCHO_WAIT_IRAT_IN_PROGRESS_CNF:
            rrccho_process_wait_irat_in_progress_cnf_event_hdlr(cmd_ptr);
            break;

          case RRCCHO_WAIT_CAMP_CNF:
            rrccho_process_wait_camp_cnf_event_hdlr(cmd_ptr);
            break;

          case RRCCHO_WAIT_ACTIVATION_RSP:
            rrccho_process_wait_activation_rsp_event_hdlr(cmd_ptr);
            break;

          case RRCCHO_WAIT_CONN_SETUP_IND:
            rrccho_process_wait_conn_setup_ind_event_hdlr(cmd_ptr);
            break;

          case RRCCHO_WAIT_INACTIVE_CNF:
            rrccho_process_wait_inactive_cnf_event_handler(cmd_ptr);
            break;

          default:
            /* Unexpected event */
            WRRC_MSG2_ERROR("Unexpected event to CHO. RRC st: %d, CHO st: %d", rrc_state,rrccho_substate);
            break;
        }
        break;

      default:
        /* Unexpected event */
        WRRC_MSG1_ERROR("Event fwded to CHO in wrong RRC st: %d",
            rrc_state);
        break;
    }
  }
}   /* rrccho_procedure_event_handler */

/*===========================================================================

FUNCTION rrccho_init_procedure

DESCRIPTION
    This function initializes everything that the RRC Cell Change Order procedure
    needs to have initialized.

DEPENDENCIES
    None.

RETURN VALUE
    None.

===========================================================================*/

void rrccho_init_procedure(void)
{
  /* Reset variables */
  rrccho_initialize_internal_data();

    /* Register the state change from DCH to Idle */
  rrcscmgr_register_for_scn( RRC_PROCEDURE_CHO,      /* Procedure name */
                             RRC_STATE_CELL_DCH,     /* From State     */
                             RRC_STATE_DISCONNECTED  /* To State       */
                           );
    /* Register the state change from FACH to Idle */
  rrcscmgr_register_for_scn( RRC_PROCEDURE_CHO,      /* Procedure name */
                             RRC_STATE_CELL_FACH,    /* From State     */
                             RRC_STATE_DISCONNECTED  /* To State       */
                           );

  /* Register the state change from CONNECTING to Idle */
  rrcscmgr_register_for_scn( RRC_PROCEDURE_CHO,      /* Procedure name */
                             RRC_STATE_CONNECTING,    /* From State     */
                             RRC_STATE_DISCONNECTED  /* To State       */
                           );

#ifdef FEATURE_RRC_DELAY_CELL_CHANGE_ORDER
  /* Define a timer used to delay 60ms before
  suspending WCDMA */
  rex_def_timer(&rrccho_temp_timer, rex_self(), RRCCHO_TEMP_TIMER_SIG);
#endif /* FEATURE_RRC_DELAY_CELL_CHANGE_ORDER */
}
#ifdef FEATURE_WCDMA_HANDLE_MDSP_HALT
/*===========================================================================

FUNCTION rrccho_handle_mdsp_halt

DESCRIPTION 
    This function handles MDSP halt in various CCO substates.

DEPENDENCIES
    None.

RETURN VALUE
    None.

===========================================================================*/
void rrccho_handle_mdsp_halt(void)
{
  if(rrccho_is_wtog_cco_active())
  {
    rrccho_initialize_internal_data();
  }
  else if(rrccho_is_gtow_pcco_in_progress())
  {
    /* Stop Timer T3174 */
    rrctmr_stop_timer(RRCTMR_T_3174_TIMER);

    /* Set PCCO status and failure reason */
    if( (rrccho_substate == RRCCHO_WAIT_IRAT_IN_PROGRESS_CNF)||
        (rrccho_substate == RRCCHO_WAIT_CAMP_CNF) )
    {
      gtow_cho_info.status = CCO_FAILURE_BEFORE_ACT_IND;
    }
    else
    {
       gtow_cho_info.status = CCO_FAILURE_AFTER_ACT_IND;
    }

    /* Proper failure reason TBD */
    gtow_cho_info.failure_reason = RRC_WCDMA_INAVALID_STATE;

    /* Send Change Mode Request to Inactive */
    rrccho_send_change_mode_req(RRC_MODE_INACTIVE);
    rrccho_substate = RRCCHO_WAIT_INACTIVE_CNF;
  }
}
#endif
/*===========================================================================
FUNCTION rrccho_get_current_substate()

DESCRIPTION
  This function will return current cho
  procedure's sub state.
DEPENDENCIES
  None

RETURN VALUE
  return current cho sub state.
SIDE EFFECTS
none
===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
uint8  rrccho_get_current_substate(void)
{
  return (uint8)rrccho_substate;
}
/*===========================================================================
FUNCTION   rrccho_return_cho_status

DESCRIPTION
returns cho status

DEPENDENCIES
  None.

RETURN VALUE
rr_interrat_cc_order_status_e_type


SIDE EFFECTS
  None.
===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif

uint8 rrccho_return_cho_status(void)
{
  return (uint8)gtow_cho_info.status;
}

/*===========================================================================
FUNCTION   rrccho_return_cho_failure_reason

DESCRIPTION
returns cho failure reason

DEPENDENCIES
  None.

RETURN VALUE
rrc_interrat_reject_reason_T

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif

uint8 rrccho_return_cho_failure_reason(void)
{
  return (uint8)gtow_cho_info.failure_reason;
}
