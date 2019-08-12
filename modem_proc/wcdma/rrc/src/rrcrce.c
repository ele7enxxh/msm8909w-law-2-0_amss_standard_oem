
/*===========================================================================

                                  R R C R C E

GENERAL DESCRIPTION
 The RRCRCE module consists of RRC CONNECTION ESTABLISHMENT PROCEDURE 
 implementation. The 3GPP TS 25.331 RRC Protocol specification is used as
 basis for implementation.

EXTERNALIZED FUNCTIONS

  rrcrce_init_procedure()
    This function will initialize the RRC Connection Establishment 
    procedure substate to RRCRCE_INITIAL Substate. This will called in the
    RRCTASK modules along with other Init Procedures.

  rrcrce_procedure_event_handler()

    This is the entry point for RRC Connection Establishment procedure. 
    All events for RCE come to this function from RRC Dispatcher. 
    Depending on the UE state of RRC Connection Procedure, it calls the
    corresponding state handler. These state handlers call substate 
    handlers depending on the the current substate of RRC Connection
    Establishment procedure.

Copyright (c) 2001-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcrce.c_v   1.84   13 Jun 2002 13:27:00   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcrce.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/05/16   sn      Changes for LTE U feature
05/09/16   sp      Made changes to add call end metrics - FR#30856
05/09/16   ad      Made changes to log important parms from OTAs
05/09/16   sp      Made changes for Mav debug snapshot
09/11/15   sr      Made changes for FR29345 EFS/NV Settings to be Read per Sub
04/23/15   vi      Made changes to add DMCR IE only when DMCR is supported in SIB3
04/02/15   vi      Made changes to set rsp_to_rrc_conn_req variable to RRC_CONNECTION_REJECT whenever rrc connection reject is received with interRAT info
03/24/15   sas     Made changes to trigger OOS scan if CSFB Est_req is aborted due to cell access restrictions.
01/21/15   sa      Made changes for QTF with CRM
01/06/14   bc      Made changes to support W2G CCO Abort mechanism.
11/21/14   bc      Changes to not to reset rrc_csfb_call_status on sending EST_CNF.
11/10/14   sp      Made changes to save procedure substates in crash debug info
10/17/14   sr      CSFB call optimization changes
09/18/14   sn      Changes to restore rrcrce_lai_rai_mismatch after LAI RAI mismatch check in rrcrce_abort_resel
08/12/14   bc      Made changes to send ABORT_IND with cause RRC_FAIL_TO_CAMP_ON when OOS occurs in 
                   either disconnected or connecting states.
08/22/14   rmn     Made changes to trigger FRLTE on RRC Conn delay timer expiry(started after CSFB Call release) 
                   after N/W retained conn in PCH state for more than rrc_fast_return_to_lte_after_csfb_timer_nv sec's.
08/04/14   bc      Made changes to fix compiler warnings.
07/23/14   sp      Made changes to remove F3s as part of FR19999
07/15/14   ac      NV reorganization
07/14/14   gv      Made changes to check for PPAC info for terminating cause "unknown"
07/11/14   vi      Made changes to convert earfcn type from uint16 to unti32
06/27/14   sn      Changes to prioritize MT call over reselection
06/23/14   sr      Made changes to unbar frequency info given in IE "redirection info" for 
                   connection reject and connection release OTA
05/06/14   geg     Use safer versions of memcpy() and memmove()
04/02/14   bc      Made changes to not to reject the connection after T300 expiry in single standby mode.
05/02/14   gv      Made changes to attempt IRAT activity only after clean up of pending BPLMN
04/10/14   sg      Made changes to correct the feature flag used for rel 10 spec CRs
03/26/14   sr      Optimization to release lock when conn reject is received
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/26/14   sn      Send abort confirm if RRC moves to disconnected state due to hard abort
                   when waiting for lock to resume after LTA
03/13/14   sn      Send abort confirm if RRC moves to disconnected state due to hard abort
09/25/13   ad      Made changes to split NV control of FDPCH and CPC DTX/DRX features
02/18/14   sr      Made changes to not send SCRI in case of hard abort
01/30/14   bc      Made changes to not handle OOS when waiting for channel config cnf
                   during connection establishment and handle it when channel config cnf is received.
01/08/14   bc      Fixed KW error to avoid NULL pointer dereferencing.
12/30/13   bc      Made changes to optimize REL 10 Spec CRs code
12/26/13   sn      Set DMCR IE in connection setup complete only when SIBs are 
                   deferred and SIB3 indicates DMCR
12/24/13   db      Introduced new NV to allow CPC,HSFACH,HSRACH,DC-HSDPA in DSDS and DSDA mode
12/09/13   sp      Made changes for OSYS 6.6.5 migration
12/03/13   mn      Moved all declarations to top of the code.
12/03/13   bc      Checked in REL 10 spec crs changes
11/14/13   sa      Made the changes for Rel-10 spec CRCR4566 under FR16756
10/30/13   as      Made changes to add feature code for FEATURE_WCDMA_3C_HSDPA
10/23/13   gv      Made changes to trigger FR2LTE only after a CS RAB gets established and then released
10/07/13   rmn     Changes for Feature: RAT Search Order for FDD/TDD LTE
09/16/13   mp      Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
09/11/13   gv      Made changes to clear the csfb call status when RRC connection is released
08/23/13   sr      Made changes to send PAGE_IND to NAS if CN page is received in PCH
08/14/13   vg      Made changes to implement a new diag event EVENT_WCDMA_RRC_FAILURE
06/25/13   sn      Block IRAT functionality in manual CSG mode
06/27/13   gv      Made changes to buffer the MM and RR commands for crash debugging
06/24/13   sr      Made changes to reset pre-redirection flag by checking V300 while sending ABORT IND
06/17/13   rmn     Made changes to avoid RRC triggering immediate local connection release on reception of SCRI L2 ACK.
06/07/13   vi      Replaced ALLOC_ASN1ARRAY1 with ALLOC_ASN1ARRAY2
06/05/13   vi      Added changes to fix NULL pointer access
05/15/13   db      Made changes to enhance NV 67333 to control FDPCH and CPC DTX/DRX features independently
04/25/13   sg      Added code for REL9 under FEATURE_WCDMA_REL9
04/18/13   sr      Added code changes to handle ABORT_REQ while waiting for lock
03/27/13   sr      Made changes to remove the IOT disabler flag for CSFB indicator
03/22/13   mp      Removed tmc.h include file
02/19/13   gv      Made changes to indicate NAS with new abort cause based on the domain when Access Class is barred
02/11/13   gv      Made changes to check for PPAC status when est cause is either registration or inter RAT reselection
02/04/13   ad      Made changes to add back the NULL pointer assignment
01/21/13   sr      Made changes to indicate the presence of SI container in RRC connection request message
01/03/13   ad      Made changes to reduce the redundant F3s
12/24/12   mp      added check not to set cause #11 in abort_cnf if EST_REQ is pending
12/20/12   sr      Added changes to ignore CCO from UTRAN when ABORT is in progress
12/18/12   sr      Made changes to include IE "csfbIndicator" in connection request for CSFB call 
                   and also ASN related changes
12/12/12   gv      Fixed compilation issue with HS RACH
12/04/12   ad      Fixed compilation errors in hsrach feature
11/30/12   ad      Made changes to remvoe the dependancy on  rfm_band_supports_dual_carrier
10/29/12   gv      Mainline the changes under FEATURE_PPAC
10/26/12   db      EFACH Redesign changes
10/23/12   pm      Made changes to assign pointer to NULL after de-allocation
10/18/12   gv      Replace FEATURE_DUAL_SIM_WCDMA_TUNEAWAY with FEATURE_DUAL_SIM
10/14/12   gv      Added code to support exchange of AS ID between WRRC and GRR commands
10/03/12   sr      Added code changes to set IDT establishment cause as given by upper layers
09/28/12   pm      Added NV support for MAC_I
08/25/12   sa      Made the changes to support HSRACH NV
09/03/12   gv      Added few comments
08/31/12   gv      Mainline the changes under feature FEATURE_WCDMA_ALLOW_ECAL_AC_BARRED_CELL
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/06/12   ad      Changes for HS-RACH feature
05/10/12   md      Added code changes to support FEATURE_MAC_I
05/11/12   sn      Changes to correctly log connection release cause
04/03/12   db      Made changes to disable CPC and FDPCH through NV
01/16/12   mn      Added handling of ABORT_REQ in Connecting state and WAIT_FOR_UE_CAMP_ON substate
01/12/12   gv      Added code to support FEATURE_PPAC
01/10/12   gv      Made changes to reset the release cause after sending ABORT/REL to NAS
11/20/11   db      Added changes to wait for l2ack in all states, not just DCH
                   and include reconfig status indicator in CU
11/16/11   ad      Changed the macro for FEATURE_WCDMA_PAUSE_MTPL_BACKOFF_VOICE_CALL 
11/16/11   sn      Added changes to fix NULL pointer access 
10/31/11   ad      Added changes to extend the CRT timer in PCH states to prevent 
                   out of sync between UE and NW if connection is released locally
10/31/11   sks     Added support for FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB.
10/28/11   mn      Made changes to allow legacy FD request in URA_PCH state
10/20/11   gv      Invoke function to send rab-rel-ind after RRC_REL_IND for 
                   PS rabs
09/12/11   gv      Renamed feature flag FEATURE_WCDMA_RRC_EMERG_CALL_SUPPORT
                   to FEATURE_WCDMA_ALLOW_ECAL_AC_BARRED_CELL
09/12/11   gv      Made changes to try for finding a cell which supports emergency call
                   when an emergency call is initiated and if the current cell 
                   does not support emergency call under feature FEATURE_WCDMA_RRC_EMERG_CALL_SUPPORT
09/05/11   gv      Fixed KW errors
08/30/11   ad      Made changes to handle default configuration 23
08/30/11   gv      Made changes to not include pre-redir info for the first attempt of RRC Connection
                   request msg following a LTOW redirection
08/12/11   sks     Made changes to handle state change ind in inactive state in
                   wait_for_srb_rel_cnf_substate for a WRLF scenario.
08/02/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.103.00
07/29/11   ad      Added code for FEATURE_WCDMA_PAUSE_MTPL_BACKOFF_VOICE_CALL feature
08/02/11   gv      Made changes to prevent RRC sending connection request on a no Access Cell
07/19/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.102.00
07/12/11   kp      Added code to not trigger Fast Dormancy in URA_PCH state
07/08/11   gv      Fixed KW errors
07/06/11   gv      Fixed KW errors
07/04/11   gv      Made changes to send capability request to LRRC only when LTE band mask 
                   is not zero
06/21/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.100.50
06/08/11   as      Made changes to not report CPC capability if Tuneaway is active
06/17/11   gv      Made changes to suspend BPLMN search when a call is initiated
                   under the feature FEATURE_WCDMA_CONTINUE_MPLMN
05/25/11   sks     Made changes to fix bugs when for indicating TDD support.
05/18/11   rl      Fixed the compiler error
05/17/11   sks     Made changes to correctly populate LTE TDD capabilities in OTA messages to n/w..
05/10/11   sks     Made changes to correctly fill GSM capabilities in Conn Setup Complete msg.
05/09/11   as      Added Null pointer access checks for srb setup list
05/03/11   su      Added code to drop UPDATE_SIB event while going to 
                   CONNECTING state when CSFB status is TRUE.
05/03/11   rl      Fixed the lint error
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/25/11   sks     Added support for new abort type for LAI change abort
04/20/11   vg      added support for mdsp halt recovery
04/26/11   sks     Added support for new abort type for LAI change abort
04/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.52
04/01/11   su      Made changes to correctly log event for RRC connection release cause.
04/01/11   sks     Fixed merge error.
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/28/11   sks     Made changes to trim RRC connection request msg if its size exceeds
                   allowed TM SDU size.
03/28/11   rl      Merged the HS-FACH/PCH changes from mainline
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/10/11   sks     Made changes to not include pre-redir info in RRC connection request
                   msg following a LTOW redirection.
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
01/12/11   rl      Migrated to VU_MODEM_WCDMA_RRC.01.88.50
12/28/10   sks     Made changes to validate EARFCN's provided in LTOW redirection info from n/w.
01/04/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.87.50
12/15/10   su      Fixed Compilation warnings.
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/29/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.85.50
11/22/10   rm      Made changes to indicate PCH UE pos capability
11/19/10   prk     Indicate e-fach capability if UE supports e-fach, irrespective
                   of network support.
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
11/02/10   rm      Added code to skip SIB7 update when CSFB call is active
09/20/10   prk/kp  Added code to support FEATURE_WCDMA_HS_FACH
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
09/10/10   rm      Made changes to enable RAT specific capability bit_mask only when it was requested by N/W
09/09/10   rm      Made changes to hold dedicated priorities during OOS
09/08/10   rm      Made changes to send the EUTRA specific capabilities to N/W
                   only when it was requested by N/W
08/24/10   rm      Fixed compiler warnings
08/23/10   rm      Fixed compiler warnings
08/17/10   sks     Made changes for PS abort, to release connection after receiving L2 ACK for SCRI.
08/16/10   ss      Added code to fill supportForFDPCH only in Rel7 and fixed compilation errors.
08/12/10   sks     Made changes to include pre-redirection in RRC connection request msg, if UE supports
                   one or more of the EUTRA bands indicated indicated in SIB19.
08/11/10   ss      Added code changes to fill supportForFDPCH only in Rel7
08/02/10   rm      Added code to abort connection on successful redirection to
                   access class barred cell.
07/23/10   ss      Added support to fill dc-hsdpa IE after checking rf support on current camped band
07/02/10   sks     Added code to support FEATURE_WRLF_SYSTEM_SEL for connected mode OOS.
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
06/22/10   kp      Fixed compilation errors
06/16/10   kp      Added code to indicate UE support of feature “Two DRX schemes in 
                   URA_PCH and CELL_PCH” and “E-DPDCH power interpolation formula”
                   for Rel-7 & above UE's.
03/18/10   gkg     Added support for sending Tx Diversity for DL Control Channels
                   for MIMO Devices i.e. IE supportofTxDivOnNonMIMOChannel.
05/10/10   gkg     Fixed Compile Warnings.
05/10/10   sks     Made changes to not start cell selection on L2 ACK for SCRI, if RCR is active. 
04/26/10   sks     Made changes to not send SCRI for PS abort from NAS.
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
03/12/10   dm      Added code for supporting Deferred SIB reading feature.
03/15/10   as      Updated RRC Code to support ASN.1 8.9 version
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
02/26/10   gkg     Removed the dependency of advertisement of F-DPCH capability on GCF flag.
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
01/11/10   gkg     Made changes not to advertise DC HSDPA capability if HSDPA CAT in NV 
                   isn't set from 21 to 24.
12/07/09   dm      Fixed linting errors
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
11/25/09   ss      Added Release Independent Band (FDD2) support.
11/13/09   sks     Added a check on the max number of GSM cells to be copied from the OTA message.
11/05/09   sks     Fixed compilation error.
11/04/09   sks     Added code to support inter-RAT redirection via connection reject
                   and both inter-RAT and inter-freq redirection via connection release.
10/26/09   sks     Fixed klocwork errors.
10/07/09   gkg\kp  Changes for REL8 under FEATURE_WCDMA_REL8 and CPC-DTX .
                   Also added EFDPCH capability in RRC Connection req
09/14/09   gkg     Made changes so that RRC Conn Request isn't resent when WAIT Timer
                   expires and Cell Access is INVALID.
07/20/09   rm      Modified code to initiate cell selection only when indicated by caller
06/05/09   ps      Made changes to not start the T305 timer is value is infinity
05/10/09   kp      Added support for demand paging RRC code using pragma.
05/08/09   ss      Updated Copyright Information
05/06/09   gkg     Made changes so that Freq Redirection is allowed through R7 RRC 
                   Connection Setup OTA.
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
04/09/09   ps      Made changes to send reconfig ind in CU/or go to idle
                   if RL failure is triggered while l2 ack for reconfig complete
03/28/09   rm      CMI Featurisation
03/25/09   gkg     Featurized code changes for High Mobility Indication. If 
                   FEATURE_WCDMA_REL7_SPECRS is not defined, no need to send
                   High Mobility Indication IE in RRC Conn Request.
03/23/09   ps      Add the defination of rrcrce_clear_num_retries_count() to 
                   clear the variable rrcrce_num_retries_count
03/09/09   kp      Added support for setting call type as other if EST_Cause is “Terminating Conversational Call”, 
                   this is as per Spec cr - R2-073821..
02/27/09   ps      Moved the code to enable FDPCH cability if 
                   nv_gcf_test_flag is TRUE to RCE
02/06/09   rm      Made changes to set supportOfHandoverToGAN to rrc_supportOfHandoverToGAN_present
                   only if dual mode is enabled 
01/30/09   ps      Made changes to not fill 6b0NonCriticalExtensions if
                   FDPCH is not defined and NV is set as REL6.if FDPCH 
                   is not defined,it should be filled only if NV is REL7 
01/21/09   kp      Bug in spec cr-3115 where the values of ‘CS call type’ in the tabular are 
                   not aligned with ASN.1 for RRC connection request message.So for now, set CS call 
                   type to others when rrc_establishment_cause is any thing other then 
                   RRC_EST_ORIG_CONV_CALL or RRC_EST_TERM_CONV_CALL or 
                   RRC_EST_EMERGENCY_CALL.
01/19/09   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
01/09/09   ps      Made changes to advertise FDPCH capbility if 
                   rrc_fdpch_capability_enable is set as TRUE
12/29/08   rm      Added support for 3G<->GAN Handover
12/26/08   sk      Abort RCE on unsupported redirection and trigger OOS to idle.
12/08/08   kp      Fixed Lint error. 
11/20/08   ps      Added protective checks before while extracting the 
                   IMSI/TMSI/PTMSI from UE identity present in the OTA msg
10/29/08   ns      Fixed compilation errors
23/10/08   gkg     Added support for events EVENT_WCDMA_CONN_REQ_CAUSE and 
                   EVENT_WCDMA_CONN_REL_CAUSE.
09/18/08   ps      Made changes to start T300 timer only after receiving 
                   RRC_CMAC_STATUS_IND from MAC
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
07/24/08   pk      Added handling for RRCRCE_WAIT_FOR_SIB7_CURRENT state in rrcrce_force_initialize().
07/14/08   gkg     Made Changes so that whenever RRC Connection Setup transitions to Cell_DCH
                   and there's change in Cell_Id, notification is sent to all the registered 
                   entities by invoking callbacks for Cell_id change.
06/09/08   pk      Extended the window to handle RCE state RRCRCE_WAIT_FOR_REDIRECT_CNF
                   for feature FEATURE_ABORT_ESTB_ON_RA_LA_MISMATCH.
05/29/08   pk      Added support to handle EST_REQ in RRCRCE_WAIT_FOR_RB0_REL_CNF state.
05/16/08   pk      Added support for Rove-in to GAN. Changes are under the feature 
                   flag FEATURE_GAN.
04/21/08   pk      Extended the window to handle more number of RCE states for feature 
                   FEATURE_ABORT_ESTB_ON_RA_LA_MISMATCH.
04/03/08   gkg     Added Support for DC with SRB5 (i.e. 11, 12, 13 and 14) through RRC
                   Conn Setup OTA.
03/07/08   vr\da   Support for sending Selected Services to network after RRC Connection Setup
                   procedure is complete 
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
22/02/08   ps/gkg  Added support for DC 3,4,6 to 10,15,16,18 to 21 through RRC
                   Connection Setup.
12/21/07   vr      Support for FEATURE MBMS.
11/29/07   vk      Set F-DPCH Capability in RRC Connection Req under FEATURE_WCDMA_FDPCH
11/28/07   vm      Added a new line at EOF to avoid compiler warning.
11/23/07   pk      Added support for a function which checks whether the currently camped RAI
                   and the one given in EST_REQ are same or not. Changes are under the feature
                   flag FEATURE_ABORT_ESTB_ON_RA_LA_MISMATCH.
11/19/07   kp      Added support for UE initiated dormancy under feature
                   flag FEATURE_UE_INITIATED_DORMANCY.
09/22/07   pk      Added support for starting T300 even if sending connection request is 
                   failed so that it is resent after T300 expiry. 
08/28/07   kp      Added support for capturing RACH transmission Statistic.
08/06/07   vm      Added fix for CR 124892. Added support to NOT send cell bar request
                   to L1 for n/w authorization failure when UE is NOT in service.
07/17/07   rm      Added support for FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING feature.
06/15/07   kp      Added support for removing GCF NV flag dependency on disabling 
                   Silent Redial with RRC Conn-Reject wait timer = 0.
06/15/07   kp      Added support for removing GCF NV flag for below scenarios.
                    1) Removing nv_gcf_test_flag dependency for code under 
                    feature flag FEATURE_RRC_HARD_ABORT.
                    2) Removing nv_gcf_test_flag dependency for handling ABORT 
                    RRC connection when received from upper layers, Start 3 seconds 
                    timer vs earlier 40sec timer.
06/11/07   vm      Fixed compilation error
06/05/07   pk      Added support to validate the cell access status for the requested 
                   domain before procedding further if one of the following happens during
                   the connection setup process.
                   1. Successful cell reselection.
                   2. NW Redirection to the redirected freq/RAT fails.
05/28/07   kp      Implementation of spec cr – 2765 “Indication of HSPA capability 
                   and conversation call type for CS”. Added CN doamin I.D & Call type
                   for CS domain support in rrc connection establishment message. 
                   CN domain I.D & call type are passed by NAS in EST_REQ, use these values
                   to fill in RCE message. These new fields will be used by N/W in deciding 
                   whether to set up SRB’s on EUL/HS transport channels
05/16/07   ps      Added usage of Standalone RB Mapping for SRBs
05/07/07   vm      Added fix for CR 115958. Removed check for ue identity altogether
                   as it is already done in rrc dispatcher file.
03/22/07   kp      Added code to handle NEW_CELL_IND in rce sub state wait for sib7 current. 
                   Now successful cell reselection will be treated as implicate SIB7 update.
01/09/07   da      Extern rrc_pdcp_enabled under feature FEATURE_UMTS_PDCP
01/03/07   da      Added ROHC capability in RRC Connection setup complete.
12/22/06   ps      Made changes to handle postVerificationPeriod and the
                           timingMaintainedSynchInd IE received in OTA messages
12/18/06   pk      Take UE immediately to the disconnected state when RRC receives
                   RRC_ABORT_REQ with hard abort flag being set to TRUE by calling 
                   the fucntion rrc_transition_to_disconnected_state()
11/23/06   segu    Supporting HARD ABORT cause from NAS in RRC_ABORT_REQ to make
                   sure RRC goes to disconnected state from connected state through 
                   INITIATE_CELL_SELECTION from CSP under the feature flag 
                   FEATURE_RRC_HARD_ABORT
10/20/06   sm      Added changes to remove padded number of bits from the length
                   included in CONTAINING IEs
10/18/06   ps      Added support for default config 22
09/25/06   kp      Added function which lets procedures know whether rce is in initial sub-state
09/24/06   sm      Fix to send rrc_ueCapabilityIndication in rrcConnReq when 
                   FEATURE_WCDMA_REL6 is defined
09/15/06   vm      Added support for the feature FEATURE_UI_RF_ENABLED_BANDS_SUPPORT.
09/12/06   sm/da   Fix code review comments and general clean-up for REL6 and HSUPA
09/05/06   vm      Removed the function call rrcrce_transition_to_disconnected_state() 
                   altogether and replaced it with existing common function 
                   rrc_transition_to_disconnected_state(). This has been done bcoz
                   the RCE specific function achieves the same functionality as the
                   common function and hence RCE specific function is not needed.
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/28/06   da      Added support for OOS area handling during transition from DCH.
08/21/06   tkk     Added SRB#5 support.
06/21/06   vr      Support to indicate rx_tx_TimeDifferenceType2 Capability
06/21/06   segu    return RRCRCE_SUCCESS in the function rrcrce_validate_cell_
                   access_status() if the est req cause is not RRC_EST_TERM_CONV_CALL  
06/08/06   vm      The function validate_dl_arfcn() used in this file now takes
                   additional arguments when the feature 
                   FEATURE_MCC_AND_850_800_BAND_COMBINATION is defined.
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE REL5 while retaining the HSDPA specifc processing 
                   under FEATURE HSDPA.
05/31/06   segu    An extra check of validating the current cell access status 
                   is added when RCE receives EST_REQ from NAS and also when
                   UE camps on after receiving the EST_REQ
06/23/06   vm      Added support to set the abort cause as RRC_MAX_ACCESS_FAILURE 
                   when RRC Connection Reject is received with wait_time = 0 and GCF
                   flag is enabled. Otherwise the abort cause stays as 
                   RRC_FAILURE_DUE_TO_CONGESTION. For details pls see CR 93247.
04/28/06   kp      Added support to populate the establishment cause of DT’s internal data
04/18/06   da      Modified code to support migration to Mar-06 Rel-5 ASN1 +
                   R6 Hacks
04/03/06   vr      More Lint Fixes
03/14/06   tkk     Lint warnings/errors cleanup
03/07/06   vr      Support to populate FDD2 bands in RRCConnectionSetupComplete message
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1
02/13/06   sg/tkk  Modified code to initiate cell selection in place of channel config for 
                   RB release during certain error scenarios
01/25/06   bd      Lint clean-up
01/23/06   vm      BPLMN Optimization for Automatically re-starting Manual 
                   BPLMN search after completion of LAU/RAU updates & added
                   support for not rejecting Manual PLMN search request when RRC state 
                   is Connecting/FACH/DCH and the establishment cause is Registration. 
12/21/05   sgk     Added check to verify if the remaining time on the T300
                   timer was > 0 on receiving the timer expiry indication and
                   if so ignore the expiry indication.
12/20/05   bd      Changed syntax for suppressing lint error 613
11/22/05   sgk     Added change to start the Signalling conn release delay timer
                   with value 0 if nv for gcf is turned off.  
11/10/05   sm      Added changes to enable switching between Rel99 and Rel5 using nv_item
11/10/05   siva    Using INITIATE_CELL_SELECTION instead of RRC_CHANNEL_CONFIG to LLC 
                   for state change to disconnected in race conditions related to 
                   connected mode 
11/08/05   vm      Added support for WTOW and WTOG Background PLMN Search. 
                   Added definition of new function rrcrce_is_rce_procedure_waiting_for_bplmn()
                   which is called by CSP before proceeding with WTOW/WTOG BPLMN search.
                   Also added support for sending SUSPEND_REQ to CSP if BPLMN search is 
                   in progress and appropriately handling SUSPEND_CNF in WAIT_FOR_UE_TO_CAMP_ON
                   RCE substate.
10/04/05   vr      Support for GTOW Packet Cell Change Order
10/03/05   tkk     Added support to handle signaling default configurations 
                   (0 & 1) received in Rel-5 RRC connection setup message.
09/29/05   tkk     Added support to handle frequency redirection + PSC through 
                   Rel-5 RRC connection setup message. 
09/28/05   vm      Added support for redirection to another frequency + psc via
                   RRC Connection Setup message.
09/02/05   ss      The functionality of translating the internal establishment cause 
                   to ASN equivalent is movedto rrcdata.c and hence 
                   update_establishment_cause is now callig the new function instead.
09/09/05   vm      Removed dependence of feature FEATURE_FACH_FREQ_REDIRECTION_IN_CONN_SETUP
                   on FEATURE HSDPA.
09/02/05   vm      Added support for sending release cause in RRC_ABORT_CNF.
08/05/05   da      Modified code to set flag exclude_modified_cell_results to 
                   false, when calling func rrcmeas_append_meas_rpt_on_rach.  This
                   is to allow adding of monitored cell results if present.
07/07/05   ttl     Fix the issue for not sending specific cap update for GSM 
                   in release 5 connection setup complete msg. 
07/05/05   vm      Added support for new feature FEATURE_FACH_FREQ_REDIRECTION_IN_CONN_SETUP
                   (when FEATURE HSDPA is not defined).
06/22/05   vk      Added the previous change under FEATURE HSDPA
06/09/05   vk      Added code to check if RRC Connection Release is rcvd. on CCCH
                   while verifying UE identity
06/06/05   da      Calling func rrcllc_print_trans_chnl_rate() after sending complete
                   msg to print trans chan rates.
05/27/05   vm      Cell bar request is now sent with an integer Tbarred timer 
                   value instead of an enum type
05/26/05   vk      Set in RRC Connection Setup Complete Message that rfc3095 
                   is unsupported for Release 5
05/05/05   vm/bu   Added support for T-barred Feature. 
                   (a) Included file "rrccsp_v.h"
                   (b) In DCH if network authentication check fails then the active set is barred
                       for max T-barred timer.
                   (c) In FACH or PCH states if network authentication check fails then the PSC
                       is barred for max T-barred timer.
05/03/05   bu      Added support to map the channel_config_cnf to the appropriate
                   channel_config_req using the transaction_id in CHANNEL_CONFIG_REQ/CNF.
                   If the transaction id in the CHANNEL_CONFIG_CNF doesn't match the
                   rce_chl_config_transaction_id, RCE ignores that CHANNEL_CONFIG_CNF.
04/06/05   vr      Include rrclogging.h unconditionally for logging on all platforms
04/05/05   da      Added function rrcrce_get_nv_items() that reads from NV
                   NV_GPRS_ANITE_GCF_I and sets flag nv_gcf_test_flag. This will 
                   indicate whether to wait for 40s  before sending signalling 
                   connection release.
04/04/05   bu/vm   Added support for inter-RAT redirection feature
                   . Added new substate RRCRCE_WAIT_FOR_RB0_PHYCHAN_RESETUP_CNF and 
                     corresponding new function 
                     rrcrce_process_rce_wait_for_rb0_phychan_resetup_cnf_substate().
03/23/05   vk      Added a print statement to access msg_version variable so that
                   compiler warning is taken care of when FEATURE HSDPA is
                   not turned OFF
03/11/05   vk      Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE HSDPA
03/03/05   vm      Added support for inter-frequency redirection feature.
                   Handled RRC_REDIRECT_CNF with different cause types for
                   both successful and unsuccessful frequency redirections.
                   Have also modified the action taken by RCE when it receives
                   ABORT_REQ from MM when waiting for redirect confirm from
                   CSP. Earlier, only RB_RELEASE request was being sent by RCE. 
                   Now, RCE sends INITIATE_CELL_SELECTION_IND to CSP with 
                   transition to disconnected state (which also releases RBs).
03/02/05  ttl      Support RRC ASN1 Release 5.
01/24/05   vk      In ura/cell pch handler, added more sub states to handle 
                   expiration of internally started RRC Timer
01/10/05   sgk     Added procedure id as second parameter to function call
                   rrcmeas_append_meas_rpt_on_rach, in function
                   rrcrce_build_rrc_connection_request_msg. 
01/07/05   da      Added check to discard connection setup message if cltd-2
                   is indicated in tx dev mode.
12/17/04   vr      Added support for WCDMA->1x handover feature
11/29/04   da      After RRC_PHY_CHAN_FAILURE_TIMER_EXPIRED_IND, changed rce
                   substate to RRCRCE_WAIT_FOR_RB0_REL_CNF and mm abort cause
                   to RRC_FAILURE_UNSPECIFIED.
11/22/04   da      Added support for phy chan setup failure and phy chan est 
                   failure for CONNECTING->CELL_DCH.  Added new timer 
                   RRCTMR_PHY_CHAN_FAILURE_TIMER.  Added new function
                   RRCRCE_TRANSITION_TO_DISCONNECTED_STATE.
11/18/04   vk      If abort flag received from MM is set to TRUE, then delay
                   the signalling connection release by starting the RRC
                   connection delay timer. Added a new sub-state to handle
                   the timer release expiration. 
09/17/04   kc      Synced up to new interfaces to send UE-Capabilities for mulit-band.
09/02/04   vk      Added release cause in print staement while sending release
                   indication to MM
08/30/04   sgk     Typecast variable i to rrc_initial_ue_id_choice_e_type in
                   function rrcrce_verify_ue_identity, to fix lint errors: Type
                   mismatch (assignment) (int/enum) and Converting enum to int.
                   Removed return after ERR_FATAL in function
                   rrcrce_send_rrc_connection_setup_complete_message to fix lint 
                   error: 'unreachable'.
08/25/04   vk      Ignore sending signalling connection release request to DT 
                   procedure if RCR is active and an abort is received from MM
08/05/04   bu      Filled the proc id in rrcrce_send_rrc_initiate_cell_selection_ind().
07/15/04   vk      Added debug statement to print establishment cause
07/02/04   vk      Corrected setting of no of mnc to 2 in TMSI scenario
06/23/04   vk      If MM specifies number of MNC as 3 and the last digit is a f in
                   RRC_EST_REQ, then flag an error and set no of MNC to 2 
                   so that RRC connection request can be successfully encoded by ASN1 
06/07/04   vk      Clean up changes as refered in CR 43355
05/17/04   vk      Instead of waiting for L2 ack for RRC Connection Setup Complete
                   Message in RRCRCE_WAIT_FOR_L2_ACK substate, directly go to
                   final substate
05/10/04   vn      Definition for rrcrce_force_initialize() to send the appropriate
                   response (if required) to MM and resets RCE state to INITIAL.
03/17/04   bu      Increment the num_retries counter only in wait for connectiun setup
                   state. Set the counter to 0 whenever connection setup or connection
                   reject is received. Set the send_abort_cnf to FALSE in 
                   rrcrce_send_rrc_abort_cnf().
           bu      Definition for rrcrce_get_num_retries_count(). num_retries_count
                   is incremented everytime RRC connection request reaches max re-tries.
03/19/04   kc      Added code for Gtow PCCO feature
03/18/04   bu      Merged changes from Cheetah mainline.
           vk      Set the send_abort_cnf to FALSE in rrcrce_send_rrc_abort_cnf().
           bu      Added more defensive code to ERR_FATAL only if mem allocation fails
                   or if mandatory parameters cannot be read.
           bu      Changed ERRs or MSG_HIGHs after memory allocation to ERR_FATALs.
                   Also added ERR_FATALs to all the failure cases in
                   rrcrce_send_rrc_connection_request_msg().
           kc      Added check to see if mem allocation was successful.
           vk      Send release to MM only if WCMDA is active.
03/15/04   bu      Removed the use of FEATURE DUALMODE BASELINE. This feature is
                   always defined (even for singlemode builds).
03/14/04   bu      More cleanup related to ABORT_REQ handling.
03/09/04   vk      Always send a ABORT_CNF to MM if an ABORT_REQ was received.
02/12/04   vk      In RRCRCE_WAIT_FOR_SRB_SETUP_CNF substate, ignore the invalid
                   checking of looking at RRC current state
02/12/04   vk      In rrcrce_change_substate_to_rrcrce_initial(), send abort ind
                   or abort cnf in RRCRCE_WAIT_FOR_RB0_REL_CNF substate or
                   RRCRCR_WAIT_FOR_SRB_REL_CNF sub state.
02/02/04   vk      Instead of waiting for L2 ack for RRC Connection Setup Complete
                   Message in RRCRCE_WAIT_FOR_L2_ACK substate, directly go to
                   final substate
12/10/03   vk      In RRCRCE_WAIT_FOR_SRB_REL_CNF substate, clear established
                   RAB list again on getting channel config confirm to
                   make sure that established RAB variable is always cleared.
                   Also, while waiting in RRCRCE_WAIT_FOR_SIB7_CURRENT substate,
                   check the status of RRC_SIB7_UPDATE_STATUS_IND cmd and 
                   send a abort indication to MM in case sib7 update failed.
                   Also added waiting for RRCRCE_WAIT_FOR_RESELCT_COMPL while
                   RRC is in disconnected sub-state
12/08/03   vk      Added RRCRCE_WAIT_FOR_RB0_REL_CNF in connecting sub state.
                   Also, in case rce receives another ABORT req in this
                   substate, it sends abort cnf only on receiving state change ind.
                   Removed checking for waiting for SIB7 update once RCE receives 
                   a new cell indication.
11/19/03   vk      Added a new sub-state RRCRCE_WAIT_FOR_RB0_REL_CNF that waits
                   for state change indication from state change manager to go
                   to disconnected state before sending abort indication to MM 
                   and then reinitializes procedure sub state to Initial
10/21/03   kc      Append START values to ABORT_IND to write back latest START
                   for each domain during an abnormal call release.
10/14/03   kc      Added support for new interfaces to append_start_list and
                   append_ue_capability
07/08/03   rj      Added code to reset ESTABLISHED_RABS and 
                   ESTABLISHED_SIGNALLING_CONNECTIONS if Radio Link failure
                   occurs when RCE waiting for an indication from RRCSCRR.
06/05/03   kc      Added Procedure_ID to rrcsmc_append_start_list function to
                   sync up to interface changes in SMC procedure.
05/23/03   svk     Added event EVENT_WCDMA_OUT_OF_SERVICE, EVENT_WCDMA_RRC_RB0_SETUP_FAILURE
05/21/03   rj      Added a function for an API to change the substate of RCE
                   to RRCRCE_INITIAL for Mode Change Controller.
05/06/06   rj      Added code to wait for RRC Connection Setup message if 
                   there is a cell reselection during RRC connection
                   Establishment.
04/24/03   rj      Added code to initiate SCRR procedure before taking UE to
                   Idle mode if active signalling connections are present
                   when MM aborts RRC Connection.
04/16/03   sk      Check V300 counter before trying to camp on other freq/rat
                   Check the bitmask for the presence of redirection info
                   in rrc connection reject message
03/18/03   rj      Added code to address a race condition between OOS and
                   RRC_ABORT_REQ from MM.
03/06/03   bu      Send REL_IND to MM only if RRC is in ACTIVE mode.
03/05/03   rj      Added code to process RRC_CPHY_OUT_OF_SERVICE_AREA_IND
                   in CONNECTING STATE and DISCONNECTED_STATE if feature
                   FEATURE_RRC_CELL_UPDATE_ERRORS_SUPPORTED is not defined.
02/27/03   rj      Provided an API to change the substate of RCE
                   procedure to RRCRCE_FINAL during G2W handover.
02/04/03   rj      Merged resvion #112 that are lost during the check-in for
                   linted changes.
02/04/03   ram     Checked in lint changes.
01/10/03   vn      Added new parameter proc_id in rrcllc_set_ordered_config.
12/31/02   rj      Added code to check whether cell reselection is in progress
                   before processing RRC COnnection Setup message. If cell 
                   reselection is in progress, then RRC Connection Setup 
                   message is trashed and RRC Connection Request is sent on
                   the new cell after successful cell reselction if 
                   V300 <= N300.
12/12/02   jh      Updated NUM_OF_BITS_FOR_CCCH_MSG_TYPE to
                   NUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE.
11/22/02   rj      Provided a new function rrcrce_is_rce_procedure_inactive
                   that indicates if RCE procedure is inactive or active so 
                   that cell reselection can be postponed.
10/28/02   rj      Updated code to fix RRC Connection Request message
                   re-transmissions.
10/24/02   rj      Updated to use rrc_malloc and rrc_free to use dual heaps 
                   in place of mem_malloc and mem_free to avoid memory
                   fragmentation
09/20/02   bu      Changed l1_status to chan_cfg_status in channel_config_cnf.
09/12/02   rj      Added a check to know whether GSM is supported before 
                   appending System Specific Capabilities. Also renamed 
                   feature FEATURE DUALMODE BASELINE to 
                   FEATURE INTERRAT HANDOVER WTOG.
09/09/02   rj      Initialized system specific Capability Update Requirement
                   to FALSE before processing RRC Connection Setup message.                   
09/03/02   rj      Updated function rrcrce_process_rce_wait_for_l2_ack_substate
                   to send RRC_ABORT_IND to MM if RRC transtions to Idle
                   Mode before L2 ACK is received for RRC Connection Setup
                   Complete Message.
08/16/02   rj      Added code to initialize "LAC" IE while sending RRC 
                   Connection Reuqest using PTMSI and RAI as Initial UE
                   Identity.
08/07/02   rj      Deleted Check for System Specific Capability Update Request
                   in the RRC Connection Setup message and added code to IE
                   "UE system specific capability" for dual mode operation.
06/13/02   rj      Deleted code that updates U-RNTI since U_RNTI data 
                   structure from RRCDATA module is deleted.
06/03/02   rj      Added code to capture receival of RRC_ABORT_REQ without
                   receiving RRC_EST_REQ from MM. Also added code to print
                   V300 counter in the F3 messages.
06/02/02   rj      Added code not to send first RRC Connection Request message
                   on the Uplink until SIB7 becomes current.
04/24/02   rj      Added code to initiate RRC Connection Establishement with 
                   cause RRC_EST_TERM_UNKNOWN.
04/22/02   rj      Added code to reset the integrity protection started status
                   on entering connected mode.
04/19/02   rj      Updated function rrcrce_validate_rrc_est_req to check for
                   minimum number of IMSI digits if RRC_EST_REQ contains IMSI
                   type Initial UE Id.
04/10/02   rj      Updated function rrcrce_process_rce_initial_substate to 
                   forward the initial integrity & Ciphering Info received
                   from MM to RRCSMC module .
03/29/02   rj      Included rrcsmc.h and renamed function rrc_append_start_list
                   to rrcsmc_append_start_list for Integrity Protection support.
                   Also modified rrcrce_send_rrc_rel_ind to update the Max 
                   HFNs from the Security Mode Command procedure.
03/25/02   rj      Fixed CR#21327 not to  start timer T305 in CELL_DCH state.
03/12/02   rj      Modified function rrcrce_send_rrc_channel_config_req_for_srb
                   not to include RB0 in the RRC_CHANNEL_CONFIG_REQ.
03/08/02   rj      Fixed some mismatches with previous check-in.
03/07/02   rj      Added code to convert additional RRC_EST_TERM_HI_SIGNALLING
                   and RRC_EST_TERM_LOW_SIGNALLING causes as per the June
                   Specification.
03/05/02   rj      Renamed varible rrc_u_rnti to rrc_u_rnti_internal.
03/04/02   rj      Added code to wait until RRC goes Idle Disconnected state
                   before sending RRC_ABORT_CNF in response to RRC_ABORT_REQ
                   command from MM.
02/28/02   rj      Added functions rrcrce_process_rce_wait_for_l2_ack_substate
                   and rrcrce_process_rce_wait_for_cu_complete_cnf_substate
                   to DISCONNECTED state to process RRC_STATE_CHANGE_IND command.
                   Fixed some bugs from previous check-in. Modified function
                   rrcrce_send_rrc_initiate_cell_selection_ind to include
                   destination state. Moved function that appends START values
                   to RRCDATA module and added code to start timer T305 after
                   entering CELL_FACH state.
02/16/02   rj      Removed clearing of bit_mask in function 
                   rrcrce_build_rrc_connection_request_msg since it was not
                   set before.
02/14/02   rj      Updated copyright for 2002.
02/14/02   rj      Added code to initiate CELL UPDATE proc if there is no
                   valid C-RNTI in CELL_FACH state. Added a new substate
                   RRCRCE_WAIT_FOR_CU_COMPLETE_CNF for CELL_FACH state 
                   implementation and added code to process events in 
                   RRCRCE_WAIT_FOR_CU_COMPLETE_CNF substate. Deleted code
                   that updates C-RNTI.
01/31/01   rj      Added code to honor RRC_NEW_CELL_IND command in substate
                   RRCRCE_WAIT_FOR_REDIRECT_CNF.
12/21/01   rj      Fixed some logical corrections for bit mask manipulations.
12/03/01   rj      Fixed a typo in PTMSI comparision.
11/21/01   rj      Updated bit mask checking to a more readable format.
11/21/01   rj      Updated code to be compatible for June release of 25.331 
                   RRC Protocol Specification.
11/13/01   rj      Fixed a bug while encoding MNC values for TMSI-LAI Initial
                   UE Id and added more F3 messages. Corrected Previous 
                   check-in.
11/05/01   rj      Fixed 5 digit PLMN comparision in the function
                   rrcrce_verify_ue_identity and Removed check for Capability
                   Update Requirement IE.
11/01/01   vn      Substitute a call to RRC_SAME_PLMN with rrc_bcch_plmns_match.
10/31/01   rj      Updated function rrcrce_validate_rrc_connection_setup_msg
                   not look for presence of RB Identity since RB Identity is
                   an optional IE.
10/25/01   rj      Added code to send RRC_ABORT_IND to MM if UE goes out of
                   service area in Connecting state.
10/24/01   rj      Initialized uninitialized vars to remove compiler warnings
10/18/01   rj      Added code to include Optional MNC field while peeking 
                   into Initial UE Id for TMSI and PTMSI in function 
                   rrcrce_verify_ue_identity.
10/17/01   rj      Added code to trigger cell selection if RB0 setup fails.
10/03/01   rj      Added F3 message to track heap when RRC Connection is
                   initiated.
09/26/10   rj      Added additional check for Initial UE Id matching in
                   function rrcrce_verify_ue_identity   
09/19/10   rj      Modified F3 message text to maximum length of 40 charecters.
09/18/01   rj      Added a new interface to know the RRC Connection status.
                   This interface will be used by the Initial Direct
                   Transfer procedure.
09/14/01   rj      Modified functions rrcrce_send_rrc_connection_request_msg 
                   and rrcrce_send_rrc_connection_setup_complete_message to 
                   check for RLC Id before allocating memory for uplink
                   message. Fixed a bug in Initial UE Id matching. Added F3
                   message to declare critical extensions are not supported.
09/14/01   kc      Added protocol error log handling code
09/06/01   rj      Updated function rrcrce_build_rrc_connection_request_msg
                   to the latest RRCMEAS interface and deleted function
                   rrcrce_get_max_no_of_reported_cells_on_rach since this
                   functionality is moved to RRCMEAS module.
09/03/01   rj      Fixed transaction Id validation in function 
                   rrcrce_validate_rrc_connection_setup_msg
08/30/01   rj      Added code to peek into OTA message and identify Initial
                   UE Id received from the RRC Connection Setup and RRC
                   Connection Reject even though ASN1 decoding is failed. If 
                   the received Initial UE Id is matched, the protocol error
                   indicator will be set to error occures in the subsequent 
                   RRC Connection request re-transmissions. Updated function
                   rrcrce_append_start_list to send PS domain dummy START 
                   value through RRC Connection Setup compelte message. Added
                   code to check for critical extensions. If critical extensions
                   are present the OTA message is ignored. Added code to 
                   identify as protocol error if ASN1 decoding is failed. Added
                   comments to specify that multiple transactions are not
                   supported. Changed UE_MODE_ACKNOWLEDGED_DATA_PLANE to 
                   UE_MODE_ACKNOWLEDGED_DATA. Removed rrcrce_dpch_info_present
                   since it is no longer required.                   
08/29/01   vk      Added header file rrcmeasi.h and removed rrcmeas.h.
                   Changed the name of the function rrc_meas_append_rpt_on_rach
                   to rrcmeas_append_rpt_on_rach. Also changed the type of the
                   input parameter of the fn from void* to ptr to
                   MeasuredResultsonRACH. Changed the name of enum type returned 
                   by the function and also deleted some of the return values.
08/03/01   rj      Added processing of  substates RRCRCE_WAIT_FOR_REDIRECT_CNF,
                   and RRCRCE_WAIT_FOR_WAIT_TIMER_TO_EXPIRE in function
                   rrcrce_process_connecting_state. Deleted code stop the timer 
                   RRCTMR_WAIT_TIME_TIMER when the timer is expired in function
                   rrcrce_process_rce_wait_for_wait_time_expire_substate.
07/26/01   rj      Added implementation to process RRC Connection Reject message.
                   Added code to send RRC_ABORT_CNF in response to RRC_ABORT_REQ.
                   Modified debug statements for OTA messages to MSG_HIGH from
                   MSG_LOW. Added code to send RRC_REL_IND to the MM from RRCRCR
                   module to take care of all types RRC Connection Releases.
                   Updated function rrcrce_verify_ue_identity to the latest types
                   of MCC and MNC. Adde code to clear the ESTABLISHED_RABS and 
                   ESTABLISHED_SIGNALLING_CONNECTIONS while bringing down RRC
                   Connection.
07/20/01   rj      Moved functions rrcrce_append_ue_radio_access_capability and 
                   rrcrce_append_gsm_specific_capability to RRCUECI module and 
                   function rrcrce_copy_capability_update_requirement to RRCUECE
                   module. Included rrcuece.h and rrcueci.h for the above 
                   functions' interfaces. Added a debug statement in function
                   rrcrce_send_rrc_abort_ind.
07/05/01   rj      Initialized all optional fields to ommitted while building
                   RRC Connection Request message in function 
                   rrcrce_build_rrc_connection_request_msg.
06/29/01   rj      Registration for state change from any connected mode states
                   to Disconnected state is added to powerup initialization
                   function. And registration for the same in RRC Connection
                   Establishment procedure is commented. This needs to be undone
                   when De-registration is implemented in the State Change
                   Manager.
06/28/01   rj      Deleted code that was wrapped with feature 
                   FEATURE_RRC_RELEASE_1_0_BETA. This code was processing
                   RRC Connection Reject message.
06/28/01   rj      Fixed a bug for ASN1 encoding failure and added code to 
                   initialize the bit mask for RRC Connection Setup Complete
                   message. Removed initialization of bit mask for Integrity
                   Check info since it is initialized in Send Chain.
06/26/01   ram     While sending a RRC Connection Setup Complete message, the
                   bit-mask to indicate Integrity protetion status was not
                   being set. Set the bit-mask to 0 indicating integrity 
                   check info is not included.
06/20/01   ram     The structure "new_state_ind" was being used incorrectly
                   in several places when a RRC_STATE_CHANGE_IND command was
                   received. The correct structure to be used is "state_change_ind".
                   Changed all uses of new_state_ind to state_change_ind.
06/19/01   vn      Changed rrcrce_process_rce_final_substate to look at 
                   state_change_ind instead of new_state_ind command fields.
                   Call rrcrce_process_rce_final_substate from 
                   rrcrce_process_disconnected_state so as to process the State
                   Change Ind to DISCONNECTED which gets to RCE only after the 
                   RRC state has been changed to DISCONNECTED state.
06/06/01   rj      Changed formal parameters to the functions 
                   rrcrce_append_ue_radio_access_capability and 
                   rrcrce_append_gsm_specific_capability. Updated missing capability
                   fields in the function rrcrce_append_ue_radio_access_capability
                   and added more comments to the UE Capabilities.
05/24/01   rj      Number of RRC Connection Request retransmissions is corrected 
                   to N300 from N300+1.
05/24/01   rj      Fixed a bug in function rrcrce_validate_rrc_connection_setup_msg
                   while validating RRC Connection Setup message.
05/18/01   ram     The internal format for PLMN ID has been changed to use
                   arrays instead of integer to store MCC and MNC. Updated
                   this module to use the new format.
05/15/01   rj      Updated function rrcrce_validate_rrc_connection_setup_msg
                   with validations for transaction Ids and modified function
                   rrcrce_process_rce_wait_for_conn_setup_substate suitably
                   to reflect the validations.
05/07/01   rj      Assigned START value length to a define constant in function
                   rrcrce_append_start_list.
05/04/01   rj      Changed local parameter initial_ue_identity_ext in function
                   rrcrce_update_initial_ue_identity to variable
                   instead of pointer to variable to avoid a warning.
05/02/01   rj      Fixed a potential memory leak in function 
                   rrcrce_send_rrc_connection_setup_complete_message.
05/02/01   rj      Updated function rrcrce_update_initial_ue_identity with the
                   fixes. Added checks for memory allocations. Updated count
                   for Inter System Capability Update Requirement in function
                   rrcrce_copy_capability_update_requirement. Added more 
                   comments and fixed some typos.
04/30/01   rj      Modified function rrcrce_copy_capability_update_requirement
                   to copy the external format Capability Update Requirement to
                   internal format. Updated code that references
                   Capability Update Requirement in internal format.
                   Deleted rrcrce_free_capability_update_requirement function.
04/29/01   rj      Added code to update C-RNTI and U-RNTI in internal format by
                   using translate functions. Added code to check status field
                   in RRC_DOWNLINK_L2ACK_IND command.
04/09/01   rj      Uncommented inclusion of customer.h
04/08/01   rj      Add Code print a message when RRC Connection Setup
                   Complete message is semt by the RCE procedure.
04/04/01   rj      Replaced RRC_NEW_STATE_IND with RRC_STATE_CHANGE_IND. Fixed
                   a bug in the function rrcrce_send_rrc_connection_request_msg.
                   Modified function rrcrce_compare_arrays to remove warnings 
                   for ARM Compiler.
03/30/01   rj      Fixed a compilation error associated with 
                   FEATURE_RRC_RELEASE_1_0_BETA.
03/30/01   rj      Updated to be compatible with latest LED based rrcasn1.h and
                   latest rrcmmif.h. Also updated for 25.331 December release.
                   Changed Initial UE Identity in RRC DATA module to internal 
                   format from external. Modified function 
                   rrcrce_update_initial_ue_identity,
                   rrcrce_update_establishment_cause,
                   rrcrce_send_rre_est_cnf. Lots of code change for December
                   Spec. Added new functions rrcrce_send_rrc_anort_ind and
                   rrcrce_validate_rrc_est_req.
03/30/01   ram     Included customer.h header file.
02/28/01   rj      Fixed the interface with RRC LLC. Added next state to the
                   rrcllc_set_ordered_config function.
02/28/01   ram     Made temporary fix for an incompatibility between llc and rce
                   in the function rrcllc_set_ordered_config. A more permenant
                   change will be made later.
02/26/01   rj      Added code to transmit RRC Connection Request if V300 <=
                   N300 and if unsupported IEs are present in the  RRC 
                   Connection Setup message or if Mandatory IEs are not
                   present.
02/22/01   rj      Fixed some typos that are hidden under the feature
                   FEATURE_RRC_RELEASE_1_0_BETA. 
02/22/01   rj      Added code to stop timer for UE to camp on when 
                   RRC_ABORT_REQ is received in RRCRCE_WAIT_FOR_UE_CAMP_ON
                   substate. Added more comments. Modified code to 
                   increment the re-transmission counter after RRC Connection
                   Request message is successfully sent. Added code to stop
                   the Timer T300 when a valid event receievd in 
                   RRCRCE_WAIT_FOR_RRC_CONN_SETUP substate. Modified code 
                   not to increment the re-transmission counter when Initial
                   UE Identity is not matched. 
02/21/01   rj      Updated function rrcrce_update_initial_ue_identity_choice
                   to convert MCC and MNC values to ASN1 format. Updated 
                   function rrcrce_get_max_no_of_reported_cells_on_rach to 
                   to validate SIB11 for including measurement report on RACH.
                   Fixed a bug in  rrcrce_build_rrc_connection_request_msg
                   function. Changed the logic to validate the mandatory IE
                   "Capability Update Requirement" in the function
                   rrcrce_process_rce_wait_for_conn_setup_substate. 
02/12/01   rj      The command RRC_TIMER_FOR_L2ACK_EXPIRED_IND is changed to 
                   RRC_TIMER_FOR_RCE_L2ACK_EXPIRED_IND. And the enum for RCE
                   L2ACK timer is changed to RRCTMR_RCE_L2ACK_TIMER from
                   RRCTMR_WAIT_FOR_L2_ACK_TIMER. Fixed some typos in the 
                   comments.
02/08/01   rj      Updated comments. Added a new local variable 
                   rrcrce_number_of_rbs. Deleted function 
                   rrcrce_send_rrc_channel_config_req. Updated code for the 
                   latest changes in Cell Change Manager. The code to send 
                   RRC Connection Request message is moved to the function
                   rrcrce_send_rrc_connection_request_msg. Fixed state machine
                   bugs captured during unit testing. Updated code for the
                   latest changes in SEND CHAIN for Acknowledeged mode 
                   Transmission.Added a new function to validate RRC Connection
                   Setup message.Removed unwanted local variables from the 
                   rrcrce_process_rce_wait_for_conn_setup_substate function.
                   Fixed the processing of RRC_NEW_CELL_IND as per latest
                   understanding. Added code to make no dangling radio 
                   resources when RRC fails to establish RRC Connection. 
                   Removed intialiation of integrity info in function
                   rrcrce_send_rrc_connection_setup_complete_message since
                   it is done in the SEND CHAIN.
01/30/01   rj      Updated to be compatible with theb latest changes in RRCLCM
                   module.
01/26/01   rj      Correct date in the edit hostory.
01/25/01   rj      Tons of changes for for new RRC LLC interface, new State
                   change mananger and new cell change manager interface. Removed 
                   rrcrce_radio_bearer_config_req function. Added new functions
                   rrcrce_update_initial_ue_identity_choice, 
                   rrcrce_update_establishment_cause, 
                   rrcrce_send_rrc_channel_config_req_for_rb0,
                   rrcrce_get_max_no_of_reported_cells_on_rach and
                   rrcrce_send_rrc_channel_config_req. Modified 
                   rrcrce_send_est_cnf functions.                     
01/24/01   rj      Moved the boolean rrcrce_dpch_info_present into this file
                   since it is internal to the RCE Procedure. Added new 
                   include files for new state change manager and cell change
                   manager. Relooked into all ERR messages and converted them
                   into MSG_HIGH and MSG_MED depending on the context. If 
                   memory allocation fails for sending commands, it is treated
                   as a ERR_FATAL. Used string.h functions and deleted the 
                   smarts from the code.                   
01/19/01   rj      Moved all declarations that are internal to RRCRCE module
                   to rrcrcei.h. Updated code  as per code review inputs.
                   Renamed rrcrce_init_rce_procedure function to  
                   rrcrce_init_procedure. Renamed rrc_rce_procedure function
                   to rrcrce_procedure_event_handler. Removed unwanted 
                   include files.
11/20/00   rj      Created file.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"
#include "rrcasn1.h"
#include "rrcrce.h"
#include "rrcrcei.h"
#include "rrcdata_v.h"
#include "rrccmd_v.h"
#include "rrcccm.h"
#include "rrclcm.h"
#include "mmtask.h"
#ifdef FEATURE_CMI
#include "mmtask_v.h"
#endif
#include "rrcsend.h"
#include "rrcmeas.h"
#include "rrcllc.h"
#include "rrcsibdb.h"
#include "rrcscmgr.h"
#include "rrctmr.h"
#include "string.h"
#include "rrcuece.h"
#include "rrcueci.h"
#include "rrcdt.h"
#include "bit.h"
#include "rrcsmc.h"
#include "rrcsibproc.h"
#include "rrcmcm.h"
#include "rrcnv.h"
#include "rrclogging.h"
#include "event.h"
#include "rrcrcr.h"
#include "rrcasn1util.h"
#include "rrccsp.h"
#include "rrccspi.h"
#include "rrccho.h"
#include "rrcmisc.h"
#include "mm_as_types.h"
#include "rr_rrc_if.h"
#include "rrcsibdbi.h"
#include "rrcpg1.h"
#include "cm.h"

#ifdef  FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
#error code not present
#endif

#include "rrcmisc.h"
#include "rrccu.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif

#ifdef FEATURE_DUAL_SIM
#include "rrcwrm.h"
#endif

#ifdef FEATURE_WCDMA_TO_LTE
#include "lte_rrc_ext_api.h"
#endif

#ifdef FEATURE_WCDMA_HS_RACH
#include "rrchsrachcfg.h"
#endif

#include "stringl.h"
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#ifdef FEATURE_UMTS_PDCP
extern boolean rrc_pdcp_enabled;
#endif /* FEATURE_UMTS_PDCP */
/* Flag to indicate whether to wait for 40 seconds before sending 
   signalling connection release */
boolean    nv_gcf_test_flag;

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
#define RRC_DEFAULT_DRX_CYCLE_MS 2560
#define RRC_NUM_DRX_CYCLE_BEFORE_WTOL_FAST_REDIR 2
/* Min Guard timer value needs to be 3s. 2sec for SIB reading during DCH->PCH trans
   and 1sec for CU after PAGE reception. */
#define RRC_GUARD_TIMER_FOR_PCH_TRANS_BEFORE_WTOL_FAST_REDIR 3000
rrcrce_csfb_status_e_type csfb_extended_status = CSFB_INVALID;
static boolean rrc_crt_restarted_once = FALSE;
extern boolean rrc_cs_rab_established;
#endif

typedef struct rrcrce_default_config_info_s {
  rrc_config_e_type   config_type;  
  struct rrc_SysInfoType16 * sib16_ptr;      /*ptr to sysInfoType16       */
  /* Pointer to DL DCCH message */
  rrc_DL_DCCH_Message     *dcch_msg_ptr;
  struct rrc_DL_DCCH_Message * rbs_ptr;    /*ptr to R5 RBS for DCs*/

  uint32              default_config_id;     /*default configuration ID   */
  boolean             def_config_ptr_present;/*flag to indicate that def
                                               config ptr has been expanded
                                               to SIB16 type.             */
} rrcrce_default_config_info_type;

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
extern boolean rrc_ue_pos_cap_pch ;
#endif

#ifdef FEATURE_HANDLE_CN_PAGE_IN_CONNECTED_STATE
  extern boolean rrcpg1_match_found_conn_mode_cn_id;
#endif

rrc_freq_type rrcrce_cell_resel_freq = 0;

rrc_scr_code_type rrcrce_cell_resel_scr = 0;
#ifdef FEATURE_DUAL_SIM
extern wl1_common_fail_enum_type rrcmcm_wl1_failure_cause;
#endif

/*===========================================================================

      RRC CONNECTION ESTABLISHMENT PROCEDURE SUBSTATE Variable        

===========================================================================*/
/* RCE Substate variable initialized to RCE_INITIAL during the RRC TASK 
   startup. */              
LOCAL rrcrce_substate_e_type  rrcrce_substate;  


LOCAL uint32 rrcrce_number_of_rbs;      /* Indicates number of RBs that are 
                                           received thru RRC CONNECTION
                                           SETUP message */
LOCAL rrc_RRC_TransactionIdentifier rrcrce_setup_transaction_identifier;
                                        /* This stores the transaction Id for
                                           RRC Connection Setup message */
LOCAL rrc_RRC_TransactionIdentifier rrcrce_reject_transaction_identifier;
                                        /* This stores the transaction Id for
                                           RRC Connection Reject message */
LOCAL rrc_RRC_StateIndicator rrcrce_state_indicator;
                                        /* This stores the State RRC State
                                           Indicator received thru RRC 
                                           Connection Setup Message */
LOCAL rrc_WaitTime rrcrce_wait_time;    /* This stores the Wait Time received
                                           through RRC Connection Reject
                                           message */

LOCAL rrc_abort_cause_e_type rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE; 
boolean rrcrce_send_abort_cnf = FALSE;
LOCAL boolean rrcrce_abort_ind_sent = FALSE;
LOCAL uint8 rrcrce_num_retries_count = 0;
LOCAL boolean rrcrce_sig_con_delay = FALSE;

LOCAL boolean rrcrce_sig_conn_rel_req_for_both_domains = FALSE;

/* RCE procedure will fill the channel config request with the current value of the 
transaction id. Transaction ids will help RCE associate a channel config confirm
with a channel config request that it had sent */
LOCAL uint8  rce_chl_config_transaction_id = 0;

/* Stores the RRC Connection Setup message */
LOCAL dl_ccch_msg_type save_dl_ccch_msg;

/* Stores the information that redirection is via Connection
 * reject or Connection Setup */
LOCAL rrcrce_conn_rsp_e_type rsp_to_rrc_conn_req;

LOCAL rrcrce_default_config_info_type rrcrce_default_config_info;

/*CN Domain Identity*/
rrc_cn_domain_identity_e_type     rrc_est_cn_domain_id;
/*Call Type*/
rrc_call_type_in_est_req_e_type  rrc_est_call_type;

/* Routing Area ID */
rrc_rai_type         rrc_est_req_for_rai;
LOCAL boolean        rrcrce_lai_rai_mismatch = FALSE;

LOCAL mac_status_ind_e_type rrcrce_cmac_status = RACH_TX_SUCCESS;

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */
#define GET_TRANS_ID() (++rce_chl_config_transaction_id % 255)



#define RRCRCE_R6_CRITICAL_EXT_PRESENT \
  ((msg_ptr->message.u.rrcConnectionSetup.u.  \
    later_than_r3.criticalExtensions.t ==  \
    T_rrc_RRCConnectionSetup_criticalExtensions_5_criticalExtensions) && \
    (msg_ptr->message.u.rrcConnectionSetup.u. \
     later_than_r3.criticalExtensions.u.criticalExtensions.t == \
     T_rrc_RRCConnectionSetup_criticalExtensions_4_criticalExtensions) && \
    (msg_ptr->message.u.rrcConnectionSetup.u. \
     later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t == \
     T_rrc_RRCConnectionSetup_criticalExtensions_3_r6)) 

#define RRCRCE_R7_CRITICAL_EXT_PRESENT \
  ((msg_ptr->message.u.rrcConnectionSetup.u.  \
    later_than_r3.criticalExtensions.t ==  \
    T_rrc_RRCConnectionSetup_criticalExtensions_5_criticalExtensions) && \
    (msg_ptr->message.u.rrcConnectionSetup.u. \
     later_than_r3.criticalExtensions.u.criticalExtensions.t == \
     T_rrc_RRCConnectionSetup_criticalExtensions_4_criticalExtensions) && \
    (msg_ptr->message.u.rrcConnectionSetup.u. \
     later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t == \
     T_rrc_RRCConnectionSetup_criticalExtensions_3_criticalExtensions) &&\
     (msg_ptr->message.u.rrcConnectionSetup.u. \
     later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.t == \
     T_rrc_RRCConnectionSetup_criticalExtensions_2_r7)) 





/*===========================================================================
** -----------------------------------------------------------------------------------
**-- ----------NON-Demand paging section Srart--------------------------------------------
** -----------------------------------------------------------------------------------
  This section will have code section that will not be demand paged. Function which should be in this section are
  -RRC functions that are called by L1/L2 which are higher priority then RRC
  -RRC functions that are called in ISR context or RTOS timer call back
  -RRC functions that indirectly gets called by L1/L2 /ISR/timer call back   
  For example: Fun1() gets called by L1. Fun2() is called by Fun1(). Now both  Fun1() & Fun2() should be NON demand paged, 
  so both should be added in this section  
  Usage of pragma 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void foo(void)
  {
    function body here
  }
        __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
  If you have another function to be added in the same section then again it needs to be wrapped with these pragma 
  for example:- function bar() is just below function foo() then function bar() needs to be written like this …. 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void bar(void)
  {
    function body here
  }
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
** ----------------------------------------------------------------------- */




/*===========================================================================
FUNCTION RRCRCE_IS_RCE_PROCEDURE_WAITING_FOR_BPLMN

DESCRIPTION
  This function looks into RRC Connection Establishment procedure's 
  sub-states and returns the status of RRC Connection Establishment
  Procedure.

DEPENDENCIES
  None

RETURN VALUE
  boolean: 
           TRUE  - if RCE is waiting for BPLMN search to be suspended/aborted.
           FALSE - otherwise
           
SIDE EFFECTS
  To be used only by CSP for BPLMN search as the logic is designed for CSP's requirement.
===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
boolean rrcrce_is_rce_procedure_waiting_for_bplmn(void)
{
  rrc_state_e_type curr_rrc_state = rrc_get_state();

  if ( ((curr_rrc_state == RRC_STATE_DISCONNECTED)
     && (rrcrce_substate != RRCRCE_INITIAL)) ||
     ((curr_rrc_state != RRC_STATE_DISCONNECTED)
     && (rrcrce_substate != RRCRCE_FINAL)))
  {
    WRRC_MSG2_HIGH("WTOW RCE is active sst %d curr_st %d", rrcrce_substate, curr_rrc_state);
    /* RCE is active */
    return(TRUE);
  }
    return(FALSE);
} /* rrcrce_is_rce_procedure_waiting_for_bplmn */
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/


/*===========================================================================

FUNCTION rrcrce_delete_default_config_info

DESCRIPTION
  This function deletes the default configuration database used to maintain the
  status of RRC connection setup processing.

DEPENDENCIES
  None

RETURN VALUE
  uint8

SIDE EFFECTS
  None
===========================================================================*/
static void rrcrce_delete_default_config_info(void)
{
  if(rrcrce_default_config_info.def_config_ptr_present == TRUE)
  {
    if((rrcrce_default_config_info.default_config_id == RRC_DEF_CONFIG_17) 
      || (rrcrce_default_config_info.default_config_id == RRC_DEF_CONFIG_23)
    )
    {
      /*free the msg_ptr previously allocated.. if failure, flag an error*/
      (void) rrcasn1_free_pdu((int)rrc_DL_DCCH_Message_PDU, 
       rrcrce_default_config_info.dcch_msg_ptr);
        /*clean the msg_ptr data structure*/
       rrcrce_default_config_info.def_config_ptr_present = FALSE;
    } 
    else
    if (rrcrce_default_config_info.rbs_ptr != NULL)
    {
      (void)rrcasn1_free_pdu((int)rrc_DL_DCCH_Message_PDU, 
      rrcrce_default_config_info.rbs_ptr);
      /*clean the msg_ptr data structure*/
      rrcrce_default_config_info.def_config_ptr_present = FALSE;
    }
    else
    {
      /*free the msg_ptr previously allocated.. if failure, flag an error*/
      (void)rrcasn1_free_pdu((int)rrc_SysInfoType16_PDU, 
      rrcrce_default_config_info.sib16_ptr);
      /*clean the msg_ptr data structure*/
      rrcrce_default_config_info.def_config_ptr_present = FALSE;
    } 
  }
  rrcrce_default_config_info.rbs_ptr = NULL;
  rrcrce_default_config_info.dcch_msg_ptr = NULL;
  rrcrce_default_config_info.sib16_ptr = NULL;
  rrcrce_default_config_info.config_type =  RRC_INVALID_CONFIG;
} /* rrcrce_delete_default_config_info */

/*===========================================================================

FUNCTION RRCRCE_SEND_CONN_SETUP_IND

DESCRIPTION
  This function builds RRC_CONN_SETUP_IND command to post to CHO proc.The calling
  function should pass in the paramters to be sent in the command. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_conn_setup_ind(rrc_conn_setup_ind_type conn_setup_ind_params);

/*===========================================================================

FUNCTION rrcrce_get_num_retries_count

DESCRIPTION
  This function returns the count of number of times RRC connection request
  retries have been maxed out.

DEPENDENCIES
  None

RETURN VALUE
  uint8

SIDE EFFECTS
  None
===========================================================================*/
uint8 rrcrce_get_num_retries_count(void)
{
  return rrcrce_num_retries_count;
}


/*===========================================================================

FUNCTION rrcrce_init_procedure

DESCRIPTION
  This function will initialize the RRC Connection Establishment procedure
  substate to RRCRCE_INITIAL Substate. This will called in the RRCTASK modules 
  along with other Init Procedures.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrcrce_init_procedure
(    
  void  
)
{
  /* Resets the RCE state machine */
  rrcrce_substate = RRCRCE_INITIAL; 
  /* The following code is added since de-registration mechanism is not place.
     It should be deleted once de-registration mechanism in place in State
     change manager */
  rrcscmgr_register_for_scn(RRC_PROCEDURE_RCE,
                            RRC_STATE_WILDCARD,
                            RRC_STATE_DISCONNECTED);

  /*Init the values*/
  rrc_est_cn_domain_id = RRC_CS_DOMAIN_CN_ID;
  rrc_est_call_type = RRC_CALL_TYPE_NONE;

} /* rrcrce_init_procedure */
#ifdef FEATURE_UE_INITIATED_DORMANCY
/*===========================================================================

FUNCTION rrcrce_send_rrc_abort_cnf_ue_init_dormancy

DESCRIPTION
  This function builds RRC_ABORT_CNF command to post to MM in response to 
  RRC_ABORT_REQ. This is only for feature UE initiated dormancy feature
  This .RRC_ABORT_REQ is kind of dummay cnf for NAS. if its sent then 
  RRC-NAS both will have RRC connection intact.This is used by RRC when 
  it receives abort_req for feature ue_init_dormancy & RRC is in OOS.
  The requirement here is that when application initiates close it can't wait 
  for longer time to receive response from modem. So If RRC is in OOS 
  return Immediately.
DEPENDENCIES
  None

RETURN VALUE
  Void
  
SIDE EFFECTS
  None
===========================================================================*/
static void rrcrce_send_rrc_abort_cnf_ue_init_dormancy
(
  rrc_rel_cause_e_type abort_cnf_cause
)
{
  mm_cmd_type *cmd_ptr;                      /* Pointer to the MM Command */

  /* Allocate memory for RRC_ABORT_CNF command */

  cmd_ptr =  mm_rrc_get_cmd_buf(RRC_ABORT_CNF);
  /* Have Zero value for HFNs as its dummy abort cnf to NAS*/ 
  memset(cmd_ptr->cmd.rrc_abort_cnf.hfn_cs, 0x00, RRC_MAX_HFN_LENGTH);
  memset(cmd_ptr->cmd.rrc_abort_cnf.hfn_ps, 0x00, RRC_MAX_HFN_LENGTH);

  cmd_ptr->cmd.rrc_abort_cnf.conn_rel_cause = abort_cnf_cause;

#ifdef FEATURE_DUAL_SIM
  cmd_ptr->cmd.rrc_abort_cnf.as_id = rrc_get_as_id();
#endif
  WRRC_MSG2_HIGH("Dispatching out MM cmd RRC_ABORT_CNF/ Send Abort Cnf in %d substate for dormancy rel_cause %d",rrcrce_substate,cmd_ptr->cmd.rrc_abort_cnf.conn_rel_cause);

  rrc_send_rrc_message_to_mm(cmd_ptr);               /* Sends the command to MM */
}/*rrcrce_send_rrc_abort_cnf */
#endif
/*===========================================================================

FUNCTION rrcrce_procedure_event_handler

DESCRIPTION
  This is the entry point for RRC Connection Establishment procedure. All
  events for RCE come to this function from RRC Dispatcher. Depending on the
  UE state of RRC Connection Procedure, it calls the corresponding state 
  handler. These state handlers call substate handlers depending on the 
  the current substate of RRC Connection Establishment procedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  
  rrc_state_e_type rrc_state = rrc_get_state();
  WRRC_MSG2_HIGH("function rrcrce_procedure_event_handler is called in rrc_state = rrc_state_e_type_value%d, and rrcrce_substate = rrcrce_substate_e_type_value%d", rrc_state, rrcrce_substate);
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_RCE,rrcrce_substate,cmd_ptr->cmd_hdr.cmd_id);

#ifdef FEATURE_UE_INITIATED_DORMANCY
  /* If Abort is for UE initiated dormancy feature, check to see if UE is in OOS  */
  if((cmd_ptr->cmd_hdr.cmd_id == RRC_ABORT_REQ)&&
     (cmd_ptr->cmd.rrc_abort_req.abort_cause == RRC_UE_INITIATED_DORMANCY)&&
     (rrc_state == RRC_STATE_DISCONNECTED)
    )
  {
    /*UE does not have RRC connection so send abort cnf right here*/
    rrcrce_send_rrc_abort_cnf_ue_init_dormancy(RRC_UE_INIT_DORMANCY_ABORT_SUCCESS);
    return;
  }
  else if((cmd_ptr->cmd_hdr.cmd_id == RRC_ABORT_REQ)&&
     (cmd_ptr->cmd.rrc_abort_req.abort_cause == RRC_UE_INITIATED_DORMANCY)&&
     (RRC_NOT_CAMPED_ON == rrc_ccm_get_curr_camping_status(RRC_PROCEDURE_RCE))
    )
  {
    /* If Abort is for UE initiated dormancy feature, check to see if UE is in OOS  */
    /*UE is in OOS so send abort cnf right here*/
    rrcrce_send_rrc_abort_cnf_ue_init_dormancy(RRC_UE_INIT_DORMANCY_UE_IN_OOS);
    return;
  }
#endif

/* Send release cause as PS_ABORT_SUCCESS only when connection has already been released*/
#ifdef FEATURE_DUAL_SIM
  if((cmd_ptr->cmd_hdr.cmd_id == RRC_ABORT_REQ) &&
      (cmd_ptr->cmd.rrc_abort_req.abort_cause == RRC_DUAL_STANDBY_PS_ABORT))
  {
    if((rrc_state == RRC_STATE_DISCONNECTED) && (rrcrce_substate == RRCRCE_INITIAL))
    {
      rrc_release_cause = RRC_REL_DUAL_STANDBY_PS_ABORT_SUCCESS;
    }
    else
    {
      rrc_release_cause = RRC_REL_CAUSE_NORMAL;
    }
  }
#endif

#ifdef FEATURE_WCDMA_TABASCO_MODEM 
  if((cmd_ptr->cmd_hdr.cmd_id == RRC_ABORT_REQ) && (rrccho_is_wtog_cco_active() == TRUE))
  {
    rrcmcm_send_irat_abort_ind();
  }
#endif

  switch (rrc_state)
  {
    case RRC_STATE_DISCONNECTED: 
      rrcrce_process_disconnected_state(cmd_ptr);
      break;
    case RRC_STATE_CONNECTING:   
      rrcrce_process_connecting_state(cmd_ptr); 
      break;
    case RRC_STATE_CELL_FACH:    
      rrcrce_process_cell_fach_state(cmd_ptr); 
      break;
    case RRC_STATE_CELL_DCH:     
      rrcrce_process_cell_dch_state(cmd_ptr); 
      break;
    case RRC_STATE_CELL_PCH:
    case RRC_STATE_URA_PCH:
      rrcrce_process_cell_ura_pch_state(cmd_ptr);
      break;
    default:  
      ERR_FATAL("Event forwarded in invalid State: %d",rrc_state,0,0);
      break;
   }  

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
  if((cmd_ptr->cmd_hdr.cmd_id == RRC_STATE_CHANGE_IND) && (cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED)
      && (csfb_extended_status == CSFB_CS_RB_RELEASED))
  {
    WRRC_MSG0_HIGH("CSFB_HACK: Resetting csfb_extended_status on state change to disconnected");
    csfb_extended_status = CSFB_INVALID;
    rrc_crt_restarted_once = FALSE;
    rrc_cs_rab_established = FALSE;
  }
#endif

} /* rrcrce_procedure_event_handler */


/*===========================================================================

FUNCTION rrcrce_get_nv_items

DESCRIPTION
  This  function gets NV item NV_GPRS_ANITE_GCF_I and stores it in local variable
  used to determine if 40s timer needs to be started before sending signalling
  connection release.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcrce_get_nv_items
(
  sys_modem_as_id_e_type wrrc_as_id
)
{
  nv_gcf_test_flag =0;
  /*read integrity and ciphering ENABLE/DISABLE flags from NV*/
  wnv_get_nv_msim(WNV_ID_GPRS_ANITE_GCF_I, &nv_gcf_test_flag, sizeof(nv_gcf_test_flag),wrrc_as_id);
  WRRC_MSG1_HIGH("NV_GPRS_ANITE_GCF_I is %d",nv_gcf_test_flag);

}

/*===========================================================================

FUNCTION rrcrce_is_abort_in_progress

DESCRIPTION
  This function returns if abort is in progress

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if abort in progress
  FALSE - otherwise

===========================================================================*/
boolean rrcrce_is_abort_in_progress()
{
  return rrcrce_send_abort_cnf;
}

/*===========================================================================

FUNCTION rrcrce_process_disconnected_state

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in Disconnected state of Idle mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_disconnected_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  boolean pcco_in_progress = rrccho_is_gtow_pcco_in_progress();
  
  if (cmd_ptr->cmd_hdr.cmd_id == RRC_CPHY_OUT_OF_SERVICE_AREA_IND)
  {
    event_report(EVENT_WCDMA_OUT_OF_SERVICE);

    WRRC_MSG2_HIGH("OOS in RCE disconnected state.rrcrce_substate %d  PCCO %d",rrcrce_substate,pcco_in_progress);

    /* If GTOW is in progress,send a conn_setup_ind with failure to CHO.
       CHO would deactivate WCDMA and pass control back to GSM.Do not send
       RRC_ABORT_IND to MM if GTOW is active */
    if(pcco_in_progress)
    { 
      rrc_conn_setup_ind_type conn_setup_ind_params;

      conn_setup_ind_params.status = FAILURE;
      conn_setup_ind_params.conn_failure_reason = RRC_CONN_FAILURE_OTHER;
      rrcrce_send_conn_setup_ind(conn_setup_ind_params);

      /* reset the RCE substate */
      rrcrce_substate = RRCRCE_INITIAL;    
      return;
    }

    if(rrcrce_send_abort_cnf == TRUE)
    {
      rrcrce_send_rrc_abort_cnf(); 
    }
    else
    {
      if(rrcrce_substate != RRCRCE_INITIAL)
      {
        rrcrce_send_rrc_abort_ind(RRC_FAIL_TO_CAMP_ON);
        rrcrce_abort_ind_sent = TRUE;
      }
    }
    if(rrcrce_substate != RRCRCE_WAIT_FOR_RB0_SETUP_CNF)
    {
      rrcrce_substate = RRCRCE_INITIAL;
      (void) rrcrce_send_rrc_initiate_cell_selection_ind(RRC_OUT_OF_SERVICE_AREA_IND,
                                                rrc_get_state() );
    }
    /* Initiate Cell Selection and remain in RRCRCE_INITIAL substate */
    
    return;
  }

  switch (rrcrce_substate)
  {
    case RRCRCE_INITIAL: 
      rrcrce_process_rce_initial_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_UE_CAMP_ON:
      rrcrce_process_rce_wait_for_ue_camp_on_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_SIB7_CURRENT:
      rrcrce_process_rce_wait_for_sib7_current(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_RB0_SETUP_CNF: 
      rrcrce_process_rce_wait_for_rb0_setup_cnf_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_RRC_CONN_SETUP: 
      rrcrce_process_rce_wait_for_conn_setup_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_SRB_SETUP_CNF:
      rrcrce_process_rce_wait_for_srb_setup_cnf_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_REDIRECT_CNF:
      rrcrce_process_rce_wait_for_redirect_cnf_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_WAIT_TIMER_TO_EXPIRE:
      rrcrce_process_rce_wait_for_wait_time_expire_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_RRC_TIMER_TO_EXPIRE:
      rrcrce_process_rce_wait_for_rrc_timer_expire_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_CU_COMPLETE_CNF:
      rrcrce_process_rce_wait_for_cu_complete_cnf_substate(cmd_ptr);
      break;
    case RRCRCR_WAIT_FOR_SRB_REL_CNF:
      rrcrce_process_rce_wait_for_srb_rel_cnf_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_SIG_CON_REL_COMPL:
      rrcrce_process_rce_wait_for_sig_con_rel_compl_substate(cmd_ptr);
      break;
    case RRCRCE_FINAL: 
      rrcrce_process_rce_final_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_RB0_REL_CNF:
      rrcrce_process_rce_wait_for_rb0_rel_cnf_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_RESELCT_COMPL:
      rrcrce_process_rce_wait_for_reselct_compl_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_RB0_PHYCHAN_RESETUP_CNF:
      rrcrce_process_rce_wait_for_rb0_phychan_resetup_cnf_substate(cmd_ptr);
      break;
    default:           
      WRRC_MSG1_ERROR("Invalid substate in DISCONNECTED State: %d",
                rrcrce_substate);
      break;
  }  
}/* rrcrce_process_disconnected_state */

/*===========================================================================

FUNCTION rrcrce_process_connecting_state

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in Connecting state of Idle mode. The connecting 
  state is a transient state created to honor Paging Type 1 message during the
  RRC Connection Establishment procedure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_connecting_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  boolean pcco_in_progress = rrccho_is_gtow_pcco_in_progress();

  if (cmd_ptr->cmd_hdr.cmd_id == RRC_CPHY_OUT_OF_SERVICE_AREA_IND)
  {
    event_report(EVENT_WCDMA_OUT_OF_SERVICE);

    WRRC_MSG2_HIGH("OOS in Connecting state.rrcrce_substate %d PCCO %d ",rrcrce_substate,pcco_in_progress);

    /* If GTOW is in progress,send a conn_setup_ind with failure to CHO
       CHO would deactivate WCDMA and pass control back to GSM */
    if(pcco_in_progress)
    { 
      rrc_conn_setup_ind_type conn_setup_ind_params;

      conn_setup_ind_params.status = FAILURE;
      conn_setup_ind_params.conn_failure_reason = RRC_CONN_FAILURE_OTHER;
      rrcrce_send_conn_setup_ind(conn_setup_ind_params);                  

      /* Reset RCE susbstate */
      rrcrce_substate = RRCRCE_INITIAL;
      return;
    }
    
    rrcrce_send_rrc_channel_config_req_for_rb0(RELEASE_RB, 
                                               RRC_STATE_DISCONNECTED,
                                               FALSE);
    rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;
    rrcrce_mm_abort_cause = RRC_FAIL_TO_CAMP_ON;
    return;
  }

  switch (rrcrce_substate)
  {
    case RRCRCE_WAIT_FOR_UE_CAMP_ON:
      rrcrce_process_rce_wait_for_ue_camp_on_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_RRC_CONN_SETUP: 
      rrcrce_process_rce_wait_for_conn_setup_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_SRB_SETUP_CNF:
      rrcrce_process_rce_wait_for_srb_setup_cnf_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_REDIRECT_CNF:
      rrcrce_process_rce_wait_for_redirect_cnf_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_WAIT_TIMER_TO_EXPIRE:
      rrcrce_process_rce_wait_for_wait_time_expire_substate(cmd_ptr);
      break;
    case RRCRCR_WAIT_FOR_SRB_REL_CNF:
      rrcrce_process_rce_wait_for_srb_rel_cnf_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_RESELCT_COMPL:
      rrcrce_process_rce_wait_for_reselct_compl_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_RB0_REL_CNF:
      rrcrce_process_rce_wait_for_rb0_rel_cnf_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_RB0_PHYCHAN_RESETUP_CNF:
      rrcrce_process_rce_wait_for_rb0_phychan_resetup_cnf_substate(cmd_ptr);
      break;
    default:     
      WRRC_MSG1_HIGH("Invalid substate in CONNECTING State: %d",rrcrce_substate);
      break;
  }  
} /* rrcrce_process_connecting_state*/
/*===========================================================================

FUNCTION rrcrce_process_cell_fach_state

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in CELL_FACH state of Connected  mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_cell_fach_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  /* If network authentication check has failed then bar the serving cell */
  if(    (cmd_ptr->cmd_hdr.cmd_id == RRC_ABORT_REQ)
      && (cmd_ptr->cmd.rrc_abort_req.auth_failed_bar_cells == TRUE)
      && (rrccsp_get_curr_scan() != RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
      && (rrccsp_get_curr_scan() != RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN)
    )
  {
    WRRC_MSG1_HIGH("N/W Auth Failed. Rcvd Abort Req from MM in RRC State: %d. Cell bar req sent to L1.", 
                                                                                  rrc_get_state());
    rrccsp_send_cphy_cell_bar_req(RRC_CSP_WCDMA_PSC_BARRED, 1280);
  }
  switch (rrcrce_substate)
  {
    case RRCRCE_WAIT_FOR_CU_COMPLETE_CNF:
      rrcrce_process_rce_wait_for_cu_complete_cnf_substate(cmd_ptr);
      break;
    case RRCRCR_WAIT_FOR_SRB_REL_CNF:
      rrcrce_process_rce_wait_for_srb_rel_cnf_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_SIG_CON_REL_COMPL:
      rrcrce_process_rce_wait_for_sig_con_rel_compl_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_RRC_TIMER_TO_EXPIRE:
      rrcrce_process_rce_wait_for_rrc_timer_expire_substate(cmd_ptr);
      break;
    case RRCRCE_FINAL: 
      rrcrce_process_rce_final_substate(cmd_ptr);
      break;
   default:     
     WRRC_MSG1_HIGH("Invalid substate in CELL_FACH state: %d",rrcrce_substate);
     break;
  }  
} /* rrcrce_process_cell_fach_state */
/*===========================================================================

FUNCTION rrcrce_process_cell_dch_state

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in CELL_DCH state of Connected  mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_cell_dch_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  /* If network authentication check has failed then bar the active set */
  if(    (cmd_ptr->cmd_hdr.cmd_id == RRC_ABORT_REQ)
      && (cmd_ptr->cmd.rrc_abort_req.auth_failed_bar_cells == TRUE)
      && (rrccsp_get_curr_scan() != RRC_CSP_OUT_OF_SERVICE_AREA_SCAN)
      && (rrccsp_get_curr_scan() != RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN)
    )
  { 
    WRRC_MSG1_HIGH("N/W Auth Failed. Rcvd Abort Req from MM in RRC State: %d. ASet bar req sent to L1.", 
                                                                                  rrc_get_state());
    rrccsp_send_cphy_cell_bar_req(RRC_CSP_WCDMA_ASET_BARRED, 1280);
  }
  switch (rrcrce_substate)
  {
    
    case RRCRCR_WAIT_FOR_SRB_REL_CNF:
      rrcrce_process_rce_wait_for_srb_rel_cnf_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_SIG_CON_REL_COMPL:
      rrcrce_process_rce_wait_for_sig_con_rel_compl_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_RRC_TIMER_TO_EXPIRE:
      rrcrce_process_rce_wait_for_rrc_timer_expire_substate(cmd_ptr);
      break;
    case RRCRCE_FINAL: 
      rrcrce_process_rce_final_substate(cmd_ptr);
      break;
    default:     
      WRRC_MSG1_HIGH("Invalid substate in CELL_DCH State: %d",rrcrce_substate);
      break;
  }  
} /* rrcrce_process_cell_dch_state */

/*===========================================================================

FUNCTION rrcrce_process_cell_ura_pch_state

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in CELL_PCH state or URA_PCH state of Connected
  mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_cell_ura_pch_state
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  /* If network authentication check has failed then bar the serving cell */
  if((cmd_ptr->cmd_hdr.cmd_id == RRC_ABORT_REQ)
      && (cmd_ptr->cmd.rrc_abort_req.auth_failed_bar_cells == TRUE))
  {
    WRRC_MSG1_HIGH("N/W Auth Failed. Rcvd Abort Req from MM in RRC State: %d. Cell bar req sent to L1.", 
                                                                                  rrc_get_state());
    rrccsp_send_cphy_cell_bar_req(RRC_CSP_WCDMA_PSC_BARRED, 1280);
  }
  switch (rrcrce_substate)
  {
    case RRCRCE_FINAL: 
      rrcrce_process_rce_final_substate(cmd_ptr);
      break;
    case RRCRCR_WAIT_FOR_SRB_REL_CNF:
      rrcrce_process_rce_wait_for_srb_rel_cnf_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_SIG_CON_REL_COMPL:
      rrcrce_process_rce_wait_for_sig_con_rel_compl_substate(cmd_ptr);
      break;
    case RRCRCE_WAIT_FOR_RRC_TIMER_TO_EXPIRE:
      rrcrce_process_rce_wait_for_rrc_timer_expire_substate(cmd_ptr);
      break;
    default:     
      WRRC_MSG1_HIGH("Invalid substate in PCH state: %d", rrcrce_substate);
      break;
  }  
} /* rrcrce_process_cell_ura_pch_state */

/*==========================================================================

FUNCTION rrcrce_update_initial_ue_identity

DESCRIPTION
  This function updates contents of Initial UE Identity Choice and Initial
  UE Identity received from NAS RRC_EST_REQ primitive.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_update_initial_ue_identity
(
  rrc_InitialUE_Identity *rrc_initial_ue_identity_ext
                                             /* Initial UE Indentity to be 
                                                sent over the air */
)
{
  uint32 i;

                                                /* Temporary varibale to update
                                                   linked list members */       
  
  switch (rrc_initial_ue_identity_choice)
  {
    case RRC_IMSI_UE_ID:
      rrc_initial_ue_identity_ext->t = T_rrc_InitialUE_Identity_imsi;

      rrc_initial_ue_identity_ext->u.imsi = 
        rtxMemAllocTypeZ (&enc_ctxt, rrc_IMSI_GSM_MAP);

      rrc_initial_ue_identity_ext->u.imsi->n = rrc_initial_ue_identity.imsi.num_digits;

      ALLOC_ASN1ARRAY2(&enc_ctxt, rrc_initial_ue_identity_ext->u.imsi, rrc_Digit);
        
      for(i = 0; i < rrc_initial_ue_identity.imsi.num_digits; i++) 
      {  
        /* Update the digit value */
        rrc_initial_ue_identity_ext->u.imsi->elem[i] = 
          rrc_initial_ue_identity.imsi.value[i];
       
      }        
      break;

    case RRC_TMSI_LAI_UE_ID:
      rrc_initial_ue_identity_ext->t = T_rrc_InitialUE_Identity_tmsi_and_LAI;
      rrc_initial_ue_identity_ext->u.tmsi_and_LAI =
        rtxMemAllocTypeZ(&enc_ctxt, rrc_TMSI_and_LAI_GSM_MAP );
      
      /* Update the TMSI length in bits */
      rrc_initial_ue_identity_ext->u.tmsi_and_LAI->tmsi.numbits = 
          RRC_MAX_GSMMAP_TMSI_LENGTH * 8;

      for (i = 0; i < RRC_MAX_GSMMAP_TMSI_LENGTH; i++)
      {
        /* Update value for TMSI */
        *(rrc_initial_ue_identity_ext->u.tmsi_and_LAI->tmsi.data + i) =
          rrc_initial_ue_identity.tmsi_lai.tmsi.value[i];
      }

      rrc_initial_ue_identity_ext->u.tmsi_and_LAI->lai.plmn_Identity.mcc.n =
        RRCRCE_MCC_LENGTH;
     
      ALLOC_ASN1ARRAY2 (&enc_ctxt, 
        &rrc_initial_ue_identity_ext->u.tmsi_and_LAI->lai.plmn_Identity.mcc, rrc_Digit);
        
      for (i = 0; i < RRCRCE_MCC_LENGTH ; i++)
      {
        /* Set value for MCC */
        rrc_initial_ue_identity_ext->u.tmsi_and_LAI->lai.plmn_Identity.mcc.elem[i] =
          rrc_initial_ue_identity.tmsi_lai.lai.plmn.mcc[i];
      }

      rrc_initial_ue_identity_ext->u.tmsi_and_LAI->lai.plmn_Identity.mnc.n=
        rrc_initial_ue_identity.tmsi_lai.lai.plmn.num_mnc_digits;
      
      ALLOC_ASN1ARRAY2 (&enc_ctxt, 
        &rrc_initial_ue_identity_ext->u.tmsi_and_LAI->lai.plmn_Identity.mnc, rrc_Digit);

      for (i = 0; i < rrc_initial_ue_identity.tmsi_lai.lai.plmn.num_mnc_digits;
           i++)
      {
        /* Set Value for MNC */
        rrc_initial_ue_identity_ext->u.tmsi_and_LAI->lai.plmn_Identity.mnc.elem[i] =
          rrc_initial_ue_identity.tmsi_lai.lai.plmn.mnc[i];
       
        
      }      
       
      
      /* Set the LAC length in bits */
      rrc_initial_ue_identity_ext->u.tmsi_and_LAI->lai.lac.numbits = 
        RRC_MAX_GSMMAP_LAC_LENGTH * 8;


      
      for (i = 0; i < RRC_MAX_GSMMAP_LAC_LENGTH; i++)
      {
        /* Update LAC value */
        *(rrc_initial_ue_identity_ext->u.tmsi_and_LAI->lai.lac.data + i ) = 
          rrc_initial_ue_identity.tmsi_lai.lai.lac[i];
      } 
      
      break;

    case RRC_PTMSI_RAI_UE_ID:
      rrc_initial_ue_identity_ext->t = T_rrc_InitialUE_Identity_p_TMSI_and_RAI;

      rrc_initial_ue_identity_ext->u.p_TMSI_and_RAI =
        rtxMemAllocTypeZ(&enc_ctxt, rrc_P_TMSI_and_RAI_GSM_MAP );
      
      /* Set PTMSI length in bits */
      rrc_initial_ue_identity_ext->u.p_TMSI_and_RAI->p_TMSI.numbits = 
        RRC_MAX_GSMMAP_PTMSI_LENGTH * 8;

      

      for (i = 0; i < RRC_MAX_GSMMAP_PTMSI_LENGTH; i++)
      {
        /*Update PTMSI value */
        *(rrc_initial_ue_identity_ext->u.p_TMSI_and_RAI->p_TMSI.data + i) =
          rrc_initial_ue_identity.ptmsi_rai.ptmsi.value[i];
      }

      rrc_initial_ue_identity_ext->u.p_TMSI_and_RAI->rai.lai.plmn_Identity.mcc.n =
        RRCRCE_MCC_LENGTH;
     
      ALLOC_ASN1ARRAY2 (&enc_ctxt, 
        &rrc_initial_ue_identity_ext->u.p_TMSI_and_RAI->rai.lai.plmn_Identity.mcc, rrc_Digit);

      for (i = 0; i < RRCRCE_MCC_LENGTH ; i++)
      {
        /* Set value for MCC */
        rrc_initial_ue_identity_ext->u.p_TMSI_and_RAI->rai.lai.plmn_Identity.
          mcc.elem[i] = rrc_initial_ue_identity.ptmsi_rai.rai.lai.plmn.mcc[i];          

      }
      rrc_initial_ue_identity_ext->u.p_TMSI_and_RAI->rai.lai.plmn_Identity.mnc.n=
        rrc_initial_ue_identity.ptmsi_rai.rai.lai.plmn.num_mnc_digits;

      ALLOC_ASN1ARRAY2 (&enc_ctxt, 
        &rrc_initial_ue_identity_ext->u.p_TMSI_and_RAI->rai.lai.plmn_Identity.mnc, rrc_Digit);

      for (i = 0; i < rrc_initial_ue_identity.ptmsi_rai.rai.lai.plmn.num_mnc_digits;
           i++)
      {
        /* Set Value for MNC */
        rrc_initial_ue_identity_ext->u.p_TMSI_and_RAI->rai.lai.plmn_Identity.mnc.elem[i] =
          rrc_initial_ue_identity.ptmsi_rai.rai.lai.plmn.mnc[i];
       
        
      }    
      
      /* Set the LAC length in bits */
      rrc_initial_ue_identity_ext->u.p_TMSI_and_RAI->rai.lai.lac.numbits = 
        RRC_MAX_GSMMAP_LAC_LENGTH * 8;

  

      
      for (i = 0; i < RRC_MAX_GSMMAP_LAC_LENGTH; i++)
      {
        /* Update LAC value */
        *(rrc_initial_ue_identity_ext->u.p_TMSI_and_RAI->rai.lai.lac.data  + i ) = 
          rrc_initial_ue_identity.ptmsi_rai.rai.lai.lac[i];
      }  
      
      /* Set the RAC length in bits */
      rrc_initial_ue_identity_ext->u.p_TMSI_and_RAI->rai.rac.numbits = 
        RRC_MAX_GSMMAP_RAC_LENGTH * 8;



      for (i = 0; i < RRC_MAX_GSMMAP_RAC_LENGTH; i++)
      {
        /* Update the RAC value */
        *(rrc_initial_ue_identity_ext->u.p_TMSI_and_RAI->rai.rac.data + i ) = 
          rrc_initial_ue_identity.ptmsi_rai.rai.rac[i];
      } 
      break;

    case RRC_IMEI_UE_ID:
      rrc_initial_ue_identity_ext->t = T_rrc_InitialUE_Identity_imei;

      /* Allocate memory for IMEI */
      rrc_initial_ue_identity_ext->u.imei = 
          rtxMemAllocTypeZ(&enc_ctxt, rrc_IMEI);

      rrc_initial_ue_identity_ext->u.imei->n = RRC_NUM_IMEI_DIGITS;
      ALLOC_ASN1ARRAY2 (&enc_ctxt, 
        rrc_initial_ue_identity_ext->u.imei, rrc_IMEI_Digit);
    
      for(i = 0; i < RRC_NUM_IMEI_DIGITS; i++) 
      {            
        
        /* Update the digit value */
        rrc_initial_ue_identity_ext->u.imei->elem[i] = 
          rrc_initial_ue_identity.imei.value[i];

       
      }       
      break;

    default:
      //This should never happen. Initial UE Identity is already validated
      ERR_FATAL("Invalid Initial UE Id t is rcvd: %d",
                 rrc_initial_ue_identity_choice,0,0);
      break;
  }
} /* rrcrce_update_initial_ue_identity*/
/*==========================================================================

FUNCTION rrcrce_update_establishment_cause

DESCRIPTION
  This function gets equivalent external RRC Connection Establishment cause
  for the internal RRC Connection Established cause received from the NAS 
  through RRC_EST_REQ primitive.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
rrc_EstablishmentCause rrcrce_update_establishment_cause
(
  void
)
{
  rrc_EstablishmentCause rrc_establishment_cause_ext;
                                      /* RRC Connection Establishment cause
                                         to be sent over the air */
  rrc_translate_establishment_cause(rrc_establishment_cause, 
                         &rrc_establishment_cause_ext);    
  return(rrc_establishment_cause_ext);
  
} /* rrcrce_update_establishment_cause */
/*===========================================================================

FUNCTION rrcrce_send_rrc_initiate_cell_selection_ind

DESCRIPTION
  This function builds RRC_INITIATE_CELL_SELECTION_IND command to post to
  CSP procedure with the cause specified.
DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type: It returns RRCRCE_SUCCESS if it successfully sends 
  RRC_INITIATE_CELL_SELECTION_IND command to CSP. Otherwise it returns 
  RRCRCE_FAILURE_MAJOR.

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_rrc_initiate_cell_selection_ind
(
  rrc_cell_sel_cause_e_type cause,           /* Cause to trigger Cell
                                                selection */
  rrc_state_e_type dest_state                /* Destination State */
)
{
  rrc_cmd_type *cmd_ptr;                    /* Pointer to the RRC Command */

  /* Allocate memory for RRC_INITIATE_CELL_SELECTION_IND command */
  cmd_ptr =  rrc_get_int_cmd_buf();
  cmd_ptr->cmd_hdr.cmd_id = RRC_INITIATE_CELL_SELECTION_IND;

  /* Fill in parameters for RRC_INITIATE_CELL_SELECTION_IND command */
  cmd_ptr->cmd.initiate_cell_sel_ind.cause = cause;
  cmd_ptr->cmd.initiate_cell_sel_ind.proc  = RRC_PROCEDURE_RCE;
#ifdef FEATURE_DISCONNECT_ON_UNSUPPORTED_IRAT_REDIRECTION
  if (cause == RRC_TRANSITION_TO_DISCONNECTED &&
      dest_state== RRC_STATE_MAX)
  {
    cmd_ptr->cmd.initiate_cell_sel_ind.dest_state = RRC_STATE_DISCONNECTED;
    cmd_ptr->cmd.initiate_cell_sel_ind.tx_to_discon_reason = RRC_TX_TO_DISCON_UNSUPPORTED_REDIRECTION;
  }
  else
#endif
  {
    cmd_ptr->cmd.initiate_cell_sel_ind.dest_state = dest_state;
    cmd_ptr->cmd.initiate_cell_sel_ind.tx_to_discon_reason = RRC_TX_TO_DISCON_OTHER;
  }

  WRRC_MSG1_HIGH("Sent Init Cell ind with cause: %d",
             cause);

  rrc_put_int_cmd(cmd_ptr);          /* Sends the command to CSP */
}/*rrcrce_send_rrc_initiate_cell_selection_ind */

/*===========================================================================

FUNCTION rrcrce_send_rrc_channel_config_req_for_rb0

DESCRIPTION
  This function builds RRC_CHANNEL_CONFIG_REQ command and  requests RRC LLC
  to either setup or modify or release  Radio Bearer 0.
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_rrc_channel_config_req_for_rb0
(
               
  rrc_rb_cfg_e_type action,                   /* Type of action RB 0 */
  rrc_state_e_type next_state,                /* Next state to transition is
                                                 expected to take place */
  boolean confirm_required                    /* This indicates whether
                                                 RRC_CHANNEL_CONFIG_CNF is 
                                                 required or not */
)
{
  rrc_cmd_type *cmd_ptr;                     /* Pointer to the RRC Command */

  /* Allocates the buffer to RRC Internal command */
  cmd_ptr =  rrc_get_int_cmd_buf();
  /* Fill in the command parameters for RRC_CHANNEL_CONFIG_REQ */
  cmd_ptr->cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;
  cmd_ptr->cmd.chan_config_req.procedure = RRC_PROCEDURE_RCE;
  cmd_ptr->cmd.chan_config_req.transaction_id = GET_TRANS_ID();
  rce_chl_config_transaction_id = (uint8) cmd_ptr->cmd.chan_config_req.transaction_id;
WRRC_MSG1_HIGH("RB0 Chan config trans_id = %d",rce_chl_config_transaction_id);

  /* Fill in parameters for RRC_CHANNEL_CONFIG_REQ command */
  cmd_ptr->cmd.chan_config_req.rrc_state_change_required = TRUE;
  cmd_ptr->cmd.chan_config_req.next_state = next_state;
  cmd_ptr->cmd.chan_config_req.num_rb_to_config = 1;
  cmd_ptr->cmd.chan_config_req.rb[0].rb_id = CCCH_RADIO_BEARER_ID;
  cmd_ptr->cmd.chan_config_req.rb[0].rb_config = action;
  cmd_ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = 
    confirm_required;
  
  rrc_put_int_cmd(cmd_ptr);    /* sends the command to RRC LLC. */
}/*rrcrce_send_rrc_channel_config_req_for_rb0 */

/*===========================================================================

FUNCTION rrcrce_send_rrc_est_cnf

DESCRIPTION
  This function builds RRC_EST_CNF command to post to MM with  
  success status of RRC Connection Establishment procedure.
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_rrc_est_cnf
(
  void
)
{
  mm_cmd_type *cmd_ptr;                      /* Pointer to the MM Command */

  /* Allocate memory for RRC_EST_CNF command */
  cmd_ptr = mm_rrc_get_cmd_buf(RRC_EST_CNF);
  
  /* No parameters to fill in for RRC_EST_CNF command */
#ifdef FEATURE_DUAL_SIM
  cmd_ptr->cmd.rrc_est_cnf.as_id = rrc_get_as_id();
#endif
  WRRC_MSG0_HIGH("Dispatching out MM cmd / Sent RRC_EST_CNF to MM");
 
  rrc_send_rrc_message_to_mm(cmd_ptr);               /* Sends the command to MM */

#ifdef FEATURE_WCDMA_TO_LTE
  /* Reset flag which indicates pre-redirection info has to be included or not in connection request msg. */
  rrccsp_reset_flag_for_excluding_pre_redir_info();
#endif

  rrcrce_cell_resel_freq = 0;
  rrcrce_cell_resel_scr = 0;
}/*rrcrce_send_rrc_est_cnf */
/*===========================================================================

FUNCTION rrcrce_send_rrc_abort_ind

DESCRIPTION
  This function builds RRC_ABORT_IND command to post to MM with failure 
  status of RRC Connection Establishment procedure.
DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type: It returns RRCRCE_SUCCESS if it successfully sends 
  RRC_ABORT_IND command to the MM. Otherwise it returns RRCRCE_FAILURE_MAJOR.

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_rrc_abort_ind
(
  rrc_abort_cause_e_type cause               /* Cause for RRC Connection 
                                                Failure */
)
{
  mm_cmd_type *cmd_ptr;                      /* Pointer to the MM Command */

  /* send abort cause as LAI/RAI changed to NAS, so that NAS can attempt silent redial 
     for a pending MO call after LU is complete. */
  if(rrcrce_lai_rai_mismatch)
  {
    WRRC_MSG2_HIGH("Changed abort cause from %d to %d", cause,RRC_ABORT_LAI_RAI_CHANGED);
    cause = RRC_ABORT_LAI_RAI_CHANGED;
  }

  /* Allocate memory for RRC_ABORT_IND command */
  cmd_ptr =  mm_rrc_get_cmd_buf(RRC_ABORT_IND);

  /* Fill in parameters for RRC_ABORT_IND command */
  cmd_ptr->cmd.rrc_abort_ind.abort_cause = cause;

#ifdef FEATURE_DUAL_SIM
  cmd_ptr->cmd.rrc_abort_ind.as_id = rrc_get_as_id();

  /* NAS should look into this value only when abort cause is RRC_ABORT_CONN_REJECT_WAIT_TMR */
  cmd_ptr->cmd.rrc_abort_ind.conn_rej_wait_timer = rrcrce_wait_time;
#endif

  /* Update the Max HFNs values to be stored in SIM or USIM */ 
  (void) rrcsmc_append_hfns_to_rel_ind(cmd_ptr->cmd.rrc_abort_ind.hfn_cs,
                                       cmd_ptr->cmd.rrc_abort_ind.hfn_ps);

  WRRC_MSG2_HIGH("Dispatching out MM cmd / Sent RRC_ABORT_IND to MM with cause: %d substate %d", cause,rrcrce_substate);
  rrc_send_rrc_message_to_mm(cmd_ptr);               /* Sends the command to MM */

  /* NAS requested to indicate the domain for which abort is sent with cause access class not allowed 
   * Release event still uses the old cause
   */
  if((cause == RRC_AC_NOT_ALLOWED_CS_DOMAIN) || (cause == RRC_AC_NOT_ALLOWED_PS_DOMAIN))
  {
    rrcrce_send_conn_rel_cause_event(RRC_ACCESS_CLASS_NOT_ALLOWED);
  }
  else
  {
    rrcrce_send_conn_rel_cause_event(cause);
  }

#ifdef FEATURE_DUAL_SIM
  rrc_release_lock();
#endif

#ifdef FEATURE_3GPP_CSFB
  /*Connection is aborted, reset CSFB call status flag*/
  rrc_csfb_call_status = FALSE;
#endif

#ifdef FEATURE_WCDMA_TO_LTE
  /* Check for V300 counter is to make sure connection request is attempted atleast once */
  if(rrc_v_300)
  {
    /* Reset flag which indicates pre-redirection info has to be included or not in connection request msg. */
    rrccsp_reset_flag_for_excluding_pre_redir_info();
  }
#endif

  /* Initialize release cause */
  rrc_release_cause = RRC_REL_CAUSE_UNSPEC;

  rrcrce_cell_resel_freq = 0;
  rrcrce_cell_resel_scr = 0;
#ifdef FEATURE_QSH_EVENT_METRIC
  rrcrce_qsh_log_connection_est_failure();
#endif
  return;
}/*rrcrce_send_rrc_abort_ind */
/*===========================================================================

FUNCTION rrcrce_send_rrc_abort_cnf

DESCRIPTION
  This function builds RRC_ABORT_CNF command to post to MM in response to 
  RRC_ABORT_REQ.
DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type: It returns RRCRCE_SUCCESS if it successfully sends 
  RRC_ABORT_CNF command to the MM. Otherwise it returns RRCRCE_FAILURE_MAJOR.

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_rrc_abort_cnf
(
  void
)
{
  mm_cmd_type *cmd_ptr;                      /* Pointer to the MM Command */

  rrcrce_send_abort_cnf = FALSE;

  rrcrce_sig_con_delay = FALSE;

  /* Allocate memory for RRC_ABORT_CNF command */
  cmd_ptr =  mm_rrc_get_cmd_buf(RRC_ABORT_CNF);
  /* No paramters to fill in for RRC_ABORT_CNF command */

#ifdef FEATURE_DUAL_SIM
  cmd_ptr->cmd.rrc_abort_cnf.as_id = rrc_get_as_id();
#endif

  /* Update the Max HFNs values to be stored in SIM or USIM */ 
  (void) rrcsmc_append_hfns_to_rel_ind(cmd_ptr->cmd.rrc_abort_cnf.hfn_cs,
                                       cmd_ptr->cmd.rrc_abort_cnf.hfn_ps);

  cmd_ptr->cmd.rrc_abort_cnf.conn_rel_cause = rrc_release_cause;

  rrc_send_rrc_message_to_mm(cmd_ptr);               /* Sends the command to MM */
WRRC_MSG2_HIGH("Dispatching out MM cmd / Sent RRC_ABORT_CNF to MM with %d substate, rel_cause %d",rrcrce_substate, cmd_ptr->cmd.rrc_abort_cnf.conn_rel_cause);

  rrcrce_send_conn_rel_cause_event((uint8)RRC_ABORT_CAUSE_TYPE_MAX + (uint8)rrc_release_cause);   
#ifdef FEATURE_DUAL_SIM
  rrc_release_lock();
#endif
#ifdef FEATURE_3GPP_CSFB
  /*Connection is aborted, reset CSFB call status flag*/
  rrc_csfb_call_status = FALSE;
#endif

  /* Initialize release cause */
  rrc_release_cause = RRC_REL_CAUSE_UNSPEC;

  rrcrce_cell_resel_freq = 0;
  rrcrce_cell_resel_scr = 0;

  return;
}/*rrcrce_send_rrc_abort_cnf */
#ifdef FEATURE_QSH_EVENT_METRIC
/*===========================================================================

FUNCTION rrcrce_qsh_log_connection_est_failure

DESCRIPTION
  This function logs connection establishmnet failure 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrcrce_qsh_log_connection_est_failure(void)
{
  wrrc_call_end_data_log_type call_end_data;
  call_end_data.type = WRRC_CALL_END_TYPE_RRC_CONNECTION_EST_FAILURE;
  wrrc_qsh_log_conn_end_info(WRRC_METRIC_LOG,&call_end_data);
}
#endif
/*===========================================================================

FUNCTION RRCRCE_SEND_CONN_SETUP_IND

DESCRIPTION
  This function builds RRC_CONN_SETUP_IND command to post to CHO proc.The calling
  function should pass in the paramters to be sent in the command. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_conn_setup_ind(rrc_conn_setup_ind_type conn_setup_ind_params)
{
  rrc_cmd_type *int_cmd_ptr = NULL;

  /* Allocates the buffer to RRC Internal command */
  int_cmd_ptr =  rrc_get_int_cmd_buf();
  /* Fill in the command parameters for RRC_CONN_SETUP_IND */
  int_cmd_ptr->cmd_hdr.cmd_id = RRC_CONN_SETUP_IND;
  int_cmd_ptr->cmd.rrc_conn_setup_ind.status = conn_setup_ind_params.status;
  int_cmd_ptr->cmd.rrc_conn_setup_ind.conn_failure_reason = 
    conn_setup_ind_params.conn_failure_reason;
  
  if ((conn_setup_ind_params.status == FAILURE)&&
      (conn_setup_ind_params.conn_failure_reason == RRC_MAX_RACH_ATTEMPTS)
     )
  {
    rrc_log_protocol_error( (uint8) (rrc_get_state()),
                            (uint8) RRC_PROCEDURE_RCE,
                            (uint8) RRCLOG_RachFailure,
                            (uint8) RRCLOG_PROTOCOL_ERROR_NA
                          );
    
  }
  
  /* Sends conn_setup_ind to cho procedure */
  rrc_put_int_cmd(int_cmd_ptr);     
  WRRC_MSG2_HIGH("Sending RRC_CONN_SETUP_IND to CHO.Status:%d, Failure reason %d",
                         int_cmd_ptr->cmd.rrc_conn_setup_ind.status,
                         int_cmd_ptr->cmd.rrc_conn_setup_ind.conn_failure_reason);
}
/*===========================================================================

FUNCTION rrcrce_validate_rrc_est_req

DESCRIPTION
  This function checks whether RRC_EST_REQ contains a valid Initial UE
  indentity.
DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type: It returns RRCRCE_SUCCESS if it finds a valid
  Initial UE Identity in RRC_EST_REQ command to the MM.
  Otherwise it returns RRCRCE_FAILURE_MINOR.

SIDE EFFECTS
  None
===========================================================================*/
rrcrce_status_e_type rrcrce_validate_rrc_est_req
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC Command */
)
{
  rrcrce_status_e_type status = RRCRCE_FAILURE_MINOR; 
                                             /* The status is initialized to
                                                Failure */
  if(cmd_ptr->cmd_hdr.cmd_id == RRC_EST_REQ)
  {
    switch(cmd_ptr->cmd.rrc_est_req.ue_id_choice)
    {
      case RRC_IMSI_UE_ID:
        if(cmd_ptr->cmd.rrc_est_req.initial_ue_id.imsi.num_digits < 6)
        {
          WRRC_MSG0_ERROR("Invalid IMSI length is sent");
        }
        else
        {
          status = RRCRCE_SUCCESS;
        }
        break;
      case RRC_TMSI_LAI_UE_ID:
        if ((cmd_ptr->cmd.rrc_est_req.initial_ue_id.tmsi_lai.lai.plmn.num_mnc_digits == 3) &&
            (cmd_ptr->cmd.rrc_est_req.initial_ue_id.tmsi_lai.lai.plmn.mnc[2] > 9))
        {
          WRRC_MSG1_HIGH("MM Sent mnc as 3 digits and last digit %d, reducing to 2 digits", 
                   cmd_ptr->cmd.rrc_est_req.initial_ue_id.tmsi_lai.lai.plmn.mnc[2]);
          cmd_ptr->cmd.rrc_est_req.initial_ue_id.tmsi_lai.lai.plmn.num_mnc_digits = 2;
        }
        status = RRCRCE_SUCCESS;
        break;

      case RRC_PTMSI_RAI_UE_ID:
        if ((cmd_ptr->cmd.rrc_est_req.initial_ue_id.ptmsi_rai.rai.lai.plmn.num_mnc_digits == 3) &&
            (cmd_ptr->cmd.rrc_est_req.initial_ue_id.ptmsi_rai.rai.lai.plmn.mnc[2] > 9))
        {
          WRRC_MSG1_HIGH("MM Sent mnc as 3 digits and last digit %d, reducing to 2 digits", 
                   cmd_ptr->cmd.rrc_est_req.initial_ue_id.ptmsi_rai.rai.lai.plmn.mnc[2]);
          cmd_ptr->cmd.rrc_est_req.initial_ue_id.ptmsi_rai.rai.lai.plmn.num_mnc_digits = 2;
        }
        status = RRCRCE_SUCCESS;
        break;
    
    
      case RRC_IMEI_UE_ID:
        status = RRCRCE_SUCCESS;
        break;

      default:
        WRRC_MSG1_HIGH("Invalid Initial UE Identity: %d",
                  cmd_ptr->cmd.rrc_est_req.ue_id_choice);
        break;
    }
  }
  return (status);

}/*rrcrce_validate_rrc_est_req */
/*===========================================================================

FUNCTION rrcrce_process_rce_initial_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in RCE_INITIAL substate of Disconnected State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_initial_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to RRC command */
)
{
  rrc_abort_cause_e_type  cause = RRC_MAX_ACCESS_FAILURE;

  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_EST_REQ: 
      rrc_protocol_error_indicator = rrc_ProtocolErrorIndicator_noError;

      /* Resets the V300 variable */
      rrc_v_300 = 0;
   
      /* Initialize the Transaction Id for RRC CONNECTION SETUP to Invalid */
      rrcrce_setup_transaction_identifier = RRCRCE_INVALID_TRANSACTION_ID;
      
      /* Initialize the Transaction Id for RRC CONNECTION REJECT to Invalid */
      rrcrce_reject_transaction_identifier = RRCRCE_INVALID_TRANSACTION_ID;

      /* Initialize the Wait Time to zero seconds */
      rrcrce_wait_time = 0;

      /* Initialize abort flag to FALSE */
      rrcrce_send_abort_cnf = FALSE;

      rrcrce_abort_ind_sent = FALSE;
      rrcrce_lai_rai_mismatch = FALSE;

      /* Initialize release cause to unspecified */
      rrc_release_cause = RRC_REL_CAUSE_UNSPEC;

      if(RRCRCE_SUCCESS == rrcrce_validate_rrc_est_req(cmd_ptr))
      {
        /*updates the establishment cause */
        rrc_establishment_cause = cmd_ptr->cmd.rrc_est_req.cause;
      
        /* Updates Initial UE Identity */
        rrc_initial_ue_identity_choice = cmd_ptr->cmd.rrc_est_req.ue_id_choice;

        rrc_initial_ue_identity = cmd_ptr->cmd.rrc_est_req.initial_ue_id;

        rrc_est_cn_domain_id = cmd_ptr->cmd.rrc_est_req.cn_domain_id;

        rrc_est_call_type = cmd_ptr->cmd.rrc_est_req.call_type;

        rrc_est_req_for_rai = cmd_ptr->cmd.rrc_est_req.rai;

        MSG_6(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
             "RRC_EST_REQ for cn_domain: %d, RAI PLMN %d-%d, RAC %d, LAC: %x %x",
             rrc_est_cn_domain_id,RRC_CSP_GET_INT_MCC(rrc_est_req_for_rai.lai.plmn),
             RRC_CSP_GET_INT_MNC(rrc_est_req_for_rai.lai.plmn),rrc_est_req_for_rai.rac[0], rrc_est_req_for_rai.lai.lac[0],
             rrc_est_req_for_rai.lai.lac[1]);

        rrcrce_send_conn_req_cause_event(rrc_establishment_cause);

        /* Function call to CSP to know whether UE is camped on  */
        if (RRC_CAMPED_ON == rrc_ccm_get_curr_camping_status(RRC_PROCEDURE_RCE))
        {
          if(RRCRCE_SUCCESS == rrcrce_validate_cell_access_status(
                                   rrc_est_cn_domain_id,
                                   rrc_establishment_cause,
                                   &cause)
            )
          {
            if(WTOW_SEARCH_STOP != rrc_csp_bplmn_srch_in_progress())
            {
              /* Due to race condition because of "context-switching", 
               * RCE needs to update it's substate before putting 
               * RRC_BPLMN_SUSPEND_REQ in the queue 
               */
              rrcrce_substate = RRCRCE_WAIT_FOR_UE_CAMP_ON;
              /* With the below feature defined we will suspend and resume
               * the BPLMN search for any establishment cause received 
               * from NAS
               */
              rrc_csp_send_bplmn_suspend_req(RRC_PROCEDURE_RCE, BPLMN_SUSPEND_REASON_REGISTRATION);

              /* Starting this timer is NOT required as CSP on receiving RRC_BPLMN_SUSPEND_REQ
               * will send RRC_BPLMN_SUSPEND_CNF almost immediately. This timer is started only 
               * as a recovery mechanism in case CSP fails to send RRC_BPLMN_SUSPEND_CNF to RCE
               */
              rrctmr_start_timer(RRCTMR_UE_TO_CAMP_ON_TIMER, 
                            (unsigned long) RRCRCE_TIMER_VALUE_FOR_UE_TO_CAMP_ON);       
            }
            else
            {
#ifdef FEATURE_DUAL_SIM
              if(rrc_acquire_lock_for_est_req(rrc_establishment_cause,0xfffffff,RRCTMR_WRM_LOCK_WAIT_TIMER_FOR_EST_REQ_IN_MS) == FAILURE)
              {
                /* Send RRC_ABORT_IND command to the MM */
                rrcrce_send_rrc_abort_ind(RRC_NO_RESOURCE_AVAILABLE);      
                return;
              }
#endif
               /* UE is camped on, then Check whether SIB7 is current */
              if(TRUE == rrcsib_check_and_update_sib7_validity (RRC_PROCEDURE_RCE))
              { 
                 /* UE is camped on and SIB7 is current, then Setup RB0 */
                 rrcrce_send_rrc_channel_config_req_for_rb0(ESTABLISH_RB,
                                                           RRC_STATE_CONNECTING,
                                                           TRUE ); 
                 rrcrce_substate = RRCRCE_WAIT_FOR_RB0_SETUP_CNF;     
              }    
              else
              {
                WRRC_MSG0_HIGH("RCE waits for SIB7 to become current"); 
                rrcrce_substate = RRCRCE_WAIT_FOR_SIB7_CURRENT;
              }
            }
          }
          else
          {
            /* FEATURE_WCDMA_ALLOW_ECAL_AC_BARRED_CELL */
            /* If the cell access class does not allow emergency call fake an immediate
             * NO SERVICE to NAS by setting the boolean est_req_for_emerg_call to TRUE.
             * CSP will take care of posting NO SERVICE to NAS
             */
            if(rrc_establishment_cause == RRC_EST_EMERGENCY_CALL)
            {
              rrcrce_send_rrc_initiate_cell_selection_ind(RRC_OUT_OF_SERVICE_AREA_IND,rrc_get_state());
              rrccsp_set_est_req_for_emerg_call();
            }
#ifdef FEATURE_WCDMA_CSFB_CALL_OPT
            else if((WCDMA_RRC_IDLE_FEATURES(RRC_CSFB_CALL_TIMELINE_OPT)) && rrc_csfb_call_status)
            {
              //Trigger OOS to avoid UE being stuck in BCH.
              rrcrce_send_rrc_initiate_cell_selection_ind(RRC_OUT_OF_SERVICE_AREA_IND,rrc_get_state());
            }
#endif
            if(rrc_est_cn_domain_id == RRC_CS_DOMAIN_CN_ID)
            {
              rrcrce_send_rrc_abort_ind(RRC_AC_NOT_ALLOWED_CS_DOMAIN);
            }
            else
            {
              rrcrce_send_rrc_abort_ind(RRC_AC_NOT_ALLOWED_PS_DOMAIN);
            }
          }
        }
        else /* UE is not camped on */
        {
          rrcrce_substate = RRCRCE_WAIT_FOR_UE_CAMP_ON;

          /* start the timer for UE to camp on */
          rrctmr_start_timer(RRCTMR_UE_TO_CAMP_ON_TIMER, 
                        (unsigned long) RRCRCE_TIMER_VALUE_FOR_UE_TO_CAMP_ON);       
        }
      }
      else /* Validation Failed */
      {
        boolean pcco_in_progress=rrccho_is_gtow_pcco_in_progress();
        WRRC_MSG1_ERROR("Validation failure in RRC_EST_REQ, PCCO %d",pcco_in_progress);

        /* If GTOW is in progress,send a conn_setup_ind with failure to CHO
           CHO would deactivate WCDMA and pass control back to GSM.Do not send
           RRC_ABORT_IND to MM if GTOW is active */
        if(pcco_in_progress)
        { 
          rrc_conn_setup_ind_type conn_setup_ind_params;
          conn_setup_ind_params.status = FAILURE;
          conn_setup_ind_params.conn_failure_reason = RRC_CONN_FAILURE_OTHER;
          rrcrce_send_conn_setup_ind(conn_setup_ind_params);                  

          /* Reset RCE susbstate */
          rrcrce_substate = RRCRCE_INITIAL;
          return;
        }
#ifdef FEATURE_WCDMA_CSFB_CALL_OPT
        if((WCDMA_RRC_IDLE_FEATURES(RRC_CSFB_CALL_TIMELINE_OPT)) && rrc_csfb_call_status)
        {
          //Trigger OOS to avoid UE being stuck in BCH.
          rrcrce_send_rrc_initiate_cell_selection_ind(RRC_OUT_OF_SERVICE_AREA_IND,rrc_get_state());
        }
#endif
        /* Send RRC_ABORT_IND command to the MM */
        rrcrce_send_rrc_abort_ind(RRC_UNSUPPORTED_INITIAL_UE_ID);     

      }
      break;

    case RRC_NEW_CELL_IND:   
      /* This Command will come from CSP in RRCRCE_INITIAL substate if RRC
         is receives RRC_ABORT_REQ from NAS while  waiting for UE
         to camp on and in that case RCE simply ignores it*/       
      break;

    case RRC_STATE_CHANGE_IND:
      MSG_MED("Unexpected Event: %d in the substate:%d",
               cmd_ptr->cmd_hdr.cmd_id, rrcrce_substate, 0);
      break;

    case RRC_ABORT_REQ:
      /* Send RRC_ABORT_CNF command to the MM */
      rrcrce_send_rrc_abort_cnf();      
        /* At this time, none of the RBs are setup. Simply reset the RCE
         substate */
      rrcrce_substate = RRCRCE_INITIAL; /* The RCE Procedure ends */
      break;

    default:     
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id, rrcrce_substate);
      break;
  }  
} /* rrcrce_process_rce_initial_substate */

/*===========================================================================

FUNCTION rrcrce_process_rce_wait_for_sib7_current

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in RRCRCE_WAIT_FOR_SIB7_CURRENT  substate of 
  Disconnected State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_sib7_current
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  rrc_abort_cause_e_type  cause = RRC_MAX_ACCESS_FAILURE;

  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_SIB7_UPDATE_STATUS_IND: /* This command actually represents
                                        that SIB 7 is current */   
      if (cmd_ptr->cmd.sib7_update_status.status == FAILURE)
      {
        boolean pcco_in_progress = rrccho_is_gtow_pcco_in_progress();
        WRRC_MSG1_HIGH("SIB7 Update Status Ind with failure PCCO %d",pcco_in_progress);

        /* If GTOW is in progress,send a conn_setup_ind with failure to CHO.
           CHO would deactivate WCDMA and pass control back to GSM.Do not send
           RRC_ABORT_IND to MM if GTOW is active */
        if(pcco_in_progress)
        { 
          rrc_conn_setup_ind_type conn_setup_ind_params;
          conn_setup_ind_params.status = FAILURE;
          conn_setup_ind_params.conn_failure_reason = RRC_CONN_FAILURE_OTHER;
          rrcrce_send_conn_setup_ind(conn_setup_ind_params);
          
          /* The RCE Procedure ends */
          rrcrce_substate = RRCRCE_INITIAL;    /* reset the RCE substate */
          break;
        }
        
        rrcrce_send_rrc_abort_ind(RRC_INTERNAL_ERROR);      
        /* The RCE Procedure ends */
        rrcrce_substate = RRCRCE_INITIAL;    /* reset the RCE substate */
        break;
      }    
    
      /* Send the internal command to RRC LLC to establish RB 0 */
        rrcrce_send_rrc_channel_config_req_for_rb0(ESTABLISH_RB,
        RRC_STATE_CONNECTING, TRUE); 
        rrcrce_substate = RRCRCE_WAIT_FOR_RB0_SETUP_CNF;     
      break;

    case RRC_ABORT_REQ:
      /* Call SIB fn to forcefully initialize its active event if any */
      rrcsib_force_sib_event_init(RRCSIB_ABORT_SIB7_UPDATE);

      /* Send RRC_ABORT_CNF command to the MM */
      rrcrce_send_rrc_abort_cnf();      
        /* At this time, none of the RBs are setup. Simply reset the RCE
         substate */
      rrcrce_substate = RRCRCE_INITIAL; /* The RCE Procedure ends */
      break;

    case RRC_NEW_CELL_IND:
      WRRC_MSG0_HIGH("Got RRC_NEW_CELL_IND in RCE sub statewait_for_sib7_current ");
      if(RRCRCE_SUCCESS != rrcrce_validate_cell_access_status(
                            rrc_est_cn_domain_id,
                            rrc_establishment_cause,
                            &cause)
        )
      {
        if(rrc_est_cn_domain_id == RRC_CS_DOMAIN_CN_ID)
        {
          rrcrce_send_rrc_abort_ind(RRC_AC_NOT_ALLOWED_CS_DOMAIN);
        }
        else
        {
          rrcrce_send_rrc_abort_ind(RRC_AC_NOT_ALLOWED_PS_DOMAIN);
        }
        rrcrce_substate = RRCRCE_INITIAL; /* The RCE Procedure ends */
        break;
      }
      /* Send the internal command to RRC LLC to establish RB 0 */
        rrcrce_send_rrc_channel_config_req_for_rb0(ESTABLISH_RB,
        RRC_STATE_CONNECTING, TRUE); 
        rrcrce_substate = RRCRCE_WAIT_FOR_RB0_SETUP_CNF;     
      break;
      
    default:     
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcrce_substate);
      break;
  }  
} /* rrcrce_process_rce_wait_for_sib7_current */


/*===========================================================================

FUNCTION rrcrce_process_rce_wait_for_ue_camp_on_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in RCE_WAIT_FOR_UE_CAMP_ON  substate of 
  Disconnected State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_ue_camp_on_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  rrc_abort_cause_e_type  cause = RRC_MAX_ACCESS_FAILURE;

  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_NEW_CELL_IND:  /* This command actually represents UE Camp on 
                               Indication */ 
  
      /* Stop the timer to UE to camp on */  
      rrctmr_stop_timer(RRCTMR_UE_TO_CAMP_ON_TIMER);

      if(RRCRCE_SUCCESS != rrcrce_validate_cell_access_status(
                              rrc_est_cn_domain_id,
                              rrc_establishment_cause,
                              &cause)
        )
      {
        if(rrc_est_cn_domain_id == RRC_CS_DOMAIN_CN_ID)
        {
          rrcrce_send_rrc_abort_ind(RRC_AC_NOT_ALLOWED_CS_DOMAIN);
        }
        else
        {
          rrcrce_send_rrc_abort_ind(RRC_AC_NOT_ALLOWED_PS_DOMAIN);
        }
        rrcrce_substate = RRCRCE_INITIAL; /* The RCE Procedure ends */
        return;
      }
#ifdef FEATURE_DUAL_SIM
      if(rrc_acquire_lock_for_est_req(rrc_establishment_cause,0xfffffff,RRCTMR_WRM_LOCK_WAIT_TIMER_FOR_EST_REQ_IN_MS) == FAILURE)
      {
        /* Send RRC_ABORT_IND command to the MM */
        rrcrce_send_rrc_abort_ind(RRC_NO_RESOURCE_AVAILABLE);     
        rrcrce_substate = RRCRCE_INITIAL; /* The RCE Procedure ends */
        return;
      }
#endif
      /* UE is camped on and SIB7 is current, then Setup RB0 */
      rrcrce_send_rrc_channel_config_req_for_rb0(ESTABLISH_RB, RRC_STATE_CONNECTING,TRUE ); 
      rrcrce_substate = RRCRCE_WAIT_FOR_RB0_SETUP_CNF; 
      break;

    case RRC_BPLMN_SUSPEND_CNF:
      /* Stop the timer to UE to camp on */  
      rrctmr_stop_timer(RRCTMR_UE_TO_CAMP_ON_TIMER);

      if(cmd_ptr->cmd.rrc_csp_bplmn_suspend_cnf.status == SUCCESS)
      {
#ifdef FEATURE_DUAL_SIM
        if(rrc_acquire_lock_for_est_req(rrc_establishment_cause,0xfffffff,RRCTMR_WRM_LOCK_WAIT_TIMER_FOR_EST_REQ_IN_MS) == FAILURE)
        {
          /* Send RRC_ABORT_IND command to the MM */
          rrcrce_send_rrc_abort_ind(RRC_NO_RESOURCE_AVAILABLE);      
          rrcrce_substate = RRCRCE_INITIAL; /* The RCE Procedure ends */
          return;
        }
#endif
        /* UE is camped on, then Check whether SIB7 is current */
        if(TRUE == rrcsib_check_and_update_sib7_validity (RRC_PROCEDURE_RCE) )
        { 
          /* UE is camped on and SIB7 is current, then Setup RB0 */
            rrcrce_send_rrc_channel_config_req_for_rb0(ESTABLISH_RB,
            RRC_STATE_CONNECTING, TRUE );  
            rrcrce_substate = RRCRCE_WAIT_FOR_RB0_SETUP_CNF;     
          }    
        else
        {
          WRRC_MSG0_HIGH("RCE waits for SIB7 to become current"); 
          rrcrce_substate = RRCRCE_WAIT_FOR_SIB7_CURRENT;
        }    
      }
      else
      {
        /* Send RRC_ABORT_IND command to the MM */
        rrcrce_send_rrc_abort_ind(RRC_FAIL_TO_CAMP_ON);     
        rrcrce_substate = RRCRCE_INITIAL;    /* reset the RCE substate */
        }    
      break;
    case RRC_TIMER_TO_CAMP_ON_EXPIRED_IND:      

      WRRC_MSG1_HIGH("RRC_TIMER_TO_CAMP_ON_EXPIRED_IND in rce substate %d",rrcrce_substate);

      /* If GTOW is in progress,send a conn_setup_ind with failure to CHO.
         CHO would deactivate WCDMA and pass control back to GSM.Do not send
         RRC_ABORT_IND to MM if GTOW is active */
      if(rrccho_is_gtow_pcco_in_progress())
      { 
        rrc_conn_setup_ind_type conn_setup_ind_params;
        conn_setup_ind_params.status = FAILURE;
        conn_setup_ind_params.conn_failure_reason = RRC_CONN_FAILURE_OTHER;
        
        WRRC_MSG1_HIGH("RRC_TIMER_TO_CAMP_ON_EXPIRED_IND when PCCO active in rce substate %d",rrcrce_substate);
        rrcrce_send_conn_setup_ind(conn_setup_ind_params);

        /* The RCE Procedure ends */
        rrcrce_substate = RRCRCE_INITIAL;    /* reset the RCE substate */
        break;
      }
      
      /* Send RRC_ABORT_IND command to the MM */
      rrcrce_send_rrc_abort_ind(RRC_FAIL_TO_CAMP_ON);     
      rrcrce_substate = RRCRCE_INITIAL;    /* reset the RCE substate */
      break;

    case RRC_ABORT_REQ:
      /* Stop the timer for UE to camp on */  
      rrctmr_stop_timer(RRCTMR_UE_TO_CAMP_ON_TIMER);

     /* Send RRC_ABORT_CNF command to the MM */
      rrcrce_send_rrc_abort_cnf();     
        /* At this time, none of the RBs are setup. Simply reset the RCE
         substate */
      rrcrce_substate = RRCRCE_INITIAL; /* The RCE Procedure ends */
      break;

    default:     
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcrce_substate);
      break;
  }  
} /* rrcrce_process_rce_wait_for_ue_camp_on_substate */

#ifdef FEATURE_WCDMA_TO_LTE
/*===========================================================================

FUNCTION rrcrce_is_band_supported_for_eutra_neighbors

DESCRIPTION
  This function checks if the EUTRA bands corresponding to the EARFCN list provided
  in SIB19 is supported by UE.

DEPENDENCIES
  Calls LTE-RRC API lte_rrc_earfcn_is_supported().

RETURN VALUE
  FALSE: If none of the bands corresponding to EARFCN list in SIB19 is supported by UE.
  TRUE : Else.

SIDE EFFECTS
  None
===========================================================================*/
static boolean rrcrce_is_band_supported_for_eutra_neighbors
(
  void
)
{
  rrc_SysInfoType19 *sib19_ptr = NULL;

  /* If SIB 19 is not present or if SIB 19 is present but freq list is not provided, send 
      "Support of E-UTRA FDD" set to TRUE if UE supports LTE mode */
  boolean band_supported = TRUE;
  uint32 idx = 0;
  uint32 earfcn;

#ifdef FEATURE_WCDMA_TO_LTE
  sib19_ptr = (rrc_SysInfoType19 *)rrc_sibdb_return_sib_for_srv_cell(rrc_SIB19);
#endif
  if(sib19_ptr == NULL)
  {
    WRRC_MSG0_HIGH("WTOL: SIB19 is NULL.");
  }
  else
  {
    if(sib19_ptr->m.eutra_FrequencyAndPriorityInfoListPresent)
    {
      rrc_EUTRA_FrequencyAndPriorityInfoList *eutra_freq_pri_list_ptr = &(sib19_ptr->eutra_FrequencyAndPriorityInfoList);

      while((eutra_freq_pri_list_ptr->n > idx))
      {
        earfcn = eutra_freq_pri_list_ptr->elem[idx].earfcn;
        if(lte_rrc_earfcn_is_supported(earfcn, rrccsp_get_lte_band_pref()))
        {
          break;
        }
        idx++;
      }
      /* If freq list is given in SIB 19, but none of the bands corresponding to the EUTRA freq are supported by UE
         (eutra_freq_pri_list_ptr traverses to the end), do not send pre-redirection info in RRC connection 
         request message to network */
      if(eutra_freq_pri_list_ptr->n == 0)
      {
        band_supported = FALSE;
      }
    }
  }
  return band_supported;
}
#endif

/*===========================================================================

FUNCTION rrcrce_build_rrc_connection_request_msg

DESCRIPTION
  This function builds the RRC CONNECTION REQUEST message with all required
  parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_build_rrc_connection_request_msg
(
  rrc_UL_CCCH_Message *msg_ptr               /* Pointer to uplink CCCH message*/
)
{

#ifdef FEATURE_3GPP_CSFB
  rrc_RRCConnectionRequest_v860NonCriticalExtensions *rce_v860_ext = NULL;

  rrc_RRCConnectionRequest_v940NonCriticalExtensions *rce_v940_ext = NULL;
#endif
#ifdef FEATURE_WCDMA_REL10 
  #error code not present
#endif

#ifdef FEATURE_QSH_EVENT_METRIC
  rrc_log_rrc_conn_req_ota conn_req_ota;
  memset(&conn_req_ota,0,sizeof(rrc_log_rrc_conn_req_ota));
#endif
  /* Select the message as RRC CONNECTION REQUEST message */
  msg_ptr->message.t = T_rrc_UL_CCCH_MessageType_rrcConnectionRequest;


  /* Set the connection establishment cause properly if there
   * was a GTOW reselection or GTOW PCCO
   */
  WRRC_MSG1_HIGH("Est Cause %d", rrc_establishment_cause);
  rrcmcm_set_establishment_cause();
  WRRC_MSG1_HIGH("Est Cause after MCM update %d", rrc_establishment_cause);

  /* To update the proc id as RCE so that first IDT can be sent without cause */
  rrcidt_update_proc_id();

  /* Initializes the RRC Connection Establishment cause */
  msg_ptr->message.u.rrcConnectionRequest.establishmentCause = 
    rrcrce_update_establishment_cause();

  /* Initializes the initial UE Id parameter */
  rrcrce_update_initial_ue_identity(
    &msg_ptr->message.u.rrcConnectionRequest.initialUE_Identity);

  /* Initializes the Protocol Error Indicator */
  msg_ptr->message.u.rrcConnectionRequest.protocolErrorIndicator = 
    rrc_protocol_error_indicator;

  /* Initialize all optional fields to ommitted */
  RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionRequest);

 /* Function call to Measurement Control & Report procedure. */
  if(RRCMEAS_RACH_SUCCESSFUL == rrcmeas_append_meas_rpt_on_rach(
     &msg_ptr->message.u.rrcConnectionRequest.measuredResultsOnRACH, RRC_PROCEDURE_RCE, FALSE))
  {
  /* if Measurement Control & Reportprocedure returns RRC_MEAS_RACH_SUCCESFUL,
     then enable the  bit_mask field */
   RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.rrcConnectionRequest,
        rrc_RRCConnectionRequest,measuredResultsOnRACH);
   
  }
  else
  {
  /*"Measured Results on RACH" IE is optional, if Measurement Control & Report
     procedure returns other than RRC_MEAS_RACH_SUCCESFUL, then disable the
     bit_mask field */
    MSG_MED("Measured Results on RACH is not appended",0,0,0);
  } 
  
  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
  {
    WRRC_MSG0_HIGH("Setting REL 5 IN CON.REQ");
     
     RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.rrcConnectionRequest,
        v3d0NonCriticalExtensions);

    RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
      rRCConnectionRequest_v3d0ext);

    msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent =1;

    /* Indicate that we are a Release 5 UE */
    msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.
      rrcConnectionRequest_v4b0ext.accessStratumReleaseIndicator = rrc_AccessStratumReleaseIndicator_rel_5;

    /* Set vb0 non-critical extension present */
    msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
      v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent =1;

    RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
      v4b0NonCriticalExtensions.v590NonCriticalExtensions);

    /* Indicate that we do not support pre-determined config */
    msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
      rrcConnectionRequest_v590ext.predefinedConfigStatusInfo = FALSE;
  }

  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6)
  {
    WRRC_MSG0_HIGH("RRCEUL: Setting REL 6 IN CON.REQ");
    /* Indicate that we are a Release 6 UE */
    msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.
      rrcConnectionRequest_v4b0ext.accessStratumReleaseIndicator = rrc_AccessStratumReleaseIndicator_rel_6;
    
    msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
      v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v690NonCriticalExtensionsPresent =1;

    RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
      v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions);

      msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
        v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.m.v6b0NonCriticalExtensionsPresent = 1;
          
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
        v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.
              rrcConnectionRequest_v6b0ext);
          
          
      msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
        v4b0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.m.v6e0NonCriticalExtensionsPresent =1;
          
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
        v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions);
          
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
        v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
        rrcConnectionRequest_v6e0ext);
  
    if ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7) && (!(rrc_disable_cpc_fdpch_nv & NV_FDPCH_DISABLE)))
    {
      msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
        v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
        rrcConnectionRequest_v6e0ext.m.supportForFDPCHPresent =1;
          
      msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
        v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
        rrcConnectionRequest_v6e0ext.supportForFDPCH = rrc_RRCConnectionRequest_v6e0ext_IEs_supportForFDPCH_true;
  
      WRRC_MSG0_HIGH("FPDCH capability set");
    }


    RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
      v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
      rrcConnectionRequest_v690ext);

    msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
      v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
      rrcConnectionRequest_v690ext.m.ueCapabilityIndicationPresent =1;

    if (CHECK_WCDMA_OPTIONAL_FEATURE_HSUPA_SUPPORTED)
    {
      /* Indicate that UE supports for hsdch and edch*/
      msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
        v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
        rrcConnectionRequest_v690ext.ueCapabilityIndication = rrc_RRCConnectionRequest_v690ext_IEs_ueCapabilityIndication_hsdch_edch;
    }
    else
    {
      WRRC_MSG2_HIGH("RRCEUL: optional feature EUL/HSUPA(0x%x->0x%x) disabled through NV item",
        wcdma_nv_optional_feature_list, CHECK_WCDMA_OPTIONAL_FEATURE_HSUPA_SUPPORTED);

      /* Indicate that UE supports for hsdch only */
      msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
        v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
        rrcConnectionRequest_v690ext.ueCapabilityIndication = rrc_RRCConnectionRequest_v690ext_IEs_ueCapabilityIndication_hsdch;
    }


    if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
    {
     RRC_RESET_MSG_IE_PRESENT(
        msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
        v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions
        );
      msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
        v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
        m.v770NonCriticalExtensionsPresent =1;
  
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
        v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
        v770NonCriticalExtensions);

      RRC_RESET_MSG_IE_PRESENT(
      msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
        v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
        v770NonCriticalExtensions.rrcConnectionRequest_v770ext);
  
  
      msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
        v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
        v770NonCriticalExtensions.rrcConnectionRequest_v770ext.m.mac_ehsSupportPresent =1;
      msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
        v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
        v770NonCriticalExtensions.rrcConnectionRequest_v770ext.mac_ehsSupport = rrc_RRCConnectionRequest_v770ext_IEs_mac_ehsSupport_true;
            
   

#ifdef FEATURE_WCDMA_HS_FACH
      if(rrcenhstate_nv_supports_e_fach() == TRUE)
      {
        msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
          v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
          v770NonCriticalExtensions.rrcConnectionRequest_v770ext.m.hspdschReception_CellFachPresent = 1;
        msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
          v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
          v770NonCriticalExtensions.rrcConnectionRequest_v770ext.hspdschReception_CellFach = rrc_RRCConnectionRequest_v770ext_IEs_hspdschReception_CellFach_true;
  
        WRRC_MSG0_HIGH("ENHSTATE:Sending Enh Cell_FACH capability");
      }
#endif /*FEATURE_WCDMA_HS_FACH*/

     if(rrcueci_nv_supports_cpc_dtx_on_dpcch())
     {
        WRRC_MSG0_HIGH("RRCCPC:Indicate UE support CPC-DTX operation on UL DPCCH");

        RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
          v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
          v770NonCriticalExtensions.rrcConnectionRequest_v770ext,
          rrc_RRCConnectionRequest_v770ext_IEs,discontinuousDpcchTransmission);
    
        msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
          v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
          v770NonCriticalExtensions.rrcConnectionRequest_v770ext.discontinuousDpcchTransmission = rrc_RRCConnectionRequest_v770ext_IEs_discontinuousDpcchTransmission_true;
     }

#ifdef FEATURE_WCDMA_REL7_SPECRS      
      if(l1_get_mobility_info())
      {
        WRRC_MSG0_HIGH("High Mobility Detected");
       RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
          v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
          v770NonCriticalExtensions.rrcConnectionRequest_v770ext,
          rrc_RRCConnectionRequest_v770ext_IEs,ueMobilityStateIndicator);  
        msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
          v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
          v770NonCriticalExtensions.rrcConnectionRequest_v770ext.ueMobilityStateIndicator = rrc_High_MobilityDetected_high_MobilityDetected;
      }
      else
      {
        WRRC_MSG0_HIGH("High Mobility not Detected");
      }
#endif

      if(!(rrc_disable_cpc_fdpch_nv & (NV_FDPCH_DISABLE|NV_EFDPCH_DISABLE)))
      {
        msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
          v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
          v770NonCriticalExtensions.m.v7b0NonCriticalExtensionsPresent =1 ;

        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
          v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
          v770NonCriticalExtensions.v7b0NonCriticalExtensions);
      
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
          v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
          v770NonCriticalExtensions.v7b0NonCriticalExtensions.rrcConnectionRequest_v7b0ext,supportForE_FDPCH);
        msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
          v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
          v770NonCriticalExtensions.v7b0NonCriticalExtensions.rrcConnectionRequest_v7b0ext.supportForE_FDPCH = rrc_RRCConnectionRequest_v7b0ext_IEs_supportForE_FDPCH_true;
      }
    }

    /* Include CN domain indicator in RCE message .
       *If CN domain is PS then no need to include Call type.
       *If CN domain is CS then need to include Call type with following rules
       * If the value of the variable ESTABLISHMENT_CAUSE is set to "Originating Conversational Call" 
       *  or "Emergency Call":
       *     set the value of the IE "Call type" to 
       *              "speech", 
       *              "video" or 
       *              "other" according to the call being initiated. 
       *  other wise set call type to others.
    */
    MSG_HIGH("rrc_est_cn_domain_id = %d,rrc_establishment_cause =%d,rrc_est_call_type = %d",
                                                                                                  rrc_est_cn_domain_id,
                                                                                                  rrc_establishment_cause,
                                                                                                  rrc_est_call_type);
    switch (rrc_est_cn_domain_id) 
    {
      case RRC_CS_DOMAIN_CN_ID:
        msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
          v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
          rrcConnectionRequest_v690ext.domainIndicator.t = T_rrc_RRCConnectionRequest_v690ext_IEs_domainIndicator_cs_domain;
        if((rrc_establishment_cause == RRC_EST_ORIG_CONV_CALL)||
           (rrc_establishment_cause == RRC_EST_EMERGENCY_CALL)
          )
        {
          switch(rrc_est_call_type)
          {
            case RRC_CALL_TYPE_SPEECH:
              msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
                v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
                rrcConnectionRequest_v690ext.domainIndicator.u.cs_domain.csCallType = rrc_RRCConnectionRequest_v690ext_IEs_csCallType_speech; 
              break;
    
            case RRC_CALL_TYPE_VIDEO:
              msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
                v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
                rrcConnectionRequest_v690ext.domainIndicator.u.cs_domain.csCallType = rrc_RRCConnectionRequest_v690ext_IEs_csCallType_video; 
              break;
    
            case RRC_CALL_TYPE_OTHERS:
              msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
                v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
                rrcConnectionRequest_v690ext.domainIndicator.u.cs_domain.csCallType = rrc_RRCConnectionRequest_v690ext_IEs_csCallType_other;
              break;
    
            default:
              WRRC_MSG1_ERROR("Wrong est call type set by NAS = %d",rrc_est_call_type);
              msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
                 v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
                 rrcConnectionRequest_v690ext.domainIndicator.u.cs_domain.csCallType = rrc_RRCConnectionRequest_v690ext_IEs_csCallType_other; 
              break;
          }
        }
        else
        {
          msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
             v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
             rrcConnectionRequest_v690ext.domainIndicator.u.cs_domain.csCallType = rrc_RRCConnectionRequest_v690ext_IEs_csCallType_other; 
        }
        break;

      case RRC_PS_DOMAIN_CN_ID:
        msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
          v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
          rrcConnectionRequest_v690ext.domainIndicator.t = T_rrc_RRCConnectionRequest_v690ext_IEs_domainIndicator_ps_domain;
        break;

      default :
        msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
          v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.
          rrcConnectionRequest_v690ext.domainIndicator.t = T_rrc_RRCConnectionRequest_v690ext_IEs_domainIndicator_ps_domain;
        WRRC_MSG1_ERROR("Invalid CN domain I.D=%d",rrc_est_cn_domain_id);
        break;
    }
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
    #error code not present
#endif

  }
  if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
  {
    WRRC_MSG0_HIGH("RRCEUL: Setting REL 7 IN CON.REQ");
    /* Indicate that we are a Release 6 UE */
    msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.
      rrcConnectionRequest_v4b0ext.accessStratumReleaseIndicator = rrc_AccessStratumReleaseIndicator_rel_7;
  }

  if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8)
  {
    WRRC_MSG0_HIGH("REL8:Setting AS Rel Indicator in RRC Conn Request.");
    /* Indicate that we are a Release 8 UE */
    msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.
      rrcConnectionRequest_v4b0ext.accessStratumReleaseIndicator = rrc_AccessStratumReleaseIndicator_rel_8;

    /*As part of R7 feature, extension v770NonCriticalExtensions ha been added already. So we can assume that it's present here in R8.*/
    /*Here we add extension v7b0NonCriticalExtensions. Later we may need to add it earlier at R7 when E-FDPCH needs to be enabled.*/
    msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.m.
        v7b0NonCriticalExtensionsPresent =1;

   RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions);

    /*Here we add extension v7b0NonCriticalExtensions*/
    msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.m.
        v860NonCriticalExtensionsPresent =1;

    RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.
        v860NonCriticalExtensions);

    RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
        v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.
        v860NonCriticalExtensions.rrcConnectionRequest_v860ext);

#ifdef FEATURE_WCDMA_DC_HSDPA
    if((rrcueci_nv_supports_dc_hsdpa() == TRUE) && 
       (!rrccsp_check_if_em_plmn()))
    {
      msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
          v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.
          v860NonCriticalExtensions.rrcConnectionRequest_v860ext.m.multiCellSupportPresent =1;

      msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
          v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.
          v860NonCriticalExtensions.rrcConnectionRequest_v860ext.multiCellSupport = rrc_RRCConnectionRequest_v860ext_IEs_multiCellSupport_true;
    }
#endif /*FEATURE_WCDMA_DC_HSDPA*/

#ifdef FEATURE_WCDMA_TO_LTE
    if(rrcmcm_is_lte_mode_enabled() &&
       rrcrce_is_band_supported_for_eutra_neighbors() &&
       !rrccsp_is_preredirection_info_to_be_excluded())
    {
      msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
          v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.
          v860NonCriticalExtensions.rrcConnectionRequest_v860ext.m.pre_redirectionInfoPresent = 1;

      if(lte_rrc_lte_fdd_is_supported(rrccsp_get_lte_band_pref()))
      {
        msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
            v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.
            v860NonCriticalExtensions.rrcConnectionRequest_v860ext.pre_redirectionInfo.supportEUTRA_FDD = TRUE;
      }
      else
      {
        msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
            v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.
            v860NonCriticalExtensions.rrcConnectionRequest_v860ext.pre_redirectionInfo.supportEUTRA_FDD = FALSE;
      }

      if(lte_rrc_lte_tdd_is_supported(rrccsp_get_lte_band_pref()))
      {
        msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
            v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.
            v860NonCriticalExtensions.rrcConnectionRequest_v860ext.pre_redirectionInfo.supportEUTRA_TDD = TRUE;
      }
      else
      {
        msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
            v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.
            v860NonCriticalExtensions.rrcConnectionRequest_v860ext.pre_redirectionInfo.supportEUTRA_TDD = FALSE;
      }
    }
#endif
#ifdef FEATURE_MAC_I
    if(rrcueci_nv_supports_mac_i() == TRUE)
    {
            /*Add the Mac-i support*/
      WRRC_MSG0_HIGH("MAC_I support added in RRC conn req");
      msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
          v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.
          v860NonCriticalExtensions.rrcConnectionRequest_v860ext.m.supportOfMACiisPresent =1;

      msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
          v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.
          v860NonCriticalExtensions.rrcConnectionRequest_v860ext.supportOfMACiis = rrc_RRCConnectionRequest_v860ext_IEs_supportOfMACiis_true;

    }
#endif /* FEATURE_MAC_I */
#ifdef FEATURE_WCDMA_HS_RACH
    if (TRUE == rrchsrach_nv_supports_hs_rach())
    {
            /*Add the Mac-i support*/
          WRRC_MSG0_HIGH("RRCHSRACH HS-RACH support added in RRC conn req");
          msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
          v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.
          v860NonCriticalExtensions.rrcConnectionRequest_v860ext.m.supportOfCommonEDCHPresent =1;

      msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
          v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.v770NonCriticalExtensions.v7b0NonCriticalExtensions.
          v860NonCriticalExtensions.rrcConnectionRequest_v860ext.supportOfCommonEDCH = rrc_RRCConnectionRequest_v860ext_IEs_supportOfCommonEDCH_true;

    }
#endif /* FEATURE_WCDMA_HS_RACH */

  }

#ifdef FEATURE_WCDMA_REL9
  if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9)
  {
    WRRC_MSG0_HIGH("REL9:Setting AS Rel Indicator in RRC Conn Request.");
    /* Indicate that we are a Release 9 UE */
    msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.
      rrcConnectionRequest_v4b0ext.accessStratumReleaseIndicator = rrc_AccessStratumReleaseIndicator_rel_9;
  }
#endif
#ifdef FEATURE_WCDMA_REL10
  #error code not present
#endif

#ifdef FEATURE_WCDMA_TO_LTE
  if(rrcmcm_is_lte_mode_enabled() &&
#ifdef LTE_BAND_NUM
      (rrccsp_check_if_lte_band_pref_is_set(rrccsp_get_lte_band_pref()) == TRUE))
#else
      (rrccsp_get_lte_band_pref() != 0))
#endif
  {
    rrcueci_send_eutra_capabilitites_req_to_lte();
  }
#endif

#ifdef FEATURE_3GPP_CSFB
  /* Setting the CSFB Indicator IE(CR5026) and the support for system information container IE(CR4321) */
  if(TRUE == rrc_csfb_call_status)
  {
    rce_v860_ext = &msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.
      v4b0NonCriticalExtensions.v590NonCriticalExtensions.v690NonCriticalExtensions.v6b0NonCriticalExtensions.v6e0NonCriticalExtensions.
      v770NonCriticalExtensions.v7b0NonCriticalExtensions.v860NonCriticalExtensions;

    rce_v860_ext->m.v7e0NonCriticalExtensionsPresent =1 ;

    rce_v860_ext->v7e0NonCriticalExtensions.m.v7g0NonCriticalExtensionsPresent =1 ;

    rce_v860_ext->v7e0NonCriticalExtensions.v7g0NonCriticalExtensions.m.v920NonCriticalExtensionsPresent =1 ;

    rce_v860_ext->v7e0NonCriticalExtensions.v7g0NonCriticalExtensions.
      v920NonCriticalExtensions.m.v940NonCriticalExtensionsPresent =1 ;

    rce_v940_ext = &rce_v860_ext->v7e0NonCriticalExtensions.v7g0NonCriticalExtensions.
      v920NonCriticalExtensions.v940NonCriticalExtensions;

    /* This is stored as part of Memory optimization. This gives the info if stored SI container
       is available with RRC or not */
    if(NULL != sibdb.rrc_decoded_sib_cont)
    {
      rce_v940_ext->rrcConnectionRequest_v940ext.m.systemInformationContainerStoredIndicatorPresent =1 ;

      rce_v940_ext->rrcConnectionRequest_v940ext.systemInformationContainerStoredIndicator = rrc_RRCConnectionRequest_v940ext_IEs_systemInformationContainerStoredIndicator_true;
    }

    rce_v940_ext->m.va40NonCriticalExtensionsPresent =1 ;

    rce_v940_ext->va40NonCriticalExtensions.m.v9b0NonCriticalExtensionsPresent =1 ;

    rce_v940_ext->va40NonCriticalExtensions.v9b0NonCriticalExtensions.rrcConnectionRequest_v9b0ext.m.cSFBIndicationPresent =1;

    rce_v940_ext->va40NonCriticalExtensions.v9b0NonCriticalExtensions.
      rrcConnectionRequest_v9b0ext.cSFBIndication = rrc_RRCConnectionRequest_v9b0ext_IEs_cSFBIndication_true;

#ifdef FEATURE_QSH_EVENT_METRIC
     wrrc_qsh_log_iho_stats(LTE_RAT_CSFB_MOB, WRRC_METRIC_UPDATE);
#endif
    WRRC_MSG1_HIGH("CSFB: Setting CSFB Indication %d [0 : SET]",rce_v940_ext->va40NonCriticalExtensions.v9b0NonCriticalExtensions.
      rrcConnectionRequest_v9b0ext.cSFBIndication);
    }
#endif

#ifdef FEATURE_WCDMA_REL10 
  #error code not present
#endif

#ifdef FEATURE_QSH_EVENT_METRIC
  conn_req_ota.cn_id = rrc_est_cn_domain_id;
  conn_req_ota.est_cause= rrc_establishment_cause;
  conn_req_ota.as_rel_indicator = msg_ptr->message.u.rrcConnectionRequest.v3d0NonCriticalExtensions.v4b0NonCriticalExtensions.
		rrcConnectionRequest_v4b0ext.accessStratumReleaseIndicator;
  rrc_qsh_log_conn_req_params(&conn_req_ota);
#endif
} /* rrcrce_build_rrc_connection_request_msg  */

/*===========================================================================

FUNCTION rrcrce_get_t300_from_serv_cell_db

DESCRIPTION
  This function reads the serving cell DB and returns the T300 value from the
  UE Timers and Constants in idle mode field

DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type : Returns RRCRCE_SUCCESS If it successful retrieves
  T300 from the SIB database. Otherwise it returns RRCRCE_FAILURE_MAJOR.

SIDE EFFECTS
  None
===========================================================================*/
rrcrce_status_e_type rrcrce_get_t300_from_serv_cell_db
(
  rrc_T_300 *rrcrce_t_300                    /* Pointer to T300 */ 
)
{
  rrcrce_status_e_type status = RRCRCE_FAILURE_MAJOR; 
                                            /* Status is initialized to
                                               Failure */
  rrc_SysInfoType1 *rrcrce_sib1;            /* Pointer to SIB 1 */

  /* Read SIB 1 from the SIB database */
  rrcrce_sib1 = (rrc_SysInfoType1 *)
    rrc_sibdb_return_sib_for_srv_cell(rrc_SIB1);

  /* Check whether SIB 1 is a valid one */
  if (rrcrce_sib1 != NULL)
  {
    *rrcrce_t_300= rrcrce_sib1 ->ue_IdleTimersAndConstants.t_300;
    status = RRCRCE_SUCCESS;
  }
  else
  {
    /* The UE should have camped and received a valid SIB1 before UE initiates
       RRC Connection Establishment procedure. */
    ERR_FATAL("SIB Database does not have a valid SIB 1",0,0,0);
  }
  return (status);
} /* rrcrce_get_t300_from_serv_cell_db */

/*===========================================================================

FUNCTION rrcrce_get_n300_from_serv_cell_db

DESCRIPTION
  This function reads the serving cell DB and returns the N300 value from the
  UE Timers and Constants in idle mode field

DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type : Returns RRCRCE_SUCCESS If it successful retrieves
  N300 from the SIB database. Otherwise it returns RRCRCE_FAILURE_MAJOR.

SIDE EFFECTS
  None
===========================================================================*/
rrcrce_status_e_type rrcrce_get_n300_from_serv_cell_db
(
  rrc_N_300 *rrcrce_n_300                     /* Pointer to N300 */  
)
{
  rrcrce_status_e_type status = RRCRCE_FAILURE_MAJOR; 
                                            /* Status is initialized to
                                               Failure */
  rrc_SysInfoType1 *rrcrce_sib1;            /* Pointer to SIB 1 */
    /* Read SIB 1 from the SIB database */
  rrcrce_sib1 = (rrc_SysInfoType1 *)rrc_sibdb_return_sib_for_srv_cell(rrc_SIB1);
  
    /* Check whether SIB 1 is a valid one */
    if (rrcrce_sib1 != NULL)
    {
      *rrcrce_n_300= rrcrce_sib1 ->ue_IdleTimersAndConstants.n_300;
      WRRC_MSG1_HIGH("N300 is %d",rrcrce_sib1 ->ue_IdleTimersAndConstants.n_300); 
      status = RRCRCE_SUCCESS;
    }
    else
    {
      /* The UE should have camped and received a valid SIB1 before UE initiates
         RRC Connection Establishment procedure. */
      WRRC_MSG0_HIGH("SIB Database does not have a valid SIB 1");
    }
 
  return (status);
} /* rrcrce_get_n300_from_serv_cell_db */

/*===========================================================================

FUNCTION rrcrce_convert_t300_to_ms

DESCRIPTION
  This function converts the rrc_T_300 enums to milliseconds

DEPENDENCIES
  None

RETURN VALUE
  int

SIDE EFFECTS
  None
===========================================================================*/
uint32 rrcrce_convert_t300_to_ms
(
  rrc_T_300 rrcrce_t_300                   /* enum for Timer T300 */
)
{
  uint32 timer_value;                       /* Timer Value in milli seconds */

  switch (rrcrce_t_300)
  {
    case rrc_T_300_ms100:
      timer_value = 100;
      break;
    /* 200 milli seconds interval*/
    case rrc_T_300_ms200:
      timer_value = 200;
      break;      
    case rrc_T_300_ms400:
      timer_value = 400;
      break;
    case rrc_T_300_ms600:
      timer_value = 600;
      break;
    case rrc_T_300_ms800:
      timer_value = 800;
      break;
    case rrc_T_300_ms1000:
      timer_value = 1000;
      break;
    case rrc_T_300_ms1200:
      timer_value = 1200;
      break;
    case rrc_T_300_ms1400:
      timer_value = 1400;
      break;
    case rrc_T_300_ms1600:
      timer_value = 1600;
      break;
    case rrc_T_300_ms1800:
      timer_value = 1800;
      break;
    case rrc_T_300_ms2000:
      timer_value = 2000;
      break;
    /*  1000 milli seconds interval*/
    case rrc_T_300_ms3000:
      timer_value = 3000;
      break;
    case rrc_T_300_ms4000:
      timer_value = 4000;
      break;
    case rrc_T_300_ms6000:
      timer_value = 6000;
      break;
    case rrc_T_300_ms8000:
      timer_value = 8000;
      break;
    default:
      WRRC_MSG0_ERROR("Unknown Timer value rcvd from SIB1");
      timer_value = 100;  /* Set lowest timer value as a default */
      break;
  }
  WRRC_MSG1_HIGH("T300 is %d ms",timer_value); 
  return (timer_value);
} /* rrcrce_convert_t300_to_ms*/

   
/*===========================================================================

FUNCTION rrcrce_process_rce_wait_for_rb0_phychan_resetup_cnf_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in RRCRCE_WAIT_FOR_RB0_PHYCHAN_RESETUP_CNF  substate of 
  Connecting State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_rb0_phychan_resetup_cnf_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  rrc_abort_cause_e_type  cause = RRC_MAX_ACCESS_FAILURE;

  switch(cmd_ptr->cmd_hdr.cmd_id) 
  {
    case RRC_ABORT_REQ:
      rrcrce_send_abort_cnf = TRUE;
      break;

    case RRC_STATE_CHANGE_IND:
      WRRC_MSG0_HIGH("State change ind received by RCE");
      /* Stop the timer T300 */
      rrctmr_stop_timer(RRCTMR_T_300_TIMER);

      if (cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED)
      {
        if (rrcrce_send_abort_cnf == TRUE)
        {
           /* Send RRC_ABORT_CNF command to the MM */
            rrcrce_send_rrc_abort_cnf();     
            /* At this time, none of the RBs are setup. Simply reset the RCE
            substate */
            rrcrce_substate = RRCRCE_INITIAL; /* The RCE Procedure ends */
            break;
        }    
        else
        {     
          rrcrce_send_rrc_abort_ind(RRC_FAILURE_UNSPECIFIED);     
          rrcrce_substate = RRCRCE_INITIAL;    /* reset the RCE substate */
        }    
      }

      break;

    case RRC_CHANNEL_CONFIG_CNF:
      /* The RRC LLC should send first Confirmation before it informs the 
         State Change Manager to go to CONNECTING STATE */
      if(cmd_ptr->cmd.chan_config_cnf.procedure == RRC_PROCEDURE_RCE)
      {
        rrcllcpcie_initialise_sync_a_post_veri_info();

        if(cmd_ptr->cmd.chan_config_cnf.transaction_id == rce_chl_config_transaction_id)
        {
          if( RRCLLC_CHAN_CFG_SUCCESS == cmd_ptr->cmd.chan_config_cnf.chan_cfg_status ) 
          {
            if(rrcrce_send_abort_cnf)
            {
               /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
               rrcrce_send_rrc_initiate_cell_selection_ind(
               RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
               /* Wait for successful release of RB0 */
               rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;
            }    
            else
            {
              if(RRCRCE_SUCCESS == rrcrce_validate_cell_access_status(
                                    rrc_est_cn_domain_id,
                                    rrc_establishment_cause,
                                    &cause))
              {
                /* Sends the RRC Connection Request message to Send Chain  and This
                   function also starts the timer T300 */
                rrcrce_send_rrc_connection_request_msg();
                /* The RCE procedure waits for the RRC Connection Setup Message */
                rrcrce_substate = RRCRCE_WAIT_FOR_RRC_CONN_SETUP;

                /* Increments the re-transmission counter for RRC Connection
                   Request Message */
                WRRC_MSG1_HIGH("RRC Connection Request is sent: %d",rrc_v_300);

                rrc_v_300++;  
              }
              else
              {
                /* Release the RB 0 and request the RRC LLC to go to Idle disconnected */
                  rrcrce_send_rrc_initiate_cell_selection_ind(
                  RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                  /* The RCE Procedure ends */
                  rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF; /* change the RCE substate */
                  if(cause  ==  RRC_ACCESS_CLASS_NOT_ALLOWED)
                  {
                    if(rrc_est_cn_domain_id == RRC_CS_DOMAIN_CN_ID)
                    {
                      rrcrce_mm_abort_cause = RRC_AC_NOT_ALLOWED_CS_DOMAIN;
                    }
                    else
                    {
                      rrcrce_mm_abort_cause = RRC_AC_NOT_ALLOWED_PS_DOMAIN;
                    }
                  }
                  else
                  {
                    rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE;
                  }
                }
            }
          }
          else
          {
            event_report(EVENT_WCDMA_RRC_RB0_SETUP_FAILURE);

            if(rrcrce_send_abort_cnf)
            {
              rrcrce_send_rrc_abort_cnf();
              /* Clear the ESTABLISHED_RABS while bringing down RRC Connection*/
              rrc_clear_established_rabs();

              /* Reset Signalling Connections status */
              rrcscr_reset_signalling_connection_status();

              rrcrce_substate = RRCRCE_INITIAL;
            }
            else
            {
              /* Send RRC_ABORT_IND command to the MM */
              rrcrce_send_rrc_abort_ind(RRC_INTERNAL_ERROR);      
              rrcrce_substate = RRCRCE_INITIAL;    /* reset the RCE substate */
            }    
            /* Trigger Cell Selection */
            rrcrce_send_rrc_initiate_cell_selection_ind(
            RRC_CELL_SEL_RB0_SETUP_FAILURE, RRC_STATE_DISCONNECTED);
          }
        }
        else
        {
          WRRC_MSG2_HIGH("Ignoring tr_id %d in chl config cnf. Expected tr_id = %d", 
                     cmd_ptr->cmd.chan_config_cnf.transaction_id,
                      rce_chl_config_transaction_id);
        }
      }
      else
      {
        WRRC_MSG1_HIGH("RRC_CHANNEL_CONFIG_CNF's proc :rrc_proc_e_type_value%d",
                  cmd_ptr->cmd.chan_config_cnf.procedure);
      }

      break;

    default:     
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcrce_substate);
      break;
   }  

} /* rrcrce_process_rce_wait_for_rb0_phychan_resetup_cnf_substate  */


/*===========================================================================

FUNCTION rrcrce_process_rce_wait_for_rb0_setup_cnf_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in RRCRCE_WAIT_FOR_RB0_SETUP_CNF  substate of 
  Disconnected State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_rb0_setup_cnf_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  switch(cmd_ptr->cmd_hdr.cmd_id) 
  {
    case RRC_ABORT_REQ:
      rrcrce_send_abort_cnf = TRUE;
      break;

    case RRC_CHANNEL_CONFIG_CNF:
      /* The RRC LLC should send first Confirmation before it informs the 
         State Change Manager to go to CONNECTING STATE */
      if(cmd_ptr->cmd.chan_config_cnf.procedure == RRC_PROCEDURE_RCE)
      {
        rrcllcpcie_initialise_sync_a_post_veri_info();

        if(cmd_ptr->cmd.chan_config_cnf.transaction_id == rce_chl_config_transaction_id)
        {
          if( RRCLLC_CHAN_CFG_SUCCESS == cmd_ptr->cmd.chan_config_cnf.chan_cfg_status ) 
          {
            /* Check with CM if there is a MO call on the other SUB. if yes, abort the connection
               and move to idle */
            if((rrcrce_send_abort_cnf) || rrcrce_abort_ind_sent
#ifdef FEATURE_DUAL_SIM
               || (wcdma_is_ue_in_dual_standby &&(TRUE == cm_call_is_there_call_on_other_sub(rrc_get_as_id(),CM_CALL_TYPE_VOICE_MASK | CM_CALL_TYPE_EMERG_MASK )))
#endif
             )
            {
              /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
               rrcrce_send_rrc_initiate_cell_selection_ind(
               RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
               /* Wait for successful release of RB0 */
               rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;
            }    
            else
            {
              /* Sends the RRC Connection Request message to Send Chain  and This
              function also starts the timer T300 */
              rrcrce_send_rrc_connection_request_msg();
              /* The RCE procedure waits for the RRC Connection Setup Message */
              rrcrce_substate = RRCRCE_WAIT_FOR_RRC_CONN_SETUP;

              /* Increments the re-transmission counter for RRC Connection
                 Request Message */
              WRRC_MSG1_HIGH("RRC Connection Request is sent: %d",rrc_v_300);

              rrc_v_300++;  
            }
          }
          else /* Channel Config for RB0 failure */
          {
            event_report(EVENT_WCDMA_RRC_RB0_SETUP_FAILURE);

            WRRC_MSG1_HIGH("Channel Config for RB0 failure. rrcrce_substate %d",rrcrce_substate);

            if(rrccho_is_gtow_pcco_in_progress())
            {
              rrc_conn_setup_ind_type conn_setup_ind_params;
              conn_setup_ind_params.status = FAILURE;
              conn_setup_ind_params.conn_failure_reason = RRC_CONN_FAILURE_OTHER;
              
              WRRC_MSG1_HIGH("Channel Config for RB0 failure when PCCO active.rrcrce_substate %d",rrcrce_substate);
              rrcrce_send_conn_setup_ind(conn_setup_ind_params);                  
                  
              /* set rce_substate to initial */
              rrcrce_substate = RRCRCE_INITIAL;
              break;
            }

            if(rrcrce_send_abort_cnf)
            {
              rrcrce_send_rrc_abort_cnf();
                /* Clear the ESTABLISHED_RABS while bringing down RRC Connection*/
                rrc_clear_established_rabs();

                /* Reset Signalling Connections status */
                rrcscr_reset_signalling_connection_status();

            }
            else if(!rrcrce_abort_ind_sent)
            {
              /* Send RRC_ABORT_IND command to the MM */
              rrcrce_send_rrc_abort_ind(RRC_INTERNAL_ERROR);   
            }    
            rrcrce_substate = RRCRCE_INITIAL;
            rrcrce_abort_ind_sent = FALSE;
            /* Trigger Cell Selection */
            rrcrce_send_rrc_initiate_cell_selection_ind(
            RRC_CELL_SEL_RB0_SETUP_FAILURE, RRC_STATE_DISCONNECTED);
          }
        }
        else
        {
          WRRC_MSG2_HIGH("Ignoring tr_id %d in chl config cnf. Expected tr_id = %d", 
                     cmd_ptr->cmd.chan_config_cnf.transaction_id,
                      rce_chl_config_transaction_id);
        }
      }
      else
      {
        WRRC_MSG1_HIGH("RRC_CHANNEL_CONFIG_CNF's proc :rrc_proc_e_type_value%d",
                  cmd_ptr->cmd.chan_config_cnf.procedure);
      }

      break;

    default:     
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcrce_substate);
      break;
   }  

} /* rrcrce_process_rce_wait_for_rb0_setup_cnf_substate  */

/*===========================================================================

FUNCTION rrcrce_compare_arrays

DESCRIPTION
  This function checks whether two arrays of specified length are equal or not

DEPENDENCIES
  This function converts type of arrays into charecter arrays before comparing.
  Hence length should be multiplied suitably if integer or any other types are
  compared.

RETURN VALUE
  TRUE if both are same. otherwise FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean rrcrce_compare_arrays
(
  void *str1,                               /* First array */
  void *str2,                               /* Second array */
  uint32 length                             /* Length of the array */
)
{  
  if(!memcmp((const char *) str1,(const char *) str2,length) )
  { 
    return (TRUE);
  }
  else
  {
    return(FALSE);
  }
} /* rrcrce_compare_arrays */

/*===========================================================================

FUNCTION rrcrce_verify_ue_identity

DESCRIPTION
  This function checks whether UE identity in the downlink CCCH message is 
  same as UE initial identity.

DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type: This returns RRCRCE_SUCCESS if the match occurs. 
  Otherwise it returns RRCRCE_FAILURE_MINOR.

SIDE EFFECTS
  None
===========================================================================*/
rrcrce_status_e_type rrcrce_verify_ue_identity
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to the received 
                                                command */
)
{
  uint32 i;                                  /* Temporary variables for 
                                                indexing */
  uint8 message_choice;                       /* Local variable for message
                                                type */ 
  uint16 initial_ue_id_choice_pos;           /* Local variable to store the
                                                position for Initial UE 
                                                Identity t.*/
  rrc_initial_ue_id_type initial_ue_id;      /* Pointer to internal format 
                                                Initial UE Indentity. */
  rrc_initial_ue_id_choice_e_type initial_ue_id_choice;

  rrcrce_status_e_type status = RRCRCE_FAILURE_MINOR; 
                                             /* The status is initialized to
                                                Failure */ 
  #ifdef TEST_FRAMEWORK
   #error code not present
#endif
  
  if(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type != rrc_DL_CCCH_Message_PDU)
  {
    return (status);
  }
 
  message_choice = 
    rrc_get_dl_ccch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                 cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);

  /* Just the initialize the Initial UE Id Choice Position to an
     invalid position */
  initial_ue_id_choice_pos = 0;

  /* Initialize temporary variable to invalid Initial UE Id Choice */
  i = 0xff;
  
  switch (message_choice)
  {
    case T_rrc_DL_CCCH_MessageType_rrcConnectionSetup:
      /* Calculate Initial UE Identity Choice position */

      /* Check whether Integrity Check info is present */
      if(0x00 == b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                           INTEGRITY_CHECK_INFO_PRESENT_POS, 
                           NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT) )
      {
        /* Check if there are enough bits to peek into the message type */
        if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
            (NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT +
             NUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE + 
             NUM_OF_BITS_FOR_CRITICAL_EXT_CHOICE +
             NUM_OF_BITS_FOR_NON_CRITIAL_EXT_PRESENT +
             NUM_OF_BITS_FOR_RRC_CONN_SETUP_OPTIONAL_FIELDS +
             NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE )
          )
        {
        
          initial_ue_id_choice_pos = 
            INTEGRITY_CHECK_INFO_PRESENT_POS +
            NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT +
            NUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE + 
            NUM_OF_BITS_FOR_CRITICAL_EXT_CHOICE +
            NUM_OF_BITS_FOR_NON_CRITIAL_EXT_PRESENT +
            NUM_OF_BITS_FOR_RRC_CONN_SETUP_OPTIONAL_FIELDS;
        }
        if(initial_ue_id_choice_pos != 0)
        {
        /*Peek into byte stream and get the Initial UE Id t */
          i =
            b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                      initial_ue_id_choice_pos,
                      NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE);
        }

      
      }
      /* The integrity Check Info is present, Skip 36 bits for message type */
      else if( 0x01 == b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                           INTEGRITY_CHECK_INFO_PRESENT_POS, 
                           NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT) )
      {
        /* Check if there are enough bits to peek into the message type */
        if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
            (NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT +
             NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO +
             NUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE + 
             NUM_OF_BITS_FOR_CRITICAL_EXT_CHOICE +
             NUM_OF_BITS_FOR_NON_CRITIAL_EXT_PRESENT +
             NUM_OF_BITS_FOR_RRC_CONN_SETUP_OPTIONAL_FIELDS +
             NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE )
          )
        {
      
          initial_ue_id_choice_pos = 
            INTEGRITY_CHECK_INFO_PRESENT_POS +
            NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO +
            NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT +
            NUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE + 
            NUM_OF_BITS_FOR_CRITICAL_EXT_CHOICE +
            NUM_OF_BITS_FOR_NON_CRITIAL_EXT_PRESENT +
            NUM_OF_BITS_FOR_RRC_CONN_SETUP_OPTIONAL_FIELDS;

        }
        if(initial_ue_id_choice_pos != 0)
        {
        /*Peek into byte stream and get the Initial UE Id t */
          i =
            b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                    initial_ue_id_choice_pos,
                    NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE);
        }

      }
      else
      {
   ///////////// Is this a security vulnerability.
        ERR_FATAL("Invalid Integrity Check Info Present %d", 0,0,0);
      }    
      
      break;
  case T_rrc_DL_CCCH_MessageType_rrcConnectionRelease:

      WRRC_MSG0_HIGH("rrcconnectionrelease rcvd");
      
      break;

    case T_rrc_DL_CCCH_MessageType_rrcConnectionReject:
      /* Calculate Initial UE Identity Choice position */

      /* Check whether Integrity Check info is present */
      if(0x00 == b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                           INTEGRITY_CHECK_INFO_PRESENT_POS, 
                           NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT) )
      {
        /* Check if there are enough bits to peek into the message type */
        if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
            (NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT +
             NUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE + 
             NUM_OF_BITS_FOR_CRITICAL_EXT_CHOICE +
             NUM_OF_BITS_FOR_NON_CRITIAL_EXT_PRESENT +
             NUM_OF_BITS_FOR_RRC_CONN_REJECT_OPTIONAL_FIELDS +
             NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE )
          )
        {
        
          initial_ue_id_choice_pos = 
            INTEGRITY_CHECK_INFO_PRESENT_POS +
            NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT +
            NUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE + 
            NUM_OF_BITS_FOR_CRITICAL_EXT_CHOICE +
            NUM_OF_BITS_FOR_NON_CRITIAL_EXT_PRESENT +
            NUM_OF_BITS_FOR_RRC_CONN_REJECT_OPTIONAL_FIELDS;
        }
        if(initial_ue_id_choice_pos != 0)
        {
        /*Peek into byte stream and get the Initial UE Id t */
          i =
            b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                      initial_ue_id_choice_pos,
                      NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE);
        }

      
      }
      /* The integrity Check Info is present, Skip 36 bits for message type */
      else if( 0x01 == b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                           INTEGRITY_CHECK_INFO_PRESENT_POS, 
                           NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT) )
      {
        /* Check if there are enough bits to peek into the message type */
        if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
            (NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT +
             NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO +
             NUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE + 
             NUM_OF_BITS_FOR_CRITICAL_EXT_CHOICE +
             NUM_OF_BITS_FOR_NON_CRITIAL_EXT_PRESENT +
             NUM_OF_BITS_FOR_RRC_CONN_REJECT_OPTIONAL_FIELDS +
             NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE )
          )
        {
      
          initial_ue_id_choice_pos = 
            INTEGRITY_CHECK_INFO_PRESENT_POS +
            NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO +
            NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT +
            NUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE + 
            NUM_OF_BITS_FOR_CRITICAL_EXT_CHOICE +
            NUM_OF_BITS_FOR_NON_CRITIAL_EXT_PRESENT +
            NUM_OF_BITS_FOR_RRC_CONN_REJECT_OPTIONAL_FIELDS;

        }
        if(initial_ue_id_choice_pos != 0)
        {
        /*Peek into byte stream and get the Initial UE Id t */
          i =
            b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                    initial_ue_id_choice_pos,
                    NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE);
        }

      }
      else
      {
   
        ERR_FATAL("Invalid Integrity Check Info Present %d",0,0,0);
      }    
      
      break;

    default:
      /* This should never happen. Dispatcher should route only RRC 
         Connection Setup  and RRC Connection Reject messages to RRCRCE 
         proecdure */
      /*lint -e793*/
      WRRC_MSG1_ERROR("wrong message routed to RCE Proc: %d",message_choice);
      /*lint +e793*/
      break;

  }
  /*lint -save -e685 */
  if ((rrc_initial_ue_id_choice_e_type)i > RRC_IMEI_UE_ID)  
  {
    WRRC_MSG1_HIGH("Invalid Initial UE Id Choice rcvd: %d", i);
    return(status);
  }
  /*lint -restore */
  else /* Continue with peeking in to Initial UE Id */
  {
    /* Lint e64 */
    initial_ue_id_choice = (rrc_initial_ue_id_choice_e_type)i;

    switch(initial_ue_id_choice)
    {
      case RRC_IMSI_UE_ID:
        /*Peek into byte stream and get the IMSI variable length */
        if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
            (uint32) (initial_ue_id_choice_pos +
             NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE + 
             NUM_OF_BITS_FOR_VARIABLE_IMSI_LENGTH )
          )
        {
          i =
            b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                    (uint16) (initial_ue_id_choice_pos + 
                    NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE),
                    NUM_OF_BITS_FOR_VARIABLE_IMSI_LENGTH);

          if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
              (uint32) (initial_ue_id_choice_pos +
               NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE + 
               NUM_OF_BITS_FOR_DIGIT * 
               (NUM_OF_BITS_FOR_VARIABLE_IMSI_LENGTH + 
               NUM_OF_BITS_FOR_FIXED_IMSI_LENGTH) )
            )
          {
             /* Update IMSI length */
            initial_ue_id.imsi.num_digits = 
              i + NUM_OF_BITS_FOR_FIXED_IMSI_LENGTH;

            if(initial_ue_id.imsi.num_digits > RRC_MAX_GSMMAP_IMSI_DIGITS)
            {
              WRRC_MSG0_ERROR("Invalid number of IMSI digits");
              return status;
            }

            if(initial_ue_id.imsi.num_digits == 
                 rrc_initial_ue_identity.imsi.num_digits)
            {
              for( i = 0; i < initial_ue_id.imsi.num_digits; i++)
              {
                /*Peek into byte stream and get the IMSI values */
                initial_ue_id.imsi.value[i] = 
                  b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                            (uint16) (initial_ue_id_choice_pos + 
                            NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +
                            NUM_OF_BITS_FOR_VARIABLE_IMSI_LENGTH +
                            (NUM_OF_BITS_FOR_DIGIT * i) ),
                            NUM_OF_BITS_FOR_DIGIT );
               
              }
            }
            /* Compare the Initial UE Id with the received one */
            if( (initial_ue_id_choice == rrc_initial_ue_identity_choice) &&
                (initial_ue_id.imsi.num_digits == 
                 rrc_initial_ue_identity.imsi.num_digits) &&
                (rrcrce_compare_arrays(initial_ue_id.imsi.value,
                                  rrc_initial_ue_identity.imsi.value,
                                  (initial_ue_id.imsi.num_digits * 
                                   sizeof(uint8)  ) ) )
               )
            {
              
               status = RRCRCE_SUCCESS;
            }
          }
        }         
        break;

      case RRC_TMSI_LAI_UE_ID:
        if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
              (uint32) (initial_ue_id_choice_pos +
               NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE + 
               NUM_OF_BITS_FOR_TMSI +
               (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) +
               NUM_OF_BITS_FOR_OPTIONAL_MNC)
              )
        {
          /* Peek into byte stream and identify number digits for MNC */
          initial_ue_id.tmsi_lai.lai.plmn.num_mnc_digits = 2 + 
             b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                      (uint16) (initial_ue_id_choice_pos + 
                      NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                      NUM_OF_BITS_FOR_TMSI +
                      (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) ),
                      NUM_OF_BITS_FOR_OPTIONAL_MNC);
          
          if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
              (uint32) (initial_ue_id_choice_pos +
               NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE + 
               NUM_OF_BITS_FOR_TMSI +
               (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) +
               NUM_OF_BITS_FOR_OPTIONAL_MNC +
               (initial_ue_id.tmsi_lai.lai.plmn.num_mnc_digits * NUM_OF_BITS_FOR_DIGIT) +
               RRC_MAX_GSMMAP_LAC_LENGTH * 8)
            ) 
          {
       
            for (i = 0; i < RRC_MAX_GSMMAP_TMSI_LENGTH; i ++)
            {
              /* Peek into byte stream and get the TMSI values */
              initial_ue_id.tmsi_lai.tmsi.value[i] = 
                b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                          (uint16) (initial_ue_id_choice_pos + 
                          NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                          (8 * i) ), 8 );
            }
          
            /* Peek into byte stream and get the MCC values */
            for (i = 0; i < RRC_MAX_MCC_DIGITS; i ++)
            {
              /* Peek into byte stream and get the TMSI values */
              initial_ue_id.tmsi_lai.lai.plmn.mcc[i] = 
                b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                          (uint16) (initial_ue_id_choice_pos + 
                          NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                          NUM_OF_BITS_FOR_TMSI +
                          (NUM_OF_BITS_FOR_DIGIT * i) ),
                          NUM_OF_BITS_FOR_DIGIT);
            }
            
            if(initial_ue_id.tmsi_lai.lai.plmn.num_mnc_digits ==
                 rrc_initial_ue_identity.tmsi_lai.lai.plmn.num_mnc_digits )
            {
              for (i = 0; (i < initial_ue_id.tmsi_lai.lai.plmn.num_mnc_digits) && (i < RRC_MAX_MNC_DIGITS); i++)
              {
                /* Peek into byte stream and get the MNC values */
                initial_ue_id.tmsi_lai.lai.plmn.mnc[i] = 
                  b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                            (uint16) (initial_ue_id_choice_pos + 
                            NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                            NUM_OF_BITS_FOR_TMSI +
                            (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) +
                            NUM_OF_BITS_FOR_OPTIONAL_MNC +
                            (NUM_OF_BITS_FOR_DIGIT * i) ),
                            NUM_OF_BITS_FOR_DIGIT);
              }
            
            
              /* Peek into byte stream and get LAC values */  
              for (i = 0; i < RRC_MAX_GSMMAP_LAC_LENGTH; i++ )
              {
                initial_ue_id.tmsi_lai.lai.lac[i] =
                  b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                        (uint16) (initial_ue_id_choice_pos + 
                        NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                        NUM_OF_BITS_FOR_TMSI +
                        (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) +
                         NUM_OF_BITS_FOR_OPTIONAL_MNC +
                        (NUM_OF_BITS_FOR_DIGIT * 
                         initial_ue_id.tmsi_lai.lai.plmn.num_mnc_digits) + 
                        ( 8 * i) ),8 );
              }
            }
            
            if(rrc_initial_ue_identity.tmsi_lai.lai.plmn.num_mnc_digits > RRC_MAX_MNC_DIGITS)
            {
              WRRC_MSG0_ERROR("Invalid number of MNC digits");
              return status;
            }

            if( (initial_ue_id_choice == rrc_initial_ue_identity_choice) &&
                (initial_ue_id.tmsi_lai.lai.plmn.num_mnc_digits ==
                 rrc_initial_ue_identity.tmsi_lai.lai.plmn.num_mnc_digits ) &&
                rrcrce_compare_arrays(initial_ue_id.tmsi_lai.lai.plmn.mcc,
                                      rrc_initial_ue_identity.tmsi_lai.lai.plmn.mcc,
                                      RRC_MAX_MCC_DIGITS) &&                
                rrcrce_compare_arrays(initial_ue_id.tmsi_lai.lai.plmn.mnc,
                                      rrc_initial_ue_identity.tmsi_lai.lai.plmn.mnc,
                                      rrc_initial_ue_identity.tmsi_lai.lai.plmn.num_mnc_digits) &&
                rrcrce_compare_arrays(initial_ue_id.tmsi_lai.tmsi.value,
                                      rrc_initial_ue_identity.tmsi_lai.tmsi.value,
                                      RRC_MAX_GSMMAP_TMSI_LENGTH) &&
                rrcrce_compare_arrays(initial_ue_id.tmsi_lai.lai.lac,
                                      rrc_initial_ue_identity.tmsi_lai.lai.lac,
                                      RRC_MAX_GSMMAP_LAC_LENGTH)
              )
            {
              
               status = RRCRCE_SUCCESS;
            }
          }
        }       
        break;

      case RRC_PTMSI_RAI_UE_ID:
        if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
              (uint32) (initial_ue_id_choice_pos +
               NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE + 
               NUM_OF_BITS_FOR_PTMSI +
               (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) +
               NUM_OF_BITS_FOR_OPTIONAL_MNC)
              )
        {
          /* Peek into byte stream and identify number digits for MNC */
          initial_ue_id.ptmsi_rai.rai.lai.plmn.num_mnc_digits = 2 + 
             b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                      (uint16) (initial_ue_id_choice_pos + 
                      NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                      NUM_OF_BITS_FOR_PTMSI +
                      (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) ),
                      NUM_OF_BITS_FOR_OPTIONAL_MNC);
          
          if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
              (uint32) (initial_ue_id_choice_pos +
               NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE + 
               NUM_OF_BITS_FOR_PTMSI +
               (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) +
               NUM_OF_BITS_FOR_OPTIONAL_MNC +
               (initial_ue_id.ptmsi_rai.rai.lai.plmn.num_mnc_digits * NUM_OF_BITS_FOR_DIGIT) +
               RRC_MAX_GSMMAP_LAC_LENGTH * 8 +
               RRC_MAX_GSMMAP_RAC_LENGTH * 8)
            ) 
          {
       
            for (i = 0; i < RRC_MAX_GSMMAP_PTMSI_LENGTH; i ++)
            {
              /* Peek into byte stream and get the TMSI values */
              initial_ue_id.ptmsi_rai.ptmsi.value[i] = 
                b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                          (uint16) (initial_ue_id_choice_pos + 
                          NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                          (8 * i) ), 8 );
            }
          
            /* Peek into byte stream and get the MCC values */
            for (i = 0; i < RRC_MAX_MCC_DIGITS; i ++)
            {
              /* Peek into byte stream and get the TMSI values */
              initial_ue_id.ptmsi_rai.rai.lai.plmn.mcc[i] = 
                b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                          (uint16) (initial_ue_id_choice_pos + 
                          NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                          NUM_OF_BITS_FOR_PTMSI +
                          (NUM_OF_BITS_FOR_DIGIT * i) ),
                          NUM_OF_BITS_FOR_DIGIT);
            }
            if(initial_ue_id.ptmsi_rai.rai.lai.plmn.num_mnc_digits ==
                 rrc_initial_ue_identity.ptmsi_rai.rai.lai.plmn.num_mnc_digits )
            {
              for (i = 0; (i < initial_ue_id.ptmsi_rai.rai.lai.plmn.num_mnc_digits) && (i < RRC_MAX_MNC_DIGITS); i++)
              {
                /* Peek into byte stream and get the MNC values */
                initial_ue_id.ptmsi_rai.rai.lai.plmn.mnc[i] = 
                  b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                            (uint16) (initial_ue_id_choice_pos + 
                            NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                            NUM_OF_BITS_FOR_PTMSI +
                            (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) +
                            NUM_OF_BITS_FOR_OPTIONAL_MNC +
                            (NUM_OF_BITS_FOR_DIGIT * i) ),
                            NUM_OF_BITS_FOR_DIGIT);
              }
            
             
              /* Peek into byte stream and get LAC values */  
              for (i = 0; i < RRC_MAX_GSMMAP_LAC_LENGTH; i++ )
              {
                initial_ue_id.ptmsi_rai.rai.lai.lac[i] =
                  b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                        (uint16) (initial_ue_id_choice_pos + 
                        NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                        NUM_OF_BITS_FOR_TMSI +
                        (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) +
                         NUM_OF_BITS_FOR_OPTIONAL_MNC +
                        (NUM_OF_BITS_FOR_DIGIT * 
                         initial_ue_id.ptmsi_rai.rai.lai.plmn.num_mnc_digits) + 
                        ( 8 * i) ),8 );
              }
              /* Peek into byte stream and get RAC value */
              for (i = 0; i < RRC_MAX_GSMMAP_RAC_LENGTH; i++)
              {
                initial_ue_id.ptmsi_rai.rai.rac[i] =
                  b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                        (uint16) (initial_ue_id_choice_pos + 
                        NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                        NUM_OF_BITS_FOR_TMSI +
                        (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) +
                         NUM_OF_BITS_FOR_OPTIONAL_MNC +
                        (NUM_OF_BITS_FOR_DIGIT * 
                         initial_ue_id.ptmsi_rai.rai.lai.plmn.num_mnc_digits) + 
                        ( 8 * RRC_MAX_GSMMAP_LAC_LENGTH) + (8 * i) ),8 );
              }
            }

            if(rrc_initial_ue_identity.ptmsi_rai.rai.lai.plmn.num_mnc_digits > RRC_MAX_MNC_DIGITS)
            {
              WRRC_MSG0_ERROR("Invalid number of MNC digits");
              return status;
            }

            if( (initial_ue_id_choice == rrc_initial_ue_identity_choice) &&
                (initial_ue_id.ptmsi_rai.rai.lai.plmn.num_mnc_digits ==
                 rrc_initial_ue_identity.ptmsi_rai.rai.lai.plmn.num_mnc_digits ) &&

                rrcrce_compare_arrays(initial_ue_id.ptmsi_rai.rai.lai.plmn.mcc,
                                rrc_initial_ue_identity.ptmsi_rai.rai.lai.plmn.mcc,
                                RRC_MAX_MCC_DIGITS)  &&
                rrcrce_compare_arrays(initial_ue_id.ptmsi_rai.rai.lai.plmn.mnc,
                                rrc_initial_ue_identity.ptmsi_rai.rai.lai.plmn.mnc,
                                rrc_initial_ue_identity.ptmsi_rai.rai.lai.plmn.
                                num_mnc_digits)  &&
                rrcrce_compare_arrays(initial_ue_id.ptmsi_rai.ptmsi.value,
                                      rrc_initial_ue_identity.ptmsi_rai.ptmsi.value,
                                      RRC_MAX_GSMMAP_PTMSI_LENGTH) &&
                rrcrce_compare_arrays(initial_ue_id.ptmsi_rai.rai.lai.lac,
                                      rrc_initial_ue_identity.ptmsi_rai.rai.lai.lac,
                                      RRC_MAX_GSMMAP_LAC_LENGTH) &&
                rrcrce_compare_arrays(initial_ue_id.ptmsi_rai.rai.rac,
                                      rrc_initial_ue_identity.ptmsi_rai.rai.rac,
                                      RRC_MAX_GSMMAP_RAC_LENGTH) 
              )
            {
              
               status = RRCRCE_SUCCESS;
            }
          }
        }       
        break;

      case RRC_IMEI_UE_ID:        
        if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
            (uint32)(initial_ue_id_choice_pos +
             NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE + 
             (NUM_OF_BITS_FOR_DIGIT * RRC_NUM_IMEI_DIGITS)  )
          )
        {
          for( i = 0; i < RRC_NUM_IMEI_DIGITS; i++)
          {
            /*Peek into byte stream and get the IMEI values */
            initial_ue_id.imei.value[i] = 
              b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                        (uint16) (initial_ue_id_choice_pos + 
                        NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +
                        (NUM_OF_BITS_FOR_DIGIT * i) ),
                        NUM_OF_BITS_FOR_DIGIT );
       
          }
          if( (initial_ue_id_choice == rrc_initial_ue_identity_choice) &&
                (rrcrce_compare_arrays(initial_ue_id.imei.value,
                                  rrc_initial_ue_identity.imei.value,
                                  (RRC_NUM_IMEI_DIGITS *
                                   sizeof(uint8)  )   ) )
               )
            {
              
               status = RRCRCE_SUCCESS;
            }
        }
        break;

      default:
        ERR_FATAL("This should never happen",0,0,0);
        break;
    }
  }
  return(status);
} /* rrcrce_verify_ue_identity */

/*===========================================================================

FUNCTION rrcrce_verify_ue_identity_r5

DESCRIPTION
  This function checks whether UE identity in the downlink CCCH message is 
  same as UE initial identity.

DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type: This returns RRCRCE_SUCCESS if the match occurs. 
  Otherwise it returns RRCRCE_FAILURE_MINOR.

SIDE EFFECTS
  None
===========================================================================*/
rrcrce_status_e_type rrcrce_verify_ue_identity_later_than_r3
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to the received 
                                                command */
)
{
  uint32 i;                                  /* Temporary variables for 
                                                indexing */
  uint8 message_choice;                       /* Local variable for message
                                                type */ 
  uint16 initial_ue_id_choice_pos;           /* Local variable to store the
                                                position for Initial UE 
                                                Identity t.*/
  rrc_initial_ue_id_type initial_ue_id;      /* Pointer to internal format 
                                                Initial UE Indentity. */
  rrc_initial_ue_id_choice_e_type initial_ue_id_choice;

  rrcrce_status_e_type status = RRCRCE_FAILURE_MINOR; 
                                             /* The status is initialized to
                                                Failure */ 
  
  if(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type != rrc_DL_CCCH_Message_PDU)
  {
    return (status);
  }
 
  message_choice = 
    rrc_get_dl_ccch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                 cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);

  /* Just the initialize the Initial UE Id Choice Position to an
     invalid position */
  initial_ue_id_choice_pos = 0;

  /* Initialize temporary variable to invalid Initial UE Id Choice */
  i = 0xff;
  
  switch (message_choice)
  {
    case T_rrc_DL_CCCH_MessageType_rrcConnectionSetup:
      /* Calculate Initial UE Identity Choice position */

      /* Check whether Integrity Check info is present */
      if(0x00 == b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                           INTEGRITY_CHECK_INFO_PRESENT_POS, 
                           NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT) )
      {
        /* Check if there are enough bits to peek into the message type */
        if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
            (NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT +
             NUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE + 
             NUM_OF_BITS_FOR_CRITICAL_EXT_CHOICE +
             NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE )
          )
        {
        
          initial_ue_id_choice_pos = 
            INTEGRITY_CHECK_INFO_PRESENT_POS +
            NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT +
            NUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE + 
            NUM_OF_BITS_FOR_CRITICAL_EXT_CHOICE;
            
        }
        if(initial_ue_id_choice_pos != 0)
        {
        /*Peek into byte stream and get the Initial UE Id t */
          i =
            b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                      initial_ue_id_choice_pos,
                      NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE);
        }

      
      }
      /* The integrity Check Info is present, Skip 36 bits for message type */
      else if( 0x01 == b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                           INTEGRITY_CHECK_INFO_PRESENT_POS, 
                           NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT) )
      {
        /* Check if there are enough bits to peek into the message type */
        if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
            (NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT +
             NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO +
             NUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE + 
             NUM_OF_BITS_FOR_CRITICAL_EXT_CHOICE +
             NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE )
          )
        {
      
          initial_ue_id_choice_pos = 
            INTEGRITY_CHECK_INFO_PRESENT_POS +
            NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO +
            NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT +
            NUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE + 
            NUM_OF_BITS_FOR_CRITICAL_EXT_CHOICE;
            

        }
        if(initial_ue_id_choice_pos != 0)
        {
        /*Peek into byte stream and get the Initial UE Id t */
          i =
            b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                    initial_ue_id_choice_pos,
                    NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE);
        }

      }
      else
      {
   
        ERR_FATAL("Invalid Integrity Check Info Present %d", 0,0,0);
      }    
      
      break;


    case T_rrc_DL_CCCH_MessageType_rrcConnectionRelease:

      WRRC_MSG0_HIGH("rrcconnectionrelease rcvd");
      
      break;

    case T_rrc_DL_CCCH_MessageType_rrcConnectionReject:
      /* Calculate Initial UE Identity Choice position */

      /* Check whether Integrity Check info is present */
      if(0x00 == b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                           INTEGRITY_CHECK_INFO_PRESENT_POS, 
                           NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT) )
      {
        /* Check if there are enough bits to peek into the message type */
        if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
            (NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT +
             NUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE + 
             NUM_OF_BITS_FOR_CRITICAL_EXT_CHOICE +
             NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE )
          )
        {
        
          initial_ue_id_choice_pos = 
            INTEGRITY_CHECK_INFO_PRESENT_POS +
            NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT +
            NUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE + 
            NUM_OF_BITS_FOR_RRC_CONN_REJECT_OPTIONAL_FIELDS;
        }
        if(initial_ue_id_choice_pos != 0)
        {
        /*Peek into byte stream and get the Initial UE Id t */
          i =
            b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                      initial_ue_id_choice_pos,
                      NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE);
        }

      
      }
      /* The integrity Check Info is present, Skip 36 bits for message type */
      else if( 0x01 == b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                           INTEGRITY_CHECK_INFO_PRESENT_POS, 
                           NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT) )
      {
        /* Check if there are enough bits to peek into the message type */
        if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
            (NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT +
             NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO +
             NUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE + 
             NUM_OF_BITS_FOR_CRITICAL_EXT_CHOICE +
             NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE )
          )
        {
      
          initial_ue_id_choice_pos = 
            INTEGRITY_CHECK_INFO_PRESENT_POS +
            NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO +
            NUM_OF_BITS_FOR_INTEGRITY_CHECK_INFO_PRESENT +
            NUM_OF_BITS_FOR_DL_CCCH_MSG_TYPE + 
            NUM_OF_BITS_FOR_RRC_CONN_REJECT_OPTIONAL_FIELDS;

        }
        if(initial_ue_id_choice_pos != 0)
        {
        /*Peek into byte stream and get the Initial UE Id t */
          i =
            b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                    initial_ue_id_choice_pos,
                    NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE);
        }

      }
      else
      {
   
        ERR_FATAL("Invalid Integrity Check Info Present %d",0,0,0);
      }    
      
      break;

    default:
      /* This should never happen. Dispatcher should route only RRC 
         Connection Setup  and RRC Connection Reject messages to RRCRCE 
         proecdure */
      /*lint -e793*/
      WRRC_MSG1_ERROR("wrong message routed to RCE Proc: %d",message_choice);
      /*lint +e793*/
      break;

  }
  /*lint -save -e685 */
  if ((rrc_initial_ue_id_choice_e_type)i > RRC_IMEI_UE_ID)  
  {
    WRRC_MSG1_HIGH("Invalid Initial UE Id Choice rcvd: %d", i);
    return(status);
  }
  /*lint -restore */
  else /* Continue with peeking in to Initial UE Id */
  {
    /* Lint e64 */
    initial_ue_id_choice = (rrc_initial_ue_id_choice_e_type)i;

    WRRC_MSG0_MED("PEEKING VALID");

    switch(initial_ue_id_choice)
    {
      case RRC_IMSI_UE_ID:
        /*Peek into byte stream and get the IMSI variable length */
        if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
            (uint32) (initial_ue_id_choice_pos +
             NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE + 
             NUM_OF_BITS_FOR_VARIABLE_IMSI_LENGTH )
          )
        {
          i =
            b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                    (uint16) (initial_ue_id_choice_pos + 
                    NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE),
                    NUM_OF_BITS_FOR_VARIABLE_IMSI_LENGTH);

          if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
              (uint32) (initial_ue_id_choice_pos +
               NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE + 
               NUM_OF_BITS_FOR_DIGIT * 
               (NUM_OF_BITS_FOR_VARIABLE_IMSI_LENGTH + 
               NUM_OF_BITS_FOR_FIXED_IMSI_LENGTH) )
            )
          {
             /* Update IMSI length */
            initial_ue_id.imsi.num_digits = 
              i + NUM_OF_BITS_FOR_FIXED_IMSI_LENGTH;

            if(initial_ue_id.imsi.num_digits > RRC_MAX_GSMMAP_IMSI_DIGITS)
            {
              WRRC_MSG0_ERROR("Invalid number of IMSI digits");
              return status;
            }

            if(initial_ue_id.imsi.num_digits == 
                  rrc_initial_ue_identity.imsi.num_digits)
            {
               for( i = 0; i < initial_ue_id.imsi.num_digits; i++)
               {
                 /*Peek into byte stream and get the IMSI values */
                 initial_ue_id.imsi.value[i] = 
                   b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                             (uint16) (initial_ue_id_choice_pos + 
                             NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +
                             NUM_OF_BITS_FOR_VARIABLE_IMSI_LENGTH +
                             (NUM_OF_BITS_FOR_DIGIT * i) ),
                             NUM_OF_BITS_FOR_DIGIT );
                
               }
            }
            /* Compare the Initial UE Id with the received one */
            if( (initial_ue_id_choice == rrc_initial_ue_identity_choice) &&
                (initial_ue_id.imsi.num_digits == 
                 rrc_initial_ue_identity.imsi.num_digits) &&
                (rrcrce_compare_arrays(initial_ue_id.imsi.value,
                                  rrc_initial_ue_identity.imsi.value,
                                  (initial_ue_id.imsi.num_digits * 
                                   sizeof(uint8)  ) ) )
               )
            {
              WRRC_MSG0_MED("IMSI MATCHED");
              
               status = RRCRCE_SUCCESS;
            }
          }
        }         
        break;

      case RRC_TMSI_LAI_UE_ID:
        if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
              (uint32) (initial_ue_id_choice_pos +
               NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE + 
               NUM_OF_BITS_FOR_TMSI +
               (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) +
               NUM_OF_BITS_FOR_OPTIONAL_MNC)
              )
        {
          /* Peek into byte stream and identify number digits for MNC */
          initial_ue_id.tmsi_lai.lai.plmn.num_mnc_digits = 2 + 
             b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                      (uint16) (initial_ue_id_choice_pos + 
                      NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                      NUM_OF_BITS_FOR_TMSI +
                      (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) ),
                      NUM_OF_BITS_FOR_OPTIONAL_MNC);
          
          if(initial_ue_id.tmsi_lai.lai.plmn.num_mnc_digits > RRC_MAX_MNC_DIGITS)
          {
            WRRC_MSG0_ERROR("Invalid number of MNC digits");
            return status;
          }

          if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
              (uint32) (initial_ue_id_choice_pos +
               NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE + 
               NUM_OF_BITS_FOR_TMSI +
               (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) +
               NUM_OF_BITS_FOR_OPTIONAL_MNC +
               (initial_ue_id.tmsi_lai.lai.plmn.num_mnc_digits * NUM_OF_BITS_FOR_DIGIT) +
               RRC_MAX_GSMMAP_LAC_LENGTH * 8)
            ) 
          {
       
            for (i = 0; i < RRC_MAX_GSMMAP_TMSI_LENGTH; i ++)
            {
              /* Peek into byte stream and get the TMSI values */
              initial_ue_id.tmsi_lai.tmsi.value[i] = 
                b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                          (uint16) (initial_ue_id_choice_pos + 
                          NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                          (8 * i) ), 8 );
            }
          
            /* Peek into byte stream and get the MCC values */
            for (i = 0; i < RRC_MAX_MCC_DIGITS; i ++)
            {
              /* Peek into byte stream and get the TMSI values */
              initial_ue_id.tmsi_lai.lai.plmn.mcc[i] = 
                b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                          (uint16) (initial_ue_id_choice_pos + 
                          NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                          NUM_OF_BITS_FOR_TMSI +
                          (NUM_OF_BITS_FOR_DIGIT * i) ),
                          NUM_OF_BITS_FOR_DIGIT);
            }
            if(initial_ue_id.tmsi_lai.lai.plmn.num_mnc_digits ==
                 rrc_initial_ue_identity.tmsi_lai.lai.plmn.num_mnc_digits )
            {
              for (i = 0; i < initial_ue_id.tmsi_lai.lai.plmn.num_mnc_digits; i++)
              {
                /* Peek into byte stream and get the MNC values */
                initial_ue_id.tmsi_lai.lai.plmn.mnc[i] = 
                  b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                            (uint16) (initial_ue_id_choice_pos + 
                            NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                            NUM_OF_BITS_FOR_TMSI +
                            (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) +
                            NUM_OF_BITS_FOR_OPTIONAL_MNC +
                            (NUM_OF_BITS_FOR_DIGIT * i) ),
                            NUM_OF_BITS_FOR_DIGIT);
              }
            
             
              /* Peek into byte stream and get LAC values */  
              for (i = 0; i < RRC_MAX_GSMMAP_LAC_LENGTH; i++ )
              {
                initial_ue_id.tmsi_lai.lai.lac[i] =
                  b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                        (uint16) (initial_ue_id_choice_pos + 
                        NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                        NUM_OF_BITS_FOR_TMSI +
                        (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) +
                         NUM_OF_BITS_FOR_OPTIONAL_MNC +
                        (NUM_OF_BITS_FOR_DIGIT * 
                         initial_ue_id.tmsi_lai.lai.plmn.num_mnc_digits) + 
                        ( 8 * i) ),8 );
              }
            }
            
            if(rrc_initial_ue_identity.tmsi_lai.lai.plmn.num_mnc_digits > RRC_MAX_MNC_DIGITS)
            {
              WRRC_MSG0_ERROR("Invalid number of MNC digits");
              return status;
            }

            if( (initial_ue_id_choice == rrc_initial_ue_identity_choice) &&
                (initial_ue_id.tmsi_lai.lai.plmn.num_mnc_digits ==
                 rrc_initial_ue_identity.tmsi_lai.lai.plmn.num_mnc_digits ) &&
                rrcrce_compare_arrays(initial_ue_id.tmsi_lai.lai.plmn.mcc,
                                      rrc_initial_ue_identity.tmsi_lai.lai.plmn.mcc,
                                      RRC_MAX_MCC_DIGITS) &&                
                rrcrce_compare_arrays(initial_ue_id.tmsi_lai.lai.plmn.mnc,
                                      rrc_initial_ue_identity.tmsi_lai.lai.plmn.mnc,
                                      rrc_initial_ue_identity.tmsi_lai.lai.plmn.num_mnc_digits) &&
                rrcrce_compare_arrays(initial_ue_id.tmsi_lai.tmsi.value,
                                      rrc_initial_ue_identity.tmsi_lai.tmsi.value,
                                      RRC_MAX_GSMMAP_TMSI_LENGTH) &&
                rrcrce_compare_arrays(initial_ue_id.tmsi_lai.lai.lac,
                                      rrc_initial_ue_identity.tmsi_lai.lai.lac,
                                      RRC_MAX_GSMMAP_LAC_LENGTH)
              )
            {
              WRRC_MSG0_MED("TMSI MATCHED");
              
               status = RRCRCE_SUCCESS;
            }
          }
        }       
        break;

      case RRC_PTMSI_RAI_UE_ID:
        if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
              (uint32) (initial_ue_id_choice_pos +
               NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE + 
               NUM_OF_BITS_FOR_PTMSI +
               (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) +
               NUM_OF_BITS_FOR_OPTIONAL_MNC)
              )
        {
          /* Peek into byte stream and identify number digits for MNC */
          initial_ue_id.ptmsi_rai.rai.lai.plmn.num_mnc_digits = 2 + 
             b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                      (uint16) (initial_ue_id_choice_pos + 
                      NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                      NUM_OF_BITS_FOR_PTMSI +
                      (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) ),
                      NUM_OF_BITS_FOR_OPTIONAL_MNC);
          
          if(initial_ue_id.ptmsi_rai.rai.lai.plmn.num_mnc_digits > RRC_MAX_MNC_DIGITS)
          {
            WRRC_MSG0_ERROR("Invalid number of MNC digits");
            return status;
          }

          if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
              (uint32) (initial_ue_id_choice_pos +
               NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE + 
               NUM_OF_BITS_FOR_PTMSI +
               (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) +
               NUM_OF_BITS_FOR_OPTIONAL_MNC +
               (initial_ue_id.ptmsi_rai.rai.lai.plmn.num_mnc_digits * NUM_OF_BITS_FOR_DIGIT) +
               RRC_MAX_GSMMAP_LAC_LENGTH * 8 +
               RRC_MAX_GSMMAP_RAC_LENGTH * 8)
            ) 
          {
       
            for (i = 0; i < RRC_MAX_GSMMAP_PTMSI_LENGTH; i ++)
            {
              /* Peek into byte stream and get the TMSI values */
              initial_ue_id.ptmsi_rai.ptmsi.value[i] = 
                b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                          (uint16) (initial_ue_id_choice_pos + 
                          NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                          (8 * i) ), 8 );
            }
          
            /* Peek into byte stream and get the MCC values */
            for (i = 0; i < RRC_MAX_MCC_DIGITS; i ++)
            {
              /* Peek into byte stream and get the TMSI values */
              initial_ue_id.ptmsi_rai.rai.lai.plmn.mcc[i] = 
                b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                          (uint16) (initial_ue_id_choice_pos + 
                          NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                          NUM_OF_BITS_FOR_PTMSI +
                          (NUM_OF_BITS_FOR_DIGIT * i) ),
                          NUM_OF_BITS_FOR_DIGIT);
            }
            if(initial_ue_id.ptmsi_rai.rai.lai.plmn.num_mnc_digits ==
                 rrc_initial_ue_identity.ptmsi_rai.rai.lai.plmn.num_mnc_digits )
            {
              for (i = 0; i < initial_ue_id.ptmsi_rai.rai.lai.plmn.num_mnc_digits; i++)
              {
                /* Peek into byte stream and get the MNC values */
                initial_ue_id.ptmsi_rai.rai.lai.plmn.mnc[i] = 
                  b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                            (uint16) (initial_ue_id_choice_pos + 
                            NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                            NUM_OF_BITS_FOR_PTMSI +
                            (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) +
                            NUM_OF_BITS_FOR_OPTIONAL_MNC +
                            (NUM_OF_BITS_FOR_DIGIT * i) ),
                            NUM_OF_BITS_FOR_DIGIT);
              }
            
              /* Peek into byte stream and get LAC values */  
              for (i = 0; i < RRC_MAX_GSMMAP_LAC_LENGTH; i++ )
              {
                initial_ue_id.ptmsi_rai.rai.lai.lac[i] =
                  b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                        (uint16) (initial_ue_id_choice_pos + 
                        NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                        NUM_OF_BITS_FOR_TMSI +
                        (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) +
                         NUM_OF_BITS_FOR_OPTIONAL_MNC +
                        (NUM_OF_BITS_FOR_DIGIT * 
                         initial_ue_id.ptmsi_rai.rai.lai.plmn.num_mnc_digits) + 
                        ( 8 * i) ),8 );
              }
              /* Peek into byte stream and get RAC value */
              for (i = 0; i < RRC_MAX_GSMMAP_RAC_LENGTH; i++)
              {
                initial_ue_id.ptmsi_rai.rai.rac[i] =
                  b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                        (uint16) (initial_ue_id_choice_pos + 
                        NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +                       
                        NUM_OF_BITS_FOR_TMSI +
                        (NUM_OF_BITS_FOR_DIGIT * RRC_MAX_MCC_DIGITS) +
                         NUM_OF_BITS_FOR_OPTIONAL_MNC +
                        (NUM_OF_BITS_FOR_DIGIT * 
                         initial_ue_id.ptmsi_rai.rai.lai.plmn.num_mnc_digits) + 
                        ( 8 * RRC_MAX_GSMMAP_LAC_LENGTH) + (8 * i) ),8 );
              }
            }
            
            if(rrc_initial_ue_identity.ptmsi_rai.rai.lai.plmn.num_mnc_digits > RRC_MAX_MNC_DIGITS)
            {
              WRRC_MSG0_ERROR("Invalid number of MNC digits");
              return status;
            }

            if( (initial_ue_id_choice == rrc_initial_ue_identity_choice) &&
                (initial_ue_id.ptmsi_rai.rai.lai.plmn.num_mnc_digits ==
                 rrc_initial_ue_identity.ptmsi_rai.rai.lai.plmn.num_mnc_digits ) &&

                rrcrce_compare_arrays(initial_ue_id.ptmsi_rai.rai.lai.plmn.mcc,
                                rrc_initial_ue_identity.ptmsi_rai.rai.lai.plmn.mcc,
                                RRC_MAX_MCC_DIGITS)  &&
                rrcrce_compare_arrays(initial_ue_id.ptmsi_rai.rai.lai.plmn.mnc,
                                rrc_initial_ue_identity.ptmsi_rai.rai.lai.plmn.mnc,
                                rrc_initial_ue_identity.ptmsi_rai.rai.lai.plmn.
                                num_mnc_digits)  &&
                rrcrce_compare_arrays(initial_ue_id.ptmsi_rai.ptmsi.value,
                                      rrc_initial_ue_identity.ptmsi_rai.ptmsi.value,
                                      RRC_MAX_GSMMAP_PTMSI_LENGTH) &&
                rrcrce_compare_arrays(initial_ue_id.ptmsi_rai.rai.lai.lac,
                                      rrc_initial_ue_identity.ptmsi_rai.rai.lai.lac,
                                      RRC_MAX_GSMMAP_LAC_LENGTH) &&
                rrcrce_compare_arrays(initial_ue_id.ptmsi_rai.rai.rac,
                                      rrc_initial_ue_identity.ptmsi_rai.rai.rac,
                                      RRC_MAX_GSMMAP_RAC_LENGTH) 
              )
            {
              WRRC_MSG0_MED("PTMSI MATCHED");
              
               status = RRCRCE_SUCCESS;
            }
          }
        }       
        break;

      case RRC_IMEI_UE_ID:        
        if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length * 8) >= 
            (uint32)(initial_ue_id_choice_pos +
             NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE + 
             (NUM_OF_BITS_FOR_DIGIT * RRC_NUM_IMEI_DIGITS)  )
          )
        {
          for( i = 0; i < RRC_NUM_IMEI_DIGITS; i++)
          {
            /*Peek into byte stream and get the IMEI values */
            initial_ue_id.imei.value[i] = 
              b_unpackb(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                        (uint16) (initial_ue_id_choice_pos + 
                        NUM_OF_BITS_FOR_INITIAL_UE_ID_CHOICE +
                        (NUM_OF_BITS_FOR_DIGIT * i) ),
                        NUM_OF_BITS_FOR_DIGIT );
       
          }
          if( (initial_ue_id_choice == rrc_initial_ue_identity_choice) &&
                (rrcrce_compare_arrays(initial_ue_id.imei.value,
                                  rrc_initial_ue_identity.imei.value,
                                  (RRC_NUM_IMEI_DIGITS *
                                   sizeof(uint8)  )   ) )
               )
            {
               WRRC_MSG0_MED("IMEI MATCHED");
              
               status = RRCRCE_SUCCESS;
            }
        }
        break;

      default:
        ERR_FATAL("This should never happen",0,0,0);
        break;
    }
  }
  if (status != RRCRCE_SUCCESS)
  {
    WRRC_MSG0_HIGH("INITIAL UE ID NOT MATCHED");
  }
  return(status);
} /* rrcrce_verify_ue_identity_later_than_r3 */

/*===========================================================================

FUNCTION rrcrce_send_rrc_connection_request_msg

DESCRIPTION
  This function builds the RRC CONNECTION REQUEST message and calls the Send
  Chain to transmit the same. Even if for some reason connection request is not
  sent, then also T300 will be started so that sending connection request can be 
  retried after its expiry.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_rrc_connection_request_msg
(
  void
)
{
  rrc_UL_CCCH_Message *msg_ptr;             /* Pointer to uplink CCCH message*/

  rlc_lc_id_type ul_lc_id;                  /* Logical Channel Id */
  
 
 /* Checks whether Logical Channel entity setup for CCCH logical
       Channel type */
  ul_lc_id = rrclcm_check_ul_rlc_lc_id ( UE_LOGCHAN_CCCH,
                                         CCCH_RADIO_BEARER_ID,                                           
                                         UE_MODE_TRANSPARENT);
  /* If RLC Logical Channel Id is already setup, the send RRC Connection
     Request message through SEND_CHAIN */
  if(ul_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    /* Allocate a buffer for an uplink CCCH Message. This buffer should be 
       released by rrcsend_send_ul_sdu() function using OSS compiler functions */

    msg_ptr = (rrc_UL_CCCH_Message *) rrc_malloc ( 
                                      sizeof(struct rrc_UL_CCCH_Message));
      rrcrce_build_rrc_connection_request_msg(msg_ptr); 
      /* This places SDU into RLC watermark queue */
      if(RRCSEND_SUCCESS != 
         rrcsend_send_ul_sdu(RRC_PROCEDURE_RCE,msg_ptr,ul_lc_id,
         RRCSEND_L2ACK_NOT_REQUIRED) )
      {
        WRRC_MSG0_ERROR("Failed to transmit RRC Conn Req Msg, Wait for T300 to expire before retransmitting again.");
      }
    }
    else
    {
    WRRC_MSG0_ERROR("Can't find RLC ID for UL RB 0, Wait for T300 to expire before retransmitting again.");
  } 

} /* rrcrce_send_rrc_connection_request_msg */

/*===========================================================================

FUNCTION rrcrce_process_default_config_info

DESCRIPTION
  This function proecess default configuration info specified in
  RRC Connection setup msg
DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type : It returns RRCRCE_SUCCESS if default config id 
  1 or 2 is indicated by NW and next state is DCH. 

SIDE EFFECTS
  None
===========================================================================*/
static rrcrce_status_e_type rrcrce_process_default_config_info (uint32 config_id_received,
                                                         rrc_RRC_StateIndicator next_rrc_state)
{

  rrcrce_status_e_type status = RRCRCE_FAILURE_MINOR; 
  
  rrcrce_default_config_info.rbs_ptr = NULL;

  rrcrce_default_config_info.sib16_ptr = NULL;
  
  if((config_id_received== RRC_DEF_CONFIG_0) ||
     (config_id_received == RRC_DEF_CONFIG_1) ||
     (config_id_received == RRC_DEF_CONFIG_3) ||
     (config_id_received == RRC_DEF_CONFIG_4) ||
     (config_id_received == RRC_DEF_CONFIG_6) ||
     (config_id_received == RRC_DEF_CONFIG_7) ||
     (config_id_received == RRC_DEF_CONFIG_8) ||
     (config_id_received == RRC_DEF_CONFIG_9) ||
     (config_id_received == RRC_DEF_CONFIG_10)
     || (config_id_received == RRC_DEF_CONFIG_11)
     || (config_id_received == RRC_DEF_CONFIG_12)
#ifdef FEATURE_VOC_AMR_WB     
     ||(config_id_received == RRC_DEF_CONFIG_13)
     ||(config_id_received == RRC_DEF_CONFIG_14)
#endif /*FEATURE_VOC_AMR_WB*/     
     ||(config_id_received == RRC_DEF_CONFIG_15)
     ||(config_id_received == RRC_DEF_CONFIG_16)
     ||(config_id_received == RRC_DEF_CONFIG_18)
     ||(config_id_received == RRC_DEF_CONFIG_19)     
     ||(config_id_received == RRC_DEF_CONFIG_20) 
     ||(config_id_received == RRC_DEF_CONFIG_21) 
     ||(config_id_received == RRC_DEF_CONFIG_22)
     ||(config_id_received == RRC_DEF_CONFIG_17)
     ||(config_id_received == RRC_DEF_CONFIG_23)
#ifdef FEATURE_WCDMA_REL10
     #error code not present
#endif
     )
  { 
    /* Since the Transport channel type is DCH, other cases have to be rejected */
    if(next_rrc_state == rrc_RRC_StateIndicator_cell_DCH)
    {

      if((config_id_received == RRC_DEF_CONFIG_17)
        ||(config_id_received == RRC_DEF_CONFIG_23)
      )
      {
        rrcrce_default_config_info.dcch_msg_ptr = NULL;
        rrcrce_default_config_info.dcch_msg_ptr = 
            (rrc_DL_DCCH_Message *)rrcasn1_decode_pdu( (void *)default_config_info[config_id_received].
                                                   default_config_ptr, rrc_DL_DCCH_Message_PDU,
                                                   default_config_info[config_id_received].default_config_len
                                                   );
        if(rrcrce_default_config_info.dcch_msg_ptr == NULL)
        {  
          ERR_FATAL("No memory reserved for rb_setup_r6_ptr ptr",0,0,0);
        } 
      }
      else
      if((config_id_received == RRC_DEF_CONFIG_11) ||
         (config_id_received == RRC_DEF_CONFIG_12) ||
         (config_id_received == RRC_DEF_CONFIG_13) ||
         (config_id_received == RRC_DEF_CONFIG_14))
      {
        rrcrce_default_config_info.rbs_ptr = 
          (rrc_DL_DCCH_Message *)rrcasn1_decode_pdu((void *)default_config_info[config_id_received].
                                                   default_config_ptr, rrc_DL_DCCH_Message_PDU,
                                                   default_config_info[config_id_received].default_config_len
                                                   );
        if(rrcrce_default_config_info.rbs_ptr == NULL)
        {  
          ERR_FATAL("No memory reserved for RBS ptr",0,0,0);
        } 
      }
      else
      {
        rrcrce_default_config_info.sib16_ptr = 
          (rrc_SysInfoType16 *)rrcasn1_decode_pdu( (void *)default_config_info[config_id_received].
                                                   default_config_ptr, rrc_SysInfoType16_PDU,
                                                   default_config_info[config_id_received].default_config_len
                                                   );
  
        if(rrcrce_default_config_info.sib16_ptr == NULL)
        {  
          ERR_FATAL("No memory reserved for SIB16 ptr",0,0,0);
        } 
      }
      rrcrce_default_config_info.config_type = RRC_DEFAULT_CONFIG;
      rrcrce_default_config_info.def_config_ptr_present = TRUE;
      rrcrce_default_config_info.default_config_id = config_id_received;
      WRRC_MSG1_HIGH("Default configuration ID: %d received",config_id_received);
      status = RRCRCE_SUCCESS;
    } 
    else
    {
      WRRC_MSG1_ERROR("RCS: Default config - %d with non-DCH target state",config_id_received);
    }
  }
  else
  { 
    /* Ideally we shouldn't be here - but being too cautious */
    WRRC_MSG1_ERROR("Invalid default configuration ID: %d received",config_id_received); 
  }

  return status;
}


/*===========================================================================

FUNCTION rrcrce_validate_rrc_connection_setup_msg

DESCRIPTION
  This function validates whether RRC CONNECTION SETUP message contain min.
  RB info to setup RRC Connection.

DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type : It returns RRCRCE_SUCCESS if it contains RB 1,RB 2,
  RB 3 and optionally RB 4 info. Otherwise it returns RRCRCE_FAILURE_MINOR.

SIDE EFFECTS
  None
===========================================================================*/
rrcrce_status_e_type rrcrce_validate_rrc_connection_setup_msg
(
  rrc_DL_CCCH_Message *msg_ptr,              /* Pointer to Downlink CCCH Msg */
  uint32 dl_sdu_type,                         /* Downlink SDU type */
  rrc_msg_ver_enum_type* msg_version_ptr
)
{

  rrc_RRCConnectionSetup_r5_IEs* rrc_conn_ptr = NULL;

  rrc_RRCConnectionSetup_r6_IEs* rrc_conn_ptr_r6 = NULL;
  rrc_SRB_InformationSetupList2_r6 * rrcrce_srb_information_setup_r6_list_ptr= NULL;
  /*ptr to rb setup r6     */
  rrc_RadioBearerSetup_r6_IEs * rb_setup_r6_ptr = NULL;
  rrc_RRCConnectionSetup_r7_IEs* rrc_conn_ptr_r7 = NULL;

  rrc_RRCConnectionSetup_r8_IEs* rrc_conn_ptr_r8 = NULL;

#ifdef FEATURE_WCDMA_REL9
  rrc_RRCConnectionSetup_r9_IEs* rrc_conn_ptr_r9 = NULL;
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
  #error code not present
#endif /* FEATURE_WCDMA_REL10 */


  rrc_SRB_InformationSetupList2 * rrcrce_srb_information_setup_list_ptr = NULL;
                                             /* Local pointer to SRB 
                                                Information List*/

  /*Local Pointer to store SRBs for DCs with SRB5*/
  rrc_SRB_InformationSetupList_r5 * rrcrce_srb5_information_setup_list_ptr = NULL;

  uint32 number_of_rbs = 0;                  /* Number SRBs received thru
                                                RRC Connection Setup Msg */
  rrc_state_e_type next_state;               /* Local variable for state */
  rrc_RRC_StateIndicator next_rrc_state ;
  rrc_TX_DiversityMode tx_div_mode = rrc_TX_DiversityMode_noDiversity;
  rrcrce_status_e_type status = RRCRCE_FAILURE_MINOR; 
  boolean dpch_info_present = FALSE;
  /* Set message version to rel 99 by default */
  *msg_version_ptr = MSG_REL99;


  /*First Update msg_version_ptr with correct version of RRC Conn Setup.*/
  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
  {
    if (msg_ptr->message.u.rrcConnectionSetup.t ==
        T_rrc_RRCConnectionSetup_r3)
    {
      WRRC_MSG0_MED("RRCHS:rrcconnectionr99 rcvd");
    }
    else if ((msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.t == 
             T_rrc_RRCConnectionSetup_criticalExtensions_5_criticalExtensions) &&
             (msg_ptr->message.u.rrcConnectionSetup.u.
             later_than_r3.criticalExtensions.u.criticalExtensions.t == 
              T_rrc_RRCConnectionSetup_criticalExtensions_4_r5))
    {
      WRRC_MSG0_MED("RRCHS:rrcconnectionr5 rcvd");
      *msg_version_ptr = MSG_REL5;
    }
    else if (RRCRCE_R6_CRITICAL_EXT_PRESENT && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6))
    {
      WRRC_MSG0_MED("RRCEUL:rrcconnectionr6 rcvd");
      *msg_version_ptr = MSG_REL6;
    }
    else if (RRCRCE_R7_CRITICAL_EXT_PRESENT && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7))
    {
      WRRC_MSG0_MED("RRCEUL:rrcconnectionr7 rcvd");
      *msg_version_ptr = MSG_REL7;
    }
    else if (RRCRCE_R8_CRITICAL_EXT_PRESENT && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8))
    {
      WRRC_MSG0_MED("REL8:RRC Conn Setup is rcvd");
      *msg_version_ptr = MSG_REL8;
    }
#ifdef FEATURE_WCDMA_REL9
    else if (RRCRCE_R9_CRITICAL_EXT_PRESENT && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9))
    {
      WRRC_MSG0_MED("REL9:RRC Conn Setup is rcvd");
      *msg_version_ptr = MSG_REL9;
    }
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
    #error code not present
#endif /* FEATURE_WCDMA_REL10 */

    else
    {
      rrc_print_supported_asn1_rel_version();
      WRRC_MSG0_ERROR("RRCHS:RRC Connection Critical extensions not supported");
      /* Status is already set to RRCRCE_FAILURE_MINOR */
      return(status); 
    }
  }
  else
  {
    /* Check if critical extensions are present */
    if(msg_ptr->message.u.rrcConnectionSetup.t ==
       T_rrc_RRCConnectionSetup_later_than_r3)
    {
      rrc_print_supported_asn1_rel_version();
      WRRC_MSG0_ERROR("Critical extensions are not supported");
       /* Status is already set to RRCRCE_FAILURE_MINOR */
      return(status); 
    }
  }


  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
  {
    memset((void*)&rrcrce_default_config_info, 0, sizeof(rrcrce_default_config_info));
    rrcrce_default_config_info.config_type = RRC_INVALID_CONFIG;
  }


  switch (*msg_version_ptr) 
  {
    case MSG_REL99:
      /*  Validate Transaction Id */
      if( msg_ptr->message.u.rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.
          rrc_TransactionIdentifier > 3 )
      {
        WRRC_MSG1_HIGH("Invalid Transaction Id is received: %d", 
                 rrcrce_setup_transaction_identifier);
        return(status);
      }    

      if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.r3.rrcConnectionSetup_r3,
          rrc_RRCConnectionSetup_r3_IEs,dl_CommonInformation))
      {
        if (RRC_CHECK_COMMON_MSG_TYPE(msg_ptr->message.u.rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.dl_CommonInformation.modeSpecificInfo,
            rrc_DL_CommonInformation_modeSpecificInfo_fdd))
        {
          if (msg_ptr->message.u.rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.dl_CommonInformation.modeSpecificInfo.u.fdd->m.
              tx_DiversityModePresent)
          {
            tx_div_mode =  msg_ptr->message.u.rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.dl_CommonInformation.modeSpecificInfo.u.fdd->tx_DiversityMode;
            if (tx_div_mode == rrc_TX_DiversityMode_dummy)
            {
              WRRC_MSG1_ERROR("Unsupported Tx Div mode:%d",
                  tx_div_mode);
              return(status);
            }
          }
        }
      }    
      next_rrc_state = msg_ptr->message.u.                   
        rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.rrc_StateIndicator;

      rrcrce_srb_information_setup_list_ptr = (rrc_SRB_InformationSetupList2 * ) &msg_ptr->message.u.                        
        rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.srb_InformationSetupList ;

      dpch_info_present = (((RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.r3.
                             rrcConnectionSetup_r3, 
                             rrc_RRCConnectionSetup_r3_IEs,ul_ChannelRequirement)) && 
                            (RRC_CHECK_COMMON_MSG_TYPE(msg_ptr->message.u.rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.
                             ul_ChannelRequirement,
                             rrc_UL_ChannelRequirement_ul_DPCH_Info))) ||
                           ((RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.r3.
                             rrcConnectionSetup_r3,
                             rrc_RRCConnectionSetup_r3_IEs,dl_InformationPerRL_List)) &&
                            (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.
                             dl_InformationPerRL_List.elem[0],
                             rrc_DL_InformationPerRL,dl_DPCH_InfoPerRL))));
      break;

    case MSG_REL5:
      rrc_conn_ptr = &msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
      criticalExtensions.u.r5.rrcConnectionSetup_r5;
    
      if (msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier > 3)
      {
        WRRC_MSG1_HIGH("Invalid Transaction Id is received: %d", 
                 msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier);
        /* Status is already set to RRCRCE_FAILURE_MINOR */  
        return(status);
      }

      next_rrc_state = rrc_conn_ptr->rrc_StateIndicator;

      /* At present, we support complete & default configuration modes only */
      if (RRC_CHECK_COMMON_MSG_TYPE(rrc_conn_ptr->specificationMode, 
          rrc_RRCConnectionSetup_r5_IEs_specificationMode_complete))
      {
        rrcrce_default_config_info.config_type = RRC_COMPLETE_CONFIG;
        rrcrce_srb_information_setup_list_ptr = (rrc_SRB_InformationSetupList2 *)
        &rrc_conn_ptr->specificationMode.u.complete.srb_InformationSetupList;
      }
      else if (RRC_CHECK_COMMON_MSG_TYPE(rrc_conn_ptr->specificationMode,
               rrc_RRCConnectionSetup_r5_IEs_specificationMode_preconfiguration))
      {
        if(rrc_conn_ptr->specificationMode.u.preconfiguration.preConfigMode.t == 
           T_rrc_RRCConnectionSetup_r5_IEs_preConfigMode_defaultConfig)
        {
          if(rrc_conn_ptr->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigMode == 
             rrc_DefaultConfigMode_fdd)
          {
            uint32 config_id_received = RRC_DEF_CONFIG_INVALID;

            config_id_received = rrc_conn_ptr->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigIdentity;

            if((config_id_received == RRC_DEF_CONFIG_0) ||
               (config_id_received == RRC_DEF_CONFIG_1) ||
               (config_id_received == RRC_DEF_CONFIG_3) ||
               (config_id_received == RRC_DEF_CONFIG_4) ||
               (config_id_received == RRC_DEF_CONFIG_6) ||
               (config_id_received == RRC_DEF_CONFIG_7) ||
               (config_id_received == RRC_DEF_CONFIG_8) ||
               (config_id_received == RRC_DEF_CONFIG_9) ||
               (config_id_received == RRC_DEF_CONFIG_10)
               ||(config_id_received == RRC_DEF_CONFIG_11)
#ifdef FEATURE_VOC_AMR_WB
               ||(config_id_received == RRC_DEF_CONFIG_13)
#endif /*FEATURE_VOC_AMR_WB*/
#ifdef FEATURE_WCDMA_REL10
               #error code not present
#endif 
               )
            { 
              /* Since the Transport channel type is DCH, other cases have to be rejected */
              if(rrc_conn_ptr->rrc_StateIndicator == rrc_RRC_StateIndicator_cell_DCH)
              {
                if((config_id_received == RRC_DEF_CONFIG_11) ||
                  (config_id_received == RRC_DEF_CONFIG_13))
                {
                  rrcrce_default_config_info.rbs_ptr = 
                    (rrc_DL_DCCH_Message *)rrcasn1_decode_pdu( (void *)default_config_info[config_id_received].
                                                   default_config_ptr, rrc_DL_DCCH_Message_PDU,
                                                   default_config_info[config_id_received].default_config_len
                                                   );
                  if(rrcrce_default_config_info.rbs_ptr == NULL)
                  {  
                    ERR_FATAL("No memory reserved for RBS ptr",0,0,0);
                  } 
                  rrcrce_srb5_information_setup_list_ptr = (rrc_SRB_InformationSetupList_r5 *)
                    &rrcrce_default_config_info.rbs_ptr->message.u.radioBearerSetup.u.later_than_r3.criticalExtensions.
                    u.criticalExtensions.u.r5.radioBearerSetup_r5.srb_InformationSetupList;
                }
                else
                {
                  rrcrce_default_config_info.sib16_ptr = (rrc_SysInfoType16 *)
                      rrcasn1_decode_pdu( (void *)default_config_info[config_id_received].
                      default_config_ptr, rrc_SysInfoType16_PDU,
                      default_config_info[config_id_received].default_config_len);
  
                  if(rrcrce_default_config_info.sib16_ptr == NULL)
                  {  
                    ERR_FATAL("No memory reserved for SIB16 ptr",0,0,0);
                  } 
                  rrcrce_srb_information_setup_list_ptr = (rrc_SRB_InformationSetupList2 *)
                      &rrcrce_default_config_info.sib16_ptr->preDefinedRadioConfiguration.predefinedRB_Configuration.srb_InformationList;
                }
                rrcrce_default_config_info.config_type = RRC_DEFAULT_CONFIG;
                rrcrce_default_config_info.def_config_ptr_present = TRUE;
                rrcrce_default_config_info.default_config_id = config_id_received;

                WRRC_MSG1_HIGH("Default configuration ID: %d received",config_id_received);
              } 
              else
              {
                WRRC_MSG1_ERROR("RCS: Default config - %d with non-DCH target state",config_id_received);
                return (status);
              }
            }
            else
            { 
              /* Ideally we shouldn't be here - but being too cautious */
              WRRC_MSG1_ERROR("Invalid default configuration ID: %d received",config_id_received);
              return(status); 
            }

          }
          else
          {
            WRRC_MSG0_ERROR("Preconfiguration Mode for TDD received .. not supported");
            return(status); 
          }
        }
        else
        { 
          /* Predefined configuration presently not supported as of now */
          WRRC_MSG0_ERROR("RCS: Predefined configuration Mode not supported");
          return (status);
        }
      }
      else

      {
        WRRC_MSG0_ERROR("RCS: Preconfiguration Mode not supported");
        return (status);
      }

      dpch_info_present = (((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_conn_ptr,
                             rrc_RRCConnectionSetup_r5_IEs,ul_ChannelRequirement)) && 
                            (RRC_CHECK_COMMON_MSG_TYPE(rrc_conn_ptr->ul_ChannelRequirement,
                             rrc_UL_ChannelRequirement_r4_ul_DPCH_Info))) ||
                           ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_conn_ptr,
                             rrc_RRCConnectionSetup_r5_IEs,dl_InformationPerRL_List)) &&
                            (RRC_MSG_COMMON_BITMASK_IE(rrc_conn_ptr->dl_InformationPerRL_List.elem[0],
                             rrc_DL_InformationPerRL_r5bis,dl_DPCH_InfoPerRL))));

      break;
    case MSG_REL6:
      rrc_conn_ptr_r6 = &msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
      criticalExtensions.u.criticalExtensions.u.r6.rrcConnectionSetup_r6;

      if (msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier > 3)
      {
        WRRC_MSG1_HIGH("Invalid Transaction Id is received: %d", 
                 msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier);
        /* Status is already set to RRCRCE_FAILURE_MINOR */  
        return(status);
      }

      next_rrc_state = rrc_conn_ptr_r6->rrc_StateIndicator;

      /* At present, we support complete & default configuration modes only */
      if (RRC_CHECK_COMMON_MSG_TYPE(rrc_conn_ptr_r6->specificationMode,
          rrc_RRCConnectionSetup_r6_IEs_specificationMode_complete))
      {
        rrcrce_default_config_info.config_type = RRC_COMPLETE_CONFIG;
        rrcrce_srb_information_setup_list_ptr = (rrc_SRB_InformationSetupList2 *)
        &rrc_conn_ptr_r6->specificationMode.u.complete.srb_InformationSetupList;
      }
      else if (RRC_CHECK_COMMON_MSG_TYPE(rrc_conn_ptr_r6->specificationMode,
               rrc_RRCConnectionSetup_r6_IEs_specificationMode_preconfiguration))
      {
        if( rrc_conn_ptr_r6->specificationMode.u.preconfiguration.preConfigMode.t == 
           T_rrc_RRCConnectionSetup_r6_IEs_preConfigMode_defaultConfig)
        {
          if(rrc_conn_ptr_r6->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigMode == 
             rrc_DefaultConfigMode_fdd)
          {
            if (RRCRCE_SUCCESS != rrcrce_process_default_config_info( 
              rrc_conn_ptr_r6->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigIdentity,
              next_rrc_state))
            {
              return(status);
            }
         

            if((rrcrce_default_config_info.default_config_id == RRC_DEF_CONFIG_17)
             || (rrcrce_default_config_info.default_config_id == RRC_DEF_CONFIG_23)
            )			
            {
              rb_setup_r6_ptr = rrcrce_get_rb_setup_r6_ptr_for_dc();
              if(rb_setup_r6_ptr != NULL)
              {
                rrcrce_srb_information_setup_r6_list_ptr = (rrc_SRB_InformationSetupList2_r6 *)
                  &rb_setup_r6_ptr->specificationMode.u.complete.srb_InformationSetupList;
              }
              else
              {
                WRRC_MSG1_ERROR("Default config-17 decoded msg ptr NULL, ptr val=%d",rb_setup_r6_ptr);
                return(status);
              }
            }
            else
            {
              uint32 config_id_received = RRC_DEF_CONFIG_INVALID;
  
              config_id_received = rrc_conn_ptr_r6->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigIdentity;
              if((config_id_received == RRC_DEF_CONFIG_11) ||
                 (config_id_received == RRC_DEF_CONFIG_12) ||
                 (config_id_received == RRC_DEF_CONFIG_13) ||
                 (config_id_received == RRC_DEF_CONFIG_14))
              {
                rrcrce_srb5_information_setup_list_ptr = (rrc_SRB_InformationSetupList_r5 *)
                    &rrcrce_default_config_info.rbs_ptr->message.u.radioBearerSetup.u.later_than_r3.criticalExtensions.
                    u.criticalExtensions.u.r5.radioBearerSetup_r5.srb_InformationSetupList;

                if(rrcrce_srb5_information_setup_list_ptr == NULL)
                {
                          WRRC_MSG0_HIGH("DC with SRB5 ptr is NULL");
                          return(status);
                }
              }
              else
              {
                rrcrce_srb_information_setup_list_ptr = (rrc_SRB_InformationSetupList2 *)
                &rrcrce_default_config_info.sib16_ptr->preDefinedRadioConfiguration.predefinedRB_Configuration.srb_InformationList;
              }
            }
          }
          else
          {
            WRRC_MSG0_ERROR("Preconfiguration Mode for TDD received .. not supported");
            return(status); 
          }
        }
        else
        { 
          /* Predefined configuration presently not supported as of now */
          WRRC_MSG0_ERROR("RCS: Predefined configuration Mode not supported");
          return (status);
        }
      }
      else
      {
        WRRC_MSG0_ERROR("RCS: Preconfiguration Mode not supported");
        return (status);
      }

      dpch_info_present = (((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_conn_ptr_r6,
                             rrc_RRCConnectionSetup_r6_IEs,ul_DPCH_Info)) && 
                            (RRC_CHECK_COMMON_MSG_TYPE(rrc_conn_ptr_r6->ul_DPCH_Info.modeSpecificInfo,
                             rrc_UL_DPCH_Info_r6_modeSpecificInfo_fdd))) ||
                           ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_conn_ptr_r6,
                             rrc_RRCConnectionSetup_r6_IEs,dl_InformationPerRL_List)) &&
                            (RRC_MSG_COMMON_BITMASK_IE(rrc_conn_ptr_r6->dl_InformationPerRL_List.elem[0], 
                             rrc_DL_InformationPerRL_r6,dl_dpchInfo))));

      break;
    case MSG_REL7:
      rrc_conn_ptr_r7 = &msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
      criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7.rrcConnectionSetup_r7;

      if (msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier > 3)
      {
        WRRC_MSG1_HIGH("Invalid Transaction Id is received: %d", 
                 msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier);
        /* Status is already set to RRCRCE_FAILURE_MINOR */  
        return(status);
      }

      next_rrc_state = rrc_conn_ptr_r7->rrc_StateIndicator;

  
#ifdef FEATURE_WCDMA_REL7_SPECRS      
      /*In RCE OTA "supportForChangeOfUE_Capability" IE is mandatory. We need to store it in
              "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
              "rrc_rnc_cap_change_support" can be updated accordingly. */     
      if(rrc_conn_ptr_r7->supportForChangeOfUE_Capability == TRUE)
      {
        rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_SUPPORTED;
      }
      else
      {
        rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_UNSUPPORTED;
      }
#endif

      /* At present, we support complete & default configuration modes only */
      if (RRC_CHECK_COMMON_MSG_TYPE(rrc_conn_ptr_r7->specificationMode,
          rrc_RRCConnectionSetup_r7_IEs_specificationMode_complete))
      {
        rrcrce_default_config_info.config_type = RRC_COMPLETE_CONFIG;
        rrcrce_srb_information_setup_list_ptr = (rrc_SRB_InformationSetupList2 *)
        &rrc_conn_ptr_r7->specificationMode.u.complete.srb_InformationSetupList;
      }
      else if (RRC_CHECK_COMMON_MSG_TYPE(rrc_conn_ptr_r7->specificationMode,
               rrc_RRCConnectionSetup_r7_IEs_specificationMode_preconfiguration))
      {
        if(rrc_conn_ptr_r7->specificationMode.u.preconfiguration.preConfigMode.t == 
           T_rrc_RRCConnectionSetup_r7_IEs_preConfigMode_defaultConfig)
        {
          if(rrc_conn_ptr_r7->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigMode == 
             rrc_DefaultConfigMode_fdd)
          {
            if (RRCRCE_SUCCESS != rrcrce_process_default_config_info( 
              rrc_conn_ptr_r7->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigIdentity,
              next_rrc_state))
            {
              return(status);
            }
            if((rrcrce_default_config_info.default_config_id == RRC_DEF_CONFIG_17)
             || (rrcrce_default_config_info.default_config_id == RRC_DEF_CONFIG_23)
            )
            {
              rb_setup_r6_ptr = rrcrce_get_rb_setup_r6_ptr_for_dc();
              if(rb_setup_r6_ptr != NULL)
              {
                rrcrce_srb_information_setup_r6_list_ptr = (rrc_SRB_InformationSetupList2_r6 *)
                  &rb_setup_r6_ptr->specificationMode.u.complete.srb_InformationSetupList;
              }
              else
              {
                WRRC_MSG1_ERROR("Default config-17 decoded msg ptr NULL, ptr val=%d",rb_setup_r6_ptr);
                return(status);
              }
            }
            else
            {
              uint32 config_id_received = RRC_DEF_CONFIG_INVALID;
  
              config_id_received = rrc_conn_ptr_r7->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigIdentity;
              if((config_id_received == RRC_DEF_CONFIG_11) ||
                 (config_id_received == RRC_DEF_CONFIG_12) ||
                 (config_id_received == RRC_DEF_CONFIG_13) ||
                 (config_id_received == RRC_DEF_CONFIG_14))
              {
                rrcrce_srb5_information_setup_list_ptr = (rrc_SRB_InformationSetupList_r5 *)
                    &rrcrce_default_config_info.rbs_ptr->message.u.radioBearerSetup.u.later_than_r3.criticalExtensions.
                    u.criticalExtensions.u.r5.radioBearerSetup_r5.srb_InformationSetupList;

                if(rrcrce_srb5_information_setup_list_ptr == NULL)
                {
                  WRRC_MSG0_HIGH("DC with SRB5 ptr is NULL");
                  return(status);
                }
              }
              else
              {
                /*lint -e740 */
                rrcrce_srb_information_setup_list_ptr = (rrc_SRB_InformationSetupList2 *)&rrcrce_default_config_info.sib16_ptr->preDefinedRadioConfiguration.predefinedRB_Configuration.srb_InformationList;
              }
            }
            /*lint +e740 */
          }
          else
          {
            WRRC_MSG0_ERROR("Preconfiguration Mode for TDD received .. not supported");
            return(status); 
          }
        }
        else
        { 
          /* Predefined configuration presently not supported as of now */
          WRRC_MSG0_ERROR("RCS: Predefined configuration Mode not supported");
          return (status);
        }
      }
      else
      {
        WRRC_MSG0_ERROR("RCS: Preconfiguration Mode not supported");
        return (status);
      }

      dpch_info_present = (((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_conn_ptr_r7, 
                             rrc_RRCConnectionSetup_r7_IEs,ul_DPCH_Info)) && 
                            (RRC_CHECK_COMMON_MSG_TYPE(rrc_conn_ptr_r7->ul_DPCH_Info.modeSpecificInfo,
                             rrc_UL_DPCH_Info_r7_modeSpecificInfo_fdd))) ||
                           ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_conn_ptr_r7,
                             rrc_RRCConnectionSetup_r7_IEs,dl_InformationPerRL_List)) &&
                            (RRC_MSG_COMMON_BITMASK_IE(rrc_conn_ptr_r7->dl_InformationPerRL_List.elem[0],
                             rrc_DL_InformationPerRL_r7,dl_dpchInfo))));

      break;

    case MSG_REL8:
      rrc_conn_ptr_r8 = &msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
      criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.rrcConnectionSetup_r8;

      if (msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier > 3)
      {
        WRRC_MSG1_HIGH("Invalid Transaction Id is received: %d", 
                 msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier);
        /* Status is already set to RRCRCE_FAILURE_MINOR */  
        return(status);
      }

      next_rrc_state = rrc_conn_ptr_r8->rrc_StateIndicator;

  
#ifdef FEATURE_WCDMA_REL7_SPECRS
      /*In RCE OTA "supportForChangeOfUE_Capability" IE is mandatory. We need to store it in
              "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
              "rrc_rnc_cap_change_support" can be updated accordingly. */
      if(rrc_conn_ptr_r8->supportForChangeOfUE_Capability == TRUE)
      {
        rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_SUPPORTED;
      }
      else
      {
        rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_UNSUPPORTED;
      }
#endif /*FEATURE_WCDMA_REL7_SPECRS*/

      /* At present, we support complete & default configuration modes only */
      if (rrc_conn_ptr_r8->specificationMode.t == 
          T_rrc_RRCConnectionSetup_r8_IEs_specificationMode_complete)
      {
        rrcrce_default_config_info.config_type = RRC_COMPLETE_CONFIG;
        /*RRC_GKG: Make sure that we don't use the following SRB ptr for anything else because we are typecasting
        different releases of SRB Lists to R99 SRB List. It's fine so long as we use it for counting.*/
        rrcrce_srb_information_setup_list_ptr = (rrc_SRB_InformationSetupList2 *)
        &rrc_conn_ptr_r8->specificationMode.u.complete.srb_InformationSetupList;
      }
      else if (rrc_conn_ptr_r8->specificationMode.t == 
               T_rrc_RRCConnectionSetup_r8_IEs_specificationMode_preconfiguration)
      {
        if(rrc_conn_ptr_r8->specificationMode.u.preconfiguration.preConfigMode.t == 
           T_rrc_RRCConnectionSetup_r8_IEs_preConfigMode_defaultConfig)
        {
          if(rrc_conn_ptr_r8->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigMode == 
             rrc_DefaultConfigMode_fdd)
          {
            if (RRCRCE_SUCCESS != rrcrce_process_default_config_info( 
              rrc_conn_ptr_r8->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigIdentity,
              next_rrc_state))
            {
              return(status);
            }
            if((rrcrce_default_config_info.default_config_id == RRC_DEF_CONFIG_17)
             || (rrcrce_default_config_info.default_config_id == RRC_DEF_CONFIG_23)
            )							
            {
              rb_setup_r6_ptr = rrcrce_get_rb_setup_r6_ptr_for_dc();
              if(rb_setup_r6_ptr != NULL)
              {
                rrcrce_srb_information_setup_r6_list_ptr = (rrc_SRB_InformationSetupList2_r6 *)
                  &rb_setup_r6_ptr->specificationMode.u.complete.srb_InformationSetupList;
              }
              else
              {
                WRRC_MSG1_ERROR("Default config-17 decoded msg ptr NULL, ptr val=%d",rb_setup_r6_ptr);
                return(status);
              }
            }
            else
            {
              uint32 config_id_received = RRC_DEF_CONFIG_INVALID;
  
              config_id_received = rrc_conn_ptr_r8->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigIdentity;
              if((config_id_received == RRC_DEF_CONFIG_11) ||
                 (config_id_received == RRC_DEF_CONFIG_12) ||
                 (config_id_received == RRC_DEF_CONFIG_13) ||
                 (config_id_received == RRC_DEF_CONFIG_14))
              {
                rrcrce_srb5_information_setup_list_ptr = (rrc_SRB_InformationSetupList_r5 *)
                    &rrcrce_default_config_info.rbs_ptr->message.u.radioBearerSetup.u.later_than_r3.criticalExtensions.
                    u.criticalExtensions.u.r5.radioBearerSetup_r5.srb_InformationSetupList;

                if(rrcrce_srb5_information_setup_list_ptr == NULL)
                {
                  WRRC_MSG0_HIGH("DC with SRB5 ptr is NULL");
                  return(status);
                }
              }
              else
              {
                /*lint -e740 */
                rrcrce_srb_information_setup_list_ptr = (rrc_SRB_InformationSetupList2 *)&rrcrce_default_config_info.sib16_ptr->preDefinedRadioConfiguration.predefinedRB_Configuration.srb_InformationList;
              }
            }
            /*lint +e740 */
          }
          else
          {
            WRRC_MSG0_ERROR("Preconfiguration Mode for TDD received .. not supported");
            return(status); 
          }
        }
        else
        { 
          /* Predefined configuration presently not supported as of now */
          WRRC_MSG0_ERROR("RCS: Predefined configuration Mode not supported");
          return (status);
        }
      }
      else
      {
        WRRC_MSG0_ERROR("RCS: Preconfiguration Mode not supported");
        return (status);
      }

      dpch_info_present = (((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_conn_ptr_r8,
                             rrc_RRCConnectionSetup_r8_IEs,ul_DPCH_Info)) && 
                            (RRC_CHECK_COMMON_MSG_TYPE(rrc_conn_ptr_r8->ul_DPCH_Info.modeSpecificInfo, 
                             rrc_UL_DPCH_Info_r7_modeSpecificInfo_fdd))) ||
                           ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_conn_ptr_r8,
                             rrc_RRCConnectionSetup_r8_IEs,dl_InformationPerRL_List)) &&
                            (RRC_MSG_COMMON_BITMASK_IE(rrc_conn_ptr_r8->dl_InformationPerRL_List.elem[0],
                             rrc_DL_InformationPerRL_r8,dl_dpchInfo))));

      break;

#ifdef FEATURE_WCDMA_REL9
    case MSG_REL9:
      rrc_conn_ptr_r9 = &msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
      criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.rrcConnectionSetup_r9;

      if (msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier > 3)
      {
        WRRC_MSG1_HIGH("Invalid Transaction Id is received: %d", 
                 msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier);
        /* Status is already set to RRCRCE_FAILURE_MINOR */  
        return(status);
      }

      next_rrc_state = rrc_conn_ptr_r9->rrc_StateIndicator;


#ifdef FEATURE_WCDMA_REL7_SPECRS
      /*In RCE OTA "supportForChangeOfUE_Capability" IE is mandatory. We need to store it in
              "rrc_ordered_rnc_cap_change_support" so that later once the procedure is over successfully, global
              "rrc_rnc_cap_change_support" can be updated accordingly. */
      if(rrc_conn_ptr_r9->supportForChangeOfUE_Capability == TRUE)
      {
        rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_SUPPORTED;
      }
      else
      {
        rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_UNSUPPORTED;
      }
#endif /*FEATURE_WCDMA_REL7_SPECRS*/

      /* At present, we support complete & default configuration modes only */
      if (rrc_conn_ptr_r9->specificationMode.t == 
          T_rrc_RRCConnectionSetup_r9_IEs_specificationMode_complete)
      {
        rrcrce_default_config_info.config_type = RRC_COMPLETE_CONFIG;
        /*RRC_GKG: Make sure that we don't use the following SRB ptr for anything else because we are typecasting
        different releases of SRB Lists to R99 SRB List. It's fine so long as we use it for counting.*/
        rrcrce_srb_information_setup_list_ptr = (rrc_SRB_InformationSetupList2 *)
        &rrc_conn_ptr_r9->specificationMode.u.complete.srb_InformationSetupList;
      }
      else if (rrc_conn_ptr_r9->specificationMode.t == 
               T_rrc_RRCConnectionSetup_r9_IEs_specificationMode_preconfiguration)
      {
        if(rrc_conn_ptr_r9->specificationMode.u.preconfiguration.preConfigMode.t == 
           T_rrc_RRCConnectionSetup_r9_IEs_preConfigMode_defaultConfig)
        {
          if(rrc_conn_ptr_r9->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigMode == 
             rrc_DefaultConfigMode_fdd)
          {
            if (RRCRCE_SUCCESS != rrcrce_process_default_config_info( 
              rrc_conn_ptr_r9->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigIdentity,
              next_rrc_state))
            {
              return(status);
            }
            if((rrcrce_default_config_info.default_config_id == RRC_DEF_CONFIG_17)
             || (rrcrce_default_config_info.default_config_id == RRC_DEF_CONFIG_23)
            )							
            {
              rb_setup_r6_ptr = rrcrce_get_rb_setup_r6_ptr_for_dc();
              if(rb_setup_r6_ptr != NULL)
              {
                rrcrce_srb_information_setup_r6_list_ptr = (rrc_SRB_InformationSetupList2_r6 *)
                  &rb_setup_r6_ptr->specificationMode.u.complete.srb_InformationSetupList;
              }
              else
              {
                WRRC_MSG1_ERROR("Default config-17 decoded msg ptr NULL, ptr val=%d",rb_setup_r6_ptr);
                return(status);
              }
            }
            else
            {
              uint32 config_id_received = RRC_DEF_CONFIG_INVALID;
  
              config_id_received = rrc_conn_ptr_r9->specificationMode.u.preconfiguration.preConfigMode.u.defaultConfig.defaultConfigIdentity;
              if((config_id_received == RRC_DEF_CONFIG_11) ||
                 (config_id_received == RRC_DEF_CONFIG_12) ||
                 (config_id_received == RRC_DEF_CONFIG_13) ||
                 (config_id_received == RRC_DEF_CONFIG_14))
              {
                rrcrce_srb5_information_setup_list_ptr = (rrc_SRB_InformationSetupList_r5 *)
                    &rrcrce_default_config_info.rbs_ptr->message.u.radioBearerSetup.u.later_than_r3.criticalExtensions.
                    u.criticalExtensions.u.r5.radioBearerSetup_r5.srb_InformationSetupList;

                if(rrcrce_srb5_information_setup_list_ptr == NULL)
                {
                  WRRC_MSG0_HIGH("DC with SRB5 ptr is NULL");
                  return(status);
                }
              }
              else
              {
                /*lint -e740 */
                rrcrce_srb_information_setup_list_ptr = (rrc_SRB_InformationSetupList2 *)&rrcrce_default_config_info.sib16_ptr->preDefinedRadioConfiguration.predefinedRB_Configuration.srb_InformationList;
              }
            }
            /*lint +e740 */
          }
          else
          {
            WRRC_MSG0_ERROR("Preconfiguration Mode for TDD received .. not supported");
            return(status); 
          }
        }
        else
        { 
          /* Predefined configuration presently not supported as of now */
          WRRC_MSG0_ERROR("RCS: Predefined configuration Mode not supported");
          return (status);
        }
      }
      else
      {
        WRRC_MSG0_ERROR("RCS: Preconfiguration Mode not supported");
        return (status);
      }

      dpch_info_present = (((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_conn_ptr_r9,
                             rrc_RRCConnectionSetup_r9_IEs,ul_DPCH_Info)) && 
                            (RRC_CHECK_COMMON_MSG_TYPE(rrc_conn_ptr_r9->ul_DPCH_Info.modeSpecificInfo, 
                             rrc_UL_DPCH_Info_r7_modeSpecificInfo_fdd))) ||
                           ((RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_conn_ptr_r9,
                             rrc_RRCConnectionSetup_r9_IEs,dl_InformationPerRL_List)) &&
                            (RRC_MSG_COMMON_BITMASK_IE(rrc_conn_ptr_r9->dl_InformationPerRL_List.elem[0],
                             rrc_DL_InformationPerRL_r8,dl_dpchInfo))));

      break;

#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
      #error code not present
#endif /* FEATURE_WCDMA_REL10 */

    default:
      WRRC_MSG0_ERROR("msg version not recognized");
      return status;
  } /* switch*/

  if((rrcrce_default_config_info.config_type == RRC_DEFAULT_CONFIG)&&
    ((rrcrce_default_config_info.default_config_id == RRC_DEF_CONFIG_17)
             || (rrcrce_default_config_info.default_config_id == RRC_DEF_CONFIG_23)
    ))
  {
    if(rrcrce_srb_information_setup_r6_list_ptr == NULL)
    { /*lint -e793 */
      WRRC_MSG0_HIGH("SRB Information Setup List IE not found");
      /*lint +e793*/
      return(status); 
    }
    number_of_rbs=rrcrce_srb_information_setup_r6_list_ptr->n;
     
  }
  else
  {
    if(rrcrce_srb_information_setup_list_ptr == NULL 
       && rrcrce_srb5_information_setup_list_ptr == NULL
    )
    { /*lint -e793 */
      WRRC_MSG0_HIGH("SRB Information Setup List IE not found");
      /*lint +e793*/
      return(status); 
    }
    if(rrcrce_srb_information_setup_list_ptr != NULL)
    {
      number_of_rbs = rrcrce_srb_information_setup_list_ptr->n;
    }
    /* Checks whether sufficient SRB info is present to setup 
       RRC Connection */
    
  }
  
  if((rrcrce_default_config_info.def_config_ptr_present == TRUE) &&
      (rrcrce_default_config_info.rbs_ptr != NULL))
  {
    if(rrcrce_srb5_information_setup_list_ptr != NULL)
    {
      number_of_rbs = rrcrce_srb5_information_setup_list_ptr->n;    
    }
  }

  rrcrce_number_of_rbs = number_of_rbs;

  /* Check whether required number SRB Info is present or not */
  if( (number_of_rbs == 3) ||  (number_of_rbs == 4) || (number_of_rbs == 5))
  {
    /* Check whether State Inidcator matches with DPCH info present */
    switch(next_rrc_state)
    {
      case rrc_RRC_StateIndicator_cell_DCH:
        /* Check whether DPCH info is present in the RRC CONNECTION SETUP
           message */
        if (dpch_info_present)
        {
          next_state = RRC_STATE_CELL_DCH;

          /* Updates the ORDERED_CONFIG with RRC CONNECTION SETUP Message
          parameters */
          if(ORDERED_CONFIG_SET == rrcllc_set_ordered_config(
                                   RRC_PROCEDURE_RCE,
                                   next_state,                                         
                                   dl_sdu_type,
                                   msg_ptr) )
          {
            status = RRCRCE_SUCCESS;  
          }
        }
        break;

      case rrc_RRC_StateIndicator_cell_FACH:    
        /* Future validations can be added here */
        next_state = RRC_STATE_CELL_FACH;

        /* Updates the ORDERED_CONFIG with RRC CONNECTION SETUP Message
        parameters */
        if(ORDERED_CONFIG_SET == rrcllc_set_ordered_config(
                                 RRC_PROCEDURE_RCE,
                                 next_state,                                         
                                 dl_sdu_type,
                                 msg_ptr) )
        { 
          status = RRCRCE_SUCCESS;  
        }
        break;

      case rrc_RRC_StateIndicator_cell_PCH:
      case rrc_RRC_StateIndicator_ura_PCH: 
      default:
        WRRC_MSG1_ERROR("Invalid RRC state Indicator is rcvd: %d",
             next_rrc_state);
        break;
    }
  }  
  else
  {
    WRRC_MSG1_HIGH("No. of SRBs are %d",number_of_rbs);
    return(status);
  }

  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
  {
    if(rrcrce_default_config_info.config_type == RRC_DEFAULT_CONFIG)
    {
      rrcrce_delete_default_config_info();
    }
  }

  return(status);  
 
} /* rrcrce_validate_rrc_connection_setup_msg */

/*===========================================================================

FUNCTION rrcrce_handle_interrat_redirection_info

DESCRIPTION
  This function processes inter-RAT redirection info received
  in RRC Connection Reject message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rrcrce_handle_interrat_redirection_info
(
  rrc_DL_CCCH_Message    *msg_ptr
#ifdef FEATURE_WCDMA_TO_LTE
  , boolean lte_redirection_info
#endif
)
{
  boolean mode_enabled = TRUE;

  if(rrccho_is_gtow_pcco_in_progress())
  {
    rrc_conn_setup_ind_type conn_setup_ind_params;
    conn_setup_ind_params.status = FAILURE;
    conn_setup_ind_params.conn_failure_reason = RRC_CONNECTION_REJECT;

    WRRC_MSG0_HIGH("RRC Connection Reject with Inter-RAT redirection info.PCCO is active");
    rrcrce_send_conn_setup_ind(conn_setup_ind_params);               
    
    rrcrce_substate = RRCRCE_INITIAL;
    return;
  }

#ifdef FEATURE_WCDMA_TO_LTE
  if(lte_redirection_info)
  {
    if(rrcmcm_is_lte_mode_enabled()
#ifdef FEATURE_FEMTO_CSG
           && (rrc_csp_int_data.network_select_mode != SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)
#endif
      )

    {
      rsp_to_rrc_conn_req = RRCRCE_CONNECTION_REJECT;
      rrcrce_send_lte_redirection_req(msg_ptr);
    }
    else
    {
      mode_enabled = FALSE;
    }
  }
  else
#endif
  {
    if(rrcmcm_is_dualmode_enabled()
#ifdef FEATURE_FEMTO_CSG
           && (rrc_csp_int_data.network_select_mode != SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)
#endif
      )
    {
      rsp_to_rrc_conn_req = RRCRCE_CONNECTION_REJECT;
      /* sends the command to Cell Selection Procedure */
      rrcrce_send_redirection_req(msg_ptr, FALSE);    
      WRRC_MSG0_HIGH("iRAT Redirection Req sent to CSP");
      /* Wait for redirect confirm from Cell Selection Procedure */
      rrcrce_substate = RRCRCE_WAIT_FOR_REDIRECT_CNF;   
    }  
    else 
    {                  
      mode_enabled = FALSE;
    }
  }

  if(!mode_enabled)
  {
    /* Start the timer for wait time. The wait time units are Secs */
    WRRC_MSG0_HIGH("iRAT Redirection rcvd when Mode is not enabled. Wt tmr strted");
#ifdef FEATURE_DISCONNECT_ON_UNSUPPORTED_IRAT_REDIRECTION
    if ((rrc_establishment_cause == RRC_EST_EMERGENCY_CALL) &&
         rrccsp_check_gsm_bands_supported())                               
    {
      rrcrce_send_rrc_initiate_cell_selection_ind(
      RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_MAX);
      /* The RCE Procedure ends */
      rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF; 

      /* Set Abort Cause */
      rrcrce_mm_abort_cause = RRC_UNSUPPORTED_REDIRECTION;
    }    
    else
#endif
    {
#ifdef FEATURE_DUAL_SIM
      /* Move to idle only for below EST cause because there is no NAS retry for 
         other EST cause */       
      if(wcdma_is_ue_in_dual_standby && ((rrc_establishment_cause == RRC_EST_REGISTRATION) ||
         (rrc_establishment_cause == RRC_EST_CALL_RE_ESTABLISH) ||
         (rrc_establishment_cause == RRC_EST_INTER_RAT_CELL_RESELECT)))
      {
        /* Release the RB 0 and request the RRC LLC to go to Idle
           disconnected */
        /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
        rrcrce_send_rrc_initiate_cell_selection_ind(
          RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
        /* The RCE Procedure ends */
        rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF; 

        /* Set Abort Cause */
        rrcrce_mm_abort_cause = RRC_ABORT_CONN_REJECT_WAIT_TMR;
      }
      else
#endif
      {
        rrctmr_start_timer(RRCTMR_WAIT_TIME_TIMER, rrcrce_wait_time * 1000);  
        rrcrce_substate = RRCRCE_WAIT_FOR_WAIT_TIMER_TO_EXPIRE;
      }
    }
  }
}

/*===========================================================================

FUNCTION rrcrce_process_rce_wait_for_conn_setup_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in RRCRCE_WAIT_FOR_CONN_SETUP substate of 
  Connecting State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_conn_setup_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to the received 
                                                command */
)
{

  rrc_DL_CCCH_Message *msg_ptr = NULL;       /* Pointer to  downlink CCCH
                                                message */ 
  rrc_N_300 rrcrce_n_300;                    /* Local Varibale to store 
                                                N300 */ 
  rrc_state_e_type next_state = RRC_STATE_MAX; 
                                             /* Local variable to store 
                                                next state */ 

  uint8 message_choice;                      /* Local variable to store message
                                                t */

  rrc_abort_cause_e_type  cause = RRC_MAX_ACCESS_FAILURE;
  rrc_msg_ver_enum_type msg_version = MSG_REL99;

#ifdef FEATURE_QSH_EVENT_METRIC
  rrc_log_rrc_conn_rej_ota conn_rej_params;
  memset(&conn_rej_params,0,sizeof(rrc_log_rrc_conn_rej_ota));
#endif

  /* Initialize the pointers to NULL */
  save_dl_ccch_msg.dl_sdu      = NULL;
  save_dl_ccch_msg.decoded_msg = NULL; 
  
  switch (cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_STATE_CHANGE_IND:
      WRRC_MSG1_HIGH("RRC_STATE_CHANGE_IND received.rrcrce_substate %d",rrcrce_substate);

      if((cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED) &&
         ( rrc_get_state() == RRC_STATE_DISCONNECTED))
      {

        /* Stop the timer T300 */
        rrctmr_stop_timer(RRCTMR_T_300_TIMER);
        if(rrccho_is_gtow_pcco_in_progress())
        {
          rrc_conn_setup_ind_type conn_setup_ind_params;
          conn_setup_ind_params.status = FAILURE;
          conn_setup_ind_params.conn_failure_reason = RRC_CONN_FAILURE_OTHER;

          WRRC_MSG1_HIGH("RRC_STATE_CHANGE_IND received when PCCO is active.rrcrce_substate %d",rrcrce_substate);      
          rrcrce_send_conn_setup_ind(conn_setup_ind_params);                  
       
          /* set rce_substate to initial */
          rrcrce_substate = RRCRCE_INITIAL;
          break;
        }

        /* Send RRC_ABORT_IND command to the MM */
          rrcrce_send_rrc_abort_ind(RRC_FAILURE_UNSPECIFIED);     
          rrcrce_substate = RRCRCE_INITIAL; 
        }    
      break;

    case RRC_ABORT_REQ: 
      /* Stop the timer T300 */
        rrctmr_stop_timer(RRCTMR_T_300_TIMER);
      /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
        rrcrce_send_rrc_initiate_cell_selection_ind(
        RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
        /* Wait for successful release of RB0 */
        rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;  
        rrcrce_send_abort_cnf = TRUE;
      
      break;

    case RRC_CMAC_STATUS_IND:
      {
           
         rrc_T_300 rrcrce_t_300 = rrc_T_300_ms1000;        /* Local parameter to store T300 */
        WRRC_MSG1_HIGH("CMAC Status Ind with status %d", cmd_ptr->cmd.cmac_status_ind);
        /* Update RACH tx stats here */
        rrcrce_cmac_status = cmd_ptr->cmd.cmac_status_ind;

        if(cmd_ptr->cmd.cmac_status_ind == MAX_RACH_CYCLES_REACHED)
        {
          /*MAC has hit MAX RACH transmission cycle, so update RACH tx parameters count here*/
          RRC_DEBUG_INCREMENT_STATS(RACH_RRC_RETRY_FAIL_DUE_TO_CMAC_STATUS_IND);
        }
        /* Value in milliseconds. Default value is 1000. */
        /* Get timer T300 from the SIB 1 database */
        (void) rrcrce_get_t300_from_serv_cell_db(&rrcrce_t_300);
          
        /* start the timer T300 */
        rrctmr_start_timer(RRCTMR_T_300_TIMER,rrcrce_convert_t300_to_ms(rrcrce_t_300)); 
          
      }
      break;
        
    case RRC_T300_EXPIRED_IND:
    case RRC_NEW_CELL_IND: 
      /* Check whether the receievd command is RRC_NEW_CELL_IND */
      if(cmd_ptr->cmd_hdr.cmd_id == RRC_NEW_CELL_IND)
      {
        /* Stop the timer T300 */
        rrctmr_stop_timer(RRCTMR_T_300_TIMER);
      }
      else
      {
        WRRC_MSG0_HIGH("RRC_T300_EXPIRED_IND received");
        if(rrctmr_get_remaining_time(RRCTMR_T_300_TIMER) > 0)
        {
          WRRC_MSG0_HIGH("Curr T300 time remaining > 0 Ignore expiry ind");
          break;
        }
      }

      rrcrce_cell_resel_freq = 0;
      rrcrce_cell_resel_scr = 0;

       /* The UE is already in CONNECTING State. The CCM should have reconfigured
          the  RB 0*/
      /* Gets N300 from the SIB 1 database */
      if( RRCRCE_SUCCESS != 
          rrcrce_get_n300_from_serv_cell_db(&rrcrce_n_300))
      {
        ERR_FATAL("N300 value is invalid", rrcrce_n_300,0,0);
      }
      
      if(((RRCRCE_SUCCESS == rrcrce_validate_cell_access_status(
                      rrc_est_cn_domain_id,rrc_establishment_cause,&cause)) &&
        (rrc_v_300 <= rrcrce_n_300))
        /* In Dual standby mode UE has to reject the connection if there is a call
           on the other SUB and move to idle mode.*/
#ifdef FEATURE_DUAL_SIM
         &&((!wcdma_is_ue_in_dual_standby) || 
             (wcdma_is_ue_in_dual_standby && (FALSE == cm_call_is_there_call_on_other_sub(rrc_get_as_id(),CM_CALL_TYPE_VOICE_MASK | CM_CALL_TYPE_EMERG_MASK))))
#endif
        )
      {        
        /* Resend RRC connection Request Message and this function restarts the
         * T300 Timer */
        rrcrce_send_rrc_connection_request_msg();
        /* RRC Connection Request Message is successfully sent */
        /* Increments the re-transmission counter for RRC Connection Request
           Message */
        WRRC_MSG1_HIGH("RRC Connection Request is sent: %d",rrc_v_300);

        rrc_v_300++;     
          
      }
      else /* Reached maximum allowed re-tries */
      { 
        rrcrce_num_retries_count++;
        WRRC_MSG1_HIGH("rrcrce_num_retries_count = %d",rrcrce_num_retries_count);

        /* If GTOW PCCO is active,sent a conn_setup_ind with failure */
        if(rrccho_is_gtow_pcco_in_progress())
        {
          rrc_conn_setup_ind_type conn_setup_ind_params;
          conn_setup_ind_params.status = FAILURE;
          conn_setup_ind_params.conn_failure_reason = RRC_MAX_RACH_ATTEMPTS;

          WRRC_MSG1_HIGH("Max RACH attempts reached when PCCO active.rrcrce_num_retries_count = %d",rrcrce_num_retries_count);
          rrcrce_send_conn_setup_ind(conn_setup_ind_params);

          /* set rce_substate to initial */
          rrcrce_substate = RRCRCE_INITIAL;
          break;
        }

        /* Release the RB 0 and request the RRC LLC to go to Idle disconnected */
        rrcrce_send_rrc_initiate_cell_selection_ind(
        RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
        /* The RCE Procedure ends */
        rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF; /* change the RCE substate */
        if(cause  ==  RRC_ACCESS_CLASS_NOT_ALLOWED)
        {
            if(rrc_est_cn_domain_id == RRC_CS_DOMAIN_CN_ID)
            {
              rrcrce_mm_abort_cause = RRC_AC_NOT_ALLOWED_CS_DOMAIN;
            }
            else
            {
              rrcrce_mm_abort_cause = RRC_AC_NOT_ALLOWED_PS_DOMAIN;
            }
        }
        else
        {
          rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE;
          rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                  (uint8) RRC_PROCEDURE_RCE,
                                  (uint8) RRCLOG_RachFailure,
                                  (uint8) RRCLOG_PROTOCOL_ERROR_NA
                                );

        }
    }
      break; 
             
    case RRC_DOWNLINK_SDU_IND:

      rrc_protocol_error_reject = FALSE;
            
      /* Assert the received message is of type rrc_DL_CCCH_Message. Also 
         Check whether reselection is in progress */
      if( (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == rrc_DL_CCCH_Message_PDU) &&
          (FALSE == rrc_ccm_is_reselection_in_progress() ) 
        )
      {
        message_choice = 
          rrc_get_dl_ccch_message_type(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);

        if(cmd_ptr->cmd.downlink_sdu_ind.decoded_msg != NULL)
        {
           /* Get the OTA message pointer */
          msg_ptr = (rrc_DL_CCCH_Message *) cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;
        }
        else /* ASN1 decoding failed */
        {
          rrc_protocol_error_reject = TRUE;
        }  

        switch( message_choice )
        {
          
          case T_rrc_DL_CCCH_MessageType_rrcConnectionSetup: /* RRC CONNECTION SETUP message
                                                 is received. */
            WRRC_MSG0_MED("RRC Connection Setup Msg is received");
            /* Reset the rrcrce_num_retries_count */
            rrcrce_num_retries_count =0;

            /* Stop the timer T300 */
            rrctmr_stop_timer(RRCTMR_T_300_TIMER);
            /* Check whether ASN1 decoding as resulted in  PROTOCOL_ERROR_REJECT */
            if(rrc_protocol_error_reject == TRUE)
            {
              rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                      (uint8) RRC_PROCEDURE_RCE,
                                      (uint8) T_rrc_FailureCauseWithProtErr_protocolError,
                                      (uint8) rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError
                                     );

              if( RRCRCE_SUCCESS !=
                  rrcrce_get_n300_from_serv_cell_db(&rrcrce_n_300))
              {
                ERR_FATAL("N300 value is invalid", rrcrce_n_300,0,0);
              }
              if(rrc_v_300 <= rrcrce_n_300)
              {          
                rrc_protocol_error_indicator = rrc_ProtocolErrorIndicator_errorOccurred;
                
                /* Resend RRC Connection Request message and this function
                   restarts the T300 Timer*/
                rrcrce_send_rrc_connection_request_msg();
                /* RRC Connection Request Message is successfully sent */
                /* Increment the re-transmission counter for RRC Connection
                   Request message */
                WRRC_MSG1_HIGH("RRC Connection Request is sent: %d",rrc_v_300);

                rrc_v_300++;
              }
              else       /* Max re-tries exceeded */
              {
                WRRC_MSG1_HIGH("Max retries exceeded.rrcrce_substate %d",rrcrce_substate);

                if(rrccho_is_gtow_pcco_in_progress())
                {
                  rrc_conn_setup_ind_type conn_setup_ind_params;
                  conn_setup_ind_params.status = FAILURE;
                  conn_setup_ind_params.conn_failure_reason = RRC_MAX_RACH_ATTEMPTS;

                  WRRC_MSG1_HIGH("Max RACH attempts reached when PCCO active.rrcrce_num_retries_count = %d",rrcrce_num_retries_count);
                  rrcrce_send_conn_setup_ind(conn_setup_ind_params);
     
                  /* set rce_substate to initial */
                  rrcrce_substate = RRCRCE_INITIAL;
                  break;
                }
                rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                        (uint8) RRC_PROCEDURE_RCE,
                                        (uint8) RRCLOG_RachFailure,
                                        (uint8) RRCLOG_PROTOCOL_ERROR_NA
                                      );       

                /* Release the RB 0 and request the RRC LLC to go to 
                   Idle Disconnected */
                rrcrce_send_rrc_initiate_cell_selection_ind(
                  RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                  /* The RCE Procedure ends */
                rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;  /* Change the RCE substate */

                rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE;
              }  
            } 
            else /* ASN1 successfully decoded the message */
            {

              rrc_RRC_StateIndicator  state_indicator_instructed = rrc_RRC_StateIndicator_ura_PCH; /* Other than FACH */
              rrc_msg_ver_enum_type   msg_release_version = MSG_INVALID;
              uint32                  mode_received = T_rrc_FrequencyInfo_modeSpecificInfo_tdd;
              boolean                 freq_info_present = FALSE;
              rrc_UARFCN              dl_uarfcn = 0;
              boolean                 crnti_present = FALSE;
              boolean                 dl_info_per_rl_list_present = FALSE;
			  
              if(rrc_v_300 != RRC_RACH_TX_STATS_ONE_TIME_RACH)
              {
                /*RACH tx Success full for UE message sent in UL, 
                 *so update RACH tx parameters count here*/
                RRC_DEBUG_INCREMENT_STATS(RACH_SUCCFUL_RRC_RETRY);
              }

              if(rrccho_is_gtow_pcco_in_progress())
              { 
                rrc_conn_setup_ind_type conn_setup_ind_params;
                conn_setup_ind_params.status = SUCCESS;
                conn_setup_ind_params.conn_failure_reason = RRC_FAILURE_NONE;

                WRRC_MSG0_HIGH("Connection Setup received and successfully decoded.Informing CHO proc");
                rrcrce_send_conn_setup_ind(conn_setup_ind_params);                  
              }                  
              /*lint -save -e613 */
              if(msg_ptr->message.u.rrcConnectionSetup.t == T_rrc_RRCConnectionSetup_r3)
              {
                msg_release_version = MSG_REL99;
                state_indicator_instructed = msg_ptr->message.u.rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.
                      rrc_StateIndicator;

                if(state_indicator_instructed == rrc_RRC_StateIndicator_cell_FACH)
                {
                  WRRC_MSG1_HIGH("R99 ConnSetUp for FACH rcvd with freq info. FreqInfoMask = 0x%x", 
                        msg_ptr->message.u.rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.m.frequencyInfoPresent);

                  if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.r3.rrcConnectionSetup_r3,
                       rrc_RRCConnectionSetup_r3_IEs,frequencyInfo))
                  {
                    freq_info_present = TRUE;
                    mode_received = msg_ptr->message.u.rrcConnectionSetup.u.r3.
                                      rrcConnectionSetup_r3.frequencyInfo.modeSpecificInfo.t;
                    dl_uarfcn = msg_ptr->message.u.rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.
                                  frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
                  }
                
                  if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.r3.rrcConnectionSetup_r3,
                     rrc_RRCConnectionSetup_r3_IEs,new_c_RNTI))
                  {
                    crnti_present = TRUE;
                  }

                  if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.r3.rrcConnectionSetup_r3,
                     rrc_RRCConnectionSetup_r3_IEs,dl_InformationPerRL_List))
                  {
                    dl_info_per_rl_list_present = TRUE;
                  }
                }
              } /* end of R99 if */
              else if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
              {
                if((msg_ptr->message.u.rrcConnectionSetup.u.
                       later_than_r3.criticalExtensions.t == 
                       T_rrc_RRCConnectionSetup_criticalExtensions_5_criticalExtensions) &&
                      (msg_ptr->message.u.rrcConnectionSetup.u.
                       later_than_r3.criticalExtensions.u.criticalExtensions.t == 
                       T_rrc_RRCConnectionSetup_criticalExtensions_4_r5))
                {
                  msg_release_version = MSG_REL5;
                  state_indicator_instructed = msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                                               criticalExtensions.u.r5.rrcConnectionSetup_r5.rrc_StateIndicator;

                  if(state_indicator_instructed == rrc_RRC_StateIndicator_cell_FACH)
                  {
                    WRRC_MSG2_HIGH("R5 ConnSetUp for FACH rcvd with freq info. bitmask = 0x%x, FreqInfoMask = 0x%x", 
                           msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                                               criticalExtensions.u.r5.rrcConnectionSetup_r5.m.frequencyInfoPresent,0);

                    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                                               criticalExtensions.u.r5.rrcConnectionSetup_r5,
                                               rrc_RRCConnectionSetup_r5_IEs,frequencyInfo))
                    { 
                      freq_info_present = TRUE;
                      mode_received = msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                                 criticalExtensions.u.r5.rrcConnectionSetup_r5.frequencyInfo.modeSpecificInfo.t;
                      dl_uarfcn = msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                                 criticalExtensions.u.r5.rrcConnectionSetup_r5.frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
                    } 

                    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                                               criticalExtensions.u.r5.rrcConnectionSetup_r5,
                                               rrc_RRCConnectionSetup_r5_IEs,new_c_RNTI))
                    {
                      crnti_present = TRUE;
                    }

                    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                                               criticalExtensions.u.r5.rrcConnectionSetup_r5,
                                               rrc_RRCConnectionSetup_r5_IEs,dl_InformationPerRL_List))
                    {
                      dl_info_per_rl_list_present = TRUE;
                    }
                  }
                } /* end of Rel-5 else if */
              }
              if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6)
              {
                if (RRCRCE_R6_CRITICAL_EXT_PRESENT)
                {
                  msg_release_version = MSG_REL6;
                  state_indicator_instructed = msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                                               criticalExtensions.u.criticalExtensions.u.r6.rrcConnectionSetup_r6.rrc_StateIndicator;

                  if(state_indicator_instructed == rrc_RRC_StateIndicator_cell_FACH)
                  {
                    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                       criticalExtensions.u.criticalExtensions.u.r6.rrcConnectionSetup_r6,
                       rrc_RRCConnectionSetup_r6_IEs,frequencyInfo))
                    { 
                      WRRC_MSG0_HIGH("R6 ConnSetUp for FACH rcvd. Freq info is present"); 
                      freq_info_present = TRUE;
                      mode_received = msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                        criticalExtensions.u.criticalExtensions.u.r6.rrcConnectionSetup_r6.frequencyInfo.modeSpecificInfo.t;
                      dl_uarfcn = msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                        criticalExtensions.u.criticalExtensions.u.r6.rrcConnectionSetup_r6.
                        frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
                    } 

                    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                       criticalExtensions.u.criticalExtensions.u.r6.rrcConnectionSetup_r6,
                       rrc_RRCConnectionSetup_r6_IEs,new_c_RNTI))
                    {
                      crnti_present = TRUE;
                    }

                    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                       criticalExtensions.u.criticalExtensions.u.r6.rrcConnectionSetup_r6,
                       rrc_RRCConnectionSetup_r6_IEs,dl_InformationPerRL_List))
                    {
                      dl_info_per_rl_list_present = TRUE;
                    }
                  }
                } /* end of Rel-6 else if */
              }

              if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
              {
                if (RRCRCE_R7_CRITICAL_EXT_PRESENT)
                {
                  msg_release_version = MSG_REL7;
                  state_indicator_instructed = msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
                                               criticalExtensions.u.criticalExtensions.u.r7.rrcConnectionSetup_r7.rrc_StateIndicator;

                  if(state_indicator_instructed == rrc_RRC_StateIndicator_cell_FACH)
                  {
                    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                       criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7.rrcConnectionSetup_r7,
                       rrc_RRCConnectionSetup_r7_IEs,frequencyInfo))
                    { 
                      WRRC_MSG0_HIGH("R7 ConnSetUp for FACH rcvd. Freq info is present"); 
                      freq_info_present = TRUE;
                      mode_received = msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                        criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7.rrcConnectionSetup_r7.frequencyInfo.modeSpecificInfo.t;
                      dl_uarfcn = msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                        criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7.rrcConnectionSetup_r7.
                        frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
                    } 

                    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                       criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7.rrcConnectionSetup_r7,
                       rrc_RRCConnectionSetup_r7_IEs,new_c_RNTI))
                    {
                      crnti_present = TRUE;
                    }

                    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                       criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7.rrcConnectionSetup_r7,
                       rrc_RRCConnectionSetup_r7_IEs,dl_InformationPerRL_List))
                    {
                      dl_info_per_rl_list_present = TRUE;
                    }
                  }
                } /* end of Rel-6 else if */
              }
              /*RRC_GKG: We can use else-if here. Also we can correct it for R7 and R6 also, though no issues.*/
              if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8)
              {
                if (RRCRCE_R8_CRITICAL_EXT_PRESENT)
                {
                  msg_release_version = MSG_REL8;
                  state_indicator_instructed = msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
                                               criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.rrcConnectionSetup_r8.rrc_StateIndicator;

                  if(state_indicator_instructed == rrc_RRC_StateIndicator_cell_FACH)
                  {
                    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                       criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.rrcConnectionSetup_r8,
                       rrc_RRCConnectionSetup_r8_IEs,frequencyInfo))
                    { 
                      WRRC_MSG0_HIGH("R8 ConnSetUp for FACH rcvd. Freq info is present"); 
                      freq_info_present = TRUE;
                      mode_received = msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
                          criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.rrcConnectionSetup_r8.frequencyInfo.modeSpecificInfo.t;
                      dl_uarfcn = msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
                          criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.rrcConnectionSetup_r8.frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
                    } 

                    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                       criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.rrcConnectionSetup_r8,
                       rrc_RRCConnectionSetup_r8_IEs,new_c_RNTI))
                    {
                      crnti_present = TRUE;
                    }

                    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                       criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.rrcConnectionSetup_r8,
                       rrc_RRCConnectionSetup_r8_IEs,dl_InformationPerRL_List))
                    {
                      dl_info_per_rl_list_present = TRUE;
                    }
                  }
                } /* end of Rel-8*/
              }

#ifdef FEATURE_WCDMA_REL9
              /*RRC_GKG: We can use else-if here. Also we can correct it for R8, R7 and R6 also, though no issues.*/
              if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9)
              {
                if (RRCRCE_R9_CRITICAL_EXT_PRESENT)
                {
                  msg_release_version = MSG_REL9;
                  state_indicator_instructed = msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
                                               criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.rrcConnectionSetup_r9.rrc_StateIndicator;

                  if(state_indicator_instructed == rrc_RRC_StateIndicator_cell_FACH)
                  {
                    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                       criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.rrcConnectionSetup_r9,
                       rrc_RRCConnectionSetup_r9_IEs,frequencyInfo))
                    { 
                      WRRC_MSG0_HIGH("R9 ConnSetUp for FACH rcvd. Freq info is present"); 
                      freq_info_present = TRUE;
                      mode_received = msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
                          criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.rrcConnectionSetup_r9.frequencyInfo.modeSpecificInfo.t;
                      dl_uarfcn = msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
                          criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.rrcConnectionSetup_r9.frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
                    } 

                    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                       criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.rrcConnectionSetup_r9,
                       rrc_RRCConnectionSetup_r9_IEs,new_c_RNTI))
                    {
                      crnti_present = TRUE;
                    }

                    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                       criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.rrcConnectionSetup_r9,
                       rrc_RRCConnectionSetup_r9_IEs,dl_InformationPerRL_List))
                    {
                      dl_info_per_rl_list_present = TRUE;
                    }
                  }
                } /* end of Rel-9*/
              }
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
              #error code not present
#endif /* FEATURE_WCDMA_REL10 */

              
              /* If connection setup is for FACH then check for the frequency info */
              if((state_indicator_instructed == rrc_RRC_StateIndicator_cell_FACH) && ((msg_release_version == MSG_REL99) ||
                 (msg_release_version == MSG_REL5) || (msg_release_version == MSG_REL6)
                 || (msg_release_version == MSG_REL7)
                 || (msg_release_version == MSG_REL8)
#ifdef FEATURE_WCDMA_REL9
                 ||(msg_release_version == MSG_REL9)
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
                #error code not present
#endif /* FEATURE_WCDMA_REL10 */


                ))
              {
                /* Check whether frequency info is present */
                if(freq_info_present && (dl_uarfcn != rrc_ccm_get_curr_camped_freq()))
                {
                  if(RRCRCE_SUCCESS != rrcrce_get_n300_from_serv_cell_db(&rrcrce_n_300))
                  { 
                    ERR_FATAL("N300 value is invalid", rrcrce_n_300,0,0);
                  } 

                  if( rrc_v_300 <= rrcrce_n_300)
                  { 
                    if(mode_received == T_rrc_FrequencyInfo_modeSpecificInfo_fdd)
                    { 
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
                      if(SUCCESS == validate_dl_arfcn(dl_uarfcn, 
                                                      rrc_ccm_get_curr_camped_plmn(), 
                                                      W_UARFCN_NOT_FROM_GSM_NBR_LIST
                                                      , TRUE
                                                      )
                        )
#else
                      if(SUCCESS == validate_dl_arfcn(dl_uarfcn
                                                      , TRUE
                                                     )
                        )
#endif
                      { 
                        rrcrce_wait_time = 1;
                        rsp_to_rrc_conn_req = RRCRCE_CONNECTION_SETUP;

                        /* Check if C-RNTI is present and RL info is present for PSC */
                        if(crnti_present && dl_info_per_rl_list_present)
                        { 
                          /* Save the encoded DL_CCCH message */
                          /* Allocated memory */
                          save_dl_ccch_msg.dl_sdu = rrc_malloc(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);

                          /* Copy the contents received in the allocated memory */
                          WCDMA_MEMCPY(save_dl_ccch_msg.dl_sdu, 
                                       cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length,
                                       cmd_ptr->cmd.downlink_sdu_ind.dl_sdu,
                                       cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length);

                          /* Save the length and type of SDU */
                          save_dl_ccch_msg.dl_sdu_length = 
                                              cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length;

                          save_dl_ccch_msg.dl_sdu_type = 
                                              cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type;

                          /* Send the command to Cell Selection Procedure */
                          rrcrce_send_redirection_req(msg_ptr, TRUE);        
                            WRRC_MSG2_HIGH("ConnSetUp inter-freq redirection from (%d,%d)", 
                                    rrc_ccm_get_curr_camped_freq(),
                                    rrc_ccm_get_curr_camped_psc());

                            if(msg_release_version == MSG_REL99)
                            {
                              MSG_HIGH("new cell info. freq %d, psc %d",
                                        dl_uarfcn, 
                                        msg_ptr->
                                        message.u.rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.
                                        dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->
                                        primaryCPICH_Info.primaryScramblingCode, 0);
                            }
                            else if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
                            {
                              if(msg_release_version == MSG_REL5)
                              {
                                MSG_HIGH("new cell info. freq %d, psc %d",
                                          dl_uarfcn,
                                          msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                                          criticalExtensions.u.r5.rrcConnectionSetup_r5.
                                          dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->
                                          primaryCPICH_Info.primaryScramblingCode, 0);
                              }
                            }
                            else if ((msg_release_version == MSG_REL6) && 
                                     (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6))
                            {

                              MSG_HIGH("new cell info. freq %d, psc %d",
                                       dl_uarfcn,
                                       msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                                       criticalExtensions.u.criticalExtensions.u.r6.rrcConnectionSetup_r6.
                                       dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->
                                       primaryCPICH_Info.primaryScramblingCode, 0);
                              
                            }
                            else if ((msg_release_version == MSG_REL7) && 
                                     (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7))
                            {

                              MSG_HIGH("new cell info. freq %d, psc %d",
                                       dl_uarfcn,
                                       msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                                       criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7.rrcConnectionSetup_r7.
                                       dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->
                                       primaryCPICH_Info.primaryScramblingCode, 0);
                              
                            }
                            else if ((msg_release_version == MSG_REL8) && 
                                     (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8))
                            {

                              MSG_HIGH("new cell info. freq %d, psc %d",
                                       dl_uarfcn,
                                       msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
                                       criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.rrcConnectionSetup_r8.
                                       dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->
                                       primaryCPICH_Info.primaryScramblingCode, 0);

                            }
#ifdef FEATURE_WCDMA_REL9

                            else if ((msg_release_version == MSG_REL9) && 
                                     (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9))
                            {

                              MSG_HIGH("new cell info. freq %d, psc %d",
                                       dl_uarfcn,
                                       msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
                                       criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.rrcConnectionSetup_r9.
                                       dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->
                                       primaryCPICH_Info.primaryScramblingCode, 0);

                            }
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
                          #error code not present
#endif /* FEATURE_WCDMA_REL10 */

                          }
                        /* C-RNTI absent or PSC information not present */
                        else
                        { 
                          /* Sends the command to Cell Selection Procedure */
                          rrcrce_send_redirection_req(msg_ptr, FALSE);     
                           WRRC_MSG1_HIGH("RCE ConnSetUp: Inter-freq redirection req for freq %d sent to CSP", 
                                                dl_uarfcn);
                        }   
                        /* Wait for redirect confirm from Cell Selection Proc */
                        rrcrce_substate = RRCRCE_WAIT_FOR_REDIRECT_CNF;   
                      } 
                      /* Unsupported frequency received. Send another Conn Req */
                      else
                      { 
                        WRRC_MSG1_HIGH("Rcvd invalid Freq %d in redirection. Send RRC Conn Req again",
                                              dl_uarfcn);

                        /* RRC Connection Request will be sent on validation failure
                         * when rrcrce_validate_rrc_connection_setup_msg() function
                         * is called.
                         */
                      } 
                    } 
                    /* Incorrect mode specific info */
                    else
                    { 
                      WRRC_MSG1_HIGH("ConnSetUp iFreq Redirection rcvd with unsupported mode_choice %d. Go to Idle",
                               mode_received);

                      /* Since this redirection is unsupported, abandon the procedure 
                       * Release RB 0 and request RRC LLC to go to Idle disconnected 
                       */
                      /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
                      rrcrce_send_rrc_initiate_cell_selection_ind(
                      RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                      /* The RCE Procedure ends */
                      rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF; 

                      /* Set Abort Cause */
                      rrcrce_mm_abort_cause = RRC_UNSUPPORTED_REDIRECTION;
                      break;
                    } 
                  } 
                  /* rrc_v_300 > rrcrce_n_300 */
                  else
                  {
                    WRRC_MSG2_HIGH("rrc_v_300 %d > rrcrce_n_300 %d. release_rb. Goto idle", rrc_v_300, rrcrce_n_300);
                    /* Release RB 0 and request RRC LLC to go to 
                       Idle Disconnected */
                    rrcrce_send_rrc_initiate_cell_selection_ind(
                      RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                    /* The RCE Procedure ends */
                    rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;  /* Change the RCE substate */

                    rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE;
                    rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                            (uint8) RRC_PROCEDURE_RCE,
                                            (uint8) RRCLOG_RachFailure,
                                            (uint8) RRCLOG_PROTOCOL_ERROR_NA
                                          );       

                  }  
                }
                /* Frequency info absent or same as the camped frequency */
                else
                {
                  WRRC_MSG0_HIGH("ConnSetUp iFreq Redirection for the same freq or absent");
                }
              }/* End of "if" destination state is FACH */
              /*lint -restore */
     

              
              /* Validate the RRC Connection Setup message */
              if((rrcrce_substate == RRCRCE_WAIT_FOR_RRC_CONN_SETUP) 
                  && (RRCRCE_SUCCESS != rrcrce_validate_rrc_connection_setup_msg(msg_ptr,
                                         cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type, &msg_version)))
              {
                WRRC_MSG0_HIGH("protocol error rej has been set to true");

                /* Unsuppored IE is received from the UTRAN */
                rrc_protocol_error_reject = TRUE;

                rrc_protocol_error_indicator = rrc_ProtocolErrorIndicator_errorOccurred;

                /* Send RRC Connection Request again if V300 <= N 300 */
                if( RRCRCE_SUCCESS !=
                  rrcrce_get_n300_from_serv_cell_db(&rrcrce_n_300))
                {
                  ERR_FATAL("N300 value is invalid", rrcrce_n_300,0,0);
                }
                if(rrc_v_300 <= rrcrce_n_300)
                {                       
                  /* Resend RRC Connection Request message and this function
                     restarts the T300 Timer*/
                  rrcrce_send_rrc_connection_request_msg();
                  /* RRC Connection Request Message is successfully sent */
                  /* Increment the re-transmission counter for RRC Connection
                       Request message */
                  WRRC_MSG1_HIGH("RRC Connection Request is sent: %d",rrc_v_300);

                  rrc_v_300++;
                }
                else       /* Max re-tries exceeded */
                {
                  WRRC_MSG1_HIGH("Max retries exceeded.rrcrce_substate %d",rrcrce_substate);

                  /* If GTOW PCCO is active,sent a conn_setup_ind with failure */
                  if(rrccho_is_gtow_pcco_in_progress())
                  {
                    rrc_conn_setup_ind_type conn_setup_ind_params;
                    conn_setup_ind_params.status = FAILURE;
                    conn_setup_ind_params.conn_failure_reason = RRC_MAX_RACH_ATTEMPTS;

                    WRRC_MSG1_HIGH("Max RACH attempts reached when PCCO active.rrcrce_num_retries_count = %d",rrcrce_num_retries_count);
                    rrcrce_send_conn_setup_ind(conn_setup_ind_params);
     
                    /* set rce_substate to initial */
                    rrcrce_substate = RRCRCE_INITIAL;
                    break;
                  }
                  rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                          (uint8) RRC_PROCEDURE_RCE,
                                          (uint8) RRCLOG_RachFailure,
                                          (uint8) RRCLOG_PROTOCOL_ERROR_NA
                                        );       
                  /* Release the RB 0 and request the RRC LLC to go to 
                     Idle Disconnected */
                  /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
                  rrcrce_send_rrc_initiate_cell_selection_ind(
                    RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                  /* The RCE Procedure ends */
                  rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;  /* Change the RCE substate */

                  /* Set Abort Cause */
                  rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE;

                }  /* Max re-tries exceeded */
              }
              /* RRC Connection Setup is successfully validated */
              else if(rrcrce_substate == RRCRCE_WAIT_FOR_RRC_CONN_SETUP)
              {
                WRRC_MSG0_MED("RRC ConnSetUp msg has been validated");

                /* Update the required parameters */
                  
                  /* Initialize the Capability Update Requirement variable */
                rrc_capability_update_requirement.
                  system_specific_cap_update_req_present = FALSE;
                
                rrc_capability_update_requirement.
                  ue_radio_capability_update_requirement = FALSE;

                if (msg_version == MSG_REL99)
                {
                  /* Update RCE Transaction Identifier */
                  /*lint -save -e613 */
                  rrcrce_setup_transaction_identifier = msg_ptr->message.u.
                    rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.
                    rrc_TransactionIdentifier;
                  
                  /* Update RRC State Indicator */

                  rrcrce_state_indicator = msg_ptr->message.u.
                    rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.
                    rrc_StateIndicator;
                  /*lint -restore */

                  /* Save the RRC Capability Update Requirement to build RRC
                     CONNECTION SETUP COMPLETE message */

                  /*lint -save -e613 */
                  rrcuece_copy_capability_update_requirement(msg_ptr->message.u.
                    rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.
                    capabilityUpdateRequirement);
                  /*lint -restore */
                }


                if (msg_version == MSG_REL5)
                {
                  /* Update RCE Transaction Identifier */
                  /*lint -save -e613 */
                  rrcrce_setup_transaction_identifier = msg_ptr->message.u.
                    rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier;
                  
                  /* Update RRC State Indicator */
                  
                  rrcrce_state_indicator = msg_ptr->message.u.
                    rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                    criticalExtensions.u.r5.rrcConnectionSetup_r5.rrc_StateIndicator;
                  
                  /*lint -restore */
                  
                  /* Initialize the Capability Update Requirement variable */
                  
                  /* Save the RRC Capability Update Requirement to build RRC
                  CONNECTION SETUP COMPLETE message */
                  /*lint -save -e613 */
                  rrcuece_copy_capability_update_requirement_later_than_r3(msg_ptr->message.u.
                    rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                    criticalExtensions.u.r5.rrcConnectionSetup_r5.capabilityUpdateRequirement);
                  /*lint -restore */

                }

                if ((msg_version == MSG_REL6) && 
                    (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6))
                {
                  /* Update RCE Transaction Identifier */
                  /*lint -save -e613 */
                  rrcrce_setup_transaction_identifier = msg_ptr->message.u.
                    rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier;
                  
                  /* Update RRC State Indicator */
                  
                  rrcrce_state_indicator = msg_ptr->message.u.
                    rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                    criticalExtensions.u.criticalExtensions.u.r6.rrcConnectionSetup_r6.rrc_StateIndicator;
                  
                  /*lint -restore */
                  
                  /* Save the RRC Capability Update Requirement to build RRC
                  CONNECTION SETUP COMPLETE message */
                  /*lint -save -e613 */
                  rrcuece_copy_capability_update_requirement_later_than_r3(msg_ptr->message.u.
                    rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                    criticalExtensions.u.criticalExtensions.u.r6.rrcConnectionSetup_r6.capabilityUpdateRequirement);
                  /*lint -restore */

                }

                if ((msg_version == MSG_REL7) && 
                    (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7))
                {
                  /* Update RCE Transaction Identifier */
                  /*lint -save -e613 */
                  rrcrce_setup_transaction_identifier = msg_ptr->message.u.
                    rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier;
                  
                  /* Update RRC State Indicator */
                  
                  rrcrce_state_indicator = msg_ptr->message.u.
                    rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                    criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7.rrcConnectionSetup_r7.rrc_StateIndicator;
                  
                  /*lint -restore */
                  
                  /* Save the RRC Capability Update Requirement to build RRC
                  CONNECTION SETUP COMPLETE message */
                  /*lint -save -e613 */
                  rrcuece_copy_capability_update_requirement_r7(&msg_ptr->message.u.
                    rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
                    criticalExtensions.u.criticalExtensions.u.r7.rrcConnectionSetup_r7.capabilityUpdateRequirement);
                  /*lint -restore */

                }


                if ((msg_version == MSG_REL8) && 
                    (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8))
                {
                  /* Update RCE Transaction Identifier */
                  /*lint -save -e613 */
                  rrcrce_setup_transaction_identifier = msg_ptr->message.u.
                    rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier;
                  
                  /* Update RRC State Indicator */
                  
                  rrcrce_state_indicator = msg_ptr->message.u.
                    rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                    criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.rrcConnectionSetup_r8.rrc_StateIndicator;
                  
                  /*lint -restore */
                  
                  /* Save the RRC Capability Update Requirement to build RRC
                  CONNECTION SETUP COMPLETE message */
                  /*lint -save -e613 */
                  rrcuece_copy_capability_update_requirement_r8(&msg_ptr->message.u.rrcConnectionSetup.u.
                  later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
                  criticalExtensions.u.r8.rrcConnectionSetup_r8.capabilityUpdateRequirement);
                  /*lint -restore */
                }

#ifdef FEATURE_WCDMA_REL9
                if ((msg_version == MSG_REL9) && 
                    (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9))
                {
                  /* Update RCE Transaction Identifier */
                  /*lint -save -e613 */
                  rrcrce_setup_transaction_identifier = msg_ptr->message.u.
                    rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier;

                  /* Update RRC State Indicator */

                  rrcrce_state_indicator = msg_ptr->message.u.
                    rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                    criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.rrcConnectionSetup_r9.rrc_StateIndicator;
                  
                  /*lint -restore */

                  /* Save the RRC Capability Update Requirement to build RRC
                  CONNECTION SETUP COMPLETE message */
                  /*lint -save -e613 */
                  rrcuece_copy_capability_update_requirement_r8(&msg_ptr->message.u.rrcConnectionSetup.u.
                  later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.
                  criticalExtensions.u.later_r8_criticalExtensions.u.r9.rrcConnectionSetup_r9.capabilityUpdateRequirement);
                  /*lint -restore */
                }
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
                #error code not present
#endif /* FEATURE_WCDMA_REL10 */


                /* Update the next state */
                if(rrcrce_state_indicator == rrc_RRC_StateIndicator_cell_DCH)
                {
                  next_state = RRC_STATE_CELL_DCH;
                }
                else if(rrcrce_state_indicator == rrc_RRC_StateIndicator_cell_FACH)
                {
                  next_state = RRC_STATE_CELL_FACH;
                } 
                else
                {
                   /* This should never happen since RRC state is already validated */
                   ERR_FATAL("Invalid RRC state Indicator is received: %d",
                              rrcrce_state_indicator,0,0);
                }
                
                rrcrce_send_rrc_channel_config_req_for_srb(ESTABLISH_RB,
                                                           next_state,
                                                           TRUE) ;  
                rrcrce_substate = RRCRCE_WAIT_FOR_SRB_SETUP_CNF;     
              } /* RRC Connection Setup is successfully validated */
              else
              {
                WRRC_MSG1_HIGH("RCE waiting in sstate %d", rrcrce_substate);
              }

            } /* ASN1 decoding is successful */
            break;     
 
          case T_rrc_DL_CCCH_MessageType_rrcConnectionReject: /* RRC CONNECTION REJECT is received */

            WRRC_MSG0_HIGH("RRC Connection Reject Msg is received");
            /* Reset the rrcrce_num_retries_count */
            rrcrce_num_retries_count =0;

            /* Stop the timer T300 */
            rrctmr_stop_timer(RRCTMR_T_300_TIMER);
            if(rrc_protocol_error_reject == TRUE) 
            /* ASN1 decoding resulted in PROTOCOL ERROR REJECT */
            {
              rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                      (uint8)RRC_PROCEDURE_RCE,
                                      (uint8) RRCLOG_RachFailure,
                                      (uint8) rrc_ProtocolErrorCause_asn1_ViolationOrEncodingError
                                    );

              if( RRCRCE_SUCCESS != 
                  rrcrce_get_n300_from_serv_cell_db(&rrcrce_n_300))
              {
                ERR_FATAL("N300 value is invalid", rrcrce_n_300,0,0);
              }               
              if( rrc_v_300 <= rrcrce_n_300)
              {            
                rrc_protocol_error_indicator = rrc_ProtocolErrorIndicator_errorOccurred;
                
                /* Re-send RRC CONNECTION REQUEST message over the air and 
                   this function restarts the Timer T300 */
                rrcrce_send_rrc_connection_request_msg();
                /* RRC Connection Request Message is successfully sent */
                /* Increment the re-transmission counter for RRC Connection
                     Request message */
                WRRC_MSG1_HIGH("RRC Connection Request is sent: %d",rrc_v_300);

                rrc_v_300++;
              } 
              else   /* Reached maximum re-tries */
              { 
                if(rrccho_is_gtow_pcco_in_progress())
                {
                  rrc_conn_setup_ind_type conn_setup_ind_params;
                  conn_setup_ind_params.status = FAILURE;
                  conn_setup_ind_params.conn_failure_reason = RRC_MAX_RACH_ATTEMPTS;

                  WRRC_MSG1_HIGH("Max RACH attempts reached when PCCO active.rrcrce_num_retries_count = %d",rrcrce_num_retries_count);
                  rrcrce_send_conn_setup_ind(conn_setup_ind_params);
     
                  /* set rce_substate to initial */
                  rrcrce_substate = RRCRCE_INITIAL;
                  break;
                }
                rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                        (uint8) RRC_PROCEDURE_RCE,
                                        (uint8) RRCLOG_RachFailure,
                                        (uint8) RRCLOG_PROTOCOL_ERROR_NA
                                      );   

                /* Release the RB 0 and request the RRC LLC to go to Idle
                   disconnected */
                /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
                rrcrce_send_rrc_initiate_cell_selection_ind(
                  RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                /* The RCE Procedure ends */
                rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF; 

                /* Set Abort Cause */
                rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE;
              }    
            }  
            else  /* ASN1 decoding is successful */
            { 
#ifdef FEATURE_QSH_EVENT_METRIC
    		  wrrc_qsh_log_rlf_oos_reject_params(WRRC_CON_REJ, WRRC_METRIC_UPDATE);
#endif
              /* Update Transaction Id for RRC Connection Reject Msg */
              /*lint -save -e613 */
              rrcrce_reject_transaction_identifier = msg_ptr->message.u.
                rrcConnectionReject.u.r3.rrcConnectionReject_r3.
                rrc_TransactionIdentifier;

              WRRC_MSG1_HIGH("Wait Time is %d", msg_ptr->message.u.rrcConnectionReject.u.
                r3.rrcConnectionReject_r3.waitTime);

              /* Update the wait time */
              rrcrce_wait_time = msg_ptr->message.u.rrcConnectionReject.u.r3.rrcConnectionReject_r3.waitTime;

#ifdef FEATURE_QSH_EVENT_METRIC
              conn_rej_params.conn_rej_cause = msg_ptr->message.u.rrcConnectionReject.u.r3.rrcConnectionReject_r3.rejectionCause;
              conn_rej_params.wait_time = rrcrce_wait_time;
              conn_rej_params.redir_info_present = FALSE;

              if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionReject.u.r3.
                  rrcConnectionReject_r3,rrc_RRCConnectionReject_r3_IEs,redirectionInfo))
              {
                conn_rej_params.redir_info_present = TRUE;

                if(msg_ptr->message.u.rrcConnectionReject.u.r3.rrcConnectionReject_r3.redirectionInfo.t == T_rrc_RedirectionInfo_frequencyInfo)
                {
                  conn_rej_params.redir_rat =   RRC_FREQ_REDIRECTION;
                }
                else
                {
                  conn_rej_params.redir_rat = RRC_INTER_SYSTEM_GSM;			
                }
              }
              else if((msg_ptr->message.u.rrcConnectionReject.u.r3.m.laterNonCriticalExtensionsPresent) &&
                      (msg_ptr->message.u.rrcConnectionReject.u.r3.laterNonCriticalExtensions.m.v690NonCriticalExtensionsPresent) &&
                      (msg_ptr->message.u.rrcConnectionReject.u.r3.laterNonCriticalExtensions.v690NonCriticalExtensions.m.v6f0NonCriticalExtensionsPresent) &&
                      (msg_ptr->message.u.rrcConnectionReject.u.r3.laterNonCriticalExtensions.v690NonCriticalExtensions.v6f0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) &&
                      (msg_ptr->message.u.rrcConnectionReject.u.r3.laterNonCriticalExtensions.v690NonCriticalExtensions.v6f0NonCriticalExtensions.
                       v860NonCriticalExtensions.rrcConnectionReject_v860ext.m.redirectionInfoPresent) &&
                      (msg_ptr->message.u.rrcConnectionReject.u.r3.laterNonCriticalExtensions.v690NonCriticalExtensions.v6f0NonCriticalExtensions.
                       v860NonCriticalExtensions.rrcConnectionReject_v860ext.redirectionInfo.t == T_rrc_RedirectionInfo_v860ext_interRATInfo) &&
                      (msg_ptr->message.u.rrcConnectionReject.u.r3.laterNonCriticalExtensions.v690NonCriticalExtensions.v6f0NonCriticalExtensions.
                       v860NonCriticalExtensions.rrcConnectionReject_v860ext.redirectionInfo.u.interRATInfo->t == T_rrc_InterRATInfo_v860ext_eutra)
                     )
              {
                conn_rej_params.redir_info_present = TRUE;			  
                conn_rej_params.redir_rat = RRC_INTER_SYSTEM_LTE;
              }

              rrc_qsh_log_conn_rej_params(&conn_rej_params);
#endif

#ifdef FEATURE_DUAL_SIM
              if(wcdma_is_ue_in_dual_standby && (TRUE  == cm_call_is_there_call_on_other_sub(rrc_get_as_id(),CM_CALL_TYPE_VOICE_MASK | CM_CALL_TYPE_EMERG_MASK)))
              {
                /* Release the RB 0 and request the RRC LLC to go to Idle disconnected */
                rrcrce_send_rrc_initiate_cell_selection_ind(
                  RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                /* The RCE Procedure ends */
                rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;

                /* Set Abort Cause */
                rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE;  
                return;
              }
#endif

              /* Validate Transaction Id and make sure critical externsions
                 are not selected */
              if( (rrcrce_reject_transaction_identifier <= 3 ) &&
                   (msg_ptr->message.u.rrcConnectionReject.t ==
                   T_rrc_RRCConnectionReject_r3 ) )
              {
                if(rrcrce_wait_time == 0)
                {
                  if(rrccho_is_gtow_pcco_in_progress())
                  {
                    rrc_conn_setup_ind_type conn_setup_ind_params;
                    conn_setup_ind_params.status = FAILURE;
                    conn_setup_ind_params.conn_failure_reason = RRC_CONNECTION_REJECT;

                    WRRC_MSG0_HIGH("PCCO active.Connection Reject with wait time = 0");
                    rrcrce_send_conn_setup_ind(conn_setup_ind_params);                  
     
                    /* set rce_substate to initial */
                    rrcrce_substate = RRCRCE_INITIAL;
                    break;
                  }

                  WRRC_MSG0_HIGH("Wt_tm=0. Send RELEASE_RB0; goto RRC_STATE_DISCONNECTED");
                  
                  /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
                 rrcrce_send_rrc_initiate_cell_selection_ind(
                   RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                    /* The RCE Procedure ends */
                 rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF; 

                    /* Set Abort Cause appropriately. When the abort cause is 
                     * RRC_MAX_ACCESS_FAILURE then SR is not attempted. This has been
                     * done for GCF test case 6.1.2.6 (part 2).
                     */
#ifndef FEATURE_WCDMA_FIELD_OPTIMIZATIONS
                 WRRC_MSG0_HIGH("Setting RRC->MM abort_cause to RRC_MAX_ACCESS_FAILURE");
                 rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE;
#else
                 WRRC_MSG0_HIGH("Setting RRC->MM abort_cause to FAILURE_DUE_TO_CONG");
                 rrcrce_mm_abort_cause = RRC_FAILURE_DUE_TO_CONGESTION;
#endif
                }
                else  /* Wait Time is non-zero */
                {
                  if( RRCRCE_SUCCESS !=
                      rrcrce_get_n300_from_serv_cell_db(&rrcrce_n_300))
                  {
                    ERR_FATAL("N300 value is invalid", rrcrce_n_300,0,0);
                  }
                  if( rrc_v_300 <= rrcrce_n_300)
                  {
                    /*Check whether redirection info is present */
                    if(RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionReject.u.r3.
                           rrcConnectionReject_r3,rrc_RRCConnectionReject_r3_IEs,redirectionInfo))
                    {
                      switch(msg_ptr->message.u.rrcConnectionReject.u.r3.
                             rrcConnectionReject_r3.redirectionInfo.t)
                      {
                        case T_rrc_RedirectionInfo_frequencyInfo:   /* Redirection to new Freq */
                          if(msg_ptr->message.u.rrcConnectionReject.u.r3.
                             rrcConnectionReject_r3.redirectionInfo.u.frequencyInfo->
                             modeSpecificInfo.t ==
                                              T_rrc_FrequencyInfo_modeSpecificInfo_fdd)
                          {
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
                            if(SUCCESS == validate_dl_arfcn(
                                              msg_ptr->message.u.rrcConnectionReject.u.r3.
                                              rrcConnectionReject_r3.redirectionInfo.u.frequencyInfo->
                                              modeSpecificInfo.u.fdd->uarfcn_DL, 
                                              rrc_ccm_get_curr_camped_plmn(),
                                              W_UARFCN_NOT_FROM_GSM_NBR_LIST
                                              , TRUE
                                              )
                              )
#else
                            if(SUCCESS == validate_dl_arfcn(
                                              msg_ptr->message.u.rrcConnectionReject.u.r3.
                                              rrcConnectionReject_r3.redirectionInfo.u.frequencyInfo->
                                              modeSpecificInfo.u.fdd->uarfcn_DL
                                              , TRUE
                                              )
                              )
#endif
                            {
                               rsp_to_rrc_conn_req = RRCRCE_CONNECTION_REJECT;

                              /* Send the command to Cell Selection Procedure */
                                rrcrce_send_redirection_req(msg_ptr, FALSE);     
                                MSG_HIGH("RCE: Inter-freq redirection req for freq %d to CSP", 
                                                    msg_ptr->message.u.rrcConnectionReject.u.r3.
                                                    rrcConnectionReject_r3.redirectionInfo.u.
                                                    frequencyInfo->modeSpecificInfo.u.fdd->uarfcn_DL, 0, 0);
                                /* Wait for redirect confirm from Cell Selection Proc */
                                rrcrce_substate = RRCRCE_WAIT_FOR_REDIRECT_CNF;   
                              }  
                            /* Unsupported frequency received. Start wait timer */
                            else
                            {
                              MSG_HIGH("iFreq Redirection rcvd with unsupported freq %d",
                                               msg_ptr->message.u.rrcConnectionReject.u.r3.
                                               rrcConnectionReject_r3.redirectionInfo.u.frequencyInfo->
                                               modeSpecificInfo.u.fdd->uarfcn_DL,0,0);

#ifdef FEATURE_DUAL_SIM
                              if(wcdma_is_ue_in_dual_standby && ((rrc_establishment_cause == RRC_EST_REGISTRATION) ||
                                 (rrc_establishment_cause == RRC_EST_CALL_RE_ESTABLISH) ||
                                 (rrc_establishment_cause == RRC_EST_INTER_RAT_CELL_RESELECT)))
                              {
                                /* Release the RB 0 and request the RRC LLC to go to Idle  disconnected */
                                /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
                                rrcrce_send_rrc_initiate_cell_selection_ind(
                                  RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                                /* The RCE Procedure ends */
                                rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF; 

                                /* Set Abort Cause */
                                rrcrce_mm_abort_cause = RRC_ABORT_CONN_REJECT_WAIT_TMR;
                              }
                              else
#endif
                              {
                                rrctmr_start_timer(RRCTMR_WAIT_TIME_TIMER, rrcrce_wait_time * 1000);  
                                rrcrce_substate = RRCRCE_WAIT_FOR_WAIT_TIMER_TO_EXPIRE;
                              }
                            }
                          }
                          else
                          {
                            MSG_HIGH("Received Unsupported Freq Info:%d in redirection",
                              msg_ptr->message.u.rrcConnectionReject.u.r3.
                              rrcConnectionReject_r3.redirectionInfo.u.frequencyInfo->
                              modeSpecificInfo.t,0,0);

                            /* Since this redirection is unsupported, abandon the procedure */
                            /* Release the RB 0 and request the RRC LLC to go to
                               Idle disconnected */
                            /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
                            rrcrce_send_rrc_initiate_cell_selection_ind(
                            RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                              /* The RCE Procedure ends */
                            rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF; 

                            /* Set Abort Cause */
                            rrcrce_mm_abort_cause = RRC_UNSUPPORTED_REDIRECTION;
                         }    
                          break;

                      case T_rrc_RedirectionInfo_interRATInfo: /* Redirection to new System */

                        rrcrce_handle_interrat_redirection_info(
                                                    msg_ptr
                                #ifdef FEATURE_WCDMA_TO_LTE
                                                    , FALSE
                                #endif
                                                    );
                          break;

                        default:  /* No redirection, resend after wait time expire */                      
                          WRRC_MSG1_HIGH("Received Unsupported redirection: %d",
                                   msg_ptr->message.u.rrcConnectionReject.u.r3.rrcConnectionReject_r3.redirectionInfo.t);

                          /* Since this redirection is unsupported, abandon the procedure */
                          /* Release the RB 0 and request the RRC LLC to go to
                             Idle disconnected */
                          /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
                         rrcrce_send_rrc_initiate_cell_selection_ind(
                           RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                         /* The RCE Procedure ends */
                         rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF; /* change the RCE substate */

                          /* Set Abort Cause */
                          rrcrce_mm_abort_cause = RRC_UNSUPPORTED_REDIRECTION;  
                          break;
                      } /* Switch for redirection t */
                    }
#ifdef FEATURE_WCDMA_TO_LTE
                    else if((msg_ptr->message.u.rrcConnectionReject.u.r3.m.laterNonCriticalExtensionsPresent) &&
                              (msg_ptr->message.u.rrcConnectionReject.u.r3.laterNonCriticalExtensions.m.v690NonCriticalExtensionsPresent) &&
                              (msg_ptr->message.u.rrcConnectionReject.u.r3.laterNonCriticalExtensions.v690NonCriticalExtensions.m.v6f0NonCriticalExtensionsPresent) &&
                              (msg_ptr->message.u.rrcConnectionReject.u.r3.laterNonCriticalExtensions.v690NonCriticalExtensions.v6f0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) &&
                              (msg_ptr->message.u.rrcConnectionReject.u.r3.laterNonCriticalExtensions.v690NonCriticalExtensions.v6f0NonCriticalExtensions.
                                 v860NonCriticalExtensions.rrcConnectionReject_v860ext.m.redirectionInfoPresent) &&
                              (msg_ptr->message.u.rrcConnectionReject.u.r3.laterNonCriticalExtensions.v690NonCriticalExtensions.v6f0NonCriticalExtensions.
                                 v860NonCriticalExtensions.rrcConnectionReject_v860ext.redirectionInfo.t == T_rrc_RedirectionInfo_v860ext_interRATInfo) &&
                              (msg_ptr->message.u.rrcConnectionReject.u.r3.laterNonCriticalExtensions.v690NonCriticalExtensions.v6f0NonCriticalExtensions.
                                 v860NonCriticalExtensions.rrcConnectionReject_v860ext.redirectionInfo.u.interRATInfo->t == T_rrc_InterRATInfo_v860ext_eutra)
                            )
                    {
                      rrcrce_handle_interrat_redirection_info(msg_ptr, TRUE);
                    }
#endif
                    else  /* Redirection info is not present */
                    {
#ifdef FEATURE_DUAL_SIM
                      if(wcdma_is_ue_in_dual_standby && ((rrc_establishment_cause == RRC_EST_REGISTRATION) ||
                         (rrc_establishment_cause == RRC_EST_CALL_RE_ESTABLISH) ||
                         (rrc_establishment_cause == RRC_EST_INTER_RAT_CELL_RESELECT)))
                      {
                        /* Release the RB 0 and request the RRC LLC to go to Idle disconnected */
                        /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
                        rrcrce_send_rrc_initiate_cell_selection_ind(
                          RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                        /* The RCE Procedure ends */
                        rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF; 

                        /* Set Abort Cause */
                        rrcrce_mm_abort_cause = RRC_ABORT_CONN_REJECT_WAIT_TMR;
                      }
                      else
#endif
                      {
                        /* Start the timer for wait time. The wait time units are
                         Secs */
                        WRRC_MSG0_HIGH("Redirection info not present. Start wt timer");
                        rrctmr_start_timer(RRCTMR_WAIT_TIME_TIMER, rrcrce_wait_time * 1000);  

                        rrcrce_substate = RRCRCE_WAIT_FOR_WAIT_TIMER_TO_EXPIRE;
                      }
                    }
                  } /*if V300 <= N300 */
                  else   /* V300 > N300 Reached maximum re-tries */
                  {
                    /* If GTOW PCCO is active,sent a conn_setup_ind with failure */
                    if(rrccho_is_gtow_pcco_in_progress())
                    {
                      rrc_conn_setup_ind_type conn_setup_ind_params;
                      conn_setup_ind_params.status = FAILURE;
                      conn_setup_ind_params.conn_failure_reason = RRC_MAX_RACH_ATTEMPTS;

                      WRRC_MSG0_HIGH("Max RACH attempts though wait_timer non zero in Conn Reject.PCCO active");
                      rrcrce_send_conn_setup_ind(conn_setup_ind_params);
                      
                      /* set rce_substate to initial */
                      rrcrce_substate = RRCRCE_INITIAL;
                      break;
                    }
                    rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                            (uint8) RRC_PROCEDURE_RCE,
                                            (uint8) RRCLOG_RachFailure,
                                            (uint8) RRCLOG_PROTOCOL_ERROR_NA
                                          );   

                    /* Release the RB 0 and request the RRC LLC to go to Idle disconnected */
                     rrcrce_send_rrc_initiate_cell_selection_ind(
                       RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                      /* The RCE Procedure ends */
                     rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;

                      /* Set Abort Cause */
                      rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE;  
                    }    
                }   /* Wait time is non-zero */
              } /* Valid Transaction Id  and critical extensions not selected */
              else /*Invalid Transaction Id is received */
              {
                 /* Set the Protocol Error Indicator to TRUE */
                 rrc_protocol_error_indicator = rrc_ProtocolErrorIndicator_errorOccurred;
              }
              /*lint -restore */
            } /* ASN1 decoding is successful */   
            break; 

          default:     
            WRRC_MSG1_ERROR("Invalid message is forwarded: %d",
                 message_choice);
            break;
        } /* Switch for the message */

      }/* Checks for rrc_DL_CCCH_Message PDU type */
      else /* Received other than rrc_DL_CCCH_Message type */
      {
        if(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type != rrc_DL_CCCH_Message_PDU)
        {
          WRRC_MSG1_HIGH("Rcvd an invalid message type:%d",
                  cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type);
        }
        else /* Reselection is in progress */
        {
          /* Stop the timer T300 */
          rrctmr_stop_timer(RRCTMR_T_300_TIMER);
          /* Trash the message received on DL CCCH since reselection is in
             progress and wait for RRC_NEW_CELL_IND from the CSP */
          rrcrce_substate = RRCRCE_WAIT_FOR_RESELCT_COMPL;  
        }
        
      }
      break;

    default:     
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcrce_substate);
      break;
  }
}
/* rrcrce_process_rce_wait_for_conn_setup_substate  */
/*===========================================================================

FUNCTION rrcrce_process_rce_wait_for_reselct_compl_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in RRCRCE_WAIT_FOR_RESELCT_COMPL substate of 
  Connecting State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_reselct_compl_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to the received 
                                                command */
)
{
  rrc_abort_cause_e_type  cause = RRC_MAX_ACCESS_FAILURE;
  rrc_N_300 rrcrce_n_300;                    /* Local Varibale to store 
                                                N300 */ 
  switch (cmd_ptr->cmd_hdr.cmd_id)
  {
      
    case RRC_STATE_CHANGE_IND:
      if((cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED) &&
         ( rrc_get_state() == RRC_STATE_DISCONNECTED))
      {
        /* Stop the timer for Physical Channel Timer */     
        rrctmr_stop_timer(RRCTMR_PHY_CHAN_FAILURE_TIMER);  

        /* Send RRC_ABORT_IND command to the MM */
        rrcrce_send_rrc_abort_ind(RRC_FAILURE_UNSPECIFIED);      
           /* Reset RCE Procedure and  Procedure ends */
        rrcrce_substate = RRCRCE_INITIAL; 
      }    
      break;

    case RRC_ABORT_REQ: 
      if (rrc_get_state() == RRC_STATE_DISCONNECTED)
      {
        /* Send RRC_ABORT_CNF command to the MM */
        rrcrce_send_rrc_abort_cnf();     
        rrcrce_substate = RRCRCE_INITIAL; /* The RCE Procedure ends */
        break;
      }
      /* Stop the timer for Physical Channel Timer */     
         rrctmr_stop_timer(RRCTMR_PHY_CHAN_FAILURE_TIMER);  

      /* Sends RRC_CHANNEL_CONFIG_REQ command to RRC LLC to release RB 0*/
     /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
         rrcrce_send_rrc_initiate_cell_selection_ind(
            RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
        /* Wait for successful release of RB0 */
        rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF; //RRCRCR_WAIT_FOR_SRB_REL_CNF; 
        rrcrce_send_abort_cnf = TRUE;
      
      break;

    case RRC_NEW_CELL_IND: 
      
      if (rrc_get_state() == RRC_STATE_DISCONNECTED)
      {
        break;
      }

      /* Stop the timer for Physical Channel Timer */     
      rrctmr_stop_timer(RRCTMR_PHY_CHAN_FAILURE_TIMER);  

      /* The UE is already in CONNECTING State. The CCM should have reconfigured
          the  RB 0*/
      /* Gets N300 from the SIB 1 database */
      if( RRCRCE_SUCCESS != 
          rrcrce_get_n300_from_serv_cell_db(&rrcrce_n_300))
      {
        ERR_FATAL("N300 value is invalid", rrcrce_n_300,0,0);
      }
      if((RRCRCE_SUCCESS == rrcrce_validate_cell_access_status
                       (rrc_est_cn_domain_id,rrc_establishment_cause,&cause)) &&
         (rrc_v_300 <= rrcrce_n_300))
      {        
        /* Resend RRC connection Request Message and this function restarts the
           T300 Timer */
        rrcrce_send_rrc_connection_request_msg();
        /* RRC Connection Request Message is successfully sent */
        /* Increments the re-transmission counter for RRC Connection Request
             Message */
        WRRC_MSG1_HIGH("RRC Connection Request is sent: %d",rrc_v_300);

        /* The RCE procedure waits for the RRC Connection Setup Message */
        rrcrce_substate = RRCRCE_WAIT_FOR_RRC_CONN_SETUP;

        rrc_v_300++;     
      }
      else /* Reached maximum allowed re-tries */
      { 
        rrcrce_num_retries_count++;
        WRRC_MSG1_HIGH("rrcrce_num_retries_count = %d",rrcrce_num_retries_count);

        /* Release the RB 0 and request the RRC LLC to go to Idle disconnected */
        /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
       rrcrce_send_rrc_initiate_cell_selection_ind(
       RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
       /* The RCE Procedure ends */
       rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF; /* change the RCE substate */ 

       /* Set Abort Cause */
       if(cause  ==  RRC_ACCESS_CLASS_NOT_ALLOWED)
       {
         if(rrc_est_cn_domain_id == RRC_CS_DOMAIN_CN_ID)
         {
           rrcrce_mm_abort_cause = RRC_AC_NOT_ALLOWED_CS_DOMAIN;
         }
         else
         {
           rrcrce_mm_abort_cause = RRC_AC_NOT_ALLOWED_PS_DOMAIN;
         }
       }
       else
       {
         rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE;
         rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                 (uint8) RRC_PROCEDURE_RCE,
                                 (uint8) RRCLOG_RachFailure,
                                 (uint8) RRCLOG_PROTOCOL_ERROR_NA
                               );   

       }
     }
      break; 
             
    case RRC_DOWNLINK_SDU_IND:

      /* RRC Connection Setup or RRC Connection Reject may be received in this
         Substate because of cell-reselections. */
      WRRC_MSG0_HIGH("Simultanous transactions not supported");
      break;
      
   
    case RRC_PHY_CHAN_FAILURE_TIMER_EXPIRED_IND:

      /* 10 second timer has expired so take UE to idle */
      WRRC_MSG0_HIGH("PHY_CHAN_FAILURE TIMER EXPIRED. Taking UE To IDLE : ");
      
      /* The agreement is that CSP would tear down RB0 and RCE
         should not send channel config req to tear down RB0 */
      rrc_transition_to_disconnected_state(RRC_PROCEDURE_RCE,RRC_TX_TO_DISCON_OTHER);
        /* Update the release cause */
        rrc_release_cause = RRC_REL_DEEP_FADE;

        /* RRC_ABORT_IND will be sent rrcrce_process_rce_wait_for_rb0_rel_cnf_substate 
        upon a state change indication */ 
        rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;
        rrcrce_mm_abort_cause = RRC_FAILURE_UNSPECIFIED;

      break;

    default:     
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcrce_substate);
      break;
  }
}
/* rrcrce_process_rce_wait_for_reselct_compl_substate  */


/*===========================================================================

FUNCTION rrcrce_validate_and_handle_acc_class_restr_on_redirec_succ

DESCRIPTION
  This function validates the access class restriction on redirection success and initiates cell selection if
  access is not allowed.

DEPENDENCIES
  None

RETURN VALUE
  uecomdef_status_e_type
  SUCCESS if access class validation succeeds 
  FAILURE otherwise

SIDE EFFECTS
  None
===========================================================================*/
static uecomdef_status_e_type rrcrce_validate_and_handle_acc_class_restr_on_redirec_succ
(
  void
)
{
  uecomdef_status_e_type status = SUCCESS;
  rrc_abort_cause_e_type  cause = RRC_MAX_ACCESS_FAILURE;

  if(RRCRCE_SUCCESS != rrcrce_validate_cell_access_status
                             (rrc_est_cn_domain_id,rrc_establishment_cause,&cause))
  {
    /*Access is not allowed on redirected frequency*/
    if(rrccho_is_gtow_pcco_in_progress())
    {
      rrc_conn_setup_ind_type conn_setup_ind_params;
      conn_setup_ind_params.status = FAILURE;
      conn_setup_ind_params.conn_failure_reason = RRC_CONN_FAILURE_OTHER;

      WRRC_MSG1_HIGH("Access not allowed when PCCO is active.rrcrce_substate %d",rrcrce_substate);      
      rrcrce_send_conn_setup_ind(conn_setup_ind_params);                  

      /* set rce_substate to initial */
      rrcrce_substate = RRCRCE_INITIAL;
    }
    else
    {
      /*
         Initiate Cell Selection Indication and wait for State change indication. 
         On state change indication send ABORT_IND to NAS with cause ACCESS CLASS not allowed
      */
        rrcrce_send_rrc_initiate_cell_selection_ind(
           RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
        rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;  /* Change the RCE substate */
        if(rrc_est_cn_domain_id == RRC_CS_DOMAIN_CN_ID)
        {
          rrcrce_mm_abort_cause = RRC_AC_NOT_ALLOWED_CS_DOMAIN;
        }
        else
        {
          rrcrce_mm_abort_cause = RRC_AC_NOT_ALLOWED_PS_DOMAIN;
        }
      }
    status = FAILURE;
  }
  return status;
}

/*===========================================================================

FUNCTION rrcrce_process_rce_wait_for_redirect_cnf_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection 
  Establishment procedure in RRCRCE_WAIT_FOR_REDIRECT_CNF substate of 
  Connecting State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_redirect_cnf_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to the received 
                                                command */
)
{
  rrc_N_300 rrcrce_n_300;                    /* Local Varibale to store N300 */ 
  rrc_cmd_type *rce_out_cmd_ptr;
  rrc_msg_ver_enum_type msg_version = MSG_REL99;
  rrc_abort_cause_e_type  cause = RRC_MAX_ACCESS_FAILURE;

  switch (cmd_ptr->cmd_hdr.cmd_id)
  {
    
    case RRC_DOWNLINK_SDU_IND:
      /* RRC Connection Setup or RRC Connection Reject may be received in this
         Substate because of cell-reselections. */
      WRRC_MSG0_HIGH("Simultanous transactions not supported");
      break;
    
    case RRC_STATE_CHANGE_IND:
      if((cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED) &&
         ( rrc_get_state() == RRC_STATE_DISCONNECTED))
      {
        if(rrccho_is_gtow_pcco_in_progress())
        {
          rrc_conn_setup_ind_type conn_setup_ind_params;
          conn_setup_ind_params.status = FAILURE;
          conn_setup_ind_params.conn_failure_reason = RRC_CONN_FAILURE_OTHER;

          WRRC_MSG1_HIGH("RRC_STATE_CHANGE_IND received when PCCO is active.rrcrce_substate %d",rrcrce_substate);      
          rrcrce_send_conn_setup_ind(conn_setup_ind_params);                  
       
          /* set rce_substate to initial */
          rrcrce_substate = RRCRCE_INITIAL;
          break;
        }

        /* Send RRC_ABORT_IND command to the MM */
         rrcrce_send_rrc_abort_ind(RRC_FAILURE_UNSPECIFIED);      
         rrcrce_substate = RRCRCE_INITIAL; 
      }    
      break;
    
    case RRC_ABORT_REQ:
      /* The agreement is that CSP would tear down RB0 and RCE
         should not send channel config req to tear down RB0 */
      rrc_transition_to_disconnected_state(RRC_PROCEDURE_RCE,RRC_TX_TO_DISCON_OTHER);
        /* Update the release cause */
        rrc_release_cause = RRC_REL_DEEP_FADE;
      
        rrcrce_send_abort_cnf = TRUE;
      
        /* RRC_ABORT_CNF will be sent rrcrce_process_rce_wait_for_rb0_rel_cnf_substate 
         * upon a state change indication */ 
        rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;
        rrcrce_mm_abort_cause = RRC_FAILURE_UNSPECIFIED;
      break;

    case RRC_REDIRECT_CNF:
      /* Redirection succeeded */
      if(cmd_ptr->cmd.redirect_cnf.status == RRC_REDIRECT_SUCCESS)
      { 
        switch(cmd_ptr->cmd.redirect_cnf.cause)
        {
          case RRC_CAMPED_ON_REDIRECTED_FREQ:

            /* Redirection success for inter-freq redirection via RRC Connection Reject */
            if(rsp_to_rrc_conn_req == RRCRCE_CONNECTION_REJECT)
            {
              WRRC_MSG0_HIGH("RCE rcvd RRC_REDIRECT_SUCCESS for Connection REJECT. Reset v300");

              /* Reset V300 counter */
              rrc_v_300 = 0;
              /*Validate the access class on the redirected frequency, if access is allowed then send connection request*/
              if(SUCCESS == rrcrce_validate_and_handle_acc_class_restr_on_redirec_succ())
              {
                /* Resend RRC Connection Request message and this function
                   restarts the T300 Timer*/
                rrcrce_send_rrc_connection_request_msg();
                WRRC_MSG1_HIGH("RRC Connection Request is sent: %d",rrc_v_300);
                rrc_v_300++;
                rrcrce_substate = RRCRCE_WAIT_FOR_RRC_CONN_SETUP;
              }
            }
            /* Redirection success for inter-freq redirection via RRC Connection Setup */
            else if(rsp_to_rrc_conn_req == RRCRCE_CONNECTION_SETUP)
            {
              WRRC_MSG0_HIGH("RCE rcvd RRC_REDIRECT_SUCCESS for Freq redirection in Conn SETUP msg");
              /*Validate the access class on the redirected frequency, if access is allowed then send connection request*/
              if(SUCCESS == rrcrce_validate_and_handle_acc_class_restr_on_redirec_succ())
              {
                /* As per Spec, V300 counter is not reset */
                /* Compare new N300 with V300 and send RRC Connection Request accordingly */
                if(RRCRCE_SUCCESS != rrcrce_get_n300_from_serv_cell_db(&rrcrce_n_300))
                {
                  ERR_FATAL("N300 value is invalid", rrcrce_n_300,0,0);
                }                    
                else if(rrc_v_300 <= rrcrce_n_300)
                {
                  WRRC_MSG2_HIGH("v300 (%d) <= n300 (%d). Send conn req again", rrc_v_300,rrcrce_n_300);

                  /* Resend RRC Connection Request message and this function
                     restarts the T300 Timer*/
                  rrcrce_send_rrc_connection_request_msg();
                  /* RRC Connection Request Message is successfully sent */
                  /* Increment the re-transmission counter for RRC Connection
                       Request message */
                  WRRC_MSG1_HIGH("RRC Connection Request is sent: %d",rrc_v_300);
                  rrc_v_300++;
                  rrcrce_substate = RRCRCE_WAIT_FOR_RRC_CONN_SETUP;
                }
                else
                {
                  WRRC_MSG2_HIGH("v300 (%d) > n300 (%d) on the new cell. Go to idle", rrc_v_300,rrcrce_n_300);    

                  /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
                   rrcrce_send_rrc_initiate_cell_selection_ind(
                     RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                    /* The RCE Procedure ends */
                    rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;  /* Change the RCE substate */
                    rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE;
                  rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                          (uint8) RRC_PROCEDURE_RCE,
                                          (uint8) RRCLOG_RachFailure,
                                          (uint8) RRCLOG_PROTOCOL_ERROR_NA
                                        );   
                  }  
              }
            }
            
            /* Incorrect value for rsp_to_rrc_conn_req - which should never be the case
             * as the var rsp_to_rrc_conn_req is always set before sending redirection
             * request to CSP.
             */
            else
            {
              WRRC_MSG1_ERROR("ERR:RCE rcvd REDIRECT_SUCCESS when rsp_to_rrc_conn_req %d. Trigger error recovery.",
                                                                                  rsp_to_rrc_conn_req);
              if(rrccho_is_gtow_pcco_in_progress())
              {
                rrc_conn_setup_ind_type conn_setup_ind_params;
                conn_setup_ind_params.status = FAILURE;
                conn_setup_ind_params.conn_failure_reason = RRC_CONN_FAILURE_OTHER;

                WRRC_MSG1_HIGH("RRC_STATE_CHANGE_IND received when PCCO is active.rrcrce_substate %d",rrcrce_substate);

                rrcrce_send_conn_setup_ind(conn_setup_ind_params);

                /* set rce_substate to initial */
                rrcrce_substate = RRCRCE_INITIAL;
                break;
              }

              /* Release RB 0 and request RRC LLC to go to Idle Disconnected */
              /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
              WRRC_MSG0_HIGH("Send Init Cell Sel Ind to CSP instead Channel Config Req");    
                rrcrce_send_rrc_initiate_cell_selection_ind(
                  RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                /* The RCE Procedure ends */
                rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;  /* Change the RCE substate */

                rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE;
              }  
            break;                                           

          case RRC_CAMPED_ON_REDIRECTED_CELL:
            WRRC_MSG0_HIGH("RCE rcvd RRC_REDIRECT_SUCCESS for PSC redirection in Conn SETUP msg");

            /* Redirection success for inter-freq redirection via RRC Connection Setup */
            if(rsp_to_rrc_conn_req == RRCRCE_CONNECTION_SETUP)
            {
              /* Decode the saved message */
              save_dl_ccch_msg.decoded_msg = 
                     ((rrc_DL_CCCH_Message *) rrcasn1_decode_pdu((void *)save_dl_ccch_msg.dl_sdu,
                                                                 (int)save_dl_ccch_msg.dl_sdu_type,
                                                                 save_dl_ccch_msg.dl_sdu_length
                                                                 ));
              /* If failed to decode then go to idle */
              if(save_dl_ccch_msg.decoded_msg == NULL)
              {
                  WRRC_MSG0_ERROR("ERR: failed to decode the pdu. Tx to discon");

                /* The agreement is that CSP would tear down RB0 and RCE
                   should not send channel config req to tear down RB0 */
                  rrc_transition_to_disconnected_state(RRC_PROCEDURE_RCE,RRC_TX_TO_DISCON_OTHER);
                  /* Update the release cause */
                  rrc_release_cause = RRC_REL_CAUSE_UNSPEC;

                  /* RRC_ABORT_IND will be sent rrcrce_process_rce_wait_for_rb0_rel_cnf_substate 
                   * upon a state change indication */ 
                  rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;
                  rrcrce_mm_abort_cause = RRC_FAIL_TO_CAMP_ON;
              }
              /* PDU was successfully decoded */
              else
              {
                /* Validate the RRC Connection Setup message */
                if(RRCRCE_SUCCESS != rrcrce_validate_rrc_connection_setup_msg(
                                                save_dl_ccch_msg.decoded_msg,
                                                save_dl_ccch_msg.dl_sdu_type, 
                                                &msg_version))
                {
                  WRRC_MSG0_ERROR("After Redirection Connection Setup Message cld not be validated");

                  /* Unsuppored IE is received from the UTRAN */
                  rrc_protocol_error_reject = TRUE;
                  rrc_protocol_error_indicator = rrc_ProtocolErrorIndicator_errorOccurred;

                  /* Send RRC Connection Request again if V300 <= N 300 */
                  if( RRCRCE_SUCCESS != rrcrce_get_n300_from_serv_cell_db(&rrcrce_n_300))
                  {
                    ERR_FATAL("N300 value is invalid", rrcrce_n_300,0,0);
                  }
                  if(rrc_v_300 <= rrcrce_n_300)
                  {                       
                    rrcrce_send_rrc_connection_request_msg();
                    WRRC_MSG1_HIGH("RRC Connection Request is sent: %d",rrc_v_300);
                    rrc_v_300++;
                  }
                  /* Max re-tries exceeded */
                  else       
                  {
                    rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                            (uint8) RRC_PROCEDURE_RCE,
                                            (uint8) RRCLOG_RachFailure,
                                            (uint8) RRCLOG_PROTOCOL_ERROR_NA
                                          );   
                    /* Release RB 0 and request RRC LLC to go to Idle Disconnected */
                    /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
                      rrcrce_send_rrc_initiate_cell_selection_ind(
                        RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                      /* The RCE Procedure ends */
                      rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;  /* Change the RCE substate */

                      rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE;
                  }  /* Max re-tries exceeded */
                }
                /* RRC Connection Setup is successfully validated */
                else
                {
                  WRRC_MSG0_HIGH("RRC ConnSetUp msg has been validated");

                  /* Reset V300 counter */
                  rrc_v_300 = 0;

                  /* Update the required parameters */
                  if (msg_version == MSG_REL99)
                  {
                    /* Update RCE Transaction Identifier */
                    /*lint -save -e613 */
                    rrcrce_setup_transaction_identifier = save_dl_ccch_msg.decoded_msg->message.u.
                      rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.rrc_TransactionIdentifier;

                    /* Update RRC State Indicator */
                    rrcrce_state_indicator = save_dl_ccch_msg.decoded_msg->message.u.
                      rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.rrc_StateIndicator;
                    /*lint -restore */

                    /* Initialize the Capability Update Requirement variable */
                    rrc_capability_update_requirement.system_specific_cap_update_req_present = FALSE;
                    rrc_capability_update_requirement.ue_radio_capability_update_requirement = FALSE;

                    /* Save the RRC Capability Update Requirement to build RRC
                       CONNECTION SETUP COMPLETE message */
                    /*lint -save -e613 */
                    rrcuece_copy_capability_update_requirement(save_dl_ccch_msg.decoded_msg->message.u.
                      rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.
                      capabilityUpdateRequirement);
                    /*lint -restore */
                  }

                  if (msg_version == MSG_REL5)
                  {
                    /* Update RCE Transaction Identifier */
                    /*lint -save -e613 */
                    rrcrce_setup_transaction_identifier = save_dl_ccch_msg.decoded_msg->message.u.
                      rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier;

                    /* Update RRC State Indicator */
                    rrcrce_state_indicator = save_dl_ccch_msg.decoded_msg->message.u.
                      rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                      criticalExtensions.u.r5.rrcConnectionSetup_r5.rrc_StateIndicator;

                    /*lint -restore */                  
                    /* Initialize the Capability Update Requirement variable */
                    rrc_capability_update_requirement.system_specific_cap_update_req_present = FALSE;
                    rrc_capability_update_requirement.ue_radio_capability_update_requirement = FALSE;

                    /* Save the RRC Capability Update Requirement to build RRC
                    CONNECTION SETUP COMPLETE message */
                    /*lint -save -e613 */
                    rrcuece_copy_capability_update_requirement_later_than_r3(save_dl_ccch_msg.decoded_msg->message.u.
                      rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                      criticalExtensions.u.r5.rrcConnectionSetup_r5.capabilityUpdateRequirement);
                    /*lint -restore */
                  }
                  if ((msg_version == MSG_REL6)&& 
                    (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6))
                  {
                    /* Update RCE Transaction Identifier */
                    /*lint -save -e613 */
                    rrcrce_setup_transaction_identifier = save_dl_ccch_msg.decoded_msg->message.u.
                      rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier;

                    /* Update RRC State Indicator */
                    rrcrce_state_indicator = save_dl_ccch_msg.decoded_msg->message.u.
                      rrcConnectionSetup.u.later_than_r3.criticalExtensions.u. criticalExtensions.u.
                      criticalExtensions.u.r6.rrcConnectionSetup_r6.rrc_StateIndicator;

                    /*lint -restore */                  
                    /* Initialize the Capability Update Requirement variable */
                    rrc_capability_update_requirement.system_specific_cap_update_req_present = FALSE;
                    rrc_capability_update_requirement.ue_radio_capability_update_requirement = FALSE;

                    /* Save the RRC Capability Update Requirement to build RRC
                    CONNECTION SETUP COMPLETE message */
                    /*lint -save -e613 */
                    rrcuece_copy_capability_update_requirement_later_than_r3(save_dl_ccch_msg.decoded_msg->message.u.
                      rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
                      criticalExtensions.u.r6.rrcConnectionSetup_r6.capabilityUpdateRequirement);
                    /*lint -restore */
                  }
                  if ((msg_version == MSG_REL7)&& 
                    (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7))
                  {
                    /* Update RCE Transaction Identifier */
                    /*lint -save -e613 */
                    rrcrce_setup_transaction_identifier = save_dl_ccch_msg.decoded_msg->message.u.
                      rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier;

                    /* Update RRC State Indicator */
                    rrcrce_state_indicator = save_dl_ccch_msg.decoded_msg->message.u.
                      rrcConnectionSetup.u.later_than_r3.criticalExtensions.u. criticalExtensions.u.
                      criticalExtensions.u.criticalExtensions.u.r7.rrcConnectionSetup_r7.rrc_StateIndicator;

                    /*lint -restore */                  
                    /* Initialize the Capability Update Requirement variable */
                    rrc_capability_update_requirement.system_specific_cap_update_req_present = FALSE;
                    rrc_capability_update_requirement.ue_radio_capability_update_requirement = FALSE;

                    /* Save the RRC Capability Update Requirement to build RRC
                    CONNECTION SETUP COMPLETE message */
                    /*lint -save -e613 */
                    rrcuece_copy_capability_update_requirement_r7(&save_dl_ccch_msg.decoded_msg->message.u.
                      rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
                      criticalExtensions.u.criticalExtensions.u.r7.rrcConnectionSetup_r7.capabilityUpdateRequirement);
                    /*lint -restore */
                  }
                  if ((msg_version == MSG_REL8)&& 
                    (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8))
                  {
                    /* Update RCE Transaction Identifier */
                    /*lint -save -e613 */
                    rrcrce_setup_transaction_identifier = save_dl_ccch_msg.decoded_msg->message.u.
                      rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier;

                    /* Update RRC State Indicator */
                    rrcrce_state_indicator = save_dl_ccch_msg.decoded_msg->message.u.
                      rrcConnectionSetup.u.later_than_r3.criticalExtensions.u. criticalExtensions.u.
                      criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.rrcConnectionSetup_r8.rrc_StateIndicator;

                    /*lint -restore */                  
                    /* Initialize the Capability Update Requirement variable */
                    rrc_capability_update_requirement.system_specific_cap_update_req_present = FALSE;
                    rrc_capability_update_requirement.ue_radio_capability_update_requirement = FALSE;

                    /* Save the RRC Capability Update Requirement to build RRC
                    CONNECTION SETUP COMPLETE message */
                    /*lint -save -e613 */
                    rrcuece_copy_capability_update_requirement_r8(&save_dl_ccch_msg.decoded_msg->message.u.
                      rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
                      criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.rrcConnectionSetup_r8.capabilityUpdateRequirement);
                    /*lint -restore */
                  }

#ifdef FEATURE_WCDMA_REL9
                  if ((msg_version == MSG_REL9)&& 
                    (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9))
                  {
                    /* Update RCE Transaction Identifier */
                    /*lint -save -e613 */
                    rrcrce_setup_transaction_identifier = save_dl_ccch_msg.decoded_msg->message.u.
                      rrcConnectionSetup.u.later_than_r3.rrc_TransactionIdentifier;

                    /* Update RRC State Indicator */
                    rrcrce_state_indicator = save_dl_ccch_msg.decoded_msg->message.u.
                      rrcConnectionSetup.u.later_than_r3.criticalExtensions.u. criticalExtensions.u.
                      criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.rrcConnectionSetup_r9.rrc_StateIndicator;

                    /*lint -restore */                  
                    /* Initialize the Capability Update Requirement variable */
                    rrc_capability_update_requirement.system_specific_cap_update_req_present = FALSE;
                    rrc_capability_update_requirement.ue_radio_capability_update_requirement = FALSE;

                    /* Save the RRC Capability Update Requirement to build RRC
                    CONNECTION SETUP COMPLETE message */
                    /*lint -save -e613 */
                    rrcuece_copy_capability_update_requirement_r8(&save_dl_ccch_msg.decoded_msg->message.u.
                      rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
                      criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.rrcConnectionSetup_r9.capabilityUpdateRequirement);
                    /*lint -restore */
                  }
                  
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
                #error code not present
#endif /* FEATURE_WCDMA_REL10 */

                    rrcrce_send_rrc_channel_config_req_for_srb(ESTABLISH_RB, RRC_STATE_CELL_FACH, TRUE);  
                    rrcrce_substate = RRCRCE_WAIT_FOR_SRB_SETUP_CNF;     
                } /* RRC Conn Setup Successfully Validated */   
              } /* PDU successfully decoded */
            }
            /* Incorrect value for rsp_to_rrc_conn_req - which should never be the case
             * as the var rsp_to_rrc_conn_req is always set before sending redirection
             * request to CSP.
             */
            else
            {
              WRRC_MSG1_ERROR("ERR:RCE rcvd REDIRECT_SUCCESS for PSC when rsp_to_rrc_conn_req %d. Recover",
                                                                        rsp_to_rrc_conn_req);
              /* Release RB 0 and request RRC LLC to go to Idle Disconnected */
              /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
              rrcrce_send_rrc_initiate_cell_selection_ind(RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                  /* The RCE Procedure ends */
              rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;  /* Change the RCE substate */

              rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE;
            }  
            break;

          default:
            WRRC_MSG1_ERROR("Redirection success rcvd by RCE w/ incorrect cause %d",
                                        cmd_ptr->cmd.redirect_cnf.cause);
            break;
        }
      }
      /* Redirection has failed */
      else 
      {
        switch(cmd_ptr->cmd.redirect_cnf.cause)
        {
          /* UE camped, but not on redirected frequency */
          case RRC_CAMPED_ON_DIFFERENT_FREQ:

            WRRC_MSG1_HIGH("RCE rcvd RDRCT_FAILURE cause %d. Checking N300 for the new cell", 
                                                  cmd_ptr->cmd.redirect_cnf.cause);

            if( RRCRCE_SUCCESS !=
                rrcrce_get_n300_from_serv_cell_db(&rrcrce_n_300))
            {
              ERR_FATAL("N300 value is invalid", rrcrce_n_300,0,0);
            }


            if((RRCRCE_SUCCESS == rrcrce_validate_cell_access_status
                           (rrc_est_cn_domain_id,rrc_establishment_cause,&cause))&&
               (rrc_v_300 <= rrcrce_n_300))
            {
              WRRC_MSG2_HIGH("v300 (%d) <= n300 (%d). Send conn req again", rrc_v_300,rrcrce_n_300);

              /* Resend RRC Connection Request message and this function
                 restarts the T300 Timer*/
              rrcrce_send_rrc_connection_request_msg();
              /* RRC Connection Request Message is successfully sent */
              /* Increment the re-transmission counter for RRC Connection
                   Request message */
              WRRC_MSG1_HIGH("RRC Connection Request is sent: %d",rrc_v_300);
              rrc_v_300++;
              rrcrce_substate = RRCRCE_WAIT_FOR_RRC_CONN_SETUP;
            }
            else
            {
              WRRC_MSG2_HIGH("v300 (%d) > n300 (%d) on the new cell. Go to idle", rrc_v_300,rrcrce_n_300);    

              if(rrccho_is_gtow_pcco_in_progress())
              {
                rrc_conn_setup_ind_type conn_setup_ind_params;
                conn_setup_ind_params.status = FAILURE;
                conn_setup_ind_params.conn_failure_reason = RRC_CONN_FAILURE_OTHER;

                WRRC_MSG1_HIGH("Max Retries when PCCO is active.rrcrce_substate %d",rrcrce_substate);      
                rrcrce_send_conn_setup_ind(conn_setup_ind_params);                  
       
                /* set rce_substate to initial */
                rrcrce_substate = RRCRCE_INITIAL;
                break;
              }

              /* Release RB 0 and request RRC LLC to go to Idle Disconnected */
              /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
              rrcrce_send_rrc_initiate_cell_selection_ind(
              RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
              /* The RCE Procedure ends */
              rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;  /* Change the RCE substate */

              if(cause  ==  RRC_ACCESS_CLASS_NOT_ALLOWED)
              {
                if(rrc_est_cn_domain_id == RRC_CS_DOMAIN_CN_ID)
                {
                  rrcrce_mm_abort_cause = RRC_AC_NOT_ALLOWED_CS_DOMAIN;
                }
                else
                {
                  rrcrce_mm_abort_cause = RRC_AC_NOT_ALLOWED_PS_DOMAIN;
                }
              }
              else
              {
                rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE;
                rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                        (uint8) RRC_PROCEDURE_RCE,
                                        (uint8) RRCLOG_RachFailure,
                                        (uint8) RRCLOG_PROTOCOL_ERROR_NA
                                      );   

              }
            }  
            break;


          case RRC_CAMPED_ON_ORIGINAL_FREQ:

            WRRC_MSG1_HIGH("RCE rcvd RDRCT_FAILURE cause %d. Sending RRC Conn Req again", 
                                                  cmd_ptr->cmd.redirect_cnf.cause);

            if(RRCRCE_SUCCESS != rrcrce_validate_cell_access_status
                      (rrc_est_cn_domain_id,rrc_establishment_cause,&cause))
            {
              /* Release RB 0 and request RRC LLC to go to Idle Disconnected */
              /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
                rrcrce_send_rrc_initiate_cell_selection_ind(
                  RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                /* The RCE Procedure ends */
                rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;  /* Change the RCE substate */

                if(cause  ==  RRC_ACCESS_CLASS_NOT_ALLOWED)
                {
                  if(rrc_est_cn_domain_id == RRC_CS_DOMAIN_CN_ID)
                  {
                    rrcrce_mm_abort_cause = RRC_AC_NOT_ALLOWED_CS_DOMAIN;
                  }
                  else
                  {
                    rrcrce_mm_abort_cause = RRC_AC_NOT_ALLOWED_PS_DOMAIN;
                  }
                }
                else
                {
                  rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE;
                }
              }
            else
            {

              /* Resend RRC Connection Request message and this function
                 restarts the T300 Timer*/
              rrcrce_send_rrc_connection_request_msg();
              /* RRC Connection Request Message is successfully sent */
              /* Increment the re-transmission counter for RRC Connection
                   Request message */
              WRRC_MSG1_HIGH("RRC Connection Request is sent: %d",rrc_v_300);
              rrc_v_300++;
              rrcrce_substate = RRCRCE_WAIT_FOR_RRC_CONN_SETUP;
            }
            break;


          /* UE failed to camp */
          case RRC_FAILED_TO_CAMP:

            WRRC_MSG1_HIGH("RCE rcvd RDRCT_FAILURE cause %d. Taking UE To IDLE.",
                                            cmd_ptr->cmd.redirect_cnf.cause);
            if(rrccho_is_gtow_pcco_in_progress())
            {
              rrc_conn_setup_ind_type conn_setup_ind_params;
              conn_setup_ind_params.status = FAILURE;
              conn_setup_ind_params.conn_failure_reason = RRC_CONN_FAILURE_OTHER;

              WRRC_MSG1_HIGH("RRC_FAILED_TO_CAMP received when PCCO is active.rrcrce_substate %d",rrcrce_substate);      
              rrcrce_send_conn_setup_ind(conn_setup_ind_params);                  

              /* set rce_substate to initial */
              rrcrce_substate = RRCRCE_INITIAL;
              break;
            }

            /* The agreement is that CSP would tear down RB0 and RCE
               should not send channel config req to tear down RB0 */
            rrc_transition_to_disconnected_state(RRC_PROCEDURE_RCE,RRC_TX_TO_DISCON_OTHER);
            /* Update the release cause */
            rrc_release_cause = RRC_REL_CAUSE_UNSPEC;
      
            /* RRC_ABORT_IND will be sent rrcrce_process_rce_wait_for_rb0_rel_cnf_substate 
             * upon a state change indication */ 
            rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;
            rrcrce_mm_abort_cause = RRC_FAIL_TO_CAMP_ON;
            break;


          /* CSP did not receive inter frequency redirect request in connecting state
           * or RCE sent wrong parameters to CSP or redirection is not supported. 
           * In this case RCE should start wait timer.
           */
          case RRC_RCE_INCOMPATIBLE:
          case RRC_RAT_REDIRECTION_NOT_SUPPORTED:
          case RRC_FREQ_REDIRECTION_NOT_SUPPORTED:
          case RRC_REDIRECTION_NOT_SUPPORTED:                    
            
            WRRC_MSG2_HIGH("RCE rcvd RDRCT_FAILURE cause %d. Strtng wt time %d", cmd_ptr->cmd.redirect_cnf.cause, rrcrce_wait_time);
            
#ifdef FEATURE_DUAL_SIM
            if(wcdma_is_ue_in_dual_standby && ((rrc_establishment_cause == RRC_EST_REGISTRATION) ||
               (rrc_establishment_cause == RRC_EST_CALL_RE_ESTABLISH) ||
               (rrc_establishment_cause == RRC_EST_INTER_RAT_CELL_RESELECT)))
            {
              /* Release the RB 0 and request the RRC LLC to go to Idle disconnected */
              /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
              rrcrce_send_rrc_initiate_cell_selection_ind(
                RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
              /* The RCE Procedure ends */
              rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF; 
  
              /* Set Abort Cause */
              rrcrce_mm_abort_cause = RRC_ABORT_CONN_REJECT_WAIT_TMR;
            }
            else
#endif
            {
               /* Start the timer for wait time. The wait time units are secs */
               rrctmr_start_timer(RRCTMR_WAIT_TIME_TIMER, rrcrce_wait_time * 1000);  
               rrcrce_substate = RRCRCE_WAIT_FOR_WAIT_TIMER_TO_EXPIRE;
            }
            break;

          case RRC_REDIRECT_GSM_FAILURE:
            /* Send Channel config request to LLC asking it to re-setup
             * FACH channels
             */
            WRRC_MSG0_HIGH("Redirect cnf (GSM_FAILURE) from CSP");

            /* Allocates the buffer to RRC Internal command */
              rce_out_cmd_ptr =  rrc_get_int_cmd_buf();
              /* Fill in the command parameters for RRC_CHANNEL_CONFIG_REQ */
              rce_out_cmd_ptr->cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;
              rce_out_cmd_ptr->cmd.chan_config_req.procedure = RRC_PROCEDURE_RCE;
              rce_out_cmd_ptr->cmd.chan_config_req.transaction_id = GET_TRANS_ID();
              rce_chl_config_transaction_id = (uint8) rce_out_cmd_ptr->cmd.chan_config_req.transaction_id;
              WRRC_MSG1_HIGH("Assigned transaction id = %d",rce_chl_config_transaction_id);

              /* Fill in parameters for RRC_CHANNEL_CONFIG_REQ command */
              rce_out_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;
              rce_out_cmd_ptr->cmd.chan_config_req.next_state = RRC_STATE_CONNECTING;
              rce_out_cmd_ptr->cmd.chan_config_req.chan_config_reason 
                                                      = RRCLLC_CHAN_CFG_REASON_PHYCHAN_RESETUP;
              rce_out_cmd_ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = 
                TRUE;

              rce_out_cmd_ptr->cmd.chan_config_req.num_rb_to_config = 0;
              rce_out_cmd_ptr->cmd.chan_config_req.release_phychls  = FALSE;
              rce_out_cmd_ptr->cmd.chan_config_req.release_trchls   = FALSE;

              rrc_put_int_cmd(rce_out_cmd_ptr);         

            rrcrce_substate = RRCRCE_WAIT_FOR_RB0_PHYCHAN_RESETUP_CNF;
            break;

          case RRC_REDIRECT_RESUME_FAILURE:
            /* Trigger OOS handling in Connecting */
            WRRC_MSG0_HIGH("Redirect cnf (RESUME_FAILURE) from CSP");

            /* Trigger Cell Selection */
            rrcrce_send_rrc_initiate_cell_selection_ind(
              RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
            break;

          /* CSP sent wrong failure cause to RCE. Send abort indication to MM */
          default:

            WRRC_MSG1_ERROR("ERR REDIRECT_FAILURE rcvd with incorrect cause %d",cmd_ptr->cmd.redirect_cnf.cause);

            if(rrccho_is_gtow_pcco_in_progress())
            {
              rrc_conn_setup_ind_type conn_setup_ind_params;
              conn_setup_ind_params.status = FAILURE;
              conn_setup_ind_params.conn_failure_reason = RRC_CONN_FAILURE_OTHER;

              WRRC_MSG1_HIGH("REDIRECT_FAIL received when PCCO is active.rrcrce_substate %d",rrcrce_substate);      
              rrcrce_send_conn_setup_ind(conn_setup_ind_params);                  

              /* set rce_substate to initial */
              rrcrce_substate = RRCRCE_INITIAL;
              break;
            }

            /* The agreement is that CSP would tear down RB0 and RCE
             * should not send channel config req to tear down RB0 
             */
            rrc_transition_to_disconnected_state(RRC_PROCEDURE_RCE,RRC_TX_TO_DISCON_OTHER);
            /* Update the release cause */
            rrc_release_cause = RRC_REL_CAUSE_UNSPEC;
      
            /* RRC_ABORT_IND will be sent rrcrce_process_rce_wait_for_rb0_rel_cnf_substate 
             * upon a state change indication */ 
            rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;
            rrcrce_mm_abort_cause = RRC_FAILURE_UNSPECIFIED;
            break;
        }
      }
      break;

    case RRC_NEW_CELL_IND:
      WRRC_MSG0_HIGH("Ignoring New Cell Ind received in RRCRCE_WAIT_FOR_REDIRECT_CNF");
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcrce_substate);
      break;
  }  /* End Switch Statement */


  /* Since RCE receives NEW_CELL_IND before REDIRECT_CNF,
   * memory should not be freed in this case (if memory
   * was allocated)
   */
  if(rrcrce_substate != RRCRCE_WAIT_FOR_REDIRECT_CNF)
  {
    /* Free the memory allocated to the encoded string */
    if(save_dl_ccch_msg.dl_sdu != NULL)
    {
      rrc_free(save_dl_ccch_msg.dl_sdu);
    }

    /* Free the memory allocated to the decoded message */
    if(save_dl_ccch_msg.decoded_msg != NULL)
    {
      (void) rrcasn1_free_pdu((int)save_dl_ccch_msg.dl_sdu_type, save_dl_ccch_msg.decoded_msg);
      save_dl_ccch_msg.decoded_msg = NULL;
    }
  }
}/* rrcrce_process_rce_wait_for_redirect_cnf_substate*/

/*===========================================================================

FUNCTION rrcrce_process_rce_wait_for_wait_time_expire_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Establishment procedure in RRCRCE_WAIT_FOR_WAIT_TIMER_TO_EXPIRE substate of 
  Connecting State
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_wait_time_expire_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  rrc_abort_cause_e_type  cause = RRC_MAX_ACCESS_FAILURE;

  switch(cmd_ptr->cmd_hdr.cmd_id)
  {   
    case RRC_DOWNLINK_SDU_IND:
      /* RRC Connection Setup or RRC Connection Reject may be received in this
         Substate because of cell-reselections. */
      WRRC_MSG0_HIGH("Simultanous transactions not supported");
      break;

    case RRC_STATE_CHANGE_IND:
      /* Stop the timer for wait time */     
       rrctmr_stop_timer(RRCTMR_WAIT_TIME_TIMER);  
   
      if((cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED) &&
         ( rrc_get_state() == RRC_STATE_DISCONNECTED))
      {
        if(rrccho_is_gtow_pcco_in_progress())
        {
          rrc_conn_setup_ind_type conn_setup_ind_params;
          conn_setup_ind_params.status = FAILURE;
          conn_setup_ind_params.conn_failure_reason = RRC_CONN_FAILURE_OTHER;

          WRRC_MSG1_HIGH("RRC_STATE_CHANGE_IND received when PCCO is active.rrcrce_substate %d",rrcrce_substate);      
          rrcrce_send_conn_setup_ind(conn_setup_ind_params);                  
       
          /* set rce_substate to initial */
          rrcrce_substate = RRCRCE_INITIAL;
          break;
        }

        /* Send RRC_ABORT_IND command to the MM */
         rrcrce_send_rrc_abort_ind(RRC_FAILURE_UNSPECIFIED);      
         rrcrce_substate = RRCRCE_INITIAL; 
      }    
      else
      {     
        WRRC_MSG0_HIGH("RRCSCMGR posted an unregistered event");
      }
      break;

    case RRC_ABORT_REQ:
      /* Stop the timer for wait time */     
      rrctmr_stop_timer(RRCTMR_WAIT_TIME_TIMER); 
    
      /* The agreement is that CSP would tear down RB0 and RCE
         should not send channel config req to tear down RB0 */
      rrc_transition_to_disconnected_state(RRC_PROCEDURE_RCE, RRC_TX_TO_DISCON_OTHER);
        /* Update the release cause */
      rrc_release_cause = RRC_REL_DEEP_FADE;
     
      rrcrce_send_abort_cnf = TRUE;
                                 
      /* RRC_ABORT_CNF will be sent rrcrce_process_rce_wait_for_rb0_rel_cnf_substate 
       * upon a state change indication */ 
      rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;
      rrcrce_mm_abort_cause = RRC_FAILURE_UNSPECIFIED;
      break;

    case RRC_WAIT_TIMER_EXPIRED_IND:

      if((RRCRCE_SUCCESS != rrcrce_validate_cell_access_status
                  (rrc_est_cn_domain_id,rrc_establishment_cause,&cause))
#ifdef FEATURE_DUAL_SIM
         || (wcdma_is_ue_in_dual_standby && (TRUE == cm_call_is_there_call_on_other_sub(rrc_get_as_id(),CM_CALL_TYPE_VOICE_MASK | CM_CALL_TYPE_EMERG_MASK)))
#endif
        )
      {
        /* Release RB 0 and request RRC LLC to go to Idle Disconnected */
        /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
        rrcrce_send_rrc_initiate_cell_selection_ind(
          RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
          /* The RCE Procedure ends */
        rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF;  /* Change the RCE substate */
        if(cause  ==  RRC_ACCESS_CLASS_NOT_ALLOWED)
        {
          if(rrc_est_cn_domain_id == RRC_CS_DOMAIN_CN_ID)
          {
            rrcrce_mm_abort_cause = RRC_AC_NOT_ALLOWED_CS_DOMAIN;
          }
          else
          {
            rrcrce_mm_abort_cause = RRC_AC_NOT_ALLOWED_PS_DOMAIN;
          }
        }
        else
        {
          rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE;
        }
      }
      else
      {
        /* Resend RRC connection Request Message and this function restarts the
           T300 Timer */
        rrcrce_send_rrc_connection_request_msg();
        rrcrce_substate = RRCRCE_WAIT_FOR_RRC_CONN_SETUP; 
        /* Increments the re-transmission counter for RRC Connection 
             Request message */ 
        WRRC_MSG1_HIGH("RRC Connection Request is sent: %d",rrc_v_300);
  
        rrc_v_300++;
      }
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcrce_substate);
      break;
  }  
} /*  rrcrce_process_rce_wait_for_wait_time_expire_substate */
/*===========================================================================

FUNCTION rrcrce_send_rrc_channel_config_req_for_srb

DESCRIPTION
  This function builds RRC_CHANNEL_CONFIG_REQ command and  requests RRC LLC
  to either setup or modify or release  Signalling Radio Bearers 1 through 4.
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_rrc_channel_config_req_for_srb
(
               
  rrc_rb_cfg_e_type action,                   /* Type of action on SRBs */
  rrc_state_e_type next_state,                /* Next state to transition is
                                                 expected to take place */
  boolean confirm_required                    /* This indiactes whether
                                                 RRC_CHANNEL_CONFIG_CNF is 
                                                 required or not */
)
{
  rrc_cmd_type *cmd_ptr;                     /* Pointer to the RRC Command */

  uint32 count = 0;                          /* Local variable for count */
  rrc_RB_Identity                rb_id;

  /* Allocates the buffer to RRC Internal command */
  cmd_ptr =  rrc_get_int_cmd_buf();
  {     
    /* Fill in the command parameters for RRC_CHANNEL_CONFIG_REQ */
    cmd_ptr->cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;
    cmd_ptr->cmd.chan_config_req.procedure = RRC_PROCEDURE_RCE;
    cmd_ptr->cmd.chan_config_req.transaction_id = GET_TRANS_ID();
    rce_chl_config_transaction_id = (uint8) cmd_ptr->cmd.chan_config_req.transaction_id;

    /* Fill in parameters for RRC_CHANNEL_CONFIG_REQ command */
    cmd_ptr->cmd.chan_config_req.rrc_state_change_required = TRUE;
    cmd_ptr->cmd.chan_config_req.next_state = next_state;
    /* The rrcrce_validate_rrc_connection_setup_msg function would
       have captured if number of rbs is equal other than 3 or 4 */
    cmd_ptr->cmd.chan_config_req.num_rb_to_config = (int32 ) 
      rrcrce_number_of_rbs;


    cmd_ptr->cmd.chan_config_req.rb[count].rb_id = DCCH_UM_RADIO_BEARER_ID;
    cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = action;

    cmd_ptr->cmd.chan_config_req.rb[count].rb_id = DCCH_AM_RADIO_BEARER_ID;
    cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = action;

    cmd_ptr->cmd.chan_config_req.rb[count].rb_id = DCCH_DT_HIGH_PRI_RADIO_BEARER_ID;
    cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = action;

    cmd_ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = 
      confirm_required;
    WRRC_MSG2_HIGH("No. of RBs in Chan config Req = %d, transaction id: %d",rrcrce_number_of_rbs, rce_chl_config_transaction_id);
    
    if(ordered_config_ptr->srb5_info.srb5_rb_exists  == TRUE)
    {
      if(rrcrce_number_of_rbs == 5)
      {
        cmd_ptr->cmd.chan_config_req.rb[count].rb_id = ordered_config_ptr->srb5_info.rb_id;
        cmd_ptr->cmd.chan_config_req.rb[count++].rb_config = action;
  
        cmd_ptr->cmd.chan_config_req.rb[count].rb_id = DCCH_DT_LOW_PRI_RADIO_BEARER_ID;
        cmd_ptr->cmd.chan_config_req.rb[count].rb_config = action;
      }
      else
      {
        cmd_ptr->cmd.chan_config_req.rb[count].rb_id = ordered_config_ptr->srb5_info.rb_id;
        cmd_ptr->cmd.chan_config_req.rb[count].rb_config = action;
      }
    }
    else 
    {
      if (rrcrce_number_of_rbs == 4)
      {
        cmd_ptr->cmd.chan_config_req.rb[count].rb_id = DCCH_DT_LOW_PRI_RADIO_BEARER_ID;
        cmd_ptr->cmd.chan_config_req.rb[count].rb_config = action;
      }
    }
 
    if(rrclcm_get_rb_id_if_dl_lc_type_exists(UE_LOGCHAN_TM_DCCH_DL, &rb_id) == RRCLCM_SUCCESS)
    {
      /* Though this a generic function for RCE, RCE can only
         release SRB#5 only during NAS abort cases */
      if(action == RELEASE_RB)
      {
        cmd_ptr->cmd.chan_config_req.rb[++count].rb_id = rb_id;
        cmd_ptr->cmd.chan_config_req.rb[count].rb_config = action;
        cmd_ptr->cmd.chan_config_req.num_rb_to_config++;
      }
      else
      {
        WRRC_MSG0_ERROR("RCE trying to do other than releasing SRB#5 ... something wrong");
        /* Anyway continue */
      }
    }
 
    rrc_put_int_cmd(cmd_ptr);                 /* sends the command to RRC
                                                 LLC. */
  }
}/*rrcrce_send_rrc_channel_config_req_for_srb */
/*===========================================================================

FUNCTION rrcrce_send_cell_update_initiate_req

DESCRIPTION
  This function send the cell update initiate request if UE does not have
  a valid C-RNTI in CELL_FACH state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcrce_send_cell_update_initiate_req
(
  rrc_proc_e_type procedure
)
{
  rrc_cmd_type *int_cmd_ptr;
  //This initiates a CELL Update procedure and enters the substate
  //RRCRCE_WAIT_CU_COMPLETE_CNF. 
  int_cmd_ptr =  rrc_get_int_cmd_buf();
  /* Fill in the command parameters for RRC_CELL_UPDATE_INITIATE_REQ */
  int_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_UPDATE_INITIATE_REQ;
  int_cmd_ptr->cmd.initiate_cell_update.cause = RRC_CELL_RESELECTION;
  int_cmd_ptr->cmd.initiate_cell_update.cu_complete_cnf_required = TRUE;
  int_cmd_ptr->cmd.initiate_cell_update.procedure = procedure;
  rrc_put_int_cmd(int_cmd_ptr);             
}/*rrcrce_send_cell_update_initiate_req*/
/*===========================================================================
FUNCTION rrcrce_process_rce_wait_for_srb_setup_cnf_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Establishment procedure in RRCRCE_WAIT_FOR_SRB_SETUP_CNF sub state of 
  Connecting State
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_srb_setup_cnf_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{ 
 
  rrc_N_300 rrcrce_n_300;                    /* Local Variable to store N300 */

  switch (cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_DOWNLINK_SDU_IND:
      /* RRC Connection Setup or RRC Connection Reject may be received in this
         Substate because of cell-reselections. */
      WRRC_MSG0_HIGH("Simultanous transactions not supported");
      break;

    case RRC_STATE_CHANGE_IND:
      if((cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED) &&
         ( rrc_get_state() == RRC_STATE_DISCONNECTED))
      {
        /* Send RRC_ABORT_IND command to the MM */
        rrcrce_send_rrc_abort_ind(RRC_FAILURE_UNSPECIFIED);     
        rrcrce_substate = RRCRCE_INITIAL; 
      }    
      break;

    case RRC_ABORT_REQ:
      /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
      rrcrce_send_rrc_initiate_cell_selection_ind(
        RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
      /* Wait for successful release of SRBs*/
      rrcrce_substate = RRCRCR_WAIT_FOR_SRB_REL_CNF;
      rrcrce_send_abort_cnf = TRUE;
      rce_chl_config_transaction_id = 0;
      break;

    case RRC_NEW_CELL_IND:          
      /* The UE is already in CONNECTING State. The CCM should have reconfigured
          the  RB 0*/
      /* Gets N300 from the SIB 1 database */
      if( RRCRCE_SUCCESS != 
          rrcrce_get_n300_from_serv_cell_db(&rrcrce_n_300))
      {
        ERR_FATAL("N300 value is invalid", rrcrce_n_300,0,0);
      }
      if(rrc_v_300 <= rrcrce_n_300)               
      {
        /* Resend RRC connection Request Message and this function restarts the
           T300 Timer */
        rrcrce_send_rrc_connection_request_msg();
        rrcrce_substate = RRCRCE_WAIT_FOR_RRC_CONN_SETUP; 
        /* Increments the re-transmission counter for RRC Connection Request
             Message */ 
        WRRC_MSG1_HIGH("RRC Connection Request is sent: %d",rrc_v_300);

        rrc_v_300++;     
      }
      else /* Reached maximum allowed re-tries */
      { 
        /* Send RRC_ABORT_IND command to MM */
        rrcrce_send_rrc_abort_ind(RRC_MAX_ACCESS_FAILURE);     
        rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                (uint8) RRC_PROCEDURE_RCE,
                                (uint8) RRCLOG_RachFailure,
                                (uint8) RRCLOG_PROTOCOL_ERROR_NA
                              );  
        rrcrce_substate = RRCRCE_INITIAL;         /* reset the RCE substate */
      }    
      break;

    case RRC_CHANNEL_CONFIG_CNF:
      if(cmd_ptr->cmd.chan_config_cnf.procedure == RRC_PROCEDURE_RCE)
      {
        rrcllcpcie_initialise_sync_a_post_veri_info();

        if(cmd_ptr->cmd.chan_config_cnf.transaction_id == rce_chl_config_transaction_id)
        {
          if( RRCLLC_CHAN_CFG_SUCCESS == cmd_ptr->cmd.chan_config_cnf.chan_cfg_status ) 
          { 
            /* Copy the Ordered RB Mapping into the current RB-Mapping database*/
            rrcllc_copy_ordered_hanging_rb_mapping_to_current();

    
#ifdef FEATURE_WCDMA_REL7_SPECRS
             rrc_save_rnc_cap_change_support();
#endif

            switch(rrc_translate_state_indicator_to_state(rrcrce_state_indicator))           
            {
              case RRC_STATE_CELL_FACH:
                /* Query for valid C-RNTI */
                if (C_RNTI_NOT_VALID == rrcllc_get_current_crnti_status() )
                {
                  /* Send RRC_CELL_UPDATE_INITIATE_REQ command to CELL UPDATE Proc */
                  rrcrce_send_cell_update_initiate_req(RRC_PROCEDURE_RCE);
                  /* Wait for CELL UPDATE to get a valid C_RNTI */
                  rrcrce_substate = RRCRCE_WAIT_FOR_CU_COMPLETE_CNF;
                }
                else
                {
                  /* Send RRC Connection Setup Complete message */
                  if (RRCRCE_SUCCESS != 
                    rrcrce_send_rrc_connection_setup_complete_message())
                  {
                    ERR_FATAL("Unable to send RRC Connection Complete Msg",0,0,0);
                  }
                  else
                  {
                    /* Start the timer for L2 ACK */
                    /* rrctmr_start_timer(RRCTMR_RCE_L2ACK_TIMER, 
                    RRCRCE_TIMER_VALUE_FOR_L2_ACK); */

                    WRRC_MSG0_HIGH("RRC Conn Setup Compl Msg is sent");        
                    rrcllc_print_trans_chnl_rate();
                    /* Clear the Ordered Config */
                    rrcllc_clear_ordered_config();

                    /* Update the necesary parameters here*/
                    rrcsmc_reset_smc_ip_started_flag();

                    rrccu_start_t_305_timer();

                    /* Send RRC_EST_CNF command to MM */
                    rrcrce_send_rrc_est_cnf();     
                    /* The RCE Procedure goes to FINAL substate */
                    rrcrce_substate = RRCRCE_FINAL;   

                    /* Reset the rrcrce_num_retries_count */
                    rrcrce_num_retries_count = 0;

                    /* Initial release cause to Unspecified */
                    rrc_release_cause = RRC_REL_CAUSE_UNSPEC;
                  }    
                }
                break;

              case RRC_STATE_CELL_DCH:
            
                /*Notify the Cell_Id to registered entities*/
                rrc_notify_cell_id_in_dch();                    
                /* Send RRC Connection Setup Complete message */
                if (RRCRCE_SUCCESS != 
                  rrcrce_send_rrc_connection_setup_complete_message())
                {
                  ERR_FATAL("Unable to send RRC Connection Complete Msg",0,0,0);
                }
                else
                {
                /* Start the timer for L2 ACK 
                rrctmr_start_timer(RRCTMR_RCE_L2ACK_TIMER, 
                  RRCRCE_TIMER_VALUE_FOR_L2_ACK); */

                  WRRC_MSG0_HIGH("RRC Conn Setup Compl Msg is sent");        
                  rrcllc_print_trans_chnl_rate();
                  /* Clear the Ordered Config */
                  rrcllc_clear_ordered_config();

                  /* Update the necesary parameters here*/
                  rrcsmc_reset_smc_ip_started_flag();

                  /* Send RRC_EST_CNF command to MM */
                  rrcrce_send_rrc_est_cnf();      
                  /* The RCE Procedure goes to FINAL substate */
                  rrcrce_substate = RRCRCE_FINAL;   

                  /* Reset the rrcrce_num_retries_count */
                  rrcrce_num_retries_count = 0;

                  /* Initial release cause to Unspecified */
                  rrc_release_cause = RRC_REL_CAUSE_UNSPEC;
                }    
                break;

              default:
                ERR_FATAL(" RRC entered into invalid state :%d",
                            rrcrce_state_indicator,0,0);
                break;
            }

          }
          else /* Physical Channel Setup Failure , Retry again */
          {  

#ifndef FEATURE_WCDMA_HS_FACH
            /* Channel config failure, so initialise the ordered RB-Mapping database*/
            rrcllc_init_ordered_hanging_rb_mapping_info();
#else
            rrcllc_copy_current_hanging_rb_mapping_to_ordered();
#endif


            (void)rrcrce_get_n300_from_serv_cell_db(&rrcrce_n_300);
            if(rrc_v_300 <= rrcrce_n_300)
            {  

              if (rrcrce_state_indicator == rrc_RRC_StateIndicator_cell_DCH && rrc_get_state() == RRC_STATE_CONNECTING)
              { 
                /* reset the ordered_config */
                if (OC_NOT_SET != rrcllc_reset_ordered_config(RRC_PROCEDURE_RCE
                                                              ,TRUE
                                                              ))
                {
                  /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
                  WRRC_MSG0_ERROR("OC Reset Fail: Send Init Cell Sel Ind to CSP");    
                  rrcrce_send_rrc_initiate_cell_selection_ind(
                     RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                  /* Change the RCE state machine */         
                  rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF; 
                  /* Set Abort Cause */
                  rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE;     
                }    
                else
                {
                  /* start phy chan failure timer */
                  rrctmr_start_timer(RRCTMR_PHY_CHAN_FAILURE_TIMER, RRCTMR_PHYCHANFAILURE_TIMER_IN_MS);     
                  /* Initiate Cell Selection and substate should be RRCRCE_WAIT_FOR_RESELCT_COMPL */
                  rrcrce_send_rrc_initiate_cell_selection_ind(
                    RRC_OUT_OF_SERVICE_AREA_IND,rrc_get_state());
                  rrcrce_substate = RRCRCE_WAIT_FOR_RESELCT_COMPL;
                  WRRC_MSG0_HIGH("PhyChanSetupFail rcvd.Initiating cell sel");
                }
              }
              else
              {
#ifdef FEATURE_WCDMA_HS_RACH
                if ((rrchsrach_camped_cell_supports_hsrach() == TRUE) && (rrcrce_state_indicator != rrc_RRC_StateIndicator_cell_DCH) && 
                    (rrc_get_state() == RRC_STATE_CONNECTING))
                {
                  /* reset the ordered_config */
                  if (OC_NOT_SET != rrcllc_reset_ordered_config(RRC_PROCEDURE_RCE,TRUE))
                  {
                    WRRC_MSG0_ERROR("Could not Reset OC");
                    /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
                    rrcrce_send_rrc_initiate_cell_selection_ind(
                      RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                    /* Change the RCE state machine */		   
                    rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF; 
                    /* Set Abort Cause */
                    rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE; 	
                  }    
              }
#endif
                rrcrce_send_rrc_connection_request_msg();
                rrcrce_substate = RRCRCE_WAIT_FOR_RRC_CONN_SETUP; 
                /* Increments the re-transmission counter for RRC Connection
                   Request Message */
                WRRC_MSG1_HIGH("RRC Connection Request is sent: %d",rrc_v_300);

                rrc_v_300++;
              }
            }
            else  /* Max re-tries exceeded */
            {
              rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                      (uint8) RRC_PROCEDURE_RCE,
                                      (uint8) RRCLOG_RachFailure,
                                      (uint8) RRCLOG_PROTOCOL_ERROR_NA
                                    );
              /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
               rrcrce_send_rrc_initiate_cell_selection_ind(
                  RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
                /* Change the RCE state machine */         
                rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF; 

                /* Set Abort Cause */
                rrcrce_mm_abort_cause = RRC_MAX_ACCESS_FAILURE;     
                rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                        (uint8) RRC_PROCEDURE_RCE,
                                        (uint8) RRCLOG_RachFailure,
                                        (uint8) RRCLOG_PROTOCOL_ERROR_NA
                                      );   
              }    
          } /* Physical Channel Setup Failure */
        }
        else
        {
          WRRC_MSG2_HIGH("Ignoring tr_id %d in chl config cnf. Expected tr_id = %d", 
                     cmd_ptr->cmd.chan_config_cnf.transaction_id,
                      rce_chl_config_transaction_id);
        }
      }
      else
      {
        WRRC_MSG1_HIGH("RRC_CHANNEL_CONFIG_CNF's proc: %d",
                  cmd_ptr->cmd.chan_config_cnf.procedure);
      }
      break;
   
    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id, rrcrce_substate);
      break;

   }  
} /* rrcrce_process_rce_wait_for_srb_setup_cnf_substate*/

/*===========================================================================
FUNCTION rrcrce_process_rce_wait_for_cu_complete_cnf_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Establishment procedure in RRCRCE_WAIT__FOR_CU_COMPLETE_CNF sub state of 
  CELL_FACH State
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_cu_complete_cnf_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{ 
  switch (cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_DOWNLINK_SDU_IND:
      /* RRC Connection Setup or RRC Connection Reject may be received in this
         Substate because of cell-reselections. */
      WRRC_MSG0_HIGH("Simultanous transactions not supported");
      break;

    case RRC_STATE_CHANGE_IND:
      if((cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED) &&
         ( rrc_get_state() == RRC_STATE_DISCONNECTED))
      {
        /* Send RRC_ABORT_IND command to the MM */
        rrcrce_send_rrc_abort_ind(RRC_FAILURE_UNSPECIFIED);    
        {        
           /* Reset RCE Procedure and  Procedure ends */
           rrcrce_substate = RRCRCE_INITIAL; 
        }    
      }
      break;

    case RRC_ABORT_REQ:
      /* Send command to LLC to release SRBs */
      /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
      rrcrce_send_rrc_initiate_cell_selection_ind(
         RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
        /* Wait for successful release of SRBs */
      rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF; //RRCRCR_WAIT_FOR_SRB_REL_CNF;    

      rrcrce_send_abort_cnf = TRUE;
      
      break;

    case RRC_NEW_CELL_IND:          
      /* The UE is already in CELL_FACH State. So Ignore the command since Cell
         Update would have been initiated */
      MSG_MED("Unexpected event:%d in the substate: %d", 
               cmd_ptr->cmd_hdr.cmd_id, rrcrce_substate,0); 
      break;

    case RRC_CELL_UPDATE_COMPLETE_CNF:
      if(cmd_ptr->cmd.cu_complete_cnf.procedure == RRC_PROCEDURE_RCE)
      {
        if (RRCRCE_SUCCESS != 
          rrcrce_send_rrc_connection_setup_complete_message())
        {
          ERR_FATAL("Unable to send RRC Connection Complete Msg",0,0,0);
        }
        else
        {
        /* Start the timer for L2 ACK 
        rrctmr_start_timer(RRCTMR_RCE_L2ACK_TIMER, 
          RRCRCE_TIMER_VALUE_FOR_L2_ACK); */
          
          WRRC_MSG0_HIGH("RRC Conn Setup Compl Msg is sent");        
          rrcllc_print_trans_chnl_rate();
          /* Clear the Ordered Config */
          rrcllc_clear_ordered_config();
          
          /* Update the necesary parameters here*/
          rrcsmc_reset_smc_ip_started_flag();
          
          rrccu_start_t_305_timer();
          
          /* Send RRC_EST_CNF command to MM */
          rrcrce_send_rrc_est_cnf();    
          /* The RCE Procedure goes to FINAL substate */
          rrcrce_substate = RRCRCE_FINAL;   
          
          /* Reset the rrcrce_num_retries_count */
          WRRC_MSG0_HIGH("Setting rrcrce_num_retries_count to 0");
          rrcrce_num_retries_count = 0;
          
          /* Initial release cause to Unspecified */
          rrc_release_cause = RRC_REL_CAUSE_UNSPEC;
        }    
      }
      else
      {
        WRRC_MSG1_HIGH("RRC_CELL_UPDATE_COMPLETE_CNF's proc: rrc_proc_e_type_value%d",
          cmd_ptr->cmd.cu_complete_cnf.procedure);
      }
      
      break;
   
    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id, rrcrce_substate);
      break;

   }  
} /* rrcrce_process_rce_wait_for_cu_complete_cnf_substate*/

/*===========================================================================
FUNCTION rrcrce_send_rrc_connection_setup_complete_message

DESCRIPTION
  This function builds the RRC CONNECTION SETUP COMPLETE message and calls
  the Send Chain to transmit the message.
DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type : It returns RRCRCE_SUCCESS if it successfully builds 
  the RRC CONNECTION SETUP COMPLETE message and calls the Send Chain. 
  Otherwise it returns RRCRCE_FAILURE_MAJOR.

SIDE EFFECTS
  None
===========================================================================*/
rrcrce_status_e_type rrcrce_send_rrc_connection_setup_complete_message
(
  void
)
{
  rrcrce_status_e_type status = RRCRCE_FAILURE_MAJOR;
  rrc_UL_DCCH_Message *msg_ptr;             /* Pointer to uplink DCCH message*/
  uint32 num_rat=0;
  rlc_lc_id_type ul_lc_id;                  /* Logical Channel Id */
  boolean ret_flag = FALSE;
  rrc_msg_ver_enum_type msg_version = MSG_REL5; 

  rrc_UE_RadioAccessCapability_v650ext local_v650ext;     /* Local copy of v650extensions */

  boolean support_ho_to_gan = FALSE;
  rrc_SysInfoType3 *sib3_ptr = (rrc_SysInfoType3*)rrc_sibdb_return_sib_for_srv_cell(rrc_SIB3);
#ifdef FEATURE_WCDMA_MIMO
  boolean support_tx_diversity_mimo = FALSE;
  if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
  {
    support_tx_diversity_mimo = TRUE;
  }
#endif
  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
  {
    msg_version = MSG_REL5;
#ifdef FEATURE_GAN_3G
    #error code not present
#endif
  }
  else
  {
    msg_version = MSG_REL99;
  }

  /* To satisfy Compiler/LINT to not generate warning */
  /*lint -e774*/
  if ((msg_version == MSG_REL5) || (ret_flag == TRUE))
  {
    MSG_LOW("Msg version is %d, Ret Flag %d", msg_version,ret_flag,0);
  }
  /*lint +e774*/

  /* Checks whether Logical Channel entity setup for DCCH logical
     Channel type */
  ul_lc_id = rrclcm_check_ul_rlc_lc_id ( UE_LOGCHAN_DCCH,
                                         DCCH_AM_RADIO_BEARER_ID,
                                         UE_MODE_ACKNOWLEDGED_DATA);

  /* If RLC Logical Channel Id is already setup, the send RRC Connection
     Request message through SEND_CHAIN */
  if(ul_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    /* Allocate a buffer for an uplink DCCH Message. This buffer should be released
       by SEND_CHAIN using OSS compiler functions */
    msg_ptr = (rrc_UL_DCCH_Message *) rrc_malloc ( 
                                               sizeof(struct rrc_UL_DCCH_Message));

    /* Select the message as RRC CONNECTION SETUP COMPLETE message */
    msg_ptr->message.t = T_rrc_UL_DCCH_MessageType_rrcConnectionSetupComplete;

    /* set the Transaction id */
    msg_ptr->message.u.rrcConnectionSetupComplete.rrc_TransactionIdentifier =
      rrcrce_setup_transaction_identifier;
   
    /* Appends START list to the RRC CONNECTION SETUP COMPLETE message */
    (void)rrcsmc_append_start_list(
            &msg_ptr->message.u.rrcConnectionSetupComplete.startList,
            RRCSMC_APPEND_START);
 
    if (rrc_capability_update_requirement.
        ue_radio_capability_update_requirement == TRUE) 
    { 
      RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetupComplete,
        rrc_RRCConnectionSetupComplete,ue_RadioAccessCapability);

      /* Initialize the v650 ext before populating it */
      RRC_RESET_MSG_IE_PRESENT(local_v650ext);
      local_v650ext.ue_RadioAccessCapabBandFDDList2.n = 0;
      local_v650ext.ue_RadioAccessCapabBandFDDList_ext.n = 0;

      ret_flag = rrcueci_append_ue_radio_access_capability(
                   &msg_ptr->message.u.rrcConnectionSetupComplete.ue_RadioAccessCapability,
                   &msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
                     rrcConnectionSetupComplete_v370ext.ue_RadioAccessCapability_v370ext,
                   &local_v650ext,
                   RRCUECI_CAP_UPDATE);

      // Set rrcConnectionSetupComplete.bit_mask to include v370NonCriticalExtensions
      RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetupComplete,
          rrc_RRCConnectionSetupComplete,v370NonCriticalExtensions);

      // Initialize rrcConnectionSetupComplete_v370ext.bit_mask
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
          rrcConnectionSetupComplete_v370ext);

      // Include rrc_ue_RadioAccessCapability_v370ext if the above function returned TRUE 
      if(TRUE == ret_flag)
      {
        RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
        rrcConnectionSetupComplete_v370ext,
        ue_RadioAccessCapability_v370ext);
      }

      // Set v370NonCriticalExtensions.bit_mask to include v380NonCriticalExtensions
      msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.m.
      v380NonCriticalExtensionsPresent =1;
        
      // Set rrcConnectionSetupComplete_v380ext.bit_mask to include RadioAccessCapability_v380ext
      RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.v380NonCriticalExtensions.
      rrcConnectionSetupComplete_v380ext,ue_RadioAccessCapability_v380ext);
      
      // Set rx_tx_TimeDifferenceType2Capable to FALSE. Revisit this section when UE supports this.
      msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.v380NonCriticalExtensions.
      rrcConnectionSetupComplete_v380ext.ue_RadioAccessCapability_v380ext.
      ue_PositioningCapabilityExt_v380.rx_tx_TimeDifferenceType2Capable = FALSE;

      // Initialize bit_mask of dl_PhysChCapabilityFDD_v380ext 
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.v380NonCriticalExtensions.
      rrcConnectionSetupComplete_v380ext.dl_PhysChCapabilityFDD_v380ext);

      // Initialize v380NonCriticalExtensions.bit_mask 
      RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
      v380NonCriticalExtensions);
  
      /* Append FDD2 list if present */
      MSG_HIGH("RCE FDD2 Status:%d, NV Release:%d, HO_GAN Status:%d",
                  (local_v650ext.ue_RadioAccessCapabBandFDDList2.n != 0 ? 1:0),
                  rrc_nv_rel_indicator,
                  support_ho_to_gan);

      if((local_v650ext.ue_RadioAccessCapabBandFDDList2.n != 0) || 
         ((msg_version == MSG_REL5) && (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5))
         )
      {
        
        /* Select v3a0NonCriticalExtensions on v380NonCriticalExtensions */
        msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
        v380NonCriticalExtensions.m.v3a0NonCriticalExtensionsPresent =1;

        RRC_RESET_MSG_IE_PRESENT( msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
        v380NonCriticalExtensions.v3a0NonCriticalExtensions);
        /* Select laterNonCriticalExtensions on v3a0NonCriticalExtensions */
        msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
        v380NonCriticalExtensions.v3a0NonCriticalExtensions.m.laterNonCriticalExtensionsPresent =1 ;

        /* Set laterNonCriticalExtensions bitmask to 0 */
        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
        v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions);

        /* Reset bit_mask for rrcConnectionSetupComplete_v3a0ext. */
        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
        v380NonCriticalExtensions.v3a0NonCriticalExtensions.rrcConnectionSetupComplete_v3a0ext);

        /* Append FDD2 list if present */
        if((local_v650ext.ue_RadioAccessCapabBandFDDList2.n != 0) || (support_ho_to_gan)
          || (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
          )
        {

          WRRC_MSG0_HIGH("Populating r3_add_ext in RRCConnSetupComplete message");

          /* Set the bitmask for r3_add_ext */
         RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
          v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions,
           rrcConnectionSetupComplete_r3_add_ext);

    
          
            RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            rrcConnectionSetupComplete_r3_add_ext);

          if(local_v650ext.ue_RadioAccessCapabBandFDDList2.n != 0)
          {
            WRRC_MSG0_HIGH("Populating Fdd2");
            
            /* rrc_rrcConnectionSetupComplete_v650ext_present */
            RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            rrcConnectionSetupComplete_r3_add_ext,
            rrcConnectionSetupComplete_v650ext);
          
            /* Assign the populated local structure to the OTA msg structure */
            msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            rrcConnectionSetupComplete_r3_add_ext.rrcConnectionSetupComplete_v650ext.
            ue_RadioAccessCapability_v650ext = local_v650ext;
          }
          
          if(support_ho_to_gan)
          {
            WRRC_MSG0_HIGH("Populating r3_add_ext in  for GAN HO support");
            
            RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            rrcConnectionSetupComplete_r3_add_ext,
              rrc_RRCConnectionSetupComplete_r3_add_ext_IEs,v680NonCriticalExtensions);
  
            RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            rrcConnectionSetupComplete_r3_add_ext.v680NonCriticalExtensions);
  
            RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            rrcConnectionSetupComplete_r3_add_ext.v680NonCriticalExtensions.
            rrcConnectionSetupComplete_v680ext.ue_RadioAccessCapability_v680ext.
            multiModeRAT_Capability_v680ext,supportOfHandoverToGAN);
  
            msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            rrcConnectionSetupComplete_r3_add_ext.v680NonCriticalExtensions.
            rrcConnectionSetupComplete_v680ext.ue_RadioAccessCapability_v680ext.
            multiModeRAT_Capability_v680ext.supportOfHandoverToGAN = 
            rrc_MultiModeRAT_Capability_v680ext_supportOfHandoverToGAN_doesSupportHandoverToGAN;
          }

          if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
          {
            if(!(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              rrcConnectionSetupComplete_r3_add_ext.m.v680NonCriticalExtensionsPresent))
            {
              msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              rrcConnectionSetupComplete_r3_add_ext.m.v680NonCriticalExtensionsPresent = 1;
    
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              rrcConnectionSetupComplete_r3_add_ext.v680NonCriticalExtensions);      
  
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              rrcConnectionSetupComplete_r3_add_ext.v680NonCriticalExtensions.
              rrcConnectionSetupComplete_v680ext.ue_RadioAccessCapability_v680ext.
              multiModeRAT_Capability_v680ext);
            }
  
            msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            rrcConnectionSetupComplete_r3_add_ext.v680NonCriticalExtensions.m.v7e0NonCriticalExtensionsPresent = 1;
  
            RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              rrcConnectionSetupComplete_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions);
  
            RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              rrcConnectionSetupComplete_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
              rrcConnectionSetupComplete_v7e0ext.ue_RadioAccessCapability);
 
            msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              rrcConnectionSetupComplete_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
              rrcConnectionSetupComplete_v7e0ext.ue_RadioAccessCapability.m.supportForTwoDRXSchemesInPCHPresent = 1;

            msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              rrcConnectionSetupComplete_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
              rrcConnectionSetupComplete_v7e0ext.ue_RadioAccessCapability.supportForTwoDRXSchemesInPCH = 
                rrc_UE_RadioAccessCapability_v7e0ext_supportForTwoDRXSchemesInPCH_true;

            WRRC_MSG0_HIGH("Indicate supportForTwoDRXSchemesInPCH");

            msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              rrcConnectionSetupComplete_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
              rrcConnectionSetupComplete_v7e0ext.ue_RadioAccessCapability.m.supportEDPDCHPowerInterpolationPresent = 1;
 
            msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              rrcConnectionSetupComplete_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
              rrcConnectionSetupComplete_v7e0ext.ue_RadioAccessCapability.supportEDPDCHPowerInterpolation = 
                rrc_UE_RadioAccessCapability_v7e0ext_supportEDPDCHPowerInterpolation_true;

            WRRC_MSG0_HIGH("Indicate supportEDPDCHPowerInterpolation");
#ifdef FEATURE_WCDMA_MIMO
            if(support_tx_diversity_mimo)
            {
              msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              rrcConnectionSetupComplete_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.m.v7f0NonCriticalExtensionsPresent = 1;
    
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              rrcConnectionSetupComplete_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.v7f0NonCriticalExtensions);
    
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              rrcConnectionSetupComplete_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
              v7f0NonCriticalExtensions.rrcConnectionSetupComplete_v7f0ext);
    
              msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              rrcConnectionSetupComplete_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
              v7f0NonCriticalExtensions.rrcConnectionSetupComplete_v7f0ext.m.ue_RadioAccessCapabilityPresent = 1;
    
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              rrcConnectionSetupComplete_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
              v7f0NonCriticalExtensions.rrcConnectionSetupComplete_v7f0ext.ue_RadioAccessCapability);
    
              msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              rrcConnectionSetupComplete_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
              v7f0NonCriticalExtensions.rrcConnectionSetupComplete_v7f0ext.ue_RadioAccessCapability.m.supportofTxDivOnNonMIMOChannelPresent = 1;
    
              msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
              v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
              rrcConnectionSetupComplete_r3_add_ext.v680NonCriticalExtensions.v7e0NonCriticalExtensions.
              v7f0NonCriticalExtensions.rrcConnectionSetupComplete_v7f0ext.ue_RadioAccessCapability.supportofTxDivOnNonMIMOChannel = 
              rrc_UE_RadioAccessCapability_v7f0ext_supportofTxDivOnNonMIMOChannel_true;
    
              WRRC_MSG0_HIGH("MIMO:Sending Tx Diversity for DL Control channels");
            }
#endif			
          }
        }
      }
      
      if ((msg_version == MSG_REL5) &&
          (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5))
      {
        WRRC_MSG0_HIGH("UE Cap REL5");

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
        if(rrc_ue_pos_cap_pch)
        {
          RRC_MSG_COMMON_SET_BITMASK_IE_TYPE2(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.v380NonCriticalExtensions.
            v3a0NonCriticalExtensions.rrcConnectionSetupComplete_v3a0ext, ue_RadioAccessCapability_v3a0ext);
          msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.v380NonCriticalExtensions.
            v3a0NonCriticalExtensions.rrcConnectionSetupComplete_v3a0ext.ue_RadioAccessCapability_v3a0ext.ue_PositioningCapabilityExt_v3a0.
            validity_CellPCH_UraPCH = rrc_UE_PositioningCapabilityExt_v3a0_validity_CellPCH_UraPCH_true;
          WRRC_MSG0_HIGH("RRCGPS: PCH validity set");
        }
#endif

        /* Select v3g0NonCriticalExtensions on laterNonCriticalExtensions */
        msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
          v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.m.
         v3g0NonCriticalExtensionsPresent =1 ;

        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
          v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          v3g0NonCriticalExtensions);

        /* No MP filed on laterNonCriticalExtensions */

        /* Select v4b0NonCriticalExtensions on v3g0NonCriticalExtensions */
        msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
          v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          v3g0NonCriticalExtensions.m.v4b0NonCriticalExtensionsPresent =1;

        /* Reset bit_mask of rrcConnectionSetupComplete_v3g0ext. */
        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
          v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          v3g0NonCriticalExtensions.rrcConnectionSetupComplete_v3g0ext);

        /* Select v590NonCriticalExtensions on v4b0NonCriticalExtensions */
        msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
          v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent =1;

        
        /* Set v4b0ext bit_mask to rrc_ue_RadioAccessCapability_v4b0ext_present */
        msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
          v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.rrcConnectionSetupComplete_v4b0ext.
          m.ue_RadioAccessCapability_v4b0extPresent =1;

        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
          v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.rrcConnectionSetupComplete_v4b0ext.
          ue_RadioAccessCapability_v4b0ext);

        msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
          v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.rrcConnectionSetupComplete_v4b0ext.
          ue_RadioAccessCapability_v4b0ext.pdcp_Capability_r4_ext.supportForRfc3095.t
          = T_rrc_PDCP_Capability_r4_ext_supportForRfc3095_notSupported;

#ifdef FEATURE_UMTS_PDCP
        if (rrc_pdcp_enabled)
        { 
          msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.rrcConnectionSetupComplete_v4b0ext.
            ue_RadioAccessCapability_v4b0ext.pdcp_Capability_r4_ext.supportForRfc3095.t = 
            T_rrc_PDCP_Capability_r4_ext_supportForRfc3095_supported;
          
          msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.rrcConnectionSetupComplete_v4b0ext.
            ue_RadioAccessCapability_v4b0ext.pdcp_Capability_r4_ext.supportForRfc3095.u.supported=
            rtxMemAllocTypeZ(&enc_ctxt, rrc_PDCP_Capability_r4_ext_supported );          
          
          
          
          msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.rrcConnectionSetupComplete_v4b0ext.
            ue_RadioAccessCapability_v4b0ext.pdcp_Capability_r4_ext.supportForRfc3095.u.supported->maxROHC_ContextSessions = 
            rrc_MaxROHC_ContextSessions_r4_s4;
    
          
          msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.rrcConnectionSetupComplete_v4b0ext.
            ue_RadioAccessCapability_v4b0ext.pdcp_Capability_r4_ext.supportForRfc3095.u.
            supported->reverseCompressionDepth =0;
        }
#endif /* FEATURE_UMTS_PDCP */

        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
                  v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
                  v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
                  rrcConnectionSetupComplete_v590ext);



        /* Select RadioAccessCapability_v590ext on rrcConnectionSetupComplete_v590ext */
        RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
          v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
          rrcConnectionSetupComplete_v590ext,
          rrc_RRCConnectionSetupComplete_v590ext_IEs,
          ue_RadioAccessCapability_v590ext);

        /* No v590 noncritical extension */
        RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
          v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
          v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions);

        if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6)
        {
          /* v590 noncritical extension present */
          msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.
            v590NonCriticalExtensions.m.v5c0NonCriticalExtensionsPresent =1;
  
          RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
            v5c0NonCriticalExtensions.rrcConnectionSetupComplete_v5c0ext);
  
          /* v5c0 noncritical extension present */
          msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
            v5c0NonCriticalExtensions.m.v690NonCriticalExtensionsPresent =1;
  
          /* No v690 noncritical extension */
          RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
            v5c0NonCriticalExtensions.v690NonCriticalExtensions);
  
          /* Indicate rrc_RRCConnectionSetupComplete_v690ext_IEs_ueCapabilityContainer_present 
             in rrcConnectionSetupComplete_v6xyext */
          RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
            v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
            v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
            v5c0NonCriticalExtensions.v690NonCriticalExtensions.rrcConnectionSetupComplete_v690ext,
            rrc_RRCConnectionSetupComplete_v690ext_IEs,ueCapabilityContainer);

        if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
        {
 #ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
          /*Set DMCR IE in connection setup complete only when SIBs are deferred and SIB3 indicates DMCR*/
          if(rrcsib_is_dmcr_ie_needed() == TRUE)
          {
              WRRC_MSG0_HIGH("Deferred SIB read:Appending Deferred meas read IE. in con. setup compl");
              msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
                 v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
                 v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
                 v5c0NonCriticalExtensions.v690NonCriticalExtensions.m.v770NonCriticalExtensionsPresent = 1;
  
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
                v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
                v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
                v5c0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions);
  
              RRC_RESET_MSG_IE_PRESENT(msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
                v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
                v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
                v5c0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
                rrcConectionSetupComplete_v770ext);
  
              msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
                v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
                v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
                v5c0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
                rrcConectionSetupComplete_v770ext.m.deferredMeasurementControlReadingPresent = 1;
  
              msg_ptr->message.u.rrcConnectionSetupComplete.v370NonCriticalExtensions.
                v380NonCriticalExtensions.v3a0NonCriticalExtensions.laterNonCriticalExtensions.
                v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.v590NonCriticalExtensions.
                v5c0NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
                rrcConectionSetupComplete_v770ext.deferredMeasurementControlReading = rrc_RRCConnectionSetupComplete_v770ext_IEs_deferredMeasurementControlReading_true;
            }
 #endif
          }
        }

        /* Appends the Radio Access Capability to the RRC CONNECTION SETUP
           COMPLETE message */
        rrcueci_append_ue_radio_access_capability_later_than_r3(&msg_ptr->message.u.rrcConnectionSetupComplete.
                                                                v370NonCriticalExtensions.v380NonCriticalExtensions.
                                                                v3a0NonCriticalExtensions.laterNonCriticalExtensions.
                                                                v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.
                                                                v590NonCriticalExtensions.rrcConnectionSetupComplete_v590ext.
                                                                ue_RadioAccessCapability_v590ext,
                                                                &msg_ptr->message.u.rrcConnectionSetupComplete.
                                                                v370NonCriticalExtensions.v380NonCriticalExtensions.
                                                                v3a0NonCriticalExtensions.laterNonCriticalExtensions.
                                                                v3g0NonCriticalExtensions.v4b0NonCriticalExtensions.
                                                                v590NonCriticalExtensions.v5c0NonCriticalExtensions.
                                                                v690NonCriticalExtensions.rrcConnectionSetupComplete_v690ext.
                                                                ueCapabilityContainer,
                                                                RRCUECI_CAP_UPDATE);
      }
    }
    if (rrcmcm_is_dualmode_enabled())
    {
      if (rrc_capability_update_requirement.
          system_specific_cap_update_req_present == TRUE) 
      /* For Single mode operation IE "UE system specific capability" will not
         be included even though UTRAN requests the same */
      {
        int count;
        num_rat=0;
        for(count = 0; count < rrc_capability_update_requirement.system_specific_cap_update_req_list.count && count < 16;count++)
        {
          if(rrc_capability_update_requirement.system_specific_cap_update_req_list.value[count] == RRC_GSM_CAP)
          {
            num_rat = 1;
            break;
          }
        }
      
        if(num_rat !=0)
        {      
          RRC_MSG_COMMON_SET_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetupComplete,
            rrc_RRCConnectionSetupComplete,ue_RATSpecificCapability);
          msg_ptr->message.u.rrcConnectionSetupComplete.ue_RATSpecificCapability.n= num_rat;
          ALLOC_ASN1ARRAY2(&enc_ctxt,&msg_ptr->message.u.rrcConnectionSetupComplete.ue_RATSpecificCapability,rrc_InterRAT_UE_RadioAccessCapability);
          rrcueci_append_gsm_specific_capability(
            &msg_ptr->message.u.rrcConnectionSetupComplete.ue_RATSpecificCapability.elem[0]);
        
        }
      }   
    }

    if(rrcrce_state_indicator == rrc_RRC_StateIndicator_cell_DCH)
    {
      rrc_update_reconfig_waiting_for_l2ack_dch(RRC_PROCEDURE_RCE);
    }

#ifdef FEATURE_QSH_EVENT_METRIC
	rrc_qsh_log_conn_setup_compl_params(rrcueci_get_cap_container_length());
#endif

    /* This places SDU into RLC watermark queue */
    if (RRCSEND_SUCCESS == 
        rrcsend_send_ul_sdu(RRC_PROCEDURE_RCE,msg_ptr,ul_lc_id,
        RRCSEND_L2ACK_REQUIRED))
    {
      status = RRCRCE_SUCCESS;
    }
    else
    {
      WRRC_MSG0_HIGH("Failed to xmit RRC conn setup Compl Msg");
    }
  }
  else
  { 
    WRRC_MSG0_ERROR("Can't find RLC ID for UL RB 2");
  } 
 
  return (status);            
} /* rrcrce_send_rrc_connection_setup_complete_message */


/*===========================================================================
FUNCTION rrcrce_send_redirection_req

DESCRIPTION
  This function builds the RRC_REDIRECT_REQ command with all parameters
  received through RRC CONNECTION REJECT message and sends the command to
  Cell Selection Procedure
  
DEPENDENCIES
  None

RETURN VALUE
  rrcrce_status_e_type : Returns RRCRCE_SUCCESS if it successfully sends
  RRC_REDIRECT_REQ command. Otherwise returns RRCRCE_FAILURE_MAJOR

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_redirection_req
(
  rrc_DL_CCCH_Message    *msg_ptr,
  boolean                psc_info_present
)
{
 
  rrc_cmd_type *cmd_ptr;                     /* Pointer to RRC Command */
  sys_band_T gsm_band;
  rrc_rr_frequence_band_e_type freq_band;
  rrc_GSM_TargetCellInfoList *GSM_target_cell_info_ptr = NULL;
  rrc_csp_acq_entry_type acq_entry;
  
  /* Allocates the buffer to RRC Internal command */
  if( (cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
  {     
    /* Fill in the command parameters to RRC_REDIRECT_REQ Command */
    cmd_ptr->cmd_hdr.cmd_id = RRC_REDIRECT_REQ;
    cmd_ptr->cmd.redirect_req.proc_id = RRC_PROCEDURE_RCE;
    cmd_ptr->cmd.redirect_req.wait_time = rrcrce_wait_time;
    cmd_ptr->cmd.redirect_req.psc = 513; /* initialize the psc to an invalid value */
    cmd_ptr->cmd.redirect_req.u.GSM_cell_info_list.size = 0;

    if(rsp_to_rrc_conn_req == RRCRCE_CONNECTION_REJECT)
    {
      switch(msg_ptr->message.u.rrcConnectionReject.u.
                 r3.rrcConnectionReject_r3.redirectionInfo.t)
      {
        case T_rrc_RedirectionInfo_frequencyInfo: 
          cmd_ptr->cmd.redirect_req.choice= RRC_FREQ_REDIRECTION;
      
          cmd_ptr->cmd.redirect_req.u.freq.uarfcn_DL = (uint16)
                                    msg_ptr->message.u.rrcConnectionReject.u.
                                    r3.rrcConnectionReject_r3.redirectionInfo.u.
                                    frequencyInfo->modeSpecificInfo.u.fdd->uarfcn_DL;


          if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(msg_ptr->message.u.rrcConnectionReject.u.r3.rrcConnectionReject_r3.
             redirectionInfo.u.frequencyInfo->modeSpecificInfo.u.fdd,
             uarfcn_UL))
          {
            cmd_ptr->cmd.redirect_req.u.freq.uarfcn_UL = (uint16)
              msg_ptr->message.u.rrcConnectionReject.u.r3.rrcConnectionReject_r3.
              redirectionInfo.u.frequencyInfo->modeSpecificInfo.u.fdd->uarfcn_UL;
          }

          /* Unbar the frequency if provided in IE redirection info 
             Spec 25304 : 5.2.6.1.3 */
          rrc_csp_int_data.wcdma_cell_time_to_reselect = 0;
          rrc_csp_int_data.cell_bar_info = RRC_CSP_WCDMA_FREQ_UNBAR;
          acq_entry.freq = cmd_ptr->cmd.redirect_req.u.freq.uarfcn_DL;
          rrc_csp_send_l1_cmd(RRC_PROCEDURE_CSP, CPHY_CELL_BAR_REQ, &acq_entry);
          break;
    
        case T_rrc_RedirectionInfo_interRATInfo:
          cmd_ptr->cmd.redirect_req.choice= RRC_INTER_SYSTEM_GSM;
          if(msg_ptr->message.u.rrcConnectionReject.u.r3.m.laterNonCriticalExtensionsPresent)
          {
            if(msg_ptr->message.u.rrcConnectionReject.u.r3.laterNonCriticalExtensions.m.
                v690NonCriticalExtensionsPresent)
            {
              if(msg_ptr->message.u.rrcConnectionReject.u.r3.laterNonCriticalExtensions.
                v690NonCriticalExtensions.rrcConnectionReject_v690ext.m.redirectionInfo_v690extPresent)
              {
                 uint32 local_idx=0;
                 GSM_target_cell_info_ptr = &msg_ptr->message.u.rrcConnectionReject.u.r3.laterNonCriticalExtensions.
                    v690NonCriticalExtensions.rrcConnectionReject_v690ext.redirectionInfo_v690ext;
                 
                 if(WTOW_SEARCH_STOP != rrc_csp_bplmn_srch_in_progress())
                 {
                   rrccsp_cleanup_bplmn_state();
                 }
    
                 while ((GSM_target_cell_info_ptr->n > local_idx) && 
                        (cmd_ptr->cmd.redirect_req.u.GSM_cell_info_list.size < MAX_RRC_RR_GSM_CELL_LIST_SIZE))
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
                       cmd_ptr->cmd.redirect_req.u.GSM_cell_info_list.arfcns[cmd_ptr->cmd.redirect_req.u.GSM_cell_info_list.size].num
                          = (uint16) GSM_target_cell_info_ptr->elem[local_idx].bcch_ARFCN;
              
                       cmd_ptr->cmd.redirect_req.u.GSM_cell_info_list.arfcns[cmd_ptr->cmd.redirect_req.u.GSM_cell_info_list.size].band
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
                       cmd_ptr->cmd.redirect_req.u.GSM_cell_info_list.BSIC[cmd_ptr->cmd.redirect_req.u.GSM_cell_info_list.size].BSIC_NCC 
                              = GSM_target_cell_info_ptr->elem[local_idx].bsic.ncc;
                       cmd_ptr->cmd.redirect_req.u.GSM_cell_info_list.BSIC[cmd_ptr->cmd.redirect_req.u.GSM_cell_info_list.size++].BSIC_BCC 
                              = GSM_target_cell_info_ptr->elem[local_idx].bsic.bcc;
                    }
                    else
                    {
                       cmd_ptr->cmd.redirect_req.u.GSM_cell_info_list.BSIC[cmd_ptr->cmd.redirect_req.u.GSM_cell_info_list.size].BSIC_NCC 
                              = 0xFF;
                       cmd_ptr->cmd.redirect_req.u.GSM_cell_info_list.BSIC[cmd_ptr->cmd.redirect_req.u.GSM_cell_info_list.size++].BSIC_BCC 
                              = 0xFF;
                    }
                    local_idx++;
                 }   
              }
            }
          }
          break;

        default:     
          WRRC_MSG0_ERROR("This should never happen");
          break;
      }      
    }
    else if(rsp_to_rrc_conn_req == RRCRCE_CONNECTION_SETUP)
    {
      cmd_ptr->cmd.redirect_req.choice= RRC_FREQ_REDIRECTION;

      if(msg_ptr->message.u.rrcConnectionSetup.t == T_rrc_RRCConnectionSetup_r3)
      {
        cmd_ptr->cmd.redirect_req.u.freq.uarfcn_DL = (uint16)
                                    msg_ptr->message.u.rrcConnectionSetup.u.
                                    r3.rrcConnectionSetup_r3.frequencyInfo.modeSpecificInfo.
                                    u.fdd->uarfcn_DL;
        if((psc_info_present)
         && (msg_ptr->message.u.rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.dl_InformationPerRL_List.n != 0))
        {
          cmd_ptr->cmd.redirect_req.psc = (uint16)
                                      msg_ptr->message.u.rrcConnectionSetup.u.r3.rrcConnectionSetup_r3.
                                      dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->
                                      primaryCPICH_Info.primaryScramblingCode;
        } 
      }
      else
      {
        if ((msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.t == 
             T_rrc_RRCConnectionSetup_criticalExtensions_5_criticalExtensions) && 
            (msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.t ==
             T_rrc_RRCConnectionSetup_criticalExtensions_4_r5)) 
        {
          if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
          {
            /* Safely assume REL-5 as other cases would have been filtered by the time we are here */
            cmd_ptr->cmd.redirect_req.u.freq.uarfcn_DL = (uint16)
            msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
              criticalExtensions.u.r5.rrcConnectionSetup_r5.frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
            
            if((psc_info_present) && 
               (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                criticalExtensions.u.r5.rrcConnectionSetup_r5,
                rrc_RRCConnectionSetup_r5_IEs,dl_InformationPerRL_List)))
            {
              cmd_ptr->cmd.redirect_req.psc = (uint16)
              msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                criticalExtensions.u.r5.rrcConnectionSetup_r5.
                dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->
                primaryCPICH_Info.primaryScramblingCode;
            }
          } /* if R5 NV is set */
        } /* if R5 msg*/
        else if (RRCRCE_R6_CRITICAL_EXT_PRESENT)
        { 
          if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6)
          {
            /* Safely assume REL-6 as other cases would have been filtered by the time we are here */
            cmd_ptr->cmd.redirect_req.u.freq.uarfcn_DL = (uint16)
            msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
              criticalExtensions.u.criticalExtensions.u.r6.rrcConnectionSetup_r6.frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
            
            if((psc_info_present) && 
               (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                criticalExtensions.u.criticalExtensions.u.r6.rrcConnectionSetup_r6, 
                rrc_RRCConnectionSetup_r6_IEs,dl_InformationPerRL_List)))
            {
              cmd_ptr->cmd.redirect_req.psc = (uint16)
              msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                criticalExtensions.u.criticalExtensions.u.r6.rrcConnectionSetup_r6.
                dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->
                primaryCPICH_Info.primaryScramblingCode;
            }
          } /* if R6 NV is set */
        } /* if R6 msg*/
        else if (RRCRCE_R7_CRITICAL_EXT_PRESENT)
        { 
          if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
          {
            /* Safely assume REL-6 as other cases would have been filtered by the time we are here */
            cmd_ptr->cmd.redirect_req.u.freq.uarfcn_DL = (uint16)
            msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
              criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7.rrcConnectionSetup_r7.frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
            
            if((psc_info_present) && 
               (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7.rrcConnectionSetup_r7,
                rrc_RRCConnectionSetup_r7_IEs,dl_InformationPerRL_List)))
            {
              cmd_ptr->cmd.redirect_req.psc = (uint16)
              msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r7.rrcConnectionSetup_r7.
                dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->
                primaryCPICH_Info.primaryScramblingCode;
            }
          } /* if R7 NV is set */
        } /* if R6 msg*/
        else if (RRCRCE_R8_CRITICAL_EXT_PRESENT)
        { 
          if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8)
          {
            /* Safely assume REL-7 as other cases would have been filtered by the time we are here */
            cmd_ptr->cmd.redirect_req.u.freq.uarfcn_DL = (uint16)
            msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
                          criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.rrcConnectionSetup_r8.frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
            
            if((psc_info_present) && (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                       criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.rrcConnectionSetup_r8,
                       rrc_RRCConnectionSetup_r8_IEs,dl_InformationPerRL_List)))
            {
              cmd_ptr->cmd.redirect_req.psc = (uint16)
                  msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
                  criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r8.rrcConnectionSetup_r8.
                  dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
            }
          } /* if R8 NV is set */
        } /* if R8 msg*/
#ifdef FEATURE_WCDMA_REL9

        else if (RRCRCE_R9_CRITICAL_EXT_PRESENT)
        { 
          if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9)
          {
            /* Safely assume REL-8 as other cases would have been filtered by the time we are here */
            cmd_ptr->cmd.redirect_req.u.freq.uarfcn_DL = (uint16)
            msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
                          criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.rrcConnectionSetup_r9.frequencyInfo.modeSpecificInfo.u.fdd->uarfcn_DL;
            
            if((psc_info_present) && (RRC_MSG_COMMON_BITMASK_IE(msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.
                       criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.rrcConnectionSetup_r9,
                       rrc_RRCConnectionSetup_r8_IEs,dl_InformationPerRL_List)))
            {
              cmd_ptr->cmd.redirect_req.psc = (uint16)
                  msg_ptr->message.u.rrcConnectionSetup.u.later_than_r3.criticalExtensions.u.criticalExtensions.u.
                  criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.later_r8_criticalExtensions.u.r9.rrcConnectionSetup_r9.
                  dl_InformationPerRL_List.elem[0].modeSpecificInfo.u.fdd->primaryCPICH_Info.primaryScramblingCode;
            }
          } /* if R9 NV is set */
        } /* if R9 msg*/
#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
         #error code not present
#endif /* FEATURE_WCDMA_REL10 */


      } /*else if later than R3 is selected */
    }
    else
    {
      ERR_FATAL("ERR: incorrect enum type %d for connection rsp", rsp_to_rrc_conn_req, 0, 0);
    }

    /* sends the command to Cell Selection Procedure */
    rrc_put_int_cmd(cmd_ptr);         
  }

   
} /* rrcrce_send_redirection_req */

#ifdef FEATURE_WCDMA_TO_LTE
/*===========================================================================
FUNCTION rrcrce_send_lte_redirection_req

DESCRIPTION
  This function builds the RRC_REDIRECT_REQ command with all parameters
  received through RRC CONNECTION REJECT message and sends the command to
  Cell Selection Procedure
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_lte_redirection_req
(
  rrc_DL_CCCH_Message    *msg_ptr
)
{
  rrc_cmd_type *cmd_ptr;                     /* Pointer to RRC Command */
  rrc_EUTRA_TargetFreqInfoList *EUTRA_target_freq_info_ptr = NULL;
  rrc_EUTRA_BlacklistedCellPerFreqList *blacklisted_cell_info_ptr = NULL;
  uint8 earfcn_count = 0;
  uint8 blacklisted_cells_count = 0;
  uint32 idx, idy;
  uint32 earfcn;
  if( (cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
  {
    /* Fill in the command parameters to RRC_REDIRECT_REQ Command */
    cmd_ptr->cmd_hdr.cmd_id = RRC_REDIRECT_REQ;
    cmd_ptr->cmd.redirect_req.proc_id = RRC_PROCEDURE_RCE;
    cmd_ptr->cmd.redirect_req.wait_time = rrcrce_wait_time;
    cmd_ptr->cmd.redirect_req.psc = 513; /* initialize the psc to an invalid value */

    cmd_ptr->cmd.redirect_req.choice = RRC_INTER_SYSTEM_LTE;
    cmd_ptr->cmd.redirect_req.u.EUTRA_target_cell_info.earfcn_count = 0;

    EUTRA_target_freq_info_ptr = &(msg_ptr->message.u.rrcConnectionReject.u.r3.laterNonCriticalExtensions.v690NonCriticalExtensions.v6f0NonCriticalExtensions.
      v860NonCriticalExtensions.rrcConnectionReject_v860ext.redirectionInfo.u.interRATInfo->u.eutra->eutra_TargetFreqInfoList);
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
      cmd_ptr->cmd.redirect_req.u.EUTRA_target_cell_info.earfcn_info[earfcn_count].earfcn = EUTRA_target_freq_info_ptr->elem[idx].dlEUTRACarrierFreq;

      /* Check if blacklisted cells also present for the current EARFCN.*/
      blacklisted_cells_count = 0;
      if(EUTRA_target_freq_info_ptr->elem[idx].m.eutraBlacklistedCellPerFreqListPresent)
      {
        idy = 0;
        blacklisted_cell_info_ptr = &(EUTRA_target_freq_info_ptr->elem[idx].eutraBlacklistedCellPerFreqList);
        while((blacklisted_cell_info_ptr->n > idy) &&
                 (blacklisted_cells_count < LTE_RRC_IRAT_MAX_EUTRA_CELLS_PER_FREQ))
        {
          cmd_ptr->cmd.redirect_req.u.EUTRA_target_cell_info.earfcn_info[earfcn_count].blacklisted_cells[blacklisted_cells_count++] = 
             blacklisted_cell_info_ptr->elem[idy].physicalCellIdentity;
          idy++;
        }
      }
      cmd_ptr->cmd.redirect_req.u.EUTRA_target_cell_info.earfcn_info[earfcn_count++].blacklisted_cells_count = blacklisted_cells_count;
      idx++;
    }
    cmd_ptr->cmd.redirect_req.u.EUTRA_target_cell_info.earfcn_count = earfcn_count;
    WRRC_MSG1_HIGH("WTOL: Num of EARFCN's in redirection info = %d", earfcn_count);

    if(earfcn_count == 0)
    {
      WRRC_MSG0_HIGH("WTOL: No target EARFCN's supported. No redirection.");
      rrc_free(cmd_ptr);
      rrctmr_start_timer(RRCTMR_WAIT_TIME_TIMER, rrcrce_wait_time * 1000);  
      rrcrce_substate = RRCRCE_WAIT_FOR_WAIT_TIMER_TO_EXPIRE;
    }
    else
    {

      if(WTOW_SEARCH_STOP != rrc_csp_bplmn_srch_in_progress())
      {
        rrccsp_cleanup_bplmn_state();
      }

      /* sends the command to Cell Selection Procedure */
      rrc_put_int_cmd(cmd_ptr);         
      /* Wait for redirect confirm from Cell Selection Procedure */
      rrcrce_substate = RRCRCE_WAIT_FOR_REDIRECT_CNF;
    }
  }
}
#endif



/*===========================================================================

FUNCTION rrcrce_send_rrc_rel_ind

DESCRIPTION
  This function builds RRC_REL_IND command to post to MM with the 
  RRC Connection Release cause.
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_rrc_rel_ind
(
  void
)
{
  mm_cmd_type *cmd_ptr;                      /* Pointer to the RRC Command */

  rrcrce_sig_con_delay = FALSE;
  /* Allocate memory for RRC_REL_IND command */
  cmd_ptr =  mm_rrc_get_cmd_buf(RRC_REL_IND);
#ifdef FEATURE_DUAL_SIM
    cmd_ptr->cmd.rrc_rel_ind.as_id = rrc_get_as_id();
#endif
    /* Fill in parameters fro RRC_REL_IND command */
    cmd_ptr->cmd.rrc_rel_ind.rel_cause = rrc_release_cause;
    
    /* Update the Max HFNs values to be stored in SIM or USIM */ 
    (void) rrcsmc_append_hfns_to_rel_ind(cmd_ptr->cmd.rrc_rel_ind.hfn_cs,
                                  cmd_ptr->cmd.rrc_rel_ind.hfn_ps);

    rrc_send_rrc_message_to_mm(cmd_ptr);               /* Sends the command to MM */
   
    WRRC_MSG1_HIGH("Dispatching out MM cmd / RRC sends RRC_REL_IND to MM,Cause %d",rrc_release_cause);

    rrcrce_send_conn_rel_cause_event((uint8)RRC_ABORT_CAUSE_TYPE_MAX + (uint8)rrc_release_cause);

#ifdef FEATURE_DUAL_SIM
  rrc_release_lock();
#endif

#ifdef FEATURE_3GPP_CSFB
  /*Connection is released, reset CSFB call status flag*/
  rrc_csfb_call_status = FALSE;
#endif

  rrc_send_rab_rel_ind_on_ps_release();

#ifdef FEATURE_WRLF_SYSTEM_SEL
  /* When releasing connection due to re-establishment timer expiry send suitable 
       search end ind to NAS (if splmn 30s timer has not expired), and disable BPLMN
       ind as FALSE to enable the periodic BPLMN searches*/
  if(RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
  {
    rrc_csp_check_n_send_suitable_srch_end_ind();
    rrc_csp_send_disable_bplmn_ind(FALSE);
  }
#endif

#ifdef FEATURE_HANDLE_CN_PAGE_IN_CONNECTED_STATE

  /* If CN identity page match is set then send the stored PAGE_IND to NAS 
     so that EST_REQ will be sent by NAS */
  if(TRUE == rrcpg1_match_found_conn_mode_cn_id)
  {
    rrcpg1_send_page_ind();
    rrcpg1_match_found_conn_mode_cn_id = FALSE;
  }
#endif

  /* Initialize release cause */
  rrc_release_cause = RRC_REL_CAUSE_UNSPEC;

}/*rrcrce_send_rrc_rel_ind */
/*===========================================================================
FUNCTION rrcrce_process_rce_wait_for_srb_rel_cnf_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Establishment procedure in RRCRCR_WAIT_FOR_SRB_REL_CNF substate of any RRC 
  state.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_srb_rel_cnf_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  switch (cmd_ptr->cmd_hdr.cmd_id)
  {

    case RRC_STATE_CHANGE_IND:
      if((cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED) &&
         ( rrc_get_state() == RRC_STATE_DISCONNECTED))
      {
        if((RRC_MODE_ACTIVE == rrcmcm_get_rrc_mode())
#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
#ifdef FEATURE_WTOG_REDIRECTION_AFTER_CONN_RELEASE
          /* For inter-RAT redirection due to connection release, handle STATE_CHANGE_IND
          * in STANDBY_MODE as RRC releases all channels before starting redirection search
          */
          || ((RRC_MODE_STANDBY == rrcmcm_get_rrc_mode())
             && (rrccsp_get_curr_scan() == RRC_CSP_INTER_RAT_REDIRECTION_SCAN))
#endif
#endif
#ifdef FEATURE_WRLF_SYSTEM_SEL
          || (RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS() && 
             (RRC_MODE_INACTIVE == rrcmcm_get_rrc_mode()))
#endif
#ifdef FEATURE_DUAL_SIM
          /*This is possible if hard abort is received when waiting for 
             lock to resumse after LTA*/
          ||((RRC_MODE_INACTIVE == rrcmcm_get_rrc_mode())&&
             (rrcmcm_wl1_failure_cause == WL1_FAILED_NO_WRM_LOCK)
            )
#endif
         )
        {
#ifdef FEATURE_DUAL_SIM
          if(RRC_MODE_INACTIVE == rrcmcm_get_rrc_mode())
          {
            rrcmcm_wl1_failure_cause = WL1_FAILED_NORMAL;
          }
#endif
          /* Check if abort is pending */
          if (rrcrce_send_abort_cnf == TRUE) 
          {
            rrcrce_send_rrc_abort_cnf();
              /* Clear the ESTABLISHED_RABS while bringing down RRC Connection*/
              rrc_clear_established_rabs();
              
              /* Reset Signalling Connections status */
              rrcscr_reset_signalling_connection_status();
              
              rrcrce_substate = RRCRCE_INITIAL;
          }
          else
          {
              rrcrce_send_rrc_rel_ind();
              /* Clear the ESTABLISHED_RABS while bringing down RRC Connection*/
              rrc_clear_established_rabs();

              /* Reset Signalling Connections status */
              rrcscr_reset_signalling_connection_status();

              rrcrce_substate = RRCRCE_INITIAL;

              break;
            }
        }
        else
        {
          /* Don't send the REL_IND to MM */
          /* Clear the ESTABLISHED_RABS while bringing down RRC Connection*/
          rrc_clear_established_rabs();

          /* Reset Signalling Connections status */
          rrcscr_reset_signalling_connection_status();

          rrcrce_substate = RRCRCE_INITIAL;
        }
      }
      break;

    case RRC_ABORT_REQ:
      /* Another RRC_ABORT_REQ is received. Just set the flag again just in 
       * case it wasn't set before
       */
      rrcrce_send_abort_cnf = TRUE;

      break;

    case RRC_CHANNEL_CONFIG_CNF:

      rrcllcpcie_initialise_sync_a_post_veri_info();

      if(cmd_ptr->cmd.chan_config_cnf.transaction_id == rce_chl_config_transaction_id)
      {
        if (rrcrce_send_abort_cnf == TRUE) 
        {
          /* Send RRC_ABORT_CNF command to the MM */
            rrcrce_send_rrc_abort_cnf();     
            /* Call this fn to make sure that Establised RABS contains invalid values */
            rrc_clear_established_rabs();

            rrcrce_substate = RRCRCE_INITIAL; /* The RCE Procedure ends */
            /* Reset Signalling Connections status */
            rrcscr_reset_signalling_connection_status();
          }    
          else
          {     
            rrcrce_send_rrc_rel_ind();
            /* Clear the ESTABLISHED_RABS while bringing down RRC Connection*/
            rrc_clear_established_rabs();

            /* Reset Signalling Connections status */
            rrcscr_reset_signalling_connection_status();

            rrcrce_substate = RRCRCE_INITIAL;

            break;
          }
      }
      else
      {
        WRRC_MSG2_HIGH("Ignoring tr_id %d in chl config cnf. Expected tr_id = %d", 
                   cmd_ptr->cmd.chan_config_cnf.transaction_id,
                    rce_chl_config_transaction_id);
      }
      break;
      
    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
        cmd_ptr->cmd_hdr.cmd_id,rrcrce_substate);
      break;
  }  

} /* rrcrce_process_rce_wait_for_srb_rel_cnf_substate */


/*===========================================================================
FUNCTION rrcrce_process_rce_wait_for_rb0_rel_cnf_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Establishment procedure in RRCRCE_WAIT_FOR_RB0_REL_CNF substate of any RRC 
  state.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_rb0_rel_cnf_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  rrc_abort_cause_e_type  cause = RRC_MAX_ACCESS_FAILURE;

  switch (cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_ABORT_REQ:
      rrcrce_send_abort_cnf = TRUE;
      break;

    case RRC_STATE_CHANGE_IND:
      WRRC_MSG0_HIGH("State change ind received by RCE");
      if (cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED)
      {
        if(rrccho_is_gtow_pcco_in_progress())
        {
          rrc_conn_setup_ind_type conn_setup_ind_params;
          conn_setup_ind_params.status = FAILURE;
          conn_setup_ind_params.conn_failure_reason = RRC_CONN_FAILURE_OTHER;

          WRRC_MSG0_HIGH("State change ind when PCCO active in rb0_rel_cnf substate");
          rrcrce_send_conn_setup_ind(conn_setup_ind_params);                  
                
          /* set rce_substate to initial */
          rrcrce_substate = RRCRCE_INITIAL;
          break;
        }
        
        if (rrcrce_send_abort_cnf == TRUE)
        {
          /* Send RRC_ABORT_CNF command to the MM */
            rrcrce_send_rrc_abort_cnf();   
            rrcrce_substate = RRCRCE_INITIAL; /* The RCE Procedure ends */
            break;
        }    
        else
        {     
          if(!rrcrce_abort_ind_sent)
          {
            rrcrce_send_rrc_abort_ind(rrcrce_mm_abort_cause);      
          }
          /* The RCE Procedure ends */
          rrcrce_substate = RRCRCE_INITIAL;    /* reset the RCE substate */
          rrcrce_abort_ind_sent = FALSE;
        }
      }
      break;

    case RRC_EST_REQ: 

      rrc_protocol_error_indicator = rrc_ProtocolErrorIndicator_noError;

      /* Resets the V300 variable */
      rrc_v_300 = 0;
   
      /* Initialize the Transaction Id for RRC CONNECTION SETUP to Invalid */
      rrcrce_setup_transaction_identifier = RRCRCE_INVALID_TRANSACTION_ID;
      
      /* Initialize the Transaction Id for RRC CONNECTION REJECT to Invalid */
      rrcrce_reject_transaction_identifier = RRCRCE_INVALID_TRANSACTION_ID;

      /* Initialize the Wait Time to zero seconds */
      rrcrce_wait_time = 0;

      /* Initialize abort flag to FALSE */
      rrcrce_send_abort_cnf = FALSE;

      rrcrce_abort_ind_sent = FALSE;

      /* Initialize release cause to unspecified */
      rrc_release_cause = RRC_REL_CAUSE_UNSPEC;

      if(RRCRCE_SUCCESS == rrcrce_validate_rrc_est_req(cmd_ptr))
      {
        /*updates the establishment cause */
        rrc_establishment_cause = cmd_ptr->cmd.rrc_est_req.cause;
      
        /* Updates Initial UE Identity */
        rrc_initial_ue_identity_choice = cmd_ptr->cmd.rrc_est_req.ue_id_choice;

        rrc_initial_ue_identity = cmd_ptr->cmd.rrc_est_req.initial_ue_id;

        rrc_est_cn_domain_id = cmd_ptr->cmd.rrc_est_req.cn_domain_id;

        rrc_est_call_type = cmd_ptr->cmd.rrc_est_req.call_type;

        rrc_est_req_for_rai = cmd_ptr->cmd.rrc_est_req.rai;
        MSG_6(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
             "RRC_EST_REQ for cn_domain: %d, RAI PLMN %d-%d, LAC: %x %x  RAC %d",
             rrc_est_cn_domain_id,RRC_CSP_GET_INT_MCC(rrc_est_req_for_rai.lai.plmn),
             RRC_CSP_GET_INT_MNC(rrc_est_req_for_rai.lai.plmn),
             rrc_est_req_for_rai.lai.lac[0],rrc_est_req_for_rai.lai.lac[1],
             rrc_est_req_for_rai.rac[0]);


        /* Function call to CSP to know whether UE is camped on  */
        if (RRC_CAMPED_ON == rrc_ccm_get_curr_camping_status(RRC_PROCEDURE_RCE))
        {
          if(RRCRCE_SUCCESS == rrcrce_validate_cell_access_status(
                                   rrc_est_cn_domain_id,
                                   rrc_establishment_cause,
                                   &cause)
            )
          {
            if(WTOW_SEARCH_STOP != rrc_csp_bplmn_srch_in_progress())
            {
              /* Due to race condition because of "context-switching", 
               * RCE needs to update it's substate before putting 
               * RRC_BPLMN_SUSPEND_REQ in the queue 
               */
              rrcrce_substate = RRCRCE_WAIT_FOR_UE_CAMP_ON;

              /* With the below feature defined we will suspend and resume
               * the BPLMN search for any establishment cause received 
               * from NAS
               */
              rrc_csp_send_bplmn_suspend_req(RRC_PROCEDURE_RCE, BPLMN_SUSPEND_REASON_REGISTRATION);
              /* Wait for NEW_CELL_IND in RRCRCE_WAIT_FOR_UE_CAMP_ON substate */
            
              /* Starting this timer is NOT required as CSP on receiving RRC_BPLMN_SUSPEND_REQ
               * will send RRC_BPLMN_SUSPEND_CNF almost immediately. This timer is started only 
               * as a recovery mechanism in case CSP fails to send RRC_BPLMN_SUSPEND_CNF to RCE
               */
              rrctmr_start_timer(RRCTMR_UE_TO_CAMP_ON_TIMER, 
                            (unsigned long) RRCRCE_TIMER_VALUE_FOR_UE_TO_CAMP_ON);       
            }
            else
            {
#ifdef FEATURE_DUAL_SIM
              if(rrc_acquire_lock_for_est_req(rrc_establishment_cause,0xfffffff,RRCTMR_WRM_LOCK_WAIT_TIMER_FOR_EST_REQ_IN_MS) == FAILURE)
              {
                /* Send RRC_ABORT_IND command to the MM */
                rrcrce_send_rrc_abort_ind(RRC_NO_RESOURCE_AVAILABLE);     
                rrcrce_substate = RRCRCE_INITIAL; /* The RCE Procedure ends */
                return;
              }
#endif
               /* UE is camped on, then Check whether SIB7 is current */
              if(TRUE == rrcsib_check_and_update_sib7_validity (RRC_PROCEDURE_RCE) )
              { 
                /* UE is camped on and SIB7 is current, then Setup RB0 */
                    rrcrce_send_rrc_channel_config_req_for_rb0(ESTABLISH_RB,
                      RRC_STATE_CONNECTING, TRUE );  
                  rrcrce_substate = RRCRCE_WAIT_FOR_RB0_SETUP_CNF;     
              }    
              else
              {
                WRRC_MSG0_HIGH("RCE waits for SIB7 to become current"); 
                rrcrce_substate = RRCRCE_WAIT_FOR_SIB7_CURRENT;
              }
            }
          }
          else
          {
            if(rrc_est_cn_domain_id == RRC_CS_DOMAIN_CN_ID)
            {
              rrcrce_send_rrc_abort_ind(RRC_AC_NOT_ALLOWED_CS_DOMAIN);
            }
            else
            {
              rrcrce_send_rrc_abort_ind(RRC_AC_NOT_ALLOWED_PS_DOMAIN);
            }
          }
        }
        else /* UE is not camped on */
        {
          rrcrce_substate = RRCRCE_WAIT_FOR_UE_CAMP_ON;

          /* start the timer for UE to camp on */
          rrctmr_start_timer(RRCTMR_UE_TO_CAMP_ON_TIMER, 
                        (unsigned long) RRCRCE_TIMER_VALUE_FOR_UE_TO_CAMP_ON);       
        }
      }
      else /* Validation Failed */
      {
        WRRC_MSG0_ERROR("Validation failure in RRC_EST_REQ");

        /* If GTOW is in progress,send a conn_setup_ind with failure to CHO
           CHO would deactivate WCDMA and pass control back to GSM.Do not send
           RRC_ABORT_IND to MM if GTOW is active */
        if(rrccho_is_gtow_pcco_in_progress())
        { 
          rrc_conn_setup_ind_type conn_setup_ind_params;
          conn_setup_ind_params.status = FAILURE;
          conn_setup_ind_params.conn_failure_reason = RRC_CONN_FAILURE_OTHER;

          WRRC_MSG0_HIGH("Validation failed in RRC_EST_REQ when PCCO is active");
          rrcrce_send_conn_setup_ind(conn_setup_ind_params);                  

          /* Reset RCE susbstate */
          rrcrce_substate = RRCRCE_INITIAL;
          return;
        }

        /* Send RRC_ABORT_IND command to the MM */
        rrcrce_send_rrc_abort_ind(RRC_UNSUPPORTED_INITIAL_UE_ID);      
      }
      break;

    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
                  cmd_ptr->cmd_hdr.cmd_id,rrcrce_substate);
      break;
  }  
} /* rrcrce_process_rce_wait_for_rb0_rel_cnf_substate */

/*===========================================================================
FUNCTION rrcrce_process_rce_wait_for_sig_con_rel_compl_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Establishment procedure in RRCRCE_WAIT_FOR_SIG_CON_REL_COMPL sub state either 
  CELL_FACH State or CELL_DCH state.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_sig_con_rel_compl_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{ 
  boolean initiate_cell_sel;

  switch (cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_DOWNLINK_SDU_IND:
      /* RRC Connection Setup or RRC Connection Reject may be received in this
         Substate because of cell-reselections. */
      WRRC_MSG0_HIGH("Simultanous transactions not supported");
      break;

    case RRC_STATE_CHANGE_IND:
      /* Send RRC_ABORT_CNF command to the MM since RCE already in this substate
         since it has received RRC_ABORT_REQ */
      if (rrcrce_send_abort_cnf == TRUE)
      {
        rrcrce_send_rrc_abort_cnf();      
      }
#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
      else if((csfb_extended_status == CSFB_CS_RB_RELEASED) && 
        (cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED))
      {
        rrcrce_send_rrc_rel_ind();
      }
#endif
        
      rrcrce_substate = RRCRCE_INITIAL; /* The RCE Procedure ends */

      /* Clear the ESTABLISHED_RABS while bringing down RRC Connection*/
      rrc_clear_established_rabs();

      /* Reset Signalling Connections status */
      rrcscr_reset_signalling_connection_status();
      rrcrce_sig_conn_rel_req_for_both_domains = FALSE;
      break;

    case RRC_ABORT_REQ:
      /* Another RRC_ABORT_REQ is received. Just set the flag again just in 
       * case it wasn't set before
       */
      rrcrce_send_abort_cnf = TRUE;

      initiate_cell_sel = FALSE;

      if(cmd_ptr->cmd.rrc_abort_req.hard_abort == TRUE)
      {
        initiate_cell_sel = TRUE;
      }

#ifdef FEATURE_DUAL_SIM
      if(cmd_ptr->cmd.rrc_abort_req.abort_cause == RRC_DUAL_STANDBY_PS_ABORT)
      {
        initiate_cell_sel = TRUE;
      }
#endif
      if(initiate_cell_sel)
      {
        /* Send initiate cell sel command to CSP so that UE is taken to Idle */
        rrc_transition_to_disconnected_state(RRC_PROCEDURE_RCE, RRC_TX_TO_DISCON_HARD_ABORT);

        /* Wait for successful release of SRBs */
        rrcrce_substate = RRCRCR_WAIT_FOR_SRB_REL_CNF;
        rrcrce_send_abort_cnf = TRUE;
      }
      break;

    case RRC_NEW_CELL_IND:          
      /* The UE is already in CELL_FACH State. So Ignore the command since Cell
         Update would have been initiated */
      MSG_MED("Unexpected event:%d in the substate: %d", 
               cmd_ptr->cmd_hdr.cmd_id, rrcrce_substate,0); 
      break;

    case RRC_SIG_CONN_RELEASE_CNF:
      /* Release the Signalling Radio Bearer irrespective of the status */
      WRRC_MSG1_HIGH("Status rcvd from SCRR: %d", 
               cmd_ptr->cmd.rrc_sig_conn_cnf.status);

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
      if(csfb_extended_status == CSFB_CS_RB_RELEASED)
      {
        /* start the timer for UE before it can release RRC Connection */
        rrctmr_start_timer(RRCTMR_RRC_CON_DELAY_TIMER, 
          (unsigned long) rrc_fast_return_to_lte_after_csfb_timer_nv); 
        
        WRRC_MSG1_HIGH("CSFB_HACK:Started wait for RRC conn release timer of value :%d ms",
          rrc_fast_return_to_lte_after_csfb_timer_nv); 
        /* Sending Close session Indication as otherwise RRC and NAS will go out of sync.*/
        rrcscr_send_close_session_ind(RRC_PS_DOMAIN_CN_ID, RRC_MAX_SESSION_CLOSE_CAUSE);
        rrcrce_substate = RRCRCE_WAIT_FOR_RRC_TIMER_TO_EXPIRE;
        break;
      }
#endif
      if (rrcrce_sig_conn_rel_req_for_both_domains == TRUE)
      {
        WRRC_MSG0_HIGH("SigConRelCnf rcvd for first domain");
        /* Remain in this sub-state waiting for the next one */
        
        /* Set the following bool to FALSE so that previous
        processing could be done again */
        rrcrce_sig_conn_rel_req_for_both_domains = FALSE;
        break;
      }
       /* On reception of L2 ACK for SCRI, irrespective of flag rrcrce_sig_con_delay,
          RRC shall now wait( duration RRCRCE_TIMER_VALUE_SMALL_FOR_RRC_CON_REL) for 
          network to trigger RRC Connection Release*/

      rrcrce_sig_con_delay = FALSE;
#ifndef FEATURE_WCDMA_FIELD_OPTIMIZATIONS
      /*This is our normal behaviour*/
      /* start the timer for UE before it can release RRC Connection */
      rrctmr_start_timer(RRCTMR_RRC_CON_DELAY_TIMER, 
        (unsigned long) RRCRCE_TIMER_VALUE_SMALL_FOR_RRC_CON_REL);    
      
      WRRC_MSG1_HIGH("Started RRC con smallar delay timer :%d",
        RRCRCE_TIMER_VALUE_SMALL_FOR_RRC_CON_REL); 
#else
      /*This is our feild optimization if FEATURE_WCDMA_FIELD_OPTIMIZATIONS is defined */
      /* start the timer for UE with zero value */
      rrctmr_start_timer(RRCTMR_RRC_CON_DELAY_TIMER, 
        (unsigned long) RRCRCE_ZERO_TIMER_VALUE_FOR_RRC_CON_REL);    
      WRRC_MSG0_HIGH("Started RRC con delay timer with 0 value"); 
#endif
      rrcrce_substate = RRCRCE_WAIT_FOR_RRC_TIMER_TO_EXPIRE;

      break;
   
    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id, rrcrce_substate);
      break;

   }  
} /* rrcrce_process_rce_wait_for_sig_con_rel_compl_substate*/
/*===========================================================================
FUNCTION rrcrce_process_rce_wait_for_rrc_timer_expire_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Establishment procedure in RRCRCE_WAIT_FOR_RRC_TIMER_TO_EXPIRE sub state 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_wait_for_rrc_timer_expire_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{ 
  rrc_cmd_type *rrcrce_close_sess_cmd_ptr;
  
  uint8 cnt = 0;
#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
  uint32 current_drx_cycle_ms = 0;	
  rrcllc_oc_set_status_e_type oc_status;
  oc_status = rrcllc_get_ordered_config_status();
#endif 
  switch (cmd_ptr->cmd_hdr.cmd_id)
  {
  case RRC_DOWNLINK_SDU_IND:
  /* RRC Connection Setup or RRC Connection Reject may be received in this
    Substate because of cell-reselections. */
    WRRC_MSG0_HIGH("Simultanous transactions not supported");
    break;
    
  case RRC_STATE_CHANGE_IND:
    /* stop the rrc wait timer */
    rrctmr_stop_timer(RRCTMR_RRC_CON_DELAY_TIMER);
    /* Send RRC_ABORT_CNF command to the MM since RCE already in this substate
    since it has received RRC_ABORT_REQ */
    
    if(rrcrce_send_abort_cnf)
    {
      rrcrce_send_rrc_abort_cnf();      
    }
#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
    else if((csfb_extended_status == CSFB_CS_RB_RELEASED) && 
        (cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED))
    {
      rrcrce_send_rrc_rel_ind();
    }
#endif
      
    rrcrce_substate = RRCRCE_INITIAL; /* The RCE Procedure ends */
     
    /* Clear the ESTABLISHED_RABS while bringing down RRC Connection*/
    rrc_clear_established_rabs();
      
    /* Reset Signalling Connections status */
    rrcscr_reset_signalling_connection_status();
    rrcrce_sig_conn_rel_req_for_both_domains = FALSE;
    break;
    
  case RRC_NEW_CELL_IND:          
  /* The UE is already in CELL_FACH State. So Ignore the command since Cell
    Update would have been initiated */
    MSG_MED("Unexpected event:%d in the substate: %d", 
      cmd_ptr->cmd_hdr.cmd_id, rrcrce_substate,0); 
    break;
    
  case RRC_ABORT_REQ:
  /* Another RRC_ABORT_REQ is received. Just set the flag again just in 
  * case it wasn't set before
    */
    rrcrce_send_abort_cnf = TRUE;


    if(cmd_ptr->cmd.rrc_abort_req.hard_abort == TRUE)
    {
      WRRC_MSG0_HIGH("Received HARD_ABORT flag from NAS");

      /* Send initiate cell sel command to CSP so that UE is taken to Idle */
      rrc_transition_to_disconnected_state(RRC_PROCEDURE_RCE, RRC_TX_TO_DISCON_HARD_ABORT);

      /* stop the timer for UE before it can release RRC Connection */
      rrctmr_stop_timer(RRCTMR_RRC_CON_DELAY_TIMER);
    
      /* Wait for successful release of SRBs */
      rrcrce_substate = RRCRCR_WAIT_FOR_SRB_REL_CNF;
      rrcrce_send_abort_cnf = TRUE;
      break;
    }

#ifdef FEATURE_DUAL_SIM
    /* For PS abort, stop timer and send SCRI if not already sent, or release the connection */
    /*If not PS abort, break*/
    if(cmd_ptr->cmd.rrc_abort_req.abort_cause == RRC_DUAL_STANDBY_PS_ABORT)
    {
      rrctmr_stop_timer(RRCTMR_RRC_CON_DELAY_TIMER);
    }
    else
#endif
    {
      break;
    }
    
  case RRC_RRC_CON_DELAY_TIMER_EXPIRED_IND:  /*lint !e616 */
    /* Release the Signalling Radio Bearer irrespective of the status */
    WRRC_MSG0_HIGH("RRC con rel delay tmr expired");
    if (rrcrcr_is_proc_active())
    {
      break;
    }
    #ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
    if(csfb_extended_status == CSFB_CS_RB_RELEASED)
    {
      WRRC_MSG0_HIGH("CSFB_HACK: Conn release timer expired.");

      if(rrccho_is_wtog_cco_active())
      {
        WRRC_MSG0_HIGH("CSFB_HACK:WTOG CCO Active. Do not initialte cell selection");
        break;
      }
#ifdef FEATURE_WRLF_SYSTEM_SEL
      /* If OOS search is in progress, restart the connection release timer till
               1. UE comes back to service.
               2. Re-establishment timers expire and RRC connection is released */
      if(RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS())
      {
        /* start the timer for UE before it can release RRC Connection */
        rrctmr_start_timer(RRCTMR_RRC_CON_DELAY_TIMER, 
          (unsigned long) rrc_fast_return_to_lte_after_csfb_timer_nv); 
        
        WRRC_MSG1_HIGH("CSFB_HACK:Re-Started wait for RRC conn release timer of value:%d ms, due to OOS",
          rrc_fast_return_to_lte_after_csfb_timer_nv); 

        break;
      }
#endif

      /* In URA-PCH if we close the connection locally without the first page, then there is a chance of out of sync.So we should run the timer atleast for 1 page*/
      if (((rrc_get_state() == RRC_STATE_CELL_PCH) || (rrc_get_state() == RRC_STATE_URA_PCH) 
          ||(OC_SET_FOR_DCH_CELL_PCH_TRANS == oc_status) 
          ||(OC_SET_FOR_DCH_URA_PCH_TRANS == oc_status)
          ||(OC_SET_FOR_FACH_CELL_PCH_TRANS == oc_status)
          ||(OC_SET_FOR_FACH_URA_PCH_TRANS == oc_status))
          &&(rrc_crt_restarted_once == FALSE)
          )
      { 
        current_drx_cycle_ms = 10*(rrcllc_get_drx_info());

        if(current_drx_cycle_ms == 0)
        {
          current_drx_cycle_ms = RRC_DEFAULT_DRX_CYCLE_MS;
          WRRC_MSG1_HIGH("CSFB_HACK:Using default DRX cycle lenght (%d in ms)",RRC_DEFAULT_DRX_CYCLE_MS);
        }

        MSG_HIGH("CSFB_HACK:Check  Restart wait %d %d %d",
          rrc_fast_return_to_lte_after_csfb_timer_nv,current_drx_cycle_ms,RRC_GUARD_TIMER_FOR_PCH_TRANS_BEFORE_WTOL_FAST_REDIR); 

        /* Guard timer value is 3s. 2sec for SIB reading during DCH->PCH trans and 1sec for CU after PAGE reception. */
        if ((RRC_NUM_DRX_CYCLE_BEFORE_WTOL_FAST_REDIR * current_drx_cycle_ms) + RRC_GUARD_TIMER_FOR_PCH_TRANS_BEFORE_WTOL_FAST_REDIR 
                                        > rrc_fast_return_to_lte_after_csfb_timer_nv ) 
        {
           rrctmr_start_timer(RRCTMR_RRC_CON_DELAY_TIMER, 
             (unsigned long) ( (2*current_drx_cycle_ms) + RRC_GUARD_TIMER_FOR_PCH_TRANS_BEFORE_WTOL_FAST_REDIR - rrc_fast_return_to_lte_after_csfb_timer_nv )); 
           rrc_crt_restarted_once = TRUE;
           break;
        }
      }
    }
#endif
    if (rrcrce_sig_con_delay == TRUE)
    {
      /* Now Send Sig Connection Release Req */
      if(!rrcscr_get_signalling_connection_status(RRC_CS_DOMAIN_CN_ID) && 
        !rrcscr_get_signalling_connection_status(RRC_PS_DOMAIN_CN_ID )                                     )
      {
        /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
        rrcrce_send_rrc_initiate_cell_selection_ind(
           RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
          /* Wait for successful release of SRBs */
          rrcrce_substate = RRCRCR_WAIT_FOR_SRB_REL_CNF;
        }    
      else
      {
        if(rrcscr_get_signalling_connection_status(RRC_CS_DOMAIN_CN_ID) )
        {
          if (rrcrcr_is_proc_active())
          {
            break;
          }
          
          /* Allocates the buffer to RRC Internal command */
          rrcrce_close_sess_cmd_ptr =  rrc_get_int_cmd_buf();
          rrcrce_close_sess_cmd_ptr->cmd_hdr.cmd_id = RRC_SIG_CONN_RELEASE_REQ;
          rrcrce_close_sess_cmd_ptr->cmd.rrc_sig_conn_req.cn_domain_id =
              RRC_CS_DOMAIN_CN_ID;
          rrcrce_close_sess_cmd_ptr->cmd.rrc_sig_conn_req.
          sig_rel_complete_cnf_required = TRUE;
           rrcrce_close_sess_cmd_ptr->cmd.rrc_sig_conn_req.rrc_proc = RRC_PROCEDURE_RCE;
          /* sends the command to Signalling Connection Release Request Proc */
          rrc_put_int_cmd(rrcrce_close_sess_cmd_ptr);  
          cnt++;
        }
        if(rrcscr_get_signalling_connection_status(RRC_PS_DOMAIN_CN_ID) )
        {
          if (rrcrcr_is_proc_active())
          {
            break;
          }
          
          /* Allocates the buffer to RRC Internal command */
          rrcrce_close_sess_cmd_ptr =  rrc_get_int_cmd_buf();
            rrcrce_close_sess_cmd_ptr->cmd_hdr.cmd_id = RRC_SIG_CONN_RELEASE_REQ;
            rrcrce_close_sess_cmd_ptr->cmd.rrc_sig_conn_req.cn_domain_id =
              RRC_PS_DOMAIN_CN_ID;
            rrcrce_close_sess_cmd_ptr->cmd.rrc_sig_conn_req.
              sig_rel_complete_cnf_required = TRUE;
            rrcrce_close_sess_cmd_ptr->cmd.rrc_sig_conn_req.rrc_proc = RRC_PROCEDURE_RCE;
            /* sends the command to Signalling Connection Release Request Proc */
            rrc_put_int_cmd(rrcrce_close_sess_cmd_ptr);  
            cnt++;
          }
        if (cnt == 2)
        {
          WRRC_MSG0_HIGH("SigConRelease req for both domains");
          rrcrce_sig_conn_rel_req_for_both_domains = TRUE;
        }
        
        /* Wait for signalling connection release to complete */
        rrcrce_substate = RRCRCE_WAIT_FOR_SIG_CON_REL_COMPL;
        
      }
      
      break;
      
    }
    /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
       rrcrce_send_rrc_initiate_cell_selection_ind(
         RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
      /* Wait for successful release of SRBs */
      rrcrce_substate = RRCRCR_WAIT_FOR_SRB_REL_CNF;                        
    break;
    
    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
        cmd_ptr->cmd_hdr.cmd_id, rrcrce_substate);
      break;
      
   }  
} /* rrcrce_process_rce_wait_for_rrc_timer_expire_substate*/
/*===========================================================================
FUNCTION rrcrce_process_rce_final_substate

DESCRIPTION
  This function processes all events that are dispatched to RRC Connection
  Establishment procedure in RRCRCE_FINAL substate of either CELL_FACH 
  state or CELL_DCH State or CELL_PCH state or URA_PCH state.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_process_rce_final_substate
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  rrc_cmd_type *rrcrce_close_sess_cmd_ptr;       /* Pointer to command to
                                                RRC_SIG_CONN_RELEASE_REQ */
  uint32 cnt = 0;
  switch (cmd_ptr->cmd_hdr.cmd_id)
  {

    case RRC_STATE_CHANGE_IND:
      if((cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED) &&
         ( rrc_get_state() == RRC_STATE_DISCONNECTED))
      {
        if((RRC_MODE_ACTIVE == rrcmcm_get_rrc_mode())
          /* For inter-RAT redirection due to connection release, handle STATE_CHANGE_IND
           * in STANDBY_MODE as RRC releases all channels before starting redirection search
           */
           || ((RRC_MODE_STANDBY == rrcmcm_get_rrc_mode())
               && (rrccsp_get_curr_scan() == RRC_CSP_INTER_RAT_REDIRECTION_SCAN))
#ifdef FEATURE_WRLF_SYSTEM_SEL
           || (RRC_IS_CONN_MODE_OOS_SRCH_IN_PROGRESS() && 
                  (RRC_MODE_INACTIVE == rrcmcm_get_rrc_mode()))
#endif
          )
        {
          /* Check if abort is pending */
          if (rrcrce_send_abort_cnf == TRUE) 
          {
            rrcrce_send_rrc_abort_cnf();
              /* Clear the ESTABLISHED_RABS while bringing down RRC Connection*/
              rrc_clear_established_rabs();
              
              /* Reset Signalling Connections status */
              rrcscr_reset_signalling_connection_status();
              
              rrcrce_substate = RRCRCE_INITIAL;
            }
            else
            {
              rrcrce_send_rrc_rel_ind();
              /* Clear the ESTABLISHED_RABS while bringing down RRC Connection*/
              rrc_clear_established_rabs();

              /* Reset Signalling Connections status */
              rrcscr_reset_signalling_connection_status();

              rrcrce_substate = RRCRCE_INITIAL;
            }
        }
        else
        {
          /* Don't send the REL_IND to MM */
          /* Clear the ESTABLISHED_RABS while bringing down RRC Connection*/
          rrc_clear_established_rabs();

          /* Reset Signalling Connections status */
          rrcscr_reset_signalling_connection_status();

          rrcrce_substate = RRCRCE_INITIAL;
        }
      }
      break;

    case RRC_ABORT_REQ:
      if((!rrcscr_get_signalling_connection_status(RRC_CS_DOMAIN_CN_ID) && 
        !rrcscr_get_signalling_connection_status(RRC_PS_DOMAIN_CN_ID)) ||
         (cmd_ptr->cmd.rrc_abort_req.auth_failed_bar_cells))
      {
        WRRC_MSG1_HIGH("No sig conn rel required. auth_failed = %d", 
                   cmd_ptr->cmd.rrc_abort_req.auth_failed_bar_cells);
        /* Do not send command to LLC to release SRBs Instead send Initiate Cell Selection Indication */
           rrcrce_send_rrc_initiate_cell_selection_ind(
           RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
        {     
          /* Wait for successful release of SRBs */
          rrcrce_substate = RRCRCR_WAIT_FOR_SRB_REL_CNF;
          rrcrce_send_abort_cnf = TRUE;
        }    
      }
      else
      {
        if((cmd_ptr->cmd.rrc_abort_req.hard_abort == TRUE)
#ifdef FEATURE_DUAL_SIM
         /* To handle the case where PS standy abort is received when RRC is in OOS
            and waiting to acquire lock */
              || ((cmd_ptr->cmd.rrc_abort_req.abort_cause == RRC_DUAL_STANDBY_PS_ABORT) &&
                  (RRC_NOT_CAMPED_ON == rrc_ccm_get_curr_camping_status(RRC_PROCEDURE_RCE)))
#endif
           )
        {
          WRRC_MSG0_HIGH("Received HARD_ABORT flag from NAS");

          /* Send initiate cell sel command to CSP so that UE is taken to Idle */
          rrc_transition_to_disconnected_state(RRC_PROCEDURE_RCE, RRC_TX_TO_DISCON_HARD_ABORT);

          /* Wait for successful release of SRBs */
          rrcrce_substate = RRCRCR_WAIT_FOR_SRB_REL_CNF;
          rrcrce_send_abort_cnf = TRUE;
          /* Don't send SCRI for hard abort cases */
          break;
        }

        if(rrcscr_get_signalling_connection_status(RRC_CS_DOMAIN_CN_ID) )
        {
          rrcrce_send_abort_cnf = TRUE;
          if (rrcrcr_is_proc_active())
          {
            break;
          }
          /* atleast one of the signalling connection was up */
          if (cmd_ptr->cmd.rrc_abort_req.delay_sig_con_release == TRUE)
          {
            WRRC_MSG0_HIGH("NAS flag for RRCConnRel TRUE");
            rrcrce_sig_con_delay = TRUE;
#ifndef FEATURE_WCDMA_FIELD_OPTIMIZATIONS
            /*This will Normal behaviour*/
            rrctmr_start_timer(RRCTMR_RRC_CON_DELAY_TIMER, 
                               (unsigned long) RRCRCE_DELAY_TIMER_VALUE_FOR_SIG_CON_REL); 
            WRRC_MSG1_HIGH("Started RRC con delay timer :%d",
                    RRCRCE_DELAY_TIMER_VALUE_FOR_SIG_CON_REL); 
#else
            /*This will for feild optimization if FEATURE_WCDMA_FIELD_OPTIMIZATIONS is defined*/
            /* start the timer for UE to camp on */
            rrctmr_start_timer(RRCTMR_RRC_CON_DELAY_TIMER, 
                               (unsigned long) RRCRCE_ZERO_TIMER_VALUE_FOR_RRC_CON_REL);
            WRRC_MSG0_HIGH("Started RRC con timer with 0 value"); 
#endif
            
            rrcrce_substate = RRCRCE_WAIT_FOR_RRC_TIMER_TO_EXPIRE;
            
            break;
          }
          else
          {
            /* Allocates the buffer to RRC Internal command */
              rrcrce_close_sess_cmd_ptr =  rrc_get_int_cmd_buf();
              rrcrce_close_sess_cmd_ptr->cmd_hdr.cmd_id = RRC_SIG_CONN_RELEASE_REQ;
              rrcrce_close_sess_cmd_ptr->cmd.rrc_sig_conn_req.cn_domain_id =
                RRC_CS_DOMAIN_CN_ID;
              rrcrce_close_sess_cmd_ptr->cmd.rrc_sig_conn_req.
                sig_rel_complete_cnf_required = TRUE;
              rrcrce_close_sess_cmd_ptr->cmd.rrc_sig_conn_req.rrc_proc = RRC_PROCEDURE_RCE;
              /* sends the command to Signalling Connection Release Request Proc */
              rrc_put_int_cmd(rrcrce_close_sess_cmd_ptr);  
              cnt++;
          }
        }
        if(rrcscr_get_signalling_connection_status(RRC_PS_DOMAIN_CN_ID) )
        {
          rrcrce_send_abort_cnf = TRUE;
          if (rrcrcr_is_proc_active())
          {
            break;
          }
          /* atleast one of the signalling connection was up */
          if (cmd_ptr->cmd.rrc_abort_req.delay_sig_con_release == TRUE)
          {
            WRRC_MSG0_HIGH("NAS flag for RRCConnRel TRUE");
            rrcrce_sig_con_delay = TRUE;
#ifndef FEATURE_WCDMA_FIELD_OPTIMIZATIONS
            /*This will Normal behaviour*/
            rrctmr_start_timer(RRCTMR_RRC_CON_DELAY_TIMER, 
                               (unsigned long) RRCRCE_DELAY_TIMER_VALUE_FOR_SIG_CON_REL); 
            WRRC_MSG1_HIGH("Started RRC con  delay timer :%d",
                     RRCRCE_DELAY_TIMER_VALUE_FOR_SIG_CON_REL); 
#else
            /*This will for feild optimization if WCDMA_FIELD_OPTIMIZATIONS is defined*/
            /* start the timer for UE to camp on */
            rrctmr_start_timer(RRCTMR_RRC_CON_DELAY_TIMER, 
                               (unsigned long) RRCRCE_ZERO_TIMER_VALUE_FOR_RRC_CON_REL);
            WRRC_MSG0_HIGH("Started RRC con delay timer with 0 value"); 
#endif
            
            rrcrce_substate = RRCRCE_WAIT_FOR_RRC_TIMER_TO_EXPIRE;
            
            break;
          }
          else
          {
            /* Allocates the buffer to RRC Internal command */
              rrcrce_close_sess_cmd_ptr =  rrc_get_int_cmd_buf();
              rrcrce_close_sess_cmd_ptr->cmd_hdr.cmd_id = RRC_SIG_CONN_RELEASE_REQ;
              rrcrce_close_sess_cmd_ptr->cmd.rrc_sig_conn_req.cn_domain_id =
                RRC_PS_DOMAIN_CN_ID;
              rrcrce_close_sess_cmd_ptr->cmd.rrc_sig_conn_req.
                sig_rel_complete_cnf_required = TRUE;
              rrcrce_close_sess_cmd_ptr->cmd.rrc_sig_conn_req.rrc_proc = RRC_PROCEDURE_RCE;
              /* sends the command to Signalling Connection Release Request Proc */
              rrc_put_int_cmd(rrcrce_close_sess_cmd_ptr);  
              cnt++;
          }
        }
        if (cnt == 2)
        {
          WRRC_MSG0_HIGH("SigConRelease req for both domains");
          rrcrce_sig_conn_rel_req_for_both_domains = TRUE;
        }
        /* Wait for signalling connection release to complete */
        rrcrce_substate = RRCRCE_WAIT_FOR_SIG_CON_REL_COMPL;
        
        rrcrce_send_abort_cnf = TRUE;

#ifdef FEATURE_DUAL_SIM
        if(cmd_ptr->cmd.rrc_abort_req.abort_cause == RRC_DUAL_STANDBY_PS_ABORT)
        {
          rrcrce_sig_con_delay = TRUE;
        }
#endif
      }
      
      break; 

    case RRC_DOWNLINK_L2ACK_IND:
       WRRC_MSG1_HIGH("L2 ack status RRC ConnSetCompl %d",cmd_ptr->cmd.downlink_l2ack_ind.status);
       break;

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
    case RRC_INTERNAL_CSFB_ABORT_REQ:
      WRRC_MSG1_HIGH("CSFB_HACK:RRC_INTERNAL_CSFB_ABORT_REQ rcvd in %d substate",rrcrce_substate);

      if(rrcscr_get_signalling_connection_status(RRC_PS_DOMAIN_CN_ID))
      {
        WRRC_MSG0_HIGH("CSFB_HACK:PS domain open, send SCRI.");

        /* Allocates the buffer to RRC Internal command */
          rrcrce_close_sess_cmd_ptr =  rrc_get_int_cmd_buf();          
          rrcrce_close_sess_cmd_ptr->cmd_hdr.cmd_id = RRC_SIG_CONN_RELEASE_REQ;
          rrcrce_close_sess_cmd_ptr->cmd.rrc_sig_conn_req.cn_domain_id =
            RRC_PS_DOMAIN_CN_ID;
          rrcrce_close_sess_cmd_ptr->cmd.rrc_sig_conn_req.
            sig_rel_complete_cnf_required = TRUE;
          rrcrce_close_sess_cmd_ptr->cmd.rrc_sig_conn_req.rrc_proc = RRC_PROCEDURE_RCE;
          /* sends the command to Signalling Connection Release Request Proc */
          rrc_put_int_cmd(rrcrce_close_sess_cmd_ptr);  
        /* Wait for signalling connection release to complete */
          rrcrce_substate = RRCRCE_WAIT_FOR_SIG_CON_REL_COMPL;
      }
      else
      {
        WRRC_MSG1_HIGH("CSFB_HACK:PS domain not open. Started wait %d ms.",
          rrc_fast_return_to_lte_after_csfb_timer_nv);

        /* start the timer for UE before it can release RRC Connection */
        rrctmr_start_timer(RRCTMR_RRC_CON_DELAY_TIMER, 
                (unsigned long) rrc_fast_return_to_lte_after_csfb_timer_nv); 
        
        rrcrce_substate = RRCRCE_WAIT_FOR_RRC_TIMER_TO_EXPIRE;
      }
      rrcrce_sig_con_delay = FALSE;
      break;
#endif
    default:
      WRRC_MSG2_HIGH("Invalid Event: %d in the substate: %d", 
                cmd_ptr->cmd_hdr.cmd_id,rrcrce_substate);
      break;
  }  

} /* rrcrce_process_rce_final_substate */

/*===========================================================================
FUNCTION rrcrce_get_rrc_connection_status

DESCRIPTION
  This function looks into RRC Connection Establishment provcedure's 
  sub-states and returns the status of RRC Connection.

DEPENDENCIES
  None

RETURN VALUE
  rrcrce_rrc_conn_status_e_type: Returns RRCRCE_RRC_CONN_ESTABLISHED if RRC
  Connection Establishment procedure is in RRCRCE_FINAL substate. Otherwise
  it returns RRCRCE_RRC_CONN_NOT_ESTABLISHED.

SIDE EFFECTS
  None
===========================================================================*/
rrcrce_rrc_conn_status_e_type rrcrce_get_rrc_connection_status
(
  void                  
)
{
  if(rrcrce_substate != RRCRCE_FINAL) 
  {
    /* RRC Connection is not established */
    return ( RRCRCE_RRC_CONN_NOT_ESTABLISHED );
  }
  else
  {
    /* RRC Connection is established */
    return ( RRCRCE_RRC_CONN_ESTABLISHED );
  }

} /* rrcrce_get_rrc_connection_status */
/*===========================================================================
FUNCTION rrcrce_is_rce_procedure_inactive

DESCRIPTION
  This function looks into RRC Connection Establishment provcedure's 
  sub-states and returns the status of RRC Connection Establishment
  Procedure.

DEPENDENCIES
  None

RETURN VALUE
  boolean: TRUE if RCE is still inactive. (Note - when RCE is waiting for 
           UE to camp, the function will still return TRUE) 
           FALSE when RCE is not in Initial substate and UE is camped on 
           a cell.

SIDE EFFECTS
  To be used only by CSP as the logic is designed for CSP's requirement.
===========================================================================*/
boolean  rrcrce_is_rce_procedure_inactive(void)
{
  if ((rrcrce_substate == RRCRCE_INITIAL) ||
      (rrcrce_substate == RRCRCE_WAIT_FOR_UE_CAMP_ON))
  {
    /* RCE will not be considered "active" if in one
     * of these substates.
     */
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
} /*rrcrce_is_rce_procedure_inactive */

/*===========================================================================
FUNCTION rrcrce_change_substate_to_rrcrce_final

DESCRIPTION
  This function changes the substate of RRC Connection Establishment provcedure's 
  to RRCRCE_FINAL

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  To be used only during G2W Handover and should not be used for any other 
  purpose.
===========================================================================*/
void rrcrce_change_substate_to_rrcrce_final
(
  void
)
{
  WRRC_MSG1_HIGH("Changing substate %d to Final",rrcrce_substate);
  rrcrce_substate = RRCRCE_FINAL;

   rrcrce_send_abort_cnf = FALSE;
   /* Reset the rrcrce_num_retries_count */
   rrcrce_num_retries_count =0;
}

/*===========================================================================
FUNCTION rrcrce_validate_access_class

DESCRIPTION
  This function validates access class of CSP

DEPENDENCIES
  None

RETURN VALUE
  rrcrce status

SIDE EFFECTS
  None
===========================================================================*/

rrcrce_status_e_type rrcrce_validate_cell_access_status
(
  rrc_cn_domain_identity_e_type   cn_domain_id,
  rrc_establish_cause_e_type   establishment_cause,
  rrc_abort_cause_e_type  *cause              /* cause */
)
{
  mm_as_cell_access_status_T cell_access_status;
  rrcrce_status_e_type status = RRCRCE_FAILURE_MINOR;

/* PPAC */
  boolean reg_allowed;
  boolean page_rsp_allowed;

  if((establishment_cause == RRC_EST_REGISTRATION) ||
     (establishment_cause == RRC_EST_INTER_RAT_CELL_RESELECT))
  {
    reg_allowed = rrc_csp_get_loc_reg_allowed_status(cn_domain_id);
    if(reg_allowed)
    {
      WRRC_MSG1_HIGH("PPAC: Loc/Reg allowed for %d domain",cn_domain_id);
      return (status = RRCRCE_SUCCESS);
    }
    else if(!reg_allowed && rrc_csp_is_ppac_valid())
    {
      WRRC_MSG1_HIGH("PPAC: Loc/Reg not allowed for %d domain",cn_domain_id);
      *cause = RRC_ACCESS_CLASS_NOT_ALLOWED;
      return (status = RRCRCE_FAILURE_MINOR);
    }
  }
  else if((establishment_cause == RRC_EST_TERM_CONV_CALL) || 
          (establishment_cause == RRC_EST_TERM_HI_SIGNALLING) ||
          (establishment_cause == RRC_EST_TERM_STREAM_CALL) ||
          (establishment_cause == RRC_EST_TERM_BACKGND_CALL) ||
          (establishment_cause == RRC_EST_TERM_INTERACT_CALL) ||
          (establishment_cause == RRC_EST_TERM_LOW_SIGNALLING) ||
          (establishment_cause == RRC_EST_TERM_UNKNOWN)
         )
  {
    page_rsp_allowed = rrc_csp_get_page_rsp_allowed_status(cn_domain_id);
    if(page_rsp_allowed)
    {
      WRRC_MSG1_HIGH("PPAC: Page rsp allowed for %d domain",cn_domain_id);
      return (status = RRCRCE_SUCCESS);
    }
  }

  /*Get the current cell access status */
  cell_access_status = rrc_csp_get_curr_cell_access_status(cn_domain_id);
  WRRC_MSG1_HIGH("cell access status from CSP internal database=%d ",cell_access_status);
  
  switch (cell_access_status)
  {
    case MM_AS_ACCESS_ALL_CALLS:
      {
        status = RRCRCE_SUCCESS;
        WRRC_MSG1_HIGH("Cell Access allowed:MM_AS_ACCESS_ALL_CALLS, status=%d",status);
      }
      break;

    case MM_AS_ACCESS_EMERGENCY_ONLY:
      if(establishment_cause == RRC_EST_EMERGENCY_CALL)
      {
        status = RRCRCE_SUCCESS;
        WRRC_MSG1_HIGH("Cell Acces allowed:MM_AS_ACCESS_EMERGENCY_ONLY, status=%d",status);
      }
      else
      {
        *cause = RRC_ACCESS_CLASS_NOT_ALLOWED;
        status = RRCRCE_FAILURE_MINOR;
        WRRC_MSG2_HIGH("Cell Acces not allowed for regular voice calls status=%d, cause=%d ",status,*cause);
      }
      break;

    case MM_AS_ACCESS_NO_CALLS:
    {
      *cause = RRC_ACCESS_CLASS_NOT_ALLOWED;
      status = RRCRCE_FAILURE_MINOR;
      WRRC_MSG2_HIGH("Cell Acces not allowed for any calls status=%d, cause=%d ",status,*cause);
    }
    break;

    case MM_AS_ACCESS_NORMAL_ONLY:
      if(establishment_cause != RRC_EST_EMERGENCY_CALL)
      {
        status = RRCRCE_SUCCESS;
        WRRC_MSG0_HIGH("Cell Acces allowed for regular voice calls");
      }
      else
      {
        *cause = RRC_ACCESS_CLASS_NOT_ALLOWED;
        status = RRCRCE_FAILURE_MINOR;
        WRRC_MSG2_HIGH("Cell Access not allowed for the calls other than normal/regular voice calls status=%d, cause=%d ",status,*cause);
      }
    break;
  }
  return status;
}

/*===========================================================================
FUNCTION rrcrce_force_initialize

DESCRIPTION
  This function changes the substate of RRC Connection Establishment provcedure's 
  to RRCRCE_INITIAL and sends the required Indication/Response to MM depending
  on RRCRCE substate. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  To be used only during Mode Change and when an Out of Service Area is indicated
  to MM while in IDLE DISCONNECTED state.
  It should not be used for any other purpose.
===========================================================================*/

void rrcrce_force_initialize
(
  boolean initiate_cell_selection
)
{
  WRRC_MSG1_HIGH("Force Initialize.Changing substate %d to Initial",rrcrce_substate);

  rrcrce_cell_resel_freq = 0;
  rrcrce_cell_resel_scr = 0;

  if(rrccho_is_gtow_pcco_in_progress())
  {
    /* No need to send the CONN_SETUP_IND to CHO */
    /* set rce_substate to initial */
    rrcrce_substate = RRCRCE_INITIAL;

    /* Reset the rrcrce_num_retries_count */
    rrcrce_num_retries_count = 0;

    rrcrce_send_abort_cnf = FALSE;

    return;
  }
  
  if(rrcrce_substate == RRCRCE_INITIAL)
  {
    rrcrce_substate = RRCRCE_INITIAL;
    /* Reset the rrcrce_num_retries_count */
    rrcrce_num_retries_count = 0;

    rrcrce_send_abort_cnf = FALSE;
  }
  else if((rrcrce_substate == RRCRCE_WAIT_FOR_UE_CAMP_ON) ||
          (rrcrce_substate == RRCRCE_WAIT_FOR_SIB7_CURRENT))
  {
    if(rrcrce_substate != RRCRCE_WAIT_FOR_SIB7_CURRENT)
    {
      /* Stop the timer to UE to camp on */  
      rrctmr_stop_timer(RRCTMR_UE_TO_CAMP_ON_TIMER);
    }

    /* Send RRC_ABORT_IND command to the MM */
    rrcrce_send_rrc_abort_ind(RRC_FAIL_TO_CAMP_ON);      
      /* The RCE Procedure ends */
      rrcrce_substate = RRCRCE_INITIAL;    /* reset the RCE substate */
      /* Reset the rrcrce_num_retries_count */
      rrcrce_num_retries_count = 0;

      rrcrce_send_abort_cnf = FALSE;
    }    
  else if (rrcrce_substate == RRCRCE_WAIT_FOR_RB0_REL_CNF)
  {
    if (rrcrce_send_abort_cnf == TRUE)
    {
      /* Send RRC_ABORT_CNF command to the MM */
      rrcrce_send_rrc_abort_cnf();      
      /* At this time, none of the RBs are setup. Simply reset the RCE
        substate */
        rrcrce_substate = RRCRCE_INITIAL; /* The RCE Procedure ends */
        /* Reset the rrcrce_num_retries_count */
        rrcrce_num_retries_count = 0;
        
        return;
      }    
      else
      {     
      /* Send RRC_ABORT_IND command to the MM */
        rrcrce_send_rrc_abort_ind(RRC_FAILURE_UNSPECIFIED);      
        /* The RCE Procedure ends */
        rrcrce_substate = RRCRCE_INITIAL;    /* reset the RCE substate */
        /* Reset the rrcrce_num_retries_count */
        rrcrce_num_retries_count = 0;
        return;
      }    
  }
  else if (rrcrce_substate == RRCRCR_WAIT_FOR_SRB_REL_CNF)
  {
      /* Send RRC_ABORT_CNF command to the MM */
      rrcrce_send_rrc_abort_cnf();     
      /* At this time, none of the RBs are setup. Simply reset the RCE
      substate */
      rrcrce_substate = RRCRCE_INITIAL; /* The RCE Procedure ends */
      /* Reset the rrcrce_num_retries_count */
      rrcrce_num_retries_count = 0;
      
      /* Call this fn to make sure that Establised RABS contains invalid values */
      rrc_clear_established_rabs();
      return;
    }    
  else if ((rrcrce_substate == RRCRCE_WAIT_FOR_RRC_CONN_SETUP) ||
           (rrcrce_substate == RRCRCE_WAIT_FOR_RESELCT_COMPL) ||
           (rrcrce_substate == RRCRCE_WAIT_FOR_REDIRECT_CNF))
  {
    if(initiate_cell_selection)
    {
      /* Send RRC_ABORT_IND command to the MM */
      rrcrce_send_rrc_abort_ind(RRC_FAIL_TO_CAMP_ON);
      /* Stop the timer to UE to camp on */  
      rrctmr_stop_timer(RRCTMR_T_300_TIMER);
      rrcrce_abort_ind_sent = TRUE;
        /* Release the RB 0 and request the RRC LLC to go to Idle disconnected */
      rrcrce_send_rrc_initiate_cell_selection_ind( RRC_TRANSITION_TO_DISCONNECTED, RRC_STATE_DISCONNECTED);
      /* The RCE Procedure ends */
      rrcrce_substate = RRCRCE_WAIT_FOR_RB0_REL_CNF; /* change the RCE substate */
    }
    else
    {
      /* Send RRC_ABORT_IND command to the MM */
      rrcrce_send_rrc_abort_ind(RRC_MAX_ACCESS_FAILURE);
      /* Stop the timer to UE to camp on */  
      rrctmr_stop_timer(RRCTMR_T_300_TIMER);
      rrcrce_substate = RRCRCE_INITIAL;
      /*Set rrcrce_abort_ind_sent to FALSE as RCE is moving to initial substate*/
      rrcrce_abort_ind_sent = FALSE;
    }    
    /* Reset the rrcrce_num_retries_count */
    rrcrce_num_retries_count = 0;
    rrcrce_send_abort_cnf = FALSE;
    rrcrce_lai_rai_mismatch = FALSE;
  }    
#ifdef FEATURE_WCDMA_HANDLE_MDSP_HALT
  else if ((rrcrce_substate == RRCRCE_WAIT_FOR_RB0_SETUP_CNF) ||
           (rrcrce_substate == RRCRCE_WAIT_FOR_WAIT_TIMER_TO_EXPIRE) ||
           (rrcrce_substate == RRCRCE_WAIT_FOR_SRB_SETUP_CNF) ||
           (rrcrce_substate == RRCRCE_WAIT_FOR_L2_ACK) ||
           (rrcrce_substate == RRCRCE_WAIT_FOR_RB0_PHYCHAN_RESETUP_CNF))
  {
    if (rrcrce_send_abort_cnf == TRUE)
    {
      /* Send RRC_ABORT_CNF command to the MM */
      rrcrce_send_rrc_abort_cnf();     
      /* At this time, none of the RBs are setup. Simply reset the RCE
        substate */
        rrcrce_substate = RRCRCE_INITIAL; /* The RCE Procedure ends */
        /* Reset the rrcrce_num_retries_count */
        rrcrce_num_retries_count = 0;
        
        return;
      }    
      else
      {     
      /* Send RRC_ABORT_IND command to the MM */
      rrcrce_send_rrc_abort_ind(RRC_INTERNAL_ERROR);
        /* Stop the timer to UE to camp on */  
        rrctmr_stop_timer(RRCTMR_T_300_TIMER);
        rrcrce_substate = RRCRCE_INITIAL;
        /*Set rrcrce_abort_ind_sent to FALSE as RCE is moving to initial substate*/
        rrcrce_abort_ind_sent = FALSE;
      /* Reset the rrcrce_num_retries_count */
      rrcrce_num_retries_count = 0;
      rrcrce_send_abort_cnf = FALSE;
    }
  }
#endif
}

/*===========================================================================

FUNCTION   rrcrce_get_sib16_ptr

DESCRIPTION

  This function gets the Sib16 ptr either from the actual sib16 or the
  SIB16 version of Default Configuration
  
DEPENDENCIES

  None.
 
RETURN VALUE

  rrc_SysInfoType16 pointer

SIDE EFFECTS

  None.

===========================================================================*/
struct rrc_SysInfoType16 * rrcrce_get_sib16_ptr()
{
  struct rrc_SysInfoType16 * sib16_ptr = NULL;

  if(rrcrce_default_config_info.config_type == RRC_DEFAULT_CONFIG)
  {
    sib16_ptr = rrcrce_default_config_info.sib16_ptr;
  }

  return sib16_ptr;
}

/*===========================================================================

FUNCTION   rrcrce_get_rbs_ptr

DESCRIPTION

  This function gets the RBS ptr either from the actual RBS or the
  RBS version of Default Configuration
  
DEPENDENCIES

  None.
 
RETURN VALUE

  rrc_DL_DCCH_Message pointer

SIDE EFFECTS

  None.

===========================================================================*/
struct rrc_DL_DCCH_Message * rrcrce_get_rbs_ptr(void)
{
  struct rrc_DL_DCCH_Message * rbs_ptr = NULL;

  if(rrcrce_default_config_info.config_type == RRC_DEFAULT_CONFIG)
  {
    rbs_ptr = rrcrce_default_config_info.rbs_ptr;
  }

  return rbs_ptr;
}

/*===========================================================================

FUNCTION   rrcrce_get_rb_setup_r6_ptr_for_dc

DESCRIPTION

  This function gets the default config ptr from RB setup r6 I.E.
  
DEPENDENCIES

  None.
 
RETURN VALUE

  rrc_RadioBearerSetup_r6_IEs pointer

SIDE EFFECTS

  None.

===========================================================================*/
struct rrc_RadioBearerSetup_r6_IEs * rrcrce_get_rb_setup_r6_ptr_for_dc
(
  void
)
{
  rrc_RadioBearerSetup_r6_IEs * rb_setup_r6_ptr = NULL;

  if(rrcrce_default_config_info.config_type == RRC_DEFAULT_CONFIG)
  {
    if(rrcrce_default_config_info.dcch_msg_ptr != NULL)
    {
      rb_setup_r6_ptr = &(rrcrce_default_config_info.dcch_msg_ptr->message.u.radioBearerSetup.
          u.later_than_r3.criticalExtensions.u.criticalExtensions.u.criticalExtensions.u.r6.radioBearerSetup_r6);
    }
  }
  WRRC_MSG1_HIGH("Default config ptr val = %x for HSPA",rb_setup_r6_ptr);
  
  return rb_setup_r6_ptr;
}
/*===========================================================================

FUNCTION   rrcrce_is_dc_config_on_hspa

DESCRIPTION

  This function will indicate if DC will configure RB's/SRB's on HSPA channel's
  i.e. EUL in UL & HSDPA on DL. Currently only DC-17 does config SRB's on 
  HSPA channel's.
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE -If DC will configure RB's/SRB's on HSPA channel's
  FASLE - other wise
  
SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcrce_is_dc_config_on_hspa
(
  void
)
{
  boolean is_dc_config_hspa = FALSE;

  /*DC-17 config's SRB's on HSPA channel's.*/
  if((rrcrce_default_config_info.config_type == RRC_DEFAULT_CONFIG)&&
    ((rrcrce_default_config_info.default_config_id == RRC_DEF_CONFIG_17)
             || (rrcrce_default_config_info.default_config_id == RRC_DEF_CONFIG_23)
    ))
  {
    WRRC_MSG1_HIGH("DC I.D =%d config on HSPA channels",
             rrcrce_default_config_info.default_config_id );
    is_dc_config_hspa = TRUE;
  }

  return is_dc_config_hspa;
}

/*===========================================================================
FUNCTION rrcrce_get_rrc_est_cause

DESCRIPTION
  Returns establishment cause

DEPENDENCIES
  None

RETURN VALUE
  Returns establishment cause

SIDE EFFECTS
  None
===========================================================================*/
rrc_establish_cause_e_type rrcrce_get_rrc_est_cause
(
  void
)
{
  WRRC_MSG1_HIGH("Est Cause is %d", rrc_establishment_cause);
  return rrc_establishment_cause;      
} /* rrcrce_get_rrc_est_cause */
#ifdef FEATURE_WCDMA_MTPL_THERMAL_BACKOFF 
/*===========================================================================
FUNCTION rrcrce_set_rrc_est_cause_for_emergency_call

DESCRIPTION
  Sets the establishment cause to RRC_EST_EMERGENCY_CALL

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_set_rrc_est_cause_for_emergency_call
(
  void
)
{
  WRRC_MSG1_HIGH("Est Cause is %d", rrc_establishment_cause);
  rrc_establishment_cause = RRC_EST_EMERGENCY_CALL;
  WRRC_MSG1_HIGH("Est Cause is %d", rrc_establishment_cause);
} /* rrcrce_get_rrc_est_cause */
#endif
/*===========================================================================
FUNCTION rrcrce_is_rce_procedure_inactive

DESCRIPTION
  This function looks into RRC Connection Establishment provcedure's 
  sub-states and returns the status of RRC Connection Establishment
  Procedure.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if RCE is in active. 
  FALSE: Otherwise 
SIDE EFFECTS
none
===========================================================================*/
boolean  rrcrce_is_rce_procedure_active(void)
{
  if(rrcrce_substate == RRCRCE_INITIAL)
  {
    /* RCE will not be considered "active" if in one
     * of these substates.
     */
    return(FALSE);
  }
  else
  {
    return(TRUE);
  }
} /*rrcrce_is_rce_procedure_active */
/*===========================================================================
FUNCTION rrcrce_get_rce_sub_state()

DESCRIPTION
  This function will return current RRC Connection Establishment 
  procedure's sub state.
DEPENDENCIES
  None

RETURN VALUE
  return current rce sub state.
SIDE EFFECTS
none
===========================================================================*/
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
rrcrce_substate_e_type  rrcrce_get_rce_sub_state(void)
{
  return rrcrce_substate;
} /*rrcrce_get_rce_sub_state*/

/*===========================================================================
FUNCTION rrcrce_check_for_rai_mismatch()

DESCRIPTION
  This function compares the camped on LAI/RAI with the one given in buffered EST_REQ to decide
  on whether to proceed with EST_REQ or reject it.
DEPENDENCIES
  None

RETURN VALUE
  TRUE: Mismatch and reject the buffered EST_REQ.
SIDE EFFECTS
none
===========================================================================*/
boolean rrcrce_check_for_rai_mismatch(rrc_plmn_identity_type camped_plmn, rrc_SysInfoType1 *sib1_ptr)
{
  rrc_rai_type    camped_on_rai;
  /* This stores the GSM-MAP NAS system info */
  rrc_gsm_map_nas_sys_info_type   nas_common_info;
  /* Pointer to SIB1 */
  unsigned char  *data_ptr;
  uint32         i;
  boolean compare_rai = FALSE;

  /* Fill up the NAS system info here */

  if( sib1_ptr == NULL )
  {
    // SIB1 for the camped cell is not present in the SIB Database.
    WRRC_MSG2_ERROR("SIB1 for MCC %d, MNC %d, is not present",
                        RRC_CSP_GET_INT_MCC(camped_plmn), 
                        RRC_CSP_GET_INT_MNC(camped_plmn)
                        );
    return FALSE;
  } // SIB1 not present 

  /* The SIB1 is present */
  nas_common_info.length = (uint32)sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts;

  /* Check if the RRC MM interface structure array can hold this info */
  if(sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts > RRC_MAX_GSM_INFO_OCTET_COUNT)
  {
   WRRC_MSG1_ERROR("Not enough space for Common NAS Sys Info (%d bytes)",
                       sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts);
   return FALSE;
  }

  /* Fill in the Common NAS system info now */
  data_ptr = sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.data;
  for( i = 0; i < sib1_ptr->cn_CommonGSM_MAP_NAS_SysInfo.numocts; i ++)
  {
    nas_common_info.value[i] =  *data_ptr;
    data_ptr ++;
  }
 
  /* Get the LAI status from local utility */
  /* TBD: Call MM utility to initialize lai first. */
  if(mm_extract_lac_from_sib_info(&camped_on_rai.lai, &nas_common_info) == FALSE)
  {
   WRRC_MSG0_ERROR("Could not extract LAI from NAS Common sys info");
   return FALSE;
  }

  /* Since MM doesn't know the PLMN from NAS system info, fill in the PLMN now */
  camped_on_rai.lai.plmn = camped_plmn;

  WRRC_MSG2_HIGH("Camped on PLMN: %d-%d ",RRC_CSP_GET_INT_MCC(camped_on_rai.lai.plmn)
                            ,RRC_CSP_GET_INT_MNC(camped_on_rai.lai.plmn)
                            );

  WRRC_MSG2_HIGH("LAC: %x %x",camped_on_rai.lai.lac[0],camped_on_rai.lai.lac[1]);

  if(rrc_est_cn_domain_id != RRC_CS_DOMAIN_CN_ID)
  {
    compare_rai = TRUE;

    /* Get the RACs from NAS Domain Specific Info */
    if(rrc_extract_rac_from_sib_info(&camped_on_rai, &sib1_ptr->cn_DomainSysInfoList) == FALSE)
    {
      WRRC_MSG0_ERROR("Could not extract RAC from NAS Domain Specific Info");
      return FALSE;
    }
    WRRC_MSG1_HIGH("RAC: %d",camped_on_rai.rac[0]);
  }

  if((camped_on_rai.lai.plmn.num_mnc_digits ==
       rrc_est_req_for_rai.lai.plmn.num_mnc_digits ) &&

     rrcrce_compare_arrays(camped_on_rai.lai.plmn.mcc,
                           rrc_est_req_for_rai.lai.plmn.mcc,
                           RRC_MAX_MCC_DIGITS)  &&
     rrcrce_compare_arrays(camped_on_rai.lai.plmn.mnc,
                           rrc_est_req_for_rai.lai.plmn.mnc,
                           rrc_est_req_for_rai.lai.plmn.
                           num_mnc_digits)  &&
     rrcrce_compare_arrays(camped_on_rai.lai.lac,
                           rrc_est_req_for_rai.lai.lac,
                           RRC_MAX_GSMMAP_LAC_LENGTH) 
    )
  {
    if(!compare_rai)
    {
      WRRC_MSG0_HIGH("LAI MATCHED");
      return FALSE;
    }
    else if(rrcrce_compare_arrays(camped_on_rai.rac,
                             rrc_est_req_for_rai.rac,
                             RRC_MAX_GSMMAP_RAC_LENGTH))
    {
      WRRC_MSG0_HIGH("RAI MATCHED");
      return FALSE;
    }
  }
  else
  {
    WRRC_MSG0_HIGH("LAI/RAI mismatch");
    rrcrce_lai_rai_mismatch = TRUE;
  }
  return TRUE;
}


/*===========================================================================
FUNCTION RRC_RCE_GET_EST_CN_DOMAIN_ID

DESCRIPTION
  Returns NAS sent CN Domain id for connection establishment.

DEPENDENCIES
  None

RETURN VALUE
  Returns establishment CN Domain ID.

SIDE EFFECTS
  None
===========================================================================*/
rrc_cn_domain_identity_e_type rrc_rce_get_est_cn_domain_id(void)
{
  return rrc_est_cn_domain_id;
}

/*===========================================================================
FUNCTION    rrcrce_send_conn_rel_cause_event 

DESCRIPTION
  This function will log the event named EVENT_WCDMA_CONN_REL_CAUSE to diag task.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_conn_rel_cause_event
(
  uint8 conn_rel_cause
)
{
  rrclog_wcdma_conn_rel_cause_event_type conn_rel_cause_event;
  switch(conn_rel_cause) 
  {
    case (uint8)RRC_FAIL_TO_CAMP_ON:
      conn_rel_cause_event.conn_rel_cause = 0;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case (uint8)RRC_INTERNAL_ERROR:
      conn_rel_cause_event.conn_rel_cause = 1;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;
  
    case (uint8)RRC_UNSUPPORTED_REDIRECTION:
      conn_rel_cause_event.conn_rel_cause = 2;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case (uint8)RRC_MAX_ACCESS_FAILURE:
      conn_rel_cause_event.conn_rel_cause = 3;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case (uint8)RRC_FAILURE_DUE_TO_CONGESTION:
      conn_rel_cause_event.conn_rel_cause = 4;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case (uint8)RRC_FAILURE_UNSPECIFIED:
      conn_rel_cause_event.conn_rel_cause = 5;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case (uint8)RRC_UNSUPPORTED_INITIAL_UE_ID:
      conn_rel_cause_event.conn_rel_cause = 6;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case (uint8)RRC_ACCESS_CLASS_NOT_ALLOWED:
      conn_rel_cause_event.conn_rel_cause = 7;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case (uint8)RRC_CONNECTION_NOT_AVAILABLE:
      conn_rel_cause_event.conn_rel_cause = 8;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case (uint8)RRC_PCCO_FAILURE:
      conn_rel_cause_event.conn_rel_cause = 9;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case (uint8)RRC_NO_RESOURCE_AVAILABLE:
      conn_rel_cause_event.conn_rel_cause = 10;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case (uint8)RRC_ABORT_LAI_RAI_CHANGED:
      conn_rel_cause_event.conn_rel_cause = 11;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case ((uint8)RRC_ABORT_CAUSE_TYPE_MAX+(uint8)RRC_REL_CAUSE_NORMAL):
      conn_rel_cause_event.conn_rel_cause = 12;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case ((uint8)RRC_ABORT_CAUSE_TYPE_MAX+(uint8)RRC_REL_CAUSE_UNSPEC):
      conn_rel_cause_event.conn_rel_cause = 13;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case ((uint8)RRC_ABORT_CAUSE_TYPE_MAX+(uint8)RRC_REL_CAUSE_PRE_EMPTIVE):
      conn_rel_cause_event.conn_rel_cause = 14;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case ((uint8)RRC_ABORT_CAUSE_TYPE_MAX+(uint8)RRC_REL_CAUSE_CONGESTION):
      conn_rel_cause_event.conn_rel_cause = 15;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case ((uint8)RRC_ABORT_CAUSE_TYPE_MAX+(uint8)RRC_REL_CAUSE_RE_ESTABLISH_REJECT):
      conn_rel_cause_event.conn_rel_cause = 16;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case ((uint8)RRC_ABORT_CAUSE_TYPE_MAX+(uint8)RRC_REL_USER_INACTIVITY):
      conn_rel_cause_event.conn_rel_cause = 17;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case ((uint8)RRC_ABORT_CAUSE_TYPE_MAX+(uint8)RRC_REL_DEEP_FADE):
      conn_rel_cause_event.conn_rel_cause = 18;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case ((uint8)RRC_ABORT_CAUSE_TYPE_MAX+(uint8)RRC_DIRECTED_SIGNALLING_REESTABLISHMENT):
      conn_rel_cause_event.conn_rel_cause = 19;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case ((uint8)RRC_ABORT_CAUSE_TYPE_MAX+(uint8)RRC_REL_CAUSE_OOS):
      conn_rel_cause_event.conn_rel_cause = 20;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case ((uint8)RRC_ABORT_CAUSE_TYPE_MAX+(uint8)RRC_UE_INIT_DORMANCY_UE_IN_OOS):
      conn_rel_cause_event.conn_rel_cause = 21;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case ((uint8)RRC_ABORT_CAUSE_TYPE_MAX+(uint8)RRC_UE_INIT_DORMANCY_ABORT_SUCCESS):
      conn_rel_cause_event.conn_rel_cause = 22;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    case ((uint8)RRC_ABORT_CAUSE_TYPE_MAX+(uint8)RRC_REL_DUAL_STANDBY_PS_ABORT_SUCCESS):
      conn_rel_cause_event.conn_rel_cause = 23;
      event_report_payload(EVENT_WCDMA_CONN_REL_CAUSE, sizeof(conn_rel_cause_event),
                       (void *)&conn_rel_cause_event);
      break;

    default:
      WRRC_MSG1_ERROR("Invalid release cause type %d",conn_rel_cause);
      break;
  }
}

/*===========================================================================
FUNCTION    rrcrce_send_conn_req_cause_event 

DESCRIPTION
  This function will log the event named EVENT_WCDMA_CONN_REQ_CAUSE to diag task.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrcrce_send_conn_req_cause_event
(
  uint8 conn_req_cause
)
{
  rrclog_wcdma_conn_req_cause_event_type conn_req_cause_event;
  if(conn_req_cause <= RRC_EST_TERM_UNKNOWN)
  {
    conn_req_cause_event.conn_req_cause = conn_req_cause;
    event_report_payload(EVENT_WCDMA_CONN_REQ_CAUSE, sizeof(conn_req_cause_event),
                       (void *)&conn_req_cause_event);
  }
}

/*===========================================================================
FUNCTION rrcrce_clear_num_retries_count()

DESCRIPTION
  This function will clear rrcrce_num_retries_count value
DEPENDENCIES
  None

RETURN VALUE
none
SIDE EFFECTS
none
===========================================================================*/
void rrcrce_clear_num_retries_count
(
  void
)
{
  /* Reset the rrcrce_num_retries_count */
  rrcrce_num_retries_count =0;
}

/*===========================================================================

FUNCTION rrcrce_trim_conn_request_msg_ul_tm_tb_size

DESCRIPTION
  This function trimms connection request message.


DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcrce_trim_conn_request_msg_ul_tm_tb_size
(
  rrc_UL_CCCH_Message *msg_ptr,
  uint32 pdu_length
)
{
  /*These If conditions should be in sequence, below is the spec section which indicates that
    *8.5.23 Measured results on RACH
    *1>for messages transmitted on CCCH, take care that the maximum allowed message size
    *    is not exceeded when forming the IE "Measured results on RACH", i.e. limit the number 
    *    of included neighbour cells or if required omit the IE "Measured results on RACH" altogether. 
    *   When limiting the number of included neighbouring cells, 
    *   the number of inter-frequency cells should be limited first i.e. inter-frequency cells should
    *   be omitted before limiting  the number of intra- frequency cells.
    */
  rrc_MonitoredCellRACH_List *curr_monitcells_ptr = NULL;  /* pointer to rrc_MeasuredResultsOnRACH used in freeing a node*/
  
#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
  #error code not present
#endif

  if(msg_ptr->message.u.rrcConnectionRequest.m.measuredResultsOnRACHPresent)
  {
    WRRC_MSG0_HIGH("rrc_RRCConnectionRequest_measuredResultsOnRACH_present bit mask set");
    if(msg_ptr->message.u.rrcConnectionRequest.measuredResultsOnRACH.m.monitoredCellsPresent)
    {
      WRRC_MSG0_HIGH("rrc_monitoredCells_present bit mask set");
      /*Exclude monitored cell results in meas rpt on rach*/
      curr_monitcells_ptr = &msg_ptr->message.u.rrcConnectionRequest.measuredResultsOnRACH.monitoredCells;
      msg_ptr->message.u.rrcConnectionRequest.measuredResultsOnRACH.m.monitoredCellsPresent = 0;        

      curr_monitcells_ptr->n = 0;
      rtxMemFreePtr(&enc_ctxt,curr_monitcells_ptr->elem);      
    }

    /*Disable bit mask*/
    msg_ptr->message.u.rrcConnectionRequest.m.measuredResultsOnRACHPresent = 0;
    WRRC_MSG1_HIGH("Removed measuredResultsOnRACH I.E , presence =%d",
                                                                     msg_ptr->message.u.rrcConnectionRequest.m.measuredResultsOnRACHPresent);
  }
}

#ifdef FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
/*===========================================================================

FUNCTION rrcrce_post_internal_csfb_abort_cmd

DESCRIPTION
  This function posts internal command to trigger RRC connection.
   abort when the CSFB CS call ends.


DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcrce_post_internal_csfb_abort_cmd(void)
{
  rrc_cmd_type *int_cmd_ptr;

  if (rrcrcr_is_proc_active())
  {
    return;
  }

  int_cmd_ptr = rrc_get_int_cmd_buf();
  
  /* Fill in the command id  */
  int_cmd_ptr->cmd_hdr.cmd_id = RRC_INTERNAL_CSFB_ABORT_REQ;
  /* Put the command on the internal queue */
  rrc_put_int_cmd( int_cmd_ptr );
  
  WRRC_MSG0_HIGH("Posted internal cmd RRC_INTERNAL_CSFB_ABORT_REQ");
  csfb_extended_status = CSFB_CS_RB_RELEASED;
}
#endif

/*===========================================================================

FUNCTION rrcrce_abort_resel

DESCRIPTION
  This function lets SIB know whether cell reselection needs to be aborted or not


DEPENDENCIES
  None.

RETURN VALUE
  boolean.

===========================================================================*/
boolean rrcrce_abort_resel
(
  rrc_plmn_identity_type plmn_id, 
  rrc_SysInfoType1* sib1_ptr,
  rrc_freq_type freq,
  rrc_scr_code_type scr_code
)
{
  boolean lai_rai_mismatch = rrcrce_lai_rai_mismatch;
  boolean lai_mismatch = rrcrce_check_for_rai_mismatch(plmn_id,sib1_ptr);

  /*Restore rrcrce_lai_rai_mismatch. If this is set we may end up 
    sending wrong cause in abort indication to NAS*/
  rrcrce_lai_rai_mismatch = lai_rai_mismatch;

  if((((rrcrce_substate == RRCRCE_WAIT_FOR_RRC_CONN_SETUP)&&
       (rrc_v_300 <= 1)&&
       (rrctmr_get_remaining_time(RRCTMR_T_300_TIMER) > 0)&&
       (rrcrce_cmac_status == RACH_TX_SUCCESS))||
       (rrcrce_substate == RRCRCE_WAIT_FOR_SRB_SETUP_CNF))&&
      (rrc_est_cn_domain_id == RRC_CS_DOMAIN_CN_ID)&&
      (rrc_establishment_cause == RRC_EST_TERM_CONV_CALL)&&
      (lai_mismatch)
     )
  {
    rrcrce_cell_resel_freq = freq;
    rrcrce_cell_resel_scr = scr_code;
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION rrcrce_ignore_resel

DESCRIPTION
  Tells whether cell reselection needs to be ignored or not


DEPENDENCIES
  None.

RETURN VALUE
  boolean.

===========================================================================*/
boolean rrcrce_ignore_resel(rrc_freq_type freq,rrc_scr_code_type scr_code)
{
  if((rrcrce_cell_resel_freq == freq)&&
     (rrcrce_cell_resel_scr == scr_code)
    )
  {
    return TRUE;
  }
  return FALSE;
}/*===========================================================================

FUNCTION rrcrce_get_mm_abort_cause

DESCRIPTION
Returns current abort cause

DEPENDENCIES
  None.

RETURN VALUE
  rrc_abort_cause_e_type.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
rrc_abort_cause_e_type rrcrce_get_mm_abort_cause()
{
  return rrcrce_mm_abort_cause; 
}

/*===========================================================================

FUNCTION rrcrce_return_abort_ind_sent_status

DESCRIPTION
Returns abort indication sent

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
boolean rrcrce_return_abort_ind_sent_status()
{
  return rrcrce_abort_ind_sent;
}