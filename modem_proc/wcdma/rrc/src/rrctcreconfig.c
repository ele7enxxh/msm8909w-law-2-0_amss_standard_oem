/*===========================================================================
              R R C TRANSPORT CHANNEL RECONFIGURATION  M O D U L E 

DESCRIPTION

  This module performs the Transport Channel Reconfiguration Procedure. This
  module processes the TCRC Message and reconfigures the transport channels.  
  
  
EXTERNALIZED FUNCTIONS

  rrctcrc_procedure_event_handler   Event handler for this procedure
  rrctcrc_init_procedure            Power-up initialization of the procedure

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Procedure must be initialized at power-up by calling rrctcrc_init_procedure. 
  
Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrctcreconfig.c#1 $   

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/16   sp      Made changes to reset CU registration info once reconfig procedure is successful
06/26/16   sg      Made changes to reduce F3s
10/19/15   sp      Made changes to trigger OOS if L2 ack for transition to PCH is received when inter freq resel is in progress
10/16/15   as      Added code to support LTA durting activation time
04/23/15   vi      Made changes to add DMCR IE only when DMCR is supported in SIB3
09/03/14   sp      Made changes to save procedure substates in crash debug info
08/08/14   ad      Made changes to trigger reconfiguration failure after cell update
                   procedure gets finished due to SIB5 change
07/23/14   sp      Made changes to remove F3s as part of FR19999
06/19/14   sg      Made changes to revert the cipher config db if cipher config type is
                   updated to step1 but later message validation failed.
06/20/14   db      Mainlined FEATURE_WCDMA_HS_RACH_DELAY_CU and introduced NV control
06/17/14   sp      Made changes to mainline RLC-restablihment redesign changes
06/02/14   sp      Made changes for FR19997 - UL rlc re-establishment redesign
04/10/14   db      Made changes to detect UL size change during FACH to FACH redirection
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/14/14   sp      Made changes to init re-establish entity on Procedure cleanup
01/16/14   db      Made changes to clear CRNTI when UE camps back on directed cell when dch to fach timer expires
01/09/14   sp      Removed code under FEATURE_CS_VOICE_OVER_HSPA feature
12/24/13   sp      Made changes to reset reestab entity at procedure cleanup
12/13/13   db      Made changes to delay Cell Update when HSRACH Capability changes until Uplink Data Arrives 
12/09/13   sp      Made changes for OSYS 6.6.5 migration
10/18/13   ad      Revert changes to init the reestablish entity during RL failure 
                   and also during clearing procedure
10/10/13   ad      Made changes to init the reestablish entity during RL failure 
                   and also during clearing procedure
09/29/13   db      Made changes to update failure status of procedure upon registration with CU and OC reset
09/16/13   mp      Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
08/14/13   vg      Made changes to implement a new diag event EVENT_WCDMA_RRC_FAILURE
12/08/13   sp      Made changes to correct the mismatch in logging of version of rb reconfig message.
07/19/13   db      Made changes to Swap OC and CC after channel config by CU when
                   a procedure is registered and it fails
06/27/13   gv      Made changes to buffer the MM and RR commands for crash debugging
04/25/13   sg      Added code for REL9 under FEATURE_WCDMA_REL9
01/03/13   ad      Made changes to reduce the redundant F3s
10/31/12   ad      Made changes not to send channel_config from procedure based on 
                   TOC usage flag toc_usage_change_oos_to_dch  
10/26/12   db      EFACH Redesign changes
10/04/12   md      Made changes to detect RLC size change during state transitions via CELL PCH state.
10/04/12   pm      Made sure SRB's are present in Chan_config_req during FACH to DCH transition
09/28/12   ad      Made changes to send reconfiguration complete if cell update confirm
                   takes us to PCH and complete message is yet to be sent to network.
                   Made changes to stop HSDPA when CU is taking from FACH to PCH and
                   OC is not set by CU
08/28/11   ad      Made changes to send channel config request when UE receives cucnf
                   after coming back to service on dch to fach or pch transition     
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/29/12   ad      Made changes for triggering CU if E-RNTI is not given.
                   Skipped the de-registration of WMs if HS-RACH is ON and RRC is 
                   triggering MR while coming out of PCH. Fixed bug to updated E-RNTI
                   in OC correctly. Added code to trigger CU if R99 or R5 CUCNF is received. 
                   Added code to keep L1 and MAC in sync wrt HS-RACH actions.
06/25/12   sa      Made the changes to delete the CRNTI while configuring the CU
04/25/12   as      Made changes to trigger cell update/drx req when UE recived OTA with 
                   redirection info and cell reselection in progress
04/06/12   ad      Changes for HS-RACH feature
04/30/12   db      Added check to swap CC and OC only on DCH to FACH fail
04/04/12   as      Made changes to initiate transition to idle when L2 ACK fails while going to PCH
26/03/12   ad      Added changes to set the TOC for OOS if DCH to FACH timer expiry 
                   happens before OOS indication comes from L1. Also added code to 
                   send channel config request from procedure after recovery from OOS.
12/09/11   ad      Added code to log WCDMA_RRC_PROTOCOL_ERRORS packet
12/08/11   db      Made changes to trigger cell update if H-RNTI is missing 
11/20/11   db      Added changes to wait for TC reccinfig to wait for l2ack in all
                   states not just DCH, include reconfig status indicator in CU
08/17/11   ad      Made changes to send channel config request when UE receives cucnf
                   after coming back to service on dch to fach or pch transition     
05/03/11   rl      Fixed the lint error
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/10/11   ad      Fixed compilation errors
03/09/11   ad      Added changes to handle simultaneous reconfig messages 
                   in fach state while waiting for cell update confirm.
                   Compilation warnings fixed.
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/11/10   prk     Removed the code which do double freeing of the memory.
09/17/10   su      Featurizing MVS related code
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
07/14/10   su      Fixed Lint errors.
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
06/10/10   prk     Made changes to wait for L2 ACK after sending reconfiguration
                   failure message if the failure is due to the lower layer configuration.
06/07/10   ss      Added code to reset SVTHHO in case of failure message
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
03/12/10   dm      Added code for supporting Deferred SIB reading feature.
03/15/10   as      Updated RRC Code to support ASN.1 8.9 version
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
12/21/09   gkg     Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
09/25/09   gkg/ps  Made changes to not intiate cell selection for FACH->FACH or
                   FACH->PCH redirection, if cell reselction is ongoing.
06/05/09   ps      Made changes to not start the T305 timer if value is infinity
05/21/09   gkg     Made changes to correct OTA version which triggers Cell_FACH
                   to Cell_PCH Freq Redirection through R7 TCRC. 
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
04/09/09   ps      Made changes to send reconfig ind in CU/or go to idle
                   if RL failure is triggered while l2 ack for reconfig complete
03/28/09   rm      CMI Featurisation
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
01/15/09   gkg     Made changes to initialize rlc_size_change_in_progress DB while 
                   handling Channel Config Confirm from LLC.
12/17/08   gkg     When RLC RESET or RL Failure happens during DCH -> FACH transition, 
                   made changes so that procedure triggers Cell Update after receiving 
                   successful channel config cnf.
12/08/08   ps      Made changes to call mvssup_set_voc_timing() for Inter-Freq
                   HHO/Timing Initialised HHO and Forced SYNCA
11/12/08   gkg     Made Changes so that T305 is started by the Reconfig procedure
                   as soon as Reconfig procedure initiates Cell Selection. This is
                   done for Cell_DCH -> Cell_FACH state.
11/10/08   ps      Added the check for dl_counter_sync info presence for R7 path also
11/04/08   rm      Added support for rove-in to GAN in RRC Connected mode.
                   Code changes are under FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE.
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
06/13/08   gkg     Added code for notification of Cell_Id change through OTAs
                   in Cell_DCH state. 
06/13/08   gkg     Made changes so that if RLC PDU size changes for RBs through
                   Transport Channel IEs of TC-Reconfig OTA, those RBs are reconfigured
                   and therefore their RLC entity is notified for new RLC PDU size.
05/30/08   gkg     In case of Cell_FACH->Cell_DCH state transition, stop T305
                   if RRC_CHANNEL_CONFIG_REQ succeeds otherwise handle timer T305
                   by initiating Cell Update.
03/06/08   da      Added MBMS phase 2C changes.  MBMS support in CELL_DCH.  
                   Includes mobility and dynamic resource management.
11/02/08   gkg     Made changes so that whenever there is a mismatch in what is
                   expected in an OTA msg and what is received, always INVALID 
                   Config is returned instead of UNSUPPORTED Config.
01/08/08   ps/gk   Made changesto support RAB type change for SCUDIF
                   through TC Reconfig msg
11/28/07   da      In case OOS area gets detected during DCH->FACH transitions,
                   made change to setup DCCH+DTCH once returning in service before
                   sending CU message with cause "re-entered service area"
10/23/07   kp      Added support for network sharing feature. Code changes to handle 
                   primary PLMN I.D in OTA message. Changes are under feature flag 
z                   FEATURE NETWORK SHARING.
10/08/07    da      Made change to pass correct proc id when registering with scm.
09/25/07    ps     Added fix to send DRX Req to L1, when Redirection IE is
                   given for the camped PSC, in FACH->PCH case. This is valid
                   when FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU is enabled.
09/10/07    ps     Added support for Frequency redirection received in 
                   FACH->FACH and FACH->PCH cases under feature flag
                   FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
08/08/07    ps     Added support for feature cell id reading in DCH state, 
                   under feature flag FEATURE_WCDMA_GPS_CELL_ID_READING.
04/20/07    da     Made changes needed to update ASN.1 version to R6 sept R6 2006
03/02/06    ps     Added support for Wb-AMR under the feature flag
                   FEATURE_VOC_AMR_WB.
02/07/07    ps     Made changes to handle Standalone Rb-Mappings
02/02/07    da     Removed code that use to handle DCH->FACH activation time
                   and 300ms delay.  functionality has been moved to LLC. 
12/22/06    ps     Made changes to handle postVerificationPeriod and the
                   timingMaintainedSynchInd IE received in OTA messages
09/27/06    sm     Corrected FEATURE_WCMDA_HSUPA to FEATURE_WCDMA_REL6
09/12/06   sm/da   Fix code review comments and general clean-up for REL6 and HSUPA
09/11/06    da     Added support for FEATURE RLC ONE SIDE RE ESTABLISHMENT
09/05/06    da     Added new func rrctcrc_get_new_crnti_valid().  This function
                   will be called when CU wants to know if the OTA msg
                   included a new CRNTI.  Also made change not to reset 
                   new_crnti_valid flag when proc is waiting for CU CNF and 
                   OOS has been detected.  This is because proc needs to send
                   a RNTI update req.
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/28/06   da      Added support for OOS area handling during transition from DCH.
07/14/06   da      Change made to call func rrc_send_rb_rate_info_packet() from 
                   send_tcrc_complete_message rather than tcrc_process_rnti_update_cnf.
06/13/06   segu    Added support for DSAC future implementation
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE REL5 while retaining the HSDPA specifc processing 
                   under FEATURE HSDPA.
04/25/06   da      Added checking the status of RNTI_UPDATE_CNF.  If failed, 
                   send failure to TCRC msg and clear procedure.
03/14/06   tkk     Lint warnings/errors cleanup
03/10/06   da      Added Initial support for PDCP under FEATURE_UMTS_PDCP
02/23/06   ss/tkk  rrcrb_update_amr_cctrch_id_for_reconfig is called only
                   if lower layers have been successfully configured.
02/15/06   sgk     Added code to process channel config cnf with config
                   cancel and to handle the cases where cu is in pending
                   config state.
12/22/05   tkk     Added support to handle CU (for certain causes) while 
                   reconfig procedure is waiting for L2 ack to go to 
                   Cell_PCH/URA_PCH from Cell_FACH.
12/19/05   da      Calling rrc_send_rb_rate_info_packet() to log rb rate info.
                   For FACH->CELL_PCH case, UE shall not do cell update if directed cell
                   info matches with current camped cell.  Else, CU is required.
12/19/05   sgk     Added function to validate if SRNS relocation info is present
                   for the rel 5 message. 
12/01/05   da      If activation time for DCH-FACH transition is greater than 300 seconds,
                   make delay before cell selection as long as activation time.
11/10/05   da      Removed extern definition of nv_rel_ind;
11/10/05   da      Checking global flag rrc_nv_rel_indicator before doing R5 related
                   processing.
10/26/05   vr      Send UL-Counter Synch info in the TCReconfigComplete message
                   only if TM ciphering is still active.Also corrected the bitmask
                   names
09/26/05   ss      added support for mac-d-hfn
07/25/05   ss      added explicit registration for Cell update cnf when L2 NACK is received.
06/15/05   da      Sending rb failure msg with cause "cell update occured" when OOS area
                   occurs while RB proc is waiting for l2 ack.  
                   Also passing indication to func rrcsend_send_ul_sdu to not send L2 ack
                   to procedure when procedure is not waiting for l2 ack.  
                   Added fix for GCF where UE was sending failure message [due to later_than_r3
                   msg chosen] with wrong transaction ID.
06/06/05   da      Calling func rrcllc_print_trans_chnl_rate() after sending complete
                   msg to print trans chan rates.
05/16/05   da      Added support for validation failure handling from L1
04/15/05   da      Added support for Phase 2 of dch failure handling: going back
                   to old configuration.
03/25/05   da      Removed FEATURE CM SUPPORTED from rrctcrc_process_channel_config_cnf.
                   Had an invalid check that looks if failure was due to compressed mode and send
                   tc reconfig failure msg with cause invalid config.
03/11/05   vk      Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE HSDPA
01/04/05   da      Added Support for phase 1 of Physical channel establishment 
                   failure handling within DCH. 
12/15/04   da      Added code to reset failure bit mask to 0.
11/30/04   vm/da   Added appropriate MSG_HIGHs for new RRC/NAS interface 
                   RRC_SERVICE_IND_FROM_OTA_MSG.
11/22/04   da      Added support for phy chan setup fail and phy chan est fail in 
                   CELL_FACH->DCH from a cell update cnf msg and rb setup/reconfig.
10/21/04   da      Removed FEATURE_CELLPCH_URAPCH.  CELLPCH and URAPCH states should always
                   be handled.  Deleted code that handles case when CELLPCH and URAPCH is not
                   defined.
10/19/04   da      Removed FEATURE SRNS RELOCATION.  SRNS_RELOCATION should always be 
                   handled.  Deleted code that handles case when SRNS_RELOCATION is not
                   defined. 
09/14/04   sk      Added support to save CN info in mm_cmd_buf and later send the
                   command to MM when procedure is succefully completed 
                   Set the failure cause to "protocol error" with 
                   "rrc_messageExtensionNotComprehended" when msg choice is later_than_r3
08/30/04   sgk     Assigned tcrc.status.failure_status to RRCTC_MSG_INVALID_CONFIG
                   instead of RRCRB_MSG_INVALID_CONFIG in function
                   validate_tcrc_message to fix lint error Type mismatch 
                   (assignment) (enum/enum).
                   Typecast c_rnti to uint16 in function process_tcrc_message
                   to fix lint error: Loss of precision (assignment)
                   (32 bits to 16 bits).
                   Removed  parameter rrc_rnti_update_cnf_type *ptr from function
                   rrctcrc_process_rnti_update_cnf and corrected it at the calling
                   location to fix lint error Symbol 'ptr' not referenced. 
                   Checked the return value of the function with FAILURE and if so,
                   printed an ERR msg , set failure cause to invalid config and
                   retuned from the function ,in function validate_tcrc_message,
                   checked if the function
                   'rrcllc_reset_ordered_config(enum {...})' return != OC_NOT_SET
                   and if so printed a Msg high in function rrctcrc_process_l2_ack_ind
                   to fix lint error Ignoring return value of function
                   'rrcsmc_process_cipher_config_for_hho(void)'.
08/20/04   jh(kc)  Added call to rrcrb_update_amr_cctrch_id_for_reconfig().
08/09/04   kc      Complete support for RB-Re-est of TM-bearers when transitioning from
                   non-DCH state to DCH state.
07/14/04   sk      RL failure/RLC reset handling
                   Handle state change while waiting for l2ack.
                   Removed the local substate enum type. modified the code to use
                   the rrbrb_substate_e_type declared in rrcrbcommon.h
                   Added an API to get the procedure current substate.
05/18/04   sk      Added a check for the state indicated by the message in 
                   state handling in process_channel_config_cnf()
05/04/04   sk      Added Freq/Scr handling in reconfiguration for CELL_FACH->CELL_FACH
                   transition.
                   Changed rrcrb_send_rnti_update_req() input parms.
03/04/04   ttl     Added FEATURE_CELLPCH_URAPCH for code of Cell_PCH/URA_PCH.
02/03/04   sk      Added Cell-PCH/URA_PCH support.
                   clear the ordered_config variable immediately after sending response
                   message, when SRNS relocation is not involved and next state is 
                   not cell_PCH or URA_PCH. 
                   Handle l2ack in  substates otherthan waiting_for_l2_ack substate.
                   SRNS relocation support in tcreconfig complete message triggered by 
                   CU proc
12/10/03   bu      Removed the calls to clear_ordered_config(). It will be called
                   only when L2 Ack is received. Also removed handling of NEW_CELL_IND
                   as it will never be received.
11/18/03   sk      Go to idle if L2ack for Tcrc complete message fails and 
                   the procedure involves SRNS relocation. Else do not take
                   any action on L2ack failure.
11/17/03   kc      Added SRNS support in RB-TCReconfig procedure.
08/04/03   vn      Initialize new fields for RLC re-establishment in RNTI_UPDATE_REQ.
03/26/03   sk      Added functionality to process Reconfig message going from
                   CELL_DCH to CELL_FACH, that comes with a new C-RNTI. If
                   camped on the same cell as directed and if C-RNTI is
                   available, do not initiate Cell Update procedure. 
                   Added  support for CELL_DCH -> CELL_FACH transition
                   Added  RRCTCRC_WAIT_FOR_CELL_SELECTION_CNF substate
                   Added rrctcrc_other_wfcscnf_substate_event_handler()
                   Added rrctcrc_fach_wfcscnf_substate_event_handler()
                   Added RRCTCRC_WAIT_FOR_CELL_SELECTION_CNF case to 
                   rrctcrc_cell_fach_event_handler()
                   No parameters for rrctcrc_send_channel_config_req()
03/18/03   sk      Modified the transaction Id checking in process_tcrc_message()
                   Changed  rrcrb_msg_failure_cause_type to rrctc_msg_failure_cause_type
                   in  rrctcrc_int_data_type and changed the enum values 
                   accordingly.
02/05/03   vn      Added processing for State Change to Idle when waiting
                   for Cell Update Confirm.
02/05/03   vn      Merged changes dropped during linting.
           vn      Added new parameter proc_id in rrcllc_set_ordered_config.
10/24/02   rj      Updated to use rrc_malloc and rrc_free to use dual heaps 
                   in place of mem_malloc and mem_free to avoid memory
                   fragmentation
09/26/02   xfg     Changed for WVS to MVS migration. The change was made under
                   FEATURE_MVS.
09/25/02   bu      Changed l1_status to chan_cfg_status under
                   FEATURE CM SUPPORTED.
09/20/02   bu      If reselection is in progress or if cell update has started
                   and register for CU CNF accordingly. On receiving cell update
                   confirm send Transport Channel Reconfig failure to UTRAN.
                   If C_RNTI is not valid register for CU CNF. On receiving
                   CU CNF send the TC Reconfig Complete. Call
                   rcllc_get_ordered_config_status() instead of
                   rrcllc_is_ordered_config_set().
08/28/02   vk      Processed Compressed Mode failure from LLC and send a
                   Transport Channel Reconfig Failure message to UTRAN with error
                   as Invalid Configuration. Changes have been put under
                   FEATURE CM SUPPORTED
08/15/02   upn     Fixed CELL_FACH unsupported config flag.
06/25/02   upn     Code implemented for state CELL_FACH support.
06/06/02   upen    Added definition for rrctcrc_build_tcrc_complete_message
08/06/01   upen    Created file.

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
#include "mmtask.h"
#ifdef FEATURE_CMI
#include "mmtask_v.h"
#endif
#include "rex.h"
#include "rrcasn1.h"
#include "rrcasn1util.h"
#include "rrccmd_v.h"
#include "rrcdata_v.h"
#include "rrcintif.h"
#include "rrcccm.h"
#include "rrccu.h"
#include "rrccui.h"
#include "rrclcm.h"
#include "rrcllc.h"
#include "rrcmmif.h"
#include "rrcrbcommon.h"
#include "rrctcreconfig.h"
#include "rrcscmgr.h"
#include "rrcsend.h"
#include "rrccspi.h"
#include "rrccsp.h"
#include "rrctmr.h"
#include "rrcsibproc.h"
#include "rrclog.h"

#ifdef FEATURE_MVS
#include "mvs.h"
#endif  /* FEATURE_MVS */
#include "rrcsmc.h"
#include "rrcmisc.h"


#include "rrcmisc.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif

#include "rrcdormancy.h"
#include "rrclogging.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Enumerated type of the different substates for this procedure           */


typedef struct {
  rrctc_msg_status_e_type failure_status; /* Failure cause                 */
  rrc_ProtocolErrorCause  prot_err_cause; /* Protocol Error cause          */
}rrctc_msg_failure_cause_type;


#define RRCTC_INVALID_TRANSACTION_ID  10

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
/* We need to introduce a delay before transmitting
on a new configuration if the base-station cannot have
the old link and new link up at the same time. */
#define RRCTCRC_TEMP_TIMER_SIG 0x4001
rex_sigs_type rrctcrc_sig;
rex_timer_type rrctcrc_temp_timer;

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* Structure to store all internal data related to TCRC procedure           */
typedef struct {
  rrcrb_substate_e_type      curr_substate; /* Store the current substate     */
  rrcrb_transaction_id_type    tr_ids;      /* Transaction ids for RR msg    */
  rrc_state_e_type             state_indicated_by_msg; /* RRC State given by RR msg     */
  rrctc_msg_failure_cause_type status;   /* Status of message validation   */
  boolean                      new_crnti_valid; /* Indicates if a valid C-RNTI was recd 
                                          * in the message transitioning from CELL_DCH
                                          * to CELL_FACH
                                          */
  uint16                       new_crnti;
  boolean                      directed_cell_info; /* Indicates if a valid C-RNTI was recd */

  rrc_int_u_rnti_type          new_urnti;
  boolean                      new_urnti_valid;          /* Indicates if a valid U-RNTI was recd */ 
  boolean                      tcrc_for_srns_relocation;  /* Indicates SRNS reloc involved with 
                                                          this msg                             */

  boolean                       hho_ciph_status;


  boolean                       cn_info_saved;           /*flag to indicate that CN info is saved */
  mm_cmd_type *                 mm_cmd_ptr;              /*pointer to hold mm_cmd with cn info */

  boolean                      trans_to_dch_ciph_config_needed; /*indicates that we are going from
                                                                  non-dch to dch and needs ciph
                                                                  config when tm bearers are present
                                                                  */
  /* Indicates if Fre-redirection is to be triggered in FACH->PCH case */
  boolean initiate_cell_selection_trans_from_fach;


  boolean high_mobility_ind;

}rrctcrc_int_data_type;

LOCAL rrctcrc_int_data_type tcrc;

boolean rrctcrc_send_initiate_cu_flag = FALSE;
boolean rrctcrc_delete_crnti = FALSE;
#ifdef FEATURE_QSH_EVENT_METRIC
extern rrc_qsh_ul_failure_ota_param_s failure_ota;
extern rrc_qsh_metric_db_type rrc_qsh_metric_db[WRRC_QSH_METRIC_MAX+1];
extern boolean rrc_qsh_dl_committed;
#endif



/* =======================================================================
**                            Function Definitions
** ======================================================================= */
/*===========================================================================

FUNCTION   RRCTCRC_IS_SRNS_IN_PROGRESS()

DESCRIPTION
 This function returns if SRNS is in progress with current reconfig procedure


        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrctcrc_is_srns_in_progress
(
  void
)
{
  return tcrc.tcrc_for_srns_relocation;
}
/*===========================================================================

FUNCTION   RRCTCRC_GET_NEW_CRNTI_VALID()

DESCRIPTION
 This function returns if reconfig msg has valid CRNTI


        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrctcrc_get_new_crnti_valid
(
  void
)
{
  return tcrc.new_crnti_valid;
}
/*===========================================================================

FUNCTION   RRCTCRC_GET_CURRENT_SUB_STATE()

DESCRIPTION
 This function is used to return current substate


        
DEPENDENCIES

  None.
 
RETURN VALUE

  rrcrb_substate_e_type current_substate.

SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
rrcrb_substate_e_type rrctcrc_get_current_sub_state( )
{
  return tcrc.curr_substate;
}
 /*===========================================================================

FUNCTION   rrctcrc_get_dest_state()

DESCRIPTION
 This function returns state indicated by message


DEPENDENCIES

  None.

RETURN VALUE

  rrc_state_e_type 

SIDE EFFECTS

  None.

===========================================================================*/
rrc_state_e_type rrctcrc_get_dest_state(void)
{
  return tcrc.state_indicated_by_msg;
}

/*===========================================================================

FUNCTION   rrctcrc_is_proc_successful()

DESCRIPTION
 This function is used to return failure status
DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean rrctcrc_is_proc_successful( )
{
  if(tcrc.status.failure_status == RRCTC_MSG_VALID)
    return TRUE;
  else return FALSE;
}


/*===========================================================================

FUNCTION   rrctcrc_update_failure_status()

DESCRIPTION
  Updates the failure status
DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
void rrctcrc_update_failure_status(rrctc_msg_status_e_type status )
{
  tcrc.status.failure_status = status;
  WRRC_MSG1_HIGH("Updated Failure status to %d",status);
  return;
}


/*===========================================================================

FUNCTION rrctcr_append_start_list_to_tcrc_complete
DESCRIPTION
  This function appends start list to tcrc message for SRNS relocation and
  also when TM bearers are present during non-dch to dch rrc-state
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
static void rrctcr_append_start_list_to_tcrc_complete
(
rrc_UL_DCCH_Message *msg_ptr               /* Pointer to Uplink DCCH Msg */
)
{
  /*start list is included in ul_counter_synchronization_info. So enable this IE in the
  bitmask */
  RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.transportChannelReconfigurationComplete, 
    rrc_TransportChannelReconfigurationComplete,ul_CounterSynchronisationInfo);
  RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.transportChannelReconfigurationComplete.ul_CounterSynchronisationInfo);
  
  
   /* call SMC API to append the start list */
  (void)rrcsmc_append_start_list(&msg_ptr->message.u.transportChannelReconfigurationComplete.
                                          ul_CounterSynchronisationInfo.startList,
                                          RRCSMC_EXTRACT_AND_APPEND_START);
}/*rrctcr_append_start_list_to_tcrc_complete*/
/*===========================================================================

FUNCTION   SEND_TCRC_FAILURE_MSG

DESCRIPTION

  This function sends the Transport Channel Reconfiguration Failure message
  on Uplink DCCH. The failure cause must be given to this
  function. If the failure cause is "Protocol Error" the
  protocol error cause must also be given to this function.

        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/     
static void send_tcrc_failure_message
(
  rrctc_msg_status_e_type failure_cause,  /* Failure Cause - should be a subset
                                          of the failure causes specified by
                                          the spec. */
  rrc_ProtocolErrorCause  prot_err_cause  /* Protocol Error cause. This is checked
                                          only if the Failure cause is set to 
                                          "Protocol Error". */
)
{
  
  rrc_UL_DCCH_Message *ptr;     /* Pointer to uplink DCCH message type */
  rlc_lc_id_type ul_lc_id;      /* Logical Channel Id for the message  */ 
  rrcsend_status_e_type status; /* Status from send chain module       */

  /* Allocate memory for this message. Note that this buffer is released
  by the SEND CHAIN */
  ptr = (rrc_UL_DCCH_Message *)rrc_malloc( 
                                          sizeof(rrc_UL_DCCH_Message));
    WRRC_MSG2_HIGH("tcrc subsstate(%d) Send failure with %d cause:[0-VALID|1-UNSUPPORTED_CONFIG|2-PHY_CHAN_FAILURE|3-SIMULTANEOUS_RECONFIG|4-PROTOCOL_ERROR|5-INVALID_CONFIG|6-INCOMPLETE_CONFIG|7-CELL_UPDATE_OCCURED]", 
   tcrc.curr_substate, tcrc.status.failure_status);

    /* We need to revert cipher config db, if cipher config db has been updated with step-1 HO. 
     This revert takes care of the scenario of OTA failure in the gap between OC is 
     set and cphy is sent. This cannot happen in a normal case. This is for extra protection */
  rrcsmc_revert_to_old_config_for_failure_message(FALSE);

  RRC_RESET_MSG_IE_PRESENT_PTR(ptr); /* Integrity Check info not present */
  ptr->message.t = T_rrc_UL_DCCH_MessageType_transportChannelReconfigurationFailure; /* RR Failure msg */
  
  RRC_RESET_MSG_IE_PRESENT(ptr->message.u.transportChannelReconfigurationFailure);
          
  /* Transaction Id - First check the rejected_transacion id. If
  it's not invalid, this message is being rejected because there
  is already another message accepted but not processed.
  If the rejected_transaction id is invalid, we are rejecting the
  accepted message, so use the accepted transaction id.

  NOTE: Don't change the order, it's important to check rejected
  transaction first. */
  if( tcrc.tr_ids.rejected_transaction != RRCTC_INVALID_TRANSACTION_ID )
  {
    
    ptr->message.u.transportChannelReconfigurationFailure.rrc_TransactionIdentifier =
      tcrc.tr_ids.rejected_transaction;
    /* We no longer need to keep this id. Make rejected_transaction id
    invalid again */
    tcrc.tr_ids.rejected_transaction = RRCTC_INVALID_TRANSACTION_ID;
  }
  else if( tcrc.tr_ids.accepted_transaction != RRCTC_INVALID_TRANSACTION_ID )
  {
    ptr->message.u.transportChannelReconfigurationFailure.rrc_TransactionIdentifier =
      tcrc.tr_ids.accepted_transaction;
    /* We no longer need to keep this id. Make rejected_transaction id
    invalid again */
    tcrc.tr_ids.accepted_transaction = RRCTC_INVALID_TRANSACTION_ID;
  }
  else
  {
    /* Don't know the transaction id for this message.( one reason
    for this could be that the message had a critical
    extenstion). Since it's unknown, we should set it to zero. */
    ptr->message.u.transportChannelReconfigurationFailure.rrc_TransactionIdentifier= 0;
    WRRC_MSG0_HIGH("Transaction id invalid for TCRC Failure message!");
  }

  switch( failure_cause )
  {
    case RRCTC_MSG_UNSUPPORTED_CONFIG:
      ptr->message.u.transportChannelReconfigurationFailure.failureCause.t =
        T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      break;

    case RRCTC_PHY_CHAN_FAILURE:
      ptr->message.u.transportChannelReconfigurationFailure.failureCause.t =
        T_rrc_FailureCauseWithProtErr_physicalChannelFailure;
      break;

    case RRCTC_SIMULTANEOUS_RECONFIG:
      ptr->message.u.transportChannelReconfigurationFailure.failureCause.t =
        T_rrc_FailureCauseWithProtErr_incompatibleSimultaneousReconfiguration;
      break;

    case RRCTC_MSG_PROTOCOL_ERROR:
      ptr->message.u.transportChannelReconfigurationFailure.failureCause.t =
        T_rrc_FailureCauseWithProtErr_protocolError;

	    ptr->message.u.transportChannelReconfigurationFailure.failureCause.u.protocolError =
  	  	rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation);
      

      /* When there is a Protocol Error, we also have to indicate
      the protocol error cause */
      ptr->message.u.transportChannelReconfigurationFailure.failureCause.u.protocolError->
        diagnosticsType.t = T_rrc_ProtocolErrorInformation_diagnosticsType_type1;
    
	    ptr->message.u.transportChannelReconfigurationFailure.failureCause.u.protocolError->
  	  	diagnosticsType.u.type1 = rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation_type1);

      
      /* Copy the given protocol error cause */
      ptr->message.u.transportChannelReconfigurationFailure.failureCause.u.protocolError->
        diagnosticsType.u.type1->protocolErrorCause = prot_err_cause;
      break;

    case RRCTC_MSG_INVALID_CONFIG:
      ptr->message.u.transportChannelReconfigurationFailure.failureCause.t =
        T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      break;

    case RRCTC_MSG_INCOMPLETE_CONFIG:
      ptr->message.u.transportChannelReconfigurationFailure.failureCause.t =
        T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      break;

    case RRCTC_CELL_UPDATE_OCCURED:
      ptr->message.u.transportChannelReconfigurationFailure.failureCause.t =
        T_rrc_FailureCauseWithProtErr_cellUpdateOccurred;
      break;

    default:
      WRRC_MSG0_HIGH("Sending TCRC Unknown Failure message");
      ptr->message.u.transportChannelReconfigurationFailure.failureCause.t =
        T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      break;
  }

#ifdef FEATURE_QSH_EVENT_METRIC

    if(rrc_qsh_dl_committed == FALSE)
    {
      failure_ota.ul_ota_type = RRC_OTA_UL_FAILURE;
      failure_ota.msg_id = WRRC_UL_DCCH_MESSAGETYPE_TRANSPORTCHANNELRECONFIGURATIONFAILURE;
      if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status())
      {
        failure_ota.ul_fail_ota.ul_ota_parms.ul_integrity_info_present = 1;
      }
      failure_ota.ul_fail_ota.ul_ota_parms.failure_cause = ptr->message.u.transportChannelReconfigurationFailure.failureCause.t;
      /* Commit data should happen after dl SDU is committed */
    }
	else
	{
	 if(rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr != NULL)
     {
      if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status())
      {
   	    ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.ul_ota_parms.ul_integrity_info_present= 1;
      }
      ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.ul_ota_parms.failure_cause= ptr->message.u.transportChannelReconfigurationFailure.failureCause.t;   
      /* Commit data will be called in send ul sdu */
	 }
	}
