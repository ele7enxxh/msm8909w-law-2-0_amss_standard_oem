/*===========================================================================
                        R R C   S I B   P R O C E S S I N G  

DESCRIPTION 

  This module performs the System Information Blocks processing for RRC.
  This module is the main module for SIB processing and it processes
  all the commands related to SIB processing. This module registers
  requests from other RRC modules to read and store SIBs from the BCCH. 
  When all the required SIBs have been read and stored, it informs the other
  RRC modules that the request has been completed.
  

  The other modules for SIB processing are

  rrcsibcollect  -  Collects encoded SIBs, concatenates SIB segments
                    if needed. 

  rrcsibdb       -  Stores the SIB database and provides access
                    functions for the SIB database.
                  
  These modules are controlled by the main SIB processing module.                 
                  

EXTERNALIZED FUNCTIONS

  rrc_sib_event_controller  Receives and processes all events related
                            to SIB processing.

  rrc_sib_proc_init         Power-up initialization of the SIB procedure
                            including the SIB database.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  
  The function rrc_sib_proc_init must be called once to initialize
  this procedure when the phone powers-up.

Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  
All Rights Reserved.QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcsibproc.c_v   1.36   24 Jun 2002 15:53:12   krishnan  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcsibproc.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/18/16   vs      Changes to avoid S-criteria check in case of plmn mismatch in NAS/BCCH plmn for X2W reselection case.
05/11/16   vs      Changes for f3 logging reduction
02/25/16   vi      Made changes to pass the correct reason in transition request to wl1 when reselection is failed due to pending l2 ack
02/24/16   sr      SIB module to not start SIB7 event in FACH when OC is not set for FACH to PCH transition
02/08/16   sas     Update Freq,PSC values in to SIBDB while reading sibs from sysinfo_Container during CSFB redirection.
06/25/15   bc      Changes to read SIBs across DRX for CGI scan.
06/03/15   ad      Made changes to take UE to idle if SIB5 capability changes from non-hsfach
                   to hsfach (vice versa) and there is a cell update being initiated
06/12/15   sp      Fixed KW errors
04/23/15   vi      Made changes to add DMCR IE only when DMCR is supported in SIB3
02/12/15   sa      Made changes for QTF with JO CRM
01/21/15   sa      Made changes for QTF with CRM
01/29/15   sn      Changes to not query wl1 api to get SFN information if STOP 
                   is already sent and when MIB is not yet received
01/29/15   sn      Changes to send NULL mask at the end of SIB event for DMCR cases
01/22/15   bc      Changes to not to ignore GET_SPECIFIC_SIBs in rrcsibproc module during
                   WTOW Automatic BPLMN scan if DRX timer expiry is pending.
01/16/15   bc      Changes made to prevent camping on BC19 ARFCN when a cell is configured in the frequency range of 875-885 MHz
01/14/15   sas     Made changes to log events for cfsb call opt feature
01/12/14   sn      Changes to pass correct frequency and PSC to wl1 api for cell reselection cases to get SFN
01/12/14   sn      Changes to prevent sending NULL mask at the end of SIB event
12/12/14   bc      Made changes to avoid printing events in QXDM when SIB value tag matches with the SIB in SIBDB.
12/01/14   sas     Added Event_Active check before invalidating the MIB,SB ValueTags
11/25/14   bc      Changes to log sibs in QXDM if their value tag matches with the ones
                   in SIB DB.
11/27/14   sr      NV control for FR23776
10/09/14   bc      Added code to receive SIBs received along with MIB.
11/14/14   sr      Code reduction to remove GET_ALL_SIBS handling
11/10/14   sp      Made changes to save procedure substates in crash debug info
11/06/14   vi      Made changes to avoid linker errors after disabling LTE
10/21/14   sn      Fixed KW errors
10/17/14   sn      QTA during SIB read changes
10/17/14   sr      CSFB call optimization changes
10/10/14   sn      FR 22503 PCH to FACH optimization
09/04/14   gv      Added code to support split acq with bplmn
09/04/14   vi      Made changes to reduce curr_scan prints in the log
08/22/14   vi      Made changes to use sib7 from sib container if SIBDB is empty and SIB7 is present in the container
08/11/14   bc      Made changes to not to apply DCH to FACH cell selection optimization for CSG cells.
08/08/14   bc      Made changes to query WL1 to get the current SFN of the BPLMN cell
08/07/14   vi      Made changes to correctly check for enum while build sib-schedule-table for readinf SIB-7 in FACH state
08/04/14   vi      Made changes to avoid compiler warnings
07/21/14   bc      Made changes to directly camp on the cell if it belongs to Bands 1,2,3 without caring
                   about MFBI.
07/21/14   sn      f3 reduction changes
07/03/14   bc      Made changes to check if band in MFBI list is supported by the UE or not
                   for reselection.
06/09/14   sn      Changes to check LA mismatch only for cell reselection cases
06/04/14   sn      Changes to initialize PLMN variable before checking LA mismatch
06/27/14   sn      Changes to prioritize MT call over reselection
06/23/14   vi      Made changes to skip SIB7 reading if SIB7 expiration timer is stil running
06/11/14   bc      Made changes to reduce DCH to FACH cell selection time.
06/06/14   bc      Made changes to double the SIB wait timer to be started during sib read for cell id in DCH.
06/23/14   sr      Made changes to invalidate sib db index for WTOL PSHO success
06/18/14   as      Made changes to delay reselection while waiting for L2 ACK for PCH transition
06/17/14   bc      Made changes to not to decode SIB1 to check if LA is forbidden
                   when it is already present in SIB DB.
26/05/14   bc      Made changes to send correct failure reason to GSM during GTOW reselection 
                   failure due to either PLMN mismatch or S-criteria failure.
05/08/14   bc      Made changes to pass arguments correctly to rrc_sibdb_return_sib_for_db_index function.
04/23/14   bc      Made changes to add the frequency to barred frequency list when sib reading is aborted due 
                   to PLMN mismatch to make sure the overlapping frequency is not skipped.
05/16/14   vi      Made changes to support MOB framework
04/14/14   bc      Made changes to access MFBI list only if the band indicated in FBI is not 
                   supported by the UE.
05/07/14   bc      Made changes to access serving cell sib db index from active cell index.
05/06/14   geg     Use safer versions of memcpy() and memmove()
04/25/14   bc      Made changes to access encoded MIB from SIBDB if decoded MIB is NULL.
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/28/14   gv      Made changes to not camp on BC19 ARFCN when FBI indicates BC6
03/17/14   sr      Made changes to not to invalidate the SIBs if SIB DB index is found during CSFB.
03/11/14   bc      Made changes to ignore bplmn get_specific_sibs when bplmn is suspended
                   during Manual BPLMN search.
03/14/14   sn      Change to handle SFN roll over case while updating SFN offsets
02/07/14   bc      Made changes to send invalid CSG ID in get spec cnf for macro cells.
03/13/14   rmn     Made changes so that WRRC considers band matched if both Band 19 and band 6 are supported.
02/21/14   bc      Made changes to intiliaze previous scanned freq and psc during BPLMN search 
                   at the end of BPLMN search
02/20/14   bc      Made changes to abort sib reading after MIB if the found VPLMN is 
                   already added to the list to be sent to NAS.
01/22/14   bc      Made changes to not to wait for SIBs scheduled in SB1 or SB2 when Value Tag of any 
                   any one of the sibs changes.
03/05/14   bc      Made changes to store the MIB with value Tag different from the expected one
                   and the existing value Tag in SIB DB during paging type 1 update SIBs event.
02/14/14   vi      Made changes to correctly type cast internal commands while saving rrc_cmd_type variables.
01/27/14   sr      Added NV control for MFBI support
01/28/14   sr      Drop Serving BCH after FACH to PCH transition
01/20/14   bc      Made changes to not to wait for SB2 during CGI when SB2 is not a required SIB.
01/03/14   sr      Added code changes for MFBI support
01/02/14   rmn     Reduction of unwanted F3's
12/30/13   bc      Made changes to optimize REL 10 Spec CRs code
11//25/13  bc      Made changes to not crash the UE when SIBs are not valid
11/07/13   bc      Made changes to abort sib reading if we find that the cell is barred 
                   or la is forbidden.
12/16/13   bc      Made changes to ignore UPDATE_SIBS when RCE is getting active.
12/2/13    bc      Changes to To avoid reading SB1 during Update SIBs event if any of the deferred SIBs are 
                   scheduled in SB1 and if SB1 is already present in SIB DB
11/21/13   sn      Start SIB wait timer for three times the rep rate for SIB7 
                   update requested by RCE
09/16/13   sn      G2W TA feature changes
11/18/13   bc      Checked in REL 10 spec crs changes
11/18/13   sn      Changes to avoid duplicate registration with state change manager
10/30/13   sr      Made changes to post UPDATE_SIBS for DMCR after NEW_CELL_IND
10/28/13   sn      Changes to continue with update SIBs event when CSFB call status
                   is true
08/29/13   sn      To handle the case where SIB5/SIB5bis is scheduled first in MIB
09/19/13   sr      Made changes to skip non mandatory SIBs for CSFB
08/15/13   sn      reset wait for WCDMA L1 release lock flag when BPLMN search is being 
                   suspended or aborted
08/20/13   sr      Made changes to update the scheduling info if SB1/SB2 is present
08/20/13   sr      Made changes to not add DMCR IE in uplink message when only SIB18/19 is deferred
08/14/13   rmn     Made changes for requirement to indicate ACQ failure because of band mismatch
07/02/13   sr      Made change to not check for S criteria for CGI reporting
06/12/13   sn      Changes to not invalidate SIBDB when MIB value tag matches
06/19/13   sn      Changes to include CSG ID and RSCP in CGI response
06/06/13   sr      Changes to check for S criteria for XTOW reselection after SIB3 is read
06/06/13   sr      Made changes to abort SIB reading when MIB PLMN doesn't match with the 
                   NAS requested PLMN for XTOW redirection
05/20/13   sn      Changes to consider W CSG feature as disabled if there is 
                   no whitelist update from NAS
05/29/13   sr      Made changes to send SIB SCHEDULE IND to L1 when SIB7 event in FACH is replaced with UPDATE_SIBS
05/28/13   sn      Changes not to invalidate SIBDB when RCE procedure started 
05/23/13   sr      Added support for SIB18/SIB19 deferring
05/14/13   sr      Hybrid CSG changes
05/10/13   sr      Made changes to send SIB SCHEDULE IND to L1 before SB1 is received and 
                   also to set 8 SFNs before SB1 in the sleep table
04/19/13   sr      Made changes to drop SBCCH when UPDATE_SIBS event is completed in FACH state 
                   and FACH to PCH transition is in progress
04/15/13   sn      Consider CSG cells for legacy MPLMN searches
04/05/13   sn      Changes to fix NULL pointer dereference issue
04/03/13   sn      Changes to skip frequencies in 4.4MHz for XtoW CSG searches
03/14/13   sn      Replace update SIBs with SIB7 event
03/14/13   sn      Changes to avoid duplicate registration with state change manager
03/08/13   sr      Added changes to introduce events for CSFB redirection with SI container
02/20/13   sn      Changes to perform suitablity check only after successful SIB3 decode 
02/19/13   sn      Changes to bar wrongly configured CSG cells
02/08/13   sn      Changes to handle SB1/SB2 removal from scheduling
01/23/13   gv      Made changes to reduce logging
01/03/13   ad      Made changes to reduce the redundant F3s
01/03/13   sr      Made changes to remove the ASN functions used for encoding of SIBs and modified the
                   processing of CSFB system information container accordingly as part of memrory optimization
12/20/12   sn      Changes to skip SIB7 for RACH procedure after a L2W redirection 
                   through SIB container
11/12/12   sn      To properly intialize CSG ID field to invalid for macro cells
12/07/12   sn      CSG feature changes
10/23/12   pm      Made changes to assign pointer to NULL after de-allocation
10/08/12   sn      Changes to decode SIB7 only once
03/02/11   kp      Made chanegs to skip sib7 reading for Qchat callin PCH state
09/03/12   gv      Made changes to allow UPDATE_SIBS_IND event when real_sib_mod is TRUE even though
                   UE camped status is NOT CAMPED
08/27/12   gv      Changes to enable DMCR functionality
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/03/12   sa      Changes for HS-RACH feature
06/06/12   sn      Changes to accept SI container even when SIB11 is not present in it
04/06/12   ad      Changes for HS-RACH feature
05/23/12   sr      Change SIB wait timer when SIBs rep rate is greater than SIB wait timer       
05/10/12   gv      Corrrected the choice tag to identify the presence of SIB5/SIB5bis scheduled in MIB
05/02/12   sn      Changes to build and send SIB schedule change indication after 
                   receiving SIB7 in FACH state
04/10/12   gv      Made changes to remove unused heap allocation and related unused code
04/10/12   pm      Added code to support the band priority nv
03/21/12   sn      Changes to include LAC info when providing cell info to GPS in DCH state
03/01/12   sn      Changes to disable DMCR
02/24/12   gv      Made changes to check validity of SIB1 from Sys Info Container
02/16/11   gv      Fixed compilation issue
02/15/12   gv      Added support to log new event when complete SIB is received
01/11/12   sn      Changes to send ETWS security info to upper layers 
01/06/12   mn      Made changes to bar the cell if band indicator in SIB5/6 indicates Band6 and
                   Band6 and Band19 both are not enabled.
01/04/12   sn      Made changes to skip reading SIB19 if its decoding fails twice
12/30/11   gv      Made changes to trash UPDATE_SIBS_IND when UE is not camped on
12/26/11   gv      Fixed compilation issue 
12/22/11   gv      Made changes to narrow down REX_ISR_LOCK under free floating of tasks
12/20/11   db      Made changes to read SIB7 for every 8s in FACH state and
                   Featurized the changes to read SIB7 for every 8s properly
12/20/11   db      Made changes to update the SIB7 timer value of E-DRX
12/16/11   gv      Replaced QSR_MSG_HIGH with MSG_HIGH
12/12/11   gv      Added code to support free floating of tasks
12/07/11   gv      Made changes to decode SB1/SB2 (if present) for SIB 
                   reading due to CGI request from LTE
11/30/11   sn      Created a new update_sibs cause to handle update_sib event 
                   posted by csp when csfb call is cancelled. 
11/08/11   gv      Added NULL pointer check before accessing SIB extension list
11/08/11   gv      Merged changes to send SIB_UPDATE_IND for SIB19 removal.
10/19/11   gv      Fixed compilation errors for BC19
09/28/11   gv      Added code to support optimized band scan in WCDMA
09/30/11   rl      Fixed the ASN1 porting issue raised at compilation
09/08/11   as      Added code to udpate the SIB7 update timer for E-DRX
08/11/11   su      Added code to check for sib1 validity as part 
                   of decoding completed sibs for BPLMN.
08/10/11   su      Added code to not invalidate the SIBDB entry 
                   when we read the SIBs through OTA during CSFB. 
07/18/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.102.00
07/08/11   su      Added code to not handle SIB process UPDATE_SIX_HR_TIMER 
                   when RRC mode is RRC_MODE_INACTIVE.
07/08/11   gv      Fixed KW errors
07/06/11   gv      Fixed KW errors
07/05/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.101.00
06/22/11   su      Added code changes to avoid memory leak while posting 
                   UPDATE_SIB_IND, and triggering OOS when DL_WK_IND is
                   received when UPDATE_SIB event is active.
06/29/11   kp      Added code set sib scan type to normal in all cases of SIB 
                   container invalid for CSFB. 
06/17/11   su      Added missing code changes to provide NV support to enable/disable
                   FEATURE_3GPP_CSFB_SKIP_OPT_WCDMA_SIBS.
06/09/11   rl      Removed the redundant code
06/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.99.50
05/27/11   su      Added code to ignore UPDATE_SIB event in connecting state 
05/03/11   su      Added code changes to provide NV support to enable/disable
	           FEATURE_3GPP_CSFB_SKIP_OPT_WCDMA_SIBS.
05/03/11   su      Added code to drop UPDATE_SIB event while going to 
                   CONNECTING state when CSFB status is TRUE.
05/03/11   rl      Fixed the lint error
04/25/11   rl      Fixed the KW error
04/19/11   su      Added code to free decoded SIBs while reading SIBs from sys info 
                   container in order to avoid exhaustion of decoded contexts.
04/18/11   su      Added code to free decoded SIBs if GET_SPECIFIC_SIB_REQ for serving cell
                   is received without camping after successful SIB reading, in order
                   to avoid exhaustion of decoded contexts.
04/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.52
04/01/11   su      Added code to handle RRCSIB_ABORT_DUE_TO_DL_WK_IND in force_sib_init
04/01/11   su      Added code to trigger OOS for DL_WK_IND in update_sibs
04/01/11   su      Added function rrcsib_is_event_update_sibs_active
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/23/11   kp      Fixed Linking issue for L2W CGI
03/21/11   rl      Added code to support L2W CGI
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/08/11   su      Made changes to mainlin the code under
                   FEATURE_RRC_ASN1_REL5.
03/03/11   su      Added code to correct the condition to check if any SIB event is already
                   in progress before doing any bcch setup in order to allow bcch to be setup
                   when serving cell event is not active and S-BCCH status at LLC is TRUE.
03/02/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.92.50
02/16/11   vg      corrected featurisation issue.
02/15/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.91.50
01/31/11   su      Added code to avoid ERR FATAL due to N-PCCPCH and 
                   S-PCCPCH is up simultaneously when Cell Reselection 
                   is triggered in FACH state.
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/27/11   rm      Added code to support BC19 under FEATURE_WCDMA_BC19
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/18/11   su      Fixed compilation warnings.
01/12/11   ad      Added changes for RRC SW decoupling
01/10/11   kp      Featurized csfb code in deferred sib functions
01/07/11   kp      Fixed klockwork errors
01/04/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.87.50
12/13/10   ss      Added code to invalidate value tag of SIBS for CSFB
12/10/10   su      Added code to support FEATURE_WCDMA_ETWS.
12/07/10   kp      Changed sib_events to sib_events_ptr under 
                   FEATURE_UPDATE_SIB7_FOR_PCH_TO_FACH
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/07/10   rl      Fixed the compiler errors/warnings
11/24/10   ss      Added code to support logging for System Information Container
12/22/10   su      Made code changes to support FEATURE_WCDMA_ETWS.
12/07/10   ss      Added code to support logging for System Information Container
12/07/10   ss      Migrated CSFB code on Osys
11/29/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.85.50
11/18/10   ss      Fixed memory leak in CSFB code
10/26/10   ss      Updated 3GPP CSFB code to modify sib scan type for failure case.
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
11/03/10   kp      Added code to enable FEATURE_WCDMA_DEFERRED_SIB11_12_READING
                   for CSFB feature.
10/26/10   kp      Modified code for 3GPP CSFB
10/26/10   as      Fixed lint and KW issues
10/11/10   rm      Fixed compilation issues
10/11/10   rm      Added code to support 3GPP CSFB
10/11/10   ss      ASN.1 migration to ASN1 9.3.0
10/11/10   ss      Upgraded SIB11Bis code to ASN 8.a and added code to init sibs lookup table 
                   after initialization of rrc_nv_rel_indicator.
09/30/10   kp      Fixed compiler warning
09/16/10   kp      Added code to check SIB1 validity.
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
09/09/10   ss      Added code to search for SIB19 and SIB11Bis, if they are already stored
09/01/10   rm      Fixed lint errors
08/27/10   rm      Made changes to index SIB with correct SIB type
08/24/10   rm      Fixed compiler warnings
08/18/10   ss      Added SIB19 logging code.
08/05/10   rm      Made changes to allow camping on BC11 when band indicator
                   is absent in SIB5/SIB6
07/09/10   rm      Fixed compiler warnings
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/18/10   gkg     Fixed Compilation warnings.
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
04/19/10   as      Added BC11 check for SIB5
14/12/10   dm      Made changes for decoding a SIB before accessing it.
04/12/10   rm      Added code to support BC11
04/12/10   dm      Made changes to not re-read the SIBs which have been already received.
03/12/10   dm      Added code for supporting Deferred SIB reading feature.
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
02/08/10   dm      Made changes to not process state change indication if sib_events_ptr 
                   is NULL.
02/03/10   dm      Made changes to abort SIB reading for neighbour cell if reselection info 
                   is cleared by CSP.
01/21/09   dm      Made changes to not update SIB sleep table till the scheduling info is 
                   received for all the SIBs.
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
11/26/09   dm      Made changes to tear down S-BCCH when SIB event is aborted due to downlink 
                   weak indication in neighbour cell.
10/23/09   dm      Fixed Klocwork warnings
10/15/09   dm      Added code for setting Update sibs event as pending when N-BCCH is up and 
                   processing it later.
10/15/09   dm      Made changes to return TRUE while checking validity of SIB7
                   when N-BCCH is up.
09/11/09   ps      Made changes for reducing the internal RRC command size 
                   under FEATURE_RRC_REDUCE_CMD_SIZE
09/15/09   sks     Made changes to abort reading SIBs if PLMN and band combination is not valid for
                   800 and 850 band or if band itself is not supported.
08/19/09   dm      Fixed lint warning.
08/17/09   dm      Made changes for initiating cell selection if PLMN id and Cell id
                   of the current camped cell and target cell match.
07/30/09   dm      Made changes to read SIB7 content while going from PCH ot FACH state.
07/21/09   dm      Added support for updating UL Interference value with the latest
                   received SIB7 content during UPDATE SIBS event.
07/03/09   dm      Added code to pass MIB PLMN_ID as argument to rrccsp_is_current_band
06/30/09   dm      Added support for updating UL Interference value with the latest
                   received SIB7 content.
06/08/09   dm      Added support for freeing 100k sib heap when WCDMA stack is 
                   inactive as part of ZI reduction activity. code changes are under
                   FEATURE_WCDMA_RRC_ZI_OPT_PHASE_2.
05/14/09   sks     Made changes to suppress compilation errors for RRC UTF.
05/14/09   kp      Added support to not drop S-BCCH during FACH to PCH transition only if 
                   event active is UPDATE_SIB’s in all other cases if S-BCCH is up during 
                   the transition then Drop.
05/10/09   kp      Added support for demand paging RRC code using pragma.
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
04/22/09   dm      Disabled FEATURE SIB SCHEDULE for RRC UTF
03/16/09   kp      Added support to get sb1/sb2 ptr by calling rrc_sibdb_return_sib_for_db_index
                   which will also do ASN.1 decoding if sb1/sb2 are present in sb db & they are 
                   not decoded .
02/27/09   kp      Added support code to free anoter 46k from sib heap under feature 
                   FEATURE_RRC_OPTIMIZED_SIB_HEAP.
02/11/09   kp      Bug fix to free ASN.1 decoded pud's from active_cell_index in 
                   rrcsib_change_serving_cell_status only if its not equal to temp_index &
                   active_cell_index is a valid sib db index.This code is under feature 
                   FEATURE_RRC_OPTIMIZED_SIB_HEAP.
02/04/09   kp      Fixed compiler warning 
02/02/09   kp      Added support for optimized SIB heap under feature flag 
                   FEATURE_RRC_OPTIMIZED_SIB_HEAP
01/27/09   kp      Removed a harmless msg_error in rrcsib_build_sib7_schedule_table
01/19/09   dm      Made changes to support code when feature 
                   FEATURE_SKIP_SIB1_3_DURING_MANUAL_PLMN_SCAN is undefined.
12/17/08   kp      Fixed issue of not setting sb1/sb2 ptr to NULL while freeing 
                   sb1/sb2 from BPLMN db under FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN.
12/08/08   ns      Added support for FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
10/07/08   ns      Added code to store SIB segments received before the scheduling block. 
                   This appears as part of the feature FEATURE_RRC_TSIB_OPTIMIZATION.
09/28/08   kp      Added code to invalidate sib db index if cell I.D & PLMN I.D of target cell
                   & current cell are same. 
09/02/08   kp      Added check to see if PLMN is GSM MAP before accessing it in 
                   rrcsib_post_process_failed_event.  
09/01/08   sks     Added missed conditions in handling Frequency Band indicator
                   in SIB 5
08/01/08   ns      Merged the fixes to handle Error_fatal in rrcsibproc if cell I.D & 
                   PLMN I.D of target cell & current cell are same. 
06/17/08   ns      Added code to receive SIB7 segment before MIB during the SIB7 update event 
                   during connection establishment.
05/29/08   ns      Added code to take care of the SIB segments being dropped when 
                   the offset is greater than 255 
05/21/08   kp      Added check to see if the type of PLMN is GSM MAP before processing PLMN I.D
                   during cell re selection.
05/02/08   kp      Added check to see if the type of PLMN is GSM MAP before processing PLMN I.D.
04/24/08   ns      Fixed klockwork errors
03/26/08   bu      Corrected the featurization for FEATURE_WCDMA_GPS_CELL_ID_READING.
03/14/08   kp      Reverted back changes made to func rrc_sib_is_current_event_sib_7_in_fach 
                   in CL:612287. 
03/10/08   kp      Added support for SIB11bis. Code changes are under feature flag 
                   FEATURE.SIB11BIS.SUPPORT 
03/07/08   vr\da   Support for SIB reading in DCH for MBMS
02/21/08   ns      Fixed klockwork errors
02/20/08   da      Changed func rrc_sib_is_current_event_sib_7_in_fach to return
                   TRUE if sib7 event is active or if no events are active.
01/28/08   vr      Changed feature definition of Inter SIB Sleep from FEATURE_INTER_SIB_SLEEP
                   to (defined(FEATURE_INTER_SIB_SLEEP) || defined(FEATURE_WCDMA_DRX_MANAGER))
                   since the new DL code in L1 uses FEATURE_WCDMA_DRX_MANAGER instead of
                   FEATURE_INTER_SIB_SLEEP
12/21/07   da      Support for FEATURE_MBMS
11/13/07   vr      Fixed compilation error
11/12/07   vr      Re-enabled the check to fail camping on a AWS cell if Freq band indicator
                   is absent in SIB5/SIB5Bis/SIB6
10/23/07   kp      Added support for network sharing feature.Changes are under feature flag 
                   FEATURE NETWORK SHARING.
09/10/07   kp      Bug fix to not access MIB ptr in temp MIB if the ptr is NULL
                   in function process_completed_sibs().This will prevent accessing NULL
                   temp holder MIB ptr in case of BPLMN scan where SIB has received 
                   MIB & SIB3 which are saved in BPLMN SIB DB & now a SB1 or SB2 
                   is received.
08/29/07   kp      Added extra guard to ignore dummy update SIB's event 
                   when UE is not camped on state
08/08/07   kp      Added support for feature cell id reading in DCH state, 
                   under feature flag FEATURE_WCDMA_GPS_CELL_ID_READING.
07/09/07   kp      Added support for Tsib optimization db.
06/25/07   da      Fixed compilation Error.
06/25/07   vr      Fail camping on band 9(1700) if freq band indicator is not present.
                   Freq band indicator is mandatory for band 9 since this is the only 
                   differentiator betweem band 9 and band 3(1800). 
04/10/07   vr      Added NULL ptr check. Check if decoded message is not NULL before 
                   processing a BCCH-BCH or BCCH-FACH message. 
04/09/07   vr      Removed GCF specific implementation of skipping SIB3 reading 
                   for WtoG Handover failure case.
03/29/07   kp      Added support for making code generic in staring 
                   sib_change_safe_guard timer for any type of UPDATE_SIBs complete. 
                   We used to start this timer only for six hrs sib event till now.
03/29/07   kp      Added support for skipping sib read if FPLMN found
                   under the feature flag FEATURE SKIP SIBS FOR FPLMN
03/28/07   kp      Added support for implementation of BCCH Mod Timer. Clear BCCH Mod DB 
                   when UE Serving cell is changed & UE transition to DCH state. 
                   Code clean up for BCCH Mod timer related variables in UPDATE_SIBs command,
                   since its implemented in PG1 module.
03/22/07   kp      Fixed bugs found while code walk through for handling SIB7 update 
                   request properly from RCE in one corner case where neighbor cell event 
                   is on going & rce has posted SIB7 event request. 
                   Send RRC_SIB7_UPDATE_STATUS_IND with success to RCE in this case.
03/14/07   vr      Reverted back changes for failing camping on AWS band if freq band
                   indicator is absent. For now, SIB reading will succeed for AWS band
                   cells even if freq band indicator is absent in SIB 5/6. Need further 
                   clarification from Standards team.
03/09/07   kp      Added check to restrict FEATURE_RRC_NO_MIB_FOUND_BAR_CELL for 
                   cell reselection only for now.
02/26/07   kp      Added a check to see CCM state is camped before starting SIB-7 event.
02/20/07   kp      Added the support to bar the cell if MIB not found at position 
                   SFN mod 32 = 0, but a transport block with correct CRC 
                   was found at that position.The code is under feature
                   flag FEATURE_RRC_NO_MIB_FOUND_BAR_CELL.This support is only
                   for RRCSIB_GET_SPECIFIC_SIBS & requested procedure is CSP.
02/05/07   vr      Added support to fail camping on AWS cell if band indicator is absent in
                   SIB5/6.
01/19/07   kp      Added support for SIB7 update in Cell FACH state.
                   The code is under feature flag FEATURE_UPDATE_SIB7_IN_FACH.
                   with this feature ON SIB-7 which doesn't have value tag will be read     
                   all the time in cell FACH state. The S-BCCH will be kept open for the 
                   duration UE stays in cell FACH state.
10/13/06   sgk     Fixed 7200 compiler warnings.
10/03/06   kp      Corrected the values passed to function update_event_for_received_value_tags 
                   for sb1 & sb2
09/25/06   kp      Added support for six hours sib re-read & sib_change_ind interface change
09/19/06   vr      Added an optimization for FEATURE_INTER_SIB_SLEEP.
                   After last segment of Scheduling block 1 and Scheduling block2,
                   set few more 1s in the schedule table to keep L1 awake. L1 cannot act to
                   a new schedule if it is sleeping.And decoding SB will always result in a 
                   new schedule where some of the 0s will be flipped to 1s. By inserting
                   some dummy 1s after last segment of SB, L1 will stay awake for that long
                   and hence can process the new schedule due to SB immediately
09/15/06   vm      Added support for the feature FEATURE_UI_RF_ENABLED_BANDS_SUPPORT.
06/23/06   vr      Fixed bug in handling Scheduling blocks.If SB is already present in SIB DB
                   and the value tag matched from MIB,the set 'sb_decoded' flag to FALSE in
                   SIB events tempholder. If flag is not cleared, RRC would unnecessarily 
                   wait for SB1/SB2 before completing the event.This would have bigger impact
                   with Inter SIB Sleep feature turned ON, since RRC would have removed the schedule
                   for SB but would wait for it.The event would eventually fail
06/08/06   vr      Added support for new feature FEATURE_MCC_AND_850_800_BAND_COMBINATION.
                   Now during cell selection on 850/800 band, immediately after reading
                   the MIB, SIB procedure calls the function 
                   rrccsp_is_sib_mcc_and_850_800_current_band_comb_valid() to determine 
                   if the UARFCN and PLMN combination is valid. Only if the combination 
                   is valid, SIB procedure proceeds with reading SIBs otherwise SIB reading
                   is aborted. Similarly, during cell reselection on 850/800 band, immediately
                   after reading the MIB, SIB procedure calls the function 
                   rrccsp_is_mcc_and_850_800_supported_band_comb_valid() to determine if 
                   the UARFCN and PLMN combination is valid. Only if the combination is valid, 
                   SIB procedure proceeds with reading SIBs otherwise SIB reading is aborted.
06/07/06   kp      Added support for six hours sib update
06/06/06   vr      Fixed intendation lint errors
05/10/06   vr      Changes to FEATURE_INTER_SIB_SLEEP. Do not initialize 
                   schedule tables in the init function since they
                   anyway would be initialized before the start of the 
                   neighbor event.
04/26/06   vr      Added support for SIB5bis
04/10/06   kp      Added L1 drift support in RRC PCH state.
03/30/06   vr      While processing pending Update SIBs Ind,populate the 
                   real_sib_mod field appropriately
03/22/06   kp      Added support to check for processing system information change 
                   Indication only is the mib value tag stored in sib data base for 
                   Serving cell is not current 
03/14/06   kp      Lint clean-up
03/07/06   vr      Extended band mismatch check for bands BC8 and BC9 in FDD2
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1
02/21/06   vm      Added support for 1700_2100 AWS Band (IV) under feature name
                   FEATURE_WCDMA_BC4.
02/07/06   vr      Changed feature name from FEATURE_SIB_SLEEP_OPTIMIZATION 
                   to FEATURE_INTER_SIB_SLEEP
02/06/06   vr      Before processing RRC_UPDATE_SIBS_IND command,check with
                   LLC if OC is set for DCH.If so,drop the command since sending
                   a PCCPCH setup request during a reconfig would result in 
                   bad L1 state.
01/23/06   vm      BPLMN Optimization for Automatically re-starting Manual 
                   BPLMN search after completion of LAU/RAU updates & added
                   support for not rejecting Manual PLMN search request when RRC state 
                   is Connecting/FACH/DCH and the establishment cause is Registration. 
01/17/06   vr      While processing buffered Update_Sibs_Ind,populate fields
                   from buffered command in store rather than the current 
                   command pointer
01/10/06   kc      Added additional debug messages for PLMN/Cell-ID
12/22/05   vr      More support for FEATURE_SKIP_SIB1_3_DURING_MANUAL_PLMN_SCAN
                   Skip reading SIB1 and SIB3 for GTOW Manual PLMN selections also
12/21/05   vr      Support for FEATURE_SIB_SLEEP_OPTIMIZATION 
12/06/05   vr      Support for FEATURE_SKIP_SIB1_3_DURING_MANUAL_PLMN_SCAN
11/21/05   vr      Fixed a bug in WTOW BPLMN support.Buffering the Update SIBs
                   command in a store before suspending BPLMN rather than storing 
                   just the IEs from the command and posting another internal 
                   one after Suspend_Cnf.
11/09/05   kc      Changes to setup PCCPCH based on L1 request
11/08/05   vr      Support for WTOW BPLMN feature.
                   1.For WTOW BPLMN SIB event,do not touch the sib db because
                   it may tamper the serving cell entry and also may cause
                   problems during Reselections.Work on a separate structure
                   for WTOW BPLMN.
                   2.Before setting up the channel for BCCH Modification
                   through pg1,query with suspend BPLMN search(through CSP)
                   if necessary
09/16/05   vr      Extend the band mismatch detection(using SIB5/6) to all bands
09/13/05   vr      If S-BCCH channel setup fails,do not send another Channel
                   Config Request to tear down S-BCCH since it is already done
                   by LLC
08/25/05   vr      Moved the 800/850 band mismatch detection from SIB5/6
                   support from FEATURE REL5 to FEATURE_RRC_ASN1_REL5 
08/19/05   vr      If the band specified in SIB5 is not supported by the UE,
                   fail the RRC_GET_SPECIFIC_SIBS_REQ request.
07/25/05   kc      Perform comparison of SIB Value-Tag's for SB's immediately after SB's are
                   read during SIB_UPDATE_REQ
07/05/05   kc/vr   SIB optimization not to wait for SIB3 during resume failure
                   after W->G handover failure.This is a GCF fix.Changes are under
                   GCF flag.
06/27/05   vr      If SIB event fails for Update SIBS Ind due to SBCCH failure or
                   SIB timer expiry,initiate Cell Update with cause Re-entering service
                   area in FACH/PCH states or initiate Cell Selection with cause 
                   Out of service in Idle.The idea is to trigger cell selection
                   and (re)read the SIBs.
06/24/05   kc      Added checks for SIB/5/SIB6 to have AICH/PICH info elements when in
                   connected mode.  Earlier checks were only for SIB6 and did not check for
                   AICH/PICH in SIB5 when this info was absent in SIB6
06/15/05   kc      Added checks to see if SIB/5/SIB6 have atleast one AICH/PICH info element.
05/31/05   kc      Added support for limiting max time we take to read sibs during reselection to a
                   factor of thrice the maximum repitition. Rate.  Also added an API to query if
                   SIB7 update is on going.
05/28/05   kc      Added API to return status whether SIB-Update is ongoing or not.
05/27/05   vm      Cell bar request is now sent with an integer Tbarred timer 
                   value instead of an enum type
05/05/05   kc/vm   Added support for T-barred Feature. 
04/07/05   kc      Added optimization to not wait for SB1, SB2 if they are already present in sib-db
                   during SIB_7_UPDATE
03/24/05   kc      Added code to break out of an infinite while loop if a wrong sib configuration
                   where number of segments is < segment offset's is transmitted in scheduling
                   information.
03/09/05   kc      Added support to read connected mode timers in v3a0 extensions.
02/10/05   sgk     Added SIB18 support.
02/09/05   kc      Added fix to update V-Tag is a SIB-Mod Ind is received when an existing one
                   is ongoing.  Also fixed adversarial test cases for sib-scheduling
12/27/04   vr      Fixed more lint errors
12/21/04   vr      Fixed lint errors
12/01/04   kc      Added support to read mib according to hard-coded schedule for cases when
                   MIB comes in multiple segments.
12/01/04   kc      Added utility fn. to check and see if MIB is read for a particular event.
12/01/04   kc      Added support to not clear serving cell status for force_sib_event_init 
                   for a neighbor cell.  
11/24/04   bu      Added the event_index in the rrcsib_update_sched_info_from_mib_sb()
                   call.
11/22/04   kc      Support for BPLMN
11/18/04   kc      Call fn. to clear serving cell status when we go from WtoG or due to OOS
11/12/04   kc      Fixed lint errors
11/09/04   sm      Optimization to read all sib's in GET_SPECIFIC_REQ and treat GET_ALL_SIBS
                   as dummy request.      
10/05/04   kc      Remove-duplication of decoding code for all SIB's in a switch stmt and
                   added it in a fn. call.  Fixed a type for SB1 vs SB2.  Update SB's when
                   SIB value tag's match the one's in SIB-DB
09/12/03   kc      Optimized Timer-based-SIB reading to not re-read based on V-Tag comp.
                   as value tags are not present.  Added changes to check if SIB 1,3,5,7
                   are scheduled mandatorily.  Also added changes to update scheduling
                   info from SB1 before DB-Index is created.  Added changes to update
                   Value-Tag's from Scheduling Blocks before other SIB's during one
                   SIB-reading process.
09/13/04   sgk     Typecast bcch_cmd_ptr->cmd.chan_config_req.transaction_id
                   to uint8 in function rrcsib_configure_bcch, typecast
                   scheduling_info->scheduling.sib_Pos.u.rep that is rep4 to
                   rep4096 to uint8 in function rrcsib_update_pos_offset_in_sched_info,
                   typecast scheduling_info->scheduling.segCount to uint8 in
                   function rrcsib_update_pos_offset_in_sched_info to fix lint 
                   error: Loss of precision (assignment) (32 bits to 8 bits).
                   Assigned  count = SIB_SRV_INDEX  in function
                   rrcsib_clear_procedure, sib_bcch_setup_req,
                   rrcsib_process_state_change_ind,
                   rrcsib_process_sib_wait_timer_expiration,
                   rrcsib_process_bcch_setup_failure,
                   rrc_sib_event_controller to fix lint error:Type mismatch
                   (assignment) (int/enum).
                   Typecast count to rrc_SIB_Type in function decode_completed_sibs 
                   and typecast count to sib_events_index_e_type in function
                   rrcsib_force_sib_event_init ot fix lint error Type mismatch
                   (arg. no. 1) (int/enum).
                   Checked the return value of the function
                   'rrcsib_configure_bcch(enum {...}, enum {...})' for FAILURE
                   and printed a msg high in function
                   rrcsib_post_process_sib_failed_event, post_process_completed_event,
                   sib_bcch_setup_req to fix lint error: Ignoring return value of
                   function.
                   Typecast event_index to uint32 in function
                   rrcsib_post_process_failed_event, typecast rrc_schedulingBlock1,
                   rrc_schedulingBlock2 to uint32 in function
                   rrcsib_update_sched_info_from_mib_sb, typecast
                   rrc_masterInformationBlock, rrc_schedulingBlock1,
                   rrc_schedulingBlock2 to uint32 in function
                   rrcsib_update_mib_sb_sched_info_from_sib_db, typecast
                   rrc_masterInformationBlock, rrc_systemInformationBlockType3,
                   rrc_schedulingBlock1, rrc_schedulingBlock2 in function
                   create_new_sib_event, typecast msg_ptr->payload.u.lastSegment.sib_Type
                   and local_complete_SIB_List->value.sib_Type to uint32 in function
                   rrcsib_check_sib_schedule_for_sibs, typecast rrc_schedulingBlock1,
                   rrc_schedulingBlock2 in function decode_completed_sibs and
                   rrc_masterInformationBlock in function process_completed_sibs,
                   typecast index to uint32 in function process_completed_sibs,
                   post_process_completed_event , process_bcch_message, typecast
                   MAX_SIB_EVENTS_INDEX to uint32 in function 
                   rrcsib_check_for_duplicate_update_sibs_cmd,
                   rrc_sib_proc_events_init, rrcsib_force_sib_event_init, typecast
                   rrc_systemInformationBlockType7 to uint32 in function
                   rrcsib_force_sib_event_init to fix lint error Converting enum to int.
                   Removed break after return in switch in function
                   rrcsib_check_sib_schedule_for_sibs to fix lint error 'unreachable'.
                   Initialized uint32 seg_index[MAX_SEGMENTS_FOR_A_SIB],
                   uint32 sib_type[MAX_SEGMENTS_FOR_A_SIB] to 0 in function
                   rrcsib_check_sib_schedule_for_sibs to fix lint error sib_type
                   and sib_index may not have been initialized. 
08/10/04   kc      Merged code for SIB-Scheduling
08/10/04   kc      Added functions for code that is re-used to remove duplication of
                   SIB_7 time conversion and Channel-Config-Req
08/06/04   kc      Reset SIB7 updating-flag to false when SIB7 is aborted by RCE or when
                   we get abort due to OOS always.
08/05/04   kc      Changes to not read SIB7 forcibly in disconnected mode.
07/22/04   kc      Changes to handle STATE_CHANGE_IND always when we go from FACH->DCH.
07/15/04   kc      Changes to support SIB-Modification-Indication.
07/13/04   kc      Fix to read SIB7 always for GET_ALL_SIBS and GET_SPECIFIC_SIBS for
                   fach-reselections.
06/23/04   kc      Fix for dead-lock situation when a SIB is removed, but SB is not
                   read during GET_SPECIFIC_SIBS.  Also changes to mandatorily read
                   SB's if they are scheduled and all SIB's whose value tags have
                   changed.
06/22/04   kc      Support to tear down S_BCCH logical channels when GET_SPEC_SIB_REQ
                   for serving_cell fails due to S_PCCPCH setup fails.
06/16/04   ttl     Init tmc_heap_sib for SIB.
06/01/04   vn      In process_completed_sibs(), fixed a problem of double-freeing
                   MIB and SIB3 temporary storage. Added code to keep track of 
                   num_stored_cells in SIB DB at any time - also added debug messages
                   to print stored num of cells.
05/28/04   svk     Added a new fn rrcsib_force_sib_event_init() to provide a 
                   graceful way to tear down BCH and/or initialize event and release
                   corresponding cells in SIBDB when an ongoing SIB7 reading is 
                   aborted by RCE or an OOS area event happens.                   
05/12/05   svk     fixes cr 42990
                   update_event_for_received_value_tags() : Added value_tag checks in the
                   case when the SIB is received and the sib is not decoded yet and the
                   MIB value tag has changed.Delete the TSIB entries for the SIB if its
                   value tag has changed.
04/27/04   svk     Rolled back the changes made for the fix for CR 41184
04/20/04   svk     Added transaction id to channel_config_req. Resolved the conflict when
                   a serving bcch is setup by RCE and another serving bcch req comes from
                   CSP. Fixes CR 41184, 39005
04/13/04   svk     In post_process_failed_event, check if the sib_db index was found for the
                    event before deleting the variables associated with the sib db.
03/30/04   svk     In post process, changed the event_name comaprison from RRC_SIB_GET_SPECIFIC_SIBS_REQ
                   to RRCSIB_GET_SPECIFIC_SIBS
03/25/04   svk     In post_process_failed_event clean up the appropriate index if the
                   SIBS have been requested by CCM or CSP. Will fix the duplicate cell_id issue
03/24/04   svk     If SIB2 is not scheduled dont wait for it.
03/23/04   svk     Brought in the cheetach fixes onto saber.
03/17/04   svk     In check_event_completed_status(): If SIB2 has not been scheduled in MIB
                   or in SB1 or SB2 andif SIB2 is requested then don't wait for SIB2.
03/11/04   svk     process_completed_sibs(): Added the check to see if MIB is decoded before
                   calling the rrc_translate_gsm_map().
02/11/04   sk      Added sib-2 in needed list
01/19/04   ram     Fixed a bug in process_completed_sibs() which had prevented the
                   procedure from storing a new MIB or SIB3 in case the first decode
                   of MIB or SIB3 for a particular SIB event failed.
01/16/04   bu      Send a SIB7_UPDATE_STATUS_IND with success to RCE if BCCH setup
                   fails or SIB wait timer expires and RCE was waiting on SIB7 becoming
                   current.
12/16/03   svk     Fixes CR 38390. Call function rrc_sibdb_find_cell_index_for_plmn_freq_psc
                   to find the database index based on plmn, freq, psc instead
                   of just freq and psc.
12/15/03   svk     rrcsib_check_and_update_sib7_validity() : Give higher priority to
                   SIB7 update requested by RCE over a inter-frequency cell-reselction is
                   in progress.
12/11/03   svk     sib_bcch_setup_done() : 
                   If the cmd is GET_ALL_SIBS and if the srv_bcch is already up then return
                   immediately else send a command to LLC to setup the serving bcch
12/11/03   svk     sib_bcch_setup_done() : Send the channel config to LLC without
                   checking if the RLC_ID is existing or not.
12/03/03   svk     Added code to try to match the SIBDB index using frequency
                   and scrambling code, mainly to reduce the time taken for
                   Idle mode reselections. This is featurized under
                   FEATURE_RRCSIB_USE_FREQ_PSC_FOR_INDEX.
11/17/03   bu      Set the procedure correctly in the sib_events database when
                   changing the active event from UPDATE_SIBS to GET_SPECIFIC_SIBS_REQ
                   to read SIB7.
11/07/03   bu      Added support for SIB7 update in IDLE.
06/03/03   ram     Added handling of potential race conditions when SIB7 reading
                   is enabled in Idle Mode and CELL_FACH. Added support for SIB7
                   reading in Connecting state based on expiration timer. This
                   is turned off for now.
05/15/03   ram     After freeing SIB pointers, equated them to NULL to avoid
                   freeing same pointer twice.
05/14/03   ram     Due to LPM, FTM etc, this procedure may have to be initialized
                   more than once after power-up so split the init function
                   into a power-up init part and a run-time init part. Made
                   changes to delete old SIBs right before new SIBs are stored
                   since they are not deleted when we receive the new value-tags.
                   Changes to store value-tags only after a event has been
                   completed succesfully.
05/05/03   ram     Featurized SIB7 updating under FEATURE_RRC_SIB7_UPDATE.
02/25/03   ram     Added additional checking for optional SIBs (SIB4, SIB6, 
                   SIB12). Even if they are indicated as present in the cell
                   if they are not scheduled by the MIB or scheduling blocks
                   we'll not wait for them. Also added code to resolve
                   contention between a GET_SPECIFIC_SIBS event for SIB7
                   updating and a UPDATE_SIBS event.
02/18/03   ram     When a active event is superseded by a higher priority
                   event, the "event_active" flag for the active (old)
                   event was not being cleared. Added code to clear the flag
                   in this case.
01/31/03   ram     Updated copyright. Linted file and fixed lint errors.
01/02/03   ram     Enabled SIB7 reading while going to connecting state and
                   also while in CELL_FACH state. Updated to ignore duplicate
                   state change notifications since we've registered for
                   notifications using wildcards.
02/05/03   sk      March2002Rel specific changes:  Removed  
                   rrc_SIB_Type_spare3 Added  rrc_systemInformationBlockType15_5
10/14/02   ram     Copied freq and scrambling code into SIB database for
                   GET_SPECIFIC_SIBS and GET_ALL_SIBS events. These can be
                   used later while updating SIBs.
09/30/02   ram     Fixed a problem related to value tag comparisons: SIBs
                   with incorrect or unknown values were being deleted
                   when a new scheduling block was read (which is correct 
                   behaviour), but they were not being re-read under some
                   conditions.
09/24/02   ram     Removed incorrect parameter in rrcsib_process_int_sib
                   _event_complete.
09/23/02   ram     Added support for cell reselection in CELL_FACH.
09/03/02   ram     Fixed a problem in post_process_completed_event where an 
                   UPDATE_SIBS event completion was not being saved. If
                   RRC state is changing to CELL_DCH, cancel any event in
                   progress since BCCH-BCH pipe would not be up in CELL_DCH.
07/26/02   ram     Added code to ignore duplicate RRC_UPDATE_SIBS_IND commands
                   when an RRC_UPDATE_SIBS_IND command is already being processed.
06/24/02   ram     Disabled timer based SIB7 updating in FACH since BCCH setup
                   in FACH has not been tested yet.
06/10/02   ram     Fix for SIB7 update request by external procedure.
                   Temporarily disabled SIB7 updating.
06/07/02   ram     Support for tearing down BCCH whenever the BCCH
                   is setup for updating SIBs through Paging Type 1
                   message or through expiration timers. Some fixes
                   for SIB7 update to make it faster.
05/30/02   ram     Added requesting procedure name in sib event cnf
                   commands. When an UPDATE_SIBS event is completed
                   added support to tear down S-BCCH since it's not
                   torn down by LLC. Added support for reading SIB7
                   when requested by an external procedure and when
                   the SIB7 expiration timer expires.
05/13/02   ram     Included frequency and scrambling code in failed 
                   SIB command confirmations also.
05/07/02   ram     Updated module to not tear down Neighbor BCCH when
                   we're done reading SIBs from a neighbor cell. This
                   is because L1 uses the timing from the PCCPCH of the
                   Neighbor cell. Also when one N-BCCH has to be torn
                   down to setup another N-BCCH the Channel Config 
                   command should only establish the new N-BCCH. The
                   old N-BCCH is torn down automatically. Made this
                   change in the function bcch_reconfigure_for_new_sib_event.
02/19/02   ram     The SIB wait timer for UPDATE_SIBS_IND event has been
                   increased to about 53 seconds. This includes the 
                   regular 12 seconds plus one SFN cycle since we may
                   have to wait 1 SFN cycle for the MIB to be modified.
                   Added interface to update connected mode timers
                   and constants if required by the UTRAN Mobility Information
                   procedure when we move to a new cell.
02/06/02   ram     Added a function bcch_reconfigure_for_new_sib_event
                   which takes care of reconfiguring the BCCH pipe when
                   a new event superceeds an existing event. Also added
                   processing to remember when an RRCSIB_UPDATE_SIBS event
                   is interrupted so that it can be resumed if needed
                   after the other event is completed. In function
                   rrcsib_change_serving_cell_status added processing to
                   reactivate pending RRCSIB_UPDATE_SIBS command if we're
                   staying in the same serving cell. 
02/05/02   ram     Added function rrcsib_change_serving_cell_status
                   which is used only by cell selection procedure. This is used
                   to indicate to the sib procedure that a new cell has been
                   designated as the serving cell in case of a succesful cell
                   selection or reselection. It also indicates if the current
                   serving cell is acceptable in case of a failed cell
                   reselection.
01/16/01   ram     Added support for Scheduling Blocks 1 & 2 which are
                   introduced in the 20001-06 version of the specs. Added
                   support for new sibs SIB 15.4 & SIB 18 although they are
                   not used currently. Added support for Neighbor BCCH
                   config and teardown to support cell-reselection.
                   Added some MSG_HIGHs for better debug capability.
12/11/01   bu      Added support to handle BCCH setup failure.
10/17/01   ram     Implemented a timer to have a maximum wait time for
                   getting SIBs in a cell. If the timer expires before
                   all SIBs are received, a failure status is sent to
                   the calling procedure and the SIB event is cancelled.
09/25/01   ram     Used the rrclcm_check_dl_rlc_lc_id function to check if
                   the BCCH is already up when a new SIB event is received.
                   If it's not setup, then this module sets up the BCCH.
                   Cleaned up the function sib_bcch_setup_req to check for
                   all error conditions.
09/24/01   ram     Added MSG_HIGHs to indicate when each SIB is ASN1 decoded
                   correctly. This is featurized under FEATURE_RRC_SIB_DEBUG.
07/31/01   ram     Enhancements for SIB processing - if ASN1 decoding fails
                   we attempt to read all the segments again for that sib.
                   Also when we get all the SIBs for a cell, cleared the 
                   BCCH status so that when we move to a new cell we can
                   setup BCCH again.
06/26/01   ram     The frequency and scrambling code were not set correctly
                   when a confirmation command was sent. Added code to set
                   the correct frequency and scrambling code. Added code to
                   check if BCCH is already setup when 2 GET_SPECIFIC_SIBS
                   commands are received back-to-back.
06/14/01   ram     Added registration with the state change manager so that the
                   SIB procedure will be notified when the state changes to
                   disconnected state. When the UE goes to disconnected state
                   the SIB event variables are cleared since the cell selection
                   procedure will send fresh sib commands to this module.
06/11/01   ram     Added support for the command RRC_UPDATE_SIBS_IND. This command
                   is sent when we receive the IE "BCCH Modification Info" indicating
                   that the MIB has been modified and needs to be read again. When
                   we get this command, we read the updated MIB, check all the
                   value tags and read all the SIBs that have new value tags.
                   Cleaned up BCCH setup code so that BCCH setup information
                   is maintained seperately from the SIB event and only one 
                   variable is maintained for the serving cell and one for the
                   neighbor cell. Added more comments.
05/18/01   ram     The internal format for PLMN ID has been changed to use
                   arrays instead of integer to store MCC and MNC. Updated
                   this module to use the new format.
05/03/01   ram     Removed the temporary fixes added to bypass the problem with
                   not getting repetitive sibs on the BCCH. The test setup can
                   now send repetitive sibs so these fixes are not needed. 
                   Checking for optional sibs 4,6 and 12 has been changed so
                   that the sib database is directly checked to determine if
                   these sibs are present. This is more reliable than checking
                   them locally.
04/30/01   ram     Cleaned up the value-tag comparison code. Bypassed comparing
                   value tags when we're camping on a cell that's not stored
                   in the database.
04/23/01   ram     Fixed a problem in sending the get_sibs_confirm command where
                   the status was not being copied correctly.
04/17/01   ram     Made a temporary fix to not read sibs when a GET_ALL_SIBS
                   command is received. This is to workaround a problem when
                   SIBs are sent only once by the test setup. These changes
                   won't be permenant.
04/13/01   ram     Fixed a couple of bugs where assignment statements were
                   used within a conditional. Removed SIB2 from the list of
                   necessary SIBs to camp on a cell. SIB2 is needed only
                   in URA_PCH state and that's currently not supported.
04/09/01   ram     When a GET_ALL_SIBS command is received, we no longer
                   send a channel config command to LLC. This is not expected
                   by LLC and is not processed. Also added code to indicate
                   to the SIB database when it's ok to let other RRC modules
                   read SIBs for the serving cell.
04/07/01   ram     Added support for checking optional SIBs such as SIB4
                   and SIB6. Added support for checking value-tags when a
                   SIB databse index has been matched with a previously
                   stored cell's PLNM and Cell ids. Removed the check for
                   whether a PDU has been freed since the sib database
                   module frees all PDUs when an index is being used for
                   a different cell.
03/23/01   ram     Updated the code to work with Dec. 2000 version of the RRC
                   Spec, with the exception of the Scheduling Blocks which were
                   not defined in the Dec. version of the ASN.1 headers. Also
                   updated to work with LED ASN.1 header file.
03/08/01   ram     Added code to discard any downlink SDU's unless the BCCH setup
                   confirm is received. This ensures we don't accept SDU's from
                   any BCCH's except the one that we requested. Added code to
                   ignore optional SIBs if all the mandatory SIBs have been
                   received and optional SIBs are not yet received. Added more
                   MSG macros to help in debugging.
02/20/01   ram     Added code to print a message when the SIB database index
                   has been found.
02/08/01   ram     Added more comments. In function decode_completed_sibs, added
                   a check to see if the ASN1 decode failed. Used the function
                   provided by rrclcm to find out the rb id for the received 
                   downlink SDU.
01/26/01   ram     Updated to the new LLC interface for sending the BCCH config
                   request. Expanded function decode_completed_sibs to decode all
                   the sibs that we support currently. Updated comments.
01/25/01   ram     Added support for processing the command RRC_GET_SPECIFIC_SIBS_REQ.
                   This command gives a specific list of SIBs to be collected from
                   either the serving or a neighbor cell. Re-organized the create_new_
                   sib_event function to setup BCCH only if it's not already setup
                   for a previous event. Added function decode_completed_sibs which
                   decodes a collected SIB directly into the specified location
                   in the SIB database. Added function convert_extern_gsm_map_plmn
                   to convert a gsm map plmn into the format used internally by the 
                   UE. Updated process_completed_sibs to have the full functionality
                   intended by the design. Added functions check_event_completion_status
                   and post_process_completed_events to check if a requested
                   event is complete and if so send confirmation commands and
                   do the necessary cleanup. Updated copyright.
12/20/00   ram     Changed sib_cell_info to get_all_sibs_req as per the change
                   in the command definition. Included rrcdata_v.h.
12/15/00   ram     The MIB and SIB3 need to be decoded before an index
                   can be found for a cell in the SIB database, since the
                   PLMN and Cell Ids are needed for that. Updated the module
                   to hold MIB and SIB3 in the events database to accomplish
                   this. Implemented the function process_completed_sibs partially.
                   this function takes SIBs that are collected by rrcsibcollect
                   module, decodes them and puts them in the right place in the
                   database.
12/07/00   ram     Updated module to follow the new rrc command structure.
                   Added a call to initialize the SIB database from
                   rrc_sib_proc_init. Added more comments.
12/01/00   ram     Included err.h and changed some MSG_HIGHs to ERRs. Corrected
                   syntax errors.
11/29/00   ram     Updated the entire module to use command queues
                   instead of os events.
11/14/00   kmp     Updated to fix compiler warnings and link errors.
11/06/00   ram     Added function headers and comments. 
11/01/00   ram     Updated to use sibcollect module to collect SIBs. Corrected
                   syntax errors.
10/21/00   ram     Created file.

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
#include "rrcasn1.h"
#include "rrcasn1util.h"
#include "rrccmd_v.h"
#include "rrccmdi.h"
#include "rrcdata_v.h"
#include "rrcllc.h"
#include "rrclcm.h"
#include "rrcscmgr.h"
#include "rrcsibproc.h"
#include "rrcsibdbi.h"
#include "rrcsibdb.h"
#include "rrccsp.h"
#include "rrctmr.h"
#include "rrcumi.h"
#include "uecomdef.h"
#include "rrcllcoc.h"
#include "rrccspi.h"
#include "rrcccm.h"
#include "rrcpg1.h"
#include "rrclogging.h"
#include "rrcmcm.h"
#include "rrcrcei.h"
#include "rrcrce.h"
#include "rrccu.h"
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
#include "bit.h"
#endif

#include "rrcdispatcher.h"
#ifdef FEATURE_WCDMA_ETWS
#include "rrcbmc.h"
#endif

#include "rrcnv.h"

#ifdef FEATURE_QCHAT
#include "rrc_qchatupk.h"
#endif

#ifdef FEATURE_FEMTO_CSG
#include "sys.h"
#endif

#ifdef FEATURE_DUAL_SIM
#include "rrcwrm.h"
#endif

#include "l1rrcif.h"
#include "rrclog.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
/* Define a maximum time to wait for SIBs in a new cell. Right now
it's selected as 12 seconds. This may be changed later if needed */
#define RRCSIB_MAX_WAIT_TIME_FOR_SIBS 12000

#define RRCSIB_TENMIN_UPDATE_TIMER 60000

#define MAX_CELL_INDEX_COUNT_TENMIN 10

/* Indicates the SFNs to be set before SB position */
/* Supposedly read from a NV */
#define MAX_SFN_BEFORE_SB_SLEEP  8

/* 
   Define the wait factor of SIB for BPLMN search as 30(3 * 10), which is 3 times the 10 ms frame numbers 
   Also set the wait timer for receiving a MIB to be 300ms, if not received by that time, we should not
   bother reading this cell
*/
#define RRCSIB_BPLMN_NBCCH_SIB_WAIT_FACTOR 30
#define RRCSIB_BPLMN_MIB_WAIT_TIMER  300
#define RRCSIB_NBCCH_MIB_WAIT_TIMER  2000

/*In most of the N/W SIB3 ret cycle is set to 640 msec
  So set MAx time to wait for SIB3 to 640msec, Any ways
  correct value of the timer will be started after receiving
  MIB for this feature*/
#define RRCSIB_MAX_SIB3_WAIT_TIMER  640

/*We want to give two chances for reading SIB3
   in DCH so wait factor 2*10msec = 20*/
#define RRCSIB_CELL_ID_SIB3_WAIT_FACTOR 20

/* Define a maximum time to wait for SIBs in a existing cell. 
This is when we're already camped on the cell but we receive
indication that the SIBs have been modified. Here we wait
for a longer time since the SFN at which they will be modified
may be in the future. Worst case is 1 SFN cycle plus 12 seconds. */
#define RRCSIB_MAX_WAIT_TIME_FOR_UPDATE_SIBS 52880

/* Default expiration time for SIB7 */
#define RRCSIB_DEFAULT_EXP_TIME_FOR_SIB7 320

#ifdef FEATURE_WCDMA_HS_FACH_DRX
  #define RRCSIB_DEFAULT_EXP_TIME_FOR_EFACH_DRX_SIB7 800
#endif
#if defined(FEATURE_WCDMA_DRX_MANAGER) || defined(FEATURE_UPDATE_SIB7_IN_FACH)

  /* Size of SIB Schedule table in bytes */
  #define RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE 512
  
  /*Both the below values should not be odd*/
  #define RRCSIB_SLEEP_SFN 16
  #define RRCSIB_WAKE_SFN  16
  
  /*Minimum duration for SIB mask invert timer*/
  #define RRCSIB_MIN_MASK_INV_TIMER RRCSIB_SLEEP_SFN + RRCSIB_WAKE_SFN
#endif
#ifdef FEATURE_SIB19_DECODE_FAIL_OPT
/* This is decided internally and it can be fine tuned */
#define SIB19_MAX_DECODE_FAIL_CNT 2
#endif

#ifdef FEATURE_RRC_SIB_SUPPORT_MAX_REPT_RATE
#define RRC_SIB_MAX_REP_RATE_SIB_WAIT_TIMER  1024
#define RRC_RADIO_FRAME_TO_MILLISECOND_CONV  10
#endif

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
#ifdef FEATURE_SIB19_DECODE_FAIL_OPT
LOCAL uint32 sib19_decode_fail_cnt=0; /*Used to track number of SIB19 decoding failures */
#endif

#ifdef FEATURE_DUAL_SIM
extern boolean rrc_drx_exp_when_wtng_for_mib;
#endif

/* Declare a 'sib_events' database. This is used to store information
about each active SIB processing event. */
sib_active_event_database_type *sib_events_ptr=NULL;

/* Boolean to indicate if an RRCSIB_UPDATE_SIBS event is pending. */
boolean update_sibs_event_pending;

/* Data type to hold information about SIB7 expiration */
rrc_sib_exp_timer_sib_info_type sib7info;

/* Enumerated type of status of configuring BCCH for new
events. */
typedef enum
{
  RRCSIB_BCCH_SETUP_FAILED, /* BCCH setup failed */
  RRCSIB_BCCH_EVENT_SAVED,  /* The event is saved for later due to
                            existing BCCH event  */
  RRCSIB_BCCH_SETUP_DONE    /* BCCH setup was done successfully */
}rrcsib_bcch_setup_status_e_type;

/* To note if power-up initialization has been done already.
This is used in initializing variables for cases other than power-up
init such as FTM and LPM */
boolean rrcsib_powerup_init_done = FALSE;

#ifdef FEATURE_UPDATE_SIB7_IN_FACH
#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
  rex_crit_sect_type sib7_in_fach_active_mutex;
  boolean is_sib7_in_fach_active = FALSE;
#endif
#endif

/* SIB procedure will fill the channel config request with the current value of the 
transaction id. Transaction ids will help SIB associate a channel config confirm
with a channel config request that it had sent */
uint8  transaction_id = 0;

/* Global Flag which prevents BPLMN(If BPLMN request is received when UPDATE_SIBs
  was on going) to start till all the SIB_Change_ind commands are processed by RRC 
  procedures & L1*/
boolean sib_change_safe_guard;

/* Boolean to indicate whether SIB Sleep table will be sent to L1 only after receiving
scheduling info for all SIBs. */
boolean sib_sleep_after_sb1_sb2 = FALSE;

//lookup table to keep track of supported sibs
uint8 supported_sibs_lookup_table[RRC_TOT_SIBS];
#if defined(FEATURE_WCDMA_DRX_MANAGER) || defined(FEATURE_UPDATE_SIB7_IN_FACH)

/* SIB schedule table that holds the SIB scehduling info */
uint8 sib_sleep_schedule_table_0[RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE];
uint8 sib_sleep_schedule_table_1[RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE];
uint8 sib_sleep_schedule_table_in_use;
#endif
/* Structure to store BPLMN SIBs */
rrcsib_bplmn_sibs_type rrcsib_bplmn_sibs;

/* Global command store */
rrcsib_cmd_store_type rrcsib_cmd_store;

#ifdef FEATURE_RRC_NO_MIB_FOUND_BAR_CELL
uint8 no_mib_bar_cell_count=0;
#endif

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
extern rrc_deferred_sib_info_type deferred_sib_info;
#endif

extern uint32 xtow_bplmn_search_overhead_timer;

#ifdef FEATURE_3GPP_CSFB
/* Flag to indicate whether SIB container in use or not */
boolean is_sib_cont_present;
#endif

#ifdef FEATURE_RRC_SIB_SUPPORT_MAX_REPT_RATE
uint32 max_sib_rept_rate   = 0;
int32 timer_value_started  = 0;
#endif
#define MAX_FDD1_BANDS 8
#define MAX_FDD2_BANDS 16

#ifdef FEATURE_DUAL_SIM
boolean rrcsib_pattern_invert = FALSE;
boolean rrcsib_stop_partition = FALSE;
boolean g_in_data_call = FALSE;
#endif

uint16 Fdd1_internal_band[MAX_FDD1_BANDS]={RRC_CSP_IMT2K_BAND_INDEX,RRC_CSP_PCS_BAND_INDEX,RRC_CSP_WCDMA_1800_BAND_INDEX,
                                           RRC_CSP_WCDMA_800_BAND_INDEX,RRC_CSP_WCDMA_1700_2100_BAND_INDEX,
                                           RRC_CSP_WCDMA_850_BAND_INDEX,RRC_CSP_BAND_MAX_INDEX,RRC_CSP_BAND_MAX_INDEX};

uint16 Fdd2_internal_band[MAX_FDD2_BANDS]={RRC_CSP_WCDMA_900_BAND_INDEX,RRC_CSP_WCDMA_1700_BAND_INDEX,RRC_CSP_BAND_MAX_INDEX,
                                           RRC_CSP_WCDMA_1500_BAND_INDEX,RRC_CSP_BAND_MAX_INDEX,RRC_CSP_BAND_MAX_INDEX,
                                           RRC_CSP_BAND_MAX_INDEX,RRC_CSP_BAND_MAX_INDEX,RRC_CSP_BAND_MAX_INDEX,
                                           RRC_CSP_BAND_MAX_INDEX,RRC_CSP_BAND_MAX_INDEX,RRC_CSP_WCDMA_BC19_BAND_INDEX,
                                           RRC_CSP_BAND_MAX_INDEX,RRC_CSP_BAND_MAX_INDEX,RRC_CSP_BAND_MAX_INDEX,
                                           RRC_CSP_BAND_MAX_INDEX};
#define RRC_PICH_AND_MEASUREMENTS_TIME 150
/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */
#define GET_TRANS_ID() (++transaction_id % 255)

#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR
extern boolean rrc_mfbi_support_nv;
#endif

/*===========================================================================

FUNCTION CEHCK_EVENT_COMPLETION_STATUS

DESCRIPTION

  This function checks the status of the sib event, the index of
  which is passed to this function. If all SIBs for the event
  have been received, it will return TRUE. Else it will return FALSE.
    
DEPENDENCIES
 
  None.

RETURN VALUE

  TRUE is event has been completed, else FALSE.

SIDE EFFECTS

  None.
  
===========================================================================*/
static boolean check_event_completion_status
(
  uint32 index  /* Index of the event in the sib events database */
  #ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
  , boolean deferred_meas_read_supported
  #endif
);

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

FUNCTION RRCSIB_IS_SIB_ACTIVE

DESCRIPTION

  
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
boolean rrcsib_is_sib_active
(
  void
)
{
  uint32 count;
  boolean status = FALSE;
  
  /* Check if there is anything pending in the cmd store */
  if(update_sibs_event_pending == TRUE)
  {
    status = TRUE;
  }
  else if (sib_events_ptr == NULL)
  {
    status = FALSE;
  }
  else
  {
    /* Check if any event is active */
    for( count = 0; count < (uint32)MAX_SIB_EVENTS_INDEX; count++ )
    {
      if( sib_events_ptr->event[count].event_active == TRUE )
      {
        status = TRUE;
        break;
      }/* End if active events */
    }
  }

  MSG_HIGH("Update SIBs pending flag: %d, SIB events ptr: %d, status: %d",
                       update_sibs_event_pending,sib_events_ptr,status);

  return status;
}                
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/

#if defined(FEATURE_WCDMA_DRX_MANAGER) || defined(FEATURE_UPDATE_SIB7_IN_FACH)

/*===========================================================================

FUNCTION rrcsib_gen_bit_mask_for_schedule_table

DESCRIPTION

  This function will generate Bit mask needed for filling 
  SIB schedule table
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
     
===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/

uint8 rrcsib_gen_bit_mask_for_schedule_table
(
  uint8 byte_offset
)
{
  uint8 bit_mask = 0xFF;
  
  /* The byte_offsets can only be 0,2,4 or 6.Since SIBs are always scheduled on 20ms
       frames,the next sfn after the offset also needs to be set to 1.Always OR the bitmask
       so that settings from other SIB Schedule will not be lost*/
  switch(byte_offset)
  {
    case 0:
      bit_mask = 0xC0; /* 1100 0000 */
    break;
    case 2:
      bit_mask = 0x30; /* 0011 0000 */
    break;
    case 4:
      bit_mask = 0x0C; /* 0000 1100 */   
    break;
    case 6: 
      bit_mask = 0x03; /* 0000 0011 */
    break;
    default:
    /* This cannot happen since Scheduling is always based on even numbered SFN.
         Be conservative and set it to 0xFF */
      WRRC_MSG0_ERROR("SIB_Sleep:Invalid segment offset for SIB ");
      bit_mask = 0xFF;
    break;
  }
  return bit_mask;
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
#endif
/*===========================================================================
FUNCTION rrc_sib_is_current_event_sib7_in_fach
DESCRIPTION
  This function will check whether the current event is sib-7 event 
  DEPENDENCIES
  None.
RETURN VALUE
  TRUE: If event is sib-7 event in FACH.
  FALSE: If event is other than sib-7 event.
SIDE EFFECTS
  None.
===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
boolean rrc_sib_is_current_event_sib7_in_fach(void)
{
  uint8 count;
  boolean status = FALSE;
  for( count = 0; count < (uint32)MAX_SIB_EVENTS_INDEX; count++ )
  {
    if( sib_events_ptr->event[count].event_active == TRUE )
    {
      if(sib_events_ptr->event[count].event_name == RRCSIB_GET_SPECIFIC_SIBS)
      {
        if((sib_events_ptr->event[count].int_event.int_event_type == RRCSIB_INT_SIB7_EVENT)&&
           (sib_events_ptr->event[count].req_proc == RRC_PROCEDURE_SIB)&&
           (sib_events_ptr->event[count].for_serving_cell == TRUE)&&
           (rrc_get_state() == RRC_STATE_CELL_FACH)
          )
        {
          status = TRUE;
          break;
        }
      }
    }/* End if active events */
  }
  return status;
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/

/*===========================================================================
FUNCTION rrc_sib_is_sib_segment_needed_by_rrc_fach
DESCRIPTION
  This function will check whether the currently received 
  SIB segment by L1 is needed by RRC or not.If its not 
  needed then L1 will not forward this sib segment to RRC.

  The input to this function is SFN, of currently received 
  SIB segment.
DEPENDENCIES
  None.
RETURN VALUE
  TRUE: If this sib segment is needed by RRC.
  FALSE: other wise.
SIDE EFFECTS
  None.
===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
boolean rrc_sib_is_sib_segment_needed_by_rrc_fach(uint16 sfn)
{
  boolean status = FALSE;

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
  boolean sib7_in_fach_status = FALSE;

  REX_ISR_LOCK(&sib7_in_fach_active_mutex);
  sib7_in_fach_status = is_sib7_in_fach_active;
  REX_ISR_UNLOCK(&sib7_in_fach_active_mutex);

  if(sib7_in_fach_status == TRUE)
  {
    uint16 byte_pos = sfn>>3;                /* (sfn/8) Gives the byte location in sib sleep schedule table */
    uint8 byte_offset = (sfn & 0x07);     /* (sfn%8)Gives the offset within the byte in sib sleep table */
    uint8 bit_mask = 0xFF;
    bit_mask = rrcsib_gen_bit_mask_for_schedule_table(byte_offset);
    if((sib_sleep_schedule_table_0[byte_pos]&bit_mask) != 0)
    {
      status = TRUE;
    }
  }
  else
  {
    status = TRUE;
  }
#else
  if(rrc_sib_is_current_event_sib7_in_fach() == TRUE)
  {
    uint16 byte_pos = sfn>>3;                /* (sfn/8) Gives the byte location in sib sleep schedule table */
    uint8 byte_offset = (sfn & 0x07);     /* (sfn%8)Gives the offset within the byte in sib sleep table */
    uint8 bit_mask = 0xFF;
    bit_mask = rrcsib_gen_bit_mask_for_schedule_table(byte_offset);
    if((sib_sleep_schedule_table_0[byte_pos]&bit_mask) != 0)
    {
      status = TRUE;
    }
  }
  else
  {
    status = TRUE;
  }
#endif
  return status;
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/

/*===========================================================================
FUNCTION rrc_sib_can_l1_start_fmo
DESCRIPTION
  This function will check whether the current event is sib-7 event 
  which is needed for starting FMO's at L1 in FACH state.
  FMO should take high priority than SIB-7 event.During the 
  frames where FMO's are going on L1 will through away all
  the sib segments, since the content may be incorrect.
DEPENDENCIES
  None.
RETURN VALUE
  TRUE: If event is sib-7 event in FACH, or no sib event present.
  FALSE: If event is other than sib-7 event.
SIDE EFFECTS
  None.
===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
boolean rrc_sib_can_l1_start_fmo(void)
{
  boolean  status = FALSE;
#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
  boolean  sib7_in_fach_status = FALSE;

  REX_ISR_LOCK(&sib7_in_fach_active_mutex);
  sib7_in_fach_status = is_sib7_in_fach_active;
  REX_ISR_UNLOCK(&sib7_in_fach_active_mutex);

  if(sib7_in_fach_status == TRUE)
  {
    /*SIB-7 event is active in Cell FACH,So FMO can be started  */
    status = TRUE;
  }
  else
  {
    if(rrcsib_is_sib_active() != TRUE)
    {
      /*No SIB event active, So FMO can be started*/
      status = TRUE;
    }
  }
#else
  if(rrc_sib_is_current_event_sib7_in_fach() == TRUE)
  {
    /*SIB-7 event is active in Cell FACH,So FMO can be started  */
    status = TRUE;
  }
  else
  {
    if(rrcsib_is_sib_active() != TRUE)
    {
      /*No SIB event active, So FMO can be started*/
      status = TRUE;
    }
  }
#endif
  WRRC_MSG1_HIGH("L1 to start FMO status %d",status);
return status;
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
#if defined(FEATURE_WCDMA_DRX_MANAGER)

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*===========================================================================

FUNCTION RRCSIB_INITIALIZE_SIB_SLEEP_SCHEDULE_TABLE

DESCRIPTION

  This function initializes the SIB schedule table  
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
     
===========================================================================*/
void rrcsib_initialize_sib_sleep_schedule_table
(
  void
);

/*===========================================================================

FUNCTION RRCSIB_UPDATE_SIB_SLEEP_SCHEDULE_TABLE

DESCRIPTION

  This function updates the SIB schedule table  
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
     
===========================================================================*/
void rrcsib_update_sib_sleep_schedule_table
(
  sib_events_index_e_type event_index   /* Index of the current event */
);

#endif
/*===========================================================================

FUNCTION RRCSIB_CHECK_BAND_MISMATCH_FROM_SIB5_SIB6

DESCRIPTION

  This function will be used to find if the band present in SIB5/SIB6
  is supported by the UE or not.
  
DEPENDENCIES

  None.

RETURN VALUE

  TRUE - If there is a band mismatch
  FALSE - If there is no band mismatch or the band info is not specified in SIB5/SIB6
  
SIDE EFFECTS

  None

===========================================================================*/
boolean rrcsib_check_band_mismatch_from_sib5_sib6(uint32 index);


/*===========================================================================

FUNCTION POST_PROCESS_COMPLETED_EVENT

DESCRIPTION

  This function performs all the necessary post-processing for events
  that have been completed (i.e. all necessary sibs have been received).
  Post-processing includes -
  
    1) Sending a confirmation event if necessary to the module that
       requested the event.
       
    2) Tearing down the BCCH radio-bearer if it's not needed anymore.
    
    3) Cleanup of internal variables related to the event.
    
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
     
===========================================================================*/
void post_process_completed_event
(
  sib_events_index_e_type index /* Index of the event in the sib event database */
);

#ifdef FEATURE_WCDMA_TO_LTE
/*===========================================================================

FUNCTION rrcsib_check_and_remove_sib19_if_not_scheduled

DESCRIPTION

  This function check if SIB19 is presnt in sibdb & then check if SIB19 
  was removed from MIB/SB1/SB2 if yes then remove the entry from SIBdb
DEPENDENCIES

  None.

RETURN VALUE

  TRUE - If SIB4/6/12 Ind present in SIB3/5/11
  FALSE - Otherwise
  
SIDE EFFECTS

  None

===========================================================================*/
void rrcsib_check_and_remove_sib19_if_not_scheduled
(
  uint32 index  /* Index of the event in the sib events database */
)
{
  rrc_ExtSIBTypeInfoSchedulingInfo_List2 *ext_siblist2_ptr = NULL;
  uint32 dbindex; /* Index to look into the database if needed */
  rrc_MasterInformationBlock *mib_ptr; /* Local pointer to MIB       */
  rrc_SysInfoTypeSB1 *sb1_ptr;         /* Local pointer to SB1       */
  rrc_SysInfoTypeSB2 *sb2_ptr;         /* Local pointer to SB2       */
  boolean is_sib19_present = FALSE;
  uint8 idx=0;

  /* Check if the event is active. It could have failed
   due to a non-gsm map PLMN or a timeout, in which case
   the event will be inactive. */
  if( sib_events_ptr->event[index].event_active == FALSE )
  {
    /* Nothing to do here */
    return;
  }
  if( sib_events_ptr->event[index].sib_db_index_found == TRUE )
  {
    /* Database index is found. Get the index */
    dbindex = sib_events_ptr->event[index].sib_db_index;
  }
  else
  {
    /* If the database index has not been found, the
    event can't be completed. Just return FALSE */
    return;
  }

  if(sibdb.cell[dbindex].sib_valid[rrc_SIB19] == TRUE)
  {
    mib_ptr = rrc_sibdb_return_sib_for_db_index(rrc_MIB,dbindex);
    
    if((mib_ptr != NULL)&&(EXTENSION_SIB_SCHED_2_PRESENT_IN_MIB(mib_ptr)))
    {
      ext_siblist2_ptr = &mib_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.
          v860NonCriticalExtensions.masterInformationBlock_v860ext.extSIBTypeInfoSchedulingInfo_List;
    }

    if(ext_siblist2_ptr == NULL)
    {
      sb2_ptr = rrc_sibdb_return_sib_for_db_index(rrc_SB2,dbindex);
      if((sb2_ptr != NULL)&&(EXTENSION_SIB_SCHED_2_PRESENT_IN_SB1_SB2(sb2_ptr,
          rrc_SysInfoTypeSB2,sysInfoTypeSB2)))
      {
        ext_siblist2_ptr = &sb2_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.
         sysInfoTypeSB2_v860ext.extSIBTypeInfoSchedulingInfo_List;
      }
      if(ext_siblist2_ptr == NULL)
      {
        sb1_ptr = rrc_sibdb_return_sib_for_db_index(rrc_SB1,dbindex);
  
        if((sb1_ptr != NULL)&&(EXTENSION_SIB_SCHED_2_PRESENT_IN_SB1_SB2(sb1_ptr,
            rrc_SysInfoTypeSB1,sysInfoTypeSB1)))
        {
          ext_siblist2_ptr = &sb1_ptr->v6b0NonCriticalExtensions.
             v860NonCriticalExtensions.sysInfoTypeSB1_v860ext.extSIBTypeInfoSchedulingInfo_List;
        }
      }
    }

    while((ext_siblist2_ptr != NULL) && (ext_siblist2_ptr->n > idx))
    {
      if(ext_siblist2_ptr->elem[idx].extensionSIB_Type2.t == T_rrc_SIB_TypeExt2_systemInfoType19)
      {
        is_sib19_present = TRUE;
        break;
      }
      idx++;
    }
    if(is_sib19_present == FALSE)
    {
      WRRC_MSG1_HIGH("sib19:removed sib19 from SIBdb. Event: %d",sib_events_ptr->event[index].event_name);
      if( sibdb.cell[dbindex].sibs.sib19 != NULL )
      {
        rrc_sibdb_free_sib(rrc_SysInfoType19_PDU,
                           (void *)sibdb.cell[dbindex].sibs.sib19);
        sibdb.cell[dbindex].sibs.sib19 = NULL;
      }
#ifdef FEATURE_RRC_OPTIMIZED_SIB_HEAP
      memset(sibdb.cell[dbindex].sibs.sib19_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
      sibdb.cell[dbindex].sibs.sib19_encoded_pdu_length = INVALID_PDU_LENGTH;
#endif
      if(sib_events_ptr->event[index].event_name == RRCSIB_UPDATE_SIBS)
      {
        sibdb.active_cell_sib_changed[rrc_SIB19] = TRUE;
      }
      sibdb.cell[dbindex].sib_valid[rrc_SIB19] = FALSE;
      sibdb.cell[dbindex].sib_value_tag[rrc_SIB19] = INVALID_MIB_SB_VALUE_TAG;
    }
  }
}
#endif

/*===========================================================================

FUNCTION rrcsib_check_if_connected_mode_sibs_present

DESCRIPTION

  This function will be used to find if SIB4/6/12 Ind present in 
  encoded format of SIB3/5/11
DEPENDENCIES

  None.

RETURN VALUE

  TRUE - If SIB4/6/12 Ind present in SIB3/5/11
  FALSE - Otherwise
  
SIDE EFFECTS

  None

===========================================================================*/
boolean rrcsib_check_if_connected_mode_sibs_present
(
  byte val,
  rrc_SIB_type rrc_sib_type
)
{
  boolean status = FALSE;
  switch(rrc_sib_type) 
  {
    case rrc_SIB3:
      if(val & SIB4_PRESENT_BIT_MASK)
      {
        status = TRUE;
      }
      break;

    case rrc_SIB5:
    case rrc_SIB5bis:
      if(val & SIB6_PRESENT_BIT_MASK)
      {
        status = TRUE;
      }
      break;

    case rrc_SIB11:
      if(val & SIB11_PRESENT_BIT_MASK)
      {
        status = TRUE;
      }
      break;

      default:
        WRRC_MSG1_HIGH("SIBtype %d not supported", rrc_sib_type);
        break;
  }

  WRRC_MSG2_HIGH("SIB%d indication present: %d", 
                      rrcsib_map_int_sibtype_to_asn1_sibtype(rrc_sib_type),status);
  return status;
}

#if  defined(FEATURE_UPDATE_SIB7_FOR_PCH_TO_FACH)
/*===========================================================================

FUNCTION RRCSIB_CONVERT_SIB7_REP_TIME_TO_MS

DESCRIPTION

  This function determines the value of the expiration
  timer for SIB7 for the serving cell.
  
  The expiration time is defined as -
  
  MAX( 320ms, SIB_REP*Expiration Time Factor)

  This function finds SIB_REP of SIB7 from
  the MIB or the Scheduling Blocks and the
  Expiration Time Factor from SIB7 and calculates
  the value of the timer.

DEPENDENCIES
 
  None.

RETURN VALUE

  SIB7 expiration timer value in milliseconds.

SIDE EFFECTS

  None.
   
===========================================================================*/
static uint32 rrcsib_convert_sib7_rep_time_to_ms
(
rrc_SchedulingInformation * sib_rep,
rrc_ExpirationTimeFactor exp_factor
)
{
  uint32 sib7exp;

  switch(RRCGET_SCHD_INFO_TAG((&sib_rep->scheduling.sib_Pos)))
  {
    //convert the rep rate to ms and multiply with exp_factor.. rep4 represents
    //4 frames or 40ms and so on..
    case      RRCGET_SCHD_INFO_SIB_POS(rep4):
      sib7exp = 40 * exp_factor;
      break;

    case      RRCGET_SCHD_INFO_SIB_POS(rep8):
      sib7exp = 80 * exp_factor;
      break;

    case      RRCGET_SCHD_INFO_SIB_POS(rep16):
      sib7exp = 160 * exp_factor;
      break;

    case      RRCGET_SCHD_INFO_SIB_POS(rep32):
      sib7exp = 320 * exp_factor;
      break;

    case      RRCGET_SCHD_INFO_SIB_POS(rep64):
      sib7exp = 640 * exp_factor;
      break;

    case      RRCGET_SCHD_INFO_SIB_POS(rep128):
      sib7exp = 1280 * exp_factor;
      break;

    case      RRCGET_SCHD_INFO_SIB_POS(rep256):
      sib7exp = 2560 * exp_factor;
      break;

    case      RRCGET_SCHD_INFO_SIB_POS(rep512):
      sib7exp = 5120 * exp_factor;
      break;

    case      RRCGET_SCHD_INFO_SIB_POS(rep1024):
      sib7exp = 10240 * exp_factor;
      break;

    case      RRCGET_SCHD_INFO_SIB_POS(rep2048):
      sib7exp = 20480 * exp_factor;
      break;

    case      RRCGET_SCHD_INFO_SIB_POS(rep4096):
      sib7exp = 40960 * exp_factor;
      break;

    default:

      WRRC_MSG0_ERROR("Invalid SIB_REP in MIB!");
      sib7exp = RRCSIB_DEFAULT_EXP_TIME_FOR_SIB7;
      break;
  }

  return sib7exp;
}
/*===========================================================================

FUNCTION RRCSIB_GET_SIB7_EXP_TIMER_VALUE

DESCRIPTION

  This function determines the value of the expiration
  timer for SIB7 for the serving cell.
  
  The expiration time is defined as -
  
  MAX( 320ms, SIB_REP*Expiration Time Factor)

  This function finds SIB_REP of SIB7 from
  the MIB or the Scheduling Blocks and the
  Expiration Time Factor from SIB7 and calculates
  the value of the timer.

DEPENDENCIES
 
  None.

RETURN VALUE

  SIB7 expiration timer value in milliseconds.

SIDE EFFECTS

  None.
   
===========================================================================*/
unsigned long  rrcsib_get_sib7_exp_timer_value( uint32 sib_db_index )
{
  unsigned long sib7exp=0;        /* To store SIB7 expiration time   */
  uint32 index;                   /* Index into sib database         */
  rrc_SIBSb_ReferenceList *sb_ptr; /* Local pointer to SIBSb ref list */
  rrc_SIB_ReferenceList *ptr;      /* Local pointer to SIB ref list   */
  boolean sib7_found = FALSE;     /* Indicate if sib7 is found       */
  boolean sb1_found = FALSE;      /* Indicate if sb1 is found incase
                                     sib7 is not present in MIB      */
  boolean sb2_found = FALSE;      /* Indicate if sb2 is found incase
                                     sib7 is not present in MIB      */
  rrc_ExpirationTimeFactor exp_factor; /* Expiration timer factor    */
  uint32 count=0, n_value=0;
  void *sib_ptr;

  /* First check if SIB7 has the expiration timer factor
  present. If not, we just return the default timer value */
  index = sib_db_index;  /* Note the current serving cell   */

#ifdef FEATURE_WCDMA_HS_FACH_DRX
    if(rrcenhstate_camped_cell_supports_e_fach()
        && rrcenhstate_camped_cell_supports_e_fach_drx()
         && (rrc_get_state() ==RRC_STATE_CELL_FACH))
   {
     return RRCSIB_DEFAULT_EXP_TIME_FOR_EFACH_DRX_SIB7;
   }
#endif

  if((sib_ptr = rrc_sibdb_return_sib_for_db_index(rrc_SIB7, index)) != NULL)
  {
    if(RRCCHECK_SIB_MSG_IE_PRESENT(((rrc_SysInfoType7*)sib_ptr),
                                      rrc_SysInfoType7_,
                                      expirationTimeFactor)
    )
    {
      /* Expiration timer factor is present */
      exp_factor =  ((rrc_SysInfoType7*)sib_ptr)->expirationTimeFactor;
    }
    else
    {
      /* Expiration timer factor not present */
      return( RRCSIB_DEFAULT_EXP_TIME_FOR_SIB7 );
    }
  }
  else
  {
    /* Can't find SIB 7! */
    WRRC_MSG0_ERROR("SIB7 not present in srv cell!");
    return( RRCSIB_DEFAULT_EXP_TIME_FOR_SIB7 );
  }

  if((sib_ptr = rrc_sibdb_return_sib_for_db_index(rrc_MIB, index)) != NULL)
  {
    /* If the MIB is valid */
    sb_ptr= &(((rrc_MasterInformationBlock*)sib_ptr)->sibSb_ReferenceList);
    count = 0;
    n_value = sb_ptr->n;

    while( n_value != 0 )
    {
      /* Check if this is for SIB7 */
      
      if(RRCGET_SCHD_INFO_TAG((&sb_ptr->elem[count].sibSb_Type)) == 
         (RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType7)))
      {
        /* We found it */
        sib7_found = TRUE;
        break; /* Out of the while loop */
      }

      /* Else check if this is for SB1 */
      if(RRCCHECK_SIBSb_TypeAndTag((&sb_ptr->elem[count].sibSb_Type),sysInfoTypeSB1))
      {
        /* We found it */
        sb1_found = TRUE;
      }

      /* Else check if this is for SB2 */
      if(RRCCHECK_SIBSb_TypeAndTag((&sb_ptr->elem[count].sibSb_Type),sysInfoTypeSB2))
      {
        /* We found it */
        sb2_found = TRUE;
      }
      /* Now increment the pointer */
      n_value--;
      count++;
    }

    if( (n_value != 0) && (sib7_found == TRUE) )
    {
      sib7exp = rrcsib_convert_sib7_rep_time_to_ms(&sb_ptr->elem[count].scheduling, exp_factor);
    }
  }/* end if MIB is valid */
  else
  {
    /* Can't find MIB! */
    WRRC_MSG0_ERROR("MIB not present in srv cell!");
    return( RRCSIB_DEFAULT_EXP_TIME_FOR_SIB7 );
  }


  /* If SIB7 was not found in the MIB.  It could be
  in one of the scheduling blocks */
  if( (sib7_found == FALSE) && (sb1_found == TRUE) )
  {
    /* Look in SB1 for SIB7 info */
    if((sib_ptr = rrc_sibdb_return_sib_for_db_index(rrc_SB1, index)) != NULL)
    {
      /* If the SB1 is valid */
      ptr= &(((rrc_SysInfoTypeSB1*)sib_ptr)->sib_ReferenceList);
      count = 0;
      n_value = ptr->n;
      while(n_value != 0 )
      {
        /* Check if this is for SIB7 */
        if(RRCGET_SCHD_INFO_TAG((&ptr->elem[count].sib_Type)) == 
            (RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType7)))
        {
          /* We found it */
          sib7_found = TRUE;
          break; /* Out of the while loop */
        }
        /* Now increment the pointer */
        n_value--;
        count++;
      }
      if( (n_value != 0) && (sib7_found == TRUE) )
      {
        sib7exp = rrcsib_convert_sib7_rep_time_to_ms(&ptr->elem[count].scheduling, exp_factor);
      }
    }
  } /* end if sb1_found == TRUE */

  else if( (sib7_found == FALSE) && (sb2_found == TRUE) )
  {
    /* Repeat the same search in Scheduling Block 2 */
    if((sib_ptr = rrc_sibdb_return_sib_for_db_index(rrc_SB2, index)) != NULL)
    {
      /* If the SB2 is valid */
      ptr= &(((rrc_SysInfoTypeSB2*)sib_ptr)->sib_ReferenceList);
      count = 0;
      n_value = ptr->n;
      
      while(n_value != 0 )
      {
        /* Check if this is for SIB7 */
        if(RRCGET_SCHD_INFO_TAG((&ptr->elem[count].sib_Type)) == 
            (RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType7)))
        {
          /* We found it */
          sib7_found = TRUE;
          break; /* Out of the while loop */
        }
        /* Now increment the pointer */
        n_value--;
        count++;
      }
      if( (n_value != 0) && (sib7_found == TRUE) )
      {
        sib7exp = rrcsib_convert_sib7_rep_time_to_ms(&ptr->elem[count].scheduling, exp_factor);
      }
    }
  } /* end if sb2_found == TRUE */


  /* Finally, check if sib7 scheduling info
  was found in any of the 3 blocks */
  if( sib7_found == TRUE )
  {
    /* Check the MAX of default expiration time and
    found expiration time */
    if( sib7exp < RRCSIB_DEFAULT_EXP_TIME_FOR_SIB7 )
    {
      sib7exp = RRCSIB_DEFAULT_EXP_TIME_FOR_SIB7;
    }
  }
  else
  {
    /* SIB7 scheduling info was not found. Use
    the default */
    sib7exp = RRCSIB_DEFAULT_EXP_TIME_FOR_SIB7;
  }

  return( sib7exp );

}
#endif
/*===========================================================================

FUNCTION RRCSIB_FREE_TSIB

DESCRIPTION


DEPENDENCIES
 
  None.

RETURN VALUE


SIDE EFFECTS

  None.
   
===========================================================================*/
static void rrcsib_free_tsib
(
  sib_events_index_e_type event_index
)
{

  if(EVENT_INDEX_VALID(event_index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return;
  }
  if(sib_events_ptr == NULL)
  {
    return;
  }
  /* In case the tempholer pointers are not freed, free them
  now */
  if( sib_events_ptr->event[event_index].tempholder.mib != NULL )
  {
    rrc_sibdb_free_sib(  rrc_MasterInformationBlock_PDU,
     (void *)sib_events_ptr->event[event_index].tempholder.mib );
    sib_events_ptr->event[event_index].tempholder.mib = NULL;
  }
  
  if( sib_events_ptr->event[event_index].tempholder.sib3 != NULL )
  {
    rrc_sibdb_free_sib(  rrc_SysInfoType3_PDU,
     (void *)sib_events_ptr->event[event_index].tempholder.sib3 );
    sib_events_ptr->event[event_index].tempholder.sib3 = NULL;
  }
}
/*===========================================================================

FUNCTION rrcsib_configure_bcch

DESCRIPTION

  This function sends channel config req for S_BCCH and N_BCCH to setup/teardown 
  or NOOP on one of these channels
DEPENDENCIES
 
  None.

RETURN VALUE

  SUCCESS: If action is successful
  FAILURE: If action fails

SIDE EFFECTS

  None.
   
===========================================================================*/
static uecomdef_status_e_type  rrcsib_configure_bcch
(
  rrcsib_bcch_action_type s_bcch_action,
  rrcsib_bcch_action_type n_bcch_action
  /*Add reason for chan_config_req which is used by LLC*/
  ,rrcllc_chan_cfg_reason_e_type chan_config_reason
)
{
  rrc_cmd_type *bcch_cmd_ptr; /* To send an CHANNEL_CONFIG_REQ */
  uint32 num_rb=0;

  //N_BCCH teardown is not supported, also we don't support setting up of 2 serving
  //PCCPCH and also, 2 NOOP's are not needed to be sent to LLC
  if( ((s_bcch_action == SBCCH_SETUP) && (n_bcch_action == NBCCH_SETUP)) ||
       (n_bcch_action == NBCCH_TEARDOWN) ||
      ((s_bcch_action == SBCCH_NOOP) && (n_bcch_action == NBCCH_NOOP))
    )
  {
    WRRC_MSG2_ERROR("Invalid S_BCCH %d, N_BCCH %d action", s_bcch_action, n_bcch_action);
    return FAILURE;
  }

  if(( bcch_cmd_ptr = rrc_get_int_cmd_buf() ) != NULL )
  {
    bcch_cmd_ptr->cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;    
    bcch_cmd_ptr->cmd.chan_config_req.procedure = RRC_PROCEDURE_SIB;
    bcch_cmd_ptr->cmd.chan_config_req.num_rb_to_config = 0;
    bcch_cmd_ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = FALSE;
    bcch_cmd_ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;
    /*Add reason for chan_config_req which is used by LLC*/
    bcch_cmd_ptr->cmd.chan_config_req.chan_config_reason  =  chan_config_reason;
    //if a setup is being done, request a transaction id, else we don't wait for
    //a cnf.. no transaction id is needed
    if(n_bcch_action == NBCCH_SETUP || s_bcch_action == SBCCH_SETUP)
    {
      bcch_cmd_ptr->cmd.chan_config_req.transaction_id = GET_TRANS_ID();
      WRRC_MSG1_HIGH("Trans Id for chan_cfg_req setup %d", 
               bcch_cmd_ptr->cmd.chan_config_req.transaction_id);
    }
  }

  if(n_bcch_action == NBCCH_SETUP)
  {
    WRRC_MSG0_HIGH("Setting up N-BCCH");

    sib_events_ptr->event[SIB_NGH_INDEX].transaction_id = 
      (uint8)bcch_cmd_ptr->cmd.chan_config_req.transaction_id;

    /* The first index is used for the N-BCCH */
    bcch_cmd_ptr->cmd.chan_config_req.num_rb_to_config++;
    bcch_cmd_ptr->cmd.chan_config_req.rb[num_rb].rb_id = BCCH_N_RADIO_BEARER_ID;
    bcch_cmd_ptr->cmd.chan_config_req.rb[num_rb].rb_config = ESTABLISH_RB;
    bcch_cmd_ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = TRUE;

    //set action to PENDING until channel-config-cnf is received
    sib_events_ptr->ngh_bcch.bcch_action = SIB_BCCH_SETUP_PENDING;
    num_rb++;
#if defined(FEATURE_WCDMA_DRX_MANAGER)

    /* Initialize SIB Sleep schedule table */
    rrcsib_initialize_sib_sleep_schedule_table();
#endif
  }

  if(s_bcch_action == SBCCH_TEARDOWN)
  {
    WRRC_MSG0_HIGH("Tearing down S-BCCH");
    
    bcch_cmd_ptr->cmd.chan_config_req.num_rb_to_config++;
    bcch_cmd_ptr->cmd.chan_config_req.rb[num_rb].rb_id = BCCH_S_RADIO_BEARER_ID;
    bcch_cmd_ptr->cmd.chan_config_req.rb[num_rb].rb_config = RELEASE_RB;

    //set action to NO_ACTION as teardown is not required
    sib_events_ptr->srv_bcch.bcch_action = SIB_BCCH_NO_ACTION;    
    num_rb++;
  }
  else if(s_bcch_action == SBCCH_SETUP)
  {
    WRRC_MSG0_HIGH("Setting up S-BCCH");
#if defined(FEATURE_WCDMA_DRX_MANAGER)
    
    /* Initialize SIB Sleep schedule table */
    rrcsib_initialize_sib_sleep_schedule_table();
#endif
    sib_events_ptr->event[SIB_SRV_INDEX].transaction_id = 
      (uint8)bcch_cmd_ptr->cmd.chan_config_req.transaction_id;

    bcch_cmd_ptr->cmd.chan_config_req.num_rb_to_config++;
    bcch_cmd_ptr->cmd.chan_config_req.rb[num_rb].rb_id = BCCH_S_RADIO_BEARER_ID;
    bcch_cmd_ptr->cmd.chan_config_req.rb[num_rb].rb_config = ESTABLISH_RB;
    bcch_cmd_ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = TRUE;

    //set action to PENDING until channel-config-cnf is received
    sib_events_ptr->srv_bcch.bcch_action = SIB_BCCH_SETUP_PENDING;
    num_rb++;
  }

  rrc_put_int_cmd( bcch_cmd_ptr );

  return SUCCESS;
}
/*===========================================================================

FUNCTION RRCSIB_SEND_COMMAND_FOR_PENDING_UPDATE_SIBS_EVENT

DESCRIPTION

  This function sends a command to the SIB procedure itself
  to update SIBs based on value tags in the serving cell. This
  is only used when a earlier update_sibs command is pending to be
  processed.
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
   
===========================================================================*/
static void rrcsib_send_command_for_pending_update_sibs_event( void )
{
  rrc_cmd_type *ptr; /* To send an UPDATE_SIBS_IND cmd if needed */

  /* Post a command to ourselves for updating the SIBs */
  if( (ptr = rrc_get_int_cmd_buf()) != NULL)
  {     
    /* Fill in the command parameters for RRC_UPDATE_SIBS_IND */
    ptr->cmd_hdr.cmd_id = RRC_UPDATE_SIBS_IND;
         
    /* MIB value tag should be as before */
    ptr->cmd.update_sibs_ind.mib_value_tag = 
      sib_events_ptr->mib_value_tag_from_command;

    ptr->cmd.update_sibs_ind.real_sib_mod = 
      sib_events_ptr->update_sibs_real_sib_mod;

    ptr->cmd.update_sibs_ind.cause = sib_events_ptr->update_sibs_cause;
    /* Procedure name */
    ptr->cmd.update_sibs_ind.procedure = sib_events_ptr->update_sibs_req_proc;
    rrc_put_int_cmd(ptr);   
  } 
}/* end function rrcsib_send_command_for_pending_update_sibs_event */
      
/*===========================================================================

FUNCTION RRCSIB_SEND_COMMAND_FOR_SAVED_UPDATE_SIBS_EVENT

DESCRIPTION

  This function sends a command to the SIB procedure itself
  to update SIBs based on value tags in the serving cell. This
  is only used when a earlier update_sibs command is buffered so that it could 
  be processed later.
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
   
===========================================================================*/
static void rrcsib_send_command_for_saved_update_sibs_event( void )
{
  rrc_cmd_type *ptr; /* To send an UPDATE_SIBS_IND cmd if needed */

  /* Post a command to ourselves for updating the SIBs */
  if( (ptr = rrc_get_int_cmd_buf()) != NULL)
  {     
    /* Fill in the command parameters for RRC_UPDATE_SIBS_IND */
    ptr->cmd_hdr.cmd_id = RRC_UPDATE_SIBS_IND;
         
    /* MIB value tag should be as before */
    ptr->cmd.update_sibs_ind.mib_value_tag = 
      rrcsib_cmd_store.saved_cmd.cmd.update_sibs_ind.mib_value_tag;
    ptr->cmd.update_sibs_ind.real_sib_mod = 
      rrcsib_cmd_store.saved_cmd.cmd.update_sibs_ind.real_sib_mod;

    /* Procedure name */
    ptr->cmd.update_sibs_ind.procedure = rrcsib_cmd_store.saved_cmd.cmd.update_sibs_ind.procedure;
    rrc_put_int_cmd(ptr);   
  } 
}/* end function rrcsib_send_command_for_pending_update_sibs_event */

/*===========================================================================

FUNCTION RRCSIB_PROCESS_INT_SIB_EVENT_COMPLETE

DESCRIPTION

  This function does the necessary processing when 
  an internal SIB event is completed.
  
  When the SIB procedue itself requests a SIB event
  it's an internal event. Currently internal events
  are used only for updating SIB7 in various conditions.
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
   
===========================================================================*/
static void rrcsib_process_int_sib_event_complete
(
  sib_events_index_e_type index,        /* Index into event database     */
  uecomdef_status_e_type event_status   /* Success or Failure indication */
)
{
  rrc_cmd_type *ptr; /* To send a command to other procedures if needed */

  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return;
  }
  
  /* Currently we only process the SIB7 expiration through
  an internal event. Only check for that command here.
  Later other internal events may be used */
  if( (sib_events_ptr->event[index].req_proc == RRC_PROCEDURE_SIB) &&
      (sib_events_ptr->event[index].int_event.int_event_type == 
       RRCSIB_INT_SIB7_EVENT)
    )
  {
    /* SIB7 update can be based on timer or external procedure request.
    Check for both and process accordingly */
    if(sib7info.sib_ext_proc_update_req == TRUE)
    {
      /* Set the value back to FASLE */
      sib7info.sib_ext_proc_update_req = FALSE;
      if( (ptr = rrc_get_int_cmd_buf())!=NULL )
      {
        WRRC_MSG0_HIGH("Sending RRC_SIB7_UPDATE_STATUS_IND");
        ptr->cmd_hdr.cmd_id = RRC_SIB7_UPDATE_STATUS_IND;
        ptr->cmd.sib7_update_status.status = event_status;
        ptr->cmd.sib7_update_status.procedure = sib7info.req_proc;
        rrc_put_int_cmd( ptr );
      }
    }
    else if(sib7info.sib_timer_update_req == TRUE)
    {
      /* Timer based update of SIB7 has been completed. */
      sib7info.sib_timer_update_req = FALSE;
#ifdef FEATURE_UPDATE_SIB7_IN_FACH
      if(rrc_get_state() == RRC_STATE_CELL_FACH)
      {/*Lets inti int_event_type to max as it will be used by 
          SIB-7 event in FACH*/
        sib_events_ptr->event[index].int_event.int_event_type 
           = RRCSIB_INT_MAX_EVENT;
      }
#endif
    }

    /* Check if there is any pending event for updating sibs
    and if so, process that event. */
    if( update_sibs_event_pending == TRUE )
    {
      /* Post a command to ourselves for updating the SIBs */
      rrcsib_send_command_for_pending_update_sibs_event();
      WRRC_MSG0_HIGH("Pending UPDATE_SIBS event activated");
      update_sibs_event_pending = FALSE;
    }
  }
}

/*===========================================================================

FUNCTION RRCSIB_SEND_SIB_SLEEP_TABLE

DESCRIPTION

  This function sends the SIB sleep table to L1 so that
  L1 will sleep for a while before receiving the SIBs.

  This function will be called when SIB event is completed in both success and failure cases and
  also when a SIB is received to send the updated sleep table.
  

DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
   
===========================================================================*/


static void  rrcsib_send_sib_sleep_table(uint8 *table_ptr)
{
  rrc_cmd_type * cmd_ptr = NULL;
  l1_req_cmd * rrc_cmd_l1_req = NULL;
  
  /* Now we have the sched table updated.Send the change_ind to L1 */
  cmd_ptr = rrc_get_int_cmd_buf();
  
  RRC_GET_POINTER_L1_REQ_VALUE(cmd_ptr->cmd.rrc_llc_req.l1_req,rrc_cmd_l1_req);
  
  cmd_ptr->cmd_hdr.cmd_id= RRC_LLC_CMD_REQ;
  cmd_ptr->cmd.rrc_llc_req.procedure = RRC_PROCEDURE_SIB;
  cmd_ptr->cmd.rrc_llc_req.cmd_dest = LAYER1_PHY;
  cmd_ptr->cmd.rrc_llc_req.l1_cmd.cmd_id = CPHY_SIB_SCHED_CHANGE_IND;
  
  rrc_cmd_l1_req->sib_sched_change_ind_req.sib_sfn_bit_mask = table_ptr;
  rrc_put_int_cmd( cmd_ptr );
  WRRC_MSG0_HIGH("SIB_Sleep:CPHY_SIB_SCHED_CHANGE_IND sent to L1");
}

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION RRCSIB_INTRO_HOLES_IN_TABLE

DESCRIPTION

  This function sends the SIB sleep table to L1 so that
  L1 will sleep for a while before receiving the SIBs.

  This function will be called when SIB event is completed in both success and failure cases and
  also when a SIB is received to send the updated sleep table.
  

DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
   
===========================================================================*/
static void  rrcsib_intro_holes_in_table(uint8 *table_ptr,uint32 sleep_sfn, uint32 wake_sfn)
{

  uint8 table_mask[RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE*8],sum;
  uint16 i=0,j,k;

  WRRC_MSG1_HIGH("RRC_G2W: sib pattern is %d",rrcsib_pattern_invert);

  /*Generate a table with sleep_sfn wake_sfn pattern*/
  while(i < RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE*8)
  {
    for(j=i; ((j<(i+sleep_sfn))&&(j < RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE*8)); j++)
    {
      table_mask[j]=rrcsib_pattern_invert;
    }

    for(k = j; ((k<(j+wake_sfn))&&(k < RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE*8)); k++)
    {
      table_mask[k]=!rrcsib_pattern_invert;
    }
    i=k;
  }

  /*and the generated table with table to be masked*/
  for(i=0;i<RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE;i++)
  {
    sum = 0;
    k=128;
    for(j=(i*8); j<((i*8)+8); j++)
    {
      sum = sum + (table_mask[j] * k);
      k = k/2;
    }
    table_ptr[i] &= sum;
  }

}
#endif

/*===========================================================================

FUNCTION RRCSIB_POST_PROCESS_FAILED_EVENT

DESCRIPTION

  This function does the necessary processing for
  a failed SIB event. It sends the failure command
  to the calling procedure if requested, and clears 
  the necessary variables.
  
  In case this is a neighbor BCCH event, it also
  tears down the neighbor BCCH pipe if requested
  by the calling function.
  
  In case of serving BCCH events required by other
  procedures, the BCCH is not torn down by
  this module since the PCCPCH needs to be up for
  timing purposes till the SCCPCH is setup.
  However for events requested by this procedure
  itself and for RRCSIB_UPDATE_SIBS event, this
  function tears down the BCCH. This is done only
  if requested by the calling procedure. 

DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
   
===========================================================================*/
static void rrcsib_post_process_failed_event
( 
  sib_events_index_e_type event_index, /* Index of the event that failed */
  boolean cnf_cmd_required, /* Indicate if a confirmation command is
                            to be sent.                             */
  rrc_sib_status_e_type failure_cause, /* Cause of failure. Only needed
                                         if CNF command is to be sent. */
  boolean sbcch_teardown_required   /* If the event is for Serving BCCH
                                       indicate if the serving BCCH pipe should be
                                       brought down. */
)
{
  rrc_cmd_type *cnf_ptr;      /* For sending confirmation */
  uint32  dbindex;
  rrc_state_e_type rrc_state;
  rrc_plmn_identity_type plmn;
  /*Variable used to hold config reason*/
  rrcllc_chan_cfg_reason_e_type chan_config_reason = RRCLLC_CHAN_CFG_REASON_NONE;
  rrc_cmd_type *int_cmd_ptr = NULL; /* For cell update/cell selection if needed */

  if(EVENT_INDEX_VALID(event_index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return;
  }

  /* Copy the PLMN id */
  if((sib_events_ptr->event[event_index].tempholder.mib != NULL)&&
     ( RRC_CHECK_COMMON_MSG_TYPE(sib_events_ptr->event[event_index].tempholder.mib->plmn_Type, 
                rrc_PLMN_Type_gsm_MAP )))
  {
    rrc_translate_gsm_map_plmn_id
                 (&(sib_events_ptr->event[event_index].tempholder.mib->plmn_Type.u.gsm_MAP->plmn_Identity), &plmn);
  }
  else
  {
    plmn.mcc[0] = plmn.mcc[1] = plmn.mcc[2] =0;
    plmn.mnc[0] = plmn.mnc[1] = plmn.mnc[2] =0;
    plmn.num_mnc_digits = 0;
  }

  if(rrc_sib_is_current_event_sib_read_in_dch() == TRUE)
  {
    /*If the SIB event active was cell id reading in DCH then set chan 
     *config reason to RRCLLC_CHAN_CFG_REASON_SIB_READ_IN_DCH 
     *which is used by LLC
     */
    chan_config_reason = RRCLLC_CHAN_CFG_REASON_SIB_READ_IN_DCH;
  }

  /* Make the event inactive */
  sib_events_ptr->event[event_index].event_active = FALSE;

#ifdef FEATURE_UPDATE_SIB7_IN_FACH
  if(rrctmr_get_remaining_time(RRCTMR_SIB_WAIT_TIMER) != (uint64)0)
#endif
  {
    /* Stop the SIB wait timer in case it's running. */
    rrctmr_stop_timer( RRCTMR_SIB_WAIT_TIMER );
#ifdef FEATURE_DUAL_SIM
    rrctmr_stop_timer(RRCTMR_SIB_STOP_PARTITION_TIMER);
    rrctmr_stop_timer(RRCTMR_SIB_MASK_INVERT_TIMER);
    rrctmr_stop_timer(RRCTMR_SIB_EMPTY_MASK_TIMER);
#endif
  }

  if ((sib_events_ptr->event[event_index].event_name == RRCSIB_GET_SPECIFIC_SIBS) &&
      (sib_events_ptr->event[event_index].req_proc == RRC_PROCEDURE_CSP)) 
  {
    if (sib_events_ptr->event[event_index].sib_db_index_found == TRUE)
    {
      dbindex = sib_events_ptr->event[event_index].sib_db_index;
      
     if((dbindex != SIBDB_INVALID_INDEX)&&
        (sibdb.active_cell_sibs_valid == TRUE)&&
        (dbindex == sibdb.active_cell_index))
      {
        WRRC_MSG1_HIGH("Since dbindex = %d is active_cell_index, skip clear of db idx",dbindex);
      }
      else if((dbindex != SIBDB_INVALID_INDEX)&&(!sib_events_ptr->event[event_index].is_mib_vtag_match))
      {
        rrc_sibdb_clear_a_cell_index(dbindex);
      }
    }
  }
  
  /* In case the tempholer pointers are not freed, free them
  now */
  rrcsib_free_tsib(event_index);
  
  /* Depending on the event a confirmation command
  may need to be sent. Do this only if the calling
  function requests it. */
  switch( sib_events_ptr->event[event_index].event_name )
  { 
    case RRCSIB_GET_SPECIFIC_SIBS:

      /* If the GET_SPECIFIC_SIBS command was
      requested by an external procedure we have
      to send a GET_SPECIFIC_SIBS_CNF. If it was
      requested by the SIB procedure itself, we
      have to process it separately. */
      if( sib_events_ptr->event[event_index].req_proc
          != RRC_PROCEDURE_SIB )
      {
        if( cnf_cmd_required == TRUE )
        {
          if( (cnf_ptr = rrc_get_int_cmd_buf())!=NULL )
          {
            WRRC_MSG0_HIGH("Sending GET_SPECIFIC_SIBS_CNF(failed)");
            cnf_ptr->cmd_hdr.cmd_id = RRC_GET_SPECIFIC_SIBS_CNF;
            cnf_ptr->cmd.get_sibs_cnf.status = failure_cause;
            cnf_ptr->cmd.get_sibs_cnf.procedure = 
              sib_events_ptr->event[event_index].req_proc;
            /*Boolean to indicate whether SIB read over OTA or through the 
                      SYS_INFO_CONTAINER*/
#ifdef FEATURE_3GPP_CSFB
            if(sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_READ_FROM_SIB_CONTAINER)
            {
              cnf_ptr->cmd.get_sibs_cnf.is_sib_read_from_container = TRUE;
            }
            else
            {
              cnf_ptr->cmd.get_sibs_cnf.is_sib_read_from_container = FALSE;
            }
#endif            
            /* Copy the frequency and Scrambling code into the
            confirmation command. For GET_SPECIFIC_SIBS command, 
            check if it's the serving cell or neigbor cell. */
            if(sib_events_ptr->event[event_index].for_serving_cell == TRUE)
            {
              cnf_ptr->cmd.get_sibs_cnf.freq = sib_events_ptr->srv_bcch.freq;
              cnf_ptr->cmd.get_sibs_cnf.scrambling_code = sib_events_ptr->srv_bcch.scr_code;

              if(failure_cause == SIB_EVENT_FAILED_FPLMN)
              {
                cnf_ptr->cmd.get_sibs_cnf.plmn_id = plmn;
                cnf_ptr->cmd.get_sibs_cnf.cell_id = 0;
#ifdef FEATURE_FEMTO_CSG
                cnf_ptr->cmd.get_sibs_cnf.csg_id = sib_events_ptr->event[event_index].csg_id;
                cnf_ptr->cmd.get_sibs_cnf.is_csg = sib_events_ptr->event[event_index].is_csg;
#endif
              }
            }
            else
            {
              cnf_ptr->cmd.get_sibs_cnf.freq = sib_events_ptr->ngh_bcch.freq;
              cnf_ptr->cmd.get_sibs_cnf.scrambling_code = sib_events_ptr->ngh_bcch.scr_code;
#ifdef FEATURE_FEMTO_CSG
              cnf_ptr->cmd.get_sibs_cnf.is_csg = sib_events_ptr->event[event_index].is_csg;
              cnf_ptr->cmd.get_sibs_cnf.csg_id = sib_events_ptr->event[event_index].csg_id;
#endif
            }
            rrc_put_int_cmd( cnf_ptr );
          }
        }
      }
      else
      {
        rrcsib_process_int_sib_event_complete( event_index, SUCCESS );
      }
      break;

    case RRCSIB_UPDATE_SIBS:

      /* Clear the saved SIB change indications, if any.
       This is only used by UPDATE_SIBS events. */
      rrc_sibdb_clear_active_cell_sib_change();

      /* If the UPDATE_SIBS command was
      requested by an external procedure there
      is no confirmation required. If it was
      requested by the SIB procedure itself, we
      have to process it separately. */
      if( sib_events_ptr->event[event_index].req_proc
          != RRC_PROCEDURE_SIB )
      {
        WRRC_MSG0_HIGH("RRC_UPDATE_SIBS_IND failed");
      }
      else
      {
        rrcsib_process_int_sib_event_complete
          ( event_index, FAILURE );
      }

      /* Failure handling for Update SIBS Event:
         If we had failed to read SIBS during Update SIBS ind,we need to trigger
         cell selection/Out of service area to read the SIBS again.Using the
         stale sibs is not good */    
      if((SIB_EVENT_FAILED_LOWER_LAYERS == failure_cause) ||
         (SIB_EVENT_FAILED_TIMEOUT == failure_cause)
        )
      { 
         WRRC_MSG1_HIGH("RRC Update SIBS event failed,failure cause:%d.Failure handling kicking in",failure_cause);
         rrc_state = rrc_get_state();
         switch(rrc_state)
         {
           case RRC_STATE_CELL_PCH:
           case RRC_STATE_URA_PCH:
           case RRC_STATE_CELL_FACH:
             /* Trigger Out Of Service area */
             if( (int_cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
             { 
               int_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_UPDATE_INITIATE_REQ;
               int_cmd_ptr->cmd.initiate_cell_update.cause = RRC_REENTERED_SERVICE_AREA;
               int_cmd_ptr->cmd.initiate_cell_update.cu_complete_cnf_required = FALSE;
               int_cmd_ptr->cmd.initiate_cell_update.procedure = RRC_PROCEDURE_SIB;
               rrc_put_int_cmd(int_cmd_ptr); 
               WRRC_MSG1_HIGH("Sending Cell Update Initiate Req,cause Re-entered Service Area,rrc_state:%d",rrc_state);
             }
             break;

           case RRC_STATE_DISCONNECTED:
             /* Trigger Cell Selection */
             if( (int_cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
             { 
               int_cmd_ptr->cmd_hdr.cmd_id = RRC_INITIATE_CELL_SELECTION_IND;
               int_cmd_ptr->cmd.initiate_cell_sel_ind.cause = RRC_OUT_OF_SERVICE_AREA_IND;
               int_cmd_ptr->cmd.initiate_cell_sel_ind.proc = RRC_PROCEDURE_SIB;
               int_cmd_ptr->cmd.initiate_cell_sel_ind.dest_state = rrc_state;
               int_cmd_ptr->cmd.initiate_cell_sel_ind.dest_freq_present = FALSE;
               int_cmd_ptr->cmd.initiate_cell_sel_ind.dest_psc_present  = FALSE;
               rrc_put_int_cmd(int_cmd_ptr); 
               WRRC_MSG1_HIGH("Sending Initiate Cell Selection Ind to CSP,cause Out Of Service Ind.rrc_state:%d",rrc_state);
             }
             break;

           default:
             WRRC_MSG0_HIGH("Invalid state,no action taken on S-BCCH failure");
             break;
         }
      }
      break;

    default:

      WRRC_MSG1_ERROR("Unknown SIB event %d", sib_events_ptr->event[event_index].event_name);
      break;
  }/* End switch */


  /* Check if a S-BCCH teardown is required. S-BCCH is torn down
  for a RRCSIB_UPDATE_SIBS command and for any command initiated
  by the SIB procedure itself or request was GET_SPECIFIC_SIBS for serving
  cell*/
  if( (
       (sib_events_ptr->event[event_index].event_name == RRCSIB_UPDATE_SIBS) ||
       (sib_events_ptr->event[event_index].req_proc == RRC_PROCEDURE_SIB)
      )       
      ||      
      (
       ( sib_events_ptr->event[event_index].event_name == RRCSIB_GET_SPECIFIC_SIBS ) &&
       ( sib_events_ptr->event[event_index].for_serving_cell == TRUE )
      )
    )
  {
#ifdef FEATURE_UPDATE_SIB7_IN_FACH
    if(
#ifdef FEATURE_WCDMA_HS_RACH
       (rrcsib_update_sib7_hs_rach() == TRUE) &&
#endif
       ((rrc_get_state() == RRC_STATE_CELL_FACH)||
       (OC_SET_FOR_CELL_FACH == rrcllc_get_ordered_config_status()))
      )
    {
      WRRC_MSG2_HIGH("sib7:RRC state is FACH,Dont tear down S-BCCH, for event =%d, req proc= rrc_proc_e_type_value%d",
             sib_events_ptr->event[event_index].event_name,sib_events_ptr->event[event_index].req_proc);
    }
    else
#endif
#ifdef FEATURE_UPDATE_SIB7_FOR_PCH_TO_FACH
    if(
#ifdef FEATURE_WCDMA_HS_RACH
       (rrcsib_update_sib7_hs_rach() == TRUE)&&
#endif
       ((rrc_get_state() == RRC_STATE_CELL_PCH)||(rrc_get_state() == RRC_STATE_URA_PCH))&&
       (sib_events_ptr->event[SIB_SRV_INDEX].int_event.int_event_type == RRCSIB_INT_SIB7_EVENT)&&
       ((rrcllc_get_ordered_config_status() != OC_SET_FOR_FACH_CELL_PCH_TRANS)&&
       (rrcllc_get_ordered_config_status() != OC_SET_FOR_FACH_URA_PCH_TRANS))
       )
    {
      WRRC_MSG2_HIGH("SIB7 read in PCH,Dont tear down S-BCCH, for event =%d, req proc=rrc_proc_e_type_value%d",
             sib_events_ptr->event[event_index].event_name,sib_events_ptr->event[event_index].req_proc);
    }
    else
#endif
    {
      /* Check if calling procedure wants the BCCH to be torn down.
      In certain conditions the BCCH would already have been taken 
      down by LLC hence it may not be required. */
      if(( sbcch_teardown_required == TRUE ) && (sib_events_ptr->event[event_index].for_serving_cell))
      {
        //teardown S_BCCH, don't change neighbor, and don't request CNF for 
        //S_BCCH setup
        (void)rrcsib_configure_bcch(SBCCH_TEARDOWN, NBCCH_NOOP
                                 ,chan_config_reason);
      }
    }
  }

#ifdef FEATURE_RRC_NO_MIB_FOUND_BAR_CELL
  no_mib_bar_cell_count = 0;
#endif

#ifdef FEATURE_UPDATE_SIB7_IN_FACH
#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
  REX_ISR_LOCK(&sib7_in_fach_active_mutex);
  if(is_sib7_in_fach_active == TRUE)
  {
    is_sib7_in_fach_active = FALSE;
  }
  REX_ISR_UNLOCK(&sib7_in_fach_active_mutex);
  WRRC_MSG1_HIGH("SIB7 in cell fach event active is set to %d",is_sib7_in_fach_active);
#endif
#endif

  //after a failed event, reset BCCH action to false always..
  sib_events_ptr->srv_bcch.bcch_action = SIB_BCCH_NO_ACTION;
  sib_events_ptr->ngh_bcch.bcch_action = SIB_BCCH_NO_ACTION;
  sib_events_ptr->event[event_index].sib_scan_type = RRC_SIB_NORMAL;
}/* End function rrcsib_post_process_sib_failed_event */


  
/*===========================================================================

FUNCTION rrcsib_invalidate_sib_scheduling_info

DESCRIPTION


DEPENDENCIES
 
  None.

RETURN VALUE
 
  None.

SIDE EFFECTS

  None.
        
===========================================================================*/
static void rrcsib_invalidate_sib_scheduling_info
(
  sib_events_index_e_type index /* Index in the SIB event database */  
)
{
  uint32 count=0;

  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return;
  }
  
  //reset the flags that checks to see if all SIB's are received  
  memset (sib_events_ptr->sib_sched, FALSE, RRC_TOT_SIBS * sizeof(sib_scheduling_info));

  for(count=0; count<RRC_TOT_SIBS; count++)
  {
    sib_events_ptr->sib_sched[count].repitition_rate = INVALID_SIB_SCHEDULE_REP;
    sib_events_ptr->sib_sched[count].num_segments = 0;
  }

  //reset the flags that checks to see if all SIB's are received  
  memset (sib_events_ptr->event[index].sched_present_for_sibs, FALSE, RRC_TOT_SIBS * sizeof(uint8));

  //reset the flag's which keep trakc of some 
  sib_events_ptr->scheduling_max_rept_rate = 0;
  sib_events_ptr->scheduling_updated_by_sb1 = FALSE;
  sib_events_ptr->scheduling_updated_by_sb2 = FALSE;

  //reset flags that indicate whether SB's are needed and decoded yet
  sib_events_ptr->event[index].tempholder.sb1_decoded=FALSE;
  sib_events_ptr->event[index].tempholder.sb2_decoded=FALSE;
}  
/*===========================================================================

FUNCTION rrcsib_update_pos_offset_in_sched_info

DESCRIPTION
  This function updates the position offset of individual sibs based on
  scheduling information received in MIB/SB1/SB2

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Updates sib_events database with scheduling information for eacy sib
        
===========================================================================*/
static void rrcsib_update_pos_offset_in_sched_info
(
  rrc_SIB_type                rrc_sib_type,       //sib type
  rrc_SchedulingInformation * scheduling_info //scheduling information
)
{

  rrc_SibOFF_List *pos_offset_ptr;  //local ptr for looping thru pos_offset
  uint32 count=0;                  //counter purposes
  uint32 index=0, n_value=0;

#ifdef FEATURE_RRC_SIB_SUPPORT_MAX_REPT_RATE
  int32  diff_timer_value = 0; 
  uint32 timer_value_pending = 0, timer_value_exhausted = 0;
#endif

  if(rrc_sib_type >= rrc_ExtensionType)
  {
    WRRC_MSG0_ERROR("Invalid SIB Type");
    return ;
  }

  //get scheduling information about rep-rate and initial_segment_offset
  //rep-rate is given in steps of 2, left shift by 1 to multiply by 2 to
  //get the real SFN.. we do comparison later on the real SFN, hence do 
  //the comparison here to make life easy to do a simple comparison later
  switch(RRCGET_SCHD_INFO_TAG((&scheduling_info->scheduling.sib_Pos)))
  {
    case RRCGET_SCHD_INFO_SIB_POS(rep4):
      sib_events_ptr->sib_sched[rrc_sib_type].repitition_rate = 4;
      sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] =
        (uint32)(scheduling_info->scheduling.sib_Pos.u.rep4 << 1);
      break;

    case RRCGET_SCHD_INFO_SIB_POS(rep8):
      sib_events_ptr->sib_sched[rrc_sib_type].repitition_rate = 8;
      sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] =
        (uint32)(scheduling_info->scheduling.sib_Pos.u.rep8 << 1);
      break;

    case RRCGET_SCHD_INFO_SIB_POS(rep16):
      sib_events_ptr->sib_sched[rrc_sib_type].repitition_rate = 16;
      sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] =
        (uint32)(scheduling_info->scheduling.sib_Pos.u.rep16 << 1);
      break;

    case RRCGET_SCHD_INFO_SIB_POS(rep32):
      sib_events_ptr->sib_sched[rrc_sib_type].repitition_rate = 32;
      sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] =
        (uint32)(scheduling_info->scheduling.sib_Pos.u.rep32 << 1);
      break;

    case RRCGET_SCHD_INFO_SIB_POS(rep64):
      sib_events_ptr->sib_sched[rrc_sib_type].repitition_rate = 64;
      sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] =
        (uint32)(scheduling_info->scheduling.sib_Pos.u.rep64 << 1);
      break;

    case RRCGET_SCHD_INFO_SIB_POS(rep128):
      sib_events_ptr->sib_sched[rrc_sib_type].repitition_rate = 128;
      sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] =
        (uint32)(scheduling_info->scheduling.sib_Pos.u.rep128 << 1);
      break;

    case RRCGET_SCHD_INFO_SIB_POS(rep256):
      sib_events_ptr->sib_sched[rrc_sib_type].repitition_rate = 256;
      sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] =
        (uint32)(scheduling_info->scheduling.sib_Pos.u.rep256 << 1);
      break;

    case RRCGET_SCHD_INFO_SIB_POS(rep512):
      sib_events_ptr->sib_sched[rrc_sib_type].repitition_rate = 512;
      sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] =
        (uint32)(scheduling_info->scheduling.sib_Pos.u.rep512 << 1);
      break;

    case RRCGET_SCHD_INFO_SIB_POS(rep1024):
      sib_events_ptr->sib_sched[rrc_sib_type].repitition_rate = 1024;
      sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] =
        (uint32)(scheduling_info->scheduling.sib_Pos.u.rep1024 << 1);
      break;

    case RRCGET_SCHD_INFO_SIB_POS(rep2048):
      sib_events_ptr->sib_sched[rrc_sib_type].repitition_rate = 2048;
      sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] =
        (uint32)(scheduling_info->scheduling.sib_Pos.u.rep2048 << 1);
      break;

    case RRCGET_SCHD_INFO_SIB_POS(rep4096):
      sib_events_ptr->sib_sched[rrc_sib_type].repitition_rate = 4096;
      sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] =
        (uint32)(scheduling_info->scheduling.sib_Pos.u.rep4096 << 1);
      break;

    default:
      WRRC_MSG1_ERROR("Invalid sib-rep-pos info %d",RRCGET_SCHD_INFO_TAG((&scheduling_info->scheduling.sib_Pos)));
      break;
  }

  //update the max rept rate if it is lower than the last received one
  if(sib_events_ptr->scheduling_max_rept_rate < sib_events_ptr->sib_sched[rrc_sib_type].repitition_rate)
  {
    sib_events_ptr->scheduling_max_rept_rate = sib_events_ptr->sib_sched[rrc_sib_type].repitition_rate;

#ifdef FEATURE_RRC_SIB_SUPPORT_MAX_REPT_RATE
    if((sib_events_ptr->event[SIB_SRV_INDEX].for_serving_cell == TRUE) &&
       (sib_events_ptr->event[SIB_SRV_INDEX].event_name       == RRCSIB_GET_SPECIFIC_SIBS) &&
       (sib_events_ptr->event[SIB_SRV_INDEX].sib_scan_type    == RRC_SIB_NORMAL) &&
       (sib_events_ptr->event[SIB_SRV_INDEX].req_proc         == RRC_PROCEDURE_CSP))
    {
      if((sib_events_ptr->scheduling_max_rept_rate > max_sib_rept_rate) &&
         (sib_events_ptr->scheduling_max_rept_rate > RRC_SIB_MAX_REP_RATE_SIB_WAIT_TIMER))
      {
        max_sib_rept_rate = sib_events_ptr->scheduling_max_rept_rate;

        timer_value_pending   = (uint32)rrctmr_get_remaining_time(RRCTMR_SIB_WAIT_TIMER);
        timer_value_exhausted = timer_value_started - timer_value_pending;

        /* Deducting the time that has already been exhausted */
        diff_timer_value = (max_sib_rept_rate * RRC_RADIO_FRAME_TO_MILLISECOND_CONV) - timer_value_exhausted;

        if(diff_timer_value > (int32)timer_value_pending)
        {
          rrctmr_stop_timer( RRCTMR_SIB_WAIT_TIMER );

	      timer_value_started = diff_timer_value;

          WRRC_MSG1_HIGH("Start SIB_WAIT timer for S_BCCH max_rep_rate %d", max_sib_rept_rate);
          rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER,(unsigned long) (diff_timer_value));
        }
      }
    }
#endif
  }

  //check if segment count is present.. then set it.. if not present it is 1
  //as per spec 25.331
  // bitmask rrc_segCount_present is not present in Objective Systems implementation
  if(scheduling_info->scheduling.segCount > 0)
  {
    sib_events_ptr->sib_sched[rrc_sib_type].num_segments = 
      (uint8)scheduling_info->scheduling.segCount;
  }
  else
  {
    sib_events_ptr->sib_sched[rrc_sib_type].num_segments = 1;
  }

  //if number of segments is > 1, then we need scheduling info for all the
  //subsequent segments..  this may be indicated in posOffsetInfo or we
  //could use the default offset of 2 for all segments no a multiple of 8..
  //segments which r a multiple of 8 are reserved for mib, or have to be
  //explicitly signalled in posOffsetInfo..
  if(sib_events_ptr->sib_sched[rrc_sib_type].num_segments > 1)
  {
    if(RRCCHECK_SIB_MSG_IE_PRESENT((&scheduling_info->scheduling),
                                rrc_,
                                sib_PosOffsetInfo))
    {
      pos_offset_ptr = &(scheduling_info->scheduling.sib_PosOffsetInfo);

      //counter is set to 1, as 0 is deemed to be the first segment..
      //below we are only dealing with subsequent segments
      count = 1;
      index = 0;
      n_value = pos_offset_ptr->n;

      //loop thru the offset ptr and initialize the sib_events db
      while(n_value != 0)
      {
        switch(pos_offset_ptr->elem[index])
        {
          case RRCGET_SIB_OFF_VALUE(so2):
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] = 2;    
            break;
          case RRCGET_SIB_OFF_VALUE(so4):
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] = 4;    
            break;
          case RRCGET_SIB_OFF_VALUE(so6):
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] = 6;    
            break;
          case RRCGET_SIB_OFF_VALUE(so8):
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] = 8;    
            break;
          case RRCGET_SIB_OFF_VALUE(so10):
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] = 10;    
            break;
          case RRCGET_SIB_OFF_VALUE(so12):
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] = 12;    
            break;
          case RRCGET_SIB_OFF_VALUE(so14):
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] = 14;    
            break;
          case RRCGET_SIB_OFF_VALUE(so16):
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] = 16;    
            break;
          case RRCGET_SIB_OFF_VALUE(so18):
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] = 18;    
            break;
          case RRCGET_SIB_OFF_VALUE(so20):
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] = 20;    
            break;
          case RRCGET_SIB_OFF_VALUE(so22):
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] = 22;    
            break;
          case RRCGET_SIB_OFF_VALUE(so24):
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] = 24;    
            break;
          case RRCGET_SIB_OFF_VALUE(so26):
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] = 26;    
            break;
          case RRCGET_SIB_OFF_VALUE(so28):
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] = 28;    
            break;
          case RRCGET_SIB_OFF_VALUE(so30):
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] = 30;    
            break;
          case RRCGET_SIB_OFF_VALUE(so32):
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] = 32;    
            break;
          default:
            WRRC_MSG1_ERROR("Invalid Seg-Offset %d", pos_offset_ptr->elem[index]);
            break;
        }
        
        //if the number of segments for which pos-offset is received is > than the
        //number of segments indicated in MIB,  print an error and don't update 
        //offset info..  we should not camp on a cell with bad position-offset count
        if((count+1) > sib_events_ptr->sib_sched[rrc_sib_type].num_segments)
        {
          WRRC_MSG0_ERROR("More Pos-Offsets than Segments.  Invalid SIB-Sched");
          break;
        }
        else
        {
          //calculate the segment offset according to the absolute offset from
          //the first segment..  this is done to make life easier during comparison
          //of scheduling info later
          sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] = 
          sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] +
          sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count-1];

          if(sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] > 4096)
          {
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count]%=4096;
          }
          count++;
          index++;
          n_value--;
        }
      }
    }
    else
    {
      //loop thru the rest of the segments and set the offset to 2 by default
      for(count=1; (count < sib_events_ptr->sib_sched[rrc_sib_type].num_segments) &&(count <= MAX_SEG_COUNT_FOR_A_SIB ); count++)
      {
        if(((sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count-1] + 2) & 0x7) == 0)
        {
          sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] = 
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count-1] + 4;

          if(sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] > 4096)
          {
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count]%=4096;
          }
        }
        else
        {
          sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] = 
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count-1] + 2;

          if(sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count] > 4096)
          {
            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[count]%=4096;
          }
        }
      }
    }
  }

  WRRC_MSG6_MED( 
           "Num. Seg for SIB %d is %d rep_rate %d SEG-Offsets is %d, %d, %d", 
           rrcsib_map_int_sibtype_to_asn1_sibtype(rrc_sib_type), 
           sib_events_ptr->sib_sched[rrc_sib_type].num_segments,
           sib_events_ptr->sib_sched[rrc_sib_type].repitition_rate,
           sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[0],
           sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[1],
           sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[2]
           );
}


/*===========================================================================

FUNCTION rrcsib_check_if_sched_info_match

DESCRIPTION
  This function is used to check whether the SIB segments stored in TSIB database were received in proper offsets
  as per the scheduling information in SB1/SB2 or not

DEPENDENCIES
 
  None.

RETURN VALUE
 
  SEG_PROC_COMPLETE : 
  If all the segments of a SIB has been received before SB1/SB2 and the concatenation was 
  successful

  SEG_PROC_NONE :
  If otherwise

SIDE EFFECTS
  None
===========================================================================*/
static seg_proc_stat_e_type rrcsib_check_if_sched_info_match
(
  rrc_SIB_type rrc_sib_type,
  sib_events_index_e_type  index
)
{
  uint8 i = 0, j=0;
  uint32 sfn_offset = 0;
  boolean valid_sib = FALSE;
  uint8 num_seg=0;
  seg_proc_stat_e_type status = SEG_PROC_NONE;

  if(rrc_sib_type >= rrc_ExtensionType)
  {
    WRRC_MSG0_ERROR("Invalid SIB Type");
    return SEG_PROC_FAILED;
  }

  WRRC_MSG1_HIGH("Checking TSIB DB for SIB %d",rrcsib_map_int_sibtype_to_asn1_sibtype(rrc_sib_type));
  for(i = 0;i<MAX_TEMP_SIB_SEGMENTS_TO_HOLD;i++)
  {
    if(tsib_optimization_db[i].index_in_use == TRUE && tsib_optimization_db[i].sib_type == rrc_sib_type)
    {
      valid_sib = FALSE;
      sfn_offset = tsib_optimization_db[i].sfn_prime<<1;
      if(tsib_optimization_db[i].seg_index < sib_events_ptr->sib_sched[rrc_sib_type].num_segments)
      {
        for(j=0;j<sib_events_ptr->sib_sched[rrc_sib_type].num_segments;j++)
        {
          if((sfn_offset % sib_events_ptr->sib_sched[rrc_sib_type].repitition_rate) == 
                            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[j])
          {
            num_seg++;
            valid_sib = TRUE;
            break;
          }
        }
        if(valid_sib == FALSE)
        {
          WRRC_MSG2_HIGH("Seg %d of SIB %d was received at wrong SFN",tsib_optimization_db[i].seg_index,
                           rrcsib_map_int_sibtype_to_asn1_sibtype(rrc_sib_type));
          tsib_optimization_db[i].index_in_use = FALSE;
          tsib_optimization_db[i].seg_data_length = 0;
          tsib_optimization_db[i].seg_index = 0;
          tsib_optimization_db[i].sfn_prime = 0;
          tsib_optimization_db[i].sib_seg_type = noSegment_chosen;
  
          tsib_optimization_db[i].sib_type = rrc_MIB;
          memset(tsib_optimization_db[i].seg_data_ptr,0x00, FIXED_SIB_DATA_LENGTH_BYTES);
        }
      }
      else
      {
        MSG_HIGH("Segment Index greater than number of segments. Invalid segment index %d for SIB %d. Total number of segments = %d",
                          tsib_optimization_db[i].seg_index,rrcsib_map_int_sibtype_to_asn1_sibtype(rrc_sib_type)
                          ,sib_events_ptr->sib_sched[rrc_sib_type].num_segments);
        tsib_optimization_db[i].index_in_use = FALSE;
        tsib_optimization_db[i].seg_data_length = 0;
        tsib_optimization_db[i].seg_index = 0;
        tsib_optimization_db[i].sfn_prime = 0;
        tsib_optimization_db[i].sib_seg_type = noSegment_chosen;

        tsib_optimization_db[i].sib_type = rrc_MIB;
        memset(tsib_optimization_db[i].seg_data_ptr,0x00, FIXED_SIB_DATA_LENGTH_BYTES);
      }
    }
  }

  if(num_seg <= sib_events_ptr->sib_sched[rrc_sib_type].num_segments)
  {
    WRRC_MSG1_HIGH("Concatenating segments for SIB # %d received before SB",rrcsib_map_int_sibtype_to_asn1_sibtype(rrc_sib_type));
    status = rrcsib_process_sib_seg_in_tsib_optim_db(rrc_sib_type,index);
  }

  return status;
  
}

/*===========================================================================

FUNCTION rrcsib_check_if_sched_info_match_for_ext_sibs

DESCRIPTION
  This function is used to check whether the SIB segments stored for extension SIBs 
  in TSIB database were received in proper offsets as per the scheduling information 
  in MIB/SB1/SB2 or not.

DEPENDENCIES
 
  None.

RETURN VALUE
 
  SEG_PROC_COMPLETE : 
  If all the segments of a SIB has been received before SB1/SB2 and the concatenation was 
  successful

  SEG_PROC_NONE :
  If otherwise

SIDE EFFECTS
  None
===========================================================================*/
static seg_proc_stat_e_type rrcsib_check_if_sched_info_match_for_ext_sibs
(
  rrc_SIB_type rrc_sib_type,
  sib_events_index_e_type  index
)
{
  uint8 i = 0, j=0;
  uint32 sfn_offset = 0;
  uint8 num_seg=0;
  seg_proc_stat_e_type status = SEG_PROC_NONE;

  WRRC_MSG0_HIGH("Checking TSIB DB for extension SIB");
  for(i = 0;i < MAX_TEMP_SIB_SEGMENTS_TO_HOLD;i++)
  {
    if(tsib_optimization_db[i].index_in_use == TRUE && tsib_optimization_db[i].sib_type == rrc_ExtensionType)
    {
      sfn_offset = tsib_optimization_db[i].sfn_prime<<1;
      if(tsib_optimization_db[i].seg_index < sib_events_ptr->sib_sched[rrc_sib_type].num_segments)
      {
        for(j=0;(j<sib_events_ptr->sib_sched[rrc_sib_type].num_segments) && (j < MAX_SEGMENTS_FOR_A_SIB);j++)
        {
          if((sfn_offset % sib_events_ptr->sib_sched[rrc_sib_type].repitition_rate) == 
                            sib_events_ptr->sib_sched[rrc_sib_type].sfn_offset[j])
          {
            num_seg++;
            tsib_optimization_db[i].sib_type = rrc_sib_type;
            break;
          }
        }
      }
    }
  }

  if(num_seg <= sib_events_ptr->sib_sched[rrc_sib_type].num_segments)
  {
    tsib_opt_db_look_up_table[rrc_sib_type] = TRUE;
    WRRC_MSG1_HIGH("Concatenating segments for ext_sib %d received before SB",rrc_sib_type);
    status = rrcsib_process_sib_seg_in_tsib_optim_db(rrc_sib_type,index);
  }

  return status;
  
}



/*===========================================================================

FUNCTION rrcsib_update_sched_info_from_mib_sb

DESCRIPTION
  Updates scheduling information based on MIB/SB1/SB2 received OTA

DEPENDENCIES
 
  None.

RETURN VALUE
 
  None.

SIDE EFFECTS
  Updates the function to update scheduling information, which updates the
  sib_events database based on scheduling information received.        
===========================================================================*/
void rrcsib_update_sched_info_from_mib_sb
(
  uint32 pdu_num,    /* PDU number defined by ASN.1 for this SIB */
  void *sib_ptr,    /* Pointer to the SIB typecast to void.     */
  sib_events_index_e_type  event_index
)
{
  rrc_MasterInformationBlock *mib_ptr; /* Local pointer to MIB       */
  rrc_SysInfoTypeSB1 *sb1_ptr;         /* Local pointer to SB1       */
  rrc_SysInfoTypeSB2 *sb2_ptr;         /* Local pointer to SB2       */

  rrc_SIBSb_ReferenceList *sibSb_ptr; /* Local pointer to SIBSb ref list */
  rrc_SIB_ReferenceList *sb_ptr = NULL;      /* Local pointer to SIB ref list   */

  rrcsib_sib_type_vtag sib_type_tag;

  uint32 sb1_rep_rate = 0;             //local vars used to calculate 
  uint32 sb2_rep_rate = 0;             //SIB time during bplmn searches
  uint32 max_rept_rate = 0;
  uint32 index=0;
  boolean  sib1_or_sib3_config_in_un_received_sb = FALSE;
  boolean  all_scheduling_blocks_received = TRUE;
  rrc_ExtSIBTypeInfoSchedulingInfo_List *ext_siblist_ptr = NULL;

#ifdef FEATURE_WCDMA_TO_LTE
  rrc_ExtSIBTypeInfoSchedulingInfo_List2 *ext_siblist2_ptr = NULL;
#endif


  rrc_SIB_ReferenceList *sb_ptr_tsib = NULL;
  rrc_SIBSb_ReferenceList *sib_sb_ptr_tsib = NULL;
  rrcsib_sib_type_vtag sib_type_tag_tsib;
  seg_proc_stat_e_type status = SEG_PROC_NONE;

  rrc_ExtSIBTypeInfoSchedulingInfo_List *ext_siblist_ptr_tsib = NULL;

#ifdef FEATURE_WCDMA_TO_LTE
  rrc_ExtSIBTypeInfoSchedulingInfo_List2 *ext_siblist2_ptr_tsib = NULL;
#endif

  rrclog_complete_sib_rcvd_event_type sib_rcvd_event;


  if(EVENT_INDEX_VALID(event_index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return;
  }

  if(((pdu_num == rrc_SB1)&&(sib_events_ptr->scheduling_updated_by_sb1 == TRUE))||
     ((pdu_num == rrc_SB2)&&(sib_events_ptr->scheduling_updated_by_sb2 == TRUE)))
  {
    WRRC_MSG1_HIGH("Scheduling already updated by %d",pdu_num);
    return;
  }

  switch(pdu_num)
  {
    //if the updating pdu is MIB, get the sib type and later update the
    //scheduling information
    case rrc_MIB:
      mib_ptr = (rrc_MasterInformationBlock *)sib_ptr;

      sibSb_ptr = &(mib_ptr->sibSb_ReferenceList);
  
      index = 0;
      while(sibSb_ptr->n > index)
      {
        rrcsib_get_sib_type_from_sibsb_type_and_tag(&sibSb_ptr->elem[index].sibSb_Type,
                                                    &sib_type_tag);
        if(sib_type_tag.sib_type != rrc_MAX_SIB)
        {
          rrcsib_update_pos_offset_in_sched_info(
            sib_type_tag.sib_type,
            &sibSb_ptr->elem[index].scheduling);
        }
        index++;
      }

      if(EXTENSION_SIB_SCHED_PRESENT_IN_MIB(mib_ptr))
      {
        ext_siblist_ptr = &(mib_ptr->v690NonCriticalExtensions.
          v6b0NonCriticalExtensions.masterInformationBlock_v6b0ext.extSIBTypeInfoSchedulingInfo_List);
  
        index = 0;
        while(ext_siblist_ptr->n > index)
        {
          if(ext_siblist_ptr->elem[index].extensionSIB_Type.t & T_rrc_SIB_TypeExt_systemInfoType11bis)
          {
            WRRC_MSG1_HIGH("sib11bis:Sched present for SIB11bis in MIB, bit mask val =%x",
              ext_siblist_ptr->elem[index].extensionSIB_Type.t);
            
            rrcsib_update_pos_offset_in_sched_info(rrc_SIB11bis, &ext_siblist_ptr->elem[index].schedulingInfo);
          }
          else
          {
            WRRC_MSG1_ERROR("sib11bis:Ext sib bit mask val =%x, Not supported",
              ext_siblist_ptr->elem[index].extensionSIB_Type.t);
          }
          index++;
        }
      }

#ifdef FEATURE_WCDMA_TO_LTE
      if((mib_ptr->m.v690NonCriticalExtensionsPresent) &&
         (mib_ptr->v690NonCriticalExtensions.m.v6b0NonCriticalExtensionsPresent) &&
         (mib_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) &&
         (mib_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.
            v860NonCriticalExtensions.masterInformationBlock_v860ext.m.extSIBTypeInfoSchedulingInfo_ListPresent))
      {
        ext_siblist2_ptr = &(mib_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.
            v860NonCriticalExtensions.masterInformationBlock_v860ext.extSIBTypeInfoSchedulingInfo_List);
  
        index = 0;
        while(ext_siblist2_ptr->n > index)
        {
          if(ext_siblist2_ptr->elem[index].extensionSIB_Type2.t & T_rrc_SIB_TypeExt2_systemInfoType19)
          {
            WRRC_MSG1_HIGH("sib19:Sched present for SIB19 in MIB, bit mask val =%x",
                ext_siblist2_ptr->elem[index].extensionSIB_Type2.t);

            rrcsib_update_pos_offset_in_sched_info(rrc_SIB19, &ext_siblist2_ptr->elem[index].schedulingInfo);
          }
          else
          {
            WRRC_MSG1_ERROR("sib19:Ext sib bit mask val =%x, Not supported",
                ext_siblist2_ptr->elem[index].extensionSIB_Type2.t);
          }
          index++;
        }
      }

#endif

      index = 0;
      sib_sb_ptr_tsib = &(mib_ptr->sibSb_ReferenceList);
      while(sib_sb_ptr_tsib->n > index)
      {
        rrcsib_get_sib_type_from_sibsb_type_and_tag(&sib_sb_ptr_tsib->elem[index].sibSb_Type,
                                                      &sib_type_tag_tsib);
        if(sib_type_tag_tsib.sib_type != rrc_MAX_SIB)
        {
          if(rrcsib_is_sib_seg_in_tsib_optim_db(sib_type_tag_tsib.sib_type) == TRUE)
          {
            status = rrcsib_check_if_sched_info_match(sib_type_tag_tsib.sib_type,event_index);
            if(status == SEG_PROC_COMPLETE)
            {
              WRRC_MSG1_HIGH("All segments were received prior to scheduling info. Received SIB %d",
                        rrcsib_map_int_sibtype_to_asn1_sibtype(sib_type_tag_tsib.sib_type));
              rrc_log_complete_sib(rrcsib_map_int_sibtype_to_asn1_sibtype(sib_type_tag_tsib.sib_type),0,0);
              sib_events_ptr->event[event_index].tsib[sib_type_tag_tsib.sib_type].sib_received = TRUE;
              sib_events_ptr->event[event_index].tsib[sib_type_tag_tsib.sib_type].sib_decoded = FALSE;
            }
          }
        }
        index++;
      }

#ifdef FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
      if(sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_MANUAL_SCAN ||
         sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN ||
         sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN )
      {
        if(sib_events_ptr->event[event_index].tsib[rrc_SB1].sib_needed == TRUE &&
            sib_events_ptr->sib_sched[rrc_SB1].repitition_rate != INVALID_SIB_SCHEDULE_REP)
        {
          max_rept_rate = sib_events_ptr->sib_sched[rrc_SB1].repitition_rate;
        }

        MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
              "BTD:-sb1_needed =%d,Rept_rate = %d sb2_needed =%d,Rept_rate = %d", 
               sib_events_ptr->event[event_index].tsib[rrc_SB1].sib_needed ,
               sib_events_ptr->sib_sched[rrc_SB1].repitition_rate,
               sib_events_ptr->event[event_index].tsib[rrc_SB2].sib_needed ,
               sib_events_ptr->sib_sched[rrc_SB2].repitition_rate);

        if(sib_events_ptr->event[event_index].tsib[rrc_SB2].sib_needed == TRUE &&
            sib_events_ptr->sib_sched[rrc_SB2].repitition_rate != INVALID_SIB_SCHEDULE_REP)
        {
          max_rept_rate = ((max_rept_rate>sib_events_ptr->sib_sched[rrc_SB2].repitition_rate)?
                                      max_rept_rate : sib_events_ptr->sib_sched[rrc_SB2].repitition_rate);
        }
      }
#endif

      if( ((sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_AUTO_BPLMN_SCAN) ||
           (sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_WTOW_AUTO_BPLMN_SCAN) ||
           (sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)||
           (sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_MANUAL_SCAN)||
           (sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN)||
           (sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_CELL_ID_READ_IN_DCH_SCAN))&&
           sib_events_ptr->event[event_index].event_name == RRCSIB_GET_SPECIFIC_SIBS)
      {
        if((sib_events_ptr->sib_sched[rrc_SIB1].repitition_rate != INVALID_SIB_SCHEDULE_REP) &&
           (sib_events_ptr->sib_sched[rrc_SIB3].repitition_rate != INVALID_SIB_SCHEDULE_REP))
        {
          /* Stop the SIB wait timer in case it's running. and start a new timer based on rep factor times the
             reptition rate for sib 3 */
          rrctmr_stop_timer( RRCTMR_SIB_WAIT_TIMER );

          WRRC_MSG2_HIGH("SIB1,3 in MIB SIB 1 rep %d, SIB 3 rep %d", 
                   sib_events_ptr->sib_sched[rrc_SIB1].repitition_rate,
                   sib_events_ptr->sib_sched[rrc_SIB3].repitition_rate);

#ifdef FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
          if(sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_MANUAL_SCAN ||
             sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN ||
             sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN )
          {
            max_rept_rate = (max_rept_rate >sib_events_ptr->sib_sched[rrc_SIB3].repitition_rate)?
                                      max_rept_rate:sib_events_ptr->sib_sched[rrc_SIB3].repitition_rate;
            max_rept_rate = (max_rept_rate >sib_events_ptr->sib_sched[rrc_SIB1].repitition_rate)?
                                      max_rept_rate : sib_events_ptr->sib_sched[rrc_SIB1].repitition_rate;
            
          WRRC_MSG1_HIGH("BTD:-Final max_rept_rate = %d",max_rept_rate);
          }
          else
#endif
          {
            max_rept_rate = (sib_events_ptr->sib_sched[rrc_SIB3].repitition_rate >
                           sib_events_ptr->sib_sched[rrc_SIB1].repitition_rate) ?
                           sib_events_ptr->sib_sched[rrc_SIB3].repitition_rate:
                           sib_events_ptr->sib_sched[rrc_SIB1].repitition_rate;
          }
          if(sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_CELL_ID_READ_IN_DCH_SCAN)
          {
            WRRC_MSG1_HIGH("RRCGPS:Stop MIB, Start SIB1,3 tmr for %d ms", (max_rept_rate * RRCSIB_CELL_ID_SIB3_WAIT_FACTOR));
            rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER,
                              (unsigned long) (max_rept_rate * RRCSIB_CELL_ID_SIB3_WAIT_FACTOR));
          }
          else
          {
          WRRC_MSG1_HIGH("Stop MIB, Start SIB1,3 tmr for %d ms", (max_rept_rate * RRCSIB_BPLMN_NBCCH_SIB_WAIT_FACTOR));
          rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER,
                              (unsigned long) (max_rept_rate * RRCSIB_BPLMN_NBCCH_SIB_WAIT_FACTOR));
          }
        }
        else
        {

          //sib1,3 info not present in MIB and SB1 and SB2 is not present, this is an invalid config..
          if((sib_events_ptr->sib_sched[rrc_SB1].repitition_rate == INVALID_SIB_SCHEDULE_REP) &&
             (sib_events_ptr->sib_sched[rrc_SB2].repitition_rate == INVALID_SIB_SCHEDULE_REP))
          {
            WRRC_MSG0_HIGH("Mandatory SIB's not present");
          }
          else  //valid config, check max scheduling rate of SB1 and SB2 and start timer
          {
            if(sib_events_ptr->sib_sched[rrc_SB1].repitition_rate != INVALID_SIB_SCHEDULE_REP)
            {
              sb1_rep_rate = sib_events_ptr->sib_sched[rrc_SB1].repitition_rate;
            }
            if(sib_events_ptr->sib_sched[rrc_SB2].repitition_rate != INVALID_SIB_SCHEDULE_REP)
            {
              sb2_rep_rate = sib_events_ptr->sib_sched[rrc_SB2].repitition_rate;
            }

            max_rept_rate = (sb1_rep_rate > sb2_rep_rate) ? sb1_rep_rate: sb2_rep_rate;

            WRRC_MSG1_HIGH("Start SB timer max_rep_rate %d", max_rept_rate);
            /* Stop the SIB wait timer in case it's running. and start a new timer based on rep factor times the
               reptition rate of max of SB1 or SB2 */
            rrctmr_stop_timer( RRCTMR_SIB_WAIT_TIMER );
            if(sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_CELL_ID_READ_IN_DCH_SCAN)
            {
              rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER,(unsigned long) (max_rept_rate * RRCSIB_CELL_ID_SIB3_WAIT_FACTOR));
            }
            else
            {
              rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER,
                                  (unsigned long) (max_rept_rate * RRCSIB_BPLMN_NBCCH_SIB_WAIT_FACTOR));
            }
          }
        }
      }
      else if(sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_NORMAL)
      {
        /* ateast one sb is present.. get the max and then start a time based on SB */
        if((sib_events_ptr->sib_sched[rrc_SB1].repitition_rate != INVALID_SIB_SCHEDULE_REP) ||
           (sib_events_ptr->sib_sched[rrc_SB2].repitition_rate != INVALID_SIB_SCHEDULE_REP))
        {
          if(sib_events_ptr->sib_sched[rrc_SB1].repitition_rate != INVALID_SIB_SCHEDULE_REP)
          {
            sb1_rep_rate = sib_events_ptr->sib_sched[rrc_SB1].repitition_rate;
          }

          if(sib_events_ptr->sib_sched[rrc_SB2].repitition_rate != INVALID_SIB_SCHEDULE_REP)
          {
            sb2_rep_rate = sib_events_ptr->sib_sched[rrc_SB2].repitition_rate;
          }

          max_rept_rate = (sb1_rep_rate > sb2_rep_rate) ? sb1_rep_rate: sb2_rep_rate;

          WRRC_MSG1_HIGH("Start SB timer for N_BCCH max_rep_rate %d", max_rept_rate);

          if(sib_events_ptr->event[event_index].for_serving_cell == FALSE)
          {
#ifdef FEATURE_DUAL_SIM
            if(g_in_data_call)
            {
              rrctmr_stop_timer(RRCTMR_SIB_WAIT_TIMER);

              /*SIB wait timer is max of 3000, 60*rep_rate*/
              rrctmr_start_timer(RRCTMR_SIB_WAIT_TIMER, MAX(3000,60*max_rept_rate));

              rrctmr_start_timer(RRCTMR_SIB_MASK_INVERT_TIMER, 
                                 MAX(max_rept_rate,RRCSIB_MIN_MASK_INV_TIMER)*RRCSIB_MASK_INVERT_FACTOR 
                                     + RRCSIB_TRANS_BUFFER + RRCSIB_EMPTY_FRAMES);

              rrctmr_start_timer(RRCTMR_SIB_STOP_PARTITION_TIMER, MAX(2500,40*max_rept_rate));
              rrcsib_stop_partition = FALSE;
            }
            else
#endif
            {
              rrctmr_stop_timer( RRCTMR_SIB_WAIT_TIMER );
              rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER,
                                  (unsigned long) (max_rept_rate * RRCSIB_BPLMN_NBCCH_SIB_WAIT_FACTOR));
            }
          }
#ifdef FEATURE_DUAL_SIM
          else if (g_in_data_call)
          {
            rrctmr_start_timer(RRCTMR_SIB_MASK_INVERT_TIMER, 
                               MAX(max_rept_rate,RRCSIB_MIN_MASK_INV_TIMER)*RRCSIB_MASK_INVERT_FACTOR 
                                + RRCSIB_TRANS_BUFFER + RRCSIB_EMPTY_FRAMES);
          }
#endif
        }
        /* none of the SB's are present.. get the max-timer updated by mib and start wait timer */
        else
        {
          WRRC_MSG1_HIGH("SB's not present N_BCCH max_rep_rate is %d", sib_events_ptr->scheduling_max_rept_rate);

          //for some unforseen reason,  max_rept_rate is still 0<uninitialized> start the default timer
          //print a MSG_ERROR to get attention
          if(sib_events_ptr->scheduling_max_rept_rate == 0)
          {
            WRRC_MSG0_ERROR("max_rept_rate == 0.  Setting default timer");
            rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER, RRCSIB_MAX_WAIT_TIME_FOR_SIBS );
          }
          else
          {
            if(sib_events_ptr->event[event_index].for_serving_cell == FALSE)
            {
#ifdef FEATURE_DUAL_SIM
              if(g_in_data_call)
              {
                rrctmr_stop_timer(RRCTMR_SIB_WAIT_TIMER);

                /*SIB wait timer is max of 3000, 60*rep_rate*/
                rrctmr_start_timer(RRCTMR_SIB_WAIT_TIMER, MAX(3000,60*sib_events_ptr->scheduling_max_rept_rate));

                rrctmr_start_timer(RRCTMR_SIB_MASK_INVERT_TIMER, 
                                   MAX(sib_events_ptr->scheduling_max_rept_rate,RRCSIB_MIN_MASK_INV_TIMER)*RRCSIB_MASK_INVERT_FACTOR
                                   + RRCSIB_TRANS_BUFFER + RRCSIB_EMPTY_FRAMES);

                rrctmr_start_timer(RRCTMR_SIB_STOP_PARTITION_TIMER, MAX(2500,40*sib_events_ptr->scheduling_max_rept_rate));
                rrcsib_stop_partition = FALSE;
              }
              else
#endif
              {
                rrctmr_stop_timer( RRCTMR_SIB_WAIT_TIMER );
            rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER, 
                                 (unsigned long) (sib_events_ptr->scheduling_max_rept_rate * RRCSIB_BPLMN_NBCCH_SIB_WAIT_FACTOR));
              }
            }
#ifdef FEATURE_DUAL_SIM
            else if (g_in_data_call)
            {
              rrctmr_start_timer(RRCTMR_SIB_MASK_INVERT_TIMER, 
                                 MAX(sib_events_ptr->scheduling_max_rept_rate,RRCSIB_MIN_MASK_INV_TIMER)*RRCSIB_MASK_INVERT_FACTOR
                                 + RRCSIB_TRANS_BUFFER + RRCSIB_EMPTY_FRAMES);
            }
#endif
          }
        }
      }
      break;
    //if the updating pdu is SB1 or SB2, get the sib type and later update the
    //scheduling information
    case rrc_SB1:
    case rrc_SB2:

      if( ((sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN) ||
           (sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_WTOW_AUTO_BPLMN_SCAN) ||
           (sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_AUTO_BPLMN_SCAN) || 
           (sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_MANUAL_SCAN) ||
           (sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN)||
           (sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_CELL_ID_READ_IN_DCH_SCAN))&&      
            sib_events_ptr->event[event_index].event_name == RRCSIB_GET_SPECIFIC_SIBS)
      {
        if((sib_events_ptr->sib_sched[rrc_SIB1].repitition_rate == INVALID_SIB_SCHEDULE_REP) ||
           (sib_events_ptr->sib_sched[rrc_SIB3].repitition_rate == INVALID_SIB_SCHEDULE_REP))
        {
          sib1_or_sib3_config_in_un_received_sb = TRUE;
        }
      }

      if( pdu_num == (uint32)rrc_SB1 )
      {
        sb1_ptr = (rrc_SysInfoTypeSB1 *)sib_ptr;
        sb_ptr = &(sb1_ptr->sib_ReferenceList);
        sib_events_ptr->scheduling_updated_by_sb1 = TRUE;
        if(EXTENSION_SIB_SCHED_PRESENT_IN_SB1_SB2(sb1_ptr,
             rrc_SysInfoTypeSB1,sysInfoTypeSB1))
        {
          ext_siblist_ptr = &(sb1_ptr->v6b0NonCriticalExtensions.sysInfoTypeSB1_v6b0ext.
            extSIBTypeInfoSchedulingInfo_List);
        }

#ifdef FEATURE_WCDMA_TO_LTE
        if((sb1_ptr->m.v6b0NonCriticalExtensionsPresent) &&
           (sb1_ptr->v6b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) &&
           (sb1_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoTypeSB1_v860ext.
               m.extSIBTypeInfoSchedulingInfo_ListPresent))
        {
            ext_siblist2_ptr = &sb1_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.
              sysInfoTypeSB1_v860ext.extSIBTypeInfoSchedulingInfo_List;
        }
#endif

      }
      else
      {
        sb2_ptr = (rrc_SysInfoTypeSB2 *)sib_ptr;
        sb_ptr = &(sb2_ptr->sib_ReferenceList);
        sib_events_ptr->scheduling_updated_by_sb2 = TRUE;
        if(EXTENSION_SIB_SCHED_PRESENT_IN_SB1_SB2(sb2_ptr,
           rrc_SysInfoTypeSB2,sysInfoTypeSB2))
        {
          ext_siblist_ptr = &(sb2_ptr->v6b0NonCriticalExtensions.sysInfoTypeSB2_v6b0ext.
            extSIBTypeInfoSchedulingInfo_List);
        }

#ifdef FEATURE_WCDMA_TO_LTE
        if((sb2_ptr->m.v6b0NonCriticalExtensionsPresent) &&
           (sb2_ptr->v6b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) &&
           (sb2_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.
              sysInfoTypeSB2_v860ext.m.extSIBTypeInfoSchedulingInfo_ListPresent))
        {
          ext_siblist2_ptr = &(sb2_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.
            sysInfoTypeSB2_v860ext.extSIBTypeInfoSchedulingInfo_List);
        }
#endif

      }

      sb_ptr_tsib = sb_ptr;
      ext_siblist_ptr_tsib = ext_siblist_ptr;
#ifdef FEATURE_WCDMA_TO_LTE
      ext_siblist2_ptr_tsib = ext_siblist2_ptr;
#endif

      index = 0;
      while(sb_ptr->n > index)
      {

        rrcsib_get_sib_type_from_sib_type_and_tag(&sb_ptr->elem[index].sib_Type,
                                                    &sib_type_tag);
        if(sib_type_tag.sib_type != rrc_MAX_SIB)
        {
          rrcsib_update_pos_offset_in_sched_info(
            sib_type_tag.sib_type,
            &sb_ptr->elem[index].scheduling);
        }
        index++;
      }
  
      if(ext_siblist_ptr != NULL)
      {
        index = 0;
        while(ext_siblist_ptr->n > index)
        {
          if(ext_siblist_ptr->elem[index].extensionSIB_Type.t & T_rrc_SIB_TypeExt_systemInfoType11bis)        
          {
            WRRC_MSG1_HIGH("sib11bis:Sched present for SIB11bis in MIB, bit mask val =%x",
                  ext_siblist_ptr->elem[index].extensionSIB_Type.t);
            
            rrcsib_update_pos_offset_in_sched_info(rrc_SIB11bis, &ext_siblist_ptr->elem[index].schedulingInfo);
          }
          else
          {
            WRRC_MSG1_ERROR("sib11bis:Ext sib bit mask val =%x, Not supported",
            RRCGET_SIB_TYPE_AND_TAG((&ext_siblist_ptr->elem[index].extensionSIB_Type)));
          }
          index++;
        }
      }

#ifdef FEATURE_WCDMA_TO_LTE
      if(ext_siblist2_ptr != NULL)
      {
        index = 0;
        while(ext_siblist2_ptr->n > index)
        {
          if(ext_siblist2_ptr->elem[index].extensionSIB_Type2.t & T_rrc_SIB_TypeExt2_systemInfoType19)
          {
            WRRC_MSG1_HIGH("sib19:Sched present for SIB19 in MIB, bit mask val =%x",
                ext_siblist2_ptr->elem[index].extensionSIB_Type2.t);
            
            rrcsib_update_pos_offset_in_sched_info(rrc_SIB19, &ext_siblist2_ptr->elem[index].schedulingInfo);
          }
          else
          {
            WRRC_MSG1_ERROR("sib19:Ext sib bit mask val =%x, Not supported",
                ext_siblist2_ptr->elem[index].extensionSIB_Type2.t);
          }
          index++;
        }
      }
#endif
        
      index = 0;
      while(sb_ptr_tsib->n > index)
      {
        rrcsib_get_sib_type_from_sib_type_and_tag(&sb_ptr_tsib->elem[index].sib_Type,
                                                      &sib_type_tag_tsib);
        if(sib_type_tag_tsib.sib_type != rrc_MAX_SIB)
        {
          if(rrcsib_is_sib_seg_in_tsib_optim_db(sib_type_tag_tsib.sib_type) == TRUE)
          {
            status = rrcsib_check_if_sched_info_match(sib_type_tag_tsib.sib_type,event_index);
            if(status == SEG_PROC_COMPLETE)
            {
              WRRC_MSG1_HIGH("All segments were received prior to scheduling info. Received SIB %d",
                        rrcsib_map_int_sibtype_to_asn1_sibtype(sib_type_tag_tsib.sib_type));
              rrc_log_complete_sib(rrcsib_map_int_sibtype_to_asn1_sibtype(sib_type_tag_tsib.sib_type),0,0);
              sib_events_ptr->event[event_index].tsib[sib_type_tag_tsib.sib_type].sib_received = TRUE;
              sib_events_ptr->event[event_index].tsib[sib_type_tag_tsib.sib_type].sib_decoded = FALSE;
            }
          }
        }
        index++;
      }

      if(ext_siblist_ptr_tsib != NULL)
      {
        index = 0;
        while(ext_siblist_ptr_tsib->n > index)
        {
          if( RRCCHECK_SIB_TypeExt(
                  (&(ext_siblist_ptr_tsib->elem[index].extensionSIB_Type)),
                   systemInfoType11bis)
          )                 
          {
            if(rrcsib_is_sib_seg_in_tsib_optim_db(rrc_ExtensionType) == TRUE)
            {
              status = rrcsib_check_if_sched_info_match_for_ext_sibs(rrc_SIB11bis,event_index);
              if(status == SEG_PROC_COMPLETE)
              {
                WRRC_MSG0_HIGH("All segments were received prior to scheduling info. Received SIB11bis");
                sib_rcvd_event.sib_type = SYSTEM_INFORMATION_BLOCK_TYPE11BIS;
                event_report_payload(EVENT_RRC_COMPLETE_SIB_RECEIVED, sizeof(sib_rcvd_event),
                                    (void *)&sib_rcvd_event);
                sib_events_ptr->event[event_index].tsib[rrc_SIB11bis].sib_received = TRUE;
                sib_events_ptr->event[event_index].tsib[rrc_SIB11bis].sib_decoded = FALSE;
              }
            }
          }
          else
          {
            WRRC_MSG1_ERROR("sib11bis:Ext sib bit mask val =%x, Not supported",
                RRCGET_SIB_TYPE_AND_TAG((&ext_siblist_ptr_tsib->elem[index].extensionSIB_Type)));
          }
          index++;
        }
      }

#ifdef FEATURE_WCDMA_TO_LTE
      if(ext_siblist2_ptr_tsib != NULL)
      {
        index = 0;
        while(ext_siblist2_ptr_tsib->n > index)
        {
          if(ext_siblist2_ptr_tsib->elem[index].extensionSIB_Type2.t & 
              T_rrc_SIB_TypeExt2_systemInfoType19)
          {
            if(rrcsib_is_sib_seg_in_tsib_optim_db(rrc_ExtensionType) == TRUE)
            {
              status = rrcsib_check_if_sched_info_match_for_ext_sibs(rrc_SIB19,event_index);
              if(status == SEG_PROC_COMPLETE)
              {
                WRRC_MSG0_HIGH("All segments were received prior to scheduling info. Received SIB19");
                sib_rcvd_event.sib_type = SYSTEM_INFORMATION_BLOCK_TYPE19;
                event_report_payload(EVENT_RRC_COMPLETE_SIB_RECEIVED, sizeof(sib_rcvd_event),
                                    (void *)&sib_rcvd_event);
                sib_events_ptr->event[event_index].tsib[rrc_SIB19].sib_received = TRUE;
                sib_events_ptr->event[event_index].tsib[rrc_SIB19].sib_decoded = FALSE;
              }
            }
          }
          else
          {
            WRRC_MSG1_ERROR("sib19:Ext sib bit mask val =%x, Not supported",
                ext_siblist2_ptr_tsib->elem[index].extensionSIB_Type2.t);
          }
          index++;
        }
      }
#endif


      //check if scan type is bplmn scan type.. if so, then if SIB1,3 sched info is to be received in
      //scheduling information, check if both are received, then start SIB1,3 timer based on the
      //max rep-rate of SIB1 or 3
      if( ((sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)||
           (sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_WTOW_AUTO_BPLMN_SCAN)||
           (sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_AUTO_BPLMN_SCAN) || 
           (sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_MANUAL_SCAN) ||
           (sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN)||
           (sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_CELL_ID_READ_IN_DCH_SCAN))&&
           sib_events_ptr->event[event_index].event_name == RRCSIB_GET_SPECIFIC_SIBS)
      {
        if(sib1_or_sib3_config_in_un_received_sb == TRUE)
        {
          if((sib_events_ptr->sib_sched[rrc_SIB1].repitition_rate != INVALID_SIB_SCHEDULE_REP) &&
             (sib_events_ptr->sib_sched[rrc_SIB3].repitition_rate != INVALID_SIB_SCHEDULE_REP))
          {
            /* Stop the SIB wait timer in case it's running. and start a new timer based on rep factor times the
               reptition rate for sib 3 */
            rrctmr_stop_timer( RRCTMR_SIB_WAIT_TIMER );
  
            WRRC_MSG2_HIGH("SIB1,3 in MIB SIB 1 rep %d, SIB 3 rep %d", 
                     sib_events_ptr->sib_sched[rrc_SIB1].repitition_rate,
                     sib_events_ptr->sib_sched[rrc_SIB3].repitition_rate);
  
            max_rept_rate = (sib_events_ptr->sib_sched[rrc_SIB3].repitition_rate >
                             sib_events_ptr->sib_sched[rrc_SIB1].repitition_rate) ?
                             sib_events_ptr->sib_sched[rrc_SIB3].repitition_rate:
                             sib_events_ptr->sib_sched[rrc_SIB1].repitition_rate;
  
            if(sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_CELL_ID_READ_IN_DCH_SCAN)
            {
              WRRC_MSG1_HIGH("RRCGPS:Stop MIB, Start SIB1,3 tmr for %d ms", (max_rept_rate * RRCSIB_CELL_ID_SIB3_WAIT_FACTOR));
              rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER,
                                (unsigned long) (max_rept_rate * RRCSIB_CELL_ID_SIB3_WAIT_FACTOR));
            }
            else
            {
              WRRC_MSG1_HIGH("Stop MIB, Start SIB1,3 tmr for %d ms", (max_rept_rate * RRCSIB_BPLMN_NBCCH_SIB_WAIT_FACTOR));
              rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER,
                                  (unsigned long) (max_rept_rate * RRCSIB_BPLMN_NBCCH_SIB_WAIT_FACTOR));
            }
          }
        }
      }
      else if(sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_NORMAL)
      {
        /* ateast one sb is present.. get the max and then start a time based on SB */
        if((sib_events_ptr->sib_sched[rrc_SB1].repitition_rate != INVALID_SIB_SCHEDULE_REP) ||
           (sib_events_ptr->sib_sched[rrc_SB2].repitition_rate != INVALID_SIB_SCHEDULE_REP))
        {
          if(sib_events_ptr->sib_sched[rrc_SB1].repitition_rate != INVALID_SIB_SCHEDULE_REP &&
             sib_events_ptr->scheduling_updated_by_sb1 == FALSE)
          {
            all_scheduling_blocks_received = FALSE;
          }

          if(sib_events_ptr->sib_sched[rrc_SB2].repitition_rate != INVALID_SIB_SCHEDULE_REP &&
             sib_events_ptr->scheduling_updated_by_sb2 == FALSE)
          {
            all_scheduling_blocks_received = FALSE;
          }

          if(TRUE == all_scheduling_blocks_received)
          {

            //for some unforseen reason,  max_rept_rate is still 0<uninitialized> start the default timer
            //print a MSG_ERROR to get attention
            if(sib_events_ptr->scheduling_max_rept_rate == 0)
            {
              WRRC_MSG0_ERROR("max_rept_rate == 0.  Setting default timer");
              rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER, RRCSIB_MAX_WAIT_TIME_FOR_SIBS );
            }
            else
            {
              if(sib_events_ptr->event[event_index].for_serving_cell == FALSE)
              {
#ifdef FEATURE_DUAL_SIM
                if(g_in_data_call)
                {
                  rrctmr_stop_timer(RRCTMR_SIB_WAIT_TIMER);
                  rrctmr_stop_timer(RRCTMR_SIB_MASK_INVERT_TIMER);
                  rrctmr_stop_timer(RRCTMR_SIB_STOP_PARTITION_TIMER);

                  /*SIB wait timer is max of 3000, 60*rep_rate*/
                  rrctmr_start_timer(RRCTMR_SIB_WAIT_TIMER, MAX(3000,60*sib_events_ptr->scheduling_max_rept_rate));

                  rrctmr_start_timer(RRCTMR_SIB_MASK_INVERT_TIMER, 
                                     MAX(sib_events_ptr->scheduling_max_rept_rate,RRCSIB_MIN_MASK_INV_TIMER)*RRCSIB_MASK_INVERT_FACTOR
                                     + RRCSIB_TRANS_BUFFER);

                  rrctmr_start_timer(RRCTMR_SIB_STOP_PARTITION_TIMER, MAX(2500,40*sib_events_ptr->scheduling_max_rept_rate));
                  rrcsib_stop_partition = FALSE;
                }
                else
#endif
                {
                  rrctmr_stop_timer( RRCTMR_SIB_WAIT_TIMER );
              rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER, 
                                   (unsigned long) (sib_events_ptr->scheduling_max_rept_rate * RRCSIB_BPLMN_NBCCH_SIB_WAIT_FACTOR));
                }
              }
#ifdef FEATURE_DUAL_SIM
              else if (g_in_data_call)
              {
                rrctmr_start_timer(RRCTMR_SIB_MASK_INVERT_TIMER, 
                                   MAX(sib_events_ptr->scheduling_max_rept_rate,RRCSIB_MIN_MASK_INV_TIMER)*RRCSIB_MASK_INVERT_FACTOR
                                   + RRCSIB_TRANS_BUFFER);
              }
#endif
            }
          }
        }
        else
        {
          WRRC_MSG0_ERROR("SB, schedule not present, but SB received");
        }
      }
      break;

    default:
      WRRC_MSG1_ERROR("Unknown Sib-Sched Type %d", pdu_num);
      break;
  }
}
/*===========================================================================

FUNCTION rrcsib_update_mib_sb_sched_info_from_sib_db

DESCRIPTION
  Updates scheduling information from sib_db.
  This is used when we get a serving-cell event that needs to
  update SIB's(eg: SIB7) and we already know the serving cell is valid and have
  the SIB's and MIB stored in DB

DEPENDENCIES
 
  None.

RETURN VALUE
 
  None.

SIDE EFFECTS
  SIB-EVENT DB is updated
===========================================================================*/
static void rrcsib_update_mib_sb_sched_info_from_sib_db
(
uint32 dbindex,
sib_events_index_e_type event_index
)
{

  void *sib_ptr;

  if((sib_ptr = rrc_sibdb_return_sib_for_db_index(rrc_MIB, dbindex)) != NULL)
  {
    WRRC_MSG2_HIGH("Sched. Info DB-Idx %d for SIB %d", dbindex, rrc_MIB);
    rrcsib_update_sched_info_from_mib_sb((uint32)rrc_MIB, sib_ptr, event_index);
  }
  else
  {
    WRRC_MSG1_ERROR("MIB not present in db %d for idx", dbindex);
  }

  if((sib_ptr = rrc_sibdb_return_sib_for_db_index(rrc_SB1, dbindex)) != NULL)
  {
    WRRC_MSG2_HIGH("Sched. Info DB-Idx %d for SIB %d", dbindex, rrc_SB1);
    rrcsib_update_sched_info_from_mib_sb((uint32)rrc_SB1,sib_ptr, event_index);
  }

  if((sib_ptr = rrc_sibdb_return_sib_for_db_index(rrc_SB2, dbindex)) != NULL)
  {
    WRRC_MSG2_HIGH("Sched. Info DB-Idx %d for SIB %d", dbindex, rrc_SB2);
    rrcsib_update_sched_info_from_mib_sb((uint32)rrc_SB2, sib_ptr, event_index);
  }
}

/*===========================================================================

FUNCTION rrc_sib_initialize_supported_sibs_lookup_table

DESCRIPTION

  Initializes lookup table of sibs that are supported.

DEPENDENCIES
 
  None.

RETURN VALUE
 
  None.

SIDE EFFECTS

  None.
        
===========================================================================*/
void rrc_sib_initialize_supported_sibs_lookup_table
(
  void
)
{
  uint32 count;

  for( count = 0; count < RRC_TOT_SIBS; count++ )
  {
    switch(count)
    {
      case rrc_MIB:
      case rrc_SIB1:
      case rrc_SIB3:
      case rrc_SIB4:
      case rrc_SIB5:
      case rrc_SIB6:
      case rrc_SIB7:
      case rrc_SIB11:
      case rrc_SIB12:
      case rrc_SIB18:
      case rrc_SB1:
      case rrc_SB2:
      case rrc_SIB2:
        supported_sibs_lookup_table[count] = TRUE;
        break;

      case rrc_SIB11bis:
        if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6)
        {
          supported_sibs_lookup_table[count] = TRUE;
        }
        else
        {
         supported_sibs_lookup_table[count] = FALSE;
       }
        break;

#ifdef FEATURE_WCDMA_TO_LTE
      case rrc_SIB19:
        supported_sibs_lookup_table[count] = TRUE;
        break;
#endif

      default:
        supported_sibs_lookup_table[count] = FALSE;
        break;
    }
  }
}
/*===========================================================================

FUNCTION RRCSIB_CHECK_IF_NEXT_SFN_EXPECTED

DESCRIPTION

  This function calculates the next possible sfn for given SIB segment

DEPENDENCIES

  None.
  
RETURN VALUE
  
  Relative diffirence between current sfn and next possible sfn

SIDE EFFECTS

  None.

===========================================================================*/
static int rrcsib_check_if_next_sfn_expected(rrc_SIB_type sib,uint16 cur_sfn,uint8 seg_index)
{
  int relative_diff_bw_curr_and_next_sib_sfn;

  /* calculate the difference in sfn-primes between Curr_sfn prime and the next possible sfn_prime
     for the sib passed in function */

  relative_diff_bw_curr_and_next_sib_sfn = ((sib_events_ptr->sib_sched[sib].sfn_offset[seg_index])/2) -
                        ((cur_sfn/2)%(((sib_events_ptr->sib_sched[sib].repitition_rate)/2)));

  /* If the above formula gives a negative value, add the repetition rate of the SIB to get the next 
     possible sfn_prime for the sib */

  if(relative_diff_bw_curr_and_next_sib_sfn <0)
  {
    relative_diff_bw_curr_and_next_sib_sfn = relative_diff_bw_curr_and_next_sib_sfn + ((sib_events_ptr->sib_sched[sib].repitition_rate )/2);
  }

  MSG_HIGH("relative_diff_bw_curr_and_next_sib_sfn is  %d for SIB %d and cur_sfn is %d", relative_diff_bw_curr_and_next_sib_sfn, rrcsib_map_int_sibtype_to_asn1_sibtype(sib),cur_sfn);
  
  return relative_diff_bw_curr_and_next_sib_sfn;
}
/*===========================================================================

FUNCTION RRCSIB_CHECK_IF_SIBS_SCHEDULED_DURING_PICH

DESCRIPTION

  This function checks if sib1 or sib3 is scheduled during the 
  PICH of next DRX cycle. 

DEPENDENCIES

  None.
  
RETURN VALUE
  
  TRUE if they are scheduled.
  FALSE if they are not scheduled.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcsib_check_if_sibs_scheduled_during_pich()
{
  uint16 cur_sfn,relative_diff;
  uint8 no_of_sfns,i;
  int next_possible_sfn;
  uint8 seg_index[MAX_SEGMENTS_FOR_A_SIB]; 
  uint8 rrc_sib_type[] = {rrc_SIB1,rrc_SIB3,rrc_SB1,rrc_SB2};
  
  cur_sfn = dl_get_cell_timing((rrc_csp_int_data.curr_acq_entry.scr_code/16),
    rrc_csp_int_data.curr_acq_entry.freq);

  if (cur_sfn == 0xFFFF)
  {
    return TRUE;
  }
  
  no_of_sfns = (xtow_bplmn_search_overhead_timer + RRC_PICH_AND_MEASUREMENTS_TIME)/20;


  for(i = 0; i < sizeof(rrc_sib_type); i++)
  {
    if((sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_sib_type[i]].sib_needed == TRUE) &&
       (sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_sib_type[i]].sib_received != TRUE)&&
       (sib_events_ptr->sib_sched[rrc_sib_type[i]].repitition_rate != INVALID_SIB_SCHEDULE_REP))
    {
      relative_diff = rrcsib_check_if_next_sfn_expected(rrc_sib_type[i],cur_sfn,0);
      if((relative_diff < no_of_sfns) &&
         ((relative_diff + ((sib_events_ptr->sib_sched[rrc_sib_type[i]].repitition_rate )/2))> (rrcllc_get_drx_info()/2)))
      {
        return TRUE;
      }
    }
  }
  return FALSE;  
} 
/*===========================================================================

FUNCTION CREATE_NEW_SIB_EVENT

DESCRIPTION

  This function activates and initializes an event for 
  getting SIBs. The event can be for either a serving
  cell or for a neighbor cell. Only one event of either
  cell type can be supported at a time. 

DEPENDENCIES

  None.
  
RETURN VALUE
  
  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void create_new_sib_event
(
  sib_events_index_e_type index, /* Index in the SIB event database
                                 for this event */
  rrc_cmd_type *cmd_ptr          /* Received command pointer */
)
{
  uint32 count;  /* Used as a counter */
  #ifdef FEATURE_SIB19_DECODE_FAIL_OPT
  sib19_decode_fail_cnt=0; 
  #endif
  
  /* Fill in relevant data for the event using the assigned
  index */

  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return;
  }

#ifdef FEATURE_3GPP_CSFB

  /*Do not reset the presence of SIB container for DMCR update SIBs*/
  if(!((cmd_ptr->cmd_hdr.cmd_id == RRC_UPDATE_SIBS_IND)&&
       (cmd_ptr->cmd.update_sibs_ind.cause == RRCSIB_DEFERRED_SIB_READ)))
  {
    /*Reset the presence of SIB container*/
    is_sib_cont_present = FALSE;
  }
#endif

  /* Event is now active */
  sib_events_ptr->event[index].event_active = TRUE;

  /* Assume that a SIB Database Index has not been found yet
  for this event. Also mib and sib3 have not been
  decoded yet and mip position hasn't been validated. 
  This is the case for all events except events where
  we're updating specific SIBs for the serving cell.
  This is handled later in this function */

  sib_events_ptr->event[index].sib_db_index_found = FALSE;
  sib_events_ptr->event[index].sib_db_index = SIBDB_INVALID_INDEX;
  sib_events_ptr->event[index].tempholder.mib_decoded = FALSE;
  sib_events_ptr->event[index].tempholder.sib3_decoded = FALSE;
  sib_events_ptr->event[index].tempholder.sib1_decoded = FALSE;
  sib_events_ptr->event[index].mib_pos_validated = FALSE;
  sib_events_ptr->event[index].is_mib_vtag_match = FALSE;
#ifdef FEATURE_FEMTO_CSG
  sib_events_ptr->event[index].for_csg = FALSE;
  sib_events_ptr->event[index].is_csg = FALSE;
  sib_events_ptr->event[index].csg_id = 0xFFFFFFFF;
#endif

#ifdef FEATURE_DUAL_SIM
  rrcsib_pattern_invert = FALSE;
  rrcsib_stop_partition = FALSE;
  g_in_data_call = WRM_G2W_TA_MODE_ACTIVE();
#endif

  //invalidate sib-scheduling info as we are setting up a new event
  rrcsib_invalidate_sib_scheduling_info(index);

#ifdef FEATURE_UPDATE_SIB7_IN_FACH
  sib_events_ptr->event[index].int_event.int_event_type 
          = RRCSIB_INT_MAX_EVENT;
#endif

#ifdef FEATURE_RRC_NO_MIB_FOUND_BAR_CELL
  no_mib_bar_cell_count = 0;
#endif

#ifdef FEATURE_RRC_SIB_SUPPORT_MAX_REPT_RATE
  max_sib_rept_rate = 0;
  timer_value_started  = RRCSIB_MAX_WAIT_TIME_FOR_SIBS;
#endif

  /*Initialize tsib optimization data base*/
  rrcsib_clear_tsib_optimization_db();

  /* Based on the type of event requested, each SIB that
  needs to be collected will be activated. */

  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_UPDATE_SIBS_IND:

      /* We use the same case statement for these two events
      since we have to get all the necessary SIBs for both
      events. But for RRC_UPDATE_SIBS_IND, we can use value-tag
      comparisons later on and get only the sibs that have changed.
      Loop through all SIBs and activate necessary ones. */
      for( count = 0; count < RRC_TOT_SIBS; count++ )
      {
        if(supported_sibs_lookup_table[count] == FALSE)
        {
          sib_events_ptr->event[index].tsib[count].sib_needed = FALSE;
        }
        else
        {
          /* We need to collect all other SIBs. Initialize all
          necessary variables within the SIB event database */ 
          sib_events_ptr->event[index].tsib[count].sib_needed = TRUE;
          sib_events_ptr->event[index].tsib[count].sib_received = FALSE;
          sib_events_ptr->event[index].tsib[count].sib_decoded = FALSE;
          sib_events_ptr->event[index].tsib[count].seg_count = 0;
          /* For now, make the current segment index invalid. This is
          because only after the first segment is received, the segment
          index will become zero. */
          sib_events_ptr->event[index].tsib[count].seg_index = (MAX_SEG_INDEX_FOR_A_SIB + 1);
          sib_events_ptr->event[index].tsib[count].sib_tot_length = 0;
        }
      }
      /* Note the event name and the name of the procedure 
      requesting this event - this is used later while sending
      the confirmation command for this event. */
        sib_events_ptr->event[index].event_name = RRCSIB_UPDATE_SIBS;

        /* If the event is RRCSIB_UPDATE_SIBS, we also need to save 
        the new value_tag for the MIB. When we read the MIB again
        we will compare the value tag and make sure it's the
        updated MIB before comparing the value tags given by the
        MIB and the time of modification. */
        sib_events_ptr->event[index].req_proc =
          cmd_ptr->cmd.update_sibs_ind.procedure;

      /* Note that this case can only be for a serving cell */
      sib_events_ptr->event[index].for_serving_cell = TRUE;

      /* Initialize the scan type to NORMAL for safety */
      sib_events_ptr->event[index].sib_scan_type = RRC_SIB_NORMAL;

      /* When a new serving cell event is received, if there
      is a RRCSIB_UPDATE_SIBS event pending, we can cancel it
      now. It's no longer valid. */
      if( update_sibs_event_pending == TRUE )
      {
        update_sibs_event_pending = FALSE;
      }
#ifdef FEATURE_UPDATE_SIB7_IN_FACH
#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
      REX_ISR_LOCK(&sib7_in_fach_active_mutex);

      if(is_sib7_in_fach_active == TRUE)
      {
        is_sib7_in_fach_active = FALSE;
      }
      REX_ISR_UNLOCK(&sib7_in_fach_active_mutex);
#endif
#endif
      break;

    case RRC_GET_SPECIFIC_SIBS_REQ:

      /* Only get those SIBs specifically requested by the calling
      procedure. Also we need to get the MIB and SIB3 always in order
      to get the index to store these SIBs in the sib database. */
#ifdef FEATURE_FEMTO_CSG
      sib_events_ptr->event[index].for_csg = cmd_ptr->cmd.get_specific_sibs_req.for_csg_cell;
#endif
      for( count = 0; count < RRC_TOT_SIBS; count++ )
      {
        boolean local_sib_needed = FALSE;

#ifdef FEATURE_3GPP_CSFB
        if(cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_READ_FROM_SIB_CONTAINER)
        {
          if(cmd_ptr->cmd.get_specific_sibs_req.sib_req[count] == TRUE)
          {
            local_sib_needed = TRUE;
          }
        }
        else
#endif

#ifdef FEATURE_SKIP_SIB1_3_DURING_MANUAL_PLMN_SCAN
        if((cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)||
           (cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_MANUAL_SCAN) ||
           ((cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN)
#ifdef FEATURE_LTE_TO_WCDMA
            && (!rrccsp_check_cgi_scan())
#endif
           )
          )
        {
          if(cmd_ptr->cmd.get_specific_sibs_req.sib_req[count] == TRUE)
          {
            local_sib_needed = TRUE;
          }
        }
        else
#endif
        {
          if( (cmd_ptr->cmd.get_specific_sibs_req.sib_req[count] == TRUE) ||
              ( count == (uint32)rrc_MIB ) ||
              ( count == (uint32)rrc_SIB3 ) ||
              ( count == (uint32)rrc_SB1) ||
              ( count == (uint32)rrc_SB2))
          {
            if (count ==(uint32) rrc_SIB11bis)
            {
              if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6)
              {
                local_sib_needed = TRUE;
              }
            }
            else
            {
              local_sib_needed = TRUE;
            }
          }
        }
        if(local_sib_needed == TRUE)
        {
          sib_events_ptr->event[index].tsib[count].sib_needed = TRUE;
          sib_events_ptr->event[index].tsib[count].sib_received = FALSE;
          sib_events_ptr->event[index].tsib[count].sib_decoded = FALSE;
          sib_events_ptr->event[index].tsib[count].seg_count = 0;
          /* For now, make the current segment index invalid. This is
           because only after the first segment is received, the segment
           index will become zero. */
          sib_events_ptr->event[index].tsib[count].seg_index = (MAX_SEG_INDEX_FOR_A_SIB + 1);
          sib_events_ptr->event[index].tsib[count].sib_tot_length = 0;
        }
        else
        {
          sib_events_ptr->event[index].tsib[count].sib_needed = FALSE;
          sib_events_ptr->event[index].tsib[count].sib_received = FALSE;
        }
      }

#ifdef FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
        if((cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)||
           (cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_MANUAL_SCAN) ||
           ((cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN)
#ifdef FEATURE_LTE_TO_WCDMA
            && (!rrccsp_check_cgi_scan())
#endif
          ))
        {
          WRRC_MSG1_HIGH("BTD:-Mark SB1/SB2 as needed for scan type = %d ",cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type);
          sib_events_ptr->event[index].tsib[rrc_SB1].sib_needed = TRUE;
          sib_events_ptr->event[index].tsib[rrc_SB1].sib_received = FALSE;
          sib_events_ptr->event[index].tsib[rrc_SB1].sib_decoded = FALSE;
          sib_events_ptr->event[index].tsib[rrc_SB1].seg_count = 0;
          /* For now, make the current segment index invalid. This is
           because only after the first segment is received, the segment
           index will become zero. */
          sib_events_ptr->event[index].tsib[rrc_SB1].seg_index = (MAX_SEG_INDEX_FOR_A_SIB + 1);
          sib_events_ptr->event[index].tsib[rrc_SB1].sib_tot_length = 0;

          sib_events_ptr->event[index].tsib[rrc_SB2].sib_needed = TRUE;
          sib_events_ptr->event[index].tsib[rrc_SB2].sib_received = FALSE;
          sib_events_ptr->event[index].tsib[rrc_SB2].sib_decoded = FALSE;
          sib_events_ptr->event[index].tsib[rrc_SB2].seg_count = 0;
          /* For now, make the current segment index invalid. This is
           because only after the first segment is received, the segment
           index will become zero. */
          sib_events_ptr->event[index].tsib[rrc_SB2].seg_index = (MAX_SEG_INDEX_FOR_A_SIB + 1);
          sib_events_ptr->event[index].tsib[rrc_SB2].sib_tot_length = 0;

        }
#endif

      //if rrc_state is CELL_FACH, then read SIB7 mandatorily for GET_SPECIFIC_SIBS..
      //this is needed because we will send cell-update immediately and need to know the
      //latest UL-Interference parameters
      if((rrc_get_state() == RRC_STATE_CELL_FACH)
#ifdef FEATURE_UPDATE_SIB7_FOR_PCH_TO_FACH
          || (
#ifdef FEATURE_WCDMA_HS_RACH
         (rrcsib_update_sib7_hs_rach() == TRUE) &&
#endif
          ((rrc_get_state() == RRC_STATE_CELL_PCH)||
          (rrc_get_state() == RRC_STATE_URA_PCH)))
#endif
         )
      {
        sib_events_ptr->event[index].tsib[rrc_SIB7].sib_needed = TRUE;
        sib_events_ptr->event[index].tsib[rrc_SIB7].sib_received = FALSE;
        sib_events_ptr->event[index].tsib[rrc_SIB7].sib_decoded = FALSE;
        sib_events_ptr->event[index].tsib[rrc_SIB7].seg_count = 0;
        /* For now, make the current segment index invalid. This is
        because only after the first segment is received, the segment
        index will become zero. */
        sib_events_ptr->event[index].tsib[rrc_SIB7].seg_index = 
          (MAX_SEG_INDEX_FOR_A_SIB + 1);
        sib_events_ptr->event[index].tsib[rrc_SIB7].sib_tot_length = 0;
      }

      /* Note the event name */
      sib_events_ptr->event[index].event_name = RRCSIB_GET_SPECIFIC_SIBS;
      
      /* Note the requesting procedure name */
      sib_events_ptr->event[index].req_proc = cmd_ptr->cmd.get_specific_sibs_req.procedure;

      //copy the scan type for this sib..
      sib_events_ptr->event[index].sib_scan_type =  cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type;


      /* In case of GET_SPECIFIC_SIBS event, if it's an internal
      event we need to check if it's for SIBs with SIB expiration
      timers. If so save the information in the event database */
      /* First check for SIB7 */
      if( (sib_events_ptr->event[index].req_proc == RRC_PROCEDURE_SIB) &&
          ((cmd_ptr->cmd.get_specific_sibs_req.
           sib_req[rrc_SIB7]) == TRUE )
        )
      {
        sib_events_ptr->event[index].int_event.int_event_type 
          = RRCSIB_INT_SIB7_EVENT;

        /* For this event, we already know all the information
        about the serving cell. Also we don't want to read
        the MIB and SIB3 again to update value tags at this
        time */
        sib_events_ptr->event[index].sib_db_index_found = TRUE;
        /* The index is the active cell index */
        sib_events_ptr->event[index].sib_db_index = sibdb.active_cell_index;
        sib_events_ptr->event[index].tempholder.mib_decoded = TRUE;
        sib_events_ptr->event[index].tempholder.sib3_decoded = TRUE;
        sib_events_ptr->event[index].mib_pos_validated = TRUE;

        sib_events_ptr->sib_sched[rrc_MIB].repitition_rate = 8;
        sib_events_ptr->sib_sched[rrc_MIB].sfn_offset[0] =0;
        sib_events_ptr->sib_sched[rrc_MIB].sfn_offset[1] =2;
        sib_events_ptr->sib_sched[rrc_MIB].sfn_offset[2] =4;
        sib_events_ptr->sib_sched[rrc_MIB].sfn_offset[3] =6;


        //update scheduling info first fromt the stored db
        rrcsib_update_mib_sb_sched_info_from_sib_db(sib_events_ptr->event[index].sib_db_index, index);

        /* Also indicate that the MIB and SIB3 for this event
        have already been received and decoded. */
        sib_events_ptr->event[index].
          tsib[rrc_MIB].sib_received = TRUE;
        sib_events_ptr->event[index].
          tsib[rrc_MIB].sib_decoded = TRUE;
        sib_events_ptr->event[index].
          tsib[rrc_SIB3].sib_received = TRUE;
        sib_events_ptr->event[index].
          tsib[rrc_SIB3].sib_decoded = TRUE;
        
        if(TRUE == rrc_sibdb_check_if_sb_present(rrc_SB1,sib_events_ptr->event[index].sib_db_index))
        {
          sib_events_ptr->event[index].tsib[rrc_SB1].sib_received = TRUE;
          sib_events_ptr->event[index].tsib[rrc_SB1].sib_decoded = TRUE;
        }
        
        if(TRUE == rrc_sibdb_check_if_sb_present(rrc_SB2, sib_events_ptr->event[index].sib_db_index))
        {
          sib_events_ptr->event[index].tsib[rrc_SB2].sib_received = TRUE;
          sib_events_ptr->event[index].tsib[rrc_SB2].sib_decoded = TRUE;
        }
#ifdef FEATURE_UPDATE_SIB7_IN_FACH
        if(rrc_sib_is_current_event_sib7_in_fach() == TRUE)
        {
          /*Since for SIB-7 event SIB will use scheduling info from MIB
            So copy scheduling info of MIB to event table.This will avoid
            discarding any SIB-7 which is scheduled just before MIB 
            when this event is brought up*/
#ifdef FEATURE_WCDMA_HS_FACH_DRX
          /* Initially we dont want to skip any SIB7. So pass skip_sfn as zero */
          rrcsib_build_sib7_schedule_table(rrc_SIB7,SIB_SRV_INDEX,0);
#else
          rrcsib_build_sib7_schedule_table(rrc_SIB7,SIB_SRV_INDEX);
#endif
        }
        else
#endif
        {
          rrcsib_update_sib_sleep_schedule_table(SIB_SRV_INDEX);
        }
      }
      /* Other than SIB7 update event, for any other serving cell
      event we should clear the update sibs flag */
      else if( (cmd_ptr->cmd.get_specific_sibs_req.for_serv_cell == TRUE) &&
               (update_sibs_event_pending == TRUE) )
      {
        update_sibs_event_pending = FALSE;
      }

      /* Check other timer based SIBs here when they are supported */

      /* Also make note of whether this GET_SPECIFIC_SIBS event
      is for serving or neighbor cells */
      if(cmd_ptr->cmd.get_specific_sibs_req.for_serv_cell == TRUE)
      {
        sib_events_ptr->event[index].for_serving_cell = TRUE;
      }
#ifdef FEATURE_UPDATE_SIB7_IN_FACH
#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
      if((sib_events_ptr->event[index].int_event.int_event_type == RRCSIB_INT_SIB7_EVENT)&&
         (sib_events_ptr->event[index].req_proc == RRC_PROCEDURE_SIB)&&
         (sib_events_ptr->event[index].for_serving_cell == TRUE)&&
         (rrc_get_state() == RRC_STATE_CELL_FACH)
        )
      {
        WRRC_MSG0_HIGH("SIB7 in cell fach event active");
        REX_ISR_LOCK(&sib7_in_fach_active_mutex);
        is_sib7_in_fach_active = TRUE;
        REX_ISR_UNLOCK(&sib7_in_fach_active_mutex);
      }
      else
      {
        REX_ISR_LOCK(&sib7_in_fach_active_mutex);
        is_sib7_in_fach_active = FALSE;
      REX_ISR_UNLOCK(&sib7_in_fach_active_mutex);
      }
#endif
#endif
      break;
   
    default:
      /* Can't happen since this is caught at a higher level function */
      MSG_MED("Unknown SIB event %d", cmd_ptr->cmd_hdr.cmd_id, 0, 0);
      break;

  }/* End switch */
}/* end function create_new_sib_event */


/*===========================================================================

FUNCTION RRCSIB_CLEAR_PROCEDURE

DESCRIPTION

  This function clears existing events. It sends confirmation
  commands if needed.
    
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None. 
===========================================================================*/
static void rrcsib_clear_procedure
(  
  rrc_sib_status_e_type failure_cause /* Cause of failure. */
)
{
  sib_events_index_e_type count; /* Counter used in loop */

  /* Check if any event is active. If so cancel it */
  for( count = SIB_SRV_INDEX; count < MAX_SIB_EVENTS_INDEX; count++ )
  {
    if( sib_events_ptr->event[count].event_active == TRUE )
    {
      /* Post-process the failed event */
      rrcsib_post_process_failed_event
        ( count, /* Event Index                 */
          TRUE,  /* Confirm Command is required */
          failure_cause, /* Failure Cause       */
          TRUE   /* Tear down S_BCCH if it's up */
        );
    }
  }
}



/*===========================================================================

FUNCTION BCCH_RECONFIGURE_FOR_NEW_SIB_EVENT

DESCRIPTION

  This function is called when a new sib event is
  received while a sib event is in progress. This may
  require a reconfiguration of the BCCH, This function
  resolves the conflict between the events and does the 
  necessary BCCH configurations.
  
  This is how the event conflicts are handled :
  
  A) Existing event is for S-BCCH, new event is for N-BCCH
  
  Tear down S-BCCH, Setup N-BCCH (Both in the same command to LLC).
  In case the existing event is RRCSIB_UPDATE_SIBS event, flag
  it as a pending event so that we can resume it when needed. Only
  if we don't move to a new cell we will resume it. In case the 
  existing event is a GET_SPECIFIC_SIBS event for getting SIB7
  we assume that SIB7 is current for now, since N-BCCH event is 
  considered higher priority.
  
  
  B) Existing event is for N-BCCH, new event is for N-BCCH
  
  Tear down N-BCCH, Setup other N-BCCH (Both in same cmd to LLC).
  
     
  C) Existing event is N-BCCH, new event is S-BCCH
  
  If the new event is RRCSIB_UPDATE_SIBS event, flag it as
  a pending event. No LLC commands are sent. In case the 
  existing event is a GET_SPECIFIC_SIBS event for getting SIB7
  we assume that SIB7 is current for now, since N-BCCH event is 
  considered higher priority. No other S-BCCH event is possible
  when N-BCCH event is in progress.
  
    
  D) Existing event is S-BCCH, new event is S-BCCH
  
  Only 2 S-BCCH events can have conflict : A GET_SPECIFIC_SIBS
  event to update SIB7 and a UPDATE_SIBS event to update sibs
  based on value-tags. In Idle State the GET_SPECIFIC_SIBS event
  to update SIB7 takes precedence since it needs to be read quickly
  for RACH attempts. In CELL_FACH however, since SIB7 updates happen
  every 320 ms, we cannot consider them higher priority than the
  UPDATE_SIBS event (UPDATE_SIBS event may never get done). We
  will postpone the SIB7 event.  

  No other S-BCCH event conflict can happen at this time.
  
DEPENDENCIES
 
  None.

RETURN VALUE
 
  rrcsib_bcch_setup_status_e_type indicating status of
  BCCH setup.

SIDE EFFECTS

  BCCH logical channel may be setup, teared down or modified to a new
  configuration.
       
===========================================================================*/
static rrcsib_bcch_setup_status_e_type bcch_reconfigure_for_new_sib_event
( 
  rrc_cmd_type* ptr, /* Pointer to the command for the new event */
  boolean srv_cell,  /* TRUE if new command is for serving cell  */
  sib_events_index_e_type old_index /* Index of the old (existing) event */
)
{

  rrcsib_bcch_setup_status_e_type bcch_status
    = RRCSIB_BCCH_SETUP_DONE; /* To hold status */

  rrc_state_e_type rrc_state = rrc_get_state();
  
  uint8* table_ptr  = NULL;
  
  if(EVENT_INDEX_VALID(old_index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    bcch_status = RRCSIB_BCCH_SETUP_FAILED;
    return (bcch_status);
  }
  
  /* All serving cell events can be replaced by a neighbor cell
  event. Check if the existing event is a serving cell event. */ 
  if((sib_events_ptr->event[old_index].for_serving_cell == TRUE)||
    (rrcllc_get_s_bcch_setup_status()==TRUE))
  {
    /* Check if the new event is for the neighbor cell. */
    if( srv_cell != TRUE )
    {
      WRRC_MSG0_HIGH("Reconfig: N-BCCH up and S-BCCH down");  

      /* Send an LLC command. Tell it to take the S-BCCH down
      and bring up the N-BCCH for the new event. */
      if(rrcsib_configure_bcch(SBCCH_TEARDOWN, NBCCH_SETUP
                ,RRCLLC_CHAN_CFG_REASON_NONE
           ) == SUCCESS)
      {
        
        /* Copy the other BCCH parameters from the command. For neighbor
        sibs the command type is always "get_specific_sibs_req". */
        sib_events_ptr->ngh_bcch.freq = ptr->cmd.get_specific_sibs_req.freq;
        sib_events_ptr->ngh_bcch.scr_code = ptr->cmd.get_specific_sibs_req.scrambling_code;

        /* Check the existing event type. If it's an RRCSIB_UPDATE_SIBS
        event, flag it as a pending event. */
        if((sib_events_ptr->event[old_index].event_active == TRUE) &&  
           (sib_events_ptr->event[old_index].event_name == RRCSIB_UPDATE_SIBS) )
        {
          update_sibs_event_pending = TRUE;
          
          //invalidate mib and sb's value tags, as we want to re-read them next time
          rrc_sibdb_invalidate_mib_sb_value_tags_for_interrupted_sib_update(sibdb.active_cell_index);
        }

        /* If it is a GET_SPECIFIC_SIBS event for updating SIB7, just assume that SIB7 is current 
           since the NBCCH event is higher priority */
        else if( (sib_events_ptr->event[old_index].req_proc == RRC_PROCEDURE_SIB) &&
          (sib_events_ptr->event[old_index].int_event.int_event_type == 
           RRCSIB_INT_SIB7_EVENT)
          )
        {
          rrcsib_process_int_sib_event_complete( old_index, SUCCESS );
        }
      }

      /* Make the current event inactive */
      sib_events_ptr->event[old_index].event_active = FALSE;

      sib_events_ptr->event[old_index].sib_scan_type = RRC_SIB_NORMAL;

    }
    else
    {
      if(rrc_state == RRC_STATE_DISCONNECTED)
      {
        /* Check if existing event is SIB7 event and new event is UPDATE_SIBS event. 
           If so the UPDATE_SIBS event can be saved for later. */
        if( (sib_events_ptr->event[old_index].req_proc == RRC_PROCEDURE_SIB) &&
            (sib_events_ptr->event[old_index].int_event.int_event_type ==
             RRCSIB_INT_SIB7_EVENT) &&
            (ptr->cmd_hdr.cmd_id == RRC_UPDATE_SIBS_IND)
          )
        {
          /* Since UE will go to CONNECTING state after reading SIB7 no need to save
           * update SIBs event as a pending event
           */
          update_sibs_event_pending = FALSE;
          /* Indicate to calling function that nothing needs to be
           * done for the new event
           */
          bcch_status = RRCSIB_BCCH_EVENT_SAVED;
          WRRC_MSG0_HIGH("SIB7 update in progress. Ignore UPDATE_SIBS event");
        }
        /* Else check if new event is SIB7 event and
         * existing event is UPDATE_SIBS event.
         */
        else if( (ptr->cmd_hdr.cmd_id == RRC_GET_SPECIFIC_SIBS_REQ) &&
          (ptr->cmd.get_specific_sibs_req.procedure == RRC_PROCEDURE_SIB) &&
          (ptr->cmd.get_specific_sibs_req.
             sib_req[rrc_SIB7] == TRUE) &&
          (sib_events_ptr->event[old_index].event_name == RRCSIB_UPDATE_SIBS)
               )
        {
          WRRC_MSG0_HIGH("Replacing UPDATE_SIBS with SIB7 event");
          /* No need to setup BCCH here since serving BCCH is already
           * setup 
           */
          sib_events_ptr->event[old_index].event_name = RRCSIB_GET_SPECIFIC_SIBS;
          sib_events_ptr->event[old_index].req_proc = RRC_PROCEDURE_SIB;
          /* Don't flag the existing event as a pending event. */
          update_sibs_event_pending = FALSE;
          sib_events_ptr->event[old_index].int_event.int_event_type 
            = RRCSIB_INT_SIB7_EVENT;
          /* We need to return setup done. This makes SIB active database
           * to be updated with dbindex so that we look only for SIB7
           */
          bcch_status = RRCSIB_BCCH_SETUP_DONE;
        }
      }
#ifdef FEATURE_UPDATE_SIB7_FOR_PCH_TO_FACH
      else if((rrc_state == RRC_STATE_CELL_PCH) || (rrc_state == RRC_STATE_URA_PCH))
      {
        if( (sib_events_ptr->event[old_index].req_proc == RRC_PROCEDURE_SIB) &&
            (sib_events_ptr->event[old_index].int_event.int_event_type ==
             RRCSIB_INT_SIB7_EVENT) &&
            (ptr->cmd_hdr.cmd_id == RRC_UPDATE_SIBS_IND)
          )
        {
          WRRC_MSG0_HIGH("SIB7 update in progress. Set UPDATE_SIBS event as pending");
          update_sibs_event_pending = TRUE;
          bcch_status = RRCSIB_BCCH_EVENT_SAVED;
        }
        else if( (ptr->cmd_hdr.cmd_id == RRC_GET_SPECIFIC_SIBS_REQ) &&
          (ptr->cmd.get_specific_sibs_req.procedure == RRC_PROCEDURE_SIB) &&
          (ptr->cmd.get_specific_sibs_req.
             sib_req[rrc_SIB7] == TRUE) &&
          (sib_events_ptr->event[old_index].event_name == RRCSIB_UPDATE_SIBS)
               )
        {
          WRRC_MSG0_HIGH("UPDATE_SIBS in progress. Ignore SIB7 update event");
          if((sib7info.sib_ext_proc_update_req == TRUE) && (sib7info.req_proc == RRC_PROCEDURE_CU))
          {
            rrcsib_send_sib7_ind();
          }
          bcch_status = RRCSIB_BCCH_EVENT_SAVED;
        }
      }
#endif
#if  defined(FEATURE_UPDATE_SIB7_IN_FACH)
      else if(rrc_state == RRC_STATE_CONNECTING)
      {
        WRRC_MSG0_HIGH("Shouldn't get a S-BCCH reconfig event");
      }
      else if(rrc_state == RRC_STATE_CELL_FACH)
      {
        /* Indicate to calling function that nothing needs to be
         * done for the new event
         */
#ifdef FEATURE_UPDATE_SIB7_IN_FACH
        if((sib_events_ptr->event[old_index].event_name == RRCSIB_GET_SPECIFIC_SIBS)&&
           (sib_events_ptr->event[old_index].req_proc == RRC_PROCEDURE_SIB)&&
           (sib_events_ptr->event[old_index].int_event.int_event_type == RRCSIB_INT_SIB7_EVENT)
          )
        {
          WRRC_MSG0_HIGH("sib7:Replacing SIB7 event with new event in RRC State FACH");
          bcch_status = RRCSIB_BCCH_SETUP_DONE;

          /* Table ptr can be assigned to any sleep table as we're going to memset the
             whole array anyways */
          table_ptr = sib_sleep_schedule_table_1;
          memset(table_ptr,0xFF,RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE);

          /* Sending the SIB sleep table with memsetted array so that L1 will sleep soon */
          rrcsib_send_sib_sleep_table(table_ptr);
        }
        else
#endif
        {
          bcch_status = RRCSIB_BCCH_EVENT_SAVED;
        }
        /* In CELL_FACH SIB7 updating will happen every
         * 320ms so we cannot hold the UPDATE_SIBS event pending. Consider
         * SIB7 update lower priority in that case.
         */
        rrcsib_process_int_sib_event_complete( old_index, SUCCESS );
      }
#endif
      else
      {
        /* No other new serving cell event can be received when
        another serving cell event is existing. */
        WRRC_MSG1_ERROR("Invalid S-BCCH event rxed! in RRC st %d", rrc_state);
        rrcsib_clear_procedure
          (SIB_EVENT_FAILED_INVALID_EVENT); /* Clear existing procedure to be safe */
        bcch_status = RRCSIB_BCCH_SETUP_FAILED;
      }      
    }/* end if srv_cell != TRUE */
  }/* end if existing event is a srv cell event */
  else
  {
    /* Exisiting event must be a nighbor cell event.
    Check if the new event is for the neighbor cell. */
    if( srv_cell != TRUE )
    {
      WRRC_MSG0_ERROR("Reconfig: N-BCCH up, old N-BCCH down");  

      /* Make the current event inactive */
      sib_events_ptr->event[old_index].event_active = FALSE;

      /* Clear the scan type */
      sib_events_ptr->event[old_index].sib_scan_type = RRC_SIB_NORMAL;

      /* Send an LLC command. Tell it to take the S-BCCH down
      and bring up the N-BCCH for the new event. */

#ifdef FEATURE_RRC_NO_MIB_FOUND_BAR_CELL
      no_mib_bar_cell_count = 0;
#endif

      /*Initialize tsib optimization data base*/
      rrcsib_clear_tsib_optimization_db();
      //setup nbcch on top of old one.. llc implicitly releases old one
      if(rrcsib_configure_bcch(SBCCH_NOOP, NBCCH_SETUP
                ,RRCLLC_CHAN_CFG_REASON_NONE
            )== SUCCESS)
      {
        /* Copy the other BCCH parameters from the command */
        sib_events_ptr->ngh_bcch.freq = ptr->cmd.get_specific_sibs_req.freq;
        sib_events_ptr->ngh_bcch.scr_code = ptr->cmd.get_specific_sibs_req.scrambling_code;
      }
    }
    else
    {
      /* The new event is a serving cell event. Check if
      it's an RRCSIB_UPDATE_SIBS event. */
      if( ptr->cmd_hdr.cmd_id == RRC_UPDATE_SIBS_IND )
      {
        /* Flag it as a pending event. Don't do anything else
        now */
        update_sibs_event_pending = TRUE;
        /* Indicate to calling function that nothing needs to be
        done for the new event */
        bcch_status = RRCSIB_BCCH_EVENT_SAVED;
      }
      /* Else check if it is a GET_SPECIFIC_SIBS event for updating SIB7.
      If so, assume that SIB7 is current since the NBCCH event is higher 
      priority */
      else if( (ptr->cmd_hdr.cmd_id == RRC_GET_SPECIFIC_SIBS_REQ)&&
        (ptr->cmd.get_specific_sibs_req.procedure == RRC_PROCEDURE_SIB) &&
        (ptr->cmd.get_specific_sibs_req.for_serv_cell == TRUE)&&
        (ptr->cmd.get_specific_sibs_req.sib_req[rrc_SIB7]
          == TRUE)
        )
      {
        /* SIB7 update can be based on timer or external procedure request.
        Check for both and process accordingly */
        if(sib7info.sib_ext_proc_update_req == TRUE)
        {
          rrcsib_send_sib7_ind();
        }        /* Indicate to calling function that nothing needs to be
        done for the new event */
        bcch_status = RRCSIB_BCCH_EVENT_SAVED;
      }
      else
      {
        WRRC_MSG0_ERROR("Invalid S-BCCH event received!");
        rrcsib_clear_procedure
          (SIB_EVENT_FAILED_INVALID_EVENT); /* Clear existing events to be safe */
        bcch_status = RRCSIB_BCCH_SETUP_FAILED;
      }
    }/* end if srv_cell != TRUE */
  }/* end if existing event is N-BCCH event */

  return( bcch_status );
}/* end function bcch_reconfigure_for_new_sib_event */

/*===========================================================================

FUNCTION SIB_BCCH_SETUP_REQ

DESCRIPTION

  This function requests the LLC module to setup a BCCH logical
  channel for receiving SIBs. All BCCH parameters related to
  the SIB events are stored by this function.
  
  Only one serving cell BCCH or one neighbor cell BCCH can
  be up at the same time. This module checks if a
  BCCH is already up for the requested PLMN and cell id and
  then sets up the BCCH if necessary.

DEPENDENCIES

  None.
 
RETURN VALUE

  rrcsib_bcch_setup_status_e_type indicating status of
  BCCH setup.

SIDE EFFECTS

  Previous BCCH setups may need to be teared down. TBD.

===========================================================================*/
static rrcsib_bcch_setup_status_e_type sib_bcch_setup_req
(
  rrc_cmd_type *cmd_ptr          /* Received command */
)
{

  boolean srv_cell=FALSE; /* To note if the command is for the
                          serving cell or not. */
  boolean setup_error_occured = FALSE; /* Indicate if an error occured
                          for BCCH setup */

  /* Find out if the command is for the serving cell or the neighbour
  cell */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_UPDATE_SIBS_IND:
    
      /* RRC_UPDATE_SIBS_IND is always for serving cell */
      srv_cell = TRUE;
      break;

    case RRC_GET_SPECIFIC_SIBS_REQ:

      if( cmd_ptr->cmd.get_specific_sibs_req.for_serv_cell == TRUE )
      {
        srv_cell = TRUE;
      }
      else
      {
        srv_cell = FALSE;
      }
      break;

    default:

      WRRC_MSG1_ERROR("Unknown SIB command %d", cmd_ptr->cmd_hdr.cmd_id); 
      setup_error_occured = TRUE;
      break;
  }/* End switch */

  if( setup_error_occured == TRUE )
  {
    return(RRCSIB_BCCH_SETUP_FAILED);
  }

  /* Before doing any BCCH setup,
  check if any event is already in progress.
  If so we need to resolve the conflict between the existing
  and new events. We can only handle one at a time. This
  may also require a BCCH reconfig. Call the function to
  handle this. */

WRRC_MSG2_HIGH(" Serving cell event %d and LLC S-BCCH setup status %d ",
             sib_events_ptr->event[SIB_SRV_INDEX].event_active ,
             rrcllc_get_s_bcch_setup_status());

  if((sib_events_ptr->event[SIB_SRV_INDEX].event_active == TRUE) || 
     ((rrcllc_get_s_bcch_setup_status()==TRUE) && 
     (cmd_ptr->cmd_hdr.cmd_id == RRC_GET_SPECIFIC_SIBS_REQ) && 
     (cmd_ptr->cmd.get_specific_sibs_req.for_serv_cell == FALSE) && 
     (cmd_ptr->cmd.get_specific_sibs_req.procedure == RRC_PROCEDURE_CSP)))
  {
      /* There is an event active! */
      return(bcch_reconfigure_for_new_sib_event(cmd_ptr, srv_cell, SIB_SRV_INDEX));
      /* Only one event is active at a time so
          no need to continue the for loop if we found
          one active event. */

    }
  else if(sib_events_ptr->event[SIB_NGH_INDEX].event_active == TRUE)
    {
      /* There is an event active! */
      return(bcch_reconfigure_for_new_sib_event(cmd_ptr, srv_cell, SIB_NGH_INDEX));
      /* Only one event is active at a time so
         no need to continue the for loop if we found
         one active event. */
    
  }

  /* If we get here, a BCCH setup is yet to be done.
  Check if the required BCCH is already up. This is done
  by calling the logical channel manager. */
  if( srv_cell == TRUE )
  {
    /* Call the LCM module */
    if ( (rrclcm_check_dl_rlc_lc_id( UE_LOGCHAN_BCCH,
    BCCH_S_RADIO_BEARER_ID,
    UE_MODE_TRANSPARENT ))
    != RRCLCM_RLC_LC_ID_NOT_FOUND)
    {
#ifdef FEATURE_UPDATE_SIB7_IN_FACH
      if((rrc_get_state() == RRC_STATE_CELL_FACH)&&
               (RRC_CAMPED_ON == rrc_ccm_get_curr_camping_status(RRC_PROCEDURE_SIB))
              )
#endif
      {
        /*In RRC State FACH, LLC will keep always BCCH up, 
          so if LC i.d found, avoid sending a set up request to LLC */
        WRRC_MSG0_HIGH("BCCH already setup for srv cell");
        sib_events_ptr->srv_bcch.bcch_action =  SIB_BCCH_SETUP_COMPLETE;
        return(RRCSIB_BCCH_SETUP_DONE);
      }
      /* BCCH is already up. No need to tear it down and
      setup a new one. */
      WRRC_MSG0_HIGH("BCCH needs to be setup for srv cell");
      
    }
  }
  else
  {
    /* Call the LCM module */
    if( (rrclcm_check_dl_rlc_lc_id( UE_LOGCHAN_BCCH,
                                    BCCH_N_RADIO_BEARER_ID,
                                    UE_MODE_TRANSPARENT ))
        != RRCLCM_RLC_LC_ID_NOT_FOUND )
    {
      WRRC_MSG0_HIGH("BCCH already up for ngh cell");
      /* This should never happen since it's caught in
      the bcch_reconfigure_for_new_sib_event function.
      However, there may be a race condition causing
      this? Let's setup the N-BCCH channel anyway. */
    }
  }

  /* If we get here, a BCCH needs to be setup.
  Send a BCCH setup request command to LLC */
  /* Indicate which radio bearer is needed */
  if( srv_cell == TRUE )
  {
    rrcllc_chan_cfg_reason_e_type chan_config_reason = RRCLLC_CHAN_CFG_REASON_NONE;
    if(cmd_ptr->cmd_hdr.cmd_id == RRC_GET_SPECIFIC_SIBS_REQ)
    {
      if((cmd_ptr->cmd.get_specific_sibs_req.for_serv_cell == TRUE)&&
          (cmd_ptr->cmd.get_specific_sibs_req.procedure == RRC_PROCEDURE_GPS)
        )
      {
        /*If the SIB event active was cell id reading in DCH then set chan 
         *config reason to RRCLLC_CHAN_CFG_REASON_SIB_READ_IN_DCH 
         *which is used by LLC
         */
        chan_config_reason = RRCLLC_CHAN_CFG_REASON_SIB_READ_IN_DCH;
      }
    }

      (void)rrcsib_configure_bcch(SBCCH_SETUP, NBCCH_NOOP
            ,chan_config_reason);

    if(cmd_ptr->cmd_hdr.cmd_id != RRC_UPDATE_SIBS_IND)
    {
    /* Copy the necessary BCCH parameters from the command */
      sib_events_ptr->srv_bcch.freq = cmd_ptr->cmd.get_specific_sibs_req.freq;
      sib_events_ptr->srv_bcch.scr_code = cmd_ptr->cmd.get_specific_sibs_req.scrambling_code;
    }
  }
  else
  {
    (void)rrcsib_configure_bcch(SBCCH_NOOP, NBCCH_SETUP
        ,RRCLLC_CHAN_CFG_REASON_NONE
        );
    
    /* Copy the necessary BCCH parameters from the command */
    sib_events_ptr->ngh_bcch.freq = cmd_ptr->cmd.get_specific_sibs_req.freq;
    sib_events_ptr->ngh_bcch.scr_code = cmd_ptr->cmd.get_specific_sibs_req.scrambling_code;
  }

  return(RRCSIB_BCCH_SETUP_DONE);
}/* end function sib_bcch_setup_req */


/*===========================================================================

FUNCTION REGISTER_NEW_SIB_EVENT

DESCRIPTION

  This function registers a new sib event for the SIB processing module.
  It adds the event to the active events list and updates the active
  event database as needed. Then it sets up  or modifies the BCCH 
  as needed for this event.
  
DEPENDENCIES
 
  None.

RETURN VALUE
 
  None.

SIDE EFFECTS

  BCCH logical channel may be setup, teared down or modified to a new
  configuration.
       
===========================================================================*/
static void register_new_sib_event
(
  rrc_cmd_type *cmd_ptr   /* Pointer to received SIB event */
)
{
  rrcsib_bcch_setup_status_e_type bcch_status; /* Status of BCCH
                                               setup */
#ifdef FEATURE_UPDATE_SIB7_FOR_PCH_TO_FACH
  unsigned long timer_value;
#endif
  uint32 ix =0;
#ifdef FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
  rrc_SIBSb_ReferenceList *sibSb_ptr=NULL;
  rrc_SIB_ReferenceList *sb_ptr = NULL;
  rrcsib_sib_type_vtag sib_type_tag;
#endif

  uint32 rrc_sib_type[MAX_SEGMENTS_FOR_A_SIB] = {0};
  uint8 count = 0;


  /* First setup the BCCH channel required for this command.
  This function also checks if there is an existing SIB
  event and if so resolves the conflict between the events. */
  bcch_status = sib_bcch_setup_req( cmd_ptr );
  
  if( bcch_status == RRCSIB_BCCH_SETUP_FAILED )
  {
    WRRC_MSG0_HIGH("Sib event dropped: BCCH failure");
    return;
  }

  if( bcch_status == RRCSIB_BCCH_EVENT_SAVED )
  {
    /* Nothing to do here, this event won't be processed now */
    return;
  }

  /* Switch on the type of event that needs to be registered */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_UPDATE_SIBS_IND:
      /* Initialize the SIB event database for this event.
      Both "GET_ALL_SIBS" and "UPDATE_SIBS" events can be only
      for the serving cell. */
      create_new_sib_event( SIB_SRV_INDEX, cmd_ptr );

      /* For all new SIB events, we start a SIB wait timer. 
      This timer is the maximum time to wait for SIBs in a cell.
      In case of "UPDATE_SIBS_IND" event, the timer is longer
      since we may have to wait one SFN cycle before getting
      the modified MIB. */
      rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER,
                          RRCSIB_MAX_WAIT_TIME_FOR_UPDATE_SIBS );
      
      /* Nothing else to do here. Eventually L1 will complete the
      BCCH setup and we will get SIBs for this event. */
      break;

    case RRC_GET_SPECIFIC_SIBS_REQ:
      /* Check if this event is for the serving cell or neighbor
      cell, and initializethe SIB event database for this event. */
      if( cmd_ptr->cmd.get_specific_sibs_req.for_serv_cell == TRUE )
      {
        /* Create the event using the serving cell index */
        create_new_sib_event( SIB_SRV_INDEX, cmd_ptr );
      }
      else
      {
        /* Create the event using the neighbor cell index */
        create_new_sib_event( SIB_NGH_INDEX, cmd_ptr );
      }
      /* For all new SIB events, we start a SIB wait timer. 
      This timer is the maximum time to wait for SIBs in a cell.
      If the timer expires before the event is completed, a failure
      status is sent to the calling procedure. Start this timer
      right now  for BPLMN/N-Setup,  timer of only 300ms is set*/
      if( (cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN) ||
          (cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_WTOW_AUTO_BPLMN_SCAN) || 
          (cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN) || 
          (cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_MANUAL_SCAN) || 
          (cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_AUTO_BPLMN_SCAN))
      {
        rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER, RRCSIB_BPLMN_MIB_WAIT_TIMER);
      }
      else if(cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_CELL_ID_READ_IN_DCH_SCAN)
      {
        /*For sib_scan_type RRC_SIB_CELL_ID_READ_IN_DCH_SCAN, start sib wiat timer to 
         *640msec for now, one we receive mib this will be re-started based on sib-3 ret rate */
        rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER, RRCSIB_MAX_SIB3_WAIT_TIMER);
      }
      else if(cmd_ptr->cmd.get_specific_sibs_req.for_serv_cell == FALSE)
      {
        rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER, RRCSIB_NBCCH_MIB_WAIT_TIMER);
      }
      /*For SIB7 request by RCE procedure*/
      /*This code can be clubbed with following else if condition if feature is mainlined in future*/
      else if((cmd_ptr->cmd.get_specific_sibs_req.procedure== RRC_PROCEDURE_SIB)&&
              (rrc_get_state() == RRC_STATE_DISCONNECTED)
             )
      {
        timer_value = 3*rrcsib_get_sib7_exp_timer_value(sibdb.active_cell_index);
        rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER, 
                         timer_value );
      }
#ifdef FEATURE_UPDATE_SIB7_FOR_PCH_TO_FACH
      else if((cmd_ptr->cmd.get_specific_sibs_req.procedure== RRC_PROCEDURE_SIB)&&
             ((rrc_get_state() == RRC_STATE_CELL_PCH)||(rrc_get_state() == RRC_STATE_URA_PCH))&&
             (sib_events_ptr->event[SIB_SRV_INDEX].int_event.int_event_type == RRCSIB_INT_SIB7_EVENT)
             )
      {
        timer_value = rrcsib_get_sib7_exp_timer_value(sibdb.active_cell_index);
        WRRC_MSG1_HIGH("Starting SIB 7 timer %d",timer_value);
        rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER, 
                         timer_value );
      }
#endif
      else
      {
#ifdef FEATURE_UPDATE_SIB7_IN_FACH
        if((cmd_ptr->cmd.get_specific_sibs_req.procedure== RRC_PROCEDURE_SIB)&&
            (rrc_get_state() == RRC_STATE_CELL_FACH)&&
            (sib_events_ptr->event[SIB_SRV_INDEX].int_event.int_event_type == RRCSIB_INT_SIB7_EVENT)
          )
        {/*sib-7 event will be kept ON always in FACH, 
           So no need to run asib wait timer for this event*/
          WRRC_MSG0_HIGH("sib7:This is SIB7 update event in FACH, do not start SIB wait timer");
        }
        else
#endif
        {
          rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER, RRCSIB_MAX_WAIT_TIME_FOR_SIBS);

#ifdef FEATURE_DUAL_SIM
          if(g_in_data_call)
          {
            rrctmr_start_timer( RRCTMR_SIB_STOP_PARTITION_TIMER, 6000);
          }
#endif
        }
      }
      /* If the search is for WTW BPLMN,initialize the bplmn sibs variables */
       if(((cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_WTOW_AUTO_BPLMN_SCAN) 
        ||(cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)
#ifdef FEATURE_LTE_TO_WCDMA
        ||((cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN)
           &&(rrccsp_check_cgi_scan()))
#endif 
          )
        &&((cmd_ptr->cmd.get_specific_sibs_req.freq == rrcsib_bplmn_sibs.prev_freq) && 
          (cmd_ptr->cmd.get_specific_sibs_req.scrambling_code == rrcsib_bplmn_sibs.prev_scr)))
        {
          /* Free and initialize the bplmns sibs */
          if(rrcsib_bplmn_sibs.mib != NULL)
          {
            sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_MIB].sib_needed = TRUE;
            sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_MIB].sib_received = TRUE;
            sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_MIB].sib_decoded = TRUE;
            sib_events_ptr->event[SIB_SRV_INDEX].tempholder.mib_decoded = TRUE;
            sib_events_ptr->sib_sched[rrc_sib_type[rrc_MIB]].repitition_rate = 8;
            sib_events_ptr->sib_sched[rrc_sib_type[rrc_MIB]].sfn_offset[0] =0;
            sib_events_ptr->sib_sched[rrc_sib_type[rrc_MIB]].sfn_offset[1] =2;
            sib_events_ptr->sib_sched[rrc_sib_type[rrc_MIB]].sfn_offset[2] =4;
            sib_events_ptr->sib_sched[rrc_sib_type[rrc_MIB]].sfn_offset[3] =6;
#ifdef FEATURE_FEMTO_CSG
            if(CSG_IND_IN_MIB(rrcsib_bplmn_sibs.mib) &&
               is_whitelist_valid) 
            {
              sib_events_ptr->event[SIB_SRV_INDEX].is_csg = TRUE;
            }
#endif
#ifdef FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
            if((sib_events_ptr->event[SIB_SRV_INDEX].sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)||
               (sib_events_ptr->event[SIB_SRV_INDEX].sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN)
              )
            {
              sibSb_ptr = &rrcsib_bplmn_sibs.mib->sibSb_ReferenceList;
              for(ix=0;ix< sibSb_ptr->n;ix++)
              {
                rrcsib_get_sib_type_from_sibsb_type_and_tag(&sibSb_ptr->elem[ix].sibSb_Type,
                                                            &sib_type_tag);
                if(sib_type_tag.sib_type == rrc_SIB5 ||
                   sib_type_tag.sib_type == rrc_SIB5bis)
                {
                  break;
                }
              }
              if(((sibSb_ptr->n > ix ))&&
                ((sib_type_tag.sib_type == rrc_SIB5) ||
                (sib_type_tag.sib_type == rrc_SIB5bis)))
              {
                WRRC_MSG0_HIGH("BTD:-SIB5/SIB5bis found in MIB");
                sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_SB1].sib_needed = FALSE;
                sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_SB2].sib_needed = FALSE;
              }
            }
#endif
            rrcsib_update_sched_info_from_mib_sb((uint32)rrc_MIB, rrcsib_bplmn_sibs.mib, SIB_SRV_INDEX);
          }
          if(rrcsib_bplmn_sibs.sib1!= NULL)
          {
            sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_SIB1].sib_needed = TRUE;
            sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_SIB1].sib_received = TRUE;
            sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_SIB1].sib_decoded = TRUE;
          }
          if(rrcsib_bplmn_sibs.sib3!= NULL)
          {
            sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_SIB3].sib_needed = TRUE;
            sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_SIB3].sib_received = TRUE;
            sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_SIB3].sib_decoded = TRUE;
            sib_events_ptr->event[SIB_SRV_INDEX].tempholder.sib3_decoded = TRUE;
#ifdef FEATURE_FEMTO_CSG
            if(CSG_ID_IN_SIB3(rrcsib_bplmn_sibs.sib3) &&
               (is_whitelist_valid))
            {
              sib_events_ptr->event[SIB_SRV_INDEX].csg_id = rrc_translate_cell_id
                              ( &(rrcsib_bplmn_sibs.sib3->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                                  v5c0NoncriticalExtension.v670NonCriticalExtension.v770NonCriticalExtension.
                                  v830NonCriticalExtension.v860NonCriticalExtension.sysInfoType3_v860ext.csgIdentity));
              WRRC_MSG1_HIGH("CSG: CSG Id is %d",sib_events_ptr->event[SIB_SRV_INDEX].csg_id);
            }
#endif
          }
          if(rrcsib_bplmn_sibs.sb1!= NULL)
          {
            sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_SB1].sib_needed = TRUE;
            sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_SB1].sib_received = TRUE;
            sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_SB1].sib_decoded = TRUE;
            sib_events_ptr->event[SIB_SRV_INDEX].tempholder.sb1_decoded = TRUE;
#ifdef FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
            if((sib_events_ptr->event[SIB_SRV_INDEX].sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)||
               ((sib_events_ptr->event[SIB_SRV_INDEX].sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN)
#ifdef FEATURE_LTE_TO_WCDMA
             &&(!rrccsp_check_cgi_scan())
#endif               
              ))
            {
              sb_ptr = &rrcsib_bplmn_sibs.sb1->sib_ReferenceList;
              ix=0;
              for(ix=0;ix< sb_ptr->n;ix++)
              {
                rrcsib_get_sib_type_from_sib_type_and_tag(&sb_ptr->elem[ix].sib_Type,
                                                      &sib_type_tag);
                if(sib_type_tag.sib_type == rrc_SIB5 ||
                    sib_type_tag.sib_type == rrc_SIB5bis)
                {
                  break;
                }
              }
              if((ix < sb_ptr->n)&&
                ((sib_type_tag.sib_type == rrc_SIB5)||
                  (sib_type_tag.sib_type == rrc_SIB5bis)))
              {
                WRRC_MSG0_HIGH("BAND_TYPE_DETECTION - SIB5/SIB5bis scheduled in SB1 ");
                sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_SB2].sib_needed = FALSE;
              }
            }
#endif
            rrcsib_update_sched_info_from_mib_sb((uint32)rrc_SB1, rrcsib_bplmn_sibs.sb1, SIB_SRV_INDEX);
          }
          if(rrcsib_bplmn_sibs.sb2!= NULL)
          {
            sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_SB2].sib_needed = TRUE;
            sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_SB2].sib_received = TRUE;
            sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_SB2].sib_decoded = TRUE;
            sib_events_ptr->event[SIB_SRV_INDEX].tempholder.sb2_decoded = TRUE;
#ifdef FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
            if((sib_events_ptr->event[SIB_SRV_INDEX].sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)||
               ((sib_events_ptr->event[SIB_SRV_INDEX].sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN)
#ifdef FEATURE_LTE_TO_WCDMA
             &&(!rrccsp_check_cgi_scan())
#endif               
              ))
            {
              sb_ptr = &rrcsib_bplmn_sibs.sb2->sib_ReferenceList;
              ix=0;
              for(ix=0;ix< sb_ptr->n;ix++)
              {
                rrcsib_get_sib_type_from_sib_type_and_tag(&sb_ptr->elem[ix].sib_Type,
                                                      &sib_type_tag);
                if(sib_type_tag.sib_type == rrc_SIB5 ||
                    sib_type_tag.sib_type == rrc_SIB5bis)
                {
                  break;
                }
              }
              if((sb_ptr->n > ix)&&
                ((sib_type_tag.sib_type == rrc_SIB5)||
                  (sib_type_tag.sib_type == rrc_SIB5bis)))
              {
                WRRC_MSG0_HIGH("BAND_TYPE_DETECTION - SIB5/SIB5bis scheduled in SB2 ");
                sib_events_ptr->event[SIB_SRV_INDEX].tsib[rrc_SB1].sib_needed = FALSE;
              }
            }
#endif
            rrcsib_update_sched_info_from_mib_sb((uint32)rrc_SB2, rrcsib_bplmn_sibs.sb2, SIB_SRV_INDEX);
          }
          if(rrc_csp_int_data.remaining_time_during_bplmn != 0)
          {
            rrctmr_stop_timer( RRCTMR_SIB_WAIT_TIMER );
            rrctmr_start_timer( RRCTMR_SIB_WAIT_TIMER,
                                    rrc_csp_int_data.remaining_time_during_bplmn);
          }
           
        }
        else
        {
          rrcsib_free_bplmn_sibs();
          if((cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_WTOW_AUTO_BPLMN_SCAN) 
           ||(cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)
#ifdef FEATURE_LTE_TO_WCDMA
           ||((cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN)
           &&(rrccsp_check_cgi_scan()))
#endif 
           )
          {
            rrcsib_bplmn_sibs.prev_freq = cmd_ptr->cmd.get_specific_sibs_req.freq ;
            rrcsib_bplmn_sibs.prev_scr = cmd_ptr->cmd.get_specific_sibs_req.scrambling_code;
          }
        }

      /*Freeing decoded SIBs if GET_SPECIFIC_SIB_REQ for serving cell
             is received without camping after successful SIB reading, in order
             to avoid exhaustion of decoded contexts*/
      if(((cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_NORMAL)
#ifdef FEATURE_3GPP_CSFB
         || (cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_READ_FROM_SIB_CONTAINER)
#endif
         ) && (cmd_ptr->cmd.get_specific_sibs_req.for_serv_cell == TRUE) && 
         (cmd_ptr->cmd.get_specific_sibs_req.procedure == RRC_PROCEDURE_CSP))
      {
        for(count = 0; count <(uint8)SIBDB_INVALID_INDEX; count++)
        {
          if(sibdb.cell[count].index_used == TRUE)
          {
            WRRC_MSG1_HIGH("Free ASN.1 decoded sib's for sibdb idx= %d", count);
            rrc_sibdb_free_all_decodedsibs_for_a_cell(count);
          }
        }
#ifdef FEATURE_3GPP_CSFB
        /* Freeing the decoded system info container PDU when GET_SPECIFIC_SIB event is triggered 
            by CSP(cell selection/reselection) */
        if(NULL != sibdb.rrc_decoded_sib_cont)
        {
          rrcasn1_free_pdu(rrc_System_Information_Container_PDU,(void *)sibdb.rrc_decoded_sib_cont);
          sibdb.rrc_decoded_sib_cont = NULL;

          /* Clearing the SIB DB index as the information inside as not used anymore */
          rrc_sibdb_clear_a_cell_index(sibdb.sib_cont_sibdb_index);
          sibdb.sib_cont_sibdb_index = SIBDB_INVALID_INDEX;
        }
#endif
      }

      break;

    default:
      WRRC_MSG1_ERROR(" Unknown SIB event registered %x",cmd_ptr->cmd_hdr.cmd_id);  
      break;
  }
}/* end function register_new_sib_event */

/*===========================================================================

FUNCTION rrcsib_check_if_scheduling_info_received_for_extended_sibs

DESCRIPTION

  This function walks thru the MIB/SB1/SB2 and checks to see if scheduling
  information is received for  extended sib's.  
DEPENDENCIES
 
  None.

RETURN VALUE

  SUCCESS: If scheduling information is received for extended sib's
  FAILURE: If scheduling information is not received for extended sib's

SIDE EFFECTS
  
===========================================================================*/
boolean rrcsib_check_if_scheduling_info_received_for_extended_sibs
(
  void * sib_ptr,           /* ptr to mib/sb1/sb2                     */
  rrc_SIB_type pdu_num,     /* Type of SIB - can be MIB, SB1 or SB2   */
  uint32 index
)
{
  rrc_MasterInformationBlock *mib_ptr; /* Local pointer to MIB       */
  rrc_SysInfoTypeSB1 *sb1_ptr;         /* Local pointer to SB1       */
  rrc_SysInfoTypeSB2 *sb2_ptr;         /* Local pointer to SB2       */
  boolean status = FALSE;
  rrc_ExtSIBTypeInfoSchedulingInfo_List *ext_siblist_ptr = NULL;
  uint32 n_index=0;

  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6)
  {
    switch(pdu_num)
    {
      //if the updating pdu is MIB, get the sib type and later update the
      //scheduling information
      case rrc_MIB:
        mib_ptr = (rrc_MasterInformationBlock *)sib_ptr;
        if(EXTENSION_SIB_SCHED_PRESENT_IN_MIB(mib_ptr))
        {
          ext_siblist_ptr = &mib_ptr->v690NonCriticalExtensions.
            v6b0NonCriticalExtensions.masterInformationBlock_v6b0ext.extSIBTypeInfoSchedulingInfo_List;
        }
        break;
        
      //if the updating pdu is SB1 or SB2, get the sib type and later update the
      //scheduling information
      case rrc_SB1:
      case rrc_SB2:
        if(TRUE == sib_events_ptr->event[index].sched_present_for_sibs[rrc_MIB])
        {
          if( pdu_num == rrc_SB1 )
          {
            sb1_ptr = (rrc_SysInfoTypeSB1 *)sib_ptr;
            if(EXTENSION_SIB_SCHED_PRESENT_IN_SB1_SB2(sb1_ptr,
                 rrc_SysInfoTypeSB1,sysInfoTypeSB1))
            {
              ext_siblist_ptr = &sb1_ptr->v6b0NonCriticalExtensions.
            sysInfoTypeSB1_v6b0ext.extSIBTypeInfoSchedulingInfo_List;
            }
          }
          else if( pdu_num == rrc_SB2 ) 
          {
            sb2_ptr = (rrc_SysInfoTypeSB2 *)sib_ptr;
            if(EXTENSION_SIB_SCHED_PRESENT_IN_SB1_SB2(sb2_ptr,
                 rrc_SysInfoTypeSB2,sysInfoTypeSB2))
            {
              ext_siblist_ptr = &sb2_ptr->v6b0NonCriticalExtensions.
            sysInfoTypeSB2_v6b0ext.extSIBTypeInfoSchedulingInfo_List;
            }
          }
        }
        //first MIB has to be decoded, until then do not process sb's
        else
        {
          WRRC_MSG0_ERROR("MIB not decoded first");
        }
        break;
     
      default:
        WRRC_MSG1_ERROR("Unknown Sib-Sched Type %d", pdu_num);
        break;
    }
   
    if(ext_siblist_ptr != NULL)
    { 
      n_index = 0;
      while( ext_siblist_ptr->n > n_index)
      {
        if( RRCCHECK_SIB_TypeExt(
                (&(ext_siblist_ptr->elem[n_index].extensionSIB_Type)),
                 systemInfoType11bis)
        )
        {
          WRRC_MSG0_HIGH("sib11bis:Sched present for sib11bis ");
          status = TRUE;
        }
        else
        {
          WRRC_MSG1_ERROR("sib11bis:Extension sib type = %x not supported", 
          RRCGET_SIB_TYPE_AND_TAG((&ext_siblist_ptr->elem[n_index].extensionSIB_Type)));
        }
        n_index++;
      }
    }
    else
    {
      status = FALSE;
    }
  }
  else
  {
    status = FALSE;
  }
  return status;
}

#ifdef FEATURE_WCDMA_TO_LTE
/*===========================================================================

FUNCTION rrcsib_check_if_scheduling_info_received_for_sib19

DESCRIPTION

  This function walks thru the MIB/SB1/SB2 and checks to see if scheduling
  information is received for SIB19.  
DEPENDENCIES
 
  None.

RETURN VALUE

  SUCCESS: If scheduling information is received for SIB19
  FAILURE: If scheduling information is not received for SIB19

SIDE EFFECTS
  
===========================================================================*/
boolean rrcsib_check_if_scheduling_info_received_for_sib19
(
  void * sib_ptr,           /* ptr to mib/sb1/sb2                     */
  rrc_SIB_type pdu_num,     /* Type of SIB - can be MIB, SB1 or SB2   */
  uint32 index
)
{
  rrc_MasterInformationBlock *mib_ptr; /* Local pointer to MIB       */
  rrc_SysInfoTypeSB1 *sb1_ptr;         /* Local pointer to SB1       */
  rrc_SysInfoTypeSB2 *sb2_ptr;         /* Local pointer to SB2       */
  boolean status = FALSE;
  rrc_ExtSIBTypeInfoSchedulingInfo_List2 *ext_siblist2_ptr = NULL;
  uint32 n_index=0;

  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6)
  {
    switch(pdu_num)
    {
      //if the updating pdu is MIB, get the sib type and later update the
      //scheduling information
      case rrc_MIB:
        mib_ptr = (rrc_MasterInformationBlock *)sib_ptr;
        if((mib_ptr->m.v690NonCriticalExtensionsPresent) &&
           (mib_ptr->v690NonCriticalExtensions.m.v6b0NonCriticalExtensionsPresent) &&
           (mib_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) &&
           (mib_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.v860NonCriticalExtensions.
             masterInformationBlock_v860ext.m.extSIBTypeInfoSchedulingInfo_ListPresent))
        {
          ext_siblist2_ptr = &mib_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.
            v860NonCriticalExtensions.masterInformationBlock_v860ext.extSIBTypeInfoSchedulingInfo_List;
        }
        break;
        
      //if the updating pdu is SB1 or SB2, get the sib type and later update the
      //scheduling information
      case rrc_SB1:
      case rrc_SB2:
        if(TRUE == sib_events_ptr->event[index].sched_present_for_sibs[rrc_MIB])
        {
          if( pdu_num == rrc_SB1 )
          {
            sb1_ptr = (rrc_SysInfoTypeSB1 *)sib_ptr;
            if((sb1_ptr->m.v6b0NonCriticalExtensionsPresent) &&
               (sb1_ptr->v6b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) &&
               (sb1_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.sysInfoTypeSB1_v860ext.
                   m.extSIBTypeInfoSchedulingInfo_ListPresent))
            {
              ext_siblist2_ptr = &sb1_ptr->v6b0NonCriticalExtensions.
                v860NonCriticalExtensions.sysInfoTypeSB1_v860ext.extSIBTypeInfoSchedulingInfo_List;
            }
          }
          else if( pdu_num == rrc_SB2 ) 
          {
            sb2_ptr = (rrc_SysInfoTypeSB2 *)sib_ptr;
            if((sb2_ptr->m.v6b0NonCriticalExtensionsPresent) &&
               (sb2_ptr->v6b0NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) &&
               (sb2_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.
                  sysInfoTypeSB2_v860ext.m.extSIBTypeInfoSchedulingInfo_ListPresent))
            {
              ext_siblist2_ptr = &sb2_ptr->v6b0NonCriticalExtensions.
            v860NonCriticalExtensions.sysInfoTypeSB2_v860ext.extSIBTypeInfoSchedulingInfo_List;
            }
          }
        }
        //first MIB has to be decoded, until then do not process sb's
        else
        {
          WRRC_MSG0_ERROR("MIB not decoded first");
        }
        break;
     
      default:
        WRRC_MSG1_ERROR("Unknown Sib-Sched Type %d", pdu_num);
        break;
    }
    
    if(ext_siblist2_ptr != NULL)
    {
      n_index = 0;
      while(ext_siblist2_ptr->n > n_index)
      {
        if(ext_siblist2_ptr->elem[n_index].extensionSIB_Type2.t &
            T_rrc_SIB_TypeExt2_systemInfoType19)
        {
          WRRC_MSG0_HIGH("sib19:Sched present for sib19 ");
          status = TRUE;
        }
        else
        {
          WRRC_MSG1_ERROR("sib19:Extension sib type = %x not supported", 
               ext_siblist2_ptr->elem[n_index].extensionSIB_Type2.t);
        }
        n_index++;
      }
    }
    else
    {
      status = FALSE;
    }
  }
  else
  {
    status = FALSE;
  }
  
  return status;
}
#endif


/*===========================================================================

FUNCTION rrcsib_check_if_scheduling_info_received_for_all_sibs

DESCRIPTION

  This function walks thru the MIB and checks to see if scheduling
  information is received for all mandatory sib's.  
DEPENDENCIES
 
  None.

RETURN VALUE

  SUCCESS: If scheduling information is received for all mandatory sib's
  FAILURE: If scheduling information is not received for all mandatory sib's

SIDE EFFECTS
  
===========================================================================*/
static uecomdef_status_e_type rrcsib_check_if_scheduling_info_received_for_all_sibs
(
  void * sib_ptr,           /* ptr to mib/sb1/sb2                     */
  rrc_SIB_type pdu_num,     /* Type of SIB - can be MIB, SB1 or SB2   */
  sib_events_index_e_type index
)
{
  rrc_MasterInformationBlock *mib_ptr; /* Local pointer to MIB       */
  rrc_SysInfoTypeSB1 *sb1_ptr;         /* Local pointer to SB1       */
  rrc_SysInfoTypeSB2 *sb2_ptr;         /* Local pointer to SB2       */

  rrc_SIBSb_ReferenceList *sibSb_ptr = NULL; /* Local pointer to SIBSb ref list */
  rrc_SIB_ReferenceList *sb_ptr=NULL;        /* Local pointer to SIB ref list   */

  rrcsib_sib_type_vtag sib_type_tag;
  boolean sb1_found, sb2_found;      /* Local flags to keep track of sb's  */
  uint32 n_index=0, n_value=0;

  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return FAILURE;
  }

  switch(pdu_num)
  {
    //if the updating pdu is MIB, get the sib type and later update the
    //scheduling information
    case rrc_MIB:
      mib_ptr = (rrc_MasterInformationBlock *)sib_ptr;

      sibSb_ptr = &(mib_ptr->sibSb_ReferenceList);
  
      //set to true for MIB, as no scheduling info is received for mib
      sib_events_ptr->event[index].sched_present_for_sibs[rrc_MIB] = TRUE;

      n_index = 0;
      n_value = sibSb_ptr->n;
      
      while( n_value != 0 )
      {
        rrcsib_get_sib_type_from_sibsb_type_and_tag(&sibSb_ptr->elem[n_index].sibSb_Type,
                                                    &sib_type_tag);
        if(sib_type_tag.sib_type != rrc_MAX_SIB)
        {
          sib_events_ptr->event[index].sched_present_for_sibs[sib_type_tag.sib_type] = TRUE;
        }
        n_value--;
        n_index++;
      }
      break;

    //if the updating pdu is SB1 or SB2, get the sib type and later update the
    //scheduling information
    case rrc_SB1:
    case rrc_SB2:

      if(TRUE == sib_events_ptr->event[index].sched_present_for_sibs[rrc_MIB])
      {
        if( pdu_num == rrc_SB1 )
        {
          sb1_ptr = (rrc_SysInfoTypeSB1 *)sib_ptr;
          sb_ptr = &(sb1_ptr->sib_ReferenceList);
        }
        else if( pdu_num == rrc_SB2 ) 
        {
          sb2_ptr = (rrc_SysInfoTypeSB2 *)sib_ptr;
          sb_ptr = &(sb2_ptr->sib_ReferenceList);
        }

        n_index = 0;
        n_value = sb_ptr->n;

        while( n_value != 0 )
        {

          rrcsib_get_sib_type_from_sib_type_and_tag(&sb_ptr->elem[n_index].sib_Type,
                                                      &sib_type_tag);

          if(sib_type_tag.sib_type != rrc_MAX_SIB)
          {
            sib_events_ptr->event[index].sched_present_for_sibs[sib_type_tag.sib_type] = TRUE;
          }
          n_value--;
          n_index++;
        }
      }
      //first MIB has to be decoded, until then do not process sb's
      else
      {
        WRRC_MSG0_ERROR("MIB not decoded first");
      }
      break;

    default:
      WRRC_MSG1_ERROR("Unknown Sib-Sched Type %d", pdu_num);
      break;
  }

  if(rrcsib_check_if_scheduling_info_received_for_extended_sibs(
           sib_ptr,pdu_num,index) == TRUE)
  {
    WRRC_MSG0_HIGH("sib11bis:Sched present for sib11bis");
    sib_events_ptr->event[index].sched_present_for_sibs[rrc_SIB11bis] = TRUE;
  }

#ifdef FEATURE_WCDMA_TO_LTE
  if(rrcsib_check_if_scheduling_info_received_for_sib19(
           sib_ptr,pdu_num,index) == TRUE)
  {
    WRRC_MSG0_HIGH("sib19:Sched present for extended sib");
    sib_events_ptr->event[index].sched_present_for_sibs[rrc_SIB19] = TRUE;
  }
#endif

  /* check to see if MIB is received first, else don't check to see if all SIB's are received */
  if(TRUE == sib_events_ptr->event[index].sched_present_for_sibs[rrc_MIB])
  {
    /* if MIB is received, check if SB1 is scheduled or not, if scheduled, then check if
     * received. if not scheduled then indicate that sb1 is found.do similar stuff for
     * sb2 as below
     */
    if(sib_events_ptr->event[index].sched_present_for_sibs[rrc_SB1] == TRUE)
    {
      if(TRUE == sib_events_ptr->event[index].tempholder.sb1_decoded)
      {
        sb1_found = TRUE;
      }
      else
      {
        sb1_found = FALSE;
      }
    }
    else
    {
      sb1_found = TRUE;
    }

    if(TRUE == sib_events_ptr->event[index].sched_present_for_sibs[rrc_SB2])
    {
      if(TRUE == sib_events_ptr->event[index].tempholder.sb2_decoded)
      {
        sb2_found = TRUE;
      }
      else
      {
        sb2_found = FALSE;
      }
    }
    else
    {
      sb2_found = TRUE;
    }

    if(sb1_found && sb2_found)
    {
      if(!sib_events_ptr->event[index].sched_present_for_sibs[rrc_SIB1] ||
         !sib_events_ptr->event[index].sched_present_for_sibs[rrc_SIB3] ||
         !sib_events_ptr->event[index].sched_present_for_sibs[rrc_SIB5] ||
         !sib_events_ptr->event[index].sched_present_for_sibs[rrc_SIB7])
      {
        WRRC_MSG0_ERROR("Sched for mand. sibs not present");
        return FAILURE;
      }
    }
  }
  else
  {
    WRRC_MSG0_ERROR("MIB not decoded first");
  }

  return SUCCESS;

}
/*===========================================================================

FUNCTION RRCSIB_PROCESS_SIB_SCHEDULING_INFO_INCOMPLETE

DESCRIPTION

  This function does the necessary processing when 
  the SIB we don't receiving scheduling information
  for all the SIB's that are mandatorily needed to
  camp on

DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  BCCH logical channels may be teared down. 
===========================================================================*/
static void rrcsib_process_sib_scheduling_info_incomplete( sib_events_index_e_type event_index )
{
  sib_events_index_e_type count = event_index;          /* Local counter */

  rrccsp_send_cphy_cell_bar_req(RRC_CSP_WCDMA_PSC_BARRED, 1280);

  /* Check if any event is active */
  for( count = SIB_SRV_INDEX; count < MAX_SIB_EVENTS_INDEX; count++ )
  {
    if( sib_events_ptr->event[count].event_active == TRUE )
    {
      /* An event was active. Since the timer expired,
      this event failed. */
      rrcsib_post_process_failed_event
        ( count, /* Event Index                 */
          TRUE,  /* Confirm Command is required */
          SIB_EVENT_FAILED_TIMEOUT, /* Timed out*/
          TRUE   /* Tear down S_BCCH if it's up */
        );
      break; /* Out of the for loop since only one event is active
             at a time. */
    }/* End if active events */
  }/* End for */
}/* End rrcsib_process_sib_scheduling_info_incomplete */
/*===========================================================================

FUNCTION RRCSIB_LOG_SYS_INF

DESCRIPTION

  This function logs the given SIBs in QXDM
  camp on

DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

===========================================================================*/
static void rrcsib_log_sys_inf
(
uint32 dbindex,
uint32 sib_type
)
{  
  uint32 log_record_length;                 /* Local variable to store Log 
                                               Record Length */
  WCDMA_SIGNALLING_MESSAGE_type *log_record_ptr;
                                            /* Local variable to store Log
                                               record */

  uint8 msg_type;
  byte *sib_encoded_ptr;
  uint32 sib_encoded_length;
  uint32 dl_sdu_len = 0;
  uint8 rb_id = BCCH_S_RADIO_BEARER_ID;
  uint8 chan_type = 254;

  switch(sib_type)
  {
    case (uint32)rrc_MIB:
      msg_type = RRCLOG_MIB;
      sib_encoded_ptr = sibdb.cell[dbindex].sibs.mib_encoded_ptr;
      sib_encoded_length = sibdb.cell[dbindex].sibs.mib_encoded_pdu_length;
      break;
    
    case (uint32)rrc_SIB1:
      msg_type = RRCLOG_SIB_1;
      sib_encoded_ptr = sibdb.cell[dbindex].sibs.sib1_encoded_ptr;
      sib_encoded_length = sibdb.cell[dbindex].sibs.sib1_encoded_pdu_length;
      break;
    
    case (uint32)rrc_SIB3:
      msg_type = RRCLOG_SIB_3;
      sib_encoded_ptr = sibdb.cell[dbindex].sibs.sib3_encoded_ptr;
      sib_encoded_length = sibdb.cell[dbindex].sibs.sib3_encoded_pdu_length;
      break;

    case (uint32)rrc_SIB5:
      msg_type = RRCLOG_SIB_5;
      sib_encoded_ptr = sibdb.cell[dbindex].sibs.sib5_encoded_ptr;
      sib_encoded_length = sibdb.cell[dbindex].sibs.sib5_encoded_pdu_length;
      break;

    case (uint32)rrc_SIB11:
      msg_type = RRCLOG_SIB_11;
      sib_encoded_ptr = sibdb.cell[dbindex].sibs.sib11_encoded_ptr;
      sib_encoded_length = sibdb.cell[dbindex].sibs.sib11_encoded_pdu_length;
      break;

    case (uint32)rrc_SIB12:
      msg_type = RRCLOG_SIB_12;
      sib_encoded_ptr = sibdb.cell[dbindex].sibs.sib12_encoded_ptr;
      sib_encoded_length = sibdb.cell[dbindex].sibs.sib12_encoded_pdu_length;
      break;

    default:
      return;
  }

  dl_sdu_len = sib_encoded_length + 1 ;

  /*Calculate the record length */
  log_record_length = 
    FPOS(WCDMA_SIGNALLING_MESSAGE_type, signalling_message) + dl_sdu_len;

  /*Allocate memory for Log Record. */
  log_record_ptr = (WCDMA_SIGNALLING_MESSAGE_type *)log_alloc( WCDMA_SIGNALLING_MESSAGE, log_record_length);
  
  /* Make sure memory allocation is successful before to proceed with 
     logging */
  if(log_record_ptr != NULL)
  {
    /* Update RB Id.*/
    log_record_ptr->rb_id = rb_id;

    /*Update Logical Channel Type */
    log_record_ptr->channel_type = chan_type;

    /* Update length of the signalling message */
    log_record_ptr->length = dl_sdu_len;

    log_record_ptr->signalling_message[0] = msg_type;

    /*commit the log record */
    WCDMA_MEMCPY((void *)&(log_record_ptr->signalling_message[1]),
                  sib_encoded_length,
                  (void *)(sib_encoded_ptr),
                  sib_encoded_length);
    log_commit(log_record_ptr);   
  }
  else
  {
    WRRC_MSG0_HIGH("log_record_ptr is NULL");
  }

} /* end function rrcsib_log_sys_inf */
/*===========================================================================

FUNCTION  UPDATE_EVENT_FOR_RECEIVED_VALUE_TAGS

DESCRIPTION

  This function updates a SIB event based on value-tag
  comparisons for the received value-tags. The value-tags
  may be from a MIB, a Scheduling Block 1 or a Scheduling
  Block 2.

DEPENDENCIES

  None.
  
RETURN VALUE
  
  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void update_event_for_received_value_tags
(
  rrc_SIB_type sib,     /* Type of SIB - can be MIB, SB1 or SB2   */
  sib_events_index_e_type event_index, /* Index for the SIB event */
  uint32 db_index       /* SIB Database index for the stored SIBs */
)
{
  /* Declare a variable to store the results from value tag
  comparisons */
  rrc_sibdb_value_tag_results_type result;
  rrc_SysInfoTypeSB1         *sb1 = NULL;
  rrc_SysInfoTypeSB2         *sb2 = NULL;
  boolean check_results=FALSE; /* Indicate if results need
  to be checked - only set to true if SIBs have already
  been stored in the database for this cell */
  rrc_sib_change_mask_type  gen_mask_changed=0x0, gen_mask_unchanged=0x0;
  uint32 count;  /* Local counters */

  rrcsib_vtag_match_type tpye_of_vtag_match;

  if(EVENT_INDEX_VALID(event_index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return;
  }

  /* Initialize result to keep Lint happy. This will be filled
  in by the sibdb module when comparing value-tags */
  for( count=0; count<RRC_TOT_SIBS; count++ )
  {
    result.result[count] = VALUE_TAG_NOT_PRESENT;
  }

  /* Switch on the sib type to compare value-tags. */
  switch( sib )
  {
    case rrc_MIB:

     /* Value-tags need to be compared with tags already
     in the database for two reasons -
      
     1) If the current cell already has matching SIBs
     stored in the database due to a previous camp-on
     at this cell, we have to compare
     the received value tags. The SIB database has
     a variable "last_plmn_and_cell_id_matched to
     indicate if this is the case.
      
     2) If we've camped on a cell for a while and
     we get indication that the MIB valuetags have
     changed, then we need to compare the new MIB's
     value-tags with the saved MIB's. */

     if( ((sibdb.last_plmn_and_cell_id_matched == TRUE) ||
         (sib_events_ptr->event[event_index].event_name == RRCSIB_UPDATE_SIBS ))
       )
     {
       /* Call the SIB Database function that does value-tag
       comparisons */
       if((rrcsib_is_event_sixhrs_sib_update(event_index) == TRUE)||
           (rrcsib_is_sixhrs_sib_timer_expired(event_index) == TRUE)
         )
       {
         tpye_of_vtag_match = RRCSIB_READ_ALL_SIBS;
       }
       else
       {
         tpye_of_vtag_match = RRCSIB_READ_SIBS_AFTER_VTAG_MATCH;
       }

       rrc_sibdb_compare_value_tags_for_cell
         ( db_index,
           sib,
           ((void *)sib_events_ptr->event[event_index].tempholder.mib),
           (&result),
       /*Need a flag to indicate forcibly re-read all sibs, only if the event is 
         UPDATE_SIBs & six hr timer expired*/
            tpye_of_vtag_match,
            event_index
         );
       check_results=TRUE;

       /* whether mib value tag matches or not, always set the SIB7 result to 
          value-tag-not-matched..  this is needed, because:
          * if MIB-Value-TAG matches SIB-DB declares that all SIB's are updated          
          * but, SIB7 does not have a value-tag and hence cannot be update
  
          this is done for:
          * GET_ALL_SIBS always since we need updated SIB7
          * GET_SPECIFIC_SIBS in CELL-FACH to take care of cell-reselections in FACH
  
        */
       if((sib_events_ptr->event[event_index].event_name == RRCSIB_GET_SPECIFIC_SIBS) &&
           (rrc_get_state() == RRC_STATE_CELL_FACH))
       {
         if(sib_events_ptr->event[event_index].tsib[rrc_SIB7].sib_received == TRUE)
         {
           result.result[rrc_SIB7] = TIMER_BASED_SIB_RCVD;
         }
         else
         {
           result.result[rrc_SIB7] = VALUE_TAG_DID_NOT_MATCH;
         }
       }
     }
     break;

    case rrc_SB1:
  
     /*Missing logic. Already taken care for MIB*/
      if(((sibdb.last_plmn_and_cell_id_matched == TRUE) ||
         (sib_events_ptr->event[event_index].event_name == RRCSIB_UPDATE_SIBS ))
        )
      {
        if((rrcsib_is_event_sixhrs_sib_update(event_index) == TRUE)||
             (rrcsib_is_sixhrs_sib_timer_expired(event_index) == TRUE)
           )
        {
          tpye_of_vtag_match = RRCSIB_READ_ALL_SIBS;
        }
        else
        {
          tpye_of_vtag_match = RRCSIB_READ_SIBS_AFTER_VTAG_MATCH;
        }

        rrc_sibdb_compare_value_tags_for_cell
          ( db_index,
            sib,
            ((void *)sibdb.cell[db_index].sibs.sb1),
          (&result),
            /*Need a flag to indicate forcibly re-read all sibs, only if the event is 
              UPDATE_SIBs & six hr timer expired*/
            tpye_of_vtag_match,
            event_index
          );

        check_results=TRUE; 
      }
      break;

    case rrc_SB2:

      if( ((sibdb.last_plmn_and_cell_id_matched == TRUE) ||
          (sib_events_ptr->event[event_index].event_name == RRCSIB_UPDATE_SIBS ))
        )
      {

        if((rrcsib_is_event_sixhrs_sib_update(event_index) == TRUE)||
             (rrcsib_is_sixhrs_sib_timer_expired(event_index) == TRUE)
           )
        {
          tpye_of_vtag_match = RRCSIB_READ_ALL_SIBS;
        }
        else
        {
          tpye_of_vtag_match = RRCSIB_READ_SIBS_AFTER_VTAG_MATCH;
        }

        rrc_sibdb_compare_value_tags_for_cell
          ( db_index,
            sib,
            ((void *)sibdb.cell[db_index].sibs.sb2),
          (&result),
            /*Need a flag to indicate forcibly re-read all sibs, only if the event is 
              UPDATE_SIBs & six hr timer expired*/
            tpye_of_vtag_match,
            event_index
          );

        check_results=TRUE;
      }
      break;

    default:
      
      WRRC_MSG1_HIGH("Invalid type for value-tag comparison %d", rrcsib_map_int_sibtype_to_asn1_sibtype(sib));
      break;
  }/* end switch */

  if( check_results == TRUE )
  {
    /* Now, check the results from the value-tag comparison.
    If any of the value-tags match, we already have th current
    SIBs for this cell and don't need to read them again. Do
    this only for SIBs that are not received yet. */
    for( count=0; count<RRC_TOT_SIBS; count++ )
    {

      if((result.result[count] == VALUE_TAG_DID_NOT_MATCH)
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
       ||(result.result[count] == DEFERRED_SIB_RCVD)
#endif
        )
      {
        if(supported_sibs_lookup_table[count] == FALSE)
        {
          sib_events_ptr->event[event_index].tsib[count].sib_needed = FALSE;
        }
        else
        {
          sib_events_ptr->event[event_index].tsib[count].sib_needed = TRUE;
        }
      }

      /* If a SIB is needed for this event and has not been
      decoded, check if we have a matching value-tag */
      if( (sib_events_ptr->event[event_index].tsib[count].sib_needed) &&
          (!(sib_events_ptr->event[event_index].tsib[count].sib_decoded))
        )
      {
        if( result.result[count] ==  VALUE_TAG_MATCHED )
        {
          gen_mask_unchanged |= (MIB_BIT << count);

          rrcsib_log_sys_inf(db_index,count);

          /* The value-tag matches! we already have this SIB 
          in the database. No need to receive or decode this SIB. */
          sib_events_ptr->event[event_index].tsib[count].sib_received = TRUE;
          sib_events_ptr->event[event_index].tsib[count].sib_decoded = TRUE;

          //if value tags match for Scheduling Blocks for SIB's already read, then
          //update the value tags from the stored SIBs.. also check to see if SB's are
          //already not decoded before MIB/SB1/SB2 are decoded
          if(((uint32)rrc_SB1== count) && 
             (sib_events_ptr->event[event_index].tempholder.sb1_decoded==FALSE))
          {
            sb1 = rrc_sibdb_return_sib_for_db_index(rrc_SB1,db_index);
            if(sb1 != NULL)
            {
              if(FAILURE == rrcsib_check_if_scheduling_info_received_for_all_sibs((void *)sb1,
                                                                      rrc_SB1, event_index))
              {
                rrcsib_process_sib_scheduling_info_incomplete(event_index); 
                return;
              }
              else
              {
                WRRC_MSG0_HIGH("Updating Sched from SIBDB SB1");
                rrcsib_update_sched_info_from_mib_sb((uint32)rrc_SB1, 
                                                  (void *) sb1, event_index);
              }
            }
          
            /* Since SB1 value tag matched and we have taken care of updating schedule present
               in SB1 from SIB DB,set this flag to true */
            sib_events_ptr->event[event_index].tempholder.sb1_decoded = TRUE;
          }

          if(((uint32)rrc_SB2== count) && 
             (sib_events_ptr->event[event_index].tempholder.sb2_decoded==FALSE))
          {
            sb2= rrc_sibdb_return_sib_for_db_index(rrc_SB2,db_index);
            if(sb2!= NULL)
            {
              if(FAILURE == rrcsib_check_if_scheduling_info_received_for_all_sibs((void *)sb2,
                                                                          rrc_SB2, event_index))
              {
                rrcsib_process_sib_scheduling_info_incomplete(event_index); 
                return;
              }
              else
              {
              WRRC_MSG0_HIGH("Updating Sched from SIBDB SB2");
              rrcsib_update_sched_info_from_mib_sb((uint32)rrc_SB2, 
                                                   (void *) sb2, event_index);
              }
            }

            /* Since SB2 value tag matched and we have taken care of updating schedule present
               in SB2 from SIB DB,set this flag to true */
            sib_events_ptr->event[event_index].tempholder.sb2_decoded = TRUE;
          }
        }
        else if( result.result[count] == VALUE_TAG_DID_NOT_MATCH)
        {
          gen_mask_changed |= (MIB_BIT << count);

        }
        else if(TIMER_BASED_SIB_RCVD == result.result[count])
        {
          /*Timer based SIB's have no value tag.. so if the SIB has already
            been received for this EVENT, treat it as received, but not
            decoded yet..  If the SIB is not even received, then it is
            treated as v_tag did not match and read again */
          sib_events_ptr->event[event_index].tsib[count].sib_received = TRUE;
          sib_events_ptr->event[event_index].tsib[count].sib_decoded = FALSE;
        }
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
        else if( result.result[count] == DEFERRED_SIB_RCVD)
        {
          if(sib_events_ptr->event[event_index].tsib[count].sib_received)
          {
            sib_events_ptr->event[event_index].tsib[count].sib_decoded = FALSE;
          }
        }
#endif
      }

      /* Otherwise, if a SIB is needed for this event and has been
      decoded, if we have a non-matching value-tag we may have to
      read the SIB again. */
      else if( (sib_events_ptr->event[event_index].tsib[count].sib_needed) &&
               (sib_events_ptr->event[event_index].tsib[count].sib_decoded)
             )
      {
        if((result.result[count] == VALUE_TAG_DID_NOT_MATCH)
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
              ||(result.result[count] == DEFERRED_SIB_RCVD)
#endif
           )

        {
            gen_mask_changed |= (MIB_BIT << count);
            /*Reset all the variables for this SIB so that we can try to
            read it again */
            sib_events_ptr->event[event_index].tsib[count].sib_received = FALSE;
            sib_events_ptr->event[event_index].tsib[count].sib_decoded = FALSE;
            sib_events_ptr->event[event_index].tsib[count].seg_count = 0;
            sib_events_ptr->event[event_index].tsib[count].seg_index = 
              (MAX_SEG_INDEX_FOR_A_SIB + 1);
            sib_events_ptr->event[event_index].tsib[count].sib_tot_length = 0;
        }
      }
    }/* end for */
  
#ifdef FEATURE_RRC_SIB_DEBUG
  WRRC_MSG2_HIGH("SIB_MASK %d is current: Value Tag Matched, Value tag changed for SIB_MASK %d read again", gen_mask_unchanged,gen_mask_changed);
#endif /* FEATURE_RRC_SIB_DEBUG */  
  }
}/* end function update_event_for_received_value_tags */

/*===========================================================================

FUNCTION rrcsib_check_sib_schedule_for_sibs

DESCRIPTION
  Check if the received bcch message fits into the segment schedule received
  in MIB/SB1 or SB2 earlier.

DEPENDENCIES
 
  None.

RETURN VALUE
  TRUE:  if segment matches schedule
  FALSE: if segment does not match schedule received

SIDE EFFECTS

  None.
        
===========================================================================*/
static boolean rrcsib_check_sib_schedule_for_sibs
(
  rrc_SystemInformation_BCH * msg_ptr
  ,sib_events_index_e_type index
)
{
  uint8 seg_index[MAX_SEGMENTS_FOR_A_SIB];
  uint32 sib_type_asn1[MAX_SEGMENTS_FOR_A_SIB] = {0}; // Zero initialization done to suppress Lint error.
  uint32 rrc_sib_type[MAX_SEGMENTS_FOR_A_SIB] = {0}; // Zero initialization done to suppress Lint error.
  uint32 count=0, i = 0;
  boolean status = FALSE;
  uint8 j = 0;
  boolean sched_info_not_received = FALSE;
  uint32 n_value=0;
  rrc_CompleteSIB_List *local_complete_SIB_List;
  OSRTDListNode *temp_list_ptr = NULL;
  rrc_CompleteSIBshort *temp_node_ptr = NULL;
  
  /*Initialisations have been done to suppress LINT error*/
  uint32 sfn_prime[MAX_SEGMENTS_FOR_A_SIB] = {0};
  sib_segment_e_type seg_type[MAX_SEGMENTS_FOR_A_SIB] = {noSegment_chosen};
  uint32 seg_length[MAX_SEGMENTS_FOR_A_SIB] = {0};
  unsigned char* value[MAX_SEGMENTS_FOR_A_SIB] = {0};
  seg_proc_stat_e_type seg_status = SEG_PROC_SUCCEEDED;

  //sfn-Prime is always a multiple of 2.. left shift once to get the actual sfn
  uint32 off_set = (msg_ptr->sfn_Prime << 1);

#ifdef FEATURE_RRC_NO_MIB_FOUND_BAR_CELL
  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return FALSE;
  }
#endif

  //here if mib rep_rate is invalid, return false as we won't have scheduling info for any other
  //sibs, if this SIB is the first segment of a MIB, process it and check set MIB scheduling info
  if(INVALID_SIB_SCHEDULE_REP == sib_events_ptr->sib_sched[sib_type_asn1[rrc_MIB]].repitition_rate)
  {
    //if MIB is not yet read and the current SIB does not start from a SFN-Prime of Multiple of 8, then
    //return right away, since MIB is not read and also the current SIB is not a MIB
    if (((off_set) & 0x7) != 0)
    {
      WRRC_MSG1_HIGH("MIB not rcvd yet. Offset is %d", off_set);
      status=FALSE;
      return status;
    }
    else
    {
      //MIB scheduling is hardcoded by spec.  Rep-Rate is 8, subsequent segments are at an offset of 2,
      //hence we have this hardcoded values seen below.
      WRRC_MSG0_HIGH("First seg of MIB rcvd. Init MIB-Sched");
      sib_events_ptr->sib_sched[rrc_sib_type[rrc_MIB]].repitition_rate = 8;
      sib_events_ptr->sib_sched[rrc_sib_type[rrc_MIB]].sfn_offset[0] =0;
      sib_events_ptr->sib_sched[rrc_sib_type[rrc_MIB]].sfn_offset[1] =2;
      sib_events_ptr->sib_sched[rrc_sib_type[rrc_MIB]].sfn_offset[2] =4;
      sib_events_ptr->sib_sched[rrc_sib_type[rrc_MIB]].sfn_offset[3] =6;
    }
  }

  //reset the flags that checks to see if all SIB's are received and decoded
  memset (seg_index, 0x0, MAX_SEGMENTS_FOR_A_SIB);
  memset (sib_type_asn1, 0x0, MAX_SEGMENTS_FOR_A_SIB);
  memset (rrc_sib_type, 0x0, MAX_SEGMENTS_FOR_A_SIB);
  
  switch(RRCGET_SIB_TYPE_AND_TAG((&msg_ptr->payload)))
  {
    //no sensible segment, do nothing but return from here..
    case RRCGET_SystemInformation_BCH_payload_TYPE(noSegment):
      status = FALSE;
      break;

    case RRCGET_SystemInformation_BCH_payload_TYPE(firstSegment):
      seg_index[count] = 0;

      sfn_prime[count] = msg_ptr->sfn_Prime;
      seg_type[count]= firstSegment_chosen;
      seg_length[count] = msg_ptr->payload.u.firstSegment.sib_Data_fixed.numbits;
      value[count] = msg_ptr->payload.u.firstSegment.sib_Data_fixed.data;      

      sib_type_asn1[count++] = (uint32)msg_ptr->payload.u.firstSegment.sib_Type;
      break;

    case RRCGET_SystemInformation_BCH_payload_TYPE(subsequentSegment):
      seg_index[count] = msg_ptr->payload.u.subsequentSegment.segmentIndex;

      sfn_prime[count] = msg_ptr->sfn_Prime;
      seg_type[count]= subsequentSegment_chosen;
      seg_length[count] = msg_ptr->payload.u.subsequentSegment.sib_Data_fixed.numbits;
      value[count] = msg_ptr->payload.u.subsequentSegment.sib_Data_fixed.data;      

      sib_type_asn1[count++] = (uint32)msg_ptr->payload.u.subsequentSegment.sib_Type;
      break;

    case RRCGET_SystemInformation_BCH_payload_TYPE(lastSegmentShort):
      seg_index[count] =  msg_ptr->payload.u.lastSegmentShort.segmentIndex;

      sfn_prime[count] = msg_ptr->sfn_Prime;
      seg_type[count]= lastSegmentShort_chosen;
      seg_length[count] = msg_ptr->payload.u.lastSegmentShort.sib_Data_variable.numbits;
      value[count] = msg_ptr->payload.u.lastSegmentShort.sib_Data_variable.data;

      sib_type_asn1[count++] = (uint32)msg_ptr->payload.u.lastSegmentShort.sib_Type;
      break;

    case RRCGET_SystemInformation_BCH_payload_TYPE(lastAndFirst):
      seg_index[count] = 0;

      sfn_prime[count] = msg_ptr->sfn_Prime;
      seg_type[count]= firstSegment_chosen;
      seg_length[count] = msg_ptr->payload.u.lastAndFirst.firstSegment.sib_Data_variable.numbits;
      value[count] = msg_ptr->payload.u.lastAndFirst.firstSegment.sib_Data_variable.data;

      sib_type_asn1[count++] = (uint32)msg_ptr->payload.u.lastAndFirst.firstSegment.sib_Type;

      seg_index[count] = msg_ptr->payload.u.lastAndFirst.lastSegmentShort.segmentIndex;

      sfn_prime[count] = msg_ptr->sfn_Prime;
      seg_type[count]= lastSegmentShort_chosen;
      seg_length[count] = msg_ptr->payload.u.lastAndFirst.lastSegmentShort.sib_Data_variable.numbits;
      value[count] = msg_ptr->payload.u.lastAndFirst.lastSegmentShort.sib_Data_variable.data;

      sib_type_asn1[count++] = (uint32)msg_ptr->payload.u.lastAndFirst.lastSegmentShort.sib_Type;
      break;

    case RRCGET_SystemInformation_BCH_payload_TYPE(lastAndComplete):

      seg_index[count] = msg_ptr->payload.u.lastAndComplete.lastSegmentShort.segmentIndex;

      sfn_prime[count] = msg_ptr->sfn_Prime;
      seg_type[count]= lastSegmentShort_chosen;
      seg_length[count] = msg_ptr->payload.u.lastAndComplete.lastSegmentShort.sib_Data_variable.numbits;
      value[count] = msg_ptr->payload.u.lastAndComplete.lastSegmentShort.sib_Data_variable.data;

      sib_type_asn1[count++] = (uint32)msg_ptr->payload.u.lastAndComplete.lastSegmentShort.sib_Type;
      

      local_complete_SIB_List = &(msg_ptr->payload.u.lastAndComplete.completeSIB_List);
      n_value = local_complete_SIB_List->count;
      temp_list_ptr = local_complete_SIB_List->head;

      while(n_value != 0)
      {
        seg_index[count] = 0;

        sfn_prime[count] = msg_ptr->sfn_Prime;
        seg_type[count]= completeSIB_chosen;
        temp_node_ptr = (rrc_CompleteSIBshort*)temp_list_ptr->data;
        if(temp_node_ptr == NULL)
        {
          WRRC_MSG0_ERROR("Invalid SIB lastAndComplete received");
          return FALSE;
        }
        seg_length[count] = temp_node_ptr->sib_Data_variable.numbits;
        value[count] = temp_node_ptr->sib_Data_variable.data;

        sib_type_asn1[count++] = (uint32)temp_node_ptr->sib_Type;

        temp_list_ptr = temp_list_ptr->next;
        n_value--;
      }
      break;

   case RRCGET_SystemInformation_BCH_payload_TYPE(lastAndCompleteAndFirst):
      
     seg_index[count] = msg_ptr->payload.u.lastAndCompleteAndFirst.lastSegmentShort.segmentIndex;

      sfn_prime[count] = msg_ptr->sfn_Prime;
      seg_type[count]= lastSegmentShort_chosen;
      seg_length[count] = msg_ptr->payload.u.lastAndCompleteAndFirst.lastSegmentShort.sib_Data_variable.numbits;
      value[count] = msg_ptr->payload.u.lastAndCompleteAndFirst.lastSegmentShort.sib_Data_variable.data;

     sib_type_asn1[count++] = (uint32)msg_ptr->payload.u.lastAndCompleteAndFirst.lastSegmentShort.sib_Type;

     seg_index[count] = 0;

      sfn_prime[count] = msg_ptr->sfn_Prime;
      seg_type[count]= firstSegment_chosen;
      seg_length[count] = msg_ptr->payload.u.lastAndCompleteAndFirst.firstSegment.sib_Data_variable.numbits;
      value[count] = msg_ptr->payload.u.lastAndCompleteAndFirst.firstSegment.sib_Data_variable.data;     

     sib_type_asn1[count++] = (uint32)msg_ptr->payload.u.lastAndCompleteAndFirst.firstSegment.sib_Type;
      
  
     local_complete_SIB_List = &(msg_ptr->payload.u.lastAndCompleteAndFirst.completeSIB_List);
     n_value = local_complete_SIB_List->count;
     temp_list_ptr = local_complete_SIB_List->head;
  
     while(n_value != 0)
     {
       seg_index[count] = 0;  

      sfn_prime[count] = msg_ptr->sfn_Prime;
      seg_type[count]= completeSIB_chosen;
      temp_node_ptr = (rrc_CompleteSIBshort*)temp_list_ptr->data;
      if(temp_node_ptr == NULL)
      {
        WRRC_MSG0_ERROR("Invalid SIB lastAndCompleteAndFirst received");
        return FALSE;
      }
      seg_length[count] = temp_node_ptr->sib_Data_variable.numbits;
      value[count] = temp_node_ptr->sib_Data_variable.data;

       sib_type_asn1[count++] = (uint32)temp_node_ptr->sib_Type;  
        
       temp_list_ptr = temp_list_ptr->next;
       n_value--;
     }
     break;

    case RRCGET_SystemInformation_BCH_payload_TYPE(completeSIB_List):
      
      local_complete_SIB_List = &(msg_ptr->payload.u.completeSIB_List);
      n_value = local_complete_SIB_List->count;
      temp_list_ptr = local_complete_SIB_List->head;

      while(n_value != 0)
      {
        seg_index[count] = 0;

        sfn_prime[count] = msg_ptr->sfn_Prime;
        seg_type[count]= completeSIB_chosen;
        temp_node_ptr = (rrc_CompleteSIBshort*)temp_list_ptr->data;
        if(temp_node_ptr == NULL)
        {
          WRRC_MSG0_ERROR("Invalid SIB completeSIB_List received");
          return FALSE;
        }
        seg_length[count] = temp_node_ptr->sib_Data_variable.numbits;
        value[count] = temp_node_ptr->sib_Data_variable.data;

        sib_type_asn1[count++] = (uint32)temp_node_ptr->sib_Type;

        temp_list_ptr = temp_list_ptr->next;
        n_value--;
      }
      break;


    case RRCGET_SystemInformation_BCH_payload_TYPE(completeAndFirst):
      seg_index[count] = 0;

      sfn_prime[count] = msg_ptr->sfn_Prime;
      seg_type[count]= firstSegment_chosen;
      seg_length[count] = msg_ptr->payload.u.completeAndFirst.firstSegment.sib_Data_variable.numbits;
      value[count] = msg_ptr->payload.u.completeAndFirst.firstSegment.sib_Data_variable.data;

      sib_type_asn1[count++] = (uint32)msg_ptr->payload.u.completeAndFirst.firstSegment.sib_Type;

      local_complete_SIB_List = &(msg_ptr->payload.u.completeAndFirst.completeSIB_List);
      n_value = local_complete_SIB_List->count;
      temp_list_ptr = local_complete_SIB_List->head;

      while(n_value != 0)
      {
        seg_index[count] = 0;

        sfn_prime[count] = msg_ptr->sfn_Prime;
        seg_type[count]= completeSIB_chosen;
        temp_node_ptr = (rrc_CompleteSIBshort*)temp_list_ptr->data;
        if(temp_node_ptr == NULL)
        {
          WRRC_MSG0_ERROR("Invalid SIB completeAndFirst received");
          return FALSE;
        }
        seg_length[count] = temp_node_ptr->sib_Data_variable.numbits;
        value[count] = temp_node_ptr->sib_Data_variable.data;

        sib_type_asn1[count++] = (uint32)temp_node_ptr->sib_Type;

        temp_list_ptr = temp_list_ptr->next;
        n_value--;
      }
      break;

    case RRCGET_SystemInformation_BCH_payload_TYPE(completeSIB):
      seg_index[count] = 0;

      sfn_prime[count] = msg_ptr->sfn_Prime;
      seg_type[count]= completeSIB_chosen;
      seg_length[count] = msg_ptr->payload.u.completeSIB.sib_Data_fixed.numbits;
      value[count] = msg_ptr->payload.u.completeSIB.sib_Data_fixed.data;   

      sib_type_asn1[count++] =(uint32) msg_ptr->payload.u.completeSIB.sib_Type;      
      break;

    case RRCGET_SystemInformation_BCH_payload_TYPE(lastSegment):
      seg_index[count] = msg_ptr->payload.u.lastSegment.segmentIndex;

      sfn_prime[count] = msg_ptr->sfn_Prime;
      seg_type[count]= lastSegment_chosen;
      seg_length[count] = msg_ptr->payload.u.lastSegment.sib_Data_fixed.numbits;
      value[count] = msg_ptr->payload.u.lastSegment.sib_Data_fixed.data;

      sib_type_asn1[count++] = (uint32)msg_ptr->payload.u.lastSegment.sib_Type;
      break;

    default:
      WRRC_MSG1_ERROR("Unknown seg-type %d", RRCGET_SIB_TYPE_AND_TAG((&msg_ptr->payload)));
      break;
  }

#ifdef FEATURE_RRC_NO_MIB_FOUND_BAR_CELL
/*If the UE does not find the master information block in a position fulfilling:
     SFN mod 32 = 0 
(Note: These are controlled by MAX_TIME_MIB_WAIT &MIB_POSITION)
but a transport block with correct CRC was found at that position), the UE shall:
1>  consider the master information block as not found; and
1>  consider the cell to be barred and
1>  consider the barred cell as using the value "allowed" in the 
IE "Intra-frequency cell re-selection indicator",and the maximum value in the IE "Tbarred".
Note: This feature we are restricting to cell reselection only for now.
*/
  if(sib_events_ptr->event[index].tsib[rrc_MIB].sib_received != TRUE)
  {
    if((sib_events_ptr->event[index].event_name == RRCSIB_GET_SPECIFIC_SIBS)&&
       (sib_events_ptr->event[index].req_proc == RRC_PROCEDURE_CSP)&&
       (sib_events_ptr->event[index].for_serving_cell == FALSE)
      )
    {
      off_set = (msg_ptr->sfn_Prime << 1);
      if((off_set%MIB_POSITION) == 0)
      {
        boolean mib_found = FALSE;
        for(i=0;i<count;i++)
        {
          if(sib_type_asn1[i] == (uint32)RRC_SIB_TYPE(masterInformationBlock))
          {
            WRRC_MSG0_HIGH("MIB found");
            mib_found = TRUE;
            break;
          }
        }
        if(mib_found == FALSE)
        {
          no_mib_bar_cell_count++;
          WRRC_MSG1_HIGH("MIB not found at MIB position, count=%d",no_mib_bar_cell_count);
          if(no_mib_bar_cell_count == MAX_TIME_MIB_WAIT)
          {
            no_mib_bar_cell_count = 0;
            WRRC_MSG0_HIGH("Baring this cell as MIB not found at SFN%32");
            //bar this cell right here
            rrccsp_send_cphy_cell_bar_req(RRC_CSP_WCDMA_PSC_BARRED, 1280);
            /* An event was active. Since the timer expired,
            this event failed. */
            rrcsib_post_process_failed_event
            ( index, /* Event Index                 */
              TRUE,  /* Confirm Command is required */
              SIB_EVENT_FAILED_TIMEOUT, /* Timed out*/
              TRUE   /* Tear down S_BCCH if it's up */
            );
            status = FALSE;
            return status;
          }
        }
      }
    }
  }
#endif

  //loop thru the list of sib segments received and compare to see if the
  //sfn-prime at which it is received matches the sib-schedule earlier
  //received in MIB/SB1/SB2.. if it matches, then return true, else return 
  //false
  for(i=0;i<count;i++)
  {
    if(sib_type_asn1[i] != RRC_SIB_TYPE(extensionType))
    {
      rrc_sib_type[i] = rrcsib_map_asn1_sibtype_to_int_sibtype((rrc_SIB_Type)sib_type_asn1[i]);
      if(rrc_sib_type[i] == rrc_MAX_SIB)
      {
        WRRC_MSG0_ERROR("Unsupported SIB");
        continue;
      }
      if(rrc_sib_type[i] == (uint32)rrc_SIB5bis)
      {
        WRRC_MSG0_HIGH("Treating SIB5bis as SIB5 for checking SIB Schedule");
        rrc_sib_type[i] = (uint32)rrc_SIB5;
      }
      if(sib_events_ptr->sib_sched[rrc_sib_type[i]].repitition_rate != INVALID_SIB_SCHEDULE_REP)
      {
        if(sib_events_ptr->sib_sched[rrc_sib_type[i]].repitition_rate == 0)
        {
          ERR_FATAL("Potential Div by zero",0,0,0);
        }
  
        if(sib_events_ptr->sib_sched[rrc_sib_type[i]].sfn_offset[seg_index[i]] == 
           (off_set % sib_events_ptr->sib_sched[rrc_sib_type[i]].repitition_rate))
        {
          status = TRUE;
  
          MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
                "Sib %d, rep_rate %d seg_idx %d Msg Offset %d, Seg-Offset %d", 
                rrcsib_map_int_sibtype_to_asn1_sibtype((rrc_SIB_type)rrc_sib_type[i]), 
                sib_events_ptr->sib_sched[rrc_sib_type[i]].repitition_rate, seg_index[i],
                off_set,sib_events_ptr->sib_sched[rrc_sib_type[i]].sfn_offset[seg_index[i]]);
        }
        else
        {
          MSG_ERROR("Sib_type %d, seg_idx %d, offset %d", rrcsib_map_int_sibtype_to_asn1_sibtype((rrc_SIB_type)rrc_sib_type[i]), seg_index[i], off_set);
        }
      }
      else
      {
  
          if(supported_sibs_lookup_table[rrc_sib_type[i]] == TRUE)
          {
            if(sib_events_ptr->sib_sched[rrc_sib_type[i]].repitition_rate == INVALID_SIB_SCHEDULE_REP)
            {
                MSG_HIGH("TSIB - SIB segment received before scheduling information SIB %d, seg_idx %d, offset %d",
                         sib_type_asn1[i],seg_index[i],off_set);
                seg_status = rrcsib_store_sib_seg_in_tsib_optim_db((rrc_SIB_type)rrc_sib_type[i],
                                                      sfn_prime[i],
                                                      seg_index[i],
                                                      seg_type[i],
                                                      seg_length[i],
                                                      (void *)value[i],
                                                      index);
                if(seg_status == SEG_PROC_STORED)
                {
                  WRRC_MSG1_HIGH("TSIB - Segment saved for SIB %d", sib_type_asn1[i]);
                }
                else
                {
                  /*Simply set this to SEG_PROC_FAILED once more as 
                  rrcsib_store_sib_seg_in_tsib_optim_db() would have taken care of*/
                  seg_status = SEG_PROC_FAILED;
                  WRRC_MSG1_HIGH("TSIB - Failed to store segment for SIB %d", sib_type_asn1[i]);
                }
            }
          }
      }
    }
    else
    {
      for(j = rrc_SIB11bis;j < rrc_ExtensionType;j++)
      {
        if(supported_sibs_lookup_table[j] == TRUE)
        {
          if(sib_events_ptr->sib_sched[j].repitition_rate != INVALID_SIB_SCHEDULE_REP)
          {
            if(sib_events_ptr->sib_sched[j].repitition_rate == 0)
            {
              ERR_FATAL("Potential Div by zero",0,0,0);
            }
            if(sib_events_ptr->sib_sched[j].sfn_offset[seg_index[i]] == 
                   (off_set % sib_events_ptr->sib_sched[j].repitition_rate))
            {
              status = TRUE;
                  sched_info_not_received = FALSE;
              MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
                    "Extension Sib , rep_rate %d seg_idx %d Msg Offset %d, Seg-Offset %d", 
                    sib_events_ptr->sib_sched[rrc_sib_type[i]].repitition_rate, seg_index[i],
                    off_set,sib_events_ptr->sib_sched[rrc_sib_type[i]].sfn_offset[seg_index[i]]);

              break;
            }
          }
          else
          {
            sched_info_not_received = TRUE;
          }
        }
      }
      if(sched_info_not_received == TRUE)
      {
        WRRC_MSG2_HIGH("TSIB - SIB segment received before scheduling information Extension SIB , seg_idx %d, offset %d",seg_index[i],off_set);
        seg_status = rrcsib_store_sib_seg_in_tsib_optim_db(rrc_ExtensionType,
                                              sfn_prime[i],
                                              seg_index[i],
                                              seg_type[i],
                                              seg_length[i],
                                              (void *)value[i],
                                              index);
        if(seg_status == SEG_PROC_STORED)
        {
          WRRC_MSG0_HIGH("TSIB - Segment saved for extension SIB");
        }
        else
        {
          /*Simply set this to SEG_PROC_FAILED once more as 
          rrcsib_store_sib_seg_in_tsib_optim_db() would have taken care of*/
          seg_status = SEG_PROC_FAILED;
          WRRC_MSG0_HIGH("TSIB - Failed to store segment for Extension SIB");
        }
      }
      else if(status == FALSE)
      {
        WRRC_MSG2_ERROR("Extension Sib , seg_idx %d, offset %d", seg_index[i], off_set);
      }
    }
  }

  return status;
}
/*===========================================================================

FUNCTION rrcsib_decode_and_save_sib_for_current_event

DESCRIPTION

  This function checks checks the sib_db for a particular sib, deletes the old
  one if present.  Then decodes the new sib for the on-going event.
  
  
DEPENDENCIES
 
  Assumes that SIB_DB_INDEX is already found for this event

RETURN VALUE
 Boolean: True, if decoding successful, else false

SIDE EFFECTS

  None.
        
===========================================================================*/
static boolean rrcsib_decode_and_save_sib_for_current_event
(
  void ** sib_ptr,
  sib_events_index_e_type event_index,
  uint32 db_index,
  rrc_SIB_type rrc_sib_type,
  uint32 pdu_num
  ,byte *sib_encoded_ptr
  ,uint32 *sib_encoded_pdu_length

)
{
  uint32 pdu_length;
  boolean status = FALSE;
  boolean conn_mode_sibs_present = FALSE;
  void *sib_ptr_tmp = NULL;


  if(EVENT_INDEX_VALID(event_index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return FALSE;
  }

  if(rrc_sib_type >= rrc_ExtensionType)
  {
    WRRC_MSG0_ERROR("Invalid SIB Type");
    return FALSE;
  }

  if( *sib_ptr != NULL )
  {
    rrc_sibdb_free_sib( (int)pdu_num,
                        (void *)*sib_ptr );
    *sib_ptr=NULL;
  }

  if((rrc_sib_type == rrc_SIB3)||
    (rrc_sib_type == rrc_SIB5)||
    (rrc_sib_type == rrc_SIB11)||
    (rrc_sib_type == rrc_SIB5bis))
  {
    conn_mode_sibs_present = rrcsib_check_if_connected_mode_sibs_present
      (sib_events_ptr->event[event_index].tsib[rrc_sib_type].sib_data_ptr[0], 
      rrc_sib_type);
    WRRC_MSG2_HIGH("conn_mode_sibs_present = %d in sib = %d", conn_mode_sibs_present, 
      rrc_sib_type);
  }


  /* Get the PDU length in bytes */
  pdu_length =  GET_PDU_LENGTH_IN_BYTES( 
    sib_events_ptr->event[event_index].tsib[rrc_sib_type].sib_tot_length );

  /* ASN.1 decode the PDU */
  sib_ptr_tmp =  rrcasn1_decode_pdu( 
    (void *)(sib_events_ptr->event[event_index].tsib[rrc_sib_type].sib_data_ptr), /* Encoded SIB      */
    (int)pdu_num,                                                       /* SIB's PDU number */
    pdu_length                                                     /* SIB's Length in bytes */
    );
  
  if(rrc_sib_type == rrc_SIB7)
  {
    if((sib_events_ptr->event[event_index].tsib[rrc_sib_type].sib_decoded == TRUE) &&
       (sib_ptr_tmp == NULL)
       )
    {
      WRRC_MSG0_HIGH("Decoding of duplicate SIB7 failed. Retaining the SIB7 stored in SIB DB");
      return TRUE;
    }
  }
  *sib_ptr = sib_ptr_tmp;

  if(*sib_ptr == NULL )
  {
    /* The decoding failed */
    WRRC_MSG1_HIGH("ASN1 Decoding failed for %d", rrc_sib_type);
  #ifdef FEATURE_SIB19_DECODE_FAIL_OPT
    if((rrc_sib_type == rrc_SIB19)&&(++sib19_decode_fail_cnt >= SIB19_MAX_DECODE_FAIL_CNT))
    {
      sib_events_ptr->event[event_index].tsib[rrc_sib_type].sib_received = TRUE;
      sib_events_ptr->event[event_index].tsib[rrc_sib_type].sib_decoded = TRUE;
      sibdb.cell[db_index].sib_valid[rrc_SIB19] = FALSE;
      sibdb.cell[db_index].sib_value_tag[rrc_SIB19] = INVALID_MIB_SB_VALUE_TAG;
      status = TRUE;
    }
    else
  #endif
    {

      /*Reset all the variables for this SIB so that we can try to
      read it again */
      sib_events_ptr->event[event_index].tsib[rrc_sib_type].sib_received = FALSE;
      sib_events_ptr->event[event_index].tsib[rrc_sib_type].sib_decoded = FALSE;
      sib_events_ptr->event[event_index].tsib[rrc_sib_type].seg_count = 0;
      sib_events_ptr->event[event_index].tsib[rrc_sib_type].seg_index = 
      (MAX_SEG_INDEX_FOR_A_SIB + 1);
      sib_events_ptr->event[event_index].tsib[rrc_SB1].sib_tot_length = 0;

      status = FALSE;
    }
  }
  else
  {
    MSG_LOW("SIB_HEAP:Free sib_encoded_ptr, val=%d",sib_encoded_ptr,0,0);
      memset(sib_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
      *sib_encoded_pdu_length = INVALID_PDU_LENGTH;

/*lint -e545*/
    WCDMA_MEMCPY(sib_encoded_ptr,
                 MAX_SIB_LENGTH_BYTES * sizeof(byte),
                 &(sib_events_ptr->event[event_index].tsib[rrc_sib_type].sib_data_ptr),
                 pdu_length);
/*lint +e545*/
    *sib_encoded_pdu_length = pdu_length ;
     MSG_LOW("SIB_HEAP:Saved Sib type=%d,len=%d(bytes) in encoded form", 
      rrc_sib_type, *sib_encoded_pdu_length, 0);

#ifdef FEATURE_UPDATE_SIB7_IN_FACH
    if(rrc_sib_is_current_event_sib7_in_fach() == TRUE)
    {
      /*For SIB7 event after receiving SIB7, init the book keeping 
        info for sib7 in event table which will make the same sib7 
        event to ready for receiving another sib7 on BCCH*/
      if(rrc_sib_type == rrc_SIB7)
      {
        sibdb.cell[db_index].sib_valid[rrc_sib_type] = TRUE;
        /*Re-starting SIB-7 event*/
        sib_events_ptr->event[event_index].tsib[rrc_sib_type].sib_received = FALSE;
        sib_events_ptr->event[event_index].tsib[rrc_sib_type].sib_decoded = FALSE;
        sib_events_ptr->event[event_index].tsib[rrc_sib_type].seg_count = 0;
        sib_events_ptr->event[event_index].tsib[rrc_sib_type].seg_index = 
          (MAX_SEG_INDEX_FOR_A_SIB + 1);
        sib_events_ptr->event[event_index].tsib[rrc_SB1].sib_tot_length = 0;
      
        status = TRUE;
        WRRC_MSG1_HIGH("sib7:Preparing SIB7 event to receive another sib7, sibdb valid=%d", 
                              sibdb.cell[db_index].sib_valid[rrc_sib_type]);
        (void)rrcllc_update_configs_with_sib7_info_in_fach();
#ifdef FEATURE_WCDMA_HS_FACH_DRX
        /* Sucessfully received SIB7. Build scheduling table and send indication */
        rrcsib_build_sib7_schedule_table(rrc_SIB7,SIB_SRV_INDEX,RRC_SKIP_SFN);
#endif
      }
    }
    else
#endif
    {
      if((rrc_sib_type == rrc_SIB11bis) ||
         (rrc_sib_type == rrc_SIB19)
        )
      {
        rrc_log_dl_sig_message_for_ext_sibs(rrc_sib_type, event_index);
      }
      /* Indicate that the PDU has been decoded. */
      sib_events_ptr->event[event_index].tsib[rrc_sib_type].sib_decoded = TRUE;

      /* Indicate in the sib database that this sib is now valid */
      sibdb.cell[db_index].sib_valid[rrc_sib_type] = TRUE;
      if(rrc_sib_type == rrc_SIB7)
      {
          sib_events_ptr->event[event_index].tsib[rrc_sib_type].sib_tot_length = 0;
      }
      status = TRUE;
    }
    if((rrc_sib_type == rrc_SIB7) && (rrc_sib7_time_nv != 0))
    {
      unsigned long exp_time = rrc_sib7_time_nv;
      exp_time = rrcsib_get_sib7_exp_timer_value(db_index);
      exp_time = (exp_time > rrc_sib7_time_nv)? exp_time : rrc_sib7_time_nv;

      rrctmr_stop_timer(RRCTMR_SIB7_EXPIRATION_TIMER);
      rrctmr_start_timer(RRCTMR_SIB7_EXPIRATION_TIMER,exp_time);
      WRRC_MSG1_HIGH("Decoding: Starting SIB7 timer for %d ms",exp_time);
    }
  }

  return status;
}
     
  


/*===========================================================================

FUNCTION rrcsib_check_pich_aich_barred_criteria

DESCRIPTION

  This function checks sib5/6 t barred criteria by checking to see if AICH info
  is present
  
DEPENDENCIES
 
  None.

RETURN VALUE
 
TRUE:  SIB-5 satisfies T-Barred Criteria
FALSE:  SIB-5 does not satisfy T-Barred Criteria

SIDE EFFECTS

  None.
        
===========================================================================*/
static boolean rrcsib_check_pich_aich_barred_criteria
(
  uint32 dbindex,         /* Index to the sib database. */
  rrc_SIB_type rrc_sib_type   /* whether it is sib 5 or 6   */
)
{

  if(rrc_SIB5 == rrc_sib_type)
  {
    if((FAILURE == rrcllc_validate_t_barred_criteria_for_pich_info(&(sibdb.cell[dbindex].sibs.sib5->sCCPCH_SystemInformationList))) || 
       (FAILURE == rrcllc_validate_t_barred_criteria_for_aich_info(&(sibdb.cell[dbindex].sibs.sib5->prach_SystemInformationList))))
    {
      return TRUE;
    }
  }
  else if (rrc_SIB6 == rrc_sib_type)
  {
    if((FAILURE == rrcllc_validate_t_barred_criteria_for_pich_info(&(sibdb.cell[dbindex].sibs.sib6->sCCPCH_SystemInformationList))) || 
       (FAILURE == rrcllc_validate_t_barred_criteria_for_aich_info(&(sibdb.cell[dbindex].sibs.sib6->prach_SystemInformationList))))
    {
      return TRUE;
    }
  }

  return FALSE;
}
/*===========================================================================

FUNCTION rrcsib_check_sib5_sib6_barred_criteria

DESCRIPTION

  This function checks sib5/6 t barred criteria by checking to see if AICH info
  is present
  
DEPENDENCIES
 
  None.

RETURN VALUE
 
SUCCESS:  SIB-5 satisfies T-Barred Criteria
FAILURE:  SIB-5 does not satisfy T-Barred Criteria

SIDE EFFECTS

  None.
        
===========================================================================*/
static uecomdef_status_e_type rrcsib_check_sib5_sib6_barred_criteria
(
  uint32 dbindex,                /* Index to the sib database. */
  sib_events_index_e_type index  /* Index into the sib_events database. */
)
{
  rrc_state_e_type rrc_state;     /* store locally the rrc current state */

  boolean sib5_read = FALSE;      /* initialize sib5 read flag to false  */
  boolean sib6_read = FALSE;      /* initialize sib5 read flag to false  */
  
  boolean cell_barred_status = FALSE;  /* indicates that a cell is barred.. */
  rrc_state = rrc_get_state();

  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return FAILURE;
  }
  /* store local information that sib 5 and sib 6 are needed, received and decoded rather than use
   * long loops everywhere
   */
  if((sib_events_ptr->event[index].tsib[rrc_SIB5].sib_needed == TRUE) &&
    (sib_events_ptr->event[index].tsib[rrc_SIB5].sib_received == TRUE) &&
    (sib_events_ptr->event[index].tsib[rrc_SIB5].sib_decoded == TRUE))
  {
     sib5_read = TRUE;
  }

  if((sib_events_ptr->event[index].tsib[rrc_SIB6].sib_needed == TRUE) &&
    (sib_events_ptr->event[index].tsib[rrc_SIB6].sib_received == TRUE) &&
    (sib_events_ptr->event[index].tsib[rrc_SIB6].sib_decoded == TRUE))
  {
    sib6_read = TRUE;
  }

  if(FALSE == sib5_read && FALSE == sib6_read)
  {
    //if neither SIB5 nor SIB6 are read, then can't really determine barred criteria
    //return success right here
    return SUCCESS;
  }

  if(sib5_read == TRUE)
  {
    if( sibdb.cell[dbindex].sibs.sib5== NULL )
    {
      /* ASN.1 decode the PDU */
      sibdb.cell[dbindex].sibs.sib5=  rrcasn1_decode_pdu( 
      (void *)(sibdb.cell[dbindex].sibs.sib5_encoded_ptr), /* Encoded SIB      */
      rrc_SysInfoType5bis_PDU,                                                       /* SIB's PDU number */
      sibdb.cell[dbindex].sibs.sib5_encoded_pdu_length   /* SIB's Length in bytes */
      );
      if(sibdb.cell[dbindex].sibs.sib5== NULL)
      {
        ERR_FATAL("SIB_HEAP:Asn.1 decode failed from SIB5", 0,0, 0);
      }
      WRRC_MSG0_HIGH("SIB_HEAP:Decode Sib5 in sibdb");
    }
    else
    {
      WRRC_MSG1_HIGH("SIB_HEAP:SIB5 already decoded,sib5_ptr=%d", sibdb.cell[dbindex].sibs.sib5);
    }
  }
  if(sib6_read == TRUE)
  {
    if( sibdb.cell[dbindex].sibs.sib6== NULL )
    {
      /* ASN.1 decode the PDU */
      sibdb.cell[dbindex].sibs.sib6=  rrcasn1_decode_pdu( 
      (void *)(sibdb.cell[dbindex].sibs.sib6_encoded_ptr), /* Encoded SIB      */
      rrc_SysInfoType6_PDU,                                                       /* SIB's PDU number */
      sibdb.cell[dbindex].sibs.sib6_encoded_pdu_length   /* SIB's Length in bytes */
      );
      if(sibdb.cell[dbindex].sibs.sib6== NULL)
      {
        ERR_FATAL("SIB_HEAP:Asn.1 decode failed from SIB6", 0,0, 0);
      }
      WRRC_MSG0_HIGH("SIB_HEAP:Decode Sib6 in sibdb");
    }
    else
    {
      WRRC_MSG1_HIGH("SIB_HEAP:SIB6 already decoded,sib6_ptr=%d", sibdb.cell[dbindex].sibs.sib6);
    }
  }

  WRRC_MSG2_HIGH("SIB5 %d, SIB6 %d read status", sib5_read, sib6_read);

  /* we should read SIB5 for connecting too, since the only time we read sib's in connecting is
   * to do cell reselection in connecting state 
   */
  if((rrc_state == RRC_STATE_CONNECTING) || (rrc_state == RRC_STATE_DISCONNECTED))
  {
    if(TRUE == sib5_read)
    {
      if(TRUE == rrcsib_check_pich_aich_barred_criteria(dbindex, rrc_SIB5))
      {
        //Call CSP API to indicate this cell as barred to L1 only for normal sib scan..
        /* Indicate in the sib database that this sib is now valid */
        cell_barred_status = TRUE;
        WRRC_MSG0_HIGH("SIB5 does not have AICH/PICH info");
      }
    }
  }
  else
  {
    /* in connected mode, wait for SIB5 too if SIB6 is already decoded.. bcos SIB5 may have some sibs which
     * are not in SIB6
     */
    if(TRUE == sib6_read && FALSE == sib5_read)
    {
      WRRC_MSG0_HIGH("SIB6 only read in connected mode yet.  Wait for SIB5");
      return SUCCESS;
    }

    /* if both sib5 and 6 are read */
    if(TRUE == sib5_read  && TRUE == sib6_read)
    {      
      /* check if pich criteria meets for cell barring */
      if((FAILURE == rrcllc_validate_t_barred_criteria_for_pich_info(&(sibdb.cell[dbindex].sibs.sib5->
                                                                     sCCPCH_SystemInformationList))) && 
         (FAILURE == rrcllc_validate_t_barred_criteria_for_pich_info(&(sibdb.cell[dbindex].sibs.sib6->
                                                                     sCCPCH_SystemInformationList))))
      {
        WRRC_MSG0_HIGH("PICH info not present in SIB5/SIB6");
        cell_barred_status = TRUE;
      }
      
      /* check if aich criteria meets for cell barring */
      if ((FAILURE == rrcllc_validate_t_barred_criteria_for_aich_info(&(sibdb.cell[dbindex].sibs.sib5->
                                                                      prach_SystemInformationList))) &&
          (FAILURE == rrcllc_validate_t_barred_criteria_for_aich_info(&(sibdb.cell[dbindex].sibs.sib6->
                                                                      prach_SystemInformationList))))
      {
        WRRC_MSG0_HIGH("AICH info not present in SIB5/SIB6");
        cell_barred_status = TRUE;
      }
    }
    else if (TRUE == sib5_read)    //else only sib5 read
    {
      WRRC_MSG0_HIGH("SIB5 only AICH/PICH info not present");
      cell_barred_status = rrcsib_check_pich_aich_barred_criteria(dbindex, rrc_SIB5);
    }
    else if (TRUE == sib6_read)   //else only sib6 read
    {
      WRRC_MSG0_HIGH("SIB6 only AICH/PICH info not present");
      cell_barred_status = rrcsib_check_pich_aich_barred_criteria(dbindex, rrc_SIB6);
    }
  }

  /* if cell is barred, call csp function */
  if(TRUE == cell_barred_status)
  {
    /* Call CSP API to indicate this cell as barred to L1 only for normal sib scan
     * Indicate in the sib database that this sib is now valid 
     */
    rrccsp_send_cphy_cell_bar_req(RRC_CSP_WCDMA_PSC_BARRED, 1280);
    rrcsib_post_process_failed_event(index, TRUE, SIB_EVENT_FAILED_OTHER, TRUE );
    return FAILURE;
  }

  return SUCCESS;
}
/*===========================================================================

FUNCTION rrcsib_process_scheduling_block_sched_info_before_sib_db_decoded

DESCRIPTION


DEPENDENCIES
 
  None.

RETURN VALUE

  SUCCESS: If scheduling information is received for all mandatory sib's
  FAILURE: If scheduling information is not received for all mandatory sib's

SIDE EFFECTS
  
===========================================================================*/
static uecomdef_status_e_type rrcsib_process_scheduling_block_sched_info_before_sib_db_decoded
(
  sib_events_index_e_type index, /* Index in the SIB event database */
  rrc_SIB_type sib
)
{
  rrc_SysInfoTypeSB1 *sb1_ptr;         /* Local pointer to SB1       */
  rrc_SysInfoTypeSB2 *sb2_ptr;         /* Local pointer to SB2       */
  void *             local_sb_ptr = NULL;
  uecomdef_status_e_type status=SUCCESS;
  uint32 pdu_length;

  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return FAILURE;
  }

  if(sib >= rrc_ExtensionType)
  {
    WRRC_MSG0_ERROR("Invalid SIB Type");
    return FAILURE;
  }

  /* Decode the sb1/sb2 to the local copy - For this, first we need the PDU
  length in bytes. Divide the PDU lenth in bits with 8 and add 1 since
  we can err on the high side for the length. */
  pdu_length = GET_PDU_LENGTH_IN_BYTES
                (sib_events_ptr->event[index].tsib[sib].sib_tot_length);

  if(sib == rrc_SB1)
  {
    /* Call rrcasn1_decode_pdu to do ASN1 decoding. Since it returns a 
    void pointer, typecast it to the SB1's type. */
    sb1_ptr =
      ((rrc_SysInfoTypeSB1 *)rrcasn1_decode_pdu( 
        (void *)(sib_events_ptr->event[index].
                 tsib[rrc_SB1].sib_data_ptr), /* Encoded MIB */
        rrc_SysInfoTypeSB1_PDU,                            /* SB1 PDU number        */
        pdu_length                                         /* SB1 Length in bytes   */
        ));
    local_sb_ptr = (void *) sb1_ptr;

    sib_events_ptr->event[index].tempholder.sb1_decoded=TRUE;
  }
  else
  {
    /* Call rrcasn1_decode_pdu to do ASN1 decoding. Since it returns a 
    void pointer, typecast it to the SB2's type. */
    sb2_ptr =
      ((rrc_SysInfoTypeSB2 *)rrcasn1_decode_pdu( 
        (void *)(sib_events_ptr->event[index].
               tsib[rrc_SB2].sib_data_ptr), /* Encoded MIB */
        rrc_SysInfoTypeSB2_PDU,                          /* SB1 PDU number        */
        pdu_length                                       /* SB1 Length in bytes   */
        ));
    local_sb_ptr = (void *) sb2_ptr;

    sib_events_ptr->event[index].tempholder.sb2_decoded=TRUE;
  }

  if(local_sb_ptr == NULL )
  {
    /* Decode did not work */
    WRRC_MSG0_HIGH("SB decode failed");

    /*Reset all the variables for this SIB so that we can try to
    read it again */
    sib_events_ptr->event[index].tsib[sib].sib_received = FALSE;
    sib_events_ptr->event[index].tsib[sib].sib_decoded = FALSE;
    sib_events_ptr->event[index].tsib[sib].seg_count = 0;
    sib_events_ptr->event[index].tsib[sib].seg_index = 
      (MAX_SEG_INDEX_FOR_A_SIB + 1);
    sib_events_ptr->event[index].tsib[sib].sib_tot_length = 0;
  }
  else
  {
    /* check if scheduling info is received for all sibs */
    if(FAILURE == rrcsib_check_if_scheduling_info_received_for_all_sibs(local_sb_ptr, sib, index))
    {
      status= FAILURE;
    }
    else
    {
      rrcsib_update_sched_info_from_mib_sb((uint32)sib, local_sb_ptr, index);
    }
  }

  /* delete the allocated memory for the scehduling block */
  if(local_sb_ptr != NULL)
  {
    if(sib == rrc_SB1)
    { 
      rrc_sibdb_free_sib( rrc_SysInfoTypeSB1_PDU,
                          local_sb_ptr);
    }
    else
    {
      rrc_sibdb_free_sib( rrc_SysInfoTypeSB2_PDU,
                          local_sb_ptr);
    }
    local_sb_ptr = NULL;
  }
  return status;
}


/*===========================================================================

FUNCTION DECODE_COMPLETED_SIBS_FOR_BPLMN

DESCRIPTION

  This function checks the sib_events database for the given index
  and for each SIB that has been completely received, it calls
  the ASN.1 decode function to decode the SIB and save it in the
  correct SIB database index. Only if the SIB database index is known
  for the received SIBs this function should be called.
  
DEPENDENCIES
 
  None.

RETURN VALUE
 
  None.

SIDE EFFECTS

  None.
        
===========================================================================*/
static boolean decode_completed_sibs_for_bplmn
(
  sib_events_index_e_type index  /* Index into the sib_events database. */
)
{
  uint32 count; /* local counter */
  boolean ret_val = FALSE;
  uint32 sib1_pdu_length;

#ifdef FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
  uint32 sb1_pdu_length;
  uint32 sb2_pdu_length;
  rrc_SIB_ReferenceList * sb_ptr = NULL;
  rrcsib_sib_type_vtag sib_type_tag;
#endif
  uint32 ix=0;
  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return FALSE;
  }

  /* Look at each SIB to see if it has been completely received
     but not yet decoded. */
  for(count = 0; count < RRC_TOT_SIBS; count++)
  {
    if((sib_events_ptr->event[index].tsib[count].sib_needed == TRUE) &&
       (sib_events_ptr->event[index].tsib[count].sib_received == TRUE) &&
       (sib_events_ptr->event[index].tsib[count].sib_decoded == FALSE)
      )
    {
      /* This is a SIB that is needed and received, but not decoded.
         switch on the SIB type and decode the SIB to the correct
         sib pointer */

      switch( count )
      {
        case rrc_SIB1 :

          if (rrcsib_bplmn_sibs.sib1 == NULL)
          {
            WRRC_MSG0_HIGH("WTOW BPLMN: Storing SIB1 in bplmn sibs database");

            /* Get the PDU length in bytes */
            sib1_pdu_length = GET_PDU_LENGTH_IN_BYTES
                              (sib_events_ptr->event[index].tsib[rrc_SIB1].
                               sib_tot_length);

            /* Decode and store SIB1 in bplmn sib database */
            rrcsib_bplmn_sibs.sib1 = 
            (rrc_SysInfoType1 *)rrcasn1_decode_pdu( 
                               (void *)(sib_events_ptr->event[index].
                               tsib[rrc_SIB1].sib_data_ptr), /* Encoded SIB3 */
                               rrc_SysInfoType1_PDU, /* SIB3's PDU number         */
                               sib1_pdu_length       /* SIB3's length in bytes    */
                               );

            if(rrcsib_bplmn_sibs.sib1 == NULL)
            {
               WRRC_MSG0_ERROR("SIB1 decoding failed");
            }

            //call csp function to know if LAC is valid or not
            if(rrccsp_check_sib1_validity(rrcsib_bplmn_sibs.sib1) != TRUE)
            {
              WRRC_MSG0_HIGH("LAC not valid from SIB1 (BPLMN), fail SIB reading");
            
              /* Post-process the failed event */
              rrcsib_post_process_failed_event
                ( index, /* Event Index                 */
                  TRUE,  /* Confirm Command is required */
                  SIB_EVENT_FAILED_INVALID_SIBS, /* Failure Cause       */
                  TRUE   /* Tear down S_BCCH if it's up */
                );
            }            
            return TRUE;
          }
          else
          {
            WRRC_MSG0_HIGH("WTOW BPLMN:SIB1 already stored in BPLMN DB");
          }
          break;
        
        case rrc_SB1:

          if (rrcsib_bplmn_sibs.sb1 == NULL)
          {
            WRRC_MSG0_HIGH("WTOW BPLMN: Storing SB1 in bplmn sibs database");
  
            /* Get the PDU length in bytes */
            sb1_pdu_length = GET_PDU_LENGTH_IN_BYTES
                              (sib_events_ptr->event[index].tsib[rrc_SB1].
                               sib_tot_length);
  
            /* Decode and store SIB1 in bplmn sib database */
            rrcsib_bplmn_sibs.sb1 = 
            (rrc_SysInfoTypeSB1*)rrcasn1_decode_pdu( 
                               (void *)(sib_events_ptr->event[index].
                               tsib[rrc_SB1].sib_data_ptr), /* Encoded SIB3 */
                               rrc_SysInfoTypeSB1_PDU, /* SIB3's PDU number         */
                               sb1_pdu_length       /* SIB3's length in bytes    */
                               );

            if(rrcsib_bplmn_sibs.sb1 == NULL)
            {
               WRRC_MSG0_ERROR("SB1 decoding failed");
            return TRUE;
            }
          
#ifdef FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
            if((sib_events_ptr->event[index].sib_scan_type != RRC_SIB_WTOW_AUTO_BPLMN_SCAN) 
             &&(sib_events_ptr->event[index].sib_scan_type != RRC_SIB_AUTO_BPLMN_SCAN)
#ifdef FEATURE_LTE_TO_WCDMA
             &&(!rrccsp_check_cgi_scan())
#endif
             )
            {
              sb_ptr = &rrcsib_bplmn_sibs.sb1->sib_ReferenceList;
              ix=0;
              for(ix=0;ix< sb_ptr->n;ix++)
              {
                rrcsib_get_sib_type_from_sib_type_and_tag(&sb_ptr->elem[ix].sib_Type,
                                                      &sib_type_tag);
                if(sib_type_tag.sib_type == rrc_SIB5 ||
                    sib_type_tag.sib_type == rrc_SIB5bis)
                {
                  break;
                }
              }
    
              if((ix < sb_ptr->n) && 
                 ((sib_type_tag.sib_type == rrc_SIB5)||
                 (sib_type_tag.sib_type == rrc_SIB5bis)))
              {
                WRRC_MSG0_HIGH("BAND_TYPE_DETECTION - SIB5/SIB5bis scheduled in SB1 ");
                sib_events_ptr->event[index].tsib[rrc_SB2].sib_needed = FALSE;
              }
            }
#endif
          }
          else
          {
            WRRC_MSG0_HIGH("WTOW BPLMN:SB1 already stored in BPLMN DB");
          }

          if(FAILURE == rrcsib_process_scheduling_block_sched_info_before_sib_db_decoded
            (index, rrc_SB1))
          {
            rrcsib_process_sib_scheduling_info_incomplete(index); 
            return TRUE;
          }
        
          break;

        case rrc_SB2:

          if (rrcsib_bplmn_sibs.sb2 == NULL)
          {
            WRRC_MSG0_HIGH("WTOW BPLMN: Storing SB2 in bplmn sibs database");
  
            /* Get the PDU length in bytes */
            sb2_pdu_length = GET_PDU_LENGTH_IN_BYTES
                              (sib_events_ptr->event[index].tsib[rrc_SB2].
                               sib_tot_length);
  
            /* Decode and store SIB1 in bplmn sib database */
            rrcsib_bplmn_sibs.sb2 = 
            (rrc_SysInfoTypeSB2*)rrcasn1_decode_pdu( 
                               (void *)(sib_events_ptr->event[index].
                               tsib[rrc_SB2].sib_data_ptr), /* Encoded SIB3 */
                               rrc_SysInfoTypeSB2_PDU, /* SIB3's PDU number         */
                               sb2_pdu_length       /* SIB3's length in bytes    */
                               );

            if(rrcsib_bplmn_sibs.sb2 == NULL)
            {
              WRRC_MSG0_ERROR("SB2 decoding failed");
              return TRUE;
            }

#ifdef FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
            if((sib_events_ptr->event[index].sib_scan_type != RRC_SIB_WTOW_AUTO_BPLMN_SCAN)
            &&(sib_events_ptr->event[index].sib_scan_type != RRC_SIB_AUTO_BPLMN_SCAN) 
#ifdef FEATURE_LTE_TO_WCDMA
            &&(!rrccsp_check_cgi_scan())
#endif
              )
            {
              sb_ptr = &rrcsib_bplmn_sibs.sb2->sib_ReferenceList;
              ix=0;
              for(ix=0;ix< sb_ptr->n;ix++)
              {
                rrcsib_get_sib_type_from_sib_type_and_tag(&sb_ptr->elem[ix].sib_Type,
                                                      &sib_type_tag);
                if(sib_type_tag.sib_type == rrc_SIB5 ||
                    sib_type_tag.sib_type == rrc_SIB5bis)
                {
                  break;
                }
              }
    
              if((ix < sb_ptr->n)&&
                  ((sib_type_tag.sib_type == rrc_SIB5)||
                    (sib_type_tag.sib_type == rrc_SIB5bis)))
              {
                WRRC_MSG0_HIGH("BAND_TYPE_DETECTION - SIB5/SIB5bis scheduled in SB2 ");
                sib_events_ptr->event[index].tsib[rrc_SB1].sib_needed = FALSE;
              }
            }
#endif
          }
          else
          {
            WRRC_MSG0_HIGH("WTOW BPLMN:SB2 already stored in BPLMN DB");
          }
  
          if(FAILURE == rrcsib_process_scheduling_block_sched_info_before_sib_db_decoded
             (index, rrc_SB2))
          {
            rrcsib_process_sib_scheduling_info_incomplete(index); 
            return TRUE;
          }
          break;

        default:
          WRRC_MSG0_HIGH("WTOW BPLMN: SIB not SIB1.Ingoring"); 
          break;
      }
    }
  }
  return ret_val;
}

/*===========================================================================

FUNCTION rrcsib_check_lac_validity_from_sib1

DESCRIPTION

  This function checksLAC validity from SIB1

DEPENDENCIES
 
  None.

RETURN VALUE
 
SUCCESS:  LAC is vaid from SIB1
FAILURE:  otherwise

SIDE EFFECTS

  None.
        
===========================================================================*/
static uecomdef_status_e_type rrcsib_check_lac_validity_from_sib1
(
  uint32 dbindex,                /* Index to the sib database. */
  sib_events_index_e_type index  /* Index into the sib_events database. */
)
{
  uecomdef_status_e_type status = FAILURE;
  boolean sib1_read = FALSE;      /* initialize sib1 read flag to false  */
  boolean sbcch_teardown_required = TRUE; /* If the event is for Serving BCCH*/
  rrc_sib_status_e_type sib_status = SIB_EVENT_FAILED_OTHER;


  if(EVENT_INDEX_VALID(index) == FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return FAILURE;
  }
  /* store local information that sib 1 are needed, received and decoded rather than use
   * long loops everywhere 
   */
  if((sib_events_ptr->event[index].tsib[rrc_SIB1].sib_needed == TRUE) &&
    (sib_events_ptr->event[index].tsib[rrc_SIB1].sib_received == TRUE) &&
    (sib_events_ptr->event[index].tsib[rrc_SIB1].sib_decoded == TRUE))
  {
     sib1_read = TRUE;
  }
  
  if(sib1_read == FALSE)
  {
    status = SUCCESS;
  }
  else
  {
    if( sibdb.cell[dbindex].sibs.sib1== NULL )
    {
      /* ASN.1 decode the PDU */
      sibdb.cell[dbindex].sibs.sib1=  rrcasn1_decode_pdu( 
        (void *)(sibdb.cell[dbindex].sibs.sib1_encoded_ptr), /* Encoded SIB      */
        rrc_SysInfoType1_PDU,                                                       /* SIB's PDU number */
        sibdb.cell[dbindex].sibs.sib1_encoded_pdu_length   /* SIB's Length in bytes */
         );
      if(sibdb.cell[dbindex].sibs.sib1== NULL)
      {
        status = FAILURE;
        WRRC_MSG0_ERROR("SIB_HEAP:Asn.1 decode failed from SIB1");
      }
      WRRC_MSG0_HIGH("SIB_HEAP:Decode Sib1 in sibdb");
    }
    else
    {
      WRRC_MSG1_HIGH("SIB_HEAP:SIB1 already decoded,sib1_ptr=%d", sibdb.cell[dbindex].sibs.sib1);
    }

    /* call csp function to know if LAC is valid or not*/
    if(rrccsp_check_sib1_validity(sibdb.cell[dbindex].sibs.sib1) == TRUE)
    {
      status = SUCCESS;
    }
    else
    {/*handle fail case*/
      if((sib_events_ptr->event[index].event_name == RRCSIB_GET_SPECIFIC_SIBS) &&
         (sib_events_ptr->event[index].req_proc == RRC_PROCEDURE_CSP))
      {
        /* W Cell Selection or GtoW Cell Reselection */
        if(sib_events_ptr->event[index].for_serving_cell == TRUE)
        {
          sbcch_teardown_required = TRUE;
        }
        else
        {
         sbcch_teardown_required = FALSE;
        }
        sib_status = SIB_EVENT_FAILED_INVALID_SIBS;
      }

      if(sib_events_ptr->event[index].event_name == RRCSIB_UPDATE_SIBS)
      {
        //set the cause such that we trigger OOS
        sib_status = SIB_EVENT_FAILED_LOWER_LAYERS;
      }
      /* Post-process the failed event */
      rrcsib_post_process_failed_event
         ( index, /* Event Index                 */
                TRUE,  /* Confirm Command is required */
                sib_status, /* Failure Cause       */
                sbcch_teardown_required   /* Tear down S_BCCH if it's up */
              );
      status = FAILURE;
    }
  }
  return (status);
}
/*===========================================================================

FUNCTION DECODE_COMPLETED_SIBS

DESCRIPTION

  This function checks the sib_events database for the given index
  and for each SIB that has been completely received, it calls
  the ASN.1 decode function to decode the SIB and save it in the
  correct SIB database index. Only if the SIB database index is known
  for the received SIBs this function should be called.
  
DEPENDENCIES
 
  None.

RETURN VALUE
 
  None.

SIDE EFFECTS

  None.
        
===========================================================================*/
static void decode_completed_sibs
(
  sib_events_index_e_type index  /* Index into the sib_events database. */
)
{
  uint32 count; /* local counter */
  uint32 dbindex; /* Index to the sib database. */
  boolean decode_succeeded; /* Indicate if a particular SIB
                               decode succeeded or not. */
  rrc_plmn_identity_type plmn;
  rrc_sib_status_e_type status = SIB_EVENT_SUCCEEDED;

  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return;
  }

  /* Make sure that the database index has been found */
  if(!(sib_events_ptr->event[index].sib_db_index_found))
  {
    WRRC_MSG0_ERROR("Can't decode SIBs till index is found");
    return;
  }
  
  WRRC_MSG1_HIGH("Cells stored %d, decoding SIBs", sibdb.num_stored_cells);  

  /* Copy the database index for this cell */
  dbindex = sib_events_ptr->event[index].sib_db_index;

  if ( FAILURE == rrcsib_check_sib5_sib6_barred_criteria(dbindex, index))
  {
    WRRC_MSG0_HIGH("Cell-Barred Criteria met.  Bar cell");
    return;
  }


  if(rrcsib_check_lac_validity_from_sib1(dbindex, index) == FAILURE)
  {
    WRRC_MSG0_HIGH("LAC not valid from SIB1, fail SIB reading");
    return;
  }

  if((sib_events_ptr->event[index].tsib[rrc_SB1].sib_needed == TRUE) &&
    (sib_events_ptr->event[index].tsib[rrc_SB1].sib_received == TRUE) &&
    (sib_events_ptr->event[index].tsib[rrc_SB1].sib_decoded == FALSE)
  )
  {

    decode_succeeded = 
    rrcsib_decode_and_save_sib_for_current_event((void *)&(sibdb.cell[dbindex].sibs.sb1),
                                        index, dbindex, rrc_SB1, 
                                        rrc_SysInfoTypeSB1_PDU
                                        ,sibdb.cell[dbindex].sibs.sb1_encoded_ptr
                                        ,&sibdb.cell[dbindex].sibs.sb1_encoded_pdu_length
                                        );

                                          
    if(decode_succeeded == TRUE)
    {
      sib_events_ptr->event[index].tempholder.sb1_decoded = TRUE;
      if(sib_events_ptr->event[index].event_name == RRCSIB_UPDATE_SIBS) 
      {
        /* This means a SIB has changed for the serving cell
           and it has been decoded correctly. */
        rrc_sibdb_save_active_cell_sib_change(rrc_SB1 );        
      }

      //check if scheduling info is received for all sibs
      if(FAILURE == rrcsib_check_if_scheduling_info_received_for_all_sibs((void *)sibdb.cell[dbindex].sibs.sb1,
                                                                          rrc_SB1, index))
      {
        rrcsib_process_sib_scheduling_info_incomplete(index); 
        return;
      }
      else
      {
        rrcsib_update_sched_info_from_mib_sb((uint32)rrc_SB1,
                                             sibdb.cell[dbindex].sibs.sb1, index);
      }

      /* When we receive a scheduling block, we can compare the
      value tags in that block to the value tags already stored
        for this event. */
      update_event_for_received_value_tags(
        rrc_SB1, /* Value tags from Scheduling block 1 */
        index,                /* Event index                        */
        dbindex               /* SIB Database index                 */
        );
    }
  }

  if( (sib_events_ptr->event[index].tsib[rrc_SB2].sib_needed == TRUE) &&
    (sib_events_ptr->event[index].tsib[rrc_SB2].sib_received == TRUE) &&
    (sib_events_ptr->event[index].tsib[rrc_SB2].sib_decoded == FALSE)
  )
  {
    decode_succeeded = 
    rrcsib_decode_and_save_sib_for_current_event((void *)&(sibdb.cell[dbindex].sibs.sb2),
                                        index, dbindex, rrc_SB2, 
                                        rrc_SysInfoTypeSB2_PDU
                                        ,sibdb.cell[dbindex].sibs.sb2_encoded_ptr
                                        ,&sibdb.cell[dbindex].sibs.sb2_encoded_pdu_length
                                     );

                                          
    if(decode_succeeded == TRUE)
    {
      sib_events_ptr->event[index].tempholder.sb2_decoded = TRUE;
      if(sib_events_ptr->event[index].event_name == RRCSIB_UPDATE_SIBS) 
      {
        /* This means a SIB has changed for the serving cell
           and it has been decoded correctly. */
        rrc_sibdb_save_active_cell_sib_change(rrc_SB2);        
      }

      //check if scheduling info is received for all sibs
      if(FAILURE == rrcsib_check_if_scheduling_info_received_for_all_sibs((void *)sibdb.cell[dbindex].sibs.sb2, 
                                                                          rrc_SB2, index))
      {
        rrcsib_process_sib_scheduling_info_incomplete(index); 
        return;
      }
      else
      {
        rrcsib_update_sched_info_from_mib_sb((uint32)rrc_SB2,
                                             sibdb.cell[dbindex].sibs.sb2, index);
      }

      /* When we receive a scheduling block, we can compare the
      value tags in that block to the value tags already stored
        for this event. */
      update_event_for_received_value_tags(
        rrc_SB2, /* Value tags from Scheduling block 1 */
        index,                /* Event index                        */
        dbindex               /* SIB Database index                 */                    
        );
    }
  }

  /* Look at each SIB to see if it has been completely received
  but not yet decoded. */
  for( count=0; count<RRC_TOT_SIBS; count++)
  {
    if( (sib_events_ptr->event[index].tsib[count].sib_needed == TRUE) &&
        (sib_events_ptr->event[index].tsib[count].sib_received == TRUE) &&
        ((sib_events_ptr->event[index].tsib[count].sib_decoded == FALSE)||
        ((count == (uint32)rrc_SIB7)&&
         (sib_events_ptr->event[index].tsib[count].sib_tot_length != 0)))
      )
    {
      /* This is a SIB that is needed and received, but not decoded.
      switch on the SIB type and decode the SIB to the correct
      sib pointer */

      decode_succeeded = FALSE; /* Set this to FALSE each time. if a decode succeeds, 
                                   it will be set to TRUE in the case statements below. */

      switch( count )
      {
        case rrc_MIB :
          // call the function that decodes mib and returns true/false based on decode status
          decode_succeeded = 
          rrcsib_decode_and_save_sib_for_current_event((void *)&(sibdb.cell[dbindex].sibs.mib),
                                              index, dbindex, rrc_MIB,
                                              rrc_MasterInformationBlock_PDU
                                              ,sibdb.cell[dbindex].sibs.mib_encoded_ptr
                                              ,&sibdb.cell[dbindex].sibs.mib_encoded_pdu_length
                                              );
          break;
 
        case rrc_SIB1 :
          // call the function that decodes sib1 and returns true/false based on decode status
          decode_succeeded = 
          rrcsib_decode_and_save_sib_for_current_event((void *)&(sibdb.cell[dbindex].sibs.sib1),
                                              index, dbindex, rrc_SIB1,
                                              rrc_SysInfoType1_PDU
                                              ,sibdb.cell[dbindex].sibs.sib1_encoded_ptr
                                              ,&sibdb.cell[dbindex].sibs.sib1_encoded_pdu_length
                                              );
          if(decode_succeeded == TRUE)
          {
            if(rrcsib_check_lac_validity_from_sib1(dbindex, index) == FAILURE)
            {
              WRRC_MSG0_HIGH("LAC not valid from SIB1, fail SIB reading");
              return;
            }
          }
          break;

        case rrc_SIB2 :
          // call the function that decodes sib2 and returns true/false based on decode status
          decode_succeeded = 
          rrcsib_decode_and_save_sib_for_current_event((void *)&(sibdb.cell[dbindex].sibs.sib2),
                                              index, dbindex, rrc_SIB2,
                                              rrc_SysInfoType2_PDU
                                              ,sibdb.cell[dbindex].sibs.sib2_encoded_ptr
                                              ,&sibdb.cell[dbindex].sibs.sib2_encoded_pdu_length
                                              );
          break;

        case rrc_SIB3 :
          // call the function that decodes sib3 and returns true/false based on decode status
          decode_succeeded = 
          rrcsib_decode_and_save_sib_for_current_event((void *)&(sibdb.cell[dbindex].sibs.sib3),
                                              index, dbindex, rrc_SIB3,
                                              rrc_SysInfoType3_PDU
                                              ,sibdb.cell[dbindex].sibs.sib3_encoded_ptr
                                              ,&sibdb.cell[dbindex].sibs.sib3_encoded_pdu_length
                                              );
          if((decode_succeeded == TRUE)&&
             (sib_events_ptr->event[index].for_serving_cell != TRUE)&&
             (!(sib_events_ptr->event[index].tempholder.sib3_decoded)))            
          {
            rrc_translate_gsm_map_plmn_id(&(sibdb.cell[dbindex].sibs.mib->plmn_Type.u.gsm_MAP->plmn_Identity),&plmn);

            /*Do not perform suitablity check for Cell ID in DCH scans*/
            if(sib_events_ptr->event[index].sib_scan_type != RRC_SIB_CELL_ID_READ_IN_DCH_SCAN)
            {
              rrccsp_initial_suitability_check(sibdb.cell[dbindex].sibs.mib,
                                               sibdb.cell[dbindex].sibs.sib3,
                                               plmn,
#ifdef FEATURE_FEMTO_CSG
                                               sib_events_ptr->event[index].csg_id,
#endif
                                               &status);
            }
            if(status != SIB_EVENT_SUCCEEDED)
            {
              rrcsib_post_process_failed_event((sib_events_index_e_type)index,
                                                TRUE,
                                                status, 
                                                TRUE);
              return;
            }
          }
          break;

        case rrc_SIB4 :
          // call the function that decodes sib4 and returns true/false based on decode status
          decode_succeeded = 
          rrcsib_decode_and_save_sib_for_current_event((void *)&(sibdb.cell[dbindex].sibs.sib4),
                                              index, dbindex, rrc_SIB4,
                                              rrc_SysInfoType4_PDU
                                              ,sibdb.cell[dbindex].sibs.sib4_encoded_ptr
                                              ,&sibdb.cell[dbindex].sibs.sib4_encoded_pdu_length
                                              );
          break;
      
        case rrc_SIB5 :
          // call the function that decodes sib5 and returns true/false based on decode status
          decode_succeeded = 
          rrcsib_decode_and_save_sib_for_current_event((void *)&(sibdb.cell[dbindex].sibs.sib5),
                                              index, dbindex, rrc_SIB5,
                                              rrc_SysInfoType5bis_PDU
                                              ,sibdb.cell[dbindex].sibs.sib5_encoded_ptr
                                              ,&sibdb.cell[dbindex].sibs.sib5_encoded_pdu_length
                                              );
          if(TRUE == decode_succeeded)
          {
            if(FAILURE == rrcsib_check_sib5_sib6_barred_criteria(dbindex, index))
            {
              WRRC_MSG0_HIGH("Cell-Barred Criteria met for SIB5 decode");
              return;
            }
          }
          break;
      
        case rrc_SIB6 :
          // call the function that decodes sib6 and returns true/false based on decode status
          decode_succeeded = 
          rrcsib_decode_and_save_sib_for_current_event((void *)&(sibdb.cell[dbindex].sibs.sib6),
                                              index, dbindex, rrc_SIB6,
                                              rrc_SysInfoType6_PDU
                                              ,sibdb.cell[dbindex].sibs.sib6_encoded_ptr
                                              ,&sibdb.cell[dbindex].sibs.sib6_encoded_pdu_length
                                              );
          if(TRUE == decode_succeeded)
          {
            if(FAILURE == rrcsib_check_sib5_sib6_barred_criteria(dbindex, index))
            {
              WRRC_MSG0_HIGH("Cell-Barred Criteria met for SIB6 decode");
              return;
            }
          }
          break;

        case rrc_SIB7 :
          // call the function that decodes sib7 and returns true/false based on decode status
          decode_succeeded = 
          rrcsib_decode_and_save_sib_for_current_event((void *)&(sibdb.cell[dbindex].sibs.sib7),
                                              index, dbindex, rrc_SIB7,
                                              rrc_SysInfoType7_PDU
                                              ,sibdb.cell[dbindex].sibs.sib7_encoded_ptr
                                              ,&sibdb.cell[dbindex].sibs.sib7_encoded_pdu_length
                                              );
          break;

        case rrc_SIB11 :
          // call the function that decodes sib11 and returns true/false based on decode status
          decode_succeeded = 
          rrcsib_decode_and_save_sib_for_current_event((void *)&(sibdb.cell[dbindex].sibs.sib11),
                                              index, dbindex, rrc_SIB11,
                                              rrc_SysInfoType11_PDU
                                              ,sibdb.cell[dbindex].sibs.sib11_encoded_ptr
                                              ,&sibdb.cell[dbindex].sibs.sib11_encoded_pdu_length
                                              );
          break;

        case rrc_SIB12 :
          // call the function that decodes sib12 and returns true/false based on decode status
          decode_succeeded = 
          rrcsib_decode_and_save_sib_for_current_event((void *)&(sibdb.cell[dbindex].sibs.sib12),
                                              index, dbindex, rrc_SIB12,
                                              rrc_SysInfoType12_PDU
                                              ,sibdb.cell[dbindex].sibs.sib12_encoded_ptr
                                              ,&sibdb.cell[dbindex].sibs.sib12_encoded_pdu_length
                                              );
          break;

        case rrc_SIB18 :
          // call the function that decodes sib18 and returns true/false based on decode status
          decode_succeeded = 
          rrcsib_decode_and_save_sib_for_current_event((void *)&(sibdb.cell[dbindex].sibs.sib18),
                                              index, dbindex, rrc_SIB18,
                                              rrc_SysInfoType18_PDU
                                              ,sibdb.cell[dbindex].sibs.sib18_encoded_ptr
                                              ,&sibdb.cell[dbindex].sibs.sib18_encoded_pdu_length
                                              );
          break;

        case rrc_SIB11bis :
          // call the function that decodes sib18 and returns true/false based on decode status
          decode_succeeded = 
          rrcsib_decode_and_save_sib_for_current_event((void *)&(sibdb.cell[dbindex].sibs.sib11bis),
                                              index, dbindex, rrc_SIB11bis,
                                              rrc_SysInfoType11bis_PDU
                                              ,sibdb.cell[dbindex].sibs.sib11bis_encoded_ptr
                                              ,&sibdb.cell[dbindex].sibs.sib11bis_encoded_pdu_length
                                              );
          break;

#ifdef FEATURE_WCDMA_TO_LTE
        case rrc_SIB19 :
          // call the function that decodes sib18 and returns true/false based on decode status
          decode_succeeded = 
          rrcsib_decode_and_save_sib_for_current_event((void *)&(sibdb.cell[dbindex].sibs.sib19),
                                              index, dbindex, rrc_SIB19,
                                              rrc_SysInfoType19_PDU
                                              ,sibdb.cell[dbindex].sibs.sib19_encoded_ptr
                                              ,&sibdb.cell[dbindex].sibs.sib19_encoded_pdu_length
                                              );
          break;
#endif

        default:

          WRRC_MSG1_ERROR("Unknown SIB%d", count);
          break;
   
      }/* end switch */

      /* If any new sibs have been decoded we may have to
      inform other procedures about it. This is done only
      if the sibs have changed in the serving cell. So
      check if the event in progress is a RRCSIB_UPDATE_SIBS
      event. If so, ask the sibdb module to save this 
      SIB change. */
      if( sib_events_ptr->event[index].event_name == RRCSIB_UPDATE_SIBS )
      {
        if( decode_succeeded == TRUE )
        {
          /* This means a SIB has changed for the serving cell
          and it has been decoded correctly. */
          rrc_sibdb_save_active_cell_sib_change( (rrc_SIB_type)count );        
        }
      }

    }/* end if (sibs need to be decoded) */
  }/* end for (RRC_TOT_SIBS) */
}/* end function decode_completed_sibs */


/*===========================================================================

FUNCTION RRCSIB_CHECK_IF_BAND_SUPPORTED

DESCRIPTION

  This function will be used to find if a particular band class 
  is supported by the UE or not.
  
DEPENDENCIES

  None.

RETURN VALUE

  TRUE - If the band is supported
  FALSE - If band not supported
  
SIDE EFFECTS

  None

===========================================================================*/
boolean rrcsib_check_if_band_supported
(
  rrc_csp_band_class_type band_class,
  boolean for_serving_cell, 
  rrc_plmn_identity_type plmn
)
{
  if(TRUE == for_serving_cell)
  {
    return rrccsp_is_current_band(band_class,plmn);
  }
  else
  {
    return rrccsp_is_supported_band(band_class);
  }
}

/*===========================================================================

FUNCTION rrcsib_check_if_plmn_is_gsm_map

DESCRIPTION

  This function will check whether the PLMN is GSM MAP.or not
  
DEPENDENCIES
 
  None.

RETURN VALUE

  TRUE: If the PLMN is GSM MAP.
  FALSE: other wise

SIDE EFFECTS

  Serving cell index in the SIB database may be changed.
  
===========================================================================*/
static boolean  rrcsib_check_if_plmn_is_gsm_map
(
  sib_events_index_e_type index,
  boolean sbcch_teardown_required
)
{
  boolean status = TRUE;
  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return FALSE;
  }
  
  if(sib_events_ptr->event[index].tempholder.mib != NULL)
  {
    if(!((RRCGET_PLMN_TYPE((&sib_events_ptr->event[index].tempholder.mib->plmn_Type))
                                                      == RRCGET_PLMN_TYPE_TAG(gsm_MAP)) ||
         (RRCGET_PLMN_TYPE((&sib_events_ptr->event[index].tempholder.mib->plmn_Type)) 
                                                      == RRCGET_PLMN_TYPE_TAG(gsm_MAP_and_ANSI_41))
        )
      )
    {
      WRRC_MSG0_HIGH("Acquired cell PLMN not GSM-MAP!");
      /* No need process this event further, just send a failure
        to the procedure that requested this sib event. */
      status = FALSE;
      rrcsib_post_process_failed_event( index, /* Event Index                 */
                                            TRUE,  /* Confirm Command is required */
                                            SIB_EVENT_FAILED_INVALID_SIBS, /* Failure cause */
                                            sbcch_teardown_required   /* Tear down S_BCCH if it's up */
                                          );
    }
  }
  else
  {
    WRRC_MSG0_ERROR("tempholder.mib ptr is NULL");
    status = FALSE;
  }
  return status;
}
/*===========================================================================

FUNCTION UPDATE_VTAGS_OF_SIBS_IN_SB1_SB2_IF_RECEIVED

DESCRIPTION

  This function updates the v-tags of sibs in sb1 and sb2
  
DEPENDENCIES
 
  None.

RETURN VALUE

  Boolean

SIDE EFFECTS

  None
  
===========================================================================*/
static boolean update_vtags_of_sibs_in_sb1_sb2_if_received
(
  sib_events_index_e_type index 
)
{

  //if SB1 is already received, i.e. v-tag matches, then update v-tags immediately
  //for the received value tags
  if((sib_events_ptr->event[index].tsib[rrc_SB1].sib_received == TRUE)&&
     (sib_events_ptr->event[index].tsib[rrc_SB1].sib_decoded == TRUE))
  {
    //since we alredy have sb1 decoded, set this flag to true
    sib_events_ptr->event[index].tempholder.sb1_decoded=TRUE;
    /*This function call is to make sure that SB1 is decoded before entering
      update_event_for_received_value_tags which expects SB1 in decoded form*/
    if(rrc_sibdb_return_sib_for_db_index(rrc_SB1,sib_events_ptr->event[index].sib_db_index) == NULL)
    {
      ERR_FATAL("SB1 is received and decoded but not available in SIB DB",0,0,0);
    }

    /* Now do the value-tag comparisons */
    update_event_for_received_value_tags( 
    rrc_SB1,       /* For the SB1 */
    index,                      /* Event Index */
    sib_events_ptr->event[index].sib_db_index);


    if(FAILURE == rrcsib_check_if_scheduling_info_received_for_all_sibs
      (sibdb.cell[sib_events_ptr->event[index].sib_db_index].sibs.sb1, 
       rrc_SB1, 
       index))
    {
      rrcsib_process_sib_scheduling_info_incomplete(index); 
      return TRUE;
    }
  }

  //if SB2 is already received, i.e. v-tag matches, then update v-tags immediately
  //for the received value tags
  if((sib_events_ptr->event[index].tsib[rrc_SB2].sib_received == TRUE)&&
     (sib_events_ptr->event[index].tsib[rrc_SB2].sib_decoded == TRUE))
  {
    //since we alredy have sb1 decoded, set this flag to true
    sib_events_ptr->event[index].tempholder.sb2_decoded=TRUE;
    /*This function call is to make sure that SB2 is decoded before entering
      update_event_for_received_value_tags which expects SB2 in decoded form*/
    if(rrc_sibdb_return_sib_for_db_index(rrc_SB2,sib_events_ptr->event[index].sib_db_index) == NULL)
    {
      ERR_FATAL("SB2 is received and decoded but not available in SIB DB",0,0,0);
    }
    /* Now do the value-tag comparisons */
    update_event_for_received_value_tags( 
    rrc_SB2,       /* For the SB2 */
    index,                      /* Event Index */
    sib_events_ptr->event[index].sib_db_index);

    if(FAILURE == rrcsib_check_if_scheduling_info_received_for_all_sibs
       (sibdb.cell[sib_events_ptr->event[index].sib_db_index].sibs.sb2, 
        rrc_SB2, 
        index))
    {
      rrcsib_process_sib_scheduling_info_incomplete(index); 
      return TRUE;
    }
  }
  return FALSE;
}
/*===========================================================================

FUNCTION PROCESS_COMPLETED_SIBS

DESCRIPTION

  This function processes SIBs that have been completely received 
  It saves the SIBs in the SIB database if the index for saving these
  SIBs is already known. If not, the SIBs are left unprocessed.
  They will be processed after the index is known.
  
  If the index for saving SIBs is not known and the PLMN id and 
  cell id have been received for this SIB, this function tries 
  to match the index for these SIBs in the SIB database. 

  Since the MIB carries the PLMN id and SIB3 carries the cell
  id, this function looks for MIB and SIB3 inorder to find the
  index. MIB and SIB3 are decoded by this function. Once the
  MIB is decoded, the value-tags are also matched if   
  SIBs have already been stored in the database for this SIB.
  
  If a match is found, SIBs can be saved at the existing Index.
  Also if SIBs are previously saved at that index, value_tag
  comparisons will be made and the event database adjusted accordingly.
  
  If a match is not found, SIBs will be saved at a new index in the
  database.
  
DEPENDENCIES
 
  None.

RETURN VALUE

  boolean

SIDE EFFECTS

  Serving cell index in the SIB database may be changed.
  
===========================================================================*/
static boolean process_completed_sibs
(
  sib_events_index_e_type index   /* Index to the SIB events database
                                  in which SIBs have been completed. */
)
{
  uint32 pdu_length; /* To store length of PDU in bytes to be passed
                     to the ASN.1 decoder. */
  uint32 dbindex;
  rrc_plmn_identity_type plmn; /* Store the PLMN from the received MIB     */
  rrc_int_cell_id_type cell;   /* Store the cell id from the received SIB3 */
  boolean non_gsm_map_plmn = FALSE; /* Indicate if the plmn found was not
                               a GSM-MAP plmn. */
  uint32 ix =0;
#ifdef FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
  rrc_SIBSb_ReferenceList *sibSb_ptr=NULL;
  rrcsib_sib_type_vtag sib_type_tag;
#endif
  rrc_MasterInformationBlock *temp_mib;
    /*Test code start*/
  rrc_int_cell_id_type cell_id;
  rrc_plmn_identity_type plmn_id;

  uint32 db_index = SIBDB_INVALID_INDEX;
  /*Test code end*/

rrc_sib_status_e_type status = SIB_EVENT_SUCCEEDED;
#ifdef FEATURE_FEMTO_CSG
  rrc_SysInfoType3 *sib3 = NULL;
#endif
  rrc_SysInfoType1 *sib1_ptr = NULL;
  rrc_MasterInformationBlock *mib_ptr = NULL;

  rrc_csp_scan_e_type curr_scan_type = rrccsp_get_curr_scan_no_f3();

  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return FALSE;
  }
  
  /* Initialize PLMN to dummy value to keep lint happy */
  plmn.mcc[0] = 0;
  plmn.mcc[1] = 0;
  plmn.mcc[2] = 0;
  plmn.num_mnc_digits = 3;
  plmn.mnc[0] = 0;
  plmn.mnc[1] = 0;
  plmn.mnc[2] = 0;

  if((sib_events_ptr->event[index].sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)||
     (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_MANUAL_SCAN)||
     (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN)||
     (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_WTOW_AUTO_BPLMN_SCAN) ||
     (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_AUTO_BPLMN_SCAN))
  {
    /* Check to see of other SIBS(currently only SIB1) is received,if MIB and SIB3 has already been stored */
    if(rrcsib_bplmn_sibs.mib != NULL)
    {
      if(TRUE == decode_completed_sibs_for_bplmn(index))
      {
        return TRUE;
      }
    }
  }

  if( sib_events_ptr->event[index].event_name == RRCSIB_UPDATE_SIBS )
  {
    rrcsib_decode_sib7_in_update_sibs(index);
  }
  if((sib_events_ptr->event[index].tsib[rrc_SIB1].sib_received)&& 
     (!(sib_events_ptr->event[index].tempholder.sib1_decoded)) &&
     (!sib_events_ptr->event[index].tsib[rrc_SIB1].sib_decoded))
  {
    rrc_SysInfoType1 *sib1_ptr;
    pdu_length = GET_PDU_LENGTH_IN_BYTES(sib_events_ptr->event[index].tsib[rrc_SIB1].sib_tot_length);
    sib1_ptr =(rrc_SysInfoType1*)rrcasn1_decode_pdu((void *)
                                                   (sib_events_ptr->event[index].
                                                     tsib[rrc_SIB1].sib_data_ptr), /* Encoded SIB1 */
                                                     rrc_SysInfoType1_PDU,/* SIB1's PDU number */
                                                     pdu_length/* SIB1's Length in bytes   */
                                                   );

    if(sib1_ptr == NULL)
    {
      /* Decode did not work */
      WRRC_MSG0_HIGH("SIB1 decode failed");
      /*Reset all the variables for this SIB so that we can try to
      read it again */
      sib_events_ptr->event[index].tsib[rrc_SIB1].sib_received = FALSE;
      sib_events_ptr->event[index].tsib[rrc_SIB1].seg_count = 0;
      sib_events_ptr->event[index].tsib[rrc_SIB1].seg_index = (MAX_SEG_INDEX_FOR_A_SIB + 1);
      sib_events_ptr->event[index].tsib[rrc_SIB1].sib_tot_length = 0;
    }
    else
    { 
      sib_events_ptr->event[index].tempholder.sib1_decoded = TRUE;

      if(sib_events_ptr->event[index].for_serving_cell == FALSE)
      {
        /*Extract PLMN from MIB */
        if(sib_events_ptr->event[index].sib_db_index_found)
        {
          uint32 dbindex = sib_events_ptr->event[index].sib_db_index;
          rrc_translate_gsm_map_plmn_id(
              &(sibdb.cell[dbindex].sibs.mib->plmn_Type.u.gsm_MAP->plmn_Identity),&plmn);
        }
        else
        {
          rrc_translate_gsm_map_plmn_id(
              &(sib_events_ptr->event[index].tempholder.mib->plmn_Type.u.gsm_MAP->plmn_Identity),&plmn);
        }
        
        if(rrcrce_abort_resel(plmn,sib1_ptr,sib_events_ptr->ngh_bcch.freq,sib_events_ptr->ngh_bcch.scr_code))
        {
          rrcsib_post_process_failed_event(index,
                                           TRUE,
                                           SIB_EVENT_FAILED_OTHER, 
                                           FALSE);
          rrc_sibdb_free_sib(rrc_SysInfoType1_PDU,(void *)sib1_ptr);
          return FALSE;
        }
      }

      if(rrccsp_check_sib1_validity(sib1_ptr) == FALSE)
      {
        rrcsib_post_process_failed_event((sib_events_index_e_type)index,
                                          TRUE,
                                          SIB_EVENT_FAILED_INVALID_SIBS, 
                                          sib_events_ptr->event[index].for_serving_cell
                                         );
        rrc_sibdb_free_sib(rrc_SysInfoType1_PDU,(void *)sib1_ptr);
        return FALSE;
      }
      if((sib_events_ptr->event[index].event_name == RRCSIB_GET_SPECIFIC_SIBS) &&
         (sib_events_ptr->event[index].req_proc == RRC_PROCEDURE_CSP) &&
         (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_NORMAL))
      {               
        if(sib_events_ptr->event[index].sib_db_index_found )
        {
          rrc_MasterInformationBlock *temp_mib_ptr = NULL;
          dbindex = sib_events_ptr->event[index].sib_db_index;
          temp_mib_ptr = rrc_sibdb_return_sib_for_db_index(rrc_MIB,dbindex);

          if(temp_mib_ptr != NULL)
          {
            if(rrccsp_check_for_lai_forbidden(sib1_ptr,temp_mib_ptr))
            { 
              rrcsib_post_process_failed_event((sib_events_index_e_type)index,
                                                TRUE,
                                                SIB_EVENT_FAILED_FB_LA, 
                                                sib_events_ptr->event[index].for_serving_cell
                                                ); 
              rrc_sibdb_free_sib(rrc_SysInfoType1_PDU,(void *)sib1_ptr);
              return FALSE;
            
            }
          }
        }
        else
        {
          if(rrccsp_check_for_lai_forbidden(sib1_ptr,sib_events_ptr->event[index].tempholder.mib))
          { 
            rrcsib_post_process_failed_event((sib_events_index_e_type)index,
                                              TRUE,
                                              SIB_EVENT_FAILED_FB_LA, 
                                              sib_events_ptr->event[index].for_serving_cell
                                              );
            rrc_sibdb_free_sib(rrc_SysInfoType1_PDU,(void *)sib1_ptr);
            return FALSE;
          }
        }         
      }
      rrc_sibdb_free_sib(rrc_SysInfoType1_PDU,(void *)sib1_ptr);
    }  
  }
  

  /* Check if this event already has a SIB database index
  for storing the SIBs. We would have the index only if
  we've already decoded the MIB and SIB 3 inorder to get the
  PLMN id and cell id of this cell. */
  if( sib_events_ptr->event[index].sib_db_index_found )
  {
    /* We can decode and store whatever SIBs we've received in
    this pass */
    MSG_LOW("Decoding received SIBs", 0, 0, 0);
    decode_completed_sibs(index);
  }
  else
  {
    /* Check if the MIB was received in this pass */
    if( (sib_events_ptr->event[index].tsib[rrc_MIB].sib_received)
      && (!(sib_events_ptr->event[index].tempholder.mib_decoded)))
    {

#ifdef FEATURE_RRC_NO_MIB_FOUND_BAR_CELL
      no_mib_bar_cell_count = 0;
#endif

      /* Decode the MIB to the local copy - For this, first we need the PDU
      length in bytes. Divide the PDU lenth in bits with 8 and add 1 since
      we can err on the high side for the length. */
      pdu_length = GET_PDU_LENGTH_IN_BYTES
                    (sib_events_ptr->event[index].tsib[rrc_MIB].
                     sib_tot_length);

      /* Call rrcasn1_decode_pdu to do ASN1 decoding. Since it returns a 
      void pointer, typecast it to the MIB's type. */
      sib_events_ptr->event[index].tempholder.mib =
        (rrc_MasterInformationBlock *)rrcasn1_decode_pdu( 
          (void *)(sib_events_ptr->event[index].
                 tsib[rrc_MIB].sib_data_ptr), /* Encoded MIB */
          rrc_MasterInformationBlock_PDU,  /* MIB's PDU number        */
          pdu_length                       /* MIB's Length in bytes   */
                        );

      if( sib_events_ptr->event[index].tempholder.mib == NULL )
      {
        /* Decode did not work */
        WRRC_MSG0_HIGH("MIB decode failed");

        /*Reset all the variables for this SIB so that we can try to
        read it again */
        sib_events_ptr->event[index].tsib[rrc_MIB].sib_received = FALSE;
        sib_events_ptr->event[index].tsib[rrc_MIB].sib_decoded = FALSE;
        sib_events_ptr->event[index].tsib[rrc_MIB].seg_count = 0;
        sib_events_ptr->event[index].tsib[rrc_MIB].seg_index = 
          (MAX_SEG_INDEX_FOR_A_SIB + 1);
        sib_events_ptr->event[index].tsib[rrc_MIB].sib_tot_length = 0;
        sib_events_ptr->sib_sched[rrc_MIB].repitition_rate = INVALID_SIB_SCHEDULE_REP;
      }
      else
      {
        /* Check to see if PLMN Type is not correct here. */
        if(rrcsib_check_if_plmn_is_gsm_map(index,FALSE) == FALSE)
        {
          return FALSE;
        }
        else
        {
          /* Either 850 or 800 or both are supported */
          rrc_translate_gsm_map_plmn_id(
              &(sib_events_ptr->event[index].tempholder.mib->plmn_Type.u.gsm_MAP->plmn_Identity),&plmn);
        }

#ifdef FEATURE_FEMTO_CSG
        if((CSG_IND_IN_MIB(sib_events_ptr->event[index].tempholder.mib))&&
            is_whitelist_valid)
        {
          sib_events_ptr->event[index].is_csg = TRUE;
        }
        WRRC_MSG1_HIGH("CSG: CSG indicator %d",sib_events_ptr->event[index].is_csg);
        /* For CSG event if it is not a CSG cell abort SIB reading*/
#endif
#ifdef FEATURE_MCC_AND_850_800_BAND_COMBINATION
        /* Check for validity of MCC and band combination for Cell Selection and
         * Reselection in 850/800 bands.
         */

        if((sib_events_ptr->event[index].event_name == RRCSIB_GET_SPECIFIC_SIBS) &&
           (sib_events_ptr->event[index].req_proc == RRC_PROCEDURE_CSP))
        {
          /* W Cell Selection or GtoW Cell Reselection */
          if(sib_events_ptr->event[index].for_serving_cell == TRUE)
          {         
            if(is_freq_in_overlapping_850_and_800_bands(sib_events_ptr->srv_bcch.freq)
               && (FALSE == rrccsp_is_sib_mcc_and_850_800_current_band_comb_valid(plmn)))
            {
              /* Save the barred freq to not skip scanning of the same in other overlapping band*/
              rrc_csp_save_barred_freq();
              WRRC_MSG0_HIGH("MCC_850_800: Aborting SIB reading due to PLMN band mismatch");
              rrcsib_post_process_failed_event((sib_events_index_e_type)index,
                                               TRUE,
                                               SIB_EVENT_FAILED_OTHER, 
                                               TRUE);
              return FALSE;
            }
          }
          /* WtoW Cell Reselection */
          else 
          {
            if(is_freq_in_overlapping_850_and_800_bands(sib_events_ptr->ngh_bcch.freq)
               && (FALSE == rrccsp_is_mcc_and_850_800_supported_band_comb_valid(
                            sib_events_ptr->ngh_bcch.freq, 
                            plmn, 
                            W_UARFCN_NOT_FROM_GSM_NBR_LIST
                            , FALSE
                            )
                  )
              )
            {
              WRRC_MSG0_ERROR("ERR INCORRECT NW CONFIG:Aborting Ngr SIB reading due to PLMN band mismatch");
              rrcsib_post_process_failed_event((sib_events_index_e_type)index,
                                               TRUE,
                                               SIB_EVENT_FAILED_OTHER, 
                                               FALSE);
              return FALSE;
            }
          }
        }
#endif
     
        if((sib_events_ptr->event[index].event_name == RRCSIB_GET_SPECIFIC_SIBS) &&
           (sib_events_ptr->event[index].req_proc == RRC_PROCEDURE_CSP) &&
           (sib_events_ptr->event[index].for_serving_cell == TRUE)
          )
        {
            /*In Network sharing cell there may be more than one PLMN broadcasted
               in a cell, so pass MIB pointer.this csp function will check if its N/W 
               shared cell then it will extract all the PLMN's for its processing*/
            if(rrccsp_skip_sibs_for_fplmn(plmn
                                          ,sib_events_ptr->event[index].tempholder.mib
              ) == TRUE)
            {
              WRRC_MSG0_HIGH("FPLMN: Cell belongs to Forbidden PLMN. Skip reading SIBs");
              rrcsib_post_process_failed_event((sib_events_index_e_type)index,
                                               TRUE,
                                               SIB_EVENT_FAILED_FPLMN, 
                                               FALSE);
              return FALSE;
            }
            if(rrccsp_skip_sibs_for_vplmn(plmn, sib_events_ptr->event[index].tempholder.mib) == TRUE)
            {
              WRRC_MSG0_HIGH("Found VPLMN already added to the available PLMN list. Skip reading SIBs");
              rrcsib_post_process_failed_event((sib_events_index_e_type)index,
                                               TRUE,
                                               SIB_EVENT_FAILED_OTHER, 
                                               FALSE);
              return FALSE;
            }

           /* Skipping to read the SIBs if MIB PLMN is not matching with the NAS requested PLMN 
              for XTOW redirection cases */
           if((rrc_csp_int_data.curr_scan  == RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN) && 
              (FALSE  == rrccsp_check_plmn_match(plmn,sib_events_ptr->event[index].tempholder.mib, TRUE)))
           {
             WRRC_MSG0_HIGH("XTOW : Cell doesn't belong to the NAS requested PLMN. Skip reading SIBs");
             rrcsib_post_process_failed_event((sib_events_index_e_type)index,
                                               TRUE,
                                               SIB_EVENT_FAILED_PLMN_MISMATCH, 
                                               FALSE);
             /* Reset ACQ retry counter so that no more ACQ attempts are made on the redirected freq and we can move ahead
                with list scan on the neighour frequencies.*/
             rrccsp_reset_num_tries_dest_freq();
             return FALSE;
           }
            if((rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_CELL_RESELECTION_SCAN)
             && (FALSE  == rrccsp_check_plmn_match(plmn,sib_events_ptr->event[index].tempholder.mib, TRUE)))
            {
              WRRC_MSG0_HIGH("There is PLMN mismatch...bailing out");
              rrcsib_post_process_failed_event((sib_events_index_e_type)index,
                            TRUE,
                            SIB_EVENT_FAILED_PLMN_MISMATCH, 
                            FALSE);
              return FALSE;
            }
          }

        /* we need to read the MIB.Decode and save MIB here */
        if((sib_events_ptr->event[index].sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)||
           (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_MANUAL_SCAN) ||
           (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN) ||
           (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_WTOW_AUTO_BPLMN_SCAN) ||
           (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_AUTO_BPLMN_SCAN)
           )
        {
            WRRC_MSG1_HIGH("Storing MIB for Manual scan in BPLMN SIBs db,Scan type %d",
                     sib_events_ptr->event[index].sib_scan_type);

            if(rrcsib_bplmn_sibs.mib == NULL)
            {
              uint32 mib_pdu_length;

              /* Get the PDU length in bytes */
              mib_pdu_length = GET_PDU_LENGTH_IN_BYTES
                (sib_events_ptr->event[index].tsib[rrc_MIB].sib_tot_length);
  
              /* Decode and store MIB in bplmn sib database */
              rrcsib_bplmn_sibs.mib = (rrc_MasterInformationBlock *)rrcasn1_decode_pdu( 
                (void *)(sib_events_ptr->event[index].
                         tsib[rrc_MIB].sib_data_ptr), /* Encoded MIB */
                rrc_MasterInformationBlock_PDU,       /* MIB's PDU number        */
                mib_pdu_length                        /* MIB's Length in bytes   */
                );

              if(rrcsib_bplmn_sibs.mib != NULL)
              {
                  if((sib_events_ptr->event[index].sib_scan_type != RRC_SIB_WTOW_AUTO_BPLMN_SCAN)
                   &&(sib_events_ptr->event[index].sib_scan_type != RRC_SIB_AUTO_BPLMN_SCAN)  
#ifdef FEATURE_LTE_TO_WCDMA
                   &&(!rrccsp_check_cgi_scan())
#endif             
                    )
                 {
#ifdef FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
                WRRC_MSG2_HIGH("SB1 needed = %d SB2 needed = %d",sib_events_ptr->event[index].tsib[rrc_SB1].sib_needed,
                      sib_events_ptr->event[index].tsib[rrc_SB2].sib_needed);
             
                sibSb_ptr = &rrcsib_bplmn_sibs.mib->sibSb_ReferenceList;
                for(ix=0;ix< sibSb_ptr->n;ix++)
                {
                  rrcsib_get_sib_type_from_sibsb_type_and_tag(&sibSb_ptr->elem[ix].sibSb_Type,
                                                              &sib_type_tag);
                  if(sib_type_tag.sib_type == rrc_SIB5 ||
                      sib_type_tag.sib_type == rrc_SIB5bis)
                  {
                    break;
                  }
                }
                    if(((sibSb_ptr->n > ix ))&&
                  ((sib_type_tag.sib_type == rrc_SIB5) ||
                    (sib_type_tag.sib_type == rrc_SIB5bis)))
                {
                  WRRC_MSG0_HIGH("BTD:-SIB5/SIB5bis found in MIB");
                  sib_events_ptr->event[index].tsib[rrc_SB1].sib_needed = FALSE;
                  sib_events_ptr->event[index].tsib[rrc_SB2].sib_needed = FALSE;
                }
#endif
              }
                }
              else
              {
                WRRC_MSG0_ERROR("BPLMN MIB decode failure");
              }
            }
            
            else
            {
              WRRC_MSG0_HIGH("WTOW BPLMN: BPLMN db already has a MIB");
            }


        }

        //check if scheduling info is received for all sibs
        if(FAILURE == rrcsib_check_if_scheduling_info_received_for_all_sibs(
          (void *)(sib_events_ptr->event[index].tempholder.mib),
          rrc_MIB,
          index))
        {
          rrcsib_process_sib_scheduling_info_incomplete(index); 
          return TRUE;
        }

        sib_events_ptr->event[index].tempholder.mib_decoded = TRUE;
        if( sib_events_ptr->event[index].event_name != RRCSIB_UPDATE_SIBS )
        {
          rrcsib_update_sched_info_from_mib_sb((uint32)rrc_MIB,
                                             sib_events_ptr->event[index].tempholder.mib, index);
        }

        if(((sib_events_ptr->event[index].sib_scan_type == RRC_SIB_WTOW_AUTO_BPLMN_SCAN )|| 
            (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_AUTO_BPLMN_SCAN)) &&
           (sib_events_ptr->event[index].event_name == RRCSIB_GET_SPECIFIC_SIBS))
        {
            uint32 count=0;  //counter purposes for 2 events
  
            /*In Network sharing cell there may be more than one PLMN broadcasted
               in a cell, so pass MIB pointer.this csp function will check if its N/W 
               shared cell then it will extract all the PLMN's for its processing*/
            if(FALSE == rrccsp_is_in_bplmn_list(plmn
                                              ,sib_events_ptr->event[index].tempholder.mib
              ))
            {
              WRRC_MSG0_HIGH("PLMN NOT in BPLMN list");
              
              /* First clear the SIB events database */
              for( count = 0; count < (uint32)MAX_SIB_EVENTS_INDEX; count++ )
              {
                if (sib_events_ptr->event[count].event_active == TRUE)
                {
                  rrcsib_post_process_failed_event((sib_events_index_e_type)count,TRUE,SIB_EVENT_FAILED_OTHER, FALSE);
                }
              }//loop thru sib events
            }//not in bplmn list
          }
        }

      /* Here we do some special processing if the event is
      RRCSIB_UPDATE_SIBS. This event is used to update the
      SIBs on the cell we've camped on. We already know
      the sib database index for this cell, so we can
      do value-tag comparisons without waiting for SIB3.
      First check if the value-tag of the MIB is the one 
      we're expecting */
      if( ( sib_events_ptr->event[index].tempholder.mib_decoded == TRUE ) &&
          ( sib_events_ptr->event[index].event_name == RRCSIB_UPDATE_SIBS )
        )
      {
        /* Check if this MIB's value-tag matches the value-tag
        we received with the command */
        /* If the MIB value Tag doesnot match with the expected value Tag and
           if the received value tag is different from the current value Tag of MIB in SIB DB,
           receive the MIB*/
        /* Here need to by pass the check for 6hr UPDATE_SIBs, */
        if((sib_events_ptr->event[index].tempholder.mib->mib_ValueTag ==
            sib_events_ptr->mib_value_tag_from_command )||
           ((sib_events_ptr->event[index].tempholder.mib->mib_ValueTag !=
             sib_events_ptr->mib_value_tag_from_command) &&
            (sib_events_ptr->event[index].tempholder.mib->mib_ValueTag !=
             sibdb.cell[sibdb.active_cell_index].sib_value_tag[rrc_MIB]))||
           (sib_events_ptr->update_sibs_cause == RRCSIB_CSFB_CALL_CANCL)
           ||(rrcsib_is_event_sixhrs_sib_update(index) == TRUE)
          )
        {
          WRRC_MSG0_HIGH("New MIB rxed for UPDATE_SIBS event");
          rrcsib_update_sched_info_from_mib_sb((uint32)rrc_MIB,
                                             sib_events_ptr->event[index].tempholder.mib, index);
          
          /* We got the MIB that we needed. We can do value-tag
          comparisons. Before that, make sure we have the 
          database index for this event. For RRCSIB_UPDATE_SIBS event,
          the database index is the current active cell's index.*/
          sib_events_ptr->event[index].sib_db_index = sibdb.active_cell_index;
          sib_events_ptr->event[index].sib_db_index_found = TRUE;

          /* Now do the value-tag comparisons */
          update_event_for_received_value_tags( 
            rrc_MIB, /* For the MIB */
            index,                      /* Event Index */
            sib_events_ptr->event[index].sib_db_index);

          /* Also free the MIB since we no longer need it */
          rrc_sibdb_free_sib( rrc_MasterInformationBlock_PDU,
            (void *)sib_events_ptr->event[index].tempholder.mib );
          sib_events_ptr->event[index].tempholder.mib = NULL;

          if(update_vtags_of_sibs_in_sb1_sb2_if_received(index))
          {
            return TRUE;
          }
          /* Now that we have the updated MIB, we can decode and store 
          that MIB and whatever SIBs we've received in this pass. */
          MSG_MED("Decoding received SIBs", 0, 0, 0);
          decode_completed_sibs(index);

        }
        else
        {
#ifdef FEATURE_RRC_SIB_DEBUG
          WRRC_MSG0_HIGH("MIB value-tag not changed yet: ignore");
#else
          MSG_LOW("MIB value-tag not changed yet: ignore", 0, 0, 0);
#endif /* FEATURE_RRC_SIB_DEBUG */
          /* Remove this MIB from tempholder - we won't use this MIB. First
          call the function that frees the mib, then indicate that the mib
          has not been received yet. */
          rrc_sibdb_free_sib(  rrc_MasterInformationBlock_PDU,
           (void *)sib_events_ptr->event[index].tempholder.mib );
          sib_events_ptr->event[index].tempholder.mib = NULL;

          sib_events_ptr->event[index].tempholder.mib_decoded = FALSE;
          sib_events_ptr->event[index].tsib[rrc_MIB].sib_received = FALSE;
        }
      }
      if((sib_events_ptr->event[index].tempholder.mib_decoded) &&
         ((curr_scan_type == RRC_CSP_LEAVING_CONN_MODE_SCAN) ||
          (curr_scan_type == RRC_CSP_CELL_DCH_TO_CELL_FACH_SCAN) ||
          (curr_scan_type == RRC_CSP_CELL_DCH_TO_CELL_PCH_SCAN) ||
          (curr_scan_type == RRC_CSP_CELL_DCH_TO_URA_PCH_SCAN) ||
          (curr_scan_type == RRC_CSP_CELL_FACH_TO_CELL_FACH_SCAN) ||
          (curr_scan_type == RRC_CSP_CELL_FACH_TO_CELL_PCH_SCAN) ||
          (curr_scan_type == RRC_CSP_CELL_FACH_TO_URA_PCH_SCAN)))
      {
        /* Check if this cell already exists. Do the search based on plmn_id, frequency, 
        and scrambling code */
        WRRC_MSG2_HIGH("Checking SIBDB index for Freq %d, PSC %d",
                 sib_events_ptr->srv_bcch.freq,sib_events_ptr->srv_bcch.scr_code);
  
        db_index = rrc_sibdb_find_cell_index_for_plmn_freq_psc(plmn,sib_events_ptr->srv_bcch.freq,
                                                      sib_events_ptr->srv_bcch.scr_code,index);

        if( db_index != SIBDB_INVALID_INDEX )
        {
#ifdef FEATURE_FEMTO_CSG

          /* Getting SIB3 to check if CSG ID is present */
          /* If CSG ID is present then don't use freq psc optimization */
          /* Not using invalid SIB index check here as it is already present inside the
             below function */
          sib3 =  rrc_sibdb_return_sib_for_db_index(rrc_SIB3,db_index);

          /* Apply freq and PSC optimization only for macro cells */ 
          if((!sib_events_ptr->event[index].is_csg) && ((NULL != sib3) && (!CSG_ID_IN_SIB3(sib3))))
#endif
          {
            if(sibdb.cell[db_index].ten_min_count < MAX_CELL_INDEX_COUNT_TENMIN)
            {
              mib_ptr =  rrc_sibdb_return_sib_for_db_index(rrc_MIB,db_index);

              if((mib_ptr != NULL) &&
                 (memcmp((void*) mib_ptr, (void*) sib_events_ptr->event[index].tempholder.mib, sizeof(rrc_MasterInformationBlock))))
              {
           
#ifdef FEATURE_FEMTO_CSG

                rrc_csp_int_data.curr_acq_entry.cell_is_hybrid = FALSE;

                sib_events_ptr->event[index].sib_db_index = 
                rrc_sibdb_create_cell_index( 
                   sibdb.cell[db_index].plmn_id, 
                   sibdb.cell[db_index].cell_id, 
                                 index,SYS_CSG_ID_INVALID,FALSE);
#else
                sib_events_ptr->event[index].sib_db_index = 
                rrc_sibdb_create_cell_index( 
                 sibdb.cell[db_index].plmn_id, 
                 sibdb.cell[db_index].cell_id, 
                                index);
#endif
                sib_events_ptr->event[index].sib_db_index_found = TRUE;
                WRRC_MSG2_HIGH("SIBDB index for Freq and PSC found! DB-Index %d,  Cell-id %d",
                         sib_events_ptr->event[index].sib_db_index, 
                         sibdb.cell[(sib_events_ptr->event[index].sib_db_index)].cell_id);
                sib_events_ptr->event[index].sib_db_index = db_index;
                update_event_for_received_value_tags( 
                    rrc_MIB, /* For the MIB */
                    index,                      /* Event Index */
                    db_index);
                if(update_vtags_of_sibs_in_sb1_sb2_if_received(index))
                {
                  return TRUE;
                }
                decode_completed_sibs(index);
              }
              else
              {
                WRRC_MSG0_HIGH("MIBs dont match completely");
              }
            }
            else
            {
              WRRC_MSG0_HIGH("Cant consider the DB index just by MIB for this cell as ten minute timer has expired.");
            }
          }
#ifdef FEATURE_FEMTO_CSG
          else
          {
            WRRC_MSG0_HIGH("CSG: Not applying freq psc sibdb opt for CSG cell");
          }
#endif
        }
      }

#ifdef FEATURE_RRCSIB_USE_FREQ_PSC_FOR_INDEX
      /* Generally we use PLMN Id and Cell Id for indexing into the database.
      But to speed up reselections, use Frequency and Primary Scrambling Code
      to find the SIB DB index if FEATURE_RRCSIB_USE_FREQ_PSC_FOR_INDEX is
      defined.This should be used only for cell reselections. */
      
      if((sib_events_ptr->event[index].tempholder.mib_decoded) && 
         (sib_events_ptr->event[index].for_serving_cell == FALSE))   
      {

          /* Check if this cell already exists. Do the search based on plmn_id, frequency, 
          and scrambling code */
          WRRC_MSG2_HIGH("Checking SIBDB index for Freq %d, PSC %d",
                   sib_events_ptr->ngh_bcch.freq,sib_events_ptr->ngh_bcch.scr_code );
  
        db_index = rrc_sibdb_find_cell_index_for_plmn_freq_psc(plmn,sib_events_ptr->ngh_bcch.freq,
                                                        sib_events_ptr->ngh_bcch.scr_code,index);
  
#ifdef FEATURE_FEMTO_CSG

       /* Getting SIB3 to check if CSG ID is present */
       /* If CSG ID is present then don't use freq psc optimization */
       /* Not using invalid SIB index check here as it is already present inside the
          below function */
       sib3 =  rrc_sibdb_return_sib_for_db_index(rrc_SIB3,db_index);

       if((NULL != sib3) && (!CSG_ID_IN_SIB3(sib3)))
       {
         WRRC_MSG1_HIGH("CSG : CSG ID is not present in SIB3 for dbindex %d",
                   sib_events_ptr->event[index].sib_db_index);
       }

       /* Apply freq and PSC optimization only for macro cells */ 
       if((!sib_events_ptr->event[index].is_csg) && ((NULL != sib3) && (!CSG_ID_IN_SIB3(sib3))))
       {
#endif
         sib_events_ptr->event[index].sib_db_index = db_index;
#ifdef FEATURE_FEMTO_CSG

         rrc_csp_int_data.curr_acq_entry.cell_is_hybrid = FALSE;
#endif
          if( sib_events_ptr->event[index].sib_db_index != SIBDB_INVALID_INDEX )
          {
            /* Although we already know the index based on freq and psc
            call the rrc_sibdb_create_cell_index which will do the necessary
            sibdb processing for using this index for this event. */
#ifdef FEATURE_FEMTO_CSG
              sib_events_ptr->event[index].sib_db_index = 
              rrc_sibdb_create_cell_index( 
                 sibdb.cell[(sib_events_ptr->event[index].sib_db_index)].plmn_id, 
                 sibdb.cell[(sib_events_ptr->event[index].sib_db_index)].cell_id, 
                             index,SYS_CSG_ID_INVALID,FALSE);
#else
            sib_events_ptr->event[index].sib_db_index = 
              rrc_sibdb_create_cell_index( 
               sibdb.cell[(sib_events_ptr->event[index].sib_db_index)].plmn_id, 
               sibdb.cell[(sib_events_ptr->event[index].sib_db_index)].cell_id, 
                                index);
#endif
            sib_events_ptr->event[index].sib_db_index_found = TRUE;
  
            WRRC_MSG2_HIGH("SIBDB index for Freq and PSC found! DB-Index %d,  Cell-id %d",
                     sib_events_ptr->event[index].sib_db_index, 
                     sibdb.cell[(sib_events_ptr->event[index].sib_db_index)].cell_id);

            /* Since we've found the database index and we've gotten the
            MIB, we can check the MIB value tags. */
            update_event_for_received_value_tags( 
              rrc_MIB, /* For the MIB */
              index,                      /* Event Index */
              sib_events_ptr->event[index].sib_db_index);
          
            if(update_vtags_of_sibs_in_sb1_sb2_if_received(index))
            {
              return TRUE;
            }
            /* We no longer need the temporarily decoded MIB. Free
            the PDU using the rrc_sibdb_free_sib function */
            rrc_sibdb_free_sib( rrc_MasterInformationBlock_PDU,
              (void *)sib_events_ptr->event[index].tempholder.mib );
            sib_events_ptr->event[index].tempholder.mib = NULL;
    
            /* Now that we have an index, we can decode and store 
            whatever SIBs we've received in this pass. */
            MSG_MED("Decoding received SIBs", 0, 0, 0);
            decode_completed_sibs(index);

            /*if SIB1 is valid, check if there is any LA/RA mismatch*/
            if((sib_events_ptr->event[index].tsib[rrc_SIB1].sib_received)&&
               (sib_events_ptr->event[index].tsib[rrc_SIB1].sib_decoded))
            {
              sib1_ptr = rrc_sibdb_return_sib_for_db_index(rrc_SIB1,
                                                           sib_events_ptr->event[index].sib_db_index);
              if(rrcrce_abort_resel(plmn,sib1_ptr,sib_events_ptr->ngh_bcch.freq,sib_events_ptr->ngh_bcch.scr_code))
              {
                 rrcsib_post_process_failed_event(index,
                                                  TRUE,
                                                  SIB_EVENT_FAILED_OTHER, 
                                                  FALSE);
                 return FALSE;
              }
            } 
          }
          else
          {
            WRRC_MSG0_HIGH("SIBDB index for Freq and PSC not found");
          }
#ifdef FEATURE_FEMTO_CSG
          }
          else
          {
            WRRC_MSG0_HIGH("CSG: Not applying freq psc sibdb opt for CSG cell");
          }
#endif
        }

#endif /* FEATURE_RRCSIB_USE_FREQ_PSC_FOR_INDEX */

#ifdef FEATURE_DUAL_SIM
      /*If MIB received this time and SIBDB index is not found, or SIBDB index found and event is not completed
        Send NULL mask */
      if((sib_events_ptr->event[index].tempholder.mib_decoded == TRUE) &&
          ((sib_events_ptr->event[index].sib_db_index_found != TRUE) ||
           (!check_event_completion_status(index,rrcsib_can_nw_support_deferred_sib(sib_events_ptr->event[index].sib_db_index))))
        )
      {
        /*This will send empty table as scheduling is not yet updated*/
        if((g_in_data_call)&&
           (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_NORMAL)&&
           (sib_events_ptr->event[index].int_event.int_event_type != RRCSIB_INT_SIB7_EVENT)
          )
        {
          rrcsib_send_empty_table(index,TRUE);
          rrctmr_start_timer(RRCTMR_SIB_EMPTY_MASK_TIMER, RRCSIB_EMPTY_FRAMES);
        }
      }
#endif
    }/* end if - MIB was received this pass */

    if((sib_events_ptr->event[index].tsib[rrc_SB1].sib_received)
      && (sib_events_ptr->event[index].tempholder.mib_decoded)
      &&(sib_events_ptr->event[index].tempholder.sb1_decoded == FALSE))
    {
      if(FAILURE == rrcsib_process_scheduling_block_sched_info_before_sib_db_decoded
         (index, rrc_SB1))
      {
        rrcsib_process_sib_scheduling_info_incomplete(index); 
        return TRUE;
      }
    }
  
    if((sib_events_ptr->event[index].tsib[rrc_SB2].sib_received)
      && (sib_events_ptr->event[index].tempholder.mib_decoded)
      &&(sib_events_ptr->event[index].tempholder.sb2_decoded == FALSE))
    {
      if(FAILURE == rrcsib_process_scheduling_block_sched_info_before_sib_db_decoded
         (index, rrc_SB2))
      {
        rrcsib_process_sib_scheduling_info_incomplete(index); 
        return TRUE;
      }
    }

    /* Then check if the SIB3 was received in this pass. 
    We do this only if the SIBDB index wasn't already found in the
    if statement above. This may be the case for UPDATE_SIB events
    or if FEATURE_RRCSIB_USE_FREQ_PSC_FOR_INDEX is defined. */
    if( (sib_events_ptr->event[index].sib_db_index_found != TRUE)
      && (sib_events_ptr->event[index].tsib[rrc_SIB3].sib_received)
      && (!(sib_events_ptr->event[index].tempholder.sib3_decoded)) )
    {
      /* Decode SIB3 to the local copy - For this, first we need the PDU
      length in bytes. Divide the PDU lenth in bits with 8 and add 1 since
      we can err on the high side for the length. */
      pdu_length = GET_PDU_LENGTH_IN_BYTES
                    (sib_events_ptr->event[index].tsib[rrc_SIB3].
                     sib_tot_length);
      (void)rrcsib_check_if_connected_mode_sibs_present
      (sib_events_ptr->event[index].tsib[rrc_SIB3].sib_data_ptr[0],
      rrc_SIB3);

      /* Call rrcasn1_decode_pdu to do ASN1 decoding. Since it returns a 
      void pointer, typecast it to the MIB's type. */
      sib_events_ptr->event[index].tempholder.sib3 =
        (rrc_SysInfoType3 *)rrcasn1_decode_pdu( 
          (void *)(sib_events_ptr->event[index].
                 tsib[rrc_SIB3].sib_data_ptr), /* Encoded SIB3 */
          rrc_SysInfoType3_PDU,            /* SIB3's PDU number         */
          pdu_length                       /* SIB3's length in bytes    */
                        );
      
      if( sib_events_ptr->event[index].tempholder.sib3 == NULL )
      {
        /* Decode did not work */
        WRRC_MSG0_HIGH("SIB3 decode failed");

        /*Reset all the variables for this SIB so that we can try to
        read it again */
        sib_events_ptr->event[index].tsib[rrc_SIB3].sib_received = FALSE;
        sib_events_ptr->event[index].tsib[rrc_SIB3].sib_decoded = FALSE;
        sib_events_ptr->event[index].tsib[rrc_SIB3].seg_count = 0;
        sib_events_ptr->event[index].tsib[rrc_SIB3].seg_index = 
          (MAX_SEG_INDEX_FOR_A_SIB + 1);
        sib_events_ptr->event[index].tsib[rrc_SIB3].sib_tot_length = 0;
      }
      else
      {
        sib_events_ptr->event[index].tempholder.sib3_decoded = TRUE;
#ifdef FEATURE_FEMTO_CSG
          if((CSG_ID_IN_SIB3(sib_events_ptr->event[index].tempholder.sib3))&&
              (is_whitelist_valid))
          {
            sib_events_ptr->event[index].csg_id = rrc_translate_cell_id
                            ( &(sib_events_ptr->event[index].tempholder.sib3->v4b0NonCriticalExtensions.v590NonCriticalExtension.
                                v5c0NoncriticalExtension.v670NonCriticalExtension.v770NonCriticalExtension.
                                v830NonCriticalExtension.v860NonCriticalExtension.sysInfoType3_v860ext.csgIdentity));
            WRRC_MSG1_HIGH("CSG: CSG Id is %d",sib_events_ptr->event[index].csg_id);
          }

        rrc_csp_int_data.curr_acq_entry.cell_is_hybrid = FALSE;

        /* This is a hybrid cell if CSG indicator is set to FALSE whereas a CSG ID is broadcasted */
        if((sib_events_ptr->event[index].csg_id != SYS_CSG_ID_INVALID) && (!sib_events_ptr->event[index].is_csg))
        {
          rrc_csp_int_data.curr_acq_entry.cell_is_hybrid = TRUE;
          sib_events_ptr->event[index].is_csg = TRUE;  
        }
        WRRC_MSG1_HIGH("CSG:  Cell_is_hybrid %d",rrc_csp_int_data.curr_acq_entry.cell_is_hybrid);		  		

        if((sib_events_ptr->event[index].is_csg)  && (sib_events_ptr->event[index].csg_id == SYS_CSG_ID_INVALID))
          {
             WRRC_MSG0_HIGH("CSG: CSG indicator present but no CSG ID is broadcasted. Bar the cell");
             rrccsp_send_cphy_cell_bar_req(RRC_CSP_WCDMA_PSC_BARRED,1280);
           
             rrcsib_post_process_failed_event((sib_events_index_e_type)index,
                                               TRUE,
                                               SIB_EVENT_FAILED_OTHER, 
                                               TRUE);
             return FALSE;
          }
        else if((sib_events_ptr->event[index].csg_id == SYS_CSG_ID_INVALID) && 
                (sib_events_ptr->event[index].for_csg))
        {
          /* For ASF scan consider macro cells too if active RAT is W*/
          if((rrccsp_asf_actively_in_prog())&&(rrc_ccm_get_curr_camped_freq() == rrccsp_get_serving_frequency())&&
              sib_events_ptr->event[index].sib_scan_type == RRC_SIB_WTOW_AUTO_BPLMN_SCAN)
          {
            WRRC_MSG0_HIGH("CSG: Macro cell found during ASF scan on camped frequency. consider it");
          }
          else
          {
            rrcsib_post_process_failed_event((sib_events_index_e_type)index,
                                              TRUE,
                                              SIB_EVENT_FAILED_OTHER, 
                                              TRUE);
            return FALSE;
         }
        }
#endif
        if(sib_events_ptr->event[index].tempholder.mib == NULL)
        {
          temp_mib = rrcsib_bplmn_sibs.mib;
        }
        else
        {
          temp_mib = sib_events_ptr->event[index].tempholder.mib;
        }
        rrc_translate_gsm_map_plmn_id(&(temp_mib->plmn_Type.u.gsm_MAP->plmn_Identity),&plmn);


        /*Do not perform suitablity check for Cell ID in DCH scans*/
       if((sib_events_ptr->event[index].sib_scan_type != RRC_SIB_CELL_ID_READ_IN_DCH_SCAN)
#ifdef FEATURE_LTE_TO_WCDMA
          &&(!rrccsp_check_cgi_scan())
#endif
         )
        {
          /* Below function performs, White list check, cell bar status. If it finds a cell as barred sends cell bar req to l1 */
           rrccsp_initial_suitability_check(temp_mib,
                                           sib_events_ptr->event[index].tempholder.sib3,
                                           plmn,
#ifdef FEATURE_FEMTO_CSG
                                           sib_events_ptr->event[index].csg_id,
#endif
                                           &status);
        }
        if(status != SIB_EVENT_SUCCEEDED)
        {
          rrcsib_post_process_failed_event((sib_events_index_e_type)index,
                                            TRUE,
                                            status, 
                                            TRUE);
          return FALSE;
        }
        else if((!sib_events_ptr->event[index].for_serving_cell)&&(FALSE/*s criteria check here*/))
        {
          WRRC_MSG0_HIGH("CSG: Suitability criteria check failed");
          rrcsib_post_process_failed_event((sib_events_index_e_type)index,
                                               TRUE,
                                               SIB_EVENT_FAILED_OTHER, 
                                               TRUE);
          return FALSE;

        }


      /* Checking S-Criteria */
        if((rrc_csp_int_data.curr_scan == RRC_CSP_GSM_TO_WCDMA_CELL_RESELECTION_SCAN) 
#ifdef FEATURE_LTE_TO_WCDMA
          && (!rrccsp_check_cgi_scan())
#endif
          )
        {
          if(FALSE == rrc_check_serv_cell_s_crit(sib_events_ptr->event[index].tempholder.sib3))
          {
            WRRC_MSG0_HIGH("S criteria for X2W reselection is not met...bailing out");
            rrcsib_post_process_failed_event((sib_events_index_e_type)index,
                          TRUE,
                          SIB_EVENT_FAILED_SCRITERIA, 
                          TRUE);
            return FALSE;
          }
        }
      }
    }/* end if -  SIB3 was received this pass */

    /* Now if we have both MIB and SIB3 decoded, we can get the
    PLMN and Cell Id and find an index into the sib datbase for this cell
    We do this only if the SIBDB index wasn't already found in the
    if statement above. This may be the case for UPDATE_SIB events
    or if FEATURE_RRCSIB_USE_FREQ_PSC_FOR_INDEX is defined. */
    
    if( (sib_events_ptr->event[index].sib_db_index_found != TRUE)
      &&(sib_events_ptr->event[index].tempholder.mib_decoded)
      &&(sib_events_ptr->event[index].tempholder.sib3_decoded) )
    {
      if(sib_events_ptr->event[index].tempholder.mib != NULL)
      {
        /* Check to see if PLMN Type is not correct here. */
        if(!((RRC_CHECK_COMMON_MSG_TYPE(sib_events_ptr->event[index].tempholder.mib->plmn_Type, 
              rrc_PLMN_Type_gsm_MAP))||
             ((RRC_CHECK_COMMON_MSG_TYPE(sib_events_ptr->event[index].tempholder.mib->plmn_Type, 
              rrc_PLMN_Type_gsm_MAP_and_ANSI_41))))
          )
        {
          WRRC_MSG0_HIGH("Acquired cell PLMN not GSM-MAP!");
          /* No need process this event further, just send a failure
            to the procedure that requested this sib event. */
          non_gsm_map_plmn = TRUE;
          rrcsib_post_process_failed_event( index, /* Event Index                 */
                                                TRUE,  /* Confirm Command is required */
                                                SIB_EVENT_FAILED_INVALID_SIBS, /* Failure cause */
                                                TRUE   /* Tear down S_BCCH if it's up */
                                              );
        }
      }

      if( non_gsm_map_plmn != TRUE )
      { 
        if((sib_events_ptr->event[index].sib_scan_type == RRC_SIB_WTOW_AUTO_BPLMN_SCAN ) ||
           (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)||
           (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_MANUAL_SCAN) ||
           (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN||
           (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_AUTO_BPLMN_SCAN))
          )
        {
          WRRC_MSG0_HIGH("WTOW BPLMN: MIB and SIB3 received and decoded");
          
          if(rrcsib_bplmn_sibs.sib3 == NULL)
          {
            uint32 sib3_pdu_length;
            
            /* Get the PDU length in bytes */
            sib3_pdu_length = GET_PDU_LENGTH_IN_BYTES
                              (sib_events_ptr->event[index].tsib[rrc_SIB3].
                               sib_tot_length);

            /* Decode and store SIB1 in bplmn sib database */
            rrcsib_bplmn_sibs.sib3 = 
            (rrc_SysInfoType3 *)rrcasn1_decode_pdu( 
                               (void *)(sib_events_ptr->event[index].
                               tsib[rrc_SIB3].sib_data_ptr), /* Encoded SIB3 */
                               rrc_SysInfoType3_PDU, /* SIB3's PDU number         */
                               sib3_pdu_length       /* SIB3's length in bytes    */
                               );

            if(rrcsib_bplmn_sibs.sib3 == NULL)
            {
              WRRC_MSG0_ERROR("SIB3 decode failure");
            }
          }
           
          /* We no longer need the temporarily decoded MIB and SIB3. Free
             the PDUs using the rrc_sibdb_free_sib function */
          rrcsib_free_tsib(index);
        }
        else
        {
          if(sib_events_ptr->event[index].sib_scan_type == RRC_SIB_CELL_ID_READ_IN_DCH_SCAN)
          {
            /*just for printing........Dont do any processing here,
             *as sib_scan_type RRC_SIB_CELL_ID_READ_IN_DCH_SCAN 
             *we dont store any SIB's, return cell I.D from tempholder.sib3
             */
            WRRC_MSG0_HIGH("RRCGPS:MIB and SIB3 received and decoded");
          }
          else
          {
            if(sib_events_ptr->event[index].tempholder.mib!= NULL)
            {
              /* Since the PLMN is good, get the cell id for this cell. */
              cell = rrc_translate_cell_id
                      ( &(sib_events_ptr->event[index].tempholder.sib3->cellIdentity));
              MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
                       "Temp SIB3 MSG-Ptr is 0x%x Checking SIBDB index for CellId %d and MCC %d MNC %d", 
                       sib_events_ptr->event[index].tempholder.sib3,cell, RRC_CSP_GET_INT_MCC(plmn), RRC_CSP_GET_INT_MNC(plmn));

              /* Get the SIB Database index for this cell */
#ifdef FEATURE_FEMTO_CSG
              sib_events_ptr->event[index].sib_db_index = rrc_sibdb_create_cell_index( plmn, cell,index,sib_events_ptr->event[index].csg_id,sib_events_ptr->event[index].is_csg);
#else
              sib_events_ptr->event[index].sib_db_index = rrc_sibdb_create_cell_index( plmn, cell,index);
#endif
              sib_events_ptr->event[index].sib_db_index_found = TRUE;
              sibdb.cell[sib_events_ptr->event[index].sib_db_index].ten_min_count = 0;
              if((rrc_ccm_get_serv_plmn_cellid(&cell_id,&plmn_id) == RRC_NEW_CELL_SELECTED)&&
                 (sib_events_ptr->event[index].for_serving_cell == FALSE)&&
                 (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_NORMAL)
                )
              {
                MSG_HIGH("Current Camped CELL I.D#%d, PLMN I.D MCC %d MNC %d",cell_id,
                                                                              RRC_CSP_GET_INT_MCC(plmn_id), 
                                                                              RRC_CSP_GET_INT_MNC(plmn_id));
                if((cell_id == cell)&&
                  (rrc_bcch_plmns_match(plmn_id,plmn) == TRUE)
                  )
                {
                  sibdb.cell[sib_events_ptr->event[index].sib_db_index].six_hr_count = 
                    MAX_CELL_INDEX_COUNT_SIXHR_CELL_SEL_RESEL;
                   WRRC_MSG1_ERROR("Target cell same as Current Camped Cell, Cell I.D %d", cell); 
                   rrcsib_post_process_failed_event((sib_events_index_e_type)index,TRUE,SIB_EVENT_FAILED_OTHER, FALSE);
                   rrcsib_initiate_cell_selection();
                   return FALSE;
                }
              }
              /* Since we've found the database index and we've gotten the
              MIB, we can check the MIB value tags. */
              update_event_for_received_value_tags( 
                rrc_MIB, /* For the MIB */
                index,                      /* Event Index */
                sib_events_ptr->event[index].sib_db_index);
              if(update_vtags_of_sibs_in_sb1_sb2_if_received(index))
              {
                return TRUE;
              }
              /* We no longer need the temporarily decoded MIB and SIB3. Free
              the PDUs using the rrc_sibdb_free_sib function */
              rrcsib_free_tsib(index);
              /* Now that we have an index, we can decode and store 
              whatever SIBs we've received in this pass. */
              MSG_MED("Decoding received SIBs", 0, 0, 0);
              decode_completed_sibs(index);
            }
            else
            {
              WRRC_MSG1_ERROR("Temp MIB ptr is NULL Value = %d",sib_events_ptr->event[index].tempholder.mib);
            }
          }
        }/* end if both mib and sib3 decoded */
      }/* end if non_gsm_map_plmn != TRUE */
    }/* end if both mib and sib3 decoded */
  }/* end if index not found */
  return TRUE; 
}/* end function process_completed_sibs */

/*===========================================================================

FUNCTION RRCSIB_IS_SIB_SCHEDULED

DESCRIPTION

  This function checks the scheduling information present
  in the current cell to determine if the given SIB is
  scheduled by the MIB or one of the scheduling blocks.
  
DEPENDENCIES
 
  None.

RETURN VALUE

  TRUE is the SIB is scheduled, FALSE if not scheduled.

SIDE EFFECTS

  None.
   
===========================================================================*/
boolean rrcsib_is_sib_scheduled
(
  rrc_SIB_type sib, /* SIB to be checked */
  uint32 dbindex    /* Index to the database */
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
  , boolean for_deferred_sib
  , rrc_SIB_type *def_sib_stored_in_sb1_sb2
#endif
)
{
  rrc_SIBSb_ReferenceList *sb_ptr; /* Local pointer to SIBSb ref list   */
  rrc_SIB_ReferenceList *ptr;      /* Local pointer to SIB ref list     */
  boolean sib_schd_found = FALSE; /* Indicate if sib schedule is found */
  boolean sb1_found = FALSE;      /* Indicate if sb1 is found incase
                                     the sib is not present in MIB     */
  boolean sb2_found = FALSE;      /* Indicate if sb2 is found incase
                                     the sib is not present in MIB     */
  uint32 sib_sb_tag = RRC_TOT_SIBS; /* To hold sib_sb value tag type.
                                     Initialize to invalid value       */
  uint32 sib_tag = RRC_TOT_SIBS;  /* To hold sib value tag type.
                                     Initialize to invalid value       */
  rrc_MasterInformationBlock *mib_ptr = NULL; /* Local pointer to MIB       */
  rrc_SysInfoTypeSB1 *sb1_ptr = NULL;         /* Local pointer to SB1       */
  rrc_SysInfoTypeSB2 *sb2_ptr = NULL;         /* Local pointer to SB2       */
  uint32 n_index=0, n_value=0;
  ptr = NULL;
  
  if( sib == rrc_MIB )
  {
    /* MIB is always present - no need to check anything */
    return( TRUE );
  }

  /* First determine the sib value tag type definition for this sib */
  switch( sib )
  {
    case rrc_SIB1 :
      sib_sb_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType1);
      break;

    case rrc_SIB2 :
      sib_sb_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType2);
      break;

    case rrc_SIB3 :
      sib_sb_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType3);
      break;

    case rrc_SIB4 :
      sib_sb_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType4);
      break;

    case rrc_SIB5 :
      sib_sb_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType5);
      break;

    case rrc_SIB6 :
      sib_sb_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType6);
      break;

    case rrc_SIB7 :
      sib_sb_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType7);
      break;

    case rrc_SIB11 :
      sib_sb_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType11);
      break;

    case rrc_SIB12 :
      sib_sb_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType12);
      break;

    case rrc_SIB18 :
      sib_sb_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIBSb_TypeAndTag_sysInfoType18);
      break;
  
    case rrc_SB1 :
      sib_sb_tag = RRCGET_SIB_TYPE_AND_TAG_VALUE_SYS_INFO_TYPE(sysInfoTypeSB1);
      break;
  
    case rrc_SB2 :
      sib_sb_tag = RRCGET_SIB_TYPE_AND_TAG_VALUE_SYS_INFO_TYPE(sysInfoTypeSB2);
      break;
  
    default:
      WRRC_MSG1_ERROR("Unknown SIB%d", sib);
      break;
  }/* end switch */

  if( sib_sb_tag == RRC_TOT_SIBS )
  {
    /* Unknown SIB - scheduling info not present */
    return( FALSE );
  }

  if( sibdb.cell[dbindex].sib_valid[rrc_MIB] )
  {
    /* If the MIB is valid */
    mib_ptr = rrc_sibdb_return_sib_for_db_index(rrc_MIB,dbindex);
    if(mib_ptr != NULL)
    {
      sb_ptr = &mib_ptr->sibSb_ReferenceList;

      n_index = 0;
      n_value = sb_ptr->n;
    
      while( n_value != 0 )
      {
        /* Check if the given sib is scheduled in the MIB */
        if(RRCGET_SIB_TYPE_AND_TAG((&sb_ptr->elem[n_index].sibSb_Type)) == sib_sb_tag)
        {
          /* We found it */
          sib_schd_found = TRUE;
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
          if(for_deferred_sib == TRUE)
          {
            *def_sib_stored_in_sb1_sb2 = rrc_MIB;
          }
#endif
          break; /* Out of the while loop */
        }

        /* Else check if this is for SB1 */
        if(RRCCHECK_SIBSb_TypeAndTag((&sb_ptr->elem[n_index].sibSb_Type),sysInfoTypeSB1))
        {
          /* We found it */
          sb1_found = TRUE;
        }

        /* Else check if this is for SB2 */
        if(RRCCHECK_SIBSb_TypeAndTag((&sb_ptr->elem[n_index].sibSb_Type),sysInfoTypeSB2))
        {
        /* We found it */
          sb2_found = TRUE;
        }
        n_index++;
        n_value--;
      }/* end while */

      if( sib_schd_found == TRUE )
      {
        /* This sib is scheduled in the MIB */
        return( TRUE );
      }
    }
    else
    {
      WRRC_MSG0_ERROR("MIB is NULL!");
      return( FALSE );
    }
  }/* end if MIB is valid */

  else
  {
    /* Can't find MIB! */
    WRRC_MSG0_ERROR("MIB not present in srv cell!");
    return( FALSE );
  }

  /* If the given sib was not scheduled in the MIB. Check if the given
  sib itself is a scheduling block */
  if( sib == rrc_SB1 )
  {
    if( sb1_found == TRUE )
    {
      /* Scheduling block 1 is scheduled in MIB */
      return(TRUE);
    }
    else
    {
      return(FALSE);
    }
  }
  /* Same for SB2 */
  if( sib == rrc_SB2 )
  {
    if( sb2_found == TRUE )
    {
      /* Scheduling block 2 is scheduled in MIB */
      return(TRUE);
    }
    else
    {
      return(FALSE);
    }
  }

  /* If we get here  the SIB is not scheduled in the MIB and
  it is not a scheduling block. So we need to find if it is
  scheduled in one of the scheduling blocks. First note the 
  tag type for this sib */
  switch( sib )
  {
    case rrc_SIB1 :
      sib_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType1);
      break;

    case rrc_SIB2 :
      sib_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType2);
      break;

    case rrc_SIB3 :
      sib_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType3);
      break;

    case rrc_SIB4 :
      sib_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType4);
      break;

    case rrc_SIB5 :
      sib_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType5);
      break;

    case rrc_SIB6 :
      sib_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType6);
      break;

    case rrc_SIB7 :
      sib_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType7);
      break;

    case rrc_SIB11 :
      sib_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType11);
      break;

    case rrc_SIB12 :
      sib_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType12);
      break;

    case rrc_SIB18 :
      sib_tag = RRCGET_COMMON_CHOICE_TYPE(rrc_SIB_TypeAndTag_sysInfoType18);
      break;
  
    default:
      WRRC_MSG1_ERROR("Unknown SIB%d", sib);
      break;
  }/* end switch */

  if( sib_tag == RRC_TOT_SIBS )
  {
    /* Unknown SIB - scheduling info not present */
    return( FALSE );
  }

  /* Now check if the given sib is scheduled in one
  of the scheduling blocks */
  if( sb1_found == TRUE )
  {
    /* Look in SB1 for this SIB  schedule */
    if( sibdb.cell[dbindex].sib_valid[rrc_SB1] )
    {
      /* If the SB1 is valid */
      /* If the MIB is valid */
      sb1_ptr = rrc_sibdb_return_sib_for_db_index(rrc_SB1,dbindex);
      if(sb1_ptr!=NULL)
      {
        ptr = &(sb1_ptr->sib_ReferenceList);
      }
      n_index = 0;
      n_value = 0;

      if(ptr != NULL)
      {
        n_value = ptr->n;
      }

      while( n_value != 0 )
      {
        /* Check if this is for SIB */
        if(RRCGET_SIB_TYPE_AND_TAG((&ptr->elem[n_index].sib_Type)) == sib_tag)
        {
          /* We found it */
          sib_schd_found = TRUE;
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
            if(for_deferred_sib == TRUE)
            {
              *def_sib_stored_in_sb1_sb2 = rrc_SB1;
            }
#endif
          break; /* Out of the while loop */
        }
        n_index++;
        n_value--;
      }
    }
    else
    {
      WRRC_MSG0_HIGH("SB1 scheduled but not received, waiting");
      /* This sib could be scheduled in SB1, We can't say for
      sure, so return TRUE for now. We'll wait till we get SB1 */
      return( TRUE );
    }
  } /* end if sb1_found == TRUE */

  if( sb2_found == TRUE )
  {
    /* Repeat the same search in Scheduling Block 2 */
    if( sibdb.cell[dbindex].sib_valid[rrc_SB2] )
    {
      /* If the SB2 is valid */
      sb2_ptr = rrc_sibdb_return_sib_for_db_index(rrc_SB2,dbindex);
      
      if(sb2_ptr!=NULL)
      {
        ptr = &(sb2_ptr->sib_ReferenceList);
      }
      n_index = 0;
      n_value = 0;

      if(ptr != NULL)
      {
        n_value = ptr->n;
      }
        
      while( n_value != 0 )
      {
        /* Check if this is for SIB */
        if(RRCGET_SIB_TYPE_AND_TAG((&ptr->elem[n_index].sib_Type)) == sib_tag)
        {
          /* We found it */
          sib_schd_found = TRUE;
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
            if(for_deferred_sib == TRUE)
            {
              *def_sib_stored_in_sb1_sb2 = rrc_SB2;
            }
#endif
          break; /* Out of the while loop */
        }
        n_index++;
        n_value--;
      }
    }
    else
    {
      WRRC_MSG0_HIGH("SB2 scheduled but not received, waiting");
      /* This sib could be scheduled in SB2, We can't say for
      sure, so return TRUE for now. We'll wait till we get SB2 */
      return( TRUE );
    }
  } /* end if sb2_found == TRUE */

  /* Finally, check if the sib scheduling info
  was found in either of the 2 blocks */
  if( sib_schd_found == TRUE )
  {
    /* It is scheduled */
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
}/* end function rrcsib_is_sib_scheduled */
   
/*===========================================================================

FUNCTION CHECK_EVENT_COMPLETION_STATUS_FOR_BPLMN

DESCRIPTION

  This function checks the status of the WTW BPLMN sib event, the index of
  which is passed to this function. If all SIBs for the event
  have been received, it will return TRUE. Else it will return FALSE.
    
DEPENDENCIES
 
  None.

RETURN VALUE

  TRUE is event has been completed, else FALSE.

SIDE EFFECTS

  None.
  
===========================================================================*/
static boolean check_event_completion_status_for_bplmn
(
  uint32 index
)
{
#ifdef FEATURE_SKIP_SIB1_3_DURING_MANUAL_PLMN_SCAN
  if((sib_events_ptr->event[index].sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)||
     (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_MANUAL_SCAN) ||
     ((sib_events_ptr->event[index].sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN) 
#ifdef FEATURE_LTE_TO_WCDMA
     && (!rrccsp_check_cgi_scan())
#endif
     ))
  {
    if(rrcsib_bplmn_sibs.mib != NULL)
    {
#ifdef FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
      if(sib_events_ptr->event[index].tsib[rrc_SB1].sib_needed == TRUE &&
         rrcsib_bplmn_sibs.sb1 == NULL)
      {
        WRRC_MSG0_HIGH("BAND_TYPE_DETECTION - SB1 not read so far ");
        return FALSE;
      }
      if(sib_events_ptr->event[index].tsib[rrc_SB2].sib_needed == TRUE &&
         rrcsib_bplmn_sibs.sb2 == NULL)
      {
        WRRC_MSG0_HIGH("BAND_TYPE_DETECTION - SB2 not read so far ");
        return FALSE;
      }
#endif

      WRRC_MSG1_HIGH(" WTOW_BPLMN:BPLMN event completed.Scan type %d",sib_events_ptr->event[index].sib_scan_type);
      rrcsib_bplmn_sibs.prev_freq=0;
      rrcsib_bplmn_sibs.prev_scr=0;
      return TRUE;
    }
    return FALSE;
  }
#endif

  if((rrcsib_bplmn_sibs.mib != NULL) && 
     (rrcsib_bplmn_sibs.sib3 != NULL) &&(
  #ifdef FEATURE_FEMTO_CSG   
     (sib_events_ptr->event[index].for_csg)||
  #endif
      (rrcsib_bplmn_sibs.sib1 != NULL)))
  {
#ifdef FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
    if((sib_events_ptr->event[index].sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)||
       (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_MANUAL_SCAN) ||
     ((sib_events_ptr->event[index].sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN) 
#ifdef FEATURE_LTE_TO_WCDMA
     && (!rrccsp_check_cgi_scan())
#endif
     ))
    {
      if(sib_events_ptr->event[index].tsib[rrc_SB1].sib_needed == TRUE &&
         rrcsib_bplmn_sibs.sb1 == NULL)
      {
        WRRC_MSG0_HIGH("BAND_TYPE_DETECTION - SB1 not read so far ");
        return FALSE;
      }
      if(sib_events_ptr->event[index].tsib[rrc_SB2].sib_needed == TRUE &&
         rrcsib_bplmn_sibs.sb2 == NULL)
      {
        WRRC_MSG0_HIGH("BAND_TYPE_DETECTION - SB2 not read so far ");
        return FALSE;
      }
    }
#endif

    WRRC_MSG1_HIGH(" WTOW_BPLMN:BPLMN event completed.Scan type %d",sib_events_ptr->event[index].sib_scan_type);
    rrcsib_bplmn_sibs.prev_freq=0;
    rrcsib_bplmn_sibs.prev_scr=0;
    return TRUE;
  }
  return FALSE;
}
    
/*===========================================================================

FUNCTION rrcsib_check_if_extension_sib_present

DESCRIPTION

  This function checks the status of the sib event, the index of
  which is passed to this function. If all SIBs for the event
  have been received, it will return TRUE. Else it will return FALSE.
    
DEPENDENCIES
 
  None.

RETURN VALUE

  TRUE is event has been completed, else FALSE.

SIDE EFFECTS

  None.
  
===========================================================================*/
boolean rrcsib_check_if_extension_sib_present
(
  uint32 dbindex    /* Database index    */
)
{
  rrc_MasterInformationBlock *mib_ptr = NULL; /* Local pointer to MIB       */
  rrc_SysInfoTypeSB1 *sb1_ptr = NULL;         /* Local pointer to SB1       */
  rrc_SysInfoTypeSB2 *sb2_ptr = NULL;         /* Local pointer to SB2       */
  boolean status = FALSE;
  rrc_ExtSIBTypeInfoSchedulingInfo_List *ext_siblist_ptr = NULL;
  uint32 n_index=0;

  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6)
  {
    if( sibdb.cell[dbindex].sib_valid[rrc_MIB] )
    {
      mib_ptr = rrc_sibdb_return_sib_for_db_index(rrc_MIB,dbindex);
      if((mib_ptr != NULL) && (EXTENSION_SIB_SCHED_PRESENT_IN_MIB(mib_ptr)))
      {
        ext_siblist_ptr = &(mib_ptr->v690NonCriticalExtensions.
          v6b0NonCriticalExtensions.masterInformationBlock_v6b0ext.extSIBTypeInfoSchedulingInfo_List);
      
        n_index = 0;
        while(ext_siblist_ptr->n > n_index)
        {
          if( RRCCHECK_SIB_TypeExt(
                  (&(ext_siblist_ptr->elem[n_index].extensionSIB_Type)),
                   systemInfoType11bis)
          )
          {
            status = TRUE;
            WRRC_MSG0_HIGH("sib11bis:Sched present for sib11bis ");
          }
          else
          {
            WRRC_MSG1_ERROR("sib11bis:Extension sib type = %d not supported", 
                 RRCGET_SIB_TYPE_AND_TAG((&ext_siblist_ptr->elem[n_index].extensionSIB_Type)));
          }
          n_index++;
        }
      }
    }
  
    if( sibdb.cell[dbindex].sib_valid[rrc_SB1] )
    {
      sb1_ptr = rrc_sibdb_return_sib_for_db_index(rrc_SB1,dbindex);
      if((sb1_ptr != NULL) && (EXTENSION_SIB_SCHED_PRESENT_IN_SB1_SB2(sb1_ptr,
              rrc_SysInfoTypeSB1,sysInfoTypeSB1)))
      {
        ext_siblist_ptr = &(sb1_ptr->v6b0NonCriticalExtensions.
            sysInfoTypeSB1_v6b0ext.extSIBTypeInfoSchedulingInfo_List);

        n_index = 0;
        while(ext_siblist_ptr->n > n_index)
        {
          if( RRCCHECK_SIB_TypeExt(
                  (&(ext_siblist_ptr->elem[n_index].extensionSIB_Type)),
                   systemInfoType11bis)
          )
          {
            status = TRUE;
            WRRC_MSG0_HIGH("sib11bis:Sched present for sib11bis ");
          }
          else
          {
            WRRC_MSG1_ERROR("sib11bis:Extension sib type = %d not supported", 
                 RRCGET_SIB_TYPE_AND_TAG((&ext_siblist_ptr->elem[n_index].extensionSIB_Type)));
          }
          n_index++;
        }
      }
    }
  
    if( sibdb.cell[dbindex].sib_valid[rrc_SB2] )
    {
      sb2_ptr = rrc_sibdb_return_sib_for_db_index(rrc_SB2,dbindex);
      if((sb2_ptr != NULL) && EXTENSION_SIB_SCHED_PRESENT_IN_SB1_SB2(sb2_ptr,
              rrc_SysInfoTypeSB2,sysInfoTypeSB2))
      {
        ext_siblist_ptr = &sb2_ptr->v6b0NonCriticalExtensions.
            sysInfoTypeSB2_v6b0ext.extSIBTypeInfoSchedulingInfo_List;
        
        n_index = 0;
        while(ext_siblist_ptr->n > n_index)
        {
          if( RRCCHECK_SIB_TypeExt(
                  (&(ext_siblist_ptr->elem[n_index].extensionSIB_Type)),
                   systemInfoType11bis)
          )
          {
            status = TRUE;
            WRRC_MSG0_HIGH("sib11bis:Sched present for sib11bis ");
          }
          else
          {
            WRRC_MSG1_ERROR("sib11bis:Extension sib type = %d not supported", 
                  RRCGET_SIB_TYPE_AND_TAG((&ext_siblist_ptr->elem[n_index].extensionSIB_Type)));
          }
          n_index++;
        }
      }
    }
  }
  else
  {
    status = FALSE;
  }

  return status;
}

#ifdef FEATURE_WCDMA_TO_LTE
/*===========================================================================

FUNCTION rrcsib_check_if_sib19_present

DESCRIPTION

  This function checks whether scheduling information for SIB19 is present in
  MIB/SB1/SB2 of the cell stored in the dbindex passed as argument.
    
DEPENDENCIES
 
  None.

RETURN VALUE

  TRUE is SIB19 is scheduled else FALSE.

SIDE EFFECTS

  None.
  
===========================================================================*/
boolean rrcsib_check_if_sib19_present
(
  uint32 dbindex    /* Database index    */
)
{
  rrc_MasterInformationBlock *mib_ptr = NULL; /* Local pointer to MIB       */
  rrc_SysInfoTypeSB1 *sb1_ptr = NULL;         /* Local pointer to SB1       */
  rrc_SysInfoTypeSB2 *sb2_ptr = NULL;         /* Local pointer to SB2       */
  boolean status = FALSE;
  rrc_ExtSIBTypeInfoSchedulingInfo_List2 *ext_siblist2_ptr = NULL;
  uint32 n_index=0;

  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6)
  {
    if( sibdb.cell[dbindex].sib_valid[rrc_MIB] )
    {
      mib_ptr = rrc_sibdb_return_sib_for_db_index(rrc_MIB,dbindex);
	  
      if((mib_ptr != NULL) && EXTENSION_SIB_SCHED_2_PRESENT_IN_MIB(mib_ptr))
      {
        ext_siblist2_ptr = &(mib_ptr->v690NonCriticalExtensions.v6b0NonCriticalExtensions.
          v860NonCriticalExtensions.masterInformationBlock_v860ext.extSIBTypeInfoSchedulingInfo_List);

        n_index = 0;
        while(ext_siblist2_ptr->n > n_index)
        {
          if(ext_siblist2_ptr->elem[n_index].extensionSIB_Type2.t ==
                T_rrc_SIB_TypeExt2_systemInfoType19)
          {
            status = TRUE;
            WRRC_MSG0_HIGH("sib19:Sched present for sib19 ");
          }
          else
          {
            WRRC_MSG1_ERROR("sib19:Extension sib type = %d not supported", 
               ext_siblist2_ptr->elem[n_index].extensionSIB_Type2.t);
          }
          n_index++;
        }   
      }      
    }
  
    if( sibdb.cell[dbindex].sib_valid[rrc_SB1] )
    {
      sb1_ptr = rrc_sibdb_return_sib_for_db_index(rrc_SB1,dbindex);
            
      if((sb1_ptr != NULL) && EXTENSION_SIB_SCHED_2_PRESENT_IN_SB1_SB2(sb1_ptr,
              rrc_SysInfoTypeSB1,sysInfoTypeSB1))
      {
        ext_siblist2_ptr = &(sb1_ptr->v6b0NonCriticalExtensions.v860NonCriticalExtensions.
          sysInfoTypeSB1_v860ext.extSIBTypeInfoSchedulingInfo_List);

        n_index = 0;
        while(ext_siblist2_ptr->n > n_index)
        {
          if(ext_siblist2_ptr->elem[n_index].extensionSIB_Type2.t ==
                T_rrc_SIB_TypeExt2_systemInfoType19)
          {
            status = TRUE;
            WRRC_MSG0_HIGH("sib19:Sched present for sib19 ");
          }
          else
          {
            WRRC_MSG1_ERROR("sib19:Extension sib type = %d not supported", 
                 ext_siblist2_ptr->elem[n_index].extensionSIB_Type2.t);
          }
          n_index++;
        } 
      }      
    }
  
    if( sibdb.cell[dbindex].sib_valid[rrc_SB2] )
    {
      sb2_ptr = rrc_sibdb_return_sib_for_db_index(rrc_SB2,dbindex);

      if((sb2_ptr != NULL) && EXTENSION_SIB_SCHED_2_PRESENT_IN_SB1_SB2(sb2_ptr,
              rrc_SysInfoTypeSB2,sysInfoTypeSB2))
      {
        ext_siblist2_ptr = &(sb2_ptr->v6b0NonCriticalExtensions.
            v860NonCriticalExtensions.sysInfoTypeSB2_v860ext.extSIBTypeInfoSchedulingInfo_List);

        n_index = 0;
        while(ext_siblist2_ptr->n > n_index)
        {
          if(ext_siblist2_ptr->elem[n_index].extensionSIB_Type2.t ==
                T_rrc_SIB_TypeExt2_systemInfoType19)
          {
            status = TRUE;
            WRRC_MSG0_HIGH("sib19:Sched present for sib19 ");
          }
          else
          {
            WRRC_MSG1_ERROR("sib19:Extension sib type = %d not supported", 
                 ext_siblist2_ptr->elem[n_index].extensionSIB_Type2.t);
          }
          n_index++;
        }      
      }      
    }
  }
  else
  {
    status = FALSE;
  }

  return status;
}
#endif

/*===========================================================================

FUNCTION CEHCK_EVENT_COMPLETION_STATUS

DESCRIPTION

  This function checks the status of the sib event, the index of
  which is passed to this function. If all SIBs for the event
  have been received, it will return TRUE. Else it will return FALSE.
    
DEPENDENCIES
 
  None.

RETURN VALUE

  TRUE is event has been completed, else FALSE.

SIDE EFFECTS

  None.
  
===========================================================================*/
static boolean check_event_completion_status
(
  uint32 index  /* Index of the event in the sib events database */
  #ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
  , boolean deferred_meas_read_supported
  #endif
)
{
  word count; /* Local counter */
  uint32 dbindex; /* Index to look into the database if needed */
  uint32 changed_sib_bitmask = 0;

#ifdef FEATURE_3GPP_CSFB
  /* If RRC state is disconnected, CSFB call in progress and event is get specific sibs for serving cell then check if all the
     mandatory SIBs have been received. If yes, conclude SIB reading and read the SIBs as part of cell selection as part of state transitions.
     For connecting to FACH there is no cell selection so we might tend to use idle mode SIBs rather than cntd mode SIBs. */
  if((rrc_csfb_call_status == TRUE) &&
     (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_NORMAL) &&
     (sib_events_ptr->event[index].event_name == RRCSIB_GET_SPECIFIC_SIBS)&&
     ((rrc_csfb_skip_sib11_opt_nv == TRUE) 
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
      || 
     ((deferred_meas_read_supported == TRUE) &&
       (RRC_STATE_DISCONNECTED == rrc_get_state()))
#endif
      )
    )
  {
    if( (sib_events_ptr->event[index].tsib[rrc_MIB].sib_received == TRUE) &&
        (sib_events_ptr->event[index].tsib[rrc_SIB1].sib_received == TRUE) &&
        (sib_events_ptr->event[index].tsib[rrc_SIB3].sib_received == TRUE) &&
        (sib_events_ptr->event[index].tsib[rrc_SIB5].sib_received == TRUE) &&
        (sib_events_ptr->event[index].tsib[rrc_SIB7].sib_received == TRUE)
      )
    {
      if(sib_events_ptr->event[index].sib_db_index_found == TRUE)
      {
          /* If value tags for a SIB matches, then dont invalidate that SIB in SIB DB*/
          for( count=1; count<RRC_TOT_SIBS; count++ )
          {
            if(sib_events_ptr->event[index].tsib[count].sib_decoded == FALSE)
            {
              sibdb.cell[sib_events_ptr->event[index].sib_db_index].sib_value_tag[count] = INVALID_MIB_SB_VALUE_TAG;
              changed_sib_bitmask = changed_sib_bitmask | (1 << count);
            }
          }
          WRRC_MSG2_HIGH("CSFB Status TRUE, Invalidating SIB mask %0X for DB index %d as value tags have changed",
          changed_sib_bitmask, sib_events_ptr->event[index].sib_db_index);
          return TRUE;  
        }
      }
    }
#endif
  
  /* Check if the event is active. It could have failed
  due to a non-gsm map PLMN or a timeout, in which case
  the event will be inactive. */
  if( sib_events_ptr->event[index].event_active == FALSE )
  {
    /* Nothing to do here */
    return(FALSE);
  }
  if( sib_events_ptr->event[index].sib_db_index_found == TRUE )
  {
    /* Database index is found. Get the index */
    dbindex = sib_events_ptr->event[index].sib_db_index;
  }
  else
  {
    /* If the database index has not been found, the
    event can't be completed. Just return FALSE */
    return(FALSE);
  }
  /*To handle the case where SB1/SB2 are removed from scheduling*/
  if((sib_events_ptr->sib_sched[rrc_SB1].repitition_rate == INVALID_SIB_SCHEDULE_REP)&&
     (sibdb.cell[dbindex].sib_valid[rrc_SB1]))
  {
    sibdb.cell[dbindex].sib_valid[rrc_SB1] = FALSE;
    rrc_sibdb_free_sib(rrc_SysInfoTypeSB1_PDU,
                       (void *)sibdb.cell[dbindex].sibs.sb1);
    sibdb.cell[dbindex].sibs.sb1 = NULL;
    memset(sibdb.cell[dbindex].sibs.sb1_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
    sibdb.cell[dbindex].sibs.sb1_encoded_pdu_length = INVALID_PDU_LENGTH;
  }

  if((sib_events_ptr->sib_sched[rrc_SB2].repitition_rate == INVALID_SIB_SCHEDULE_REP)&&
     (sibdb.cell[dbindex].sib_valid[rrc_SB2]))
  {
    sibdb.cell[dbindex].sib_valid[rrc_SB2] = FALSE;
     rrc_sibdb_free_sib(rrc_SysInfoTypeSB2_PDU,
                       (void *)sibdb.cell[dbindex].sibs.sb2);
    sibdb.cell[dbindex].sibs.sb2 = NULL;
    memset(sibdb.cell[dbindex].sibs.sb2_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
    sibdb.cell[dbindex].sibs.sb2_encoded_pdu_length = INVALID_PDU_LENGTH;
  }
  for( count = 0; count < RRC_TOT_SIBS; count++ )
  {
    /* Check if each SIB that's needed has been received */
    if( (sib_events_ptr->event[index].tsib[count].sib_needed) &&
        (!(sib_events_ptr->event[index].tsib[count].sib_decoded)) )
    {
      /* Atleast one necessary SIB has not been decoded.
      Check if this SIB is one of the mandatory SIBs. If so,
      we will return FALSE */
      switch( count )
      {
        case rrc_SIB2:
        /* Check is SIB2 is scheduled in the MIB or the Scheduling
        blocks */
        if( (rrcsib_is_sib_scheduled( rrc_SIB2,
                dbindex
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
              , FALSE
              ,NULL
#endif
        )) == TRUE
          )
        {
          WRRC_MSG0_HIGH("SIB2 is scheduled in MIB or SB");
          return( FALSE );
        }
        else
        {
          sib_events_ptr->event[index].tsib[count].sib_needed = FALSE;
          WRRC_MSG0_HIGH("SIB2 is not scheduled in MIB or SB");
        }
        break;

        case rrc_SIB18:
        /* Check is SIB18 is scheduled in the MIB or the Scheduling
           blocks */
        if( (rrcsib_is_sib_scheduled( rrc_SIB18,
              dbindex 
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
            , FALSE
            , NULL
#endif
        )) == TRUE
          )
        {
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
          if(deferred_meas_read_supported == FALSE)
#endif
          {
            WRRC_MSG0_HIGH("SIB18 is scheduled in MIB or SB");
            return( FALSE );
          }
        }
        else
        {
          sib_events_ptr->event[index].tsib[count].sib_needed = FALSE;
          WRRC_MSG0_HIGH("SIB18 is not scheduled in MIB or SB");
        }
        break;

        case rrc_SIB4:
          /* Check if SIB 4 is present in this cell
          First check if SIB3 is present. SIB3 will indicate if
          SIB4 will be transmitted or not */
          if((sibdb.cell[dbindex].sib_valid[rrc_SIB3] == TRUE)&&
             (sib_events_ptr->event[index].tsib[rrc_SIB3].sib_decoded))
          {
            if(rrcsib_check_if_connected_mode_sibs_present
              (sibdb.cell[dbindex].sibs.sib3_encoded_ptr[0],
              rrc_SIB3) == TRUE )
            {
              WRRC_MSG0_HIGH("SIB4 present in cell but not received");
              /* Check is SIB4 is scheduled in the MIB or the Scheduling
              blocks */
              if( (rrcsib_is_sib_scheduled
                    ( rrc_SIB4,
                      dbindex
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
                    , FALSE
                    , NULL
#endif
                      )) == TRUE
                )
              {
                WRRC_MSG0_HIGH("SIB4 is scheduled in MIB or SB");
                return( FALSE );
              }
              else
              {
                WRRC_MSG0_HIGH("SIB4 is not scheduled in MIB or SB");
              }
            }
          }
          else
          {
            /* SIB3 is not received yet. Make sure SIB3 is needed for this
            event. If not, we won't wait for SIB4. */
            if( sib_events_ptr->event[index].tsib[rrc_SIB3].sib_needed )
            {
              return(FALSE);
            }
          }
          break;

        case rrc_SIB6:
          /* Check if SIB 6 is present in this cell 
          First check if SIB5 is present. SIB5 will indicate if
          SIB6 will be transmitted or not */
          if((sibdb.cell[dbindex].sib_valid[rrc_SIB5] == TRUE) &&
             (sib_events_ptr->event[index].tsib[rrc_SIB5].sib_decoded))
          {

            if(rrcsib_check_if_connected_mode_sibs_present
              (sibdb.cell[dbindex].sibs.sib5_encoded_ptr[0],
              rrc_SIB5) == TRUE )
            {
              WRRC_MSG0_HIGH("SIB6 present in cell but not received");
              /* Check is SIB6 is scheduled in the MIB or the Scheduling
              blocks */
              if( (rrcsib_is_sib_scheduled
                    ( rrc_SIB6,
                      dbindex
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
                    , FALSE
                    , NULL
#endif
                      )) == TRUE
                )
              {
                WRRC_MSG0_HIGH("SIB6 is scheduled in MIB or SB");
                return( FALSE );
              }
              else
              {
                WRRC_MSG0_HIGH("SIB6 is not scheduled in MIB or SB");
              }
            }
          }
          else
          {
            /* SIB5 is not received yet. Make sure SIB5 is needed for this
            event. If not, we won't wait for SIB6. */
            if( sib_events_ptr->event[index].tsib[rrc_SIB5].sib_needed )
            {
              return(FALSE);
            }
          }
          break;

        case rrc_SIB12:
          /* Check if SIB 12 is present in this cell
          First check if SIB11 is present. SIB11 will indicate if
          SIB12 will be transmitted or not */
          
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
          if(deferred_meas_read_supported == FALSE)
#endif
          {
          if((sibdb.cell[dbindex].sib_valid[rrc_SIB11] == TRUE)&&
             (sib_events_ptr->event[index].tsib[rrc_SIB11].sib_decoded))
          {

            if(rrcsib_check_if_connected_mode_sibs_present
              (sibdb.cell[dbindex].sibs.sib11_encoded_ptr[0],
              rrc_SIB11) == TRUE )
            {
              WRRC_MSG0_HIGH("SIB 12 present in cell but not received");
              /* Check is SIB 12 is scheduled in the MIB or the Scheduling
              blocks */
              if( (rrcsib_is_sib_scheduled
                    ( rrc_SIB12,
                          dbindex
  #ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
                       , FALSE
                       , NULL
  #endif
                          )) == TRUE
                )
              {
                WRRC_MSG0_HIGH("SIB 12 is scheduled in MIB or SB");
                return( FALSE );
              }
              else
              {
                WRRC_MSG0_HIGH("SIB 12 is not scheduled in MIB or SB");
              }
            }
          }
          else
          {
            /* SIB11 is not received yet. Make sure SIB11 is needed for this
            event. If not, we won't wait for SIB12. */
            if( sib_events_ptr->event[index].tsib[rrc_SIB11].sib_needed )
            {
              return(FALSE);
            }
          }
          }
          break;

        case rrc_SB1:
          if(rrc_sibdb_check_if_sb_present(rrc_SB1, dbindex))
          {
            /* Scheduling block present but not received yet */
            WRRC_MSG0_HIGH("SB1 present in cell but not received");
            return(FALSE);
          }
          else
          {
            //invalidate SB1 presence in this cell as it is not scheduled in
            //MIB 
            sib_events_ptr->event[index].tsib[count].sib_needed = FALSE;
          }
          break;

        case rrc_SB2:
          if(rrc_sibdb_check_if_sb_present(rrc_SB2, dbindex))
          {
            /* Scheduling block present but not received yet */
            WRRC_MSG0_HIGH("SB2 present in cell but not received");
            return(FALSE);
          }
          else
          {
            //invalidate sb2's presence in this cell as it is not scheduled in
            //MIB
            sib_events_ptr->event[index].tsib[count].sib_needed = FALSE;
          }
          break;
  
        case rrc_SIB11bis:
          if( sib_events_ptr->event[index].tsib[rrc_SIB11bis].sib_needed )
          {
            if(rrcsib_check_if_extension_sib_present(dbindex) == TRUE)
            {
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
              if(deferred_meas_read_supported == FALSE)
#endif
              {
              /* SIB11bis present but not received yet */
              WRRC_MSG0_HIGH("sib11bis:Waiting to receive SIB11bis");
              return(FALSE);
            }
            }
            else
            {
              /* SIB11bis not present in this cell */
              WRRC_MSG0_HIGH("sib11bis:SIB11bis not present in this cell");
              sib_events_ptr->event[index].tsib[rrc_SIB11bis].sib_needed  = FALSE;
            }
          }
          break;

#ifdef FEATURE_WCDMA_TO_LTE
        case rrc_SIB19:
          if( sib_events_ptr->event[index].tsib[rrc_SIB19].sib_needed )
          {
            if(rrcsib_check_if_sib19_present(dbindex) == TRUE)
            {
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
          /* If DMCR is supported and LTOW redirection is in progress though SIB19 is present
             return TRUE so that SIB event will be completed */ 
              if((deferred_meas_read_supported == TRUE) && 
                 (RRC_CSP_GSM_TO_WCDMA_REDIRECTION_SCAN == rrc_csp_int_data.curr_scan) &&
                 (RRC_CSP_INTERRAT_LTE == rrc_csp_int_data.interrat_type))

              {
                /* Do nothing */
              }
             else
#endif
              {
                /* SIB19 present but not received yet */
              WRRC_MSG0_HIGH("sib19:Waiting to receive SIB19");
              return(FALSE);
            }
            }
            else
            {
              /* SIB19 not present in this cell */
              WRRC_MSG0_HIGH("sib19:SIB19 not present in this cell");
              sib_events_ptr->event[index].tsib[rrc_SIB19].sib_needed  = FALSE;
            }
          }
          break;
#endif

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
        case rrc_SIB11:
          if(deferred_meas_read_supported == TRUE)
          {
            break;
          }
          else
          {
            return(FALSE);
          }
#endif
  
        default:
          /* All other SIBs that we need are mandatory */
          return(FALSE);
      }
    }
  }/* end for */

#ifdef FEATURE_WCDMA_TO_LTE
  rrcsib_check_and_remove_sib19_if_not_scheduled(index);
#endif

  /* IF we get here, All required sibs have been received */
  return(TRUE);
}/* end function check_event_completion_status */

#ifdef FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
/*===========================================================================

FUNCTION rrcsib_is_sib5bis_scheduled

DESCRIPTION

  This function checks whether SIB5bis is present in MIB/SB in the BPLMN DB
  
DEPENDENCIES
 
  None.

RETURN VALUE

  TRUE : SIB5BIS is present
  FALSE : Otherwise

SIDE EFFECTS

  None.
  
===========================================================================*/
static boolean rrcsib_is_sib5bis_scheduled
( 
  sib_events_index_e_type index
)
{
  rrc_SIBSb_ReferenceList * sibSb_ptr=NULL;
  rrc_SIB_ReferenceList * sb_ptr = NULL;
  uint32 ix=0;
  if((sib_events_ptr->event[index].sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)||
     (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_MANUAL_SCAN)||
     (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN))
  {
    if(rrcsib_bplmn_sibs.mib != NULL)
    {
      sibSb_ptr = &rrcsib_bplmn_sibs.mib->sibSb_ReferenceList;
      for(ix=0;ix< sibSb_ptr->n;ix++)
      {
        if(RRC_CHECK_COMMON_MSG_TYPE(sibSb_ptr->elem[ix].sibSb_Type,
         rrc_SIBSb_TypeAndTag_sysInfoType5bis))
        {
          WRRC_MSG0_HIGH("BTD:-SIB5bis present in MIB. set is_sib5bis_scheduled to TRUE");
          return TRUE;
        }
        else if(RRC_CHECK_COMMON_MSG_TYPE(sibSb_ptr->elem[ix].sibSb_Type,
         rrc_SIBSb_TypeAndTag_sysInfoType5))
        {
          WRRC_MSG0_HIGH("BTD:-SIB5 present in MIB. set is_sib5bis_scheduled to FALSE");
          return FALSE;
        }
      }
    }
    if(rrcsib_bplmn_sibs.sb1!=NULL)
    {
      sb_ptr = &(rrcsib_bplmn_sibs.sb1->sib_ReferenceList);
 
      for(ix=0;ix< sb_ptr->n;ix++)
      {
        if(RRC_CHECK_COMMON_MSG_TYPE(sb_ptr->elem[ix].sib_Type,
         rrc_SIB_TypeAndTag_sysInfoType5bis))
        {
          WRRC_MSG0_HIGH("BTD:-SIB5bis present in SB1. set is_sib5bis_scheduled to TRUE");
          return TRUE;
        }
        else if(RRC_CHECK_COMMON_MSG_TYPE(sb_ptr->elem[ix].sib_Type,
            rrc_SIB_TypeAndTag_sysInfoType5))
        {
          WRRC_MSG0_HIGH("BTD:-SIB5 present in SB1. set is_sib5bis_scheduled to FALSE");
          return FALSE;
        }
      }
    }

    if(rrcsib_bplmn_sibs.sb2 !=NULL)
    {
      sb_ptr = &rrcsib_bplmn_sibs.sb2->sib_ReferenceList;
      ix=0;
      for(ix=0;ix< sb_ptr->n;ix++)
      {
        if(RRC_CHECK_COMMON_MSG_TYPE(sb_ptr->elem[ix].sib_Type,
         rrc_SIB_TypeAndTag_sysInfoType5bis))
        {
          WRRC_MSG0_HIGH("BTD:-SIB5bis present in SB2 set is_sib5bis_scheduled to TRUE");
          return TRUE;
        }
        else if(RRC_CHECK_COMMON_MSG_TYPE(sb_ptr->elem[ix].sib_Type,
         rrc_SIB_TypeAndTag_sysInfoType5))
        {
          WRRC_MSG0_HIGH("BTD:-SIB5 present in SB2 set is_sib5bis_scheduled to FALSE");
          return FALSE;
        }
      }
    }
  }

  WRRC_MSG0_HIGH("ERR:Neither SIB5/SIB5bis is present");
  return FALSE;
}
#endif
/*===========================================================================

FUNCTION POST_PROCESS_COMPLETED_EVENT

DESCRIPTION

  This function performs all the necessary post-processing for events
  that have been completed (i.e. all necessary sibs have been received).
  Post-processing includes -
  
    1) Sending a confirmation event if necessary to the module that
       requested the event.
       
    2) Tearing down the BCCH radio-bearer if it's not needed anymore.
    
    3) Cleanup of internal variables related to the event.
    
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
  
===========================================================================*/
void post_process_completed_event
(
  sib_events_index_e_type index /* Index of the event in the sib event database */
)
{
  rrc_cmd_type *cnf_ptr; /* For sending confirmation */
  uint32 dbindex = 0;        /* Index to the SIB database*/
  rrc_plmn_identity_type plmn;
  rrc_SysInfoType1 *sib1 = NULL;
  rrc_gsm_map_nas_sys_info_type nas_common_info;
  uint32 sib1_pdu_length;

  if(EVENT_INDEX_VALID(index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return;
  }

  /* Initialize PLMN to dummy value to keep lint happy */
  plmn.mcc[0] = 0;
  plmn.mcc[1] = 0;
  plmn.mcc[2] = 0;
  plmn.num_mnc_digits = 3;
  plmn.mnc[0] = 0;
  plmn.mnc[1] = 0;
  plmn.mnc[2] = 0;

  /* Stop the SIB wait timer */
  rrctmr_stop_timer( RRCTMR_SIB_WAIT_TIMER );

#ifdef FEATURE_DUAL_SIM
  rrctmr_stop_timer( RRCTMR_SIB_STOP_PARTITION_TIMER );
  rrctmr_stop_timer( RRCTMR_SIB_MASK_INVERT_TIMER );
  rrctmr_stop_timer(RRCTMR_SIB_EMPTY_MASK_TIMER);
#endif

  /*Added support for SIB scan type = RRC_SIB_CELL_ID_READ_IN_DCH_SCAN.
   *Currently this scan type is used to read MIB & SIB3 only, so If we have received 
   *MIB/SIB3 then end this event here itself by sending confirmation to RRCGPS module
   */

  if((sib_events_ptr->event[index].sib_scan_type != RRC_SIB_WTOW_AUTO_BPLMN_SCAN) &&
     (sib_events_ptr->event[index].sib_scan_type != RRC_SIB_WTOW_MANUAL_BPLMN_SCAN) &&
     (sib_events_ptr->event[index].sib_scan_type != RRC_SIB_MANUAL_SCAN) &&
     (sib_events_ptr->event[index].sib_scan_type != RRC_SIB_MANUAL_BPLMN_SCAN)
     &&(sib_events_ptr->event[index].sib_scan_type != RRC_SIB_CELL_ID_READ_IN_DCH_SCAN)
     && (sib_events_ptr->event[index].sib_scan_type != RRC_SIB_AUTO_BPLMN_SCAN))
  {
    /* Note the SIB database index for this event. */
    dbindex = sib_events_ptr->event[index].sib_db_index;
  
    /* Once a event is completed succesfully, we have to store all the
    value tags in the SIB database so that we can compare them later
    if needed. */
    /*Do not update value tags for SIB7 event. It is possible that 
      we replaced update SIBs with SIB7 event and end up having old SIBs
      but new value tags*/
    if (sib_events_ptr->event[index].int_event.int_event_type != RRCSIB_INT_SIB7_EVENT)
    {
      rrc_sibdb_store_value_tags_for_new_cell(dbindex
      #ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
                                             ,index
      #endif
                                              );
    }

  }
  /* Check if a S-BCCH teardown is required. S-BCCH is torn down
  for a RRCSIB_UPDATE_SIBS command and for any command initiated
  by the SIB procedure itself */
  if( (sib_events_ptr->event[index].event_name == RRCSIB_UPDATE_SIBS) ||
      (sib_events_ptr->event[index].req_proc == RRC_PROCEDURE_SIB)
      ||(rrc_sib_is_current_event_sib_read_in_dch() == TRUE)
    )
  {
#ifdef FEATURE_UPDATE_SIB7_IN_FACH
    if(
#ifdef FEATURE_WCDMA_HS_RACH
       (rrcsib_update_sib7_hs_rach() == TRUE)&&
#endif
       (rrc_get_state() == RRC_STATE_CELL_FACH) && 
       ((rrcllc_get_ordered_config_status() != OC_SET_FOR_FACH_CELL_PCH_TRANS) &&
       (rrcllc_get_ordered_config_status() != OC_SET_FOR_FACH_URA_PCH_TRANS))
      )
    {
      /*With feature FEATURE_UPDATE_SIB7_IN_FACH defined, 
        BCCH will be ON in FACH, So no need to tear down S-BCCH*/
      WRRC_MSG0_HIGH("RRC State is FACH, So skip Tearing down S-BCCH");
    }
    else
#endif
#ifdef FEATURE_UPDATE_SIB7_FOR_PCH_TO_FACH
     if(
#ifdef FEATURE_WCDMA_HS_RACH
        (rrcsib_update_sib7_hs_rach() == TRUE)&&
#endif
        ((rrc_get_state() == RRC_STATE_CELL_PCH)||(rrc_get_state() == RRC_STATE_URA_PCH))&&
          (sib_events_ptr->event[SIB_SRV_INDEX].int_event.int_event_type == RRCSIB_INT_SIB7_EVENT)&&
          (sib_events_ptr->event[SIB_SRV_INDEX].req_proc == RRC_PROCEDURE_SIB)
       )
     {
          WRRC_MSG0_HIGH("SIB7 Update in PCH state, So skip Tearing down S-BCCH");
     }
    else
#endif
    {
      rrcllc_chan_cfg_reason_e_type chan_config_reason = RRCLLC_CHAN_CFG_REASON_NONE;
      if(rrc_sib_is_current_event_sib_read_in_dch() == TRUE)
      {
        chan_config_reason = RRCLLC_CHAN_CFG_REASON_SIB_READ_IN_DCH;
      }
      WRRC_MSG0_HIGH("Tearing down S-BCCH");
      (void)rrcsib_configure_bcch(SBCCH_TEARDOWN, NBCCH_NOOP
                  ,chan_config_reason);
    }
  }

  /* For UPDATE_SIBS event no confirmation is required */
  if( sib_events_ptr->event[index].event_name == RRCSIB_UPDATE_SIBS )
  {
    WRRC_MSG0_HIGH("UPDATE_SIBS Event Completed");
    /* Here we have to notify other RRC procedures about
    any SIBs that may have changed for the Active Cell */
    rrc_sibdb_notify_sib_change_to_procs();

    /* Once that's done, clear the saved SIB change
    indications */
    rrc_sibdb_clear_active_cell_sib_change();
    /*Reset six hrs sib count for serving cell sib db*/
    if((rrcsib_is_event_sixhrs_sib_update(index) == TRUE)||
        (rrcsib_is_sixhrs_sib_timer_expired(index) == TRUE)
      )
    {
      WRRC_MSG1_HIGH("6hr sib:Reset six hrs sib count for serving cell sib db, currnt value=%d",
                        sibdb.cell[sibdb.active_cell_index].six_hr_count);
      sibdb.cell[sibdb.active_cell_index].six_hr_count = 0;
    }

    sib_change_safe_guard = TRUE;
    WRRC_MSG0_HIGH("Starting sib_change_safe_guard Timer");
    /*Start 1sec timer for safe guarding processing of sib_change_ind against BPLMN*/
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
    if(sib_events_ptr->update_sibs_cause == RRCSIB_DEFERRED_SIB_READ)
    {
      rrctmr_start_timer( RRC_SIB_CHANGE_GUARD_TIMER,RRCSIB_SIB_CHANGE_GUARD_TMR_FOR_DEFERRED_SIB );
    }
    else
#endif
    {
    rrctmr_start_timer( RRC_SIB_CHANGE_GUARD_TIMER,RRCSIB_SIB_CHANGE_GUARD_TIMER_VALUE );
  }
  }
  else
  {
    /* Check if the request came from another procedure or
    the SIB procedure itself. If it's from another procedure 
    and if it's not an UPDATE_SIBS command, we send a confirm command */
    if( sib_events_ptr->event[index].req_proc != RRC_PROCEDURE_SIB )
    {
      if( (cnf_ptr = rrc_get_int_cmd_buf())!=NULL )
      {
        if( sib_events_ptr->event[index].event_name == RRCSIB_GET_SPECIFIC_SIBS )
        {
          cnf_ptr->cmd_hdr.cmd_id = RRC_GET_SPECIFIC_SIBS_CNF;
          WRRC_MSG0_HIGH("Sending GET_SPECIFIC_SIBS_CNF cmd");
    
          /* Copy the frequency and Scrambling code into the
          confirmation command. For GET_SPECIFIC_SIBS command, 
          check if it's the serving cell or neigbor cell. */
          if(sib_events_ptr->event[index].for_serving_cell == TRUE)
          {
            cnf_ptr->cmd.get_sibs_cnf.freq = sib_events_ptr->srv_bcch.freq;
            cnf_ptr->cmd.get_sibs_cnf.scrambling_code = sib_events_ptr->srv_bcch.scr_code;
            /* Also Copy the frequency and Scrambling code into the
              SIB Database */
            if((sib_events_ptr->event[index].sib_scan_type != RRC_SIB_WTOW_AUTO_BPLMN_SCAN)&&
               (sib_events_ptr->event[index].sib_scan_type != RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)&&
               (sib_events_ptr->event[index].sib_scan_type != RRC_SIB_MANUAL_SCAN)&&
               (sib_events_ptr->event[index].sib_scan_type != RRC_SIB_MANUAL_BPLMN_SCAN)
               &&(sib_events_ptr->event[index].sib_scan_type != RRC_SIB_CELL_ID_READ_IN_DCH_SCAN)
              &&   (sib_events_ptr->event[index].sib_scan_type != RRC_SIB_AUTO_BPLMN_SCAN))
            {
              /* Also Copy the frequency and Scrambling code into the
              SIB Database */
              sibdb.cell[dbindex].freq = sib_events_ptr->srv_bcch.freq;
              sibdb.cell[dbindex].scr_code = sib_events_ptr->srv_bcch.scr_code;
              /* No need to reset srv_bcch status here. We expect to get a
              "GET_ALL_SIBS" command for the same, so we should not setup bcch
              again on the same cell. */
            }
            /* No need to reset srv_bcch status here. We expect to get a
            "GET_ALL_SIBS" command for the same, so we should not setup bcch
            again on the same cell. */
          }
          else
          {

            cnf_ptr->cmd.get_sibs_cnf.freq = sib_events_ptr->ngh_bcch.freq;
            cnf_ptr->cmd.get_sibs_cnf.scrambling_code = sib_events_ptr->ngh_bcch.scr_code;
            /* Also Copy the frequency and Scrambling code into the
            SIB Database */
            if((sib_events_ptr->event[index].sib_scan_type != RRC_SIB_WTOW_AUTO_BPLMN_SCAN)&&
               (sib_events_ptr->event[index].sib_scan_type != RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)&&
               (sib_events_ptr->event[index].sib_scan_type != RRC_SIB_MANUAL_SCAN)&&
               (sib_events_ptr->event[index].sib_scan_type != RRC_SIB_MANUAL_BPLMN_SCAN)&&
               (sib_events_ptr->event[index].sib_scan_type != RRC_SIB_AUTO_BPLMN_SCAN))
            {
              /* Also Copy the frequency and Scrambling code into the
              SIB Database */
              sibdb.cell[dbindex].freq = sib_events_ptr->ngh_bcch.freq;
              sibdb.cell[dbindex].scr_code = sib_events_ptr->ngh_bcch.scr_code;
              /* Make sure the ngh_bcch information is reset */
              sib_events_ptr->ngh_bcch.bcch_action =  SIB_BCCH_NO_ACTION;
            }
          }    
        }
        if((sib_events_ptr->event[index].sib_scan_type == RRC_SIB_WTOW_AUTO_BPLMN_SCAN) ||
           (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)||
           (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_MANUAL_SCAN)||
           (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN)||
           (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_AUTO_BPLMN_SCAN))
        {
          /* Copy the PLMN id */
          if( RRCGET_PLMN_TYPE((&rrcsib_bplmn_sibs.mib->plmn_Type))
                == RRCGET_PLMN_TYPE_TAG(gsm_MAP))
          {
            rrc_translate_gsm_map_plmn_id
              (&(rrcsib_bplmn_sibs.mib->plmn_Type.u.gsm_MAP->plmn_Identity), &plmn);
          }
          cnf_ptr->cmd.get_sibs_cnf.plmn_id = plmn;
          
#ifdef FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
          if(rrcsib_is_sib5bis_scheduled(index) == TRUE)
          {
            cnf_ptr->cmd.get_sibs_cnf.is_sib5bis_scheduled = TRUE;
          }
          else 
          {
            cnf_ptr->cmd.get_sibs_cnf.is_sib5bis_scheduled = FALSE;
          }
#endif

#ifdef FEATURE_SKIP_SIB1_3_DURING_MANUAL_PLMN_SCAN
          /* We dont have SIB3 for manual scans and hence we dont have the cell id.So dont
           try accessing the cell id field in BPLMN SIB3.Assign it to zero.CSP should 
           not look into this field */
          if((sib_events_ptr->event[index].sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)||
             (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_MANUAL_SCAN)||
             ((sib_events_ptr->event[index].sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN) 
#ifdef FEATURE_LTE_TO_WCDMA
             && (!rrccsp_check_cgi_scan())
#endif
             ))
          {
            cnf_ptr->cmd.get_sibs_cnf.cell_id = 0;
#ifdef FEATURE_FEMTO_CSG
            cnf_ptr->cmd.get_sibs_cnf.csg_id = sib_events_ptr->event[index].csg_id;
            cnf_ptr->cmd.get_sibs_cnf.is_csg = sib_events_ptr->event[index].is_csg;
#endif
          }
          else
#endif
          {
            /* Copy all Cell Id */
            cnf_ptr->cmd.get_sibs_cnf.cell_id = rrc_translate_cell_id(&rrcsib_bplmn_sibs.sib3->cellIdentity);
#ifdef FEATURE_FEMTO_CSG
            cnf_ptr->cmd.get_sibs_cnf.csg_id = sib_events_ptr->event[index].csg_id;
            cnf_ptr->cmd.get_sibs_cnf.is_csg = sib_events_ptr->event[index].is_csg;
#endif
          }

          /* We dont ahve a db index.Set it to 0.CSP should NOT look into this field */ 
          cnf_ptr->cmd.get_sibs_cnf.sib_index_for_cell = 0;
        }
        else if(sib_events_ptr->event[index].sib_scan_type == RRC_SIB_CELL_ID_READ_IN_DCH_SCAN)
        {
          /* Get the plmn id for this cell. Make sure the
          PLMN id is a GSM-MAP PLMN. Convert it to the internal format used by RRC. */
          if(sib_events_ptr->event[index].tempholder.mib != NULL)
          {
            if( sib_events_ptr->event[index].tempholder.mib->plmn_Type.t
                == T_rrc_PLMN_Type_gsm_MAP )
            {
              rrc_translate_gsm_map_plmn_id 
                (&(sib_events_ptr->event[index].tempholder.mib->plmn_Type.u.gsm_MAP->plmn_Identity),
                 &plmn
                );
            }
          }
          cnf_ptr->cmd.get_sibs_cnf.plmn_id = plmn;
          if(sib_events_ptr->event[index].tempholder.sib3 != NULL)
          {
             cnf_ptr->cmd.get_sibs_cnf.cell_id  = rrc_translate_cell_id
                      ( &(sib_events_ptr->event[index].tempholder.sib3->cellIdentity));
              MSG_HIGH("RRCGPS:Rexd Cell I.D CellId %d and MCC %d MNC %d", 
                       cnf_ptr->cmd.get_sibs_cnf.cell_id,
                       RRC_CSP_GET_INT_MCC(plmn), 
                       RRC_CSP_GET_INT_MNC(plmn));
          }
          else
          {
            WRRC_MSG0_HIGH("RRCGPS: Error temp sib3 ptr = NULL");
            cnf_ptr->cmd.get_sibs_cnf.cell_id = 0;
#ifdef FEATURE_FEMTO_CSG
            cnf_ptr->cmd.get_sibs_cnf.csg_id = sib_events_ptr->event[index].csg_id;
            cnf_ptr->cmd.get_sibs_cnf.is_csg = sib_events_ptr->event[index].is_csg;
#endif
          }
          if(sib_events_ptr->event[index].tsib[rrc_SIB1].sib_received == TRUE)
          {
           /* Get the PDU length in bytes */
           sib1_pdu_length = GET_PDU_LENGTH_IN_BYTES
                              (sib_events_ptr->event[index].tsib[rrc_SIB1].
                               sib_tot_length);

            /* Decode SIB1 */
            sib1 = 
            (rrc_SysInfoType1 *)rrcasn1_decode_pdu( 
                               (void *)(sib_events_ptr->event[index].
                               tsib[rrc_SIB1].sib_data_ptr), /* Encoded SIB1 */
                               rrc_SysInfoType1_PDU, /* SIB1's PDU number         */
                               sib1_pdu_length       /* SIB1's length in bytes    */
                               );

            if(sib1 !=NULL) 
            {
              (void)rrccsp_fill_nas_common_info(sib1, &nas_common_info);
              cnf_ptr->cmd.get_sibs_cnf.lac[0]=nas_common_info.value[0];
              cnf_ptr->cmd.get_sibs_cnf.lac[1]=nas_common_info.value[1];
              (void)rrcasn1_free_pdu(rrc_SysInfoType1_PDU,(void*)sib1);
              sib1 = NULL;
            }
            else
            {
              WRRC_MSG0_ERROR("RRCGPS: SIB1 failed to decode. Not sending LAC info");
            }
          }
        }
        else
        {
          /* Copy all the necessary parameters for the confirmation command. */
          cnf_ptr->cmd.get_sibs_cnf.cell_id = sibdb.cell[dbindex].cell_id;
          cnf_ptr->cmd.get_sibs_cnf.plmn_id = sibdb.cell[dbindex].plmn_id;
#ifdef FEATURE_FEMTO_CSG
          cnf_ptr->cmd.get_sibs_cnf.csg_id = sib_events_ptr->event[index].csg_id;
          cnf_ptr->cmd.get_sibs_cnf.is_csg = sibdb.cell[dbindex].is_csg;
#endif
          /* SIB DB Index is also sent in the confirmation so that the procedure
          can use the index to read the SIBs if needed. Alternately the PLMN and
          Cell Id can also be used to read SIBs. */
          cnf_ptr->cmd.get_sibs_cnf.sib_index_for_cell = dbindex; 
        }

        /*Boolean to indicate whether SIB read over OTA or through the 
          SYS_INFO_CONTAINER*/
#ifdef FEATURE_3GPP_CSFB
        if(sib_events_ptr->event[index].sib_scan_type == RRC_SIB_READ_FROM_SIB_CONTAINER)
        {
          cnf_ptr->cmd.get_sibs_cnf.is_sib_read_from_container = TRUE;
        }
        else
        {
          cnf_ptr->cmd.get_sibs_cnf.is_sib_read_from_container = FALSE;
        }
#endif        
        /* Procedure name */
        cnf_ptr->cmd.get_sibs_cnf.procedure = sib_events_ptr->event[index].req_proc;
 
  
        /* Indicate that the event succeeded. This is the default
        for now. If we have to send confirmations with a failure status, this
        needs to be changed. */
        cnf_ptr->cmd.get_sibs_cnf.status = SIB_EVENT_SUCCEEDED;
  
        /* Now send the command */
        rrc_put_int_cmd( cnf_ptr );
      }
    }
    else
    {
      /* This command was requested by the SIB procedure itself.
      Process it accordingly */
      rrcsib_process_int_sib_event_complete( index, SUCCESS );
    }
  }/* end if event is not UPDATE_SIBS event */

#ifdef FEATURE_RRC_NO_MIB_FOUND_BAR_CELL
  no_mib_bar_cell_count = 0;
#endif

  /* Now do the necessary cleanup for this event */
  sib_events_ptr->event[index].event_active = FALSE;
  sib_events_ptr->event[index].sib_scan_type = RRC_SIB_NORMAL;

#ifdef FEATURE_UPDATE_SIB7_IN_FACH
#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
  REX_ISR_LOCK(&sib7_in_fach_active_mutex);

  if(is_sib7_in_fach_active == TRUE)
  {
    is_sib7_in_fach_active = FALSE;
  }
  REX_ISR_UNLOCK(&sib7_in_fach_active_mutex);
#endif
#endif

  /* For safety, in case the tempholer pointers are not freed, free them
  now */
  rrcsib_free_tsib(index);

}/* end function post_process_completed_event */

/*===========================================================================

FUNCTION RRCSIB_PROCESS_STATE_CHANGE_IND

DESCRIPTION

  This function does the necessary processing when RRC state
  changes to disconnected state in idle mode. This includes
  reseting the BCCH parameters and cancelling any sib events
  that have not been completed.

  The reason for cancelling sib events is that as soon as
  we move back to disconnected state, the cell selection
  procedure will start the cell selection process by sending
  fresh GET_SPECIFIC_SIBS and GET_ALL_SIBS cmds.
    
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  BCCH logical channels may be set up and teared down. 
===========================================================================*/
static void rrcsib_process_state_change_ind
( 
  rrc_cmd_type *ptr /* Received command */
)
{
  sib_events_index_e_type count;              /* Counter */
  boolean drop_serving_bcch = TRUE;


  WRRC_MSG0_HIGH("SIB module processing state change indication");
  /* Check what state change occured */
  if( ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED ) 
  {
    if(sib_events_ptr == NULL)
    {
      /* Invalidating the serving cell index if WCDMA is stopped already
         Serving cell index is not invalidated for WTOL PSHO scenario because W is stopped before state change is received */
      rrc_sibdb_clear_active_cell_status();
      WRRC_MSG0_HIGH("sib_events_ptr freed.");
      return;
    }

    rrcsib_cmd_store.pending_flag = FALSE;
    update_sibs_event_pending = FALSE;
  
    /* State changed to disconnected. We'll re-acquire the cell
    and get all the SIBs. Cancel any events that are in 
    progress */
    for( count = SIB_SRV_INDEX; count < MAX_SIB_EVENTS_INDEX; count++ )
    {
      if( sib_events_ptr->event[count].event_active == TRUE )
      {
        WRRC_MSG1_ERROR("Going to Idle: SIB event %d failed",
            sib_events_ptr->event[count].event_name);
        /* Post-process the failed event */
        rrcsib_post_process_failed_event
          ( count, /* Event Index                 */
            TRUE,  /* Confirm Command is required */
            SIB_EVENT_FAILED_INCOMPATIBLE_STATE, /* Failure cause */
            FALSE  /* Don't tear down S_BCCH */
          );
      }
    }

    //clear the sibdb active-status..  We won't have an active serving cell 
    //until we do cell selection again
    rrc_sibdb_clear_active_cell_status();
  }

  if( ptr->cmd.state_change_ind.new_state == RRC_STATE_CELL_DCH ) 
  {
    /* State changed to CELL_DCH. LLC would have brought down
    the BCCH-BCH pipe. Cancel any events that are in 
    progress */

    update_sibs_event_pending = FALSE;

    for( count = SIB_SRV_INDEX; count < MAX_SIB_EVENTS_INDEX; count++ )
    {
      if( sib_events_ptr->event[count].event_active == TRUE )
      {
        WRRC_MSG1_ERROR("Going to DCH: SIB event %d failed",
            sib_events_ptr->event[count].event_name);
        /* Post-process the failed event. No need to
        tear down S-BCCH since it's already torn down
        by LLC while going to DCH. */
        rrcsib_post_process_failed_event
          ( count, /* Event Index                 */
            TRUE,  /* Confirm Command is required */
            SIB_EVENT_FAILED_INCOMPATIBLE_STATE, /* Failure cause */
            FALSE  /* Tear down S_BCCH if it's up */
          );
      }
    }
    rrcpg1_check_and_abort_bcch_mod_info_processing();
  }

  if( ptr->cmd.state_change_ind.new_state == RRC_STATE_CELL_FACH )
  {
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
    rrcsib_update_sib_for_deferred_sib();
#endif

    /* We've moved to CELL_FACH. Start timer based update of SIB7 */ 
  }

  if((( ptr->cmd.state_change_ind.new_state == RRC_STATE_CELL_PCH ) ||
      (ptr->cmd.state_change_ind.new_state == RRC_STATE_URA_PCH ))&&
      (ptr->cmd.state_change_ind.previous_state == RRC_STATE_CELL_DCH))
  {
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
    rrcsib_update_sib_for_deferred_sib();
#endif
  }

#ifdef FEATURE_UPDATE_SIB7_IN_FACH
#ifdef FEATURE_WCDMA_HS_RACH
  if(rrcsib_update_sib7_hs_rach() == TRUE)
#endif
  {
  if(( ptr->cmd.state_change_ind.previous_state == RRC_STATE_CELL_FACH)&&
      (( ptr->cmd.state_change_ind.new_state == RRC_STATE_CELL_PCH)||
       ( ptr->cmd.state_change_ind.new_state == RRC_STATE_URA_PCH)
      )
    )
  {
    for( count = SIB_SRV_INDEX; count < MAX_SIB_EVENTS_INDEX; count++ )
    {
      if( sib_events_ptr->event[count].event_active == TRUE )
      {
        /*For UE going out of FACH, always clean up SIB7 event.
          Since SIB7 reading is only appicable in cell FACH*/
        if(sib_events_ptr->event[count].event_name == RRCSIB_GET_SPECIFIC_SIBS)
        {
          if((sib_events_ptr->event[count].int_event.int_event_type == RRCSIB_INT_SIB7_EVENT)&&
             (sib_events_ptr->event[count].req_proc == RRC_PROCEDURE_SIB)&&
             (sib_events_ptr->event[count].for_serving_cell == TRUE)
            )
          {
            WRRC_MSG1_HIGH("sib7:Going out of FACH SIB event %d failed", 
                     sib_events_ptr->event[count].event_name);
             /* Post-process the failed event. No need to
                 tear down S-BCCH since it's already torn down
                 by LLC while going to PCH. */
            rrcsib_post_process_failed_event
              ( (sib_events_index_e_type)count, /* Event Index                 */
                FALSE,  /* Confirm Command is required */
                SIB_EVENT_FAILED_INCOMPATIBLE_STATE, /* Failure cause */
                FALSE  /* Don't tear down S_BCCH */
               );
            break;
          }
        }
        else if (sib_events_ptr->event[count].event_name == RRCSIB_UPDATE_SIBS) 
        {
          drop_serving_bcch = FALSE;
        }
      }/* End if active events */
    }

    if((drop_serving_bcch == TRUE)&&(rrcllc_get_s_bcch_setup_status() == TRUE))
    {
      (void)rrcsib_configure_bcch(SBCCH_TEARDOWN, NBCCH_NOOP
                                  ,RRCLLC_CHAN_CFG_REASON_NONE
                                  );
    }
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
    rrcsib_update_sib_for_deferred_sib();
#endif
  }
  }
#endif
  /* Add other state changes here */
}

/*===========================================================================

FUNCTION RRCSIB_PROCESS_SIB_WAIT_TIMER_EXPIRATION

DESCRIPTION

  This function does the necessary processing when 
  the SIB wait timer expires. The SIB wait timer 
  is the maximum time to wait for SIBs in a cell.
  
  Note that only one SIB event is supported at a time.
  Hence only one timer is needed. If many SIB events are
  supported simultaneously, many timers would be
  needed.  

DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  BCCH logical channels may be teared down. 
===========================================================================*/
static void rrcsib_process_sib_wait_timer_expiration( void )
{
  sib_events_index_e_type count;          /* Local counter */

  /* Check if any event is active */
  for( count=SIB_SRV_INDEX; count<MAX_SIB_EVENTS_INDEX; count++ )
  {
    if( sib_events_ptr->event[count].event_active == TRUE )
    {
    
      if((sib_events_ptr->event[count].sib_scan_type == RRC_SIB_WTOW_AUTO_BPLMN_SCAN) || 
         (sib_events_ptr->event[count].sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)||
         (sib_events_ptr->event[count].sib_scan_type == RRC_SIB_MANUAL_SCAN))
      { 
        WRRC_MSG0_HIGH("Do not tear down S-BCCH during BPLMN and MIB/SIB timer expired");
        rrcsib_post_process_failed_event((sib_events_index_e_type)count,TRUE,SIB_EVENT_FAILED_TIMEOUT, FALSE);
        break;
      }
      else
      {
        /* An event was active. Since the timer expired,
        this event failed. */
        rrcsib_post_process_failed_event
          ( count, /* Event Index                 */
            TRUE,  /* Confirm Command is required */
            SIB_EVENT_FAILED_TIMEOUT, /* Timed out*/
            TRUE   /* Tear down S_BCCH if it's up */
            );
      }
      break; /* Out of the for loop since only one event is active
                   at a time. */
    }/* End if active events */
  }/* End for */
}/* End function rrcsib_process_sib_wait_timer_expiration */


/*===========================================================================

FUNCTION RRCSIB_PROCESS_SIB7_EXP_TIMER_EXPIRATION

DESCRIPTION

  This function processes the indication that the
  SIB7 expiration timer has expired. It creates a
  command to read SIB7 again. 

DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  S-BCCH logical channels will be setup.
===========================================================================*/
static void rrcsib_process_sib7_exp_timer_expiration( void )
{
  rrc_cmd_type *cmd; /* To send a SIB command if needed */
  uint32 count;      /* Local Counter */

  /* Check if we're in CELL_FACH or CONNECTING. In other states we 
  won't update SIB7 based on a timer. */

#ifdef FEATURE_WCDMA_HS_RACH
  if(rrcsib_update_sib7_hs_rach() == FALSE)
    return;
#endif
  if( (rrc_get_state() == RRC_STATE_CELL_FACH) ||
      (rrc_get_state() == RRC_STATE_CONNECTING) )
  {
    /* Generate a command for the SIB procedure itself to
    get SIB7. Make sure there's not already a pending
    command  */
    if( sib7info.sib_ext_proc_update_req == FALSE )
    {
      if((cmd = rrc_get_int_cmd_buf())!=NULL)
      {
        cmd->cmd_hdr.cmd_id = RRC_GET_SPECIFIC_SIBS_REQ;
        cmd->cmd.get_specific_sibs_req.for_serv_cell = TRUE;
        for( count=0; count<RRC_TOT_SIBS; count++ )
        {
          cmd->cmd.get_specific_sibs_req.sib_req[count] = FALSE;
        }
        /* Only SIB7 is requested. */
        cmd->cmd.get_specific_sibs_req.sib_req[rrc_SIB7]
          = TRUE;
        cmd->cmd.get_specific_sibs_req.procedure = RRC_PROCEDURE_SIB;
        /* Get the frequency and the scrambling code from the
        SIB database itself */
        cmd->cmd.get_specific_sibs_req.freq = 
          sibdb.cell[sibdb.active_cell_index].freq;
        cmd->cmd.get_specific_sibs_req.scrambling_code =
          sibdb.cell[sibdb.active_cell_index].scr_code;
        cmd->cmd.get_specific_sibs_req.sib_scan_type = RRC_SIB_NORMAL;

        WRRC_MSG0_HIGH("sib7:posting RRC_GET_SPECIFIC_SIBS_REQ to sib");

        rrc_put_int_cmd( cmd );

        /* Set an internal variable to indicate that SIB7 update
        is based on a timer expiration. */
        sib7info.sib_timer_update_req = TRUE;
      }
    }
    else
    {
      /* Can't support timer based request when an external
      request is pending. */
      WRRC_MSG0_ERROR("SIB7 request error");
    }
  }
  else
  {
    WRRC_MSG0_HIGH("Ignoring SIB7 tmr exp");
  }
}/* End function rrcsib_process_sib7_exp_timer_expiration */

/*===========================================================================

FUNCTION RRCSIB_PROCESS_BCCH_SETUP_FAILURE

DESCRIPTION

  This function does the necessary processing when 
  the BCCH setup fails.
  
  Note that only one SIB event is supported at a time.
  If many SIB events are supported simultaneously,
  we have to check which event had the BCCH setup
  failure. 

DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
   
===========================================================================*/
static void rrcsib_process_bcch_setup_failure( void )
{
  sib_events_index_e_type count;

  /* Check if any event is active. Only one should
  be active - send a failure status for that event. */
  for( count=SIB_SRV_INDEX; count<MAX_SIB_EVENTS_INDEX; count++ )
  {
    if( sib_events_ptr->event[count].event_active == TRUE )
    {
      /* An event was active. Since the BCCH setup failed,
      send a failure status. */
      WRRC_MSG1_HIGH("BCCH setup failed for event %d", count);
      rrcsib_post_process_failed_event
        ( count, /* Event Index                 */
          TRUE,  /* Confirm Command is required */
          SIB_EVENT_FAILED_LOWER_LAYERS, /* Failure reason */
          FALSE   /* Do not tear down S_BCCH */
        );
      break; /* Out of the for loop since only one event is active
             at a time. */
    }/* End if active events */
  }/* End for */
}/* End function rrcsib_process_bcch_setup_failure */
/*===========================================================================

FUNCTION RRCSIB_CHECK_MIB_READ_STATUS

DESCRIPTION

  This is the function get's the current active index of sib_events_ptr->
  Takes care of pending configuration for a particular event.
  
DEPENDENCIES
 
  None.

RETURN VALUE
  TRUE:  If MIB is already read
  FALSE: If MIB is yet to be read
  
SIDE EFFECTS

  None.
 
===========================================================================*/
boolean rrcsib_check_mib_read_status
(
  boolean for_serv_cell
)
{
  sib_events_index_e_type index;  /* Index in the SIB events database
                                     to store information about a SIB event. */
  if(TRUE == for_serv_cell)
  {
    index = SIB_SRV_INDEX;
  }
  else
  {
    index = SIB_NGH_INDEX;
  }

  /* A valid BCCH-BCH message has been received and it needs to be 
     processed. But first check if the event for this index is active.  */
  if( sib_events_ptr->event[index].event_active == FALSE )
  {
    WRRC_MSG1_HIGH("Index not active %d", index);
  }
  /* If the event is active, make sure we've already received
  the confirmation from LLC that the BCCH has been setup for this
  event. Otherwise this message may not be for this event, so we
  have to ignore it. */
  else if( (index == SIB_SRV_INDEX) && 
           (sib_events_ptr->srv_bcch.bcch_action != SIB_BCCH_SETUP_COMPLETE)
         )
  {
    WRRC_MSG0_HIGH("S-BCCH not yet setup");
  }
  else if( (index == SIB_NGH_INDEX) && 
           (sib_events_ptr->ngh_bcch.bcch_action != SIB_BCCH_SETUP_COMPLETE)
         )
  {
    WRRC_MSG0_HIGH("N-BCCH not yet setup");
  }
  else
  {
    /* if event is active and is setup correctly, then check to see if mib is received and
     * decoded.if yes return true else return false
     */
    if(FALSE == sib_events_ptr->event[index].tempholder.mib_decoded)
    {
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }

  /* By default if event is not active or we are still waiting for cnf from LLC/L1
   * for channel configuration, then return FALSE to indicate that MIB is not yet read.
   */
  return FALSE;
}

/*===========================================================================

FUNCTION PROCESS_BCCH_MESSAGE

DESCRIPTION

  This is the function that processes the received BCCH message.
  The received message can be a BCCH-BCH message or a BCCH-FACH
  message. In case of a BCCH-FACH message, the message type will
  be either a System Information Message or a System Information
  change indication message.
  
  If SIBS have been received in this message, this function also
  calls the necessary functions to decode and store the SIBs.
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
 
===========================================================================*/
static void process_bcch_message
(
  rrc_cmd_type *cmd_ptr  /* Pointer to command */
)
{
  
  rrc_SystemInformation_BCH *msg; /* Local pointer to point to the
                                  the received System Information
                                  message */
  sib_events_index_e_type index;  /* Index in the SIB events database
                                  to store information about a SIB event. */
  rrc_sib_collect_e_type result;  /* To save the result of a process_sys
                                  _info_msg function */
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
  boolean deferred_meas_read_supported = FALSE;
#endif
  boolean band_mismatch = FALSE;  /* Boolean to indicate band mismatch */
  
  /* Find out whether the message is a BCCH-BCH message or a BCCH-FACH
  message. This is done by checking the logical channel id included 
  in the command. In case of a BCCH-BCH message save the index to
  store the contents of this msg. One index is used for serving cell
  BCCH and the other for neighbor cell BCCH. */
  if((rrclcm_get_dl_rb_id(cmd_ptr->cmd.downlink_sdu_ind.lc_id)) == 
    BCCH_S_RADIO_BEARER_ID)
  {
    index = SIB_SRV_INDEX;  /* Message is from Serving cell BCCH */
  }
  else if((rrclcm_get_dl_rb_id(cmd_ptr->cmd.downlink_sdu_ind.lc_id)) == 
    BCCH_N_RADIO_BEARER_ID)
  {
    index = SIB_NGH_INDEX;  /* Message is from Neighbor cell BCCH */     
  }
  else if((rrclcm_get_dl_rb_id(cmd_ptr->cmd.downlink_sdu_ind.lc_id)) == 
    BCCH_FACH_RADIO_BEARER_ID)
  {
    WRRC_MSG0_HIGH("BCCH-FACH message received");
    //This is not supported yet. When it's supported, we'll check if
    //it's a System Information Change Indication message and if so,
    //process it. We don't process a System Information message on
    //the FACH since only SIB10 is transmitted in the FACH.
    index = MAX_SIB_EVENTS_INDEX;  /* Message is not BCCH-BCH. */     
  }
  else
  {
    WRRC_MSG0_ERROR("Non-BCCH message delivered to SIB module!");
    index = MAX_SIB_EVENTS_INDEX;  /* Invalid message */
  }
      
  if( index < MAX_SIB_EVENTS_INDEX )
  {
    /* A valid BCCH-BCH message has been received and it needs
    to be processed. But first check if the event for this index
    is active.  */

    if( sib_events_ptr->event[index].event_active == FALSE )
    {
#ifdef FEATURE_UPDATE_SIB7_IN_FACH
      if(index == SIB_SRV_INDEX)
      {
        if(rrcsib_can_sib7_event_start() == TRUE)
        {
          sib7info.sib_ext_proc_update_req = FALSE;
          rrcsib_process_sib7_exp_timer_expiration();
        }
        else
        {
          WRRC_MSG0_HIGH("Ignoring the message since event not active");
        }
      }
      else
#endif
      {
        MSG_LOW("Ignoring the message since event not active", 0, 0, 0);
      }
    }
    /* If the event is active, make sure we've already received
    the confirmation from LLC that the BCCH has been setup for this
    event. Otherwise this message may not be for this event, so we
    have to ignore it. */
    else if( (index == SIB_SRV_INDEX) && 
             (sib_events_ptr->srv_bcch.bcch_action != SIB_BCCH_SETUP_COMPLETE)
           )
    {
      WRRC_MSG0_HIGH("Ignoring msg: BCCH setup ack rxed");
    }
    else if( (index == SIB_NGH_INDEX) && 
             (sib_events_ptr->ngh_bcch.bcch_action != SIB_BCCH_SETUP_COMPLETE)
           )
    {
      WRRC_MSG0_HIGH("Ignoring msg: BCCH setup ack rxed");
    }
    else
    {
      /* Point a local pointer to the received message */
      msg = (rrc_SystemInformation_BCH *)(cmd_ptr->cmd.downlink_sdu_ind.decoded_msg);

#if ((!defined(FEATURE_RRC_UNIT_TEST)) && (!defined(TEST_FRAMEWORK)))
      //only if sib-schedule matches continue processing, else do not process sibs
      if((rrcsib_check_sib_schedule_for_sibs(msg
                                             ,index
                                             ) == FALSE)
        )
      {
        return;
      }
#endif

      /* Now process the message */
      result = process_sys_info_msg( msg, index );

      if( result == SIBS_COMPLETED )
      {
        /* The sibcollect module has completed atleast one SIB.
        We need to look at the SIB event database for completed
        SIBs and ASN.1 decode them. */
        if(FALSE == process_completed_sibs( index ))
        {
          WRRC_MSG0_ERROR("Aborting SIB reading");
          return;
        }

        if((sib_events_ptr->event[index].sib_scan_type == RRC_SIB_NORMAL)&&
           (sib_events_ptr->event[index].int_event.int_event_type != RRCSIB_INT_SIB7_EVENT )&&
           (sib_events_ptr->event[index].tsib[rrc_MIB].sib_received == TRUE)
#ifdef FEATURE_DUAL_SIM
           &&(rrctmr_get_remaining_time(RRCTMR_SIB_EMPTY_MASK_TIMER) == 0)
#endif
           )
        {
          if((sib_sleep_after_sb1_sb2 == TRUE)
#ifdef FEATURE_DUAL_SIM
              ||(g_in_data_call)
#endif
             )
          {
            if(((!sib_events_ptr->event[index].sched_present_for_sibs[rrc_SB1])||
                ((sib_events_ptr->event[index].sched_present_for_sibs[rrc_SB1])&&
                (sib_events_ptr->event[index].tempholder.sb1_decoded))
             )&&
            ((!sib_events_ptr->event[index].sched_present_for_sibs[rrc_SB2])||
             ((sib_events_ptr->event[index].sched_present_for_sibs[rrc_SB2])&&
              (sib_events_ptr->event[index].tempholder.sb2_decoded))
             )
            )
            {
              rrcsib_update_sib_sleep_schedule_table(index);
            }
          }
          else
          {
            rrcsib_update_sib_sleep_schedule_table(index);
          }
        }
      }

      if(sib_events_ptr->event[index].sib_scan_type == RRC_SIB_CELL_ID_READ_IN_DCH_SCAN)
      {
        /*If the event is cell id read in DCH, then check & perform event completion status here*/
        if(check_event_completion_status_for_cell_id_read((uint32)index) == TRUE )
        {
          post_process_completed_event( index );
        }
      }
      else
      /* Before exiting, also check the event status to determine
      if all SIBs for the event have been received. If so, a confirmation
      event is sent to the calling procedure. */
      if((sib_events_ptr->event[index].sib_scan_type == RRC_SIB_WTOW_AUTO_BPLMN_SCAN) ||
         (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN) ||
         (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_MANUAL_BPLMN_SCAN) ||
         (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_MANUAL_SCAN)||
         (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_AUTO_BPLMN_SCAN))
      {
        if(check_event_completion_status_for_bplmn((uint32)index) == TRUE )
        {
          post_process_completed_event( index );
        }
      }
      else 
      {

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
         if(rrcsib_can_event_support_deferred_sib(index)== TRUE)
         {
           if(sib_events_ptr->event[index].sib_db_index_found == TRUE)
           {
             deferred_meas_read_supported =
               rrcsib_can_nw_support_deferred_sib(sib_events_ptr->event[index].sib_db_index);
           }
         }
#endif

        /* Before exiting, also check the event status to determine
        if all SIBs for the event have been received. If so, a confirmation
        event is sent to the calling procedure. */

        if( check_event_completion_status( (uint32)index 
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
        ,deferred_meas_read_supported
#endif
          ) == TRUE )
        {
  
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
          if(deferred_meas_read_supported == TRUE)
          {  
            rrcsib_post_process_deferred_sib(index, sib_events_ptr->event[index].sib_db_index);
          }
#endif

          /* Check for band mismatch */
          if( (sib_events_ptr->event[index].event_name == RRCSIB_GET_SPECIFIC_SIBS) && 
              (sib_events_ptr->event[index].req_proc == RRC_PROCEDURE_CSP) &&
              (sib_events_ptr->event[index].sib_db_index_found) )
          {
            band_mismatch = rrcsib_check_band_mismatch_from_sib5_sib6((uint32)index);
          }
  
          if(TRUE == band_mismatch) 
          {
            /* Save the barred freq to not skip scanning of the same in other overlapping band*/
            rrc_csp_save_barred_freq();
            rrc_csp_int_data.barred_due_to_band_mismatch = TRUE;

            WRRC_MSG0_HIGH("Band mismatch: Barring entire frequency for 1280 sec because of band mismatch. Failing SIB event");
            rrccsp_send_cphy_cell_bar_req(RRC_CSP_WCDMA_FREQ_BARRED, 1280);

            rrcsib_post_process_failed_event
                     ( index, /* Event Index                 */
                       TRUE,  /* Confirm Command is required */
                       (rrc_sib_status_e_type)SIB_EVENT_FAILED_INVALID_SIBS, /* Failure Cause       */
                       TRUE   /* Tear down S_BCCH if it's up */
                     );
          }
          else
          {
            if(sib_events_ptr->event[index].for_serving_cell == FALSE)
            {
              if(rrctmr_get_remaining_time(RRCTMR_WAIT_FOR_L2_ACK_FOR_PCH_TRANSITION_TIMER) != 0)
              {
                WRRC_MSG0_HIGH("RRC_DEBUG: Delaying reselection as UE is waiting for L2 ACK");
                rrcsib_post_process_failed_event
                     ( index, /* Event Index                 */
                       TRUE,  /* Confirm Command is required */
                       (rrc_sib_status_e_type)SIB_EVENT_FAILED_L2_ACK_WAIT, /* Failure Cause       */
                       TRUE   /* Tear down S_BCCH if it's up */
                     );
                return;
              }
            }
            /* This event has been completed. Call the post-processing
            function to send the event confirmation if needed,
            do the necessary cleanup and teardown the BCCH radio bearer
            if necessary. */
            post_process_completed_event( index );
          } 
        }/* end if( check_event_completion_status( (uint32)index ) == TRUE ) */
      }
    }/* end Else */
  }/* end if( index < MAX_SIB_EVENTS_INDEX ) */
}/* end function process_bcch_message */


/*===========================================================================
FUNCTION RRCSIB_CHECK_FOR_DUPLICATE_UPDATE_SIBS_CMD
DESCRIPTION
  This function checks if there is already a active RRC_UPDATE_SIBS_IND
  event. It should be called when a new RRC_UPDATE_SIBS_IND is received.
DEPENDENCIES
  None.
RETURN VALUE
  TRUE if there is a event, FALSE if not.
SIDE EFFECTS
  None. 
===========================================================================*/
static boolean rrcsib_check_for_duplicate_update_sibs_cmd( uint32 mib_value_tag )
{
  uint32 count; /* Counter used in loop */
  /* Check if any event is active. If so, check the event type  */
  for( count = 0; count < (uint32)MAX_SIB_EVENTS_INDEX; count++ )
  {
    if( sib_events_ptr->event[count].event_active == TRUE )
    {
      if( sib_events_ptr->event[count].event_name == RRCSIB_UPDATE_SIBS )
      {
        if(sib_events_ptr->mib_value_tag_from_command == mib_value_tag)
        {
          /* Event already exists */
          return( TRUE );
        }
        else
        {
          //event is being over-written by a new UPDATE_SIBs as vtag's 
          //are changed..  need to fail the old one and start looking
          //for a MIB with new value tag
          rrcsib_post_process_failed_event
            ( (sib_events_index_e_type)count,       /* Event Index                 */
              FALSE,                                /* Confirm Command is required */
              SIB_EVENT_VALID_DUPLICATE_UPDATE_IND, /* Duplicate valid SIB_UPDATE*/
              TRUE                                  /* Tear down S_BCCH if it's up */
            );

          WRRC_MSG2_HIGH("Valid BCCH-Mod Intrpt. Old-Vtag->%d New-Vtag->%d", 
                   sib_events_ptr->mib_value_tag_from_command, mib_value_tag);

          return ( FALSE );
        }
      }
    }
  }
  /* Event does not exist */
  return( FALSE );
}

#ifdef FEATURE_3GPP_CSFB
/*===========================================================================

FUNCTION rrc_sib_post_complete_for_csfb_sib_read

DESCRIPTION

  This function posts complete to CSP on CSFB SIB read completed.
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

===========================================================================*/

void rrc_sib_post_complete_for_csfb_sib_read
(
  rrc_sib_status_e_type status /* Status of the SIB event-success or failure*/
)
{
  rrc_cmd_type *cnf_ptr; /* For sending confirmation */
 
  if( (cnf_ptr = rrc_get_int_cmd_buf()) != NULL )
  {
    cnf_ptr->cmd_hdr.cmd_id = RRC_GET_SPECIFIC_SIBS_CNF;
    WRRC_MSG0_HIGH("Sending GET_SPECIFIC_SIBS_CNF cmd for CSFB");
    /* Copy the frequency and Scrambling code into the
     confirmation command. For GET_SPECIFIC_SIBS command, 
     check if it's the serving cell or neigbor cell. */
    cnf_ptr->cmd.get_sibs_cnf.freq = sib_events_ptr->srv_bcch.freq;
    cnf_ptr->cmd.get_sibs_cnf.scrambling_code = sib_events_ptr->srv_bcch.scr_code;
    cnf_ptr->cmd.get_sibs_cnf.procedure = sib_events_ptr->event[SIB_SRV_INDEX].req_proc;

    /*Boolean to indicate whether SIB read over OTA or through the 
           SYS_INFO_container*/
#ifdef FEATURE_3GPP_CSFB
    if(sib_events_ptr->event[SIB_SRV_INDEX].sib_scan_type == RRC_SIB_READ_FROM_SIB_CONTAINER)
    {
      cnf_ptr->cmd.get_sibs_cnf.is_sib_read_from_container = TRUE;
    }
    else
    {
      cnf_ptr->cmd.get_sibs_cnf.is_sib_read_from_container = FALSE;
    }
#endif
    
    cnf_ptr->cmd.get_sibs_cnf.status= status;
    /* Also Copy the frequency and Scrambling code into the
            SIB Database */
    /* No need to reset srv_bcch status here. We expect to get a
       "GET_ALL_SIBS" command for the same, so we should not setup bcch
    again on the same cell. */
    
    /* Copy all the necessary parameters for the confirmation command */
    if(sib_events_ptr->event[SIB_SRV_INDEX].sib_db_index_found)
    {
      cnf_ptr->cmd.get_sibs_cnf.cell_id = 
        sibdb.cell[sib_events_ptr->event[SIB_SRV_INDEX].sib_db_index].cell_id;
      cnf_ptr->cmd.get_sibs_cnf.plmn_id = 
         sibdb.cell[sib_events_ptr->event[SIB_SRV_INDEX].sib_db_index].plmn_id;
#ifdef FEATURE_FEMTO_CSG
      cnf_ptr->cmd.get_sibs_cnf.csg_id = 
         sibdb.cell[sib_events_ptr->event[SIB_SRV_INDEX].sib_db_index].csg_id;
      cnf_ptr->cmd.get_sibs_cnf.is_csg = 
         sibdb.cell[sib_events_ptr->event[SIB_SRV_INDEX].sib_db_index].is_csg;
#endif
    }

    /* SIB DB Index is also sent in the confirmation so that the procedure
    can use the index to read the SIBs if needed. Alternately the PLMN and
    Cell Id can also be used to read SIBs. */
    cnf_ptr->cmd.get_sibs_cnf.sib_index_for_cell = 
      sib_events_ptr->event[SIB_SRV_INDEX].sib_db_index;
    
    /* Now send the command */
    rrc_put_int_cmd( cnf_ptr );
  }

  /* after a failed event, reset BCCH action to false always.*/
  sib_events_ptr->srv_bcch.bcch_action = SIB_BCCH_NO_ACTION;
  sib_events_ptr->event[SIB_SRV_INDEX].event_active = FALSE;
  sib_events_ptr->event[SIB_SRV_INDEX].event_name = RRCSIB_GET_SPECIFIC_SIBS;
  sib_events_ptr->event[SIB_SRV_INDEX].sib_scan_type = RRC_SIB_NORMAL;
  sib_events_ptr->event[SIB_SRV_INDEX].sib_db_index_found = FALSE;

}

#endif
/*===========================================================================

FUNCTION  rrcsib_update_tenmin_sib_timer

DESCRIPTION

  This function will Implement the required ten minutes
  Timer for each of the valid sib index after MIB and SIB3
  of the cell is read.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
     
===========================================================================*/
void rrcsib_update_tenmin_sib_timer(void)
{
  uint8 count;

  for( count=0; count<=SIBDB_MAX_INDEX; count++ )
  {
    if(sibdb.cell[count].index_used == TRUE)
    {
      if(sibdb.cell[count].ten_min_count < MAX_CELL_INDEX_COUNT_TENMIN)
      {
        sibdb.cell[count].ten_min_count ++;
      }
      WRRC_MSG2_HIGH("6hr sib:Count value for index = %d is %d",count,sibdb.cell[count].ten_min_count);
    }
  }
  //Start 1min timer for getting ten minutes timer
  rrctmr_start_timer( RRCTMR_TENMIN_SIB_UPDATE_TIMER,
                        RRCSIB_TENMIN_UPDATE_TIMER );
}
/*===========================================================================

FUNCTION RRC_SIB_EVENT_CONTROLLER

DESCRIPTION

  This is the main function that controls the SIB processing module.
  The SIB processing module is event based, i.e it waits for events
  to be received from other RRC modules and does the required processing
  for each event. This function receives events from the RRC dispatcher
  and processes the events as needed.
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  BCCH logical channels may be set up and teared down. 
===========================================================================*/

void rrc_sib_event_controller
(
  rrc_cmd_type *cmd_ptr  /* Pointer to command */
)
{
  sib_events_index_e_type count; /* Counter used in loop */

  rrc_BCCH_FACH_Message *bcch_fach_msg_ptr;         /* Pointer to Downlink DCCH Msg */

#ifdef FEATURE_3GPP_CSFB
  rrc_csfb_sib_container_event_type csfb_sib_event;
#endif

#ifdef FEATURE_WCDMA_CSFB_CALL_OPT
  rrclog_wcdma_csfb_call_opt_event_type  event_payload;
#endif

  if (cmd_ptr->cmd_hdr.cmd_id != RRC_DOWNLINK_SDU_IND)
  {
    WRRC_MSG0_HIGH_OPT("function rrc_sib_event_controller is called");
  }  
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_SIB,RRC_INVALID_PROC_SUBSTATE,cmd_ptr->cmd_hdr.cmd_id);

  /* Switch on the received command and take appropriate
  action. */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_GET_SPECIFIC_SIBS_REQ:
      /* This is a request to get specific sibs for either
      the serving cell or a neighbor cell. */

      WRRC_MSG2_HIGH("RRC_GET_SPECIFIC_SIBS_REQ cmd received contents:for_serving_cell %d,scan type %d",
                cmd_ptr->cmd.get_specific_sibs_req.for_serv_cell,
                cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type);

      /* Call the function to do the necessary processing */
      if(((cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_WTOW_AUTO_BPLMN_SCAN)||
          (cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_WTOW_MANUAL_BPLMN_SCAN)) &&
         (rrc_csp_return_pending_type() != RRC_CSP_WTOW_BPLMN_DRX_SRCH_TIMER_PENDING)&&
          (WTOW_SEARCH_RESUME != rrc_csp_bplmn_srch_in_progress()))
      {                                                                      
        WRRC_MSG1_ERROR("ERR BPLMN not active.Trashing the GET_SPECIFIC_SIBS_REQ.Scan type %d",
                   cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type);
        break;      
      }
#ifdef FEATURE_DUAL_SIM
      /*Do a retain lock to get any pending requests*/
      wrm_retain_lock_for_primary();
      if(((cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_NORMAL)||
          (cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_MANUAL_SCAN)) &&
          (rrc_csp_waiting_to_release_lock()))
      {                                                                      
        WRRC_MSG1_HIGH("DSIM: Search Preempted, trash the GET_SPECIFIC_SIBS_REQ, Scan_type %d",
                   cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type);
        break;      
      }
#endif
#ifdef FEATURE_UPDATE_SIB7_IN_FACH
      if((rrc_get_state() == RRC_STATE_CELL_FACH)&&
         (cmd_ptr->cmd.get_specific_sibs_req.procedure == RRC_PROCEDURE_SIB)
        )
      {
        /*Lets check whether its a sib-7 event*/
        if((cmd_ptr->cmd.get_specific_sibs_req.for_serv_cell == TRUE)&&
           (cmd_ptr->cmd.get_specific_sibs_req.
                 sib_req[rrc_SIB7] == TRUE)
          )
        {
          if((rrcsib_is_sib_active() == TRUE)||
              (sibdb.active_cell_sibs_valid == FALSE)
            )
          {
            WRRC_MSG1_HIGH( "sib7:SIB event active,db cell valid=%d so Ignoring sib7 event", 
                                  sibdb.active_cell_sibs_valid);
            break;
          }
        }
      }
#endif

      if(cmd_ptr->cmd.get_specific_sibs_req.for_serv_cell == FALSE)
      {
        if(rrc_csp_is_reselection_cleared())
        { 
          /* Break as DL wk indication was received for this cell and reselection data (freq/scr)
                       have been cleared*/
          WRRC_MSG0_ERROR("Reselect info cleared. Trashing GET_SPECIFIC_SIBS_REQ.");
          rrcsib_post_process_failed_event( SIB_NGH_INDEX, TRUE,
                                          SIB_EVENT_FAILED_OTHER, 
                                          FALSE  
                                        );
          break;
        }
      }
#ifdef FEATURE_3GPP_CSFB
      if((cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type == RRC_SIB_READ_FROM_SIB_CONTAINER) &&
         (rrcsib_read_sibs_from_sys_info_container(cmd_ptr) == TRUE))
      {
        csfb_sib_event.sib_container_sts = RRC_SIB_CONTAINER_VALID;
        event_report_payload(EVENT_WCDMA_RRC_CSFB_SIB_CONTAINER, sizeof(csfb_sib_event),
                       (void *)&csfb_sib_event);

        WRRC_MSG0_HIGH("SIB reading successful from SYS_INFO_CONTAINER");

#ifdef FEATURE_WCDMA_CSFB_CALL_OPT
       if(WCDMA_RRC_IDLE_FEATURES(RRC_CSFB_CALL_TIMELINE_OPT) && rrc_csfb_call_status)
       {
         /* For CSFB call, if SIB container read is success post GET_SPEC_SIBS_CNF here itself */
         is_sib_cont_present = TRUE;
         rrc_sib_post_complete_for_csfb_sib_read(SIB_EVENT_SUCCEEDED);

         event_payload.status = RRC_BCCH_SETUP_SKIPPED;   //Logging the event as the BCCH setup is skipped.
         event_report_payload(EVENT_WCDMA_CSFB_CALL_OPT, sizeof(event_payload),(void *)&event_payload);
       }
#endif
      }
      else
      {
        register_new_sib_event( cmd_ptr );  
      }
#else
      register_new_sib_event( cmd_ptr );
#endif
      break;

    case RRC_UPDATE_SIBS_IND:
      /* We have received indication that the SIBs for
      the cell we have camped on need to be read again.
      Atleast one SIB would have changed. We need to 
      read the MIB again and check the value tags. 
      This is usually triggered by a Paging Type 1 message
      with "BCCH Modification Info IE or by System Info
      Change Indication message. Check for duplicates since
      we can receive several of these messages while processing
      one UPDATE_SIBS command. */
      if((rrcsib_is_current_event_due_to_sfn_drift() == TRUE)&&
          (cmd_ptr->cmd.update_sibs_ind.cause == RRCSIB_SIX_HR_TMR_EXPIRY)
        )
      {
        sib_events_ptr->update_sibs_req_proc = RRC_PROCEDURE_SIB;
        WRRC_MSG0_HIGH("Overwriting Update_sibs due to SFN drift with six hrs sib");
      }
      else if((cmd_ptr->cmd.update_sibs_ind.real_sib_mod == FALSE)&&
          (rrcsib_is_sib_active() == TRUE)
        )
      {
        WRRC_MSG0_HIGH("Ignored UPDATE_SIBS cmd as a sib event is active");
      }
      /* If real_sib_mod is TRUE, UPDATE_SIBS event should be allowed. 
       * This is needed for reading deferred SIBs in Idle Mode 
       */
      else if((cmd_ptr->cmd.update_sibs_ind.real_sib_mod == FALSE) &&
              (rrc_ccm_get_curr_camping_status(RRC_PROCEDURE_SIB) == RRC_NOT_CAMPED_ON)
             )
      {
        WRRC_MSG0_HIGH("Ignored UPDATE_SIBS cmd as there is no active cell");
      }
      else
      {
        if( (rrcsib_check_for_duplicate_update_sibs_cmd(cmd_ptr->cmd.update_sibs_ind.mib_value_tag)) == TRUE )
        {
          WRRC_MSG0_HIGH("Ignored duplicate UPDATE_SIBS cmd");
        }
        /* Check with LLC is OC status is set for DCH.If so,we are in the middle
        of reconfig and L1 will not be able to handle the PCCPCH setup request.
        Ignore the Update SIBs Ind.If the reconfig is successful we would go to
        DCH anyway.If it fails we would go thru cell selection process and go back to 
        FACH.The only possible pitfall here is if the modification time is out in the
        future and we fail the reconfig,do the cell selection and come back on the same
        cell before the modification time is hit.This is a very narrow window */   
        else if((OC_SET_FOR_CELL_DCH == rrcllc_get_ordered_config_status())||
                    (rrc_get_state() == RRC_STATE_CELL_DCH)
                  )
        {
          WRRC_MSG0_HIGH("Ignoring UPDATE_SIBS_IND since OC set for DCH");
        }
        else if(rrc_get_state() == RRC_STATE_CONNECTING)
        {
          WRRC_MSG0_HIGH("Ignoring UPDATE_SIBS_IND since state is CONNECTING");
        }
        else if((rrc_get_state() == RRC_STATE_DISCONNECTED) && (rrcrce_get_rce_sub_state() != RRCRCE_INITIAL))
        {
          WRRC_MSG0_HIGH("Ignoring UPDATE_SIBS_IND since RCE is getting active");
        }

        else
        {
        
          /* Check to see if 3G->3G BPLMN is active.If it is active,we have to 
             suspend it before we can send the PCCPCH setup message to L1 */
          if(rrcsib_cmd_store.pending_flag == TRUE)
          {
            if(cmd_ptr->cmd.update_sibs_ind.real_sib_mod ==FALSE)
            {
              WRRC_MSG0_HIGH("Do not overwrite UPDATE_SIBS_IND since this is an Internal event");
            }
            else
            {
              /* Save the UPDATE_SIBS command in the command store and set pending flag */
              WRRC_MSG0_HIGH("Overwriting UPDATE_SIBS cmd to process after BPLMN suspension/Update sibs tmr expiry");
              WCDMA_MEMCPY(&rrcsib_cmd_store.saved_cmd,
                           sizeof(rrc_cmd_type),
                           cmd_ptr,
                           sizeof(rrc_cmd_int_type));
              update_sibs_event_pending = TRUE;
            }
          }
          else if(WTOW_SEARCH_STOP != rrc_csp_bplmn_srch_in_progress())
          {
            rrcsib_cmd_store.pending_flag = TRUE;
            update_sibs_event_pending = TRUE;
  
            rrc_csp_send_bplmn_suspend_req(RRC_PROCEDURE_SIB,BPLMN_SUSPEND_REASON_BCCH_MODIFICATION);
  
            /* Save the UPDATE_SIBS command in the command store and set pending flag */
            WRRC_MSG0_HIGH("BPLMN active when RRC_UPDATE_SIBS_IND is received .Buffering to process after BPLMN suspension");
            WCDMA_MEMCPY(&rrcsib_cmd_store.saved_cmd,
                         sizeof(rrc_cmd_type),
                         cmd_ptr,
                         sizeof(rrc_cmd_int_type));
          }
          else
          {
            /* Save necessary information for this event */
            sib_events_ptr->mib_value_tag_from_command =
              cmd_ptr->cmd.update_sibs_ind.mib_value_tag;
    
            sib_events_ptr->update_sibs_req_proc = cmd_ptr->cmd.update_sibs_ind.procedure;
  
            sib_events_ptr->update_sibs_cause = cmd_ptr->cmd.update_sibs_ind.cause;

            sib_events_ptr->update_sibs_real_sib_mod = cmd_ptr->cmd.update_sibs_ind.real_sib_mod;
            if(rrcllc_get_n_bcch_setup_status() == TRUE)
            {
              rrcsib_cmd_store.pending_flag = TRUE;
              update_sibs_event_pending = TRUE;
              WRRC_MSG0_HIGH("Buffering RRC_UPDATE_SIBS_IND command to process after N-BCCH is torn down.");
              WCDMA_MEMCPY(&rrcsib_cmd_store.saved_cmd,
                           sizeof(rrc_cmd_type),
                           cmd_ptr,
                           sizeof(rrc_cmd_int_type));

              if(rrctmr_get_remaining_time(RRC_SIB_CHANGE_GUARD_TIMER) == 0)
              {
               /*Start 1sec timer for safe guarding processing of sib_change_ind against BPLMN*/
                rrctmr_start_timer( RRC_SIB_CHANGE_GUARD_TIMER,RRCSIB_PENDING_UPDATE_SIB_TIMER_VALUE );
              }
            } 
            else
            {  
              WRRC_MSG0_HIGH( "RRC_UPDATE_SIBS_IND cmd received");
              /* Call the function to do the necessary processing */
              register_new_sib_event( cmd_ptr );
            }
          }
        }
      }
      break;

    case RRC_CHANNEL_CONFIG_CNF:
      /* We've received confirmation from LLC for the required
      BCCH setup. Make sure the setup succeeded. */

      /* Check if any event is active. If so cancel it */
      for( count = SIB_SRV_INDEX; count < MAX_SIB_EVENTS_INDEX; count++ )
      {
        if( sib_events_ptr->event[count].event_active == TRUE )
        {
          if (sib_events_ptr->event[count].transaction_id == 
              cmd_ptr->cmd.chan_config_cnf.transaction_id)
          {
            WRRC_MSG1_HIGH("Transaction Id %d for chan cfg req recvd",
                     cmd_ptr->cmd.chan_config_cnf.transaction_id);

            if( cmd_ptr->cmd.chan_config_cnf.chan_cfg_status == RRCLLC_CHAN_CFG_SUCCESS )
            {
              MSG_MED( "Received cnf of BCCH setup", 0, 0, 0);
              /* Check which sib_event is waiting for a LLC confirmation */
              if( sib_events_ptr->srv_bcch.bcch_action == SIB_BCCH_SETUP_PENDING )
              {
                sib_events_ptr->srv_bcch.bcch_action = SIB_BCCH_SETUP_COMPLETE;
              }
              else if( sib_events_ptr->ngh_bcch.bcch_action == SIB_BCCH_SETUP_PENDING)
              {
                sib_events_ptr->ngh_bcch.bcch_action = SIB_BCCH_SETUP_COMPLETE;
              }
              else
              {
                WRRC_MSG0_HIGH("Ignoring CNF for BCCH teardown");
              }

              if((SIB_SRV_INDEX == count) && (RRCSIB_UPDATE_SIBS == sib_events_ptr->event[count].event_name) &&
                 (RRCSIB_SFN_UPDATE == sib_events_ptr->update_sibs_cause))
              {
                WRRC_MSG0_ERROR("SNF-Update-Event Cnf, aborting event");
                
                rrcsib_post_process_failed_event
                  ( count, /* Event Index                 */
                    TRUE,  /* Confirm Command is required */
                    SIB_EVENT_FAILED_OTHER, /* Failure reason */
                    TRUE   /* Tear down S_BCCH if it's up */
                  );
                sib_events_ptr->update_sibs_real_sib_mod = FALSE; /* doesn't do anything now */
              }
#ifdef FEATURE_3GPP_CSFB
              if((RRCSIB_GET_SPECIFIC_SIBS == sib_events_ptr->event[count].event_name)&&
              (sib_events_ptr->event[count].sib_scan_type == RRC_SIB_READ_FROM_SIB_CONTAINER))
              {
                is_sib_cont_present = TRUE;
                rrc_sib_post_complete_for_csfb_sib_read(SIB_EVENT_SUCCEEDED);
              }
#endif
              if((WCDMA_RRC_IDLE_FEATURES(RRC_SIB7_SKIP_READ))&&(RRCSIB_GET_SPECIFIC_SIBS == sib_events_ptr->event[count].event_name)&&
                 (sib_events_ptr->event[count].int_event.int_event_type == RRCSIB_INT_SIB7_EVENT)&&
                 ((rrc_get_state() == RRC_STATE_CELL_PCH)||(rrc_get_state() == RRC_STATE_URA_PCH)))
              {
                if(sib7info.sib_ext_proc_update_req == TRUE)
                {
                  rrc_cmd_type  *ptr = NULL;
                  ptr = rrc_get_int_cmd_buf();
                  WRRC_MSG0_HIGH("Sending RRC_SIB7_UPDATE_STATUS_IND");
                  ptr->cmd_hdr.cmd_id = RRC_SIB7_UPDATE_STATUS_IND;
                  ptr->cmd.sib7_update_status.status = SUCCESS;
                  ptr->cmd.sib7_update_status.procedure = sib7info.req_proc;
                  rrc_put_int_cmd( ptr );
                }
              }
            }
            else
            {
              WRRC_MSG0_ERROR(" BCCH setup failed!");
              /* Handle the BCCH setup failure */
              if((sib_events_ptr->update_sibs_real_sib_mod == FALSE) && 
                    (RRCSIB_UPDATE_SIBS == sib_events_ptr->event[count].event_name) &&
                    (SIB_SRV_INDEX == count))
              {
                WRRC_MSG0_HIGH("BCCH setup failed for L1 drift or six hrs sib req, dont trigger OOS/Cell selection");
                rrcsib_post_process_failed_event
                  ( count, /* Event Index                 */
                    FALSE,  /* Confirm Command is required */
                    SIB_EVENT_FAILED_OTHER, /* Failure reason */
                    FALSE   /* Do not tear down S_BCCH */
                  );
              }
              else
#ifdef FEATURE_3GPP_CSFB
              if((RRCSIB_GET_SPECIFIC_SIBS == sib_events_ptr->event[count].event_name)&&
                 (sib_events_ptr->event[count].sib_scan_type == RRC_SIB_READ_FROM_SIB_CONTAINER))
              {
                rrc_sib_post_complete_for_csfb_sib_read(SIB_EVENT_FAILED_LOWER_LAYERS);
              }
              else
#endif
              {
                rrcsib_process_bcch_setup_failure();
              }
            }
          }
          else
          {
            WRRC_MSG2_HIGH("Transaction Id %d for chan cfg req did not match with %d",
            sib_events_ptr->event[count].transaction_id, cmd_ptr->cmd.chan_config_cnf.transaction_id);
          }
        }
      }
      /* Note : This is not complete yet! we also need to check for
      SIB_BCCH_RELEASE_PENDING in case we have to release BCCH's.
      This is not implemented right now. */

      break;

    case RRC_DOWNLINK_SDU_IND:
      /* Process the message if it is not NULL */
      if(cmd_ptr->cmd.downlink_sdu_ind.decoded_msg != NULL)
      {
        if(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == rrc_BCCH_BCH_Message_PDU)
        {
          /* We've received a System information message. Check
          the message type and call the appropriate function. */
          process_bcch_message( cmd_ptr );
        }
        else  //this is a BCCH_FACH message.. process only sib-mod-ind, we don't support
              //system-info on fach
        {
          /*get the bcch_fach_msg_ptr for sib-change as this message is already decoded*/
          bcch_fach_msg_ptr =  (rrc_BCCH_FACH_Message *) cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;
  
          //we only support sib modification ind.. other messages are not supported
          if(RRCGET_SIB_TYPE_AND_TAG((&bcch_fach_msg_ptr->message))
                  == T_rrc_BCCH_FACH_MessageType_systemInformationChangeIndication)
          {  
           /*Compare the value of IE "MIB value tag" in the IE "BCCH modification info" 
             with the value tag stored for the master information block in SIB data base 
             for serving cell.If both Differ only then process SysInfoChange message other 
             wise UE has current system info so Ignore this sysInfo.This check is made to 
             make UE compliant to 25.331 Standard section 8.1.1.7.3 */
#ifdef FEATURE_WCDMA_ETWS
            if ((RRC_MSG_COMMON_BITMASK_IE_PTR((&bcch_fach_msg_ptr->message.u.systemInformationChangeIndication),
                                               rrc_,laterNonCriticalExtensions)) && 
                (RRC_MSG_COMMON_BITMASK_IE_PTR((&bcch_fach_msg_ptr->message.u.systemInformationChangeIndication.
                                                laterNonCriticalExtensions),rrc_,v860NonCriticalExtensions)) && 
                (RRC_MSG_COMMON_BITMASK_IE_PTR((&bcch_fach_msg_ptr->message.u.systemInformationChangeIndication.
                                           laterNonCriticalExtensions.v860NonCriticalExtensions.
                                           systemInformationChangeIndication_v860ext),rrc_,etws_Information)))
            {
              WRRC_MSG0_HIGH(" SYS_INFO_CHNG_IND: ETWS info present");
              rrcbmc_send_etws_information_ind_message(bcch_fach_msg_ptr->message.u.systemInformationChangeIndication.
                                                       laterNonCriticalExtensions.v860NonCriticalExtensions.
                                                       systemInformationChangeIndication_v860ext.etws_Information,NULL);
            }
#endif
  
            if((rrc_get_serving_cell_mib_value_tag()) != (bcch_fach_msg_ptr->message.u.systemInformationChangeIndication.
                  bcch_ModificationInfo.mib_ValueTag))
            {
              /*The Handling of BCCH Mod Time is implemented in rrcpg1, So call the 
                the API provided by rrcpg1 for handling BCCH Mod Info I.E*/
  
              /* Check if the modification time is present here*/
              if(RRC_MSG_COMMON_BITMASK_IE_PTR(
                    (&bcch_fach_msg_ptr->message.u.systemInformationChangeIndication.bcch_ModificationInfo),
                    rrc_,
                    bcch_ModificationTime)
              )
              {
                rrcpg1_process_bcch_mod_info_msg_ie(
                                                     (uint8)bcch_fach_msg_ptr->message.u.systemInformationChangeIndication.
                                                       bcch_ModificationInfo.mib_ValueTag,
                                                     (uint16)bcch_fach_msg_ptr->message.u.systemInformationChangeIndication.
                                                       bcch_ModificationInfo.bcch_ModificationTime,
                                                     TRUE
                                                    );
              }
              else
              {
                rrcpg1_process_bcch_mod_info_msg_ie(
                                                     (uint8)bcch_fach_msg_ptr->message.u.systemInformationChangeIndication.
                                                       bcch_ModificationInfo.mib_ValueTag,
                                                     RRCPG1_INVALID_BCCH_MOD_TIMER_VAL,
                                                     FALSE
                                                    );
              }
            }
            else
            {
              WRRC_MSG0_HIGH("Ignoring SysInfoChange message since Serving cell's MIB value tage is current");
            }
          }//only sibmodificationind is supported at present
        }
      }
      else
      {
        WRRC_MSG0_ERROR("DOWNLINK_SDU_IND decoded_msg is NULL");
      }
      break;

    case RRC_STATE_CHANGE_IND:

      /* A State Change notification has been received  */
      rrcsib_process_state_change_ind( cmd_ptr );
      break;

    case RRC_SIB_WAIT_TIMER_EXPIRED_IND:

      /* The SIB wait timer has expired. That means we have waited
      for the maximum amount of time for getting SIBs from a cell.
      If any SIB event is active, it needs to be cancelled and a failure
      notice sent to the procedure that requested the event.  */
      rrcsib_process_sib_wait_timer_expiration();
      break;

#ifdef FEATURE_DUAL_SIM
    case RRC_SIB_STOP_PARTITION_TMR_EXPIRED_IND:
    case RRC_SIB_EMPTY_MASK_TMR_EXPIRED_IND:
    case RRC_SIB_MASK_INVERT_TMR_EXPIRED_IND:
      if( sib_events_ptr->event[SIB_SRV_INDEX].event_active == TRUE )
      {
        count = SIB_SRV_INDEX;
      }
      else if(sib_events_ptr->event[SIB_NGH_INDEX].event_active == TRUE)
      {
        count = SIB_NGH_INDEX;
      }
      else
      {
        return;
      }

      /*Ignore timers for SIB7 events. Added for safety*/
      if(sib_events_ptr->event[count].int_event.int_event_type == RRCSIB_INT_SIB7_EVENT)
      {
        return;
      }
      if(cmd_ptr->cmd_hdr.cmd_id == RRC_SIB_STOP_PARTITION_TMR_EXPIRED_IND)
      {
        rrcsib_stop_partition = TRUE;
        rrcsib_update_sib_sleep_schedule_table(count);
        rrctmr_stop_timer(RRCTMR_SIB_MASK_INVERT_TIMER);
      }

      else if((cmd_ptr->cmd_hdr.cmd_id == RRC_SIB_MASK_INVERT_TMR_EXPIRED_IND)&&(!rrcsib_stop_partition))
      {
        rrcsib_pattern_invert = !rrcsib_pattern_invert;
        rrcsib_send_empty_table(count,TRUE);
        rrctmr_start_timer(RRCTMR_SIB_EMPTY_MASK_TIMER,RRCSIB_EMPTY_FRAMES );
        rrctmr_start_timer(RRCTMR_SIB_MASK_INVERT_TIMER,
                            MAX(sib_events_ptr->scheduling_max_rept_rate,RRCSIB_MIN_MASK_INV_TIMER)*RRCSIB_MASK_INVERT_FACTOR + 
                            RRCSIB_TRANS_BUFFER + RRCSIB_EMPTY_FRAMES );
      }
      else
      {
        if(((!sib_events_ptr->event[count].sched_present_for_sibs[rrc_SB1])||
           ((sib_events_ptr->event[count].sched_present_for_sibs[rrc_SB1])&&
            (sib_events_ptr->event[count].tempholder.sb1_decoded))
           )&&
          ((!sib_events_ptr->event[count].sched_present_for_sibs[rrc_SB2])||
           ((sib_events_ptr->event[count].sched_present_for_sibs[rrc_SB2])&&
           (sib_events_ptr->event[count].tempholder.sb2_decoded))
           )
          )
         {
            rrcsib_update_sib_sleep_schedule_table(count);
         }
         else
         {
          rrcsib_send_empty_table(count,FALSE);
         }
      }
      break;
#endif

    case RRC_SIB7_EXP_TIMER_EXPIRED_IND:

      /* The SIB7 expiration timer has expired. That means we
      have to read SIB7 again for the serving cell. */
      rrcsib_process_sib7_exp_timer_expiration();
      break;

    case RRC_BPLMN_SUSPEND_CNF:
      
      WRRC_MSG1_HIGH("WTOW SIB Rx:RRC_BPLMN_SUSPEND_CNF with status %d",cmd_ptr->cmd.rrc_csp_bplmn_suspend_cnf.status);

      if(cmd_ptr->cmd.rrc_csp_bplmn_suspend_cnf.status == SUCCESS)
      {
        /* Check to see if there is a buffered cmd.Currently we support only UPDATE_SIBS_IND */
        if( (rrcsib_cmd_store.pending_flag == TRUE) &&
            (rrcsib_cmd_store.saved_cmd.cmd_hdr.cmd_id == RRC_UPDATE_SIBS_IND) )
        {
          WRRC_MSG0_HIGH( "Processing buffered RRC_UPDATE_SIBS_IND cmd after BPLMN suspension");

          /* Save necessary information for this event */
          sib_events_ptr->mib_value_tag_from_command =
          rrcsib_cmd_store.saved_cmd.cmd.update_sibs_ind.mib_value_tag;

          sib_events_ptr->update_sibs_req_proc =
          rrcsib_cmd_store.saved_cmd.cmd.update_sibs_ind.procedure;

          sib_events_ptr->update_sibs_cause = 
          rrcsib_cmd_store.saved_cmd.cmd.update_sibs_ind.cause;

          sib_events_ptr->update_sibs_real_sib_mod = 
		  rrcsib_cmd_store.saved_cmd.cmd.update_sibs_ind.real_sib_mod;
          /* Call the function to do the necessary processing */
          register_new_sib_event(&rrcsib_cmd_store.saved_cmd);
        }
        else
        {
          WRRC_MSG0_ERROR("No pending command in SIB cmd store after BPLMN_SUSPEND_CNF");
        }
      }
      else
      {
        WRRC_MSG0_ERROR("ERR CSP sent SUSPEND_CNF with status FAILURE");
      }

    /* Set the pending flag to FALSE */
      update_sibs_event_pending = FALSE;

    /* Clear the command store */
      rrcsib_cmd_store.pending_flag = FALSE;
      break;

      case RRC_SIX_HRS_SIB_TIMER_UPDATE_IND:
        rrcsib_update_sixhr_sib_timer();
        break;

      case RRC_TEN_MIN_SIB_TIMER_UPDATE_IND:
        rrcsib_update_tenmin_sib_timer();
        break;

      case RRC_SIB_CHANGE_GUARD_TIMER_EXPIRED_IND:
          sib_change_safe_guard = FALSE;
          /* Check if there is any pending event for the old serving
                cell an if so, process that event. */
          if((update_sibs_event_pending == TRUE) &&
             (rrcsib_cmd_store.pending_flag == TRUE) &&
             (rrcsib_cmd_store.saved_cmd.cmd_hdr.cmd_id == RRC_UPDATE_SIBS_IND)&&
             (sibdb.active_cell_index != SIBDB_INVALID_INDEX)&&
             (sibdb.active_cell_sibs_valid == TRUE)
            )
          {
            /* Post a command to ourselves for updating the SIBs */
            rrcsib_send_command_for_saved_update_sibs_event();
            WRRC_MSG0_HIGH("Saved UPDATE_SIBS event activated");

            update_sibs_event_pending = FALSE;
            rrcsib_cmd_store.pending_flag = FALSE;
          }
          break;

    default:
      WRRC_MSG1_ERROR("Invalid SIB event %x", cmd_ptr->cmd_hdr.cmd_id);
      break;
  }/* end Switch */
}/* end function rrc_sib_event_controller */

/*===========================================================================

FUNCTION RRC_SIB_PROC_EVENTS_INIT

DESCRIPTION

  This function initializes the SIB events to their
  initial state. This may be called at powerup as well as
  when the procedure needs to be re-initialized such as
  due to Factory Test Mode or Low Power Mode.

DEPENDENCIES

  None.

RETURN VALUE

  None.
  
SIDE EFFECTS

  None.

===========================================================================*/
void rrc_sib_proc_events_init( void )
{
  sib_events_index_e_type count; /* Counter used in loops */
  
  /* First clear the SIB events database */
  if(sib_events_ptr != NULL)
  {
    for( count = SIB_SRV_INDEX; count < MAX_SIB_EVENTS_INDEX; count++ )
    {
      sib_events_ptr->event[count].event_active = FALSE; /* No SIB events active now */
      sib_events_ptr->event[count].sib_scan_type = RRC_SIB_NORMAL;
    }
    /* Initialize the BCCH parameters */
    sib_events_ptr->srv_bcch.bcch_action = SIB_BCCH_NO_ACTION;
    sib_events_ptr->ngh_bcch.bcch_action = SIB_BCCH_NO_ACTION;
  }
  /* Note that there is no need to initialize any other SIB events database
  element here since they will be initialized when an event becomes active */

  /* Indicate there is no RRCSIB_UPDATE_SIBS event pending at this
  time. This is used only after we select a serving cell. */
  update_sibs_event_pending = FALSE;

  /* Clear the cmd store pending flag */
  WRRC_MSG0_HIGH("Setting SIB cmd store pending flag to FALSE");
  rrcsib_cmd_store.pending_flag = FALSE;

  /* Indicate there is no SIB7 expiration info stored at present */
  sib7info.sib_ext_proc_update_req = FALSE;
  sib7info.sib_timer_update_req = FALSE;

  /* Then initialize the SIB database. If this is a power-up
  init, we can just initialize all the variables. Otherwise we
  may have to free any used SIB database pointers to avoid memory
  leaks. */
  if( rrcsib_powerup_init_done == TRUE )
  {
    /* This is not a power-up init, so the procedue may have been
    active before this. Stop any timers in case they are running. */
    rrctmr_stop_timer( RRCTMR_SIB_WAIT_TIMER );
    rrctmr_stop_timer( RRCTMR_SIB7_EXP_TIMER ); 

#ifdef FEATURE_DUAL_SIM
    rrctmr_stop_timer( RRCTMR_SIB_STOP_PARTITION_TIMER );
    rrctmr_stop_timer( RRCTMR_SIB_MASK_INVERT_TIMER );
    rrctmr_stop_timer( RRCTMR_SIB_EMPTY_MASK_TIMER );
#endif

    /* For safety, in case the tempholer pointers are not freed, free them
    now */
    for( count = SIB_SRV_INDEX; count < MAX_SIB_EVENTS_INDEX; count++ )
    {
      rrcsib_free_tsib(count);
    }
  }
/* Global Flag which prevents BPLMN(If BPLMN request is received when UPDATE_SIBs
  was on going) to start till all the SIB_Change  commands are processed by procedures*/
  sib_change_safe_guard = FALSE;
#ifdef FEATURE_RRC_NO_MIB_FOUND_BAR_CELL
  no_mib_bar_cell_count = 0;
#endif

  /*Initialize tsib optimization data base*/
  rrcsib_clear_tsib_optimization_db();

}/* end function rrc_sib_proc_events_init */

/*===========================================================================

FUNCTION RRC_SIB_DATABASE_INIT

DESCRIPTION

  This function initializes the SIB database to it's
  initial state. This may be called at powerup as well as
  when the procedure needs to be re-initialized such as
  due to Factory Test Mode or Low Power Mode.

DEPENDENCIES

  None.

RETURN VALUE

  None.
  
SIDE EFFECTS

  None.

===========================================================================*/
static void rrc_sib_database_init( void )
{
  uint32 count; /* Counter used in loops */
  
  /* If this is a power-up
  init, we can just initialize all the variables. Otherwise we
  may have to free any used SIB database pointers to avoid memory
  leaks. */
  if( rrcsib_powerup_init_done == FALSE )
  {
    transaction_id = 0;
    rrc_sibdb_init();
  }
  else
  {
    /* For each SIBDB index, if the index is used, then
    free each sib pointer in the index. */
    for( count=0; count<=SIBDB_MAX_INDEX; count++ )
    {
      if(sibdb.cell[count].index_used == TRUE)
      {
        WRRC_MSG1_HIGH("SIB Heap: Freeing SIB Idx %d sibdbinit", count);
        rrc_sibdb_cell_header_init(count);/* This frees all the SIBs */
      }
    }
    /* Now initialize SIBDB variables */
    rrc_sibdb_init();
  }
}/* end function rrc_sib_database_init */

/*===========================================================================

FUNCTION RRC_SIB_PROC_INIT

DESCRIPTION

  This function initializes the SIB processing procedure.
  This function needs to be called once only, at powerup.
  The SIB database and the SIB events database are cleared
  by this function.

DEPENDENCIES

  None.

RETURN VALUE

  None.
  
SIDE EFFECTS

  None.

===========================================================================*/
void rrc_sib_proc_init( void )
{


  sib_events_ptr = NULL;

  /* Initialize SIB events */
  rrc_sib_proc_events_init();

  /* Initialize the database */
  rrc_sib_database_init();


  /* Also send registration to state change manager. We need to know when
  RRC goes to Idle mode (disconnected state). The BCCH will be cleared at
  that time so we need to clear our internal variables that keep track of
  the BCCH status. */
  rrcscmgr_register_for_scn( RRC_PROCEDURE_SIB,  /* Procedure name */
                             RRC_STATE_WILDCARD, /* From State     */
                             RRC_STATE_DISCONNECTED /* To State    */
                           );

  /* We also need to know when we move from any other state to 
  CELL_DCH state. In CELL_DCH State it is not possible to have
  the BCCH-BCH pipe up and LLC will bring down the pipe.
  We need to clear our internal variables */
  rrcscmgr_register_for_scn( RRC_PROCEDURE_SIB,  /* Procedure name */
                             RRC_STATE_WILDCARD, /* From State     */
                             RRC_STATE_CELL_DCH  /* To State       */
                           );

  /* We also need to know when we move from any other state to 
  CELL_FACH state. Only in CELL_FACH State we will perform expiration
  timer based update of SIBs. This is done only for SIBs with
  expiration timers. */

#if  defined(FEATURE_WCDMA_DEFERRED_SIB11_12_READING)
  rrcscmgr_register_for_scn( RRC_PROCEDURE_SIB,   /* Procedure name */ 
                             RRC_STATE_WILDCARD,  /* From State     */     
                             RRC_STATE_CELL_FACH  /* To State       */       
                           );
#endif

#ifdef FEATURE_UPDATE_SIB7_IN_FACH

  /* And we register for state change from FACH to any other
  state. This is to stop timer based update of SIBs with expiration
  timers. */
  rrcscmgr_register_for_scn( RRC_PROCEDURE_SIB,   /* Procedure name */ 
                             RRC_STATE_CELL_FACH, /* From State     */
                             RRC_STATE_WILDCARD   /* To State       */
                           );
#endif /* FEATURE_RRC_SIB7_UPDATE_IN_FACH */

#ifndef FEATURE_UPDATE_SIB7_IN_FACH
   /* And we register for state change from DCH to URA PCH
      state. This is to trigger update SIBs for deferred SIBs */
  rrcscmgr_register_for_scn( RRC_PROCEDURE_SIB,   /* Procedure name */ 
                             RRC_STATE_WILDCARD, /* From State	  */
                             RRC_STATE_URA_PCH   /* To State		 */
                           );
    /* And we register for state change from DCH to CELL PCH
      state. This is to trigger update SIBs for deferred SIBs */
  rrcscmgr_register_for_scn( RRC_PROCEDURE_SIB,   /* Procedure name */ 
                             RRC_STATE_WILDCARD, /* From State		*/
                             RRC_STATE_CELL_PCH /* To State	   */
                           );
#endif /* FEATURE_UPDATE_SIB7_IN_FACH */

  //Start 10min timer for getting six hours timer
  rrctmr_start_timer( RRCTMR_SIXHR_SIB_UPDATE_TIMER,
                        RRCSIB_SIX_HR_SIB_UPDATE_TIMER );

  rrctmr_start_timer( RRCTMR_TENMIN_SIB_UPDATE_TIMER,
                        RRCSIB_TENMIN_UPDATE_TIMER );
  /* Note that power-up initialization has been done */
  rrcsib_powerup_init_done = TRUE;

}/* end function rrc_sib_proc_init */



/*===========================================================================

FUNCTION RRCSIB_CHANGE_SERVING_CELL_STATUS

DESCRIPTION

  This function is used to give information about any change
  in the serving cell due to cell selection or cell reselection.
  The calling procedure needs to indicate whether a new cell
  has been successfully selected. If so, it should give the PLMN
  and cell ids for the cell. Only after all the required
  sibs for that cell have been read, this function should be
  called.
  
  This function must be used ONLY by the cell selection and
  reselection procedure.
  
DEPENDENCIES

  None.

RETURN VALUE

  SUCCESS if the serving cell status was changed correctly.
  FAILURE if the PLMN and cell id can't be found in the database.
  
SIDE EFFECTS

  None.

===========================================================================*/
uecomdef_status_e_type rrcsib_change_serving_cell_status
(
  boolean new_cell_selection_succeeded,
            /* set to TRUE if a new cell has been selected through
            cell selection or reselection. Set to FALSE if a new
            cell reselection failed. */
  boolean existing_serving_cell_valid,
            /* Set to TRUE if we can go back to the existing serving
            cell. This is only used if new_cell_selection_succeeded
            is set to FALSE. Set to FALSE if the existing cell is
            no longer valid and the new cell selection failed. */
  rrc_plmn_identity_type plmn,    
           /* The PLMN Id for the cell */ 
  rrc_int_cell_id_type cell       
           /* The cell id for the cell */
)
{
  uint32 temp_index;
  rrc_SysInfoType1 *sib1 = NULL;
  uint32 count = 0;

  if( new_cell_selection_succeeded == TRUE )
  {
    /* A new serving cell has been selected. Store the given
    index. When other modules want to read sibs from the serving
    cell, this index is used to find the required sibs. */
    if( ( temp_index = rrc_sibdb_find_cell_index( plmn, cell )) !=
        SIBDB_INVALID_INDEX )
    {
      WRRC_MSG1_HIGH("Serving Cell Index is now %d", temp_index);
      for(count = 0; count <SIBDB_INVALID_INDEX; count++)
      {
        if((count != temp_index) && (sibdb.cell[count].index_used == TRUE))
        {
          WRRC_MSG1_HIGH("Free ASN.1 decoded sib's for sibdb idx= %d", count);
          rrc_sibdb_free_all_decodedsibs_for_a_cell(count);
        }
      }

      sibdb.active_cell_index = temp_index;
      sibdb.active_cell_sibs_valid = TRUE;

      /* Whenever we go to a new cell we need to check if the
        connected mode timers and constants need to be updated.
      */
      if( (rrcumi_check_ue_timer_constants_update_status(TRUE))
          == RRC_CONN_TIMER_AND_CONSTANTS_UPDATE_REQ )
      {
        sib1 = rrc_sibdb_return_sib_for_db_index(
          rrc_SIB1,temp_index);
        if(sib1 != NULL)
        {
          (void)rrc_update_ue_conn_timers_and_constants
          (          
            (RRC_MSG_COMMON_BITMASK_IE_PTR(sib1, rrc_SysInfoType1_,ue_ConnTimersAndConstants)) ? 
              &(sib1->ue_ConnTimersAndConstants):
              NULL,            
            (RRC_MSG_COMMON_BITMASK_IE_PTR(sib1, rrc_SysInfoType1_,v3a0NonCriticalExtensions))? 
              &(sib1->v3a0NonCriticalExtensions.
              sysInfoType1_v3a0ext.ue_ConnTimersAndConstants_v3a0ext):
              NULL,
             ((sib1->m.v3a0NonCriticalExtensionsPresent) &&
              (sib1->v3a0NonCriticalExtensions.m.v860NonCriticalExtentionsPresent))? 
              &(sib1->v3a0NonCriticalExtensions.v860NonCriticalExtentions.
              sysInfoType1_v860ext.ue_ConnTimersAndConstants):
              NULL
          );
        }
        else
        {
          WRRC_MSG0_ERROR("SIB1 for serving cell is NULL!");
          return( FAILURE );
        }
      }
      else
      {
        sib1 = rrc_sibdb_return_sib_for_db_index(
          rrc_SIB1,temp_index);
        if(sib1 != NULL)
        {
          (void)rrc_update_ue_conn_timers_and_constants(NULL, NULL, 
             ((sib1->m.v3a0NonCriticalExtensionsPresent) &&
              (sib1->v3a0NonCriticalExtensions.m.v860NonCriticalExtentionsPresent))? 
              &(sib1->v3a0NonCriticalExtensions.v860NonCriticalExtentions.
              sysInfoType1_v860ext.ue_ConnTimersAndConstants):
              NULL
            );
        }
      }
    }
    else
    {
      WRRC_MSG0_ERROR("Invalid Serving Cell selected");

#ifdef FEATURE_RRCSIB_USE_FREQ_PSC_FOR_INDEX
      //rrc_sibdb_init();
#endif
      return( FAILURE );
    }
    /* Pending UPDATE_SIBS event can be cancelled since
    we're on a new cell now. */
    if( update_sibs_event_pending == TRUE )
    {
      update_sibs_event_pending = FALSE;
      if((rrcsib_cmd_store.pending_flag == TRUE) &&
         (rrcsib_cmd_store.saved_cmd.cmd_hdr.cmd_id == RRC_UPDATE_SIBS_IND)
        )
      {
        rrcsib_cmd_store.pending_flag = FALSE;
      }
      if(rrctmr_get_remaining_time(RRC_SIB_CHANGE_GUARD_TIMER) != 0)
      {
        rrctmr_stop_timer( RRC_SIB_CHANGE_GUARD_TIMER);
      }
    }
    rrcpg1_check_and_abort_bcch_mod_info_processing();

  }
  else
  {
    /* A new cell selection/reselection process failed.
    First check if the old cell is still valid. If not,
    we don't have any valid serving cell. */
    if( existing_serving_cell_valid == FALSE )
    {
      WRRC_MSG0_HIGH("Serving Cell no longer valid!");
      for(count = 0; count <SIBDB_INVALID_INDEX; count++)
      {
        if(sibdb.cell[count].index_used == TRUE)
        {
          WRRC_MSG1_HIGH("Free ASN.1 decoded sib's for sibdb idx= %d", count);
          rrc_sibdb_free_all_decodedsibs_for_a_cell(count);
        }
      }

      sibdb.active_cell_sibs_valid = FALSE;
      sibdb.active_cell_index = SIBDB_INVALID_INDEX;
      /* Pending UPDATE_SIBS event can be cancelled since
         serving cell is now invalid. */
      if( update_sibs_event_pending == TRUE )
      {
        update_sibs_event_pending = FALSE;
      }
      rrcpg1_check_and_abort_bcch_mod_info_processing();
    }
    else
    {
      for(count = 0; count <SIBDB_INVALID_INDEX; count++)
      {
        if((count != sibdb.active_cell_index) && (sibdb.cell[count].index_used == TRUE))
        {
              WRRC_MSG1_HIGH("Free ASN.1 decoded sib's for sibdb idx= %d", count);
              rrc_sibdb_free_all_decodedsibs_for_a_cell(count);
        }
      }

      /* Check if there is any pending event for the old serving
      cell and if so, process that event. */
      if( update_sibs_event_pending == TRUE )
      {
        /* Post a command to ourselves for updating the SIBs */
        rrcsib_send_command_for_pending_update_sibs_event();
        WRRC_MSG0_HIGH("Pending UPDATE_SIBS event activated");
        update_sibs_event_pending = FALSE;
      }
    }
  }/* end if new_cell_selection_succeeded == FALSE */
  return( SUCCESS );
}/* end function rrcsib_change_serving_cell_status */



/*===========================================================================

FUNCTION RRCSIB_CHECK_AND_UPDATE_SIB7_VALIDITY

DESCRIPTION

  This function checks if the SIB7 for the current active
  cell is valid or not. If it's not valid it will generate 
  a command to read SIB7 again. Then it will send a 
  command RRC_SIB7_UPDATE_STATUS_IND to indicate to the
  calling procedure if the SIB7 read succeeded.
  
DEPENDENCIES

  None.

RETURN VALUE

  TRUE if SIB7 is valid. FALSE if SIB7 is not valid. If
  FALSE is returned a RRC_SIB7_UPDATE_STATUS_IND will be sent
  to the calling procedure. 
  
SIDE EFFECTS

  BCCH logical channel may be enabled to read SIB7.

===========================================================================*/
boolean rrcsib_check_and_update_sib7_validity
(
  rrc_proc_e_type  procedure  /* The procedure that needs the SIB7 */
)
{
  rrc_cmd_type *cmd; /* To send a SIB command if needed */
  uint32 count;      /* Local Counter */
  boolean result = TRUE;    /* Store result  */

#ifdef FEATURE_WCDMA_HS_RACH
  if(rrcsib_update_sib7_hs_rach() == FALSE)
    return TRUE;
#endif

#ifdef FEATURE_3GPP_CSFB
/* when CSFB is active or SIB container is present bypass reading SIB7 */
  if((rrc_csfb_call_status)||(is_sib_cont_present))
  {
    for( count = SIB_SRV_INDEX; count < MAX_SIB_EVENTS_INDEX; count++ )
    {
      if( sib_events_ptr->event[count].event_active == TRUE )
      {
        if(sib_events_ptr->event[count].event_name == RRCSIB_UPDATE_SIBS)
        {
          /* Post-process the failed event.to kill the RRCSIB_UPDATE_SIBS
                   event*/
          WRRC_MSG0_HIGH("Dropping RRCSIB_UPDATE_SIBS event and skippng SIB 7 Update");
          rrcsib_post_process_failed_event
            ( (sib_events_index_e_type)count, /* Event Index */
              FALSE,  /* Confirm Command is required */
              SIB_EVENT_FAILED_OTHER, /* Failure cause */
              TRUE  /* Don't tear down S_BCCH */
             );
        }
      }
    }/* End if active events */
    return TRUE;
  }
#endif

/*lint -e527 statement unreachable */
  /* Check if any Nighbor BCCH event is active.
  Nieghbor BCCH events are considered higher priority
  so we'll use the current SIB7 rather than pre-empting
  the neighbor BCCH event. */
  if( sib_events_ptr->event[SIB_NGH_INDEX].event_active == TRUE)
  {
    /*----------------------------------------------------------------------------
    Check if inter-frequency cell reselection is in progress. If yes, then the
    SIB7 update requested by RCE is given higher priority than the cell-reselection.
    First post the failure indication to CSP. This will enable CSP to tear down
    N-BCCH and block further reselections at L1. After this post an internal
    command to SIB proc with a GET_SPECIFIC_SIBS cmd. By the time, the 
    GET_SPECIFIC_SIBS command is dequeued, there would no conflicts as the 
    N-BCCH would have already been torn down.
    ----------------------------------------------------------------------------*/
    if(rrccsp_is_inter_frequency_cell_resel_in_progress())
    {
      /* First send a failure status to CSP procedure */
      rrcsib_post_process_failed_event
      ( SIB_NGH_INDEX, /* Event Index                 */
        TRUE,  /* Confirm Command is required */
        SIB_EVENT_FAILED_OTHER, /* Failure reason */
        FALSE   /* Dont Tear down S_BCCH  */
      );

      WRRC_MSG0_HIGH("Replacing GET_SPECIFIC_SIBS for InterFreq with SIB7 event");

      /* Don't flag the existing event as a pending event. */
      update_sibs_event_pending = FALSE;

      /* Make the current event inactive */
      sib_events_ptr->event[SIB_NGH_INDEX].event_active = FALSE;

    }
    else
    {
      /* If there is no conflict with inter-frequency then proceed as earlier.
      Send TRUE status to RCE so that it can establish the connections */
      result = TRUE;
      return result;
    }
  }
  else if (rrcllc_get_n_bcch_setup_status() == TRUE)
  {
    WRRC_MSG0_HIGH("N-BCCH is up. Use current SIB7.");
    result = TRUE;
    return result;
  }

  /* Check if we're in Idle Mode. In Idle Mode the UE is in DRX
  mode and we don't wake up to read SIB7 so just assume that it's
  not valid. */
  if ((rrc_get_state() == RRC_STATE_DISCONNECTED) 
#ifdef FEATURE_UPDATE_SIB7_FOR_PCH_TO_FACH
    || (rrc_get_state() == RRC_STATE_CELL_PCH)
    || (rrc_get_state() == RRC_STATE_URA_PCH)
#endif
      )
  {
    result = FALSE; 
    /* Generate a command for the SIB procedure itself to
    get SIB7. Make sure there's not already a pending
    command  */
    if( sib7info.sib_ext_proc_update_req == FALSE )
    {

#ifdef FEATURE_QCHAT
      if(qchatupk_wcdma_ssib7_enabled() &&
         qchatupk_is_tch_setup_in_progress()&&
          ((rrc_get_state() == RRC_STATE_CELL_PCH)
           || (rrc_get_state() == RRC_STATE_URA_PCH)))
      
      {
        WRRC_MSG0_HIGH("QCHAT: Skip SIB 7 update (normally requested here)");
        result = TRUE;
        return result;
      }
#endif
      if(rrctmr_get_remaining_time(RRCTMR_SIB7_EXPIRATION_TIMER) == 0)
      {
        WRRC_MSG0_HIGH("SIB 7 Update requested");
        if((cmd = rrc_get_int_cmd_buf())!=NULL)
        {
          cmd->cmd_hdr.cmd_id = RRC_GET_SPECIFIC_SIBS_REQ;
          cmd->cmd.get_specific_sibs_req.for_serv_cell = TRUE;
          for( count=0; count<RRC_TOT_SIBS; count++ )
          {
            cmd->cmd.get_specific_sibs_req.sib_req[count] = FALSE;
          }
          /* Only SIB7 is requested. */
          cmd->cmd.get_specific_sibs_req.sib_req[rrc_SIB7]
            = TRUE;
          cmd->cmd.get_specific_sibs_req.procedure = RRC_PROCEDURE_SIB;
          /* Get the frequency and the scrambling code from the
          SIB database itself */
          cmd->cmd.get_specific_sibs_req.freq = 
            sibdb.cell[sibdb.active_cell_index].freq;
        
          cmd->cmd.get_specific_sibs_req.sib_scan_type = RRC_SIB_NORMAL;

          cmd->cmd.get_specific_sibs_req.scrambling_code =
            sibdb.cell[sibdb.active_cell_index].scr_code;

          rrc_put_int_cmd( cmd );

          /* Set an internal variable to indicate that another
          procedure requested the SIB7 update */
          sib7info.sib_ext_proc_update_req = TRUE;
          sib7info.req_proc = procedure;

        }
      }
      else
      {
        /*No need to read SIB 7 if SIB7 Expiration timer didnt expired.*/
        WRRC_MSG0_HIGH("Skipping SIB 7 Update since SIB-7 expiration timer is still running");
        return TRUE;
      }
    }
    else
    {
      /* Can't support more than one procedure request
      at a time */
      WRRC_MSG0_ERROR("SIB7 request error");
    }
  }
  else
  {
    /* For now we don't support CELL_PCH and URA_PCH state
    and we don't sleep in any other state. So SIB7 should
    always be valid */
    result = TRUE; 
  }

  return(result);
/*lint +e527 statement unreachable */
}

#ifdef FEATURE_WCDMA_HS_RACH

/*===========================================================================

FUNCTION RRCSIB_UPDATE_SIB7_HS_RACH

DESCRIPTION

  This function checks if the ul_Interference parameter is present in SIB5 or SIB7.
  If not then a SIB7 update is required.
  
DEPENDENCIES

  None.

RETURN VALUE

  TRUE if SIB7 update is required 
  
SIDE EFFECTS

  BCCH logical channel may be enabled to read SIB7.

===========================================================================*/
boolean rrcsib_update_sib7_hs_rach
(
  void  
)
{
  rrc_SysInfoType5    *sib5_ptr = NULL;

  rrc_CommonEDCHSystemInfoFDD *common_edch_sys_info_ptr = NULL;

  if(! rrchsrach_camped_cell_supports_hsrach())
    return TRUE;
  
  /* sib5 null check is done in above function */
  sib5_ptr = (rrc_SysInfoType5 *)rrc_sibdb_return_sib_for_srv_cell(rrc_SIB5);
  common_edch_sys_info_ptr = &sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.\
   v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.v890NonCriticalExtensions.v8b0NonCriticalExtensions.\
   v8d0NonCriticalExtensions.sysInfoType5_v8d0ext.commonEDCHSystemInfoFDD;

  if(sib5_ptr == NULL)
    return TRUE;

  /* Get the UL interference parameter */
  if((common_edch_sys_info_ptr != NULL) && 
     (common_edch_sys_info_ptr->m.ul_InterferenceForCommonEDCHPresent))
  {
    if((common_edch_sys_info_ptr->ul_InterferenceForCommonEDCH >= RRCLLC_MIN_UL_INTERFERENCE) &&
       (common_edch_sys_info_ptr->ul_InterferenceForCommonEDCH <= RRCLLC_MAX_UL_INTERFERENCE))
    {
      return FALSE;
    }      
    else
    {
      WRRC_MSG0_ERROR("Invalid UL interference");
      return TRUE;
    }
  }
 return TRUE;
}

#endif



/*===========================================================================

FUNCTION RRCSIB_GET_SIB_7_UPDATE_STATUS

DESCRIPTION

  API to provide info whether SIB_7 update is going on or not.
    
DEPENDENCIES

  None.

RETURN VALUE

SUCCESS:  SIB_7 update is on-going
FAILURE:  SIB_7 update is not-on-going  
  
SIDE EFFECTS

  

===========================================================================*/
uecomdef_status_e_type rrcsib_get_sib_7_update_status
(
  void
)
{
  uint32  count; /* Counter used in loops */

  for( count = 0; count < (uint32)MAX_SIB_EVENTS_INDEX; count++ )
  {
    /* Check if the event is stil active.*/
    if (sib_events_ptr->event[count].event_active == TRUE)
    {
      /* Ensure that the SIB7 update was requested.*/
      if( (sib_events_ptr->event[count].event_name == RRCSIB_GET_SPECIFIC_SIBS) &&
          (sib_events_ptr->event[count].req_proc == RRC_PROCEDURE_SIB))
      {
        /* sib7 update is on-going */
        WRRC_MSG0_HIGH("SIB-7 Update on-going");
        return SUCCESS;
      }
    }
  }

  /* sib7 update is not happening */
  return FAILURE;
}

/*===========================================================================

FUNCTION RRCSIB_FORCE_SIB_EVENT_INIT

DESCRIPTION

  This function will be used in the following 2 scenarios
  
  1. When RCE needs to abort SIB procedure from reading the SIBs when it had earlier
     requested to update SIB7
     
     Action : SIB procedure will clean up the event specific variables associated
     It will delete all the SIB's that it had partially read for that event. It will
     send CNF with failure to RCE. It will send a CHANNEL_CFG_REQ to LLC procedure 
     to tear down the S-BCCH.
     
  2. CSP can call this function when it detects Out of service area
  
     Action : SIB will clean up all variables associated with the active event. It will
     delete all the sibs that it had partially read. Send FAILURE to the procedure which
     requested it. No need to send a tear down to LLC as by this time all the channels
     would already have been torn down.
  
DEPENDENCIES

  None.

RETURN VALUE

  
  
SIDE EFFECTS

  

===========================================================================*/
void rrcsib_force_sib_event_init(rrcsib_event_abort_cause_e_type abort_cause) 
{
  uint32  count; /* Counter used in loops */
  uint32  dbindex;
  uint32  sib;

#ifdef FEATURE_DUAL_SIM
  rrc_csp_scan_e_type curr_scan = rrccsp_get_curr_scan();
  rrc_drx_exp_when_wtng_for_mib = FALSE;

  /*To take care of scenarios where SIB event is getting aborted when CSP is waiting for BCH release confirm*/
  if((curr_scan == RRC_CSP_WTOW_BPLMN_AUTOMATIC_SCAN)||
     (curr_scan == RRC_CSP_WTOW_BPLMN_MANUAL_SCAN))
  {
    rrccsp_reset_wait_l1_rel_lock();
  }
#endif

  switch (abort_cause)
  {
    case RRCSIB_ABORT_SIB7_UPDATE:
      /* First clear the SIB events database */
      for( count = 0; count < (uint32)MAX_SIB_EVENTS_INDEX; count++ )
      {
        // Check if the event is stil active.
        if (sib_events_ptr->event[count].event_active == TRUE)
        {
          // Ensure that the SIB7 update was requested.
          if( (sib_events_ptr->event[count].event_name == RRCSIB_GET_SPECIFIC_SIBS) &&
              (sib_events_ptr->event[count].req_proc == RRC_PROCEDURE_SIB)
#ifdef FEATURE_UPDATE_SIB7_IN_FACH
              &&/*Since CSP calls rrcsib_force_sib_event_init with casue RRCSIB_ABORT_SIB7_UPDATE 
                  on  receiving DL_WEAK ind, for sib-7 event in Fach its ok to ignore & continue
                  as RRC will either get OOS or perform Cell reselction at the end */
              (rrc_sib_is_current_event_sib7_in_fach() == FALSE)
#endif
            )
          {
            // If the a database index was found/created for this event
            // then clear up all the sibs that were got until this time.
            if (sib_events_ptr->event[count].sib_db_index_found == TRUE)
            {
              dbindex = sib_events_ptr->event[count].sib_db_index;
      
              // If an index has already been found then clear allthe sibs collected so far
              // and clear other variables associated with this index
              if (dbindex != SIBDB_INVALID_INDEX)
              {
                /* The decoding failed */
                WRRC_MSG0_HIGH("Clearing TSIB variables for SIB7");
                sib = (uint32)rrc_SIB7;
                /*Reset all the variables for this SIB so that we can try to
                read it again */
                sib_events_ptr->event[count].tsib[sib].sib_received = FALSE;
                sib_events_ptr->event[count].tsib[sib].sib_decoded = FALSE;
                sib_events_ptr->event[count].tsib[sib].seg_count = 0;
                sib_events_ptr->event[count].tsib[sib].seg_index = 
                  (MAX_SEG_INDEX_FOR_A_SIB + 1);
                sib_events_ptr->event[count].tsib[sib].sib_tot_length = 0;
              }
            }
            
            if (sib_events_ptr->srv_bcch.bcch_action == SIB_BCCH_NO_ACTION)
            {
              rrcsib_post_process_failed_event((sib_events_index_e_type)count,FALSE,SIB_EVENT_FAILED_OTHER, FALSE);
            }
            else //if bcch_action == SIB_BCCH_SETUP_COMPLETE, or bcch_action == SIB_BCCH_SETUP_PENDING
            {
              /* If the channel config req for setting serving BCCH is sent to LLC but 
              the confirm for the same is not received yet. or the BCCH has already been
              setup then send another request for teardown. The 4th parameter in the
              below function is set to TRUE so that post_process_failed event will take
              care of sending the BCCH tear down*/
              rrcsib_post_process_failed_event((sib_events_index_e_type)count,FALSE,SIB_EVENT_FAILED_OTHER, TRUE);   
            }
          }
        }
      }

      /* abandon the sib7 update info.. above processing in post_process should take care of resetting
         all flags, but in race conditions when event's are not yet initialized and we get this function
         call, better to reset these flags right here
       */
      sib7info.sib_ext_proc_update_req = FALSE;
      sib7info.sib_timer_update_req = FALSE;

      break;

    case RRCSIB_ABORT_DUE_TO_OOS:
    case RRCSIB_ABORT_DUE_TO_NGH_OOS:
    case RRCSIB_ABORT_DUE_TO_WTOW_BPLMN:
    case RRCSIB_ABORT_DUE_TO_DL_WK_IND:
    case RRCSIB_ABORT_DUE_SIB_READ_IN_DCH:
    case RRCSIB_ABORT_DUE_SIB_READ_IN_DCH_NO_PCCPCH_DROP:
      /* First clear the SIB events database */
      for( count = 0; count < (uint32)MAX_SIB_EVENTS_INDEX; count++ )
      {
        if (sib_events_ptr->event[count].event_active == TRUE)
        {
          if((sib_events_ptr->event[count].event_name == RRCSIB_GET_SPECIFIC_SIBS)&&
            (sib_events_ptr->event[count].req_proc == RRC_PROCEDURE_GPS))
          {
            /*Some Special handling for cell id read in dch event. In some cases tear down 
             *of S-BCCH is taken care by SIB in that case rrcsib_force_sib_event_init() is 
             *called by procedure with cause RRCSIB_ABORT_DUE_SIB_READ_IN_DCH. 
             *In other cases LLC will take care of tearing down of S-BCCH(Secnarios like 
             *W to G HO failure, on resuming on W LLC would simply want to kill sib event, 
             *as there is no S-BCCH)
             */
            if(RRCSIB_ABORT_DUE_SIB_READ_IN_DCH == abort_cause)
            {
              WRRC_MSG0_HIGH("RRCGPS: Abort SIB read for cell id reading in DCH and DROP BCCH");
              rrcsib_post_process_failed_event((sib_events_index_e_type)count,TRUE,SIB_EVENT_FAILED_OTHER, TRUE);
            }
            else
            {
              WRRC_MSG0_HIGH("RRCGPS: Abort SIB read for cell id reading in DCH and no BCCH Drop");
              rrcsib_post_process_failed_event((sib_events_index_e_type)count,TRUE,SIB_EVENT_FAILED_OTHER, FALSE);
            }
          }
          else
          {
            if(RRCSIB_ABORT_DUE_TO_NGH_OOS != abort_cause)
            {
              if(RRCSIB_ABORT_DUE_TO_DL_WK_IND == abort_cause)
              {
                rrcsib_post_process_failed_event((sib_events_index_e_type)count,FALSE,SIB_EVENT_FAILED_TIMEOUT, TRUE);
              }
              else
              {
                rrcsib_post_process_failed_event((sib_events_index_e_type)count,FALSE,SIB_EVENT_FAILED_OTHER, FALSE);
              }
            }
            else
            {
              if((rrcllc_get_s_bcch_setup_status() == TRUE)||
                ((sib_events_ptr->event[SIB_SRV_INDEX].for_serving_cell)&&
                (sib_events_ptr->srv_bcch.bcch_action == SIB_BCCH_SETUP_PENDING))
                )
              {
                rrcsib_post_process_failed_event((sib_events_index_e_type)count,FALSE,SIB_EVENT_FAILED_OTHER, TRUE);
              }
              else
              {
                rrcsib_post_process_failed_event((sib_events_index_e_type)count,FALSE,SIB_EVENT_FAILED_OTHER, FALSE);
              }
            }
          }
        }
      }

      //call the generic sib_proc_Events_init just to reset all other SIB based events that need
      //to be re-iniitalized..  This is added as protection to take care of resetting flags when
      //an event is not generated.. because in post_process_failed_event all resetting is done 
      //only if an EVENT is active
      rrc_sib_proc_events_init();
      
      //only if abort is for serving or due to oos, reset the active_cell_Status, for neighbor oos don't
      //reset serving cell status as we need a serving camped cell
      if(((RRCSIB_ABORT_DUE_TO_OOS == abort_cause)||
        (RRCSIB_ABORT_DUE_TO_DL_WK_IND == abort_cause)) && 
        (rrc_get_state() != RRC_STATE_CONNECTING)&&
        (!((rrc_get_state() == RRC_STATE_DISCONNECTED)&& (rrcrce_get_rce_sub_state() != RRCRCE_INITIAL))))

      {
        rrc_sibdb_clear_active_cell_status();
      }
      break;
    default:
      break;
  }
}
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
/*===========================================================================

FUNCTION rrcsib_check_if_bands_are_supported

DESCRIPTION

  This function will check whether the bands from the list are supported by the UE
  or not.

DEPENDENCIES

  None.

RETURN VALUE

  FALSE - If we support any of the multiple bands 
  TRUE - If we dont support any of the multiple bands 
  
SIDE EFFECTS

  None
===========================================================================*/
static boolean rrcsib_check_if_bands_are_supported(rrc_MultipleFrequencyBandIndicatorListFDD multi_band_list,
                                                   rrc_plmn_identity_type plmn,boolean for_serving_cell)
{
  /*check if UE supports any of the bands in the list passed */
  uint8 i;
  for(i=0;i<multi_band_list.n;i++)
  {
    if(multi_band_list.elem[i].t == T_rrc_FrequencyBandsIndicatorFDD_frequencyBandsIndicator1)
    {
      rrc_RadioFrequencyBandFDD FDD1 = multi_band_list.elem[i].u.frequencyBandsIndicator1;
      if((Fdd1_internal_band[FDD1] != RRC_CSP_BAND_MAX_INDEX) && 
         (rrcsib_check_if_band_supported(rrc_nv_band_priority_config.band_priority_list[Fdd1_internal_band[FDD1]],for_serving_cell,plmn)))
      {
        return FALSE;
      }
      else if((Fdd1_internal_band[FDD1] != RRC_CSP_BAND_MAX_INDEX) &&
               rrccsp_is_supported_band(rrc_nv_band_priority_config.band_priority_list[Fdd1_internal_band[FDD1]]))
      {
        /* If the band in the MFBI list is supported by the UE but if it is not the current scanned band
           then bar the UARFCN. */
        return TRUE;
      }
    }
    else if(multi_band_list.elem[i].t == T_rrc_FrequencyBandsIndicatorFDD_frequencyBandsIndicator2)
    {
      rrc_RadioFrequencyBandFDD2 FDD2 = multi_band_list.elem[i].u.frequencyBandsIndicator2;
      if((Fdd2_internal_band[FDD2] != RRC_CSP_BAND_MAX_INDEX) && 
         (rrcsib_check_if_band_supported(rrc_nv_band_priority_config.band_priority_list[Fdd2_internal_band[FDD2]],for_serving_cell,plmn)))
      {
        return FALSE;
      }
      else if((Fdd2_internal_band[FDD2] != RRC_CSP_BAND_MAX_INDEX) &&
               rrccsp_is_supported_band(rrc_nv_band_priority_config.band_priority_list[Fdd2_internal_band[FDD2]]))
      {
        /* If the band in the MFBI list is supported by the UE but if it is not the current scanned band
           then bar the UARFCN. */
        return TRUE;
      }
    }
  }
  return TRUE;
}
/*===========================================================================

FUNCTION rrcsib_check_multibands_supported_from_sib6

DESCRIPTION

  This function will be used to find if we support any of the multiple bands 
  indicated in the SIB6

DEPENDENCIES

  None.

RETURN VALUE

  FALSE - If we support any of the multiple bands in sib6
  TRUE - If we dont support any of the multiple bands in sib6
  
SIDE EFFECTS

  None

===========================================================================*/
static boolean rrcsib_check_multibands_supported_from_sib6(rrc_SysInfoType6 *sib6_ptr,rrc_plmn_identity_type plmn,boolean for_serving_cell)
{
  /*If Multiple band list is present in sib6, then check if UE supports any of those bands */
  rrc_SysInfoType6_va80ext_IEs *rrc_va80_6ext= NULL;
  if( (rrc_mfbi_support_nv) && (sib6_ptr != NULL) && 
     (sib6_ptr->m.v4b0NonCriticalExtensionsPresent) &&
     (sib6_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent) && 
     (sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v650nonCriticalExtensionsPresent) &&
     (sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650nonCriticalExtensions.m.v690nonCriticalExtensionsPresent) &&
     (sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650nonCriticalExtensions.v690nonCriticalExtensions.m.v770NonCriticalExtensionsPresent) &&
     (sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650nonCriticalExtensions.v690nonCriticalExtensions.v770NonCriticalExtensions
       .m.va40NonCriticalExtensionsPresent) &&
     (sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650nonCriticalExtensions.v690nonCriticalExtensions.v770NonCriticalExtensions.va40NonCriticalExtensions
       .m.va80NonCriticalExtensionsPresent) &&
     (sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650nonCriticalExtensions.v690nonCriticalExtensions
       .v770NonCriticalExtensions.va40NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType6_va80ext.m.multipleFrequencyBandIndicatorListFDDPresent))
  {
    rrc_va80_6ext=&sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650nonCriticalExtensions.v690nonCriticalExtensions
                            .v770NonCriticalExtensions.va40NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType6_va80ext;
    return rrcsib_check_if_bands_are_supported(rrc_va80_6ext->multipleFrequencyBandIndicatorListFDD,plmn,for_serving_cell);

  }
  return TRUE;
}
/*===========================================================================

FUNCTION rrcsib_check_multibands_supported_from_sib5

DESCRIPTION

  This function will be used to find if we support any of the multiple bands 
  indicated in the SIB5
  
DEPENDENCIES

  None.

RETURN VALUE

  FALSE - If we support any of the multiple bands in sib5
  TRUE - If we dont support any of the multiple bands in sib5
  
SIDE EFFECTS

  None

===========================================================================*/
static boolean rrcsib_check_multibands_supported_from_sib5(rrc_SysInfoType5 *sib5_ptr,rrc_plmn_identity_type plmn,boolean for_serving_cell)
{
  rrc_SysInfoType5_va80ext_IEs *rrc_va80_5ext= NULL;
  /* If the multiple band list is present in sib5, then check if we support any of the bands in the list */
  if((rrc_mfbi_support_nv) && (sib5_ptr != NULL) && 
    (sib5_ptr->m.v4b0NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent) && 
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v650NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.m.v680NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.m.v690NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.m.v770NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.m.
                 v860NonCriticalExtensionsPresent) &&  
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
                 v860NonCriticalExtensions.m.v890NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
                 v860NonCriticalExtensions.v890NonCriticalExtensions.m.v8b0NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
                 v860NonCriticalExtensions.v890NonCriticalExtensions.v8b0NonCriticalExtensions.m.v8d0NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
                v860NonCriticalExtensions.v890NonCriticalExtensions.v8b0NonCriticalExtensions.v8d0NonCriticalExtensions.m.va40NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions
               .v770NonCriticalExtensions.v860NonCriticalExtensions.v890NonCriticalExtensions.v8b0NonCriticalExtensions.v8d0NonCriticalExtensions.va40NonCriticalExtensions
               .m.va80NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions
               .v770NonCriticalExtensions.v860NonCriticalExtensions.v890NonCriticalExtensions.v8b0NonCriticalExtensions.v8d0NonCriticalExtensions.va40NonCriticalExtensions.
                va80NonCriticalExtensions.sysInfoType5_va80ext.m.multipleFrequencyBandIndicatorListFDDPresent))
  {
    rrc_va80_5ext=&sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions
                  .v770NonCriticalExtensions.v860NonCriticalExtensions.v890NonCriticalExtensions.v8b0NonCriticalExtensions.v8d0NonCriticalExtensions.va40NonCriticalExtensions.
                   va80NonCriticalExtensions.sysInfoType5_va80ext;
    return rrcsib_check_if_bands_are_supported(rrc_va80_5ext->multipleFrequencyBandIndicatorListFDD,plmn,for_serving_cell);
  }
  return TRUE;
}
/*===========================================================================

FUNCTION rrcsib_check_for_redirection_bands_support

DESCRIPTION

  This function will be used to find if the bandindicator present in SIB5/SIB6
  is supported by the UE or not.
  
DEPENDENCIES

  None.

RETURN VALUE

  TRUE - If there is a band mismatch
  FALSE - If there is no band mismatch or the band info is not specified in SIB5/SIB6
  
SIDE EFFECTS

  None

===========================================================================*/

void rrcsib_check_for_redirection_bands_support(boolean *supportoffrequencyband)
{
  /* Local variables */ 
  rrc_SysInfoType5 *sib5_ptr = NULL;
  uint32 i;
  rrc_SysInfoType5_va40ext_IEs *rrc_va40_ext= NULL;
  sib5_ptr = (rrc_SysInfoType5 *) rrc_sibdb_return_sib_for_srv_cell(rrc_SIB5);
  supportoffrequencyband[0] = FALSE;
  supportoffrequencyband[1] = FALSE;
  rrc_va40_ext = &sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.
                  v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.v890NonCriticalExtensions.v8b0NonCriticalExtensions.
                  v8d0NonCriticalExtensions.va40NonCriticalExtensions.sysInfoType5_va40ext;

  /* Check if frequencyBandsIndicatorsupport IE is present */
  
  if((sib5_ptr != NULL) && 
     (sib5_ptr->m.v4b0NonCriticalExtensionsPresent) &&
     (sib5_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent) && 
     (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v650NonCriticalExtensionsPresent) &&
     (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.m.v680NonCriticalExtensionsPresent) &&
     (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.m.v690NonCriticalExtensionsPresent) &&
     (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.
      m.v770NonCriticalExtensionsPresent) &&
     (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.m.
          v860NonCriticalExtensionsPresent) &&
     (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
          v860NonCriticalExtensions.m.v890NonCriticalExtensionsPresent) &&
     (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
          v860NonCriticalExtensions.v890NonCriticalExtensions.m.v8b0NonCriticalExtensionsPresent) &&
     (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
          v860NonCriticalExtensions.v890NonCriticalExtensions.v8b0NonCriticalExtensions.m.v8d0NonCriticalExtensionsPresent) &&
     (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
          v860NonCriticalExtensions.v890NonCriticalExtensions.v8b0NonCriticalExtensions.v8d0NonCriticalExtensions.m.va40NonCriticalExtensionsPresent) &&
     (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions
         .v770NonCriticalExtensions.v860NonCriticalExtensions.v890NonCriticalExtensions.v8b0NonCriticalExtensions.v8d0NonCriticalExtensions.va40NonCriticalExtensions.
         sysInfoType5_va40ext.m.frequencyBandsIndicatorSupportPresent))
  {
    if(rrc_va40_ext->frequencyBandsIndicatorSupport.modeSpecificInfo.t == T_rrc_FrequencyBandsIndicatorSupport_modeSpecificInfo_fdd)
    {
        for(i=0;i<rrc_va40_ext->frequencyBandsIndicatorSupport.modeSpecificInfo.u.fdd->frequencyBandsSupportFDD.n;i++)
      {
        if(rrc_va40_ext->frequencyBandsIndicatorSupport.modeSpecificInfo.u.fdd->frequencyBandsSupportFDD.elem[i].t == T_rrc_FrequencyBandsIndicatorFDD_frequencyBandsIndicator1)
        {
          rrc_RadioFrequencyBandFDD FDD1 = rrc_va40_ext->frequencyBandsIndicatorSupport.modeSpecificInfo.u.fdd->frequencyBandsSupportFDD.elem[i].u.frequencyBandsIndicator1;
          if(rrccsp_is_supported_band(rrc_nv_band_priority_config.band_priority_list[Fdd1_internal_band[FDD1]]))
          {
            WRRC_MSG0_HIGH("band is supported");
            supportoffrequencyband[i] = TRUE;
          }
        }
        else if(rrc_va40_ext->frequencyBandsIndicatorSupport.modeSpecificInfo.u.fdd->frequencyBandsSupportFDD.elem[i].t == T_rrc_FrequencyBandsIndicatorFDD_frequencyBandsIndicator2)
        {
          rrc_RadioFrequencyBandFDD2 FDD2 = rrc_va40_ext->frequencyBandsIndicatorSupport.modeSpecificInfo.u.fdd->frequencyBandsSupportFDD.elem[i].u.frequencyBandsIndicator2;
          if(rrccsp_is_supported_band(rrc_nv_band_priority_config.band_priority_list[Fdd2_internal_band[FDD2]]))
          {
            WRRC_MSG0_HIGH("band is supported");
            supportoffrequencyband[i] = TRUE;
          }
        }
      }
    }
  }
}
#endif
#ifdef FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR
/*===========================================================================

FUNCTION rrcsib_check_band_mismatch_from_sib5_sib6

DESCRIPTION

  This function will be used to find if the band present in SIB5/SIB6
  is supported by the UE or not.
  
DEPENDENCIES

  None.

RETURN VALUE

  TRUE - If there is a band mismatch
  FALSE - If there is no band mismatch or the band info is not specified in SIB5/SIB6
  
SIDE EFFECTS

  None

===========================================================================*/
boolean rrcsib_check_band_mismatch_from_sib5_sib6(uint32 index)
{
  /* Local variables */
  rrc_state_e_type rrc_state;     
  rrc_csp_band_class_type  band_class;
  boolean band_mismatch = FALSE;
  rrc_SysInfoType6 *sib6_ptr = NULL; 
  rrc_SysInfoType5 *sib5_ptr = NULL;
  rrc_MasterInformationBlock *mib_ptr = NULL;
  boolean sib5_read = FALSE;
  boolean sib6_read = FALSE;
  boolean fdd2_extension_present = FALSE;
  uint32 dbindex = sib_events_ptr->event[index].sib_db_index;
  boolean for_serving_cell = sib_events_ptr->event[index].for_serving_cell;
  rrc_plmn_identity_type plmn;
  rrc_RadioFrequencyBandFDD FDD2;
  
  /* Initialize PLMN to dummy value to keep lint happy */
  plmn.mcc[0] = 0;
  plmn.mcc[1] = 0;
  plmn.mcc[2] = 0;
  plmn.num_mnc_digits = 3;
  plmn.mnc[0] = 0;
  plmn.mnc[1] = 0;
  plmn.mnc[2] = 0;

  if(for_serving_cell)
  {
    band_class = rrc_get_frequency_band(
                            rrccsp_get_serving_frequency(), rrccsp_get_curr_nas_requested_plmn());
  }
  else
  {
    band_class = rrc_get_frequency_band(
                            rrccsp_get_neighbor_frequency(), rrccsp_get_curr_nas_requested_plmn());
  }


  /* Store local information that sib 5 and sib 6 are needed, received and decoded rather than use
     long loops everywhere */
  if((sib_events_ptr->event[index].tsib[rrc_SIB5].sib_needed == TRUE) &&
     (sib_events_ptr->event[index].tsib[rrc_SIB5].sib_received == TRUE) &&
     (sib_events_ptr->event[index].tsib[rrc_SIB5].sib_decoded == TRUE))
  {
     sib5_read = TRUE;
  }

  if((sib_events_ptr->event[index].tsib[rrc_SIB6].sib_needed == TRUE) &&
     (sib_events_ptr->event[index].tsib[rrc_SIB6].sib_received == TRUE) &&
     (sib_events_ptr->event[index].tsib[rrc_SIB6].sib_decoded == TRUE))
  {
    sib6_read = TRUE;
  }

  if((FALSE == sib5_read) && (FALSE == sib6_read))
  {
    /* If neither SIB5 nor SIB6 are read,cant determine band mismatch,return FALSE */ 
    WRRC_MSG0_HIGH("SIB5 & SIB6 not read yet");
    return FALSE;
  }

  /* Get the current RRC state */
  rrc_state = rrc_get_state();

  mib_ptr = (rrc_MasterInformationBlock *)rrc_sibdb_return_sib_for_db_index
                                     ((rrc_SIB_type)rrc_MIB,
                                       dbindex
                                     );

  if(mib_ptr != NULL)
  {
    /* Get PLMN */
    rrc_translate_gsm_map_plmn_id(&(mib_ptr->plmn_Type.u.gsm_MAP->plmn_Identity),&plmn);
  }


  /* Connected mode handling.Check for SIB6.If freqBandInfo present,check for mismatch and return status */
  if((rrc_state != RRC_STATE_CONNECTING) && (rrc_state != RRC_STATE_DISCONNECTED))
  {
    if(TRUE == sib6_read)
    {
      sib6_ptr = (rrc_SysInfoType6 *)rrc_sibdb_return_sib_for_db_index
                                     ((rrc_SIB_type)rrc_SIB6,
                                       dbindex
                                     );
      if((sib6_ptr != NULL) && 
         (RRC_MSG_COMMON_BITMASK_IE_PTR(sib6_ptr,rrc_SysInfoType6_, v4b0NonCriticalExtensions)) &&
         (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib6_ptr->v4b0NonCriticalExtensions),rrc_, sysInfoType6_v4b0ext)) &&
         (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib6_ptr->v4b0NonCriticalExtensions.sysInfoType6_v4b0ext),  
               rrc_SysInfoType6_v4b0ext_IEs_,frequencyBandIndicator))
       )
      {
        rrc_RadioFrequencyBandFDD FDD1 = sib6_ptr->v4b0NonCriticalExtensions.sysInfoType6_v4b0ext.frequencyBandIndicator;
        /* Check for 800 band mismatch*/
        if(sib6_ptr->v4b0NonCriticalExtensions.sysInfoType6_v4b0ext.frequencyBandIndicator ==
                     rrc_RadioFrequencyBandFDD_bandVI)
        {
          if(rrcsib_check_if_band_supported(RRC_CSP_WCDMA_800_BAND,for_serving_cell,plmn) || 
             rrcsib_check_if_band_supported(RRC_CSP_WCDMA_BC19_BAND,for_serving_cell,plmn))
          {
            return FALSE;
          }
          else
          {
            if(rrccsp_is_supported_band(RRC_CSP_WCDMA_800_BAND))
            {
              /* If the band indicated in FBI is supported by the UE and if it is not the current scanned band,
                 then bar the UARFCN and if the FBI is not supported, check the MFBI list of the cell. */
              return TRUE;
            }
            WRRC_MSG0_HIGH("The band in FBI is not supported.Look into the MFBI list.");
              /*The band in FBI is not supported by UE,look into the MFBI list*/
            return rrcsib_check_multibands_supported_from_sib6(sib6_ptr,plmn,for_serving_cell);
          }
        }
        else if(sib6_ptr->v4b0NonCriticalExtensions.sysInfoType6_v4b0ext.frequencyBandIndicator ==
                  rrc_RadioFrequencyBandFDD_extension_indicator)
        {
          fdd2_extension_present = TRUE;

        }
        else
        {
          if((Fdd1_internal_band[FDD1] == RRC_CSP_BAND_MAX_INDEX) || 
             (!rrcsib_check_if_band_supported(rrc_nv_band_priority_config.band_priority_list[Fdd1_internal_band[FDD1]],for_serving_cell,plmn)))  
          {
            if((Fdd1_internal_band[FDD1] != RRC_CSP_BAND_MAX_INDEX) &&
                rrccsp_is_supported_band(rrc_nv_band_priority_config.band_priority_list[Fdd1_internal_band[FDD1]]))
            {
              /* If the band indicated in FBI is supported by the UE and if it is not the current scanned band,
                 then bar the UARFCN and if the FBI is not supported, check the MFBI list of the cell. */
              return TRUE;
            }
            WRRC_MSG0_HIGH("The band in FBI is not supported.Look into the MFBI list.");
            /*The band in FBI is not supported by UE,look into the MFBI list*/
            return rrcsib_check_multibands_supported_from_sib6(sib6_ptr,plmn,for_serving_cell);
          }
          else
          {
            return FALSE;
          }
        }
      }

      /* Check FDD2 band field in SIB6 for mismatch */
      if((TRUE == fdd2_extension_present) &&
         (sib6_ptr != NULL) && 
         (RRC_MSG_COMMON_BITMASK_IE_PTR(sib6_ptr,rrc_SysInfoType6_, v4b0NonCriticalExtensions)) &&
         (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib6_ptr->v4b0NonCriticalExtensions), 
                        rrc_SysInfoType6_v4b0NonCriticalExtensions_, 
                        v590NonCriticalExtensions))&&
         (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions),
                        rrc_,v650nonCriticalExtensions)) &&
         (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650nonCriticalExtensions),
                        rrc_,sysInfoType6_v650ext))
        ) 
      {
        WRRC_MSG0_HIGH("FDD2 extensions present in SIB6");
        FDD2 = sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.
                                         v650nonCriticalExtensions.sysInfoType6_v650ext.frequencyBandIndicator2;
        if((Fdd2_internal_band[FDD2] == RRC_CSP_BAND_MAX_INDEX) ||
           (!rrcsib_check_if_band_supported(rrc_nv_band_priority_config.band_priority_list[Fdd2_internal_band[FDD2]],for_serving_cell,plmn)))  
        {
          if((Fdd2_internal_band[FDD2] != RRC_CSP_BAND_MAX_INDEX) &&
              rrccsp_is_supported_band(rrc_nv_band_priority_config.band_priority_list[Fdd2_internal_band[FDD2]]))
          {
            /* If the band indicated in FBI is supported by the UE and if it is not the current scanned band,
               then bar the UARFCN and if the FBI is not supported, check the MFBI list of the cell. */
            return TRUE;
          }
          WRRC_MSG0_HIGH("The band in FBI is not supported.Look into the MFBI list.");
          /*The band in FBI2 is not supported by UE,look into the MFBI list*/
          return rrcsib_check_multibands_supported_from_sib6(sib6_ptr,plmn,for_serving_cell);
        }
        else
        {
          return FALSE;
        }
      }  
      if((rrc_mfbi_support_nv) && (sib6_ptr != NULL) && 
      (sib6_ptr->m.v4b0NonCriticalExtensionsPresent) &&
      (sib6_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent) && 
      (sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v650nonCriticalExtensionsPresent) &&
      (sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650nonCriticalExtensions.m.v690nonCriticalExtensionsPresent) &&
      (sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650nonCriticalExtensions.v690nonCriticalExtensions.m.v770NonCriticalExtensionsPresent) &&
      (sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650nonCriticalExtensions.v690nonCriticalExtensions.v770NonCriticalExtensions
        .m.va40NonCriticalExtensionsPresent) &&
      (sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650nonCriticalExtensions.v690nonCriticalExtensions.v770NonCriticalExtensions.va40NonCriticalExtensions
        .m.va80NonCriticalExtensionsPresent) &&
      (sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650nonCriticalExtensions.v690nonCriticalExtensions
        .v770NonCriticalExtensions.va40NonCriticalExtensions.va80NonCriticalExtensions.sysInfoType6_va80ext.m.multipleFrequencyBandIndicatorListFDDPresent))
      {
        if((band_class == RRC_CSP_IMT2K_BAND) || (band_class == RRC_CSP_WCDMA_1800_BAND) 
          ||(band_class == RRC_CSP_PCS_BAND)
        )
        {
          if(rrccsp_is_supported_band(band_class))
          {
            WRRC_MSG1_HIGH("No band mismatch, Allow camping on band :%d",band_class);
            return FALSE;
          }
          else
          {
            /* Frequency should be barred according to spec */
            WRRC_MSG1_HIGH("Band mismatch, Don't allow camping on band :%d",band_class);
            return TRUE;
          }
        }
        WRRC_MSG0_HIGH("FBIs not set.Look into MFBI list");
        return rrcsib_check_multibands_supported_from_sib6(sib6_ptr,plmn,for_serving_cell);
      }
    }
  }

  fdd2_extension_present = FALSE;

  /* If we reach this point,it means that we could not use SIB6 to find the band mismatch.Try SIB5 */
  /* Get the SIB5 ptr from sibdb */
  sib5_ptr = (rrc_SysInfoType5 *)rrc_sibdb_return_sib_for_db_index
                                             ((rrc_SIB_type)rrc_SIB5,
                                               dbindex
                                             );
  /* Check if frequencyBandIndicator IE is present */
  if((sib5_ptr != NULL) &&
     (RRC_MSG_COMMON_BITMASK_IE_PTR(sib5_ptr, rrc_SysInfoType5_,v4b0NonCriticalExtensions)) &&
     (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib5_ptr->v4b0NonCriticalExtensions),
                  rrc_, sysInfoType5_v4b0ext)) &&
     (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib5_ptr->v4b0NonCriticalExtensions.sysInfoType5_v4b0ext),
                  rrc_SysInfoType5_v4b0ext_IEs_, frequencyBandIndicator))
   )
  {
    /* Check for 800 band mismatch*/
    rrc_RadioFrequencyBandFDD FDD1 = sib5_ptr->v4b0NonCriticalExtensions.sysInfoType5_v4b0ext.frequencyBandIndicator;
    if(sib5_ptr->v4b0NonCriticalExtensions.sysInfoType5_v4b0ext.frequencyBandIndicator ==
          rrc_RadioFrequencyBandFDD_bandVI)
    {
      if(rrcsib_check_if_band_supported(RRC_CSP_WCDMA_800_BAND,for_serving_cell,plmn))
      {
        return FALSE;
      }
      else
      {
        if(rrccsp_is_supported_band(RRC_CSP_WCDMA_800_BAND))
        {
          /* If the band indicated in FBI is supported by the UE and if it is not the current scanned band,
             then bar the UARFCN and if the FBI is not supported, check the MFBI list of the cell. */
          return TRUE;
        }
        else if(rrcsib_check_if_band_supported(RRC_CSP_WCDMA_BC19_BAND,for_serving_cell,plmn))
        {
          /*
            As per 36.800, (R4-091781), Clause  5.5
            However, in order to support the existing legacy UE which has the capability for only Band VI, 
            the band indicator and the UARFCN must be set as "Band VI" and "UTRA Band XIX" in the DL frequency range
            of 875-885 MHz and 885-890 MHz, respectively in UTRAN 
          */
          WRRC_MSG0_HIGH("BC19: SIB5 has 800 band and UE supports BC19");
          return FALSE;
        }
        WRRC_MSG0_HIGH("The band in FBI is not supported.Look into the MFBI list.");
        /*The band in FBI is not supported by UE,look into the MFBI list*/
        return rrcsib_check_multibands_supported_from_sib5(sib5_ptr,plmn,for_serving_cell);
      }
    }
    else if(sib5_ptr->v4b0NonCriticalExtensions.sysInfoType5_v4b0ext.frequencyBandIndicator ==
              rrc_RadioFrequencyBandFDD_extension_indicator)
    {
      fdd2_extension_present = TRUE;
    }
    else
    {
      if((Fdd1_internal_band[FDD1] == RRC_CSP_BAND_MAX_INDEX) ||
         (!rrcsib_check_if_band_supported(rrc_nv_band_priority_config.band_priority_list[Fdd1_internal_band[FDD1]],for_serving_cell,plmn)))
      {
        if((Fdd1_internal_band[FDD1] != RRC_CSP_BAND_MAX_INDEX) &&
            rrccsp_is_supported_band(rrc_nv_band_priority_config.band_priority_list[Fdd1_internal_band[FDD1]]))
        {
          /* If the band indicated in FBI is supported by the UE and if it is not the current scanned band,
             then bar the UARFCN and if the FBI is not supported, check the MFBI list of the cell. */
          return TRUE;
        }
        WRRC_MSG0_HIGH("The band in FBI is not supported.Look into the MFBI list.");
        /*The band in FBI is not supported by UE,look into the MFBI list*/
        return rrcsib_check_multibands_supported_from_sib5(sib5_ptr,plmn,for_serving_cell);
      }
      else
      {
        return FALSE;
      }
    }
  }
  /* Check the FDD2 band field in SIB5 for mismatch */
  if((fdd2_extension_present == TRUE) &&
     (sib5_ptr != NULL) && 
     (RRC_MSG_COMMON_BITMASK_IE_PTR(sib5_ptr, rrc_SysInfoType5_, v4b0NonCriticalExtensions)) &&
     (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib5_ptr->v4b0NonCriticalExtensions), 
                      rrc_SysInfoType5_v4b0NonCriticalExtensions_,v590NonCriticalExtensions)) &&
     (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions),
                      rrc_, v650NonCriticalExtensions)) &&
     (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions),
                      rrc_, sysInfoType5_v650ext))
     ) 
  {
    WRRC_MSG0_HIGH("FDD2 extensions present in SIB5");
    FDD2 = sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.
                                     sysInfoType5_v650ext.frequencyBandIndicator2;
    /* Check for 900 band mismatch*/
    if((Fdd2_internal_band[FDD2] == RRC_CSP_BAND_MAX_INDEX) ||
      (!rrcsib_check_if_band_supported(rrc_nv_band_priority_config.band_priority_list[Fdd2_internal_band[FDD2]],for_serving_cell,plmn)))
    {
      if((Fdd2_internal_band[FDD2] != RRC_CSP_BAND_MAX_INDEX) &&
          rrccsp_is_supported_band(rrc_nv_band_priority_config.band_priority_list[Fdd2_internal_band[FDD2]]))
      {
        /* If the band indicated in FBI is supported by the UE and if it is not the current scanned band,
             then bar the UARFCN and if the FBI is not supported, check the MFBI list of the cell. */
        return TRUE;
      }
      WRRC_MSG0_HIGH("The band in FBI is not supported.Look into the MFBI list.");
      /*The band in FBI2 is not supported by UE,look into the MFBI list*/
      return rrcsib_check_multibands_supported_from_sib5(sib5_ptr,plmn,for_serving_cell);
    }
    else
    {
      return FALSE;
    }
  }
  if((rrc_mfbi_support_nv) && (sib5_ptr != NULL) && 
    (sib5_ptr->m.v4b0NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent) && 
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v650NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.m.v680NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.m.v690NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.m.v770NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.m.
                 v860NonCriticalExtensionsPresent) &&  
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
                 v860NonCriticalExtensions.m.v890NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
                 v860NonCriticalExtensions.v890NonCriticalExtensions.m.v8b0NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
                 v860NonCriticalExtensions.v890NonCriticalExtensions.v8b0NonCriticalExtensions.m.v8d0NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.
                v860NonCriticalExtensions.v890NonCriticalExtensions.v8b0NonCriticalExtensions.v8d0NonCriticalExtensions.m.va40NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions
               .v770NonCriticalExtensions.v860NonCriticalExtensions.v890NonCriticalExtensions.v8b0NonCriticalExtensions.v8d0NonCriticalExtensions.va40NonCriticalExtensions
               .m.va80NonCriticalExtensionsPresent) &&
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions
               .v770NonCriticalExtensions.v860NonCriticalExtensions.v890NonCriticalExtensions.v8b0NonCriticalExtensions.v8d0NonCriticalExtensions.va40NonCriticalExtensions.va80NonCriticalExtensions.
                sysInfoType5_va80ext.m.multipleFrequencyBandIndicatorListFDDPresent))
  {
    if((band_class == RRC_CSP_IMT2K_BAND) || (band_class == RRC_CSP_WCDMA_1800_BAND) 
      ||(band_class == RRC_CSP_PCS_BAND)
    )
    {
      if(rrccsp_is_supported_band(band_class))
      {
        WRRC_MSG1_HIGH("No band mismatch, Allow camping on band :%d",band_class);
        return FALSE;
      }
      else
      {
        /* Frequency should be barred according to spec */
        WRRC_MSG1_HIGH("Band mismatch, Don't allow camping on band :%d",band_class);
        return TRUE;
      }
    }
    WRRC_MSG0_HIGH("FBIs not set.Look into MFBI list");
    return rrcsib_check_multibands_supported_from_sib5(sib5_ptr,plmn,for_serving_cell);
  }
  /* Frequency Band Indicator not present*/
  {

    if((band_class == RRC_CSP_IMT2K_BAND) || (band_class == RRC_CSP_WCDMA_1800_BAND) 
       ||(band_class == RRC_CSP_PCS_BAND)
      )
    {   
      if(rrccsp_is_supported_band(band_class))
      {
        WRRC_MSG1_HIGH("No band mismatch, Allow camping on band :%d",band_class);
        band_mismatch = FALSE;
      }
      else
      {
        /* Frequency should be barred according to spec */
        WRRC_MSG1_HIGH("Band mismatch, Don't allow camping on band :%d",band_class);
        band_mismatch = TRUE;
      }          
    }
    else if((band_class == RRC_CSP_WCDMA_1700_2100_BAND)
    || (band_class == RRC_CSP_WCDMA_1700_BAND)
    || (band_class == RRC_CSP_WCDMA_BC19_BAND))
    {
      WRRC_MSG1_HIGH("Band indicator is not present. Bar the frequency:%d",band_class);
      band_mismatch = TRUE;
    }

    else if((band_class == RRC_CSP_WCDMA_850_BAND)
    || (band_class == RRC_CSP_WCDMA_800_BAND)
    || (band_class == RRC_CSP_WCDMA_900_BAND)
    || (band_class == RRC_CSP_WCDMA_1500_BAND))
    {
      /* UE behavior is unspecified */
      WRRC_MSG1_ERROR("Band indicator is not present. Lets allow camping on band :%d",band_class);
      band_mismatch = FALSE;
    }

    else
    {
      WRRC_MSG1_HIGH("Band not supported, Don't allow camping on band :%d",band_class);
      band_mismatch = TRUE;
    }

    return band_mismatch;
  }
}
#else
/*===========================================================================

FUNCTION rrcsib_check_band_mismatch_from_sib5_sib6

DESCRIPTION

  This function will be used to find if the band present in SIB5/SIB6
  is supported by the UE or not.
  
DEPENDENCIES

  None.

RETURN VALUE

  TRUE - If there is a band mismatch
  FALSE - If there is no band mismatch or the band info is not specified in SIB5/SIB6
  
SIDE EFFECTS

  None

===========================================================================*/
boolean rrcsib_check_band_mismatch_from_sib5_sib6(uint32 index)
{
  /* Local variables */
  rrc_state_e_type rrc_state;     
  rrc_SysInfoType6 *sib6_ptr = NULL; 
  rrc_SysInfoType5 *sib5_ptr = NULL;
  rrc_MasterInformationBlock *mib_ptr = NULL;
  boolean sib5_read = FALSE;
  boolean sib6_read = FALSE;
  boolean fdd2_extension_present = FALSE;
  uint32 dbindex = sib_events_ptr->event[index].sib_db_index;
  boolean for_serving_cell = sib_events_ptr->event[index].for_serving_cell;
  rrc_plmn_identity_type plmn;
  
  /* Initialize PLMN to dummy value to keep lint happy */
  plmn.mcc[0] = 0;
  plmn.mcc[1] = 0;
  plmn.mcc[2] = 0;
  plmn.num_mnc_digits = 3;
  plmn.mnc[0] = 0;
  plmn.mnc[1] = 0;
  plmn.mnc[2] = 0;

  /* Store local information that sib 5 and sib 6 are needed, received and decoded rather than use
     long loops everywhere */
  if((sib_events_ptr->event[index].tsib[rrc_SIB5].sib_needed == TRUE) &&
     (sib_events_ptr->event[index].tsib[rrc_SIB5].sib_received == TRUE) &&
     (sib_events_ptr->event[index].tsib[rrc_SIB5].sib_decoded == TRUE))
  {
     sib5_read = TRUE;
  }

  if((sib_events_ptr->event[index].tsib[rrc_SIB6].sib_needed == TRUE) &&
     (sib_events_ptr->event[index].tsib[rrc_SIB6].sib_received == TRUE) &&
     (sib_events_ptr->event[index].tsib[rrc_SIB6].sib_decoded == TRUE))
  {
    sib6_read = TRUE;
  }

  if((FALSE == sib5_read) && (FALSE == sib6_read))
  {
    /* If neither SIB5 nor SIB6 are read,cant determine band mismatch,return FALSE */ 
    WRRC_MSG0_HIGH("SIB5 & SIB6 not read yet");
    return FALSE;
  }

  /* Get the current RRC state */
  rrc_state = rrc_get_state();

  mib_ptr = (rrc_MasterInformationBlock *)rrc_sibdb_return_sib_for_db_index
                                     ((rrc_SIB_type)rrc_MIB,
                                       dbindex
                                     );

  if(mib_ptr != NULL)
  {
    /* Get PLMN */
    rrc_translate_gsm_map_plmn_id(&(mib_ptr->plmn_Type.u.gsm_MAP->plmn_Identity),&plmn);
  }


  /* Connected mode handling.Check for SIB6.If freqBandInfo present,check for mismatch and return status */
  if((rrc_state != RRC_STATE_CONNECTING) && (rrc_state != RRC_STATE_DISCONNECTED))
  {
    if(TRUE == sib6_read)
    {
      sib6_ptr = (rrc_SysInfoType6 *)rrc_sibdb_return_sib_for_db_index
                                     ((rrc_SIB_type)rrc_SIB6,
                                       dbindex
                                     );
      if((sib6_ptr != NULL) && 
         (RRC_MSG_COMMON_BITMASK_IE_PTR(sib6_ptr,rrc_SysInfoType6_, v4b0NonCriticalExtensions)) &&
         (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib6_ptr->v4b0NonCriticalExtensions),rrc_, sysInfoType6_v4b0ext)) &&
         (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib6_ptr->v4b0NonCriticalExtensions.sysInfoType6_v4b0ext),  
               rrc_SysInfoType6_v4b0ext_IEs_,frequencyBandIndicator))
       )
      {
        /* Check for 800 band mismatch*/
        if(sib6_ptr->v4b0NonCriticalExtensions.sysInfoType6_v4b0ext.frequencyBandIndicator ==
                     rrc_RadioFrequencyBandFDD_bandVI)
        {
          if(!rrcsib_check_if_band_supported(RRC_CSP_WCDMA_800_BAND,for_serving_cell,plmn))  
          {
            if(rrcsib_check_if_band_supported(RRC_CSP_WCDMA_BC19_BAND,for_serving_cell,plmn))
            {
              /*
                As per 36.800, (R4-091781), Clause  5.5
                However, in order to support the existing legacy UE which has the capability for only Band VI, 
                the band indicator and the UARFCN must be set as "Band VI" and "UTRA Band XIX" in the DL frequency range
                of 875-885 MHz and 885-890 MHz, respectively in UTRAN 
              */
              WRRC_MSG0_HIGH("BC19: SIB6 has 800 band and UE supports BC19");
              return FALSE;
            }
            else
            {
              WRRC_MSG0_HIGH("Band mismatch:SIB6 has 800 band which is not supported by UE");
              return TRUE;
            }
          }
          else
          {
            WRRC_MSG0_HIGH("Band match:SIB6 has 800 band which is supported by UE");
            return FALSE;
          }
        }

        /* Check for 850 band mismatch */
        else if(sib6_ptr->v4b0NonCriticalExtensions.sysInfoType6_v4b0ext.frequencyBandIndicator ==
                  rrc_RadioFrequencyBandFDD_bandV)
        {
          if(!rrcsib_check_if_band_supported(RRC_CSP_WCDMA_850_BAND,for_serving_cell,plmn))  
          {
            WRRC_MSG0_HIGH("Band mismatch:SIB6 has 850 band which is not supported by UE");
            return TRUE;
          }
          else
          {
            WRRC_MSG0_HIGH("Band match:SIB6 has 850 band which is supported by UE");
            return FALSE;
          }
        }

        /* Check for 1800 band mismatch */
        else if(sib6_ptr->v4b0NonCriticalExtensions.sysInfoType6_v4b0ext.frequencyBandIndicator ==
                  rrc_RadioFrequencyBandFDD_fdd1800)
        {
          if(!rrcsib_check_if_band_supported(RRC_CSP_WCDMA_1800_BAND,for_serving_cell,plmn))  
          {
            WRRC_MSG0_HIGH("Band mismatch:SIB6 has 1800 band which is not supported by UE");
            return TRUE;
          }
          else
          {
            WRRC_MSG0_HIGH("Band match:SIB6 has 1800 band which is supported by UE");
            return FALSE;
          }
        }

        /* Check for 1900 band mismatch */
        else if(sib6_ptr->v4b0NonCriticalExtensions.sysInfoType6_v4b0ext.frequencyBandIndicator ==
                  rrc_RadioFrequencyBandFDD_fdd1900)
        {
          if(!rrcsib_check_if_band_supported(RRC_CSP_PCS_BAND,for_serving_cell,plmn))  
          {
            WRRC_MSG0_HIGH("Band mismatch:SIB6 has 1900 band which is not supported by UE");
            return TRUE;
          }
          else
          {
            WRRC_MSG0_HIGH("Band match:SIB6 has 1900 band which is supported by UE");
            return FALSE;
          }
        }

        /* Check for 2100 band mismatch */
        else if(sib6_ptr->v4b0NonCriticalExtensions.sysInfoType6_v4b0ext.frequencyBandIndicator ==
                  rrc_RadioFrequencyBandFDD_fdd2100)
        {
          if(!rrcsib_check_if_band_supported(RRC_CSP_IMT2K_BAND,for_serving_cell,plmn))  
          {
            WRRC_MSG0_HIGH("Band mismatch:SIB6 has 2100 band which is not supported by UE");
            return TRUE;
          }
          else
          {
            WRRC_MSG0_HIGH("Band match:SIB6 has 2100 band which is supported by UE");
            return FALSE;
          }
        }

        /* Check for 1700_2100 AWS band mismatch */
        else if(sib6_ptr->v4b0NonCriticalExtensions.sysInfoType6_v4b0ext.frequencyBandIndicator ==
                  rrc_RadioFrequencyBandFDD_bandIV)
        {
          if(!rrcsib_check_if_band_supported(RRC_CSP_WCDMA_1700_2100_BAND,for_serving_cell,plmn))  
          {
            WRRC_MSG0_HIGH("Band mismatch:SIB6 has 1700_2100 AWS band which is not supported by UE");
            return TRUE;
          }
          else
          {
            WRRC_MSG0_HIGH("Band match:SIB6 has 1700_2100 AWS band which is supported by UE");
            return FALSE;
          }
        }
        else if(sib6_ptr->v4b0NonCriticalExtensions.sysInfoType6_v4b0ext.frequencyBandIndicator ==
                  rrc_RadioFrequencyBandFDD_extension_indicator)
        {
          fdd2_extension_present = TRUE;

          /* If the UE does not support any of the frequency bands that can be indicated 
           * by the IE "Frequency band indicator 2" (i.e., Band VIII or beyond), bar the frequency */
           if(!rrccsp_is_supported_band(RRC_CSP_WCDMA_900_BAND) && 
              !rrccsp_is_supported_band(RRC_CSP_WCDMA_1700_BAND) &&
              !rrccsp_is_supported_band(RRC_CSP_WCDMA_1500_BAND) &&
              !rrccsp_is_supported_band(RRC_CSP_WCDMA_BC19_BAND))
           {
             WRRC_MSG0_HIGH("Band mismatch:SIB6 has extension indicator but UE does not support any of FDD2 bands");
             return TRUE;
           }

        }
        else
        {
          WRRC_MSG0_HIGH("Band mismatch:band given in SIB6 is not supported by UE");
          return TRUE;
        }
      }

      /* Check FDD2 band field in SIB6 for mismatch */
      if((TRUE == fdd2_extension_present) &&
         (sib6_ptr != NULL) && 
         (RRC_MSG_COMMON_BITMASK_IE_PTR(sib6_ptr,rrc_SysInfoType6_, v4b0NonCriticalExtensions)) &&
         (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib6_ptr->v4b0NonCriticalExtensions), 
                        rrc_SysInfoType6_v4b0NonCriticalExtensions_, 
                        v590NonCriticalExtensions))&&
         (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions),
                        rrc_,v650nonCriticalExtensions)) &&
         (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650nonCriticalExtensions),
                        rrc_,sysInfoType6_v650ext))
        ) 
      {
        WRRC_MSG0_HIGH("FDD2 extensions present in SIB6");

        /* Check for 900 band mismatch*/
        if(sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.
           v650nonCriticalExtensions.sysInfoType6_v650ext.frequencyBandIndicator2 == rrc_RadioFrequencyBandFDD2_bandVIII)
        {
          if(!rrcsib_check_if_band_supported(RRC_CSP_WCDMA_900_BAND,for_serving_cell,plmn))  
          {
            WRRC_MSG0_HIGH("Band mismatch:SIB6 has 900 band which is not supported by UE");
            return TRUE;
          }
          else
          {
            WRRC_MSG0_HIGH("Band match:SIB6 has 900 band which is supported by UE");
            return FALSE;
          }
        }
        else
        /* Check for 900 band mismatch*/
        if(sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.
           v650nonCriticalExtensions.sysInfoType6_v650ext.frequencyBandIndicator2 == rrc_RadioFrequencyBandFDD2_bandIX)
        {
          if(!rrcsib_check_if_band_supported(RRC_CSP_WCDMA_1700_BAND,for_serving_cell,plmn))  
          {
            WRRC_MSG0_HIGH("Band mismatch:SIB6 has BC9 band which is not supported by UE");
            return TRUE;
          }
          else
          {
            WRRC_MSG0_HIGH("Band match:SIB6 has BC9 band which is supported by UE");
            return FALSE;
          }
        }
        else
        /* Check for 1500 band mismatch*/
        if(sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.
           v650nonCriticalExtensions.sysInfoType6_v650ext.frequencyBandIndicator2 == rrc_RadioFrequencyBandFDD2_bandXI)
        {
          if(!rrcsib_check_if_band_supported(RRC_CSP_WCDMA_1500_BAND,for_serving_cell,plmn))  
          {
            WRRC_MSG0_HIGH("Band mismatch:SIB5 has BC11 band which is not supported by UE");
            return TRUE;
          }
          else
          {
            WRRC_MSG0_HIGH("Band match:SIB5 has BC11 band which is supported by UE");
            return FALSE;
          }
        }
        else
        /* Check for BC19 band mismatch*/
        if(sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.
           v650nonCriticalExtensions.sysInfoType6_v650ext.frequencyBandIndicator2 == rrc_RadioFrequencyBandFDD2_bandXIX)
        {
          if(!rrcsib_check_if_band_supported(RRC_CSP_WCDMA_BC19_BAND,for_serving_cell,plmn))  
          {
            WRRC_MSG0_HIGH("BC19: Band mismatch:SIB5 has BC19 band which is not supported by UE");
            return TRUE;
          }
          else
          {
            WRRC_MSG0_HIGH("SIB19: Band match:SIB5 has BC19 band which is supported by UE");
            return FALSE;
          }
        }
        else
        {
          WRRC_MSG0_HIGH("Band mismatch:band given in SIB6 extension is not supported by UE");
          return TRUE;
      }  
      }
    }
  }

  fdd2_extension_present = FALSE;

  /* If we reach this point,it means that we could not use SIB6 to find the band mismatch.Try SIB5 */
  /* Get the SIB5 ptr from sibdb */
  sib5_ptr = (rrc_SysInfoType5 *)rrc_sibdb_return_sib_for_db_index
                                             ((rrc_SIB_type)rrc_SIB5,
                                               dbindex
                                             );
  /* Check if frequencyBandIndicator IE is present */
  if((sib5_ptr != NULL) &&
     (RRC_MSG_COMMON_BITMASK_IE_PTR(sib5_ptr, rrc_SysInfoType5_,v4b0NonCriticalExtensions)) &&
     (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib5_ptr->v4b0NonCriticalExtensions),
                  rrc_, sysInfoType5_v4b0ext)) &&
     (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib5_ptr->v4b0NonCriticalExtensions.sysInfoType5_v4b0ext),
                  rrc_SysInfoType5_v4b0ext_IEs_, frequencyBandIndicator))
   )
  {
    /* Check for 800 band mismatch*/
    if(sib5_ptr->v4b0NonCriticalExtensions.sysInfoType5_v4b0ext.frequencyBandIndicator ==
          rrc_RadioFrequencyBandFDD_bandVI)
    {
      if(!rrcsib_check_if_band_supported(RRC_CSP_WCDMA_800_BAND,for_serving_cell,plmn))  
      {
        /* This change will prevent the UE to camp on BC19 ARFCN when FBI indicates BC6. In general
         * it is expected that the NW will always configure BC6 as the FBI and ARFCN corresponding
         * to BC6 for the frequency range 875-885. Therefore to prevent intra-f HO from being treated
         * as inter-f HO this change is made to ensure that UE camps on BC6 ARFCN only. It is also always
         * expected that if BC19 is enabled BC6 is also supported by the UE
         */
        if(rrcsib_check_if_band_supported(RRC_CSP_WCDMA_BC19_BAND,for_serving_cell,plmn) && 
           !rrccsp_is_supported_band(RRC_CSP_WCDMA_800_BAND)
          )
        {
          /*
            As per 36.800, (R4-091781), Clause  5.5
            However, in order to support the existing legacy UE which has the capability for only Band VI, 
            the band indicator and the UARFCN must be set as "Band VI" and "UTRA Band XIX" in the DL frequency range
            of 875-885 MHz and 885-890 MHz, respectively in UTRAN 
          */
          WRRC_MSG0_HIGH("BC19: SIB5 has 800 band and UE supports BC19");
          return FALSE;
        }
        else
        {
          WRRC_MSG0_HIGH("Band mismatch:SIB5 has 800 band which is not the current band under scan");
          return TRUE;
        }
      }
      else
      {
        WRRC_MSG0_HIGH("Band match:SIB5 has 800 band which is supported by UE");
        return FALSE;
      }
    }

    /* Check for 850 band mismatch */
    else if(sib5_ptr->v4b0NonCriticalExtensions.sysInfoType5_v4b0ext.frequencyBandIndicator ==
              rrc_RadioFrequencyBandFDD_bandV)
    {
      if(!rrcsib_check_if_band_supported(RRC_CSP_WCDMA_850_BAND,for_serving_cell,plmn))  
      {
        WRRC_MSG0_HIGH("Band mismatch:SIB5 has 850 band which is not supported by UE");
        return TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("Band match:SIB5 has 850 band which is supported by UE");
        return FALSE;
      }
    }

    /* Check for 1800 band mismatch */
    else if(sib5_ptr->v4b0NonCriticalExtensions.sysInfoType5_v4b0ext.frequencyBandIndicator ==
              rrc_RadioFrequencyBandFDD_fdd1800)
    {
      if(!rrcsib_check_if_band_supported(RRC_CSP_WCDMA_1800_BAND,for_serving_cell,plmn))  
      {
        WRRC_MSG0_HIGH("Band mismatch:SIB5 has 1800 band which is not supported by UE");
        return TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("Band match:SIB5 has 1800 band which is supported by UE");
        return FALSE;
      }
    }

    /* Check for 1900 band mismatch */
    else if(sib5_ptr->v4b0NonCriticalExtensions.sysInfoType5_v4b0ext.frequencyBandIndicator ==
              rrc_RadioFrequencyBandFDD_fdd1900)
    {
      if(!rrcsib_check_if_band_supported(RRC_CSP_PCS_BAND,for_serving_cell,plmn))  
      {
        WRRC_MSG0_HIGH("Band mismatch:SIB5 has 1900 band which is not supported by UE");
        return TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("Band match:SIB5 has 1900 band which is supported by UE");
        return FALSE;
      }
    }

    /* Check for 2100 band mismatch */
    else if(sib5_ptr->v4b0NonCriticalExtensions.sysInfoType5_v4b0ext.frequencyBandIndicator ==
              rrc_RadioFrequencyBandFDD_fdd2100)
    {
      if(!rrcsib_check_if_band_supported(RRC_CSP_IMT2K_BAND,for_serving_cell,plmn))  
      {
        WRRC_MSG0_HIGH("Band mismatch:SIB5 has 2100 band which is not supported by UE");
        return TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("Band match:SIB5 has 2100 band which is supported by UE");
        return FALSE;
      }
    }

    /* Check for 1700_2100 AWS band mismatch */
    else if(sib5_ptr->v4b0NonCriticalExtensions.sysInfoType5_v4b0ext.frequencyBandIndicator ==
              rrc_RadioFrequencyBandFDD_bandIV)
    {
      if(!rrcsib_check_if_band_supported(RRC_CSP_WCDMA_1700_2100_BAND,for_serving_cell,plmn))  
      {
        WRRC_MSG0_HIGH("Band mismatch:SIB5 has 1700_2100 AWS band which is not supported by UE");
        return TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("Band match:SIB5 has 1700_2100 AWS band which is supported by UE");
        return FALSE;
      }
    }
    else if(sib5_ptr->v4b0NonCriticalExtensions.sysInfoType5_v4b0ext.frequencyBandIndicator ==
              rrc_RadioFrequencyBandFDD_extension_indicator)
    {
      fdd2_extension_present = TRUE;

      /* If the UE does not support any of the frequency bands that can be indicated 
       * by the IE "Frequency band indicator 2" (i.e., Band VIII or beyond), bar the frequency */
       if(!rrccsp_is_supported_band(RRC_CSP_WCDMA_900_BAND) && 
          !rrccsp_is_supported_band(RRC_CSP_WCDMA_1700_BAND) &&
          !rrccsp_is_supported_band(RRC_CSP_WCDMA_1500_BAND) &&
          !rrccsp_is_supported_band(RRC_CSP_WCDMA_BC19_BAND))
       {
         WRRC_MSG0_HIGH("Band mismatch:SIB5 has extension indicator but UE does not support any of FDD2 bands");
         return TRUE;
       }
    }
    else
    {
      WRRC_MSG0_HIGH("Band mismatch:band given in SIB5 has extension is not supported by UE");
      return TRUE;
    }
  }

  /* Check the FDD2 band field in SIB5 for mismatch */
  if((fdd2_extension_present == TRUE) &&
     (sib5_ptr != NULL) && 
     (RRC_MSG_COMMON_BITMASK_IE_PTR(sib5_ptr, rrc_SysInfoType5_, v4b0NonCriticalExtensions)) &&
     (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib5_ptr->v4b0NonCriticalExtensions), 
                      rrc_SysInfoType5_v4b0NonCriticalExtensions_,v590NonCriticalExtensions)) &&
     (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions),
                      rrc_, v650NonCriticalExtensions)) &&
     (RRC_MSG_COMMON_BITMASK_IE_PTR((&sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions),
                      rrc_, sysInfoType5_v650ext))
     ) 
  {
    WRRC_MSG0_HIGH("FDD2 extensions present in SIB5");

    /* Check for 900 band mismatch*/
    if(sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.
       sysInfoType5_v650ext.frequencyBandIndicator2 == rrc_RadioFrequencyBandFDD2_bandVIII)
    {
      if(!rrcsib_check_if_band_supported(RRC_CSP_WCDMA_900_BAND,for_serving_cell,plmn))  
      {
        WRRC_MSG0_HIGH("Band mismatch:SIB5 has 900 band which is not supported by UE");
        return TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("Band match:SIB5 has 900 band which is supported by UE");
        return FALSE;
      }
    }
    else
    /* Check for 900 band mismatch*/
    if(sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.
       sysInfoType5_v650ext.frequencyBandIndicator2 == rrc_RadioFrequencyBandFDD2_bandIX)
    {
      if(!rrcsib_check_if_band_supported(RRC_CSP_WCDMA_1700_BAND,for_serving_cell,plmn))  
      {
        WRRC_MSG0_HIGH("Band mismatch:SIB5 has BC9 band which is not supported by UE");
        return TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("Band match:SIB5 has BC9 band which is supported by UE");
        return FALSE;
      }
    }
    else
    /* Check for 1500 band mismatch*/
    if(sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.
       sysInfoType5_v650ext.frequencyBandIndicator2 == rrc_RadioFrequencyBandFDD2_bandXI)
    {
      if(!rrcsib_check_if_band_supported(RRC_CSP_WCDMA_1500_BAND,for_serving_cell,plmn))  
      {
        WRRC_MSG0_HIGH("Band mismatch:SIB5 has BC11 band which is not supported by UE");
        return TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("Band match:SIB5 has BC11 band which is supported by UE");
        return FALSE;
      }
    }
    else
    /* Check for BC19 band mismatch*/
    if(sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.
       sysInfoType5_v650ext.frequencyBandIndicator2 == rrc_RadioFrequencyBandFDD2_bandXIX)
    {
      if(!rrcsib_check_if_band_supported(RRC_CSP_WCDMA_BC19_BAND,for_serving_cell,plmn))  
      {
        WRRC_MSG0_HIGH("Band mismatch:SIB5 has BC19 band which is not supported by UE");
        return TRUE;
      }
      else
      {
        WRRC_MSG0_HIGH("Band match:SIB5 has BC19 band which is supported by UE");
        return FALSE;
      }
    }
    else
    {
      WRRC_MSG0_HIGH("Band mismatch:band given in SIB5 extension is not supported by UE");
      return TRUE;
    }
  }
  /* Frequency Band Indicator not present*/
  {
    rrc_csp_band_class_type    band_class;
    boolean band_mismatch = FALSE;

    if(for_serving_cell)
    {
      band_class = rrc_get_frequency_band(
                              rrccsp_get_serving_frequency(), rrccsp_get_curr_nas_requested_plmn());
    }
    else
    {
      band_class = rrc_get_frequency_band(
                              rrccsp_get_neighbor_frequency(), rrccsp_get_curr_nas_requested_plmn());
    }

    if((band_class == RRC_CSP_IMT2K_BAND) || (band_class == RRC_CSP_WCDMA_1800_BAND))
    {
      if(rrccsp_is_supported_band(band_class))
      {
        WRRC_MSG1_HIGH("No band mismatch, Allow camping on band :%d",band_class);
        band_mismatch = FALSE;
      }
      else
      {
        /* Frequency should be barred according to spec */
        WRRC_MSG1_HIGH("Band mismatch, Don't allow camping on band :%d",band_class);
        band_mismatch = TRUE;
      }
    }

    else if(band_class == RRC_CSP_PCS_BAND)
    {
      WRRC_MSG1_HIGH("No band mismatch, Allow camping on band :%d",band_class);
      band_mismatch = FALSE;
    }

    else if((band_class == RRC_CSP_WCDMA_1700_2100_BAND)
    || (band_class == RRC_CSP_WCDMA_1700_BAND)
    || (band_class == RRC_CSP_WCDMA_BC19_BAND))
    {
      WRRC_MSG1_HIGH("Band indicator is not present. Bar the frequency:%d",band_class);
      band_mismatch = TRUE;
    }

    else if((band_class == RRC_CSP_WCDMA_850_BAND)
    || (band_class == RRC_CSP_WCDMA_800_BAND)
    || (band_class == RRC_CSP_WCDMA_900_BAND)
    || (band_class == RRC_CSP_WCDMA_1500_BAND))
    {
      /* UE behavior is unspecified */
      WRRC_MSG1_ERROR("Band indicator is not present. Lets allow camping on band :%d",band_class);
      band_mismatch = FALSE;
    }

    else
    {
      WRRC_MSG1_HIGH("Band not supported, Don't allow camping on band :%d",band_class);
      band_mismatch = TRUE;
    }

    return band_mismatch;
  }
}
#endif
/*===========================================================================

FUNCTION RRC_BPLMN_RETURN_SIB_FOR_CELL

DESCRIPTION

  This function returns a pointer to the requested SIB.
  if this particular SIB has not been read completely for that cell, 
  this function returns NULL.
  
  The pointer is typecast to void since different sibs have
  different type definitions. The calling function is responsible
  for typecasting the pointer to the required SIB type.
   
DEPENDENCIES

  NONE

RETURN VALUE

  Pointer to the requested SIB. 
  "NULL" if the requested SIB has not been read.
  

SIDE EFFECTS

  None.

===========================================================================*/
void * rrc_bplmn_return_sib_for_cell
(
  rrc_SIB_type  req_sib          /* The SIB that is required */
)
{
  void *ptr = NULL; /* Local ptr */

  /* We only store MIB,SIB1 and SIB3 for WTW BPLMN searches */
  switch(req_sib)
  {
    case rrc_MIB:
      ptr = (void *)rrcsib_bplmn_sibs.mib; 
      break;

    case rrc_SIB1:
      ptr = (void *)rrcsib_bplmn_sibs.sib1; 
      break;

    case rrc_SIB3:
      ptr = (void *)rrcsib_bplmn_sibs.sib3; 
      break;

    default:
      WRRC_MSG1_ERROR("Invalid SIB query for sib_type %d",rrcsib_map_int_sibtype_to_asn1_sibtype(req_sib));
      break;
  }

  return ptr;
}
/*===========================================================================

FUNCTION RRCSIB_BPLMN_INIT_PREV_FREQ_PSC

DESCRIPTION

  This function intializes previously scanned Freq and PSC during BPLMN search to 0.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcsib_bplmn_init_prev_freq_psc
(
  void
)
{
  rrcsib_bplmn_sibs.prev_freq = 0;
  rrcsib_bplmn_sibs.prev_scr = 0;
}

/*===========================================================================

FUNCTION RRCSIB_FREE_BPLMN_SIBS

DESCRIPTION

  This function frees the memory used by a System Information
  Block stored in the BLOMN SIB database using the ASN.1 free pdu function.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcsib_free_bplmn_sibs
(
  void
)
{
  if( rrcsib_bplmn_sibs.mib != NULL )
  {
    (void)rrcasn1_free_pdu(rrc_MasterInformationBlock_PDU, rrcsib_bplmn_sibs.mib );
  }

  if( rrcsib_bplmn_sibs.sib1 != NULL )
  {
    (void)rrcasn1_free_pdu( rrc_SysInfoType1_PDU, rrcsib_bplmn_sibs.sib1 );
  }

  if( rrcsib_bplmn_sibs.sib3 != NULL )
  {
    (void)rrcasn1_free_pdu( rrc_SysInfoType3_PDU, rrcsib_bplmn_sibs.sib3 );
  }

#ifdef FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
  if( rrcsib_bplmn_sibs.sb1!= NULL )
  {
    (void)rrcasn1_free_pdu( rrc_SysInfoTypeSB1_PDU, rrcsib_bplmn_sibs.sb1);
  }
  if( rrcsib_bplmn_sibs.sb2 != NULL )
  {
    (void)rrcasn1_free_pdu( rrc_SysInfoTypeSB2_PDU, rrcsib_bplmn_sibs.sb2);
  }
#endif
  rrcsib_bplmn_sibs.mib = NULL;
  rrcsib_bplmn_sibs.sib1 = NULL;
  rrcsib_bplmn_sibs.sib3 = NULL;
#ifdef FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
  rrcsib_bplmn_sibs.sb1 = NULL;
  rrcsib_bplmn_sibs.sb2 = NULL;
#endif
}

#if defined(FEATURE_WCDMA_DRX_MANAGER)

/*===========================================================================

FUNCTION RRCSIB_INITIALIZE_SIB_SLEEP_SCHEDULE_TABLE

DESCRIPTION

   This function initializes the SIB schedule table  
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
     
===========================================================================*/
void rrcsib_initialize_sib_sleep_schedule_table
(
  void
)
{
  /* Initialise to all 1s so that L1 will stay awake initially */
  memset(sib_sleep_schedule_table_0,0xFF,RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE);
  memset(sib_sleep_schedule_table_1,0xFF,RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE);
  sib_sleep_schedule_table_in_use = 0;
  WRRC_MSG0_HIGH("SIB_Sleep: Initialized SIB Sleep Schedule Tables to all 1s");
}



/*===========================================================================

FUNCTION RRCSIB_SET_BIT_POS_BEFORE_SB_POS

DESCRIPTION

   This function sets the bit position in the SIB sleep table.
   It sets the bit position before the SB based on the number of bits to set
   
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
     
===========================================================================*/
void rrcsib_set_bit_pos_before_sb_pos(uint32 num_bits_req, uint16 array_idx, uint8 *table_ptr)
{
  uint8 max_idx = 0;
  uint8 loop_idx = 0;
  uint32 bit_pos_val = 0;
  uint16 table_idx = 0;
  uint32 shift_pos_val = 0;

  if(num_bits_req)
  {
    /* Getting the value of maximum times the loop should run
       i.e indicates the num of bytes to fill */
    /* Dividing by 8 to get the value for which the loop should run
       to fill the sib sleep table */
    max_idx = ((num_bits_req - 1)/8) + 1;

    /* Storing the value to be set in sleep table */
    shift_pos_val = (1 <<num_bits_req ) - 1;

    for(loop_idx = 0; loop_idx < max_idx ; loop_idx++)
    {
      /* Extracting the 8 bits required 
         Eg : if num of bits to be set is 13
         for idx = 0, first 8 bits will be considered 
         for idx = 1, extracting the next 8 bits and shifting the first 8 bits as its already been set */

      bit_pos_val = (shift_pos_val) >> (loop_idx * 8); 

      /* Finding the table_idx and storing it */
      /* If the array index is 2 and number of bytes to fill is 4 
         then we need to fill the indexes 0,1,511,510 */
      if(array_idx >= (1+loop_idx))
      {
        table_idx = (array_idx - 1 - loop_idx);
      }
      else
      {
        table_idx =  (RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE - 1 - loop_idx);
      }

      /* To make sure setting the 8 bits */ 
      table_ptr[table_idx] |= (bit_pos_val) & 0xFF;
    }
  }
}



/*===========================================================================

FUNCTION RRCSIB_UPDATE_SIB_SLEEP_SCHEDULE_TABLE

DESCRIPTION

  This function updates the SIB schedule table  
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
     
===========================================================================*/
void rrcsib_update_sib_sleep_schedule_table
(
  sib_events_index_e_type event_index   /* Index of the current event */
)
{
  uint32 count = 0;
  uint32 seg_count = 0;
  uint32 loop_count = 0;
  uint32 memcpy_count = 0;
  uint32 bytes_to_copy =0;
  uint8 * table_ptr = NULL;
  uint8 work_table;
  uint32 rep_count;

  /* Calculates how much offset should be sent as arguement */
  uint8 bit_offset = 0;

#ifdef FEATURE_DUAL_SIM
  boolean all_sibs_received = TRUE;
  boolean event_status = FALSE;


  if((sib_events_ptr->event[event_index].sib_db_index_found == TRUE)&&
     (sib_events_ptr->event[event_index].sib_db_index < SIBDB_INVALID_INDEX ))
  {
    event_status = check_event_completion_status(event_index,rrcsib_can_nw_support_deferred_sib(sib_events_ptr->event[event_index].sib_db_index));
  }

#endif

  if(EVENT_INDEX_VALID(event_index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return;
  }

  if(sib_sleep_schedule_table_in_use == 0)
  {
    table_ptr = sib_sleep_schedule_table_1;
    work_table = 1;
  }
  else
  {
    table_ptr = sib_sleep_schedule_table_0;
    work_table = 0;
  }

  memset(table_ptr,0x00,RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE);   

  /* Loop through the all the SIB(segments) and update the sleep schedule table based on 
     the SIB Schedule */
  for(count = 1; count < RRC_TOT_SIBS; count++)
  {
    /* Include the schedule only if the SIB is needed & the SIB has a valid schedule 
           and if it has not been decoded already */
    if((sib_events_ptr->event[event_index].tsib[count].sib_needed == TRUE) &&
       (sib_events_ptr->event[event_index].tsib[count].sib_received == FALSE)&&
       (sib_events_ptr->sib_sched[count].repitition_rate != INVALID_SIB_SCHEDULE_REP)
      )
    {

  #ifdef FEATURE_DUAL_SIM
      all_sibs_received = FALSE;
  #endif
  
      /* Calculate the rep count to be filled in the loop */
      rep_count = sib_events_ptr->scheduling_max_rept_rate/sib_events_ptr->sib_sched[count].repitition_rate;
      
      for (seg_count = 0; seg_count < sib_events_ptr->sib_sched[count].num_segments; seg_count++)
      {
        uint32 sfn = sib_events_ptr->sib_sched[count].sfn_offset[seg_count]; /* Local copy of SFN */
        uint32 byte_pos = sfn>>3;                /* (sfn/8) Gives the byte location in sib sleep schedule table */
        uint8 byte_offset = (uint8)(sfn & 0x07); /* (sfn%8)Gives the offset within the byte in sib sleep table */
        uint8 bit_mask = 0xFF;

        bit_mask = rrcsib_gen_bit_mask_for_schedule_table(byte_offset);
        /* Take into account the repetition rate and set the repetition masks also.Do it only till max rep rate
            since we would do a memcpy for rest of the table.Apply the calculated bitmask for the 
            byte till the max repetition cycle */
        for(loop_count = 0; loop_count < rep_count; loop_count++)
        {
          uint32 rep_byte_pos = byte_pos + (loop_count * (sib_events_ptr->sib_sched[count].repitition_rate/8));

          /*rep_byte_pos should never be greather than RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE 
            If for some reasons this happens, disable SIB sleep*/
          if(rep_byte_pos >= RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE)
          {
            memset(table_ptr,0xFF,RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE);
            rrcsib_send_sib_sleep_table(table_ptr);
            sib_sleep_schedule_table_in_use = work_table;
            return;
          }
          table_ptr[rep_byte_pos] |= bit_mask;

          /* Optimization to set a few extra 1s after last segment of Scheduling blocks
             Add 4 1s if this segment in the last segment of SB1 or SB2. this is to keep 
             L1 awake after the last SB segment. SB can have schedule for some SIBs and L1
             needs to stay awake to process the new schedule from SB    */
          if(((count == (uint32)rrc_SB1) ||(count == (uint32)rrc_SB2)) &&
              (seg_count == (uint32)(sib_events_ptr->sib_sched[count].num_segments-1)))
          {
            uint8 curr_pos_bit_mask = 0x00;
            uint8 next_pos_bit_mask = 0x00;
            uint32 pos_bit_shift = (1 <<MAX_SFN_BEFORE_SB_SLEEP ) - 1;

            switch(bit_mask)
            {
              case 0xC0:
                /* Actual bitmask : 1100 0000. 
                   Modified bitmask with 4 dummy ones: 1111 1100 (0xFC) */
                curr_pos_bit_mask = 0xFC; 
                break;

              case 0x30:
                /* Actual bitmask : 0011 0000. 
                   Modified bitmask with 4 dummy ones: 0011 1111 (0x3F) */
                curr_pos_bit_mask = 0x3F; 

                if(MAX_SFN_BEFORE_SB_SLEEP)
                {
                  /* Making sure to set the bits in the curr_pos_bit_mask */
                  /* Since the actual bitmask is 0011 0000, if number of bits to be set is 4 
                     We need to set 2 bits in the current index and remaining 2 bits in the previous
                     index */
                  curr_pos_bit_mask|= (( pos_bit_shift& 0x3 ) << 6);
                }

                if(MAX_SFN_BEFORE_SB_SLEEP > 2)
                {
                  /* Subtracting the number of bits since we have set this before */
                  bit_offset = MAX_SFN_BEFORE_SB_SLEEP - 2;
                }
                break;

              case 0x0C:
                /* Actual bitmask : 0000 1100. 
                   Modified bitmask with 4 dummy ones: 0000 1111 1100 0000(0x0F,0xC0) */
                curr_pos_bit_mask = 0x0F; 
                next_pos_bit_mask = 0xC0;

                if(MAX_SFN_BEFORE_SB_SLEEP)
                { 
                  curr_pos_bit_mask|= ((pos_bit_shift & 0xF ) << 4);
                }

                if(MAX_SFN_BEFORE_SB_SLEEP > 4)
                {
                  bit_offset = MAX_SFN_BEFORE_SB_SLEEP - 4;
                }
                break;

              case 0x03:
                /* Actual bitmask : 0000 0011. 
                   Modified bitmask with 4 dummy ones: 0000 0011 1111 0000(0x03,0xF0) */
                next_pos_bit_mask = 0xF0;

                if(MAX_SFN_BEFORE_SB_SLEEP)
                {
                  curr_pos_bit_mask|= ((pos_bit_shift & 0x3F) << 2);
                }

                if(MAX_SFN_BEFORE_SB_SLEEP > 6)
                {
                  bit_offset = MAX_SFN_BEFORE_SB_SLEEP - 6;
                }
                break;

              default:
                /* Actual bitmask : 1111 1111. 
                   Modified bitmask with 4 dummy ones: 1111 1111 1111 0000(0xFF,0xF0) */
                next_pos_bit_mask = 0xF0;
                break;
            }

            /* This function is used to set the nuber of bits to be set in the 
               sib sleep table. This is used for filling the previous table index */
            if(bit_offset)
            { 
              rrcsib_set_bit_pos_before_sb_pos(bit_offset, rep_byte_pos, table_ptr);
            }

            /* Modify the current pos bitmask */
            table_ptr[rep_byte_pos] |= curr_pos_bit_mask;

            /* Set the next byte. Make sure you are not crossing the array boundary */
            if(rep_byte_pos < (RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE -1))  
            {
              /* Set the 4 frames(40ms) in the next byte to all 1s so that L1 will stay awake after SB */
              table_ptr[rep_byte_pos+1] |= next_pos_bit_mask;
            }
            else //wraparound case
            {
              table_ptr[0] |= next_pos_bit_mask;
            }
          }  
        }
      }
    }
  }

  /* Now we have filled the table for max repetition rate bytes.Fill the rest of the table by memcpy */
  //memcpy_count = RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE/sib_events_ptr->scheduling_max_rept_rate;
  memcpy_count = 4096/(sib_events_ptr->scheduling_max_rept_rate);
  bytes_to_copy = sib_events_ptr->scheduling_max_rept_rate/8;

  for(count = 1; count < memcpy_count ; count++)
  {
    uint32 index = count * bytes_to_copy;
    WCDMA_MEMCPY(table_ptr+index,
                 sizeof(uint8) * (RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE - index),
                 table_ptr,
                 bytes_to_copy);
  }

#ifdef FEATURE_DUAL_SIM
  if((g_in_data_call)&&(!rrcsib_stop_partition))
  {
    WRRC_MSG1_HIGH("Introduce holes %d",sib_events_ptr->event[event_index].int_event.int_event_type);

    if(!((sib_events_ptr->event[event_index].req_proc == RRC_PROCEDURE_SIB) &&
       (sib_events_ptr->event[event_index].int_event.int_event_type == RRCSIB_INT_SIB7_EVENT)))
    {
      rrcsib_intro_holes_in_table(table_ptr,RRCSIB_SLEEP_SFN,RRCSIB_WAKE_SFN);
    }
  }

  /*Avoid sending NULL mask for all SIBs received and event completion cases
    In case of DMCR, all SIBs received will be FALSE but event completion will be true*/
  if((!all_sibs_received)&&(!event_status))
#endif
  {
    rrcsib_send_sib_sleep_table(table_ptr);
  }
#ifdef FEATURE_DUAL_SIM
  else
  {
    return;
  }
#endif

  sib_sleep_schedule_table_in_use = work_table;


  for(count = 0; count < (sib_events_ptr->scheduling_max_rept_rate/8);count=count+8)
  {
     WRRC_MSG8_HIGH("SIB_Sleep: Table Contents byte = %0X,%0X,%0X,%0X,%0X,%0X,%0X,%0X",
                   table_ptr[count],table_ptr[count+1],table_ptr[count+2],table_ptr[count+3],
                   table_ptr[count+4],table_ptr[count+5],table_ptr[count+6],table_ptr[count+7]);

  }
}

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION RRCSIB_UPDATE_SIB_SLEEP_SCHEDULE_TABLE

DESCRIPTION

  This function updates the SIB schedule table  
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
     
===========================================================================*/
void rrcsib_send_empty_table
(
  sib_events_index_e_type event_index,
  boolean is_empty
)
{
  uint8 * table_ptr = NULL;
  uint8 work_table;
  uint32 count;

  if(EVENT_INDEX_VALID(event_index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return;
  }

  WRRC_MSG1_HIGH("Sending empty table %d",is_empty);

  if(sib_sleep_schedule_table_in_use == 0)
  {
    table_ptr = sib_sleep_schedule_table_1;
    work_table = 1;
  }
  else
  {
    table_ptr = sib_sleep_schedule_table_0;
    work_table = 0;
  }

  if(is_empty)
  {
    memset(table_ptr,0x00,RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE);   
  }
  else
  {
    memset(table_ptr,0xFF,RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE);

    rrcsib_intro_holes_in_table(table_ptr,RRCSIB_SLEEP_SFN,RRCSIB_WAKE_SFN);

  }

  for(count = 0; count < 20;count=count+8)
  {
    WRRC_MSG8_HIGH("SIB_Sleep: Table Contents byte = %0X,%0X,%0X,%0X,%0X,%0X,%0X,%0X",
                   table_ptr[count],table_ptr[count+1],table_ptr[count+2],table_ptr[count+3],
                   table_ptr[count+4],table_ptr[count+5],table_ptr[count+6],table_ptr[count+7]);
  }
  rrcsib_send_sib_sleep_table(table_ptr);

  sib_sleep_schedule_table_in_use = work_table;

}
#endif

/*===========================================================================

FUNCTION RRCSIB_GET_SIB_SLEEP_SCHEDULE_TABLE_LOCATION

DESCRIPTION

  This function returns the location to the sib sleep table.This 
  would be passed to L1 in the CPHY_SETUP_REQ for N-BCCH 
  
DEPENDENCIES
 
  None.

RETURN VALUE

  Pointer to the table

SIDE EFFECTS

  None.
     
===========================================================================*/
uint8* rrcsib_get_sib_sleep_schedule_table_location
(
  void
)
{
  rrcsib_initialize_sib_sleep_schedule_table();
  return sib_sleep_schedule_table_0;
}
#endif
/*===========================================================================

FUNCTION  rrcsib_update_sixhr_sib_timer

DESCRIPTION

  This function will Implement the required six hours 
  Timer for each of the valid sib index.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
     
===========================================================================*/
void rrcsib_update_sixhr_sib_timer(void)
{

    /*Call the function which will update the counter
       vaule for each active cell index & restart the 
       10 minTimer*/
    rrc_sibdb_update_six_hr_timer();

  if((sibdb.active_cell_sibs_valid  == TRUE)&&
      (sibdb.cell[sibdb.active_cell_index].six_hr_count == MAX_CELL_INDEX_COUNT_SIXHR)&&
      (rrcmcm_get_rrc_mode() != RRC_MODE_INACTIVE)
    )
  {
    rrc_cmd_type *rrc_cmd_ptr;            /* Pointer to the RRC Command */
    rrc_MasterInformationBlock * mib_ptr;  /*Pointer to MIB*/
    boolean sib_active;
    rrc_state_e_type rrc_state;

    rrc_state = rrc_get_state();
    sib_active = rrcsib_is_sib_active();
    /*Update sibs due to SFN drift has low priority than six hrs sib, 
        So check for it, If yes let six hrs sib be posted*/
    if(sib_active == TRUE)
    {
      if(rrcsib_is_current_event_due_to_sfn_drift() == TRUE)
      {
        sib_active = FALSE;
      }
    }
    /*Currently six hrs Update_sibs is supported only in DISCONNECTED state.
       six hrs Update_sib will be posted only is all following cases are satisfied
       1) None of the sib event should be active
       2) RCE procedure should not be active, as UE will be moving to CONNECTING state.
       3) sib_change_safe_guard should not be set to TRUE which make sure that when procedures
           are acting on sib_change_ind,Differ posting of six hrs Update_sibs.
       4) BPLMN Should not be started.
       5) SIB should not have any pending command.
    */
    if((sib_active == FALSE)&&
        (rrc_state == RRC_STATE_DISCONNECTED) &&
        (rrcsib_cmd_store.pending_flag == FALSE)&&
        (WTOW_SEARCH_STOP == rrc_csp_bplmn_srch_in_progress())&&
        (rrcrce_is_rce_procedure_active() == FALSE)&&
        (rrcpg1_return_bcch_mod_info_status() == FALSE)
      )
    {
      /* Build RRC_UPDATE_SIBS_IND command to post to SIB */
      /* Allocates the buffer to RRC Internal command */
      if( (rrc_cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
      {     
        /* Fill in the command parameters for RRC_UPDATE_SIBS_IND */
        rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_UPDATE_SIBS_IND;
  
        if((mib_ptr= (rrc_MasterInformationBlock *)  rrc_sibdb_return_sib_for_srv_cell
           (rrc_MIB)) != NULL)
        {
          rrc_cmd_ptr->cmd.update_sibs_ind.mib_value_tag = mib_ptr->mib_ValueTag;
  
          WRRC_MSG2_HIGH("mib-vtag is %d sfn is %d ***", rrc_cmd_ptr->cmd.update_sibs_ind.mib_value_tag, 
                   seq_get_sfn());
  
          rrc_cmd_ptr->cmd.update_sibs_ind.procedure = RRC_PROCEDURE_SIB;
          rrc_cmd_ptr->cmd.update_sibs_ind.cause = RRCSIB_SIX_HR_TMR_EXPIRY;
          rrc_cmd_ptr->cmd.update_sibs_ind.real_sib_mod = FALSE;
          WRRC_MSG0_HIGH( "6hrs sib:6hrs sib timer expired for serving cell Post RRC_UPDATE_SIBS_IND cmd ");
  
          rrc_put_int_cmd(rrc_cmd_ptr);   
        }
      } 
    }
  }
    
}

/*===========================================================================

FUNCTION rrcsib_is_need_to_cleanup_cell

DESCRIPTION

  This function will check whether the six hours Criteria needs 
  to be checked for this sib event.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If six hours Criteria needs to be checked 
  FALSE: Other wise

SIDE EFFECTS
  None.
     
===========================================================================*/
boolean rrcsib_is_need_to_cleanup_cell(  sib_events_index_e_type  event_index)
{

  if(EVENT_INDEX_VALID(event_index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return FALSE;
  }
  
  if((sib_events_ptr->event[event_index].event_name == RRCSIB_GET_SPECIFIC_SIBS) &&
      (sib_events_ptr->event[event_index].sib_scan_type == RRC_SIB_NORMAL) &&
      (sib_events_ptr->event[event_index].req_proc == RRC_PROCEDURE_CSP))
  {
    WRRC_MSG0_HIGH("6hr sib:For this event check six hr Criteria if Index found in SIB db");
    return TRUE;
  }
  else
  {
     WRRC_MSG0_HIGH("6hr sib:No need to check six hr Criteria for this SIB event");
    return FALSE;
  }
}

/*===========================================================================

FUNCTION rrcsib_is_event_sixhrs_sib_update

DESCRIPTION

  This function will check whether the event is UPDATE_SIBs
  due to six hrs sib timer expiry
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If this event is UPDATE_SIBs due to six hrs sib timer expiry
  FALSE: Other wise

SIDE EFFECTS
  None.
     
===========================================================================*/
boolean rrcsib_is_event_sixhrs_sib_update(  sib_events_index_e_type  event_index)
{
  boolean status = FALSE;

  if(EVENT_INDEX_VALID(event_index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return FALSE;
  }
  
  if(sib_events_ptr->event[event_index].event_name == RRCSIB_UPDATE_SIBS )
  {
    if(sibdb.active_cell_sibs_valid == TRUE)
    {
       if(sib_events_ptr->update_sibs_cause == RRCSIB_SIX_HR_TMR_EXPIRY)
        {
          WRRC_MSG0_HIGH("6hr sib:This event is six hrs update_sibs event");
          status = TRUE;
        }
    }
  }
  return status;
}
/*===========================================================================

FUNCTION rrcsib_is_sixhrs_sib_timer_expired

DESCRIPTION

  This function will check whether the event is UPDATE_SIBs
  return TRUE if six hrs sib timer has expired for serving cell
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If six hrs sib timer has expired for serving cell
  FALSE: Other wise

SIDE EFFECTS
  None.
     
===========================================================================*/
boolean rrcsib_is_sixhrs_sib_timer_expired(  sib_events_index_e_type  event_index)
{
  boolean status = FALSE; 

  if(EVENT_INDEX_VALID(event_index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return FALSE;
  }
  
  if((sib_events_ptr->event[event_index].event_name == RRCSIB_UPDATE_SIBS )&&
      (sibdb.active_cell_sibs_valid == TRUE)&&
      (sibdb.cell[sibdb.active_cell_index].six_hr_count  == MAX_CELL_INDEX_COUNT_SIXHR)&&
      (rrc_get_state() == RRC_STATE_DISCONNECTED)
    )
  {
    WRRC_MSG0_HIGH("Six hours SIB timer for serving cell expired");
    status = TRUE;
  }
  return status;
}
/*===========================================================================

FUNCTION rrcsib_is_current_event_due_to_sfn_drift

DESCRIPTION

  This function will check whether the event is UPDATE_SIBs
  due to SFN drift
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: UPDATE_SIBs is due to SFN drift
  FALSE: Other wise

SIDE EFFECTS
  None.
     
===========================================================================*/
boolean rrcsib_is_current_event_due_to_sfn_drift(void)
{
  uint8 count;
  boolean status = FALSE;
  for( count = 0; count < (uint32)MAX_SIB_EVENTS_INDEX; count++ )
  {
    if( sib_events_ptr->event[count].event_active == TRUE )
    {
      if(sib_events_ptr->event[count].event_name == RRCSIB_UPDATE_SIBS)
      {
        if(sib_events_ptr->update_sibs_cause == RRCSIB_SFN_UPDATE)
        {
          status = TRUE;
          break;
        }
      }
    }/* End if active events */
  }
  return status;
}
#ifdef FEATURE_UPDATE_SIB7_IN_FACH
/*===========================================================================
FUNCTION rrcsib_can_sib7_event_start
DESCRIPTION
  This function will check whether the sib-7 event be started.
  condition for it is UE must be in FACH state & none of other 
  sib events are active.
DEPENDENCIES
  None.
RETURN VALUE
  TRUE: If sib-7 event can be started.
  FALSE: Other wise
SIDE EFFECTS
  None.
===========================================================================*/
boolean rrcsib_can_sib7_event_start(void)
{
  boolean start_sib7_event=FALSE;
  if(rrc_get_state() == RRC_STATE_CELL_FACH)
  {
    rrcllc_oc_set_status_e_type oc_set_status;
    oc_set_status = rrcllc_get_ordered_config_status();
    if(!((OC_SET_FOR_CELL_DCH == oc_set_status)||
           (OC_SET_FOR_URA_PCH == oc_set_status)||
           (OC_SET_FOR_CELL_PCH == oc_set_status)||
           (OC_SET_FOR_FACH_CELL_PCH_TRANS == oc_set_status)||
           (OC_SET_FOR_FACH_URA_PCH_TRANS == oc_set_status) ||
           (TRUE == rrccu_check_if_cu_ordered_state_is_pch_and_oc_is_not_set())
          )
      )
    {
      if((rrcsib_is_sib_active() == FALSE)&&
          (sibdb.active_cell_sibs_valid == TRUE)&&
          (RRC_CAMPED_ON == rrc_ccm_get_curr_camping_status(RRC_PROCEDURE_SIB))
        )
      {
#ifdef FEATURE_WCDMA_HS_RACH
        if(rrcsib_update_sib7_hs_rach() == TRUE)
#endif
        {
        WRRC_MSG0_HIGH("sib7:Start sib-7 event in FACH state");
        start_sib7_event=TRUE;
      }
#ifdef FEATURE_WCDMA_HS_RACH
        else
        {
          WRRC_MSG0_HIGH("HSRACH:SIB7 event not started");
        }
#endif
      }
      else
      {
        WRRC_MSG0_HIGH("sib7:Ignore this attempt to start sib7 as sib event is active");
      }
    }
    else
    {
      WRRC_MSG1_HIGH("sib7:Ignore this attempt to start sib7 event in FACH as OC is set=%d",oc_set_status);
    }
  }
  return start_sib7_event;
}
/*===========================================================================

FUNCTION rrcsib_build_sib7_schedule_table

DESCRIPTION

  This function updates the SIB schedule table
  for SIB-7 only.
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
     
===========================================================================*/
void rrcsib_build_sib7_schedule_table
(
  rrc_SIB_type rrc_sib_type,                /* Indicates whether this update is from MIB or SBs */
  sib_events_index_e_type event_index   /* Index of the current event */
#ifdef FEATURE_WCDMA_HS_FACH_DRX
  ,uint32 skip_sfn
#endif
)
{
  uint32 count = 0;
  uint32 seg_count = 0;
  uint32 loop_count = 0;
  uint8 * table_ptr = NULL;
  uint32 rep_count;
#ifdef FEATURE_WCDMA_HS_FACH_DRX
  uint16 start_count=0;
  uint16 stop_count=0;
#endif

  if(EVENT_INDEX_VALID(event_index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return;
  }

  WRRC_MSG2_HIGH("SIB_Sleep:Updating SIB Sleep Schedule Table for SIB :%d Current table in use:%d",
            rrcsib_map_int_sibtype_to_asn1_sibtype(rrc_sib_type),sib_sleep_schedule_table_in_use);

  if(rrc_sib_is_current_event_sib7_in_fach() ==TRUE)
  {
    table_ptr = sib_sleep_schedule_table_0;

    memset(table_ptr,0x00,RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE);   
  
    /* Loop through the all the SIB(segments) and update the sleep schedule table based on 
       the SIB Schedule */
    for(count = 1; count < RRC_TOT_SIBS; count++)
    {
      /* Include the schedule only if the SIB is needed & the SIB has a valid schedule 
         and if it has not been decoded already */
      if((sib_events_ptr->event[event_index].tsib[count].sib_needed == TRUE) &&
         (sib_events_ptr->event[event_index].tsib[count].sib_decoded == FALSE) &&
         (sib_events_ptr->sib_sched[count].repitition_rate != INVALID_SIB_SCHEDULE_REP)) 
      {
        /* rep_count - keeps track of number of SIB7 occurences in one frame(4096 sfns) */
        /* Lets take an example. SIB7: rep_rate:64, offset:2 */
        /* rep_count = 4096/64 = 64 SIB7 occurences in one frame */ 
        rep_count = 4096/sib_events_ptr->sib_sched[count].repitition_rate;
#ifdef FEATURE_WCDMA_HS_FACH_DRX
        
        /* Holds current occurence of SIB7. Lets assume curr_sfn=1. */
        /* start_count = 0 (means we just now received first occurence of SIB7) */
        start_count = (curr_sfn*2)/sib_events_ptr->sib_sched[count].repitition_rate;

         /* start_count + No of SIB7 to skip. */
         /* No of SIB7 to skip =  800/64 + 1 = 13. Means 13 SIB7 occurences in 8s */
        stop_count = start_count + (skip_sfn/sib_events_ptr->sib_sched[count].repitition_rate)+1;

        /* To take care of roll over case. 
           If start_count = 60 then stop_count = 73%64 = 9
           for start_count=0 stop_count = 13%64 = 13 */
        stop_count = stop_count%rep_count;
#endif

        for (seg_count = 0; seg_count < sib_events_ptr->sib_sched[count].num_segments; seg_count++)
        {
          uint32 sfn = sib_events_ptr->sib_sched[count].sfn_offset[seg_count]; /* Local copy of SFN */
          uint32 byte_pos = sfn>>3;                /* (sfn/8) Gives the byte location in sib sleep schedule table */
          uint8 byte_offset = (uint8)(sfn & 0x07); /* (sfn%8)Gives the offset within the byte in sib sleep table */
          uint8 bit_mask = 0xFF;
          boolean is_sib7_rep_less_then_32 = FALSE; 
   
          bit_mask = rrcsib_gen_bit_mask_for_schedule_table(byte_offset);
          if(count == rrc_SIB7)
          {
            if(sib_events_ptr->sib_sched[count].repitition_rate < 32)
            {
              is_sib7_rep_less_then_32 = TRUE;
            }
          }
          /* Apply the calculated bitmask for the byte till the max repetition cycle */
#ifdef FEATURE_WCDMA_HS_FACH_DRX
          if(rrcenhstate_camped_cell_supports_e_fach() &&
             rrcenhstate_camped_cell_supports_e_fach_drx())
          {
            for(loop_count = 0; loop_count<rep_count; loop_count++)
            {
              uint32 rep_byte_pos;
              rep_byte_pos = byte_pos + (loop_count * (sib_events_ptr->sib_sched[count].repitition_rate/8));
              /* Apply bit mask for all positions except for occurences in the range start_count to stop_count */
              
              /* if - roll over case. elseif - normal case */ 
              if((stop_count < start_count)&&(rep_byte_pos < RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE))
              {
                if((loop_count >= stop_count)&&(loop_count <= start_count))
                {
                  table_ptr[rep_byte_pos] |= bit_mask;
                }
              }
              else if(rep_byte_pos < RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE) 
              {
                if((loop_count <= start_count)||(loop_count >= stop_count))
                {
                  table_ptr[rep_byte_pos] |= bit_mask;
                }
              }
            }
          }
          else
#endif
          {
          for(loop_count = 0; loop_count < rep_count; loop_count++)
          {
            uint32 rep_byte_pos;
            if(is_sib7_rep_less_then_32 == TRUE)
            {
              /* Lets skipp intermidiate sib-7's, as Spec says SIB-7 have to be read at a minimum of 320msec
               * Its also controled by a calulation, which uses a parameter from sib-7. But since in our implimentation
               * S-BCCH is up all the time in FACH, So lets read SIB7 at a minmum of 320msec
               */
              rep_byte_pos = byte_pos + (loop_count * (32/8));
            }
            else
            {
              rep_byte_pos = byte_pos + (loop_count * (sib_events_ptr->sib_sched[count].repitition_rate/8));
            }
            
            if(rep_byte_pos < RRCSIB_SIB_SLEEP_SCHEDULE_TABLE_SIZE)
            {
              table_ptr[rep_byte_pos] |= bit_mask;
            }
          }
        }
      }
    }
    }

    /* Debug print */
    MSG_HIGH("SIB_Sleep Current table in use:%d table after update by SIB %d :Max repetition rate is %d",
              sib_sleep_schedule_table_in_use, rrcsib_map_int_sibtype_to_asn1_sibtype(rrc_sib_type),sib_events_ptr->scheduling_max_rept_rate);
#ifdef FEATURE_WCDMA_HS_FACH_DRX
    rrcsib_send_sib_sleep_table(sib_sleep_schedule_table_0);
#endif
  
    for(count = 0; count < (sib_events_ptr->scheduling_max_rept_rate/8);count=count+8)
    {
    WRRC_MSG8_HIGH("SIB_Sleep: Table Contents byte = %0X,%0X,%0X,%0X,%0X,%0X,%0X,%0X",
                   table_ptr[count],table_ptr[count+1],table_ptr[count+2],table_ptr[count+3],
                   table_ptr[count+4],table_ptr[count+5],table_ptr[count+6],table_ptr[count+7]);
    }
  }
}
#endif
 
/*===========================================================================

FUNCTION check_event_completion_status_for_cell_id_read

DESCRIPTION

  This function checks the status of the cell id read sib event
  in cell dch state of RRC, the sib event index is passed to this
  function. If all SIBs(i.e.SIB3 only) for the event have been received, 
  it will return TRUE. Else it will return FALSE.
    
DEPENDENCIES
 
  None.

RETURN VALUE

  TRUE is event has been completed, else FALSE.

SIDE EFFECTS

  None.
  
===========================================================================*/
boolean check_event_completion_status_for_cell_id_read
(
  uint32 index
)
{
  boolean status = FALSE;
  if (sib_events_ptr->event[index].event_active == TRUE)
  {
    if((sib_events_ptr->event[index].event_name == RRCSIB_GET_SPECIFIC_SIBS)&&
       (sib_events_ptr->event[index].req_proc == RRC_PROCEDURE_GPS)&&
       (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_CELL_ID_READ_IN_DCH_SCAN))
    {
      /*For sib_scan_type RRC_SIB_CELL_ID_READ_IN_DCH_SCAN, we need SIB3 only as this event 
       *is to read Cell I.D which comes in SIB3.Since we dont store any SIB's for this event, so check
       *tempholder to see if we have received SIB3, If yes then return TRUE since we completed task 
       *for this sibevent, else return FALSE
       */
      if((sib_events_ptr->event[index].tempholder.mib_decoded == TRUE)&&
         (sib_events_ptr->event[index].tempholder.sib3_decoded == TRUE)&&
         (sib_events_ptr->event[index].tsib[rrc_SIB1].sib_received == TRUE))
      {
        status = TRUE;
        WRRC_MSG1_HIGH("RRCGPS:CELL I.D read in DCH event completed.Scan type %d",sib_events_ptr->event[index].sib_scan_type);
      }
    }
  }
  return status;
}

/*===========================================================================
FUNCTION rrc_sib_is_current_event_sib_read_in_dch
DESCRIPTION
  This function will check whether the current event is sib reading in dch event 
  DEPENDENCIES
  None.
RETURN VALUE
  TRUE: If event is sib reading in DCH.
  FALSE: otherwise.
SIDE EFFECTS
  None.
===========================================================================*/
boolean rrc_sib_is_current_event_sib_read_in_dch(void)
{
  uint8 count;
  boolean status = FALSE;
  for( count = 0; count < (uint32)MAX_SIB_EVENTS_INDEX; count++ )
  {
    if( sib_events_ptr->event[count].event_active == TRUE )
    {
      if(sib_events_ptr->event[count].event_name == RRCSIB_GET_SPECIFIC_SIBS)
      {
        if((sib_events_ptr->event[count].req_proc == RRC_PROCEDURE_GPS) &&
           (sib_events_ptr->event[count].for_serving_cell == TRUE)&&
           (sib_events_ptr->event[count].sib_scan_type == RRC_SIB_CELL_ID_READ_IN_DCH_SCAN)
          )
        {
          WRRC_MSG0_HIGH("RRCGPS:Event I.D. in DCH active");
          status = TRUE;
          break;
        }
      }
    }/* End if active events */
  }
  return status;
}

/*===========================================================================
FUNCTION RRC_SIB_GET_PROC_ID_FOR_SIB_READ_IN_DCH
DESCRIPTION
  This function will return the proc id which has initiated sib reading in dch
  DEPENDENCIES
  None.
RETURN VALUE
  Proc id  If event is sib reading in DCH.
  PROC_NONE: otherwise.
SIDE EFFECTS
  None.
===========================================================================*/
rrc_proc_e_type rrc_sib_get_proc_id_for_sib_read_in_dch(void)
{
  uint8 count;
  rrc_proc_e_type proc_id = RRC_PROCEDURE_NONE;

  for(count = 0; count < (uint32)MAX_SIB_EVENTS_INDEX; count++)
  {
    if( sib_events_ptr->event[count].event_active == TRUE )
    {
      if((sib_events_ptr->event[count].event_name == RRCSIB_GET_SPECIFIC_SIBS) &&
         (sib_events_ptr->event[count].sib_scan_type == RRC_SIB_CELL_ID_READ_IN_DCH_SCAN))
      {
        WRRC_MSG1_HIGH("SIB reading in DCH is active. proc_id %d",sib_events_ptr->event[count].req_proc);
        proc_id = sib_events_ptr->event[count].req_proc;
        break;
      }
    }
  }

  return proc_id;
}

#ifdef FEATURE_UPDATE_SIB7_IN_FACH
/*===========================================================================
FUNCTION rrc_sib_can_s_bcch_torndown_in_fach_to_pch_transition
DESCRIPTION
  This function will return status on whether S-BCCH needs to be dropped at FACH to PCH transition
  DEPENDENCIES
  None.
RETURN VALUE
TRUE - Drop S-BCCH
FALSE - otherwise
SIDE EFFECTS
  None.
===========================================================================*/
boolean rrc_sib_can_s_bcch_torndown_in_fach_to_pch_transition
(
  void
)
{
  boolean status = FALSE;
  boolean is_update_sib_active = FALSE;
  uint8 count;
  /* Check if any event is active */
  for( count = 0; count < (uint8)MAX_SIB_EVENTS_INDEX; count++ )
  {
    if(( sib_events_ptr->event[count].event_active == TRUE )&&
        ( sib_events_ptr->event[count].event_name == RRCSIB_UPDATE_SIBS ))
    {
      WRRC_MSG1_HIGH("SIB event %d active",sib_events_ptr->event[count].event_name);
          is_update_sib_active =  TRUE;
      break;
    }/* End if active events */
  }
  
  if(is_update_sib_active == TRUE)
  {
    status = FALSE;
  }
  else
  {
    if(rrc_sib_is_current_event_sib7_in_fach() == TRUE)
    {
      sib7info.sib_timer_update_req = FALSE;
      sib7info.sib_ext_proc_update_req = FALSE;
      sib_events_ptr->srv_bcch.bcch_action = SIB_BCCH_NO_ACTION;
      rrcsib_force_sib_event_init(RRCSIB_ABORT_SIB7_UPDATE);
    }
    status = TRUE;
  }
  return status;
}
#endif
/*===========================================================================

FUNCTION rrcsib_is_event_update_sibs_active

DESCRIPTION

  This function will check whether the event is UPDATE_SIBs
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If this event is UPDATE_SIBs due to deferred SIB reading.
  FALSE: Other wise

SIDE EFFECTS
  None.
     
===========================================================================*/
boolean rrcsib_is_event_update_sibs_active
(
  void
)
{
  uint32  count; /* Counter used in loops */
  boolean status = FALSE;

  for( count = 0; count < (uint32)MAX_SIB_EVENTS_INDEX; count++ )
  {
    // Check if the event is stil active.
    if (sib_events_ptr->event[count].event_active == TRUE)
    {
      // Ensure that the SIB7 update was requested.
      if( sib_events_ptr->event[count].event_name == RRCSIB_UPDATE_SIBS)
      {
        //sib7 update is on-going
        WRRC_MSG0_HIGH("UPDATE_SIBS on-going");
        status = TRUE;
      }
    }
  }
  return status;
}

/*===========================================================================
FUNCTION rrcsib_map_asn1_sibtype_to_int_sibtype

DESCRIPTION

  This function maps the value used for an ASN1 sib type to the corresponding internal sib type.
  
DEPENDENCIES
  None

RETURN VALUE
  Value used for internal SIB type

SIDE EFFECTS
  None.

===========================================================================*/

rrc_SIB_type rrcsib_map_asn1_sibtype_to_int_sibtype
(
  rrc_SIB_Type asn1_sib_type
)
{
  rrc_SIB_type int_sib_type = rrc_MAX_SIB;

  switch(asn1_sib_type)
  {
    case RRC_SIB_TYPE(masterInformationBlock) :
      int_sib_type = rrc_MIB;
      break;
    case RRC_SIB_TYPE(systemInformationBlockType1) :
      int_sib_type = rrc_SIB1; 
      break;
    case RRC_SIB_TYPE(systemInformationBlockType2) :
      int_sib_type = rrc_SIB2;
      break;
    case RRC_SIB_TYPE(systemInformationBlockType3) :
      int_sib_type = rrc_SIB3;
      break;
    case RRC_SIB_TYPE(systemInformationBlockType4) :
      int_sib_type = rrc_SIB4;
      break;
    case RRC_SIB_TYPE(systemInformationBlockType5) :
      int_sib_type = rrc_SIB5;
      break;
    case RRC_SIB_TYPE(systemInformationBlockType6) :
      int_sib_type = rrc_SIB6;
      break;
    case RRC_SIB_TYPE(systemInformationBlockType7) :
      int_sib_type = rrc_SIB7;
      break;
    case RRC_SIB_TYPE(systemInformationBlockType11) :
      int_sib_type = rrc_SIB11;
      break;
    case RRC_SIB_TYPE(systemInformationBlockType12):
      int_sib_type = rrc_SIB12; 
      break;
    case RRC_SIB_TYPE(systemInformationBlockType18):
      int_sib_type = rrc_SIB18;
      break;
    case RRC_SIB_TYPE(schedulingBlock1) :
      int_sib_type = rrc_SB1;
      break;
    case RRC_SIB_TYPE(schedulingBlock2) :
      int_sib_type = rrc_SB2;
      break;
    case RRC_SIB_TYPE(systemInformationBlockType5bis) :
      int_sib_type = rrc_SIB5bis;
      break;
    case RRC_SIB_TYPE(extensionType):
      int_sib_type = rrc_ExtensionType;
      break;

    default:
      WRRC_MSG1_ERROR("Invalid SIB Type %d",asn1_sib_type);
      break;
  }
  return int_sib_type;
}

/*===========================================================================
FUNCTION rrcsib_map_asn1_sibtype_to_int_sibtype

DESCRIPTION

  This function maps the value used for an ASN1 sib type to the corresponding internal sib type.
  
DEPENDENCIES
  None

RETURN VALUE
  Value used for internal SIB type

SIDE EFFECTS
  None.

===========================================================================*/

rrc_SIB_Type rrcsib_map_int_sibtype_to_asn1_sibtype
(
  rrc_SIB_type int_sib_type
)
{
  rrc_SIB_Type asn1_sib_type = rrc_SIB_Type_dummy;

  switch(int_sib_type)
  {
    case rrc_MIB:
      asn1_sib_type = RRC_SIB_TYPE(masterInformationBlock);
      break;
    case rrc_SIB1 :
      asn1_sib_type = RRC_SIB_TYPE(systemInformationBlockType1); 
      break;
    case rrc_SIB2:
      asn1_sib_type = RRC_SIB_TYPE(systemInformationBlockType2);
      break;
    case rrc_SIB3 :
      asn1_sib_type = RRC_SIB_TYPE(systemInformationBlockType3);
      break;
    case rrc_SIB4 :
      asn1_sib_type = RRC_SIB_TYPE(systemInformationBlockType4);
      break;
    case rrc_SIB5 :
      asn1_sib_type = RRC_SIB_TYPE(systemInformationBlockType5);
      break;
    case rrc_SIB6 :
      asn1_sib_type = RRC_SIB_TYPE(systemInformationBlockType6);
      break;
    case rrc_SIB7 :
      asn1_sib_type = RRC_SIB_TYPE(systemInformationBlockType7);
      break;
    case rrc_SIB11 :
      asn1_sib_type = RRC_SIB_TYPE(systemInformationBlockType11);
      break;
    case rrc_SIB12:
      asn1_sib_type = RRC_SIB_TYPE(systemInformationBlockType12); 
      break;
    case rrc_SIB18:
      asn1_sib_type = RRC_SIB_TYPE(systemInformationBlockType18);
      break;
    case rrc_SB1 :
      asn1_sib_type = RRC_SIB_TYPE(schedulingBlock1);
      break;
    case rrc_SB2 :
      asn1_sib_type = RRC_SIB_TYPE(schedulingBlock2);
      break;
    case rrc_SIB5bis :
      asn1_sib_type = RRC_SIB_TYPE(systemInformationBlockType5bis);
      break;
    case rrc_SIB11bis:
      asn1_sib_type = RRC_SIB_TYPE(extensionType);
      break;
#ifdef FEATURE_WCDMA_TO_LTE
    case rrc_SIB19:
      asn1_sib_type = RRC_SIB_TYPE(extensionType);
      break;
#endif

    default:
      WRRC_MSG0_ERROR("Unsupported SIB");
      break;
  }
  return asn1_sib_type;
}

/*===========================================================================
FUNCTION rrcsib_decode_sib7_in_update_sibs

DESCRIPTION

  This function decodes and saves the sib7 
  received for UPDATE SIBS event.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void rrcsib_decode_sib7_in_update_sibs
(
  sib_events_index_e_type index   /* Index to the SIB events database
                                  in which SIBs have been completed. */
)
{
  boolean decode_succeeded = FALSE;

  if((sib_events_ptr->event[index].tsib[rrc_SIB7].sib_received == TRUE)&&
     (sib_events_ptr->event[index].tsib[rrc_SIB7].sib_tot_length != 0)
     )
  {
  
  /*New SIB7 has been received*/
    WRRC_MSG0_HIGH("Decoding SIB7 in UPDATE SIBS event");
    decode_succeeded = 
        rrcsib_decode_and_save_sib_for_current_event((void *)&(sibdb.cell[sibdb.active_cell_index].sibs.sib7),
                                            index, sibdb.active_cell_index, rrc_SIB7,
                                            rrc_SysInfoType7_PDU
                                            ,sibdb.cell[sibdb.active_cell_index].sibs.sib7_encoded_ptr
                                            ,&sibdb.cell[sibdb.active_cell_index].sibs.sib7_encoded_pdu_length
                                            );

/*Decode of SIB7 successful. Indicate the change in sibdb. Also, update LLC config pointers with SIB7 content */
  if( decode_succeeded == TRUE )
    {
      rrc_sibdb_save_active_cell_sib_change(rrc_SIB7);
      (void)rrcllc_update_configs_with_sib7_info_in_fach();
    }
  }

  return;
}
/*===========================================================================
FUNCTION rrcsib_initiate_cell_selection

DESCRIPTION

  This function sends initiate cell selection indication with cause as OOS, if current state
  is Disconnected/Connecting. If the current state is PCH/FACH, it sends CU initiate req with cause 
  as Reentered Service Area.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void rrcsib_initiate_cell_selection
(
void
)
{
  rrc_state_e_type rrc_state = rrc_get_state();
  rrc_cmd_type *int_cmd_ptr = NULL;

  switch(rrc_state)
  {
    case RRC_STATE_CELL_PCH:
    case RRC_STATE_URA_PCH:
    case RRC_STATE_CELL_FACH:
      /* Trigger Out Of Service area */
      if( (int_cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
      { 
        int_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_UPDATE_INITIATE_REQ;
        int_cmd_ptr->cmd.initiate_cell_update.cause = RRC_REENTERED_SERVICE_AREA;
        int_cmd_ptr->cmd.initiate_cell_update.cu_complete_cnf_required = FALSE;
        int_cmd_ptr->cmd.initiate_cell_update.procedure = RRC_PROCEDURE_SIB;
        rrc_put_int_cmd(int_cmd_ptr); 
        WRRC_MSG1_HIGH("Sending Cell Update Initiate Req,cause Re-entered Service Area,rrc_state:%d",rrc_state);
      }
      break;

    case RRC_STATE_DISCONNECTED:
    case RRC_STATE_CONNECTING:
      /* Trigger Cell Selection */
      if( (int_cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
      { 
        int_cmd_ptr->cmd_hdr.cmd_id = RRC_INITIATE_CELL_SELECTION_IND;
        int_cmd_ptr->cmd.initiate_cell_sel_ind.cause = RRC_OUT_OF_SERVICE_AREA_IND;
        int_cmd_ptr->cmd.initiate_cell_sel_ind.proc = RRC_PROCEDURE_SIB;
        int_cmd_ptr->cmd.initiate_cell_sel_ind.dest_state = rrc_state;
        int_cmd_ptr->cmd.initiate_cell_sel_ind.dest_freq_present = FALSE;
        int_cmd_ptr->cmd.initiate_cell_sel_ind.dest_psc_present  = FALSE;
        rrc_put_int_cmd(int_cmd_ptr); 
        WRRC_MSG1_HIGH("Sending Initiate Cell Selection Ind to CSP,cause Out Of Service Ind.rrc_state:%d",rrc_state);
      }
      break;

    default:
      WRRC_MSG0_HIGH("Invalid state,no action taken on S-BCCH failure");
      break;
  }
  return ;
}
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING

/*===========================================================================
FUNCTION rrcsib_can_event_support_deferred_sib

DESCRIPTION

  This function checks whether the current event can support Deferred SIB reading. s
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean: True, if event can support Deferred SIB reading

SIDE EFFECTS
  None.

===========================================================================*/

boolean rrcsib_can_event_support_deferred_sib
(
  sib_events_index_e_type index
)
{
  boolean status =  FALSE;

  if((sib_events_ptr->event[index].event_name == RRCSIB_GET_SPECIFIC_SIBS)&&
     (sib_events_ptr->event[index].sib_scan_type == RRC_SIB_NORMAL)&&
     (sib_events_ptr->event[index].tsib[rrc_SIB11].sib_needed))
  {
    status = TRUE;
  }
  return status;
}

/*===========================================================================
FUNCTION rrcsib_can_nw_support_deferred_sib

DESCRIPTION

  This function checks whether Deferred SIB reading is supported by the UTRAN or not. This is done 
  by looking at the IE 'Deferred measurement reading support' present in SIB 3.
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean: True, if Deferred SIB reading is supported, else false

SIDE EFFECTS
  None.

===========================================================================*/
boolean rrcsib_can_nw_support_deferred_sib
(
  int32 dbindex
)
{
  boolean status =  FALSE;
  rrc_SysInfoType3 *sib3_ptr = NULL;

#ifdef FEATURE_3GPP_CSFB
  if((rrc_csfb_call_status == TRUE)&&
      (rrc_csfb_skip_sib11_opt_nv == TRUE))
  {
    WRRC_MSG0_HIGH("CSFB:Deferred SIB11 feature enabled for CSFB ");
    return TRUE;
  }
#endif

  if( sibdb.cell[dbindex].sib_valid[rrc_SIB3] )
  {
    sib3_ptr = rrc_sibdb_return_sib_for_db_index(rrc_SIB3,dbindex);
    if(sib3_ptr!=NULL)
    {
      if(DEFERRED_MEAS_SUPPORT(sib3_ptr))
      {
        WRRC_MSG1_HIGH("Deferred Measurement reading supported for dbindex %d",dbindex);
        status = TRUE;
      }
      else
      {
        WRRC_MSG1_HIGH("Deferred Measurement reading not supported for dbindex %d",dbindex);
      }
    }
  }
  return status;
}


/*===========================================================================

FUNCTION rrcsib_is_event_update_sibs_for_deferred_sib

DESCRIPTION

  This function will check whether the event is UPDATE_SIBs
  due to deferred SIB reading.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If this event is UPDATE_SIBs due to deferred SIB reading.
  FALSE: Other wise

SIDE EFFECTS
  None.

===========================================================================*/
boolean rrcsib_is_event_update_sibs_for_deferred_sib
(
  sib_events_index_e_type  event_index
)
{
  boolean status = FALSE;

  if(EVENT_INDEX_VALID(event_index) ==FALSE)
  {
    WRRC_MSG0_ERROR("Invalid SIB Event Index");
    return FALSE;
  }
  
  if(sib_events_ptr->event[event_index].event_name == RRCSIB_UPDATE_SIBS )
  {
    if(sibdb.active_cell_sibs_valid == TRUE)
    {
       if(sib_events_ptr->update_sibs_cause == RRCSIB_DEFERRED_SIB_READ)
       {
          WRRC_MSG0_HIGH("Deferred SIB:This event is Update SIBS due to Deferred SIB reading");
          status = TRUE;
       }
       else
       {
         WRRC_MSG0_HIGH("Deferred SIB:Event not Update SIBS due to Deferred SIB reading");
       }
    }
  }
  return status;
}
/*===========================================================================

FUNCTION  rrcsib_update_sib_for_deferred_sib

DESCRIPTION

  This function will post Update SIBS comand for deferred SIB reading.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
     
===========================================================================*/
void rrcsib_update_sib_for_deferred_sib
(
)
{
  int32 dbindex = sibdb.active_cell_index;
  rrcllc_oc_set_status_e_type oc_set_status ;
  if(rrcsib_cmd_store.pending_flag == TRUE)
  {
    WRRC_MSG0_HIGH("Deferred SIB: pending cmd flag true");
  }

  if(rrcsib_is_sib11_11bis_12_read_deferred(DEFERRED_UPDATE_SIB)==TRUE)
  {
    rrc_cmd_type *rrc_cmd_ptr;            /* Pointer to the RRC Command */
    rrc_MasterInformationBlock * mib_ptr;  /*Pointer to MIB*/
    {
      oc_set_status = rrcllc_get_ordered_config_status();
      if(oc_set_status != OC_SET_FOR_CELL_DCH)
      {
        /* Build RRC_UPDATE_SIBS_IND command to post to SIB */
        /* Allocates the buffer to RRC Internal command */
        if( (rrc_cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
        {     
          /* Fill in the command parameters for RRC_UPDATE_SIBS_IND */
          rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_UPDATE_SIBS_IND;

          if((mib_ptr= (rrc_MasterInformationBlock *)  rrc_sibdb_return_sib_for_srv_cell
             (rrc_MIB)) != NULL)
          {
            rrc_cmd_ptr->cmd.update_sibs_ind.mib_value_tag = mib_ptr->mib_ValueTag;
            rrc_cmd_ptr->cmd.update_sibs_ind.procedure = RRC_PROCEDURE_SIB;
            rrc_cmd_ptr->cmd.update_sibs_ind.real_sib_mod = TRUE;
            rrc_cmd_ptr->cmd.update_sibs_ind.cause = RRCSIB_DEFERRED_SIB_READ;
            WRRC_MSG0_HIGH("Deferred SIB:Posting RRC_UPDATE_SIBS_IND cmd ");
    
            rrc_put_int_cmd(rrc_cmd_ptr);   
          }
        } 
      }
    }
  }
  else
  {
    WRRC_MSG1_HIGH("Deferred SIB:SIB11 already read.Val tag %d", sibdb.cell[dbindex].sib_value_tag[rrc_SIB11]);
  }
}
/*===========================================================================
FUNCTION rrcsib_is_sib11_11bis_12_read_deferred

DESCRIPTION

  This function checks whether reading of SIB11/11bis/12 is deferred for the serving cell.
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean: True, if reading of SIB11/11bis/12 is deferred, else false

SIDE EFFECTS
  None.

===========================================================================*/
boolean rrcsib_is_sib11_11bis_12_read_deferred
(
  rrcsib_deferred_sib_type deferred_sib_type
)
{
  boolean status = FALSE;
  int32 dbindex = sibdb.active_cell_index;

  if(sibdb.active_cell_sibs_valid  == TRUE)
  {
    if(rrcsib_can_nw_support_deferred_sib(dbindex) == TRUE)
    {
      switch(deferred_sib_type) 
      {
        case DEFERRED_SIB11:
          if(sibdb.cell[dbindex].sib_value_tag[rrc_SIB11] == INVALID_MIB_SB_VALUE_TAG)
          {
            status = TRUE;
          }
          break;
        case DEFERRED_SIB12:
          if((rrcsib_check_if_sib12_present_for_dbindex(dbindex) == TRUE)&&
             (sibdb.cell[dbindex].sib_value_tag[rrc_SIB12]
                  == INVALID_MIB_SB_VALUE_TAG)
             )
          {
            status = TRUE;
          }
          break;
          
        case DEFERRED_SIB11_BIS:
          if((rrcsib_check_if_extension_sib_present(dbindex) == TRUE)&&
                  (sibdb.cell[dbindex].sib_value_tag[rrc_SIB11bis] == INVALID_MIB_SB_VALUE_TAG))
          {
            status = TRUE;
          }
          break;
  
        case DEFERRED_ANY_ALLOWED_SIB:
          if((sibdb.cell[dbindex].sib_value_tag[rrc_SIB11] == INVALID_MIB_SB_VALUE_TAG)||
             ((rrcsib_check_if_sib12_present_for_dbindex(dbindex) == TRUE)&&
             (sibdb.cell[dbindex].sib_value_tag[rrc_SIB12]
                  == INVALID_MIB_SB_VALUE_TAG))
            ||((rrcsib_check_if_extension_sib_present(dbindex) == TRUE)&&
                              (sibdb.cell[dbindex].sib_value_tag[rrc_SIB11bis] == INVALID_MIB_SB_VALUE_TAG))
           )
          {
            status = TRUE;
          }
          break;

        case DEFERRED_UPDATE_SIB:
          if((sibdb.cell[dbindex].sib_value_tag[rrc_SIB11] == INVALID_MIB_SB_VALUE_TAG)||
             ((rrcsib_check_if_sib12_present_for_dbindex(dbindex) == TRUE)&&
              (sibdb.cell[dbindex].sib_value_tag[rrc_SIB12]
                == INVALID_MIB_SB_VALUE_TAG))
             |((rrcsib_check_if_extension_sib_present(dbindex) == TRUE)&&
               (sibdb.cell[dbindex].sib_value_tag[rrc_SIB11bis] == INVALID_MIB_SB_VALUE_TAG))
#ifdef FEATURE_WCDMA_TO_LTE
           || ((rrcsib_check_if_sib19_present(dbindex) == TRUE)&&
               (sibdb.cell[dbindex].sib_value_tag[rrc_SIB19] == INVALID_MIB_SB_VALUE_TAG))
#endif
           || ((sibdb.cell[dbindex].sib_value_tag[rrc_SIB18] == INVALID_MIB_SB_VALUE_TAG)  &&
               ((rrcsib_is_sib_scheduled( rrc_SIB18,dbindex, FALSE,NULL))))
            )
          {
            status = TRUE;
          }
          break;
  
        default:
          WRRC_MSG0_ERROR("Deferred SIB:Unknown SIB type");
          break;
      }
    }
  }

  if(status == TRUE)
  {
    WRRC_MSG0_HIGH("Deferred SIB:SIBS deferred");
  }
  else
  {
    WRRC_MSG0_HIGH("Deferred SIB:SIBS not deferred.");
  }
  return status;
}



/*===========================================================================
FUNCTION rrcsib_is_dmcr_ie_needed

DESCRIPTION

  This function checks whether DMCR IE is needed for the OTA.
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean: 

SIDE EFFECTS
  None.

===========================================================================*/

boolean rrcsib_is_dmcr_ie_needed()
{

  rrc_SysInfoType3 *sib3_ptr = (rrc_SysInfoType3*)rrc_sibdb_return_sib_for_srv_cell(rrc_SIB3);
  if(sib3_ptr == NULL)
  {
    MSG_ERROR("SIB3 is NULL do not add DMCR IE",0,0,0);
  }
  else if(rrcsib_is_sib11_11bis_12_read_deferred(DEFERRED_ANY_ALLOWED_SIB) &&
             (DEFERRED_MEAS_SUPPORT(sib3_ptr)))
  {
     return TRUE;
  }
  return FALSE;
}


/*===========================================================================

FUNCTION  rrcsib_is_sib12_present_in_serving_cell

DESCRIPTION

  This function will check if SIB12 is present for the given SIB DB index.

DEPENDENCIES
  None.

RETURN VALUE
  True: if present
  False: else

SIDE EFFECTS
  None.
     
===========================================================================*/

boolean rrcsib_check_if_sib12_present_for_dbindex
(
  int32 dbindex
)
{
  if((rrcsib_is_sib_scheduled( rrc_SIB12,
                                                       dbindex
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
                                                       , FALSE
                                                       ,NULL
#endif
                                                       )) == TRUE)
  {
    if( sibdb.cell[dbindex].sib_valid[rrc_SIB11] == TRUE)
    {
      if(rrcsib_check_if_connected_mode_sibs_present(sibdb.cell[dbindex].sibs.sib11_encoded_ptr[0],
          rrc_SIB11) == TRUE )
      {
        return TRUE;
      }
      else
      {
       return FALSE;
      }
    }
    else
    {
       WRRC_MSG0_HIGH("Deferred SIB: SIB11 not read");
       return TRUE;
    }
  }
  else
  {
    WRRC_MSG0_HIGH("SIB 12 is not scheduled in MIB or SB");
    return FALSE;
  }
}
/*===========================================================================

FUNCTION  rrcsib_post_process_deferred_sib

DESCRIPTION

  This function will perform all the required processing needed once SIBs have been read and the SIB event is to be 
  completed. 

DEPENDENCIES
  None.

RETURN VALUE
  True: if present
  False: else

SIDE EFFECTS
  None.
     
===========================================================================*/

void rrcsib_post_process_deferred_sib(sib_events_index_e_type index,   uint32 dbindex)
{
  if((sib_events_ptr->event[index].tsib[rrc_SIB11].sib_needed)&&
     !(sib_events_ptr->event[index].tsib[rrc_SIB11].sib_decoded))
  {
    WRRC_MSG0_HIGH("Deferred SIB: SIB11 not read");
    sibdb.cell[dbindex].sib_value_tag[rrc_SIB11] = INVALID_MIB_SB_VALUE_TAG;
    if(sib_events_ptr->event[index].tsib[rrc_SIB11].sib_data_ptr != NULL)
    {
      deferred_sib_info.dsib.sib11.seg_index = 
        sib_events_ptr->event[index].tsib[rrc_SIB11].seg_index;

      deferred_sib_info.dsib.sib11.sib_tot_length = 
        sib_events_ptr->event[index].tsib[rrc_SIB11].sib_tot_length;

      b_copy( sib_events_ptr->event[index].tsib[rrc_SIB11].sib_data_ptr,  /* Source Pointer */
              0,    /* Source Offset is always 0 since we copy all the received bits */
             deferred_sib_info.dsib.sib11.sib_data_ptr, /* Destination Pointer */
             sib_events_ptr->event[index].tsib[rrc_SIB11].sib_tot_length,  /* Destination Offset */
             FIXED_SIB_DATA_LENGTH_BITS  /* Length of the subsequent segment is fixed */
            );
    }
  }

  if((sib_events_ptr->event[index].tsib[rrc_SIB12].sib_needed)&&
      !(sib_events_ptr->event[index].tsib[rrc_SIB12].sib_decoded)
     )
  {
    if(rrcsib_check_if_sib12_present_for_dbindex(dbindex)==TRUE)
    {
      WRRC_MSG0_HIGH("Deferred SIB: SIB12 not read");
      sibdb.cell[dbindex].sib_value_tag[rrc_SIB12] = INVALID_MIB_SB_VALUE_TAG;
      if(sib_events_ptr->event[index].tsib[rrc_SIB12].sib_data_ptr != NULL)
      {
        deferred_sib_info.dsib.sib12.seg_index = 
          sib_events_ptr->event[index].tsib[rrc_SIB12].seg_index;
      
        deferred_sib_info.dsib.sib12.sib_tot_length = 
          sib_events_ptr->event[index].tsib[rrc_SIB12].sib_tot_length;

        b_copy( sib_events_ptr->event[index].tsib[rrc_SIB12].sib_data_ptr,  /* Source Pointer */
                0,    /* Source Offset is always 0 since we copy all the received bits */
               deferred_sib_info.dsib.sib12.sib_data_ptr, /* Destination Pointer */
               sib_events_ptr->event[index].tsib[rrc_SIB12].sib_tot_length,  /* Destination Offset */
               FIXED_SIB_DATA_LENGTH_BITS  /* Length of the subsequent segment is fixed */
              );
      }
    }
  }

  if((sib_events_ptr->event[index].tsib[rrc_SIB11bis].sib_needed )&&
     !(sib_events_ptr->event[index].tsib[rrc_SIB11bis].sib_decoded)
    )
  {
    if(rrcsib_check_if_extension_sib_present(dbindex) == TRUE)
    {
      WRRC_MSG0_HIGH("Deferred SIB: SIB11bis not read");
      sibdb.cell[dbindex].sib_value_tag[rrc_SIB11bis] = INVALID_MIB_SB_VALUE_TAG;
      if(sib_events_ptr->event[index].tsib[rrc_SIB11bis].sib_data_ptr != NULL)
      {
        deferred_sib_info.dsib.sib11bis.seg_index = 
          sib_events_ptr->event[index].tsib[rrc_SIB11bis].seg_index;
  
        deferred_sib_info.dsib.sib11bis.sib_tot_length = 
          sib_events_ptr->event[index].tsib[rrc_SIB11bis].sib_tot_length;
        
        b_copy( sib_events_ptr->event[index].tsib[rrc_SIB11bis].sib_data_ptr,  /* Source Pointer */
                        0,    /* Source Offset is always 0 since we copy all the received bits */
                deferred_sib_info.dsib.sib11bis.sib_data_ptr, /* Destination Pointer */
                sib_events_ptr->event[index].tsib[rrc_SIB11bis].sib_tot_length,  /* Destination Offset */
                FIXED_SIB_DATA_LENGTH_BITS  /* Length of the subsequent segment is fixed */
              );
      }
    }
  }

  /* When the SIB event is completed, SIBs which are deferred but read needs to be copied 
     from sib_events_ptr and stored in deferred_sib_info DB */
  if((sib_events_ptr->event[index].tsib[rrc_SIB18].sib_needed )&&
     !(sib_events_ptr->event[index].tsib[rrc_SIB18].sib_decoded)
    )
  {
    if((rrcsib_is_sib_scheduled( rrc_SIB18,dbindex, FALSE,NULL) == TRUE))
    {
      WRRC_MSG0_HIGH("Deferred SIB: SIB18 not read");
      sibdb.cell[dbindex].sib_value_tag[rrc_SIB18] = INVALID_MIB_SB_VALUE_TAG;
      if(sib_events_ptr->event[index].tsib[rrc_SIB18].sib_data_ptr != NULL)
      {
        deferred_sib_info.dsib.sib18.seg_index = 
          sib_events_ptr->event[index].tsib[rrc_SIB18].seg_index;
  
        deferred_sib_info.dsib.sib18.sib_tot_length = 
          sib_events_ptr->event[index].tsib[rrc_SIB18].sib_tot_length;
        
        b_copy( sib_events_ptr->event[index].tsib[rrc_SIB18].sib_data_ptr,  /* Source Pointer */
                        0,    /* Source Offset is always 0 since we copy all the received bits */
                deferred_sib_info.dsib.sib18.sib_data_ptr, /* Destination Pointer */
                sib_events_ptr->event[index].tsib[rrc_SIB18].sib_tot_length,  /* Destination Offset */
                FIXED_SIB_DATA_LENGTH_BITS  /* Length of the subsequent segment is fixed */
              );
      }
    }
  }

#ifdef FEATURE_WCDMA_TO_LTE
  if((sib_events_ptr->event[index].tsib[rrc_SIB19].sib_needed )&&
     !(sib_events_ptr->event[index].tsib[rrc_SIB19].sib_decoded)
    )
  {
    if(rrcsib_check_if_sib19_present(dbindex) == TRUE)
    {
      WRRC_MSG0_HIGH("Deferred SIB: SIB19 not read");
      sibdb.cell[dbindex].sib_value_tag[rrc_SIB19] = INVALID_MIB_SB_VALUE_TAG;
      if(sib_events_ptr->event[index].tsib[rrc_SIB19].sib_data_ptr != NULL)
      {
        deferred_sib_info.dsib.sib19.seg_index = 
          sib_events_ptr->event[index].tsib[rrc_SIB19].seg_index;
  
        deferred_sib_info.dsib.sib19.sib_tot_length = 
          sib_events_ptr->event[index].tsib[rrc_SIB19].sib_tot_length;
        
        b_copy( sib_events_ptr->event[index].tsib[rrc_SIB19].sib_data_ptr,  /* Source Pointer */
                        0,    /* Source Offset is always 0 since we copy all the received bits */
                deferred_sib_info.dsib.sib19.sib_data_ptr, /* Destination Pointer */
                sib_events_ptr->event[index].tsib[rrc_SIB19].sib_tot_length,  /* Destination Offset */
                FIXED_SIB_DATA_LENGTH_BITS  /* Length of the subsequent segment is fixed */
              );
      }
    }
  }

#endif

  if((sibdb.cell[dbindex].sib_value_tag[rrc_SIB11] == INVALID_MIB_SB_VALUE_TAG)||
     (sibdb.cell[dbindex].sib_value_tag[rrc_SIB12] == INVALID_MIB_SB_VALUE_TAG)||
     (sibdb.cell[dbindex].sib_value_tag[rrc_SIB11bis] == INVALID_MIB_SB_VALUE_TAG) ||
#ifdef FEATURE_WCDMA_TO_LTE
     (sibdb.cell[dbindex].sib_value_tag[rrc_SIB19] == INVALID_MIB_SB_VALUE_TAG) ||
#endif
     (sibdb.cell[dbindex].sib_value_tag[rrc_SIB18] == INVALID_MIB_SB_VALUE_TAG)
    )
  {
    rrcsib_store_sib_seg_in_deferred_sib_db();
    deferred_sib_info.cell_id = sibdb.cell[dbindex].cell_id;
    deferred_sib_info.plmn_id = sibdb.cell[dbindex].plmn_id;
  }

return;
}

#endif


#ifdef FEATURE_3GPP_CSFB
/*===========================================================================

FUNCTION  rrc_sibdb_invalidate_sib_v_tag_for_csfb

DESCRIPTION

  This function will invalidate the sibdb for requested sib mask

DEPENDENCIES
  None.

RETURN VALUE
  True: if present
  False: else

SIDE EFFECTS
  None.
     
===========================================================================*/
void rrc_sibdb_invalidate_sib_v_tag_for_csfb
(
  uint32  req_sib_bit_mask,          /* SIB's  that are required to be invalidated*/
  rrc_plmn_identity_type plmn,       /* The PLMN Id for the cell */ 
  rrc_int_cell_id_type cell          /* The cell id for the cell */
)
{
  uint32 count=0, dbindex=0;
  rrc_csp_sib_mask_type  gen_mask=MIB_BIT;
  rrc_SIB_type sib_type=rrc_MIB;

  if( (dbindex = rrc_sibdb_find_cell_index(plmn, cell)) == SIBDB_INVALID_INDEX )
  {
    WRRC_MSG0_HIGH("Match not found for given PLMN and cell id");
    return ;
  }

  if(!(SIBDB_VALID_INDEX(dbindex)))
  {
    WRRC_MSG0_ERROR("Invalid database index given");
    return;
  }
  if( sibdb.cell[dbindex].index_used == FALSE )
  {
    WRRC_MSG0_ERROR("Unused database index given");
    return ;
  }

  for(count=0; count<RRC_TOT_SIBS; count++, gen_mask <<= 1)
  {
    sib_type = rrc_MAX_SIB;
    
    switch (req_sib_bit_mask & gen_mask)
    {
      case MIB_BIT:
        sib_type = rrc_MIB;
        break;

      case SIB_1_BIT:
        sib_type = rrc_SIB1;
        break;
       
      case SIB_2_BIT:
        sib_type = rrc_SIB2;
        break;

      case SIB_3_BIT:
        sib_type = rrc_SIB3;
        break;

      case SIB_4_BIT:
        sib_type = rrc_SIB4;
        break;

      case SIB_5_BIT:
        sib_type = rrc_SIB5;
        break;

      case SIB_6_BIT:
        sib_type = rrc_SIB6;
        break;

      case SIB_7_BIT:
        sib_type = rrc_SIB7;
        break;

      case SIB_11_BIT:
        sib_type = rrc_SIB11;
        break;

      case SIB_12_BIT:
        sib_type = rrc_SIB12;
        break;

      case SIB_18_BIT:
        sib_type = rrc_SIB18;
        break;

      case SB_1_BIT:
        sib_type = rrc_SB1;
        break;

      case SB_2_BIT:
        sib_type = rrc_SB2;
        break;

      case SIB_5bis_BIT:
        sib_type = rrc_SIB5bis;
        break;

      case SIB_11BIS_BIT:
        sib_type = rrc_SIB11bis;
        break;

#ifdef FEATURE_WCDMA_TO_LTE
      case SIB_19_BIT:
        sib_type = rrc_SIB19;
        break;
#endif      

      default:
        sib_type = rrc_MAX_SIB;
        break;
    }

    if(sib_type != rrc_MAX_SIB)
    {
      WRRC_MSG1_HIGH("Invalidate %d SIB v-tag",sib_type);
      sib_events_ptr->sib_sched[sib_type].repitition_rate = INVALID_SIB_SCHEDULE_REP;
      sib_events_ptr->sib_sched[sib_type].num_segments = 0;
      sibdb.cell[dbindex].sib_value_tag[sib_type] = INVALID_MIB_SB_VALUE_TAG;
      sibdb.cell[dbindex].sib_valid[sib_type] = FALSE;
      sibdb.cell[dbindex].six_hr_count = MAX_CELL_INDEX_COUNT_SIXHR_CELL_SEL_RESEL;
    }
  }
}



/*===========================================================================

FUNCTION    rrc_log_sys_info_container 

DESCRIPTION
    This function will log rrc_System_Information_Container_PDU message into QXDM.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void rrc_log_sys_info_container
(
  rrc_cmd_type *cmd_ptr                 /* To hold received command */
)
{  
#ifndef T_WINNT
  uint32 log_record_length;                 /* Local variable to store Log 
                                               Record Length */
  WCDMA_SIGNALLING_MESSAGE_type *log_record_ptr;
                                            /* Local variable to store Log
                                               record */
  rrclog_sig_msg_event_type sig_msg_event;  /* Variable for message rx event */

#else
  #error code not present
#endif /* T_WINNT */ 
  
  uint32 dl_sdu_len = 0;
  uint8 rb_id     = BCCH_S_RADIO_BEARER_ID;
  uint8 msg_type  = RRCLOG_SIB_CONTAINER_COMPLETE_SIB;
  uint8 chan_type = RRCLOG_SIB_CONTAINER;

  //need 1 extra byte for signalling_message[0]

 dl_sdu_len = cmd_ptr->cmd.get_specific_sibs_req.sys_info_cont_ptr->cont_len + 1;

  WRRC_MSG0_HIGH("Logging rrc_System_Information_Container_PDU");

#ifdef T_WINNT
  #error code not present
#else  /* QXDM logging */

  /* Construct and send an event. */
  sig_msg_event.channel_type = chan_type;
  sig_msg_event.message_type = msg_type;
  event_report_payload(EVENT_RRC_MESSAGE_RECEIVED, sizeof(sig_msg_event),
                       (void *)&sig_msg_event);

  /*Calculate the record length */
  log_record_length = 
    FPOS(WCDMA_SIGNALLING_MESSAGE_type, signalling_message) + dl_sdu_len;

  /*Allocate memory for Log Record. */
  log_record_ptr = (WCDMA_SIGNALLING_MESSAGE_type *)log_alloc( WCDMA_SIGNALLING_MESSAGE, log_record_length);
  
  /* Make sure memory allocation is successful before to proceed with 
     logging */
  if(log_record_ptr != NULL)
  {
    /* Update RB Id.*/
    log_record_ptr->rb_id = rb_id;

    /*Update Logical Channel Type */
    log_record_ptr->channel_type = chan_type;

    /* Update length of the signalling message */
    log_record_ptr->length = dl_sdu_len;

    log_record_ptr->signalling_message[0] = msg_type;

    /* Copy the Signalling mesaage payload to the log record */
    WCDMA_MEMCPY((void *)&(log_record_ptr->signalling_message[1]),
                 (log_record_length - FPOS(WCDMA_SIGNALLING_MESSAGE_type, signalling_message) - 1),
                 (void *)(cmd_ptr->cmd.get_specific_sibs_req.sys_info_cont_ptr->cont_ptr),
                 cmd_ptr->cmd.get_specific_sibs_req.sys_info_cont_ptr->cont_len);

    /*commit the log record */
    log_commit(log_record_ptr);   
  }
  else
  {
    WRRC_MSG0_HIGH("log_record_ptr is NULL");
    /* The QXDM is not turn ON. Hence Do not log the OTA messages */
  }
#endif /*T_WINNT */
} /* end function rrc_log_dl_sig_message */

/*===========================================================================

FUNCTION  rrcsib_check_nw_support_deferred_sib

DESCRIPTION

  This function will check the network functionality for deferred SIB11

DEPENDENCIES
  None.

RETURN VALUE
  True: if present
  False: else

SIDE EFFECTS
  None.
     
===========================================================================*/
boolean rrcsib_check_nw_support_deferred_sib
(
  rrc_System_Information_Container  *local_sys_info_cont_ptr
)
{
#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
  rrc_SysInfoType3 *sib3_decoded = NULL;
#endif

  if((rrc_csfb_call_status == TRUE)&&
      (rrc_csfb_skip_sib11_opt_nv == TRUE))
  {
    WRRC_MSG0_HIGH("CSFB:Deferred SIB11 feature enabled for CSFB ");
    return TRUE;
  }

#ifdef FEATURE_WCDMA_DEFERRED_SIB11_12_READING
  sib3_decoded = &local_sys_info_cont_ptr->sysInfoType3;

  if(DEFERRED_MEAS_SUPPORT(sib3_decoded))
    return TRUE;
  else
#endif
    return FALSE;
}

/*===========================================================================

FUNCTION  rrcsib_read_system_info_container

DESCRIPTION

  This function will perform all the required processing needed once SIBs have been read and the SIB event is to be 
  completed. 

DEPENDENCIES
  None.

RETURN VALUE
  True: if present
  False: else

SIDE EFFECTS
  None.
     
===========================================================================*/
boolean rrcsib_validate_sys_info_container
(
  rrc_System_Information_Container *local_sys_info_cont_ptr   /* Pointer to received SIB event */
)
{
  boolean ret_val = TRUE;
  rrc_csfb_sib_container_event_type csfb_sib_event;


  /* Check to see if PLMN Type is not correct here. */
  if(!( (local_sys_info_cont_ptr->mib.plmn_Type.t == T_rrc_PLMN_Type_gsm_MAP)||
        (local_sys_info_cont_ptr->mib.plmn_Type.t == T_rrc_PLMN_Type_gsm_MAP_and_ANSI_41)
      )
    )
  {
    csfb_sib_event.sib_container_sts = RRC_SIB_CONTAINER_INCORRECT_PLMN_NOT_GSM_MAP;

    WRRC_MSG0_HIGH("CSFB: Acquired cell PLMN not GSM-MAP!");
    ret_val = FALSE;
  }

  if((rrccsp_check_sib1_validity(&local_sys_info_cont_ptr->sysInfoType1) == FALSE) &&
      (ret_val == TRUE))
  {
    csfb_sib_event.sib_container_sts = RRC_SIB_CONTAINER_INCORRECT_INVALID_SIB1;
    ret_val = FALSE;
  }

  event_report_payload(EVENT_WCDMA_RRC_CSFB_SIB_CONTAINER, sizeof(csfb_sib_event),
                       (void *)&csfb_sib_event);
  return ret_val;
}

/*===========================================================================

FUNCTION  rrcsib_read_system_info_container

DESCRIPTION

  This function will perform all the required processing needed once SIBs have been read and the SIB event is to be 
  completed. 

DEPENDENCIES
  None.

RETURN VALUE
  True: if present
  False: else

SIDE EFFECTS
  None.
     
===========================================================================*/
uint32 rrcsib_get_sibdb_index_for_sys_info_container
(
  rrc_System_Information_Container  *local_sys_info_cont_ptr,
  sib_events_index_e_type event_index
)
{
  rrc_plmn_identity_type plmn;
  rrc_int_cell_id_type cell;   
  uint32 sib_db_index = SIBDB_INVALID_INDEX;
  
  /* Initialize PLMN to dummy value to keep lint happy */
  plmn.mcc[0] = 0;
  plmn.mcc[1] = 0;
  plmn.mcc[2] = 0;
  plmn.num_mnc_digits = 3;
  plmn.mnc[0] = 0;
  plmn.mnc[1] = 0;
  plmn.mnc[2] = 0;

  /* Get the plmn id for this cell. Make sure the PLMN id is a GSM-MAP PLMN. 
     * Convert it to the internal format used by RRC. */
  if(local_sys_info_cont_ptr->mib.plmn_Type.t == T_rrc_PLMN_Type_gsm_MAP)
  {
    rrc_translate_gsm_map_plmn_id(
              &(local_sys_info_cont_ptr->mib.plmn_Type.u.gsm_MAP->plmn_Identity),
              &plmn
              );
  }
  else if(local_sys_info_cont_ptr->mib.plmn_Type.t == T_rrc_PLMN_Type_gsm_MAP_and_ANSI_41)
  {
    rrc_translate_gsm_map_plmn_id(
              &(local_sys_info_cont_ptr->mib.plmn_Type.u.gsm_MAP_and_ANSI_41->plmn_Identity),
              &plmn
              );
  }
  
  cell = rrc_translate_cell_id(&(local_sys_info_cont_ptr->sysInfoType3.cellIdentity));
  
  /* Get the SIB Database index for this cell */
  MSG_HIGH("CSFB: Checking SIBDB index CellId %d and MCC %d MNC %d", cell, 
        RRC_CSP_GET_INT_MCC(plmn), 
        RRC_CSP_GET_INT_MNC(plmn));
  
#ifdef FEATURE_FEMTO_CSG
  sib_db_index = rrc_sibdb_create_cell_index(plmn,cell,event_index,SYS_CSG_ID_INVALID,FALSE);
#else
  sib_db_index = rrc_sibdb_create_cell_index(plmn, cell, event_index);
#endif

  return sib_db_index;
}


/*===========================================================================

FUNCTION  rrcsib_read_sibs_from_sys_info_container

DESCRIPTION

  This function will read the SIBS from system information container

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.
     
===========================================================================*/
boolean rrcsib_read_sibs_from_sys_info_container
(
  rrc_cmd_type *cmd_ptr   /* Pointer to received SIB event */
)
{
  sib_events_index_e_type index=0;  
  uint32 sib_db_index=SIBDB_INVALID_INDEX;
  rrc_plmn_identity_type plmn;
  rrc_int_cell_id_type cell = 0;
  rrcsib_bcch_setup_status_e_type bcch_status;
  uint8 count = 0;
  rrc_csfb_sib_container_event_type csfb_sib_event;

  /* Initialize PLMN to dummy value to keep lint happy */
  plmn.mcc[0] = 0;
  plmn.mcc[1] = 0;
  plmn.mcc[2] = 0;
  plmn.num_mnc_digits = 3;
  plmn.mnc[0] = 0;
  plmn.mnc[1] = 0;
  plmn.mnc[2] = 0;

  /*Freeing decoded SIBs if GET_SPECIFIC_SIB_REQ for serving cell
   *is received without camping after successful SIB reading, in order
   *to avoid exhaustion of decoded contexts
   */

  if((cmd_ptr->cmd.get_specific_sibs_req.for_serv_cell == TRUE) && 
     (cmd_ptr->cmd.get_specific_sibs_req.procedure == RRC_PROCEDURE_CSP))
  {
    for(count = 0; count <(uint8)SIBDB_INVALID_INDEX; count++)
    {
      if(sibdb.cell[count].index_used == TRUE)
      {
        WRRC_MSG1_HIGH("Free ASN.1 decoded sib's for sibdb idx= %d", count);
        rrc_sibdb_free_all_decodedsibs_for_a_cell(count);
      }
    }
  }

  /* Freeing the decoded system info container when SIB event fails for a SIB container
     and we enter here for another SIB container */
  if(NULL != sibdb.rrc_decoded_sib_cont)
  {
    rrcasn1_free_pdu(rrc_System_Information_Container_PDU,(void *)sibdb.rrc_decoded_sib_cont);
    sibdb.rrc_decoded_sib_cont = NULL;

    /* Clearing the SIB DB index as the information inside as not used anymore */
    rrc_sibdb_clear_a_cell_index(sibdb.sib_cont_sibdb_index);
    sibdb.sib_cont_sibdb_index = SIBDB_INVALID_INDEX;
  }

  if(cmd_ptr->cmd.get_specific_sibs_req.for_serv_cell == TRUE)
    index = SIB_SRV_INDEX;
  else
  {
    WRRC_MSG0_ERROR("Not valid for neighor cells");
    cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type = RRC_SIB_NORMAL;
    return FALSE;
  }

  rrc_log_sys_info_container(cmd_ptr);

  /* Process System Information Container */
  sibdb.rrc_decoded_sib_cont = (rrc_System_Information_Container *)rrcasn1_decode_pdu( 
    (void *)cmd_ptr->cmd.get_specific_sibs_req.sys_info_cont_ptr->cont_ptr, 
    rrc_System_Information_Container_PDU,                      
    cmd_ptr->cmd.get_specific_sibs_req.sys_info_cont_ptr->cont_len);          

  if(sibdb.rrc_decoded_sib_cont == NULL)
  {
    WRRC_MSG0_ERROR("rrc_System_Information_Container_PDU decoding failed");
    cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type = RRC_SIB_NORMAL;

    csfb_sib_event.sib_container_sts = RRC_SIB_CONTAINER_INCORRECT_ASN_DECODE_FAILURE;
    event_report_payload(EVENT_WCDMA_RRC_CSFB_SIB_CONTAINER, sizeof(csfb_sib_event),
                       (void *)&csfb_sib_event);

    return FALSE;
  }
  
  /* Validate System Information Container for mandatory SIBS */
  if (FALSE == rrcsib_validate_sys_info_container(sibdb.rrc_decoded_sib_cont))
  {
    WRRC_MSG0_ERROR("Not valid SIB Container");    
    cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type = RRC_SIB_NORMAL;
    if(rrcasn1_free_pdu(rrc_System_Information_Container_PDU,(void *)sibdb.rrc_decoded_sib_cont))
    {
      ERR_FATAL("Unable to free the rrc_System_Information_Container_PDU",0,0,0);
    }

    sibdb.rrc_decoded_sib_cont = NULL;
    return FALSE;    
  }

  /* If MIB and SIB3 decoding is successful, update SIBDB 
     * Get the SIBDB index and update MIB and SIB3 information in SIBDB */
  sib_db_index = rrcsib_get_sibdb_index_for_sys_info_container(sibdb.rrc_decoded_sib_cont, index);

  if(sib_db_index == SIBDB_INVALID_INDEX)
  {
    WRRC_MSG0_HIGH("Invalid SIBDB index");
    cmd_ptr->cmd.get_specific_sibs_req.sib_scan_type = RRC_SIB_NORMAL;

    if(rrcasn1_free_pdu(rrc_System_Information_Container_PDU,(void *)sibdb.rrc_decoded_sib_cont))
    {
      ERR_FATAL("Unable to free the decoded PDU",0,0,0);
    }

    sibdb.rrc_decoded_sib_cont = NULL;

    csfb_sib_event.sib_container_sts = RRC_SIB_CONTAINER_INCORRECT_SIBDB_INDEX_NOT_FOUND;
    event_report_payload(EVENT_WCDMA_RRC_CSFB_SIB_CONTAINER, sizeof(csfb_sib_event),
                       (void *)&csfb_sib_event);

    return FALSE;
  }

  sib_events_ptr->event[index].for_serving_cell = TRUE;
  sib_events_ptr->event[index].sib_scan_type = RRC_SIB_READ_FROM_SIB_CONTAINER;
  sib_events_ptr->event[index].event_name = RRCSIB_GET_SPECIFIC_SIBS;
  sib_events_ptr->event[index].req_proc = RRC_PROCEDURE_CSP;
  sib_events_ptr->event[index].sib_db_index_found = TRUE;
  sib_events_ptr->event[index].sib_db_index = sib_db_index;

  /* Storing the SIB DB index of the stored SIB container so that this can be cleared upon next new SIB event */
  sibdb.sib_cont_sibdb_index = sib_db_index;

  /* Freeing the decoded SIB pointers in SIB DB index and invalidating it so that
     decoded info from SIB container can be used */
  rrcsib_update_sibdb_csfb_sib_cont(sib_db_index,index,sibdb.rrc_decoded_sib_cont);
  
  /* Get the plmn id for this cell. Make sure the PLMN id is a GSM-MAP PLMN. 
     * Convert it to the internal format used by RRC. */
  if(sibdb.rrc_decoded_sib_cont->mib.plmn_Type.t == T_rrc_PLMN_Type_gsm_MAP)
  {
    rrc_translate_gsm_map_plmn_id(
              &(sibdb.rrc_decoded_sib_cont->mib.plmn_Type.u.gsm_MAP->plmn_Identity),
              &plmn
              );
  }
  else if(sibdb.rrc_decoded_sib_cont->mib.plmn_Type.t == T_rrc_PLMN_Type_gsm_MAP_and_ANSI_41)
  {
    rrc_translate_gsm_map_plmn_id(
              &(sibdb.rrc_decoded_sib_cont->mib.plmn_Type.u.gsm_MAP_and_ANSI_41->plmn_Identity),
              &plmn
              );
  }
  
  cell = rrc_translate_cell_id(&(sibdb.rrc_decoded_sib_cont->sysInfoType3.cellIdentity));

  sibdb.cell[sib_db_index].freq = cmd_ptr->cmd.get_specific_sibs_req.freq;
  sibdb.cell[sib_db_index].scr_code = cmd_ptr->cmd.get_specific_sibs_req.scrambling_code;
  sibdb.cell[sib_db_index].plmn_id= plmn;
  sibdb.cell[sib_db_index].cell_id = cell;

  /* Get the SIB Database index for this cell */
  MSG_HIGH("CSFB: Checking SIBDB index CellId %d and MCC %d MNC %d", cell, 
        RRC_CSP_GET_INT_MCC(plmn), 
        RRC_CSP_GET_INT_MNC(plmn));

#ifdef FEATURE_WCDMA_CSFB_CALL_OPT
  /* Skip BCH setup if CSFB call is progress */
  if(!(WCDMA_RRC_IDLE_FEATURES(RRC_CSFB_CALL_TIMELINE_OPT) && rrc_csfb_call_status))
#endif
  {
    /* First setup the BCCH channel required for this command.
       This function also checks if there is an existing SIB
       event and if so resolves the conflict between the events. */
    bcch_status = sib_bcch_setup_req( cmd_ptr );

    //Mark event active here a sib_bcch_setup_req will need event_active to be FALSE
    sib_events_ptr->event[index].event_active = TRUE;


    if( bcch_status == RRCSIB_BCCH_SETUP_FAILED )
    {
      WRRC_MSG0_HIGH("Sib event dropped: BCCH failure");
      rrc_sib_post_complete_for_csfb_sib_read(SIB_EVENT_FAILED_LOWER_LAYERS);
      return FALSE;
    }

    if( bcch_status == RRCSIB_BCCH_SETUP_DONE )
    {
      sib_events_ptr->srv_bcch.bcch_action = SIB_BCCH_SETUP_PENDING;
    }

    /* Get the SIB Database index for this cell */
    WRRC_MSG1_HIGH("CSFB: bcch_status = %d", bcch_status);
  }
#ifdef FEATURE_WCDMA_CSFB_CALL_OPT
  /* Update sib_events_ptr so that GET_SPECIFIC_SIBS_CNF will be filled with correct freq and scr */
  else
  {
    sib_events_ptr->srv_bcch.freq = cmd_ptr->cmd.get_specific_sibs_req.freq;
    sib_events_ptr->srv_bcch.scr_code = cmd_ptr->cmd.get_specific_sibs_req.scrambling_code;      
  }  
#endif  

  return TRUE;
}

/*===========================================================================

FUNCTION  rrcsib_update_sibdb_csfb_sib_cont

DESCRIPTION

  This function will update SIBDB related info necessary for SIB container durin CSFB
  for for all SIBs. This function frees the decoded and encoded SIB DB contents and also 
  invalidate the necessary parameters so that info from SIB container can be used
  

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
     
===========================================================================*/
void rrcsib_update_sibdb_csfb_sib_cont
(
  uint32 sib_db_index,
  sib_events_index_e_type index,
  rrc_System_Information_Container  *rrc_dec_sys_info_container
)
{
  uint8 v_tag_idx = 0;

  WRRC_MSG0_HIGH("CSFB: Set V-Tag for all SIB's as invalid in SIB DB"); 

  /* Invalidating the value tags of all SIBs in SIB DB */
  for(v_tag_idx = 0;v_tag_idx<RRC_TOT_SIBS;v_tag_idx++)
  {
    sibdb.cell[sib_db_index].sib_value_tag[v_tag_idx] = INVALID_MIB_SB_VALUE_TAG;
  }

  /* Update MIB in SIBDB */
  /* This is very important as the required info from decoded SIB container will be used 
     only if this is set to INVALID_PDU_LENGTH */
  /* Dont touch the variable "sib_valid", as its used when querying SIB ptr from SIB DB */
  memset(sibdb.cell[sib_db_index].sibs.mib_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));     
  sibdb.cell[sib_db_index].sibs.mib_encoded_pdu_length = INVALID_PDU_LENGTH;
  if(sibdb.cell[sib_db_index].sibs.mib != NULL)
  {
    rrc_sibdb_free_sib(rrc_MasterInformationBlock_PDU, sibdb.cell[sib_db_index].sibs.mib );
  }
  sibdb.cell[sib_db_index].sib_valid[rrc_MIB] = TRUE;

  /* SIB1 */  
  memset(sibdb.cell[sib_db_index].sibs.sib1_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
  sibdb.cell[sib_db_index].sibs.sib1_encoded_pdu_length = INVALID_PDU_LENGTH;
  if(sibdb.cell[sib_db_index].sibs.sib1 != NULL)
  {
    rrc_sibdb_free_sib(rrc_SysInfoType1_PDU, sibdb.cell[sib_db_index].sibs.sib1 );
  }
  sibdb.cell[sib_db_index].sib_valid[rrc_SIB1] = TRUE;

  /*SIB3 */
  memset(sibdb.cell[sib_db_index].sibs.sib3_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
  sibdb.cell[sib_db_index].sibs.sib3_encoded_pdu_length = INVALID_PDU_LENGTH;  
  if(sibdb.cell[sib_db_index].sibs.sib3 != NULL)
  {
    rrc_sibdb_free_sib(rrc_SysInfoType3_PDU, sibdb.cell[sib_db_index].sibs.sib3 );
  }
  sibdb.cell[sib_db_index].sib_valid[rrc_SIB3] = TRUE;

  /* SIB5 */
  memset(sibdb.cell[sib_db_index].sibs.sib5_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
  sibdb.cell[sib_db_index].sibs.sib5_encoded_pdu_length = INVALID_PDU_LENGTH;
  if(sibdb.cell[sib_db_index].sibs.sib5 != NULL)
  {
    rrc_sibdb_free_sib(rrc_SysInfoType5bis_PDU, sibdb.cell[sib_db_index].sibs.sib5 );
  }
  sibdb.cell[sib_db_index].sib_valid[rrc_SIB5] = TRUE;

  /* SIB7 */  
  memset(sibdb.cell[sib_db_index].sibs.sib7_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
  sibdb.cell[sib_db_index].sibs.sib7_encoded_pdu_length = INVALID_PDU_LENGTH;
  if(sibdb.cell[sib_db_index].sibs.sib7 != NULL)
  {
    rrc_sibdb_free_sib(rrc_SysInfoType7_PDU, sibdb.cell[sib_db_index].sibs.sib7 );
  }
  sibdb.cell[sib_db_index].sib_valid[rrc_SIB7] = TRUE;
  

  /* SIB11 */
  if(rrc_dec_sys_info_container->m.sysInfoType11Present) 
  {
    memset(sibdb.cell[sib_db_index].sibs.sib11_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
    sibdb.cell[sib_db_index].sibs.sib11_encoded_pdu_length = INVALID_PDU_LENGTH;
    if(sibdb.cell[sib_db_index].sibs.sib11 != NULL)
    {
      rrc_sibdb_free_sib(rrc_SysInfoType11_PDU, sibdb.cell[sib_db_index].sibs.sib11);
    }
    sibdb.cell[sib_db_index].sib_valid[rrc_SIB11] = TRUE;
  } 

  /* SIB11bis */
  if(rrc_dec_sys_info_container->m.sysInfoType11bisPresent) 
  {
    memset(sibdb.cell[sib_db_index].sibs.sib11bis_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
    sibdb.cell[sib_db_index].sibs.sib11bis_encoded_pdu_length = INVALID_PDU_LENGTH;
    if(sibdb.cell[sib_db_index].sibs.sib11bis != NULL)
    {
      rrc_sibdb_free_sib(rrc_SysInfoType11bis_PDU, sibdb.cell[sib_db_index].sibs.sib11bis);
    }
    sibdb.cell[sib_db_index].sib_valid[rrc_SIB11bis] = TRUE;
  }

  /* SIB12 */
  if(rrc_dec_sys_info_container->m.sysInfoType12Present) 
  {
    memset(sibdb.cell[sib_db_index].sibs.sib12_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
    sibdb.cell[sib_db_index].sibs.sib12_encoded_pdu_length = INVALID_PDU_LENGTH;
    if(sibdb.cell[sib_db_index].sibs.sib12!= NULL)
    {
      rrc_sibdb_free_sib(rrc_SysInfoType12_PDU, sibdb.cell[sib_db_index].sibs.sib12);
    }
    sibdb.cell[sib_db_index].sib_valid[rrc_SIB12] = TRUE;
  }

  /* SB1 */
  if(rrc_dec_sys_info_container->m.sysInfoTypeSB1Present) 
  {
    memset(sibdb.cell[sib_db_index].sibs.sb1_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
    sibdb.cell[sib_db_index].sibs.sb1_encoded_pdu_length = INVALID_PDU_LENGTH;
    if(sibdb.cell[sib_db_index].sibs.sb1!= NULL)
    {
      rrc_sibdb_free_sib(rrc_SysInfoTypeSB1_PDU, sibdb.cell[sib_db_index].sibs.sb1);
    }
    sibdb.cell[sib_db_index].sib_valid[rrc_SB1] = TRUE;
  }

  /* SB2 */
  if(rrc_dec_sys_info_container->m.sysInfoTypeSB2Present) 
  {
    memset(sibdb.cell[sib_db_index].sibs.sb2_encoded_ptr, 0, MAX_SIB_LENGTH_BYTES*sizeof(byte));
    sibdb.cell[sib_db_index].sibs.sb2_encoded_pdu_length = INVALID_PDU_LENGTH;
    if(sibdb.cell[sib_db_index].sibs.sb2!= NULL)
    {
      rrc_sibdb_free_sib(rrc_SysInfoTypeSB2_PDU, sibdb.cell[sib_db_index].sibs.sb2);
    }
    sibdb.cell[sib_db_index].sib_valid[rrc_SB2] = TRUE;
  }
}
#endif

/*===========================================================================
FUNCTION    rrc_log_complete_sib 

DESCRIPTION
  This function will log rrc_System_Information_Block_PDU message into QXDM.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrc_log_complete_sib
(
  rrc_SIB_Type  sib_Type,
  rrc_SegmentIndex seg_index,
  uint32 sfn_prime
)
{
  rrclog_complete_sib_rcvd_event_type sib_rcvd_event;

  uint8 sib_type;
  rrc_SIB_type sib_int_type;

  switch(sib_Type)
  {
    case rrc_SIB_Type_masterInformationBlock:
      sib_type = MASTER_INFORMATION_BLOCK;
      break;
    case rrc_SIB_Type_systemInformationBlockType1:
      sib_type = SYSTEM_INFORMATION_BLOCK_TYPE1;
      break;
    case rrc_SIB_Type_systemInformationBlockType2:
      sib_type = SYSTEM_INFORMATION_BLOCK_TYPE2;
      break;
    case rrc_SIB_Type_systemInformationBlockType3:
      sib_type = SYSTEM_INFORMATION_BLOCK_TYPE3;
      break;
    case rrc_SIB_Type_systemInformationBlockType4:
      sib_type = SYSTEM_INFORMATION_BLOCK_TYPE4;
      break;
    case rrc_SIB_Type_systemInformationBlockType5:
      sib_type = SYSTEM_INFORMATION_BLOCK_TYPE5;
      break;
    case rrc_SIB_Type_systemInformationBlockType6:
      sib_type = SYSTEM_INFORMATION_BLOCK_TYPE6;
      break;
    case rrc_SIB_Type_systemInformationBlockType7:
      sib_type = SYSTEM_INFORMATION_BLOCK_TYPE7;
      break;
    case rrc_SIB_Type_systemInformationBlockType11:
      sib_type = SYSTEM_INFORMATION_BLOCK_TYPE11;
      break;
    case rrc_SIB_Type_systemInformationBlockType12:
      sib_type = SYSTEM_INFORMATION_BLOCK_TYPE12;
      break;
    case rrc_SIB_Type_systemInformationBlockType18:
      sib_type = SYSTEM_INFORMATION_BLOCK_TYPE18;
      break;
    case rrc_SIB_Type_schedulingBlock1:
      sib_type = SCHEDULING_BLOCK_TYPE1;
      break;
    case rrc_SIB_Type_schedulingBlock2:
      sib_type = SCHEDULING_BLOCK_TYPE2;
      break;
    case rrc_SIB_Type_systemInformationBlockType5bis:
      sib_type = SYSTEM_INFORMATION_BLOCK_TYPE5BIS;
      break;
#if defined(FEATURE_SIB11BIS_SUPPORT) || defined(FEATURE_WCDMA_SIB19)
    case rrc_SIB_Type_extensionType:
      sib_int_type = rrcsib_map_extension_sib_to_internal_sibtype(seg_index,sfn_prime);
#ifdef FEATURE_SIB11BIS_SUPPORT
      if(sib_int_type == rrc_SIB11bis)
      {
        sib_type = SYSTEM_INFORMATION_BLOCK_TYPE11BIS;
      }
      else 
#endif
#ifdef FEATURE_WCDMA_SIB19
      if(sib_int_type == rrc_SIB19)
      {
        sib_type = SYSTEM_INFORMATION_BLOCK_TYPE19;
      }
      else
#endif
      {
        sib_type = EXTENSION_SIB_TYPE_UNSUPPORTED;
      }
      break;
#endif
    default:
      sib_type = SIB_TYPE_UNSUPPORTED;
      WRRC_MSG0_ERROR("Unsupported SIB Type");
      break;
  }

  sib_rcvd_event.sib_type = sib_type;
  event_report_payload(EVENT_RRC_COMPLETE_SIB_RECEIVED, sizeof(sib_rcvd_event),
                       (void *)&sib_rcvd_event);
}

/*===========================================================================

FUNCTION rrcsib_send_sib7_ind

DESCRIPTION

  Sends SIB7 update status indication
  
DEPENDENCIES
 
  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.
     
===========================================================================*/
void rrcsib_send_sib7_ind()
{
  rrc_cmd_type *cnf_ptr;
  /* Set the value back to FASLE */
  sib7info.sib_ext_proc_update_req = FALSE;
  if( (cnf_ptr = rrc_get_int_cmd_buf())!=NULL )
  {
    WRRC_MSG0_HIGH("Sending RRC_SIB7_UPDATE_STATUS_IND");
    cnf_ptr->cmd_hdr.cmd_id = RRC_SIB7_UPDATE_STATUS_IND;
    cnf_ptr->cmd.sib7_update_status.status = SUCCESS;
    cnf_ptr->cmd.sib7_update_status.procedure = sib7info.req_proc;
    rrc_put_int_cmd( cnf_ptr );
  }
}
/*===========================================================================

FUNCTION rrcsib_update_sibs_active_for_bcch_mod

DESCRIPTION

  This function checks whether update sibs event if active for bcch mod
  
DEPENDENCIES
 
  None.

RETURN VALUE

  TRUE if active, otherwise FALSE.

SIDE EFFECTS

  None.
     
===========================================================================*/


boolean rrcsib_update_sibs_active_for_bcch_mod(void)
{
   if(rrcsib_is_event_update_sibs_active()
      && (sib_events_ptr->update_sibs_cause == RRCSIB_REAL_SIB_MOD)
     )
   {
      WRRC_MSG0_HIGH("Update SIBs event active with cause RRCSIB_REAL_SIB_MOD");
      return TRUE;
   }
   return FALSE;
}
#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION    RRCSIB_CHECK_IF_SIB_G_PAGE_COLLIDE 

DESCRIPTION
  This function will check if G paging occasion collides with SIB of max rep rate.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if collides, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean rrcsib_check_if_sib_g_page_collide(uint8 gap_duration)
{
  uint16 cur_sfn = 0xFFFF;
  uint16 no_of_sfns,relative_diff;
  uint8 count,seg_index;
  uint8 event_index = MAX_SIB_EVENTS_INDEX;


  /* If W is going to be stopped, return here*/
  if(RRCMCM_WAIT_STOP_CNF == rrcmcm_get_substate())
  {
    return TRUE;
  }
  
  if(sib_events_ptr->event[SIB_SRV_INDEX].event_active == TRUE)
  {
    event_index = SIB_SRV_INDEX;
  }
  else if(sib_events_ptr->event[SIB_NGH_INDEX].event_active == TRUE)
  {
    event_index = SIB_NGH_INDEX;
  }

  if(event_index == MAX_SIB_EVENTS_INDEX)
  {
    return TRUE;
  }

  /*If MIB is not yet received return TRUE*/
  if(sib_events_ptr->event[event_index].tsib[rrc_MIB].sib_received != TRUE)
  {
    return TRUE;
  }

  if(event_index == SIB_NGH_INDEX)
  {
    cur_sfn = dl_get_cell_timing((rrc_csp_int_data.new_cell_acq_entry.scr_code/16), 
      rrc_csp_int_data.new_cell_acq_entry.freq);
  }
  else
  {
    cur_sfn = dl_get_cell_timing((rrc_csp_int_data.curr_acq_entry.scr_code/16), 
      rrc_csp_int_data.curr_acq_entry.freq);
  }

  if (cur_sfn == 0xFFFF)
  {
    return TRUE;
  }

  /*gap_duration is not expected to be more than 60ms. If gap_duration is 30ms, we wrongly assume number of sfns that will be missing to be 2
       In real there is a chance of missing three sfns. 10 is added to take care of this*/
  no_of_sfns = (gap_duration + 10)/20;

  for(count = 1; count < RRC_TOT_SIBS; count++)
  {
    if((sib_events_ptr->event[event_index].tsib[count].sib_needed == TRUE) &&
       (sib_events_ptr->event[event_index].tsib[count].sib_received != TRUE)&&
       (sib_events_ptr->sib_sched[count].repitition_rate != INVALID_SIB_SCHEDULE_REP)&&
       (sib_events_ptr->sib_sched[count].repitition_rate == sib_events_ptr->scheduling_max_rept_rate))
    {
       for(seg_index = 0; seg_index < sib_events_ptr->sib_sched[count].num_segments; seg_index++)
       {
         relative_diff = rrcsib_check_if_next_sfn_expected(count,cur_sfn,seg_index);
         if(relative_diff < no_of_sfns)
         {
           return TRUE;
         }
       }
    }
  }

  return FALSE;
}

/*===========================================================================
FUNCTION    RRCSIB_INTRODUCE_HOLES_FOR_G_PAGE 

DESCRIPTION
  This function will check if G page collides with SIB of maximum rep rate,
  if not sends table with holes introduced
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rrcsib_introduce_holes_for_g_page(uint8 duration)
{

  uint16 curr_sfn;
  uint8 *table_ptr = NULL;
  uint32 i;
  uint8 byte_pos,byte_offset,mask;

  if(rrcsib_check_if_sib_g_page_collide(duration))
  {
    return;
  }

  curr_sfn = dl_get_cell_timing((rrc_csp_int_data.new_cell_acq_entry.scr_code/16), 
    rrc_csp_int_data.new_cell_acq_entry.freq);

  /*Copy the contents to SIB sleep that is already sent to the one that is being sent now*/
  if(sib_sleep_schedule_table_in_use == 0)
  {
    table_ptr = sib_sleep_schedule_table_1;
    WCDMA_MEMCPY(sib_sleep_schedule_table_1,sizeof(sib_sleep_schedule_table_1),sib_sleep_schedule_table_0,sizeof(sib_sleep_schedule_table_0));
    sib_sleep_schedule_table_in_use = 1;
  }
  else
  {
    table_ptr = sib_sleep_schedule_table_0;
    WCDMA_MEMCPY(sib_sleep_schedule_table_0,sizeof(sib_sleep_schedule_table_0),sib_sleep_schedule_table_1,sizeof(sib_sleep_schedule_table_1));
    sib_sleep_schedule_table_in_use = 0;
  }


  /*Following code will take care of setting zeroes from curr_sfn till curr_sfn + duration */

  for(i = curr_sfn; i < curr_sfn + duration/10; i++)
  {
    /*Apply the logic only for even numbered SFNs. generate bit function mask doesnt handle odd numbered SFNs*/
    if (i%2 == 0) 
    {
      byte_pos = i >> 3;
      byte_offset = i & 0x07;

      mask = ~ (rrcsib_gen_bit_mask_for_schedule_table(byte_offset));
      table_ptr[byte_pos] &= mask;
    }
  }

  rrcsib_send_sib_sleep_table(table_ptr);
}
#endif
