/*===========================================================================
  R R C   R A D I O   B E A R E R   R E C O N F I G   M O D U L E 

DESCRIPTION

  This module performs the Radio Bearer Reconfiguration Procedure. This
  module processes the Radio Bearer Reconfig Message and appropriately
  reconfigures the radio bearers given by the message. 
  
  The Radio Bearer Reconfig Message may reconfig both signalling radio bearers
  and radio bearers that are mapped to radio access bearers. 
  It may set also re-configure previously established radio bearers.
  The procedure may also initiate a hard-handover if the Radio Bearer
  Reconfig Message indicates a frequency change.
  
  
EXTERNALIZED FUNCTIONS

  rrcrbrc_procedure_event_handler   Event handler for this procedure
  rrcrbrc_init_procedure            Power-up initialization of the procedure

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Procedure must be initialized at power-up by calling rrcrbrc_init_procedure. 
  
Copyright (c) 1999-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcrbreconfig.c#1 $   

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/16   sp      Made changes to reset CU registration info once reconfig procedure is successful
06/20/16   sg      Made changes to reduce F3s
10/19/15   sp      Made changes to trigger OOS if L2 ack for transition to PCH is received when inter freq resel is in progress
10/16/15   as      Added code to support LTA durting activation time
04/23/15   vi      Made changes to add DMCR IE only when DMCR is supported in SIB3
09/03/14   sp      Made changes to save procedure substates in crash debug info
08/08/14   ad      Made changes to trigger reconfiguration failure after cell update
                   procedure gets finished due to SIB5 change
08/04/14   sp      Made changes to fix compilation errors
07/23/14   sp      Made changes to remove F3s as part of FR19999
06/19/14   sg      Made changes to revert the cipher config db if cipher config type is
                   updated to step1 but later message validation failed.
06/20/14   db      Mainlined FEATURE_WCDMA_HS_RACH_DELAY_CU and introduced NV control
06/17/14   sp      Made changes to mainline RLC-restablihment redesign changes
06/02/14   sp      Made changes for FR19997 - UL rlc re-establishment redesign
04/10/14   db      Made changes to detect UL size change during FACH to FACH redirection
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/14/14   sp      Made changes to init re-establish entity on Procedure cleanup
03/13/14   sp      Removed code under FEATURE_WCDMA_HS_PCH feature flag
02/13/14   ad      Made changes to remove redundant F3s
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
06/28/13   db      rbreconfig changes to buffer the MM and RR commands for crash debugging
06/27/13   db      Backed out buffer the MM and RR commands for crash debugging
06/28/13   db      Made changes to not access NULL pointer
06/27/13   gv      Made changes to buffer the MM and RR commands for crash debugging
06/27/13   sa      Added changes to support call type mask for AMR with PS0x0 config
06/20/13   sa      Made changes for Default config in CELL_FACH
04/25/13   sg      Added code for REL9 under FEATURE_WCDMA_REL9
03/14/13   md      Made changes to provide active subscription ID to NAS APIs for DSDA
01/23/13   mp      Fixing KW errors
01/03/13   ad      Made changes to reduce the redundant F3s
12/12/12   mp      Fixing KW errors
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
09/28/11   mn      Made changes to send channel config request when UE receives cucnf
                   after coming back to service on dch to fach or pch transition     
09/26/12   pm      Made changes to reset the ordered codec when procedure is cleared
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/29/12   ad      Made changes for triggering CU if E-RNTI is not given.
                   Skipped the de-registration of WMs if HS-RACH is ON and RRC is 
                   triggering MR while coming out of PCH. Fixed bug to updated E-RNTI
                   in OC correctly. Added code to trigger CU if R99 or R5 CUCNF is received. 
                   Added code to keep L1 and MAC in sync wrt HS-RACH actions.
06/25/12   sa      Made the changes to delete the C-RNTI while configuring the CU
06/12/12   sa      Made the changes to keep the active codec info
04/25/12   as      Made changes to trigger cell update/drx req when UE recived OTA with 
                   redirection info and cell reselection in progress
04/06/12   ad      Changes for HS-RACH feature
04/30/12   db      Added check to swap CC and OC only on DCH to FACH fail
04/12/12   as      Added code to support feature NB/WB handover support
04/04/12   as      Made changes to initiate transition to idle when L2 ACK fails while going to PCH
26/03/12   ad      Added changes to set the TOC for OOS if DCH to FACH timer expiry 
                   happens before OOS indication comes from L1. Also added code to 
                   send channel config request from procedure after recovery from OOS.
12/09/11   ad      Added code to log WCDMA_RRC_PROTOCOL_ERRORS packet
12/08/11   db      Made changes to trigger cell update if H-RNTI is missing 
11/20/11   db      Added changes to wait for RBRC to wait for l2 ack in all states
                   not just DCH
10/13/11   db      Made changes to restore/clear ordered codec when RB failure occurs
08/17/11   ad      Made changes to send channel config request when UE receives cucnf
                   after coming back to service on dch to fach or pch transition     
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/11/10   prk     Removed the code which do double freeing of the memory.
10/08/10   prk     Removing unnecessary code for e-fach calls.
10/04/10   prk     Added code to support FEATURE_WCDMA_HS_PCH.
09/20/10   prk/kp  Added code to support FEATURE_WCDMA_HS_FACH
09/17/10   su      Featurizing MVS related code
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
09/13/10   as      Added code to support FEATURE_DUAL_SIM
07/14/10   su      Fixed Lint errors.
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
06/10/10   prk     Made changes to wait for L2 ACK after sending reconfiguration
                   failure message if the failure is due to the lower layer configuration.
06/07/10   ss      Added code to reset SVTHHO in case of failure message
05/07/10   prk     Fixed compiler warnings.
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
03/12/10   dm      Added code for supporting Deferred SIB reading feature.
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
12/21/09   gkg     Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
12/14/09   kp      Made changes send RAB_EST_IND only if PS doamin is open.
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
10/28/09   gkg     Fixed KW Errors.
10/13/09   gkg     Made the changes such that Codec info is set to RRC_CODEC_NONE
                   if CS DATA TYPE is mentioned in RRC_SYNC_IND.
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
10/08/09   gkg     Fixed compile warning.
10/01/09   gkg     Made changes so that SCUDIF is handled based on derived
                   configuration instead of relying on MM.
09/25/09   gkg/ps  Made changes to not intiate cell selection for FACH->FACH or
                   FACH->PCH redirection, if cell reselction is ongoing.
08/17/09   ss      Added code to provide ciphering ON/OFF indication to UI 
                   under feature flag FEATURE_UMTS_VOICE_CIPHERING_IND.
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
12/17/08   gkg     When RLC RESET or RL Failure happens during DCH -> FACH transition, 
                   made changes so that procedure triggers Cell Update after receiving 
                   successful channel config cnf.
12/08/08   ps      Made changes to call mvssup_set_voc_timing() for Inter-Freq
                   HHO/Timing Initialised HHO and Forced SYNCA
11/28/08   gkg     Made Changes for new MVS-RRC Interface to (de)configure vocoder
                   for AMR Voice Calls.
11/12/08   gkg     Made Changes so that T305 is started by the Reconfig procedure
                   as soon as Reconfig procedure initiates Cell Selection. This is
                   done for Cell_DCH -> Cell_FACH state.
11/04/08   rm      Added support for rove-in to GAN in RRC Connected mode.
                   Code changes are under FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE.
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
06/13/08   gkg     Added code for notification of Cell_Id change through OTAs
                   in Cell_DCH state. 
06/13/08   gkg     Made changes so that if RLC PDU size changes for RBs through
                   Transport Channel IEs of RB-Reconfig OTA, those RBs are reconfigured
                   and therefore their RLC entity is notified for new RLC PDU size.
06/13/08   gkg     For SCUDIF call through RB-Reconfig, the no. of RBs in the modified
                   RAB have't been populated in rbrc variable. So made changes in
                   rrcrbrc_configure_rab_change() to rely on global rrc_est_rabs instead
                   of relying on rbrc for no. of RBs.
05/30/08   gkg     In case of Cell_FACH->Cell_DCH state transition, stop T305
                   if RRC_CHANNEL_CONFIG_REQ succeeds otherwise handle timer T305
                   by initiating Cell Update.
04/17/08   ps      Fixed compiler error when FEATURE_SCUDIF_ENHANCEMENT and 
                   FEATURE_VOC_AMR_WB are enabled 
04/14/08   gkg     Made changes for configuration of AMR Call so that no. of AMR
                   classes in UL to be registered with MVS is 3 whenever no. of RBs in
                   voice RAB is 3. This is done irrespective of amr Rate.
03/11/08   kp      Fixed lint errors for Negative indentation in 
                   function rrcrbrc_update_ordered_config()
03/06/08   da      Added MBMS phase 2C changes.  MBMS support in CELL_DCH.  
                   Includes mobility and dynamic resource management.
11/02/08   gkg     Made changes so that whenever there is a mismatch in what is
                   expected in an OTA msg and what is received, always INVALID 
                   Config is returned instead of UNSUPPORTED Config.
01/30/08   gkg     Made changes to send Start value in the Complete msg,
                   only when the Re-establish is caused by RBRC 
01/08/08   ps/gk   Made modifcations for SCUDIF to use NAS function for
                   quering if RAB type has changed, when RAB Info not
                   provided in OTA
11/28/07   da      In case OOS area gets detected during DCH->FACH transitions,
                   made change to setup DCCH+DTCH once returning in service before
                   sending CU message with cause "re-entered service area"
11/28/07   rm      Added support for handling multiple RLC operations on a
                   single RB under FEATURE MULTIPLE RLC OPERATIONS
26/11/07   gkg     Added support for VT->Voice fallback and vice versa
                   by RB-Reconfig msg for SCUDIF under feature flag 
                   FEATURE_SCUDIF_ENHANCEMENT
11/08/07   rm      Made changes to fix compilation error when both
                   FEATURE_BYPASS_SYNCA_POST_VERI and FEATURE_VOC_AMR_WB
                   features are defined
10/23/07   kp      Added support for network sharing feature. Code changes to handle 
                   primary PLMN I.D in OTA message. Changes are under feature flag 
                   FEATURE NETWORK SHARING.
09/25/07   ps      Added fix to send DRX Req to L1, when Redirection IE is
                   given for the camped PSC, in FACH->PCH case. This is valid
                   when FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU is enabled.
09/10/07   ps      Added support for Frequency redirection received in 
                   FACH->FACH and FACH->PCH cases under feature flag
                   FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
08/29/07   ps      Made correction to validate an SRB5 if present in the 
                   Rb Reconfig list
08/08/07   ps      Added support for feature cell id reading in DCH state, 
                   under feature flag FEATURE_WCDMA_GPS_CELL_ID_READING.
04/24/06   ps      Made corrections for merge issues for WB-AMR under 
                   FEATURE_VOC_AMR_WB
04/20/07   da      Made changes needed to update ASN.1 version to R6 sept R6 2006
03/14/06   ps      Made correction to send failure cause as "Unsupported 
                   config" if an unknown codec type is received in OTA
03/02/06   ps      Added support for Wb-AMR under the feature flag
                   FEATURE_VOC_AMR_WB.
02/07/07   ps      Made changes to handle Standalone Rb-Mappings
02/02/07   da      Removed code that use to handle DCH->FACH activation time
                   and 300ms delay.  functionality has been moved to LLC. 
12/22/06   ps      Made changes to handle postVerificationPeriod and the
                   timingMaintainedSynchInd IE received in OTA messages
11/15/06   sm      Make use of function rrcllc_validate_rb_info_reconfig_list_r6()
                   instead of code-duplication
09/12/06   da      Fix for lint warnings
09/12/06   sm/da   Fix code review comments and general clean-up for REL6 and HSUPA
09/11/06   da      Added support for FEATURE RLC ONE SIDE RE ESTABLISHMENT
09/05/06   da      Added new func rrcrbrc_get_new_crnti_valid().  This function
                   will be called when CU wants to know if the OTA msg
                   included a new CRNTI.  Also made change not to reset 
                   new_crnti_valid flag when proc is waiting for CU CNF and 
                   OOS has been detected.  This is because proc needs to send
                   a RNTI update req.
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/28/06   da      Added support for OOS area handling during transition from DCH.
06/13/06   segu    Added support for DSAC future implementation
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE REL5 while retaining the HSDPA specifc processing 
                   under FEATURE HSDPA.
05/03/06   da      Fix for compilation error introduced inside FEATURE_UMTS_PDCP.                          
04/25/06   da      Added checking the status of RNTI_UPDATE_CNF.  If failed, 
                   send failure to RBRC msg and clear procedure.
04/18/06   da      Modified code to support migration to Mar-06 Rel-5 ASN1 +
                   R6 Hacks
03/15/06   tkk     Lint warnings/errors cleanup
03/13/06   da      Removed sending CN domain info to NAS before configuring
                   Lower Layer in R5 part of code.
03/10/06   da      Added Initial support for PDCP under FEATURE_UMTS_PDCP
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1
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
12/01/05   da      If activation time for DCH-FACH transition is greater than 300 ms,
                   make delay before cell selection as long as activation time.
11/10/05   da      Removed extern definition of nv_rel_ind;
11/10/05   da      Checking global flag rrc_nv_rel_indicator before doing R5 related
                   processing.
10/26/05   vr      Send UL-Counter Synch info in the RBReconfigComplete message
                   only if TM ciphering is still active.Also corrected the bitmask
                   names
10/05/05   ss      added check for return value of get_mac_d_hfn and added a
                   print
09/26/05   ss      added support for mac-d-hfn in procedures
07/25/05   ss      Added explicit registration for cellupdte cnf on L2 nack flr.
06/15/05   da      Sending rb failure msg with cause "cell update occured" when OOS area
                   occurs while RB proc is waiting for l2 ack.  
                   Also passing indication to func rrcsend_send_ul_sdu to not send L2 ack
                   to procedure when procedure is not waiting for l2 ack.  
                   Added fix for GCF where UE was sending failure message [due to later_than_r3
                   msg chosen] with wrong transaction ID.
06/06/05   da      Calling func rrcllc_print_trans_chnl_rate() after sending complete
                   msg to print trans chan rates.
05/16/05   da      Added support for validation failure handling from L1
04/14/05   da      Added support for Phase 2 of dch failure handling: going back
                   to old configuration.
03/28/05   da      Fix so that re_est_ind is sent to RABM for all RBs that got reconfiged
                   and affected.
03/25/05   da      Removed FEATURE CM SUPPORTED from rrcrbrc_process_channel_config_cnf.
                   Had an invalid check that looks if failure was due to compressed mode and send
                   rbrc failure msg with cause invalid config.
03/11/05   vk      Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE HSDPA.
03/02/05  ttl      Support RRC ASN1 Release 5.
01/04/05   da      Added Support for phase 1 of Physical channel establishment 
                   failure handling within DCH. 
11/30/04   vm/da   Added appropriate MSG_HIGHs for new RRC/NAS interface 
                   RRC_SERVICE_IND_FROM_OTA_MSG.
11/23/04   sm      Added support for Dynamic Power Class feature
11/22/04   da      Added support for phy chan setup fail and phy chan est fail in 
                   CELL_FACH->DCH from a cell update cnf msg and rb setup/reconfig.
10/25/04   svk     process_rbrc_message() : If stop and continue is being configured for a transparent mode rb
                   then just print an error message. Do not reject the RB reconfig message in this case. 
                   Also modified some error messages that were getting printed while processing the rbrc msg.
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
08/30/04   sgk     Removed return after ERR_FATAL in functions
                   rrcrbrc_append_start_list_to_rbrc_complete,
                   send_rbrc_failure_message, send_rbrc_complete_message,
                   rrcrbrc_send_indication_to_upper_layers to fix lint error
                   Unreachable.
                   Checked if the function rrcsmc_process_cipher_config_for_hho
                   returned FAILURE and if so printed a MSG_HIGH in function
                   validate_rbrc_message, to fix lint error return value of 
                   function not being checked.       
                   Checked if the function 'rrcllc_reset_ordered_config(enum {...})'
                   return != OC_NOT_SET and printed a Msg high in function
                   rrcrbrc_process_l2_ack_ind.
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
11/13/03   sk      Merged the Cell_PCH/URA_PCH supporting code 
01/26/04   sk      clear the ordered_config variable immediately after sending response
                   message, when SRNS relocation is not involved. 
                   Handle l2ack in rrcrbrc substates otherthan waiting_for_l2_ack substate.
                   SRNS relocation support in rbreconfig complete message triggered by 
                   CU proc
12/10/03   bu      Removed the calls to clear_ordered_config(). It will be called
                   only when L2 Ack is received. Also removed handling of NEW_CELL_IND
                   as it will never be received.
11/18/03   sk      Send the indication to upper layers when rbreconfig complete
                   message is submitted to lower layers.
                   Go to idle if L2ack for rbreconfig complete message fails and 
                   the procedure involves SRNS relocation. Else do not take
                   any action on L2ack failure.
11/17/03   kc      Added SRNS support in RB-Reconfig procedure.
08/11/03   vn      Initialize new fields for RLC re-establishmnet in RNTI_UPDATE_REQ
08/07/03   sk      Do not query rabm if "rab info to reconfigure" IE is
                   present.
07/29/03   sk      Modified validate_rb_ids function to use new function  
                   rrc_find_rb_in_est_rabs defined in rrcdata.c
06/24/03   sk      Do not send the indication to upper layer when L2ack fails.
06/20/03   sk      Initiate CU if l2ack for rbrc complete fails
06/20/03   sk      Added new function to validate rb_ids
06/04/03   sk      Terminate RBRC procedure after sending the failure with 
                   appropriate cause when message validation fails.
05/04/03   sk      RBRC notifies RABM about the successfulreconfiguration if there 
                   is any PS-RAB even when RABinfotoreconfigure is not present.
05/21/03   sk      Removed the special character that was causing compilation errors.
05/09/03   sk      Graceful handling of SRNS Relocation failure
                   If New-URNTI is present then send failure with cause 
                   "unsupported configuration"
05/01/03   sk      Removed unused element from the internal data structure.
                   Removed unused global declaration.
04/08/03   sk      Added the processing of rb info affected list.
                   Removed rrcrbrc_ie_e_type structure
                   Added rrcrbrc_rb_info_type structue
                   Removed rrcrbrc_cmd global declaration
                   Removed send_msg_to_llc function 
03/18/03   sk      Modified the transaction Id checking in process_rbrc_message()
03/04/03   sk      Check whether the RAB_ID in the reconfig message is present 
                   in ESTABLISHED_RABS during validation of rbreconfig message. 
02/13/03   vn      Added functionality to process Reconfig message going from
                   CELL_DCH to CELL_FACH, that comes with a new C-RNTI. If
                   camped on the same cell as directed and if C-RNTI is
                   available, do not initiate Cell Update procedure. 
02/05/03   vn      Defined new functions to handle State Change Indications
                   to Idle.
02/10/03   sk      Fixed problem in validate_rbrc_message where RAB info to
                   Reconfigure IE was being accessed without checking if it
                   was present.
02/05/03   vn      Merged changes dropped during linting.
           vn      Merged the following from Cell_FACH branch:
           vn      On a state change to Idle, do not clear ordered config.
           upn/vn  Handle a state change to Idle when the procedure is active.
           vn      Added new parameter proc_id in rrcllc_set_ordered_config.
01/20/03   upn     Process recvd message for RRC_DOWNLINK_SDU_IND in 
                   RRCRBRC_WAIT_FOR_CHAN_CFG_CNF & RRCRBRC_WAIT_FOR_L2_ACK
                   substates
02/04/03   ram     Checked in lint changes, updated copyright.
12/12/02   ram     Corrected typo in previous checkin.
11/22/02   vn      Wherever action is taken based on the next_state in
                   Channel Config Request, also check for 
                   rrc_state_change_required flag. 
10/30/02   vn      Added an extra delay before starting cell selection when
                   going DCH->FACH. This is to ensure that L2 Ack for RB
                   Reconfiguration Message on DCH is received by RNC.
10/24/02   rj      Updated to use rrc_malloc and rrc_free to use dual heaps 
                   in place of mem_malloc and mem_free to avoid memory
                   fragmentation
09/27/02   upn     cell_DCH - cell_FACH transitions implementation
09/26/02   xfg     Changed for WVS to MVS migration. The change was made under
                   FEATURE_MVS.
09/25/02   bu      Changed l1_status to chan_cfg_status under
                   FEATURE CM SUPPORTED.
09/20/02   bu      If reselection is in progress or if cell update has started
                   and register for CU CNF accordingly. On receiving cell update
                   confirm send RB Reconfig failure to UTRAN. If C_RNTI is not valid
                   register for CU CNF. On receiving CU CNF send the RB Reconfig
                   Complete. Call rcllc_get_ordered_config_status() instead of
                   rrcllc_is_ordered_config_set().
08/28/02   vk      Processed Compressed Mode failure from LLC and send a
                   Radio Bearer Reconfiguration Failure message to UTRAN
                   with error as Invalid Configuration. Changes have been
                   put under FEATURE CM SUPPORTED
08/15/02   upn     Fixed CELL_FACH unsupported config flag 
06/25/02   upn     Code implemented for state CELL_FACH support.
06/06/02   upen     Added definition for rrcrbrc_build_rbrc_complete_message
05/30/02   upen     Added June conversions and CELL_DCH functionality. Disabled
                    iot reconfig failure. 
04/25/02   upen     Modified rrcrbrc_send_iot_failure_msg failure cause to protocol
                    error with cause  rrc_messageTypeNonexistent.
03/19/02   upen     added rrcrbrc_send_iot_failure_msg functionality for sending
                    failure messages on receiving reconfig message.
08/06/01   upen     Created file.

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
#include "rrcasn1util.h"
#include "rrccmd_v.h"
#include "rrcdata_v.h"
#include "rrcintif.h"
#include "rrcccm.h"
#include "rrccu.h"
#include "rrccui.h"
#include "rrclbt.h"
#include "rrclcm.h"
#include "rrcllc.h"
#include "rrcmmif.h"
#include "rrcmnif.h"
#include "rrcrbcommon.h"
#include "rrcrbreconfig.h"
#include "rrcscmgr.h"
#include "rrcsend.h"
#include "rrccspi.h"
#include "rrccsp.h"
#include "rrctmr.h"
#include "rrcsibproc.h"
#include "rrcsmc.h"
#include "rrclog.h"

#ifdef FEATURE_MVS
#include "mvs.h"
#endif  /* FEATURE_MVS */

#include "rrciho.h"
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
#if 0
typedef enum
{
  RRCRBRC_INITIAL,                   /* Initial Substate                    */
  RRCRBRC_WAIT_FOR_CELL_UPDATE_CNF,  /* Wait for cell update                */
  RRCRBRC_WAIT_FOR_CELL_SELECTION_CNF,  /* wait for cell selection      */
  RRCRBRC_WAIT_FOR_CHAN_CFG_CNF,     /* Waiting for chan cfg cnf from LLC   */                      
  RRCRBRC_WAIT_FOR_CRNTI_UPDATE_CNF,     /* Waiting for CRNTI update cnf   */                      
  RRCRBRC_WAIT_FOR_L2_ACK,           /* Waiting for Layer 2 ack for uplink
                                    message transmission                   */
  RRCRBRC_WAIT_FOR_REESTABLISH_SRB2, /*waiting for re-establishment of srb2 */
  RRCRBRC_MAX_SUBSTATES
}rrcrbrc_substate_e_type;
#endif

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
/* We need to introduce a delay before transmitting
on a new configuration if the base-station cannot have
the old link and new link up at the same time. */
  #define RRCRBRC_TEMP_TIMER_SIG 0x4001
rex_sigs_type rrcrbrc_sig;
rex_timer_type rrcrbrc_temp_timer;

typedef enum
{
  RB_STOP_CONTINUE_ABSENT=0,
  RB_STOP,
  RB_CONTINUE
}rrcrbrc_rlc_stop_cont_e_type;

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
typedef struct
{
  boolean is_valid;     /* indicates if there are RBs that need to be 
                           released/affected. If this is FALSE, the two
                           items below are not used                       */
  uint32 num_rbs;       /* Number of RBs released by the message          */
  rrc_RB_Identity rb_id[MAX_RB]; /* Hold the ids of the RBs that are
                           released/affected by the RBR message           */
  /* Indicate if for this ie, stop or continue is specified. */
  rrcrbrc_rlc_stop_cont_e_type  rb_stop_continue[MAX_RB]; 
  uint16 ul_cfg_mask[MAX_RB];
  uint16 dl_cfg_mask[MAX_RB];
}rrcrbrc_rb_info_type;



/* Structure to store all internal data related to RBRC procedure           */
typedef struct
{
  rrcrb_substate_e_type         curr_substate;  /* Store the current substate     */
  rrcrb_transaction_id_type     tr_ids;      /* Transaction ids for RR msg    */
  rrc_state_e_type              state_indicated_by_msg; /* RRC State given by RR msg     */
  rrcrb_msg_failure_cause_type  status;   /* Status of message validation   */
  rrcrbrc_rb_info_type          rbs_tobe_reconfigured;/* RBs to be reconfigured*/
  rrcrbrc_rb_info_type          rbs_tobe_affected; /* RBs to be affected       */
  rrcrb_rab_info_per_msg_type   rabs;      /* Hold info about each rab in one
                                         RB RC message till they're processed*/
  boolean                       directed_cell_info; /* Indicates if a valid C-RNTI was recd */
  boolean                       new_crnti_valid; /* Indicates if a valid C-RNTI was recd 
                                                  * in the message transitioning from CELL_DCH
                                                  * to CELL_FACH */
  uint16                        new_crnti;
  rrc_int_u_rnti_type           new_urnti;
  boolean                       rbrc_for_srns_relocation;
  boolean                       cn_info_saved;
  mm_cmd_type *                 mm_cmd_ptr;

  boolean                       hho_ciph_status;


  boolean                       trans_to_dch_ciph_config_needed;
 /* Indicates if Fre-redirection is to be triggered in FACH->PCH case */
  boolean initiate_cell_selection_trans_from_fach;

  boolean high_mobility_ind;

}rrcrbrc_int_data_type;

LOCAL rrcrbrc_int_data_type rbrc;


boolean rrcrbrc_send_initiate_cu_flag = FALSE;
boolean rrcrbrc_delete_crnti = FALSE;

#ifdef FEATURE_UMTS_PDCP
extern boolean rrc_pdcp_enabled;
#endif /* FEATURE_UMTS_PDCP*/

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
boolean rrcrbrc_is_srns_in_progress
(
  void
)
{
  return rbrc.rbrc_for_srns_relocation;
}
/*===========================================================================

FUNCTION   RRCRBRC_GET_NEW_CRNTI_VALID()

DESCRIPTION
 This function returns if reconfig msg has valid CRNTI


        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcrbrc_get_new_crnti_valid
(
  void
)
{
  return rbrc.new_crnti_valid;
}
 /*===========================================================================

FUNCTION   RRCRBRC_GET_CURRENT_SUB_STATE()

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
rrcrb_substate_e_type rrcrbrc_get_current_sub_state( )
{
  return rbrc.curr_substate;
}
/*===========================================================================
 
 FUNCTION   rrcrbrc_get_dest_state()
 
 DESCRIPTION
  This function is used to return destination state
 
 
 
 DEPENDENCIES
 
   None.
 
 RETURN VALUE
 
   rrc_state_e_type
 
 SIDE EFFECTS

   None.

 ===========================================================================*/
 rrc_state_e_type rrcrbrc_get_dest_state(void)
 {
   return rbrc.state_indicated_by_msg;
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
boolean rrcrbrc_is_proc_successful( )
{
  if(rbrc.status.failure_status == RRCRB_MSG_VALID)
    return TRUE;
  else return FALSE;
}
 

/*===========================================================================

FUNCTION   rrcrbrc_update_failure_status()

DESCRIPTION
  Updates the failure status
DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
void rrcrbrc_update_failure_status(rrcrb_msg_status_e_type status )
{
  rbrc.status.failure_status = status;
  WRRC_MSG1_HIGH("Updated Failure status to %d",status);
  return;
}


/*===========================================================================

FUNCTION   rrcrbrc_configure_rab_change()

DESCRIPTION
 This function re-configures RABs and other lower layer parameters for VT -> Voice 
 and Voice -> VT fallback for RB-Reconfig OTA msg. Also, if SCUDIF is not happening, then
 this function checks/configures for vocoder/rate change for Voice RAB. 
        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbrc_configure_rab_change(void)
{
#if defined (FEATURE_SCUDIF_ENHANCEMENT) || defined (FEATURE_VOC_AMR_WB)
  uint8 rab_index =0;
#endif /*FEATURE_SCUDIF_ENHANCEMENT || FEATURE_VOC_AMR_WB*/

#ifdef FEATURE_SCUDIF_ENHANCEMENT
  #error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/

#ifdef FEATURE_VOC_AMR_WB
  mvs_mode_type nas_sync_ind = rrc_get_current_codec_mode();
#endif /*FEATURE_VOC_AMR_WB*/

  WRRC_MSG1_HIGH("No. of RABs in RBRC %d", rbrc.rabs.num_rabs_in_msg);
#ifdef FEATURE_SCUDIF_ENHANCEMENT
  #error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/
  {

#ifdef FEATURE_VOC_AMR_WB

    for(rab_index =0;rab_index <rbrc.rabs.num_rabs_in_msg; rab_index++)
    {
      if(rbrc.rabs.rabs_in_msg[rab_index].rab_type == RRCRB_CS_VOICE_RAB)
      {
        nas_sync_ind = rbrc.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator;
        break;
      }
    }
  
#endif /*FEATURE_VOC_AMR_WB*/
    
#ifdef FEATURE_VOC_AMR_WB
    rrcrb_update_amr_cctrch_id_for_reconfig(nas_sync_ind);
  
#else/*FEATURE_VOC_AMR_WB*/
    rrcrb_update_amr_cctrch_id_for_reconfig();
#endif/*FEATURE_VOC_AMR_WB*/
  }
}


/*===========================================================================

FUNCTION rrcrbrc_append_start_list_to_rbrc_complete

DESCRIPTION
  This function appends start list to rbrc message for SRNS relocation and
  also when TM bearers are present during non-dch to dch rrc-state
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
static void rrcrbrc_append_start_list_to_rbrc_complete
(
rrc_UL_DCCH_Message *msg_ptr               /* Pointer to Uplink DCCH Msg */
)
{
  /*start list is included in ul_counter_synchronization_info. So enable this IE in the
  bitmask */
  RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.radioBearerReconfigurationComplete,
    rrc_RadioBearerReconfigurationComplete,ul_CounterSynchronisationInfo); 
  RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.radioBearerReconfigurationComplete.ul_CounterSynchronisationInfo);
  /* Allocate memory for START List */
  /* call SMC API to append the start list */
  (void)rrcsmc_append_start_list(&msg_ptr->message.u.radioBearerReconfigurationComplete.ul_CounterSynchronisationInfo.startList,
                                          RRCSMC_EXTRACT_AND_APPEND_START);
}
/*===========================================================================

FUNCTION   SEND_RB RC_FAILURE_MSG

DESCRIPTION

  This function sends the Radio Bearer Reconfiguration Failure message
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
static void send_rbrc_failure_message
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
  ptr = (rrc_UL_DCCH_Message *)rrc_malloc( 
                                          sizeof(rrc_UL_DCCH_Message));

    WRRC_MSG2_HIGH("rbe subsstate(%d) Send failure with %d cause:   [0-VALID|1-UNSUPPORTED_CONFIG|2-PHY_CHAN_FAILURE|3-SIMULTANEOUS_RECONFIG|4-PROTOCOL_ERROR|5-INVALID_CONFIG|6-INCOMPLETE_CONFIG|7-CELL_UPDATE_OCCURED]", 
   rbrc.curr_substate, rbrc.status.failure_status);

    /* we need to revert cipher config db, if cipher config db has been updated with step-1 HO. 
     This revert takes care of the scenario of OTA failure in the gap between OC is 
     set and cphy is sent. This cannot happen in a normal case. This is for extra protection */
  rrcsmc_revert_to_old_config_for_failure_message(FALSE);

  RRC_RESET_MSG_IE_PRESENT_PTR(ptr); /* Integrity Check info not present */
  ptr->message.t = T_rrc_UL_DCCH_MessageType_radioBearerReconfigurationFailure; /* RR Failure msg */

  /* Set the bit-mask -  ?? */
  RRC_RESET_MSG_IE_PRESENT(ptr->message.u.radioBearerReconfigurationFailure);

  /* Transaction Id - First check the rejected_transacion id. If
  it's not invalid, this message is being rejected because there
  is already another message accepted but not processed.
  If the rejected_transaction id is invalid, we are rejecting the
  accepted message, so use the accepted transaction id.

  NOTE: Don't change the order, it's important to check rejected
  transaction first. */
  if ( rbrc.tr_ids.rejected_transaction != RRCRB_INVALID_TRANSACTION_ID )
  {

    ptr->message.u.radioBearerReconfigurationFailure.rrc_TransactionIdentifier =
    rbrc.tr_ids.rejected_transaction;
    /* We no longer need to keep this id. Make rejected_transaction id
    invalid again */
    rbrc.tr_ids.rejected_transaction = RRCRB_INVALID_TRANSACTION_ID;
  } 
  else if ( rbrc.tr_ids.accepted_transaction != RRCRB_INVALID_TRANSACTION_ID )
  {
    ptr->message.u.radioBearerReconfigurationFailure.rrc_TransactionIdentifier =
    rbrc.tr_ids.accepted_transaction;
    /* We no longer need to keep this id. Make rejected_transaction id
    invalid again */
    rbrc.tr_ids.accepted_transaction = RRCRB_INVALID_TRANSACTION_ID;
  } 
  else
  {
    /* Don't know the transaction id for this message.( one reason
    for this could be that the message had a critical
    extenstion). Since it's unknown, we should set it to zero. */
    ptr->message.u.radioBearerReconfigurationFailure.rrc_TransactionIdentifier= 0;
    WRRC_MSG0_HIGH("Invalide Transaction id for RBRF message!");
  }

  switch ( failure_cause )
  {
  case RRCRB_MSG_UNSUPPORTED_CONFIG:
    ptr->message.u.radioBearerReconfigurationFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_configurationUnsupported;
    break;

  case RRCRB_PHY_CHAN_FAILURE:
    ptr->message.u.radioBearerReconfigurationFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_physicalChannelFailure;
    break;

  case RRCRB_SIMULTANEOUS_RECONFIG:
    ptr->message.u.radioBearerReconfigurationFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_incompatibleSimultaneousReconfiguration;
    break;

  case RRCRB_MSG_PROTOCOL_ERROR:
    ptr->message.u.radioBearerReconfigurationFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_protocolError;

    /* When there is a Protocol Error, we also have to indicate
    the protocol error cause */
    ptr->message.u.radioBearerReconfigurationFailure.failureCause.u.protocolError =
	  	rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation);
    
    ptr->message.u.radioBearerReconfigurationFailure.failureCause.u.protocolError->
      diagnosticsType.t = T_rrc_ProtocolErrorInformation_diagnosticsType_type1;

    ptr->message.u.radioBearerReconfigurationFailure.failureCause.u.protocolError->
      diagnosticsType.u.type1 = rtxMemAllocTypeZ (&enc_ctxt, rrc_ProtocolErrorInformation_type1);
    
    /* Copy the given protocol error cause */
    ptr->message.u.radioBearerReconfigurationFailure.failureCause.u.protocolError->
      diagnosticsType.u.type1->protocolErrorCause = prot_err_cause;
    break;

  case RRCRB_MSG_INVALID_CONFIG:
    ptr->message.u.radioBearerReconfigurationFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_invalidConfiguration;
    break;

  case RRCRB_MSG_INCOMPLETE_CONFIG:
    ptr->message.u.radioBearerReconfigurationFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_configurationIncomplete;
    break;

  case RRCRB_CELL_UPDATE_OCCURED:
    ptr->message.u.radioBearerReconfigurationFailure.failureCause.t =
    T_rrc_FailureCauseWithProtErr_cellUpdateOccurred;
    break;

  default:
    WRRC_MSG0_ERROR("Sending RBRC Unknown Failure message");
    ptr->message.u.radioBearerReconfigurationFailure.failureCause.t =
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
    failure_ota.msg_id = WRRC_UL_DCCH_MESSAGETYPE_RADIOBEARERRECONFIGURATIONFAILURE;
    if(RRCSMC_IP_STARTED == rrcsmc_check_integrity_status())
    {
      failure_ota.ul_fail_ota.ul_ota_parms.ul_integrity_info_present = 1;
    }

    failure_ota.ul_fail_ota.ul_ota_parms.failure_cause = ptr->message.u.radioBearerReconfigurationFailure.failureCause.t;
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
      ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.ul_ota_parms.failure_cause= ptr->message.u.radioBearerReconfigurationFailure.failureCause.t;   
      /* Commit data will be called in send ul sdu */
    }
  }
#endif
    rrc_log_protocol_error( (uint8) (rrc_get_state()),
                            (uint8) RRC_PROCEDURE_RBRC,
                            (uint8) ptr->message.u.radioBearerReconfigurationFailure.failureCause.t,
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
    WRRC_MSG1_HIGH("Sending RBRC Failure msg to send-chain Cause:%d",
             ptr->message.u.radioBearerReconfigurationFailure.failureCause.t);
    
    /* Need to wait for L2 ack for all states because RRC needs to trigger include reconfig status indicator
    if RLC acknowledgement is not yet received.
    As per 8.3.1.3
    1> if the IE "Cell update cause" is set to "radio link failure" and the UE has submitted a reconfiguration response
    message to RLC and the UE has not received the RLC acknowledgement for the response message:
    2> include the IE "Reconfiguration Status Indicator" and set it to TRUE.*/
    if(rbrc.status.failure_status == RRCRB_PHY_CHAN_FAILURE)
    {
      rrc_update_reconfig_waiting_for_l2ack_dch(RRC_PROCEDURE_RBRC);
      status = rrcsend_send_ul_sdu(RRC_PROCEDURE_RBRC,  /* Procedure is RBRC    */
                                   (void *)ptr,        /* Message pointer     */
                                   ul_lc_id,           /* RLC id              */
                                   RRCSEND_L2ACK_REQUIRED  /* ACK is required */
                                  );
    }
    else
    {
      status = rrcsend_send_ul_sdu(RRC_PROCEDURE_RBRC,  /* Procedure is RBRC    */
                                   (void *)ptr,        /* Message pointer     */
                                   ul_lc_id,           /* RLC id              */
                                   RRCSEND_L2ACK_NOT_REQUIRED  /* No need for ACK */
                                  );
    }     
    if ( status != RRCSEND_SUCCESS )
    {
      WRRC_MSG1_ERROR("Send chain failed for RB RC failure msg: error cause %d", status);
    }
  } else
  {
    /* Unexpected failure. ERR for now. May need to ERR_FATAL here */
    WRRC_MSG1_ERROR("Can't send UL DCCH msg! Invalid lc id: %d", ul_lc_id );
    /* It's important to free the memory since we didn't send the
    message. */
    rrc_free( ptr );
  }
}/* end function send_rbrc_failure_message */



/*===========================================================================

FUNCTION   SEND_RBRC_COMPLETE_MSG

DESCRIPTION

  This function sends the Radio Bearer Reconfig Complete message
  on Uplink DCCH. 
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/     
static void send_rbrc_complete_message( boolean proc_wait_for_l2_ack )
{

  rrc_UL_DCCH_Message *ptr;     /* Pointer to uplink DCCH message type */
  rlc_lc_id_type ul_lc_id; /* Logical Channel Id for the message  */
  rrcsend_status_e_type status; /* Status from send chain module       */
  rrcllc_oc_set_status_e_type oc_status = rrcllc_get_ordered_config_status();

  /* Allocate memory for this message. Note that this buffer is released
  by the SEND CHAIN */
  ptr = (rrc_UL_DCCH_Message *)rrc_malloc( 
                                          sizeof(rrc_UL_DCCH_Message));
  if (proc_wait_for_l2_ack)
  {
     WRRC_MSG0_HIGH("Waiting for L2 ACK for RBRC Complete msg");
  }

  RRC_RESET_MSG_IE_PRESENT_PTR(ptr); /* Integrity Check info not present */
  /* RB RC Complete msg */
  ptr->message.t = T_rrc_UL_DCCH_MessageType_radioBearerReconfigurationComplete; 

  /* Fill up the message contents. The bit_mask is set to 0 since none
  of the optional IE's are included for now. */
  RRC_RESET_MSG_IE_PRESENT(ptr->message.u.radioBearerReconfigurationComplete);

  if (rbrc.rbrc_for_srns_relocation == TRUE)
  {
    rrcrbrc_append_start_list_to_rbrc_complete(ptr);
  }
  else
  {
    if (rrcllc_get_rlc_size_change_status() 
      && ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH || 
             rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
          || (rrc_get_rlc_size_change_status_for_procedure_id(RRC_PROCEDURE_RBRC) == TRUE)))
    {
      // Check if next state is CELL+PCH, and OC API returns RLC size change, then include counter sync
      if (rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH || 
          rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
      {
        /* Ask SMC to do start value calculation for PS domain */
        WRRC_MSG0_HIGH("RLCSIZEINDICATED Invoking SMC extract func");
        (void)rrcsmc_extract_return_hfn_per_domain_and_update_start_flag(RRC_PS_DOMAIN_CN_ID);
        
        
      }
      else if (rrc_get_rlc_size_change_status_for_procedure_id(RRC_PROCEDURE_RBRC) == TRUE)
      {
        rrc_set_rlc_size_change_status(RRC_PROCEDURE_NONE, 
                                       FALSE, RRC_INVALID_RB_ID ,
                                       RLC_RE_ESTABLISH_NONE);
      }
        /*start list is included in ul_counter_synchronization_info. So enable this IE in the
        bitmask */
      RRC_MSG_COMMON_SET_BITMASK_IE(ptr->message.u.radioBearerReconfigurationComplete,
        rrc_RadioBearerReconfigurationComplete,ul_CounterSynchronisationInfo); 
      
      RRC_RESET_MSG_IE_PRESENT(ptr->message.u.
	  	radioBearerReconfigurationComplete.ul_CounterSynchronisationInfo);
     
      /* call SMC API to append the start list */
      (void)rrcsmc_append_start_list(
        &ptr->message.u.radioBearerReconfigurationComplete.ul_CounterSynchronisationInfo.startList,
        RRCSMC_APPEND_START);
    }
  } 

  //if this is case of IFHHO and not SRNS relocation(i.e. ul counter sync is not present)
  //then also send START_List and set count c act time and configure smc for Step_2

  if ( ((rbrc.hho_ciph_status == TRUE) || (rbrc.trans_to_dch_ciph_config_needed == TRUE)) &&
       (TRUE == rrcsmc_tm_ciph_active()))
  {
    if(! (RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.radioBearerReconfigurationComplete,
           rrc_RadioBearerReconfigurationComplete,ul_CounterSynchronisationInfo)))
    {
      rrcrbrc_append_start_list_to_rbrc_complete(ptr);
    }
  
    //Set the bitmask for count_c_act_time and copy the same..
    RRC_MSG_COMMON_SET_BITMASK_IE(ptr->message.u.radioBearerReconfigurationComplete,
      rrc_RadioBearerReconfigurationComplete,count_C_ActivationTime);
    
    ptr->message.u.radioBearerReconfigurationComplete.count_C_ActivationTime =
      rrcsmc_step_2_cipher_config_for_hho();
  
    //Send step 2 config for HHO.. only one of these flags will be active, reset both of them
    //here
    if(rbrc.hho_ciph_status)
    {
      rbrc.hho_ciph_status =  FALSE;      
    }
    else if(rbrc.trans_to_dch_ciph_config_needed)
    {
      rbrc.trans_to_dch_ciph_config_needed = FALSE;      
    }
  }

  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
  {
 #ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
    if(rrcsib_is_dmcr_ie_needed() == TRUE)
    {
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.radioBearerReconfigurationComplete, laterNonCriticalExtensions);
 
      RRC_RESET_MSG_IE_PRESENT(ptr->message.u.radioBearerReconfigurationComplete.laterNonCriticalExtensions);
 
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.radioBearerReconfigurationComplete.laterNonCriticalExtensions, v770NonCriticalExtensions);
 
      RRC_RESET_MSG_IE_PRESENT(ptr->message.u.radioBearerReconfigurationComplete.laterNonCriticalExtensions.v770NonCriticalExtensions);
 
      RRC_RESET_MSG_IE_PRESENT(ptr->message.u.radioBearerReconfigurationComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        radioBearerReconfigurationComplete_v770ext);
 
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(ptr->message.u.radioBearerReconfigurationComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        radioBearerReconfigurationComplete_v770ext, deferredMeasurementControlReading);
      
      ptr->message.u.radioBearerReconfigurationComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        radioBearerReconfigurationComplete_v770ext.deferredMeasurementControlReading = rrc_RadioBearerReconfigurationComplete_v770ext_IEs_deferredMeasurementControlReading_true;
    }
#endif
  }

  /* Transaction Id - use the accepted transaction id if known. Else
  use zero. Since this is a RB RC Complete message, we don't check 
  rejected transaction ids. */
  if ( rbrc.tr_ids.accepted_transaction != RRCRB_INVALID_TRANSACTION_ID )
  {
    ptr->message.u.radioBearerReconfigurationComplete.rrc_TransactionIdentifier=
    rbrc.tr_ids.accepted_transaction;
    /* We no longer need to keep this id. Make rejected_transaction id
    invalid again unless UE is waiting for RRCRB_WAIT_FOR_L2_ACK*/
    if (!proc_wait_for_l2_ack)
    {
      rbrc.tr_ids.accepted_transaction = RRCRB_INVALID_TRANSACTION_ID;
    }
  } else
  {
    /* Don't know the transaction id for this message.( one reason
    for this could be that the message had a critical
    extenstion). Since it's unknown, we should set it to zero. */
    ptr->message.u.radioBearerReconfigurationComplete.rrc_TransactionIdentifier = 0;
    WRRC_MSG0_HIGH("Transaction id invalid for RB RC Complete message!");
  }

  /* For now, there is nothing else to include in this message. We
  can give it to the send chain. */

  /* Get the RLC logical channel id for this msg from Logical Channel Manager ??*/
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
    (void) rex_clr_sigs(rex_self(), RRCRBRC_TEMP_TIMER_SIG);
    (void) rex_set_timer(&rrcrbrc_temp_timer, 5000);   /* 5 seconds */

    rrcrbrc_sig = rex_wait(RRCRBRC_TEMP_TIMER_SIG);
    MSG_LOW("RRCRBRC signal:%d", rrcrbrc_sig, 0, 0);
    (void) rex_clr_sigs(rex_self(), RRCRBRC_TEMP_TIMER_SIG);
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
    failure_ota.msg_id = WRRC_UL_DCCH_MESSAGETYPE_RADIOBEARERRECONFIGURATIONCOMPLETE;

    if(RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.radioBearerReconfigurationComplete,
     rrc_RadioBearerReconfigurationComplete,count_C_ActivationTime))
    {
        failure_ota.ul_fail_ota.ul_ota_parms.count_c_activation = ptr->message.u.radioBearerReconfigurationComplete.count_C_ActivationTime;
    }
		
    if (RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.radioBearerReconfigurationComplete,
    rrc_RadioBearerReconfigurationComplete,ul_CounterSynchronisationInfo))
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
    if(RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.radioBearerReconfigurationComplete,
      rrc_RadioBearerReconfigurationComplete,count_C_ActivationTime))
    {
      ((wrrc_qsh_metric_ota_s*)rrc_qsh_metric_db[WRRC_QSH_METRIC_OTA].data_ptr)->ota_info.ul_ota_parms.count_c_activation= ptr->message.u.radioBearerReconfigurationComplete.count_C_ActivationTime;
    }
		
    if (RRC_MSG_COMMON_BITMASK_IE(ptr->message.u.radioBearerReconfigurationComplete,
    rrc_RadioBearerReconfigurationComplete,ul_CounterSynchronisationInfo))
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
    WRRC_MSG0_HIGH("Sending RB Reconfig Complete Message to send-chain");
    if (proc_wait_for_l2_ack) 
    {
      status = rrcsend_send_ul_sdu(RRC_PROCEDURE_RBRC,  /* Procedure is RBRC    */
                                   (void *)ptr,        /* Message pointer     */
                                   ul_lc_id,           /* RLC id              */
                                   RRCSEND_L2ACK_REQUIRED  /* ACK is required */
                                   );
    }
    else
    {
      status = rrcsend_send_ul_sdu(RRC_PROCEDURE_RBRC,  /* Procedure is RBRC    */
                                   (void *)ptr,        /* Message pointer     */
                                   ul_lc_id,           /* RLC id              */
                                   RRCSEND_L2ACK_NOT_REQUIRED  /* ACK is not required */
                                   );
    }

    if ( status != RRCSEND_SUCCESS )
    {
      WRRC_MSG1_ERROR("Send chain failed for RB RC complete msg: error cause %d", status);
    }
  } else
  {
    /* Unexpected failure. ERR for now. May need to ERR_FATAL here */
    WRRC_MSG1_ERROR("Can't send UL DCCH msg! invalid lc id: %d", ul_lc_id ); 
    /* It's important to free the memory since we didn't send the
    message. */
    rrc_free( ptr );
  }

  rrc_send_rb_rate_info_packet();

}/* end function send_rbrc_complete_message */

/*===========================================================================

FUNCTION   RRCRBRC_SEND_INDICATION_TO_UPPER_LAYERS

DESCRIPTION

  This function sends the approprate indications to MM or RABM layers
  depending on the contents of radiobearerreconfiguration message that
  has been stored locally.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbrc_send_indication_to_upper_layers
(
  void
)
{
  mm_cmd_type   *mm_ptr;     /* To send the MM_SYNC_IND command */
  rabm_cmd_type *rabm_ptr; /* To send the RABMAS_RAB_ESTABLISH_IND command */
  uint32         rab_index=0; /* Used if there is more than 1 RAB in the message */
  uint32         rb_index=0; /* Used if there is more than 1 RAB in the message */
  uint32         local_rb_index=0; /* Used if there is more than 1 RAB in the message */
  boolean        rab_search = FALSE;

  /* send the nas cn info if stored */
  if (rbrc.cn_info_saved != FALSE)
  {
    /* Put the command on MM queue */
    rrc_send_rrc_message_to_mm(rbrc.mm_cmd_ptr);
    /* clear the flag */
    rbrc.cn_info_saved = FALSE;
  }

  for(rb_index = 0; rb_index <rbrc.rabs.num_rabs_in_msg; rb_index++)
  {
    if ((rbrc.rabs.rabs_in_msg[rab_index].rab_type == RRCRB_CS_VOICE_RAB) ||
        (rbrc.rabs.rabs_in_msg[rab_index].rab_type == RRCRB_CS_DATA_RAB) )
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
      mm_ptr->cmd.rrc_sync_ind.rab_info.rab_id = 
      rbrc.rabs.rabs_in_msg[rab_index].rab_id;
   
     /*By this time, rab type should have been updated in rbrc*/
      if(rbrc.rabs.rabs_in_msg[rab_index].rab_type == RRCRB_CS_VOICE_RAB)
      {
        mm_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type = RRCMN_CS_VOICE_CALL;
      }
      else if(rbrc.rabs.rabs_in_msg[rab_index].rab_type == RRCRB_CS_DATA_RAB)
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
      if(rbrc.rabs.rabs_in_msg[rab_index].rab_type == RRCRB_CS_VOICE_RAB)
      {
        if(rrc_get_current_codec_mode() == MVS_MODE_AMR_WB )
        {
          mm_ptr->cmd.rrc_sync_ind.rab_info.codec = RRC_CODEC_WB_AMR;
        }
        else
        {
          mm_ptr->cmd.rrc_sync_ind.rab_info.codec = RRC_CODEC_NB_AMR;
        }
        WRRC_MSG1_HIGH("Codec %d in RRC_SYNC_IND",mm_ptr->cmd.rrc_sync_ind.rab_info.codec);
      }
#endif/*FEATURE_VOC_AMR_WB*/
      /* Put the command on MM queue */
      WRRC_MSG2_HIGH("Dispatching out MM cmd / Sending RRC_SYNC_IND to MM for RAB type %d RAB-id %d", 
                  mm_ptr->cmd.rrc_sync_ind.rab_info.cs_rab_type, 
                  mm_ptr->cmd.rrc_sync_ind.rab_info.rab_id);
  
      rrc_send_rrc_message_to_mm(mm_ptr);
    }
    else if ( rbrc.rabs.rabs_in_msg[rab_index].rab_type == RRCRB_PS_DATA_RAB )
    {
    
      if(rrcscrr_is_cn_domain_open(RRC_PS_DOMAIN_CN_ID) == TRUE)
      {
        /* It's a PS RAB. Send the RABMAS_RAB_ESTABLISH_IND primitive to RABM. */
        if ((rabm_ptr = rabm_get_cmd_buf()) == NULL)
        {
          ERR_FATAL("Can't send RABM cmd - out of memory!", 0, 0, 0);
        }
        rabm_ptr->header.cmd_id      = RABMAS_RAB_ESTABLISH_IND;
        rabm_ptr->header.message_set = (uint16)MS_RABM_RRC;
        rabm_ptr->cmd.rrc_establish_ind.rabid = 
        (rabid_T)(rbrc.rabs.rabs_in_msg[rab_index].rab_id);
#ifdef FEATURE_DUAL_SIM
        rabm_ptr->cmd.rrc_establish_ind.as_id = rrc_get_as_id();
#endif
  
        /* Put the command on RABM queue */
        WRRC_MSG1_HIGH("Sending RAB_ESTABLISH_IND to RABM RABid%d",
                  rbrc.rabs.rabs_in_msg[rab_index].rab_id);
        rabm_put_cmd(rabm_ptr);
      }
      else
      {
        WRRC_MSG0_HIGH("Skip sending RAB_ESTABLISH_IND as PS domain not open");
      }
    }
  }
  /* Indicate rabm whether or not the rab info to modify is present in the
  message */
  if (rrc_check_rabs_associated_with_cn_domain(RRC_PS_DOMAIN_CN_ID))
  {
    for (rab_index = 0; rab_index< MAX_RAB_TO_SETUP; rab_index++)
    {
      if ( (rrc_est_rabs.rabs[rab_index].cn_domain == RRC_PS_DOMAIN_CN_ID) &&
           (rrc_est_rabs.rabs[rab_index].num_rbs_for_rab > 0) )
      {
        /* found a valid RAB for PS domain. Now check whether any of the RB of this RAB were 
        in the rb info reconfig list or rb info affected list.*/
        for (rb_index=0; rb_index<rrc_est_rabs.rabs[rab_index].num_rbs_for_rab; rb_index++)
        {
          if (rbrc.rbs_tobe_reconfigured.num_rbs>0)
          {
            for (local_rb_index =0; local_rb_index<rbrc.rbs_tobe_reconfigured.num_rbs; local_rb_index++)
            {              
              if (rbrc.rbs_tobe_reconfigured.rb_id[local_rb_index] == 
                  rrc_est_rabs.rabs[rab_index].rb_for_rab[rb_index].rb_id)
              {
                rab_search = TRUE;
                break;
              }
            }
            
          }       
          if (!rab_search)
          {
            if (rbrc.rbs_tobe_affected.num_rbs>0)
            {
              for (local_rb_index=0; local_rb_index<rbrc.rbs_tobe_affected.num_rbs; local_rb_index++)
              {              
                if (rbrc.rbs_tobe_affected.rb_id[local_rb_index] == 
                    rrc_est_rabs.rabs[rab_index].rb_for_rab[rb_index].rb_id)
                {
                  rab_search = TRUE;
                  break;
                }
              }
            }       
          }
          /* if any one RB matches with the RBs associated with RAB then no need to
          continue looking for other RBs. break from here otherwise continue */
          if (rab_search)
          {
            break;
          }
        } /*for rb_index loop */
      }/* if ps_domain match found in established rab */
      /* if rab is found then send the indication to RABM
      and continue with remaining RABs */
      if (rab_search == TRUE)
      {
        if(rrcscrr_is_cn_domain_open(RRC_PS_DOMAIN_CN_ID) == TRUE)
        {
          
          /*there is a PS domain RAB so send a notification to RABM */
          if ((rabm_ptr = rabm_get_cmd_buf()) == NULL)
          {
            ERR_FATAL("Can't send RABM cmd - out of memory!", 0, 0, 0);
          }
          rabm_ptr->header.cmd_id      = RABMAS_RAB_ESTABLISH_IND;
          rabm_ptr->header.message_set = MS_RABM_RRC;
          rabm_ptr->cmd.rrc_establish_ind.rabid = (rabid_T)(rrc_est_rabs.rabs[rab_index].rab_id); 
#ifdef FEATURE_DUAL_SIM
          rabm_ptr->cmd.rrc_establish_ind.as_id = rrc_get_as_id();
#endif
          /* Put the command on RABM queue */
          WRRC_MSG1_HIGH("Send RAB_ESTABLISH_IND RABid:%d",
                    rrc_est_rabs.rabs[rab_index].rab_id);
          rabm_put_cmd(rabm_ptr);
        }
        else
        {
          WRRC_MSG0_HIGH("Skip sending RAB_ESTABLISH_IND as PS domain not open");
        }

        rab_search=FALSE;
      }
    }/*end of for*/
  }/*if there is any ps rab */

#ifdef FEATURE_DUAL_SIM
  rrc_check_send_change_priority();
#endif
} /*rrcrbrc_send_indication_to_upper_layers */

#ifdef FEATURE_VOC_AMR_WB
/*===========================================================================

FUNCTION   rrcrbrc_update_nas_sync_ind

DESCRIPTION Updates the NAS Syncronisation Indicator field received in the RB Reconfig message.
            In case the NAS Syncronisation Indicator IE is not present , then a default value of 
            current active  codec is used.

DEPENDENCIES
  None.

RETURN VALUE
 None

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL uecomdef_status_e_type rrcrbrc_update_nas_sync_ind
(
   uint32 rab_index,
   rrc_NAS_Synchronisation_Indicator *nas_Synchronisation_Indicator 
)
{
  rbrc.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator = 
  rrc_translate_nas_sync_indicator(nas_Synchronisation_Indicator); 
   
  if( rbrc.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator == MVS_MODE_NONE)
  {
    rbrc.status.failure_status= RRCRB_MSG_UNSUPPORTED_CONFIG;
    WRRC_MSG1_HIGH("Unsupported Codec Configuration %d",
        rbrc.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator);

    return FAILURE;
  }
#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
  rrc_set_ordered_codec_status(TRUE);
#endif
  rrc_set_ordered_codec_mode(rbrc.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator);
  return SUCCESS;
}
#endif /*FEATURE_VOC_AMR_WB*/

 /*===========================================================================

FUNCTION rrcrbrc_is_pcpich_info_present

DESCRIPTION
  This function checks whether Primary CPICH info is present in
  the message or not.

DEPENDENCIES
  None.

RETURN VALUE
Boolean. TRUE  : Primary CPICH info is present in the message
                 FALSE : Primary CPICH info is not present in the message
===========================================================================*/
static boolean  rrcrbrc_is_pcpich_info_present
(
  rrc_RadioBearerReconfiguration  *msg_ptr,
  rrc_msg_ver_enum_type            msg_version
)
{
  boolean result = FALSE;
  uint16 rrcrbrc_psc = 512;

#ifdef FEATURE_WCDMA_REL10
  #error code not present
#endif /* FEATURE_WCDMA_REL10 */

#ifdef FEATURE_WCDMA_REL9
  if (msg_version == MSG_REL9)
  {
    rrc_RadioBearerReconfiguration_r9_IEs *rbrc_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.
      criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.radioBearerReconfiguration_r9;  
  
    if(RRCRB_R9_MSG_IE_PRESENT(rbrc_ptr,dl_InformationPerRL_List))
    {
      if(rbrc_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
        T_rrc_DL_InformationPerRL_r8_modeSpecificInfo_fdd)
      {
        rrcrbrc_psc = msg_ptr->u.later_than_r3.criticalExtensions.u.
          criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.radioBearerReconfiguration_r9.
          dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
  
        result = TRUE;
      }
    }
  }
  else
#endif /* FEATURE_WCDMA_REL9 */
  if (msg_version == MSG_REL8)
  {
    rrc_RadioBearerReconfiguration_r8_IEs *rbrc_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.
      criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.radioBearerReconfiguration_r8;  

    if(RRCRB_R8_MSG_IE_PRESENT(rbrc_ptr,rrc_RadioBearerReconfiguration_r8_IEs,dl_InformationPerRL_List))
    {
      if(rbrc_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
        T_rrc_DL_InformationPerRL_r8_modeSpecificInfo_fdd)
      {
        rrcrbrc_psc = msg_ptr->u.later_than_r3.criticalExtensions.u.
          criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.radioBearerReconfiguration_r8.
          dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;

        result = TRUE;
      }
    }
  }
  else
  if (msg_version == MSG_REL7)
  {
    if ((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
         criticalExtensions.u.r7.radioBearerReconfiguration_r7.m.dl_InformationPerRL_ListPresent) &&
         (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
         criticalExtensions.u.r7.radioBearerReconfiguration_r7.dl_InformationPerRL_List.
         elem[0].modeSpecificInfo.t == 
         T_rrc_DL_InformationPerRL_r7_modeSpecificInfo2_fdd)
       )
    {
      rrcrbrc_psc = msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
               criticalExtensions.u.r7.radioBearerReconfiguration_r7.dl_InformationPerRL_List.
               elem[0].modeSpecificInfo.u
               .fdd->primaryCPICH_Info.primaryScramblingCode;
      result = TRUE;
    }
  }
  else

  if (msg_version == MSG_REL6)
  {
    if ((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
         criticalExtensions.u.r6.radioBearerReconfiguration_r6.m.dl_InformationPerRL_ListPresent) &&
         (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
         criticalExtensions.u.r6.radioBearerReconfiguration_r6.dl_InformationPerRL_List.
         elem[0].modeSpecificInfo.t == 
         T_rrc_DL_InformationPerRL_r6_modeSpecificInfo_fdd)
       )
    {
      rrcrbrc_psc = msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
               criticalExtensions.u.r6.radioBearerReconfiguration_r6.dl_InformationPerRL_List.
               elem[0].modeSpecificInfo.u
               .fdd->primaryCPICH_Info.primaryScramblingCode;
      result = TRUE;
    }
  }
  else
  /* this function is used to find whether directed cell info is present
     or not during Cell_DCH to Cell_FACH state transition. It is safe to 
     assume that the rl list will have only one element.
   */
  if (msg_version == MSG_REL5)
  {
    if ((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
         r5.radioBearerReconfiguration_r5.m.dl_InformationPerRL_ListPresent) &&
        (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
         r5.radioBearerReconfiguration_r5.dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
          T_rrc_DL_InformationPerRL_r5_modeSpecificInfo_fdd)
       )
    {
      rrcrbrc_psc = msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
               r5.radioBearerReconfiguration_r5.dl_InformationPerRL_List.elem[0].modeSpecificInfo.u
               .fdd->primaryCPICH_Info.primaryScramblingCode;
      result = TRUE;
    }
  }
  else
  {
    if (msg_ptr->u.r3.radioBearerReconfiguration_r3.dl_InformationPerRL_List.elem[0].modeSpecificInfo.t ==
        T_rrc_DL_InformationPerRL_modeSpecificInfo_fdd)
    {
      rrcrbrc_psc = msg_ptr->u.r3.radioBearerReconfiguration_r3.dl_InformationPerRL_List.
               elem[0].modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
      result = TRUE;
    }
  }

  MSG_HIGH(" RRCRBRECONFIG: Received PSC: %d result:%d for msg_version:%d(0-MSG_REL99,1-MSG_REL5,2-MSG_REL6,3-MSG_REL7,4-MSG_REL8,5-MSG_REL9,6-MSG_REL10,other-MSG_INVALID) ",rrcrbrc_psc,result,msg_version);
  return result;
}  /* rrcrbrc_is_pcpich_info_present */


/*===========================================================================

FUNCTION   VALIDATE_RB_ID

DESCRIPTION

  This function validates the rb_ids in the received RB Reconfig Message.
  If signalling RB id is present it returns RRC_RB_PRESENT. For user plane RB ids
  it checks whether the rb to be reconfigured or affected is present in the 
  established rbs. If not this function returns RRC_RB_NOT_PRESENT.

DEPENDENCIES

  None.
 
RETURN VALUE

  rrc_rb_search_e_type 
          RRC_RB_PRESENT     :rb-id exists in the ESTABLISHED_RAB variable or
                              it is a signalling RB.
          RRC_RB_NOT_PRESENT :rb-id is not a signalling RB and it also
                              does not exist in the ESTABLISHED_RAB variable.

SIDE EFFECTS

  None.

===========================================================================*/
static rrc_rb_search_e_type validate_rb_id(rrc_RB_Identity rb_id)
{
  rrc_rb_search_e_type rb_srch = RRC_RB_NOT_PRESENT;
  /*check whether it is signalling rbid */
  /*Do not include rb-id0 in the check because it is for CCCH */
  if ((rb_id > CCCH_RADIO_BEARER_ID) && (rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID))
  {
    rb_srch = RRC_RB_PRESENT;
  }
  else
  {
    if(rrcllc_has_config_srb5(CURRENT_CONFIG, rb_id) == TRUE)
    {
      rb_srch = RRC_RB_PRESENT;
    }
    else
    {

      /*it must be user plane rb. Check whether this RB exists in established_rab db */
      rb_srch = rrc_find_rb_in_est_rabs (rb_id);
    }
  }
  
  return rb_srch;
}

/*===========================================================================

FUNCTION   RRCRBRC_VALIDATE_SRNS_RELOCATION_INFO()

DESCRIPTION

  This function checks whether SRNS relocation info is present or not
  and also checks whether the current state and state indicated by the message
  are both Cell_DCH or not 
        
DEPENDENCIES

  None.
 
RETURN VALUE

  Boolean. TRUE : SRNS relocation info is present
           FALSE : SRNS relocation info is not present

SIDE EFFECTS

  None.

===========================================================================*/
static boolean rrcrbrc_validate_srns_relocation_info
(
  rrc_RadioBearerReconfiguration *msg_ptr, /* Pointer to the RB RC message */
  rrc_msg_ver_enum_type msg_version
)
{
  boolean result = FALSE;

  if (msg_version == MSG_REL5)
  {
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.
         u.r5.radioBearerReconfiguration_r5,rrc_RadioBearerReconfiguration_r5_IEs,new_U_RNTI))
    {
      result = TRUE;
    }
  }
  else if (msg_version == MSG_REL6)
  {
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
         criticalExtensions.u.r6.radioBearerReconfiguration_r6,rrc_RadioBearerReconfiguration_r6_IEs,
         new_U_RNTI))
    { 
      result = TRUE; 
    }
  }
  else if (msg_version == MSG_REL7)
  {
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
         criticalExtensions.u.r7.radioBearerReconfiguration_r7,
         rrc_RadioBearerReconfiguration_r7_IEs,new_U_RNTI))
    { 
      result = TRUE; 
    }
  }
/*RRC_GKG: Find out why RBE and RBR dedices based on DL Sync Info but RBRC dedices based on URNTI*/
  else if (msg_version == MSG_REL8)
  {
    if(RRCRB_R8_MSG_IE_PRESENT((&msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
        criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.radioBearerReconfiguration_r8),
        rrc_RadioBearerReconfiguration_r8_IEs, new_U_RNTI))
    { 
      result = TRUE; 
    }
  }
#ifdef FEATURE_WCDMA_REL9
/*RRC_GKG: Find out why RBE and RBR dedices based on DL Sync Info but RBRC dedices based on URNTI*/
  else if (msg_version == MSG_REL9)
  {
    if(RRCRB_R9_MSG_IE_PRESENT((&msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
        criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.radioBearerReconfiguration_r9),
         new_U_RNTI))
    { 
      result = TRUE; 
    }
  }

#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
  #error code not present
#endif /* FEATURE_WCDMA_REL10 */


  else
  {
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerReconfiguration_r3,
          rrc_RadioBearerReconfiguration_r3_IEs,new_U_RNTI))
    {
      /*check whether the SRNS relocation is within Cell_DCH or not */
      result = TRUE;
    }
  }

  return result; 
}

/*===========================================================================

FUNCTION   VALIDATE_RBRC_MESSAGE

DESCRIPTION

  This function validates the received RB Reconfig Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a Radio Bearer Reconfig Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_rbrc_message
(
rrc_RadioBearerReconfiguration *msg_ptr /* Pointer to the RB RC message */
)
{
    rrc_rab_id_type rab_id;           /* To store RAB id in internal RRC format */
    rrcmn_rab_status_T rab_type;      /* To store the status returned by MN layer. This
                                    indicates what type of RAB is received. This is 
                                    used only if the RAB is for the CS_domain */
  rrc_rab_search_e_type rab_status; /* To check if RAB is present in ESTABLISHED_RABS */
  uint32 rab_index=0;               /* Used as an index when storing established
                                    RABs locally. Only when we have more than
                                    one RAB per message, this will be incremented */
#ifdef FEATURE_UMTS_PDCP
  rrc_RB_InformationReconfig *rb_info_ptr = NULL;
  rrc_HeaderCompressionInfo *pdcpHeaderCompressionInfo = NULL;  
#endif /* FEATURE_UMTS_PDCP*/
#ifdef FEATURE_VOC_AMR_WB
  rrc_RAB_InformationReconfigList *rab_list_ptr = NULL;
  mvs_mode_type nas_sync_ind=rrc_get_current_codec_mode();
#endif/*FEATURE_VOC_AMR_WB*/
  /* Assume the message is valid for now */
  rbrc.status.failure_status = RRCRB_MSG_VALID;

  if ( msg_ptr->t == 
       T_rrc_RadioBearerReconfiguration_later_than_r3 )
  {
    /* Critical extensions are present that are not supported by this
    release. We have to send a Radio Bearer Reconfig Failure message. */
    WRRC_MSG0_HIGH("Critical exstension present in RB RC msg!");
    rbrc.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
    rbrc.status.prot_err_cause = rrc_ProtocolErrorCause_messageExtensionNotComprehended;
    return;
  }


  /* Check the RRC State indicatior given by the message */
  switch ( msg_ptr->u.r3.radioBearerReconfiguration_r3.rrc_StateIndicator )
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      rbrc.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      rbrc.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

     case rrc_RRC_StateIndicator_cell_PCH:
      rbrc.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;

    case rrc_RRC_StateIndicator_ura_PCH:
      rbrc.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;

    default:
      /* No other states are supported right now. */
      WRRC_MSG1_HIGH("Unsupported state %d in RB Reconfig Message", 
               msg_ptr->u.r3.radioBearerReconfiguration_r3.rrc_StateIndicator);
      rbrc.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
      break;
  }
  if (rbrc.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG)
  {
    return;
  }

  if ( msg_ptr->u.r3.radioBearerReconfiguration_r3.rb_InformationReconfigList.n ==
       0 )
  {
    /* Validate the rb_InformationReconfigList here.This is a mandatory field, so no
    bit mask for it*/
    WRRC_MSG0_HIGH("RBReconfiglist missing in RBRC msg");
    rbrc.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
    rbrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
    return;
  }

  /* Check if RB Information Affected list is present. This
  is not supported in the first release. This may be supported
  later */
  if ( RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerReconfiguration_r3,
        rrc_RadioBearerReconfiguration_r3_IEs,rb_InformationAffectedList)
     )
  {
    MSG_LOW("RB Info Affected list present in RB RC Message",0,0,0);
  }

  if ( RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerReconfiguration_r3,
        rrc_RadioBearerReconfiguration_r3_IEs,new_U_RNTI)
     )
  {
    /*check whether the SRNS relocation is within Cell_DCH or not.
     SRNS relocation information can come in reconfiguration messages
     only when the state transition is Cell_DCH->Cell_DCH */
    if (!((rbrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
     WRRC_MSG2_ERROR("SRNS reloc with invalid states cur_state %d, msg_state %d",
             rrc_get_state(), rbrc.state_indicated_by_msg);
      rbrc.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    /* message with valid SNRS relocation info */
    else
    {
      /*set the flag that indicates that this procedure
      involves SRNS relocation */
      rbrc.rbrc_for_srns_relocation = TRUE;
      /*store the new U-RNTI */
      rrc_translate_urnti (&(msg_ptr->u.r3.radioBearerReconfiguration_r3.new_U_RNTI),
                            &rbrc.new_urnti);
    }

  }
  
 
  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  rbrc.trans_to_dch_ciph_config_needed = FALSE;
  if((rbrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    //check to see if dl-dpch-common info is present while going to dch
    if((RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerReconfiguration_r3,
        rrc_RadioBearerReconfiguration_r3_IEs,dl_CommonInformation)) && 
       (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerReconfiguration_r3.dl_CommonInformation,
        rrc_DL_CommonInformation,dl_DPCH_InfoCommon)))
    {
      //check if ciphering is active in mac for cs domain, if it is then,
      //start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
      if(rrcsmc_tm_ciph_active() == TRUE)
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 config activated");
        if(rrcsmc_process_cipher_config_for_hho(RRCSMC_INVALID_HFN_VALUE) == FAILURE)
        {
          WRRC_MSG0_HIGH("Processing Ciphering Config for HHO failed");
        }
        rbrc.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }
/*The processing of RAB Info Reconfig is done differently based on feature WB-AMR.
When feature FEATURE_VOC_AMR_WB is enabled, we process all the RABs but in case of
no feature, we process only the first RAB. Right now, there's in fact no need to process more
than one RABs in the same OTA.*/
/*********************************/    
#ifdef FEATURE_VOC_AMR_WB    
/*********************************/  
  if ( RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerReconfiguration_r3,
       rrc_RadioBearerReconfiguration_r3_IEs,rab_InformationReconfigList)
     )
  {
    MSG_LOW("RAB Info Reconfig in RB RC Message", 0, 0, 0);
      
    if (msg_ptr->u.r3.radioBearerReconfiguration_r3.rab_InformationReconfigList.n != 0)
    {
      rab_list_ptr = &msg_ptr->u.r3.radioBearerReconfiguration_r3.rab_InformationReconfigList;   
     
      for (rab_index=0; (rab_list_ptr->n > rab_index) && (rab_index < RRCRB_MAX_RAB_PER_MSG); rab_index++)
      {
        /* Check if the RAB id is GSM_MAP type. We don't support any
            other type */
        if ( rab_list_ptr->elem[rab_index].rab_Identity.t != T_rrc_RAB_Identity_gsm_MAP_RAB_Identity  )
        {
          WRRC_MSG1_HIGH("RABid t%d is not GSM_MAP!",
                   rab_list_ptr->elem[rab_index].rab_Identity.t);
          rbrc.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
          return;
        }
    
        /* GSM_MAP RAB Id. Check if it's valid by calling the
        appropriate NAS entity. First convert it to internal
        format. */
        rab_id = rrcrb_translate_gsm_map_rab_id(
                                               (rab_list_ptr->elem[rab_index].rab_Identity.u.gsm_MAP_RAB_Identity)
                                               );
    
        /* Check if the RAB id is already in ESTABLISHED_RABS */
        rab_status = rrc_find_rab_in_established_rabs (rab_id);
        if ( rab_status != RRC_RAB_FOUND )
        {
          WRRC_MSG1_HIGH("RAB ID 0X%x not in ESTABLISHED_RABS", rab_id);
          rbrc.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
          return;
        }
    
        /* Store this info locally so that ESTABLISHED_RABS can
        be updated later, when these RABs are established */
        rbrc.rabs.rabs_in_msg[rab_index].rab_id_is_gsm_map = TRUE;
        rbrc.rabs.rabs_in_msg[rab_index].rab_id = rab_id;
    
         /* Check if this RAB is CS Domain or PS Domain */
        if ( rab_list_ptr->elem[rab_index].cn_DomainIdentity  == rrc_CN_DomainIdentity_cs_domain )
        {
          /* Store the domain identity. This will be later added to
          ESTABLISHED_RABS */
          rbrc.rabs.rabs_in_msg[rab_index].cn_domain = RRC_CS_DOMAIN_CN_ID;
    
          /* Check if it is in loopback test */
          if ( rrclbt_is_lb_on() == TRUE )
          {
            /* Set Rab type to Data */
            MSG_LOW("Loopback Test", 0, 0, 0);
            rbrc.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_DATA_RAB;
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
              rrc_get_rab_type_for_given_rab_id(rbrc.rabs.rabs_in_msg[rab_index].rab_id));
    
            switch ( rab_type )
            {
              case RRCMN_CS_DATA_CALL:
                rbrc.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_DATA_RAB;
                break;
    
              case RRCMN_CS_VOICE_CALL:
                rbrc.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_VOICE_RAB;

#ifdef FEATURE_SCUDIF_ENHANCEMENT
                 #error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/

                break;
    
              default:
                WRRC_MSG0_ERROR("CS RAB validation failed!");
                rbrc.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
                break;
    
            } /* end switch */

            /*As a general approach we should use/update the NAS Sync Indicator even for CS DATA*/
            if((rbrc.rabs.rabs_in_msg[rab_index].rab_type == RRCRB_CS_VOICE_RAB)
#ifdef FEATURE_SCUDIF_ENHANCEMENT
#error code not present
#endif
)
            {
              if(rrcrbrc_update_nas_sync_ind(rab_index,&(rab_list_ptr->elem[rab_index].nas_Synchronisation_Indicator))
                  == FAILURE)
              {
                return;
              }
              nas_sync_ind = rbrc.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator;              
            }
          }
    
          if ( (rbrc.status.failure_status == RRCRB_MSG_INVALID_CONFIG) ||
               (rbrc.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG)
             )
          {
            /* No need to continue */
            return;
          }
        }
        /* Else check for PS Domain */
        else if ( rab_list_ptr->elem[rab_index].cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain )
        {
          /* Store the domain identity. This will be later added to
            ESTABLISHED_RABS */
          rbrc.rabs.rabs_in_msg[rab_index].cn_domain = RRC_PS_DOMAIN_CN_ID;
          WRRC_MSG1_HIGH("PS RABId %d to be setup", rab_id);
            rbrc.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_PS_DATA_RAB;
        }   
      }     
    }
  }/* if rrc_RadioBearerReconfiguration_r3_IEs_rab_InformationReconfigList_present is present*/
  rbrc.rabs.num_rabs_in_msg = rab_index;
  rrc_set_ordered_codec_mode(nas_sync_ind);

/*********************************/   
#else/*FEATURE_VOC_AMR_WB*/
/*********************************/ 

  if ( RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerReconfiguration_r3,
       rrc_RadioBearerReconfiguration_r3_IEs,rab_InformationReconfigList)
     )
  {
    MSG_LOW("RAB Info Reconfig in RB RC Message", 0, 0, 0);
    // nas sync variable present. need any validation ?
    /* Check if the RAB id is GSM_MAP type. We don't support any
     other type */
    if ( msg_ptr->u.r3.radioBearerReconfiguration_r3.rab_InformationReconfigList
         .elem[0].rab_Identity.t != T_rrc_RAB_Identity_gsm_MAP_RAB_Identity
       )
    {
      WRRC_MSG1_HIGH("RABid t%d is not GSM_MAP!",
               msg_ptr->u.r3.radioBearerReconfiguration_r3.rab_InformationReconfigList.elem[0].rab_Identity.t);
      rbrc.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
      return;
    }
   
    /* GSM_MAP RAB Id. Check if it's valid by calling the
    appropriate NAS entity. First convert it to internal
    format. */
    rab_id = rrcrb_translate_gsm_map_rab_id(
                                           (msg_ptr->u.r3.radioBearerReconfiguration_r3.rab_InformationReconfigList
                                             .elem[0].rab_Identity.u.gsm_MAP_RAB_Identity)
                                           );
   
    /* Check if the RAB id is already in ESTABLISHED_RABS */
    rab_status = rrc_find_rab_in_established_rabs (rab_id);
    if ( rab_status != RRC_RAB_FOUND )
    {
      WRRC_MSG1_HIGH("RAB ID 0X%x not in ESTABLISHED_RABS", rab_id);
      rbrc.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  
    /* Store this info locally so that ESTABLISHED_RABS can
    be updated later, when these RABs are established */
    rbrc.rabs.rabs_in_msg[rab_index].rab_id_is_gsm_map = TRUE;
    rbrc.rabs.rabs_in_msg[rab_index].rab_id = rab_id;
  
    /* Check if this RAB is CS Domain or PS Domain */
    if ( msg_ptr->u.r3.radioBearerReconfiguration_r3.rab_InformationReconfigList
         .elem[0].cn_DomainIdentity  == rrc_CN_DomainIdentity_cs_domain )
    {
      /* Store the domain identity. This will be later added to
      ESTABLISHED_RABS */
      rbrc.rabs.rabs_in_msg[rab_index].cn_domain = RRC_CS_DOMAIN_CN_ID;
  
      /* Check if it is in loopback test */
      if ( rrclbt_is_lb_on() == TRUE )
      {
        /* Set Rab type to Data */
        MSG_LOW("Loopback Test", 0, 0, 0);
        rbrc.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_DATA_RAB;
      }
      else
      {
        /* For CS RABs, Call MN layer to verify this RAB */
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
        rab_type = rrcmn_verify_rab_subs( (radio_access_bearer_T)rab_id,rrc_get_as_id() );
#else
        rab_type = rrcmn_verify_rab( (radio_access_bearer_T)rab_id );
#endif
        MSG_HIGH("For RAB-Id %d, RAB type from NAS = %d, RAB type from EST_RABs = %d",rab_id, rab_type,
          rrc_get_rab_type_for_given_rab_id(rbrc.rabs.rabs_in_msg[rab_index].rab_id));
  
        switch ( rab_type )
        {
          case RRCMN_CS_DATA_CALL:
            rbrc.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_DATA_RAB;
            break;
  
          case RRCMN_CS_VOICE_CALL:
            rbrc.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_VOICE_RAB;
#ifdef FEATURE_SCUDIF_ENHANCEMENT
            #error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/

            break;
  
          default:
            WRRC_MSG0_ERROR("CS RAB validation failed!");
            rbrc.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
            break;
  
        } /* end switch */
      }
  
      if ( (rbrc.status.failure_status == RRCRB_MSG_INVALID_CONFIG) ||
           (rbrc.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG)
         )
      {
      /* No need to continue */
        return;
      }
      /*Since we handle only one RAB here, just hard code it to 1.*/
      rbrc.rabs.num_rabs_in_msg = 1;   
    }
    /* Else check for PS Domain */
    else if ( msg_ptr->u.r3.radioBearerReconfiguration_r3.rab_InformationReconfigList
              .elem[0].cn_DomainIdentity == RRC_PS_DOMAIN_CN_ID )
    {
      /* Store the domain identity. This will be later added to
        ESTABLISHED_RABS */
      rbrc.rabs.rabs_in_msg[rab_index].cn_domain = RRC_PS_DOMAIN_CN_ID;
      WRRC_MSG1_HIGH("PS RABId %d to be setup", rab_id);
        rbrc.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_PS_DATA_RAB;
      /*Since we handle only one RAB here, just hard code it to 1.*/
      rbrc.rabs.num_rabs_in_msg = 1; 
    }
  }/* if rrc_RadioBearerReconfiguration_r3_IEs_rab_InformationReconfigList_present is present*/

/*********************************/    
#endif /*FEATURE_VOC_AMR_WB*/
/*********************************/  
 
#ifdef FEATURE_UMTS_PDCP
 /* Validate all PDCP info if present */
  if (rrc_pdcp_enabled) 
  {
    if (msg_ptr->u.r3.radioBearerReconfiguration_r3.rb_InformationReconfigList.n != 0)
    {
        
      uint32 local_rb_idx =0;
      while (msg_ptr->u.r3.radioBearerReconfiguration_r3.rb_InformationReconfigList.n > local_rb_idx) 
      {
        rb_info_ptr = &msg_ptr->u.r3.radioBearerReconfiguration_r3.rb_InformationReconfigList.elem[local_rb_idx]; 
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_ptr,rrc_RB_InformationReconfig,pdcp_SN_Info))
        {
          WRRC_MSG0_HIGH("PDCP SN info not supported");
          rbrc.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
          return; 
        }
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_ptr,rrc_RB_InformationReconfig,pdcp_Info))
        {
          if (rrc_get_rab_type_for_given_rb_id(rb_info_ptr->rb_Identity) != RRCRB_PS_DATA_RAB)
          {
            WRRC_MSG1_HIGH("NW incld PDCP info for non-ps rb id %d: Invalid",
                     rb_info_ptr->rb_Identity);
            rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
            return;
          }
          
          /* check whether lossless SRNS relocation flag is set or   
           * and header compression algo is present in the message 
           */
          if ((RRC_MSG_COMMON_BITMASK_IE(rb_info_ptr->pdcp_Info.pdcp_Info,rrc_PDCP_Info,losslessSRNS_RelocSupport)) && 
              (RRC_CHECK_COMMON_MSG_TYPE(rb_info_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport,rrc_LosslessSRNS_RelocSupport_supported)))
          {
            if (rb_info_ptr->pdcp_Info.pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_absent)
            {
              WRRC_MSG0_HIGH("PDCP pdu header is absent.  Invalid config");
              rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
              return;
            }   
          } /* if rrc_PDCP_Info_r4_losslessSRNS_RelocSupport_present */

          if (rb_info_ptr->pdcp_Info.pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) 
          {
            if ((RRC_MSG_COMMON_BITMASK_IE(rb_info_ptr->pdcp_Info.pdcp_Info,rrc_PDCP_Info,losslessSRNS_RelocSupport)) && 
                (RRC_CHECK_COMMON_MSG_TYPE(rb_info_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport,rrc_LosslessSRNS_RelocSupport_notSupported)))
            {
              if (!(RRC_MSG_COMMON_BITMASK_IE(rb_info_ptr->pdcp_Info.pdcp_Info,rrc_PDCP_Info,headerCompressionInfoList)) 
                  ) 
              {
                WRRC_MSG0_HIGH("PDCP header compress info list not present");
                rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                return;
              }
            }
          }
          
          if (RRC_MSG_COMMON_BITMASK_IE(rb_info_ptr->pdcp_Info.pdcp_Info,rrc_PDCP_Info,headerCompressionInfoList)) 
          {
            uint32 local_idx=0;
            while (rb_info_ptr->pdcp_Info.pdcp_Info.headerCompressionInfoList.n > local_idx) 
            {
              pdcpHeaderCompressionInfo = &rb_info_ptr->pdcp_Info.pdcp_Info.headerCompressionInfoList.elem[local_idx];
              if (RRC_CHECK_COMMON_MSG_TYPE(pdcpHeaderCompressionInfo->algorithmSpecificInfo,
                  rrc_AlgorithmSpecificInfo_rfc2507_Info))
              {
				        /* if the UE capability "Maximum header compression context space", is exceeded with this configuration send
				         * failure with cause "Invalid Config"
                 */
                if ( 
                    (pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER > IPHC_MAX_HC_CONTEXT_SPACE))
                {
                  WRRC_MSG1_HIGH("Max header comprss context space exceeded : %d",
                           pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER);
                  rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
                  return;
                }
              }
              local_idx++;
            } /*  while (pdcpHeaderCompressionInfoList) */
          } /*  if (rb_info_ptr->pdcp_Info.pdcp_Info.bit_mask & rrc_PDCP_Info_r4_headerCompressionInfoList_present) */

        } /* if (rb_info_ptr->bit_mask & rrc_RB_InformationReconfig_r5_pdcp_Info_present) */
        local_rb_idx++;
      } /* while (rb_list_ptr) */
    } /* if (rbrc_ptr->specificationMode.u.complete.bit_mask & rrc_complete_rb_InformationReconfigList_present) */
  }
#endif /* FEATURE_UMTS_PDCP */

  /*Cell_PCH/URA_PCH specific validations */
  if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
     /* If UTRAN DRX coefficient is not present then it is invalid configuration */
     if (!(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerReconfiguration_r3,
         rrc_RadioBearerReconfiguration_r3_IEs,utran_DRX_CycleLengthCoeff)))
     {
       WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", rbrc.state_indicated_by_msg);
       rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
       return;
     }
     if (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
     {
       /*should we do it after getting l2ack for response??*/
       if (!(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerReconfiguration_r3,
             rrc_RadioBearerReconfiguration_r3_IEs,ura_Identity)))
           
       {
         WRRC_MSG0_HIGH("URA ID not present");
         rrccu_clear_ura_id ();
       }
       else
       {
         /*URA ID present in the message set ura_identity variable with this value */
         if (msg_ptr->u.r3.radioBearerReconfiguration_r3.ura_Identity.numbits == 16)
         {
           /*rrccu_translate_ura_id (&(msg_ptr->u.r3.radioBearerReconfiguration_r3.ura_Identity));*/
           rrccu_update_ura_id (&(msg_ptr->u.r3.radioBearerReconfiguration_r3.ura_Identity));
         }
         else
         {
           WRRC_MSG1_ERROR("Invalid URA ID length%d",
               msg_ptr->u.r3.radioBearerReconfiguration_r3.ura_Identity.numbits);
           rbrc.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
           rbrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
           return;
         }

       }
           
     }
  }
}/* end function validate_rbrc_message */


/*===========================================================================

FUNCTION   VALIDATE_RBRC_MESSAGE_R5

DESCRIPTION

  This function validates the received RB Reconfig Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a Radio Bearer Reconfig Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_rbrc_message_r5
(
  rrc_RadioBearerReconfiguration *msg_ptr /* Pointer to the RB RC message */
)
{
  rrc_RadioBearerReconfiguration_r5_IEs *rbrc_ptr = NULL;

  rrc_RAB_InformationReconfigList *rab_list_ptr = NULL;  
#ifdef FEATURE_UMTS_PDCP
  rrc_RB_InformationReconfig_r5 *rb_info_ptr = NULL;
  rrc_HeaderCompressionInfo_r4 *pdcpHeaderCompressionInfo = NULL;  
#endif /* FEATURE_UMTS_PDCP*/
  rrc_rab_id_type rab_id;           /* To store RAB id in internal RRC format */
  rrcmn_rab_status_T rab_type;      /* To store the status returned by MN layer. This
                                    indicates what type of RAB is received. This is 
                                    used only if the RAB is for the CS_domain */
  rrc_rab_search_e_type rab_status; /* To check if RAB is present in ESTABLISHED_RABS */
  uint32 rab_index=0;               /* Used as an index when storing established
                                    RABs locally. Only when we have more than
                                    one RAB per message, this will be incremented */
#ifdef FEATURE_VOC_AMR_WB
  mvs_mode_type nas_sync_ind=rrc_get_current_codec_mode();
#endif/*FEATURE_VOC_AMR_WB*/


  /* Assume the message is valid for now */
  rbrc.status.failure_status = RRCRB_MSG_VALID;

  rbrc_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
             r5.radioBearerReconfiguration_r5;

  /* Check the RRC State indicatior given by the message and 
     store it in the rbrc internal memory
   */
  switch (rbrc_ptr->rrc_StateIndicator)
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      rbrc.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      rbrc.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      rbrc.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;

    case rrc_RRC_StateIndicator_ura_PCH:
      rbrc.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;

    default:
      /* No other states are supported right now. */
      WRRC_MSG1_HIGH("Unsupported state %d in RBRC R5 Msg", rbrc_ptr->rrc_StateIndicator);
      rbrc.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
      break;
  }

  if (rbrc.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG)
  {
    return;
  }

  /* Validate DL HSPDSCH */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r5_IEs,dl_HSPDSCH_Information))
  {
    /* Check if network does not try to initiate HSDPA with invalid state */
    if( rrc_hsdpa_initial_validation(rbrc_ptr->rrc_StateIndicator, 
      RRC_CHECK_COMMON_MSG_TYPE(rbrc_ptr->dl_HSPDSCH_Information.modeSpecificInfo,
      rrc_DL_HSPDSCH_Information_modeSpecificInfo_tdd))
                                                     == FAILURE)  
    {
      rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }

  /* Not support to the pre-config option. */
  if (!(RRC_CHECK_COMMON_MSG_TYPE(rbrc_ptr->specificationMode,
      rrc_RadioBearerReconfiguration_r5_IEs_specificationMode_complete)))
  {
    WRRC_MSG0_HIGH("Non-support preconfig t in RBRC");
    rbrc.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
    return;
  }

  

  /* Obtain U_RNTI if present for SRNS relocation.  */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,
  	rrc_RadioBearerReconfiguration_r5_IEs,new_U_RNTI))
  {
    /* check whether the SRNS relocation is within Cell_DCH or not.
       SRNS relocation information can come in reconfiguration messages
       only when the state transition is Cell_DCH->Cell_DCH */
    if (!((rbrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("SRNS reloc with invalid states cur_state %d, msg_state %d",
          rrc_get_state(), rbrc.state_indicated_by_msg);
      rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    /* message with valid SNRS relocation info */
    else
    {
      /* Set the flag that indicates that this procedure
         involves SRNS relocation */
      rbrc.rbrc_for_srns_relocation = TRUE;
      /*store the new U-RNTI */
      rrc_translate_urnti (&(rbrc_ptr->new_U_RNTI), &rbrc.new_urnti);
    }
  }

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  rbrc.trans_to_dch_ciph_config_needed = FALSE;
  if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    /* check to see if dl-dpch-common info is present while going to dch */
    if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r5_IEs,dl_CommonInformation)) &&
        (RRC_MSG_COMMON_BITMASK_IE(rbrc_ptr->dl_CommonInformation,
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
             (RRC_MSG_COMMON_BITMASK_IE(rbrc_ptr->dl_CommonInformation.dl_DPCH_InfoCommon,
             rrc_DL_DPCH_InfoCommon_r4,mac_d_HFN_initial_value)),
             &rbrc_ptr->dl_CommonInformation.dl_DPCH_InfoCommon.mac_d_HFN_initial_value,
             &mac_d_hfn_l);
        if(loc_ret_val == FAILURE)
        {
          WRRC_MSG0_HIGH("get_mac_d_hfn failed");
        }	 
        if (rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
        {
          WRRC_MSG0_HIGH("Processing Ciphering Config for HHO failed");
        }
        rbrc.trans_to_dch_ciph_config_needed = TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
      }
    }
  }

  /*Here we process all the RABs in RAB Info Reconfig List*/    
  rbrc.rabs.num_rabs_in_msg =0;
  if (rbrc_ptr->specificationMode.u.complete.m.rab_InformationReconfigListPresent)
  {
    MSG_LOW("RAB Info Reconfig in R5 RBRC Msg", 0, 0, 0);

    rab_list_ptr = &rbrc_ptr->specificationMode.u.complete.rab_InformationReconfigList;

    for (rab_index = 0; rab_list_ptr->n > rab_index && rab_index < RRCRB_MAX_RAB_PER_MSG; rab_index++)
    {
      /* Check if the RAB id is GSM_MAP type. We don't support any
         other type */
      if (rab_list_ptr->elem[rab_index].rab_Identity.t != T_rrc_RAB_Identity_gsm_MAP_RAB_Identity)
      {
        WRRC_MSG1_HIGH("RABid t%d is not GSM_MAP!", rab_list_ptr->elem[rab_index].rab_Identity.t);
        rbrc.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
        return;
      }

      /* GSM_MAP RAB Id. Check if it's valid by calling the appropriate NAS entity. 
         First convert it to internal format. */
      rab_id = rrcrb_translate_gsm_map_rab_id((rab_list_ptr->elem[rab_index].rab_Identity.u.gsm_MAP_RAB_Identity));

      /* Check if the RAB id is already in ESTABLISHED_RABS */
      rab_status = rrc_find_rab_in_established_rabs(rab_id);
      if ( rab_status != RRC_RAB_FOUND )
      {
        WRRC_MSG1_HIGH("RAB ID 0X%x not in ESTABLISHED_RABS", rab_id);
        rbrc.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
        return;
      }

      /* Store this info locally so that ESTABLISHED_RABS can
         be updated later, when these RABs are established */
      rbrc.rabs.rabs_in_msg[rab_index].rab_id_is_gsm_map = TRUE;
      rbrc.rabs.rabs_in_msg[rab_index].rab_id = rab_id;

      /* Check if this RAB is CS Domain or PS Domain */
      if ( rab_list_ptr->elem[rab_index].cn_DomainIdentity  == rrc_CN_DomainIdentity_cs_domain )
      {
        /* Store the domain identity. This will be later added to
        ESTABLISHED_RABS */
        rbrc.rabs.rabs_in_msg[rab_index].cn_domain = RRC_CS_DOMAIN_CN_ID;

        /* Check if it is in loopback test */
        if ( rrclbt_is_lb_on() == TRUE )
        {
          /* Set Rab type to Data */
          MSG_LOW("Loopback Test", 0, 0, 0);
          rbrc.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_DATA_RAB;
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
            rrc_get_rab_type_for_given_rab_id(rbrc.rabs.rabs_in_msg[rab_index].rab_id));

          switch ( rab_type )
          {
            case RRCMN_CS_DATA_CALL:
              rbrc.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_DATA_RAB;
              break;

            case RRCMN_CS_VOICE_CALL:
              rbrc.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_VOICE_RAB;

#ifdef FEATURE_SCUDIF_ENHANCEMENT
              #error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/
              break;

            default:
              WRRC_MSG0_ERROR("CS RAB validation failed!");
              rbrc.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
              break;

          } /* end switch */
#ifdef FEATURE_VOC_AMR_WB  
          /*As a general approach we should use/update the NAS Sync Indicator even for CS DATA*/
          if((rbrc.rabs.rabs_in_msg[rab_index].rab_type == RRCRB_CS_VOICE_RAB)
    #ifdef FEATURE_SCUDIF_ENHANCEMENT
#error code not present
#endif
)
          {
            if(rrcrbrc_update_nas_sync_ind(rab_index,&(rab_list_ptr->elem[rab_index].nas_Synchronisation_Indicator))
                == FAILURE)
            {
              return;
            }
            nas_sync_ind = rbrc.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator;              
          }
#endif /*FEATURE_VOC_AMR_WB*/   
        }

        if ( (rbrc.status.failure_status == RRCRB_MSG_INVALID_CONFIG) ||
             (rbrc.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG)
           )
        {
          /* No need to continue */
          return;
        }
      }
      /* Else check for PS Domain */
      else if ( rab_list_ptr->elem[rab_index].cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain )
      {
        /* Store the domain identity. This will be later added to
           ESTABLISHED_RABS 
         */
        rbrc.rabs.rabs_in_msg[rab_index].cn_domain = RRC_PS_DOMAIN_CN_ID;
        WRRC_MSG1_HIGH("PS RABId %d to be setup", rab_id);
        rbrc.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_PS_DATA_RAB;
      }
      rab_index++;
      rbrc.rabs.num_rabs_in_msg ++;
    } // for each RAB info
  } //RAB info IE present
#ifdef FEATURE_VOC_AMR_WB
  rrc_set_ordered_codec_mode(nas_sync_ind);
#endif/*FEATURE_VOC_AMR_WB*/
  if (rbrc_ptr->specificationMode.u.complete.m.rb_PDCPContextRelocationListPresent)
  {
    WRRC_MSG0_HIGH("PDCP Context Relocation not supported");
    rbrc.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
    return;
  }

#ifdef FEATURE_UMTS_PDCP
 if (rrc_pdcp_enabled)
 {
   /* Validate all PDCP info if present */
   if (rbrc_ptr->specificationMode.u.complete.m.rb_InformationReconfigListPresent )
   {
     uint32 local_rb_idx =0;
	 
	 while (rbrc_ptr->specificationMode.u.complete.rb_InformationReconfigList.n > local_rb_idx) 
	 {
	   	 rb_info_ptr = &rbrc_ptr->specificationMode.u.complete.rb_InformationReconfigList.elem[local_rb_idx];
	   if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_ptr,rrc_RB_InformationReconfig_r5,pdcp_SN_Info))
	   {
		 WRRC_MSG0_HIGH("PDCP SN info not suported");
		 rbrc.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
		 return;
	   }
	   if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_ptr,rrc_RB_InformationReconfig_r5,pdcp_Info))
	   {
		 if (rrc_get_rab_type_for_given_rb_id(rb_info_ptr->rb_Identity) != RRCRB_PS_DATA_RAB)
		 {
		   WRRC_MSG1_HIGH("NW incld PDCP info for non-ps rb id %d: Invalid",
					rb_info_ptr->rb_Identity);
		   rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
		   return;
		 }
         /* check whether lossless SRNS relocation flag is set or   
          * and header compression algo is present in the message 
          */
		 if ((RRC_MSG_COMMON_BITMASK_IE(rb_info_ptr->pdcp_Info.pdcp_Info,rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) && 
			 (RRC_CHECK_COMMON_MSG_TYPE(rb_info_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport,rrc_LosslessSRNS_RelocSupport_supported)))
		 {
		   if (rb_info_ptr->pdcp_Info.pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_absent)
		   {
			 WRRC_MSG0_HIGH("PDCP pdu header is absent.  Invalid config");
			 rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
			 return;
		   }
		   
		 } /* if rrc_PDCP_Info_r4_losslessSRNS_RelocSupport_present */

		 if (rb_info_ptr->pdcp_Info.pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) 
		 {
		   if ((RRC_MSG_COMMON_BITMASK_IE(rb_info_ptr->pdcp_Info.pdcp_Info,rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) && 
			   (RRC_CHECK_COMMON_MSG_TYPE(rb_info_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport,rrc_LosslessSRNS_RelocSupport_notSupported)))
		   {
			 if (!(RRC_MSG_COMMON_BITMASK_IE(rb_info_ptr->pdcp_Info.pdcp_Info,
			 	rrc_PDCP_Info_r4,headerCompressionInfoList)))
			 {
			   WRRC_MSG0_HIGH("PDCP header compress info list not present");
			   rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
			   return;
			 }
		   }
		 }

		 if (RRC_MSG_COMMON_BITMASK_IE(rb_info_ptr->pdcp_Info.pdcp_Info,
			 	rrc_PDCP_Info_r4,headerCompressionInfoList)) 
		 {
		   uint32 local_idx=0;
		   while (rb_info_ptr->pdcp_Info.pdcp_Info.headerCompressionInfoList.n >   local_idx) 
		   {
		     pdcpHeaderCompressionInfo = &rb_info_ptr->pdcp_Info.pdcp_Info.headerCompressionInfoList.elem[local_idx];
			 if (RRC_CHECK_COMMON_MSG_TYPE(pdcpHeaderCompressionInfo->algorithmSpecificInfo,
				 rrc_AlgorithmSpecificInfo_r4_rfc2507_Info))
			 {
			   /* if the UE capability "Maximum header compression context space", is exceeded with this configuration send
                * failure with cause "Invalid Config"
                */
			   if ( 
				   (pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER > IPHC_MAX_HC_CONTEXT_SPACE))
			   {
				 WRRC_MSG1_HIGH("Max header comprss context space exceeded : %d",
						  pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER);
				 rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
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
				 if ((pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->ul_RFC3095.max_CID > ROHC_MAX_CONTEXT_SESSIONS)) 
				 {
				   WRRC_MSG1_HIGH("Invalid config.  MAX_CID %d is invalid",
							pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->ul_RFC3095.max_CID);
				   rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
				   return;
				 }
			   }
			   if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info,dl_RFC3095))
			   {
				 if ((pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->dl_RFC3095.max_CID > ROHC_MAX_CONTEXT_SESSIONS)) 
				 {
				   WRRC_MSG1_HIGH("Invalid config.  MAX_CID %d is invalid",
							pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->dl_RFC3095.max_CID);
				   rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
				   return;
				 }
			   }
			 }/* else if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.t == rrc_rfc3095_Info)*/

			 local_idx++;
		   } /*  while (pdcpHeaderCompressionInfoList) */
		 } /*  if (rb_info_ptr->pdcp_Info.pdcp_Info.bit_mask & rrc_PDCP_Info_r4_headerCompressionInfoList_present) */

	   } /* if (rb_info_ptr->bit_mask & rrc_RB_InformationReconfig_r5_pdcp_Info_present) */
	   local_rb_idx++;
	 } /* while (rb_list_ptr) */
     
   } /* if (rbrc_ptr->specificationMode.u.complete.bit_mask & rrc_complete_rb_InformationReconfigList_present) */
 }
#endif /* FEATURE_UMTS_PDCP */

  /*Cell_PCH/URA_PCH specific validations */
 if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
     (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
 {
   /* If UTRAN DRX coefficient is not present then it is invalid configuration */
   if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,
         rrc_RadioBearerReconfiguration_r5_IEs,utran_DRX_CycleLengthCoeff)))
   {
     WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", rbrc.state_indicated_by_msg);
     rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
     return;
   }
   if (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
   {
     /*should we do it after getting l2ack for response??*/
     if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r5_IEs,ura_Identity)))
     {
       WRRC_MSG0_HIGH("URA ID not present");
       rrccu_clear_ura_id ();
     }
     else
     {
       /*URA ID present in the message set ura_identity variable with this value */
       if (rbrc_ptr->ura_Identity.numbits == 16)
       {
         rrccu_update_ura_id (&(rbrc_ptr->ura_Identity));
       }
       else
       {
         WRRC_MSG1_ERROR("Invalid URA ID length%d", rbrc_ptr->ura_Identity.numbits);
         rbrc.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
         rbrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
         return;
       }
     }
   }
 } // if RRC_STATE_CELL_PCH or RRC_STATE_URA_PCH
 
}/* end function validate_rbrc_message_r5 */

/*===========================================================================

FUNCTION   VALIDATE_RBRC_MESSAGE_R6

DESCRIPTION

  This function validates the received RB Reconfig Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a Radio Bearer Reconfig Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_rbrc_message_r6
(
  rrc_RadioBearerReconfiguration_r6_IEs *rbrc_ptr /* Pointer to the RB RC message */
)
{
  rrc_RAB_InformationReconfigList *rab_list_ptr = NULL;  

  rrc_rab_id_type rab_id;           /* To store RAB id in internal RRC format */
  rrcmn_rab_status_T rab_type;      /* To store the status returned by MN layer. This
                                    indicates what type of RAB is received. This is 
                                    used only if the RAB is for the CS_domain */
  rrc_rab_search_e_type rab_status; /* To check if RAB is present in ESTABLISHED_RABS */
  uint32 rab_index=0;               /* Used as an index when storing established
                                    RABs locally. Only when we have more than
                                    one RAB per message, this will be incremented */
#ifdef FEATURE_VOC_AMR_WB
  mvs_mode_type nas_sync_ind=rrc_get_current_codec_mode();
#endif/*FEATURE_VOC_AMR_WB*/

  /* Assume the message is valid for now */
  rbrc.status.failure_status = RRCRB_MSG_VALID;


  /* Check the RRC State indicatior given by the message and 
     store it in the rbrc internal memory
   */
  switch (rbrc_ptr->rrc_StateIndicator)
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      rbrc.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      rbrc.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      rbrc.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;

    case rrc_RRC_StateIndicator_ura_PCH:
      rbrc.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;

    default:
      /* No other states are supported right now. */
      WRRC_MSG1_HIGH("Unsupported state %d in RBRC R5 Msg", rbrc_ptr->rrc_StateIndicator);
      rbrc.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
      break;
  }

  if (rbrc.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG)
  {
    return;
  }

  /* Validate DL HSPDSCH */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r6_IEs,dl_HSPDSCH_Information))
  {
    /* Check if network does not try to initiate HSDPA with invalid state */
    if (rrc_hsdpa_initial_validation_r6(rbrc_ptr->rrc_StateIndicator, 
                                     &rbrc_ptr->dl_HSPDSCH_Information) == FAILURE)
    {
      rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }

  /* Not support to the pre-config option. */
  if (rbrc_ptr->specificationMode.t != 
      T_rrc_RadioBearerReconfiguration_r6_IEs_specificationMode_complete)
  {
    WRRC_MSG0_HIGH("Non-support preconfig t in RBRC");
    rbrc.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
    return;
  }

  

  /* Obtain U_RNTI if present for SRNS relocation.  */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,
  	rrc_RadioBearerReconfiguration_r6_IEs,new_U_RNTI))
  {
    /* check whether the SRNS relocation is within Cell_DCH or not.
       SRNS relocation information can come in reconfiguration messages
       only when the state transition is Cell_DCH->Cell_DCH */
    if (!((rbrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("SRNS reloc with invalid states cur_state %d, msg_state %d",
          rrc_get_state(), rbrc.state_indicated_by_msg);
      rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    /* message with valid SNRS relocation info */
    else
    {
      /* Set the flag that indicates that this procedure
         involves SRNS relocation */
      rbrc.rbrc_for_srns_relocation = TRUE;
      /*store the new U-RNTI */
      rrc_translate_urnti (&(rbrc_ptr->new_U_RNTI), &rbrc.new_urnti);
    }
  }

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  rbrc.trans_to_dch_ciph_config_needed = FALSE;
  if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    /* check to see if dl-dpch-common info is present while going to dch */
    if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,
         rrc_RadioBearerReconfiguration_r6_IEs,dl_CommonInformation)) &&
        (RRC_MSG_COMMON_BITMASK_IE(rbrc_ptr->dl_CommonInformation,
         rrc_DL_CommonInformation_r6,dl_dpchInfoCommon)))
    {
      if (rbrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.t == 
          T_rrc_DL_CommonInformation_r6_dl_dpchInfoCommon_dl_DPCH_InfoCommon)
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
               ( RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.dl_DPCH_InfoCommon,
               rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
               &rbrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
               &mac_d_hfn_l);
          if(loc_ret_val == FAILURE)
          {
            WRRC_MSG0_HIGH("get_mac_d_hfn failed");
          }
          if (rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
          {
            WRRC_MSG0_HIGH("Processing Ciphering Config for HHO failed");
          }
          rbrc.trans_to_dch_ciph_config_needed = TRUE;
        }
        else
        {
          WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
        }
      }
    }
  }

  if (rbrc_ptr->specificationMode.u.complete.m.rab_InformationReconfigListPresent)
  {
    MSG_LOW("RAB Info Reconfig in R6 RBRC Msg", 0, 0, 0);

    rab_list_ptr = &rbrc_ptr->specificationMode.u.complete.rab_InformationReconfigList;

    for (rab_index = 0; rab_list_ptr->n > rab_index && rab_index < RRCRB_MAX_RAB_PER_MSG; rab_index++)
    {
      /* Check if the RAB id is GSM_MAP type. We don't support any
         other type */
      if (rab_list_ptr->elem[rab_index].rab_Identity.t != T_rrc_RAB_Identity_gsm_MAP_RAB_Identity)
      {
        WRRC_MSG1_HIGH("RABid t%d is not GSM_MAP!", rab_list_ptr->elem[rab_index].rab_Identity.t);
        rbrc.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
        return;
      }

      /* GSM_MAP RAB Id. Check if it's valid by calling the appropriate NAS entity. 
         First convert it to internal format. */
      rab_id = rrcrb_translate_gsm_map_rab_id((rab_list_ptr->elem[rab_index].rab_Identity.u.gsm_MAP_RAB_Identity));

      /* Check if the RAB id is already in ESTABLISHED_RABS */
      rab_status = rrc_find_rab_in_established_rabs(rab_id);
      if ( rab_status != RRC_RAB_FOUND )
      {
        WRRC_MSG1_HIGH("RAB ID 0X%x not in ESTABLISHED_RABS", rab_id);
        rbrc.status.failure_status=RRCRB_MSG_INVALID_CONFIG;
        return;
      }

      /* Store this info locally so that ESTABLISHED_RABS can
         be updated later, when these RABs are established */
      rbrc.rabs.rabs_in_msg[rab_index].rab_id_is_gsm_map = TRUE;
      rbrc.rabs.rabs_in_msg[rab_index].rab_id = rab_id;

      /* Check if this RAB is CS Domain or PS Domain */
      if ( rab_list_ptr->elem[rab_index].cn_DomainIdentity  == rrc_CN_DomainIdentity_cs_domain )
      {
        /* Store the domain identity. This will be later added to
        ESTABLISHED_RABS */
        rbrc.rabs.rabs_in_msg[rab_index].cn_domain = RRC_CS_DOMAIN_CN_ID;

        /* Check if it is in loopback test */
        if ( rrclbt_is_lb_on() == TRUE )
        {
          /* Set Rab type to Data */
          MSG_LOW("Loopback Test", 0, 0, 0);
          rbrc.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_DATA_RAB;
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
            rrc_get_rab_type_for_given_rab_id(rbrc.rabs.rabs_in_msg[rab_index].rab_id));

          switch ( rab_type )
          {
            case RRCMN_CS_DATA_CALL:
              rbrc.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_DATA_RAB;
              break;

            case RRCMN_CS_VOICE_CALL:
              rbrc.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_CS_VOICE_RAB;

#ifdef FEATURE_SCUDIF_ENHANCEMENT
              #error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/

              break;

            default:
              WRRC_MSG0_ERROR("CS RAB validation failed!");
              rbrc.status.failure_status=RRCRB_MSG_UNSUPPORTED_CONFIG;
              break;

          } /* end switch */
#ifdef FEATURE_VOC_AMR_WB
          /*As a general approach we should use/update the NAS Sync Indicator even for CS DATA*/
          if((rbrc.rabs.rabs_in_msg[rab_index].rab_type == RRCRB_CS_VOICE_RAB)
#ifdef FEATURE_SCUDIF_ENHANCEMENT
#error code not present
#endif
)
          {
              if(rrcrbrc_update_nas_sync_ind(rab_index,
                &(rab_list_ptr->elem[rab_index].nas_Synchronisation_Indicator))
                 == FAILURE)
              {
                return;
              }
              nas_sync_ind =  rbrc.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator;
          }
#endif /*FEATURE_VOC_AMR_WB*/
  
        }

        if ( (rbrc.status.failure_status == RRCRB_MSG_INVALID_CONFIG) ||
             (rbrc.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG)
           )
        {
          /* No need to continue */
          return;
        }
      }
      /* Else check for PS Domain */
      else if ( rab_list_ptr->elem[rab_index].cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain )
      {
        /* Store the domain identity. This will be later added to
           ESTABLISHED_RABS 
         */
        rbrc.rabs.rabs_in_msg[rab_index].cn_domain = RRC_PS_DOMAIN_CN_ID;
        WRRC_MSG1_HIGH("PS RABId %d to be setup", rab_id);
        rbrc.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_PS_DATA_RAB;
      }
      
      rbrc.rabs.num_rabs_in_msg ++;
    } // for each RAB info
  } //RAB info IE present
#ifdef FEATURE_VOC_AMR_WB
  rrc_set_ordered_codec_mode(nas_sync_ind);

#endif/*FEATURE_VOC_AMR_WB*/

  if((T_rrc_RadioBearerReconfiguration_r6_IEs_specificationMode_complete ==
     (rbrc_ptr->specificationMode.t) &&
      rbrc_ptr->specificationMode.u.complete.m.rb_InformationReconfigListPresent))
  {
    /* Save the linked list head pointer */
    if (FAILURE == rrcllc_validate_rb_info_reconfig_list_r6(
        &rbrc_ptr->specificationMode.u.complete.rb_InformationReconfigList))
    {
      rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }

  /*Cell_PCH/URA_PCH specific validations */
  if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
    /* If UTRAN DRX coefficient is not present then it is invalid configuration */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,
          rrc_RadioBearerReconfiguration_r6_IEs,utran_DRX_CycleLengthCoeff)))
    {
      WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", rbrc.state_indicated_by_msg);
      rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    if (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
    {
      /*should we do it after getting l2ack for response??*/
      if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,
	  	rrc_RadioBearerReconfiguration_r6_IEs,ura_Identity)))
      {
        WRRC_MSG0_HIGH("URA ID not present");
        rrccu_clear_ura_id ();
      }
      else
      {
        /*URA ID present in the message set ura_identity variable with this value */
        if (rbrc_ptr->ura_Identity.numbits == 16)
        {
          rrccu_update_ura_id (&(rbrc_ptr->ura_Identity));
        }
        else
        {
          WRRC_MSG1_ERROR("Invalid URA ID length%d", rbrc_ptr->ura_Identity.numbits);
          rbrc.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
          rbrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
          return;
        }
      }
    }
  } // if RRC_STATE_CELL_PCH or RRC_STATE_URA_PCH

}/* end function validate_rbrc_message_r6 */

/*===========================================================================

FUNCTION   VALIDATE_RBRC_MESSAGE_R7

DESCRIPTION

  This function validates the received RB Reconfig Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a Radio Bearer Reconfig Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void validate_rbrc_message_r7
(
  rrc_RadioBearerReconfiguration_r7_IEs *rbrc_ptr /* Pointer to the RB RC message */
)
{
#ifdef FEATURE_VOC_AMR_WB
  mvs_mode_type nas_sync_ind=rrc_get_current_codec_mode();
#endif/*FEATURE_VOC_AMR_WB*/

  /* Assume the message is valid for now */
  rbrc.status.failure_status = RRCRB_MSG_VALID;


  /* Check the RRC State indicatior given by the message and 
     store it in the rbrc internal memory
   */
  switch (rbrc_ptr->rrc_StateIndicator)
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      rbrc.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      rbrc.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      rbrc.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;

    case rrc_RRC_StateIndicator_ura_PCH:
      rbrc.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;

    default:
      /* No other states are supported right now. */
      WRRC_MSG1_HIGH("Unsupported state %d in RBRC R7 Msg", rbrc_ptr->rrc_StateIndicator);
      rbrc.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
      break;
  }

  if (rbrc.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG)
  {
    return;
  }

  /* Validate DL HSPDSCH */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r7_IEs,dl_HSPDSCH_Information))
  {
    /* Check if network does not try to initiate HSDPA with invalid state */
    if (rrc_hsdpa_initial_validation(rbrc_ptr->rrc_StateIndicator, 
                                    RRC_CHECK_COMMON_MSG_TYPE(rbrc_ptr->dl_HSPDSCH_Information.modeSpecificInfo,
                                     rrc_DL_HSPDSCH_Information_modeSpecificInfo_tdd)) == FAILURE)
    {
      rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }

  /* Not support to the pre-config option. */
  if (rbrc_ptr->specificationMode.t != 
      T_rrc_RadioBearerReconfiguration_r7_IEs_specificationMode_complete)
  {
    WRRC_MSG0_HIGH("Non-support preconfig t in RBRC");
    rbrc.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
    return;
  }

  

  /* Obtain U_RNTI if present for SRNS relocation.  */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r7_IEs,new_U_RNTI))
  {
    /* check whether the SRNS relocation is within Cell_DCH or not.
       SRNS relocation information can come in reconfiguration messages
       only when the state transition is Cell_DCH->Cell_DCH */
    if (!((rbrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("SRNS reloc with invalid states cur_state %d, msg_state %d",
          rrc_get_state(), rbrc.state_indicated_by_msg);
      rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    /* message with valid SNRS relocation info */
    else
    {
      /* Set the flag that indicates that this procedure
         involves SRNS relocation */
      rbrc.rbrc_for_srns_relocation = TRUE;
      /*store the new U-RNTI */
      rrc_translate_urnti (&(rbrc_ptr->new_U_RNTI), &rbrc.new_urnti);
    }
  }

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch
   */
  rbrc.trans_to_dch_ciph_config_needed = FALSE;
  if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    /* check to see if dl-dpch-common info is present while going to dch */
    if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,  
         rrc_RadioBearerReconfiguration_r7_IEs,dl_CommonInformation)) &&
        (RRC_MSG_COMMON_BITMASK_IE(rbrc_ptr->dl_CommonInformation,
         rrc_DL_CommonInformation_r7,dl_dpchInfoCommon)))
    {
      if (rbrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.t == 
          T_rrc_DL_CommonInformation_r7_dl_dpchInfoCommon_dl_DPCH_InfoCommon)
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
               (RRC_MSG_COMMON_BITMASK_IE_PTR( 
               rbrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.dl_DPCH_InfoCommon,
               rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
               &rbrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
               &mac_d_hfn_l);
          if(loc_ret_val == FAILURE)
          {
            WRRC_MSG0_HIGH("get_mac_d_hfn failed");
          }
          if (rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
          {
            WRRC_MSG0_HIGH("Processing Ciphering Config for HHO failed");
          }
          rbrc.trans_to_dch_ciph_config_needed = TRUE;
        }
        else
        {
          WRRC_MSG0_HIGH("To-DCH-Step-1 not needed");
        }
      }
    }
  }

  if (rbrc_ptr->specificationMode.u.complete.m.rab_InformationReconfigListPresent)
  {

    rbrc.status.failure_status = rrc_update_rab_info_reconfig_param(
    &rbrc_ptr->specificationMode.u.complete.rab_InformationReconfigList, &rbrc.rabs 
#ifdef FEATURE_VOC_AMR_WB
                                             ,&nas_sync_ind
#endif/*FEATURE_VOC_AMR_WB*/
                                           );
                                           
   if( rbrc.status.failure_status != RRCRB_MSG_VALID) 
   {
      return;
    }
    // for each RAB info
  } //RAB info IE present
#ifdef FEATURE_VOC_AMR_WB
  rrc_set_ordered_codec_mode(nas_sync_ind);
 
#endif/*FEATURE_VOC_AMR_WB*/

  if((T_rrc_RadioBearerReconfiguration_r7_IEs_specificationMode_complete ==
     (rbrc_ptr->specificationMode.t) &&
      rbrc_ptr->specificationMode.u.complete.m.rb_InformationReconfigListPresent))
  {
    /* Save the linked list head pointer */
    if (FAILURE == rrcllc_validate_rb_info_reconfig_list_r7(
        &rbrc_ptr->specificationMode.u.complete.rb_InformationReconfigList))
    {
      rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }

  /*Cell_PCH/URA_PCH specific validations */
  if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
    /* If UTRAN DRX coefficient is not present then it is invalid configuration */
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,
          rrc_RadioBearerReconfiguration_r7_IEs,utran_DRX_CycleLengthCoeff)))
    {
      WRRC_MSG1_ERROR("UTRAN DRX Coef not preset, State:%d", rbrc.state_indicated_by_msg);
      rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    if (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
    {
      /*should we do it after getting l2ack for response??*/
      if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r7_IEs,ura_Identity)))
      {
        WRRC_MSG0_HIGH("URA ID not present");
        rrccu_clear_ura_id ();
      }
      else
      {
        /*URA ID present in the message set ura_identity variable with this value */
        if (rbrc_ptr->ura_Identity.numbits == 16)
        {
          rrccu_update_ura_id (&(rbrc_ptr->ura_Identity));
        }
        else
        {
          WRRC_MSG1_ERROR("Invalid URA ID length%d", rbrc_ptr->ura_Identity.numbits);
          rbrc.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
          rbrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
          return;
        }
      }
    }
  } // if RRC_STATE_CELL_PCH or RRC_STATE_URA_PCH

}/* end function validate_rbrc_message_r7 */


/*===========================================================================

FUNCTION   RRCRBRC_SEND_CHANNEL_CONFIG_REQ

DESCRIPTION

  This function sends the RRC_CHANNEL_CONFIG_REQ command to LLC
  to reconfig the necessary RBs associated with a RAB. If signalling
  Radio bearers are affected and included in the Radio Bearer
  Reconfig Message, they will also be sent in the same command.
  
  Currently the messege is not expected to have Signalling
  Radio Bearers for reconfig. If these are added in the future,
  they will also need to be sent in the same command.
        
DEPENDENCIES

  ORDERED_CONFIG must be set before calling this function.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbrc_send_channel_config_req
(
  void
)
{

  rrc_cmd_type *ptr;          /* To hold the Command                       */
  uint32 rb_count;            /* To count the number of RBs in the command */
  uint32 rb_index=0;          /* Index for the RB list */
  rrc_state_e_type next_rrc_state = rbrc.state_indicated_by_msg;
  
  /* Get the command buffer */
  ptr = rrc_get_int_cmd_buf();
  /* Fill in the command id  */
  ptr->cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;

  /* First fill up the Radio Bearers that need to be reconfigured.
  For this we need to know how many RBs need to be reconfigured.
  */
  
  if( rbrc.rbs_tobe_reconfigured.is_valid == TRUE )
  {
    for(rb_count=0; rb_count<rbrc.rbs_tobe_reconfigured.num_rbs; rb_count++)
    {
          /* Fill up the RB id and indicate that it needs to be reconfigured */
          ptr->cmd.chan_config_req.rb[rb_index].rb_id = 
            rbrc.rbs_tobe_reconfigured.rb_id[rb_count];

       ptr->cmd.chan_config_req.rb[rb_index].rb_config = RECONFIG_RB;
       ptr->cmd.chan_config_req.rb[rb_index].rb_config = RECONFIG_RB_OTA;
       ptr->cmd.chan_config_req.rb[rb_index].ul_cfg_mask = rbrc.rbs_tobe_reconfigured.ul_cfg_mask[rb_count];
       ptr->cmd.chan_config_req.rb[rb_index].dl_cfg_mask = rbrc.rbs_tobe_reconfigured.dl_cfg_mask[rb_count];
       rb_index++;
       
    }
  }/* end if rbrc.rbs_tobe_reconfigured.is_valid  == TRUE */
  if( rbrc.rbs_tobe_affected.is_valid == TRUE )
  {
    for(rb_count=0; rb_count<rbrc.rbs_tobe_affected.num_rbs; rb_count++)
    {
          /* Fill up the RB id and indicate that it needs to be affected */
          ptr->cmd.chan_config_req.rb[rb_index].rb_id = 
            rbrc.rbs_tobe_affected.rb_id[rb_count];
          ptr->cmd.chan_config_req.rb[rb_index].rb_config = RECONFIG_RB_OTA;
          ptr->cmd.chan_config_req.rb[rb_index].ul_cfg_mask = ptr->cmd.chan_config_req.rb[rb_index].dl_cfg_mask = MODIFY_RB_CFG;
          rb_index++;
    }
  }/* end if rbrc.rbs_tobe_affected.is_valid  == TRUE */



  /* Check if state change is required */
  if( (rrc_get_state()) == (rbrc.state_indicated_by_msg) )
  {
    ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;
  }
  else
  {
    /* The message indicates that state change is required.
    Indicate to LLC what the next state should be. */
    ptr->cmd.chan_config_req.rrc_state_change_required = TRUE;
    ptr->cmd.chan_config_req.next_state                = rbrc.state_indicated_by_msg;

    if ((rrc_get_state() == RRC_STATE_CELL_FACH) && 
        ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) || 
         (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH)))
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
  of reconfigured and affected RBs */
  ptr->cmd.chan_config_req.num_rb_to_config = (uint16)rb_index;
  if(rb_index < MAX_RB)
  {
    rrc_fill_rb_id_chan_req_from_standalone_db(&rb_index, &ptr->cmd.chan_config_req);
  }

  if(rbrc.state_indicated_by_msg == RRC_STATE_CELL_DCH)
  {
    /* Fill those RBs in chan_config_req for which RLC PDU size has changed and 
    that are already not present in chan_config_req*/
    rrc_fill_rb_id_chan_req_from_rb_list_mapped_to_dch(&rb_index, &ptr->cmd.chan_config_req);

    if(rrc_get_state() == RRC_STATE_CELL_FACH)
    {
      rrc_fill_srb_in_chan_config_req(&rb_index, &ptr->cmd.chan_config_req);
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
    rrc_fill_rb_id_chan_req_from_est_rabs(&rb_index, &ptr->cmd.chan_config_req);
  }

  /* Indicate that we need a confirmation for this command */
  ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = TRUE;

  /* Indicate the id of the procedure sending the command */
  ptr->cmd.chan_config_req.procedure = RRC_PROCEDURE_RBRC;

  /* We are not required to fill any other data for this command.
  Now, send the command */
  MSG_HIGH("Sending CHANNEL_CONFIG_REQ RBs:%d, curr_st:%d, next_st:%d", 
             rb_index, rrc_get_state(), rbrc.state_indicated_by_msg);
  rrc_put_int_cmd( ptr );

}/* end function rrcrbrc_send_channel_config_req */


/*===========================================================================

FUNCTION  RRCRBRC_CLEAR_PROCEDURE

DESCRIPTION

  This function clears all the internal variables in
  the RBRC procedure and brings them to their default
  values.
    
DEPENDENCIES

  The Substate of the procedure should be changed
  to initial substate before calling this function.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbrc_clear_procedure( void )
{
  uint32 rab_index;
  /* Initialize the transaction id's for this procedure.
  Use a number that can't be an actual received transaction
  id. */
  rbrc.tr_ids.accepted_transaction = RRCRB_INVALID_TRANSACTION_ID;
  rbrc.tr_ids.rejected_transaction = RRCRB_INVALID_TRANSACTION_ID;

  rbrc.new_crnti_valid       = FALSE;
  rbrc.directed_cell_info    = FALSE;
  rrcrbrc_send_initiate_cu_flag = FALSE;
  rrcrbrc_delete_crnti          = FALSE;
  /* Indicate that there are no RBs to be reconfigured and
  no rbs tobe affected. This is the default value. */
  rbrc.rbs_tobe_reconfigured.is_valid = FALSE;
  rbrc.rbs_tobe_affected.is_valid     = FALSE;

  if (rbrc.cn_info_saved != FALSE)
  {
    /* free the memory allocated for mm cmd */
    WRRC_MSG1_HIGH("Free MM cmd buffer %x", rbrc.mm_cmd_ptr);
    mm_free_cmd_buf(rbrc.mm_cmd_ptr);
    /* set cn_info_saved to false */
    rbrc.cn_info_saved = FALSE;
  }

  rbrc.rbrc_for_srns_relocation = FALSE;

  /*clear the rab information */
  for (rab_index=0; rab_index<RRCRB_MAX_RAB_PER_MSG; rab_index++)
  {
    rbrc.rabs.rabs_in_msg[rab_index].num_rbs_for_rab = 0;
    rbrc.rabs.rabs_in_msg[rab_index].rab_type = RRCRB_UNKNOWN_RAB;    
    rbrc.rabs.chan_config_needed[rab_index] = FALSE;
#ifdef FEATURE_VOC_AMR_WB
    rbrc.rabs.rabs_in_msg[rab_index].nas_Synchronisation_Indicator = MVS_MODE_NONE;
#endif /*FEATURE_VOC_AMR_WB*/
  }
  rbrc.rabs.num_rabs_in_msg = 0;
  rbrc.status.failure_status = RRCRB_MSG_VALID;
  rbrc.trans_to_dch_ciph_config_needed = FALSE;

  rbrc.initiate_cell_selection_trans_from_fach = FALSE;     

  rbrc.high_mobility_ind = FALSE;

#ifdef FEATURE_VOC_AMR_WB
  rrc_set_ordered_codec_mode(rrc_get_current_codec_mode());
#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
  rrc_set_ordered_codec_status(FALSE);
#endif
#endif

  rrc_check_and_init_rrc_reestab_entity(RRC_PROCEDURE_RBRC);

}



/*===========================================================================

FUNCTION   RRCRBRC_PROCESS_SUCCESSFUL_PROC_COMPLETION

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
static void rrcrbrc_process_successful_proc_completion
(
 boolean indicate_rlc
)
{
  /* send the nas cn info if stored */
  if (rbrc.cn_info_saved != FALSE)
  {
    if(rbrc.mm_cmd_ptr->cmd.hdr.message_id == (byte) RRC_SERVICE_IND_FROM_OTA_MSG )
    {
      /* Update the cell access information in the service info structure*/
      rrc_ccm_update_service_info(rbrc.mm_cmd_ptr);
    }
    /* Put the command on MM queue */
    rrc_send_rrc_message_to_mm(rbrc.mm_cmd_ptr);
    /* clear the flag */
    rbrc.cn_info_saved = FALSE;
  }
  /* Clear ORDERED_CONFIG - UE can now accept other ordered
  configurations */
  rrcllc_clear_ordered_config();  

  if (((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) || 
       (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH )) &&
      (indicate_rlc == TRUE))
  {
    /* register with RLC for uplink data indication */
    rrcrb_register_with_rlc_for_ul_data_ind (RRC_PROCEDURE_RBRC);
  }

  /* Go to the next sub-state in the state machine. That's
  the initial substate since we've sent the RB RC complete
  message */
  rbrc.curr_substate = RRCRB_INITIAL;

  /* At this point we don't need to remember any of the
  Radio Bearer information since it's stored in ESTABLISHED_RABS.
  Clear our internal variables */
  rrcrbrc_clear_procedure();

  if (rrccu_return_cu_substate() == RRCCU_WAIT_FOR_RB_PROC_COMPL_IND) 
  {
    rrcrb_send_rb_proc_compl_ind();
  }
  rrccu_reset_registration_info_for_procedure(RRC_PROCEDURE_RBRC);
}

/*===========================================================================

FUNCTION   RRCRBRC_UPDATE_ORDERED_CONFIG

DESCRIPTION

  This function processes the received Channel Config Confirmation
  from LLC. If the required Radio-bearers have been reconfig succesfully,
  and if any of the radio-bearers map to radio-access-bearers, this
  information needs to be given to the NAS entity for that RAB.
  
  Currently Circuit-Switched voice and Circuit-Switched data RABs are 
  supported. If a voice RAB has been assigned by the UTRAN, this function
  gets the RLC ids and the AMR mode information from LLC and sends this
  information to voice services. If a CS Data RAB has been assigned, the
  RLC ids for those RBs are sent to the data services entity. Then this
  function sends the RRC_SYNC_IND primitive to MM.
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbrc_update_ordered_config
(
 rrc_DL_DCCH_Message *dcch_msg_ptr      /* Pointer to the RB Reconfig message */
)
{
  rrc_state_e_type                       current_state;       /* current RRC State  */
  rrcllc_oc_status_e_type                oc_status;/* To hold ordered_config status          */
  boolean                                ordered_config_err_occured = FALSE; /* Used to indicate if an error
                                                                       occured while setting ordered_config. */
  rrc_int_c_rnti_type                    c_rnti;
  rrc_int_u_rnti_type                    u_rnti;
  rrcrb_rnti_update_info_type            rnti_update_info;   /* to hold rnti update info */
  rrc_RadioBearerReconfiguration         *msg_ptr; /* Pointer to the RB Reconfig message        */
  rrcrb_directed_cell_info_type          directed_cell_info; /* directed cell info struct */


   /* We have to reconfigure RBs. First set ORDERED_CONFIG
  so that LLC can process the necessary parameters. */
  /* Get the pointer to the actual RB Reconfig message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerReconfiguration);
  oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_RBRC,
                                       rbrc.state_indicated_by_msg,
                                       rrc_DL_DCCH_Message_PDU,
                                       (void *)dcch_msg_ptr
                                     );
  switch (oc_status)
  {
    case ORDERED_CONFIG_SET:
      if((rrc_sib_is_current_event_sib_read_in_dch()) &&
         (RRC_PROCEDURE_GPS == rrc_sib_get_proc_id_for_sib_read_in_dch()))
      {
        WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
        rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
      }

    current_state = rrc_get_state(); 

    directed_cell_info.freq_info_present = FALSE;
    directed_cell_info.pscr_present = FALSE;  
    /* Populate the local variables to indicate if Re-direction info is present */
    if((current_state==RRC_STATE_CELL_FACH) && 
       ((rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
        || (rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
        || (rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)))
    {
      if((RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerReconfiguration_r3, 
          rrc_RadioBearerReconfiguration_r3_IEs,frequencyInfo)) &&
         (msg_ptr->u.r3.radioBearerReconfiguration_r3.frequencyInfo.
          modeSpecificInfo.t == T_rrc_FrequencyInfo_modeSpecificInfo_fdd))
      {
        directed_cell_info.freq_info_present = TRUE;
        directed_cell_info.uarfcn_dl         = msg_ptr->u.r3.radioBearerReconfiguration_r3.
                                               frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
      }
       
      if (rrcrbrc_is_pcpich_info_present(msg_ptr, MSG_REL99))
      {
        directed_cell_info.pscr_present = TRUE;
        directed_cell_info.pscr = msg_ptr->u.r3.radioBearerReconfiguration_r3.dl_InformationPerRL_List.elem[0].
                                  modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
      }
      /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
          FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
      if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
        (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
      {
        WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
        if(rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
        {
          rrcrbrc_send_initiate_cu_flag = TRUE;
          rrcrbrc_delete_crnti          = TRUE;
        }
        else
        {
          rbrc.directed_cell_info = TRUE;
        }
      }      
    }
    if ((current_state==rbrc.state_indicated_by_msg) ||
         ((current_state==RRC_STATE_CELL_FACH) && 
            (rbrc.state_indicated_by_msg==RRC_STATE_CELL_DCH)))

    {
      /* CELL_DCH->CELL_DCH or CELL_FACH->CELL_FACH case. We can release the
      Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */
      /*check whether any frequency info is present in the message
      and transition is CELL_FACH->CELL_FACH. In this case
      initiate a cell update after channel config is successfully completed */
      if ((current_state==RRC_STATE_CELL_FACH)&& 
         (rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
      {
     
       
        /*call the function that evaluates the directed cell info and set the flags accordingly */
        if((rrcrbrc_send_initiate_cu_flag == FALSE) && 
          (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
        {

          if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerReconfiguration_r3,
           rrc_RadioBearerReconfiguration_r3_IEs,new_C_RNTI))
          {
            rbrc.new_crnti_valid = TRUE;
            rrc_translate_crnti(&msg_ptr->u.r3.radioBearerReconfiguration_r3.new_C_RNTI, 
                                &c_rnti);
            rbrc.new_crnti = (uint16)c_rnti;
          }
          else
          {
            rbrc.new_crnti_valid = FALSE;
          }
      
          rrcllcoc_update_toc_with_oc_from_fach();
          
          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
                                                 RRC_TRANSITION_FROM_FACH,
                                                 rbrc.state_indicated_by_msg);
  
  
          rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
     
            
          /* exit from the switch case */
          break;
        }
      } /* if CELL_FACH->CELL_FACH reconfiguration */
     
      /* Call the function that creates and sends a channel
        config request to LLC. */
      rrcrbrc_send_channel_config_req();

      /* Change the substate since we're waiting for the
         confirmation from LLC for the channel configs */
      rbrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
    }
    else if((current_state==RRC_STATE_CELL_DCH) && 
            (rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
    {
      if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerReconfiguration_r3,
         rrc_RadioBearerReconfiguration_r3_IEs,new_C_RNTI))
      {
        rbrc.new_crnti_valid = TRUE;
        rrc_translate_crnti(&msg_ptr->u.r3.radioBearerReconfiguration_r3.new_C_RNTI, 
                            &c_rnti);
        rbrc.new_crnti = (uint16)c_rnti;
      }
      else
      {
        rbrc.new_crnti_valid = FALSE;
      }

      /*Initiate the cell selection*/
      rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
                                              RRC_TRANSITION_FROM_DCH,
                                              rbrc.state_indicated_by_msg);
      rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

      /* Start T_305 [periodic cell update] timer */
      rrccu_start_t_305_timer();
    }
    else if((current_state==RRC_STATE_CELL_DCH) && 
            (rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH))
    {
       rbrc.directed_cell_info = rrcrbrc_is_pcpich_info_present(msg_ptr, MSG_REL99);
      /*Send the RRCRBRC complete msg on old configuration */
      send_rbrc_complete_message(TRUE);
      /*wait for L2ack */
      rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
	  	  
      rrcllc_print_trans_chnl_rate();
    }
    else if((current_state==RRC_STATE_CELL_DCH) && 
            (rbrc.state_indicated_by_msg==RRC_STATE_URA_PCH))
    {
      /* no need to check directed cell info in this case */
      /*Send the RRCRBRC complete msg on old configuration */
      send_rbrc_complete_message(TRUE);
      /*wait for L2ack */
      rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;

      rrcllc_print_trans_chnl_rate();
    }
    else if((current_state==RRC_STATE_CELL_FACH) && 
            ((rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
             (rbrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
    {

      /*call the function that evaluates the directed cell info and set the flags accordingly */
      if((rbrc.directed_cell_info == FALSE) && 
        (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
      {
        if(rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
        {
          rbrc.directed_cell_info = rrcrbrc_is_pcpich_info_present(msg_ptr, MSG_REL99);
        }
        rbrc.initiate_cell_selection_trans_from_fach = TRUE;
      }
    

      /* check whether CPICH info is present or not. Store this
      info  */
     // rbrc.directed_cell_info = rrcrbrc_is_pcpich_info_present(msg_ptr);
      /*Send the RRCRBRC complete msg on old configuration */
      /* check whether new C-RNTI was present in the message. In that case
      we have to configure MAC with new C-RNTI before sending the response 
      message */
      if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerReconfiguration_r3,
         rrc_RadioBearerReconfiguration_r3_IEs,new_C_RNTI))
      {
        rrc_translate_crnti(&msg_ptr->u.r3.radioBearerReconfiguration_r3.new_C_RNTI, 
                            &c_rnti);
        /* Send a RNTI Update Request */
        rnti_update_info.crnti_action = RRC_RNTI_UPDATE;
        rnti_update_info.crnti        = (uint16)c_rnti;
        /* check whether URNTI is also present in the message */
        if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerReconfiguration_r3,
         rrc_RadioBearerReconfiguration_r3_IEs,new_U_RNTI))
        {
          rrc_translate_urnti(&msg_ptr->u.r3.radioBearerReconfiguration_r3.new_U_RNTI, 
                              &u_rnti);
          rnti_update_info.urnti_action = RRC_RNTI_UPDATE; 
          rnti_update_info.urnti        = u_rnti; 
        }
        else
        {
          rnti_update_info.urnti_action = RRC_RNTI_NOCHANGE; 
        }
        rnti_update_info.procedure            = RRC_PROCEDURE_RBRC;
        rnti_update_info.rlc_re_establish_srb = FALSE;
        rnti_update_info.rlc_re_establish_rab = FALSE;
        rnti_update_info.cnf_reqd             = TRUE;
        rrcrb_send_rnti_update_req(rnti_update_info);
        /* wait for the RNTI update confirm before sending the response
        message */
        rbrc.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
      }
      else 
      {
        send_rbrc_complete_message(TRUE);
        /*wait for L2ack */
        rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;

        rrcllc_print_trans_chnl_rate();
      }
    }
    break;

    case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:
        rbrc.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
        /* Send the RB Release Failure Message */
        send_rbrc_failure_message( rbrc.status.failure_status,
                                  rbrc.status.prot_err_cause );

        /* Go back to initial substate */
        rbrc.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

    case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:
        rbrc.status.failure_status =RRCRB_MSG_UNSUPPORTED_CONFIG;
        /* Send the RB Release Failure Message */
        send_rbrc_failure_message( rbrc.status.failure_status,
                                  rbrc.status.prot_err_cause );

        /* Go back to initial substate */
        rbrc.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

    case ORDERED_CONFIG_CONFIGURATION_INVALID:

        rbrc.status.failure_status =RRCRB_MSG_INVALID_CONFIG;
        /* Send the RB Release Failure Message */
        send_rbrc_failure_message( rbrc.status.failure_status,
                                  rbrc.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbrc.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

    case ORDERED_CONFIG_NOT_SET_OTHER:
        WRRC_MSG0_HIGH("LLC returned other failure!");
        /* When LLC sends this, there is a serious problem
        and we'll go back to disconnected state. No need
        to send the failure message here, we can just
        go back to initial substate */
        rbrc.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME        
    case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("RBRC processing deferred due to LTA!");

        rbrc.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
    default:

        WRRC_MSG0_ERROR("Unknown return value from LLC");
        /* Unknown error, just go back to initial substate */
        rbrc.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
  }/* end switch */

  if ( ordered_config_err_occured == TRUE )
  {
    /* No need to continue. Clear the procedure
    variables and return. */
    rrcrbrc_clear_procedure();
    return;
  }
  else
  {
    if(rbrc.rbrc_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }
  }
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
static uecomdef_status_e_type rrcrbrc_check_and_send_failure_for_simul_reconfig
(
  rrc_RRC_TransactionIdentifier transaction_id
)
{
  if ((rrcllc_get_ordered_config_status() != OC_NOT_SET) ||
      (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
      (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
  {
    MSG_HIGH("Incomatible simultaneous reconfig! Sending RBRC failure trans_id:%d, accepted_trans_id:%d, rejected_trans_id:%d",
      transaction_id,rbrc.tr_ids.accepted_transaction,rbrc.tr_ids.rejected_transaction);
    if ((transaction_id != rbrc.tr_ids.accepted_transaction ) && (transaction_id != rbrc.tr_ids.rejected_transaction ))
    {
      /* We have received a RB RC message with a new transaction id.
      This message should be in our rejected transaction list */
      rbrc.tr_ids.rejected_transaction = transaction_id;
      rbrc.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;

      /*Check whether Cell Update is going on */
      if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
           (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
      {
        if(rbrc.curr_substate == RRCRB_WAIT_FOR_CELL_UPDATE_CNF)
        {
          send_rbrc_failure_message( rbrc.status.failure_status,
                                   rbrc.status.prot_err_cause );
          rbrc.status.failure_status = RRCRB_MSG_VALID;
        }
        else
        {
          WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
          (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBRC);
          /* Wait until Cell update procedure is completed */
          rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF; 
        }
      }
      else
      {
        send_rbrc_failure_message( rbrc.status.failure_status,
                                   rbrc.status.prot_err_cause );
        /*continue the same substate */ 
      }
    }
    return FAILURE;
  }

  /* We have received a RB RC message with a new transaction id.
   Accept the transaction */
  rbrc.tr_ids.accepted_transaction = transaction_id;

  return(SUCCESS);
}

/*===========================================================================

FUNCTION   rrcrbrc_validate_and_check_integrity_for_srns_relocation

DESCRIPTION

  This function validates whether received rbrc message is due to SRNS relocation,
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
static uecomdef_status_e_type rrcrbrc_validate_and_check_integrity_for_srns_relocation
(
  rrc_cmd_type *cmd_ptr,
  /* Pointer to the RB Reconfig message        */
  rrc_RadioBearerReconfiguration *msg_ptr,
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

  if (TRUE == rrcrbrc_validate_srns_relocation_info(msg_ptr, msg_version))
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
        if (transaction_id == rbrc.tr_ids.rejected_transaction )
        {
          WRRC_MSG1_HIGH("Duplicate RBRC msg - rejected tr-id%d match!",
                   rbrc.tr_ids.rejected_transaction);
          /* Nothing else to do, just ignore the message and return */
        } 
        else
        {
          /* send a reconfig failure messge with invalid
          configuration and 
          continue the same substate with any ongoing procedure */
          rbrc.tr_ids.rejected_transaction = transaction_id;
          if (ip_check_status == RRCSMC_SRNS_SECURITY_INFO_INVALID)
          {
            rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          }
          else
          {
            rbrc.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
          }
          /*Check whether Cell Update is going on */
          if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
               (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
          {
            WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
            (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBRC);
            /* Wait until Cell update procedure is completed */
            rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF; 
          }
          else
          {
            send_rbrc_failure_message( rbrc.status.failure_status,
                                            rbrc.status.prot_err_cause );
          }
        }
        /*do not clear the procedure or change the rbrc substate because
        some other rbrc procedure might be in progress. Just return from
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
FUNCTION   rrcrbrc_process_after_oc_config_set_r8

DESCRIPTION
  This function processes the R8 RBRC OTA after OC is set by this OTA. So if we call this function, it means
  that ORDERED_CONFIG_SET is returned while setting OC by RBRC OTA.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void rrcrbrc_process_after_oc_config_set_r8
(
  rrc_DL_DCCH_Message * dcch_msg_ptr, 
  rrc_RadioBearerReconfiguration_r8_IEs  *rbrc_ptr
)
{
  rrc_state_e_type                         current_state;       /* current RRC State  */
  rrcrb_directed_cell_info_type        directed_cell_info; /* directed cell info struct */
  rrc_RadioBearerReconfiguration   *msg_ptr; /* Pointer to the RB Reconfig message        */
  rrc_int_c_rnti_type                      c_rnti;
  rrc_int_u_rnti_type                      u_rnti;
  rrcrb_rnti_update_info_type         rnti_update_info;   /* to hold rnti update info */

  /* Get the pointer to the actual RB Reconfig message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerReconfiguration);

  if(rrc_sib_is_current_event_sib_read_in_dch())
  {
    WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
    rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
  }

  current_state = rrc_get_state(); 

  directed_cell_info.freq_info_present = FALSE;
  directed_cell_info.pscr_present = FALSE;  

  /* Populate the local variables to indicate if Re-direction info is present */
  if((current_state==RRC_STATE_CELL_FACH) && 
    ((rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
    ||(rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
    ||(rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)))
  {
    if((RRCRB_R8_MSG_IE_PRESENT(rbrc_ptr,rrc_RadioBearerReconfiguration_r8_IEs,frequencyInfo)) &&
      (rbrc_ptr->frequencyInfo.modeSpecificInfo.t == T_rrc_FrequencyInfo_modeSpecificInfo_fdd))
    {
      directed_cell_info.freq_info_present = TRUE;
      directed_cell_info.uarfcn_dl  = rbrc_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
    }
  
    if(rrcrbrc_is_pcpich_info_present(msg_ptr, MSG_REL8))
    {
      directed_cell_info.pscr_present = TRUE;
      directed_cell_info.pscr = rbrc_ptr->dl_InformationPerRL_List.elem[0].
                  modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
    }
    /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
          FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
    if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
      (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
    {
      WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
      if(rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
      {
        rrcrbrc_send_initiate_cu_flag = TRUE;
        rrcrbrc_delete_crnti          = TRUE;
      }
      else
      {
        rbrc.directed_cell_info = TRUE;
      }
    }  
  }

  /*DCH->DCH, FACH->FACH, FACH->DCH*/
  if ((current_state==rbrc.state_indicated_by_msg) ||((current_state==RRC_STATE_CELL_FACH) && 
    (rbrc.state_indicated_by_msg==RRC_STATE_CELL_DCH)))
  {
    /*check whether any frequency info is present in the message and transition is 
    CELL_FACH->CELL_FACH. In this case initiate a cell update after channel config is 
    successfully completed */
    if ((current_state==RRC_STATE_CELL_FACH)&& (rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
    {
      /*call the function that evaluates the directed cell info and set the flags accordingly */
      if((rrcrbrc_send_initiate_cu_flag == FALSE) && 
        (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
      {
        if(RRCRB_R8_MSG_IE_PRESENT(rbrc_ptr,rrc_RadioBearerReconfiguration_r8_IEs,new_C_RNTI))
        {
          rbrc.new_crnti_valid = TRUE;
          rrc_translate_crnti(&rbrc_ptr->new_C_RNTI, &c_rnti);
          rbrc.new_crnti = (uint16)c_rnti;
        }
        else
        {
          rbrc.new_crnti_valid = FALSE;
        }
        /* Now copy OC to TOC */
        rrcllcoc_update_toc_with_oc_from_fach();
        /*Initiate the cell selection*/
        rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
                    RRC_TRANSITION_FROM_FACH,rbrc.state_indicated_by_msg);

        rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

        /*We can return from here. Because FACH->FACH is done.*/
        return;
      }
    } /* if CELL_FACH->CELL_FACH reconfiguration */


    /* Call the function that creates and sends a channel config request to LLC. */
    rrcrbrc_send_channel_config_req();
    
    /* Change the substate since we're waiting for the confirmation from LLC for the 
    channel configs */
    rbrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
  }

  /*DCH -> FACH*/
  else if ((current_state==RRC_STATE_CELL_DCH) && 
    (rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
  {
    if(RRCRB_R8_MSG_IE_PRESENT(rbrc_ptr,rrc_RadioBearerReconfiguration_r8_IEs,new_C_RNTI))
    {
      rbrc.new_crnti_valid = TRUE;
      rrc_translate_crnti(&rbrc_ptr->new_C_RNTI, &c_rnti);
      rbrc.new_crnti = (uint16)c_rnti;
    }
    else
    {
      rbrc.new_crnti_valid = FALSE;
    }

    /*Initiate the cell selection*/
    rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
              RRC_TRANSITION_FROM_DCH, RRC_STATE_CELL_FACH);

    rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
  
    /* Start T_305 [periodic cell update] timer */
    rrccu_start_t_305_timer();
  }

  /*DCH -> Cell_PCH*/
  else if ((current_state==RRC_STATE_CELL_DCH) && 
    (rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH))
  {
    rbrc.directed_cell_info = rrcrbrc_is_pcpich_info_present(msg_ptr, MSG_REL8);
    /*Send the RRCRBRC complete msg on old configuration */
    send_rbrc_complete_message(TRUE);
    /*wait for L2ack */
    rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
    rrcllc_print_trans_chnl_rate();
  }

  /*DCH -> URA_PCH*/
  else if ((current_state==RRC_STATE_CELL_DCH) && 
    (rbrc.state_indicated_by_msg==RRC_STATE_URA_PCH))
  {
    /* no need to check directed cell info in this case */
    /*Send the RRCRBRC complete msg on old configuration */
    send_rbrc_complete_message(TRUE);
    /*wait for L2ack */
    rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
    rrcllc_print_trans_chnl_rate();
  }

  /*FACH -> PCH*/
  else if ((current_state==RRC_STATE_CELL_FACH) && 
    ((rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
    (rbrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
  {
  /*call the function that evaluates the directed cell info and set the flags accordingly */
    if((rbrc.directed_cell_info == FALSE) && 
      (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
    {
      if(rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
      {
        rbrc.directed_cell_info = rrcrbrc_is_pcpich_info_present(msg_ptr, MSG_REL8);
      }
      rbrc.initiate_cell_selection_trans_from_fach = TRUE;
    }
  
    /*Send the RRCRBRC complete msg on old configuration */
    /* check whether new C-RNTI was present in the message. In that case we have to configure 
    MAC with new C-RNTI before sending the response message */
    if(RRCRB_R8_MSG_IE_PRESENT(rbrc_ptr,rrc_RadioBearerReconfiguration_r8_IEs,new_C_RNTI))
    {
      rrc_translate_crnti(&rbrc_ptr->new_C_RNTI, &c_rnti);
      rnti_update_info.crnti_action = RRC_RNTI_UPDATE;
      rnti_update_info.crnti        = (uint16)c_rnti;

      /* check whether URNTI is also present in the message */
      if(RRCRB_R8_MSG_IE_PRESENT(rbrc_ptr,rrc_RadioBearerReconfiguration_r8_IEs,new_U_RNTI))
      {
        rrc_translate_urnti(&rbrc_ptr->new_U_RNTI, &u_rnti);
        rnti_update_info.urnti_action = RRC_RNTI_UPDATE; 
        rnti_update_info.urnti        = u_rnti; 
      }
      else
      {
        rnti_update_info.urnti_action = RRC_RNTI_NOCHANGE; 
      }

      rnti_update_info.procedure                = RRC_PROCEDURE_RBRC;
      rnti_update_info.rlc_re_establish_srb  = FALSE;
      rnti_update_info.rlc_re_establish_rab  = FALSE;
      rnti_update_info.cnf_reqd                  = TRUE;

      /* Send a RNTI Update Request */
      rrcrb_send_rnti_update_req(rnti_update_info);

      /* wait for the RNTI update confirm before sending the response message */
      rbrc.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
    }
    else
    {
      send_rbrc_complete_message(TRUE);
      /*wait for L2ack */
      rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
      rrcllc_print_trans_chnl_rate();
    }
  }
  return;
}

/*===========================================================================
FUNCTION   rrcrbrc_process_rb_reconfig_list_r8

DESCRIPTION
  This function proceses RB Reconfig List for R8 RBRC OTA.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void rrcrbrc_process_rb_reconfig_list_r8
(
  rrc_RB_InformationReconfigList_r8  *rb_info_reconfig_list_r8_ptr 
)
{
  uint8  localrb_count = 0;
  rrcrbrc_rlc_stop_cont_e_type  rlc_stop_cont_e_value = RB_STOP_CONTINUE_ABSENT;  
  rrc_RB_InformationReconfig_r8 * rb_info_reconfig_ptr=NULL;
  /*store the rb ids tobe released */
  rbrc.rbs_tobe_reconfigured.is_valid = TRUE;

  while (rb_info_reconfig_list_r8_ptr->n > localrb_count)
  {
      // Initialize the enum to this value.
      rlc_stop_cont_e_value = RB_STOP_CONTINUE_ABSENT;
      rb_info_reconfig_ptr = &rb_info_reconfig_list_r8_ptr->elem[localrb_count];
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr, 
          rrc_RB_InformationReconfig_r8,rb_StopContinue))
      {
        if (rb_info_reconfig_ptr->rb_StopContinue == rrc_RB_StopContinue_stopRB)
        {
          rlc_stop_cont_e_value = RB_STOP;
          WRRC_MSG1_HIGH("REL8: Stop recvd for RB %d", (uint16)(rb_info_reconfig_ptr->rb_Identity));
        }
        else if (rb_info_reconfig_ptr->rb_StopContinue == rrc_RB_StopContinue_continueRB)
        {
          rlc_stop_cont_e_value = RB_CONTINUE;
          WRRC_MSG1_HIGH("REL8: Continue recvd for RB %d", (uint16)(rb_info_reconfig_ptr->rb_Identity));
        }
      }

      rbrc.rbs_tobe_reconfigured.rb_id[localrb_count] = 
          rb_info_reconfig_ptr->rb_Identity;

      rbrc.rbs_tobe_reconfigured.rb_stop_continue[localrb_count] = rlc_stop_cont_e_value;

      rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] = rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] = 0;

      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,rrc_RB_InformationReconfig_r8,rlc_Info))&&
        (RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_ptr->rlc_Info,rrc_RLC_Info_r7,ul_RLC_Mode)))
      {
        rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] |= MODIFY_RB_CFG;
      }
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,rrc_RB_InformationReconfig_r8,rlc_Info))&&
        (RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_ptr->rlc_Info,rrc_RLC_Info_r7,dl_RLC_Mode)))
      {
        rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] |= MODIFY_RB_CFG;
      }
      if(rlc_stop_cont_e_value == RB_STOP)
      {
        rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] |= STOP_RB_CFG;
        rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] |= STOP_RB_CFG;
      }
      else if(rlc_stop_cont_e_value == RB_CONTINUE)
      {
        rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] |= CONTINUE_RB_CFG;
        rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] |= CONTINUE_RB_CFG;
      }

      localrb_count++;

  }

  rbrc.rbs_tobe_reconfigured.num_rbs = localrb_count;

  WRRC_MSG1_HIGH("Num of RBs tobe reconfigured:%d", localrb_count);
}

/*===========================================================================
FUNCTION   RRCRBRC_UPDATE_ORDERED_CONFIG_R8

DESCRIPTION
  This function processes the received Channel Config Confirmation
  from LLC. If the required Radio-bearers have been reconfig succesfully,
  and if any of the radio-bearers map to radio-access-bearers, this
  information needs to be given to the NAS entity for that RAB.
  
  Currently Circuit-Switched voice and Circuit-Switched data RABs are 
  supported. If a voice RAB has been assigned by the UTRAN, this function
  gets the RLC ids and the AMR mode information from LLC and sends this
  information to voice services. If a CS Data RAB has been assigned, the
  RLC ids for those RBs are sent to the data services entity. Then this
  function sends the RRC_SYNC_IND primitive to MM.
          
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void rrcrbrc_update_ordered_config_r8
(
  rrc_DL_DCCH_Message *dcch_msg_ptr,      /* Pointer to the RB Reconfig message */
  rrc_msg_ver_enum_type                   msg_version
)
{
  rrcllc_oc_status_e_type                oc_status;/* To hold ordered_config status  */
  /* Used to indicate if an error occured while setting ordered_config. */
  boolean                                      ordered_config_err_occured = FALSE; 

  /* We have to reconfigure RBs. First set ORDERED_CONFIG so that LLC can process the 
  necessary parameters. */
  oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_RBRC,
                     rbrc.state_indicated_by_msg, rrc_DL_DCCH_Message_PDU,
                     (void *)dcch_msg_ptr);
  
  switch (oc_status)
  {
    case ORDERED_CONFIG_SET:
      if(msg_version == MSG_REL8)
      {
      rrcrbrc_process_after_oc_config_set_r8(dcch_msg_ptr,
          &dcch_msg_ptr->message.u.radioBearerReconfiguration.u.later_than_r3.criticalExtensions.u.
          criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.
          radioBearerReconfiguration_r8);
      }
#ifdef FEATURE_WCDMA_REL9
      if(msg_version == MSG_REL9)
      {
        rrcrbrc_process_after_oc_config_set_r9(dcch_msg_ptr,
        &dcch_msg_ptr->message.u.radioBearerReconfiguration.u.later_than_r3.criticalExtensions.u.
        criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
        later_r8_criticalExtensions.u.r9.radioBearerReconfiguration_r9);
      }
#endif
#ifdef FEATURE_WCDMA_REL10
      #error code not present
#endif
      break;

    case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:
      rbrc.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
      /* Send the RB Release Failure Message */
      send_rbrc_failure_message( rbrc.status.failure_status,
                                 rbrc.status.prot_err_cause );

      /* Go back to initial substate */
      rbrc.curr_substate = RRCRB_INITIAL;

      /* Indicate that an error occured */
      ordered_config_err_occured = TRUE;
      break;

    case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:
      rbrc.status.failure_status =RRCRB_MSG_UNSUPPORTED_CONFIG;
      /* Send the RB Release Failure Message */
      send_rbrc_failure_message( rbrc.status.failure_status,
                                 rbrc.status.prot_err_cause );

      /* Go back to initial substate */
      rbrc.curr_substate = RRCRB_INITIAL;

      /* Indicate that an error occured */
      ordered_config_err_occured = TRUE;
      break;
      
      case ORDERED_CONFIG_CONFIGURATION_INVALID:

       rbrc.status.failure_status =RRCRB_MSG_INVALID_CONFIG;
       /* Send the RB Release Failure Message */
       send_rbrc_failure_message( rbrc.status.failure_status,
                                 rbrc.status.prot_err_cause
                               );
   
       /* Go back to initial substate */
       rbrc.curr_substate = RRCRB_INITIAL;
   
       /* Indicate that an error occured */
       ordered_config_err_occured = TRUE;
       break;
        
    case ORDERED_CONFIG_NOT_SET_OTHER:
      WRRC_MSG0_HIGH("LLC returned other failure!");
      /* When LLC sends this, there is a serious problem
      and we'll go back to disconnected state. No need
      to send the failure message here, we can just
      go back to initial substate */
      rbrc.curr_substate = RRCRB_INITIAL;

      /* Indicate that an error occured */
      ordered_config_err_occured = TRUE;
      break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME      
    case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("RBRC processing deferred due to LTA!");
    
        rbrc.curr_substate = RRCRB_INITIAL;
    
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
    default:

      WRRC_MSG0_ERROR("Unknown return value from LLC");
      /* Unknown error, just go back to initial substate */
      rbrc.curr_substate = RRCRB_INITIAL;

      /* Indicate that an error occured */
      ordered_config_err_occured = TRUE;
      break;
  }/* end switch */

  if (ordered_config_err_occured == TRUE)
  {
    /* No need to continue. Clear the procedure variables and return. */
    rrcrbrc_clear_procedure();
    return;
  }
  else
  {
    if (rbrc.rbrc_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }
  }
}



/*===========================================================================
FUNCTION   VALIDATE_RBRC_MESSAGE_R8

DESCRIPTION
  This function validates the received RB Reconfig Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a Radio Bearer Reconfig Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void validate_rbrc_message_r8
(
  rrc_RadioBearerReconfiguration_r8_IEs *rbrc_ptr /* Pointer to the R8 RB RC message */
)
{
#ifdef FEATURE_VOC_AMR_WB
  /*For CS RABs (if present) the codec type will be updated in rrc_ota_codec_mode_status
  even if RAB-InfoReconfig List isn't present*/
  mvs_mode_type nas_sync_ind=rrc_get_current_codec_mode();
#endif/*FEATURE_VOC_AMR_WB*/

  /* Assume the message is valid for now */
  rbrc.status.failure_status = RRCRB_MSG_VALID;

  /* Check the RRC State indicatior given by the message and 
     store it in the rbrc internal memory */
  switch (rbrc_ptr->rrc_StateIndicator)
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      rbrc.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      rbrc.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      rbrc.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;

    case rrc_RRC_StateIndicator_ura_PCH:
      rbrc.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;

    default:
      /* No other states are supported right now. */
      WRRC_MSG1_HIGH("Unsupported state %d in RBRC R8 Msg", rbrc_ptr->rrc_StateIndicator);
      rbrc.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
      break;
  }

  if (rbrc.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG)
  {
    return;
  }

  /*RRC_GKG: Find out whether any Reconfig OTA support Pre-Reconfig*/
  /* Not support to the pre-config option. */
  if (rbrc_ptr->specificationMode.t != 
      T_rrc_RadioBearerReconfiguration_r8_IEs_specificationMode_complete)
  {
    WRRC_MSG0_ERROR("REL8: Non-support preconfig t in RBRC");
    rbrc.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
    return;
  }

  /* Obtain U_RNTI if present for SRNS relocation.  */
  if(RRCRB_R8_MSG_IE_PRESENT(rbrc_ptr,rrc_RadioBearerReconfiguration_r8_IEs,new_U_RNTI))
  {
    /* check whether the SRNS relocation is within Cell_DCH or not.
       SRNS relocation information can come in reconfiguration messages
       only when the state transition is Cell_DCH->Cell_DCH */
    if (!((rbrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("REL8: SRNS reloc with invalid states cur_state %d, msg_state %d",
          rrc_get_state(), rbrc.state_indicated_by_msg);
      rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    /* message with valid SNRS relocation info */
    else
    {
      /* Set the flag that indicates that this procedure involves SRNS relocation */
      rbrc.rbrc_for_srns_relocation = TRUE;

      /*store the new U-RNTI */
      rrc_translate_urnti(&(rbrc_ptr->new_U_RNTI), &rbrc.new_urnti);
    }
  }

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch */
  rbrc.trans_to_dch_ciph_config_needed = FALSE;
  /*RRC_GKG: Even though, any inconsistency exists in Security parameters, we don't fail the OTA.
  Check whether it's as per Spec.*/
  if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    /* check to see if dl-dpch-common info is present while going to dch */
    if ((RRCRB_R8_MSG_IE_PRESENT(rbrc_ptr,rrc_RadioBearerReconfiguration_r8_IEs,dl_CommonInformation)) &&
        (RRC_MSG_COMMON_BITMASK_IE(rbrc_ptr->dl_CommonInformation,
         rrc_DL_CommonInformation_r8,dl_dpchInfoCommon)))
    {
      if (rbrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.t == 
          T_rrc_DL_CommonInformation_r8_dl_dpchInfoCommon_dl_DPCH_InfoCommon)
      {
        /* check if ciphering is active in mac for cs domain, if it is then,
          start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
        */
        if (rrcsmc_tm_ciph_active() == TRUE)
        {
          uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
          uecomdef_status_e_type loc_ret_val;

          WRRC_MSG0_HIGH("REL8: To-DCH-Step-1 config activated");

          loc_ret_val = rrcrb_get_mac_d_hfn((RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.dl_DPCH_InfoCommon,
		  	   rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
               &rbrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
               &mac_d_hfn_l);

          if(loc_ret_val == FAILURE)
          {
            WRRC_MSG0_HIGH("get_mac_d_hfn failed");
          }
          if (rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
          {
            WRRC_MSG0_HIGH("Processing Ciphering Config for HHO failed");
          }
          rbrc.trans_to_dch_ciph_config_needed = TRUE;
        }
        else
        {
          WRRC_MSG0_HIGH("REL8: To-DCH-Step-1 not needed");
        }
      }
    }
  }

  /*Check the RABs in the List.
  1. Make sure that the given RABs are in EST_RABs.
  2. Update the NAS-Sync Ind for CS RABs, if present.*/
  if (rbrc_ptr->specificationMode.u.complete.m.rab_InformationReconfigListPresent)
  {
    rbrc.status.failure_status = rrc_update_rab_info_reconfig_param_r8(
    &rbrc_ptr->specificationMode.u.complete.rab_InformationReconfigList, &rbrc.rabs 
#ifdef FEATURE_VOC_AMR_WB
                                             ,&nas_sync_ind
#endif/*FEATURE_VOC_AMR_WB*/
                                           );
   if( rbrc.status.failure_status != RRCRB_MSG_VALID) 
   {
      return;
    }
  }

#ifdef FEATURE_VOC_AMR_WB
  rrc_set_ordered_codec_mode(nas_sync_ind);
#endif/*FEATURE_VOC_AMR_WB*/

  /*Do the validations for RB-Reconfig List
  1. The RB-Id should be present in EST_RABs
  2. If RB is being STOP/CONTINUE, then it can't be SRB1/2 and TM URBs
  3. Check for PDCP Info SRNS relocation etc.*/
  if((T_rrc_RadioBearerReconfiguration_r8_IEs_specificationMode_complete ==
     (rbrc_ptr->specificationMode.t) &&
      rbrc_ptr->specificationMode.u.complete.m.rb_InformationReconfigListPresent))
  {
    /* Save the linked list head pointer */
    if (FAILURE == rrcllc_validate_rb_info_reconfig_list_r8(
        &rbrc_ptr->specificationMode.u.complete.rb_InformationReconfigList))
    {
      rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }

  /*Cell_PCH/URA_PCH specific validations - 
     1. UTRAN DRX Cycle Length Coeff has to be present.
     2.  If state is URA_PCH, and URA-ID is present then check for it's length
  */
  if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
    /* If UTRAN DRX coefficient is not present then it is invalid configuration */
    if (!(RRCRB_R8_MSG_IE_PRESENT(rbrc_ptr,rrc_RadioBearerReconfiguration_r8_IEs,
      utran_DRX_CycleLengthCoeff)))
    {
      WRRC_MSG1_ERROR("REL8: UTRAN DRX Coef not preset, State:%d", rbrc.state_indicated_by_msg);
      rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    if (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
    {
      /*If UE state is Cell_FACH then set the flag rrccu_ura_update_required if more than one URA-IDs are
      present in SIB2*/
      if (!(RRCRB_R8_MSG_IE_PRESENT(rbrc_ptr,rrc_RadioBearerReconfiguration_r8_IEs,ura_Identity)))
      {
        WRRC_MSG0_HIGH("REL8: URA-ID not present for URA_PCH state.");
        rrccu_clear_ura_id();
      }
      else
      {
        /*URA ID present in the message set ura_identity variable with this value */
        if (rbrc_ptr->ura_Identity.numbits == 16)
        {
          rrccu_update_ura_id (&(rbrc_ptr->ura_Identity));
        }
        else
        {
          WRRC_MSG1_ERROR("REL8: Invalid URA-ID length%d", rbrc_ptr->ura_Identity.numbits);
          rbrc.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
          rbrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
          return;
        }
      }
    }
  } // if RRC_STATE_CELL_PCH or RRC_STATE_URA_PCH

}/* end function validate_rbrc_message_r7 */

/*===========================================================================
FUNCTION   process_rbrc_message_r8

DESCRIPTION
  This function processes the received RB Reconfig Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a Radio Bearer Reconfig Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to configure the Radio Bearers given by the message.
    
    If ORDERED_CONFIG could not be set, a Radio Bearer Reconfig Failure message
    will be sent to UTRAN.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void process_rbrc_message_r8
(
  rrc_RRC_TransactionIdentifier transaction_id,
  rrc_DL_DCCH_Message *dcch_msg_ptr  ,
  rrc_RadioBearerReconfiguration_r8_IEs *rbrc_ptr
)
{
  rrc_CN_InformationInfo_r6         *cn_info_ptr = NULL; 
  uint32                                       localrb_count;
  boolean                                     is_valid_rbid = TRUE;
  uint16                                       invalid_rbid = 0xff;
  rrc_msg_ver_enum_type                   msg_version = MSG_REL8;
  rrc_RB_InformationAffectedList_r8  *rb_info_affected_list_r8_ptr = NULL;

  WRRC_MSG1_MED("REL8: Recvd RBRC Msg sub_st%d", rbrc.curr_substate);

  if (FAILURE == rrcrbrc_check_and_send_failure_for_simul_reconfig(transaction_id))
  {
    return;
  }

  if(rbrc_ptr == NULL)
  {
    return;
  }
  /* Validate the Message to make sure there is no invalid or incompatible configuration given by the message */
  validate_rbrc_message_r8( rbrc_ptr );

  if ( rbrc.status.failure_status != RRCRB_MSG_VALID )
  {
    /* RB Message was invalid. Send the RB Reconfig Failure Message */
    send_rbrc_failure_message(rbrc.status.failure_status, rbrc.status.prot_err_cause);

    /* Make sure we're back in the initial sub-state */
    rbrc.curr_substate = RRCRB_INITIAL;
    rrcrbrc_clear_procedure();
    return;
  }

  if(RRCRB_R8_MSG_IE_PRESENT(rbrc_ptr,rrc_RadioBearerReconfiguration_r8_IEs,cn_InformationInfo))
  {
    cn_info_ptr = &(rbrc_ptr->cn_InformationInfo);

    /* CN info presents. */
    if (cn_info_ptr != NULL)
    {
      WRRC_MSG0_HIGH("REL8: Saving CN info for NAS");
      rbrc.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(cn_info_ptr);
      if (rbrc.mm_cmd_ptr == NULL)
      {
        WRRC_MSG0_ERROR("Could not build SERVICE_IND cmd");
      }
      else
      {
        /* set cn_info_saved flag */
        rbrc.cn_info_saved = TRUE;
      }
    }
  }

  if((RRCRB_R8_MSG_IE_PRESENT(rbrc_ptr,rrc_RadioBearerReconfiguration_r8_IEs,
    ueMobilityStateIndicator)) && (rbrc_ptr->ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected))
  {
    WRRC_MSG0_HIGH("REL8: High Mobility Ind Set to TRUE in RBRC.");
    rbrc.high_mobility_ind = TRUE;
  }

#ifdef FEATURE_WCDMA_REL7_SPECRS
  /*If RBRC OTA provides "supportForChangeOfUE_Capability" IE, we need to store it in
        "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
        "rrc_rnc_cap_change_support" can be updated accordingly. */  
  if(RRCRB_R8_MSG_IE_PRESENT(rbrc_ptr,rrc_RadioBearerReconfiguration_r8_IEs,
    supportForChangeOfUE_Capability))
  {
    WRRC_MSG1_HIGH("REL8: Support for Change of UE Cap is %d",rbrc_ptr->supportForChangeOfUE_Capability);
    if(rbrc_ptr->supportForChangeOfUE_Capability == TRUE)
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

  /*If RBRC OTA provides "responseToChangeOfUE_Capability_present" IE, we need to store it so that later
       if procedure is successful, RBRC need to notify the COUEC procedure  about this IE.*/
  rrc_change_of_ue_cap_response = FALSE;       
  
  if(RRCRB_R8_MSG_IE_PRESENT(rbrc_ptr,rrc_RadioBearerReconfiguration_r8_IEs,
    responseToChangeOfUE_Capability))
  {
    WRRC_MSG0_HIGH("REL8: Response for Change of UE Cap is given");
    rrc_change_of_ue_cap_response = TRUE;
  }
#endif /*FEATURE_WCDMA_REL7_SPECRS*/

  /* Process madatory RB information to reconfigure */
  if ((rbrc_ptr->specificationMode.u.complete.m.rb_InformationReconfigListPresent) &&
      rbrc_ptr->specificationMode.u.complete.rb_InformationReconfigList.n != 0 )
  {
    /*Processing of TN-ReconfigList will not fail because we have already validated it.*/
    rrcrbrc_process_rb_reconfig_list_r8(&rbrc_ptr->specificationMode.u.complete.rb_InformationReconfigList);

  /* update mac rab status to get tb size configuration*/
    rrcrb_update_mac_rab_status(
    RRC_PROCEDURE_RBRC,
    RRC_PS_DOMAIN_CN_ID,
    NULL,
    TRUE);
  }

  /* Continue further if there are no invalid Rbids in recnfig list */
  /*Store the rb ids tobe affected */
  if (rbrc_ptr->specificationMode.u.complete.m.rb_InformationAffectedListPresent)
  {
    rbrc.rbs_tobe_affected.is_valid = TRUE;

    rb_info_affected_list_r8_ptr = &rbrc_ptr->specificationMode.u.complete.rb_InformationAffectedList;
    localrb_count = 0;
    while ( rb_info_affected_list_r8_ptr->n > localrb_count )
    {
      /*first check whether this rb-id exists in established_rab variable or not */
      if (! validate_rb_id (rb_info_affected_list_r8_ptr->elem[localrb_count].rb_Identity))
      {
        /*rab-id does not exist in established_rab*/
        rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        is_valid_rbid = FALSE;
        invalid_rbid = (uint16)(rb_info_affected_list_r8_ptr->elem[localrb_count].rb_Identity);
        break;
      }
      rbrc.rbs_tobe_affected.rb_id[localrb_count] = rb_info_affected_list_r8_ptr->elem[localrb_count].rb_Identity;
      localrb_count++;
    }
    rbrc.rbs_tobe_affected.num_rbs= localrb_count;

    WRRC_MSG1_HIGH("REL8: Num of RBs to be affected:%d", localrb_count);
  }

  if (!is_valid_rbid)
  {
    /*one of the rab-id does not exist in established_rab*/
    WRRC_MSG1_ERROR("REL8: RB-id %d is not in EST_RABs", invalid_rbid);

    /* Send the RB Reconfig Failure Message */
    send_rbrc_failure_message( rbrc.status.failure_status, rbrc.status.prot_err_cause);

    /* Make sure we're back in the initial sub-state */
    rrcrbrc_clear_procedure();
    rbrc.curr_substate = RRCRB_INITIAL;
    return;
  }

  /*Update the OS with the OTA Content.*/
  rrcrbrc_update_ordered_config_r8 (dcch_msg_ptr, msg_version);
  return;
} /* process_rbrc_message_r7 */

#ifdef FEATURE_WCDMA_REL9

/*===========================================================================
FUNCTION   rrcrbrc_process_after_oc_config_set_r9

DESCRIPTION
  This function processes the R9 RBRC OTA after OC is set by this OTA. So if we call this function, it means
  that ORDERED_CONFIG_SET is returned while setting OC by RBRC OTA.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rrcrbrc_process_after_oc_config_set_r9
(
  rrc_DL_DCCH_Message * dcch_msg_ptr, 
  rrc_RadioBearerReconfiguration_r9_IEs  *rbrc_ptr
)
{
  rrc_state_e_type                         current_state;       /* current RRC State  */
  rrcrb_directed_cell_info_type        directed_cell_info; /* directed cell info struct */
  rrc_RadioBearerReconfiguration   *msg_ptr; /* Pointer to the RB Reconfig message        */
  rrc_int_c_rnti_type                      c_rnti;
  rrc_int_u_rnti_type                      u_rnti;
  rrcrb_rnti_update_info_type         rnti_update_info;   /* to hold rnti update info */

  /* Get the pointer to the actual RB Reconfig message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerReconfiguration);

  if(rrc_sib_is_current_event_sib_read_in_dch())
  {
    WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
    rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
  }

  current_state = rrc_get_state(); 

  directed_cell_info.freq_info_present = FALSE;
  directed_cell_info.pscr_present = FALSE;  

  /* Populate the local variables to indicate if Re-direction info is present */
  if((current_state==RRC_STATE_CELL_FACH) && 
    ((rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
    ||(rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
    ||(rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)))
  {
    if((RRCRB_R9_MSG_IE_PRESENT(rbrc_ptr,frequencyInfo)) &&
      (rbrc_ptr->frequencyInfo.modeSpecificInfo.t == T_rrc_FrequencyInfo_modeSpecificInfo_fdd))
    {
      directed_cell_info.freq_info_present = TRUE;
      directed_cell_info.uarfcn_dl  = rbrc_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
    }
  
    if(rrcrbrc_is_pcpich_info_present(msg_ptr, MSG_REL9))
    {
      directed_cell_info.pscr_present = TRUE;
      directed_cell_info.pscr = rbrc_ptr->dl_InformationPerRL_List.elem[0].
                  modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
    }
    /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
          FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
    if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
      (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
    {
      WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
      if(rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
      {
        rrcrbrc_send_initiate_cu_flag = TRUE;
        rrcrbrc_delete_crnti          = TRUE;
      }
      else
      {
        rbrc.directed_cell_info = TRUE;
      }
    }  
  }

  /*DCH->DCH, FACH->FACH, FACH->DCH*/
  if ((current_state==rbrc.state_indicated_by_msg) ||((current_state==RRC_STATE_CELL_FACH) && 
    (rbrc.state_indicated_by_msg==RRC_STATE_CELL_DCH)))
  {
    /*check whether any frequency info is present in the message and transition is 
    CELL_FACH->CELL_FACH. In this case initiate a cell update after channel config is 
    successfully completed */
    if ((current_state==RRC_STATE_CELL_FACH)&& (rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
    {
      /*call the function that evaluates the directed cell info and set the flags accordingly */
      if((rrcrbrc_send_initiate_cu_flag == FALSE) && 
        (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
      {
        if(RRCRB_R9_MSG_IE_PRESENT(rbrc_ptr,new_C_RNTI))
        {
          rbrc.new_crnti_valid = TRUE;
          rrc_translate_crnti(&rbrc_ptr->new_C_RNTI, &c_rnti);
          rbrc.new_crnti = (uint16)c_rnti;
        }
        else
        {
          rbrc.new_crnti_valid = FALSE;
        }
        /* Now copy OC to TOC */
        rrcllcoc_update_toc_with_oc_from_fach();
        /*Initiate the cell selection*/
        rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
                    RRC_TRANSITION_FROM_FACH,rbrc.state_indicated_by_msg);

        rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

        /*We can return from here. Because FACH->FACH is done.*/
        return;
      }
    } /* if CELL_FACH->CELL_FACH reconfiguration */

    /* Call the function that creates and sends a channel config request to LLC. */
    rrcrbrc_send_channel_config_req();
    
    /* Change the substate since we're waiting for the confirmation from LLC for the 
    channel configs */
    rbrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
  }

  /*DCH -> FACH*/
  else if ((current_state==RRC_STATE_CELL_DCH) && 
    (rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
  {
    if(RRCRB_R9_MSG_IE_PRESENT(rbrc_ptr,new_C_RNTI))
    {
      rbrc.new_crnti_valid = TRUE;
      rrc_translate_crnti(&rbrc_ptr->new_C_RNTI, &c_rnti);
      rbrc.new_crnti = (uint16)c_rnti;
    }
    else
    {
      rbrc.new_crnti_valid = FALSE;
    }

    /*Initiate the cell selection*/
    rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
              RRC_TRANSITION_FROM_DCH, RRC_STATE_CELL_FACH);

    rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
  
    /* Start T_305 [periodic cell update] timer */
    rrccu_start_t_305_timer();
  }

  /*DCH -> Cell_PCH*/
  else if ((current_state==RRC_STATE_CELL_DCH) && 
    (rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH))
  {
    rbrc.directed_cell_info = rrcrbrc_is_pcpich_info_present(msg_ptr, MSG_REL9);
    /*Send the RRCRBRC complete msg on old configuration */
    send_rbrc_complete_message(TRUE);
    /*wait for L2ack */
    rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
    rrcllc_print_trans_chnl_rate();
  }

  /*DCH -> URA_PCH*/
  else if ((current_state==RRC_STATE_CELL_DCH) && 
    (rbrc.state_indicated_by_msg==RRC_STATE_URA_PCH))
  {
    /* no need to check directed cell info in this case */
    /*Send the RRCRBRC complete msg on old configuration */
    send_rbrc_complete_message(TRUE);
    /*wait for L2ack */
    rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
    rrcllc_print_trans_chnl_rate();
  }

  /*FACH -> PCH*/
  else if ((current_state==RRC_STATE_CELL_FACH) && 
    ((rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
    (rbrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
  {
  /*call the function that evaluates the directed cell info and set the flags accordingly */
    if((rbrc.directed_cell_info == FALSE) && 
      (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
    {
      if(rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
      {
        rbrc.directed_cell_info = rrcrbrc_is_pcpich_info_present(msg_ptr, MSG_REL9);
      }
      rbrc.initiate_cell_selection_trans_from_fach = TRUE;
    }
  
    /*Send the RRCRBRC complete msg on old configuration */
    /* check whether new C-RNTI was present in the message. In that case we have to configure 
    MAC with new C-RNTI before sending the response message */
    if(RRCRB_R9_MSG_IE_PRESENT(rbrc_ptr,new_C_RNTI))
    {
      rrc_translate_crnti(&rbrc_ptr->new_C_RNTI, &c_rnti);
      rnti_update_info.crnti_action = RRC_RNTI_UPDATE;
      rnti_update_info.crnti        = (uint16)c_rnti;

      /* check whether URNTI is also present in the message */
      if(RRCRB_R9_MSG_IE_PRESENT(rbrc_ptr,new_U_RNTI))
      {
        rrc_translate_urnti(&rbrc_ptr->new_U_RNTI, &u_rnti);
        rnti_update_info.urnti_action = RRC_RNTI_UPDATE; 
        rnti_update_info.urnti        = u_rnti; 
      }
      else
      {
        rnti_update_info.urnti_action = RRC_RNTI_NOCHANGE; 
      }

      rnti_update_info.procedure                = RRC_PROCEDURE_RBRC;
      rnti_update_info.rlc_re_establish_srb  = FALSE;
      rnti_update_info.rlc_re_establish_rab  = FALSE;
      rnti_update_info.cnf_reqd                  = TRUE;

      /* Send a RNTI Update Request */
      rrcrb_send_rnti_update_req(rnti_update_info);

      /* wait for the RNTI update confirm before sending the response message */
      rbrc.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
    }
    else
    {
      send_rbrc_complete_message(TRUE);
      /*wait for L2ack */
      rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
      rrcllc_print_trans_chnl_rate();
    }
  }
  return;
}



/*===========================================================================
FUNCTION   VALIDATE_RBRC_MESSAGE_R9

DESCRIPTION
  This function validates the received RB Reconfig Message. This includes
  checking for unsupported or invalid configurations, unknown extensions
  and invalid IEs.

  If a Radio Bearer Reconfig Failure message needs to be sent due to
  a problem with the message, this procedure stores the failure cause
  to be used while sending the failure message.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void validate_rbrc_message_r9
(
  rrc_RadioBearerReconfiguration_r9_IEs *rbrc_ptr /* Pointer to the R8 RB RC message */
)
{
#ifdef FEATURE_VOC_AMR_WB
  /*For CS RABs (if present) the codec type will be updated in rrc_ota_codec_mode_status
  even if RAB-InfoReconfig List isn't present*/
  mvs_mode_type nas_sync_ind=rrc_get_current_codec_mode();
#endif/*FEATURE_VOC_AMR_WB*/

  /* Assume the message is valid for now */
  rbrc.status.failure_status = RRCRB_MSG_VALID;

  /* Check the RRC State indicatior given by the message and 
     store it in the rbrc internal memory */
  switch (rbrc_ptr->rrc_StateIndicator)
  {
    case rrc_RRC_StateIndicator_cell_DCH:
      rbrc.state_indicated_by_msg = RRC_STATE_CELL_DCH;
      break;

    case rrc_RRC_StateIndicator_cell_FACH:
      rbrc.state_indicated_by_msg = RRC_STATE_CELL_FACH;
      break;

    case rrc_RRC_StateIndicator_cell_PCH:
      rbrc.state_indicated_by_msg = RRC_STATE_CELL_PCH;
      break;

    case rrc_RRC_StateIndicator_ura_PCH:
      rbrc.state_indicated_by_msg = RRC_STATE_URA_PCH;
      break;

    default:
      /* No other states are supported right now. */
      WRRC_MSG1_HIGH("Unsupported state %d in RBRC R9 Msg", rbrc_ptr->rrc_StateIndicator);
      rbrc.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
      break;
  }

  if (rbrc.status.failure_status == RRCRB_MSG_UNSUPPORTED_CONFIG)
  {
    return;
  }

  /*RRC_GKG: Find out whether any Reconfig OTA support Pre-Reconfig*/
  /* Not support to the pre-config option. */
  if (rbrc_ptr->specificationMode.t != 
      T_rrc_RadioBearerReconfiguration_r9_IEs_specificationMode_complete)
  {
    WRRC_MSG0_ERROR("REL9: Non-support preconfig t in RBRC");
    rbrc.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
    return;
  }

  /* Obtain U_RNTI if present for SRNS relocation.  */
  if(RRCRB_R9_MSG_IE_PRESENT(rbrc_ptr,new_U_RNTI))
  {
    /* check whether the SRNS relocation is within Cell_DCH or not.
       SRNS relocation information can come in reconfiguration messages
       only when the state transition is Cell_DCH->Cell_DCH */
#ifndef FEATURE_WCDMA_DEFAULT_CFG_CELLFACH
    if (!((rbrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && 
          ( rrc_get_state() == RRC_STATE_CELL_DCH)))
    {
      WRRC_MSG2_ERROR("REL9: SRNS reloc with invalid states cur_state %d, msg_state %d",
          rrc_get_state(), rbrc.state_indicated_by_msg);
      rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    /* message with valid SNRS relocation info */
    else
#endif
    {
      /* Set the flag that indicates that this procedure involves SRNS relocation */
      rbrc.rbrc_for_srns_relocation = TRUE;

      /*store the new U-RNTI */
      rrc_translate_urnti(&(rbrc_ptr->new_U_RNTI), &rbrc.new_urnti);
    }
  }

  /* check if we are going to dch from a non-dch state and dl-dpch-common-info is present
     if it is and if TM ciphering bearers are active, then we need to do special processing
     for step-1 and step-2 ciphering restart while going to dch */
  rbrc.trans_to_dch_ciph_config_needed = FALSE;
  /*RRC_GKG: Even though, any inconsistency exists in Security parameters, we don't fail the OTA.
  Check whether it's as per Spec.*/
  if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_DCH) && ( rrc_get_state() != RRC_STATE_CELL_DCH))
  {
    /* check to see if dl-dpch-common info is present while going to dch */
    if ((RRCRB_R9_MSG_IE_PRESENT(rbrc_ptr,dl_CommonInformation)) &&
        (RRC_MSG_COMMON_BITMASK_IE(rbrc_ptr->dl_CommonInformation,
         rrc_DL_CommonInformation_r8,dl_dpchInfoCommon)))
    {
      if (rbrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.t == 
          T_rrc_DL_CommonInformation_r8_dl_dpchInfoCommon_dl_DPCH_InfoCommon)
      {
        /* check if ciphering is active in mac for cs domain, if it is then,
          start 2 tiered HHO procedure STEP_1 and STEP_2 for rl-re-establishment
        */
        if (rrcsmc_tm_ciph_active() == TRUE)
        {
          uint32 mac_d_hfn_l=RRCSMC_INVALID_HFN_VALUE;
          uecomdef_status_e_type loc_ret_val;

          WRRC_MSG0_HIGH("REL9: To-DCH-Step-1 config activated");

          loc_ret_val = rrcrb_get_mac_d_hfn((RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.dl_DPCH_InfoCommon,
		  	   rrc_DL_DPCH_InfoCommon_r6,mac_d_HFN_initial_value)),
               &rbrc_ptr->dl_CommonInformation.dl_dpchInfoCommon.u.dl_DPCH_InfoCommon->mac_d_HFN_initial_value,
               &mac_d_hfn_l);

          if(loc_ret_val == FAILURE)
          {
            WRRC_MSG0_HIGH("get_mac_d_hfn failed");
          }
          if (rrcsmc_process_cipher_config_for_hho(mac_d_hfn_l) == FAILURE)
          {
            WRRC_MSG0_HIGH("Processing Ciphering Config for HHO failed");
          }
          rbrc.trans_to_dch_ciph_config_needed = TRUE;
        }
        else
        {
          WRRC_MSG0_HIGH("REL9: To-DCH-Step-1 not needed");
        }
      }
    }
  }

  /*Check the RABs in the List.
  1. Make sure that the given RABs are in EST_RABs.
  2. Update the NAS-Sync Ind for CS RABs, if present.*/
  if (rbrc_ptr->specificationMode.u.complete.m.rab_InformationReconfigListPresent)
  {
    rbrc.status.failure_status = rrc_update_rab_info_reconfig_param_r8(
    &rbrc_ptr->specificationMode.u.complete.rab_InformationReconfigList, &rbrc.rabs 
#ifdef FEATURE_VOC_AMR_WB
                                             ,&nas_sync_ind
#endif/*FEATURE_VOC_AMR_WB*/
                                           );
   if( rbrc.status.failure_status != RRCRB_MSG_VALID) 
   {
      return;
    }
  }

#ifdef FEATURE_VOC_AMR_WB
  rrc_set_ordered_codec_mode(nas_sync_ind);
#endif/*FEATURE_VOC_AMR_WB*/

  /*Do the validations for RB-Reconfig List
  1. The RB-Id should be present in EST_RABs
  2. If RB is being STOP/CONTINUE, then it can't be SRB1/2 and TM URBs
  3. Check for PDCP Info SRNS relocation etc.*/
  if((T_rrc_RadioBearerReconfiguration_r9_IEs_specificationMode_complete ==
     (rbrc_ptr->specificationMode.t) &&
      rbrc_ptr->specificationMode.u.complete.m.rb_InformationReconfigListPresent))
  {
    /* Save the linked list head pointer */
    if (FAILURE == rrcllc_validate_rb_info_reconfig_list_r8(
        &rbrc_ptr->specificationMode.u.complete.rb_InformationReconfigList))
    {
      rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
  }

  /*Cell_PCH/URA_PCH specific validations - 
     1. UTRAN DRX Cycle Length Coeff has to be present.
     2.  If state is URA_PCH, and URA-ID is present then check for it's length
  */
  if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
  {
    /* If UTRAN DRX coefficient is not present then it is invalid configuration */
    if (!(RRCRB_R9_MSG_IE_PRESENT(rbrc_ptr,
      utran_DRX_CycleLengthCoeff)))
    {
      WRRC_MSG1_ERROR("REL9: UTRAN DRX Coef not preset, State:%d", rbrc.state_indicated_by_msg);
      rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      return;
    }
    if (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
    {
      /*If UE state is Cell_FACH then set the flag rrccu_ura_update_required if more than one URA-IDs are
      present in SIB2*/
      if (!(RRCRB_R9_MSG_IE_PRESENT(rbrc_ptr,ura_Identity)))
      {
        WRRC_MSG0_HIGH("REL9: URA-ID not present for URA_PCH state.");
        rrccu_clear_ura_id();
      }
      else
      {
        /*URA ID present in the message set ura_identity variable with this value */
        if (rbrc_ptr->ura_Identity.numbits == 16)
        {
          rrccu_update_ura_id (&(rbrc_ptr->ura_Identity));
        }
        else
        {
          WRRC_MSG1_ERROR("REL9: Invalid URA-ID length%d", rbrc_ptr->ura_Identity.numbits);
          rbrc.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
          rbrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
          return;
        }
      }
    }
  } // if RRC_STATE_CELL_PCH or RRC_STATE_URA_PCH

}/* end function validate_rbrc_message_r7 */


/*===========================================================================
FUNCTION   process_rbrc_message_r9

DESCRIPTION
  This function processes the received RB Reconfig Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a Radio Bearer Reconfig Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to configure the Radio Bearers given by the message.
    
    If ORDERED_CONFIG could not be set, a Radio Bearer Reconfig Failure message
    will be sent to UTRAN.
        
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void process_rbrc_message_r9
(
  rrc_RRC_TransactionIdentifier transaction_id,
  rrc_DL_DCCH_Message *dcch_msg_ptr  ,
  rrc_RadioBearerReconfiguration_r9_IEs *rbrc_ptr
)
{
  rrc_CN_InformationInfo_r6         *cn_info_ptr = NULL; 
  uint32                                       localrb_count;
  boolean                                     is_valid_rbid = TRUE;
  uint16                                       invalid_rbid = 0xff;
  rrc_msg_ver_enum_type                   msg_version = MSG_REL9;
  rrc_RB_InformationAffectedList_r8  *rb_info_affected_list_r8_ptr = NULL;

  WRRC_MSG1_MED("REL9: Recvd RBRC Msg sub_st%d", rbrc.curr_substate);

  if (FAILURE == rrcrbrc_check_and_send_failure_for_simul_reconfig(transaction_id))
  {
    return;
  }

  if(rbrc_ptr == NULL)
  {
    return;
  }
  /* Validate the Message to make sure there is no invalid or incompatible configuration given by the message */
  validate_rbrc_message_r9( rbrc_ptr );

  if ( rbrc.status.failure_status != RRCRB_MSG_VALID )
  {
    /* RB Message was invalid. Send the RB Reconfig Failure Message */
    send_rbrc_failure_message(rbrc.status.failure_status, rbrc.status.prot_err_cause);

    /* Make sure we're back in the initial sub-state */
    rbrc.curr_substate = RRCRB_INITIAL;
    rrcrbrc_clear_procedure();
    return;
  }

  if(RRCRB_R9_MSG_IE_PRESENT(rbrc_ptr,cn_InformationInfo))
  {
    cn_info_ptr = &(rbrc_ptr->cn_InformationInfo);

    /* CN info presents. */
    if (cn_info_ptr != NULL)
    {
      WRRC_MSG0_HIGH("REL9: Saving CN info for NAS");
      rbrc.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(cn_info_ptr);
      if (rbrc.mm_cmd_ptr == NULL)
      {
        WRRC_MSG0_ERROR("Could not build SERVICE_IND cmd");
      }
      else
      {
        /* set cn_info_saved flag */
        rbrc.cn_info_saved = TRUE;
      }
    }
  }

  if((RRCRB_R9_MSG_IE_PRESENT(rbrc_ptr,
    ueMobilityStateIndicator)) && (rbrc_ptr->ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected))
  {
    WRRC_MSG0_HIGH("REL9: High Mobility Ind Set to TRUE in RBRC.");
    rbrc.high_mobility_ind = TRUE;
  }

#ifdef FEATURE_WCDMA_REL7_SPECRS
  /*If RBRC OTA provides "supportForChangeOfUE_Capability" IE, we need to store it in
        "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
        "rrc_rnc_cap_change_support" can be updated accordingly. */  
  if(RRCRB_R9_MSG_IE_PRESENT(rbrc_ptr,
    supportForChangeOfUE_Capability))
  {
    WRRC_MSG1_HIGH("REL9: Support for Change of UE Cap is %d",rbrc_ptr->supportForChangeOfUE_Capability);
    if(rbrc_ptr->supportForChangeOfUE_Capability == TRUE)
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

  /*If RBRC OTA provides "responseToChangeOfUE_Capability_present" IE, we need to store it so that later
       if procedure is successful, RBRC need to notify the COUEC procedure  about this IE.*/
  rrc_change_of_ue_cap_response = FALSE;       
  
  if(RRCRB_R9_MSG_IE_PRESENT(rbrc_ptr,
    responseToChangeOfUE_Capability))
  {
    WRRC_MSG0_HIGH("REL9: Response for Change of UE Cap is given");
    rrc_change_of_ue_cap_response = TRUE;
  }
#endif /*FEATURE_WCDMA_REL7_SPECRS*/

  /* Process madatory RB information to reconfigure */
  if ((rbrc_ptr->specificationMode.u.complete.m.rb_InformationReconfigListPresent) &&
      rbrc_ptr->specificationMode.u.complete.rb_InformationReconfigList.n != 0 )
  {
    /*Processing of TN-ReconfigList will not fail because we have already validated it.*/
    rrcrbrc_process_rb_reconfig_list_r8(&rbrc_ptr->specificationMode.u.complete.rb_InformationReconfigList);

  /* update mac rab status to get tb size configuration*/
    rrcrb_update_mac_rab_status(
    RRC_PROCEDURE_RBRC,
    RRC_PS_DOMAIN_CN_ID,
    NULL,
    TRUE);
  }

  /* Continue further if there are no invalid Rbids in recnfig list */
  /*Store the rb ids tobe affected */
  if (rbrc_ptr->specificationMode.u.complete.m.rb_InformationAffectedListPresent)
  {
    rbrc.rbs_tobe_affected.is_valid = TRUE;

    rb_info_affected_list_r8_ptr = &rbrc_ptr->specificationMode.u.complete.rb_InformationAffectedList;
    localrb_count = 0;
    while ( rb_info_affected_list_r8_ptr->n > localrb_count )
    {
      /*first check whether this rb-id exists in established_rab variable or not */
      if (! validate_rb_id (rb_info_affected_list_r8_ptr->elem[localrb_count].rb_Identity))
      {
        /*rab-id does not exist in established_rab*/
        rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        is_valid_rbid = FALSE;
        invalid_rbid = (uint16)(rb_info_affected_list_r8_ptr->elem[localrb_count].rb_Identity);
        break;
      }
      rbrc.rbs_tobe_affected.rb_id[localrb_count] = rb_info_affected_list_r8_ptr->elem[localrb_count].rb_Identity;
      localrb_count++;
    }
    rbrc.rbs_tobe_affected.num_rbs= localrb_count;

    WRRC_MSG1_HIGH("REL9: Num of RBs to be affected:%d", localrb_count);
  }

  if (!is_valid_rbid)
  {
    /*one of the rab-id does not exist in established_rab*/
    WRRC_MSG1_ERROR("REL9: RB-id %d is not in EST_RABs", invalid_rbid);

    /* Send the RB Reconfig Failure Message */
    send_rbrc_failure_message( rbrc.status.failure_status, rbrc.status.prot_err_cause);

    /* Make sure we're back in the initial sub-state */
    rrcrbrc_clear_procedure();
    rbrc.curr_substate = RRCRB_INITIAL;
    return;
  }

  /*Update the OS with the OTA Content.*/
  rrcrbrc_update_ordered_config_r8 (dcch_msg_ptr, msg_version);
  return;
} /* process_rbrc_message_r7 */


#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif /* FEATURE_WCDMA_REL10 */

/*===========================================================================

FUNCTION   RRCRBRC_UPDATE_ORDERED_CONFIG_R5

DESCRIPTION

  This function processes the received Channel Config Confirmation
  from LLC. If the required Radio-bearers have been reconfig succesfully,
  and if any of the radio-bearers map to radio-access-bearers, this
  information needs to be given to the NAS entity for that RAB.
  
  Currently Circuit-Switched voice and Circuit-Switched data RABs are 
  supported. If a voice RAB has been assigned by the UTRAN, this function
  gets the RLC ids and the AMR mode information from LLC and sends this
  information to voice services. If a CS Data RAB has been assigned, the
  RLC ids for those RBs are sent to the data services entity. Then this
  function sends the RRC_SYNC_IND primitive to MM.
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbrc_update_ordered_config_r5
(
  rrc_DL_DCCH_Message *dcch_msg_ptr,      /* Pointer to the RB Reconfig message */
  rrc_msg_ver_enum_type                   msg_version
)
{
  rrc_state_e_type                       current_state;       /* current RRC State  */
  rrcllc_oc_status_e_type                oc_status;/* To hold ordered_config status          */
  boolean                                ordered_config_err_occured = FALSE; /* Used to indicate if an error
                                                                       occured while setting ordered_config. */
  rrc_int_c_rnti_type                    c_rnti;
  rrc_int_u_rnti_type                    u_rnti;
  rrcrb_rnti_update_info_type            rnti_update_info;   /* to hold rnti update info */
  rrc_RadioBearerReconfiguration         *msg_ptr; /* Pointer to the RB Reconfig message        */
  rrcrb_directed_cell_info_type          directed_cell_info; /* directed cell info struct */
  boolean                                crnti_present = FALSE;
  ASN1BitStr32                           *crnti_ptr = NULL;
  rrc_RadioBearerReconfiguration_r5_IEs  *rbrc_ptr = NULL;

  /* We have to reconfigure RBs. First set ORDERED_CONFIG
    so that LLC can process the necessary parameters. */
  /* Get the pointer to the actual RB Reconfig message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerReconfiguration);
  oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_RBRC,
                                         rbrc.state_indicated_by_msg,
                                         rrc_DL_DCCH_Message_PDU,
                                         (void *)dcch_msg_ptr
                                       );
  
  rbrc_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
             r5.radioBearerReconfiguration_r5;
  switch (oc_status)
  {
    case ORDERED_CONFIG_SET:
      if((rrc_sib_is_current_event_sib_read_in_dch()) &&
         (RRC_PROCEDURE_GPS == rrc_sib_get_proc_id_for_sib_read_in_dch()))
      {
        WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
        rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
      }
      current_state = rrc_get_state(); 
     
      directed_cell_info.freq_info_present = FALSE;
      directed_cell_info.pscr_present = FALSE;
     
      /* Populate the local variables to indicate if Re-direction info is present */
      if((current_state==RRC_STATE_CELL_FACH) && 
            ((rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
            ||(rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
            ||(rbrc.state_indicated_by_msg==RRC_STATE_URA_PCH)
            ))
      {
        if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r5_IEs,frequencyInfo)) &&
               (rbrc_ptr->frequencyInfo.modeSpecificInfo.t == 
                T_rrc_FrequencyInfo_modeSpecificInfo_fdd)
              )
        {
          directed_cell_info.freq_info_present = TRUE;
          directed_cell_info.uarfcn_dl =  rbrc_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
        }
           
 
        if (rrcrbrc_is_pcpich_info_present(msg_ptr, msg_version))
        {
          directed_cell_info.pscr_present = TRUE;
          directed_cell_info.pscr = rbrc_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.u
                                    .fdd->primaryCPICH_Info.primaryScramblingCode;
        }
        /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
              FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
        if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
          (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
        {
          WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
          if(rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
          {
            rrcrbrc_send_initiate_cu_flag = TRUE;
            rrcrbrc_delete_crnti          = TRUE;
          }
          else
          {
            rbrc.directed_cell_info = TRUE;
          }
        }             
      }
      if ((current_state==rbrc.state_indicated_by_msg) ||
          ((current_state==RRC_STATE_CELL_FACH) && 
           (rbrc.state_indicated_by_msg==RRC_STATE_CELL_DCH)))
      {
        /* CELL_DCH->CELL_DCH or CELL_FACH->CELL_FACH case. We can release the
        Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */
        
        /*check whether any frequency info is present in the message
        and transition is CELL_FACH->CELL_FACH. In this case
        initiate a cell update after channel config is successfully completed */
        if ((current_state==RRC_STATE_CELL_FACH)&& 
            (rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
        {
          /*call the function that evaluates the directed cell info and set the flags accordingly */
          if((rrcrbrc_send_initiate_cu_flag == FALSE) && 
            (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
          {
            if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r5_IEs,new_C_RNTI))
            {
              rbrc.new_crnti_valid = TRUE;
              rrc_translate_crnti(&rbrc_ptr->new_C_RNTI, &c_rnti);
              rbrc.new_crnti = (uint16)c_rnti;
            }
            else
            {
              rbrc.new_crnti_valid = FALSE;
            }
    
            rrcllcoc_update_toc_with_oc_from_fach();
            /*Initiate the cell selection*/
            rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
                                                       RRC_TRANSITION_FROM_FACH,
                                                       rbrc.state_indicated_by_msg);
    
    
                
                
            rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
    
  
            /* exit from the switch case */
            break;
          }
    
        } /* if CELL_FACH->CELL_FACH reconfiguration */
        /* Call the function that creates and sends a channel
          config request to LLC. */
        rrcrbrc_send_channel_config_req();

        /* Change the substate since we're waiting for the
           confirmation from LLC for the channel configs */
        rbrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
      }
      else if ((current_state==RRC_STATE_CELL_DCH) && 
               (rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
      {

        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r5_IEs,new_C_RNTI))
        {
          rbrc.new_crnti_valid = TRUE;
          rrc_translate_crnti(&rbrc_ptr->new_C_RNTI, &c_rnti);
          rbrc.new_crnti = (uint16)c_rnti;
        }
        else
        {
          rbrc.new_crnti_valid = FALSE;
        }

        /*Initiate the cell selection*/
        rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
                                                RRC_TRANSITION_FROM_DCH,
                                                rbrc.state_indicated_by_msg);
        rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

        /* Start T_305 [periodic cell update] timer */
        rrccu_start_t_305_timer();

      }
      else if ((current_state==RRC_STATE_CELL_DCH) && 
               (rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH))
      {
        rbrc.directed_cell_info = rrcrbrc_is_pcpich_info_present(msg_ptr, msg_version);
        /*Send the RRCRBRC complete msg on old configuration */
        send_rbrc_complete_message(TRUE);
        /*wait for L2ack */
        rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;

        rrcllc_print_trans_chnl_rate();
      }
      else if ((current_state==RRC_STATE_CELL_DCH) && 
               (rbrc.state_indicated_by_msg==RRC_STATE_URA_PCH))
      {
        /* no need to check directed cell info in this case */
        /*Send the RRCRBRC complete msg on old configuration */
        send_rbrc_complete_message(TRUE);
        /*wait for L2ack */
        rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;

        rrcllc_print_trans_chnl_rate();
      }
      else if ((current_state==RRC_STATE_CELL_FACH) && 
               ((rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
                (rbrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
      {
        /*call the function that evaluates the directed cell info and set the flags accordingly */
        if((rbrc.directed_cell_info == FALSE) && 
          (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
        {
            
          if(rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
          {
            rbrc.directed_cell_info = rrcrbrc_is_pcpich_info_present(msg_ptr, msg_version);
          }


          rbrc.initiate_cell_selection_trans_from_fach = TRUE;
        }
    

        /*Send the RRCRBRC complete msg on old configuration */
        /* check whether new C-RNTI was present in the message. In that case
        we have to configure MAC with new C-RNTI before sending the response 
        message */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r5_IEs,new_C_RNTI))
        {
          crnti_present = TRUE;
          crnti_ptr = &rbrc_ptr->new_C_RNTI;
        }

        if (crnti_present)
        {
          rrc_translate_crnti(crnti_ptr, &c_rnti);
          /* Send a RNTI Update Request */
          rnti_update_info.crnti_action = RRC_RNTI_UPDATE;
          rnti_update_info.crnti        = (uint16)c_rnti;
          /* check whether URNTI is also present in the message */
          if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r5_IEs,new_U_RNTI))
          {
            rrc_translate_urnti(&rbrc_ptr->new_U_RNTI, &u_rnti);
            rnti_update_info.urnti_action = RRC_RNTI_UPDATE; 
            rnti_update_info.urnti        = u_rnti; 
          }
          else
          {
            rnti_update_info.urnti_action = RRC_RNTI_NOCHANGE; 
          }
          rnti_update_info.procedure            = RRC_PROCEDURE_RBRC;
          rnti_update_info.rlc_re_establish_srb = FALSE;
          rnti_update_info.rlc_re_establish_rab = FALSE;
          rnti_update_info.cnf_reqd             = TRUE;
          rrcrb_send_rnti_update_req(rnti_update_info);
          /* wait for the RNTI update confirm before sending the response
          message */
          rbrc.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
        }
        else
        {
          send_rbrc_complete_message(TRUE);
          /*wait for L2ack */
          rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
          rrcllc_print_trans_chnl_rate();
        }
      }
      break;

    case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:
      rbrc.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
      /* Send the RB Release Failure Message */
      send_rbrc_failure_message( rbrc.status.failure_status,
                                 rbrc.status.prot_err_cause );

      /* Go back to initial substate */
      rbrc.curr_substate = RRCRB_INITIAL;

      /* Indicate that an error occured */
      ordered_config_err_occured = TRUE;
      break;

    case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

      rbrc.status.failure_status =RRCRB_MSG_UNSUPPORTED_CONFIG;
      /* Send the RB Release Failure Message */
      send_rbrc_failure_message( rbrc.status.failure_status,
                                 rbrc.status.prot_err_cause );

      /* Go back to initial substate */
      rbrc.curr_substate = RRCRB_INITIAL;

      /* Indicate that an error occured */
      ordered_config_err_occured = TRUE;
      break;

    case ORDERED_CONFIG_CONFIGURATION_INVALID:

        rbrc.status.failure_status =RRCRB_MSG_INVALID_CONFIG;
        /* Send the RB Release Failure Message */
        send_rbrc_failure_message( rbrc.status.failure_status,
                                  rbrc.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbrc.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

    case ORDERED_CONFIG_NOT_SET_OTHER:
      WRRC_MSG0_HIGH("LLC returned other failure!");
      /* When LLC sends this, there is a serious problem
      and we'll go back to disconnected state. No need
      to send the failure message here, we can just
      go back to initial substate */
      rbrc.curr_substate = RRCRB_INITIAL;

      /* Indicate that an error occured */
      ordered_config_err_occured = TRUE;
      break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME      
    case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("RBRC processing deferred due to LTA!");
    
        rbrc.curr_substate = RRCRB_INITIAL;
    
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
    default:

      WRRC_MSG0_ERROR("Unknown return value from LLC");
      /* Unknown error, just go back to initial substate */
      rbrc.curr_substate = RRCRB_INITIAL;

      /* Indicate that an error occured */
      ordered_config_err_occured = TRUE;
      break;
  }/* end switch */

  if ( ordered_config_err_occured == TRUE )
  {
    /* No need to continue. Clear the procedure
    variables and return. */
    rrcrbrc_clear_procedure();
    return;
  }
  else
  {
    if (rbrc.rbrc_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }
  }
}
/*===========================================================================

FUNCTION   rrcrbrc_check_and_get_primary_plmn_id

DESCRIPTION

  This function will check whether primary plmn id is present 
  in r3,r5 path of RBRC message. If present then it will return 
  primary plmn id.
        
DEPENDENCIES
  None.

RETURN VALUE
TRUE : IF Primary PLMN I.D is present in RBRC message in r3,r5 path
FFALSE: Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
static boolean rrcrbrc_check_and_get_primary_plmn_id
(
  /* Pointer to the RBRC message */
  rrc_RadioBearerReconfiguration *msg_ptr,
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
      /*Check to see if primary PLMN I.D is present in RBRC message ext
       * rrc_RadioBearerReconfiguration-v690ext which is present in 
       * v690NonCriticalExtensions in r99 path
       */
      if((msg_ptr->u.r3.m.v3aoNonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.v3aoNonCriticalExtensions.m.laterNonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.v3aoNonCriticalExtensions.laterNonCriticalExtensions.m.
         v4b0NonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.v3aoNonCriticalExtensions.laterNonCriticalExtensions.
           v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.v3aoNonCriticalExtensions.laterNonCriticalExtensions.
           v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v5d0NonCriticalExtenstionsPresent)&&
         (msg_ptr->u.r3.v3aoNonCriticalExtensions.laterNonCriticalExtensions.
           v4b0NonCriticalExtensions.v590NonCriticalExtensions.
           v5d0NonCriticalExtenstions.m.v690NonCriticalExtensionsPresent)&&
         (msg_ptr->u.r3.v3aoNonCriticalExtensions.laterNonCriticalExtensions.
           v4b0NonCriticalExtensions.v590NonCriticalExtensions.
           v5d0NonCriticalExtenstions.v690NonCriticalExtensions.
          radioBearerReconfiguration_v690ext.m.primary_plmn_IdentityPresent)
        )
      {
        primary_plmn_Id_present = TRUE;
        *primary_plmn_Identity = msg_ptr->u.r3.v3aoNonCriticalExtensions.laterNonCriticalExtensions.
           v4b0NonCriticalExtensions.v590NonCriticalExtensions.v5d0NonCriticalExtenstions.
           v690NonCriticalExtensions.radioBearerReconfiguration_v690ext.primary_plmn_Identity;
      }
      break;
    
    case MSG_REL5:
      /*Check to see if primary PLMN I.D is present in RBRC message ext
       * rrc_RadioBearerReconfiguration-v690ext which is present in 
       * v690NonCriticalExtensions in r5 path
       */
      if((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.m.v5d0NonCriticalExtenstionsPresent)&&
         (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.
           v5d0NonCriticalExtenstions.m.v690NonCriticalExtensionsPresent)&&
         (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.
           v5d0NonCriticalExtenstions.v690NonCriticalExtensions.
           radioBearerReconfiguration_v690ext.m.primary_plmn_IdentityPresent)
       )
      {
        primary_plmn_Id_present = TRUE;
        *primary_plmn_Identity =msg_ptr->u.later_than_r3.criticalExtensions.u.
            criticalExtensions.u.r5.v5d0NonCriticalExtenstions.v690NonCriticalExtensions.
           radioBearerReconfiguration_v690ext.primary_plmn_Identity;
      }
      break;
   
    default:
      break;
  }
  WRRC_MSG2_HIGH("primary_plmn_Id_present:%d,msg_version:%d",primary_plmn_Id_present,msg_version);
  return primary_plmn_Id_present;
}
/*===========================================================================

FUNCTION   rrcrbrc_process_high_mobility_ind_in_non_critical_ext

DESCRIPTION

  This function will process High Mobility Ind received in Non-critical ext
        
DEPENDENCIES
  None.

RETURN VALUE
None

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbrc_process_high_mobility_ind_in_non_critical_ext
(
  rrc_RadioBearerReconfiguration *msg_ptr,
  rrc_msg_ver_enum_type msg_version
)
{
  if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
  {
    if(msg_version == MSG_REL5)
    {
      if((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.m.v5d0NonCriticalExtenstionsPresent) &&
      (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.v5d0NonCriticalExtenstions.m.v690NonCriticalExtensionsPresent) &&
      (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.v5d0NonCriticalExtenstions.v690NonCriticalExtensions.m.
      v770NonCriticalExtensionsPresent) &&
      (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.v5d0NonCriticalExtenstions.v690NonCriticalExtensions.
      v770NonCriticalExtensions.radioBearerReconfiguration_v770ext.m.ueMobilityStateIndicatorPresent))
      {
        if(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.r5.v5d0NonCriticalExtenstions.v690NonCriticalExtensions.
            v770NonCriticalExtensions.radioBearerReconfiguration_v770ext.ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
        {
          rbrc.high_mobility_ind = TRUE;
        }
      }  
    }
    else if(msg_version == MSG_REL6)
    {
      if((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.v6b0NonCriticalExtensions.m.
          v6f0NonCriticalExtensionsPresent) &&
          (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.v6b0NonCriticalExtensions.
          v6f0NonCriticalExtensions.m.v770NonCriticalExtensionsPresent) &&
          (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.v6b0NonCriticalExtensions.
          v6f0NonCriticalExtensions.v770NonCriticalExtensions.radioBearerReconfiguration_v770ext.m.
           ueMobilityStateIndicatorPresent))
      {
        if(msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.v6b0NonCriticalExtensions.
            v6f0NonCriticalExtensions.v770NonCriticalExtensions.radioBearerReconfiguration_v770ext.ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
        {
          rbrc.high_mobility_ind = TRUE;
        }
      }
    }
    else if(msg_version == MSG_REL99)
    {
      if((msg_ptr->u.r3.m.v3aoNonCriticalExtensionsPresent) &&
      (msg_ptr->u.r3.v3aoNonCriticalExtensions.m.laterNonCriticalExtensionsPresent) &&
      (msg_ptr->u.r3.v3aoNonCriticalExtensions.laterNonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent) &&
      (msg_ptr->u.r3.v3aoNonCriticalExtensions.laterNonCriticalExtensions.
      v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent) &&
      (msg_ptr->u.r3.v3aoNonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.
      v5d0NonCriticalExtenstionsPresent) &&
      (msg_ptr->u.r3.v3aoNonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
      v5d0NonCriticalExtenstions.m.v690NonCriticalExtensionsPresent) &&
      (msg_ptr->u.r3.v3aoNonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
      v5d0NonCriticalExtenstions.v690NonCriticalExtensions.m.v770NonCriticalExtensionsPresent) &&
      (msg_ptr->u.r3.v3aoNonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
      v5d0NonCriticalExtenstions.v690NonCriticalExtensions.v770NonCriticalExtensions.radioBearerReconfiguration_v770ext.m.
        ueMobilityStateIndicatorPresent))
      {
        if(msg_ptr->u.r3.v3aoNonCriticalExtensions.laterNonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
            v5d0NonCriticalExtenstions.v690NonCriticalExtensions.v770NonCriticalExtensions.radioBearerReconfiguration_v770ext.
            ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected)
        {
          rbrc.high_mobility_ind = TRUE;
        }
      }
    }  
  }
  WRRC_MSG1_HIGH("High Mobility Ind Set to %d (0-Not set, 1-SET)in RBRC.",rbrc.high_mobility_ind);
}

/*===========================================================================

FUNCTION   PROCESS_RBRC_MESSAGE_R5

DESCRIPTION

  This function processes the received RB Reconfig Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a Radio Bearer Reconfig Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to configure the Radio Bearers given by the message.
    
    If ORDERED_CONFIG could not be set, a Radio Bearer Reconfig Failure message
    will be sent to UTRAN.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_rbrc_message_r5
(
  rrc_cmd_type *cmd_ptr
)
{

  rrc_DL_DCCH_Message                    *dcch_msg_ptr;      /* Pointer to the RB Reconfig message */
  rrc_RadioBearerReconfiguration         *msg_ptr; /* Pointer to the RB Reconfig message        */
  rrc_CN_InformationInfo                 *cn_info_ptr = NULL; 
  uint32                                  localrb_count;
  rrcrbrc_rlc_stop_cont_e_type            rlc_stop_cont_e_value = RB_STOP_CONTINUE_ABSENT;
  boolean                                 is_valid_rbid = TRUE;
  rrc_RB_Identity invalid_rbid = 0xff;
  rlc_lc_id_type                          lc_id;
  uecomdef_logch_mode_e_type              lc_mode;

  rrc_msg_ver_enum_type                   msg_version = MSG_REL5;
  rrc_RadioBearerReconfiguration_r5_IEs  *rbrc_ptr = NULL;
  rrc_RB_InformationReconfig_r5  *rb_info_reconfig_ptr = NULL;
  rrc_RB_InformationAffectedList_r5  *rb_info_affected_list_r5_ptr = NULL;

   /*Boolean used to indicate primary plmn i.d is present in OTA msg*/
  boolean primary_plmn_Id_present = FALSE;
   /*Variable to hold primary plmn i.d if present in OTA msg*/
  rrc_PLMN_Identity primary_plmn_Identity;

  WRRC_MSG1_HIGH("Recvd RBRC R5 Msg sub_st%d", rbrc.curr_substate);
  
  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;

  /* Get the pointer to the actual RB Reconfig message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerReconfiguration);

  if (FAILURE == 
      rrcrbrc_check_and_send_failure_for_simul_reconfig(
      msg_ptr->u.later_than_r3.rrc_TransactionIdentifier))
  {
    return;
  }

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  validate_rbrc_message_r5( msg_ptr );

  if ( rbrc.status.failure_status != RRCRB_MSG_VALID )
  {
    /* RB Message was invalid. */
    /* Send the RB Reconfig Failure Message */
    send_rbrc_failure_message( rbrc.status.failure_status,
                               rbrc.status.prot_err_cause
                             );

    /* Make sure we're back in the initial sub-state */
    rbrc.curr_substate = RRCRB_INITIAL;
    rrcrbrc_clear_procedure();
    return;
  }

  /* If the message is valid, proceed further */
  /* Check if there is CN information present. If so, we need
  to send the information to the Cell Change manager */
  rbrc_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
                        r5.radioBearerReconfiguration_r5;

  /* update mac rab status to get tb size configuration*/
  if (rbrc_ptr->specificationMode.u.complete.m.rb_InformationReconfigListPresent)
  {
    rrcrb_update_mac_rab_status(
    RRC_PROCEDURE_RBRC,
    RRC_PS_DOMAIN_CN_ID,
    NULL,
    TRUE);
  }
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,
  	rrc_RadioBearerReconfiguration_r5_IEs,cn_InformationInfo))
  {
    cn_info_ptr = &(rbrc_ptr->cn_InformationInfo);
  }

  /* CN info presents. */
  if (cn_info_ptr != NULL)
  {
    WRRC_MSG0_HIGH("Saving CN info for NAS");
    
      /*Check to see if primary PLMN I.D is present in RBRC message ext
       * rrc_RadioBearerReconfiguration-v690ext which is present in 
       * v690NonCriticalExtensions
       */
       primary_plmn_Id_present = rrcrbrc_check_and_get_primary_plmn_id(msg_ptr,
                                                                  &primary_plmn_Identity,MSG_REL5);
    
    rbrc.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info(cn_info_ptr,
                                                           primary_plmn_Id_present,
                                                           &primary_plmn_Identity);
    if (rbrc.mm_cmd_ptr == NULL)
    {
      WRRC_MSG0_ERROR("Could not build SERVICE_IND cmd");
    }
    else
    {
      /* set cn_info_saved flag */
      rbrc.cn_info_saved = TRUE;
    }
  }

  rrcrbrc_process_high_mobility_ind_in_non_critical_ext(msg_ptr,MSG_REL5);
 
  /* Process madatory RB information to reconfigure */
  /* Process madatory RB information to reconfigure */
  if ((rbrc_ptr->specificationMode.u.complete.m.rb_InformationReconfigListPresent) &&
      (rbrc_ptr->specificationMode.u.complete.rb_InformationReconfigList.n != 0 ))
  {
    /*store the rb ids tobe released */
    rbrc.rbs_tobe_reconfigured.is_valid = TRUE;
    
    localrb_count = 0;
    while (rbrc_ptr->specificationMode.u.complete.rb_InformationReconfigList.n >  localrb_count)
    {
      rb_info_reconfig_ptr = &rbrc_ptr->specificationMode.u.complete.rb_InformationReconfigList.elem[localrb_count];
      /*First check whether this rb-id exists in established_rab variable or not */
      if (! validate_rb_id((rb_info_reconfig_ptr->rb_Identity)))
      {
        /*rab-id does not exist in established_rab*/
        rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        is_valid_rbid = FALSE;
        invalid_rbid = rb_info_reconfig_ptr->rb_Identity;
        /*Quit the loop*/
        break;
      }

      // Initialize the enum to this value.
      rlc_stop_cont_e_value = RB_STOP_CONTINUE_ABSENT;

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,
          rrc_RB_InformationReconfig_r5,rb_StopContinue))
      {
        if ((uint16)(rb_info_reconfig_ptr->rb_Identity) <= 2)
        {
          /*rb-id does not exist in established_rab*/
          rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          is_valid_rbid = FALSE;
          invalid_rbid = rb_info_reconfig_ptr->rb_Identity;
          WRRC_MSG1_ERROR("RB ID %d can't be stopped/continued", invalid_rbid);
          /*Quit the loop*/
          break;
        }

        lc_id = rrclcm_get_ul_rlc_lc_id((rb_info_reconfig_ptr->rb_Identity));

        if (lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND)
        {
          /*rb-id does not exist in established_rab*/
          rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          is_valid_rbid = FALSE;
          invalid_rbid = rb_info_reconfig_ptr->rb_Identity;
          WRRC_MSG1_ERROR("No corresponding LC id for the RB %d",invalid_rbid);
          /*Quit the loop*/
          break;
        }

        if (rrclcm_get_ul_rlc_mode(lc_id, &lc_mode) == RRCLCM_FAILURE)
        {
          /*rb-id does not exist in established_rab*/
          rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          is_valid_rbid = FALSE;
          invalid_rbid = rb_info_reconfig_ptr->rb_Identity;
          WRRC_MSG1_ERROR("Invalid lc_mode for RB %d",invalid_rbid);
          /*Quit the loop*/
          break;
        }

        if (rb_info_reconfig_ptr->rb_StopContinue == rrc_RB_StopContinue_stopRB)
        {
          if (lc_mode == UE_MODE_TRANSPARENT)
          {
            // In case of transparent mode RB's just print an error message if stop/continue
            // is specified. Do not reject the rbreconfig message because of this.
            invalid_rbid = rb_info_reconfig_ptr->rb_Identity;
            WRRC_MSG2_HIGH("TM RB %d lc_id %d can't be stopped/contd", invalid_rbid,lc_id);
          }
          else
          {
            rlc_stop_cont_e_value = RB_STOP;
            WRRC_MSG1_HIGH("Stop recvd for RB %d", 
                     (uint16)(rb_info_reconfig_ptr->rb_Identity));
          }

        }
        else if (rb_info_reconfig_ptr->rb_StopContinue == rrc_RB_StopContinue_continueRB)
        {
          if (lc_mode == UE_MODE_TRANSPARENT)
          {
            // In case of transparent mode RB's just print an error message if stop/continue
            // is specified. Do not reject the rbreconfig message because of this.
            invalid_rbid = rb_info_reconfig_ptr->rb_Identity;
            WRRC_MSG2_HIGH("TM RB %d lc_id %d can't be stopped/contd", invalid_rbid,lc_id);
          }
          else
          {
            rlc_stop_cont_e_value = RB_CONTINUE;
            WRRC_MSG1_HIGH("Continue recvd for RB %d", 
                     (uint16)(rb_info_reconfig_ptr->rb_Identity));
          }
        }
      }

      rbrc.rbs_tobe_reconfigured.rb_id[localrb_count] = 
          rb_info_reconfig_ptr->rb_Identity;
      rbrc.rbs_tobe_reconfigured.rb_stop_continue[localrb_count] = rlc_stop_cont_e_value;
      rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] = rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] = 0;
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,
	  	rrc_RB_InformationReconfig_r5,rlc_Info))&&
        (RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_ptr->rlc_Info,
        rrc_RLC_Info_r5,ul_RLC_Mode)))
      {
        rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] |= MODIFY_RB_CFG;
      }
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,
	  	rrc_RB_InformationReconfig_r5,rlc_Info))&&
        (RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_ptr->rlc_Info,
        rrc_RLC_Info_r5,dl_RLC_Mode)))
      {
        rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] |= MODIFY_RB_CFG;
      }
      if(rlc_stop_cont_e_value == RB_STOP)
      {
        rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] |= STOP_RB_CFG;
        rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] |= STOP_RB_CFG;
      }
      else if(rlc_stop_cont_e_value == RB_CONTINUE)
      {
        rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] |= CONTINUE_RB_CFG;
        rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] |= CONTINUE_RB_CFG;
      }

      localrb_count++;
      ;
    }
    rbrc.rbs_tobe_reconfigured.num_rbs = localrb_count;
    WRRC_MSG1_HIGH("Num of RBs tobe reconfigured:%d", localrb_count);
  }

  /* Continue further if there are no invalid Rbids in recnfig list */
  if (is_valid_rbid)
  {
    /*Store the rb ids tobe affected */
    if (rbrc_ptr->specificationMode.u.complete.m.rb_InformationAffectedListPresent)
    {
      rbrc.rbs_tobe_affected.is_valid = TRUE;
      rb_info_affected_list_r5_ptr = 
        &rbrc_ptr->specificationMode.u.complete.rb_InformationAffectedList;
      localrb_count = 0;
      while ( rb_info_affected_list_r5_ptr->n >localrb_count)
      {
        /*first check whether this rb-id exists in established_rab variable or not */
        if (! validate_rb_id ((rb_info_affected_list_r5_ptr->elem[localrb_count].rb_Identity)))
        {
          /*rab-id does not exist in established_rab*/
          rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          is_valid_rbid = FALSE;
          invalid_rbid = (rb_info_affected_list_r5_ptr->elem[localrb_count].rb_Identity);
          /*one of the rab-id does not exist in established_rab*/
          WRRC_MSG1_ERROR("rb-id %d is not in established_rab", invalid_rbid);
          break;
        }
        rbrc.rbs_tobe_affected.rb_id[localrb_count] = 
          rb_info_affected_list_r5_ptr->elem[localrb_count].rb_Identity;
        localrb_count++;
      }
      rbrc.rbs_tobe_affected.num_rbs= localrb_count;
      WRRC_MSG1_HIGH("Num of RBs tobe affected:%d", localrb_count);
    }
  }

  if (!is_valid_rbid)
  {
    /*one of the rab-id does not exist in established_rab*/
    WRRC_MSG1_ERROR("rb-id %d is not in established_rab", invalid_rbid);
    /* Send the RB Reconfig Failure Message */
    send_rbrc_failure_message( rbrc.status.failure_status,
                               rbrc.status.prot_err_cause
                             );
    /* Make sure we're back in the initial sub-state */
    rrcrbrc_clear_procedure();
    rbrc.curr_substate = RRCRB_INITIAL;
    return;
  }
  /* For Cell_FACH or Cell_DCH continue with the ordered config setting */
  rrcrbrc_update_ordered_config_r5 (dcch_msg_ptr, msg_version);
}/* end function process_rbrc_message_r5 */



/*===========================================================================

FUNCTION   RRCRBRC_UPDATE_ORDERED_CONFIG_R6

DESCRIPTION

  This function processes the received Channel Config Confirmation
  from LLC. If the required Radio-bearers have been reconfig succesfully,
  and if any of the radio-bearers map to radio-access-bearers, this
  information needs to be given to the NAS entity for that RAB.
  
  Currently Circuit-Switched voice and Circuit-Switched data RABs are 
  supported. If a voice RAB has been assigned by the UTRAN, this function
  gets the RLC ids and the AMR mode information from LLC and sends this
  information to voice services. If a CS Data RAB has been assigned, the
  RLC ids for those RBs are sent to the data services entity. Then this
  function sends the RRC_SYNC_IND primitive to MM.
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbrc_update_ordered_config_r6
(
  rrc_DL_DCCH_Message *dcch_msg_ptr,      /* Pointer to the RB Reconfig message */
  rrc_msg_ver_enum_type                   msg_version
)
{
  rrc_state_e_type                       current_state;       /* current RRC State  */
  rrcllc_oc_status_e_type                oc_status;/* To hold ordered_config status          */
  boolean                                ordered_config_err_occured = FALSE; /* Used to indicate if an error
                                                                       occured while setting ordered_config. */
  rrc_int_c_rnti_type                    c_rnti;
  rrc_int_u_rnti_type                    u_rnti;
  rrcrb_rnti_update_info_type            rnti_update_info;   /* to hold rnti update info */
  rrc_RadioBearerReconfiguration         *msg_ptr; /* Pointer to the RB Reconfig message        */
  rrcrb_directed_cell_info_type          directed_cell_info; /* directed cell info struct */
  boolean                                crnti_present = FALSE;
  ASN1BitStr32                           *crnti_ptr = NULL;
  rrc_RadioBearerReconfiguration_r6_IEs  *rbrc_ptr = NULL;

  /* We have to reconfigure RBs. First set ORDERED_CONFIG
    so that LLC can process the necessary parameters. */
  /* Get the pointer to the actual RB Reconfig message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerReconfiguration);
  oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_RBRC,
                                         rbrc.state_indicated_by_msg,
                                         rrc_DL_DCCH_Message_PDU,
                                         (void *)dcch_msg_ptr
                                       );
  
  rbrc_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
             criticalExtensions.u.r6.radioBearerReconfiguration_r6;
  switch (oc_status)
  {
    case ORDERED_CONFIG_SET:
      if((rrc_sib_is_current_event_sib_read_in_dch()) &&
         (RRC_PROCEDURE_GPS == rrc_sib_get_proc_id_for_sib_read_in_dch()))
       {
         WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
         rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
       }

      current_state = rrc_get_state(); 
      directed_cell_info.freq_info_present = FALSE;
      directed_cell_info.pscr_present = FALSE;  
      /* Populate the local variables to indicate if Re-direction info is present */
      if((current_state==RRC_STATE_CELL_FACH) && 
            ((rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
            ||(rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
            || (rbrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
      {
        if ((RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r6_IEs,frequencyInfo)) &&
               (rbrc_ptr->frequencyInfo.modeSpecificInfo.t == 
                T_rrc_FrequencyInfo_modeSpecificInfo_fdd)
              )
        {
          directed_cell_info.freq_info_present = TRUE;
          directed_cell_info.uarfcn_dl         = 
          rbrc_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
        }
        if (rrcrbrc_is_pcpich_info_present(msg_ptr, msg_version))
        {
          directed_cell_info.pscr_present = TRUE;
          directed_cell_info.pscr = rbrc_ptr->dl_InformationPerRL_List.elem[0].modeSpecificInfo.u
                                    .fdd->primaryCPICH_Info.primaryScramblingCode;
        }
        /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
             FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
        if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
          (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
        {
          WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
          if(rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
          {
            rrcrbrc_send_initiate_cu_flag = TRUE;
            rrcrbrc_delete_crnti          = TRUE;
          }
          else
          {
            rbrc.directed_cell_info = TRUE;
          }
        }   
      }
      if ((current_state==rbrc.state_indicated_by_msg) ||
          ((current_state==RRC_STATE_CELL_FACH) && 
           (rbrc.state_indicated_by_msg==RRC_STATE_CELL_DCH)))
      {
        /* CELL_DCH->CELL_DCH or CELL_FACH->CELL_FACH case. We can release the
        Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */

        /*check whether any frequency info is present in the message
        and transition is CELL_FACH->CELL_FACH. In this case
        initiate a cell update after channel config is successfully completed */
        if ((current_state==RRC_STATE_CELL_FACH)&& 
            (rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
        {
          /*call the function that evaluates the directed cell info and set the flags accordingly */
          if((rrcrbrc_send_initiate_cu_flag == FALSE) && 
            (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
          {
            if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r6_IEs,
				                         new_C_RNTI))
            {
              rbrc.new_crnti_valid = TRUE;
              rrc_translate_crnti(&rbrc_ptr->new_C_RNTI, &c_rnti);
              rbrc.new_crnti = (uint16)c_rnti;
            }
            else
            {
              rbrc.new_crnti_valid = FALSE;
            }
    
            /* Now copy OC to TOC */
            rrcllcoc_update_toc_with_oc_from_fach();
            /*Initiate the cell selection*/
            rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
                                                       RRC_TRANSITION_FROM_FACH,
                                                       rbrc.state_indicated_by_msg);
    
    
                
                
            rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
         
      
            /* exit from the switch case */
            break;
          }
        } /* if CELL_FACH->CELL_FACH reconfiguration */
        /* Call the function that creates and sends a channel
          config request to LLC. */
        rrcrbrc_send_channel_config_req();

        /* Change the substate since we're waiting for the
           confirmation from LLC for the channel configs */
        rbrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
      }
      else if ((current_state==RRC_STATE_CELL_DCH) && 
               (rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
      {

        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,
			rrc_RadioBearerReconfiguration_r6_IEs,new_C_RNTI))
        {
          rbrc.new_crnti_valid = TRUE;
          rrc_translate_crnti(&rbrc_ptr->new_C_RNTI, &c_rnti);
          rbrc.new_crnti = (uint16)c_rnti;
        }
        else
        {
          rbrc.new_crnti_valid = FALSE;
        }

        /*Initiate the cell selection*/
        rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
                                                RRC_TRANSITION_FROM_DCH,
                                                RRC_STATE_CELL_FACH);
        rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

        /* Start T_305 [periodic cell update] timer */
        rrccu_start_t_305_timer();		
      }
      else if ((current_state==RRC_STATE_CELL_DCH) && 
               (rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH))
      {
        rbrc.directed_cell_info = rrcrbrc_is_pcpich_info_present(msg_ptr, msg_version);
        /*Send the RRCRBRC complete msg on old configuration */
        send_rbrc_complete_message(TRUE);
        /*wait for L2ack */
        rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;

        rrcllc_print_trans_chnl_rate();
      }
      else if ((current_state==RRC_STATE_CELL_DCH) && 
               (rbrc.state_indicated_by_msg==RRC_STATE_URA_PCH))
      {
        /* no need to check directed cell info in this case */
        /*Send the RRCRBRC complete msg on old configuration */
        send_rbrc_complete_message(TRUE);
        /*wait for L2ack */
        rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;

        rrcllc_print_trans_chnl_rate();
      }
      else if ((current_state==RRC_STATE_CELL_FACH) && 
               ((rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
                (rbrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
      {

        /*call the function that evaluates the directed cell info and set the flags accordingly */
        if((rbrc.directed_cell_info == FALSE) && 
          (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
        {
            
          if(rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
          {
           rbrc.directed_cell_info = rrcrbrc_is_pcpich_info_present(msg_ptr, msg_version);
          }
          rbrc.initiate_cell_selection_trans_from_fach = TRUE;
 
        }
    

        /*Send the RRCRBRC complete msg on old configuration */
        /* check whether new C-RNTI was present in the message. In that case
        we have to configure MAC with new C-RNTI before sending the response 
        message */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r6_IEs,
			new_C_RNTI))
        {
          crnti_present = TRUE;
          crnti_ptr = &rbrc_ptr->new_C_RNTI;
        }

        if (crnti_present)
        {
          rrc_translate_crnti(crnti_ptr, &c_rnti);
          /* Send a RNTI Update Request */
          rnti_update_info.crnti_action = RRC_RNTI_UPDATE;
          rnti_update_info.crnti        = (uint16)c_rnti;
          /* check whether URNTI is also present in the message */
          if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r6_IEs,
		  	new_U_RNTI))
          {
            rrc_translate_urnti(&rbrc_ptr->new_U_RNTI, &u_rnti);
            rnti_update_info.urnti_action = RRC_RNTI_UPDATE; 
            rnti_update_info.urnti        = u_rnti; 
          }
          else
          {
            rnti_update_info.urnti_action = RRC_RNTI_NOCHANGE; 
          }
          rnti_update_info.procedure            = RRC_PROCEDURE_RBRC;
          rnti_update_info.rlc_re_establish_srb = FALSE;
          rnti_update_info.rlc_re_establish_rab = FALSE;
          rnti_update_info.cnf_reqd             = TRUE;
          rrcrb_send_rnti_update_req(rnti_update_info);
          /* wait for the RNTI update confirm before sending the response
          message */
          rbrc.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
        }
        else
        {
          send_rbrc_complete_message(TRUE);
          /*wait for L2ack */
          rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
          rrcllc_print_trans_chnl_rate();
        }
      }
      break;

    case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:
      rbrc.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
      /* Send the RB Release Failure Message */
      send_rbrc_failure_message( rbrc.status.failure_status,
                                 rbrc.status.prot_err_cause );

      /* Go back to initial substate */
      rbrc.curr_substate = RRCRB_INITIAL;

      /* Indicate that an error occured */
      ordered_config_err_occured = TRUE;
      break;

    case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

      rbrc.status.failure_status =RRCRB_MSG_UNSUPPORTED_CONFIG;
      /* Send the RB Release Failure Message */
      send_rbrc_failure_message( rbrc.status.failure_status,
                                 rbrc.status.prot_err_cause );

      /* Go back to initial substate */
      rbrc.curr_substate = RRCRB_INITIAL;

      /* Indicate that an error occured */
      ordered_config_err_occured = TRUE;
      break;

    case ORDERED_CONFIG_CONFIGURATION_INVALID:
        rbrc.status.failure_status =RRCRB_MSG_INVALID_CONFIG;
        /* Send the RB Release Failure Message */
        send_rbrc_failure_message( rbrc.status.failure_status,
                                  rbrc.status.prot_err_cause
                                );

        /* Go back to initial substate */
        rbrc.curr_substate = RRCRB_INITIAL;

        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;

    case ORDERED_CONFIG_NOT_SET_OTHER:
      WRRC_MSG0_HIGH("LLC returned other failure!");
      /* When LLC sends this, there is a serious problem
      and we'll go back to disconnected state. No need
      to send the failure message here, we can just
      go back to initial substate */
      rbrc.curr_substate = RRCRB_INITIAL;

      /* Indicate that an error occured */
      ordered_config_err_occured = TRUE;
      break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME      
    case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
      WRRC_MSG0_HIGH("RBRC processing deferred due to LTA!");
  
      rbrc.curr_substate = RRCRB_INITIAL;
  
      /* Indicate that an error occured */
      ordered_config_err_occured = TRUE;
      break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
    default:

      WRRC_MSG0_ERROR("Unknown return value from LLC");
      /* Unknown error, just go back to initial substate */
      rbrc.curr_substate = RRCRB_INITIAL;

      /* Indicate that an error occured */
      ordered_config_err_occured = TRUE;
      break;
  }/* end switch */

  if ( ordered_config_err_occured == TRUE )
  {
    /* No need to continue. Clear the procedure
    variables and return. */
    rrcrbrc_clear_procedure();
    return;
  }
  else
  {
    if (rbrc.rbrc_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }
  }
}

/*===========================================================================

FUNCTION   process_rbrc_message_r6

DESCRIPTION

  This function processes the received RB Reconfig Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a Radio Bearer Reconfig Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to configure the Radio Bearers given by the message.
    
    If ORDERED_CONFIG could not be set, a Radio Bearer Reconfig Failure message
    will be sent to UTRAN.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_rbrc_message_r6
(
  rrc_RRC_TransactionIdentifier transaction_id,

  rrc_DL_DCCH_Message *dcch_msg_ptr  ,
  /* Pointer to the RB Reconfig message        */
  rrc_RadioBearerReconfiguration_r6_IEs *rbrc_ptr
)
{
  rrc_CN_InformationInfo_r6              *cn_info_ptr = NULL; 
  rrc_RadioBearerReconfiguration         *msg_ptr; /* Pointer to the RB Reconfig message        */
  uint32                                  localrb_count;
  rrcrbrc_rlc_stop_cont_e_type            rlc_stop_cont_e_value = RB_STOP_CONTINUE_ABSENT;
  boolean                                 is_valid_rbid = TRUE;
  uint16                                  invalid_rbid = 0xff;
  rlc_lc_id_type                          lc_id;
  uecomdef_logch_mode_e_type              lc_mode;

  rrc_msg_ver_enum_type                   msg_version = MSG_REL6;
  rrc_RB_InformationReconfig_r6  *rb_info_reconfig_r6_ptr = NULL;
  rrc_RB_InformationAffectedList_r6  *rb_info_affected_list_r6_ptr = NULL;

  WRRC_MSG1_HIGH("RRCEUL: Recvd RBRC R6 Msg sub_st%d", rbrc.curr_substate);

   

  /* Get the pointer to the actual RB Reconfig message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerReconfiguration);
  if (FAILURE == 
    rrcrbrc_check_and_send_failure_for_simul_reconfig(transaction_id))
  {
    return;
  }

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  if(rbrc_ptr != NULL)
  {
    validate_rbrc_message_r6( rbrc_ptr );
  }
  else
  {
    WRRC_MSG0_ERROR("rbrc_ptr is NULL.");
    return;
  }

  if ( rbrc.status.failure_status != RRCRB_MSG_VALID )
  {
    /* RB Message was invalid. */
    /* Send the RB Reconfig Failure Message */
    send_rbrc_failure_message( rbrc.status.failure_status,
                               rbrc.status.prot_err_cause
                             );

    /* Make sure we're back in the initial sub-state */
    rbrc.curr_substate = RRCRB_INITIAL;
    rrcrbrc_clear_procedure();
    return;
  }

  /* update mac rab status to get tb size configuration*/
  if (rbrc_ptr->specificationMode.u.complete.m.rb_InformationReconfigListPresent)
  {
    rrcrb_update_mac_rab_status(
    RRC_PROCEDURE_RBRC,
    RRC_PS_DOMAIN_CN_ID,
    NULL,
    TRUE);
  }

  if (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
  	criticalExtensions.u.r6.m.radioBearerReconfiguration_r6_add_extPresent)
  {
    WRRC_MSG0_ERROR("RRCEUL: radioBearerReconfiguration_r6_add_ext not yet supported");
  } 
  
  if (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
      criticalExtensions.u.r6.m.v6b0NonCriticalExtensionsPresent)
  {
  
    rrcrbrc_process_high_mobility_ind_in_non_critical_ext(msg_ptr,MSG_REL6);
  
  }

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,
  	rrc_RadioBearerReconfiguration_r6_IEs,cn_InformationInfo))
  {
    cn_info_ptr = &(rbrc_ptr->cn_InformationInfo);

    /* CN info presents. */
    if (cn_info_ptr != NULL)
    {
      WRRC_MSG0_HIGH("Saving CN info for NAS");
      rbrc.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(cn_info_ptr);
      if (rbrc.mm_cmd_ptr != NULL)
      {
        /* set cn_info_saved flag */
        rbrc.cn_info_saved = TRUE;
      }
    }
  }


  /* Process madatory RB information to reconfigure */
  /* Process madatory RB information to reconfigure */
  if ((rbrc_ptr->specificationMode.u.complete.m.rb_InformationReconfigListPresent) &&
      rbrc_ptr->specificationMode.u.complete.rb_InformationReconfigList.n != 0 )
  {
    /*store the rb ids tobe released */
    rbrc.rbs_tobe_reconfigured.is_valid = TRUE;
    
    localrb_count = 0;
    while (rbrc_ptr->specificationMode.u.complete.rb_InformationReconfigList.n > localrb_count)
    {
      // Initialize the enum to this value.
      rb_info_reconfig_r6_ptr = &rbrc_ptr->specificationMode.u.complete.rb_InformationReconfigList.elem[localrb_count];
      rlc_stop_cont_e_value = RB_STOP_CONTINUE_ABSENT;

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_r6_ptr,
          rrc_RB_InformationReconfig_r6,rb_StopContinue))
      {
        lc_id = rrclcm_get_ul_rlc_lc_id((rb_info_reconfig_r6_ptr->rb_Identity));

        (void) rrclcm_get_ul_rlc_mode(lc_id, &lc_mode);

        WRRC_MSG2_HIGH("RRCRBRC:Stop/Continue[0-Stop,1-Continue] recvd for RB %d", 
                   rb_info_reconfig_r6_ptr->rb_StopContinue,(uint16)(rb_info_reconfig_r6_ptr->rb_Identity));
        if (rb_info_reconfig_r6_ptr->rb_StopContinue == rrc_RB_StopContinue_stopRB)
        {
          rlc_stop_cont_e_value = RB_STOP;
        }
        else if (rb_info_reconfig_r6_ptr->rb_StopContinue == rrc_RB_StopContinue_continueRB)
        {
          rlc_stop_cont_e_value = RB_CONTINUE;
        }
      }

      rbrc.rbs_tobe_reconfigured.rb_id[localrb_count] = 
          rb_info_reconfig_r6_ptr->rb_Identity;
      rbrc.rbs_tobe_reconfigured.rb_stop_continue[localrb_count] = rlc_stop_cont_e_value;

      rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] = rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] = 0;
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_r6_ptr,
	  	rrc_RB_InformationReconfig_r6,rlc_Info))&&
        (RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_r6_ptr->rlc_Info,
        rrc_RLC_Info_r6,ul_RLC_Mode)))
      {
        rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] |= MODIFY_RB_CFG;
      }
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_r6_ptr,
	  	rrc_RB_InformationReconfig_r6,rlc_Info))&&
        (RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_r6_ptr->rlc_Info,
        rrc_RLC_Info_r6,dl_RLC_Mode)))
      {
        rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] |= MODIFY_RB_CFG;
      }
      if(rlc_stop_cont_e_value == RB_STOP)
      {
        rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] |= STOP_RB_CFG;
        rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] |= STOP_RB_CFG;
      }
      else if(rlc_stop_cont_e_value == RB_CONTINUE)
      {
        rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] |= CONTINUE_RB_CFG;
        rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] |= CONTINUE_RB_CFG;
      }

      localrb_count++;
      
    }
    rbrc.rbs_tobe_reconfigured.num_rbs = localrb_count;
    WRRC_MSG1_HIGH("Num of RBs tobe reconfigured:%d", localrb_count);
  }

  /* Continue further if there are no invalid Rbids in recnfig list */
  if (is_valid_rbid)
  {
    /*Store the rb ids tobe affected */
    if (rbrc_ptr->specificationMode.u.complete.m.rb_InformationAffectedListPresent)
    {
      rbrc.rbs_tobe_affected.is_valid = TRUE;
      rb_info_affected_list_r6_ptr = 
        &rbrc_ptr->specificationMode.u.complete.rb_InformationAffectedList;
      localrb_count = 0;
      while ( rb_info_affected_list_r6_ptr->n > localrb_count )
      {
        /*first check whether this rb-id exists in established_rab variable or not */
        if (! validate_rb_id ((rb_info_affected_list_r6_ptr->elem[localrb_count].rb_Identity)))
        {
          /*rab-id does not exist in established_rab*/
          rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          is_valid_rbid = FALSE;
          invalid_rbid = (uint16)(rb_info_affected_list_r6_ptr->elem[localrb_count].rb_Identity);
          /*one of the rab-id does not exist in established_rab*/
          WRRC_MSG1_ERROR("rb-id %d is not in established_rab", invalid_rbid);
          break;
        }
        rbrc.rbs_tobe_affected.rb_id[localrb_count] = 
          rb_info_affected_list_r6_ptr->elem[localrb_count].rb_Identity;
        localrb_count++;
      }
      rbrc.rbs_tobe_affected.num_rbs= localrb_count;
      WRRC_MSG1_HIGH("Num of RBs tobe affected:%d", localrb_count);
    }
  }

  if (!is_valid_rbid)
  {
    /*one of the rab-id does not exist in established_rab*/
    WRRC_MSG1_ERROR("rb-id %d is not in established_rab", invalid_rbid);
    /* Send the RB Reconfig Failure Message */
    send_rbrc_failure_message( rbrc.status.failure_status,
                               rbrc.status.prot_err_cause
                             );
    /* Make sure we're back in the initial sub-state */
    rrcrbrc_clear_procedure();
    rbrc.curr_substate = RRCRB_INITIAL;
    return;
  }
  /* For Cell_FACH or Cell_DCH continue with the ordered config setting */
  rrcrbrc_update_ordered_config_r6 (dcch_msg_ptr, msg_version);
} /* process_rbrc_message_r6 */
/*===========================================================================

FUNCTION   RRCRBRC_UPDATE_ORDERED_CONFIG_R7

DESCRIPTION

  This function processes the received Channel Config Confirmation
  from LLC. If the required Radio-bearers have been reconfig succesfully,
  and if any of the radio-bearers map to radio-access-bearers, this
  information needs to be given to the NAS entity for that RAB.
  
  Currently Circuit-Switched voice and Circuit-Switched data RABs are 
  supported. If a voice RAB has been assigned by the UTRAN, this function
  gets the RLC ids and the AMR mode information from LLC and sends this
  information to voice services. If a CS Data RAB has been assigned, the
  RLC ids for those RBs are sent to the data services entity. Then this
  function sends the RRC_SYNC_IND primitive to MM.
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbrc_update_ordered_config_r7
(
  rrc_DL_DCCH_Message *dcch_msg_ptr,      /* Pointer to the RB Reconfig message */
  rrc_msg_ver_enum_type                   msg_version
)
{
  rrc_state_e_type                       current_state;       /* current RRC State  */
  rrcllc_oc_status_e_type                oc_status;/* To hold ordered_config status          */
  boolean                                ordered_config_err_occured = FALSE; /* Used to indicate if an error
                                                                       occured while setting ordered_config. */
  rrc_int_c_rnti_type                    c_rnti;
  rrc_int_u_rnti_type                    u_rnti;
  rrcrb_rnti_update_info_type            rnti_update_info;   /* to hold rnti update info */
  rrc_RadioBearerReconfiguration         *msg_ptr; /* Pointer to the RB Reconfig message        */
  rrcrb_directed_cell_info_type          directed_cell_info; /* directed cell info struct */
  boolean                                crnti_present = FALSE;
  ASN1BitStr32                           *crnti_ptr = NULL;
  rrc_RadioBearerReconfiguration_r7_IEs  *rbrc_ptr = NULL;

  /* We have to reconfigure RBs. First set ORDERED_CONFIG
    so that LLC can process the necessary parameters. */
  /* Get the pointer to the actual RB Reconfig message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerReconfiguration);
  oc_status = rrcllc_set_ordered_config( RRC_PROCEDURE_RBRC,
                                         rbrc.state_indicated_by_msg,
                                         rrc_DL_DCCH_Message_PDU,
                                         (void *)dcch_msg_ptr
                                       );
  
  rbrc_ptr = &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
             criticalExtensions.u.r7.radioBearerReconfiguration_r7;
  switch (oc_status)
  {
    case ORDERED_CONFIG_SET:
     if(rrc_sib_is_current_event_sib_read_in_dch())
     {
      WRRC_MSG0_HIGH("RRCGPS: Force Init GPS ");
      rrcsib_force_sib_event_init(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH);
     }
    current_state = rrc_get_state(); 

    directed_cell_info.freq_info_present = FALSE;
    directed_cell_info.pscr_present = FALSE;  
    /* Populate the local variables to indicate if Re-direction info is present */
    if((current_state==RRC_STATE_CELL_FACH) && 
       ((rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
        || (rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
        || (rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)))
    {
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r7_IEs,frequencyInfo)) &&
         (rbrc_ptr->frequencyInfo.modeSpecificInfo.t ==
             T_rrc_FrequencyInfo_modeSpecificInfo_fdd))
      {
        directed_cell_info.freq_info_present = TRUE;
        directed_cell_info.uarfcn_dl  = rbrc_ptr->frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
      }
       
      if (rrcrbrc_is_pcpich_info_present(msg_ptr, MSG_REL7 ))
      {
        directed_cell_info.pscr_present = TRUE;
        directed_cell_info.pscr = rbrc_ptr->dl_InformationPerRL_List.elem[0].
                                  modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
      }
      /*If Cell Reselection is going on and OTA is trying to perform Freq Redirection (FACH->FACH or
          FACH -> PCH), treat it as un-supported configuration and send failure.*/ 
      if((rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE) && 
        (rrccsp_check_initiate_cell_selection_handling(&directed_cell_info) == FALSE))
      {
        WRRC_MSG0_HIGH("Freq_Redir:Skipping Freq Redir bcoz Cell Resel in progress");
        if(rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH)
        {
          rrcrbrc_send_initiate_cu_flag = TRUE;
          rrcrbrc_delete_crnti          = TRUE;
        }
        else
        {
          rbrc.directed_cell_info = TRUE;
        }
      }      
    }
    if ((current_state==rbrc.state_indicated_by_msg) ||
        ((current_state==RRC_STATE_CELL_FACH) && 
         (rbrc.state_indicated_by_msg==RRC_STATE_CELL_DCH)))
    {
      /* CELL_DCH->CELL_DCH or CELL_FACH->CELL_FACH case. We can release the
      Radio Bearers now using the RRC_CHANNEL_CONFIG_REQ command. */
  
      /*check whether any frequency info is present in the message
      and transition is CELL_FACH->CELL_FACH. In this case
      initiate a cell update after channel config is successfully completed */
      if ((current_state==RRC_STATE_CELL_FACH)&& 
          (rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
      {
        /*call the function that evaluates the directed cell info and set the flags accordingly */
        if((rrcrbrc_send_initiate_cu_flag == FALSE) && 
          (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
        {
          if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r7_IEs,new_C_RNTI))
          {
            rbrc.new_crnti_valid = TRUE;
            rrc_translate_crnti(&rbrc_ptr->new_C_RNTI, &c_rnti);
            rbrc.new_crnti = (uint16)c_rnti;
          }
          else
          {
            rbrc.new_crnti_valid = FALSE;
          }
  
          /* Now copy OC to TOC */
          rrcllcoc_update_toc_with_oc_from_fach();
          /*Initiate the cell selection*/
          rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
                                                     RRC_TRANSITION_FROM_FACH,
                                                     rbrc.state_indicated_by_msg);
  
  
              
              
          rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
       
    
          /* exit from the switch case */
          break;
        }
      } /* if CELL_FACH->CELL_FACH reconfiguration */
      /* Call the function that creates and sends a channel
        config request to LLC. */
      rrcrbrc_send_channel_config_req();
  
      /* Change the substate since we're waiting for the
         confirmation from LLC for the channel configs */
      rbrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
    }
    else if ((current_state==RRC_STATE_CELL_DCH) && 
               (rbrc.state_indicated_by_msg==RRC_STATE_CELL_FACH))
    {
   
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r7_IEs,new_C_RNTI))
      {
        rbrc.new_crnti_valid = TRUE;
        rrc_translate_crnti(&rbrc_ptr->new_C_RNTI, &c_rnti);
        rbrc.new_crnti = (uint16)c_rnti;
      }
      else
      {
        rbrc.new_crnti_valid = FALSE;
      }
   
      /*Initiate the cell selection*/
      rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
                                              RRC_TRANSITION_FROM_DCH,
                                              RRC_STATE_CELL_FACH);
      rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
   
      /* Start T_305 [periodic cell update] timer */
      rrccu_start_t_305_timer();		
    }
    else if ((current_state==RRC_STATE_CELL_DCH) && 
             (rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH))
    {
      rbrc.directed_cell_info = rrcrbrc_is_pcpich_info_present(msg_ptr, msg_version);
      /*Send the RRCRBRC complete msg on old configuration */
      send_rbrc_complete_message(TRUE);
      /*wait for L2ack */
      rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
  
      rrcllc_print_trans_chnl_rate();
    }
    else if ((current_state==RRC_STATE_CELL_DCH) && 
             (rbrc.state_indicated_by_msg==RRC_STATE_URA_PCH))
    {
      /* no need to check directed cell info in this case */
      /*Send the RRCRBRC complete msg on old configuration */
      send_rbrc_complete_message(TRUE);
      /*wait for L2ack */
      rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
  
      rrcllc_print_trans_chnl_rate();
    }
    else if ((current_state==RRC_STATE_CELL_FACH) && 
             ((rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH) ||
              (rbrc.state_indicated_by_msg==RRC_STATE_URA_PCH)))
    {
      /*call the function that evaluates the directed cell info and set the flags accordingly */
      if((rbrc.directed_cell_info == FALSE) && 
        (rrcrb_validate_directed_cell_info (directed_cell_info) == FALSE))
      {
          
        if(rbrc.state_indicated_by_msg==RRC_STATE_CELL_PCH)
        {
         rbrc.directed_cell_info = rrcrbrc_is_pcpich_info_present(msg_ptr, msg_version);
        }
        rbrc.initiate_cell_selection_trans_from_fach = TRUE;
  
      }
  
  
      /*Send the RRCRBRC complete msg on old configuration */
      /* check whether new C-RNTI was present in the message. In that case
      we have to configure MAC with new C-RNTI before sending the response 
      message */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r7_IEs,new_C_RNTI))
      {
        crnti_present = TRUE;
        crnti_ptr = &rbrc_ptr->new_C_RNTI;
      }
  
      if (crnti_present)
      {
        rrc_translate_crnti(crnti_ptr, &c_rnti);
        /* Send a RNTI Update Request */
        rnti_update_info.crnti_action = RRC_RNTI_UPDATE;
        rnti_update_info.crnti        = (uint16)c_rnti;
        /* check whether URNTI is also present in the message */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r7_IEs,new_U_RNTI))
        {
          rrc_translate_urnti(&rbrc_ptr->new_U_RNTI, &u_rnti);
          rnti_update_info.urnti_action = RRC_RNTI_UPDATE; 
          rnti_update_info.urnti        = u_rnti; 
        }
        else
        {
          rnti_update_info.urnti_action = RRC_RNTI_NOCHANGE; 
        }
        rnti_update_info.procedure            = RRC_PROCEDURE_RBRC;
        rnti_update_info.rlc_re_establish_srb = FALSE;
        rnti_update_info.rlc_re_establish_rab = FALSE;
        rnti_update_info.cnf_reqd             = TRUE;
        rrcrb_send_rnti_update_req(rnti_update_info);
        /* wait for the RNTI update confirm before sending the response
        message */
        rbrc.curr_substate = RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF;
      }
      else
      {
        send_rbrc_complete_message(TRUE);
        /*wait for L2ack */
        rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
        rrcllc_print_trans_chnl_rate();
      }
    }
    break;

    case ORDERED_CONFIG_INCOMPATABLE_SIMULTANEOUS_RECONFIG:
      rbrc.status.failure_status = RRCRB_SIMULTANEOUS_RECONFIG;
      /* Send the RB Release Failure Message */
      send_rbrc_failure_message( rbrc.status.failure_status,
                                 rbrc.status.prot_err_cause );

      /* Go back to initial substate */
      rbrc.curr_substate = RRCRB_INITIAL;

      /* Indicate that an error occured */
      ordered_config_err_occured = TRUE;
      break;

    case ORDERED_CONFIG_CONFIGURATION_NOT_SUPPORTED:

      rbrc.status.failure_status =RRCRB_MSG_UNSUPPORTED_CONFIG;
      /* Send the RB Release Failure Message */
      send_rbrc_failure_message( rbrc.status.failure_status,
                                 rbrc.status.prot_err_cause );

      /* Go back to initial substate */
      rbrc.curr_substate = RRCRB_INITIAL;

      /* Indicate that an error occured */
      ordered_config_err_occured = TRUE;
      break;
      
      case ORDERED_CONFIG_CONFIGURATION_INVALID:

       rbrc.status.failure_status =RRCRB_MSG_INVALID_CONFIG;
       /* Send the RB Release Failure Message */
       send_rbrc_failure_message( rbrc.status.failure_status,
                                 rbrc.status.prot_err_cause
                               );
   
       /* Go back to initial substate */
       rbrc.curr_substate = RRCRB_INITIAL;
   
       /* Indicate that an error occured */
       ordered_config_err_occured = TRUE;
       break;
        
    case ORDERED_CONFIG_NOT_SET_OTHER:
      WRRC_MSG0_HIGH("LLC returned other failure!");
      /* When LLC sends this, there is a serious problem
      and we'll go back to disconnected state. No need
      to send the failure message here, we can just
      go back to initial substate */
      rbrc.curr_substate = RRCRB_INITIAL;

      /* Indicate that an error occured */
      ordered_config_err_occured = TRUE;
      break;

#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME      
    case ORDERED_CONFIG_CONFIGURATION_SET_DEFERRED:
        WRRC_MSG0_HIGH("RBRC processing deferred due to LTA!");
    
        rbrc.curr_substate = RRCRB_INITIAL;
    
        /* Indicate that an error occured */
        ordered_config_err_occured = TRUE;
        break;
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/
    default:

      WRRC_MSG0_ERROR("Unknown return value from LLC");
      /* Unknown error, just go back to initial substate */
      rbrc.curr_substate = RRCRB_INITIAL;

      /* Indicate that an error occured */
      ordered_config_err_occured = TRUE;
      break;
  }/* end switch */

  if ( ordered_config_err_occured == TRUE )
  {
    /* No need to continue. Clear the procedure
    variables and return. */
    rrcrbrc_clear_procedure();
    return;
  }
  else
  {
    if (rbrc.rbrc_for_srns_relocation == TRUE)
    {
      rrcsmc_commit_fresh_for_srns_relocation();
    }
  }
}

/*===========================================================================

FUNCTION   process_rbrc_message_r7

DESCRIPTION

  This function processes the received RB Reconfig Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a Radio Bearer Reconfig Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to configure the Radio Bearers given by the message.
    
    If ORDERED_CONFIG could not be set, a Radio Bearer Reconfig Failure message
    will be sent to UTRAN.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_rbrc_message_r7
(
  rrc_RRC_TransactionIdentifier transaction_id,

  rrc_DL_DCCH_Message *dcch_msg_ptr  ,
  /* Pointer to the RB Reconfig message        */
  rrc_RadioBearerReconfiguration_r7_IEs *rbrc_ptr
)
{
  rrc_CN_InformationInfo_r6              *cn_info_ptr = NULL; 
  uint32                                  localrb_count;
  rrcrbrc_rlc_stop_cont_e_type            rlc_stop_cont_e_value = RB_STOP_CONTINUE_ABSENT;
  boolean                                 is_valid_rbid = TRUE;
  uint16                                  invalid_rbid = 0xff;
  rlc_lc_id_type                          lc_id;
  uecomdef_logch_mode_e_type              lc_mode;
  rrc_RadioBearerReconfiguration         *msg_ptr; /* Pointer to the RB Reconfig message        */
  rrc_msg_ver_enum_type                   msg_version = MSG_REL7;
  rrc_RB_InformationReconfig_r7  *rb_info_reconfig_r7_ptr = NULL;
  rrc_RB_InformationAffectedList_r7  *rb_info_affected_list_r7_ptr = NULL;

  WRRC_MSG1_HIGH("RRCEUL: Recvd RBRC r7 Msg sub_st%d", rbrc.curr_substate);
    /* Get the pointer to the actual RB Reconfig message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerReconfiguration);
  if (FAILURE == 
    rrcrbrc_check_and_send_failure_for_simul_reconfig(transaction_id))
  {
    return;
  }

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  if(rbrc_ptr != NULL)
  {
    validate_rbrc_message_r7( rbrc_ptr );
  }
  else
  {
    WRRC_MSG0_ERROR("rbrc_ptr is NULL.");
    return;
  }

  if ( rbrc.status.failure_status != RRCRB_MSG_VALID )
  {
    /* RB Message was invalid. */
    /* Send the RB Reconfig Failure Message */
    send_rbrc_failure_message( rbrc.status.failure_status,
                               rbrc.status.prot_err_cause
                             );

    /* Make sure we're back in the initial sub-state */
    rbrc.curr_substate = RRCRB_INITIAL;
    rrcrbrc_clear_procedure();
    return;
  }

  /* update mac rab status to get tb size configuration*/
  if (rbrc_ptr->specificationMode.u.complete.m.rb_InformationReconfigListPresent)
  {
    rrcrb_update_mac_rab_status(
    RRC_PROCEDURE_RBRC,
    RRC_PS_DOMAIN_CN_ID,
    NULL,
    TRUE);
  }

  if ((msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
      criticalExtensions.u.r7.m.v780NonCriticalExtensionsPresent)
      && (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
      criticalExtensions.u.r7.v780NonCriticalExtensions.m.v790NonCriticalExtensionsPresent))
  {
    WRRC_MSG0_ERROR("RRCHSPA+: rrc_v790NonCriticalExtensions_present not yet supported");
  } 
        
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r7_IEs,cn_InformationInfo))
  {
    cn_info_ptr = &(rbrc_ptr->cn_InformationInfo);

    /* CN info presents. */
    if (cn_info_ptr != NULL)
    {
      WRRC_MSG0_HIGH("Saving CN info for NAS");
      rbrc.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info_r6(cn_info_ptr);
      if (rbrc.mm_cmd_ptr != NULL)
      {
        /* set cn_info_saved flag */
        rbrc.cn_info_saved = TRUE;
      }
    }
  }

#ifdef FEATURE_WCDMA_REL7_SPECRS
  /*If RBRC OTA provides "supportForChangeOfUE_Capability" IE, we need to store it in
        "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
        "rrc_rnc_cap_change_support" can be updated accordingly. */  
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r7_IEs,supportForChangeOfUE_Capability))
  {
    if(rbrc_ptr->supportForChangeOfUE_Capability == TRUE)
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
  


  /*If RBRC OTA provides "responseToChangeOfUE_Capability_present" IE, we need to store it so that later
       if procedure is successful, RBRC need to notify the COUEC procedure  about this IE.*/
       
  rrc_change_of_ue_cap_response = FALSE;       
  
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r7_IEs,responseToChangeOfUE_Capability))
  {
    rrc_change_of_ue_cap_response = TRUE;
  }
#endif 


  if((RRC_MSG_COMMON_BITMASK_IE_PTR(rbrc_ptr,rrc_RadioBearerReconfiguration_r7_IEs,ueMobilityStateIndicator)) &&
      (rbrc_ptr->ueMobilityStateIndicator == rrc_High_MobilityDetected_high_MobilityDetected))
  {
    WRRC_MSG0_HIGH("High Mobility Ind Set to TRUE in RBRC.");
    rbrc.high_mobility_ind = TRUE;
  }

  /* Process madatory RB information to reconfigure */
  /* Process madatory RB information to reconfigure */
  if ((rbrc_ptr->specificationMode.u.complete.m.rb_InformationReconfigListPresent) &&
      rbrc_ptr->specificationMode.u.complete.rb_InformationReconfigList.n != 0 )
  {
    /*store the rb ids tobe released */
    rbrc.rbs_tobe_reconfigured.is_valid = TRUE;
    
    localrb_count = 0;
    while (rbrc_ptr->specificationMode.u.complete.rb_InformationReconfigList.n > localrb_count)
    {
      rb_info_reconfig_r7_ptr = &rbrc_ptr->specificationMode.u.complete.rb_InformationReconfigList.elem[localrb_count];
      // Initialize the enum to this value.
      rlc_stop_cont_e_value = RB_STOP_CONTINUE_ABSENT;

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_r7_ptr, 
          rrc_RB_InformationReconfig_r7,rb_StopContinue))
      {
        lc_id = rrclcm_get_ul_rlc_lc_id(rb_info_reconfig_r7_ptr->rb_Identity);

        (void) rrclcm_get_ul_rlc_mode(lc_id, &lc_mode);

          WRRC_MSG2_HIGH("%d[0:STOP/1:CONTINUE recvd for RB %d", rb_info_reconfig_r7_ptr->rb_StopContinue,
                   (uint16)(rb_info_reconfig_r7_ptr->rb_Identity));

        if (rb_info_reconfig_r7_ptr->rb_StopContinue == rrc_RB_StopContinue_stopRB)
        {
          rlc_stop_cont_e_value = RB_STOP;
        }
        else if (rb_info_reconfig_r7_ptr->rb_StopContinue == rrc_RB_StopContinue_continueRB)
        {
          rlc_stop_cont_e_value = RB_CONTINUE;
        }
      }

      rbrc.rbs_tobe_reconfigured.rb_id[localrb_count] = 
          rb_info_reconfig_r7_ptr->rb_Identity;
      rbrc.rbs_tobe_reconfigured.rb_stop_continue[localrb_count] = rlc_stop_cont_e_value;

      rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] = rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] = 0;
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_r7_ptr,
	  	rrc_RB_InformationReconfig_r7,rlc_Info))&&
        (RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_r7_ptr->rlc_Info,
        rrc_RLC_Info_r7,ul_RLC_Mode)))
      {
        rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] |= MODIFY_RB_CFG;
      }
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_r7_ptr,
	  	rrc_RB_InformationReconfig_r7,rlc_Info))&&
        (RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_r7_ptr->rlc_Info,
        rrc_RLC_Info_r7,dl_RLC_Mode)))
      {
        rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] |= MODIFY_RB_CFG;
      }
      if(rlc_stop_cont_e_value == RB_STOP)
      {
        rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] |= STOP_RB_CFG;
        rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] |= STOP_RB_CFG;
      }
      else if(rlc_stop_cont_e_value == RB_CONTINUE)
      {
        rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] |= CONTINUE_RB_CFG;
        rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] |= CONTINUE_RB_CFG;
      }

      localrb_count++;
    }
    rbrc.rbs_tobe_reconfigured.num_rbs = localrb_count;
    WRRC_MSG1_HIGH("Num of RBs tobe reconfigured:%d", localrb_count);
  }

  /* Continue further if there are no invalid Rbids in recnfig list */
  if (is_valid_rbid)
  {
    /*Store the rb ids tobe affected */
    if (rbrc_ptr->specificationMode.u.complete.m.rb_InformationAffectedListPresent)
    {
      rbrc.rbs_tobe_affected.is_valid = TRUE;
      rb_info_affected_list_r7_ptr = 
        &rbrc_ptr->specificationMode.u.complete.rb_InformationAffectedList;
      localrb_count = 0;
      while ( rb_info_affected_list_r7_ptr->n > localrb_count )
      {
        /*first check whether this rb-id exists in established_rab variable or not */
        if (! validate_rb_id ((rb_info_affected_list_r7_ptr->elem[localrb_count].rb_Identity)))
        {
          /*rab-id does not exist in established_rab*/
          rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          is_valid_rbid = FALSE;
          invalid_rbid = (uint16)(rb_info_affected_list_r7_ptr->elem[localrb_count].rb_Identity);
          /*one of the rab-id does not exist in established_rab*/
          WRRC_MSG1_ERROR("rb-id %d is not in established_rab", invalid_rbid);
          break;
        }
        rbrc.rbs_tobe_affected.rb_id[localrb_count] = 
          rb_info_affected_list_r7_ptr->elem[localrb_count].rb_Identity;
        localrb_count++;
      }
      rbrc.rbs_tobe_affected.num_rbs= localrb_count;
      WRRC_MSG1_HIGH("Num of RBs tobe affected:%d", localrb_count);
    }
  }

  if (!is_valid_rbid)
  {
    /*one of the rab-id does not exist in established_rab*/
    WRRC_MSG1_ERROR("rb-id %d is not in established_rab", invalid_rbid);
    /* Send the RB Reconfig Failure Message */
    send_rbrc_failure_message( rbrc.status.failure_status,
                               rbrc.status.prot_err_cause
                             );
    /* Make sure we're back in the initial sub-state */
    rrcrbrc_clear_procedure();
    rbrc.curr_substate = RRCRB_INITIAL;
    return;
  }
  /* For Cell_FACH or Cell_DCH continue with the ordered config setting */
  rrcrbrc_update_ordered_config_r7 (dcch_msg_ptr, msg_version);
} /* process_rbrc_message_r7 */

/*===========================================================================

FUNCTION   RRCRBRC_PROCESS_NEW_CELL_IND

DESCRIPTION

  This function processes the received new cell ind command.
   It handles it in CELL FACH state.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbrc_process_new_cell_ind
(
  rrc_new_cell_ind_type *ptr /* Pointer to the State Change Ind cmd */
)
{
  
  /* If the UE has camped on directed cell then do not send a cell update,
    * else send a CELL update with cause "cell Re-selection 
    */
  
  if(rrccsp_is_selected_cell_utran_directed()&&
      (rbrc.new_crnti_valid))    
  {
    /* No need to initiate cell update as the C-RNTI is available
     * and UE is camped on the directed cell.
     */
    rrcrbrc_send_initiate_cu_flag = FALSE;
  }
  else
  {
    rrcrbrc_send_initiate_cu_flag = TRUE;
    rrcrbrc_delete_crnti          = TRUE;
  }
#ifdef FEATURE_WCDMA_HS_FACH
 /* If H-RNTI is not given in OTA then initiate cell update*/
  if(rrcenhstate_set_hrnti_status_for_transitions())
  {
    rrcrbrc_send_initiate_cu_flag = TRUE;
    rrcrbrc_delete_crnti          = TRUE;
  }
#endif

#ifdef FEATURE_WCDMA_HS_RACH
 /* Check if E-RNTI is not given in OTA then initiate cell update*/
        if(rrchsrach_check_ernti_status())
        {
          rrcrbrc_send_initiate_cu_flag = TRUE;
          rrcrbrc_delete_crnti          = TRUE;
        }
#endif


  /* Change the substate since we're waiting for the
   confirmation from LLC for the channel configs */
  rrcrbrc_send_channel_config_req();
  rbrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;

}

/*===========================================================================

FUNCTION   rrcrbrc_send_rbrc_failure_unsupported_config

DESCRIPTION

  This function sends the rbreconfig-failure messsage with cause unsupported
  configuration because UE dont support the extension provided by the network.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbrc_send_rbrc_failure_unsupported_config
(
  /* transaction id of RB Reconfig message */
  rrc_RRC_TransactionIdentifier transaction_id  
)
{
  WRRC_MSG0_HIGH("Un supported revision of RBRC msg");

  /* The first thing to do is check the transaction id.
  If the transaction id is the same as the accepted
  or rejected transaction ids, we don't process
  this message. It's a duplicate and will just be
  dropped. But we also don't change the current
  sub-state since we're in the middle of
  processing a previous message. Note that
  the transaction id is included only if critical
  extensions are not present. */
  if ( transaction_id == rbrc.tr_ids.rejected_transaction )
  {
    WRRC_MSG1_HIGH("Duplicate RBRC msg - rejected tr-id%d match!",
             rbrc.tr_ids.rejected_transaction);
    /* Nothing else to do, just ignore the message and return */
  } 
  else
  {
    rbrc.status.failure_status = RRCRB_MSG_UNSUPPORTED_CONFIG;
    rbrc.tr_ids.rejected_transaction = transaction_id;

    /*Check whether Cell Update is going on */
    if ( (rrc_ccm_is_reselection_in_progress() == TRUE)     ||
         (RRCCU_STARTED == rrccu_get_cell_update_started_status() ))
    {
      WRRC_MSG0_HIGH("Cell Update Started! Register for CU CNF");
      (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBRC);
      /* Wait until Cell update procedure is completed */
      rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF; 
    }
    else
    {
      send_rbrc_failure_message( rbrc.status.failure_status,
                                 rbrc.status.prot_err_cause );
    }
  }
}
/*===========================================================================

FUNCTION   PROCESS_RBRC_MESSAGE

DESCRIPTION

  This function processes the received RB Reconfig Message. It first
  validates the message. If the message has an invalid or unsupported
  configuration, a Radio Bearer Reconfig Failure message will be sent to
  the UTRAN. If the message is valid, it will be processed as follows -
  
    ORDERED_CONFIG will be set by calling LLC. 
    
    If ORDERED_CONFIG could be set without any conflicts, a RRC_CHANNEL_CONFIG
    _REQ will be sent to LLC to configure the Radio Bearers given by the message.
    
    If ORDERED_CONFIG could not be set, a Radio Bearer Reconfig Failure message
    will be sent to UTRAN.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void process_rbrc_message
(
  rrc_cmd_type *cmd_ptr
)
{

  rrc_DL_DCCH_Message                    *dcch_msg_ptr;      /* Pointer to the RB Reconfig message */
  rrc_RadioBearerReconfiguration         *msg_ptr; /* Pointer to the RB Reconfig message        */

  rrc_RB_InformationReconfig  *rb_info_reconfig_ptr = NULL;
  rrc_RB_InformationAffectedList  *rb_info_affected_list_ptr = NULL;
  uint32                                  localrb_count;
  rrcrbrc_rlc_stop_cont_e_type            rlc_stop_cont_e_value = RB_STOP_CONTINUE_ABSENT;
  boolean                                 is_valid_rbid = TRUE;
  uint16                                  invalid_rbid = 0xff;
  rlc_lc_id_type                          lc_id;
  uecomdef_logch_mode_e_type            lc_mode;

   /*Boolean used to indicate primary plmn i.d is present in OTA msg*/
  boolean primary_plmn_Id_present = FALSE;
   /*Variable to hold primary plmn i.d if present in OTA msg*/
  rrc_PLMN_Identity primary_plmn_Identity;


  dcch_msg_ptr = (rrc_DL_DCCH_Message *)cmd_ptr->cmd. downlink_sdu_ind.decoded_msg;

  /* If ASN.1 decoding failed, the received pointer would be null.
  So we know we got a RB Reconfig Message (by peeking into
  the encoded bit stream), but we were not able to decode it.
  Send a RB reconfig failure message with the appropriate
  cause. */
  if ( dcch_msg_ptr == NULL )
  {
    WRRC_MSG0_HIGH("ASN decode failed for RB Reconfig msg");
    rbrc.status.failure_status = RRCRB_MSG_PROTOCOL_ERROR;
    rbrc.status.prot_err_cause = rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError;
    rbrc.tr_ids.rejected_transaction = 0;
    send_rbrc_failure_message( rbrc.status.failure_status,
                               rbrc.status.prot_err_cause
                             );
    

    /* Nothing else to do here 
    continue with the same substate because some other reconfiguration
    message might be ongoing*/
    return;
  }

  /* Make sure it's the Radio Bearer Reconfig message
  although we would not have gotten here unless it's
  the right message */
  if ( dcch_msg_ptr->message.t != T_rrc_DL_DCCH_MessageType_radioBearerReconfiguration )
  {
    WRRC_MSG1_ERROR("Incorrect message given to RBRC proc %d", dcch_msg_ptr->message.t);
    return;
  }   

  /* Get the pointer to the actual RB Reconfig message */
  msg_ptr = &(dcch_msg_ptr->message.u.radioBearerReconfiguration);

  /* This RBRC msg is Rel5/Rel-6 version. */
  if (msg_ptr->t != T_rrc_RadioBearerReconfiguration_r3)
  {
    if ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) &&
        (msg_ptr->u.later_than_r3.criticalExtensions.t == 
           T_rrc_RadioBearerReconfiguration_criticalExtensions_5_criticalExtensions))
    {
      if (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.t ==
           T_rrc_RadioBearerReconfiguration_criticalExtensions_4_r5)
      { 
        /* check for SRNS relocation scenario */
        if (FAILURE == rrcrbrc_validate_and_check_integrity_for_srns_relocation(
            cmd_ptr, msg_ptr, msg_ptr->u.later_than_r3.rrc_TransactionIdentifier, 
            MSG_REL5))
        {
          /*do not clear the procedure or change the rbr substate because
            some other rbreconfig procedure might be in progress. Just return from
            here */
          return;
        }
   
        process_rbrc_message_r5(cmd_ptr);
      }
      /* if we are here then it means that t selected is: 
         rrc_RadioBearerReconfiguration_later_than_r3_criticalExtensions_
         criticalExtensions_criticalExtensions */
      else if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6) && 
              (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t ==
               T_rrc_RadioBearerReconfiguration_criticalExtensions_3_r6))
      {
        /* check for SRNS relocation scenario */
        if (FAILURE == rrcrbrc_validate_and_check_integrity_for_srns_relocation(
            cmd_ptr, msg_ptr, msg_ptr->u.later_than_r3.rrc_TransactionIdentifier, 
            MSG_REL6))
        {
          /* do not clear the procedure or change the rbr substate because
          some other rbreconfig procedure might be in progress. Just return from
          here */
          return;
        }
  
        process_rbrc_message_r6(
          msg_ptr->u.later_than_r3.rrc_TransactionIdentifier,
          (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg,
          &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
          criticalExtensions.u.r6.radioBearerReconfiguration_r6
        );
  

      }
      /* if we are here then it means that t selected is: 
         rrc_RadioBearerReconfiguration_later_than_r3_criticalExtensions_
         criticalExtensions_criticalExtensions */
      else if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7) && 
              (msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t ==
               T_rrc_RadioBearerReconfiguration_criticalExtensions_2_r7))
      {
        /* check for SRNS relocation scenario */
        if (FAILURE == rrcrbrc_validate_and_check_integrity_for_srns_relocation(
            cmd_ptr, msg_ptr, msg_ptr->u.later_than_r3.rrc_TransactionIdentifier, 
            MSG_REL7))
        {
          /* do not clear the procedure or change the rbr substate because
          some other rbreconfig procedure might be in progress. Just return from
          here */
          return;
        }
  
        process_rbrc_message_r7(
          msg_ptr->u.later_than_r3.rrc_TransactionIdentifier,
          (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg,
          &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
          criticalExtensions.u.criticalExtensions.u.r7.radioBearerReconfiguration_r7
        );
  

      }
      /* Now check whether R8 RBRC OTA is given or not*/
      else if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8) && 
        (RRCRB_R8_CRITICAL_EXT_PRESENT_PTR(msg_ptr, 
        rrc_RadioBearerReconfiguration)))
      {
        /* check for SRNS relocation scenario */
        if (FAILURE == rrcrbrc_validate_and_check_integrity_for_srns_relocation(
            cmd_ptr, msg_ptr, msg_ptr->u.later_than_r3.rrc_TransactionIdentifier, MSG_REL8))
        {
          /* do not clear the procedure or change the rbr substate because
          some other rbreconfig procedure might be in progress. Just return from
          here */
          return;
        }
        process_rbrc_message_r8(msg_ptr->u.later_than_r3.rrc_TransactionIdentifier,
          (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg,
          &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
          criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.radioBearerReconfiguration_r8);
      }
#ifdef FEATURE_WCDMA_REL9

     else if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9) && 
       (RRCRB_R9_CRITICAL_EXT_PRESENT_PTR(msg_ptr, 
       rrc_RadioBearerReconfiguration)))
     {
       /* check for SRNS relocation scenario */
       if (FAILURE == rrcrbrc_validate_and_check_integrity_for_srns_relocation(
           cmd_ptr, msg_ptr, msg_ptr->u.later_than_r3.rrc_TransactionIdentifier, MSG_REL9))
       {
         /* do not clear the procedure or change the rbr substate because
         some other rbreconfig procedure might be in progress. Just return from
         here */
         return;
       }
       process_rbrc_message_r9(msg_ptr->u.later_than_r3.rrc_TransactionIdentifier,
         (rrc_DL_DCCH_Message *)cmd_ptr->cmd.downlink_sdu_ind.decoded_msg,
         &msg_ptr->u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
         criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.radioBearerReconfiguration_r9);
     }

#endif /* FEATURE_WCDMA_REL9 */

#ifdef FEATURE_WCDMA_REL10
  #error code not present
#endif /* FEATURE_WCDMA_REL10 */


      else
      {
        rrc_print_supported_asn1_rel_version();
  
        WRRC_MSG0_ERROR("Configuration not supported.  Send RB reconfig failure");
    
        rrcrbrc_send_rbrc_failure_unsupported_config(
            msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);      
      }
      /* return - because all r5, r6, r7 and r8 processing are done inside the function */
      return;
    }
    else
    {
      rrc_print_supported_asn1_rel_version();
  
      WRRC_MSG0_ERROR("Configuration not supported.  Send RB reconfig failure");
  
      rrcrbrc_send_rbrc_failure_unsupported_config(
        msg_ptr->u.later_than_r3.rrc_TransactionIdentifier);
  
      /*do not clear the procedure or change the rbr substate because
       some other rbreconfig procedure might be in progress. Just return from
       here */
      return;
    }
  }

  WRRC_MSG1_MED("Recvd RBRC Msg sub_st%d", rbrc.curr_substate);

  /* check for SRNS relocation scenario */
  if (FAILURE == rrcrbrc_validate_and_check_integrity_for_srns_relocation(
      cmd_ptr, 
      msg_ptr, 
      msg_ptr->u.r3.radioBearerReconfiguration_r3.rrc_TransactionIdentifier,
      MSG_REL99))
  {
    /*do not clear the procedure or change the rbr substate because
    some other rbreconfig procedure might be in progress. Just return from
    here */
    return;
  }

  if (FAILURE == 
    rrcrbrc_check_and_send_failure_for_simul_reconfig(
      msg_ptr->u.r3.radioBearerReconfiguration_r3.rrc_TransactionIdentifier))
     {
     return;
  }

  /* Validate the Message to make sure there is no
  invalid or incompatible configuration given by the
  message */
  validate_rbrc_message( msg_ptr );

  if ( rbrc.status.failure_status != RRCRB_MSG_VALID )
  {
    /* RB Message was invalid. */
    /* Send the RB Reconfig Failure Message */
    send_rbrc_failure_message( rbrc.status.failure_status,
                               rbrc.status.prot_err_cause
                             );

    /* Make sure we're back in the initial sub-state */
    rbrc.curr_substate = RRCRB_INITIAL;
    rrcrbrc_clear_procedure();
    return;
  }

  /* update mac rab status to get tb size configuration*/
  if (msg_ptr->u.r3.radioBearerReconfiguration_r3.rb_InformationReconfigList.n != 0)
  {
    rrcrb_update_mac_rab_status(
    RRC_PROCEDURE_RBRC,
    RRC_PS_DOMAIN_CN_ID,
    NULL,
    TRUE);
  }

  /* If the message is valid, proceed further */
  /* Check if there is CN information present. If so, we need
  to send the information to the Cell Change manager */
  if ( RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerReconfiguration_r3,
        rrc_RadioBearerReconfiguration_r3_IEs,cn_InformationInfo)
     )
  {
    WRRC_MSG0_HIGH("Saving CN info for NAS");
     
    /*Check to see if primary PLMN I.D is present in RBRC message ext
     * rrc_RadioBearerReconfiguration-v690ext which is present in 
     * v690NonCriticalExtensions
     */
    primary_plmn_Id_present = rrcrbrc_check_and_get_primary_plmn_id(msg_ptr,
                                                                  &primary_plmn_Identity,MSG_REL99);
      
    rbrc.mm_cmd_ptr = rrc_ccm_buid_service_ind_with_nas_cn_info(
                     &(msg_ptr->u.r3.radioBearerReconfiguration_r3.cn_InformationInfo) 
                     ,primary_plmn_Id_present
                     ,&primary_plmn_Identity
      );
    if (rbrc.mm_cmd_ptr == NULL)
    {
      WRRC_MSG0_ERROR("Could not build SERVICE_IND cmd");
    }
    else
    {
      /* set cn_info_saved flag */
      rbrc.cn_info_saved = TRUE;
    }
  }
   

  rrcrbrc_process_high_mobility_ind_in_non_critical_ext(msg_ptr,MSG_REL99);

   
  /* Process madatory RB information to reconfigure */
  if ( msg_ptr->u.r3.radioBearerReconfiguration_r3.rb_InformationReconfigList.n
     != 0 )
  {
    /*store the rb ids tobe released */
    rbrc.rbs_tobe_reconfigured.is_valid = TRUE;

    localrb_count = 0;
    while (msg_ptr->u.r3.radioBearerReconfiguration_r3.rb_InformationReconfigList.n > localrb_count)
    {
       rb_info_reconfig_ptr = 
       &msg_ptr->u.r3.radioBearerReconfiguration_r3.rb_InformationReconfigList.elem[localrb_count];
      /*First check whether this rb-id exists in established_rab variable or not */
      if (! validate_rb_id(rb_info_reconfig_ptr->rb_Identity))
      {
        /*rab-id does not exist in established_rab*/
        rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
        is_valid_rbid = FALSE;
        invalid_rbid = (uint16)(rb_info_reconfig_ptr->rb_Identity);
        /*one of the rab-id does not exist in established_rab*/
          WRRC_MSG1_ERROR("rb-id %d is not in established_rab", invalid_rbid);
        /*Quit the loop*/
        break;
      }
      
      // Initialize the enum to this value.
      rlc_stop_cont_e_value = RB_STOP_CONTINUE_ABSENT;

      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,
	  	rrc_RB_InformationReconfig,rb_StopContinue))
      {
        if ((rb_info_reconfig_ptr->rb_Identity) <= 2)
        {
          /*rb-id does not exist in established_rab*/
          rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          is_valid_rbid = FALSE;
          invalid_rbid = (uint16)(rb_info_reconfig_ptr->rb_Identity);
          WRRC_MSG1_ERROR("RB ID %d can't be stopped/continued", invalid_rbid);
          /*Quit the loop*/
          break;
        }
        
        lc_id = rrclcm_get_ul_rlc_lc_id(rb_info_reconfig_ptr->rb_Identity);

        if (lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND)
        {
          /*rb-id does not exist in established_rab*/
          rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          is_valid_rbid = FALSE;
          invalid_rbid = (uint16)(rb_info_reconfig_ptr->rb_Identity);
          WRRC_MSG1_ERROR("No corresponding LC id for the RB %d",invalid_rbid);
          /*Quit the loop*/
          break;
        }
        
        if (rrclcm_get_ul_rlc_mode(lc_id, &lc_mode) == RRCLCM_FAILURE)
        {
          /*rb-id does not exist in established_rab*/
          rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          is_valid_rbid = FALSE;
          invalid_rbid = (uint16)(rb_info_reconfig_ptr->rb_Identity);
          WRRC_MSG1_ERROR("Invalid lc_mode for RB %d",invalid_rbid);
          /*Quit the loop*/
          break;
        }
        MSG_HIGH("%d[0:STOP/1:CONTINUE] recvd for RB %d lc_mode:%d ", 
         rb_info_reconfig_ptr->rb_StopContinue,(uint16)(rb_info_reconfig_ptr->rb_Identity),lc_mode);
        if (rb_info_reconfig_ptr->rb_StopContinue == rrc_RB_StopContinue_stopRB)
        {
          if (lc_mode == UE_MODE_TRANSPARENT)
          {
            // In case of transparent mode RB's just print an error message if stop/continue
            // is specified. Do not reject the rbreconfig message because of this.
            invalid_rbid = (uint16)(rb_info_reconfig_ptr->rb_Identity);
            /* Transparent mode can't be stopped/contd */
          }
          else
          {
            rlc_stop_cont_e_value = RB_STOP;
          }
          
        }
        else if (rb_info_reconfig_ptr->rb_StopContinue == rrc_RB_StopContinue_continueRB)
        {
          if (lc_mode == UE_MODE_TRANSPARENT)
          {
            // In case of transparent mode RB's just print an error message if stop/continue
            // is specified. Do not reject the rbreconfig message because of this.
            invalid_rbid = (uint16)(rb_info_reconfig_ptr->rb_Identity);
            /* Transparent mode can't be stopped/contd */

          }
          else
          {
            rlc_stop_cont_e_value = RB_CONTINUE;
          }
        }
      }
      
      rbrc.rbs_tobe_reconfigured.rb_id[localrb_count] = 
          rb_info_reconfig_ptr->rb_Identity;
      rbrc.rbs_tobe_reconfigured.rb_stop_continue[localrb_count] = rlc_stop_cont_e_value;

      rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] = rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] = 0;
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,
	  	rrc_RB_InformationReconfig,rlc_Info))&&
        (RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_ptr->rlc_Info,
        rrc_RLC_Info,ul_RLC_Mode)))
      {
        rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] |= MODIFY_RB_CFG;
      }
      if((RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,
	  	rrc_RB_InformationReconfig,rlc_Info))&&
        (RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_ptr->rlc_Info,
        rrc_RLC_Info,dl_RLC_Mode)))
      {
        rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] |= MODIFY_RB_CFG;
      }
      if(rlc_stop_cont_e_value == RB_STOP)
      {
        rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] |= STOP_RB_CFG;
        rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] |= STOP_RB_CFG;
      }
      else if(rlc_stop_cont_e_value == RB_CONTINUE)
      {
        rbrc.rbs_tobe_reconfigured.ul_cfg_mask[localrb_count] |= CONTINUE_RB_CFG;
        rbrc.rbs_tobe_reconfigured.dl_cfg_mask[localrb_count] |= CONTINUE_RB_CFG;
      }

      localrb_count++;
    }
    rbrc.rbs_tobe_reconfigured.num_rbs = localrb_count;
    WRRC_MSG1_HIGH("Num of RBs tobe reconfigured:%d", localrb_count);
  }

  /* Continue further if there are no invalid Rbids in recnfig list */
  if (is_valid_rbid)
  {
    /*Store the rb ids tobe affected */
    /*lint -e514*/
    if (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->u.r3.radioBearerReconfiguration_r3,
       rrc_RadioBearerReconfiguration_r3_IEs,rb_InformationAffectedList)) 
        
      /*lint +e514*/
    {
      rbrc.rbs_tobe_affected.is_valid = TRUE;
      rb_info_affected_list_ptr = 
      &msg_ptr->u.r3.radioBearerReconfiguration_r3.rb_InformationAffectedList;
      localrb_count = 0;
      while ( rb_info_affected_list_ptr->n > localrb_count )
      {
        /*first check whether this rb-id exists in established_rab variable or not */
        if (! validate_rb_id ((rb_info_affected_list_ptr->elem[localrb_count].rb_Identity)))
        {
          /*rab-id does not exist in established_rab*/
          rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
          is_valid_rbid = FALSE;
          invalid_rbid = (uint16)(rb_info_affected_list_ptr->elem[localrb_count].rb_Identity);
          /*one of the rab-id does not exist in established_rab*/
          WRRC_MSG1_ERROR("rb-id %d is not in established_rab", invalid_rbid);
          break;
        }
        rbrc.rbs_tobe_affected.rb_id[localrb_count] = 
          rb_info_affected_list_ptr->elem[localrb_count].rb_Identity;
        localrb_count++;
      }
      rbrc.rbs_tobe_affected.num_rbs= localrb_count;
      WRRC_MSG1_HIGH("Num of RBs tobe affected:%d", localrb_count);
    }
  }

  if (!is_valid_rbid)
  {
    WRRC_MSG0_ERROR("Sending RB reconfig failure msg"); 
   /* Send the RB Reconfig Failure Message */
   send_rbrc_failure_message( rbrc.status.failure_status,
                              rbrc.status.prot_err_cause
                            );
   /* Make sure we're back in the initial sub-state */
   rrcrbrc_clear_procedure();
   rbrc.curr_substate = RRCRB_INITIAL;
   return;
  }
 /* For Cell_FACH or Cell_DCH continue with the ordered config setting */
 rrcrbrc_update_ordered_config (dcch_msg_ptr);
}/* end function process_rbrc_message */


/*===========================================================================

FUNCTION   RRCRBRC_PROCESS_RNTI_UPDATE_CNF

DESCRIPTION

  This function processes RNTI_UPDATE_CNF
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbrc_process_rnti_update_cnf
(
void
)
{
  rrc_state_e_type curr_state;

  curr_state = rrc_get_state();

  if ((curr_state == RRC_STATE_CELL_FACH) && 
      ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
       (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH)))
  {
   /*send the response message now */
    send_rbrc_complete_message(TRUE);
    /*wait for L2ack */
    rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
    rrcllc_print_trans_chnl_rate();
  }
  else
  {
    /* We should not get here*/
    WRRC_MSG1_HIGH("Unexpected RNTI Update cnf,rbrc state %d", 
                                   rbrc.curr_substate);
  }
}

/*===========================================================================

FUNCTION   RRCRBRC_PROCESS_CHANNEL_CONFIG_CNF

DESCRIPTION

  This function processes the received Channel Config Confirmation
  from LLC. If the required Radio-bearers have been reconfig succesfully,
  and if any of the radio-bearers map to radio-access-bearers, this
  information needs to be given to the NAS entity for that RAB.
  
  Currently Circuit-Switched voice and Circuit-Switched data RABs are 
  supported. If a voice RAB has been assigned by the UTRAN, this function
  gets the RLC ids and the AMR mode information from LLC and sends this
  information to voice services. If a CS Data RAB has been assigned, the
  RLC ids for those RBs are sent to the data services entity. Then this
  function sends the RRC_SYNC_IND primitive to MM.
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbrc_process_channel_config_cnf
(
rrc_channel_config_cnf_type *ptr  /* Pointer to the Channel config cnf cmd */
)
{
  rrc_state_e_type            curr_state;
  rrcrb_rnti_update_info_type rnti_update_info;   /* to hold rnti update info */
  rrc_cmd_type *cmd; /* To send a LLC command if channel config failed */
  rrc_CellUpdateCause       cu_cause; /*To store CU cause from CU procedure*/
  rrc_rlc_re_est_for_srns_rb_e_type rb_type; /*rb type tobe reestablished */

#ifdef FEATURE_VOC_AMR_WB
  mvs_mode_type nas_sync_ind=rrc_get_current_codec_mode();
#endif /*FEATURE_VOC_AMR_WB*/

  rrcllcpcie_initialise_sync_a_post_veri_info();


  /* Check if the channel configuration was succesful. */
  if ( ptr->chan_cfg_status != RRCLLC_CHAN_CFG_SUCCESS )
  {
#ifndef FEATURE_WCDMA_HS_FACH
    /* Channel config failure, so initialise the ordered RB-Mapping database*/
    rrcllc_init_ordered_hanging_rb_mapping_info();
#else
    rrcllc_copy_current_hanging_rb_mapping_to_ordered();
#endif


    /*Here we initialize rlc_size_change_in_progress DB so that if it was set by RBRC, 
    it is ready for next procedure*/
    rrc_initialize_rlc_size_change();

    WRRC_MSG0_ERROR("CHAN_CONFIG_CNF with Failure");
#ifdef FEATURE_VOC_AMR_WB
    rrc_set_ordered_codec_mode(nas_sync_ind);
#endif/*FEATURE_VOC_AMR_WB*/
  /* update mac rab status to get tb size configuration*/
    rrcrb_update_mac_rab_status(
    RRC_PROCEDURE_RBRC,
    RRC_PS_DOMAIN_CN_ID,
    NULL,
    TRUE);
    rbrc.status.failure_status = RRCRB_PHY_CHAN_FAILURE;
    if (ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_CFG_CANCELLED)
    {
      /* cell update is already in progress as this is for pending config
       * cancellation
       */
      WRRC_MSG0_HIGH("Channel Config cancel waiting for CU complete");
      rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      return; 
    }
    
    /* validation failure happened.  Send RB failure msg with invalid config */
    if (ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_OTHER)
    {
      rbrc.status.failure_status = RRCRB_MSG_INVALID_CONFIG;
      if(rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG)
      {
        rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBRC, RRC_UNRECOVERABLE_ERROR, FALSE);
        return; 
      }

      WRRC_MSG0_HIGH("Sending RBRC failure msg with invalid config");
      send_rbrc_failure_message( rbrc.status.failure_status,
                                 rbrc.status.prot_err_cause
                                 );

      /* Reset ORDERED_CONFIG - UE can now accept other ordered
      configurations */
      if (OC_NOT_SET == rrcllc_reset_ordered_config(RRC_PROCEDURE_RBRC
                                                    ,TRUE
                                                    ))
      {
        /* Then clear all local variables that may
        need to be cleared */
        rrcrbrc_clear_procedure();
        /* Make sure we're back in the initial sub-state */
        rbrc.curr_substate = RRCRB_INITIAL;
      }
      /*During FACH->DCH transition, if T305 got expired, we need to start T305 with zero timeout.*/	  
      if(rrc_get_state() == RRC_STATE_CELL_FACH && rbrc.state_indicated_by_msg == RRC_STATE_CELL_DCH)
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
    request, and rbrc substate should be RRCRB_WAIT_FOR_CELL_UPDATE_CNF */ 
    if (rrc_get_state() == RRC_STATE_CELL_FACH && rbrc.state_indicated_by_msg == RRC_STATE_CELL_DCH)
    {
      WRRC_MSG0_HIGH("PHYCHANFAIL FACH->DCH.  Initiate CU");

      /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
      rbrc.status.failure_status = RRCRB_PHY_CHAN_FAILURE;
      rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBRC, RRC_PHY_CHAN_FAILURE,TRUE);
      /* Wait for CELL UPDATE to get a valid C_RNTI */
      rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      rrcrbrc_send_initiate_cu_flag = FALSE;
      rrcrbrc_delete_crnti          = FALSE;
      return;
    }
    /* Check if PHY_CHAN_FAILURE happended within CELL_DCH.  For now, initatiate a cell update request
       with cause RL Failure.  RBRC substate should be RRCRB_WAIT_FOR_CELL_UPDATE_CNF*/
    else if (rrc_get_state() == RRC_STATE_CELL_DCH && rbrc.state_indicated_by_msg == RRC_STATE_CELL_DCH)
    {
      rbrc.status.failure_status = RRCRB_PHY_CHAN_FAILURE;

      if(rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG)
      {
        rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBRC, RRC_RADIO_LINK_FAILURE, FALSE);
        return; 
      }

      if (ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_PHY_CHL)
      {
        WRRC_MSG0_HIGH("RBreconfig failed, going back to old cfg success");
        send_rbrc_failure_message( rbrc.status.failure_status,
                                   rbrc.status.prot_err_cause
                                   );
        /* Go back to initial substate */
        rbrc.curr_substate = RRCRB_INITIAL;
        /* Then clear all local variables that may
        need to be cleared */
        rrcrbrc_clear_procedure();
        /* Clear ORDERED_CONFIG - UE can now accept other ordered
        configurations */
        rrcllc_clear_ordered_config(); 
      }
      else if(ptr->chan_cfg_status == RRCLLC_CHAN_CFG_FAILURE_BACK_TO_OLD_CONFIG)
      {
        WRRC_MSG0_HIGH("RBreconfig failed.  Old config failed.  Initiate CU");
        /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBRC, RRC_RADIO_LINK_FAILURE,TRUE);
        /* Wait for CELL UPDATE to get a valid C_RNTI */
        rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
        rrcrbrc_send_initiate_cu_flag = FALSE;
        rrcrbrc_delete_crnti          = FALSE;
      }
    }
    else
    {
      if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
          (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
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
        cmd->cmd.chan_config_req.procedure = RRC_PROCEDURE_RBRC;
        /* No need to fill any other parameter in this command.
        When LLC sees the next state is Disconnected State, it
        will tear down all the Radio Bearers. */
        rrc_put_int_cmd( cmd );
      } 
      else
      {
        WRRC_MSG0_HIGH("Channel Config Failed!");
        rbrc.status.failure_status = RRCRB_PHY_CHAN_FAILURE;
        send_rbrc_failure_message( rbrc.status.failure_status,
                                   rbrc.status.prot_err_cause
                                   );
      }
      /* Go back to initial substate */
      rbrc.curr_substate = RRCRB_INITIAL;
      /* Then clear all local variables that may
      need to be cleared */
      rrcrbrc_clear_procedure();
    }
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
#endif /*FEATURE_WCDMA_REL7_SPECRS*/
  if((ordered_config.set_status == OC_SET_FOR_DCH_FACH_TRANS) || 
      (ordered_config.set_status == OC_SET_FOR_DCH_CELL_PCH_TRANS)||
      (ordered_config.set_status == OC_SET_FOR_DCH_URA_PCH_TRANS))	
  {
    if(rbrc.high_mobility_ind == TRUE)
    {
      WRRC_MSG0_HIGH("Sending High Mobility");
      rrcrb_send_upd_ota_parms_ind(RRC_PROCEDURE_RBRC, CPHY_HIGH_MOBILITY_INCL);
    }
  }

  /*For FACH->DCH, if CHAN_CONFIG_REQ is succeeded, we need to stop T305*/  
  if(rrc_get_state() == RRC_STATE_CELL_FACH && rbrc.state_indicated_by_msg == RRC_STATE_CELL_DCH)
  {
    WRRC_MSG0_HIGH("Chan_config succeeded for FACH->DCH. So Stopping T305");
    rrctmr_stop_timer(RRCTMR_T_305_TIMER);
  }  

   /*Notify the Cell_Id to registered entities*/
  if(rbrc.state_indicated_by_msg == RRC_STATE_CELL_DCH)
  {
    rrc_notify_cell_id_in_dch();
  }	
     
  /* Copy the Ordered RB Mapping into the current RB-Mapping database*/
  rrcllc_copy_ordered_hanging_rb_mapping_to_current();


  /*Here, we check and then configure for SCUDIF. Otherwise, we check for codec/rate
  change for AMR RAB. Notice that the SCUDIF/Rate change can happen with or without
  RAB Reconfig Info IE in RBRC OTA */
  rrcrbrc_configure_rab_change();
     

  if(ptr->inter_freq_hho_status == TRUE)
  {
    rbrc.hho_ciph_status = TRUE;
  }
  else
  {
    rbrc.hho_ciph_status = FALSE;  
  }

  curr_state=rrc_get_state();
  if ((RRC_STATE_CELL_FACH == curr_state) && 
      ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_FACH) ||
       (rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
       (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH)))
  {
    if (rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) 
    {
      /* this is the case of CELL_FACH->CELL_PCH state transition.
      In current implementation we do initiate cell selection. Instead
      we treat current cell as strongest cell */
      if ((rbrc.directed_cell_info) && (!rrccsp_is_selected_cell_utran_directed()))
      {
        /* check whether current camped scr code is same as the one
        sent in the message. If not then initiate cell update procedure */

          /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
          rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBRC,
                                              RRC_CELL_RESELECTION,
                                              FALSE);

        /* No need to send ul data registration to rlc. This will be taken care
        by cellupdate procedure */
        rrcrbrc_process_successful_proc_completion(FALSE);
      }
      else
      {
        /* send drx info to L1. Since this is CELL_FACH->CELL_PCH state transition
        RB procedure is responsible for sending the DRX info to lower layer*/
        rrcrb_send_l1_drx_req (RRC_PROCEDURE_RBRC);

        rrcrbrc_process_successful_proc_completion (TRUE);
      }
      /* return from here */
      return;
    }
    else if (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH)
    {
      /* send drx info to L1 */
      rrcrb_send_l1_drx_req (RRC_PROCEDURE_RBRC);
      /* check whether URA update is required */
      if ( rrccu_is_ura_update_reqd() == TRUE)
      {
        rrcrb_send_ura_update_initiate_req( RRC_PROCEDURE_RBRC,
                                            RRC_URA_RESELECTION,
                                            FALSE);
      }
      /* no need to wait for ura update completion 
      declare that proc is successfully completed */
      rrcrbrc_process_successful_proc_completion (TRUE);
      /* return from here */
      return;
    }
    if(rrcrbrc_send_initiate_cu_flag
       || (rrccu_return_cu_substate() == RRCCU_WAIT_FOR_RB_PROC_COMPL_IND) 
       )
    {
      if (rrcrbrc_delete_crnti)
      {
        /* Send a RNTI Update Request */
        rnti_update_info.crnti_action         = RRC_RNTI_DELETE;
        rnti_update_info.urnti_action         = RRC_RNTI_NOCHANGE;
        rnti_update_info.procedure            = RRC_PROCEDURE_RBRC;
        rnti_update_info.rlc_re_establish_srb = FALSE;
        rnti_update_info.rlc_re_establish_rab = FALSE;
        rnti_update_info.cnf_reqd             = FALSE;

        rrcrb_send_rnti_update_req(rnti_update_info);
        rbrc.new_crnti_valid=FALSE;
      }
      /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
      rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBRC,
                                          RRC_CELL_RESELECTION,
                                          TRUE);
      /* Wait for CELL UPDATE to get a valid C_RNTI */
      rbrc.status.failure_status = RRCRB_MSG_VALID;
      rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      rrcrbrc_send_initiate_cu_flag = FALSE;
      rrcrbrc_delete_crnti          = FALSE;
      return;
    }
    /*Following case will occur when RLC RESET happens on SRB2 during DCH->FACH Transition*/
    else if((rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG) &&
      (rrccu_get_cell_update_started_status_with_cause(&cu_cause) == RRCCU_STARTED) && 
      (cu_cause == rrc_CellUpdateCause_rlc_unrecoverableError) && (rrccu_get_rb_id_unrec_error() == 
      DCCH_AM_RADIO_BEARER_ID))
    {
      if(rbrc.new_crnti_valid)
      {
        /* Send a RNTI Update Request */
        rnti_update_info.crnti_action         = RRC_RNTI_UPDATE;
        rnti_update_info.urnti_action         = RRC_RNTI_NOCHANGE;
        rnti_update_info.crnti                = rbrc.new_crnti;
        rnti_update_info.procedure            = RRC_PROCEDURE_RBRC;
        rnti_update_info.rlc_re_establish_srb = FALSE;
        rnti_update_info.rlc_re_establish_rab = FALSE;
        rnti_update_info.cnf_reqd             = FALSE;
  
        rrcrb_send_rnti_update_req(rnti_update_info);
        /* Send a Radio Bearer Setup Complete later */      
      }

      /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
      rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBRC,
                                          RRC_UNRECOVERABLE_ERROR,
                                          TRUE);    

      /* Wait for CELL UPDATE to get a valid C_RNTI */
      rbrc.status.failure_status = RRCRB_MSG_VALID;
      rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;

      return;                                          
    }
    /* Coming here implies the "rrcrbrc_send_initiate_cu_flag" is FALSE.
     * Check if Valid CRNTI is stored in RBRC (for DCH->FACH case).
     * If so, send a RNTI Update Req to LLC and NOT wait for
     * Cell Update Complete Confirm. Cell Update will NOT be 
     * initiated when there is a valid C-RNTI and rrcrbrc_send_initiate_cu_flag
     * is FALSE.
     */
    else if(rbrc.new_crnti_valid)
    {
      /* Send a RNTI Update Request */
      rnti_update_info.crnti_action         = RRC_RNTI_UPDATE;
      rnti_update_info.urnti_action         = RRC_RNTI_NOCHANGE;
      rnti_update_info.crnti                = rbrc.new_crnti;
      rnti_update_info.procedure            = RRC_PROCEDURE_RBRC;
      rnti_update_info.rlc_re_establish_srb = FALSE;
      rnti_update_info.rlc_re_establish_rab = FALSE;
      rnti_update_info.cnf_reqd             = FALSE;

      rrcrb_send_rnti_update_req(rnti_update_info);
      /* Send a Radio Bearer Setup Complete later */      
    }
      /* Query for valid C-RNTI */
    else if (C_RNTI_NOT_VALID == rrcllc_get_current_crnti_status() )
    {
      /* Wait for CELL UPDATE to get a valid C_RNTI. Register for CU complete cnf. */
      WRRC_MSG0_HIGH("No Valid C_RNTI! Register for CU CNF");
      (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBRC);
      rbrc.status.failure_status = RRCRB_MSG_VALID;
      rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;
      return;
    }

  } /* if current state is CELL_FACH */
  else if (RRC_STATE_CELL_PCH == curr_state)
  {
    if(rrcrbrc_send_initiate_cu_flag)
    {
      /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
      rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBRC,
                                          RRC_CELL_RESELECTION,
                                          FALSE);
      rrcrbrc_send_initiate_cu_flag = FALSE;
      /* No need to send ul data registration to rlc. This will be taken care
      by cellupdate procedure */
      rrcrbrc_process_successful_proc_completion(FALSE);
#ifdef FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE
      #error code not present
#endif
      return;
    }
    else
    {
      /*Perform the final steps for successful completion of the
      procedure */
      rrcrbrc_process_successful_proc_completion(TRUE);
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
        rrcrb_send_ura_update_initiate_req( RRC_PROCEDURE_RBRC,
                                            RRC_URA_RESELECTION,
                                            FALSE);
      }
    }
    /* No need to send ul data registration to rlc. This will be taken care
    by cellupdate procedure */
    rrcrbrc_process_successful_proc_completion(TRUE);
#ifdef FEATURE_GAN_3G_ROVE_IN_CONNECTED_STATE
      #error code not present
#endif
    return;
  }


  /*check whether this reconfiguration involves SRNS relocation */
  if (rbrc.rbrc_for_srns_relocation == TRUE)
  {
    if ((RRC_STATE_CELL_DCH == curr_state)
#ifdef FEATURE_WCDMA_DEFAULT_CFG_CELLFACH
         || (RRC_STATE_CELL_FACH == curr_state)
#endif
        )
    {
  
      /* Re-establish SRB2 before sending the response message */
      rb_type  = RRC_RE_EST_RB_2;
      rrcrb_send_rlc_re_est_req_to_smc(RRC_PROCEDURE_RBRC, rb_type, TRUE);
  
      rbrc.curr_substate = RRCRB_WAIT_FOR_REESTABLISH_SRB2;
    }
    else
    {
      WRRC_MSG1_ERROR("SRNS not supp. in state %d", curr_state);
    }
  }
  else /*normal reconfiguration without SRNS relocation */
  {   
    /* Send the Radio Bearer Reconfig Complete message */
    if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
        (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH))

    {
      send_rbrc_complete_message(TRUE);
    }
    else
    {
      /* Need to wait for L2 ack for all states because RRC needs to trigger include reconfig status indicator
      if RLC acknowledgement is not yet received.
      As per 8.3.1.3
      1> if the IE "Cell update cause" is set to "radio link failure" and the UE has submitted a reconfiguration response
      message to RLC and the UE has not received the RLC acknowledgement for the response message:
      2> include the IE "Reconfiguration Status Indicator" and set it to TRUE.*/
        rrc_update_reconfig_waiting_for_l2ack_dch(RRC_PROCEDURE_RBRC);
        send_rbrc_complete_message(TRUE);
      /* IF CU is waiting to handle RLF/RLC unrecoverable error, then send complete
       * and delay for 50ms so to make sure Complete goes out to NW on old config
       */
      if((rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG)
        && (rrc_get_state() == RRC_STATE_CELL_DCH))
      {
        /* wait for some time before initiating cell update */
        WRRC_MSG1_HIGH("Delaying by %d ms ",RRCRB_PND_CFG_DELAY);
        (void) rex_clr_sigs(rex_self(), RRCRBRC_TEMP_TIMER_SIG);
        (void) rex_set_timer(&rrcrbrc_temp_timer, RRCRB_PND_CFG_DELAY);
    
        rrcrbrc_sig = rex_wait(RRCRBRC_TEMP_TIMER_SIG);
        MSG_LOW("RRCRBRC signal:%d", rrcrbrc_sig, 0, 0);
        (void) rex_clr_sigs(rex_self(), RRCRBRC_TEMP_TIMER_SIG);
        rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBRC, RRC_UNRECOVERABLE_ERROR, FALSE);
      } 
      /*Following case handles the scenario when RF Failure or RLC unrecoverable error occured
        during DCH -> FACH or FACH -> FACH transiton.*/
      else if((rrccu_return_cu_substate() == RRCCU_WAIT_FOR_PENDING_CONFIG) &&
        (rrccu_get_cell_update_started_status_with_cause(&cu_cause) == RRCCU_STARTED))
      {
        if((cu_cause == rrc_CellUpdateCause_rlc_unrecoverableError) && (rrccu_get_rb_id_unrec_error() != DCCH_AM_RADIO_BEARER_ID))
        {
          /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
          rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBRC,
                                              RRC_UNRECOVERABLE_ERROR,
                                              TRUE);
        }
        else if(cu_cause == rrc_CellUpdateCause_radiolinkFailure)
        {
          /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
          rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBRC,
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
    if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
        (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH))

    {
      /* Change the substate to the next substate - this is
      where we wait for L2 ack from RLC for sending the RB RC
      complete message. */
      rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
    }
    else
    {
      /* clear the ordered config here
      treat the procedure as successful. */
      rrcllc_clear_ordered_config();  

      /*inform upper layers if necessary */
      rrcrbrc_send_indication_to_upper_layers();

      /* Go to the next sub-state in the state machine. That's
      the initial substate since we've sent the RB RC complete
      message */
      rbrc.curr_substate = RRCRB_INITIAL;

      /* At this point we don't need to remember any of the
      Radio Bearer information since it's stored in ESTABLISHED_RABS.
      Clear our internal variables */
      rrcrbrc_clear_procedure();
    }
  }
} /* rrcrbrc_process_channel_config_cnf */


/*===========================================================================

FUNCTION   RRCRBRC_PROCESS_RLC_REESTABLISHMENT_CNF

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
static void rrcrbrc_process_rlc_reestablishment_cnf
(
void
)
{
  rrcrb_rnti_update_info_type rnti_update_info; /* structure to be used to request RNTI update */

  /* Send the RNTI update request to rrcllc to update with new-URNTI */
  /* Send a RNTI Update Request */
  rnti_update_info.crnti_action         = RRC_RNTI_NOCHANGE;
  rnti_update_info.urnti_action         = RRC_RNTI_UPDATE;
  rnti_update_info.urnti                = rbrc.new_urnti;
  rnti_update_info.procedure            = RRC_PROCEDURE_RBRC;
  rnti_update_info.rlc_re_establish_srb = FALSE;
  rnti_update_info.rlc_re_establish_rab = FALSE;
  rnti_update_info.cnf_reqd             = FALSE;

  rrcrb_send_rnti_update_req(rnti_update_info);

  /* Send the reconfiguration complete message */

  send_rbrc_complete_message(TRUE);

  /* Change the substate to the next substate - this is
     where we wait for L2 ack from RLC for sending the RB Reconfig
     complete message. */
  rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
  rrcllc_print_trans_chnl_rate();
  
}

/*===========================================================================

FUNCTION   RRCRBRC_PROCESS_STATE_CHANGE_IND

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
static void rrcrbrc_process_state_change_ind
(
rrc_state_change_ind_type *ptr /* Pointer to the State Change Ind cmd */
)
{
  rrc_CellUpdateCause       cu_cause;
  /* Check if we're going to disconnected state */
  if ( ptr->new_state == RRC_STATE_DISCONNECTED )
  {
    /* Then change to initial substate */
    rbrc.curr_substate = RRCRB_INITIAL;

    /* Then clear all local variables that may
    need to be cleared */
    rrcrbrc_clear_procedure();
  }
  else if (ptr->new_state == RRC_STATE_CELL_FACH )
  {
    if(ptr->previous_state == RRC_STATE_CELL_DCH)
    {
      if (((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
          (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH)) &&
           (rbrc.curr_substate == RRCRB_WAIT_FOR_L2_ACK))
      {
        WRRC_MSG1_HIGH("state change ind rbrc_st:%d",rbrc.curr_substate); 
        /* Then change to initial substate */
        rbrc.curr_substate = RRCRB_INITIAL;

        /* Then clear all local variables that may
        need to be cleared */
        /* Note OC is set at this point. No need to clear it
        because cu procedure will take care of it */
        rrcrbrc_clear_procedure();
      }
      else if (rbrc.status.failure_status == RRCRB_MSG_VALID)
      {
        /* if cell update procedure is active with cause OOS, wait 
         * for the CU complete notification to send chan_config_req to
         * LLC 
         */
        if ((rrccu_get_cell_update_started_status_with_cause(&cu_cause)== RRCCU_STARTED) &&
            (cu_cause == rrc_CellUpdateCause_re_enteredServiceArea) &&
            (rrcllc_get_toc_usage() ==TOC_FOR_OOS))
        {
          rrcrbrc_delete_crnti = TRUE;
          WRRC_MSG0_HIGH("Complete rbrc msg first.  Indicate compl to CU once done");
        }
        /* Call the function that creates and sends the  channel
           config information to be requested from LLC. */
        rrcrbrc_send_channel_config_req();

        /* Change the substate since we're waiting for the
           confirmation from LLC for the channel configs */
        rbrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;

        if((rrccsp_is_selected_cell_utran_directed()) &&
           (rbrc.new_crnti_valid))
        {
           /* No need to initiate cell update as the C-RNTI is available
            * and UE is camped on the directed cell.
            */
          rrcrbrc_send_initiate_cu_flag = FALSE;
        }
        else
        {
          rrcrbrc_send_initiate_cu_flag = TRUE;
          rrcrbrc_delete_crnti = TRUE;
        }
#ifdef FEATURE_WCDMA_HS_FACH
 /* If H-RNTI is not given in OTA then initiate cell update*/
        if(rrcenhstate_set_hrnti_status_for_transitions())
        {
          rrcrbrc_send_initiate_cu_flag = TRUE;
          rrcrbrc_delete_crnti          = TRUE;
        }
#endif

#ifdef FEATURE_WCDMA_HS_RACH
 /* Check if E-RNTI is not given in OTA then initiate cell update*/
        if(rrchsrach_check_ernti_status())
        {
          rrcrbrc_send_initiate_cu_flag = TRUE;
          rrcrbrc_delete_crnti          = TRUE;
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
      MSG_HIGH("Recvd state change %d to %d rbrc state %d, No action",
                ptr->previous_state, ptr->new_state, rbrc.curr_substate);
    }
  }
  else if (ptr->new_state == RRC_STATE_CELL_PCH || 
           ptr->new_state == RRC_STATE_URA_PCH)
  {
    if(ptr->previous_state == RRC_STATE_CELL_DCH)
    {

      /* Call the function that creates and sends the  channel
         config information to be requested from LLC. */
      rrcrbrc_send_channel_config_req();
  
      /* Change the substate since we're waiting for the
         confirmation from LLC for the channel configs */
      rbrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
    
      if (ptr->new_state == RRC_STATE_CELL_PCH)
      {
        if (rbrc.directed_cell_info)
        {
          if (rrccsp_is_selected_cell_utran_directed()) 
          {
            /* No need to initiate cell update UE is camped on 
            the directed cell. */
            WRRC_MSG0_HIGH("Directed cell match, No CU reqd");
            rrcrbrc_send_initiate_cu_flag = FALSE;
          }
          else
          {
            rrcrbrc_send_initiate_cu_flag = TRUE;
            rrcrbrc_delete_crnti          = TRUE;
          }
        }
        else
        {
          /* we have to initiate cell update incase cpich info is not present
          in the message */
          rrcrbrc_send_initiate_cu_flag = TRUE;
          rrcrbrc_delete_crnti          = TRUE;
        }
      }
      else
      {
        /* URA_PCH case */
        rrcrbrc_send_initiate_cu_flag = FALSE;
      }
    }
    /* Handle the case of FACH->PCH with Freq Redirection */
    else if( (ptr->previous_state == RRC_STATE_CELL_FACH) && 
        (rbrc.initiate_cell_selection_trans_from_fach == TRUE ))
    {
      /* Call the function that creates and sends a channel
         config request to LLC. */
      rbrc.initiate_cell_selection_trans_from_fach = FALSE;
      if (ptr->new_state == RRC_STATE_CELL_PCH)
      {
        if (rbrc.directed_cell_info)
        {
          if(rrccsp_is_selected_cell_utran_directed())
          {
            /* No need to initiate cell update as the C-RNTI is available
              * and UE is camped on the directed cell.
              */
            rrcrbrc_send_initiate_cu_flag = FALSE;
          }
          else
          {
            rrcrbrc_send_initiate_cu_flag = TRUE;
            rrcrbrc_delete_crnti          = TRUE;
          }
        }
        else
        {
          /* we have to initiate cell update incase cpich info is not present
          in the message */
          rrcrbrc_send_initiate_cu_flag = TRUE;
          rrcrbrc_delete_crnti          = TRUE;
        }
      }
      else
      {
        /* No need of sending Cell update in case of URA PCH  */
        rrcrbrc_send_initiate_cu_flag = FALSE;
      }
      
      rrcrbrc_send_channel_config_req();
      /* Change the substate since we're waiting for the
         confirmation from LLC for the channel configs */
      rbrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
    }
    else
    {
      MSG_HIGH("Recvd state change %d to %d rbrc state %d, No action",
               ptr->previous_state, ptr->new_state, rbrc.curr_substate);
    }
    /* IF CU is waiting for RB proc to complete, no need to initate CU through RB procedure. Reset flag. */
    if (rrccu_return_cu_substate() == RRCCU_WAIT_FOR_RB_PROC_COMPL_IND) 
    {
      rrcrbrc_send_initiate_cu_flag = FALSE;
    }
  } 
  else
  {
    /* We should not get here since we haven't registered for
    other state changes */
    MSG_HIGH("Unsupported state change %d to %d rbrc state %d", ptr->previous_state,
             ptr->new_state, rbrc.curr_substate);
  }
}

/*===========================================================================

FUNCTION   RRCRBRC_PROCESS_L2_ACK_IND

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
static void rrcrbrc_process_l2_ack_ind
(
rrc_downlink_l2ack_ind_type *ptr /* Pointer to the L2 Ack Ind cmd */
)
{
  rrc_rlc_re_est_for_srns_rb_e_type rb_type;      /*rb type tobe reestablished */
  rrc_state_e_type                  current_state;       /* current RRC State  */
  rrc_CellUpdateCause       cu_cause;
  uint8                    rejected_tr_id = RRCRB_INVALID_TRANSACTION_ID;
  WRRC_MSG1_HIGH("RRCRBC: L2 ack %d (0:Success 1:Failure)for RBRC Complete msg received", ptr->status);

  if ( ptr->status == SUCCESS )
  {
    current_state = rrc_get_state(); 

    /* Back up accepted transaction ID */
    rejected_tr_id = rbrc.tr_ids.accepted_transaction;

    /* clean up trans id*/
    rbrc.tr_ids.accepted_transaction = RRCRB_INVALID_TRANSACTION_ID;

    if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
        (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
    {

      if(current_state==RRC_STATE_CELL_DCH) 
      {
        /*L2 Ack for the RBRC Complete message is received on the old configuration
        Now initiate a cell selection */
        rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
                                                RRC_TRANSITION_FROM_DCH,
                                                rbrc.state_indicated_by_msg);
        /*wait for the state_change indication after the cell selection
        is successfully completed */
        rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;

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
          rrcrb_send_cell_update_initiate_req(RRC_PROCEDURE_RBRC,
                                              RRC_REENTERED_SERVICE_AREA,
                                              TRUE);
          return;
        }
        if (RRCCU_STARTED == rrccu_get_cell_update_started_status_with_cause(&cu_cause) &&
            (cu_cause == rrc_CellUpdateCause_re_enteredServiceArea || cu_cause == rrc_CellUpdateCause_rlc_unrecoverableError ||
             cu_cause == rrc_CellUpdateCause_radiolinkFailure))
        {
          /* This is a case where cell update is in progress while RBRC transitions 
             UE to Cell_PCH or URA_PCH. Though L2 Ack is gotten, send a PCR failure 
             later when CU procedure gets Cell update confirm. If we were to act on 
             L2 Ack, RRC moves to Cell_PCH/URA_PCH where cell update confirm message
             is not handled (unless CU registers on RBRC's behalf) & UE would stuck 
             in Cell_PCH forever [in the absence of reselections].
          */
          if(SUCCESS != rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBRC))
          {
            WRRC_MSG0_ERROR("RBRC procedure failed to register with CU");
          }

          WRRC_MSG1_HIGH("Registered for Cell Update procedure completion: cu_cause = %d", cu_cause);
          rbrc.status.failure_status = RRCRB_PHY_CHAN_FAILURE;
          rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_UPDATE_CNF;

          /* Restore the accepted transaction ID for sending failure later */
          rbrc.tr_ids.rejected_transaction = rejected_tr_id;
        }
        else
        {

          /* In case of FACH ->PCH transition is Freq Re-direction 
           * info is present then trigger Cell Selection 
           */
          if((rbrc.initiate_cell_selection_trans_from_fach == TRUE )
            &&(rrccsp_check_initiate_cell_selection_handling(NULL) == TRUE))
          {
            /* Now copy OC to TOC */
            rrcllcoc_update_toc_with_oc_from_fach();
            
            /*Initiate the cell selection*/
            rrcrb_send_initiate_cell_selection_req( RRC_PROCEDURE_RBRC,
                                                   RRC_TRANSITION_FROM_FACH,
                                                   rbrc.state_indicated_by_msg);

        
            
            rbrc.curr_substate = RRCRB_WAIT_FOR_CELL_SELECTION_CNF;
          }
          else
          {
             rbrc.initiate_cell_selection_trans_from_fach = FALSE;

            /* Call the function that creates and sends a channel
               config request to LLC. */
            rrcrbrc_send_channel_config_req();
            /* Change the substate since we're waiting for the
               confirmation from LLC for the channel configs */
            rbrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
          }
        }
      }
      else
      {
        WRRC_MSG2_ERROR("Invalid combination of rrcstate%d and rrcrbrc substate%d",
               current_state, rbrc.curr_substate);
      }
      
      /*return from here */
      return;
    }
    /* Control comes here when the procedure does not involve Cell_PCH or URA_PCH */
    if (rbrc.rbrc_for_srns_relocation)
    {
      if ((RRC_STATE_CELL_DCH == rrc_get_state())
#ifdef FEATURE_WCDMA_DEFAULT_CFG_CELLFACH
         || (RRC_STATE_CELL_FACH == rrc_get_state())
#endif
          )
      {
        /* Initiate the re-establishment of remaining RBs before 
        RBRC procedure goes back to initial substate. 
        No need to wait for the confirmation */
        rb_type  = RRC_RE_EST_NON_RB_2_UM_AM;
        rrcrb_send_rlc_re_est_req_to_smc(RRC_PROCEDURE_RBRC, rb_type, FALSE);

        /*inform upper layers if necessary */
        rrcrbrc_send_indication_to_upper_layers();
      }
      else
      {
        WRRC_MSG1_ERROR("SRNS reloc flag is ON in rrc state %d", rrc_get_state());
      }
    }
  } 
  
  else
  {
    /* l2ack failed. If reconfiguration involves SARNS relocation then
    go to idle. Otherwise just complete the procedure */
    /* Here make sure that CU is not active.  If CU is active with OOS area handling
       then NACK coming from RLC shall be ingnored as RBRC failure with cause
       CU complete should be sent to NW */
    if (RRC_STATE_CELL_FACH == rrc_get_state() &&
        rrccu_get_cell_update_started_status_with_cause(&cu_cause) == RRCCU_STARTED && 
        (cu_cause == rrc_CellUpdateCause_re_enteredServiceArea || cu_cause == rrc_CellUpdateCause_rlc_unrecoverableError ||
         cu_cause == rrc_CellUpdateCause_radiolinkFailure))
    {
      (void)rrccu_register_for_cell_update_to_complete(RRC_PROCEDURE_RBRC);
      WRRC_MSG1_HIGH("NACK being ingnored as TCRC failure msg will be sent/Registered for Cell Update procedure completion: cu_cause = %d", cu_cause);
      return;
    }

    rbrc.tr_ids.accepted_transaction = RRCRB_INVALID_TRANSACTION_ID;
    if ((rbrc.rbrc_for_srns_relocation) ||
        (rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
        (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
    {
      WRRC_MSG2_ERROR("L2ack failed SRNSreloc:%d, next_st:%d",
          rbrc.rbrc_for_srns_relocation,
          rbrc.state_indicated_by_msg);
      /* Going to Idle should always go through CSP  */
      rrc_transition_to_disconnected_state(RRC_PROCEDURE_RBRC,RRC_TX_TO_DISCON_OTHER);
    }
  }

  /* Clear ORDERED_CONFIG - UE can now accept other ordered
  configurations */
  /* Note OC is set at this point. No need to clear it
  if the next state is cell_pch or ura_pch and
  l2 ack failied because cu procedure will take care of it */
  if (((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
      (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH)) &&
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
      (void)rrcllc_reset_ordered_config(RRC_PROCEDURE_RBRC,TRUE);
    }
  }
  else
  {
    rrcllc_clear_ordered_config();  
  }

  if (rbrc.rbrc_for_srns_relocation == TRUE)
  {
    rrcdormancy_update_dormancy_for_srns_hho_cco(RRC_PROCEDURE_RBRC);
  }

  /* Go to the next sub-state in the state machine. That's
  the initial substate since we've sent the RB RC complete
  message */
  rbrc.curr_substate = RRCRB_INITIAL;

  /* At this point we don't need to remember any of the
  Radio Bearer information since it's stored in ESTABLISHED_RABS.
  Clear our internal variables */
  rrcrbrc_clear_procedure();
}


/*===========================================================================

FUNCTION   RRCRBRC_INITIAL_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBRC procedure is in the RRCRB_INITIAL substate. This function
  looks at the received event and does the appropriate processing
  of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrcrbrc_initial_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  
  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Reconfiguration Message. Otherwise
    we would not have received this command. Copy the 
    received message pointer. */

    /* Process the RB Reconfig message */
    process_rbrc_message( cmd_ptr );

    break;

  default:

    /* No other events except Downlink SDU is expected in this
    substate */
    MSG_MED("Ignoring RB RC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate, 0);
    break;
  }
}



/*===========================================================================

FUNCTION   RRCRBRC_DCH_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBRC procedure is in the RRCRB_WAIT_FOR_CHAN_CFG_CNF substate
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
static void rrcrbrc_dch_wfchcnf_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  
  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcrbrc_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Reconfig Message. Otherwise
    we would not have received this command. Copy the 
    received message pointer. */

    /* Process the RB Reconfig message */

    process_rbrc_message( cmd_ptr );

    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBRC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate, 0);
    break;
  }
}


/*===========================================================================

FUNCTION   RRCRBRC_PCH_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBRC procedure is in the RRCRB_WAIT_FOR_CHAN_CFG_CNF substate
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
static void rrcrbrc_pch_wfchcnf_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{


  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  
  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcrbrc_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:
  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBRC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate, 0);
    break;
  }
}

/*===========================================================================

FUNCTION   rrcrbrc_other_wfcucnf_substate_event_handler

DESCRIPTION

  This function handles the events received by this module when
  the RBRC procedure is in the RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate
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
static void rrcrbrc_other_wfcucnf_substate_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_STATE_CHANGE_IND:
      /* Indication that RRC's state has changed */
      rrcrbrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;
    
    default:    
      /* No other events expected in this
      substate */
      MSG_MED("Ignoring RBRC event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate, 0);
      break;
  }
}

/*===========================================================================

FUNCTION   rrcrbrc_other_wfcscnf_event_handler

DESCRIPTION

  This function handles the events received by this module when
  the RBRC procedure is in the RRCRB_WAIT_FOR_CELL_SELECTION_CNF substate
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
static void rrcrbrc_other_wfcscnf_event_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_STATE_CHANGE_IND:
      /* Indication that RRC's state has changed */
      rrcrbrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
      break;
    
    default:    
      /* No other events expected in this
      substate */
      MSG_MED("Ignoring RBRC event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate, 0);
      break;
  }
}

/*===========================================================================

FUNCTION   RRCRBRC_DCH_WFCUCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBRC procedure is in the RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate
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

static void rrcrbrc_dch_wfcucnf_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  case RRC_CELL_UPDATE_COMPLETE_CNF:
      if(rbrc.status.failure_status == RRCRB_SIMULTANEOUS_RECONFIG)
      {
        /* Send the RB Reconfig Failure Message */
        send_rbrc_failure_message(rbrc.status.failure_status,
                                  rbrc.status.prot_err_cause
                                );
        /* Then clear all local variables that may
        need to be cleared */
        rrcrbrc_clear_procedure();

        /* Make sure we're back in the initial sub-state */
        rbrc.curr_substate = RRCRB_INITIAL;
      }
      else if(rbrc.status.failure_status == RRCRB_MSG_VALID)
      {
        /* Send the RB Reconfig Complete message */
        if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
            (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH))

        {
          send_rbrc_complete_message(TRUE);
        }
        else
        {
          send_rbrc_complete_message(FALSE);
        }
        /*inform upper layers if necessary */
        rrcrbrc_send_indication_to_upper_layers();

        rrcllc_print_trans_chnl_rate();
        /*if next state is Cell_PCH or URA_PCH then we have to 
        treat the procedure as successful only after getting L2ack
        for the response message */
        if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
            (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH))

        {
          /* Change the substate to the next substate - this is
          where we wait for L2 ack from RLC for sending the RB RC
          complete message. */
          rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
        }
        else
        {
          /* clear the ordered config here
          treat the procedure as successful. */
          rrcllc_clear_ordered_config();  

          /* send the nas cn info if stored */
          if (rbrc.cn_info_saved != FALSE)
          {
            /* Put the command on MM queue */
            rrc_send_rrc_message_to_mm(rbrc.mm_cmd_ptr);
            /* clear the flag */
            rbrc.cn_info_saved = FALSE;
          }

          /* Go to the next sub-state in the state machine. That's
          the initial substate since we've sent the RB RC complete
          message */
          rbrc.curr_substate = RRCRB_INITIAL;

          /* At this point we don't need to remember any of the
          Radio Bearer information since it's stored in ESTABLISHED_RABS.
          Clear our internal variables */
          rrcrbrc_clear_procedure();
        }
      }
      /*  In the case of Phy_Chan_Failure, RB setup failure must be send
      and oc should be reset. */
      else if(rbrc.status.failure_status == RRCRB_PHY_CHAN_FAILURE)
      {
        
        /* Send the RB Setup Failure Message */
        send_rbrc_failure_message( rbrc.status.failure_status,
                                  rbrc.status.prot_err_cause
                                );

        /* Then clear all local variables that may
        need to be cleared */
        rrcrbrc_clear_procedure();
        /* Make sure we're back in the initial sub-state */
        rbrc.curr_substate = RRCRB_INITIAL;
      }
      else
      {
        WRRC_MSG1_ERROR("Failure status %d not handled",rbrc.status.failure_status);
      }

    break;

  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcrbrc_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Setup Message. Otherwise
    we would not have received this command. Copy the 
    received message pointer. */
    /* Process the RB Reconfig message */
    process_rbrc_message( cmd_ptr );
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBRC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate, 0);
    break;
  }

}



/*===========================================================================

FUNCTION   RRCRBRC_FACH_WFCUCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBRC procedure is in the RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate
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

static void rrcrbrc_fach_wfcucnf_event_handler
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
      if(rbrc.status.failure_status == RRCRB_SIMULTANEOUS_RECONFIG)
      {
        /* Send the RB Reconfig Failure Message */
        send_rbrc_failure_message(rbrc.status.failure_status,
                                  rbrc.status.prot_err_cause
                                );
        /* Then clear all local variables that may
        need to be cleared */
        rrcrbrc_clear_procedure();

        /* Make sure we're back in the initial sub-state */
        rbrc.curr_substate = RRCRB_INITIAL;
      }
      else if(rbrc.status.failure_status == RRCRB_MSG_VALID)
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
          rrcrbrc_send_channel_config_req();
  
          /* Change the substate since we're waiting for the
             confirmation from LLC for the channel configs */
          rbrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;	
          return;					
        }
				
        /* Send the RB Reconfig Complete message */
        if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
            (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH))

        {
          send_rbrc_complete_message(TRUE);
        }
        else
        {
          send_rbrc_complete_message(FALSE);
        }
        /*inform upper layers if necessary */
        rrcrbrc_send_indication_to_upper_layers();

        rrcllc_print_trans_chnl_rate();
        /*if next state is Cell_PCH or URA_PCH then we have to 
        treat the procedure as successful only after getting L2ack
        for the response message */
        if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
            (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH))
        {
          /* Change the substate to the next substate - this is
          where we wait for L2 ack from RLC for sending the RB RC
          complete message. */
          rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
        }
        else
        {
          /* clear the ordered config here
          treat the procedure as successful. */
          rrcllc_clear_ordered_config();  

          /* send the nas cn info if stored */
          if (rbrc.cn_info_saved != FALSE)
          {
            /* Put the command on MM queue */
            rrc_send_rrc_message_to_mm(rbrc.mm_cmd_ptr);
            /* clear the flag */
            rbrc.cn_info_saved = FALSE;
          }

          /* Go to the next sub-state in the state machine. That's
          the initial substate since we've sent the RB RC complete
          message */
          rbrc.curr_substate = RRCRB_INITIAL;

          /* At this point we don't need to remember any of the
          Radio Bearer information since it's stored in ESTABLISHED_RABS.
          Clear our internal variables */
          rrcrbrc_clear_procedure();
        }
      }
      /*  In the case of Phy_Chan_Failure, RB setup failure must be send
      and oc should be reset. */
      else if(rbrc.status.failure_status == RRCRB_PHY_CHAN_FAILURE)
      {
        
        /* Send the RB Setup Failure Message */
        send_rbrc_failure_message( rbrc.status.failure_status,
                                  rbrc.status.prot_err_cause
                                );

        /* Then clear all local variables that may
        need to be cleared */
        rrcrbrc_clear_procedure();
        /* Make sure we're back in the initial sub-state */
        rbrc.curr_substate = RRCRB_INITIAL;
      }
      else
      {
        WRRC_MSG1_ERROR("Failure status %d not handled",rbrc.status.failure_status);
      }

    break;

  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcrbrc_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Setup Message. Otherwise
    we would not have received this command. Copy the 
    received message pointer. */
    /* Process the RB Reconfig message */
    process_rbrc_message( cmd_ptr );
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBRC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate, 0);
    break;
  }

}

/*===========================================================================

FUNCTION   rrcrbrc_FACH_WF_CSCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBRC procedure is in the RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate
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

static void rrcrbrc_fach_wfcscnf_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_STATE_CHANGE_IND:
   /* Indication that RRC's state has changed */
    rrcrbrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:
    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Setup Message. Otherwise
    we would not have received this command. Copy the 
    received message pointer. */
    /* Process the RBRC Setup message */
    process_rbrc_message( cmd_ptr );
    break;

  case RRC_NEW_CELL_IND:
    rrcrbrc_process_new_cell_ind( &(cmd_ptr->cmd.new_cell_ind) );
    break;
   /*In case of FACH->FACH OOS, CCM will not send a NEW_CELL_IND to procedure,
   * But CU will be informed, and CU will inform the procedure after the Cell
   * Update Procedure is complete*/
  case RRC_CELL_UPDATE_COMPLETE_CNF:
    if(rbrc.state_indicated_by_msg == RRC_STATE_CELL_FACH)
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
        rrcrbrc_send_channel_config_req();
        /* Also reset the rrcrbrc_send_initiate_cu_flag as cell update
         * will not be needed since CU and CUCNF for oos took care 
         * of that
         */
        rrcrbrc_send_initiate_cu_flag = FALSE;
  
        /* Change the substate since we're waiting for the
        confirmation from LLC for the channel configs */
        rbrc.curr_substate = RRCRB_WAIT_FOR_CHAN_CFG_CNF;
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
      MSG_MED("Ignoring RBRC event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate, 0);
      break;
  }

}

/*===========================================================================

FUNCTION   RRCRBRC_FACH_WFCRNTIUPDATECNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBRC procedure is in the RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF substate
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

static void rrcrbrc_fach_wfcrntiupdatecnf_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_STATE_CHANGE_IND:
   /* Indication that RRC's state has changed */
    rrcrbrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
   
    break;


  case RRC_RNTI_UPDATE_CNF:
    if (cmd_ptr->cmd.rnti_update_cnf.status == SUCCESS) 
    {
      rrcrbrc_process_rnti_update_cnf();
    }
    else
    {
      WRRC_MSG0_HIGH("RNTI_UPDATE failed.  Send RBRC failure");
      rbrc.status.failure_status = RRCRB_MSG_VALID;
      
      send_rbrc_failure_message(rbrc.status.failure_status,
                                rbrc.status.prot_err_cause);
      
      /* Then clear all local variables that may
         need to be cleared */
      rrcrbrc_clear_procedure();

      /* Make sure we're back in the initial sub-state */
      rbrc.curr_substate = RRCRB_INITIAL;
    }
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBRC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate, 0);
    break;
  }

}

/*===========================================================================

FUNCTION   RRCRBRC_PCH_WFCUCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBRC procedure is in the RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate
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
static void rrcrbrc_pch_wfcucnf_event_handler
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
        if(rbrc.status.failure_status != RRCRB_MSG_VALID)
    {
      /* Send the PC Reconfig Failure Message */
      send_rbrc_failure_message(rbrc.status.failure_status,
                                rbrc.status.prot_err_cause
                              );
      /* Then clear all local variables that may
      need to be cleared */
      rrcrbrc_clear_procedure();

      /* Make sure we're back in the initial sub-state */
      rbrc.curr_substate = RRCRB_INITIAL;
    }
    else 
    {
      /* Send the TC Reconfig Complete message */
      if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
          (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH))

      {
        send_rbrc_complete_message(TRUE);
      }
      else
      {
        send_rbrc_complete_message(FALSE);
      }
      /* prints rate for each transport channel */
      rrcllc_print_trans_chnl_rate();
      /*if next state is Cell_PCH or URA_PCH then we have to 
      treat the procedure as successful only after getting L2ack
      for the response message */
      if ((rbrc.state_indicated_by_msg == RRC_STATE_CELL_PCH) ||
          (rbrc.state_indicated_by_msg == RRC_STATE_URA_PCH))

      {
        WRRC_MSG0_HIGH("Waiting for L2 ACK for RBRC Complete msg");
        /* Change the substate to the next substate - this is
        where we wait for L2 ack from RLC for sending the RB RC
        complete message. */
        rbrc.curr_substate = RRCRB_WAIT_FOR_L2_ACK;
      }
      else
      {
        /* clear the ordered config here
        treat the procedure as successful. */
        rrcllc_clear_ordered_config();  

        /* Go to the next sub-state in the state machine. That's
        the initial substate since we've sent the RB RC complete
        message */
        rbrc.curr_substate = RRCRB_INITIAL;

        /* At this point we don't need to remember any of the
        Radio Bearer information since it's stored in ESTABLISHED_RABS.
        Clear our internal variables */
        rrcrbrc_clear_procedure();
      }
    }
    break;

  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcrbrc_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:
    /* Indication that RRC's state has changed */
    rrcrbrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

 

    case RRC_DOWNLINK_SDU_IND:
      WRRC_MSG2_HIGH("Unexpected DOWNLINK_SDU_IND in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate);
    break;

  default:

    /* No other events expected in this
    substate */
    WRRC_MSG2_HIGH("Ignoring RBRC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate);
    break;
  }

}

/*===========================================================================

FUNCTION   RRCRBRC_PCH_WF_CSCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBRC procedure is in the RRCRB_WAIT_FOR_CELL_SELECTION_CNF substate
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

static void rrcrbrc_pch_wfcscnf_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {

  case RRC_STATE_CHANGE_IND:
   /* Indication that RRC's state has changed */
    rrcrbrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
   
    break;


  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBRC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate, 0);
    break;
  }

}

/*===========================================================================

FUNCTION   RRCRBRC_FACH_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBRC procedure is in the RRCRB_WAIT_FOR_CHAN_CFG_CNF substate
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
static void rrcrbrc_fach_wfchcnf_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  
  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    rrcrbrc_process_channel_config_cnf( &(cmd_ptr->cmd.chan_config_cnf) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Reconfig Message. Otherwise
    we would not have received this command. Copy the 
    received message pointer. */
    /* Process the RB Reconfig message */
     process_rbrc_message( cmd_ptr );

    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RBRC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate, 0);
    break;
  }
}



/*===========================================================================

FUNCTION   RRCRBRC_OTHER_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBRC procedure is in the RRCRB_WAIT_FOR_CHAN_CFG_CNF substate
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
static void rrcrbrc_other_wfchcnf_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  
  case RRC_CHANNEL_CONFIG_CNF:

    /* Confirmation from LLC for the channel configurations */
    WRRC_MSG0_HIGH("Ignoring chan config cnf since UE not in DCH or FACH");
    /* In this state this procedure should no longer be active.*/

    /* Then change to initial substate */
    rbrc.curr_substate = RRCRB_INITIAL;

    /* Then clear all local variables that may
    need to be cleared */
    rrcrbrc_clear_procedure();
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. */
    WRRC_MSG0_HIGH("Ignoring downlink sdu since UE not in DCH or FACH");
    /* In this state this procedure should no longer be active.*/

    /* Then change to initial substate */
    rbrc.curr_substate = RRCRB_INITIAL;

    /* Then clear all local variables that may
    need to be cleared */
    rrcrbrc_clear_procedure();
    break;

  default:

    /* No other events expected in this
    substate */
    MSG_MED("Ignoring RB RC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate, 0);
    break;
  }
}


/*===========================================================================

FUNCTION   RRCRBRC_DCH_WFSRB2REESTABLISH_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBRC procedure is in the RRCRB_WAIT_FOR_REESTABLISH_SRB2 substate
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
static void rrcrbrc_dch_wfsrb2reestablish_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  
  case RRC_RE_EST_RLC_FOR_SRNS_CNF:

    /* Confirmation from SMC of RLC re-establishment confirm */
    rrcrbrc_process_rlc_reestablishment_cnf( );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Reconfig Message. Otherwise
    we would not have received this command. Copy the 
    received message pointer. */
    /* Process the RB Reconfig message */
    process_rbrc_message( cmd_ptr );

    break;

  default:

    /* No other events except Downlink SDU is expected in this
    substate */
    MSG_MED("Ignoring RBRC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate, 0);
    break;
  }
}


/*===========================================================================

FUNCTION   RRCRBRC_DCH_WFL2ACK_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBRC procedure is in the RRCRB_WAIT_FOR_L2_ACK substate
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
static void rrcrbrc_dch_wfl2ack_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{

  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  
  case RRC_DOWNLINK_L2ACK_IND:

    /* Confirmation from LLC for the channel configurations */
    rrcrbrc_process_l2_ack_ind( &(cmd_ptr->cmd.downlink_l2ack_ind) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Reconfig Message. Otherwise
    we would not have received this command. Copy the 
    received message pointer. */
    /* Process the RB Reconfig message */
    process_rbrc_message( cmd_ptr );

    break;
  
  case RRC_CELL_UPDATE_COMPLETE_CNF:
    rbrc.status.failure_status = RRCRB_CELL_UPDATE_OCCURED;
    /* Send the PC Reconfig Failure Message */
    send_rbrc_failure_message(rbrc.status.failure_status,
                              rbrc.status.prot_err_cause
                              );

    /* Then clear all local variables that may
    need to be cleared */
    rrcrbrc_clear_procedure();

    /* Make sure we're back in the initial sub-state */
    rbrc.curr_substate = RRCRB_INITIAL;
    break;

  default:

    /* No other events except Downlink SDU is expected in this
    substate */
    MSG_MED("Ignoring RBRC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate, 0);
    break;
  }
}


/*===========================================================================

FUNCTION   RRCRBRC_FACH_WFL2ACK_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBRC procedure is in the RRCRB_WAIT_FOR_L2_ACK substate
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
static void rrcrbrc_fach_wfl2ack_substate_event_handler
(
rrc_cmd_type *cmd_ptr
)
{


  /* Switch on the received command's id */
  switch ( cmd_ptr->cmd_hdr.cmd_id )
  {
  
  case RRC_DOWNLINK_L2ACK_IND:

    /* Confirmation from LLC for the channel configurations */
    rrcrbrc_process_l2_ack_ind( &(cmd_ptr->cmd.downlink_l2ack_ind) );
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  case RRC_DOWNLINK_SDU_IND:

    /* A Downlink SDU has been received from RLC. This
    SDU should contain a Radio Bearer Reconfig Message. Otherwise
    we would not have received this command. Copy the 
    received message pointer. */
    /* Process the RB Reconfig message */
    process_rbrc_message( cmd_ptr );

    break;

  case RRC_CELL_UPDATE_COMPLETE_CNF:
    rbrc.status.failure_status = RRCRB_CELL_UPDATE_OCCURED;
    /* Send the PC Reconfig Failure Message */
    send_rbrc_failure_message(rbrc.status.failure_status,
                              rbrc.status.prot_err_cause
                              );

    /* Then clear all local variables that may
    need to be cleared */
    rrcrbrc_clear_procedure();

    /* Make sure we're back in the initial sub-state */
    rbrc.curr_substate = RRCRB_INITIAL;
    break;

  default:

    /* No other events except Downlink SDU is expected in this
    substate */
    MSG_MED("Ignoring RBRC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate, 0);
    break;
  }
}



/*===========================================================================

FUNCTION   RRCRBRC_OTHER_WFL2ACK_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the RBRC procedure is in the RRCRB_WAIT_FOR_L2_ACK substate
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
static void rrcrbrc_other_wfl2ack_substate_event_handler
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
    WRRC_MSG1_HIGH("Ignoring L2 ack/downlink SDU since UE not in DCH or FACH cmd_id:%d", cmd_ptr->cmd_hdr.cmd_id);
    /* In this state this procedure should no longer be active.*/

    /* Then change to initial substate */
    rbrc.curr_substate = RRCRB_INITIAL;

    /* Then clear all local variables that may
    need to be cleared */
    rrcrbrc_clear_procedure();
    break;

  case RRC_STATE_CHANGE_IND:

    /* Indication that RRC's state has changed */
    rrcrbrc_process_state_change_ind( &(cmd_ptr->cmd.state_change_ind) );
    break;

  default:

    /* No other events except Downlink SDU is expected in this
    substate */
    MSG_MED("Ignoring RB RC event %x in substate %d",
            cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate, 0);
    break;
  }
}





/*===========================================================================

FUNCTION   RRCRBRC_CELL_DCH_EVENT_HANDLER

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
static void rrcrbrc_cell_dch_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  if ((cmd_ptr->cmd_hdr.cmd_id == RRC_DOWNLINK_L2ACK_IND) &&
      (rbrc.curr_substate != RRCRB_WAIT_FOR_L2_ACK))
  {
    /* We could get L2ack for a previously sent message while we are already
    processing the next message. So l2ack could be received any any of the following
    substates. If rbrc substate does not indicate that we are not waiting for l2ack
    then just print the status */
    if (cmd_ptr->cmd.downlink_l2ack_ind.status == SUCCESS)
    {
      MSG_HIGH("Recd L2ack for MUI:%d, Lc_Id:%d, rbrc_subst:%d",
               cmd_ptr->cmd.downlink_l2ack_ind.mui, 
               cmd_ptr->cmd.downlink_l2ack_ind.lc_id, rbrc.curr_substate);
    }
    else
    {
      MSG_HIGH("L2ack failed for MUI:%d, Lc_Id:%d, rbrc_subst:%d",
               cmd_ptr->cmd.downlink_l2ack_ind.mui, 
               cmd_ptr->cmd.downlink_l2ack_ind.lc_id, rbrc.curr_substate);
    }
    /* return from here so that procedure can continue with existing process */
    return;
  }
  /* Switch on the current substate */
  switch ( rbrc.curr_substate )
  {
  case RRCRB_INITIAL:

    /* Call the event handler for RRCRB_INITIAL substate */
    rrcrbrc_initial_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_CHAN_CFG_CNF:

    /* Call the event handler for RRCRB_WAIT_FOR_CHAN_CFG_CNF substate */
    rrcrbrc_dch_wfchcnf_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_L2_ACK:

    /* Call the event handler for RRCRB_WAIT_FOR_L2_ACK substate */
    rrcrbrc_dch_wfl2ack_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_REESTABLISH_SRB2:

    /* Call the event handler for RRCRB_WAIT_FOR_REESTABLISH_SRB2 substate */
    rrcrbrc_dch_wfsrb2reestablish_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
    /* Call the event handler for RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate */
    rrcrbrc_dch_wfcucnf_event_handler( cmd_ptr);
    break;

  default:

    /* No other substates processed for this RRC state    */
    WRRC_MSG2_HIGH("Command %x not processed in CELL_DCH for substate %d", 
            cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate);
    break;

  }
}



/*===========================================================================

FUNCTION   RRCRBRC_CELL_FACH_EVENT_HANDLER

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
static void rrcrbrc_cell_fach_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  if ((cmd_ptr->cmd_hdr.cmd_id == RRC_DOWNLINK_L2ACK_IND) &&
      (rbrc.curr_substate != RRCRB_WAIT_FOR_L2_ACK))
  {
    /* We could get L2ack for a previously sent message while we are already
    processing the next message. So l2ack could be received any any of the following
    substates. If rbrc substate does not indicate that we are not waiting for l2ack
    then just print the status */
    MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCRBRC:L2ack %d (0-Success 1-Failure) for MUI:%d, Lc_Id:%d, rbe_subst:%d",
         cmd_ptr->cmd.downlink_l2ack_ind.status,
               cmd_ptr->cmd.downlink_l2ack_ind.mui, 
               cmd_ptr->cmd.downlink_l2ack_ind.lc_id, rbrc.curr_substate);

    /* return from here so that procedure can continue with existing process */
    return;
  }

  /* Switch on the current substate */
  switch ( rbrc.curr_substate )
  {
  case RRCRB_INITIAL:

    /* Call the event handler for RRCRB_INITIAL substate */
    /* NOTE: Here we need to check if a cell-update procedure
    has been initialized or not. For now, just call the event handler */
    rrcrbrc_initial_substate_event_handler( cmd_ptr );
    break;
   
  case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
    /* Call the event handler for RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate */
    rrcrbrc_fach_wfcucnf_event_handler( cmd_ptr);
    break;

  case RRCRB_WAIT_FOR_CELL_SELECTION_CNF:

    rrcrbrc_fach_wfcscnf_event_handler( cmd_ptr);
    break;

  case RRCRB_WAIT_FOR_CHAN_CFG_CNF:

    /* Call the event handler for RRCRB_WAIT_FOR_CHAN_CFG_CNF substate */
    rrcrbrc_fach_wfchcnf_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF:

      /* Call the event handler for RRCRB_WAIT_FOR_CRNTI_UPDATE_CNF substate */
      rrcrbrc_fach_wfcrntiupdatecnf_substate_event_handler( cmd_ptr );
      break;

  case RRCRB_WAIT_FOR_L2_ACK:

    /* Call the event handler for RRCRB_WAIT_FOR_L2_ACK substate */
    rrcrbrc_fach_wfl2ack_substate_event_handler( cmd_ptr );
    break;

#ifdef FEATURE_WCDMA_DEFAULT_CFG_CELLFACH
  case RRCRB_WAIT_FOR_REESTABLISH_SRB2:

    /* Call the event handler for RRCRB_WAIT_FOR_REESTABLISH_SRB2 substate */
    rrcrbrc_dch_wfsrb2reestablish_substate_event_handler( cmd_ptr );
    break;

#endif
  default:

    /* No other substates processed for this RRC state    */
    WRRC_MSG2_HIGH("Command %x not processed in CELL_FACH for substate %d", 
            cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate);
    break;
  }
}


/*===========================================================================

FUNCTION   RRCRBRC_PCH_EVENT_HANDLER

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
static void rrcrbrc_pch_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the current substate */
  switch ( rbrc.curr_substate )
  {
  case RRCRB_INITIAL:

    /* Call the event handler for RRCRB_INITIAL substate */
    rrcrbrc_initial_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
    /* Call the event handler for RRCRB_WAIT_FOR_CELL_UPDATE_CNF substate */
    rrcrbrc_pch_wfcucnf_event_handler( cmd_ptr);
    break;

  case RRCRB_WAIT_FOR_CELL_SELECTION_CNF:

    rrcrbrc_pch_wfcscnf_event_handler( cmd_ptr);
    break;

  case RRCRB_WAIT_FOR_CHAN_CFG_CNF:

    /* Call the event handler for RRCRB_WAIT_FOR_CHAN_CFG_CNF substate */
    rrcrbrc_pch_wfchcnf_substate_event_handler( cmd_ptr );
    break;

  case RRCRB_WAIT_FOR_L2_ACK:

    /* The only way we could be in pch and rb state is "wait_for_l2_ack is if
       oos happened while waiting for l2 ack and cell update confirm takes UE to pch
       In this case, send failure msg of rbrc with cause cell update occured 
     */
    rbrc.status.failure_status = RRCRB_CELL_UPDATE_OCCURED;

    send_rbrc_failure_message(rbrc.status.failure_status,
                              rbrc.status.prot_err_cause);

    /* Then clear all local variables that may
       need to be cleared */
    rrcrbrc_clear_procedure();

    /* Make sure we're back in the initial sub-state */
    rbrc.curr_substate = RRCRB_INITIAL;
    break;

  default:

    /* No other substates processed for this RRC state    */
    MSG_MED("Command %x not processed in substate%d rrc_state%d", 
            cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate, rrc_get_state());
    break;

  }
}

/*===========================================================================

FUNCTION   RRCRBRC_OTHER_STATE_EVENT_HANDLER

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
static void rrcrbrc_other_state_event_handler
(
rrc_cmd_type *cmd_ptr
)
{
  /* Switch on the current substate */
  switch ( rbrc.curr_substate )
  {
  
    case RRCRB_WAIT_FOR_CHAN_CFG_CNF:
  
      /* Call the event handler for WAIT_FOR_CHAN_CFG_CNF substate */
      rrcrbrc_other_wfchcnf_substate_event_handler( cmd_ptr );
      break;
  
    case RRCRB_WAIT_FOR_L2_ACK:
  
      /* Call the event handler for WAIT_FOR_L2_ACK substate */
      rrcrbrc_other_wfl2ack_substate_event_handler( cmd_ptr );
      break;
  
    case RRCRB_WAIT_FOR_CELL_UPDATE_CNF:
      rrcrbrc_other_wfcucnf_substate_event_handler( cmd_ptr);
      break;

    case RRCRB_WAIT_FOR_CELL_SELECTION_CNF:
      rrcrbrc_other_wfcscnf_event_handler( cmd_ptr);
      break;

  default:

    /* No other substates processed for this RRC state    */
    MSG_MED("Command %x not processed in IDLE or PCH states for substate %d", 
            cmd_ptr->cmd_hdr.cmd_id, rbrc.curr_substate, 0);
    break;
  }
}

/*===========================================================================

FUNCTION RRCRBRC_PROCEDURE_EVENT_HANDLER


DESCRIPTION

  This is the main function handling external events received by the
  Radio Bearer Reconfiguration procedure. This function is called by the
  dispatcher whenever an event is received for this module. This function
  checks the current RRC state and calls the event handler for
  that state. 
  
  Note that the Radio Bearer Reconfiguration procedure can be initiated
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
void rrcrbrc_procedure_event_handler
(
rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{

  /* Get the current RRC State */
  rrc_state_e_type rrc_state = rrc_get_state();

  WRRC_MSG2_HIGH_OPT("function rrcrbrc_procedure_event_handler is called in rrc_state = rrc_state_e_type_value%d and rbrc.curr_substate = rrcrb_substate_e_type_value%d", rrc_state, rbrc.curr_substate);
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_RBRC,rbrc.curr_substate,cmd_ptr->cmd_hdr.cmd_id);

  switch (rrc_state)
  {
  
  case RRC_STATE_CELL_FACH:    
    /* If it's CELL_FACH state, call the event handler
    for CELL_FACH state */
    rrcrbrc_cell_fach_event_handler(cmd_ptr);
    break;

  case RRC_STATE_CELL_DCH:    
    /* If it's CELL_DCH state, call the event handler
    for CELL_DCH state */
    rrcrbrc_cell_dch_event_handler(cmd_ptr);
    break;

    case RRC_STATE_URA_PCH:
    case RRC_STATE_CELL_PCH:
      /* If it's CELL_DCH state, call the event handler
      for CELL_PCH state */
      rrcrbrc_pch_event_handler(cmd_ptr);
      break;

    case RRC_STATE_CONNECTING:   
    case RRC_STATE_DISCONNECTED: 
    /* If it's any other state call the event handler
    for other states. The processing of events in
    other RRC states is identical for this procedure */
    rrcrbrc_other_state_event_handler(cmd_ptr);
    break;

  default:  
    WRRC_MSG1_ERROR("Invalid RRC State: %d", rrc_state);
    break;
  }  
}/* end function rrcrbrc_procedure_event_handler */


/*===========================================================================

FUNCTION rrcrbrc_build_rbrc_complete_message

DESCRIPTION
  This function builds the RBRC Complete Message.
  
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS

  None

===========================================================================*/
void rrcrbrc_build_rbrc_complete_message
(
rrc_RRC_TransactionIdentifier trans_id,     /* Transaction Id */
rrc_UL_DCCH_Message *msg_ptr,               /* Pointer to Uplink DCCH Msg */
boolean              srns_reloc,            /* Indicates whether SRNS reloc is involved */ 
boolean              cipher_update_required /* Indicates whether TM ciphering is present */
)
{                                         

  /* Select the message as RBRC Complete message */
  msg_ptr->message.t = T_rrc_UL_DCCH_MessageType_radioBearerReconfigurationComplete;

  /* Initialize the bit mask not to include the optional fields */
  RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.radioBearerReconfigurationComplete);

  if (srns_reloc == TRUE)
  {
    rrcrbrc_append_start_list_to_rbrc_complete(msg_ptr);
  }

  if((cipher_update_required == TRUE) && (TRUE == rrcsmc_tm_ciph_active()))
  {
    if(! (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.radioBearerReconfigurationComplete,
           rrc_RadioBearerReconfigurationComplete,ul_CounterSynchronisationInfo)))
    {
      rrcrbrc_append_start_list_to_rbrc_complete(msg_ptr);
    }
  
    //Set the bitmask for count_c_act_time and copy the same..
    RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.radioBearerReconfigurationComplete,
      rrc_RadioBearerReconfigurationComplete,count_C_ActivationTime);
    
    msg_ptr->message.u.radioBearerReconfigurationComplete.count_C_ActivationTime =
      rrcsmc_step_2_cipher_config_for_hho();
  
    //Send step 2 config for HHO
    rbrc.hho_ciph_status = FALSE;      
  }
  
  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
  {
 #ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
    if(rrcsib_is_dmcr_ie_needed() == TRUE)
    {
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.radioBearerReconfigurationComplete, laterNonCriticalExtensions);
 
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.radioBearerReconfigurationComplete.laterNonCriticalExtensions);
 
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.radioBearerReconfigurationComplete.laterNonCriticalExtensions, v770NonCriticalExtensions);
 
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.radioBearerReconfigurationComplete.laterNonCriticalExtensions.v770NonCriticalExtensions);
 
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.radioBearerReconfigurationComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        radioBearerReconfigurationComplete_v770ext);
 
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.radioBearerReconfigurationComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        radioBearerReconfigurationComplete_v770ext, deferredMeasurementControlReading);
      
      msg_ptr->message.u.radioBearerReconfigurationComplete.laterNonCriticalExtensions.v770NonCriticalExtensions.
        radioBearerReconfigurationComplete_v770ext.deferredMeasurementControlReading = rrc_RadioBearerReconfigurationComplete_v770ext_IEs_deferredMeasurementControlReading_true;      
    }
#endif
  }
  /* Initialize the Transaction Id */
  msg_ptr->message.u.radioBearerReconfigurationComplete.
  rrc_TransactionIdentifier = trans_id;

} /* rrcrbrc_build_rbrc_complete_message */




/*===========================================================================

FUNCTION  RRCRBRC_INIT_PROCEDURE

DESCRIPTION

  This function initializes the RBRC procedure.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcrbrc_init_procedure( void )
{
  uint32 count;

  rrcrbrc_send_initiate_cu_flag = FALSE;

  /* Initialize the transaction id's for this procedure.
  Use a number that can't be an actual received transaction
  id. */
  rbrc.tr_ids.accepted_transaction = RRCRB_INVALID_TRANSACTION_ID;
  rbrc.tr_ids.rejected_transaction = RRCRB_INVALID_TRANSACTION_ID;

  /* Initialize the current substate */
  rbrc.curr_substate = RRCRB_INITIAL;
 
  /* Initialize New CRNTI valid to FALSE in the begining */
  rbrc.new_crnti_valid = FALSE;
  rbrc.directed_cell_info = FALSE;
  rbrc.status.failure_status = RRCRB_MSG_VALID;
  /* set cn_info_saved to false */
  rbrc.cn_info_saved = FALSE;

  /* Indicate that there are no RBs to be released and
  no rbs tobe affected. This is the default value. */
  rbrc.rbs_tobe_reconfigured.is_valid = FALSE;
  rbrc.rbs_tobe_affected.is_valid     = FALSE;

  /*clear the SRNS relocation flag */
  rbrc.rbrc_for_srns_relocation = FALSE;

  rbrc.hho_ciph_status = FALSE;

  /* Initialize the RABS per message structure */
  for ( count=0; count < RRCRB_MAX_RAB_PER_MSG; count++ )
  {
    rbrc.rabs.rabs_in_msg[count].num_rbs_for_rab = 0;
    rbrc.rabs.rabs_in_msg[count].rab_type = RRCRB_UNKNOWN_RAB;    
    rbrc.rabs.chan_config_needed[count] = FALSE;
#ifdef FEATURE_VOC_AMR_WB
    rbrc.rabs.rabs_in_msg[count].nas_Synchronisation_Indicator = MVS_MODE_NONE;
#endif /*FEATURE_VOC_AMR_WB*/

  }
  rbrc.rabs.num_rabs_in_msg = 0;

  rbrc.high_mobility_ind = FALSE;

  /* Register for state change notification -
  We want to be notified when the state changes
  from CELL_DCH to CELL_FACH and from any state
  to disconnected state. */
  rrcscmgr_register_for_scn( RRC_PROCEDURE_RBRC,  /* Procedure name */
                             RRC_STATE_CELL_DCH, /* From State     */
                             RRC_STATE_CELL_FACH /* To State       */
                           );
  rrcscmgr_register_for_scn( RRC_PROCEDURE_RBRC,  /* Procedure name */
                             RRC_STATE_CELL_DCH, /* From State     */
                             RRC_STATE_CELL_PCH /* To State       */
                           );
  rrcscmgr_register_for_scn( RRC_PROCEDURE_RBRC,  /* Procedure name */
                             RRC_STATE_CELL_DCH, /* From State     */
                             RRC_STATE_URA_PCH /* To State       */
                           );
  rrcscmgr_register_for_scn( RRC_PROCEDURE_RBRC,  /* Procedure name */
                             RRC_STATE_WILDCARD, /* From State     */
                             RRC_STATE_DISCONNECTED /* To State    */
                           );

  rrcscmgr_register_for_scn( RRC_PROCEDURE_RBRC,  /* Procedure name */
                             RRC_STATE_CELL_FACH, /* From State     */
                             RRC_STATE_CELL_PCH /* To State       */
                           );

  rrcscmgr_register_for_scn( RRC_PROCEDURE_RBRC,  /* Procedure name */
                             RRC_STATE_CELL_FACH, /* From State     */
                             RRC_STATE_URA_PCH /* To State       */
                           );
  rbrc.initiate_cell_selection_trans_from_fach = FALSE;     

  /* Define a timer used to delay 5 seconds before
  sending the RB Reconfig Complete message */
  rex_def_timer(&rrcrbrc_temp_timer, rex_self(), RRCRBRC_TEMP_TIMER_SIG);
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

uint8 rrcrbrc_get_num_rbs_reccfg(void)
{
  return rbrc.rbs_tobe_reconfigured.num_rbs;
}
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

uint8 rrcrbrc_get_srns_relocation(void)
{
  return rbrc.rbrc_for_srns_relocation;
}

#endif
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
/*===========================================================================

FUNCTION rrcrbrc_set_failure_cause

DESCRIPTION

DEPENDENCIES


RETURN VALUE
  Returns 

SIDE EFFECTS

  None

===========================================================================*/
void rrcrbrc_set_failure_cause(rrcrb_msg_status_e_type cause)
{
  rbrc.status.failure_status = cause;
}

#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/

// #endif