#endif  
        
  rrc_log_protocol_error( (uint8) (rrc_get_state()),
                          (uint8) RRC_PROCEDURE_TCR,
                          (uint8) ptr->message.u.transportChannelReconfigurationFailure.failureCause.t,
                          (uint8) prot_err_cause
                        );

  /* Get the RLC logical channel id for this msg from Logical Channel Manager ??*/
  ul_lc_id = rrclcm_check_ul_rlc_lc_id ( UE_LOGCHAN_DCCH,
                                         DCCH_AM_RADIO_BEARER_ID,
                                         UE_MODE_ACKNOWLEDGED_DATA);

  /* If the logical channel id is not invalid, send the message to the
  send chain module */
  if(ul_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    WRRC_MSG1_HIGH("Sending TBRC Failure msg to send-chain Cause:%d",
             ptr->message.u.transportChannelReconfigurationFailure.failureCause.t);

    /* Need to wait for L2 ack for all states because RRC needs to trigger include reconfig status indicator
    if RLC acknowledgement is not yet received.
    As per 8.3.1.3
    1> if the IE "Cell update cause" is set to "radio link failure" and the UE has submitted a reconfiguration response
    message to RLC and the UE has not received the RLC acknowledgement for the response message:
    2> include the IE "Reconfiguration Status Indicator" and set it to TRUE.*/
    if(tcrc.status.failure_status == RRCTC_PHY_CHAN_FAILURE)
    {
      rrc_update_reconfig_waiting_for_l2ack_dch(RRC_PROCEDURE_TCR);
      status = rrcsend_send_ul_sdu(RRC_PROCEDURE_TCR,  /* Procedure is TCRC    */
                                   (void *)ptr,        /* Message pointer     */
                                   ul_lc_id,           /* RLC id              */
                                   RRCSEND_L2ACK_REQUIRED  /* ACK is required */
                                  );
    }
    else
    {
      status = rrcsend_send_ul_sdu(RRC_PROCEDURE_TCR,  /* Procedure is TCRC    */
                                   (void *)ptr,        /* Message pointer     */
                                   ul_lc_id,           /* RLC id              */
                                     RRCSEND_L2ACK_NOT_REQUIRED  /* No need for ACK */
                                  );
    }
    if( status != RRCSEND_SUCCESS )
    {
      WRRC_MSG1_ERROR("Send chain failed for TCRC failure msg: error cause %d", status);
    }
  }
  else
  {
    /* Unexpected failure. ERR for now. May need to ERR_FATAL here */
    WRRC_MSG1_ERROR("Can't send UL DCCH msg! invalid lc id: %d", ul_lc_id );
    /* It's important to free the memory since we didn't send the
    message. */
    rrc_free( ptr );
  }
}/* end function send_tcrc_failure_message */



/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */


/*===========================================================================

FUNCTION   SEND_TCRC_COMPLETE_MSG

DESCRIPTION

  This function sends the TCRC Complete message
  on Uplink DCCH. 
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/     
static void send_tcrc_complete_message( boolean proc_wait_for_l2_ack )
{
  
  rrc_UL_DCCH_Message *ptr;     /* Pointer to uplink DCCH message type */
  rlc_lc_id_type ul_lc_id;      /* Logical Channel Id for the message  */ 
  rrcsend_status_e_type status; /* Status from send chain module       */
#ifdef FEATURE_QSH_EVENT_METRIC
  rrcllc_oc_set_status_e_type oc_status = rrcllc_get_ordered_config_status();
#endif
  /* Allocate memory for this message. Note that this buffer is released
  by the SEND CHAIN */
  ptr = (rrc_UL_DCCH_Message *)rrc_malloc( 
                                          sizeof(rrc_UL_DCCH_Message));
  if (proc_wait_for_l2_ack)
  {
     WRRC_MSG0_HIGH("Waiting for L2 ACK for TCRC Complete msg");
  }
  
  
  RRC_RESET_MSG_IE_PRESENT_PTR(ptr); /* Integrity Check info not present */
  /* TCRC Complete msg */
  ptr->message.t = T_rrc_UL_DCCH_MessageType_transportChannelReconfigurationComplete; 

  /* Fill up the message contents. The bit_mask is set to 0 since none
  of the optional IE's are included for now. */
  RRC_RESET_MSG_IE_PRESENT(ptr->message.u.transportChannelReconfigurationComplete); 

  /* If SRNS relocation was triggered as a result of this message then 
  append the start list */
  if (tcrc.tcrc_for_srns_relocation == TRUE)
  {
    rrctcr_append_start_list_to_tcrc_complete(ptr);
  }
  else
  {
    if (rrcllc_get_rlc_size_change_status() 
      && ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH || 
             tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
          || (rrc_get_rlc_size_change_status_for_procedure_id(RRC_PROCEDURE_TCR) == TRUE)))
    {
      // Check if next state is CELL+PCH, and OC API returns RLC size change, then include counter sync
      if (tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH || 
          tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
      {
        /* Ask SMC to do start value calculation for PS domain */
        WRRC_MSG0_HIGH("RLCSIZEINDICATED Invoking SMC extract func");
        (void)rrcsmc_extract_return_hfn_per_domain_and_update_start_flag(RRC_PS_DOMAIN_CN_ID);
        
        
      }
      else if (rrc_get_rlc_size_change_status_for_procedure_id(RRC_PROCEDURE_TCR) == TRUE)
      {
        rrc_set_rlc_size_change_status(RRC_PROCEDURE_NONE, 
                                       FALSE, RRC_INVALID_RB_ID ,
                                       RLC_RE_ESTABLISH_NONE);
      }
        /*start list is included in ul_counter_synchronization_info. So enable this IE in the
        bitmask */
      RRC_MSG_COMMON_SET_BITMASK_IE(ptr->message.u.transportChannelReconfigurationComplete,
        rrc_TransportChannelReconfigurationComplete,ul_CounterSynchronisationInfo); 
      
      RRC_RESET_MSG_IE_PRESENT(ptr->message.u.
	  	transportChannelReconfigurationComplete.ul_CounterSynchronisationInfo);
     
      /* call SMC API to append the start list */
      (void)rrcsmc_append_start_list(
        &ptr->message.u.transportChannelReconfigurationComplete.ul_CounterSynchronisationInfo.startList,
        RRCSMC_APPEND_START);
    }
  } 


  //if this is case of IFHHO and not SRNS relocation(i.e. ul counter sync is not present)
  //then also send START_List and set count c act time and configure smc for Step_2
  //also in the case when we move from non-dch to dch sate and TM bearers are active at
  //that time, we need to append start list and send ciph-activation-cfn to network
  if ((( tcrc.hho_ciph_status == TRUE)  || (tcrc.trans_to_dch_ciph_config_needed == TRUE)) &&
      (TRUE == rrcsmc_tm_ciph_active()))
  {
    if(! ( RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.transportChannelReconfigurationComplete,
           rrc_TransportChannelReconfigurationComplete,ul_CounterSynchronisationInfo)))
    {
      rrctcr_append_start_list_to_tcrc_complete(ptr);
    }
  
    //Set the bitmask for count_c_act_time and copy the same..
    RRC_MSG_COMMON_SET_BITMASK_IE(ptr->message.u.transportChannelReconfigurationComplete,
      rrc_TransportChannelReconfigurationComplete,count_C_ActivationTime);
    
    ptr->message.u.transportChannelReconfigurationComplete.count_C_ActivationTime =
      rrcsmc_step_2_cipher_config_for_hho();
  
    //Send step 2 config for HHO.. only one of these flags will be active, reset both of them
    //here
    if(tcrc.hho_ciph_status)
    {
      tcrc.hho_ciph_status =  FALSE;      
    }
    else if(tcrc.trans_to_dch_ciph_config_needed)
    {
      tcrc.trans_to_dch_ciph_config_needed = FALSE;      
    }
  }

  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
  {
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
    if(rrcsib_is_dmcr_ie_needed() == TRUE)
    {
      ptr->message.u.transportChannelReconfigurationComplete.m.laterNonCriticalExtensionsPresent = 1;
 
      RRC_RESET_MSG_IE_PRESENT(ptr->message.u.transportChannelReconfigurationComplete.
             laterNonCriticalExtensions);
    
      ptr->message.u.transportChannelReconfigurationComplete.laterNonCriticalExtensions.
            m.v770NonCriticalExtensionsPresent = 1;
 
      RRC_RESET_MSG_IE_PRESENT(ptr->message.u.transportChannelReconfigurationComplete.
         laterNonCriticalExtensions.v770NonCriticalExtensions);
    
      ptr->message.u.transportChannelReconfigurationComplete.laterNonCriticalExtensions.
       v770NonCriticalExtensions.transportChannelReconfigurationComplete_v770ext.m.
       deferredMeasurementControlReadingPresent = 1;
      
      ptr->message.u.transportChannelReconfigurationComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        transportChannelReconfigurationComplete_v770ext.deferredMeasurementControlReading = 
         rrc_TransportChannelReconfigurationComplete_v770ext_IEs_deferredMeasurementControlReading_true;
    }
#endif
  }

  /* Transaction Id - use the accepted transaction id if known. Else
  use zero. Since this is a TCRC Complete message, we don't check 
  rejected transaction ids.  If procedure is waiting for l2 ack, then delete trans id
  after getting L2ack */
  if( tcrc.tr_ids.accepted_transaction != RRCTC_INVALID_TRANSACTION_ID )
  {
    ptr->message.u.transportChannelReconfigurationComplete.rrc_TransactionIdentifier=
      tcrc.tr_ids.accepted_transaction;
    /* We no longer need to keep this id if state is not RRCRB_WAIT_FOR_L2_ACK. 
       Make rejected_transaction id invalid again */
    if (!proc_wait_for_l2_ack)
    {
      tcrc.tr_ids.accepted_transaction = RRCTC_INVALID_TRANSACTION_ID;
    }
  }
  else
  {
    /* Don't know the transaction id for this message.( one reason
    for this could be that the message had a critical
    extenstion). Since it's unknown, we should set it to zero. */
    ptr->message.u.transportChannelReconfigurationComplete.rrc_TransactionIdentifier = 0;
    WRRC_MSG0_HIGH("Transaction id invalid for TCRC Complete message!");
  }

  /* For now, there is nothing else to include in this message. We
  can give it to the send chain. */

  /* Get the RLC logical channel id for this msg from Logical Channel Manager ??*/
  ul_lc_id = rrclcm_check_ul_rlc_lc_id ( UE_LOGCHAN_DCCH,
                                         DCCH_AM_RADIO_BEARER_ID,
                                         UE_MODE_ACKNOWLEDGED_DATA);

  /* If the logical channel id is not invalid, send the message to the
  send chain module */
  if(ul_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
#ifdef FEATURE_RRC_DELAY_TX_ON_NEW_CFG
    /* Start a timer for 5 seconds, so that the
    base station can enable the new configuration before the
    message is sent. This is needed only if the base station
    cannot have both the old and new configurations up at 
    the same time. */

    WRRC_MSG0_HIGH("Delaying 5 seconds");
    (void) rex_clr_sigs(rex_self(), RRCTCRC_TEMP_TIMER_SIG);
    (void) rex_set_timer(&rrctcrc_temp_timer, 5000);   /* 5 seconds */

    rrctcrc_sig = rex_wait(RRCTCRC_TEMP_TIMER_SIG);
    MSG_MED("RRCTCRC signal->%d", rrctcrc_sig, 0, 0);
    (void) rex_clr_sigs(rex_self(), RRCTCRC_TEMP_TIMER_SIG);
#endif /* FEATURE_RRC_DELAY_TX_ON_NEW_CFG */

#ifdef FEATURE_QSH_EVENT_METRIC
/*
	uint8 count_c_activation;
	uint8 failure_cause;
	uint8 cs_start_present:1;
	uint8 ps_start_present:1;
	uint8 ul_integrity_info_present:1;
	uint8 ul_counter_sync_info:1;
*/
    if(rrc_qsh_dl_committed == FALSE)
    {
      failure_ota.ul_ota_type = RRC_OTA_UL_CPLT;
      failure_ota.msg_id = WRRC_UL_DCCH_MESSAGETYPE_TRANSPORTCHANNELRECONFIGURATIONCOMPLETE;
      if(RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.transportChannelReconfigurationComplete,
        rrc_TransportChannelReconfigurationComplete,count_C_ActivationTime))
      {
        failure_ota.ul_fail_ota.ul_ota_parms.count_c_activation= ptr->message.u.transportChannelReconfigurationComplete.count_C_ActivationTime;
      }
      if (RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.transportChannelReconfigurationComplete,
      rrc_TransportChannelReconfigurationComplete,ul_CounterSynchronisationInfo))
      {	 
        failure_ota.ul_fail_ota.ul_ota_parms.ul_counter_sync_info= 1;
      }
      if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status())
      {
        failure_ota.ul_fail_ota.ul_ota_parms.ul_integrity_info_present= 1;
      }
      /* Commit data should happen after dl SDU is committed */
    }
    else if(rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr != NULL)
    {
      if(RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.transportChannelReconfigurationComplete,
        rrc_TransportChannelReconfigurationComplete,count_C_ActivationTime))
      {
        ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.ul_ota_parms.count_c_activation= ptr->message.u.transportChannelReconfigurationComplete.count_C_ActivationTime;
      }
      if (RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.transportChannelReconfigurationComplete,
      rrc_TransportChannelReconfigurationComplete,ul_CounterSynchronisationInfo))
      {	 
        ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.ul_ota_parms.ul_counter_sync_info= 1;
      }
      if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status())
      {
        ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.ul_ota_parms.ul_integrity_info_present= 1;
      }
      /* Commit data will be called in send ul sdu */
    }
#endif
    WRRC_MSG0_HIGH("Sending TCRC Complete Message to send-chain");
    if (proc_wait_for_l2_ack)
    {
      status = rrcsend_send_ul_sdu(RRC_PROCEDURE_TCR,  /* Procedure is RRC_PROCEDURE_TCR    */
                                   (void *)ptr,        /* Message pointer     */
                                   ul_lc_id,           /* RLC id              */
                                   RRCSEND_L2ACK_REQUIRED  /* ACK is required */
                                   );
    }
    else
    {
      status = rrcsend_send_ul_sdu(RRC_PROCEDURE_TCR,  /* Procedure is RRC_PROCEDURE_TCR    */
                                   (void *)ptr,        /* Message pointer     */
                                   ul_lc_id,           /* RLC id              */
                                   RRCSEND_L2ACK_NOT_REQUIRED  /* ACK is not required */
                                   );
    }

    if( status != RRCSEND_SUCCESS )
    {
      WRRC_MSG1_ERROR("Send chain failed for TCRC complete msg: error cause %d", status);
    }
  }
  else
  {
    /* Unexpected failure. ERR for now. May need to ERR_FATAL here */
    WRRC_MSG1_ERROR("Can't send UL DCCH msg! invalid lc id: %d", ul_lc_id ); 
    /* It's important to free the memory since we didn't send the
    message. */
    rrc_free( ptr );
  }
  rrc_send_rb_rate_info_packet();
}/* end function send_tcrc_complete_message */

/*===========================================================================

FUNCTION rrctcrc_is_pcpich_info_present

DESCRIPTION
  This function checks whether Primary CPICH info is present in
  the message or not.

DEPENDENCIES
  None.

RETURN VALUE
              Boolean.
                 TRUE  : Primary CPICH info is present in the message
                 FALSE : Primary CPICH info is not present in the message

===========================================================================*/
static boolean  rrctcrc_is_pcpich_info_present
(
  rrc_TransportChannelReconfiguration  *msg_ptr,
  rrc_msg_ver_enum_type                   msg_version
)
{
  /*this function is used to find whether directed cell info is present or not during Cell_DCH to Cell_FACH 
  state transition. It is safe to assume that the rl list will have only one element.*/
  boolean result = FALSE;
  uint16 rrctcrc_psc = 512;
  if (MSG_REL5 == msg_version)
  {
    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.transportChannelReconfiguration_r5,
     rrc_TransportChannelReconfiguration_r5_IEs,dl_InformationPerRL_List))
    {
      if(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.
       transportChannelReconfiguration_r5.dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
       T_rrc_DL_InformationPerRL_r5_modeSpecificInfo_fdd)
      { 
        rrctcrc_psc = 
                 msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.
                 transportChannelReconfiguration_r5.dl_InformationPerRL_List.elem[0].modeSpecificInfo.
                 u.fdd->primaryCPICH_Info.primaryScramblingCode;

        result = TRUE;

      }
    }
  }
  else if (MSG_REL6 == msg_version)
  {
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
        criticalExtensions.u.r6.transportChannelReconfiguration_r6, 
        rrc_TransportChannelReconfiguration_r6_IEs,dl_InformationPerRL_List))
    {
      if(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.
       transportChannelReconfiguration_r6.dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
       T_rrc_DL_InformationPerRL_r6_modeSpecificInfo_fdd)
      { 
        rrctcrc_psc = 
                 msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.
                 transportChannelReconfiguration_r6.dl_InformationPerRL_List.elem[0].modeSpecificInfo.
                 u.fdd->primaryCPICH_Info.primaryScramblingCode;

        result = TRUE;
      }
    }
  }
  else if (MSG_REL7 == msg_version)
  {
   rrc_TransportChannelReconfiguration_r7_IEs *tcreconfig_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
          criticalExtensions.u.r7.transportChannelReconfiguration_r7;
    if (RRCRBR_R7_MSG_IE_PRESENT (tcreconfig_ptr,rrc_TransportChannelReconfiguration_r7_IEs,dl_InformationPerRL_List) )
    {
      if(tcreconfig_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
       T_rrc_DL_InformationPerRL_r7_modeSpecificInfo_fdd)
      { 
        rrctcrc_psc = 
                 tcreconfig_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.
                 u.fdd->primaryCPICH_Info.primaryScramblingCode;

        result = TRUE;
      }
    }
  }
  else if (MSG_REL8 == msg_version)
  {
    rrc_TransportChannelReconfiguration_r8_IEs *tcreconfig_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.
      criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.transportChannelReconfiguration_r8;
    if (RRCRB_R8_MSG_IE_PRESENT (tcreconfig_ptr,rrc_TransportChannelReconfiguration_r8_IEs,dl_InformationPerRL_List) )
    {
      if(tcreconfig_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
       T_rrc_DL_InformationPerRL_r8_modeSpecificInfo_fdd)
      { 
        rrctcrc_psc = 
                tcreconfig_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.
                u.fdd->primaryCPICH_Info.primaryScramblingCode;

        result = TRUE;
      }
    }
  }
  
#ifdef FEATURE_WCDMA_REL10
  #error code not present
#endif /* FEATURE_WCDMA_REL10 */
#ifdef FEATURE_WCDMA_REL9
  else if (MSG_REL9 == msg_version)
  {
    rrc_TransportChannelReconfiguration_r9_IEs *tcreconfig_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.
      criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.transportChannelReconfiguration_r9;
    if (RRCRB_R9_MSG_IE_PRESENT(tcreconfig_ptr,dl_InformationPerRL_List) )
    {
      if(tcreconfig_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
       T_rrc_DL_InformationPerRL_r8_modeSpecificInfo_fdd)
      { 
        rrctcrc_psc = 
                tcreconfig_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.
                u.fdd->primaryCPICH_Info.primaryScramblingCode;

        result = TRUE;
      }
    }
  }
#endif /* FEATURE_WCDMA_REL9 */

  else if((MSG_REL99 == msg_version) &&
     (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.transportChannelReconfiguration_r3,
     rrc_TransportChannelReconfiguration_r3_IEs,dl_InformationPerRL_List)))
  {
    if(msg_ptr->u.r3.transportChannelReconfiguration_r3.dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
    T_rrc_DL_InformationPerRL_modeSpecificInfo_fdd)
    { 
      rrctcrc_psc = 
               msg_ptr->u.r3.transportChannelReconfiguration_r3.dl_InformationPerRL_List.
               elem[0].modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;

        result = TRUE;
    }
  }
  MSG_HIGH("Primary SCR in TCRC message %d result %d for msg_version:%d(0-MSG_REL99,1-MSG_REL5,2-MSG_REL6,3-MSG_REL7,4-MSG_REL8,5-MSG_REL9,6-MSG_REL10,other-MSG_INVALID)",rrctcrc_psc,result,msg_version);
  return result;
  
}  /* rrctcrc_is_pcpich_info_present */



/*===========================================================================

FUNCTION   RRCTCRC_VALIDATE_SRNS_RELOCATION_INFO()

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
static boolean rrctcrc_validate_srns_relocation_info
(
  rrc_TransportChannelReconfiguration *msg_ptr, /* Pointer to the TCRC message */
  rrc_msg_ver_enum_type msg_version  
)
{
   if (MSG_REL99 == msg_version)
   {
     /*check whether dl_counter_sync info is present or not . Do not check for
     the contents of DL_counter_sync_info. First the integrity check has tobe made
     with new integrity protection mode info in the message. Later we can check
     the dl_counter_sync_info contents and send a failure if PDCP info is present */
     if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.transportChannelReconfiguration_r3,  
       rrc_TransportChannelReconfiguration_r3_IEs,dl_CounterSynchronisationInfo))
     {
       return TRUE;
     }
   }
   else if (MSG_REL5 == msg_version)
   {
      /*check whether dl_counter_sync info is present or not . Do not check for
      the contents of DL_counter_sync_info. First the integrity check has tobe made
      with new integrity protection mode info in the message. Later we can check
     the dl_counter_sync_info contents and send a failure if PDCP info is present */
     if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.
         criticalExtensions.u.r5.transportChannelReconfiguration_r5,
         rrc_TransportChannelReconfiguration_r5_IEs,dl_CounterSynchronisationInfo))
     {
       return TRUE;
     }
   }
   else if (MSG_REL6 == msg_version)
   {
      /*check whether dl_counter_sync info is present or not . Do not check for
      the contents of DL_counter_sync_info. First the integrity check has tobe made
      with new integrity protection mode info in the message. Later we can check
     the dl_counter_sync_info contents and send a failure if PDCP info is present */
     if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
         criticalExtensions.u.r6.transportChannelReconfiguration_r6,
         rrc_TransportChannelReconfiguration_r6_IEs,dl_CounterSynchronisationInfo))
     {
       return TRUE;
     }
   }
   else if (MSG_REL7 == msg_version)
   {
     rrc_TransportChannelReconfiguration_r7_IEs *tcreconfig_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
          criticalExtensions.u.r7.transportChannelReconfiguration_r7;

      /*check whether dl_counter_sync info is present or not . Do not check for
      the contents of DL_counter_sync_info. First the integrity check has tobe made
      with new integrity protection mode info in the message. Later we can check
     the dl_counter_sync_info contents and send a failure if PDCP info is present */
     if (RRCRBR_R7_MSG_IE_PRESENT(tcreconfig_ptr,rrc_TransportChannelReconfiguration_r7_IEs,dl_CounterSynchronisationInfo) )
     {
       return TRUE;
     }
   }
   else if (MSG_REL8 == msg_version)
   {
     rrc_TransportChannelReconfiguration_r8_IEs *tcreconfig_ptr = &msg_ptr->u.later_than_r3.
        criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.transportChannelReconfiguration_r8;

      /*check whether dl_counter_sync info is present or not . Do not check for
      the contents of DL_counter_sync_info. First the integrity check has tobe made
      with new integrity protection mode info in the message. Later we can check
     the dl_counter_sync_info contents and send a failure if PDCP info is present */
     if (RRCRB_R8_MSG_IE_PRESENT (tcreconfig_ptr,rrc_TransportChannelReconfiguration_r8_IEs,dl_CounterSynchronisationInfo) )
     {
       return TRUE;
     }
   }
#ifdef FEATURE_WCDMA_REL9
   else if (MSG_REL9 == msg_version)
   {
     rrc_TransportChannelReconfiguration_r9_IEs *tcreconfig_ptr = &msg_ptr->u.later_than_r3.
        criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.transportChannelReconfiguration_r9;

      /*check whether dl_counter_sync info is present or not . Do not check for
      the contents of DL_counter_sync_info. First the integrity check has tobe made
      with new integrity protection mode info in the message. Later we can check
     the dl_counter_sync_info contents and send a failure if PDCP info is present */
     if (RRCRB_R9_MSG_IE_PRESENT(tcreconfig_ptr,dl_CounterSynchronisationInfo) )
     {
       return TRUE;
     }
   }
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
  #error code not present
#endif /* FEATURE_WCDMA_REL10 */

   else
   {
     rrc_print_supported_asn1_rel_version();
     WRRC_MSG1_ERROR("validate SRNS relocation - incorrect message version: %d",
         msg_version);
   }

   return(FALSE);    
}

/*===========================================================================

FUNCTION   VALIDATE_TCRC_MESSAGE

DESCRIPTION

  This function validates the received TCRC Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a TCRC Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_tcrc_message
(
  rrc_TransportChannelReconfiguration *msg_ptr /* Pointer to the TCRC message */
)
{
 
 
  /* Assume the message is valid for now */
  tcrc.status.failure_status = RRCTC_MSG_VALID;

  if( msg_ptr->t == 
      T_rrc_TransportChannelReconfiguration_later_than_r3 )
  {
    /* Critical extensions are present that are not supported by this
    release. We have to send a TCRC Failure message. */
    WRRC_MSG0_HIGH("Critical exstension present in TCRC msg!");
    tcrc.status.failure_status = RRCTC_MSG_PROTOCOL_ERROR;
    tcrc.status.prot_err_cause = rrc_ProtocolErrorCause_messageExtensionNotComprehended;
    return;
  }


  /* Check the RRC State indicatior given by the message */
  switch( msg_ptr->u.r3.transportChannelReconfiguration_r3.rrc_StateIndicator )
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      tcrc.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      tcrc.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      tcrc.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;

    case rrc_RRC_StateIndicator_ura_PCH:
      tcrc.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;

    default:
       
      /* No other states are supported right now. */
      WRRC_MSG0_HIGH("Unsupported state in TCRC Message");
      tcrc.status.failure_status = RRCTC_MSG_UNSUPPORTED_CONFIG;
      break;
  }

  if( tcrc.status.failure_status == RRCTC_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
     return;
  }
  if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3,
      rrc_TransportChannelReconfiguration_r3,v3a0NonCriticalExtensions))
  {
    WRRC_MSG0_HIGH("NonCritical extension present: Ignoring");
  }
  /* Check if RB with PDCP Information list is present. This
  is not supported in the first release. This may be supported
  later */                                    
  if( RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.transportChannelReconfiguration_r3,
        rrc_TransportChannelReconfiguration_r3_IEs,dl_CounterSynchronisationInfo))
  {
  /*check whether the SRNS relocation is within Cell_DCH or not.
     SRNS relocation information can come in reconfiguration messages
     only when the state transition is Cell_DCH->Cell_DCH */
    if (!((tcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("SRNS reloc with invalid states cur_state %d, msg_state %d",
             rrc_get_state(), tcrc.state_indicated_by_msg);
      tcrc.status.failure_status=RRCTC_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has 
    PDCP info list. If it is true then this configuration is unsupported */
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.transportChannelReconfiguration_r3.dl_CounterSynchronisationInfo,
        rrc_DL_CounterSynchronisationInfo,rB_WithPDCP_InfoList))
    {
      WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
      tcrc.status.failure_status=RRCTC_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation. 
        Set the flag that indicates that this procedure
      involves SRNS relocation */
      tcrc.tcrc_for_srns_relocation = TRUE;
      
    /* store if new U-RNTI is present in the message */
      if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.transportChannelReconfiguration_r3,
          rrc_TransportChannelReconfiguration_r3_IEs,new_U_RNTI))
      {
        tcrc.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(msg_ptr->u.r3.transportChannelReconfiguration_r3.new_U_RNTI),
                             &tcrc.new_urnti);
      }
    }
  }

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  tcrc.trans_to_dch_ciph_config_needed = FALSE;
  if((tcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.transportChannelReconfiguration_r3,
        rrc_TransportChannelReconfiguration_r3_IEs,dl_CommonInformation)) && 
       (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.transportChannelReconfiguration_r3.dl_CommonInformation,
        rrc_DL_CommonInformation,dl_DPCH_InfoCommon)))
    {
      //check if ciphering is active in mac for cs domain, if it is then,
      //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      if(rrcsmc_tm_ciph_active() == TRUE)
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 config activated");
        if(rrcsmc_process_cipher_config_for_hho(RRCSMC_INVALID_HFN_VALUE) == FAILURE)
        {
           WRRC_MSG0_HIGH("SMC process ciphering config for HHO failed");
        }
        tcrc.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }
  /*Cell_PCH/URA_PCH specific validations */
  if ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
     /* If UTRAN DRX coefficient is not present then it is invalid configuration */
     if (!(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.transportChannelReconfiguration_r3,
         rrc_TransportChannelReconfiguration_r3_IEs,utran_DRX_CycleLengthCoeff)))
     {
       WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", tcrc.state_indicated_by_msg);
       tcrc.status.failure_status = RRCTC_MSG_INVALID_CONFIG;
       return;
     }
     if (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
     {
       /*should we do it after getting l2ack for response??*/
       if (!(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.transportChannelReconfiguration_r3,
             rrc_TransportChannelReconfiguration_r3_IEs,ura_Identity)))
           
       {
         WRRC_MSG0_HIGH("URA ID not present");
         rrccu_clear_ura_id ();
       }
       else
       {
         /*URA ID present in the message set ura_identity variable with this value */
         if (msg_ptr->u.r3.transportChannelReconfiguration_r3.ura_Identity.numbits == 16)
         {
           rrccu_update_ura_id (&(msg_ptr->u.r3.transportChannelReconfiguration_r3.ura_Identity));
         }
         else
         {
           WRRC_MSG1_ERROR("Invalid URA ID length%d",
               msg_ptr->u.r3.transportChannelReconfiguration_r3.ura_Identity.numbits);
           tcrc.status.failure_status = RRCTC_MSG_PROTOCOL_ERROR;
           tcrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
           return;
         }

       }
           
     }
  }

}/* end function validate_tcrc_message */



/*===========================================================================

FUNCTION   VALIDATE_TCRECONFIG_R5

DESCRIPTION

  This function validates the received R5 TCRC Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a TCRC Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_tcreconfig_r5
(
  rrc_TransportChannelReconfiguration *msg_ptr /* Pointer to the TCRC message */
)
{
  rrc_TransportChannelReconfiguration_r5_IEs* tcrc_ptr = NULL;

  uecomdef_status_e_type status;
  /* Assume the message is valid for now */
  tcrc.status.failure_status = RRCTC_MSG_VALID;


  tcrc_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.
    criticalExtensions.u.r5.transportChannelReconfiguration_r5;

  
  /* Check the RRC State indicatior given by the message */
  switch( tcrc_ptr->rrc_StateIndicator )
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      tcrc.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      tcrc.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      tcrc.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;

    case rrc_RRC_StateIndicator_ura_PCH:
      tcrc.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;

    default:
       
      /* No other states are supported right now. */
      WRRC_MSG0_HIGH("Unsupported state in TCRC Message");
      tcrc.status.failure_status = RRCTC_MSG_UNSUPPORTED_CONFIG;
      break;
  }

  if( tcrc.status.failure_status == RRCTC_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
     return;
  }

  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(tcrc_ptr, 
       rrc_TransportChannelReconfiguration_r5_IEs,dl_HSPDSCH_Information) )
  {
    /* Check if network does not try to initiate HSDPA with invalid state */
    status = rrc_hsdpa_initial_validation(tcrc_ptr->rrc_StateIndicator, 
      RRC_CHECK_COMMON_MSG_TYPE(tcrc_ptr->dl_HSPDSCH_Information.modeSpecificInfo,
      rrc_DL_HSPDSCH_Information_modeSpecificInfo_tdd));

    
    if (status == FAILURE)
    {
      tcrc.status.failure_status = RRCTC_MSG_INVALID_CONFIG;
      return;
    }
  }
  
  /* Check if RB with PDCP Information list is present. This
  is not supported in the first release. This may be supported
  later */                                    
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(tcrc_ptr,
        rrc_TransportChannelReconfiguration_r5_IEs,dl_CounterSynchronisationInfo))
  {
  /*check whether the SRNS relocation is within Cell_DCH or not.
     SRNS relocation information can come in reconfiguration messages
     only when the state transition is Cell_DCH->Cell_DCH */
    if (!((tcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("SRNS reloc with invalid states cur_state %d, msg_state %d",
             rrc_get_state(), tcrc.state_indicated_by_msg);
      tcrc.status.failure_status=RRCTC_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has 
    PDCP info list. If it is true then this configuration is unsupported */
    if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2(tcrc_ptr->dl_CounterSynchronisationInfo,
        rb_WithPDCP_InfoList)) ||
        (RRC_MSG_COMMON_BITMASK_IE(tcrc_ptr->dl_CounterSynchronisationInfo,
         rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
    {
      WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
      tcrc.status.failure_status=RRCTC_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation. 
        Set the flag that indicates that this procedure
      involves SRNS relocation */
      tcrc.tcrc_for_srns_relocation = TRUE;
      
    /* store if new U-RNTI is present in the message */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(tcrc_ptr, 
          rrc_TransportChannelReconfiguration_r5_IEs,new_U_RNTI))
      {
        tcrc.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(tcrc_ptr->new_U_RNTI),
                             &tcrc.new_urnti);
      }
    }
  }

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  tcrc.trans_to_dch_ciph_config_needed = FALSE;
  if((tcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(tcrc_ptr,
        rrc_TransportChannelReconfiguration_r5_IEs,dl_CommonInformation)) && 
       (RRC_MSG_COMMON_BITMASK_IE(tcrc_ptr->dl_CommonInformation,
        rrc_DL_CommonInformation_r5,dl_DPCH_InfoCommon)))
    {
      //check if ciphering is active in mac for cs domain, if it is then,
      //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      if(rrcsmc_tm_ciph_active() == TRUE)
      {
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        uecomdef_status_e_type loc_ret_val;
        WRRC_MSG0_HIGH("To-DCH-Step-1 config activated");
        loc_ret_val = rrcrb_get_mac_d_hfn(
             (  RRC_MSG_COMMON_BITMASK_IE(tcrc_ptr->dl_CommonInformation.dl_DPCH_InfoCommon,
             rrc_DL_DPCH_InfoCommon_r4,mac_d_HFN_initial_value)),
             &tcrc_ptr->dl_CommonInformation.dl_DPCH_InfoCommon.mac_d_HFN_initial_value,
             &mac_d_hfn_l);        
        if(loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("get_mac_d_hfn failed");
        }
        if(rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
        {
           WRRC_MSG0_HIGH("SMC process ciphering config for HHO failed");
        }
        tcrc.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }
  /*Cell_PCH/URA_PCH specific validations */
  if ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
     /* If UTRAN DRX coefficient is not present then it is invalid configuration */
     if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(tcrc_ptr, 
         rrc_TransportChannelReconfiguration_r5_IEs,utran_DRX_CycleLengthCoeff)))
     {
       WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", tcrc.state_indicated_by_msg);
       tcrc.status.failure_status = RRCTC_MSG_INVALID_CONFIG;
       return;
     }
     if (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
     {
       /*should we do it after getting l2ack for response??*/
       if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(tcrc_ptr, 
             rrc_TransportChannelReconfiguration_r5_IEs,ura_Identity)))
           
       {
         WRRC_MSG0_HIGH("URA ID not present");
         rrccu_clear_ura_id ();
       }
       else
       {
         /*URA ID present in the message set ura_identity variable with this value */
         if (tcrc_ptr->ura_Identity.numbits == 16)
         {
           rrccu_update_ura_id (&(tcrc_ptr->ura_Identity));
         }
         else
         {
           WRRC_MSG1_ERROR("Invalid URA ID length%d",
               tcrc_ptr->ura_Identity.numbits);
           tcrc.status.failure_status = RRCTC_MSG_PROTOCOL_ERROR;
           tcrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
           return;
         }

       }
           
     }
  }

}/* end function validate_tcreconfig_r5 */



