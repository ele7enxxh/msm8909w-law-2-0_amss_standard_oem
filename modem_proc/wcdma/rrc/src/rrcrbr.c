/*===========================================================================
      R R C   R A D I O   B E A R E R   R E L E A S E   M O D U L E 

DESCRIPTION

  This module performs the Radio Bearer Release Procedure. This
  module processes the Radio Bearer Release Message and releases the 
  radio bearers given by the message. 
  
  The Radio Bearer Release Message may release both signalling radio bearers
  and radio bearers that are mapped to radio access bearers. 
  It may set also re-configure previously established radio bearers.
  The procedure may also initiate a hard-handover if the Radio Bearer
  Release Message indicates a frequency change or a frame offset change.
  
  
EXTERNALIZED FUNCTIONS

  rrcrbr_procedure_event_handler   Event handler for this procedure
  rrcrbr_init_procedure            Power-up initialization of the procedure

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Procedure must be initialized at power-up by calling rrcrbr_init_procedure. 
  
Copyright (c) 2001-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcrbr.c#1 $   

when       who     what, where, why
--------   ---     ---------------------------------------------------------
11/03/16   sp      Made changes to reset CU registration info once reconfig procedure is successful
06/20/16   sg      Made changes to reduce F3s
10/19/15   sp      Made changes to trigger OOS if L2 ack for transition to PCH is received when inter freq resel is in progress
10/16/15   as      Added code to support LTA durting activation time
04/23/15   vi      Made changes to add DMCR IE only when DMCR is supported in SIB3
04/14/15   sp      Made changes to indicate Multi rab status to L1
09/03/14   sp      Made changes to save procedure substates in crash debug info
08/08/14   ad      Made changes to trigger reconfiguration failure after cell update
                   procedure gets finished due to SIB5 change
07/23/14   sp      Made changes to remove F3s as part of FR19999
06/19/14   sg      Made changes to revert the cipher config db if cipher config type is
                   updated to step1 but later message validation failed.
06/24/14   sp      Made changes to send close session indication after sync ind for all cases. 
06/20/14   db      Mainlined FEATURE_WCDMA_HS_RACH_DELAY_CU and introduced NV control
06/17/14   sp      Made changes to mainline RLC-restablihment redesign changes
06/02/14   sp      Made changes for FR19997 - UL rlc re-establishment redesign
04/10/14   db      Made changes to detect UL size change during FACH to FACH redirection
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/13/14   sp      Made changes to include as_id in RRC_ESTABLISH_IND and RRC_SYNC_IND
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
03/14/13   md      Made changes to provide active subscription ID to NAS APIs for DSDA
01/23/13   mp      Fixing KW errors
01/03/13   ad      Made changes to reduce the redundant F3s
10/31/12   ad      Made changes not to send channel_config from procedure based on 
                   TOC usage flag toc_usage_change_oos_to_dch  
10/26/12   db      EFACH Redesign changes
10/18/12   gv      Replace FEATURE_DUAL_SIM_WCDMA_TUNEAWAY with FEATURE_DUAL_SIM
10/04/12   md      Made changes to detect RLC size change during state transitions via CELL PCH state.
10/04/12   pm      Removed unnecessary comments
10/04/12   pm      Made sure SRB's are present in Chan_config_req during FACH to DCH transition
09/28/12   ad      Made changes to send reconfiguration complete if cell update confirm
                   takes us to PCH and complete message is yet to be sent to network.
                   Made changes to stop HSDPA when CU is taking from FACH to PCH and
                   OC is not set by CU
09/26/12   pm      Made changes to reset the ordered codec when procedure is cleared
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/29/12   ad      Made changes for triggering CU if E-RNTI is not given.
                   Skipped the de-registration of WMs if HS-RACH is ON and RRC is 
                   triggering MR while coming out of PCH. Fixed bug to updated E-RNTI
                   in OC correctly. Added code to trigger CU if R99 or R5 CUCNF is received. 
                   Added code to keep L1 and MAC in sync wrt HS-RACH actions.
06/25/12   sa      Made the change to delete the C-RNTI while configuring the CU
06/12/12   sa      Made the changes to keep the active codec info
04/06/12   ad      Changes for HS-RACH feature
04/30/12   db      Added check to swap CC and OC only on DCH to FACH fail
04/25/12   as      Made changes to trigger cell update/drx req when UE recived OTA with 
                   redirection info and cell reselection in progress
04/12/12   as      Added code to support feature NB/WB handover support
04/04/12   as      Made changes to initiate transition to idle when L2 ACK fails while going to PCH
26/03/12   ad      Added changes to set the TOC for OOS if DCH to FACH timer expiry 
                   happens before OOS indication comes from L1. Also added code to 
                   send channel config request from procedure after recovery from OOS.
01/13/12   rl      Made changes to only append the start list without recalculating the start val
                   in rbr complete msg.
12/09/11   ad      Added code to log WCDMA_RRC_PROTOCOL_ERRORS packet
12/08/11   db      Made changes to trigger cell update if H-RNTI is missing 
11/20/11   db      Added changes to wait for l2 ack in all states not just DCH
                   and include reconfig status indicator in CU
10/13/11   db      Made changes to restore/clear ordered codec when RB failure occurs
08/19/11   ad      Made changes to ignore reconfigured RBs if they are also 
                   released in RB-release
06/21/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.100.50
06/09/11   ad      Added changes to ignore the duplicate RB IDs in chan_config_req
                   when releasing RBs in RBR
05/10/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.97.50
04/05/11   kp      Added changes to delay sending CLOSE_SES_IND to NAS for RB 
                   rel with DCH to FACH transition 
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
01/04/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.87.50
12/07/10   su      Provided fix to avoid access of array out of bounds
                   under FEATURE_CS_VOICE_OVER_HSPA.
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/30/10   su      Provided fix to avoid access of array out of bounds.
11/29/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.85.50
11/19/10   kp      Added code to avoid bufferoverflow of chan_config_req.rb
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/11/10   prk     Removed the code which do double freeing of the memory.
10/01/10   prk     Made changes to set modify request to RLC for the RB which 
                   is reconfigured with RB release OTA.
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
09/13/10   as      Added code to support FEATURE_DUAL_SIM
07/14/10   su      Fixed Lint errors.
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
06/10/10   prk     Made changes to wait for L2 ACK after sending RB release
                   failure message if the failure is due to the lower layer configuration.
06/07/10   ss      Added code to reset SVTHHO in case of failure message
05/07/10   prk     Fixed compiler warnings.
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
03/12/10   dm      Added code for supporting Deferred SIB reading feature.
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
02/11/10   gkg     Made changes for Opt Timer for CSoHS.
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
12/21/09   gkg     Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
11/09/09   kp      Added code to relese SRB-4 through RB Release.
11/02/09   rmsd    Fixed compilation error.
10/28/09   gkg     Fixed KW Errors.
10/13/09   gkg     Made the changes such that Codec info is set to RRC_CODEC_NONE
                   if CS DATA TYPE is mentioned in RRC_SYNC_IND.
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
10/01/09   gkg     Made changes so that SCUDIF is handled based on derived
                   configuration instead of relying on MM.
09/25/09   gkg/ps  Made changes to not intiate cell selection for FACH->FACH or
                   FACH->PCH redirection, if cell reselction is ongoing.
08/17/09   ss      Added code to provide ciphering ON/OFF indication to UI 
                   under feature flag FEATURE_UMTS_VOICE_CIPHERING_IND.
06/05/09   ps      Made changes to not start the T305 timer if value is infinity
05/08/09   ss      Updated Copyright Information
05/06/09   kp      Added support to close CN doamin indicated in RB release only 
                   if the CN doamin is open.
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
10/05/08   ps      Added the check if Primary CPICH info is present in 
                   RB Release msg for R6 and R7 path which missing earlier
11/04/08   rm      Added support for rove-in to GAN in RRC Connected mode.
                   Code changes are under FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE.
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
07/17/08   gkg     Made Changes to fix the issue of configuring MVS for AMR call
                   even though all the Voice RBs are released by RBR OTA.
06/17/08   gkg     Fixed typo while setting the DL cfg mask for RLC entity of RBs
                   under RB-Reconfig IE of OTA.
06/13/08   gkg     Added code for notification of Cell_Id change through OTAs
                   in Cell_DCH state. 
06/13/08   gkg     Made changes so that if RLC PDU size changes for RBs through
                   Transport Channel IEs of RB-Release OTA, those RBs are reconfigured
                   and therefore their RLC entity is notified for new RLC PDU size.
06/13/08   gkg     To handle the corner scenario of SCUDIF call through RB-Release,
                   made changes in rrcrbr_process_channel_config_cnf() so that 
                   global rrc_est_rabs is updated before it's used to determine
                   the no. of RBs in the RAB and AMR Class C logical id.
06/05/08   ns      Added code to store the maximum of Count C values of RBs to be released.
                   This max value will be used later to calculate START
05/30/08   gkg     In case of Cell_FACH->Cell_DCH state transition, stop T305
                   if RRC_CHANNEL_CONFIG_REQ succeeds otherwise handle timer T305
                   by initiating Cell Update.
03/06/08   da      Added MBMS phase 2C changes.  MBMS support in CELL_DCH.  
                   Includes mobility and dynamic resource management.
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
11/02/08   gkg     Made changes so that whenever there is a mismatch in what is
                   expected in an OTA msg and what is received, always INVALID 
                   Config is returned instead of UNSUPPORTED Config.
01/30/08   gkg     Made changes to send Start value in the Complete msg,
                   only when the Re-establish is caused by RBR 
01/17/08   ps      Made correction to allow  release RB-ID 4 through 
                   RB-Release msg.
01/08/08   ps/gk   Made modifcations for SCUDIF to use NAS function for
                   quering if RAB type has changed, when RAB Info not
                   provided in OTA
12/21/07   da      Support for FEATURE_MBMS
12/12/07   gkg     Added support for Voice->VT  under 
                   feature flag FEATURE_SCUDIF_ENHANCEMENT
12/03/07   ps      Added support for SRB5 through REL6 OTA messages.
11/28/07   da      In case OOS area gets detected during DCH->FACH transitions,
                   made change to setup DCCH+DTCH once returning in service before
                   sending CU message with cause "re-entered service area"
11/28/07   rm      Added support for handling multiple RLC operations on a
                   single RB under FEATURE MULTIPLE RLC OPERATIONS
10/23/07   kp      Added support for network sharing feature. Code changes to handle 
                   primary PLMN I.D in OTA message. Changes are under feature flag 
                   FEATURE NETWORK SHARING.
10/08/07   da      Made change to pass correct proc id when registering with scm.
09/25/07   ps      Added fix to send DRX Req to L1, when Redirection IE is
                   given for the camped PSC, in FACH->PCH case. This is valid
                   when FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU is enabled.
09/10/07   ps      Added support for Frequency redirection received in 
                   FACH->FACh and FACH->PCH cases under feature flag
                   FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
08/08/07   ps      Added support for feature cell id reading in DCH state, 
                   under feature flag FEATURE_WCDMA_GPS_CELL_ID_READING.
03/22/07   rm      Initialized temp_num_rb to zero to supress the compiler
                   warning
03/14/06   ps      Made correction to send failure cause as "Unsupported 
                   config" if an unknown codec type is received in OTA
03/02/06   ps      Added support for Wb-AMR under the feature flag
                   FEATURE_VOC_AMR_WB.
02/07/07   ps      Made changes to handle Standalone Rb-Mappings
02/02/07   da      Removed code that use to handle DCH->FACH activation time
                   and 300ms delay.  functionality has been moved to LLC. 
12/22/06   ps      Made changes to handle postVerificationPeriod and the
                   timingMaintainedSynchInd IE received in OTA messages
12/18/06   da      Fixed compiler warnings on 7200.
11/15/06   sm      Added support for IE rb-reconfig-info in release-6 path of
                   rb-release message
09/12/06   sm/da   Fix code review comments and general clean-up for REL6 and HSUPA
09/11/06   da      Added support for FEATURE RLC ONE SIDE RE ESTABLISHMENT
09/05/06   da      Added new func rrcrbr_get_new_crnti_valid().  This function
                   will be called when CU wants to know if the OTA msg
                   included a new CRNTI.  Also made change not to reset 
                   new_crnti_valid flag when proc is waiting for CU CNF and 
                   OOS has been detected.  This is because proc needs to send
                   a RNTI update req.
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/28/06   da      Added support for OOS area handling during transition from DCH.
08/21/06   tkk     Added SRB#5 support.
06/13/06   da      Made change to ignore non-existant RB_ID in rb release list.
06/13/06   segu    Added support for DSAC future implementation
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE REL5 while retaining the HSDPA specifc processing 
                   under FEATURE HSDPA.
04/25/06   da      Added checking the status of RNTI_UPDATE_CNF.  If failed, 
                   send failure to RBR msg and clear procedure.
03/14/06   tkk     Lint warnings/errors cleanup
03/13/06   da      Correct 2 places where CN domain info followed by signalling
                   connection release was sent to upper layers.  Signalling 
                   connection release shall be sent before CN domain info.
03/10/06   da      Added Initial support for PDCP under FEATURE_UMTS_PDCP
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1
02/23/06   ss/tkk  rrcrb_update_amr_cctrch_id_for_reconfig is called only
                   if lower layers have been successfully configured.
02/17/06   da      On DCH->PCH transition, made change indicate signalling
                   connection release to NAS before initiating cell selection.
02/17/06   da      On DCH->FACH transition, made change indicate signalling
                   connection release to NAS before initiating cell selection. 
02/15/06   sgk     Added code to process channel config cnf with config
                   cancel and to handle the cases where cu is in pending
                   config state.
12/22/05   tkk     Added support to handle CU (for certain causes) while 
                   reconfig procedure is waiting for L2 ack to go to 
                   Cell_PCH/URA_PCH from Cell_FACH.
12/19/05   da      Calling rrc_send_rb_rate_info_packet() to log rb rate info.
                   For FACH->CELL_PCH case, UE shall not do cell update if directed cell
                   info matches with current camped cell.  Else, CU is required.
12/01/05   da      If activation time for DCH-FACH transition is greater than 300 ms,
                   make delay before cell selection as long as activation time.
11/23/05   vr      Moved handling of InterRAT Vocoder optimization feature to
                   rrcdata.c
11/10/05   da      Removed extern definition of nv_rel_ind;
11/10/05   da      Checking global flag rrc_nv_rel_indicator before doing R5 related
                   processing.
10/26/05   vr      Send UL-Counter Synch info in the RBReleaseComplete message
                   only if TM ciphering is still active.Also corrected the bitmask
                   names
09/30/05   ss      Added changes for support of mac-d-hfn IE for rel5
07/29/05   da      Made change to update var mac_rab_status
07/28/05   kc      Fixed a simple compilation error. Seems like Ctrl while doing Ctrl-x 
                   Ctrl-f slipped through                   
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
03/25/05   da      Removed FEATURE CM SUPPORTED from rrcrbr_process_channel_config_cnf.
                   Had an invalid check that looks if failure was to due compressed mode and send
                   rb release failure msg with cause invalid config.
03/11/05   vk      Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE HSDPA
03/02/05  ttl      Support RRC ASN1 Release 5.
02/23/05   kc      Interrat HO optimization support.  Call IHO function to indicate
                   release of Voice-call due to RBR
01/07/05   da      Added fix to not invalidate the trans ID after building rb 
                   release complete msg as a response to CU Cnf.
01/04/05   da      Added Support for phase 1 of Physical channel establishment 
                   failure handling within DCH. 
12/21/04   da      Fixed Lint errors
11/30/04   vm/da   Added appropriate MSG_HIGHs for new RRC/NAS interface 
                   RRC_SERVICE_IND_FROM_OTA_MSG. 
11/23/04   sm      Added support for Dynamic Power class and cleaned up UL TPC power limit
                   feature code for some scenarios.
10/21/04   da      Removed FEATURE_CELLPCH_URAPCH.  CELLPCH and URAPCH states should always
                   be handled.  Deleted code that handles case when CELLPCH and URAPCH is not
                   defined.
10/19/04   da      Removed FEATURE SRNS RELOCATION.  SRNS_RELOCATION should always be
                   handled.  Deleted code that handles case when SRNS_RELOCATION is not
                   defined. 
10/07/04   da      Added a fix where RAB REL IND was being sent after signalling
                   connections has been successfully released and session was closed.
                   This occured when next state is Cell_DCH or Cell_FACH.
09/14/04   sk      Added support to save CN info in mm_cmd_buf and later send the
                   command to MM when procedure is succefully completed 
                   Set the failure cause to "protocol error" with 
                   "rrc_messageExtensionNotComprehended" when msg choice is later_than_r3
08/30/04   sgk     Assigned the right enums for cn domain in function
                   rrcrbr_process_successful_proc_completion,
                   rrcrbr_process_channel_config_cnf,
                   rrcrbr_process_l2_ack_ind, rrcrbr_fach_wfcucnf_event_handler
                   to fix lint error: Type mismatch (arg. no. 1) (enum/enum).
                   Removed function parameter rrc_rnti_update_cnf_type *ptr
                   from function rrcrbr_process_rnti_update_cnf and corrected
                   it in the calling location, to fixl int error Symbol 'ptr'
                   not referenced.                   
                   Removed return after ERR_FATAL in functions 
                   rrcrbr_append_start_list_to_rbr_complete,
                   send_rbr_failure_message, send_rbr_complete_message,
                   rrcrbr_process_channel_config_cnf.
                   Checked the return value of the function
                   'rrcsmc_process_cipher_config_for_hho(void)' for FAILURE,
                   and printed a MSG_HIGH in function validate_rbrelease_message
                   to fix lint error: Ignoring return value of function.
                   Checked if the return of 'rrcllc_reset_ordered_config(enum {...})'
                   != OC_NOT_SET and then printed a msg high if so, in function
                   rrcrbr_process_l2_ack_ind to fix lint error Ignoring return
                   value of function.
08/20/04   jh(kc)  Added call to rrcrb_update_amr_cctrch_id_for_reconfig().
08/09/04   kc      Complete support for RB-Re-est of TM-bearers when transitioning from
                   non-DCH state to DCH state.
07/14/04   sk      RL failure/RLC reset handling
                   Handle state change while waiting for l2ack.
                   Removed the local substate enum type. modified the code to use
                   the rrbrb_substate_e_type declared in rrcrbcommon.h
                   Added an API to get the procedure current substate.
06/04/04   svk     Call L1function to enable TFC restriction for UE Tx Power for
                   PS calls only. Under
                   feature #ifdef FEATURE_WCDMA PWR TFC ELMN
05/18/04   sk      Added a check for the state indicated by the message in 
                   state handling in process_channel_config_cnf()
05/04/04   sk      Added Freq/Scr handling in reconfiguration for CELL_FACH->CELL_FACH
                   transition.
                   Changed rrcrb_send_rnti_update_req() input parms.
03/04/04   ttl     Added FEATURE_CELLPCH_URAPCH for code of Cell_PCH/URA_PCH.
02/03/04   sk      Added CELL_PCH/URA_PCH support
                   clear the ordered_config variable immediately after sending response
                   message, when SRNS relocation is not involved and next state is 
                   not cell_PCH or URA_PCH. 
                   Handle l2ack in substates otherthan waiting_for_l2_ack substate.
                   SRNS relocation support in rrcrbr complete message triggered by 
                   CU proc
12/10/03   bu      Removed the calls to clear_ordered_config(). It will be called
                   only when L2 Ack is received. Also removed handling of NEW_CELL_IND
                   as it will never be received.
11/18/03   sk      Go to idle if L2ack for RBR complete message fails and 
                   the procedure involves SRNS relocation. Else do not take
                   any action on L2ack failure.
11/17/03   kc      Added support for SRNS-Relocation in RB-Release procedure.
08/11/03   vn      Initialize new field for RLC re-establishment in RNTI_UPDATE_REQ
03/25/03   sk      Added a chek for rb existance in established rab.
03/25/03   sk      Added 300ms delay during CELL_DCH->CELL_FACH transition
03/20/03   sk      Added missing CELL_FACH->CELLDCH trasition support
03/18/03   sk      Modified the transaction Id checking in 
                   rrcrbr_process_rbrelease_message()
02/13/02   vn      Added functionality to process Reconfig message going from
                   CELL_DCH to CELL_FACH, that comes with a new C-RNTI. If
                   camped on the same cell as directed and if C-RNTI is
                   available, do not initiate Cell Update procedure. 
02/14/03   sk      Added rbr support for CELL_DCH -> CELL_FACH transition
                   Added  RRCRBR_WAIT_FOR_CELL_SELECTION_CNF substate
                   Modified rrcrbr_rb_release_info_type to rrcrbr_rb_info_type
                   to make it generic structure.
                   Renamed some of the parameters in rrcrbr_int_data_type
                   Added rrcrbr_other_wfcscnf_substate_event_handler()
                   Added rrcrbr_fach_wfcscnf_substate_event_handler()
                   Added RRCRBR_WAIT_FOR_CELL_SELECTION_CNF case to 
                   rrcrbr_cell_fach_event_handler()
                   No parameters for rrcrbr_send_channel_config_req()
02/14/03   sk      Changed function name process_rbrelease_message to
                   rrcrbr_process_rbrelease_message
02/19/03   vn      Added processing for State Change to Idle when waiting
                   for Cell Update Confirm.
02/10/03   rj      Removed dependnency of Signalling Connection Release 
                   Indication IE to process RB Releasse Info IE.
02/05/03   vn      Merged changes dropped during linting.
           vn      Added new parameter proc_id in rrcllc_set_ordered_config.
02/04/03   ram     Checked in lint changes, updated copyright.
01/17/03   upn     Fixed rbs_to_release flag for Signalling connection release 
                   indication processing
01/09/03   upn     Added functionality for supporting Signalling
                   connection release indication
12/12/02   bu      Fixed a merge problem related to supporting the Signalling
                   connection release indication in PS mode.
12/10/02   vk      Added support to handle Signalling connection release 
                   indication in PS mode
10/24/02   rj      Updated to use rrc_malloc and rrc_free to use dual heaps 
                   in place of mem_malloc and mem_free to avoid memory
                   fragmentation
09/25/02   bu      Changed l1_status to chan_cfg_status under
                   FEATURE CM SUPPORTED.
09/20/02   bu      If reselection is in progress or if cell update has started
                   and register for CU CNF accordingly. On receiving cell update
                   confirm send RB Release failure to UTRAN. If C_RNTI is not valid
                   register for CU CNF. On receiving CU CNF send the RB Release
                   Complete. Call rcllc_get_ordered_config_status() instead of
                   rrcllc_is_ordered_config_set().
08/28/02   vk      Processed Compressed Mode failure from LLC and send a
                   Radio Bearer Release Failure message to UTRAN with error
                   as Invalid Configuration. Changes have been put under
                   FEATURE CM SUPPORTED
08/13/02   upn     Fixed array counter in rrcrbr_send_channel_config_req
06/07/02   upn     Set the message_set field before sending the RABMAS_RAB_RELEASE_IND
                   to RABM task.
06/07/02   upn      fixed cell update initiate problem 
05/13/02   upn     rrcrbr_cell_fach_event_handler
03/13/02   bu      Uncommented the code related to CELL_FACH in
                   rrcrbr_process_channel_config_cnf and rrcrbr_process_rbrelease_message
03/05/02   upn     Added function rrcrbr_build_rbr_complete_message to build
                   radio bearer complete message. 
02/21/02   upn     Code implemented for state CELL_FACH support.
12/07/01   bu      Added MSG_HIGH to report that we are waiting for L2 ACK.
12/04/01   rj      Fixed a compilation problem.
12/04/01   rj      Updated code to be compatible for June release of 25.331 
                   RRC Protocol Specification.
                   Corrected comments for L2 ACK failure. Fixed previous check-in 
                   mismatches.
10/18/01   upn     Radio Link Failure support: Updated rrcrbr_process_l2_ack_ind
                   function.Removed rrc_clear_established_rabs procedure.
11/12/01   upn     Added support to send CN info to NAS.
09/13/01   ram     PS RAB support: When a PS RAB is released, send the
                   RABMAS_RAB_RELEASE_IND primitive to RABM layer.
                   Freed memory of uplink messages when there is a failure to
                   send the message.
09/05/01   ram     Updated with code review comments. Implemented changes required
                   for handling ASN.1 decoding failure when message type is known.
09/03/01   rj      Removed include file rrcasnutil.h since it is no longer 
                   required after moving send_rrc_status_message to SEND CHAIN.
08/30/01   rj      Changed UE_MODE_ACKNOWLEDGED_DATA_PLANE to 
                   UE_MODE_ACKNOWLEDGED_DATA.
07/31/01   ram     Removed initialization of the bit-mask that indicates if
                   integrity check info is present. This is initialized by
                   the send-chain. Merged in code that delays 5 seconds before
                   transmitting on a new configuration. This is conditional
                   and only needed if the base station cannot have both the
                   old and new configurations up at the same time.
07/26/01   ram     Added MSG_HIGHs for significant events.
06/08/01   rj      Modifed function rrcrbr_process_l2_ack_ind to compare 
                   L2 Ack status with SUCCESS enum in stead of RRC_L2ACK_SUCCESS.
                   This was requied since interfaces with RLC are modified.
05/10/01   ram     Created file.

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
#include "rrcasn1util.h"
#include "rabmtask.h"
#include "rex.h"
#include "rrcasn1.h"
#include "rrccmd_v.h"
#include "rrcccm.h"
#include "rrcdata_v.h"
#include "rrcdt.h"
#include "rrccu.h"
#include "rrccui.h"
#include "rrcintif.h"
#include "rrclcm.h"
#include "rrcllc.h"
#include "rrcmmif.h"
#include "rrcrbcommon.h"
#include "rrcrbr.h"
#include "rrcscmgr.h"
#include "rrcsend.h"
#include "rrccspi.h"
#include "rrccsp.h"
#include "rrctmr.h"
#include "rrcsibproc.h"
#include "rrcsmc.h"
#include "rrclog.h"

#include "rrcmnif.h"
#include "rrclbt.h"

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

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
/* We need to introduce a delay before transmitting
on a new configuration if the base-station cannot have
the old link and new link up at the same time. */
#define RRCRBR_TEMP_TIMER_SIG 0x2000
rex_sigs_type rrcrbr_sig;
rex_timer_type rrcrbr_temp_timer;

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
/* Structure to hold information about the RBs to be released.             */
typedef struct
{
  boolean is_valid;     /* indicates if there are RBs that need to be 
                           released/affected. If this is FALSE, the two
                           items below are not used                       */
  uint32 num_rbs;       /* Number of RBs released by the message          */
  rrc_RB_Identity rb_id[MAX_RB]; /* Hold the ids of the RBs that are
                           released/affected by the RBR message           */
}rrcrbr_rb_info_type;

typedef struct {
  rrc_RB_Identity rb_id; /* Hold the ids of the RBs */
  rrc_rb_cfg_e_type rb_config;
  uint16 ul_cfg_mask;
  uint16 dl_cfg_mask;
}rrcrbr_rb_id_action;

typedef struct {
  boolean is_valid;
  uint32 num_rbs;       /* Number of RBs tobe configured by the message          */
  rrcrbr_rb_id_action rb_info[MAX_RB];
}rrcrbr_rb_reconfig_info_type;

/* Structure to store all internal data related to RBR procedure           */
typedef struct
{
  rrcrb_substate_e_type curr_substate;  /* Store the current substate     */
  rrcrb_transaction_id_type tr_ids;      /* Transaction ids for RBR msg    */
  rrc_state_e_type state_indicated_by_msg; /* RRC State given by RBR msg   */
  rrcrb_msg_failure_cause_type status;   /* Status of message validation   */
  rrcrbr_rb_info_type rbs_tobe_released; /* Information on the RBs to be
                                            released                       */
  rrcrbr_rb_reconfig_info_type rbs_tobe_reconfigured; /* Information on the RBs
                                             to be reconfigured              */
  rrcrbr_rb_info_type rbs_tobe_affected; /* Information on the RBs to be
                                            affected                       */
  rrcrb_rab_info_per_msg_type rabs;      /* Hold info about each rab in one
                                         RBR message till they're processed.
                                         This is used when RABs need to be
                                         reconfigured.                     */
  boolean need_to_release_cn_domain;     /* TRUE if a signalling connection
                                         for a CN domain is to be released */
  rrc_cn_domain_identity_e_type cn_to_release;   /* The domain to be released      */

  boolean    new_crnti_valid;            /* Indicates if a valid C-RNTI was recd 
                                          * in the message transitioning from CELL_DCH
                                          * to CELL_FACH
                                          */
  uint16     new_crnti;
  boolean    directed_cell_info; /* Indicates if a valid C-RNTI was recd */
  rrc_int_u_rnti_type          new_urnti;
  boolean                      new_urnti_valid;          /* Indicates if a valid U-RNTI was recd */ 
  boolean                      rbr_for_srns_relocation;  /* Indicates SRNS reloc involved with 
                                                            this msg                             */

  boolean                       hho_ciph_status;
  boolean                       cn_info_saved;           /*flag to indicate that CN info is saved */
  mm_cmd_type *                 mm_cmd_ptr;              /*pointer to hold mm_cmd with cn info */

  boolean                       trans_to_dch_ciph_config_needed;
  /* Indicates if Fre-redirection is to be triggered in FACH->PCH case */
  boolean initiate_cell_selection_trans_from_fach;

  boolean high_mobility_ind;

}rrcrbr_int_data_type;

LOCAL rrcrbr_int_data_type rbr;

boolean rrcrbr_send_initiate_cu_flag = FALSE;
boolean rrcrbr_delete_crnti          = FALSE;
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

FUNCTION   rrcrbr_mark_cn_doamin_to_be_closed

DESCRIPTION

  This function processes the received SCRI in the RB release message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbr_mark_cn_doamin_to_be_closed
(
  rrc_CN_DomainIdentity signallingConnectionRelIndication
)
{
  MSG_HIGH("Domain CS status %d (0:NOT open, 1:Open)/PS status %d (0:NOT open, 1:Open) for SCRI:%d(0- CS domain,1-PS domain)",rrcdt_is_cs_domain_open(),rrcdt_is_ps_domain_open(),signallingConnectionRelIndication);

  if (rrc_CN_DomainIdentity_cs_domain == signallingConnectionRelIndication)
  {
      if(rrcdt_is_cs_domain_open())
      {
        /* Need to release a CN domain */
        rbr.need_to_release_cn_domain = TRUE; 
      }
      /* Copy the domain that is to be released */
      rbr.cn_to_release = RRC_CS_DOMAIN_CN_ID;
  }
  else
  {
      if(rrcdt_is_ps_domain_open())
      {
        /* Need to release a CN domain */
        rbr.need_to_release_cn_domain = TRUE; 
      }
      /* Copy the domain that is to be released */
      rbr.cn_to_release = RRC_PS_DOMAIN_CN_ID;
  }
}
/*===========================================================================

FUNCTION   RRCRBR_INDICATE_LAYER1_IF_CONFIG_MSG_RELEASE_VOICE_RAB

DESCRIPTION

  If radio bearer release message indicates UE to release voice RAB then this
  function will indicate layer1 about the same by calling some feature specific
  call back function provided by layer1.
  If there is no change in the voice RAB status then this function will not do anything.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbr_indicate_layer1_if_config_msg_release_voice_rab(void)
{
  uint32 rb_index, count; /* counter */
  uint32 num_rb, temp_num_rb;
  uint32 rb_id_list[MAX_RB_PER_RAB];

  if ((TRUE == rbr.rbs_tobe_released.is_valid) && 
       (TRUE == rrc_get_rb_id_list_associated_with_voice_rab(&num_rb, rb_id_list)))
  {
    temp_num_rb = num_rb;
    for (rb_index=0; rb_index<rbr.rbs_tobe_released.num_rbs; rb_index++)
    {
      for (count=0; count<num_rb; count++)
      {
        if (rb_id_list[count] == rbr.rbs_tobe_released.rb_id[rb_index])
        {
          --temp_num_rb;
          break;
        }
      }
    }

    /* if num_rb is 0 at this point then it means voice RAB is going to be released */
    if (0 == temp_num_rb)
    {
      rrc_indicate_layer1_next_config_will_release_voice_rab();
    }
  }
}
/*===========================================================================

FUNCTION   RRCRBRC_IS_SRNS_IN_PROGRESS()

DESCRIPTION
 This function returns if SRNS is in progress with current reconfig procedure


        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcrbr_is_srns_in_progress
(
  void 
)
{
  return rbr.rbr_for_srns_relocation;
}
/*===========================================================================

FUNCTION   RRCRBR_GET_NEW_CRNTI_VALID()

DESCRIPTION
 This function returns if reconfig msg has valid CRNTI


        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcrbr_get_new_crnti_valid
(
  void
)
{
  return rbr.new_crnti_valid;
}
 /*===========================================================================

FUNCTION   RRCRBR_GET_CURRENT_SUB_STATE()

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
rrcrb_substate_e_type rrcrbr_get_current_sub_state( )
{
  return rbr.curr_substate;
}
  /*===========================================================================
 
 FUNCTION   rrcrbr_get_dest_state()
 
 DESCRIPTION
Returns the state indicated by the message
 DEPENDENCIES
 
   None.

 RETURN VALUE
 
   rrc_state_e_type
 
 SIDE EFFECTS
 
   None.

 ===========================================================================*/
 rrc_state_e_type rrcrbr_get_dest_state(void)
 {
   return rbr.state_indicated_by_msg;
 }
/*===========================================================================

FUNCTION   rrcrbrc_is_proc_successful()

DESCRIPTION
 This function is used to return failure status
DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rrcrbr_is_proc_successful( )
{
  if(rbr.status.failure_status == RRCRB_MSG_VALID)
    return TRUE;
  return FALSE;
}

/*===========================================================================

FUNCTION   rrcrbr_update_failure_status()

DESCRIPTION
  Updates the failure status
DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
void rrcrbr_update_failure_status(rrcrb_msg_status_e_type status )
{
  rbr.status.failure_status = status;
  WRRC_MSG1_HIGH("Updated Failure status to %d",status);
  return;
}

/*===========================================================================

FUNCTION rrcrbr_append_start_list_to_rbr_complete
DESCRIPTION
  This function appends start list to rbr message for SRNS relocation and
  also when TM bearers are present during non-dch to dch rrc-state
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
static void rrcrbr_append_start_list_to_rbr_complete
(
rrc_UL_DCCH_Message *msg_ptr               /* Pointer to Uplink DCCH Msg */
)
{
  /*start list is included in ul_counter_synchronization_info. So enable this IE in the
  bitmask */
  RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.radioBearerReleaseComplete,
  rrc_RadioBearerReleaseComplete,ul_CounterSynchronisationInfo);

  RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.radioBearerReleaseComplete.
  	ul_CounterSynchronisationInfo);
 
  (void)rrcsmc_append_start_list(&msg_ptr->message.u.radioBearerReleaseComplete.ul_CounterSynchronisationInfo.startList,
                           RRCSMC_EXTRACT_AND_APPEND_START);

}
/*===========================================================================
FUNCTION   SEND_RBR_FAILURE_MSG

DESCRIPTION

  This function sends the Radio Bearer Release Failure message
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
static void send_rbr_failure_message
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
  rlc_lc_id_type ul_lc_id;      /* Logical Channel Id for the message  */ 
  rrcsend_status_e_type status; /* Status from send chain module       */

  /* Allocate memory for this message. Note that this buffer is released
  by the SEND CHAIN */
  ptr = (rrc_UL_DCCH_Message *)rrc_malloc(
                                         sizeof(rrc_UL_DCCH_Message));
    WRRC_MSG2_HIGH("rbr subsstate(%d) Send failure with %d cause:   [0-VALID|1-UNSUPPORTED_CONFIG|2-PHY_CHAN_FAILURE|3-SIMULTANEOUS_RECONFIG|4-PROTOCOL_ERROR|5-INVALID_CONFIG|6-INCOMPLETE_CONFIG|7-CELL_UPDATE_OCCURED]", 
   rbr.curr_substate, rbr.status.failure_status);

    /* we need to revert cipher config db, if cipher config db has been updated with step-1 HO. 
     This revert takes care of the scenario of OTA failure in the gap between OC is 
     set and cphy is sent. This cannot happen in a normal case. This is for extra protection */
  rrcsmc_revert_to_old_config_for_failure_message(FALSE);

  ptr->message.t = T_rrc_UL_DCCH_MessageType_radioBearerReleaseFailure; /* RBR Failure msg */

  /* Set the bit-mask - neither rrc_potentiallySuccesfulBearerList_present
  nor rrc_RadioBearerReconfigurationFailure_nonCriticalExtensions_present
  are included */
  RRC_RESET_MSG_IE_PRESENT(ptr->message.u.radioBearerReleaseFailure);

  /* Transaction Id - First check the rejected_transacion id. If
  it's not invalid, this message is being rejected because there
  is already another message accepted but not processed.
  If the rejected_transaction id is invalid, we are rejecting the
  accepted message, so use the accepted transaction id.

  NOTE: Don't change the order, it's important to check rejected
  transaction first. */
  if ( rbr.tr_ids.rejected_transaction != RRCRB_INVALID_TRANSACTION_ID )
  {
    ptr->message.u.radioBearerReleaseFailure.rrc_TransactionIdentifier =
    rbr.tr_ids.rejected_transaction;
    /* We no longer need to keep this id. Make rejected_transaction id
    invalid again */
    rbr.tr_ids.rejected_transaction = RRCRB_INVALID_TRANSACTION_ID;
  }
  else if ( rbr.tr_ids.accepted_transaction != RRCRB_INVALID_TRANSACTION_ID )
  {
    ptr->message.u.radioBearerReleaseFailure.rrc_TransactionIdentifier =
    rbr.tr_ids.accepted_transaction;
    /* We no longer need to keep this id. Make rejected_transaction id
    invalid again */
    rbr.tr_ids.accepted_transaction = RRCRB_INVALID_TRANSACTION_ID;
  }
  else
  {
    /* Don't know the transaction id for this message.( one reason
    for this could be that the message had a critical
    extenstion). Since it's unknown, we should set it to zero. */
    ptr->message.u.radioBearerReleaseFailure.rrc_TransactionIdentifier = 0;
    WRRC_MSG0_HIGH("Transaction id invalid for RBR Failure message!");
  }

  switch ( failure_cause )
  {
  case RRCRB_MSG_UNSUPPORTED_CONFIG:
    ptr->message.u.radioBearerReleaseFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_configurationUnsupported;
    break;

  case RRCRB_PHY_CHAN_FAILURE:
    ptr->message.u.radioBearerReleaseFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_physicalChannelFailure;
    break;

  case RRCRB_SIMULTANEOUS_RECONFIG:
    ptr->message.u.radioBearerReleaseFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_incompatibleSimultaneousReconfiguration;
    break;

  case RRCRB_MSG_PROTOCOL_ERROR:
    ptr->message.u.radioBearerReleaseFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_protocolError;

  	ptr->message.u.radioBearerReleaseFailure.failureCause.u.protocolError =
	  	rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation);

    /* When there is a Protocol Error, we also have to indicate
    the protocol error cause */
    ptr->message.u.radioBearerReleaseFailure.failureCause.u.protocolError->diagnosticsType.t =
      T_rrc_ProtocolErrorInformation_diagnosticsType_type1;
    
	  ptr->message.u.radioBearerReleaseFailure.failureCause.u.protocolError->
		  diagnosticsType.u.type1 = rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation_type1);

    /* Copy the given protocol error cause */
    ptr->message.u.radioBearerReleaseFailure.failureCause.u.protocolError->
      diagnosticsType.u.type1->protocolErrorCause = prot_err_cause;
    break;

  case RRCRB_MSG_INVALID_CONFIG:
    ptr->message.u.radioBearerReleaseFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_invalidConfiguration;
    break;

  case RRCRB_MSG_INCOMPLETE_CONFIG:
    ptr->message.u.radioBearerReleaseFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_configurationIncomplete;
    break;
  
  case RRCRB_CELL_UPDATE_OCCURED:
     ptr->message.u.radioBearerReleaseFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_cellUpdateOccurred;
    break;

  default:
    WRRC_MSG0_HIGH("Sending RBR Unknown Failure message");
    ptr->message.u.radioBearerReleaseFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_configurationUnsupported;
    break;
  }
#ifdef FEATURE_QSH_EVENT_METRIC
  if(rrc_qsh_dl_committed == FALSE)
  {
    failure_ota.ul_ota_type = RRC_OTA_UL_FAILURE;
    failure_ota.msg_id = WRRC_UL_DCCH_MESSAGETYPE_RADIOBEARERRELEASEFAILURE;
    if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status())
    {
      failure_ota.ul_fail_ota.ul_ota_parms.ul_integrity_info_present = 1;
    }
		
    failure_ota.ul_fail_ota.ul_ota_parms.failure_cause = ptr->message.u.radioBearerReleaseFailure.failureCause.t;
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
			
      ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.ul_ota_parms.failure_cause= ptr->message.u.radioBearerReleaseFailure.failureCause.t;   
      /* Commit data will be called in send ul sdu */
    }
  }
#endif
  /*log the protocol error*/
  rrc_log_protocol_error( (uint8) (rrc_get_state()),
                          (uint8) RRC_PROCEDURE_RBR,
                          (uint8) ptr->message.u.radioBearerReleaseFailure.failureCause.t,
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
    WRRC_MSG1_HIGH("Sending RBR Failure msg to send-chain Cause:%d",
             ptr->message.u.radioBearerReleaseFailure.failureCause.t);

    /* Need to wait for L2 ack for all states because RRC needs to trigger include reconfig status indicator
    if RLC acknowledgement is not yet received.
    As per 8.3.1.3
    1> if the IE "Cell update cause" is set to "radio link failure" and the UE has submitted a reconfiguration response
    message to RLC and the UE has not received the RLC acknowledgement for the response message:
    2> include the IE "Reconfiguration Status Indicator" and set it to TRUE.*/
    if( rbr.status.failure_status == RRCRB_PHY_CHAN_FAILURE)
    {
      rrc_update_reconfig_waiting_for_l2ack_dch(RRC_PROCEDURE_RBR);
      status = rrcsend_send_ul_sdu(RRC_PROCEDURE_RBR,  /* Procedure is RBR    */
                                   (void *)ptr,        /* Message pointer     */
                                   ul_lc_id,           /* RLC id              */
                                   RRCSEND_L2ACK_REQUIRED  /* ACK is required */
                                  );
    }
    else
    {
      status = rrcsend_send_ul_sdu(RRC_PROCEDURE_RBR,  /* Procedure is RBR    */
                                   (void *)ptr,        /* Message pointer     */
                                   ul_lc_id,           /* RLC id              */
                                   RRCSEND_L2ACK_NOT_REQUIRED  /* No need for ACK */
                                  );
    }

    if ( status != RRCSEND_SUCCESS )
    {
      WRRC_MSG1_ERROR("Send chain failed for RBR failure msg: error cause %d", status);
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
}/* end function send_rbr_failure_message */

/*===========================================================================

FUNCTION   RRCRBR_BUILD_RBR_COMPLETE_MSG

DESCRIPTION

  This function builds the Radio Bearer Release Complete message to be sent
  on Uplink DCCH. 
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/     
void rrcrbr_build_rbr_complete_message
(
  rrc_RRC_TransactionIdentifier transaction_id,  /* Transaction Identifier to be sent in Uplink */
  rrc_UL_DCCH_Message *ptr,                      /* Uplink DCCH message pointer */
  boolean              srns_reloc,               /* Indicates whether SRNS reloc is involved */ 
  boolean              cipher_update_required    /* Indicates whether TM ciphering is present */
)
{
  /* Identify as RBR Complete msg */
  ptr->message.t = T_rrc_UL_DCCH_MessageType_radioBearerReleaseComplete; 

  /* Fill up the message contents. The bit_mask is set to 0 since none
  of the optional IE's are included for now. */
  RRC_RESET_MSG_IE_PRESENT(ptr->message.u.radioBearerReleaseComplete);


  if (srns_reloc == TRUE)
  {
    rrcrbr_append_start_list_to_rbr_complete(ptr);
  }
  if((cipher_update_required == TRUE) && (TRUE == rrcsmc_tm_ciph_active()))
  {
    if(! (RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.radioBearerReleaseComplete,
           rrc_RadioBearerReleaseComplete,ul_CounterSynchronisationInfo)))
    {
      rrcrbr_append_start_list_to_rbr_complete(ptr);
    }
    //Set the bitmask for count_c_act_time and copy the same..
   RRC_MSG_COMMON_SET_BITMASK_IE(ptr->message.u.radioBearerReleaseComplete,
      rrc_RadioBearerReleaseComplete,count_C_ActivationTime);
	
    ptr->message.u.radioBearerReleaseComplete.count_C_ActivationTime =
      rrcsmc_step_2_cipher_config_for_hho();
  }

  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
  {
  #ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
    if(rrcsib_is_dmcr_ie_needed() == TRUE)
    {
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.radioBearerReleaseComplete, laterNonCriticalExtensions);
 
      RRC_RESET_MSG_IE_PRESENT(ptr->message.u.radioBearerReleaseComplete.laterNonCriticalExtensions);
      
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.radioBearerReleaseComplete.laterNonCriticalExtensions, v770NonCriticalExtensions);
      
      RRC_RESET_MSG_IE_PRESENT(ptr->message.u.radioBearerReleaseComplete.laterNonCriticalExtensions.v770NonCriticalExtensions);
 
      RRC_RESET_MSG_IE_PRESENT(ptr->message.u.radioBearerReleaseComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        radioBearerReleaseComplete_v770ext);
      
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.radioBearerReleaseComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        radioBearerReleaseComplete_v770ext, deferredMeasurementControlReading);
      
      ptr->message.u.radioBearerReleaseComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        radioBearerReleaseComplete_v770ext.deferredMeasurementControlReading = rrc_RadioBearerReleaseComplete_v770ext_IEs_deferredMeasurementControlReading_true;
    }
#endif
  }

  ptr->message.u.radioBearerReleaseComplete.rrc_TransactionIdentifier =
  transaction_id;
  /* We no longer need to keep this id. Make rejected_transaction id
  invalid again */
  //rbr.tr_ids.accepted_transaction = RRCRB_INVALID_TRANSACTION_ID;

}/* end function rrcrbr_build_rbr_complete_message */


/*===========================================================================

FUNCTION   SEND_RBR_COMPLETE_MSG

DESCRIPTION

  This function sends the Radio Bearer Release Complete message
  on Uplink DCCH. 
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/     
static void send_rbr_complete_message( boolean proc_wait_for_l2_ack )
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
     WRRC_MSG0_HIGH("Waiting for L2 ACK for RBR Complete msg");
  }
  ptr->message.t = T_rrc_UL_DCCH_MessageType_radioBearerReleaseComplete; /* RBR Complete msg */

  /* Fill up the message contents. The bit_mask is set to 0 since none
  of the optional IE's are included for now. */
  RRC_RESET_MSG_IE_PRESENT(ptr->message.u.radioBearerReleaseComplete); 

  if (rbr.rbr_for_srns_relocation == TRUE)
  {
    rrcrbr_append_start_list_to_rbr_complete(ptr);
  }
  else
  {
    if (rrcllc_get_rlc_size_change_status() 
      && ((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH || 
             rbr.state_indicated_by_msg == RRC_STATE_URA_PCH)
          || (rrc_get_rlc_size_change_status_for_procedure_id(RRC_PROCEDURE_RBR) == TRUE)))
    {
      // Check if next state is CELL+PCH, and OC API returns RLC size change, then include counter sync
      if (rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH || 
          rbr.state_indicated_by_msg == RRC_STATE_URA_PCH)
      {
        /* Ask SMC to do start value calculation for PS domain */
        WRRC_MSG0_HIGH("RLCSIZEINDICATED Invoking SMC extract func");
        (void)rrcsmc_extract_return_hfn_per_domain_and_update_start_flag(RRC_PS_DOMAIN_CN_ID);
      }
      else if (rrc_get_rlc_size_change_status_for_procedure_id(RRC_PROCEDURE_RBR) == TRUE)
      {
        rrc_set_rlc_size_change_status(RRC_PROCEDURE_NONE, 
                                       FALSE, RRC_INVALID_RB_ID ,
                                       RLC_RE_ESTABLISH_NONE);
      }
      /*start list is included in ul_counter_synchronization_info. So enable this IE in the
      bitmask */
      RRC_MSG_COMMON_SET_BITMASK_IE(ptr->message.u.radioBearerReleaseComplete,
        rrc_RadioBearerReleaseComplete,ul_CounterSynchronisationInfo);

	 RRC_RESET_MSG_IE_PRESENT(ptr->message.u.radioBearerReleaseComplete.ul_CounterSynchronisationInfo);

      
     /* call SMC API to append the start list */
     (void)rrcsmc_append_start_list(&ptr->message.u.radioBearerReleaseComplete.ul_CounterSynchronisationInfo.startList,
                           RRCSMC_APPEND_START);

    }
  }

  //if this is case of IFHHO and not SRNS relocation(i.e. ul counter sync is not present)
  //then also send START_List and set count c act time and configure smc for Step_2

  if ( ((rbr.hho_ciph_status == TRUE) || (rbr.trans_to_dch_ciph_config_needed == TRUE)) &&
       (TRUE == rrcsmc_tm_ciph_active()))
  {
    if(! (RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.radioBearerReleaseComplete,
           rrc_RadioBearerReleaseComplete,ul_CounterSynchronisationInfo)))
    {
      rrcrbr_append_start_list_to_rbr_complete(ptr);
    }
  
    //Set the bitmask for count_c_act_time and copy the same..
    RRC_MSG_COMMON_SET_BITMASK_IE(ptr->message.u.radioBearerReleaseComplete,
      rrc_RadioBearerReleaseComplete,count_C_ActivationTime);
    
    ptr->message.u.radioBearerReleaseComplete.count_C_ActivationTime =
      rrcsmc_step_2_cipher_config_for_hho();
  
    //Send step 2 config for HHO.. only one of these flags will be active, reset both of them
    //here
    if(rbr.hho_ciph_status)
    {
      rbr.hho_ciph_status =  FALSE;      
    }
    else if(rbr.trans_to_dch_ciph_config_needed)
    {
      rbr.trans_to_dch_ciph_config_needed = FALSE;      
    }
  }

  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
  {
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
    if(rrcsib_is_dmcr_ie_needed() == TRUE)
    {
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.radioBearerReleaseComplete, laterNonCriticalExtensions);

      RRC_RESET_MSG_IE_PRESENT(ptr->message.u.radioBearerReleaseComplete.laterNonCriticalExtensions);
       
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.radioBearerReleaseComplete.laterNonCriticalExtensions, v770NonCriticalExtensions);
       
      RRC_RESET_MSG_IE_PRESENT(ptr->message.u.radioBearerReleaseComplete.laterNonCriticalExtensions.v770NonCriticalExtensions);

      RRC_RESET_MSG_IE_PRESENT(ptr->message.u.radioBearerReleaseComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        radioBearerReleaseComplete_v770ext);
       
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.radioBearerReleaseComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        radioBearerReleaseComplete_v770ext, deferredMeasurementControlReading);
       
      ptr->message.u.radioBearerReleaseComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        radioBearerReleaseComplete_v770ext.deferredMeasurementControlReading = rrc_RadioBearerReleaseComplete_v770ext_IEs_deferredMeasurementControlReading_true;
    }
#endif
  }

  /* Transaction Id - use the accepted transaction id if known. Else
  use zero. Since this is a RBR Complete message, we don't check 
  rejected transaction ids. */
  if ( rbr.tr_ids.accepted_transaction != RRCRB_INVALID_TRANSACTION_ID )
  {
    ptr->message.u.radioBearerReleaseComplete.rrc_TransactionIdentifier =
    rbr.tr_ids.accepted_transaction;
    /* We no longer need to keep this id. Make rejected_transaction id
    invalid again unless proc is waiting for L2ACK*/
    if (!proc_wait_for_l2_ack)
    {
      rbr.tr_ids.accepted_transaction = RRCRB_INVALID_TRANSACTION_ID;
    }
  }
  else
  {
    /* Don't know the transaction id for this message.( one reason
    for this could be that the message had a critical
    extenstion). Since it's unknown, we should set it to zero. */
    ptr->message.u.radioBearerReleaseComplete.rrc_TransactionIdentifier = 0;
    WRRC_MSG0_HIGH("Transaction id invalid for RBR Complete message!");
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
    (void) rex_clr_sigs(rex_self(), RRCRBR_TEMP_TIMER_SIG);
    (void) rex_set_timer(&rrcrbr_temp_timer, 5000);   /* 5 seconds */

    rrcrbr_sig = rex_wait(RRCRBR_TEMP_TIMER_SIG);
    MSG_LOW("RRCRBR signal:%d", rrcrbr_sig, 0, 0);
    (void) rex_clr_sigs(rex_self(), RRCRBR_TEMP_TIMER_SIG);
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
      failure_ota.msg_id = WRRC_UL_DCCH_MESSAGETYPE_RADIOBEARERRELEASECOMPLETE;

      if (RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.radioBearerReleaseComplete,
      rrc_RadioBearerReleaseComplete,ul_CounterSynchronisationInfo))
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
      if (RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.radioBearerReleaseComplete,
      rrc_RadioBearerReleaseComplete,ul_CounterSynchronisationInfo))
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
    WRRC_MSG0_HIGH("Sending RBR Complete message to send-chain");
    if (proc_wait_for_l2_ack) 
    {
      status = rrcsend_send_ul_sdu(RRC_PROCEDURE_RBR,  /* Procedure is RBR    */
                                   (void *)ptr,        /* Message pointer     */
                                   ul_lc_id,           /* RLC id              */
                                   RRCSEND_L2ACK_REQUIRED  /* ACK is required */
                                   );
    }
    else
    {
      status = rrcsend_send_ul_sdu(RRC_PROCEDURE_RBR,  /* Procedure is RBR    */
                                   (void *)ptr,        /* Message pointer     */
                                   ul_lc_id,           /* RLC id              */
                                   RRCSEND_L2ACK_NOT_REQUIRED  /* ACK is required */
                                   );
    }

    if ( status != RRCSEND_SUCCESS )
    {
      WRRC_MSG1_ERROR("Send chain failed for RBR complete msg: error cause %d", status);
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

/* 
   Skip sending CLOSE_SESS_IND to NAS for PCH transitions and SRNS relocation cases
   For transition to PCH state, CLOSE_SESS_IND will be sent in rrcrbr_process_successful_proc_completion() so that
   RRC_SYNC_IND is always sent ahead of CLOSE_SESS_IND to NAS
   For SRNS relocation, CLOSE_SESS_IND will be sent after L2 ack is received 
*/
  
  if((rbr.state_indicated_by_msg != RRC_STATE_CELL_PCH)
      && (rbr.state_indicated_by_msg != RRC_STATE_URA_PCH)
      && (rbr.rbr_for_srns_relocation != TRUE))
  {
    if (rbr.need_to_release_cn_domain == TRUE)
    {
      if(rbr.cn_to_release == RRC_CS_DOMAIN_CN_ID)
      {      
        (void)rrcscr_release_signalling_connection(RRC_CS_DOMAIN_CN_ID);
      }
      else
      {
        (void)rrcscr_release_signalling_connection(RRC_PS_DOMAIN_CN_ID);      
      }
      rbr.need_to_release_cn_domain = FALSE;
    }
  }
}/* end function send_rbr_complete_message */


/*===========================================================================

FUNCTION rrcrbr_is_pcpich_info_present

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
static boolean  rrcrbr_is_pcpich_info_present
(
  rrc_RadioBearerRelease  *msg_ptr,
  rrc_msg_ver_enum_type   msg_version
)
{
  boolean result = FALSE;
  uint16 rrcrbr_psc = 512;

  
#ifdef FEATURE_WCDMA_REL10
  #error code not present
#endif /*FEATURE_WCDMA_REL10*/

#ifdef FEATURE_WCDMA_REL9

  if(msg_version == MSG_REL9)
  {
    rrc_RadioBearerRelease_r9_IEs *r9_rb_rel_msg_ptr  =
        &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.radioBearerRelease_r9;
    
    if(RRCRB_R9_MSG_IE_PRESENT(r9_rb_rel_msg_ptr,dl_InformationPerRL_List))      
    {
      if(r9_rb_rel_msg_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
        T_rrc_DL_InformationPerRL_r8_modeSpecificInfo_fdd)
      { 
        rrcrbr_psc = 
          r9_rb_rel_msg_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.
          u.fdd->primaryCPICH_Info.primaryScramblingCode;
        result = TRUE;
      }
    }
  }
  else
#endif /*FEATURE_WCDMA_REL9*/

  if(msg_version == MSG_REL8)
  {
    rrc_RadioBearerRelease_r8_IEs *r8_rb_rel_msg_ptr  =
        &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.radioBearerRelease_r8;
    
    if(RRCRB_R8_MSG_IE_PRESENT(r8_rb_rel_msg_ptr,rrc_RadioBearerRelease_r8_IEs,dl_InformationPerRL_List))      
    {
      if(r8_rb_rel_msg_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
        T_rrc_DL_InformationPerRL_r8_modeSpecificInfo_fdd)
      { 
        rrcrbr_psc = r8_rb_rel_msg_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.
          u.fdd->primaryCPICH_Info.primaryScramblingCode;
        result = TRUE;
      }
    }
  }
  else
  if(msg_version == MSG_REL7)
  {
    rrc_RadioBearerRelease_r7_IEs *r7_rb_rel_msg_ptr  =
        &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7.radioBearerRelease_r7;
    
    if(RRCRBR_R7_MSG_IE_PRESENT(r7_rb_rel_msg_ptr,rrc_RadioBearerRelease_r7_IEs,dl_InformationPerRL_List))
      
    {
      if(r7_rb_rel_msg_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
        T_rrc_DL_InformationPerRL_r7_modeSpecificInfo_fdd)
      { 
        rrcrbr_psc =
          r7_rb_rel_msg_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.
          u.fdd->primaryCPICH_Info.primaryScramblingCode;
        result = TRUE;
      }
    }
  }
  else

  if(msg_version == MSG_REL6)
  {
    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.
		criticalExtensions.u.criticalExtensions.u.r6.radioBearerRelease_r6,
      rrc_RadioBearerRelease_r6_IEs,dl_InformationPerRL_List))
    {
      if(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
        u.r6.radioBearerRelease_r6.dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
        T_rrc_DL_InformationPerRL_r6_modeSpecificInfo_fdd)
      { 
        rrcrbr_psc =
          msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.
          radioBearerRelease_r6.dl_InformationPerRL_List.elem[0].modeSpecificInfo.
          u.fdd->primaryCPICH_Info.primaryScramblingCode;
        result = TRUE;
      }
    }
  }
  else
  if(msg_version == MSG_REL5)
  {
    if (msg_ptr->t == T_rrc_RadioBearerRelease_later_than_r3)
    {
      if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.radioBearerRelease_r5,
        rrc_RadioBearerRelease_r5_IEs,dl_InformationPerRL_List))
      {
        if(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.
          radioBearerRelease_r5.dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
          T_rrc_DL_InformationPerRL_r5_modeSpecificInfo_fdd)
        { 
          rrcrbr_psc =
          msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.
            radioBearerRelease_r5.dl_InformationPerRL_List.elem[0].modeSpecificInfo.
            u.fdd->primaryCPICH_Info.primaryScramblingCode;
          result = TRUE;
        }
      }
    }
  }
  else
  {
    /*this function is used to find whether directed cell info is present or not during Cell_DCH to Cell_FACH 
    state transition. It is safe to assume that the rl list will have only one element.*/
    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3,
     rrc_RadioBearerRelease_r3_IEs,dl_InformationPerRL_List))
    {
      if(msg_ptr->u.r3.radioBearerRelease_r3.dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
      T_rrc_DL_InformationPerRL_modeSpecificInfo_fdd)
      { 
        rrcrbr_psc =
              msg_ptr->u.r3.radioBearerRelease_r3.dl_InformationPerRL_List.
              elem[0].modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
        result = TRUE;
      }
    }
  }
  MSG_HIGH("RRCRBR Received PSC %d result:%d (0-Not present, 1-Present)for msg_version:%d(0-MSG_REL99,1-MSG_REL5,2-MSG_REL6,3-MSG_REL7,4-MSG_REL8,5-MSG_REL9,6-MSG_REL10,other-MSG_INVALID)",rrcrbr_psc,result,msg_version);
  return result;
  
}  /* rrcrbr_is_pcpich_info_present */


/*===========================================================================

FUNCTION   RRCRBR_VALIDATE_SRNS_RELOCATION_INFO()

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
static boolean rrcrbr_validate_srns_relocation_info
(
rrc_RadioBearerRelease *msg_ptr      /* Pointer to the RB Setup message */
,rrc_msg_ver_enum_type msg_version
)
{
   
  if (msg_version == MSG_REL5)
  {
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.
		u.criticalExtensions.u.r5.radioBearerRelease_r5,
      rrc_RadioBearerRelease_r5_IEs,dl_CounterSynchronisationInfo))
    {
      return TRUE;
    } 
    else
    {
      return FALSE;
    }
  }
  if (msg_version == MSG_REL6)
  {
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u. 
         criticalExtensions.u.r6.radioBearerRelease_r6,
         rrc_RadioBearerRelease_r6_IEs,dl_CounterSynchronisationInfo)) 
    { 
      return TRUE; 
    } 
    else
    {
      return FALSE;
    }
  }

  if (msg_version == MSG_REL7)
  {
    if (RRCRBR_R7_MSG_IE_PRESENT((&msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u. 
         criticalExtensions.u.r7.radioBearerRelease_r7),rrc_RadioBearerRelease_r7_IEs,
         dl_CounterSynchronisationInfo)) 
    { 
      return TRUE; 
    } 
    else
    {
      return FALSE;
    }
  }
  if (msg_version == MSG_REL8)
  {
    if(RRCRB_R8_MSG_IE_PRESENT((&msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
        criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.radioBearerRelease_r8),rrc_RadioBearerRelease_r8_IEs,
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
if (msg_version == MSG_REL9)
{
  if(RRCRB_R9_MSG_IE_PRESENT((&msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
      criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.radioBearerRelease_r9),
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

  if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3,
    rrc_RadioBearerRelease_r3_IEs,dl_CounterSynchronisationInfo))
  {
     return TRUE;
  }
  else 
  {
    return FALSE;
  }
}

 /*===========================================================================
FUNCTION   rrcrbr_configure_rab_change()

DESCRIPTION
 This function re-configures RABs and other lower layer parameters for VT -> Voice 
 and Voice -> VT fallback for RB-Release OTA msg. Also, if SCUDIF is not happening, then
 this function checks/configures for vocoder/rate change for Voice RAB. 
        
DEPENDENCIES
  None.
 
RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None.
===========================================================================*/
static boolean rrcrbr_configure_rab_change
(
  uint8 rab_idx
)
{
#ifdef FEATURE_SCUDIF_ENHANCEMENT
  #error code not present
#endif/* FEATURE_SCUDIF_ENHANCEMENT*/
  mm_cmd_type *mm_ptr = NULL;   /* To send the MM_SYNC_IND command */

  if(rab_idx >= RRCRB_MAX_RAB_PER_MSG)
  {
    WRRC_MSG1_ERROR("Invalid RAB idx passed %d",rab_idx);
    return FALSE;
  }
#ifdef FEATURE_SCUDIF_ENHANCEMENT
  #error code not present
#endif/* FEATURE_SCUDIF_ENHANCEMENT*/
  {

    if(rbr.rabs.rabs_in_msg[rab_idx].rab_type == RRCRB_CS_VOICE_RAB)
    {
     
      rrcrb_update_amr_cctrch_id_for_reconfig(
#ifdef FEATURE_VOC_AMR_WB
      rbr.rabs.rabs_in_msg[rab_idx].nas_Synchronisation_Indicator
#endif
      );

    }
    mm_ptr = mm_rrc_get_cmd_buf(RRC_SYNC_IND);

    mm_ptr->cmd.rrc_sync_ind.cn_domain_id = RRC_CS_DOMAIN_CN_ID;
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
    mm_ptr->cmd.rrc_sync_ind.rab_info.rab_id =  rbr.rabs.rabs_in_msg[rab_idx].rab_id;
  
    if(rbr.rabs.rabs_in_msg[rab_idx].rab_type == RRCRB_CS_VOICE_RAB)
    {
      mm_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type = RRCMN_CS_VOICE_CALL;
    }
    else if(rbr.rabs.rabs_in_msg[rab_idx].rab_type == RRCRB_CS_DATA_RAB)
    {
      mm_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type = RRCMN_CS_DATA_CALL;        
    }
    else
    {
      WRRC_MSG0_ERROR("Something wrong with RAB Type");      
      mm_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type = RRCMN_INVALID_RAB_ID;
    }
#ifdef FEATURE_VOC_AMR_WB
    mm_ptr->cmd.rrc_sync_ind.rab_info.codec = RRC_CODEC_NONE;
    if(mm_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type == RRCMN_CS_VOICE_CALL)
    {
      /* Fill in the codec type used in RRC_SYNC_IND*/
      if(rbr.rabs.rabs_in_msg[rab_idx].nas_Synchronisation_Indicator == MVS_MODE_AMR_WB)
      {
        mm_ptr->cmd.rrc_sync_ind.rab_info.codec = RRC_CODEC_WB_AMR;
      }
      else
      {
        mm_ptr->cmd.rrc_sync_ind.rab_info.codec = RRC_CODEC_NB_AMR;
      }
    }
    WRRC_MSG1_HIGH("Codec %d in RRC_SYNC_IND",mm_ptr->cmd.rrc_sync_ind.rab_info.codec);   
#endif/*FEATURE_VOC_AMR_WB*/

#ifdef FEATURE_UMTS_VOICE_CIPHERING_IND
    #error code not present
#endif
#ifdef FEATURE_DUAL_SIM
    mm_ptr->cmd.rrc_sync_ind.as_id = rrc_get_as_id();
#endif
    /* Put the command on MM queue */
    WRRC_MSG2_HIGH("Dispatching out MM cmd / Sending RRC_SYNC_IND to MM for RAB type %d RAB-id %d", 
                mm_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type, 
                mm_ptr->cmd.rrc_sync_ind.rab_info.rab_id);
  
    rrc_send_rrc_message_to_mm(mm_ptr);
  }
#ifdef FEATURE_SCUDIF_ENHANCEMENT
  #error code not present
#else
  return FALSE;
#endif
}

#if defined (FEATURE_VOC_AMR_WB)  || defined(FEATURE_SCUDIF_ENHANCEMENT)
/*===========================================================================

FUNCTION   rrcrbr_update_rab_info_reconfiglist

DESCRIPTION Updates the RAB information received in the RAB_InformationReconfigList IE in 
                      RB Release message.
                      It updates the NAS Syncronisation Indicator field received in the RB Release message.
                      In case the NAS Syncronisation Indicator IE is not present , then a default value of 
                      current codec  used is updated.



DEPENDENCIES
  None.

RETURN VALUE
 None

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL uecomdef_status_e_type rrcrbr_update_rab_info_reconfiglist
(
  rrc_RAB_InformationReconfigList *rab_list_ptr
)
{
  uint32 rab_index=0;  
#ifdef FEATURE_VOC_AMR_WB
  mvs_mode_type nas_sync_ind = rrc_get_current_codec_mode();
#endif /*FEATURE_VOC_AMR_WB*/

  rrc_rab_id_type rab_id;           /* To store RAB id in internal RRC format */
  rrcmn_rab_status_T rab_type = RRCMN_INVALID_RAB_ID;     /* To store the status returned by MN layer. This
                                    indicates what type of RAB is received. This is
                                    used only if the RAB is for the CS_domain */
                                    
  if(rab_list_ptr != NULL)
  {
    for (rab_index=0; (rab_list_ptr->n > rab_index) && (rab_index < RRCRB_MAX_RAB_PER_MSG); rab_index++)
    {
      rab_id = 
          rrcrb_translate_gsm_map_rab_id((rab_list_ptr->elem[rab_index].rab_Identity.u.gsm_MAP_RAB_Identity));
                                            
  
      /* Store this info locally so that ESTABLISHED_RABS can
      be updated later, when these RABs are established */
      rbr.rabs.rabs_in_msg[rab_index].rab_id_is_gsm_map = TRUE;
      rbr.rabs.rabs_in_msg[rab_index].rab_id = rab_id;
      
      /* Check if this RAB is CS Domain or PS Domain */  
      if(rab_list_ptr->elem[rab_index].cn_DomainIdentity == rrc_CN_DomainIdentity_cs_domain )
      {
        /* Check if it is in loopback test */
        if ( rrclbt_is_lb_on() == TRUE )
        {
          /* Set Rab type to Data */
          MSG_LOW("Loopback Test", 0, 0, 0);
          rbr.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_DATA_RAB;
        }
        else
        {
           /* For CS RABs, Call MN layer to verify this RAB */
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
          rab_type = rrcmn_verify_rab_subs( (radio_access_bearer_T)rab_id ,rrc_get_as_id());
#else
          rab_type = rrcmn_verify_rab( (radio_access_bearer_T)rab_id );
#endif
          rbr.rabs.rabs_in_msg[rab_index].cn_domain =RRC_CS_DOMAIN_CN_ID;

          MSG_HIGH("For RAB-Id %d, RAB type from NAS = %d, RAB type from EST_RABs = %d",rab_id, rab_type,
            rrc_get_rab_type_for_given_rab_id(rbr.rabs.rabs_in_msg[rab_index].rab_id));

          if( rab_type == RRCMN_CS_VOICE_CALL )
          {
            rbr.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_VOICE_RAB;
          }
          else
          {
            rbr.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_DATA_RAB;
          }
          
#ifdef FEATURE_VOC_AMR_WB
          if( (rab_type == RRCMN_CS_VOICE_CALL )
#ifdef FEATURE_SCUDIF_ENHANCEMENT
#error code not present
#endif
             )
          {
            nas_sync_ind =  rrc_translate_nas_sync_indicator(&(rab_list_ptr->elem[rab_index].nas_Synchronisation_Indicator));
            if( nas_sync_ind == MVS_MODE_NONE)
            {
              rbr.status.failure_status= RRCRB_MSG_UNSUPPORTED_CONFIG;
              WRRC_MSG1_HIGH("Unsupported configuration .. codec not supported %d",nas_sync_ind );
              return FAILURE;
            }
#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
            rrc_set_ordered_codec_status(TRUE);
#endif
          }
#endif /*FEATURE_VOC_AMR_WB*/
        }   
      }
      else
      {
        rbr.rabs.rabs_in_msg[rab_index].cn_domain =RRC_PS_DOMAIN_CN_ID;
        rbr.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_PS_DATA_RAB;
      }
#ifdef FEATURE_VOC_AMR_WB
      rbr.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator = nas_sync_ind;
#endif /*FEATURE_VOC_AMR_WB*/
     
    } 
  }
  
  rbr.rabs.num_rabs_in_msg = rab_index;
#ifdef FEATURE_VOC_AMR_WB
  rrc_set_ordered_codec_mode(nas_sync_ind);
#endif /*FEATURE_VOC_AMR_WB*/
  return SUCCESS;
}
#endif /*FEATURE_VOC_AMR_WB || FEATURE_SCUDIF_ENHANCEMENT*/



/*===========================================================================

FUNCTION   VALIDATE_RBRELEASE_R7

DESCRIPTION

  This function validates the received Rel 7 RB Release Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a Radio Bearer Release Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_rbrelease_r7
(
rrc_RadioBearerRelease *msg_ptr      /* Pointer to the RB Release message */
)
{
  rrc_RadioBearerRelease_r7_IEs* rb_release_ptr = NULL;
  
  /* Assume the message is valid for now */
  rbr.status.failure_status = RRCRB_MSG_VALID;
  
 
  rb_release_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
    u.criticalExtensions.u.r7.radioBearerRelease_r7;
  
  
  /* Check the RRC State indicatior given by the message */
  switch (rb_release_ptr->rrc_StateIndicator )
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      rbr.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;
    
    case rrc_RRC_StateIndicator_cell_FACH:
      rbr.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      rbr.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;
    
    case rrc_RRC_StateIndicator_ura_PCH:
      rbr.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;
    
    default:
      /* No other states are supported right now. */
      WRRC_MSG0_HIGH("Unsupported state in RB Release Message");
      rbr.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
      break;
  }
  
  if ( rbr.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
    return;
  }

  /* Validate DL HSPDSCH */
  if ( RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,dl_HSPDSCH_Information))
  {
    /* Check if network does not try to initiate HSDPA with invalid state */
    if (rrc_hsdpa_initial_validation(rb_release_ptr->rrc_StateIndicator, 
                                      RRC_CHECK_COMMON_MSG_TYPE(rb_release_ptr->dl_HSPDSCH_Information.modeSpecificInfo,
                                        rrc_DL_HSPDSCH_Information_modeSpecificInfo_tdd) ) == FAILURE)
    {
      rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }
  
  /* Check if there are new RABs to be reconfigured
  This is not supported now. */
  /*if (rb_release_ptr->bit_mask & rrc_RadioBearerRelease_r6_IEs_rab_InformationReconfigList_present)
  {
    MSG_MED("RAB Info to reconfigure present in RBR Message", 0, 0, 0);
  } */

  if ( rb_release_ptr->rb_InformationReleaseList.n == 0 )
  {
    WRRC_MSG0_HIGH("RBInfoReleaselist is absent in RBR Message");
    rbr.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
    rbr.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
    return;
  }

  /*if ( rb_release_ptr->bit_mask & rrc_RadioBearerRelease_r5_IEs_rb_InformationAffectedList_present )
  {
    MSG_MED("RB Info Affected list present in RBR Message", 0, 0, 0);
  }*/


  if(RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,rb_InformationReconfigList))
  {
    /* Save the linked list head pointer */
    if (FAILURE == rrcllc_validate_rb_info_reconfig_list_r7(
        &rb_release_ptr->rb_InformationReconfigList))
    {
      rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
#if defined (FEATURE_VOC_AMR_WB ) || defined(FEATURE_SCUDIF_ENHANCEMENT)

    if(RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,rab_InformationReconfigList))
    {
      if(rrcrbr_update_rab_info_reconfiglist(&rb_release_ptr->rab_InformationReconfigList)
         == FAILURE)
      {
        rbr.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
        return;
      }
    }
    else
    {
      /* Set the Ordered codec mode with the current active codec */
      rrc_set_ordered_codec_mode(rrc_active_codec);
      rbr.rabs.num_rabs_in_msg =0;
    }
#endif /*FEATURE_VOC_AMR_WB*/

  }
  
  /* Check if RB with PDCP Information list is present. This
  is not supported in the first release. This may be supported
  later */

  if (RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,dl_CounterSynchronisationInfo))
  {
    /*check whether the SRNS relocation is within Cell_DCH or not.
    SRNS relocation information can come in reconfiguration messages
    only when the state transition is Cell_DCH->Cell_DCH */
    if (!((rbr.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
      ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("SRNS reloc with invalid states cur_state %d, msg_state %d",
        rrc_get_state(), rbr.state_indicated_by_msg);
      rbr.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has 
    PDCP info list. If it is true then this configuration is unsupported */
    if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2(rb_release_ptr->dl_CounterSynchronisationInfo,
         rb_WithPDCP_InfoList)) ||
        (RRC_MSG_COMMON_BITMASK_IE(rb_release_ptr->dl_CounterSynchronisationInfo,
         rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
    {
      WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
      rbr.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation. 
         Set the flag that indicates that this procedure
         involves SRNS relocation */
      rbr.rbr_for_srns_relocation = TRUE;
      /* store if new U-RNTI is present in the message */

      if ( RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,new_U_RNTI))
      {
        rbr.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(rb_release_ptr->new_U_RNTI),
          &rbr.new_urnti);
      }
    }
  } /* dl counter sync info */

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  if((rbr.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    
    if((RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,dl_CommonInformation)) 
        && (RRC_MSG_COMMON_BITMASK_IE(rb_release_ptr->dl_CommonInformation, 
        rrc_DL_CommonInformation_r7,dl_dpchInfoCommon)) &&
       (rb_release_ptr->dl_CommonInformation.dl_dpchInfoCommon.t == 
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
             ( RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.dl_DPCH_InfoCommon,
              rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
             &rb_release_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
             &mac_d_hfn_l);

        if(loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("get_mac_d_hfn failed");
        }
        if(rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
        {
          WRRC_MSG0_HIGH("Process Ciphering Config for HHO failed");
        }
        rbr.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }
  

  /*Cell_PCH/URA_PCH specific validations */
  if ((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
    (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
    /* If UTRAN DRX coefficient is not present then it is invalid configuration */
    
    if (!( RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,utran_DRX_CycleLengthCoeff)))
    {
      WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", rbr.state_indicated_by_msg);
      rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    if (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH)
    {
      /*should we do it after getting l2ack for response??*/
     
      if (!( RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,ura_Identity)))
        
      {
        WRRC_MSG0_HIGH("URA ID not present");
        rrccu_clear_ura_id ();
      }
      else
      {
        /*URA ID present in the message set ura_identity variable with this value */
        if (rb_release_ptr->ura_Identity.numbits == 16)
        {
          rrccu_update_ura_id (&(rb_release_ptr->ura_Identity));
        }
        else
        {
          WRRC_MSG1_ERROR("Invalid URA ID length%d",
            rb_release_ptr->ura_Identity.numbits);
          rbr.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
          rbr.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
          return;
        }
        
      }
      
    }
  }
 
  /* Reject if any MBMS info is present */
  if (RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,mbms_PL_ServiceRestrictInfo))
  {
    WRRC_MSG0_HIGH("MBMS info not supported in RBR");
    rbr.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
  }

}/* end function validate_rbrelease_r7 */




/*===========================================================================

FUNCTION   VALIDATE_RBRELEASE_R6

DESCRIPTION

  This function validates the received Rel 5 RB Release Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a Radio Bearer Release Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_rbrelease_r6
(
rrc_RadioBearerRelease *msg_ptr      /* Pointer to the RB Release message */
)
{
  rrc_RadioBearerRelease_r6_IEs* rb_release_ptr = NULL;
  
  /* Assume the message is valid for now */
  rbr.status.failure_status = RRCRB_MSG_VALID;
  
  
  rb_release_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.
    u.criticalExtensions.u.r6.radioBearerRelease_r6;
  
  
  /* Check the RRC State indicatior given by the message */
  switch (rb_release_ptr->rrc_StateIndicator )
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      rbr.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;
    
    case rrc_RRC_StateIndicator_cell_FACH:
      rbr.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      rbr.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;
    
    case rrc_RRC_StateIndicator_ura_PCH:
      rbr.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;
    
    default:
      /* No other states are supported right now. */
      WRRC_MSG0_HIGH("Unsupported state in RB Release Message");
      rbr.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
      break;
  }
  
  if ( rbr.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
    return;
  }

  /* Validate DL HSPDSCH */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, rrc_RadioBearerRelease_r6_IEs,dl_HSPDSCH_Information))
  {
    /* Check if network does not try to initiate HSDPA with invalid state */
    if (rrc_hsdpa_initial_validation_r6(rb_release_ptr->rrc_StateIndicator, 
                                        &rb_release_ptr->dl_HSPDSCH_Information) == FAILURE)
    {
      rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }
  
  /* Check if there are new RABs to be reconfigured
  This is not supported now. */
  /*if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, rrc_RadioBearerRelease_r6_IEs,rab_InformationReconfigList))
  {
    MSG_MED("RAB Info to reconfigure present in RBR Message", 0, 0, 0);
  } */

  if ( rb_release_ptr->rb_InformationReleaseList.n == 0 )
  {
    WRRC_MSG0_HIGH("RBInfoReleaselist is absent in RBR Message");
    rbr.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
    rbr.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
    return;
  }

  /*if ( RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, rrc_RadioBearerRelease_r5_IEs_rb_InformationAffectedList) )
  {
    MSG_MED("RB Info Affected list present in RBR Message", 0, 0, 0);
  }*/


  if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,
               rrc_RadioBearerRelease_r6_IEs,rb_InformationReconfigList))
  {
    /* Save the linked list head pointer */
    if (FAILURE == rrcllc_validate_rb_info_reconfig_list_r6(
        &rb_release_ptr->rb_InformationReconfigList))
    {
      rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
#if defined (FEATURE_VOC_AMR_WB)  || defined(FEATURE_SCUDIF_ENHANCEMENT)
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, rrc_RadioBearerRelease_r6_IEs,rab_InformationReconfigList))
    {
      if(rrcrbr_update_rab_info_reconfiglist(&rb_release_ptr->rab_InformationReconfigList)
         == FAILURE)
      {
        rbr.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
        return;
      }
    }
    else
    {
#ifdef FEATURE_VOC_AMR_WB
      /* Set the Ordered codec mode with the current active codec */
      rrc_set_ordered_codec_mode(rrc_active_codec);
#endif
      rbr.rabs.num_rabs_in_msg =0;
    }
#endif /*FEATURE_VOC_AMR_WB  || FEATURE_SCUDIF_ENHANCEMENT*/

  }
  
  /* Check if RB with PDCP Information list is present. This
  is not supported in the first release. This may be supported
  later */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,
    rrc_RadioBearerRelease_r6_IEs,dl_CounterSynchronisationInfo))
  {
    /*check whether the SRNS relocation is within Cell_DCH or not.
    SRNS relocation information can come in reconfiguration messages
    only when the state transition is Cell_DCH->Cell_DCH */
    if (!((rbr.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
      ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("SRNS reloc with invalid states cur_state %d, msg_state %d",
        rrc_get_state(), rbr.state_indicated_by_msg);
      rbr.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has 
    PDCP info list. If it is true then this configuration is unsupported */
    if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2(rb_release_ptr->dl_CounterSynchronisationInfo,
         rb_WithPDCP_InfoList)) ||
        (RRC_MSG_COMMON_BITMASK_IE(rb_release_ptr->dl_CounterSynchronisationInfo,
         rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
    {
      WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
      rbr.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation. 
         Set the flag that indicates that this procedure
         involves SRNS relocation */
      rbr.rbr_for_srns_relocation = TRUE;
      /* store if new U-RNTI is present in the message */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, 
          rrc_RadioBearerRelease_r6_IEs,new_U_RNTI))
      {
        rbr.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(rb_release_ptr->new_U_RNTI),
          &rbr.new_urnti);
      }
    }
  } /* dl counter sync info */

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  if((rbr.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,
        rrc_RadioBearerRelease_r6_IEs,dl_CommonInformation)) && 
       (RRC_MSG_COMMON_BITMASK_IE(rb_release_ptr->dl_CommonInformation,
        rrc_DL_CommonInformation_r6,dl_dpchInfoCommon)) &&
       (rb_release_ptr->dl_CommonInformation.dl_dpchInfoCommon.t == 
        T_rrc_DL_CommonInformation_r6_dl_dpchInfoCommon_dl_DPCH_InfoCommon))
    {
      //check if ciphering is active in mac for cs domain, if it is then,
      //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      if(rrcsmc_tm_ciph_active() == TRUE)
      {
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        uecomdef_status_e_type loc_ret_val;
        WRRC_MSG0_HIGH("To-DCH-Step-1 config activated");

        loc_ret_val = rrcrb_get_mac_d_hfn(
             ( RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr->dl_CommonInformation.dl_dpchInfoCommon.
             u.dl_DPCH_InfoCommon,rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
             &rb_release_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
             &mac_d_hfn_l);

        if(loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("get_mac_d_hfn failed");
        }
        if(rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
        {
          WRRC_MSG0_HIGH("Process Ciphering Config for HHO failed");
        }
        rbr.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }
  

  /*Cell_PCH/URA_PCH specific validations */
  if ((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
    (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
    /* If UTRAN DRX coefficient is not present then it is invalid configuration */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, 
      rrc_RadioBearerRelease_r6_IEs,utran_DRX_CycleLengthCoeff)))
    {
      WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", rbr.state_indicated_by_msg);
      rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    if (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH)
    {
      /*should we do it after getting l2ack for response??*/
      if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, 
        rrc_RadioBearerRelease_r6_IEs,ura_Identity)))
        
      {
        WRRC_MSG0_HIGH("URA ID not present");
        rrccu_clear_ura_id ();
      }
      else
      {
        /*URA ID present in the message set ura_identity variable with this value */
        if (rb_release_ptr->ura_Identity.numbits == 16)
        {
          rrccu_update_ura_id (&(rb_release_ptr->ura_Identity));
        }
        else
        {
          WRRC_MSG1_ERROR("Invalid URA ID length%d",
            rb_release_ptr->ura_Identity.numbits);
          rbr.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
          rbr.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
          return;
        }
        
      }
      
    }
  }

  /* Reject if any MBMS info is present */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, 
      rrc_RadioBearerRelease_r6_IEs,mbms_PL_ServiceRestrictInfo)) ||
      (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, 
      rrc_RadioBearerRelease_r6_IEs,mbms_PL_ServiceRestrictInfo)))
  {
    WRRC_MSG0_HIGH("MBMS info not supported in RBR");
    rbr.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
  }

}/* end function validate_rbrelease_r6 */


/*===========================================================================

FUNCTION   VALIDATE_RBRELEASE_R5

DESCRIPTION

  This function validates the received Rel 5 RB Release Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a Radio Bearer Release Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_rbrelease_r5
(
rrc_RadioBearerRelease *msg_ptr      /* Pointer to the RB Release message */
)
{
  rrc_RadioBearerRelease_r5_IEs* rb_release_ptr = NULL;
  
  /* Assume the message is valid for now */
  rbr.status.failure_status = RRCRB_MSG_VALID;
  
  
  rb_release_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.
    u.r5.radioBearerRelease_r5;
  
  
  /* Check the RRC State indicatior given by the message */
  switch ( rb_release_ptr->rrc_StateIndicator )
  {
  case rrc_RRC_StateIndicator_cell_DCH:
    rbr.state_indicated_by_msg = RRC_STATE_CELL_DCH;
    break;
    
  case rrc_RRC_StateIndicator_cell_FACH:
    rbr.state_indicated_by_msg = RRC_STATE_CELL_FACH;
    break;
  case rrc_RRC_StateIndicator_cell_PCH:
    rbr.state_indicated_by_msg = RRC_STATE_CELL_PCH;
    break;
    
  case rrc_RRC_StateIndicator_ura_PCH:
    rbr.state_indicated_by_msg = RRC_STATE_URA_PCH;
    break;
    
  default:
    /* No other states are supported right now. */
    WRRC_MSG0_HIGH("Unsupported state in RB Release Message");
    rbr.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
    break;
  }
  
  if ( rbr.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
    return;
  }

  /* Validate DL HSPDSCH */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, rrc_RadioBearerRelease_r5_IEs,dl_HSPDSCH_Information))
  {

    /* Check if network does not try to initiate HSDPA with invalid state */
    if (rrc_hsdpa_initial_validation(rb_release_ptr->rrc_StateIndicator, 
                RRC_CHECK_COMMON_MSG_TYPE(rb_release_ptr->dl_HSPDSCH_Information.modeSpecificInfo,
                   rrc_DL_HSPDSCH_Information_modeSpecificInfo_tdd ) ) == FAILURE)
    {
      rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }

  /* Check if there are new RABs to be reconfigured
  This is not supported now. */
#if defined (FEATURE_VOC_AMR_WB)  || defined(FEATURE_SCUDIF_ENHANCEMENT) 
 
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, rrc_RadioBearerRelease_r5_IEs,rab_InformationReconfigList))
  {
    MSG_MED("RAB Info to reconfigure present in RBR Message", 0, 0, 0);
    if(rrcrbr_update_rab_info_reconfiglist(&rb_release_ptr->rab_InformationReconfigList)
       == FAILURE)
    {
      return;
    }
  }
  else
  {
    rbr.rabs.num_rabs_in_msg =0;
#ifdef FEATURE_VOC_AMR_WB
    /* Set the Ordered codec mode with the current active codec */
    rrc_set_ordered_codec_mode(rrc_active_codec);
#endif
  }
#else /*FEATURE_VOC_AMR_WB*/
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, rrc_RadioBearerRelease_r5_IEs,rab_InformationReconfigList))
  {
    MSG_MED("RAB Info to reconfigure present in RBR Message", 0, 0, 0);
  } 
#endif /*FEATURE_VOC_AMR_WB  || FEATURE_SCUDIF_ENHANCEMENT*/
  if ( rb_release_ptr->rb_InformationReleaseList.n == 0 )
  {
    WRRC_MSG0_HIGH("RBInfoReleaselist is absent in RBR Message");
    rbr.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
    rbr.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
    return;
  }

  if ( RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, rrc_RadioBearerRelease_r5_IEs,rb_InformationAffectedList) )
  {
    MSG_MED("RB Info Affected list present in RBR Message", 0, 0, 0);
  }


  
  /* Check if RB with PDCP Information list is present. This
  is not supported in the first release. This may be supported
  later */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,
    rrc_RadioBearerRelease_r5_IEs,dl_CounterSynchronisationInfo))
  {
    /*check whether the SRNS relocation is within Cell_DCH or not.
    SRNS relocation information can come in reconfiguration messages
    only when the state transition is Cell_DCH->Cell_DCH */
    if (!((rbr.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
      ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("SRNS reloc with invalid states cur_state %d, msg_state %d",
        rrc_get_state(), rbr.state_indicated_by_msg);
      rbr.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has 
    PDCP info list. If it is true then this configuration is unsupported */
    if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2(rb_release_ptr->dl_CounterSynchronisationInfo,
          rb_WithPDCP_InfoList)) ||
        (RRC_MSG_COMMON_BITMASK_IE(rb_release_ptr->dl_CounterSynchronisationInfo,
         rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
    {
      WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
      rbr.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
    /* The procedure involves Lossy SRNS relocation. 
    Set the flag that indicates that this procedure
      involves SRNS relocation */
      rbr.rbr_for_srns_relocation = TRUE;
      /* store if new U-RNTI is present in the message */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, 
        rrc_RadioBearerRelease_r5_IEs,new_U_RNTI))
      {
        rbr.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(rb_release_ptr->new_U_RNTI),
          &rbr.new_urnti);
      }
    }

    
  } /* dl counter sync info */

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  if((rbr.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,
        rrc_RadioBearerRelease_r5_IEs,dl_CommonInformation)) && 
       (RRC_MSG_COMMON_BITMASK_IE(rb_release_ptr->dl_CommonInformation,
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
             ( RRC_MSG_COMMON_BITMASK_IE(rb_release_ptr->dl_CommonInformation.dl_DPCH_InfoCommon,
             rrc_DL_DPCH_InfoCommon_r4,mac_d_HFN_initial_value)),
             &rb_release_ptr->dl_CommonInformation.dl_DPCH_InfoCommon.mac_d_HFN_initial_value,
             &mac_d_hfn_l);
        if(loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("get_mac_d_hfn failed");
        }
        if(rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
        {
          WRRC_MSG0_HIGH("Process Ciphering Config for HHO failed");
        }
        rbr.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }
  

  /*Cell_PCH/URA_PCH specific validations */
  if ((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
    (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
    /* If UTRAN DRX coefficient is not present then it is invalid configuration */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, 
      rrc_RadioBearerRelease_r5_IEs,utran_DRX_CycleLengthCoeff)))
    {
      WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", rbr.state_indicated_by_msg);
      rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    if (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH)
    {
      /*should we do it after getting l2ack for response??*/
      if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, 
        rrc_RadioBearerRelease_r5_IEs,ura_Identity)))
        
      {
        WRRC_MSG0_HIGH("URA ID not present");
        rrccu_clear_ura_id ();
      }
      else
      {
        /*URA ID present in the message set ura_identity variable with this value */
        if (rb_release_ptr->ura_Identity.numbits == 16)
        {
          rrccu_update_ura_id (&(rb_release_ptr->ura_Identity));
        }
        else
        {
          WRRC_MSG1_ERROR("Invalid URA ID length%d",
            rb_release_ptr->ura_Identity.numbits);
          rbr.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
          rbr.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
          return;
        }
        
      }
      
    }
  }


}/* end function validate_rbrelease_r5 */



/*===========================================================================

FUNCTION   VALIDATE_RBRELEASE_MESSAGE

DESCRIPTION

  This function validates the received RB Release Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a Radio Bearer Release Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_rbrelease_message
(
rrc_RadioBearerRelease *msg_ptr      /* Pointer to the RB Release message */
)
{

  /* Assume the message is valid for now */
  rbr.status.failure_status = RRCRB_MSG_VALID;

  if ( msg_ptr->t == T_rrc_RadioBearerRelease_later_than_r3 )
  {
    /* Critical extensions are present that are not supported by this
    release. We have to send a Radio Bearer Release Failure message. */
    WRRC_MSG0_HIGH("Critical exstension present in RBR msg!");
    rbr.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
    rbr.status.prot_err_cause = rrc_ProtocolErrorCause_messageExtensionNotComprehended;
    return;
  }

  /* Check the RRC State indicatior given by the message */
  switch ( msg_ptr->u.r3.radioBearerRelease_r3.rrc_StateIndicator )
  {
  case rrc_RRC_StateIndicator_cell_DCH:
    rbr.state_indicated_by_msg = RRC_STATE_CELL_DCH;
    break;

  case rrc_RRC_StateIndicator_cell_FACH:
    rbr.state_indicated_by_msg = RRC_STATE_CELL_FACH;
    break;
  case rrc_RRC_StateIndicator_cell_PCH:
    rbr.state_indicated_by_msg = RRC_STATE_CELL_PCH;
    break;

  case rrc_RRC_StateIndicator_ura_PCH:
    rbr.state_indicated_by_msg = RRC_STATE_URA_PCH;
    break;

  default:
    /* No other states are supported right now. */
    WRRC_MSG0_HIGH("Unsupported state in RB Release Message");
    rbr.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
    break;
  }

  if ( rbr.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG )
  {
    /* No need to continue */
    return;
  }


  /* Check if there are new RABs to be reconfigured
  This is not supported now. */
#if defined (FEATURE_VOC_AMR_WB)  || defined(FEATURE_SCUDIF_ENHANCEMENT)
  if ( (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3,
        rrc_RadioBearerRelease_r3_IEs,rab_InformationReconfigList) )
     )
  {
   
    if(rrcrbr_update_rab_info_reconfiglist( &msg_ptr->u.r3.radioBearerRelease_r3.rab_InformationReconfigList)
         == FAILURE)
    {
      return;
    }
  }
  else
  {
    rbr.rabs.num_rabs_in_msg =0;
#ifdef FEATURE_VOC_AMR_WB
     /* Set the Ordered codec mode with the current active codec */
    rrc_set_ordered_codec_mode(rrc_active_codec);
#endif
  }
#else /*FEATURE_VOC_AMR_WB*/
  if ( (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3,
        rrc_RadioBearerRelease_r3_IEs,rab_InformationReconfigList) )
     )
  {
    MSG_LOW("RAB Info to reconfigure present in RBR Message",0,0,0);
  }
#endif /*FEATURE_VOC_AMR_WB  || FEATURE_SCUDIF_ENHANCEMENT*/
  if ( msg_ptr->u.r3.radioBearerRelease_r3.rb_InformationReleaseList.n
      == 0 )
  {
    WRRC_MSG0_HIGH("RBInfoReleaselist is absent in RBR Message");
    rbr.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
    rbr.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
    return;
  }
  /* Check if RB Information Affected list is present. This
  is not supported in the first release. This may be supported
  later */
  if ( RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3,
        rrc_RadioBearerRelease_r3_IEs,rb_InformationAffectedList) 
     )
  {
    MSG_MED("RB Info Affected list present in RBR Message", 0, 0, 0);
  }

  /* Check if RB with PDCP Information list is present. This
  is not supported in the first release. This may be supported
  later */
  if ( RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3,
        rrc_RadioBearerRelease_r3_IEs,dl_CounterSynchronisationInfo))
  {
  /*check whether the SRNS relocation is within Cell_DCH or not.
     SRNS relocation information can come in reconfiguration messages
     only when the state transition is Cell_DCH->Cell_DCH */
    if (!((rbr.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("SRNS reloc with invalid states cur_state %d, msg_state %d",
             rrc_get_state(), rbr.state_indicated_by_msg);
      rbr.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has 
    PDCP info list. If it is true then this configuration is unsupported */
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3.dl_CounterSynchronisationInfo,
        rrc_DL_CounterSynchronisationInfo,rB_WithPDCP_InfoList))
    {
      WRRC_MSG0_HIGH("PDCP info in DL counter sync info not supported");
      rbr.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation. 
        Set the flag that indicates that this procedure
      involves SRNS relocation */
      rbr.rbr_for_srns_relocation = TRUE;
      /* store if new U-RNTI is present in the message */
      if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3,
          rrc_RadioBearerRelease_r3_IEs,new_U_RNTI))
      {
        rbr.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(msg_ptr->u.r3.radioBearerRelease_r3.new_U_RNTI),
                              &rbr.new_urnti);
      }
    }
  } /* dl counter sync info */

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  if((rbr.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3,
        rrc_RadioBearerRelease_r3_IEs,dl_CommonInformation)) && 
       (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3.dl_CommonInformation,
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
        rbr.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }
  /*Cell_PCH/URA_PCH specific validations */
  if ((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
     /* If UTRAN DRX coefficient is not present then it is invalid configuration */
     if (!(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3,
         rrc_RadioBearerRelease_r3_IEs,utran_DRX_CycleLengthCoeff)))
     {
       WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", rbr.state_indicated_by_msg);
       rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
       return;
     }
     if (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH)
     {
       /*should we do it after getting l2ack for response??*/
       if (!(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3, 
             rrc_RadioBearerRelease_r3_IEs,ura_Identity)))
           
       {
         WRRC_MSG0_HIGH("URA ID not present");
         rrccu_clear_ura_id ();
       }
       else
       {
         /*URA ID present in the message set ura_identity variable with this value */
         if (msg_ptr->u.r3.radioBearerRelease_r3.ura_Identity.numbits == 16)
         {
           rrccu_update_ura_id (&(msg_ptr->u.r3.radioBearerRelease_r3.ura_Identity));
         }
         else
         {
           WRRC_MSG1_ERROR("Invalid URA ID length%d",
               msg_ptr->u.r3.radioBearerRelease_r3.ura_Identity.numbits);
           rbr.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
           rbr.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
           return;
         }

       }
           
     }
  }

}/* end function validate_rbrelease_message */



/*===========================================================================

FUNCTION   RRCRBR_SEND_CHANNEL_CONFIG_REQ

DESCRIPTION

  This function sends the RRC_CHANNEL_CONFIG_REQ command to LLC
  to release the specified RBs. If signalling Radio 
  bearers are affected and included in the Radio Bearer
  Release Message, they will also be sent in the same command.
        
DEPENDENCIES

  ORDERED_CONFIG must be set before calling this function.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbr_send_channel_config_req
(
  void
)
{
  rrc_cmd_type *ptr;          /* To hold the Command                       */
  uint32 rb_count=0;          /* To count the number of RBs in the command */
  uint32 rb_index=0;          /* To index the array of RBs in the command */
  rrc_state_e_type next_rrc_state = rbr.state_indicated_by_msg;
  
  /* Get the command buffer */
    ptr = rrc_get_int_cmd_buf();
    /* Fill in the command id  */
    ptr->cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;

    /* First fill up the Radio Bearers that need to be released.
    For this we need to know how many RBs need to be released.
    */
    
    if( rbr.rbs_tobe_released.is_valid == TRUE )
    {
      for(rb_count=0; ((rb_count<rbr.rbs_tobe_released.num_rbs)&&(rb_index < MAX_RB)); rb_count++)
      {
        /* Fill up the RB id and indicate that it needs to be released */
        if(MAX_RB == rrcllc_check_if_rbid_present_in_chan_config_within_index(
                  &ptr->cmd.chan_config_req, rbr.rbs_tobe_released.rb_id[rb_count],rb_index))
        {
          ptr->cmd.chan_config_req.rb[rb_index].rb_id = 
            rbr.rbs_tobe_released.rb_id[rb_count];
          ptr->cmd.chan_config_req.rb[rb_index].rb_config = RELEASE_RB;
          rb_index++;
        }
        else
        {
          WRRC_MSG2_HIGH(" Ignoring Duplicate RB ID % d in RBR at index %d ",(uint16)rbr.rbs_tobe_released.rb_id[rb_count],rb_index);
        }
      }
    }/* end if rbr.rbs_tobe_released.is_valid  == TRUE */

    if(TRUE == rbr.rbs_tobe_reconfigured.is_valid)
    {
      for(rb_count=0; ((rb_count<rbr.rbs_tobe_reconfigured.num_rbs)&&(rb_index < MAX_RB)); rb_count++)
      {
        if(MAX_RB == rrcllc_check_if_rbid_present_in_chan_config_within_index(
                  &ptr->cmd.chan_config_req, (uint16)rbr.rbs_tobe_reconfigured.rb_info[rb_count].rb_id,rb_index))
        {
          /* Fill up the RB id and indicate that it needs to be released */
          ptr->cmd.chan_config_req.rb[rb_index].rb_id = 
            (uint16)rbr.rbs_tobe_reconfigured.rb_info[rb_count].rb_id;
          ptr->cmd.chan_config_req.rb[rb_index].rb_config = RECONFIG_RB_OTA;
          ptr->cmd.chan_config_req.rb[rb_index].ul_cfg_mask = ptr->cmd.chan_config_req.rb[rb_index].dl_cfg_mask = MODIFY_RB_CFG;
          rb_index++;
        }
        else
        {
          WRRC_MSG2_HIGH(" Ignoring Duplicate RB ID % d in RBR at index %d ",(uint16)rbr.rbs_tobe_reconfigured.rb_info[rb_count].rb_id,rb_index);
        }				
      }
    }/* end if rbr.rbs_tobe_reconfigured.is_valid  == TRUE */

    if( rbr.rbs_tobe_affected.is_valid == TRUE )
    {
      for(rb_count=0; ((rb_count<rbr.rbs_tobe_affected.num_rbs)&&(rb_index < MAX_RB)); rb_count++)
      {
        if(MAX_RB == rrcllc_check_if_rbid_present_in_chan_config_within_index(
                  &ptr->cmd.chan_config_req, (uint16)rbr.rbs_tobe_affected.rb_id[rb_count],rb_index))
        {
          /* Fill up the RB id and indicate that it needs to be released */
          ptr->cmd.chan_config_req.rb[rb_index].rb_id = 
            rbr.rbs_tobe_affected.rb_id[rb_count];
          ptr->cmd.chan_config_req.rb[rb_index].rb_config = RECONFIG_RB_OTA;
          ptr->cmd.chan_config_req.rb[rb_index].ul_cfg_mask = ptr->cmd.chan_config_req.rb[rb_index].dl_cfg_mask = MODIFY_RB_CFG;
          rb_index++;
        }
        else
        {
          WRRC_MSG2_HIGH(" Ignoring Duplicate RB ID % d in RBR at index %d ",(uint16)rbr.rbs_tobe_affected.rb_id[rb_count],rb_index);
        }				
      }
    }/* end if rbr.rbs_tobe_released.is_valid  == TRUE */


    /* Check if state change is required */
    if( (rrc_get_state()) == (rbr.state_indicated_by_msg) )
    {
      ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;
    }
    else
    {
      /* The message indicates that state change is required.
      Indicate to LLC what the next state should be. */
      ptr->cmd.chan_config_req.rrc_state_change_required = TRUE;
      ptr->cmd.chan_config_req.next_state = rbr.state_indicated_by_msg;

      if ((rrc_get_state() == RRC_STATE_CELL_FACH) && 
          ((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) || 
           (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH))&&
           (rb_index < (MAX_RB - 1)))
      {
        /* UE is directly transitioning to Cell_PCH state from Cell_FACH state so
        include release of CCCH and establishment of PCCH in the same channel config
        request */
        ptr->cmd.chan_config_req.rb[rb_index].rb_id       = CCCH_RADIO_BEARER_ID;
        ptr->cmd.chan_config_req.rb[rb_index++].rb_config = RELEASE_RB;
        ptr->cmd.chan_config_req.rb[rb_index].rb_id       = PCCH_RADIO_BEARER_ID;
        ptr->cmd.chan_config_req.rb[rb_index++].rb_config = ESTABLISH_RB;
      }
    }

    /* Number of RBs to config. Note rec_rb_count is the total
    of established and reconfigured RBs */
    ptr->cmd.chan_config_req.num_rb_to_config = (uint16)rb_index; 
    if(rb_index < MAX_RB)
    {
      rrc_fill_rb_id_chan_req_from_standalone_db(&rb_index, &ptr->cmd.chan_config_req);
    }

    if((rbr.state_indicated_by_msg == RRC_STATE_CELL_DCH)&&(rb_index < MAX_RB))
    {
      /* Fill those RBs in chan_config_req for which RLC PDU size has changed and 
      that are already not present in chan_config_req*/
      rrc_fill_rb_id_chan_req_from_rb_list_mapped_to_dch(&rb_index, &ptr->cmd.chan_config_req);
      if(rrc_get_state() == RRC_STATE_CELL_FACH)
      {
        rrc_fill_srb_in_chan_config_req(&rb_index, &ptr->cmd.chan_config_req);
      }
    }
  
    /*If state transition is Cell_DCH->Cell_FACH or vice-verca, add all the AM 
    PS RBs in chan_config_req*/	
    if(((next_rrc_state == RRC_STATE_CELL_FACH) && 
         (transition_config.toc_usage == TOC_FOR_DCH_AND_FACH ||
          transition_config.toc_usage == TOC_FOR_FACH_AND_PCH ||
          transition_config.toc_usage == TOC_FOR_FACH_AND_PCH_WITH_DCH_INFO))
         ||
        ((rrc_get_state() == RRC_STATE_CELL_FACH) && (next_rrc_state == RRC_STATE_CELL_DCH))
      )
    {
      rrc_fill_rb_id_chan_req_from_est_rabs(&rb_index, &ptr->cmd.chan_config_req);
    }
 
    /* Indicate the id of the procedure sending the command */
    ptr->cmd.chan_config_req.procedure = RRC_PROCEDURE_RBR;

    /* Indicate that we need a confirmation for this command */
    ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = TRUE;

    MSG_HIGH("CHANNEL_CONFIG_REQ RBs:%d, curr_st:%d, next_st:%d", 
             rb_index,rrc_get_state(),rbr.state_indicated_by_msg);
    /* We are not required to fill any other data for this command.
    Now, send the command */
    rrc_put_int_cmd( ptr );
}/* end function rrcrbr_send_channel_config_req */


/*===========================================================================

FUNCTION  RRCRBR_CLEAR_PROCEDURE

DESCRIPTION

  This function clears all the internal variables in
  the RBR procedure and brings them to their default
  values.
    
DEPENDENCIES

  The Substate of the procedure should be changed
  to initial substate before calling this function.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbr_clear_procedure( void )
{
  uint32 count;

  /* Initialize the transaction id's for this procedure.
  Use a number that can't be an actual received transaction
  id. */
  rbr.tr_ids.accepted_transaction = RRCRB_INVALID_TRANSACTION_ID;
  rbr.tr_ids.rejected_transaction = RRCRB_INVALID_TRANSACTION_ID;
  /* Clear the C-RNTI valid flag */
  rbr.new_crnti_valid    = FALSE;
  rbr.directed_cell_info = FALSE;

  /* Reset Initiate Cell Update Flag */
  rrcrbr_send_initiate_cu_flag = FALSE;
  rrcrbr_delete_crnti          = FALSE;

  rbr.rbr_for_srns_relocation = FALSE;
  rbr.new_urnti_valid = FALSE;
  rbr.status.failure_status = RRCRB_MSG_VALID;
  /* free the memory allocate for mm_cmd_buf
  if it is not free or in case of unsuccessful events
  before sending this cmd to mm */
  if (rbr.cn_info_saved != FALSE)
  {
    /* free the memory allocated for mm cmd */
    WRRC_MSG1_HIGH("Free MM cmd buffer %x", rbr.mm_cmd_ptr);
    mm_free_cmd_buf(rbr.mm_cmd_ptr);
    /* set cn_info_saved to false */
    rbr.cn_info_saved = FALSE;
  }

  /* Initialize the RABS per message structure */
  for ( count=0; count < RRCRB_MAX_RAB_PER_MSG; count++ )
  {
    rbr.rabs.rabs_in_msg[count].num_rbs_for_rab = 0;
    rbr.rabs.rabs_in_msg[count].rab_type = RRCRB_UNKNOWN_RAB;
    rbr.rabs.chan_config_needed[count] = FALSE;
#ifdef FEATURE_VOC_AMR_WB
    rbr.rabs.rabs_in_msg[count].nas_Synchronisation_Indicator = MVS_MODE_NONE;
#endif /*FEATURE_VOC_AMR_WB*/

  }
  rbr.rabs.num_rabs_in_msg = 0;

  /* Indicate that there are no signalling connections
  to be released. */
  rbr.need_to_release_cn_domain = FALSE;

  /* Indicate that there are no RBs to be released. This
  is the default value. */
  rbr.rbs_tobe_released.is_valid = FALSE;
  rbr.rbs_tobe_reconfigured.is_valid = FALSE;
  rbr.rbs_tobe_affected.is_valid = FALSE;
  rbr.trans_to_dch_ciph_config_needed = FALSE;
  
  rbr.initiate_cell_selection_trans_from_fach = FALSE;     

  rbr.high_mobility_ind = FALSE;

#ifdef FEATURE_VOC_AMR_WB
  rrc_set_ordered_codec_mode(rrc_get_current_codec_mode());
#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
  rrc_set_ordered_codec_status(FALSE);
#endif
#endif

  rrc_check_and_init_rrc_reestab_entity(RRC_PROCEDURE_RBR);
}


/*===========================================================================

FUNCTION   RRCRBR_PROCESS_SUCCESSFUL_PROC_COMPLETION

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
static void rrcrbr_process_successful_proc_completion
(
 boolean indicate_rlc
)
{
  /* send the nas cn info if stored */
  if (rbr.cn_info_saved != FALSE)
  {
    WRRC_MSG0_HIGH("Dispatching out MM cmd / RRC_SERVICE_IND_FROM_OTA_MSG due to CN info to MM");
    if(rbr.mm_cmd_ptr->cmd.hdr.message_id == (byte) RRC_SERVICE_IND_FROM_OTA_MSG )
    {
      /* update the cell access information in the service info structure */
      rrc_ccm_update_service_info(rbr.mm_cmd_ptr);
    }
    /* Put the command on MM queue */
    rrc_send_rrc_message_to_mm(rbr.mm_cmd_ptr);
    /* clear the flag */
    rbr.cn_info_saved = FALSE;
  }
  /* Clear ORDERED_CONFIG - UE can now accept other ordered
  configurations */
  rrcllc_clear_ordered_config();  

  if (((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) || 
       (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH )) &&
      (indicate_rlc == TRUE))
  {
    /* register with RLC for uplink data indication */
    rrcrb_register_with_rlc_for_ul_data_ind (RRC_PROCEDURE_RBR);

  }

  /* Send CLOSE_SESS_IND to NAS for PCH transitions */
  /* For PCH transitions, close session indication is delayed until completion of reconfig procedure 
     If NAS sends close session req before RRC sends close session ind, code enhancement is needed to ignore CLOSE session req from NAS
  */
  if (rbr.need_to_release_cn_domain == TRUE)
  {
    if(rbr.cn_to_release == RRC_CS_DOMAIN_CN_ID)
    {      
      (void)rrcscr_release_signalling_connection(RRC_CS_DOMAIN_CN_ID);
    }
    else
    {
      (void)rrcscr_release_signalling_connection(RRC_PS_DOMAIN_CN_ID);      
    }
    rbr.need_to_release_cn_domain = FALSE;
  }

    /* send the nas cn info if stored */
  if (rbr.cn_info_saved != FALSE)
  {
    /* Put the command on MM queue */
    rrc_send_rrc_message_to_mm(rbr.mm_cmd_ptr);
    /* clear the flag */
    rbr.cn_info_saved = FALSE;
  }


  /* Go to the next sub-state in the state machine. That's
  the initial substate since we've sent the RB RC complete
  message */
  rbr.curr_substate = RRCRB_INITIAL;

  /* At this point we don't need to remember any of the
  Radio Bearer information since it's stored in ESTABLISHED_RABS.
  Clear our internal variables */
  rrcrbr_clear_procedure();
  if (rrccu_return_cu_substate() == RRCCU_WAIT_FOR_RB_PROC_COMPL_IND) 
  {
    rrcrb_send_rb_proc_compl_ind();
  }

  rrccu_reset_registration_info_for_procedure(RRC_PROCEDURE_RBR);
}    /* rrcrbr_process_successful_proc_completion */

/*===========================================================================

FUNCTION   rrcrbr_process_high_mobility_ind_in_non_critical_ext

DESCRIPTION

  This function will process High Mobility Ind received in Non-critical ext
        
DEPENDENCIES
  None.

RETURN VALUE
None

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbr_process_high_mobility_ind_in_non_critical_ext
(
  rrc_RadioBearerRelease *msg_ptr,
  rrc_msg_ver_enum_type msg_version
)
{
  if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
  {
    if(msg_version == MSG_REL5)
    {
      if((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.m.v690NonCriticalExtensionsPresent) &
        (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.v690NonCriticalExtensions.m.
        v770NonCriticalExtensionsPresent) &&
        (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.v690NonCriticalExtensions.v770NonCriticalExtensions.
        radioBearerRelease_v770ext.m.ueMobilityStateIndicatorPresent))
      {
        if(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.v690NonCriticalExtensions.v770NonCriticalExtensions.
            radioBearerRelease_v770ext.ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
        {
          rbr.high_mobility_ind = TRUE;
        }
      }
    }
    else if(msg_version == MSG_REL6)
    {
     if((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.m.v6b0NonCriticalExtensionsPresent) &&
         (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.v6b0NonCriticalExtensions.m.
         v770NonCriticalExtensionsPresent) &&
         (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.v6b0NonCriticalExtensions.v770NonCriticalExtensions.
         radioBearerRelease_v770ext.m.ueMobilityStateIndicatorPresent))
      {
        if(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.v6b0NonCriticalExtensions.
           v770NonCriticalExtensions.radioBearerRelease_v770ext.ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
        {
          rbr.high_mobility_ind = TRUE;
        }
      }
    }
    else if(msg_version == MSG_REL99)
    {
      if((msg_ptr->u.r3.m.v3a0NonCriticalExtensionsPresent) &&
        (msg_ptr->u.r3.v3a0NonCriticalExtensions.m.laterNonCriticalExtensionsPresent) &&
        (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent) &&
        (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.m.
               v590NonCriticalExtensionsPresent) &&
        (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.
        v690NonCriticalExtensionsPresent) &&
        (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.m.v770NonCriticalExtensionsPresent) &&
        (msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
        v770NonCriticalExtensions.radioBearerRelease_v770ext.m.ueMobilityStateIndicatorPresent))
      {
        if(msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
            v690NonCriticalExtensions.v770NonCriticalExtensions.radioBearerRelease_v770ext.ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
        {
          rbr.high_mobility_ind = TRUE;
        }
      }
    }  
  }
  WRRC_MSG2_HIGH("High Mobility Ind Set to %d in RBR.for msg_version:%d(0-MSG_REL99,1-MSG_REL5,2-MSG_REL6,3-MSG_REL7,4-MSG_REL8,5-MSG_REL9,6-MSG_REL10,other-MSG_INVALID)",rbr.high_mobility_ind,msg_version);  
}

/*===========================================================================

FUNCTION   RRCRBR_PROCESS_RBRELEASE_R6

DESCRIPTION

  This function processes the received Rel 6 RB Release Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a Radio Bearer Release Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to release the Radio Bearers given by the message.
    
    If ORDERED_CONFIG could not be set, a Radio Bearer Release Failure message
    will be sent to UTRAN.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbr_process_rbrelease_r6
(
  rrc_cmd_type *cmd_ptr
)
{
  rrc_RB_InformationReleaseList  *rb_info_rel_list_ptr;
  rrc_RB_InformationAffectedList_r6 *rb_info_affected_list_ptr;
   rrc_RB_InformationReconfig_r6 *rb_info_reconfig_ptr;

  rrc_RadioBearerRelease *msg_ptr;  /* Pointer to the RB Release message        */
  rrcrb_directed_cell_info_type directed_cell_info; /* directed cell info struct */

  rrcllc_oc_status_e_type oc_status;/* To hold ordered_config status          */
  boolean ordered_config_err_occured = FALSE; /* Used to indicate if an error
                                    occured while setting ordered_config. */
  boolean rb_rel_srch = FALSE; 
  boolean rab_found = FALSE; 
  rrc_state_e_type current_state;       /* current RRC State  */
  uint32 rab_count, rb_count, localrb_count;
  /* Local C_RNTI value */
  rrc_int_c_rnti_type              c_rnti;
  rrc_int_u_rnti_type              u_rnti;
  rrcrb_rnti_update_info_type      rnti_update_info;   /* to hold rnti update info */

  rrc_msg_ver_enum_type msg_version = MSG_REL6;


  rrcsmc_srns_ip_check_status_e_type    ip_check_status = RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE;


  rrc_DL_DCCH_Message *dcch_msg_ptr ;     /* Pointer to the RB Release message */

  /* Store parameter IEs */
  rrc_RadioBearerRelease_r6_IEs* rb_release_ptr = NULL;
  uint32 local_rb_idx=0;

  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  /* Get the pointer to the actual RB Release message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerRelease);


  /* Store Msg Ptr in a local variable */
  rb_release_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.
    u.criticalExtensions.u.r6.radioBearerRelease_r6;



  if (TRUE == rrcrbr_validate_srns_relocation_info(msg_ptr, msg_version))
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
        break;

      case RRCSMC_SRNS_SECURITY_INFO_INVALID:
      case RRCSMC_INCOMPATIBLE_SECURITY_RECONFIGURATION:
        if (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier
                       == rbr.tr_ids.rejected_transaction)
        {
          WRRC_MSG1_HIGH("Invalid security config-rejected tr-id%d match!",
                   rbr.tr_ids.rejected_transaction);
          /* Nothing else to do, just ignore the message and return */
        } 
        else
        {
          /* send a reconfig failure messge with invalid
          configuration and 
          continue the same substate with any ongoing procedure */
          rbr.tr_ids.rejected_transaction = 
            (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
          if (ip_check_status == RRCSMC_SRNS_SECURITY_INFO_INVALID)
          {
            rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          }
          else
          {
            rbr.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
          }
          /*Check whether Cell Update is going on */
          if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
               (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
          {
            if(rbr.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
            {
              send_rbr_failure_message( rbr.status.failure_status,
                                       rbr.status.prot_err_cause );
              rbr.status.failure_status = RRCRB_MSG_VALID;
            }
            else
            {
              WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
              (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBR);
              /* Wait until Cell update procedure is completed */
              rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
            }
          }
          else
          {
            send_rbr_failure_message( rbr.status.failure_status,
                                            rbr.status.prot_err_cause );
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
      /*do not clear the procedure or change the rbr substate because
      some other rbe procedure might be in progress. Just return from
      here */
      return;
    }
  }


  if ((rrcllc_get_ordered_config_status() != OC_NOT_SET) ||
      (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
      (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
  {
     MSG_HIGH("Incomatible simultaneous reconfig! Rejecting RBR message,transaction_id:%d,accepted_transaction:%d",
         msg_ptr->u.later_than_r3.rrc_TransactionIdentifier,rbr.tr_ids.accepted_transaction,rbr.tr_ids.rejected_transaction );
     if((msg_ptr->u.later_than_r3.rrc_TransactionIdentifier != rbr.tr_ids.accepted_transaction) && 
         (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier != rbr.tr_ids.rejected_transaction))
     {
       /* We have received a RBR message with a new transaction id.
       This message should be in our rejected transaction
       list */
       rbr.tr_ids.rejected_transaction = 
         (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
       rbr.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;

       /*Check whether Cell Update is going on */
       if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
            (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
       {
         if(rbr.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
         {
           send_rbr_failure_message( rbr.status.failure_status,
                                     rbr.status.prot_err_cause );
           rbr.status.failure_status = RRCRB_MSG_VALID;
         }
         else
         {
            WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
           (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBR);
           /* Wait until Cell update procedure is completed */
           rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
         }
       }
       else
       {
         send_rbr_failure_message( rbr.status.failure_status,
                                         rbr.status.prot_err_cause );
         /*continue the same substate */ 
       }
     }
     return;
  }

  /* We have received a RB release message with a new transaction id.
  Accept the transaction Id */
  rbr.tr_ids.accepted_transaction =
     msg_ptr->u.later_than_r3.rrc_TransactionIdentifier;

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  validate_rbrelease_r6(msg_ptr);

  if ( rbr.status.failure_status != RRCRB_MSG_VALID )
  {
    /* RB Message was invalid. */
    WRRC_MSG0_HIGH("RBR Message validation failed");

    /* Send the RB Release Failure Message */
    send_rbr_failure_message( rbr.status.failure_status,
                              rbr.status.prot_err_cause
                            );

    rrcrbr_clear_procedure();
    /* Make sure we're back in the initial sub-state */
    rbr.curr_substate = RRCRB_INITIAL;
  }
  /* If the message is valid, proceed further */
  else
  {
    boolean                  additional_srb;
    uecomdef_logchan_e_type  dl_sdu_type = UE_LOGCHAN_NONE;

    WRRC_MSG0_HIGH("Processing RBR Message");
    rrcrbr_process_high_mobility_ind_in_non_critical_ext(msg_ptr,MSG_REL6);
    
    /*store the rb ids tobe released */
    rbr.rbs_tobe_released.is_valid = TRUE;
    rb_info_rel_list_ptr = &rb_release_ptr->rb_InformationReleaseList;
    localrb_count = 0;
	local_rb_idx =0;
    while ( rb_info_rel_list_ptr->n >  local_rb_idx)
    {
      additional_srb = FALSE;

      if((rrclcm_get_dl_lc_type(rrclcm_get_dl_rlc_lc_id(rb_info_rel_list_ptr->elem[local_rb_idx]), &dl_sdu_type)) == RRCLCM_SUCCESS)
      {
        if(dl_sdu_type == UE_LOGCHAN_TM_DCCH_DL)
        {
          additional_srb = TRUE;
        }
      }
      /* check whether rb id is signalling rbid or does not exist in 
       * established rabs 
       */
      if (additional_srb == FALSE)
      { 
        /*check whether rb id is signalling rbid or does not exist in established
        rabs */
        if( rb_info_rel_list_ptr->elem[local_rb_idx] < DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
        {
          /* RB Message was invalid. */
          WRRC_MSG1_HIGH("Invalid RB id:%d in RBR Message", rb_info_rel_list_ptr->elem[local_rb_idx]);
          /* Send the RB Release Failure Message */
          rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          send_rbr_failure_message( rbr.status.failure_status,
                                    rbr.status.prot_err_cause
                                  );
  
          rrcrbr_clear_procedure();
          /* Make sure we're back in the initial sub-state */
          rbr.curr_substate = RRCRB_INITIAL;
          return;
  
        }
        else if( rb_info_rel_list_ptr->elem[local_rb_idx] == DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
        {
          if(RRCLCM_RLC_LC_ID_NOT_FOUND != rrclcm_check_dl_rlc_lc_id(
                                           UE_LOGCHAN_DCCH,
                                           DCCH_DT_LOW_PRI_RADIO_BEARER_ID,
                                           UE_MODE_ACKNOWLEDGED_DATA))
          {
            WRRC_MSG0_HIGH("Release SRB4");
          }
          else
          {
            WRRC_MSG1_HIGH("Ignore Non-exitent rb_id = %d in rbr msg",rb_info_rel_list_ptr->elem[local_rb_idx]);
			local_rb_idx++;
            continue;
          }
        }
        else if (!rrc_find_rb_in_est_rabs(rb_info_rel_list_ptr->elem[local_rb_idx]))
        {
          WRRC_MSG1_HIGH("Ignore Non-exitent rb_id =%d in rbr msg",rb_info_rel_list_ptr->elem[local_rb_idx]);
          local_rb_idx++;
          continue;
        }
      }

      rbr.rbs_tobe_released.rb_id[localrb_count] = rb_info_rel_list_ptr->elem[local_rb_idx];
      localrb_count++;
      local_rb_idx++;
    }
    rbr.rbs_tobe_released.num_rbs = localrb_count;
    WRRC_MSG1_HIGH("Number ofRBs tobe released:%d", localrb_count);

    /*store the rb ids tobe reconfigured */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,
          rrc_RadioBearerRelease_r6_IEs,rb_InformationReconfigList))
    {
      rbr.rbs_tobe_reconfigured.is_valid = TRUE;
            localrb_count = 0;
      while ( rb_release_ptr->rb_InformationReconfigList.n >localrb_count )
      {
      
 	   rb_info_reconfig_ptr = 
			  &rb_release_ptr->rb_InformationReconfigList.elem[localrb_count];
        /* Fill up the RB id and indicate that it needs to be reconfigured */
        rbr.rbs_tobe_reconfigured.rb_info[localrb_count].rb_id= 
          rb_info_reconfig_ptr->rb_Identity;
        rbr.rbs_tobe_reconfigured.rb_info[localrb_count].rb_config = RECONFIG_RB;
        rbr.rbs_tobe_reconfigured.rb_info[localrb_count].rb_config = RECONFIG_RB_OTA;
        rbr.rbs_tobe_reconfigured.rb_info[localrb_count].ul_cfg_mask = rbr.rbs_tobe_reconfigured.rb_info[localrb_count].dl_cfg_mask = 0;
        if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,
			rrc_RB_InformationReconfig_r6,rlc_Info))&&
            (RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_ptr->rlc_Info,
            rrc_RLC_Info_r6,ul_RLC_Mode)))
        {
          rbr.rbs_tobe_reconfigured.rb_info[localrb_count].ul_cfg_mask = MODIFY_RB_CFG;
        }
        if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,
			rrc_RB_InformationReconfig_r6,rlc_Info))&&
            (RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_ptr->rlc_Info,
            rrc_RLC_Info_r6,dl_RLC_Mode)))
        {
          rbr.rbs_tobe_reconfigured.rb_info[localrb_count].dl_cfg_mask = MODIFY_RB_CFG;
        }
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,
			rrc_RB_InformationReconfig_r6,rb_StopContinue))
        {
          MSG_HIGH("rb-reconfig-info rb %d to %d[0:STOP/1:CONTINUE rb_count %d", 
              rbr.rbs_tobe_reconfigured.rb_info[localrb_count].rb_id, rb_info_reconfig_ptr->rb_StopContinue,localrb_count);
          /* over-ride rb_config with STOP or continue */
          if (rrc_RB_StopContinue_stopRB == rb_info_reconfig_ptr->rb_StopContinue)
          {
            rbr.rbs_tobe_reconfigured.rb_info[localrb_count].ul_cfg_mask |= STOP_RB_CFG;
            rbr.rbs_tobe_reconfigured.rb_info[localrb_count].dl_cfg_mask |= STOP_RB_CFG;
          }
          else if (rrc_RB_StopContinue_continueRB == rb_info_reconfig_ptr->rb_StopContinue)
          {
            rbr.rbs_tobe_reconfigured.rb_info[localrb_count].ul_cfg_mask |= CONTINUE_RB_CFG;
            rbr.rbs_tobe_reconfigured.rb_info[localrb_count].dl_cfg_mask |= CONTINUE_RB_CFG;
            
          }
        }

        localrb_count++;
        /* Go to the next reconfigured RB */
      }

      rbr.rbs_tobe_reconfigured.num_rbs= localrb_count;
      WRRC_MSG1_HIGH("Number ofRBs tobe reconfigured:%d", localrb_count);
    }

    /*store the rb ids tobe affected */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,
          rrc_RadioBearerRelease_r6_IEs,rb_InformationAffectedList))
    {
      rbr.rbs_tobe_affected.is_valid = TRUE;
      rb_info_affected_list_ptr = 
      &rb_release_ptr->rb_InformationAffectedList;
      localrb_count = 0;
      while ( rb_info_affected_list_ptr->n > localrb_count)
      {
        /*check whether rb id is signalling rbid or does not exist in established
        rabs */
        if(( rb_info_affected_list_ptr->elem[localrb_count].rb_Identity > DCCH_DT_LOW_PRI_RADIO_BEARER_ID)&&
           (! rrc_find_rb_in_est_rabs(rb_info_affected_list_ptr->elem[localrb_count].rb_Identity)))
        {
          /* RB Message was invalid. */
          WRRC_MSG1_HIGH("Invalid RB id:%d in RBR Message", rb_info_affected_list_ptr->elem[localrb_count].rb_Identity);
          /* Send the RB Release Failure Message */
          rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          send_rbr_failure_message( rbr.status.failure_status,
                                    rbr.status.prot_err_cause
                                  );

          rrcrbr_clear_procedure();
          /* Make sure we're back in the initial sub-state */
          rbr.curr_substate = RRCRB_INITIAL;
          return;

        }
        rbr.rbs_tobe_affected.rb_id[localrb_count] = 
        rb_info_affected_list_ptr->elem[localrb_count].rb_Identity;
        localrb_count++;
      }
      rbr.rbs_tobe_affected.num_rbs= localrb_count;
      WRRC_MSG1_HIGH("Number ofRBs tobe affected:%d", localrb_count);
    }


    /* If there is a signalling connection to be released,
    or if there are individual RBs to be released, we
    will be releasing some RBs. */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,
        rrc_RadioBearerRelease_r6_IEs,signallingConnectionRelIndication))
    {
      rrcrbr_mark_cn_doamin_to_be_closed(rb_release_ptr->signallingConnectionRelIndication);
    
      for ( rab_count = 0; rab_count< MAX_RAB_TO_SETUP; rab_count++ )
      {
        if ( (rrc_est_rabs.rabs[rab_count].cn_domain == rbr.cn_to_release) &&
             (rrc_est_rabs.rabs[rab_count].num_rbs_for_rab > 0) )
        {
          rab_found = TRUE;
          for ( rb_count=0; rb_count<rrc_est_rabs.rabs[rab_count].num_rbs_for_rab; rb_count++ )
          {
            rb_rel_srch = FALSE;
            for ( localrb_count=0; localrb_count<rbr.rbs_tobe_released.num_rbs; localrb_count++ )
            {
              if ( rrc_est_rabs.rabs[rab_count].rb_for_rab[rb_count].rb_id == 
                   rbr.rbs_tobe_released.rb_id[localrb_count] )
              {
                rb_rel_srch = TRUE;
                break;
              }
            }
            if ( rb_rel_srch == FALSE ) // est rabs has an rb_id not in the rb release list
            {
              /* send failure message */
              rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
              send_rbr_failure_message( rbr.status.failure_status,
                                        rbr.status.prot_err_cause);
              /* Go back to initial substate */
              rbr.curr_substate = RRCRB_INITIAL;
              /* Then clear all local variables that may
              need to be cleared */
              rrcrbr_clear_procedure();
              return;
            }
          }/* rb_count loop */       
        } /*end of if loop */
      }/* rab_count loop */
      /*if none of the rabs in the establishedrabs have rb information*/
      if ( rab_found==FALSE )
      {
        WRRC_MSG1_HIGH("RB lookup in est_rabs for cn:%d failed", rbr.cn_to_release );
        /* send failure message */
        rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause);
        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;
        /* Then clear all local variables that may
        need to be cleared */
        rrcrbr_clear_procedure();
        return;

      }
    }
    /* Check if there is CN information present. If so, we need
    to send the information to the Cell Change manager */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, rrc_RadioBearerRelease_r6_IEs,cn_InformationInfo))
    {
      WRRC_MSG0_HIGH("Saving CN info for NAS");
      rbr.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(
        &(rb_release_ptr->cn_InformationInfo) );
      if (rbr.mm_cmd_ptr != NULL)
      {
        /* set cn_info_saved flag */
        rbr.cn_info_saved = TRUE;
      }
    }

    /* Indicate layer1 if rb release message is going to release voice RAB */
    rrcrbr_indicate_layer1_if_config_msg_release_voice_rab();

    rrcrb_update_mac_rab_status(RRC_PROCEDURE_RBR, RRC_CS_DOMAIN_CN_ID,
                                &rb_release_ptr->rb_InformationReleaseList,
                                FALSE);

    /* Set ORDERED_CONFIG so that LLC can process the
    necessary parameters. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_RBR,
                                           rbr.state_indicated_by_msg,
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

        current_state = rrc_get_state(); 
        /* Populate the local variables to indicate if Re-direction info is present */
        directed_cell_info.freq_info_present = FALSE;
        directed_cell_info.pscr_present = FALSE; 
        if((current_state==RRC_STATE_CELL_FACH) && 
               ((rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH)
               ||(rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH)
               ||(rbr.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, 
                 rrc_RadioBearerRelease_r6_IEs,frequencyInfo)) &&
                (RRC_CHECK_COMMON_MSG_TYPE(rb_release_ptr->frequencyInfo.modeSpecificInfo,
                rrc_FrequencyInfo_modeSpecificInfo_fdd)))
          {
            directed_cell_info.freq_info_present = TRUE;
            directed_cell_info.uarfcn_dl  = rb_release_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
          }
          
          if (rrcrbr_is_pcpich_info_present(msg_ptr,MSG_REL6))
          {
            if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, rrc_RadioBearerRelease_r6_IEs,dl_InformationPerRL_List)) &&
              (rb_release_ptr->dl_InformationPerRL_List.n != 0))
            {
              
              directed_cell_info.pscr_present = TRUE;
              directed_cell_info.pscr = rb_release_ptr->dl_InformationPerRL_List.elem[0].
                modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
            }
          }   
           /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
                 FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
          if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
            (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
          {
            WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
            if(rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH)
            {
              rrcrbr_send_initiate_cu_flag = TRUE;
              rrcrbr_delete_crnti          = TRUE;
            }
            else
            {
              rbr.directed_cell_info = TRUE;
            }
          }  
        }
        if (
            (current_state==rbr.state_indicated_by_msg) ||
            ((current_state==RRC_STATE_CELL_FACH) && 
             (rbr.state_indicated_by_msg==RRC_STATE_CELL_DCH))
            )
        {
          /* CELL_DCH->CELL_DCH or CELL_FACH->CELL_FACH or 
          CELL_FACH->CELL_DCH case. We can release the
          Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */
          /*check whether any frequency info is present in the message
          and transition is CELL_FACH->CELL_FACH. In this case
          initiate a cell update after channel config is successfully completed */
          if ((current_state==RRC_STATE_CELL_FACH)&& 
             (rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH))
          {
             /*call the function that evaluates the directed cell info and set the flags accordingly */
            if ((rrcrb_validate_directed_cell_info (directed_cell_info)== FALSE)
                &&(rrcrbr_send_initiate_cu_flag == FALSE))
            {
              if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,
                 rrc_RadioBearerRelease_r6_IEs,new_C_RNTI))
              {
                rbr.new_crnti_valid = TRUE;
                rrc_translate_crnti(&rb_release_ptr->new_C_RNTI, 
                                    &c_rnti);
                rbr.new_crnti = (uint16)c_rnti;
              }
              else
              {
                rbr.new_crnti_valid = FALSE;
              }

              rrcllcoc_update_toc_with_oc_from_fach();
              /*Initiate the cell selection*/
              rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBR,
                                                     RRC_TRANSITION_FROM_FACH,
                                                     rbr.state_indicated_by_msg);
  
  
           
              
              rbr.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
              /* exit from the switch case */
              break;
     
            }
          } /* if CELL_FACH->CELL_FACH reconfiguration */

          /* Call the function that creates and sends a channel
            config request to LLC. */
          rrcrbr_send_channel_config_req();

          /* Change the substate since we're waiting for the
             confirmation from LLC for the channel configs */
          rbr.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH))
        {
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,
             rrc_RadioBearerRelease_r6_IEs,new_C_RNTI))
          {
            rbr.new_crnti_valid = TRUE;
            rrc_translate_crnti(&rb_release_ptr->new_C_RNTI, 
                                &c_rnti);
            rbr.new_crnti = (uint16)c_rnti;
          }
          else
          {
            rbr.new_crnti_valid = FALSE;
          }

          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBR,
                                                  RRC_TRANSITION_FROM_DCH,
                                                  RRC_STATE_CELL_FACH);
          rbr.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

          /* Start T_305 [periodic cell update] timer */
          rrccu_start_t_305_timer();		  
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                 (rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH))
        {
          /*store the directed cell info */
           rbr.directed_cell_info = rrcrbr_is_pcpich_info_present(msg_ptr,MSG_REL6);

           /*Send the rbr complete msg on old configuration */
           send_rbr_complete_message(TRUE);
           /*wait for L2ack */
           rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
           rrcllc_print_trans_chnl_rate();
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (rbr.state_indicated_by_msg==RRC_STATE_URA_PCH))
        {
          /*Send the RBR complete msg on old configuration */
          send_rbr_complete_message(TRUE);
          /*wait for L2ack */
          rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
          rrcllc_print_trans_chnl_rate();
        }
        else if((current_state==RRC_STATE_CELL_FACH) && 
                ((rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
                 (rbr.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          /*call the function that evaluates the directed cell info and set the flags accordingly */
          if ((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE)
            && (rbr.directed_cell_info == FALSE))
          {
            if(rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH)
            {
              rbr.directed_cell_info = rrcrbr_is_pcpich_info_present(msg_ptr,MSG_REL6);
            }
            rbr.initiate_cell_selection_trans_from_fach  = TRUE;
          }
    

          /* check whether CPICH info is present or not. Store this
          info  */
         // rbr.directed_cell_info = rrcrbr_is_pcpich_info_present(msg_ptr);
          /*Send the RRCTCTC complete msg on old configuration */
          /* check whether new C-RNTI was present in the message. In that case
          we have to configure MAC with new C-RNTI before sending the response 
          message */
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,
             rrc_RadioBearerRelease_r6_IEs,new_C_RNTI))
          {
            rrc_translate_crnti(&rb_release_ptr->new_C_RNTI, 
                                &c_rnti);
            /* Send a RNTI Update Request */
            rnti_update_info.crnti_action    = RRC_RNTI_UPDATE;
            rnti_update_info.crnti           = (uint16)c_rnti;
            /* check whether URNTI is also present in the message */
            if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,
             rrc_RadioBearerRelease_r6_IEs,new_U_RNTI))
            {
              rrc_translate_urnti(&rb_release_ptr->new_U_RNTI, 
                                  &u_rnti);
              rnti_update_info.urnti_action    = RRC_RNTI_UPDATE; 
              rnti_update_info.urnti           = u_rnti; 
            }
            else
            {
              rnti_update_info.urnti_action    = RRC_RNTI_NOCHANGE; 
            }
            rnti_update_info.procedure            = RRC_PROCEDURE_RBR;
            rnti_update_info.rlc_re_establish_srb = FALSE;
            rnti_update_info.rlc_re_establish_rab = FALSE;
            rnti_update_info.cnf_reqd             = TRUE;
            rrcrb_send_rnti_update_req(rnti_update_info);
            /* wait for the RNTI update confirm before sending the response
            message */
            rbr.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
          }
          else 
          {
            send_rbr_complete_message(TRUE);
            /*wait for L2ack */
            rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
            rrcllc_print_trans_chnl_rate();
          }
        }
        break;

      case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

        rbr.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
        /* Send the RB Release Failure Message */
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

        rbr.status.failure_status =RRCRB_MSG_UNSUPPORTED_CONFIG;
        /* Send the RB Release Failure Message */
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
		
      case ORDERED_CONFIG_CONFIGURATION_INVALID:

        rbr.status.failure_status =RRCRB_MSG_INVALID_CONFIG;
        /* Send the RB Release Failure Message */
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_NOT_SET_OTHER:
        WRRC_MSG0_HIGH("LLC returned other failure!");
        /* When LLC sends this, there is a serious problem
        and we'll go back to disconnected state. No need
        to send the failure message here, we can just
        go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
      case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("RBR processing deferred due to LTA!");

        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
      default:

        WRRC_MSG0_ERROR("Unknown return value from LLC");
        /* Unknown error, just go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
    }/* end switch */

    if ( ordered_config_err_occured == TRUE )
    {
        /* No need to continue. Clear the procedure
        variables and return. */
        rrcrbr_clear_procedure();
        return;
    }


    if(rbr.rbr_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }
  }/* end if - valid rb message */
}

/*===========================================================================

FUNCTION   rrcrbr_check_and_get_primary_plmn_id

DESCRIPTION

  This function will check whether primary plmn id is present 
  in r3,r5 path of RBR message. If present then it will return 
  primary plmn id.
        
DEPENDENCIES
  None.

RETURN VALUE
TRUE : IF Primary PLMN I.D is present in RBR message in r3, r5 path
FFALSE: Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
static boolean rrcrbr_check_and_get_primary_plmn_id
(
  /* Pointer to the RBR message */
  rrc_RadioBearerRelease *msg_ptr,
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
  if (MSG_REL99 == msg_version)
  {
      /*Check to see if primary PLMN I.D is present in RBR message ext
       * rrc_RadioBearerRelease-v690ext which is present in 
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
          radioBearerRelease_v690ext.m.primary_plmn_IdentityPresent)
        )
      {
        primary_plmn_Id_present = TRUE;
        *primary_plmn_Identity = msg_ptr->u.r3.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
            radioBearerRelease_v690ext.primary_plmn_Identity;
      }
  }
  else if (MSG_REL5 == msg_version)
  {
      /*Check to see if primary PLMN I.D is present in RBR message ext
       * rrc_RadioBearerRelease-v690ext which is present in 
       * v690NonCriticalExtensions in r5 path
       */
      if((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.m.v690NonCriticalExtensionsPresent)&&
          (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.
           v690NonCriticalExtensions.radioBearerRelease_v690ext.m.primary_plmn_IdentityPresent)
        )
      {
        primary_plmn_Id_present = TRUE;
        *primary_plmn_Identity = msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.
           v690NonCriticalExtensions.radioBearerRelease_v690ext.primary_plmn_Identity;
      }
  }
  WRRC_MSG2_HIGH("NWS:RBR msg contains primary_plmn_Id %d (0:Not present,1: Present)for msg_version:%d(0-MSG_REL99,1-MSG_REL5,2-MSG_REL6,3-MSG_REL7,4-MSG_REL8,5-MSG_REL9,6-MSG_REL10,other-MSG_INVALID)",primary_plmn_Id_present,msg_version);
  return primary_plmn_Id_present;
}
/*===========================================================================

FUNCTION   RRCRBR_PROCESS_RBRELEASE_R5

DESCRIPTION

  This function processes the received Rel 5 RB Release Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a Radio Bearer Release Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to release the Radio Bearers given by the message.
    
    If ORDERED_CONFIG could not be set, a Radio Bearer Release Failure message
    will be sent to UTRAN.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbr_process_rbrelease_r5
(
  rrc_cmd_type *cmd_ptr
)
{
  rrc_RB_InformationReleaseList  *rb_info_rel_list_ptr;
  rrc_RB_InformationAffectedList_r5 *rb_info_affected_list_ptr;
  rrc_RadioBearerRelease *msg_ptr;  /* Pointer to the RB Release message        */
  rrcrb_directed_cell_info_type directed_cell_info; /* directed cell info struct */

  rrcllc_oc_status_e_type oc_status;/* To hold ordered_config status          */
  boolean ordered_config_err_occured = FALSE; /* Used to indicate if an error
                                    occured while setting ordered_config. */
  boolean rb_rel_srch = FALSE; 
  boolean rab_found = FALSE; 
  rrc_state_e_type current_state;       /* current RRC State  */
  uint32 rab_count, rb_count, localrb_count;
  /* Local C_RNTI value */
  rrc_int_c_rnti_type              c_rnti;
  rrc_int_u_rnti_type              u_rnti;
  rrcrb_rnti_update_info_type      rnti_update_info;   /* to hold rnti update info */
  uint32 local_idx=0;
  rrc_msg_ver_enum_type msg_version = MSG_REL5;


  rrcsmc_srns_ip_check_status_e_type    ip_check_status = RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE;


  rrc_DL_DCCH_Message *dcch_msg_ptr ;     /* Pointer to the RB Release message */

  /* Store parameter IEs */
  rrc_RadioBearerRelease_r5_IEs* rb_release_ptr = NULL;

   /*Boolean used to indicate primary plmn i.d is present in OTA msg*/
  boolean primary_plmn_Id_present = FALSE;
   /*Variable to hold primary plmn i.d if present in OTA msg*/
  rrc_PLMN_Identity primary_plmn_Identity;


  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  /* Get the pointer to the actual RB Release message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerRelease);

  if ((msg_ptr->u.later_than_r3.criticalExtensions.t == 
       T_rrc_RadioBearerRelease_criticalExtensions_5_criticalExtensions) &&
      (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.t == 
       T_rrc_RadioBearerRelease_criticalExtensions_4_r5))
  {
    /* Store Msg Ptr in a local variable */
    rb_release_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.
                  u.r5.radioBearerRelease_r5;

  } 
  else
  {
    WRRC_MSG0_HIGH("Unsupported RB Release t");
    rbr.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;

    /*Check whether Cell Update is going on */
    if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
         (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
    {
      if(rbr.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
      {
        WRRC_MSG2_HIGH("rbr subsstate(%d) Send failure with %d cause", rbr.curr_substate, rbr.status.failure_status);
        send_rbr_failure_message( rbr.status.failure_status,
                                 rbr.status.prot_err_cause );
        rbr.status.failure_status = RRCRB_MSG_VALID;
      }
      else
      {
        WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
        (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBR);
        /* Wait until Cell update procedure is completed */
        rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      }
    }
    else
    {
      send_rbr_failure_message( rbr.status.failure_status,
                                      rbr.status.prot_err_cause );
      /*continue the same substate */ 
    }
    return;
  }

  if (TRUE == rrcrbr_validate_srns_relocation_info(msg_ptr, msg_version))
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
        break;

      case RRCSMC_SRNS_SECURITY_INFO_INVALID:
      case RRCSMC_INCOMPATIBLE_SECURITY_RECONFIGURATION:
        if (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier
                       == rbr.tr_ids.rejected_transaction)
        {
          WRRC_MSG1_HIGH("Invalid security config-rejected tr-id%d match!",
                   rbr.tr_ids.rejected_transaction);
          /* Nothing else to do, just ignore the message and return */
        } 
        else
        {
          /* send a reconfig failure messge with invalid
          configuration and 
          continue the same substate with any ongoing procedure */
          rbr.tr_ids.rejected_transaction = 
            (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
          if (ip_check_status == RRCSMC_SRNS_SECURITY_INFO_INVALID)
          {
            rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          }
          else
          {
            rbr.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
          }
          /*Check whether Cell Update is going on */
          if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
               (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
          {
            if(rbr.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
            {
              send_rbr_failure_message( rbr.status.failure_status,
                                       rbr.status.prot_err_cause );
              rbr.status.failure_status = RRCRB_MSG_VALID;
            }
            else
            {
              WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
              (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBR);
              /* Wait until Cell update procedure is completed */
              rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
            }
          }
          else
          {
            send_rbr_failure_message( rbr.status.failure_status,
                                            rbr.status.prot_err_cause );
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
      /*do not clear the procedure or change the rbr substate because
      some other rbe procedure might be in progress. Just return from
      here */
      return;
    }
  }


  if ((rrcllc_get_ordered_config_status() != OC_NOT_SET) ||
      (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
      (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
  {
     MSG_HIGH("Incomatible simultaneous reconfig! Rejecting RBR message,transaction_id:%d,accepted_transaction:%d",
         msg_ptr->u.later_than_r3.rrc_TransactionIdentifier,rbr.tr_ids.accepted_transaction,rbr.tr_ids.rejected_transaction );
     if((msg_ptr->u.later_than_r3.rrc_TransactionIdentifier != rbr.tr_ids.accepted_transaction) && 
         (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier != rbr.tr_ids.rejected_transaction))
     {
       /* We have received a RBR message with a new transaction id.
       This message should be in our rejected transaction
       list */
       rbr.tr_ids.rejected_transaction = 
         (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
       rbr.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;

       /*Check whether Cell Update is going on */
       if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
            (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
       {
         WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
         if (SUCCESS != rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBR))
         {
           WRRC_MSG0_ERROR("RBR failed to register with CU");
         }
         /* Wait until Cell update procedure is completed */
         rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF; 
       }
       else
       {
         send_rbr_failure_message( rbr.status.failure_status,
                                         rbr.status.prot_err_cause );
         /*continue the same substate */ 
       }
     }
     return;
  }

  /* We have received a RB release message with a new transaction id.
  Accept the transaction Id */
  rbr.tr_ids.accepted_transaction =
     msg_ptr->u.later_than_r3.rrc_TransactionIdentifier;

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  validate_rbrelease_r5(msg_ptr);

  if ( rbr.status.failure_status != RRCRB_MSG_VALID )
  {
    /* RB Message was invalid. */
    WRRC_MSG0_HIGH("RBR Message validation failed");

    /* Send the RB Release Failure Message */
    send_rbr_failure_message( rbr.status.failure_status,
                              rbr.status.prot_err_cause
                            );

    rrcrbr_clear_procedure();
    /* Make sure we're back in the initial sub-state */
    rbr.curr_substate = RRCRB_INITIAL;
  }
  /* If the message is valid, proceed further */
  else
  {
    boolean                  additional_srb;
    uecomdef_logchan_e_type  dl_sdu_type = UE_LOGCHAN_NONE;

    WRRC_MSG0_HIGH("Processing RBR Message");

    rrcrbr_process_high_mobility_ind_in_non_critical_ext(msg_ptr,MSG_REL5);
    
    /*store the rb ids tobe released */
    rbr.rbs_tobe_released.is_valid = TRUE;
    rb_info_rel_list_ptr = &rb_release_ptr->rb_InformationReleaseList;
    localrb_count = 0;
	local_idx =0;
    while ( rb_info_rel_list_ptr->n  > local_idx)
    {
      additional_srb = FALSE;

      if((rrclcm_get_dl_lc_type(rrclcm_get_dl_rlc_lc_id(rb_info_rel_list_ptr->elem[local_idx]), &dl_sdu_type)) == RRCLCM_SUCCESS)
      {
        if(dl_sdu_type == UE_LOGCHAN_TM_DCCH_DL)
        {
          additional_srb = TRUE;
        }
      }

      /*check whether rb id is signalling rbid or does not exist in established
      rabs */
      if (additional_srb == FALSE)
      { 
        if((rb_info_rel_list_ptr->elem[local_idx] < DCCH_DT_LOW_PRI_RADIO_BEARER_ID))
        {
          /* RB Message was invalid. */
          WRRC_MSG1_HIGH("Invalid RB id:%d in RBR Message", rb_info_rel_list_ptr->elem[local_idx]);
          /* Send the RB Release Failure Message */
          rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          send_rbr_failure_message( rbr.status.failure_status,
                                    rbr.status.prot_err_cause
                                  );

          rrcrbr_clear_procedure();
          /* Make sure we're back in the initial sub-state */
          rbr.curr_substate = RRCRB_INITIAL;
          return;

        }
        else if( rb_info_rel_list_ptr->elem[local_idx] == DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
        {
          if(RRCLCM_RLC_LC_ID_NOT_FOUND != rrclcm_check_dl_rlc_lc_id(
                                           UE_LOGCHAN_DCCH,
                                           DCCH_DT_LOW_PRI_RADIO_BEARER_ID,
                                           UE_MODE_ACKNOWLEDGED_DATA))
          {
            WRRC_MSG0_HIGH("Release SRB4");
          }
          else
          {
            WRRC_MSG1_HIGH("Ignore Non-exitent rb_id = %d in rbr msg",rb_info_rel_list_ptr->elem[local_idx]);
            local_idx++;
            continue;
          }
        }
        else if(!rrc_find_rb_in_est_rabs(rb_info_rel_list_ptr->elem[local_idx]))
        {
          WRRC_MSG1_HIGH("Ignore Non-exitent rb_id in rbr msg",rb_info_rel_list_ptr->elem[local_idx]);
          local_idx++;
          continue;
        }
      }

      rbr.rbs_tobe_released.rb_id[localrb_count] = rb_info_rel_list_ptr->elem[local_idx];
      localrb_count++;
      local_idx++;
    }
    rbr.rbs_tobe_released.num_rbs = localrb_count;
    WRRC_MSG1_HIGH("Number ofRBs tobe released:%d", localrb_count);

    /*store the rb ids tobe affected */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,
          rrc_RadioBearerRelease_r5_IEs,rb_InformationAffectedList))
    {
      rbr.rbs_tobe_affected.is_valid = TRUE;
      rb_info_affected_list_ptr = 
      &rb_release_ptr->rb_InformationAffectedList;
      localrb_count = 0;
      while ( rb_info_affected_list_ptr->n > localrb_count )
      {
        /*check whether rb id is signalling rbid or does not exist in established
        rabs */
        if(( rb_info_affected_list_ptr->elem[localrb_count].rb_Identity > DCCH_DT_LOW_PRI_RADIO_BEARER_ID)&&
           (! rrc_find_rb_in_est_rabs(rb_info_affected_list_ptr->elem[localrb_count].rb_Identity)))
        {
          /* RB Message was invalid. */
          WRRC_MSG1_HIGH("Invalid RB id:%d in RBR Message", rb_info_affected_list_ptr->elem[localrb_count].rb_Identity);
          /* Send the RB Release Failure Message */
          rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          send_rbr_failure_message( rbr.status.failure_status,
                                    rbr.status.prot_err_cause
                                  );

          rrcrbr_clear_procedure();
          /* Make sure we're back in the initial sub-state */
          rbr.curr_substate = RRCRB_INITIAL;
          return;

        }
        rbr.rbs_tobe_affected.rb_id[localrb_count] = 
        rb_info_affected_list_ptr->elem[localrb_count].rb_Identity;
        localrb_count++;
      }
      rbr.rbs_tobe_affected.num_rbs= localrb_count;
      WRRC_MSG1_HIGH("Number ofRBs tobe affected:%d", localrb_count);
    }


    /* If there is a signalling connection to be released,
    or if there are individual RBs to be released, we
    will be releasing some RBs. */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,
        rrc_RadioBearerRelease_r5_IEs,signallingConnectionRelIndication))
    {
      rrcrbr_mark_cn_doamin_to_be_closed(rb_release_ptr->signallingConnectionRelIndication);
  
      for ( rab_count = 0; rab_count< MAX_RAB_TO_SETUP; rab_count++ )
      {
        if ( (rrc_est_rabs.rabs[rab_count].cn_domain == rbr.cn_to_release) &&
             (rrc_est_rabs.rabs[rab_count].num_rbs_for_rab > 0) )
        {
          rab_found = TRUE;
          for ( rb_count=0; rb_count<rrc_est_rabs.rabs[rab_count].num_rbs_for_rab; rb_count++ )
          {
            rb_rel_srch = FALSE;
            for ( localrb_count=0; localrb_count<rbr.rbs_tobe_released.num_rbs; localrb_count++ )
            {
              if ( rrc_est_rabs.rabs[rab_count].rb_for_rab[rb_count].rb_id == 
                   rbr.rbs_tobe_released.rb_id[localrb_count] )
              {
                rb_rel_srch = TRUE;
                break;
              }
            }
            if ( rb_rel_srch == FALSE ) // est rabs has an rb_id not in the rb release list
            {
              /* send failure message */
              rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
              send_rbr_failure_message( rbr.status.failure_status,
                                        rbr.status.prot_err_cause);
              /* Go back to initial substate */
              rbr.curr_substate = RRCRB_INITIAL;
              /* Then clear all local variables that may
              need to be cleared */
              rrcrbr_clear_procedure();
              return;
            }
          }/* rb_count loop */       
        } /*end of if loop */
      }/* rab_count loop */
      /*if none of the rabs in the establishedrabs have rb information*/
      if ( rab_found==FALSE )
      {
        WRRC_MSG1_HIGH("RB lookup in est_rabs for cn:%d failed", rbr.cn_to_release );
        /* send failure message */
        rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause);
        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;
        /* Then clear all local variables that may
        need to be cleared */
        rrcrbr_clear_procedure();
        return;

      }
    }

    /* Check if there is CN information present. If so, we need
    to send the information to the Cell Change manager */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, rrc_RadioBearerRelease_r5_IEs,cn_InformationInfo))
    {
      WRRC_MSG0_HIGH("Saving CN info for NAS");
    
      /*Check to see if primary PLMN I.D is present in RBR message ext
       * rrc_RadioBearerRelease-v690ext which is present in 
       * v690NonCriticalExtensions
       */
       primary_plmn_Id_present = rrcrbr_check_and_get_primary_plmn_id(msg_ptr,
                                                                  &primary_plmn_Identity,MSG_REL5);
    
      rbr.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info(
                     &(rb_release_ptr->cn_InformationInfo) 
                     ,primary_plmn_Id_present
                     ,&primary_plmn_Identity
        );
      if (rbr.mm_cmd_ptr != NULL)
      {
        /* set cn_info_saved flag */
        rbr.cn_info_saved = TRUE;
      }
    }

    /* Indicate layer1 if rb release message is going to release voice RAB */
    rrcrbr_indicate_layer1_if_config_msg_release_voice_rab();

    rrcrb_update_mac_rab_status(RRC_PROCEDURE_RBR, RRC_CS_DOMAIN_CN_ID,
                                &rb_release_ptr->rb_InformationReleaseList,
                                FALSE);

    /* Set ORDERED_CONFIG so that LLC can process the
    necessary parameters. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_RBR,
                                           rbr.state_indicated_by_msg,
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

        current_state = rrc_get_state(); 
       /* Populate the local variables to indicate if Re-direction info is present */
        directed_cell_info.freq_info_present = FALSE;
        directed_cell_info.pscr_present = FALSE; 
        if((current_state==RRC_STATE_CELL_FACH) && 
               ((rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH)
               ||(rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH)
               ||(rbr.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, 
                  rrc_RadioBearerRelease_r5_IEs,frequencyInfo)) &&
                 (RRC_CHECK_COMMON_MSG_TYPE(rb_release_ptr->frequencyInfo.modeSpecificInfo,
                 rrc_FrequencyInfo_modeSpecificInfo_fdd)))
          {
            directed_cell_info.freq_info_present = TRUE;
            directed_cell_info.uarfcn_dl  = rb_release_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
          }
         
          if (rrcrbr_is_pcpich_info_present(msg_ptr,MSG_REL5))
          {
            if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr, rrc_RadioBearerRelease_r5_IEs,dl_InformationPerRL_List)) &&
              (rb_release_ptr->dl_InformationPerRL_List.n != 0))
            {
              
              directed_cell_info.pscr_present = TRUE;
              directed_cell_info.pscr = rb_release_ptr->dl_InformationPerRL_List.elem[0].
                modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
            }
          }      
          /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
                 FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
          if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
            (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
          {
            WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
            if(rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH)
            {
              rrcrbr_send_initiate_cu_flag = TRUE;
              rrcrbr_delete_crnti          = TRUE;
            }
            else
            {
              rbr.directed_cell_info = TRUE;
            }
          }  
        }
        if (
            (current_state==rbr.state_indicated_by_msg) ||
            ((current_state==RRC_STATE_CELL_FACH) && 
             (rbr.state_indicated_by_msg==RRC_STATE_CELL_DCH))
            )
        {
          /* CELL_DCH->CELL_DCH or CELL_FACH->CELL_FACH or 
          CELL_FACH->CELL_DCH case. We can release the
          Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */
          /*check whether any frequency info is present in the message
          and transition is CELL_FACH->CELL_FACH. In this case
          initiate a cell update after channel config is successfully completed */
          if ((current_state==RRC_STATE_CELL_FACH)&& 
             (rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH))
          {
             /*call the function that evaluates the directed cell info and set the flags accordingly */
            if ((rrcrbr_send_initiate_cu_flag == FALSE) &&
                (rrcrb_validate_directed_cell_info (directed_cell_info)== FALSE))
            {
              if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,
              rrc_RadioBearerRelease_r5_IEs,new_C_RNTI))
              {
                rbr.new_crnti_valid = TRUE;
                rrc_translate_crnti(&rb_release_ptr->new_C_RNTI, 
                                    &c_rnti);
                rbr.new_crnti = (uint16)c_rnti;
              }
              else
              {
                rbr.new_crnti_valid = FALSE;
              }
   
              rrcllcoc_update_toc_with_oc_from_fach();
              /*Initiate the cell selection*/
              rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBR,
                                                       RRC_TRANSITION_FROM_FACH,
                                                       rbr.state_indicated_by_msg);

              rbr.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

              /* exit from the switch case */
              break;
     
            }
          } /* if CELL_FACH->CELL_FACH reconfiguration */

          /* Call the function that creates and sends a channel
            config request to LLC. */
          rrcrbr_send_channel_config_req();

          /* Change the substate since we're waiting for the
             confirmation from LLC for the channel configs */
          rbr.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH))
        {
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,
             rrc_RadioBearerRelease_r5_IEs,new_C_RNTI))
          {
            rbr.new_crnti_valid = TRUE;
            rrc_translate_crnti(&rb_release_ptr->new_C_RNTI, 
                                &c_rnti);
            rbr.new_crnti = (uint16)c_rnti;
          }
          else
          {
            rbr.new_crnti_valid = FALSE;
          }

          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBR,
                                                  RRC_TRANSITION_FROM_DCH,
                                                  rbr.state_indicated_by_msg);
          rbr.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

          /* Start T_305 [periodic cell update] timer */
          rrccu_start_t_305_timer();

        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                 (rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH))
        {
          /*store the directed cell info */
           rbr.directed_cell_info = rrcrbr_is_pcpich_info_present(msg_ptr,MSG_REL5);

           /*Send the rbr complete msg on old configuration */
           send_rbr_complete_message(TRUE);
           /*wait for L2ack */
           rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
           rrcllc_print_trans_chnl_rate();
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (rbr.state_indicated_by_msg==RRC_STATE_URA_PCH))
        {
          /*Send the RBR complete msg on old configuration */
          send_rbr_complete_message(TRUE);
          /*wait for L2ack */
          rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
          rrcllc_print_trans_chnl_rate();
        }
        else if((current_state==RRC_STATE_CELL_FACH) && 
                ((rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
                 (rbr.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          /*call the function that evaluates the directed cell info and set the flags accordingly */
          if ((rbr.directed_cell_info == FALSE)
            && (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
          {
            if(rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH)
            {
              rbr.directed_cell_info = rrcrbr_is_pcpich_info_present(msg_ptr,MSG_REL5);
            }

            rbr.initiate_cell_selection_trans_from_fach  = TRUE;
          }
    

          /* check whether CPICH info is present or not. Store this
          info  */
         // rbr.directed_cell_info = rrcrbr_is_pcpich_info_present(msg_ptr);
          /*Send the RRCTCTC complete msg on old configuration */
          /* check whether new C-RNTI was present in the message. In that case
          we have to configure MAC with new C-RNTI before sending the response 
          message */
          if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,
             rrc_RadioBearerRelease_r5_IEs,new_C_RNTI))
          {
            rrc_translate_crnti(&rb_release_ptr->new_C_RNTI, 
                                &c_rnti);
            /* Send a RNTI Update Request */
            rnti_update_info.crnti_action    = RRC_RNTI_UPDATE;
            rnti_update_info.crnti           = (uint16)c_rnti;
            /* check whether URNTI is also present in the message */
            if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr,
             rrc_RadioBearerRelease_r5_IEs,new_U_RNTI))
            {
              rrc_translate_urnti(&rb_release_ptr->new_U_RNTI, 
                                  &u_rnti);
              rnti_update_info.urnti_action    = RRC_RNTI_UPDATE; 
              rnti_update_info.urnti           = u_rnti; 
            }
            else
            {
              rnti_update_info.urnti_action    = RRC_RNTI_NOCHANGE; 
            }
            rnti_update_info.procedure            = RRC_PROCEDURE_RBR;
            rnti_update_info.rlc_re_establish_srb = FALSE;
            rnti_update_info.rlc_re_establish_rab = FALSE;
            rnti_update_info.cnf_reqd             = TRUE;
            rrcrb_send_rnti_update_req(rnti_update_info);
            /* wait for the RNTI update confirm before sending the response
            message */
            rbr.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
          }
          else 
          {
            send_rbr_complete_message(TRUE);
            /*wait for L2ack */
            rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
            rrcllc_print_trans_chnl_rate();
          }
        }
        break;

      case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

        rbr.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
        /* Send the RB Release Failure Message */
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

        rbr.status.failure_status =RRCRB_MSG_UNSUPPORTED_CONFIG;
        /* Send the RB Release Failure Message */
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_INVALID:

        rbr.status.failure_status =RRCRB_MSG_INVALID_CONFIG;
        /* Send the RB Release Failure Message */
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_NOT_SET_OTHER:
        WRRC_MSG0_HIGH("LLC returned other failure!");
        /* When LLC sends this, there is a serious problem
        and we'll go back to disconnected state. No need
        to send the failure message here, we can just
        go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME        
      case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("RBR processing deferred due to LTA!");
      
        rbr.curr_substate = RRCRB_INITIAL;
      
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
      default:

        WRRC_MSG0_ERROR("Unknown return value from LLC");
        /* Unknown error, just go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
    }/* end switch */

    if ( ordered_config_err_occured == TRUE )
    {
        /* No need to continue. Clear the procedure
        variables and return. */
        rrcrbr_clear_procedure();
        return;
    }


    if(rbr.rbr_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }
  }/* end if - valid rb message */
}

/*===========================================================================

FUNCTION   RRCRBR_PROCESS_NEW_CELL_IND

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
static void rrcrbr_process_new_cell_ind
(
  rrc_new_cell_ind_type *ptr /* Pointer to the State Change Ind cmd */
)
{
     
  if(rrccsp_is_selected_cell_utran_directed()&&
      (rbr.new_crnti_valid))  
  {
    /* No need to initiate cell update as the C-RNTI is available
     * and UE is camped on the directed cell.
     */
    rrcrbr_send_initiate_cu_flag = FALSE;
  }
  else
  {
    rrcrbr_send_initiate_cu_flag = TRUE;
    rrcrbr_delete_crnti          = TRUE;
  }
#ifdef FEATURE_WCDMA_HS_FACH
 /* If H-RNTI is not given in OTA then initiate cell update*/
  if(rrcenhstate_set_hrnti_status_for_transitions())
  {
    rrcrbr_send_initiate_cu_flag = TRUE;
    rrcrbr_delete_crnti          = TRUE;
  }
#endif
  
#ifdef FEATURE_WCDMA_HS_RACH
 /* Check if E-RNTI is not given in OTA then initiate cell update*/
        if(rrchsrach_check_ernti_status())
        {
          rrcrbr_send_initiate_cu_flag = TRUE;
          rrcrbr_delete_crnti          = TRUE;
        }
#endif

  
    /* Call the function that creates and sends the  channel
    config information to be requested from LLC. */
  rrcrbr_send_channel_config_req();
  
  
    /* Change the substate since we're waiting for the
    confirmation from LLC for the channel configs */
    rbr.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
}


/*===========================================================================

FUNCTION   RRCRBR_PROCESS_RBRELEASE_R7

DESCRIPTION

  This function processes the received Rel 6 RB Release Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a Radio Bearer Release Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to release the Radio Bearers given by the message.
    
    If ORDERED_CONFIG could not be set, a Radio Bearer Release Failure message
    will be sent to UTRAN.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbr_process_rbrelease_r7
(
  rrc_cmd_type *cmd_ptr
)
{
  rrc_RB_InformationReleaseList  *rb_info_rel_list_ptr;
  rrc_RB_InformationAffectedList_r7 *rb_info_affected_list_ptr;
   rrc_RB_InformationReconfig_r7 *rb_info_reconfig_ptr;
  rrc_RadioBearerRelease *msg_ptr;  /* Pointer to the RB Release message        */
  rrcrb_directed_cell_info_type directed_cell_info; /* directed cell info struct */

  rrcllc_oc_status_e_type oc_status;/* To hold ordered_config status          */
  boolean ordered_config_err_occured = FALSE; /* Used to indicate if an error
                                    occured while setting ordered_config. */
  boolean rb_rel_srch = FALSE; 
  boolean rab_found = FALSE; 
  rrc_state_e_type current_state;       /* current RRC State  */
  uint32 rab_count, rb_count, localrb_count;
  /* Local C_RNTI value */
  rrc_int_c_rnti_type              c_rnti;
  rrc_int_u_rnti_type              u_rnti;
  rrcrb_rnti_update_info_type      rnti_update_info;   /* to hold rnti update info */

  rrc_msg_ver_enum_type msg_version = MSG_REL7;


  rrcsmc_srns_ip_check_status_e_type    ip_check_status = RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE;


  rrc_DL_DCCH_Message *dcch_msg_ptr ;     /* Pointer to the RB Release message */

  /* Store parameter IEs */
  rrc_RadioBearerRelease_r7_IEs* rb_release_ptr = NULL;


  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  /* Get the pointer to the actual RB Release message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerRelease);


  /* Store Msg Ptr in a local variable */
  rb_release_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.
    u.criticalExtensions.u.criticalExtensions.u.r7.radioBearerRelease_r7;



  if (TRUE == rrcrbr_validate_srns_relocation_info(msg_ptr, msg_version))
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
        break;

      case RRCSMC_SRNS_SECURITY_INFO_INVALID:
      case RRCSMC_INCOMPATIBLE_SECURITY_RECONFIGURATION:
        if (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier
                       == rbr.tr_ids.rejected_transaction)
        {
          WRRC_MSG1_HIGH("Invalid security config-rejected tr-id%d match!",
                   rbr.tr_ids.rejected_transaction);
          /* Nothing else to do, just ignore the message and return */
        } 
        else
        {
          /* send a reconfig failure messge with invalid
          configuration and 
          continue the same substate with any ongoing procedure */
          rbr.tr_ids.rejected_transaction = 
            (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
          if (ip_check_status == RRCSMC_SRNS_SECURITY_INFO_INVALID)
          {
            rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          }
          else
          {
            rbr.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
          }
          /*Check whether Cell Update is going on */
          if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
               (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
          {
            if(rbr.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
            {
              send_rbr_failure_message( rbr.status.failure_status,
                                       rbr.status.prot_err_cause );
              rbr.status.failure_status = RRCRB_MSG_VALID;
            }
            else
            {
              WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
              (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBR);
              /* Wait until Cell update procedure is completed */
              rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
            }
          }
          else
          {
            send_rbr_failure_message( rbr.status.failure_status,
                                            rbr.status.prot_err_cause );
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
      /*do not clear the procedure or change the rbr substate because
      some other rbe procedure might be in progress. Just return from
      here */
      return;
    }
  }


  if ((rrcllc_get_ordered_config_status() != OC_NOT_SET) ||
      (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
      (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
  {
     MSG_HIGH("Incomatible simultaneous reconfig! Rejecting RBR message,transaction_id:%d,accepted_transaction:%d",
         msg_ptr->u.later_than_r3.rrc_TransactionIdentifier,rbr.tr_ids.accepted_transaction,rbr.tr_ids.rejected_transaction );
     if((msg_ptr->u.later_than_r3.rrc_TransactionIdentifier != rbr.tr_ids.accepted_transaction) &&
         (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier != rbr.tr_ids.rejected_transaction))
     {
       /* We have received a RBR message with a new transaction id.
       This message should be in our rejected transaction
       list */
       rbr.tr_ids.rejected_transaction = 
         (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
       rbr.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;

       /*Check whether Cell Update is going on */
       if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
            (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
       {
         if(rbr.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
         {
           send_rbr_failure_message( rbr.status.failure_status,
                                    rbr.status.prot_err_cause );
           rbr.status.failure_status = RRCRB_MSG_VALID;
         }
         else
         {
           WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
           (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBR);
           /* Wait until Cell update procedure is completed */
           rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
         }
       }
       else
       {
         send_rbr_failure_message( rbr.status.failure_status,
                                         rbr.status.prot_err_cause );
         /*continue the same substate */ 
       }
     }
     return;
  }

  /* We have received a RB release message with a new transaction id.
  Accept the transaction Id */
  rbr.tr_ids.accepted_transaction =
     msg_ptr->u.later_than_r3.rrc_TransactionIdentifier;

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  validate_rbrelease_r7(msg_ptr);

  if ( rbr.status.failure_status != RRCRB_MSG_VALID )
  {
    /* RB Message was invalid. */
    WRRC_MSG0_HIGH("RBR Message validation failed");

    /* Send the RB Release Failure Message */
    send_rbr_failure_message( rbr.status.failure_status,
                              rbr.status.prot_err_cause
                            );

    rrcrbr_clear_procedure();
    /* Make sure we're back in the initial sub-state */
    rbr.curr_substate = RRCRB_INITIAL;
  }
  /* If the message is valid, proceed further */
  else
  {

    WRRC_MSG0_HIGH("Processing RBR Message");

    if(RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,ueMobilityStateIndicator))
    {
      if(rb_release_ptr->ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
      {
        WRRC_MSG0_HIGH("High Mobility Ind Set to TRUE in RBR.");
        rbr.high_mobility_ind = TRUE;
      }
    }
    /*store the rb ids tobe released */
    rbr.rbs_tobe_released.is_valid = TRUE;
    rb_info_rel_list_ptr = &rb_release_ptr->rb_InformationReleaseList;
    localrb_count = 0;
    while ( rb_info_rel_list_ptr->n > localrb_count)
    {
      /*check whether rb id is signalling rbid or does not exist in established
      rabs */
      if( rb_info_rel_list_ptr->elem[localrb_count] < DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
      {
        /* RB Message was invalid. */
        WRRC_MSG1_HIGH("Invalid RB id:%d in RBR Message", rb_info_rel_list_ptr->elem[localrb_count]);
        /* Send the RB Release Failure Message */
        rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );

        rrcrbr_clear_procedure();
        /* Make sure we're back in the initial sub-state */
        rbr.curr_substate = RRCRB_INITIAL;
        return;

      }
      else if( rb_info_rel_list_ptr->elem[localrb_count] == DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
      {
        if(RRCLCM_RLC_LC_ID_NOT_FOUND != rrclcm_check_dl_rlc_lc_id(
                                         UE_LOGCHAN_DCCH,
                                         DCCH_DT_LOW_PRI_RADIO_BEARER_ID,
                                         UE_MODE_ACKNOWLEDGED_DATA))
        {
          WRRC_MSG0_HIGH("Release SRB4");
        }
        else
        {
          WRRC_MSG1_HIGH("Ignore Non-exitent rb_id = %d in rbr msg",rb_info_rel_list_ptr->elem[localrb_count]);
		  localrb_count++;

		  continue;
        }
      }
      else if (!rrc_find_rb_in_est_rabs(rb_info_rel_list_ptr->elem[localrb_count]))
      {
        WRRC_MSG1_HIGH("Ignore Non-exitent rb_id =%d in rbr msg",rb_info_rel_list_ptr->elem[localrb_count]);
        localrb_count++;
        continue;
      }

      rbr.rbs_tobe_released.rb_id[localrb_count] = rb_info_rel_list_ptr->elem[localrb_count];
      localrb_count++;
    }
    rbr.rbs_tobe_released.num_rbs = localrb_count;
    WRRC_MSG1_HIGH("Number ofRBs tobe released:%d", localrb_count);

    /*store the rb ids tobe reconfigured */
    
    if (RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,rb_InformationReconfigList))
    {
      rbr.rbs_tobe_reconfigured.is_valid = TRUE;
            localrb_count = 0;
      while ( rb_release_ptr->rb_InformationReconfigList.n > localrb_count )
      {
      
	   rb_info_reconfig_ptr = 
			  &rb_release_ptr->rb_InformationReconfigList.elem[localrb_count];
        /* Fill up the RB id and indicate that it needs to be reconfigured */
        rbr.rbs_tobe_reconfigured.rb_info[localrb_count].rb_id= 
          rb_info_reconfig_ptr->rb_Identity;
        rbr.rbs_tobe_reconfigured.rb_info[localrb_count].rb_config = RECONFIG_RB;
        rbr.rbs_tobe_reconfigured.rb_info[localrb_count].rb_config = RECONFIG_RB_OTA;
        rbr.rbs_tobe_reconfigured.rb_info[localrb_count].ul_cfg_mask = rbr.rbs_tobe_reconfigured.rb_info[localrb_count].dl_cfg_mask = 0;
        if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,rrc_RB_InformationReconfig_r7,rlc_Info))&&
            (RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_ptr->rlc_Info,rrc_RLC_Info_r7,ul_RLC_Mode)))
        {
          rbr.rbs_tobe_reconfigured.rb_info[localrb_count].ul_cfg_mask = MODIFY_RB_CFG;
        }
        if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,rrc_RB_InformationReconfig_r7,rlc_Info))&&
            (RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_ptr->rlc_Info,rrc_RLC_Info_r7,dl_RLC_Mode)))
        {
          rbr.rbs_tobe_reconfigured.rb_info[localrb_count].dl_cfg_mask = MODIFY_RB_CFG;
        }
 
        if (  RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,
			rrc_RB_InformationReconfig_r7,rb_StopContinue))
        {
          MSG_HIGH("rb-reconfig-info rb %d to %d [0:STOP/1:CONTINUE] rb_count %d", 
              rbr.rbs_tobe_reconfigured.rb_info[localrb_count].rb_id, rb_info_reconfig_ptr->rb_StopContinue,localrb_count);
          /* over-ride rb_config with STOP or continue */
          if (rrc_RB_StopContinue_stopRB == rb_info_reconfig_ptr->rb_StopContinue)
          {
            rbr.rbs_tobe_reconfigured.rb_info[localrb_count].ul_cfg_mask |= STOP_RB_CFG;
            rbr.rbs_tobe_reconfigured.rb_info[localrb_count].dl_cfg_mask |= STOP_RB_CFG;
          }
          else if (rrc_RB_StopContinue_continueRB == rb_info_reconfig_ptr->rb_StopContinue)
          {
            rbr.rbs_tobe_reconfigured.rb_info[localrb_count].ul_cfg_mask |= CONTINUE_RB_CFG;
            rbr.rbs_tobe_reconfigured.rb_info[localrb_count].dl_cfg_mask |= CONTINUE_RB_CFG;
          }
        }

        localrb_count++;
        /* Go to the next reconfigured RB */
      }

      rbr.rbs_tobe_reconfigured.num_rbs= localrb_count;
      WRRC_MSG1_HIGH("Number ofRBs tobe reconfigured:%d", localrb_count);
    }

    /*store the rb ids tobe affected */
   
    if ( RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,rb_InformationAffectedList))
    {
      rbr.rbs_tobe_affected.is_valid = TRUE;
      rb_info_affected_list_ptr = 
      &rb_release_ptr->rb_InformationAffectedList;
      localrb_count = 0;
      while ( rb_info_affected_list_ptr->n > localrb_count )
      {
        /*check whether rb id is signalling rbid or does not exist in established
        rabs */
        if(( rb_info_affected_list_ptr->elem[localrb_count].rb_Identity > DCCH_DT_LOW_PRI_RADIO_BEARER_ID)&&
           (! rrc_find_rb_in_est_rabs(rb_info_affected_list_ptr->elem[localrb_count].rb_Identity)))
        {
          /* RB Message was invalid. */
          WRRC_MSG1_HIGH("Invalid RB id:%d in RBR Message", rb_info_affected_list_ptr->elem[localrb_count].rb_Identity);
          /* Send the RB Release Failure Message */
          rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          send_rbr_failure_message( rbr.status.failure_status,
                                    rbr.status.prot_err_cause
                                  );

          rrcrbr_clear_procedure();
          /* Make sure we're back in the initial sub-state */
          rbr.curr_substate = RRCRB_INITIAL;
          return;

        }
        rbr.rbs_tobe_affected.rb_id[localrb_count] = 
        rb_info_affected_list_ptr->elem[localrb_count].rb_Identity;
        localrb_count++;
        
      }
      rbr.rbs_tobe_affected.num_rbs= localrb_count;
      WRRC_MSG1_HIGH("Number ofRBs tobe affected:%d", localrb_count);
    }


    /* If there is a signalling connection to be released,
    or if there are individual RBs to be released, we
    will be releasing some RBs. */
     
    if (RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,signallingConnectionRelIndication))
    {
      rrcrbr_mark_cn_doamin_to_be_closed(rb_release_ptr->signallingConnectionRelIndication);
  
      for ( rab_count = 0; rab_count< MAX_RAB_TO_SETUP; rab_count++ )
      {
        if ( (rrc_est_rabs.rabs[rab_count].cn_domain == rbr.cn_to_release) &&
             (rrc_est_rabs.rabs[rab_count].num_rbs_for_rab > 0) )
        {
          rab_found = TRUE;
          for ( rb_count=0; rb_count<rrc_est_rabs.rabs[rab_count].num_rbs_for_rab; rb_count++ )
          {
            rb_rel_srch = FALSE;
            for ( localrb_count=0; localrb_count<rbr.rbs_tobe_released.num_rbs; localrb_count++ )
            {
              if ( rrc_est_rabs.rabs[rab_count].rb_for_rab[rb_count].rb_id == 
                   rbr.rbs_tobe_released.rb_id[localrb_count] )
              {
                rb_rel_srch = TRUE;
                break;
              }
            }
            if ( rb_rel_srch == FALSE ) // est rabs has an rb_id not in the rb release list
            {
              /* send failure message */
              rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
              send_rbr_failure_message( rbr.status.failure_status,
                                        rbr.status.prot_err_cause);
              /* Go back to initial substate */
              rbr.curr_substate = RRCRB_INITIAL;
              /* Then clear all local variables that may
              need to be cleared */
              rrcrbr_clear_procedure();
              return;
            }
          }/* rb_count loop */       
        } /*end of if loop */
      }/* rab_count loop */
      /*if none of the rabs in the establishedrabs have rb information*/
      if ( rab_found==FALSE )
      {
        WRRC_MSG1_HIGH("RB lookup in est_rabs for cn:%d failed", rbr.cn_to_release );
        /* send failure message */
        rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause);
        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;
        /* Then clear all local variables that may
        need to be cleared */
        rrcrbr_clear_procedure();
        return;

      }
    }

    /* Check if there is CN information present. If so, we need
    to send the information to the Cell Change manager */
    if (RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,cn_InformationInfo))
    {
      WRRC_MSG0_HIGH("Saving CN info for NAS");
      rbr.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(
        &(rb_release_ptr->cn_InformationInfo) );
      if (rbr.mm_cmd_ptr != NULL)
      {
        /* set cn_info_saved flag */
        rbr.cn_info_saved = TRUE;
      }
    }
#ifdef FEATURE_WCDMA_REL7_SPECRS
    /*If RBR OTA provides "supportForChangeOfUE_Capability" IE, we need to store it in
           "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
           "rrc_rnc_cap_change_support" can be updated accordingly. */  	
    if(RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs, supportForChangeOfUE_Capability))
    {
      if(rb_release_ptr->supportForChangeOfUE_Capability == TRUE)
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

    /* Indicate layer1 if rb release message is going to release voice RAB */
    rrcrbr_indicate_layer1_if_config_msg_release_voice_rab();

    rrcrb_update_mac_rab_status(RRC_PROCEDURE_RBR, RRC_CS_DOMAIN_CN_ID,
                                &rb_release_ptr->rb_InformationReleaseList,
                                FALSE);

    /* Set ORDERED_CONFIG so that LLC can process the
    necessary parameters. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_RBR,
                                           rbr.state_indicated_by_msg,
                                           rrc_DL_DCCH_Message_PDU,
                                           (void *)dcch_msg_ptr
                                         );

    switch ( oc_status )
    {
      case ORDERED_CONFIG_SET:
        if(rrc_sib_is_current_event_sib_read_in_dch())
        {
           WRRC_MSG0_HIGH("Force Init GPS ");
           rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
        }
        current_state = rrc_get_state(); 
       /* Populate the local variables to indicate if Re-direction info is present */
        directed_cell_info.freq_info_present = FALSE;
        directed_cell_info.pscr_present = FALSE; 
        if((current_state==RRC_STATE_CELL_FACH) && 
               ((rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH)
               ||(rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH)
               ||(rbr.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          if((RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,frequencyInfo))
             &&(RRC_CHECK_COMMON_MSG_TYPE(rb_release_ptr->frequencyInfo.modeSpecificInfo,
             rrc_FrequencyInfo_modeSpecificInfo_fdd)))
          {
            directed_cell_info.freq_info_present = TRUE;
            directed_cell_info.uarfcn_dl  = rb_release_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
          }
         
          if (rrcrbr_is_pcpich_info_present(msg_ptr,MSG_REL7))
          {
            if ((RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,dl_InformationPerRL_List))
                && (rb_release_ptr->dl_InformationPerRL_List.n != 0))
            {
              
              directed_cell_info.pscr_present = TRUE;
              directed_cell_info.pscr = rb_release_ptr->dl_InformationPerRL_List.elem[0].
                modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
            }
          }      
          /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
                 FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
          if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
            (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
          {
            WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
            if(rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH)
            {
              rrcrbr_send_initiate_cu_flag = TRUE;
              rrcrbr_delete_crnti          = TRUE;
            }
            else
            {
              rbr.directed_cell_info = TRUE;
            }
          }  
        }

        if (
            (current_state==rbr.state_indicated_by_msg) ||
            ((current_state==RRC_STATE_CELL_FACH) && 
             (rbr.state_indicated_by_msg==RRC_STATE_CELL_DCH))
            )
        {
          /* CELL_DCH->CELL_DCH or CELL_FACH->CELL_FACH or 
          CELL_FACH->CELL_DCH case. We can release the
          Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */
          /*check whether any frequency info is present in the message
          and transition is CELL_FACH->CELL_FACH. In this case
          initiate a cell update after channel config is successfully completed */
          if ((current_state==RRC_STATE_CELL_FACH)&& 
             (rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH))
          {
             /*call the function that evaluates the directed cell info and set the flags accordingly */
            if ((rrcrbr_send_initiate_cu_flag == FALSE) &&
                (rrcrb_validate_directed_cell_info (directed_cell_info)== FALSE))
            {
              if(RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,new_C_RNTI))
              {
                rbr.new_crnti_valid = TRUE;
                rrc_translate_crnti(&rb_release_ptr->new_C_RNTI, 
                                    &c_rnti);
                rbr.new_crnti = (uint16)c_rnti;
              }
              else
              {
                rbr.new_crnti_valid = FALSE;
              }
   
              rrcllcoc_update_toc_with_oc_from_fach();
              /*Initiate the cell selection*/
              rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBR,
                                                       RRC_TRANSITION_FROM_FACH,
                                                       rbr.state_indicated_by_msg);

              rbr.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

              /* exit from the switch case */
              break;
     
            }
          } /* if CELL_FACH->CELL_FACH reconfiguration */

          /* Call the function that creates and sends a channel
            config request to LLC. */
          rrcrbr_send_channel_config_req();

          /* Change the substate since we're waiting for the
             confirmation from LLC for the channel configs */
          rbr.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH))
        {
            
          if(RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,new_C_RNTI))
          {
            rbr.new_crnti_valid = TRUE;
            rrc_translate_crnti(&rb_release_ptr->new_C_RNTI, 
                                &c_rnti);
            rbr.new_crnti = (uint16)c_rnti;
          }
          else
          {
            rbr.new_crnti_valid = FALSE;
          }

          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBR,
                                                  RRC_TRANSITION_FROM_DCH,
                                                  RRC_STATE_CELL_FACH);
          rbr.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

          /* Start T_305 [periodic cell update] timer */
          rrccu_start_t_305_timer();		  
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                 (rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH))
        {
          /*store the directed cell info */
           rbr.directed_cell_info = rrcrbr_is_pcpich_info_present(msg_ptr,MSG_REL7);

           /*Send the rbr complete msg on old configuration */
           send_rbr_complete_message(TRUE);
           /*wait for L2ack */
           rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
           rrcllc_print_trans_chnl_rate();
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (rbr.state_indicated_by_msg==RRC_STATE_URA_PCH))
        {
          /*Send the RBR complete msg on old configuration */
          send_rbr_complete_message(TRUE);
          /*wait for L2ack */
          rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
          rrcllc_print_trans_chnl_rate();
        }
        else if((current_state==RRC_STATE_CELL_FACH) && 
                ((rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
                 (rbr.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          /* check whether CPICH info is present or not. Store this
          info  */
         // rbr.directed_cell_info = rrcrbr_is_pcpich_info_present(msg_ptr,MSG_REL7);
          /*Send the RRCTCTC complete msg on old configuration */
          /* check whether new C-RNTI was present in the message. In that case
          we have to configure MAC with new C-RNTI before sending the response 
          message */
          /*call the function that evaluates the directed cell info and set the flags accordingly */
          if ((rbr.directed_cell_info == FALSE) &&
            (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
          {
            if(rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH)
            {
              rbr.directed_cell_info = rrcrbr_is_pcpich_info_present(msg_ptr,MSG_REL7);
            }
            rbr.initiate_cell_selection_trans_from_fach  = TRUE;
          }
     
          if(RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,new_C_RNTI))
          {
            rrc_translate_crnti(&rb_release_ptr->new_C_RNTI, 
                                &c_rnti);
            /* Send a RNTI Update Request */
            rnti_update_info.crnti_action    = RRC_RNTI_UPDATE;
            rnti_update_info.crnti           = (uint16)c_rnti;
            /* check whether URNTI is also present in the message */
            
            if (RRCRBR_R7_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r7_IEs,new_U_RNTI))
            {
              rrc_translate_urnti(&rb_release_ptr->new_U_RNTI, 
                                  &u_rnti);
              rnti_update_info.urnti_action    = RRC_RNTI_UPDATE; 
              rnti_update_info.urnti           = u_rnti; 
            }
            else
            {
              rnti_update_info.urnti_action    = RRC_RNTI_NOCHANGE; 
            }
            rnti_update_info.procedure            = RRC_PROCEDURE_RBR;
            rnti_update_info.rlc_re_establish_srb = FALSE;
            rnti_update_info.rlc_re_establish_rab = FALSE;
            rnti_update_info.cnf_reqd             = TRUE;
            rrcrb_send_rnti_update_req(rnti_update_info);
            /* wait for the RNTI update confirm before sending the response
            message */
            rbr.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
          }
          else 
          {
            send_rbr_complete_message(TRUE);
            /*wait for L2ack */
            rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
            rrcllc_print_trans_chnl_rate();
          }
        }
        break;

      case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

        rbr.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
        /* Send the RB Release Failure Message */
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

        rbr.status.failure_status =RRCRB_MSG_UNSUPPORTED_CONFIG;
        /* Send the RB Release Failure Message */
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_INVALID:

        rbr.status.failure_status =RRCRB_MSG_INVALID_CONFIG;
        /* Send the RB Release Failure Message */
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
      
      case ORDERED_CONFIG_NOT_SET_OTHER:
        WRRC_MSG0_HIGH("LLC returned other failure!");
        /* When LLC sends this, there is a serious problem
        and we'll go back to disconnected state. No need
        to send the failure message here, we can just
        go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME        
      case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("RBR processing deferred due to LTA!");
      
        rbr.curr_substate = RRCRB_INITIAL;
      
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
      default:

        WRRC_MSG0_ERROR("Unknown return value from LLC");
        /* Unknown error, just go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
    }/* end switch */

    if ( ordered_config_err_occured == TRUE )
    {
        /* No need to continue. Clear the procedure
        variables and return. */
        rrcrbr_clear_procedure();
        return;
    }


    if(rbr.rbr_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }
  }/* end if - valid rb message */
}


#if defined (FEATURE_VOC_AMR_WB)  || defined(FEATURE_SCUDIF_ENHANCEMENT)
/*===========================================================================

FUNCTION   rrcrbr_update_rab_info_reconfiglist_r8

DESCRIPTION Updates the RAB information received in the RAB_InformationReconfigList IE in 
                      RB Release message.
                      It updates the NAS Syncronisation Indicator field received in the RB Release message.
                      In case the NAS Syncronisation Indicator IE is not present , then a default value of 
                      current codec  used is updated.



DEPENDENCIES
  None.

RETURN VALUE
 None

SIDE EFFECTS
  None.

===========================================================================*/
static uecomdef_status_e_type rrcrbr_update_rab_info_reconfiglist_r8
(
  struct rrc_RAB_InformationReconfigList_r8 *rab_list_ptr
)
{
  uint32 rab_index=0;  
  rrc_rab_id_type rab_id;           /* To store RAB id in internal RRC format */
  rrcmn_rab_status_T rab_type = RRCMN_INVALID_RAB_ID;     

#ifdef FEATURE_VOC_AMR_WB
  mvs_mode_type nas_sync_ind = rrc_get_current_codec_mode();
#endif /*FEATURE_VOC_AMR_WB*/
                                    
  if(rab_list_ptr != NULL)
  {
    for (rab_index=0; (rab_list_ptr->n > rab_index) && (rab_index < RRCRB_MAX_RAB_PER_MSG); rab_index++)
    {
      rab_id = rrcrb_translate_gsm_map_rab_id((rab_list_ptr->elem[rab_index].rab_Identity.u.gsm_MAP_RAB_Identity));

      /*RRC_GKG: Here we can add the validation for RAB-Id from EST_RABs*/
      rbr.rabs.rabs_in_msg[rab_index].rab_id_is_gsm_map = TRUE;
      rbr.rabs.rabs_in_msg[rab_index].rab_id = rab_id;
      
      /* Check if this RAB is CS Domain or PS Domain */  
      if(rab_list_ptr->elem[rab_index].cn_DomainIdentity == RRC_CS_DOMAIN_CN_ID )
      {
        /* Check if it is in loopback test */
        if (rrclbt_is_lb_on() == TRUE )
        {
          /* Set Rab type to Data */
          MSG_LOW("Loopback Test", 0, 0, 0);
          rbr.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_DATA_RAB;
        }
        else
        {
           /* For CS RABs, Call MN layer to verify this RAB */
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
          rab_type = rrcmn_verify_rab_subs( (radio_access_bearer_T)rab_id ,rrc_get_as_id());
#else
          rab_type = rrcmn_verify_rab( (radio_access_bearer_T)rab_id );
#endif
          rbr.rabs.rabs_in_msg[rab_index].cn_domain =RRC_CS_DOMAIN_CN_ID;

          MSG_HIGH("For RAB-Id %d, RAB type from NAS = %d, RAB type from EST_RABs = %d",rab_id, rab_type,
            rrc_get_rab_type_for_given_rab_id(rbr.rabs.rabs_in_msg[rab_index].rab_id));

          if( rab_type == RRCMN_CS_VOICE_CALL )
          {
            rbr.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_VOICE_RAB;
          }
          else
          {
            rbr.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_DATA_RAB;
          }

#ifdef FEATURE_VOC_AMR_WB
          if( (rab_type == RRCMN_CS_VOICE_CALL )
#ifdef FEATURE_SCUDIF_ENHANCEMENT
#error code not present
#endif
             )
          {
            nas_sync_ind =  rrc_translate_nas_sync_indicator(&(rab_list_ptr->elem[rab_index].nas_Synchronisation_Indicator));
            if( nas_sync_ind == MVS_MODE_NONE)
            {
              rbr.status.failure_status= RRCRB_MSG_UNSUPPORTED_CONFIG;
              WRRC_MSG1_HIGH("REL8: Unsupported CODEC type %d",nas_sync_ind );
              return FAILURE;
            }
#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
            rrc_set_ordered_codec_status(TRUE);
#endif
          }
#endif /*FEATURE_VOC_AMR_WB*/
        }   
      }
      else
      {
        rbr.rabs.rabs_in_msg[rab_index].cn_domain =RRC_PS_DOMAIN_CN_ID;
        rbr.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_PS_DATA_RAB;
      }
#ifdef FEATURE_VOC_AMR_WB
      rbr.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator = nas_sync_ind;
#endif /*FEATURE_VOC_AMR_WB*/
    } 
  }
  
  rbr.rabs.num_rabs_in_msg = rab_index;

#ifdef FEATURE_VOC_AMR_WB
  rrc_set_ordered_codec_mode(nas_sync_ind);
#endif /*FEATURE_VOC_AMR_WB*/

  return SUCCESS;
}
#endif /* FEATURE_VOC_AMR_WB || FEATURE_SCUDIF_ENHANCEMENT*/

/*===========================================================================
FUNCTION   VALIDATE_RBRELEASE_R8

DESCRIPTION
  This function validates the received Rel 8 RB Release Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a Radio Bearer Release Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void validate_rbrelease_r8
(
rrc_RadioBearerRelease *msg_ptr      /* Pointer to the RB Release message */
)
{
  rrc_RadioBearerRelease_r8_IEs* rb_release_ptr = NULL;
  rbr.status.failure_status = RRCRB_MSG_VALID;

  rb_release_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
    u.criticalExtensions.u.criticalExtensions.u.r8.radioBearerRelease_r8;
  
  /* Check the RRC State indicatior given by the message */
  switch (rb_release_ptr->rrc_StateIndicator )
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      rbr.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;
    
    case rrc_RRC_StateIndicator_cell_FACH:
      rbr.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      rbr.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;
    
    case rrc_RRC_StateIndicator_ura_PCH:
      rbr.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;
    
    default:
      /* No other states are supported right now. */
      WRRC_MSG0_HIGH("REL8: Unsupported state in RB Release Message");
      rbr.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
      break;
  }
  
  if ( rbr.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG )
  {
    return;
  }
  
  if(rb_release_ptr->rb_InformationReleaseList.n == 0 )
  {
    WRRC_MSG0_HIGH("REL8: rb_InformationReleaseList is absent!");
    rbr.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
    rbr.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
    return;
  }

  if(RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,rb_InformationReconfigList))
  {
    /* Save the linked list head pointer */
    if (FAILURE == rrcllc_validate_rb_info_reconfig_list_r8(&rb_release_ptr->rb_InformationReconfigList))
    {
      rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
#if defined (FEATURE_VOC_AMR_WB ) || defined(FEATURE_SCUDIF_ENHANCEMENT)
    if(RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,rab_InformationReconfigList))
    {
      if(rrcrbr_update_rab_info_reconfiglist_r8(&rb_release_ptr->rab_InformationReconfigList) == FAILURE)
      {
        rbr.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
        return;
      }
    }
    else
    {
      /* Set the Ordered codec mode with the current active codec */
      rrc_set_ordered_codec_mode(rrc_active_codec);
      rbr.rabs.num_rabs_in_msg =0;
    }
#endif /*FEATURE_VOC_AMR_WB || FEATURE_SCUDIF_ENHANCEMENT*/
  }
  
  if(RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,
    dl_CounterSynchronisationInfo))
  {
    /*check whether the SRNS relocation is within Cell_DCH or not.
    SRNS relocation information can come in reconfiguration messages
    only when the state transition is Cell_DCH->Cell_DCH */
    if (!((rbr.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
      ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("REL8: SRNS reloc with invalid states cur_state %d, msg_state %d",
        rrc_get_state(), rbr.state_indicated_by_msg);
      rbr.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has 
    PDCP info list. If it is true then this configuration is unsupported */
    if((RRC_MSG_COMMON_BITMASK_IE_TYPE2(rb_release_ptr->dl_CounterSynchronisationInfo,
		rb_WithPDCP_InfoList)) ||
        (RRC_MSG_COMMON_BITMASK_IE(rb_release_ptr->dl_CounterSynchronisationInfo,
        rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
    {
      WRRC_MSG0_HIGH("REL8: PDCP info in DL counter sync info not supported");
      rbr.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation. Set the flag that indicates that this procedure
         involves SRNS relocation */
      rbr.rbr_for_srns_relocation = TRUE;

      /* store if new U-RNTI is present in the message */
      if(RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,new_U_RNTI))
      {
        rbr.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(rb_release_ptr->new_U_RNTI), &rbr.new_urnti);
      }
    }
  } /* dl counter sync info */

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch */
  if((rbr.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,dl_CommonInformation)) 
      && (RRC_MSG_COMMON_BITMASK_IE(rb_release_ptr->dl_CommonInformation,
      rrc_DL_CommonInformation_r8,dl_dpchInfoCommon)) &&
      ( rb_release_ptr->dl_CommonInformation.dl_dpchInfoCommon.t ==
      T_rrc_DL_CommonInformation_r8_dl_dpchInfoCommon_dl_DPCH_InfoCommon))
    {
      //check if ciphering is active in mac for cs domain, if it is then,
      //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      if(rrcsmc_tm_ciph_active() == TRUE)
      {
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        uecomdef_status_e_type loc_ret_val;

        WRRC_MSG0_HIGH("REL8: To-DCH-Step-1 config activated");

        loc_ret_val = rrcrb_get_mac_d_hfn(( RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.dl_DPCH_InfoCommon,
			  rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
             &rb_release_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
             &mac_d_hfn_l);

        if(loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("REL8: get_mac_d_hfn failed");
        }
        if(rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
        {
          WRRC_MSG0_HIGH("REL8:Process Ciphering Config for HHO failed");
        }
        rbr.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("REL8: To-DCH-Step-1 not needed");
      }
    }
  }

  /*Cell_PCH/URA_PCH specific validations */
  if ((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||(rbr.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
    /* If UTRAN DRX coefficient is not present then it is invalid configuration */
    if (!( RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,
      utran_DRX_CycleLengthCoeff)))
    {
      WRRC_MSG1_ERROR("REL8: UTRAN DRX Coef not preset, State:%d", rbr.state_indicated_by_msg);
      rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    if (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH)
    {
      if (!( RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,ura_Identity)))  
      {
        WRRC_MSG0_HIGH("REL8: URA ID not present");
        rrccu_clear_ura_id ();
      }
      else
      {
        /*URA ID present in the message set ura_identity variable with this value */
        if (rb_release_ptr->ura_Identity.numbits == 16)
        {
          rrccu_update_ura_id (&(rb_release_ptr->ura_Identity));
        }
        else
        {
          WRRC_MSG1_ERROR("Invalid URA ID length%d", rb_release_ptr->ura_Identity.numbits);
          rbr.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
          rbr.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
          return;
        }
      }
    }
  }
  return;
}

/*===========================================================================
FUNCTION   rrcrbr_process_after_oc_config_set_r8

DESCRIPTION
  This function processes the R8 RBR OTA after OC is set by this OTA. So if we call this function, it means
  that ORDERED_CONFIG_SET is returned while setting OC by RBR OTA.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void rrcrbr_process_after_oc_config_set_r8
(
  rrc_DL_DCCH_Message *dcch_msg_ptr,
  rrc_RadioBearerRelease_r8_IEs* rb_release_ptr
)
{
  rrc_state_e_type                           current_state;       
  rrcrb_directed_cell_info_type         directed_cell_info; 
  rrc_RadioBearerRelease *              msg_ptr;  
  rrc_int_c_rnti_type                        c_rnti;
  rrc_int_u_rnti_type                        u_rnti;
  rrcrb_rnti_update_info_type           rnti_update_info;   

  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerRelease);
  current_state = rrc_get_state(); 

  if(rrc_sib_is_current_event_sib_read_in_dch())
  {
    WRRC_MSG0_HIGH("REL8: Force Init GPS ");
    rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
  }

  /* Populate the local variables to indicate if Re-direction info is present */
  directed_cell_info.freq_info_present = FALSE;
  directed_cell_info.pscr_present = FALSE; 
  if((current_state==RRC_STATE_CELL_FACH) && ((rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH)
    ||(rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||(rbr.state_indicated_by_msg==RRC_STATE_URA_PCH)))
  {
    if((RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,frequencyInfo))
      &&(RRC_CHECK_COMMON_MSG_TYPE(rb_release_ptr->frequencyInfo.modeSpecificInfo,
      rrc_FrequencyInfo_modeSpecificInfo_fdd)))
    {
      directed_cell_info.freq_info_present = TRUE;
      directed_cell_info.uarfcn_dl  = rb_release_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
    }
   
    if(rrcrbr_is_pcpich_info_present(msg_ptr,MSG_REL8))
    {
      if((RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,
        dl_InformationPerRL_List)) && (rb_release_ptr->dl_InformationPerRL_List.n != 0))
      {
        directed_cell_info.pscr_present = TRUE;
        directed_cell_info.pscr = rb_release_ptr->dl_InformationPerRL_List.elem[0].
          modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
      }
    }      
    /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
            FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
    if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
      (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
    {
      WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
      if(rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH)
      {
        rrcrbr_send_initiate_cu_flag = TRUE;
        rrcrbr_delete_crnti          = TRUE;
      }
      else
      {
        rbr.directed_cell_info = TRUE;
      }
    } 
  }
  
  if((current_state==rbr.state_indicated_by_msg) ||((current_state==RRC_STATE_CELL_FACH) && 
    (rbr.state_indicated_by_msg==RRC_STATE_CELL_DCH)))
  {
    if((current_state==RRC_STATE_CELL_FACH) && (rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH))
    {
       /*call the function that evaluates the directed cell info and set the flags accordingly */
      if ((rrcrbr_send_initiate_cu_flag == FALSE) &&
          (rrcrb_validate_directed_cell_info (directed_cell_info)== FALSE))
      {
        if(RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,new_C_RNTI))
        {
          rbr.new_crnti_valid = TRUE;
          rrc_translate_crnti(&rb_release_ptr->new_C_RNTI, &c_rnti);
          rbr.new_crnti = (uint16)c_rnti;
        }
        else
        {
          rbr.new_crnti_valid = FALSE;
        }
  
        rrcllcoc_update_toc_with_oc_from_fach();
        /*Initiate the cell selection*/
        rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBR, RRC_TRANSITION_FROM_FACH,
                                                 rbr.state_indicated_by_msg);
  
        rbr.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
        return;
        }

    } 
  
    /* Call the function that creates and sends a channel config request to LLC. */
    rrcrbr_send_channel_config_req();
  
    /* Change the substate since we're waiting for the confirmation from LLC for the channel configs */
    rbr.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
  }

  else if((current_state==RRC_STATE_CELL_DCH) && (rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH))
  {
      
    if(RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,new_C_RNTI))
    {
      rbr.new_crnti_valid = TRUE;
      rrc_translate_crnti(&rb_release_ptr->new_C_RNTI, &c_rnti);
      rbr.new_crnti = (uint16)c_rnti;
    }
    else
    {
      rbr.new_crnti_valid = FALSE;
    }
  
    /*Initiate the cell selection*/
    rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBR, RRC_TRANSITION_FROM_DCH,
                                            RRC_STATE_CELL_FACH);
    rbr.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
  
    /* Start T_305 [periodic cell update] timer */
    rrccu_start_t_305_timer();
  }

  else if((current_state==RRC_STATE_CELL_DCH) && (rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH))
  {
    /*store the directed cell info */
     rbr.directed_cell_info = rrcrbr_is_pcpich_info_present(msg_ptr,MSG_REL8);
  
     /*Send the rbr complete msg on old configuration */
     send_rbr_complete_message(TRUE);
     /*wait for L2ack */
     rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
     rrcllc_print_trans_chnl_rate();
  }

  else if((current_state==RRC_STATE_CELL_DCH) && (rbr.state_indicated_by_msg==RRC_STATE_URA_PCH))
  {
    /*Send the RBR complete msg on old configuration */
    send_rbr_complete_message(TRUE);
    /*wait for L2ack */
    rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
    rrcllc_print_trans_chnl_rate();
  }

  else if((current_state==RRC_STATE_CELL_FACH) && ((rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
    (rbr.state_indicated_by_msg==RRC_STATE_URA_PCH)))
  {
    /*call the function that evaluates the directed cell info and set the flags accordingly */
    if((rbr.directed_cell_info == FALSE) &&
      (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
    {
      if(rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH)
      {
        rbr.directed_cell_info = rrcrbr_is_pcpich_info_present(msg_ptr,MSG_REL8);
      }
      rbr.initiate_cell_selection_trans_from_fach  = TRUE;
    }
  
    if(RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,new_C_RNTI))
    {
      rrc_translate_crnti(&rb_release_ptr->new_C_RNTI, &c_rnti);
      /* Send a RNTI Update Request */
      rnti_update_info.crnti_action    = RRC_RNTI_UPDATE;
      rnti_update_info.crnti           = (uint16)c_rnti;
      /* check whether URNTI is also present in the message */
      if (RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,new_U_RNTI))
      {
        rrc_translate_urnti(&rb_release_ptr->new_U_RNTI, &u_rnti);
        rnti_update_info.urnti_action    = RRC_RNTI_UPDATE; 
        rnti_update_info.urnti           = u_rnti; 
      }
      else
      {
        rnti_update_info.urnti_action    = RRC_RNTI_NOCHANGE; 
      }
      rnti_update_info.procedure            = RRC_PROCEDURE_RBR;
      rnti_update_info.rlc_re_establish_srb = FALSE;
      rnti_update_info.rlc_re_establish_rab = FALSE;
      rnti_update_info.cnf_reqd             = TRUE;
      rrcrb_send_rnti_update_req(rnti_update_info);
      /* wait for the RNTI update confirm before sending the response message */
      rbr.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
    }
    else 
    {
      send_rbr_complete_message(TRUE);
      rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
      rrcllc_print_trans_chnl_rate();
    }
  }
  return;  
}

/*===========================================================================
FUNCTION   RRCRBR_PROCESS_RBRELEASE_R8

DESCRIPTION
  This function processes the received Rel 8 RB Release Message. It first validates the message. 
  If the message has an invalid or unsupported configuration, a Radio Bearer Release Failure message 
  will be sent to the UTRAN. If the message is valid, it will be processed as follows -

    ORDERED_CONFIG will be set by calling LLC. 

    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG_REQ will be 
    sent to LLC to release the Radio Bearers given by the message.
    
    If ORDERED_CONFIG could not be set, a Radio Bearer Release Failure message will be sent to UTRAN.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void rrcrbr_process_rbrelease_r8
(
  rrc_cmd_type *cmd_ptr
)
{
  rrc_RB_InformationReleaseList  *rb_info_rel_list_ptr;
  rrc_RB_InformationAffectedList_r8 *rb_info_affected_list_ptr;
  rrc_RB_InformationReconfig_r8 *rb_info_reconfig_ptr;
  rrc_RadioBearerRelease *msg_ptr;  /* Pointer to the RB Release message        */

  rrcllc_oc_status_e_type oc_status;/* To hold ordered_config status */
  /* Used to indicate if an error occured while setting ordered_config. */
  boolean ordered_config_err_occured = FALSE; 
  boolean rb_rel_srch = FALSE; 
  boolean rab_found = FALSE; 

  uint32 rab_count, rb_count, localrb_count,local_idx=0;
  rrc_msg_ver_enum_type msg_version = MSG_REL8;
  rrcsmc_srns_ip_check_status_e_type    ip_check_status = RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE;
  rrc_DL_DCCH_Message *dcch_msg_ptr ;     /* Pointer to the RB Release message */
  rrc_RadioBearerRelease_r8_IEs* rb_release_ptr = NULL;

  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  /* Get the pointer to the actual RB Release message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerRelease);

  /* Store Msg Ptr in a local variable */
  rb_release_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.
    u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.radioBearerRelease_r8;

  if(TRUE == rrcrbr_validate_srns_relocation_info(msg_ptr, msg_version))
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
        break;

      case RRCSMC_SRNS_SECURITY_INFO_INVALID:
      case RRCSMC_INCOMPATIBLE_SECURITY_RECONFIGURATION:
        if (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier
                       == rbr.tr_ids.rejected_transaction)
        {
          WRRC_MSG1_HIGH("Invalid security config-rejected tr-id%d match!",
                   rbr.tr_ids.rejected_transaction);
          /* Nothing else to do, just ignore the message and return */
        } 
        else
        {
          /* send a reconfig failure messge with invalid
          configuration and 
          continue the same substate with any ongoing procedure */
          rbr.tr_ids.rejected_transaction = 
            (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
          if (ip_check_status == RRCSMC_SRNS_SECURITY_INFO_INVALID)
          {
            rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          }
          else
          {
            rbr.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
          }
          /*Check whether Cell Update is going on */
          if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
               (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
          {
            if(rbr.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
            {
              send_rbr_failure_message( rbr.status.failure_status,
                                       rbr.status.prot_err_cause );
              rbr.status.failure_status = RRCRB_MSG_VALID;
            }
            else
            {
              WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
              (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBR);
              /* Wait until Cell update procedure is completed */
              rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
            }
          }
          else
          {
            send_rbr_failure_message( rbr.status.failure_status,
                                            rbr.status.prot_err_cause );
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
      /*do not clear the procedure or change the rbr substate because
      some other rbe procedure might be in progress. Just return from
      here */
      return;
    }
  }

  if((rrcllc_get_ordered_config_status() != OC_NOT_SET) ||
    (rrc_ccm_is_reselection_in_progress() == TRUE) ||
    (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
  {
     MSG_HIGH("Incomatible simultaneous reconfig! Rejecting RBR message,transaction_id:%d,accepted_transaction:%d",
         msg_ptr->u.later_than_r3.rrc_TransactionIdentifier,rbr.tr_ids.accepted_transaction,rbr.tr_ids.rejected_transaction );
     if((msg_ptr->u.later_than_r3.rrc_TransactionIdentifier != rbr.tr_ids.accepted_transaction) &&
         (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier != rbr.tr_ids.rejected_transaction))
     {
       /* We have received a RBR message with a new transaction id.
       This message should be in our rejected transaction
       list */
       rbr.tr_ids.rejected_transaction = (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
       rbr.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;

       /*Check whether Cell Update is going on */
       if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
            (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
       {
         if(rbr.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
         {
           send_rbr_failure_message( rbr.status.failure_status,
                                    rbr.status.prot_err_cause );
           rbr.status.failure_status = RRCRB_MSG_VALID;
         }
         else
         {
           WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
           (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBR);
           /* Wait until Cell update procedure is completed */
           rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
         }
       }
       else
       {
         send_rbr_failure_message( rbr.status.failure_status,
                                         rbr.status.prot_err_cause );
         /*continue the same substate */ 
       }
     }
     return;
  }

  /* We have received a RB release message with a new transaction id.
  Accept the transaction Id */
  rbr.tr_ids.accepted_transaction = msg_ptr->u.later_than_r3.rrc_TransactionIdentifier;

  /* Validate the Message to make sure there is no invalid or incompatible configuration 
  given by the message */
  validate_rbrelease_r8(msg_ptr);

  if(rbr.status.failure_status != RRCRB_MSG_VALID )
  {
    /* RB Message was invalid. */
    WRRC_MSG0_HIGH("REL8: RBR Message validation failed");

    /* Send the RB Release Failure Message */
    send_rbr_failure_message( rbr.status.failure_status, rbr.status.prot_err_cause);

    rrcrbr_clear_procedure();
    /* Make sure we're back in the initial sub-state */
    rbr.curr_substate = RRCRB_INITIAL;
  }
  /* If the message is valid, proceed further */
  else
  {
    WRRC_MSG0_HIGH("REL8: Processing RBR Message");

    if(RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,ueMobilityStateIndicator))
    {
      if(rb_release_ptr->ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
      {
        WRRC_MSG0_HIGH("High Mobility Ind Set to TRUE in RBR.");
        rbr.high_mobility_ind = TRUE;
      }
    }
    /*store the rb ids tobe released */
    rbr.rbs_tobe_released.is_valid = TRUE;
    rb_info_rel_list_ptr = &rb_release_ptr->rb_InformationReleaseList;
    localrb_count = 0;
	local_idx=0;
    while (rb_info_rel_list_ptr->n > local_idx)
    {
      /*check whether rb id is signalling rbid or does not exist in established
      rabs */
      if( rb_info_rel_list_ptr->elem[local_idx] < DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
      {
        /* RB Message was invalid. */
        WRRC_MSG1_HIGH("REL8: Invalid RB id:%d in RBR Message", rb_info_rel_list_ptr->elem[local_idx]);
        /* Send the RB Release Failure Message */
        rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        send_rbr_failure_message( rbr.status.failure_status, rbr.status.prot_err_cause);

        rrcrbr_clear_procedure();
        /* Make sure we're back in the initial sub-state */
        rbr.curr_substate = RRCRB_INITIAL;
        return;
      }
      else if( rb_info_rel_list_ptr->elem[local_idx] == DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
      {
        if(RRCLCM_RLC_LC_ID_NOT_FOUND != rrclcm_check_dl_rlc_lc_id(
                                         UE_LOGCHAN_DCCH,
                                         DCCH_DT_LOW_PRI_RADIO_BEARER_ID,
                                         UE_MODE_ACKNOWLEDGED_DATA))
        {
          WRRC_MSG0_HIGH("Release SRB4");
        }
        else
        {
          WRRC_MSG1_HIGH("Rel8:Ignore Non-exitent rb_id = %d in rbr msg",rb_info_rel_list_ptr->elem[local_idx]);
          local_idx++;
          continue;
        }
      }
      else if (!rrc_find_rb_in_est_rabs(rb_info_rel_list_ptr->elem[local_idx]))
      {
        WRRC_MSG1_HIGH("REL8: Ignore Non-exitent rb_id =%d in rbr msg",rb_info_rel_list_ptr->elem[local_idx]);
        local_idx++;
        continue;
      }

      rbr.rbs_tobe_released.rb_id[localrb_count] = rb_info_rel_list_ptr->elem[local_idx];
      localrb_count++;
	  local_idx++;
    }

    rbr.rbs_tobe_released.num_rbs = localrb_count;

    WRRC_MSG1_HIGH("REL8: Number of RBs to be released:%d", localrb_count);

    if(RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,rb_InformationReconfigList))
    {
      rbr.rbs_tobe_reconfigured.is_valid = TRUE;
      
      localrb_count = 0;
      while (rb_release_ptr->rb_InformationReconfigList.n > localrb_count )
      {
        rb_info_reconfig_ptr = &rb_release_ptr->rb_InformationReconfigList.elem[localrb_count];
        /* Fill up the RB id and indicate that it needs to be reconfigured */
        rbr.rbs_tobe_reconfigured.rb_info[localrb_count].rb_id= 
          rb_info_reconfig_ptr->rb_Identity;
        rbr.rbs_tobe_reconfigured.rb_info[localrb_count].rb_config = RECONFIG_RB;

        rbr.rbs_tobe_reconfigured.rb_info[localrb_count].rb_config = RECONFIG_RB_OTA;
        rbr.rbs_tobe_reconfigured.rb_info[localrb_count].ul_cfg_mask = rbr.rbs_tobe_reconfigured.rb_info[localrb_count].dl_cfg_mask = 0;
        if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,
			rrc_RB_InformationReconfig_r8,rlc_Info))&&
          (RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_ptr->rlc_Info,
          rrc_RLC_Info_r7,ul_RLC_Mode)))
        {
          rbr.rbs_tobe_reconfigured.rb_info[localrb_count].ul_cfg_mask = MODIFY_RB_CFG;
        }
        if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,
			rrc_RB_InformationReconfig_r8,rlc_Info))&&
          (RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_ptr->rlc_Info,
          rrc_RLC_Info_r7,dl_RLC_Mode)))
        {
          rbr.rbs_tobe_reconfigured.rb_info[localrb_count].dl_cfg_mask = MODIFY_RB_CFG;
        }
 
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,
			rrc_RB_InformationReconfig_r8,rb_StopContinue))
        {
          MSG_HIGH("REL8: RB-Id %d to %d [0:STOP/1:CONTINUE] rb_count %d", 
              rbr.rbs_tobe_reconfigured.rb_info[localrb_count].rb_id, rb_info_reconfig_ptr->rb_StopContinue,localrb_count);
          /* over-ride rb_config with STOP or continue */
          if (rrc_RB_StopContinue_stopRB == rb_info_reconfig_ptr->rb_StopContinue)
          {
            rbr.rbs_tobe_reconfigured.rb_info[localrb_count].ul_cfg_mask |= STOP_RB_CFG;
            rbr.rbs_tobe_reconfigured.rb_info[localrb_count].dl_cfg_mask |= STOP_RB_CFG;
          }
          else if (rrc_RB_StopContinue_continueRB == rb_info_reconfig_ptr->rb_StopContinue)
          {
            rbr.rbs_tobe_reconfigured.rb_info[localrb_count].ul_cfg_mask |= CONTINUE_RB_CFG;
            rbr.rbs_tobe_reconfigured.rb_info[localrb_count].dl_cfg_mask |= CONTINUE_RB_CFG;
          }
        }

        localrb_count++;
        /* Go to the next reconfigured RB */
      
      }

      rbr.rbs_tobe_reconfigured.num_rbs= localrb_count;
      WRRC_MSG1_HIGH("REL8: Number of RBs to be reconfigured:%d", localrb_count);
    }

    /*store the rb ids tobe affected */
    if(RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,rb_InformationAffectedList))
    {
      rbr.rbs_tobe_affected.is_valid = TRUE;
      rb_info_affected_list_ptr = 
      &rb_release_ptr->rb_InformationAffectedList;
      localrb_count = 0;
      while ( rb_info_affected_list_ptr->n > localrb_count)
      {
        /*check whether rb id is signalling rbid or does not exist in established
        rabs */
        if((rb_info_affected_list_ptr->elem[localrb_count].rb_Identity > DCCH_DT_LOW_PRI_RADIO_BEARER_ID) &&
          (!rrc_find_rb_in_est_rabs((rb_info_affected_list_ptr->elem[localrb_count].rb_Identity))))
        {
          /* RB Message was invalid. */
          WRRC_MSG1_HIGH("REL8: Invalid RB id:%d in RBR Message", rb_info_affected_list_ptr->elem[localrb_count].rb_Identity);
          /* Send the RB Release Failure Message */
          rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          send_rbr_failure_message( rbr.status.failure_status, rbr.status.prot_err_cause);

          rrcrbr_clear_procedure();
          /* Make sure we're back in the initial sub-state */
          rbr.curr_substate = RRCRB_INITIAL;
          return;
        }
        rbr.rbs_tobe_affected.rb_id[localrb_count] = 
        rb_info_affected_list_ptr->elem[localrb_count].rb_Identity;
        localrb_count++;
      }
      rbr.rbs_tobe_affected.num_rbs= localrb_count;
      WRRC_MSG1_HIGH("REL8: Number of RBs tobe affected:%d", localrb_count);
    }

    /* If there is a signalling connection to be released,
    or if there are individual RBs to be released, we
    will be releasing some RBs. */
    if(RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,
      signallingConnectionRelIndication))
    {
      /* Need to release a CN domain */
      rbr.need_to_release_cn_domain = TRUE; 
      /* Copy the domain that is to be released */
      rbr.cn_to_release = rb_release_ptr->signallingConnectionRelIndication;

      for(rab_count = 0; rab_count< MAX_RAB_TO_SETUP; rab_count++)
      {
        if((rrc_est_rabs.rabs[rab_count].cn_domain == rbr.cn_to_release) &&
          (rrc_est_rabs.rabs[rab_count].num_rbs_for_rab > 0))
        {
          rab_found = TRUE;
          for ( rb_count=0; rb_count<rrc_est_rabs.rabs[rab_count].num_rbs_for_rab; rb_count++ )
          {
            rb_rel_srch = FALSE;
            for(localrb_count=0; localrb_count<rbr.rbs_tobe_released.num_rbs; localrb_count++)
            {
              if(rrc_est_rabs.rabs[rab_count].rb_for_rab[rb_count].rb_id == 
                  rbr.rbs_tobe_released.rb_id[localrb_count])
              {
                rb_rel_srch = TRUE;
                break;
              }
            }
            if(rb_rel_srch == FALSE) // est rabs has an rb_id not in the rb release list
            {
              /* send failure message */
              rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
              send_rbr_failure_message( rbr.status.failure_status, rbr.status.prot_err_cause);
              /* Go back to initial substate */
              rbr.curr_substate = RRCRB_INITIAL;
              /* Then clear all local variables that may need to be cleared */
              rrcrbr_clear_procedure();
              return;
            }
          }/* rb_count loop */       
        } /*end of if loop */
      }/* rab_count loop */

      /*if none of the rabs in the establishedrabs have rb information*/
      if(rab_found==FALSE )
      {
        WRRC_MSG1_HIGH("REL8: RB lookup in EST_RABS for cn:%d failed", rbr.cn_to_release );
        /* send failure message */
        rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        send_rbr_failure_message( rbr.status.failure_status, rbr.status.prot_err_cause);
        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;
        /* Then clear all local variables that may
        need to be cleared */
        rrcrbr_clear_procedure();
        return;
      }
    }

    /* Check if there is CN information present. If so, we need to send the information to the Cell Change manager */
    if (RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs,cn_InformationInfo))
    {
      WRRC_MSG0_HIGH("REL8: Saving CN info for NAS");
      rbr.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(
        &(rb_release_ptr->cn_InformationInfo) );
      if (rbr.mm_cmd_ptr != NULL)
      {
        /* set cn_info_saved flag */
        rbr.cn_info_saved = TRUE;
      }
    }
#ifdef FEATURE_WCDMA_REL7_SPECRS
    /*If RBR OTA provides "supportForChangeOfUE_Capability" IE, we need to store it in
           "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
           "rrc_rnc_cap_change_support" can be updated accordingly. */
    if(RRCRB_R8_MSG_IE_PRESENT(rb_release_ptr,rrc_RadioBearerRelease_r8_IEs, 
      supportForChangeOfUE_Capability))
    {
      if(rb_release_ptr->supportForChangeOfUE_Capability == TRUE)
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
#endif /*FEATURE_WCDMA_REL7_SPECRS*/

    /* Indicate layer1 if rb release message is going to release voice RAB */
    rrcrbr_indicate_layer1_if_config_msg_release_voice_rab();

    rrcrb_update_mac_rab_status(RRC_PROCEDURE_RBR, RRC_CS_DOMAIN_CN_ID,
                                &rb_release_ptr->rb_InformationReleaseList, FALSE);

    /* Set ORDERED_CONFIG so that LLC can process the necessary parameters. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_RBR, rbr.state_indicated_by_msg,
                      rrc_DL_DCCH_Message_PDU, (void *)dcch_msg_ptr);

    switch ( oc_status )
    {
      case ORDERED_CONFIG_SET:
        rrcrbr_process_after_oc_config_set_r8(dcch_msg_ptr, rb_release_ptr);
        break;

      case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

        rbr.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
        /* Send the RB Release Failure Message */
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

        rbr.status.failure_status =RRCRB_MSG_UNSUPPORTED_CONFIG;
        /* Send the RB Release Failure Message */
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_INVALID:

        rbr.status.failure_status =RRCRB_MSG_INVALID_CONFIG;
        /* Send the RB Release Failure Message */
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
      
      case ORDERED_CONFIG_NOT_SET_OTHER:
        WRRC_MSG0_HIGH("LLC returned other failure!");
        /* When LLC sends this, there is a serious problem
        and we'll go back to disconnected state. No need
        to send the failure message here, we can just
        go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME        
      case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("RBR processing deferred due to LTA!");
      
        rbr.curr_substate = RRCRB_INITIAL;
      
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
      default:

        WRRC_MSG0_ERROR("Unknown return value from LLC");
        /* Unknown error, just go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
    }/* end switch */

    if ( ordered_config_err_occured == TRUE )
    {
        /* No need to continue. Clear the procedure
        variables and return. */
        rrcrbr_clear_procedure();
        return;
    }

    if(rbr.rbr_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }
  }/* end if - valid rb message */
  return;
}

#ifdef FEATURE_WCDMA_REL9


/*===========================================================================
FUNCTION   rrcrbr_process_after_oc_config_set_r9

DESCRIPTION
  This function processes the R9 RBR OTA after OC is set by this OTA. So if we call this function, it means
  that ORDERED_CONFIG_SET is returned while setting OC by RBR OTA.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void rrcrbr_process_after_oc_config_set_r9
(
  rrc_DL_DCCH_Message *dcch_msg_ptr,
  rrc_RadioBearerRelease_r9_IEs* rb_release_ptr
)
{
  rrc_state_e_type                           current_state;       
  rrcrb_directed_cell_info_type         directed_cell_info; 
  rrc_RadioBearerRelease *              msg_ptr;  
  rrc_int_c_rnti_type                        c_rnti;
  rrc_int_u_rnti_type                        u_rnti;
  rrcrb_rnti_update_info_type           rnti_update_info;   

  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerRelease);
  current_state = rrc_get_state(); 

  if(rrc_sib_is_current_event_sib_read_in_dch())
  {
    WRRC_MSG0_HIGH("REL9: Force Init GPS ");
    rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
  }

  /* Populate the local variables to indicate if Re-direction info is present */
  directed_cell_info.freq_info_present = FALSE;
  directed_cell_info.pscr_present = FALSE; 
  if((current_state==RRC_STATE_CELL_FACH) && ((rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH)
    ||(rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||(rbr.state_indicated_by_msg==RRC_STATE_URA_PCH)))
  {
    if((RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,frequencyInfo))
      &&(RRC_CHECK_COMMON_MSG_TYPE(rb_release_ptr->frequencyInfo.modeSpecificInfo,
      rrc_FrequencyInfo_modeSpecificInfo_fdd)))
    {
      directed_cell_info.freq_info_present = TRUE;
      directed_cell_info.uarfcn_dl  = rb_release_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
    }
   
    if(rrcrbr_is_pcpich_info_present(msg_ptr,MSG_REL9))
    {
      if((RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,
        dl_InformationPerRL_List)) && (rb_release_ptr->dl_InformationPerRL_List.n != 0))
      {
        directed_cell_info.pscr_present = TRUE;
        directed_cell_info.pscr = rb_release_ptr->dl_InformationPerRL_List.elem[0].
          modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
      }
    }      
    /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
            FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
    if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
      (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
    {
      WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
      if(rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH)
      {
        rrcrbr_send_initiate_cu_flag = TRUE;
        rrcrbr_delete_crnti          = TRUE;
      }
      else
      {
        rbr.directed_cell_info = TRUE;
      }
    } 
  }
  
  if((current_state==rbr.state_indicated_by_msg) ||((current_state==RRC_STATE_CELL_FACH) && 
    (rbr.state_indicated_by_msg==RRC_STATE_CELL_DCH)))
  {
    if((current_state==RRC_STATE_CELL_FACH) && (rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH))
    {
       /*call the function that evaluates the directed cell info and set the flags accordingly */
      if ((rrcrbr_send_initiate_cu_flag == FALSE) &&
          (rrcrb_validate_directed_cell_info (directed_cell_info)== FALSE))
      {
        if(RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,new_C_RNTI))
        {
          rbr.new_crnti_valid = TRUE;
          rrc_translate_crnti(&rb_release_ptr->new_C_RNTI, &c_rnti);
          rbr.new_crnti = (uint16)c_rnti;
        }
        else
        {
          rbr.new_crnti_valid = FALSE;
        }
  
        rrcllcoc_update_toc_with_oc_from_fach();
        /*Initiate the cell selection*/
        rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBR, RRC_TRANSITION_FROM_FACH,
                                                 rbr.state_indicated_by_msg);
  
        rbr.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
        return;
        }

    } 
  
    /* Call the function that creates and sends a channel config request to LLC. */
    rrcrbr_send_channel_config_req();
  
    /* Change the substate since we're waiting for the confirmation from LLC for the channel configs */
    rbr.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
  }

  else if((current_state==RRC_STATE_CELL_DCH) && (rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH))
  {
      
    if(RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,new_C_RNTI))
    {
      rbr.new_crnti_valid = TRUE;
      rrc_translate_crnti(&rb_release_ptr->new_C_RNTI, &c_rnti);
      rbr.new_crnti = (uint16)c_rnti;
    }
    else
    {
      rbr.new_crnti_valid = FALSE;
    }
  
    /*Initiate the cell selection*/
    rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBR, RRC_TRANSITION_FROM_DCH,
                                            RRC_STATE_CELL_FACH);
    rbr.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
  
    /* Start T_305 [periodic cell update] timer */
    rrccu_start_t_305_timer();
  }

  else if((current_state==RRC_STATE_CELL_DCH) && (rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH))
  {
    /*store the directed cell info */
     rbr.directed_cell_info = rrcrbr_is_pcpich_info_present(msg_ptr,MSG_REL9);
  
     /*Send the rbr complete msg on old configuration */
     send_rbr_complete_message(TRUE);
     /*wait for L2ack */
     rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
     rrcllc_print_trans_chnl_rate();
  }

  else if((current_state==RRC_STATE_CELL_DCH) && (rbr.state_indicated_by_msg==RRC_STATE_URA_PCH))
  {
    /*Send the RBR complete msg on old configuration */
    send_rbr_complete_message(TRUE);
    /*wait for L2ack */
    rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
    rrcllc_print_trans_chnl_rate();
  }

  else if((current_state==RRC_STATE_CELL_FACH) && ((rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
    (rbr.state_indicated_by_msg==RRC_STATE_URA_PCH)))
  {
    /*call the function that evaluates the directed cell info and set the flags accordingly */
    if((rbr.directed_cell_info == FALSE) &&
      (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
    {
      if(rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH)
      {
        rbr.directed_cell_info = rrcrbr_is_pcpich_info_present(msg_ptr,MSG_REL9);
      }
      rbr.initiate_cell_selection_trans_from_fach  = TRUE;
    }
  
    if(RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,new_C_RNTI))
    {
      rrc_translate_crnti(&rb_release_ptr->new_C_RNTI, &c_rnti);
      /* Send a RNTI Update Request */
      rnti_update_info.crnti_action    = RRC_RNTI_UPDATE;
      rnti_update_info.crnti           = (uint16)c_rnti;
      /* check whether URNTI is also present in the message */
      if (RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,new_U_RNTI))
      {
        rrc_translate_urnti(&rb_release_ptr->new_U_RNTI, &u_rnti);
        rnti_update_info.urnti_action    = RRC_RNTI_UPDATE; 
        rnti_update_info.urnti           = u_rnti; 
      }
      else
      {
        rnti_update_info.urnti_action    = RRC_RNTI_NOCHANGE; 
      }
      rnti_update_info.procedure            = RRC_PROCEDURE_RBR;
      rnti_update_info.rlc_re_establish_srb = FALSE;
      rnti_update_info.rlc_re_establish_rab = FALSE;
      rnti_update_info.cnf_reqd             = TRUE;
      rrcrb_send_rnti_update_req(rnti_update_info);
      /* wait for the RNTI update confirm before sending the response message */
      rbr.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
    }
    else 
    {
      send_rbr_complete_message(TRUE);
      rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
      rrcllc_print_trans_chnl_rate();
    }
  }
  return;  
}




/*===========================================================================
FUNCTION   VALIDATE_RBRELEASE_R9

DESCRIPTION
  This function validates the received Rel 9 RB Release Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a Radio Bearer Release Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void validate_rbrelease_r9
(
rrc_RadioBearerRelease *msg_ptr      /* Pointer to the RB Release message */
)
{
  rrc_RadioBearerRelease_r9_IEs* rb_release_ptr = NULL;
  rbr.status.failure_status = RRCRB_MSG_VALID;

  rb_release_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.
    u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.radioBearerRelease_r9;
  
  /* Check the RRC State indicatior given by the message */
  switch (rb_release_ptr->rrc_StateIndicator )
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      rbr.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;
    
    case rrc_RRC_StateIndicator_cell_FACH:
      rbr.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      rbr.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;
    
    case rrc_RRC_StateIndicator_ura_PCH:
      rbr.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;
    
    default:
      /* No other states are supported right now. */
      WRRC_MSG0_HIGH("REL8: Unsupported state in RB Release Message");
      rbr.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
      break;
  }
  
  if ( rbr.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG )
  {
    return;
  }
  
  if(rb_release_ptr->rb_InformationReleaseList.n == 0 )
  {
    WRRC_MSG0_HIGH("REL9: rb_InformationReleaseList is absent!");
    rbr.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
    rbr.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
    return;
  }

  if(RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,rb_InformationReconfigList))
  {
    /* Save the linked list head pointer */
    if (FAILURE == rrcllc_validate_rb_info_reconfig_list_r8(&rb_release_ptr->rb_InformationReconfigList))
    {
      rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
#if defined (FEATURE_VOC_AMR_WB ) || defined(FEATURE_SCUDIF_ENHANCEMENT)
    if(RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,rab_InformationReconfigList))
    {
      if(rrcrbr_update_rab_info_reconfiglist_r8(&rb_release_ptr->rab_InformationReconfigList) == FAILURE)
      {
        rbr.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
        return;
      }
    }
    else
    {
      /* Set the Ordered codec mode with the current active codec */
      rrc_set_ordered_codec_mode(rrc_active_codec);
      rbr.rabs.num_rabs_in_msg =0;
    }
#endif /*FEATURE_VOC_AMR_WB || FEATURE_SCUDIF_ENHANCEMENT*/
  }
  
  if(RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,
    dl_CounterSynchronisationInfo))
  {
    /*check whether the SRNS relocation is within Cell_DCH or not.
    SRNS relocation information can come in reconfiguration messages
    only when the state transition is Cell_DCH->Cell_DCH */
    if (!((rbr.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
      ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("REL9: SRNS reloc with invalid states cur_state %d, msg_state %d",
        rrc_get_state(), rbr.state_indicated_by_msg);
      rbr.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    /* msg has SRNS relocation info. Check whether dl_counterSynchronizationInfo has 
    PDCP info list. If it is true then this configuration is unsupported */
    if((RRC_MSG_COMMON_BITMASK_IE_TYPE2(rb_release_ptr->dl_CounterSynchronisationInfo,
		rb_WithPDCP_InfoList)) ||
        (RRC_MSG_COMMON_BITMASK_IE(rb_release_ptr->dl_CounterSynchronisationInfo,
        rrc_DL_CounterSynchronisationInfo_r5,rb_PDCPContextRelocationList)))
    {
      WRRC_MSG0_HIGH("REL9: PDCP info in DL counter sync info not supported");
      rbr.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return;
    }
    else
    {
      /* The procedure involves Lossy SRNS relocation. Set the flag that indicates that this procedure
         involves SRNS relocation */
      rbr.rbr_for_srns_relocation = TRUE;

      /* store if new U-RNTI is present in the message */
      if(RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,new_U_RNTI))
      {
        rbr.new_urnti_valid = TRUE;
        rrc_translate_urnti (&(rb_release_ptr->new_U_RNTI), &rbr.new_urnti);
      }
    }
  } /* dl counter sync info */

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch */
  if((rbr.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,dl_CommonInformation)) 
      && (RRC_MSG_COMMON_BITMASK_IE(rb_release_ptr->dl_CommonInformation,
      rrc_DL_CommonInformation_r8,dl_dpchInfoCommon)) &&
      ( rb_release_ptr->dl_CommonInformation.dl_dpchInfoCommon.t ==
      T_rrc_DL_CommonInformation_r8_dl_dpchInfoCommon_dl_DPCH_InfoCommon))
    {
      //check if ciphering is active in mac for cs domain, if it is then,
      //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      if(rrcsmc_tm_ciph_active() == TRUE)
      {
        uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
        uecomdef_status_e_type loc_ret_val;

        WRRC_MSG0_HIGH("REL9: To-DCH-Step-1 config activated");

        loc_ret_val = rrcrb_get_mac_d_hfn(( RRC_MSG_COMMON_BITMASK_IE_PTR(rb_release_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.dl_DPCH_InfoCommon,
			  rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
             &rb_release_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
             &mac_d_hfn_l);

        if(loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("REL9: get_mac_d_hfn failed");
        }
        if(rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
        {
          WRRC_MSG0_HIGH("REL9:Process Ciphering Config for HHO failed");
        }
        rbr.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("REL9: To-DCH-Step-1 not needed");
      }
    }
  }

  /*Cell_PCH/URA_PCH specific validations */
  if ((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||(rbr.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
    /* If UTRAN DRX coefficient is not present then it is invalid configuration */
    if (!( RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,
      utran_DRX_CycleLengthCoeff)))
    {
      WRRC_MSG1_ERROR("REL9: UTRAN DRX Coef not preset, State:%d", rbr.state_indicated_by_msg);
      rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    if (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH)
    {
      if (!( RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,ura_Identity)))  
      {
        WRRC_MSG0_HIGH("REL9: URA ID not present");
        rrccu_clear_ura_id ();
      }
      else
      {
        /*URA ID present in the message set ura_identity variable with this value */
        if (rb_release_ptr->ura_Identity.numbits == 16)
        {
          rrccu_update_ura_id (&(rb_release_ptr->ura_Identity));
        }
        else
        {
          WRRC_MSG1_ERROR("Invalid URA ID length%d", rb_release_ptr->ura_Identity.numbits);
          rbr.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
          rbr.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
          return;
        }
      }
    }
  }
  return;
}



/*===========================================================================
FUNCTION   RRCRBR_PROCESS_RBRELEASE_R9

DESCRIPTION
  This function processes the received Rel 9 RB Release Message. It first validates the message. 
  If the message has an invalid or unsupported configuration, a Radio Bearer Release Failure message 
  will be sent to the UTRAN. If the message is valid, it will be processed as follows -

    ORDERED_CONFIG will be set by calling LLC. 

    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG_REQ will be 
    sent to LLC to release the Radio Bearers given by the message.
    
    If ORDERED_CONFIG could not be set, a Radio Bearer Release Failure message will be sent to UTRAN.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void rrcrbr_process_rbrelease_r9
(
  rrc_cmd_type *cmd_ptr
)
{
  rrc_RB_InformationReleaseList  *rb_info_rel_list_ptr;
  rrc_RB_InformationAffectedList_r8 *rb_info_affected_list_ptr;
  rrc_RB_InformationReconfig_r8 *rb_info_reconfig_ptr;
  rrc_RadioBearerRelease *msg_ptr;  /* Pointer to the RB Release message        */

  rrcllc_oc_status_e_type oc_status;/* To hold ordered_config status */
  /* Used to indicate if an error occured while setting ordered_config. */
  boolean ordered_config_err_occured = FALSE; 
  boolean rb_rel_srch = FALSE; 
  boolean rab_found = FALSE; 

  uint32 rab_count, rb_count, localrb_count,local_idx=0;
  rrc_msg_ver_enum_type msg_version = MSG_REL9;
  rrcsmc_srns_ip_check_status_e_type    ip_check_status = RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE;
  rrc_DL_DCCH_Message *dcch_msg_ptr ;     /* Pointer to the RB Release message */
  rrc_RadioBearerRelease_r9_IEs* rb_release_ptr = NULL;

  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  /* Get the pointer to the actual RB Release message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerRelease);

  /* Store Msg Ptr in a local variable */
  rb_release_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.
    u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.radioBearerRelease_r9;

  if(TRUE == rrcrbr_validate_srns_relocation_info(msg_ptr, msg_version))
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
        break;

      case RRCSMC_SRNS_SECURITY_INFO_INVALID:
      case RRCSMC_INCOMPATIBLE_SECURITY_RECONFIGURATION:
        if (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier
                       == rbr.tr_ids.rejected_transaction)
        {
          WRRC_MSG1_HIGH("Invalid security config-rejected tr-id%d match!",
                   rbr.tr_ids.rejected_transaction);
          /* Nothing else to do, just ignore the message and return */
        } 
        else
        {
          /* send a reconfig failure messge with invalid
          configuration and 
          continue the same substate with any ongoing procedure */
          rbr.tr_ids.rejected_transaction = 
            (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
          if (ip_check_status == RRCSMC_SRNS_SECURITY_INFO_INVALID)
          {
            rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          }
          else
          {
            rbr.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
          }
          /*Check whether Cell Update is going on */
          if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
               (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
          {
            if(rbr.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
            {
              send_rbr_failure_message( rbr.status.failure_status,
                                       rbr.status.prot_err_cause );
              rbr.status.failure_status = RRCRB_MSG_VALID;
            }
            else
            {
              WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
              (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBR);
              /* Wait until Cell update procedure is completed */
              rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
            }
          }
          else
          {
            send_rbr_failure_message( rbr.status.failure_status,
                                            rbr.status.prot_err_cause );
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
      /*do not clear the procedure or change the rbr substate because
      some other rbe procedure might be in progress. Just return from
      here */
      return;
    }
  }

  if((rrcllc_get_ordered_config_status() != OC_NOT_SET) ||
    (rrc_ccm_is_reselection_in_progress() == TRUE) ||
    (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
  {
     MSG_HIGH("Incomatible simultaneous reconfig! Rejecting RBR message,transaction_id:%d,accepted_transaction:%d",
         msg_ptr->u.later_than_r3.rrc_TransactionIdentifier,rbr.tr_ids.accepted_transaction,rbr.tr_ids.rejected_transaction );
     if((msg_ptr->u.later_than_r3.rrc_TransactionIdentifier != rbr.tr_ids.accepted_transaction) &&
         (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier != rbr.tr_ids.rejected_transaction))
     {
       /* We have received a RBR message with a new transaction id.
       This message should be in our rejected transaction
       list */
       rbr.tr_ids.rejected_transaction = (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
       rbr.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;

       /*Check whether Cell Update is going on */
       if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
            (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
       {
         if(rbr.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
         {
           send_rbr_failure_message( rbr.status.failure_status,
                                    rbr.status.prot_err_cause );
           rbr.status.failure_status = RRCRB_MSG_VALID;
         }
         else
         {
           WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
           (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBR);
           /* Wait until Cell update procedure is completed */
           rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
         }
       }
       else
       {
         send_rbr_failure_message( rbr.status.failure_status,
                                         rbr.status.prot_err_cause );
         /*continue the same substate */ 
       }
     }
     return;
  }

  /* We have received a RB release message with a new transaction id.
  Accept the transaction Id */
  rbr.tr_ids.accepted_transaction = msg_ptr->u.later_than_r3.rrc_TransactionIdentifier;

  /* Validate the Message to make sure there is no invalid or incompatible configuration 
  given by the message */
  validate_rbrelease_r9(msg_ptr);

  if(rbr.status.failure_status != RRCRB_MSG_VALID )
  {
    /* RB Message was invalid. */
    WRRC_MSG0_HIGH("REL9: RBR Message validation failed");

    /* Send the RB Release Failure Message */
    send_rbr_failure_message( rbr.status.failure_status, rbr.status.prot_err_cause);

    rrcrbr_clear_procedure();
    /* Make sure we're back in the initial sub-state */
    rbr.curr_substate = RRCRB_INITIAL;
  }
  /* If the message is valid, proceed further */
  else
  {
    WRRC_MSG0_HIGH("REL9: Processing RBR Message");

    if(RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,ueMobilityStateIndicator))
    {
      if(rb_release_ptr->ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
      {
        WRRC_MSG0_HIGH("High Mobility Ind Set to TRUE in RBR.");
        rbr.high_mobility_ind = TRUE;
      }
    }
    /*store the rb ids tobe released */
    rbr.rbs_tobe_released.is_valid = TRUE;
    rb_info_rel_list_ptr = &rb_release_ptr->rb_InformationReleaseList;
    localrb_count = 0;
  local_idx=0;
    while (rb_info_rel_list_ptr->n > local_idx)
    {
      /*check whether rb id is signalling rbid or does not exist in established
      rabs */
      if( rb_info_rel_list_ptr->elem[local_idx] < DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
      {
        /* RB Message was invalid. */
        WRRC_MSG1_HIGH("REL9: Invalid RB id:%d in RBR Message", rb_info_rel_list_ptr->elem[local_idx]);
        /* Send the RB Release Failure Message */
        rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        send_rbr_failure_message( rbr.status.failure_status, rbr.status.prot_err_cause);

        rrcrbr_clear_procedure();
        /* Make sure we're back in the initial sub-state */
        rbr.curr_substate = RRCRB_INITIAL;
        return;
      }
      else if( rb_info_rel_list_ptr->elem[local_idx] == DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
      {
        if(RRCLCM_RLC_LC_ID_NOT_FOUND != rrclcm_check_dl_rlc_lc_id(
                                         UE_LOGCHAN_DCCH,
                                         DCCH_DT_LOW_PRI_RADIO_BEARER_ID,
                                         UE_MODE_ACKNOWLEDGED_DATA))
        {
          WRRC_MSG0_HIGH("Release SRB4");
        }
        else
        {
          WRRC_MSG1_HIGH("Rel9:Ignore Non-exitent rb_id = %d in rbr msg",rb_info_rel_list_ptr->elem[local_idx]);
          local_idx++;
          continue;
        }
      }
      else if (!rrc_find_rb_in_est_rabs(rb_info_rel_list_ptr->elem[local_idx]))
      {
        WRRC_MSG1_HIGH("REL9: Ignore Non-exitent rb_id =%d in rbr msg",rb_info_rel_list_ptr->elem[local_idx]);
        local_idx++;
        continue;
      }

      rbr.rbs_tobe_released.rb_id[localrb_count] = rb_info_rel_list_ptr->elem[local_idx];
      localrb_count++;
    local_idx++;
    }

    rbr.rbs_tobe_released.num_rbs = localrb_count;

    WRRC_MSG1_HIGH("REL9: Number of RBs to be released:%d", localrb_count);

    if(RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,rb_InformationReconfigList))
    {
      rbr.rbs_tobe_reconfigured.is_valid = TRUE;
      
      localrb_count = 0;
      while (rb_release_ptr->rb_InformationReconfigList.n > localrb_count )
      {
        rb_info_reconfig_ptr = &rb_release_ptr->rb_InformationReconfigList.elem[localrb_count];
        /* Fill up the RB id and indicate that it needs to be reconfigured */
        rbr.rbs_tobe_reconfigured.rb_info[localrb_count].rb_id= 
          rb_info_reconfig_ptr->rb_Identity;
        rbr.rbs_tobe_reconfigured.rb_info[localrb_count].rb_config = RECONFIG_RB;

        rbr.rbs_tobe_reconfigured.rb_info[localrb_count].rb_config = RECONFIG_RB_OTA;
        rbr.rbs_tobe_reconfigured.rb_info[localrb_count].ul_cfg_mask = rbr.rbs_tobe_reconfigured.rb_info[localrb_count].dl_cfg_mask = 0;
        if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,
      rrc_RB_InformationReconfig_r8,rlc_Info))&&
          (RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_ptr->rlc_Info,
          rrc_RLC_Info_r7,ul_RLC_Mode)))
        {
          rbr.rbs_tobe_reconfigured.rb_info[localrb_count].ul_cfg_mask = MODIFY_RB_CFG;
        }
        if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,
      rrc_RB_InformationReconfig_r8,rlc_Info))&&
          (RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_ptr->rlc_Info,
          rrc_RLC_Info_r7,dl_RLC_Mode)))
        {
          rbr.rbs_tobe_reconfigured.rb_info[localrb_count].dl_cfg_mask = MODIFY_RB_CFG;
        }
 
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,
      rrc_RB_InformationReconfig_r8,rb_StopContinue))
        {
          MSG_HIGH("REL9: RB-Id %d to %d [0:STOP/1:CONTINUE] rb_count %d", 
              rbr.rbs_tobe_reconfigured.rb_info[localrb_count].rb_id, rb_info_reconfig_ptr->rb_StopContinue,localrb_count);
          /* over-ride rb_config with STOP or continue */
          if (rrc_RB_StopContinue_stopRB == rb_info_reconfig_ptr->rb_StopContinue)
          {
            rbr.rbs_tobe_reconfigured.rb_info[localrb_count].ul_cfg_mask |= STOP_RB_CFG;
            rbr.rbs_tobe_reconfigured.rb_info[localrb_count].dl_cfg_mask |= STOP_RB_CFG;
          }
          else if (rrc_RB_StopContinue_continueRB == rb_info_reconfig_ptr->rb_StopContinue)
          {
            rbr.rbs_tobe_reconfigured.rb_info[localrb_count].ul_cfg_mask |= CONTINUE_RB_CFG;
            rbr.rbs_tobe_reconfigured.rb_info[localrb_count].dl_cfg_mask |= CONTINUE_RB_CFG;
          }
        }

        localrb_count++;
        /* Go to the next reconfigured RB */
      
      }

      rbr.rbs_tobe_reconfigured.num_rbs= localrb_count;
      WRRC_MSG1_HIGH("REL9: Number of RBs to be reconfigured:%d", localrb_count);
    }

    /*store the rb ids tobe affected */
    if(RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,rb_InformationAffectedList))
    {
      rbr.rbs_tobe_affected.is_valid = TRUE;
      rb_info_affected_list_ptr = 
      &rb_release_ptr->rb_InformationAffectedList;
      localrb_count = 0;
      while ( rb_info_affected_list_ptr->n > localrb_count)
      {
        /*check whether rb id is signalling rbid or does not exist in established
        rabs */
        if((rb_info_affected_list_ptr->elem[localrb_count].rb_Identity > DCCH_DT_LOW_PRI_RADIO_BEARER_ID) &&
          (!rrc_find_rb_in_est_rabs((rb_info_affected_list_ptr->elem[localrb_count].rb_Identity))))
        {
          /* RB Message was invalid. */
          WRRC_MSG1_HIGH("REL9: Invalid RB id:%d in RBR Message", rb_info_affected_list_ptr->elem[localrb_count].rb_Identity);
          /* Send the RB Release Failure Message */
          rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          send_rbr_failure_message( rbr.status.failure_status, rbr.status.prot_err_cause);

          rrcrbr_clear_procedure();
          /* Make sure we're back in the initial sub-state */
          rbr.curr_substate = RRCRB_INITIAL;
          return;
        }
        rbr.rbs_tobe_affected.rb_id[localrb_count] = 
        rb_info_affected_list_ptr->elem[localrb_count].rb_Identity;
        localrb_count++;
      }
      rbr.rbs_tobe_affected.num_rbs= localrb_count;
      WRRC_MSG1_HIGH("REL9: Number of RBs tobe affected:%d", localrb_count);
    }

    /* If there is a signalling connection to be released,
    or if there are individual RBs to be released, we
    will be releasing some RBs. */
    if(RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,
      signallingConnectionRelIndication))
    {
      /* Need to release a CN domain */
      rbr.need_to_release_cn_domain = TRUE; 
      /* Copy the domain that is to be released */
      rbr.cn_to_release = rb_release_ptr->signallingConnectionRelIndication;

      for(rab_count = 0; rab_count< MAX_RAB_TO_SETUP; rab_count++)
      {
        if((rrc_est_rabs.rabs[rab_count].cn_domain == rbr.cn_to_release) &&
          (rrc_est_rabs.rabs[rab_count].num_rbs_for_rab > 0))
        {
          rab_found = TRUE;
          for ( rb_count=0; rb_count<rrc_est_rabs.rabs[rab_count].num_rbs_for_rab; rb_count++ )
          {
            rb_rel_srch = FALSE;
            for(localrb_count=0; localrb_count<rbr.rbs_tobe_released.num_rbs; localrb_count++)
            {
              if(rrc_est_rabs.rabs[rab_count].rb_for_rab[rb_count].rb_id == 
                  rbr.rbs_tobe_released.rb_id[localrb_count])
              {
                rb_rel_srch = TRUE;
                break;
              }
            }
            if(rb_rel_srch == FALSE) // est rabs has an rb_id not in the rb release list
            {
              /* send failure message */
              rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
              send_rbr_failure_message( rbr.status.failure_status, rbr.status.prot_err_cause);
              /* Go back to initial substate */
              rbr.curr_substate = RRCRB_INITIAL;
              /* Then clear all local variables that may need to be cleared */
              rrcrbr_clear_procedure();
              return;
            }
          }/* rb_count loop */       
        } /*end of if loop */
      }/* rab_count loop */

      /*if none of the rabs in the establishedrabs have rb information*/
      if(rab_found==FALSE )
      {
        WRRC_MSG1_HIGH("REL9: RB lookup in EST_RABS for cn:%d failed", rbr.cn_to_release );
        /* send failure message */
        rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        send_rbr_failure_message( rbr.status.failure_status, rbr.status.prot_err_cause);
        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;
        /* Then clear all local variables that may
        need to be cleared */
        rrcrbr_clear_procedure();
        return;
      }
    }

    /* Check if there is CN information present. If so, we need to send the information to the Cell Change manager */
    if (RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr,cn_InformationInfo))
    {
      WRRC_MSG0_HIGH("REL9: Saving CN info for NAS");
      rbr.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(
        &(rb_release_ptr->cn_InformationInfo) );
      if (rbr.mm_cmd_ptr != NULL)
      {
        /* set cn_info_saved flag */
        rbr.cn_info_saved = TRUE;
      }
    }
#ifdef FEATURE_WCDMA_REL7_SPECRS
    /*If RBR OTA provides "supportForChangeOfUE_Capability" IE, we need to store it in
           "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
           "rrc_rnc_cap_change_support" can be updated accordingly. */
    if(RRCRB_R9_MSG_IE_PRESENT(rb_release_ptr, 
      supportForChangeOfUE_Capability))
    {
      if(rb_release_ptr->supportForChangeOfUE_Capability == TRUE)
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
#endif /*FEATURE_WCDMA_REL7_SPECRS*/

    /* Indicate layer1 if rb release message is going to release voice RAB */
    rrcrbr_indicate_layer1_if_config_msg_release_voice_rab();

    rrcrb_update_mac_rab_status(RRC_PROCEDURE_RBR, RRC_CS_DOMAIN_CN_ID,
                                &rb_release_ptr->rb_InformationReleaseList, FALSE);

    /* Set ORDERED_CONFIG so that LLC can process the necessary parameters. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_RBR, rbr.state_indicated_by_msg,
                      rrc_DL_DCCH_Message_PDU, (void *)dcch_msg_ptr);

    switch ( oc_status )
    {
      case ORDERED_CONFIG_SET:
        rrcrbr_process_after_oc_config_set_r9(dcch_msg_ptr, rb_release_ptr);
        break;

      case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

        rbr.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
        /* Send the RB Release Failure Message */
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

        rbr.status.failure_status =RRCRB_MSG_UNSUPPORTED_CONFIG;
        /* Send the RB Release Failure Message */
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_INVALID:

        rbr.status.failure_status =RRCRB_MSG_INVALID_CONFIG;
        /* Send the RB Release Failure Message */
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
      
      case ORDERED_CONFIG_NOT_SET_OTHER:
        WRRC_MSG0_HIGH("LLC returned other failure!");
        /* When LLC sends this, there is a serious problem
        and we'll go back to disconnected state. No need
        to send the failure message here, we can just
        go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME        
      case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("RBR processing deferred due to LTA!");
      
        rbr.curr_substate = RRCRB_INITIAL;
      
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
      default:

        WRRC_MSG0_ERROR("Unknown return value from LLC");
        /* Unknown error, just go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
    }/* end switch */

    if ( ordered_config_err_occured == TRUE )
    {
        /* No need to continue. Clear the procedure
        variables and return. */
        rrcrbr_clear_procedure();
        return;
    }

    if(rbr.rbr_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }
  }/* end if - valid rb message */
  return;
}


#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif /* FEATURE_WCDMA_REL10 */


/*===========================================================================

FUNCTION   RRCRBR_PROCESS_RBRELEASE_MESSAGE

DESCRIPTION

  This function processes the received RB Release Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a Radio Bearer Release Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to release the Radio Bearers given by the message.
    
    If ORDERED_CONFIG could not be set, a Radio Bearer Release Failure message
    will be sent to UTRAN.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbr_process_rbrelease_message
(
  rrc_cmd_type *cmd_ptr
  
)
{

  rrc_RB_InformationReleaseList  *rb_info_rel_list_ptr;
  rrc_RB_InformationAffectedList *rb_info_affected_list_ptr;
  rrc_RadioBearerRelease *msg_ptr;  /* Pointer to the RB Release message        */
  rrcrb_directed_cell_info_type directed_cell_info; /* directed cell info struct */

  rrcllc_oc_status_e_type oc_status;/* To hold ordered_config status          */
  boolean ordered_config_err_occured = FALSE; /* Used to indicate if an error
                                    occured while setting ordered_config. */
  boolean rb_rel_srch = FALSE; 
  boolean rab_found = FALSE; 
  rrc_state_e_type current_state;       /* current RRC State  */
  uint32 rab_count, rb_count, localrb_count,local_idx=0;
  /* Local C_RNTI value */
  rrc_int_c_rnti_type              c_rnti;
  rrc_int_u_rnti_type              u_rnti;
  rrcrb_rnti_update_info_type      rnti_update_info;   /* to hold rnti update info */


  rrc_msg_ver_enum_type msg_version = MSG_REL99;

  rrcsmc_srns_ip_check_status_e_type    ip_check_status = RRCSMC_SRNS_INTEGRITY_CHECK_FAILURE;

  rrc_DL_DCCH_Message *dcch_msg_ptr ;     /* Pointer to the RB Release message */

   /*Boolean used to indicate primary plmn i.d is present in OTA msg*/
  boolean primary_plmn_Id_present = FALSE;
   /*Variable to hold primary plmn i.d if present in OTA msg*/
  rrc_PLMN_Identity primary_plmn_Identity;

  /* A Downlink SDU has been received from RLC. This
  SDU should contain a Radio Bearer Release Message. Otherwise
  we would not have received this command. Copy the 
  received message pointer. */
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  /* If ASN.1 decoding failed, the received pointer would be null.
  So we know we got a RB Release Message (by peeking into
  the encoded bit stream), but we were not able to decode it.
  Send a RB release failure message with the appropriate
  cause. */
  if ( dcch_msg_ptr == NULL )
  {
    WRRC_MSG0_HIGH("ASN decode failed for RBR msg");
    rbr.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
    rbr.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
    send_rbr_failure_message( rbr.status.failure_status,
                              rbr.status.prot_err_cause
                            );

    /* Then change to initial substate */
    rbr.curr_substate = RRCRB_INITIAL;

    /* Then clear all local variables that may
    need to be cleared */
    rrcrbr_clear_procedure();

    /* Nothing else to do here */
    return;
  }

  /* Make sure it's the Radio Bearer Release message
  although we would not have gotten here unless it's
  the right message */
  if ( dcch_msg_ptr->message.t != T_rrc_DL_DCCH_MessageType_radioBearerRelease )
  {
    WRRC_MSG1_ERROR("Incorrect message given to RBR proc %d", dcch_msg_ptr->message.t);
    return;
  }

  /* Get the pointer to the actual RB Release message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerRelease);

  /* The first thing to do is check the transaction id.
  If the transaction id is the same as the accepted
  or rejected transaction ids, we don't process
  this message. It's a duplicate and will just be
  dropped. But we also don't change the current
  sub-state since we're in the middle of
  processing a previous message. Note that
  the transaction id is included only if critical
  extensions are not present. */
  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) 
  {
    if( msg_ptr->t != T_rrc_RadioBearerRelease_r3 )
    {
      if ((msg_ptr->u.later_than_r3.criticalExtensions.t == 
           T_rrc_RadioBearerRelease_criticalExtensions_5_criticalExtensions) &&
          (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.t == 
           T_rrc_RadioBearerRelease_criticalExtensions_4_r5))
        
      {
        MSG_LOW("RRCHS:rbreleaser5",0,0,0);
        rrcrbr_process_rbrelease_r5(cmd_ptr);
        return;
      }
      else if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6) &&
              (msg_ptr->u.later_than_r3.criticalExtensions.t == 
               T_rrc_RadioBearerRelease_criticalExtensions_5_criticalExtensions) &&
              (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.t == 
               T_rrc_RadioBearerRelease_criticalExtensions_4_criticalExtensions) &&
              (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t &
               T_rrc_RadioBearerRelease_criticalExtensions_3_r6))
      {
        rrcrbr_process_rbrelease_r6(cmd_ptr);
        return;
      }
      else if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7) &&
              (msg_ptr->u.later_than_r3.criticalExtensions.t == 
               T_rrc_RadioBearerRelease_criticalExtensions_5_criticalExtensions) &&
              (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.t == 
               T_rrc_RadioBearerRelease_criticalExtensions_4_criticalExtensions) &&
              (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t &
               T_rrc_RadioBearerRelease_criticalExtensions_3_criticalExtensions)
               &&  (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t &
               T_rrc_RadioBearerRelease_criticalExtensions_2_r7))
      {
        rrcrbr_process_rbrelease_r7(cmd_ptr);
        return;
      }
      else if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8) &&
            (RRCRB_R8_CRITICAL_EXT_PRESENT_PTR(msg_ptr, 
            rrc_RadioBearerRelease)))
      {
        rrcrbr_process_rbrelease_r8(cmd_ptr);
        return;
      }

#ifdef FEATURE_WCDMA_REL9
     else if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9) &&
           (RRCRB_R9_CRITICAL_EXT_PRESENT_PTR(msg_ptr, 
           rrc_RadioBearerRelease)))
     {
       rrcrbr_process_rbrelease_r9(cmd_ptr);
       return;
     }
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
  #error code not present
#endif /* FEATURE_WCDMA_REL10 */


    }
  }
  if( msg_ptr->t != T_rrc_RadioBearerRelease_r3 )
  {
    rrc_print_supported_asn1_rel_version();

    WRRC_MSG1_HIGH("Unsupported RB Rel t:%d", msg_ptr->t);

    if ( (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier)
                   ==  rbr.tr_ids.rejected_transaction )
    {
      WRRC_MSG1_HIGH("Duplicate RBR msg! rejected tr-id%d match!", rbr.tr_ids.rejected_transaction);
      /* Nothing else to do, just ignore the message and return */
    } 
    else
    {
      /* We have received a RBR message with a new transaction id.
      This message should be in our rejected transaction
      list */
      rbr.tr_ids.rejected_transaction = 
        (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
      rbr.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
      rbr.status.prot_err_cause = rrc_ProtocolErrorCause_messageExtensionNotComprehended;
      /*Check whether Cell Update is going on */
      if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
           (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
      {
        if(rbr.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
        {
          send_rbr_failure_message( rbr.status.failure_status,
                                    rbr.status.prot_err_cause );
          rbr.status.failure_status = RRCRB_MSG_VALID;
        }
        else
        {
          WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
          (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBR);
          /* Wait until Cell update procedure is completed */
          rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        }
      }
      else
      {
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause );
        /*continue the same substate */ 
      }
    }
    return;
  }

  WRRC_MSG0_HIGH("RRCHS:rbreleaser99 rcvd");
  if (TRUE == rrcrbr_validate_srns_relocation_info(msg_ptr, msg_version))
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
        break;

      case RRCSMC_SRNS_SECURITY_INFO_INVALID:
      case RRCSMC_INCOMPATIBLE_SECURITY_RECONFIGURATION:
        if (msg_ptr->u.r3.radioBearerRelease_r3.rrc_TransactionIdentifier
                       == rbr.tr_ids.rejected_transaction)
        {
          WRRC_MSG1_HIGH("Invalid security config-rejected tr-id%d match!",
                   rbr.tr_ids.rejected_transaction);
          /* Nothing else to do, just ignore the message and return */
        } 
        else
        {
          /* send a reconfig failure messge with invalid
          configuration and 
          continue the same substate with any ongoing procedure */
          rbr.tr_ids.rejected_transaction = 
            (msg_ptr->u.r3.radioBearerRelease_r3.rrc_TransactionIdentifier);
          if (ip_check_status == RRCSMC_SRNS_SECURITY_INFO_INVALID)
          {
            rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          }
          else
          {
            rbr.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
          }
          /*Check whether Cell Update is going on */
          if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
               (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
          {
            if(rbr.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
            {
              send_rbr_failure_message( rbr.status.failure_status,
                                       rbr.status.prot_err_cause );
              rbr.status.failure_status = RRCRB_MSG_VALID;
            }
            else
            {
              WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
              (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBR);
              /* Wait until Cell update procedure is completed */
              rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
            }
          }
          else
          {
            send_rbr_failure_message( rbr.status.failure_status,
                                            rbr.status.prot_err_cause );
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
      /*do not clear the procedure or change the rbr substate because
      some other rbe procedure might be in progress. Just return from
      here */
      return;
    }
  }

  if ((rrcllc_get_ordered_config_status() != OC_NOT_SET) ||
      (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
      (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
  {
     MSG_HIGH("Incomatible simultaneous reconfig! Rejecting RBR message,transaction_id:%d,accepted_transaction:%d",
         msg_ptr->u.later_than_r3.rrc_TransactionIdentifier,rbr.tr_ids.accepted_transaction,rbr.tr_ids.rejected_transaction );
     if((msg_ptr->u.later_than_r3.rrc_TransactionIdentifier != rbr.tr_ids.accepted_transaction) && 
         (msg_ptr->u.later_than_r3.rrc_TransactionIdentifier != rbr.tr_ids.rejected_transaction))
     {
       /* We have received a RBR message with a new transaction id.
       This message should be in our rejected transaction
       list */
       rbr.tr_ids.rejected_transaction = 
         (msg_ptr->u.r3.radioBearerRelease_r3.rrc_TransactionIdentifier);
       rbr.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;

       /*Check whether Cell Update is going on */
       if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
            (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
       {
         if(rbr.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
         {
           send_rbr_failure_message( rbr.status.failure_status,
                                     rbr.status.prot_err_cause );
           rbr.status.failure_status = RRCRB_MSG_VALID;
         }
         else
         {
           WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
           (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBR);
           /* Wait until Cell update procedure is completed */
           rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
         }
       }
       else
       {
         send_rbr_failure_message( rbr.status.failure_status,
                                         rbr.status.prot_err_cause );
         /*continue the same substate */ 
       }
     }
     return;
  }

  /* We have received a RB release message with a new transaction id.
  Accept the transaction Id */
  rbr.tr_ids.accepted_transaction =
     msg_ptr->u.r3.radioBearerRelease_r3.rrc_TransactionIdentifier;

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  validate_rbrelease_message( msg_ptr );

  if ( rbr.status.failure_status != RRCRB_MSG_VALID )
  {
    /* RB Message was invalid. */
    WRRC_MSG0_HIGH("RBR Message validation failed");

    /* Send the RB Release Failure Message */
    send_rbr_failure_message( rbr.status.failure_status,
                              rbr.status.prot_err_cause
                            );

    rrcrbr_clear_procedure();
    /* Make sure we're back in the initial sub-state */
    rbr.curr_substate = RRCRB_INITIAL;
  }
  /* If the message is valid, proceed further */
  else
  {

    WRRC_MSG0_HIGH("Processing RBR Message");

    rrcrbr_process_high_mobility_ind_in_non_critical_ext(msg_ptr,MSG_REL99);


    /*store the rb ids tobe released */
    rbr.rbs_tobe_released.is_valid = TRUE;
    rb_info_rel_list_ptr = &msg_ptr->u.r3.radioBearerRelease_r3.rb_InformationReleaseList;
    localrb_count = 0;
	local_idx =0;
    while ( rb_info_rel_list_ptr->n > local_idx)
    {
      /*check whether rb id is signalling rbid or does not exist in established
      rabs */
      if(( rb_info_rel_list_ptr->elem[local_idx] < DCCH_DT_LOW_PRI_RADIO_BEARER_ID))
      {
        /* RB Message was invalid. */
        WRRC_MSG1_HIGH("Invalid RB id:%d in RBR Message", rb_info_rel_list_ptr->elem[local_idx]);
        /* Send the RB Release Failure Message */
        rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );
        rrcrbr_clear_procedure();
        /* Make sure we're back in the initial sub-state */
        rbr.curr_substate = RRCRB_INITIAL;
        return;

      }
      else if(! rrc_find_rb_in_est_rabs((rb_info_rel_list_ptr->elem[local_idx]))) 
      {
        WRRC_MSG1_HIGH("Ignore Non-exitent rb_id in rbr msg",rb_info_rel_list_ptr->elem[local_idx]);
        local_idx++;
        continue;
      }

      rbr.rbs_tobe_released.rb_id[localrb_count] = rb_info_rel_list_ptr->elem[local_idx];
      localrb_count++;
	  local_idx++;
    }
    rbr.rbs_tobe_released.num_rbs = localrb_count;
    WRRC_MSG1_HIGH("Number ofRBs tobe released:%d", localrb_count);

    /*store the rb ids tobe affected */
    if ( RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3,
          rrc_RadioBearerRelease_r3_IEs,rb_InformationAffectedList))
    {
      rbr.rbs_tobe_affected.is_valid = TRUE;
      rb_info_affected_list_ptr = 
      &msg_ptr->u.r3.radioBearerRelease_r3.rb_InformationAffectedList;
      localrb_count = 0;
      while ( rb_info_affected_list_ptr->n > localrb_count )
      {
        /*check whether rb id is signalling rbid or does not exist in established
        rabs */
        if(( rb_info_affected_list_ptr->elem[localrb_count].rb_Identity > DCCH_DT_LOW_PRI_RADIO_BEARER_ID)&&
           (! rrc_find_rb_in_est_rabs((rb_info_affected_list_ptr->elem[localrb_count].rb_Identity))))
        {
          /* RB Message was invalid. */
          WRRC_MSG1_HIGH("Invalid RB id:%d in RBR Message", rb_info_affected_list_ptr->elem[localrb_count].rb_Identity);
          /* Send the RB Release Failure Message */
          rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          send_rbr_failure_message( rbr.status.failure_status,
                                    rbr.status.prot_err_cause
                                  );

          rrcrbr_clear_procedure();
          /* Make sure we're back in the initial sub-state */
          rbr.curr_substate = RRCRB_INITIAL;
          return;

        }
        rbr.rbs_tobe_affected.rb_id[localrb_count] = 
        rb_info_affected_list_ptr->elem[localrb_count].rb_Identity;
        localrb_count++;
      }
      rbr.rbs_tobe_affected.num_rbs= localrb_count;
      WRRC_MSG1_HIGH("Number ofRBs tobe affected:%d", localrb_count);
    }


    /* If there is a signalling connection to be released,
    or if there are individual RBs to be released, we
    will be releasing some RBs. */
    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3,
        rrc_RadioBearerRelease_r3_IEs,signallingConnectionRelIndication))
    {
      rrcrbr_mark_cn_doamin_to_be_closed(msg_ptr->u.r3.radioBearerRelease_r3.signallingConnectionRelIndication);
  
      for ( rab_count = 0; rab_count< MAX_RAB_TO_SETUP; rab_count++ )
      {
        if ( (rrc_est_rabs.rabs[rab_count].cn_domain == rbr.cn_to_release) &&
             (rrc_est_rabs.rabs[rab_count].num_rbs_for_rab > 0) )
        {
          rab_found = TRUE;
          for ( rb_count=0; rb_count<rrc_est_rabs.rabs[rab_count].num_rbs_for_rab; rb_count++ )
          {
            rb_rel_srch = FALSE;
            for ( localrb_count=0; localrb_count<rbr.rbs_tobe_released.num_rbs; localrb_count++ )
            {
              if ( rrc_est_rabs.rabs[rab_count].rb_for_rab[rb_count].rb_id == 
                   rbr.rbs_tobe_released.rb_id[localrb_count] )
              {
                rb_rel_srch = TRUE;
                break;
              }
            }
            if ( rb_rel_srch == FALSE ) // est rabs has an rb_id not in the rb release list
            {
              /* send failure message */
              rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
              send_rbr_failure_message( rbr.status.failure_status,
                                        rbr.status.prot_err_cause);
              /* Go back to initial substate */
              rbr.curr_substate = RRCRB_INITIAL;
              /* Then clear all local variables that may
              need to be cleared */
              rrcrbr_clear_procedure();
              return;
            }
          }/* rb_count loop */       
        } /*end of if loop */
      }/* rab_count loop */
      /*if non of the rabs in the establishedrabs have rb information*/
      if ( rab_found==FALSE )
      {
        WRRC_MSG1_HIGH("RB lookup in est_rabs for cn:%d failed", rbr.cn_to_release );
        /* send failure message */
        rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause);
        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;
        /* Then clear all local variables that may
        need to be cleared */
        rrcrbr_clear_procedure();
        return;

      }
    }

    /* Check if there is CN information present. If so, we need
    to send the information to the Cell Change manager */
    if ( RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3,
          rrc_RadioBearerRelease_r3_IEs,cn_InformationInfo))
    {
      WRRC_MSG0_HIGH("Saving CN info for NAS");
     
      /*Check to see if primary PLMN I.D is present in RBR message ext
       * rrc_RadioBearerRelease-v690ext which is present in 
       * v690NonCriticalExtensions
       */
       primary_plmn_Id_present = rrcrbr_check_and_get_primary_plmn_id(msg_ptr,
                                                                  &primary_plmn_Identity,MSG_REL99);
      
      rbr.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info(
                      &(msg_ptr->u.r3.radioBearerRelease_r3.cn_InformationInfo)
                     ,primary_plmn_Id_present
                     ,&primary_plmn_Identity
           );
      if (rbr.mm_cmd_ptr != NULL)
      {
        /* set cn_info_saved flag */
        rbr.cn_info_saved = TRUE;
      }
    }

    /* Indicate layer1 if rb release message is going to release voice RAB */
    rrcrbr_indicate_layer1_if_config_msg_release_voice_rab();

    rrcrb_update_mac_rab_status(RRC_PROCEDURE_RBR, RRC_CS_DOMAIN_CN_ID,
                                &msg_ptr->u.r3.radioBearerRelease_r3.rb_InformationReleaseList,
                                FALSE);

    /* Set ORDERED_CONFIG so that LLC can process the
    necessary parameters. */
    oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_RBR,
                                           rbr.state_indicated_by_msg,
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
        current_state = rrc_get_state(); 
        /* Populate the local variables to indicate if Re-direction info is present */
        directed_cell_info.freq_info_present = FALSE;
        directed_cell_info.pscr_present = FALSE; 
        if((current_state==RRC_STATE_CELL_FACH) && 
               ( (rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH)
               ||(rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH)
              || (rbr.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          if((RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3,
                   rrc_RadioBearerRelease_r3_IEs,frequencyInfo)) &&
                  (RRC_CHECK_COMMON_MSG_TYPE(msg_ptr->u.r3.radioBearerRelease_r3.frequencyInfo.
                   modeSpecificInfo,rrc_FrequencyInfo_modeSpecificInfo_fdd)))
          {
            directed_cell_info.freq_info_present = TRUE;
            directed_cell_info.uarfcn_dl         = msg_ptr->u.r3.radioBearerRelease_r3.
                                                   frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
          }
          
          if (rrcrbr_is_pcpich_info_present(msg_ptr,MSG_REL99))
          {
            directed_cell_info.pscr_present = TRUE;
            directed_cell_info.pscr = msg_ptr->u.r3.radioBearerRelease_r3.dl_InformationPerRL_List.elem[0].
                                      modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
          }
          /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
                  FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
          if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
            (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
          {
            WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
            if(rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH)
            {
              rrcrbr_send_initiate_cu_flag = TRUE;
              rrcrbr_delete_crnti          = TRUE;
            }
            else
            {
              rbr.directed_cell_info = TRUE;
            }
          }                  
        }
        if (
            (current_state==rbr.state_indicated_by_msg) ||
            ((current_state==RRC_STATE_CELL_FACH) && 
             (rbr.state_indicated_by_msg==RRC_STATE_CELL_DCH))
            )
        {
          /* CELL_DCH->CELL_DCH or CELL_FACH->CELL_FACH or 
          CELL_FACH->CELL_DCH case. We can release the
          Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */
          MSG_MED("Sending RRC_CHANNEL_CONFIG_REQ to LLC", 0, 0, 0);
          /*check whether any frequency info is present in the message
          and transition is CELL_FACH->CELL_FACH. In this case
          initiate a cell update after channel config is successfully completed */
          if ((current_state==RRC_STATE_CELL_FACH)&& 
             (rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH))
          {
            /*call the function that evaluates the directed cell info and set the flags accordingly */
            if ((rrcrbr_send_initiate_cu_flag == FALSE) &&
                (rrcrb_validate_directed_cell_info (directed_cell_info)== FALSE))
            {
              if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3,
                  rrc_RadioBearerRelease_r3_IEs,new_C_RNTI))
              {
                rbr.new_crnti_valid = TRUE;
                rrc_translate_crnti(&msg_ptr->u.r3.radioBearerRelease_r3.new_C_RNTI, 
                                    &c_rnti);
                rbr.new_crnti = (uint16)c_rnti;
              }
              else
              {
                rbr.new_crnti_valid = FALSE;
              }

              rrcllcoc_update_toc_with_oc_from_fach();
              /*Initiate the cell selection*/
              rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBR,
                                                      RRC_TRANSITION_FROM_FACH,
                                                      rbr.state_indicated_by_msg);
            
              rbr.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
              /* exit from the switch case */
              break;
            }
          } /* if CELL_FACH->CELL_FACH reconfiguration */

          /* Call the function that creates and sends a channel
            config request to LLC. */
          rrcrbr_send_channel_config_req();

          /* Change the substate since we're waiting for the
             confirmation from LLC for the channel configs */
          rbr.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (rbr.state_indicated_by_msg==RRC_STATE_CELL_FACH))
        {
          if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3,
             rrc_RadioBearerRelease_r3_IEs,new_C_RNTI))
          {
            rbr.new_crnti_valid = TRUE;
            rrc_translate_crnti(&msg_ptr->u.r3.radioBearerRelease_r3.new_C_RNTI, 
                                &c_rnti);
            rbr.new_crnti = (uint16)c_rnti;
          }
          else
          {
            rbr.new_crnti_valid = FALSE;
          }

          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBR,
                                                  RRC_TRANSITION_FROM_DCH,
                                                  rbr.state_indicated_by_msg);
          rbr.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

          /* Start T_305 [periodic cell update] timer */
          rrccu_start_t_305_timer();
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                 (rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH))
        {
          /*store the directed cell info */
           rbr.directed_cell_info = rrcrbr_is_pcpich_info_present(msg_ptr,MSG_REL99);

           /*Send the rbr complete msg on old configuration */
           send_rbr_complete_message(TRUE);
           /*wait for L2ack */
           rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
           rrcllc_print_trans_chnl_rate();
        }
        else if((current_state==RRC_STATE_CELL_DCH) && 
                (rbr.state_indicated_by_msg==RRC_STATE_URA_PCH))
        {
          /*Send the RBR complete msg on old configuration */
          send_rbr_complete_message(TRUE);
          /*wait for L2ack */
          rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
          rrcllc_print_trans_chnl_rate();
        }
        else if((current_state==RRC_STATE_CELL_FACH) && 
                ((rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
                 (rbr.state_indicated_by_msg==RRC_STATE_URA_PCH)))
        {
          /*call the function that evaluates the directed cell info and set the flags accordingly */
          if ((rbr.directed_cell_info == FALSE) &&
            (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
          {
            if(rbr.state_indicated_by_msg==RRC_STATE_CELL_PCH)
            {
              rbr.directed_cell_info = rrcrbr_is_pcpich_info_present(msg_ptr,MSG_REL99);
            }
            rbr.initiate_cell_selection_trans_from_fach  = TRUE;
          }

          /* check whether CPICH info is present or not. Store this
          info  */
         // rbr.directed_cell_info = rrcrbr_is_pcpich_info_present(msg_ptr);
          /*Send the RRCTCTC complete msg on old configuration */
          /* check whether new C-RNTI was present in the message. In that case
          we have to configure MAC with new C-RNTI before sending the response 
          message */
          if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3,
             rrc_RadioBearerRelease_r3_IEs,new_C_RNTI))
          {
            rrc_translate_crnti(&msg_ptr->u.r3.radioBearerRelease_r3.new_C_RNTI, 
                                &c_rnti);
            /* Send a RNTI Update Request */
            rnti_update_info.crnti_action    = RRC_RNTI_UPDATE;
            rnti_update_info.crnti           = (uint16)c_rnti;
            /* check whether URNTI is also present in the message */
            if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerRelease_r3,
             rrc_RadioBearerRelease_r3_IEs,new_U_RNTI))
            {
              rrc_translate_urnti(&msg_ptr->u.r3.radioBearerRelease_r3.new_U_RNTI, 
                                  &u_rnti);
              rnti_update_info.urnti_action    = RRC_RNTI_UPDATE; 
              rnti_update_info.urnti           = u_rnti; 
            }
            else
            {
              rnti_update_info.urnti_action    = RRC_RNTI_NOCHANGE; 
            }
            rnti_update_info.procedure            = RRC_PROCEDURE_RBR;
            rnti_update_info.rlc_re_establish_srb = FALSE;
            rnti_update_info.rlc_re_establish_rab = FALSE;
            rnti_update_info.cnf_reqd             = TRUE;
            rrcrb_send_rnti_update_req(rnti_update_info);
            /* wait for the RNTI update confirm before sending the response
            message */
            rbr.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
          }
          else 
          {
            send_rbr_complete_message(TRUE);
            /*wait for L2ack */
            rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
            rrcllc_print_trans_chnl_rate();
          }
        }
        break;

      case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:

        rbr.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
        /* Send the RB Release Failure Message */
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

        rbr.status.failure_status =RRCRB_MSG_UNSUPPORTED_CONFIG;
        /* Send the RB Release Failure Message */
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
		
      case ORDERED_CONFIG_CONFIGURATION_INVALID:

        rbr.status.failure_status =RRCRB_MSG_INVALID_CONFIG;
        /* Send the RB Release Failure Message */
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

      case ORDERED_CONFIG_NOT_SET_OTHER:
        WRRC_MSG0_HIGH("LLC returned other failure!");
        /* When LLC sends this, there is a serious problem
        and we'll go back to disconnected state. No need
        to send the failure message here, we can just
        go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME        
      case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("RBR processing deferred due to LTA!");
      
        rbr.curr_substate = RRCRB_INITIAL;
      
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
      default:

        WRRC_MSG0_ERROR("Unknown return value from LLC");
        /* Unknown error, just go back to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
    }/* end switch */

    if ( ordered_config_err_occured == TRUE )
    {
        /* No need to continue. Clear the procedure
        variables and return. */
        rrcrbr_clear_procedure();
        return;
    }

    if(rbr.rbr_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }
  }/* end if - valid rb message */
}/* end function rrcrbr_process_rbrelease_message */


/*===========================================================================

FUNCTION   RRCRBR_PROCESS_RNTI_UPDATE_CNF

DESCRIPTION

  This function processes RNTI_UPDATE_CNF
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbr_process_rnti_update_cnf
(
void
)
{
  rrc_state_e_type curr_state;

  curr_state = rrc_get_state();

  if ((curr_state == RRC_STATE_CELL_FACH) && 
      ((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
       (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH)))
  {
   /*send the response message now */
    send_rbr_complete_message(TRUE);
    /*wait for L2ack */
    rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
    rrcllc_print_trans_chnl_rate();
  }
  else
  {
    /* We should not get here*/
    WRRC_MSG1_HIGH("Unexpected RNTI Update cnf,RBR state %d", 
                                   rbr.curr_substate);
  }
}

/*===========================================================================

FUNCTION   RRCRBR_PROCESS_CHANNEL_CONFIG_CNF

DESCRIPTION

  This function processes the received Channel Config Confirmation
  from LLC. If the required Radio-bearers have been released succesfully,
  and if any of the radio-access-bearers have to be released because
  all the radio-bearers mapped to those RABs have been released, this
  information needs to be given to the NAS entity for that RAB.
  This is done by sending an RRC_SYNC_IND to MM.
  
  If a signalling connection has been released, this procedure has
  to notify the RRC Connection Establishment procedure since the
  RRC connection establishment procedure keeps track of all the
  signalling connections established for NAS.
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbr_process_channel_config_cnf
(
rrc_channel_config_cnf_type *ptr  /* Pointer to the Channel config cnf cmd */
)
{
  rrcrb_rnti_update_info_type rnti_update_info;   /* to hold rnti update info */
  mm_cmd_type *mm_ptr;   /* To send the MM_SYNC_IND command */
  rrc_cmd_type *cmd; /* To send a LLC command if channel config failed */
  rabm_cmd_type *rabm_ptr; /* To send the RABMAS_RAB_RELEASE_IND  command */
  rrc_rb_remove_result_type rab_result; /* To hold result
                         of removing RBs from ESTABLISHED_RABS */
  uint8 count;          /* Local counter */
  rrc_CellUpdateCause       cu_cause;  /*To store CU cause from CU procedure*/
  rrc_state_e_type curr_state;
#ifdef FEATURE_SCUDIF_ENHANCEMENT
  #error code not present
#endif
  boolean amr_configured = FALSE;
  boolean cs_rab_released = FALSE;
  rrc_rlc_re_est_for_srns_rb_e_type rb_type; /*rb type tobe reestablished */
#ifdef FEATURE_VOC_AMR_WB
  mvs_mode_type current_mode = rrc_get_current_codec_mode();
#endif/*FEATURE_VOC_AMR_WB*/
#ifdef FEATURE_DUAL_SIM
  boolean rab_removed = FALSE;
#endif

  rrcllcpcie_initialise_sync_a_post_veri_info();


  /* Check if the channel configuration was succesful. */
  if( ptr->chan_cfg_status != RRCLLC_CHAN_CFG_SUCCESS )
  {
     /*
        Since channel config comfirm came with a failure, hence revert the max count c value
        in the temporary variable to the original value
    */
    rrcsmc_revert_max_count_c();
     
#ifndef FEATURE_WCDMA_HS_FACH
    /* Channel config failure, so initialise the ordered RB-Mapping database*/
    rrcllc_init_ordered_hanging_rb_mapping_info();
#else
   rrcllc_copy_current_hanging_rb_mapping_to_ordered();
#endif


    /*Here we initialize rlc_size_change_in_progress DB so that if it was set by RBR, 
    it is ready for next procedure*/
    rrc_initialize_rlc_size_change();

    WRRC_MSG0_ERROR("CHAN_CONFIG_CNF with Failure");
#ifdef FEATURE_VOC_AMR_WB
    rrc_set_ordered_codec_mode(current_mode);
#endif/*FEATURE_VOC_AMR_WB*/
    /* Roll back mac_rab_status */
    rrcrb_update_mac_rab_status(RRC_PROCEDURE_RBR, RRC_CS_DOMAIN_CN_ID,NULL, TRUE);
    
    rbr.status.failure_status = RRCRB_PHY_CHAN_FAILURE;
    if (ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_CFG_CANCELLED)
    {
      /* cell update is already in progress as this is for pending config
       * cancellation
       */

      rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      return; 
    }
    
    /* validation failure happened.  Send RB failure msg with invalid config */
    if (ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_OTHER)
    {
      
      rbr.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      if(rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG)
      {
        rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBR, RRC_UNRECOVERABLE_ERROR, FALSE);
        return; 
      }

      WRRC_MSG0_HIGH("Sending RBR failure msg with invalid config");
      send_rbr_failure_message( rbr.status.failure_status,
                                rbr.status.prot_err_cause
                                );
      /* Reset ORDERED_CONFIG - UE can now accept other ordered
      configurations */
      if (OC_NOT_SET == rrcllc_reset_ordered_config(RRC_PROCEDURE_RBR
                                                    ,TRUE
                                                    ))
      {
        /* Then clear all local variables that may
        need to be cleared */
        rrcrbr_clear_procedure();
        /* Make sure we're back in the initial sub-state */
        rbr.curr_substate = RRCRB_INITIAL;
      }
      /*During FACH->DCH transition, if T305 got expired, we need to start T305 with zero timeout.*/	  
      if(rrc_get_state() == RRC_STATE_CELL_FACH && rbr.state_indicated_by_msg == RRC_STATE_CELL_DCH)
      {
        if((rrctmr_get_remaining_time(RRCTMR_T_305_TIMER) == 0)
            &&  (rrc_convert_t305_to_ms(RRC_GET_T305()) != RRCTMR_INFINITY_VAL ))
        {
          WRRC_MSG0_HIGH("Chan_config_req failed for FACH->DCH. So Starting T305 with zero timeout.");        
          rrctmr_start_timer(RRCTMR_T_305_TIMER, 0);
        }
      }	  
      return;
    } /* if (ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_OTHER) */

    /* Check if PHY_CHAN_FAILURE happened in FACH->DCH Transition.  If so, initiate a cell update
    request, and rbr substate should be RRCRB_WAIT_FOR_CELL_UPDATE_CNF */ 
    if (rrc_get_state() == RRC_STATE_CELL_FACH && rbr.state_indicated_by_msg == RRC_STATE_CELL_DCH)
    {
      WRRC_MSG0_HIGH("PHYCHANFAIL FACH->DCH.  Initiate CU");

      /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
      rbr.status.failure_status = RRCRB_PHY_CHAN_FAILURE;
      rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBR, RRC_PHY_CHAN_FAILURE,TRUE);
      /* Wait for CELL UPDATE to get a valid C_RNTI */
      rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      rrcrbr_send_initiate_cu_flag = FALSE;
      rrcrbr_delete_crnti          = FALSE;
      return;
    }
    /* Check if PHY_CHAN_FAILURE happended within CELL_DCH.  For now, initatiate a cell update request
       with cause RL Failure.  RBR substate should be RRCRB_WAIT_FOR_CELL_UPDATE_CNF*/
    else if (rrc_get_state() == RRC_STATE_CELL_DCH && rbr.state_indicated_by_msg == RRC_STATE_CELL_DCH)
    {
      rbr.status.failure_status = RRCRB_PHY_CHAN_FAILURE;
      if(rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG)
      {
        rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBR, RRC_RADIO_LINK_FAILURE, FALSE);
        return; 
      }
      if (ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_PHY_CHL)
      {
        WRRC_MSG0_HIGH("RBRC failed.  Going back to old config success.");
        send_rbr_failure_message( rbr.status.failure_status,
                                  rbr.status.prot_err_cause
                                  );
        /* Clear ORDERED_CONFIG - UE can now accept other ordered
        configurations */
        rrcllc_clear_ordered_config(); 
      }
      else if (ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_BACK_TO_OLD_CONFIG)
      {
        WRRC_MSG0_HIGH("RBRC failed. Going back to old config failed. Initiate CU");
        /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */

        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBR, RRC_RADIO_LINK_FAILURE,TRUE);
        /* Wait for CELL UPDATE to get a valid C_RNTI */
        rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        rrcrbr_send_initiate_cu_flag = FALSE;
        rrcrbr_delete_crnti          = FALSE;
        return;
      }
    }
    else if ((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH))
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
        cmd->cmd.chan_config_req.procedure = RRC_PROCEDURE_RBR;
        /* No need to fill any other parameter in this command.
        When LLC sees the next state is Disconnected State, it
        will tear down all the Radio Bearers. */
        rrc_put_int_cmd( cmd );
      } 
      else
      {
      rbr.status.failure_status = RRCRB_PHY_CHAN_FAILURE;
      send_rbr_failure_message( rbr.status.failure_status,
                              rbr.status.prot_err_cause
                            );
    }
    /* Go back to initial substate */
    rbr.curr_substate = RRCRB_INITIAL;
    /* At this point we don't need to remember any of the
    Radio Bearer information since it's stored in ESTABLISHED_RABS.
    Clear our internal variables */
    rrcrbr_clear_procedure();

    return;
  }
 
#ifdef FEATURE_WCDMA_REL7_SPECRS
  rrc_save_rnc_cap_change_support();
#endif /*FEATURE_WCDMA_REL7_SPECRS*/
  if((ordered_config.set_status == OC_SET_FOR_DCH_FACH_TRANS) || 
        (ordered_config.set_status == OC_SET_FOR_DCH_CELL_PCH_TRANS) ||
        (ordered_config.set_status == OC_SET_FOR_DCH_URA_PCH_TRANS))
  {
    if(rbr.high_mobility_ind)
    {
      rrcrb_send_upd_ota_parms_ind(RRC_PROCEDURE_RBR, CPHY_HIGH_MOBILITY_INCL);
    }
  }

  /*For FACH->DCH, if CHAN_CONFIG_REQ is succeeded, we need to stop T305*/  
  if(rrc_get_state() == RRC_STATE_CELL_FACH && rbr.state_indicated_by_msg == RRC_STATE_CELL_DCH)
  {
    WRRC_MSG0_HIGH("Chan_config succeeded for FACH->DCH. So Stopping T305");
    rrctmr_stop_timer(RRCTMR_T_305_TIMER);
  }    

    /*Notify the Cell_Id to registered entities*/
  if(rbr.state_indicated_by_msg == RRC_STATE_CELL_DCH)
  {
    rrc_notify_cell_id_in_dch();
  }	

  /* Copy the Ordered RB Mapping into the current RB-Mapping database*/
  rrcllc_copy_ordered_hanging_rb_mapping_to_current();


  if(ptr->inter_freq_hho_status == TRUE)
  {
    rbr.hho_ciph_status = TRUE;
  }
  else
  {
    rbr.hho_ciph_status = FALSE;  
  }

  /*If any RBs need to be released, they need to be updated in rrc_est_rabs as well.
  After releasing the RBs, if it results in releasing the RAB, NAS needs to be notified.*/
  if( rbr.rbs_tobe_released.is_valid == TRUE )
  {
    /*Loop through all the RBs that needs to be released.*/
    for( count=0; count < rbr.rbs_tobe_released.num_rbs; count++ )
    {
      /* Call the function to remove each RB */
      rrc_remove_rb_from_established_rabs( (rbr.rbs_tobe_released.rb_id[count]),
                                           &(rab_result));

      /* Now check if a RAB was removed because of this RB */
      if ( rab_result.result == RRC_RB_AND_RAB_REMOVED )
      {
        /* Since a RAB was removed, we should send the indication to the appropriate NAS entity. For CS
        RABs send the command to MM layer and for PS RABs send it to RABM layer. */
#ifdef FEATURE_DUAL_SIM
        rab_removed = TRUE;
#endif
        if ( rab_result.cn_domain == RRC_CS_DOMAIN_CN_ID )
        {
          cs_rab_released = TRUE;
          mm_ptr = mm_rrc_get_cmd_buf(RRC_SYNC_IND);
#ifdef FEATURE_DUAL_SIM
          mm_ptr->cmd.rrc_sync_ind.as_id = rrc_get_as_id();
#endif
          mm_ptr->cmd.rrc_sync_ind.cn_domain_id = RRC_CS_DOMAIN_CN_ID;
          mm_ptr->cmd.rrc_sync_ind.cause = RRC_RAB_ESTABLISHED;
          mm_ptr->cmd.rrc_sync_ind.rab_id_present = TRUE;
          mm_ptr->cmd.rrc_sync_ind.rab_info.action = RAB_RELEASED;
          mm_ptr->cmd.rrc_sync_ind.rab_info.rab_id = rab_result.rab_id;

          if(rab_result.rab_type == RRCRB_CS_VOICE_RAB)
          {
            mm_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type = RRCMN_CS_VOICE_CALL;
          }
          else if(rab_result.rab_type == RRCRB_CS_DATA_RAB)
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
          if(rab_result.rab_type == RRCRB_CS_VOICE_RAB)
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
          else
          {
            mm_ptr->cmd.rrc_sync_ind.rab_info.codec = RRC_CODEC_NONE;
          }
#endif/*FEATURE_VOC_AMR_WB*/

#ifdef FEATURE_UMTS_VOICE_CIPHERING_IND
          #error code not present
#endif
          /* Put the command on MM queue */
          /* Put the command on MM queue */
          MSG_HIGH("Dispatching out MM cmd / Sending RRC_SYNC_IND to MM for RAB type %d RAB-id %d Codec %d", 
                mm_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type, 
                mm_ptr->cmd.rrc_sync_ind.rab_info.rab_id, 
                mm_ptr->cmd.rrc_sync_ind.rab_info.codec);
          rrc_send_rrc_message_to_mm(mm_ptr);
        }/* end if cs domain */
        else if ( rab_result.cn_domain == RRC_PS_DOMAIN_CN_ID )
        {
          if ((rabm_ptr = rabm_get_cmd_buf()) == NULL)
          {
            ERR_FATAL("Can't send RABM cmd - out of memory!", 0, 0, 0);
          }
          rabm_ptr->header.cmd_id = RABMAS_RAB_RELEASE_IND;
          rabm_ptr->header.message_set = MS_RABM_RRC;
          rabm_ptr->cmd.rrc_release_ind.rabid = 
          (rabid_T)(rab_result.rab_id);
#ifdef FEATURE_DUAL_SIM
          rabm_ptr->cmd.rrc_release_ind.as_id = rrc_get_as_id();
#endif
          /* Put the command on RABM queue */
          WRRC_MSG0_HIGH("Sending RABMAS_RAB_RELEASE_IND to RABM");
          rabm_put_cmd(rabm_ptr);
        }/* end if ps domain */
      }/* end if RB_AND_RAB_REMOVED */
    }/* end for */
#ifdef FEATURE_DUAL_SIM
    if(rab_removed == TRUE)
    {
      rrc_check_send_change_priority();
    }
#endif
  }/* end if rbr.rbs.need_to_release_rbs == TRUE */

  /* Since channel config confirm has been received with a success, hence update the value of 
  maximum Count C in the permanent variable from the temporary one */
  rrcsmc_update_count_c_released_rbs();


  /*Go through all the RABs that came through RBR OTA. Usually, these should come
  either for SCUDIF or Codec change for Voice. But UTRAN can give RABs in RBR without
  these scenarios as well, just for sync purpose. For each of the RAB, NAS needs to be notified*/
  for( count=0; count < rbr.rabs.num_rabs_in_msg; count++ )
  {
    if (rbr.rabs.rabs_in_msg[count].cn_domain == RRC_CS_DOMAIN_CN_ID )
    {
#ifdef FEATURE_SCUDIF_ENHANCEMENT
#error code not present
#else /*FEATURE_SCUDIF_ENHANCEMENT*/
      (void)rrcrbr_configure_rab_change(count);
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/
      amr_configured = TRUE;
    }
    else if ( rbr.rabs.rabs_in_msg[count].cn_domain == RRC_PS_DOMAIN_CN_ID)
    {
      if ((rabm_ptr = rabm_get_cmd_buf()) == NULL)
      {
        ERR_FATAL("Can't send RABM cmd - out of memory!", 0, 0, 0);
      }
      rabm_ptr->header.cmd_id = RABMAS_RAB_ESTABLISH_IND;
      rabm_ptr->header.message_set = MS_RABM_RRC;
      rabm_ptr->cmd.rrc_establish_ind.rabid = 
      (rabid_T)(rbr.rabs.rabs_in_msg[count].rab_id);

#ifdef FEATURE_DUAL_SIM
      rabm_ptr->cmd.rrc_establish_ind.as_id = rrc_get_as_id();
#endif

      /* Put the command on RABM queue */
      WRRC_MSG0_HIGH("Sending RABMAS_RAB_ESTABLISH_IND to RABM");
      rabm_put_cmd(rabm_ptr);
    }/* end if ps domain */   
  }


  /*Here we handle SCUDIF without any RAB Reconfig IE in RBR OTA*/
#ifdef FEATURE_SCUDIF_ENHANCEMENT
  #error code not present
#endif/*FEATURE_SCUDIF_ENHANCEMENT*/


  /*Here we handle the case of AMR Rate change without any RAB Reconfig IE in RBR OTA */
  if((cs_rab_released == FALSE) && (amr_configured == FALSE)
#ifdef FEATURE_SCUDIF_ENHANCEMENT
    #error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/
    )
  {

#ifdef FEATURE_VOC_AMR_WB

      WRRC_MSG1_HIGH("Configuring MVS with mode %d from RBR.",current_mode);
      rrcrb_update_amr_cctrch_id_for_reconfig(current_mode);

#else /*FEATURE_VOC_AMR_WB*/

    WRRC_MSG0_HIGH("Configuring NB AMR from RBR.");
    rrcrb_update_amr_cctrch_id_for_reconfig();

#endif /*FEATURE_VOC_AMR_WB*/

  }


  /* Check for CELL FACH before sending the rbr complete message */
  curr_state=rrc_get_state();
  if ((RRC_STATE_CELL_FACH == curr_state) && 
      ((rbr.state_indicated_by_msg == RRC_STATE_CELL_FACH) ||
       (rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
       (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH)))
  {
    if (rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) 
    {
      /* this is the case of CELL_FACH->CELL_PCH state transition.
      In current implementation we do initiate cell selection. Instead
      we treat current cell as strongest cell */
      if ((rbr.directed_cell_info)&& (!rrccsp_is_selected_cell_utran_directed()))
      {
        /* check whether current camped scr code is same as the one
        sent in the message. If not then initiate cell update procedure */

          /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
          rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBR,
                                              RRC_CELL_RESELECTION,
                                              FALSE);
        /* No need to send ul data registration to rlc. This will be taken care
        by cellupdate procedure */
        rrcrbr_process_successful_proc_completion(FALSE);
      }
      else
      {
        /* send drx info to L1. Since this is CELL_FACH->CELL_PCH state transition
        RB procedure is responsible for sending the DRX info to lower layer*/
        rrcrb_send_l1_drx_req (RRC_PROCEDURE_RBR);

        rrcrbr_process_successful_proc_completion (TRUE);
      }
      /* return from here */
      return;
    }
    else if (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH)
    {
      /* send drx info to L1 */
      rrcrb_send_l1_drx_req (RRC_PROCEDURE_RBR);
      /* check whether URA update is required */
      if ( rrccu_is_ura_update_reqd() == TRUE)
      {
        rrcrb_send_ura_update_initiate_req( RRC_PROCEDURE_RBR,
                                            RRC_URA_RESELECTION,
                                            FALSE);
      }
      /* no need to wait for ura update completion 
      declare that proc is successfully completed */
      rrcrbr_process_successful_proc_completion (TRUE);
      /* return from here */
      return;
    }
    if(rrcrbr_send_initiate_cu_flag
       || (rrccu_return_cu_substate() == RRCCU_WAIT_FOR_RB_PROC_COMPL_IND) 
       )
    {
      if (rrcrbr_delete_crnti)
      {
        /* Send a RNTI Update Request */
        rnti_update_info.crnti_action         = RRC_RNTI_DELETE;
        rnti_update_info.urnti_action         = RRC_RNTI_NOCHANGE;
        rnti_update_info.procedure            = RRC_PROCEDURE_RBR;
        rnti_update_info.rlc_re_establish_srb = FALSE;
        rnti_update_info.rlc_re_establish_rab = FALSE;
        rnti_update_info.cnf_reqd             = FALSE;
        rbr.new_crnti_valid=FALSE;

        rrcrb_send_rnti_update_req(rnti_update_info);
      }
      /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
      rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBR,
                                          RRC_CELL_RESELECTION,
                                          TRUE);
      /* Wait for CELL UPDATE to get a valid C_RNTI */
      rbr.status.failure_status = RRCRB_MSG_VALID;
      rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      rrcrbr_send_initiate_cu_flag = FALSE;
      rrcrbr_delete_crnti          = FALSE;
      return;
    }
    /*Following case will occur when RLC RESET happens on SRB2 during DCH->FACH Transition*/
    else if((rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG) &&
      (rrccu_get_cell_update_started_status_with_cause(&cu_cause) == RRCCU_STARTED) && 
      (cu_cause == rrc_CellUpdateCause_rlc_unrecoverableError) && (rrccu_get_rb_id_unrec_error() == 
      DCCH_AM_RADIO_BEARER_ID))
    {
      if(rbr.new_crnti_valid)
      {
        /* Send a RNTI Update Request */
        rnti_update_info.crnti_action         = RRC_RNTI_UPDATE;
        rnti_update_info.urnti_action         = RRC_RNTI_NOCHANGE;
        rnti_update_info.crnti                = rbr.new_crnti;
        rnti_update_info.procedure            = RRC_PROCEDURE_RBR;
        rnti_update_info.rlc_re_establish_srb = FALSE;
        rnti_update_info.rlc_re_establish_rab = FALSE;
        rnti_update_info.cnf_reqd             = FALSE;
  
        rrcrb_send_rnti_update_req(rnti_update_info);
        /* Send a Radio Bearer Setup Complete later */      
      }

      /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
      rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBR,
                                          RRC_UNRECOVERABLE_ERROR,
                                          TRUE);

      /* Wait for CELL UPDATE to get a valid C_RNTI */
      rbr.status.failure_status = RRCRB_MSG_VALID;
      rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;

      return;                                          
    }    
    /* Coming here implies the "rrcrbr_send_initiate_cu_flag" is FALSE.
     * Check if Valid CRNTI is stored in RBR (for DCH->FACH case).
     * If so, send a RNTI Update Req to LLC and NOT wait for
     * Cell Update Complete Confirm. Cell Update will NOT be 
     * initiated when there is a valid C-RNTI and rrcrbr_send_initiate_cu_flag
     * is FALSE.
     */
    else if(rbr.new_crnti_valid)
    {
      /* Send a RNTI Update Request */
      rnti_update_info.crnti_action         = RRC_RNTI_UPDATE;
      rnti_update_info.urnti_action         = RRC_RNTI_NOCHANGE;
      rnti_update_info.crnti                = rbr.new_crnti;
      rnti_update_info.procedure            = RRC_PROCEDURE_RBR;
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
      if (SUCCESS != rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBR))
      {
        WRRC_MSG0_ERROR("RBR failed to register with CU");
      }
      rbr.status.failure_status = RRCRB_MSG_VALID;
      rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      return;
    }    
  } /* if current state is CELL_FACH */
  else if (RRC_STATE_CELL_PCH == curr_state)
  {
    if(rrcrbr_send_initiate_cu_flag)
    {
      /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
      rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBR,
                                          RRC_CELL_RESELECTION,
                                          FALSE);
      rrcrbr_send_initiate_cu_flag = FALSE;
      /* No need to send ul data registration to rlc. This will be taken care
      by cellupdate procedure */
      rrcrbr_process_successful_proc_completion(FALSE);
#ifdef FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE
      #error code not present
#endif

      return;
    }
    else
    {
      /*Perform the final steps for successful completion of the
      procedure */
      rrcrbr_process_successful_proc_completion(TRUE);
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
        rrcrb_send_ura_update_initiate_req( RRC_PROCEDURE_RBR,
                                            RRC_URA_RESELECTION,
                                            FALSE);
      }
    }
    /* No need to send ul data registration to rlc. This will be taken care
    by cellupdate procedure */
    rrcrbr_process_successful_proc_completion(TRUE);
#ifdef FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE
      #error code not present
#endif

    return;
  }

  /*check whether this reconfiguration involves SRNS relocation */
  if (rbr.rbr_for_srns_relocation == TRUE)
  {
//  if (RRC_STATE_CELL_DCH == curr_state)
  {

    /* Re-establish SRB2 before sending the response message */
    rb_type  = RRC_RE_EST_RB_2;
    rrcrb_send_rlc_re_est_req_to_smc(RRC_PROCEDURE_RBR, rb_type, TRUE);

    rbr.curr_substate = RRCRB_WAIT_FOR_REESTABLISH_SRB2;
  }
  }
  else /*normal reconfiguration without SRNS relocation */
  {   
    /* Send the Radio Bearer Release Complete message */
    if ((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
        (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH))

    {
      send_rbr_complete_message(TRUE);
    }
    else
    {
         /* Need to wait for L2 ack for all states because RRC needs to trigger include reconfig status indicator
    if RLC acknowledgement is not yet received.
    As per 8.3.1.3
    1> if the IE "Cell update cause" is set to "radio link failure" and the UE has submitted a reconfiguration response
    message to RLC and the UE has not received the RLC acknowledgement for the response message:
    2> include the IE "Reconfiguration Status Indicator" and set it to TRUE.*/
        rrc_update_reconfig_waiting_for_l2ack_dch(RRC_PROCEDURE_RBR);
        send_rbr_complete_message(TRUE);

      if((rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG)
        && (rrc_get_state() == RRC_STATE_CELL_DCH))
      {
        if(rrccu_check_rb_in_estab_rab())
        {
          /* IF CU is waiting to handle RLF/RLC unrecoverable error, then send complete
           * and delay for 50ms so to make sure Complete goes out to NW on old config
           */   
          WRRC_MSG1_HIGH("Delaying %d ms", RRCRB_PND_CFG_DELAY);
          (void) rex_clr_sigs(rex_self(), RRCRBR_TEMP_TIMER_SIG);
          (void) rex_set_timer(&rrcrbr_temp_timer, RRCRB_PND_CFG_DELAY); 
          rrcrbr_sig = rex_wait(RRCRBR_TEMP_TIMER_SIG);
          MSG_LOW("RRCRBR signal:%d", rrcrbr_sig, 0, 0);
          (void) rex_clr_sigs(rex_self(), RRCRBR_TEMP_TIMER_SIG);
          rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBR, RRC_UNRECOVERABLE_ERROR, FALSE);
          
        }
      }
      /*Following case handles the scenario when RF Failure or RLC unrecoverable error occured
        during DCH -> FACH or FACH -> FACH transiton.*/
      else if((rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG) &&
        (rrccu_get_cell_update_started_status_with_cause(&cu_cause) == RRCCU_STARTED))
      {
        if((cu_cause == rrc_CellUpdateCause_rlc_unrecoverableError) && (rrccu_get_rb_id_unrec_error() != DCCH_AM_RADIO_BEARER_ID))
        {
          /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
          rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBR,
                                              RRC_UNRECOVERABLE_ERROR,
                                              TRUE);
        }
        else if(cu_cause == rrc_CellUpdateCause_radiolinkFailure)
        {
          /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
          rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBR,
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
    if ((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
        (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH))

    {
      /* Change the substate to the next substate - this is
      where we wait for L2 ack from RLC for sending the RBR
      complete message. */
      rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
    }
    else
    {
      /* send the nas cn info if stored */
      if (rbr.cn_info_saved != FALSE)
      {
        /* Put the command on MM queue */
        rrc_send_rrc_message_to_mm(rbr.mm_cmd_ptr);
        /* clear the flag */
        rbr.cn_info_saved = FALSE;
      }

      /* Clear ORDERED_CONFIG - UE can now accept other ordered
      configurations */
      rrcllc_clear_ordered_config();  

      /* Go to the next sub-state in the state machine. That's
      the initial substate since we've sent the RBR complete
      message */
      rbr.curr_substate = RRCRB_INITIAL;

      /* At this point we don't need to remember any of the
      Radio Bearer information since it's stored in ESTABLISHED_RABS.
      Clear our internal variables */
      rrcrbr_clear_procedure();
    }
  }
}

/*===========================================================================

FUNCTION   RRCRBR_PROCESS_RLC_REESTABLISHMENT_CNF

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
static void rrcrbr_process_rlc_reestablishment_cnf
(
void
)
{
  rrcrb_rnti_update_info_type rnti_update_info; /* structure to be used to request RNTI update */

  /* send rnti update request if URNTI was present in the rebsetup message */
  if (rbr.new_urnti_valid == TRUE)
  {
    /* Send the RNTI update request to rrcllc to update with new-URNTI */
    /* Send a RNTI Update Request */
    rnti_update_info.crnti_action         = RRC_RNTI_NOCHANGE;
    rnti_update_info.urnti_action         = RRC_RNTI_UPDATE;
    rnti_update_info.urnti                = rbr.new_urnti;
    rnti_update_info.procedure            = RRC_PROCEDURE_RBR;
    rnti_update_info.rlc_re_establish_srb = FALSE;
    rnti_update_info.rlc_re_establish_rab = FALSE;
    rnti_update_info.cnf_reqd             = FALSE;

    rrcrb_send_rnti_update_req(rnti_update_info);
  }

  /* Send the reconfiguration complete message */

  send_rbr_complete_message(TRUE);


  /* Change the substate to the next substate - this is
     where we wait for L2 ack from RLC for sending the RB Reconfig
     complete message. */
  rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
  rrcllc_print_trans_chnl_rate();
  
}

/*===========================================================================

FUNCTION   RRCRBR_PROCESS_STATE_CHANGE_IND

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
static void rrcrbr_process_state_change_ind
(
  rrc_state_change_ind_type *ptr /* Pointer to the State Change Ind cmd */
)
{
  rrc_CellUpdateCause cu_cause;
  /* Check if we're going to disconnected state */
  if( ptr->new_state == RRC_STATE_DISCONNECTED )
  {
    /* Then change to initial substate */
    rbr.curr_substate = RRCRB_INITIAL;

    /* Then clear all local variables that may
    need to be cleared */
    rrcrbr_clear_procedure();
  }

  else if (ptr->new_state == RRC_STATE_CELL_FACH )
  {
    if(ptr->previous_state == RRC_STATE_CELL_DCH)
    {
      if (((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
          (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH)) &&
           (rbr.curr_substate == RRCRB_WAIT_FOR_L2_ACK))
      {
        WRRC_MSG1_HIGH("state change ind rbr_st:%d",rbr.curr_substate); 
        /* Then change to initial substate */
        rbr.curr_substate = RRCRB_INITIAL;

        /* Then clear all local variables that may
        need to be cleared */
        rrcrbr_clear_procedure();
      }
      else if (rbr.status.failure_status == RRCRB_MSG_VALID)
      {
        /* if cell update procedure is active with cause OOS, wait 
         * for the CU complete notification to send chan_config_req to
         * LLC 
         */
        if ((rrccu_get_cell_update_started_status_with_cause(&cu_cause)== RRCCU_STARTED) &&
            (cu_cause == rrc_CellUpdateCause_re_enteredServiceArea) &&
            (rrcllc_get_toc_usage() ==TOC_FOR_OOS))
        {
          rrcrbr_delete_crnti          = TRUE;
          WRRC_MSG0_HIGH("Complete rbr msg first.  Indicate compl to CU once done");
        }
        /* Call the function that creates and sends the  channel
           config information to be requested from LLC. */
        rrcrbr_send_channel_config_req();
        /* Change the substate since we're waiting for the
           confirmation from LLC for the channel configs */
        rbr.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;

        if((rrccsp_is_selected_cell_utran_directed()) &&
           (rbr.new_crnti_valid))
        {
          /* No need to initiate cell update as the C-RNTI is available
           * and UE is camped on the directed cell.
           */
          rrcrbr_send_initiate_cu_flag = FALSE;
        }
        else
        {
          rrcrbr_send_initiate_cu_flag = TRUE;
          rrcrbr_delete_crnti = TRUE;
        }
#ifdef FEATURE_WCDMA_HS_FACH
 /* If H-RNTI is not given in OTA then initiate cell update*/
        if(rrcenhstate_set_hrnti_status_for_transitions())
        {
          rrcrbr_send_initiate_cu_flag = TRUE;
          rrcrbr_delete_crnti          = TRUE;
        }
#endif

#ifdef FEATURE_WCDMA_HS_RACH
 /* Check if E-RNTI is not given in OTA then initiate cell update*/
        if(rrchsrach_check_ernti_status())
        {
          rrcrbr_send_initiate_cu_flag = TRUE;
          rrcrbr_delete_crnti          = TRUE;
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
        if (rbr.directed_cell_info)
        {
          if (rrccsp_is_selected_cell_utran_directed()) 
          {
            /* No need to initiate cell update UE is camped on 
            the directed cell. */
            WRRC_MSG0_HIGH("Directed cell match, No CU reqd");
            rrcrbr_send_initiate_cu_flag = FALSE;
          }
          else
          {
            rrcrbr_send_initiate_cu_flag = TRUE;
            rrcrbr_delete_crnti          = TRUE;
          }
        }
        else
        {
          /* we have to initiate cell update incase cpich info is not present
          in the message */
          rrcrbr_send_initiate_cu_flag = TRUE;
          rrcrbr_delete_crnti          = TRUE;
        }
      }
      else
      {
        /* URA_PCH case */
        rrcrbr_send_initiate_cu_flag = FALSE;
      }

      /* Call the function that creates and sends the  channel
      config information to be requested from LLC. */
      rrcrbr_send_channel_config_req();

      /* Change the substate since we're waiting for the
         confirmation from LLC for the channel configs */
      rbr.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
    }
    else if ((ptr->previous_state == RRC_STATE_CELL_FACH) &&
     ( rbr.initiate_cell_selection_trans_from_fach == TRUE))
    {
      rbr.initiate_cell_selection_trans_from_fach = FALSE;
     
       /* Call the function that creates and sends a channel
          config request to LLC. */
      if (rbr.directed_cell_info)
      {
        if(rrccsp_is_selected_cell_utran_directed())
        {
           /* No need to initiate cell update as the C-RNTI is available
            * and UE is camped on the directed cell.
            */
          rrcrbr_send_initiate_cu_flag = FALSE;
        }
        else
        {
          rrcrbr_send_initiate_cu_flag = TRUE;
          rrcrbr_delete_crnti          = TRUE;
        }
      }
      else
      {
        rrcrbr_send_initiate_cu_flag = TRUE;
        rrcrbr_delete_crnti          = TRUE;
      }
      rrcrbr_send_channel_config_req();
       /* Change the substate since we're waiting for the
          confirmation from LLC for the channel configs */
      rbr.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
    }
    else
    {
      MSG_HIGH("Recvd state change %d to %d RBR state %d, No action",
               ptr->previous_state, ptr->new_state, rbr.curr_substate);
    }

    /* IF CU is waiting for RB proc to complete, no need to initate CU through RB procedure. Reset flag. */
    if (rrccu_return_cu_substate() == RRCCU_WAIT_FOR_RB_PROC_COMPL_IND) 
    {
      rrcrbr_send_initiate_cu_flag = FALSE;
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

FUNCTION   RRCRBR_PROCESS_L2_ACK_IND

DESCRIPTION

  This function processes the received Layer 2 ACK indication.
  If the L2 ACK indicates that the RBR Complete message has
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
static void rrcrbr_process_l2_ack_ind
(
rrc_downlink_l2ack_ind_type *ptr /* Pointer to the L2 Ack Ind cmd */
)
{
  rrc_state_e_type current_state;       /* current RRC State  */
  rrc_rlc_re_est_for_srns_rb_e_type rb_type; /*rb type tobe reestablished */
  rrc_CellUpdateCause       cu_cause;
  uint8                    rejected_tr_id = RRCRB_INVALID_TRANSACTION_ID;
  WRRC_MSG1_HIGH("RRCRBR:L2 ack %d <0:Success 1:Failure>for RBR Complete msg received", ptr->status);
  if ( ptr->status == SUCCESS )
  {
    current_state = rrc_get_state(); 

    /* Back up accepted transaction ID */
    rejected_tr_id = rbr.tr_ids.accepted_transaction;
    
    /* clear transaction id*/
    rbr.tr_ids.accepted_transaction = RRCRB_INVALID_TRANSACTION_ID;
      
    if ((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
        (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH))
    {
      if(current_state==RRC_STATE_CELL_DCH) 
      {
       
        /*L2 Ack for the RBR Complete message is received on the old configuration
        Now initiate a cell selection */
        rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBR,
                                                RRC_TRANSITION_FROM_DCH,
                                                rbr.state_indicated_by_msg);
        /*wait for the state_change indication after the cell selection
        is successfully completed */
        rbr.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

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
          rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBR,
                                              RRC_REENTERED_SERVICE_AREA,
                                              TRUE);
          return;
        }
        if (RRCCU_STARTED == rrccu_get_cell_update_started_status_with_cause(&cu_cause) &&
            (cu_cause == rrc_CellUpdateCause_re_enteredServiceArea || cu_cause == rrc_CellUpdateCause_rlc_unrecoverableError ||
             cu_cause == rrc_CellUpdateCause_radiolinkFailure))
        {
          /* This is a case where cell update is in progress while RBR transitions 
             UE to Cell_PCH or URA_PCH. Though L2 Ack is gotten, send a PCR failure 
             later when CU procedure gets Cell update confirm. If we were to act on 
             L2 Ack, RRC moves to Cell_PCH/URA_PCH where cell update confirm message
             is not handled (unless CU registers on RBR's behalf) & UE would stuck 
             in Cell_PCH forever [in the absence of reselections].
          */
          if(SUCCESS != rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBR))
          {
            WRRC_MSG0_ERROR("RBR procedure failed to register with CU");
          }
          WRRC_MSG1_HIGH("Registered for Cell Update procedure completion: cu_cause = %d", cu_cause);
          rbr.status.failure_status = RRCRB_PHY_CHAN_FAILURE;
          rbr.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;

          /* Restore the accepted transaction ID for sending failure later */
          rbr.tr_ids.rejected_transaction = rejected_tr_id;
        }
        else
        {
          if((rbr.initiate_cell_selection_trans_from_fach == TRUE )
            && (rrccsp_check_initiate_cell_selection_handling(NULL) == TRUE))
          {
           
            /*Initiate the cell selection*/
            rrcllcoc_update_toc_with_oc_from_fach();
             
            rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBR,
                                                    RRC_TRANSITION_FROM_FACH,
                                                    rbr.state_indicated_by_msg);
 
 
             
             
            rbr.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
          }
          else
          {

            rbr.initiate_cell_selection_trans_from_fach = FALSE;

            /* Call the function that creates and sends a channel
                config request to LLC. */
            rrcrbr_send_channel_config_req();
            /* Change the substate since we're waiting for the
                confirmation from LLC for the channel configs */
            rbr.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
          }
        }
      }
      else
      {
        WRRC_MSG2_ERROR("Invalid combination of rrcstate%d and rrcrbr substate%d",
               current_state, rbr.curr_substate);
      }

      /*return from here */
      return;
    }

    /* if SRNS relocation is involved with this message then re
    establish the RBs other than RB2 here */
    if (rbr.rbr_for_srns_relocation)
    {
      if (RRC_STATE_CELL_DCH == rrc_get_state())
      {
        /* Initiate the re-establishment of remaining RBs before 
        RBR procedure goes back to initial substate. 
        No need to wait for the confirmation */
        rb_type  = RRC_RE_EST_NON_RB_2_UM_AM;
        rrcrb_send_rlc_re_est_req_to_smc(RRC_PROCEDURE_RBR, rb_type, FALSE);
        /* send the nas cn info if stored */
        if (rbr.cn_info_saved != FALSE)
        {
          /* Put the command on MM queue */
          rrc_send_rrc_message_to_mm(rbr.mm_cmd_ptr);
          /* clear the flag */
          rbr.cn_info_saved = FALSE;
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
      then NACK coming from RLC shall be ingnored as RBR failure with cause
      CU complete should be sent to NW */
    if (RRC_STATE_CELL_FACH == rrc_get_state() &&
        RRCCU_STARTED == rrccu_get_cell_update_started_status_with_cause(&cu_cause) && 
        (cu_cause == rrc_CellUpdateCause_re_enteredServiceArea || cu_cause == rrc_CellUpdateCause_rlc_unrecoverableError ||
         cu_cause == rrc_CellUpdateCause_radiolinkFailure))
    {
      (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBR);
      WRRC_MSG1_HIGH("NACK being ingnored as RBR failure msg will be sent, Registered for Cell Update procedure completion: cu_cause = %d", cu_cause);
      return;
    }

    /* clear transaction id */
    rbr.tr_ids.accepted_transaction = RRCRB_INVALID_TRANSACTION_ID;

    if (rbr.rbr_for_srns_relocation)
    {
      /* l2ack failed. If reconfiguration involves SARNS relocation then
      go to idle. Otherwise just complete the procedure */
      WRRC_MSG2_ERROR("L2ack failed SRNSreloc:%d, next_st:%d",
          rbr.rbr_for_srns_relocation,
          rbr.state_indicated_by_msg);
      /* Going to Idle should always go through CSP  */
      rrc_transition_to_disconnected_state(RRC_PROCEDURE_RBR,RRC_TX_TO_DISCON_OTHER);
    }
  }

  /* Note OC is set at this point. No need to clear it
  if the next state is cell_pch or ura_pch and
  l2 ack failied because cu procedure will take care of it */
  if (((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH)) &&
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
      (void)rrcllc_reset_ordered_config(RRC_PROCEDURE_RBR,TRUE);
    }
  }
  else
  {
    /* Send CLOSE_SESS_IND to NAS for SRNS relocation case */    
    if (rbr.need_to_release_cn_domain == TRUE)
    {
      if(rbr.cn_to_release == RRC_CS_DOMAIN_CN_ID)
      {      
        (void)rrcscr_release_signalling_connection(RRC_CS_DOMAIN_CN_ID);
      }
      else
      {
        (void)rrcscr_release_signalling_connection(RRC_PS_DOMAIN_CN_ID);      
      }
      rbr.need_to_release_cn_domain = FALSE;
    }
    /* Clear ORDERED_CONFIG - UE can now accept other ordered
    configurations */
    rrcllc_clear_ordered_config();  
  }

  if(rbr.rbr_for_srns_relocation == TRUE)
  {
    rrcdormancy_update_dormancy_for_srns_hho_cco(RRC_PROCEDURE_RBR);
  }

  /* Go to the next sub-state in the state machine. That's
  the initial substate since we've sent the RBR complete
  message */
  rbr.curr_substate = RRCRB_INITIAL;

  /* At this point we don't need to remember any of the
  Radio Bearer information since it's stored in ESTABLISHED_RABS.
  Clear our internal variables */
  rrcrbr_clear_procedure();
}


/*===========================================================================

FUNCTION   RRCRBR_INITIAL_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBR procedure is in the RRCRB_INITIAL substate. This function
  looks at the received event and does the appropriate processing
  of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbr_initial_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the received command's id */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {

    case RRC_DOWNLINK_SDU_IND:
      
      /* A Downlink SDU has been received from RLC. This
      SDU should contain a Radio Bearer Release Message. Otherwise
      we would not have received this command. Copy the 
      received message pointer. */
      /* Process the RB Release message */
      rrcrbr_process_rbrelease_message( cmd_ptr );
      break;

    default:

      /* No other events except Downlink SDU is expected in this
      substate */
      MSG_MED("Ignoring RBR event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate, 0);
      break;
  }
}

/*===========================================================================

FUNCTION   rrcrbr_other_wfcucnf_substate_event_handler

DESCRIPTION

  This function handles the events received by this module when
  the RBR procedure is in the RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate
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
static void rrcrbr_other_wfcucnf_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_STATE_CHANGE_IND:
      /* Indication that RRC's state has changed */
      rrcrbr_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;
    
    default:    
      /* No other events expected in this
      substate */
      MSG_MED("Ignoring RBR event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate, 0);
      break;
  }
}

/*===========================================================================

FUNCTION   RRCRBR_DCH_WFSRB2REESTABLISH_SUBSTATE_EVENT_HANDLER

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
static void rrcrbr_dch_wfsrb2reestablish_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  
  case RRC_RE_EST_RLC_FOR_SRNS_CNF:

    /* Confirmation from SMC of RLC re-establishment confirm */
    rrcrbr_process_rlc_reestablishment_cnf( );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbr_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Reconfig Message. Otherwise
    we would not have received this command. Copy the 
    received message pointer. */
    /* Process the RB Reconfig message */
    rrcrbr_process_rbrelease_message( cmd_ptr );

    break;

  default:

    /* No other events except Downlink SDU is expected in this
    substate */
    MSG_MED("Ignoring RBR event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate, 0);
    break;
  }
}

/*===========================================================================

FUNCTION   rrcrbr_other_wfcscnf_substate_event_handler

DESCRIPTION

  This function handles the events received by this module when
  the RBR procedure is in the RRCRB_WAIT_FOR_CELL_SELECTION_CNF substate
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
static void rrcrbr_other_wfcscnf_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_STATE_CHANGE_IND:
      /* Indication that RRC's state has changed */
      rrcrbr_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;
    
    default:    
      /* No other events expected in this
      substate */
      WRRC_MSG2_HIGH("Ignoring RBR event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate);
      break;
  }
}


/*===========================================================================

FUNCTION   RRCRBR_DCJ_WFCUCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the rbr procedure is in the RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate
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

static void rrcrbr_dch_wfcucnf_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  case RRC_CELL_UPDATE_COMPLETE_CNF:
    if (rbr.status.failure_status == RRCRB_SIMULTANEOUS_RECONFIG)
    {
      /* Send the RB Release Failure Message */
      send_rbr_failure_message( rbr.status.failure_status,
                                rbr.status.prot_err_cause
                              );
      /* Then clear all local variables that may
      need to be cleared */
      rrcrbr_clear_procedure();

      /* Make sure we're back in the initial sub-state */
      rbr.curr_substate = RRCRB_INITIAL;
    }
    else if (rbr.status.failure_status == RRCRB_MSG_VALID)
    {
      /* Send the Radio Bearer Release Complete message */
      if ((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
          (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH))

      {
        send_rbr_complete_message(TRUE);
      }
      else
      {
        send_rbr_complete_message(FALSE);
      }

      rrcllc_print_trans_chnl_rate();
      /*if next state is Cell_PCH or URA_PCH then we have to 
      treat the procedure as successful only after getting L2ack
      for the response message */
      if ((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
          (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH))

      {
        /* Change the substate to the next substate - this is
        where we wait for L2 ack from RLC for sending the RBR
        complete message. */
        rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
      }
      else
      {
        /* Clear ORDERED_CONFIG - UE can now accept other ordered
        configurations */
        rrcllc_clear_ordered_config();  

        /* send the nas cn info if stored */
        if (rbr.cn_info_saved != FALSE)
        {
          /* Put the command on MM queue */
          rrc_send_rrc_message_to_mm(rbr.mm_cmd_ptr);
          /* clear the flag */
          rbr.cn_info_saved = FALSE;
        }

        /* Go to the next sub-state in the state machine. That's
        the initial substate since we've sent the RBR complete
        message */
        rbr.curr_substate = RRCRB_INITIAL;

        /* At this point we don't need to remember any of the
        Radio Bearer information since it's stored in ESTABLISHED_RABS.
        Clear our internal variables */
        rrcrbr_clear_procedure();
      }

    }
    /*  In the case of Phy_Chan_Failure, RB setup failure must be send
    and oc should be reset. */
    else if(rbr.status.failure_status == RRCRB_PHY_CHAN_FAILURE)
    {

      /* Send the RB Setup Failure Message */
      send_rbr_failure_message( rbr.status.failure_status,
                                rbr.status.prot_err_cause
                              );
      /* Then clear all local variables that may
      need to be cleared */
      rrcrbr_clear_procedure();
      /* Make sure we're back in the initial sub-state */
      rbr.curr_substate = RRCRB_INITIAL;
    }
    else
    {
      WRRC_MSG1_ERROR("Failure status %d not handled",rbr.status.failure_status);
    }

    break;

  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcrbr_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbr_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Setup Message. Otherwise
    we would not have received this command. Copy the 
    received message pointer. */
    /* Process the RB Release message */
    rrcrbr_process_rbrelease_message( cmd_ptr );
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring rbr event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate, 0);
    break;
  }

}



/*===========================================================================

FUNCTION   RRCrbr_FACH_WFCUCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the rbr procedure is in the RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate
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

static void rrcrbr_fach_wfcucnf_event_handler
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
    if (rbr.status.failure_status == RRCRB_SIMULTANEOUS_RECONFIG)
    {
      /* Send the RB Release Failure Message */
      send_rbr_failure_message( rbr.status.failure_status,
                                rbr.status.prot_err_cause
                              );
      /* Then clear all local variables that may
      need to be cleared */
      rrcrbr_clear_procedure();

      /* Make sure we're back in the initial sub-state */
      rbr.curr_substate = RRCRB_INITIAL;
    }
    else if (rbr.status.failure_status == RRCRB_MSG_VALID)
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
          rrcrbr_send_channel_config_req();
  
          /* Change the substate since we're waiting for the
             confirmation from LLC for the channel configs */
          rbr.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;	
          return;					
        }

      /* Send the Radio Bearer Release Complete message */
      if ((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
          (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH))
      {
        send_rbr_complete_message(TRUE);
      }
      else
      {
        send_rbr_complete_message(FALSE);
      }
      rrcllc_print_trans_chnl_rate();
      /* if next state is Cell_PCH or URA_PCH then we have to 
         treat the procedure as successful only after getting L2ack
         for the response message */
      if ((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
          (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH))
        
      {
        /* Change the substate to the next substate - this is
           where we wait for L2 ack from RLC for sending the RBR
           complete message. */
        rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
      }
      else
      {    
        /* Clear ORDERED_CONFIG - UE can now accept other ordered
           configurations */
        rrcllc_clear_ordered_config();  

        /* send the nas cn info if stored */
        if (rbr.cn_info_saved != FALSE)
        {
          /* Put the command on MM queue */
          rrc_send_rrc_message_to_mm(rbr.mm_cmd_ptr);
          /* clear the flag */
          rbr.cn_info_saved = FALSE;
        }

        /* Go to the next sub-state in the state machine. That's
           the initial substate since we've sent the RBR complete
           message */
        rbr.curr_substate = RRCRB_INITIAL;

        /* At this point we don't need to remember any of the
           Radio Bearer information since it's stored in ESTABLISHED_RABS.
           Clear our internal variables */
        rrcrbr_clear_procedure();
      }
    }
    /*  In the case of Phy_Chan_Failure, RB setup failure must be send
    and oc should be reset. */
    else if(rbr.status.failure_status == RRCRB_PHY_CHAN_FAILURE)
    {

      /* Send the RB Setup Failure Message */
      send_rbr_failure_message( rbr.status.failure_status,
                                rbr.status.prot_err_cause
                              );
      /* Then clear all local variables that may
      need to be cleared */
      rrcrbr_clear_procedure();
      /* Make sure we're back in the initial sub-state */
      rbr.curr_substate = RRCRB_INITIAL;
    }
    else
    {
      WRRC_MSG1_ERROR("Failure status %d not handled",rbr.status.failure_status);
    }

    break;

  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcrbr_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbr_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Setup Message. Otherwise
    we would not have received this command. Copy the 
    received message pointer. */
    /* Process the RB Release message */
    rrcrbr_process_rbrelease_message( cmd_ptr );
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring rbr event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate, 0);
    break;
  }

}

/*===========================================================================

FUNCTION   RRCRBR_DCH_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBR procedure is in the RRCRB_WAIT_FOR_CHAN_CFG_CNF substate
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
static void rrcrbr_dch_wfchcnf_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  
  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcrbr_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbr_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Release Message. Otherwise
    we would not have received this command. Copy the 
    received message pointer. */
    /* Process the RB Release message */
    rrcrbr_process_rbrelease_message( cmd_ptr );
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBR event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate, 0);
    break;
  }
}


/*===========================================================================

FUNCTION   RRCRBR_FACH_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBR procedure is in the RRCRB_WAIT_FOR_CHAN_CFG_CNF substate
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
static void rrcrbr_fach_wfchcnf_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  
  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcrbr_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbr_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Release Message. Otherwise
    we would not have received this command. Copy the 
    received message pointer. */
    /* Process the RB Release message */
    rrcrbr_process_rbrelease_message( cmd_ptr );
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBR event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate, 0);
    break;
  }
}



/*===========================================================================

FUNCTION   RRCRBR_OTHER_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBR procedure is in the RRCRB_WAIT_FOR_CHAN_CFG_CNF substate
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
static void rrcrbr_other_wfchcnf_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the received command's id */
  WRRC_MSG1_HIGH("Received command:%d",cmd_ptr->cmd_hdr.cmd_id);
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  
  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    /*Ignoring chan config cnf since UE not in DCH or FACH */
    /* In this state this procedure should no longer be active. */

    /* Then change to initial substate */
    rbr.curr_substate = RRCRB_INITIAL;

    /* Then clear all local variables that may
    need to be cleared */
    rrcrbr_clear_procedure();
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbr_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. */
    /* Ignoring downlink sdu since UE not in DCH or FACH */
    /* In this state this procedure should no longer be active. */

    /* Then change to initial substate */
    rbr.curr_substate = RRCRB_INITIAL;

    /* Then clear all local variables that may
    need to be cleared */
    rrcrbr_clear_procedure();
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBR event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate, 0);
    break;
  }
}



/*===========================================================================

FUNCTION   RRCRBR_DCH_WFL2ACK_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBR procedure is in the RRCRB_WAIT_FOR_L2_ACK substate
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
static void rrcrbr_dch_wfl2ack_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  
  case RRC_DOWNLINK_L2ACK_IND:

    /* Confirmation from LLC for the channel configurations */
    rrcrbr_process_l2_ack_ind( &(cmd_ptr->cmd.downlink_l2ack_ind) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbr_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Release Message. Otherwise
    we would not have received this command. Copy the 
    received message pointer. */
    /* Process the RB Release message */
    rrcrbr_process_rbrelease_message( cmd_ptr );
    break;


  case RRC_CELL_UPDATE_COMPLETE_CNF:
    rbr.status.failure_status = RRCRB_CELL_UPDATE_OCCURED;
    /* Send the RBR Reconfig Failure Message */
    send_rbr_failure_message(rbr.status.failure_status,
                              rbr.status.prot_err_cause
                             );
    /* Then clear all local variables that may
    need to be cleared */
    rrcrbr_clear_procedure();

    /* Make sure we're back in the initial sub-state */
    rbr.curr_substate = RRCRB_INITIAL;
    break;

  default:

    /* No other events except Downlink SDU is expected in this
    substate */
    MSG_MED("Ignoring RBR event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate, 0);
    break;
  }
}


/*===========================================================================

FUNCTION   RRCRBR_FACH_WFL2ACK_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBR procedure is in the RRCRB_WAIT_FOR_L2_ACK substate
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
static void rrcrbr_fach_wfl2ack_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  
  case RRC_DOWNLINK_L2ACK_IND:

    /* Confirmation from LLC for the channel configurations */
    rrcrbr_process_l2_ack_ind( &(cmd_ptr->cmd.downlink_l2ack_ind) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbr_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Release Message. Otherwise
    we would not have received this command. Copy the 
    received message pointer. */
    /* Process the RB Release message */
    rrcrbr_process_rbrelease_message( cmd_ptr );
    break;

  case RRC_CELL_UPDATE_COMPLETE_CNF:
    rbr.status.failure_status = RRCRB_CELL_UPDATE_OCCURED;
    /* Send the RBR Reconfig Failure Message */
    send_rbr_failure_message(rbr.status.failure_status,
                              rbr.status.prot_err_cause
                             );
    /* Then clear all local variables that may
    need to be cleared */
    rrcrbr_clear_procedure();

    /* Make sure we're back in the initial sub-state */
    rbr.curr_substate = RRCRB_INITIAL;
    break;

  default:

    /* No other events except Downlink SDU is expected in this
    substate */
    MSG_MED("Ignoring RBR event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate, 0);
    break;
  }
}



/*===========================================================================

FUNCTION   RRCRBR_OTHER_WFL2ACK_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBR procedure is in the RRCRB_WAIT_FOR_L2_ACK substate
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
static void rrcrbr_other_wfl2ack_substate_event_handler
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
    WRRC_MSG1_HIGH("Ignoring L2 ack/downlink SDU since UE not in DCH or FACH cmd_id:%0x", cmd_ptr->cmd_hdr.cmd_id);
    /* In this state this procedure should no longer be active. */

    /* Then change to initial substate */
    rbr.curr_substate = RRCRB_INITIAL;

    /* Then clear all local variables that may
    need to be cleared */
    rrcrbr_clear_procedure();
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbr_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  default:

    /* No other events except Downlink SDU is expected in this
    substate */
    MSG_MED("Ignoring RBR event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate, 0);
    break;
  }
}




/*===========================================================================

FUNCTION   RRCrbr_FACH_WFCSCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the rbr procedure is in the RRCRB_WAIT_FOR_CELL_SELECTION_CNF substate
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

static void rrcrbr_fach_wfcscnf_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
   /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

    case RRC_STATE_CHANGE_IND:
     /* Indication that RRC's state has changed */
      rrcrbr_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;
    case RRC_NEW_CELL_IND:
      rrcrbr_process_new_cell_ind( &(cmd_ptr->cmd.new_cell_ind) );
      break;

    case RRC_CELL_UPDATE_COMPLETE_CNF:
      if(rbr.state_indicated_by_msg == RRC_STATE_CELL_FACH)
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
          rrcrbr_send_channel_config_req();
          /* Also reset the rrcrbr_send_initiate_cu_flag to false as cell update
           * will not be needed since CU and CUCNF for oos took care 
           * of that
           */
          rrcrbr_send_initiate_cu_flag = FALSE;
          /* Change the substate since we're waiting for the
          confirmation from LLC for the channel configs */
          rbr.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
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
      WRRC_MSG2_HIGH("Ignoring RBR event %x in substate %d",
         cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate);
      break;
   }


}


/*===========================================================================

FUNCTION   RRCRBR_FACH_WFCRNTIUPDATECNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBR procedure is in the RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF substate
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

static void rrcrbr_fach_wfcrntiupdatecnf_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_STATE_CHANGE_IND:
   /* Indication that RRC's state has changed */
    rrcrbr_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
   
    break;


  case RRC_RNTI_UPDATE_CNF:
    if (cmd_ptr->cmd.rnti_update_cnf.status == SUCCESS) 
    {
      rrcrbr_process_rnti_update_cnf();
    }
    else
    {
      WRRC_MSG0_HIGH("RNTI update failed.  Send RBR failure");
      rbr.status.failure_status = RRCRB_MSG_VALID;
      
      send_rbr_failure_message(rbr.status.failure_status,
                               rbr.status.prot_err_cause);
      
      /* Then clear all local variables that may
         need to be cleared */
      rrcrbr_clear_procedure();

      /* Make sure we're back in the initial sub-state */
      rbr.curr_substate = RRCRB_INITIAL;
    }
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBR event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate, 0);
    break;
  }

}

/*===========================================================================

FUNCTION   RRCRBR_PCH_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBR procedure is in the RRCRB_WAIT_FOR_CHAN_CFG_CNF substate
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
static void rrcrbr_pch_wfchcnf_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{


  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  
  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcrbr_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbr_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:
  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBR event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate, 0);
    break;
  }
}

/*===========================================================================

FUNCTION   RRCRBR_PCH_WFCUCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBR procedure is in the RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate
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
static void rrcrbr_pch_wfcucnf_event_handler
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
    if(rbr.status.failure_status != RRCRB_MSG_VALID)
    {
      /* Send the PC Reconfig Failure Message */
      send_rbr_failure_message(rbr.status.failure_status,
                                rbr.status.prot_err_cause
                              );
      /* Then clear all local variables that may
      need to be cleared */
      rrcrbr_clear_procedure();

      /* Make sure we're back in the initial sub-state */
      rbr.curr_substate = RRCRB_INITIAL;
    }
    else 
    {
      /* Send the TC Reconfig Complete message */
      if ((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
          (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH))

      {
        send_rbr_complete_message(TRUE);
      }
      else
      {
        send_rbr_complete_message(FALSE);
      }
      /* prints rate for each transport channel */
      rrcllc_print_trans_chnl_rate();
      /*if next state is Cell_PCH or URA_PCH then we have to 
      treat the procedure as successful only after getting L2ack
      for the response message */
      if ((rbr.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
          (rbr.state_indicated_by_msg == RRC_STATE_URA_PCH))

      {
        /* Change the substate to the next substate - this is
        where we wait for L2 ack from RLC for sending the RB RC
        complete message. */
        rbr.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
      }
      else
      {
        /* clear the ordered config here
        treat the procedure as successful. */
        rrcllc_clear_ordered_config();  

        /* Go to the next sub-state in the state machine. That's
        the initial substate since we've sent the RB RC complete
        message */
        rbr.curr_substate = RRCRB_INITIAL;

        /* At this point we don't need to remember any of the
        Radio Bearer information since it's stored in ESTABLISHED_RABS.
        Clear our internal variables */
        rrcrbr_clear_procedure();
      }
    }
    break;

  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcrbr_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:
    /* Indication that RRC's state has changed */
    rrcrbr_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

 

    case RRC_DOWNLINK_SDU_IND:
      WRRC_MSG2_HIGH("Unexpected DOWNLINK_SDU_IND in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate);
    break;

  default:

    /* No other events expected in this
    substate */
    WRRC_MSG2_HIGH("Ignoring RBR event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate);
    break;
  }

}

/*===========================================================================

FUNCTION   RRCRBR_PCH_WF_CSCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBR procedure is in the RRCRB_WAIT_FOR_CELL_SELECTION_CNF substate
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

static void rrcrbr_pch_wfcscnf_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_STATE_CHANGE_IND:
   /* Indication that RRC's state has changed */
    rrcrbr_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
   
    break;


  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBR event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate, 0);
    break;
  }

}


/*===========================================================================

FUNCTION   RRCRBR_CELL_DCH_EVENT_HANDLER

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
static void rrcrbr_cell_dch_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  if ((cmd_ptr->cmd_hdr.cmd_id == RRC_DOWNLINK_L2ACK_IND) &&
      (rbr.curr_substate != RRCRB_WAIT_FOR_L2_ACK))
  {
    /* We could get L2ack for a previously sent message while we are already
    processing the next message. So l2ack could be received any any of the following
    substates. If RBR substate does not indicate that we are not waiting for l2ack
    then just print the status */
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCRBR:L2ack status %d <0-Success 1-Failure> for MUI:%d, Lc_Id:%d, rbe_subst:%d",
               cmd_ptr->cmd.downlink_l2ack_ind.status,
               cmd_ptr->cmd.downlink_l2ack_ind.mui, 
               cmd_ptr->cmd.downlink_l2ack_ind.lc_id, rbr.curr_substate);

    /* return from here so that procedure can continue with existing process */
    return;
  }
  /* Switch on the current substate */
  switch ( rbr.curr_substate )
  {
  case RRCRB_INITIAL:

    /* Call the event handler for RRCRB_INITIAL substate */
    rrcrbr_initial_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_CHAN_CFG_CNF:

    /* Call the event handler for RRCRB_WAIT_FOR_CHAN_CFG_CNF substate */
    rrcrbr_dch_wfchcnf_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_L2_ACK:

    /* Call the event handler for RRCRB_WAIT_FOR_L2_ACK substate */
    rrcrbr_dch_wfl2ack_substate_event_handler( cmd_ptr );
    break;

    case RRCRB_WAIT_FOR_REESTABLISH_SRB2:
      rrcrbr_dch_wfsrb2reestablish_substate_event_handler(cmd_ptr);
      break;

    case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
      /* Call the event handler for RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate */
      rrcrbr_dch_wfcucnf_event_handler( cmd_ptr);
      break;

  default:

    /* No other substates processed for this RRC state    */
    MSG_MED("Command %x not processed in CELL_DCH for substate %d", 
            cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate, 0);
    break;

  }
}



/*===========================================================================

FUNCTION   RRCRBR_CELL_FACH_EVENT_HANDLER

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
static void rrcrbr_cell_fach_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  if ((cmd_ptr->cmd_hdr.cmd_id == RRC_DOWNLINK_L2ACK_IND) &&
      (rbr.curr_substate != RRCRB_WAIT_FOR_L2_ACK))
  {
    /* We could get L2ack for a previously sent message while we are already
    processing the next message. So l2ack could be received any any of the following
    substates. If RBR substate does not indicate that we are not waiting for l2ack
    then just print the status */
   MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCRBR:L2ack status %d <0-Success 1-Failure> for MUI:%d, Lc_Id:%d, rbe_subst:%d",
   cmd_ptr->cmd.downlink_l2ack_ind.status,
               cmd_ptr->cmd.downlink_l2ack_ind.mui, 
               cmd_ptr->cmd.downlink_l2ack_ind.lc_id, rbr.curr_substate);

    /* return from here so that procedure can continue with existing process */
    return;
  }

  /* Switch on the current substate */
  switch ( rbr.curr_substate )
  {
  case RRCRB_INITIAL:
    /* Call the event handler for RRCRB_INITIAL substate */
    /* NOTE: Here we need to check if a cell-update procedure
    has been initialized or not. For now, just call the event handler */
    rrcrbr_initial_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
    /* Call the event handler for RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate */
    rrcrbr_fach_wfcucnf_event_handler( cmd_ptr);
    break;

  case RRCRB_WAIT_FOR_CHAN_CFG_CNF:
    /* Call the event handler for RRCRB_WAIT_FOR_CHAN_CFG_CNF substate */
    rrcrbr_fach_wfchcnf_substate_event_handler( cmd_ptr );
    break;

  case  RRCRB_WAIT_FOR_CELL_SELECTION_CNF:
    /* Call the event handler for RRCRB_WAIT_FOR_CHAN_CFG_CNF substate */
    rrcrbr_fach_wfcscnf_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF:
    /* Call the event handler for RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF substate */
    rrcrbr_fach_wfcrntiupdatecnf_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_L2_ACK:
    /* Call the event handler for RRCRB_WAIT_FOR_L2_ACK substate */
    rrcrbr_fach_wfl2ack_substate_event_handler( cmd_ptr );
    break;

  default:
    /* No other substates processed for this RRC state    */
    MSG_MED("Command %x not processed in CELL_FACH for substate %d", 
            cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate, 0);
    break;
  }
}

/*===========================================================================

FUNCTION   RRCRBR_PCH_EVENT_HANDLER

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
static void rrcrbr_pch_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the current substate */
  switch ( rbr.curr_substate )
  {
  case RRCRB_INITIAL:

    /* Call the event handler for RRCRB_INITIAL substate */
    rrcrbr_initial_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
    /* Call the event handler for RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate */
    rrcrbr_pch_wfcucnf_event_handler( cmd_ptr);
    break;

  case RRCRB_WAIT_FOR_CELL_SELECTION_CNF:

    rrcrbr_pch_wfcscnf_event_handler( cmd_ptr);
    break;

  case RRCRB_WAIT_FOR_CHAN_CFG_CNF:

    /* Call the event handler for RRCRB_WAIT_FOR_CHAN_CFG_CNF substate */
    rrcrbr_pch_wfchcnf_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_L2_ACK:

    /* The only way we could be in pch and rb state is "wait_for_l2_ack is if
       oos happened while waiting for l2 ack and cell update confirm takes UE to pch
       In this case, send failure msg of rbrc with cause cell update occured 
    */
    rbr.status.failure_status = RRCRB_CELL_UPDATE_OCCURED;

    send_rbr_failure_message(rbr.status.failure_status,
                              rbr.status.prot_err_cause);
    /* Then clear all local variables that may
       need to be cleared */
    rrcrbr_clear_procedure();

    /* Make sure we're back in the initial sub-state */
    rbr.curr_substate = RRCRB_INITIAL;
    break;

  default:

    /* No other substates processed for this RRC state    */
    MSG_MED("Command %x not processed in substate%d rrc_state%d", 
            cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate, rrc_get_state());
    break;

  }
}




/*===========================================================================

FUNCTION   RRCRBR_OTHER_STATE_EVENT_HANDLER

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
static void rrcrbr_other_state_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the current substate */
  switch ( rbr.curr_substate )
  {
  
    case RRCRB_WAIT_FOR_CHAN_CFG_CNF:
      /* Call the event handler for WAIT_FOR_CHAN_CFG_CNF substate */
      rrcrbr_other_wfchcnf_substate_event_handler( cmd_ptr );
      break;

    case RRCRB_WAIT_FOR_L2_ACK:
      /* Call the event handler for WAIT_FOR_L2_ACK substate */
      rrcrbr_other_wfl2ack_substate_event_handler( cmd_ptr );
      break;

    case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
      rrcrbr_other_wfcucnf_substate_event_handler( cmd_ptr);
      break;

    case  RRCRB_WAIT_FOR_CELL_SELECTION_CNF:
      /* Call the event handler for RRCRB_WAIT_FOR_CHAN_CFG_CNF substate */
      rrcrbr_other_wfcscnf_substate_event_handler( cmd_ptr );
      break;

    default:
      /* No other substates processed for this RRC state    */
      MSG_MED("Command %x not processed in IDLE or PCH states for substate %d", 
            cmd_ptr->cmd_hdr.cmd_id, rbr.curr_substate, 0);
      break;
  }
}


/*===========================================================================

FUNCTION RRCRBR_PROCEDURE_EVENT_HANDLER


DESCRIPTION

  This is the main function handling external events received by the
  Radio Bearer Release procedure. This function is called by the
  dispatcher whenever an event is received for this module. This function
  checks the current RRC state and calls the event handler for
  that state. 
  
  Note that the Radio Bearer Release procedure can be initiated
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
void rrcrbr_procedure_event_handler
(
rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  /* Get the current RRC State */
  rrc_state_e_type rrc_state = rrc_get_state();

  WRRC_MSG2_HIGH_OPT("function rrcrbr_procedure_event_handler is called in rrc_state = rrc_state_e_type_value%d and rbr.curr_substate = rrcrb_substate_e_type_value%d", rrc_state, rbr.curr_substate);
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_RBR,rbr.curr_substate,cmd_ptr->cmd_hdr.cmd_id);

  switch (rrc_state)
  {
  
  case RRC_STATE_CELL_FACH:    
    /* If it's CELL_FACH state, call the event handler
    for CELL_FACH state */
    rrcrbr_cell_fach_event_handler(cmd_ptr);
    break;

  case RRC_STATE_CELL_DCH:    
    /* If it's CELL_DCH state, call the event handler
    for CELL_DCH state */
    rrcrbr_cell_dch_event_handler(cmd_ptr);
    break;

    case RRC_STATE_URA_PCH:
    case RRC_STATE_CELL_PCH:
      /* If it's CELL_DCH state, call the event handler
      for CELL_PCH state */
      rrcrbr_pch_event_handler(cmd_ptr);
      break;

  case RRC_STATE_CONNECTING:   
  case RRC_STATE_DISCONNECTED: 
    /* If it's any other state call the event handler
    for other states. The processing of events in
    other RRC states is identical for this procedure */
    rrcrbr_other_state_event_handler(cmd_ptr);
    break;

  default:  
    WRRC_MSG1_ERROR("Invalid RRC State: %d", rrc_state);
    break;
  }  
}/* end function rrcrbr_procedure_event_handler */



/*===========================================================================

FUNCTION  RRCRBR_INIT_PROCEDURE

DESCRIPTION

  This function initializes the RBR procedure.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcrbr_init_procedure( void )
{
  uint32 count;
  rrcrbr_send_initiate_cu_flag = FALSE;


  /* Initialize the transaction id's for this procedure.
  Use a number that can't be an actual received transaction
  id. */
  rbr.tr_ids.accepted_transaction = RRCRB_INVALID_TRANSACTION_ID;
  rbr.tr_ids.rejected_transaction = RRCRB_INVALID_TRANSACTION_ID;

  rbr.rbr_for_srns_relocation = FALSE;
  rbr.new_urnti_valid = FALSE;

  rbr.hho_ciph_status = FALSE;

  rbr.trans_to_dch_ciph_config_needed = FALSE;
  /* Initialize the current substate */
  rbr.curr_substate = RRCRB_INITIAL;

  /* set cn_info_saved to false */
  rbr.cn_info_saved = FALSE;

  /* Initialize CRNTI valid to FALSE */
  rbr.new_crnti_valid    = FALSE;
  rbr.directed_cell_info = FALSE;

  /* Initialize the RABS per message structure.
  For Radio Bearer Relase, these structures are not
  expected to be used unless there are RABs to
  be reconfigured given by the message */
  for ( count=0; count < RRCRB_MAX_RAB_PER_MSG; count++ )
  {
    rbr.rabs.rabs_in_msg[count].rab_type = RRCRB_UNKNOWN_RAB;    
    rbr.rabs.rabs_in_msg[count].num_rbs_for_rab = 0;
    rbr.rabs.chan_config_needed[count] = FALSE;
#ifdef FEATURE_VOC_AMR_WB
    rbr.rabs.rabs_in_msg[count].nas_Synchronisation_Indicator = MVS_MODE_NONE;
#endif /*FEATURE_VOC_AMR_WB*/

  }
  rbr.rabs.num_rabs_in_msg = 0;
  rbr.status.failure_status = RRCRB_MSG_VALID;
  /* Indicate that there are no signalling connections
  to be released. */
  rbr.need_to_release_cn_domain = FALSE;

  /* Indicate that there are no RBs to be released/reconfigured/affected. This
  is the default value. */
  rbr.rbs_tobe_released.is_valid = FALSE;
  rbr.rbs_tobe_reconfigured.is_valid = FALSE;
  rbr.rbs_tobe_affected.is_valid = FALSE;

  rbr.high_mobility_ind = FALSE;
  /* Register for state change notification -
  We want to be notified when the state changes
  from CELL_DCH to CELL_FACH and from any state
  to disconnected state. */
  rrcscmgr_register_for_scn( RRC_PROCEDURE_RBR,  /* Procedure name */
                             RRC_STATE_CELL_DCH, /* From State     */
                             RRC_STATE_CELL_FACH /* To State       */
                           );

  rrcscmgr_register_for_scn( RRC_PROCEDURE_RBR,  /* Procedure name */
                             RRC_STATE_WILDCARD, /* From State     */
                             RRC_STATE_DISCONNECTED /* To State    */
                           );
  rrcscmgr_register_for_scn( RRC_PROCEDURE_RBR,  /* Procedure name */
                             RRC_STATE_CELL_DCH, /* From State     */
                             RRC_STATE_CELL_PCH /* To State       */
                           );
  rrcscmgr_register_for_scn( RRC_PROCEDURE_RBR,  /* Procedure name */
                             RRC_STATE_CELL_DCH, /* From State     */
                             RRC_STATE_URA_PCH /* To State       */
                           );

  rrcscmgr_register_for_scn( RRC_PROCEDURE_RBR,  /* Procedure name */
                             RRC_STATE_CELL_FACH, /* From State     */
                             RRC_STATE_CELL_PCH /* To State       */
                           );

  rrcscmgr_register_for_scn( RRC_PROCEDURE_RBR,  /* Procedure name */
                             RRC_STATE_CELL_FACH, /* From State     */
                             RRC_STATE_URA_PCH /* To State       */
                           );
  rbr.initiate_cell_selection_trans_from_fach = FALSE;     

  /* Define a timer used to delay 5 seconds before
  sending the RB Release Complete message */
  rex_def_timer(&rrcrbr_temp_timer, rex_self(), RRCRBR_TEMP_TIMER_SIG);
}

#ifdef FEATURE_QSH_EVENT_METRIC
/*===========================================================================

FUNCTION  rrcrbr_get_num_rbs_to_release

DESCRIPTION
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
uint8 rrcrbr_get_num_rbs_to_release(void)
{
  return rbr.rbs_tobe_released.num_rbs;
}
/*===========================================================================

FUNCTION  rrcrbr_get_num_rbs_to_recfg

DESCRIPTION
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

uint8 rrcrbr_get_num_rbs_to_recfg(void)
{
return rbr.rbs_tobe_reconfigured.num_rbs;
}

/*===========================================================================

FUNCTION  rrcrbr_get_srns_relocation

DESCRIPTION
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/

uint8 rrcrbr_get_srns_relocation(void)
{
return rbr.rbr_for_srns_relocation;
}
#endif
/*====================================================================
FUNCTION: rrcrbr_is_rb_being_released

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

boolean rrcrbr_is_rb_being_released(uint8 rb_id)
{
  rrcllc_oc_set_status_e_type	  oc_status;
  uint8 count=0;	 /* Used if there is more than 1 RAB in the message */
  oc_status = rrcllc_get_ordered_config_status();
  
  if ((oc_status != OC_NOT_SET) && (ordered_config.set_by_proc == RRC_PROCEDURE_RBR))
  {
    /* Check whether this RAB exists in established_rab db */
    if (TRUE == rbr.rbs_tobe_released.is_valid)
    {/* This RAB is valid. Look at each RB within the RAB */
      for( count=0; count<rbr.rbs_tobe_released.num_rbs; count++ )
      {
        if (rbr.rbs_tobe_released.rb_id[count] == rb_id)
        {
           return TRUE;
        }
      }
    }
  }
  return FALSE;	
}
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
/*===========================================================================

FUNCTION rrcrbr_set_failure_cause

DESCRIPTION

DEPENDENCIES


RETURN VALUE
  Returns 

SIDE EFFECTS

  None

===========================================================================*/
void rrcrbr_set_failure_cause(rrcrb_msg_status_e_type cause)
{
  rbr.status.failure_status = cause;
}

#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
