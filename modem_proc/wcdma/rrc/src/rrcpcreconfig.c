/*===========================================================================
              R R C PHYSICAL CHANNEL RECONFIGURATION  M O D U L E 

DESCRIPTION

  This module performs the Physical Channel Reconfiguration Procedure. This
  module processes the Physical Channel Reconfiguration Message and sets up 
  the reconfigurations given by the message. 
  
  
EXTERNALIZED FUNCTIONS

  rrcpcrc_procedure_event_handler   Event handler for this procedure
  rrcpcrc_init_procedure            Power-up initialization of the procedure

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Procedure must be initialized at power-up by calling rrcpcrc_init_procedure. 
  
Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcpcreconfig.c#1 $   

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/16   sp      Made changes to reset CU registration info once reconfig procedure is successful
06/20/16   sp      Made changes to reduce F3s
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
03/22/13   mp      Removed tmc.h include file
01/03/13   ad      Made changes to reduce the redundant F3s
10/31/12   ad      Made changes not to send channel_config from procedure based on 
                   TOC usage flag toc_usage_change_oos_to_dch  
10/26/12   db      EFACH Redesign changes
10/04/12   md      Made changes to detect RLC size change during state transitions via CELL PCH state.
10/04/12   pm      Made sure SRB's are present in Chan_config_req during FACH to DCH transition
09/28/11   mn      Made changes to send channel config request when UE receives cucnf
                   after coming back to service on dch to fach or pch transition     
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/29/12   ad      Made changes for triggering CU if E-RNTI is not given.
                   Skipped the de-registration of WMs if HS-RACH is ON and RRC is 
                   triggering MR while coming out of PCH. Fixed bug to updated E-RNTI
                   in OC correctly. Added code to trigger CU if R99 or R5 CUCNF is received. 
                   Added code to keep L1 and MAC in sync wrt HS-RACH actions.
06/25/12   sa      Made the changes to delete the C-RNTI while configuring the CU
04/25/12   as      Made changes to trigger cell update/drx req when UE recived OTA with 
                   redirection info and cell reselection in progress
04/06/12   ad      Changes for HS-RACH feature
05/29/12   ad      Fixed a bug in R8 FACH to FACH without redirection case.
                   Coding bug in which we return before sending channel_config_req
04/30/12   db      Added check to swap CC and OC only on DCH to FACH fail
04/04/12   as      Made changes to initiate transition to idle when L2 ACK fails while going to PCH
26/03/12   ad      Added changes to set the TOC for OOS if DCH to FACH timer expiry 
                   happens before OOS indication comes from L1. Also added code to 
                   send channel config request from procedure after recovery from OOS.
12/09/11   ad      Added code to log WCDMA_RRC_PROTOCOL_ERRORS packet
12/08/11   db      Made changes to trigger cell update if H-RNTI is missing 
11/20/11   db      Added changes to wait for l2ack in all states, not just DCH.
                   Also set reconfig status indicator in CU
08/17/11   ad      Made changes to send channel config request when UE receives cucnf
                   after coming back to service on dch to fach or pch transition     
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/09/11   ad      Added changes to handle simultaneous reconfig messages 
                   in fach state while waiting for cell update confirm.
                   Compilation warnings fixed.
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/11/10   prk     Removed the code which do double freeing of the memory.
09/17/10   su      Featurizing MVS related code
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
07/14/10   su      Fixed Lint errors.
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
06/10/10   prk     Made changes to wait for L2 ACK after sending reconfiguration 
                   failure message if the failure is due to the lower layer configuration.
06/07/10   ss       Added code to reset SVTHHO in case of failure message
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
05/21/09   gkg     Made changes to correct OTA version which triggers 
                   Cell_FACH/Cell_DCH to Cell_PCH Freq Redirection through R7 PCRC. 
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
11/04/08   rm      Added support for rove-in to GAN in RRC Connected mode.
                   Code changes are under FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE.
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
06/13/08   gkg     Added code for notification of Cell_Id change through OTAs
                   in Cell_DCH state. 
06/13/08   gkg     Made changes so that if RLC PDU size changes for RBs through
                   Transport Channel IEs of PC-Reconfig OTA, those RBs are reconfigured
                   and therefore their RLC entity is notified for new RLC PDU size.
05/30/08   gkg     In case of Cell_FACH->Cell_DCH state transition, stop T305
                   if RRC_CHANNEL_CONFIG_REQ succeeds otherwise handle timer T305
                   by initiating Cell Update.
03/06/08   da      Added MBMS phase 2C changes.  MBMS support in CELL_DCH.  
                   Includes mobility and dynamic resource management.
11/02/08   gkg     Made changes so that whenever there is a mismatch in what is
                   expected in an OTA msg and what is received, always INVALID 
                   Config is returned instead of UNSUPPORTED Config.
11/28/07   da      In case OOS area gets detected during DCH->FACH transitions,
                   made change to setup DCCH+DTCH once returning in service before
                   sending CU message with cause "re-entered service area"
11/05/07   ps      Fixed the bug to send a Channel Conifg Request to LLC on
                   receiving State Change Ind for FACH->PCH.
10/23/07   kp      Added support for network sharing feature. Code changes to handle 
                   primary PLMN I.D in OTA message. Changes are under feature flag 
                   FEATURE_NETWORK_SHARING.
10/08/07   da      Made change to pass correct proc id when registering with scm.
09/25/07   ps      Added fix to send DRX Req to L1, when Redirection IE is
                   given for the camped PSC, in FACH->PCH case. This is valid
                   when FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU is enabled.
09/10/07   ps      Added support for Frequency redirection received in 
                   FACH->FACH and FACH->PCH cases under feature flag
                   FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
08/08/07   ps      Added support for feature cell id reading in DCH state, 
                   under feature flag FEATURE_WCDMA_GPS_CELL_ID_READING.
04/20/07   da      Made changes needed to update ASN.1 version to R6 sept R6 2006
03/02/06   ps      Added support for Wb-AMR under the feature flag
                   FEATURE_VOC_AMR_WB.
02/02/07   da      Removed code that use to handle DCH->FACH activation time
                   and 300ms delay.  functionality has been moved to LLC. 
12/22/06  ps       Made changes to handle postVerificationPeriod and the
                           timingMaintainedSynchInd IE received in OTA messages
09/12/06   sm/da   Fix code review comments and general clean-up for REL6 and HSUPA
09/11/06   da      Added support for FEATURE RLC ONE SIDE RE_ESTABLISHMENT
09/05/06   da      Added new func rrcpcrc_get_new_crnti_valid().  This function
                   will be called when CU wants to know if the OTA msg
                   included a new CRNTI.  Also made change not to reset 
                   new_crnti_valid flag when proc is waiting for CU CNF and 
                   OOS has been detected.  This is because proc needs to send
                   a RNTI update req.
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/28/06   da      Added support for OOS area handling during transition from DCH.
06/13/06   segu    Added support for DSAC feature implementation
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE REL5 while retaining the HSDPA specifc processing 
                   under FEATURE HSDPA.
04/25/06   da      Added checking the status of RNTI_UPDATE_CNF.  If failed, 
                   send failure to PCRC msg and clear procedure.
03/15/06   tkk     Lint warnings/errors cleanup
03/10/06   da      Added Initial support for PDCP under FEATURE_UMTS_PDCP
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1
02/15/06   sgk     Added code to process channel config cnf with config
                   cancel and to handle the cases where cu is in pending
                   config state.
01/11/06   vr      For Rel 5 message,extract the CN info from the Rel-5
                   IE of the message instead of the taking it from R99 IE
12/22/05   tkk     Added support to handle CU (for certain causes) while 
                   reconfig procedure is waiting for L2 ack to go to 
                   Cell_PCH/URA_PCH from Cell_FACH.
12/19/05   da      For FACH->CELL_PCH case, UE shall not do cell update if directed cell
                   info matches with current camped cell.  Else, CU is required.
12/01/05   da      If activation time for DCH-FACH transition is greater than 300 ms,
                   make delay before cell selection as long as activation time.
11/10/05   da      Removed extern definition of nv_rel_ind;
11/10/05   da      Checking global flag rrc_nv_rel_indicator before doing R5 related
                   processing.
10/26/05   vr      Send UL-Counter Synch info in the PCReconfigComplete message
                   only if TM ciphering is still active.
09/26/05   ss      added support for mac_d_hfn in reconfig messages.
07/25/05   ss      added explicit registration for cell udpate confirm during l2nack flr
06/15/05   da      Sending rb failure msg with cause "cell update occured" when OOS area
                   occurs while RB proc is waiting for l2 ack.  
                   Also passing indication to func rrcsend_send_ul_sdu to not send L2 ack 
                   to procedure when procedure is not waiting for l2 ack.  
                   Added fix for GCF where UE was sending failure message [due to later_than_r3
                   msg chosen] with wrong transaction ID.
06/06/05   da      Calling func rrcllc_print_trans_chnl_rate() after sending complete
                   msg to print trans chan rates.
05/16/05   da      Added support to handle validation failure from L1
04/15/05   da      Added support for Phase 2 of dch failure handling: going back
                   to old configuration.
03/25/05   da      Removed FEATURE CM SUPPORTED from rrcpcrc_process_channel_config_cnf.
                   Had an invalid check that looks if failure was to due to compressed mode and send
                   pc reconfig failure msg with cause invalid config.
03/11/05   vk      Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE HSDPA
01/04/05   da      Added Support for phase 1 of Physical channel establishment 
                   failure handling within DCH. 
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
08/30/04   sgk     Removed param rrc_rnti_update_cnf_type *ptr,  a pointer to
                   rnti_update cnf cmd in function rrcpcrc_process_rnti_update_cnf
                   corrected it at the calling location, to fix lint error ptr 
                   not referenced.
                   Removed the return after ERR_FATAL in functions
                   rrcpcrc_append_start_list_to_pcrc_complete,
                   send_pcrc_failure_message, send_pcrc_complete_message,
                   rrcpcrc_dch_compressed_mode_handler, to fix lint warnings
                   'unreachable'.
                   Checked the return value of 'rrcllc_reset_ordered_config' if
                   != OC_NOT_SET and printed a MSG_HIGH to fix lint error Ignoring
                   return value of function.
08/20/04   jh(kc)  Added call to rrcrb_update_amr_cctrch_id_for_reconfig().
08/09/04   kc      Complete support for RB-Re-est of TM-bearers when transitioning from
                   non-DCH state to DCH state.
08/13/04   vn      Process State Change to Idle Disconnected in all substates.
07/21/04   sk      Changed some msg_med to msg_high to get more debug info from field.
07/19/04   sk      Set cnf reqd to TRUE while sending RNTI update req to update RNTI
                   during CELL_FACH->CELL_PCH transition.
07/14/04   sk      RL failure/RLC reset handling
                   Handle state change while waiting for l2ack.
                   Removed the local substate enum type. modified the code to use
                   the rrbrb_substate_e_type declared in rrcrbcommon.h
                   Added new API to get current substate.
05/18/04   sk      Added a check for the state indicated by the message in 
                   state handling in process_channel_config_cnf()
05/04/04   sk      Added Freq/Scr handling in reconfiguration for CELL_FACH->CELL_FACH
                   transition.
                   Changed rrcrb_send_rnti_update_req() input parms.
03/04/04   ttl     Added FEATURE_CELLPCH_URAPCH for code of Cell_PCH/URA_PCH.
02/03/04   sk      clear the ordered_config variable immediately after sending response
                   message, when SRNS relocation is not involved. 
                   Handle l2ack in rrcpcrc substates otherthan waiting_for_l2_ack substate.
                   SRNS relocation support in pcreconfig complete message triggered by 
                   CU proc
12/10/03   bu      Removed the calls to clear_ordered_config(). It will be called
                   only when L2 Ack is received. Also removed handling of NEW_CELL_IND
                   as it will never be received.
11/18/03   sk      Go to idle if L2ack for pcrc complete message fails and 
                   the procedure involves SRNS relocation. Else do not take
                   any action on L2ack failure.
11/17/03   kc      Added SRNS support in RB-PCReconfig procedure.
08/04/03   vn      Initialize new fields for RLC re-establishment in RNTI_UPDATE_REQ.
04/01/03   sk      Include TransactionId in the bit_mask in
                   PhysicalChannelReconfiguration failure message.
                   Do not include TransactionId in the bit_mask if the failure
                   is due to runtime error due to overlapped compressed mode
                   configuration.
03/26/03   sk      Added functionality to process Reconfig message going from
                   CELL_DCH to CELL_FACH, that comes with a new C-RNTI. If
                   camped on the same cell as directed and if C-RNTI is
                   available, do not initiate Cell Update procedure. 
                   Added  support for CELL_DCH -> CELL_FACH transition
                   Added  RRCPCRC_WAIT_FOR_CELL_SELECTION_CNF substate
                   Added rrcpcrc_other_wfcscnf_substate_event_handler()
                   Added rrcpcrc_fach_wfcscnf_substate_event_handler()
                   Added RRCPCRC_WAIT_FOR_CELL_SELECTION_CNF case to 
                   rrcpcrc_cell_fach_event_handler()
                   No parameters for rrcpcrc_send_channel_config_req()
03/18/03   sk      Modified the transaction Id checking in process_pcrc_message()
                   Changed  rrcrb_msg_failure_cause_type to rrcpc_msg_failure_cause_type
                   in  rrcpcrc_int_data_type and changed the enum values 
                   accordingly.
02/19/03   vn      Added processing for State Change to Idle when waiting
                   for Cell Update Confirm.
02/05/03   vn      Merged changes dropped during linting.
           vn      Added new parameter proc_id in rrcllc_set_ordered_config.
02/04/03   ram     Checked in lint changes, updated copyright.
10/24/02   rj      Updated to use rrc_malloc and rrc_free to use dual heaps 
                   in place of mem_malloc and mem_free to avoid memory
                   fragmentation
10/03/02   vk      Processed RRC_COMPRESSED_MODE_IND command in various substates 
                   instead of CPHY_RRC_COMPRESSED_MODE_IND
09/26/02   xfg     Changed for WVS to MVS migration. The change was made under
                   FEATURE_MVS
09/25/02   bu      Changed l1_status to chan_cfg_status under
                   FEATURE CM SUPPORTED.
09/20/02   bu      If reselection is in progress or if cell update has started
                   and register for CU CNF accordingly. On receiving cell update
                   confirm send Physical Channel Reconfig failure to UTRAN.
                   If C_RNTI is not valid register for CU CNF. On receiving CU CNF
                   send the Physical Channel Reconfig Complete. Call
                   rcllc_get_ordered_config_status() instead of
                   rrcllc_is_ordered_config_set().
08/28/02   vk      Processed Compressed Mode failure from LLC and send a
                   Physical Channel Reconfiguration Failure message to UTRAN
                   with error as Invalid Configuration. Implemented
                   rrcpcrc_dch_compressed_mode_handler(..) to send
                   physical channel reconfiguration failure message to UTRAN
                   and to post an internal command to measurement procedure
                   to indicate the deleted measurement identity. Changes have
                   been put under FEATURE CM SUPPORTED
08/15/02   upn     Fixed CELL_FACH unsupported config flag 
06/25/02   upn     Code implemented for state CELL_FACH support.
06/06/02   upen    Added definition for rrcpcrc_build_pcrc_complete_message
04/09/02   upen    Added June conversions and CELL_DCH functionality.
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
#include "rrcpcreconfig.h"
#include "rrcscmgr.h"
#include "rrcsend.h"
#include "rrccspi.h"
#include "rrccsp.h"
#include "rrctmr.h"
#include "rrcsibproc.h"
#include "rrcsmc.h"

#ifdef FEATURE_MVS
#include "mvs.h"
#endif  /* FEATURE_MVS */
#include "rrcpcreconfig.h"
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


typedef struct
{
  rrcpc_msg_status_e_type failure_status; /* Failure cause                 */
  rrc_ProtocolErrorCause  prot_err_cause; /* Protocol Error cause          */
}rrcpc_msg_failure_cause_type;


#define RRCPC_INVALID_TRANSACTION_ID  10

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
/* We need to introduce a delay before transmitting
on a new configuration if the base-station cannot have
the old link and new link up at the same time. */
#define RRCPCRC_TEMP_TIMER_SIG 0x4001
rex_sigs_type rrcpcrc_sig;
rex_timer_type rrcpcrc_temp_timer;

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* Structure to store all internal data related to PCRC procedure           */
typedef struct
{
  rrcrb_substate_e_type   curr_substate; /* Store the current substate     */
  rrcrb_transaction_id_type tr_ids;        /* Transaction ids for RR msg    */
  rrc_state_e_type          state_indicated_by_msg;/* RRC State given by RR msg     */
  rrcpc_msg_failure_cause_type status;     /* Status of message validation   */
  boolean                   new_crnti_valid; /* Indicates if a valid C-RNTI was recd 
                                            * in the message transitioning from CELL_DCH
                                            * to CELL_FACH */
  uint16                    new_crnti;
  boolean                   directed_cell_info; /* Indicates if a valid C-RNTI was recd */
 
  rrc_int_u_rnti_type          new_urnti;
  boolean                      new_urnti_valid;          /* Indicates if a valid U-RNTI was recd */ 
  boolean                      pcrc_for_srns_relocation;  /* Indicates SRNS reloc involved with 
                                                            this msg                             */
 
  boolean                       hho_ciph_status;
  boolean                       cn_info_saved;           /*flag to indicate that CN info is saved */
  mm_cmd_type *                 mm_cmd_ptr;              /*pointer to hold mm_cmd with cn info */

  boolean                       trans_to_dch_ciph_config_needed;
  /* Indicates if Fre-redirection is to be triggered in FACH->PCH case */
  boolean initiate_cell_selection_trans_from_fach;

  boolean high_mobility_ind;

}rrcpcrc_int_data_type;

LOCAL rrcpcrc_int_data_type pcrc;

boolean rrcpcrc_send_initiate_cu_flag = FALSE;
boolean rrcpcrc_delete_crnti = FALSE;

#ifdef FEATURE_QSH_EVENT_METRIC
extern rrc_qsh_ul_failure_ota_param_s failure_ota;
extern rrc_qsh_metric_db_type rrc_qsh_metric_db[WRRC_QSH_METRIC_MAX+1];
extern boolean rrc_qsh_dl_committed;
#endif


/* =======================================================================
**                            Function Definitions
** ======================================================================= */
/*===========================================================================

FUNCTION   RRCPCRC_IS_SRNS_IN_PROGRESS()

DESCRIPTION
 This function returns if SRNS is in progress with current reconfig procedure


        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcpcrc_is_srns_in_progress
(
  void
)
{
  return pcrc.pcrc_for_srns_relocation;
}
/*===========================================================================

FUNCTION   RRCPCRC_GET_NEW_CRNTI_VALID()

DESCRIPTION
 This function returns if reconfig msg has valid CRNTI


        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcpcrc_get_new_crnti_valid
(
  void
)
{
  return pcrc.new_crnti_valid;
}
/*===========================================================================

FUNCTION   RRCPCRC_GET_CURRENT_SUB_STATE()

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
rrcrb_substate_e_type rrcpcrc_get_current_sub_state( )
{
  return pcrc.curr_substate;
}
 /*===========================================================================

FUNCTION   rrcpcrc_get_dest_state()

DESCRIPTION
 This function is used to return destination state
 indicated in OTA.
DEPENDENCIES

  None.

RETURN VALUE

  rrc_state_e_type.

SIDE EFFECTS

  None.

===========================================================================*/
rrc_state_e_type rrcpcrc_get_dest_state( )
{
  return pcrc.state_indicated_by_msg;
}
/*===========================================================================

FUNCTION   rrcpcrc_is_proc_successful()

DESCRIPTION
 This function is used to return failure status

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean rrcpcrc_is_proc_successful( )
{
  if(pcrc.status.failure_status == RRCPC_MSG_VALID)
    return TRUE;
  else return FALSE;
}

/*===========================================================================

FUNCTION   rrcpcrc_update_failure_status()

DESCRIPTION
  Updates the failure status
DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
void rrcpcrc_update_failure_status(rrcpc_msg_status_e_type status )
{
  pcrc.status.failure_status = status;
  WRRC_MSG1_HIGH("Updated Failure status to %d",status);
  return;
}

/*===========================================================================

FUNCTION rrcpcrc_append_start_list_to_pcrc_complete

DESCRIPTION
  This function appends start list to pcrc message for SRNS relocation and
  also when TM bearers are present during non-dch to dch rrc-state
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
static void rrcpcrc_append_start_list_to_pcrc_complete
(
rrc_UL_DCCH_Message *msg_ptr               /* Pointer to Uplink DCCH Msg */
)
{
  /*start list is included in ul_counter_synchronization_info. So enable this IE in the
  bitmask */
  RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.physicalChannelReconfigurationComplete,
    rrc_PhysicalChannelReconfigurationComplete,ul_CounterSynchronisationInfo); 
  RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.physicalChannelReconfigurationComplete.ul_CounterSynchronisationInfo);

 
  /* call SMC API to append the start list */
  (void)rrcsmc_append_start_list(&msg_ptr->message.u.physicalChannelReconfigurationComplete.
                                          ul_CounterSynchronisationInfo.startList,
                                          RRCSMC_EXTRACT_AND_APPEND_START);

}

/*===========================================================================

FUNCTION rrcpcrc_is_pcpich_info_present

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
static boolean  rrcpcrc_is_pcpich_info_present
(
  rrc_PhysicalChannelReconfiguration *msg_ptr,
  rrc_msg_ver_enum_type msg_version
)
{
  boolean result = FALSE;
  uint16  rrcpcrc_psc = 512;
  if (MSG_REL5 == msg_version)
  {
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.
        physicalChannelReconfiguration_r5,
        rrc_PhysicalChannelReconfiguration_r5_IEs,dl_InformationPerRL_List))
    {
      if(RRC_CHECK_COMMON_MSG_TYPE(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.
        physicalChannelReconfiguration_r5.dl_InformationPerRL_List.elem[0].modeSpecificInfo,
        rrc_DL_InformationPerRL_r5_modeSpecificInfo_fdd))
      { 
        rrcpcrc_psc = 
                 msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.
                 physicalChannelReconfiguration_r5.dl_InformationPerRL_List.
                 elem[0].modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
        result = TRUE;
      }
    }
  }
  else
  if (MSG_REL6 == msg_version)
  {
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
        criticalExtensions.u.r6.physicalChannelReconfiguration_r6,
        rrc_PhysicalChannelReconfiguration_r6_IEs,dl_InformationPerRL_List))
    {
      if(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.
       physicalChannelReconfiguration_r6.dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
       T_rrc_DL_InformationPerRL_r6_modeSpecificInfo_fdd)
      { 
        rrcpcrc_psc = 
                 msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.
                 physicalChannelReconfiguration_r6.dl_InformationPerRL_List.elem[0].modeSpecificInfo.
                 u.fdd->primaryCPICH_Info.primaryScramblingCode;
        result = TRUE;
      }
    }
  }
  else
  if (MSG_REL7 == msg_version)
  {
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
        criticalExtensions.u.r7.physicalChannelReconfiguration_r7,
        rrc_PhysicalChannelReconfiguration_r7_IEs,dl_InformationPerRL_List))
    {
      if(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7.
       physicalChannelReconfiguration_r7.dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
       T_rrc_DL_InformationPerRL_r7_modeSpecificInfo_fdd)
      { 
        rrcpcrc_psc = 
                 msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7.
                 physicalChannelReconfiguration_r7.dl_InformationPerRL_List.elem[0].modeSpecificInfo.
                 u.fdd->primaryCPICH_Info.primaryScramblingCode;
        result = TRUE;
      }
    }
  }
  else
  if (MSG_REL8 == msg_version)
  {
    rrc_PhysicalChannelReconfiguration_r8_IEs *pcreconfig_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.
      criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.physicalChannelReconfiguration_r8;

    if (RRCRB_R8_MSG_IE_PRESENT(pcreconfig_ptr,rrc_PhysicalChannelReconfiguration_r8_IEs,dl_InformationPerRL_List))
    {
      if(pcreconfig_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
        T_rrc_DL_InformationPerRL_r8_modeSpecificInfo_fdd)
      { 
        rrcpcrc_psc = 
                pcreconfig_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.
                u.fdd->primaryCPICH_Info.primaryScramblingCode;
        result = TRUE;
      }
    }
  }
  else
#ifdef FEATURE_WCDMA_REL9

  if (MSG_REL9 == msg_version)
  {
    rrc_PhysicalChannelReconfiguration_r9_IEs *pcreconfig_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.
      criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.physicalChannelReconfiguration_r9;

    if (RRCRB_R9_MSG_IE_PRESENT(pcreconfig_ptr,dl_InformationPerRL_List))
    {
      if(pcreconfig_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
        T_rrc_DL_InformationPerRL_r8_modeSpecificInfo_fdd)
      { 
        rrcpcrc_psc = 
                pcreconfig_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.
                u.fdd->primaryCPICH_Info.primaryScramblingCode;

        result = TRUE;
      }
    }
  }
  else
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif /* FEATURE_WCDMA_REL10 */

  /*this function is used to find whether directed cell info is present or not during Cell_DCH to Cell_FACH 
  state transition. It is safe to assume that the rl list will have only one element.*/
  if ((MSG_REL99 == msg_version) &&
      (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.physicalChannelReconfiguration_r3,
       rrc_PhysicalChannelReconfiguration_r3_IEs,dl_InformationPerRL_List)))
  {
    if(RRC_CHECK_COMMON_MSG_TYPE(msg_ptr->u.r3.physicalChannelReconfiguration_r3.
       dl_InformationPerRL_List.elem[0].modeSpecificInfo,
      rrc_DL_InformationPerRL_modeSpecificInfo_fdd))
    { 
      rrcpcrc_psc = 
               msg_ptr->u.r3.physicalChannelReconfiguration_r3.dl_InformationPerRL_List.
               elem[0].modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
        result = TRUE;
    }
  }
  MSG_HIGH("Primary SCR in PCRC message %d result:%d for MSG_VERSION: %d(0-MSG_REL99,1-MSG_REL5,2-MSG_REL6,3-MSG_REL7,4-MSG_REL8,5-MSG_REL9,6-MSG_REL10,other-MSG_INVALID)",rrcpcrc_psc,result,msg_version);
  return result;
}  /* rrcpcrc_is_pcpich_info_present */


/*===========================================================================

FUNCTION   SEND_PCRC_FAILURE_MSG

DESCRIPTION

  This function sends the Physical Channel Reconfiguration Failure message
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
static void send_pcrc_failure_message
(
  rrcpc_msg_status_e_type failure_cause,  /* Failure Cause - should be a subset
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
   WRRC_MSG2_HIGH("pcrc subsstate(%d) Send failure with %d cause:   [0-VALID|1-UNSUPPORTED_CONFIG|2-PHY_CHAN_FAILURE|3-SIMULTANEOUS_RECONFIG|4-PROTOCOL_ERROR|5-INVALID_CONFIG|6-INCOMPLETE_CONFIG|7-CELL_UPDATE_OCCURED]", 
   pcrc.curr_substate, pcrc.status.failure_status);

  /* we need to revert cipher config db, if cipher config db has been updated with step-1 HO. 
     This revert takes care of the scenario of OTA failure in the gap between OC is 
     set and cphy is sent. This cannot happen in a normal case. This is for extra protection */
  rrcsmc_revert_to_old_config_for_failure_message(FALSE);

  RRC_RESET_MSG_IE_PRESENT_PTR(ptr); /* Integrity Check info not present */
  ptr->message.t = T_rrc_UL_DCCH_MessageType_physicalChannelReconfigurationFailure; /* RR Failure msg */
  
  RRC_RESET_MSG_IE_PRESENT(ptr->message.u.physicalChannelReconfigurationFailure);
  /* Unlike other RBControl failure messages the transactionId in PhChFailure
     is optional */
  RRC_MSG_COMMON_SET_BITMASK_IE(ptr->message.u.physicalChannelReconfigurationFailure,
        rrc_PhysicalChannelReconfigurationFailure,rrc_TransactionIdentifier);
  
  /* Transaction Id - First check the rejected_transacion id. If
  it's not invalid, this message is being rejected because there
  is already another message accepted but not processed.
  If the rejected_transaction id is invalid, we are rejecting the
  accepted message, so use the accepted transaction id.

  NOTE: Don't change the order, it's important to check rejected
  transaction first. */
  if( pcrc.tr_ids.rejected_transaction != RRCPC_INVALID_TRANSACTION_ID )
  {
    
    ptr->message.u.physicalChannelReconfigurationFailure.rrc_TransactionIdentifier =
      pcrc.tr_ids.rejected_transaction;
    /* We no longer need to keep this id. Make rejected_transaction id
    invalid again */
    pcrc.tr_ids.rejected_transaction = RRCPC_INVALID_TRANSACTION_ID;
  }
  else if( pcrc.tr_ids.accepted_transaction != RRCPC_INVALID_TRANSACTION_ID )
  {
    ptr->message.u.physicalChannelReconfigurationFailure.rrc_TransactionIdentifier =
      pcrc.tr_ids.accepted_transaction;
    /* We no longer need to keep this id. Make rejected_transaction id
    invalid again */
    pcrc.tr_ids.accepted_transaction = RRCPC_INVALID_TRANSACTION_ID;
  }
  else
  {
    /* Don't know the transaction id for this message.( one reason
    for this could be that the message had a critical
    extenstion). Since it's unknown, we should set it to zero. */
    ptr->message.u.physicalChannelReconfigurationFailure.rrc_TransactionIdentifier= 0;
    WRRC_MSG0_HIGH("Transaction id invalid for PCRC Reconfig Failure message!");
  }

  switch( failure_cause )
  {
    case RRCPC_MSG_UNSUPPORTED_CONFIG:
      ptr->message.u.physicalChannelReconfigurationFailure.failureCause.t =
        T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      break;

    case RRCPC_PHY_CHAN_FAILURE:
      ptr->message.u.physicalChannelReconfigurationFailure.failureCause.t =
        T_rrc_FailureCauseWithProtErr_physicalChannelFailure;
      break;

    case RRCPC_SIMULTANEOUS_RECONFIG:
      ptr->message.u.physicalChannelReconfigurationFailure.failureCause.t =
        T_rrc_FailureCauseWithProtErr_incompatibleSimultaneousReconfiguration;
      break;

    case RRCPC_MSG_PROTOCOL_ERROR:
      ptr->message.u.physicalChannelReconfigurationFailure.failureCause.t =
        T_rrc_FailureCauseWithProtErr_protocolError;

  	  ptr->message.u.physicalChannelReconfigurationFailure.failureCause.u.protocolError =
  	  	rtxMemAllocTypeZ(&enc_ctxt, rrc_ProtocolErrorInformation);
      
      
      /* Indicate the protocol error cause */
      ptr->message.u.physicalChannelReconfigurationFailure.failureCause.u.protocolError->diagnosticsType.t =
          T_rrc_ProtocolErrorInformation_diagnosticsType_type1;
     
  	  ptr->message.u.physicalChannelReconfigurationFailure.failureCause.u.protocolError->
  		    diagnosticsType.u.type1 = rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation_type1);

      /* Copy the given protocol error cause */
      ptr->message.u.physicalChannelReconfigurationFailure.failureCause.u.protocolError->
        diagnosticsType.u.type1->protocolErrorCause = prot_err_cause;
      break;

    case RRCPC_MSG_INVALID_CONFIG:
      ptr->message.u.physicalChannelReconfigurationFailure.failureCause.t =
        T_rrc_FailureCauseWithProtErr_invalidConfiguration;
      break;

    case RRCPC_MSG_INCOMPLETE_CONFIG:
      ptr->message.u.physicalChannelReconfigurationFailure.failureCause.t =
        T_rrc_FailureCauseWithProtErr_configurationIncomplete;
      break;

    case RRCPC_CELL_UPDATE_OCCURED:
      ptr->message.u.physicalChannelReconfigurationFailure.failureCause.t =
        T_rrc_FailureCauseWithProtErr_cellUpdateOccurred;
    break;

    default:
      WRRC_MSG0_HIGH("Sending PCRC Unknown Failure message");
      ptr->message.u.physicalChannelReconfigurationFailure.failureCause.t =
        T_rrc_FailureCauseWithProtErr_configurationUnsupported;
      break;
  }
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
    failure_ota.ul_ota_type = RRC_OTA_UL_FAILURE;
    failure_ota.msg_id = WRRC_UL_DCCH_MESSAGETYPE_PHYSICALCHANNELRECONFIGURATIONFAILURE;
    if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status())
    {
      failure_ota.ul_fail_ota.ul_ota_parms.ul_integrity_info_present = 1;
    }
    failure_ota.ul_fail_ota.ul_ota_parms.failure_cause = ptr->message.u.physicalChannelReconfigurationFailure.failureCause.t;
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
      ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.ul_ota_parms.failure_cause= ptr->message.u.physicalChannelReconfigurationFailure.failureCause.t;   
      /* Commit data will be called in send ul sdu */
    }
  }