/*===========================================================================

FUNCTION   VALIDATE_TCRECONFIG_R6

DESCRIPTION

  This function validates the received R6 TCRC Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a TCRC Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_tcreconfig_r6
(
  rrc_TransportChannelReconfiguration_r6_IEs* tcrc_ptr /* Pointer to rel-6 TCRC message */
)
{
  uecomdef_status_e_type status;

  /* Assume the message is valid for now */
  tcrc.status.failure_status = RRCTC_MSG_VALID;

  /* Check the RRC State indicatior given by the message */
  switch( tcrc_ptr->rrc_StateIndicator )
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      tcrc.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      tcrc.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      tcrc.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;

    case rrc_RRC_StateIndicator_ura_PCH:
      tcrc.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;

    default:
       
      /* No other states are supported right now. */
      WRRC_MSG0_HIGH("Unsupported state in TCRC Message");
      tcrc.status.failure_status = RRCTC_MSG_UNSUPPORTED_CONFIG;
      break;
  }

  if( tcrc.status.failure_status == RRCTC_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
     return;
  }

  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(tcrc_ptr, 
       rrc_TransportChannelReconfiguration_r6_IEs,dl_HSPDSCH_Information))
  {
    /* Check if network does not try to initiate HSDPA with invalid state */
    status = rrc_hsdpa_initial_validation_r6(tcrc_ptr->rrc_StateIndicator, 
      &tcrc_ptr->dl_HSPDSCH_Information);
    
    if (status == FAILURE)
    {
      tcrc.status.failure_status = RRCTC_MSG_INVALID_CONFIG;
      return;
    }
  }
  
  /* Check if RB with PDCP Information list is present. This
  is not supported in the first release. This may be supported
  later */                                    
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(tcrc_ptr,
        rrc_TransportChannelReconfiguration_r6_IEs,dl_CounterSynchronisationInfo))
  {
  /*check whether the SRNS relocation is within Cell_DCH or not.
     SRNS relocation information can come in reconfiguration messages
     only when the state transition is Cell_DCH->Cell_DCH */
    if (!((tcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("SRNS reloc with invalid states cur_state %d, msg_state %d",
             rrc_get_state(), tcrc.state_indicated_by_msg);
      tcrc.status.failure_status=RRCTC_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has 
    PDCP info list. If it is true then this configuration is unsupported */
    if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2(tcrc_ptr->dl_CounterSynchronisationInfo,
        rb_WithPDCP_InfoList)) ||
        (RRC_MSG_COMMON_BITMASK_IE(tcrc_ptr->dl_CounterSynchronisationInfo,
         rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
    {
      WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
      tcrc.status.failure_status=RRCTC_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation. 
        Set the flag that indicates that this procedure
      involves SRNS relocation */
      tcrc.tcrc_for_srns_relocation = TRUE;
      
    /* store if new U-RNTI is present in the message */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(tcrc_ptr, 
          rrc_TransportChannelReconfiguration_r6_IEs,new_U_RNTI))
      {
        tcrc.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(tcrc_ptr->new_U_RNTI),
                             &tcrc.new_urnti);
      }
    }
  }

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  tcrc.trans_to_dch_ciph_config_needed = FALSE;
  if((tcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(tcrc_ptr,
        rrc_TransportChannelReconfiguration_r6_IEs,dl_CommonInformation)) && 
       (RRC_MSG_COMMON_BITMASK_IE(tcrc_ptr->dl_CommonInformation,
       rrc_DL_CommonInformation_r6,dl_dpchInfoCommon)) &&
       (T_rrc_DL_CommonInformation_r6_dl_dpchInfoCommon_dl_DPCH_InfoCommon == 
         tcrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.t)
       )
    {
      //check if ciphering is active in mac for cs domain, if it is then,
      //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      if(rrcsmc_tm_ciph_active() == TRUE)
      {
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        uecomdef_status_e_type loc_ret_val;
        WRRC_MSG0_HIGH("To-DCH-Step-1 config activated");
        loc_ret_val = rrcrb_get_mac_d_hfn(
             ( RRC_MSG_COMMON_BITMASK_IE_PTR(tcrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon,rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
              &tcrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
              &mac_d_hfn_l);
        if (loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("get_mac_d_hfn failed");
        }
        if(rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
        {
           WRRC_MSG0_HIGH("SMC process ciphering config for HHO failed");
        }
        tcrc.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }
  /*Cell_PCH/URA_PCH specific validations */
  if ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
     /* If UTRAN DRX coefficient is not present then it is invalid configuration */
     if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(tcrc_ptr, 
         rrc_TransportChannelReconfiguration_r6_IEs,utran_DRX_CycleLengthCoeff)))
     {
       WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", tcrc.state_indicated_by_msg);
       tcrc.status.failure_status = RRCTC_MSG_INVALID_CONFIG;
       return;
     }
     if (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
     {
       /*should we do it after getting l2ack for response??*/
       if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(tcrc_ptr, 
             rrc_TransportChannelReconfiguration_r6_IEs,ura_Identity)))
           
       {
         WRRC_MSG0_HIGH("URA ID not present");
         rrccu_clear_ura_id ();
       }
       else
       {
         /*URA ID present in the message set ura_identity variable with this value */
         if (tcrc_ptr->ura_Identity.numbits == 16)
         {
           rrccu_update_ura_id (&(tcrc_ptr->ura_Identity));
         }
         else
         {
           WRRC_MSG1_ERROR("Invalid URA ID length%d",
               tcrc_ptr->ura_Identity.numbits);
           tcrc.status.failure_status = RRCTC_MSG_PROTOCOL_ERROR;
           tcrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
           return;
         }
       }
     }
  }
}/* end function validate_tcreconfig_r5 */


/*===========================================================================

FUNCTION   RRCTCRC_SEND_CHANNEL_CONFIG_REQ

DESCRIPTION

  This function sends the RRC_CHANNEL_CONFIG_REQ command to LLC
          
DEPENDENCIES

  ORDERED_CONFIG must be set before calling this function.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_send_channel_config_req
(
 void 
)
{
  rrc_cmd_type  *ptr;          /* To hold the Command                       */
  uint32        rb_count =0;      /* To count RBs to be reconfigured           */
  rrc_state_e_type next_rrc_state = tcrc.state_indicated_by_msg;
   
  /* Get the command buffer */
  ptr = rrc_get_int_cmd_buf();
  /* Fill in the command id  */
  ptr->cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;
 
  /* Check if state change is required */
  if( (rrc_get_state()) == (tcrc.state_indicated_by_msg) )
  {
    ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;
  }
  else
  {
    /* The message indicates that state change is required.
    Indicate to LLC what the next state should be. */
    ptr->cmd.chan_config_req.rrc_state_change_required = TRUE;
    ptr->cmd.chan_config_req.next_state = tcrc.state_indicated_by_msg;

    if ((rrc_get_state() == RRC_STATE_CELL_FACH) && 
        ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) || 
         (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)))
    {
      /* UE is directly transitioning to Cell_PCH state from Cell_FACH state so
      include release of CCCH and establishment of PCCH in the same channel config
      request */
      ptr->cmd.chan_config_req.rb[rb_count].rb_id       = CCCH_RADIO_BEARER_ID;
      ptr->cmd.chan_config_req.rb[rb_count++].rb_config = RELEASE_RB;
      ptr->cmd.chan_config_req.rb[rb_count].rb_id       = PCCH_RADIO_BEARER_ID;
      ptr->cmd.chan_config_req.rb[rb_count++].rb_config = ESTABLISH_RB;
    }
  }

  ptr->cmd.chan_config_req.num_rb_to_config = (int) rb_count; 
  /* Indicate the id of the procedure sending the command */

  rrc_fill_rb_id_chan_req_from_standalone_db(&rb_count, &ptr->cmd.chan_config_req);

  if(tcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH)
  {
    /* Fill those RBs in chan_config_req for which RLC PDU size has changed and 
      that are already not present in chan_config_req*/
    rrc_fill_rb_id_chan_req_from_rb_list_mapped_to_dch(&rb_count, &ptr->cmd.chan_config_req);

    if(rrc_get_state() == RRC_STATE_CELL_FACH)
    {
      rrc_fill_srb_in_chan_config_req(&rb_count, &ptr->cmd.chan_config_req);
    }
  }

  /*If state transition is Cell_DCH->Cell_FACH or vice-verca, add all the AM PS RBs
   in chan_config_req*/	
  if(((next_rrc_state == RRC_STATE_CELL_FACH) && 
       (transition_config.toc_usage == TOC_FOR_DCH_AND_FACH ||
        transition_config.toc_usage == TOC_FOR_FACH_AND_PCH ||
        transition_config.toc_usage == TOC_FOR_FACH_AND_PCH_WITH_DCH_INFO))
       ||
      ((rrc_get_state() == RRC_STATE_CELL_FACH) && (next_rrc_state == RRC_STATE_CELL_DCH))
    )
  {
    rrc_fill_rb_id_chan_req_from_est_rabs(&rb_count, &ptr->cmd.chan_config_req);
  }

  ptr->cmd.chan_config_req.procedure = RRC_PROCEDURE_TCR;

  /* Indicate that we need a confirmation for this command */
  ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = TRUE;

  /* We are not required to fill any other data for this command.
  Now, send the command */
  rrc_put_int_cmd( ptr );
}/* end function rrctcrc_send_channel_config_req */


/*===========================================================================

FUNCTION  RRCTCRC_CLEAR_PROCEDURE

DESCRIPTION

  This function clears all the internal variables in
  the TCRC procedure and brings them to their default
  values.
    
DEPENDENCIES

  The Substate of the procedure should be changed
  to initial substate before calling this function.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_clear_procedure( void )
{

  /* Initialize the transaction id's for this procedure.
  Use a number that can't be an actual received transaction
  id. */
  tcrc.tr_ids.accepted_transaction = RRCTC_INVALID_TRANSACTION_ID;
  tcrc.tr_ids.rejected_transaction = RRCTC_INVALID_TRANSACTION_ID;

  /* Clear the C-RNTI valid flag */
  tcrc.new_crnti_valid = FALSE;

  /* Reset Initiate Cell Update Flag */
  rrctcrc_send_initiate_cu_flag = FALSE;
  rrctcrc_delete_crnti          = FALSE;
  tcrc.directed_cell_info       = FALSE;

  tcrc.tcrc_for_srns_relocation = FALSE;
  tcrc.new_crnti_valid = FALSE;
  tcrc.status.failure_status = RRCTC_MSG_VALID;
  /* free the memory allocate for mm_cmd_buf
  if it is not free or in case of unsuccessful events
  before sending this cmd to mm */
  if (tcrc.cn_info_saved != FALSE)
  {
    /* free the memory allocated for mm cmd */
    WRRC_MSG1_HIGH("Free MM cmd buffer %x", tcrc.mm_cmd_ptr);
    mm_free_cmd_buf(tcrc.mm_cmd_ptr);
    /* set cn_info_saved to false */
    tcrc.cn_info_saved = FALSE;
  }

  tcrc.trans_to_dch_ciph_config_needed = FALSE;
  tcrc.initiate_cell_selection_trans_from_fach = FALSE;     


  tcrc.high_mobility_ind = FALSE;

  rrc_check_and_init_rrc_reestab_entity(RRC_PROCEDURE_TCR);
  
}


