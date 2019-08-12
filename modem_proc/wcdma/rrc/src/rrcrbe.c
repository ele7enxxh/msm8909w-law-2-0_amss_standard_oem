/*===========================================================================
  R R C   R A D I O   B E A R E R   E S T A B L I S H M E N T   M O D U L E 

DESCRIPTION

  This module performs the Radio Bearer Establishment Procedure. This
  module processes the Radio Bearer Setup Message and sets up the 
  radio bearers given by the message. 
  
  The Radio Bearer Setup Message may setup both signalling radio bearers
  and radio bearers that are mapped to radio access bearers. 
  It may set also re-configure previously established radio bearers.
  The procedure may also initiate a hard-handover if the Radio Bearer
  Setup Message indicates a frequency change or a frame offset change.
  
  
EXTERNALIZED FUNCTIONS

  rrcrbe_procedure_event_handler   Event handler for this procedure
  rrcrbe_init_procedure            Power-up initialization of the procedure

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Procedure must be initialized at power-up by calling rrcrbe_init_procedure. 
  
Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcrbe.c_v   1.32   10 Jun 2002 10:31:16   upuntamb  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcrbe.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/16/16   sp      Made changes to index rbe global correctly when RB setup gives rab reconfig list and rab setup list
12/03/15   nr      Made changes to fix KW errors
10/16/15   as      Added code to support LTA durting activation time
06/01/15   sp      Made changes to avoid configuring Data RB to lower layers for when OOS is received during fach to fach redirection cases
04/23/15   vi      Made changes to add DMCR IE only when DMCR is supported in SIB3
04/14/15   sp      Made changes to indicate Multi rab status to L1
09/03/14   sp      Made changes to save procedure substates in crash debug info
08/08/14   ad      Made changes to trigger reconfiguration failure after cell update
                   procedure gets finished due to SIB5 change
08/04/14   sp      Made changes to fix compilation errors
07/18/14   ad      Made changes to reduce F3s
06/19/14   sg      Made changes to revert the cipher config db if cipher config type is
                   updated to step1 but later message validation failed.
06/20/14   db      Mainlined FEATURE_WCDMA_HS_RACH_DELAY_CU and introduced NV control
06/17/14   sp      Made changes to mainline RLC-restablihment redesign changes
06/02/14   sp      Made changes for FR19997 - UL rlc re-establishment redesign
05/30/14   sa       Made changes to support 64x64 single PS RAB detection
05/06/14   geg     Use safer versions of memcpy() and memmove()
04/10/14   db      Made changes to detect UL size change during FACH to FACH redirection
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/13/14   sp      Made changes to include as_id in RRC_ESTABLISH_IND and RRC_SYNC_IND
03/14/14   sp      Made changes to init re-establish entity on Procedure cleanup
01/16/14   db      Made changes to clear CRNTI when UE camps back on directed cell when dch to fach timer expires
01/09/14   sp      Removed code under FEATURE_CS_VOICE_OVER_HSPA feature
12/24/13   sp      Made changes to reset reestab entity at procedure cleanup
12/13/13   db      Made changes to delay Cell Update when HSRACH Capability changes until Uplink Data Arrives 
12/09/13   sp      Made changes for OSYS 6.6.5 migration
11/29/13   sp      Made changes to calculate the count c activation time for a fach 
                   to dch after moving to DCH state
10/23/13   gv      Made changes to trigger FR2LTE only after a CS RAB gets established and then released
10/18/13   ad      Revert changes to init the reestablish entity during RL failure 
                   and also during clearing procedure
10/10/13   ad      Made changes to init the reestablish entity during RL failure 
                   and also during clearing procedure
09/29/13   db      Made changes to update failure status of procedure upon registration with CU and OC reset
09/16/13   mp      Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
08/27/13   sp      Made changes to remove voice rab not found print
08/14/13   vg      Made changes to implement a new diag event EVENT_WCDMA_RRC_FAILURE
07/19/13   db      Made changes to Swap OC and CC after channel config by CU when
                   a procedure is registered and it fails
06/27/13   gv      Made changes to buffer the MM and RR commands for crash debugging
06/27/13   sa      Added changes to support call type mask for AMR with PS0x0 config
04/25/13   sg      Added code for REL9 under FEATURE_WCDMA_REL9
03/14/13   md      Made changes to provide active subscription ID to NAS APIs for DSDA
01/23/13   mp      Fixing KW errors
01/03/13   ad      Made changes to reduce the redundant F3s
11/22/12   sa      Made changes to update the call type mask for CS data 
10/31/12   ad      Made changes not to send channel_config from procedure based on 
                   TOC usage flag toc_usage_change_oos_to_dch  
10/26/12   db      EFACH Redesign changes
10/19/12   sa      Made the changes to update the call type mask for cs voice and cs data
10/18/12   gv      Replace FEATURE_DUAL_SIM_WCDMA_TUNEAWAY with FEATURE_DUAL_SIM
10/04/12   md      Made changes to detect RLC size change during state transitions via CELL PCH state.
10/04/12   pm      Removed unnecessary comments
10/04/12   pm      Made sure SRB's are present in Chan_config_req during FACH to DCH transition
10/03/12   pm      Made the changes to reset the TOC usage flag on waiting for the cu confirm which sets during OOS.
09/26/12   pm      Made changes to reset the ordered codec when procedure is cleared
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/29/12   ad      Made changes for triggering CU if E-RNTI is not given.
                   Skipped the de-registration of WMs if HS-RACH is ON and RRC is 
                   triggering MR while coming out of PCH. Fixed bug to updated E-RNTI
                   in OC correctly. Added code to trigger CU if R99 or R5 CUCNF is received. 
                   Added code to keep L1 and MAC in sync wrt HS-RACH actions.
06/12/12   sa      Made the changes to keep the active codec info.
04/06/12   ad      Changes for HS-RACH feature
04/25/12   as      Made changes to trigger cell update/drx req when UE recived OTA with 
                   redirection info and cell reselection in progress
04/30/12   db      Added check to swap CC and OC only on DCH to FACH fail
04/12/12   as      Added code to support feature NB/WB handover support
04/04/12   as      Made changes to initiate transition to idle when L2 ACK fails while going to PCH
12/09/11   ad      Added code to log WCDMA_RRC_PROTOCOL_ERRORS packet
12/08/11   db      Made changes to trigger cell update if H-RNTI is missing 
11/20/11   db      Added changes to wait for l2 ack in all states, not just DCH
                   Include reconfig status indicator in CU
10/13/11   db      Made changes to restore/clear ordered codec when RB failure occurs
08/22/11   sks     Fixed compilation error.
08/18/11   ad      Made changes to block rab establish indication if rabs have 
                   already been released under WRLF feature
06/17/11   gv      Removed changes to abort a suspended BPLMN search after receiving RB 
                   setup OTA under the feature FEATURE_WCDMA_CONTINUE_MPLMN
05/03/11   rl      Fixed the lint error
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/02/11   ad      Providing an API to L1 to check for AMR config
02/11/11   rl       Fixed the compiler warning
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
11/02/10   su      Fixed Klocwork Warnings.
10/20/10   su      Fixed Lint warnings.
10/11/10   prk     Removed the code which do double freeing of the memory.
10/04/10   prk     Made chagnes to use correct variable to determine next RRC state.
                   without this change frequency redirection with RB setup fails.
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
09/13/10   as      Added code to support FEATURE_DUAL_SIM
07/14/10   su      Fixed Lint errors.
07/02/10   sks     Added code to support FEATURE_WRLF_SYSTEM_SEL for connected mode OOS.
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
06/10/10   prk     Made changes to wait for L2 ACK after sending RB setup 
                   failure message if the failure is due to the lower layer configuration.
06/07/10   ss      Added code to reset SVTHHO in case of failure message
05/07/10   prk     Fixed compiler warnings.
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
04/23/10   as      Fixed Klocwork issues
04/15/10   gkg     Fixed KW Errors in R8 Code.
03/12/10   dm      Added code for supporting Deferred SIB reading feature.
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
02/11/10   gkg     Made changes for Opt Timer for CSoHS.
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
01/22/10   ss      Made changes in Start-Value calculation for CU in DCH to FACH with RAB est
12/22/09   gkg     Made changes for CSoHS so that NV 6878 controls CSoHS operation.
12/22/09   rmsd    Fixed lint warnings.
12/21/09   gkg     Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
12/14/09   kp      Made changes send RAB_EST_IND only if PS doamin is open.
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
10/30/09   as      Fixed klockwork warnings
10/28/09   gkg     Fixed KW Errors.
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
10/01/09   gkg     Made changes so that SCUDIF is handled based on derived
                   configuration instead of relying on MM.
09/25/09   gkg/ps  Made changes to not intiate cell selection for FACH->FACH or
                   FACH->PCH redirection, if cell reselction is ongoing.
09/11/09   ps      Made changes for reducing the internal RRC command size 
                   under FEATURE_RRC_REDUCE_CMD_SIZE
08/17/09   ss      Added code to provide ciphering ON/OFF indication to UI 
                   under feature flag FEATURE_UMTS_VOICE_CIPHERING_IND.
06/10/09   gkg     Made changes to send RB Setup Failure, if the OTA doesn't have
                   any RB related IEs. Made changes for all the releases.
06/05/09   ps      Made changes to not start the T305 timer if value is infinity
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
04/09/09   ps      Made changes to send reconfig ind in CU/or go to idle
                   if RL failure is triggered while l2 ack for reconfig complete
03/28/09   rm      CMI Featurisation
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
01/15/09   gkg     Made changes to initialize rlc_size_change_in_progress DB while 
                   handling Channel Config Confirm from LLC.
12/26/08   sks     Added code to abort any pending BPLMN search on reception of 
                   Radio Bearer Setup Message.
12/17/08   gkg     When RLC RESET or RL Failure happens during DCH -> FACH transition, 
                   made changes so that procedure triggers Cell Update after receiving 
                   successful channel config cnf.
12/08/08   ps      Made changes to call mvssup_set_voc_timing() for Inter-Freq
                   HHO/Timing Initialised HHO and Forced SYNCA
12/04/08   gkg     Made changes to mute compiler warning when feature FEATURE_VOC_AMR_WB 
                   isn't defined.
11/28/08   gkg     Made Changes for new MVS-RRC Interface to (de)configure vocoder
                   for AMR Voice Calls.
11/06/08   gkg     Added code so that the content of the RAB-Info to Setup is accessed
                   only if RAB-Info to Setup IE is present in RB-Setup OTA.
11/04/08   rm      Added support for rove-in to GAN in RRC Connected mode.
                   Code changes are under FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE.
24/10/08   gkg     If RAB Info to setup IE isn't given in RBE OTA, instead of returning 
                   immediately go ahead for validating and processing other RB related IEs.
10/21/08   ps      Update the action as RECONFIG if the SRB is already 
                   established
10/01/08   gkg     Fixed the Klockwork errors.
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
09/15/08   ns      Made changes to calculate the activation time using the CFN of the
                   new cell, and thereby ensure that the activation time sent is at least 200
                   frames ahead of CFN. 
06/17/08   gkg     Fixed typo while setting the DL cfg mask for RLC entity of RBs
                   under RB-Reconfig IE of OTA.
06/13/08   gkg     Added code for notification of Cell_Id change through OTAs
                   in Cell_DCH state. 
06/13/08   gkg     Made changes so that if RLC PDU size changes for RBs through
                   Transport Channel IEs of RB-Setup OTA, those RBs are reconfigured
                   and therefore their RLC entity is notified for new RLC PDU size.
06/13/08   gkg     To handle the corner scenario of SCUDIF call through RB-Setup,
                   made changes in rrcrbe_process_channel_config_cnf() so that 
                   global rrc_est_rabs is updated before it's used to determine
                   the no. of RBs in the RAB and AMR Class C logical id.
05/30/08   gkg     In case of Cell_FACH->Cell_DCH state transition, stop T305
                   if RRC_CHANNEL_CONFIG_REQ succeeds otherwise handle timer T305
                   by initiating Cell Update.
04/15/08   kp      Fixed lint errors.
04/14/08   gkg     Made changes for configuration of AMR Call so that no. of AMR
                   classes in UL to be registered with MVS is 3 whenever no. of RBs in
                   voice RAB is 3. This is done irrespective of amr Rate.
04/11/08   ps      Fixed Compiler warning
01/04/08   gkg     Added support for asymmetric AMR call by updating DL rlc ids, 
                   tr-ch ids and cctrch ids separate from UL.
19/03/08   gkg     Made Changes so that MVS is configured for AMR call in DL using DL
                   CTFCs instead of relying on UL CTFCs.
03/06/08   da      Added MBMS phase 2C changes.  MBMS support in CELL_DCH.  
                   Includes mobility and dynamic resource management.
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
02/11/08   gkg     Made changes so that whenever there is a mismatch in what is
                   expected in an OTA msg and what is received, always INVALID 
                   Config is returned instead of UNSUPPORTED Config.
01/30/08   gkg     Made changes to send Start value in the Complete msg,
                   only when the Re-establish is caused by RBE 
01/08/08   ps/gkg  Made changes to query the RAb type from Mn even for 
                   Handover To utran msg.
12/21/07   da      Support for FEATURE_MBMS
11/28/07   da      In case OOS area gets detected during DCH->FACH transitions,
                   made change to setup DCCH+DTCH once returning in service before
                   sending CU message with cause "re-entered service area"
11/28/07   rm      Added support for handling multiple RLC operations on a
                   single RB under FEATURE MULTIPLE RLC OPERATIONS
26/11/07   gkg     Added support for VT->Voice fallback by RB-Setup msg 
                   for SCUDIF under feature flag FEATURE_SCUDIF_ENHANCEMENT
10/30/07   ps       Added support for DC 14
10/23/07   ps      Added support for DC 20 through Rb-Setup msg
10/23/07   kp      Added support for network sharing feature. Code changes to handle 
                   primary PLMN I.D in OTA message. Changes are under feature flag 
                   FEATURE NETWORK SHARING.
10/08/07   da      Made change to pass correct proc id when registering with scm.
09/10/07   ps      Added support for Frequency redirection received in 
                   FACH->FACH and FACH->PCH cases under feature flag
                   FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
08/08/07   ps      Added support for feature cell id reading in DCH state, 
                   under feature flag FEATURE_WCDMA_GPS_CELL_ID_READING.
05/23/07   ps      Added support for "RB Info to Change" IE in RB-Setup msg.
                   Change are made under feature flag FEATURE_DEFAULT_CONFIG_IN_RB_SETUP.
04/24/06   ps      Made corrections for merge issues for WB-AMR under 
                   FEATURE_VOC_AMR_WB
04/10/07   ps      Added support for default configs sent through Rb-Setup msg
03/14/06   ps      Made correction to send failure cause as "Unsupported 
                   config" if an unknown codec type is received in OTA
03/02/06   ps      Added support for Wb-AMR under the feature flag
                   FEATURE_VOC_AMR_WB.
02/07/07   ps      Made changes to handle Standalone Rb-Mappings
02/05/07   da      Fix for NULL pointer derefences reported by Klocwork. 
02/02/07   da      Removed code that use to handle DCH->FACH activation time
                   and 300ms delay.  functionality has been moved to LLC. 
01/18/07   sm      Added code review comment changes for CR2284R1
01/15/07   sm      Fixed compilation error which got introduced with fix of Spec CR
                   CR2284R1 for FEATURE_WCDMA_REL6
01/12/07   sm/vr   Implementation of Spec CR 2284R1 and added support for HHO with
                   AMR rb-setup message 
12/22/06   ps      Made changes to handle postVerificationPeriod and the
                   timingMaintainedSynchInd IE received in OTA messages
11/15/06   sm      Added support for IE rb-reconfig-info and rab-reconfig
                   in release-6 path of rb-setup message
09/12/06   sm/da   Fix code review comments and general clean-up for REL6 and HSUPA
09/11/06   da      Added support for FEATURE RLC ONE SIDE RE ESTABLISHMENT
09/05/06   da      Added new func rrcrbe_get_new_crnti_valid().  This function
                   will be called when CU wants to know if the OTA msg
                   included a new CRNTI.  Also made change not to reset 
                   new_crnti_valid flag when proc is waiting for CU CNF and 
                   OOS has been detected.  This is because proc needs to send
                   a RNTI update req.
08/31/06   ss      Fixed lint errors
08/30/06   sm      Fixed REL6 compilation errors because of REL-6/EUL merge to mainline
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/28/06   da      Added support for OOS area handling during transition from DCH.
07/07/06   tkk     Added code to update MVS when PS RAB is established 
                   while reconfiguring the AMR rate during multi-RAB call.
07/05/06   da      fixed compilation error.
06/26/06   sgk     Corrected compilation error.
06/26/06   tkk     Added support to process third RB information given later 
                   through RB setup during a voice call.
06/13/06   segu    Added support for DSAC future implementation
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE REL5 while retaining the HSDPA specifc processing 
                   under FEATURE HSDPA.
06/25/06   vr      Assign correct default ID for SRB4 if the Id is omitted in the
                   RBSetup message
05/05/06   da      Added check to reject RB setup msg if NW tries to setup an
                   RB with an RB_ID of that an already established RB.
04/25/06   da      Added checking the status of RNTI_UPDATE_CNF.  If failed, 
                   send failure to RBS msg and clear procedure.
                   Init mac_rab_status in rrcrbe_procedure_event_handler
04/18/06   da      Modified code to support migration to Mar-06 Rel-5 ASN1 +
                   R6 Hacks
03/15/06   tkk     Lint warnings/errors cleanup
03/10/06   da      Added Initial support for PDCP under FEATURE_UMTS_PDCP
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1
02/15/06   sgk     Added code to process channel config cnf with config
                   cancel and to handle the cases where cu is in pending
                   config state.  
12/22/05   tkk     Added support to handle CU (for certain causes) while 
                   reconfig procedure is waiting for L2 ack to go to 
                   Cell_PCH/URA_PCH from Cell_FACH.
12/19/05   da      Calling rrc_send_rb_rate_info_packet() to log rb rate info.
12/01/05   da      If activation time for DCH-FACH transition is greater than 300 ms,
                   make delay before cell selection as long as activation time.
11/10/05   da      Removed extern definition of nv_rel_ind;
11/10/05   da      Checking global flag rrc_nv_rel_indicator before doing R5 related
                   processing.
08/11/05   vk      Removed the check to set failure_status if for a CS Data
                   loopback Call, fn rrcmn_verify_rab() returns invalid RB ID
                   for Release 5
08/05/05   vk      Use rab list pointer when translating RAB ID for Release 5
07/29/05   da      Made change to update var mac_rab_status
07/25/05   ss      Added explicit registration for cell update cnf on l2 nack flr.
06/15/05   da      Sending rb failure msg with cause "cell update occured" when OOS area
                   occurs while RB proc is waiting for l2 ack.
                   Also passing indication to func rrcsend_send_ul_sdu to not send L2 ack to
                   procedure when procedure is not waiting for l2 ack.
                   Added fix for GCF where UE was sending failure message [due to later_than_r3
                   msg chosen] with wrong transaction ID.
06/06/05   da      Calling func rrcllc_print_trans_chnl_rate() after sending complete
                   msg to print trans chan rates.
05/16/05   da      Added support for validation failure handling from L1
04/14/05   da      Added support for Phase 2 of dch failure handling: going back
                   to old configuration.
03/25/05   da      Removed FEATURE CM SUPPORTED from rrcrbe_process_channel_config_cnf.
                   Had an invalid check that looks if failure was due to compressed mode and send
                   rb setup failure msg with cause invalid config.
03/11/05   vk      Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE HSDPA
03/14/05   sk      Fixed the problem with not restoring rab_info_ptr and using it
                   in further validations.
03/11/05   sk      Added a check to verify whether all RABs in the rabinfo list 
                   belong to same CN domain id or not otherwise this is an invalid configuration
03/02/05	ttl			 Support RRC ASN1 Release 5.
02/18/05   kc      Fix to clear up TM-Ciphering-Config while going to cell-fach due to
                   RB-Setup Channel-EST-Failure
01/04/05   da      Added Support for phase 1 of Physical channel establishment 
                   failure handling within DCH. 
12/21/04   da      Fixed Lint errors
11/30/04   vm/da   Added appropriate MSG_HIGHs for new RRC/NAS interface 
                   RRC_SERVICE_IND_FROM_OTA_MSG.
11/23/04   sm      Added support for Dynamic Power Class and fixed some scenarios of UL
                   TPC power limiting
11/22/04   da      Added support for phy chan setup fail and phy chan est fail in 
                   CELL_FACH->DCH from a cell update cnf msg and rb setup/reconfig.
10/21/04   da      Removed FEATURE_CELLPCH_URAPCH.  CELLPCH and URAPCH states should always
                   be handled.  Deleted code that handles case when CELLPCH and URAPCH is not
                   defined.
10/19/04   da      Removed FEATURE SRNS RELOCATION.  SRNS_RELOCATION should always be 
                   handled.  Deleted code that handles case when SRNS_RELOCATION is not
                   defined.
09/30/04   sk      Changed the faiure cause from unsupported config to  invalid_
                   configuration when rab validation fails.
09/14/04   sk      Added support to save CN info in mm_cmd_buf and later send the
                   command to MM when procedure is succefully completed 
                   Set the failure cause to "protocol error" with 
                   "rrc_messageExtensionNotComprehended" when msg choice is later_than_r3
08/30/04   sgk     Set rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
                   instead of RRCRBE_WAIT_FOR_CELL_SELECTION_CNF in function
                   rrcrbe_send_channel_config_req to fix lint error: Type 
                   mismatch (assignment) (enum/enum). 
                   Typecast ciphering_activation_cfn to uint8 in function
                   process_rbsetup_message to fixl lint error Loss of precision
                   (assignment) (11 bits to 8 bits).
                   Initialized rab_status to RRC_RAB_NOT_FOUND in function 
                   process_rbsetup_message to fix lint error Symbol 'rab_status'
                   conceivably not initialized.
                   Removed the parameter rrc_rnti_update_cnf_type *ptr from
                   function rrcrbe_process_rnti_update_cnf and corrected it at 
                   the calling location to fix lint error: Symbol 'ptr' not
                   referenced.
                   Commented out local structure 'rrcrbe_substate_e_type' to fix 
                   multiple lint errors 'rrcrbe_substate_e_type' and enum constants
                   'RRCRBE_INITIAL', 'RRCRBE_WAIT_FOR_CELL_UPDATE_CNF', 
                   'RRCRBE_WAIT_FOR_CHAN_CFG_CNF', 'RRCRBE_WAIT_FOR_CRNTI_UPDATE_CNF',
                   'RRCRBE_WAIT_FOR_L2_ACK', 'RRCRBE_MAX_SUBSTATES' not referenced.
                   Removed return after ERR_FATAL in functions
                   send_rbs_failure_message, send_rbs_complete_message,
                   rrcrbe_send_indication_to_upper_layers, to fix lint error:
                   'unreachable'.
                   Initialize the variable rab_type to RRCMN_INVALID_RAB_ID in
                   function validate_rbsetup_message to fixl int error Variable
                   'rab_type' may not have been initialized.
                   Replaced RRCRBE_WAIT_FOR_REESTABLISH_SRB2 by 
                   RRCRB_WAIT_FOR_REESTABLISH_SRB2 in switch in
                   rrcrbe_cell_dch_event_handler to fix lit error Type mismatch
                   with switch expression.  
08/20/04   jh      Updated to use new APIs for enhanced AMR processing.
08/05/04   bu      Filled the proc id in rrcrbe_send_initiate_cell_selection_req().
07/14/04   sk      RL failure/RLC reset handling
                   Removed the local substate enum type. modified the code to use
                   the rrbrb_substate_e_type declared in rrcrbcommon.h
                   Added new API to get current substate.
07/12/04   sk      Added debug prints for rab query and processing chan_config_cnf
06/04/04   svk     Call L1function to disable TFC restriction for UE Tx Power when
                   a voice call is added.feature #ifdef FEATURE WCDMA PWR TFC ELMN
05/18/04   sk      Added a check for the state indicated by the message in 
                   state handling in process_channel_config_cnf()
05/12/04   jh      Fixed typo in #ifdef
05/11/04   jh      Conditionally enabled new MVS interface for multi-rate AMR.
05/04/04   sk      Added Freq/Scr handling in reconfiguration for CELL_FACH->CELL_FACH
                   transition.
                   Changed rrcrb_send_rnti_update_req() input parms.
04/20/04   sk      Populate re-est timer association for the rab-id before adding it
                   to established_rab variable.
04/06/04   ttl     Handle multiple RABs setup in a single RBSetupMessage.
03/24/04   sk      Added the support to handle the rb setup message with only SRB info list
02/03/04   sk      clear the ordered_config variable immediately after sending response
                   message, when SRNS relocation is not involved and next state is 
                   not cell_PCH or URA_PCH. 
                   Handle l2ack in  substates otherthan waiting_for_l2_ack substate.
                   Fixed few lint errors.
12/10/03   bu      Removed the calls to clear_ordered_config(). It will be called
                   only when L2 Ack is received. Also removed handling of NEW_CELL_IND
                   as it will never be received.
11/18/03   sk      Send the indication to upper layers when rbsetup complete
                   message is submitted to lower layers.
                   Go to idle if L2ack for rbsetup complete message fails and 
                   the procedure involves SRNS relocation. Else do not take
                   any action on L2ack failure.
11/17/03   kc      Added SRNS support in RB-Setup Procedure.
10/08/03   sk      Added processing of SRB info to setup list
08/11/03   vn      Initialize new fields for RLC re-establishment in RNTI_UPDATE_REQ.
07/29/03   sk      Added a check for valid rlc size for class-A bits corresponding
                   to 12.2K rate. This will take care of sending failure if network
                   tries to configure lower AMR rates.
07/08/03   sk      Send failure message if rab already exists in established rabs.
06/27/03   sk      Round the ciphering_activation_cfn value to make it multiples
                   8 frames.
06/25/03   sk      Clear the procedure when Msg validation fails.
                   Added more debug info. 
                   Removed lint errors.
05/28/03   sk      Featurised MVS changes
05/21/03   sk      Added missing edit history.
                   Merged the MVS AMR related changes.
04/03/03   sk      Added the transaction Id checking before sending 
                   RBSetup complete/failure messages. 
03/18/03   sk      Modified the transaction Id checking in process_rbsetup_message()
02/13/03   vn      Added functionality to process Reconfig message going from
                   CELL_DCH to CELL_FACH, that comes with a new C-RNTI. If
                   camped on the same cell as directed and if C-RNTI is
                   available, do not initiate Cell Update procedure. 
02/05/03   vn      Added processing for State Change to Idle when waiting
                   for Cell Update Confirm or Cell Selection Confirm.
02/18/03   sk      Changed rrc_start_Value_present
                   to  rrc_RadioBearerSetupComplete_start_Value_present
02/05/03   sk      March2002Rel specific changes 
                   to  rrc_RadioBearerSetup_r3_IEs_rab_InformationSetupList_present
                   Changed  rrc_rab_InformationSetupList_present
                   to  rrc_RadioBearerSetup_r3_IEs_rab_InformationSetupList_present
02/05/03   vn      Merged changes dropped during linting.
           vn      Added new parameter proc_id in rrcllc_set_ordered_config.
02/04/03   ram     Checked in lint changes, updated copyright.
11/22/02   vn      Wherever action is taken based on the next_state in
                   Channel Config Request, also check for 
                   rrc_state_change_required flag. 
10/31/02   vn      Added an extra delay before starting cell selection when
                   going DCH->FACH. This is to ensure that L2 Ack for RB
                   Setup Message on DCH is received by RNC.
10/24/02   rj      Updated to use rrc_malloc and rrc_free to use dual heaps 
                   in place of mem_malloc and mem_free to avoid memory
                   fragmentation
10/01/02   vn      Merged folowing from DCH to FACH transition branch.
           vn      Fixed so that local rrc_cmd buffer is not enqueued.
                   Changed the order to send Channel Config first before 
                   initiating Cell Update on a DCH to FACH transition.
           upn     cell_DCH to cell_FACH transition implementation
09/28/02   kc      Added more changes to support PS Data ciphering.
09/27/02   kc      Added add. ciphering code to extract activation time from
                   RB setup message or calculate ciph. activation sn from current
                   CFN queried from L1.
09/25/02   xfg     Changes for WVS to MVS migration. The changes were made under
                   FEATURE_MVS.
09/25/02   bu      Changed l1_status to chan_cfg_status under
                   FEATURE CM SUPPORTED.
09/20/02   bu      If reselection is in progress or if cell update has started
                   and register for CU CNF accordingly. On receiving cell update
                   confirm send RB Setup failure to UTRAN. If C_RNTI is not valid
                   register for CU CNF. On receiving CU CNF send the RB Setup
                   Complete. Call rcllc_get_ordered_config_status() instead of
                   rrcllc_is_ordered_config_set().
09/04/02   kc      Added interface fn. to get established rabs during RB 
                   establishment
08/13/02   upn     Fixed array counter in rrcrbe_send_channel_config_req
07/31/02   vk      Updated the format of previous comments
07/31/02   vk      Processed Compressed Mode failure from LLC and send a 
                   Radio Bearer Setup Failure message to UTRAN with error
                   as Invalid Configuration. Changes have been put under
                   FEATURE CM UPPORTED
06/10/02   upn     Removed check for RB Info Affected List.
06/06/02   upn     removed definitions for reconfig complete messages to be sent on
                   uplink DCCH.
05/13/02   upn     enabled rrcrbe_cell_fach_event_handler.
03/13/02   bu      Uncommented the code related to CELL_FACH in
                   rrcrbe_process_channel_config_cnf and process_rbsetup_message.
03/08/02   bu      Added definitions for functions rrcpcr_build_pcr_complete_message,
                   rrctcr_build_tcr_complete_message and
                   rrcrbrc_build_rbrc_complete_message.
01/21/02   upn     Code implemented for state CELL_FACH support.
02/08/02   xfg     Don't Query CMN to verify Rab for LB for a CS service
01/30/02   xfg     Don't query RABM to verify rab for loopback test in PS domain
12/07/01   bu      Added MSG_HIGH to report that we are waiting for L2 ACK.
12/04/01   rj      Fixed a compilation problem.
12/04/01   rj      Updated code to be compatible for June release of 25.331 
                   RRC Protocol Specification.
                   Corrected comments for L2 ACK failure.
11/12/01   upn     Added support to send CN info to NAS.
11/06/01   upn     Set the message_set field before sending the RABMAS_RAB_ESTABLISH_IND
                   to RABM task. Value assigned is MS_RABM_RRC.
11/05/01   upn     sync indication functionality moved so as to be sent from RRC to MM
                   after the L2 ack from the RB Setup Message has been received.
10/19/01   upn     Radio Link Failure support: Updated rrcrbe_process_l2_ack_ind
                   function.Removed rrc_clear_established_rabs procedure.
10/02/01   ram     Fixed a problem with PS RAB establishment where the right RAB
                   type was not being stored in the internal variable rbe.
09/13/01   ram     When a PS RAB is established, a RABMAS_RAB_ESTABLISH_IND primitive
                   should be sent to the RABM layer. Added code to do that.
                   Freed memory of uplink messages when there is a failure to
                   send the message.
09/07/01   ram     Added support for setting up Packet Switched RABs.
09/05/01   ram     Updated with code review comments. Implemented changes required
                   for handling ASN.1 decoding failure when message type is known.
09/03/01   rj      Removed include file rrcasnutil.h since it is no longer 
                   required after moving send_rrc_status_message to SEND CHAIN.
08/30/01   rj      Changed UE_MODE_ACKNOWLEDGED_DATA_PLANE to 
                   UE_MODE_ACKNOWLEDGED_DATA.
08/13/01   ram     Added support for CS data calls. Changed some MSG_HIGHs to
                   ERRs since they were failure conditions.
07/31/01   ram     Removed initialization of the bit-mask that indicates if
                   integrity check info is present. This is initialized by
                   the send-chain. Merged in code that delays 5 seconds before
                   transmitting on a new configuration. This is conditional
                   and only needed if the base station cannot have both the
                   old and new configurations up at the same time.
07/26/01   ram     Added MSG_HIGHs for significant events. Added checking of
                   RAB type validity which was commented out earlier.
06/19/01   ram     Commented out the checking of RAB type validity. The MN
                   interface to check RAB type is not working as designed so
                   this has been taken out for now.
06/08/01   rj      Modifed function rrcrbe_process_l2_ack_ind to compare 
                   L2 Ack status with SUCCESS enum in stead of RRC_L2ACK_SUCCESS.
                   This was required since interfaces with RLC are modified.
05/17/01   ram     Added interface to provide AMR info and RLC ids to
                   Voice Services. Added registration to get state change
                   notification. Fixed compiler warnings. Added more
                   comments.
05/08/01   ram     Changed transaction-id implementation according to our
                   current understanding. Made various updates to message
                   processing and validation. Support for all expected
                   events in all substates added. Now checking if 
                   ordered_config is set before validating the message.
05/02/01   ram     Added functionality to send RB Setup Failure message for
                   various different failure reasons. Added functionality
                   to send MM_SYNC_IND to MM layer when RABs have been established
                   succesfully. Added interfaces to voice services. Added code
                   to store all data related to RABs temporarily before 
                   ESTABLSIHED_RABS can be updated. Cleaned up code.
04/06/01   ram     Created file.

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
#include "rabmtask.h"
#include "rex.h"
#include "rrcasn1.h"

#include "rrccmd_v.h"
#include "rrcccm.h"
#include "rrcdata_v.h"
#include "rrccu.h"
#include "rrccui.h"
#include "rrcintif.h"
#include "rrclcm.h"
#include "rrcllc.h"
#include "rrcmmif.h"
#include "rrcmnif.h"
#include "rrcrbcommon.h"
#include "rrcrbe.h"
#include "rrcscmgr.h"
#include "rrcsend.h"
#include "rrclbt.h"
#include "rrccspi.h"
#include "rrccsp.h"
#include "rrcsibproc.h"
#include "rrcsmc.h"
#include "rrclog.h"
#include "rrcasn1util.h"

#include "seq.h"
#include "rrcsmc.h"

#include "rrcllcoc.h"
#include "rrctmr.h"

#include "rrcmisc.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif

#include "rrcdormancy.h"
#include "rrclogging.h"
#include "rrcrce.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* AMR 12.2 kbps speech */
#define AMR_MODE_122_CLASS_A_BIT_SIZE  81
#define AMR_MODE_122_CLASS_B_BIT_SIZE 103
#define AMR_MODE_122_CLASS_C_BIT_SIZE  60

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* We need to introduce a delay before transmitting
on a new configuration if the base-station cannot have
the old link and new link up at the same time. */
#define RRCRBE_TEMP_TIMER_SIG 0x4000
rex_sigs_type rrcrbe_sig;
rex_timer_type rrcrbe_temp_timer;
  

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* Structure to store all internal data related to RBE procedure           */
typedef struct
{
  rrcrb_substate_e_type curr_substate;  /* Store the current substate     */
  rrcrb_transaction_id_type tr_ids;      /* Transaction ids for RBS msg    */
  rrc_state_e_type state_from_msg;       /* RRC State given by RBS msg     */
  rrcrb_msg_failure_cause_type status;   /* Status of message validation   */
  rrcrb_rab_info_per_msg_type  rabs;      /* Hold info about each rab in one
                                         RBS message till they're processed*/
  boolean                      new_crnti_valid;           /* Indicates if a valid C-RNTI was
                                                            recd in the message transitioning
                                                            from CELL_DCH to CELL_FACH       */
  uint16                       new_crnti;
  boolean                      directed_cell_info; /* Indicates if a valid C-RNTI was recd */
  rrc_int_u_rnti_type          new_urnti;
  boolean                      new_urnti_valid;          /* Indicates if a valid U-RNTI was recd */
  boolean                      rbe_for_srns_relocation;  /* Indicates SRNS reloc involved with
                                                            this msg                             */
  boolean                       cn_info_saved;           /*flag to indicate that CN info is saved */
  mm_cmd_type *                 mm_cmd_ptr;              /*pointer to hold mm_cmd with cn info */
  boolean                       hho_ciph_status;
  boolean                       trans_to_dch_ciph_config_needed;


  boolean first_tm_rb_setup;
}rrcrbe_int_data_type;

LOCAL rrcrbe_int_data_type rbe;

rrc_cmd_type rrc_cmd;

boolean rrcrbe_send_initiate_cu_flag = FALSE;
boolean rrcrbe_delete_crnti          = FALSE;
uint8 ciphering_activation_cfn;

mac_rab_status_e_type mac_rab_status;
#ifdef FEATURE_UMTS_PDCP
extern boolean rrc_pdcp_enabled;
#endif /* FEATURE_UMTS_PDCP*/

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
extern boolean rrc_cs_rab_established;
extern rrcrce_csfb_status_e_type csfb_extended_status;
#endif

#ifdef FEATURE_QSH_EVENT_METRIC
extern rrc_qsh_ul_failure_ota_param_s failure_ota;
extern rrc_qsh_metric_db_type rrc_qsh_metric_db[WRRC_QSH_METRIC_MAX+1];
extern boolean rrc_qsh_dl_committed;
#endif


/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                            Function Definitions
** ======================================================================= */
/*===========================================================================

FUNCTION   RRCRBE_INDICATE_LAYER1_IF_RB_SETUP_CONFIGURE_VOICE_RAB

DESCRIPTION

  If radio bearer setup message indicates UE to setup voice RAB then this
  function will indicate layer1 about the same by calling some feature specific
  call back function provided by layer1.
  If received rbsetup message is not for setting up voice call then this function
  will not do anything.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbe_indicate_layer1_if_rb_setup_configure_voice_rab(void)
{
  uint32 rab_index;

  /*From L1 point of view, CS over HSPA call is similar to PS Data Call, so no need to inform L1.*/
  for (rab_index=0; rab_index<rbe.rabs.num_rabs_in_msg; rab_index++)
  {
    if (RRCRB_CS_VOICE_RAB == rbe.rabs.rabs_in_msg[rab_index].rab_type)
    {
      rrc_indicate_layer1_next_config_is_for_voice();
    }
  }
}
/*===========================================================================

FUNCTION   RRCRBE_IS_SRNS_IN_PROGRESS()

DESCRIPTION
 This function returns if SRNS is in progress with current reconfig procedure



DEPENDENCIES

  None.

RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcrbe_is_srns_in_progress
(
  void
)
{
  return rbe.rbe_for_srns_relocation;
}
/*===========================================================================

FUNCTION   RRCRBE_GET_NEW_CRNTI_VALID()

DESCRIPTION
 This function returns if reconfig msg has valid CRNTI


        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcrbe_get_new_crnti_valid
(
  void
)
{
  return rbe.new_crnti_valid;
}

/*===========================================================================

FUNCTION   RRCRBE_GET_CURRENT_SUB_STATE()

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
rrcrb_substate_e_type rrcrbe_get_current_sub_state( )
{
  return rbe.curr_substate;
}
/*===========================================================================

FUNCTION   rrcrbe_get_dest_state()

DESCRIPTION
 This function is used to return destination state

DEPENDENCIES

  None.

RETURN VALUE

  rrc_state_e_type .

SIDE EFFECTS

  None.

===========================================================================*/
rrc_state_e_type rrcrbe_get_dest_state(void)
{
  return rbe.state_from_msg;
}
/*===========================================================================

FUNCTION   rrcrbe_is_proc_successful()

DESCRIPTION
 This function is used to return failure status
DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rrcrbe_is_proc_successful( )
{
  if(rbe.status.failure_status == RRCRB_MSG_VALID)
    return TRUE;
  return FALSE;
}

/*===========================================================================

FUNCTION   rrcrbe_update_failure_status()

DESCRIPTION
 Updates the failure status
DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
void rrcrbe_update_failure_status(rrcrb_msg_status_e_type status )
{
  rbe.status.failure_status = status;
  WRRC_MSG1_HIGH("Updated Failure status to %d",status);
  return;
}


/*===========================================================================

FUNCTION   SEND_RBS_FAILURE_MSG

DESCRIPTION

  This function sends the Radio Bearer Setup Failure message
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
static void send_rbs_failure_message
(
rrcrb_msg_status_e_type failure_cause,  /* Failure Cause - should be a subset
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
  ptr = (rrc_UL_DCCH_Message *)rrc_malloc(sizeof(rrc_UL_DCCH_Message));

    WRRC_MSG2_HIGH("rbe subsstate(%d) Send failure with %d cause:   [0-VALID|1-UNSUPPORTED_CONFIG|2-PHY_CHAN_FAILURE|3-SIMULTANEOUS_RECONFIG|4-PROTOCOL_ERROR|5-INVALID_CONFIG|6-INCOMPLETE_CONFIG|7-CELL_UPDATE_OCCURED]", 
   rbe.curr_substate, rbe.status.failure_status);

    /* we need to revert cipher config db, if cipher config db has been updated with step-1 HO. 
     This revert takes care of the scenario of OTA failure in the gap between OC is 
     set and cphy is sent. This cannot happen in a normal case. This is for extra protection */
  rrcsmc_revert_to_old_config_for_failure_message(FALSE);
  ptr->message.t = T_rrc_UL_DCCH_MessageType_radioBearerSetupFailure; /* RBS Failure msg */

  /* Set the bit-mask - neither rrc_potentiallySuccesfulBearerList_present
  nor rrc_RadioBearerReconfigurationFailure_nonCriticalExtensions_present
  are included */
  memset(&ptr->message.u.radioBearerSetupFailure.m,0,
          sizeof(ptr->message.u.radioBearerSetupFailure.m));

  /* Transaction Id - First check the rejected_transacion id. If
  it's not invalid, this message is being rejected because there
  is already another message accepted but not processed.
  If the rejected_transaction id is invalid, we are rejecting the
  accepted message, so use the accepted transaction id.

  NOTE: Don't change the order, it's important to check rejected
  transaction first. */
  if ( rbe.tr_ids.rejected_transaction != RRCRB_INVALID_TRANSACTION_ID )
  {
    ptr->message.u.radioBearerSetupFailure.rrc_TransactionIdentifier =
    rbe.tr_ids.rejected_transaction;
    /* We no longer need to keep this id. Make rejected_transaction id
    invalid again */
    rbe.tr_ids.rejected_transaction = RRCRB_INVALID_TRANSACTION_ID;
  }
  else if ( rbe.tr_ids.accepted_transaction != RRCRB_INVALID_TRANSACTION_ID )
  {
    ptr->message.u.radioBearerSetupFailure.rrc_TransactionIdentifier =
    rbe.tr_ids.accepted_transaction;
    /* We no longer need to keep this id. Make rejected_transaction id
    invalid again */
    rbe.tr_ids.accepted_transaction = RRCRB_INVALID_TRANSACTION_ID;
  }
  else
  {
    /* Don't know the transaction id for this message.( one reason
    for this could be that the message had a critical
    extenstion). Since it's unknown, we should set it to zero. */
    ptr->message.u.radioBearerSetupFailure.rrc_TransactionIdentifier = 0;
    WRRC_MSG0_HIGH("Invalid Trid for RBS Failure msg!");
  }
  if (ptr->message.u.radioBearerSetupFailure.rrc_TransactionIdentifier > 3)
  {
    WRRC_MSG1_HIGH("Invalid TrId:%d from RBS db, assuming 0",
               ptr->message.u.radioBearerSetupFailure.rrc_TransactionIdentifier);
    ptr->message.u.radioBearerSetupFailure.rrc_TransactionIdentifier = 0;
  }

  switch ( failure_cause )
  {
  case RRCRB_MSG_UNSUPPORTED_CONFIG:

    ptr->message.u.radioBearerSetupFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_configurationUnsupported;
    break;

  case RRCRB_PHY_CHAN_FAILURE:

    ptr->message.u.radioBearerSetupFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_physicalChannelFailure;
    break;

  case RRCRB_SIMULTANEOUS_RECONFIG:

    ptr->message.u.radioBearerSetupFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_incompatibleSimultaneousReconfiguration;
    break;

  case RRCRB_MSG_PROTOCOL_ERROR:

    ptr->message.u.radioBearerSetupFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_protocolError;

    /* When there is a Protocol Error, we also have to indicate
    the protocol error cause */
    ptr->message.u.radioBearerSetupFailure.failureCause.u.protocolError =
	  	rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation);
    
    ptr->message.u.radioBearerSetupFailure.failureCause.u.protocolError->diagnosticsType.t  =
      T_rrc_ProtocolErrorInformation_diagnosticsType_type1;

	  ptr->message.u.radioBearerSetupFailure.failureCause.u.protocolError->
      diagnosticsType.u.type1 = rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation_type1);
    
    /* Copy the given protocol error cause */
    ptr->message.u.radioBearerSetupFailure.failureCause.u.protocolError->
    diagnosticsType.u.type1->protocolErrorCause = prot_err_cause;
    break;

  case RRCRB_MSG_INVALID_CONFIG:

    ptr->message.u.radioBearerSetupFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_invalidConfiguration;
    break;

  case RRCRB_MSG_INCOMPLETE_CONFIG:

    ptr->message.u.radioBearerSetupFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_configurationIncomplete;
    break;

  case RRCRB_CELL_UPDATE_OCCURED:

    ptr->message.u.radioBearerSetupFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_cellUpdateOccurred;
    break;

  default:
    WRRC_MSG0_ERROR("Sending RBE Unknown Failure message");
    ptr->message.u.radioBearerSetupFailure.failureCause.t =
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
   /* PHY channel failure means already a DL OTA is logged */
   if(rrc_qsh_dl_committed == FALSE)
   {
     failure_ota.ul_ota_type = RRC_OTA_UL_FAILURE;
     failure_ota.msg_id = WRRC_UL_DCCH_MESSAGETYPE_RADIOBEARERSETUPFAILURE;
     if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status())
     {
       failure_ota.ul_fail_ota.ul_ota_parms.ul_integrity_info_present = 1;
     }
     failure_ota.ul_fail_ota.ul_ota_parms.failure_cause = ptr->message.u.radioBearerSetupFailure.failureCause.t;
	 /* Commit data should happen after dl SDU is committed */
   }
   else if(rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr != NULL)
     {
       if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status())
       {
     	((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.ul_ota_parms.ul_integrity_info_present= 1;
       }
       ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.ul_ota_parms.failure_cause= ptr->message.u.radioBearerSetupFailure.failureCause.t;   
       /* Commit data will be called in send ul sdu */
     }
#endif
  /*log the protocol error*/
  rrc_log_protocol_error( (uint8) (rrc_get_state()),
                          (uint8) RRC_PROCEDURE_RBE,
                          (uint8) ptr->message.u.radioBearerSetupFailure.failureCause.t,
                          (uint8) prot_err_cause
                        );

  /* Get the RLC logical channel id for this msg from Logical Channel Manager */
  ul_lc_id = rrclcm_check_ul_rlc_lc_id ( UE_LOGCHAN_DCCH,
                                         DCCH_AM_RADIO_BEARER_ID,
                                         UE_MODE_ACKNOWLEDGED_DATA);

  /* If the logical channel id is not invalid, send the message to the
  send chain module */
  if (ul_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    WRRC_MSG1_HIGH("Sending RBS Failure msg to send-chain Cause:%d",
             ptr->message.u.radioBearerSetupFailure.failureCause.t);
    /* Need to wait for L2 ack for all states because RRC needs to trigger include reconfig status indicator
    if RLC acknowledgement is not yet received.
    As per 8.3.1.3
    1> if the IE "Cell update cause" is set to "radio link failure" and the UE has submitted a reconfiguration response
    message to RLC and the UE has not received the RLC acknowledgement for the response message:
    2> include the IE "Reconfiguration Status Indicator" and set it to TRUE.*/
    if(rbe.status.failure_status == RRCRB_PHY_CHAN_FAILURE)
    {
      rrc_update_reconfig_waiting_for_l2ack_dch(RRC_PROCEDURE_RBE);
      status = rrcsend_send_ul_sdu(RRC_PROCEDURE_RBE,  /* Procedure is RBE    */
                                   (void *)ptr,        /* Message pointer     */
                                   ul_lc_id,           /* RLC id              */
                                   RRCSEND_L2ACK_REQUIRED  /* ACK is required */
                                  );
    }
    else
    {
      status = rrcsend_send_ul_sdu(RRC_PROCEDURE_RBE,  /* Procedure is RBE    */
                                   (void *)ptr,        /* Message pointer     */
                                   ul_lc_id,           /* RLC id              */
                                   RRCSEND_L2ACK_NOT_REQUIRED  /* No need for ACK */
                                  );
    }
    if ( status != RRCSEND_SUCCESS )
    {
      WRRC_MSG1_ERROR("Send chain failed for RBS failure msg: error cause %d", status);
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
}/* end function send_rbs_failure_message */

/*===========================================================================

FUNCTION   rrcrbe_append_start_list_in_ul_counter_sync_info

DESCRIPTION

  This function appends start list in ul counter sync info in rbs-complete msg.
  This takes care of the start value of a given domain which will be included
  in rbs-complete msg and hence it doesnt re-compute the start value for that
  domain.

DEPENDENCIES

  None.

RETURN VALUE

  SUCCESS or FAILURE

SIDE EFFECTS

  None.

===========================================================================*/
static uecomdef_status_e_type rrcrbe_append_start_list_in_ul_counter_sync_info
(
  rrc_UL_DCCH_Message *ptr,
  byte start_value[RRC_MAX_START_LENGTH], /* start_value should always have a valid value */
  rrcsmc_append_start_e_type append_start_type
)
{
  /* Local ptr for start list traversal */
  rrc_STARTList *start_list_ptr;

  if (RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.radioBearerSetupComplete,
    rrc_RadioBearerSetupComplete,ul_CounterSynchronisationInfo))
  {
    /* ul-counter sync info already updated */
    WRRC_MSG0_HIGH("ul-counter-sync-info already updated - ignore this call");
    return(SUCCESS);
  }
  RRC_MSG_COMMON_SET_BITMASK_IE(ptr->message.u.radioBearerSetupComplete,
   rrc_RadioBearerSetupComplete,ul_CounterSynchronisationInfo); 

  memset(&ptr->message.u.radioBearerSetupComplete.ul_CounterSynchronisationInfo.m,0,
          sizeof(ptr->message.u.radioBearerSetupComplete.ul_CounterSynchronisationInfo.m));

  /* Assign local ptr to Start List */
  start_list_ptr = &ptr->message.u.radioBearerSetupComplete.ul_CounterSynchronisationInfo.startList;

  return rrcsmc_append_start_list_with_already_computed_start(
           start_list_ptr,
           start_value,
           append_start_type);

}

/*===========================================================================

FUNCTION   SEND_RBS_COMPLETE_MSG

DESCRIPTION

  This function sends the Radio Bearer Setup Complete message
  on Uplink DCCH.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void send_rbs_complete_message( boolean proc_wait_for_l2_ack )
{
  rrc_UL_DCCH_Message *ptr;     /* Pointer to uplink DCCH message type */
  rlc_lc_id_type ul_lc_id; /* Logical Channel Id for the message  */
  rrcsend_status_e_type status; /* Status from send chain module       */
  boolean new_tm_rb_setup = FALSE; /* Boolean to keep track of new TM RB setup */
  /*to get start value if new UM/AM rb's are added*/
  byte start_value[RRC_MAX_START_LENGTH] = {0};

  /* Allocate memory for this message. Note that this buffer is released
  by the SEND CHAIN */
  ptr = (rrc_UL_DCCH_Message *)rrc_malloc(sizeof(rrc_UL_DCCH_Message));
  if (proc_wait_for_l2_ack)
  {
     WRRC_MSG0_MED("Waiting for L2 ACK for RBE Complete msg");
  }

  ptr->message.t = T_rrc_UL_DCCH_MessageType_radioBearerSetupComplete; /* RBS Complete msg */

  /* Fill up the message contents. The bit_mask is set to 0 since none
  of the optional IE's are included for now. */
  RRC_RESET_MSG_IE_PRESENT(ptr->message.u.radioBearerSetupComplete);

  /* make sure that dl-counter-sync info is not received in the rb-setup message */
  if (FALSE == rbe.rbe_for_srns_relocation)
  {
    if (rrcsmc_get_and_reset_start_value_to_transmit(start_value) == TRUE)
    {
      /*if ciphering is enabled and new um/am rb's are setup, then send
        START value for n/w to sync up*/
      ptr->message.u.radioBearerSetupComplete.m.start_ValuePresent = 1;

      ptr->message.u.radioBearerSetupComplete.start_Value.numbits =
        RRC_START_VALUE_LENGTH;

      /*copy the contents of latest_start_list into start_list*/
      WCDMA_MEMCPY(ptr->message.u.radioBearerSetupComplete.start_Value.data,
                   ptr->message.u.radioBearerSetupComplete.start_Value.numbits,
                   start_value,
                   RRC_MAX_START_LENGTH);

      WRRC_MSG3_MED("Reconfig START: 0x%x 0x%x 0x%x", start_value[0],
                                                 start_value[1],
                                                 start_value[2]);
    }
  }
  /* in case its srns relocation then we need to set the 
    start value because this function will not be called.
    If this function would have been called earlier then 
    this call to this function is nothing but a NOOP.
    */
  (void) rrcsmc_get_and_reset_start_value_to_transmit(start_value);
  
  if (rbe.rbe_for_srns_relocation == TRUE)
  {
    (void)rrcrbe_append_start_list_in_ul_counter_sync_info
      (ptr, start_value, RRCSMC_EXTRACT_AND_APPEND_START);
  }
  else
  {
    if (rrcllc_get_rlc_size_change_status() 
      && ((rbe.state_from_msg == RRC_STATE_CELL_PCH || 
             rbe.state_from_msg == RRC_STATE_URA_PCH)
          || (rrc_get_rlc_size_change_status_for_procedure_id(RRC_PROCEDURE_RBE) == TRUE)))
    {
      // Check if next state is CELL+PCH, and OC API returns RLC size change, then include counter sync
      if (rbe.state_from_msg == RRC_STATE_CELL_PCH || 
          rbe.state_from_msg == RRC_STATE_URA_PCH)
      {
        /* Ask SMC to do start value calculation for PS domain */
        WRRC_MSG0_HIGH("RLCSIZEINDICATED Invoking SMC extract func");
        (void)rrcsmc_extract_return_hfn_per_domain_and_update_start_flag(RRC_PS_DOMAIN_CN_ID);
      }
      else if (rrc_get_rlc_size_change_status_for_procedure_id(RRC_PROCEDURE_RBE) == TRUE)
      {
        rrc_set_rlc_size_change_status(RRC_PROCEDURE_NONE, 
                                       FALSE, RRC_INVALID_RB_ID ,
                                       RLC_RE_ESTABLISH_NONE);
      }

      (void)rrcrbe_append_start_list_in_ul_counter_sync_info
        (ptr, start_value, RRCSMC_APPEND_START);
    }
  }
  

  /* Transaction Id - use the accepted transaction id if known. Else
  use zero. Since this is a RBS Complete message, we don't check
  rejected transaction ids. */
  if ( rbe.tr_ids.accepted_transaction != RRCRB_INVALID_TRANSACTION_ID )
  {
    if (rbe.tr_ids.accepted_transaction<4)
    {
      ptr->message.u.radioBearerSetupComplete.rrc_TransactionIdentifier =
      rbe.tr_ids.accepted_transaction;
    }
    else
    {
      WRRC_MSG1_HIGH("Invalid TransactionId:%d in RBE db sending 0",
                  rbe.tr_ids.accepted_transaction);
      ptr->message.u.radioBearerSetupComplete.rrc_TransactionIdentifier = 0;
    }
    /* We no longer need to keep this id. Make rejected_transaction id
    invalid again unless rb substate is RRCRB_WAIT_FOR_L2_ACK */
    if (!proc_wait_for_l2_ack)
    {
      rbe.tr_ids.accepted_transaction = RRCRB_INVALID_TRANSACTION_ID;
    }
  }
  else
  {
    /* Don't know the transaction id for this message.( one reason
    for this could be that the message had a critical
    extenstion). Since it's unknown, we should set it to zero. */
    ptr->message.u.radioBearerSetupComplete.rrc_TransactionIdentifier = 0;
    WRRC_MSG0_HIGH("Invalid Trid for RBS Complete msg!");
  }

  /* For now, there is nothing else to include in this message. We
  can give it to the send chain. */

  /* Get the RLC logical channel id for this msg from Logical Channel Manager */
  ul_lc_id = rrclcm_check_ul_rlc_lc_id ( UE_LOGCHAN_DCCH,
                                         DCCH_AM_RADIO_BEARER_ID,
                                         UE_MODE_ACKNOWLEDGED_DATA);

  /* If the logical channel id is not invalid, send the message to the
  send chain module */
  if (ul_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
#ifdef FEATURE_RRC_DELAY_TX_ON_NEW_CFG
    /* Start a timer for 5 seconds, so that the
    base station can enable the new configuration before the
    message is sent. This is needed only if the base station
    cannot have both the old and new configurations up at
    the same time. */
    WRRC_MSG0_HIGH("Delaying 5 seconds");
    (void) rex_clr_sigs(rex_self(), RRCRBE_TEMP_TIMER_SIG);
    rex_set_timer(&rrcrbe_temp_timer, 5000);   /* 5 seconds */

    rrcrbe_sig = rex_wait(RRCRBE_TEMP_TIMER_SIG);
    MSG_LOW("RRCRBE signal:%d", rrcrbe_sig,0, 0);
    (void) rex_clr_sigs(rex_self(), RRCRBE_TEMP_TIMER_SIG);
#endif /* FEATURE_RRC_DELAY_TX_ON_NEW_CFG */

    new_tm_rb_setup = rrcsmc_is_new_tm_rb_setup();

    if( (rrcsmc_is_ciphering_enabled() == RRCSMC_SECURITY_STARTED) &&
        (new_tm_rb_setup == TRUE) )
    {
      /*if ciphering is enabled and new tm rb's are setup, then send
        ciphering activation time in rb_setup_complete message*/
     RRC_MSG_COMMON_SET_BITMASK_IE(ptr->message.u.radioBearerSetupComplete,
        rrc_RadioBearerSetupComplete,count_C_ActivationTime);

      ptr->message.u.radioBearerSetupComplete.count_C_ActivationTime =
        ciphering_activation_cfn;

      WRRC_MSG1_HIGH("Ciphering activation CFN = %d", ciphering_activation_cfn);
    }

    MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"hho_ciph_status: %d, trans_to_dch_ciph_config_needed: %d, new_tm_rb_setup: %d TM_CIPH_STATUS : %d FIRST TM RB STATUS :%d",
    rbe.hho_ciph_status, rbe.trans_to_dch_ciph_config_needed, new_tm_rb_setup,rrcsmc_tm_ciph_active(),rbe.first_tm_rb_setup);
   
    
    //if this is case of IFHHO and not SRNS relocation(i.e. ul counter sync is not present)
    //then also send START_List and set count c act time and configure smc for Step_2
    if (((rbe.hho_ciph_status == TRUE) || (rbe.trans_to_dch_ciph_config_needed == TRUE)) &&
        ((TRUE == rrcsmc_tm_ciph_active()) && (FALSE == new_tm_rb_setup)))
    {
      (void)rrcrbe_append_start_list_in_ul_counter_sync_info(
        ptr, start_value, RRCSMC_EXTRACT_AND_APPEND_START);

      //Set the bitmask for count_c_act_time and copy the same..
      RRC_MSG_COMMON_SET_BITMASK_IE(ptr->message.u.radioBearerSetupComplete,
        rrc_RadioBearerSetupComplete,count_C_ActivationTime);
      
      ptr->message.u.radioBearerSetupComplete.count_C_ActivationTime =
        rrcsmc_step_2_cipher_config_for_hho();

      //Send step 2 config for HHO.. only one of these flags will be active, reset both of them
      //here
      if(rbe.hho_ciph_status)
      {
        rbe.hho_ciph_status =  FALSE;      
      }
      else if(rbe.trans_to_dch_ciph_config_needed)
      {
        rbe.trans_to_dch_ciph_config_needed = FALSE;      
      }
    }
    /* Check for HHO condition */
    if(TRUE == rbe.hho_ciph_status)
    {
      /* Check if TM bearers were established using the RBE message. If so do not do a Step-1, Step2
         ciphering config. Need to include UL Counter Sync info, but do not do the start value calculation 
         for the CS domain since it already has been done as a prt of channel config req. 
         Use the value that is appended in the "Start Value" above. Extract the start value
         for PS domain alone */
      if(TRUE == new_tm_rb_setup)
      {
        (void)rrcrbe_append_start_list_in_ul_counter_sync_info(
          ptr, start_value, RRCSMC_EXTRACT_AND_APPEND_START);
      }
    }

    if((TRUE == rrcsmc_tm_ciph_active()) && (rbe.first_tm_rb_setup == TRUE))
    {
      WRRC_MSG0_HIGH("First TM RB setup. Step2 HHO");
      if(!(RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.radioBearerSetupComplete,
        rrc_RadioBearerSetupComplete,count_C_ActivationTime)))
      {
        //Set the bitmask for count_c_act_time and copy the same..
        RRC_MSG_COMMON_SET_BITMASK_IE(ptr->message.u.radioBearerSetupComplete,
               rrc_RadioBearerSetupComplete,count_C_ActivationTime);
      }

      ptr->message.u.radioBearerSetupComplete.count_C_ActivationTime =
      rrcsmc_step_2_cipher_config_for_first_tm_rb_setup();

      //Send step 2 config for HHO.. only one of these flags will be active, reset both of them
      //here
      rbe.hho_ciph_status =  FALSE;      
      rbe.first_tm_rb_setup = FALSE;
    }

    if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
    {
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
      if(rrcsib_is_dmcr_ie_needed() == TRUE)
      {
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.radioBearerSetupComplete, laterNonCriticalExtensions);
 
        RRC_RESET_MSG_IE_PRESENT(ptr->message.u.radioBearerSetupComplete.laterNonCriticalExtensions);
 
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.radioBearerSetupComplete.laterNonCriticalExtensions, v770NonCriticalExtensions);
 
        RRC_RESET_MSG_IE_PRESENT(ptr->message.u.radioBearerSetupComplete.laterNonCriticalExtensions.v770NonCriticalExtensions);
 
        RRC_RESET_MSG_IE_PRESENT(ptr->message.u.radioBearerSetupComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
          radioBearerSetupComplete_v770ext);
 
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.radioBearerSetupComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
          radioBearerSetupComplete_v770ext, deferredMeasurementControlReading);
        
        ptr->message.u.radioBearerSetupComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
          radioBearerSetupComplete_v770ext.deferredMeasurementControlReading = rrc_RadioBearerSetupComplete_v770ext_IEs_deferredMeasurementControlReading_true;
      }
#endif
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
    if(rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr != NULL)
    {
      if(RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.radioBearerSetupComplete,
  	  rrc_RadioBearerSetupComplete,count_C_ActivationTime))
  	  {
        ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.ul_ota_parms.count_c_activation= ptr->message.u.radioBearerSetupComplete.count_C_ActivationTime;
  	  }
      if (RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.radioBearerSetupComplete,
      rrc_RadioBearerSetupComplete,ul_CounterSynchronisationInfo))
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
    WRRC_MSG0_HIGH("Sending RBS Complete message to send-chain");
    if (proc_wait_for_l2_ack)
    {
      status = rrcsend_send_ul_sdu(RRC_PROCEDURE_RBE,  /* Procedure is RBE    */
                                   (void *)ptr,        /* Message pointer     */
                                   ul_lc_id,           /* RLC id              */
                                   RRCSEND_L2ACK_REQUIRED  /* ACK is required */
                                   );
    }
    else
    {
      status = rrcsend_send_ul_sdu(RRC_PROCEDURE_RBE,  /* Procedure is RBE    */
                                   (void *)ptr,        /* Message pointer     */
                                   ul_lc_id,           /* RLC id              */
                                   RRCSEND_L2ACK_NOT_REQUIRED  /* ACK is NOT required */
                                   );
    }

    if ( status != RRCSEND_SUCCESS )
    {
      WRRC_MSG1_ERROR("Send chain failed for RBS complete msg: error cause %d", status);
    }
    else /* Send is successful */
    {
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

}/* end function send_rbs_complete_message */


/*===========================================================================

FUNCTION   RRCRBE_SEND_INDICATION_TO_UPPER_LAYERS

DESCRIPTION

  This function processes the received Layer 2 ACK indication.
  If the L2 ACK indicates that the RBS Complete message has
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
static void rrcrbe_send_indication_to_upper_layers
(
  void
)
{
  mm_cmd_type *mm_ptr;     /* To send the MM_SYNC_IND command */
  rabm_cmd_type *rabm_ptr; /* To send the RABMAS_RAB_ESTABLISH_IND command */
  uint32 rab_index=0;    /* Used if there is more than 1 RAB in the message */
  /* send the nas cn info if stored */
  if (rbe.cn_info_saved != FALSE)
  {
    WRRC_MSG0_HIGH("Dispatching out MM cmd / RRC_SERVICE_IND_FROM_OTA_MSG due to CN info to MM");
    if(rbe.mm_cmd_ptr->cmd.hdr.message_id ==(byte) RRC_SERVICE_IND_FROM_OTA_MSG)
    {
      /* update the cell access information in service info */
      rrc_ccm_update_service_info(rbe.mm_cmd_ptr);
    }
    /* Put the command on MM queue */
    rrc_send_rrc_message_to_mm(rbe.mm_cmd_ptr);
    /* clear the flag */
    rbe.cn_info_saved = FALSE;
  }

  /* Now we need to send primitives to the appropriate NAS entity.
    CS calls, it's to the MM layer and for PS calls it's to the RABM layer.*/
  for (rab_index=0; rab_index < rbe.rabs.num_rabs_in_msg; rab_index++)
  {
    if ( (rbe.rabs.rabs_in_msg[rab_index].rab_type == RRCRB_CS_VOICE_RAB) ||
         (rbe.rabs.rabs_in_msg[rab_index].rab_type == RRCRB_CS_DATA_RAB) )
    {
      /* It's a CS RAB. Send the RRC_SYNC_IND primitive to MM. */
     mm_ptr = mm_rrc_get_cmd_buf(RRC_SYNC_IND);

#ifdef FEATURE_DUAL_SIM
      mm_ptr->cmd.rrc_sync_ind.as_id = rrc_get_as_id();
#endif

      mm_ptr->cmd.rrc_sync_ind.cn_domain_id = RRC_CS_DOMAIN_CN_ID;

      /* If MS is in loopback test mode, set casue to Loopback */
      if ( rrclbt_is_lb_on() == TRUE )
      {
        mm_ptr->cmd.rrc_sync_ind.cause = RRC_LOOPBACK_ESTABLISHED;
      }
      else
      {
        mm_ptr->cmd.rrc_sync_ind.cause = RRC_RAB_ESTABLISHED;
      }
      mm_ptr->cmd.rrc_sync_ind.rab_id_present = TRUE;    
      mm_ptr->cmd.rrc_sync_ind.rab_info.action = RAB_ESTABLISHED;    
      mm_ptr->cmd.rrc_sync_ind.rab_info.rab_id = rbe.rabs.rabs_in_msg[rab_index].rab_id;

      if(rbe.rabs.rabs_in_msg[rab_index].rab_type == RRCRB_CS_VOICE_RAB)
      {
        mm_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type = RRCMN_CS_VOICE_CALL;
       /* Set the CS RAB established status to TRUE here to trigger FR2LTE in case UE moved to WCDMA for CSFB call */
#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
        if(csfb_extended_status == CSFB_STARTED)
        {
          rrc_cs_rab_established = TRUE;
        }
#endif
      }
      else if(rbe.rabs.rabs_in_msg[rab_index].rab_type == RRCRB_CS_DATA_RAB)
      {
        mm_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type = RRCMN_CS_DATA_CALL;
      }
      else
      {
        WRRC_MSG0_ERROR("Something wrong with RAB Type");      
        mm_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type = RRCMN_INVALID_RAB_ID;
      }
  
#ifdef FEATURE_VOC_AMR_WB
      /* Fill in the codec type used in RRC_SYNC_IND*/ 
      mm_ptr->cmd.rrc_sync_ind.rab_info.codec = RRC_CODEC_NONE;
      if(rbe.rabs.rabs_in_msg[rab_index].rab_type == RRCRB_CS_VOICE_RAB)
      {
        if(rrc_get_current_codec_mode() == MVS_MODE_AMR_WB )
        {
          mm_ptr->cmd.rrc_sync_ind.rab_info.codec = RRC_CODEC_WB_AMR;
        }
        else
        {
          mm_ptr->cmd.rrc_sync_ind.rab_info.codec = RRC_CODEC_NB_AMR;
        }
      }
#endif/*FEATURE_VOC_AMR_WB*/

#ifdef FEATURE_UMTS_VOICE_CIPHERING_IND
      #error code not present
#endif /*FEATURE_UMTS_VOICE_CIPHERING_IND*/

      /* Put the command on MM queue */
      MSG_HIGH("Dispatching out MM cmd / Sending RRC_SYNC_IND to MM for RAB type %d RAB-id %d Codec %d ", 
                mm_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type, 
                mm_ptr->cmd.rrc_sync_ind.rab_info.rab_id,
                mm_ptr->cmd.rrc_sync_ind.rab_info.codec);
      rrc_send_rrc_message_to_mm(mm_ptr);
    }

    else if ( rbe.rabs.rabs_in_msg[rab_index].rab_type == RRCRB_PS_DATA_RAB )
    {

      if(rrcscrr_is_cn_domain_open(RRC_PS_DOMAIN_CN_ID) == TRUE)
      {
        /* It's a PS RAB. Send the RABMAS_RAB_ESTABLISH_IND primitive to RABM. */
        if ((rabm_ptr = rabm_get_cmd_buf()) == NULL)
        {
          ERR_FATAL("Can't send RABM cmd - out of memory!", 0, 0, 0);
        }
        rabm_ptr->header.cmd_id = RABMAS_RAB_ESTABLISH_IND;
        rabm_ptr->header.message_set = MS_RABM_RRC;
        rabm_ptr->cmd.rrc_establish_ind.rabid = (rabid_T)(rbe.rabs.rabs_in_msg[rab_index].rab_id);
#ifdef FEATURE_DUAL_SIM
        rabm_ptr->cmd.rrc_establish_ind.as_id = rrc_get_as_id();
#endif
        /* Put the command on RABM queue */
        WRRC_MSG1_HIGH("Sending RAB_ESTABLISH_IND to RABM RABid %d",
                 rbe.rabs.rabs_in_msg[rab_index].rab_id);
        rabm_put_cmd(rabm_ptr);
      }
      else
      {
        WRRC_MSG0_HIGH("Skip sending RAB_ESTABLISH_IND as PS domain not open");
      }
    }
  }
#ifdef FEATURE_DUAL_SIM
  rrc_check_send_change_priority();
#endif
} /*rrcrbe_send_indication_upper_layers */

/*===========================================================================

FUNCTION rrcrbe_send_crnti_update_req

DESCRIPTION
  This function sends the RNTI Update request to LLC to update
  with new C-RNTI.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static void  rrcrbe_send_crnti_update_req(uint16  crnti)
{
  rrc_cmd_type  *cmd_ptr;

  /* Allocates the buffer to RRC Internal command */
  if( (cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
  {
    cmd_ptr->cmd_hdr.cmd_id = RRC_RNTI_UPDATE_REQ;

    /* Fill parameters for RRC_RNTI_UPDATE_REQ command */
    cmd_ptr->cmd.rnti_update_req.procedure = RRC_PROCEDURE_RBE;

    /* Indicate that C-RNTI needs to be updated */
    cmd_ptr->cmd.rnti_update_req.crnti_action = RRC_RNTI_UPDATE;
    cmd_ptr->cmd.rnti_update_req.crnti = crnti;

    /* Indicate that U-RNTI has not changed. Not till S RNS relocation
     * is supported.
     */
    cmd_ptr->cmd.rnti_update_req.urnti_action = RRC_RNTI_NOCHANGE;

    cmd_ptr->cmd.rnti_update_req.rlc_re_establish_srb = FALSE;
    cmd_ptr->cmd.rnti_update_req.rlc_re_establish_rab = FALSE;

    /* Confirmation from LLC is not needed */
    cmd_ptr->cmd.rnti_update_req.cnf_required = FALSE;

    rrc_put_int_cmd(cmd_ptr);       /* sends the command to RRC  LLC. */

  }
}  /* rrcrbe_send_crnti_update_req */

/*===========================================================================

FUNCTION rrcrbe_send_cell_update_initiate_req

DESCRIPTION
  This function send the cell update initiate request if UE does not have
  a valid C-RNTI in CELL_FACH state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static void rrcrbe_send_cell_update_initiate_req
(
rrc_proc_e_type procedure
)
{
  rrc_cmd_type *int_cmd_ptr;

  if ( (int_cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
  {
    /* Fill in the command parameters for RRC_CELL_UPDATE_INITIATE_REQ */
    int_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_UPDATE_INITIATE_REQ;
    int_cmd_ptr->cmd.initiate_cell_update.cause = RRC_CELL_RESELECTION;
    int_cmd_ptr->cmd.initiate_cell_update.cu_complete_cnf_required = TRUE;
    int_cmd_ptr->cmd.initiate_cell_update.procedure = procedure;

    rrc_put_int_cmd(int_cmd_ptr);
  }
}/*rrcrbe_send_cell_update_initiate_req*/

/*===========================================================================

FUNCTION   RRCRBE_VALIDATE_SRNS_RELOCATION_INFO()

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
static boolean rrcrbe_validate_srns_relocation_info
(
rrc_RadioBearerSetup *msg_ptr      /* Pointer to the RB Setup message */
,rrc_msg_ver_enum_type msg_version
)
{
   /*check whether dl_counter_sync info is present or not . Do not check for
   the contents of DL_counter_sync_info. First the integrity check has tobe made
   with new integrity protection mode info in the message. Later we can check
   the dl_counter_sync_info contents and send a failure if PDCP info is present */

  if (msg_version == MSG_REL5)
  {
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.radioBearerSetup_r5,
      rrc_RadioBearerSetup_r5_IEs,dl_CounterSynchronisationInfo))
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else if (msg_version == MSG_REL6)
  {
     if (RRC_CHECK_COMMON_MSG_TYPE(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
       criticalExtensions.u.r6.radioBearerSetup_r6.specificationMode,
       rrc_RadioBearerSetup_r6_IEs_specificationMode_complete) &&
      (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
       criticalExtensions.u.r6.radioBearerSetup_r6.specificationMode.u.
       complete.m.dl_CounterSynchronisationInfoPresent))
     {
       return TRUE;
     }
     else
     {
       return FALSE;
     }
  }
  else if (msg_version == MSG_REL7)
  {
     if ((RRC_CHECK_COMMON_MSG_TYPE(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
       criticalExtensions.u.r7.radioBearerSetup_r7.specificationMode,
       rrc_RadioBearerSetup_r7_IEs_specificationMode_complete)) &&
      (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
       criticalExtensions.u.criticalExtensions.u.r7.radioBearerSetup_r7.specificationMode.u.complete.m. 
       dl_CounterSynchronisationInfoPresent))
     {
       return TRUE;
     }
     else
     {
       return FALSE;
     }
  }
  else if (msg_version == MSG_REL8)
  {
    if (RRCRB_R8_MSG_IE_PRESENT((&msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
        criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.radioBearerSetup_r8),rrc_RadioBearerSetup_r8_IEs,
        dl_CounterSynchronisationInfo))
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
#ifdef FEATURE_WCDMA_REL9
  else if (msg_version == MSG_REL9)
  {
    if (RRCRB_R9_MSG_IE_PRESENT((&msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
        criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.radioBearerSetup_r9),
        dl_CounterSynchronisationInfo))
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }

#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
  #error code not present
#endif /* FEATURE_WCDMA_REL10 */


  else
  if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
      rrc_RadioBearerSetup_r3_IEs,dl_CounterSynchronisationInfo))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION   rrcrbe_is_first_tm_rb_setup

DESCRIPTION

  This function checks whether any of TM RBs have been already established or not.

DEPENDENCIES

  None.

RETURN VALUE

  TRUE : If a new TM RB is being set up and there's no existing TM RB in the RABs already established.
  FALSE : Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
static boolean rrcrbe_is_first_tm_rb_setup
(
  boolean new_tm_rb
)
{
  boolean status = TRUE;
  uint8 i=0, j=0, lc_id;
  rrclcm_status_e_type rrclcm_status = RRCLCM_FAILURE;
  uecomdef_logch_mode_e_type rx_mode;
  boolean dl_mode_tm = FALSE;
  
  if(new_tm_rb)
  {
    for(i=0;i<MAX_RAB_TO_SETUP;i++)
    {
      for(j=0;j<rrc_est_rabs.rabs[i].num_rbs_for_rab;j++)
      {
        dl_mode_tm = FALSE;
        lc_id = rrclcm_get_dl_rlc_lc_id(rrc_est_rabs.rabs[i].rb_for_rab[j].rb_id);
        if( lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
        {
          rrclcm_status = rrclcm_get_dl_rlc_mode(lc_id, &rx_mode);
          if((rrclcm_status == RRCLCM_SUCCESS) && (rx_mode == UE_MODE_TRANSPARENT))
          {
            dl_mode_tm = TRUE;
          }
        }

        if(dl_mode_tm == TRUE)
        {
          lc_id = rrclcm_get_ul_rlc_lc_id(rrc_est_rabs.rabs[i].rb_for_rab[j].rb_id);
          if( lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
          {
            rrclcm_status = rrclcm_get_ul_rlc_mode(lc_id, &rx_mode);
            if((rrclcm_status == RRCLCM_SUCCESS) && (rx_mode == UE_MODE_TRANSPARENT))
            {
              status = FALSE;
              break;
            }
          }
        }
      }
      if(status == FALSE)
      {
        break;
      }
    }
  }
  else
  {
    status = FALSE;
  }

  return status;
}


/*===========================================================================

FUNCTION   VALIDATE_RBSETUP_R5

DESCRIPTION

  This function validates the received R5 RB Setup Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a Radio Bearer Setup Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_rbsetup_r5
(
rrc_RadioBearerSetup *msg_ptr      /* Pointer to the RB Setup message */
)
{

  uecomdef_status_e_type status;

  rrc_RadioBearerSetup_r5_IEs* rbsetup_ptr = NULL;

  rrc_RAB_InformationSetupList_r5 *rab_list_ptr = NULL;

  rrc_RB_InformationSetupList_r5 *rb_list_ptr = NULL;

#ifdef FEATURE_UMTS_PDCP

  rrc_HeaderCompressionInfo_r4 *pdcpHeaderCompressionInfo = NULL;

#endif /* FEATURE_UMTS_PDCP*/

  boolean new_tm_rb = FALSE;
  rlc_lc_id_type local_lc_id;
  uecomdef_logch_mode_e_type rx_mode;
  rrclcm_status_e_type result;
  boolean dl_tm_flag;

  rrc_rab_id_type rab_id;           /* To store RAB id in internal RRC format */
  rrcmn_rab_status_T rab_type = RRCMN_INVALID_RAB_ID;     /* To store the status returned by MN layer. This
                                    indicates what type of RAB is received. This is
                                    used only if the RAB is for the CS_domain */
  uint32 rab_index=0;               /* Used as an index when storing established
                                    RABs locally. Only when we have more than
                                    one RAB per message, this will be incremented */
  uint32 rb_index =0;                          
  boolean voice_rab_already_exists = FALSE;
#ifdef FEATURE_VOC_AMR_WB
 /* set the default nas_sync_ind to the current codec used*/
  mvs_mode_type nas_sync_ind=rrc_get_current_codec_mode();
#endif /*FEATURE_VOC_AMR_WB*/
  /* Assume the message is valid for now */
  rbe.status.failure_status = RRCRB_MSG_VALID;


  rbsetup_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.radioBearerSetup_r5;
  switch (rbsetup_ptr->rrc_StateIndicator)
  {
  case rrc_RRC_StateIndicator_cell_DCH:

    rbe.state_from_msg = RRC_STATE_CELL_DCH;
    break;

  case rrc_RRC_StateIndicator_cell_FACH:

    rbe.state_from_msg = RRC_STATE_CELL_FACH;
    break;

  default: /* Just to satisfy LINT */
    WRRC_MSG0_ERROR("Invalid RRC state from Msg");
    rbe.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
    break;

  } /* End of switch */

  if ( rbe.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
    return;
  }

  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,rrc_RadioBearerSetup_r5_IEs,
  	dl_HSPDSCH_Information))
  {

    /* Check if network does not try to initiate HSDPA with invalid state */

    status = rrc_hsdpa_initial_validation(rbsetup_ptr->rrc_StateIndicator, 
      RRC_CHECK_COMMON_MSG_TYPE(rbsetup_ptr->dl_HSPDSCH_Information.modeSpecificInfo,
      rrc_DL_HSPDSCH_Information_modeSpecificInfo_tdd));


    if (status == FAILURE)
    {
      rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }
  rab_index =0;
  /* Check if there are new RABs to be established */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
  	rrc_RadioBearerSetup_r5_IEs,rab_InformationSetupList))
  {

    rab_list_ptr = &rbsetup_ptr->rab_InformationSetupList;

    /* Check if the RAB id is GSM_MAP type. We don't support any
    other type */
    if ( !(rab_list_ptr->elem[rab_index].rab_Info.rab_Identity.t ==
          T_rrc_RAB_Identity_gsm_MAP_RAB_Identity))
    {
      WRRC_MSG1_HIGH("RAB ID type %d is not GSM_MAP!",rab_list_ptr->elem[rab_index].rab_Info.rab_Identity.t);
      rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return;
    }

    /* check for the unsupported PDCP info */

    do  /* going through the RAB info setup list */
    {
      rb_list_ptr = &rab_list_ptr->elem[rab_index].rb_InformationSetupList;
      rb_index= 0;
      do  /* going through the RB info setup list */
      {
        rrc_RB_InformationSetup_r5 * rb_setup_info_ptr;
		rb_setup_info_ptr = &rb_list_ptr->elem[rb_index];
        if (rrc_find_rb_in_est_rabs(rb_setup_info_ptr->rb_Identity) == RRC_RB_PRESENT) 
        {
          WRRC_MSG1_HIGH("RB id %d already exist in est rabs: Invalid config", 
                   rb_setup_info_ptr->rb_Identity);
          rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          return;
        }

        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_setup_info_ptr,
            rrc_RB_InformationSetup_r5,pdcp_Info))
        {
#ifdef FEATURE_UMTS_PDCP
          if (rrc_pdcp_enabled)
          {
           /* If the RAB is for cs domain, send RBS failure with cause
             *   invalid configuration
             */
            if (rab_list_ptr->elem[rab_index].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain)
            {
              WRRC_MSG0_HIGH("PDCP info invalid for CS rab");
              rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
              return;
            }
            /* check whether lossless SRNS relocation flag is set
             * and header compression algo is present in the message */
            if ((RRC_MSG_COMMON_BITMASK_IE(rb_setup_info_ptr->pdcp_Info,
				rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
                (RRC_CHECK_COMMON_MSG_TYPE(rb_setup_info_ptr->pdcp_Info.losslessSRNS_RelocSupport,
                rrc_LosslessSRNS_RelocSupport_supported)))
            {
              if (rb_setup_info_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_absent)
              {
                WRRC_MSG0_HIGH("PDCP pdu header is absent.  Invalid config");
                rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                return;
              }

            } /* if rrc_PDCP_Info_r4_losslessSRNS_RelocSupport_present */
            if (rb_setup_info_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present)
            {
              if ((RRC_MSG_COMMON_BITMASK_IE(rb_setup_info_ptr->pdcp_Info,
			  	  rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
                  (RRC_CHECK_COMMON_MSG_TYPE(rb_setup_info_ptr->pdcp_Info.losslessSRNS_RelocSupport,
                   rrc_LosslessSRNS_RelocSupport_notSupported)))
              {
                if (!(RRC_MSG_COMMON_BITMASK_IE(rb_setup_info_ptr->pdcp_Info,
					rrc_PDCP_Info_r4,headerCompressionInfoList)))
                {
                  WRRC_MSG0_HIGH("PDCP header compress info list not present");
                  rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                  return;
                }
              }
            }

            if (RRC_MSG_COMMON_BITMASK_IE(rb_setup_info_ptr->pdcp_Info,
				rrc_PDCP_Info_r4,headerCompressionInfoList))
            {
              uint32 local_idx=0;
             
              while (rb_setup_info_ptr->pdcp_Info.headerCompressionInfoList.n > local_idx)
              {
                 pdcpHeaderCompressionInfo = &rb_setup_info_ptr->pdcp_Info.headerCompressionInfoList.elem[local_idx];
                if (RRC_CHECK_COMMON_MSG_TYPE(pdcpHeaderCompressionInfo->algorithmSpecificInfo,
                    rrc_AlgorithmSpecificInfo_r4_rfc2507_Info))
                {
                  /* if the UE capability "Maximum header compression context space", is exceeded with this configuration send
                   * failure with cause "Invalid Config"
                   */
                  if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER > IPHC_MAX_HC_CONTEXT_SPACE)
                  {
                    WRRC_MSG1_HIGH("Invalid config.  max header %d is invalid",
                             pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER);
                    rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                    return;
                  }
                }
                else if (RRC_CHECK_COMMON_MSG_TYPE(pdcpHeaderCompressionInfo->algorithmSpecificInfo,
                         rrc_AlgorithmSpecificInfo_r4_rfc3095_Info))
                {
                  /* 	the chosen MAX_CID shall not be greater than the value "Maximum number of ROHC context sessions" as 
                   *  indicated in the IE "PDCP Capability"
                   */
                  if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(pdcpHeaderCompressionInfo->algorithmSpecificInfo.
				  	u.rfc3095_Info,ul_RFC3095))
                  {
                    if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->ul_RFC3095.max_CID > ROHC_MAX_CONTEXT_SESSIONS)
                    {
                      WRRC_MSG1_HIGH("Invalid config.  MAX_CID %d is invalid",
                               pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->ul_RFC3095.max_CID);
                      rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                      return;
                    }
                  }

                  if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info,
				  	dl_RFC3095))
                  {
                    if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->dl_RFC3095.max_CID > ROHC_MAX_CONTEXT_SESSIONS)
                    {
                      WRRC_MSG1_HIGH("Invalid config.  MAX_CID %d is invalid",
                               pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->dl_RFC3095.max_CID);
                      rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                      return;
                    }
                  }

                }  /* if (pdcpHeaderCompressionInfoList->value.algorithmSpecificInfo.t == rrc_rfc3095_Info)*/

                local_idx++;
              } /*  while (pdcpHeaderCompressionInfoList) */

            } /*  if (rb_list_ptr->value.pdcp_Info.bit_mask & rrc_PDCP_Info_r4_headerCompressionInfoList_present) */
          }
#endif
        } /* if (rb_list_ptr->value.bit_mask & rrc_RB_InformationSetup_r5_pdcp_Info_present) */

        if(new_tm_rb == FALSE)
        {
          dl_tm_flag = FALSE;
		  
          if((RRC_CHECK_COMMON_MSG_TYPE(rb_setup_info_ptr->rlc_InfoChoice,rrc_RLC_InfoChoice_r5_rlc_Info)) &&
          (((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_setup_info_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,ul_RLC_Mode)) &&
               (rb_setup_info_ptr->rlc_InfoChoice.u.rlc_Info->ul_RLC_Mode.t==
               T_rrc_UL_RLC_Mode_ul_TM_RLC_Mode))
               &&
               ((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_setup_info_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,dl_RLC_Mode)) &&
               (RRC_CHECK_COMMON_MSG_TYPE(rb_setup_info_ptr->rlc_InfoChoice.u.rlc_Info->dl_RLC_Mode,
                rrc_DL_RLC_Mode_r5_dl_TM_RLC_Mode)))))
          {
            new_tm_rb = TRUE;
          }
          else if(RRC_CHECK_COMMON_MSG_TYPE(rb_setup_info_ptr->rlc_InfoChoice,
		  	rrc_RLC_InfoChoice_r5_same_as_RB))
          {
            local_lc_id = rrclcm_get_dl_rlc_lc_id(rb_setup_info_ptr->rlc_InfoChoice.u.same_as_RB);
            if(local_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
            {
              result = rrclcm_get_dl_rlc_mode(local_lc_id, &rx_mode);
              if(result == RRCLCM_SUCCESS && rx_mode == UE_MODE_TRANSPARENT)
              {
                dl_tm_flag = TRUE;
              }
            }
  
            local_lc_id = rrclcm_get_ul_rlc_lc_id(rb_setup_info_ptr->rlc_InfoChoice.u.same_as_RB);
            if(local_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
            {
              result = rrclcm_get_ul_rlc_mode(local_lc_id, &rx_mode);
              if(dl_tm_flag == TRUE && result == RRCLCM_SUCCESS && rx_mode == UE_MODE_TRANSPARENT)
              {
                new_tm_rb = TRUE;
              }
            }
          }
        }
        /*point to the next link */
        rb_index++;
      } while (rb_list_ptr->n > rb_index);
      /* Go to next RAB setup infomation  */
      rab_index++;
    } while (rab_list_ptr->n > rab_index);

  }
  else
  {
    /* There is no RAB to be setup in this message.
      No further validation is needed */
    rbe.rabs.num_rabs_in_msg = 0;
  }
  if(new_tm_rb == TRUE)
  {
    WRRC_MSG0_HIGH("One or more TM RBs being setup");
  }
  /*check for the SRNS relocation info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
      rrc_RadioBearerSetup_r5_IEs,dl_CounterSynchronisationInfo))
  {

    /*check whether the SRNS relocation is within Cell_DCH or not.
  SRNS relocation information can come in reconfiguration messages
    only when the state transition is Cell_DCH->Cell_DCH */

    if (!((rbe.state_from_msg == RRC_STATE_CELL_DCH) &&
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("SRNS reloc with invalid states cur_state %d, msg_state %d",
          rrc_get_state(), rbe.state_from_msg);
      rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has
    PDCP info list. If it is true then this configuration is unsupported */

    if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2(rbsetup_ptr->dl_CounterSynchronisationInfo,
        rb_WithPDCP_InfoList)) ||
        (RRC_MSG_COMMON_BITMASK_IE(rbsetup_ptr->dl_CounterSynchronisationInfo,
         rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
    {
      WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
      rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation.
      Set the flag that indicates that this procedure
        involves SRNS relocation */
      rbe.rbe_for_srns_relocation = TRUE;
      /* store if new U-RNTI is present in the message */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
          rrc_RadioBearerSetup_r5_IEs,new_U_RNTI))
      {
        rbe.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(rbsetup_ptr->new_U_RNTI),
                             &rbe.new_urnti);
      }
    }

  }

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
  	rrc_RadioBearerSetup_r5_IEs,rab_InformationSetupList))
  {
    /* GSM_MAP RAB Id. Check if it's valid by calling the
    appropriate NAS entity. First convert it to internal
    format. */
    rab_index =0;
    rab_list_ptr = &rbsetup_ptr->rab_InformationSetupList;

    /* check whether all RABs in the rabinfo list belong to same CN domain id or not
       otherwise this is an invalid configuration */
    while (rab_list_ptr->n > (rab_index+1) )
    {
      if (rab_list_ptr->elem[rab_index].rab_Info.cn_DomainIdentity !=
          rab_list_ptr->elem[rab_index+1].rab_Info.cn_DomainIdentity)
      {
        WRRC_MSG2_ERROR("Different CN domains in rab-list %d %d",
             rab_list_ptr->elem[rab_index].rab_Info.cn_DomainIdentity,
             rab_list_ptr->elem[rab_index+1].rab_Info.cn_DomainIdentity
             );
        rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
        return;
      }
      rab_index++;;
    }
  
    /* Restore the Ptr */
    rab_list_ptr = &rbsetup_ptr->rab_InformationSetupList;
  
  
    for (rab_index=0; (rab_list_ptr->n > rab_index) && (rab_index < RRCRB_MAX_RAB_PER_MSG); rab_index++)
    {
      rab_id = rrcrb_translate_gsm_map_rab_id((rab_list_ptr->elem[rab_index].rab_Info.
                                                rab_Identity.u.gsm_MAP_RAB_Identity));
  
      /* Store this info locally so that ESTABLISHED_RABS can
    be updated later, when these RABs are established */
      rbe.rabs.rabs_in_msg[rab_index].rab_id_is_gsm_map = TRUE;
      rbe.rabs.rabs_in_msg[rab_index].rab_id = rab_id;
      rbe.rabs.rabs_in_msg[rab_index].re_est_timer =
      rab_list_ptr->elem[rab_index].rab_Info.re_EstablishmentTimer;
      /* Check if this RAB is CS Domain or PS Domain */
      if ( rab_list_ptr->elem[rab_index].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain )
      {
        /* Store the domain identity. This will be later added to
          ESTABLISHED_RABS */
        rbe.rabs.rabs_in_msg[rab_index].cn_domain = RRC_CS_DOMAIN_CN_ID;
  
        /* For CS RABs, Call MN layer to verify this RAB */
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
        rab_type = rrcmn_verify_rab_subs( (radio_access_bearer_T)rab_id,rrc_get_as_id());
#else
        rab_type = rrcmn_verify_rab( (radio_access_bearer_T)rab_id );
#endif
        MSG_HIGH("For RAB-Id %d, RAB type from NAS = %d, RAB type from EST_RABs = %d",rab_id, rab_type,
            rrc_get_rab_type_for_given_rab_id(rbe.rabs.rabs_in_msg[rab_index].rab_id));
  
        switch ( rab_type )
        {
          case RRCMN_CS_DATA_CALL:
  
            MSG_LOW("Received a CS Data RAB Id %d", rab_id, 0, 0);
            break;
  
          case RRCMN_CS_VOICE_CALL:
            MSG_LOW("Received a CS Voice RAB Id %d", rab_id, 0, 0);
            if (rrc_find_if_voice_rab_exists_in_established_rabs( rbe.rabs.rabs_in_msg[rab_index].rab_id )
              == TRUE)
            {
              WRRC_MSG1_HIGH("Voice RAB Id - %d already exists", rbe.rabs.rabs_in_msg[rab_index].rab_id );
              /*First condition of "if" covers the case when third RB is being added for AMR call*/
              /*Second condition of "if" covers the case when previously established Voice Call was of type CS over HSPA.*/
              if(rab_list_ptr->elem[rab_index].rb_InformationSetupList.n == 1)
              {
                /* Most likely network is trying to add third RB to switch from lower AMR rate to higher */
                voice_rab_already_exists = TRUE;
              }
              else
              {
                rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                WRRC_MSG0_HIGH("Invalid configuration .. more than one RAB subflow is being added");
                return;
              }
            }
            else
            {
              /* Whenever a new voice RAB is to be set up, at least two RBs need to be associated with that RAB.
              Therefore, first condition of "if" checks for presence of more than 1 RB. And the second condition
              of "if" ensures that it's for CS over DCH.*/
              if (rab_list_ptr->elem[rab_index].rb_InformationSetupList.n == 1)
              {
#ifdef FEATURE_SCUDIF_ENHANCEMENT
                #error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/

                WRRC_MSG0_HIGH("Not enough RBs mapped to voice RAB!");
                rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
            }
            }
            break;
  
          default:
            WRRC_MSG1_HIGH("CS RAB validation failed! RABid %d",rab_id);
            break;
  
        } /* end switch */
  
        if ( (rbe.status.failure_status == RRCRB_MSG_INVALID_CONFIG) ||
           (rbe.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG))
        {
          /* No need to continue */
          return;
        }

#ifdef FEATURE_VOC_AMR_WB
        if((rab_type == RRCMN_CS_VOICE_CALL )
#ifdef FEATURE_SCUDIF_ENHANCEMENT
#error code not present
#endif/*FEATURE_SCUDIF_ENHANCEMENT*/
         )
        {
          /* If the NAS Sync Indicator IE is present in the OTA msg then , extract it and update the RBE database */
          if(rab_list_ptr->elem[rab_index].rab_Info.m.nas_Synchronisation_IndicatorPresent)
          {
            nas_sync_ind =  rrc_translate_nas_sync_indicator(&(rab_list_ptr->elem[rab_index].rab_Info.nas_Synchronisation_Indicator));
            if( nas_sync_ind == MVS_MODE_NONE )
            {
              rbe.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
              WRRC_MSG1_HIGH("Unsupported  configuration .. codec not supported %d",rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator );
              return;
            }
          }
          rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator = nas_sync_ind;    
#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM   
          rrc_set_ordered_codec_status(TRUE);
#endif
        }
#endif /*FEATURE_VOC_AMR_WB*/

      }
      /* Else check for PS Domain */
      else if ( rab_list_ptr->elem[rab_index].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain )
      {
        /* Store the domain identity. This will be later added to
          ESTABLISHED_RABS */
        rbe.rabs.rabs_in_msg[rab_index].cn_domain = RRC_PS_DOMAIN_CN_ID;
  
        /* For PS Domain, if it's a loopback, then no need for query,
        otherwise, call RABM module to verify the RAB */
        if ( rrclbt_is_lb_on() == TRUE ||
             rabm_rabid_validate( RAB_ESTABLISH_QUERY, (uint8)rab_id )
             == TRUE )
        {
          MSG_LOW("PS RAB Id %d to be setup", rab_id, 0, 0);
        }
        else
        {
          WRRC_MSG1_ERROR("PS RAB validation failed! RABid %d",rab_id);
          rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
          return;
        }
      }
  
      if(voice_rab_already_exists == TRUE)
      {
        rbe.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_VOICE_RAB;
        WRRC_MSG0_HIGH("Updated RAB type as voice");
      }
      else
      {
        /* ASN1 structure 'rrc_RAB_InformationSetup' is validated at this point,
            hence update rab_type in structure 'rbe.rabs.rabs_in_msg[rab_index]'. */
        (void) rrc_update_rab_type_in_rab_info_r5(
           &(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.
                      u.r5.radioBearerSetup_r5.rab_InformationSetupList.elem[rab_index]),
           &(rbe.rabs.rabs_in_msg[rab_index]));
      }
  
      MSG_HIGH("RAB_ID:%d,Domain:%d,re_est_tmr:%d",
               rbe.rabs.rabs_in_msg[rab_index].rab_id,
               rbe.rabs.rabs_in_msg[rab_index].cn_domain,
               rbe.rabs.rabs_in_msg[rab_index].re_est_timer);
#ifdef FEATURE_VOC_AMR_WB
      /* set the ordered codec mode which is sent in the ordered config to MAC.
        * If the RBE msg is not for Voice RAB also then also the current codec value
        * is passed to MAC 
        */
      rrc_set_ordered_codec_mode(nas_sync_ind);
      rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator = nas_sync_ind;
#endif /*FEATURE_VOC_AMR_WB*/
  
      /* Go to next RAB info  */
      
    }
    if (rab_list_ptr->n > rab_index)
    {
      WRRC_MSG0_HIGH("Too many RABs in an RBSetupMessage");
      rbe.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
      return;    
    }
  }
  /* Store the number of the RAB info in the RBSetup message. */
  rbe.rabs.num_rabs_in_msg = rab_index;
  WRRC_MSG1_HIGH("%d RABs in an RBSetupMessage", rab_index);

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  rbe.trans_to_dch_ciph_config_needed = FALSE;
  if ((rbe.state_from_msg == RRC_STATE_CELL_DCH) && 
      (rrc_get_state() != RRC_STATE_CELL_DCH)
     )
  {
    /* check to see if dl-dpch-common info is present while going to dch */
    if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
		rrc_RadioBearerSetup_r5_IEs,dl_CommonInformation)) &&
        (RRC_MSG_COMMON_BITMASK_IE(rbsetup_ptr->dl_CommonInformation, 
         rrc_DL_CommonInformation_r5,dl_DPCH_InfoCommon)))
    {
      /* check if ciphering is active in mac for cs domain, if it is then,
        start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      */
      if (rrcsmc_tm_ciph_active() == TRUE)
      {
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        uecomdef_status_e_type loc_ret_val;
        WRRC_MSG0_HIGH("To-DCH-Step-1 config activated");
        loc_ret_val = rrcrb_get_mac_d_hfn(
             (RRC_MSG_COMMON_BITMASK_IE(rbsetup_ptr->dl_CommonInformation.dl_DPCH_InfoCommon,
             rrc_DL_DPCH_InfoCommon_r4,mac_d_HFN_initial_value)),
             &rbsetup_ptr->dl_CommonInformation.dl_DPCH_InfoCommon.mac_d_HFN_initial_value,
             &mac_d_hfn_l);
        if(loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("get_mac_d_hfn failed");
        }
        if (rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
        {
          WRRC_MSG0_HIGH("Processing Ciphering Config for HHO failed");
        }
        rbe.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }

  if(rbe.state_from_msg == RRC_STATE_CELL_DCH)
  {
    rbe.first_tm_rb_setup = rrcrbe_is_first_tm_rb_setup(new_tm_rb);
    WRRC_MSG1_HIGH("First TM RB being set status : %d",rbe.first_tm_rb_setup);
    if(rbe.first_tm_rb_setup == TRUE)
    {
      /* check to see if dl-dpch-common info is present while going to dch */
      if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
	  	 rrc_RadioBearerSetup_r5_IEs,dl_CommonInformation)) &&
          (RRC_MSG_COMMON_BITMASK_IE(rbsetup_ptr->dl_CommonInformation,
           rrc_DL_CommonInformation_r5,dl_DPCH_InfoCommon)))
      {
        /* check if ciphering is active in mac for cs domain, if it is then,
          start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
        */
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        uecomdef_status_e_type loc_ret_val;
        WRRC_MSG0_HIGH("First time TM RB Setup HHO Step-1 config activated");
        loc_ret_val = rrcrb_get_mac_d_hfn(
             (RRC_MSG_COMMON_BITMASK_IE(rbsetup_ptr->dl_CommonInformation.dl_DPCH_InfoCommon,
             rrc_DL_DPCH_InfoCommon_r4,mac_d_HFN_initial_value)),
             &rbsetup_ptr->dl_CommonInformation.dl_DPCH_InfoCommon.mac_d_HFN_initial_value,
             &mac_d_hfn_l);
        if(loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("get_mac_d_hfn failed");
        }
        if (rrcsmc_process_cipher_config_for_first_tm_setup(mac_d_hfn_l) == FAILURE)
        {
          WRRC_MSG0_HIGH("Processing Ciphering Config for First TM RB Setup failed");
          /*This step is reached only when the first step returns a failure,. i.e, if the ciphering is not active for CS domain.
              Hence setting rbe.first_tm_rb_setup to FALSE so that step 2 is not done*/
          rbe.first_tm_rb_setup = FALSE;
        }
      }
    }
  }
  /* If UTRAN DRX coefficient is not present then it is invalid configuration */
  if ((rbe.state_from_msg == RRC_STATE_CELL_PCH) ||
      (rbe.state_from_msg  == RRC_STATE_URA_PCH))
  {

    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
          rrc_RadioBearerSetup_r5_IEs,utran_DRX_CycleLengthCoeff)))
    {
      WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", rbe.state_from_msg);
      rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }
  if (rbe.state_from_msg == RRC_STATE_URA_PCH)
  {
    /*should we do it after getting l2ack for response??*/
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
          rrc_RadioBearerSetup_r5_IEs,ura_Identity)))
    {
      WRRC_MSG0_HIGH("URA ID not present");
      rrccu_clear_ura_id ();
    }
    else
    {
      /*URA ID present in the message set ura_identity variable with this value */
      if (rbsetup_ptr->ura_Identity.numbits == 16)
      {
        rrccu_update_ura_id (&(rbsetup_ptr->ura_Identity));
      }
      else
      {
        WRRC_MSG1_ERROR("Invalid URA ID length%d",
            rbsetup_ptr->ura_Identity.numbits);
        rbe.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
        rbe.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
        return;
      }
    }
  }


  return;

}/* end function validate_rbsetup_r5 */


/*===========================================================================

FUNCTION   VALIDATE_RBSETUP_R6

DESCRIPTION

  This function validates the received R6 RB Setup Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a Radio Bearer Setup Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_rbsetup_r6
(
rrc_RadioBearerSetup *msg_ptr      /* Pointer to the RB Setup message */
)
{
  uecomdef_status_e_type status;

  rrc_RadioBearerSetup_r6_IEs* rbsetup_ptr = NULL;

  rrc_RAB_InformationSetupList_r6 *rab_list_ptr = NULL;

  rrc_RAB_InformationReconfigList *rab_reconfig_list_ptr = NULL;

  rrc_rab_search_e_type rab_status; /* To check if RAB is present in ESTABLISHED_RABS */

  rrc_RB_InformationSetup_r6 *rb_info_ptr = NULL;

#ifdef FEATURE_UMTS_PDCP

   rrc_HeaderCompressionInfo_r4 *pdcpHeaderCompressionInfo = NULL;

#endif /* FEATURE_UMTS_PDCP*/

  rrc_rab_id_type rab_id;           /* To store RAB id in internal RRC format */
  rrcmn_rab_status_T rab_type = RRCMN_INVALID_RAB_ID;     /* To store the status returned by MN layer. This
                                    indicates what type of RAB is received. This is
                                    used only if the RAB is for the CS_domain */
  uint32 rab_index=0;               /* Used as an index when storing established
                                    RABs locally. Only when we have more than
                                    one RAB per message, this will be incremented */
  uint32 rb_index =0;
  boolean new_tm_rb = FALSE;
  rlc_lc_id_type local_lc_id;
  uecomdef_logch_mode_e_type rx_mode;
  rrclcm_status_e_type result;
  boolean dl_tm_flag;

  boolean voice_rab_already_exists = FALSE;
 #ifdef FEATURE_VOC_AMR_WB
  mvs_mode_type nas_sync_ind=rrc_get_current_codec_mode();
 #endif /*FEATURE_VOC_AMR_WB*/

  /* Assume the message is valid for now */
  rbe.status.failure_status = RRCRB_MSG_VALID;


  rbsetup_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
    u.r6.radioBearerSetup_r6;
  switch (rbsetup_ptr->rrc_StateIndicator)
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      rbe.state_from_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      rbe.state_from_msg = RRC_STATE_CELL_FACH;
      break;

    default: /* Just to satisfy LINT */
      WRRC_MSG0_ERROR("Invalid RRC state from Msg");
      rbe.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
      break;

  } /* End of switch */

  if ( rbe.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
    return;
  }

  /*Reject RB setup with default configuration */
  if (RRC_CHECK_COMMON_MSG_TYPE(rbsetup_ptr->specificationMode,
      rrc_RadioBearerSetup_r6_IEs_specificationMode_dummy))
  {
    WRRC_MSG0_ERROR("Default config not expected on rbsetup");
    rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
    return;
  }

  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,rrc_RadioBearerSetup_r6_IEs,dl_HSPDSCH_Information) )
  {
    /* Check if network does not try to initiate HSDPA with invalid state */
    status = rrc_hsdpa_initial_validation_r6(rbsetup_ptr->rrc_StateIndicator,
                                             &rbsetup_ptr->dl_HSPDSCH_Information);

    if (status == FAILURE)
    {
      rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }
 

  if (RRC_CHECK_COMMON_MSG_TYPE(rbsetup_ptr->specificationMode,
      rrc_RadioBearerSetup_r6_IEs_specificationMode_complete))
  {
    /* Check if there are new RABs to be established */

    if (rbsetup_ptr->specificationMode.u.complete.m.rab_InformationSetupListPresent)
    {
      rab_list_ptr = &rbsetup_ptr->specificationMode.u.complete.rab_InformationSetupList;
      rab_index =0;
      /* check for the unsupported PDCP info */
  
      while (rab_list_ptr->n > rab_index)  /* going through the RAB info setup list */
      {
        /* Check if the RAB id is GSM_MAP type. We don't support any
           other type */
        if ((rab_list_ptr->elem[rab_index].rab_Info.rab_Identity.t 
             != T_rrc_RAB_Identity_gsm_MAP_RAB_Identity))
        {
          WRRC_MSG1_HIGH("RAB ID type %d is not GSM_MAP!",rab_list_ptr->elem[rab_index].rab_Info.rab_Identity.t);
          rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
          return;
        }

        if(rab_list_ptr->n > (rab_index + 1))
        {
          if (rab_list_ptr->elem[rab_index].rab_Info.cn_DomainIdentity !=
              rab_list_ptr->elem[rab_index+1].rab_Info.cn_DomainIdentity)
          {
            WRRC_MSG2_ERROR("Different CN domains in rab-list %d %d",
                rab_list_ptr->elem[rab_index].rab_Info.cn_DomainIdentity,
                rab_list_ptr->elem[rab_index+1].rab_Info.cn_DomainIdentity
                );
            rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
            return;
          }
        }

        if (rab_list_ptr->elem[rab_index].rab_Info.m.mbms_SessionIdentityPresent)
        {
          WRRC_MSG0_HIGH("MBMS session Identity present. Not supported");
          rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
          return;
        }
 
        /* check for the unsupported PDCP info */
        
        rb_index =0;
        /* going through the RB info setup list */
        while (rab_list_ptr->elem[rab_index].rb_InformationSetupList.n > rb_index)
        {
          rb_info_ptr = &rab_list_ptr->elem[rab_index].rb_InformationSetupList.elem[rb_index];
          if (rrc_find_rb_in_est_rabs(rb_info_ptr->rb_Identity) == RRC_RB_PRESENT) 
          {
            WRRC_MSG1_HIGH("RB id %d already exist in est rabs: Invalid config", 
                     rb_info_ptr->rb_Identity);
            rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
            return;
          }
  
          if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_ptr,
              rrc_RB_InformationSetup_r6,pdcp_Info))
          {
  #ifdef FEATURE_UMTS_PDCP
            if (rrc_pdcp_enabled)
            {
              /* If the RAB is for cs domain, send RBS failure with cause
                 *   invalid configuration
                 */
              if (rab_list_ptr->elem[rab_index].rab_Info.cn_DomainIdentity == RRC_CS_DOMAIN_CN_ID)
              {
                WRRC_MSG0_HIGH("PDCP info invalid for CS rab");
                rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                return;
              }
              /* check whether lossless SRNS relocation flag is set
               * and header compression algo is present in the message */
              if ((RRC_MSG_COMMON_BITMASK_IE(rb_info_ptr->pdcp_Info,rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
                  (RRC_CHECK_COMMON_MSG_TYPE(rb_info_ptr->pdcp_Info.losslessSRNS_RelocSupport,rrc_LosslessSRNS_RelocSupport_supported)))
              {
                if (rb_info_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_absent)
                {
                  WRRC_MSG0_HIGH("PDCP pdu header is absent.  Invalid config");
                  rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                  return;
                }
  
              } /* if rrc_PDCP_Info_r4_losslessSRNS_RelocSupport_present */
              if (rb_info_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present)
              {
                if ((RRC_MSG_COMMON_BITMASK_IE(rb_info_ptr->pdcp_Info,rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
                    (RRC_CHECK_COMMON_MSG_TYPE(rb_info_ptr->pdcp_Info.losslessSRNS_RelocSupport,
                    rrc_LosslessSRNS_RelocSupport_notSupported)))
                {
                  if (!(RRC_MSG_COMMON_BITMASK_IE(rb_info_ptr->pdcp_Info,rrc_PDCP_Info_r4,headerCompressionInfoList)))
                  {
                    WRRC_MSG0_HIGH("PDCP header compress info list not present");
                    rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                    return;
                  }
                }
              }
  
              if (RRC_MSG_COMMON_BITMASK_IE(rb_info_ptr->pdcp_Info, rrc_PDCP_Info_r4,headerCompressionInfoList))
              {
                uint32 local_idx =0;
                while (rb_info_ptr->pdcp_Info.headerCompressionInfoList.n > local_idx)
                {
                  pdcpHeaderCompressionInfo = &rb_info_ptr->pdcp_Info.headerCompressionInfoList.elem[local_idx];
                  if (RRC_CHECK_COMMON_MSG_TYPE(pdcpHeaderCompressionInfo->algorithmSpecificInfo,
                      rrc_AlgorithmSpecificInfo_r4_rfc2507_Info))
                  {
                    /* if the UE capability "Maximum header compression context space", is exceeded with this configuration send
                     * failure with cause "Invalid Config"
                     */
                    if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER > IPHC_MAX_HC_CONTEXT_SPACE)
                    {
                      WRRC_MSG1_HIGH("Invalid config.  max header %d is invalid",
                               pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER);
                      rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                      return;
                    }
                  }
                  else if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.t ==
                           T_rrc_AlgorithmSpecificInfo_r4_rfc3095_Info)
                  {
                    /* 	the chosen MAX_CID shall not be greater than the value "Maximum number of ROHC context sessions" as 
                     *  indicated in the IE "PDCP Capability"
                     */
                    if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info,ul_RFC3095))
                    {
                      if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->ul_RFC3095.max_CID > ROHC_MAX_CONTEXT_SESSIONS)
                      {
                        WRRC_MSG1_HIGH("Invalid config.  MAX_CID %d is invalid",
                                 pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->ul_RFC3095.max_CID);
                        rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                        return;
                      }
                    }
  
                    if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info,dl_RFC3095))
                    {
                      if(pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->dl_RFC3095.max_CID > ROHC_MAX_CONTEXT_SESSIONS)
                      {
                        WRRC_MSG1_HIGH("Invalid config.  MAX_CID %d is invalid",
                                 pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->dl_RFC3095.max_CID);
                        rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                        return;
                      }
                    }
                  }  /* if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.t == rrc_rfc3095_Info)*/
                  local_idx++;
                } 
              } /*  if (rb_info_ptr->pdcp_Info.bit_mask & rrc_PDCP_Info_r4_headerCompressionInfoList_present) */
            }
  #endif
          } /* if (rb_info_ptr->bit_mask & rrc_RB_InformationSetup_r6_pdcp_Info_present) */
  

          /* Reject if MBMS info is included */
          if ((rab_list_ptr->elem[rab_index].rab_Info.m.mbms_SessionIdentityPresent))
          {
            WRRC_MSG0_HIGH("MBMS session Identity present. Not supported");
            rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
            return;
          }

          if(new_tm_rb == FALSE)
          {
            dl_tm_flag = FALSE;
            if((RRC_CHECK_COMMON_MSG_TYPE(rb_info_ptr->rlc_InfoChoice,rrc_RLC_InfoChoice_r6_rlc_Info)) &&
              (((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_ptr->rlc_InfoChoice.u.rlc_Info,
                 rrc_RLC_Info_r6,ul_RLC_Mode))&&
                 (rb_info_ptr->rlc_InfoChoice.u.rlc_Info->ul_RLC_Mode.t == 
                 T_rrc_UL_RLC_Mode_ul_TM_RLC_Mode))) &&
                 ((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r6,dl_RLC_Mode))&&
                 (RRC_CHECK_COMMON_MSG_TYPE(rb_info_ptr->rlc_InfoChoice.u.rlc_Info->dl_RLC_Mode,
                 rrc_DL_RLC_Mode_r6_dl_TM_RLC_Mode))))
            {
              new_tm_rb = TRUE;
            }
            else if(RRC_CHECK_COMMON_MSG_TYPE(rb_info_ptr->rlc_InfoChoice,rrc_RLC_InfoChoice_r6_same_as_RB))
            {
              local_lc_id = rrclcm_get_dl_rlc_lc_id(rb_info_ptr->rlc_InfoChoice.u.same_as_RB);
              if(local_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
              {
                result = rrclcm_get_dl_rlc_mode(local_lc_id, &rx_mode);
                if(result == RRCLCM_SUCCESS && rx_mode == UE_MODE_TRANSPARENT)
                {
                  dl_tm_flag = TRUE;
                }
              }
    
              local_lc_id = rrclcm_get_ul_rlc_lc_id(rb_info_ptr->rlc_InfoChoice.u.same_as_RB);
              if(local_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
              {
                result = rrclcm_get_ul_rlc_mode(local_lc_id, &rx_mode);
                if(dl_tm_flag == TRUE && result == RRCLCM_SUCCESS && rx_mode == UE_MODE_TRANSPARENT)
                {
                  new_tm_rb = TRUE;
                }
              }
            }
          }
          /*point to the next link */
          rb_index++;
        }
        /* Go to next RAB setup infomation  */
        rab_index++;
      } 
  
    }
    else
    {
      /* There is no RAB to be setup in this message.
        No further validation is needed */
      rbe.rabs.num_rabs_in_msg = 0;
    }
    if(new_tm_rb == TRUE)
    {
      WRRC_MSG0_HIGH("One or more TM RBs being setup");
    }
  
    if (rbsetup_ptr->specificationMode.u.complete.m.dl_CounterSynchronisationInfoPresent)
    {
      /*check whether the SRNS relocation is within Cell_DCH or not.
        SRNS relocation information can come in reconfiguration messages
        only when the state transition is Cell_DCH->Cell_DCH */
      if (!((rbe.state_from_msg == RRC_STATE_CELL_DCH) &&
            ( rrc_get_state() == RRC_STATE_CELL_DCH)))
      {
        WRRC_MSG2_ERROR("SRNS reloc with invalid states cur_state %d, msg_state %d",
            rrc_get_state(), rbe.state_from_msg);
        rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
        return;
      }
     /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has
       PDCP info list. If it is true then this configuration is unsupported */
  
      if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2(rbsetup_ptr->specificationMode.u.complete.dl_CounterSynchronisationInfo,
           rb_WithPDCP_InfoList)) ||
          (RRC_MSG_COMMON_BITMASK_IE(rbsetup_ptr->specificationMode.u.complete.dl_CounterSynchronisationInfo,
           rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
      {
        WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
        rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
        return;
      }
      else
      {
        /* The procedure involves Lossy SRNS relocation.
        Set the flag that indicates that this procedure
        involves SRNS relocation */
        rbe.rbe_for_srns_relocation = TRUE;
        /* store if new U-RNTI is present in the message */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
           rrc_RadioBearerSetup_r6_IEs,new_U_RNTI))
        {
          rbe.new_urnti_valid = TRUE;
          rrc_translate_urnti (&(rbsetup_ptr->new_U_RNTI),
                               &rbe.new_urnti);
        }
      }
    }
  
    /* Process RAB reconfig before RAB setup because we want to catch NW's which
       are trying to setup and reconfig RAB with the same message */
    if (rbsetup_ptr->specificationMode.u.complete.m.rab_InformationReconfigListPresent)
    {
      MSG_LOW("RAB Info Reconfig in R6 RBS Msg", 0, 0, 0);
  
      rab_reconfig_list_ptr = &rbsetup_ptr->specificationMode.u.complete.rab_InformationReconfigList;
  
      for (rab_index = 0;(rab_reconfig_list_ptr->n > rab_index) && (rab_index < RRCRB_MAX_RAB_PER_MSG); rab_index++)
      {
        /* Check if the RAB id is GSM_MAP type. We don't support any
           other type */
        if (rab_reconfig_list_ptr->elem[rab_index].rab_Identity.t != T_rrc_RAB_Identity_gsm_MAP_RAB_Identity)
        {
          WRRC_MSG1_HIGH("RABid t%d is not GSM_MAP!", rab_reconfig_list_ptr->elem[rab_index].rab_Identity.t);
          rbe.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
          return;
        }
  
        /* GSM_MAP RAB Id. Check if it's valid by calling the appropriate NAS entity. 
           First convert it to internal format. */
        rab_id = rrcrb_translate_gsm_map_rab_id((rab_reconfig_list_ptr->elem[rab_index].rab_Identity.u.gsm_MAP_RAB_Identity));
  
        /* Check if the RAB id is already in ESTABLISHED_RABS */
        rab_status = rrc_find_rab_in_established_rabs(rab_id);
        if ( rab_status != RRC_RAB_FOUND )
        {
          WRRC_MSG1_HIGH("RAB ID 0X%x not in ESTABLISHED_RABS", rab_id);
          rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
          return;
        }
  
        /* Store this info locally so that ESTABLISHED_RABS can
           be updated later, when these RABs are established */
        rbe.rabs.rabs_in_msg[rab_index].rab_id_is_gsm_map = TRUE;
        rbe.rabs.rabs_in_msg[rab_index].rab_id = rab_id;
  
        /* Check if this RAB is CS Domain or PS Domain */
        if ( rab_reconfig_list_ptr->elem[rab_index].cn_DomainIdentity  == rrc_CN_DomainIdentity_cs_domain )
        {
          /* Store the domain identity. This will be later added to
          ESTABLISHED_RABS */
          rbe.rabs.rabs_in_msg[rab_index].cn_domain = RRC_CS_DOMAIN_CN_ID;
  
          /* Check if it is in loopback test */
          if ( rrclbt_is_lb_on() == TRUE )
          {
            /* Set Rab type to Data */
            MSG_LOW("Loopback Test", 0, 0, 0);
            rbe.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_DATA_RAB;
          }
          else
          {
            /* For CS RABs, Call MN layer to verify this RAB */
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
            rab_type = rrcmn_verify_rab_subs( (radio_access_bearer_T)rab_id ,rrc_get_as_id());
#else
            rab_type = rrcmn_verify_rab( (radio_access_bearer_T)rab_id );
#endif

            MSG_HIGH("For RAB-Id %d, RAB type from NAS = %d, RAB type from EST_RABs = %d",rab_id, rab_type,
                rrc_get_rab_type_for_given_rab_id(rbe.rabs.rabs_in_msg[rab_index].rab_id));  

            switch ( rab_type )
            {
              case RRCMN_CS_DATA_CALL:
                rbe.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_DATA_RAB;
                break;
  
              case RRCMN_CS_VOICE_CALL:
                rbe.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_VOICE_RAB;
                break;
  
              default:
                WRRC_MSG0_ERROR("CS RAB validation failed!");
                rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
                break;
  
            } /* end switch */
          }
  
          if ( (rbe.status.failure_status == RRCRB_MSG_INVALID_CONFIG) ||
               (rbe.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG)
             )
          {
            /* No need to continue */
            return;
          }

#ifdef FEATURE_VOC_AMR_WB
          if((rab_type == RRCMN_CS_VOICE_CALL )
#ifdef FEATURE_SCUDIF_ENHANCEMENT
#error code not present
#endif/*FEATURE_SCUDIF_ENHANCEMENT*/
           )
          {
            nas_sync_ind =   rrc_translate_nas_sync_indicator(&(rab_reconfig_list_ptr->elem[rab_index].nas_Synchronisation_Indicator));
            if( nas_sync_ind == MVS_MODE_NONE )
            {
              rbe.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
              return;
            }
            rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator = nas_sync_ind;             
#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
            rrc_set_ordered_codec_status(TRUE);
#endif
          }
#endif/*FEATURE_VOC_AMR_WB*/

        }
        /* Else check for PS Domain */
        else if ( rab_reconfig_list_ptr->elem[rab_index].cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain )
        {
          /* Store the domain identity. This will be later added to
             ESTABLISHED_RABS 
           */
          rbe.rabs.rabs_in_msg[rab_index].cn_domain = RRC_PS_DOMAIN_CN_ID;
          WRRC_MSG1_HIGH("PS RABId %d to be setup", rab_id);
          rbe.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_PS_DATA_RAB;
        }
  #ifdef FEATURE_VOC_AMR_WB
       /* set the ordered codec mode which is sent in the ordered config to MAC.
        * If the RBE msg is not for Voice RAB also then also the current codec value
        * is passed to MAC 
        */
        rrc_set_ordered_codec_mode(nas_sync_ind);
        rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator = nas_sync_ind;
  #endif /*FEATURE_VOC_AMR_WB*/
  
        
      } // for each RAB info
      rbe.rabs.num_rabs_in_msg = rab_index;
      WRRC_MSG1_HIGH("Total Reconfig RABs %d",rbe.rabs.num_rabs_in_msg);  
    }

    if (rbsetup_ptr->specificationMode.u.complete.m.rab_InformationSetupListPresent)	
    {
      uint8 rab_list_index = 0;
      /* GSM_MAP RAB Id. Check if it's valid by calling the
      appropriate NAS entity. First convert it to internal
      format. */
      rab_list_ptr = &rbsetup_ptr->specificationMode.u.complete.rab_InformationSetupList;
    
      for (rab_index=rbe.rabs.num_rabs_in_msg;(rab_list_ptr->n > rab_list_index) && (rab_index < RRCRB_MAX_RAB_PER_MSG) 
                                               && (rab_list_index < RRCRB_MAX_RAB_PER_MSG); rab_index++,rab_list_index++)
      {
        rab_id = rrcrb_translate_gsm_map_rab_id((rab_list_ptr->elem[rab_list_index].rab_Info.
                                                  rab_Identity.u.gsm_MAP_RAB_Identity));
    
        /* Store this info locally so that ESTABLISHED_RABS can
        be updated later, when these RABs are established */
        rbe.rabs.rabs_in_msg[rab_index].rab_id_is_gsm_map = TRUE;
        rbe.rabs.rabs_in_msg[rab_index].rab_id = rab_id;
        rbe.rabs.rabs_in_msg[rab_index].re_est_timer =
        rab_list_ptr->elem[rab_list_index].rab_Info.re_EstablishmentTimer;
        /* Check if this RAB is CS Domain or PS Domain */
        if ( rab_list_ptr->elem[rab_list_index].rab_Info.cn_DomainIdentity == RRC_CS_DOMAIN_CN_ID )
        {
          /* Store the domain identity. This will be later added to
            ESTABLISHED_RABS */
          rbe.rabs.rabs_in_msg[rab_index].cn_domain = RRC_CS_DOMAIN_CN_ID;
    
          /* For CS RABs, Call MN layer to verify this RAB */
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
          rab_type = rrcmn_verify_rab_subs( (radio_access_bearer_T)rab_id ,rrc_get_as_id());
#else
          rab_type = rrcmn_verify_rab( (radio_access_bearer_T)rab_id );
#endif
          MSG_HIGH("For RAB-Id %d, RAB type from NAS = %d, RAB type from EST_RABs = %d",rab_id, rab_type,
                rrc_get_rab_type_for_given_rab_id(rbe.rabs.rabs_in_msg[rab_index].rab_id));  
    
          switch ( rab_type )
          {
            case RRCMN_CS_DATA_CALL:
    
              MSG_LOW("Received a CS Data RAB Id %d", rab_id, 0, 0);
              break;
    
            case RRCMN_CS_VOICE_CALL:
    
              MSG_LOW("Received a CS Voice RAB Id %d", rab_id, 0, 0);
                if (rrc_find_if_voice_rab_exists_in_established_rabs( rbe.rabs.rabs_in_msg[rab_index].rab_id )
                    == TRUE)
                {
                  WRRC_MSG1_HIGH("Voice RAB Id - %d already exists", rbe.rabs.rabs_in_msg[rab_index].rab_id );
                  /*First condition of "if" covers the case when third RB is being added for AMR call*/
                  /*Second condition of "if" covers the case when previously established Voice Call was of type CS over HSPA.*/
                  if(rab_list_ptr->elem[rab_list_index].rb_InformationSetupList.n == 1)
                  {
                    /* Most likely network is trying to add third RB to switch from lower AMR rate to higher */
                    voice_rab_already_exists = TRUE;
                  }
                  else
                  {
                    rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                    WRRC_MSG0_HIGH("Invalid configuration .. more than one RAB subflow is being added");
                    return;
                  }
                }
                /*The given RAB isn't an existing Voice RAB.*/
                else
                {
                  /* Whenever a new voice RAB is to be set up, at least two RBs need to be associated with that RAB.
                  Therefore, first condition of "if" checks for presence of more than 1 RB. And the second condition
                  of "if" ensures that it's for CS over DCH.*/
                  if (rab_list_ptr->elem[rab_list_index].rb_InformationSetupList.n == 1)
                  {
#ifdef FEATURE_SCUDIF_ENHANCEMENT
                    #error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/

                    WRRC_MSG0_HIGH("Not enough RBs mapped to voice RAB!");
                    rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
                  }
                }
              break;
    
            default:
              WRRC_MSG1_ERROR("CS RAB validation failed! RABid %d",rab_id);
              break;
    
          } /* end switch */
    
          if ( (rbe.status.failure_status == RRCRB_MSG_INVALID_CONFIG) ||
             (rbe.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG))
          {
            /* No need to continue */
            return;
          }

#ifdef FEATURE_VOC_AMR_WB
          if((rab_type == RRCMN_CS_VOICE_CALL )
#ifdef FEATURE_SCUDIF_ENHANCEMENT
#error code not present
#endif/*FEATURE_SCUDIF_ENHANCEMENT*/
           )
          {
            if(rab_list_ptr->elem[rab_list_index].rab_Info.m.nas_Synchronisation_IndicatorPresent)
            {
              nas_sync_ind =  rrc_translate_nas_sync_indicator(
                                     &(rab_list_ptr->elem[rab_list_index].rab_Info.nas_Synchronisation_Indicator));
              if( nas_sync_ind == MVS_MODE_NONE )
              {
                rbe.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
                WRRC_MSG1_HIGH("Unsupported configuration .. codec not supported %d",rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator );
                return;
              }          
            }
            rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator =nas_sync_ind;        
#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
            rrc_set_ordered_codec_status(TRUE);
#endif
          }
#endif /*FEATURE_VOC_AMR_WB*/

        }
        /* Else check for PS Domain */
        else if ( rab_list_ptr->elem[rab_list_index].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain )
        {
          /* Store the domain identity. This will be later added to
            ESTABLISHED_RABS */
          rbe.rabs.rabs_in_msg[rab_index].cn_domain = RRC_PS_DOMAIN_CN_ID;
    
          /* For PS Domain, if it's a loopback, then no need for query,
          otherwise, call RABM module to verify the RAB */
          if ( rrclbt_is_lb_on() == TRUE ||
               rabm_rabid_validate( RAB_ESTABLISH_QUERY, (uint8)rab_id ) == TRUE )
          {
            MSG_LOW("PS RAB Id %d to be setup", rab_id, 0, 0);
          }
          else
          {
            WRRC_MSG1_ERROR("PS RAB validation failed! RABid %d",rab_id);
            rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
            return;
          }
        }
    
        if(voice_rab_already_exists == TRUE)
        {
          rbe.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_VOICE_RAB;
          WRRC_MSG0_HIGH("Updated RAB type as voice");
        }
        else
        {
          /* ASN1 structure 'rrc_RAB_InformationSetup' is validated at this point,
              hence update rab_type in structure 'rbe.rabs.rabs_in_msg[rab_index]'. */
              /* change argument */
          (void) rrc_update_rab_type_in_rab_info_r6(
             &(rbsetup_ptr->specificationMode.u.complete.rab_InformationSetupList.elem[rab_list_index]),
             &(rbe.rabs.rabs_in_msg[rab_index]));
        }
    
        MSG_HIGH("RAB_ID:%d,Domain:%d,re_est_tmr:%d",
                 rbe.rabs.rabs_in_msg[rab_index].rab_id,
                 rbe.rabs.rabs_in_msg[rab_index].cn_domain,
                 rbe.rabs.rabs_in_msg[rab_index].re_est_timer);
#ifdef FEATURE_VOC_AMR_WB
        /* set the ordered codec mode which is sent in the ordered config to MAC.
          * If the RBE msg is not for Voice RAB also then also the current codec value
          * is passed to MAC 
          */
        rrc_set_ordered_codec_mode(nas_sync_ind);
        rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator = nas_sync_ind;
#endif /*FEATURE_VOC_AMR_WB*/
    
        /* Go to next RAB info  */
       
      }
      if (rab_list_ptr->n > rab_index)
      {
        WRRC_MSG0_HIGH("Too many RABs in an RBSetupMessage");
        rbe.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
        return;  
      }
    }
    rbe.rabs.num_rabs_in_msg = rab_index;
  }

  /* Store the number of the RAB info in the RBSetup message. */
  WRRC_MSG1_MED("%d RABs  in an RBSetupMessage",  rbe.rabs.num_rabs_in_msg);

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
  */
  rbe.trans_to_dch_ciph_config_needed = FALSE;
  if((rbe.state_from_msg == RRC_STATE_CELL_DCH) && 
     (rrc_get_state() != RRC_STATE_CELL_DCH)
    )
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
       rrc_RadioBearerSetup_r6_IEs,dl_CommonInformation)) && 
       (RRC_MSG_COMMON_BITMASK_IE(rbsetup_ptr->dl_CommonInformation,rrc_DL_CommonInformation_r6,
       dl_dpchInfoCommon)) &&
       (T_rrc_DL_CommonInformation_r6_dl_dpchInfoCommon_dl_DPCH_InfoCommon == 
         rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.t))
    {
      //check if ciphering is active in mac for cs domain, if it is then,
      //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      if(rrcsmc_tm_ciph_active() == TRUE)
      {
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        uecomdef_status_e_type loc_ret_val;
        WRRC_MSG0_HIGH("To-DCH-Step-1 config activated");
        loc_ret_val = rrcrb_get_mac_d_hfn(
             ( RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon,rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
              &rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
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
        rbe.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }

  if(rbe.state_from_msg == RRC_STATE_CELL_DCH)
  {
    rbe.first_tm_rb_setup = rrcrbe_is_first_tm_rb_setup(new_tm_rb);
    if(new_tm_rb)
    {
      WRRC_MSG1_HIGH("First TM RB being set status : %d",rbe.first_tm_rb_setup);
    }
    if(rbe.first_tm_rb_setup == TRUE)
    {
      //check to see if dl-dpch-common info is present while going to dch
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
         rrc_RadioBearerSetup_r6_IEs,dl_CommonInformation)) && 
         (RRC_MSG_COMMON_BITMASK_IE(rbsetup_ptr->dl_CommonInformation,
         rrc_DL_CommonInformation_r6,dl_dpchInfoCommon)) &&
         (T_rrc_DL_CommonInformation_r6_dl_dpchInfoCommon_dl_DPCH_InfoCommon == 
          rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.t))
      {
        uecomdef_status_e_type loc_ret_val;
        //check if ciphering is active in mac for cs domain, if it is then,
        //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        
        WRRC_MSG0_HIGH("First time TM RB Setup HHO Step-1 config activated");
        loc_ret_val = rrcrb_get_mac_d_hfn(
             ( RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon,rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
              &rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
              &mac_d_hfn_l);
        if (loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("get_mac_d_hfn failed");
        }
        if(rrcsmc_process_cipher_config_for_first_tm_setup(mac_d_hfn_l) == FAILURE)
        {
          WRRC_MSG0_HIGH("SMC process ciphering config for First TM RB Setup failed");
          /*This step is reached only when the first step returns a failure,. i.e, if the ciphering is not active for CS domain.
            Hence setting rbe.first_tm_rb_setup to FALSE so that step 2 is not done*/
            rbe.first_tm_rb_setup = FALSE;
        }

      }
    }
  }

  /* If UTRAN DRX coefficient is not present then it is invalid configuration */
  if ((rbe.state_from_msg == RRC_STATE_CELL_PCH) ||
      (rbe.state_from_msg  == RRC_STATE_URA_PCH))
  {

    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
         rrc_RadioBearerSetup_r6_IEs,utran_DRX_CycleLengthCoeff)))
    {
      WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", rbe.state_from_msg);
      rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }

  if((RRC_CHECK_COMMON_MSG_TYPE(rbsetup_ptr->specificationMode,
  	rrc_RadioBearerSetup_r6_IEs_specificationMode_complete))
      &&
      (rbsetup_ptr->specificationMode.u.complete.m.rb_InformationReconfigListPresent))
  {
    /* Save the linked list head pointer */
    if (FAILURE == rrcllc_validate_rb_info_reconfig_list_r6(
        &rbsetup_ptr->specificationMode.u.complete.rb_InformationReconfigList))
    {
      rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }

  if (rbe.state_from_msg == RRC_STATE_URA_PCH)
  {
    /*should we do it after getting l2ack for response??*/
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
         rrc_RadioBearerSetup_r6_IEs,ura_Identity)))
    {
      WRRC_MSG0_HIGH("URA ID not present");
      rrccu_clear_ura_id ();
    }
    else
    {
      /*URA ID present in the message set ura_identity variable with this value */
      if (rbsetup_ptr->ura_Identity.numbits == 16)
      {
        rrccu_update_ura_id (&(rbsetup_ptr->ura_Identity));
      }
      else
      {
        WRRC_MSG1_ERROR("Invalid URA ID length%d",
            rbsetup_ptr->ura_Identity.numbits);
        rbe.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
        rbe.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
        return;
      }
    }
  }


  return;

}/* end function validate_rbsetup_r6 */


/*===========================================================================

FUNCTION   VALIDATE_RBSETUP_MESSAGE

DESCRIPTION

  This function validates the received RB Setup Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a Radio Bearer Setup Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_rbsetup_message
(
rrc_RadioBearerSetup *msg_ptr      /* Pointer to the RB Setup message */
)
{
  rrc_rab_id_type rab_id;           /* To store RAB id in internal RRC format */
  rrcmn_rab_status_T rab_type = RRCMN_INVALID_RAB_ID;      /* To store the status returned by MN layer. This
                                    indicates what type of RAB is received. This is 
                                    used only if the RAB is for the CS_domain */
  uint32 rab_index=0;               /* Used as an index when storing established
                                    RABs locally. Only when we have more than
                                    one RAB per message, this will be incremented */
  rrc_RB_InformationSetup  *rb_infosetup_ptr;
  rrc_RAB_InformationSetupList *rab_info_ptr;
  uint32 rb_index=0;
  boolean voice_rab_already_exists = FALSE;
#ifdef FEATURE_UMTS_PDCP
   rrc_HeaderCompressionInfo *pdcpHeaderCompressionInfo = NULL;
#endif /* FEATURE_UMTS_PDCP*/

#ifdef FEATURE_VOC_AMR_WB
  mvs_mode_type nas_sync_ind=rrc_get_current_codec_mode();
#endif /*FEATURE_VOC_AMR_WB*/

  boolean new_tm_rb = FALSE;
  rlc_lc_id_type local_lc_id;
  uecomdef_logch_mode_e_type rx_mode;
  rrclcm_status_e_type result;
  boolean dl_tm_flag;

  /* Assume the message is valid for now */
  rbe.status.failure_status = RRCRB_MSG_VALID;

  if ( msg_ptr->t == T_rrc_RadioBearerSetup_later_than_r3 )
  {
    /* Critical extensions are present that are not supported by this
    release. We have to send a Radio Bearer Setup Failure message. */
    WRRC_MSG0_HIGH("Critical exstension present in RBS msg!");
    rbe.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
    rbe.status.prot_err_cause = rrc_ProtocolErrorCause_messageExtensionNotComprehended;
    return;
  }

  /* Check the RRC State indicatior given by the message */
  switch ( msg_ptr->u.r3.radioBearerSetup_r3.rrc_StateIndicator )
  {
  case rrc_RRC_StateIndicator_cell_DCH:

    rbe.state_from_msg = RRC_STATE_CELL_DCH;
    break;

  case rrc_RRC_StateIndicator_cell_FACH:

    rbe.state_from_msg = RRC_STATE_CELL_FACH;
    break;

  default:

    /* No other states are supported right now. */
    WRRC_MSG0_ERROR("Unsupported state in RB Setup Message");
    rbe.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
    break;
  }

  if ( rbe.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
    return;
  }

  /* Check if there are new RABs to be established */
  if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
  	rrc_RadioBearerSetup_r3_IEs,rab_InformationSetupList)
     )
  {
    MSG_LOW("RAB Info present in RBS Message", 0, 0, 0);

    rab_info_ptr = &msg_ptr->u.r3.radioBearerSetup_r3.rab_InformationSetupList;
    rab_index =0;
    /* check for the unsupported PDCP info */
    /* going through the RAB info setup list */
    while (rab_info_ptr->n > rab_index) 
    {
      /* Check if the RAB id is GSM_MAP type. We don't support any
      other type */
      if ( rab_info_ptr->elem[rab_index].rab_Info.rab_Identity.t
           != T_rrc_RAB_Identity_gsm_MAP_RAB_Identity
         )
      {
        WRRC_MSG1_HIGH("RAB ID type %d is not GSM_MAP!",rab_info_ptr->elem[rab_index].rab_Info.rab_Identity.t);
        rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
        return;
      }
    
     rb_index =0;
	  
      do  /* going through the RB info setup list */
      {
       rb_infosetup_ptr = &rab_info_ptr->elem[rab_index].rb_InformationSetupList.elem[rb_index];
        if (rrc_find_rb_in_est_rabs(rb_infosetup_ptr->rb_Identity) == RRC_RB_PRESENT) 
        {
          WRRC_MSG1_HIGH("RB id %d already exist in est rabs: Invalid config", 
                   rb_infosetup_ptr->rb_Identity);
          rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          return;
        }
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_infosetup_ptr,
            rrc_RB_InformationSetup,pdcp_Info))
        {
#ifdef FEATURE_UMTS_PDCP
          if (rrc_pdcp_enabled)
          {
             /* If the RAB is for cs domain, send RBS failure with cause
              * invalid configuration
              */
            if (rab_info_ptr->elem[rab_index].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain)
            {
              WRRC_MSG0_HIGH("PDCP info invalid for CS rab");
              rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
              return;
            }

            /* check whether lossless SRNS relocation flag is set
             * and header compression algo is present in the message */
            if ((RRC_MSG_COMMON_BITMASK_IE(rb_infosetup_ptr->pdcp_Info,
				rrc_PDCP_Info,losslessSRNS_RelocSupport)) &&
                (RRC_CHECK_COMMON_MSG_TYPE(rb_infosetup_ptr->pdcp_Info.losslessSRNS_RelocSupport,
                rrc_LosslessSRNS_RelocSupport_supported)))
            {
              if (rb_infosetup_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_absent)
              {
                WRRC_MSG0_HIGH("PDCP pdu header is absent.  Invalid config");
                rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                return;
              }
            } /* if rrc_PDCP_Info_losslessSRNS_RelocSupport_present */

            if (rb_infosetup_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present)
            {
              if ((RRC_MSG_COMMON_BITMASK_IE(rb_infosetup_ptr->pdcp_Info,rrc_PDCP_Info,losslessSRNS_RelocSupport)) &&
                  (RRC_CHECK_COMMON_MSG_TYPE(rb_infosetup_ptr->pdcp_Info.losslessSRNS_RelocSupport,rrc_LosslessSRNS_RelocSupport_notSupported)))
              {
                if (!(RRC_MSG_COMMON_BITMASK_IE(rb_infosetup_ptr->pdcp_Info,rrc_PDCP_Info,headerCompressionInfoList)))
                {
                  WRRC_MSG0_HIGH("PDCP header compress info list not present.  Invalid Config");
                  rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                  return;
                }
              }
            }

            if (RRC_MSG_COMMON_BITMASK_IE(rb_infosetup_ptr->pdcp_Info,rrc_PDCP_Info,headerCompressionInfoList))
            {
            uint32 local_idx=0;
              
              while (rb_infosetup_ptr->pdcp_Info.headerCompressionInfoList.n > local_idx)
              {
                pdcpHeaderCompressionInfo = &rb_infosetup_ptr->pdcp_Info.headerCompressionInfoList.elem[local_idx];
                if (RRC_CHECK_COMMON_MSG_TYPE(pdcpHeaderCompressionInfo->algorithmSpecificInfo,
                    rrc_AlgorithmSpecificInfo_rfc2507_Info))
                {
                  /* if the UE capability "Maximum header compression context space", is exceeded with this configuration send
                   * failure with cause "Invalid Config"
                   */
                  if(pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER > IPHC_MAX_HC_CONTEXT_SPACE)
                  {
                    WRRC_MSG1_HIGH("Invalid config.  max header %d is invalid",
                             pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER);
                    rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                    return;
                  }
                }
                local_idx++;
              } /*  while (pdcpHeaderCompressionInfoList) */

            } /*  if (rb_infosetup_ptr->pdcp_Info.bit_mask & rrc_PDCP_Info_headerCompressionInfoList_present) */
          }
#endif

        } /* if pdcp info is present */
        if(new_tm_rb == FALSE)
        {
          dl_tm_flag = FALSE;
          if((RRC_CHECK_COMMON_MSG_TYPE(rb_infosetup_ptr->rlc_InfoChoice,rrc_RLC_InfoChoice_rlc_Info)) &&
          (((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_infosetup_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info,ul_RLC_Mode))&&
               (rb_infosetup_ptr->rlc_InfoChoice.u.rlc_Info->ul_RLC_Mode.t == T_rrc_UL_RLC_Mode_ul_TM_RLC_Mode))
               &&
               ((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_infosetup_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info,dl_RLC_Mode))&&
               (RRC_CHECK_COMMON_MSG_TYPE(rb_infosetup_ptr->rlc_InfoChoice.u.rlc_Info->dl_RLC_Mode,
               rrc_DL_RLC_Mode_dl_TM_RLC_Mode)))))
          {
            new_tm_rb = TRUE;
          }
          else if(RRC_CHECK_COMMON_MSG_TYPE(rb_infosetup_ptr->rlc_InfoChoice,rrc_RLC_InfoChoice_same_as_RB))
          {
            local_lc_id = rrclcm_get_dl_rlc_lc_id(rb_infosetup_ptr->rlc_InfoChoice.u.same_as_RB);
            if(local_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
            {
              result = rrclcm_get_dl_rlc_mode(local_lc_id, &rx_mode);
              if(result == RRCLCM_SUCCESS && rx_mode == UE_MODE_TRANSPARENT)
              {
                dl_tm_flag = TRUE;
              }
            }
  
            local_lc_id = rrclcm_get_ul_rlc_lc_id(rb_infosetup_ptr->rlc_InfoChoice.u.same_as_RB);
            if(local_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
            {
              result = rrclcm_get_ul_rlc_mode(local_lc_id, &rx_mode);
              if(dl_tm_flag == TRUE && result == RRCLCM_SUCCESS && rx_mode == UE_MODE_TRANSPARENT)
              {
                new_tm_rb = TRUE;
              }
            }
          }
        }
        /*point to the next link */
        rb_index++;
      } while (rab_info_ptr->elem[rab_index].rb_InformationSetupList.n > rb_index);
      /* Go to next RAB setup infomation  */
      rab_index++;
    } //while (rab_info_ptr);

  }
  else
  {
    /* There is no RAB to be setup in this message.
    No further validation is needed */
    rbe.rabs.num_rabs_in_msg = 0;
  }
  if(new_tm_rb == TRUE)
  {
    WRRC_MSG0_HIGH("One or more TM RBs being setup");
  }

  /*check for the SRNS relocation info */
  if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
      rrc_RadioBearerSetup_r3_IEs,dl_CounterSynchronisationInfo))
  {
    /*check whether the SRNS relocation is within Cell_DCH or not.
     SRNS relocation information can come in reconfiguration messages
     only when the state transition is Cell_DCH->Cell_DCH */
    if (!((rbe.state_from_msg == RRC_STATE_CELL_DCH) &&
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("SRNS reloc with invalid states cur_state %d, msg_state %d",
             rrc_get_state(), rbe.state_from_msg);
      rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has
    PDCP info list. If it is true then this configuration is unsupported */
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3.dl_CounterSynchronisationInfo,
        rrc_DL_CounterSynchronisationInfo,rB_WithPDCP_InfoList))
    {
      WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
      rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation.
        Set the flag that indicates that this procedure
      involves SRNS relocation */
      rbe.rbe_for_srns_relocation = TRUE;
      /* store if new U-RNTI is present in the message */
      if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
          rrc_RadioBearerSetup_r3_IEs,new_U_RNTI))
      {
        rbe.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(msg_ptr->u.r3.radioBearerSetup_r3.new_U_RNTI),
                              &rbe.new_urnti);
      }
    }
  }


  if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
         rrc_RadioBearerSetup_r3_IEs,rab_InformationSetupList)
     )
  {
    /* GSM_MAP RAB Id. Check if it's valid by calling the
    appropriate NAS entity. First convert it to internal
    format. */
    rab_info_ptr = &msg_ptr->u.r3.radioBearerSetup_r3.rab_InformationSetupList;
    rab_index =0;
    /* check whether all RABs in the rabinfo list belong to same CN domain id or not
    otherwise this is an invalid configuration */
    while ((rab_info_ptr->n) >  (rab_index+1 ))
    {
      if (rab_info_ptr->elem[rab_index].rab_Info.cn_DomainIdentity !=
          rab_info_ptr->elem[rab_index+1].rab_Info.cn_DomainIdentity)
      {
        WRRC_MSG2_ERROR("Different CN domains in rab-list %d %d",
             rab_info_ptr->elem[rab_index].rab_Info.cn_DomainIdentity,
             rab_info_ptr->elem[rab_index+1].rab_Info.cn_DomainIdentity
             );
        rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
        return;
      }
      rab_index++;
    }
  
    /* restore the rab info pointer */
    rab_info_ptr = &msg_ptr->u.r3.radioBearerSetup_r3.rab_InformationSetupList;
  
    for (rab_index=0; (rab_info_ptr->n > rab_index) && (rab_index < RRCRB_MAX_RAB_PER_MSG); rab_index++)
    {
      rab_id = rrcrb_translate_gsm_map_rab_id(
                                             (rab_info_ptr->elem[rab_index].rab_Info.
                                               rab_Identity.u.gsm_MAP_RAB_Identity)
                                             );
  
      /* Store this info locally so that ESTABLISHED_RABS can
      be updated later, when these RABs are established */
      rbe.rabs.rabs_in_msg[rab_index].rab_id_is_gsm_map = TRUE;
      rbe.rabs.rabs_in_msg[rab_index].rab_id = rab_id;
      rbe.rabs.rabs_in_msg[rab_index].re_est_timer =
        rab_info_ptr->elem[rab_index].rab_Info.re_EstablishmentTimer;
      /* Check if this RAB is CS Domain or PS Domain */
      if ( msg_ptr->u.r3.radioBearerSetup_r3.rab_InformationSetupList
           .elem[rab_index].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain )
      {
        /* Store the domain identity. This will be later added to
        ESTABLISHED_RABS */
        rbe.rabs.rabs_in_msg[rab_index].cn_domain = RRC_CS_DOMAIN_CN_ID;
  
        /* For CS RABs, Call MN layer to verify this RAB */
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
        rab_type = rrcmn_verify_rab_subs( (radio_access_bearer_T)rab_id,rrc_get_as_id());
#else
        rab_type = rrcmn_verify_rab( (radio_access_bearer_T)rab_id );
#endif
        MSG_HIGH("For RAB-Id %d, RAB type from NAS = %d, RAB type from EST_RABs = %d",rab_id, rab_type,
            rrc_get_rab_type_for_given_rab_id(rbe.rabs.rabs_in_msg[rab_index].rab_id));
  
        switch ( rab_type )
        {
          case RRCMN_CS_DATA_CALL:
  
            MSG_LOW("Received a CS Data RAB Id %d", rab_id, 0, 0);
            break;
  
          case RRCMN_CS_VOICE_CALL:
  
            MSG_LOW("Received a CS Voice RAB Id %d", rab_id, 0, 0);
            if (rrc_find_if_voice_rab_exists_in_established_rabs( rbe.rabs.rabs_in_msg[rab_index].rab_id )
              == TRUE)
            {
              WRRC_MSG1_HIGH("Voice RAB Id - %d already exists", rbe.rabs.rabs_in_msg[rab_index].rab_id );
            
              /*First condition of "if" covers the case when third RB is being added for AMR call*/
              /*Second condition of "if" covers the case when previously established Voice Call was of type CS over HSPA.*/
              if(rab_info_ptr->elem[rab_index].rb_InformationSetupList.n == 1)
              {
                  /* Most likely network is trying to add third RB to switch from lower AMR rate to higher */
                  voice_rab_already_exists = TRUE;
                }
                else
                {
                  rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;             
                  WRRC_MSG0_HIGH("Invalid configuration .. more than one RAB subflow is being added");
                  return;
                }
            }
            else
            {
              /* Whenever a new voice RAB is to be set up, at least two RBs need to be associated with that RAB.
              Therefore, first condition of "if" checks for presence of more than 1 RB. And the second condition
              of "if" ensures that it's for CS over DCH.*/
              if (rab_info_ptr->elem[rab_index].rb_InformationSetupList.n  == 1)
              {
  #ifdef FEATURE_SCUDIF_ENHANCEMENT
                #error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/
  
                WRRC_MSG0_HIGH("Not enough RBs mapped to voice RAB!");
                rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
              }
            }
            break;
  
          default:
            WRRC_MSG0_HIGH("RAB type not known to MN layer");
            break;
        } /* end switch */
  
        if ( (rbe.status.failure_status == RRCRB_MSG_INVALID_CONFIG) ||
             (rbe.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG)
           )
        {
          /* No need to continue */
          return;
        }
#ifdef FEATURE_VOC_AMR_WB
        if((rab_type == RRCMN_CS_VOICE_CALL )
#ifdef FEATURE_SCUDIF_ENHANCEMENT
#error code not present
#endif/*FEATURE_SCUDIF_ENHANCEMENT*/
         )
        {
          if(rab_info_ptr->elem[rab_index].rab_Info.m.nas_Synchronisation_IndicatorPresent)
          {
            nas_sync_ind = rrc_translate_nas_sync_indicator(
                                           &(rab_info_ptr->elem[rab_index].rab_Info.nas_Synchronisation_Indicator));
            
            if( nas_sync_ind == MVS_MODE_NONE)
            {
              rbe.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
              WRRC_MSG1_HIGH("Unsupported configuration .. codec not supported %d",rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator );
              return;
            }
          }
          rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator = nas_sync_ind;
#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
          rrc_set_ordered_codec_status(TRUE);
#endif
        }
#endif /*FEATURE_VOC_AMR_WB*/

      }
      /* Else check for PS Domain */
      else if ( msg_ptr->u.r3.radioBearerSetup_r3.rab_InformationSetupList
                .elem[rab_index].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain )
      {
        /* Store the domain identity. This will be later added to
        ESTABLISHED_RABS */
        rbe.rabs.rabs_in_msg[rab_index].cn_domain = RRC_PS_DOMAIN_CN_ID;
  
        /* For PS Domain, if it's a loopback, then no need for query,
           otherwise, call RABM module to verify the RAB */
        if ( rrclbt_is_lb_on() == TRUE ||
             rabm_rabid_validate( RAB_ESTABLISH_QUERY, (uint8)rab_id )
             == TRUE )
        {
          MSG_LOW("PS RAB Id %d to be setup", rab_id, 0, 0);
        }
        else
        {
          WRRC_MSG1_ERROR("PS RAB validation failed! RABid %d",rab_id);
          rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
          return;
        }
      }
  
      if(voice_rab_already_exists == TRUE)
      {
        rbe.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_VOICE_RAB;
        WRRC_MSG0_HIGH("updated RAB type as CS Voice");
      }
      else
      {
        /* ASN1 structure 'rrc_RAB_InformationSetup' is validated at this point,
            hence update rab_type in structure 'rbe.rabs.rabs_in_msg[rab_index]'.
        */
        (void)rrc_update_rab_type_in_rab_info(
           &(msg_ptr->u.r3.radioBearerSetup_r3.rab_InformationSetupList.elem[rab_index]),
           &(rbe.rabs.rabs_in_msg[rab_index]));
      }
  
      MSG_HIGH("RAB_ID:%d,Domain:%d,re_est_tmr:%d",
                        rbe.rabs.rabs_in_msg[rab_index].rab_id,
                        rbe.rabs.rabs_in_msg[rab_index].cn_domain,
                        rbe.rabs.rabs_in_msg[rab_index].re_est_timer);
  #ifdef FEATURE_VOC_AMR_WB
      /* set the ordered codec mode which is sent in the ordered config to MAC.
        * If the RBE msg is not for Voice RAB also then also the current codec value
        * is passed to MAC 
        */
      rrc_set_ordered_codec_mode(nas_sync_ind);
      rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator = nas_sync_ind;
  #endif /*FEATURE_VOC_AMR_WB*/
  
     
    }
    if (rab_info_ptr->n > rab_index)
    {
      WRRC_MSG0_HIGH("Too many RABs in an RBSetupMessage");
      rbe.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
      return;  
    }
  }
  /* Store the number of the RAB info in the RBSetup message. */
  rbe.rabs.num_rabs_in_msg = rab_index;
  WRRC_MSG1_HIGH("%d RABs in an RBSetupMessage", rab_index);

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  rbe.trans_to_dch_ciph_config_needed = FALSE;
  if((rbe.state_from_msg == RRC_STATE_CELL_DCH) && 
     (rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
        rrc_RadioBearerSetup_r3_IEs,dl_CommonInformation)) && 
       (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3.dl_CommonInformation,
        rrc_DL_CommonInformation,dl_DPCH_InfoCommon)))
    {
      //check if ciphering is active in mac for cs domain, if it is then,
      //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      if(rrcsmc_tm_ciph_active() == TRUE)
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 config activated");
        if(rrcsmc_process_cipher_config_for_hho(RRCSMC_INVALID_HFN_VALUE) == FAILURE)
        {
          WRRC_MSG0_HIGH("Process Ciphering Config for HHO failed");
        }
        rbe.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }

  if(rbe.state_from_msg == RRC_STATE_CELL_DCH)
  {
    rbe.first_tm_rb_setup = rrcrbe_is_first_tm_rb_setup(new_tm_rb);
    if(new_tm_rb)
    {
      WRRC_MSG1_HIGH("First TM RB being set status : %d",rbe.first_tm_rb_setup);
    }
    if(rbe.first_tm_rb_setup == TRUE)
    {
      /* check to see if dl-dpch-common info is present while going to dch */
      if((RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
          rrc_RadioBearerSetup_r3_IEs,dl_CommonInformation)) && 
         (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3.dl_CommonInformation,
          rrc_DL_CommonInformation,dl_DPCH_InfoCommon)))
      {
        /* check if ciphering is active in mac for cs domain, if it is then,
          start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
        */
        WRRC_MSG0_HIGH("First time TM RB Setup Step-1 config activated");
        if(rrcsmc_process_cipher_config_for_first_tm_setup(RRCSMC_INVALID_HFN_VALUE) == FAILURE)
        {
          WRRC_MSG0_HIGH("Process Ciphering Config for First TM RB Setup  failed");
          /*This step is reached only when the first step returns a failure,. i.e, if the ciphering is not active for CS domain.
            Hence setting rbe.first_tm_rb_setup to FALSE so that step 2 is not done*/
          rbe.first_tm_rb_setup = FALSE;
        }
      }
    }
  }

  /*Cell_PCH/URA_PCH specific validations */
  if ((rbe.state_from_msg == RRC_STATE_CELL_PCH) ||
      (rbe.state_from_msg == RRC_STATE_URA_PCH))
  {
     /* If UTRAN DRX coefficient is not present then it is invalid configuration */
     if (!(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
         rrc_RadioBearerSetup_r3_IEs,utran_DRX_CycleLengthCoeff)))
     {
       WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", rbe.state_from_msg);
       rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
       return;
     }
     if (rbe.state_from_msg == RRC_STATE_URA_PCH)
     {
       /*should we do it after getting l2ack for response??*/
       if (!(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
             rrc_RadioBearerSetup_r3_IEs,ura_Identity)))

       {
         WRRC_MSG0_HIGH("URA ID not present");
         rrccu_clear_ura_id ();
       }
       else
       {
         /*URA ID present in the message set ura_identity variable with this value */
         if (msg_ptr->u.r3.radioBearerSetup_r3.ura_Identity.numbits == 16)
         {
           rrccu_update_ura_id (&(msg_ptr->u.r3.radioBearerSetup_r3.ura_Identity));
         }
         else
         {
           WRRC_MSG1_ERROR("Invalid URA ID length%d",
               msg_ptr->u.r3.radioBearerSetup_r3.ura_Identity.numbits);
           rbe.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
           rbe.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
           return;
         }

       }

     }
  }
}/* end function validate_rbsetup_message */


/*===========================================================================
FUNCTION   rrcrbe_get_rab_index_from_rbe_db

DESCRIPTION
This function returns the rab index from rbe global for the passed rab id

DEPENDENCIES
  None

RETURN VALUE
  RAB Index.

SIDE EFFECTS
  None.
===========================================================================*/
static uint8 rrcrbe_get_rab_index_from_rbe_db(rrc_rab_id_type rab_id)
{
  uint8 rbe_rab_index = RRCRB_MAX_RAB_PER_MSG;
  
  for(rbe_rab_index=0;rbe_rab_index < rbe.rabs.num_rabs_in_msg && rbe_rab_index < RRCRB_MAX_RAB_PER_MSG;rbe_rab_index++)
  {
    if(rbe.rabs.rabs_in_msg[rbe_rab_index].rab_id == rab_id)
    {
      break;
    }
  }
  return rbe_rab_index;
}
/*===========================================================================

FUNCTION   RRCRBE_SEND_CHANNEL_CONFIG_REQ

DESCRIPTION

  This function sends the RRC_CHANNEL_CONFIG_REQ command to LLC
  to setup the necessary RBs associated with a RAB. If signalling
  Radio bearers are affected and included in the Radio Bearer
  Setup Message, they will also be sent in the same command.

  Currently the messege is not expected to have Signalling
  Radio Bearers for setup. If these are added in the future,
  they will also need to be sent in the same command.

DEPENDENCIES

  ORDERED_CONFIG must be set before calling this function.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbe_send_channel_config_req
(
rrc_RadioBearerSetup *msg_ptr      /* Pointer to the RB Setup message */
)
{
  rrc_cmd_type *out_cmd_ptr;  /* To hold the Command                       */
  uint32 rb_count=0;          /* To count the number of RBs in the command */
  uint32 rbe_rb_cnt = 0;
  rrc_RB_InformationSetupList *rb_ptr; /* Temporary pointer to RB info      */
  rrc_RB_InformationAffectedList* rec_rb_ptr; /* Pointer to RBs to be
                                             Reconfigured                  */
  uint32 rab_index=0;         /* Used when more than one RAB is received
                              in a RBS message                             */
  uint32 srb_cnt=0;
  rrc_RB_Identity rb_id=DCCH_DT_HIGH_PRI_RADIO_BEARER_ID;

  rrc_RAB_InformationSetupList *rab_info_ptr;
  rrc_SRB_InformationSetupList     *    srb_setup_ptr;
  rrcrb_directed_cell_info_type        directed_cell_info; /* directed cell info struct */
  rrc_state_e_type next_rrc_state = rbe.state_from_msg;

  /* Fill in the command id  */
  rrc_cmd.cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;

  /* First fill up the Radio Bearers that need to be setup.
  For this we need to know how many RBs need to be setup.
  If RABs need to be setup, check how many RBs are mapped
  to RABs. */

  if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,rrc_RadioBearerSetup_r3_IEs,srb_InformationSetupList))
  {

    /* Get the SRB setup list */
    srb_setup_ptr = &msg_ptr->u.r3.radioBearerSetup_r3.srb_InformationSetupList;

    while((srb_setup_ptr->n > srb_cnt ) && (srb_cnt < UE_MAX_SRB_SETUP))
    {
      /* Check to see if the RB ID is there ... */
      if(srb_setup_ptr->elem[srb_cnt].m.rb_IdentityPresent)
      {
        rb_id = srb_setup_ptr->elem[srb_cnt].rb_Identity;
      }

      /*
      * ... if not, then start with the first SRB and get the next SRB ID
      * each time one is not included in the IE.
      */
      else
      {
        /* Get the next SRB, note this increments to 4 the first time
         * Note: When we start supporting SRB5 and upwards, the RLC type
         * should be checked before assiging the default SRB id.
         */
        rb_id++;
      }

      /* Fill up the RB id and indicate that it needs to be established */
      rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = rb_id;
      if(rrclcm_get_dl_rlc_lc_id(rb_id) == RRCLCM_RLC_LC_ID_NOT_FOUND )
      {
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = ESTABLISH_RB;
      }
      else
      {
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB_OTA;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = 
           rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = MODIFY_RB_CFG;
      }

      rb_count++;
      srb_cnt++;


    } /* end SRB setup */

  }

  if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
         rrc_RadioBearerSetup_r3_IEs,rab_InformationSetupList))
  {
    rab_info_ptr = &msg_ptr->u.r3.radioBearerSetup_r3.rab_InformationSetupList;
    for (rab_index=0; rab_index < rbe.rabs.num_rabs_in_msg && (rab_info_ptr->n > rab_index); rab_index++)
    {
      if ( rbe.rabs.chan_config_needed[rab_index] == TRUE )
      {
        rb_ptr = &rab_info_ptr->elem[rab_index].rb_InformationSetupList;
  
        if ( rb_ptr->n  == 0 )
        {
          WRRC_MSG0_HIGH("No RBs present in msg!");
        }
        else
        {
          /* Get the RBE db rab index */
          uint8 rbe_rab_index = rrcrbe_get_rab_index_from_rbe_db(
                                    rrcrb_translate_gsm_map_rab_id(rab_info_ptr->elem[rab_index].rab_Info.rab_Identity.u.gsm_MAP_RAB_Identity));
          rbe_rb_cnt = 0;
          if((rbe_rab_index < RRCRB_MAX_RAB_PER_MSG) && (rbe_rab_index < rbe.rabs.num_rabs_in_msg ))
          {
            while ( rb_ptr->n > rbe_rb_cnt )
            {              
              /* Fill up the RB id and indicate that it needs to be established */
              rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = rb_ptr->elem[rbe_rb_cnt].rb_Identity;
              rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = ESTABLISH_RB;
              /* Update the local structure so that we know which RB id's we've requested to be setup */
              rbe.rabs.rabs_in_msg[rbe_rab_index].rb_for_rab[rbe_rb_cnt].rb_id = rb_ptr->elem[rbe_rb_cnt].rb_Identity;
    
              /* Increment count of RBs */
              rb_count++;
              rbe_rb_cnt ++;
    
              /* Do a safety check on the count. We can't do more than 32
                       configs in a command */
              if ( (rb_count >= MAX_RB) || (rbe_rb_cnt >= MAX_RB_PER_RAB))
              {
                WRRC_MSG0_HIGH("MAX RB count exceeded!");
                break;
              }
    
              /* Go to the next RB */
              
            }/* end while */
            /* Save the number of RBs for this RAB */
            rbe.rabs.rabs_in_msg[rbe_rab_index].num_rbs_for_rab = rbe_rb_cnt;
          }
          else
          {
            WRRC_MSG1_ERROR("RBE RAB INDEX not found!!! RAB_INDEX %d",rbe_rab_index);
          }
        }/* end if rabs_setup_needed */
      }
    
    }
  }

  /* Now check if the message contains an RB information to be
  affected list. If so, we need to reconfigure some RBs */
  if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
        rrc_RadioBearerSetup_r3_IEs,rb_InformationAffectedList)
     )
  {
    uint8 local_idx=0;

    rec_rb_ptr = &msg_ptr->u.r3.radioBearerSetup_r3.rb_InformationAffectedList;

    if ( rec_rb_ptr->n  == 0 )
    {
      WRRC_MSG0_HIGH("No affected RBs in msg!");
    }
    else
    {
      while ( rec_rb_ptr->n > local_idx )
      {

        /* Fill up the RB id and indicate that it needs to be reconfigured */
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = rec_rb_ptr->elem[local_idx].rb_Identity;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB_OTA;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = MODIFY_RB_CFG;
        /* Increment count of RBs. This is done here since we
        need to use the next index that's not used already. */
        rb_count++;

        /* Go to the next affected RB */
        local_idx++;

        /* Do a safety check on the count. We can't do more than 32
        configs in a command */
        if ( rb_count >= MAX_RB )
        {
          WRRC_MSG0_HIGH("MAX RB count exceeded!");
          break;
        }
      } /* end while */
    }
  }
  else
  {
    MSG_LOW("No affected RB list in msg", 0, 0, 0);
  }/* end if affected list present */

  /* Number of RBs to config. Note rb_count is the total
  of established and reconfigured RBs */
  rrc_cmd.cmd.chan_config_req.num_rb_to_config = (int) rb_count;

  if(rb_count < MAX_RB)
  {
    rrc_fill_rb_id_chan_req_from_standalone_db(&rb_count, &rrc_cmd.cmd.chan_config_req);
  }

  if(rbe.state_from_msg == RRC_STATE_CELL_DCH)
  {
    /* Fill those RBs in chan_config_req for which RLC PDU size has changed 
    and that are already not present in chan_config_req*/
    if(rb_count < MAX_RB)
    {
      rrc_fill_rb_id_chan_req_from_rb_list_mapped_to_dch(&rb_count, &rrc_cmd.cmd.chan_config_req);
    }
    if(rrc_get_state() == RRC_STATE_CELL_FACH)
    {
      rrc_fill_srb_in_chan_config_req(&rb_count, &rrc_cmd.cmd.chan_config_req);
    }
    /*Following function fills the RBs to be released if CSoHS -> CSoDCH is happening.*/	 
  }

  /*If state transition is Cell_DCH->Cell_FACH or vice-verca, 
  add all the existing AM PS RBs in chan_config_req*/	
  if(((next_rrc_state == RRC_STATE_CELL_FACH) && 
       (transition_config.toc_usage == TOC_FOR_DCH_AND_FACH ||
        transition_config.toc_usage == TOC_FOR_FACH_AND_PCH ||
        transition_config.toc_usage == TOC_FOR_FACH_AND_PCH_WITH_DCH_INFO))
       ||
      ((rrc_get_state() == RRC_STATE_CELL_FACH) && (next_rrc_state == RRC_STATE_CELL_DCH))
    )
  {
    rrc_fill_rb_id_chan_req_from_est_rabs(&rb_count, &rrc_cmd.cmd.chan_config_req);
  }

  /* Check if state change is required */
  if ( (rrc_get_state()) == (rbe.state_from_msg) )
  {
    rrc_cmd.cmd.chan_config_req.rrc_state_change_required = FALSE;
  }
  else
  {
    /* The message indicates that state change is required.
    Indicate to LLC what the next state should be. */
    rrc_cmd.cmd.chan_config_req.rrc_state_change_required = TRUE;
    rrc_cmd.cmd.chan_config_req.next_state = rbe.state_from_msg;
  }

  /* Indicate the id of the procedure sending the command */
  rrc_cmd.cmd.chan_config_req.procedure = RRC_PROCEDURE_RBE;

  /* Indicate that we need a confirmation for this command */
  rrc_cmd.cmd.chan_config_req.rrc_channel_config_cnf_required = TRUE;

  /* We are not required to fill any other data for this command.
   Now, send the command */
  if(((rrc_get_state()) == RRC_STATE_CELL_DCH) &&
     (rrc_cmd.cmd.chan_config_req.next_state== RRC_STATE_CELL_FACH) &&
     (rrc_cmd.cmd.chan_config_req.rrc_state_change_required))
  {

    /*Initiate the cell selection*/
    rrcrb_send_initiate_cell_selection_req(RRC_PROCEDURE_RBE,
                                           RRC_TRANSITION_FROM_DCH,
                                           rbe.state_from_msg);

    rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

    /* Start T_305 [periodic cell update] timer */
    rrccu_start_t_305_timer();
  }
  else
  {
    /*check whether any frequency info is present in the message
    and transition is CELL_FACH->CELL_FACH. In this case
    initiate a cell update after channel config is successfully completed */
    if (((rrc_get_state())==RRC_STATE_CELL_FACH)&&
       (rbe.state_from_msg==RRC_STATE_CELL_FACH))
    {
      if((RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
          rrc_RadioBearerSetup_r3_IEs,frequencyInfo)) &&
         (msg_ptr->u.r3.radioBearerSetup_r3.frequencyInfo.
          modeSpecificInfo.t == T_rrc_FrequencyInfo_modeSpecificInfo_fdd))
      {
        directed_cell_info.freq_info_present = TRUE;
        directed_cell_info.uarfcn_dl         = msg_ptr->u.r3.radioBearerSetup_r3.
                                               frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
      }
      else
      {
        directed_cell_info.freq_info_present = FALSE;
      }
      if ((RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
              rrc_RadioBearerSetup_r3_IEs,dl_InformationPerRL_List)) &&
            (msg_ptr->u.r3.radioBearerSetup_r3.
               dl_InformationPerRL_List.elem[0].modeSpecificInfo.t
               == T_rrc_DL_InformationPerRL_modeSpecificInfo_fdd))
      {
        directed_cell_info.pscr_present = TRUE;
        directed_cell_info.pscr = msg_ptr->u.r3.radioBearerSetup_r3.dl_InformationPerRL_List.elem[0].
                                  modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
      }
      else
      {
        directed_cell_info.pscr_present = FALSE;
      }
      if (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE)
      {
        /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
                 FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
        if(rrccsp_check_initiate_cell_selection_handling(NULL) == FALSE)
        {
          WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
       
          rrcrbe_send_initiate_cu_flag = TRUE;
          rrcrbe_delete_crnti          = TRUE;
        }
        else
        {
        
          rrc_int_c_rnti_type      c_rnti;       /* Local C_RNTI value */
          if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
             rrc_RadioBearerSetup_r3_IEs,new_C_RNTI))
          {
            rbe.new_crnti_valid = TRUE;
            rrc_translate_crnti(&msg_ptr->u.r3.radioBearerSetup_r3.new_C_RNTI,
                                &c_rnti);
            rbe.new_crnti = (uint16)c_rnti;
          }
          else
          {
            rbe.new_crnti_valid = FALSE;
          }
          rrcllcoc_update_toc_with_oc_from_fach();
          
          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req(RRC_PROCEDURE_RBE,
                                                  RRC_TRANSITION_FROM_FACH,
                                                  rbe.state_from_msg);
       
          rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
              
          return;
        }
      }
    } /* if CELL_FACH->CELL_FACH reconfiguration */
    /* Get a Command buffer and copy the local command to the buffer
    * so as to enqueue it on the internal queue.
    */
    if ((out_cmd_ptr = rrc_get_int_cmd_buf())!=NULL)
    {
      WCDMA_MEMCPY(out_cmd_ptr,
                   sizeof(rrc_cmd_type),
                   &rrc_cmd,
                   sizeof(rrc_cmd_int_type));
      rrc_put_int_cmd( out_cmd_ptr );
    }
  }

}/* end function rrcrbe_send_channel_config_req */


/*===========================================================================

FUNCTION   RRCRBE_SEND_CHANNEL_CONFIG_REQ_R5

DESCRIPTION

  This function sends the RRC_CHANNEL_CONFIG_REQ command to LLC
  to setup the necessary RBs associated with a RAB. If signalling
  Radio bearers are affected and included in the Radio Bearer
  Setup Message, they will also be sent in the same command.

  Currently the messege is not expected to have Signalling
  Radio Bearers for setup. If these are added in the future,
  they will also need to be sent in the same command.

DEPENDENCIES

  ORDERED_CONFIG must be set before calling this function.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbe_send_channel_config_req_r5
(
rrc_RadioBearerSetup *msg_ptr      /* Pointer to the RB Setup message */
)
{
  rrc_cmd_type *out_cmd_ptr;  /* To hold the Command                       */
  uint32 rb_count=0;          /* To count the number of RBs in the command */
  uint32 rbe_rb_cnt = 0;

  uint32 rab_index=0;         /* Used when more than one RAB is received in a RBS message */
  uint32 srb_cnt=0;
  rrc_RB_Identity rb_id=0;

  rrc_RB_InformationSetupList_r5 *rb_ptr = NULL; /* Temporary pointer to RB info      */
  rrc_RB_InformationAffectedList_r5*  rec_rb_ptr = NULL;  /* Pointer to RBs to be Reconfigured */

  rrc_RAB_InformationSetupList_r5 *rab_info_ptr = NULL;
  rrc_SRB_InformationSetupList_r5   *      srb_setup_ptr = NULL;
  rrcrb_directed_cell_info_type        directed_cell_info; /* directed cell info struct */

  rrc_RadioBearerSetup_r5_IEs* rbsetup_ptr = NULL;
  rrc_state_e_type next_rrc_state = rbe.state_from_msg;

  /* Fill in the command id  */
  rrc_cmd.cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;

  /* Set RBSetup Ptr */
  rbsetup_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.radioBearerSetup_r5;


  /* First fill up the Radio Bearers that need to be setup.
  For this we need to know how many RBs need to be setup.
  If RABs need to be setup, check how many RBs are mapped
  to RABs. */

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,rrc_RadioBearerSetup_r5_IEs,srb_InformationSetupList))
  {
    /* Get the SRB setup list */
    srb_setup_ptr = &rbsetup_ptr->srb_InformationSetupList;

    while ((srb_setup_ptr->n > srb_cnt) && (srb_cnt < UE_MAX_SRB_SETUP))
    {
      /* Check to see if the RB ID is there ... */
      if (srb_setup_ptr->elem[srb_cnt].m.rb_IdentityPresent)
      {
        rb_id = srb_setup_ptr->elem[srb_cnt].rb_Identity;
      }

      /*
      * ... if not, then start with the first SRB and get the next SRB ID
      * each time one is not included in the IE.
      */
      else
      {
        /* Get the next SRB, note this increments to 1 the first time */
        rb_id++;
      }

      /* Fill up the RB id and indicate that it needs to be established */
      rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = rb_id;
      if(rrclcm_get_dl_rlc_lc_id(rb_id) == RRCLCM_RLC_LC_ID_NOT_FOUND )
      {
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = ESTABLISH_RB;
      }
      else
      {
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB_OTA;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = 
            rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = MODIFY_RB_CFG;
      }

      rb_count++;
      srb_cnt++;


    } /* end SRB setup */

  }

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,rrc_RadioBearerSetup_r5_IEs,rab_InformationSetupList))  
  {
    rab_info_ptr = &rbsetup_ptr->rab_InformationSetupList;
    for (rab_index=0; rab_index < rbe.rabs.num_rabs_in_msg && (rab_info_ptr->n > rab_index); rab_index++)
    {
      if ( rbe.rabs.chan_config_needed[rab_index] == TRUE )
      {
        rb_ptr = &rab_info_ptr->elem[rab_index].rb_InformationSetupList;
  
        if ( rb_ptr->n == 0 )
        {
          WRRC_MSG0_HIGH("No RBs present in msg!");
        }
        else
        {
          /* Get the RBE db rab index */
          uint8 rbe_rab_index = rrcrbe_get_rab_index_from_rbe_db(
                                   rrcrb_translate_gsm_map_rab_id(rab_info_ptr->elem[rab_index].rab_Info.rab_Identity.u.gsm_MAP_RAB_Identity));
          rbe_rb_cnt = 0;
          if((rbe_rab_index < RRCRB_MAX_RAB_PER_MSG) && (rbe_rab_index < rbe.rabs.num_rabs_in_msg ))
          {
            while ( rb_ptr->n > rbe_rb_cnt )
            {
              /* Fill up the RB id and indicate that it needs to be established */
              rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = rb_ptr->elem[rbe_rb_cnt].rb_Identity;
              rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = ESTABLISH_RB;

              /* Update the local structure so that we know which RB id's we've requested to be setup */
              rbe.rabs.rabs_in_msg[rbe_rab_index].rb_for_rab[rbe_rb_cnt].rb_id = rb_ptr->elem[rbe_rb_cnt].rb_Identity;

              /* Increment count of RBs */
              rb_count++;
              rbe_rb_cnt ++;
    
              /* Do a safety check on the count. We can't do more than 32
                       configs in a command */
              if ( (rb_count >= MAX_RB) || (rbe_rb_cnt >= MAX_RB_PER_RAB))
              {
                WRRC_MSG0_HIGH("MAX RB count exceeded!");
                break;
              }
    
              /* Go to the next RB */
            }/* end while */
            /* Save the number of RBs for this RAB */
            rbe.rabs.rabs_in_msg[rbe_rab_index].num_rbs_for_rab = rbe_rb_cnt;
          }
          else
          {
            WRRC_MSG1_ERROR("RBE RAB INDEX not found!!! RAB_INDEX %d",rbe_rab_index);
          }
        }
      }/* end if rabs_setup_needed */
      
    }
  }

  /* Now check if the message contains an RB information to be
  affected list. If so, we need to reconfigure some RBs */
  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,rrc_RadioBearerSetup_r5_IEs,rb_InformationAffectedList))
  {
  uint32 local_idx =0;

    rec_rb_ptr = &rbsetup_ptr->rb_InformationAffectedList;

    if ( rec_rb_ptr->n == 0 )
    {
      WRRC_MSG0_HIGH("No affected RBs in msg!");
    }
    else
    {
      while ( rec_rb_ptr->n > local_idx)
      {

        /* Fill up the RB id and indicate that it needs to be reconfigured */
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = rec_rb_ptr->elem[local_idx].rb_Identity;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB_OTA;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = MODIFY_RB_CFG;
        /* Increment count of RBs. This is done here since we
        need to use the next index that's not used already. */
        rb_count++;

        /* Go to the next affected RB */
       local_idx++;

        /* Do a safety check on the count. We can't do more than 32
        configs in a command */
        if ( rb_count >= MAX_RB )
        {
          WRRC_MSG0_HIGH("MAX RB count exceeded!");
          break;
        }
      } /* end while */
    }
  }
  else
  {
    MSG_LOW("No affected RB list in msg", 0, 0, 0);
  }/* end if affected list present */

  /* Number of RBs to config. Note rb_count is the total
  of established and reconfigured RBs */
  rrc_cmd.cmd.chan_config_req.num_rb_to_config = (int) rb_count;

  if(rb_count < MAX_RB)
  {
    rrc_fill_rb_id_chan_req_from_standalone_db(&rb_count, &rrc_cmd.cmd.chan_config_req);
  }

  if(rbe.state_from_msg == RRC_STATE_CELL_DCH)
  {
    /* Fill those RBs in chan_config_req for which RLC PDU size has changed and 
    that are already not present in chan_config_req*/
    if(rb_count < MAX_RB)
    {
      rrc_fill_rb_id_chan_req_from_rb_list_mapped_to_dch(&rb_count, &rrc_cmd.cmd.chan_config_req);
    }
    if(rrc_get_state() == RRC_STATE_CELL_FACH)
    {
      rrc_fill_srb_in_chan_config_req(&rb_count, &rrc_cmd.cmd.chan_config_req);
    }
    /*Following function fills the RBs to be released if CSoHS -> CSoDCH is happening.*/	 
  }

  /*If state transition is Cell_DCH->Cell_FACH or vice-verca, add all 
  the existing AM PS RBs in chan_config_req*/	
  if(((next_rrc_state == RRC_STATE_CELL_FACH) && 
       (transition_config.toc_usage == TOC_FOR_DCH_AND_FACH ||
        transition_config.toc_usage == TOC_FOR_FACH_AND_PCH ||
        transition_config.toc_usage == TOC_FOR_FACH_AND_PCH_WITH_DCH_INFO))
       ||
      ((rrc_get_state() == RRC_STATE_CELL_FACH) && (next_rrc_state == RRC_STATE_CELL_DCH))
    )
  {
    rrc_fill_rb_id_chan_req_from_est_rabs(&rb_count, &rrc_cmd.cmd.chan_config_req);
  }

  /* Check if state change is required */
  if ( (rrc_get_state()) == (rbe.state_from_msg) )
  {
    rrc_cmd.cmd.chan_config_req.rrc_state_change_required = FALSE;
  }
  else
  {
    /* The message indicates that state change is required.
    Indicate to LLC what the next state should be. */
    rrc_cmd.cmd.chan_config_req.rrc_state_change_required = TRUE;
    rrc_cmd.cmd.chan_config_req.next_state = rbe.state_from_msg;
  }

  /* Indicate the id of the procedure sending the command */
  rrc_cmd.cmd.chan_config_req.procedure = RRC_PROCEDURE_RBE;

  /* Indicate that we need a confirmation for this command */
  rrc_cmd.cmd.chan_config_req.rrc_channel_config_cnf_required = TRUE;

  /* We are not required to fill any other data for this command.
   Now, send the command */
  if (((rrc_get_state()) == RRC_STATE_CELL_DCH) &&
      (rrc_cmd.cmd.chan_config_req.next_state== RRC_STATE_CELL_FACH) &&
      (rrc_cmd.cmd.chan_config_req.rrc_state_change_required))
  {

    /*Initiate the cell selection*/
    rrcrb_send_initiate_cell_selection_req(RRC_PROCEDURE_RBE,
                                           RRC_TRANSITION_FROM_DCH,
                                           rbe.state_from_msg);

    rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

    /* Start T_305 [periodic cell update] timer */
    rrccu_start_t_305_timer();

  } 
  else
  {
    /*check whether any frequency info is present in the message
    and transition is CELL_FACH->CELL_FACH. In this case
    initiate a cell update after channel config is successfully completed */
    if (((rrc_get_state())==RRC_STATE_CELL_FACH)&&
        (rbe.state_from_msg==RRC_STATE_CELL_FACH))
    {
      if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
           rrc_RadioBearerSetup_r5_IEs,frequencyInfo)) &&
          (rbsetup_ptr->frequencyInfo.
           modeSpecificInfo.t == T_rrc_FrequencyInfo_modeSpecificInfo_fdd))
      {
        directed_cell_info.freq_info_present = TRUE;
        directed_cell_info.uarfcn_dl = rbsetup_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
      }
      else
      {
        directed_cell_info.freq_info_present = FALSE;
      }
      if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,rrc_RadioBearerSetup_r5_IEs,dl_InformationPerRL_List)) &&
          (rbsetup_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.t
           == T_rrc_DL_InformationPerRL_r5_modeSpecificInfo_fdd))
      {
        directed_cell_info.pscr_present = TRUE;
        directed_cell_info.pscr = rbsetup_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
      }
      else
      {
        directed_cell_info.pscr_present = FALSE;
      }
      if (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE)
      {
        /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection 
              (FACH->FACH ), treat it as un-supported configuration and send failure.*/ 
        if(rrccsp_check_initiate_cell_selection_handling(NULL) == FALSE)
        {
          WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
       
          rrcrbe_send_initiate_cu_flag = TRUE;
          rrcrbe_delete_crnti          = TRUE;
        }
        else
        {
          rrc_int_c_rnti_type      c_rnti;       /* Local C_RNTI value */
          if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
                  rrc_RadioBearerSetup_r5_IEs,new_C_RNTI))
          {
            rbe.new_crnti_valid = TRUE;
            rrc_translate_crnti(&rbsetup_ptr->new_C_RNTI,
                                &c_rnti);
            rbe.new_crnti = (uint16)c_rnti;
          }
          else
          {
            rbe.new_crnti_valid = FALSE;
          }
          rrcllcoc_update_toc_with_oc_from_fach();
          
          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req(RRC_PROCEDURE_RBE,
                                                  RRC_TRANSITION_FROM_FACH,
                                                  rbe.state_from_msg);
       
          rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
           
          return;
        }
      }


    } /* if CELL_FACH->CELL_FACH reconfiguration */
    /* Get a Command buffer and copy the local command to the buffer
    * so as to enqueue it on the internal queue.
    */
    if ((out_cmd_ptr = rrc_get_int_cmd_buf())!=NULL)
    {
      WCDMA_MEMCPY(out_cmd_ptr,
                   sizeof(rrc_cmd_type),
                   &rrc_cmd,
                   sizeof(rrc_cmd_int_type));
      rrc_put_int_cmd( out_cmd_ptr );
    }
  }

}/* end function rrcrbe_send_channel_config_req_r5 */


/*===========================================================================

FUNCTION   RRCRBE_SEND_CHANNEL_CONFIG_REQ_R6

DESCRIPTION

  This function sends the RRC_CHANNEL_CONFIG_REQ command to LLC
  to setup the necessary RBs associated with a RAB. If signalling
  Radio bearers are affected and included in the Radio Bearer
  Setup Message, they will also be sent in the same command.

  Currently the messege is not expected to have Signalling
  Radio Bearers for setup. If these are added in the future,
  they will also need to be sent in the same command.

DEPENDENCIES

  ORDERED_CONFIG must be set before calling this function.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbe_send_channel_config_req_r6
(
rrc_RadioBearerSetup *msg_ptr      /* Pointer to the RB Setup message */
)
{
  rrc_cmd_type *out_cmd_ptr;  /* To hold the Command                       */
  uint32 rb_count=0;          /* To count the number of RBs in the command */
  uint32 rbe_rb_cnt = 0;

  uint32 rab_index=0;         /* Used when more than one RAB is received in a RBS message */
  uint32 srb_cnt=0;
  rrc_RB_Identity rb_id=0;

  rrc_RB_InformationSetupList_r6 *rb_ptr = NULL; /* Temporary pointer to RB info      */
  rrc_RB_InformationAffectedList_r6 * affected_rb_ptr = NULL;  /* Pointer to RBs to be Reconfigured */
  rrc_RB_InformationReconfigList_r6 * reconfig_rb_ptr = NULL;
  rrc_RAB_InformationSetupList_r6 *rab_info_ptr = NULL;
  rrc_SRB_InformationSetupList_r6       *  srb_setup_ptr = NULL;
  rrcrb_directed_cell_info_type        directed_cell_info; /* directed cell info struct */

  rrc_RadioBearerSetup_r6_IEs* rbsetup_ptr = NULL;
  rrc_state_e_type next_rrc_state = rbe.state_from_msg;

  /* Fill in the command id  */
  rrc_cmd.cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;

  /* Set RBSetup Ptr */
  rbsetup_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
    criticalExtensions.u.r6.radioBearerSetup_r6;


  if (rbsetup_ptr->specificationMode.t ==
      T_rrc_RadioBearerSetup_r6_IEs_specificationMode_complete)
  {
    /* First fill up the Radio Bearers that need to be setup.
    For this we need to know how many RBs need to be setup.
    If RABs need to be setup, check how many RBs are mapped
    to RABs. */
    if (rbsetup_ptr->specificationMode.u.complete.m.srb_InformationSetupListPresent)
    {
      /* Get the SRB setup list */
      srb_setup_ptr = &rbsetup_ptr->specificationMode.u.complete.srb_InformationSetupList;
  
      while ((srb_setup_ptr->n > srb_cnt) && (srb_cnt < UE_MAX_SRB_SETUP))
      {
        /* Check to see if the RB ID is there ... */
        if (srb_setup_ptr->elem[srb_cnt].m.rb_IdentityPresent)
        {
          rb_id = srb_setup_ptr->elem[srb_cnt].rb_Identity;
        }
  
        /*
        * ... if not, then start with the first SRB and get the next SRB ID
        * each time one is not included in the IE.
        */
        else
        {
          /* Get the next SRB, note this increments to 1 the first time */
          rb_id++;
        }
  
        /* Fill up the RB id and indicate that it needs to be established */
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = rb_id;
        if(rrclcm_get_dl_rlc_lc_id(rb_id) == RRCLCM_RLC_LC_ID_NOT_FOUND )
        {
          rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = ESTABLISH_RB;
        }
        else
        {
          rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB;
          rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB_OTA;
          rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = 
            rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = MODIFY_RB_CFG;
        }
  
        rb_count++;
        srb_cnt++;
      } /* end SRB setup */
    }

    if (rbsetup_ptr->specificationMode.u.complete.m.rab_InformationSetupListPresent)	
    {
      rab_info_ptr = &rbsetup_ptr->specificationMode.u.complete.rab_InformationSetupList;

      for (rab_index=0; (rab_index < rbe.rabs.num_rabs_in_msg )&& (rab_info_ptr->n > rab_index); rab_index++)
      {
        if ( rbe.rabs.chan_config_needed[rab_index] == TRUE )
        {
          rb_ptr = &rab_info_ptr->elem[rab_index].rb_InformationSetupList;
    
          if ( rb_ptr->n == 0 )
          {
            WRRC_MSG0_HIGH("No RBs present in msg!");
          }
          else
          {
            /* Get the RBE db rab index */
            uint8 rbe_rab_index = rrcrbe_get_rab_index_from_rbe_db(
                                     rrcrb_translate_gsm_map_rab_id(rab_info_ptr->elem[rab_index].rab_Info.rab_Identity.u.gsm_MAP_RAB_Identity));
            rbe_rb_cnt = 0;
            if((rbe_rab_index < RRCRB_MAX_RAB_PER_MSG) && (rbe_rab_index < rbe.rabs.num_rabs_in_msg ))
            {
              while ( rb_ptr->n > rbe_rb_cnt )
              {
                /* Fill up the RB id and indicate that it needs to be established */
                rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = rb_ptr->elem[rbe_rb_cnt].rb_Identity;
                rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = ESTABLISH_RB;

                /* Update the local structure so that we know which RB id's we've requested to be setup */
                rbe.rabs.rabs_in_msg[rbe_rab_index].rb_for_rab[rbe_rb_cnt].rb_id = rb_ptr->elem[rbe_rb_cnt].rb_Identity;

                /* Increment count of RBs */
                rb_count++;
                rbe_rb_cnt ++;
      
                /* Do a safety check on the count. We can't do more than 32
                         configs in a command */
                if ( (rb_count >= MAX_RB) || (rbe_rb_cnt >= MAX_RB_PER_RAB))
                {
                  WRRC_MSG0_HIGH("MAX RB count exceeded!");
                  break;
                }
      
                /* Go to the next RB */
              }/* end while */
              /* Save the number of RBs for this RAB */
              rbe.rabs.rabs_in_msg[rbe_rab_index].num_rbs_for_rab = rbe_rb_cnt;
            }
            else
            {
              WRRC_MSG1_ERROR("RBE RAB INDEX not found!!! RAB_INDEX %d",rbe_rab_index);
            }
          }
        }/* end if rabs_setup_needed */
      }
    }

    /* Now check if the message contains an RB information to be
       reconfig list. If so, we need to reconfigure some RBs */
    if ( rbsetup_ptr->specificationMode.u.complete.m.rb_InformationReconfigListPresent)
    {
     uint32 local_idx=0;
  
      reconfig_rb_ptr = &rbsetup_ptr->specificationMode.u.complete.rb_InformationReconfigList;
  
      while ( reconfig_rb_ptr->n > local_idx )
      {
  
        /* Fill up the RB id and indicate that it needs to be reconfigured */
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = reconfig_rb_ptr->elem[local_idx].rb_Identity;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB_OTA;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = 0;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = 0;
        if((reconfig_rb_ptr->elem[local_idx].m.rlc_InfoPresent)&&
           (reconfig_rb_ptr->elem[local_idx].rlc_Info.m.ul_RLC_ModePresent))
        {
          rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = MODIFY_RB_CFG;
        }
        if((reconfig_rb_ptr->elem[local_idx].m.rlc_InfoPresent)&&
           (reconfig_rb_ptr->elem[local_idx].rlc_Info.m.dl_RLC_ModePresent))
        {
          rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = MODIFY_RB_CFG;
        }
        if (reconfig_rb_ptr->elem[local_idx].m.rb_StopContinuePresent )
        {
          /* over-ride rb_config with STOP or continue */
          if (rrc_RB_StopContinue_stopRB == reconfig_rb_ptr->elem[local_idx].rb_StopContinue)
          {
            rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask |= STOP_RB_CFG;
            rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask |= STOP_RB_CFG;
          }
          else if (rrc_RB_StopContinue_continueRB == reconfig_rb_ptr->elem[local_idx].rb_StopContinue)
          {
            rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask |= CONTINUE_RB_CFG;
            rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask |= CONTINUE_RB_CFG;
            
          }
        }
  
        /* Increment count of RBs. This is done here since we
        need to use the next index that's not used already. */
        rb_count++;
        /* Go to the next affected RB */
        local_idx++;
  
        /* Do a safety check on the count. We can't do more than 32
        configs in a command */
        if ( rb_count >= MAX_RB )
        {
          WRRC_MSG0_HIGH("MAX RB count exceeded!");
          break;
        }
      } /* end while */
    }
  
    /* Now check if the message contains an RB information to be
    affected list. If so, we need to reconfigure some RBs */
    if ( rbsetup_ptr->specificationMode.u.complete.m.rb_InformationAffectedListPresent)
    {
      uint32 local_idx=0;
  
      affected_rb_ptr = &rbsetup_ptr->specificationMode.u.complete.rb_InformationAffectedList;
  
      if ( affected_rb_ptr->n == 0 )
      {
        WRRC_MSG0_HIGH("No affected RBs in msg!");
      }
      else
      {
        while ( affected_rb_ptr->n > local_idx )
        {
  
          /* Fill up the RB id and indicate that it needs to be reconfigured */
          rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = affected_rb_ptr->elem[local_idx].rb_Identity;
          rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB;
          rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB_OTA;
          rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = MODIFY_RB_CFG;
          /* Increment count of RBs. This is done here since we
          need to use the next index that's not used already. */
          rb_count++;
  
          /* Go to the next affected RB */
         local_idx++;
  
          /* Do a safety check on the count. We can't do more than 32
          configs in a command */
          if ( rb_count >= MAX_RB )
          {
            WRRC_MSG0_HIGH("MAX RB count exceeded!");
            break;
          }
        } /* end while */
      }
    }
    else
    {
      MSG_LOW("No affected RB list in msg", 0, 0, 0);
    }/* end if affected list present */

  }
  else if( rbsetup_ptr->specificationMode.t == T_rrc_RadioBearerSetup_r6_IEs_specificationMode_dummy)
  {
    WRRC_MSG0_ERROR("default config not expected in RB-Setup");
  }

  /* Number of RBs to config. Note rb_count is the total
  of established and reconfigured RBs */
  rrc_cmd.cmd.chan_config_req.num_rb_to_config = (int) rb_count;

  if(rb_count < MAX_RB)
  {
    rrc_fill_rb_id_chan_req_from_standalone_db(&rb_count, &rrc_cmd.cmd.chan_config_req);
  }

  if(rbe.state_from_msg == RRC_STATE_CELL_DCH)
  {
     /* Fill those RBs in chan_config_req for which RLC PDU size has changed and 
     that are already not present in chan_config_req*/
    if(rb_count < MAX_RB)
    {
      rrc_fill_rb_id_chan_req_from_rb_list_mapped_to_dch(&rb_count, &rrc_cmd.cmd.chan_config_req);
    }
    if(rrc_get_state() == RRC_STATE_CELL_FACH)
    {
      rrc_fill_srb_in_chan_config_req(&rb_count, &rrc_cmd.cmd.chan_config_req);
    }
    /*Following function fills the RBs to be released if CSoHS -> CSoDCH is happening.*/	 
  }
 
  /*If state transition is Cell_DCH->Cell_FACH or vice-verca, add all the existing 
  AM PS RBs in chan_config_req*/	
  if(((next_rrc_state == RRC_STATE_CELL_FACH) && 
       (transition_config.toc_usage == TOC_FOR_DCH_AND_FACH ||
        transition_config.toc_usage == TOC_FOR_FACH_AND_PCH ||
        transition_config.toc_usage == TOC_FOR_FACH_AND_PCH_WITH_DCH_INFO))
       ||
      ((rrc_get_state() == RRC_STATE_CELL_FACH) && (next_rrc_state == RRC_STATE_CELL_DCH))
    )
  {
    rrc_fill_rb_id_chan_req_from_est_rabs(&rb_count, &rrc_cmd.cmd.chan_config_req);
  }
  
  /* Check if state change is required */
  if ( (rrc_get_state()) == (rbe.state_from_msg) )
  {
    rrc_cmd.cmd.chan_config_req.rrc_state_change_required = FALSE;
  }
  else
  {
    /* The message indicates that state change is required.
    Indicate to LLC what the next state should be. */
    rrc_cmd.cmd.chan_config_req.rrc_state_change_required = TRUE;
    rrc_cmd.cmd.chan_config_req.next_state = rbe.state_from_msg;
  }

  /* Indicate the id of the procedure sending the command */
  rrc_cmd.cmd.chan_config_req.procedure = RRC_PROCEDURE_RBE;

  /* Indicate that we need a confirmation for this command */
  rrc_cmd.cmd.chan_config_req.rrc_channel_config_cnf_required = TRUE;

  /* We are not required to fill any other data for this command.
   Now, send the command */
  if (((rrc_get_state()) == RRC_STATE_CELL_DCH) &&
      (rrc_cmd.cmd.chan_config_req.next_state== RRC_STATE_CELL_FACH) &&
      (rrc_cmd.cmd.chan_config_req.rrc_state_change_required))
  {

    /*Initiate the cell selection*/
    rrcrb_send_initiate_cell_selection_req(RRC_PROCEDURE_RBE,
                                           RRC_TRANSITION_FROM_DCH,
                                           rbe.state_from_msg);

    rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

    /* Start T_305 [periodic cell update] timer */
    rrccu_start_t_305_timer();
  }
  else
  {
    /*check whether any frequency info is present in the message
    and transition is CELL_FACH->CELL_FACH. In this case
    initiate a cell update after channel config is successfully completed */
    if (((rrc_get_state())==RRC_STATE_CELL_FACH)&&
        (rbe.state_from_msg==RRC_STATE_CELL_FACH))
    {
      if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
          rrc_RadioBearerSetup_r6_IEs,frequencyInfo)) &&
          (rbsetup_ptr->frequencyInfo.
           modeSpecificInfo.t == T_rrc_FrequencyInfo_modeSpecificInfo_fdd))
      {
        directed_cell_info.freq_info_present = TRUE;
        directed_cell_info.uarfcn_dl = rbsetup_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
      }
      else
      {
        directed_cell_info.freq_info_present = FALSE;
      }
      if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,rrc_RadioBearerSetup_r6_IEs,dl_InformationPerRL_List)) &&
          (rbsetup_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.t
           == T_rrc_DL_InformationPerRL_r6_modeSpecificInfo_fdd))
      {
        directed_cell_info.pscr_present = TRUE;
        directed_cell_info.pscr = rbsetup_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
      }
      else
      {
        directed_cell_info.pscr_present = FALSE;
      }
      if (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE)
      {
        /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection 
              (FACH->FACH ), treat it as un-supported configuration and send failure.*/ 
        if(rrccsp_check_initiate_cell_selection_handling(NULL) == FALSE)
        {
          WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
       
          rrcrbe_send_initiate_cu_flag = TRUE;
          rrcrbe_delete_crnti          = TRUE;
        }
        else
        {
          rrc_int_c_rnti_type      c_rnti;       /* Local C_RNTI value */
          if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
                rrc_RadioBearerSetup_r6_IEs,new_C_RNTI))
          {
            rbe.new_crnti_valid = TRUE;
            rrc_translate_crnti(&rbsetup_ptr->new_C_RNTI,
                                &c_rnti);
            rbe.new_crnti = (uint16)c_rnti;
          }
          else
          {
            rbe.new_crnti_valid = FALSE;
          }
          rrcllcoc_update_toc_with_oc_from_fach();
          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req(RRC_PROCEDURE_RBE,
                                                  RRC_TRANSITION_FROM_FACH,
                                                  rbe.state_from_msg);
       
          rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
                     
          return;
        }
      }
    } /* if CELL_FACH->CELL_FACH reconfiguration */
    /* Get a Command buffer and copy the local command to the buffer
    * so as to enqueue it on the internal queue.
    */

    if ((out_cmd_ptr = rrc_get_int_cmd_buf())!=NULL)
    {
      WCDMA_MEMCPY(out_cmd_ptr,
                   sizeof(rrc_cmd_type),
                   &rrc_cmd,
                   sizeof(rrc_cmd_int_type));
      rrc_put_int_cmd( out_cmd_ptr );
    }
  }
}/* end function rrcrbe_send_channel_config_req_r6 */


/*===========================================================================

FUNCTION  RRCRBE_CLEAR_PROCEDURE

DESCRIPTION

  This function clears all the internal variables in
  the RBE procedure and brings them to their default
  values.

DEPENDENCIES

  The Substate of the procedure should be changed
  to initial substate before calling this function.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbe_clear_procedure( void )
{
  uint32 count;

  /* Initialize the transaction id's for this procedure.
  Use a number that can't be an actual received transaction
  id. */
  rbe.tr_ids.accepted_transaction = RRCRB_INVALID_TRANSACTION_ID;
  rbe.tr_ids.rejected_transaction = RRCRB_INVALID_TRANSACTION_ID;

  /* Clear the C-RNTI valid flag */
  rbe.new_crnti_valid    = FALSE;
  rbe.directed_cell_info = FALSE;

  rrcrbe_send_initiate_cu_flag = FALSE;
  rrcrbe_delete_crnti          = FALSE;

  rbe.rbe_for_srns_relocation = FALSE;
  rbe.new_urnti_valid = FALSE;

  /* free the memory allocate for mm_cmd_buf
  if it is not free or in case of unsuccessful events
  before sending this cmd to mm */
  if (rbe.cn_info_saved != FALSE)
  {
    /* free the memory allocated for mm cmd */
    WRRC_MSG1_HIGH("Free MM cmd buffer %x", rbe.mm_cmd_ptr);
    mm_free_cmd_buf(rbe.mm_cmd_ptr);
    /* set cn_info_saved to false */
    rbe.cn_info_saved = FALSE;
  }
  rbe.status.failure_status = RRCRB_MSG_VALID;

  /* Initialize the RABS per message structure */
  for ( count=0; count < RRCRB_MAX_RAB_PER_MSG; count++ )
  {
    rbe.rabs.rabs_in_msg[count].num_rbs_for_rab = 0;
    rbe.rabs.rabs_in_msg[count].rab_type= RRCRB_UNKNOWN_RAB;
    rbe.rabs.chan_config_needed[count] = FALSE;
#ifdef FEATURE_VOC_AMR_WB
    rbe.rabs.rabs_in_msg[count].nas_Synchronisation_Indicator = MVS_MODE_NONE;
#endif /*FEATURE_VOC_AMR_WB*/
  }
  rbe.rabs.num_rabs_in_msg = 0;

#ifdef FEATURE_VOC_AMR_WB
  rrc_set_ordered_codec_mode(rrc_get_current_codec_mode());
#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
  rrc_set_ordered_codec_status(FALSE);
#endif
#endif

  rrc_check_and_init_rrc_reestab_entity(RRC_PROCEDURE_RBE);
  rrccu_reset_registration_info_for_procedure(RRC_PROCEDURE_RBE);

}
/*===========================================================================
FUNCTION   rrcrbe_prevalidate_rab_setup_list_r8

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static uecomdef_status_e_type rrcrbe_prevalidate_rab_setup_list_r8
(
  rrc_RAB_InformationSetupList_r8 *rab_list_ptr,
  boolean *new_tm_rb
)
{
  rrc_RB_InformationSetup_r8 *rb_info_ptr = NULL;
  rrc_RAB_InformationSetup_r8 * rab_info_ptr=NULL;
  uint32 rab_idx =0,rb_index=0;

  rlc_lc_id_type local_lc_id;
  rrclcm_status_e_type result;
  boolean dl_tm_flag;
  uecomdef_logch_mode_e_type rx_mode;
  
  do  /*going through the RAB info setup list */
  {
    rab_info_ptr = &rab_list_ptr->elem[rab_idx];
    /*Reject if RAB Info Replace IE is present.*/
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(rab_info_ptr,rrc_RAB_InformationSetup_r8,rab_InfoReplace))
    {
      WRRC_MSG0_ERROR("REL8:IE rab_InfoReplace not supported yet!");
      rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return FAILURE;  
    }

    /* Reject if MBMS info is included */
    if (RRC_MSG_COMMON_BITMASK_IE(rab_info_ptr->rab_Info,rrc_RAB_Info_r7,mbms_SessionIdentity))
    {
      WRRC_MSG0_ERROR("REL8:IE mbms_SessionIdentity not supported yet!");
      rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return FAILURE;
    }

    /* Reject if MBMS info is included */
    if (RRC_MSG_COMMON_BITMASK_IE_TYPE2(rab_info_ptr->rab_Info,mbms_ServiceIdentity))
    {
      WRRC_MSG0_ERROR("REL8:IE mbms_ServiceIdentity not supported yet!");
      rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return FAILURE;
    }

    /*RRC_GKG: While supporting CSoHSPA, ensure to remove this check.*/
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(rab_info_ptr,rrc_RAB_InformationSetup_r8,cs_HSPA_Information))
    {
      WRRC_MSG0_ERROR("REL8:IE cs_HSPA_Information not supported yet!");
      rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return FAILURE;  
    }

    /* Check if the RAB id is GSM_MAP type. We don't support any other type */
    if((rab_info_ptr->rab_Info.rab_Identity.t
        != T_rrc_RAB_Identity_gsm_MAP_RAB_Identity))
    {
      WRRC_MSG1_HIGH("REL8:RAB ID type %d is not GSM_MAP!",rab_info_ptr->rab_Info.rab_Identity.t);
      rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return FAILURE;
    }

    /* check whether all RABs in the rabinfo list belong to same CN domain id or not
        otherwise this is an invalid configuration */
    if(rab_list_ptr->n > (rab_idx +1))
    {
      if(rab_info_ptr->rab_Info.cn_DomainIdentity !=
        rab_list_ptr->elem[rab_idx +1].rab_Info.cn_DomainIdentity)
      {
        WRRC_MSG2_ERROR("REL8:Different CN domains in RAB-list %d %d",
            rab_info_ptr->rab_Info.cn_DomainIdentity,
            rab_list_ptr->elem[rab_idx +1].rab_Info.cn_DomainIdentity);

        rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
        return FAILURE;
      }
    }

    /* check for the unsupported PDCP info */
    rb_index =0;
    do  /*going through the RB info setup list*/
    {
      
	  rb_info_ptr = &rab_info_ptr->rb_InformationSetupList.elem[rb_index];
      if(rrc_find_rb_in_est_rabs(rb_info_ptr->rb_Identity) == RRC_RB_PRESENT) 
      {
        WRRC_MSG1_HIGH("REL8:RB id %d already exist in est rabs: Invalid config", rb_info_ptr->rb_Identity);
        rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        return FAILURE;
      }

      if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_ptr,rrc_RB_InformationSetup_r8,pdcp_Info))
      {
#ifdef FEATURE_UMTS_PDCP
        if (rrc_pdcp_enabled)
        {
          /*If the RAB is for cs domain, send RBS failure with cause invalid configuration*/
          if (rab_info_ptr->rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain)
          {
            WRRC_MSG0_HIGH("REL8:PDCP info invalid for CS RAB");
            rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
            return FAILURE;
          }

          if(rab_info_ptr->rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain)
          {
            if(rrcllc_validate_pdcp_info_in_rb_info_setup_list_r8(rb_info_ptr) == FALSE)
            {
              rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
              WRRC_MSG0_ERROR("REL8:PDCP Validation Failed!");
              return FAILURE;
            }
          }
        }
#endif /*FEATURE_UMTS_PDCP*/
      } 

      if(*new_tm_rb == FALSE)
      {
        dl_tm_flag = FALSE;
        if((RRC_CHECK_COMMON_MSG_TYPE(rb_info_ptr->rlc_InfoChoice,rrc_RLC_InfoChoice_r7_rlc_Info)) &&
          (((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_ptr->rlc_InfoChoice.u.rlc_Info,
             rrc_RLC_Info_r7,ul_RLC_Mode))&&
             (rb_info_ptr->rlc_InfoChoice.u.rlc_Info->ul_RLC_Mode.t == 
             T_rrc_UL_RLC_Mode_ul_TM_RLC_Mode))) &&
             ((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,dl_RLC_Mode))&&
             (RRC_CHECK_COMMON_MSG_TYPE(rb_info_ptr->rlc_InfoChoice.u.rlc_Info->dl_RLC_Mode,
             rrc_DL_RLC_Mode_r7_dl_TM_RLC_Mode))))
        {
          *new_tm_rb = TRUE;
        }
        else if(RRC_CHECK_COMMON_MSG_TYPE(rb_info_ptr->rlc_InfoChoice,rrc_RLC_InfoChoice_r7_same_as_RB))
        {
          local_lc_id = rrclcm_get_dl_rlc_lc_id(rb_info_ptr->rlc_InfoChoice.u.same_as_RB);
          if(local_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
          {
            result = rrclcm_get_dl_rlc_mode(local_lc_id, &rx_mode);
            if((result == RRCLCM_SUCCESS) && (rx_mode == UE_MODE_TRANSPARENT))
            {
              dl_tm_flag = TRUE;
            }
          }

          local_lc_id = rrclcm_get_ul_rlc_lc_id(rb_info_ptr->rlc_InfoChoice.u.same_as_RB);
          if(local_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
          {
            result = rrclcm_get_ul_rlc_mode(local_lc_id, &rx_mode);
            if((dl_tm_flag == TRUE) && (result == RRCLCM_SUCCESS) && (rx_mode == UE_MODE_TRANSPARENT))
            {
              *new_tm_rb = TRUE;
            }
          }
        }
      }
          
      
      /*point to the next link */
      rb_index++;
    } while (rab_info_ptr->rb_InformationSetupList.n >  rb_index);

    /* Go to next RAB setup infomation  */
    rab_idx++;
  } while (rab_list_ptr->n > rab_idx);

  return SUCCESS;
}

/*===========================================================================
FUNCTION   rrcrbe_validate_rab_setup_list_r8

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static uecomdef_status_e_type rrcrbe_validate_rab_setup_list_r8
(
  rrc_RAB_InformationSetupList_r8 * rab_list_ptr,
  uint32 no_of_rabs
 #ifdef FEATURE_VOC_AMR_WB
  ,mvs_mode_type nas_sync_ind
 #endif /*FEATURE_VOC_AMR_WB*/  
)
{
  rrc_rab_id_type rab_id;           /* To store RAB id in internal RRC format */
  
  /* To store the status returned by MN layer. This indicates what type of RAB is received. This is
  used only if the RAB is for the CS_domain */
  rrcmn_rab_status_T rab_type = RRCMN_INVALID_RAB_ID;     
  
  /* Used as an index when storing established RABs locally. Only when we have more than
  one RAB per message, this will be incremented */
  uint32 rab_index=0;
  uint8 rab_list_index=0;  /* rab_list_index keeps track of rrc_RAB_InformationSetupList_r8*/

  boolean voice_rab_already_exists = FALSE;
    
  for (rab_index= no_of_rabs; (rab_list_ptr->n > rab_list_index) && (rab_index < RRCRB_MAX_RAB_PER_MSG)
                              && (rab_list_index < RRCRB_MAX_RAB_PER_MSG); rab_index++,rab_list_index++)
  {
    /* GSM_MAP RAB Id. First convert it to internal format. */      
    rab_id = rrcrb_translate_gsm_map_rab_id((rab_list_ptr->elem[rab_list_index].rab_Info.
                                                  rab_Identity.u.gsm_MAP_RAB_Identity));
    
    /* Store this info locally so that ESTABLISHED_RABS can
    be updated later, when these RABs are established */
    rbe.rabs.rabs_in_msg[rab_index].rab_id_is_gsm_map = TRUE;
    rbe.rabs.rabs_in_msg[rab_index].rab_id = rab_id;
    rbe.rabs.rabs_in_msg[rab_index].re_est_timer =
    rab_list_ptr->elem[rab_list_index].rab_Info.re_EstablishmentTimer;
    /* Check if this RAB is CS Domain or PS Domain */
    if ( rab_list_ptr->elem[rab_list_index].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain )
    {
      /* Store the domain identity. This will be later added to ESTABLISHED_RABS */
      rbe.rabs.rabs_in_msg[rab_index].cn_domain = RRC_CS_DOMAIN_CN_ID;
    
      /* For CS RABs, Call MN layer to verify this RAB */
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
      rab_type = rrcmn_verify_rab_subs((radio_access_bearer_T)rab_id,rrc_get_as_id());
#else
      rab_type = rrcmn_verify_rab((radio_access_bearer_T)rab_id);
#endif
      MSG_HIGH("For RAB-Id %d, RAB type from NAS = %d, RAB type from EST_RABs = %d",rab_id, rab_type,
        rrc_get_rab_type_for_given_rab_id(rbe.rabs.rabs_in_msg[rab_index].rab_id));
    
      switch ( rab_type )
      {
        case RRCMN_CS_DATA_CALL:
    
          MSG_LOW("Received a CS Data RAB Id %d", rab_id, 0, 0);
          break;
    
        case RRCMN_CS_VOICE_CALL:

          MSG_LOW("Received a CS Voice RAB Id %d", rab_id, 0, 0);
          {
            if (rrc_find_if_voice_rab_exists_in_established_rabs( rbe.rabs.rabs_in_msg[rab_index].rab_id)
                 == TRUE)
            {
              WRRC_MSG1_HIGH("REL8:Voice RAB Id - %d already exists", rbe.rabs.rabs_in_msg[rab_index].rab_id );

              /*First condition of "if" covers the case when third RB is being added for AMR call*/
              /*Second condition of "if" covers the case when previously established Voice Call was of type CS over HSPA.*/
              if( rab_list_ptr->elem[rab_list_index].rb_InformationSetupList.n == 1)
              {
                voice_rab_already_exists = TRUE;
              }
              else
              {
#ifdef FEATURE_SCUDIF_ENHANCEMENT
                #error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/              
                rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                WRRC_MSG0_HIGH("REL8:Invalid configuration .. more than one RAB subflow is being added");
                return FAILURE;
              }
            }
            /*The given RAB isn't an existing Voice RAB.*/
            else
            {
              /* Whenever a new voice RAB is to be set up, at least two RBs need to be associated with that RAB.
              Therefore, first condition of "if" checks for presence of more than 1 RB. And the second condition
              of "if" ensures that it's for CS over DCH.*/
              if ( rab_list_ptr->elem[rab_list_index].rb_InformationSetupList.n == 1)
              {
                WRRC_MSG0_HIGH("REL8:Not enough RBs mapped to voice RAB!");
                rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
              }
            }
          }          
          break;
    
        default:
          WRRC_MSG1_ERROR("REL8:CS RAB validation failed! RABid %d",rab_id);
          break;
      } /* end switch */
    
      if ( (rbe.status.failure_status == RRCRB_MSG_INVALID_CONFIG) ||
         (rbe.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG))
      {
        /* No need to continue */
        return FAILURE;
      }

#ifdef FEATURE_VOC_AMR_WB
      if((rab_type == RRCMN_CS_VOICE_CALL )
#ifdef FEATURE_SCUDIF_ENHANCEMENT
#error code not present
#endif/*FEATURE_SCUDIF_ENHANCEMENT*/
      )
      {
        if(rab_list_ptr->elem[rab_list_index].rab_Info.m.nas_Synchronisation_IndicatorPresent)
        {
          nas_sync_ind =  rrc_translate_nas_sync_indicator(
                                 &(rab_list_ptr->elem[rab_list_index].rab_Info.nas_Synchronisation_Indicator));
          if( nas_sync_ind == MVS_MODE_NONE )
          {
            rbe.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
            WRRC_MSG1_HIGH("REL8:Unsupported configuration .. codec not supported %d",rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator );
            return FAILURE;
          }          
        }
        rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator =nas_sync_ind;        
#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
        rrc_set_ordered_codec_status(TRUE);
#endif
      }
#endif /*FEATURE_VOC_AMR_WB*/
    }

    /* Else check for PS Domain */
    else if ( rab_list_ptr->elem[rab_list_index].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain )
    {
      /* Store the domain identity. This will be later added to ESTABLISHED_RABS */
        rbe.rabs.rabs_in_msg[rab_index].cn_domain = RRC_PS_DOMAIN_CN_ID;
    
      /* For PS Domain, if it's a loopback, then no need for query, otherwise, call RABM module to verify the RAB */
      if ( rrclbt_is_lb_on() == TRUE ||rabm_rabid_validate( RAB_ESTABLISH_QUERY, (uint8)rab_id )
           == TRUE )
      {
        MSG_LOW("PS RAB Id %d to be setup", rab_id, 0, 0);
      }
      else
      {
        WRRC_MSG1_ERROR("REL8:PS RAB validation failed! RABid %d",rab_id);
        rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
        return FAILURE;
      }
    }
    
    if(voice_rab_already_exists == TRUE)
    {
      rbe.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_VOICE_RAB;
      WRRC_MSG0_HIGH("REL8:Updated RAB type as voice");
    }
    else
    {
      /* ASN1 structure 'rrc_RAB_InformationSetup' is validated at this point,
          hence update rab_type in structure 'rbe.rabs.rabs_in_msg[rab_index]'. */
          /* change argument */
      (void) rrc_update_rab_type_in_rab_info_r8(&(rab_list_ptr->elem[rab_list_index]),
         &(rbe.rabs.rabs_in_msg[rab_index]));
    }
    
    WRRC_MSG4_HIGH("REL8:RAB_ID:%d,Domain:%d,re_est_tmr:%d, rab_type: %d",
             rbe.rabs.rabs_in_msg[rab_index].rab_id,
             rbe.rabs.rabs_in_msg[rab_index].cn_domain,
             rbe.rabs.rabs_in_msg[rab_index].re_est_timer,
             rbe.rabs.rabs_in_msg[rab_index].rab_type);
#ifdef FEATURE_VOC_AMR_WB
    /* set the ordered codec mode which is sent in the ordered config to MAC.
      * If the RBE msg is not for Voice RAB also then also the current codec value
      * is passed to MAC 
      */
    rrc_set_ordered_codec_mode(nas_sync_ind);
    rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator = nas_sync_ind;
#endif /*FEATURE_VOC_AMR_WB*/
    
    /* Go to next RAB info  */
  }
  if (rab_list_ptr->n > rab_index)
  {
    WRRC_MSG0_HIGH("REL8:Too many RABs in an RBSetupMessage");
    rbe.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;

    return FAILURE;
  }
  
  rbe.rabs.num_rabs_in_msg = rab_index;
  return SUCCESS;
}

/*===========================================================================
FUNCTION   RRCRBE_SEND_CHANNEL_CONFIG_REQ_R8

DESCRIPTION
  This function sends the RRC_CHANNEL_CONFIG_REQ command to LLC
  to setup the necessary RBs associated with a RAB. If signalling
  Radio bearers are affected and included in the Radio Bearer
  Setup Message, they will also be sent in the same command.

  Currently the messege is not expected to have Signalling
  Radio Bearers for setup. If these are added in the future,
  they will also need to be sent in the same command.

DEPENDENCIES
  ORDERED_CONFIG must be set before calling this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void rrcrbe_send_channel_config_req_r8
(
  rrc_RadioBearerSetup *msg_ptr      /* Pointer to the RB Setup message */
)
{
  rrc_cmd_type *out_cmd_ptr;  /* To hold the Command                       */
  uint32 rb_count=0;          /* To count the number of RBs in the command */
  uint32 rbe_rb_cnt = 0;

  uint32 rab_index=0;         /* Used when more than one RAB is received in a RBS message */
  uint32 srb_cnt=0;
  rrc_RB_Identity rb_id=0;

  rrc_RB_InformationSetupList_r8 *rb_ptr = NULL; /* Temporary pointer to RB info      */
  rrc_RB_InformationAffectedList_r8 *affected_rb_ptr = NULL;  /* Pointer to RBs to be Reconfigured */
  rrc_RB_InformationReconfigList_r8 *reconfig_rb_ptr = NULL;
  rrc_RAB_InformationSetupList_r8 *rab_info_ptr = NULL;
  rrc_SRB_InformationSetupList_r8        * srb_setup_ptr = NULL;
  rrcrb_directed_cell_info_type        directed_cell_info; /* directed cell info struct */

  rrc_RadioBearerSetup_r8_IEs* rbsetup_ptr = NULL;
  rrc_state_e_type next_rrc_state = rbe.state_from_msg;

  /* Fill in the command id  */
  rrc_cmd.cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;

  /* Set RBSetup Ptr */
  rbsetup_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
    criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.radioBearerSetup_r8;


  /* First fill up the Radio Bearers that need to be setup. For this we need to know how many RBs need to be setup.
  If RABs need to be setup, check how many RBs are mapped to RABs. */
  if(RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,srb_InformationSetupList))
  {
    /* Get the SRB setup list */
    srb_setup_ptr = &rbsetup_ptr->srb_InformationSetupList;

    while ((srb_setup_ptr->n > srb_cnt) && (srb_cnt < UE_MAX_SRB_SETUP))
    {
      /* Check to see if the RB ID is there ... */
      if (srb_setup_ptr->elem[srb_cnt].m.rb_IdentityPresent)
      {
        rb_id = srb_setup_ptr->elem[srb_cnt].rb_Identity;
      }
      /*If not, then start with the first SRB and get the next SRB ID each time one is not included in the IE.*/
      else
      {
        /* Get the next SRB, note this increments to 1 the first time */
        rb_id++;
      }
  
      /* Fill up the RB id and indicate that it needs to be established */
      rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = rb_id;
      if(rrclcm_get_dl_rlc_lc_id(rb_id) == RRCLCM_RLC_LC_ID_NOT_FOUND )
      {
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = ESTABLISH_RB;
      }
      else
      {
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB_OTA;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = 
        rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = MODIFY_RB_CFG;
      }
      rb_count++;
      srb_cnt++;


    } /* end SRB setup */
  }

  if(RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,rab_InformationSetupList))
  {
    rab_info_ptr = &rbsetup_ptr->rab_InformationSetupList;
    for (rab_index=0; rab_index < rbe.rabs.num_rabs_in_msg && rab_info_ptr->n > rab_index; rab_index++)
    {
      if ( rbe.rabs.chan_config_needed[rab_index] == TRUE)
      {
        rb_ptr = &rab_info_ptr->elem[rab_index].rb_InformationSetupList;

        if ( rb_ptr->n == 0 )
        {
          WRRC_MSG0_HIGH("REL8:No RBs present in msg!");
        }
        else
        {
          /* Get the RBE db rab index */
          uint8 rbe_rab_index = rrcrbe_get_rab_index_from_rbe_db(
                                   rrcrb_translate_gsm_map_rab_id(rab_info_ptr->elem[rab_index].rab_Info.rab_Identity.u.gsm_MAP_RAB_Identity));
          rbe_rb_cnt = 0;
          if((rbe_rab_index < RRCRB_MAX_RAB_PER_MSG) && (rbe_rab_index < rbe.rabs.num_rabs_in_msg ))
          {
            while ( rb_ptr->n > rbe_rb_cnt )
            {              
              /* Fill up the RB id and indicate that it needs to be established */
              rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = rb_ptr->elem[rbe_rb_cnt].rb_Identity;
              rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = ESTABLISH_RB;

              /* Update the local structure so that we know which RB id's we've requested to be setup */
              rbe.rabs.rabs_in_msg[rbe_rab_index].rb_for_rab[rbe_rb_cnt].rb_id = rb_ptr->elem[rbe_rb_cnt].rb_Identity;

              /* Increment count of RBs */
              rb_count++;
              rbe_rb_cnt ++;
    
              /* Do a safety check on the count. We can't do more than 32
                       configs in a command */
              if ( (rb_count >= MAX_RB) || (rbe_rb_cnt >= MAX_RB_PER_RAB))
              {
                WRRC_MSG0_HIGH("REL8:MAX RBs per RAB count exceeded!");
                break;
              }
    
              /* Go to the next RB */
            }/* end while */
            /* Save the number of RBs for this RAB */
            rbe.rabs.rabs_in_msg[rbe_rab_index].num_rbs_for_rab = rbe_rb_cnt;
          }
          else
          {
            WRRC_MSG1_ERROR("RBE RAB INDEX not found!!! RAB_INDEX %d",rbe_rab_index);
          }
        }
      }/* end if rabs_setup_needed */
    }
  }
  /* Now check if the message contains an RB information to be
     reconfig list. If so, we need to reconfigure some RBs */
  if(RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,rb_InformationReconfigList))
  {
    uint32 local_idx=0;
  
    reconfig_rb_ptr = &rbsetup_ptr->rb_InformationReconfigList;
  
    while ( reconfig_rb_ptr->n > local_idx )
    {
  
      /* Fill up the RB id and indicate that it needs to be reconfigured */
      rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = reconfig_rb_ptr->elem[local_idx].rb_Identity;
      rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB;
      rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB_OTA;
      rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = 0;
      rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = 0;
      if((reconfig_rb_ptr->elem[local_idx].m.rlc_InfoPresent)&&
         (reconfig_rb_ptr->elem[local_idx].rlc_Info.m.ul_RLC_ModePresent))
      {
        rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = MODIFY_RB_CFG;
      }
      if((reconfig_rb_ptr->elem[local_idx].m.rlc_InfoPresent)&&
         (reconfig_rb_ptr->elem[local_idx].rlc_Info.m.dl_RLC_ModePresent))
      {
        rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = MODIFY_RB_CFG;
      }
  
      if (reconfig_rb_ptr->elem[local_idx].m.rb_StopContinuePresent)
      {
        /* over-ride rb_config with STOP or continue */
        if (rrc_RB_StopContinue_stopRB == reconfig_rb_ptr->elem[local_idx].rb_StopContinue)
        {
          rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask |= STOP_RB_CFG;
          rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask |= STOP_RB_CFG;
        }
        else if (rrc_RB_StopContinue_continueRB == reconfig_rb_ptr->elem[local_idx].rb_StopContinue)
        {
          rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask |= CONTINUE_RB_CFG;
          rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask |= CONTINUE_RB_CFG;
        }
      }
  
      /* Increment count of RBs. This is done here since we
      need to use the next index that's not used already. */
      rb_count++;
  
      /* Go to the next affected RB */
      local_idx++;
  
      /* Do a safety check on the count. We can't do more than 32
      configs in a command */
      if ( rb_count >= MAX_RB )
      {
        WRRC_MSG0_ERROR("REL8:MAX RB count exceeded!");
        break;
      }
    } /* end while */
  }
  
  /* Now check if the message contains an RB information to be
   affected list. If so, we need to reconfigure some RBs */
  if(RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,rb_InformationAffectedList))
  {
   uint32 local_idx=0;
  
    affected_rb_ptr = &rbsetup_ptr->rb_InformationAffectedList;
  
    if ( affected_rb_ptr->n == 0 )
    {
      WRRC_MSG0_HIGH("REL8:No affected RBs in msg!");
    }
    else
    {
      while ( affected_rb_ptr->n > local_idx )
      {
  
        /* Fill up the RB id and indicate that it needs to be reconfigured */
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = affected_rb_ptr->elem[local_idx].rb_Identity;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB_OTA;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = MODIFY_RB_CFG;
  
        /* Increment count of RBs. This is done here since we
        need to use the next index that's not used already. */
        rb_count++;
  
        /* Go to the next affected RB */
        local_idx++;
  
        /* Do a safety check on the count. We can't do more than 32
        configs in a command */
        if ( rb_count >= MAX_RB )
        {
          WRRC_MSG0_HIGH("REL8:MAX RB count exceeded!");
          break;
        }
      } /* end while */
    }
  }
  else
  {
    MSG_LOW("No affected RB list in msg", 0, 0, 0);
  }/* end if affected list present */


  /* Number of RBs to config. Note rb_count is the total
   of established and reconfigured RBs */
  rrc_cmd.cmd.chan_config_req.num_rb_to_config = (int) rb_count;

  if(rb_count < MAX_RB)
  {
    rrc_fill_rb_id_chan_req_from_standalone_db(&rb_count, &rrc_cmd.cmd.chan_config_req);
  }

  if(rbe.state_from_msg == RRC_STATE_CELL_DCH)
  {
     /* Fill those RBs in chan_config_req for which RLC PDU size has changed and 
     that are already not present in chan_config_req*/
    if(rb_count < MAX_RB)
    {
      rrc_fill_rb_id_chan_req_from_rb_list_mapped_to_dch(&rb_count, &rrc_cmd.cmd.chan_config_req);
    }
    if(rrc_get_state() == RRC_STATE_CELL_FACH)
    {
      rrc_fill_srb_in_chan_config_req(&rb_count, &rrc_cmd.cmd.chan_config_req);
    }
  }
 
  /*If state transition is Cell_DCH->Cell_FACH or vice-verca, add all the existing 
  AM PS RBs in chan_config_req*/	
  if(((next_rrc_state == RRC_STATE_CELL_FACH) && 
       (transition_config.toc_usage == TOC_FOR_DCH_AND_FACH ||
        transition_config.toc_usage == TOC_FOR_FACH_AND_PCH ||
        transition_config.toc_usage == TOC_FOR_FACH_AND_PCH_WITH_DCH_INFO))
       ||
      ((rrc_get_state() == RRC_STATE_CELL_FACH) && (next_rrc_state == RRC_STATE_CELL_DCH))
    )
  {
    rrc_fill_rb_id_chan_req_from_est_rabs(&rb_count, &rrc_cmd.cmd.chan_config_req);
  }
  /* Check if state change is required */
  if ( (rrc_get_state()) == (rbe.state_from_msg) )
  {
    rrc_cmd.cmd.chan_config_req.rrc_state_change_required = FALSE;
  }
  else
  {
    /* The message indicates that state change is required.
    Indicate to LLC what the next state should be. */
    rrc_cmd.cmd.chan_config_req.rrc_state_change_required = TRUE;
    rrc_cmd.cmd.chan_config_req.next_state = rbe.state_from_msg;
  }

  /* Indicate the id of the procedure sending the command */
  rrc_cmd.cmd.chan_config_req.procedure = RRC_PROCEDURE_RBE;

  /* Indicate that we need a confirmation for this command */
  rrc_cmd.cmd.chan_config_req.rrc_channel_config_cnf_required = TRUE;

  /* We are not required to fill any other data for this command.
   Now, send the command */
  if (((rrc_get_state()) == RRC_STATE_CELL_DCH) &&
      (rrc_cmd.cmd.chan_config_req.next_state== RRC_STATE_CELL_FACH) &&
      (rrc_cmd.cmd.chan_config_req.rrc_state_change_required))
  {
    /*Initiate the cell selection*/
    rrcrb_send_initiate_cell_selection_req(RRC_PROCEDURE_RBE,
            RRC_TRANSITION_FROM_DCH, rbe.state_from_msg);

    rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

    /* Start T_305 [periodic cell update] timer */
    rrccu_start_t_305_timer();
  }
  else
  {
    /*check whether any frequency info is present in the message
    and transition is CELL_FACH->CELL_FACH. In this case
    initiate a cell update after channel config is successfully completed */
    if (((rrc_get_state())==RRC_STATE_CELL_FACH)&&
        (rbe.state_from_msg==RRC_STATE_CELL_FACH))
    {
      if((RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,frequencyInfo)) &&
          (rbsetup_ptr->frequencyInfo.modeSpecificInfo.t == T_rrc_FrequencyInfo_modeSpecificInfo_fdd))
      {
        directed_cell_info.freq_info_present = TRUE;
        directed_cell_info.uarfcn_dl = rbsetup_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
      }
      else
      {
        directed_cell_info.freq_info_present = FALSE;
      }
      if ((RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,dl_InformationPerRL_List)) &&
          (rbsetup_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.t == 
          T_rrc_DL_InformationPerRL_r8_modeSpecificInfo_fdd))
      {
        directed_cell_info.pscr_present = TRUE;
        directed_cell_info.pscr = rbsetup_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
      }
      else
      {
        directed_cell_info.pscr_present = FALSE;
      }
      if (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE)
      {
        /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection 
          FACH->FACH ), treat it as un-supported configuration and send failure.*/ 
        if(rrccsp_check_initiate_cell_selection_handling(NULL) == FALSE)
        {
          WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
       
          rrcrbe_send_initiate_cu_flag = TRUE;
          rrcrbe_delete_crnti          = TRUE;
        }
        else
        {
          rrc_int_c_rnti_type      c_rnti;       /* Local C_RNTI value */
          if(RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,new_C_RNTI))
          {
            rbe.new_crnti_valid = TRUE;
            rrc_translate_crnti(&rbsetup_ptr->new_C_RNTI,
                                &c_rnti);
            rbe.new_crnti = (uint16)c_rnti;
          }
          else
          {
            rbe.new_crnti_valid = FALSE;
          }
          rrcllcoc_update_toc_with_oc_from_fach();
          
          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req(RRC_PROCEDURE_RBE,
                                                  RRC_TRANSITION_FROM_FACH,
                                                  rbe.state_from_msg);
       
          rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
              
          return;
        }
      }

    } /* if CELL_FACH->CELL_FACH reconfiguration */

    /* Get a Command buffer and copy the local command to the buffer so as to 
    enqueue it on the internal queue. */
    if ((out_cmd_ptr = rrc_get_int_cmd_buf())!=NULL)
    {
      WCDMA_MEMCPY(out_cmd_ptr,
                   sizeof(rrc_cmd_type),
                   &rrc_cmd,
                   sizeof(rrc_cmd_int_type));
      rrc_put_int_cmd( out_cmd_ptr );
    }
  }
}

/*===========================================================================
FUNCTION   VALIDATE_RBSETUP_R8

DESCRIPTION
  This function validates the received R8 RB Setup Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a Radio Bearer Setup Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void validate_rbsetup_r8
(
  rrc_RadioBearerSetup *msg_ptr      /* Pointer to the RB Setup message */
)
{
  rrc_RadioBearerSetup_r8_IEs* rbsetup_ptr = NULL;

  boolean new_tm_rb = FALSE;

#ifdef FEATURE_VOC_AMR_WB
  mvs_mode_type nas_sync_ind=rrc_get_current_codec_mode();
#endif /*FEATURE_VOC_AMR_WB*/

  /* Assume the message is valid for now */
  rbe.status.failure_status = RRCRB_MSG_VALID;

  rbsetup_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
    u.criticalExtensions.u.criticalExtensions.u.r8.radioBearerSetup_r8;

  switch (rbsetup_ptr->rrc_StateIndicator)
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      rbe.state_from_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      rbe.state_from_msg = RRC_STATE_CELL_FACH;
      break;

    default: /* Just to satisfy LINT */
      WRRC_MSG0_ERROR("REL8:Invalid RRC state from Msg");
      rbe.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
      break;

  } /* End of switch */

  if ( rbe.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
    return;
  }

  /* Check if there are new RABs to be established */
  if(RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,rab_InformationSetupList))
  {
    WRRC_MSG0_HIGH("REL8:RAB Info Setup List present.");
    if(rrcrbe_prevalidate_rab_setup_list_r8(&rbsetup_ptr->rab_InformationSetupList,&new_tm_rb)== FAILURE)
    {
      return;
    }
  }
  else
  {
    /*There is no RAB to be setup in this message.*/
    rbe.rabs.num_rabs_in_msg = 0;
  }  

  if(RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,dl_CounterSynchronisationInfo))
  {
    /*check whether the SRNS relocation is within Cell_DCH or not.
      SRNS relocation information can come in reconfiguration messages
      only when the state transition is Cell_DCH->Cell_DCH */
    if (!((rbe.state_from_msg == RRC_STATE_CELL_DCH) &&
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("REL8:SRNS reloc with invalid states cur_state %d, msg_state %d",
          rrc_get_state(), rbe.state_from_msg);
      rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
      return;
    }

    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has
       PDCP info list. If it is true then this configuration is unsupported */
    if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2(rbsetup_ptr->dl_CounterSynchronisationInfo,
		rb_WithPDCP_InfoList)) ||
        (RRC_MSG_COMMON_BITMASK_IE(rbsetup_ptr->dl_CounterSynchronisationInfo, 
        rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
    {
      WRRC_MSG0_HIGH("REL8:PDCP info in DL counter sync info not supported");
      rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation.
      Set the flag that indicates that this procedure
      involves SRNS relocation */
      rbe.rbe_for_srns_relocation = TRUE;
      /* store if new U-RNTI is present in the message */
      if (RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,new_U_RNTI))
      {
        rbe.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(rbsetup_ptr->new_U_RNTI), &rbe.new_urnti);
      }
    }
  }

  /* Process RAB reconfig before RAB setup because we want to catch NW's which
    are trying to setup and reconfig RAB with the same message */
  if(RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,rab_InformationReconfigList))
  {

    rbe.status.failure_status = rrc_update_rab_info_reconfig_param_r8(
    &rbsetup_ptr->rab_InformationReconfigList, &rbe.rabs 
#ifdef FEATURE_VOC_AMR_WB
                                           ,&nas_sync_ind
#endif/*FEATURE_VOC_AMR_WB*/
                                         );
    if( rbe.status.failure_status != RRCRB_MSG_VALID) 
    {
      return;
    }
  }

  if(RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,rab_InformationSetupList))
  {
    if(rrcrbe_validate_rab_setup_list_r8(&rbsetup_ptr->rab_InformationSetupList, 
        rbe.rabs.num_rabs_in_msg
#ifdef FEATURE_VOC_AMR_WB          
        ,nas_sync_ind
#endif /*FEATURE_VOC_AMR_WB*/          
        ) == FAILURE)
    {
      return;
    }
  }

  WRRC_MSG1_MED("REL8:%d RABs  in an RBSetup",  rbe.rabs.num_rabs_in_msg);

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch */
  rbe.trans_to_dch_ciph_config_needed = FALSE;

  if((rbe.state_from_msg == RRC_STATE_CELL_DCH) && (rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,dl_CommonInformation)) && 
       (RRC_MSG_COMMON_BITMASK_IE(rbsetup_ptr->dl_CommonInformation,
       rrc_DL_CommonInformation_r8,dl_dpchInfoCommon)) &&
       (T_rrc_DL_CommonInformation_r8_dl_dpchInfoCommon_dl_DPCH_InfoCommon
       == rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.t))
    {
      //check if ciphering is active in mac for cs domain, if it is then,
      //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      if(rrcsmc_tm_ciph_active() == TRUE)
      {
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        uecomdef_status_e_type loc_ret_val;
        WRRC_MSG0_HIGH("REL8:To-DCH-Step-1 config activated");
        loc_ret_val = rrcrb_get_mac_d_hfn(
             ( RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon,rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
              &rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
              &mac_d_hfn_l);
        if (loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("REL8:get_mac_d_hfn failed");
        }
        if(rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
        {
           WRRC_MSG0_HIGH("REL8:SMC process ciphering config for HHO failed");
        }
        rbe.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("REL8:To-DCH-Step-1 not needed");
      }
    }
  }

  if(rbe.state_from_msg == RRC_STATE_CELL_DCH)
  {
    rbe.first_tm_rb_setup = rrcrbe_is_first_tm_rb_setup(new_tm_rb);
    if(new_tm_rb)
    {
      WRRC_MSG1_HIGH("First TM RB being set status : %d",rbe.first_tm_rb_setup);
    }
    if(rbe.first_tm_rb_setup == TRUE)
    {
      //check to see if dl-dpch-common info is present while going to dch
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
         rrc_RadioBearerSetup_r8_IEs,dl_CommonInformation)) && 
         (RRC_MSG_COMMON_BITMASK_IE(rbsetup_ptr->dl_CommonInformation,
         rrc_DL_CommonInformation_r8,dl_dpchInfoCommon)) &&
         (T_rrc_DL_CommonInformation_r8_dl_dpchInfoCommon_dl_DPCH_InfoCommon == 
          rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.t))
      {
        uecomdef_status_e_type loc_ret_val;
        //check if ciphering is active in mac for cs domain, if it is then,
        //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        
        WRRC_MSG0_HIGH("First time TM RB Setup HHO Step-1 config activated");
        loc_ret_val = rrcrb_get_mac_d_hfn(
             ( RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon,rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
              &rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
              &mac_d_hfn_l);
        if (loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("get_mac_d_hfn failed");
        }
        if(rrcsmc_process_cipher_config_for_first_tm_setup(mac_d_hfn_l) == FAILURE)
        {
          WRRC_MSG0_HIGH("SMC process ciphering config for First TM RB Setup failed");
          /*This step is reached only when the first step returns a failure,. i.e, if the ciphering is not active for CS domain.
            Hence setting rbe.first_tm_rb_setup to FALSE so that step 2 is not done*/
            rbe.first_tm_rb_setup = FALSE;
        }

      }
    }
  }

  /* If UTRAN DRX coefficient is not present then it is invalid configuration */
  if ((rbe.state_from_msg == RRC_STATE_CELL_PCH) ||
      (rbe.state_from_msg  == RRC_STATE_URA_PCH))
  {

    if (!(RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,utran_DRX_CycleLengthCoeff)))
    {
      WRRC_MSG1_ERROR("REL8:UTRAN DRX Coef not preset, State:%d", rbe.state_from_msg);
      rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }

  if(RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,rb_InformationReconfigList))
  {
    /* Save the linked list head pointer */
    if (FAILURE == rrcllc_validate_rb_info_reconfig_list_r8(&rbsetup_ptr->rb_InformationReconfigList))
    {
      rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }

  if (rbe.state_from_msg == RRC_STATE_URA_PCH)
  {
    /*should we do it after getting l2ack for response??*/
    if (!(RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,ura_Identity)))
    {
      WRRC_MSG0_HIGH("REL8:URA ID not present");
      rrccu_clear_ura_id ();
    }
    else
    {
      /*URA ID present in the message set ura_identity variable with this value */
      if (rbsetup_ptr->ura_Identity.numbits == 16)
      {
        rrccu_update_ura_id (&(rbsetup_ptr->ura_Identity));
      }
      else
      {
        WRRC_MSG1_ERROR("REL8:Invalid URA ID length%d", rbsetup_ptr->ura_Identity.numbits);
        rbe.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
        rbe.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
        return;
      }
    }
  }

  return;
}

/*===========================================================================
FUNCTION   PROCESS_RBSETUP_R8

DESCRIPTION
  This function processes the Rel 8 RB Setup Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a Radio Bearer Setup Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -

    ORDERED_CONFIG will be set by calling LLC.

    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to configure the Radio Bearers given by the message.

    If ORDERED_CONFIG could not be set, a Radio Bearer Setup Failure message
    will be sent to UTRAN.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void process_rbsetup_r8
(
  rrc_cmd_type *cmd_ptr
)
{
  rrc_DL_DCCH_Message     *dcch_msg_ptr;   /* Pointer to the RB Setup message */
  rrc_RadioBearerSetup *msg_ptr;    /* Pointer to the RB Setup message        */

  rrc_rab_search_e_type rab_status = RRC_RAB_NOT_FOUND; /* To check if RAB is present in ESTABLISHED_RABS */
  rrcllc_oc_status_e_type oc_status;/* To hold ordered_config status          */
  uint32 rab_index=0;               /* Used when more than 1 rab is present in
                                    a RBS message                             */
  rrc_int_c_rnti_type      c_rnti;       /* Local C_RNTI value */
  boolean ordered_config_err_occured = FALSE; /* Used to indicate if an error
                                                 occured while setting ordered_config. */
  rrc_msg_ver_enum_type msg_version = MSG_REL8;

  rrc_RadioBearerSetup_r8_IEs* rbsetup_ptr = NULL;

  rrcsmc_srns_ip_check_status_e_type    ip_check_status = RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE;


  WRRC_MSG1_MED("REL8:RRC cur_state %d", rbe.curr_substate);

  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  /* Get the pointer to the actual RB Setup message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerSetup);

  /* Store Msg Ptr in a local variable */
  rbsetup_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
    u.r8.radioBearerSetup_r8;

  if (TRUE == rrcrbe_validate_srns_relocation_info(msg_ptr, msg_version))
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
      WRRC_MSG0_ERROR("REL8:IP check failed. Discarding RBS msg");
      break;

    case RRCSMC_SRNS_SECURITY_INFO_INVALID:
    case RRCSMC_INCOMPATIBLE_SECURITY_RECONFIGURATION:
      if (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier
          == rbe.tr_ids.rejected_transaction)
      {
        WRRC_MSG1_HIGH("REL8:Duplicate RBE msg - rejected tr-id%d match!",
                 rbe.tr_ids.rejected_transaction);
        /* Nothing else to do, just ignore the message and return */
      }
      else
      {
        /* send a reconfig failure messge with invalid
        configuration and
          continue the same substate with any ongoing procedure */
        rbe.tr_ids.rejected_transaction =
        (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
        if (ip_check_status == RRCSMC_SRNS_SECURITY_INFO_INVALID)
        {
          rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        }
        else
        {
          rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
        }
        /*Check whether Cell Update is going on */
        if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
             (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
        {
          if(rbe.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
          {
            WRRC_MSG2_HIGH("rbe subsstate(%d) Send failure with %d cause", rbe.curr_substate, rbe.status.failure_status);
            send_rbs_failure_message( rbe.status.failure_status,
                                     rbe.status.prot_err_cause );
            rbe.status.failure_status = RRCRB_MSG_VALID;
          }
          else
          {
            WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
            (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBE);
            /* Wait until Cell update procedure is completed */
            rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
          }
        }
        else
        {
          send_rbs_failure_message( rbe.status.failure_status,
                                    rbe.status.prot_err_cause );
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
    if (ip_check_status != RRCSMC_SRNS_INTEGRITY_CHECK_SUCCESS)
    {
      /*do not clear the procedure or change the rbe substate because
      some other rbe procedure might be in progress. Just return from
        here */
      return;
    }

  } /* End of validation of srns relocation */

  if ((rrcllc_get_ordered_config_status() != OC_NOT_SET) ||
      (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
      (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
  {
    WRRC_MSG0_HIGH("REL8:Incomatible simultaneous reconfig! Rejecting RBS message");
    if ( (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier)
         == rbe.tr_ids.accepted_transaction )
    {
      WRRC_MSG1_HIGH("REL8:Duplicate RBS msg! accepted tr-id%d match!",
               rbe.tr_ids.accepted_transaction);
      /* Nothing else to do, just return */
    }
    else if ( (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier)
                == rbe.tr_ids.rejected_transaction )
    {
      WRRC_MSG1_HIGH("REL8:Duplicate RBS msg! rejected tr-id%d match!", rbe.tr_ids.rejected_transaction);
      /* Nothing else to do, just return */
    }
    else
    {
      /* We have received a RBS message with a new transaction id.
      This message should be in our rejected transaction
        list */
      rbe.tr_ids.rejected_transaction =
      (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
      rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;

      /*Check whether Cell Update is going on */
      if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
           (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
      {
        if(rbe.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
        {
          send_rbs_failure_message( rbe.status.failure_status,
                                   rbe.status.prot_err_cause );
          rbe.status.failure_status = RRCRB_MSG_VALID;
        }
        else
        {
          WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
          (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBE);
          /* Wait until Cell update procedure is completed */
          rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        }
      }
      else
      {
        send_rbs_failure_message( rbe.status.failure_status,
                                  rbe.status.prot_err_cause );
        /*continue the same substate */
      }
    }
    return;
  }

  /* We have received a RB setup message with a new transaction id.
  Accept the transaction Id */
  rbe.tr_ids.accepted_transaction =
  msg_ptr->u.later_than_r3.rrc_TransactionIdentifier;

  validate_rbsetup_r8(msg_ptr);

  if ( rbe.status.failure_status != RRCRB_MSG_VALID )
  {
    /* RB Message was invalid. */
    WRRC_MSG0_HIGH("REL8:RB Message validation failed");
    /* Send the RB Setup Failure Message */
    send_rbs_failure_message( rbe.status.failure_status, rbe.status.prot_err_cause);

    /* Make sure we're back in the initial sub-state */
    rbe.curr_substate = RRCRB_INITIAL;
    rrcrbe_clear_procedure();
  }
  /* If the message is valid, proceed further */
  else
  {
    rrcrbe_indicate_layer1_if_rb_setup_configure_voice_rab();

     /* If new rabs are setup, update mac rab status */
    if(RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,rab_InformationSetupList))
    {
      rrc_RAB_InformationSetupList_r8 *rab_list_ptr = &rbsetup_ptr->rab_InformationSetupList;
      rrcrb_update_mac_rab_status(RRC_PROCEDURE_RBE,
                                 (rrc_cn_domain_identity_e_type)(rab_list_ptr->elem[0].rab_Info.cn_DomainIdentity) ,
                                  NULL,FALSE);
    }

    /*calculate ciphering_activation_cfn only if ciphering is enabled*/
    if (rrcsmc_is_ciphering_enabled() == RRCSMC_SECURITY_STARTED)
    {
      if(RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,activationTime))
      {
        /*256 is the CFN rollover length*/
        /*250 is used to activate ciphering after 250 10ms time interval,
        this should give sufficient time for n/w side and ue side to have
        configured ciphering on the rb's successfully*/
        ciphering_activation_cfn =
        (uint8) ((rbsetup_ptr->activationTime + 250) % 256);
        /*round it to 8 frame boundary */
        ciphering_activation_cfn = (uint8)((ciphering_activation_cfn/8)*8);

      }
      else
      {
        /*256 is the CFN rollover length*/
        /*250 is used to activate ciphering after 250 10ms time interval,
        this should give sufficient time for n/w side and ue side to have
        configured ciphering on the rb's successfully*/
        ciphering_activation_cfn = (uint8)((seq_get_cfn() + 250) % 256);
        /*round it to 8 frame boundary */
        ciphering_activation_cfn = (uint8)((ciphering_activation_cfn/8)*8);
      }
    }

    /*call rrcsmc function to update ciphering activation cfn*/
    rrcsmc_update_ciphering_activation_cfn(ciphering_activation_cfn);

    /* Check if there is CN information present. */
    if(RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,cn_InformationInfo))
    {
      rbe.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(
        &(rbsetup_ptr->cn_InformationInfo) );
      if (rbe.mm_cmd_ptr == NULL)
      {
        WRRC_MSG0_ERROR("REL8:Could not build SERVICE_IND cmd");
      }
      else
      {
        /* set cn_info_saved flag */
        rbe.cn_info_saved = TRUE;
      }
    }

#ifdef FEATURE_WCDMA_REL7_SPECRS
    /*If RBE OTA provides "supportForChangeOfUE_Capability" IE, we need to store it in
           "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
          "rrc_rnc_cap_change_support" can be updated accordingly. */  	
    if(RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,supportForChangeOfUE_Capability))
    {
      if(rbsetup_ptr->supportForChangeOfUE_Capability == TRUE)
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
#endif  /*FEATURE_WCDMA_REL7_SPECRS*/

    /* Check if there are new RABs to be established or if there are
       affected RB or SRBs to be setup */

    if ((RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,rab_InformationSetupList)) ||
        (((RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,rb_InformationAffectedList))  
          ) ||
        ((RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,srb_InformationSetupList)) 
         )))
    {
      /* Check if the RAB id is already in ESTABLISHED_RABS */
      for (rab_index=0; rab_index < rbe.rabs.num_rabs_in_msg; rab_index++)
      {
        rab_status = rrc_find_rab_in_established_rabs(rbe.rabs.rabs_in_msg[rab_index].rab_id );
        rbe.rabs.chan_config_needed[rab_index] = TRUE;

        if ((rab_status != RRC_RAB_NOT_FOUND) &&
             (rrc_find_if_voice_rab_exists_in_established_rabs( rbe.rabs.rabs_in_msg[rab_index].rab_id )
               == FALSE))
        {
          WRRC_MSG1_HIGH("REL8:RAB %d already exists in EST_RABS!",
                   rbe.rabs.rabs_in_msg[rab_index].rab_id);
         /* send a failure message if the rab in the message  already
           exists in   established_rabs. Later add a check for the rbs
           associated with the rabs so that the failure will be sent only when
           any of the rb-ids match with the rb-ids in established_rab for that
           rab */
          rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
          /* Send the RB Setup Failure Message */
          send_rbs_failure_message( rbe.status.failure_status,
                                    rbe.status.prot_err_cause
                                    );

        /* Make sure we're back in the initial sub-state */
          rbe.curr_substate = RRCRB_INITIAL;
          rrcrbe_clear_procedure();
          return;
        }
      }

      /* First set ORDERED_CONFIG so that LLC can process the necessary parameters.
      This function will copy all RABs from the rxd msg to oc. */
      oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_RBE, rbe.state_from_msg,
                                             rrc_DL_DCCH_Message_PDU, (void *)dcch_msg_ptr);

      switch ( oc_status )
      {
        case ORDERED_CONFIG_SET:
          if(rrc_sib_is_current_event_sib_read_in_dch())
          {
             WRRC_MSG0_HIGH("REL8:RRCGPS: Force Init GPS ");
             rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
          }

          /* Ordered Config has been set. We can setup the
            Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */

          WRRC_MSG0_HIGH("REL8:Sending RRC_CHANNEL_CONFIG_REQ to LLC");
          rrcrbe_send_channel_config_req_r8(msg_ptr);

          if(((rrc_get_state()) == RRC_STATE_CELL_DCH) &&
              (rrc_cmd.cmd.chan_config_req.next_state== RRC_STATE_CELL_FACH) &&
              (rrc_cmd.cmd.chan_config_req.rrc_state_change_required))
          {
            if(RRCRB_R8_MSG_IE_PRESENT(rbsetup_ptr,rrc_RadioBearerSetup_r8_IEs,new_C_RNTI))
            {
              rbe.new_crnti_valid = TRUE;
              rrc_translate_crnti(&rbsetup_ptr->new_C_RNTI, &c_rnti);
              rbe.new_crnti = (uint16)c_rnti;
            }
            else
            {
              rbe.new_crnti_valid = FALSE;
            }
            rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
          }
          else
          {
            if(rbe.curr_substate != RRCRB_WAIT_FOR_CELL_SELECTION_CNF)
            {
               /*Change the substate since we're waiting for the confirmation from LLC for the channel configs */
               rbe.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
            }
          }
          break;

        case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

          rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
          /* Send the RB Setup Failure Message */
          send_rbs_failure_message( rbe.status.failure_status, rbe.status.prot_err_cause);

          /* Go back to initial substate */
          rbe.curr_substate = RRCRB_INITIAL;

          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;

        case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

          rbe.status.failure_status =RRCRB_MSG_UNSUPPORTED_CONFIG;
          /* Send the RB Setup Failure Message */
          send_rbs_failure_message( rbe.status.failure_status, rbe.status.prot_err_cause);

          /* Go back to initial substate */
          rbe.curr_substate = RRCRB_INITIAL;

          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;
          
          case ORDERED_CONFIG_CONFIGURATION_INVALID:
 
           rbe.status.failure_status =RRCRB_MSG_INVALID_CONFIG;
           /* Send the RB Setup Failure Message */
           send_rbs_failure_message( rbe.status.failure_status, rbe.status.prot_err_cause);
 
           /* Go back to initial substate */
           rbe.curr_substate = RRCRB_INITIAL;
 
           /* Indicate that an error occured */
           ordered_config_err_occured = TRUE;
           break;
        case ORDERED_CONFIG_NOT_SET_OTHER:
          WRRC_MSG0_HIGH("REL8:LLC returned other failure!");
          /* When LLC sends this, there is a serious problem
          and we'll go back to disconnected state. No need
          to send the failure message here, we can just
          go back to initial substate */
          rbe.curr_substate = RRCRB_INITIAL;

          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
        case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
          WRRC_MSG0_HIGH("RB setup processing deferred due to LTA!");

          rbe.curr_substate = RRCRB_INITIAL;

          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/          
        default:

          WRRC_MSG0_ERROR("REL8:Unknown return value from LLC");
          /* Unknown error, just go back to initial substate */
          rbe.curr_substate = RRCRB_INITIAL;

          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;
        }/* end switch */

        if ( ordered_config_err_occured == TRUE )
        {
          /* No need to continue. Clear the procedure
            variables and return. */
          rrcrbe_clear_procedure();
          return;
        }

        if (rbe.rbe_for_srns_relocation == TRUE)
        {
          rrcsmc_commit_fresh_for_srns_relocation();
        }
    }
    else
    {
      /*If we are here, it means that RABs are not given in RB-Setup.
      Also, SRBs and RB Affected IEs are not given.*/
      WRRC_MSG0_ERROR("REL8:No RB IEs are given in the OTA.");
      rbe.status.failure_status= RRCRB_MSG_INVALID_CONFIG;
      rbe.status.prot_err_cause = rrc_ProtocolErrorCause_informationElementMissing;  

      /* Send the RB Setup Failure Message */
      send_rbs_failure_message(rbe.status.failure_status, rbe.status.prot_err_cause);

      /* Make sure we're back in the initial sub-state */
      rbe.curr_substate = RRCRB_INITIAL;
      rrcrbe_clear_procedure();

      return;  
    }/* end if - RBs need to be reconfigured */
  } /* else if valid rb message */
}/* end function process_rbsetup_message_r6 */


/*===========================================================================

FUNCTION   RRCRBE_SEND_CHANNEL_CONFIG_REQ_R7

DESCRIPTION

  This function sends the RRC_CHANNEL_CONFIG_REQ command to LLC
  to setup the necessary RBs associated with a RAB. If signalling
  Radio bearers are affected and included in the Radio Bearer
  Setup Message, they will also be sent in the same command.

  Currently the messege is not expected to have Signalling
  Radio Bearers for setup. If these are added in the future,
  they will also need to be sent in the same command.

DEPENDENCIES

  ORDERED_CONFIG must be set before calling this function.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbe_send_channel_config_req_r7
(
rrc_RadioBearerSetup *msg_ptr      /* Pointer to the RB Setup message */
)
{
  rrc_cmd_type *out_cmd_ptr;  /* To hold the Command                       */
  uint32 rb_count=0;          /* To count the number of RBs in the command */
  uint32 rbe_rb_cnt = 0;

  uint32 rab_index=0;         /* Used when more than one RAB is received in a RBS message */
  uint32 srb_cnt=0;
  rrc_RB_Identity rb_id=0;

  rrc_RB_InformationSetupList_r7 *rb_ptr = NULL; /* Temporary pointer to RB info      */
  rrc_RB_InformationAffectedList_r7 *affected_rb_ptr = NULL;  /* Pointer to RBs to be Reconfigured */
  rrc_RB_InformationReconfigList_r7 *reconfig_rb_ptr = NULL;
  rrc_RAB_InformationSetupList_r7 *rab_info_ptr = NULL;
  rrc_SRB_InformationSetupList_r7    *     srb_setup_ptr = NULL;
  rrcrb_directed_cell_info_type        directed_cell_info; /* directed cell info struct */

  rrc_RadioBearerSetup_r7_IEs* rbsetup_ptr = NULL;
  rrc_state_e_type next_rrc_state = rbe.state_from_msg;

  /* Fill in the command id  */
  rrc_cmd.cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;

  /* Set RBSetup Ptr */
  rbsetup_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
    criticalExtensions.u.criticalExtensions.u.r7.radioBearerSetup_r7;


  if (RRC_CHECK_COMMON_MSG_TYPE(rbsetup_ptr->specificationMode,
      rrc_RadioBearerSetup_r7_IEs_specificationMode_complete))
  {
    /* First fill up the Radio Bearers that need to be setup.
    For this we need to know how many RBs need to be setup.
    If RABs need to be setup, check how many RBs are mapped
    to RABs. */
    if (rbsetup_ptr->specificationMode.u.complete.m.srb_InformationSetupListPresent)
    {
      /* Get the SRB setup list */
      srb_setup_ptr = &rbsetup_ptr->specificationMode.u.complete.srb_InformationSetupList;
  
      while ((srb_setup_ptr->n > srb_cnt) && (srb_cnt < UE_MAX_SRB_SETUP))
      {
        /* Check to see if the RB ID is there ... */
        if (srb_setup_ptr->elem[srb_cnt].m.rb_IdentityPresent)
        {
          rb_id = srb_setup_ptr->elem[srb_cnt].rb_Identity;
        }
  
        /*
        * ... if not, then start with the first SRB and get the next SRB ID
        * each time one is not included in the IE.
        */
        else
        {
          /* Get the next SRB, note this increments to 1 the first time */
          rb_id++;
        }
  
        /* Fill up the RB id and indicate that it needs to be established */
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = rb_id;
        if(rrclcm_get_dl_rlc_lc_id(rb_id) == RRCLCM_RLC_LC_ID_NOT_FOUND )
        {
          rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = ESTABLISH_RB;
        }
        else
        {
          rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB;
          rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB_OTA;
          rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = 
            rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = MODIFY_RB_CFG;
        }
  
        rb_count++;
        srb_cnt++;
  

  
      } /* end SRB setup */
  
    }

    if (rbsetup_ptr->specificationMode.u.complete.m.rab_InformationSetupListPresent)	
    {
      rab_info_ptr = &rbsetup_ptr->specificationMode.u.complete.rab_InformationSetupList;
      for (rab_index=0; rab_index < rbe.rabs.num_rabs_in_msg && (rab_info_ptr->n > rab_index); rab_index++)
      {
        if ( rbe.rabs.chan_config_needed[rab_index] == TRUE )
        {
          rb_ptr = &rab_info_ptr->elem[rab_index].rb_InformationSetupList;
    
          if ( rb_ptr->n == 0 )
          {
            WRRC_MSG0_HIGH("No RBs present in msg!");
          }
          else
          {
            /* Get the RBE db rab index */
            uint8 rbe_rab_index = rrcrbe_get_rab_index_from_rbe_db(
                                     rrcrb_translate_gsm_map_rab_id(rab_info_ptr->elem[rab_index].rab_Info.rab_Identity.u.gsm_MAP_RAB_Identity));
            rbe_rb_cnt = 0;
            if((rbe_rab_index < RRCRB_MAX_RAB_PER_MSG) && (rbe_rab_index < rbe.rabs.num_rabs_in_msg ))
            {
              while ( rb_ptr->n > rbe_rb_cnt )
              {                
                /* Fill up the RB id and indicate that it needs to be established */
                rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = rb_ptr->elem[rbe_rb_cnt].rb_Identity;
                rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = ESTABLISH_RB;

                /* Update the local structure so that we know which RB id's we've requested to be setup */
                rbe.rabs.rabs_in_msg[rbe_rab_index].rb_for_rab[rbe_rb_cnt].rb_id = rb_ptr->elem[rbe_rb_cnt].rb_Identity;

                /* Increment count of RBs */
                rb_count++;
                rbe_rb_cnt ++;
      
                /* Do a safety check on the count. We can't do more than 32
                         configs in a command */
                if ( (rb_count >= MAX_RB) || (rbe_rb_cnt >= MAX_RB_PER_RAB))
                {
                  WRRC_MSG0_HIGH("MAX RB count exceeded!");
                  break;
                }
      
                /* Go to the next RB */
              }/* end while */
              /* Save the number of RBs for this RAB */
              rbe.rabs.rabs_in_msg[rbe_rab_index].num_rbs_for_rab = rbe_rb_cnt;
            }
            else
            {
              WRRC_MSG1_ERROR("RBE RAB INDEX not found!!! RAB_INDEX %d",rbe_rab_index);
            }
          }
        }/* end if rabs_setup_needed */
      }
    }
    /* Now check if the message contains an RB information to be
       reconfig list. If so, we need to reconfigure some RBs */
    if ( rbsetup_ptr->specificationMode.u.complete.m.rb_InformationReconfigListPresent)
    {
      uint32 local_idx=0;
  
      reconfig_rb_ptr = &rbsetup_ptr->specificationMode.u.complete.rb_InformationReconfigList;
  
      while ( reconfig_rb_ptr->n > local_idx )
      {
  
        /* Fill up the RB id and indicate that it needs to be reconfigured */
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = reconfig_rb_ptr->elem[local_idx].rb_Identity;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB_OTA;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = 0;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = 0;
        if((reconfig_rb_ptr->elem[local_idx].m.rlc_InfoPresent)&&
           (reconfig_rb_ptr->elem[local_idx].rlc_Info.m.ul_RLC_ModePresent))
        {
          rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = MODIFY_RB_CFG;
        }
        if((reconfig_rb_ptr->elem[local_idx].m.rlc_InfoPresent)&&
           (reconfig_rb_ptr->elem[local_idx].rlc_Info.m.dl_RLC_ModePresent))
        {
          rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = MODIFY_RB_CFG;
        }

        if (reconfig_rb_ptr->elem[local_idx].m.rb_StopContinuePresent)
        {
          /* over-ride rb_config with STOP or continue */
          if (rrc_RB_StopContinue_stopRB == reconfig_rb_ptr->elem[local_idx].rb_StopContinue)
          {
            rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask |= STOP_RB_CFG;
            rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask |= STOP_RB_CFG;
          }
          else if (rrc_RB_StopContinue_continueRB == reconfig_rb_ptr->elem[local_idx].rb_StopContinue)
          {
            rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask |= CONTINUE_RB_CFG;
            rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask |= CONTINUE_RB_CFG;
          }
        }
  
        /* Increment count of RBs. This is done here since we
        need to use the next index that's not used already. */
        rb_count++;
  
        /* Go to the next affected RB */
        local_idx++;
  
        /* Do a safety check on the count. We can't do more than 32
        configs in a command */
        if ( rb_count >= MAX_RB )
        {
          WRRC_MSG0_HIGH("MAX RB count exceeded!");
          break;
        }
      } /* end while */
    }
  
    /* Now check if the message contains an RB information to be
    affected list. If so, we need to reconfigure some RBs */
    if ( rbsetup_ptr->specificationMode.u.complete.m.rb_InformationAffectedListPresent)
    {
      uint32 local_idx=0;
  
      affected_rb_ptr = &rbsetup_ptr->specificationMode.u.complete.rb_InformationAffectedList;
  
      if ( affected_rb_ptr->n == 0 )
      {
        WRRC_MSG0_HIGH("No affected RBs in msg!");
      }
      else
      {
        while ( affected_rb_ptr->n > local_idx )
        {
  
          /* Fill up the RB id and indicate that it needs to be reconfigured */
          rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = affected_rb_ptr->elem[local_idx].rb_Identity;
          rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB;
          rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB_OTA;
          rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = MODIFY_RB_CFG;
  
          /* Increment count of RBs. This is done here since we
          need to use the next index that's not used already. */
          rb_count++;
  
          /* Go to the next affected RB */
          local_idx++;
  
          /* Do a safety check on the count. We can't do more than 32
          configs in a command */
          if ( rb_count >= MAX_RB )
          {
            WRRC_MSG0_HIGH("MAX RB count exceeded!");
            break;
          }
        } /* end while */
      }
    }
    else
    {
      MSG_LOW("No affected RB list in msg", 0, 0, 0);
    }/* end if affected list present */

  }
  else if( rbsetup_ptr->specificationMode.t == T_rrc_RadioBearerSetup_r6_IEs_specificationMode_dummy)
  {
    WRRC_MSG0_HIGH("Preconfig mode not supported for RB-setup");
  }

  /* Number of RBs to config. Note rb_count is the total
  of established and reconfigured RBs */
  rrc_cmd.cmd.chan_config_req.num_rb_to_config = (int) rb_count;

  if(rb_count < MAX_RB)
  {
    rrc_fill_rb_id_chan_req_from_standalone_db(&rb_count, &rrc_cmd.cmd.chan_config_req);
  }

  if(rbe.state_from_msg == RRC_STATE_CELL_DCH)
  {
     /* Fill those RBs in chan_config_req for which RLC PDU size has changed and 
     that are already not present in chan_config_req*/
    if(rb_count < MAX_RB)
    {
      rrc_fill_rb_id_chan_req_from_rb_list_mapped_to_dch(&rb_count, &rrc_cmd.cmd.chan_config_req);
    }
    if(rrc_get_state() == RRC_STATE_CELL_FACH)
    {
      rrc_fill_srb_in_chan_config_req(&rb_count, &rrc_cmd.cmd.chan_config_req);
    }
    /*Following function fills the RBs to be released if CSoDCH -> CSoHS or vice versa is happening.*/	 
  }
 
  /*If state transition is Cell_DCH->Cell_FACH or vice-verca, add all the existing 
  AM PS RBs in chan_config_req*/	
  if(((next_rrc_state == RRC_STATE_CELL_FACH) && 
       (transition_config.toc_usage == TOC_FOR_DCH_AND_FACH ||
        transition_config.toc_usage == TOC_FOR_FACH_AND_PCH ||
        transition_config.toc_usage == TOC_FOR_FACH_AND_PCH_WITH_DCH_INFO))
       ||
      ((rrc_get_state() == RRC_STATE_CELL_FACH) && (next_rrc_state == RRC_STATE_CELL_DCH))
    )
  {
    rrc_fill_rb_id_chan_req_from_est_rabs(&rb_count, &rrc_cmd.cmd.chan_config_req);
  }
  /* Check if state change is required */
  if ( (rrc_get_state()) == (rbe.state_from_msg) )
  {
    rrc_cmd.cmd.chan_config_req.rrc_state_change_required = FALSE;
  }
  else
  {
    /* The message indicates that state change is required.
    Indicate to LLC what the next state should be. */
    rrc_cmd.cmd.chan_config_req.rrc_state_change_required = TRUE;
    rrc_cmd.cmd.chan_config_req.next_state = rbe.state_from_msg;
  }

  /* Indicate the id of the procedure sending the command */
  rrc_cmd.cmd.chan_config_req.procedure = RRC_PROCEDURE_RBE;

  /* Indicate that we need a confirmation for this command */
  rrc_cmd.cmd.chan_config_req.rrc_channel_config_cnf_required = TRUE;

  /* We are not required to fill any other data for this command.
   Now, send the command */
  if (((rrc_get_state()) == RRC_STATE_CELL_DCH) &&
      (rrc_cmd.cmd.chan_config_req.next_state== RRC_STATE_CELL_FACH) &&
      (rrc_cmd.cmd.chan_config_req.rrc_state_change_required))
  {

    /*Initiate the cell selection*/
    rrcrb_send_initiate_cell_selection_req(RRC_PROCEDURE_RBE,
                                           RRC_TRANSITION_FROM_DCH,
                                           rbe.state_from_msg);

    rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

    /* Start T_305 [periodic cell update] timer */
    rrccu_start_t_305_timer();
  }
  else
  {
    /*check whether any frequency info is present in the message
    and transition is CELL_FACH->CELL_FACH. In this case
    initiate a cell update after channel config is successfully completed */
    if (((rrc_get_state())==RRC_STATE_CELL_FACH)&&
        (rbe.state_from_msg==RRC_STATE_CELL_FACH))
    {
      if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
           rrc_RadioBearerSetup_r7_IEs,frequencyInfo)) &&
          (rbsetup_ptr->frequencyInfo.
           modeSpecificInfo.t == T_rrc_FrequencyInfo_modeSpecificInfo_fdd))
      {
        directed_cell_info.freq_info_present = TRUE;
        directed_cell_info.uarfcn_dl = rbsetup_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
      }
      else
      {
        directed_cell_info.freq_info_present = FALSE;
      }
      if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
	  	rrc_RadioBearerSetup_r7_IEs,dl_InformationPerRL_List)) &&
          (rbsetup_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.t
           == T_rrc_DL_InformationPerRL_r7_modeSpecificInfo_fdd))
      {
        directed_cell_info.pscr_present = TRUE;
        directed_cell_info.pscr = rbsetup_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
      }
      else
      {
        directed_cell_info.pscr_present = FALSE;
      }
      if (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE)
      {
        /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection 
          FACH->FACH ), treat it as un-supported configuration and send failure.*/ 
        if(rrccsp_check_initiate_cell_selection_handling(NULL) == FALSE)
        {
          WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
       
          rrcrbe_send_initiate_cu_flag = TRUE;
          rrcrbe_delete_crnti          = TRUE;
        }
        else
        {
          rrc_int_c_rnti_type      c_rnti;       /* Local C_RNTI value */
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,rrc_RadioBearerSetup_r7_IEs,new_C_RNTI))
          {
            rbe.new_crnti_valid = TRUE;
            rrc_translate_crnti(&rbsetup_ptr->new_C_RNTI,
                                &c_rnti);
            rbe.new_crnti = (uint16)c_rnti;
          }
          else
          {
            rbe.new_crnti_valid = FALSE;
          }
          rrcllcoc_update_toc_with_oc_from_fach();
          
          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req(RRC_PROCEDURE_RBE,
                                                  RRC_TRANSITION_FROM_FACH,
                                                  rbe.state_from_msg);
       
          rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
              
          return;
        }
      }
    } /* if CELL_FACH->CELL_FACH reconfiguration */
    /* Get a Command buffer and copy the local command to the buffer
    * so as to enqueue it on the internal queue.
    */

    if ((out_cmd_ptr = rrc_get_int_cmd_buf())!=NULL)
    {
      WCDMA_MEMCPY(out_cmd_ptr,
                   sizeof(rrc_cmd_type),
                   &rrc_cmd,
                   sizeof(rrc_cmd_int_type));
      rrc_put_int_cmd( out_cmd_ptr );
    }
  }
}

/*===========================================================================
FUNCTION   rrcrbe_validate_rab_setup_list_r7

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uecomdef_status_e_type rrcrbe_validate_rab_setup_list_r7
(
  rrc_RAB_InformationSetupList_r7 * rab_list_ptr,
  uint32 no_of_rabs
 #ifdef FEATURE_VOC_AMR_WB
  ,mvs_mode_type nas_sync_ind
 #endif /*FEATURE_VOC_AMR_WB*/  
)
{
  rrc_rab_id_type rab_id;           /* To store RAB id in internal RRC format */
  
  /* To store the status returned by MN layer. This indicates what type of RAB is received. This is
  used only if the RAB is for the CS_domain */
  rrcmn_rab_status_T rab_type = RRCMN_INVALID_RAB_ID;     
  
  /* Used as an index when storing established RABs locally. Only when we have more than
  one RAB per message, this will be incremented */
  uint32 rab_index=0;
  uint8 rab_list_index=0;

  boolean voice_rab_already_exists = FALSE;

  for (rab_index = no_of_rabs;(rab_list_ptr->n > rab_list_index) && (rab_index < RRCRB_MAX_RAB_PER_MSG)
                              &&(rab_list_index < RRCRB_MAX_RAB_PER_MSG) ; rab_index++,rab_list_index++)
  {
    /* GSM_MAP RAB Id. First convert it to internal format. */      
    rab_id = rrcrb_translate_gsm_map_rab_id((rab_list_ptr->elem[rab_list_index].rab_Info.
                                                  rab_Identity.u.gsm_MAP_RAB_Identity));
    
    /* Store this info locally so that ESTABLISHED_RABS can
    be updated later, when these RABs are established */
    rbe.rabs.rabs_in_msg[rab_index].rab_id_is_gsm_map = TRUE;
    rbe.rabs.rabs_in_msg[rab_index].rab_id = rab_id;
    rbe.rabs.rabs_in_msg[rab_index].re_est_timer =
    rab_list_ptr->elem[rab_list_index].rab_Info.re_EstablishmentTimer;
    /* Check if this RAB is CS Domain or PS Domain */
    if ( rab_list_ptr->elem[rab_list_index].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain )
    {
      /* Store the domain identity. This will be later added to ESTABLISHED_RABS */
      rbe.rabs.rabs_in_msg[rab_index].cn_domain = RRC_CS_DOMAIN_CN_ID;

      /* For CS RABs, Call MN layer to verify this RAB */
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
      rab_type = rrcmn_verify_rab_subs((radio_access_bearer_T)rab_id,rrc_get_as_id());
#else
      rab_type = rrcmn_verify_rab((radio_access_bearer_T)rab_id);
#endif
      switch ( rab_type )
      {
        case RRCMN_CS_DATA_CALL:

          MSG_LOW("Received a CS Data RAB Id %d", rab_id, 0, 0);
          break;

        case RRCMN_CS_VOICE_CALL:

          MSG_LOW("Received a CS Voice RAB Id %d", rab_id, 0, 0);
#ifdef FEATURE_VOC_AMR_WB
          if(RRC_MSG_COMMON_BITMASK_IE(rab_list_ptr->elem[rab_list_index].rab_Info,
              rrc_RAB_Info_r7,nas_Synchronisation_Indicator))
          {
            nas_sync_ind =  rrc_translate_nas_sync_indicator(
                                   &(rab_list_ptr->elem[rab_list_index].rab_Info.nas_Synchronisation_Indicator));
            if( nas_sync_ind == MVS_MODE_NONE )
            {
              rbe.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
              WRRC_MSG1_HIGH("Unsupported codec %d",rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator );
              return FAILURE;
            }          
          }

          rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator =nas_sync_ind;        
#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
          rrc_set_ordered_codec_status(TRUE);
#endif
#endif /*FEATURE_VOC_AMR_WB*/

          if (rrc_find_if_voice_rab_exists_in_established_rabs( rbe.rabs.rabs_in_msg[rab_index].rab_id)
               == TRUE)
          {
            WRRC_MSG1_HIGH("Voice RAB Id - %d already exists", rbe.rabs.rabs_in_msg[rab_index].rab_id );

            /*First condition of "if" covers the case when third RB is being added for AMR call*/
            /*Second condition of "if" covers the case when previously established Voice Call was of type CS over HSPA.*/
            if(rab_list_ptr->elem[rab_list_index].rb_InformationSetupList.n == 1)
            {
                voice_rab_already_exists = TRUE;
            }
            else
            {
              rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
              WRRC_MSG0_HIGH("Invalid configuration .. more than one RAB subflow is being added");
              return FAILURE;
            }
          }
          /*The given RAB isn't an existing Voice RAB.*/
          else
          {
            /* Whenever a new voice RAB is to be set up, at least two RBs need to be associated with that RAB.
            Therefore, first condition of "if" checks for presence of more than 1 RB. And the second condition
            of "if" ensures that it's for CS over DCH.*/
            if (rab_list_ptr->elem[rab_list_index].rb_InformationSetupList.n == 1)
            {
#ifdef FEATURE_SCUDIF_ENHANCEMENT
              #error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/
              WRRC_MSG0_HIGH("Not enough RBs mapped to voice RAB!");
              rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
          }          
          }
        break;

        default:
          WRRC_MSG1_ERROR("CS RAB validation failed! RABid %d",rab_id);
          break;
      } /* end switch */

      if((rbe.status.failure_status == RRCRB_MSG_INVALID_CONFIG) ||
         (rbe.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG))
      {
        /* No need to continue */
        return FAILURE;
      }
    }

    /* Else check for PS Domain */
    else if ( rab_list_ptr->elem[rab_list_index].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain )
    {
      /* Store the domain identity. This will be later added to ESTABLISHED_RABS */
        rbe.rabs.rabs_in_msg[rab_index].cn_domain = RRC_PS_DOMAIN_CN_ID;

      /* For PS Domain, if it's a loopback, then no need for query, otherwise, call RABM module to verify the RAB */
      if ( rrclbt_is_lb_on() == TRUE ||rabm_rabid_validate( RAB_ESTABLISH_QUERY, (uint8)rab_id )
           == TRUE )
      {
        MSG_LOW("PS RAB Id %d to be setup", rab_id, 0, 0);
      }
      else
      {
        WRRC_MSG1_ERROR("PS RAB validation failed! RABid %d",rab_id);
        rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
        return FAILURE;
      }
    }
    
    if(voice_rab_already_exists == TRUE)
    {
      rbe.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_VOICE_RAB;
      WRRC_MSG0_HIGH("Updated RAB type as voice");
    }
    else
    {
      /* ASN1 structure 'rrc_RAB_InformationSetup' is validated at this point,
      hence update rab_type in structure 'rbe.rabs.rabs_in_msg[rab_index]'. */
      /* change argument */
      (void) rrc_update_rab_type_in_rab_info_r7(&(rab_list_ptr->elem[rab_list_index]),
         &(rbe.rabs.rabs_in_msg[rab_index]));
    }

    MSG_HIGH("RAB_ID:%d,Domain:%d,re_est_tmr:%d",
             rbe.rabs.rabs_in_msg[rab_index].rab_id,
             rbe.rabs.rabs_in_msg[rab_index].cn_domain,
             rbe.rabs.rabs_in_msg[rab_index].re_est_timer);
#ifdef FEATURE_VOC_AMR_WB
    /* set the ordered codec mode which is sent in the ordered config to MAC.
      * If the RBE msg is not for Voice RAB also then also the current codec value
      * is passed to MAC 
      */
    rrc_set_ordered_codec_mode(nas_sync_ind);
    rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator = nas_sync_ind;
#endif /*FEATURE_VOC_AMR_WB*/

    /* Go to next RAB info  */
    
  }
  if (rab_list_ptr->n > rab_index)
  {
    WRRC_MSG0_HIGH("Too many RABs in an RBSetupMessage");
    rbe.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;

    return FAILURE;
  }
  
  rbe.rabs.num_rabs_in_msg = rab_index;
  return SUCCESS;
}

/*===========================================================================
FUNCTION   rrcrbe_prevalidate_rab_setup_list_r7

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uecomdef_status_e_type rrcrbe_prevalidate_rab_setup_list_r7
(
  rrc_RAB_InformationSetupList_r7 *rab_list_ptr,
  boolean *new_tm_rb
)
{
   rrc_RB_InformationSetup_r7 *rb_info_ptr = NULL;
  
  rlc_lc_id_type local_lc_id;
  rrclcm_status_e_type result;
  boolean dl_tm_flag;
  uecomdef_logch_mode_e_type rx_mode;
  
#ifdef FEATURE_UMTS_PDCP
   rrc_HeaderCompressionInfo_r4 *pdcpHeaderCompressionInfo = NULL;
#endif /* FEATURE_UMTS_PDCP*/
   uint32 rab_index=0,rb_index=0;
  do  /* going through the RAB info setup list */
  {
    /* Check if the RAB id is GSM_MAP type. We don't support any other type */
    if ((rab_list_ptr->elem[rab_index].rab_Info.rab_Identity.t
      != T_rrc_RAB_Identity_gsm_MAP_RAB_Identity))
    {
      WRRC_MSG1_HIGH("RAB ID type %d is not GSM_MAP!",rab_list_ptr->elem[rab_index].rab_Info.rab_Identity.t);
      rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return FAILURE;
    }

    /* check whether all RABs in the rabinfo list belong to same CN domain id or not
    otherwise this is an invalid configuration */
    if (rab_list_ptr->n > (rab_index+1))
    {
      if (rab_list_ptr->elem[rab_index].rab_Info.cn_DomainIdentity !=
          rab_list_ptr->elem[rab_index+1].rab_Info.cn_DomainIdentity)
      {
        WRRC_MSG2_ERROR("Different CN domains in rab-list %d %d",
            rab_list_ptr->elem[rab_index].rab_Info.cn_DomainIdentity,
            rab_list_ptr->elem[rab_index+1].rab_Info.cn_DomainIdentity
            );
        rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
        return FAILURE;
      }
    }

    /* Reject if MBMS info is included */
    if ((rab_list_ptr->elem[rab_index].rab_Info.m.mbms_SessionIdentityPresent))
    {
      WRRC_MSG0_HIGH("MBMS session Identity present. Not supported");
      rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return FAILURE;
    }

    /* Reject if MBMS info is included */
    if ((rab_list_ptr->elem[rab_index].rab_Info.m.mbms_ServiceIdentityPresent))
    {
      WRRC_MSG0_HIGH("MBMS session Identity present. Not supported");
      rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return FAILURE;
    }

    /* check for the unsupported PDCP info */
    rb_index=0;
    do  /* going through the RB info setup list */
    {

      rb_info_ptr = &rab_list_ptr->elem[rab_index].rb_InformationSetupList.elem[rb_index];
      if (rrc_find_rb_in_est_rabs(rb_info_ptr->rb_Identity) == RRC_RB_PRESENT) 
      {
        WRRC_MSG1_HIGH("RB id %d already exist in est rabs: Invalid config", 
                 rb_info_ptr->rb_Identity);
        rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        return FAILURE;
      }

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_ptr,rrc_RB_InformationSetup_r7,pdcp_Info))
      {
#ifdef FEATURE_UMTS_PDCP
        if (rrc_pdcp_enabled)
        {
          /* If the RAB is for cs domain, send RBS failure with cause invalid configuration */
          if (rab_list_ptr->elem[rab_index].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain)
          {
            WRRC_MSG0_HIGH("PDCP info invalid for CS rab");
            rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
            return FAILURE;
          }

          if(rab_list_ptr->elem[rab_index].rab_Info.cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain)
          {
            /* check whether lossless SRNS relocation flag is set
             * and header compression algo is present in the message */
            if ((RRC_MSG_COMMON_BITMASK_IE(rb_info_ptr->pdcp_Info,rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
                (RRC_CHECK_COMMON_MSG_TYPE(rb_info_ptr->pdcp_Info.losslessSRNS_RelocSupport,
                rrc_LosslessSRNS_RelocSupport_supported)))
            {
              if (rb_info_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_absent)
              {
                WRRC_MSG0_HIGH("PDCP pdu header is absent.  Invalid config");
                rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                return FAILURE;
              }
            } /* if rrc_PDCP_Info_r4_losslessSRNS_RelocSupport_present */

            if (rb_info_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present)
            {
              if ((RRC_MSG_COMMON_BITMASK_IE(rb_info_ptr->pdcp_Info,rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
                (RRC_CHECK_COMMON_MSG_TYPE(rb_info_ptr->pdcp_Info.losslessSRNS_RelocSupport,
                rrc_LosslessSRNS_RelocSupport_notSupported)))
              {
                if (!(RRC_MSG_COMMON_BITMASK_IE(rb_info_ptr->pdcp_Info,rrc_PDCP_Info_r4,headerCompressionInfoList)))
                {
                  WRRC_MSG0_HIGH("PDCP header compress info list not present");
                  rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                  return FAILURE;
                }
              }
            }

            if (RRC_MSG_COMMON_BITMASK_IE(rb_info_ptr->pdcp_Info,rrc_PDCP_Info_r4,headerCompressionInfoList))
            {
              uint32 local_idx=0;
              while (rb_info_ptr->pdcp_Info.headerCompressionInfoList.n > local_idx)
              {
                pdcpHeaderCompressionInfo = &rb_info_ptr->pdcp_Info.headerCompressionInfoList.elem[local_idx];
                if (RRC_CHECK_COMMON_MSG_TYPE(pdcpHeaderCompressionInfo->algorithmSpecificInfo,
                    rrc_AlgorithmSpecificInfo_r4_rfc2507_Info))
                {
                  /* if the UE capability "Maximum header compression context space", is exceeded with this configuration send
                  * failure with cause "Invalid Config"  */
                  if ( 
                    (pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER > IPHC_MAX_HC_CONTEXT_SPACE))
                  {
                    WRRC_MSG1_HIGH("Invalid config.  max header %d is invalid",
                            pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER);
                    rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                    return FAILURE;
                  }
                }
                else if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.t == T_rrc_AlgorithmSpecificInfo_r4_rfc3095_Info)
                {
                  /* the chosen MAX_CID shall not be greater than the value "Maximum number of ROHC context sessions" as 
                  *  indicated in the IE "PDCP Capability" */
                  if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info,
				  	ul_RFC3095))
                  {
                    if ( (pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->ul_RFC3095.max_CID > ROHC_MAX_CONTEXT_SESSIONS))
                    {
                      WRRC_MSG1_HIGH("Invalid config.  MAX_CID %d is invalid", pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->ul_RFC3095.max_CID);
                      rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                      return FAILURE;
                    }
                  }

                  if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info,dl_RFC3095))
                  {
                    if ((pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->dl_RFC3095.max_CID > ROHC_MAX_CONTEXT_SESSIONS))
                    {
                      WRRC_MSG1_HIGH("Invalid config.  MAX_CID %d is invalid",
                               pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->dl_RFC3095.max_CID);
                      rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                      return FAILURE;
                    }
                  }
                }  /* if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.t == rrc_rfc3095_Info)*/

                local_idx++;
              } /*  while (pdcpHeaderCompressionInfoList) */
            } /*  if (rb_info_ptr->pdcp_Info.bit_mask & rrc_PDCP_Info_r4_headerCompressionInfoList_present) */
          }
        }
#endif /*FEATURE_UMTS_PDCP*/
      } /* if (rb_info_ptr->bit_mask & rrc_RB_InformationSetup_r6_pdcp_Info_present) */
      /*point to the next link */

      if(*new_tm_rb == FALSE)
      {
        dl_tm_flag = FALSE;
        if((RRC_CHECK_COMMON_MSG_TYPE(rb_info_ptr->rlc_InfoChoice,rrc_RLC_InfoChoice_r7_rlc_Info)) &&
          (((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_ptr->rlc_InfoChoice.u.rlc_Info,
             rrc_RLC_Info_r7,ul_RLC_Mode))&&
             (rb_info_ptr->rlc_InfoChoice.u.rlc_Info->ul_RLC_Mode.t == 
             T_rrc_UL_RLC_Mode_ul_TM_RLC_Mode))) &&
             ((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,dl_RLC_Mode))&&
             (RRC_CHECK_COMMON_MSG_TYPE(rb_info_ptr->rlc_InfoChoice.u.rlc_Info->dl_RLC_Mode,
             rrc_DL_RLC_Mode_r7_dl_TM_RLC_Mode))))
        {
          *new_tm_rb = TRUE;
        }
        else if(RRC_CHECK_COMMON_MSG_TYPE(rb_info_ptr->rlc_InfoChoice,rrc_RLC_InfoChoice_r7_same_as_RB))
        {
          local_lc_id = rrclcm_get_dl_rlc_lc_id(rb_info_ptr->rlc_InfoChoice.u.same_as_RB);
          if(local_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
          {
            result = rrclcm_get_dl_rlc_mode(local_lc_id, &rx_mode);
            if((result == RRCLCM_SUCCESS) && (rx_mode == UE_MODE_TRANSPARENT))
            {
              dl_tm_flag = TRUE;
            }
          }

          local_lc_id = rrclcm_get_ul_rlc_lc_id(rb_info_ptr->rlc_InfoChoice.u.same_as_RB);
          if(local_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
          {
            result = rrclcm_get_ul_rlc_mode(local_lc_id, &rx_mode);
            if((dl_tm_flag == TRUE) && (result == RRCLCM_SUCCESS) && (rx_mode == UE_MODE_TRANSPARENT))
            {
              *new_tm_rb = TRUE;
            }
          }
        }
      }
      
      rb_index++;
    } while (rab_list_ptr->elem[rab_index].rb_InformationSetupList.n > rb_index);

    /* Go to next RAB setup infomation  */
    rab_index++;
  } while (rab_list_ptr->n > rab_index);

  return SUCCESS;
}

/*===========================================================================
FUNCTION   VALIDATE_RBSETUP_R7

DESCRIPTION
  This function validates the received R6 RB Setup Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a Radio Bearer Setup Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void validate_rbsetup_r7
(
  rrc_RadioBearerSetup *msg_ptr      /* Pointer to the RB Setup message */
)
{

  rrc_RadioBearerSetup_r7_IEs* rbsetup_ptr = NULL;

  uecomdef_status_e_type status;

#ifdef FEATURE_VOC_AMR_WB
  mvs_mode_type nas_sync_ind=rrc_get_current_codec_mode();
#endif /*FEATURE_VOC_AMR_WB*/

  boolean new_tm_rb = FALSE;

  /* Assume the message is valid for now */
  rbe.status.failure_status = RRCRB_MSG_VALID;

  rbsetup_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
    u.criticalExtensions.u.r7.radioBearerSetup_r7;

  switch (rbsetup_ptr->rrc_StateIndicator)
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      rbe.state_from_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      rbe.state_from_msg = RRC_STATE_CELL_FACH;
      break;

    default: /* Just to satisfy LINT */
      WRRC_MSG0_ERROR("Invalid RRC state from Msg");
      rbe.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
      break;

  } /* End of switch */

  if ( rbe.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
    return;
  }
#if 0
   /*Yet to add the support ofr REL7 modes.*/
  /*Reject RB setup with default configuration */
  if (rbsetup_ptr->specificationMode.t &
      rrc_RadioBearerSetup_r7_IEs_specificationMode_preconfiguration)
  {
    WRRC_MSG0_ERROR("Default config not supported on rbsetup");
    rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
    return;
  }
#endif
  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,rrc_RadioBearerSetup_r7_IEs,dl_HSPDSCH_Information))
  {

    /* Check if network does not try to initiate HSDPA with invalid state */
    status = rrc_hsdpa_initial_validation(rbsetup_ptr->rrc_StateIndicator,
             RRC_CHECK_COMMON_MSG_TYPE(rbsetup_ptr->dl_HSPDSCH_Information.modeSpecificInfo,
                                rrc_DL_HSPDSCH_Information_modeSpecificInfo_tdd));

    if (status == FAILURE)
    {
      rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }

  if (RRC_CHECK_COMMON_MSG_TYPE(rbsetup_ptr->specificationMode,
      rrc_RadioBearerSetup_r7_IEs_specificationMode_complete))
  {
    /* Check if there are new RABs to be established */
    if (rbsetup_ptr->specificationMode.u.complete.m.rab_InformationSetupListPresent)
    {
      if(rrcrbe_prevalidate_rab_setup_list_r7(&rbsetup_ptr->specificationMode.u.complete.
          rab_InformationSetupList,&new_tm_rb) == FAILURE)
      {
        return;
      }
    }
    else
    {
      /* There is no RAB to be setup in this message.*/
      rbe.rabs.num_rabs_in_msg = 0;
    }
  
    if (rbsetup_ptr->specificationMode.u.complete.m.dl_CounterSynchronisationInfoPresent)
    {
      /*check whether the SRNS relocation is within Cell_DCH or not.
        SRNS relocation information can come in reconfiguration messages
        only when the state transition is Cell_DCH->Cell_DCH */
      if (!((rbe.state_from_msg == RRC_STATE_CELL_DCH) &&
            ( rrc_get_state() == RRC_STATE_CELL_DCH)))
      {
        WRRC_MSG2_ERROR("SRNS reloc with invalid states cur_state %d, msg_state %d",
            rrc_get_state(), rbe.state_from_msg);
        rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
        return;
      }
     /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has
       PDCP info list. If it is true then this configuration is unsupported */
  
      if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2(rbsetup_ptr->specificationMode.u.complete.dl_CounterSynchronisationInfo,
           rb_WithPDCP_InfoList)) ||
          (RRC_MSG_COMMON_BITMASK_IE(rbsetup_ptr->specificationMode.u.complete.dl_CounterSynchronisationInfo,
           rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
      {
        WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
        rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
        return;
      }
      else
      {
        /* The procedure involves Lossy SRNS relocation.
        Set the flag that indicates that this procedure
        involves SRNS relocation */
        rbe.rbe_for_srns_relocation = TRUE;
        /* store if new U-RNTI is present in the message */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
            rrc_RadioBearerSetup_r7_IEs,new_U_RNTI))
        {
          rbe.new_urnti_valid = TRUE;
          rrc_translate_urnti (&(rbsetup_ptr->new_U_RNTI),
                               &rbe.new_urnti);
        }
      }
    }

   /* Process RAB reconfig before RAB setup because we want to catch NW's which
       are trying to setup and reconfig RAB with the same message */
    if (rbsetup_ptr->specificationMode.u.complete.m.rab_InformationReconfigListPresent)
    {
      MSG_LOW("RAB Info Reconfig in R6 RBS Msg", 0, 0, 0);


      rbe.status.failure_status = rrc_update_rab_info_reconfig_param(
      &rbsetup_ptr->specificationMode.u.complete.rab_InformationReconfigList, &rbe.rabs 
#ifdef FEATURE_VOC_AMR_WB
                                             ,&nas_sync_ind
#endif/*FEATURE_VOC_AMR_WB*/
                                           );
                                           
      if( rbe.status.failure_status != RRCRB_MSG_VALID) 
      {
        return;
      }
      
    }

    if (rbsetup_ptr->specificationMode.u.complete.m.rab_InformationSetupListPresent)
    {
      if(rrcrbe_validate_rab_setup_list_r7(&rbsetup_ptr->specificationMode.u.complete.
          rab_InformationSetupList, rbe.rabs.num_rabs_in_msg
    #ifdef FEATURE_VOC_AMR_WB
          ,nas_sync_ind
    #endif /*FEATURE_VOC_AMR_WB*/
          ) == FAILURE)
        {
          return;
        }
      }
    }

  /* Store the number of the RAB info in the RBSetup message. */
  WRRC_MSG1_MED("%d RABs  in an RBSetupMessage",  rbe.rabs.num_rabs_in_msg);

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
  */
  rbe.trans_to_dch_ciph_config_needed = FALSE;
  if((rbe.state_from_msg == RRC_STATE_CELL_DCH) && 
     (rrc_get_state() != RRC_STATE_CELL_DCH)
    )
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
        rrc_RadioBearerSetup_r7_IEs,dl_CommonInformation)) && 
       (RRC_MSG_COMMON_BITMASK_IE(rbsetup_ptr->dl_CommonInformation,
       rrc_DL_CommonInformation_r7,dl_dpchInfoCommon)) &&
       ( rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.t ==
       T_rrc_DL_CommonInformation_r7_dl_dpchInfoCommon_dl_DPCH_InfoCommon))
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
              rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon,rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
              &rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
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
        rbe.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }

  if(rbe.state_from_msg == RRC_STATE_CELL_DCH)
  {
    rbe.first_tm_rb_setup = rrcrbe_is_first_tm_rb_setup(new_tm_rb);
    if(new_tm_rb)
    {
      WRRC_MSG1_HIGH("First TM RB being set status : %d",rbe.first_tm_rb_setup);
    }
    if(rbe.first_tm_rb_setup == TRUE)
    {
      //check to see if dl-dpch-common info is present while going to dch
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
         rrc_RadioBearerSetup_r7_IEs,dl_CommonInformation)) && 
         (RRC_MSG_COMMON_BITMASK_IE(rbsetup_ptr->dl_CommonInformation,
         rrc_DL_CommonInformation_r7,dl_dpchInfoCommon)) &&
         (T_rrc_DL_CommonInformation_r7_dl_dpchInfoCommon_dl_DPCH_InfoCommon == 
          rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.t))
      {
        uecomdef_status_e_type loc_ret_val;
        //check if ciphering is active in mac for cs domain, if it is then,
        //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        
        WRRC_MSG0_HIGH("First time TM RB Setup HHO Step-1 config activated");
        loc_ret_val = rrcrb_get_mac_d_hfn(
             ( RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon,rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
              &rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
              &mac_d_hfn_l);
        if (loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("get_mac_d_hfn failed");
        }
        if(rrcsmc_process_cipher_config_for_first_tm_setup(mac_d_hfn_l) == FAILURE)
        {
          WRRC_MSG0_HIGH("SMC process ciphering config for First TM RB Setup failed");
          /*This step is reached only when the first step returns a failure,. i.e, if the ciphering is not active for CS domain.
            Hence setting rbe.first_tm_rb_setup to FALSE so that step 2 is not done*/
            rbe.first_tm_rb_setup = FALSE;
        }

      }
    }
  }


  /* If UTRAN DRX coefficient is not present then it is invalid configuration */
  if ((rbe.state_from_msg == RRC_STATE_CELL_PCH) ||
      (rbe.state_from_msg  == RRC_STATE_URA_PCH))
  {

    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
          rrc_RadioBearerSetup_r7_IEs,utran_DRX_CycleLengthCoeff)))
    {
      WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", rbe.state_from_msg);
      rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }

  if((RRC_CHECK_COMMON_MSG_TYPE(rbsetup_ptr->specificationMode,rrc_RadioBearerSetup_r7_IEs_specificationMode_complete)) &&
      rbsetup_ptr->specificationMode.u.complete.m.rb_InformationReconfigListPresent)
  {
    /* Save the linked list head pointer */
    if (FAILURE == rrcllc_validate_rb_info_reconfig_list_r7(
        &rbsetup_ptr->specificationMode.u.complete.rb_InformationReconfigList))
    {
      rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }

  if (rbe.state_from_msg == RRC_STATE_URA_PCH)
  {
    /*should we do it after getting l2ack for response??*/
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
          rrc_RadioBearerSetup_r7_IEs,ura_Identity)))
    {
      WRRC_MSG0_HIGH("URA ID not present");
      rrccu_clear_ura_id ();
    }
    else
    {
      /*URA ID present in the message set ura_identity variable with this value */
      if (rbsetup_ptr->ura_Identity.numbits == 16)
      {
        rrccu_update_ura_id (&(rbsetup_ptr->ura_Identity));
      }
      else
      {
        WRRC_MSG1_ERROR("Invalid URA ID length%d",
            rbsetup_ptr->ura_Identity.numbits);
        rbe.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
        rbe.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
        return;
      }
    }
  }
  return;

}

/*===========================================================================
FUNCTION   PROCESS_RBSETUP_R7

DESCRIPTION

  This function processes the Rel 6 RB Setup Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a Radio Bearer Setup Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -

    ORDERED_CONFIG will be set by calling LLC.

    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to configure the Radio Bearers given by the message.

    If ORDERED_CONFIG could not be set, a Radio Bearer Setup Failure message
    will be sent to UTRAN.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_rbsetup_r7
(
rrc_cmd_type *cmd_ptr
)
{
  rrc_DL_DCCH_Message     *dcch_msg_ptr;   /* Pointer to the RB Setup message */
  rrc_RadioBearerSetup *msg_ptr;    /* Pointer to the RB Setup message        */

  rrc_rab_search_e_type rab_status = RRC_RAB_NOT_FOUND; /* To check if RAB is present in ESTABLISHED_RABS */
  rrcllc_oc_status_e_type oc_status;/* To hold ordered_config status          */
  uint32 rab_index=0;               /* Used when more than 1 rab is present in
                                    a RBS message                             */
  rrc_int_c_rnti_type      c_rnti;       /* Local C_RNTI value */
  boolean ordered_config_err_occured = FALSE; /* Used to indicate if an error
                                                 occured while setting ordered_config. */
  rrc_msg_ver_enum_type msg_version = MSG_REL7;

  rrc_RadioBearerSetup_r7_IEs* rbsetup_ptr = NULL;

  rrcsmc_srns_ip_check_status_e_type    ip_check_status = RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE;


  WRRC_MSG1_MED("RRCEUL:rbsetupr7 cur_state %d", rbe.curr_substate);

  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  /* Get the pointer to the actual RB Setup message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerSetup);

  /* Store Msg Ptr in a local variable */
  rbsetup_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
    u.r7.radioBearerSetup_r7;

  if (TRUE == rrcrbe_validate_srns_relocation_info(msg_ptr, msg_version))
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
      WRRC_MSG0_ERROR("IP check failed. Discarding RBS msg");
      break;

    case RRCSMC_SRNS_SECURITY_INFO_INVALID:
    case RRCSMC_INCOMPATIBLE_SECURITY_RECONFIGURATION:
      if (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier
          == rbe.tr_ids.rejected_transaction)
      {
        WRRC_MSG1_HIGH("Duplicate RBE msg - rejected tr-id%d match!",
                 rbe.tr_ids.rejected_transaction);
        /* Nothing else to do, just ignore the message and return */
      }
      else
      {
        /* send a reconfig failure messge with invalid
        configuration and
          continue the same substate with any ongoing procedure */
        rbe.tr_ids.rejected_transaction =
        (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
        if (ip_check_status == RRCSMC_SRNS_SECURITY_INFO_INVALID)
        {
          rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        }
        else
        {
          rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
        }
        /*Check whether Cell Update is going on */
        if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
             (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
        {
          if(rbe.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
          {
            send_rbs_failure_message( rbe.status.failure_status,
                                     rbe.status.prot_err_cause );
            rbe.status.failure_status = RRCRB_MSG_VALID;
          }
          else
          {
            WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
            (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBE);
            /* Wait until Cell update procedure is completed */
            rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
          }
        }
        else
        {
          send_rbs_failure_message( rbe.status.failure_status,
                                    rbe.status.prot_err_cause );
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
    if (ip_check_status != RRCSMC_SRNS_INTEGRITY_CHECK_SUCCESS)
    {
      /*do not clear the procedure or change the rbe substate because
      some other rbe procedure might be in progress. Just return from
        here */
      return;
    }

  } /* End of validation of srns relocation */

  if ((rrcllc_get_ordered_config_status() != OC_NOT_SET) ||
      (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
      (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
  {
    WRRC_MSG0_HIGH("Incomatible simultaneous reconfig! Rejecting RBS message");
    if ( (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier)
         == rbe.tr_ids.accepted_transaction )
    {
      WRRC_MSG1_HIGH("Duplicate RBS msg! accepted tr-id%d match!",
               rbe.tr_ids.accepted_transaction);
      /* Nothing else to do, just return */
    }
    else if ( (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier)
                == rbe.tr_ids.rejected_transaction )
    {
      WRRC_MSG1_HIGH("Duplicate RBS msg! rejected tr-id%d match!", rbe.tr_ids.rejected_transaction);
      /* Nothing else to do, just return */
    }
    else
    {
      /* We have received a RBS message with a new transaction id.
      This message should be in our rejected transaction
        list */
      rbe.tr_ids.rejected_transaction =
      (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
      rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;

      /*Check whether Cell Update is going on */
      if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
           (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
      {
        if(rbe.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
        {
          send_rbs_failure_message( rbe.status.failure_status,
                                   rbe.status.prot_err_cause );
          rbe.status.failure_status = RRCRB_MSG_VALID;
        }
        else
        {
          WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
          (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBE);
          /* Wait until Cell update procedure is completed */
          rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        }
      } 
      else
      {
        send_rbs_failure_message( rbe.status.failure_status,
                                  rbe.status.prot_err_cause );
        /*continue the same substate */
      }
    }
    return;
  }

  /* We have received a RB setup message with a new transaction id.
  Accept the transaction Id */
  rbe.tr_ids.accepted_transaction =
  msg_ptr->u.later_than_r3.rrc_TransactionIdentifier;

  validate_rbsetup_r7(msg_ptr);

  if ( rbe.status.failure_status != RRCRB_MSG_VALID )
  {
    /* RB Message was invalid. */
    WRRC_MSG0_HIGH("RB Message validation failed");
    /* Send the RB Setup Failure Message */
    send_rbs_failure_message( rbe.status.failure_status,
                              rbe.status.prot_err_cause
                            );

    /* Make sure we're back in the initial sub-state */
    rbe.curr_substate = RRCRB_INITIAL;
    rrcrbe_clear_procedure();
  }
  /* If the message is valid, proceed further */
  else
  {
    rrcrbe_indicate_layer1_if_rb_setup_configure_voice_rab();

    /*From lower layer's point of view, CS over HSPA call is same as PS call.
    So updating mac_rab_status as PS Call for Cs over HSPA.*/

    if(rbsetup_ptr->specificationMode.u.complete.m.rab_InformationSetupListPresent)
    {
      rrc_RAB_InformationSetupList_r7 *rab_list_ptr = &(rbsetup_ptr->specificationMode.u.complete.rab_InformationSetupList);
      rrcrb_update_mac_rab_status(RRC_PROCEDURE_RBE,
                                 (rrc_cn_domain_identity_e_type)(rab_list_ptr->elem[0].rab_Info.cn_DomainIdentity) ,
                                  NULL,FALSE);
    }

    /*calculate ciphering_activation_cfn only if ciphering is enabled*/
    if (rrcsmc_is_ciphering_enabled() == RRCSMC_SECURITY_STARTED)
    {
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,rrc_RadioBearerSetup_r7_IEs,activationTime))
      {
        /*256 is the CFN rollover length*/
        /*250 is used to activate ciphering after 250 10ms time interval,
        this should give sufficient time for n/w side and ue side to have
        configured ciphering on the rb's successfully*/
        ciphering_activation_cfn =
        (uint8) ((rbsetup_ptr->activationTime + 250) % 256);
        /*round it to 8 frame boundary */
        ciphering_activation_cfn = (uint8)((ciphering_activation_cfn/8)*8);

      }
      else
      {
        /*256 is the CFN rollover length*/
        /*250 is used to activate ciphering after 250 10ms time interval,
        this should give sufficient time for n/w side and ue side to have
        configured ciphering on the rb's successfully*/
        ciphering_activation_cfn = (uint8)((seq_get_cfn() + 250) % 256);
        /*round it to 8 frame boundary */
        ciphering_activation_cfn = (uint8)((ciphering_activation_cfn/8)*8);
      }
    }

    /*call rrcsmc function to update ciphering activation cfn*/
    rrcsmc_update_ciphering_activation_cfn(ciphering_activation_cfn);

    /* Check if there is CN information present. */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,rrc_RadioBearerSetup_r7_IEs,cn_InformationInfo))
    {
      rbe.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(
        &(rbsetup_ptr->cn_InformationInfo) );
      if (rbe.mm_cmd_ptr == NULL)
      {
        WRRC_MSG0_ERROR("Could not build SERVICE_IND cmd");
      }
      else
      {
        /* set cn_info_saved flag */
        rbe.cn_info_saved = TRUE;
      }
    }

#ifdef FEATURE_WCDMA_REL7_SPECRS
    /*If RBE OTA provides "supportForChangeOfUE_Capability" IE, we need to store it in
           "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
          "rrc_rnc_cap_change_support" can be updated accordingly. */  	
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,rrc_RadioBearerSetup_r7_IEs,supportForChangeOfUE_Capability))
    {
      if(rbsetup_ptr->supportForChangeOfUE_Capability == TRUE)
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
    /* Check if there are new RABs to be established or if there are
       affected RB or SRBs to be setup
     */

    if ((rbsetup_ptr->specificationMode.u.complete.m.rab_InformationSetupListPresent) ||
        (((rbsetup_ptr->specificationMode.u.complete.m.rb_InformationAffectedListPresent) &&
          (rbsetup_ptr->specificationMode.u.complete.rb_InformationAffectedList.n != 0 )) ||
        ((rbsetup_ptr->specificationMode.u.complete.m.srb_InformationSetupListPresent) &&
         (rbsetup_ptr->specificationMode.u.complete.srb_InformationSetupList.n != 0))))
    {
      /* Check if the RAB id is already in ESTABLISHED_RABS */
      for (rab_index=0; rab_index < rbe.rabs.num_rabs_in_msg; rab_index++)
      {
        rab_status = rrc_find_rab_in_established_rabs
          (rbe.rabs.rabs_in_msg[rab_index].rab_id );
        rbe.rabs.chan_config_needed[rab_index] = TRUE;
        /*Right now, we are not handling VT -> CSoHS Voice scenarios.*/
#ifdef FEATURE_SCUDIF_ENHANCEMENT
        #error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/

        /*The following case handles the scenario when the RAB being established is present in EST RABs But it's not 
        CS VOICE RAB. In that case, RB-Setup Failure has to be sent*/
        if ((rab_status != RRC_RAB_NOT_FOUND) &&
             (rrc_find_if_voice_rab_exists_in_established_rabs( rbe.rabs.rabs_in_msg[rab_index].rab_id )
               == FALSE))
        {
          WRRC_MSG1_HIGH("RAB %d already exists in EST_RABS!",
                   rbe.rabs.rabs_in_msg[rab_index].rab_id);
         /* send a failure message if the rab in the message  already
           exists in   established_rabs. Later add a check for the rbs
           associated with the rabs so that the failure will be sent only when
           any of the rb-ids match with the rb-ids in established_rab for that
           rab */
          rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
          /* Send the RB Setup Failure Message */
          send_rbs_failure_message( rbe.status.failure_status,
                                    rbe.status.prot_err_cause
                                    );

        /* Make sure we're back in the initial sub-state */
          rbe.curr_substate = RRCRB_INITIAL;
          rrcrbe_clear_procedure();
          return;
        }
      }

      /* We have to establish this RAB. First set ORDERED_CONFIG
      so that LLC can process the necessary parameters.
      This function will copy all RABs from the rxd msg to oc. */
      oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_RBE,
                                             rbe.state_from_msg,
                                             rrc_DL_DCCH_Message_PDU,
                                             (void *)dcch_msg_ptr
                                             );

      switch ( oc_status )
      {
        case ORDERED_CONFIG_SET:
            if(rrc_sib_is_current_event_sib_read_in_dch())
            {
               WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
               rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
            }

          /* Ordered Config has been set. We can setup the
            Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */
          WRRC_MSG0_HIGH("Sending RRC_CHANNEL_CONFIG_REQ to LLC");
          /* Call the function that creates and sends a channel
          config request to LLC. */
          rrcrbe_send_channel_config_req_r7( msg_ptr );

          if (((rrc_get_state()) == RRC_STATE_CELL_DCH) &&
              (rrc_cmd.cmd.chan_config_req.next_state== RRC_STATE_CELL_FACH) &&
              (rrc_cmd.cmd.chan_config_req.rrc_state_change_required))
          {
            if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
                rrc_RadioBearerSetup_r5_IEs,new_C_RNTI))
            {
              rbe.new_crnti_valid = TRUE;
              rrc_translate_crnti(&rbsetup_ptr->new_C_RNTI,
                                  &c_rnti);
              rbe.new_crnti = (uint16)c_rnti;
            }
            else
            {
              rbe.new_crnti_valid = FALSE;
            }
            rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
          }
          else
          {
            if( rbe.curr_substate != RRCRB_WAIT_FOR_CELL_SELECTION_CNF )
            {
               /* Change the substate since we're waiting for the
                 confirmation from LLC for the channel configs */
               rbe.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
            }
          }
          break;

        case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

          rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
          /* Send the RB Setup Failure Message */
          send_rbs_failure_message( rbe.status.failure_status,
                                    rbe.status.prot_err_cause
                                  );

          /* Go back to initial substate */
          rbe.curr_substate = RRCRB_INITIAL;

          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;

        case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

          rbe.status.failure_status =RRCRB_MSG_UNSUPPORTED_CONFIG;
          /* Send the RB Setup Failure Message */
          send_rbs_failure_message( rbe.status.failure_status,
                                    rbe.status.prot_err_cause
                                  );

          /* Go back to initial substate */
          rbe.curr_substate = RRCRB_INITIAL;

          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;
          
          case ORDERED_CONFIG_CONFIGURATION_INVALID:
 
           rbe.status.failure_status =RRCRB_MSG_INVALID_CONFIG;
           /* Send the RB Setup Failure Message */
           send_rbs_failure_message( rbe.status.failure_status,
                                     rbe.status.prot_err_cause
                                   );
 
           /* Go back to initial substate */
           rbe.curr_substate = RRCRB_INITIAL;
 
           /* Indicate that an error occured */
           ordered_config_err_occured = TRUE;
           break;
        case ORDERED_CONFIG_NOT_SET_OTHER:
          WRRC_MSG0_HIGH("LLC returned other failure!");
          /* When LLC sends this, there is a serious problem
          and we'll go back to disconnected state. No need
          to send the failure message here, we can just
          go back to initial substate */
          rbe.curr_substate = RRCRB_INITIAL;

          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME          
        case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
          WRRC_MSG0_HIGH("RB setup processing deferred due to LTA!");

          rbe.curr_substate = RRCRB_INITIAL;
        
          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
        default:

          WRRC_MSG0_ERROR("Unknown return value from LLC");
          /* Unknown error, just go back to initial substate */
          rbe.curr_substate = RRCRB_INITIAL;

          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;
        }/* end switch */

        if ( ordered_config_err_occured == TRUE )
        {
          /* No need to continue. Clear the procedure
            variables and return. */
          rrcrbe_clear_procedure();
          return;
        }

        if (rbe.rbe_for_srns_relocation == TRUE)
        {
          rrcsmc_commit_fresh_for_srns_relocation();
        }
    }
    else
    {
      /*If we are here, it means that RABs are not given in RB-Setup.
      Also, SRBs and RB Affected IEs are not given.*/

      WRRC_MSG0_ERROR("No RB IEs are given in the OTA.");
      rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
      rbe.status.prot_err_cause = rrc_ProtocolErrorCause_informationElementMissing;  

      /* Send the RB Setup Failure Message */
      send_rbs_failure_message(rbe.status.failure_status, rbe.status.prot_err_cause);
      /* Make sure we're back in the initial sub-state */
      rbe.curr_substate = RRCRB_INITIAL;
      rrcrbe_clear_procedure();

      return;          
    }/* end if - RBs need to be reconfigured */
  } /* else if valid rb message */
}/* end function process_rbsetup_message_r6 */

/*===========================================================================

FUNCTION   PROCESS_RBSETUP_R6

DESCRIPTION

  This function processes the Rel 6 RB Setup Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a Radio Bearer Setup Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -

    ORDERED_CONFIG will be set by calling LLC.

    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to configure the Radio Bearers given by the message.

    If ORDERED_CONFIG could not be set, a Radio Bearer Setup Failure message
    will be sent to UTRAN.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_rbsetup_r6
(
rrc_cmd_type *cmd_ptr
)
{
  rrc_DL_DCCH_Message     *dcch_msg_ptr;   /* Pointer to the RB Setup message */
  rrc_RadioBearerSetup *msg_ptr;    /* Pointer to the RB Setup message        */

  rrc_rab_search_e_type rab_status = RRC_RAB_NOT_FOUND; /* To check if RAB is present in ESTABLISHED_RABS */
  rrcllc_oc_status_e_type oc_status;/* To hold ordered_config status          */
  uint32 rab_index=0;               /* Used when more than 1 rab is present in
                                    a RBS message                             */
  rrc_int_c_rnti_type      c_rnti;       /* Local C_RNTI value */
  boolean ordered_config_err_occured = FALSE; /* Used to indicate if an error
                                                 occured while setting ordered_config. */
  rrc_msg_ver_enum_type msg_version = MSG_REL6;

  rrc_RadioBearerSetup_r6_IEs* rbsetup_ptr = NULL;

  rrcsmc_srns_ip_check_status_e_type    ip_check_status = RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE;


  WRRC_MSG1_MED("RRCEUL:rbsetupr6 cur_state %d", rbe.curr_substate);

  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  /* Get the pointer to the actual RB Setup message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerSetup);

  /* Store Msg Ptr in a local variable */
  rbsetup_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
    u.r6.radioBearerSetup_r6;

  if (TRUE == rrcrbe_validate_srns_relocation_info(msg_ptr, msg_version))
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
      WRRC_MSG0_ERROR("IP check failed. Discarding RBS msg");
      break;

    case RRCSMC_SRNS_SECURITY_INFO_INVALID:
    case RRCSMC_INCOMPATIBLE_SECURITY_RECONFIGURATION:
      if (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier
          == rbe.tr_ids.rejected_transaction)
      {
        WRRC_MSG1_HIGH("Duplicate RBE msg - rejected tr-id%d match!",
                 rbe.tr_ids.rejected_transaction);
        /* Nothing else to do, just ignore the message and return */
      }
      else
      {
        /* send a reconfig failure messge with invalid
        configuration and
          continue the same substate with any ongoing procedure */
        rbe.tr_ids.rejected_transaction =
        (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
        if (ip_check_status == RRCSMC_SRNS_SECURITY_INFO_INVALID)
        {
          rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        }
        else
        {
          rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
        }
        /*Check whether Cell Update is going on */
        if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
             (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
        {
          if(rbe.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
          {
            send_rbs_failure_message( rbe.status.failure_status,
                                     rbe.status.prot_err_cause );
            rbe.status.failure_status = RRCRB_MSG_VALID;
          }
          else
          {
            WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
            (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBE);
            /* Wait until Cell update procedure is completed */
            rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
          }
        }
        else
        {
          send_rbs_failure_message( rbe.status.failure_status,
                                    rbe.status.prot_err_cause );
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
    if (ip_check_status != RRCSMC_SRNS_INTEGRITY_CHECK_SUCCESS)
    {
      /*do not clear the procedure or change the rbe substate because
      some other rbe procedure might be in progress. Just return from
        here */
      return;
    }

  } /* End of validation of srns relocation */

  if ((rrcllc_get_ordered_config_status() != OC_NOT_SET) ||
      (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
      (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
  {
    WRRC_MSG0_HIGH("Incomatible simultaneous reconfig! Rejecting RBS message");
    if ( (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier)
         == rbe.tr_ids.accepted_transaction )
    {
      WRRC_MSG1_HIGH("Duplicate RBS msg! accepted tr-id%d match!",
               rbe.tr_ids.accepted_transaction);
      /* Nothing else to do, just return */
    }
    else if ( (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier)
                == rbe.tr_ids.rejected_transaction )
    {
      WRRC_MSG1_HIGH("Duplicate RBS msg! rejected tr-id%d match!", rbe.tr_ids.rejected_transaction);
      /* Nothing else to do, just return */
    }
    else
    {
      /* We have received a RBS message with a new transaction id.
      This message should be in our rejected transaction
        list */
      rbe.tr_ids.rejected_transaction =
      (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
      rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;

      /*Check whether Cell Update is going on */
      if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
           (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
      {
        if(rbe.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
        {
          send_rbs_failure_message( rbe.status.failure_status,
                                   rbe.status.prot_err_cause );
          rbe.status.failure_status = RRCRB_MSG_VALID;
        }
        else
        {
          WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
          (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBE);
          /* Wait until Cell update procedure is completed */
          rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        }
      }
      else
      {
        send_rbs_failure_message( rbe.status.failure_status,
                                  rbe.status.prot_err_cause );
        /*continue the same substate */
      }
    }
    return;
  }

  /* We have received a RB setup message with a new transaction id.
  Accept the transaction Id */
  rbe.tr_ids.accepted_transaction =
  msg_ptr->u.later_than_r3.rrc_TransactionIdentifier;

  validate_rbsetup_r6(msg_ptr);

  if ( rbe.status.failure_status != RRCRB_MSG_VALID )
  {
    /* RB Message was invalid. */
    WRRC_MSG0_HIGH("RB Message validation failed");
    /* Send the RB Setup Failure Message */
    send_rbs_failure_message( rbe.status.failure_status,
                              rbe.status.prot_err_cause
                            );

    /* Make sure we're back in the initial sub-state */
    rbe.curr_substate = RRCRB_INITIAL;
    rrcrbe_clear_procedure();
  }
  /* If the message is valid, proceed further */
  else
  {
    rrcrbe_indicate_layer1_if_rb_setup_configure_voice_rab();

     /* If new rabs are setup, update mac rab status */

    if(rbsetup_ptr->specificationMode.u.complete.m.rab_InformationSetupListPresent)
    {
      rrc_RAB_InformationSetupList_r6 *rab_list_ptr = &(rbsetup_ptr->specificationMode.u.complete.rab_InformationSetupList);
      rrcrb_update_mac_rab_status(RRC_PROCEDURE_RBE,
                                 (rrc_cn_domain_identity_e_type)(rab_list_ptr->elem[0].rab_Info.cn_DomainIdentity) ,
                                  NULL,FALSE);
    }

    /*calculate ciphering_activation_cfn only if ciphering is enabled*/
    if (rrcsmc_is_ciphering_enabled() == RRCSMC_SECURITY_STARTED)
    {
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,rrc_RadioBearerSetup_r6_IEs,activationTime))
      {
        /*256 is the CFN rollover length*/
        /*250 is used to activate ciphering after 250 10ms time interval,
        this should give sufficient time for n/w side and ue side to have
        configured ciphering on the rb's successfully*/
        ciphering_activation_cfn =
        (uint8) ((rbsetup_ptr->activationTime + 250) % 256);
        /*round it to 8 frame boundary */
        ciphering_activation_cfn = (uint8)((ciphering_activation_cfn/8)*8);

      }
      else
      {
        /*256 is the CFN rollover length*/
        /*250 is used to activate ciphering after 250 10ms time interval,
        this should give sufficient time for n/w side and ue side to have
        configured ciphering on the rb's successfully*/
        ciphering_activation_cfn = (uint8)((seq_get_cfn() + 250) % 256);
        /*round it to 8 frame boundary */
        ciphering_activation_cfn = (uint8)((ciphering_activation_cfn/8)*8);
      }
    }

    /*call rrcsmc function to update ciphering activation cfn*/
    rrcsmc_update_ciphering_activation_cfn(ciphering_activation_cfn);

    /* Check if there is CN information present. */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,rrc_RadioBearerSetup_r6_IEs,cn_InformationInfo))
    {
      rbe.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(
        &(rbsetup_ptr->cn_InformationInfo) );
      if (rbe.mm_cmd_ptr == NULL)
      {
        WRRC_MSG0_ERROR("Could not build SERVICE_IND cmd");
      }
      else
      {
        /* set cn_info_saved flag */
        rbe.cn_info_saved = TRUE;
      }
    }

    /* Check if there are new RABs to be established or if there are
       affected RB or SRBs to be setup
     */

    if ((rbsetup_ptr->specificationMode.u.complete.m.rab_InformationSetupListPresent) ||
        (((rbsetup_ptr->specificationMode.u.complete.m.rb_InformationAffectedListPresent) &&
          (rbsetup_ptr->specificationMode.u.complete.rb_InformationAffectedList.n != 0 )) ||
        ((rbsetup_ptr->specificationMode.u.complete.m.srb_InformationSetupListPresent) &&
         (rbsetup_ptr->specificationMode.u.complete.srb_InformationSetupList.n != 0))))
    {
      /* Check if the RAB id is already in ESTABLISHED_RABS */
      for (rab_index=0; rab_index < rbe.rabs.num_rabs_in_msg; rab_index++)
      {
        rab_status = rrc_find_rab_in_established_rabs
          (rbe.rabs.rabs_in_msg[rab_index].rab_id );
        rbe.rabs.chan_config_needed[rab_index] = TRUE;
#ifdef FEATURE_SCUDIF_ENHANCEMENT
        #error code not present
#endif

        if ((rab_status != RRC_RAB_NOT_FOUND) &&
             (rrc_find_if_voice_rab_exists_in_established_rabs( rbe.rabs.rabs_in_msg[rab_index].rab_id )
               == FALSE))
        {
          WRRC_MSG1_HIGH("RAB %d already exists in EST_RABS!",
                   rbe.rabs.rabs_in_msg[rab_index].rab_id);
         /* send a failure message if the rab in the message  already
           exists in   established_rabs. Later add a check for the rbs
           associated with the rabs so that the failure will be sent only when
           any of the rb-ids match with the rb-ids in established_rab for that
           rab */
          rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
          /* Send the RB Setup Failure Message */
          send_rbs_failure_message( rbe.status.failure_status,
                                    rbe.status.prot_err_cause
                                    );

        /* Make sure we're back in the initial sub-state */
          rbe.curr_substate = RRCRB_INITIAL;
          rrcrbe_clear_procedure();
          return;
        }
      }

      /* We have to establish this RAB. First set ORDERED_CONFIG
      so that LLC can process the necessary parameters.
      This function will copy all RABs from the rxd msg to oc. */
      oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_RBE,
                                             rbe.state_from_msg,
                                             rrc_DL_DCCH_Message_PDU,
                                             (void *)dcch_msg_ptr
                                             );

      switch ( oc_status )
      {
        case ORDERED_CONFIG_SET:

          if((rrc_sib_is_current_event_sib_read_in_dch()) &&
             (RRC_PROCEDURE_GPS == rrc_sib_get_proc_id_for_sib_read_in_dch()))
          {
            WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
            rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
          }


          /* Ordered Config has been set. We can setup the
            Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */
          WRRC_MSG0_HIGH("Sending RRC_CHANNEL_CONFIG_REQ to LLC");
          /* Call the function that creates and sends a channel
          config request to LLC. */
          rrcrbe_send_channel_config_req_r6( msg_ptr );

          if (((rrc_get_state()) == RRC_STATE_CELL_DCH) &&
              (rrc_cmd.cmd.chan_config_req.next_state== RRC_STATE_CELL_FACH) &&
              (rrc_cmd.cmd.chan_config_req.rrc_state_change_required))
          {
            if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
                rrc_RadioBearerSetup_r5_IEs,new_C_RNTI))
            {
              rbe.new_crnti_valid = TRUE;
              rrc_translate_crnti(&rbsetup_ptr->new_C_RNTI,
                                  &c_rnti);
              rbe.new_crnti = (uint16)c_rnti;
            }
            else
            {
              rbe.new_crnti_valid = FALSE;
            }
            rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
          }
          else
          {
           if( rbe.curr_substate != RRCRB_WAIT_FOR_CELL_SELECTION_CNF )
           {
             /* Change the substate since we're waiting for the
               confirmation from LLC for the channel configs */
             rbe.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
           }
          }
          break;

        case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

          rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
          /* Send the RB Setup Failure Message */
          send_rbs_failure_message( rbe.status.failure_status,
                                    rbe.status.prot_err_cause
                                  );

          /* Go back to initial substate */
          rbe.curr_substate = RRCRB_INITIAL;

          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;

        case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

          rbe.status.failure_status =RRCRB_MSG_UNSUPPORTED_CONFIG;
          /* Send the RB Setup Failure Message */
          send_rbs_failure_message( rbe.status.failure_status,
                                    rbe.status.prot_err_cause
                                  );

          /* Go back to initial substate */
          rbe.curr_substate = RRCRB_INITIAL;

          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;

	case ORDERED_CONFIG_CONFIGURATION_INVALID:

          rbe.status.failure_status =RRCRB_MSG_INVALID_CONFIG;
          /* Send the RB Setup Failure Message */
          send_rbs_failure_message( rbe.status.failure_status,
                                    rbe.status.prot_err_cause
                                  );

          /* Go back to initial substate */
          rbe.curr_substate = RRCRB_INITIAL;

          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;

        case ORDERED_CONFIG_NOT_SET_OTHER:
          WRRC_MSG0_HIGH("LLC returned other failure!");
          /* When LLC sends this, there is a serious problem
          and we'll go back to disconnected state. No need
          to send the failure message here, we can just
          go back to initial substate */
          rbe.curr_substate = RRCRB_INITIAL;

          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME          
        case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
          WRRC_MSG0_HIGH("RB setup processing deferred due to LTA!");
          rbe.curr_substate = RRCRB_INITIAL;
        
          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
        default:

          WRRC_MSG0_ERROR("Unknown return value from LLC");
          /* Unknown error, just go back to initial substate */
          rbe.curr_substate = RRCRB_INITIAL;

          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;
        }/* end switch */

        if ( ordered_config_err_occured == TRUE )
        {
          /* No need to continue. Clear the procedure
            variables and return. */
          rrcrbe_clear_procedure();
          return;
        }

        if (rbe.rbe_for_srns_relocation == TRUE)
        {
          rrcsmc_commit_fresh_for_srns_relocation();
        }
    }
    else
    {
      /*If we are here, it means that RABs are not given in RB-Setup.
      Also, SRBs and RB Affected IEs are not given.*/

      WRRC_MSG0_ERROR("No RB IEs are given in the OTA.");
      rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
      rbe.status.prot_err_cause = rrc_ProtocolErrorCause_informationElementMissing;  

      /* Send the RB Setup Failure Message */
      send_rbs_failure_message(rbe.status.failure_status, rbe.status.prot_err_cause);
      /* Make sure we're back in the initial sub-state */
      rbe.curr_substate = RRCRB_INITIAL;
      rrcrbe_clear_procedure();

      return;          
    }/* end if - RBs need to be reconfigured */
  } /* else if valid rb message */
}/* end function process_rbsetup_message_r6 */

/*===========================================================================

FUNCTION   rrcrbe_check_and_get_primary_plmn_id

DESCRIPTION

  This function will check whether primary plmn id is present 
  in r3,r5 path of RBE message. If present then it will return 
  primary plmn id.
        
DEPENDENCIES
  None.

RETURN VALUE
TRUE : IF Primary PLMN I.D is present in RBE message in r3,r5 path
FFALSE: Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
static boolean rrcrbe_check_and_get_primary_plmn_id
(
  /* Pointer to the RBE message */
  rrc_RadioBearerSetup *msg_ptr,
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
      /*Check to see if primary PLMN I.D is present in RBE message ext
       * rrc_RadioBearerSetup-v690ext which is present in 
       * v690NonCriticalExtensions in r99 path
       */
      if((RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3,rrc_RadioBearerSetup_r3,v3a0NonCriticalExtensions))&&
         (msg_ptr->u.r3.v3a0NonCriticalExtensions.m.laterNonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
           v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
           v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v5d0NonCriticalExtenstionsPresent)&&
         (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
           v4b0NonCriticalExtensions.v590NonCriticalExtensions.
           v5d0NonCriticalExtenstions.m.v690NonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
           v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5d0NonCriticalExtenstions.
           v690NonCriticalExtensions.radioBearerSetup_v690ext.m.primary_plmn_IdentityPresent)
        )
      {
        WRRC_MSG0_HIGH("NWS:RBE msg contains primary_plmn_Id in r99 path");
        primary_plmn_Id_present = TRUE;
        *primary_plmn_Identity = msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
           v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5d0NonCriticalExtenstions.
           v690NonCriticalExtensions.radioBearerSetup_v690ext.primary_plmn_Identity;
      }
      break;
  
    case MSG_REL5:
    /*Check to see if primary PLMN I.D is present in RBE message ext
     * rrc_RadioBearerSetup-v690ext which is present in 
     * v690NonCriticalExtensions in r5 path
     */
    if((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.m.v5d0NonCriticalExtenstionsPresent)&&
       (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.
         v5d0NonCriticalExtenstions.m.v690NonCriticalExtensionsPresent)&&
       (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.
         v5d0NonCriticalExtenstions.v690NonCriticalExtensions.
         radioBearerSetup_v690ext.m.primary_plmn_IdentityPresent)
      )
    {
      WRRC_MSG0_HIGH("NWS:RBE msg contains primary_plmn_Id in r5 path");
      primary_plmn_Id_present = TRUE;
      *primary_plmn_Identity = msg_ptr->u.later_than_r3.criticalExtensions.u.
             criticalExtensions.u.r5.v5d0NonCriticalExtenstions.v690NonCriticalExtensions.
             radioBearerSetup_v690ext.primary_plmn_Identity;
    }
    break;
    
    default:
      WRRC_MSG1_ERROR("NWS: Msg version = %d, Not supported",msg_version);
      break;
  }
  
  return primary_plmn_Id_present;
}
/*===========================================================================

FUNCTION   PROCESS_RBSETUP_R5

DESCRIPTION

  This function processes the Rel 5 RB Setup Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a Radio Bearer Setup Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -

    ORDERED_CONFIG will be set by calling LLC.

    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to configure the Radio Bearers given by the message.

    If ORDERED_CONFIG could not be set, a Radio Bearer Setup Failure message
    will be sent to UTRAN.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_rbsetup_r5
(
rrc_cmd_type *cmd_ptr
)
{
  rrc_DL_DCCH_Message     *dcch_msg_ptr; /* Pointer to the RB Setup message */
  rrc_RadioBearerSetup *msg_ptr;    /* Pointer to the RB Setup message        */

  rrc_rab_search_e_type rab_status = RRC_RAB_NOT_FOUND; /* To check if RAB is present in ESTABLISHED_RABS */
  rrcllc_oc_status_e_type oc_status;/* To hold ordered_config status          */
  uint32 rab_index=0;               /* Used when more than 1 rab is present in
                                    a RBS message                             */
  rrc_int_c_rnti_type      c_rnti;       /* Local C_RNTI value */
  boolean ordered_config_err_occured = FALSE; /* Used to indicate if an error
                                    occured while setting ordered_config. */

  rrc_msg_ver_enum_type msg_version = MSG_REL5;

  rrc_RadioBearerSetup_r5_IEs* rbsetup_ptr = NULL;

  rrcsmc_srns_ip_check_status_e_type    ip_check_status = RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE;

   /*Boolean used to indicate primary plmn i.d is present in OTA msg*/
  boolean primary_plmn_Id_present = FALSE;
   /*Variable to hold primary plmn i.d if present in OTA msg*/
  rrc_PLMN_Identity primary_plmn_Identity;


  WRRC_MSG1_MED("RRCHS:rbsetupr5 cur_state %d", rbe.curr_substate);

  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  /* Get the pointer to the actual RB Setup message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerSetup);


  /* The first thing to do is check if Rel 5 contents are present */
  if ((msg_ptr->u.later_than_r3.criticalExtensions.t ==
       T_rrc_RadioBearerSetup_criticalExtensions_5_criticalExtensions) &&
      (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.t ==
       T_rrc_RadioBearerSetup_criticalExtensions_4_r5))
  {
    /* Store Msg Ptr in a local variable */
    rbsetup_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.
                  u.r5.radioBearerSetup_r5;
  }
  else
  {
    WRRC_MSG1_HIGH("Unsupported RB Setup t:%d", msg_ptr->t);
    rbe.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
    rbe.status.prot_err_cause = rrc_ProtocolErrorCause_messageExtensionNotComprehended;

    if ((msg_ptr->u.later_than_r3.rrc_TransactionIdentifier)
        == rbe.tr_ids.rejected_transaction )
    {
      WRRC_MSG1_HIGH("Duplicate RBS msg! rejected tr-id%d match!", rbe.tr_ids.rejected_transaction);
      /* Nothing else to do, just ignore the message and return */
    }
    else
    {
      rbe.tr_ids.rejected_transaction = msg_ptr->u.later_than_r3.rrc_TransactionIdentifier;

      /*Check whether Cell Update is going on */
      if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
         (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
      {
        if(rbe.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
        {
          send_rbs_failure_message( rbe.status.failure_status,
                                   rbe.status.prot_err_cause );
          rbe.status.failure_status = RRCRB_MSG_VALID;
        }
        else
        {
          WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
          (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBE);
          /* Wait until Cell update procedure is completed */
          rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        }
      }
      else
      {
        send_rbs_failure_message( rbe.status.failure_status,
                                rbe.status.prot_err_cause );
      }
    }

    return;
  }

  if (TRUE == rrcrbe_validate_srns_relocation_info(msg_ptr, msg_version))
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
      WRRC_MSG0_ERROR("IP check failed. Discarding RBS msg");
      break;

    case RRCSMC_SRNS_SECURITY_INFO_INVALID:
    case RRCSMC_INCOMPATIBLE_SECURITY_RECONFIGURATION:
      if (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier
          == rbe.tr_ids.rejected_transaction)
      {
        WRRC_MSG1_HIGH("Duplicate RBE msg - rejected tr-id%d match!",
                 rbe.tr_ids.rejected_transaction);
        /* Nothing else to do, just ignore the message and return */
      }
      else
      {
        /* send a reconfig failure messge with invalid
        configuration and
          continue the same substate with any ongoing procedure */
        rbe.tr_ids.rejected_transaction =
        (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
        if (ip_check_status == RRCSMC_SRNS_SECURITY_INFO_INVALID)
        {
          rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        }
        else
        {
          rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
        }
        /*Check whether Cell Update is going on */
        if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
             (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
        {
          if(rbe.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
          {
            send_rbs_failure_message( rbe.status.failure_status,
                                     rbe.status.prot_err_cause );
            rbe.status.failure_status = RRCRB_MSG_VALID;
          }
          else
          {
            WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
            (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBE);
            /* Wait until Cell update procedure is completed */
            rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
          }
        }
        else
        {
          send_rbs_failure_message( rbe.status.failure_status,
                                    rbe.status.prot_err_cause );
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
    if (ip_check_status != RRCSMC_SRNS_INTEGRITY_CHECK_SUCCESS)
    {
      /*do not clear the procedure or change the rbe substate because
      some other rbe procedure might be in progress. Just return from
        here */
      return;
    }

  } /* End of validation of srns relocation */

  if ((rrcllc_get_ordered_config_status() != OC_NOT_SET) ||
      (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
      (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
  {
    WRRC_MSG0_HIGH("Incomatible simultaneous reconfig! Rejecting RBS message");
    if ( (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier)
         == rbe.tr_ids.accepted_transaction )
    {
      WRRC_MSG1_HIGH("Duplicate RBS msg! accepted tr-id%d match!",
               rbe.tr_ids.accepted_transaction);
      /* Nothing else to do, just return */
    }
    else if ( (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier)
                == rbe.tr_ids.rejected_transaction )
    {
      WRRC_MSG1_HIGH("Duplicate RBS msg! rejected tr-id%d match!", rbe.tr_ids.rejected_transaction);
      /* Nothing else to do, just return */
    }
    else
    {
      /* We have received a RBS message with a new transaction id.
      This message should be in our rejected transaction
        list */
      rbe.tr_ids.rejected_transaction =
      (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
      rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;

      /*Check whether Cell Update is going on */
      if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
           (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
      {
        WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
        (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBE);
        /* Wait until Cell update procedure is completed */
        rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      } else
      {
        send_rbs_failure_message( rbe.status.failure_status,
                                  rbe.status.prot_err_cause );
        /*continue the same substate */
      }
    }
    return;
  }

  /* We have received a RB setup message with a new transaction id.
  Accept the transaction Id */
  rbe.tr_ids.accepted_transaction =
  msg_ptr->u.later_than_r3.rrc_TransactionIdentifier;

  validate_rbsetup_r5(msg_ptr);

  if ( rbe.status.failure_status != RRCRB_MSG_VALID )
  {
    /* RB Message was invalid. */
    WRRC_MSG0_HIGH("RB Message validation failed");

    /* Send the RB Setup Failure Message */
    send_rbs_failure_message( rbe.status.failure_status,
                              rbe.status.prot_err_cause
                            );

    /* Make sure we're back in the initial sub-state */
    rbe.curr_substate = RRCRB_INITIAL;
    rrcrbe_clear_procedure();
  }
  /* If the message is valid, proceed further */
  else
  {
    rrcrbe_indicate_layer1_if_rb_setup_configure_voice_rab();

    /* If new rabs are setup, update mac rab status */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
        rrc_RadioBearerSetup_r5_IEs,rab_InformationSetupList))
    {
      rrc_RAB_InformationSetupList_r5 *rab_list_ptr = &(rbsetup_ptr->rab_InformationSetupList);
      rrcrb_update_mac_rab_status(RRC_PROCEDURE_RBE,
                                 (rrc_cn_domain_identity_e_type)(rab_list_ptr->elem[0].rab_Info.cn_DomainIdentity) ,
                                  NULL,FALSE);
    }

    /*calculate ciphering_activation_cfn only if ciphering is enabled*/
    if (rrcsmc_is_ciphering_enabled() == RRCSMC_SECURITY_STARTED)
    {
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
	  	rrc_RadioBearerSetup_r5_IEs,activationTime))
      {
        /*256 is the CFN rollover length*/
        /*250 is used to activate ciphering after 250 10ms time interval,
        this should give sufficient time for n/w side and ue side to have
        configured ciphering on the rb's successfully*/
        ciphering_activation_cfn =
        (uint8) ((rbsetup_ptr->activationTime + 250) % 256);
        /*round it to 8 frame boundary */
        ciphering_activation_cfn = (uint8)((ciphering_activation_cfn/8)*8);

      }
      else
      {
        /*256 is the CFN rollover length*/
        /*250 is used to activate ciphering after 250 10ms time interval,
        this should give sufficient time for n/w side and ue side to have
        configured ciphering on the rb's successfully*/
        ciphering_activation_cfn = (uint8)((seq_get_cfn() + 250) % 256);
        /*round it to 8 frame boundary */
        ciphering_activation_cfn = (uint8)((ciphering_activation_cfn/8)*8);
      }
    }

    /*call rrcsmc function to update ciphering activation cfn*/
    rrcsmc_update_ciphering_activation_cfn(ciphering_activation_cfn);

    /* Check if there is CN information present. */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,rrc_RadioBearerSetup_r5_IEs,cn_InformationInfo))
    {
      WRRC_MSG0_HIGH("Saving CN info for NAS");
      /*Check to see if primary PLMN I.D is present in RBE message ext
       * radioBearerSetup-v690ext which is present in 
       * v690NonCriticalExtensions
       */
       primary_plmn_Id_present = rrcrbe_check_and_get_primary_plmn_id(msg_ptr,
                                                                  &primary_plmn_Identity,MSG_REL5);
      rbe.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info(
                     &(rbsetup_ptr->cn_InformationInfo) 
                     ,primary_plmn_Id_present
                     ,&primary_plmn_Identity
          );
      if (rbe.mm_cmd_ptr == NULL)
      {
        WRRC_MSG0_ERROR("Could not build SERVICE_IND cmd");
      }
      else
      {
        /* set cn_info_saved flag */
        rbe.cn_info_saved = TRUE;
      }

    }

    /* Check if there are new RABs to be established */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
        rrc_RadioBearerSetup_r5_IEs,rab_InformationSetupList))
    {
      /* Check if the RAB id is already in ESTABLISHED_RABS */
      for (rab_index=0; rab_index < rbe.rabs.num_rabs_in_msg; rab_index++)
      {
        rab_status = rrc_find_rab_in_established_rabs
                     ( rbe.rabs.rabs_in_msg[rab_index].rab_id );
        rbe.rabs.chan_config_needed[rab_index] = TRUE;
        if (rab_status != RRC_RAB_NOT_FOUND)
        {
          WRRC_MSG1_HIGH("RAB %d already exists in EST_RABS!",
                   rbe.rabs.rabs_in_msg[rab_index].rab_id);
          break;
        }
      }

#ifdef FEATURE_SCUDIF_ENHANCEMENT
      #error code not present
#endif

      if ( (rab_status == RRC_RAB_NOT_FOUND) || 
           (rrc_find_if_voice_rab_exists_in_established_rabs( rbe.rabs.rabs_in_msg[rab_index].rab_id )
               == TRUE))
      {
        /* We have to establish this RAB. First set ORDERED_CONFIG
        so that LLC can process the necessary parameters.
          This function will copy all RABs from the rxd msg to oc. */
        oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_RBE,
                                               rbe.state_from_msg,
                                               rrc_DL_DCCH_Message_PDU,
                                               (void *)dcch_msg_ptr
                                             );

        switch ( oc_status )
        {
          case ORDERED_CONFIG_SET:
            if((rrc_sib_is_current_event_sib_read_in_dch()) &&
               (RRC_PROCEDURE_GPS == rrc_sib_get_proc_id_for_sib_read_in_dch()))
            {
               WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
               rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
            }

    

            /* Ordered Config has been set. We can setup the
              Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */
            WRRC_MSG0_HIGH("Sending RRC_CHANNEL_CONFIG_REQ to LLC");
            /* Call the function that creates and sends a channel
            config request to LLC. */
            rrcrbe_send_channel_config_req_r5( msg_ptr );
  
            if (((rrc_get_state()) == RRC_STATE_CELL_DCH) &&
                (rrc_cmd.cmd.chan_config_req.next_state== RRC_STATE_CELL_FACH) &&
                (rrc_cmd.cmd.chan_config_req.rrc_state_change_required))
            {
              if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
                  rrc_RadioBearerSetup_r5_IEs,new_C_RNTI))
              {
                rbe.new_crnti_valid = TRUE;
                rrc_translate_crnti(&rbsetup_ptr->new_C_RNTI,
                                    &c_rnti);
                rbe.new_crnti = (uint16)c_rnti;
              }
              else
              {
                rbe.new_crnti_valid = FALSE;
              }
              rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
            }
            else
            {
              /* Change the substate since we're waiting for the
                confirmation from LLC for the channel configs */
              rbe.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
            }
            break;
  
          case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:
  
            rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
            /* Send the RB Setup Failure Message */
            send_rbs_failure_message( rbe.status.failure_status,
                                      rbe.status.prot_err_cause
                                    );
  
            /* Go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
  
          case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:
  
            rbe.status.failure_status =RRCRB_MSG_UNSUPPORTED_CONFIG;
            /* Send the RB Setup Failure Message */
            send_rbs_failure_message( rbe.status.failure_status,
                                      rbe.status.prot_err_cause
                                    );
  
            /* Go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;

	   case ORDERED_CONFIG_CONFIGURATION_INVALID:
  
            rbe.status.failure_status =RRCRB_MSG_INVALID_CONFIG;
            /* Send the RB Setup Failure Message */
            send_rbs_failure_message( rbe.status.failure_status,
                                      rbe.status.prot_err_cause
                                    );
  
            /* Go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
  
          case ORDERED_CONFIG_NOT_SET_OTHER:
            WRRC_MSG0_HIGH("LLC returned other failure!");
            /* When LLC sends this, there is a serious problem
            and we'll go back to disconnected state. No need
            to send the failure message here, we can just
            go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
  
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME  
          case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
            WRRC_MSG0_HIGH("RB setup processing deferred due to LTA!");
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/        
          default:
  
            WRRC_MSG0_ERROR("Unknown return value from LLC");
            /* Unknown error, just go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
        }/* end switch */

        if ( ordered_config_err_occured == TRUE )
        {
          /* No need to continue. Clear the procedure
            variables and return. */
          rrcrbe_clear_procedure();
          return;
        }

        if (rbe.rbe_for_srns_relocation == TRUE)
        {
          rrcsmc_commit_fresh_for_srns_relocation();
        }

      }
      else
      {
        /* send a failure message if the rab in the message  already
        exists in established_rabs. Later add a check for the rbs
        associated with the rabs so that the failure will be sent only when
        any of the rb-ids match with the rb-ids in established_rab for that
          rab */
        rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
        /* Send the RB Setup Failure Message */
        send_rbs_failure_message( rbe.status.failure_status,
                                  rbe.status.prot_err_cause
                                );

        /* Make sure we're back in the initial sub-state */
        rbe.curr_substate = RRCRB_INITIAL;
        rrcrbe_clear_procedure();
        return;
      }
    }




    /* If RABs need to be setup, the RBs to be setup for
    that RAB and any other RBs to be re-configured as indicated
    by the message will be sent in the same channel config
    command. But if no new RBs have been setup, then we
    need to send a separate command for any RBs that
    need to be re-configured */
    if ( rbe.rabs.chan_config_needed[0] == FALSE )
    {
      /* RABs are not being setup for this message.
      Even though we don't have to setup RABs, there may
      be signalling RBs to be reconfigured in the message. Check
        if there are any */

      if (((RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,rrc_RadioBearerSetup_r5_IEs,rb_InformationAffectedList)) &&
          (rbsetup_ptr->rb_InformationAffectedList.n != 0 )) ||
          ((RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,rrc_RadioBearerSetup_r5_IEs,srb_InformationSetupList)) &&
          (rbsetup_ptr->srb_InformationSetupList.n != 0)))
      {

        /* We have to reconfigure these RBs. First set ORDERED_CONFIG
          so that LLC can process the necessary parameters. */
        oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_RBE,
                                               rbe.state_from_msg,
                                               rrc_DL_DCCH_Message_PDU,
                                               (void *)dcch_msg_ptr
                                             );

        switch ( oc_status )
        {
          case ORDERED_CONFIG_SET:
            if((rrc_sib_is_current_event_sib_read_in_dch()) &&
               (RRC_PROCEDURE_GPS == rrc_sib_get_proc_id_for_sib_read_in_dch()))
            {
              WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
              rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
            }

            /* Ordered Config has been set. We can setup the
              Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */
            MSG_LOW("Sending RRC_CHANNEL_CONFIG_REQ to LLC", 0, 0, 0);
            /* Call the function that creates and sends a channel
            config request to LLC. */
            rrcrbe_send_channel_config_req_r5( msg_ptr );
  
            if (((rrc_get_state()) == RRC_STATE_CELL_DCH) &&
                (rrc_cmd.cmd.chan_config_req.next_state== RRC_STATE_CELL_FACH) &&
                (rrc_cmd.cmd.chan_config_req.rrc_state_change_required))
            {
              if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
                  rrc_RadioBearerSetup_r5_IEs,new_C_RNTI))
              {
                rbe.new_crnti_valid = TRUE;
                rrc_translate_crnti(&rbsetup_ptr->new_C_RNTI,
                                    &c_rnti);
                rbe.new_crnti = (uint16)c_rnti;
              }
              else
              {
                rbe.new_crnti_valid = FALSE;
              }
              rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
            }
            else
            {
              if( rbe.curr_substate != RRCRB_WAIT_FOR_CELL_SELECTION_CNF )
              {
                /* Change the substate since we're waiting for the
                confirmation from LLC for the channel configs */
                rbe.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
              }
            }
  
            break;
  
          case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:
  
            rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
            /* Send the RB Setup Failure Message */
            send_rbs_failure_message( rbe.status.failure_status,
                                      rbe.status.prot_err_cause
                                    );
  
            /* Go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
  
          case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:
  
            rbe.status.failure_status =RRCRB_MSG_UNSUPPORTED_CONFIG;
            /* Send the RB Setup Failure Message */
            send_rbs_failure_message( rbe.status.failure_status,
                                      rbe.status.prot_err_cause
                                    );
  
            /* Go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
			
	   case ORDERED_CONFIG_CONFIGURATION_INVALID:
  
            rbe.status.failure_status =RRCRB_MSG_INVALID_CONFIG;
            /* Send the RB Setup Failure Message */
            send_rbs_failure_message( rbe.status.failure_status,
                                      rbe.status.prot_err_cause
                                    );
  
            /* Go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
  
  
          case ORDERED_CONFIG_NOT_SET_OTHER:
            WRRC_MSG0_HIGH("LLC returned other failure!");
            /* When LLC sends this, there is a serious problem
            and we'll go back to disconnected state. No need
            to send the failure message here, we can just
            go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
  
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME  
          case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:

            WRRC_MSG0_HIGH("RB setup processing deferred due to LTA!");
            rbe.curr_substate = RRCRB_INITIAL;

            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
          break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
          default:
  
            WRRC_MSG0_ERROR("Unknown return value from LLC");
            /* Unknown error, just go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
        }/* end switch */

        if ( ordered_config_err_occured == TRUE )
        {
          /* No need to continue. Clear the procedure
            variables and return. */
          rrcrbe_clear_procedure();
          return;
        }
      }
      else
      {
        /*If we are here, it means that RABs are not given in RB-Setup.
        Also, SRBs and RB Affected IEs are not given.*/

        WRRC_MSG0_ERROR("No RB IEs are given in the OTA.");
        rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
        rbe.status.prot_err_cause = rrc_ProtocolErrorCause_informationElementMissing;  

        /* Send the RB Setup Failure Message */
        send_rbs_failure_message(rbe.status.failure_status, rbe.status.prot_err_cause);
        /* Make sure we're back in the initial sub-state */
        rbe.curr_substate = RRCRB_INITIAL;
        rrcrbe_clear_procedure();

        return;          
      }/* end if - RBs need to be reconfigured */
    }/* end if - rbe.rabs.chan_config_needed[rab_index] == FALSE */
  }/* end if - valid rb message */
}/* end function process_rbsetup_message_r5 */


/*===========================================================================

FUNCTION   RRCRBE_PROCESS_NEW_CELL_IND

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
static void rrcrbe_process_new_cell_ind
(
  rrc_new_cell_ind_type *ptr /* Pointer to the State Change Ind cmd */
)
{
  rrc_cmd_type  *out_cmd_ptr;
  
  WRRC_MSG0_HIGH("Sending RRC_CHANNEL_CONFIG_REQ to LLC");
  /* If the UE has camped on directed cell then do not send a cell update,
   * else send a CELL update with cause "cell Re-selection 
   */
  
  if(rrccsp_is_selected_cell_utran_directed()&&
      (rbe.new_crnti_valid)) 
  {
    /* No need to initiate cell update as the C-RNTI is available
     * and UE is camped on the directed cell.
     */
    rrcrbe_send_initiate_cu_flag = FALSE;
  }
  else
  {
    rrcrbe_send_initiate_cu_flag = TRUE;
    rrcrbe_delete_crnti          = TRUE;
  }
#ifdef FEATURE_WCDMA_HS_FACH
 /* If H-RNTI is not given in OTA then initiate cell update*/
  if(rrcenhstate_set_hrnti_status_for_transitions())
  {
    rrcrbe_send_initiate_cu_flag = TRUE;
    rrcrbe_delete_crnti          = TRUE;
  }
#endif
      
#ifdef FEATURE_WCDMA_HS_RACH
 /* Check if E-RNTI is not given in OTA then initiate cell update*/
        if(rrchsrach_check_ernti_status())
        {
          rrcrbe_send_initiate_cu_flag = TRUE;
          rrcrbe_delete_crnti          = TRUE;
        }
#endif

      
  /* Call the function that creates and sends the  channel
     config information to be requested from LLC. */
    
  if ((out_cmd_ptr = rrc_get_int_cmd_buf())!=NULL)
  {

    WCDMA_MEMCPY(out_cmd_ptr,
                 sizeof(rrc_cmd_type),
                 &rrc_cmd,
                 sizeof(rrc_cmd_int_type));

    rrc_put_int_cmd( out_cmd_ptr );
  }

  
  /* Change the substate since we're waiting for the
     confirmation from LLC for the channel configs */
  rbe.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
  
}

/*===========================================================================

FUNCTION   rrcrbe_send_rbsetup_failure_message_ext_not_comprehended

DESCRIPTION
  Function to send failure message because message extension is not comprehended.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbe_send_rbsetup_failure_message_ext_not_comprehended
(
  rrc_RadioBearerSetup *msg_ptr
)
{
  if ( (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier)
                 == rbe.tr_ids.rejected_transaction )
  {
    WRRC_MSG1_HIGH("Duplicate RBS msg! rejected tr-id%d match!", rbe.tr_ids.rejected_transaction);
    /* Nothing else to do, just ignore the message and return */
  }
  else
  {

    /* We have received a RBS message with a new transaction id.
    This message should be in our rejected transaction
    list */
    rbe.tr_ids.rejected_transaction =
      (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
    rbe.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
    rbe.status.prot_err_cause = rrc_ProtocolErrorCause_messageExtensionNotComprehended;
    /*Check whether Cell Update is going on */
    if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
         (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
    {
      WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
      (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBE);
      /* Wait until Cell update procedure is completed */
      rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
    }
    else
    {
      send_rbs_failure_message( rbe.status.failure_status,
                                      rbe.status.prot_err_cause );
      /*continue the same substate */
    }
  }
}

#ifdef FEATURE_WCDMA_REL9

/*===========================================================================
FUNCTION   RRCRBE_SEND_CHANNEL_CONFIG_REQ_R9

DESCRIPTION
  This function sends the RRC_CHANNEL_CONFIG_REQ command to LLC
  to setup the necessary RBs associated with a RAB. If signalling
  Radio bearers are affected and included in the Radio Bearer
  Setup Message, they will also be sent in the same command.

  Currently the messege is not expected to have Signalling
  Radio Bearers for setup. If these are added in the future,
  they will also need to be sent in the same command.

DEPENDENCIES
  ORDERED_CONFIG must be set before calling this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void rrcrbe_send_channel_config_req_r9
(
  rrc_RadioBearerSetup *msg_ptr      /* Pointer to the RB Setup message */
)
{
  rrc_cmd_type *out_cmd_ptr;  /* To hold the Command                       */
  uint32 rb_count=0;          /* To count the number of RBs in the command */
  uint32 rbe_rb_cnt = 0;

  uint32 rab_index=0;         /* Used when more than one RAB is received in a RBS message */
  uint32 srb_cnt=0;
  rrc_RB_Identity rb_id=0;

  rrc_RB_InformationSetupList_r8 *rb_ptr = NULL; /* Temporary pointer to RB info      */
  rrc_RB_InformationAffectedList_r8 *affected_rb_ptr = NULL;  /* Pointer to RBs to be Reconfigured */
  rrc_RB_InformationReconfigList_r8 *reconfig_rb_ptr = NULL;
  rrc_RAB_InformationSetupList_r8 *rab_info_ptr = NULL;
  rrc_SRB_InformationSetupList_r8        * srb_setup_ptr = NULL;
  rrcrb_directed_cell_info_type        directed_cell_info; /* directed cell info struct */

  rrc_RadioBearerSetup_r9_IEs* rbsetup_ptr = NULL;
  rrc_state_e_type next_rrc_state = rbe.state_from_msg;

  /* Fill in the command id  */
  rrc_cmd.cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;

  /* Set RBSetup Ptr */
  rbsetup_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
    criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.radioBearerSetup_r9;


  /* First fill up the Radio Bearers that need to be setup. For this we need to know how many RBs need to be setup.
  If RABs need to be setup, check how many RBs are mapped to RABs. */
  if(RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,srb_InformationSetupList))
  {
    /* Get the SRB setup list */
    srb_setup_ptr = &rbsetup_ptr->srb_InformationSetupList;

    while ((srb_setup_ptr->n > srb_cnt) && (srb_cnt < UE_MAX_SRB_SETUP))
    {
      /* Check to see if the RB ID is there ... */
      if (srb_setup_ptr->elem[srb_cnt].m.rb_IdentityPresent)
      {
        rb_id = srb_setup_ptr->elem[srb_cnt].rb_Identity;
      }
      /*If not, then start with the first SRB and get the next SRB ID each time one is not included in the IE.*/
      else
      {
        /* Get the next SRB, note this increments to 1 the first time */
        rb_id++;
      }
  
      /* Fill up the RB id and indicate that it needs to be established */
      rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = rb_id;
      if(rrclcm_get_dl_rlc_lc_id(rb_id) == RRCLCM_RLC_LC_ID_NOT_FOUND )
      {
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = ESTABLISH_RB;
      }
      else
      {
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB_OTA;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = 
        rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = MODIFY_RB_CFG;
      }
      rb_count++;
      srb_cnt++;


    } /* end SRB setup */
  }

  if(RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,rab_InformationSetupList))
  {
    rab_info_ptr = &rbsetup_ptr->rab_InformationSetupList;
    for (rab_index=0; rab_index < rbe.rabs.num_rabs_in_msg && rab_info_ptr->n > rab_index; rab_index++)
    {
      if ( rbe.rabs.chan_config_needed[rab_index] == TRUE)
      {
        rb_ptr = &rab_info_ptr->elem[rab_index].rb_InformationSetupList;

        if ( rb_ptr->n == 0 )
        {
          WRRC_MSG0_HIGH("REL9:No RBs present in msg!");
        }
        else
        {
          /* Get the RBE db rab index */
          uint8 rbe_rab_index = rrcrbe_get_rab_index_from_rbe_db(
                                   rrcrb_translate_gsm_map_rab_id(rab_info_ptr->elem[rab_index].rab_Info.rab_Identity.u.gsm_MAP_RAB_Identity));
          rbe_rb_cnt = 0;
          if((rbe_rab_index < RRCRB_MAX_RAB_PER_MSG) && (rbe_rab_index < rbe.rabs.num_rabs_in_msg ))
          {
            while ( rb_ptr->n > rbe_rb_cnt )
            {          
              /* Fill up the RB id and indicate that it needs to be established */
              rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = rb_ptr->elem[rbe_rb_cnt].rb_Identity;
              rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = ESTABLISH_RB;

              /* Update the local structure so that we know which RB id's we've requested to be setup */
              rbe.rabs.rabs_in_msg[rbe_rab_index].rb_for_rab[rbe_rb_cnt].rb_id = rb_ptr->elem[rbe_rb_cnt].rb_Identity;

              /* Increment count of RBs */
              rb_count++;
              rbe_rb_cnt ++;
    
              /* Do a safety check on the count. We can't do more than 32
                       configs in a command */
              if ( (rb_count >= MAX_RB) || (rbe_rb_cnt >= MAX_RB_PER_RAB))
              {
                WRRC_MSG0_HIGH("MAX RB count exceeded!");
                break;
              }
              /* Go to the next RB */
            }/* end while */
            /* Save the number of RBs for this RAB */
            rbe.rabs.rabs_in_msg[rbe_rab_index].num_rbs_for_rab = rbe_rb_cnt;
          }
          else
          {
            WRRC_MSG1_ERROR("RBE RAB INDEX not found!!! RAB_INDEX %d",rbe_rab_index);
          }
        }
      }/* end if rabs_setup_needed */
    }
  }
  /* Now check if the message contains an RB information to be
     reconfig list. If so, we need to reconfigure some RBs */
  if(RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,rb_InformationReconfigList))
  {
    uint32 local_idx=0;
    WRRC_MSG0_MED("REL9:RB info reconfig list present");
  
    reconfig_rb_ptr = &rbsetup_ptr->rb_InformationReconfigList;
  
    while ( reconfig_rb_ptr->n > local_idx )
    {
  
      /* Fill up the RB id and indicate that it needs to be reconfigured */
      rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = reconfig_rb_ptr->elem[local_idx].rb_Identity;
      rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB;
      rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB_OTA;
      rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = 0;
      rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = 0;
      if((reconfig_rb_ptr->elem[local_idx].m.rlc_InfoPresent)&&
         (reconfig_rb_ptr->elem[local_idx].rlc_Info.m.ul_RLC_ModePresent))
      {
        rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = MODIFY_RB_CFG;
      }
      if((reconfig_rb_ptr->elem[local_idx].m.rlc_InfoPresent)&&
         (reconfig_rb_ptr->elem[local_idx].rlc_Info.m.dl_RLC_ModePresent))
      {
        rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = MODIFY_RB_CFG;
      }
  
      if (reconfig_rb_ptr->elem[local_idx].m.rb_StopContinuePresent)
      {
        /* over-ride rb_config with STOP or continue */
        if (rrc_RB_StopContinue_stopRB == reconfig_rb_ptr->elem[local_idx].rb_StopContinue)
        {
          rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask |= STOP_RB_CFG;
          rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask |= STOP_RB_CFG;
        }
        else if (rrc_RB_StopContinue_continueRB == reconfig_rb_ptr->elem[local_idx].rb_StopContinue)
        {
          rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask |= CONTINUE_RB_CFG;
          rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask |= CONTINUE_RB_CFG;
          
        }
      }
  
      /* Increment count of RBs. This is done here since we
      need to use the next index that's not used already. */
      rb_count++;
  
      /* Go to the next affected RB */
      local_idx++;
  
      /* Do a safety check on the count. We can't do more than 32
      configs in a command */
      if ( rb_count >= MAX_RB )
      {
        WRRC_MSG0_ERROR("REL9:MAX RB count exceeded!");
        break;
      }
    } /* end while */
  }
  
  /* Now check if the message contains an RB information to be
   affected list. If so, we need to reconfigure some RBs */
  if(RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,rb_InformationAffectedList))
  {
   uint32 local_idx=0;
  
    affected_rb_ptr = &rbsetup_ptr->rb_InformationAffectedList;
  
    if ( affected_rb_ptr->n == 0 )
    {
      WRRC_MSG0_HIGH("REL9:No affected RBs in msg!");
    }
    else
    {
      while ( affected_rb_ptr->n > local_idx )
      {
  
        /* Fill up the RB id and indicate that it needs to be reconfigured */
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_id = affected_rb_ptr->elem[local_idx].rb_Identity;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].rb_config = RECONFIG_RB_OTA;
        rrc_cmd.cmd.chan_config_req.rb[rb_count].ul_cfg_mask = rrc_cmd.cmd.chan_config_req.rb[rb_count].dl_cfg_mask = MODIFY_RB_CFG;
  
        /* Increment count of RBs. This is done here since we
        need to use the next index that's not used already. */
        rb_count++;
  
        /* Go to the next affected RB */
        local_idx++;
  
        /* Do a safety check on the count. We can't do more than 32
        configs in a command */
        if ( rb_count >= MAX_RB )
        {
          WRRC_MSG0_HIGH("REL9:MAX RB count exceeded!");
          break;
        }
      } /* end while */
    }
  }
  else
  {
    MSG_LOW("No affected RB list in msg", 0, 0, 0);
  }/* end if affected list present */


  /* Number of RBs to config. Note rb_count is the total
   of established and reconfigured RBs */
  rrc_cmd.cmd.chan_config_req.num_rb_to_config = (int) rb_count;

  if(rb_count < MAX_RB)
  {
    rrc_fill_rb_id_chan_req_from_standalone_db(&rb_count, &rrc_cmd.cmd.chan_config_req);
  }

  if(rbe.state_from_msg == RRC_STATE_CELL_DCH)
  {
     /* Fill those RBs in chan_config_req for which RLC PDU size has changed and 
     that are already not present in chan_config_req*/
    if(rb_count < MAX_RB)
    {
      rrc_fill_rb_id_chan_req_from_rb_list_mapped_to_dch(&rb_count, &rrc_cmd.cmd.chan_config_req);
    }
    if(rrc_get_state() == RRC_STATE_CELL_FACH)
    {
      rrc_fill_srb_in_chan_config_req(&rb_count, &rrc_cmd.cmd.chan_config_req);
    }
  }
 
  /*If state transition is Cell_DCH->Cell_FACH or vice-verca, add all the existing 
  AM PS RBs in chan_config_req*/  
  if(((next_rrc_state == RRC_STATE_CELL_FACH) && 
       (transition_config.toc_usage == TOC_FOR_DCH_AND_FACH ||
        transition_config.toc_usage == TOC_FOR_FACH_AND_PCH ||
        transition_config.toc_usage == TOC_FOR_FACH_AND_PCH_WITH_DCH_INFO))
       ||
      ((rrc_get_state() == RRC_STATE_CELL_FACH) && (next_rrc_state == RRC_STATE_CELL_DCH))
    )
  {
    rrc_fill_rb_id_chan_req_from_est_rabs(&rb_count, &rrc_cmd.cmd.chan_config_req);
  }
  /* Check if state change is required */
  if ( (rrc_get_state()) == (rbe.state_from_msg) )
  {
    rrc_cmd.cmd.chan_config_req.rrc_state_change_required = FALSE;
  }
  else
  {
    /* The message indicates that state change is required.
    Indicate to LLC what the next state should be. */
    rrc_cmd.cmd.chan_config_req.rrc_state_change_required = TRUE;
    rrc_cmd.cmd.chan_config_req.next_state = rbe.state_from_msg;
  }

  /* Indicate the id of the procedure sending the command */
  rrc_cmd.cmd.chan_config_req.procedure = RRC_PROCEDURE_RBE;

  /* Indicate that we need a confirmation for this command */
  rrc_cmd.cmd.chan_config_req.rrc_channel_config_cnf_required = TRUE;

  /* We are not required to fill any other data for this command.
   Now, send the command */
  if (((rrc_get_state()) == RRC_STATE_CELL_DCH) &&
      (rrc_cmd.cmd.chan_config_req.next_state== RRC_STATE_CELL_FACH) &&
      (rrc_cmd.cmd.chan_config_req.rrc_state_change_required))
  {
    /*Initiate the cell selection*/
    rrcrb_send_initiate_cell_selection_req(RRC_PROCEDURE_RBE,
            RRC_TRANSITION_FROM_DCH, rbe.state_from_msg);

    rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

    /* Start T_305 [periodic cell update] timer */
    rrccu_start_t_305_timer();
  }
  else
  {
    /*check whether any frequency info is present in the message
    and transition is CELL_FACH->CELL_FACH. In this case
    initiate a cell update after channel config is successfully completed */
    if (((rrc_get_state())==RRC_STATE_CELL_FACH)&&
        (rbe.state_from_msg==RRC_STATE_CELL_FACH))
    {
      if((RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,frequencyInfo)) &&
          (rbsetup_ptr->frequencyInfo.modeSpecificInfo.t == T_rrc_FrequencyInfo_modeSpecificInfo_fdd))
      {
        directed_cell_info.freq_info_present = TRUE;
        directed_cell_info.uarfcn_dl = rbsetup_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
      }
      else
      {
        directed_cell_info.freq_info_present = FALSE;
      }
      if ((RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,dl_InformationPerRL_List)) &&
          (rbsetup_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.t == 
          T_rrc_DL_InformationPerRL_r8_modeSpecificInfo_fdd))
      {
        directed_cell_info.pscr_present = TRUE;
        directed_cell_info.pscr = rbsetup_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
      }
      else
      {
        directed_cell_info.pscr_present = FALSE;
      }
      if (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE)
      {
        /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection 
          FACH->FACH ), treat it as un-supported configuration and send failure.*/ 
        if(rrccsp_check_initiate_cell_selection_handling(NULL) == FALSE)
        {
          WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
       
          rrcrbe_send_initiate_cu_flag = TRUE;
          rrcrbe_delete_crnti          = TRUE;
        }
        else
        {
          rrc_int_c_rnti_type      c_rnti;       /* Local C_RNTI value */
          if(RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,new_C_RNTI))
          {
            rbe.new_crnti_valid = TRUE;
            rrc_translate_crnti(&rbsetup_ptr->new_C_RNTI,
                                &c_rnti);
            rbe.new_crnti = (uint16)c_rnti;
          }
          else
          {
            rbe.new_crnti_valid = FALSE;
          }
          rrcllcoc_update_toc_with_oc_from_fach();
          
          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req(RRC_PROCEDURE_RBE,
                                                  RRC_TRANSITION_FROM_FACH,
                                                  rbe.state_from_msg);
       
          rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
              
          return;
        }
      }

    } /* if CELL_FACH->CELL_FACH reconfiguration */

    /* Get a Command buffer and copy the local command to the buffer so as to 
    enqueue it on the internal queue. */
    if ((out_cmd_ptr = rrc_get_int_cmd_buf())!=NULL)
    {
      WCDMA_MEMCPY(out_cmd_ptr,
                   sizeof(rrc_cmd_type),
                   &rrc_cmd,
                   sizeof(rrc_cmd_int_type));
      rrc_put_int_cmd( out_cmd_ptr );
    }
  }
}





/*===========================================================================
FUNCTION   VALIDATE_RBSETUP_R9

DESCRIPTION
  This function validates the received R9 RB Setup Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a Radio Bearer Setup Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void validate_rbsetup_r9
(
  rrc_RadioBearerSetup *msg_ptr      /* Pointer to the RB Setup message */
)
{
  rrc_RadioBearerSetup_r9_IEs* rbsetup_ptr = NULL;

  boolean new_tm_rb = FALSE;

#ifdef FEATURE_VOC_AMR_WB
  mvs_mode_type nas_sync_ind=rrc_get_current_codec_mode();
#endif /*FEATURE_VOC_AMR_WB*/

  /* Assume the message is valid for now */
  rbe.status.failure_status = RRCRB_MSG_VALID;

  rbsetup_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
    u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.radioBearerSetup_r9;

  switch (rbsetup_ptr->rrc_StateIndicator)
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      rbe.state_from_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      rbe.state_from_msg = RRC_STATE_CELL_FACH;
      break;

    default: /* Just to satisfy LINT */
      WRRC_MSG0_ERROR("REL9:Invalid RRC state from Msg");
      rbe.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
      break;

  } /* End of switch */

  if ( rbe.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
    return;
  }

  /* Check if there are new RABs to be established */
  if(RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,rab_InformationSetupList))
  {
    if(rrcrbe_prevalidate_rab_setup_list_r8(&rbsetup_ptr->rab_InformationSetupList,&new_tm_rb) == FAILURE)
    {
      return;
    }
  }
  else
  {
    /*There is no RAB to be setup in this message.*/
    rbe.rabs.num_rabs_in_msg = 0;
  }  

  if(RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,dl_CounterSynchronisationInfo))
  {
    /*check whether the SRNS relocation is within Cell_DCH or not.
      SRNS relocation information can come in reconfiguration messages
      only when the state transition is Cell_DCH->Cell_DCH */
    if (!((rbe.state_from_msg == RRC_STATE_CELL_DCH) &&
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("REL9:SRNS reloc with invalid states cur_state %d, msg_state %d",
          rrc_get_state(), rbe.state_from_msg);
      rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
      return;
    }

    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has
       PDCP info list. If it is true then this configuration is unsupported */
    if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2(rbsetup_ptr->dl_CounterSynchronisationInfo,
		rb_WithPDCP_InfoList)) ||
        (RRC_MSG_COMMON_BITMASK_IE(rbsetup_ptr->dl_CounterSynchronisationInfo, 
        rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
    {
      WRRC_MSG0_HIGH("REL9:PDCP info in DL counter sync info not supported");
      rbe.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation.
      Set the flag that indicates that this procedure
      involves SRNS relocation */
      rbe.rbe_for_srns_relocation = TRUE;
      /* store if new U-RNTI is present in the message */
      if (RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,new_U_RNTI))
      {
        rbe.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(rbsetup_ptr->new_U_RNTI), &rbe.new_urnti);
      }
    }
  }

  /* Process RAB reconfig before RAB setup because we want to catch NW's which
    are trying to setup and reconfig RAB with the same message */
  if(RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,rab_InformationReconfigList))
  {

    rbe.status.failure_status = rrc_update_rab_info_reconfig_param_r8(
    &rbsetup_ptr->rab_InformationReconfigList, &rbe.rabs 
#ifdef FEATURE_VOC_AMR_WB
                                           ,&nas_sync_ind
#endif/*FEATURE_VOC_AMR_WB*/
                                         );
    if( rbe.status.failure_status != RRCRB_MSG_VALID) 
    {
      return;
    }
  }

  if(RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,rab_InformationSetupList))
  {
    if(rrcrbe_validate_rab_setup_list_r8(&rbsetup_ptr->rab_InformationSetupList, 
        rbe.rabs.num_rabs_in_msg
#ifdef FEATURE_VOC_AMR_WB          
        ,nas_sync_ind
#endif /*FEATURE_VOC_AMR_WB*/          
        ) == FAILURE)
    {
      return;
    }
  }

  WRRC_MSG1_MED("REL9:%d RABs  in an RBSetup",  rbe.rabs.num_rabs_in_msg);

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch */
  rbe.trans_to_dch_ciph_config_needed = FALSE;

  if((rbe.state_from_msg == RRC_STATE_CELL_DCH) && (rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,dl_CommonInformation)) && 
       (RRC_MSG_COMMON_BITMASK_IE(rbsetup_ptr->dl_CommonInformation,
       rrc_DL_CommonInformation_r8,dl_dpchInfoCommon)) &&
       (T_rrc_DL_CommonInformation_r8_dl_dpchInfoCommon_dl_DPCH_InfoCommon
       == rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.t))
    {
      //check if ciphering is active in mac for cs domain, if it is then,
      //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      if(rrcsmc_tm_ciph_active() == TRUE)
      {
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        uecomdef_status_e_type loc_ret_val;
        WRRC_MSG0_HIGH("REL9:To-DCH-Step-1 config activated");
        loc_ret_val = rrcrb_get_mac_d_hfn(
             ( RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon,rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
              &rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
              &mac_d_hfn_l);
        if (loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("REL9:get_mac_d_hfn failed");
        }
        if(rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
        {
           WRRC_MSG0_HIGH("REL9:SMC process ciphering config for HHO failed");
        }
        rbe.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("REL9:To-DCH-Step-1 not needed");
      }
    }
  }

  if(rbe.state_from_msg == RRC_STATE_CELL_DCH)
  {
    rbe.first_tm_rb_setup = rrcrbe_is_first_tm_rb_setup(new_tm_rb);
    if(new_tm_rb)
    {
      WRRC_MSG1_HIGH("First TM RB being set status : %d",rbe.first_tm_rb_setup);
    }
    if(rbe.first_tm_rb_setup == TRUE)
    {
      //check to see if dl-dpch-common info is present while going to dch
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr,
         rrc_RadioBearerSetup_r9_IEs,dl_CommonInformation)) && 
         (RRC_MSG_COMMON_BITMASK_IE(rbsetup_ptr->dl_CommonInformation,
         rrc_DL_CommonInformation_r8,dl_dpchInfoCommon)) &&
         (T_rrc_DL_CommonInformation_r8_dl_dpchInfoCommon_dl_DPCH_InfoCommon == 
          rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.t))
      {
        uecomdef_status_e_type loc_ret_val;
        //check if ciphering is active in mac for cs domain, if it is then,
        //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        
        WRRC_MSG0_HIGH("First time TM RB Setup HHO Step-1 config activated");
        loc_ret_val = rrcrb_get_mac_d_hfn(
             ( RRC_MSG_COMMON_BITMASK_IE_PTR(rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon,rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
              &rbsetup_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.
               dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
              &mac_d_hfn_l);
        if (loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("get_mac_d_hfn failed");
        }
        if(rrcsmc_process_cipher_config_for_first_tm_setup(mac_d_hfn_l) == FAILURE)
        {
          WRRC_MSG0_HIGH("SMC process ciphering config for First TM RB Setup failed");
          /*This step is reached only when the first step returns a failure,. i.e, if the ciphering is not active for CS domain.
            Hence setting rbe.first_tm_rb_setup to FALSE so that step 2 is not done*/
            rbe.first_tm_rb_setup = FALSE;
        }

      }
    }
  }

  /* If UTRAN DRX coefficient is not present then it is invalid configuration */
  if ((rbe.state_from_msg == RRC_STATE_CELL_PCH) ||
      (rbe.state_from_msg  == RRC_STATE_URA_PCH))
  {

    if (!(RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,utran_DRX_CycleLengthCoeff)))
    {
      WRRC_MSG1_ERROR("REL9:UTRAN DRX Coef not preset, State:%d", rbe.state_from_msg);
      rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }

  if(RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,rb_InformationReconfigList))
  {
    /* Save the linked list head pointer */
    if (FAILURE == rrcllc_validate_rb_info_reconfig_list_r8(&rbsetup_ptr->rb_InformationReconfigList))
    {
      rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }

  if (rbe.state_from_msg == RRC_STATE_URA_PCH)
  {
    /*should we do it after getting l2ack for response??*/
    if (!(RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,ura_Identity)))
    {
      WRRC_MSG0_HIGH("REL9:URA ID not present");
      rrccu_clear_ura_id ();
    }
    else
    {
      /*URA ID present in the message set ura_identity variable with this value */
      if (rbsetup_ptr->ura_Identity.numbits == 16)
      {
        rrccu_update_ura_id (&(rbsetup_ptr->ura_Identity));
      }
      else
      {
        WRRC_MSG1_ERROR("REL9:Invalid URA ID length%d", rbsetup_ptr->ura_Identity.numbits);
        rbe.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
        rbe.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
        return;
      }
    }
  }

  return;
}







/*===========================================================================
FUNCTION   process_rbsetup_r9

DESCRIPTION
  This function processes the Rel 9 RB Setup Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a Radio Bearer Setup Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -

    ORDERED_CONFIG will be set by calling LLC.

    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to configure the Radio Bearers given by the message.

    If ORDERED_CONFIG could not be set, a Radio Bearer Setup Failure message
    will be sent to UTRAN.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void process_rbsetup_r9
(
  rrc_cmd_type *cmd_ptr
)
{
  rrc_DL_DCCH_Message     *dcch_msg_ptr;   /* Pointer to the RB Setup message */
  rrc_RadioBearerSetup *msg_ptr;    /* Pointer to the RB Setup message        */

  rrc_rab_search_e_type rab_status = RRC_RAB_NOT_FOUND; /* To check if RAB is present in ESTABLISHED_RABS */
  rrcllc_oc_status_e_type oc_status;/* To hold ordered_config status          */
  uint32 rab_index=0;               /* Used when more than 1 rab is present in
                                    a RBS message                             */
  rrc_int_c_rnti_type      c_rnti;       /* Local C_RNTI value */
  boolean ordered_config_err_occured = FALSE; /* Used to indicate if an error
                                                 occured while setting ordered_config. */
  rrc_msg_ver_enum_type msg_version = MSG_REL9;

  rrc_RadioBearerSetup_r9_IEs* rbsetup_ptr = NULL;

  rrcsmc_srns_ip_check_status_e_type    ip_check_status = RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE;


  WRRC_MSG1_MED("REL9:RRC cur_state %d", rbe.curr_substate);

  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  /* Get the pointer to the actual RB Setup message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerSetup);

  /* Store Msg Ptr in a local variable */
  rbsetup_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
    u.later_r8_criticalExtensions.u.r9.radioBearerSetup_r9;

  if (TRUE == rrcrbe_validate_srns_relocation_info(msg_ptr, msg_version))
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
      WRRC_MSG0_ERROR("REL9:IP check failed. Discarding RBS msg");
      break;

    case RRCSMC_SRNS_SECURITY_INFO_INVALID:
    case RRCSMC_INCOMPATIBLE_SECURITY_RECONFIGURATION:
      if (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier
          == rbe.tr_ids.rejected_transaction)
      {
        WRRC_MSG1_HIGH("REL9:Duplicate RBE msg - rejected tr-id%d match!",
                 rbe.tr_ids.rejected_transaction);
        /* Nothing else to do, just ignore the message and return */
      }
      else
      {
        /* send a reconfig failure messge with invalid
        configuration and
          continue the same substate with any ongoing procedure */
        rbe.tr_ids.rejected_transaction =
        (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
        if (ip_check_status == RRCSMC_SRNS_SECURITY_INFO_INVALID)
        {
          rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        }
        else
        {
          rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
        }
        /*Check whether Cell Update is going on */
        if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
             (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
        {
          if(rbe.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
          {
            WRRC_MSG2_HIGH("rbe subsstate(%d) Send failure with %d cause", rbe.curr_substate, rbe.status.failure_status);
            send_rbs_failure_message( rbe.status.failure_status,
                                     rbe.status.prot_err_cause );
            rbe.status.failure_status = RRCRB_MSG_VALID;
          }
          else
          {
            WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
            (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBE);
            /* Wait until Cell update procedure is completed */
            rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
          }
        }
        else
        {
          send_rbs_failure_message( rbe.status.failure_status,
                                    rbe.status.prot_err_cause );
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
    if (ip_check_status != RRCSMC_SRNS_INTEGRITY_CHECK_SUCCESS)
    {
      /*do not clear the procedure or change the rbe substate because
      some other rbe procedure might be in progress. Just return from
        here */
      return;
    }

  } /* End of validation of srns relocation */

  if ((rrcllc_get_ordered_config_status() != OC_NOT_SET) ||
      (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
      (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
  {
    WRRC_MSG0_HIGH("REL9:Incomatible simultaneous reconfig! Rejecting RBS message");
    if ( (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier)
         == rbe.tr_ids.accepted_transaction )
    {
      WRRC_MSG1_HIGH("REL9:Duplicate RBS msg! accepted tr-id%d match!",
               rbe.tr_ids.accepted_transaction);
      /* Nothing else to do, just return */
    }
    else if ( (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier)
                == rbe.tr_ids.rejected_transaction )
    {
      WRRC_MSG1_HIGH("REL9:Duplicate RBS msg! rejected tr-id%d match!", rbe.tr_ids.rejected_transaction);
      /* Nothing else to do, just return */
    }
    else
    {
      /* We have received a RBS message with a new transaction id.
      This message should be in our rejected transaction
        list */
      rbe.tr_ids.rejected_transaction =
      (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
      rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;

      /*Check whether Cell Update is going on */
      if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
           (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
      {
        if(rbe.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
        {
          send_rbs_failure_message( rbe.status.failure_status,
                                   rbe.status.prot_err_cause );
          rbe.status.failure_status = RRCRB_MSG_VALID;
        }
        else
        {
          WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
          (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBE);
          /* Wait until Cell update procedure is completed */
          rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        }
      }
      else
      {
        send_rbs_failure_message( rbe.status.failure_status,
                                  rbe.status.prot_err_cause );
        /*continue the same substate */
      }
    }
    return;
  }

  /* We have received a RB setup message with a new transaction id.
  Accept the transaction Id */
  rbe.tr_ids.accepted_transaction =
  msg_ptr->u.later_than_r3.rrc_TransactionIdentifier;

  validate_rbsetup_r9(msg_ptr);

  if ( rbe.status.failure_status != RRCRB_MSG_VALID )
  {
    /* RB Message was invalid. */
    WRRC_MSG0_HIGH("REL9:RB Message validation failed");
    /* Send the RB Setup Failure Message */
    send_rbs_failure_message( rbe.status.failure_status, rbe.status.prot_err_cause);

    /* Make sure we're back in the initial sub-state */
    rbe.curr_substate = RRCRB_INITIAL;
    rrcrbe_clear_procedure();
  }
  /* If the message is valid, proceed further */
  else
  {
    rrcrbe_indicate_layer1_if_rb_setup_configure_voice_rab();

     /* If new rabs are setup, update mac rab status */
    if(RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,rab_InformationSetupList))
    {
      rrc_RAB_InformationSetupList_r8 *rab_list_ptr = &rbsetup_ptr->rab_InformationSetupList;
      rrcrb_update_mac_rab_status(RRC_PROCEDURE_RBE,
                                 (rrc_cn_domain_identity_e_type)(rab_list_ptr->elem[0].rab_Info.cn_DomainIdentity) ,
                                  NULL,FALSE);
    }

    /*calculate ciphering_activation_cfn only if ciphering is enabled*/
    if (rrcsmc_is_ciphering_enabled() == RRCSMC_SECURITY_STARTED)
    {
      if(RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,activationTime))
      {
        /*256 is the CFN rollover length*/
        /*250 is used to activate ciphering after 250 10ms time interval,
        this should give sufficient time for n/w side and ue side to have
        configured ciphering on the rb's successfully*/
        ciphering_activation_cfn =
        (uint8) ((rbsetup_ptr->activationTime + 250) % 256);
        /*round it to 8 frame boundary */
        ciphering_activation_cfn = (uint8)((ciphering_activation_cfn/8)*8);

      }
      else
      {
        /*256 is the CFN rollover length*/
        /*250 is used to activate ciphering after 250 10ms time interval,
        this should give sufficient time for n/w side and ue side to have
        configured ciphering on the rb's successfully*/
        ciphering_activation_cfn = (uint8)((seq_get_cfn() + 250) % 256);
        /*round it to 8 frame boundary */
        ciphering_activation_cfn = (uint8)((ciphering_activation_cfn/8)*8);
      }
    }

    /*call rrcsmc function to update ciphering activation cfn*/
    rrcsmc_update_ciphering_activation_cfn(ciphering_activation_cfn);

    /* Check if there is CN information present. */
    if(RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,cn_InformationInfo))
    {
      rbe.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(
        &(rbsetup_ptr->cn_InformationInfo) );
      if (rbe.mm_cmd_ptr == NULL)
      {
        WRRC_MSG0_ERROR("REL9:Could not build SERVICE_IND cmd");
      }
      else
      {
        /* set cn_info_saved flag */
        rbe.cn_info_saved = TRUE;
      }
    }

#ifdef FEATURE_WCDMA_REL7_SPECRS
    /*If RBE OTA provides "supportForChangeOfUE_Capability" IE, we need to store it in
           "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
          "rrc_rnc_cap_change_support" can be updated accordingly. */  	
    if(RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,supportForChangeOfUE_Capability))
    {
      if(rbsetup_ptr->supportForChangeOfUE_Capability == TRUE)
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
#endif  /*FEATURE_WCDMA_REL7_SPECRS*/

    /* Check if there are new RABs to be established or if there are
       affected RB or SRBs to be setup */

    if ((RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,rab_InformationSetupList)) ||
        (((RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,rb_InformationAffectedList))  
          ) ||
        ((RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,srb_InformationSetupList)) 
         )))
    {
      /* Check if the RAB id is already in ESTABLISHED_RABS */
      for (rab_index=0; rab_index < rbe.rabs.num_rabs_in_msg; rab_index++)
      {
        rab_status = rrc_find_rab_in_established_rabs(rbe.rabs.rabs_in_msg[rab_index].rab_id );
        rbe.rabs.chan_config_needed[rab_index] = TRUE;

        if ((rab_status != RRC_RAB_NOT_FOUND) &&
             (rrc_find_if_voice_rab_exists_in_established_rabs( rbe.rabs.rabs_in_msg[rab_index].rab_id )
               == FALSE))
        {
          WRRC_MSG1_HIGH("REL9:RAB %d already exists in EST_RABS!",
                   rbe.rabs.rabs_in_msg[rab_index].rab_id);
         /* send a failure message if the rab in the message  already
           exists in   established_rabs. Later add a check for the rbs
           associated with the rabs so that the failure will be sent only when
           any of the rb-ids match with the rb-ids in established_rab for that
           rab */
          rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
          /* Send the RB Setup Failure Message */
          send_rbs_failure_message( rbe.status.failure_status,
                                    rbe.status.prot_err_cause
                                    );

        /* Make sure we're back in the initial sub-state */
          rbe.curr_substate = RRCRB_INITIAL;
          rrcrbe_clear_procedure();
          return;
        }
      }

      /* First set ORDERED_CONFIG so that LLC can process the necessary parameters.
      This function will copy all RABs from the rxd msg to oc. */
      oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_RBE, rbe.state_from_msg,
                                             rrc_DL_DCCH_Message_PDU, (void *)dcch_msg_ptr);

      switch ( oc_status )
      {
        case ORDERED_CONFIG_SET:
          if(rrc_sib_is_current_event_sib_read_in_dch())
          {
             WRRC_MSG0_HIGH("REL9:RRCGPS: Force Init GPS ");
             rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
          }

          /* Ordered Config has been set. We can setup the
            Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */

          WRRC_MSG0_HIGH("REL9:Sending RRC_CHANNEL_CONFIG_REQ to LLC");
          rrcrbe_send_channel_config_req_r9(msg_ptr);

          if(((rrc_get_state()) == RRC_STATE_CELL_DCH) &&
              (rrc_cmd.cmd.chan_config_req.next_state== RRC_STATE_CELL_FACH) &&
              (rrc_cmd.cmd.chan_config_req.rrc_state_change_required))
          {
            if(RRCRB_R9_MSG_IE_PRESENT(rbsetup_ptr,new_C_RNTI))
            {
              rbe.new_crnti_valid = TRUE;
              rrc_translate_crnti(&rbsetup_ptr->new_C_RNTI, &c_rnti);
              rbe.new_crnti = (uint16)c_rnti;
            }
            else
            {
              rbe.new_crnti_valid = FALSE;
            }
            rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
          }
          else
          {
            if(rbe.curr_substate != RRCRB_WAIT_FOR_CELL_SELECTION_CNF)
            {
               /*Change the substate since we're waiting for the confirmation from LLC for the channel configs */
               rbe.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
            }
          }
          break;

        case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

          rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
          /* Send the RB Setup Failure Message */
          send_rbs_failure_message( rbe.status.failure_status, rbe.status.prot_err_cause);

          /* Go back to initial substate */
          rbe.curr_substate = RRCRB_INITIAL;

          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;

        case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

          rbe.status.failure_status =RRCRB_MSG_UNSUPPORTED_CONFIG;
          /* Send the RB Setup Failure Message */
          send_rbs_failure_message( rbe.status.failure_status, rbe.status.prot_err_cause);

          /* Go back to initial substate */
          rbe.curr_substate = RRCRB_INITIAL;

          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;
          
          case ORDERED_CONFIG_CONFIGURATION_INVALID:
 
           rbe.status.failure_status =RRCRB_MSG_INVALID_CONFIG;
           /* Send the RB Setup Failure Message */
           send_rbs_failure_message( rbe.status.failure_status, rbe.status.prot_err_cause);
 
           /* Go back to initial substate */
           rbe.curr_substate = RRCRB_INITIAL;
 
           /* Indicate that an error occured */
           ordered_config_err_occured = TRUE;
           break;
        case ORDERED_CONFIG_NOT_SET_OTHER:
          WRRC_MSG0_HIGH("REL9:LLC returned other failure!");
          /* When LLC sends this, there is a serious problem
          and we'll go back to disconnected state. No need
          to send the failure message here, we can just
          go back to initial substate */
          rbe.curr_substate = RRCRB_INITIAL;

          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME          
        case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
          WRRC_MSG0_HIGH("RB setup processing deferred due to LTA!");
          rbe.curr_substate = RRCRB_INITIAL;
        
          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
        default:

          WRRC_MSG0_ERROR("REL9:Unknown return value from LLC");
          /* Unknown error, just go back to initial substate */
          rbe.curr_substate = RRCRB_INITIAL;

          /* Indicate that an error occured */
          ordered_config_err_occured = TRUE;
          break;
        }/* end switch */

        if ( ordered_config_err_occured == TRUE )
        {
          /* No need to continue. Clear the procedure
            variables and return. */
          rrcrbe_clear_procedure();
          return;
        }

        if (rbe.rbe_for_srns_relocation == TRUE)
        {
          rrcsmc_commit_fresh_for_srns_relocation();
        }
    }
    else
    {
      /*If we are here, it means that RABs are not given in RB-Setup.
      Also, SRBs and RB Affected IEs are not given.*/
      WRRC_MSG0_ERROR("REL9:No RB IEs are given in the OTA.");
      rbe.status.failure_status= RRCRB_MSG_INVALID_CONFIG;
      rbe.status.prot_err_cause = rrc_ProtocolErrorCause_informationElementMissing;  

      /* Send the RB Setup Failure Message */
      send_rbs_failure_message(rbe.status.failure_status, rbe.status.prot_err_cause);

      /* Make sure we're back in the initial sub-state */
      rbe.curr_substate = RRCRB_INITIAL;
      rrcrbe_clear_procedure();

      return;  
    }/* end if - RBs need to be reconfigured */
  } /* else if valid rb message */
}/* end function process_rbsetup_message_r9 */

#endif /* FEATURE_WCDMA_REL9 */


#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif /* FEATURE_WCDMA_REL10 */


/*===========================================================================

FUNCTION   PROCESS_RBSETUP_MESSAGE

DESCRIPTION

  This function processes the received RB Setup Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a Radio Bearer Setup Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -

    ORDERED_CONFIG will be set by calling LLC.

    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to configure the Radio Bearers given by the message.

    If ORDERED_CONFIG could not be set, a Radio Bearer Setup Failure message
    will be sent to UTRAN.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_rbsetup_message
(
rrc_cmd_type *cmd_ptr
)
{

  rrc_DL_DCCH_Message     *dcch_msg_ptr; /* Pointer to the RB Setup message */
  rrc_RadioBearerSetup *msg_ptr;    /* Pointer to the RB Setup message        */
  rrc_rab_search_e_type rab_status = RRC_RAB_NOT_FOUND; /* To check if RAB is present in ESTABLISHED_RABS */
  rrcllc_oc_status_e_type oc_status;/* To hold ordered_config status          */
  uint32 rab_index=0;               /* Used when more than 1 rab is present in
                                    a RBS message                             */
  rrc_int_c_rnti_type      c_rnti;       /* Local C_RNTI value */
  boolean ordered_config_err_occured = FALSE; /* Used to indicate if an error
                                    occured while setting ordered_config. */

   /*Boolean used to indicate primary plmn i.d is present in OTA msg*/
  boolean primary_plmn_Id_present = FALSE;
   /*Variable to hold primary plmn i.d if present in OTA msg*/
  rrc_PLMN_Identity primary_plmn_Identity;


  rrcsmc_srns_ip_check_status_e_type    ip_check_status = RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE;

  rbe.first_tm_rb_setup = FALSE;

  WRRC_MSG1_MED("Recvd RB Setup Msg cur_state %d", rbe.curr_substate);


  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.
                          downlink_sdu_ind.decoded_msg;
  /* If ASN.1 decoding failed, the received pointer would be null.
  So we know we got a RB Setup Message (by peeking into
  the encoded bit stream), but we were not able to decode it.
  Send a RB setup failure message with the appropriate
  cause. */
  if ( dcch_msg_ptr == NULL )
  {
    WRRC_MSG0_HIGH("ASN decode failed for RBS msg");
    rbe.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
    rbe.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
    rbe.tr_ids.rejected_transaction = 0;
    send_rbs_failure_message( rbe.status.failure_status,
                              rbe.status.prot_err_cause
                            );

    /* Nothing else to do here */
    /*do not change the current state. Some other RBsetup procedure might be
    on going. Maintain the current state and do not clear the procedure */
    return;
  }

  /* Make sure it's the Radio Bearer Setup message
  although we would not have gotten here unless it's
  the right message */
  if ( dcch_msg_ptr->message.t != T_rrc_DL_DCCH_MessageType_radioBearerSetup )
  {
    WRRC_MSG1_ERROR("Incorrect message given to RBE proc %d", dcch_msg_ptr->message.t);
    return;
  }

  /* Get the pointer to the actual RB Setup message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerSetup);

  /* The first thing to do is check the transaction id.
  If the transaction id is the same as the accepted
  or rejected transaction ids, we don't process
  this message. It's a duplicate and will just be
  dropped. But we also don't change the current
  sub-state since we're in the middle of
  processing a previous message. Note that
  the transaction id is included only if critical
  extensions are not present. */
  if (msg_ptr->t != T_rrc_RadioBearerSetup_r3 )
  {
    if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
    {
      MSG_LOW("rbsetup later than r3 chosen",0,0,0);

      /* Check if Rel 5 contents are present */
      if ((msg_ptr->u.later_than_r3.criticalExtensions.t ==
           T_rrc_RadioBearerSetup_criticalExtensions_5_criticalExtensions) &&
          (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.t ==
           T_rrc_RadioBearerSetup_criticalExtensions_4_r5))
      {
        process_rbsetup_r5(cmd_ptr);
        return;
      }
      /* Check if Rel 6 contents are present */
      else if ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6) &&
               (msg_ptr->u.later_than_r3.criticalExtensions.t ==
                T_rrc_RadioBearerSetup_criticalExtensions_5_criticalExtensions) &&
               (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.t ==
                T_rrc_RadioBearerSetup_criticalExtensions_4_criticalExtensions) &&
               (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t ==
                T_rrc_RadioBearerSetup_criticalExtensions_3_r6))
      {
        process_rbsetup_r6(cmd_ptr);
        return;
      }
      /* Check if Rel 6 contents are present */
      else if ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7) &&
        (RRCRB_R7_CRITICAL_EXT_PRESENT_PTR(
                  msg_ptr, 
                  rrc_RadioBearerSetup)))
      {
        process_rbsetup_r7(cmd_ptr);
        return;
      }
      /* Check if Rel 8 contents are present */
      else if ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8) &&
        (RRCRB_R8_CRITICAL_EXT_PRESENT_PTR(
                  msg_ptr, 
                  rrc_RadioBearerSetup)))
      {
        process_rbsetup_r8(cmd_ptr);
        return;
      }
      
#ifdef FEATURE_WCDMA_REL9
      /* Check if Rel 9 contents are present */
      else if ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9) &&
        (RRCRB_R9_CRITICAL_EXT_PRESENT_PTR(
                  msg_ptr, 
                  rrc_RadioBearerSetup)))
      {
        process_rbsetup_r9(cmd_ptr);
        return;
      }


#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
      #error code not present
#endif /* FEATURE_WCDMA_REL10 */


      else
      {
        rrc_print_supported_asn1_rel_version();
        WRRC_MSG0_ERROR("Configuration not supported.  Send RB setup failure");
        rrcrbe_send_rbsetup_failure_message_ext_not_comprehended(msg_ptr);

        /*do not change the current state. Some other RBsetup procedure might be
        on going. Maintain the current state and do not clear the procedure */
        return;
      }
    }
    else
    {
      WRRC_MSG1_HIGH("Unsupported RB Setup t:%d", msg_ptr->t);

      rrc_print_supported_asn1_rel_version();

      rrcrbe_send_rbsetup_failure_message_ext_not_comprehended(msg_ptr);

      /*do not change the current state. Some other RBsetup procedure might be
      on going. Maintain the current state and do not clear the procedure */
      return;
    }
  } /* if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6)  */

  if (TRUE == rrcrbe_validate_srns_relocation_info(msg_ptr, MSG_REL99))
  {
    ip_check_status = rrcsmc_check_integrity_for_srns_relocation(cmd_ptr, MSG_REL99);

    switch (ip_check_status)
    {
      case RRCSMC_SRNS_INTEGRITY_CHECK_SUCCESS:
        /*continue further processing with the message */
        break;

      case RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE:
        /*Discard the message and continue with ongoing procedure as if this
        message was not received */
        WRRC_MSG0_ERROR("IP check failed. Discarding RBS msg");
        break;

      case RRCSMC_SRNS_SECURITY_INFO_INVALID:
      case RRCSMC_INCOMPATIBLE_SECURITY_RECONFIGURATION:
        if (msg_ptr->u.r3.radioBearerSetup_r3.rrc_TransactionIdentifier
                       == rbe.tr_ids.rejected_transaction)
        {
          WRRC_MSG1_HIGH("Duplicate RBE msg - rejected tr-id%d match!",
                   rbe.tr_ids.rejected_transaction);
          /* Nothing else to do, just ignore the message and return */
        }
        else
        {
          /* send a reconfig failure messge with invalid
          configuration and
          continue the same substate with any ongoing procedure */
          rbe.tr_ids.rejected_transaction =
            (msg_ptr->u.r3.radioBearerSetup_r3.rrc_TransactionIdentifier);
          if (ip_check_status == RRCSMC_SRNS_SECURITY_INFO_INVALID)
          {
            rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          }
          else
          {
            rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
          }
          /*Check whether Cell Update is going on */
          if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
               (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
          {
            if(rbe.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
            {
              send_rbs_failure_message( rbe.status.failure_status,
                                       rbe.status.prot_err_cause );
              rbe.status.failure_status = RRCRB_MSG_VALID;
            }
            else
            {
              WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
              (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBE);
              /* Wait until Cell update procedure is completed */
              rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
            }
          }
          else
          {
            send_rbs_failure_message( rbe.status.failure_status,
                                            rbe.status.prot_err_cause );
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
    if (ip_check_status != RRCSMC_SRNS_INTEGRITY_CHECK_SUCCESS)
    {
      /*do not clear the procedure or change the rbe substate because
      some other rbe procedure might be in progress. Just return from
      here */
      return;
    }

  }

  if ((rrcllc_get_ordered_config_status() != OC_NOT_SET) ||
      (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
      (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
  {
    WRRC_MSG0_HIGH("Incomatible simultaneous reconfig! Rejecting RBS message");
    if ( (msg_ptr->u.r3.radioBearerSetup_r3.rrc_TransactionIdentifier)
           == rbe.tr_ids.accepted_transaction )
    {
      WRRC_MSG1_HIGH("Duplicate RBS msg! accepted tr-id%d match!",
               rbe.tr_ids.accepted_transaction);
      /* Nothing else to do, just return */
    }
    else if ( (msg_ptr->u.r3.radioBearerSetup_r3.rrc_TransactionIdentifier)
                   == rbe.tr_ids.rejected_transaction )
    {
      WRRC_MSG1_HIGH("Duplicate RBS msg! rejected tr-id%d match!", rbe.tr_ids.rejected_transaction);
      /* Nothing else to do, just return */
    }
    else
    {
      /* We have received a RBS message with a new transaction id.
      This message should be in our rejected transaction
      list */
      rbe.tr_ids.rejected_transaction =
        (msg_ptr->u.r3.radioBearerSetup_r3.rrc_TransactionIdentifier);
      rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;

      /*Check whether Cell Update is going on */
      if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
           (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
      {
        if(rbe.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
        {
          send_rbs_failure_message( rbe.status.failure_status,
                                     rbe.status.prot_err_cause );
          rbe.status.failure_status = RRCRB_MSG_VALID;
        }
        else
        {
          WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
          (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBE);
          /* Wait until Cell update procedure is completed */
          rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        }
      }
      else
      {
        send_rbs_failure_message( rbe.status.failure_status,
                                        rbe.status.prot_err_cause );
        /*continue the same substate */
      }
    }
    return;
  }

  /* We have received a RB setup message with a new transaction id.
     Accept the transaction Id */
  rbe.tr_ids.accepted_transaction =
    msg_ptr->u.r3.radioBearerSetup_r3.rrc_TransactionIdentifier;

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  validate_rbsetup_message( msg_ptr );

  if ( rbe.status.failure_status != RRCRB_MSG_VALID )
  {
    /* RB Message was invalid. */
    WRRC_MSG0_HIGH("RB Message validation failed");

    /* Send the RB Setup Failure Message */
    send_rbs_failure_message( rbe.status.failure_status,
                              rbe.status.prot_err_cause
                            );

    /* Make sure we're back in the initial sub-state */
    rbe.curr_substate = RRCRB_INITIAL;
    rrcrbe_clear_procedure();
  }
  /* If the message is valid, proceed further */
  else
  {
    rrcrbe_indicate_layer1_if_rb_setup_configure_voice_rab();

    /* If new rabs are setup, update mac rab status */
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
         rrc_RadioBearerSetup_r3_IEs,rab_InformationSetupList))
    {
      rrc_RAB_InformationSetupList *rab_list_ptr = &(msg_ptr->u.r3.radioBearerSetup_r3.rab_InformationSetupList);
      rrcrb_update_mac_rab_status(RRC_PROCEDURE_RBE,
                                 (rrc_cn_domain_identity_e_type)(rab_list_ptr->elem[0].rab_Info.cn_DomainIdentity) ,
                                  NULL,FALSE);
    }

    /*calculate ciphering_activation_cfn only if ciphering is enabled*/
    if(rrcsmc_is_ciphering_enabled() == RRCSMC_SECURITY_STARTED)
    {
      if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
             rrc_RadioBearerSetup_r3_IEs,activationTime)
         )
      {
        /*256 is the CFN rollover length*/
        /*250 is used to activate ciphering after 250 10ms time interval,
          this should give sufficient time for n/w side and ue side to have
          configured ciphering on the rb's successfully*/
        ciphering_activation_cfn =
         (uint8) ((msg_ptr->u.r3.radioBearerSetup_r3.activationTime + 250) % 256);
        /*round it to 8 frame boundary */
        ciphering_activation_cfn = (uint8)((ciphering_activation_cfn/8)*8);

      }
      else
      {
        /*256 is the CFN rollover length*/
        /*250 is used to activate ciphering after 250 10ms time interval,
          this should give sufficient time for n/w side and ue side to have
          configured ciphering on the rb's successfully*/
        ciphering_activation_cfn = (uint8)((seq_get_cfn() + 250) % 256);
        /*round it to 8 frame boundary */
        ciphering_activation_cfn = (uint8)((ciphering_activation_cfn/8)*8);
      }
    }

    /*call rrcsmc function to update ciphering activation cfn*/
    rrcsmc_update_ciphering_activation_cfn(ciphering_activation_cfn);

    /* Check if there is CN information present. If so, we need
    to send the information to the Cell Change manager */
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
          rrc_RadioBearerSetup_r3_IEs,cn_InformationInfo))
    {
      /*Check to see if primary PLMN I.D is present in RBE message ext
       * radioBearerSetup-v690ext which is present in 
       * v690NonCriticalExtensions
       */
       primary_plmn_Id_present = rrcrbe_check_and_get_primary_plmn_id(msg_ptr,
                                                                  &primary_plmn_Identity,MSG_REL99);
      rbe.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info(
                     &(msg_ptr->u.r3.radioBearerSetup_r3.cn_InformationInfo)
                     ,primary_plmn_Id_present
                     ,&primary_plmn_Identity
            );
      if (rbe.mm_cmd_ptr == NULL)
      {
        WRRC_MSG0_ERROR("Could not build SERVICE_IND cmd");
      }
      else
      {
        /* set cn_info_saved flag */
        rbe.cn_info_saved = TRUE;
      }

    }




    /* Check if there are new RABs to be established */
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
           rrc_RadioBearerSetup_r3_IEs,rab_InformationSetupList)
       )
    {
      //NOTE: Only one RAB is expected per message. This may
      //Change later.

      /* Check if the RAB id is already in ESTABLISHED_RABS */
      for (rab_index=0; rab_index < rbe.rabs.num_rabs_in_msg; rab_index++)
      {
        rab_status = rrc_find_rab_in_established_rabs
                     ( rbe.rabs.rabs_in_msg[rab_index].rab_id );
        rbe.rabs.chan_config_needed[rab_index] = TRUE;
        if (rab_status != RRC_RAB_NOT_FOUND)
        {
          WRRC_MSG1_HIGH("RAB %d already exists in EST_RABS!",
                   rbe.rabs.rabs_in_msg[rab_index].rab_id);
          break;
        }
      }
#ifdef FEATURE_SCUDIF_ENHANCEMENT
      #error code not present
#endif

      if ( (rab_status == RRC_RAB_NOT_FOUND) ||
           (rrc_find_if_voice_rab_exists_in_established_rabs( rbe.rabs.rabs_in_msg[rab_index].rab_id )
               == TRUE))
      {
        /* We have to establish this RAB. First set ORDERED_CONFIG
        so that LLC can process the necessary parameters.
        This function will copy all RABs from the rxd msg to oc. */
        oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_RBE,
                                               rbe.state_from_msg,
                                               rrc_DL_DCCH_Message_PDU,
                                               (void *)dcch_msg_ptr
                                             );

        switch ( oc_status )
        {
          case ORDERED_CONFIG_SET:
            if((rrc_sib_is_current_event_sib_read_in_dch()) &&
               (RRC_PROCEDURE_GPS == rrc_sib_get_proc_id_for_sib_read_in_dch()))
            {
               WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
               rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
            }

            /* Ordered Config has been set. We can setup the
            Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */
            WRRC_MSG0_HIGH("Sending RRC_CHANNEL_CONFIG_REQ to LLC");
            /* Call the function that creates and sends a channel
            config request to LLC. */
            rrcrbe_send_channel_config_req( msg_ptr );
  
            if(((rrc_get_state()) == RRC_STATE_CELL_DCH) &&
               (rrc_cmd.cmd.chan_config_req.next_state== RRC_STATE_CELL_FACH) &&
               (rrc_cmd.cmd.chan_config_req.rrc_state_change_required))
            {
              if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
                 rrc_RadioBearerSetup_r3_IEs,new_C_RNTI))
              {
                rbe.new_crnti_valid = TRUE;
                rrc_translate_crnti(&msg_ptr->u.r3.radioBearerSetup_r3.new_C_RNTI,
                                    &c_rnti);
                rbe.new_crnti = (uint16)c_rnti;
              }
              else
              {
                rbe.new_crnti_valid = FALSE;
              }
              rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
            }
            else
            {
              if( rbe.curr_substate != RRCRB_WAIT_FOR_CELL_SELECTION_CNF )
              {
                /* Change the substate since we're waiting for the
                confirmation from LLC for the channel configs */
                rbe.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
              }
            }
            break;
  
          case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:
  
            rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
            /* Send the RB Setup Failure Message */
            send_rbs_failure_message( rbe.status.failure_status,
                                      rbe.status.prot_err_cause
                                    );
  
            /* Go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
  
          case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:
  
            rbe.status.failure_status =RRCRB_MSG_UNSUPPORTED_CONFIG;
            /* Send the RB Setup Failure Message */
            send_rbs_failure_message( rbe.status.failure_status,
                                      rbe.status.prot_err_cause
                                    );
  
            /* Go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
  
	   case ORDERED_CONFIG_CONFIGURATION_INVALID:
  
            rbe.status.failure_status =RRCRB_MSG_INVALID_CONFIG;
            /* Send the RB Setup Failure Message */
            send_rbs_failure_message( rbe.status.failure_status,
                                      rbe.status.prot_err_cause
                                    );
  
            /* Go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
			
          case ORDERED_CONFIG_NOT_SET_OTHER:
            WRRC_MSG0_HIGH("LLC returned other failure!");
            /* When LLC sends this, there is a serious problem
            and we'll go back to disconnected state. No need
            to send the failure message here, we can just
            go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
  
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME 
          case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
            WRRC_MSG0_HIGH("RB setup processing deferred due to LTA!");
            rbe.curr_substate = RRCRB_INITIAL;
          
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
          default:
  
            WRRC_MSG0_ERROR("Unknown return value from LLC");
            /* Unknown error, just go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
        }/* end switch */

        if ( ordered_config_err_occured == TRUE )
        {
          /* No need to continue. Clear the procedure
          variables and return. */
          rrcrbe_clear_procedure();
          return;
        }

        if(rbe.rbe_for_srns_relocation == TRUE)
        {
          rrcsmc_commit_fresh_for_srns_relocation();
        }
      }
      else
      {
        /* send a failure message if the rab in the message  already
        exists in established_rabs. Later add a check for the rbs
        associated with the rabs so that the failure will be sent only when
        any of the rb-ids match with the rb-ids in established_rab for that
        rab */
        rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
        /* Send the RB Setup Failure Message */
        send_rbs_failure_message( rbe.status.failure_status,
                                  rbe.status.prot_err_cause
                                );

        /* Make sure we're back in the initial sub-state */
        rbe.curr_substate = RRCRB_INITIAL;
        rrcrbe_clear_procedure();
        return;
      }
    }

    /* If RABs need to be setup, the RBs to be setup for
    that RAB and any other RBs to be re-configured as indicated
    by the message will be sent in the same channel config
    command. But if no new RBs have been setup, then we
    need to send a separate command for any RBs that
    need to be re-configured */
    if ( rbe.rabs.chan_config_needed[0] == FALSE )
    {
      /* RABs are not being setup for this message.
      Even though we don't have to setup RABs, there may
      be signalling RBs to be reconfigured in the message. Check
      if there are any */

      if (
          ( msg_ptr->u.r3.radioBearerSetup_r3.rb_InformationAffectedList.n != 0 )||
          ( msg_ptr->u.r3.radioBearerSetup_r3.srb_InformationSetupList.n != 0 )
     )
      {
        /* We have to reconfigure these RBs. First set ORDERED_CONFIG
        so that LLC can process the necessary parameters. */
        oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_RBE,
                                               rbe.state_from_msg,
                                               rrc_DL_DCCH_Message_PDU,
                                               (void *)dcch_msg_ptr
                                             );

        switch ( oc_status )
        {
          case ORDERED_CONFIG_SET:
            if((rrc_sib_is_current_event_sib_read_in_dch()) &&
               (RRC_PROCEDURE_GPS == rrc_sib_get_proc_id_for_sib_read_in_dch()))
            {
               WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
               rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
            }

            /* Ordered Config has been set. We can setup the
            Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */
            MSG_LOW("Sending RRC_CHANNEL_CONFIG_REQ to LLC", 0, 0, 0);
            /* Call the function that creates and sends a channel
            config request to LLC. */
            rrcrbe_send_channel_config_req( msg_ptr );
  
            if(((rrc_get_state()) == RRC_STATE_CELL_DCH) &&
                (rrc_cmd.cmd.chan_config_req.next_state== RRC_STATE_CELL_FACH) &&
                (rrc_cmd.cmd.chan_config_req.rrc_state_change_required))
            {
              if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerSetup_r3,
                 rrc_RadioBearerSetup_r3_IEs,new_C_RNTI))
              {
                rbe.new_crnti_valid = TRUE;
                rrc_translate_crnti(&msg_ptr->u.r3.radioBearerSetup_r3.new_C_RNTI,
                                    &c_rnti);
                rbe.new_crnti = (uint16)c_rnti;
              }
              else
              {
                rbe.new_crnti_valid = FALSE;
              }
              rbe.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
            }
            else
            {
              /* Change the substate since we're waiting for the
              confirmation from LLC for the channel configs */
              rbe.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
            }
  
            break;
  
          case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:
  
            rbe.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
            /* Send the RB Setup Failure Message */
            send_rbs_failure_message( rbe.status.failure_status,
                                      rbe.status.prot_err_cause
                                    );
  
            /* Go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
  
          case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:
  
            rbe.status.failure_status =RRCRB_MSG_UNSUPPORTED_CONFIG;
            /* Send the RB Setup Failure Message */
            send_rbs_failure_message( rbe.status.failure_status,
                                      rbe.status.prot_err_cause
                                    );
  
            /* Go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;

	   case ORDERED_CONFIG_CONFIGURATION_INVALID:
  
            rbe.status.failure_status =RRCRB_MSG_INVALID_CONFIG;
            /* Send the RB Setup Failure Message */
            send_rbs_failure_message( rbe.status.failure_status,
                                      rbe.status.prot_err_cause
                                    );
  
            /* Go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
  
          case ORDERED_CONFIG_NOT_SET_OTHER:
            WRRC_MSG0_HIGH("LLC returned other failure!");
            /* When LLC sends this, there is a serious problem
            and we'll go back to disconnected state. No need
            to send the failure message here, we can just
            go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
  
 #ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME                     
          case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
            WRRC_MSG0_HIGH("RB setup processing deferred due to LTA!");
            rbe.curr_substate = RRCRB_INITIAL;
          
          /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
  #endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
          default:
  
            WRRC_MSG0_ERROR("Unknown return value from LLC");
            /* Unknown error, just go back to initial substate */
            rbe.curr_substate = RRCRB_INITIAL;
  
            /* Indicate that an error occured */
            ordered_config_err_occured = TRUE;
            break;
          }/* end switch */

        if ( ordered_config_err_occured == TRUE )
        {
          /* No need to continue. Clear the procedure
          variables and return. */
          rrcrbe_clear_procedure();
          return;
        }
      }
      else
      {
        /*If we are here, it means that RABs are not given in RB-Setup.
        Also, SRBs and RB Affected IEs are not given.*/

        WRRC_MSG0_ERROR("No RB IEs are given in the OTA.");
        rbe.status.prot_err_cause = rrc_ProtocolErrorCause_informationElementMissing;
        rbe.status.failure_status=RRCRB_MSG_INVALID_CONFIG;

        /* Send the RB Setup Failure Message */
        send_rbs_failure_message( rbe.status.failure_status, rbe.status.prot_err_cause);
        /* Make sure we're back in the initial sub-state */
        rbe.curr_substate = RRCRB_INITIAL;
        rrcrbe_clear_procedure();

        return;          
      }/* end if - RBs need to be reconfigured */
    }/* end if - rbe.rabs.chan_config_needed[rab_index] == FALSE */
  }/* end if - valid rb message */
}/* end function process_rbsetup_message */



/*===========================================================================

FUNCTION   RRCRBE_PROCESS_RNTI_UPDATE_CNF

DESCRIPTION

  This function processes RNTI_UPDATE_CNF

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbe_process_rnti_update_cnf
(
void
)
{
  rrc_state_e_type curr_state;

  curr_state = rrc_get_state();

  if ((curr_state == RRC_STATE_CELL_FACH) &&
      ((rbe.state_from_msg == RRC_STATE_CELL_PCH) ||
       (rbe.state_from_msg == RRC_STATE_URA_PCH)))
  {
   /*send the response message now */
    send_rbs_complete_message(TRUE);
    /*wait for L2ack */
    rbe.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
    /* prints rate for each transport channel */
    rrcllc_print_trans_chnl_rate();
  }
  else
  {
    /* We should not get here*/
    WRRC_MSG1_HIGH("Unexpected RNTI Update cnf,RBE state %d",
                                   rbe.curr_substate);
  }
}

/*===========================================================================

FUNCTION   RRCRBE_PROCESS_CHANNEL_CONFIG_CNF

DESCRIPTION

  This function processes the received Channel Config Confirmation
  from LLC. If the required Radio-bearers have been setup succesfully,
  and if any of the radio-bearers map to radio-access-bearers, this
  information needs to be given to the NAS entity for that RAB.

  If a voice RAB has been assigned by the UTRAN, this function
  gets the RLC ids and the AMR mode information from LLC and sends this
  information to voice services. But for CS and PS data calls,
  the RLC ids are not sent by this function. The data services
  entity calls the required function in rrcdata module to get
  the RLC ids whenever needed.

  This function sends the RRC_SYNC_IND primitive to MM for Circuit-Switched
  calls. This function sends the RABMAS_RAB_ESTABLISH_IND primitive to
  RABM for Packet-Switched calls.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbe_process_channel_config_cnf
(
rrc_channel_config_cnf_type *ptr  /* Pointer to the Channel config cnf cmd */
)
{

  uint32 rab_index=0;    /* Used if there is more than 1 RAB in the
                         message */
  rrcllc_amr_mapping_info_type amr_info={0}; /* To store AMR mode
                         info incase a voice RAB has been setup */
  rrc_state_e_type curr_state;
  rrcrb_rnti_update_info_type rnti_update_info;   /* to hold rnti update info */
  rrc_CellUpdateCause       cu_cause; /*To store CU cause from CU procedure*/

  rrc_rlc_re_est_for_srns_rb_e_type rb_type; /*rb type tobe reestablished */
  boolean amr_rate_configured = FALSE;
  
#ifdef FEATURE_VOC_AMR_WB
  mvs_mode_type current_mode = rrc_get_current_codec_mode();
#endif /*FEATURE_VOC_AMR_WB*/

#ifdef FEATURE_SCUDIF_ENHANCEMENT
  #error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/

  rrcllcpcie_initialise_sync_a_post_veri_info();

  /* Check if the channel configuration was succesful. */
  if ( ptr->chan_cfg_status != RRCLLC_CHAN_CFG_SUCCESS )
  {
    WRRC_MSG0_ERROR("CHAN_CONFIG_CNF with Failure");
#ifndef FEATURE_WCDMA_HS_FACH
    /* Channel config failure, so initialise the ordered RB-Mapping database*/
    rrcllc_init_ordered_hanging_rb_mapping_info();
#else
    rrcllc_copy_current_hanging_rb_mapping_to_ordered();
#endif


    /*Here we initialize rlc_size_change_in_progress DB so that if it was set by RBE, 
    it is ready for next procedure*/
    rrc_initialize_rlc_size_change();

#ifdef FEATURE_VOC_AMR_WB
    rrc_set_ordered_codec_mode(current_mode);
#endif/*FEATURE_VOC_AMR_WB*/
    /* Roll back mac_rab_status */
    rrcrb_update_mac_rab_status(RRC_PROCEDURE_RBE, RRC_CS_DOMAIN_CN_ID, NULL,TRUE);

    rbe.status.failure_status = RRCRB_PHY_CHAN_FAILURE;
    /* Channel config got Cancelled because of RL_FAILURE or RLC_UNRECOVERABLE
     * error.  Wait for CU procedure to complete before sending failure msg to NW
     */
    if (ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_CFG_CANCELLED)
    {
      /* cell update is already in progress as this is for pending config
       * cancellation
       */
      WRRC_MSG0_HIGH("Channel Config cancel waiting for CU complete");
      rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      return;
    }
    /* validation failure happened.  Send RB failure msg with invalid config */
    if (ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_OTHER)
    {

      rbe.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      /* Check if CU is waiting to handle RLC unrecoverable error.
       * If so, wait for CU to complete before sending failure to NW.
       */
      if(rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG)
      {
        rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBE, RRC_UNRECOVERABLE_ERROR, FALSE);
        return;
      }

      WRRC_MSG0_HIGH("Sending RBE failure msg with invalid config");
      send_rbs_failure_message( rbe.status.failure_status,
                                rbe.status.prot_err_cause
                                );

      /* Reset ORDERED_CONFIG - UE can now accept other ordered
      configurations */
       (void)rrcllc_reset_ordered_config(RRC_PROCEDURE_RBE,TRUE);

        /* Then clear all local variables that may
        need to be cleared */
        rrcrbe_clear_procedure();
        /* Make sure we're back in the initial sub-state */
        rbe.curr_substate = RRCRB_INITIAL;
      
      /*During FACH->DCH transition, if T305 got expired, we need to start T305 with zero timeout.*/	  
      if(rrc_get_state() == RRC_STATE_CELL_FACH && rbe.state_from_msg == RRC_STATE_CELL_DCH)
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




    //anytime chan-config for rbe fails,  we go to fach, hence call the API which SMC provides
    //to release TM bearers for CS domain.  This needs to be done as going to fach does not
    //go through regular channel-config and hence smc needs to know of this release of tm bearers
    //directly through rb procedure
    rrcsmc_update_ciphering_config_info_for_signalling_release(RRC_CS_DOMAIN_CN_ID);

    /* Check if PHY_CHAN_FAILURE happened in FACH->DCH Transition.  If so, initiate a cell update
    request, and rbe substate should be RRCRB_WAIT_FOR_CELL_UPDATE_CNF */
    if (rrc_get_state() == RRC_STATE_CELL_FACH && rbe.state_from_msg == RRC_STATE_CELL_DCH)
    {
      WRRC_MSG0_HIGH("PHYCHANFAIL FACH->DCH.  Initiate CU");
      /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
      rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBE, RRC_PHY_CHAN_FAILURE,TRUE);
      /* Wait for CELL UPDATE to get a valid C_RNTI */
      rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      rrcrbe_send_initiate_cu_flag = FALSE;
      rrcrbe_delete_crnti          = FALSE;
    }
    /* Check if PHY_CHAN_FAILURE happended within CELL_DCH.  For now, initatiate a cell update request
       with cause RL Failure.  RBE substate should be RRCRB_WAIT_FOR_CELL_UPDATE_CNF*/
    else if (rrc_get_state() == RRC_STATE_CELL_DCH && rbe.state_from_msg == RRC_STATE_CELL_DCH)
    {
      /* if cu is waiting to handle RL/RLC unrecoverable failure, wait for
       * Cell update to complete before sending failure.
      */
      if(rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG)
      {
        rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBE, RRC_RADIO_LINK_FAILURE, FALSE);
        return;
      }

      if (ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_PHY_CHL)
      {
        WRRC_MSG0_HIGH("RBsetup failed, going back to old cfg success");
        send_rbs_failure_message( rbe.status.failure_status,
                                  rbe.status.prot_err_cause
                                  );
        /* Go back to initial substate */
        rbe.curr_substate = RRCRB_INITIAL;
        /* Then clear all local variables that may
        need to be cleared */
        rrcrbe_clear_procedure();
        rrcllc_clear_ordered_config();
      }
      else if (ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_BACK_TO_OLD_CONFIG)
      {
        WRRC_MSG0_HIGH("RBsetup failed. Old config failed.  Initiate CU");
        /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBE, RRC_RADIO_LINK_FAILURE,TRUE);
        /* Wait for CELL UPDATE to get a valid C_RNTI */
        rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        rrcrbe_send_initiate_cu_flag = FALSE;
        rrcrbe_delete_crnti          = FALSE;
      }
    }
    else
    {
      send_rbs_failure_message( rbe.status.failure_status,
                                rbe.status.prot_err_cause
                                );
      /* Go back to initial substate */
      rbe.curr_substate = RRCRB_INITIAL;
      /* Then clear all local variables that may
      need to be cleared */
      rrcrbe_clear_procedure();
    }
    return;
  }
 
#ifdef FEATURE_WCDMA_REL7_SPECRS    
  rrc_save_rnc_cap_change_support();
#endif /*FEATURE_WCDMA_REL7_SPECRS*/
  
  /*For FACH->DCH, if CHAN_CONFIG_REQ is succeeded, we need to stop T305*/  
  if(rrc_get_state() == RRC_STATE_CELL_FACH && rbe.state_from_msg == RRC_STATE_CELL_DCH)
  {
    rrctmr_stop_timer(RRCTMR_T_305_TIMER);
  }  

   /*Notify the Cell_Id to registered entities*/
  if(rbe.state_from_msg == RRC_STATE_CELL_DCH)
  {
    rrc_notify_cell_id_in_dch();
  }	

  /* Copy the Ordered RB Mapping into the current RB-Mapping database*/
  rrcllc_copy_ordered_hanging_rb_mapping_to_current();

  rbe.hho_ciph_status = ptr->inter_freq_hho_status;
  WRRC_MSG1_HIGH("HHO Ciph Status set to %d from chan cfg cnf", rbe.hho_ciph_status);
  /* Channel config was succesful. Check if any
  RABs were setup, or only Signalling RBs were setup */
  for (rab_index=0; rab_index < rbe.rabs.num_rabs_in_msg; rab_index++)
  {
#ifdef FEATURE_VOC_AMR_WB  
    /*This flag is used to notify whether the RAB has been updated in rrc_est_rabs.*/
    boolean existing_rab_updated = FALSE;
#endif /*FEATURE_VOC_AMR_WB*/

    if ( rbe.rabs.chan_config_needed[rab_index] == TRUE )
    {
      /* Some RABs were setup. Set the default value again
      till we get the next message */
      rbe.rabs.chan_config_needed[rab_index] = FALSE;

   /* Now we need to update the UE Variable ESTABLISHED_RABS.
      Note that we have only one RAB per message now. When
      more RABs are established, we need to do this for each
      RAB.Check if the RAB id is already in ESTABLISHED_RABS.
      It shouldn't be, but we have other RRC procedures running
      at the same time, so make a safety check here. */
      if ( (rrc_find_rab_in_established_rabs( rbe.rabs.rabs_in_msg[rab_index].rab_id ))
           == RRC_RAB_NOT_FOUND )
      {
        /* We can add this RAB. */
        WRRC_MSG3_MED("Add RAB id %d, RAB type: %d(0:Voice, 2:PS) in EST_RAB, rab_index: %d",
                     rbe.rabs.rabs_in_msg[rab_index].rab_id,
                     rbe.rabs.rabs_in_msg[rab_index].rab_type, rab_index);
        rrc_add_rab_to_established_rabs( &(rbe.rabs.rabs_in_msg[rab_index]));
      }
      else
      {
        /*We are here. It implies that the RAB was already present in rrc_est_rabs.*/
        WRRC_MSG0_HIGH("RAB already in established RABs");
        if((rrc_find_if_voice_rab_exists_in_established_rabs(rbe.rabs.rabs_in_msg[rab_index].rab_id) == TRUE))

        {
#ifdef FEATURE_VOC_AMR_WB        
          existing_rab_updated = TRUE;
#endif /*FEATURE_VOC_AMR_WB*/
          rrc_update_rb_info_to_existing_rab(&rbe.rabs.rabs_in_msg[rab_index]);
          WRRC_MSG1_HIGH("updated RB info in established RAB %d", rbe.rabs.rabs_in_msg[rab_index].rab_id );
        }    
        /*For VT -> Voice SCUDIF, we will come here.*/
#ifdef FEATURE_SCUDIF_ENHANCEMENT
#error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/
        else
        {
          WRRC_MSG1_HIGH("RAB Type %d in RBE DB", rbe.rabs.rabs_in_msg[rab_index].rab_type);
        }
      }

#ifdef FEATURE_SCUDIF_ENHANCEMENT
      #error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/
      {
        /*By this time, RBs and RABs have been updated in rrc_est_rabs.*/
        /* Now check what kind of RAB was setup */
        WRRC_MSG1_MED("Channel config succeeded for voice RAB %d[0:RRCRB_CS_VOICE_RAB|1:RRCRB_CS_DATA_RAB|2:RRCRB_PS_DATA_RAB|3:RRCRB_UNKNOWN_RAB]", rbe.rabs.rabs_in_msg[rab_index].rab_type);		
        if ( rbe.rabs.rabs_in_msg[rab_index].rab_type == RRCRB_CS_VOICE_RAB )
        {
          uint8 rab_idx = 0;
          if((rab_idx = rrc_get_idx_in_rrc_est_rabs_for_rab_id(rbe.rabs.rabs_in_msg[rab_index].rab_id)) 
                < MAX_RAB_TO_SETUP)
#ifdef FEATURE_VOC_AMR_WB
          {
            if((existing_rab_updated) && (rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator!=current_mode))
            {
              rrc_set_current_codec_mode(rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator);
              current_mode = rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator;
              rrc_change_codec_configuration(rab_idx);
            }
            else
            {
              rrc_set_current_codec_mode(rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator);
              current_mode = rbe.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator;
              if( current_mode == MVS_MODE_AMR_WB)
              {
                amr_info.mode_type = MVS_MODE_AMR_WB;
                (void) rrcllc_get_amr_wb_mapping(&(amr_info));
                rrc_configure_amr_wb_mode_param(&(amr_info));
              }
              else
              {
                amr_info.mode_type = MVS_MODE_AMR;
                /*Now MVS needs to be configured. The structure amr_info will be filled by this function. */
                (void) rrcllc_get_amr_mapping(&(amr_info));
                /*Configure MVS to register RLC queues for AMR Call.*/
                rrc_configure_mvs(&amr_info, rab_idx);
              }
            }
          }

#else /*FEATURE_VOC_AMR_WB*/
          {
            /*Now MVS needs to be configured. The structure amr_info will be filled by this function. */
            (void) rrcllc_get_amr_mapping(&(amr_info));
            /*Configure MVS to register RLC queues for AMR Call.*/
            rrc_configure_mvs(&amr_info, rab_idx);
          }

#endif /*FEATURE_VOC_AMR_WB*/
  
          amr_rate_configured = TRUE;
          WRRC_MSG0_HIGH("Updating MVS for voice RAB");
        }
      }
    }/* end if - rbe.rabs.chan_config_needed[rab_index] == TRUE */
  } /* for() */


#ifdef FEATURE_SCUDIF_ENHANCEMENT
  #error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/

  /* Make sure MVS gets updated with the TB sizes if reconfigured 
  while adding CS DATA RAB/PS RAB/SRBs */
  if(amr_rate_configured == FALSE)
  {
#ifdef FEATURE_VOC_AMR_WB
    rrcrb_update_amr_cctrch_id_for_reconfig(current_mode);
#else/*FEATURE_VOC_AMR_WB */
    rrcrb_update_amr_cctrch_id_for_reconfig();
#endif/*FEATURE_VOC_AMR_WB */
  }
  /* Check for CELL FACH before sending the rbs complete message */
  curr_state = rrc_get_state();
  if ((RRC_STATE_CELL_FACH == curr_state) &&
      ((rbe.state_from_msg == RRC_STATE_CELL_FACH) ||
       (rbe.state_from_msg == RRC_STATE_CELL_PCH) ||
       (rbe.state_from_msg == RRC_STATE_URA_PCH)))
  {
    if(rrcrbe_send_initiate_cu_flag 
       || (rrccu_return_cu_substate() == RRCCU_WAIT_FOR_RB_PROC_COMPL_IND) 
       )
    {
      if (rrcrbe_delete_crnti)
      {
        /* Send a RNTI Update Request */
        rnti_update_info.crnti_action         = RRC_RNTI_DELETE;
        rnti_update_info.urnti_action         = RRC_RNTI_NOCHANGE;
        rnti_update_info.procedure            = RRC_PROCEDURE_RBE;
        rnti_update_info.rlc_re_establish_srb = FALSE;
        rnti_update_info.rlc_re_establish_rab = FALSE;
        rnti_update_info.cnf_reqd             = FALSE;

        rrcrb_send_rnti_update_req(rnti_update_info);
        rbe.new_crnti_valid = FALSE;
      }
      /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
      rrcrbe_send_cell_update_initiate_req(RRC_PROCEDURE_RBE);
      /* Wait for CELL UPDATE to get a valid C_RNTI */
      rbe.status.failure_status = RRCRB_MSG_VALID;
      rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      rrcrbe_send_initiate_cu_flag = FALSE;
      rrcrbe_delete_crnti          = FALSE;
      return;
    }
    /*Following case will occur when RLC RESET happens on SRB2 during DCH->FACH Transition*/
    else if((rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG) &&
      (rrccu_get_cell_update_started_status_with_cause(&cu_cause) == RRCCU_STARTED) && 
      (cu_cause == rrc_CellUpdateCause_rlc_unrecoverableError) && (rrccu_get_rb_id_unrec_error() == 
      DCCH_AM_RADIO_BEARER_ID))
    {
      if(rbe.new_crnti_valid)
      {
        /* Send a RNTI Update Request */
        rnti_update_info.crnti_action         = RRC_RNTI_UPDATE;
        rnti_update_info.urnti_action         = RRC_RNTI_NOCHANGE;
        rnti_update_info.crnti                = rbe.new_crnti;
        rnti_update_info.procedure            = RRC_PROCEDURE_RBE;
        rnti_update_info.rlc_re_establish_srb = FALSE;
        rnti_update_info.rlc_re_establish_rab = FALSE;
        rnti_update_info.cnf_reqd             = FALSE;
  
        rrcrb_send_rnti_update_req(rnti_update_info);
        /* Send a Radio Bearer Setup Complete later */      
      }

      /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
      rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBE,
                                          RRC_UNRECOVERABLE_ERROR,
                                          TRUE);

      /* Wait for CELL UPDATE to get a valid C_RNTI */
      rbe.status.failure_status = RRCRB_MSG_VALID;
      rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;

      return;                                          
    }    
    /* Coming here implies the "rrcrbe_send_initiate_cu_flag" is FALSE.
     * Check if Valid CRNTI is stored in RBE (for DCH->FACH case).
     * If so, send a RNTI Update Req to LLC and NOT wait for
     * Cell Update Complete Confirm. Cell Update will NOT be
     * initiated when there is a valid C-RNTI and send_initiate_cu_flag
     * is FALSE.
     */
    else if(rbe.new_crnti_valid)
    {
      /* Send a RNTI Update Request */
      rrcrbe_send_crnti_update_req(rbe.new_crnti);
      /* Send a Radio Bearer Setup Complete later */
    }

    /* Query for valid C-RNTI */
    else if (C_RNTI_NOT_VALID == rrcllc_get_current_crnti_status() )
    {
      /* Wait for CELL UPDATE to get a valid C_RNTI. Register for CU complete cnf. */
      WRRC_MSG0_HIGH("No Valid C_RNTI! Register for CU CNF");
      (void) rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBE);
      rbe.status.failure_status = RRCRB_MSG_VALID;
      rbe.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      return;
    }
  }


  /*check whether this reconfiguration involves SRNS relocation */
  if (rbe.rbe_for_srns_relocation == TRUE)
  {
    if (RRC_STATE_CELL_DCH == curr_state)
    {
      /* Re-establish SRB2 before sending the response message */
      rb_type  = RRC_RE_EST_RB_2;
      rrcrb_send_rlc_re_est_req_to_smc(RRC_PROCEDURE_RBE, rb_type, TRUE);

      rbe.curr_substate = RRCRB_WAIT_FOR_REESTABLISH_SRB2;
    }
    else
    {
      WRRC_MSG1_ERROR("SRNS not supp. in state %d", curr_state);
    }
  }
  else /*normal reconfiguration without SRNS relocation */
  {
    /* Send the Radio Bearer Reconfig Complete message */
    if ((rbe.state_from_msg == RRC_STATE_CELL_PCH) ||
        (rbe.state_from_msg == RRC_STATE_URA_PCH))
    {
      send_rbs_complete_message(TRUE);
#ifdef FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE
      #error code not present
#endif
    }
    else
    {
     /* Need to wait for L2 ack for all states because RRC needs to trigger include reconfig status indicator
     if RLC acknowledgement is not yet received.
     As per 8.3.1.3
     1> if the IE "Cell update cause" is set to "radio link failure" and the UE has submitted a reconfiguration response
     message to RLC and the UE has not received the RLC acknowledgement for the response message:
     2> include the IE "Reconfiguration Status Indicator" and set it to TRUE.*/
        rrc_update_reconfig_waiting_for_l2ack_dch(RRC_PROCEDURE_RBE);
        send_rbs_complete_message(TRUE);
        
      /* IF CU is waiting to handle RLF/RLC unrecoverable error, then send complete
       * and delay for 50ms so to make sure Complete goes out to NW on old config
       */
      if((rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG)
        && (rrc_get_state() == RRC_STATE_CELL_DCH))
      {
        WRRC_MSG1_HIGH("Delaying by %d ms", RRCRB_PND_CFG_DELAY);
        (void) rex_clr_sigs(rex_self(), RRCRBE_TEMP_TIMER_SIG);
        (void) rex_set_timer(&rrcrbe_temp_timer, RRCRB_PND_CFG_DELAY);
        rrcrbe_sig = rex_wait(RRCRBE_TEMP_TIMER_SIG);
        MSG_LOW("RRCRBE signal:%d", rrcrbe_sig,0, 0);
        (void) rex_clr_sigs(rex_self(), RRCRBE_TEMP_TIMER_SIG);
        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBE, RRC_UNRECOVERABLE_ERROR, FALSE);
      }
      /*Following case handles the scenario when RF Failure or RLC unrecoverable error occured
        during DCH -> FACH or FACH -> FACH transiton.*/
      else if((rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG) &&
      (rrccu_get_cell_update_started_status_with_cause(&cu_cause) == RRCCU_STARTED))
      {
        if((cu_cause == rrc_CellUpdateCause_rlc_unrecoverableError) && (rrccu_get_rb_id_unrec_error() != DCCH_AM_RADIO_BEARER_ID))
        {
          /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
          rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBE,
                                              RRC_UNRECOVERABLE_ERROR,
                                              TRUE);
        }
        else if(cu_cause == rrc_CellUpdateCause_radiolinkFailure)
        {
          /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
          rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBE,
                                              RRC_RADIO_LINK_FAILURE,
                                              TRUE);
        }
      }      
    }
    /* send rbs indication to upperlayers */
    rrcrbe_send_indication_to_upper_layers();

    if (rrcllc_get_ordered_config_status_wo_f3() != OC_SET_FOR_DCH_FACH_TRANS)
    {
      /* prints rate for each transport channel */
      rrcllc_print_trans_chnl_rate();
    }

    /*if next state is Cell_PCH or URA_PCH then we have to
    treat the procedure as successful only after getting L2ack
    for the response message */
    if ((rbe.state_from_msg == RRC_STATE_CELL_PCH) ||
        (rbe.state_from_msg == RRC_STATE_URA_PCH))
    {
      /* Change the substate to the next substate - this is
      where we wait for L2 ack from RLC for sending the RB RC
      complete message. */
      rbe.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
    }
    else
    {
      /* Clear ORDERED_CONFIG - UE can now accept other ordered
      configurations */
      rrcllc_clear_ordered_config();

      /* Go to the next sub-state in the state machine. That's
      the initial substate since we've sent the RBS complete
      message */
      rbe.curr_substate = RRCRB_INITIAL;

      /* At this point we don't need to remember any of the
      Radio Bearer information since it's stored in ESTABLISHED_RABS.
      Clear our internal variables */
      rrcrbe_clear_procedure();
    }
  }

}
/*===========================================================================

FUNCTION   RRCRBE_PROCESS_RLC_REESTABLISHMENT_CNF

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
static void rrcrbe_process_rlc_reestablishment_cnf
(
void
)
{
  rrcrb_rnti_update_info_type rnti_update_info; /* structure to be used to request RNTI update */

  /* send rnti update request if URNTI was present in the rebsetup message */
  if (rbe.new_urnti_valid == TRUE)
  {
    /* Send the RNTI update request to rrcllc to update with new-URNTI */
    /* Send a RNTI Update Request */
    rnti_update_info.crnti_action         = RRC_RNTI_NOCHANGE;
    rnti_update_info.urnti_action         = RRC_RNTI_UPDATE;
    rnti_update_info.urnti                = rbe.new_urnti;
    rnti_update_info.procedure            = RRC_PROCEDURE_RBE;
    rnti_update_info.rlc_re_establish_srb = FALSE;
    rnti_update_info.rlc_re_establish_rab = FALSE;
    rnti_update_info.cnf_reqd             = FALSE;

    rrcrb_send_rnti_update_req(rnti_update_info);
  }

  /* Send the reconfiguration complete message */

  send_rbs_complete_message(TRUE);

  /* Change the substate to the next substate - this is
     where we wait for L2 ack from RLC for sending the RB Reconfig
     complete message. */
  rbe.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
  /* prints rate for each transport channel */
  rrcllc_print_trans_chnl_rate();

}




/*===========================================================================

FUNCTION   RRCRBE_PROCESS_STATE_CHANGE_IND

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
static void rrcrbe_process_state_change_ind
(
rrc_state_change_ind_type *ptr /* Pointer to the State Change Ind cmd */
)
{
  rrc_cmd_type  *out_cmd_ptr;
  rrc_CellUpdateCause cu_cause;
  /* Check if we're going to disconnected state */
  if ( ptr->new_state == RRC_STATE_DISCONNECTED )
  {
    /* Then change to initial substate */
    rbe.curr_substate = RRCRB_INITIAL;

    /* Then clear all local variables that may
    need to be cleared */
    rrcrbe_clear_procedure();
  }

  else if (ptr->new_state == RRC_STATE_CELL_FACH)
  {

    if(ptr->previous_state == RRC_STATE_CELL_DCH)
    {
      if (rbe.status.failure_status != RRCRB_MSG_VALID)
      {
        WRRC_MSG0_HIGH("Ignoring STATE_CHANGE_IND");
      }
      else
      {
        /* if cell update procedure is active with cause OOS, wait 
         * for the CU complete notification to send chan_config_req to
         * LLC 
         */
        if (rrccu_get_cell_update_started_status_with_cause(&cu_cause)== RRCCU_STARTED &&
            cu_cause == rrc_CellUpdateCause_re_enteredServiceArea) 
        {
          rrcrbe_delete_crnti = TRUE;
          WRRC_MSG0_HIGH("Complete rbe msg first.  Indicate compl to CU once done");
        }
        
        MSG_LOW("Sending RRC_CHANNEL_CONFIG_REQ to LLC", 0, 0, 0);
        /* Call the function that creates and sends the  channel
           config information to be requested from LLC. */
      
        if ((out_cmd_ptr = rrc_get_int_cmd_buf())!=NULL)
        {
          WCDMA_MEMCPY(out_cmd_ptr,
                       sizeof(rrc_cmd_type),
                       &rrc_cmd,
                       sizeof(rrc_cmd_int_type));
          rrc_put_int_cmd( out_cmd_ptr );
        }
        
        /* Change the substate since we're waiting for the
           confirmation from LLC for the channel configs */
        rbe.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
      
        if((rrccsp_is_selected_cell_utran_directed()) &&
           (rbe.new_crnti_valid))
        {
          /* No need to initiate cell update as the C-RNTI is available
          * and UE is camped on the directed cell.
          */
          rrcrbe_send_initiate_cu_flag=FALSE;
        }
        else
        {
          rrcrbe_send_initiate_cu_flag=TRUE;
          rrcrbe_delete_crnti = TRUE;
        }
#ifdef FEATURE_WCDMA_HS_FACH
 /* If H-RNTI is not given in OTA then initiate cell update*/
        if(rrcenhstate_set_hrnti_status_for_transitions())
        {
          rrcrbe_send_initiate_cu_flag = TRUE;
          rrcrbe_delete_crnti          = TRUE;
        }
#endif
#ifdef FEATURE_WCDMA_HS_RACH
 /* Check if E-RNTI is not given in OTA then initiate cell update*/
        if(rrchsrach_check_ernti_status())
        {
          rrcrbe_send_initiate_cu_flag = TRUE;
          rrcrbe_delete_crnti          = TRUE;
        }
#endif

      }
    }
    else
    {

      /* Currently there is no action when we go to cell_fach.
         This is not expected for release 1 since UE won't support
         transition to cell_fach. */
      WRRC_MSG0_HIGH("Received State change to CELL_FACH, no action");

    }
    /* NOTE: When we do support this, we need to start timer
    T305 if it's not running. We need to stop DRX mode, and
    if C-RNTI is not sent in the message, send a command to
    cell-update procedure. */
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

FUNCTION   RRCRBE_PROCESS_L2_ACK_IND

DESCRIPTION

  This function processes the received Layer 2 ACK indication.
  If the L2 ACK indicates that the RBS Complete message has
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
static void rrcrbe_process_l2_ack_ind
(
rrc_downlink_l2ack_ind_type *ptr /* Pointer to the L2 Ack Ind cmd */
)
{
  rrc_rlc_re_est_for_srns_rb_e_type rb_type; /*rb type tobe reestablished */
  rrc_CellUpdateCause       cu_cause;
  WRRC_MSG1_HIGH("RRCRBE:L2 ack %d <0:Success 1:Failure>for RBS Complete msg received", ptr->status);
  if ( ptr->status == SUCCESS )
  {
    /* clear trans id */
    rbe.tr_ids.accepted_transaction = RRCRB_INVALID_TRANSACTION_ID;

    /* if SRNS relocation is involved with this message then re
    establish the RBs other than RB2 here */
    if (rbe.rbe_for_srns_relocation)
    {
      if (RRC_STATE_CELL_DCH == rrc_get_state())
      {
        /* Initiate the re-establishment of remaining RBs before
        RBE procedure goes back to initial substate.
        No need to wait for the confirmation */
        rb_type  = RRC_RE_EST_NON_RB_2_UM_AM;
        rrcrb_send_rlc_re_est_req_to_smc(RRC_PROCEDURE_RBE, rb_type, FALSE);

        /* send rbs indication to upperlayers */
        rrcrbe_send_indication_to_upper_layers();
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
     then NACK coming from RLC shall be ingnored as RBE failure with cause
     CU complete should be sent to NW */
    if (RRC_STATE_CELL_FACH == rrc_get_state() &&
        RRCCU_STARTED == rrccu_get_cell_update_started_status_with_cause(&cu_cause) &&
        (cu_cause == rrc_CellUpdateCause_re_enteredServiceArea || cu_cause == rrc_CellUpdateCause_rlc_unrecoverableError ||
         cu_cause == rrc_CellUpdateCause_radiolinkFailure))
    {
      WRRC_MSG0_HIGH("NACK being ingnored as RBE failure msg will be sent");
      (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBE);
      return;
    }

    /* clear trans id*/
    rbe.tr_ids.accepted_transaction = RRCRB_INVALID_TRANSACTION_ID;

    if ((rbe.rbe_for_srns_relocation) ||
        (rbe.state_from_msg == RRC_STATE_CELL_PCH) ||
        (rbe.state_from_msg == RRC_STATE_URA_PCH))
    {
      /* l2ack failed. If reconfiguration involves SARNS relocation then
      go to idle. Otherwise just complete the procedure */
      WRRC_MSG2_ERROR("L2ack failed SRNSreloc:%d, next_st%d",
          rbe.rbe_for_srns_relocation,
          rbe.state_from_msg);
      /* Going to Idle should always go through CSP  */
      rrc_transition_to_disconnected_state(RRC_PROCEDURE_RBE,RRC_TX_TO_DISCON_OTHER);
    }
  }

  /* Update Dormancy Status before clearing procedure */
  if (rbe.rbe_for_srns_relocation == TRUE)
  {
    rrcdormancy_update_dormancy_for_srns_hho_cco(RRC_PROCEDURE_RBE);
  }

  /* Clear ORDERED_CONFIG - UE can now accept other ordered
  configurations */
  rrcllc_clear_ordered_config();

  /* Go to the next sub-state in the state machine. That's
  the initial substate since we've sent the RBS complete
  message */
  rbe.curr_substate = RRCRB_INITIAL;

  /* At this point we don't need to remember any of the
  Radio Bearer information since it's stored in ESTABLISHED_RABS.
  Clear our internal variables */
  rrcrbe_clear_procedure();
}


/*===========================================================================

FUNCTION   RRCRBE_INITIAL_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBE procedure is in the RRCRBE_INITIAL substate. This function
  looks at the received event and does the appropriate processing
  of the event.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbe_initial_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Setup Message. Otherwise
    we would not have received this command. Copy the
    received message pointer. */
    /* Process the RB Setup message */
    process_rbsetup_message( cmd_ptr );
    break;

  default:

    /* No other events except Downlink SDU is expected in this
    substate */
    MSG_MED("Ignoring RBE event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate, 0);
    break;
  }
}


 
/*===========================================================================

FUNCTION   RRCRBE_DCH_WFSRB2REESTABLISH_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBE procedure is in the RRCRBE_WAIT_FOR_REESTABLISH_SRB2 substate
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
static void rrcrbe_dch_wfsrb2reestablish_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_RE_EST_RLC_FOR_SRNS_CNF:

    /* Confirmation from SMC of RLC re-establishment confirm */
    rrcrbe_process_rlc_reestablishment_cnf( );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbe_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Reconfig Message. Otherwise
    we would not have received this command. Copy the
    received message pointer. */
    /* Process the RB Reconfig message */
    process_rbsetup_message( cmd_ptr );

    break;

  default:

    /* No other events except Downlink SDU is expected in this
    substate */
    MSG_MED("Ignoring RBE event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate, 0);
    break;
  }
}

/*===========================================================================

FUNCTION   RRCRBE_DCH_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBE procedure is in the RRCRBE_WAIT_FOR_CHAN_CFG_CNF substate
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
static void rrcrbe_dch_wfchcnf_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcrbe_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbe_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Setup Message. Otherwise
    we would not have received this command. Copy the
    received message pointer. */
    /* Process the RB Setup message */
    process_rbsetup_message( cmd_ptr );
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBE event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate, 0);
    break;
  }
}


/*===========================================================================

FUNCTION   rrcrbe_other_wfcucnf_substate_event_handler

DESCRIPTION

  This function handles the events received by this module when
  the RBE procedure is in the RRCRBE_WAIT_FOR_CELL_UPDATE_CNF substate
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
static void rrcrbe_other_wfcucnf_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_STATE_CHANGE_IND:
      /* Indication that RRC's state has changed */
      rrcrbe_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;

    default:
      /* No other events expected in this
      substate */
      MSG_MED("Ignoring RBE event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate, 0);
      break;
  }
}

/*===========================================================================

FUNCTION   rrcrbe_other_wfcscnf_event_handler

DESCRIPTION

  This function handles the events received by this module when
  the RBE procedure is in the RRCRBE_WAIT_FOR_CELL_SELECTION_CNF substate
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
static void rrcrbe_other_wfcscnf_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_STATE_CHANGE_IND:
      /* Indication that RRC's state has changed */
      rrcrbe_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;

    default:
      /* No other events expected in this
      substate */
      MSG_MED("Ignoring RBE event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate, 0);
      break;
  }
}

/*===========================================================================

FUNCTION   RRCRBE_DCH_WFCUCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBE procedure is in the RRCRBE_WAIT_FOR_CELL_UPDATE_CNF substate
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

static void rrcrbe_dch_wfcucnf_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_CELL_UPDATE_COMPLETE_CNF:
      if(rbe.status.failure_status == RRCRB_SIMULTANEOUS_RECONFIG)
      {
        /* Send the RB Setup Failure Message */
        send_rbs_failure_message( rbe.status.failure_status,
                                  rbe.status.prot_err_cause
                                );
        /* Then clear all local variables that may
        need to be cleared */
        rrcrbe_clear_procedure();

        /* Make sure we're back in the initial sub-state */
        rbe.curr_substate = RRCRB_INITIAL;
      }
      else if(rbe.status.failure_status == RRCRB_MSG_VALID)
      {
        /* Send the Radio Bearer Setup Complete message */
        if ((rbe.state_from_msg == RRC_STATE_CELL_PCH) ||
            (rbe.state_from_msg == RRC_STATE_URA_PCH))
        {
          send_rbs_complete_message(TRUE);
        }
        else
        {
          send_rbs_complete_message(FALSE);
        }
        /* send rbs indication to upperlayers */
        rrcrbe_send_indication_to_upper_layers();
        /* prints rate for each transport channel */
        rrcllc_print_trans_chnl_rate();

        /*if next state is Cell_PCH or URA_PCH then we have to
        treat the procedure as successful only after getting L2ack
        for the response message */
        if ((rbe.state_from_msg == RRC_STATE_CELL_PCH) ||
            (rbe.state_from_msg == RRC_STATE_URA_PCH))
        {
          /* Change the substate to the next substate - this is
          where we wait for L2 ack from RLC for sending the RB RC
          complete message. */
          rbe.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
        }
        else
        {
          /* Clear ORDERED_CONFIG - UE can now accept other ordered
          configurations */
          rrcllc_clear_ordered_config();

          /* Go to the next sub-state in the state machine. That's
          the initial substate since we've sent the RBS complete
          message */
          rbe.curr_substate = RRCRB_INITIAL;

          /* At this point we don't need to remember any of the
          Radio Bearer information since it's stored in ESTABLISHED_RABS.
          Clear our internal variables */
          rrcrbe_clear_procedure();
        }
      }
      /*  In the case of Phy_Chan_Failure, RB setup failure must be send
      and oc should be reset. */
      else if(rbe.status.failure_status == RRCRB_PHY_CHAN_FAILURE)
      {

        /* Send the RB Setup Failure Message */
        send_rbs_failure_message( rbe.status.failure_status,
                                  rbe.status.prot_err_cause
                                );

        /* Then clear all local variables that may
        need to be cleared */
        rrcrbe_clear_procedure();
        /* Make sure we're back in the initial sub-state */
        rbe.curr_substate = RRCRB_INITIAL;
      }
      else
      {
        WRRC_MSG1_ERROR("Failure status %d not handled",rbe.status.failure_status);
      }

    break;

  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcrbe_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbe_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Setup Message. Otherwise
    we would not have received this command. Copy the
    received message pointer. */
    /* Process the RB Setup message */
    process_rbsetup_message( cmd_ptr );
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBE event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate, 0);
    break;
  }

}


/*===========================================================================

FUNCTION   RRCRBE_FACH_WFCUCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBE procedure is in the RRCRBE_WAIT_FOR_CELL_UPDATE_CNF substate
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

static void rrcrbe_fach_wfcucnf_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  rrc_cmd_type  *out_cmd_ptr;

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_CELL_UPDATE_COMPLETE_CNF:
#ifdef FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE
      #error code not present
#endif
      if(rbe.status.failure_status == RRCRB_SIMULTANEOUS_RECONFIG)
      {
        /* Send the RB Setup Failure Message */
        send_rbs_failure_message( rbe.status.failure_status,
                                  rbe.status.prot_err_cause
                                );
        /* Then clear all local variables that may
        need to be cleared */
        rrcrbe_clear_procedure();

        /* Make sure we're back in the initial sub-state */
        rbe.curr_substate = RRCRB_INITIAL;
      }
      else if(rbe.status.failure_status == RRCRB_MSG_VALID)
      {

          /* if OOS happens while waiting for cell update confirm then send  
         * channel config request again to reset TOC and to configure lower layers
             The extra condition added for checking the toc_usage_change_oos_to_dch variable 
           is not needed. It has just been added for protection. The extra check is
           sending another channel configuration so removing the check.
           The extra check is not needed because in case of radio bearer release/setup
           we end up releasing/establishing the same rb twice which will fail.*/
        if (rrcllc_get_toc_usage() ==TOC_FOR_OOS )
        {
          WRRC_MSG1_HIGH("DEBUG: Send channel config req for OOS scenarios TOC usage %d ",rrcllc_get_toc_usage());
					
          /* Call the function that creates and sends the  channel
             config information to be requested from LLC. */
          if ((out_cmd_ptr = rrc_get_int_cmd_buf())!=NULL)
          {
          
             WCDMA_MEMCPY(out_cmd_ptr,
                          sizeof(rrc_cmd_type),
                          &rrc_cmd,
                          sizeof(rrc_cmd_int_type));
          
             rrc_put_int_cmd( out_cmd_ptr );
          }
          /* Change the substate since we're waiting for the
             confirmation from LLC for the channel configs */
          rbe.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;	
          return;					
        }

        /* Send the Radio Bearer Setup Complete message */
        if ((rbe.state_from_msg == RRC_STATE_CELL_PCH) ||
            (rbe.state_from_msg == RRC_STATE_URA_PCH))
        {
          send_rbs_complete_message(TRUE);
        }
        else
        {
          send_rbs_complete_message(FALSE);
        }
        /* send rbs indication to upperlayers */
        rrcrbe_send_indication_to_upper_layers();
        /* prints rate for each transport channel */
        rrcllc_print_trans_chnl_rate();
        /* if next state is Cell_PCH or URA_PCH then we have to 
           treat the procedure as successful only after getting L2ack
           for the response message */
        if ((rbe.state_from_msg == RRC_STATE_CELL_PCH) ||
            (rbe.state_from_msg == RRC_STATE_URA_PCH))
        {
          /* Change the substate to the next substate - this is
             where we wait for L2 ack from RLC for sending the RB RC
             complete message. */
          rbe.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
        }
        else
        {
          /* Clear ORDERED_CONFIG - UE can now accept other ordered
             configurations */
          rrcllc_clear_ordered_config();  

          /* Go to the next sub-state in the state machine. That's
             the initial substate since we've sent the RBS complete
             message */
          rbe.curr_substate = RRCRB_INITIAL;

          /* At this point we don't need to remember any of the
             Radio Bearer information since it's stored in ESTABLISHED_RABS.
             Clear our internal variables */
          rrcrbe_clear_procedure();
        }
      }
      /*  In the case of Phy_Chan_Failure, RB setup failure must be send
      and oc should be reset. */
      else if(rbe.status.failure_status == RRCRB_PHY_CHAN_FAILURE)
      {
        /* Send the RB Setup Failure Message */
        send_rbs_failure_message( rbe.status.failure_status,
                                  rbe.status.prot_err_cause
                                );

        /* Then clear all local variables that may
        need to be cleared */
        rrcrbe_clear_procedure();
        /* Make sure we're back in the initial sub-state */
        rbe.curr_substate = RRCRB_INITIAL;
      }
      else
      {
        WRRC_MSG1_ERROR("Failure status %d not handled",rbe.status.failure_status);
      }

    break;

  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcrbe_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbe_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Setup Message. Otherwise
    we would not have received this command. Copy the
    received message pointer. */
    /* Process the RB Setup message */
    process_rbsetup_message( cmd_ptr );
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBE event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate, 0);
    break;
  }

}

/*===========================================================================

FUNCTION   RRCRBE_FACH_WF_CSCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBE procedure is in the RRCRBE_WAIT_FOR_CELL_UPDATE_CNF substate
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

static void rrcrbe_fach_wfcscnf_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_STATE_CHANGE_IND:
   /* Indication that RRC's state has changed */
    rrcrbe_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );

    break;


  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Setup Message. Otherwise
    we would not have received this command. Copy the
    received message pointer. */
    /* Process the RB Setup message */
    process_rbsetup_message( cmd_ptr );
    break;
  case RRC_NEW_CELL_IND:
    rrcrbe_process_new_cell_ind( &(cmd_ptr->cmd.new_cell_ind) );
    break;

  case RRC_CELL_UPDATE_COMPLETE_CNF:
    if(rbe.state_from_msg == RRC_STATE_CELL_FACH )
    {
  
      /* if OOS was detected during trans from DCH, cell update
         procedure has completed.  send channel config req to 
         complete the reconfiguration procedure
      */
      uint8 rab_index =0;
      boolean chan_conf_needed = FALSE;
      /* If the RB doesn't exist in EST_RABS, it means RB setup has not yet sent a channel config. CR#830919*/
      for (rab_index=0; rab_index < rbe.rabs.num_rabs_in_msg; rab_index++)
      {
        if(((rrc_find_rab_in_established_rabs( rbe.rabs.rabs_in_msg[rab_index].rab_id ))
                == RRC_RAB_NOT_FOUND ))
        {
          chan_conf_needed = TRUE;
          break;
        }
      }
      if(chan_conf_needed == TRUE)
      {
          /* RBE has not sent any channel config yet. Send the channel config now */
        rrc_cmd_type  *out_cmd_ptr;
        WRRC_MSG0_HIGH("Sending RRC_CHANNEL_CONFIG_REQ to LLC");
        /* Call the function that creates and sends the  channel
        config information to be requested from LLC. */
    
        if ((out_cmd_ptr = rrc_get_int_cmd_buf())!=NULL)
        {
          WCDMA_MEMCPY(out_cmd_ptr,
                       sizeof(rrc_cmd_type),
                       &rrc_cmd,
                       sizeof(rrc_cmd_int_type));
          rrc_put_int_cmd( out_cmd_ptr );
        }

        /* Also reset the rrcrbe_send_initiate_cu_flag to false as cell update
         * will not be needed since CU and CUCNF for oos took care 
         * of that
         */
        rrcrbe_send_initiate_cu_flag = FALSE;
  
        /* Change the substate since we're waiting for the
        confirmation from LLC for the channel configs */
        rbe.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
      
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
    MSG_MED("Ignoring RBE event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate, 0);
    break;
  }

}

/*===========================================================================

FUNCTION   RRCRBE_FACH_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBE procedure is in the RRCRBE_WAIT_FOR_CHAN_CFG_CNF substate
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
static void rrcrbe_fach_wfchcnf_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcrbe_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbe_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Setup Message. Otherwise
    we would not have received this command. Copy the
    received message pointer. */
    /* Process the RB Setup message */
    process_rbsetup_message( cmd_ptr );
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBE event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate, 0);
    break;
  }
}



/*===========================================================================

FUNCTION   RRCRBE_OTHER_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBE procedure is in the RRCRBE_WAIT_FOR_CHAN_CFG_CNF substate
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
static void rrcrbe_other_wfchcnf_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_CHANNEL_CONFIG_CNF:
  case RRC_DOWNLINK_SDU_IND:

    /* Confirmation from LLC for the channel configurations */
    WRRC_MSG1_HIGH("Ignoring chan config cnf since UE not in DCH or FACH cmd_id:%0x",cmd_ptr->cmd_hdr.cmd_id);
    /* In this state this procedure should no longer be active. */

    /* Then change to initial substate */
    rbe.curr_substate = RRCRB_INITIAL;

    /* Then clear all local variables that may
    need to be cleared */
    rrcrbe_clear_procedure();
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbe_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBE event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate, 0);
    break;
  }
}



/*===========================================================================

FUNCTION   RRCRBE_DCH_WFL2ACK_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBE procedure is in the RRCRBE_WAIT_FOR_L2_ACK substate
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
static void rrcrbe_dch_wfl2ack_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_DOWNLINK_L2ACK_IND:

    /* Confirmation from LLC for the channel configurations */
    rrcrbe_process_l2_ack_ind( &(cmd_ptr->cmd.downlink_l2ack_ind) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbe_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Setup Message. Otherwise
    we would not have received this command. Copy the
    received message pointer. */
    /* Process the RB Setup message */
    process_rbsetup_message( cmd_ptr );
    break;

  case RRC_CELL_UPDATE_COMPLETE_CNF:
    rbe.status.failure_status = RRCRB_CELL_UPDATE_OCCURED;
    /* Send the RBE Failure Message */
    send_rbs_failure_message(rbe.status.failure_status,
                              rbe.status.prot_err_cause
                             );

    /* Then clear all local variables that may
    need to be cleared */
    rrcrbe_clear_procedure();

    /* Make sure we're back in the initial sub-state */
    rbe.curr_substate = RRCRB_INITIAL;
    break;

  default:

    /* No other events except Downlink SDU is expected in this
    substate */
    MSG_MED("Ignoring RBE event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate, 0);
    break;
  }
}


/*===========================================================================

FUNCTION   RRCRBE_FACH_WFL2ACK_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBE procedure is in the RRCRBE_WAIT_FOR_L2_ACK substate
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
static void rrcrbe_fach_wfl2ack_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_DOWNLINK_L2ACK_IND:

    /* Confirmation from LLC for the channel configurations */
    rrcrbe_process_l2_ack_ind( &(cmd_ptr->cmd.downlink_l2ack_ind) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbe_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Setup Message. Otherwise
    we would not have received this command. Copy the
    received message pointer. */
    /* Process the RB Setup message */
    process_rbsetup_message( cmd_ptr );
    break;


  case RRC_CELL_UPDATE_COMPLETE_CNF:
    rbe.status.failure_status = RRCRB_CELL_UPDATE_OCCURED;
    /* Send the RBE Failure Message */
    send_rbs_failure_message(rbe.status.failure_status,
                              rbe.status.prot_err_cause
                             );

    /* Then clear all local variables that may
    need to be cleared */
    rrcrbe_clear_procedure();

    /* Make sure we're back in the initial sub-state */
    rbe.curr_substate = RRCRB_INITIAL;
    break;

  default:

    /* No other events except Downlink SDU is expected in this
    substate */
    MSG_MED("Ignoring RBE event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate, 0);
    break;
  }
}



/*===========================================================================

FUNCTION   RRCRBE_OTHER_WFL2ACK_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBE procedure is in the RRCRBE_WAIT_FOR_L2_ACK substate
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
static void rrcrbe_other_wfl2ack_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_DOWNLINK_L2ACK_IND:
  case RRC_DOWNLINK_SDU_IND:

    /* Confirmation from LLC for the channel configurations */
    WRRC_MSG1_HIGH("Ignoring L2 ack/downlink SDU since UE not in DCH or FACH cmd_id:%0x", cmd_ptr->cmd_hdr.cmd_id );
    /* In this state this procedure should no longer be active. */

    /* Then change to initial substate */
    rbe.curr_substate = RRCRB_INITIAL;

    /* Then clear all local variables that may
    need to be cleared */
    rrcrbe_clear_procedure();
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbe_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  default:

    /* No other events except Downlink SDU is expected in this
    substate */
    WRRC_MSG2_ERROR("Ignoring RBE event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate);
    break;
  }
}

/*===========================================================================

FUNCTION   RRCRBE_FACH_WFCRNTIUPDATECNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBE procedure is in the rrcRBE_WAIT_FOR_CRNTI_UPDATE_CNF substate
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

static void rrcrbe_fach_wfcrntiupdatecnf_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_STATE_CHANGE_IND:
   /* Indication that RRC's state has changed */
    rrcrbe_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );

    break;


  case RRC_RNTI_UPDATE_CNF:
    if (cmd_ptr->cmd.rnti_update_cnf.status == SUCCESS) 
    {
      rrcrbe_process_rnti_update_cnf();
    }
    else
    {
      WRRC_MSG0_HIGH("RNTI update failed.  Send RBS failure msg");
      rbe.status.failure_status = RRCRB_MSG_VALID;
      
      send_rbs_failure_message(rbe.status.failure_status,
                               rbe.status.prot_err_cause);
      
      /* Then clear all local variables that may
         need to be cleared */
      rrcrbe_clear_procedure();

      /* Make sure we're back in the initial sub-state */
      rbe.curr_substate = RRCRB_INITIAL;
    }
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBE event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate, 0);
    break;
  }

}


/*===========================================================================

FUNCTION   RRCRBE_PCH_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBE procedure is in the RRCRBE_WAIT_FOR_CHAN_CFG_CNF substate
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
static void rrcrbe_pch_wfchcnf_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{


  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcrbe_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbe_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:
  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBE event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate, 0);
    break;
  }
}

/*===========================================================================

FUNCTION   RRCRBE_PCH_WFCUCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBE procedure is in the RRCRBE_WAIT_FOR_CELL_UPDATE_CNF substate
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
static void rrcrbe_pch_wfcucnf_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_CHANNEL_CONFIG_CNF:
    /* Confirmation from LLC for the channel configurations */
    rrcrbe_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:
    /* Indication that RRC's state has changed */
    rrcrbe_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;



    case RRC_DOWNLINK_SDU_IND:
      WRRC_MSG2_HIGH("Unexpected DOWNLINK_SDU_IND in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate);
    break;

  default:

    /* No other events expected in this
    substate */
    WRRC_MSG2_ERROR("Ignoring RBE event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate);
    break;
  }

}

/*===========================================================================

FUNCTION   RRCRBE_PCH_WF_CSCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBE procedure is in the rrcRBE_WAIT_FOR_CELL_SELECTION_CNF substate
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

static void rrcrbe_pch_wfcscnf_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_STATE_CHANGE_IND:
   /* Indication that RRC's state has changed */
    rrcrbe_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );

    break;


  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBE event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate, 0);
    break;
  }

}



/*===========================================================================

FUNCTION   RRCRBE_CELL_DCH_EVENT_HANDLER

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
static void rrcrbe_cell_dch_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  if ((cmd_ptr->cmd_hdr.cmd_id == RRC_DOWNLINK_L2ACK_IND) &&
      (rbe.curr_substate != RRCRB_WAIT_FOR_L2_ACK))
  {
    /* We could get L2ack for a previously sent message while we are already
    processing the next message. So l2ack could be received any any of the following
    substates. If RBE substate does not indicate that we are not waiting for l2ack
    then just print the status */
    MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCRBE:L2ack %d <0-Success 1-Failure> for MUI:%d, Lc_Id:%d, rbe_subst:%d",
    cmd_ptr->cmd.downlink_l2ack_ind.status,
               cmd_ptr->cmd.downlink_l2ack_ind.mui,
               cmd_ptr->cmd.downlink_l2ack_ind.lc_id, rbe.curr_substate);

    /* return from here so that procedure can continue with existing process */
    return;
  }
  /* Switch on the current substate */
  switch ( rbe.curr_substate )
  {
  case RRCRB_INITIAL:

    /* Call the event handler for RRCRBE_INITIAL substate */
    rrcrbe_initial_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_CHAN_CFG_CNF:

    /* Call the event handler for RRCRBE_WAIT_FOR_CHAN_CFG_CNF substate */
    rrcrbe_dch_wfchcnf_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_L2_ACK:

    /* Call the event handler for RRCRBE_WAIT_FOR_L2_ACK substate */
    rrcrbe_dch_wfl2ack_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_REESTABLISH_SRB2:

    /* Call the event handler for RRCRBE_WAIT_FOR_REESTABLISH_SRB2 substate */
    rrcrbe_dch_wfsrb2reestablish_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
    /* Call the event handler for RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate */
    rrcrbe_dch_wfcucnf_event_handler( cmd_ptr);
    break;

  default:

    /* No other substates processed for this RRC state    */
    WRRC_MSG2_ERROR("Command %x not processed in CELL_DCH for substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate);
    break;

  }
}



/*===========================================================================

FUNCTION   RRCRBE_CELL_FACH_EVENT_HANDLER

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
static void rrcrbe_cell_fach_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  if ((cmd_ptr->cmd_hdr.cmd_id == RRC_DOWNLINK_L2ACK_IND) &&
      (rbe.curr_substate != RRCRB_WAIT_FOR_L2_ACK))
  {
    /* We could get L2ack for a previously sent message while we are already
    processing the next message. So l2ack could be received any any of the following
    substates. If RBE substate does not indicate that we are not waiting for l2ack
    then just print the status */

      MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"L2ack %d <0-Success 1-Failure> for MUI:%d, Lc_Id:%d, rbe_subst:%d",
               cmd_ptr->cmd.downlink_l2ack_ind.status,
               cmd_ptr->cmd.downlink_l2ack_ind.mui,
               cmd_ptr->cmd.downlink_l2ack_ind.lc_id, rbe.curr_substate);

    /* return from here so that procedure can continue with existing process */
    return;
  }

  /* Switch on the current substate */
  switch ( rbe.curr_substate )
  {
  case RRCRB_INITIAL:
    /* Call the event handler for RRCRBE_INITIAL substate */
    /* NOTE: Here we need to check if a cell-update procedure
    has been initialized or not. For now, just call the event handler */
    rrcrbe_initial_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
    /* Call the event handler for RRCRBE_WAIT_FOR_CELL_UPDATE_CNF substate */
    rrcrbe_fach_wfcucnf_event_handler( cmd_ptr);
    break;

  case RRCRB_WAIT_FOR_CELL_SELECTION_CNF:
    rrcrbe_fach_wfcscnf_event_handler( cmd_ptr);
    break;

  case RRCRB_WAIT_FOR_CHAN_CFG_CNF:
    /* Call the event handler for RRCRBE_WAIT_FOR_CHAN_CFG_CNF substate */
    rrcrbe_fach_wfchcnf_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF:
    /* Call the event handler for RRCRBE_WAIT_FOR_CRNTI_UPDATE_CNF substate */
    rrcrbe_fach_wfcrntiupdatecnf_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_L2_ACK:
    /* Call the event handler for RRCRBE_WAIT_FOR_L2_ACK substate */
    rrcrbe_fach_wfl2ack_substate_event_handler( cmd_ptr );
    break;

  default:
    /* No other substates processed for this RRC state    */
    WRRC_MSG2_ERROR("Command %x not processed in CELL_FACH for substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate);
    break;
  }
}


/*===========================================================================

FUNCTION   RRCRBE_PCH_EVENT_HANDLER

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
static void rrcrbe_pch_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the current substate */
  switch ( rbe.curr_substate )
  {
  case RRCRB_INITIAL:

    /* Call the event handler for RRCrbe_INITIAL substate */
    rrcrbe_initial_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
    /* Call the event handler for RRCRBE_WAIT_FOR_CELL_UPDATE_CNF substate */
    rrcrbe_pch_wfcucnf_event_handler( cmd_ptr);
    break;

  case RRCRB_WAIT_FOR_CELL_SELECTION_CNF:

    rrcrbe_pch_wfcscnf_event_handler( cmd_ptr);
    break;

  case RRCRB_WAIT_FOR_CHAN_CFG_CNF:

    /* Call the event handler for RRCRBE_WAIT_FOR_CHAN_CFG_CNF substate */
    rrcrbe_pch_wfchcnf_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_L2_ACK:

    /* The only way we could be in pch and rb state is "wait_for_l2_ack is if
       oos happened while waiting for l2 ack and cell update confirm takes UE to pch
       In this case, send failure msg of rbrc with cause cell update occured
    */
    rbe.status.failure_status = RRCRB_CELL_UPDATE_OCCURED;

    send_rbs_failure_message(rbe.status.failure_status,
                              rbe.status.prot_err_cause);

    /* Then clear all local variables that may
       need to be cleared */
    rrcrbe_clear_procedure();

    /* Make sure we're back in the initial sub-state */
    rbe.curr_substate = RRCRB_INITIAL;
    break;

  default:

    /* No other substates processed for this RRC state    */
    MSG_MED("Command %x not processed in substate%d rrc_state%d",
            cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate, rrc_get_state());
    break;

  }
}


/*===========================================================================

FUNCTION   RRCRBE_OTHER_STATE_EVENT_HANDLER

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
static void rrcrbe_other_state_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the current substate */
  switch ( rbe.curr_substate )
  {

  case RRCRB_WAIT_FOR_CHAN_CFG_CNF:

    /* Call the event handler for WAIT_FOR_CHAN_CFG_CNF substate */
    rrcrbe_other_wfchcnf_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_L2_ACK:

    /* Call the event handler for WAIT_FOR_L2_ACK substate */
    rrcrbe_other_wfl2ack_substate_event_handler( cmd_ptr );
      break;

    case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
      rrcrbe_other_wfcucnf_substate_event_handler( cmd_ptr);
      break;

    case RRCRB_WAIT_FOR_CELL_SELECTION_CNF:
      rrcrbe_other_wfcscnf_event_handler( cmd_ptr);
      break;

    default:
    /* No other substates processed for this RRC state    */
    MSG_MED("Command %x not processed in IDLE or PCH states for substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbe.curr_substate, 0);
    break;
  }
}



/*===========================================================================

FUNCTION RRCRBE_PROCEDURE_EVENT_HANDLER


DESCRIPTION

  This is the main function handling external events received by the
  Radio Bearer Establishment procedure. This function is called by the
  dispatcher whenever an event is received for this module. This function
  checks the current RRC state and calls the event handler for
  that state.

  Note that the Radio Bearer Establishment procedure can be initiated
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
void rrcrbe_procedure_event_handler
(
rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  /* Get the current RRC State */
  rrc_state_e_type rrc_state = rrc_get_state();

  WRRC_MSG2_HIGH_OPT("function rrcrbe_procedure_event_handler is called in rrc_state = rrc_state_e_type_value%d and rbe.curr_substate = rrcrb_substate_e_type_value%d", rrc_state, rbe.curr_substate);
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_RBE,rbe.curr_substate,cmd_ptr->cmd_hdr.cmd_id);

  if ((cmd_ptr->cmd_hdr.cmd_id == RRC_STATE_CHANGE_IND) &&
      (cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED)) 
  {
    /* init mac_rab_Status to no rabs */
    mac_rab_status = NO_RABS;
    MSG_LOW("MAC_RAB status;%d", mac_rab_status, 0, 0);
  } 

  switch (rrc_state)
  {

  case RRC_STATE_CELL_FACH:
    /* If it's CELL_FACH state, call the event handler
    for CELL_FACH state */
    rrcrbe_cell_fach_event_handler(cmd_ptr);
    break;

  case RRC_STATE_CELL_DCH:
    /* If it's CELL_DCH state, call the event handler
    for CELL_DCH state */
    rrcrbe_cell_dch_event_handler(cmd_ptr);
    break;

    case RRC_STATE_URA_PCH:
    case RRC_STATE_CELL_PCH:
      /* If it's CELL_DCH state, call the event handler
      for CELL_PCH state */
      rrcrbe_pch_event_handler(cmd_ptr);
      break;

  case RRC_STATE_CONNECTING:
  case RRC_STATE_DISCONNECTED:
    /* If it's any other state call the event handler
    for other states. The processing of events in
    other RRC states is identical for this procedure */
    rrcrbe_other_state_event_handler(cmd_ptr);
    break;

  default:
    WRRC_MSG1_ERROR("Invalid RRC State: %d", rrc_state);
    break;
  }
}/* end function rrcrbe_procedure_event_handler */



/*===========================================================================

FUNCTION  RRCRBE_INIT_PROCEDURE

DESCRIPTION

  This function initializes the RBE procedure.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcrbe_init_procedure( void )
{
  uint32 count;

  rrcrbe_send_initiate_cu_flag = FALSE;
  rrcrbe_delete_crnti          = FALSE;

  /* Initialize the transaction id's for this procedure.
  Use a number that can't be an actual received transaction
  id. */
  rbe.tr_ids.accepted_transaction = RRCRB_INVALID_TRANSACTION_ID;
  rbe.tr_ids.rejected_transaction = RRCRB_INVALID_TRANSACTION_ID;

  /* Initialize the current substate */
  rbe.curr_substate = RRCRB_INITIAL;
  /* Initialize CRNTI valid to FALSE */
  rbe.new_crnti_valid    = FALSE;
  rbe.directed_cell_info = FALSE;

  rbe.rbe_for_srns_relocation = FALSE;
  rbe.new_urnti_valid = FALSE;

  /* Initialize the RABS per message structure */
  for ( count=0; count < RRCRB_MAX_RAB_PER_MSG; count++ )
  {
    rbe.rabs.rabs_in_msg[count].num_rbs_for_rab = 0;
    rbe.rabs.rabs_in_msg[count].rab_type= RRCRB_UNKNOWN_RAB;
    rbe.rabs.chan_config_needed[count] = FALSE;
#ifdef FEATURE_VOC_AMR_WB
    rbe.rabs.rabs_in_msg[count].nas_Synchronisation_Indicator = MVS_MODE_NONE;
#endif /*FEATURE_VOC_AMR_WB*/
  }
  rbe.rabs.num_rabs_in_msg = 0;
  /* Init mac rab status to NO_RABS */
  mac_rab_status = NO_RABS;
  rbe.status.failure_status = RRCRB_MSG_VALID;
  rbe.first_tm_rb_setup = FALSE;
  
  /* Register for state change notification -
  We want to be notified when the state changes
  from CELL_DCH to CELL_FACH and from any state
  to disconnected state. */
  rrcscmgr_register_for_scn( RRC_PROCEDURE_RBE,  /* Procedure name */
                             RRC_STATE_CELL_DCH, /* From State     */
                             RRC_STATE_CELL_FACH /* To State       */
                           );

  rrcscmgr_register_for_scn( RRC_PROCEDURE_RBE,  /* Procedure name */
                             RRC_STATE_WILDCARD, /* From State     */
                             RRC_STATE_DISCONNECTED /* To State    */
                           );

  rrcscmgr_register_for_scn( RRC_PROCEDURE_RBE,  /* Procedure name */
                             RRC_STATE_CELL_DCH, /* From State     */
                             RRC_STATE_CELL_PCH /* To State       */
                           );
  rrcscmgr_register_for_scn( RRC_PROCEDURE_RBE,  /* Procedure name */
                             RRC_STATE_CELL_DCH, /* From State     */
                             RRC_STATE_URA_PCH /* To State       */
                           );

  rrcscmgr_register_for_scn( RRC_PROCEDURE_RBE,  /* Procedure name */
                             RRC_STATE_CELL_FACH, /* From State     */
                             RRC_STATE_CELL_PCH /* To State       */
                           );

  rrcscmgr_register_for_scn( RRC_PROCEDURE_RBE,  /* Procedure name */
                             RRC_STATE_CELL_FACH, /* From State     */
                             RRC_STATE_URA_PCH /* To State       */
                           );
  /* Define a timer used to delay 5 seconds before
  sending the RB Setup Complete message */
  rex_def_timer(&rrcrbe_temp_timer, rex_self(), RRCRBE_TEMP_TIMER_SIG);

}
/*===========================================================================

FUNCTION rrcrbe_get_est_rab_ptr

DESCRIPTION

  This function returns established rabs ptr to SMC procedure

DEPENDENCIES

  None.

RETURN VALUE

  Pointer to established rabs

SIDE EFFECTS

  None.

===========================================================================*/
rrcrb_rab_info_per_msg_type  * rrcrbe_get_est_rab_ptr(void)
{
  return &(rbe.rabs);
}
/*====================================================================
FUNCTION: rrc_l1_query_is_amr_call_enabled

DESCRIPTION:
  This function is called during the CPHY_SETUP processing and the return
  value signifies whether an AMR voice call is actually being setup or not.
  The initial call_setup_mask was returning VOICE CALL SETUP UP if the
  domain is CS. This function enhances more checks and returns TRUE only
  if it is going to be a CS voice call. 

DEPENDENCIES:
  NONE

RETURN VALUE:
  Return value is a boolean and gives information whether AMR call is being
  established.

SIDE EFFECTS:
  None.
====================================================================*/

boolean rrc_l1_query_is_amr_call_enabled(void)
{
  uint8 rab_index;


  /*Assuming DCH to FACH transition doesn't setup Voice RAB*/
  if(ordered_config.set_by_proc == RRC_PROCEDURE_RBE)
  {
    for(rab_index =0; rab_index < RRCRB_MAX_RAB_PER_MSG; rab_index++)
    {
      if( (rbe.rabs.rabs_in_msg[rab_index].cn_domain == RRC_CS_DOMAIN_CN_ID)&& 
          (rbe.rabs.rabs_in_msg[rab_index].num_rbs_for_rab > 1) )
      {
        WRRC_MSG0_HIGH("Voice RAB found in rbe");
        return TRUE;
      }
    }
  }

  /*Loop through all the EST_RABs to find out any RAB is mapped to AMR call.*/
  for(rab_index =0; rab_index <MAX_RAB_TO_SETUP; rab_index++)
  {
    if( (rrc_est_rabs.rabs[rab_index].cn_domain == RRC_CS_DOMAIN_CN_ID) && 
        (rrc_est_rabs.rabs[rab_index].num_rbs_for_rab > 1) )
    {
      WRRC_MSG0_HIGH("Voice RAB found in est rabs");
      return TRUE;
    }
  }

  return FALSE;
}
/*===========================================================================

FUNCTION   RRCRBE_SEND_INDICATION_TO_UPPER_LAYERS

DESCRIPTION
  This function sends RABMAS_RAB_ESTABLISH_IND primitive
  to the RABM layer of NAS.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rrcrbe_send_rabm_est_ind_to_upper_layers
(
  void
)
{
  rabm_cmd_type *rabm_ptr; /* To send the RABMAS_RAB_ESTABLISH_IND command */
  uint32 rab_index=0;    /* Used if there is more than 1 RAB in the message */

   /* Check whether this RAB exists in established_rab db */
  for (rab_index=0; rab_index<MAX_RAB_TO_SETUP; rab_index++)
  {
    if ((rrc_est_rabs.rabs[rab_index].cn_domain == RRC_PS_DOMAIN_CN_ID) &&
         (rrc_est_rabs.rabs[rab_index].num_rbs_for_rab > 0) &&
         (rrc_est_rabs.rabs[rab_index].rab_id !=  RRC_INVALID_RAB_ID)
#ifdef FEATURE_WRLF_SYSTEM_SEL         
         &&(!((rrccu_get_rabs_need_to_be_released_T314() && rrc_est_rabs.rabs[rab_index].re_est_timer == rrc_Re_EstablishmentTimer_useT314)||
         (rrccu_get_rabs_need_to_be_released_T315() && rrc_est_rabs.rabs[rab_index].re_est_timer == rrc_Re_EstablishmentTimer_useT315)))
#endif 
        )
    {
      if ((rabm_ptr = rabm_get_cmd_buf()) == NULL)
      {
        ERR_FATAL("Can't send RABM cmd - out of memory!", 0, 0, 0);
      }
      rabm_ptr->header.cmd_id = RABMAS_RAB_ESTABLISH_IND;
      rabm_ptr->header.message_set = MS_RABM_RRC;
      rabm_ptr->cmd.rrc_establish_ind.rabid = (rabid_T)(rrc_est_rabs.rabs[rab_index].rab_id);
#ifdef FEATURE_DUAL_SIM
      rabm_ptr->cmd.rrc_establish_ind.as_id = rrc_get_as_id();
#endif

      /* Put the command on RABM queue */
      WRRC_MSG1_HIGH("Sending RAB_ESTABLISH_IND to RABM RAB ID %d",
               rrc_est_rabs.rabs[rab_index].rab_id);
      rabm_put_cmd(rabm_ptr);
    }
  }
}
/*===========================================================================

FUNCTION   rrc_is_cs_data_rab_present_fet

DESCRIPTION

  This function indicates whether any voice RAB is present or not in est_rabs database.
  This function is going to be used by MAC/RRC to verify whether Voice RAB is present 
  or not.
  This function has been modified such that it returns TRUE only if CS_VOICE_RAB is of DCH type.        
DEPENDENCIES

None

RETURN VALUE

  TRUE: if CS DATA RAB exist in est_rabs database
  FALSE: if CS DATA RAB doesnt exist in est_rabs database

SIDE EFFECTS

  None.

===========================================================================*/

boolean rrcrbe_is_cs_data_rab_present (void)
{
  uint8 count; /* Local Counter */
  
  for(count=0; count<RRCRB_MAX_RAB_PER_MSG; count++)
  {
    if( RRCRB_CS_DATA_RAB == rbe.rabs.rabs_in_msg[count].rab_type)
    {
        /* We have a match */
        return( TRUE );
    }
  }
  return(FALSE);
}
/*===========================================================================

FUNCTION rrcrbe_is_dl_ps_zero_zero_size_configured_for_all_rabs

DESCRIPTION

  This function fills in the transport block size per PS RAB

  
DEPENDENCIES


RETURN VALUE
  Returns FAILURE if PS RAB id doesn't find the size.  Otherwise returns SUCCESS.
 

SIDE EFFECTS

  None

===========================================================================*/
rrc_ps_data_rate_e_type rrcrbe_is_dl_ps_zero_zero_size_configured_for_all_rabs
(
void
)
{
  int i;
  rrc_RB_Identity rb_id;
  rrc_proc_e_type                 proc_id;
  rrcllc_oc_process_state_e_type  process_state; 
  rrcllc_oc_set_status_e_type     oc_status;
  rrc_ps_data_rate_e_type ps_zero_rb_found = RRC_PS_DATA_RATE_RAB_NOT_CONFIGURED;
  oc_status = rrcllc_get_ordered_config_state_and_proc_wo_f3(&proc_id,&process_state);

  if ((OC_NOT_SET == oc_status) || (RRC_PROCEDURE_RBE != proc_id))
  {
     return ps_zero_rb_found;
  }

  for(i = 0; i < RRCRB_MAX_RAB_PER_MSG; i++)
  {
    if(RRCRB_PS_DATA_RAB == rbe.rabs.rabs_in_msg[i].rab_type )
    {
      ps_zero_rb_found = RRC_PS_DATA_RATE_ZERO_CONFIGURED;
      rb_id = rbe.rabs.rabs_in_msg[i].rb_for_rab[0].rb_id;
      if(rrcllc_get_dl_ded_rate(RRC_STATE_CELL_DCH,rb_id) != 0)
      {
        ps_zero_rb_found = RRC_PS_DATA_RATE_NON_ZERO_CONFIGURED;
        break;
      }
    }
  }
  return ps_zero_rb_found;
}
/*====================================================================
FUNCTION: rrc_l1_query_psrab_data_64_64_is_present

DESCRIPTION:
  This function is called during the CPHY_SETUP processing and the return
  value signifies whether an 64_64 data call is actually being setup or not.

DEPENDENCIES:
  NONE

RETURN VALUE:
  Return value is a l1_rrc_config_rab_enum_type and gives information whether 64_64 call is being
  established.

SIDE EFFECTS:
  None.
====================================================================*/

l1_rrc_config_rab_enum_type rrc_l1_query_psrab_data_64_64_is_present(void)
{
	  rrcllc_oc_set_status_e_type	  oc_status;
	
	  uint32 rab_index=0,i=0,dl_rate=0,ul_rate=0;	 /* Used if there is more than 1 RAB in the message */
	  boolean ps_rab_found = FALSE;
	  l1_rrc_config_rab_enum_type l1_rrc_config_rab = RAB_TYPE_NONE;
	  oc_status = rrcllc_get_ordered_config_status_wo_f3();

	 /* Check whether this RAB exists in established_rab db */
	  for (rab_index=0; rab_index<MAX_RAB_TO_SETUP; rab_index++)
	  {
		if ((rrc_est_rabs.rabs[rab_index].cn_domain == RRC_PS_DOMAIN_CN_ID) &&
			 (rrc_est_rabs.rabs[rab_index].num_rbs_for_rab > 0) &&
			 (rrc_est_rabs.rabs[rab_index].rab_id !=  RRC_INVALID_RAB_ID)
			)
		{
		  if (ps_rab_found)
		  {
			 return RAB_TYPE_MRAB;
		  }

		  for(i=0; i< rrc_est_rabs.rabs[rab_index].num_rbs_for_rab; i++)
		  {
			  dl_rate = rrcllc_get_dl_ded_rate(rrc_get_state(),  rrc_est_rabs.rabs[rab_index].rb_for_rab[i].rb_id);
			  ul_rate = rrcllc_get_ul_ded_rate(rrc_get_state(),  rrc_est_rabs.rabs[rab_index].rb_for_rab[i].rb_id);
			  ps_rab_found = TRUE;
			  if ((64000 == dl_rate) && (dl_rate == ul_rate))
			  {
				MSG_HIGH("rrc_l1_query_psrab_data_64_64_is_present dl:%d ul:%d rb_id:%d",dl_rate, ul_rate, rrc_est_rabs.rabs[rab_index].rb_for_rab[i].rb_id);
				/*checking 64kbps*/
				l1_rrc_config_rab = RAB_TYPE_UL_DL_64_ONLY;
			  }
		  }
		}
	  }
	
	  if ((oc_status != OC_NOT_SET) && (ordered_config.set_by_proc == RRC_PROCEDURE_RBE))
	  {
		for(rab_index =0; rab_index < RRCRB_MAX_RAB_PER_MSG; rab_index++)
		{
		  if( (rbe.rabs.rabs_in_msg[rab_index].cn_domain == RRC_PS_DOMAIN_CN_ID)&& 
			  (rbe.rabs.rabs_in_msg[rab_index].num_rbs_for_rab > 0) &&
			  (rbe.rabs.rabs_in_msg[rab_index].rab_id !=  RRC_INVALID_RAB_ID) 
			)
		  {
			 if (ps_rab_found )
			 {
				return RAB_TYPE_MRAB;
			 }
			 for(i=0; i< rbe.rabs.rabs_in_msg[rab_index].num_rbs_for_rab; i++)
			 {
				dl_rate = rrcllc_get_dl_ded_rate(rrc_get_state(),  rbe.rabs.rabs_in_msg[rab_index].rb_for_rab[i].rb_id);
				ul_rate = rrcllc_get_ul_ded_rate(rrc_get_state(),  rbe.rabs.rabs_in_msg[rab_index].rb_for_rab[i].rb_id);
				ps_rab_found = TRUE;
				/*checking 64kbps*/
				if ((64000 == dl_rate) && (dl_rate == ul_rate))
				{
				  MSG_HIGH("rrc_l1_query_psrab_data_64_64_is_present dl:%d ul:%d rb_id:%d",dl_rate, ul_rate, rrc_est_rabs.rabs[rab_index].rb_for_rab[i].rb_id);
				  l1_rrc_config_rab = RAB_TYPE_UL_DL_64_ONLY;
				}
			 }
		  }
		}
	  }
     return l1_rrc_config_rab;	
}
#ifdef FEATURE_QSH_EVENT_METRIC
/*===========================================================================
FUNCTION rrc_get_sec_edch_transmission
  
DESCRIPTION
  This function is called by QSH module to collect events/metrics/dumps
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/
void rrc_get_mapping_combo_rb(rrc_proc_e_type proc_id, rrc_overall_ota_info_s* overall_ota_info_ptr)
{
  uint8 ix=0;
  uint8 rb_id = 0;
  if(proc_id == RRC_PROCEDURE_RBE)
  {
    for(ix = 0; ix< rbe.rabs.num_rabs_in_msg; ix++)
    {
  	 if(rbe.rabs.rabs_in_msg[ix].num_rbs_for_rab != 0)
  	 {
  	   rb_id = rbe.rabs.rabs_in_msg[ix].rb_for_rab[0].rb_id;
  	  break;
  	 }
    }
  }
  else
  {
    for(ix=0; ix<MAX_RAB_TO_SETUP; ix++)
    {
      if(rrc_est_rabs.rabs[ix].num_rbs_for_rab != 0)
      {
        rb_id = rrc_est_rabs.rabs[ix].rb_for_rab[0].rb_id;
        break;
      }
    }
  }
  overall_ota_info_ptr->mapping_combo_drb = rrc_get_mapping_combo(rb_id);
  
}
/*===========================================================================
FUNCTION rrc_get_mapping_combo_srb
  
DESCRIPTION
  This function is called by QSH module to collect events/metrics/dumps
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/
void rrc_get_mapping_combo_srb(rrc_overall_ota_info_s* overall_ota_info_ptr)
{
  overall_ota_info_ptr->mapping_combo_srb = rrc_get_mapping_combo(DCCH_AM_RADIO_BEARER_ID);
}

/*===========================================================================
FUNCTION rrc_get_sec_edch_transmission
  
DESCRIPTION
  This function is called by QSH module to collect events/metrics/dumps
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/
uint8 rrc_get_mapping_combo(uint8 rb_id)
{
  uint8 idx= 0; 
  uint8 comboCounter = 0;
  uint8 combo_type = 0;
  for(idx =0;idx< MAX_HANGING_RB_MAPPING_INFO;idx++)
  {
    if(rrc_ordered_hanging_rb[idx].rb_id == rb_id)
    {
      for(;comboCounter<MAX_MAPPING_COMBO;comboCounter++)
      {
        if(rrc_ordered_hanging_rb[idx].mapping_info[comboCounter].combo_type != 0)
        {
          combo_type |= rrc_ordered_hanging_rb[idx].mapping_info[comboCounter].combo_type;
        }
      }
      break;
    }
  }
  return combo_type;
}

/*===========================================================================
FUNCTION rrc_get_sec_edch_transmission
  
DESCRIPTION
  This function is called by QSH module to collect events/metrics/dumps
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/

void rrc_get_one_sided_re_est_flag(rrc_proc_e_type proc_id, rrc_overall_ota_info_s* overall_ota_info_ptr)
{
  uint8 entity_idx = UE_MAX_AM_ENTITY;
  uint8 ix=0;
  uint8 rb_id = 0;
  
  if(proc_id == RRC_PROCEDURE_RBE)
  {
    for(ix = 0; ix< rbe.rabs.num_rabs_in_msg; ix++)
    {
       if(rbe.rabs.rabs_in_msg[ix].num_rbs_for_rab != 0)
       {
         rb_id = rbe.rabs.rabs_in_msg[ix].rb_for_rab[0].rb_id;
 		break;
       }
    }
  }
  else
  {
    for(ix=0; ix<MAX_RAB_TO_SETUP; ix++)
	{
	  if(rrc_est_rabs.rabs[ix].num_rbs_for_rab != 0)
	  {
		rb_id = rrc_est_rabs.rabs[ix].rb_for_rab[0].rb_id;
		break;
	  }
	}
  }
  entity_idx = get_am_index_wo_f3(rb_id);
  if (entity_idx != UE_MAX_AM_ENTITY)
  {
    if(ordered_config_ptr->rlc_one_sided_reest[entity_idx] == TRUE)
    {
  	overall_ota_info_ptr->one_sided_re_est = 1;
    }
    else
    {
  	overall_ota_info_ptr->one_sided_re_est = 0;
    }
  }
}

/*===========================================================================
FUNCTION rrc_get_sec_edch_transmission
  
DESCRIPTION
  This function is called by QSH module to collect events/metrics/dumps
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/

void rrcqsh_get_num_rbs_added(rrc_proc_e_type proc_id, rrc_overall_ota_info_s* overall_ota_info_ptr)
{
  uint8 ix = 0;
  /* RB can be added only by RBsetup and HO to utran */
  if(proc_id == RRC_PROCEDURE_RBE)
  {
    for(ix=0 ;ix<rbe.rabs.num_rabs_in_msg; ix++)
    {
      overall_ota_info_ptr->num_rb_add += rbe.rabs.rabs_in_msg[ix].num_rbs_for_rab;
    }
  }
  /* Need to add for IHO*/
}
/*===========================================================================
FUNCTION rrc_get_sec_edch_transmission
  
DESCRIPTION
  This function is called by QSH module to collect events/metrics/dumps
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/

void rrc_get_rlc_size_type(rrc_proc_e_type proc_id, rrc_overall_ota_info_s* overall_ota_info_ptr)
{
 uint8 rb_id = 0;
 uint8 ix=0;
 if(proc_id == RRC_PROCEDURE_ASU)
 {
   return;
 }
 if(proc_id == RRC_PROCEDURE_RBE)
 {
   for(ix = 0; ix< rbe.rabs.num_rabs_in_msg; ix++)
   {
      if(rbe.rabs.rabs_in_msg[ix].num_rbs_for_rab != 0)
      {
        rb_id = rbe.rabs.rabs_in_msg[ix].rb_for_rab[0].rb_id;
		break;
      }
   }
 }
 else
 {
    for(ix=0; ix<MAX_RAB_TO_SETUP; ix++)
   	{
      if(rrc_est_rabs.rabs[ix].num_rbs_for_rab != 0)
      {
        rb_id = rrc_est_rabs.rabs[ix].rb_for_rab[0].rb_id;
		break;
      }
    }
 }
 if(rrc_get_dl_am_rlc_pdu_size(rb_id) == 0)
 {
   overall_ota_info_ptr->rlc_size_type = 1;
 }
}

/*===========================================================================
FUNCTION rrc_get_sec_edch_transmission
  
DESCRIPTION
  This function is called by QSH module to collect events/metrics/dumps
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/

uint8 rrbrbe_srns_relocation(void)
{
  return rbe.rbe_for_srns_relocation;
}

#endif
/*====================================================================
FUNCTION: rrc_rbe_get_ps_mrabs

DESCRIPTION:
  This function is called during the CPHY_SETUP processing and the return
  value signifies whether multiple PDP is configured or not (more than 2)

DEPENDENCIES:
  NONE

RETURN VALUE:
  Return value is TRUE, more than 2 PDP configured else FALSE
  established.

SIDE EFFECTS:
  None.
====================================================================*/

boolean rrc_rbe_get_ps_mrabs(void)
{
  rrcllc_oc_set_status_e_type	  oc_status;
  uint32 rab_index=0;	 /* Used if there is more than 1 RAB in the message */
  uint8 ps_rab_found = 0;
  boolean ps_mrab_found = FALSE;
  oc_status = rrcllc_get_ordered_config_status();
  
  /* Check whether this RAB exists in established_rab db */
  for (rab_index=0; rab_index<MAX_RAB_TO_SETUP; rab_index++)
  {
    if ((rrc_est_rabs.rabs[rab_index].cn_domain == RRC_PS_DOMAIN_CN_ID) &&
    (rrc_est_rabs.rabs[rab_index].num_rbs_for_rab > 0) &&
    (rrc_est_rabs.rabs[rab_index].rab_id !=  RRC_INVALID_RAB_ID)&&
    (!rrcrbr_is_rb_being_released(rrc_est_rabs.rabs[rab_index].rb_for_rab[0].rb_id))
    )
    {
      ps_rab_found+=1;
    }
    if (ps_rab_found >= RRC_PS_MRAB)
    {
      ps_mrab_found = TRUE;
      break;
    }
  }
  if ((oc_status != OC_NOT_SET) && (ordered_config.set_by_proc == RRC_PROCEDURE_RBE))
  {
    for(rab_index =0; rab_index < RRCRB_MAX_RAB_PER_MSG; rab_index++)
    {
      if( (rbe.rabs.rabs_in_msg[rab_index].cn_domain == RRC_PS_DOMAIN_CN_ID)&& 
      (rbe.rabs.rabs_in_msg[rab_index].num_rbs_for_rab > 0) &&
      (rbe.rabs.rabs_in_msg[rab_index].rab_id !=  RRC_INVALID_RAB_ID) 
      )
      {
        ps_rab_found+=1;
      }
      if (ps_rab_found >= RRC_PS_MRAB)
      {
        ps_mrab_found = TRUE;
        break;
      }
    }
  }
  return ps_mrab_found;	
}
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
/*===========================================================================

FUNCTION rrcrbe_set_failure_cause

DESCRIPTION


  
DEPENDENCIES


RETURN VALUE
  Returns FAILURE if PS RAB id doesn't find the size.  Otherwise returns SUCCESS.
 

SIDE EFFECTS

  None

===========================================================================*/
void rrcrbe_set_failure_cause(rrcrb_msg_status_e_type cause)
{
  rbe.status.failure_status = cause;
}
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/