#endif
  /*log the protocol error*/
  rrc_log_protocol_error( (uint8) (rrc_get_state()),
                          (uint8) RRC_PROCEDURE_PCR,
                          (uint8) ptr->message.u.physicalChannelReconfigurationFailure.failureCause.t,
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
    WRRC_MSG1_HIGH("Sending PCRC Failure msg to send-chain Cause:%d",
             ptr->message.u.physicalChannelReconfigurationFailure.failureCause.t);
    /* Need to wait for L2 ack for all states because RRC needs to trigger include reconfig status indicator
    if RLC acknowledgement is not yet received.
    As per 8.3.1.3
    1> if the IE "Cell update cause" is set to "radio link failure" and the UE has submitted a reconfiguration response
    message to RLC and the UE has not received the RLC acknowledgement for the response message:
    2> include the IE "Reconfiguration Status Indicator" and set it to TRUE.*/
    if( pcrc.status.failure_status == RRCPC_PHY_CHAN_FAILURE )
    {
      rrc_update_reconfig_waiting_for_l2ack_dch(RRC_PROCEDURE_PCR);
      status = rrcsend_send_ul_sdu(RRC_PROCEDURE_PCR,  /* Procedure is PCR    */
                                   (void *)ptr,        /* Message pointer     */
                                   ul_lc_id,           /* RLC id              */
                                   RRCSEND_L2ACK_REQUIRED  /* ACK is required */
                                  );
    }
    else
    {
       status = rrcsend_send_ul_sdu(RRC_PROCEDURE_PCR,  /* Procedure is PCR    */
                                 (void *)ptr,        /* Message pointer     */
                                 ul_lc_id,           /* RLC id              */
                                   RRCSEND_L2ACK_NOT_REQUIRED  /* No need for ACK */
                                );
    }
    if( status != RRCSEND_SUCCESS )
    {
      WRRC_MSG1_ERROR("Send chain failed for PCRC failure msg: error cause %d", status);
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
}/* end function send_pcrc_failure_message */



/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */


/*===========================================================================

FUNCTION   SEND_PCRC_COMPLETE_MSG

DESCRIPTION

  This function sends the Physical Channel Reconfiguration Complete message
  on Uplink DCCH. 
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/     
static void send_pcrc_complete_message( boolean proc_wait_for_l2_ack)
{
  
  rrc_UL_DCCH_Message *ptr;     /* Pointer to uplink DCCH message type */
  rlc_lc_id_type ul_lc_id;      /* Logical Channel Id for the message  */ 
  rrcsend_status_e_type status; /* Status from send chain module       */
  rrcllc_oc_set_status_e_type oc_status = rrcllc_get_ordered_config_status();

  /* Allocate memory for this message. Note that this buffer is released
  by the SEND CHAIN */
  ptr = (rrc_UL_DCCH_Message *)rrc_malloc( 
                                          sizeof(rrc_UL_DCCH_Message));
  if (proc_wait_for_l2_ack)
  {
     WRRC_MSG0_HIGH("Waiting for L2 ACK for PCRC Complete msg");
  }
  
  RRC_RESET_MSG_IE_PRESENT_PTR(ptr); /* Integrity Check info not present */
  /* PCRC Complete msg */
  ptr->message.t = T_rrc_UL_DCCH_MessageType_physicalChannelReconfigurationComplete; 

  /* Fill up the message contents. The bit_mask is set to 0 since none
  of the optional IE's are included for now. */
  RRC_RESET_MSG_IE_PRESENT(ptr->message.u.physicalChannelReconfigurationComplete);

  if (pcrc.pcrc_for_srns_relocation == TRUE)
  {
    rrcpcrc_append_start_list_to_pcrc_complete(ptr);
  }
  else
  {
    if (rrcllc_get_rlc_size_change_status() 
        && ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH || 
               pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
            || (rrc_get_rlc_size_change_status_for_procedure_id(RRC_PROCEDURE_PCR) == TRUE)))
    {
      // Check if next state is CELL+PCH, and OC API returns RLC size change, then include counter sync
      if (pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH || 
          pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
      {
        /* Ask SMC to do start value calculation for PS domain */
        WRRC_MSG0_HIGH("RLCSIZEINDICATED Invoking SMC extract func");
        (void)rrcsmc_extract_return_hfn_per_domain_and_update_start_flag(RRC_PS_DOMAIN_CN_ID);
                 
      }
      else if (rrc_get_rlc_size_change_status_for_procedure_id(RRC_PROCEDURE_PCR) == TRUE)
      {
        rrc_set_rlc_size_change_status(RRC_PROCEDURE_NONE, 
                                       FALSE, RRC_INVALID_RB_ID ,
                                       RLC_RE_ESTABLISH_NONE);
      }
        /*start list is included in ul_counter_synchronization_info. So enable this IE in the
        bitmask */
      RRC_MSG_COMMON_SET_BITMASK_IE(ptr->message.u.physicalChannelReconfigurationComplete,
        rrc_PhysicalChannelReconfigurationComplete,ul_CounterSynchronisationInfo); 
      
      RRC_RESET_MSG_IE_PRESENT(ptr->message.u.
      physicalChannelReconfigurationComplete.ul_CounterSynchronisationInfo);
     
      /* call SMC API to append the start list */
      (void)rrcsmc_append_start_list(
        &ptr->message.u.physicalChannelReconfigurationComplete.ul_CounterSynchronisationInfo.startList,
        RRCSMC_APPEND_START);
    }
  } 

  //if this is case of IFHHO and not SRNS relocation(i.e. ul counter sync is not present)
  //then also send START_List and set count c act time and configure smc for Step_2

  if ( ((pcrc.hho_ciph_status == TRUE) || (pcrc.trans_to_dch_ciph_config_needed == TRUE)) &&
       (TRUE == rrcsmc_tm_ciph_active()))
  {
    if(! (RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.physicalChannelReconfigurationComplete,
           rrc_PhysicalChannelReconfigurationComplete,ul_CounterSynchronisationInfo)))
    {
      rrcpcrc_append_start_list_to_pcrc_complete(ptr);
    }
  
    //Set the bitmask for count_c_act_time and copy the same..
    RRC_MSG_COMMON_SET_BITMASK_IE(ptr->message.u.physicalChannelReconfigurationComplete,
      rrc_PhysicalChannelReconfigurationComplete,count_C_ActivationTime);
    
    ptr->message.u.physicalChannelReconfigurationComplete.count_C_ActivationTime =
      rrcsmc_step_2_cipher_config_for_hho();
  
    //Send step 2 config for HHO.. only one of these flags will be active, reset both of them
    //here
    if(pcrc.hho_ciph_status)
    {
      pcrc.hho_ciph_status =  FALSE;      
    }
    else if(pcrc.trans_to_dch_ciph_config_needed)
    {
      pcrc.trans_to_dch_ciph_config_needed = FALSE;      
    }
  }

  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
  {
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
    if(rrcsib_is_dmcr_ie_needed() == TRUE)
    {
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.physicalChannelReconfigurationComplete, laterNonCriticalExtensions);
 
      RRC_RESET_MSG_IE_PRESENT(ptr->message.u.physicalChannelReconfigurationComplete.laterNonCriticalExtensions);
 
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.physicalChannelReconfigurationComplete.laterNonCriticalExtensions, v770NonCriticalExtensions);
 
      RRC_RESET_MSG_IE_PRESENT(ptr->message.u.physicalChannelReconfigurationComplete.laterNonCriticalExtensions.v770NonCriticalExtensions);
      
      RRC_RESET_MSG_IE_PRESENT(ptr->message.u.physicalChannelReconfigurationComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        physicalChannelReconfigurationComplete_v770ext);
 
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.physicalChannelReconfigurationComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        physicalChannelReconfigurationComplete_v770ext, deferredMeasurementControlReading);
 
      ptr->message.u.physicalChannelReconfigurationComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        physicalChannelReconfigurationComplete_v770ext.deferredMeasurementControlReading = rrc_PhysicalChannelReconfigurationComplete_v770ext_IEs_deferredMeasurementControlReading_true;
    }
#endif
  }


  /* PCRC Complete msg */
  ptr->message.t = T_rrc_UL_DCCH_MessageType_physicalChannelReconfigurationComplete; 

  /* Transaction Id - use the accepted transaction id if known. Else
  use zero. Since this is a PCRC Complete message, we don't check 
  rejected transaction ids. */
  if( pcrc.tr_ids.accepted_transaction != RRCPC_INVALID_TRANSACTION_ID )
  {
    ptr->message.u.physicalChannelReconfigurationComplete.rrc_TransactionIdentifier=
      pcrc.tr_ids.accepted_transaction;
    /* We no longer need to keep this id. Make rejected_transaction id
    invalid again unless next state is RRCRB_WAIT_FOR_L2_ACK */
    if (!proc_wait_for_l2_ack)
    {
      pcrc.tr_ids.accepted_transaction = RRCPC_INVALID_TRANSACTION_ID;
    }
  }
  else
  {
    /* Don't know the transaction id for this message.( one reason
    for this could be that the message had a critical
    extenstion). Since it's unknown, we should set it to zero. */
    ptr->message.u.physicalChannelReconfigurationComplete.rrc_TransactionIdentifier = 0;
    WRRC_MSG0_HIGH("Transaction id invalid for PCRC Complete message!");
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
    (void) rex_clr_sigs(rex_self(), RRCPCRC_TEMP_TIMER_SIG);
    (void) rex_set_timer(&rrcpcrc_temp_timer, 5000);   /* 5 seconds */

    rrcpcrc_sig = rex_wait(RRCPCRC_TEMP_TIMER_SIG);
    MSG_LOW("RRCPCRC signal:%d", rrcpcrc_sig, 0, 0);
    (void) rex_clr_sigs(rex_self(), RRCPCRC_TEMP_TIMER_SIG);
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
      failure_ota.msg_id = WRRC_UL_DCCH_MESSAGETYPE_PHYSICALCHANNELRECONFIGURATIONCOMPLETE;
      if(RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.physicalChannelReconfigurationComplete,
        rrc_PhysicalChannelReconfigurationComplete,count_C_ActivationTime))
      {
        failure_ota.ul_fail_ota.ul_ota_parms.count_c_activation= ptr->message.u.physicalChannelReconfigurationComplete.count_C_ActivationTime;
      }
      if (RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.physicalChannelReconfigurationComplete,
      rrc_PhysicalChannelReconfigurationComplete,ul_CounterSynchronisationInfo))
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
      if(RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.physicalChannelReconfigurationComplete,
        rrc_PhysicalChannelReconfigurationComplete,count_C_ActivationTime))
      {
        ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.ul_ota_parms.count_c_activation= ptr->message.u.physicalChannelReconfigurationComplete.count_C_ActivationTime;
      }
      if (RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.physicalChannelReconfigurationComplete,
      rrc_PhysicalChannelReconfigurationComplete,ul_CounterSynchronisationInfo))
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
    WRRC_MSG0_HIGH("Sending RPCRC Complete Message to send-chain");
    if (proc_wait_for_l2_ack) 
    {
      status = rrcsend_send_ul_sdu(RRC_PROCEDURE_PCR,  /* Procedure is PCR    */
                                   (void *)ptr,        /* Message pointer     */
                                   ul_lc_id,           /* RLC id              */
                                   RRCSEND_L2ACK_REQUIRED  /* ACK is required */
                                   );
    }
    else
    {
      status = rrcsend_send_ul_sdu(RRC_PROCEDURE_PCR,  /* Procedure is PCR    */
                                   (void *)ptr,        /* Message pointer     */
                                   ul_lc_id,           /* RLC id              */
                                   RRCSEND_L2ACK_NOT_REQUIRED  /* ACK is required */
                                   );
    }

    if( status != RRCSEND_SUCCESS )
    {
      WRRC_MSG1_ERROR("Send chain failed for PCRC complete msg: error cause %d", status);
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
}/* end function send_pcrc_complete_message */


/*===========================================================================

FUNCTION   RRCPCRC_VALIDATE_SRNS_RELOCATION_INFO()

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
static boolean rrcpcrc_validate_srns_relocation_info
(
  rrc_PhysicalChannelReconfiguration *msg_ptr,    /* Pointer to the pcReconfigMessage */
  rrc_msg_ver_enum_type msg_version
)
{
   /*check whether dl_counter_sync info is present or not . Do not check for
   the contents of DL_counter_sync_info. First the integrity check has tobe made
   with new integrity protection mode info in the message. Later we can check
   the dl_counter_sync_info contents and send a failure if PDCP info is present */
  if (msg_version == MSG_REL5)
  {
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.physicalChannelReconfiguration_r5,
      rrc_PhysicalChannelReconfiguration_r5_IEs,dl_CounterSynchronisationInfo))
    {
      return TRUE;
    } 
  }
  else
  if (msg_version == MSG_REL6)
  {
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
        criticalExtensions.u.r6.physicalChannelReconfiguration_r6,
        rrc_PhysicalChannelReconfiguration_r6_IEs,dl_CounterSynchronisationInfo))
    {
      return TRUE;
    } 
  }
  else
  if (msg_version == MSG_REL7)
  {
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
        criticalExtensions.u.r7.physicalChannelReconfiguration_r7,
        rrc_PhysicalChannelReconfiguration_r7_IEs,dl_CounterSynchronisationInfo))
    {
      return TRUE;
    } 
  }
  else
  if (msg_version == MSG_REL8)
  {
     rrc_PhysicalChannelReconfiguration_r8_IEs *pcreconfig_ptr = &msg_ptr->u.later_than_r3.
        criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.
        physicalChannelReconfiguration_r8;  

    if(RRCRB_R8_MSG_IE_PRESENT (pcreconfig_ptr,rrc_PhysicalChannelReconfiguration_r8_IEs,
      dl_CounterSynchronisationInfo))
    {
      return TRUE;
    } 
  }
  else
#ifdef FEATURE_WCDMA_REL9
  if (msg_version == MSG_REL9)
  {
     rrc_PhysicalChannelReconfiguration_r9_IEs *pcreconfig_ptr = &msg_ptr->u.later_than_r3.
        criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.physicalChannelReconfiguration_r9;

    if(RRCRB_R9_MSG_IE_PRESENT (pcreconfig_ptr,
      dl_CounterSynchronisationInfo))
    {
      return TRUE;
    } 
  }
  else
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
  #error code not present
#endif /* FEATURE_WCDMA_REL10 */

  if ((msg_version == MSG_REL99) && 
      (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.physicalChannelReconfiguration_r3,
       rrc_PhysicalChannelReconfiguration_r3_IEs,dl_CounterSynchronisationInfo)))
  {
     return TRUE;
  }

  return FALSE;
}




/*===========================================================================

FUNCTION   VALIDATE_PCRC_MESSAGE

DESCRIPTION

  This function validates the received PC Reconfig Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a PCRC Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_pcrc_r5
(
  rrc_PhysicalChannelReconfiguration_r5_IEs *msg_ptr /* Pointer to the PCRC message */
)
{
 
  uecomdef_status_e_type status;
    /* Assume the message is valid for now */
  pcrc.status.failure_status = RRCPC_MSG_VALID;

  /* Check the RRC State indicatior given by the message */
  switch( msg_ptr->rrc_StateIndicator )
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      pcrc.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      pcrc.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      pcrc.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;

    case rrc_RRC_StateIndicator_ura_PCH:
      pcrc.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;


    default:
      /* No other states are supported right now. */
      WRRC_MSG0_HIGH("Unsupported state in PC Reconfig Message");
      pcrc.status.failure_status = RRCPC_MSG_UNSUPPORTED_CONFIG;
      break;
  }

  if( pcrc.status.failure_status == RRCPC_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
     return;
  }

  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr,rrc_PhysicalChannelReconfiguration_r5_IEs,dl_HSPDSCH_Information))
  {
    
    /* Check if network does not try to initiate HSDPA with invalid state */
    status = rrc_hsdpa_initial_validation(msg_ptr->rrc_StateIndicator, 
      RRC_CHECK_COMMON_MSG_TYPE(msg_ptr->dl_HSPDSCH_Information.modeSpecificInfo,
      rrc_DL_HSPDSCH_Information_modeSpecificInfo_tdd));
    if (status == FAILURE)
    {
      pcrc.status.failure_status = RRCPC_MSG_INVALID_CONFIG;
      return;
    }
  }
  
   /* Check if RB with PDCP Information list is present. This
  is not supported in the first release. This may be supported
  later */                                    
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr, 
        rrc_PhysicalChannelReconfiguration_r5_IEs,dl_CounterSynchronisationInfo)) 
  {

    /*check whether the SRNS relocation is within Cell_DCH or not.
     SRNS relocation information can come in reconfiguration messages
     only when the state transition is Cell_DCH->Cell_DCH */
    if (!((pcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("PCRC : SRNS reloc with invalid states cur_state %d, msg_state %d",
             rrc_get_state(), pcrc.state_indicated_by_msg);
      pcrc.status.failure_status=RRCPC_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has 
    PDCP info list. If it is true then this configuration is unsupported */
    if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2(msg_ptr->dl_CounterSynchronisationInfo,
		rb_WithPDCP_InfoList)) ||
        (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->dl_CounterSynchronisationInfo,
         rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
    {
      WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
      pcrc.status.failure_status=RRCPC_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation. 
        Set the flag that indicates that this procedure
      involves SRNS relocation */
      pcrc.pcrc_for_srns_relocation = TRUE;
      /* store if new U-RNTI is present in the message */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr, rrc_PhysicalChannelReconfiguration_r5_IEs,new_U_RNTI))
      {
        pcrc.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(msg_ptr->new_U_RNTI),
                              &pcrc.new_urnti);
      }
    }
  }

    /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  pcrc.trans_to_dch_ciph_config_needed = FALSE;
  if((pcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr,
        rrc_PhysicalChannelReconfiguration_r5_IEs,dl_CommonInformation)) && 
       (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->dl_CommonInformation,
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
             (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->dl_CommonInformation.dl_DPCH_InfoCommon,
             rrc_DL_DPCH_InfoCommon_r4,mac_d_HFN_initial_value)),
             &msg_ptr->dl_CommonInformation.dl_DPCH_InfoCommon.mac_d_HFN_initial_value,
             &mac_d_hfn_l);

        if(loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("get_mac_d_hfn failed ");
        }
        if(rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
        {
          WRRC_MSG0_HIGH("rrcsmc_process_cipher_config_for_hho failed");
        }
        pcrc.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }

  /*Cell_PCH/URA_PCH specific validations */
  if ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
     /* If UTRAN DRX coefficient is not present then it is invalid configuration */
     if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr, 
         rrc_PhysicalChannelReconfiguration_r5_IEs,utran_DRX_CycleLengthCoeff)))
     {
       WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", pcrc.state_indicated_by_msg);
       pcrc.status.failure_status = RRCPC_MSG_INVALID_CONFIG;
       return;
     }
     if (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
     {
       /*should we do it after getting l2ack for response??*/
       if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr, rrc_PhysicalChannelReconfiguration_r5_IEs,ura_Identity)))
       {
         WRRC_MSG0_HIGH("URA ID not present");
         rrccu_clear_ura_id ();
       }
       else
       {
         /*URA ID present in the message set ura_identity variable with this value */
         if (msg_ptr->ura_Identity.numbits == 16)
         {
           rrccu_update_ura_id (&(msg_ptr->ura_Identity));
         }
         else
         {
           WRRC_MSG1_ERROR("Invalid URA ID length%d",
               msg_ptr->ura_Identity.numbits);
           pcrc.status.failure_status = RRCPC_MSG_PROTOCOL_ERROR;
           pcrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
           return;
         }

       }
           
     }
  }
}/* end function validate_pcrc_r5 */



/*===========================================================================

FUNCTION   validate_pcrc_r6

DESCRIPTION

  This function validates the received PC Reconfig Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a PCRC Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_pcrc_r6
(
  rrc_PhysicalChannelReconfiguration_r6_IEs *msg_ptr /* Pointer to the PCRC message */
)
{
 
  uecomdef_status_e_type status;
    /* Assume the message is valid for now */
  pcrc.status.failure_status = RRCPC_MSG_VALID;

  /* Check the RRC State indicatior given by the message */
  switch( msg_ptr->rrc_StateIndicator )
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      pcrc.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      pcrc.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      pcrc.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;

    case rrc_RRC_StateIndicator_ura_PCH:
      pcrc.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;


    default:
      /* No other states are supported right now. */
      WRRC_MSG0_HIGH("Unsupported state in PC Reconfig Message");
      pcrc.status.failure_status = RRCPC_MSG_UNSUPPORTED_CONFIG;
      break;
  }

  if( pcrc.status.failure_status == RRCPC_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
     return;
  }

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr, 
      rrc_PhysicalChannelReconfiguration_r6_IEs,dl_HSPDSCH_Information))
  {
    
    /* Check if network does not try to initiate HSDPA with invalid state */
    status = rrc_hsdpa_initial_validation_r6(msg_ptr->rrc_StateIndicator, 
      &msg_ptr->dl_HSPDSCH_Information);
    
    if (status == FAILURE)
    {
      pcrc.status.failure_status = RRCPC_MSG_INVALID_CONFIG;
      return;
    }
  }
  
   /* Check if RB with PDCP Information list is present. This
  is not supported in the first release. This may be supported
  later */                                    
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr, 
        rrc_PhysicalChannelReconfiguration_r6_IEs,dl_CounterSynchronisationInfo)) 
  {

    /*check whether the SRNS relocation is within Cell_DCH or not.
     SRNS relocation information can come in reconfiguration messages
     only when the state transition is Cell_DCH->Cell_DCH */
    if (!((pcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("PCRC : SRNS reloc with invalid states cur_state %d, msg_state %d",
             rrc_get_state(), pcrc.state_indicated_by_msg);
      pcrc.status.failure_status=RRCPC_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has 
    PDCP info list. If it is true then this configuration is unsupported */
    if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2(msg_ptr->dl_CounterSynchronisationInfo,
		rb_WithPDCP_InfoList)) ||
        (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->dl_CounterSynchronisationInfo,
         rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
    {
      WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
      pcrc.status.failure_status=RRCPC_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation. 
        Set the flag that indicates that this procedure
      involves SRNS relocation */
      pcrc.pcrc_for_srns_relocation = TRUE;
      /* store if new U-RNTI is present in the message */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr, rrc_PhysicalChannelReconfiguration_r6_IEs,new_U_RNTI))
      {
        pcrc.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(msg_ptr->new_U_RNTI),
                              &pcrc.new_urnti);
      }
    }
  }

    /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  pcrc.trans_to_dch_ciph_config_needed = FALSE;
  if((pcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr,
        rrc_PhysicalChannelReconfiguration_r6_IEs,dl_CommonInformation)) && 
       (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->dl_CommonInformation,rrc_DL_CommonInformation_r6,
       dl_dpchInfoCommon)) &&
       (T_rrc_DL_CommonInformation_r6_dl_dpchInfoCommon_dl_DPCH_InfoCommon == 
         msg_ptr->dl_CommonInformation.dl_dpchInfoCommon.t))
    {
      //check if ciphering is active in mac for cs domain, if it is then,
      //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      if(rrcsmc_tm_ciph_active() == TRUE)
      {
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        uecomdef_status_e_type loc_ret_val;
        WRRC_MSG0_HIGH("To-DCH-Step-1 config activated");
        loc_ret_val = rrcrb_get_mac_d_hfn(
             (RRC_MSG_COMMON_BITMASK_IE_PTR(
             msg_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon,rrc_DL_DPCH_InfoCommon_r6,
               mac_d_HFN_initial_value)),
              &msg_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
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
        pcrc.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }

  /*Cell_PCH/URA_PCH specific validations */
  if ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
     /* If UTRAN DRX coefficient is not present then it is invalid configuration */
     if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr, 
         rrc_PhysicalChannelReconfiguration_r6_IEs,utran_DRX_CycleLengthCoeff)))
     {
       WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", pcrc.state_indicated_by_msg);
       pcrc.status.failure_status = RRCPC_MSG_INVALID_CONFIG;
       return;
     }
     if (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
     {
       /*should we do it after getting l2ack for response??*/
       if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr, rrc_PhysicalChannelReconfiguration_r6_IEs,ura_Identity)))
       {
         WRRC_MSG0_HIGH("URA ID not present");
         rrccu_clear_ura_id ();
       }
       else
       {
         /*URA ID present in the message set ura_identity variable with this value */
         if (msg_ptr->ura_Identity.numbits == 16)
         {
           rrccu_update_ura_id (&(msg_ptr->ura_Identity));
         }
         else
         {
           WRRC_MSG1_ERROR("Invalid URA ID length%d",
               msg_ptr->ura_Identity.numbits);
           pcrc.status.failure_status = RRCPC_MSG_PROTOCOL_ERROR;
           pcrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
           return;
         }

       }
           
     }
  }
}/* end validate_pcrc_r6() */


/*===========================================================================

FUNCTION   validate_pcrc_r7

DESCRIPTION

  This function validates the received PC Reconfig Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a PCRC Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_pcrc_r7
(
  rrc_PhysicalChannelReconfiguration_r7_IEs *msg_ptr /* Pointer to the PCRC message */
)
{
 
  uecomdef_status_e_type status;
    /* Assume the message is valid for now */
  pcrc.status.failure_status = RRCPC_MSG_VALID;

  /* Check the RRC State indicatior given by the message */
  switch( msg_ptr->rrc_StateIndicator )
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      pcrc.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      pcrc.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      pcrc.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;

    case rrc_RRC_StateIndicator_ura_PCH:
      pcrc.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;


    default:
      /* No other states are supported right now. */
      WRRC_MSG0_HIGH("Unsupported state in PC Reconfig Message");
      pcrc.status.failure_status = RRCPC_MSG_UNSUPPORTED_CONFIG;
      break;
  }

  if( pcrc.status.failure_status == RRCPC_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
     return;
  }

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr, 
      rrc_PhysicalChannelReconfiguration_r7_IEs,dl_HSPDSCH_Information))
  {
    
    /* Check if network does not try to initiate HSDPA with invalid state */
    status = rrc_hsdpa_initial_validation(msg_ptr->rrc_StateIndicator, 
          RRC_CHECK_COMMON_MSG_TYPE(msg_ptr->dl_HSPDSCH_Information.modeSpecificInfo,
          rrc_DL_HSPDSCH_Information_modeSpecificInfo_tdd));
    
    if (status == FAILURE)
    {
      pcrc.status.failure_status = RRCPC_MSG_INVALID_CONFIG;
      return;
    }
  }
  
   /* Check if RB with PDCP Information list is present. This
  is not supported in the first release. This may be supported
  later */                                    
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr, 
        rrc_PhysicalChannelReconfiguration_r7_IEs,dl_CounterSynchronisationInfo)) 
  {

    /*check whether the SRNS relocation is within Cell_DCH or not.
     SRNS relocation information can come in reconfiguration messages
     only when the state transition is Cell_DCH->Cell_DCH */
    if (!((pcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("PCRC : SRNS reloc with invalid states cur_state %d, msg_state %d",
             rrc_get_state(), pcrc.state_indicated_by_msg);
      pcrc.status.failure_status=RRCPC_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has 
    PDCP info list. If it is true then this configuration is unsupported */
    if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2(msg_ptr->dl_CounterSynchronisationInfo,
		rb_WithPDCP_InfoList)) ||
        (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->dl_CounterSynchronisationInfo,
         rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
    {
      WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
      pcrc.status.failure_status=RRCPC_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation. 
        Set the flag that indicates that this procedure
      involves SRNS relocation */
      pcrc.pcrc_for_srns_relocation = TRUE;
      /* store if new U-RNTI is present in the message */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr, rrc_PhysicalChannelReconfiguration_r7_IEs,new_U_RNTI))
      {
        pcrc.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(msg_ptr->new_U_RNTI),
                              &pcrc.new_urnti);
      }
    }
  }

    /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  pcrc.trans_to_dch_ciph_config_needed = FALSE;
  if((pcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr,
        rrc_PhysicalChannelReconfiguration_r7_IEs,dl_CommonInformation)) && 
       (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->dl_CommonInformation,
       rrc_DL_CommonInformation_r7,dl_dpchInfoCommon)) &&
       (T_rrc_DL_CommonInformation_r7_dl_dpchInfoCommon_dl_DPCH_InfoCommon == 
         msg_ptr->dl_CommonInformation.dl_dpchInfoCommon.t))
    {
      //check if ciphering is active in mac for cs domain, if it is then,
      //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      if(rrcsmc_tm_ciph_active() == TRUE)
      {
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        uecomdef_status_e_type loc_ret_val;
        WRRC_MSG0_HIGH("To-DCH-Step-1 config activated");
        loc_ret_val = rrcrb_get_mac_d_hfn(
             (RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon,rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
              &msg_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
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
        pcrc.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }

  /*Cell_PCH/URA_PCH specific validations */
  if ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
     /* If UTRAN DRX coefficient is not present then it is invalid configuration */
     if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr, 
         rrc_PhysicalChannelReconfiguration_r7_IEs,utran_DRX_CycleLengthCoeff)))
     {
       WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", pcrc.state_indicated_by_msg);
       pcrc.status.failure_status = RRCPC_MSG_INVALID_CONFIG;
       return;
     }
     if (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
     {
       /*should we do it after getting l2ack for response??*/
       if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr, rrc_PhysicalChannelReconfiguration_r7_IEs,ura_Identity)))
       {
         WRRC_MSG0_HIGH("URA ID not present");
         rrccu_clear_ura_id ();
       }
       else
       {
         /*URA ID present in the message set ura_identity variable with this value */
         if (msg_ptr->ura_Identity.numbits == 16)
         {
           rrccu_update_ura_id (&(msg_ptr->ura_Identity));
         }
         else
         {
           WRRC_MSG1_ERROR("Invalid URA ID length%d",
               msg_ptr->ura_Identity.numbits);
           pcrc.status.failure_status = RRCPC_MSG_PROTOCOL_ERROR;
           pcrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
           return;
         }

       }
           
     }
  }
}/* end validate_pcrc_r7() */


/*===========================================================================

FUNCTION   rrcpcrc_check_and_send_failure_for_simul_reconfig

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
static uecomdef_status_e_type rrcpcrc_check_and_send_failure_for_simul_reconfig
(
  rrc_RRC_TransactionIdentifier transaction_id
)
{
  if ((rrcllc_get_ordered_config_status() != OC_NOT_SET) ||
      (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
     (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
  {
    MSG_HIGH("Incomatible simultaneous reconfig! Rejecting PCRC message,transaction_id:%d,accepted_transaction:%d rejected_trans_id:%d",
      transaction_id,pcrc.tr_ids.accepted_transaction,pcrc.tr_ids.rejected_transaction);
    if ((transaction_id != pcrc.tr_ids.accepted_transaction) && (transaction_id != pcrc.tr_ids.rejected_transaction))
    {
      /* We have received a RB RC message with a new transaction id.
      This message should be in our rejected transaction list */
      pcrc.tr_ids.rejected_transaction = transaction_id;
      pcrc.status.failure_status = RRCPC_SIMULTANEOUS_RECONFIG;
 
      /*Check whether Cell Update is going on */
      if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
           (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
      {
        if(pcrc.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
        {
          send_pcrc_failure_message( pcrc.status.failure_status,
                                   pcrc.status.prot_err_cause );
          pcrc.status.failure_status = RRCPC_MSG_VALID;
        }
        else
        {
          WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
          if (SUCCESS != rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_PCR))
          {
            WRRC_MSG0_ERROR("PCR failed to register with CU");
          }
          /* Wait until Cell update procedure is completed */
          pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF; 
        }
      }
      else
      {
        send_pcrc_failure_message( pcrc.status.failure_status,
                                        pcrc.status.prot_err_cause );
        /*continue the same substate */ 
      }
    }
    return (FAILURE);
  }

 /* We have received a Physical Channel Reconfig message with a new transaction id.
 Accept the new transaction ID  */
  pcrc.tr_ids.accepted_transaction = transaction_id;

  return (SUCCESS);
}