/*===========================================================================

FUNCTION   rrcrbrc_check_and_send_failure_for_simul_reconfig

DESCRIPTION

  This function check whether received reconfig message results in simultaneous
  reconfig or not. If yes, then it sends a failure message to NW and returns FAILURE.
  If no, then it updates the accepted transaction id and return SUCCESS
          
DEPENDENCIES

  None.

REUSABILITY: R99/R5/R6

RETURN VALUE

  SUCCESS or FAILURE

SIDE EFFECTS

  None.

===========================================================================*/
static uecomdef_status_e_type rrctcrc_check_and_send_failure_for_simul_reconfig
(
  rrc_RRC_TransactionIdentifier transaction_id
)
{
  if ((rrcllc_get_ordered_config_status() != OC_NOT_SET) ||
    (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
    (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
  {
    MSG_HIGH("Incomatible simultaneous reconfig! Rejecting TC RC message trans_id:%d,accepted tr_id:%d,rejected tr-id:%d", 
      transaction_id, tcrc.tr_ids.accepted_transaction,tcrc.tr_ids.rejected_transaction);
    if((transaction_id != tcrc.tr_ids.accepted_transaction ) && (transaction_id  != tcrc.tr_ids.rejected_transaction))
    {
      /* We have received a TC RC message with a new transaction id.
      This message should be in our rejected transaction
      list */
      tcrc.tr_ids.rejected_transaction = transaction_id;
      tcrc.status.failure_status = RRCTC_SIMULTANEOUS_RECONFIG;
 
      /*Check whether Cell Update is going on */
      if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
         (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
      {
        if(tcrc.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
        {
          send_tcrc_failure_message( tcrc.status.failure_status,
                                   tcrc.status.prot_err_cause );
          tcrc.status.failure_status = RRCTC_MSG_VALID;
        }
        else
        {
          WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
          if (SUCCESS != rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_TCR))
          {
            WRRC_MSG0_ERROR("TCR failed to register with CU");
          }
          /* Wait until Cell update procedure is completed */
          tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF; 
        }
      }
      else
      {
        send_tcrc_failure_message( tcrc.status.failure_status,
                        tcrc.status.prot_err_cause );
        /*continue the same substate */ 
      }
    }
    return FAILURE;
  }

  /* We have received a RB RC message with a new transaction id.
   Accept the transaction */
  tcrc.tr_ids.accepted_transaction = transaction_id;

  return(SUCCESS);
}

/*===========================================================================

FUNCTION   RRCTCRC_PROCESS_SUCCESSFUL_PROC_COMPLETION

DESCRIPTION

  This function does the final steps when the procedure is
  about to complete successfully.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_process_successful_proc_completion
(
 boolean indicate_rlc
)
{
  /* send the nas cn info if stored */
  if (tcrc.cn_info_saved != FALSE)
  {
    if(tcrc.mm_cmd_ptr->cmd.hdr.message_id == (byte) RRC_SERVICE_IND_FROM_OTA_MSG )
    {
       /* Update the access class information in the service info structure */
       rrc_ccm_update_service_info(tcrc.mm_cmd_ptr);
    }
    /* Put the command on MM queue */
    rrc_send_rrc_message_to_mm(tcrc.mm_cmd_ptr);
    /* clear the flag */
    tcrc.cn_info_saved = FALSE;
  }
  /* Clear ORDERED_CONFIG - UE can now accept other ordered
  configurations */
  rrcllc_clear_ordered_config();  

  if (((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) || 
       (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH )) &&
      (indicate_rlc == TRUE))
  {
    /* register with RLC for uplink data indication */
    rrcrb_register_with_rlc_for_ul_data_ind (RRC_PROCEDURE_TCR);

  }

  /* Go to the next sub-state in the state machine. That's
  the initial substate since we've sent the RB RC complete
  message */
  tcrc.curr_substate = RRCRB_INITIAL;

  /* At this point we don't need to remember any of the
  Radio Bearer information since it's stored in ESTABLISHED_RABS.
  Clear our internal variables */
  rrctcrc_clear_procedure();

  if (rrccu_return_cu_substate() == RRCCU_WAIT_FOR_RB_PROC_COMPL_IND) 
  {
    rrcrb_send_rb_proc_compl_ind();
  }

  rrccu_reset_registration_info_for_procedure(RRC_PROCEDURE_TCR);

}    /* rrctcrc_process_successful_proc_completion */


/*===========================================================================

FUNCTION   VALIDATE_TCRECONFIG_R7

DESCRIPTION

  This function validates the received R6 TCRC Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a TCRC Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_tcreconfig_r7
(
  rrc_TransportChannelReconfiguration_r7_IEs* tcrc_ptr /* Pointer to rel-6 TCRC message */
)
{
  uecomdef_status_e_type status;

  /* Assume the message is valid for now */
  tcrc.status.failure_status = RRCTC_MSG_VALID;

  /* Check the RRC State indicatior given by the message */
  switch( tcrc_ptr->rrc_StateIndicator )
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      tcrc.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      tcrc.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      tcrc.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;

    case rrc_RRC_StateIndicator_ura_PCH:
      tcrc.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;

    default:
       
      /* No other states are supported right now. */
      WRRC_MSG0_HIGH("Unsupported state in TCRC Message");
      tcrc.status.failure_status = RRCTC_MSG_UNSUPPORTED_CONFIG;
      break;
  }

  if( tcrc.status.failure_status == RRCTC_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
     return;
  }

  if(RRCRBR_R7_MSG_IE_PRESENT (tcrc_ptr,rrc_TransportChannelReconfiguration_r7_IEs,dl_HSPDSCH_Information))  
  {
    /* Check if network does not try to initiate HSDPA with invalid state */

    status = rrc_hsdpa_initial_validation(tcrc_ptr->rrc_StateIndicator, 
     RRC_CHECK_COMMON_MSG_TYPE(tcrc_ptr->dl_HSPDSCH_Information.modeSpecificInfo,
     rrc_DL_HSPDSCH_Information_modeSpecificInfo_tdd));

    if (status == FAILURE)
    {
      tcrc.status.failure_status = RRCTC_MSG_INVALID_CONFIG;
      return;
    }
  }
  
  /* Check if RB with PDCP Information list is present. This
  is not supported in the first release. This may be supported
  later */                                    
  if(RRCRBR_R7_MSG_IE_PRESENT (tcrc_ptr,rrc_TransportChannelReconfiguration_r7_IEs,dl_CounterSynchronisationInfo)) 
   {
  /*check whether the SRNS relocation is within Cell_DCH or not.
     SRNS relocation information can come in reconfiguration messages
     only when the state transition is Cell_DCH->Cell_DCH */
    if (!((tcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("SRNS reloc with invalid states cur_state %d, msg_state %d",
             rrc_get_state(), tcrc.state_indicated_by_msg);
      tcrc.status.failure_status=RRCTC_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has 
    PDCP info list. If it is true then this configuration is unsupported */
    if ((RRC_MSG_COMMON_BITMASK_IE(tcrc_ptr->dl_CounterSynchronisationInfo,
        ,rb_WithPDCP_InfoList)) ||
        (RRC_MSG_COMMON_BITMASK_IE(tcrc_ptr->dl_CounterSynchronisationInfo,
         rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
    {
      WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
      tcrc.status.failure_status=RRCTC_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation. 
        Set the flag that indicates that this procedure
      involves SRNS relocation */
      tcrc.tcrc_for_srns_relocation = TRUE;
      
    /* store if new U-RNTI is present in the message */
      if(RRCRBR_R7_MSG_IE_PRESENT (tcrc_ptr,rrc_TransportChannelReconfiguration_r7_IEs,new_U_RNTI))  
      {
        tcrc.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(tcrc_ptr->new_U_RNTI),
                             &tcrc.new_urnti);
      }
    }
  }

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  tcrc.trans_to_dch_ciph_config_needed = FALSE;
  if((tcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRCRBR_R7_MSG_IE_PRESENT (tcrc_ptr,rrc_TransportChannelReconfiguration_r7_IEs,dl_CommonInformation)) 
        && (RRC_MSG_COMMON_BITMASK_IE(tcrc_ptr->dl_CommonInformation,
        rrc_DL_CommonInformation_r7,dl_dpchInfoCommon))
        && (T_rrc_DL_CommonInformation_r7_dl_dpchInfoCommon_dl_DPCH_InfoCommon == 
         tcrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.t)
       )
    {
      //check if ciphering is active in mac for cs domain, if it is then,
      //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      if(rrcsmc_tm_ciph_active() == TRUE)
      {
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        uecomdef_status_e_type loc_ret_val;
        WRRC_MSG0_HIGH("To-DCH-Step-1 config activated");
        loc_ret_val = rrcrb_get_mac_d_hfn(
             ( RRC_MSG_COMMON_BITMASK_IE_PTR(tcrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon,rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
              &tcrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
              &mac_d_hfn_l);
        if (loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("get_mac_d_hfn failed");
        }
        if(rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
        {
           WRRC_MSG0_HIGH("SMC process ciphering config for HHO failed");
        }
        tcrc.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }
  /*Cell_PCH/URA_PCH specific validations */
  if ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
     /* If UTRAN DRX coefficient is not present then it is invalid configuration */
     if (!(RRCRBR_R7_MSG_IE_PRESENT (tcrc_ptr,rrc_TransportChannelReconfiguration_r7_IEs,utran_DRX_CycleLengthCoeff)) )  
     {
       WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", tcrc.state_indicated_by_msg);
       tcrc.status.failure_status = RRCTC_MSG_INVALID_CONFIG;
       return;
     }
     if (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
     {
       /*should we do it after getting l2ack for response??*/
       if (!(RRCRBR_R7_MSG_IE_PRESENT (tcrc_ptr,rrc_TransportChannelReconfiguration_r7_IEs,ura_Identity)) ) 
           
       {
         WRRC_MSG0_HIGH("URA ID not present");
         rrccu_clear_ura_id ();
       }
       else
       {
         /*URA ID present in the message set ura_identity variable with this value */
         if (tcrc_ptr->ura_Identity.numbits == 16)
         {
           rrccu_update_ura_id (&(tcrc_ptr->ura_Identity));
         }
         else
         {
           WRRC_MSG1_ERROR("Invalid URA ID length%d",
               tcrc_ptr->ura_Identity.numbits);
           tcrc.status.failure_status = RRCTC_MSG_PROTOCOL_ERROR;
           tcrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
           return;
         }
       }
     }
  }
}/* end function validate_tcreconfig_r5 */

/*===========================================================================

FUNCTION   PROCESS_TCRECONFIG_R7

DESCRIPTION

  This function processes the Rel 6 TCRC Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a TCRC Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to configure the transport channels given by the message.
    
    If ORDERED_CONFIG could not be set, a TCRC Failure message
    will be sent to UTRAN.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_tcreconfig_r7
(
  rrc_RRC_TransactionIdentifier transaction_id,

  rrc_DL_DCCH_Message *dcch_msg_ptr  ,
  /* Pointer to the RB Reconfig message        */
  rrc_TransportChannelReconfiguration_r7_IEs *tcreconfig_ptr
)
{

  rrc_TransportChannelReconfiguration *msg_ptr; /* Pointer to the TCRC message        */
  rrcllc_oc_status_e_type             oc_status;/* To hold ordered_config status          */

  rrc_state_e_type                    current_state; /* current RRC State  */
  rrc_int_c_rnti_type                 c_rnti;       /* Local C_RNTI value */
  rrc_int_u_rnti_type                 u_rnti;
  rrcrb_rnti_update_info_type         rnti_update_info;   /* to hold rnti update info */
  rrcrb_directed_cell_info_type       directed_cell_info; /* directed cell info struct */

  /* Used to indicate if an error occured while setting ordered_config. */
  boolean ordered_config_err_occured = FALSE; 
 
  /* Get the pointer to the actual TCRC message */
  msg_ptr = &(dcch_msg_ptr->message.u.transportChannelReconfiguration);
 

  if (FAILURE == 
      rrctcrc_check_and_send_failure_for_simul_reconfig(
        msg_ptr->u.later_than_r3.rrc_TransactionIdentifier))
  {
    return;
  }

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  validate_tcreconfig_r7( tcreconfig_ptr );

  if( tcrc.status.failure_status != RRCTC_MSG_VALID )
  {
    /* TCRC Message was invalid. */
    WRRC_MSG0_ERROR("TCRC Message validation failed");

    /* Send the TCRC Failure Message */
    send_tcrc_failure_message( tcrc.status.failure_status,
                              tcrc.status.prot_err_cause
                            );

    /* Make sure we're back in the initial sub-state */
    tcrc.curr_substate = RRCRB_INITIAL;
  }
  /* If the message is valid, proceed further */
  else
  {

    MSG_LOW("Processing TCRC Message", 0, 0, 0);

    /* Check if there is CN information present. If so, we need
    to send the information to the Cell Change manager */
    if(RRCRBR_R7_MSG_IE_PRESENT (tcreconfig_ptr,rrc_TransportChannelReconfiguration_r7_IEs,cn_InformationInfo) ) 
    {
      WRRC_MSG0_HIGH("Saving CN info for NAS");
      tcrc.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(
        &(tcreconfig_ptr->cn_InformationInfo) );
      if (tcrc.mm_cmd_ptr != NULL)
      {
        /* set cn_info_saved flag */
        tcrc.cn_info_saved = TRUE;
      }

    }

     
#ifdef FEATURE_WCDMA_REL7_SPECRS	
	/*If TCRC OTA provides "supportForChangeOfUE_Capability" IE, we need to store it in
           "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
           "rrc_rnc_cap_change_support" can be updated accordingly. */  
    if(RRCRBR_R7_MSG_IE_PRESENT (tcreconfig_ptr,rrc_TransportChannelReconfiguration_r7_IEs,
		supportForChangeOfUE_Capability))
    {
      if(tcreconfig_ptr->supportForChangeOfUE_Capability == TRUE)
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




    /*If TCRC OTA provides "responseToChangeOfUE_Capability_present" IE, we need to store it so that later
          if procedure is successful, TCRC need to notify the COUEC procedure  about this IE.*/
          
    rrc_change_of_ue_cap_response = FALSE;          
    
    if(RRCRBR_R7_MSG_IE_PRESENT (tcreconfig_ptr,rrc_TransportChannelReconfiguration_r7_IEs,
          responseToChangeOfUE_Capability))
    {
      rrc_change_of_ue_cap_response = TRUE;
    }        

#endif	
    if(RRCRBR_R7_MSG_IE_PRESENT (tcreconfig_ptr,rrc_TransportChannelReconfiguration_r7_IEs,ueMobilityStateIndicator) ) 
    {
      if(tcreconfig_ptr->ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
      {
        WRRC_MSG0_HIGH("High Mobility Ind Set to TRUE in TCRC.");
        tcrc.high_mobility_ind = TRUE;
      }
    }
     
    /* We have to reconfigure Transport Channel. First set ORDERED_CONFIG
       so that LLC can process the necessary parameters. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_TCR,
                                           tcrc.state_indicated_by_msg,
                                           rrc_DL_DCCH_Message_PDU,
                                           (void *)dcch_msg_ptr 
                                         );

    switch( oc_status )
    {

      case ORDERED_CONFIG_SET:
        if(rrc_sib_is_current_event_sib_read_in_dch())
        {
          WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
          rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
        }
        current_state = rrc_get_state(); 
        /* Populate the local variables to indicate if Re-direction info is present */
        directed_cell_info.freq_info_present = FALSE;
        directed_cell_info.pscr_present = FALSE;   
        if((current_state==RRC_STATE_CELL_FACH) && 
               ((tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
               ||(tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
               ||(tcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          if((RRCRBR_R7_MSG_IE_PRESENT (tcreconfig_ptr,rrc_TransportChannelReconfiguration_r7_IEs,frequencyInfo) )
             && (RRC_CHECK_COMMON_MSG_TYPE(tcreconfig_ptr->frequencyInfo.modeSpecificInfo,
                rrc_FrequencyInfo_modeSpecificInfo_fdd)))
          {
            directed_cell_info.freq_info_present = TRUE;
            directed_cell_info.uarfcn_dl = 
              tcreconfig_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
          }
         
          if (rrctcrc_is_pcpich_info_present(msg_ptr, MSG_REL7))
          {
            directed_cell_info.pscr_present = TRUE;
            directed_cell_info.pscr = tcreconfig_ptr->dl_InformationPerRL_List.elem[0].
                                      modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
          }      
          /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
                 FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
          if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
            (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
          {
            WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
            if(tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
            {
              rrctcrc_send_initiate_cu_flag = TRUE;
              rrctcrc_delete_crnti          = TRUE;
            }
            else
            {
              tcrc.directed_cell_info = TRUE;
            }
          }  
        }
        if (
            (current_state==tcrc.state_indicated_by_msg)||
            ((current_state==RRC_STATE_CELL_FACH)&& 
             (tcrc.state_indicated_by_msg==RRC_STATE_CELL_DCH))
           )
        {
          /*  CellDCH<->CellDCH or CellFACH<->CellFACH or CellFACH->CellDCH
           transition.  Ordered Config has been set. We can setup the
           Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */

           /*check whether any frequency info is present in the message
           and transition is CELL_FACH->CELL_FACH. In this case
           initiate a cell update after channel config is successfully completed */
           if ((current_state==RRC_STATE_CELL_FACH)&& 
              (tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
           {
              /*call the function that evaluates the directed cell info and set the flags accordingly */
             if ((rrctcrc_send_initiate_cu_flag == FALSE) &&
                (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
             {
               if(RRCRBR_R7_MSG_IE_PRESENT(tcreconfig_ptr,rrc_TransportChannelReconfiguration_r7_IEs,new_C_RNTI) )
               {
                 tcrc.new_crnti_valid = TRUE;
                 rrc_translate_crnti(&tcreconfig_ptr->new_C_RNTI, 
                                       &c_rnti);
                 tcrc.new_crnti = (uint16)c_rnti;
               }
               else
               {
                 tcrc.new_crnti_valid = FALSE;
               }
     
               /* Now copy OC to TOC */
               rrcllcoc_update_toc_with_oc_from_fach();
               /*Initiate the cell selection*/
               rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
                                                        RRC_TRANSITION_FROM_FACH,
                                                        tcrc.state_indicated_by_msg);
                 
               tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
          
               /* exit from the switch case */
               break;
             }
    
           } /* if CELL_FACH->CELL_FACH reconfiguration */

           /* Call the function that creates and sends a channel
           config request to LLC. */
           rrctcrc_send_channel_config_req( );

           /* Change the substate since we're waiting for the
           confirmation from LLC for the channel configs */
           tcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                  (tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
        {
          if(RRCRBR_R7_MSG_IE_PRESENT(tcreconfig_ptr,rrc_TransportChannelReconfiguration_r7_IEs,new_C_RNTI) )
          {
             tcrc.new_crnti_valid = TRUE;
             rrc_translate_crnti(&tcreconfig_ptr->new_C_RNTI, 
                                  &c_rnti);
             tcrc.new_crnti = (uint16)c_rnti;
          }
          else
          {
            tcrc.new_crnti_valid = FALSE;
          }

          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_TCR,
                                                  RRC_TRANSITION_FROM_DCH,
                                                  tcrc.state_indicated_by_msg);

          tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

          /* Start T_305 [periodic cell update] timer */
          rrccu_start_t_305_timer();
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                 (tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH))
        {
          /*store the directed cell info */
           tcrc.directed_cell_info = rrctcrc_is_pcpich_info_present(msg_ptr, MSG_REL7);

           /*Send the TCRC complete msg on old configuration */
           send_tcrc_complete_message(TRUE);
           /*wait for L2ack */
           tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
        
           /* prints rate for each transport channel */
           rrcllc_print_trans_chnl_rate();

        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (tcrc.state_indicated_by_msg==RRC_STATE_URA_PCH))
        {
          /*Send the TCRC complete msg on old configuration */
          send_tcrc_complete_message(TRUE);
          /*wait for L2ack */
          tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
        
          /* prints rate for each transport channel */
          rrcllc_print_trans_chnl_rate();
          
        }
        else if((current_state==RRC_STATE_CELL_FACH) && 
                ((tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
                 (tcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          /*call the function that evaluates the directed cell info and set the flags accordingly */
          if ((tcrc.directed_cell_info == FALSE) &&
            (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
          {  
            if(tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
            {
              tcrc.directed_cell_info = rrctcrc_is_pcpich_info_present(msg_ptr, MSG_REL7);
            }

            tcrc.initiate_cell_selection_trans_from_fach = TRUE;
          }
    

          /* check whether CPICH info is present or not. Store this
          info  */
         // tcrc.directed_cell_info = rrctcrc_is_pcpich_info_present(msg_ptr);
          /*Send the RRCTCTC complete msg on old configuration */
          /* check whether new C-RNTI was present in the message. In that case
          we have to configure MAC with new C-RNTI before sending the response 
          message */
          if(RRCRBR_R7_MSG_IE_PRESENT(tcreconfig_ptr,rrc_TransportChannelReconfiguration_r7_IEs,new_C_RNTI) )
          {
            rrc_translate_crnti(&tcreconfig_ptr->new_C_RNTI, 
                                &c_rnti);
            /* Send a RNTI Update Request */
            rnti_update_info.crnti_action = RRC_RNTI_UPDATE;
            rnti_update_info.crnti        = (uint16)c_rnti;
            /* check whether URNTI is also present in the message */
            if (RRCRBR_R7_MSG_IE_PRESENT(tcreconfig_ptr,rrc_TransportChannelReconfiguration_r7_IEs,new_U_RNTI) )
            {
              rrc_translate_urnti(&tcreconfig_ptr->new_U_RNTI, 
                                  &u_rnti);
              rnti_update_info.urnti_action = RRC_RNTI_UPDATE; 
              rnti_update_info.urnti        = u_rnti; 
            }
            else
            {
              rnti_update_info.urnti_action = RRC_RNTI_NOCHANGE; 
            }
            rnti_update_info.procedure            = RRC_PROCEDURE_TCR;
            rnti_update_info.rlc_re_establish_srb = FALSE;
            rnti_update_info.rlc_re_establish_rab = FALSE;
            rnti_update_info.cnf_reqd             = TRUE;
            rrcrb_send_rnti_update_req(rnti_update_info);
            /* wait for the RNTI update confirm before sending the response
            message */
            tcrc.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
          }
          else 
          {
            send_tcrc_complete_message(TRUE);
            /*wait for L2ack */
            tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
            /* prints rate for each transport channel */
            rrcllc_print_trans_chnl_rate();

          }
        }
        break;

      case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:
        tcrc.status.failure_status = RRCTC_SIMULTANEOUS_RECONFIG;
        /* Send the TCRC Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause );

        /* Go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:
        tcrc.status.failure_status =RRCTC_MSG_UNSUPPORTED_CONFIG;
        /* Send the TCRC Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause );

        /* Go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

       case ORDERED_CONFIG_CONFIGURATION_INVALID:
        tcrc.status.failure_status =RRCTC_MSG_INVALID_CONFIG;
        /* Send the TCRC Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause );

        /* Go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
      
      case ORDERED_CONFIG_NOT_SET_OTHER:
        WRRC_MSG0_HIGH("LLC returned other failure!");
        /* When LLC sends this, there is a serious problem
        and we'll go back to disconnected state. No need
        to send the failure message here, we can just
        go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME        
      case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("TCR processing deferred due to LTA!");
    
        tcrc.curr_substate = RRCRB_INITIAL;
    
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
      default:

        WRRC_MSG0_ERROR("Unknown return value from LLC");
        /* Unknown error, just go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
    }/* end switch */
    
    if( ordered_config_err_occured == TRUE )
    {
      /* No need to continue. Clear the procedure
      variables and return. */
      rrctcrc_clear_procedure();
      return;
    }

    if(tcrc.tcrc_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }
  } 
  if (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
        criticalExtensions.u.r7.m.transportChannelReconfiguration_r7_add_extPresent)
  {
    WRRC_MSG0_ERROR("RRCEUL: rrc_transportChannelReconfiguration_r7_add_ext_present not yet supported");
  } 
  
  if( (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
      criticalExtensions.u.r7.m.v780NonCriticalExtensionsPresent)
      &&(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7.v780NonCriticalExtensions.m.v7d0NonCriticalExtensionsPresent))
  {
    WRRC_MSG0_ERROR("RRCEUL: r7-nonCriticalExtensions not yet supported");
  }
}

/*===========================================================================

FUNCTION   VALIDATE_TCRECONFIG_R8

DESCRIPTION

  This function validates the received R6 TCRC Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a TCRC Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_tcreconfig_r8
(
  rrc_TransportChannelReconfiguration_r8_IEs* tcrc_ptr /* Pointer to rel-6 TCRC message */
)
{
  uecomdef_status_e_type status;

  /* Assume the message is valid for now */
  tcrc.status.failure_status = RRCTC_MSG_VALID;

  /* Check the RRC State indicatior given by the message */
  switch( tcrc_ptr->rrc_StateIndicator )
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      tcrc.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      tcrc.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      tcrc.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;

    case rrc_RRC_StateIndicator_ura_PCH:
      tcrc.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;

    default:
       
      /* No other states are supported right now. */
      WRRC_MSG0_HIGH("Unsupported state in TCRC Message");
      tcrc.status.failure_status = RRCTC_MSG_UNSUPPORTED_CONFIG;
      break;
  }

  if( tcrc.status.failure_status == RRCTC_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
     return;
  }

  if(RRCRB_R8_MSG_IE_PRESENT (tcrc_ptr,rrc_TransportChannelReconfiguration_r8_IEs,dl_HSPDSCH_Information) ) 
  {
    /* Check if network does not try to initiate HSDPA with invalid state */

    status = rrc_hsdpa_initial_validation(tcrc_ptr->rrc_StateIndicator, 
     RRC_CHECK_COMMON_MSG_TYPE(tcrc_ptr->dl_HSPDSCH_Information.modeSpecificInfo,
     rrc_DL_HSPDSCH_Information_modeSpecificInfo_tdd));

    if (status == FAILURE)
    {
      tcrc.status.failure_status = RRCTC_MSG_INVALID_CONFIG;
      return;
    }
  }
  
  /* Check if RB with PDCP Information list is present. This
  is not supported in the first release. This may be supported
  later */                                    
  if(RRCRB_R8_MSG_IE_PRESENT (tcrc_ptr,rrc_TransportChannelReconfiguration_r8_IEs,dl_CounterSynchronisationInfo) )
   {
  /*check whether the SRNS relocation is within Cell_DCH or not.
     SRNS relocation information can come in reconfiguration messages
     only when the state transition is Cell_DCH->Cell_DCH */
    if (!((tcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("SRNS reloc with invalid states cur_state %d, msg_state %d",
             rrc_get_state(), tcrc.state_indicated_by_msg);
      tcrc.status.failure_status=RRCTC_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has 
    PDCP info list. If it is true then this configuration is unsupported */
    if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2(tcrc_ptr->dl_CounterSynchronisationInfo,
        rb_WithPDCP_InfoList)) ||
        (RRC_MSG_COMMON_BITMASK_IE(tcrc_ptr->dl_CounterSynchronisationInfo,
         rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
    {
      WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
      tcrc.status.failure_status=RRCTC_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation. 
        Set the flag that indicates that this procedure
      involves SRNS relocation */
      tcrc.tcrc_for_srns_relocation = TRUE;
      
    /* store if new U-RNTI is present in the message */
      if(RRCRB_R8_MSG_IE_PRESENT (tcrc_ptr,rrc_TransportChannelReconfiguration_r8_IEs,new_U_RNTI) ) 
      {
        tcrc.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(tcrc_ptr->new_U_RNTI),
                             &tcrc.new_urnti);
      }
    }
  }

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  tcrc.trans_to_dch_ciph_config_needed = FALSE;
  if((tcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRCRB_R8_MSG_IE_PRESENT (tcrc_ptr,rrc_TransportChannelReconfiguration_r8_IEs,dl_CommonInformation) )
        && (RRC_MSG_COMMON_BITMASK_IE(tcrc_ptr->dl_CommonInformation,rrc_DL_CommonInformation_r8,dl_dpchInfoCommon))
        && (T_rrc_DL_CommonInformation_r8_dl_dpchInfoCommon_dl_DPCH_InfoCommon == 
         tcrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.t)
       )
    {
      //check if ciphering is active in mac for cs domain, if it is then,
      //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      if(rrcsmc_tm_ciph_active() == TRUE)
      {
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        uecomdef_status_e_type loc_ret_val;
        WRRC_MSG0_HIGH("To-DCH-Step-1 config activated");
        loc_ret_val = rrcrb_get_mac_d_hfn(
             (RRC_MSG_COMMON_BITMASK_IE_PTR(tcrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon,
               rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
              &tcrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
              &mac_d_hfn_l);
        if (loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("get_mac_d_hfn failed");
        }
        if(rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
        {
           WRRC_MSG0_HIGH("SMC process ciphering config for HHO failed");
        }
        tcrc.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }
  /*Cell_PCH/URA_PCH specific validations */
  if ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
     /* If UTRAN DRX coefficient is not present then it is invalid configuration */
     if (!(RRCRB_R8_MSG_IE_PRESENT (tcrc_ptr,rrc_TransportChannelReconfiguration_r8_IEs,utran_DRX_CycleLengthCoeff) )  )
     {
       WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", tcrc.state_indicated_by_msg);
       tcrc.status.failure_status = RRCTC_MSG_INVALID_CONFIG;
       return;
     }
     if (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
     {
       /*should we do it after getting l2ack for response??*/
       if (!(RRCRB_R8_MSG_IE_PRESENT (tcrc_ptr,rrc_TransportChannelReconfiguration_r8_IEs,ura_Identity) ) )
           
       {
         WRRC_MSG0_HIGH("URA ID not present");
         rrccu_clear_ura_id ();
       }
       else
       {
         /*URA ID present in the message set ura_identity variable with this value */
         if (tcrc_ptr->ura_Identity.numbits == 16)
         {
           rrccu_update_ura_id (&(tcrc_ptr->ura_Identity));
         }
         else
         {
           WRRC_MSG1_ERROR("Invalid URA ID length%d",
               tcrc_ptr->ura_Identity.numbits);
           tcrc.status.failure_status = RRCTC_MSG_PROTOCOL_ERROR;
           tcrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
           return;
         }
       }
     }
  }
}/* end function validate_tcreconfig_r5 */

/*===========================================================================

FUNCTION   PROCESS_TCRECONFIG_R8

DESCRIPTION

  This function processes the Rel 8 TCRC Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a TCRC Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to configure the transport channels given by the message.
    
    If ORDERED_CONFIG could not be set, a TCRC Failure message
    will be sent to UTRAN.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_tcreconfig_r8
(
  rrc_RRC_TransactionIdentifier transaction_id,

  rrc_DL_DCCH_Message *dcch_msg_ptr  ,
  /* Pointer to the RB Reconfig message        */
  rrc_TransportChannelReconfiguration_r8_IEs *tcreconfig_ptr
)
{

  rrc_TransportChannelReconfiguration *msg_ptr; /* Pointer to the TCRC message        */
  rrcllc_oc_status_e_type             oc_status;/* To hold ordered_config status          */

  rrc_state_e_type                    current_state; /* current RRC State  */
  rrc_int_c_rnti_type                 c_rnti;       /* Local C_RNTI value */
  rrc_int_u_rnti_type                 u_rnti;
  rrcrb_rnti_update_info_type         rnti_update_info;   /* to hold rnti update info */
  rrcrb_directed_cell_info_type       directed_cell_info; /* directed cell info struct */

  /* Used to indicate if an error occured while setting ordered_config. */
  boolean ordered_config_err_occured = FALSE; 
 
  /* Get the pointer to the actual TCRC message */
  msg_ptr = &(dcch_msg_ptr->message.u.transportChannelReconfiguration);
 

  if (FAILURE == 
      rrctcrc_check_and_send_failure_for_simul_reconfig(
        msg_ptr->u.later_than_r3.rrc_TransactionIdentifier))
  {
    return;
  }

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  validate_tcreconfig_r8( tcreconfig_ptr );


  if( tcrc.status.failure_status != RRCTC_MSG_VALID )
  {
    /* TCRC Message was invalid. */
    WRRC_MSG0_ERROR("TCRC Message validation failed");

    /* Send the TCRC Failure Message */
    send_tcrc_failure_message( tcrc.status.failure_status,
                              tcrc.status.prot_err_cause
                            );

    /* Make sure we're back in the initial sub-state */
    tcrc.curr_substate = RRCRB_INITIAL;
  }
  /* If the message is valid, proceed further */
  else
  {

    MSG_LOW("Processing TCRC Message", 0, 0, 0);

    /* Check if there is CN information present. If so, we need
    to send the information to the Cell Change manager */
    if(RRCRB_R8_MSG_IE_PRESENT (tcreconfig_ptr,rrc_TransportChannelReconfiguration_r8_IEs,cn_InformationInfo) ) 
    {
      WRRC_MSG0_HIGH("Saving CN info for NAS");
      tcrc.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(
        &(tcreconfig_ptr->cn_InformationInfo) );
      if (tcrc.mm_cmd_ptr == NULL)
      {
        WRRC_MSG0_ERROR("Could not build SERVICE_IND cmd");
      }
      else
      {
        /* set cn_info_saved flag */
        tcrc.cn_info_saved = TRUE;
      }

    }

     
#ifdef FEATURE_WCDMA_REL7_SPECRS
	/*If TCRC OTA provides "supportForChangeOfUE_Capability" IE, we need to store it in
           "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
           "rrc_rnc_cap_change_support" can be updated accordingly. */  
    if(RRCRB_R8_MSG_IE_PRESENT (tcreconfig_ptr,rrc_TransportChannelReconfiguration_r8_IEs,
		supportForChangeOfUE_Capability))
    {
      if(tcreconfig_ptr->supportForChangeOfUE_Capability == TRUE)
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




    /*If TCRC OTA provides "responseToChangeOfUE_Capability_present" IE, we need to store it so that later
          if procedure is successful, TCRC need to notify the COUEC procedure  about this IE.*/
          
    rrc_change_of_ue_cap_response = FALSE;          
    
    if(RRCRB_R8_MSG_IE_PRESENT (tcreconfig_ptr,rrc_TransportChannelReconfiguration_r8_IEs,
          responseToChangeOfUE_Capability))
    {
      rrc_change_of_ue_cap_response = TRUE;
    }        

#endif	
    if(RRCRB_R8_MSG_IE_PRESENT (tcreconfig_ptr,rrc_TransportChannelReconfiguration_r8_IEs,ueMobilityStateIndicator) ) 
    {
      if(tcreconfig_ptr->ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
      {
        WRRC_MSG0_HIGH("High Mobility Ind Set to TRUE in TCRC.");
        tcrc.high_mobility_ind = TRUE;
      }
    }
     
    /* We have to reconfigure Transport Channel. First set ORDERED_CONFIG
       so that LLC can process the necessary parameters. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_TCR,
                                           tcrc.state_indicated_by_msg,
                                           rrc_DL_DCCH_Message_PDU,
                                           (void *)dcch_msg_ptr 
                                         );

    switch( oc_status )
    {

      case ORDERED_CONFIG_SET:
        if(rrc_sib_is_current_event_sib_read_in_dch())
        {
          WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
          rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
        }
        current_state = rrc_get_state(); 
        /* Populate the local variables to indicate if Re-direction info is present */
        directed_cell_info.freq_info_present = FALSE;
        directed_cell_info.pscr_present = FALSE;   
        if((current_state==RRC_STATE_CELL_FACH) && 
               ((tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
               ||(tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
               ||(tcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          if((RRCRB_R8_MSG_IE_PRESENT (tcreconfig_ptr,rrc_TransportChannelReconfiguration_r8_IEs,frequencyInfo) )
             && (RRC_CHECK_COMMON_MSG_TYPE(tcreconfig_ptr->frequencyInfo.modeSpecificInfo,
                     rrc_FrequencyInfo_modeSpecificInfo_fdd)))
          {
            directed_cell_info.freq_info_present = TRUE;
            directed_cell_info.uarfcn_dl = 
              tcreconfig_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
          }
         
          if (rrctcrc_is_pcpich_info_present(msg_ptr, MSG_REL8))
          {
            directed_cell_info.pscr_present = TRUE;
            directed_cell_info.pscr = tcreconfig_ptr->dl_InformationPerRL_List.elem[0].
                                      modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
          }      
          /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
                 FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
          if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
            (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
          {
            WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
            if(tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
            {
              rrctcrc_send_initiate_cu_flag = TRUE;
              rrctcrc_delete_crnti          = TRUE;
            }
            else
            {
              tcrc.directed_cell_info = TRUE;
            }
          }  
        }
        if (
            (current_state==tcrc.state_indicated_by_msg)||
            ((current_state==RRC_STATE_CELL_FACH)&& 
             (tcrc.state_indicated_by_msg==RRC_STATE_CELL_DCH))
           )
        {
          /*  CellDCH<->CellDCH or CellFACH<->CellFACH or CellFACH->CellDCH
           transition.  Ordered Config has been set. We can setup the
           Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */

           /*check whether any frequency info is present in the message
           and transition is CELL_FACH->CELL_FACH. In this case
           initiate a cell update after channel config is successfully completed */
           if ((current_state==RRC_STATE_CELL_FACH)&& 
              (tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
           {
             /*call the function that evaluates the directed cell info and set the flags accordingly */
             if ((rrctcrc_send_initiate_cu_flag == FALSE) &&
                (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
             {
               if(RRCRB_R8_MSG_IE_PRESENT(tcreconfig_ptr,rrc_TransportChannelReconfiguration_r8_IEs,new_C_RNTI)) 
               {
                 tcrc.new_crnti_valid = TRUE;
                 rrc_translate_crnti(&tcreconfig_ptr->new_C_RNTI, 
                                       &c_rnti);
                 tcrc.new_crnti = (uint16)c_rnti;
               }
               else
               {
                 tcrc.new_crnti_valid = FALSE;
               }
     
               /* Now copy OC to TOC */
               rrcllcoc_update_toc_with_oc_from_fach();
               /*Initiate the cell selection*/
               rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
                                                        RRC_TRANSITION_FROM_FACH,
                                                        tcrc.state_indicated_by_msg);
                 
               tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
          
               /* exit from the switch case */
               break;
             }
    
           } /* if CELL_FACH->CELL_FACH reconfiguration */

           /* Call the function that creates and sends a channel
           config request to LLC. */
           rrctcrc_send_channel_config_req( );

           /* Change the substate since we're waiting for the
           confirmation from LLC for the channel configs */
           tcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                  (tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
        {
          if(RRCRB_R8_MSG_IE_PRESENT(tcreconfig_ptr,rrc_TransportChannelReconfiguration_r8_IEs,new_C_RNTI) )
          {
             tcrc.new_crnti_valid = TRUE;
             rrc_translate_crnti(&tcreconfig_ptr->new_C_RNTI, 
                                  &c_rnti);
             tcrc.new_crnti = (uint16)c_rnti;
          }
          else
          {
            tcrc.new_crnti_valid = FALSE;
          }

          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_TCR,
                                                  RRC_TRANSITION_FROM_DCH,
                                                  tcrc.state_indicated_by_msg);

          tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

          /* Start T_305 [periodic cell update] timer */
          rrccu_start_t_305_timer();
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                 (tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH))
        {
          /*store the directed cell info */
           tcrc.directed_cell_info = rrctcrc_is_pcpich_info_present(msg_ptr, MSG_REL6);

           /*Send the TCRC complete msg on old configuration */
           send_tcrc_complete_message(TRUE);
           /*wait for L2ack */
           tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
        
           /* prints rate for each transport channel */
           rrcllc_print_trans_chnl_rate();

        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (tcrc.state_indicated_by_msg==RRC_STATE_URA_PCH))
        {
          /*Send the TCRC complete msg on old configuration */
          send_tcrc_complete_message(TRUE);
          /*wait for L2ack */
          tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
        
          /* prints rate for each transport channel */
          rrcllc_print_trans_chnl_rate();
          
        }
        else if((current_state==RRC_STATE_CELL_FACH) && 
                ((tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
                 (tcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          /*call the function that evaluates the directed cell info and set the flags accordingly */
          if ((tcrc.directed_cell_info == FALSE) &&
            (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
          {  
            if(tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
            {
              tcrc.directed_cell_info = rrctcrc_is_pcpich_info_present(msg_ptr, MSG_REL8);
            }

            tcrc.initiate_cell_selection_trans_from_fach = TRUE;
          }
    

          /* check whether CPICH info is present or not. Store this
          info  */
         // tcrc.directed_cell_info = rrctcrc_is_pcpich_info_present(msg_ptr);
          /*Send the RRCTCTC complete msg on old configuration */
          /* check whether new C-RNTI was present in the message. In that case
          we have to configure MAC with new C-RNTI before sending the response 
          message */
          if(RRCRB_R8_MSG_IE_PRESENT(tcreconfig_ptr,rrc_TransportChannelReconfiguration_r8_IEs,new_C_RNTI) )
          {
            rrc_translate_crnti(&tcreconfig_ptr->new_C_RNTI, 
                                &c_rnti);
            /* Send a RNTI Update Request */
            rnti_update_info.crnti_action = RRC_RNTI_UPDATE;
            rnti_update_info.crnti        = (uint16)c_rnti;
            /* check whether URNTI is also present in the message */
            if (RRCRB_R8_MSG_IE_PRESENT(tcreconfig_ptr,rrc_TransportChannelReconfiguration_r8_IEs,new_U_RNTI) )
            {
              rrc_translate_urnti(&tcreconfig_ptr->new_U_RNTI, 
                                  &u_rnti);
              rnti_update_info.urnti_action = RRC_RNTI_UPDATE; 
              rnti_update_info.urnti        = u_rnti; 
            }
            else
            {
              rnti_update_info.urnti_action = RRC_RNTI_NOCHANGE; 
            }
            rnti_update_info.procedure            = RRC_PROCEDURE_TCR;
            rnti_update_info.rlc_re_establish_srb = FALSE;
            rnti_update_info.rlc_re_establish_rab = FALSE;
            rnti_update_info.cnf_reqd             = TRUE;
            rrcrb_send_rnti_update_req(rnti_update_info);
            /* wait for the RNTI update confirm before sending the response
            message */
            tcrc.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
          }
          else 
          {
            send_tcrc_complete_message(TRUE);
            /*wait for L2ack */
            tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
            /* prints rate for each transport channel */
            rrcllc_print_trans_chnl_rate();

          }
        }
        break;

      case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:
        tcrc.status.failure_status = RRCTC_SIMULTANEOUS_RECONFIG;
        /* Send the TCRC Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause );

        /* Go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:
        tcrc.status.failure_status =RRCTC_MSG_UNSUPPORTED_CONFIG;
        /* Send the TCRC Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause );

        /* Go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

       case ORDERED_CONFIG_CONFIGURATION_INVALID:
        tcrc.status.failure_status =RRCTC_MSG_INVALID_CONFIG;
        /* Send the TCRC Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause );

        /* Go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
      
      case ORDERED_CONFIG_NOT_SET_OTHER:
        WRRC_MSG0_HIGH("LLC returned other failure!");
        /* When LLC sends this, there is a serious problem
        and we'll go back to disconnected state. No need
        to send the failure message here, we can just
        go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME        
      case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("TCR processing deferred due to LTA!");
      
        tcrc.curr_substate = RRCRB_INITIAL;
      
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
      default:

        WRRC_MSG0_ERROR("Unknown return value from LLC");
        /* Unknown error, just go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
    }/* end switch */
    
    if( ordered_config_err_occured == TRUE )
    {
      /* No need to continue. Clear the procedure
      variables and return. */
      rrctcrc_clear_procedure();
      return;
    }

    if(tcrc.tcrc_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }

  } 

}

#ifdef FEATURE_WCDMA_REL9

/*===========================================================================

FUNCTION   VALIDATE_TCRECONFIG_R9

DESCRIPTION

  This function validates the received R9 TCRC Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a TCRC Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_tcreconfig_r9
(
  rrc_TransportChannelReconfiguration_r9_IEs* tcrc_ptr /* Pointer to rel-6 TCRC message */
)
{
  uecomdef_status_e_type status;

  /* Assume the message is valid for now */
  tcrc.status.failure_status = RRCTC_MSG_VALID;

  /* Check the RRC State indicatior given by the message */
  switch( tcrc_ptr->rrc_StateIndicator )
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      tcrc.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      tcrc.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      tcrc.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;

    case rrc_RRC_StateIndicator_ura_PCH:
      tcrc.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;

    default:
       
      /* No other states are supported right now. */
      WRRC_MSG0_HIGH("Unsupported state in TCRC Message");
      tcrc.status.failure_status = RRCTC_MSG_UNSUPPORTED_CONFIG;
      break;
  }

  if( tcrc.status.failure_status == RRCTC_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
     return;
  }

  if(RRCRB_R9_MSG_IE_PRESENT(tcrc_ptr,dl_HSPDSCH_Information) ) 
  {
    
    /* Check if network does not try to initiate HSDPA with invalid state */

    status = rrc_hsdpa_initial_validation(tcrc_ptr->rrc_StateIndicator, 
     RRC_CHECK_COMMON_MSG_TYPE(tcrc_ptr->dl_HSPDSCH_Information.modeSpecificInfo,
     rrc_DL_HSPDSCH_Information_r9_modeSpecificInfo_tdd));

    if (status == FAILURE)
    {
      tcrc.status.failure_status = RRCTC_MSG_INVALID_CONFIG;
      return;
    }
  }
  
  /* Check if RB with PDCP Information list is present. This
  is not supported in the first release. This may be supported
  later */                                    
  if(RRCRB_R9_MSG_IE_PRESENT(tcrc_ptr,dl_CounterSynchronisationInfo) )
   {
  /*check whether the SRNS relocation is within Cell_DCH or not.
     SRNS relocation information can come in reconfiguration messages
     only when the state transition is Cell_DCH->Cell_DCH */
    if (!((tcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("SRNS reloc with invalid states cur_state %d, msg_state %d",
             rrc_get_state(), tcrc.state_indicated_by_msg);
      tcrc.status.failure_status=RRCTC_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has 
    PDCP info list. If it is true then this configuration is unsupported */
    if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2(tcrc_ptr->dl_CounterSynchronisationInfo,
        rb_WithPDCP_InfoList)) ||
        (RRC_MSG_COMMON_BITMASK_IE(tcrc_ptr->dl_CounterSynchronisationInfo,
         rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
    {
      WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
      tcrc.status.failure_status=RRCTC_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation. 
        Set the flag that indicates that this procedure
      involves SRNS relocation */
      tcrc.tcrc_for_srns_relocation = TRUE;
      
    /* store if new U-RNTI is present in the message */
      if(RRCRB_R9_MSG_IE_PRESENT(tcrc_ptr,new_U_RNTI) ) 
      {
        tcrc.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(tcrc_ptr->new_U_RNTI),
                             &tcrc.new_urnti);
      }
    }
  }

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  tcrc.trans_to_dch_ciph_config_needed = FALSE;
  if((tcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRCRB_R9_MSG_IE_PRESENT(tcrc_ptr,dl_CommonInformation) )
        && (RRC_MSG_COMMON_BITMASK_IE(tcrc_ptr->dl_CommonInformation,rrc_DL_CommonInformation_r8,dl_dpchInfoCommon))
        && (T_rrc_DL_CommonInformation_r8_dl_dpchInfoCommon_dl_DPCH_InfoCommon == 
         tcrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.t)
       )
    {
      //check if ciphering is active in mac for cs domain, if it is then,
      //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      if(rrcsmc_tm_ciph_active() == TRUE)
      {
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        uecomdef_status_e_type loc_ret_val;
        WRRC_MSG0_HIGH("To-DCH-Step-1 config activated");
        loc_ret_val = rrcrb_get_mac_d_hfn(
             (RRC_MSG_COMMON_BITMASK_IE_PTR(tcrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon,
               rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
              &tcrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
              &mac_d_hfn_l);
        if (loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("get_mac_d_hfn failed");
        }
        if(rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
        {
           WRRC_MSG0_HIGH("SMC process ciphering config for HHO failed");
        }
        tcrc.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }
  /*Cell_PCH/URA_PCH specific validations */
  if ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
     /* If UTRAN DRX coefficient is not present then it is invalid configuration */
     if (!(RRCRB_R9_MSG_IE_PRESENT(tcrc_ptr,utran_DRX_CycleLengthCoeff) )  )
     {
       WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", tcrc.state_indicated_by_msg);
       tcrc.status.failure_status = RRCTC_MSG_INVALID_CONFIG;
       return;
     }
     if (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
     {
       /*should we do it after getting l2ack for response??*/
       if (!(RRCRB_R9_MSG_IE_PRESENT(tcrc_ptr,ura_Identity) ) )
           
       {
         WRRC_MSG0_HIGH("URA ID not present");
         rrccu_clear_ura_id ();
       }
       else
       {
         /*URA ID present in the message set ura_identity variable with this value */
         if (tcrc_ptr->ura_Identity.numbits == 16)
         {
           rrccu_update_ura_id (&(tcrc_ptr->ura_Identity));
         }
         else
         {
           WRRC_MSG1_ERROR("Invalid URA ID length%d",
               tcrc_ptr->ura_Identity.numbits);
           tcrc.status.failure_status = RRCTC_MSG_PROTOCOL_ERROR;
           tcrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
           return;
         }
       }
     }
  }
}/* end function validate_tcreconfig_r5 */

/*===========================================================================

FUNCTION   PROCESS_TCRECONFIG_R9

DESCRIPTION

  This function processes the Rel 9 TCRC Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a TCRC Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to configure the transport channels given by the message.
    
    If ORDERED_CONFIG could not be set, a TCRC Failure message
    will be sent to UTRAN.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_tcreconfig_r9
(
  rrc_RRC_TransactionIdentifier transaction_id,

  rrc_DL_DCCH_Message *dcch_msg_ptr  ,
  /* Pointer to the RB Reconfig message        */
  rrc_TransportChannelReconfiguration_r9_IEs *tcreconfig_ptr
)
{

  rrc_TransportChannelReconfiguration *msg_ptr; /* Pointer to the TCRC message        */
  rrcllc_oc_status_e_type             oc_status;/* To hold ordered_config status          */

  rrc_state_e_type                    current_state; /* current RRC State  */
  rrc_int_c_rnti_type                 c_rnti;       /* Local C_RNTI value */
  rrc_int_u_rnti_type                 u_rnti;
  rrcrb_rnti_update_info_type         rnti_update_info;   /* to hold rnti update info */
  rrcrb_directed_cell_info_type       directed_cell_info; /* directed cell info struct */

  /* Used to indicate if an error occured while setting ordered_config. */
  boolean ordered_config_err_occured = FALSE; 
 
  /* Get the pointer to the actual TCRC message */
  msg_ptr = &(dcch_msg_ptr->message.u.transportChannelReconfiguration);
 

  if (FAILURE == 
      rrctcrc_check_and_send_failure_for_simul_reconfig(
        msg_ptr->u.later_than_r3.rrc_TransactionIdentifier))
  {
    return;
  }

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  validate_tcreconfig_r9( tcreconfig_ptr );


  if( tcrc.status.failure_status != RRCTC_MSG_VALID )
  {
    /* TCRC Message was invalid. */
    WRRC_MSG0_ERROR("TCRC Message validation failed");

    /* Send the TCRC Failure Message */
    send_tcrc_failure_message( tcrc.status.failure_status,
                              tcrc.status.prot_err_cause
                            );

    /* Make sure we're back in the initial sub-state */
    tcrc.curr_substate = RRCRB_INITIAL;
  }
  /* If the message is valid, proceed further */
  else
  {

    MSG_LOW("Processing TCRC Message", 0, 0, 0);

    /* Check if there is CN information present. If so, we need
    to send the information to the Cell Change manager */
    if(RRCRB_R9_MSG_IE_PRESENT(tcreconfig_ptr,cn_InformationInfo) ) 
    {
      WRRC_MSG0_HIGH("Saving CN info for NAS");
      tcrc.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(
        &(tcreconfig_ptr->cn_InformationInfo) );
      if (tcrc.mm_cmd_ptr == NULL)
      {
        WRRC_MSG0_ERROR("Could not build SERVICE_IND cmd");
      }
      else
      {
        /* set cn_info_saved flag */
        tcrc.cn_info_saved = TRUE;
      }

    }

     
#ifdef FEATURE_WCDMA_REL7_SPECRS
  /*If TCRC OTA provides "supportForChangeOfUE_Capability" IE, we need to store it in
           "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
           "rrc_rnc_cap_change_support" can be updated accordingly. */  
    if(RRCRB_R9_MSG_IE_PRESENT(tcreconfig_ptr,
    supportForChangeOfUE_Capability))
    {
      if(tcreconfig_ptr->supportForChangeOfUE_Capability == TRUE)
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




    /*If TCRC OTA provides "responseToChangeOfUE_Capability_present" IE, we need to store it so that later
          if procedure is successful, TCRC need to notify the COUEC procedure  about this IE.*/
          
    rrc_change_of_ue_cap_response = FALSE;          
    
    if(RRCRB_R9_MSG_IE_PRESENT(tcreconfig_ptr,
          responseToChangeOfUE_Capability))
    {
      rrc_change_of_ue_cap_response = TRUE;
    }        

#endif	
    if(RRCRB_R9_MSG_IE_PRESENT(tcreconfig_ptr,ueMobilityStateIndicator) ) 
    {
      if(tcreconfig_ptr->ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
      {
        WRRC_MSG0_HIGH("High Mobility Ind Set to TRUE in TCRC.");
        tcrc.high_mobility_ind = TRUE;
      }
    }
     
    /* We have to reconfigure Transport Channel. First set ORDERED_CONFIG
       so that LLC can process the necessary parameters. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_TCR,
                                           tcrc.state_indicated_by_msg,
                                           rrc_DL_DCCH_Message_PDU,
                                           (void *)dcch_msg_ptr 
                                         );

    switch( oc_status )
    {

      case ORDERED_CONFIG_SET:
        if(rrc_sib_is_current_event_sib_read_in_dch())
        {
          WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
          rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
        }
        current_state = rrc_get_state(); 
        /* Populate the local variables to indicate if Re-direction info is present */
        directed_cell_info.freq_info_present = FALSE;
        directed_cell_info.pscr_present = FALSE;   
        if((current_state==RRC_STATE_CELL_FACH) && 
               ((tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
               ||(tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
               ||(tcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          if((RRCRB_R9_MSG_IE_PRESENT(tcreconfig_ptr,frequencyInfo) )
             && (RRC_CHECK_COMMON_MSG_TYPE(tcreconfig_ptr->frequencyInfo.modeSpecificInfo,
                     rrc_FrequencyInfo_modeSpecificInfo_fdd)))
          {
            directed_cell_info.freq_info_present = TRUE;
            directed_cell_info.uarfcn_dl = 
              tcreconfig_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
          }
         
          if (rrctcrc_is_pcpich_info_present(msg_ptr, MSG_REL9))
          {
            directed_cell_info.pscr_present = TRUE;
            directed_cell_info.pscr = tcreconfig_ptr->dl_InformationPerRL_List.elem[0].
                                      modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
          }      
          /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
                 FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
          if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
            (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
          {
            WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
            if(tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
            {
              rrctcrc_send_initiate_cu_flag = TRUE;
              rrctcrc_delete_crnti          = TRUE;
            }
            else
            {
              tcrc.directed_cell_info = TRUE;
            }
          }  
        }
        if (
            (current_state==tcrc.state_indicated_by_msg)||
            ((current_state==RRC_STATE_CELL_FACH)&& 
             (tcrc.state_indicated_by_msg==RRC_STATE_CELL_DCH))
           )
        {
          /*  CellDCH<->CellDCH or CellFACH<->CellFACH or CellFACH->CellDCH
           transition.  Ordered Config has been set. We can setup the
           Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */

           /*check whether any frequency info is present in the message
           and transition is CELL_FACH->CELL_FACH. In this case
           initiate a cell update after channel config is successfully completed */
           if ((current_state==RRC_STATE_CELL_FACH)&& 
              (tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
           {
             /*call the function that evaluates the directed cell info and set the flags accordingly */
             if ((rrctcrc_send_initiate_cu_flag == FALSE) &&
                (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
             {
               if(RRCRB_R9_MSG_IE_PRESENT(tcreconfig_ptr,new_C_RNTI)) 
               {
                 tcrc.new_crnti_valid = TRUE;
                 rrc_translate_crnti(&tcreconfig_ptr->new_C_RNTI, 
                                       &c_rnti);
                 tcrc.new_crnti = (uint16)c_rnti;
               }
               else
               {
                 tcrc.new_crnti_valid = FALSE;
               }
     
               /* Now copy OC to TOC */
               rrcllcoc_update_toc_with_oc_from_fach();
               /*Initiate the cell selection*/
               rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
                                                        RRC_TRANSITION_FROM_FACH,
                                                        tcrc.state_indicated_by_msg);
                 
               tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
          
               /* exit from the switch case */
               break;
             }
    
           } /* if CELL_FACH->CELL_FACH reconfiguration */

           /* Call the function that creates and sends a channel
           config request to LLC. */
           rrctcrc_send_channel_config_req( );

           /* Change the substate since we're waiting for the
           confirmation from LLC for the channel configs */
           tcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                  (tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
        {
          if(RRCRB_R9_MSG_IE_PRESENT(tcreconfig_ptr,new_C_RNTI) )
          {
             tcrc.new_crnti_valid = TRUE;
             rrc_translate_crnti(&tcreconfig_ptr->new_C_RNTI, 
                                  &c_rnti);
             tcrc.new_crnti = (uint16)c_rnti;
          }
          else
          {
            tcrc.new_crnti_valid = FALSE;
          }

          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_TCR,
                                                  RRC_TRANSITION_FROM_DCH,
                                                  tcrc.state_indicated_by_msg);

          tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

          /* Start T_305 [periodic cell update] timer */
          rrccu_start_t_305_timer();
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                 (tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH))
        {
          /*store the directed cell info */
           tcrc.directed_cell_info = rrctcrc_is_pcpich_info_present(msg_ptr, MSG_REL6);

           /*Send the TCRC complete msg on old configuration */
           send_tcrc_complete_message(TRUE);
           /*wait for L2ack */
           tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
        
           /* prints rate for each transport channel */
           rrcllc_print_trans_chnl_rate();

        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (tcrc.state_indicated_by_msg==RRC_STATE_URA_PCH))
        {
          /*Send the TCRC complete msg on old configuration */
          send_tcrc_complete_message(TRUE);
          /*wait for L2ack */
          tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
        
          /* prints rate for each transport channel */
          rrcllc_print_trans_chnl_rate();
          
        }
        else if((current_state==RRC_STATE_CELL_FACH) && 
                ((tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
                 (tcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          /*call the function that evaluates the directed cell info and set the flags accordingly */
          if ((tcrc.directed_cell_info == FALSE) &&
            (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
          {  
            if(tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
            {
              tcrc.directed_cell_info = rrctcrc_is_pcpich_info_present(msg_ptr, MSG_REL9);
            }

            tcrc.initiate_cell_selection_trans_from_fach = TRUE;
          }
    

          /* check whether CPICH info is present or not. Store this
          info  */
         // tcrc.directed_cell_info = rrctcrc_is_pcpich_info_present(msg_ptr);
          /*Send the RRCTCTC complete msg on old configuration */
          /* check whether new C-RNTI was present in the message. In that case
          we have to configure MAC with new C-RNTI before sending the response 
          message */
          if(RRCRB_R9_MSG_IE_PRESENT(tcreconfig_ptr,new_C_RNTI) )
          {
            rrc_translate_crnti(&tcreconfig_ptr->new_C_RNTI, 
                                &c_rnti);
            /* Send a RNTI Update Request */
            rnti_update_info.crnti_action = RRC_RNTI_UPDATE;
            rnti_update_info.crnti        = (uint16)c_rnti;
            /* check whether URNTI is also present in the message */
            if (RRCRB_R9_MSG_IE_PRESENT(tcreconfig_ptr,new_U_RNTI) )
            {
              rrc_translate_urnti(&tcreconfig_ptr->new_U_RNTI, 
                                  &u_rnti);
              rnti_update_info.urnti_action = RRC_RNTI_UPDATE; 
              rnti_update_info.urnti        = u_rnti; 
            }
            else
            {
              rnti_update_info.urnti_action = RRC_RNTI_NOCHANGE; 
            }
            rnti_update_info.procedure            = RRC_PROCEDURE_TCR;
            rnti_update_info.rlc_re_establish_srb = FALSE;
            rnti_update_info.rlc_re_establish_rab = FALSE;
            rnti_update_info.cnf_reqd             = TRUE;
            rrcrb_send_rnti_update_req(rnti_update_info);
            /* wait for the RNTI update confirm before sending the response
            message */
            tcrc.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
          }
          else 
          {
            send_tcrc_complete_message(TRUE);
            /*wait for L2ack */
            tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
            /* prints rate for each transport channel */
            rrcllc_print_trans_chnl_rate();

          }
        }
        break;

      case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:
        tcrc.status.failure_status = RRCTC_SIMULTANEOUS_RECONFIG;
        /* Send the TCRC Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause );

        /* Go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:
        tcrc.status.failure_status =RRCTC_MSG_UNSUPPORTED_CONFIG;
        /* Send the TCRC Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause );

        /* Go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

       case ORDERED_CONFIG_CONFIGURATION_INVALID:
        tcrc.status.failure_status =RRCTC_MSG_INVALID_CONFIG;
        /* Send the TCRC Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause );

        /* Go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
      
      case ORDERED_CONFIG_NOT_SET_OTHER:
        WRRC_MSG0_HIGH("LLC returned other failure!");
        /* When LLC sends this, there is a serious problem
        and we'll go back to disconnected state. No need
        to send the failure message here, we can just
        go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME          
      case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("TCR processing deferred due to LTA!");
      
        tcrc.curr_substate = RRCRB_INITIAL;
      
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
      default:

        WRRC_MSG0_ERROR("Unknown return value from LLC");
        /* Unknown error, just go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
    }/* end switch */
    
    if( ordered_config_err_occured == TRUE )
    {
      /* No need to continue. Clear the procedure
      variables and return. */
      rrctcrc_clear_procedure();
      return;
    }

    if(tcrc.tcrc_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }

  } 

}




#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif /* FEATURE_WCDMA_REL10 */


/*===========================================================================

FUNCTION   rrctcrc_check_and_get_primary_plmn_id

DESCRIPTION

  This function will check whether primary plmn id is present 
  in r3,r5 path of TCR message. If present then it will return 
  primary plmn id.
        
DEPENDENCIES
  None.

RETURN VALUE
TRUE : IF Primary PLMN I.D is present in TCR message in r3,r5 path
FFALSE: Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
static boolean rrctcrc_check_and_get_primary_plmn_id
(
  /* Pointer to the TCR message */
  rrc_TransportChannelReconfiguration *msg_ptr,
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
  if (MSG_REL99 == msg_version)
  {
      /*Check to see if primary PLMN I.D is present in TCR message ext
       * rrc_TransportChannelReconfiguration-v690ext which is present in 
       * v690NonCriticalExtensions in r99 path
       */
      if((msg_ptr->u.r3.m.v3a0NonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.v3a0NonCriticalExtensions.m.laterNonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
           v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
           v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v690NonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
           v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
           transportChannelReconfiguration_v690ext.m.primary_plmn_IdentityPresent)
        )
      {
        primary_plmn_Id_present = TRUE;
        *primary_plmn_Identity = msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
           v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
           transportChannelReconfiguration_v690ext.primary_plmn_Identity;
      }
  }
  else if (MSG_REL5 == msg_version)
  {
      /*Check to see if primary PLMN I.D is present in TCR message ext
       * rrc_TransportChannelReconfiguration-v690ext which is present in 
       * v690NonCriticalExtensions in r5 path
       */
      if((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.m.v690NonCriticalExtensionsPresent)&&
         (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.
          v690NonCriticalExtensions.transportChannelReconfiguration_v690ext.m.primary_plmn_IdentityPresent)
       )
      {
        primary_plmn_Id_present = TRUE;
        *primary_plmn_Identity = msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.
           v690NonCriticalExtensions.transportChannelReconfiguration_v690ext.primary_plmn_Identity;
      }
  }
  WRRC_MSG2_HIGH("NWS:TCR msg contains primary_plmn_Id %d for msg_version:%d(0-MSG_REL99,1-MSG_REL5,2-MSG_REL6,3-MSG_REL7,4-MSG_REL8,5-MSG_REL9,6-MSG_REL10,other-MSG_INVALID)",primary_plmn_Id_present,msg_version);  
  return primary_plmn_Id_present;
}
/*===========================================================================

FUNCTION   rrctc_process_high_mobility_ind_in_non_critical_ext

DESCRIPTION

  This function will process High Mobility Ind received in Non-critical ext
        
DEPENDENCIES
  None.

RETURN VALUE
None

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctc_process_high_mobility_ind_in_non_critical_ext
(
  rrc_TransportChannelReconfiguration *msg_ptr,
  rrc_msg_ver_enum_type msg_version
)
{
  if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
  {
    if(msg_version == MSG_REL5)
    {
      if((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.m.v690NonCriticalExtensionsPresent) &&
       (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.v690NonCriticalExtensions.m.
       v770NonCriticalExtensionsPresent) &
       (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.v690NonCriticalExtensions.v770NonCriticalExtensions.
       transportChannelReconfiguration_v770ext.m.ueMobilityStateIndicatorPresent))
      {
        if(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.v690NonCriticalExtensions.v770NonCriticalExtensions.
            transportChannelReconfiguration_v770ext.ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
        {
          tcrc.high_mobility_ind = TRUE;
        }
      }
    }
    else if(msg_version == MSG_REL6)
    {
      if((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.v6b0NonCriticalExtensions.m.v770NonCriticalExtensionsPresent) &&
      (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.v6b0NonCriticalExtensions.v770NonCriticalExtensions.
      transportChannelReconfiguration_v770ext.m.ueMobilityStateIndicatorPresent))
      {
        if(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.v6b0NonCriticalExtensions.v770NonCriticalExtensions.transportChannelReconfiguration_v770ext.ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
        {
          tcrc.high_mobility_ind = TRUE;
        }
      }
      
    }
    else if(msg_version == MSG_REL99)
    {
      if((msg_ptr->u.r3.m.v3a0NonCriticalExtensionsPresent) &&
        (msg_ptr->u.r3.v3a0NonCriticalExtensions.m.laterNonCriticalExtensionsPresent) &&
        (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent) &&
        (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent) &&
        (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.
        v690NonCriticalExtensionsPresent) &&
        (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
        m.v770NonCriticalExtensionsPresent) &&
        (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
        v770NonCriticalExtensions.transportChannelReconfiguration_v770ext.m.ueMobilityStateIndicatorPresent))
      {
        if(msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
            v770NonCriticalExtensions.transportChannelReconfiguration_v770ext.ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
        {
          tcrc.high_mobility_ind = TRUE;
        }
      }
    }  
  }
  WRRC_MSG2_HIGH("High Mobility Ind Set to %d in TCRC.for msg_version:%d(0-MSG_REL99,1-MSG_REL5,2-MSG_REL6,3-MSG_REL7,4-MSG_REL8,5-MSG_REL9,6-MSG_REL10,other-MSG_INVALID)",tcrc.high_mobility_ind,msg_version);
}

/*===========================================================================

FUNCTION   PROCESS_TCRECONFIG_R5

DESCRIPTION

  This function processes the Rel 5 TCRC Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a TCRC Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to configure the transport channels given by the message.
    
    If ORDERED_CONFIG could not be set, a TCRC Failure message
    will be sent to UTRAN.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_tcreconfig_r5
(
  rrc_cmd_type *cmd_ptr      /* Pointer to the TCRC message */
)
{

  rrc_TransportChannelReconfiguration *msg_ptr; /* Pointer to the TCRC message        */
  rrcllc_oc_status_e_type             oc_status;/* To hold ordered_config status          */

  rrc_state_e_type                    current_state; /* current RRC State  */
  rrc_int_c_rnti_type                 c_rnti;       /* Local C_RNTI value */
  rrc_int_u_rnti_type                 u_rnti;
  rrcrb_rnti_update_info_type         rnti_update_info;   /* to hold rnti update info */
  rrc_DL_DCCH_Message                 *dcch_msg_ptr; /* Point to the DCCH message     */
  rrcrb_directed_cell_info_type       directed_cell_info; /* directed cell info struct */

  /* Pointer to message */
  rrc_TransportChannelReconfiguration_r5_IEs* tcreconfig_ptr = NULL;

  /* Used to indicate if an error occured while setting ordered_config. */
  boolean ordered_config_err_occured = FALSE; 

   /*Boolean used to indicate primary plmn i.d is present in OTA msg*/
  boolean primary_plmn_Id_present = FALSE;
   /*Variable to hold primary plmn i.d if present in OTA msg*/
  rrc_PLMN_Identity primary_plmn_Identity;


  /* A Downlink SDU has been received from RLC. This
  SDU should contain a Transport Channel Reconfiguration Message. Otherwise
  we would not have received this command. Copy the 
  received message pointer. */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
                     downlink_sdu_ind.decoded_msg;
  
  /* Get the pointer to the actual TCRC message */
  msg_ptr = &(dcch_msg_ptr->message.u.transportChannelReconfiguration);
 
  if (FAILURE == 
      rrctcrc_check_and_send_failure_for_simul_reconfig(
        msg_ptr->u.later_than_r3.rrc_TransactionIdentifier))
  {
    return;
  }

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  validate_tcreconfig_r5( msg_ptr );

  /* Store message pointer in a local variable */
  tcreconfig_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.
    criticalExtensions.u.r5.transportChannelReconfiguration_r5;


  if( tcrc.status.failure_status != RRCTC_MSG_VALID )
  {
    /* TCRC Message was invalid. */
    WRRC_MSG0_ERROR("TCRC Message validation failed");

    /* Send the TCRC Failure Message */
    send_tcrc_failure_message( tcrc.status.failure_status,
                              tcrc.status.prot_err_cause
                            );

    /* Make sure we're back in the initial sub-state */
    tcrc.curr_substate = RRCRB_INITIAL;
  }
  /* If the message is valid, proceed further */
  else
  {

    MSG_LOW("Processing TCRC Message", 0, 0, 0);

    /* Check if there is CN information present. If so, we need
    to send the information to the Cell Change manager */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(tcreconfig_ptr,
        rrc_TransportChannelReconfiguration_r5_IEs,cn_InformationInfo))
    {
      WRRC_MSG0_HIGH("Saving CN info for NAS");
    
      /*Check to see if primary PLMN I.D is present in TCR message ext
       * rrc_TransportChannelReconfiguration-v690ext which is present in 
       * v690NonCriticalExtensions
       */
       primary_plmn_Id_present = rrctcrc_check_and_get_primary_plmn_id(msg_ptr,
                                                                  &primary_plmn_Identity,MSG_REL5);
    
      tcrc.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info(
                     &(tcreconfig_ptr->cn_InformationInfo) 
                     ,primary_plmn_Id_present
                     ,&primary_plmn_Identity
           );
      if (tcrc.mm_cmd_ptr != NULL)
      {
        /* set cn_info_saved flag */
        tcrc.cn_info_saved = TRUE;
      }

    }


    rrctc_process_high_mobility_ind_in_non_critical_ext(msg_ptr,MSG_REL5);


     
    /* We have to reconfigure Transport Channel. First set ORDERED_CONFIG
       so that LLC can process the necessary parameters. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_TCR,
                                           tcrc.state_indicated_by_msg,
                                           rrc_DL_DCCH_Message_PDU,
                                           (void *)dcch_msg_ptr 
                                         );

    switch( oc_status )
    {

      case ORDERED_CONFIG_SET:
        if((rrc_sib_is_current_event_sib_read_in_dch()) &&
           (RRC_PROCEDURE_GPS == rrc_sib_get_proc_id_for_sib_read_in_dch()))
        {
          WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
          rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
        }
        current_state = rrc_get_state(); 

        /* Populate the local variables to indicate if Re-direction info is present */
        directed_cell_info.freq_info_present = FALSE;
        directed_cell_info.pscr_present = FALSE;   
        if((current_state==RRC_STATE_CELL_FACH) && 
               ((tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
               ||(tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
               ||(tcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          if((RRC_MSG_COMMON_BITMASK_IE_PTR(tcreconfig_ptr, 
                  rrc_TransportChannelReconfiguration_r5_IEs,frequencyInfo)) &&
                 (RRC_CHECK_COMMON_MSG_TYPE(tcreconfig_ptr->frequencyInfo.
                  modeSpecificInfo,rrc_FrequencyInfo_modeSpecificInfo_fdd)))
          {
            directed_cell_info.freq_info_present = TRUE;
            directed_cell_info.uarfcn_dl = 
              tcreconfig_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
          }
         
          if (rrctcrc_is_pcpich_info_present(msg_ptr, MSG_REL5))
          {
            directed_cell_info.pscr_present = TRUE;
            directed_cell_info.pscr = tcreconfig_ptr->dl_InformationPerRL_List.elem[0].
                                      modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
          }      
          /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
                 FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
          if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
            (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
          {
            WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
            if(tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
            {
              rrctcrc_send_initiate_cu_flag = TRUE;
              rrctcrc_delete_crnti          = TRUE;
            }
            else
            {
              tcrc.directed_cell_info = TRUE;
            }
          }  
        }
        if (
            (current_state==tcrc.state_indicated_by_msg)||
            ((current_state==RRC_STATE_CELL_FACH)&& 
             (tcrc.state_indicated_by_msg==RRC_STATE_CELL_DCH))
           )
        {
          /*  CellDCH<->CellDCH or CellFACH<->CellFACH or CellFACH->CellDCH
           transition.  Ordered Config has been set. We can setup the
           Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */

           /*check whether any frequency info is present in the message
           and transition is CELL_FACH->CELL_FACH. In this case
           initiate a cell update after channel config is successfully completed */
           if ((current_state==RRC_STATE_CELL_FACH)&& 
              (tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
           {
              /*call the function that evaluates the directed cell info and set the flags accordingly */
             if ((rrctcrc_send_initiate_cu_flag == FALSE) &&
                (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
             {
               if (RRC_MSG_COMMON_BITMASK_IE_PTR(tcreconfig_ptr, 
                  rrc_TransportChannelReconfiguration_r5_IEs,new_C_RNTI))
               {
                 tcrc.new_crnti_valid = TRUE;
                 rrc_translate_crnti(&tcreconfig_ptr->new_C_RNTI, 
                                       &c_rnti);
                 tcrc.new_crnti = (uint16)c_rnti;
               }
               else
               {
                 tcrc.new_crnti_valid = FALSE;
               }
               /* Now copy OC to TOC */
               rrcllcoc_update_toc_with_oc_from_fach();
               /*Initiate the cell selection*/
               rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
                                                        RRC_TRANSITION_FROM_FACH,
                                                        tcrc.state_indicated_by_msg);
     

               tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
          
               /* exit from the switch case */
               break;
             }
    
           } /* if CELL_FACH->CELL_FACH reconfiguration */

           /* Call the function that creates and sends a channel
           config request to LLC. */
           rrctcrc_send_channel_config_req( );

           /* Change the substate since we're waiting for the
           confirmation from LLC for the channel configs */
           tcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                  (tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
        {
          if (RRC_MSG_COMMON_BITMASK_IE_PTR(tcreconfig_ptr, 
             rrc_TransportChannelReconfiguration_r5_IEs,new_C_RNTI))
          {
             tcrc.new_crnti_valid = TRUE;
             rrc_translate_crnti(&tcreconfig_ptr->new_C_RNTI, 
                                  &c_rnti);
             tcrc.new_crnti = (uint16)c_rnti;
          }
          else
          {
            tcrc.new_crnti_valid = FALSE;
          }

          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_TCR,
                                                  RRC_TRANSITION_FROM_DCH,
                                                  RRC_STATE_CELL_FACH);

          tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

          /* Start T_305 [periodic cell update] timer */
          rrccu_start_t_305_timer();
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                 (tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH))
        {
          /*store the directed cell info */
           tcrc.directed_cell_info = rrctcrc_is_pcpich_info_present(msg_ptr, MSG_REL5);

           /*Send the TCRC complete msg on old configuration */
           send_tcrc_complete_message(TRUE);
           /*wait for L2ack */
           tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
        
           /* prints rate for each transport channel */
           rrcllc_print_trans_chnl_rate();

        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (tcrc.state_indicated_by_msg==RRC_STATE_URA_PCH))
        {
          /*Send the TCRC complete msg on old configuration */
          send_tcrc_complete_message(TRUE);
          /*wait for L2ack */
          tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
        
          /* prints rate for each transport channel */
          rrcllc_print_trans_chnl_rate();
          
        }
        else if((current_state==RRC_STATE_CELL_FACH) && 
                ((tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
                 (tcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          /*call the function that evaluates the directed cell info and set the flags accordingly */
          if ((tcrc.directed_cell_info == FALSE) &&
            (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
          {
            if(tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
            {
              tcrc.directed_cell_info = rrctcrc_is_pcpich_info_present(msg_ptr, MSG_REL5);
            }

            tcrc.initiate_cell_selection_trans_from_fach = TRUE;
          }
    

          /* check whether CPICH info is present or not. Store this
          info  */
         // tcrc.directed_cell_info = rrctcrc_is_pcpich_info_present(msg_ptr);
          /*Send the RRCTCTC complete msg on old configuration */
          /* check whether new C-RNTI was present in the message. In that case
          we have to configure MAC with new C-RNTI before sending the response 
          message */
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(tcreconfig_ptr,
             rrc_TransportChannelReconfiguration_r5_IEs,new_C_RNTI))
          {
            rrc_translate_crnti(&tcreconfig_ptr->new_C_RNTI, 
                                &c_rnti);
            /* Send a RNTI Update Request */
            rnti_update_info.crnti_action = RRC_RNTI_UPDATE;
            rnti_update_info.crnti        = (uint16)c_rnti;
            /* check whether URNTI is also present in the message */
            if (RRC_MSG_COMMON_BITMASK_IE_PTR(tcreconfig_ptr,
             rrc_TransportChannelReconfiguration_r5_IEs,new_U_RNTI))
            {
              rrc_translate_urnti(&tcreconfig_ptr->new_U_RNTI, 
                                  &u_rnti);
              rnti_update_info.urnti_action = RRC_RNTI_UPDATE; 
              rnti_update_info.urnti        = u_rnti; 
            }
            else
            {
              rnti_update_info.urnti_action = RRC_RNTI_NOCHANGE; 
            }
            rnti_update_info.procedure            = RRC_PROCEDURE_TCR;
            rnti_update_info.rlc_re_establish_srb = FALSE;
            rnti_update_info.rlc_re_establish_rab = FALSE;
            rnti_update_info.cnf_reqd             = TRUE;
            rrcrb_send_rnti_update_req(rnti_update_info);
            /* wait for the RNTI update confirm before sending the response
            message */
            tcrc.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
          }
          else 
          {
            send_tcrc_complete_message(TRUE);
            /*wait for L2ack */
            tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
            /* prints rate for each transport channel */
            rrcllc_print_trans_chnl_rate();
			
          }
        }
        break;

      case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:
        tcrc.status.failure_status = RRCTC_SIMULTANEOUS_RECONFIG;
        /* Send the TCRC Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause );

        /* Go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:
        tcrc.status.failure_status =RRCTC_MSG_UNSUPPORTED_CONFIG;
        /* Send the TCRC Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause );

        /* Go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
		
      case ORDERED_CONFIG_CONFIGURATION_INVALID:
        tcrc.status.failure_status =RRCTC_MSG_INVALID_CONFIG;
        /* Send the TCRC Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause );

        /* Go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_NOT_SET_OTHER:
        WRRC_MSG0_HIGH("LLC returned other failure!");
        /* When LLC sends this, there is a serious problem
        and we'll go back to disconnected state. No need
        to send the failure message here, we can just
        go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME          
      case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("TCR processing deferred due to LTA!");
      
        tcrc.curr_substate = RRCRB_INITIAL;
      
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
      default:

        WRRC_MSG0_ERROR("Unknown return value from LLC");
        /* Unknown error, just go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
    }/* end switch */
    
    if( ordered_config_err_occured == TRUE )
    {
      /* No need to continue. Clear the procedure
      variables and return. */
      rrctcrc_clear_procedure();
      return;
    }

    if(tcrc.tcrc_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }

  } 
}/* end function process_tcreconfig_r5 */



/*===========================================================================

FUNCTION   PROCESS_TCRECONFIG_R6

DESCRIPTION

  This function processes the Rel 6 TCRC Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a TCRC Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to configure the transport channels given by the message.
    
    If ORDERED_CONFIG could not be set, a TCRC Failure message
    will be sent to UTRAN.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_tcreconfig_r6
(
  rrc_RRC_TransactionIdentifier transaction_id,

  rrc_DL_DCCH_Message *dcch_msg_ptr  ,
  /* Pointer to the RB Reconfig message        */
  rrc_TransportChannelReconfiguration_r6_IEs *tcreconfig_ptr
)
{

  rrc_TransportChannelReconfiguration *msg_ptr; /* Pointer to the TCRC message        */
  rrcllc_oc_status_e_type             oc_status;/* To hold ordered_config status          */

  rrc_state_e_type                    current_state; /* current RRC State  */
  rrc_int_c_rnti_type                 c_rnti;       /* Local C_RNTI value */
  rrc_int_u_rnti_type                 u_rnti;
  rrcrb_rnti_update_info_type         rnti_update_info;   /* to hold rnti update info */
  rrcrb_directed_cell_info_type       directed_cell_info; /* directed cell info struct */

  /* Used to indicate if an error occured while setting ordered_config. */
  boolean ordered_config_err_occured = FALSE; 
 
  /* Get the pointer to the actual TCRC message */
  msg_ptr = &(dcch_msg_ptr->message.u.transportChannelReconfiguration);
 

  if (FAILURE == 
      rrctcrc_check_and_send_failure_for_simul_reconfig(
        msg_ptr->u.later_than_r3.rrc_TransactionIdentifier))
  {
    return;
  }

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  validate_tcreconfig_r6( tcreconfig_ptr );



  if( tcrc.status.failure_status != RRCTC_MSG_VALID )
  {
    /* TCRC Message was invalid. */
    WRRC_MSG0_ERROR("TCRC Message validation failed");

    /* Send the TCRC Failure Message */
    send_tcrc_failure_message( tcrc.status.failure_status,
                              tcrc.status.prot_err_cause
                            );

    /* Make sure we're back in the initial sub-state */
    tcrc.curr_substate = RRCRB_INITIAL;
  }
  /* If the message is valid, proceed further */
  else
  {

    MSG_LOW("Processing TCRC Message", 0, 0, 0);

    /* Check if there is CN information present. If so, we need
    to send the information to the Cell Change manager */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(tcreconfig_ptr,
        rrc_TransportChannelReconfiguration_r6_IEs,cn_InformationInfo))
    {
      WRRC_MSG0_HIGH("Saving CN info for NAS");
      tcrc.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(
        &(tcreconfig_ptr->cn_InformationInfo) );
      if (tcrc.mm_cmd_ptr != NULL)
      {
        /* set cn_info_saved flag */
        tcrc.cn_info_saved = TRUE;
      }

    }

     
    /* We have to reconfigure Transport Channel. First set ORDERED_CONFIG
       so that LLC can process the necessary parameters. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_TCR,
                                           tcrc.state_indicated_by_msg,
                                           rrc_DL_DCCH_Message_PDU,
                                           (void *)dcch_msg_ptr 
                                         );

    switch( oc_status )
    {

      case ORDERED_CONFIG_SET:
        if((rrc_sib_is_current_event_sib_read_in_dch()) &&
           (RRC_PROCEDURE_GPS == rrc_sib_get_proc_id_for_sib_read_in_dch()))
        {
          WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
          rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
        }
        current_state = rrc_get_state(); 
        /* Populate the local variables to indicate if Re-direction info is present */
        directed_cell_info.freq_info_present = FALSE;
        directed_cell_info.pscr_present = FALSE;   
        if((current_state==RRC_STATE_CELL_FACH) && 
               ((tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
               ||(tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
               ||(tcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          if((RRC_MSG_COMMON_BITMASK_IE_PTR(tcreconfig_ptr, 
                rrc_TransportChannelReconfiguration_r6_IEs,frequencyInfo)) &&
               (RRC_CHECK_COMMON_MSG_TYPE(tcreconfig_ptr->frequencyInfo.modeSpecificInfo,
                rrc_FrequencyInfo_modeSpecificInfo_fdd)))
          {
            directed_cell_info.freq_info_present = TRUE;
            directed_cell_info.uarfcn_dl = 
              tcreconfig_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
          }
         
          if (rrctcrc_is_pcpich_info_present(msg_ptr, MSG_REL6))
          {
            directed_cell_info.pscr_present = TRUE;
            directed_cell_info.pscr = tcreconfig_ptr->dl_InformationPerRL_List.elem[0].
                                      modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
          }  
          /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
                 FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
          if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
            (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
          {
            WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
            if(tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
            {
              rrctcrc_send_initiate_cu_flag = TRUE;
              rrctcrc_delete_crnti          = TRUE;
            }
            else
            {
              tcrc.directed_cell_info = TRUE;
            }
          }  
        }
        if (
            (current_state==tcrc.state_indicated_by_msg)||
            ((current_state==RRC_STATE_CELL_FACH)&& 
             (tcrc.state_indicated_by_msg==RRC_STATE_CELL_DCH))
           )
        {
          /*  CellDCH<->CellDCH or CellFACH<->CellFACH or CellFACH->CellDCH
           transition.  Ordered Config has been set. We can setup the
           Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */

           /*check whether any frequency info is present in the message
           and transition is CELL_FACH->CELL_FACH. In this case
           initiate a cell update after channel config is successfully completed */
           if ((current_state==RRC_STATE_CELL_FACH)&& 
              (tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
           {
     
       
             /*call the function that evaluates the directed cell info and set the flags accordingly */
             if ((rrctcrc_send_initiate_cu_flag == FALSE) &&
                (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
             {
               if (RRC_MSG_COMMON_BITMASK_IE_PTR(tcreconfig_ptr, 
                  rrc_TransportChannelReconfiguration_r6_IEs,new_C_RNTI))
               {
                 tcrc.new_crnti_valid = TRUE;
                 rrc_translate_crnti(&tcreconfig_ptr->new_C_RNTI, 
                                       &c_rnti);
                 tcrc.new_crnti = (uint16)c_rnti;
               }
               else
               {
                 tcrc.new_crnti_valid = FALSE;
               }
     
               /* Now copy OC to TOC */
               rrcllcoc_update_toc_with_oc_from_fach();
               /*Initiate the cell selection*/
               rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
                                                        RRC_TRANSITION_FROM_FACH,
                                                        tcrc.state_indicated_by_msg);
                 
               tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
          
               /* exit from the switch case */
               break;
             }
    
           } /* if CELL_FACH->CELL_FACH reconfiguration */

           /* Call the function that creates and sends a channel
           config request to LLC. */
           rrctcrc_send_channel_config_req( );

           /* Change the substate since we're waiting for the
           confirmation from LLC for the channel configs */
           tcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                  (tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
        {
          if (RRC_MSG_COMMON_BITMASK_IE_PTR(tcreconfig_ptr, 
             rrc_TransportChannelReconfiguration_r6_IEs,new_C_RNTI))
          {
             tcrc.new_crnti_valid = TRUE;
             rrc_translate_crnti(&tcreconfig_ptr->new_C_RNTI, 
                                  &c_rnti);
             tcrc.new_crnti = (uint16)c_rnti;
          }
          else
          {
            tcrc.new_crnti_valid = FALSE;
          }

          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_TCR,
                                                  RRC_TRANSITION_FROM_DCH,
                                                  tcrc.state_indicated_by_msg);

          tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

          /* Start T_305 [periodic cell update] timer */
          rrccu_start_t_305_timer();
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                 (tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH))
        {
          /*store the directed cell info */
           tcrc.directed_cell_info = rrctcrc_is_pcpich_info_present(msg_ptr, MSG_REL6);

           /*Send the TCRC complete msg on old configuration */
           send_tcrc_complete_message(TRUE);
           /*wait for L2ack */
           tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
        
           /* prints rate for each transport channel */
           rrcllc_print_trans_chnl_rate();

        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (tcrc.state_indicated_by_msg==RRC_STATE_URA_PCH))
        {
          /*Send the TCRC complete msg on old configuration */
          send_tcrc_complete_message(TRUE);
          /*wait for L2ack */
          tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
        
          /* prints rate for each transport channel */
          rrcllc_print_trans_chnl_rate();
          
        }
        else if((current_state==RRC_STATE_CELL_FACH) && 
                ((tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
                 (tcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          /*call the function that evaluates the directed cell info and set the flags accordingly */
          if ((tcrc.directed_cell_info == FALSE) &&
            (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
          {  
            if(tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
            {
              tcrc.directed_cell_info = rrctcrc_is_pcpich_info_present(msg_ptr, MSG_REL6);
            }

            tcrc.initiate_cell_selection_trans_from_fach = TRUE;
          }
    

          /* check whether CPICH info is present or not. Store this
          info  */
         // tcrc.directed_cell_info = rrctcrc_is_pcpich_info_present(msg_ptr);
          /*Send the RRCTCTC complete msg on old configuration */
          /* check whether new C-RNTI was present in the message. In that case
          we have to configure MAC with new C-RNTI before sending the response 
          message */
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(tcreconfig_ptr,
             rrc_TransportChannelReconfiguration_r6_IEs,new_C_RNTI))
          {
            rrc_translate_crnti(&tcreconfig_ptr->new_C_RNTI, 
                                &c_rnti);
            /* Send a RNTI Update Request */
            rnti_update_info.crnti_action = RRC_RNTI_UPDATE;
            rnti_update_info.crnti        = (uint16)c_rnti;
            /* check whether URNTI is also present in the message */
            if (RRC_MSG_COMMON_BITMASK_IE_PTR(tcreconfig_ptr,
             rrc_TransportChannelReconfiguration_r6_IEs,new_U_RNTI))
            {
              rrc_translate_urnti(&tcreconfig_ptr->new_U_RNTI, 
                                  &u_rnti);
              rnti_update_info.urnti_action = RRC_RNTI_UPDATE; 
              rnti_update_info.urnti        = u_rnti; 
            }
            else
            {
              rnti_update_info.urnti_action = RRC_RNTI_NOCHANGE; 
            }
            rnti_update_info.procedure            = RRC_PROCEDURE_TCR;
            rnti_update_info.rlc_re_establish_srb = FALSE;
            rnti_update_info.rlc_re_establish_rab = FALSE;
            rnti_update_info.cnf_reqd             = TRUE;
            rrcrb_send_rnti_update_req(rnti_update_info);
            /* wait for the RNTI update confirm before sending the response
            message */
            tcrc.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
          }
          else 
          {
            send_tcrc_complete_message(TRUE);
            /*wait for L2ack */
            tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
            /* prints rate for each transport channel */
            rrcllc_print_trans_chnl_rate();

          }
        }
        break;

      case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:
        tcrc.status.failure_status = RRCTC_SIMULTANEOUS_RECONFIG;
        /* Send the TCRC Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause );

        /* Go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:
        tcrc.status.failure_status =RRCTC_MSG_UNSUPPORTED_CONFIG;
        /* Send the TCRC Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause );

        /* Go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_INVALID:
        tcrc.status.failure_status =RRCTC_MSG_INVALID_CONFIG;
        /* Send the TCRC Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause );

        /* Go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;


      case ORDERED_CONFIG_NOT_SET_OTHER:
        WRRC_MSG0_HIGH("LLC returned other failure!");
        /* When LLC sends this, there is a serious problem
        and we'll go back to disconnected state. No need
        to send the failure message here, we can just
        go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME          
      case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("TCR processing deferred due to LTA!");
      
        tcrc.curr_substate = RRCRB_INITIAL;
      
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
      default:

        WRRC_MSG0_ERROR("Unknown return value from LLC");
        /* Unknown error, just go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
    }/* end switch */
    
    if( ordered_config_err_occured == TRUE )
    {
      /* No need to continue. Clear the procedure
      variables and return. */
      rrctcrc_clear_procedure();
      return;
    }

    if(tcrc.tcrc_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }

  } 
  if (RRC_MSG_COMMON_BITMASK_IE_TYPE2(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6,
  	transportChannelReconfiguration_r6_add_ext))
  {
    WRRC_MSG0_ERROR("RRCEUL: transportChannelReconfiguration_r6_add_ext not yet supported");
  } 
  
  if (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
      criticalExtensions.u.r6.m.v6b0NonCriticalExtensionsPresent)
  {
  
    rrctc_process_high_mobility_ind_in_non_critical_ext(msg_ptr,MSG_REL6);
  
  }
}/* end function process_tcreconfig_r5 */

/*===========================================================================

FUNCTION   RRCRBRC_PROCESS_NEW_CELL_IND

DESCRIPTION

  This function processes the received new cell ind command.
  <more later>.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_process_new_cell_ind
(
  rrc_new_cell_ind_type *ptr /* Pointer to the State Change Ind cmd */
)
{
   /* If the UE has camped on directed cell then do not send a cell update,
     * else send a CELL update with cause "cell Re-selection 
     "*/
  if(rrccsp_is_selected_cell_utran_directed())
  {
    /* No need to initiate cell update as the C-RNTI is available
      * and UE is camped on the directed cell.
      */
    rrctcrc_send_initiate_cu_flag = FALSE;
  }
  else
  {
    rrctcrc_send_initiate_cu_flag = TRUE;
    rrctcrc_delete_crnti = TRUE;
  }
#ifdef FEATURE_WCDMA_HS_FACH
   /* If H-RNTI is not given in OTA then initiate cell update*/
  if(rrcenhstate_set_hrnti_status_for_transitions())
  {
    rrctcrc_send_initiate_cu_flag = TRUE;
    rrctcrc_delete_crnti          = TRUE;
  }
#endif
      
#ifdef FEATURE_WCDMA_HS_RACH
 /* Check if E-RNTI is not given in OTA then initiate cell update*/
        if(rrchsrach_check_ernti_status())
        {
          rrctcrc_send_initiate_cu_flag = TRUE;
          rrctcrc_delete_crnti          = TRUE;
        }
#endif


  rrctcrc_send_channel_config_req();
  
  /* Change the substate since we're waiting for the
     confirmation from LLC for the channel configs */
  tcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
     
}

/*===========================================================================

FUNCTION   rrctcrc_validate_and_check_integrity_for_srns_relocation

DESCRIPTION

  This function validates whether received tcrc message is due to SRNS relocation,
  if yes, then it performs the integrity check on the received message because
  integrity check was skipped for message which triggers SRNS relocation.
          
DEPENDENCIES

  None.

REUSABILITY: R99/R5/R6

RETURN VALUE

  SUCCESS or FAILURE

SIDE EFFECTS

  None.

===========================================================================*/
static uecomdef_status_e_type rrctcrc_validate_and_check_integrity_for_srns_relocation
(
  rrc_cmd_type *cmd_ptr,
  /* Pointer to the RB Reconfig message        */
  rrc_TransportChannelReconfiguration *msg_ptr,
  rrc_RRC_TransactionIdentifier transaction_id,  
  rrc_msg_ver_enum_type msg_version
)
{
  rrcsmc_srns_ip_check_status_e_type ip_check_status = 
                                                 RRCSMC_SRNS_INTEGRITY_CHECK_SUCCESS;

  /* Check whether this reconfig message has SRNS 
  relocation info. If this is true then inform SMC so
  that it uses the integrity protection info present in the
  message to do integrity check for this message */

  if (TRUE == rrctcrc_validate_srns_relocation_info(msg_ptr, msg_version))
  {
    /* Check Integrity for srns relocation */
    ip_check_status = rrcsmc_check_integrity_for_srns_relocation(cmd_ptr, msg_version);

    switch (ip_check_status)
    {
      case RRCSMC_SRNS_INTEGRITY_CHECK_SUCCESS:
        /*continue further processing with the message */
        break;

      case RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE:
        /*Discard the message and continue with ongoing procedure as if this
        message was not received */
        break;

      case RRCSMC_SRNS_SECURITY_INFO_INVALID:
      case RRCSMC_INCOMPATIBLE_SECURITY_RECONFIGURATION:
        if (transaction_id == tcrc.tr_ids.rejected_transaction)
        {
          WRRC_MSG1_HIGH("Duplicate TCRC msg - rejected tr-id%d match!",
                   tcrc.tr_ids.rejected_transaction);
          /* Nothing else to do, just ignore the message and return */
        } 
        else
        {
          /* send a reconfig failure messge with invalid
          configuration and 
          continue the same substate with any ongoing procedure */
          tcrc.tr_ids.rejected_transaction = transaction_id;

          if (ip_check_status == RRCSMC_SRNS_SECURITY_INFO_INVALID)
          {
            tcrc.status.failure_status = RRCTC_MSG_INVALID_CONFIG;
          }
          else
          {
            tcrc.status.failure_status = RRCTC_SIMULTANEOUS_RECONFIG;
          }
          /*Check whether Cell Update is going on */
          if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
               (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
          {
            WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
            (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBE);
            /* Wait until Cell update procedure is completed */
            tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF; 
          }
          else
          {
            send_tcrc_failure_message( tcrc.status.failure_status,
                                            tcrc.status.prot_err_cause );
          }
        }
        /*do not clear the procedure or change the rbe substate because
        some other rbe procedure might be in progress. Just return from
        here */
        break;

      default:
        /*should not come here */
        break;

    }

  }

  if (ip_check_status != RRCSMC_SRNS_INTEGRITY_CHECK_SUCCESS)
  {
    return FAILURE;
  }
  else
  {
    return SUCCESS;
  }
}

/*===========================================================================

FUNCTION   rrctcrc_send_tcrc_failure_unsupported_config

DESCRIPTION

  This function sends the tcreconfig-failure messsage with cause unsupported
  configuration because UE dont support the extension provided by the network.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_send_tcrc_failure_unsupported_config
(
  /* transaction id of RB Reconfig message */
  rrc_RRC_TransactionIdentifier transaction_id  
)
{
  /* The first thing to do is check the transaction id.
  If the transaction id is the same as the accepted
  or rejected transaction ids, we don't process
  this message. It's a duplicate and will just be
  dropped. But we also don't change the current
  sub-state since we're in the middle of
  processing a previous message. Note that
  the transaction id is included only if critical
  extensions are not present. */
  WRRC_MSG0_HIGH("Unsupported TCReconfig t");
  if (transaction_id == tcrc.tr_ids.rejected_transaction)
  {
    WRRC_MSG1_HIGH("Duplicate TCRC msg! rejected tr-id%d match!", tcrc.tr_ids.rejected_transaction);
    /* Nothing else to do, just ignore the message and return */
  } 
  else
  {
    /* We have received a tcrc message with a new transaction id.
    This message should be in our rejected transaction  
    list */
    tcrc.tr_ids.rejected_transaction = transaction_id;
    tcrc.status.failure_status = RRCTC_MSG_PROTOCOL_ERROR;
    tcrc.status.prot_err_cause = rrc_ProtocolErrorCause_messageExtensionNotComprehended;
    
    /*Check whether Cell Update is going on */
    if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
         (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
    {
      WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
      if (SUCCESS != rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_TCR))
      {
        WRRC_MSG0_ERROR("TCR failed to register with CU");
      }
      /* Wait until Cell update procedure is completed */
      tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF; 
    }
    else
    {
      send_tcrc_failure_message( tcrc.status.failure_status,
                                 tcrc.status.prot_err_cause );
      /*continue the same substate */ 
    }
  }
  return;
}

/*===========================================================================

FUNCTION   PROCESS_TCRC_MESSAGE

DESCRIPTION

  This function processes the received TCRC Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a TCRC Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to configure the transport channels given by the message.
    
    If ORDERED_CONFIG could not be set, a TCRC Failure message
    will be sent to UTRAN.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_tcrc_message
(
  rrc_cmd_type *cmd_ptr      /* Pointer to the TCRC message */
)
{

  rrc_TransportChannelReconfiguration *msg_ptr; /* Pointer to the TCRC message        */
  rrcllc_oc_status_e_type             oc_status;/* To hold ordered_config status          */

  rrc_state_e_type                    current_state; /* current RRC State  */
  rrc_int_c_rnti_type                 c_rnti;       /* Local C_RNTI value */
  rrc_int_u_rnti_type                 u_rnti;
  rrcrb_rnti_update_info_type         rnti_update_info;   /* to hold rnti update info */
  rrc_DL_DCCH_Message                 *dcch_msg_ptr; /* Point to the DCCH message     */
  rrcrb_directed_cell_info_type       directed_cell_info; /* directed cell info struct */
  boolean                             ordered_config_err_occured = FALSE; /* Used to indicate if an error
                                                                         occured while setting ordered_config. */

   /*Boolean used to indicate primary plmn i.d is present in OTA msg*/
  boolean primary_plmn_Id_present = FALSE;
   /*Variable to hold primary plmn i.d if present in OTA msg*/
  rrc_PLMN_Identity primary_plmn_Identity;

  /* A Downlink SDU has been received from RLC. This
  SDU should contain a Transport Channel Reconfiguration Message. Otherwise
  we would not have received this command. Copy the 
  received message pointer. */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
                     downlink_sdu_ind.decoded_msg;
  WRRC_MSG0_HIGH("Received TCRC Message");

  /* If ASN.1 decoding failed, the received pointer would be null.
  So we know we got a TCRC Message (by peeking into
  the encoded bit stream), but we were not able to decode it.
  Send a TCRC failure message with the appropriate
  cause. */
  if( dcch_msg_ptr == NULL )
  {
    WRRC_MSG0_HIGH("ASN decode failed for TCRC Msg");
    tcrc.status.failure_status = RRCTC_MSG_PROTOCOL_ERROR;
    tcrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
    send_tcrc_failure_message( tcrc.status.failure_status,
                              tcrc.status.prot_err_cause
                            );

    /* Then change to initial substate */
    tcrc.curr_substate = RRCRB_INITIAL;

    /* Then clear all local variables that may
    need to be cleared */
    rrctcrc_clear_procedure();

    /* Nothing else to do here */
    return;
  }


  /* Make sure it's the TCRC message
  although we would not have gotten here unless it's
  the right message */
  if( dcch_msg_ptr->message.t != T_rrc_DL_DCCH_MessageType_transportChannelReconfiguration )
  {
    WRRC_MSG1_ERROR("Incorrect message given to TCRC proc %d", dcch_msg_ptr->message.t);
    return;
  }

  /* Get the pointer to the actual TCRC message */
  msg_ptr = &(dcch_msg_ptr->message.u.transportChannelReconfiguration);
 
  /* This TCRC msg is Rel5 version. */
  if (msg_ptr->t != T_rrc_TransportChannelReconfiguration_r3)
  {
    if ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) &&
        (msg_ptr->u.later_than_r3.criticalExtensions.t == 
           T_rrc_TransportChannelReconfiguration_criticalExtensions_5_criticalExtensions))
    {
      if (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.t ==
           T_rrc_TransportChannelReconfiguration_criticalExtensions_4_r5)
      { 
        MSG_LOW("RRCHS:tcrcr5 msg",0,0,0);
  
        /* check for SRNS relocation scenario */
        if (FAILURE == rrctcrc_validate_and_check_integrity_for_srns_relocation(
            cmd_ptr, msg_ptr, msg_ptr->u.later_than_r3.rrc_TransactionIdentifier, 
            MSG_REL5))
        {
          /*do not clear the procedure or change the rbr substate because
            some other rbreconfig procedure might be in progress. Just return from
            here */
          return;
        }
    
        process_tcreconfig_r5(cmd_ptr);
      }
      /* if we are here then it means that t selected is: 
         rrc_RadioBearerReconfiguration_later_than_r3_criticalExtensions_
         criticalExtensions_criticalExtensions */
      else if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6) && 
              (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t ==
               T_rrc_TransportChannelReconfiguration_criticalExtensions_3_r6))
      {

        /* check for SRNS relocation scenario */
        if (FAILURE == rrctcrc_validate_and_check_integrity_for_srns_relocation(
            cmd_ptr, msg_ptr, msg_ptr->u.later_than_r3.rrc_TransactionIdentifier, 
            MSG_REL6))
        {
          /* do not clear the procedure or change the rbr substate because
          some other rbreconfig procedure might be in progress. Just return from
          here */
          return;
        }

        process_tcreconfig_r6(
          msg_ptr->u.later_than_r3.rrc_TransactionIdentifier,
          (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg,
          &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
          criticalExtensions.u.r6.transportChannelReconfiguration_r6
        );
  
      }
      /* if we are here then it means that t selected is: 
         rrc_RadioBearerReconfiguration_later_than_r3_criticalExtensions_
         criticalExtensions_criticalExtensions */
      else if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7) && 
              (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t ==
               T_rrc_TransportChannelReconfiguration_criticalExtensions_2_r7))
      {

        /* check for SRNS relocation scenario */
        if (FAILURE == rrctcrc_validate_and_check_integrity_for_srns_relocation(
            cmd_ptr, msg_ptr, msg_ptr->u.later_than_r3.rrc_TransactionIdentifier, 
            MSG_REL7))
        {
          /* do not clear the procedure or change the rbr substate because
          some other rbreconfig procedure might be in progress. Just return from
          here */
          return;
        }

        process_tcreconfig_r7(
          msg_ptr->u.later_than_r3.rrc_TransactionIdentifier,
          (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg,
          &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
          criticalExtensions.u.r7.transportChannelReconfiguration_r7
        );
  
      }
      /* if we are here then it means that t selected is: 
         rrc_RadioBearerReconfiguration_later_than_r3_criticalExtensions_
         criticalExtensions_criticalExtensions */
      else if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8) && 
          (RRCRB_R8_CRITICAL_EXT_PRESENT_PTR(msg_ptr, 
                  rrc_TransportChannelReconfiguration)))
      {

        /* check for SRNS relocation scenario */
        if (FAILURE == rrctcrc_validate_and_check_integrity_for_srns_relocation(
            cmd_ptr, msg_ptr, msg_ptr->u.later_than_r3.rrc_TransactionIdentifier, 
            MSG_REL8))
        {
          /* do not clear the procedure or change the rbr substate because
          some other rbreconfig procedure might be in progress. Just return from
          here */
          return;
        }

        process_tcreconfig_r8(
          msg_ptr->u.later_than_r3.rrc_TransactionIdentifier,
          (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg,
          &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
          criticalExtensions.u.criticalExtensions.u.r8.transportChannelReconfiguration_r8
        );
  
      }
#ifdef FEATURE_WCDMA_REL9
          /* if we are here then it means that t selected is: 
         rrc_RadioBearerReconfiguration_later_than_r3_criticalExtensions_
         criticalExtensions_criticalExtensions */
      else if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9) && 
          (RRCRB_R9_CRITICAL_EXT_PRESENT_PTR(msg_ptr, 
                  rrc_TransportChannelReconfiguration)))
      {

        /* check for SRNS relocation scenario */
        if (FAILURE == rrctcrc_validate_and_check_integrity_for_srns_relocation(
            cmd_ptr, msg_ptr, msg_ptr->u.later_than_r3.rrc_TransactionIdentifier, 
            MSG_REL9))
        {
          /* do not clear the procedure or change the rbr substate because
          some other rbreconfig procedure might be in progress. Just return from
          here */
          return;
        }

        process_tcreconfig_r9(
          msg_ptr->u.later_than_r3.rrc_TransactionIdentifier,
          (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg,
          &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
          criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.transportChannelReconfiguration_r9
        );
  
      }
#endif /* FEATURE_WCDMA_REL9 */      
#ifdef FEATURE_WCDMA_REL10
      #error code not present
#endif /* FEATURE_WCDMA_REL10 */      

      else
      {
        rrc_print_supported_asn1_rel_version();
  
        WRRC_MSG0_ERROR("Configuration not supported.  Send TC reconfig failure");
    
        rrctcrc_send_tcrc_failure_unsupported_config(
            msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);      
      }

      /* return - because all r5 and r6 processing are done inside the
         function */
      return;
    }
    else
    {
      rrc_print_supported_asn1_rel_version();
  
      WRRC_MSG0_ERROR("Configuration not supported.  Send TC reconfig failure");
  
      rrctcrc_send_tcrc_failure_unsupported_config(
        msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
  
      /*do not clear the procedure or change the rbr substate because
       some other rbreconfig procedure might be in progress. Just return from
       here */
      return;
    }
  }

  MSG_LOW("RRCHS:tcrcr99 rcvd",0,0,0);

  /* check for SRNS relocation scenario */
  if (FAILURE == rrctcrc_validate_and_check_integrity_for_srns_relocation(
      cmd_ptr, 
      msg_ptr, 
      msg_ptr->u.r3.transportChannelReconfiguration_r3.rrc_TransactionIdentifier,
      MSG_REL99))
  {
    /*do not clear the procedure or change the rbr substate because
    some other rbreconfig procedure might be in progress. Just return from
    here */
    return;
  }

  if (FAILURE == 
      rrctcrc_check_and_send_failure_for_simul_reconfig(
      msg_ptr->u.r3.transportChannelReconfiguration_r3.rrc_TransactionIdentifier))
  {
    return;
  }

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  validate_tcrc_message( msg_ptr );

  if( tcrc.status.failure_status != RRCTC_MSG_VALID )
  {
    /* TCRC Message was invalid. */
    MSG_LOW("TCRC Message validation failed", 0, 0, 0);

    /* Send the TCRC Failure Message */
    send_tcrc_failure_message( tcrc.status.failure_status,
                              tcrc.status.prot_err_cause
                            );

    /* Make sure we're back in the initial sub-state */
    tcrc.curr_substate = RRCRB_INITIAL;
  }
  /* If the message is valid, proceed further */
  else
  {

    MSG_LOW("Processing TCRC Message", 0, 0, 0);

    /* Check if there is CN information present. If so, we need
    to send the information to the Cell Change manager */
    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.transportChannelReconfiguration_r3,
		rrc_TransportChannelReconfiguration_r3_IEs,cn_InformationInfo)
      )
    {
      WRRC_MSG0_HIGH("Saving CN info for NAS");
     
    /*Check to see if primary PLMN I.D is present in TCR message ext
     * rrc_TransportChannelReconfiguration-v690ext which is present in 
     * v690NonCriticalExtensions
     */
    primary_plmn_Id_present = rrctcrc_check_and_get_primary_plmn_id(msg_ptr,
                                                                  &primary_plmn_Identity,MSG_REL99);
     
      tcrc.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info(
                     &(msg_ptr->u.r3.transportChannelReconfiguration_r3.cn_InformationInfo)
                     ,primary_plmn_Id_present
                     ,&primary_plmn_Identity
          );
      if (tcrc.mm_cmd_ptr != NULL)
      {
        /* set cn_info_saved flag */
        tcrc.cn_info_saved = TRUE;
      }

    }

     
    rrctc_process_high_mobility_ind_in_non_critical_ext(msg_ptr,MSG_REL99);

     
    /* We have to reconfigure Transport Channel. First set ORDERED_CONFIG
       so that LLC can process the necessary parameters. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_TCR,
                                           tcrc.state_indicated_by_msg,
                                           rrc_DL_DCCH_Message_PDU,
                                           (void *)dcch_msg_ptr 
                                         );

    switch( oc_status )
    {

      case ORDERED_CONFIG_SET:
        if((rrc_sib_is_current_event_sib_read_in_dch()) &&
           (RRC_PROCEDURE_GPS == rrc_sib_get_proc_id_for_sib_read_in_dch()))
        {
          WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
          rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
        }
        current_state = rrc_get_state(); 
        /* Populate the local variables to indicate if Re-direction info is present */
        directed_cell_info.freq_info_present = FALSE;
        directed_cell_info.pscr_present = FALSE;   
        if((current_state==RRC_STATE_CELL_FACH) && 
               ((tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
               ||(tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
               ||(tcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          if((RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.transportChannelReconfiguration_r3,
              rrc_TransportChannelReconfiguration_r3_IEs,frequencyInfo)) &&
             (RRC_CHECK_COMMON_MSG_TYPE(msg_ptr->u.r3.transportChannelReconfiguration_r3.frequencyInfo.
              modeSpecificInfo,rrc_FrequencyInfo_modeSpecificInfo_fdd)))
          {
            directed_cell_info.freq_info_present = TRUE;
            directed_cell_info.uarfcn_dl         = msg_ptr->u.r3.transportChannelReconfiguration_r3.
                                                   frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
          }
         
          if (rrctcrc_is_pcpich_info_present(msg_ptr, MSG_REL99))
          {
            directed_cell_info.pscr_present = TRUE;
            directed_cell_info.pscr = msg_ptr->u.r3.transportChannelReconfiguration_r3.dl_InformationPerRL_List.elem[0].
                                      modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
          }    
          /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
                 FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
          if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
            (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
          {
            WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
            if(tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
            {
              rrctcrc_send_initiate_cu_flag = TRUE;
              rrctcrc_delete_crnti          = TRUE;
            }
            else
            {
              tcrc.directed_cell_info = TRUE;
            }
          }  
        }

       
    if (
            (current_state==tcrc.state_indicated_by_msg)||
            ((current_state==RRC_STATE_CELL_FACH)&& 
             (tcrc.state_indicated_by_msg==RRC_STATE_CELL_DCH))
           )
        {
          /*  CellDCH<->CellDCH or CellFACH<->CellFACH or CellFACH->CellDCH
           transition.  Ordered Config has been set. We can setup the
           Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */
           MSG_LOW("Sending RRC_CHANNEL_CONFIG_REQ to LLC", 0, 0, 0);

           /*check whether any frequency info is present in the message
           and transition is CELL_FACH->CELL_FACH. In this case
           initiate a cell update after channel config is successfully completed */
           if ((current_state==RRC_STATE_CELL_FACH)&& 
              (tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
           {

            
             /*call the function that evaluates the directed cell info and set the flags accordingly */
             if ((rrctcrc_send_initiate_cu_flag == FALSE) &&
                (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
             {
               if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.transportChannelReconfiguration_r3, 
                  rrc_TransportChannelReconfiguration_r3_IEs,new_C_RNTI))
               {
                 tcrc.new_crnti_valid = TRUE;
                 rrc_translate_crnti(&msg_ptr->u.r3.transportChannelReconfiguration_r3.new_C_RNTI, 
                                      &c_rnti);
                 tcrc.new_crnti = (uint16)c_rnti;
               }
               else
               {
                 tcrc.new_crnti_valid = FALSE;
               }
               /* Now copy OC to TOC */
               rrcllcoc_update_toc_with_oc_from_fach();
                 
               /*Initiate the cell selection*/
               rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
                                                        RRC_TRANSITION_FROM_FACH,
                                                        tcrc.state_indicated_by_msg);
       
                 
               tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
          
                        /* exit from the switch case */
               break;
             }
    
           } /* if CELL_FACH->CELL_FACH reconfiguration */     
           /* Call the function that creates and sends a channel
           config request to LLC. */
           rrctcrc_send_channel_config_req( );

           /* Change the substate since we're waiting for the
           confirmation from LLC for the channel configs */
           tcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                  (tcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
        {
          if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.transportChannelReconfiguration_r3,
             rrc_TransportChannelReconfiguration_r3_IEs,new_C_RNTI))
          {
             tcrc.new_crnti_valid = TRUE;
             rrc_translate_crnti(&msg_ptr->u.r3.transportChannelReconfiguration_r3.new_C_RNTI, 
                                  &c_rnti);
             tcrc.new_crnti = (uint16)c_rnti;
          }
          else
          {
            tcrc.new_crnti_valid = FALSE;
          }

          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_TCR,
                                                  RRC_TRANSITION_FROM_DCH,
                                                  tcrc.state_indicated_by_msg);

          tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

          /* Start T_305 [periodic cell update] timer */
          rrccu_start_t_305_timer();

        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                 (tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH))
        {
          /*store the directed cell info */
           tcrc.directed_cell_info = rrctcrc_is_pcpich_info_present(msg_ptr, MSG_REL99);

           /*Send the TCRC complete msg on old configuration */
           send_tcrc_complete_message(TRUE);
           /*wait for L2ack */
           tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
        
           /* prints rate for each transport channel */
           rrcllc_print_trans_chnl_rate();
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (tcrc.state_indicated_by_msg==RRC_STATE_URA_PCH))
        {
          /*Send the TCRC complete msg on old configuration */
          send_tcrc_complete_message(TRUE);
          /*wait for L2ack */
          tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
          /* prints rate for each transport channel */
          rrcllc_print_trans_chnl_rate();
          
        }
        else if((current_state==RRC_STATE_CELL_FACH) && 
                ((tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
                 (tcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          /*call the function that evaluates the directed cell info and set the flags accordingly */
          if ((tcrc.directed_cell_info == FALSE) &&
            (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
          {
            if(tcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
            {
              tcrc.directed_cell_info = rrctcrc_is_pcpich_info_present(msg_ptr, MSG_REL99);
            }
            tcrc.initiate_cell_selection_trans_from_fach = TRUE;
          }
    

          /* check whether CPICH info is present or not. Store this
          info  */
         // tcrc.directed_cell_info = rrctcrc_is_pcpich_info_present(msg_ptr);
          /*Send the RRCTCTC complete msg on old configuration */
          /* check whether new C-RNTI was present in the message. In that case
          we have to configure MAC with new C-RNTI before sending the response 
          message */
          if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.transportChannelReconfiguration_r3,
             rrc_TransportChannelReconfiguration_r3_IEs,new_C_RNTI))
          {
            rrc_translate_crnti(&msg_ptr->u.r3.transportChannelReconfiguration_r3.new_C_RNTI, 
                                &c_rnti);
            /* Send a RNTI Update Request */
            rnti_update_info.crnti_action = RRC_RNTI_UPDATE;
            rnti_update_info.crnti        = (uint16)c_rnti;
            /* check whether URNTI is also present in the message */
            if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.transportChannelReconfiguration_r3,
             rrc_TransportChannelReconfiguration_r3_IEs,new_U_RNTI))
            {
              rrc_translate_urnti(&msg_ptr->u.r3.transportChannelReconfiguration_r3.new_U_RNTI, 
                                  &u_rnti);
              rnti_update_info.urnti_action = RRC_RNTI_UPDATE; 
              rnti_update_info.urnti        = u_rnti; 
            }
            else
            {
              rnti_update_info.urnti_action = RRC_RNTI_NOCHANGE; 
            }
            rnti_update_info.procedure            = RRC_PROCEDURE_TCR;
            rnti_update_info.rlc_re_establish_srb = FALSE;
            rnti_update_info.rlc_re_establish_rab = FALSE;
            rnti_update_info.cnf_reqd             = TRUE;
            rrcrb_send_rnti_update_req(rnti_update_info);
            /* wait for the RNTI update confirm before sending the response
            message */
            tcrc.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
          }
          else 
          {
            send_tcrc_complete_message(TRUE);
            /*wait for L2ack */
            tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
            /* prints rate for each transport channel */
            rrcllc_print_trans_chnl_rate();

          }
        }
        break;

      case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:
        tcrc.status.failure_status = RRCTC_SIMULTANEOUS_RECONFIG;
        /* Send the TCRC Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause );

        /* Go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:
        tcrc.status.failure_status =RRCTC_MSG_UNSUPPORTED_CONFIG;
        /* Send the TCRC Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause );

        /* Go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_INVALID:
        tcrc.status.failure_status =RRCTC_MSG_INVALID_CONFIG;
        /* Send the TCRC Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause );

        /* Go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;


      case ORDERED_CONFIG_NOT_SET_OTHER:
        WRRC_MSG0_HIGH("LLC returned other failure!");
        /* When LLC sends this, there is a serious problem
        and we'll go back to disconnected state. No need
        to send the failure message here, we can just
        go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME          
      case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("TCR processing deferred due to LTA!");
      
        tcrc.curr_substate = RRCRB_INITIAL;
      
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
      default:

        WRRC_MSG0_ERROR("Unknown return value from LLC");
        /* Unknown error, just go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
    }/* end switch */
    
    if( ordered_config_err_occured == TRUE )
    {
      /* No need to continue. Clear the procedure
      variables and return. */
      rrctcrc_clear_procedure();
      return;
    }

    if(tcrc.tcrc_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }

  } 
}/* end function process_tcrc_message */

/*===========================================================================

FUNCTION   RRCTCRC_PROCESS_RNTI_UPDATE_CNF

DESCRIPTION

  This function processes RNTI_UPDATE_CNF
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_process_rnti_update_cnf
(
void
)
{
  rrc_state_e_type curr_state;

  curr_state = rrc_get_state();

  if ((curr_state == RRC_STATE_CELL_FACH) && 
      ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
       (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)))
  {
   /*send the response message now */
    send_tcrc_complete_message(TRUE);
    /*wait for L2ack */
    tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
	/* prints rate for each transport channel */
	rrcllc_print_trans_chnl_rate();

  }
  else
  {
    /* We should not get here*/
    WRRC_MSG1_HIGH("Unexpected RNTI Update cnf,TCRC state %d", 
                                   tcrc.curr_substate);
  }
}

/*===========================================================================

FUNCTION   RRCTCRC_PROCESS_CHANNEL_CONFIG_CNF

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
static void rrctcrc_process_channel_config_cnf
(
  rrc_channel_config_cnf_type *ptr  /* Pointer to the Channel config cnf cmd */
)
{
  rrc_state_e_type curr_state;
  rrcrb_rnti_update_info_type rnti_update_info;   /* to hold rnti update info */
  rrc_cmd_type *cmd; /* To send a LLC command if channel config failed */
  rrc_CellUpdateCause       cu_cause;  /*To store CU cause from CU procedure*/
  rrc_rlc_re_est_for_srns_rb_e_type rb_type; /*rb type tobe reestablished */

  rrcllcpcie_initialise_sync_a_post_veri_info();
  /* Check if the channel configuration was succesful. */
  if( ptr->chan_cfg_status != RRCLLC_CHAN_CFG_SUCCESS )
  {
#ifndef FEATURE_WCDMA_HS_FACH
    /* Channel config failure, so initialise the ordered RB-Mapping database*/
    rrcllc_init_ordered_hanging_rb_mapping_info();
#else
    rrcllc_copy_current_hanging_rb_mapping_to_ordered();
#endif


    /*Here we initialize rlc_size_change_in_progress DB so that if it was set by TCRC, 
    it is ready for next procedure*/
    rrc_initialize_rlc_size_change();

    WRRC_MSG0_ERROR("CHAN_CONFIG_CNF with Failure");
    tcrc.status.failure_status = RRCTC_PHY_CHAN_FAILURE;
    if (ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_CFG_CANCELLED)
    {
      /* cell update is already in progress as this is for pending config
       * cancellation
       */

      tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      return; 
    }
    /* validation failure happened.  Send RB failure msg with invalid config */
    if (ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_OTHER)
    {
      tcrc.status.failure_status = RRCTC_MSG_INVALID_CONFIG;
      if(rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG)
      {
        tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_TCR, RRC_UNRECOVERABLE_ERROR, FALSE);
        return; 
      }

      WRRC_MSG0_HIGH("Sending TCRC failure msg with invalid config");
      send_tcrc_failure_message( tcrc.status.failure_status,
                                tcrc.status.prot_err_cause
                                );

      /* Reset ORDERED_CONFIG - UE can now accept other ordered
      configurations */
      (void)rrcllc_reset_ordered_config(RRC_PROCEDURE_TCR,TRUE);
        /* Then clear all local variables that may
        need to be cleared */
        rrctcrc_clear_procedure();
        /* Make sure we're back in the initial sub-state */
        tcrc.curr_substate = RRCRB_INITIAL;
      /*During FACH->DCH transition, if T305 got expired, we need to start T305 with zero timeout.*/	  
      if(rrc_get_state() == RRC_STATE_CELL_FACH && tcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH)
      {
        if((rrctmr_get_remaining_time(RRCTMR_T_305_TIMER) == 0)
            && (rrc_convert_t305_to_ms(RRC_GET_T305()) != RRCTMR_INFINITY_VAL ))
        {
          WRRC_MSG0_HIGH("Chan_config_req failed for FACH->DCH. So Starting T305 with zero timeout.");        
          rrctmr_start_timer(RRCTMR_T_305_TIMER, 0);
        }
      }	  
      return;
    } /* if (ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_OTHER) */

    /* Check if PHY_CHAN_FAILURE happened in FACH->DCH Transition.  If so, initiate a cell update
    request, and tcrc substate should be RRCRB_WAIT_FOR_CELL_UPDATE_CNF */

    if (rrc_get_state() == RRC_STATE_CELL_FACH && tcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH)
    {
      WRRC_MSG0_HIGH("PHYCHANFAIL FACH->DCH.  Initiate CU");

      tcrc.status.failure_status = RRCTC_PHY_CHAN_FAILURE;
      /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
      rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_TCR, RRC_PHY_CHAN_FAILURE,TRUE);
      /* Wait for CELL UPDATE to get a valid C_RNTI */
      tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      rrctcrc_send_initiate_cu_flag = FALSE;
      rrctcrc_delete_crnti          = FALSE;
    }
    /* Check if PHY_CHAN_FAILURE happended within CELL_DCH.  For now, initatiate a cell update request
       with cause RL Failure.  RBRC substate should be RRCRB_WAIT_FOR_CELL_UPDATE_CNF*/
    else if (rrc_get_state() == RRC_STATE_CELL_DCH && tcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH)
    {
      tcrc.status.failure_status = RRCTC_PHY_CHAN_FAILURE;
      if(rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG)
      {
        tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_TCR, RRC_RADIO_LINK_FAILURE, FALSE);
        return; 
      }

      if (ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_PHY_CHL)
      {
        WRRC_MSG0_HIGH("TCRC failed, going back to old cfg success");
        
        send_tcrc_failure_message( tcrc.status.failure_status,
                                   tcrc.status.prot_err_cause
                                   );
        /* Go back to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;
        /* Then clear all local variables that may
        need to be cleared */
        rrctcrc_clear_procedure();
        /* Clear ORDERED_CONFIG - UE can now accept other ordered
        configurations */
        rrcllc_clear_ordered_config(); 
      }
      else if(ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_BACK_TO_OLD_CONFIG)
      {
        WRRC_MSG0_HIGH("TCRC failed. Going back to old config failed. Initiate CU");

        /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_TCR, RRC_RADIO_LINK_FAILURE,TRUE);
        /* Wait for CELL UPDATE to get a valid C_RNTI */
        tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        rrctcrc_send_initiate_cu_flag = FALSE;
        rrctcrc_delete_crnti          = FALSE;
      }

    }
    else
    {
      if ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
          (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
      {
        /* go to idle */
        cmd=rrc_get_int_cmd_buf();
        cmd->cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;
        /* Need to change state */
        cmd->cmd.chan_config_req.rrc_state_change_required = TRUE;
        /* Next state is disconnected state */
        cmd->cmd.chan_config_req.next_state = RRC_STATE_DISCONNECTED;
        /* We don't need a confirm for this command */
        cmd->cmd.chan_config_req.rrc_channel_config_cnf_required = FALSE;
        /* Procedure name */
        cmd->cmd.chan_config_req.procedure = RRC_PROCEDURE_TCR;
        /* No need to fill any other parameter in this command.
        When LLC sees the next state is Disconnected State, it
        will tear down all the Radio Bearers. */
        rrc_put_int_cmd( cmd );
      } 
      else
      {
        tcrc.status.failure_status = RRCTC_PHY_CHAN_FAILURE;
        send_tcrc_failure_message( tcrc.status.failure_status,
                                   tcrc.status.prot_err_cause
                                   );
      }
      /* Go back to initial substate */
      tcrc.curr_substate = RRCRB_INITIAL;
    }
    return;
  }

  if( ptr->chan_cfg_status != RRCLLC_CHAN_CFG_SUCCESS )
  {
    WRRC_MSG0_HIGH("Channel Config Failed!");
    tcrc.status.failure_status = RRCTC_PHY_CHAN_FAILURE;
    send_tcrc_failure_message( tcrc.status.failure_status,
                              tcrc.status.prot_err_cause
                            );
    /* Go back to initial substate */
    tcrc.curr_substate = RRCRB_INITIAL;
    return;
  }


#ifdef FEATURE_WCDMA_REL7_SPECRS
  rrc_save_rnc_cap_change_support();

  /*Since Change in UE capability is successful, COUEC needs to be notified for this change.*/
  if(rrc_change_of_ue_cap_response)
  {
    rrccouec_process_completion_cap_change_with_reconfig();
    rrc_change_of_ue_cap_response = FALSE;    
  }
#endif

  if((ordered_config.set_status == OC_SET_FOR_DCH_FACH_TRANS) || 
        (ordered_config.set_status == OC_SET_FOR_DCH_CELL_PCH_TRANS)||
        (ordered_config.set_status == OC_SET_FOR_DCH_URA_PCH_TRANS))
  {
    if(tcrc.high_mobility_ind)
    {
      rrcrb_send_upd_ota_parms_ind(RRC_PROCEDURE_TCR, CPHY_HIGH_MOBILITY_INCL);
    }
  }
  
  /*For FACH->DCH, if CHAN_CONFIG_REQ is succeeded, we need to stop T305*/
  if(rrc_get_state() == RRC_STATE_CELL_FACH && tcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH)
  {
    WRRC_MSG0_HIGH("Chan_config succeeded for FACH->DCH. So Stopping T305");
    rrctmr_stop_timer(RRCTMR_T_305_TIMER);
  }      

  /*Notify the Cell_Id to registered entities*/
  if(tcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH)
  {
    rrc_notify_cell_id_in_dch();
  }	
  /* Copy the Ordered RB Mapping into the current RB-Mapping database*/
  rrcllc_copy_ordered_hanging_rb_mapping_to_current();

#ifdef FEATURE_SCUDIF_ENHANCEMENT
  #error code not present
#endif
  {
#ifdef FEATURE_VOC_AMR_WB
    rrcrb_update_amr_cctrch_id_for_reconfig(rrc_active_codec);
#else/*FEATURE_VOC_AMR_WB*/
    rrcrb_update_amr_cctrch_id_for_reconfig();
#endif/*FEATURE_VOC_AMR_WB*/  
  }

    /* Check for CELL FACH before sending the rbs complete message */
  curr_state=rrc_get_state();
  if ((RRC_STATE_CELL_FACH == curr_state) && 
      ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_FACH) ||
       (tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
       (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)))
  {
    if (tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) 
    {
      /* this is the case of CELL_FACH->CELL_PCH state transition.
      In current implementation we do initiate cell selection. Instead
      we treat current cell as strongest cell */
      if ((tcrc.directed_cell_info)&& (!rrccsp_is_selected_cell_utran_directed()))
      {
        /* check whether current camped scr code is same as the one
        sent in the message. If not then initiate cell update procedure */
  
          /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
          rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_TCR,
                                              RRC_CELL_RESELECTION,
                                              FALSE);

        /* No need to send ul data registration to rlc. This will be taken care
        by cellupdate procedure */
        rrctcrc_process_successful_proc_completion(FALSE);
      }
      else
      {
        /* send drx info to L1. Since this is CELL_FACH->CELL_PCH state transition
        RB procedure is responsible for sending the DRX info to lower layer*/
        rrcrb_send_l1_drx_req (RRC_PROCEDURE_TCR);

        rrctcrc_process_successful_proc_completion (TRUE);
      }
      /* return from here */
      return;
    }
    else if (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
    {
      /* send drx info to L1 */
      rrcrb_send_l1_drx_req (RRC_PROCEDURE_TCR);
      /* check whether URA update is required */
      if ( rrccu_is_ura_update_reqd() == TRUE)
      {
        rrcrb_send_ura_update_initiate_req( RRC_PROCEDURE_TCR,
                                            RRC_URA_RESELECTION,
                                            FALSE);
      }
      /* no need to wait for ura update completion 
      declare that proc is successfully completed */
      rrctcrc_process_successful_proc_completion (TRUE);
      /* return from here */
      return;
    }
    if(rrctcrc_send_initiate_cu_flag
       || (rrccu_return_cu_substate() == RRCCU_WAIT_FOR_RB_PROC_COMPL_IND) 
       )
    {
      if (rrctcrc_delete_crnti)
      {
        /* Send a RNTI Update Request */
        rnti_update_info.crnti_action         = RRC_RNTI_DELETE;
        rnti_update_info.urnti_action         = RRC_RNTI_NOCHANGE;
        rnti_update_info.procedure            = RRC_PROCEDURE_TCR;
        rnti_update_info.rlc_re_establish_srb = FALSE;
        rnti_update_info.rlc_re_establish_rab = FALSE;
        rnti_update_info.cnf_reqd             = FALSE;

        rrcrb_send_rnti_update_req(rnti_update_info);
        tcrc.new_crnti_valid=FALSE;
      }
      /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
      rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_TCR,
                                          RRC_CELL_RESELECTION,
                                          TRUE);
      /* Wait for CELL UPDATE to get a valid C_RNTI */
      tcrc.status.failure_status = RRCTC_MSG_VALID;
      tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      rrctcrc_send_initiate_cu_flag = FALSE;
      rrctcrc_delete_crnti          = FALSE;
      return;
    }
    /*Following case will occur when RLC RESET happens on SRB2 during DCH->FACH Transition*/
    else if((rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG) &&
      (rrccu_get_cell_update_started_status_with_cause(&cu_cause) == RRCCU_STARTED) && 
      (cu_cause == rrc_CellUpdateCause_rlc_unrecoverableError) && (rrccu_get_rb_id_unrec_error() == 
      DCCH_AM_RADIO_BEARER_ID))
    {
      if(tcrc.new_crnti_valid)
      {
        /* Send a RNTI Update Request */
        rnti_update_info.crnti_action         = RRC_RNTI_UPDATE;
        rnti_update_info.urnti_action         = RRC_RNTI_NOCHANGE;
        rnti_update_info.crnti                = tcrc.new_crnti;
        rnti_update_info.procedure            = RRC_PROCEDURE_TCR;
        rnti_update_info.rlc_re_establish_srb = FALSE;
        rnti_update_info.rlc_re_establish_rab = FALSE;
        rnti_update_info.cnf_reqd             = FALSE;

        rrcrb_send_rnti_update_req(rnti_update_info);
        /* Send a TCRC Complete later */      
      }

      /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
      rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_TCR,
                                          RRC_UNRECOVERABLE_ERROR,
                                          TRUE);

      /* Wait for CELL UPDATE to get a valid C_RNTI */
      tcrc.status.failure_status = RRCTC_MSG_VALID;
      tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;

      return;                                          
    }    
    /* Coming here implies the "rrctcrc_send_initiate_cu_flag" is FALSE.
     * Check if Valid CRNTI is stored in TCRC (for DCH->FACH case).
     * If so, send a RNTI Update Req to LLC and NOT wait for
     * Cell Update Complete Confirm. Cell Update will NOT be 
     * initiated when there is a valid C-RNTI and rrctcrc_send_initiate_cu_flag
     * is FALSE.
     */
    else if(tcrc.new_crnti_valid)
    {
      /* Send a RNTI Update Request */
      rnti_update_info.crnti_action         = RRC_RNTI_UPDATE;
      rnti_update_info.urnti_action         = RRC_RNTI_NOCHANGE;
      rnti_update_info.crnti                = tcrc.new_crnti;
      rnti_update_info.procedure            = RRC_PROCEDURE_TCR;
      rnti_update_info.rlc_re_establish_srb = FALSE;
      rnti_update_info.rlc_re_establish_rab = FALSE;
      rnti_update_info.cnf_reqd             = FALSE;

      rrcrb_send_rnti_update_req(rnti_update_info);
    }

    /* Query for valid C-RNTI */
    else if (C_RNTI_NOT_VALID == rrcllc_get_current_crnti_status() )
    {
      /* Wait for CELL UPDATE to get a valid C_RNTI. Register for CU complete cnf. */
      WRRC_MSG0_HIGH("No Valid C_RNTI! Register for CU CNF");
      if (SUCCESS != rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_TCR))
      {
        WRRC_MSG0_ERROR("TCR failed to register with CU");
      }
      tcrc.status.failure_status = RRCTC_MSG_VALID;
      tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      return;
    }    
  }  /* if current state is CELL_FACH */
  else if (RRC_STATE_CELL_PCH == curr_state)
  {
    if(rrctcrc_send_initiate_cu_flag)
    {
      /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
      rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_TCR,
                                          RRC_CELL_RESELECTION,
                                          FALSE);
      rrctcrc_send_initiate_cu_flag = FALSE;
      /* No need to send ul data registration to rlc. This will be taken care
      by cellupdate procedure */
      rrctcrc_process_successful_proc_completion(FALSE);
#ifdef FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE
      #error code not present
#endif

      return;
    }
    else
    {
      /*Perform the final steps for successful completion of the
      procedure */
      rrctcrc_process_successful_proc_completion(TRUE);
#ifdef FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE
      #error code not present
#endif

      return;
    }
  }
  else if(RRC_STATE_URA_PCH == curr_state)
  {
    /* check whether current camped scr code is same as the one
    sent in the message. If not then initiate cell update procedure */
    if (!((rrccu_get_cell_update_started_status_with_cause(&cu_cause)== RRCCU_STARTED) &&
       (cu_cause == rrc_CellUpdateCause_re_enteredServiceArea) &&
      ( (rrcllc_get_toc_usage() ==TOC_FOR_OOS)||
      (rrcllc_get_toc_usage() ==TOC_FOR_OOS_WITHOUT_DCH_INFO)||
      (rrcllc_get_toc_usage() ==TOC_FOR_OOS_WITH_DCH_INFO))))
    {
      /* check whether URA update is required */
      if ( rrccu_is_ura_update_reqd() == TRUE)
      {
        rrcrb_send_ura_update_initiate_req( RRC_PROCEDURE_TCR,
                                            RRC_URA_RESELECTION,
                                            FALSE);
      }
    }
    /* No need to send ul data registration to rlc. This will be taken care
    by cellupdate procedure */
    rrctcrc_process_successful_proc_completion(TRUE);
#ifdef FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE
      #error code not present
#endif

    return;
  }

  if(ptr->inter_freq_hho_status == TRUE)
  {
    tcrc.hho_ciph_status = TRUE;
  }
  else
  {
    tcrc.hho_ciph_status = FALSE;  
  }

  /*check whether this reconfiguration involves SRNS relocation */
  if (tcrc.tcrc_for_srns_relocation == TRUE)
  {
    if (RRC_STATE_CELL_DCH == curr_state)
    {
  
      /* Re-establish SRB2 before sending the response message */
      rb_type  = RRC_RE_EST_RB_2;
      rrcrb_send_rlc_re_est_req_to_smc(RRC_PROCEDURE_TCR, rb_type, TRUE);
  
      tcrc.curr_substate = RRCRB_WAIT_FOR_REESTABLISH_SRB2;
    }
    else
    {
      WRRC_MSG1_ERROR("SRNS not supp. in state %d", curr_state);
    }
  }
  else /*normal reconfiguration without SRNS relocation */
  {   
    /* Send the Radio Bearer Reconfig Complete message */
    if ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
        (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
    {
      send_tcrc_complete_message(TRUE);
    }
    else
    {
      /* Need to wait for L2 ack for all states because RRC needs to trigger include reconfig status indicator
      if RLC acknowledgement is not yet received.
      As per 8.3.1.3
      1> if the IE "Cell update cause" is set to "radio link failure" and the UE has submitted a reconfiguration response
      message to RLC and the UE has not received the RLC acknowledgement for the response message:
      2> include the IE "Reconfiguration Status Indicator" and set it to TRUE.*/
        rrc_update_reconfig_waiting_for_l2ack_dch(RRC_PROCEDURE_TCR);
        send_tcrc_complete_message(TRUE);
      if((rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG) 
        && (rrc_get_state() == RRC_STATE_CELL_DCH))
      {  
        /* wait for some time before initiating cell update */
        WRRC_MSG1_HIGH("Delaying by %d ms", RRCRB_PND_CFG_DELAY);
        (void) rex_clr_sigs(rex_self(), RRCTCRC_TEMP_TIMER_SIG);
        (void) rex_set_timer(&rrctcrc_temp_timer, RRCRB_PND_CFG_DELAY); 

        rrctcrc_sig = rex_wait(RRCTCRC_TEMP_TIMER_SIG);
        MSG_MED("RRCTCRC signal->%d", rrctcrc_sig, 0, 0);
        (void) rex_clr_sigs(rex_self(), RRCTCRC_TEMP_TIMER_SIG);
        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_TCR, RRC_UNRECOVERABLE_ERROR, FALSE);
      }
      /*Following case handles the scenario when RF Failure or RLC unrecoverable error occured
        during DCH -> FACH or FACH -> FACH transiton.*/
      else if((rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG) &&
        (rrccu_get_cell_update_started_status_with_cause(&cu_cause) == RRCCU_STARTED))
      {
        /*This handles the case only if RLC unrecoverable error happened not on SRB2.*/
        if((cu_cause == rrc_CellUpdateCause_rlc_unrecoverableError) && (rrccu_get_rb_id_unrec_error() != DCCH_AM_RADIO_BEARER_ID))
        {
          /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
          rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_TCR,
                                              RRC_UNRECOVERABLE_ERROR,
                                              TRUE);
        }
        else if(cu_cause == rrc_CellUpdateCause_radiolinkFailure)
        {
          /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
          rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_TCR,
                                              RRC_RADIO_LINK_FAILURE,
                                              TRUE);
        }
      }      
    }
  
    if (rrcllc_get_ordered_config_status_wo_f3() != OC_SET_FOR_DCH_FACH_TRANS)
    {
      /* prints rate for each transport channel */
      rrcllc_print_trans_chnl_rate();
    }

    /* send the nas cn info if stored */
    if (tcrc.cn_info_saved != FALSE)
    {
      /* Put the command on MM queue */
      rrc_send_rrc_message_to_mm(tcrc.mm_cmd_ptr);
      /* clear the flag */
      tcrc.cn_info_saved = FALSE;
    }

    /*if next state is Cell_PCH or URA_PCH then we have to 
    treat the procedure as successful only after getting L2ack
    for the response message */
    if ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
        (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
    {
      /* Change the substate to the next substate - this is
      where we wait for L2 ack from RLC for sending the RB RC
      complete message. */
      tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
    }
    else
    {
      /* clear the ordered config here
      treat the procedure as successful. */
      rrcllc_clear_ordered_config();  

      /* Go to the next sub-state in the state machine. That's
      the initial substate since we've sent the RB RC complete
      message */
      tcrc.curr_substate = RRCRB_INITIAL;

      /* At this point we don't need to remember any of the
      Radio Bearer information since it's stored in ESTABLISHED_RABS.
      Clear our internal variables */
      rrctcrc_clear_procedure();
    }
  }

}

/*===========================================================================

FUNCTION   RRCTCRC_PROCESS_RLC_REESTABLISHMENT_CNF

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
static void rrctcrc_process_rlc_reestablishment_cnf
(
void
)
{
  rrcrb_rnti_update_info_type rnti_update_info; /* structure to be used to request RNTI update */

  /* send rnti update request if URNTI was present in the rebsetup message */
  if (tcrc.new_urnti_valid == TRUE)
  {
    /* Send the RNTI update request to rrcllc to update with new-URNTI */
    /* Send a RNTI Update Request */
    rnti_update_info.crnti_action         = RRC_RNTI_NOCHANGE;
    rnti_update_info.urnti_action         = RRC_RNTI_UPDATE;
    rnti_update_info.urnti                = tcrc.new_urnti;
    rnti_update_info.procedure            = RRC_PROCEDURE_TCR;
    rnti_update_info.rlc_re_establish_srb = FALSE;
    rnti_update_info.rlc_re_establish_rab = FALSE;
    rnti_update_info.cnf_reqd             = FALSE;

    rrcrb_send_rnti_update_req(rnti_update_info);
  }

  /* Send the reconfiguration complete message */

  send_tcrc_complete_message(TRUE);


  /* Change the substate to the next substate - this is
     where we wait for L2 ack from RLC for sending the RB Reconfig
     complete message. */
  tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
  /* prints rate for each transport channel */
  rrcllc_print_trans_chnl_rate();
  
}

/*===========================================================================

FUNCTION   RRCTCRC_PROCESS_STATE_CHANGE_IND

DESCRIPTION

  This function processes the received State Change indication command.
  <more later>.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_process_state_change_ind
(
  rrc_state_change_ind_type *ptr /* Pointer to the State Change Ind cmd */
)
{
  rrc_CellUpdateCause cu_cause;
  /* Check if we're going to disconnected state */
  if( ptr->new_state == RRC_STATE_DISCONNECTED )
  {
    /* Then change to initial substate */
    tcrc.curr_substate = RRCRB_INITIAL;

    /* Then clear all local variables that may
    need to be cleared */
    rrctcrc_clear_procedure();
  }

  else if(ptr->new_state == RRC_STATE_CELL_FACH )
  {
    if(ptr->previous_state == RRC_STATE_CELL_DCH)
    {
      if (((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
          (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)) &&
           (tcrc.curr_substate == RRCRB_WAIT_FOR_L2_ACK))
      {
        WRRC_MSG1_HIGH("state change ind tcrc_st:%d",tcrc.curr_substate); 
        /* Then change to initial substate */
        tcrc.curr_substate = RRCRB_INITIAL;

        /* Then clear all local variables that may
        need to be cleared */
        rrctcrc_clear_procedure();
      }
      else if (tcrc.status.failure_status == RRCTC_MSG_VALID)
      {
        /* if cell update procedure is active with cause OOS, wait 
         * for the CU complete notification to send chan_config_req to
         * LLC 
         */
        if ((rrccu_get_cell_update_started_status_with_cause(&cu_cause)== RRCCU_STARTED) &&
            (cu_cause == rrc_CellUpdateCause_re_enteredServiceArea) &&
            (rrcllc_get_toc_usage() ==TOC_FOR_OOS)) 
        {
          rrctcrc_delete_crnti = TRUE;
          WRRC_MSG0_HIGH("Complete tcrc msg first.  Indicate compl to CU once done");

        }
        /* Call the function that creates and sends the  channel
           config information to be requested from LLC. */
        rrctcrc_send_channel_config_req();

        /* Change the substate since we're waiting for the
           confirmation from LLC for the channel configs */
        tcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;

        if((rrccsp_is_selected_cell_utran_directed()) &&
           (tcrc.new_crnti_valid))
        {
          /* No need to initiate cell update as the C-RNTI is available
           * and UE is camped on the directed cell.
           */
          rrctcrc_send_initiate_cu_flag=FALSE;
        }
        else
        {
          rrctcrc_send_initiate_cu_flag=TRUE;
          rrctcrc_delete_crnti = TRUE;
        }
#ifdef FEATURE_WCDMA_HS_FACH
 /* If H-RNTI is not given in OTA then initiate cell update*/
        if(rrcenhstate_set_hrnti_status_for_transitions())
        {
          rrctcrc_send_initiate_cu_flag = TRUE;
          rrctcrc_delete_crnti          = TRUE;
        }
#endif
#ifdef FEATURE_WCDMA_HS_RACH
 /* Check if E-RNTI is not given in OTA then initiate cell update*/
        if(rrchsrach_check_ernti_status())
        {
          rrctcrc_send_initiate_cu_flag = TRUE;
          rrctcrc_delete_crnti          = TRUE;
        }
#endif


      }
      else 
      {
        WRRC_MSG0_HIGH("Ignoring STATE_CHANGE_IND");
      }
     } 
    else
    {
      /* Currently there is no action when we go to cell_fach.
      This is not expected for release 1 since UE won't support
      transition to cell_fach. */
      WRRC_MSG0_HIGH("Received State change to CELL_FACH, no action");

    }
  }
  else if (ptr->new_state == RRC_STATE_CELL_PCH || 
           ptr->new_state == RRC_STATE_URA_PCH)
  {
    if(ptr->previous_state == RRC_STATE_CELL_DCH)
    {
      if (ptr->new_state == RRC_STATE_CELL_PCH) 
      {
        if (tcrc.directed_cell_info)
        {
          if (rrccsp_is_selected_cell_utran_directed()) 
          {
            /* No need to initiate cell update UE is camped on 
            the directed cell. */
            WRRC_MSG0_HIGH("Directed cell match, No CU reqd");
            rrctcrc_send_initiate_cu_flag = FALSE;
          }
          else
          {
            rrctcrc_send_initiate_cu_flag = TRUE;
            rrctcrc_delete_crnti          = TRUE;
          }
        }
        else
        {
          /* we have to initiate cell update incase cpich info is not present
          in the message */
          rrctcrc_send_initiate_cu_flag = TRUE;
          rrctcrc_delete_crnti          = TRUE;
        }
      }
      else
      {
        /* URA_PCH case */
        rrctcrc_send_initiate_cu_flag = FALSE;
      }
      
      /* Call the function that creates and sends the  channel
         config information to be requested from LLC. */
      rrctcrc_send_channel_config_req();
      /* Change the substate since we're waiting for the
         confirmation from LLC for the channel configs */
      tcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
    }
    else if ((ptr->previous_state == RRC_STATE_CELL_FACH) && 
         (tcrc.initiate_cell_selection_trans_from_fach == TRUE ))
    {
      /* Call the function that creates and sends a channel
          config request to LLC. */
      tcrc.initiate_cell_selection_trans_from_fach = FALSE;
      if (ptr->new_state == RRC_STATE_CELL_PCH) 
      {
        if (tcrc.directed_cell_info)
        {
          if(rrccsp_is_selected_cell_utran_directed())
          {
            /* No need to initiate cell update as the C-RNTI is available
              * and UE is camped on the directed cell.
              */
            rrctcrc_send_initiate_cu_flag = FALSE;
          }
          else
          {
            rrctcrc_send_initiate_cu_flag = TRUE;
            rrctcrc_delete_crnti          = TRUE;
          }
        }
        else
        {
          /* we have to initiate cell update incase cpich info is not present
          in the message */
          rrctcrc_send_initiate_cu_flag = TRUE;
          rrctcrc_delete_crnti          = TRUE;
        }
      }
      else
      {
        /* URA_PCH case */
        rrctcrc_send_initiate_cu_flag = FALSE;
      }
       
      rrctcrc_send_channel_config_req();
      /* Change the substate since we're waiting for the
          confirmation from LLC for the channel configs */
      tcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
    }

    else
    {
      MSG_HIGH("Recvd state change %d to %d tcrc state %d, No action",
                ptr->previous_state, ptr->new_state, tcrc.curr_substate);
    }

    /* IF CU is waiting for RB proc to complete, no need to initate CU through RB procedure. Reset flag. */
    if (rrccu_return_cu_substate() == RRCCU_WAIT_FOR_RB_PROC_COMPL_IND) 
    {
      rrctcrc_send_initiate_cu_flag = FALSE;
    }


  }
  else
  {
    /* We should not get here since we haven't registered for
    other state changes */
    WRRC_MSG2_HIGH("Unsupported state change %d to %d", ptr->previous_state,
             ptr->new_state);
  }
}





/*===========================================================================

FUNCTION   RRCTCRC_PROCESS_L2_ACK_IND

DESCRIPTION

  This function processes the received Layer 2 ACK indication.
  If the L2 ACK indicates that the TCRC Complete message has
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
static void rrctcrc_process_l2_ack_ind
(
  rrc_downlink_l2ack_ind_type *ptr /* Pointer to the L2 Ack Ind cmd */
)
{
  rrc_state_e_type                  current_state;       /* current RRC State  */
  rrc_rlc_re_est_for_srns_rb_e_type rb_type; /*rb type tobe reestablished */
  rrc_CellUpdateCause       cu_cause;
  uint8                    rejected_tr_id = RRCTC_INVALID_TRANSACTION_ID;
  WRRC_MSG1_HIGH("TCRC:L2 ack %d (0:Success 1:Failure)for RBS Complete msg received", ptr->status);

  if( ptr->status == SUCCESS )
  {
    current_state = rrc_get_state(); 

    /* Back up accepted transaction ID */
    rejected_tr_id = tcrc.tr_ids.accepted_transaction;

    /* Make trans ID invalid */
    tcrc.tr_ids.accepted_transaction = RRCTC_INVALID_TRANSACTION_ID;

    if ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
        (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
    {

      if(current_state == RRC_STATE_CELL_DCH) 
      {
        /*L2 Ack for the TCRC Complete message is received on the old configuration
        Now initiate a cell selection */
        rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_TCR,
                                                RRC_TRANSITION_FROM_DCH,
                                                tcrc.state_indicated_by_msg);
        /*wait for the state_change indication after the cell selection
        is successfully completed */
        tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

        /* Start T_305 [periodic cell update] timer */
        rrccu_start_t_305_timer();

      }
      else if(current_state == RRC_STATE_CELL_FACH) 
      {
        /* 
           Trigger cell update with cause OOS if inter frequency reselection is in progress
           Procedure substate remains same, once CU is compelted, failure meassage will be sent 
           to NW.
        */
        if(rrccsp_is_inter_frequency_cell_resel_in_progress() == TRUE)
        {
          rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_TCR,
                                              RRC_REENTERED_SERVICE_AREA,
                                              TRUE);
          return;
        }
        if (RRCCU_STARTED == rrccu_get_cell_update_started_status_with_cause(&cu_cause) &&
            (cu_cause == rrc_CellUpdateCause_re_enteredServiceArea || cu_cause == rrc_CellUpdateCause_rlc_unrecoverableError ||
             cu_cause == rrc_CellUpdateCause_radiolinkFailure))
        {
          /* This is a case where cell update is in progress while TCR transitions 
             UE to Cell_PCH or URA_PCH. Though L2 Ack is gotten, send a PCR failure 
             later when CU procedure gets Cell update confirm. If we were to act on 
             L2 Ack, RRC moves to Cell_PCH/URA_PCH where cell update confirm message
             is not handled (unless CU registers on TCR's behalf) & UE would stuck 
             in Cell_PCH forever [in the absence of reselections].
          */
          if(SUCCESS != rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_TCR))
          {
            WRRC_MSG0_ERROR("TCR procedure failed to register with CU");
          }

          WRRC_MSG1_HIGH("Registered for Cell Update procedure completion: cu_cause = %d", cu_cause);

          tcrc.status.failure_status = RRCTC_PHY_CHAN_FAILURE;
          tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;

          /* Restore the accepted transaction ID for sending failure later */
          tcrc.tr_ids.rejected_transaction = rejected_tr_id;
        }
        else
        {
          if((tcrc.initiate_cell_selection_trans_from_fach == TRUE )
            &&(rrccsp_check_initiate_cell_selection_handling(NULL) == TRUE)) 
          {
            /* Now copy OC to TOC */
            rrcllcoc_update_toc_with_oc_from_fach();
             
            /*Initiate the cell selection*/
            rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_TCR,
                                                    RRC_TRANSITION_FROM_FACH,
                                                    tcrc.state_indicated_by_msg);
 
             
            tcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
          }
          else
          {
            /* Call the function that creates and sends a channel
              config request to LLC. */
            rrctcrc_send_channel_config_req();
            tcrc.initiate_cell_selection_trans_from_fach = FALSE;
            /* Change the substate since we're waiting for the
               confirmation from LLC for the channel configs */
            tcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
          }
        }
      }
      else
      {
        WRRC_MSG2_ERROR("Invalid combination of rrcstate%d and rrctcrc substate%d",
               current_state, tcrc.curr_substate);
      }
      /*return from here */
      return;
    }
    /* if SRNS relocation is involved with this message then re
    establish the RBs other than RB2 here */
    if (tcrc.tcrc_for_srns_relocation)
    {
      if (RRC_STATE_CELL_DCH == rrc_get_state())
      {
        /* Initiate the re-establishment of remaining RBs before 
        TCRC procedure goes back to initial substate. 
        No need to wait for the confirmation */
        rb_type  = RRC_RE_EST_NON_RB_2_UM_AM;
        rrcrb_send_rlc_re_est_req_to_smc(RRC_PROCEDURE_TCR, rb_type, FALSE);
        /* send the nas cn info if stored */
        if (tcrc.cn_info_saved != FALSE)
        {
          /* Put the command on MM queue */
          rrc_send_rrc_message_to_mm(tcrc.mm_cmd_ptr);
          /* clear the flag */
          tcrc.cn_info_saved = FALSE;
        }
      }
      else
      {
        WRRC_MSG1_ERROR("SRNS reloc flag is ON in rrc state %d", rrc_get_state());
      }
    }
  }
  else
  {

    /* Here make sure that CU is not active.  If CU is active with OOS area handling
       then NACK coming from RLC shall be ingnored as TCRC failure with cause
       CU complete should be sent to NW */
    if (RRC_STATE_CELL_FACH == rrc_get_state() &&
        RRCCU_STARTED == rrccu_get_cell_update_started_status_with_cause(&cu_cause) && 
        (cu_cause == rrc_CellUpdateCause_re_enteredServiceArea || cu_cause == rrc_CellUpdateCause_rlc_unrecoverableError ||
         cu_cause == rrc_CellUpdateCause_radiolinkFailure))
    {
      (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_TCR);
      WRRC_MSG1_HIGH("NACK being ingnored as TCRC failure msg will be sent/Registered for Cell Update procedure completion: cu_cause = %d", cu_cause);
      return;
    }

    /* Make trans ID invalid */
    tcrc.tr_ids.accepted_transaction = RRCTC_INVALID_TRANSACTION_ID;

    if (tcrc.tcrc_for_srns_relocation)
    {
      /* l2ack failed. If reconfiguration involves SARNS relocation then
      go to idle. Otherwise just complete the procedure */
      WRRC_MSG2_ERROR("L2ack failed SRNSReloc:%d, next_state:%d",
            tcrc.tcrc_for_srns_relocation,
            tcrc.state_indicated_by_msg);
      /* Going to Idle should always go through CSP  */
      rrc_transition_to_disconnected_state(RRC_PROCEDURE_TCR,RRC_TX_TO_DISCON_OTHER);
    }
  }

  /* Note OC is set at this point. No need to clear it
  if the next state is cell_pch or ura_pch and
  l2 ack failied because cu procedure will take care of it */
  if (((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)) &&
       (ptr->status != SUCCESS))
  {
    WRRC_MSG0_HIGH("L2 ack failed for PCH st");
    if (OC_SET_FOR_DCH_FACH_TRANS == rrcllc_get_ordered_config_status())
    {
      /* oc is already cleared by CU and reset for dch->fach transition 
      in this case do not clear oc */
      WRRC_MSG0_HIGH("OC is set by cu");
    }
    else
    {
     (void)rrcllc_reset_ordered_config(RRC_PROCEDURE_TCR,TRUE);
    }
  }
  else
  {
    /* Clear ORDERED_CONFIG - UE can now accept other ordered
    configurations */
    rrcllc_clear_ordered_config();  
  }

  if(tcrc.tcrc_for_srns_relocation == TRUE)
  {
    rrcdormancy_update_dormancy_for_srns_hho_cco(RRC_PROCEDURE_TCR);
  }    

  /* Go to the next sub-state in the state machine. That's
  the initial substate since we've sent the TCRC complete
  message */
  tcrc.curr_substate = RRCRB_INITIAL;

  /* Clear our internal variables */
  rrctcrc_clear_procedure();
}
/*===========================================================================

FUNCTION   RRCTCRC_INITIAL_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the TCRC procedure is in the RRCRB_INITIAL substate. This function
  looks at the received event and does the appropriate processing
  of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_initial_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the received command's id */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {

    case RRC_DOWNLINK_SDU_IND:
        /* Process the TCRC message */
      process_tcrc_message( cmd_ptr );
      
      break;

    default:

      /* No other events except Downlink SDU is expected in this
      substate */
      MSG_MED("Ignoring TCRC event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate, 0);
      break;
  }
}


/*===========================================================================

FUNCTION   RRCTCRC_DCH_WFSRB2REESTABLISH_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the TCRC procedure is in the RRCRB_WAIT_FOR_REESTABLISH_SRB2 substate
  and the UE is in CELL_DCH state. This function looks at the 
  received event and does the appropriate processing
  of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_dch_wfsrb2reestablish_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  
  case RRC_RE_EST_RLC_FOR_SRNS_CNF:

    /* Confirmation from SMC of RLC re-establishment confirm */
    rrctcrc_process_rlc_reestablishment_cnf( );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrctcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* Process the tc Reconfig message */  
    process_tcrc_message( cmd_ptr );

    break;

  default:

    /* No other events except Downlink SDU is expected in this
    substate */
    MSG_MED("Ignoring TCRC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate, 0);
    break;
  }
}

/*===========================================================================

FUNCTION   RRCTCRC_DCH_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the TCRC procedure is in the RRCRB_WAIT_FOR_CHAN_CFG_CNF substate
  when the UE is in CELL_DCH state. This function looks at
  the received event and does the appropriate processing
  of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_dch_wfchcnf_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {

    case RRC_CHANNEL_CONFIG_CNF:
      
      /* Confirmation from LLC for the channel configurations */
      rrctcrc_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
      break;

    case RRC_STATE_CHANGE_IND:
      
      /* Indication that RRC's state has changed */
      rrctcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;

    case RRC_DOWNLINK_SDU_IND:
      
        /* Process the TCRC message */
      process_tcrc_message( cmd_ptr );
    
      break;

    default:

      /* No other events expected in this
      substate */
      MSG_MED("Ignoring TCRC event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate, 0);
      break;
  }
}


/*===========================================================================

FUNCTION   rrctcrc_other_wfcucnf_substate_event_handler

DESCRIPTION

  This function handles the events received by this module when
  the TCRC procedure is in the RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate
  and the UE is in a state other than CELL_FACH or CELL_DCH. 
  This function looks at the received event and does the 
  appropriate processing of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_other_wfcucnf_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_STATE_CHANGE_IND:
      /* Indication that RRC's state has changed */
      rrctcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;
    
    default:    
      /* No other events expected in this
      substate */
      MSG_MED("Ignoring TCRC event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate, 0);
      break;
  }
}

/*===========================================================================

FUNCTION   rrctcrc_other_wfcscnf_substate_event_handler

DESCRIPTION

  This function handles the events received by this module when
  the TCRC procedure is in the RRCRB_WAIT_FOR_CELL_SELECTION_CNF substate
  and the UE is in a state other than CELL_FACH or CELL_DCH. 
  This function looks at the received event and does the 
  appropriate processing of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_other_wfcscnf_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_STATE_CHANGE_IND:
      /* Indication that RRC's state has changed */
      rrctcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;

    
    default:    
      /* No other events expected in this
      substate */
      WRRC_MSG2_HIGH("Ignoring TCRC event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate);
      break;
  }
}




/*===========================================================================

FUNCTION   RRCTCRC_DCH_WFCUCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the TCRC procedure is in the RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate
  and the UE is in CELL_DCH state. This function looks at 
  the received event and does the appropriate processing
  of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

static void rrctcrc_dch_wfcucnf_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_CELL_UPDATE_COMPLETE_CNF:
      if(tcrc.status.failure_status == RRCTC_SIMULTANEOUS_RECONFIG)
      {
        /* Send the TC Reconfig Failure Message */
        send_tcrc_failure_message(tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause
                                );
        /* Then clear all local variables that may
        need to be cleared */
        rrctcrc_clear_procedure();

        /* Make sure we're back in the initial sub-state */
        tcrc.curr_substate = RRCRB_INITIAL;
      }
      else if(tcrc.status.failure_status == RRCTC_MSG_VALID)
      {
        /* Send the TC Reconfig Complete message */
        if ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
            (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
        {
          send_tcrc_complete_message(TRUE);
        }
        else
        {
          send_tcrc_complete_message(FALSE);
        }
        /* prints rate for each transport channel */
        rrcllc_print_trans_chnl_rate();
        /*if next state is Cell_PCH or URA_PCH then we have to 
        treat the procedure as successful only after getting L2ack
        for the response message */
        if ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
            (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
        {
          /* Change the substate to the next substate - this is
          where we wait for L2 ack from RLC for sending the RB RC
          complete message. */
          tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
          /* prints rate for each transport channel */
          rrcllc_print_trans_chnl_rate();
        }
        else
        {
          /* clear the ordered config here
          treat the procedure as successful. */
          rrcllc_clear_ordered_config();  

          /* send the nas cn info if stored */
          if (tcrc.cn_info_saved != FALSE)
          {
            /* Put the command on MM queue */
            rrc_send_rrc_message_to_mm(tcrc.mm_cmd_ptr);
            /* clear the flag */
            tcrc.cn_info_saved = FALSE;
          }

          /* Go to the next sub-state in the state machine. That's
          the initial substate since we've sent the RB RC complete
          message */
          tcrc.curr_substate = RRCRB_INITIAL;

          /* At this point we don't need to remember any of the
          Radio Bearer information since it's stored in ESTABLISHED_RABS.
          Clear our internal variables */
          rrctcrc_clear_procedure();
        }
      }
      /*  In the case of Phy_Chan_Failure, RB setup failure must be send
      and oc should be reset. */
      else if(tcrc.status.failure_status == RRCTC_PHY_CHAN_FAILURE)
      {
        
        /* Send the RB Setup Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause
                                );

        /* Then clear all local variables that may
        need to be cleared */
        rrctcrc_clear_procedure();
        /* Make sure we're back in the initial sub-state */
        tcrc.curr_substate = RRCRB_INITIAL;
      }
      else
      {
        WRRC_MSG1_ERROR("Failure status %d not handled",tcrc.status.failure_status);
      }

    break;

  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrctcrc_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrctcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* Process the TC Reconfig message */
    process_tcrc_message( cmd_ptr );
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring TCRC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate, 0);
    break;
  }

}



/*===========================================================================

FUNCTION   RRCTCRC_FACH_WFCUCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the TCRC procedure is in the RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate
  and the UE is in CELL_FACH state. This function looks at 
  the received event and does the appropriate processing
  of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

static void rrctcrc_fach_wfcucnf_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_CELL_UPDATE_COMPLETE_CNF:
#ifdef FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE
      #error code not present
#endif
      if(tcrc.status.failure_status == RRCTC_SIMULTANEOUS_RECONFIG)
      {
        /* Send the TC Reconfig Failure Message */
        send_tcrc_failure_message(tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause
                                );
        /* Then clear all local variables that may
        need to be cleared */
        rrctcrc_clear_procedure();

        /* Make sure we're back in the initial sub-state */
        tcrc.curr_substate = RRCRB_INITIAL;
      }
      else if(tcrc.status.failure_status == RRCTC_MSG_VALID)
      {

        /* if OOS happens while waiting for cell update confirm then send  
         * channel config request again to reset TOC and to configure lower layers
           The extra condition added for checking the toc_usage_change_oos_to_dch variable 
           is not needed. It has just been added for protection. The extra check is
           sending another channel configuration so removing the check.
           The extra check is not needed because in case of radio bearer release/setup
           we end up releasing/establishing the same rb twice which will fail.*/
        if (rrcllc_get_toc_usage() ==TOC_FOR_OOS)
        {
          WRRC_MSG1_HIGH("Send channel config req for OOS scenarios TOC usage %d  ",
            rrcllc_get_toc_usage());
					
          /* Call the function that creates and sends the  channel
             config information to be requested from LLC. */
          rrctcrc_send_channel_config_req();
  
          /* Change the substate since we're waiting for the
             confirmation from LLC for the channel configs */
          tcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;	
          return;					
        }

        /* Send the TC Reconfig Complete message */
        if ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
            (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
        {
          send_tcrc_complete_message(FALSE);
        }
        else
        {
          send_tcrc_complete_message(FALSE);
        }
        /* prints rate for each transport channel */
        rrcllc_print_trans_chnl_rate();
        /* if next state is Cell_PCH or URA_PCH then we have to 
           treat the procedure as successful only after getting L2ack
           for the response message */
        if ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
            (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
        {
          /* Change the substate to the next substate - this is
             where we wait for L2 ack from RLC for sending the RB RC
             complete message. */
          tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
        }
        else
        {
          /* clear the ordered config here
             treat the procedure as successful. */
          rrcllc_clear_ordered_config();  

          /* send the nas cn info if stored */
          if (tcrc.cn_info_saved != FALSE)
          {
            /* Put the command on MM queue */
            rrc_send_rrc_message_to_mm(tcrc.mm_cmd_ptr);
            /* clear the flag */
            tcrc.cn_info_saved = FALSE;
          }

          /* Go to the next sub-state in the state machine. That's
             the initial substate since we've sent the RB RC complete
             message */
          tcrc.curr_substate = RRCRB_INITIAL;

          /* At this point we don't need to remember any of the
             Radio Bearer information since it's stored in ESTABLISHED_RABS.
             Clear our internal variables */
          rrctcrc_clear_procedure();
        }
      }
      /*  In the case of Phy_Chan_Failure, RB setup failure must be send
      and oc should be reset. */
      else if(tcrc.status.failure_status == RRCTC_PHY_CHAN_FAILURE)
      {
        
        /* Send the RB Setup Failure Message */
        send_tcrc_failure_message( tcrc.status.failure_status,
                                  tcrc.status.prot_err_cause
                                );

        /* Then clear all local variables that may
        need to be cleared */
        rrctcrc_clear_procedure();
        /* Make sure we're back in the initial sub-state */
        tcrc.curr_substate = RRCRB_INITIAL;
      }
      else
      {
        WRRC_MSG1_ERROR("Failure status %d not handled",tcrc.status.failure_status);
      }

    break;

  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrctcrc_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrctcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* Process the TC Reconfig message */
    process_tcrc_message( cmd_ptr );
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring TCRC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate, 0);
    break;
  }

}

/*===========================================================================

FUNCTION   RRCTCRC_FACH_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the TCRC procedure is in the RRCRB_WAIT_FOR_CHAN_CFG_CNF substate
  and the UE is in CELL_FACH state. This function looks at 
  the received event and does the appropriate processing
  of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_fach_wfchcnf_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {

    case RRC_CHANNEL_CONFIG_CNF:
      
      /* Confirmation from LLC for the channel configurations */
      rrctcrc_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
      break;

    case RRC_STATE_CHANGE_IND:
      
      /* Indication that RRC's state has changed */
      rrctcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;

    case RRC_DOWNLINK_SDU_IND:
      
        /* Process the TCRC message */
       process_tcrc_message( cmd_ptr );
      
      break;

    default:

      /* No other events expected in this
      substate */
      MSG_MED("Ignoring TCRC event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate, 0);
      break;
  }
}



/*===========================================================================

FUNCTION   RRCTCRC_OTHER_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the TCRC procedure is in the RRCRB_WAIT_FOR_CHAN_CFG_CNF substate
  and the UE is in any other state except CELL_DCH and CELL_FACH. 
  This function looks at the received event and does the appropriate
  processing of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_other_wfchcnf_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the received command's id */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {

    case RRC_CHANNEL_CONFIG_CNF:
    case RRC_DOWNLINK_SDU_IND:		
      
      /* Confirmation from LLC for the channel configurations */
      WRRC_MSG1_HIGH("Ignoring chan config cnf/downlink sdu since UE not in DCH or FACH cmd_id:%0x", cmd_ptr->cmd_hdr.cmd_id);
      /* In this state this procedure should no longer be active. */

      /* Then change to initial substate */
      tcrc.curr_substate = RRCRB_INITIAL;

      /* Then clear all local variables that may
      need to be cleared */
      rrctcrc_clear_procedure();
      break;

    case RRC_STATE_CHANGE_IND:
      
      /* Indication that RRC's state has changed */
      rrctcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;

    default:

      /* No other events expected in this
      substate */
      MSG_MED("Ignoring TCRC event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate, 0);
      break;
  }
}



/*===========================================================================

FUNCTION   RRCTCRC_DCH_WFL2ACK_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the TCRC procedure is in the RRCRB_WAIT_FOR_L2_ACK substate
  and the UE is in CELL_DCH state. This function looks at the 
  received event and does the appropriate processing
  of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_dch_wfl2ack_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {

    case RRC_DOWNLINK_L2ACK_IND:
      
      /* Confirmation from LLC for the channel configurations */
      rrctcrc_process_l2_ack_ind( &(cmd_ptr->cmd.downlink_l2ack_ind) );
      break;

    case RRC_STATE_CHANGE_IND:
      
      /* Indication that RRC's state has changed */
      rrctcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;

    case RRC_DOWNLINK_SDU_IND:
      
        /* Process the TCRC message */
      process_tcrc_message( cmd_ptr );
      
      break;

    case RRC_CELL_UPDATE_COMPLETE_CNF:
      /* The only reason this scenario could occur is if while in wait_for_l2_ack substate oos area
         happened. In this case, send failure msg of rbrc with cause cell update occured 
      */
      tcrc.status.failure_status = RRCTC_CELL_UPDATE_OCCURED;
      /* Send the TC Reconfig Failure Message */
      send_tcrc_failure_message(tcrc.status.failure_status,
                                tcrc.status.prot_err_cause
                                );
      /* Then clear all local variables that may
      need to be cleared */
      rrctcrc_clear_procedure();

      /* Make sure we're back in the initial sub-state */
      tcrc.curr_substate = RRCRB_INITIAL;
      break;

    default:

      /* No other events except Downlink SDU is expected in this
      substate */
      MSG_MED("Ignoring TCRC event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate, 0);
      break;
  }
}


/*===========================================================================

FUNCTION   RRCTCRC_FACH_WFL2ACK_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the TCRC procedure is in the RRCRB_WAIT_FOR_L2_ACK substate
  and the UE is in CELL_FACH state. This function looks at the 
  received event and does the appropriate processing
  of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_fach_wfl2ack_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {

    case RRC_DOWNLINK_L2ACK_IND:
      
      /* Confirmation from LLC for the channel configurations */
      rrctcrc_process_l2_ack_ind( &(cmd_ptr->cmd.downlink_l2ack_ind) );
      break;

    case RRC_STATE_CHANGE_IND:
      
      /* Indication that RRC's state has changed */
      rrctcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;

    case RRC_DOWNLINK_SDU_IND:
      
        /* Process the TCRC message */
      process_tcrc_message( cmd_ptr );
      
      break;

    case RRC_CELL_UPDATE_COMPLETE_CNF:
      /* The only reason this scenario could occur is if while in wait_for_l2_ack substate oos area
         happened. In this case, send failure msg of rbrc with cause cell update occured 
      */
      tcrc.status.failure_status = RRCTC_CELL_UPDATE_OCCURED;
      /* Send the PC Reconfig Failure Message */
      send_tcrc_failure_message(tcrc.status.failure_status,
                                tcrc.status.prot_err_cause
                                );
      /* Then clear all local variables that may
      need to be cleared */
      rrctcrc_clear_procedure();

      /* Make sure we're back in the initial sub-state */
      tcrc.curr_substate = RRCRB_INITIAL;
      break;

    default:

      /* No other events except Downlink SDU is expected in this
      substate */
      MSG_MED("Ignoring TCRC event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate, 0);
      break;
  }
}

/*===========================================================================

FUNCTION   RRCTCRC_FACH_WFCRNTIUPDATECNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the TCRC procedure is in the RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF substate
  and the UE is in CELL_FACH state. This function looks at 
  the received event and does the appropriate processing
  of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

static void rrctcrc_fach_wfcrntiupdatecnf_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_STATE_CHANGE_IND:
   /* Indication that RRC's state has changed */
    rrctcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
   
    break;


  case RRC_RNTI_UPDATE_CNF:
    if (cmd_ptr->cmd.rnti_update_cnf.status == SUCCESS) 
    {
      rrctcrc_process_rnti_update_cnf();
    }
    else
    {
      WRRC_MSG0_HIGH("RNTI update failed.  send TCRC failure");
      tcrc.status.failure_status = RRCTC_SIMULTANEOUS_RECONFIG;
      /* Send the TCRC Failure Message */
      send_tcrc_failure_message( tcrc.status.failure_status,
                                 tcrc.status.prot_err_cause );

      
      /* Then clear all local variables that may
         need to be cleared */
      rrctcrc_clear_procedure();

      /* Make sure we're back in the initial sub-state */
      tcrc.curr_substate = RRCRB_INITIAL;

    }
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring TCRC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate, 0);
    break;
  }

}


/*===========================================================================

FUNCTION   RRCTCRC_FACH_WFCSCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the TCRC procedure is in the RRCRB_WAIT_FOR_CELL_SELECTION_CNF substate
  and the UE is in CELL_FACH state. This function looks at 
  the received event and does the appropriate processing
  of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

static void rrctcrc_fach_wfcscnf_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
   /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

    case RRC_STATE_CHANGE_IND:
     /* Indication that RRC's state has changed */
      rrctcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;
    case RRC_NEW_CELL_IND:
      rrctcrc_process_new_cell_ind( &(cmd_ptr->cmd.new_cell_ind) );
      break;
      
    case RRC_CELL_UPDATE_COMPLETE_CNF:
      if(tcrc.state_indicated_by_msg == RRC_STATE_CELL_FACH)
      {
      
        /* if OOS was detected during trans from DCH, cell update
           procedure has completed.  send channel config req to 
           complete the reconfiguration procedure
        */
        if ((rrcllc_get_toc_usage() ==TOC_FOR_OOS) 
            || (rrcllc_get_toc_usage() ==TOC_FOR_OOS_WITHOUT_DCH_INFO)
            ||(rrcllc_get_toc_usage() ==TOC_FOR_OOS_WITH_DCH_INFO)
            )
        {
          /* Call the function that creates and sends the  channel
          config information to be requested from LLC. */
          rrctcrc_send_channel_config_req();
    
          /* Also reset the rrctcrc_send_initiate_cu_flag to false as cell update
           * will not be needed since CU and CUCNF for oos took care 
           * of that
           */
          rrctcrc_send_initiate_cu_flag = FALSE;
          /* Change the substate since we're waiting for the
          confirmation from LLC for the channel configs */
          tcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;         
        }
        else
        {
          WRRC_MSG0_ERROR("Expected Cell Update Cnf command in Wait for Cell Selection state only in case of OOS");
        }
      }
      else
      {
         WRRC_MSG0_ERROR("Expected Cell Update Cnf in Cell Selection cnf state only in FACH->FACh case");
      }    
      break;

    default:
      /* No other events expected in this
      substate */
      WRRC_MSG2_HIGH("Ignoring TCRC event %x in substate %d",
         cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate);
      break;
   }


}



/*===========================================================================

FUNCTION   RRCTCRC_OTHER_WFL2ACK_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the TCRC procedure is in the RRCRB_WAIT_FOR_L2_ACK substate
  and the UE is in any other state except CELL_DCH and CELL_FACH. 
  This function looks at the received event and does the appropriate
  processing of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_other_wfl2ack_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  
  /* Switch on the received command's id */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {

    case RRC_DOWNLINK_L2ACK_IND:
    case RRC_DOWNLINK_SDU_IND:
      
      /* Confirmation from LLC for the channel configurations */
      WRRC_MSG0_HIGH("Ignoring L2 ack/downlink SDU since UE not in DCH or FACH");
      /* In this state this procedure should no longer be active. */

      /* Then change to initial substate */
      tcrc.curr_substate = RRCRB_INITIAL;

      /* Then clear all local variables that may
      need to be cleared */
      rrctcrc_clear_procedure();
      break;

    case RRC_STATE_CHANGE_IND:
      
      /* Indication that RRC's state has changed */
      rrctcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;

    default:

      /* No other events except Downlink SDU is expected in this
      substate */
      MSG_MED("Ignoring TCRC event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate, 0);
      break;
  }
}


/*===========================================================================

FUNCTION   RRCTCRC_PCH_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the TCRC procedure is in the RRCRB_WAIT_FOR_CHAN_CFG_CNF substate
  when the UE is in CELL_PCH state. This function looks at
  the received event and does the appropriate processing
  of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_pch_wfchcnf_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{


  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  
  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrctcrc_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrctcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:
  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring TCRC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate, 0);
    break;
  }
}

/*===========================================================================

FUNCTION   RRCTCRC_PCH_WFCUCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the TCRC procedure is in the RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate
  and the UE is in CELL_PCH state. This function looks at 
  the received event and does the appropriate processing
  of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_pch_wfcucnf_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  case RRC_CELL_UPDATE_COMPLETE_CNF:
    /*For the scenario DCH to FACH through reconfig and UE does a cell update and CUCNF moves to PCH state.
    After CUCNF is completed procedure is cleared but procedure is not sending reconfigurarion complete
    Below changes will send the reconfiguration complete which will push UE out of PCH state*/
    if(tcrc.status.failure_status != RRCTC_MSG_VALID)
    {
      /* Send the PC Reconfig Failure Message */
      send_tcrc_failure_message(tcrc.status.failure_status,
                                tcrc.status.prot_err_cause
                              );
      /* Then clear all local variables that may
      need to be cleared */
      rrctcrc_clear_procedure();

      /* Make sure we're back in the initial sub-state */
      tcrc.curr_substate = RRCRB_INITIAL;
    }
    else 
    {
      /* Send the TC Reconfig Complete message */
      if ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
          (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))

      {
        send_tcrc_complete_message(TRUE);
      }
      else
      {
        send_tcrc_complete_message(FALSE);
      }
      /* prints rate for each transport channel */
      rrcllc_print_trans_chnl_rate();
      /*if next state is Cell_PCH or URA_PCH then we have to 
      treat the procedure as successful only after getting L2ack
      for the response message */
      if ((tcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
          (tcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))

      {
        /* Change the substate to the next substate - this is
        where we wait for L2 ack from RLC for sending the RB RC
        complete message. */
        tcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
      }
      else
      {
        /* clear the ordered config here
        treat the procedure as successful. */
        rrcllc_clear_ordered_config();  

        /* Go to the next sub-state in the state machine. That's
        the initial substate since we've sent the RB RC complete
        message */
        tcrc.curr_substate = RRCRB_INITIAL;

        /* At this point we don't need to remember any of the
        Radio Bearer information since it's stored in ESTABLISHED_RABS.
        Clear our internal variables */
        rrctcrc_clear_procedure();
      }
    }
    break;

  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrctcrc_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:
    /* Indication that RRC's state has changed */
    rrctcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

 

    case RRC_DOWNLINK_SDU_IND:
      WRRC_MSG2_HIGH("Unexpected DOWNLINK_SDU_IND in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate);
    break;

  default:

    /* No other events expected in this
    substate */
    WRRC_MSG2_HIGH("Ignoring TCRC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate);
    break;
  }

}

/*===========================================================================

FUNCTION   RRCTCRC_PCH_WF_CSCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the TCRC procedure is in the RRCRB_WAIT_FOR_CELL_SELECTION_CNF substate
  and the UE is in CELL_PCH state. This function looks at 
  the received event and does the appropriate processing
  of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

static void rrctcrc_pch_wfcscnf_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_STATE_CHANGE_IND:
   /* Indication that RRC's state has changed */
    rrctcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
   
    break;


  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring TCRC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate, 0);
    break;
  }

}

/*===========================================================================

FUNCTION   RRCTCRC_CELL_DCH_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when RRC
  is in CELL_DCH state. This module checks the current sub-state of
  this procedure and calls the appropriate sub-state function to
  process the received event.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_cell_dch_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  if ((cmd_ptr->cmd_hdr.cmd_id == RRC_DOWNLINK_L2ACK_IND) &&
      (tcrc.curr_substate != RRCRB_WAIT_FOR_L2_ACK))
  {
    /* We could get L2ack for a previously sent message while we are already
    processing the next message. So l2ack could be received any any of the following
    substates. If TCRC substate does not indicate that we are not waiting for l2ack
    then just print the status */
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"TCRC:L2ack status %d <0-Success 1-Failure> for MUI:%d, Lc_Id:%d, tcrc_subst:%d",
               cmd_ptr->cmd.downlink_l2ack_ind.status,
               cmd_ptr->cmd.downlink_l2ack_ind.mui, 
               cmd_ptr->cmd.downlink_l2ack_ind.lc_id, tcrc.curr_substate);

    /* return from here so that procedure can continue with existing process */
    return;
  }
  /* Switch on the current substate */
  switch( tcrc.curr_substate )
  {
    case RRCRB_INITIAL:
     
      /* Call the event handler for RRCRB_INITIAL substate */
      rrctcrc_initial_substate_event_handler( cmd_ptr );
      break;

    case RRCRB_WAIT_FOR_CHAN_CFG_CNF:
     
      /* Call the event handler for RRCRB_WAIT_FOR_CHAN_CFG_CNF substate */
      rrctcrc_dch_wfchcnf_substate_event_handler( cmd_ptr );
      break;

    case RRCRB_WAIT_FOR_L2_ACK:
     
      /* Call the event handler for RRCRB_WAIT_FOR_L2_ACK substate */
      rrctcrc_dch_wfl2ack_substate_event_handler( cmd_ptr );
      break;

    case RRCRB_WAIT_FOR_REESTABLISH_SRB2:
      rrctcrc_dch_wfsrb2reestablish_substate_event_handler(cmd_ptr);
      break;

    case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
      /* Call the event handler for RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate */
      rrctcrc_dch_wfcucnf_event_handler( cmd_ptr);
      break;

    default:

      /* No other substates processed for this RRC state    */
      MSG_MED("Command %x not processed in CELL_DCH for substate %d", 
              cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate, 0);
      break;

  }
}

                                                                  

/*===========================================================================

FUNCTION   RRCTCRC_CELL_FACH_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when RRC
  is in CELL_FACH state. This module checks the current sub-state of
  this procedure and calls the appropriate sub-state function to
  process the received event.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_cell_fach_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  if ((cmd_ptr->cmd_hdr.cmd_id == RRC_DOWNLINK_L2ACK_IND) &&
      (tcrc.curr_substate != RRCRB_WAIT_FOR_L2_ACK))
  {
    /* We could get L2ack for a previously sent message while we are already
    processing the next message. So l2ack could be received any any of the following
    substates. If TCRC substate does not indicate that we are not waiting for l2ack
    then just print the status */
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"TCRC:L2ack status %d <0-Success 1-Failure> for MUI:%d, Lc_Id:%d, tcrc_subst:%d",
               cmd_ptr->cmd.downlink_l2ack_ind.status,
               cmd_ptr->cmd.downlink_l2ack_ind.mui, 
               cmd_ptr->cmd.downlink_l2ack_ind.lc_id, tcrc.curr_substate);

    /* return from here so that procedure can continue with existing process */
    return;
  }

  /* Switch on the current substate */
  switch( tcrc.curr_substate )
  {
    case RRCRB_INITIAL:
      /* Call the event handler for RRCRB_INITIAL substate */
      /* NOTE: Here we need to check if a cell-update procedure
      has been initialized or not. For now, just call the event handler */
      rrctcrc_initial_substate_event_handler( cmd_ptr );
      break;
   
    case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
      /* Call the event handler for RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate */
      rrctcrc_fach_wfcucnf_event_handler( cmd_ptr);
      break;

    case RRCRB_WAIT_FOR_CHAN_CFG_CNF:
      /* Call the event handler for RRCRB_WAIT_FOR_CHAN_CFG_CNF substate */
      rrctcrc_fach_wfchcnf_substate_event_handler( cmd_ptr );
      break;

    case  RRCRB_WAIT_FOR_CELL_SELECTION_CNF:
      /* Call the event handler for RRCRB_WAIT_FOR_CHAN_CFG_CNF substate */
      rrctcrc_fach_wfcscnf_substate_event_handler( cmd_ptr );
      break;

    case RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF:
        /* Call the event handler for RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF substate */
        rrctcrc_fach_wfcrntiupdatecnf_substate_event_handler( cmd_ptr );
        break;

    case RRCRB_WAIT_FOR_L2_ACK:
      /* Call the event handler for RRCRB_WAIT_FOR_L2_ACK substate */
      rrctcrc_fach_wfl2ack_substate_event_handler( cmd_ptr );
      break;

    default:
      /* No other substates processed for this RRC state    */
      MSG_MED("Command %x not processed in CELL_FACH for substate %d", 
              cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate, 0);
      break;
  }
}


/*===========================================================================

FUNCTION   RRCTCRC_PCH_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when RRC
  is in CELL_PCH/URA_PCH state. This module checks the current sub-state of
  this procedure and calls the appropriate sub-state function to
  process the received event.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_pch_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the current substate */
  switch ( tcrc.curr_substate )
  {
  case RRCRB_INITIAL:

    /* Call the event handler for RRCTCRC_INITIAL substate */
    rrctcrc_initial_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
    /* Call the event handler for RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate */
    rrctcrc_pch_wfcucnf_event_handler( cmd_ptr);
    break;

  case RRCRB_WAIT_FOR_CELL_SELECTION_CNF:

    rrctcrc_pch_wfcscnf_event_handler( cmd_ptr);
    break;

  case RRCRB_WAIT_FOR_CHAN_CFG_CNF:

    /* Call the event handler for RRCRB_WAIT_FOR_CHAN_CFG_CNF substate */
    rrctcrc_pch_wfchcnf_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_L2_ACK:

    /* The only way we could be in pch and rb state is "wait_for_l2_ack" is if
       oos happened while waiting for l2 ack and cell update confirm takes UE to pch
       In this case, send failure msg of rbrc with cause cell update occured 
    */
    tcrc.status.failure_status = RRCTC_CELL_UPDATE_OCCURED;

    send_tcrc_failure_message(tcrc.status.failure_status,
                              tcrc.status.prot_err_cause);

    /* Then clear all local variables that may
      need to be cleared */
    rrctcrc_clear_procedure();

    /* Make sure we're back in the initial sub-state */
    tcrc.curr_substate = RRCRB_INITIAL;
    break;

  default:

    /* No other substates processed for this RRC state    */
    MSG_MED("Command %x not processed in substate%d rrc_state%d", 
            cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate, rrc_get_state());
    break;

  }
}



/*===========================================================================

FUNCTION   RRCTCRC_OTHER_STATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when RRC
  is in any other state except CELL_FACH and CELL_DCH. This module 
  checks the current sub-state of this procedure and calls the
  appropriate sub-state function to process the received event.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrctcrc_other_state_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the current substate */
  switch( tcrc.curr_substate )
  {
    case RRCRB_WAIT_FOR_CHAN_CFG_CNF:
      /* Call the event handler for WAIT_FOR_CHAN_CFG_CNF substate */
      rrctcrc_other_wfchcnf_substate_event_handler( cmd_ptr );
      break;

    case RRCRB_WAIT_FOR_L2_ACK:
      /* Call the event handler for WAIT_FOR_L2_ACK substate */
      rrctcrc_other_wfl2ack_substate_event_handler( cmd_ptr );
      break;

    case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
      rrctcrc_other_wfcucnf_substate_event_handler( cmd_ptr);
      break;

    case  RRCRB_WAIT_FOR_CELL_SELECTION_CNF:
      /* Call the event handler for RRCRB_WAIT_FOR_CHAN_CFG_CNF substate */
      rrctcrc_other_wfcscnf_substate_event_handler( cmd_ptr );
      break;


    default:
      /* No other substates processed for this RRC state    */
      MSG_MED("Command %x not processed in IDLE or PCH states for substate %d", 
              cmd_ptr->cmd_hdr.cmd_id, tcrc.curr_substate, 0);
      break;
  }
}



/*===========================================================================

FUNCTION RRCTCRC_PROCEDURE_EVENT_HANDLER


DESCRIPTION

  This is the main function handling external events received by the
  TCRC procedure. This function is called by the
  dispatcher whenever an event is received for this module. This function
  checks the current RRC state and calls the event handler for
  that state. 
  
  Note that the TCRC procedure can be initiated
  only in the CELL_FACH and CELL_DCH states. However, while the
  procedure is active, the RRC state may change to any of the other
  states. Hence all RRC states are checked by this module.

DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
  
===========================================================================*/
void rrctcrc_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  /* Get the current RRC State */
  rrc_state_e_type rrc_state = rrc_get_state();

  WRRC_MSG2_HIGH_OPT("function rrctcrc_procedure_event_handler is called in rrc_state = %d and tcrc.curr_substate = %d", rrc_state, tcrc.curr_substate);
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_TCR,tcrc.curr_substate,cmd_ptr->cmd_hdr.cmd_id);

  switch (rrc_state)
  {

    case RRC_STATE_CELL_FACH:    
      /* If it's CELL_FACH state, call the event handler
      for CELL_FACH state */
      rrctcrc_cell_fach_event_handler(cmd_ptr);
      break;

    case RRC_STATE_CELL_DCH:    
      /* If it's CELL_DCH state, call the event handler
      for CELL_DCH state */
      rrctcrc_cell_dch_event_handler(cmd_ptr);
      break;

    case RRC_STATE_URA_PCH:
    case RRC_STATE_CELL_PCH:
      /* If it's CELL_DCH state, call the event handler
      for CELL_PCH state */
      rrctcrc_pch_event_handler(cmd_ptr);
      break;

    case RRC_STATE_CONNECTING:   
    case RRC_STATE_DISCONNECTED: 
      /* If it's any other state call the event handler
      for other states. The processing of events in
      other RRC states is identical for this procedure */
      rrctcrc_other_state_event_handler(cmd_ptr);
      break;

    default:  
      WRRC_MSG1_ERROR("Invalid RRC State: %d", rrc_state);
      break;
  }  
}/* end function rrctcrc_procedure_event_handler */
/*===========================================================================

FUNCTION rrctcr_build_tcr_complete_message

DESCRIPTION
  This function appends start_list to tcrc-complete message, needed during
  SRNS relocation or while going from non-dch to dch state when TM bearers
  are active
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrctcr_build_tcr_complete_message
(
rrc_RRC_TransactionIdentifier trans_id,     /* Transaction Id */
rrc_UL_DCCH_Message *msg_ptr,               /* Pointer to Uplink DCCH Msg */
boolean              srns_reloc,            /* Indicates whether SRNS reloc is involved */ 
boolean              cipher_update_required /* Indicates whether TM ciphering is present */
)
{                                         

  /* Select the message as TCR Complete message */
  msg_ptr->message.t = T_rrc_UL_DCCH_MessageType_transportChannelReconfigurationComplete;

  /* Initialize the bit mask not to include the optional fields */
  RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.transportChannelReconfigurationComplete);

  /* If SRNS relocation was triggered as a result of this message then 
  append the start list */
  if (srns_reloc == TRUE)
  {
    rrctcr_append_start_list_to_tcrc_complete(msg_ptr);
  }

  if((cipher_update_required == TRUE) && (TRUE == rrcsmc_tm_ciph_active()))
  {
    if(! (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.transportChannelReconfigurationComplete,
           rrc_TransportChannelReconfigurationComplete,ul_CounterSynchronisationInfo)))
    {
      rrctcr_append_start_list_to_tcrc_complete(msg_ptr);
    }

    //Set the bitmask for count_c_act_time and copy the same..
    RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.transportChannelReconfigurationComplete,
      rrc_TransportChannelReconfigurationComplete,count_C_ActivationTime);

    msg_ptr->message.u.transportChannelReconfigurationComplete.count_C_ActivationTime =
      rrcsmc_step_2_cipher_config_for_hho();
  }

  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
  {
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
    if(rrcsib_is_dmcr_ie_needed() == TRUE)
    {
      msg_ptr->message.u.transportChannelReconfigurationComplete.m.
        laterNonCriticalExtensionsPresent = 1;
 
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.transportChannelReconfigurationComplete.
         laterNonCriticalExtensions);
    
      msg_ptr->message.u.transportChannelReconfigurationComplete.
        laterNonCriticalExtensions.m.v770NonCriticalExtensionsPresent = 1;
 
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.transportChannelReconfigurationComplete.
        laterNonCriticalExtensions.v770NonCriticalExtensions);
    
      msg_ptr->message.u.transportChannelReconfigurationComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        transportChannelReconfigurationComplete_v770ext.m.deferredMeasurementControlReadingPresent = 1;
      
      msg_ptr->message.u.transportChannelReconfigurationComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        transportChannelReconfigurationComplete_v770ext.deferredMeasurementControlReading = 
        rrc_TransportChannelReconfigurationComplete_v770ext_IEs_deferredMeasurementControlReading_true;
    }
#endif
  }

  /* Initialize the Transaction Id */
  msg_ptr->message.u.transportChannelReconfigurationComplete.
  rrc_TransactionIdentifier = trans_id;
} /* rrctcr_build_tcr_complete_message */


/*===========================================================================

FUNCTION  RRCTCRC_INIT_PROCEDURE

DESCRIPTION

  This function initializes the TCRC procedure.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrctcrc_init_procedure( void )
{

  /* Initialize the transaction id's for this procedure.
  Use a number that can't be an actual received transaction
  id. */
  tcrc.tr_ids.accepted_transaction = RRCTC_INVALID_TRANSACTION_ID;
  tcrc.tr_ids.rejected_transaction = RRCTC_INVALID_TRANSACTION_ID;

  /* Initialize the current substate */
  tcrc.curr_substate = RRCRB_INITIAL;
  rrctcrc_send_initiate_cu_flag = FALSE;

  /* Initialize CRNTI valid to FALSE */
  tcrc.new_crnti_valid    = FALSE;
  tcrc.directed_cell_info = FALSE;

  /*clear the SRNS relocation flag */
  tcrc.tcrc_for_srns_relocation = FALSE;

  tcrc.hho_ciph_status = FALSE;

  /* set cn_info_saved to false */
  tcrc.cn_info_saved = FALSE;

  tcrc.trans_to_dch_ciph_config_needed = FALSE;

  tcrc.high_mobility_ind = FALSE;
  tcrc.status.failure_status = RRCTC_MSG_VALID;
  /* Register for state change notification -
  We want to be notified when the state changes
  from CELL_DCH to CELL_FACH and from any state
  to disconnected state. */
  rrcscmgr_register_for_scn( RRC_PROCEDURE_TCR,  /* Procedure name */
                             RRC_STATE_CELL_DCH, /* From State     */
                             RRC_STATE_CELL_FACH /* To State       */
                           );

  rrcscmgr_register_for_scn( RRC_PROCEDURE_TCR,  /* Procedure name */
                             RRC_STATE_WILDCARD, /* From State     */
                             RRC_STATE_DISCONNECTED /* To State    */
                           );
  rrcscmgr_register_for_scn( RRC_PROCEDURE_TCR,  /* Procedure name */
                             RRC_STATE_CELL_DCH, /* From State     */
                             RRC_STATE_CELL_PCH /* To State       */
                           );
  rrcscmgr_register_for_scn( RRC_PROCEDURE_TCR,  /* Procedure name */
                             RRC_STATE_CELL_DCH, /* From State     */
                             RRC_STATE_URA_PCH /* To State       */
                           );

  rrcscmgr_register_for_scn( RRC_PROCEDURE_TCR,  /* Procedure name */
                             RRC_STATE_CELL_FACH, /* From State     */
                             RRC_STATE_CELL_PCH /* To State       */
                           );

  rrcscmgr_register_for_scn( RRC_PROCEDURE_TCR,  /* Procedure name */
                             RRC_STATE_CELL_FACH, /* From State     */
                             RRC_STATE_URA_PCH /* To State       */
                           );
  tcrc.initiate_cell_selection_trans_from_fach = FALSE;     

  /* Define a timer used to delay 5 seconds before
  sending the TCRC Complete message */
  rex_def_timer(&rrctcrc_temp_timer, rex_self(), RRCTCRC_TEMP_TIMER_SIG);

}
#ifdef FEATURE_QSH_EVENT_METRIC

/*===========================================================================

FUNCTION  

DESCRIPTION

DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

uint8 rrctcr_get_srns_relocation(void)
{
 return tcrc.tcrc_for_srns_relocation;
}

#endif
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
/*===========================================================================

FUNCTION rrctcrc_set_failure_cause

DESCRIPTION

DEPENDENCIES


RETURN VALUE
  Returns 

SIDE EFFECTS

  None

===========================================================================*/
void rrctcrc_set_failure_cause(rrcrb_msg_status_e_type cause)
{
  tcrc.status.failure_status = (rrctc_msg_status_e_type)cause;
}

#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/