/*===========================================================================

FUNCTION   VALIDATE_PCRC_MESSAGE

DESCRIPTION

  This function validates the received PC Reconfig Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a PCRC Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_pcrc_message
(
  rrc_PhysicalChannelReconfiguration *msg_ptr /* Pointer to the PCRC message */
)
{
 
    /* Assume the message is valid for now */
  pcrc.status.failure_status = RRCPC_MSG_VALID;
  
  if( msg_ptr->t == 
      T_rrc_PhysicalChannelReconfiguration_later_than_r3 )
  {
    /* Critical extensions are present that are not supported by this
    release. We have to send a PCRC Failure message. */
    WRRC_MSG0_HIGH("Critical exstension present in PCRC msg!");
    pcrc.status.failure_status = RRCPC_MSG_PROTOCOL_ERROR;
    pcrc.status.prot_err_cause = rrc_ProtocolErrorCause_messageExtensionNotComprehended;
    return;
  }

  /* Check the RRC State indicatior given by the message */
  switch( msg_ptr->u.r3.physicalChannelReconfiguration_r3.rrc_StateIndicator )
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      pcrc.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      pcrc.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      pcrc.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;

    case rrc_RRC_StateIndicator_ura_PCH:
      pcrc.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;

    default:
      /* No other states are supported right now. */
      WRRC_MSG0_HIGH("Unsupported state in PC Reconfig Message");
      pcrc.status.failure_status = RRCPC_MSG_UNSUPPORTED_CONFIG;
      break;
  }

  if( pcrc.status.failure_status == RRCPC_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
     return;
  }
  if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3, 
      rrc_PhysicalChannelReconfiguration_r3,v3a0NonCriticalExtensions))
  {
    WRRC_MSG0_HIGH("NonCritical extension present: Ignoring");
  }

   /* Check if RB with PDCP Information list is present. This
  is not supported in the first release. This may be supported
  later */                                    
  if( RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.physicalChannelReconfiguration_r3,
       rrc_PhysicalChannelReconfiguration_r3_IEs,dl_CounterSynchronisationInfo))
  {

    /*check whether the SRNS relocation is within Cell_DCH or not.
     SRNS relocation information can come in reconfiguration messages
     only when the state transition is Cell_DCH->Cell_DCH */
    if (!((pcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("PCRC : SRNS reloc with invalid states cur_state %d, msg_state %d",
             rrc_get_state(), pcrc.state_indicated_by_msg);
      pcrc.status.failure_status=RRCPC_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has 
    PDCP info list. If it is true then this configuration is unsupported */
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.physicalChannelReconfiguration_r3.dl_CounterSynchronisationInfo,
		       rrc_DL_CounterSynchronisationInfo,rB_WithPDCP_InfoList))
    {
      WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
      pcrc.status.failure_status=RRCPC_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation. 
        Set the flag that indicates that this procedure
      involves SRNS relocation */
      pcrc.pcrc_for_srns_relocation = TRUE;
      /* store if new U-RNTI is present in the message */
      if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.physicalChannelReconfiguration_r3,
      rrc_PhysicalChannelReconfiguration_r3_IEs,new_U_RNTI))
      {
        pcrc.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(msg_ptr->u.r3.physicalChannelReconfiguration_r3.new_U_RNTI),
                              &pcrc.new_urnti);
      }
    }
  }

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  pcrc.trans_to_dch_ciph_config_needed = FALSE;
  if((pcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.physicalChannelReconfiguration_r3,
        rrc_PhysicalChannelReconfiguration_r3_IEs,dl_CommonInformation)) && 
       (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.physicalChannelReconfiguration_r3.dl_CommonInformation,
        rrc_DL_CommonInformation,dl_DPCH_InfoCommon)))
    {
      //check if ciphering is active in mac for cs domain, if it is then,
      //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      if(rrcsmc_tm_ciph_active() == TRUE)
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 config activated");
        if(rrcsmc_process_cipher_config_for_hho(RRCSMC_INVALID_HFN_VALUE) == FAILURE)
        {
          WRRC_MSG0_HIGH("rrcsmc_process_cipher_config_for_hho failed");
        }
        pcrc.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }
  /*Cell_PCH/URA_PCH specific validations */
  if ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
     /* If UTRAN DRX coefficient is not present then it is invalid configuration */
     if (!(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.physicalChannelReconfiguration_r3,
         rrc_PhysicalChannelReconfiguration_r3_IEs,utran_DRX_CycleLengthCoeff)))
     {
       WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", pcrc.state_indicated_by_msg);
       pcrc.status.failure_status = RRCPC_MSG_INVALID_CONFIG;
       return;
     }
     if (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
     {
       /*should we do it after getting l2ack for response??*/
       if (!(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.physicalChannelReconfiguration_r3,
             rrc_PhysicalChannelReconfiguration_r3_IEs,ura_Identity)))
           
       {
         WRRC_MSG0_HIGH("URA ID not present");
         rrccu_clear_ura_id ();
       }
       else
       {
         /*URA ID present in the message set ura_identity variable with this value */
         if (msg_ptr->u.r3.physicalChannelReconfiguration_r3.ura_Identity.numbits == 16)
         {
           /*rrccu_translate_ura_id (&(msg_ptr->u.r3.radioBearerReconfiguration_r3.ura_Identity));*/
           rrccu_update_ura_id (&(msg_ptr->u.r3.physicalChannelReconfiguration_r3.ura_Identity));
         }
         else
         {
           WRRC_MSG1_ERROR("Invalid URA ID length%d",
               msg_ptr->u.r3.physicalChannelReconfiguration_r3.ura_Identity.numbits);
           pcrc.status.failure_status = RRCPC_MSG_PROTOCOL_ERROR;
           pcrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
           return;
         }

       }
           
     }
  }
}/* end function validate_pcrc_message */


/*===========================================================================

FUNCTION   RRCPCRC_SEND_CHANNEL_CONFIG_REQ

DESCRIPTION

  This function sends the RRC_CHANNEL_CONFIG_REQ command to LLC
        
DEPENDENCIES

  ORDERED_CONFIG must be set before calling this function.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcpcrc_send_channel_config_req
(
  void
)
{
  rrc_cmd_type *ptr;          /* To hold the Command                       */
  uint32 rb_count=0;      /* To count RBs to be reconfigured           */
  rrc_state_e_type next_rrc_state = pcrc.state_indicated_by_msg;
    
  /* Get the command buffer */
  ptr = rrc_get_int_cmd_buf();
  {
    /* Fill in the command id  */
    ptr->cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;


    /* Check if state change is required */
    if( (rrc_get_state()) == (pcrc.state_indicated_by_msg) )
    {
      ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;
    }
    else
    {
      /* The message indicates that state change is required.
      Indicate to LLC what the next state should be. */
      ptr->cmd.chan_config_req.rrc_state_change_required = TRUE;
      ptr->cmd.chan_config_req.next_state = pcrc.state_indicated_by_msg;

      if ((rrc_get_state() == RRC_STATE_CELL_FACH) && 
          ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) || 
           (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)))
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
    ptr->cmd.chan_config_req.procedure = RRC_PROCEDURE_PCR;

    /* Indicate that we need a confirmation for this command */
    ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = TRUE;

    /*If state transition is Cell_DCH->Cell_FACH or vice-verca, 
      add all the existing AM PS RBs in chan_config_req*/	
    if(((next_rrc_state == RRC_STATE_CELL_FACH) && 
         ((transition_config.toc_usage == TOC_FOR_DCH_AND_FACH) ||
          (transition_config.toc_usage == TOC_FOR_FACH_AND_PCH) ||
          (transition_config.toc_usage == TOC_FOR_FACH_AND_PCH_WITH_DCH_INFO)))
         ||
        ((rrc_get_state() == RRC_STATE_CELL_FACH) && (next_rrc_state == RRC_STATE_CELL_DCH))
      )
    {
      rrc_fill_rb_id_chan_req_from_est_rabs(&rb_count, &ptr->cmd.chan_config_req);
    }

    if((pcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) 
          && (rrc_get_state() == RRC_STATE_CELL_FACH))
    {
      rrc_fill_srb_in_chan_config_req(&rb_count, &ptr->cmd.chan_config_req);
    }

    /* We are not required to fill any other data for this command.
    Now, send the command */
    WRRC_MSG1_HIGH("CHANNEL_CONFIG_REQ RBs %d",rb_count);
    rrc_put_int_cmd( ptr );

    
  }
}/* end function rrcpcrc_send_channel_config_req */


/*===========================================================================

FUNCTION  RRCPCRC_CLEAR_PROCEDURE

DESCRIPTION

  This function clears all the internal variables in
  the PCRC procedure and brings them to their default
  values.
    
DEPENDENCIES

  The Substate of the procedure should be changed
  to initial substate before calling this function.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcpcrc_clear_procedure( void )
{
  /* Initialize the transaction id's for this procedure.
  Use a number that can't be an actual received transaction
  id. */
  pcrc.tr_ids.accepted_transaction = RRCPC_INVALID_TRANSACTION_ID;
  pcrc.tr_ids.rejected_transaction = RRCPC_INVALID_TRANSACTION_ID;

  /* Initialize the current substate */
  pcrc.curr_substate = RRCRB_INITIAL;

   /* Clear the C-RNTI valid flag */
  pcrc.new_crnti_valid    = FALSE;
  pcrc.directed_cell_info = FALSE;

  pcrc.pcrc_for_srns_relocation = FALSE;
  pcrc.new_urnti_valid = FALSE;

  /* free the memory allocate for mm_cmd_buf
  if it is not free or in case of unsuccessful events
  before sending this cmd to mm */
  if (pcrc.cn_info_saved != FALSE)
  {
    /* free the memory allocated for mm cmd */
    WRRC_MSG1_HIGH("Free MM cmd buffer %x", pcrc.mm_cmd_ptr);
    mm_free_cmd_buf(pcrc.mm_cmd_ptr);
    /* set cn_info_saved to false */
    pcrc.cn_info_saved = FALSE;
  }

  /* Reset Initiate Cell Update Flag */
  rrcpcrc_send_initiate_cu_flag = FALSE;
  rrcpcrc_delete_crnti = FALSE;

  //Set hho_ciph_status to false
  pcrc.hho_ciph_status = FALSE;      

  pcrc.trans_to_dch_ciph_config_needed = FALSE;
  pcrc.initiate_cell_selection_trans_from_fach = FALSE;     
  pcrc.status.failure_status = RRCPC_MSG_VALID;

  pcrc.high_mobility_ind = FALSE;

  rrc_check_and_init_rrc_reestab_entity(RRC_PROCEDURE_PCR);
}


/*===========================================================================

FUNCTION   RRCPCRC_PROCESS_SUCCESSFUL_PROC_COMPLETION

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
static void rrcpcrc_process_successful_proc_completion
(
 boolean indicate_rlc
)
{
  /* send the nas cn info if stored */
  if (pcrc.cn_info_saved != FALSE)
  {
    /* Put the command on MM queue */

    /* clear the flag */
    if(pcrc.mm_cmd_ptr->cmd.hdr.message_id == (byte) RRC_SERVICE_IND_FROM_OTA_MSG )
    {
       rrc_ccm_update_service_info(pcrc.mm_cmd_ptr);
    }
    rrc_send_rrc_message_to_mm(pcrc.mm_cmd_ptr);
    pcrc.cn_info_saved = FALSE;
  }
  /* Clear ORDERED_CONFIG - UE can now accept other ordered
  configurations */
  rrcllc_clear_ordered_config();  

  if (((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) || 
       (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH )) &&
      (indicate_rlc == TRUE))
  {
    /* register with RLC for uplink data indication */
    rrcrb_register_with_rlc_for_ul_data_ind (RRC_PROCEDURE_PCR);

  }

  /* Go to the next sub-state in the state machine. That's
  the initial substate since we've sent the RB RC complete
  message */
  pcrc.curr_substate = RRCRB_INITIAL;

  /* At this point we don't need to remember any of the
  Radio Bearer information since it's stored in ESTABLISHED_RABS.
  Clear our internal variables */
  rrcpcrc_clear_procedure();

  if (rrccu_return_cu_substate() == RRCCU_WAIT_FOR_RB_PROC_COMPL_IND) 
  {
    rrcrb_send_rb_proc_compl_ind();
  }

  rrccu_reset_registration_info_for_procedure(RRC_PROCEDURE_PCR);

}    /* rrcpcrc_process_successful_proc_completion */
/*===========================================================================

FUNCTION   rrcpcr_check_and_get_primary_plmn_id

DESCRIPTION

  This function will check whether primary plmn id is present 
  in r3,r5 path of PCR message. If present then it will return 
  primary plmn id.
        
DEPENDENCIES
  None.

RETURN VALUE
TRUE : IF Primary PLMN I.D is present in PCR message in r3,r5 path
FFALSE: Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
static boolean rrcpcr_check_and_get_primary_plmn_id
(
  /* Pointer to the PCR message */
  rrc_PhysicalChannelReconfiguration *msg_ptr,
  /*This will copy primary plmn i.d if present in OTA msg*/
  rrc_PLMN_Identity *primary_plmn_Identity ,
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
  /*Check to see if primary PLMN I.D is present in PCR message ext
   * physicalChannelReconfiguration-v690ext which is present in 
   * v690NonCriticalExtensions
   */
  if (MSG_REL99 == msg_version)
  {
      if((msg_ptr->u.r3.m.v3a0NonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.v3a0NonCriticalExtensions.m.laterNonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.m.v4b0NonCriticalExtenstionsPresent)&&
          (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          v4b0NonCriticalExtenstions.m.v590NonCriticalExtenstionsPresent)&&
          (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          v4b0NonCriticalExtenstions.v590NonCriticalExtenstions.m.v690NonCriticalExtensionsPresent)&&
          (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          v4b0NonCriticalExtenstions.v590NonCriticalExtenstions.v690NonCriticalExtensions.
          physicalChannelReconfiguration_v690ext.m.primary_plmn_IdentityPresent)
        )
      {
        primary_plmn_Id_present = TRUE;
        *primary_plmn_Identity = msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              v4b0NonCriticalExtenstions.v590NonCriticalExtenstions.v690NonCriticalExtensions.
               physicalChannelReconfiguration_v690ext.primary_plmn_Identity;
      }
  }
  else if (MSG_REL5 == msg_version)
  {
      if((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.m.v690NonCriticalExtensionsPresent)
         &&
         (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.
           u.r5.v690NonCriticalExtensions.physicalChannelReconfiguration_v690ext.m.primary_plmn_IdentityPresent)
        )
      {
        primary_plmn_Id_present = TRUE;
        *primary_plmn_Identity = msg_ptr->u.later_than_r3.criticalExtensions.u.
              criticalExtensions.u.r5.v690NonCriticalExtensions.
              physicalChannelReconfiguration_v690ext.primary_plmn_Identity;
      }
  }
  WRRC_MSG2_HIGH("NWS:PCR msg contains primary_plmn_Id %d (0-Not present 1-Prsent) for msg_version:%d(0-MSG_REL99,1-MSG_REL5,2-MSG_REL6,3-MSG_REL7,4-MSG_REL8,5-MSG_REL9,6-MSG_REL10,other-MSG_INVALID)",primary_plmn_Id_present,msg_version);  
  return primary_plmn_Id_present;
}



/*===========================================================================

FUNCTION   rrcpcrc_process_high_mobility_ind_in_non_critical_ext

DESCRIPTION

  This function will process High Mobility Ind received in Non-critical ext
        
DEPENDENCIES
  None.

RETURN VALUE
None

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcpcrc_process_high_mobility_ind_in_non_critical_ext
(
  rrc_PhysicalChannelReconfiguration *msg_ptr,
  rrc_msg_ver_enum_type msg_version
)
{
  if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
  {
    if(msg_version == MSG_REL5)
    {
     if((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.m.v690NonCriticalExtensionsPresent) &&
          (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.v690NonCriticalExtensions.m.
          v770NonCriticalExtensionsPresent) &&
          (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.v690NonCriticalExtensions.v770NonCriticalExtensions.
          physicalChannelReconfiguration_v770ext.m.ueMobilityStateIndicatorPresent))
      {
        if(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.v690NonCriticalExtensions.v770NonCriticalExtensions.
            physicalChannelReconfiguration_v770ext.ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
        {
          pcrc.high_mobility_ind = TRUE;
        }
      }
    }
    else if(msg_version == MSG_REL6)
    {
     if(( msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.v6b0NonCriticalExtensions.m.
           v770NonCriticalExtensionsPresent) &&
              (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.v6b0NonCriticalExtensions.v770NonCriticalExtensions.
              physicalChannelReconfiguration_v770ext.m.ueMobilityStateIndicatorPresent))
     {
       if(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.v6b0NonCriticalExtensions.
           v770NonCriticalExtensions.physicalChannelReconfiguration_v770ext.ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
       {
         pcrc.high_mobility_ind = TRUE;
       }
     }     
    }
    else if(msg_version == MSG_REL99)
    {
      if((msg_ptr->u.r3.m.v3a0NonCriticalExtensionsPresent) &&
        (msg_ptr->u.r3.v3a0NonCriticalExtensions.m.laterNonCriticalExtensionsPresent) &&
        (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.m.v4b0NonCriticalExtenstionsPresent) &&
        (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtenstions.m.
        v590NonCriticalExtenstionsPresent) && 
        (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtenstions.v590NonCriticalExtenstions.m. 
        v690NonCriticalExtensionsPresent) &&
        (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtenstions.v590NonCriticalExtenstions.
        v690NonCriticalExtensions.m.v770NonCriticalExtensionsPresent) &&
        (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtenstions.v590NonCriticalExtenstions.
        v690NonCriticalExtensions.v770NonCriticalExtensions.physicalChannelReconfiguration_v770ext.m.ueMobilityStateIndicatorPresent))
      {
        if(msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtenstions.
			v590NonCriticalExtenstions.v690NonCriticalExtensions.v770NonCriticalExtensions.
			physicalChannelReconfiguration_v770ext.ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected) 
        {
          pcrc.high_mobility_ind = TRUE;
        }
      }
    }  
  }
  WRRC_MSG2_HIGH("High Mobility Ind Set to %d in PCRC.msg_version:%d (0-MSG_REL99,1-MSG_REL5,2-MSG_REL6,3-MSG_REL7,4-MSG_REL8,5-MSG_REL9,6-MSG_REL10,other-MSG_INVALID)",pcrc.high_mobility_ind,msg_version);
}

/*===========================================================================

FUNCTION   PROCESS_PCRC_R5

DESCRIPTION

  This function processes the received Rel 5 PC Reconfig Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a PCRC Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to reconfigure the Physical Channel given by the message.
    
    If ORDERED_CONFIG could not be set, a PCRC Failure message
    will be sent to UTRAN.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_pcrc_r5
(
  rrc_cmd_type *cmd_ptr
)
{
  rrc_PhysicalChannelReconfiguration_r5_IEs* pcrc_ptr = NULL;

  rrc_DL_DCCH_Message  *dcch_msg_ptr = NULL; /* Pointer to the RB Setup message */
  rrc_PhysicalChannelReconfiguration *msg_ptr = NULL;		/* Pointer to the RB Setup message        */

  rrcllc_oc_status_e_type             oc_status;/* To hold ordered_config status   */
  rrc_state_e_type                    current_state; /* current RRC State  */
  rrc_int_c_rnti_type                 c_rnti;       /* Local C_RNTI value */
  rrc_int_u_rnti_type                 u_rnti;
  rrcrb_directed_cell_info_type       directed_cell_info; /* directed cell info struct */

  rrcrb_rnti_update_info_type rnti_update_info;   /* to hold rnti update info */

  boolean       ordered_config_err_occured = FALSE; /* Used to indicate if an error
                                    occured while setting ordered_config. */

   /*Boolean used to indicate primary plmn i.d is present in OTA msg*/
  boolean primary_plmn_Id_present = FALSE;
   /*Variable to hold primary plmn i.d if present in OTA msg*/
  rrc_PLMN_Identity primary_plmn_Identity;


  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  msg_ptr = &dcch_msg_ptr->message.u.physicalChannelReconfiguration;

  if (FAILURE == 
      rrcpcrc_check_and_send_failure_for_simul_reconfig(
        msg_ptr->u.later_than_r3.rrc_TransactionIdentifier))
  {
    return;
  }

  /* Store Msg Ptr in a local variable */
  pcrc_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.
    criticalExtensions.u.r5.physicalChannelReconfiguration_r5;

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  validate_pcrc_r5( pcrc_ptr );

  if( pcrc.status.failure_status != RRCPC_MSG_VALID )
  {
    /*  Message was invalid. */
    WRRC_MSG0_ERROR("PCRC Message validation failed");

    /* Send the PC Reconfig Failure Message */
    send_pcrc_failure_message( pcrc.status.failure_status,
                              pcrc.status.prot_err_cause
                            );

    /* Make sure we're back in the initial sub-state */
    pcrc.curr_substate = RRCRB_INITIAL;
  }
  /* If the message is valid, proceed further */
  else
  {

    MSG_LOW("Processing PCRC Message", 0, 0, 0);

    /* Check if there is CN information present. If so, we need
    to send the information to the Cell Change manager */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr,rrc_PhysicalChannelReconfiguration_r5_IEs,cn_InformationInfo))
    {
      WRRC_MSG0_HIGH("Saving CN info for NAS");
      /*Check to see if primary PLMN I.D is present in PCR message ext
       * physicalChannelReconfiguration-v690ext which is present in 
       * v690NonCriticalExtensions
       */
       primary_plmn_Id_present = rrcpcr_check_and_get_primary_plmn_id(msg_ptr,
                                                                  &primary_plmn_Identity,MSG_REL5);
      pcrc.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info(
                        &(pcrc_ptr->cn_InformationInfo)
                        ,primary_plmn_Id_present
                        ,&primary_plmn_Identity
                        );
 
      if (pcrc.mm_cmd_ptr != NULL)
      {
        /* set cn_info_saved flag */
        pcrc.cn_info_saved = TRUE;
      }
    }
    rrcpcrc_process_high_mobility_ind_in_non_critical_ext(msg_ptr,MSG_REL5);
   
    /* Need to send reconfig IE info. First set ORDERED_CONFIG
    so that LLC can process the necessary parameters. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_PCR,
                                           pcrc.state_indicated_by_msg,
                                           rrc_DL_DCCH_Message_PDU,
                                           (void *)dcch_msg_ptr );

    switch( oc_status )
    {

      case ORDERED_CONFIG_SET:
        current_state = rrc_get_state(); 
        if((rrc_sib_is_current_event_sib_read_in_dch()) &&
           (RRC_PROCEDURE_GPS == rrc_sib_get_proc_id_for_sib_read_in_dch()))
        {
          WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
          rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
        }



        /* Populate the local variables to indicate if Re-direction info is present */
        directed_cell_info.freq_info_present = FALSE;
        directed_cell_info.pscr_present = FALSE;
        if((current_state==RRC_STATE_CELL_FACH) && 
                 ((pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
                 ||(pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
                 ||(pcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
           
          if((RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr, 
              rrc_PhysicalChannelReconfiguration_r5_IEs,frequencyInfo)) &&
             (RRC_CHECK_COMMON_MSG_TYPE(pcrc_ptr->frequencyInfo.
              modeSpecificInfo,rrc_FrequencyInfo_modeSpecificInfo_fdd)))           
          {
            directed_cell_info.freq_info_present = TRUE;
            directed_cell_info.uarfcn_dl = pcrc_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
          }
          
          if (rrcpcrc_is_pcpich_info_present(msg_ptr, MSG_REL5))
          {
            directed_cell_info.pscr_present = TRUE;
            directed_cell_info.pscr = pcrc_ptr->dl_InformationPerRL_List.elem[0].
                                      modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
          }
          /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
          FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
          if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
             (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
          {
            WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
            if(pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
            {
              rrcpcrc_send_initiate_cu_flag = TRUE;
              rrcpcrc_delete_crnti          = TRUE;
            }
            else
            {
              pcrc.directed_cell_info = TRUE;
            }
          }          
        }
        if (
            (current_state==pcrc.state_indicated_by_msg)||
            ((current_state==RRC_STATE_CELL_FACH)&& 
             (pcrc.state_indicated_by_msg==RRC_STATE_CELL_DCH))
           )
        {
          /*  CellDCH<->CellDCH or CellFACH<->CellFACH or CellFACH->CellDCH 
          transition. Ordered Config has been set. We can setup the
          Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */

          /*check whether any frequency info is present in the message
          and transition is CELL_FACH->CELL_FACH. In this case
          initiate a cell update after channel config is successfully completed */
          if ((current_state==RRC_STATE_CELL_FACH)&& 
             (pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
          {
     
            /*call the function that evaluates the directed cell info and set the flags accordingly */
            if ((rrcpcrc_send_initiate_cu_flag == FALSE)&&
                (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
            {
    
              if(RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr, rrc_PhysicalChannelReconfiguration_r5_IEs,new_C_RNTI))
              {
                pcrc.new_crnti_valid = TRUE;
                rrc_translate_crnti(&pcrc_ptr->new_C_RNTI, 
                                    &c_rnti);
                pcrc.new_crnti = (uint16)c_rnti;
              }
              else
              {
                pcrc.new_crnti_valid = FALSE;
              }
              /* Now copy OC to TOC */
              rrcllcoc_update_toc_with_oc_from_fach();
              /*Initiate the cell selection*/
              rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_PCR,
                                                       RRC_TRANSITION_FROM_FACH,
                                                       pcrc.state_indicated_by_msg);
    
        
              pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
     
              /* exit from the switch case */
              break;
            }
           } /* if CELL_FACH->CELL_FACH reconfiguration */

          /* Call the function that creates and sends a channel
          config request to LLC. */
          rrcpcrc_send_channel_config_req( );
             /* Change the substate since we're waiting for the
             confirmation from LLC for the channel configs */
          pcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
        {
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr, rrc_PhysicalChannelReconfiguration_r5_IEs,new_C_RNTI))
          {
            pcrc.new_crnti_valid = TRUE;
            rrc_translate_crnti(&pcrc_ptr->new_C_RNTI, 
                                &c_rnti);
            pcrc.new_crnti = (uint16)c_rnti;
          }
          else
          {
            pcrc.new_crnti_valid = FALSE;
          }


          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_PCR,
                                                  RRC_TRANSITION_FROM_DCH,
                                                  pcrc.state_indicated_by_msg);
          pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

          /* Start T_305 [periodic cell update] timer */
          rrccu_start_t_305_timer();

        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH))
        {
          pcrc.directed_cell_info = rrcpcrc_is_pcpich_info_present(msg_ptr, MSG_REL5);
         /*Send the PCRC complete msg on old configuration */
         send_pcrc_complete_message(TRUE);
         /*wait for L2ack */
         pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
         /* prints rate for each transport channel */
         rrcllc_print_trans_chnl_rate();
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (pcrc.state_indicated_by_msg==RRC_STATE_URA_PCH))
        {
          /*Send the PCRC complete msg on old configuration */
          send_pcrc_complete_message(TRUE);
          /*wait for L2ack */
          pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
          /* prints rate for each transport channel */
          rrcllc_print_trans_chnl_rate();
        }
        else if((current_state==RRC_STATE_CELL_FACH) && 
                ((pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
                 (pcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          /*call the function that evaluates the directed cell info and set the flags accordingly */
          if ((pcrc.directed_cell_info == FALSE) &&
            (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
          {
            if(pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
            {
              pcrc.directed_cell_info = rrcpcrc_is_pcpich_info_present(msg_ptr, MSG_REL5);
            }
          
            pcrc.initiate_cell_selection_trans_from_fach = TRUE;
          }
          

          /* check whether new C-RNTI was present in the message. In that case
          we have to configure MAC with new C-RNTI before sending the response 
          message */
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr, 
             rrc_PhysicalChannelReconfiguration_r5_IEs,new_C_RNTI))
          {
            rrc_translate_crnti(&pcrc_ptr->new_C_RNTI, 
                                &c_rnti);
            /* Send a RNTI Update Request */
            rnti_update_info.crnti_action    = RRC_RNTI_UPDATE;
            rnti_update_info.crnti           = (uint16)c_rnti;
            /* check whether URNTI is also present in the message */
            if (RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr,
             rrc_PhysicalChannelReconfiguration_r5_IEs,new_U_RNTI))
            {
              rrc_translate_urnti(&pcrc_ptr->new_U_RNTI, 
                                  &u_rnti);
              rnti_update_info.urnti_action = RRC_RNTI_UPDATE; 
              rnti_update_info.urnti        = u_rnti; 
            }
            else
            {
              rnti_update_info.urnti_action = RRC_RNTI_NOCHANGE; 
            }
            rnti_update_info.procedure            = RRC_PROCEDURE_PCR;
            rnti_update_info.rlc_re_establish_srb = FALSE;
            rnti_update_info.rlc_re_establish_rab = FALSE;
            rnti_update_info.cnf_reqd             = TRUE;

            rrcrb_send_rnti_update_req(rnti_update_info);
            pcrc.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
          }
          else 
          {
            /* There is no new C-RNTI So send the response on the old configuration
            */
            send_pcrc_complete_message(TRUE);
            /*wait for L2ack */
            pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
            /* prints rate for each transport channel */
            rrcllc_print_trans_chnl_rate();
          }
        }
        break;

      case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

        pcrc.status.failure_status = RRCPC_SIMULTANEOUS_RECONFIG;
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause );

        /* Go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

        pcrc.status.failure_status =RRCPC_MSG_UNSUPPORTED_CONFIG;
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause );

        /* Go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_INVALID:

        pcrc.status.failure_status =RRCPC_MSG_INVALID_CONFIG;
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause );

        /* Go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
		
      case ORDERED_CONFIG_NOT_SET_OTHER:
        WRRC_MSG0_HIGH("LLC returned other failure!");
        /* When LLC sends this, there is a serious problem
        and we'll go back to disconnected state. No need
        to send the failure message here, we can just
        go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME         
        case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
          WRRC_MSG0_HIGH("PCR processing deferred due to LTA!");
          pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/

      default:

        WRRC_MSG0_ERROR("Unknown return value from LLC");
        /* Unknown error, just go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
    }/* end switch */

    if( ordered_config_err_occured == TRUE )
    {
      /* No need to continue. Clear the procedure
      variables and return. */
      rrcpcrc_clear_procedure();
      return;
    }

    if(pcrc.pcrc_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }
  }
}




/*===========================================================================

FUNCTION   PROCESS_PCRC_R6

DESCRIPTION

  This function processes the received Rel 6 PC Reconfig Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a PCRC Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to reconfigure the Physical Channel given by the message.
    
    If ORDERED_CONFIG could not be set, a PCRC Failure message
    will be sent to UTRAN.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_pcrc_r6
(
  rrc_RRC_TransactionIdentifier transaction_id,

  rrc_DL_DCCH_Message *dcch_msg_ptr  ,
  /* Pointer to the RB Reconfig message        */
  rrc_PhysicalChannelReconfiguration_r6_IEs *pcrc_ptr
)
{
  rrc_PhysicalChannelReconfiguration *msg_ptr = NULL;		/* Pointer to the RB Setup message        */

  rrcllc_oc_status_e_type             oc_status;/* To hold ordered_config status   */
  rrc_state_e_type                    current_state; /* current RRC State  */
  rrc_int_c_rnti_type                 c_rnti;       /* Local C_RNTI value */
  rrc_int_u_rnti_type                 u_rnti;
  rrcrb_directed_cell_info_type       directed_cell_info; /* directed cell info struct */

  rrcrb_rnti_update_info_type rnti_update_info;   /* to hold rnti update info */

  boolean       ordered_config_err_occured = FALSE; /* Used to indicate if an error
                                    occured while setting ordered_config. */

  msg_ptr = &dcch_msg_ptr->message.u.physicalChannelReconfiguration;

  if (FAILURE == 
      rrcpcrc_check_and_send_failure_for_simul_reconfig(
        msg_ptr->u.later_than_r3.rrc_TransactionIdentifier))
  {
    return;
  }

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  validate_pcrc_r6( pcrc_ptr );

  if( pcrc.status.failure_status != RRCPC_MSG_VALID )
  {
    /*  Message was invalid. */
    WRRC_MSG0_ERROR("PCRC Message validation failed");

    /* Send the PC Reconfig Failure Message */
    send_pcrc_failure_message( pcrc.status.failure_status,
                              pcrc.status.prot_err_cause
                            );

    /* Make sure we're back in the initial sub-state */
    pcrc.curr_substate = RRCRB_INITIAL;
  }
  /* If the message is valid, proceed further */
  else
  {

    MSG_LOW("Processing PCRC Message", 0, 0, 0);

    /* Check if there is CN information present. If so, we need
    to send the information to the Cell Change manager */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr, rrc_PhysicalChannelReconfiguration_r6_IEs,cn_InformationInfo))
    {
      WRRC_MSG0_HIGH("Saving CN info for NAS");
      pcrc.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(
                        &(pcrc_ptr->cn_InformationInfo));
 
      if (pcrc.mm_cmd_ptr == NULL)
      {
        WRRC_MSG0_ERROR("Could not build SERVICE_IND cmd");
      }
      else
      {
        /* set cn_info_saved flag */
        pcrc.cn_info_saved = TRUE;
      }
    }

    if (RRC_MSG_COMMON_BITMASK_IE_TYPE2(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
        criticalExtensions.u.r6,
        physicalChannelReconfiguration_r6_add_ext))
    {
      WRRC_MSG0_ERROR("RRCEUL: physicalChannelReconfiguration_r6_add_ext not yet supported");
    } 

    if (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
        criticalExtensions.u.r6.m.v6b0NonCriticalExtensionsPresent)
    {
 
      rrcpcrc_process_high_mobility_ind_in_non_critical_ext(msg_ptr,MSG_REL6);
         
    }
        
    /* Need to send reconfig IE info. First set ORDERED_CONFIG
    so that LLC can process the necessary parameters. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_PCR,
                                           pcrc.state_indicated_by_msg,
                                           rrc_DL_DCCH_Message_PDU,
                                           (void *)dcch_msg_ptr );

    switch( oc_status )
    {
      case ORDERED_CONFIG_SET:
        current_state = rrc_get_state(); 
        if((rrc_sib_is_current_event_sib_read_in_dch()) &&
           (RRC_PROCEDURE_GPS == rrc_sib_get_proc_id_for_sib_read_in_dch()))
        {
           WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
           rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
        }

       /* Populate the local variables to indicate if Re-direction info is present */
        directed_cell_info.freq_info_present = FALSE;
        directed_cell_info.pscr_present = FALSE;
       
        if((current_state==RRC_STATE_CELL_FACH) && 
               ((pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
               ||(pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
               ||(pcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          
          if((RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr, 
                    rrc_PhysicalChannelReconfiguration_r6_IEs,frequencyInfo)) &&
                   (RRC_CHECK_COMMON_MSG_TYPE(pcrc_ptr->frequencyInfo.
                    modeSpecificInfo,rrc_FrequencyInfo_modeSpecificInfo_fdd)))           
          {
            directed_cell_info.freq_info_present = TRUE;
            directed_cell_info.uarfcn_dl = pcrc_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
          }
          
          if (rrcpcrc_is_pcpich_info_present(msg_ptr, MSG_REL6))
          {
            directed_cell_info.pscr_present = TRUE;
            directed_cell_info.pscr = pcrc_ptr->dl_InformationPerRL_List.elem[0].
                                      modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
          }     
          /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
           FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
          if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
             (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
          {
            WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
            if(pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
            {
              rrcpcrc_send_initiate_cu_flag = TRUE;
              rrcpcrc_delete_crnti          = TRUE;
            }
            else
            {
              pcrc.directed_cell_info = TRUE;
            }
          }
        }

        if (
             (current_state==pcrc.state_indicated_by_msg)||
             ((current_state==RRC_STATE_CELL_FACH)&& 
              (pcrc.state_indicated_by_msg==RRC_STATE_CELL_DCH))
            )
        {
          /*  CellDCH<->CellDCH or CellFACH<->CellFACH or CellFACH->CellDCH 
          transition. Ordered Config has been set. We can setup the
          Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */

          /*check whether any frequency info is present in the message
          and transition is CELL_FACH->CELL_FACH. In this case
          initiate a cell update after channel config is successfully completed */
          if ((current_state==RRC_STATE_CELL_FACH)&& 
             (pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
          {
     
            /*call the function that evaluates the directed cell info and set the flags accordingly */
            if ((rrcpcrc_send_initiate_cu_flag == FALSE)&&
                (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
            {
              if(RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr, rrc_PhysicalChannelReconfiguration_r6_IEs,new_C_RNTI))
              {
                pcrc.new_crnti_valid = TRUE;
                rrc_translate_crnti(&pcrc_ptr->new_C_RNTI, 
                                    &c_rnti);
                pcrc.new_crnti = (uint16)c_rnti;
              }
              else
              {
                pcrc.new_crnti_valid = FALSE;
              }
   
              rrcllcoc_update_toc_with_oc_from_fach();
                 /*Initiate the cell selection*/
              rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_PCR,
                                                      RRC_TRANSITION_FROM_FACH,
                                                      pcrc.state_indicated_by_msg);
   
   
            
            
              pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
     
               /* exit from the switch case */
              break;
            }
    
          } /* if CELL_FACH->CELL_FACH reconfiguration */

          /* Call the function that creates and sends a channel
          config request to LLC. */
          rrcpcrc_send_channel_config_req( );
             /* Change the substate since we're waiting for the
             confirmation from LLC for the channel configs */
          pcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
        {
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr, rrc_PhysicalChannelReconfiguration_r6_IEs,new_C_RNTI))
          {
            pcrc.new_crnti_valid = TRUE;
            rrc_translate_crnti(&pcrc_ptr->new_C_RNTI, 
                                &c_rnti);
            pcrc.new_crnti = (uint16)c_rnti;
          }
          else
          {
            pcrc.new_crnti_valid = FALSE;
          }

          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_PCR,
                                                  RRC_TRANSITION_FROM_DCH,
                                                  RRC_STATE_CELL_FACH);
          pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

          /* Start T_305 [periodic cell update] timer */
          rrccu_start_t_305_timer();		  
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH))
        {
          pcrc.directed_cell_info = rrcpcrc_is_pcpich_info_present(msg_ptr, MSG_REL6);
         /*Send the PCRC complete msg on old configuration */
         send_pcrc_complete_message(TRUE);
         /*wait for L2ack */
         pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
         /* prints rate for each transport channel */
         rrcllc_print_trans_chnl_rate();
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (pcrc.state_indicated_by_msg==RRC_STATE_URA_PCH))
        {
          /*Send the PCRC complete msg on old configuration */
          send_pcrc_complete_message(TRUE);
          /*wait for L2ack */
          pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
          /* prints rate for each transport channel */
          rrcllc_print_trans_chnl_rate();
        }
        else if((current_state==RRC_STATE_CELL_FACH) && 
                ((pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
                 (pcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          /*call the function that evaluates the directed cell info and set the flags accordingly */
          if ((pcrc.directed_cell_info == FALSE) &&
            (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
          {
            if(pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
            {
              pcrc.directed_cell_info = rrcpcrc_is_pcpich_info_present(msg_ptr, MSG_REL6);
            }
       
            pcrc.initiate_cell_selection_trans_from_fach = TRUE;
          }


          /* check whether new C-RNTI was present in the message. In that case
          we have to configure MAC with new C-RNTI before sending the response 
          message */
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr, 
             rrc_PhysicalChannelReconfiguration_r6_IEs,new_C_RNTI))
          {
            rrc_translate_crnti(&pcrc_ptr->new_C_RNTI, 
                                &c_rnti);
            /* Send a RNTI Update Request */
            rnti_update_info.crnti_action    = RRC_RNTI_UPDATE;
            rnti_update_info.crnti           = (uint16)c_rnti;
            /* check whether URNTI is also present in the message */
            if (RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr,
             rrc_PhysicalChannelReconfiguration_r6_IEs,new_U_RNTI))
            {
              rrc_translate_urnti(&pcrc_ptr->new_U_RNTI, 
                                  &u_rnti);
              rnti_update_info.urnti_action = RRC_RNTI_UPDATE; 
              rnti_update_info.urnti        = u_rnti; 
            }
            else
            {
              rnti_update_info.urnti_action = RRC_RNTI_NOCHANGE; 
            }
            rnti_update_info.procedure            = RRC_PROCEDURE_PCR;
            rnti_update_info.rlc_re_establish_srb = FALSE;
            rnti_update_info.rlc_re_establish_rab = FALSE;
            rnti_update_info.cnf_reqd             = TRUE;

            rrcrb_send_rnti_update_req(rnti_update_info);
            pcrc.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
          }
          else 
          {
            /* There is no new C-RNTI So send the response on the old configuration
            */
            send_pcrc_complete_message(TRUE);
            /*wait for L2ack */
            pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
            /* prints rate for each transport channel */
            rrcllc_print_trans_chnl_rate();
          }
        }
        break;

      case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

        pcrc.status.failure_status = RRCPC_SIMULTANEOUS_RECONFIG;
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause );

        /* Go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

        pcrc.status.failure_status =RRCPC_MSG_UNSUPPORTED_CONFIG;
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause );

        /* Go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_INVALID:

        pcrc.status.failure_status =RRCPC_MSG_INVALID_CONFIG;
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause );

        /* Go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
		
      case ORDERED_CONFIG_NOT_SET_OTHER:
        WRRC_MSG0_HIGH("LLC returned other failure!");
        /* When LLC sends this, there is a serious problem
        and we'll go back to disconnected state. No need
        to send the failure message here, we can just
        go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME      
        case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
          WRRC_MSG0_HIGH("PCR processing deferred due to LTA!");
          pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/

      default:

        WRRC_MSG0_ERROR("Unknown return value from LLC");
        /* Unknown error, just go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
    }/* end switch */

    if( ordered_config_err_occured == TRUE )
    {
      /* No need to continue. Clear the procedure
      variables and return. */
      rrcpcrc_clear_procedure();
      return;
    }

    if(pcrc.pcrc_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }
  }
}



/*===========================================================================

FUNCTION   PROCESS_PCRC_R7

DESCRIPTION

  This function processes the received Rel 6 PC Reconfig Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a PCRC Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to reconfigure the Physical Channel given by the message.
    
    If ORDERED_CONFIG could not be set, a PCRC Failure message
    will be sent to UTRAN.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_pcrc_r7
(
  rrc_RRC_TransactionIdentifier transaction_id,

  rrc_DL_DCCH_Message *dcch_msg_ptr  ,
  /* Pointer to the RB Reconfig message        */
  rrc_PhysicalChannelReconfiguration_r7_IEs *pcrc_ptr
)
{
  rrc_PhysicalChannelReconfiguration *msg_ptr = NULL;		/* Pointer to the RB Setup message        */

  rrcllc_oc_status_e_type             oc_status;/* To hold ordered_config status   */
  rrc_state_e_type                    current_state; /* current RRC State  */
  rrc_int_c_rnti_type                 c_rnti;       /* Local C_RNTI value */
  rrc_int_u_rnti_type                 u_rnti;
  rrcrb_directed_cell_info_type       directed_cell_info; /* directed cell info struct */

  rrcrb_rnti_update_info_type rnti_update_info;   /* to hold rnti update info */

  boolean       ordered_config_err_occured = FALSE; /* Used to indicate if an error
                                    occured while setting ordered_config. */

  msg_ptr = &dcch_msg_ptr->message.u.physicalChannelReconfiguration;
WRRC_MSG1_HIGH("Received PCRC msg rel 7, substate %d", pcrc.curr_substate);
  if (FAILURE == 
      rrcpcrc_check_and_send_failure_for_simul_reconfig(
        msg_ptr->u.later_than_r3.rrc_TransactionIdentifier))
  {
    return;
  }

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  validate_pcrc_r7( pcrc_ptr );

  if( pcrc.status.failure_status != RRCPC_MSG_VALID )
  {
    /*  Message was invalid. */
    WRRC_MSG0_ERROR("PCRC Message validation failed");

    /* Send the PC Reconfig Failure Message */
    send_pcrc_failure_message( pcrc.status.failure_status,
                              pcrc.status.prot_err_cause
                            );

    /* Make sure we're back in the initial sub-state */
    pcrc.curr_substate = RRCRB_INITIAL;
  }
  /* If the message is valid, proceed further */
  else
  {

    MSG_LOW("Processing PCRC Message", 0, 0, 0);

    /* Check if there is CN information present. If so, we need
    to send the information to the Cell Change manager */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr, rrc_PhysicalChannelReconfiguration_r7_IEs,cn_InformationInfo))
    {
      WRRC_MSG0_HIGH("Saving CN info for NAS");
      pcrc.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(
                        &(pcrc_ptr->cn_InformationInfo));
 
      if (pcrc.mm_cmd_ptr == NULL)
      {
        WRRC_MSG0_ERROR("Could not build SERVICE_IND cmd");
      }
      else
      {
        /* set cn_info_saved flag */
        pcrc.cn_info_saved = TRUE;
      }
    }
      
 
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr, rrc_PhysicalChannelReconfiguration_r7_IEs,ueMobilityStateIndicator))
    {
      if(pcrc_ptr->ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
      {
        WRRC_MSG0_HIGH("High Mobility Ind Set to TRUE in PCRC.");
        pcrc.high_mobility_ind = TRUE;
      }
    }

#ifdef FEATURE_WCDMA_REL7_SPECRS
     /*If PCRC OTA provides "supportForChangeOfUE_Capability" IE, we need to store it in
          "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
          "rrc_rnc_cap_change_support" can be updated accordingly. */  	
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr, rrc_PhysicalChannelReconfiguration_r7_IEs,supportForChangeOfUE_Capability))
    {
      if(pcrc_ptr->supportForChangeOfUE_Capability == TRUE)
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




    /*If PCRC OTA provides "responseToChangeOfUE_Capability_present" IE, we need to store it so that later
        if procedure is successful, PCRC need to notify the COUEC procedure  about this IE.*/
        
    rrc_change_of_ue_cap_response = FALSE;
    
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr, rrc_PhysicalChannelReconfiguration_r7_IEs,responseToChangeOfUE_Capability))
    {
      rrc_change_of_ue_cap_response = TRUE;
    }    
#endif

    if (RRC_MSG_COMMON_BITMASK_IE_TYPE2(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7,
        physicalChannelReconfiguration_r7_add_ext))
       
    {
      WRRC_MSG0_ERROR("RRCEUL: rrc_physicalChannelReconfiguration_r7_add_ext_present not yet supported");
    } 

    if ((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
		criticalExtensions.u.criticalExtensions.u.r7.m.v780NonCriticalExtensionsPresent)
        &&(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
        u.criticalExtensions.u.r7.v780NonCriticalExtensions.m.v7d0NonCriticalExtensionsPresent))
    {
      WRRC_MSG0_ERROR("RRCEUL: r7_v780NonCriticalExtensions_nonCriticalExtensions_present not yet supported");
    }
    /* Need to send reconfig IE info. First set ORDERED_CONFIG
    so that LLC can process the necessary parameters. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_PCR,
                                           pcrc.state_indicated_by_msg,
                                           rrc_DL_DCCH_Message_PDU,
                                           (void *)dcch_msg_ptr );

    switch( oc_status )
    {
      case ORDERED_CONFIG_SET:
        current_state = rrc_get_state(); 
        
        if(rrc_sib_is_current_event_sib_read_in_dch())
        {
          WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
          rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
        }
        /* Populate the local variables to indicate if Re-direction info is present */
        directed_cell_info.freq_info_present = FALSE;
        directed_cell_info.pscr_present = FALSE;
        if((current_state==RRC_STATE_CELL_FACH) && 
                 ((pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
                 ||(pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
                 ||(pcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
           
          if((RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr, 
              rrc_PhysicalChannelReconfiguration_r7_IEs,frequencyInfo)) &&
             (RRC_CHECK_COMMON_MSG_TYPE(pcrc_ptr->frequencyInfo.
              modeSpecificInfo,rrc_FrequencyInfo_modeSpecificInfo_fdd)))           
          {
            directed_cell_info.freq_info_present = TRUE;
            directed_cell_info.uarfcn_dl = pcrc_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
          }
          
          if (rrcpcrc_is_pcpich_info_present(msg_ptr, MSG_REL7))
          {
            directed_cell_info.pscr_present = TRUE;
            directed_cell_info.pscr = pcrc_ptr->dl_InformationPerRL_List.elem[0].
                                      modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
          }
          /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
          FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
          if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
            (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
          {
            WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
            if(pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
            {
              rrcpcrc_send_initiate_cu_flag = TRUE;
              rrcpcrc_delete_crnti          = TRUE;
            }
            else
            {
              pcrc.directed_cell_info = TRUE;
            }
          }          
        }
        if (
            (current_state==pcrc.state_indicated_by_msg)||
            ((current_state==RRC_STATE_CELL_FACH)&& 
             (pcrc.state_indicated_by_msg==RRC_STATE_CELL_DCH))
           )
        {
          /*  CellDCH<->CellDCH or CellFACH<->CellFACH or CellFACH->CellDCH 
          transition. Ordered Config has been set. We can setup the
          Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */

          /*check whether any frequency info is present in the message
          and transition is CELL_FACH->CELL_FACH. In this case
          initiate a cell update after channel config is successfully completed */
          if ((current_state==RRC_STATE_CELL_FACH)&& 
             (pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
          {
     
            /*call the function that evaluates the directed cell info and set the flags accordingly */
            if ((rrcpcrc_send_initiate_cu_flag == FALSE)&&
                (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
            {
    
              if(RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr, rrc_PhysicalChannelReconfiguration_r7_IEs,new_C_RNTI))
              {
                pcrc.new_crnti_valid = TRUE;
                rrc_translate_crnti(&pcrc_ptr->new_C_RNTI, 
                                    &c_rnti);
                pcrc.new_crnti = (uint16)c_rnti;
              }
              else
              {
                pcrc.new_crnti_valid = FALSE;
              }
              /* Now copy OC to TOC */
              rrcllcoc_update_toc_with_oc_from_fach();
              /*Initiate the cell selection*/
              rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_PCR,
                                                       RRC_TRANSITION_FROM_FACH,
                                                       pcrc.state_indicated_by_msg);
    
        
              pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
     
              /* exit from the switch case */
              break;
            }
    
          } /* if CELL_FACH->CELL_FACH reconfiguration */

          /* Call the function that creates and sends a channel
          config request to LLC. */
          rrcpcrc_send_channel_config_req( );
             /* Change the substate since we're waiting for the
             confirmation from LLC for the channel configs */
          pcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
        {
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr, rrc_PhysicalChannelReconfiguration_r7_IEs,new_C_RNTI))
          {
            pcrc.new_crnti_valid = TRUE;
            rrc_translate_crnti(&pcrc_ptr->new_C_RNTI, 
                                &c_rnti);
            pcrc.new_crnti = (uint16)c_rnti;
          }
          else
          {
            pcrc.new_crnti_valid = FALSE;
          }

          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_PCR,
                                                  RRC_TRANSITION_FROM_DCH,
                                                  RRC_STATE_CELL_FACH);
          pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

          /* Start T_305 [periodic cell update] timer */
          rrccu_start_t_305_timer();		  
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH))
        {
          pcrc.directed_cell_info = rrcpcrc_is_pcpich_info_present(msg_ptr, MSG_REL7);
         /*Send the PCRC complete msg on old configuration */
         send_pcrc_complete_message(TRUE);
         /*wait for L2ack */
         pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
         /* prints rate for each transport channel */
         rrcllc_print_trans_chnl_rate();
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (pcrc.state_indicated_by_msg==RRC_STATE_URA_PCH))
        {
          /*Send the PCRC complete msg on old configuration */
          send_pcrc_complete_message(TRUE);
          /*wait for L2ack */
          pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
          /* prints rate for each transport channel */
          rrcllc_print_trans_chnl_rate();
        }
        else if((current_state==RRC_STATE_CELL_FACH) && 
                ((pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
                 (pcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          /*call the function that evaluates the directed cell info and set the flags accordingly */
          if ((pcrc.directed_cell_info == FALSE) &&
            (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
          {
            if(pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
            {
              pcrc.directed_cell_info = rrcpcrc_is_pcpich_info_present(msg_ptr, MSG_REL7);
            }
          
            pcrc.initiate_cell_selection_trans_from_fach = TRUE;
          }
          

          /* check whether new C-RNTI was present in the message. In that case
          we have to configure MAC with new C-RNTI before sending the response 
          message */
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr, 
             rrc_PhysicalChannelReconfiguration_r7_IEs,new_C_RNTI))
          {
            rrc_translate_crnti(&pcrc_ptr->new_C_RNTI, 
                                &c_rnti);
            /* Send a RNTI Update Request */
            rnti_update_info.crnti_action    = RRC_RNTI_UPDATE;
            rnti_update_info.crnti           = (uint16)c_rnti;
            /* check whether URNTI is also present in the message */
            if (RRC_MSG_COMMON_BITMASK_IE_PTR(pcrc_ptr,
             rrc_PhysicalChannelReconfiguration_r7_IEs,new_U_RNTI))
            {
              rrc_translate_urnti(&pcrc_ptr->new_U_RNTI, 
                                  &u_rnti);
              rnti_update_info.urnti_action = RRC_RNTI_UPDATE; 
              rnti_update_info.urnti        = u_rnti; 
            }
            else
            {
              rnti_update_info.urnti_action = RRC_RNTI_NOCHANGE; 
            }
            rnti_update_info.procedure            = RRC_PROCEDURE_PCR;
            rnti_update_info.rlc_re_establish_srb = FALSE;
            rnti_update_info.rlc_re_establish_rab = FALSE;
            rnti_update_info.cnf_reqd             = TRUE;

            rrcrb_send_rnti_update_req(rnti_update_info);
            pcrc.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
          }
          else 
          {
            /* There is no new C-RNTI So send the response on the old configuration
            */
            send_pcrc_complete_message(TRUE);
            /*wait for L2ack */
            pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
            /* prints rate for each transport channel */
            rrcllc_print_trans_chnl_rate();
          }
        }
        break;

      case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

        pcrc.status.failure_status = RRCPC_SIMULTANEOUS_RECONFIG;
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause );

        /* Go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

        pcrc.status.failure_status =RRCPC_MSG_UNSUPPORTED_CONFIG;
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause );

        /* Go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_INVALID:

        pcrc.status.failure_status =RRCPC_MSG_INVALID_CONFIG;
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause );

        /* Go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
      
      case ORDERED_CONFIG_NOT_SET_OTHER:
        WRRC_MSG0_HIGH("LLC returned other failure!");
        /* When LLC sends this, there is a serious problem
        and we'll go back to disconnected state. No need
        to send the failure message here, we can just
        go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME         
      case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("PCR processing deferred due to LTA!");
        pcrc.curr_substate = RRCRB_INITIAL;
      
      /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/

      default:

        WRRC_MSG0_ERROR("Unknown return value from LLC");
        /* Unknown error, just go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
    }/* end switch */

    if( ordered_config_err_occured == TRUE )
    {
      /* No need to continue. Clear the procedure
      variables and return. */
      rrcpcrc_clear_procedure();
      return;
    }

    if(pcrc.pcrc_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }
  }
}

/*===========================================================================
FUNCTION   rrcpcrc_process_after_oc_config_set_for_fach_to_fach_r8

DESCRIPTION
  This function processes the R8 PCRC OTA after OC is set by this OTA and When FACH to FACH
  state transition is triggered by the OTA.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean rrcpcrc_process_after_oc_config_set_for_fach_to_fach_r8
(
  rrc_PhysicalChannelReconfiguration * msg_ptr,
  rrcrb_directed_cell_info_type  directed_cell_info
)
{
  rrc_int_c_rnti_type                 c_rnti;           /* Local C_RNTI value */
  rrc_PhysicalChannelReconfiguration_r8_IEs *pcrc_ptr = NULL;

  pcrc_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
          criticalExtensions.u.criticalExtensions.u.r8.physicalChannelReconfiguration_r8;


  /*call the function that evaluates the directed cell info and set the flags accordingly */
  if ((rrcpcrc_send_initiate_cu_flag == FALSE)&&
    (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
  {
    if(RRCRB_R8_MSG_IE_PRESENT(pcrc_ptr,rrc_PhysicalChannelReconfiguration_r8_IEs,new_C_RNTI))
    {
      pcrc.new_crnti_valid = TRUE;
      rrc_translate_crnti(&pcrc_ptr->new_C_RNTI, &c_rnti);
      pcrc.new_crnti = (uint16)c_rnti;
    }
    else
    {
      pcrc.new_crnti_valid = FALSE;
    }

    /* Now copy OC to TOC */
    rrcllcoc_update_toc_with_oc_from_fach();
    /*Initiate the cell selection*/
    rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_PCR, 
                   RRC_TRANSITION_FROM_FACH, pcrc.state_indicated_by_msg);
  
    pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
	return TRUE;
  }

  return FALSE;
}

/*===========================================================================
FUNCTION   rrcpcrc_process_after_oc_config_set_r8

DESCRIPTION
  This function processes the R8 PCRC OTA after OC is set by this OTA. So if we call this function, it means
  that ORDERED_CONFIG_SET is returned while setting OC by PCRC OTA.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void rrcpcrc_process_after_oc_config_set_r8
(
  rrc_DL_DCCH_Message *dcch_msg_ptr,
  rrc_PhysicalChannelReconfiguration_r8_IEs *pcrc_ptr  
)
{
  rrc_int_c_rnti_type                 c_rnti;           /* Local C_RNTI value */
  rrc_int_u_rnti_type                 u_rnti;           /* Local U_RNTI value*/
  rrc_state_e_type                    current_state;              /* current RRC State  */  
  rrcrb_directed_cell_info_type  directed_cell_info;        /* directed cell info struct */
  rrcrb_rnti_update_info_type    rnti_update_info;             /* to hold rnti update info */  
  rrc_PhysicalChannelReconfiguration *msg_ptr = NULL;  /* Pointer to the RB Setup message */

  current_state = rrc_get_state(); 

  msg_ptr = &dcch_msg_ptr->message.u.physicalChannelReconfiguration;
  
  if(rrc_sib_is_current_event_sib_read_in_dch())
  {
    WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
    rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
  }

  /* Populate the local variables to indicate if Re-direction info is present */
  directed_cell_info.freq_info_present = FALSE;
  directed_cell_info.pscr_present = FALSE;

  if((current_state==RRC_STATE_CELL_FACH) && 
      ((pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
      ||(pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
      ||(pcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
  {
    if((RRCRB_R8_MSG_IE_PRESENT(pcrc_ptr,rrc_PhysicalChannelReconfiguration_r8_IEs,frequencyInfo)) &&
      (RRC_CHECK_COMMON_MSG_TYPE(pcrc_ptr->frequencyInfo.modeSpecificInfo,
      rrc_FrequencyInfo_modeSpecificInfo_fdd)))           
    {
      directed_cell_info.freq_info_present = TRUE;
      directed_cell_info.uarfcn_dl = pcrc_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
    }
    if (rrcpcrc_is_pcpich_info_present(msg_ptr, MSG_REL8))
    {
      directed_cell_info.pscr_present = TRUE;
      directed_cell_info.pscr = pcrc_ptr->dl_InformationPerRL_List.elem[0].
                              modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
    }
    /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
    FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
    if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
       (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
    {
      WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
      if(pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
      {
        rrcpcrc_send_initiate_cu_flag = TRUE;
        rrcpcrc_delete_crnti          = TRUE;
      }
      else
      {
        pcrc.directed_cell_info = TRUE;
      }
    }
  }

  /* CellDCH<->CellDCH or CellFACH<->CellFACH or CellFACH->CellDCH transition. */
  if((current_state==pcrc.state_indicated_by_msg)||((current_state==RRC_STATE_CELL_FACH) && 
    (pcrc.state_indicated_by_msg==RRC_STATE_CELL_DCH)))
  {
    if ((current_state==RRC_STATE_CELL_FACH)&& 
     (pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
    {
      if(rrcpcrc_process_after_oc_config_set_for_fach_to_fach_r8(msg_ptr, directed_cell_info))	
      return;
    }/* if CELL_FACH->CELL_FACH reconfiguration */


    /* Call the function that creates and sends a channel config request to LLC. */
    rrcpcrc_send_channel_config_req( );

    /* Change the substate since we're waiting for the confirmation from LLC for the channel configs */
    pcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
  }

  /*Cell_DCH -> Cell_FACH*/
  else if((current_state==RRC_STATE_CELL_DCH) && 
    (pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
  {
    if(RRCRB_R8_MSG_IE_PRESENT(pcrc_ptr,rrc_PhysicalChannelReconfiguration_r8_IEs,new_C_RNTI))
    {
      pcrc.new_crnti_valid = TRUE;
      rrc_translate_crnti(&pcrc_ptr->new_C_RNTI, &c_rnti);
      pcrc.new_crnti = (uint16)c_rnti;
    }
    else
    {
      pcrc.new_crnti_valid = FALSE;
    }

    /*Initiate the cell selection*/
    rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_PCR,
                  RRC_TRANSITION_FROM_DCH, RRC_STATE_CELL_FACH);
    pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
    
    /* Start T_305 [periodic cell update] timer */
    rrccu_start_t_305_timer();
  }

  /*Cell_DCH -> Cell_PCH*/
  else if((current_state==RRC_STATE_CELL_DCH) && 
      (pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH))
  {
    pcrc.directed_cell_info = rrcpcrc_is_pcpich_info_present(msg_ptr, MSG_REL8);

    /*Send the PCRC complete msg on old configuration */
    send_pcrc_complete_message(TRUE);

    /*wait for L2ack */
    pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
    /* prints rate for each transport channel */
    rrcllc_print_trans_chnl_rate();
  }

  /*Cell_DCH -> URA_PCH*/
  else if((current_state==RRC_STATE_CELL_DCH) && 
      (pcrc.state_indicated_by_msg==RRC_STATE_URA_PCH))
  {
    /*Send the PCRC complete msg on old configuration */
    send_pcrc_complete_message(TRUE);
    /*wait for L2ack */
    pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
    /* prints rate for each transport channel */
    rrcllc_print_trans_chnl_rate();
  }

  /*Cell_FACH -> Cell_PCH or Cell_FACH ->URA_PCH*/
  else if((current_state==RRC_STATE_CELL_FACH) && 
      ((pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
      (pcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
  {
    /*call the function that evaluates the directed cell info and set the flags accordingly */
    if((pcrc.directed_cell_info == FALSE) &&
      (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
    {
      if(pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
      {
        pcrc.directed_cell_info = rrcpcrc_is_pcpich_info_present(msg_ptr, MSG_REL8);
      }
      pcrc.initiate_cell_selection_trans_from_fach = TRUE;
    }
  
    /* check whether new C-RNTI was present in the message. In that case
    we have to configure MAC with new C-RNTI before sending the response message */
    if(RRCRB_R8_MSG_IE_PRESENT(pcrc_ptr,rrc_PhysicalChannelReconfiguration_r8_IEs,new_C_RNTI))
    {
      rrc_translate_crnti(&pcrc_ptr->new_C_RNTI, &c_rnti);

      /* Send a RNTI Update Request */
      rnti_update_info.crnti_action    = RRC_RNTI_UPDATE;
      rnti_update_info.crnti           = (uint16)c_rnti;

      /* check whether URNTI is also present in the message */
      if(RRCRB_R8_MSG_IE_PRESENT(pcrc_ptr,rrc_PhysicalChannelReconfiguration_r8_IEs,new_U_RNTI))
      {
        rrc_translate_urnti(&pcrc_ptr->new_U_RNTI, &u_rnti);
        rnti_update_info.urnti_action = RRC_RNTI_UPDATE; 
        rnti_update_info.urnti        = u_rnti; 
      }
      else
      {
        rnti_update_info.urnti_action = RRC_RNTI_NOCHANGE; 
      }
      rnti_update_info.procedure            = RRC_PROCEDURE_PCR;
      rnti_update_info.rlc_re_establish_srb = FALSE;
      rnti_update_info.rlc_re_establish_rab = FALSE;
      rnti_update_info.cnf_reqd             = TRUE;

      rrcrb_send_rnti_update_req(rnti_update_info);
      pcrc.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
    }
    else 
    {
      /* There is no new C-RNTI So send the response on the old configuration */
      send_pcrc_complete_message(TRUE);
      /*wait for L2ack */
      pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
      /* prints rate for each transport channel */
      rrcllc_print_trans_chnl_rate();
    }
  }
  return;
}



/*===========================================================================
FUNCTION   validate_pcrc_r8

DESCRIPTION
  This function validates the received PC Reconfig Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a PCRC Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void validate_pcrc_r8
(
  rrc_PhysicalChannelReconfiguration_r8_IEs *msg_ptr /* Pointer to the PCRC message */
)
{
  /* Assume the message is valid for now */
  pcrc.status.failure_status = RRCPC_MSG_VALID;

  /* Check the RRC State indicatior given by the message */
  switch( msg_ptr->rrc_StateIndicator )
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      pcrc.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      pcrc.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      pcrc.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;

    case rrc_RRC_StateIndicator_ura_PCH:
      pcrc.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;

    default:
      /* No other states are supported right now. */
      WRRC_MSG0_HIGH("Unsupported state in PC Reconfig Message");
      pcrc.status.failure_status = RRCPC_MSG_UNSUPPORTED_CONFIG;
      break;
  }

  if( pcrc.status.failure_status == RRCPC_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
     return;
  }

  /*RRC_GKG: We can put a check for HSDPA, HSUPA for TDD and return failure.*/
 
   /* Check if RB with PDCP Information list is present. This
  is not supported in the first release. This may be supported
  later */                                    
  if(RRCRB_R8_MSG_IE_PRESENT(msg_ptr,rrc_PhysicalChannelReconfiguration_r8_IEs,
    dl_CounterSynchronisationInfo))
  {
    /*check whether the SRNS relocation is within Cell_DCH or not.
     SRNS relocation information can come in reconfiguration messages
     only when the state transition is Cell_DCH->Cell_DCH */
    if (!((pcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("PCRC : SRNS reloc with invalid states cur_state %d, msg_state %d",
             rrc_get_state(), pcrc.state_indicated_by_msg);
      pcrc.status.failure_status=RRCPC_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has 
    PDCP info list. If it is true then this configuration is unsupported */
    if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2(msg_ptr->dl_CounterSynchronisationInfo,
		rb_WithPDCP_InfoList)) ||
        (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->dl_CounterSynchronisationInfo,
         rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
    {
      WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
      pcrc.status.failure_status=RRCPC_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation. Set the flag that indicates that this procedure
      involves SRNS relocation */
      pcrc.pcrc_for_srns_relocation = TRUE;
      /* store if new U-RNTI is present in the message */
      if(RRCRB_R8_MSG_IE_PRESENT(msg_ptr,rrc_PhysicalChannelReconfiguration_r8_IEs,
        new_U_RNTI))
      {
        pcrc.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(msg_ptr->new_U_RNTI), &pcrc.new_urnti);
      }
    }
  }

    /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  pcrc.trans_to_dch_ciph_config_needed = FALSE;

  if((pcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
    (rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRCRB_R8_MSG_IE_PRESENT(msg_ptr,rrc_PhysicalChannelReconfiguration_r8_IEs,
      dl_CommonInformation)) && (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->dl_CommonInformation,
      rrc_DL_CommonInformation_r8,dl_dpchInfoCommon)) && (T_rrc_DL_CommonInformation_r8_dl_dpchInfoCommon_dl_DPCH_InfoCommon
      == msg_ptr->dl_CommonInformation.dl_dpchInfoCommon.t))
    {
      //check if ciphering is active in mac for cs domain, if it is then,
      //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      if(rrcsmc_tm_ciph_active() == TRUE)
      {
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        uecomdef_status_e_type loc_ret_val;

        WRRC_MSG0_HIGH("To-DCH-Step-1 config activated");

        loc_ret_val = rrcrb_get_mac_d_hfn(( 
          RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.dl_DPCH_InfoCommon,
          rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
          &msg_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
          &mac_d_hfn_l);

        if (loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("get_mac_d_hfn failed");
        }
        if(rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
        {
           WRRC_MSG0_HIGH("SMC process ciphering config for HHO failed");
        }
        pcrc.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }

  /*Cell_PCH/URA_PCH specific validations */
  if ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
    /* If UTRAN DRX coefficient is not present then it is invalid configuration */
    if (!(RRCRBR_R7_MSG_IE_PRESENT(msg_ptr,rrc_PhysicalChannelReconfiguration_r8_IEs,
      utran_DRX_CycleLengthCoeff)))
    {
      WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", pcrc.state_indicated_by_msg);
      pcrc.status.failure_status = RRCPC_MSG_INVALID_CONFIG;
      return;
    }

    if (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
    {
      /*should we do it after getting l2ack for response??*/
      if (!(RRCRBR_R7_MSG_IE_PRESENT(msg_ptr,rrc_PhysicalChannelReconfiguration_r8_IEs,ura_Identity)))
      {
        WRRC_MSG0_HIGH("URA ID not present");
        rrccu_clear_ura_id ();
      }
      else
      {
        /*URA ID present in the message set ura_identity variable with this value */
        if (msg_ptr->ura_Identity.numbits == 16)
        {
          rrccu_update_ura_id (&(msg_ptr->ura_Identity));
        }
        else
        {
          WRRC_MSG1_ERROR("Invalid URA ID length%d", msg_ptr->ura_Identity.numbits);
          pcrc.status.failure_status = RRCPC_MSG_PROTOCOL_ERROR;
          pcrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
          return;
        }
      }
    }
  }
}/* end validate_pcrc_r7() */

/*===========================================================================
FUNCTION   PROCESS_PCRC_R8

DESCRIPTION
  This function processes the received Rel 8 PC Reconfig Message. It first validates the message. 
  If the message has an invalid or unsupported configuration, a PCRC Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -

    ORDERED_CONFIG will be set by calling LLC. 

    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG_REQ 
    will be sent to LLC to reconfigure the Physical Channel given by the message.
    
    If ORDERED_CONFIG could not be set, a PCRC Failure message
    will be sent to UTRAN.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void process_pcrc_r8
(
  rrc_RRC_TransactionIdentifier transaction_id,
  rrc_DL_DCCH_Message *dcch_msg_ptr  ,
  rrc_PhysicalChannelReconfiguration_r8_IEs *pcrc_ptr   /* Pointer to the R8 PC Reconfig message */
)
{
  rrc_PhysicalChannelReconfiguration *msg_ptr = NULL;  /* Pointer to the RB Setup message */
  rrcllc_oc_status_e_type          oc_status;                     /* To hold ordered_config status   */

  /* Used to indicate if an error occured while setting ordered_config. */
  boolean       ordered_config_err_occured = FALSE; 
  
  msg_ptr = &dcch_msg_ptr->message.u.physicalChannelReconfiguration;
  WRRC_MSG1_HIGH("REL8: Received PC-Reconfig OTA, PCRC-substate %d", pcrc.curr_substate);

  if (FAILURE == rrcpcrc_check_and_send_failure_for_simul_reconfig(
        msg_ptr->u.later_than_r3.rrc_TransactionIdentifier))
  {
    return;
  }

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  validate_pcrc_r8( pcrc_ptr );

  if( pcrc.status.failure_status != RRCPC_MSG_VALID )
  {
    /*  Message was invalid. */
    WRRC_MSG0_ERROR("REL8: PCRC Message validation failed");

    /* Send the PC Reconfig Failure Message */
    send_pcrc_failure_message( pcrc.status.failure_status, pcrc.status.prot_err_cause);

    /* Make sure we're back in the initial sub-state */
    pcrc.curr_substate = RRCRB_INITIAL;
  }
  /* If the message is valid, proceed further */
  else
  {
    MSG_LOW("Processing PCRC Message", 0, 0, 0);

    /* Check if there is CN information present. If so, we need to send the information to the 
    Cell Change manager */
    if(RRCRB_R8_MSG_IE_PRESENT(pcrc_ptr,rrc_PhysicalChannelReconfiguration_r8_IEs,
      cn_InformationInfo))
    {
      WRRC_MSG0_HIGH("Saving CN info for NAS");
      pcrc.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(
          &(pcrc_ptr->cn_InformationInfo));
 
      if (pcrc.mm_cmd_ptr == NULL)
      {
        WRRC_MSG0_ERROR("Could not build SERVICE_IND cmd");
      }
      else
      {
        /* set cn_info_saved flag */
        pcrc.cn_info_saved = TRUE;
      }
    }

    /*Check whether Mobility State Indicator is present in OTA, if yes, then store it PCRC global.
    Later we will inform L1 about this indicator.*/
    if(RRCRB_R8_MSG_IE_PRESENT(pcrc_ptr,rrc_PhysicalChannelReconfiguration_r8_IEs,
      ueMobilityStateIndicator))
    {
      if(pcrc_ptr->ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
      {
        WRRC_MSG0_HIGH("High Mobility Ind Set to TRUE in PCRC.");
        pcrc.high_mobility_ind = TRUE;
      }
    }

    /*Support for "Change of UE Capability" is under this feature*/
#ifdef FEATURE_WCDMA_REL7_SPECRS
    /*If PCRC OTA provides "supportForChangeOfUE_Capability" IE, we need to store it in
    "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
    "rrc_rnc_cap_change_support" can be updated accordingly. */
    if(RRCRB_R8_MSG_IE_PRESENT(pcrc_ptr,rrc_PhysicalChannelReconfiguration_r8_IEs,
      supportForChangeOfUE_Capability))
    {
      if(pcrc_ptr->supportForChangeOfUE_Capability == TRUE)
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

    /*If PCRC OTA provides "responseToChangeOfUE_Capability_present" IE, we need to store it so that later
    if procedure is successful, PCRC need to notify the COUEC procedure  about this IE.*/
    rrc_change_of_ue_cap_response = FALSE;
    
    if(RRCRB_R8_MSG_IE_PRESENT(pcrc_ptr,rrc_PhysicalChannelReconfiguration_r8_IEs,
      responseToChangeOfUE_Capability))
    {
      rrc_change_of_ue_cap_response = TRUE;
    }    
#endif /*FEATURE_WCDMA_REL7_SPECRS*/

    /* Need to send reconfig IE info. First set ORDERED_CONFIG
    so that LLC can process the necessary parameters. */
    oc_status = rrcllc_set_ordered_config(RRC_PROCEDURE_PCR, pcrc.state_indicated_by_msg, 
                                                            rrc_DL_DCCH_Message_PDU,(void *)dcch_msg_ptr );
    switch(oc_status)
    {
      case ORDERED_CONFIG_SET:
        rrcpcrc_process_after_oc_config_set_r8(dcch_msg_ptr,pcrc_ptr);
        break;

      case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

        pcrc.status.failure_status = RRCPC_SIMULTANEOUS_RECONFIG;
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause );

        /* Go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:
        pcrc.status.failure_status =RRCPC_MSG_UNSUPPORTED_CONFIG;
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause );

        /* Go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_INVALID:

        pcrc.status.failure_status =RRCPC_MSG_INVALID_CONFIG;
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause );

        /* Go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
      
      case ORDERED_CONFIG_NOT_SET_OTHER:
        WRRC_MSG0_HIGH("LLC returned other failure!");
        /* When LLC sends this, there is a serious problem
        and we'll go back to disconnected state. No need
        to send the failure message here, we can just
        go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME         
      case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("PCR processing deferred due to LTA!");
        pcrc.curr_substate = RRCRB_INITIAL;
      
      /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/

      default:

        WRRC_MSG0_ERROR("Unknown return value from LLC");
        /* Unknown error, just go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
    }/* end switch */

    if(ordered_config_err_occured == TRUE)
    {
      /* No need to continue. Clear the procedure variables and return. */
      rrcpcrc_clear_procedure();
      return;
    }
    else
    {
      if(pcrc.pcrc_for_srns_relocation == TRUE)
      {
        rrcsmc_commit_fresh_for_srns_relocation();
      }
    }
  }
}

#ifdef FEATURE_WCDMA_REL9

/*===========================================================================
FUNCTION   rrcpcrc_process_after_oc_config_set_for_fach_to_fach_r9

DESCRIPTION
  This function processes the R8 PCRC OTA after OC is set by this OTA and When FACH to FACH
  state transition is triggered by the OTA.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean rrcpcrc_process_after_oc_config_set_for_fach_to_fach_r9
(
  rrc_PhysicalChannelReconfiguration * msg_ptr,
  rrcrb_directed_cell_info_type  directed_cell_info
)
{
  rrc_int_c_rnti_type                 c_rnti;           /* Local C_RNTI value */
  rrc_PhysicalChannelReconfiguration_r9_IEs *pcrc_ptr = NULL;

  pcrc_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
          criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.physicalChannelReconfiguration_r9;


  /*call the function that evaluates the directed cell info and set the flags accordingly */
  if ((rrcpcrc_send_initiate_cu_flag == FALSE)&&
    (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
  {
    if(RRCRB_R9_MSG_IE_PRESENT(pcrc_ptr,new_C_RNTI))
    {
      pcrc.new_crnti_valid = TRUE;
      rrc_translate_crnti(&pcrc_ptr->new_C_RNTI, &c_rnti);
      pcrc.new_crnti = (uint16)c_rnti;
    }
    else
    {
      pcrc.new_crnti_valid = FALSE;
    }

    /* Now copy OC to TOC */
    rrcllcoc_update_toc_with_oc_from_fach();
    /*Initiate the cell selection*/
    rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_PCR, 
                   RRC_TRANSITION_FROM_FACH, pcrc.state_indicated_by_msg);
  
    pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
  return TRUE;
  }

  return FALSE;
}



/*===========================================================================
FUNCTION   rrcpcrc_process_after_oc_config_set_r9

DESCRIPTION
  This function processes the R8 PCRC OTA after OC is set by this OTA. So if we call this function, it means
  that ORDERED_CONFIG_SET is returned while setting OC by PCRC OTA.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void rrcpcrc_process_after_oc_config_set_r9
(
  rrc_DL_DCCH_Message *dcch_msg_ptr,
  rrc_PhysicalChannelReconfiguration_r9_IEs *pcrc_ptr  
)
{
  rrc_int_c_rnti_type                 c_rnti;           /* Local C_RNTI value */
  rrc_int_u_rnti_type                 u_rnti;           /* Local U_RNTI value*/
  rrc_state_e_type                    current_state;              /* current RRC State  */  
  rrcrb_directed_cell_info_type  directed_cell_info;        /* directed cell info struct */
  rrcrb_rnti_update_info_type    rnti_update_info;             /* to hold rnti update info */  
  rrc_PhysicalChannelReconfiguration *msg_ptr = NULL;  /* Pointer to the RB Setup message */

  current_state = rrc_get_state(); 

  msg_ptr = &dcch_msg_ptr->message.u.physicalChannelReconfiguration;
  
  if(rrc_sib_is_current_event_sib_read_in_dch())
  {
    WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
    rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
  }

  /* Populate the local variables to indicate if Re-direction info is present */
  directed_cell_info.freq_info_present = FALSE;
  directed_cell_info.pscr_present = FALSE;

  if((current_state==RRC_STATE_CELL_FACH) && 
      ((pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
      ||(pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
      ||(pcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
  {
    if((RRCRB_R9_MSG_IE_PRESENT(pcrc_ptr,frequencyInfo)) &&
      (RRC_CHECK_COMMON_MSG_TYPE(pcrc_ptr->frequencyInfo.modeSpecificInfo,
      rrc_FrequencyInfo_modeSpecificInfo_fdd)))           
    {
      directed_cell_info.freq_info_present = TRUE;
      directed_cell_info.uarfcn_dl = pcrc_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
    }
    if (rrcpcrc_is_pcpich_info_present(msg_ptr, MSG_REL9))
    {
      directed_cell_info.pscr_present = TRUE;
      directed_cell_info.pscr = pcrc_ptr->dl_InformationPerRL_List.elem[0].
                              modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
    }
    /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
    FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
    if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
       (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
    {
      WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
      if(pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
      {
        rrcpcrc_send_initiate_cu_flag = TRUE;
        rrcpcrc_delete_crnti          = TRUE;
      }
      else
      {
        pcrc.directed_cell_info = TRUE;
      }
    }
  }

  /* CellDCH<->CellDCH or CellFACH<->CellFACH or CellFACH->CellDCH transition. */
  if((current_state==pcrc.state_indicated_by_msg)||((current_state==RRC_STATE_CELL_FACH) && 
    (pcrc.state_indicated_by_msg==RRC_STATE_CELL_DCH)))
  {
    if ((current_state==RRC_STATE_CELL_FACH)&& 
     (pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
    {
      if(rrcpcrc_process_after_oc_config_set_for_fach_to_fach_r9(msg_ptr, directed_cell_info))
      return;
    }/* if CELL_FACH->CELL_FACH reconfiguration */

    /* Call the function that creates and sends a channel config request to LLC. */
    rrcpcrc_send_channel_config_req( );

    /* Change the substate since we're waiting for the confirmation from LLC for the channel configs */
    pcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
  }

  /*Cell_DCH -> Cell_FACH*/
  else if((current_state==RRC_STATE_CELL_DCH) && 
    (pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
  {
    if(RRCRB_R9_MSG_IE_PRESENT(pcrc_ptr,new_C_RNTI))
    {
      pcrc.new_crnti_valid = TRUE;
      rrc_translate_crnti(&pcrc_ptr->new_C_RNTI, &c_rnti);
      pcrc.new_crnti = (uint16)c_rnti;
    }
    else
    {
      pcrc.new_crnti_valid = FALSE;
    }

    /*Initiate the cell selection*/
    rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_PCR,
                  RRC_TRANSITION_FROM_DCH, RRC_STATE_CELL_FACH);
    pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
    
    /* Start T_305 [periodic cell update] timer */
    rrccu_start_t_305_timer();
  }

  /*Cell_DCH -> Cell_PCH*/
  else if((current_state==RRC_STATE_CELL_DCH) && 
      (pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH))
  {
    pcrc.directed_cell_info = rrcpcrc_is_pcpich_info_present(msg_ptr, MSG_REL9);

    /*Send the PCRC complete msg on old configuration */
    send_pcrc_complete_message(TRUE);

    /*wait for L2ack */
    pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
    /* prints rate for each transport channel */
    rrcllc_print_trans_chnl_rate();
  }

  /*Cell_DCH -> URA_PCH*/
  else if((current_state==RRC_STATE_CELL_DCH) && 
      (pcrc.state_indicated_by_msg==RRC_STATE_URA_PCH))
  {
    /*Send the PCRC complete msg on old configuration */
    send_pcrc_complete_message(TRUE);
    /*wait for L2ack */
    pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
    /* prints rate for each transport channel */
    rrcllc_print_trans_chnl_rate();
  }

  /*Cell_FACH -> Cell_PCH or Cell_FACH ->URA_PCH*/
  else if((current_state==RRC_STATE_CELL_FACH) && 
      ((pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
      (pcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
  {
    /*call the function that evaluates the directed cell info and set the flags accordingly */
    if((pcrc.directed_cell_info == FALSE) &&
      (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
    {
      if(pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
      {
        pcrc.directed_cell_info = rrcpcrc_is_pcpich_info_present(msg_ptr, MSG_REL9);
      }
      pcrc.initiate_cell_selection_trans_from_fach = TRUE;
    }
  
    /* check whether new C-RNTI was present in the message. In that case
    we have to configure MAC with new C-RNTI before sending the response message */
    if(RRCRB_R9_MSG_IE_PRESENT(pcrc_ptr,new_C_RNTI))
    {
      rrc_translate_crnti(&pcrc_ptr->new_C_RNTI, &c_rnti);

      /* Send a RNTI Update Request */
      rnti_update_info.crnti_action    = RRC_RNTI_UPDATE;
      rnti_update_info.crnti           = (uint16)c_rnti;

      /* check whether URNTI is also present in the message */
      if(RRCRB_R9_MSG_IE_PRESENT(pcrc_ptr,new_U_RNTI))
      {
        rrc_translate_urnti(&pcrc_ptr->new_U_RNTI, &u_rnti);
        rnti_update_info.urnti_action = RRC_RNTI_UPDATE; 
        rnti_update_info.urnti        = u_rnti; 
      }
      else
      {
        rnti_update_info.urnti_action = RRC_RNTI_NOCHANGE; 
      }
      rnti_update_info.procedure            = RRC_PROCEDURE_PCR;
      rnti_update_info.rlc_re_establish_srb = FALSE;
      rnti_update_info.rlc_re_establish_rab = FALSE;
      rnti_update_info.cnf_reqd             = TRUE;

      rrcrb_send_rnti_update_req(rnti_update_info);
      pcrc.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
    }
    else 
    {
      /* There is no new C-RNTI So send the response on the old configuration */
      send_pcrc_complete_message(TRUE);
      /*wait for L2ack */
      pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
      /* prints rate for each transport channel */
      rrcllc_print_trans_chnl_rate();
    }
  }
  return;
}





/*===========================================================================
FUNCTION   validate_pcrc_r9

DESCRIPTION
  This function validates the received PC Reconfig Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a PCRC Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void validate_pcrc_r9
(
  rrc_PhysicalChannelReconfiguration_r9_IEs *msg_ptr /* Pointer to the PCRC message */
)
{
  /* Assume the message is valid for now */
  pcrc.status.failure_status = RRCPC_MSG_VALID;

  /* Check the RRC State indicatior given by the message */
  switch( msg_ptr->rrc_StateIndicator )
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      pcrc.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      pcrc.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      pcrc.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;

    case rrc_RRC_StateIndicator_ura_PCH:
      pcrc.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;

    default:
      /* No other states are supported right now. */
      WRRC_MSG0_HIGH("Unsupported state in PC Reconfig Message");
      pcrc.status.failure_status = RRCPC_MSG_UNSUPPORTED_CONFIG;
      break;
  }

  if( pcrc.status.failure_status == RRCPC_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
     return;
  }

  /*RRC_GKG: We can put a check for HSDPA, HSUPA for TDD and return failure.*/
 
   /* Check if RB with PDCP Information list is present. This
  is not supported in the first release. This may be supported
  later */                                    
  if(RRCRB_R9_MSG_IE_PRESENT(msg_ptr,dl_CounterSynchronisationInfo))
  {
    /*check whether the SRNS relocation is within Cell_DCH or not.
     SRNS relocation information can come in reconfiguration messages
     only when the state transition is Cell_DCH->Cell_DCH */
    if (!((pcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("PCRC : SRNS reloc with invalid states cur_state %d, msg_state %d",
             rrc_get_state(), pcrc.state_indicated_by_msg);
      pcrc.status.failure_status=RRCPC_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has 
    PDCP info list. If it is true then this configuration is unsupported */
    if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2(msg_ptr->dl_CounterSynchronisationInfo,
		rb_WithPDCP_InfoList)) ||
        (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->dl_CounterSynchronisationInfo,
         rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
    {
      WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
      pcrc.status.failure_status=RRCPC_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation. Set the flag that indicates that this procedure
      involves SRNS relocation */
      pcrc.pcrc_for_srns_relocation = TRUE;
      /* store if new U-RNTI is present in the message */
      if(RRCRB_R9_MSG_IE_PRESENT(msg_ptr,
        new_U_RNTI))
      {
        pcrc.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(msg_ptr->new_U_RNTI), &pcrc.new_urnti);
      }
    }
  }

    /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  pcrc.trans_to_dch_ciph_config_needed = FALSE;

  if((pcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
    (rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRCRB_R9_MSG_IE_PRESENT(msg_ptr,
      dl_CommonInformation)) && (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->dl_CommonInformation,
      rrc_DL_CommonInformation_r8,dl_dpchInfoCommon)) && (T_rrc_DL_CommonInformation_r8_dl_dpchInfoCommon_dl_DPCH_InfoCommon
      == msg_ptr->dl_CommonInformation.dl_dpchInfoCommon.t))
    {
      //check if ciphering is active in mac for cs domain, if it is then,
      //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      if(rrcsmc_tm_ciph_active() == TRUE)
      {
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        uecomdef_status_e_type loc_ret_val;

        WRRC_MSG0_HIGH("To-DCH-Step-1 config activated");

        loc_ret_val = rrcrb_get_mac_d_hfn(( 
          RRC_MSG_COMMON_BITMASK_IE_PTR(msg_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.dl_DPCH_InfoCommon,
          rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
          &msg_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
          &mac_d_hfn_l);

        if (loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("get_mac_d_hfn failed");
        }
        if(rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
        {
           WRRC_MSG0_HIGH("SMC process ciphering config for HHO failed");
        }
        pcrc.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }

  /*Cell_PCH/URA_PCH specific validations */
  if ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
    /* If UTRAN DRX coefficient is not present then it is invalid configuration */
    if (!(RRCRB_R9_MSG_IE_PRESENT(msg_ptr, utran_DRX_CycleLengthCoeff)))
    {
      WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", pcrc.state_indicated_by_msg);
      pcrc.status.failure_status = RRCPC_MSG_INVALID_CONFIG;
      return;
    }

    if (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
    {
      /*should we do it after getting l2ack for response??*/
      if (!(RRCRB_R9_MSG_IE_PRESENT(msg_ptr,ura_Identity)))
      {
        WRRC_MSG0_HIGH("URA ID not present");
        rrccu_clear_ura_id ();
      }
      else
      {
        /*URA ID present in the message set ura_identity variable with this value */
        if (msg_ptr->ura_Identity.numbits == 16)
        {
          rrccu_update_ura_id (&(msg_ptr->ura_Identity));
        }
        else
        {
          WRRC_MSG1_ERROR("Invalid URA ID length%d", msg_ptr->ura_Identity.numbits);
          pcrc.status.failure_status = RRCPC_MSG_PROTOCOL_ERROR;
          pcrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
          return;
        }
      }
    }
  }
}/* end validate_pcrc_r7() */



/*===========================================================================
FUNCTION   PROCESS_PCRC_R9

DESCRIPTION
  This function processes the received Rel 9 PC Reconfig Message. It first validates the message. 
  If the message has an invalid or unsupported configuration, a PCRC Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -

    ORDERED_CONFIG will be set by calling LLC. 

    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG_REQ 
    will be sent to LLC to reconfigure the Physical Channel given by the message.
    
    If ORDERED_CONFIG could not be set, a PCRC Failure message
    will be sent to UTRAN.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void process_pcrc_r9
(
  rrc_RRC_TransactionIdentifier transaction_id,
  rrc_DL_DCCH_Message *dcch_msg_ptr  ,
  rrc_PhysicalChannelReconfiguration_r9_IEs *pcrc_ptr   /* Pointer to the R8 PC Reconfig message */
)
{
  rrc_PhysicalChannelReconfiguration *msg_ptr = NULL;  /* Pointer to the RB Setup message */
  rrcllc_oc_status_e_type          oc_status;                     /* To hold ordered_config status   */

  /* Used to indicate if an error occured while setting ordered_config. */
  boolean       ordered_config_err_occured = FALSE; 
  
  msg_ptr = &dcch_msg_ptr->message.u.physicalChannelReconfiguration;
  WRRC_MSG1_HIGH("REL9: Received PC-Reconfig OTA, PCRC-substate %d", pcrc.curr_substate);

  if (FAILURE == rrcpcrc_check_and_send_failure_for_simul_reconfig(
        msg_ptr->u.later_than_r3.rrc_TransactionIdentifier))
  {
    return;
  }

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  validate_pcrc_r9( pcrc_ptr );

  if( pcrc.status.failure_status != RRCPC_MSG_VALID )
  {
    /*  Message was invalid. */
    WRRC_MSG0_ERROR("REL9: PCRC Message validation failed");

    /* Send the PC Reconfig Failure Message */
    send_pcrc_failure_message( pcrc.status.failure_status, pcrc.status.prot_err_cause);

    /* Make sure we're back in the initial sub-state */
    pcrc.curr_substate = RRCRB_INITIAL;
  }
  /* If the message is valid, proceed further */
  else
  {
    MSG_LOW("Processing PCRC Message", 0, 0, 0);

    /* Check if there is CN information present. If so, we need to send the information to the 
    Cell Change manager */
    if(RRCRB_R9_MSG_IE_PRESENT(pcrc_ptr,
      cn_InformationInfo))
    {
      WRRC_MSG0_HIGH("Saving CN info for NAS");
      pcrc.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(
          &(pcrc_ptr->cn_InformationInfo));
 
      if (pcrc.mm_cmd_ptr == NULL)
      {
        WRRC_MSG0_ERROR("Could not build SERVICE_IND cmd");
      }
      else
      {
        /* set cn_info_saved flag */
        pcrc.cn_info_saved = TRUE;
      }
    }

    /*Check whether Mobility State Indicator is present in OTA, if yes, then store it PCRC global.
    Later we will inform L1 about this indicator.*/
    if(RRCRB_R9_MSG_IE_PRESENT(pcrc_ptr,
      ueMobilityStateIndicator))
    {
      if(pcrc_ptr->ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
      {
        WRRC_MSG0_HIGH("High Mobility Ind Set to TRUE in PCRC.");
        pcrc.high_mobility_ind = TRUE;
      }
    }

    /*Support for "Change of UE Capability" is under this feature*/
#ifdef FEATURE_WCDMA_REL7_SPECRS
    /*If PCRC OTA provides "supportForChangeOfUE_Capability" IE, we need to store it in
    "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
    "rrc_rnc_cap_change_support" can be updated accordingly. */
    if(RRCRB_R9_MSG_IE_PRESENT(pcrc_ptr,
      supportForChangeOfUE_Capability))
    {
      if(pcrc_ptr->supportForChangeOfUE_Capability == TRUE)
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

    /*If PCRC OTA provides "responseToChangeOfUE_Capability_present" IE, we need to store it so that later
    if procedure is successful, PCRC need to notify the COUEC procedure  about this IE.*/
    rrc_change_of_ue_cap_response = FALSE;
    
    if(RRCRB_R9_MSG_IE_PRESENT(pcrc_ptr,
      responseToChangeOfUE_Capability))
    {
      rrc_change_of_ue_cap_response = TRUE;
    }    
#endif /*FEATURE_WCDMA_REL7_SPECRS*/

    /* Need to send reconfig IE info. First set ORDERED_CONFIG
    so that LLC can process the necessary parameters. */
    oc_status = rrcllc_set_ordered_config(RRC_PROCEDURE_PCR, pcrc.state_indicated_by_msg, 
                                                            rrc_DL_DCCH_Message_PDU,(void *)dcch_msg_ptr );
    switch(oc_status)
    {
      case ORDERED_CONFIG_SET:
        rrcpcrc_process_after_oc_config_set_r9(dcch_msg_ptr,pcrc_ptr);
        break;

      case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

        pcrc.status.failure_status = RRCPC_SIMULTANEOUS_RECONFIG;
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause );

        /* Go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:
        pcrc.status.failure_status =RRCPC_MSG_UNSUPPORTED_CONFIG;
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause );

        /* Go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_INVALID:

        pcrc.status.failure_status =RRCPC_MSG_INVALID_CONFIG;
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause );

        /* Go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
      
      case ORDERED_CONFIG_NOT_SET_OTHER:
        WRRC_MSG0_HIGH("LLC returned other failure!");
        /* When LLC sends this, there is a serious problem
        and we'll go back to disconnected state. No need
        to send the failure message here, we can just
        go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME         
      case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("PCR processing deferred due to LTA!");
        pcrc.curr_substate = RRCRB_INITIAL;
      
      /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/

      default:

        WRRC_MSG0_ERROR("Unknown return value from LLC");
        /* Unknown error, just go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
    }/* end switch */

    if(ordered_config_err_occured == TRUE)
    {
      /* No need to continue. Clear the procedure variables and return. */
      rrcpcrc_clear_procedure();
      return;
    }
    else
    {
      if(pcrc.pcrc_for_srns_relocation == TRUE)
      {
        rrcsmc_commit_fresh_for_srns_relocation();
      }
    }
  }
}


#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif /* FEATURE_WCDMA_REL10 */


/*===========================================================================

FUNCTION   RRCRPRC_PROCESS_NEW_CELL_IND

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
static void rrcpcrc_process_new_cell_ind
(
  rrc_new_cell_ind_type *ptr /* Pointer to the State Change Ind cmd */
)
{
  
  /* If the UE has camped on directed cell then do not send a cell update,
    * else send a CELL update with cause "cell Re-selection 
    "*/
  if(rrccsp_is_selected_cell_utran_directed()&&
      (pcrc.new_crnti_valid)) 
  {
     /* No need to initiate cell update as the C-RNTI is available
      * and UE is camped on the directed cell.
      */
    rrcpcrc_send_initiate_cu_flag = FALSE;
  }
  else
  {
    rrcpcrc_send_initiate_cu_flag = TRUE;
    rrcpcrc_delete_crnti = TRUE;
  }
#ifdef FEATURE_WCDMA_HS_FACH
 /* If H-RNTI is not given in OTA then initiate cell update*/
  if(rrcenhstate_set_hrnti_status_for_transitions())
  {
    rrcpcrc_send_initiate_cu_flag = TRUE;
    rrcpcrc_delete_crnti          = TRUE;
  }
#endif
     
#ifdef FEATURE_WCDMA_HS_RACH
 /* Check if E-RNTI is not given in OTA then initiate cell update*/
        if(rrchsrach_check_ernti_status())
        {
          rrcpcrc_send_initiate_cu_flag = TRUE;
          rrcpcrc_delete_crnti          = TRUE;
        }
#endif

     
  rrcpcrc_send_channel_config_req();
  
  /* Change the substate since we're waiting for the
     confirmation from LLC for the channel configs */
  pcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
 
}

/*===========================================================================

FUNCTION   rrcpcrc_validate_and_check_integrity_for_srns_relocation

DESCRIPTION

  This function validates whether received pcrc message is due to SRNS relocation,
  if yes, then it performs the integrity check on the received message because
  integrity check was skipped for message which triggers SRNS relocation.
          
DEPENDENCIES

  None.

REUSABILITY: R99/R5/R6/R7/R8

RETURN VALUE

  SUCCESS or FAILURE

SIDE EFFECTS

  None.

===========================================================================*/
static uecomdef_status_e_type rrcpcrc_validate_and_check_integrity_for_srns_relocation
(
  rrc_cmd_type *cmd_ptr,
  /* Pointer to the PC Reconfig message        */
  rrc_PhysicalChannelReconfiguration *msg_ptr,
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


  if (TRUE == rrcpcrc_validate_srns_relocation_info(msg_ptr, msg_version))
  {
    ip_check_status = rrcsmc_check_integrity_for_srns_relocation(cmd_ptr
    , msg_version
    );
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
        if (transaction_id == pcrc.tr_ids.rejected_transaction)
        {
          WRRC_MSG1_HIGH("Invalid security info - rejected tr-id%d match!",
                   pcrc.tr_ids.rejected_transaction);
          /* Nothing else to do, just ignore the message and return */
        } 
        else
        {
          /* send a reconfig failure messge with invalid configuration and 
          continue the same substate with any ongoing procedure */
          pcrc.tr_ids.rejected_transaction = transaction_id;

          if (ip_check_status == RRCSMC_SRNS_SECURITY_INFO_INVALID)
          {
            pcrc.status.failure_status = RRCPC_MSG_INVALID_CONFIG;
          }
          else
          {
            pcrc.status.failure_status = RRCPC_SIMULTANEOUS_RECONFIG;
          }
          /*Check whether Cell Update is going on */
          if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
               (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
          {
            WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
            (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_PCR);
            /* Wait until Cell update procedure is completed */
            pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF; 
          }
          else
          {
            send_pcrc_failure_message( pcrc.status.failure_status,
                                            pcrc.status.prot_err_cause );
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
    /*do not clear the procedure or change the pcrc substate because
    some other pcrc procedure might be in progress. Just return from
    here */
    return FAILURE;
  }
  else
  {
    return SUCCESS;
  }  
}

/*===========================================================================

FUNCTION   rrcpcrc_send_pcrc_failure_unsupported_config

DESCRIPTION

  This function sends the pcreconfig-failure messsage with cause unsupported
  configuration because UE dont support the extension provided by the network.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcpcrc_send_pcrc_failure_unsupported_config
(
  /* transaction id of RB Reconfig message */
  rrc_RRC_TransactionIdentifier transaction_id  
)
{
  WRRC_MSG0_HIGH("Unsupported PC Reconfig t");
  if (transaction_id == pcrc.tr_ids.rejected_transaction )
  {
    WRRC_MSG1_HIGH("Duplicate PCRC msg! rejected tr-id%d match!",pcrc.tr_ids.rejected_transaction);
    /* Nothing else to do, just ignore the message and return */
  }
  else
  {
    /* We have received a pcrc message with a new transaction id.
    This message should be in our rejected transaction  
    list */
    pcrc.tr_ids.rejected_transaction = transaction_id;
    pcrc.status.failure_status = RRCPC_MSG_PROTOCOL_ERROR;
    pcrc.status.prot_err_cause = rrc_ProtocolErrorCause_messageExtensionNotComprehended;
    /*Check whether Cell Update is going on */
    if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
         (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
    {
      WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
      if (SUCCESS != rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_PCR))
      {
        WRRC_MSG0_ERROR("PCR failed to register with CU");
      }
      /* Wait until Cell update procedure is completed */
      pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF; 
    }
    else
    {
      send_pcrc_failure_message( pcrc.status.failure_status,
                                 pcrc.status.prot_err_cause );
      /*continue the same substate */ 
    }
  }
  return;
}



/*===========================================================================

FUNCTION   PROCESS_PCRC_MESSAGE

DESCRIPTION

  This function processes the received PC Reconfig Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a PCRC Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to reconfigure the Physical Channel given by the message.
    
    If ORDERED_CONFIG could not be set, a PCRC Failure message
    will be sent to UTRAN.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_pcrc_message
(
  rrc_cmd_type *cmd_ptr
  
)
{

  rrc_PhysicalChannelReconfiguration *msg_ptr;  /* Pointer to the PC Reconfig message        */
  rrcllc_oc_status_e_type             oc_status;/* To hold ordered_config status   */
  rrc_state_e_type                    current_state; /* current RRC State  */
  rrc_int_c_rnti_type                 c_rnti;       /* Local C_RNTI value */
  rrc_int_u_rnti_type                 u_rnti;
  rrcrb_directed_cell_info_type       directed_cell_info; /* directed cell info struct */

  boolean       ordered_config_err_occured = FALSE; /* Used to indicate if an error
                                    occured while setting ordered_config. */

  rrc_DL_DCCH_Message         *dcch_msg_ptr ;  /* Pointer to the PC Reconfig message */

  rrcrb_rnti_update_info_type rnti_update_info;   /* to hold rnti update info */

   /*Boolean used to indicate primary plmn i.d is present in OTA msg*/
  boolean primary_plmn_Id_present = FALSE;
   /*Variable to hold primary plmn i.d if present in OTA msg*/
  rrc_PLMN_Identity primary_plmn_Identity;

  /* A Downlink SDU has been received from RLC. This
  SDU should contain a PC Reconfiguration Message. Otherwise
  we would not have received this command. Copy the 
  received message pointer. */
  
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  WRRC_MSG1_HIGH("Received PCRC msg, substate %d", pcrc.curr_substate);

  /* If ASN.1 decoding failed, the received pointer would be null.
  So we know we got a PCRC Message (by peeking into
  the encoded bit stream), but we were not able to decode it.
  Send a PC Reconfig failure message with the appropriate
  cause. and continue in same substate*/
  if( dcch_msg_ptr == NULL )
  {
    WRRC_MSG0_HIGH("ASN decode failed for PCRC msg");
    pcrc.status.failure_status = RRCPC_MSG_PROTOCOL_ERROR;
    pcrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
    send_pcrc_failure_message( pcrc.status.failure_status,
                              pcrc.status.prot_err_cause
                            );
    
    /* Nothing else to do here */
    return;
  }


  /* Make sure it's the PCRC message
  although we would not have gotten here unless it's
  the right message */ 
  if( dcch_msg_ptr->message.t != T_rrc_DL_DCCH_MessageType_physicalChannelReconfiguration)
  {
    WRRC_MSG1_ERROR("Incorrect message given to PCR proc %d", dcch_msg_ptr->message.t);
    return;
  }

  /* Get the pointer to the actual PC Reconfig message */
  msg_ptr = &(dcch_msg_ptr->message.u.physicalChannelReconfiguration);

  /* This PCRC msg is Rel5/Rel-6 version. */
  if (msg_ptr->t != T_rrc_PhysicalChannelReconfiguration_r3)
  {
    if ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) &&
        (msg_ptr->u.later_than_r3.criticalExtensions.t == 
           T_rrc_PhysicalChannelReconfiguration_criticalExtensions_5_criticalExtensions))
    {
      if (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.t ==
           T_rrc_PhysicalChannelReconfiguration_criticalExtensions_4_r5)
      { 
        /* check for SRNS relocation scenario */
        if (FAILURE == rrcpcrc_validate_and_check_integrity_for_srns_relocation(
            cmd_ptr, msg_ptr, msg_ptr->u.later_than_r3.rrc_TransactionIdentifier, 
            MSG_REL5))
        {
          /*do not clear the procedure or change the rbr substate because
            some other rbreconfig procedure might be in progress. Just return from
            here */
          return;
        }

        process_pcrc_r5(cmd_ptr);
      }
      /* if we are here then it means that t selected is: 
         rrc_RadioBearerReconfiguration_later_than_r3_criticalExtensions_
         criticalExtensions_criticalExtensions */
      else if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6) && 
              (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
              criticalExtensions.t ==
               T_rrc_PhysicalChannelReconfiguration_criticalExtensions_3_r6))
      {
        /* check for SRNS relocation scenario */
        if (FAILURE == rrcpcrc_validate_and_check_integrity_for_srns_relocation(
            cmd_ptr, msg_ptr, msg_ptr->u.later_than_r3.rrc_TransactionIdentifier, 
            MSG_REL6))
        {
          /* do not clear the procedure or change the rbr substate because
          some other rbreconfig procedure might be in progress. Just return from
          here */
          return;
        }

        process_pcrc_r6(
          msg_ptr->u.later_than_r3.rrc_TransactionIdentifier,
          (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg,
          &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
          criticalExtensions.u.r6.physicalChannelReconfiguration_r6
        );
  

      }
      /* if we are here then it means that t selected is: 
         rrc_RadioBearerReconfiguration_later_than_r3_criticalExtensions_
         criticalExtensions_criticalExtensions */
      else if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7) && 
              (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
              criticalExtensions.u.criticalExtensions.t ==
               T_rrc_PhysicalChannelReconfiguration_criticalExtensions_2_r7))
      {
        /* check for SRNS relocation scenario */
        if (FAILURE == rrcpcrc_validate_and_check_integrity_for_srns_relocation(
            cmd_ptr, msg_ptr, msg_ptr->u.later_than_r3.rrc_TransactionIdentifier, 
            MSG_REL7))
        {
          /* do not clear the procedure or change the rbr substate because
          some other rbreconfig procedure might be in progress. Just return from
          here */
          return;
        }

        process_pcrc_r7(
          msg_ptr->u.later_than_r3.rrc_TransactionIdentifier,
          (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg,
          &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
          criticalExtensions.u.r7.physicalChannelReconfiguration_r7
        );
  
       
  
      }
      /*Now we check whether R8 PCRC is present or not. */
      else if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8) && 
        (RRCRB_R8_CRITICAL_EXT_PRESENT_PTR(msg_ptr, rrc_PhysicalChannelReconfiguration)))
      {
        /* check for SRNS relocation scenario */
        if (FAILURE == rrcpcrc_validate_and_check_integrity_for_srns_relocation(
          cmd_ptr, msg_ptr, msg_ptr->u.later_than_r3.rrc_TransactionIdentifier, MSG_REL8))
        {
          /* do not clear the procedure or change the rbr substate because
          some other rbreconfig procedure might be in progress. Just return from
          here */
          return;
        }
        /*Now process the R8 PCRC OTA and then return.*/
        process_pcrc_r8(msg_ptr->u.later_than_r3.rrc_TransactionIdentifier,
          (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg,
          &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
          criticalExtensions.u.criticalExtensions.u.r8.physicalChannelReconfiguration_r8);
      }
#ifdef FEATURE_WCDMA_REL9
      /*Now we check whether R9 PCRC is present or not. */
      else if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9) && 
        (RRCRB_R9_CRITICAL_EXT_PRESENT_PTR(msg_ptr, rrc_PhysicalChannelReconfiguration)))
      {
        /* check for SRNS relocation scenario */
        if (FAILURE == rrcpcrc_validate_and_check_integrity_for_srns_relocation(
          cmd_ptr, msg_ptr, msg_ptr->u.later_than_r3.rrc_TransactionIdentifier, MSG_REL9))
        {
          /* do not clear the procedure or change the rbr substate because
          some other rbreconfig procedure might be in progress. Just return from
          here */
          return;
        }
        /*Now process the R8 PCRC OTA and then return.*/
        process_pcrc_r9(msg_ptr->u.later_than_r3.rrc_TransactionIdentifier,
          (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg,
          &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
          criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.physicalChannelReconfiguration_r9);
      }
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
      #error code not present
#endif /* FEATURE_WCDMA_REL10 */

      else
      {
        rrc_print_supported_asn1_rel_version();
  
        WRRC_MSG0_ERROR("Configuration not supported.  Send PC reconfig failure");
    
        rrcpcrc_send_pcrc_failure_unsupported_config(
            msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);      
      }

      /* return - because all r5, r6, r7 and r8 processing are done inside the function */
      return;
    }
    else
    {
      rrc_print_supported_asn1_rel_version();
  
      WRRC_MSG0_ERROR("Configuration not supported.  Send PC reconfig failure");
  
      rrcpcrc_send_pcrc_failure_unsupported_config(
        msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
  
      /*do not clear the procedure or change the rbr substate because
       some other rbreconfig procedure might be in progress. Just return from
       here */
      return;
    }
  }

  /* check for SRNS relocation scenario */
  if (FAILURE == rrcpcrc_validate_and_check_integrity_for_srns_relocation(
      cmd_ptr, 
      msg_ptr, 
      msg_ptr->u.r3.physicalChannelReconfiguration_r3.rrc_TransactionIdentifier,
      MSG_REL99))
  {
    /*do not clear the procedure or change the rbr substate because
    some other rbreconfig procedure might be in progress. Just return from
    here */
    return;
  }

  if (FAILURE == 
      rrcpcrc_check_and_send_failure_for_simul_reconfig(
      msg_ptr->u.r3.physicalChannelReconfiguration_r3.rrc_TransactionIdentifier))
  {
    return;
  }

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  validate_pcrc_message( msg_ptr );

  if( pcrc.status.failure_status != RRCPC_MSG_VALID )
  {
    /*  Message was invalid. */
    MSG_LOW("PCRC Message validation failed", 0, 0, 0);

    /* Send the PC Reconfig Failure Message */
    send_pcrc_failure_message( pcrc.status.failure_status,
                              pcrc.status.prot_err_cause
                            );

    /* Make sure we're back in the initial sub-state */
    pcrc.curr_substate = RRCRB_INITIAL;
  }
  /* If the message is valid, proceed further */
  else
  {

    MSG_LOW("Processing PCRC Message", 0, 0, 0);

    /* Check if there is CN information present. If so, we need
    to send the information to the Cell Change manager */
    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.physicalChannelReconfiguration_r3,
        rrc_PhysicalChannelReconfiguration_r3_IEs,cn_InformationInfo)
      )
    {
      WRRC_MSG0_HIGH("Saving CN info for NAS");
      /*Check to see if primary PLMN I.D is present in PCR message ext
       * physicalChannelReconfiguration-v690ext which is present in 
       * v690NonCriticalExtensions
       */
       primary_plmn_Id_present = rrcpcr_check_and_get_primary_plmn_id(msg_ptr,
                                                                  &primary_plmn_Identity,MSG_REL99);
      pcrc.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info(
                     &(msg_ptr->u.r3.physicalChannelReconfiguration_r3.cn_InformationInfo) 
                     ,primary_plmn_Id_present
                     ,&primary_plmn_Identity
              );
      if (pcrc.mm_cmd_ptr == NULL)
      {
        WRRC_MSG0_ERROR("Could not build SERVICE_IND cmd");
      }
      else
      {
        /* set cn_info_saved flag */
        pcrc.cn_info_saved = TRUE;
      }
    }
      
    rrcpcrc_process_high_mobility_ind_in_non_critical_ext(msg_ptr,MSG_REL99);

    /* Need to send reconfig IE info. First set ORDERED_CONFIG
    so that LLC can process the necessary parameters. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_PCR,
                                           pcrc.state_indicated_by_msg,
                                           rrc_DL_DCCH_Message_PDU,
                                           (void *)dcch_msg_ptr );

    switch( oc_status )
    {

      case ORDERED_CONFIG_SET:
        current_state = rrc_get_state(); 

        /* Populate the local variables to indicate if Re-direction info is present */
        directed_cell_info.freq_info_present = FALSE;
        directed_cell_info.pscr_present = FALSE;
     
        if((current_state==RRC_STATE_CELL_FACH) && 
               ((pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
               ||(pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
               ||(pcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          if((RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.physicalChannelReconfiguration_r3,
                   rrc_PhysicalChannelReconfiguration_r3_IEs,frequencyInfo)) &&
                  (RRC_CHECK_COMMON_MSG_TYPE(msg_ptr->u.r3.physicalChannelReconfiguration_r3.frequencyInfo.
                   modeSpecificInfo,rrc_FrequencyInfo_modeSpecificInfo_fdd)))           
          {
            directed_cell_info.freq_info_present = TRUE;
            directed_cell_info.uarfcn_dl         = msg_ptr->u.r3.physicalChannelReconfiguration_r3.
                                                   frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
          }
         
          if (rrcpcrc_is_pcpich_info_present(msg_ptr, MSG_REL99))
          {
   
            directed_cell_info.pscr_present = TRUE;
            directed_cell_info.pscr = msg_ptr->u.r3.physicalChannelReconfiguration_r3.dl_InformationPerRL_List.elem[0].
                                      modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
          }    
          /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
          FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
          if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
             (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
          {
            WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
            if(pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
            {
              rrcpcrc_send_initiate_cu_flag = TRUE;
              rrcpcrc_delete_crnti          = TRUE;
            }
            else
            {
              pcrc.directed_cell_info = TRUE;
            }
          }  
        }

        if((rrc_sib_is_current_event_sib_read_in_dch()) &&
           (RRC_PROCEDURE_GPS == rrc_sib_get_proc_id_for_sib_read_in_dch()))
        {
           WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
           rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
        }
        if (
            (current_state==pcrc.state_indicated_by_msg)||
            ((current_state==RRC_STATE_CELL_FACH)&& 
             (pcrc.state_indicated_by_msg==RRC_STATE_CELL_DCH))
           )
        {
          /*  CellDCH<->CellDCH or CellFACH<->CellFACH or CellFACH->CellDCH 
          transition. Ordered Config has been set. We can setup the
          Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */
          MSG_LOW("Sending RRC_CHANNEL_CONFIG_REQ to LLC", 0, 0, 0);

          /*check whether any frequency info is present in the message
          and transition is CELL_FACH->CELL_FACH. In this case
          initiate a cell update after channel config is successfully completed */
          if ((current_state==RRC_STATE_CELL_FACH)&& 
             (pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
          {
     
       
            /*call the function that evaluates the directed cell info and set the flags accordingly */
            if ((rrcpcrc_send_initiate_cu_flag == FALSE)&&
                (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
            {
              if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.physicalChannelReconfiguration_r3,
                 rrc_PhysicalChannelReconfiguration_r3_IEs,new_C_RNTI))
              {
                pcrc.new_crnti_valid = TRUE;
                rrc_translate_crnti(&msg_ptr->u.r3.physicalChannelReconfiguration_r3.new_C_RNTI, 
                                    &c_rnti);
                pcrc.new_crnti = (uint16)c_rnti;
              }
              else
              {
                pcrc.new_crnti_valid = FALSE;
              }
    
              rrcllcoc_update_toc_with_oc_from_fach();
              
              /*Initiate the cell selection*/
              rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_PCR,
                                                       RRC_TRANSITION_FROM_FACH,
                                                       pcrc.state_indicated_by_msg);
    
    
                
                
              pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
         
              /* exit from the switch case */
              break;
            }
    
          } /* if CELL_FACH->CELL_FACH reconfiguration */

          /* Call the function that creates and sends a channel
          config request to LLC. */
          rrcpcrc_send_channel_config_req( );
             /* Change the substate since we're waiting for the
             confirmation from LLC for the channel configs */
          pcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (pcrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
        {
          if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.physicalChannelReconfiguration_r3,
             rrc_PhysicalChannelReconfiguration_r3_IEs,new_C_RNTI))
          {
            pcrc.new_crnti_valid = TRUE;
            rrc_translate_crnti(&msg_ptr->u.r3.physicalChannelReconfiguration_r3.new_C_RNTI, 
                                &c_rnti);
            pcrc.new_crnti = (uint16)c_rnti;
          }
          else
          {
            pcrc.new_crnti_valid = FALSE;
          }

          /* prints rate for each transport channel */
          rrcllc_print_trans_chnl_rate();

          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_PCR,
                                                  RRC_TRANSITION_FROM_DCH,
                                                  pcrc.state_indicated_by_msg);
          pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

          /* Start T_305 [periodic cell update] timer */
          rrccu_start_t_305_timer();
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH))
        {
          pcrc.directed_cell_info = rrcpcrc_is_pcpich_info_present(msg_ptr, MSG_REL99);

         /*Send the PCRC complete msg on old configuration */
         send_pcrc_complete_message(TRUE);
         /*wait for L2ack */
         pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
         /* prints rate for each transport channel */
         rrcllc_print_trans_chnl_rate();
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (pcrc.state_indicated_by_msg==RRC_STATE_URA_PCH))
        {
          /*Send the PCRC complete msg on old configuration */
          send_pcrc_complete_message(TRUE);
          /*wait for L2ack */
          pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
          /* prints rate for each transport channel */
          rrcllc_print_trans_chnl_rate();
        }
        else if((current_state==RRC_STATE_CELL_FACH) && 
                ((pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
                 (pcrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          /*call the function that evaluates the directed cell info and set the flags accordingly */
          if ((pcrc.directed_cell_info == FALSE) &&
            (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
          {
            if(pcrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
            {
              pcrc.directed_cell_info = rrcpcrc_is_pcpich_info_present(msg_ptr, MSG_REL99);
            }
            pcrc.initiate_cell_selection_trans_from_fach = TRUE;
          }

          /* check whether new C-RNTI was present in the message. In that case
          we have to configure MAC with new C-RNTI before sending the response 
          message */
          if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.physicalChannelReconfiguration_r3,
             rrc_PhysicalChannelReconfiguration_r3_IEs,new_C_RNTI))
          {
            rrc_translate_crnti(&msg_ptr->u.r3.physicalChannelReconfiguration_r3.new_C_RNTI, 
                                &c_rnti);
            /* Send a RNTI Update Request */
            rnti_update_info.crnti_action    = RRC_RNTI_UPDATE;
            rnti_update_info.crnti           = (uint16)c_rnti;
            /* check whether URNTI is also present in the message */
            if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.physicalChannelReconfiguration_r3,
             rrc_PhysicalChannelReconfiguration_r3_IEs,new_U_RNTI))
            {
              rrc_translate_urnti(&msg_ptr->u.r3.physicalChannelReconfiguration_r3.new_U_RNTI, 
                                  &u_rnti);
              rnti_update_info.urnti_action = RRC_RNTI_UPDATE; 
              rnti_update_info.urnti        = u_rnti; 
            }
            else
            {
              rnti_update_info.urnti_action = RRC_RNTI_NOCHANGE; 
            }
            rnti_update_info.procedure            = RRC_PROCEDURE_PCR;
            rnti_update_info.rlc_re_establish_srb = FALSE;
            rnti_update_info.rlc_re_establish_rab = FALSE;
            rnti_update_info.cnf_reqd             = TRUE;

            rrcrb_send_rnti_update_req(rnti_update_info);
            pcrc.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
          }
          else 
          {
            /* There is no new C-RNTI So send the response on the old configuration
            */
            send_pcrc_complete_message(TRUE);
            /*wait for L2ack */
            pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
            /* prints rate for each transport channel */
            rrcllc_print_trans_chnl_rate();
          }
        }
        break;

      case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

        pcrc.status.failure_status = RRCPC_SIMULTANEOUS_RECONFIG;
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause );

        /* Go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

        pcrc.status.failure_status =RRCPC_MSG_UNSUPPORTED_CONFIG;
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause );

        /* Go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_INVALID:

        pcrc.status.failure_status =RRCPC_MSG_INVALID_CONFIG;
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause );

        /* Go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_NOT_SET_OTHER:
        WRRC_MSG0_HIGH("LLC returned other failure!");
        /* When LLC sends this, there is a serious problem
        and we'll go back to disconnected state. No need
        to send the failure message here, we can just
        go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME         
      case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("PCR processing deferred due to LTA!");
        pcrc.curr_substate = RRCRB_INITIAL;
      
      /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/

      default:

        WRRC_MSG0_ERROR("Unknown return value from LLC");
        /* Unknown error, just go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
    }/* end switch */

    if( ordered_config_err_occured == TRUE )
    {
      /* No need to continue. Clear the procedure
      variables and return. */
      rrcpcrc_clear_procedure();
      return;
    }

    if(pcrc.pcrc_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }
  }
}/* end function process_pcrc_message */

 /*===========================================================================

FUNCTION   RRCPCRC_PROCESS_RNTI_UPDATE_CNF

DESCRIPTION

  This function processes RNTI_UPDATE_CNF
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcpcrc_process_rnti_update_cnf
(
void
)
{
  rrc_state_e_type curr_state;

  curr_state = rrc_get_state();

  if ((curr_state == RRC_STATE_CELL_FACH) && 
      ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
       (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)))
  {
   /*send the response message now */
    send_pcrc_complete_message(TRUE);
    /*wait for L2ack */
    pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
    /* prints rate for each transport channel */
    rrcllc_print_trans_chnl_rate();
  }
  else
  {
    /* We should not get here*/
    WRRC_MSG1_HIGH("Unexpected RNTI Update cnf,pcrc state %d", 
                                   pcrc.curr_substate);
  }
}



/*===========================================================================

FUNCTION   RRCPCRC_PROCESS_CHANNEL_CONFIG_CNF

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
static void rrcpcrc_process_channel_config_cnf
(
  rrc_channel_config_cnf_type *ptr  /* Pointer to the Channel config cnf cmd */
)
{
  rrc_state_e_type            curr_state;
  rrcrb_rnti_update_info_type rnti_update_info;   /* to hold rnti update info */
  rrc_cmd_type *cmd; /* To send a LLC command if channel config failed */
  rrc_CellUpdateCause       cu_cause;  /*To store CU cause from CU procedure*/

  rrc_rlc_re_est_for_srns_rb_e_type rb_type; /*rb type tobe reestablished */
  {
#ifdef FEATURE_VOC_AMR_WB
    rrcrb_update_amr_cctrch_id_for_reconfig(rrc_active_codec);
#else/*FEATURE_VOC_AMR_WB*/
    rrcrb_update_amr_cctrch_id_for_reconfig();
#endif/*FEATURE_VOC_AMR_WB*/
  }

  rrcllcpcie_initialise_sync_a_post_veri_info();

  /* Check if the channel configuration was succesful. */
  if( ptr->chan_cfg_status != RRCLLC_CHAN_CFG_SUCCESS )
  {

    /*Here we initialize rlc_size_change_in_progress DB so that if it was set by PCRC, 
    it is ready for next procedure*/
    rrc_initialize_rlc_size_change();

    WRRC_MSG0_ERROR("CHAN_CONFIG_CNF with Failure");
    pcrc.status.failure_status = RRCPC_PHY_CHAN_FAILURE;
    if (ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_CFG_CANCELLED)
    {
      /* cell update is already in progress as this is for pending config
       * cancellation
       */
      WRRC_MSG0_HIGH("CU active.  Wait for CU to complete");
      pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      return; 
    }
    
    /* validation failure happened.  Send RB failure msg with invalid config */
    if (ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_OTHER)
    {
      pcrc.status.failure_status = RRCPC_MSG_INVALID_CONFIG;

      if(rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG)
      {
        WRRC_MSG0_HIGH("CU active.  Initiate CU and wait for CU complete");
        pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_PCR, RRC_UNRECOVERABLE_ERROR, FALSE);
        return; 
      }

      WRRC_MSG0_HIGH("Sending PCRC failure msg with invalid config");
      send_pcrc_failure_message( pcrc.status.failure_status,
                                pcrc.status.prot_err_cause
                                );
      /* Reset ORDERED_CONFIG - UE can now accept other ordered
      configurations */
      (void)rrcllc_reset_ordered_config(RRC_PROCEDURE_PCR,TRUE);
        /* Then clear all local variables that may
        need to be cleared */
        rrcpcrc_clear_procedure();
        /* Make sure we're back in the initial sub-state */
        pcrc.curr_substate = RRCRB_INITIAL;

      /*During FACH->DCH transition, if T305 got expired, we need to start T305 with zero timeout.*/
      if(rrc_get_state() == RRC_STATE_CELL_FACH && pcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH)
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
    request, and pcrc substate should be RRCRB_WAIT_FOR_CELL_UPDATE_CNF */ 
    if (rrc_get_state() == RRC_STATE_CELL_FACH && pcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH)
    {
      WRRC_MSG0_HIGH("PHYCHANFAIL FACH->DCH.  Initiate CU");

      pcrc.status.failure_status = RRCPC_PHY_CHAN_FAILURE;
      /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
      rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_PCR, RRC_PHY_CHAN_FAILURE,TRUE);
      /* Wait for CELL UPDATE to get a valid C_RNTI */
      pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      rrcpcrc_send_initiate_cu_flag = FALSE;
      rrcpcrc_delete_crnti = FALSE;
      
    }
    /* Check if PHY_CHAN_FAILURE happended within CELL_DCH.  For now, initatiate a cell update request
       with cause RL Failure.  RBRC substate should be RRCRB_WAIT_FOR_CELL_UPDATE_CNF*/
    else if (rrc_get_state() == RRC_STATE_CELL_DCH && pcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH)
    {
      pcrc.status.failure_status = RRCPC_PHY_CHAN_FAILURE;
      if(rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG)
      {
        WRRC_MSG0_HIGH("CU active.  Initiate CU and wait for CU to complete");
        pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_PCR, RRC_RADIO_LINK_FAILURE, FALSE);
        return; 
      }
      if(ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_PHY_CHL)
      {
        WRRC_MSG0_HIGH("PCRC failed, going back to old cfg success");
        
        
        send_pcrc_failure_message( pcrc.status.failure_status,
                                   pcrc.status.prot_err_cause
                                   );
        /* Go back to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;
        /* Then clear all local variables that may
        need to be cleared */
        rrcpcrc_clear_procedure();
        /* Clear ORDERED_CONFIG - UE can now accept other ordered
        configurations */
        rrcllc_clear_ordered_config(); 
      }
      else if(ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_BACK_TO_OLD_CONFIG)
      {
        WRRC_MSG0_HIGH("PCRC failed. Going back to old config failed.  Initiate CU");

        /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_PCR, RRC_RADIO_LINK_FAILURE,TRUE);
        /* Wait for CELL UPDATE to get a valid C_RNTI */
        pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        rrcpcrc_send_initiate_cu_flag = FALSE;
        rrcpcrc_delete_crnti = FALSE;
      }
    }
    else
    {
      if ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
          (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
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
          cmd->cmd.chan_config_req.procedure = RRC_PROCEDURE_PCR;
          /* No need to fill any other parameter in this command.
          When LLC sees the next state is Disconnected State, it
          will tear down all the Radio Bearers. */
          rrc_put_int_cmd( cmd );
        } 
        else
        {
        pcrc.status.failure_status = RRCPC_PHY_CHAN_FAILURE;
        send_pcrc_failure_message( pcrc.status.failure_status,
                                   pcrc.status.prot_err_cause
                                   );
      }
      /* Go back to initial substate */
      pcrc.curr_substate = RRCRB_INITIAL;
      /* At this point we don't need to remember any of the
      Radio Bearer information since it's stored in ESTABLISHED_RABS.
      Clear our internal variables */
      rrcpcrc_clear_procedure();
    }
    return;
  }

#ifdef FEATURE_WCDMA_REL7_SPECRS
  rrc_save_rnc_cap_change_support();

  /*Since Chnage in UE capability is successful, COUEC needs to be notified for this chnage.*/
  if(rrc_change_of_ue_cap_response)
  {
    rrccouec_process_completion_cap_change_with_reconfig();
    rrc_change_of_ue_cap_response = FALSE;
  }
#endif

  if((ordered_config.set_status == OC_SET_FOR_DCH_FACH_TRANS) || 
      (ordered_config.set_status == OC_SET_FOR_DCH_CELL_PCH_TRANS) ||
      (ordered_config.set_status == OC_SET_FOR_DCH_URA_PCH_TRANS))	
  {
    if(pcrc.high_mobility_ind == TRUE)
    {
      rrcrb_send_upd_ota_parms_ind(RRC_PROCEDURE_PCR, CPHY_HIGH_MOBILITY_INCL);
    }
  }  

  /*For FACH->DCH, if CHAN_CONFIG_REQ is succeeded, we need to stop T305*/  
  if(rrc_get_state() == RRC_STATE_CELL_FACH && pcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH)
  {
    WRRC_MSG0_HIGH("Chan_config succeeded for FACH->DCH. So Stopping T305");
    rrctmr_stop_timer(RRCTMR_T_305_TIMER);
  }

    /*Notify the Cell_Id to registered entities*/
  if(pcrc.state_indicated_by_msg == RRC_STATE_CELL_DCH)
  {
    rrc_notify_cell_id_in_dch();
  }	
    /* Check for CELL FACH before sending the rbs complete message */
  curr_state=rrc_get_state();
  if ((RRC_STATE_CELL_FACH == curr_state) && 
      ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_FACH) ||
       (pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
       (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)))
  {
    if (pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) 
    {
      /* this is the case of CELL_FACH->CELL_PCH state transition.
      In current implementation we do initiate cell selection. Instead
      we treat current cell as strongest cell */
      if ((pcrc.directed_cell_info) && (!rrccsp_is_selected_cell_utran_directed()))
      {
        /* check whether current camped scr code is same as the one
        sent in the message. If not then initiate cell update procedure */

          /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
          rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_PCR,
                                              RRC_CELL_RESELECTION,
                                              FALSE);
      /* No need to send ul data registration to rlc. This will be taken care
      by cellupdate procedure */
        rrcpcrc_process_successful_proc_completion(FALSE);
      }
      else
      {
        /* send drx info to L1. Since this is CELL_FACH->CELL_PCH state transition
        RB procedure is responsible for sending the DRX info to lower layer*/
        rrcrb_send_l1_drx_req (RRC_PROCEDURE_PCR);

        rrcpcrc_process_successful_proc_completion (TRUE);
      }
      /* return from here */
      return;
    }
    else if (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
    {
      /* send drx info to L1 */
      rrcrb_send_l1_drx_req (RRC_PROCEDURE_PCR);
      /* check whether URA update is required */
      if ( rrccu_is_ura_update_reqd() == TRUE)
      {
        rrcrb_send_ura_update_initiate_req( RRC_PROCEDURE_PCR,
                                            RRC_URA_RESELECTION,
                                            FALSE);
      }
      /* no need to wait for ura update completion 
      declare that proc is successfully completed */
      rrcpcrc_process_successful_proc_completion (TRUE);
      /* return from here */
      return;
    }
    if(rrcpcrc_send_initiate_cu_flag
       || (rrccu_return_cu_substate() == RRCCU_WAIT_FOR_RB_PROC_COMPL_IND) 
       )
    {
      if (rrcpcrc_delete_crnti)
      {
        /* Send a RNTI Update Request */
        rnti_update_info.crnti_action         = RRC_RNTI_DELETE;
        rnti_update_info.urnti_action         = RRC_RNTI_NOCHANGE;
        rnti_update_info.procedure            = RRC_PROCEDURE_PCR;
        rnti_update_info.rlc_re_establish_srb = FALSE;
        rnti_update_info.rlc_re_establish_rab = FALSE;
        rnti_update_info.cnf_reqd             = FALSE;

        rrcrb_send_rnti_update_req(rnti_update_info);
        pcrc.new_crnti_valid=FALSE;
      }
      /* this is the case of DCH->FACH transition */
      /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
      rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_PCR,
                                          RRC_CELL_RESELECTION,
                                          TRUE);
      /* Wait for CELL UPDATE to get a valid C_RNTI */
      pcrc.status.failure_status = RRCPC_MSG_VALID;
      pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      rrcpcrc_send_initiate_cu_flag = FALSE;
      rrcpcrc_delete_crnti = FALSE;
      return;
    }
    /*Following case will occur when RLC RESET happens on SRB2 during DCH->FACH Transition*/
    else if((rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG) &&
      (rrccu_get_cell_update_started_status_with_cause(&cu_cause) == RRCCU_STARTED) && 
      (cu_cause == rrc_CellUpdateCause_rlc_unrecoverableError) && (rrccu_get_rb_id_unrec_error() == DCCH_AM_RADIO_BEARER_ID))
    {
      if(pcrc.new_crnti_valid)
      {
        /* Send a RNTI Update Request */
        rnti_update_info.crnti_action         = RRC_RNTI_UPDATE;
        rnti_update_info.urnti_action         = RRC_RNTI_NOCHANGE;
        rnti_update_info.crnti                = pcrc.new_crnti;
        rnti_update_info.procedure            = RRC_PROCEDURE_PCR;
        rnti_update_info.rlc_re_establish_srb = FALSE;
        rnti_update_info.rlc_re_establish_rab = FALSE;
        rnti_update_info.cnf_reqd             = FALSE;
  
        rrcrb_send_rnti_update_req(rnti_update_info);
        /* Send a Radio Bearer Setup Complete later */      
      }

      /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
      rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_PCR,
                                          RRC_UNRECOVERABLE_ERROR,
                                          TRUE);

      /* Wait for CELL UPDATE to get a valid C_RNTI */
      pcrc.status.failure_status = RRCPC_MSG_VALID;
      pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;

      return;                                          
    }
    
    /* Coming here implies the "rrcpcrc_send_initiate_cu_flag" is FALSE.
     * Check if Valid CRNTI is stored in PCRC (for DCH->FACH case).
     * If so, send a RNTI Update Req to LLC and NOT wait for
     * Cell Update Complete Confirm. Cell Update will NOT be 
     * initiated when there is a valid C-RNTI and rrcpcrc_send_initiate_cu_flag
     * is FALSE.
     */
    else if(pcrc.new_crnti_valid)
    {
      /* Send a RNTI Update Request */
      rnti_update_info.crnti_action         = RRC_RNTI_UPDATE;
      rnti_update_info.urnti_action         = RRC_RNTI_NOCHANGE;
      rnti_update_info.crnti                = pcrc.new_crnti;
      rnti_update_info.procedure            = RRC_PROCEDURE_PCR;
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
      if (SUCCESS != rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_PCR))
      {
        WRRC_MSG0_ERROR("PCR failed to register with CU");
      }
      pcrc.status.failure_status = RRCPC_MSG_VALID;
      pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      return;
    } 
  } /* if current_state is CELL_FACH */
  else if (RRC_STATE_CELL_PCH == curr_state)
  {
    if(rrcpcrc_send_initiate_cu_flag)
    {
      /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
      rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_PCR,
                                          RRC_CELL_RESELECTION,
                                          FALSE);
      rrcpcrc_send_initiate_cu_flag = FALSE;
      /* No need to send ul data registration to rlc. This will be taken care
      by cellupdate procedure */
      rrcpcrc_process_successful_proc_completion(FALSE);
#ifdef FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE
      #error code not present
#endif

      return;
    }
    else
    {
      /*Perform the final steps for successful completion of the
      procedure */
      rrcpcrc_process_successful_proc_completion(TRUE);
#ifdef FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE
      #error code not present
#endif

      /* return from here */
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
        rrcrb_send_ura_update_initiate_req( RRC_PROCEDURE_PCR,
                                          RRC_URA_RESELECTION,
                                          FALSE);
      }
    }
    /* send ul data registration to rlc. */
    rrcpcrc_process_successful_proc_completion(TRUE);
#ifdef FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE
      #error code not present
#endif

    /* return from here */
    return;
  }

  if(ptr->inter_freq_hho_status == TRUE)
  {
    pcrc.hho_ciph_status = TRUE;
  }
  else
  {
    pcrc.hho_ciph_status = FALSE;  
  }
  
  /*check whether this reconfiguration involves SRNS relocation */
  if (pcrc.pcrc_for_srns_relocation == TRUE)
  {
    if (RRC_STATE_CELL_DCH == curr_state)
    {

      /* Re-establish SRB2 before sending the response message */
      rb_type  = RRC_RE_EST_RB_2;
      rrcrb_send_rlc_re_est_req_to_smc(RRC_PROCEDURE_PCR, rb_type, TRUE);
  
      pcrc.curr_substate = RRCRB_WAIT_FOR_REESTABLISH_SRB2;
    }
    else
    {
      WRRC_MSG1_ERROR("SRNS not supp. in state %d", curr_state);
    }
  }
  else /*normal reconfiguration without SRNS relocation */
  {   
    /* Send the Radio Bearer Reconfig Complete message */

    if ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
        (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))

    {
      send_pcrc_complete_message(TRUE);
    }
    else
    {
    /* Need to wait for L2 ack for all states because RRC needs to trigger include reconfig status indicator
    if RLC acknowledgement is not yet received.
    As per 8.3.1.3
    1> if the IE "Cell update cause" is set to "radio link failure" and the UE has submitted a reconfiguration response
    message to RLC and the UE has not received the RLC acknowledgement for the response message:
    2> include the IE "Reconfiguration Status Indicator" and set it to TRUE.*/
        rrc_update_reconfig_waiting_for_l2ack_dch(RRC_PROCEDURE_PCR);
        send_pcrc_complete_message(TRUE);
      if((rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG)
        && (rrc_get_state() == RRC_STATE_CELL_DCH))
      { 
        /* wait for some time before initiating cell update */
        WRRC_MSG1_HIGH("Delaying by %d ms ", RRCRB_PND_CFG_DELAY);
        (void) rex_clr_sigs(rex_self(), RRCPCRC_TEMP_TIMER_SIG);
        (void) rex_set_timer(&rrcpcrc_temp_timer, RRCRB_PND_CFG_DELAY);  

        rrcpcrc_sig = rex_wait(RRCPCRC_TEMP_TIMER_SIG);
        MSG_LOW("RRCPCRC signal:%d", rrcpcrc_sig, 0, 0);
        (void) rex_clr_sigs(rex_self(), RRCPCRC_TEMP_TIMER_SIG);
        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_PCR, RRC_UNRECOVERABLE_ERROR, FALSE);   
      } 
      else if((rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG) &&
      (rrccu_get_cell_update_started_status_with_cause(&cu_cause) == RRCCU_STARTED))
      {
        if((cu_cause == rrc_CellUpdateCause_rlc_unrecoverableError) && (rrccu_get_rb_id_unrec_error() != DCCH_AM_RADIO_BEARER_ID))
        {
          /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
          rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_PCR,
                                              RRC_UNRECOVERABLE_ERROR,
                                              TRUE);
        }
        else if(cu_cause == rrc_CellUpdateCause_radiolinkFailure)
        {
          /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
          rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_PCR,
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
    /*if next state is Cell_PCH or URA_PCH then we have to 
    treat the procedure as successful only after getting L2ack
    for the response message */
    if ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
        (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))

    {
      /* Change the substate to the next substate - this is
      where we wait for L2 ack from RLC for sending the RB RC
      complete message. */
      pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
    }
    else
    {
      /* clear the ordered config here
      treat the procedure as successful. */
      rrcllc_clear_ordered_config();  

      /* send the nas cn info if stored */
      if (pcrc.cn_info_saved != FALSE)
      {
        /* Put the command on MM queue */
        rrc_send_rrc_message_to_mm(pcrc.mm_cmd_ptr);
        /* clear the flag */
        pcrc.cn_info_saved = FALSE;
      }
      /* Go to the next sub-state in the state machine. That's
      the initial substate since we've sent the RB RC complete
      message */
      pcrc.curr_substate = RRCRB_INITIAL;

      /* At this point we don't need to remember any of the
      Radio Bearer information since it's stored in ESTABLISHED_RABS.
      Clear our internal variables */
      rrcpcrc_clear_procedure();
    }
  
  }
}

/*===========================================================================

FUNCTION   RRCPCRC_PROCESS_RLC_REESTABLISHMENT_CNF

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
static void rrcpcrc_process_rlc_reestablishment_cnf
(
void
)
{
  rrcrb_rnti_update_info_type rnti_update_info; /* structure to be used to request RNTI update */

  /* send rnti update request if URNTI was present in the rebsetup message */
  if (pcrc.new_urnti_valid == TRUE)
  {
    /* Send the RNTI update request to rrcllc to update with new-URNTI */
    /* Send a RNTI Update Request */
    rnti_update_info.crnti_action         = RRC_RNTI_NOCHANGE;
    rnti_update_info.urnti_action         = RRC_RNTI_UPDATE;
    rnti_update_info.urnti                = pcrc.new_urnti;
    rnti_update_info.procedure            = RRC_PROCEDURE_PCR;
    rnti_update_info.rlc_re_establish_srb = FALSE;
    rnti_update_info.rlc_re_establish_rab = FALSE;
    rnti_update_info.cnf_reqd             = FALSE;

    rrcrb_send_rnti_update_req(rnti_update_info);
  }

  /* Send the reconfiguration complete message */

  send_pcrc_complete_message(TRUE);


  /* Change the substate to the next substate - this is
     where we wait for L2 ack from RLC for sending the RB Reconfig
     complete message. */
  pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
  /* prints rate for each transport channel */
  rrcllc_print_trans_chnl_rate();
  
}

/*===========================================================================

FUNCTION   RRCPCRC_PROCESS_STATE_CHANGE_IND

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
static void rrcpcrc_process_state_change_ind
(
  rrc_state_change_ind_type *ptr /* Pointer to the State Change Ind cmd */
)
{
  rrc_CellUpdateCause cu_cause;
  /* Check if we're going to disconnected state */
  if( ptr->new_state == RRC_STATE_DISCONNECTED )
  {
    /* Then change to initial substate */
    pcrc.curr_substate = RRCRB_INITIAL;

    /* Then clear all local variables that may
    need to be cleared */
    rrcpcrc_clear_procedure();
  }

  else if(ptr->new_state == RRC_STATE_CELL_FACH )
  {
    if(ptr->previous_state == RRC_STATE_CELL_DCH)
    {
      if (((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
          (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)) &&
           (pcrc.curr_substate == RRCRB_WAIT_FOR_L2_ACK))
      {
        WRRC_MSG1_HIGH("state change ind pcrc_st:%d",pcrc.curr_substate); 
        /* Then change to initial substate */
        pcrc.curr_substate = RRCRB_INITIAL;

        /* Then clear all local variables that may
        need to be cleared */
        rrcpcrc_clear_procedure();
      }
      else if(pcrc.status.failure_status == RRCPC_MSG_VALID)
      {
        /* if cell update procedure is active with cause OOS, wait 
         * for the CU complete notification to send chan_config_req to
         * LLC 
         */
        if ((rrccu_get_cell_update_started_status_with_cause(&cu_cause)== RRCCU_STARTED) &&
            (cu_cause == rrc_CellUpdateCause_re_enteredServiceArea) && 
            (rrcllc_get_toc_usage() == TOC_FOR_OOS)) 
        {
          rrcpcrc_delete_crnti = TRUE;
          WRRC_MSG0_HIGH("Complete pcrc msg first.  Indicate compl to CU once done");
        }

        MSG_MED("Sending RRC_CHANNEL_CONFIG_REQ to LLC", 0, 0, 0);
        /* Call the function that creates and sends the  channel
           config information to be requested from LLC. */
        rrcpcrc_send_channel_config_req();
        /* Change the substate since we're waiting for the
           confirmation from LLC for the channel configs */
        pcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;

        if((rrccsp_is_selected_cell_utran_directed()) &&
           (pcrc.new_crnti_valid))
        {
          /* No need to initiate cell update as the C-RNTI is available
           * and UE is camped on the directed cell.
           */
          rrcpcrc_send_initiate_cu_flag=FALSE;
        }
        else
        {
          rrcpcrc_send_initiate_cu_flag=TRUE;
          rrcpcrc_delete_crnti = TRUE;
        }
#ifdef FEATURE_WCDMA_HS_FACH
 /* If H-RNTI is not given in OTA then initiate cell update*/
        if(rrcenhstate_set_hrnti_status_for_transitions())
        {
          rrcpcrc_send_initiate_cu_flag = TRUE;
          rrcpcrc_delete_crnti          = TRUE;
        }
#endif
#ifdef FEATURE_WCDMA_HS_RACH
 /* Check if E-RNTI is not given in OTA then initiate cell update*/
        if(rrchsrach_check_ernti_status())
        {
          rrcpcrc_send_initiate_cu_flag = TRUE;
          rrcpcrc_delete_crnti          = TRUE;
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
        if (pcrc.directed_cell_info )
        {
          if (rrccsp_is_selected_cell_utran_directed()) 
          {
            /* No need to initiate cell update UE is camped on 
            the directed cell. */
            WRRC_MSG0_HIGH("Directed cell match, No CU reqd");
            rrcpcrc_send_initiate_cu_flag = FALSE;
          }
          else
          {
            rrcpcrc_send_initiate_cu_flag = TRUE;
            rrcpcrc_delete_crnti = TRUE;
          }
        }
        else
        {
          /* we have to initiate cell update incase cpich info is not present
          in the message */
          rrcpcrc_send_initiate_cu_flag = TRUE;
          rrcpcrc_delete_crnti = TRUE;
        }
      }
      else
      {
        /* URA_PCH case */
        rrcpcrc_send_initiate_cu_flag = FALSE;
      }

      /* Call the function that creates and sends the  channel
         config information to be requested from LLC. */
      rrcpcrc_send_channel_config_req();
  
      /* Change the substate since we're waiting for the
         confirmation from LLC for the channel configs */
      pcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
    }
    else if ((ptr->previous_state == RRC_STATE_CELL_FACH) &&
    (pcrc.initiate_cell_selection_trans_from_fach == TRUE))
    {
      /* Call the function that creates and sends a channel
      config request to LLC. */
      pcrc.initiate_cell_selection_trans_from_fach = FALSE;
      if (pcrc.directed_cell_info )
      {
        if(rrccsp_is_selected_cell_utran_directed())
        {
          /* No need to initiate cell update as the C-RNTI is available
           * and UE is camped on the directed cell.
           */
          rrcpcrc_send_initiate_cu_flag = FALSE;
        }
        else
        {
          rrcpcrc_send_initiate_cu_flag = TRUE;
          rrcpcrc_delete_crnti = TRUE;
        }
      }
      else
      {
        rrcpcrc_send_initiate_cu_flag = TRUE;
        rrcpcrc_delete_crnti = TRUE;
      }
      rrcpcrc_send_channel_config_req();
      /* Change the substate since we're waiting for the
      confirmation from LLC for the channel configs */
      pcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
    }

    else
    {
      MSG_HIGH("Recvd state change %d to %d pcrc state %d, No action",
                ptr->previous_state, ptr->new_state, pcrc.curr_substate);
    }

    /* IF CU is waiting for RB proc to complete, no need to initate CU through RB procedure. Reset flag. */
    if (rrccu_return_cu_substate() == RRCCU_WAIT_FOR_RB_PROC_COMPL_IND) 
    {
      rrcpcrc_send_initiate_cu_flag = FALSE;
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

FUNCTION   RRCPCRC_PROCESS_L2_ACK_IND

DESCRIPTION

  This function processes the received Layer 2 ACK indication.
  If the L2 ACK indicates that the PCRC complete message has
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
static void rrcpcrc_process_l2_ack_ind
(
  rrc_downlink_l2ack_ind_type *ptr /* Pointer to the L2 Ack Ind cmd */
)
{
  rrc_state_e_type                       current_state;       /* current RRC State  */

  rrc_rlc_re_est_for_srns_rb_e_type rb_type; /*rb type tobe reestablished */
  rrc_CellUpdateCause       cu_cause;
  uint8                    rejected_tr_id = RRCPC_INVALID_TRANSACTION_ID;
  WRRC_MSG1_HIGH("PCRC:L2 ack %d <0:Success 1:Failure>for RBS Complete msg received", ptr->status);

  if( ptr->status == SUCCESS )
  {
    current_state = rrc_get_state();

    /* Back up accepted transaction ID */
    rejected_tr_id = pcrc.tr_ids.accepted_transaction;
    
    /* Clear the transaction ID */
    pcrc.tr_ids.accepted_transaction = RRCPC_INVALID_TRANSACTION_ID;
      
    if ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
        (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
    {
      if(current_state==RRC_STATE_CELL_DCH) 
      {
        /*L2 Ack for the PCRC Complete message is received on the old configuration
          Now initiate a cell selection */
        rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_PCR,
                                                RRC_TRANSITION_FROM_DCH,
                                                pcrc.state_indicated_by_msg);

        /*wait for the state_change indication after the cell selection
        is successfully completed */
        pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

        /* Start T_305 [periodic cell update] timer */
        rrccu_start_t_305_timer();

      }
      else if(current_state==RRC_STATE_CELL_FACH) 
      {
        /* 
           Trigger cell update with cause OOS if inter frequency reselection is in progress
           Procedure substate remains same, once CU is compelted, failure meassage will be sent 
           to NW.
        */
        if(rrccsp_is_inter_frequency_cell_resel_in_progress() == TRUE)
        {
          rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_PCR,
                                              RRC_REENTERED_SERVICE_AREA,
                                              TRUE);
          return;
        }
        if (RRCCU_STARTED == rrccu_get_cell_update_started_status_with_cause(&cu_cause) &&
            (cu_cause == rrc_CellUpdateCause_re_enteredServiceArea || cu_cause == rrc_CellUpdateCause_rlc_unrecoverableError ||
             cu_cause == rrc_CellUpdateCause_radiolinkFailure))
        {
          /* This is a case where cell update is in progress while PCR transitions 
             UE to Cell_PCH or URA_PCH. Though L2 Ack is gotten, send a PCR failure 
             later when CU procedure gets Cell update confirm. If we were to act on 
             L2 Ack, RRC moves to Cell_PCH/URA_PCH where cell update confirm message
             is not handled (unless CU registers on PCR's behalf) & UE would stuck 
             in Cell_PCH forever [in the absence of reselections].
          */
          if(SUCCESS != rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_PCR))
          {
            WRRC_MSG0_ERROR("PCR procedure failed to register with CU");
          }
          WRRC_MSG1_HIGH("Registered for Cell Update procedure completion: cu_cause = %d", cu_cause);

          pcrc.status.failure_status = RRCPC_PHY_CHAN_FAILURE;
          pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;

          /* Restore the accepted transaction ID for sending failure later */
          pcrc.tr_ids.rejected_transaction = rejected_tr_id;
        }
        else
        {
          if((pcrc.initiate_cell_selection_trans_from_fach == TRUE )
            &&(rrccsp_check_initiate_cell_selection_handling(NULL) == TRUE))
          {
            /*Initiate the cell selection*/
            rrcllcoc_update_toc_with_oc_from_fach();   
             
            rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_PCR,
                                                    RRC_TRANSITION_FROM_FACH,
                                                    pcrc.state_indicated_by_msg);
 
 
             
            pcrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
          }
          else
          {
            pcrc.initiate_cell_selection_trans_from_fach = FALSE;
            /* Call the function that creates and sends a channel
               config request to LLC. */
            rrcpcrc_send_channel_config_req();
            /* Change the substate since we're waiting for the
               confirmation from LLC for the channel configs */
            pcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
          }
        }
      }
      else
      {
        WRRC_MSG2_ERROR("Invalid combination of rrcstate%d and rrcpcrc substate%d",
               current_state, pcrc.curr_substate);
      }

      /*return from here */
      return;
    }
    /* if SRNS relocation is involved with this message then re
    establish the RBs other than RB2 here */
    if (pcrc.pcrc_for_srns_relocation)
    {
      if (RRC_STATE_CELL_DCH == rrc_get_state())
      {
        /* Initiate the re-establishment of remaining RBs before 
        PCRC procedure goes back to initial substate. 
        No need to wait for the confirmation */
        rb_type  = RRC_RE_EST_NON_RB_2_UM_AM;
        rrcrb_send_rlc_re_est_req_to_smc(RRC_PROCEDURE_PCR, rb_type, FALSE);

        /* send the nas cn info if stored */
        if (pcrc.cn_info_saved != FALSE)
        {
          /* Put the command on MM queue */
          rrc_send_rrc_message_to_mm(pcrc.mm_cmd_ptr);
          /* clear the flag */
          pcrc.cn_info_saved = FALSE;
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
      (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_PCR);
       /* Wait until Cell update procedure is completed */
      WRRC_MSG1_HIGH("NACK being ingnored as PCRC failure msg will be sent/Registered for Cell Update procedure completion: cu_cause = %d", cu_cause);
      return;
    }

    /* Clear the transaction ID */
    pcrc.tr_ids.accepted_transaction = RRCPC_INVALID_TRANSACTION_ID;

    if (pcrc.pcrc_for_srns_relocation) 
    {
      /* l2ack failed. If reconfiguration involves SARNS relocation then
      go to idle. Otherwise just complete the procedure */
      WRRC_MSG2_ERROR("L2ack failed SRNSreloc%d, next_st:%d",
          pcrc.pcrc_for_srns_relocation,
          pcrc.state_indicated_by_msg);
      /* Going to Idle should always go through CSP  */
      rrc_transition_to_disconnected_state(RRC_PROCEDURE_PCR,RRC_TX_TO_DISCON_OTHER);
    }
  }

  /* Note OC is set at this point. No need to clear it
  if the next state is cell_pch or ura_pch and
  l2 ack failied because cu procedure will take care of it */
  if (((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH)) &&
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
      (void)rrcllc_reset_ordered_config(RRC_PROCEDURE_PCR,TRUE);
    }
  }
  else
  {
    /* Clear ORDERED_CONFIG - UE can now accept other ordered
    configurations */
    rrcllc_clear_ordered_config();  
  }
  
  if(pcrc.pcrc_for_srns_relocation == TRUE)
  {
    rrcdormancy_update_dormancy_for_srns_hho_cco(RRC_PROCEDURE_PCR);
  }
  
  /* Go to the next sub-state in the state machine. That's
  the initial substate since we've sent the PCRC complete
  message */
  pcrc.curr_substate = RRCRB_INITIAL;

  /*  Clear our internal variables */
  rrcpcrc_clear_procedure();
}


/*===========================================================================

FUNCTION   RRCPCRC_INITIAL_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the PCRC procedure is in the RRCRB_INITIAL substate. This function
  looks at the received event and does the appropriate processing
  of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcpcrc_initial_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the received command's id */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {

    case RRC_DOWNLINK_SDU_IND:
        /* Process the PC Reconfig message */
      process_pcrc_message( cmd_ptr );
      
      break;


    case RRC_COMPRESSED_MODE_IND:
      rrcpcrc_dch_compressed_mode_handler(&cmd_ptr->cmd.cm_ind);
      break;
      

    default:

      /* No other events except Downlink SDU is expected in this
      substate */
      MSG_HIGH("Command 0x%x ignored in substate%d rrc_state%d", 
              cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate, rrc_get_state());
      break;
  }
}
/*===========================================================================

FUNCTION   RRCPCRC_DCH_WFSRB2REESTABLISH_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBE procedure is in the RRCRB_WAIT_FOR_REESTABLISH_SRB2 substate
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
static void rrcpcrc_dch_wfsrb2reestablish_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  
  case RRC_RE_EST_RLC_FOR_SRNS_CNF:

    /* Confirmation from SMC of RLC re-establishment confirm */
    rrcpcrc_process_rlc_reestablishment_cnf( );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcpcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Reconfig Message. Otherwise
    we would not have received this command. Copy the 
    received message pointer. */
    /* Process the RB Reconfig message */
    process_pcrc_message( cmd_ptr );

    break;

  default:

    /* No other events except Downlink SDU is expected in this
    substate */
    MSG_HIGH("Command 0x%x ignored in substate%d rrc_state%d", 
            cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate, rrc_get_state());
    break;
  }
}

/*===========================================================================

FUNCTION   RRCPCRC_DCH_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the PCRC procedure is in the RRCRB_WAIT_FOR_CHAN_CFG_CNF substate
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
static void rrcpcrc_dch_wfchcnf_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the received command's id */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {

    case RRC_CHANNEL_CONFIG_CNF:
      
      /* Confirmation from LLC for the channel configurations */
      rrcpcrc_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
      break;

    case RRC_STATE_CHANGE_IND:
      
      /* Indication that RRC's state has changed */
      rrcpcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;

    case RRC_DOWNLINK_SDU_IND:
      
        /* Process the PC Reconfig message */
      process_pcrc_message( cmd_ptr);
          
      break;

    case RRC_COMPRESSED_MODE_IND:
      rrcpcrc_dch_compressed_mode_handler(&cmd_ptr->cmd.cm_ind);
      break;
      
    default:

      /* No other events expected in this
      substate */
      MSG_HIGH("Command 0x%x ignored in substate%d rrc_state%d", 
              cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate, rrc_get_state());
      break;
  }
}


/*===========================================================================

FUNCTION   rrcpcrc_other_wfcucnf_substate_event_handler

DESCRIPTION

  This function handles the events received by this module when
  the PCRC procedure is in the RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate
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
static void rrcpcrc_other_wfcucnf_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_STATE_CHANGE_IND:
      /* Indication that RRC's state has changed */
      rrcpcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;
    
    default:    
      /* No other events expected in this
      substate */
      MSG_HIGH("Command 0x%x ignored in substate%d rrc_state%d", 
              cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate, rrc_get_state());
      break;
  }
}


/*===========================================================================

FUNCTION   RRCPCRC_DCH_WFCUCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the PCRC procedure is in the RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate
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

static void rrcpcrc_dch_wfcucnf_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_CELL_UPDATE_COMPLETE_CNF:
      if(pcrc.status.failure_status == RRCPC_SIMULTANEOUS_RECONFIG)
      {
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message(pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause
                                );
        /* Then clear all local variables that may
        need to be cleared */
        rrcpcrc_clear_procedure();

        /* Make sure we're back in the initial sub-state */
        pcrc.curr_substate = RRCRB_INITIAL;
      }
      else if(pcrc.status.failure_status == RRCPC_MSG_VALID)
      {
        /* Send the TC Reconfig Complete message */
        if ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
            (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))

        {
          send_pcrc_complete_message(TRUE);
        }
        else
        {
          send_pcrc_complete_message(FALSE);
        }
        /* prints rate for each transport channel */
        rrcllc_print_trans_chnl_rate();
        /*if next state is Cell_PCH or URA_PCH then we have to 
        treat the procedure as successful only after getting L2ack
        for the response message */
        if ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
            (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))

        {
          /* Change the substate to the next substate - this is
          where we wait for L2 ack from RLC for sending the RB RC
          complete message. */
          pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
        }
        else
        {
          /* clear the ordered config here
          treat the procedure as successful. */
          rrcllc_clear_ordered_config();  
          /* send the nas cn info if stored */
          if (pcrc.cn_info_saved != FALSE)
          {
            /* Put the command on MM queue */
            rrc_send_rrc_message_to_mm(pcrc.mm_cmd_ptr);
            /* clear the flag */
            pcrc.cn_info_saved = FALSE;
          }

          /* Go to the next sub-state in the state machine. That's
          the initial substate since we've sent the RB RC complete
          message */
          pcrc.curr_substate = RRCRB_INITIAL;

          /* At this point we don't need to remember any of the
          Radio Bearer information since it's stored in ESTABLISHED_RABS.
          Clear our internal variables */
          rrcpcrc_clear_procedure();
        }
      }
      /*  In the case of Phy_Chan_Failure, RB setup failure must be send
          and oc should be reset. */
      else if(pcrc.status.failure_status == RRCPC_PHY_CHAN_FAILURE)
      {
        
        /* Send the RB Setup Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause
                                );

        /* Then clear all local variables that may
        need to be cleared */
        rrcpcrc_clear_procedure();
        /* Make sure we're back in the initial sub-state */
        pcrc.curr_substate = RRCRB_INITIAL;
      }
      else
      {
        WRRC_MSG1_ERROR("Failure status %d not handled",pcrc.status.failure_status);
      }

    break;

  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcpcrc_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcpcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* Process the PC Reconfig message */
    process_pcrc_message( cmd_ptr );
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_HIGH("Command 0x%x ignored in substate%d rrc_state%d", 
            cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate, rrc_get_state());
    break;
  }

}


/*===========================================================================

FUNCTION   RRCPCRC_FACH_WFCUCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the PCRC procedure is in the RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate
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

static void rrcpcrc_fach_wfcucnf_event_handler
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
      if(pcrc.status.failure_status == RRCPC_SIMULTANEOUS_RECONFIG)
      {
        /* Send the PC Reconfig Failure Message */
        send_pcrc_failure_message(pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause
                                );
        /* Then clear all local variables that may
        need to be cleared */
        rrcpcrc_clear_procedure();

        /* Make sure we're back in the initial sub-state */
        pcrc.curr_substate = RRCRB_INITIAL;
      }
      else if(pcrc.status.failure_status == RRCPC_MSG_VALID)
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
          rrcpcrc_send_channel_config_req();
  
          /* Change the substate since we're waiting for the
             confirmation from LLC for the channel configs */
          pcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;	
          return;					
        }
      
        /* Send the TC Reconfig Complete message */
        if ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
            (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
          
        {
          send_pcrc_complete_message(TRUE);
        }
        else
        {
          send_pcrc_complete_message(FALSE);
        }
        /*if next state is Cell_PCH or URA_PCH then we have to 
          treat the procedure as successful only after getting L2ack
          for the response message */
        if ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
            (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
          
        {
          /* Change the substate to the next substate - this is
             where we wait for L2 ack from RLC for sending the RB RC
             complete message. */
          pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
        }
        else
        {
          /* clear the ordered config here
             treat the procedure as successful. */
          rrcllc_clear_ordered_config();  
          /* send the nas cn info if stored */
          if (pcrc.cn_info_saved != FALSE)
          {
            /* Put the command on MM queue */
            rrc_send_rrc_message_to_mm(pcrc.mm_cmd_ptr);
            /* clear the flag */
            pcrc.cn_info_saved = FALSE;
          }

          /* Go to the next sub-state in the state machine. That's
             the initial substate since we've sent the RB RC complete
             message */
          pcrc.curr_substate = RRCRB_INITIAL;
            
          /* At this point we don't need to remember any of the
             Radio Bearer information since it's stored in ESTABLISHED_RABS.
             Clear our internal variables */
          rrcpcrc_clear_procedure();
        }
      }
      /*  In the case of Phy_Chan_Failure, RB setup failure must be send
          and oc should be reset. */
      else if(pcrc.status.failure_status == RRCPC_PHY_CHAN_FAILURE)
      {
        
        /* Send the RB Setup Failure Message */
        send_pcrc_failure_message( pcrc.status.failure_status,
                                  pcrc.status.prot_err_cause
                                );


        /* Then clear all local variables that may
        need to be cleared */
        rrcpcrc_clear_procedure();
        /* Make sure we're back in the initial sub-state */
        pcrc.curr_substate = RRCRB_INITIAL;
      }
      else
      {
        WRRC_MSG1_ERROR("Failure status %d not handled",pcrc.status.failure_status);
      }

    break;

  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcpcrc_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcpcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* Process the PC Reconfig message */
    process_pcrc_message( cmd_ptr );
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_HIGH("Command 0x%x ignored in substate%d rrc_state%d", 
            cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate, rrc_get_state());
    break;
  }

}
/*===========================================================================

FUNCTION   RRCPCRC_PCH_WF_CSCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the PCRC procedure is in the RRCRB_WAIT_FOR_CELL_SELECTION_CNF substate
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

static void rrcpcrc_pch_wfcscnf_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_STATE_CHANGE_IND:
   /* Indication that RRC's state has changed */
    rrcpcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
   
    break;


  default:

    /* No other events expected in this
    substate */
    MSG_HIGH("Command 0x%x ignored in substate%d rrc_state%d", 
            cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate, rrc_get_state());
    break;
  }

}



/*===========================================================================

FUNCTION   RRCPCRC_PCH_WFCUCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the PCRC procedure is in the RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate
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
static void rrcpcrc_pch_wfcucnf_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  case RRC_CELL_UPDATE_COMPLETE_CNF:
    if(pcrc.status.failure_status != RRCPC_MSG_VALID)
    {
      /* Send the PC Reconfig Failure Message */
      send_pcrc_failure_message(pcrc.status.failure_status,
                                pcrc.status.prot_err_cause
                              );
      /* Then clear all local variables that may
      need to be cleared */
      rrcpcrc_clear_procedure();

      /* Make sure we're back in the initial sub-state */
      pcrc.curr_substate = RRCRB_INITIAL;
    }
    else 
    {
      /* Send the TC Reconfig Complete message */
      if ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
          (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))

      {
        send_pcrc_complete_message(TRUE);
      }
      else
      {
        send_pcrc_complete_message(FALSE);
      }
      /* prints rate for each transport channel */
      rrcllc_print_trans_chnl_rate();
      /*if next state is Cell_PCH or URA_PCH then we have to 
      treat the procedure as successful only after getting L2ack
      for the response message */
      if ((pcrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
          (pcrc.state_indicated_by_msg == RRC_STATE_URA_PCH))

      {
        /* Change the substate to the next substate - this is
        where we wait for L2 ack from RLC for sending the RB RC
        complete message. */
        pcrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
      }
      else
      {
        /* clear the ordered config here
        treat the procedure as successful. */
        rrcllc_clear_ordered_config();  

        /* Go to the next sub-state in the state machine. That's
        the initial substate since we've sent the RB RC complete
        message */
        pcrc.curr_substate = RRCRB_INITIAL;

        /* At this point we don't need to remember any of the
        Radio Bearer information since it's stored in ESTABLISHED_RABS.
        Clear our internal variables */
        rrcpcrc_clear_procedure();
      }
    }
    break;

  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcpcrc_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:
    /* Indication that RRC's state has changed */
    rrcpcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

 

    case RRC_DOWNLINK_SDU_IND:
      WRRC_MSG2_HIGH("Unexpected DOWNLINK_SDU_IND in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate);
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_HIGH("Command 0x%x ignored in substate%d rrc_state%d", 
            cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate, rrc_get_state());
    break;
  }

}

/*===========================================================================

FUNCTION   RRCPCRC_PCH_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the PCRC procedure is in the RRCRB_WAIT_FOR_CHAN_CFG_CNF substate
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
static void rrcpcrc_pch_wfchcnf_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{


  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  
  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcpcrc_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcpcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:
  default:

    /* No other events expected in this
    substate */
    MSG_HIGH("Command 0x%x ignored in substate%d rrc_state%d", 
            cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate, rrc_get_state());
    break;
  }
}


/*===========================================================================

FUNCTION   RRCPCRC_FACH_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the PCRC procedure is in the RRCRB_WAIT_FOR_CHAN_CFG_CNF substate
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
static void rrcpcrc_fach_wfchcnf_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {

    case RRC_CHANNEL_CONFIG_CNF:
      
      /* Confirmation from LLC for the channel configurations */
      rrcpcrc_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
      break;

    case RRC_STATE_CHANGE_IND:
      
      /* Indication that RRC's state has changed */
      rrcpcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;

    case RRC_DOWNLINK_SDU_IND:
      
        /* Process the PC Reconfig message */
        // Reject - upen ? 
      process_pcrc_message( cmd_ptr );
      
      break;

    default:

      /* No other events expected in this
      substate */
      MSG_HIGH("Command 0x%x ignored in substate%d rrc_state%d", 
              cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate, rrc_get_state());
      break;
  }
}



/*===========================================================================

FUNCTION   RRCPCRC_OTHER_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the PCRC procedure is in the RRCRB_WAIT_FOR_CHAN_CFG_CNF substate
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
static void rrcpcrc_other_wfchcnf_substate_event_handler
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
      WRRC_MSG1_HIGH("Ignoring chan config cnf/downlink sdu since UE not in DCH or FACH and cmd_id:%0x", cmd_ptr->cmd_hdr.cmd_id);
      /* In this state this procedure should no longer be active. */

      /* Then change to initial substate */
      pcrc.curr_substate = RRCRB_INITIAL;

      /* Then clear all local variables that may
      need to be cleared */
      rrcpcrc_clear_procedure();
      break;

    case RRC_STATE_CHANGE_IND:
      
      /* Indication that RRC's state has changed */
      rrcpcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;

    default:

      /* No other events expected in this
      substate */
      MSG_HIGH("Command 0x%x ignored in substate%d rrc_state%d", 
              cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate, rrc_get_state());
      break;
  }
}



/*===========================================================================

FUNCTION   RRCPCRC_DCH_WFL2ACK_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the PCRC procedure is in the RRCRB_WAIT_FOR_L2_ACK substate
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
static void rrcpcrc_dch_wfl2ack_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {

    case RRC_DOWNLINK_L2ACK_IND:
      
      /* Confirmation from LLC for the channel configurations */
      rrcpcrc_process_l2_ack_ind( &(cmd_ptr->cmd.downlink_l2ack_ind) );
      break;

    case RRC_STATE_CHANGE_IND:
      
      /* Indication that RRC's state has changed */
      rrcpcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;

    case RRC_DOWNLINK_SDU_IND:
      
        /* Process the PC Reconfig message */
      process_pcrc_message( cmd_ptr );
      
      break;

    case RRC_COMPRESSED_MODE_IND:
      rrcpcrc_dch_compressed_mode_handler(&cmd_ptr->cmd.cm_ind);
      break;
      

    case RRC_CELL_UPDATE_COMPLETE_CNF:
      /* The only reason this scenario could occur is if while in wait_for_l2_ack substate oos area
         happened. In this case, send failure msg with cause cell update occured 
      */
      pcrc.status.failure_status = RRCPC_CELL_UPDATE_OCCURED;

      /* Send the PC Reconfig Failure Message */
      send_pcrc_failure_message(pcrc.status.failure_status,
                                pcrc.status.prot_err_cause
                                );

      /* Then clear all local variables that may
      need to be cleared */
      rrcpcrc_clear_procedure();

      /* Make sure we're back in the initial sub-state */
      pcrc.curr_substate = RRCRB_INITIAL;
      break;

    default:

      /* No other events except Downlink SDU is expected in this
      substate */
      MSG_HIGH("Command 0x%x ignored in substate%d rrc_state%d", 
              cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate, rrc_get_state());
      break;
  }
}


/*===========================================================================

FUNCTION   RRCPCRC_FACH_WFL2ACK_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the PCRC procedure is in the RRCRB_WAIT_FOR_L2_ACK substate
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
static void rrcpcrc_fach_wfl2ack_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  
  /* Switch on the received command's id */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {

    case RRC_DOWNLINK_L2ACK_IND:
      
      /* Confirmation from LLC for the channel configurations */
      rrcpcrc_process_l2_ack_ind( &(cmd_ptr->cmd.downlink_l2ack_ind) );
      break;

    case RRC_STATE_CHANGE_IND:
      
      /* Indication that RRC's state has changed */
      rrcpcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;

    case RRC_DOWNLINK_SDU_IND:
      
        /* Process the PC Reconfig message */
      process_pcrc_message( cmd_ptr );
      
      break;
  
    case RRC_CELL_UPDATE_COMPLETE_CNF:
      /* The only reason this scenario could occur is if while in wait_for_l2_ack substate oos area
         happened. In this case, send failure msg with cause cell update occured 
      */
      pcrc.status.failure_status = RRCPC_CELL_UPDATE_OCCURED;
      /* Send the PC Reconfig Failure Message */
      send_pcrc_failure_message(pcrc.status.failure_status,
                                pcrc.status.prot_err_cause
                                );
      /* Then clear all local variables that may
      need to be cleared */
      rrcpcrc_clear_procedure();

      /* Make sure we're back in the initial sub-state */
      pcrc.curr_substate = RRCRB_INITIAL;
      break;

    default:

      /* No other events except Downlink SDU is expected in this
      substate */
      MSG_HIGH("Command 0x%x ignored in substate%d rrc_state%d", 
              cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate, rrc_get_state());
      break;
  }
}



/*===========================================================================

FUNCTION   RRCPCRC_FACH_WFCSCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the PCRC procedure is in the RRCRB_WAIT_FOR_CELL_SELECTION_CNF substate
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
static void rrcpcrc_fach_wfcscnf_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
   /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

    case RRC_STATE_CHANGE_IND:
     /* Indication that RRC's state has changed */
      rrcpcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;
    case RRC_NEW_CELL_IND:
      rrcpcrc_process_new_cell_ind( &(cmd_ptr->cmd.new_cell_ind) );
      break;

    case RRC_CELL_UPDATE_COMPLETE_CNF:
      if(pcrc.state_indicated_by_msg == RRC_STATE_CELL_FACH)
      {
        /* if OOS was detected during trans from DCH, cell update
           procedure has completed.  send channel config req to 
           complete the reconfiguration procedure
          */
        if( (rrcllc_get_toc_usage() ==TOC_FOR_OOS) 
            || (rrcllc_get_toc_usage() ==TOC_FOR_OOS_WITHOUT_DCH_INFO)
            ||(rrcllc_get_toc_usage() ==TOC_FOR_OOS_WITH_DCH_INFO)
         )
        {
          /* Call the function that creates and sends the  channel
          config information to be requested from LLC. */
          rrcpcrc_send_channel_config_req();
          /* Also reset the rrcpcrc_send_initiate_cu_flag to false as cell update
           * will not be needed since CU and CUCNF for oos took care 
           * of that
           */
          rrcpcrc_send_initiate_cu_flag = FALSE;
          /* Change the substate since we're waiting for the
          confirmation from LLC for the channel configs */
          pcrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
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
      MSG_HIGH("Command 0x%x ignored in substate%d rrc_state%d", 
              cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate, rrc_get_state());
      break;
   }


}
/*===========================================================================

FUNCTION   RRCPCRC_FACH_WFCRNTIUPDATECNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the PCRC procedure is in the RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF substate
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

static void rrcpcrc_fach_wfcrntiupdatecnf_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_STATE_CHANGE_IND:
   /* Indication that RRC's state has changed */
    rrcpcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
   
    break;


  case RRC_RNTI_UPDATE_CNF:
    if (cmd_ptr->cmd.rnti_update_cnf.status == SUCCESS) 
    {
      rrcpcrc_process_rnti_update_cnf();
    }
    else
    {
      WRRC_MSG0_HIGH("RNTI update failed.  Send PCRC failure msg");
      pcrc.status.failure_status = RRCPC_SIMULTANEOUS_RECONFIG;
      /* Send the TCRC Failure Message */
      send_pcrc_failure_message( pcrc.status.failure_status,
                                 pcrc.status.prot_err_cause );

      
      /* Then clear all local variables that may
         need to be cleared */
      rrcpcrc_clear_procedure();

      /* Make sure we're back in the initial sub-state */
      pcrc.curr_substate = RRCRB_INITIAL;
    }
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_HIGH("Command 0x%x ignored in substate%d rrc_state%d", 
            cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate, rrc_get_state());
    break;
  }

}  /* rrcpcrc_fach_wfcrntiupdatecnf_substate_event_handler */

/*===========================================================================

FUNCTION   RRCPCRC_OTHER_WFL2ACK_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the PCRC procedure is in the RRCRB_WAIT_FOR_L2_ACK substate
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
static void rrcpcrc_other_wfl2ack_substate_event_handler
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
      WRRC_MSG1_HIGH("Ignoring L2 ack/downlink SDU since UE not in DCH or FACH cmd_id:%0x", cmd_ptr->cmd_hdr.cmd_id);
      /* In this state this procedure should no longer be active. */

      /* Then change to initial substate */
      pcrc.curr_substate = RRCRB_INITIAL;

      /* Then clear all local variables that may
      need to be cleared */
      rrcpcrc_clear_procedure();
      break;

    case RRC_STATE_CHANGE_IND:
      
      /* Indication that RRC's state has changed */
      rrcpcrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;

    default:

      /* No other events except Downlink SDU is expected in this
      substate */
      WRRC_MSG2_HIGH("Ignoring PCRC event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate);
      break;
  }
}





/*===========================================================================

FUNCTION   RRCPCRC_CELL_DCH_EVENT_HANDLER

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
static void rrcpcrc_cell_dch_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  if ((cmd_ptr->cmd_hdr.cmd_id == RRC_DOWNLINK_L2ACK_IND) &&
      (pcrc.curr_substate != RRCRB_WAIT_FOR_L2_ACK))
  {
    /* We could get L2ack for a previously sent message while we are already
    processing the next message. So l2ack could be received any any of the following
    substates. If PCrc substate does not indicate that we are not waiting for l2ack
    then just print the status */
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"PCR:L2ack status %d <0-Success 1-Failure> for MUI:%d, Lc_Id:%d, rbe_subst:%d",
               cmd_ptr->cmd.downlink_l2ack_ind.status,
               cmd_ptr->cmd.downlink_l2ack_ind.mui, 
               cmd_ptr->cmd.downlink_l2ack_ind.lc_id, pcrc.curr_substate);

    /* return from here so that procedure can continue with existing process */
    return;
  }
  /* Switch on the current substate */
  switch( pcrc.curr_substate )
  {
    case RRCRB_INITIAL:
     
      /* Call the event handler for RRCRB_INITIAL substate */
      rrcpcrc_initial_substate_event_handler( cmd_ptr );
      break;

    case RRCRB_WAIT_FOR_CHAN_CFG_CNF:
     
      /* Call the event handler for RRCRB_WAIT_FOR_CHAN_CFG_CNF substate */
      rrcpcrc_dch_wfchcnf_substate_event_handler( cmd_ptr );
      break;

    case RRCRB_WAIT_FOR_L2_ACK:
     
      /* Call the event handler for RRCRB_WAIT_FOR_L2_ACK substate */
      rrcpcrc_dch_wfl2ack_substate_event_handler( cmd_ptr );
      break;

    case RRCRB_WAIT_FOR_REESTABLISH_SRB2:
      rrcpcrc_dch_wfsrb2reestablish_substate_event_handler(cmd_ptr);
      break;

    case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
      /* Call the event handler for RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate */
      rrcpcrc_dch_wfcucnf_event_handler( cmd_ptr);
      break;

    default:

      /* No other substates processed for this RRC state    */
      WRRC_MSG2_HIGH("Command %x not processed in CELL_DCH for substate %d", 
              cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate);
      break;

  }
}

                                                                  

/*===========================================================================

FUNCTION   RRCPCRC_CELL_FACH_EVENT_HANDLER

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
static void rrcpcrc_cell_fach_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  if ((cmd_ptr->cmd_hdr.cmd_id == RRC_DOWNLINK_L2ACK_IND) &&
      (pcrc.curr_substate != RRCRB_WAIT_FOR_L2_ACK))
  {
    /* We could get L2ack for a previously sent message while we are already
    processing the next message. So l2ack could be received any any of the following
    substates. If PCrc substate does not indicate that we are not waiting for l2ack
    then just print the status */
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"PCR:L2ack status %d <0-Success 1-Failure> for MUI:%d, Lc_Id:%d, rbe_subst:%d",
               cmd_ptr->cmd.downlink_l2ack_ind.status,
               cmd_ptr->cmd.downlink_l2ack_ind.mui, 
               cmd_ptr->cmd.downlink_l2ack_ind.lc_id, pcrc.curr_substate);

    /* return from here so that procedure can continue with existing process */
    return;
  }

  /* Switch on the current substate */
  switch( pcrc.curr_substate )
  {
    case RRCRB_INITIAL:
      /* Call the event handler for RRCRB_INITIAL substate */
      /* NOTE: Here we need to check if a cell-update procedure
      has been initialized or not. For now, just call the event handler */
      rrcpcrc_initial_substate_event_handler( cmd_ptr );
      break;
   
    case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
      /* Call the event handler for RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate */
      rrcpcrc_fach_wfcucnf_event_handler( cmd_ptr);
      break;

    case RRCRB_WAIT_FOR_CHAN_CFG_CNF:
      /* Call the event handler for RRCRB_WAIT_FOR_CHAN_CFG_CNF substate */
      rrcpcrc_fach_wfchcnf_substate_event_handler( cmd_ptr );
      break;

  case  RRCRB_WAIT_FOR_CELL_SELECTION_CNF:
      /* Call the event handler for RRCRB_WAIT_FOR_CHAN_CFG_CNF substate */
      rrcpcrc_fach_wfcscnf_substate_event_handler( cmd_ptr );
      break;


    case RRCRB_WAIT_FOR_L2_ACK:
      /* Call the event handler for RRCRB_WAIT_FOR_L2_ACK substate */
      rrcpcrc_fach_wfl2ack_substate_event_handler( cmd_ptr );
      break;

    case RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF:
        /* Call the event handler for RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF substate */
        rrcpcrc_fach_wfcrntiupdatecnf_substate_event_handler( cmd_ptr );
        break;

    default:
      /* No other substates processed for this RRC state    */
      WRRC_MSG2_HIGH("Command %x not processed in CELL_FACH for substate %d", 
              cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate);
      break;
  }
}


/*===========================================================================

FUNCTION   RRCPCRC_PCH_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when RRC
  is in CELL_PCH state. This module checks the current sub-state of
  this procedure and calls the appropriate sub-state function to
  process the received event.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcpcrc_pch_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the current substate */
  switch ( pcrc.curr_substate )
  {
  case RRCRB_INITIAL:

    /* Call the event handler for RRCRB_INITIAL substate */
    rrcpcrc_initial_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
    /* Call the event handler for RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate */
    rrcpcrc_pch_wfcucnf_event_handler( cmd_ptr);
    break;

  case RRCRB_WAIT_FOR_CELL_SELECTION_CNF:

    rrcpcrc_pch_wfcscnf_event_handler( cmd_ptr);
    break;

  case RRCRB_WAIT_FOR_CHAN_CFG_CNF:

    /* Call the event handler for RRCRB_WAIT_FOR_CHAN_CFG_CNF substate */
    rrcpcrc_pch_wfchcnf_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_L2_ACK:

    /* The only way we could be in pch and rb state is "wait_for_l2_ack" is if
       oos happened while waiting for l2 ack and cell update confirm takes UE to pch
       In this case, send failure msg of rbrc with cause cell update occured 
    */
    pcrc.status.failure_status = RRCPC_CELL_UPDATE_OCCURED;

    send_pcrc_failure_message(pcrc.status.failure_status,
                              pcrc.status.prot_err_cause);

    /* Then clear all local variables that may
       need to be cleared */
    rrcpcrc_clear_procedure();

    /* Make sure we're back in the initial sub-state */
    pcrc.curr_substate = RRCRB_INITIAL;
    break;


  default:

    /* No other substates processed for this RRC state    */
    MSG_HIGH("Command %x not processed in substate%d rrc_state%d", 
            cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate, rrc_get_state());
    break;

  }
}


/*===========================================================================

FUNCTION   RRCPCRC_OTHER_STATE_EVENT_HANDLER

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
static void rrcpcrc_other_state_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the current substate */
  switch( pcrc.curr_substate )
  {
    
    case RRCRB_WAIT_FOR_CHAN_CFG_CNF:
     
      /* Call the event handler for WAIT_FOR_CHAN_CFG_CNF substate */
      rrcpcrc_other_wfchcnf_substate_event_handler( cmd_ptr );
      break;

    case RRCRB_WAIT_FOR_L2_ACK:
     
      /* Call the event handler for WAIT_FOR_L2_ACK substate */
      rrcpcrc_other_wfl2ack_substate_event_handler( cmd_ptr );
      break;

    case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
      rrcpcrc_other_wfcucnf_substate_event_handler( cmd_ptr);
      break;

    default:

      /* No other substates processed for this RRC state    */
      MSG_MED("Command %x not processed in IDLE or PCH states for substate %d", 
              cmd_ptr->cmd_hdr.cmd_id, pcrc.curr_substate, 0);
      break;
  }
}



/*===========================================================================

FUNCTION RRCPCRC_PROCEDURE_EVENT_HANDLER


DESCRIPTION

  This is the main function handling external events received by the
  PCRC procedure. This function is called by the
  dispatcher whenever an event is received for this module. This function
  checks the current RRC state and calls the event handler for
  that state. 
  
  Note that the PCRC procedure can be initiated
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
void rrcpcrc_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  /* Get the current RRC State */
  rrc_state_e_type rrc_state = rrc_get_state();

  WRRC_MSG2_HIGH_OPT("function rrcpcrc_procedure_event_handler is called in rrc_state = rrc_state_e_type_value%d and pcrc.curr_substate = rrcrb_substate_e_type_value%d", rrc_state, pcrc.curr_substate);
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_PCR,pcrc.curr_substate,cmd_ptr->cmd_hdr.cmd_id);

   /* Process State Change to Idle Disconnected in all substates */
  if((cmd_ptr->cmd_hdr.cmd_id == RRC_STATE_CHANGE_IND) &&
     (cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED ))
  {
    /* Then change to initial substate */
    pcrc.curr_substate = RRCRB_INITIAL;
    /* Then clear all local variables that may
    need to be cleared */
    rrcpcrc_clear_procedure();
  }
  switch (rrc_state)
  {

    case RRC_STATE_CELL_FACH:    
      /* If it's CELL_FACH state, call the event handler
      for CELL_FACH state */
      rrcpcrc_cell_fach_event_handler(cmd_ptr);
      break;

    case RRC_STATE_CELL_DCH:    
      /* If it's CELL_DCH state, call the event handler
      for CELL_DCH state */
      rrcpcrc_cell_dch_event_handler(cmd_ptr);
      break;

    case RRC_STATE_CELL_PCH:
    case RRC_STATE_URA_PCH:
      /* If it's CELL_DCH state, call the event handler
      for CELL_PCH state */
      rrcpcrc_pch_event_handler(cmd_ptr);
      break;

    case RRC_STATE_CONNECTING:   
    case RRC_STATE_DISCONNECTED: 
      /* If it's any other state call the event handler
      for other states. The processing of events in
      other RRC states is identical for this procedure */
      rrcpcrc_other_state_event_handler(cmd_ptr);
      break;

    default:  
      WRRC_MSG1_ERROR("Invalid RRC State: %d", rrc_state);
      break;
  }  
}/* end function rrcpcrc_procedure_event_handler */


/*===========================================================================

FUNCTION rrcpcr_build_pcr_complete_message

DESCRIPTION
  This function builds the PCR Complete message.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrcpcr_build_pcr_complete_message
(
  rrc_RRC_TransactionIdentifier trans_id,   /* Transaction Id */
  rrc_UL_DCCH_Message *msg_ptr,             /* Pointer to Uplink DCCH Msg */
  boolean              srns_reloc,          /* Indicates whether SRNS reloc is involved */ 
  boolean            cipher_update_required /* Indicates whether TM ciphering is present */
)
{                                         

  /* Select the message as PCR Complete message */
  msg_ptr->message.t = T_rrc_UL_DCCH_MessageType_physicalChannelReconfigurationComplete;

  /* Initialize the bit mask not to include the optional fields */
 RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.physicalChannelReconfigurationComplete);

  if (srns_reloc == TRUE)
  {
    rrcpcrc_append_start_list_to_pcrc_complete(msg_ptr);
  }

  if((cipher_update_required == TRUE ) && (TRUE == rrcsmc_tm_ciph_active()))
  {
    if(! (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.physicalChannelReconfigurationComplete,
           rrc_PhysicalChannelReconfigurationComplete,ul_CounterSynchronisationInfo)))
    {
      rrcpcrc_append_start_list_to_pcrc_complete(msg_ptr);
    }
  
    //Set the bitmask for count_c_act_time and copy the same..
    RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.physicalChannelReconfigurationComplete,
      rrc_PhysicalChannelReconfigurationComplete,count_C_ActivationTime);
    
    msg_ptr->message.u.physicalChannelReconfigurationComplete.count_C_ActivationTime =
      rrcsmc_step_2_cipher_config_for_hho();  
  }

  
  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
  {
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
    if(rrcsib_is_dmcr_ie_needed() == TRUE)
    {
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.physicalChannelReconfigurationComplete, laterNonCriticalExtensions);
 
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.physicalChannelReconfigurationComplete.laterNonCriticalExtensions);
 
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.physicalChannelReconfigurationComplete.laterNonCriticalExtensions, v770NonCriticalExtensions);
 
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.physicalChannelReconfigurationComplete.laterNonCriticalExtensions.v770NonCriticalExtensions);
      
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.physicalChannelReconfigurationComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        physicalChannelReconfigurationComplete_v770ext);
 
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.physicalChannelReconfigurationComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        physicalChannelReconfigurationComplete_v770ext, deferredMeasurementControlReading);
 
      msg_ptr->message.u.physicalChannelReconfigurationComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        physicalChannelReconfigurationComplete_v770ext.deferredMeasurementControlReading = rrc_PhysicalChannelReconfigurationComplete_v770ext_IEs_deferredMeasurementControlReading_true;
    }
#endif
   }

  /* Initialize the Transaction Id */
  msg_ptr->message.u.physicalChannelReconfigurationComplete.
  rrc_TransactionIdentifier = trans_id;
} /* rrcpcr_build_pcr_complete_message */



/*===========================================================================

FUNCTION  RRCPCRC_INIT_PROCEDURE

DESCRIPTION

  This function initializes the PCRC procedure.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcpcrc_init_procedure( void )
{
//  uint32 count;

  /* Initialize the transaction id's for this procedure.
  Use a number that can't be an actual received transaction
  id. */
  pcrc.tr_ids.accepted_transaction = RRCPC_INVALID_TRANSACTION_ID;
  pcrc.tr_ids.rejected_transaction = RRCPC_INVALID_TRANSACTION_ID;

  pcrc.pcrc_for_srns_relocation = FALSE;
  pcrc.new_urnti_valid = FALSE;


  pcrc.hho_ciph_status = FALSE;

  /* set cn_info_saved to false */
  pcrc.cn_info_saved = FALSE;

  /* Initialize the current substate */
  pcrc.curr_substate = RRCRB_INITIAL;

  /* Clear the C-RNTI valid flag */
  pcrc.new_crnti_valid    = FALSE;
  pcrc.directed_cell_info = FALSE;

  /* Reset Initiate Cell Update Flag */
  rrcpcrc_send_initiate_cu_flag = FALSE;

  pcrc.status.failure_status = RRCPC_MSG_VALID;
  pcrc.high_mobility_ind = FALSE;

  /* Register for state change notification -
  We want to be notified when the state changes
  from CELL_DCH to CELL_FACH and from any state
  to disconnected state. */
  rrcscmgr_register_for_scn( RRC_PROCEDURE_PCR,  /* Procedure name */
                             RRC_STATE_CELL_DCH, /* From State     */
                             RRC_STATE_CELL_FACH /* To State       */
                           );

  rrcscmgr_register_for_scn( RRC_PROCEDURE_PCR,  /* Procedure name */
                             RRC_STATE_WILDCARD, /* From State     */
                             RRC_STATE_DISCONNECTED /* To State    */
                           );

  rrcscmgr_register_for_scn( RRC_PROCEDURE_PCR,  /* Procedure name */
                             RRC_STATE_CELL_DCH, /* From State     */
                             RRC_STATE_CELL_PCH /* To State       */
                           );
  rrcscmgr_register_for_scn( RRC_PROCEDURE_PCR,  /* Procedure name */
                             RRC_STATE_CELL_DCH, /* From State     */
                             RRC_STATE_URA_PCH /* To State       */
                           );

  rrcscmgr_register_for_scn( RRC_PROCEDURE_PCR,  /* Procedure name */
                             RRC_STATE_CELL_FACH, /* From State     */
                             RRC_STATE_CELL_PCH /* To State       */
                           );

  rrcscmgr_register_for_scn( RRC_PROCEDURE_PCR,  /* Procedure name */
                             RRC_STATE_CELL_FACH, /* From State     */
                             RRC_STATE_URA_PCH /* To State       */
                           );
  pcrc.initiate_cell_selection_trans_from_fach = FALSE;     

  /* Define a timer used to delay 5 seconds before
  sending the PC Reconfig Complete message */
  rex_def_timer(&rrcpcrc_temp_timer, rex_self(), RRCPCRC_TEMP_TIMER_SIG);

}

// #endif
/*===========================================================================

FUNCTION  RRCPCRC_DCH_COMPRESSED_MODE_HANDLER

DESCRIPTION

  This function sends a physical channel reconfiguration failure
  message to UTRAN to indicate a compressed mode run time error
  indication and also posts an internal command to Measurements
  procedure to indicate the deleted Measurement Identity
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

void rrcpcrc_dch_compressed_mode_handler
(
  l1_compressed_mode_ind_type *cm_ptr
)
{
  rrc_UL_DCCH_Message *ptr = NULL; /* Pointer to uplink DCCH message type */
  rlc_lc_id_type ul_lc_id;         /* Logical Channel Id for the message  */ 
  rrcsend_status_e_type status;    /* Status from send chain module       */
  
  /* To post a command RRC_DELETE_MEAS_REQ to measurement procedure */
  rrc_cmd_type* rrcmeas_cmd_ptr = NULL;
  
  
  /* Send a Delete measurement Req to L1 */
    rrcmeas_cmd_ptr = rrc_get_int_cmd_buf();
    rrcmeas_cmd_ptr->cmd_hdr.cmd_id = RRC_DELETE_MEAS_REQ;
    
    /* Set meas identity */
    rrcmeas_cmd_ptr->cmd.cm_ind.meas_id = 
      cm_ptr->meas_id;
    
    rrc_put_int_cmd(rrcmeas_cmd_ptr);
  
  /* Now send a Physical Channel Reconfig Failure Msg */ 
  
  /* Allocate memory for this message. The buffer is released
   * by the Send Chain */

  ptr = (rrc_UL_DCCH_Message *)rrc_malloc( 
    sizeof(rrc_UL_DCCH_Message));
  
  
  RRC_RESET_MSG_IE_PRESENT_PTR(ptr); /* Integrity Check info not present */
  /* Set t to Physical Channel Reconfig Failure Msg */
  ptr->message.t = T_rrc_UL_DCCH_MessageType_physicalChannelReconfigurationFailure;

  /* Unlike other RBControl failure messages the transactionId in PhChFailure
     is optional and it is not present if the failure is due to runtime error
     due to overlapping compressed mode configuration */
  RRC_RESET_MSG_IE_PRESENT(ptr->message.u.physicalChannelReconfigurationFailure);

  /* Set Error Cause to Compressed Mode Run Time Error */
  ptr->message.u.physicalChannelReconfigurationFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_compressedModeRuntimeError;
  
  /* Set TGPSI that has been deleted */
  ptr->message.u.physicalChannelReconfigurationFailure.
    failureCause.u.compressedModeRuntimeError = (uint8)cm_ptr->tgpsi;
  
  
  /* Get the RLC logical channel id for this msg from Logical Channel Manager */
  ul_lc_id = rrclcm_check_ul_rlc_lc_id ( UE_LOGCHAN_DCCH,
    DCCH_AM_RADIO_BEARER_ID,
    UE_MODE_ACKNOWLEDGED_DATA);
  
  /* If the logical channel id is not invalid, send the message to the
   * send chain module */
  if(ul_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    WRRC_MSG0_HIGH("Sending PCRC Failure message to send-chain");

    /* Ack for this procedure is not required */
    status = rrcsend_send_ul_sdu(RRC_PROCEDURE_PCR, (void *)ptr, 
      ul_lc_id, RRCSEND_L2ACK_NOT_REQUIRED);

    if( status != RRCSEND_SUCCESS )
    {
      WRRC_MSG1_ERROR("Send chain failed for PCRC failure msg: error cause %d", status);
    }
  }
  else
  {
    /* Unexpected failure */
    WRRC_MSG1_ERROR("Can't send UL DCCH msg! invalid lc id: %d", ul_lc_id );

    /* It's important to free the memory since we didn't send the msg */
    rrc_free( ptr );
    
  }
  return;  
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

uint8 rrcpcr_get_srns_relocation(void)
{
  return pcrc.pcrc_for_srns_relocation;
}
#endif 
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
/*===========================================================================

FUNCTION rrcpcrc_set_failure_cause

DESCRIPTION

DEPENDENCIES


RETURN VALUE
  Returns 

SIDE EFFECTS

  None

===========================================================================*/
void rrcpcrc_set_failure_cause(rrcrb_msg_status_e_type cause)
{
  pcrc.status.failure_status = (rrcpc_msg_status_e_type)cause;
}

#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/


